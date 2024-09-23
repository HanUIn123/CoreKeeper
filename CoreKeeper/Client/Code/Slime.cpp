#include "pch.h"
#include "../Header/Slime.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "../Header/Player.h"

CSlime::CSlime(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev)
{
    m_eType = Engine::MON_SLIME;
    m_fIdleY = 0.6f;
    m_fJumpY = 8.f;
    m_eState = IDLE;
}

CSlime::~CSlime()
{
}

HRESULT CSlime::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    m_pTransformCom->Set_Pos(vPos.x, m_fIdleY, vPos.z);
    m_pStateCom->Set_Stat(100, 0, 10, 0);
    Set_Speed(0.6f);
    return S_OK;
}

_int CSlime::Update_GameObject(const _float& fTimeDelta)
{
    if (m_bStopDraw)
        return 0;
    if (!m_bDropSettings)
    {
        m_bDropSettings = true;
        m_pDropItem = dynamic_cast<CItem*>(Get_GameObject(L"Layer_GameLogic", L"Seed_Slime"));
        m_pDropItem->Set_Active(false);
    }
    if(m_eState != DEAD)
        Check_Hitted();
    m_pAnimatorCom->Update_Animation();
    if (m_bKnockBackEnd)
    {
        if(m_eState != DEAD)
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
    

    //Apply_Billboard();

    Add_RenderGroup(RENDER_ALPHA, this);
    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CSlime::LateUpdate_GameObject()
{

    Engine::CGameObject::LateUpdate_GameObject();
}

void CSlime::Render_GameObject()
{
    if (m_bStopDraw)
        return;
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
    m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture();

    m_pBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());
    m_pBufferCom->Render_Buffer();

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

    return S_OK;

}

STATE CSlime::State_Change()
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player"));
    if (m_eState == IDLE)
    {
        CGameObject* pWeapon = pPlayer->Get_HandedItem();
        // 플레이어가 무기를 들고 공격하는 상태면 충돌 체크
        if (pPlayer->Get_CurState() == SWING)
        {
            CCollider* pWeaponCollider = dynamic_cast<Engine::CCollider*>(pWeapon->Get_Component(ID_DYNAMIC, L"Com_Collider"));
            if (m_pColliderCom->Check_Collision(pWeaponCollider))
            {
                _vec3 vPlayerPos, vPos;
                dynamic_cast<CTransform*>(pPlayer->Get_Component(ID_DYNAMIC, L"Com_Transform"))->Get_Info(INFO_POS, &vPlayerPos);
                m_pTransformCom->Get_Info(INFO_POS, &vPos);

                // 무기와 충돌 했는데 공격 범위 이내인 경우
                if (m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, 5.f))
                    return SWING;
                // 공격 범위 밖인 경우
                else
                    return WALK;
            }
        }
    }
    return m_eState;
}

CSlime* CSlime::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CSlime* pSlime = new CSlime(pGraphicDev);

    if (FAILED(pSlime->Ready_GameObject()))
    {
        Safe_Release(pSlime);
        MSG_BOX("슬라임 소환 실패");
        return nullptr;
    }
    return pSlime;
}

void CSlime::Free()
{
    Engine::CGameObject::Free();
}
