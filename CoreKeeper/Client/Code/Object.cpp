#include "pch.h"
#include "../Header/Object.h"
#include "Export_Utility.h"

CObject::CObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_pTransformCom(nullptr)
	, m_pBufferCom(nullptr)
	, m_iBuildingImgNum(0)
{
}

CObject::~CObject()
{
}

HRESULT CObject::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

_int CObject::Update_GameObject(const _float& fTimeDelta)
{
	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CObject::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CObject::Render_GameObject()
{

}

HRESULT CObject::Add_Component()
{
	return S_OK;
}

HRESULT CObject::Setup_Material()
{
	D3DMATERIAL9		tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = { 1.f, 1.f, 1.f, 1.f };
	tMtrl.Specular = { 1.f, 1.f, 1.f, 1.f };
	tMtrl.Ambient = { 0.7f, 0.7f, 0.7f, 0.7f };

	tMtrl.Emissive = { 0.2f, 0.2f, 0.2f, 0.2f };
	tMtrl.Power = 0.f;

	m_pGraphicDev->SetMaterial(&tMtrl);

	return S_OK;
}

CObject* CObject::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CObject* pObject = new CObject(pGraphicDev);

	if (FAILED(pObject->Ready_GameObject()))
	{
		Safe_Release(pObject);
		MSG_BOX("pBuilding Create Failed");
		return nullptr;
	}

	return pObject;
}

void CObject::Free()
{
	Engine::CGameObject::Free();
}
