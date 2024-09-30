#include "pch.h"
#include "../Header/MiniMapFrame.h"
#include "../Header/Export_System.h"
#include "../Header//Export_Utility.h"

CMiniMapFrame::CMiniMapFrame(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CGameObject(pGraphicDev)
    , m_bIsMinimapExpanded(false)
{
}

CMiniMapFrame::~CMiniMapFrame()
{
}

HRESULT CMiniMapFrame::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    return S_OK;
}

_int CMiniMapFrame::Update_GameObject(const _float& fTimeDelta)
{

    _vec3 vPos;
    m_pTransformCom->Set_Scale(100.0f, 75.0f, 0.0f);
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    m_pTransformCom->Set_Pos(480.0f, 200.0f, 0.0f);

    Add_RenderGroup(RENDER_UI, this);

    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CMiniMapFrame::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CMiniMapFrame::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture();

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CMiniMapFrame::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MapFrame"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

CMiniMapFrame* CMiniMapFrame::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMiniMapFrame* pMiniFrame = new CMiniMapFrame(pGraphicDev);

    if (FAILED(pMiniFrame->Ready_GameObject()))
    {
        Safe_Release(pMiniFrame);
        MSG_BOX("pMiniFrame Create Failed");
        return nullptr;
    }

    return pMiniFrame;
}

void CMiniMapFrame::Free()
{
    Engine::CGameObject::Free();
}
