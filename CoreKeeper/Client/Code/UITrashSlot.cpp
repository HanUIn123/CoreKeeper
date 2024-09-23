#include "pch.h"
#include "..\Header\UITrashSlot.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\UITrashCan.h"

CUITrashSlot::CUITrashSlot(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bCollapse(false), m_bFirst(false), m_bWindow(false)

{
}

CUITrashSlot::~CUITrashSlot()
{
}

HRESULT CUITrashSlot::Ready_GameObject(_vec2 vPos, _vec2 vSize)
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

	m_BRect.left = vPos.x - vSize.x / 2;
	m_BRect.right = vPos.x + vSize.x / 2;
	m_BRect.top = vPos.y - vSize.y / 2;
	m_BRect.bottom = vPos.y + vSize.y / 2;

	return S_OK;
}

_int CUITrashSlot::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (m_bWindow)
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(g_hWnd, &pt);

		CInventory* pPlayerInv = dynamic_cast<Engine::CInventory*>
			(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"Player", L"Com_Inventory"));

		
		if (Map_Picked(pt))
		{
			m_bCollapse = true;

			if (Engine::Button_Down(DIM_LB))
			{
				CInventory* pCursorInv = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_UI", L"UI_Cursor", L"Com_Inventory"));
				CInventory* pTrashInv = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_UI", L"UI_TrashCan", L"Com_TrashInventory"));

				vector<CItem*>* pCvecItem = pCursorInv->Get_VecItemP();
				vector<CItem*>* pTvecItem = pTrashInv->Get_VecItemP();

				if (!pTrashInv->Check_Empty(0) && !pCursorInv->Check_Empty(0))
				{
					if ((*pTvecItem)[0]->Get_ItemNum() > ITEM_ETC && (*pTvecItem)[0]->Get_ItemNum() == (*pCvecItem)[0]->Get_ItemNum())
					{
						(*pTvecItem)[0]->Add_Count((*pCvecItem)[0]->Get_Count());
						pCursorInv->Remove_Item(0);
					}
				}
				else
				    pTrashInv->Swap_Item(&(*pCvecItem)[0], &(*pTvecItem)[0]);
			}

		}
		else
			m_bCollapse = false;

		Engine::Add_RenderGroup(RENDER_UI, this);
	}
	return iExit;
}

void CUITrashSlot::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CUITrashSlot::Render_GameObject()
{
	_matrix matWorld;

	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	if (m_bCollapse)
	{
		m_pColTextureCom->Set_Texture();

		m_pBufferCom->Render_Buffer();
	}

	matWorld._11 = 15.f;
	matWorld._22 = 15.f;
	matWorld._42 -= 35.f;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pArrowTextureCom->Set_Texture(1);

	m_pBufferCom->Render_Buffer();

	matWorld._42 += 35.f;

	CInventory* pTrashInv = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_UI", L"UI_TrashCan", L"Com_TrashInventory"));

	if (!pTrashInv->Check_Empty(0))
	{
		m_pItem = pTrashInv->Get_Item(0);

		ITEMNUM pNum = m_pItem->Get_ItemNum();

		_int iCount = m_pItem->Get_Count();

		switch (pNum)
		{
		case ITEM_SEED:
			matWorld._11 = 10.f;
			matWorld._22 = 10.f;
			break;

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

		case ITEM_SHOVEL:
			matWorld._11 = 60.f;
			matWorld._22 = 60.f;

			matWorld._42 -= 8.f;
			break;

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
		}


		if (iCount != 1)
		{
			wstring sFont = std::to_wstring(iCount);

			const _tchar* tFont = sFont.c_str();

			_vec2 pos(m_BRect.right - 1.f, m_BRect.top + 12.f);

			Engine::Render_Font(L"Font_Item", tFont, &pos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
		}

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

		m_pItem->Get_Texture()->Set_Texture();

		m_pItem->Get_Buffer()->Render_First();
	}

}

HRESULT CUITrashSlot::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UITrashSlot"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });

	pComponent = m_pColTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIInvSelected"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_ColTexture", pComponent });

	pComponent = m_pArrowTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UITrashCan"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_ArrowTexture", pComponent });

	return S_OK;
}

CUITrashSlot* CUITrashSlot::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize)
{
	CUITrashSlot* pUICraftSlot = new CUITrashSlot(pGraphicDev);

	if (FAILED(pUICraftSlot->Ready_GameObject(vPos, vSize)))
	{
		Safe_Release(pUICraftSlot);
		MSG_BOX("UIStatus Create Failed");
		return nullptr;
	}

	return pUICraftSlot;
}

void CUITrashSlot::Free()
{
	Engine::CGameObject::Free();
}
