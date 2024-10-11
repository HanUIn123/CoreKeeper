#include "pch.h"
#include "../Header/BlackPlane.h"
#include "Export_System.h"
#include "Export_Utility.h"

CBlackPlane::CBlackPlane(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CGameObject(pGraphicDev)
    , m_iCount(0)
    , m_iOpacity(255)
    , m_bFadeOver(false)
{
}

CBlackPlane::~CBlackPlane()
{
}

HRESULT CBlackPlane::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    return S_OK;
}

_int CBlackPlane::Update_GameObject(const _float& fTimeDelta)
{
    _vec3   vPos;
    m_pTransformCom->Set_Scale(WINCX, WINCY, 0.0f);
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    m_pTransformCom->Set_Pos(0, 0, 0.0f);

    if (!m_bFadeOver)
    {
        Fade_In(); 
    }
    else
    {
        Fade_Out(); 
    }

    Engine::Add_RenderGroup(RENDER_SUBORDINATE, this);

    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CBlackPlane::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CBlackPlane::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture();

    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(m_iOpacity, 255, 255, 255));
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
    m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);

    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CBlackPlane::StartFadeIn()
{
    m_bFadeOver = false; 
    m_iOpacity = 0; 
    m_iCount = 0; 
}

void CBlackPlane::StartFadeOut()
{
    m_bFadeOver = false; 
    m_iOpacity = 0;      
    m_iCount = 0;        

    while (m_iOpacity < 255)
    {
        if (m_iCount++)
        {
            m_iOpacity += 1;
            if (m_iOpacity > 255)
            {
                m_iCount = 0;
                m_iOpacity = 255;
            }
        }
    }
}

HRESULT CBlackPlane::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_BlackTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_BlackTexture"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_BlackTransform", pComponent });

    return S_OK;
}

CBlackPlane* CBlackPlane::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CBlackPlane* pBlackPlane = new CBlackPlane(pGraphicDev);

    if (FAILED(pBlackPlane->Ready_GameObject()))
    {
        Safe_Release(pBlackPlane);
        MSG_BOX("pBlackPlane Create Failed");
        return nullptr;
    }

    return pBlackPlane;
}

_int CBlackPlane::Fade_In()
{
    if (!m_bFadeOver)
    {
        if (m_iCount++)
        {
            m_iOpacity -= 2;

            if (m_iOpacity <= 0) 
            {
                m_iCount = 0;
                m_bFadeOver = true; 
                m_iOpacity = 0;
                return 0;
            }
        }
    }
}

_int CBlackPlane::Fade_Out()
{
    if (!m_bFadeOver)
    {
        if (m_iCount++)
        {
            m_iOpacity += 2;

            if (m_iOpacity >= 255) 
            {
                m_iCount = 0;
                m_bFadeOver = true; 
                m_iOpacity = 255; 
                return 0;
            }
        }
    }
}

void CBlackPlane::Set_Opacity(_int _iOpacity)
{
    m_iOpacity = _iOpacity;  
}

void CBlackPlane::Free()
{
    Engine::CGameObject::Free();
}
