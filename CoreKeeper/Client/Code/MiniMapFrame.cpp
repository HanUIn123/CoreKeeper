#include "pch.h"
#include "../Header/MiniMapFrame.h"
#include "../Header/Export_System.h"
#include "../Header//Export_Utility.h"

CMiniMapFrame::CMiniMapFrame(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CGameObject(pGraphicDev)
    , m_bIsMinimapExpanded(false)
    , m_fZoomRatio(300.0f)
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
    if (Engine::Get_DIKeyState(DIK_M) & 0x80)
    {
        if (!m_bKeyPressed)
        {
            m_bIsMinimapExpanded = !m_bIsMinimapExpanded;
            m_bKeyPressed = true;
        }
    }
    else
    {
        m_bKeyPressed = false;
    }

    if (m_bIsMinimapExpanded)
    {
        _vec3 vPos;
        m_pTransformCom->Set_Scale(350.0f, 260.0f, 0.0f);
        m_pTransformCom->Get_Info(INFO_POS, &vPos);
        m_pTransformCom->Set_Pos(0.0f, 0.0f, 0.0f);
    }
    else
    {
        _vec3 vPos;
        m_pTransformCom->Set_Scale(100.0f, 75.0f, 0.0f);
        m_pTransformCom->Get_Info(INFO_POS, &vPos);
        m_pTransformCom->Set_Pos(480.0f, 200.0f, 0.0f);
    }

    //Frame_Regulate();

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

void CMiniMapFrame::Frame_Regulate()
{
    m_fZoomRatio = 300.0f / CRenderer::GetInstance()->Get_ZoomRatio();

    if (m_bIsMinimapExpanded)
    {
        _vec3 vPos;
        float fFrameWidth = 350.f * m_fZoomRatio;
        float fFrameHeight = 240.f * m_fZoomRatio;

        if (fFrameWidth <= 420.0f)
        {
            m_pTransformCom->Set_Scale(fFrameWidth, fFrameHeight, 0.0f);
            m_pTransformCom->Get_Info(INFO_POS, &vPos);
            m_pTransformCom->Set_Pos(0.0f, 0.0f, 0.0f);
        }
    }
    else
    {
        _vec3 vPos;
        m_pTransformCom->Set_Scale(100.0f, 75.0f, 0.0f);
        m_pTransformCom->Get_Info(INFO_POS, &vPos);
        m_pTransformCom->Set_Pos(480.0f, 200.0f, 0.0f);
    }
}

void CMiniMapFrame::Free()
{
    Engine::CGameObject::Free();
}
