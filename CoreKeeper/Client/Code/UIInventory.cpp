#include "pch.h"
#include "..\Header\UIInventory.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Enum.h"
#include "..\Header\Item.h"
#include "..\Header\Player.h"

CUIInventory::CUIInventory(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_iCurInv(0), m_bShow(false), m_bCollapse(false)

{
}

CUIInventory::~CUIInventory()
{
}

HRESULT CUIInventory::Ready_GameObject(_vec2 vPos, _int _iIndex)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	_D3DVIEWPORT9 Viewport;

	m_pGraphicDev->GetViewport(&Viewport);
	float height = (_float)Viewport.Height;
	float width = (_float)Viewport.Width;

	float x = vPos.x - width / 2;
	float y = height / 2 - vPos.y;

	m_pTransformCom->Set_Pos(x, y, 0);

	_vec2 vSize = { 30.f, 30.f };

	m_pTransformCom->Set_Scale(vSize.x, vSize.y, 1.f);

	m_BRect.left = vPos.x - vSize.x / 2;
	m_BRect.right = vPos.x + vSize.x / 2;
	m_BRect.top = vPos.y - vSize.y / 2;
	m_BRect.bottom = vPos.y + vSize.y / 2;

	m_iIndex = _iIndex + 1;

	return S_OK;
}

_int CUIInventory::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (m_bShow)
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(g_hWnd, &pt);

		if (Map_Picked(pt))
		{
			if (Engine::Button_Down(DIM_LB))
			{
				CInventory* pCursorInv = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_UI", L"UI_Cursor", L"Com_Inventory"));
				CInventory* pPlayerInv = dynamic_cast<Engine::CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"Player", L"Com_Inventory"));

				vector<CItem*>* pCvecItem = pCursorInv->Get_VecItemP();
				vector<CItem*>* pPvecItem = pPlayerInv->Get_VecItemP();

				_int iIndex = m_iIndex - 1;

				if (!pPlayerInv->Check_Empty(iIndex) && !pCursorInv->Check_Empty(0))
				{
					if ((*pPvecItem)[iIndex]->Get_ItemNum() > ITEM_ETC && (*pPvecItem)[iIndex]->Get_ItemNum() == (*pCvecItem)[0]->Get_ItemNum())
					{
						(*pPvecItem)[iIndex]->Add_Count((*pCvecItem)[0]->Get_Count());
						pCursorInv->Remove_Item(0);
					}
					else if ((*pPvecItem)[iIndex]->Get_ItemNum() != (*pCvecItem)[0]->Get_ItemNum())
						pPlayerInv->Swap_Item(&(*pCvecItem)[0], &(*pPvecItem)[iIndex]);
				}
				else
					pPlayerInv->Swap_Item(&(*pCvecItem)[0], &(*pPvecItem)[iIndex]);
			}

			m_bCollapse = true;
		}
		else
		{
			m_bCollapse = false;
		}

		Engine::Add_RenderGroup(RENDER_UI, this);
	}
	return iExit;
}

void CUIInventory::LateUpdate_GameObject()
{
	

	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIInventory::Render_GameObject()
{
	
	_matrix matWorld;
	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pTextureCom->Set_Texture();

	m_pRcTextureCom->Render_Buffer();

	if (m_bCollapse && (m_iCurInv != m_iIndex))
	{
		m_pColTextureCom->Set_Texture();

		m_pRcTextureCom->Render_Buffer();
	}


	CInventory* pPlayerInv = dynamic_cast<Engine::CInventory*>
		(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"Player", L"Com_Inventory"));

	_int iIndex = m_iIndex;

	//if (iIndex >= pPlayerInv->Get_SlotCount())
	//{
		iIndex--;
	//}

	if (!pPlayerInv->Check_Empty(iIndex))
	{
		pItem = pPlayerInv->Get_Item(iIndex);

		_int iCount = pItem->Get_Count();

		pItem->Get_Texture()->Set_Texture();

		Engine::ITEMNUM eNum = pItem->Get_ItemNum();


		switch (eNum)
		{
		//case ITEM_SEED:
		//	matWorld._11 = 10.f;
		//	matWorld._22 = 10.f;
		//	break;

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
			matWorld._11 = 30.f;
			matWorld._22 = 30.f;

			matWorld._42 -= 13.f;
			break;

		case ITEM_CHEST:
			matWorld._11 = 30.f;
			matWorld._22 = 30.f;
			break;

		case ITEM_LEG:
			matWorld._11 = 40.f;
			matWorld._22 = 40.f;

			matWorld._42 += 15.f;
			break;

		dafault:
			matWorld._11 = 20.f;
			matWorld._22 = 20.f;
			break;
		}


		if (iCount != 1)
		{
			wstring sFont = std::to_wstring(iCount);

			const _tchar* tFont = sFont.c_str();

			_vec2 pos(m_BRect.right - 1.f, m_BRect.top + 12.f);

			Engine::Render_Font(L"Font_Item", tFont, &pos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
		}

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

		pItem->Get_Buffer()->Render_First();
	}
}

HRESULT CUIInventory::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pRcTextureCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_RcBuffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIScreenInvTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pNumTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UINumber"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_NumTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });

	pComponent = m_pInventoryCom = dynamic_cast<CInventory*>(Engine::Clone_Proto(L"Proto_PlayerInventory"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Inventory", pComponent });

	pComponent = m_pAnimatorCom = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_Animator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Animator", pComponent });

	pComponent = m_pColTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIInvSelected"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_ColTexture", pComponent });

	pComponent = m_pItemNumTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIItemNumber"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_ItemNumTexture", pComponent });

	return S_OK;
}

CUIInventory* CUIInventory::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _int _iIndex)
{
	CUIInventory* pUIInventory = new CUIInventory(pGraphicDev);

	if (FAILED(pUIInventory->Ready_GameObject(vPos, _iIndex)))
	{
		Safe_Release(pUIInventory);
		MSG_BOX("UIInventory Create Failed");
		return nullptr;
	}

	return pUIInventory;
}

void CUIInventory::Free()
{
	Engine::CGameObject::Free();
}
