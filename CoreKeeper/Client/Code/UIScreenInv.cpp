#include "pch.h"
#include "..\Header\UIScreenInv.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Enum.h"
#include "..\Header\Item.h"
#include "..\Header\Player.h"
#include "..\Header\UICursor.h"
#include "..\Header\UIItemFrame.h"

CUIScreenInv::CUIScreenInv(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_iCurInv(0), m_bFirst(true), m_bCollapse(false), m_bMove(false), m_bStay(false)

{
}

CUIScreenInv::~CUIScreenInv()
{
}

HRESULT CUIScreenInv::Ready_GameObject(_vec2 vPos, _int _iIndex)
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

	/*
	m_BRect.left = vPos.x - vSize.x;
	m_BRect.right = vPos.x + vSize.x;
	m_BRect.top = vPos.y - vSize.y;
	m_BRect.bottom = vPos.y + vSize.y;
	*/
	
	m_BRect.left = vPos.x - vSize.x / 2;
	m_BRect.right = vPos.x + vSize.x / 2;
	m_BRect.top = vPos.y - vSize.y / 2;
	m_BRect.bottom = vPos.y + vSize.y / 2;
	
	m_iIndex = _iIndex;
	m_fPosX = x;

	return S_OK;
}

_int CUIScreenInv::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);


	if (m_bFirst)
	{
		CUICursor* pCursor = dynamic_cast<CUICursor*>(Engine::Get_GameObject(L"Layer_UI", L"UI_Cursor"));
		pCursor->Set_Rect(m_iIndex - 1, m_BRect);

		m_bFirst = false;
	}

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player"));
	NULL_CHECK_RETURN(pPlayer, -1);

	m_iCurInv = pPlayer->Get_iHandNum() + 1;

	//if (m_iCurInv > 10)
	//	m_iCurInv = 0;

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	_int iIndex = m_iIndex + 1;

	if (iIndex == 10)
		iIndex = 0;

	_byte byKey;

	switch (iIndex)
	{
	case 0:
		byKey = DIK_0;
		break;

	case 1:
		byKey = DIK_1;
		break;

	case 2:
		byKey = DIK_2;
		break;

	case 3:
		byKey = DIK_3;
		break;

	case 4:
		byKey = DIK_4;
		break;

	case 5:
		byKey = DIK_5;
		break;

	case 6:
		byKey = DIK_6;
		break;

	case 7:
		byKey = DIK_7;
		break;

	case 8:
		byKey = DIK_8;
		break;

	case 9:
		byKey = DIK_9;
		break;

	default:
		byKey = DIK_0;
		break;

	}


	if(Map_Picked(pt))
	{
		CInventory* pPlayerInv = dynamic_cast<Engine::CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"Player", L"Com_Inventory"));

		if (pPlayer->Get_InvWindow() && Engine::Button_Down(DIM_LB))
		{
			CInventory* pCursorInv = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_UI", L"UI_Cursor", L"Com_Inventory"));

			vector<CItem*>* pCvecItem = pCursorInv->Get_VecItemP();
			vector<CItem*>* pPvecItem = pPlayerInv->Get_VecItemP();

			_int iIndex = m_iIndex - 1;

			if (iIndex == -1)
				iIndex = 9;

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
		else if (Engine::Button_Down(DIM_LB))
		{
			m_iCurInv = m_iIndex - 1;

			if (m_iCurInv == -1)
				m_iCurInv = 9;

			pPlayer->Set_iHandNum(m_iCurInv);
		}

		if (!pPlayerInv->Check_Empty(iIndex) && !m_bStay)
		{
			m_pItem = pPlayerInv->Get_Item(iIndex);

			CUIItemFrame* pItemF = dynamic_cast<CUIItemFrame*>(Engine::Get_GameObject(L"Layer_UI", L"UI_ItemFrame"));

			pItemF->Set_Window(m_pItem);

			m_bStay = true;
		}

		m_bCollapse = true;
	}
	else if (!Map_Picked(pt))
	{
		if (m_bCollapse)
		{
			CUIItemFrame* pItemF = dynamic_cast<CUIItemFrame*>(Engine::Get_GameObject(L"Layer_UI", L"UI_ItemFrame"));

			pItemF->Set_Window(m_pItem);

			m_bStay = false;
		}
		m_bCollapse = false;
	}


	if (Engine::Get_DIKeyState(byKey))
	{
		pPlayer->Set_iHandNum(m_iIndex);
	}
	

	return iExit;
}

