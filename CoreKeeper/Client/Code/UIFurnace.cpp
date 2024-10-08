#include "pch.h"
#include "..\Header\UIFurnace.h"
#include "..\Header\UICursor.h"
#include "..\Header\CraftMgr.h"
#include "Export_System.h"
#include "Export_Utility.h"

CUIFurnace::CUIFurnace(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bWindow(false), m_bLSlot(false), m_bRSlot(false), m_fTime(80.f)

{
}

CUIFurnace::~CUIFurnace()
{
}

HRESULT CUIFurnace::Ready_GameObject(_vec2 vPos, _vec2 vSize)
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

	m_BLRect.left = (_long)(WINCX / 2 - 50.f - 15.f);
	m_BLRect.right = (_long)(WINCX / 2 - 50.f + 15.f);
	m_BLRect.top = (_long)(WINCY / 2 - 65.f - 15.f);
	m_BLRect.bottom = (_long)(WINCY / 2 - 65.f + 15.f);

	m_BRRect.left = (_long)(WINCX / 2 + 50.f - 15.f);
	m_BRRect.right = (_long)(WINCX / 2 + 50.f + 15.f);
	m_BRRect.top = (_long)(WINCY / 2 - 65.f - 15.f);
	m_BRRect.bottom = (_long)(WINCY / 2 - 65.f + 15.f);

	m_Rect = { (_long)(m_BRRect.left - 15.f), (_long)(m_BRRect.top - 15.f), (_long)(m_BRRect.right + 15.f) ,(_long)(m_BRRect.bottom + 15.f) };

	m_FirstRect = m_Rect;

	return S_OK;
}

