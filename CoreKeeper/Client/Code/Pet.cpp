#include "pch.h"
#include "../Header/Pet.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "../Header/Player.h"
#include "../Header/PetFire.h"

CPet::CPet(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev)
{
    m_fIdleY = 1.3f;
    m_eState = IDLE;
    m_eDir = RIGHT;

    m_bFlip = false;

    m_fAggroDistance = 14.f;

    m_bCharging = false;
    m_iFrameCount = 0;
    m_iAttackAnimProgress = 0;

    m_vecFireName.reserve(16);
    m_iCurNumber = 0;

    m_fIdleTimeLimit = 3.f;

    m_eIdleType = PET_IDLE;
    m_bIdleFirstFrame = true;
    
    m_iChaseType = 0;
    m_bChaseFirstFrame = true;
    m_iChaseStep = 0;
    m_vMoveDir = { 0, 0, 0 };
    m_fSaveTimer = 0.f;

    m_iSwingType = 0;
    m_bSwingFirstFrame = true;

    m_bIsInteracting = false;
    m_bIsTeleporting = false;
    m_bIsForcedMove = false;
    m_fTeleportTimer = 0.f;

    // 플레이어에서 관리할 것
    m_bStopDraw = false;
    m_bRespawned = false;
    m_bJustRespawned = true;
}

CPet::~CPet()
{
}

HRESULT CPet::Ready_GameObject(_vec3 vPos)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
    m_vRespawnPoint = { vPos.x, m_fIdleY, vPos.z };
    m_pTransformCom->Set_Pos(m_vRespawnPoint.x, m_vRespawnPoint.y, m_vRespawnPoint.z);
    m_pTransformCom->Set_Scale(1.5f, 1.5f, 1.5f);
    Set_Speed(4.5f);
    return S_OK;
}

_int CPet::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);
    if (m_bStopDraw)
    {
        m_bRespawned = false;
        return 0;
    }

    Set_Cast();
    if (!m_bRespawned)
    {
        m_bRespawned = true;
        _vec3 vPlayerPos;
        m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
        Set_Teleport(vPlayerPos);
    }

    if (m_bIsTeleporting)
    {
        switch (m_eDir)
        {
        case FRONT:
            m_pAnimatorCom->Set_CurState(IDLE, 288, 293, 8);
            break;
        case RIGHT:
        case LEFT:
            m_pAnimatorCom->Set_CurState(IDLE, 300, 305, 8);
            break;
        case BACK:
            m_pAnimatorCom->Set_CurState(IDLE, 312, 317, 8);
            break;
        }
        if (m_pAnimatorCom->Get_MotionEnd())
            m_bIsTeleporting = false;
    }
    else
    {
        Pattern_Interact();
        if (m_bIsInteracting)
        {
            switch (m_eDir)
            {
            case FRONT:
                m_pAnimatorCom->Set_CurState(IDLE, 252, 261, 8);
                break;
            case RIGHT:
            case LEFT:
                m_pAnimatorCom->Set_CurState(IDLE, 264, 273, 8);
                break;
            case BACK:
                m_pAnimatorCom->Set_CurState(IDLE, 276, 285, 8);
                break;
            }
            if (m_pAnimatorCom->Get_MotionEnd())
                m_bIsInteracting = false;
        }
        else
        {
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
        }
        //Set_StuckFree(fTimeDelta);
    }
    Flip();
    m_pAnimatorCom->Update_Animation();
    Engine::Add_RenderGroup(RENDER_ALPHA, this);
    return iExit;
}

