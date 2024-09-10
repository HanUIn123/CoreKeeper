#include "pch.h"
#include "..\Header\UIStatus.h"
#include "Export_System.h"
#include "Export_Utility.h"

CUIStatus::CUIStatus(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)

{
}

CUIStatus::~CUIStatus()
{
}

HRESULT CUIStatus::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

_int CUIStatus::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);
	
	m_pTransformCom->m_vScale = { 100.f, 100.f, 1.f };

	m_pTransformCom->Set_Pos(100.f, 100.f, 0.f);

	Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CUIStatus::LateUpdate_GameObject()
{
	
	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIStatus::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CUIStatus::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_AnimTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	
	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });
	return S_OK;
}

CUIStatus* CUIStatus::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CUIStatus* pUIStatus = new CUIStatus(pGraphicDev);

	if (FAILED(pUIStatus->Ready_GameObject()))
	{
		Safe_Release(pUIStatus);
		MSG_BOX("pPlayer Create Failed");
		return nullptr;
	}

	return pUIStatus;
}

void CUIStatus::Free()
{
	Engine::CGameObject::Free();
}
