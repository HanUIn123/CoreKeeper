#include "pch.h"
#include "../Header/Slime.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "../Header/Player.h"

CSlime::CSlime(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev)
{
    m_eType = Engine::MON_SLIME;
    m_fIdleY = 0.4f;
    m_fJumpY = 3.f;
    m_eState = IDLE;
    m_fAggroDistance = 8.f;
}

CSlime::~CSlime()
{
}

HRESULT CSlime::Ready_GameObject(_vec3 vPos)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_vRespawnPoint = { vPos.x, m_fIdleY, vPos.z };
    m_pTransformCom->Set_Pos(m_vRespawnPoint.x, m_vRespawnPoint.y, m_vRespawnPoint.z);
    m_pStateCom->Set_Stat(100, 0, 10, 0);
    m_vecDropItem.push_back(ITEM_MUCUS);
    m_vecDropItem.push_back(ITEM_WOOD);
    m_vecDropItem.push_back(ITEM_BERRY_SEED);
    Set_Speed(0.8f);

    m_pHitParticleCom->init(L"../Bin/Resource/Texture/Effect/Hit_%d.png", 5, 1.0f);

    return S_OK;
}

_int CSlime::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);
    if (m_bStopDraw)
    {
        Set_RespawnTimer(fTimeDelta);
        return 0;
    }

    Set_Cast();

    if (m_eState != DEAD && m_iSpeedWeight)
        Check_Hitted();

    if (m_bKnockBackEnd)
    {
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
    }
    else
        KnockBack(fTimeDelta, 1.8f);

    if (m_bHit)
    {
        m_pHitParticleCom->update(fTimeDelta);

        if (m_pHitParticleCom->isDead())
        {
            m_pHitParticleCom->reset();
            m_bHit = false;
        }
    }


    //Apply_Billboard();

    //Set_StuckFree(fTimeDelta);
    m_pAnimatorCom->Update_Animation();
    Add_RenderGroup(RENDER_ALPHA, this);
    return iExit;
}

void CSlime::LateUpdate_GameObject()
{

    Engine::CGameObject::LateUpdate_GameObject();
}

void CSlime::Render_GameObject()
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

HRESULT CSlime::Add_Component()
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

    pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_SlimeAnimTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_SlimeTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pColliderCom = dynamic_cast<CCollider*>(Engine::Clone_Proto(L"Proto_SlimeCollider"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    pComponent = m_pHitParticleCom = dynamic_cast<CHit*>(Engine::Clone_Proto(L"Proto_Hit"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Hit", pComponent });

    return S_OK;

}

// 일정 시간마다 타일 한칸 이동 or 정지
void CSlime::Pattern_Idle(const _float& fTimeDelta)
{
    m_pAnimatorCom->Set_CurState(IDLE, 0, 8, 10);
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
            m_fIdleTimeLimit = rand() % 4 + 2; // 1 ~ 5초
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

        switch (m_iDir)
        {
        case 0:
            // 정지
            break;
        case 1:
            // 상
            fLookSpeed = m_fSpeed;
            break;
        case 2:
            // 우상
            fLookSpeed = m_fDiagSpeed;
            fRightSpeed = m_fDiagSpeed;
            break;
        case 3:
            // 우
            fRightSpeed = m_fSpeed;
            break;
        case 4:
            // 우하
            fLookSpeed = -m_fDiagSpeed;
            fRightSpeed = m_fDiagSpeed;
            break;
        case 5:
            // 하
            fLookSpeed = m_fSpeed;
            break;
        case 6:
            // 좌하
            fLookSpeed = -m_fDiagSpeed;
            fRightSpeed = -m_fDiagSpeed;
            break;
        case 7:
            // 좌
            fRightSpeed = -m_fSpeed;
            break;
        case 8:
            // 좌상
            fLookSpeed = m_fDiagSpeed;
            fRightSpeed = -m_fDiagSpeed;
            break;
        }

        Set_Stop(fTimeDelta, &vLook, fLookSpeed, &vRight, fRightSpeed);

        if (m_iDir)
        {
            m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, fLookSpeed * m_iSpeedWeight);
            m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, fRightSpeed * m_iSpeedWeight);
        }
    }
    else
    {
        m_bIdling = false;
        m_fIdleTime = 0.f;
    }
}

