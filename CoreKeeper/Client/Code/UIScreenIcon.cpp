#include "pch.h"
#include "..\Header\UIScreenIcon.h"
#include "Export_System.h"
#include "Export_Utility.h"

#include "..\Header\UIInvPlate.h"
#include "..\Header\UIScreenInv.h"
#include "..\Header\Player.h"
#include "..\Header\UIInventory.h"
#include "..\Header\UIItemSlot.h"
#include "..\Header\UIPlayerStatus.h"
#include "..\Header\UIPlayerStats.h"
#include "..\Header\UIPlayerCraft.h"

CUIScreenIcon::CUIScreenIcon(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bCollapse(false), m_bExit(false), m_bClicked(false), m_bFirst(true)

{
}

CUIScreenIcon::~CUIScreenIcon()
{
}

HRESULT CUIScreenIcon::Ready_GameObject(_vec2 vPos, _vec2 vSize, const _uint iIndex)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	D3DXMatrixIdentity(&ButtonWorld);

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

	m_iIndex = iIndex;

	return S_OK;
}

_int CUIScreenIcon::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (m_bFirst)
	{
		CInventory* pInv = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"Player", L"Com_Inventory"));

		m_vecItem = pInv->Get_VecItem();

		m_bFirst = false;
	}

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	if (Map_Picked(pt))
	{
		if (Engine::Get_DIMouseState(DIM_LB))
		{
			m_bClicked = true;
		}
		else if ((!Engine::Get_DIMouseState(DIM_LB)) && m_bClicked)
		{
			m_bClicked = false;

			CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player"));

			if (m_iIndex == ICON_BAG || m_iIndex == ICON_BAG_COL)
			{
				pPlayer->Set_Inventory();
				pPlayer->Set_Craft();
			}
			else if(m_iIndex == ICON_MAP || m_iIndex == ICON_MAP_COL)
			{
				pPlayer->Set_Map();
			}
			else if (m_iIndex == ICON_HAND && m_bExit)
			{
				pPlayer->Set_Inventory();
				pPlayer->Set_Craft();

				pPlayer->Set_InvWindow();
				pPlayer->Set_CraftWindow();
				pPlayer->Set_MapWindow();

				m_bExit = false;
			}
			//인덱스에 따라 출력되는 창 변경
		}

		if (m_iIndex == ICON_BAG || m_iIndex == ICON_MAP)
		{
			m_bCollapse = true;
		}
	}
	else
		m_bCollapse = false;

	Engine::Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CUIScreenIcon::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIScreenIcon::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	//인벤토리 오픈했을때 출력
	if (m_bExit && (m_iIndex == ICON_HAND))
	{
		m_pTextureCom->Set_Texture(ICON_EXIT);
	}
	else
	{
		if (m_bCollapse)
			m_pTextureCom->Set_Texture(m_iIndex + 1);
		else
			m_pTextureCom->Set_Texture(m_iIndex);
	}
	m_pBufferCom->Render_Buffer();
}

void CUIScreenIcon::Set_Inventory()
{
	CUIScreenIcon* pIcon = dynamic_cast<CUIScreenIcon*>(Engine::Get_GameObject(L"Layer_UI", L"UIScreenicon_Hand"));
	pIcon->Set_Exit();

	CUIInvPlate* pPlate = dynamic_cast<CUIInvPlate*>(Engine::Get_GameObject(L"Layer_UI", L"UI_Plate"));

	pPlate->Set_Render();

	for (int i = 0; i < 10; i++)
	{
		wstring string;

		string = L"UI_ScreenInv_" + std::to_wstring(i);

		CUIScreenInv* pInv = dynamic_cast<CUIScreenInv*>(Engine::Get_GameObject(L"Layer_UI", string.c_str()));

		pInv->Move_Pos();
	}

	CInventory* pPlayer = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"Player", L"Com_Inventory"));

	for (int i = 11; i < pPlayer->Get_Slot() + 1; i++)
	{
		wstring string;

		string = L"UI_Inventory_" + std::to_wstring(i);

		CUIInventory* pInventory = dynamic_cast<CUIInventory*>(Engine::Get_GameObject(L"Layer_UI", string.c_str()));

		pInventory->Set_Show();
	}

	for (int i = 0; i < 10; i++)
	{
		wstring string;

		string = L"UIItemSlot_" + std::to_wstring(i);

		CUIItemSlot* pSlot = dynamic_cast<CUIItemSlot*>(Engine::Get_GameObject(L"Layer_UI", string.c_str()));

		pSlot->Set_Window();
	}

	CUIPlayerStatus* pStatus = dynamic_cast<CUIPlayerStatus*>(Engine::Get_GameObject(L"Layer_UI", L"UIPlayerStatus"));
	pStatus->Set_Window();

	CUIPlayerStats* pStats = dynamic_cast<CUIPlayerStats*>(Engine::Get_GameObject(L"Layer_UI", L"UIPlayerStats"));
	pStats->Set_Window();

}

void CUIScreenIcon::Set_Map()
{


}

void CUIScreenIcon::Set_Craft()
{
	CUIPlayerCraft* pCraft = dynamic_cast<CUIPlayerCraft*>(Engine::Get_GameObject(L"Layer_UI", L"UIPlayerCraft"));
	pCraft->Set_Window();

}

HRESULT CUIScreenIcon::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIIconTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });

	return S_OK;
}

CUIScreenIcon* CUIScreenIcon::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize, const _uint iIndex)
{
	CUIScreenIcon* pUIScreenIcon = new CUIScreenIcon(pGraphicDev);

	if (FAILED(pUIScreenIcon->Ready_GameObject(vPos, vSize, iIndex)))
	{
		Safe_Release(pUIScreenIcon);
		MSG_BOX("UIScreenIcon Create Failed");
		return nullptr;
	}

	return pUIScreenIcon;
}

void CUIScreenIcon::Free()
{
	Engine::CGameObject::Free();
}
