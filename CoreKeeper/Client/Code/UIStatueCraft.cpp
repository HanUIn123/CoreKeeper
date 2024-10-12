#include "pch.h"
#include "..\Header\UIStatueCraft.h"
#include "..\Header\CraftMgr.h"
#include "../Header/UIItemFrame.h"

CUIStatueCraft::CUIStatueCraft(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bCollapse(false), m_bFirst(false), m_bWindow(false), m_iIndex(0), m_bEnough(false), m_bStay(false)

{
}

CUIStatueCraft::~CUIStatueCraft()
{
}

HRESULT CUIStatueCraft::Ready_GameObject(_vec2 vPos, _vec2 vSize)
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

	return S_OK;
}

_int CUIStatueCraft::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (m_bWindow)
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(g_hWnd, &pt);

		CInventory* pPlayer = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"Player", L"Com_Inventory"));

		switch (m_iIndex)
		{
		case 1:
			if (CCraftMgr::GetInstance()->Craftable(pPlayer, ITEM_PLAYER_SPAWNER, MATERIAL_END))
				m_bEnough = true;
			else
				m_bEnough = false;
			break;

		case 3:
			if (CCraftMgr::GetInstance()->Craftable(pPlayer, ITEM_MAL_SPAWNER, MATERIAL_END))
				m_bEnough = true;
			else
				m_bEnough = false;
			break;

		case 5:
			if (CCraftMgr::GetInstance()->Craftable(pPlayer, ITEM_AZEOS_SPAWNER, MATERIAL_END))
				m_bEnough = true;	
			else
				m_bEnough = false;
			break;
		}

		if (Map_Picked(pt))
		{
			m_bCollapse = true;

			if (!m_bStay)
			{
				switch (m_iIndex)
				{
				case 1:
					m_pInventoryCom->Add_Item(CCraftMgr::GetInstance()->CraftExp(ITEM_PLAYER_SPAWNER, MATERIAL_END));
					break;

				case 3:
					m_pInventoryCom->Add_Item(CCraftMgr::GetInstance()->CraftExp(ITEM_MAL_SPAWNER, MATERIAL_END));
					break;

				case 5:
					m_pInventoryCom->Add_Item(CCraftMgr::GetInstance()->CraftExp(ITEM_AZEOS_SPAWNER, MATERIAL_END));
					break;
				}

				CUIItemFrame* pFrame = dynamic_cast<CUIItemFrame*>(Engine::Get_GameObject(L"Layer_UI", L"UI_ItemFrame"));

				pFrame->Set_Window(m_pInventoryCom->Get_Item(0), pt, true);

				m_bStay = true;
			}

			if (Engine::Button_Down(DIM_LB))
			{
				CInventory* pCursor = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_UI", L"UI_Cursor", L"Com_Inventory"));

				if (pCursor->Check_Empty(0))
				{
					if (m_bEnough)
					{
						ITEMNUM eNum = ITEM_END;

						switch (m_iIndex)
						{
						case 1:
							eNum = ITEM_PLAYER_SPAWNER;
							break;
						case 3:
							eNum = ITEM_MAL_SPAWNER;
							break;
						case 5:
							eNum = ITEM_AZEOS_SPAWNER;
							break;
						}

						pCursor->Add_Item(CCraftMgr::GetInstance()->Craft(pPlayer, eNum, MATERIAL_END));
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

void CUIStatueCraft::LateUpdate_GameObject()
{
	if(m_bWindow)
		Engine::Add_RenderGroup(RENDER_UI, this);
	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIStatueCraft::Render_GameObject()
{
	_matrix matWorld;

	m_pTransformCom->Get_WorldMatrix(&matWorld);

	if (m_iIndex == 0)
	{
		matWorld._42 -= 8.f;
	}

	if (m_iIndex == 1)
	{
		matWorld._42 -= 8.f;
	}

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	if (m_iIndex != 0 && m_iIndex != 2 && m_iIndex != 4)
	{ 
		if (m_bCollapse)
		{
			m_pColTextureCom->Set_Texture();

			m_pBufferCom->Render_Buffer();
		}
	
		matWorld._11 = 25.f;
		matWorld._22 = 25.f;

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

		m_pTextureCom->Set_Texture(m_iIndex / 2);
		if (m_bEnough)
		{
			m_pBufferCom->Render_Buffer();

		}
		else
		{
			m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

			m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(100, 255, 255, 255));
			m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
			m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

			m_pBufferCom->Render_Buffer();

			m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);

			m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

			m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		}
	}
}

HRESULT CUIStatueCraft::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	
	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_SpawnerTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });
	
	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });
	

	pComponent = m_pColTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIInvSelected"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_ColTexture", pComponent });

	pComponent = m_pInventoryCom = dynamic_cast<CInventory*>(Engine::Clone_Proto(L"Proto_OneSlotInventory"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Inventory", pComponent });

	/*
	pComponent = m_pArrowTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UITrashCan"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_ArrowTexture", pComponent });
	*/

	return S_OK;
}

CUIStatueCraft* CUIStatueCraft::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize)
{
	CUIStatueCraft* pUIStatueCraft = new CUIStatueCraft(pGraphicDev);

	if (FAILED(pUIStatueCraft->Ready_GameObject(vPos, vSize)))
	{
		Safe_Release(pUIStatueCraft);
		MSG_BOX("UIStatus Create Failed");
		return nullptr;
	}

	return pUIStatueCraft;
}

void CUIStatueCraft::Free()
{
	Engine::CGameObject::Free();
}
