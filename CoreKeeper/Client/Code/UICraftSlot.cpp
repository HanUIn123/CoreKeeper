#include "pch.h"
#include "..\Header\UICraftSlot.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\Torch.h"
#include "..\Header\Stage.h"
#include "..\Header\CraftMgr.h"
#include "..\Header\UIItemFrame.h"

CUICraftSlot::CUICraftSlot(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bCollapse(false), m_bWindow(false), m_iIndex(0), m_bEnough(false), m_bStay(false)

{
}

CUICraftSlot::~CUICraftSlot()
{
}

HRESULT CUICraftSlot::Ready_GameObject(_vec2 vPos, _vec2 vSize, _int _iIndex, _bool bDirection)
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

	m_iIndex = _iIndex;

	m_bDirection = bDirection;

	Ready_Table(); // 檣策蝶縑 蜃朝 嬴檜蠱擊 map縑 厥橫邀

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

		CInventory* pPlayerInv = dynamic_cast<Engine::CInventory*>
			(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"Player", L"Com_Inventory"));

		if (m_eItemType.eItemNum == ITEM_RING || m_eItemType.eItemNum == ITEM_NECKLACE)
		{
			MATERIAL eMat = MATERIAL_END;

			switch (m_eItemType.iTextureNum)
			{
			case 43:
				eMat = MATERIAL_COPPER;
				break;

			case 44:
				eMat = MATERIAL_IRON;
				break;

			case 46:
				eMat = MATERIAL_COPPER;
				break;

			case 47:
				eMat = MATERIAL_IRON;
				break;
			}

			if (CCraftMgr::GetInstance()->Craftable(pPlayerInv, m_eItemType.eItemNum, eMat))
			{
				m_bEnough = true;
			}
			else
				m_bEnough = false;
		}
		else if (m_eItemType.eItemNum == ITEM_TABLE)
		{
			MATERIAL eMat = MATERIAL_END;

			switch (m_eItemType.iTextureNum)
			{
			case 2:
				eMat = MATERIAL_WOOD;
				break;
			case 12:
				eMat = MATERIAL_COPPER;
				break;

			case 19:
				eMat = MATERIAL_IRON;
				break;
			}

			if (CCraftMgr::GetInstance()->Craftable(pPlayerInv, m_eItemType.eItemNum, eMat))
			{
				m_bEnough = true;
			}
			else
				m_bEnough = false;
		}
		else if (m_eItemType.eItemNum == ITEM_ASSISTANCE)
		{
			MATERIAL eMat = MATERIAL_END;

			switch (m_eItemType.iTextureNum)
			{
			case 31:
				eMat = MATERIAL_WOOD;
				break;
			case 37:
				eMat = MATERIAL_IRON;
				break;
			}

			if (CCraftMgr::GetInstance()->Craftable(pPlayerInv, m_eItemType.eItemNum, eMat))
			{
				m_bEnough = true;
			}
			else
				m_bEnough = false;
		}
		else if (m_eItemType.eItemNum == ITEM_WATERINGCAN)
		{
			if (m_eItemType.eItemMat == MATERIAL_WOOD)
			{
				if (CCraftMgr::GetInstance()->Craftable(pPlayerInv, m_eItemType.eItemNum, MATERIAL_COPPER))
				{
					m_bEnough = true;
				}
				else
					m_bEnough = false;
			}
			else
			{
				if (CCraftMgr::GetInstance()->Craftable(pPlayerInv, m_eItemType.eItemNum, MATERIAL_IRON))
				{
					m_bEnough = true;
				}
				else
					m_bEnough = false;
			}
		}
		else
		{
			if (CCraftMgr::GetInstance()->Craftable(pPlayerInv, m_eItemType.eItemNum, m_eItemType.eItemMat))
			{
				m_bEnough = true;
			}
			else
				m_bEnough = false;
		}

		if (Map_Picked(pt))
		{
			m_bCollapse = true;

			if (!m_bStay)
			{
				if (m_eItemType.eItemNum == ITEM_RING || m_eItemType.eItemNum == ITEM_NECKLACE)
				{
					MATERIAL eMat = MATERIAL_END;

					switch (m_eItemType.iTextureNum)
					{
					case 43:
						eMat = MATERIAL_COPPER;
						break;

					case 44:
						eMat = MATERIAL_IRON;
						break;

					case 46:
						eMat = MATERIAL_COPPER;
						break;

					case 47:
						eMat = MATERIAL_IRON;
						break;
					}

					m_pInventoryCom->Add_Item(CCraftMgr::GetInstance()->CraftExp(m_eItemType.eItemNum, eMat));
				}
				else if (m_eItemType.eItemNum == ITEM_TABLE)
				{
					MATERIAL eMat = MATERIAL_END;

					switch (m_eItemType.iTextureNum)
					{
					case 2:
						eMat = MATERIAL_WOOD;
						break;

					case 12:
						eMat = MATERIAL_COPPER;
						break;

					case 19:
						eMat = MATERIAL_IRON;
						break;
					}

					m_pInventoryCom->Add_Item(CCraftMgr::GetInstance()->CraftExp(m_eItemType.eItemNum, eMat));
				}
				else if (m_eItemType.eItemNum == ITEM_WATERINGCAN)
				{
					if(m_eItemType.eItemMat == MATERIAL_WOOD)
						m_pInventoryCom->Add_Item(CCraftMgr::GetInstance()->CraftExp(m_eItemType.eItemNum, MATERIAL_COPPER));
					if (m_eItemType.eItemMat == MATERIAL_IRON)
						m_pInventoryCom->Add_Item(CCraftMgr::GetInstance()->CraftExp(m_eItemType.eItemNum, MATERIAL_IRON));
				}
				else if (m_eItemType.eItemNum == ITEM_ASSISTANCE)
				{
					MATERIAL eMat = MATERIAL_END;

					switch (m_eItemType.iTextureNum)
					{
					case 31:
						eMat = MATERIAL_WOOD;
						break;
					case 37:
						eMat = MATERIAL_IRON;
						break;
					}

					m_pInventoryCom->Add_Item(CCraftMgr::GetInstance()->CraftExp(m_eItemType.eItemNum, eMat));
				}
				else
				{
					m_pInventoryCom->Add_Item(CCraftMgr::GetInstance()->CraftExp(m_eItemType.eItemNum, m_eItemType.eItemMat));
				}

				CUIItemFrame* pFrame = dynamic_cast<CUIItemFrame*>(Engine::Get_GameObject(L"Layer_UI", L"UI_ItemFrame"));

				pFrame->Set_Window(m_pInventoryCom->Get_Item(0), pt, true);
				
				m_bStay = true;
			}

			if (Engine::Button_Down(DIM_LB))
			{
				if (m_bEnough)
				{
					if (m_eItemType.eItemNum == ITEM_RING || m_eItemType.eItemNum == ITEM_NECKLACE)
					{
						MATERIAL eMat = MATERIAL_END;

						switch (m_eItemType.iTextureNum)
						{
						case 43:
							eMat = MATERIAL_COPPER;
							break;

						case 44:
							eMat = MATERIAL_IRON;
							break;

						case 46:
							eMat = MATERIAL_COPPER;
							break;

						case 47:
							eMat = MATERIAL_IRON;
							break;
						}

						CInventory* pCursorInv = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_UI", L"UI_Cursor", L"Com_Inventory"));

						pCursorInv->Add_Item(CCraftMgr::GetInstance()->Craft(pPlayerInv, m_eItemType.eItemNum, eMat));
						Engine::CSoundMgr::GetInstance()->Play(L"uiPickup.wav", SOUND_UI_INVENTORY, 0.2f);
					}
					else if (m_eItemType.eItemNum == ITEM_TABLE)
					{
						MATERIAL eMat = MATERIAL_END;

						switch (m_eItemType.iTextureNum)
						{
						case 2:
							eMat = MATERIAL_WOOD;
							break;
						case 12:
							eMat = MATERIAL_COPPER;
							break;

						case 19:
							eMat = MATERIAL_IRON;
							break;
						}

						CInventory* pCursorInv = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_UI", L"UI_Cursor", L"Com_Inventory"));

						pCursorInv->Add_Item(CCraftMgr::GetInstance()->Craft(pPlayerInv, m_eItemType.eItemNum, eMat));
						Engine::CSoundMgr::GetInstance()->Play(L"uiPickup.wav", SOUND_UI_INVENTORY, 0.2f);
					}
					else if (m_eItemType.eItemNum == ITEM_ASSISTANCE)
					{
						MATERIAL eMat = MATERIAL_END;

						switch (m_eItemType.iTextureNum)
						{
						case 31:
							eMat = MATERIAL_WOOD;
							break;
						case 37:
							eMat = MATERIAL_IRON;
							break;
						}

						CInventory* pCursorInv = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_UI", L"UI_Cursor", L"Com_Inventory"));

						pCursorInv->Add_Item(CCraftMgr::GetInstance()->Craft(pPlayerInv, m_eItemType.eItemNum, eMat));
						Engine::CSoundMgr::GetInstance()->Play(L"uiPickup.wav", SOUND_UI_INVENTORY, 0.2f);
					}
					else if (m_eItemType.eItemNum == ITEM_WATERINGCAN)
					{
						CInventory* pCursorInv = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_UI", L"UI_Cursor", L"Com_Inventory"));

						if (m_eItemType.eItemMat == MATERIAL_WOOD)
						{
							pCursorInv->Add_Item(CCraftMgr::GetInstance()->Craft(pPlayerInv, m_eItemType.eItemNum, MATERIAL_COPPER));
							Engine::CSoundMgr::GetInstance()->Play(L"uiPickup.wav", SOUND_UI_INVENTORY, 0.2f);
						}
						else if (m_eItemType.eItemMat == MATERIAL_IRON)
						{
							pCursorInv->Add_Item(CCraftMgr::GetInstance()->Craft(pPlayerInv, m_eItemType.eItemNum, MATERIAL_IRON));
							Engine::CSoundMgr::GetInstance()->Play(L"uiPickup.wav", SOUND_UI_INVENTORY, 0.2f);
						}
					}
					/*else if (m_eItemType.eItemNum == ITEM_POTION_HP || m_eItemType.eItemNum == ITEM_POTION_ATT || m_eItemType.eItemNum == ITEM_POTION_DEF)
					{
						CInventory* pCursorInv = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_UI", L"UI_Cursor", L"Com_Inventory"));

						pCursorInv->Add_Item(CCraftMgr::GetInstance()->Craft(pPlayerInv, m_eItemType.eItemNum, MATERIAL_END));
					}*/
					else
					{
						CInventory* pCursorInv = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_UI", L"UI_Cursor", L"Com_Inventory"));

						pCursorInv->Add_Item(CCraftMgr::GetInstance()->Craft(pPlayerInv, m_eItemType.eItemNum, m_eItemType.eItemMat));
						Engine::CSoundMgr::GetInstance()->Play(L"uiPickup.wav", SOUND_UI_INVENTORY, 0.2f);
					}
				}	
			}

		}
		else
		{
			if (m_bCollapse)
			{
				CUIItemFrame* pItemF = dynamic_cast<CUIItemFrame*>(Engine::Get_GameObject(L"Layer_UI", L"UI_ItemFrame"));

				pItemF->Set_WindowDis();

				m_pInventoryCom->Remove_Item(0);

				m_bStay = false;
			}
			m_bCollapse = false;
		}
	}
	else if (!m_bWindow && m_bCollapse)
	{
		CUIItemFrame* pItemF = dynamic_cast<CUIItemFrame*>(Engine::Get_GameObject(L"Layer_UI", L"UI_ItemFrame"));

		pItemF->Set_WindowDis();

		m_pInventoryCom->Remove_Item(0);

		m_bStay = false;

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
	_matrix matWorld;

	m_pTransformCom->Get_WorldMatrix(&matWorld);

	if (m_bDirection == true)
	{
		switch (m_eTableType)
		{
		case TABLE_CRAFT:
			matWorld._41 += 48.f;
			break;
		case TABLE_ANVIL:
			matWorld._41 += 48.f;
			break;
		case TABLE_ALCHEMY:
			matWorld._41 += 172.f;
			break;
		case TABLE_ACCESSORY:
			matWorld._41 += 172.f;
			break;
		case TABLE_MUSIC:
			matWorld._41 += 172.f;
			break;
		}
	}

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	if (m_bCollapse)
	{
		m_pColTextureCom->Set_Texture();
	}
	m_pBufferCom->Render_Buffer();

	matWorld._11 -= 3.f;
	matWorld._22 -= 3.f;

	if (!m_bEnough)
	{
		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

		m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

		m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(100, 255, 255, 255));
		m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
		m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		m_pItemTextureCom->Set_Texture(m_eItemType.iTextureNum);

		m_pBufferCom->Render_Buffer();

		m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);

		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

		m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	}
	else if (m_bEnough)
	{
		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

		m_pItemTextureCom->Set_Texture(m_eItemType.iTextureNum);

		m_pBufferCom->Render_Buffer();
	}

	if (m_eSlotType == UCITEM_TORCH)
	{
		wstring sFont = std::to_wstring(3);

		const _tchar* tFont = sFont.c_str();

		_vec2 pos(m_BRect.right - 15.f, m_BRect.top + 30.f);

		Engine::Render_Font(L"Font_Item", tFont, &pos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	}
}

