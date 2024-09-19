#include "pch.h"
#include "..\Header\UICursor.h"
#include "Export_System.h"
#include "Export_Utility.h"

CUICursor::CUICursor(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bDisable(false)

{
}

CUICursor::~CUICursor()
{
}

HRESULT CUICursor::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(10.f, 10.f, 0);

	return S_OK;
}

_int CUICursor::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (!m_bDisable)
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(g_hWnd, &pt);

		_D3DVIEWPORT9 Viewport;

		m_pGraphicDev->GetViewport(&Viewport);
		float height = (_float)Viewport.Height;
		float width = (_float)Viewport.Width;

		float x = (_float)pt.x - width / 2 + 6.f;
		float y = height / 2 - (_float)pt.y - 6.f;

		m_pTransformCom->Set_Pos(x, y, 0);
	}

	return iExit;
}

void CUICursor::LateUpdate_GameObject()
{
	if (!m_bDisable)
	{
		Add_RenderGroup(RENDER_UI, this);
	}

	Engine::CGameObject::LateUpdate_GameObject();
}

void CUICursor::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	
	m_pTextureCom->Set_Texture();
	
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

HRESULT CUICursor::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UICursor"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });

	pComponent = m_pInventoryCom = dynamic_cast<CInventory*>(Engine::Clone_Proto(L"Proto_MouseInventory"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Inventory", pComponent });

	return S_OK;
}

CUICursor* CUICursor::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CUICursor* pUICursor = new CUICursor(pGraphicDev);

	if (FAILED(pUICursor->Ready_GameObject()))
	{
		Safe_Release(pUICursor);
		MSG_BOX("pUICursor Create Failed");
		return nullptr;
	}

	return pUICursor;
}

void CUICursor::Free()
{
	Engine::CGameObject::Free();
}
