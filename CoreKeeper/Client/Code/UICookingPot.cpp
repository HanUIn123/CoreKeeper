#include "pch.h"
#include "..\Header\UICookingPot.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\CraftMgr.h"
#include "..\Header\Food.h"

CUICookingPot::CUICookingPot(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bRender(false), m_fTime(80.f)

{

	for (int i = 0; i < 3; i++)
	{
		m_bRectPicked[i] = false;
	}
}

CUICookingPot::~CUICookingPot()
{
}

HRESULT CUICookingPot::Ready_GameObject(_vec2 vPos, _vec2 vSize)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	D3DVIEWPORT9 Viewport;

	m_pGraphicDev->GetViewport(&Viewport);
	float height = (_float)Viewport.Height;
	float width = (_float)Viewport.Width;

	float x = vPos.x - width / 2;
	float y = height / 2 - vPos.y;

	m_pTransformCom->Set_Pos(x, y, 0);

	m_pTransformCom->Set_Scale(vSize.x, vSize.y, 1.f);

	m_BRect[0] = { (_long)(WINCX / 2.f - 60.f - 15.f), (_long)(WINCY / 2.f - 140.f - 15.f), (_long)(WINCX / 2.f - 60.f + 15.f), (_long)(WINCY / 2.f - 140.f + 15.f) };

	m_BRect[1] = { (_long)(WINCX / 2.f - 60.f - 15.f), (_long)(WINCY / 2.f - 65.f - 15.f), (_long)(WINCX / 2.f - 60.f + 15.f), (_long)(WINCY / 2.f - 65.f + 15.f) };

	m_BRect[2] = { (_long)(WINCX / 2.f + 65.f - 15.f), (_long)(WINCY / 2.f - 105.f - 15.f), (_long)(WINCX / 2.f + 65.f + 15.f), (_long)(WINCY / 2.f - 105.f + 15.f) };

	m_Rect = { (_long)(m_BRect[2].left - 15.f), (_long)(m_BRect[2].top - 15.f),  (_long)(m_BRect[2].right + 15.f),  (_long)(m_BRect[2].bottom + 15.f) };

	m_rFirstRect = m_Rect;

	return S_OK;
}