void CUIScreenInv::LateUpdate_GameObject()
{
	Add_RenderGroup(RENDER_UI, this);

	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIScreenInv::Render_GameObject()
{

	_matrix matWorld;
	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	if (m_iCurInv == 10 && m_iIndex == 0)
	{
		m_pTextureCom->Set_Texture(1);
	}
	else if (m_iCurInv == m_iIndex)
	{
		m_pTextureCom->Set_Texture(1);
	}
	else
		m_pTextureCom->Set_Texture();
		
	m_pRcTextureCom->Render_Buffer();

	if (m_bCollapse && (m_iCurInv != m_iIndex))
	{
		m_pColTextureCom->Set_Texture();

		m_pRcTextureCom->Render_Buffer();
	}

	CInventory* pPlayerInv = dynamic_cast<Engine::CInventory*>
		(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"Player", L"Com_Inventory"));

	_int iIndex = m_iIndex - 1;

	if (iIndex == -1)
		iIndex = 9;

	if (!pPlayerInv->Check_Empty(iIndex))
	{
		m_pItem = pPlayerInv->Get_Item(iIndex);

		_int iCount = m_pItem->Get_Count();
		
		if (iCount != 1)
		{
			wstring sFont = std::to_wstring(iCount);

			const _tchar* tFont = sFont.c_str();

			_vec2 pos(m_BRect.right - 1.f, m_BRect.top + 12.f);

			Engine::Render_Font(L"Font_Item", tFont, &pos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
		}

		m_pItem->Get_Texture()->Set_Texture();

		//_vec3 vScale = m_pItem->Get_Transform()->m_vScale;
		
		Engine::ITEMNUM eNum = m_pItem->Get_ItemNum();

		switch (eNum)
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

		dafault:
			matWorld._11 = 20.f;
			matWorld._22 = 20.f;
			break;
		}

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

		m_pItem->Get_Buffer()->Render_First();


	}

	if (iIndex == 10)
	{
		wstring sFont = std::to_wstring(0);

		const _tchar* tFont = sFont.c_str();

		_vec2 pos(m_BRect.right - 5.f, m_BRect.top - 8.f);

		Engine::Render_Font(L"Font_Inv", tFont, &pos, D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f));
	}
	else
	{
		wstring sFont = std::to_wstring(m_iIndex);

		const _tchar* tFont = sFont.c_str();

		_vec2 pos(m_BRect.right - 5.f, m_BRect.top - 8.f);

		Engine::Render_Font(L"Font_Inv", tFont, &pos, D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f));
	}

}

void CUIScreenInv::Move_Pos()
{

	_float y;


	if (!m_bMove)
	{
		m_pTransformCom->Set_Pos(m_fPosX, -39.f, 0);
		m_bMove = true;

		y = 399.f;
	}
	else
	{
		m_pTransformCom->Set_Pos(m_fPosX, -290.f, 0);
		m_bMove = false;

		y = 650.f;
	}

	_vec3 vPos, vSize;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);


	vSize = { m_pTransformCom->Get_Scale()->x, m_pTransformCom->Get_Scale()->y, m_pTransformCom->Get_Scale()->z };

	m_BRect.top = y - vSize.y / 2;
	m_BRect.bottom = y + vSize.y / 2;
}

HRESULT CUIScreenInv::Add_Component()
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

CUIScreenInv* CUIScreenInv::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _int _iIndex)
{
	CUIScreenInv* pUIScreenInv = new CUIScreenInv(pGraphicDev);

	if (FAILED(pUIScreenInv->Ready_GameObject(vPos, _iIndex)))
	{
		Safe_Release(pUIScreenInv);
		MSG_BOX("UIScreenInv Create Failed");
		return nullptr;
	}

	return pUIScreenInv;
}

void CUIScreenInv::Free()
{
	Engine::CGameObject::Free();
}
