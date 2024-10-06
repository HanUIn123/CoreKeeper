#include "pch.h"
#include "..\Header\Hunter.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "../Header/Player.h"
#include "../Header/Bullet.h"

CHunter::CHunter(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev)
{
    m_eType = Engine::MON_HUNTER;
    m_fIdleY = 0.65f;
    m_eState = IDLE;
    m_eDir = FRONT;

    m_bFlip = false;

    m_fAggroDistance = 14.f;
    m_fRange = 12.f;

    m_iAttackAnimProgress = 0;
    m_bCharging = false;
    m_iFrameCount = 0;

    m_vecBulletName.reserve(16);
    m_iCurNumber = 0;

    m_fImmuneTimeLimit = 0.5f;

    m_eIdleType = HUNTER_BUSH;
    m_bIdleFirstFrame = true;
    m_iChaseType = 0;
    m_bChaseFirstFrame = true;
    m_iSwingType = 0;
    m_bIsReloaded = true;
    m_bSwingFirstFrame = true;
    m_bLeaveBeforeSwing = false;

    m_pHunterEye = nullptr;
}

CHunter::~CHunter()
{
}

HRESULT CHunter::Ready_GameObject(_vec3 vPos)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_vRespawnPoint = { vPos.x, m_fIdleY, vPos.z };
    m_pTransformCom->Set_Pos(m_vRespawnPoint.x, m_vRespawnPoint.y, m_vRespawnPoint.z);
    m_pTransformCom->Set_Scale(1.2f, 1.2f, 1.2f);
    m_pStateCom->Set_Stat(100, 0, 10, 0);
    m_vecDropItem.push_back(ITEM_SWORD);
    m_vecDropItem.push_back(ITEM_WOOD);
    Set_Speed(1.5f);

    m_pHitParticleCom->init(L"../Bin/Resource/Texture/Effect/Hit_%d.png", 5, 1.0f);

    return S_OK;
}

