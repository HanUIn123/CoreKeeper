#include "pch.h"
#include "..\Header\UIItemSlot.h"
#include "Export_System.h"
#include "Export_Utility.h"

CUIItemSlot::CUIItemSlot(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bCollapse(false), m_bFirst(false), m_bWindow(false), m_iIndex(0), m_pItem(nullptr)

{
}

CUIItemSlot::~CUIItemSlot()
{
}

HRESULT CUIItemSlot::Ready_GameObject(_vec2 vPos, _vec2 vSize, SLOTTYPE _eType)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	_D3DVIEWPORT9 Viewport;

	m_pGraphicDev->GetViewport(&Viewport);
	float height = (_float)Viewport.Height;
	float width = (_float)Viewport.Width;

	float x = vPos.x - width / 2;
	float y = height / 2 - vPos.y;

	m_pTransformCom->Set_Scale(vSize.x, vSize.y , 1.f);
	m_pTransformCom->Set_Pos(x, y, 0);


	m_BRect.left = vPos.x - vSize.x;
	m_BRect.right = vPos.x + vSize.x;
	m_BRect.top = vPos.y - vSize.y;
	m_BRect.bottom = vPos.y + vSize.y;

	m_eSlotType = _eType;

	switch (m_eSlotType)
	{
	case SLOT_HELM:
		m_iIndex = 0;
		eSlotItemType = ITEM_HELMET;
		break;

	case SLOT_CHEST:
		m_iIndex = 1;
		eSlotItemType = ITEM_CHEST;
		break;

	case SLOT_LEGGINGS:
		m_iIndex = 5;
		eSlotItemType = ITEM_LEG;
		break;

	case SLOT_WEAPON:
		m_iIndex = 2;
		eSlotItemType = ITEM_ASSISTANCE;
		break;

	case SLOT_NECKLACE:
		m_iIndex = 6;
		eSlotItemType = ITEM_NECKLACE;
		break;

	case SLOT_RING1:
		m_iIndex = 7;
		eSlotItemType = ITEM_RING;
		break;

	case SLOT_RING2:
		m_iIndex = 7;
		eSlotItemType = ITEM_RING;
		break;

	case SLOT_BAG:
		m_iIndex = 3;
		eSlotItemType = ITEM_BAG;
		break;

	case SLOT_LANTERN:
		m_iIndex = 4;
		eSlotItemType = ITEM_LANTERN;
		break;

	case SLOT_PET:
		m_iIndex = 8;
		eSlotItemType = ITEM_PET;
		break;

	default:
		break;
	}


	return S_OK;
}

_int CUIItemSlot::Update_GameObject(const _float& fTimeDelta)
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

			if (Engine::Button_Down(DIM_LB))
			{
				CInventory* pCursorInv = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_UI", L"UI_Cursor", L"Com_Inventory"));
				CInventory* pPlayerInv = dynamic_cast<Engine::CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"Player", L"Com_EquipInventory"));

				CItem* pItem = pCursorInv->Get_Item(0);
	
				ITEMNUM eNum = ITEM_ETC;

				if(pItem)
					eNum = pItem->Get_ItemNum();

				if (eSlotItemType == eNum)
				{
					vector<CItem*>* pCvecItem = pCursorInv->Get_VecItemP();
					vector<CItem*>* pPvecItem = pPlayerInv->Get_VecItemP();

					pPlayerInv->Swap_Item(&(*pCvecItem)[0], &(*pPvecItem)[m_eSlotType]);
				}
				else if (pCursorInv->Check_Empty(0) && !pPlayerInv->Check_Empty(m_eSlotType))
				{
					vector<CItem*>* pCvecItem = pCursorInv->Get_VecItemP();
					vector<CItem*>* pPvecItem = pPlayerInv->Get_VecItemP();
					(*pPvecItem)[m_eSlotType]->Set_Follow(false);
					pPlayerInv->Swap_Item(&(*pCvecItem)[0], &(*pPvecItem)[m_eSlotType]);
				}
			}
		}
		else
			m_bCollapse = false;

	}
	return iExit;
}

