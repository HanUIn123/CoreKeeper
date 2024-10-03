#include "pch.h"
#include "../Header/Shaman.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "../Header/Player.h"
#include "../Header/Projectile.h"

CShaman::CShaman(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev)
{
    m_eType = Engine::MON_SHAMAN;
    m_fIdleY = 0.8f;
    m_eState = IDLE;

    m_bFlip = false;

    m_fAggroDistance = 14.f;
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

CShaman::~CShaman()
{
}

HRESULT CShaman::Ready_GameObject(_vec3 vPos)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_pTransformCom->Set_Pos(vPos.x, m_fIdleY, vPos.z);
    m_pTransformCom->Set_Scale(1.2f, 1.2f, 1.2f);
    m_pColliderCom->Set_Offset(_vec3(-0.25f, 0, 0));
    m_pStateCom->Set_Stat(100, 0, 10, 0);
    m_vecDropItem.push_back(ITEM_STAFF);
    m_vecDropItem.push_back(ITEM_WOOD);
    Set_Speed(1.5f);
    return S_OK;
}

_int CShaman::Update_GameObject(const _float& fTimeDelta)
{
    if (m_bStopDraw)
        return 0;

    Set_Light();

    
    if (m_eState != DEAD && !Check_Wall())
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

    Flip();
    Set_StuckFree(fTimeDelta);
    m_pAnimatorCom->Update_Animation();
    Add_RenderGroup(RENDER_ALPHA, this);
    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CShaman::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CShaman::Render_GameObject()
{
    if (m_bStopDraw)
        return;
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

    m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture();

    m_pBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());
    m_pBufferCom->Render_Buffer();
    m_pColliderCom->Render_Collider();

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CShaman::Add_Component()
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

    pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_ShamanAnimTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ShamanTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pColliderCom = dynamic_cast<CCollider*>(Engine::Clone_Proto(L"Proto_ShamanCollider"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    return S_OK;

}

void CShaman::Pattern_Idle(const _float& fTimeDelta)
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
        _vec3	vLook, vRight;
        _float  fLookSpeed = 0, fRightSpeed = 0;
        m_pTransformCom->Get_Info(INFO_LOOK, &vLook);
        m_pTransformCom->Get_Info(INFO_RIGHT, &vRight);

        // 실제 이동
        switch (m_iDir)
        {
        case 0:
            // 정지
            break;
        case 1:
            // 상
            m_eDir = BACK;
            fLookSpeed = m_fSpeed;
            break;
        case 2:
            // 우상
            m_eDir = RIGHT;
            fLookSpeed = m_fDiagSpeed;
            fRightSpeed = m_fDiagSpeed;
            break;
        case 3:
            // 우
            m_eDir = RIGHT;
            fRightSpeed = m_fSpeed;
            break;
        case 4:
            // 우하
            m_eDir = RIGHT;
            fLookSpeed = -m_fDiagSpeed;
            fRightSpeed = m_fDiagSpeed;
            break;
        case 5:
            // 하
            m_eDir = FRONT;
            fLookSpeed = m_fSpeed;
            break;
        case 6:
            // 좌하
            m_eDir = LEFT;
            fLookSpeed = -m_fDiagSpeed;
            fRightSpeed = -m_fDiagSpeed;
            break;
        case 7:
            // 좌
            m_eDir = LEFT;
            fRightSpeed = -m_fSpeed;
            break;
        case 8:
            // 좌상
            m_eDir = LEFT;
            fLookSpeed = m_fDiagSpeed;
            fRightSpeed = -m_fDiagSpeed;
            break;
        }

        Set_Stop(&vLook, fLookSpeed, &vRight, fRightSpeed);
        if (m_iSpeedWeight == 0)
        {
            m_iDir = 0;
            m_bIdling = false;
            m_fIdleTime = 0.f;
        }
        else if (m_iDir)
        {
            _int iWeight = 1;
            if (m_eDir == LEFT)
                iWeight = -1;
            m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, fLookSpeed * m_iSpeedWeight);
            m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, fRightSpeed * iWeight * m_iSpeedWeight);
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
            m_pAnimatorCom->Set_CurState(IDLE, 24, 29, 8);
            break;
        case RIGHT:
        case LEFT:
            m_pAnimatorCom->Set_CurState(IDLE, 32, 37, 8);
            break;
        case BACK:
            m_pAnimatorCom->Set_CurState(IDLE, 40, 45, 8);
            break;
        }
    }

}