void CUICraftSlot::Set_Window(TABLETYPE _eTableType, MATERIAL _eMaterial, _bool _bDirection)
{
	if (m_bWindow)
	{
		if (_bDirection == true)
		{
			switch (_eTableType)
			{
			case TABLE_CRAFT:
			{
				m_BRect.left -= 48.f;
				m_BRect.right -= 48.f;
				break;
			}
			case TABLE_ANVIL:
			{
				m_BRect.left -= 48.f;
				m_BRect.right -= 48.f;
				break;
			}
			case TABLE_ALCHEMY:
			{
				m_BRect.left -= 172.f;
				m_BRect.right -= 172.f;
				break;
			}
			case TABLE_ACCESSORY:
			{
				m_BRect.left -= 172.f;
				m_BRect.right -= 172.f;
				break;
			}
			case TABLE_MUSIC:
			{
				m_BRect.left -= 172.f;
				m_BRect.right -= 172.f;
				break;
			}
			}
		}
	}
	else
	{
		if (_bDirection == true)
		{
			switch (_eTableType)
			{
			case TABLE_CRAFT:
			{
				m_BRect.left += 48.f;
				m_BRect.right += 48.f;
				break;
			}
			case TABLE_ANVIL:
			{
				m_BRect.left += 48.f;
				m_BRect.right += 48.f;
				break;
			}
			case TABLE_ALCHEMY:
			{
				m_BRect.left += 172.f;
				m_BRect.right += 172.f;
				break;
			}
			case TABLE_ACCESSORY:
			{
				m_BRect.left += 172.f;
				m_BRect.right += 172.f;
				break;
			}
			case TABLE_MUSIC:
			{
				m_BRect.left += 172.f;
				m_BRect.right += 172.f;
				break;
			}
			}
		}
	}

	if (m_bWindow)
	{
		m_bWindow = false;
	}
	else if (!m_bWindow)
	{

		m_eTableType = _eTableType;

		m_eMatrial = _eMaterial;

		m_bWindow = true;

		m_bDirection = _bDirection;

		auto iter = mapItemType.find({ _eTableType, _bDirection });

		if (iter != mapItemType.end() && iter->second.eItemMat != _eMaterial && iter->second.eItemMat != MATERIAL_END)
		{
			map<pair<TABLETYPE, _bool>, UIITEM>::iterator seconditer = mapItemType.find({ _eTableType, _bDirection });

			if (iter->second.eItemMat > _eMaterial)
			{
				m_bWindow = false;
				return;
			}

			for (; seconditer != mapItemType.end(); seconditer++)
			{
				if (seconditer->second.eItemMat == _eMaterial && seconditer->first.second == _bDirection)
					break;
			}

			if (seconditer == mapItemType.end())
			{
				m_bWindow = false;

				return;
			}

			m_eItemType = seconditer->second;
		}
		else
		{
			if (iter == mapItemType.end())
			{
				m_bWindow = false;

				return;
			}

			m_eItemType = iter->second;
		}

		if (iter == mapItemType.end())
		{
			m_bWindow = false;

			return;
		}
	}
}