void CUIItemSlot::LateUpdate_GameObject()
{
	if (m_bWindow)
	{
		Engine::Add_RenderGroup(RENDER_UI, this);
	}
	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIItemSlot::Render_GameObject()	
{
	_matrix matWorld;

	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	if (m_bCollapse)
	{
		m_pColTextureCom->Set_Texture();
	}
	m_pBufferCom->Render_Buffer();

	CInventory* pPlayerInv = dynamic_cast<Engine::CInventory*>
		(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"Player", L"Com_EquipInventory"));

	m_pItem = pPlayerInv->Get_Item(m_eSlotType);

	if (!pPlayerInv->Check_Empty(m_eSlotType))
	{
		Engine::MATERIAL material = m_pItem->Get_ItemMaterial();

		m_pItem->Get_Texture()->Set_Texture(m_pItem->Get_TextureNumber());

		Engine::ITEMNUM eNum = m_pItem->Get_ItemNum();

		switch (eNum)
		{
		case ITEM_SWORD:
			matWorld._11 = 40.f;
			matWorld._22 = 40.f;

			matWorld._42 -= 8.f;
			break;

		case ITEM_WOOD:
			matWorld._11 = 12.f;
			matWorld._22 = 12.f;
			break;

		case ITEM_BOW:
			matWorld._11 = 45.f;
			matWorld._22 = 45.f;

			matWorld._42 += 2.f;
			break;

		case ITEM_HOE:
			matWorld._11 = 50.f;
			matWorld._22 = 50.f;

			matWorld._42 -= 8.f;
			break;

		case ITEM_PICKAXE:
			matWorld._11 = 50.f;
			matWorld._22 = 50.f;

			matWorld._42 -= 8.f;
			break;

			//case ITEM_SHOVEL:
			//	matWorld._11 = 60.f;
			//	matWorld._22 = 60.f;

			//	matWorld._42 -= 8.f;
			//	break;

		case ITEM_STAFF:
			matWorld._11 = 35.f;
			matWorld._22 = 35.f;

			matWorld._42 -= 8.f;
			break;

		case ITEM_HELMET:
			matWorld._11 = 20.f;
			matWorld._22 = 20.f;
			break;

		case ITEM_CHEST:
			matWorld._11 = 20.f;
			matWorld._22 = 20.f;
			break;

		case ITEM_LEG:
			matWorld._11 = 20.f;
			matWorld._22 = 20.f;
			break;

		case ITEM_COPPER:
			matWorld._11 = 20.f;
			matWorld._22 = 20.f;
			break;

		case ITEM_IRON:
			matWorld._11 = 20.f;
			matWorld._22 = 20.f;
			break;

		case ITEM_SCARLET:
			matWorld._11 = 20.f;
			matWorld._22 = 20.f;
			break;

		case ITEM_COPPER_BAR:
			matWorld._11 = 20.f;
			matWorld._22 = 20.f;
			break;

		case ITEM_IRON_BAR:
			matWorld._11 = 20.f;
			matWorld._22 = 20.f;
			break;

		case ITEM_SCARLET_BAR:
			matWorld._11 = 20.f;
			matWorld._22 = 20.f;
			break;

		case ITEM_TORCH:
			matWorld._11 = 25.f;
			matWorld._22 = 25.f;
			break;

		case ITEM_INSTRUMENT_PIANO:
			matWorld._11 = 25.f;
			matWorld._22 = 25.f;
			break;

		case ITEM_INSTRUMENT_CELLO:
			matWorld._11 = 25.f;
			matWorld._22 = 25.f;
			break;

		case ITEM_INSTRUMENT_HARP:
			matWorld._11 = 25.f;
			matWorld._22 = 25.f;
			break;

		case ITEM_INSTRUMENT_FLUTE:
			matWorld._11 = 25.f;
			matWorld._22 = 25.f;
			break;

		case ITEM_INSTRUMENT_OCARINA:
			matWorld._11 = 25.f;
			matWorld._22 = 25.f;
			break;

		case ITEM_INSTRUMENT_DRUM:
			matWorld._11 = 25.f;
			matWorld._22 = 25.f;
			break;

		default:
			matWorld._11 = 15.f;
			matWorld._22 = 15.f;
			break;
		}


		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

		m_pItem->Get_Buffer()->Render_First();
	}
	else
	{
		m_pSlotBufferCom->Set_Index(m_iIndex);

		m_pSlotTextureCom->Set_Texture();

		m_pSlotBufferCom->Render_Buffer();
	}
	//m_pTextureCom->Set_Texture();

}

HRESULT CUIItemSlot::Add_Component()
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

CUIItemSlot* CUIItemSlot::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize, SLOTTYPE _eType)
{
	CUIItemSlot* pUIItemslot = new CUIItemSlot(pGraphicDev);

	if (FAILED(pUIItemslot->Ready_GameObject(vPos, vSize, _eType)))
	{
		Safe_Release(pUIItemslot);
		MSG_BOX("UIStatus Create Failed");
		return nullptr;
	}

	return pUIItemslot;
}

void CUIItemSlot::Free()
{
	Engine::CGameObject::Free();
}
