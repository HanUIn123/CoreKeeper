#include "pch.h"
#include "../Header/WorldMap.h"
#include "../Header/Export_System.h"
#include "../Header//Export_Utility.h"
#include "../../Client/Header/Terrain.h"
#include "../../Client/Header/DynamicCamera.h"

CWorldMap::CWorldMap(LPDIRECT3DDEVICE9 pGraphicDev)
    :Engine::CGameObject(pGraphicDev)
    , m_bShowMap(false)
{
    m_WorldMapMainViewPort.X = 0;
    m_WorldMapMainViewPort.Y = 0;
    m_WorldMapMainViewPort.Width = WINCX;
    m_WorldMapMainViewPort.Height = WINCY;
    m_WorldMapMainViewPort.MinZ = 0.0f;
    m_WorldMapMainViewPort.MaxZ = 1.0f;

    m_WorldMapViewPort.X = 400;
    m_WorldMapViewPort.Y = 120;
    m_WorldMapViewPort.Width = 480;
    m_WorldMapViewPort.Height = 480;
    m_WorldMapViewPort.MinZ = 0.0f;
    m_WorldMapViewPort.MaxZ = 1.0f;
}

CWorldMap::~CWorldMap()
{
}

HRESULT CWorldMap::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);


    return S_OK;
}

_int CWorldMap::Update_GameObject(const _float& fTimeDelta)
{
    CDynamicCamera* pDynamicCamera = dynamic_cast<CDynamicCamera*>(Engine::Get_GameObject(L"Layer_Environment", L"DynamicCamera"));

    if (pDynamicCamera)
    {
        if(pDynamicCamera->Get_IsWorldMap())
        {
            m_bShowMap = true;
        }
        else
        {
            m_bShowMap = false;
        }
    }

    if (m_bShowMap)
    {
        m_pGraphicDev->SetViewport(&m_WorldMapViewPort);

        if (pDynamicCamera)
        {
            // 카메라 줌비율 가져와서, WorldMapFrame의 비율도 그만큼 줄여버리기(늘리기).
            float zoomRatio = pDynamicCamera->Get_ZoomRatio();

            m_pTransformCom->Set_Scale(240 * zoomRatio, 250.0f * zoomRatio, 0.0f);
        }

        m_pTransformCom->Set_Pos(0, 0.0f, 0.0f);

        m_pGraphicDev->SetViewport(&m_WorldMapMainViewPort);

        Add_RenderGroup(RENDER_MAP, this);
    }

    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CWorldMap::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CWorldMap::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture();
    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CWorldMap::Add_Component()
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

CWorldMap* CWorldMap::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CWorldMap* pWorldMap = new CWorldMap(pGraphicDev);

    if (FAILED(pWorldMap->Ready_GameObject()))
    {
        Safe_Release(pWorldMap);
        MSG_BOX("pWorldMap Create Failed");
        return nullptr;
    }

    return pWorldMap;
}

void CWorldMap::Free()
{
    Engine::CGameObject::Free();
}