_int CHunter::Update_GameObject(const _float& fTimeDelta)
{
    if (m_bStopDraw)
        return 0;

    Set_Cast();
    m_pAnimatorCom->Update_Animation();

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

    Add_RenderGroup(RENDER_ALPHA, this);
    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CHunter::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CHunter::Render_GameObject()
{
    if (m_bStopDraw)
        return;

    _vec3		vPos, vPlayerPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

    if (!m_pCalculatorCom->Check_Distance2D(&vPos, &vPlayerPos, 40.f))
        return;

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

    m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture();

    m_pBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());
    m_pBufferCom->Render_Buffer();
    m_pColliderCom->Render_Collider();

    if (m_bHit)
        m_pHitParticleCom->render();

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CHunter::Add_Component()
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

    pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_HunterAnimTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_HunterTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pColliderCom = dynamic_cast<CCollider*>(Engine::Clone_Proto(L"Proto_HunterCollider"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    pComponent = m_pHitParticleCom = dynamic_cast<CHit*>(Engine::Clone_Proto(L"Proto_Hit"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Hit", pComponent });

    return S_OK;

}

void CHunter::Pattern_Idle(const _float& fTimeDelta)
{
    m_bChaseFirstFrame = true;
    m_bSwingFirstFrame = true;

    // 애니메이션 종료 혹은 정해진 시간이 끝났을 경우
    if (!m_bIdling)
    {
        m_bIdling = true;
        _int iIdleType = 0;

        // 이전 행동에 따라 다음 행동 설정
        switch(m_eIdleType)
        {
        // 은신 후 높은 확률로 은신(지속) OR 빼꼼 OR 은신 해제
        case HUNTER_BUSH:
            iIdleType = rand() % 7;
            if (iIdleType < 4)
                m_eIdleType = HUNTER_BUSH;
            else if (iIdleType < 6)
                m_eIdleType = HUNTER_PEAK;
            else
                m_eIdleType = HUNTER_LEAVEBUSH;
            break;
        // 빼꼼 후 은신
        case HUNTER_PEAK:
        // 은신 진입 후 은신
        case HUNTER_GOTOBUSH:
            m_eIdleType = HUNTER_BUSH;
            break;
        // 은신 해제 후 이동
        case HUNTER_LEAVEBUSH:
            m_eIdleType = HUNTER_MOVE;
            break;
        // 정지 후 높은 확률로 은신 OR 이동
        case HUNTER_IDLE:
            iIdleType = rand() % 4;
            if (iIdleType < 3)
                m_eIdleType = HUNTER_GOTOBUSH;
            else
                m_eIdleType = HUNTER_MOVE;
            break;
        // 이동 후 정지
        case HUNTER_MOVE:
            m_eIdleType = HUNTER_IDLE;
            break;
        default:
            break;
        }

        // 다음 행동에 따른 세부 사항 설정
        // 1. LEAVEBUSH, MOVE 시 방향 지정
        // 2. BUSH, MOVE, IDLE 시 IDLETIMELIMIT 설정
        switch (m_eIdleType)
        {
        case HUNTER_BUSH:
            m_fIdleTimeLimit = 2.f;
            break;
        case HUNTER_MOVE:
            m_fIdleTimeLimit = rand() % 2 + 1.5f;
            m_iDir = rand() % 8 + 1;
            break;
        case HUNTER_IDLE:
            m_fIdleTimeLimit = 1.f;
            m_iDir = 0;
            break;
        case HUNTER_LEAVEBUSH:
            m_iDir = rand() % 8 + 1;
            break;
        }
    }

    if (!g_bIsTopCamera)
        m_eDir = FRONT;


    if (m_bIdleFirstFrame)
    {
        m_bIdleFirstFrame = false;
        switch (m_pAnimatorCom->Get_PreState())
        {
        case WALK:
        case SWING:
            m_eIdleType = HUNTER_GOTOBUSH;
            break;
        default:
            break;
        }
    }
    

    // 현재 IDLE 상태 및 방향에 따른 애니메이션 설정
    switch (m_eIdleType)
    {
    case HUNTER_BUSH:
        m_pAnimatorCom->Set_CurState(IDLE, 0, 9, 8);
        break;
    case HUNTER_PEAK:
        m_pAnimatorCom->Set_CurState(IDLE, 16, 25, 8);
        break;
    case HUNTER_GOTOBUSH:
        switch (m_eDir)
        {
        case FRONT:
            m_pAnimatorCom->Set_CurState(IDLE, 32, 33, 12);
            break;
        case RIGHT:
        case LEFT:
            m_pAnimatorCom->Set_CurState(IDLE, 48, 49, 12);
            break;
        case BACK:
            m_pAnimatorCom->Set_CurState(IDLE, 64, 65, 12);
            break;
        }
        break;
    case HUNTER_LEAVEBUSH:
        switch (m_eDir)
        {
        case FRONT:
            m_pAnimatorCom->Set_CurState(IDLE, 80, 81, 12);
            break;
        case RIGHT:
        case LEFT:
            m_pAnimatorCom->Set_CurState(IDLE, 96, 97, 12);
            break;
        case BACK:
            m_pAnimatorCom->Set_CurState(IDLE, 112, 113, 12);
            break;
        }
        break;
    case HUNTER_IDLE:
        switch (m_eDir)
        {
        case FRONT:
            m_pAnimatorCom->Set_CurState(IDLE, 128, 132, 8);
            break;
        case RIGHT:
        case LEFT:
            m_pAnimatorCom->Set_CurState(IDLE, 144, 148, 8);
            break;
        case BACK:
            m_pAnimatorCom->Set_CurState(IDLE, 160, 164, 8);
            break;
        }
        break;
    case HUNTER_MOVE:
        switch (m_eDir)
        {
        case FRONT:
            m_pAnimatorCom->Set_CurState(IDLE, 176, 181, 8);
            break;
        case RIGHT:
        case LEFT:
            m_pAnimatorCom->Set_CurState(IDLE, 192, 197, 8);
            break;
        case BACK:
            m_pAnimatorCom->Set_CurState(IDLE, 208, 213, 8);
            break;
        }
        break;
    }

    // 현재 설정된 행동의 종료 시점 설정
    switch (m_eIdleType)
    {
    case HUNTER_BUSH:
    case HUNTER_IDLE:
        if (m_fIdleTime <= m_fIdleTimeLimit)
            m_fIdleTime += fTimeDelta;
        else
        {
            m_fIdleTime = 0.f;
            m_bIdling = false;
        }
        break;
    case HUNTER_PEAK:
    case HUNTER_GOTOBUSH:
    case HUNTER_LEAVEBUSH:
        if (m_pAnimatorCom->Get_MotionEnd())
            m_bIdling = false;
        break;
    case HUNTER_MOVE:
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
            break;
        }
        else
        {
            m_bIdling = false;
            m_fIdleTime = 0.f;
        }
    }
}

// 어그로 범위에 플레이어 진입 시 플레이어 방향으로 이동
void CHunter::Pattern_Chase(const _float& fTimeDelta)
{
    m_bIdleFirstFrame = true;
    m_bSwingFirstFrame = true;
    if (m_bChaseFirstFrame)
    {
        m_bChaseFirstFrame = false;
        switch (m_eIdleType)
        {
        // 은신 해제 설정
        case HUNTER_BUSH:
        case HUNTER_PEAK:
        case HUNTER_GOTOBUSH:
            m_iChaseType = 0;
            break;
        // 이동 설정
        case HUNTER_LEAVEBUSH:
        case HUNTER_IDLE:
        case HUNTER_MOVE:
            m_iChaseType = 1;
            break;
        }
    }

    _vec3		vPos, vPlayerPos, vDir;
    m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    vDir = vPlayerPos - vPos;
    D3DXVec3Normalize(&vDir, &vDir);

    // 플레이어 위치에 따른 방향(4방향) 계산
    if (g_bIsTopCamera)
        Set_Direction(&vPlayerPos);
    else
        m_eDir = FRONT;

    // 이동 전에 은신 해제
    if (!m_iChaseType)
    {
        switch (m_eDir)
        {
        case FRONT:
            m_pAnimatorCom->Set_CurState(WALK, 80, 81, 4);
            break;
        case RIGHT:
        case LEFT:
            m_pAnimatorCom->Set_CurState(WALK, 96, 97, 4);
            break;
        case BACK:
            m_pAnimatorCom->Set_CurState(WALK, 112, 113, 4);
            break;
        }
        // 은신 해제 다하면 추격 설정
        if (m_pAnimatorCom->Get_MotionEnd())
            m_iChaseType = 1;
    }
    // 은신 해제된 상태면 이동
    else
    {
        switch (m_eDir)
        {
        case FRONT:
            m_pAnimatorCom->Set_CurState(WALK, 176, 181, 8);
            break;
        case RIGHT:
        case LEFT:
            m_pAnimatorCom->Set_CurState(WALK, 192, 197, 8);
            break;
        case BACK:
            m_pAnimatorCom->Set_CurState(WALK, 208, 213, 8);
            break;
        }
        Set_Stop(&vDir, m_fSpeed);
        m_pTransformCom->Move_Pos(&vDir, fTimeDelta, m_fSpeed * m_iSpeedWeight);
    }
}

// 총알 생성해서 날린 후 장전
void CHunter::Pattern_Attack(const _float& fTimeDelta)
{
    m_bAttackSuccess = false;
    m_bChaseFirstFrame = true;
    m_bIdleFirstFrame = true;

    if (m_bSwingFirstFrame)
    {
        m_bSwingFirstFrame = false;
        if (m_pAnimatorCom->Get_PreState() == IDLE)
            if (m_eIdleType == HUNTER_BUSH || m_eIdleType == HUNTER_PEAK)
                m_bLeaveBeforeSwing = true;
    }

    if (m_bLeaveBeforeSwing)
    {
        switch (m_eDir)
        {
        case FRONT:
            m_pAnimatorCom->Set_CurState(IDLE, 80, 81, 12);
            break;
        case RIGHT:
        case LEFT:
            m_pAnimatorCom->Set_CurState(IDLE, 96, 97, 12);
            break;
        case BACK:
            m_pAnimatorCom->Set_CurState(IDLE, 112, 113, 12);
            break;
        }
        if (m_pAnimatorCom->Get_MotionEnd())
            m_bLeaveBeforeSwing = false;
    }
    else
    {
        // 플레이어 위치에 따른 방향(4방향) 계산
        _vec3 vPlayerPos;
        m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
        if (g_bIsTopCamera)
            Set_Direction(&vPlayerPos);
        else
            m_eDir = FRONT;

        // 방향에 따른 애니메이션 설정 : 방향 바꼈다고 장전 끊기지 않도록 설정
        _int iFrame = 0, iFrameSpeed = 4;
        // 장전된 상태면 총알 생성 후 비장전 상태로 전환
        if (m_bIsReloaded)
        {
            switch (m_eDir)
            {
            case FRONT:
                iFrame = 224 + m_iAttackAnimProgress;
                break;
            case RIGHT:
            case LEFT:
                iFrame = 240 + m_iAttackAnimProgress;
                break;
            case BACK:
                iFrame = 256 + m_iAttackAnimProgress;
                break;
            }

            if (iFrame % 16 == 1)
            {
                iFrameSpeed = 30;
                if (m_iFrameCount == iFrameSpeed)
                {
                    CScene* pScene = Engine::Get_Scene();
                    _vec3 vPos;
                    m_pTransformCom->Get_Info(INFO_POS, &vPos);
                    CGameObject* pBullet = CBullet::Create(m_pGraphicDev, vPos);
                    NULL_CHECK(pBullet);
                    m_vecBulletName.push_back(L"Monster_Created_Bullet" + std::to_wstring(m_iTagNumber++));
                    FAILED_CHECK_RETURN(pScene->Create_GameObject(L"Layer_GameLogic", pBullet, m_vecBulletName.back().c_str()), );
                }
            }
            else
                iFrameSpeed = 8;

            if (m_iFrameCount++ > iFrameSpeed)
            {
                m_iFrameCount = 0;
                if (++m_iAttackAnimProgress >= 3)
                {
                    m_bAttackSuccess = true;
                    m_bIsReloaded = false;
                    m_iAttackAnimProgress = 0;
                }
                m_pAnimatorCom->Set_CurState(SWING, iFrame, iFrame, -1);
            }
        }
        // 장전 안됐으면 장전
        else
        {
            switch (m_eDir)
            {
            case FRONT:
                iFrame = 272 + m_iAttackAnimProgress;
                break;
            case RIGHT:
            case LEFT:
                iFrame = 288 + m_iAttackAnimProgress;
                break;
            case BACK:
                iFrame = 304 + m_iAttackAnimProgress;
                break;
            }
            if (m_iFrameCount++ > iFrameSpeed)
            {
                m_iFrameCount = 0;
                if (++m_iAttackAnimProgress >= 16)
                {
                    m_bAttackSuccess = true;
                    m_bIsReloaded = true;
                    m_iAttackAnimProgress = 0;
                }
                m_pAnimatorCom->Set_CurState(SWING, iFrame, iFrame, -1);
            }
        }
    }
  
}

// 죽어버리기
void CHunter::Pattern_Dead()
{
    if (m_bKnockBackEnd)
    {
        switch (m_eDir)
        {
        case FRONT:
            m_pAnimatorCom->Set_CurState(DEAD, 32, 33, 8);
            break;
        case RIGHT:
        case LEFT:
            m_pAnimatorCom->Set_CurState(DEAD, 48, 49, 8);
            break;
        case BACK:
            m_pAnimatorCom->Set_CurState(DEAD, 64, 65, 8);
            break;
        }
        
        if (m_pAnimatorCom->Get_MotionEnd())
        {
            m_bStopDraw = true;
            m_pColliderCom->Set_Offset(_vec3(0, -100.f, 0));
            Drop_Item();
        }
    }
}

STATE CHunter::State_Change()
{
    _vec3 vPlayerPos, vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
    switch (m_eState)
    {
    case IDLE:
        // 쫓아가면서 사거리 계산
        if (m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, m_fRange))
            return SWING;
        if (m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, m_fAggroDistance))
            return WALK;
        break;
    case WALK:
        // 쫓아가면서 사거리 계산
        if (!m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, m_fAggroDistance))
            return IDLE;
        if (m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, m_fRange))
            return SWING;
        break;
    case SWING:
        // 공격 모션(장전 OR 사격)이 끝났을 때
        if (m_bAttackSuccess)
        {
            // 어그로 범위 내가 아닌 경우
            if (!m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, m_fAggroDistance))
            {
                m_iFrameCount = 0;
                m_iAttackAnimProgress = 0;
                return IDLE;
            }
            // 공격 사거리 이내가 아닌 경우
            if (!m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, m_fRange))
            {
                m_iFrameCount = 0;
                m_iAttackAnimProgress = 0;
                return WALK;
            }
        }
        break;
    }
    return m_eState;
}

void CHunter::Set_Direction(_vec3* vPlayerPos)
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



CHunter* CHunter::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CHunter* pHunter = new CHunter(pGraphicDev);

    if (FAILED(pHunter->Ready_GameObject(vPos)))
    {
        Safe_Release(pHunter);
        MSG_BOX("Hunter Create Failed");
        return nullptr;
    }
    return pHunter;
}

void CHunter::Free()
{
    Engine::CGameObject::Free();
}