#include "pch.h"
#include "../Header/Malugaz.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "../Header/Player.h"
#include "../Header/Projectile.h"
#include "../Header/Fire.h"

CMalugaz::CMalugaz(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev), m_iPhase(1), m_iIdleCount(0), m_iTextureNum(0)
{
    m_eType = Engine::MON_MALUGAZ;
    m_fIdleY = 2.6f;
    m_eState = IDLE;

    m_bFlip = false;

    m_fAggroDistance = 100.f;
    m_fRange = 10.f;

    m_iAttackAnimProgress = 0;
    m_bCharging = false;
    m_iFrameCount = 0;

    m_fSpeedWeight = 2.f;

    m_fImmuneTimeLimit = 0.5f;

    m_bLightEnable = true;
    m_iLightNum = g_iLightNum++;

    m_iCurNumber = 0;
}

CMalugaz::~CMalugaz()
{
}

HRESULT CMalugaz::Ready_GameObject(_vec3 vPos)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_pTransformCom->Set_Pos(vPos.x, m_fIdleY, vPos.z);
    m_pTransformCom->Set_Scale(3.2f, 3.2f, 3.2f);
    m_pColliderCom->Set_Offset(_vec3(-0.25f, -0.5f, 0));
    m_pStateCom->Set_Stat(1000, 0, 25, 0);
    //m_vecDropItem.push_back(ITEM_STAFF);
    //m_vecDropItem.push_back(ITEM_WOOD);
    Set_Speed(6.0f);

    m_pHitParticleCom->init(L"../Bin/Resource/Texture/Effect/Hit_%d.png", 5, 2.0f);

    return S_OK;
}

_int CMalugaz::Update_GameObject(const _float& fTimeDelta)
{
    if (m_bStopDraw)
        return 0;

    Set_Cast();
    //Set_Light();

    if (m_eState != DEAD)
        m_eState = State_Change();
    switch (m_eState)
    {
    case IDLE:
        Pattern_Idle(fTimeDelta);
        break;
    case WALK:
        Pattern_Chase(fTimeDelta);
        break;
    case SWING:
        Pattern_Attack(fTimeDelta);
        break;
    case DEAD:
        Pattern_Dead();
        break;
    }

    if (m_eState != DEAD && m_bKnockBackEnd)
        Check_Hitted();

    if (m_bKnockBackStart)
        m_fImmuneTime += fTimeDelta;

    if (m_fImmuneTime > m_fImmuneTimeLimit)
    {
        m_fImmuneTime = 0.f;
        m_bKnockBackStart = false;
        m_bKnockBackEnd = true;
    }

    if (m_bHit)
    {
        m_pHitParticleCom->update(fTimeDelta);

        if (m_pHitParticleCom->isDead())
        {
            m_pHitParticleCom->reset();
            m_bHit = false;
        }
    }

    Flip();
    Set_StuckFree(fTimeDelta);
    m_pAnimatorCom->Update_Animation();
    Add_RenderGroup(RENDER_ALPHA, this);
    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CMalugaz::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CMalugaz::Render_GameObject()
{
    if (m_bStopDraw)
        return;

    //m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

    m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture(m_iTextureNum);
    m_pTransformCom->Apply_BillBoard();

    m_pBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());
    m_pBufferCom->Render_Buffer();
    m_pColliderCom->Render_Collider();

    if (m_bHit)
        m_pHitParticleCom->render();

    //m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CMalugaz::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    pComponent = m_pStateCom = dynamic_cast<CState*>(Engine::Clone_Proto(L"Proto_State"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_State", pComponent });

    pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

    pComponent = m_pAnimatorCom = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_Animator"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Animator", pComponent });

    pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_Malugaz1AnimTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pBufferCom2 = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_Malugaz2AnimTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer2", pComponent });

    pComponent = m_pBufferCom3 = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_Malugaz3AnimTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer3", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MalugazTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pColliderCom = dynamic_cast<CCollider*>(Engine::Clone_Proto(L"Proto_MalugazCollider"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    pComponent = m_pHitParticleCom = dynamic_cast<CHit*>(Engine::Clone_Proto(L"Proto_Hit"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Hit", pComponent });

    return S_OK;

}