_int CUICookingPot::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (m_bRender)
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(g_hWnd, &pt);

		if (USlot_Picked(pt))
		{
			m_bRectPicked[0] = true;

			if (Engine::Button_Down(DIM_LB))
			{
				CInventory* pCursorInv = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_UI", L"UI_Cursor", L"Com_Inventory"));

				if (!pCursorInv->Check_Empty(0))
				{
					if (pCursorInv->Get_Item(0)->Get_ItemNum() == ITEM_BERRY || pCursorInv->Get_Item(0)->Get_ItemNum() == ITEM_MUSHROOM || pCursorInv->Get_Item(0)->Get_ItemNum() == ITEM_PEPPER || pCursorInv->Get_Item(0)->Get_ItemNum() == ITEM_CARROT)
					{
						vector<CItem*>* pCvecItem = pCursorInv->Get_VecItemP();
						vector<CItem*>* pFvecItem = m_pUSlotInventoryCom->Get_VecItemP();

						if (!m_pUSlotInventoryCom->Check_Empty(0) && !pCursorInv->Check_Empty(0))
						{
							if ((*pFvecItem)[0]->Get_ItemNum() > ITEM_ETC && (*pFvecItem)[0]->Get_ItemNum() == (*pCvecItem)[0]->Get_ItemNum())
							{
								(*pFvecItem)[0]->Add_Count((*pCvecItem)[0]->Get_Count());
								pCursorInv->Remove_Item(0);
							}
							else if ((*pFvecItem)[0]->Get_ItemNum() != (*pCvecItem)[0]->Get_ItemNum())
								m_pUSlotInventoryCom->Swap_Item(&(*pCvecItem)[0], &(*pFvecItem)[0]);
						}
						else
							m_pUSlotInventoryCom->Swap_Item(&(*pCvecItem)[0], &(*pFvecItem)[0]);
					}
				}
				else if (pCursorInv->Check_Empty(0))
				{
					vector<CItem*>* pCvecItem = pCursorInv->Get_VecItemP();
					vector<CItem*>* pFvecItem = m_pUSlotInventoryCom->Get_VecItemP();

					if (!m_pUSlotInventoryCom->Check_Empty(0) && !pCursorInv->Check_Empty(0))
					{
						if ((*pFvecItem)[0]->Get_ItemNum() > ITEM_ETC && (*pFvecItem)[0]->Get_ItemNum() == (*pCvecItem)[0]->Get_ItemNum())
						{
							(*pFvecItem)[0]->Add_Count((*pCvecItem)[0]->Get_Count());
							pCursorInv->Remove_Item(0);
						}
						else if ((*pFvecItem)[0]->Get_ItemNum() != (*pCvecItem)[0]->Get_ItemNum())
							m_pUSlotInventoryCom->Swap_Item(&(*pCvecItem)[0], &(*pFvecItem)[0]);
					}
					else
						m_pUSlotInventoryCom->Swap_Item(&(*pCvecItem)[0], &(*pFvecItem)[0]);
				}
			}
		}
		else
			m_bRectPicked[0] = false;

		if (DSlot_Picked(pt))
		{
			m_bRectPicked[1] = true;

			if (Engine::Button_Down(DIM_LB))
			{
				CInventory* pCursorInv = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_UI", L"UI_Cursor", L"Com_Inventory"));

				if (!pCursorInv->Check_Empty(0))
				{
					if (pCursorInv->Get_Item(0)->Get_ItemNum() == ITEM_BERRY || pCursorInv->Get_Item(0)->Get_ItemNum() == ITEM_MUSHROOM || pCursorInv->Get_Item(0)->Get_ItemNum() == ITEM_PEPPER || pCursorInv->Get_Item(0)->Get_ItemNum() == ITEM_CARROT)
					{
						vector<CItem*>* pCvecItem = pCursorInv->Get_VecItemP();
						vector<CItem*>* pFvecItem = m_pDSlotInventoryCom->Get_VecItemP();

						if (!m_pDSlotInventoryCom->Check_Empty(0) && !pCursorInv->Check_Empty(0))
						{
							if ((*pFvecItem)[0]->Get_ItemNum() > ITEM_ETC && (*pFvecItem)[0]->Get_ItemNum() == (*pCvecItem)[0]->Get_ItemNum())
							{
								(*pFvecItem)[0]->Add_Count((*pCvecItem)[0]->Get_Count());
								pCursorInv->Remove_Item(0);
							}
							else if ((*pFvecItem)[0]->Get_ItemNum() != (*pCvecItem)[0]->Get_ItemNum())
								m_pDSlotInventoryCom->Swap_Item(&(*pCvecItem)[0], &(*pFvecItem)[0]);
						}
						else
							m_pDSlotInventoryCom->Swap_Item(&(*pCvecItem)[0], &(*pFvecItem)[0]);
					}
				}
				else if (pCursorInv->Check_Empty(0))
				{
					vector<CItem*>* pCvecItem = pCursorInv->Get_VecItemP();
					vector<CItem*>* pFvecItem = m_pDSlotInventoryCom->Get_VecItemP();

					if (!m_pDSlotInventoryCom->Check_Empty(0) && !pCursorInv->Check_Empty(0))
					{
						if ((*pFvecItem)[0]->Get_ItemNum() > ITEM_ETC && (*pFvecItem)[0]->Get_ItemNum() == (*pCvecItem)[0]->Get_ItemNum())
						{
							(*pFvecItem)[0]->Add_Count((*pCvecItem)[0]->Get_Count());
							pCursorInv->Remove_Item(0);
						}
						else if ((*pFvecItem)[0]->Get_ItemNum() != (*pCvecItem)[0]->Get_ItemNum())
							m_pDSlotInventoryCom->Swap_Item(&(*pCvecItem)[0], &(*pFvecItem)[0]);
					}
					else
						m_pDSlotInventoryCom->Swap_Item(&(*pCvecItem)[0], &(*pFvecItem)[0]);
				}
			}
		}
		else
			m_bRectPicked[1] = false;

		if (RSlot_Picked(pt))
		{
			m_bRectPicked[2] = true;

			if (Engine::Button_Down(DIM_LB))
			{
				CInventory* pCursorInv = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_UI", L"UI_Cursor", L"Com_Inventory"));

				if (pCursorInv->Check_Empty(0))
				{
					vector<CItem*>* pCvecItem = pCursorInv->Get_VecItemP();
					vector<CItem*>* pFvecItem = m_pRSlotInventoryCom->Get_VecItemP();

					if (!m_pRSlotInventoryCom->Check_Empty(0) && !pCursorInv->Check_Empty(0))
					{
						if ((*pFvecItem)[0]->Get_ItemNum() > ITEM_ETC && (*pFvecItem)[0]->Get_ItemNum() == (*pCvecItem)[0]->Get_ItemNum())
						{
							(*pFvecItem)[0]->Add_Count((*pCvecItem)[0]->Get_Count());
							pCursorInv->Remove_Item(0);
						}
						else if ((*pFvecItem)[0]->Get_ItemNum() != (*pCvecItem)[0]->Get_ItemNum())
							m_pRSlotInventoryCom->Swap_Item(&(*pCvecItem)[0], &(*pFvecItem)[0]);
					}
					else
						m_pRSlotInventoryCom->Swap_Item(&(*pCvecItem)[0], &(*pFvecItem)[0]);
				}
			}
		}
		else
			m_bRectPicked[2] = false;


		Add_RenderGroup(RENDER_UI, this);
	}

	if (!m_pUSlotInventoryCom->Check_Empty(0) && !m_pDSlotInventoryCom->Check_Empty(0))
	{
		m_fTime -= fTimeDelta * 5.f;

		m_Rect.top = (_long)(m_rFirstRect.top + ((_float)(m_rFirstRect.bottom - m_rFirstRect.top) - ((_float)(m_rFirstRect.bottom - m_rFirstRect.top) * (m_fTime / 80.f))));

		if (m_fTime <= 0)
		{
			m_fTime = 80.f;

			ITEMNUM eUNum = m_pUSlotInventoryCom->Get_Item(0)->Get_ItemNum();

			ITEMNUM eDNum = m_pDSlotInventoryCom->Get_Item(0)->Get_ItemNum();

			CItem* pCraftItem = CCraftMgr::GetInstance()->Cook(eUNum, eDNum);

			m_pRSlotInventoryCom->Add_Item(pCraftItem);

			m_pUSlotInventoryCom->Minus_Item(m_pUSlotInventoryCom->Get_Item(0)->Get_ItemNum(), 1);

			m_pDSlotInventoryCom->Minus_Item(m_pDSlotInventoryCom->Get_Item(0)->Get_ItemNum(), 1);
		}
	}
	else if (m_pUSlotInventoryCom->Check_Empty(0) && !m_pDSlotInventoryCom->Check_Empty(0) || m_pUSlotInventoryCom->Check_Empty(0) && !m_pDSlotInventoryCom->Check_Empty(0))
	{
		m_fTime = 80.f;
	}

	return iExit;
}