void CPet::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CPet::Render_GameObject()
{
    if (m_bStopDraw)
        return;

    _vec3		vPos, vPlayerPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture();

    m_pBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());
    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CPet::Add_Component()
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

    pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_PetAnimTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PetTex")); // Proto_PetTailTex
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void CPet::Pattern_Idle(const _float& fTimeDelta)
{
    m_bChaseFirstFrame = true;
    m_bSwingFirstFrame = true;



    // 패턴 종료 시 다음 패턴 설정
    if (!m_bIdling)
    {
        m_bIdling = true;
        m_fIdleTime = 0.f;
        _int iIdleType = 0;
        switch (m_eIdleType)
        {
        case PET_IDLE:
            iIdleType = rand() % 5;
            if (0 == iIdleType)
                m_eIdleType = PET_STRETCH;
            else if (1 == iIdleType)
                m_eIdleType = PET_SITDOWN;
            else
                m_eIdleType = PET_IDLE;
            break;
        case PET_STRETCH:
        case PET_STAND:
            m_eIdleType = PET_IDLE;
            break;
        case PET_SITDOWN:
        case PET_GROOM:
            m_eIdleType = PET_SITTING;
            break;
        case PET_SITTING:
            iIdleType = rand() % 5;
            if (0 == iIdleType)
                m_eIdleType = PET_STAND;
            else if (1 == iIdleType)
                m_eIdleType = PET_GROOM;
            else 
                m_eIdleType = PET_SITTING;
            break;
        default:
            break;
        }
    }

    if (m_bIdleFirstFrame)
    {
        m_bIdleFirstFrame = false;
        m_eIdleType = PET_IDLE;
        m_fIdleTime = 0.f;
        m_fSpeed = 4.5f;
    }

    if (!g_bIsTopCamera)
        m_eDir = FRONT;



    // 패턴 애니메이션 세팅
    switch (m_eIdleType)
    {
    case PET_IDLE:
        switch (m_eDir)
        {
        case FRONT:
            m_pAnimatorCom->Set_CurState(IDLE, 0, 5, 8);
            break;
        case RIGHT:
        case LEFT:
            m_pAnimatorCom->Set_CurState(IDLE, 12, 17, 8);
            break;
        case BACK:
            m_pAnimatorCom->Set_CurState(IDLE, 24, 29, 8);
            break;
        }
        break;
    case PET_STRETCH:
        switch (m_eDir)
        {
        case FRONT:
            m_pAnimatorCom->Set_CurState(IDLE, 36, 43, 12);
            break;
        case RIGHT:
        case LEFT:
            m_pAnimatorCom->Set_CurState(IDLE, 48, 55, 12);
            break;
        case BACK:
            m_pAnimatorCom->Set_CurState(IDLE, 60, 67, 12);
            break;
        }
        break;
    case PET_SITDOWN:
        switch (m_eDir)
        {
        case FRONT:
            m_pAnimatorCom->Set_CurState(IDLE, 72, 75, 8);
            break;
        case RIGHT:
        case LEFT:
            m_pAnimatorCom->Set_CurState(IDLE, 84, 87, 8);
            break;
        case BACK:
            m_pAnimatorCom->Set_CurState(IDLE, 96, 99, 8);
            break;
        }
        break;
    case PET_SITTING:
        switch (m_eDir)
        {
        case FRONT:
            m_pAnimatorCom->Set_CurState(IDLE, 108, 113, 8);
            break;
        case RIGHT:
        case LEFT:
            m_pAnimatorCom->Set_CurState(IDLE, 120, 125, 8);
            break;
        case BACK:
            m_pAnimatorCom->Set_CurState(IDLE, 132, 137, 8);
            break;
        }
        break;
    case PET_STAND:
        switch (m_eDir)
        {
        case FRONT:
            m_pAnimatorCom->Set_CurState(IDLE, 144, 147, 8);
            break;
        case RIGHT:
        case LEFT:
            m_pAnimatorCom->Set_CurState(IDLE, 156, 159, 8);
            break;
        case BACK:
            m_pAnimatorCom->Set_CurState(IDLE, 168, 171, 8);
            break;
        }
        break;
    case PET_GROOM:
        switch (m_eDir)
        {
        case FRONT:
            m_pAnimatorCom->Set_CurState(IDLE, 180, 191, 8);
            break;
        case RIGHT:
        case LEFT:
            m_pAnimatorCom->Set_CurState(IDLE, 192, 203, 8);
            break;
        case BACK:
            m_pAnimatorCom->Set_CurState(IDLE, 204, 215, 8);
            break;
        }
        break;
    }

    // 패턴 종료 시점 설정
    switch (m_eIdleType)
    {
    case PET_IDLE:
    case PET_SITTING:
        if (m_fIdleTime <= m_fIdleTimeLimit)
            m_fIdleTime += fTimeDelta;
        else
        {
            m_fIdleTime = 0.f;
            m_bIdling = false;
        }
        break;
    case PET_STRETCH:
    case PET_SITDOWN:
    case PET_STAND:
    case PET_GROOM:
        if (m_pAnimatorCom->Get_MotionEnd())
            m_bIdling = false;
        break;
    }
}