void CMalugaz::Pattern_Idle(const _float& fTimeDelta)
{
    // 벽 확인 추가할 것

    // 특정 방향으로 가다가 멈추면 해당 방향의 IDLE 적용 : IDLE은 방향 변경 금지
    if (!m_bIdling)
    {
        m_bIdling = true;
        if (m_iDir)
        {
            m_fIdleTimeLimit = rand() % 3 + 1; // 1 ~ 3초
            m_iDir = 0;
        }
        else
        {
            m_fIdleTimeLimit = rand() % 4 + 2; // 2 ~ 5초
            m_iDir = rand() % 8 + 1;
        }
    }
    if (m_fIdleTime <= m_fIdleTimeLimit)
    {
        m_fIdleTime += fTimeDelta;

        // 이동을 없애기 위해 속도 값을 0으로 설정
        _float  fLookSpeed = 0, fRightSpeed = 0;

        if (m_iSpeedWeight == 0)
        {
            m_iDir = 0;
            m_bIdling = false;
            m_fIdleTime = 0.f;
        }
    }
    else
    {
        m_bIdling = false;
        m_fIdleTime = 0.f;
    }

    if (!g_bIsTopCamera)
        m_eDir = FRONT;

    // 이동 상태 애니메이션
    if (!m_iDir)
    {
        switch (m_eDir)
        {
        case FRONT:
            m_pAnimatorCom->Set_CurState(IDLE, 0, 5, 8);
            break;
        case RIGHT:
        case LEFT:
            m_pAnimatorCom->Set_CurState(IDLE, 8, 13, 8);
            break;
        case BACK:
            m_pAnimatorCom->Set_CurState(IDLE, 16, 21, 8);
            break;
        }
    }
    // 정지 상태 애니메이션
    else
    {
        switch (m_eDir)
        {
        case FRONT:
            m_pAnimatorCom->Set_CurState(IDLE, 0, 11, 8);
            break;
        case RIGHT:
        case LEFT:
            m_pAnimatorCom->Set_CurState(IDLE, 12, 23, 8);
            break;
        case BACK:
            m_pAnimatorCom->Set_CurState(IDLE, 24, 35, 8);
            break;
        }
    }
}


// 플레이어 방향으로 이동, 추후 A스타 알고리즘으로 변경
void CMalugaz::Pattern_Chase(const _float& fTimeDelta)
{
    if (m_iPhase == 1)
    {
        Pattern_Teleport(fTimeDelta);
    }
    else
    {
        Pattern_Run(fTimeDelta);
    }
}

void CMalugaz::Pattern_Attack(const _float& fTimeDelta)
{
    if (m_iPhase == 1)
    {
        int iRandom = rand();

        if(iRandom % 10 < 7)
            Pattern_Shoot(fTimeDelta);
        else
            Pattern_Generate(fTimeDelta);
    }
    else
    {
        Pattern_Punch(fTimeDelta);
    } 
}

// 죽어버리기
void CMalugaz::Pattern_Dead()
{
    if (m_iPhase == 1)
    {
        m_iPhase = 2;
        m_pStateCom->Set_Stat(1000, 0, 25, 0);
        m_pStateCom->Set_Revive();

        m_fImmuneTime = 0.f;
        m_bKnockBackStart = true;
        m_bKnockBackEnd = false;

        m_pBufferCom = m_pBufferCom2;
        m_iTextureNum = 1;
        m_pAnimatorCom->Set_CurState(IDLE, 0, 5, 8);
        m_eState = IDLE;
    }
    if (m_bKnockBackEnd)
    {
        m_pBufferCom = m_pBufferCom3;
        m_iTextureNum = 2;
        m_pAnimatorCom->Set_CurState(DEAD, 0, 10, 8);
        if (m_pAnimatorCom->Get_MotionEnd())
        {
            m_pGraphicDev->LightEnable(m_iLightNum, FALSE); // 조명 비활성화
            m_bLightEnable = false;
            m_bStopDraw = true;
            Drop_Item();
        }
    }
}

