#include "pch.h"
#include "../Header/WhitePlane.h"
#include "Export_Utility.h"
#include "Export_System.h"
#include "../Client/Header/StoryBackGround.h"


CWhitePlane::CWhitePlane(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CGameObject(pGraphicDev)
    , m_iCount(0)
    , m_iOpacity(0)
    , m_bFadeOver(false)
    , m_bStop(false)
{
}

CWhitePlane::~CWhitePlane()
{
}

HRESULT CWhitePlane::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    return S_OK;
}

_int CWhitePlane::Update_GameObject(const _float& fTimeDelta)
{
    if (!m_bStop)
    {
        CStoryBackGround* pStoryBackGround = dynamic_cast<CStoryBackGround*>(Engine::Get_GameObject(L"Layer_Environment", L"StoryBackGround"));

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
            if (pStoryBackGround->Get_Stop())
                Fade_Out();
        }

        if (pStoryBackGround->Get_Stop())
            Engine::Add_RenderGroup(RENDER_PRIORITY, this);

        return Engine::CGameObject::Update_GameObject(fTimeDelta);
    }
}

void CWhitePlane::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CWhitePlane::Render_GameObject()
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

void CWhitePlane::StartFadeIn()
{
    m_bFadeOver = false;
    m_iOpacity = 0;
    m_iCount = 0;
}

void CWhitePlane::StartFadeOut()
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

HRESULT CWhitePlane::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_WhiteTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_WhiteTexture"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_WhiteTransform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_WhiteTransform", pComponent });

    return S_OK;
}

CWhitePlane* CWhitePlane::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CWhitePlane* pWhitePlane = new CWhitePlane(pGraphicDev);

    if (FAILED(pWhitePlane->Ready_GameObject()))
    {
        Safe_Release(pWhitePlane);
        MSG_BOX("pWhitePlane Create Failed");
        return nullptr;
    }

    return pWhitePlane;
}

_int CWhitePlane::Fade_In()
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

_int CWhitePlane::Fade_Out()
{
    if (m_bFadeOver)
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

void CWhitePlane::Set_Opacity(_int _iOpacity)
{
    m_iOpacity = _iOpacity;
}

void CWhitePlane::Free()
{
    Engine::CGameObject::Free();
}