void CPet::Pattern_Chase(const _float& fTimeDelta)
{
    m_bIdleFirstFrame = true;
    m_bSwingFirstFrame = true;
    if (m_bChaseFirstFrame)
    {
        m_bChaseFirstFrame = false;
        m_iChaseStep = 0;
        switch (m_eIdleType)
        {
            // 해당 패턴이 끝난 후 일어서서 이동
        case PET_SITTING:
        case PET_SITDOWN:
        case PET_GROOM:
            m_iChaseType = 0;
            break;
            // 해당 패턴이 끝나면 이동
        case PET_IDLE:
        case PET_STRETCH:
        case PET_STAND:
            m_iChaseType = 1;
            break;
        }
    }        
    _vec3		vPos, vPlayerPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
    
    _int iPlayerIndex = _int(vPlayerPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + (vPlayerPos.x + 0.5f * VTXITV);
    if (m_listTargetIndex.empty())
        m_listTargetIndex.push_back(iPlayerIndex);
    else
    {
        m_fSaveTimer += fTimeDelta;
        if (m_fSaveTimer >= 1.5f)
        {
            m_fSaveTimer = 0.f;
            if (m_listTargetIndex.back() != iPlayerIndex)
                m_listTargetIndex.push_back(iPlayerIndex);
        }
    }


    // 해당 패턴이 끝난 후 일어서서 이동
    if (0 == m_iChaseType)
    {
        // 해당 패턴이 끝났는지 확인
        if (0 == m_iChaseStep) 
        {
            // 대충 확인하는 중
        }
        // 퀵 스탠딩(던파 아님)
        else if (1 == m_iChaseStep)
        {
            switch (m_eDir)
            {
            case FRONT:
                m_pAnimatorCom->Set_CurState(WALK, 144, 147, 5);
                break;
            case RIGHT:
            case LEFT:
                m_pAnimatorCom->Set_CurState(WALK, 156, 159, 5);
                break;
            case BACK:
                m_pAnimatorCom->Set_CurState(WALK, 168, 171, 5);
                break;
            }
        }
        // 놈을 산채로 잡아라
        else
            m_iChaseType = 1;

        if (m_pAnimatorCom->Get_MotionEnd())
            m_iChaseStep++;

    }
    // 해당 패턴이 끝나면 방향 설정 후 이동
    else
    {
        if (!Check_Wall())
        {
            for (auto iter = m_listTargetIndex.begin(); iter != m_listTargetIndex.end();)
            {
                if (m_listTargetIndex.empty())
                    break;
                m_listTargetIndex.erase(iter++);
            }
        }
        if (m_fSpeed < 8.f)
            m_fSpeed += fTimeDelta;
        if (!m_listTargetIndex.empty())
        {
            _int iFrontIndex = m_listTargetIndex.front();
            _int iCurPetIndex = _int(vPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + (vPos.x + 0.5f * VTXITV);
            _float fX;
            _float fZ;
            if (iFrontIndex == iCurPetIndex) 
            {
                m_listTargetIndex.pop_front();
                if (!m_listTargetIndex.empty())
                {
                    iFrontIndex = m_listTargetIndex.front();
                    fX = (iFrontIndex % (VTXCNTX - 1)) * VTXITV;
                    fZ = (iFrontIndex / (VTXCNTX - 1)) * VTXITV;
                    _vec3 vDestPos = { fX, 0, fZ };
                    m_vMoveDir = vDestPos - vPos;
                }
                else
                    m_vMoveDir = vPlayerPos - vPos;
            }
            else 
            {
                iFrontIndex = m_listTargetIndex.front();
                fX = (iFrontIndex % (VTXCNTX - 1)) * VTXITV;
                fZ = (iFrontIndex / (VTXCNTX - 1)) * VTXITV;
                _vec3 vDestPos = { fX, 0, fZ };
                m_vMoveDir = vDestPos - vPos;
            }
        }
        else
            m_vMoveDir = vPlayerPos - vPos;
        
        D3DXVec3Normalize(&m_vMoveDir, &m_vMoveDir);
        m_vMoveDir.y = 0;
        Check_Stop();
        if (m_iSpeedWeight == 0)
        {
            m_fTeleportTimer += fTimeDelta;
            if (g_bIsTopCamera)
                Set_Direction(&vPlayerPos);
            switch (m_eDir)
            {
            case FRONT:
            case BACK:
                if (vPlayerPos.x >= vPos.x)
                    m_vMoveDir = { 1, 0, 0 };
                else
                    m_vMoveDir = { -1, 0, 0 };
                Check_Stop();
                m_pTransformCom->Move_Pos(&m_vMoveDir, fTimeDelta, m_fSpeed * m_iSpeedWeight);
                break;
            case LEFT:
            case RIGHT:
                if (vPlayerPos.z >= vPos.z)
                    m_vMoveDir = { 0, 0, 1 };
                else
                    m_vMoveDir = { 0, 0, -1 };
                Check_Stop();
                m_pTransformCom->Move_Pos(&m_vMoveDir, fTimeDelta, m_fSpeed * m_iSpeedWeight);
                break;
            }
            
            if (m_fTeleportTimer >= 2.f)
            {
                m_fTeleportTimer = 0.f;
                Set_Teleport(vPlayerPos);
            }
        }
        else
        {
            m_fTeleportTimer = 0.f;
            if (g_bIsTopCamera)
                Set_Direction(&vPlayerPos);
            else
                m_eDir = FRONT;

            switch (m_eDir)
            {
            case FRONT:
                m_pAnimatorCom->Set_CurState(WALK, 216, 221, 8);
                break;
            case RIGHT:
            case LEFT:
                m_pAnimatorCom->Set_CurState(WALK, 228, 233, 8);
                break;
            case BACK:
                m_pAnimatorCom->Set_CurState(WALK, 240, 245, 8);
                break;
            }
            m_pTransformCom->Move_Pos(&m_vMoveDir, fTimeDelta, m_fSpeed * m_iSpeedWeight);
        }
    }
}

void CPet::Pattern_Attack(const _float& fTimeDelta)
{
}

void CPet::Pattern_Interact()
{
    if (m_eState == IDLE)
    {
        if (Engine::Key_Down(DIK_E))
        {
            if (!m_bIsInteracting)
            {
                m_bIsInteracting = true;
                m_bIdleFirstFrame = true;
                m_bChaseFirstFrame = true;
                m_bSwingFirstFrame = true;
                _vec3 vPlayerPos;
                m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
                if (g_bIsTopCamera)
                    Set_Direction(&vPlayerPos);
                else
                    m_eDir = FRONT;
            }
        }
    }
}

void CPet::Pattern_Dead()
{
}

STATE CPet::State_Change()
{
    _vec3		vPos, vPlayerPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
    if (!m_pCalculatorCom->Check_Distance2D(&vPos, &vPlayerPos, 12.f))
    {
        Set_Teleport(vPlayerPos);
        return IDLE;
    }
    switch (m_eState)
    {
    case WALK:
        if (m_pCalculatorCom->Check_Distance2D(&vPos, &vPlayerPos, 2.f))
            return IDLE;
        break;
    case IDLE:
        if (!m_pCalculatorCom->Check_Distance2D(&vPos, &vPlayerPos, 3.f))
            return WALK;
        break;
    }
    return m_eState;
}

void CPet::Set_Direction(_vec3* vPlayerPos)
{
    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);

    // 몬스터 위치를 지나는 기울기가 1인 직선의 방정식에 플레이어의 위치 대입
    float diagLBtoRT = -vPlayerPos->x - vPlayerPos->z + (vPos.z + vPos.x);

    // 몬스터 위치를 지나는 기울기가 -1인 직선의 방정식에 플레이어의 위치 대입
    float diagLTtoRB = vPlayerPos->x - vPlayerPos->z + (vPos.z - vPos.x);

    // 플레이어가 상단
    if (diagLBtoRT <= 0 && diagLTtoRB <= 0)
    {
        if (m_eDir == FRONT)
        {
            for (auto iter = m_listTargetIndex.begin(); iter != m_listTargetIndex.end();)
            {
                if (m_listTargetIndex.empty())
                    break;
                m_listTargetIndex.erase(iter++);
            }
        }
        m_eDir = BACK;
    }
    // 플레이어가 우측
    else if (diagLBtoRT < 0 && diagLTtoRB > 0)
    {
        if (m_eDir == LEFT)
        {
            for (auto iter = m_listTargetIndex.begin(); iter != m_listTargetIndex.end();)
            {
                if (m_listTargetIndex.empty())
                    break;
                m_listTargetIndex.erase(iter++);
            }
        }
        m_eDir = RIGHT;
    }
    // 플레이어가 좌측
    else if (diagLBtoRT >= 0 && diagLTtoRB <= 0)
    {
        if (m_eDir == RIGHT)
        {
            for (auto iter = m_listTargetIndex.begin(); iter != m_listTargetIndex.end();)
            {
                if (m_listTargetIndex.empty())
                    break;
                m_listTargetIndex.erase(iter++);
            }
        }
        m_eDir = LEFT;
    }
    // 플레이어가 하단에 있을 경우
    else if (diagLBtoRT >= 0 && diagLTtoRB >= 0)
    {
        if (m_eDir == BACK)
        {
            for (auto iter = m_listTargetIndex.begin(); iter != m_listTargetIndex.end();)
            {
                if (m_listTargetIndex.empty())
                    break;
                m_listTargetIndex.erase(iter++);
            }
        }
        m_eDir = FRONT;
    }
}

void CPet::Set_Teleport(_vec3 vPlayerPos)
{
    m_bIsTeleporting = true;
    // 플레이어 위치 인덱스 구하기
    _int iIndex = _int(vPlayerPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + (vPlayerPos.x + 0.5f * VTXITV);
    _int iLeft = iIndex - 1;
    _int iRight = iIndex + 1;
    _int iBottom = iIndex - (VTXCNTX - 1);
    _int iTop = iIndex + (VTXCNTX - 1);
    _float fX, fZ;
    if (0 <= iLeft && iLeft < (VTXCNTX - 1) * (VTXCNTZ - 1))
    {
        if (!m_pTerrain->Get_UnreachableByIndex(iLeft))
        {
            fX = (iLeft % (VTXCNTX - 1)) * VTXITV;
            fZ = (iLeft / (VTXCNTX - 1)) * VTXITV;
            m_pTransformCom->Set_Pos(fX, m_fIdleY, fZ);
        }
        else
        {
            if (0 <= iRight && iRight < (VTXCNTX - 1) * (VTXCNTZ - 1))
            {
                if (!m_pTerrain->Get_UnreachableByIndex(iRight))
                {
                    fX = (iRight % (VTXCNTX - 1)) * VTXITV;
                    fZ = (iRight / (VTXCNTX - 1)) * VTXITV;
                    m_pTransformCom->Set_Pos(fX, m_fIdleY, fZ);
                }
                else
                {
                    if (0 <= iBottom && iBottom < (VTXCNTX - 1) * (VTXCNTZ - 1))
                    {
                        if (!m_pTerrain->Get_UnreachableByIndex(iBottom))
                        {
                            fX = (iBottom % (VTXCNTX - 1)) * VTXITV;
                            fZ = (iBottom / (VTXCNTX - 1)) * VTXITV;
                            m_pTransformCom->Set_Pos(fX, m_fIdleY, fZ);
                        }
                        else
                        {
                            if (0 <= iTop && iTop < (VTXCNTX - 1) * (VTXCNTZ - 1))
                            {
                                if (!m_pTerrain->Get_UnreachableByIndex(iTop))
                                {
                                    fX = (iTop % (VTXCNTX - 1)) * VTXITV;
                                    fZ = (iTop / (VTXCNTX - 1)) * VTXITV;
                                    m_pTransformCom->Set_Pos(fX, m_fIdleY, fZ);
                                }
                                else
                                    m_pTransformCom->Set_Pos(vPlayerPos.x, m_fIdleY, vPlayerPos.z);
                            }
                        }
                    }
                }
            }
        }
    }
    m_fTeleportTimer = 0.f;
    for (auto iter = m_listTargetIndex.begin(); iter != m_listTargetIndex.end();)
    {
        if (m_listTargetIndex.empty())
            break;
        m_listTargetIndex.erase(iter++);
    }
}

void CPet::Check_Stop()
{
    m_iSpeedWeight = 1;
    _vec3 vCheckPos;
    m_pTransformCom->Get_Info(INFO_POS, &vCheckPos);

    // 미래의 캐릭터 중점 좌표
    vCheckPos += m_vMoveDir * m_fSpeed * 0.1f;

    // 미래 중점 좌표 기준 인덱스 값
    _int iIndex = _int(vCheckPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + (vCheckPos.x + 0.5f * VTXITV);
    if (0 <= iIndex && iIndex < (VTXCNTX - 1) * (VTXCNTZ - 1))
        if (m_pTerrain->Get_UnreachableByIndex(iIndex))
            m_iSpeedWeight = 0;
}

CPet* CPet::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CPet* pPet = new CPet(pGraphicDev);

    if (FAILED(pPet->Ready_GameObject(vPos)))
    {
        Safe_Release(pPet);
        MSG_BOX("Pet Create Failed");
        return nullptr;
    }
    return pPet;
}

void CPet::Free()
{
    Engine::CGameObject::Free();
}