STATE CMalugaz::State_Change()
{
   /* m_iPatternNum %= 6;

    switch (m_iPatternNum)
    {
    case 1:
    case 6:
        return SWING;
    case 3:
        return WALK;
    default :
        return IDLE;
    }*/
    
    _vec3 vPlayerPos, vPos;
    CGameObject* pWeapon;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

    switch (m_eState)
    {
    case IDLE:
        m_iIdleCount++;
        if (m_iIdleCount <= 100)
            return IDLE;
        else
        {
            m_iIdleCount = 0;
        }
        pWeapon = m_pPlayer->Get_HandedItem();
        // 플레이어가 무기를 들고 공격하는 상태면 충돌 체크
        if (m_pPlayer->Get_CurState() == SWING)
        {
            CCollider* pWeaponCollider = dynamic_cast<Engine::CCollider*>(pWeapon->Get_Component(ID_DYNAMIC, L"Com_Collider"));
            if (m_pColliderCom->Check_Collision(pWeaponCollider))
            {
                // 무기와 충돌 했는데 공격 범위 이내인 경우
                if (m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, m_fRange))
                    return SWING;
                // 공격 범위 밖인 경우
                else
                    return WALK;
            }
        }
        // 플레이어가 어그로 범위 내에 들어올 경우(선공)
        if (m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, m_fRange))
            return SWING;
        else if (m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, m_fAggroDistance))
            return WALK;
        break;
    case WALK:
        // 쫓아가면서 사거리 계산
        if (m_iPhase == 1)
            return IDLE;
        else
        {
            if (m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, m_fRange * 0.1f))
                return SWING;
            else if(!m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, m_fAggroDistance))
                return WALK;    
        }
        
        break;
    case SWING:
        // 공격 모션이 끝났을 때
        if (m_bAttackSuccess)
        {
            if (m_iPhase == 1)
                return IDLE;
            else
            {
                return WALK;
            }
            // 공격 사거리 이내가 아닌 경우
            //if (!m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, m_fRange))
            //{
            //    m_iFrameCount = 0;
            //    m_iAttackAnimProgress = 0;
            //    return WALK;
            //}
            //if (m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, m_fAggroDistance))
            //    return IDLE;
        }
        break;
    }
    return m_eState;
}

void CMalugaz::Set_Direction(_vec3* vPlayerPos)
{
    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);

    // 몬스터 위치를 지나는 기울기가 1인 직선의 방정식에 플레이어의 위치 대입
    float diagLBtoRT = -vPlayerPos->x - vPlayerPos->z + (vPos.z + vPos.x);

    // 몬스터 위치를 지나는 기울기가 -1인 직선의 방정식에 플레이어의 위치 대입
    float diagLTtoRB = vPlayerPos->x - vPlayerPos->z + (vPos.z - vPos.x);

    // 플레이어가 상단
    if (diagLBtoRT <= 0 && diagLTtoRB <= 0)
        m_eDir = BACK;
    // 플레이어가 우측
    else if (diagLBtoRT < 0 && diagLTtoRB > 0)
        m_eDir = RIGHT;
    // 플레이어가 좌측
    else if (diagLBtoRT >= 0 && diagLTtoRB <= 0)
        m_eDir = LEFT;
    // 플레이어가 하단에 있을 경우
    else if (diagLBtoRT >= 0 && diagLTtoRB >= 0)
        m_eDir = FRONT;
}

void CMalugaz::Set_Light()
{
    D3DLIGHT9 light;
    ZeroMemory(&light, sizeof(D3DLIGHT9));

    light.Type = D3DLIGHT_POINT; // 포인트 조명
    light.Diffuse = { 1.f, 1.f, 1.f, 1.f }; // 확산 색상
    light.Specular = { 1.f, 1.f, 1.f, 1.f }; // 반사 색상
    light.Ambient = { 1.f, 1.f, 1.f, 1.f }; // 주변광

    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);

    light.Position = vPos; // 횃불의 위치
    light.Range = 3.0f; // 조명의 범위
    light.Falloff = 1.f; // 감쇠
    light.Attenuation0 = 1.0f; // 감쇠 계수
    light.Attenuation1 = 0.01f;
    light.Attenuation2 = 0.0f;

    m_pGraphicDev->SetLight(m_iLightNum, &light); // 조명 설정
    //if (m_bLightEnable)
    //    m_pGraphicDev->LightEnable(m_iLightNum, TRUE);
    //else
    //    m_pGraphicDev->LightEnable(m_iLightNum, FALSE);
}