// 플레이어 방향으로 이동
void CSlime::Pattern_Chase(const _float& fTimeDelta)
{
    _vec3		vPlayerPos, vPos, vDir;
    m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    vDir = vPlayerPos - vPos;
    D3DXVec3Normalize(&vDir, &vDir);
    m_pAnimatorCom->Set_CurState(WALK, 12, 21, 8);

    Set_Stop(fTimeDelta, &vDir, m_fSpeed);
    m_pTransformCom->Move_Pos(&vDir, fTimeDelta, m_fSpeed * m_iSpeedWeight);
}

// 점프 공격
void CSlime::Pattern_Attack(const _float& fTimeDelta)
{
    _vec3		vPos, vPlayerPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    m_pAnimatorCom->Set_CurState(SWING, 24, 35, 8);
    m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
    m_fSpeedWeight = 0.f;
    // 점프 중이 아닐 때
    if (m_pAnimatorCom->Get_MotionIndex() <= 29 || m_pAnimatorCom->Get_MotionIndex() >= 34)
    {
        m_bAttackSuccess = false;
        m_bFallStart = false;
        m_vStartPoint = vPos;
        m_vAttackPoint = vPlayerPos - vPos;
        D3DXVec3Normalize(&m_vAttackPoint, &m_vAttackPoint);
        if (m_pAnimatorCom->Get_MotionIndex() == 25 || m_pAnimatorCom->Get_MotionIndex() == 34)
            m_pTransformCom->Set_Pos(vPos.x, m_fIdleY - 0.001f, vPos.z);
        else if (m_pAnimatorCom->Get_MotionIndex() == 35)
        {
            m_pTransformCom->Set_Pos(vPos.x, m_fIdleY, vPos.z);
            m_bJumping = false;
            //if (m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, m_fAggroDistance))
            //    m_eState = SWING;
            //else
            //    m_eState = WALK;
        }
    }
    else
        // 점프 할 때
    {
        JumpY(fTimeDelta);
        m_fSpeedWeight = 6.f;
        if (vPos.y > m_fIdleY)
        {
            // 공격 성공
            if (m_pColliderCom->Check_Collision(m_pPlayerCollider))
            {
                if (!m_bAttackSuccess)
                {
                    m_bAttackSuccess = true;
                    m_pPlayer->Set_KnockBack(vPos, m_pStateCom->Get_Stat()->iAttack, (1 - (m_fJumpTime / m_fJumpFrame)) + 0.5f, HIT_NORMAL);
                }
            }
        }
        if (m_bAttackSuccess)
            FallDir(fTimeDelta);
        else
        {
            Set_Stop(fTimeDelta, &m_vAttackPoint, m_fSpeedWeight);
            m_pTransformCom->Move_Pos(&m_vAttackPoint, fTimeDelta, m_fSpeedWeight * m_iSpeedWeight);
        }
    }
}

void CSlime::Pattern_Dead()
{
    m_pAnimatorCom->Set_CurState(DEAD, 36, 41, 8);

    if (m_pAnimatorCom->Get_MotionEnd())
    {
        m_bStopDraw = true;
        m_pColliderCom->Set_Offset(_vec3(0, -100.f, 0));
        Drop_Item();
    }
}

STATE CSlime::State_Change()
{
    _vec3 vPos, vPlayerPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
    if (m_eState == WALK)
    {
        if (m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, m_fAggroDistance))
            m_eState = SWING;
        else
            m_eState = WALK;
    }
    if (m_eState == SWING && vPos.y == m_fIdleY)
    {
        if (!m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, m_fAggroDistance))
            return IDLE;
    }
    return m_eState;
}

CSlime* CSlime::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CSlime* pSlime = new CSlime(pGraphicDev);

    if (FAILED(pSlime->Ready_GameObject(vPos)))
    {
        Safe_Release(pSlime);
        MSG_BOX("Mushroom Create Failed");
        return nullptr;
    }
    return pSlime;
}

void CSlime::Free()
{
    Engine::CGameObject::Free();
}