_int CUIFurnace::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (m_bWindow)
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(g_hWnd, &pt);

		if (LSlot_Picked(pt))
		{
			m_bLSlot = true;

			if (Engine::Button_Down(DIM_LB))
			{
				CInventory* pCursorInv = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_UI", L"UI_Cursor", L"Com_Inventory"));

				if (!pCursorInv->Check_Empty(0))
				{
					if (pCursorInv->Get_Item(0)->Get_ItemNum() == ITEM_COPPER || pCursorInv->Get_Item(0)->Get_ItemNum() == ITEM_IRON || pCursorInv->Get_Item(0)->Get_ItemNum() == ITEM_SCARLET)
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
		else if (!LSlot_Picked(pt))
		{
			m_bLSlot = false;
		}

		if (RSlot_Picked(pt))
		{
			m_bRSlot = true;

			if (Engine::Button_Down(DIM_LB))
			{
				CInventory* pCursorInv = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_UI", L"UI_Cursor", L"Com_Inventory"));

				vector<CItem*>* pCvecItem = pCursorInv->Get_VecItemP();
				vector<CItem*>* pFvecItem = m_pSecondInventoryCom->Get_VecItemP();

				if (!m_pSecondInventoryCom->Check_Empty(0) && !pCursorInv->Check_Empty(0))
				{
					if ((*pFvecItem)[0]->Get_ItemNum() > ITEM_ETC && (*pFvecItem)[0]->Get_ItemNum() == (*pCvecItem)[0]->Get_ItemNum())
					{
						(*pFvecItem)[0]->Add_Count((*pCvecItem)[0]->Get_Count());
						pCursorInv->Remove_Item(0);
					}
					else if ((*pFvecItem)[0]->Get_ItemNum() != (*pCvecItem)[0]->Get_ItemNum())
						m_pSecondInventoryCom->Swap_Item(&(*pCvecItem)[0], &(*pFvecItem)[0]);
				}
				else
					m_pSecondInventoryCom->Swap_Item(&(*pCvecItem)[0], &(*pFvecItem)[0]);
			}
		}
		else if (!RSlot_Picked(pt))
		{
			m_bRSlot = false;
		}
		Add_RenderGroup(RENDER_UI, this);
	}

	if (!m_pInventoryCom->Check_Empty(0))
	{
		m_fTime -= fTimeDelta * 5.f;

		m_Rect.top = (_long)(m_FirstRect.top + ((_float)(m_FirstRect.bottom - m_FirstRect.top) - ((_float)(m_FirstRect.bottom - m_FirstRect.top) * (m_fTime / 80.f))));

		if (m_fTime <= 0)
		{
			m_fTime = 80.f;

			CItem* pItem = m_pInventoryCom->Get_Item(0);

			ITEMNUM eBarNum = ITEM_END;
			MATERIAL eBarMat = MATERIAL_END;

			switch (pItem->Get_ItemNum())
			{
			case ITEM_COPPER:
				eBarNum = ITEM_COPPER_BAR;
				eBarMat = MATERIAL_COPPER;
				break;

			case ITEM_IRON:
				eBarNum = ITEM_IRON_BAR;
				eBarMat = MATERIAL_IRON;
				break;

			case ITEM_SCARLET:
				eBarNum = ITEM_SCARLET_BAR;
				eBarMat = MATERIAL_SCARLET;
				break;

			}

			CItem* pCraftItem = CCraftMgr::GetInstance()->Craft(m_pInventoryCom, eBarNum, eBarMat);

			m_pSecondInventoryCom->Add_Item(pCraftItem);
		}
	}
	else
	{
		m_fTime = 80.f; 
	}

	return iExit;
}

void CUIFurnace::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIFurnace::Render_GameObject()
{
	_matrix matWorld;

	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	matWorld._11 = 30.f;
	matWorld._22 = 30.f;
	matWorld._41 -= 76.f;
	matWorld._42 += 10.f;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	Set_BarTexture(m_pCBarBufferCom, 0);

	matWorld._41 += 80.f;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	Set_BarTexture(m_pIBarBufferCom, 1);

	matWorld._41 += 80.f;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	Set_BarTexture(m_pSBarBufferCom, 2);

	m_pTransformCom->Get_WorldMatrix(&matWorld);

	matWorld._11 = 30.f;
	matWorld._22 = 30.f;
	matWorld._41 -= 50.f;
	matWorld._42 -= 65.f;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pTextureCom->Set_Texture(1);

	m_pLSlotBufferCom->Render_Buffer();

	if (!m_pInventoryCom->Check_Empty(0))
	{
		matWorld._11 = 20.f;
		matWorld._22 = 20.f;

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

		CItem* pItem = m_pInventoryCom->Get_Item(0);

		_int iMatNum = pItem->Get_TextureNumber();

		pItem->Get_Texture()->Set_Texture(iMatNum);

		pItem->Get_Buffer()->Render_Buffer();

		_int iNum = pItem->Get_Count();

		if (iNum != 1)
		{
			wstring sFont = std::to_wstring(iNum);

			const _tchar* tFont = sFont.c_str();

			_vec2 pos(m_BLRect.right - 10.f, m_BLRect.top + 25.f);

			Engine::Render_Font(L"Font_Item", tFont, &pos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
		}
	}

	if (m_bLSlot)
	{
		matWorld._11 = 30.f;
		matWorld._22 = 30.f;

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

		m_pSelTextureCom->Set_Texture();

		m_pSelBufferCom->Render_Buffer();
	}

	matWorld._11 = 30.f;
	matWorld._22 = 30.f;
	matWorld._41 += 100.f;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pTextureCom->Set_Texture(2);

	m_pRSlotBufferCom->Render_Buffer();
	

	if (!m_pInventoryCom->Check_Empty(0))
	{
		m_pGraphicDev->SetScissorRect(&m_Rect);

		m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);

		m_pTextureCom->Set_Texture(1);

		m_pSciBufferCom->Render_Buffer();

		m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
	}

	if (!m_pInventoryCom->Check_Empty(0) && m_pSecondInventoryCom->Check_Empty(0))
	{
		matWorld._11 = 20.f;
		matWorld._22 = 20.f;

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

		CItem* pItem = m_pInventoryCom->Get_Item(0);

		MATERIAL eMat = pItem->Get_ItemMaterial();

		switch (eMat)
		{
		case MATERIAL_COPPER:
			m_pBarTextureCom->Set_Texture(0);
			break;

		case MATERIAL_IRON:
			m_pBarTextureCom->Set_Texture(1);
			break;

		case MATERIAL_SCARLET:
			m_pBarTextureCom->Set_Texture(2);
			break;
		}

		m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

		m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(100, 255, 255, 255));
		m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
		m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		m_pBackBufferCom->Render_Buffer();

		m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);

		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

		m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	}
	if (!m_pSecondInventoryCom->Check_Empty(0))
	{
		matWorld._11 = 20.f;
		matWorld._22 = 20.f;

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

		CItem* pItem = m_pSecondInventoryCom->Get_Item(0);

		_int iMatNum = pItem->Get_TextureNumber();

		m_pBarTextureCom->Set_Texture(iMatNum);

		pItem->Get_Buffer()->Render_Buffer();

		_int iNum = pItem->Get_Count();

		if (iNum != 1)
		{
			wstring sFont = std::to_wstring(iNum);

			const _tchar* tFont = sFont.c_str();

			_vec2 pos(m_BRRect.right - 10.f, m_BRRect.top + 25.f);

			Engine::Render_Font(L"Font_Item", tFont, &pos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
		}
	}
	if (m_bRSlot)
	{
		matWorld._11 = 30.f;
		matWorld._22 = 30.f;

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

		m_pSelTextureCom->Set_Texture();

		m_pSelBufferCom->Render_Buffer();
	}
}