void CUICookingPot::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CUICookingPot::Render_GameObject()
{
	_matrix matWorld;

	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	matWorld._11 = 30.f;
	matWorld._22 = 30.f;

	matWorld._41 -= 60.f;
	matWorld._42 += 10.f;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pTextureCom->Set_Texture(1);

	m_pUSlotBufferCom->Render_Buffer();

	if (!m_pUSlotInventoryCom->Check_Empty(0))
	{
		CItem* pItem = m_pUSlotInventoryCom->Get_Item(0);

		_int iNum = pItem->Get_TextureNumber();

		pItem->Get_Texture()->Set_Texture(iNum);

		pItem->Get_Buffer()->Render_First();
	}

	if (m_bRectPicked[0])
	{
		m_pColTextureCom->Set_Texture(0);

		m_pColBufferCom->Render_Buffer();
	}

	matWorld._11 = 30.f;
	matWorld._22 = 30.f;

	matWorld._42 -= 75.f;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pTextureCom->Set_Texture(1);

	m_pDSlotBufferCom->Render_Buffer();

	if (!m_pDSlotInventoryCom->Check_Empty(0))
	{
		CItem* pItem = m_pDSlotInventoryCom->Get_Item(0);

		_int iNum = pItem->Get_TextureNumber();

		pItem->Get_Texture()->Set_Texture(iNum);

		pItem->Get_Buffer()->Render_First();
	}

	if (m_bRectPicked[1])
	{
		m_pColTextureCom->Set_Texture(0);

		m_pColBufferCom->Render_Buffer();
	}

	m_pTransformCom->Get_WorldMatrix(&matWorld);

	matWorld._11 = 30.f;
	matWorld._22 = 30.f;

	matWorld._41 += 65.f;
	matWorld._42 -= 25.f;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pTextureCom->Set_Texture(2);

	m_pRSlotBufferCom->Render_Buffer();

	if (!m_pUSlotInventoryCom->Check_Empty(0) && !m_pDSlotInventoryCom->Check_Empty(0))
	{
		m_pGraphicDev->SetScissorRect(&m_Rect);

		m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);

		m_pTextureCom->Set_Texture(1);

		m_pSciBufferCom->Render_Buffer();

		m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
	}

	if (!m_pRSlotInventoryCom->Check_Empty(0))
	{
		CItem* pItem = m_pRSlotInventoryCom->Get_Item(0);

		_int iNum = pItem->Get_TextureNumber();

		pItem->Get_Texture()->Set_Texture(iNum);

		pItem->Get_Buffer()->Render_First();
	}
	else if (!m_pUSlotInventoryCom->Check_Empty(0) && !m_pDSlotInventoryCom->Check_Empty(0) && m_pRSlotInventoryCom->Check_Empty(0))
	{
		ITEMNUM eUNum = m_pUSlotInventoryCom->Get_Item(0)->Get_ItemNum();

		ITEMNUM eDNum = m_pDSlotInventoryCom->Get_Item(0)->Get_ItemNum();

		ITEMNUM eRNum = ITEM_END;

		switch (eUNum)
		{
		case ITEM_BERRY:
			switch (eDNum)
			{
			case ITEM_BERRY:
				eRNum = ITEM_BERRY_BERRY_FOOD;
				break;
			case ITEM_PEPPER:
				eRNum = ITEM_BERRY_PEPPER_FOOD;
				break;
			case ITEM_CARROT:
				eRNum = ITEM_BERRY_CARROT_FOOD;
				break;
			case ITEM_MUSHROOM:
				eRNum = ITEM_BERRY_MUSHROOM_FOOD;
				break;
			}
			break;
		case ITEM_PEPPER:
			switch (eDNum)
			{
			case ITEM_PEPPER:
				eRNum = ITEM_PEPPER_PEPPER_FOOD;
				break;
			case ITEM_CARROT:
				eRNum = ITEM_PEPPER_CARROT_FOOD;
				break;
			case ITEM_MUSHROOM:
				eRNum = ITEM_PEPPER_MUSHROOM_FOOD;
				break;
			}
			break;
		case ITEM_CARROT:
			switch (eDNum)
			{
			case ITEM_CARROT:
				eRNum = ITEM_CARROT_CARROT_FOOD;
				break;
			case ITEM_MUSHROOM:
				eRNum = ITEM_CARROT_MUSHROOM_FOOD;
				break;
			}
			break;
		case ITEM_MUSHROOM:
			eRNum = ITEM_MUSHROOM_MUSHROOM_FOOD;
			break;
		}
		_int iNum = eRNum - ITEM_BERRY_BERRY_FOOD;

		m_pFoodTextureCom->Set_Texture(iNum);

		m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

		m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(100, 255, 255, 255));
		m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
		m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		m_pFoodBufferCom->Render_Buffer();

		m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);

		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

		m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	}

	if (m_bRectPicked[2])
	{
		m_pColTextureCom->Set_Texture(0);

		m_pColBufferCom->Render_Buffer();
	}

}

