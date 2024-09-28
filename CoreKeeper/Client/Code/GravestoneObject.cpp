#include "pch.h"
#include "../Header/GravestoneObject.h"
#include "Export_System.h"
#include "Export_Utility.h"

CGravestoneObject::CGravestoneObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: CObject(pGraphicDev)
{
}

CGravestoneObject::~CGravestoneObject()
{
}

HRESULT CGravestoneObject::Ready_GameObject(_vec3 vPos)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

	return S_OK;
}

_int CGravestoneObject::Update_GameObject(const _float& fTimeDelta)
{
	Add_RenderGroup(RENDER_ALPHA, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CGravestoneObject::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CGravestoneObject::Render_GameObject()
{
	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CGravestoneObject::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CObjectTex*>(Engine::Clone_Proto(L"Proto_ObjectTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_GravestoneTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	pComponent = m_pInventoryCom = dynamic_cast<CInventory*>(Engine::Clone_Proto(L"Proto_GravestoneInventory"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Inventory", pComponent });

	return S_OK;
}

CGravestoneObject* CGravestoneObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
	CGravestoneObject* pCore = new CGravestoneObject(pGraphicDev);

	if (FAILED(pCore->Ready_GameObject(vPos)))
	{
		Safe_Release(pCore);
		MSG_BOX("pCore Create Failed");
		return nullptr;
	}

	return pCore;
}

void CGravestoneObject::Free()
{
	Engine::CGameObject::Free();
}
