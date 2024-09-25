#include "pch.h"
#include "../Header/Mushroom.h"
#include "Export_System.h"
#include "Export_Utility.h"

CMushroom::CMushroom(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev)
{
    m_eType = Engine::MON_MUSHROOM;
    m_fIdleY = 0.8f;
    m_eState = IDLE;
}

CMushroom::~CMushroom()
{
}

HRESULT CMushroom::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    m_pTransformCom->Set_Pos(vPos.x, m_fIdleY, vPos.z);
    m_pStateCom->Set_Stat(100, 0, 10, 0);
    m_vecDropItem.push_back(ITEM_MUCUS);
    m_vecDropItem.push_back(ITEM_SEED);
    m_vecDropItem.push_back(ITEM_WOOD);
    Set_Speed(0.6f);
    return S_OK;
}

_int CMushroom::Update_GameObject(const _float& fTimeDelta)
{
    Add_RenderGroup(RENDER_ALPHA, this);
    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CMushroom::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CMushroom::Render_GameObject()
{
    if (m_bStopDraw)
        return;
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
    m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture();

    m_pBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());
    m_pBufferCom->Render_Buffer();
    m_pColliderCom->Render_Collider();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CMushroom::Add_Component()
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

STATE CMushroom::State_Change()
{
    return STATE();
}

CMushroom* CMushroom::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    return nullptr;
}

void CMushroom::Free()
{
}
