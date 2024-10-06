#include "pch.h"
#include "../Header/Tile.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\Core.h"

CTile::CTile(LPDIRECT3DDEVICE9 pGraphicDev)
	: CObject(pGraphicDev), m_iTextureNum(0), m_iStatueNum(0), m_bActive(true)
{
}

CTile::~CTile()
{
}

HRESULT CTile::Ready_GameObject(int _iStatueNum, int _iTextureNum, _vec3 vPos)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_iStatueNum = _iStatueNum;
	m_iTextureNum = _iTextureNum;

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

	return S_OK;
}

_int CTile::Update_GameObject(const _float& fTimeDelta)
{
	CCore* pCore = dynamic_cast<CCore*>(Engine::Get_GameObject(L"Layer_Environment", L"Core"));
	m_bActive = pCore->Get_ActiveCore(m_iStatueNum);

	Add_RenderGroup(RENDER_ALPHA, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CTile::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CTile::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	FAILED_CHECK_RETURN(Setup_Material(), );

	m_pTextureCom->Set_Texture(m_iTextureNum);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	if (m_bActive)
	{
		m_pEmissiveTextureCom->Set_Texture(m_iTextureNum);
		m_pEmissiveBufferCom->Render_Buffer();
	}

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CTile::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CObjectTex*>(Engine::Clone_Proto(L"Proto_TileTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_TileTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	pComponent = m_pEmissiveBufferCom = dynamic_cast<CObjectTex*>(Engine::Clone_Proto(L"Proto_TileTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_EmissiveBuffer", pComponent });

	pComponent = m_pEmissiveTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_TileEmissiveTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_EmissiveTexture", pComponent });

	return S_OK;
}

CTile* CTile::Create(LPDIRECT3DDEVICE9 pGraphicDev, int _iStatueNum, int _iTextureNum, _vec3 vPos)
{
	CTile* pCore = new CTile(pGraphicDev);

	if (FAILED(pCore->Ready_GameObject(_iStatueNum, _iTextureNum, vPos)))
	{
		Safe_Release(pCore);
		MSG_BOX("pCore Create Failed");
		return nullptr;
	}

	return pCore;
}

void CTile::Free()
{
	Engine::CGameObject::Free();
}