HRESULT CUICookingPot::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_PlateBuffer", pComponent });

	pComponent = m_pUSlotBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_USlotBuffer", pComponent });

	pComponent = m_pDSlotBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_DSlotBuffer", pComponent });

	pComponent = m_pRSlotBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_RSlotBuffer", pComponent });

	pComponent = m_pColBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_ColBuffer", pComponent });

	pComponent = m_pSciBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_SciBuffer", pComponent });

	pComponent = m_pFoodBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_FoodBuffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_CookingPot"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pColTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIInvSelected"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_ColTexture", pComponent });

	pComponent = m_pFoodTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_FoodTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_FoodTexture", pComponent });

	pComponent = m_pUSlotInventoryCom = dynamic_cast<CInventory*>(Engine::Clone_Proto(L"Proto_FurnaceInventory"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_USlotInventory", pComponent });

	pComponent = m_pDSlotInventoryCom = dynamic_cast<CInventory*>(Engine::Clone_Proto(L"Proto_FurnaceInventory"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_DSlotInventory", pComponent });

	pComponent = m_pRSlotInventoryCom = dynamic_cast<CInventory*>(Engine::Clone_Proto(L"Proto_FurnaceInventory"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_RSlotInventory", pComponent });

	return S_OK;
}

CUICookingPot* CUICookingPot::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize)
{
	CUICookingPot* pUIPlate = new CUICookingPot(pGraphicDev);

	if (FAILED(pUIPlate->Ready_GameObject(vPos, vSize)))
	{
		Safe_Release(pUIPlate);
		MSG_BOX("UIStatus Create Failed");
		return nullptr;
	}

	return pUIPlate;
}

void CUICookingPot::Free()
{
	Engine::CGameObject::Free();
}