void CMalugaz::Pattern_Shoot(const _float& fTimeDelta)
{
    m_bAttackSuccess = false;
    _vec3 vPlayerPos;
    m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

    // 플레이어 위치에 따른 방향(4방향) 계산
    if (g_bIsTopCamera)
        Set_Direction(&vPlayerPos);
    else
        m_eDir = FRONT;

    // 방향에 따른 애니메이션 설정 : 방향 바꼈다고 차징 끊기지 않도록 설정
    _int iFrame = 0, iFrameSpeed = 0;
    switch (m_eDir)
    {
    case FRONT:
        iFrame = 72 + m_iAttackAnimProgress;
        break;
    case RIGHT:
    case LEFT:
        iFrame = 84 + m_iAttackAnimProgress;
        break;
    case BACK:
        iFrame = 96 + m_iAttackAnimProgress;
        break;
    }

    CGameObject* pProjectile(nullptr);

    iFrameSpeed = 8;

    // 차징 시
    if (iFrame % 10 < 4)
    {
        if (m_bLightEnable)
        {
            m_bLightEnable = false;
            // 샤먼 차징 시 조명 끄고 실제 불덩이 생성하여 조명 적용
            CScene* pScene = Engine::Get_Scene();
            _vec3 vPos;
            m_pTransformCom->Get_Info(INFO_POS, &vPos);
            vPos.y -= 2.f;
            pProjectile = CProjectile::Create(m_pGraphicDev, vPos);
            NULL_CHECK(pProjectile);
            dynamic_cast<CProjectile*>(pProjectile)->Set_ChargeActive(false);
            dynamic_cast<CProjectile*>(pProjectile)->Set_ChargingTime(10);
            m_vecProjectileName.push_back(L"Monster_Created_Fireball" + std::to_wstring(m_iTagNumber++));
            FAILED_CHECK_RETURN(pScene->Create_GameObject(L"Layer_GameLogic", pProjectile, m_vecProjectileName.back().c_str()), );
        }
        iFrameSpeed = 6;
    }
    else
    {
        iFrameSpeed = 8;
    }


    if (m_iFrameCount++ > iFrameSpeed)
    {
        m_iFrameCount = 0;
        if (++m_iAttackAnimProgress >= 8)
        {
            if (!m_bLightEnable)
                m_bLightEnable = true;
            m_bAttackSuccess = true;
            m_iAttackAnimProgress = 0;
        }
        m_pAnimatorCom->Set_CurState(SWING, iFrame, iFrame, -1);
    }
}

void CMalugaz::Pattern_Teleport(const _float& fTimeDelta)
{
    _vec3 vPlayerPos;
    m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
    m_pTransformCom->Set_Pos(vPlayerPos.x, 2.6f, vPlayerPos.z);
}

void CMalugaz::Pattern_Generate(const _float& fTimeDelta)
{
    m_bAttackSuccess = false;

    _vec3 vPlayerPos;
    m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

    // 플레이어 주위 위치 10개 지정하기
    _vec3 vFirePos[10];
    float fRadius = 5.0f;  // 플레이어 주변의 거리(반경)를 설정
    float fMinRadius = 2.0f; // 최소 반경 설정

    for (int i = 0; i < 10; i++)
    {
        // 랜덤 반경(최소 반경과 최대 반경 사이의 값)
        float randomRadius = fMinRadius + static_cast<float>(rand()) / RAND_MAX * (fRadius - fMinRadius);

        // 랜덤 각도 (0 ~ 360도 사이)
        float randomAngle = static_cast<float>(rand()) / RAND_MAX * 360.0f;

        
        // 각도와 반경을 기반으로 X, Z 좌표 계산
        vFirePos[i].x = vPlayerPos.x + randomRadius * cosf(D3DXToRadian(randomAngle));
        vFirePos[i].z = vPlayerPos.z + randomRadius * sinf(D3DXToRadian(randomAngle));

        // Y 좌표는 플레이어의 Y 좌표와 동일하게 설정
        vFirePos[i].y = vPlayerPos.y;
    }

    // 방향에 따른 애니메이션 설정 : 방향 바꼈다고 차징 끊기지 않도록 설정
    _int iFrame = 0, iFrameSpeed = 0;
    switch (m_eDir)
    {
    case FRONT:
        iFrame = 36 + m_iAttackAnimProgress;
        break;
    case RIGHT:
    case LEFT:
        iFrame = 48 + m_iAttackAnimProgress;
        break;
    case BACK:
        iFrame = 60 + m_iAttackAnimProgress;
        break;
    }

    CGameObject* pFire(nullptr);

    iFrameSpeed = 8;

    // 차징 시
    if (iFrame % 7 < 2)
    {
        iFrameSpeed = 6;
    }
    else
    {
        if (m_bLightEnable)
        {
            m_bLightEnable = false;
            // 샤먼 차징 시 조명 끄고 실제 불덩이 생성하여 조명 적용
            CScene* pScene = Engine::Get_Scene();
            for (int i = 0; i < 10; i++)
            {
                pFire = CFire::Create(m_pGraphicDev, vFirePos[i]);
                NULL_CHECK(pFire);
                m_vecProjectileName.push_back(L"Monster_Created_Fireball" + std::to_wstring(m_iTagNumber++));
                FAILED_CHECK_RETURN(pScene->Create_GameObject(L"Layer_GameLogic", pFire, m_vecProjectileName.back().c_str()), );
            }
        }
        iFrameSpeed = 8;
    }


    if (m_iFrameCount++ > iFrameSpeed)
    {
        m_iFrameCount = 0;
        if (++m_iAttackAnimProgress >= 8)
        {
            if (!m_bLightEnable)
                m_bLightEnable = true;
            m_bAttackSuccess = true;
            m_iAttackAnimProgress = 0;
        }
        m_pAnimatorCom->Set_CurState(SWING, iFrame, iFrame, -1);
    }
}

