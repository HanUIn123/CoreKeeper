#include "pch.h"
#include "..\Header\UICookingPot.h"
#include "Export_System.h"
#include "Export_Utility.h"

CUICookingPot::CUICookingPot(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bRender(false)

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

	m_BRect[2] = { (_long)(WINCX / 2.f - 60.f - 15.f), (_long)(WINCY / 2.f + 140.f - 15.f), (_long)(WINCX / 2.f - 60.f + 15.f), (_long)(WINCY / 2.f + 140.f + 15.f) };

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
					if (pCursorInv->Get_Item(0)->Get_ItemNum() == ITEM_BERRY || pCursorInv->Get_Item(0)->Get_ItemNum() == ITEM_MUSHROOM || pCursorInv->Get_Item(0)->Get_ItemNum() == ITEM_BERRY || pCursorInv->Get_Item(0)->Get_ItemNum() == ITEM_MUSHROOM)
					{
						vector<CItem*>* pCvecItem = pCursorInv->Get_VecItemP();
						vector<CItem*>* pFvecItem = m_pInventoryCom->Get_VecItemP();

						if (!m_pInventoryCom->Check_Empty(0) && !pCursorInv->Check_Empty(0))
						{
							if ((*pFvecItem)[0]->Get_ItemNum() > ITEM_ETC && (*pFvecItem)[0]->Get_ItemNum() == (*pCvecItem)[0]->Get_ItemNum())
							{
								(*pFvecItem)[0]->Add_Count((*pCvecItem)[0]->Get_Count());
								pCursorInv->Remove_Item(0);
							}
							else if ((*pFvecItem)[0]->Get_ItemNum() != (*pCvecItem)[0]->Get_ItemNum())
								m_pInventoryCom->Swap_Item(&(*pCvecItem)[0], &(*pFvecItem)[0]);
						}
						else
							m_pInventoryCom->Swap_Item(&(*pCvecItem)[0], &(*pFvecItem)[0]);
					}
				}
				else if (pCursorInv->Check_Empty(0))
				{
					vector<CItem*>* pCvecItem = pCursorInv->Get_VecItemP();
					vector<CItem*>* pFvecItem = m_pInventoryCom->Get_VecItemP();

					if (!m_pInventoryCom->Check_Empty(0) && !pCursorInv->Check_Empty(0))
					{
						if ((*pFvecItem)[0]->Get_ItemNum() > ITEM_ETC && (*pFvecItem)[0]->Get_ItemNum() == (*pCvecItem)[0]->Get_ItemNum())
						{
							(*pFvecItem)[0]->Add_Count((*pCvecItem)[0]->Get_Count());
							pCursorInv->Remove_Item(0);
						}
						else if ((*pFvecItem)[0]->Get_ItemNum() != (*pCvecItem)[0]->Get_ItemNum())
							m_pInventoryCom->Swap_Item(&(*pCvecItem)[0], &(*pFvecItem)[0]);
					}
					else
						m_pInventoryCom->Swap_Item(&(*pCvecItem)[0], &(*pFvecItem)[0]);
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
					if (pCursorInv->Get_Item(0)->Get_ItemNum() == ITEM_BERRY_SEED || pCursorInv->Get_Item(0)->Get_ItemNum() == ITEM_MUSHROOM || pCursorInv->Get_Item(0)->Get_ItemNum() == ITEM_BERRY || pCursorInv->Get_Item(0)->Get_ItemNum() == ITEM_MUSHROOM)
					{
						vector<CItem*>* pCvecItem = pCursorInv->Get_VecItemP();
						vector<CItem*>* pFvecItem = m_pInventoryCom->Get_VecItemP();

						if (!m_pInventoryCom->Check_Empty(0) && !pCursorInv->Check_Empty(0))
						{
							if ((*pFvecItem)[0]->Get_ItemNum() > ITEM_ETC && (*pFvecItem)[0]->Get_ItemNum() == (*pCvecItem)[0]->Get_ItemNum())
							{
								(*pFvecItem)[0]->Add_Count((*pCvecItem)[0]->Get_Count());
								pCursorInv->Remove_Item(0);
							}
							else if ((*pFvecItem)[0]->Get_ItemNum() != (*pCvecItem)[0]->Get_ItemNum())
								m_pInventoryCom->Swap_Item(&(*pCvecItem)[0], &(*pFvecItem)[0]);
						}
						else
							m_pInventoryCom->Swap_Item(&(*pCvecItem)[0], &(*pFvecItem)[0]);
					}
				}
				else if (pCursorInv->Check_Empty(0))
				{
					vector<CItem*>* pCvecItem = pCursorInv->Get_VecItemP();
					vector<CItem*>* pFvecItem = m_pInventoryCom->Get_VecItemP();

					if (!m_pInventoryCom->Check_Empty(0) && !pCursorInv->Check_Empty(0))
					{
						if ((*pFvecItem)[0]->Get_ItemNum() > ITEM_ETC && (*pFvecItem)[0]->Get_ItemNum() == (*pCvecItem)[0]->Get_ItemNum())
						{
							(*pFvecItem)[0]->Add_Count((*pCvecItem)[0]->Get_Count());
							pCursorInv->Remove_Item(0);
						}
						else if ((*pFvecItem)[0]->Get_ItemNum() != (*pCvecItem)[0]->Get_ItemNum())
							m_pInventoryCom->Swap_Item(&(*pCvecItem)[0], &(*pFvecItem)[0]);
					}
					else
						m_pInventoryCom->Swap_Item(&(*pCvecItem)[0], &(*pFvecItem)[0]);
				}
			}
		}
		else
			m_bRectPicked[1] = false;

		if (RSlot_Picked(pt))
		{
			m_bRectPicked[2] = true;
		}
		else
			m_bRectPicked[2] = false;

		Add_RenderGroup(RENDER_UI, this);
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

	if (m_bRectPicked[1])
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

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_CookingPot"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pColTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIInvSelected"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_ColTexture", pComponent });

	pComponent = m_pInventoryCom = dynamic_cast<CInventory*>(Engine::Clone_Proto(L"Proto_FurnaceInventory"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Inventory", pComponent });

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