// 플레이어 방향으로 이동, 추후 A스타 알고리즘으로 변경
void CShaman::Pattern_Chase(const _float& fTimeDelta)
{
    Engine::CTransform* pPlayerTransform = dynamic_cast<Engine::CTransform*>
        (Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK(pPlayerTransform);

    _vec3		vPos, vPlayerPos, vDir;
    pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    vDir = vPlayerPos - vPos;
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
        m_pAnimatorCom->Set_CurState(WALK, 24, 29, 8);
        break;
    case RIGHT:
    case LEFT:
        m_pAnimatorCom->Set_CurState(WALK, 32, 37, 8);
        break;
    case BACK:
        m_pAnimatorCom->Set_CurState(WALK, 40, 45, 8);
        break;
    }
    Set_Stop(&vDir, m_fSpeed);
    m_pTransformCom->Move_Pos(&vDir, fTimeDelta, m_fSpeed * m_iSpeedWeight);
}

// 투사체 생성해서 날리기
void CShaman::Pattern_Attack(const _float& fTimeDelta)
{
    m_bAttackSuccess = false;
    Engine::CTransform* pPlayerTransform = dynamic_cast<Engine::CTransform*>
        (Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK(pPlayerTransform);

    _vec3 vPlayerPos;
    pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

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
        iFrame = 48 + m_iAttackAnimProgress;
        break;
    case RIGHT:
    case LEFT:
        iFrame = 56 + m_iAttackAnimProgress;
        break;
    case BACK:
        iFrame = 64 + m_iAttackAnimProgress;
        break;
    }

    // 차징 시
    if (iFrame % 8 < 3)
    {
        if (m_bLightEnable)
        {
            m_bLightEnable = false;
            // 샤먼 차징 시 조명 끄고 실제 불덩이 생성하여 조명 적용
            CScene* pScene = Engine::Get_Scene();
            _vec3 vPos;
            m_pTransformCom->Get_Info(INFO_POS, &vPos);
            vPos.y += 2.f;
            CGameObject* pProjectile = CProjectile::Create(m_pGraphicDev, vPos);
            NULL_CHECK(pProjectile);

            m_vecProjectileName.push_back(L"Monster_Created_Fireball" + std::to_wstring(m_iTagNumber++));
            FAILED_CHECK_RETURN(pScene->Create_GameObject(L"Layer_GameLogic", pProjectile, m_vecProjectileName.back().c_str()), );
        }
        iFrameSpeed = 45;
    }
    // 차징 종료 시
    else
        iFrameSpeed = 8;

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

// 죽어버리기
void CShaman::Pattern_Dead()
{
    if (m_bKnockBackEnd)
    {
        m_pAnimatorCom->Set_CurState(DEAD, 40, 44, 4);
        if (m_pAnimatorCom->Get_MotionEnd())
        {
            m_pGraphicDev->LightEnable(m_iLightNum, FALSE); // 조명 비활성화
            m_bLightEnable = false;
            m_bStopDraw = true;
            Drop_Item();
        }
    }
}

STATE CShaman::State_Change()
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player"));
    _vec3 vPlayerPos, vPos;
    CGameObject* pWeapon;
    dynamic_cast<CTransform*>(pPlayer->Get_Component(ID_DYNAMIC, L"Com_Transform"))->Get_Info(INFO_POS, &vPlayerPos);
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    switch (m_eState)
    {
    case IDLE:
        pWeapon = pPlayer->Get_HandedItem();
        // 플레이어가 무기를 들고 공격하는 상태면 충돌 체크
        if (pPlayer->Get_CurState() == SWING)
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
        if (m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, m_fRange))
            return SWING;
        if (!m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, m_fAggroDistance))
            return IDLE;
        break;
    case SWING:
        // 공격 모션이 끝났을 때
        if (m_bAttackSuccess)
        {
            // 공격 사거리 이내가 아닌 경우
            if (!m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, m_fRange))
            {
                m_iFrameCount = 0;
                m_iAttackAnimProgress = 0;
                return WALK;
            }
            if (!m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, m_fAggroDistance))
                return IDLE;
        }
        break;
    }
    return m_eState;
}

void CShaman::Set_Direction(_vec3* vPlayerPos)
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

void CShaman::Set_Light()
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
    m_pGraphicDev->LightEnable(m_iLightNum, TRUE);
}

CShaman* CShaman::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CShaman* pShaman = new CShaman(pGraphicDev);

    if (FAILED(pShaman->Ready_GameObject(vPos)))
    {
        Safe_Release(pShaman);
        MSG_BOX("Shaman Create Failed");
        return nullptr;
    }
    return pShaman;
}

void CShaman::Free()
{
    Engine::CGameObject::Free();
}