void CMalugaz::Pattern_Run(const _float& fTimeDelta)
{
    _vec3		vPos, vPlayerPos, vDir;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
    vDir = vPlayerPos - vPos;
    vDir.y = 0.0f;
    D3DXVec3Normalize(&vDir, &vDir);

    // 플레이어 위치에 따른 방향(4방향) 계산
    if (g_bIsTopCamera)
        Set_Direction(&vPlayerPos);
    else
        m_eDir = FRONT;

    // 방향에 따른 애니메이션 설정
    switch (m_eDir)
    {
    case FRONT:
        m_pAnimatorCom->Set_CurState(WALK, 18, 23, 8);
        break;
    case RIGHT:
    case LEFT:
        m_pAnimatorCom->Set_CurState(WALK, 24, 29, 8);
        break;
    case BACK:
        m_pAnimatorCom->Set_CurState(WALK, 30, 35, 8);
        break;
    }
    Set_Stop(&vDir, m_fSpeed);
    m_pTransformCom->Move_Pos(&vDir, fTimeDelta, m_fSpeed * m_iSpeedWeight);
}

void CMalugaz::Pattern_Punch(const _float& fTimeDelta)
{
    m_bAttackSuccess = false;

    _vec3 vPos, vFirePos[9];
    m_pTransformCom->Get_Info(INFO_POS, &vPos);

    _float fOffset = 1.0f; // 각 불덩이 간의 거리
    int index = 0;

    for (int i = -1; i <= 1; ++i)  // z축 방향 (-1, 0, 1)
    {
        for (int j = -1; j <= 1; ++j)  // x축 방향 (-1, 0, 1)
        {
            vFirePos[index] = _vec3(vPos.x + j * fOffset, 0.8f, vPos.z + i * fOffset);  // x, z 좌표에 오프셋 추가
            ++index;
        }
    }

    // 방향에 따른 애니메이션 설정 : 방향 바꼈다고 차징 끊기지 않도록 설정
    _int iFrame = 0, iFrameSpeed = 0;
    switch (m_eDir)
    {
    case FRONT:
        iFrame = 36 + m_iAttackAnimProgress;
        break;
    case RIGHT:
    case LEFT:
        iFrame = 42 + m_iAttackAnimProgress;
        break;
    case BACK:
        iFrame = 48 + m_iAttackAnimProgress;
        break;
    }

    CGameObject* pFire(nullptr);

    iFrameSpeed = 8;

    // 차징 시
    if (iFrame % 6 < 2)
    {
        if (m_bLightEnable)
        {
            m_bLightEnable = false;
            // 샤먼 차징 시 조명 끄고 실제 불덩이 생성하여 조명 적용
            CScene* pScene = Engine::Get_Scene();
            for (int i = 0; i < 6; i++)
            {
                pFire = CFire::Create(m_pGraphicDev, vFirePos[i]);
                dynamic_cast<CFire*>(pFire)->Set_Burn();
                NULL_CHECK(pFire);
                m_vecProjectileName.push_back(L"Monster_Created_Fireball" + std::to_wstring(m_iTagNumber++));
                FAILED_CHECK_RETURN(pScene->Create_GameObject(L"Layer_GameLogic", pFire, m_vecProjectileName.back().c_str()), );
            }
        }
        iFrameSpeed = 9;
    }
    else
    {
        iFrameSpeed = 8;
    }


    if (m_iFrameCount++ > iFrameSpeed)
    {
        m_iFrameCount = 0;
        if (++m_iAttackAnimProgress >= 8)
        {
            if (!m_bLightEnable)
                m_bLightEnable = true;
            m_bAttackSuccess = true;
            m_iAttackAnimProgress = 0;
        }
        m_pAnimatorCom->Set_CurState(SWING, iFrame, iFrame, -1);
    }
}

CMalugaz* CMalugaz::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CMalugaz* pMalugaz = new CMalugaz(pGraphicDev);

    if (FAILED(pMalugaz->Ready_GameObject(vPos)))
    {
        Safe_Release(pMalugaz);
        MSG_BOX("Malugaz Create Failed");
        return nullptr;
    }
    return pMalugaz;
}

void CMalugaz::Free()
{
    Engine::CGameObject::Free();
}