#include "pch.h"
#include "..\Header\Terrain.h"
#include "Export_Utility.h"

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
	m_vPickPos = { };
	m_vecTextureNumber.resize((VTXCNTX - 1) * (VTXCNTZ - 1));
	m_vecUnreachable.resize((VTXCNTX - 1) * (VTXCNTZ - 1));
}

CTerrain::~CTerrain()
{
}

HRESULT CTerrain::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

_int CTerrain::Update_GameObject(const _float& fTimeDelta)
{
	Add_RenderGroup(RENDER_NONALPHA, this);

	Add_RenderGroup(RENDER_MAP, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CTerrain::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CTerrain::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	FAILED_CHECK_RETURN(Setup_Material(), );

	for (int i = 0; i < VTXCNTZ - 1; ++i)
	{
		for (int j = 0; j < VTXCNTX - 1; ++j)
		{
			int idx = i * (VTXCNTX - 1) + j;

			// 임시 확인용.
			//if (m_vecUnreachable[idx])
			//	continue;

			auto texture = m_pTextureCom->Get_Texture(m_vecTextureNumber[idx]);
			m_pGraphicDev->SetTexture(0, texture);
			m_pBufferCom->Render_Texture(idx * 6);
		}
	}

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CTerrain::Add_Component()
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

HRESULT CTerrain::Setup_Material()
{
	D3DMATERIAL9		tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = { 1.f, 1.f, 1.f, 1.f };
	tMtrl.Specular = { 1.f, 1.f, 1.f, 1.f };
	tMtrl.Ambient = { 0.7f, 0.7f, 0.7f, 0.7f };

	tMtrl.Emissive = { 0.01f, 0.01f, 0.01f, 0.01f };
	tMtrl.Power = 0.f;

	m_pGraphicDev->SetMaterial(&tMtrl);

	return S_OK;
}

_vec3* CTerrain::Get_PickPos()
{
	m_vPickPos = m_pCalculatorCom->Picking_OnTerrain(g_hWnd, m_pBufferCom, m_pTransformCom);
	return &m_vPickPos;
}

CTerrain* CTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTerrain* pTerrain = new CTerrain(pGraphicDev);

	if (FAILED(pTerrain->Ready_GameObject()))
	{
		Safe_Release(pTerrain);
		MSG_BOX("pTerrain Create Failed");
		return nullptr;
	}

	return pTerrain;
}

void CTerrain::Free()
{
	Engine::CGameObject::Free();
}