void CUIFurnace::Set_BarTexture(Engine::CRcTex* _pBufferCom, _int _iIndex)
{
	CInventory* pPlayer = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"Player", L"Com_Inventory"));

	_bool bEnough(false);

	switch (_iIndex)
	{
	case 0:
	{
		if(pPlayer->Enough_Item(ITEM_COPPER, 1))
			bEnough = true;
		break;
	}
	case 1:
	{
		if (pPlayer->Enough_Item(ITEM_IRON, 1))
			bEnough = true;
		break;
	}
	case 2:
	{
		if (pPlayer->Enough_Item(ITEM_SCARLET, 1))
			bEnough = true;
		break;
	}
	}

	if (!bEnough)
	{
		m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

		m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(100, 255, 255, 255));
		m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
		m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		m_pBarTextureCom->Set_Texture(_iIndex);

		_pBufferCom->Render_Buffer();

		m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);

		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

		m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	}
	else
	{
		m_pBarTextureCom->Set_Texture(_iIndex);

		_pBufferCom->Render_Buffer();
	}
}

HRESULT CUIFurnace::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pCBarBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_CBarBuffer", pComponent });

	pComponent = m_pIBarBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_IBarBuffer", pComponent });

	pComponent = m_pSBarBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_SBarBuffer", pComponent });

	pComponent = m_pLSlotBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_LSlotBuffer", pComponent });

	pComponent = m_pRSlotBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_RSlotBuffer", pComponent });

	pComponent = m_pSelBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_SelBuffer", pComponent });

	pComponent = m_pSciBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_SciBuffer", pComponent });

	pComponent = m_pBackBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_BackBuffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIFurnace"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pBarTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_BarTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_BarTexture", pComponent });

	pComponent = m_pSelTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIInvSelected"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_SelTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });

	pComponent = m_pInventoryCom = dynamic_cast<CInventory*>(Engine::Clone_Proto(L"Proto_FurnaceInventory"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Inventory", pComponent });

	pComponent = m_pSecondInventoryCom = dynamic_cast<CInventory*>(Engine::Clone_Proto(L"Proto_FurnaceInventory"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_SecondInventory", pComponent });

	return S_OK;
}

CUIFurnace* CUIFurnace::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize)
{
	CUIFurnace* pUIFurnace = new CUIFurnace(pGraphicDev);

	if (FAILED(pUIFurnace->Ready_GameObject(vPos, vSize)))
	{
		Safe_Release(pUIFurnace);
		MSG_BOX("UIStatus Create Failed");
		return nullptr;
	}

	return pUIFurnace;
}

void CUIFurnace::Free()
{
	Engine::CGameObject::Free();
}