void CUICraftSlot::Ready_Table()
{
	switch (m_iIndex)
	{
	case 0:
	{
		//謝難

		UIITEM TORCH = { m_iIndex, ITEM_TORCH, MATERIAL_END, 0 };
		mapItemType.insert({ make_pair(TABLE_PLAYER, TRUE), TORCH });

		UIITEM WOODSWORD = { m_iIndex, ITEM_SWORD, MATERIAL_WOOD, 4 };
		mapItemType.insert({ make_pair(TABLE_CRAFT, TRUE), WOODSWORD });

		UIITEM COPPERPICKAXE = { m_iIndex, ITEM_PICKAXE, MATERIAL_COPPER, 13 };
		mapItemType.insert({ make_pair(TABLE_CRAFT, TRUE), COPPERPICKAXE });

		UIITEM IRONPICKAXE = { m_iIndex, ITEM_PICKAXE, MATERIAL_IRON, 20 };
		mapItemType.insert({ make_pair(TABLE_CRAFT, TRUE), IRONPICKAXE });

		UIITEM COPPERHELM = { m_iIndex, ITEM_HELMET, MATERIAL_COPPER, 28 };
		mapItemType.insert({ make_pair(TABLE_ANVIL, TRUE), COPPERHELM });

		UIITEM HEALTHPOTION = { m_iIndex, ITEM_POTION_HP, MATERIAL_WOOD, 40 };
		mapItemType.insert({ make_pair(TABLE_ALCHEMY, TRUE), HEALTHPOTION });

		UIITEM COPPERNECKLACE = { m_iIndex, ITEM_NECKLACE, MATERIAL_WOOD, 43 };
		mapItemType.insert({ make_pair(TABLE_ACCESSORY, TRUE), COPPERNECKLACE });

		UIITEM IRONHELM = { m_iIndex, ITEM_HELMET, MATERIAL_IRON, 34 };
		mapItemType.insert({ make_pair(TABLE_ANVIL, TRUE), IRONHELM });

		// 辦難

		UIITEM FURNACE = { m_iIndex, ITEM_FURNACE, MATERIAL_WOOD, 10 };
		mapItemType.insert({ make_pair(TABLE_CRAFT, FALSE), FURNACE });

		UIITEM COPPERANVIL = { m_iIndex, ITEM_ANVIL, MATERIAL_COPPER, 17 };
		mapItemType.insert({ make_pair(TABLE_CRAFT, FALSE), COPPERANVIL });


		UIITEM POTIONTABLE = { m_iIndex, ITEM_POTION_TABLE, MATERIAL_IRON, 18 };
		mapItemType.insert({ make_pair(TABLE_CRAFT, FALSE), POTIONTABLE });
		//UIITEM MUSICTABLE = { m_iIndex, ITEM_MUSIC_TABLE, MATERIAL_IRON, 24 };
		//mapItemType.insert({ make_pair(TABLE_CRAFT, FALSE), MUSICTABLE });

		UIITEM COPPERSWORD = { m_iIndex, ITEM_SWORD, MATERIAL_COPPER, 32 };
		mapItemType.insert({ make_pair(TABLE_ANVIL, FALSE), COPPERSWORD });

		UIITEM IRONSWORD = { m_iIndex, ITEM_SWORD, MATERIAL_IRON, 38 };
		mapItemType.insert({ make_pair(TABLE_ANVIL, FALSE), IRONSWORD });

		break;
	}
	case 1:
	{
		//謝難
		UIITEM WOODPICKAXE = { m_iIndex, ITEM_PICKAXE, MATERIAL_WOOD, 1 };
		mapItemType.insert({ make_pair(TABLE_PLAYER, TRUE), WOODPICKAXE });

		UIITEM WOODHOE = { m_iIndex, ITEM_HOE, MATERIAL_WOOD , 5};
		mapItemType.insert({ make_pair(TABLE_CRAFT, TRUE), WOODHOE });

		UIITEM COPPERHOE = { m_iIndex, ITEM_HOE, MATERIAL_COPPER, 14 };
		mapItemType.insert({ make_pair(TABLE_CRAFT, TRUE), COPPERHOE });

		UIITEM IRONHOE = { m_iIndex, ITEM_HOE, MATERIAL_IRON, 21 };
		mapItemType.insert({ make_pair(TABLE_CRAFT, TRUE), IRONHOE });

		UIITEM COPPERCHEST = { m_iIndex, ITEM_CHEST, MATERIAL_COPPER, 29 };
		mapItemType.insert({ make_pair(TABLE_ANVIL, TRUE), COPPERCHEST });

		UIITEM ATTPOTION = { m_iIndex, ITEM_POTION_ATT, MATERIAL_WOOD, 41 };
		mapItemType.insert({ make_pair(TABLE_ALCHEMY, TRUE), ATTPOTION });

		UIITEM IRONNECKLACE = { m_iIndex, ITEM_NECKLACE, MATERIAL_WOOD, 44 };
		mapItemType.insert({ make_pair(TABLE_ACCESSORY, TRUE), IRONNECKLACE });

		UIITEM IRONCHEST = { m_iIndex, ITEM_CHEST, MATERIAL_IRON, 35 };
		mapItemType.insert({ make_pair(TABLE_ANVIL, TRUE), IRONCHEST });

		// 辦難
		UIITEM COPPERTABLE = { m_iIndex, ITEM_TABLE, MATERIAL_WOOD, 12 };
		mapItemType.insert({ make_pair(TABLE_CRAFT, FALSE), COPPERTABLE });

		UIITEM COOKINGPOT = { m_iIndex, ITEM_COOKINGPOT, MATERIAL_COPPER, 11};
		mapItemType.insert({ make_pair(TABLE_CRAFT, FALSE), COOKINGPOT });

		UIITEM SPRINKLER = { m_iIndex, ITEM_SPRINKLER, MATERIAL_IRON, 25 };
		mapItemType.insert({ make_pair(TABLE_CRAFT, FALSE), SPRINKLER });


		UIITEM IRONBOW = { m_iIndex, ITEM_BOW, MATERIAL_IRON, 39 };
		mapItemType.insert({ make_pair(TABLE_ANVIL, FALSE), IRONBOW });

		break;
	}
	case 2:
	{
		//謝難

		UIITEM BASICCRAFTTABLE = { m_iIndex, ITEM_TABLE, MATERIAL_WOOD, 2 };
		mapItemType.insert({ make_pair(TABLE_PLAYER, TRUE), BASICCRAFTTABLE });

		UIITEM WATERINGCAN= { m_iIndex, ITEM_WATERINGCAN, MATERIAL_WOOD, 6 };
		mapItemType.insert({ make_pair(TABLE_CRAFT, TRUE), WATERINGCAN });

		UIITEM SMALLLAMP = { m_iIndex, ITEM_LANTERN, MATERIAL_COPPER, 15 };
		mapItemType.insert({ make_pair(TABLE_CRAFT, TRUE), SMALLLAMP });

		UIITEM BIGLAMP = { m_iIndex, ITEM_LANTERN, MATERIAL_IRON, 22 };
		mapItemType.insert({ make_pair(TABLE_CRAFT, TRUE), BIGLAMP });

		UIITEM COPPERLEG = { m_iIndex, ITEM_LEG, MATERIAL_COPPER, 30 };
		mapItemType.insert({ make_pair(TABLE_ANVIL, TRUE), COPPERLEG });

		UIITEM DEFPOTION = { m_iIndex, ITEM_POTION_DEF, MATERIAL_WOOD, 42 };
		mapItemType.insert({ make_pair(TABLE_ALCHEMY, TRUE), DEFPOTION });

		UIITEM MAGNETRING = { m_iIndex, ITEM_RING, MATERIAL_WOOD, 46 };
		mapItemType.insert({ make_pair(TABLE_ACCESSORY, TRUE), MAGNETRING });

		UIITEM IRONLEG = { m_iIndex, ITEM_LEG, MATERIAL_IRON, 36 };
		mapItemType.insert({ make_pair(TABLE_ANVIL, TRUE), IRONLEG });

		// 辦難

		UIITEM ACCESSORYTABLE = { m_iIndex, ITEM_ACCESSORY_TABLE, MATERIAL_COPPER, 27 };
		mapItemType.insert({ make_pair(TABLE_CRAFT, FALSE), ACCESSORYTABLE });

		UIITEM IRONANVIL = { m_iIndex, ITEM_ANVIL, MATERIAL_IRON, 26 };
		mapItemType.insert({ make_pair(TABLE_CRAFT, FALSE), IRONANVIL });

		break;
	}
	case 3:
	{
		//謝難

		UIITEM CHEST = { m_iIndex, ITEM_BOX, MATERIAL_END, 3 };
		mapItemType.insert({ make_pair(TABLE_PLAYER, TRUE), CHEST });

		UIITEM WOODHELMET = { m_iIndex, ITEM_HELMET, MATERIAL_WOOD, 7 };
		mapItemType.insert({ make_pair(TABLE_CRAFT, TRUE), WOODHELMET });

		UIITEM SMALLBAG = { m_iIndex, ITEM_BAG, MATERIAL_COPPER, 16 };
		mapItemType.insert({ make_pair(TABLE_CRAFT, TRUE), SMALLBAG });

		UIITEM BIGBAG = { m_iIndex, ITEM_BAG, MATERIAL_IRON, 23 };
		mapItemType.insert({ make_pair(TABLE_CRAFT, TRUE), BIGBAG });

		UIITEM WOODSHIELD = { m_iIndex, ITEM_ASSISTANCE, MATERIAL_COPPER, 31 };
		mapItemType.insert({ make_pair(TABLE_ANVIL, TRUE), WOODSHIELD });

		UIITEM SPEEDRING = { m_iIndex, ITEM_RING, MATERIAL_WOOD, 47 };
		mapItemType.insert({ make_pair(TABLE_ACCESSORY, TRUE), SPEEDRING });

		UIITEM IRONSHIELD = { m_iIndex, ITEM_ASSISTANCE, MATERIAL_IRON, 37 };
		mapItemType.insert({ make_pair(TABLE_ANVIL, TRUE), IRONSHIELD });


		UIITEM IRONTABLE = { m_iIndex, ITEM_TABLE, MATERIAL_COPPER, 19 };
		mapItemType.insert({ make_pair(TABLE_CRAFT, FALSE), IRONTABLE });

		break;
	}
	case 4:
	{
		UIITEM WOODCHEST = { m_iIndex, ITEM_CHEST , MATERIAL_WOOD, 8 };
		mapItemType.insert({ make_pair(TABLE_CRAFT, TRUE), WOODCHEST });

		UIITEM IRONWATERINGCAN = { m_iIndex, ITEM_WATERINGCAN, MATERIAL_IRON, 49 };
		mapItemType.insert({ make_pair(TABLE_CRAFT, TRUE), IRONWATERINGCAN });

		break;
	}
	case 5:
	{
		UIITEM WOODLEG = { m_iIndex, ITEM_LEG, MATERIAL_WOOD, 9 };
		mapItemType.insert({ make_pair(TABLE_CRAFT, TRUE), WOODLEG });

		break;
	}
	default:
		break;
	}
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

	pComponent = m_pItemTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIPlayerCraftItem"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_ItemTexture", pComponent });

	pComponent = m_pInventoryCom = dynamic_cast<CInventory*>(Engine::Clone_Proto(L"Proto_OneSlotInventory"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Inventory", pComponent });


	return S_OK;
}

CUICraftSlot* CUICraftSlot::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize, _int _iIndex, _bool bDirection)
{
	CUICraftSlot* pUICraftSlot = new CUICraftSlot(pGraphicDev);

	if (FAILED(pUICraftSlot->Ready_GameObject(vPos, vSize, _iIndex, bDirection)))
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
