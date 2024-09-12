#include "pch.h"
#include "..\Header\UIScreenInv.h"
#include "Export_System.h"
#include "Export_Utility.h"

CUIScreenInv::CUIScreenInv(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_iCurInv(2)

{
}

CUIScreenInv::~CUIScreenInv()
{
}

HRESULT CUIScreenInv::Ready_GameObject(_vec2 vPos, _int _iIndex)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	_D3DVIEWPORT9 Viewport;

	m_pGraphicDev->GetViewport(&Viewport);
	float height = (_float)Viewport.Height;
	float width = (_float)Viewport.Width;

	float x = vPos.x - width / 2;
	float y = height / 2 - vPos.y;

	m_pTransformCom->Set_Pos(x, y, 0);

	_vec2 vSize = { 20.f, 20.f };

	m_pTransformCom->m_vScale = { vSize.x, vSize.y, 1.f };

	m_BRect.left = vPos.x - vSize.x / 2;
	m_BRect.right = vPos.x + vSize.x / 2;
	m_BRect.top = vPos.y - vSize.y / 2;
	m_BRect.bottom = vPos.y + vSize.y / 2;

	m_iIndex = _iIndex;


	return S_OK;
}

_int CUIScreenInv::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);


	if (Map_Picked(pt))
	{
		if (Engine::Get_DIMouseState(DIM_LB))
		{
			m_iCurInv = m_iIndex;
		}
	}

	Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CUIScreenInv::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIScreenInv::Render_GameObject()
{

	_matrix matWorld;
	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);


	if (m_iCurInv == m_iIndex)
	{
		m_pTextureCom->Set_Texture(1);
	}
	else
		m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	m_pItemTextureCom->Set_Texture(0);

	matWorld._11 = 10.f;
	matWorld._22 = 10.f;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pBufferCom->Render_Buffer();
	
}

HRESULT CUIScreenInv::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIScreenInvTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pItemTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ItemTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_UITexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });
	return S_OK;
}

CUIScreenInv* CUIScreenInv::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _int _iIndex)
{
	CUIScreenInv* pUIScreenInv = new CUIScreenInv(pGraphicDev);

	if (FAILED(pUIScreenInv->Ready_GameObject(vPos, _iIndex)))
	{
		Safe_Release(pUIScreenInv);
		MSG_BOX("UIScreenInv Create Failed");
		return nullptr;
	}

	return pUIScreenInv;
}

void CUIScreenInv::Free()
{
	Engine::CGameObject::Free();
}
