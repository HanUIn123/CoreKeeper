#include "pch.h"
#include "..\Header\UICraftSlot.h"
#include "Export_System.h"
#include "Export_Utility.h"

CUICraftSlot::CUICraftSlot(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bCollapse(false), m_bFirst(false), m_bWindow(false), m_iIndex(0)

{
}

CUICraftSlot::~CUICraftSlot()
{
}

HRESULT CUICraftSlot::Ready_GameObject(_vec2 vPos, _vec2 vSize, CSLOTTYPE _eType)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	_D3DVIEWPORT9 Viewport;

	m_pGraphicDev->GetViewport(&Viewport);
	float height = (_float)Viewport.Height;
	float width = (_float)Viewport.Width;

	float x = vPos.x - width / 2;
	float y = height / 2 - vPos.y;

	m_pTransformCom->m_vScale = { vSize.x, vSize.y , 1.f };
	m_pTransformCom->Set_Pos(x, y, 0);


	m_BRect.left = vPos.x - vSize.x / 2;
	m_BRect.right = vPos.x + vSize.x / 2;
	m_BRect.top = vPos.y - vSize.y / 2;
	m_BRect.bottom = vPos.y + vSize.y / 2;

	m_eSlotType = _eType;

	return S_OK;
}

_int CUICraftSlot::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (m_bWindow)
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(g_hWnd, &pt);

		if (Map_Picked(pt))
		{
			m_bCollapse = true;

			if (Engine::Get_DIMouseState(DIM_LB))
			{
				switch (m_eSlotType)
				{
				
				default:
					break;
				}
			}

		}
		else
			m_bCollapse = false;

	}
	return iExit;
}

void CUICraftSlot::LateUpdate_GameObject()
{
	if (m_bWindow)
	{
		Engine::Add_RenderGroup(RENDER_UI, this);
	}
	Engine::CGameObject::LateUpdate_GameObject();
}

void CUICraftSlot::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	if (m_bCollapse)
	{
		m_pColTextureCom->Set_Texture();
	}
	m_pBufferCom->Render_Buffer();

	/*
	m_pSlotBufferCom->Set_Index(m_iIndex);

	m_pSlotTextureCom->Set_Texture();

	m_pSlotBufferCom->Render_Buffer();
	*/

}

HRESULT CUICraftSlot::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UISlot"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pSlotBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_UISilhouettes"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_SlotBuffer", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });

	pComponent = m_pColTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIInvSelected"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_ColTexture", pComponent });

	pComponent = m_pSlotTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UISilhouette"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_SlotTexture", pComponent });

	return S_OK;
}

CUICraftSlot* CUICraftSlot::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize, CSLOTTYPE _eType)
{
	CUICraftSlot* pUICraftSlot = new CUICraftSlot(pGraphicDev);

	if (FAILED(pUICraftSlot->Ready_GameObject(vPos, vSize, _eType)))
	{
		Safe_Release(pUICraftSlot);
		MSG_BOX("UIStatus Create Failed");
		return nullptr;
	}

	return pUICraftSlot;
}

void CUICraftSlot::Free()
{
	Engine::CGameObject::Free();
}
