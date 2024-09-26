#include "pch.h"
#include "../Header/MapToolTerrain.h"

CMapToolTerrain::CMapToolTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CGameObject(pGraphicDev)
    , m_pBufferCom(nullptr)
    , m_pTransformCom(nullptr)
    , m_pCalculatorCom(nullptr)
    , vPickPos(0, 0, 0)
{
    m_vecTextureNumber.resize((VTXCNTX-1) * (VTXCNTZ-1));
    m_vecUnreachable.resize((VTXCNTX - 1) * (VTXCNTZ - 1));

}

CMapToolTerrain::~CMapToolTerrain()
{
}

HRESULT CMapToolTerrain::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    return S_OK;
}

_int CMapToolTerrain::Update_GameObject(const _float& fTimeDelta)
{
    Add_RenderGroup(RENDER_NONALPHA, this);

    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CMapToolTerrain::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CMapToolTerrain::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    for (int i = 0; i < VTXCNTZ-1; ++i)
    {
        for (int j = 0; j < VTXCNTX-1; ++j)
        {
            int idx = i * (VTXCNTX-1) + j;

            if (m_vecUnreachable[idx])
                continue;

            auto texture = m_pTextureCom->Get_Texture(m_vecTextureNumber[idx]);
            m_pGraphicDev->SetTexture(0, texture);
            m_pBufferCom->Render_Texture(idx * 6);
        }
    }

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CMapToolTerrain::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pBufferCom = dynamic_cast<CMapToolTex*>(Engine::Clone_Proto(L"Proto_MapToolTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Calculator", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MapToolTerrainTexture"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

HRESULT CMapToolTerrain::Setup_Material()
{
    D3DMATERIAL9		tMtrl;
    ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

    tMtrl.Diffuse = { 1.f, 1.f, 1.f, 1.f };
    tMtrl.Specular = { 1.f, 1.f, 1.f, 1.f };
    tMtrl.Ambient = { 0.2f, 0.2f, 0.2f, 1.f };

    tMtrl.Emissive = { 0.f, 0.f, 0.f, 0.f };
    tMtrl.Power = 0.f;

    m_pGraphicDev->SetMaterial(&tMtrl);


    return S_OK;
}

_vec3 CMapToolTerrain::Picking_OnTerrain()
{   
    _vec3 vPickPos = m_pCalculatorCom->Picking_OnTerrain(g_hWnd, m_pBufferCom, m_pTransformCom);

    return vPickPos;
}

CMapToolTerrain* CMapToolTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMapToolTerrain* pMaptoolTerrain = new CMapToolTerrain(pGraphicDev);

    if (FAILED(pMaptoolTerrain->Ready_GameObject()))
    {
        Safe_Release(pMaptoolTerrain);
        MSG_BOX("pTerrain Create Failed");
        return nullptr;
    }

    return pMaptoolTerrain;
}

void CMapToolTerrain::Free()
{
    Engine::CGameObject::Free();
}
