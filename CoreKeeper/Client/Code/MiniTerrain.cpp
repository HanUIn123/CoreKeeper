#include "pch.h"
#include "../Header/MiniTerrain.h"
#include "Export_System.h"
#include "Export_Utility.h"

CMiniTerrain::CMiniTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CGameObject(pGraphicDev)
    , m_iMiniTerrainIndex(0)
{
    m_vecTextureNumber.resize((VTXCNTX - 1) * (VTXCNTZ - 1));

    m_vecIlluminated.resize((VTXCNTX - 1) * (VTXCNTZ - 1), false);
}

CMiniTerrain::~CMiniTerrain()
{
}

HRESULT CMiniTerrain::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
    return S_OK;
}

_int CMiniTerrain::Update_GameObject(const _float& fTimeDelta)
{
    CTransform* pPlayerTransform = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));

    if (pPlayerTransform)
    {
        _vec3 vPlayerPos;
        pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

        //int idx = i * (VTXCNTX - 1) + j;
        m_iMiniTerrainIndex = _int((_int(vPlayerPos.z) * (VTXCNTX - 1) + vPlayerPos.x));

        Illuminate_TerrainIndex(m_iMiniTerrainIndex);
    }


    Add_RenderGroup(RENDER_MAP, this);
    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CMiniTerrain::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CMiniTerrain::Render_GameObject()
{
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

    D3DMATERIAL9		backGroundMaterial;
    ZeroMemory(&backGroundMaterial, sizeof(D3DMATERIAL9));

    backGroundMaterial.Diffuse = D3DXCOLOR(0, 0, 0, 1);
    backGroundMaterial.Specular = { 0.f, 0.f, 0.f, 1.f };
    backGroundMaterial.Ambient = { 0.f, 0.f, 0.f, 1.f };
    backGroundMaterial.Emissive = { 0.f, 0.f, 0.f, 1.f };
    backGroundMaterial.Power = 0.f;

    
    for (int iIndex = 0; iIndex < m_vecIlluminated.size(); ++iIndex)
    {
        if (!m_vecIlluminated[iIndex])
        {
            m_pGraphicDev->SetMaterial(&backGroundMaterial);
            m_pGraphicDev->SetTexture(0, nullptr);
            m_pBufferCom->Render_Texture(iIndex * 6);
        }
    }

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CMiniTerrain::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pBufferCom = dynamic_cast<CTerrainTex*>(Engine::Clone_Proto(L"Proto_TerrainTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_TerrainTexture"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Calculator", pComponent });

    return S_OK;
}

void CMiniTerrain::Illuminate_TerrainIndex(_int _iIndex)
{
    if (_iIndex >= 0 && _iIndex <= m_vecIlluminated.size())
    {
        m_vecIlluminated[_iIndex] = true;
        if(_iIndex + 1 < m_vecIlluminated.size())
            m_vecIlluminated[_iIndex + 1] = true;
        if (_iIndex + VTXCNTX < m_vecIlluminated.size())
            m_vecIlluminated[_iIndex + VTXCNTX] = true;
    }
}

CMiniTerrain* CMiniTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMiniTerrain* pMiniTerrain = new CMiniTerrain(pGraphicDev);

    if (FAILED(pMiniTerrain->Ready_GameObject()))
    {
        Safe_Release(pMiniTerrain);
        MSG_BOX("pMiniTerrain Create Failed");
        return nullptr;
    }
    return pMiniTerrain;
}

void CMiniTerrain::Free()
{
    Engine::CGameObject::Free();
}
