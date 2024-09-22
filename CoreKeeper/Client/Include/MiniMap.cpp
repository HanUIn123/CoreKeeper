#include "pch.h"
#include "..\Header\MiniMap.h"
#include "..\Header\Export_System.h"
#include "..\Header\Export_Utility.h"

CMiniMap::CMiniMap(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CMiniMap::~CMiniMap()
{
}

HRESULT CMiniMap::Ready_GameObject(_vec2 vPos, _vec2 vSize)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	_D3DVIEWPORT9 MMapViewport;

	MMapViewport.Height = 300.f;
   // MMapViewport.MaxZ = 

	return S_OK;
}

_int CMiniMap::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	return iExit;
}

void CMiniMap::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CMiniMap::Render_GameObject()
{
	_matrix matWorld;

	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pBufferCom->Render_Buffer();
}

HRESULT CMiniMap::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });

	return S_OK;
}

CMiniMap* CMiniMap::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize)
{
	CMiniMap* pMiniMap = new CMiniMap(pGraphicDev);

	if (FAILED(pMiniMap->Ready_GameObject(vPos, vSize)))
	{
		Safe_Release(pMiniMap);
		MSG_BOX("UIStatus Create Failed");
		return nullptr;
	}

	return pMiniMap;
}

void CMiniMap::Free()
{
	Engine::CGameObject::Free();
}
