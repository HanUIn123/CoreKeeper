#include "pch.h"
#include "..\Header\UICraftSlot.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\Torch.h"
#include "..\Header\Stage.h"
#include "..\Header\CraftMgr.h"

CUICraftSlot::CUICraftSlot(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bCollapse(false), m_bWindow(false), m_iIndex(0), m_bEnough(false)

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

	/*
	switch (_iIndex)
	{
	case 0:
		m_eSlotType = UCITEM_TORCH;
		break;

	case 1:
		m_eSlotType = UCITEM_WOODENPICK;
		break;

	case 2:
		m_eSlotType = UCITEM_WORKBENCH;
		break;

	case 3:
		m_eSlotType = UCITEM_CHEST;
		break;

	default:
		m_eSlotType = UCITEM_TORCH;
		break;
	}*/

	m_iIndex = _iIndex;

	m_bDirection = bDirection;

	Ready_Table(); // 인덱스에 맞는 아이템을 map에 넣어줌

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

		/*
		switch (m_eSlotType)
		{
		case UCITEM_TORCH:

			if(CCraftMgr::GetInstance()->Craftable(pPlayerInv, ITEM_TORCH, MATERIAL_WOOD))
			{
				m_bEnough = true;
			}
			else
				m_bEnough = false;
			break;

		case UCITEM_WOODENPICK:

			if (CCraftMgr::GetInstance()->Craftable(pPlayerInv, ITEM_PICKAXE, MATERIAL_WOOD))
			{
				m_bEnough = true;
			}
			else
				m_bEnough = false;
			break;

		case UCITEM_WORKBENCH:
			if (pPlayerInv->Enough_Item(ITEM_WOOD, 8))
			{
				m_bEnough = true;
			}
			else
				m_bEnough = false;
			break;

		case UCITEM_CHEST:
			if (pPlayerInv->Enough_Item(ITEM_WOOD, 5))
			{
				m_bEnough = true;
			}
			else
				m_bEnough = false;
			break;

		default:
			m_bEnough = false;
			break;
		}
		*/

		if (CCraftMgr::GetInstance()->Craftable(pPlayerInv, m_eItemType.eItemNum, m_eItemType.eItemMat))
		{
			m_bEnough = true;
		}
		else
			m_bEnough = false;

		if (Map_Picked(pt))
		{
			m_bCollapse = true;

			if (Engine::Button_Down(DIM_LB))
			{
				if (m_bEnough)
				{
					CInventory* pCursorInv = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_UI", L"UI_Cursor", L"Com_Inventory"));

					pCursorInv->Add_Item(CCraftMgr::GetInstance()->Craft(pPlayerInv, m_eItemType.eItemNum, m_eItemType.eItemMat));
					/*
					CStage* pStage = dynamic_cast<CStage*>(Engine::Get_Scene());

					CItem* pItem = nullptr;

					
					switch (m_eSlotType)
					{
					case UCITEM_TORCH:
						pCursorInv->Add_Item(CCraftMgr::GetInstance()->Craft(pPlayerInv, ITEM_TORCH, MATERIAL_WOOD));
						break;

					case UCITEM_WOODENPICK:
						pCursorInv->Add_Item(CCraftMgr::GetInstance()->Craft(pPlayerInv, ITEM_PICKAXE, MATERIAL_WOOD));
						break;
					}*/
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

	/*
	switch (m_iIndex)
	{
	case 0:
		matWorld._11 -= 20.f;
		matWorld._22 -= 10.f;
		break;

	case 1:
		matWorld._11 -= 10.f;
		matWorld._22 -= 10.f;
		break;

	case 2:
		matWorld._11 -= 10.f;
		matWorld._22 -= 10.f;
		break;

	case 3:
		matWorld._11 -= 10.f;
		matWorld._22 -= 10.f;
		break;

	}*/
	
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

		m_pItemTextureCom->Set_Texture(m_iIndex);

		m_pBufferCom->Render_Buffer();

		m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);

		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

		m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	}
	else if (m_bEnough)
	{
		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

		m_pItemTextureCom->Set_Texture(m_iIndex);

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

void CUICraftSlot::Set_Window(TABLETYPE _eTableType, _bool _bDirection)
{
	if (m_bWindow)
		m_bWindow = false;
	else
	{
		m_bWindow = true;

		auto iter = mapItemType.find({ _eTableType, _bDirection });

		if (iter == mapItemType.end())
		{
			m_bWindow = false;

			return;
		}

		m_eItemType = iter->second;
	}
}

void CUICraftSlot::Ready_Table()
{
	switch (m_iIndex)
	{
	case 0:
	{
		UIITEM TORCH = { m_iIndex, ITEM_TORCH };
		mapItemType.insert({ make_pair(TABLE_PLAYER, TRUE), TORCH });

		UIITEM WOODSWORD = { m_iIndex, ITEM_SWORD, MATERIAL_WOOD };
		mapItemType.insert({ make_pair(TABLE_CRAFT, TRUE), TORCH });

		UIITEM COPPERPICKAXE = { m_iIndex, ITEM_PICKAXE, MATERIAL_COPPER };
		mapItemType.insert({ make_pair(TABLE_CRAFT, TRUE), COPPERPICKAXE });

		break;
	}
	case 1:
	{
		UIITEM WOODPICKAXE = { m_iIndex, ITEM_PICKAXE, MATERIAL_WOOD };
		mapItemType.insert({ make_pair(TABLE_PLAYER, TRUE), WOODPICKAXE });

		UIITEM WOODHOE = { m_iIndex, ITEM_HOE, MATERIAL_WOOD };
		mapItemType.insert({ make_pair(TABLE_CRAFT, TRUE), WOODPICKAXE });

		break;
	}
	case 2:
	{
		UIITEM BASICCRAFTTABLE = { m_iIndex, ITEM_TABLE };
		mapItemType.insert({ make_pair(TABLE_PLAYER, TRUE), BASICCRAFTTABLE });

		break;
	}
	case 3:
	{
		UIITEM CHEST = { m_iIndex, ITEM_CHEST };
		mapItemType.insert({ make_pair(TABLE_PLAYER, TRUE), CHEST });

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
