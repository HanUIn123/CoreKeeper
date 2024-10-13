#include "pch.h"
#include "..\Header\UICursor.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\Player.h"

CUICursor::CUICursor(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bDisable(false), m_bFirst(true)

{
}

CUICursor::~CUICursor()
{
}

HRESULT CUICursor::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(10.f, 10.f, 0);

	return S_OK;
}

_int CUICursor::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	//if (m_bFirst)
	//{
	//	m_bDisable = false;

	//	m_bFirst = false;
	//}

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	if (!m_bDisable)
	{
		_D3DVIEWPORT9 Viewport;

		m_pGraphicDev->GetViewport(&Viewport);
		float height = (_float)Viewport.Height;
		float width = (_float)Viewport.Width;

		float x = (_float)pt.x - width / 2 + 6.f;
		float y = height / 2 - (_float)pt.y - 6.f;

		m_pTransformCom->Set_Pos(x, y, 0);
	}

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player"));

	for (int i = 0; i < 13; i++)
	{
		if (Map_Picked(pt, i))
		{
			pPlayer->Set_DisMove();
			break;
		}
		else if (!Map_Picked(pt, i))
			pPlayer->Set_EnaMove();
	}

	return iExit;
}

void CUICursor::LateUpdate_GameObject()
{
	if (!m_bDisable)
	{
		Add_RenderGroup(RENDER_SUBORDINATE, this);
	}

	Engine::CGameObject::LateUpdate_GameObject();
}

void CUICursor::Render_GameObject()
{
	_matrix matWorld;

	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
	
	m_pTextureCom->Set_Texture();
	
	m_pBufferCom->Render_Buffer();
	
	if (!m_pInventoryCom->Check_Empty(0))
	{
	
		m_pItem = m_pInventoryCom->Get_Item(0);

		_int iCount = m_pItem->Get_Count();

		ITEMNUM eNum = m_pItem->Get_ItemNum();

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

		default:
			matWorld._11 = 15.f;
			matWorld._22 = 15.f;
			break;
		}

		matWorld._41 -= 25.f;
		matWorld._42 += 10.f;

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

		Engine::MATERIAL material = m_pItem->Get_ItemMaterial();


		//if (material != MATERIAL_END)
			m_pItem->Get_Texture()->Set_Texture(m_pItem->Get_TextureNumber());
		//else if (material == MATERIAL_SPECIAL)
		//	m_pItem->Get_Texture()->Set_Texture(material);
		//else
		//	m_pItem->Get_Texture()->Set_Texture();

		m_pItem->Get_Buffer()->Render_First();

		if (iCount != 1)
		{
			wstring sFont = std::to_wstring(iCount);

			const _tchar* tFont = sFont.c_str();

			D3DVIEWPORT9 ViewPort;

			m_pGraphicDev->GetViewport(&ViewPort);
			float height = (_float)ViewPort.Height;
			float width = (_float)ViewPort.Width;

			float x = (matWorld._41 + 2.f) + width / 2;
			float y = height / 2 - (matWorld._42);

			_vec2 vPos = { x, y };

			Engine::Render_Font(L"Font_Inv", tFont, &vPos, D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f));
		}
	}
	
}

HRESULT CUICursor::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UICursor"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });

	pComponent = m_pInventoryCom = dynamic_cast<CInventory*>(Engine::Clone_Proto(L"Proto_MouseInventory"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Inventory", pComponent });

	return S_OK;
}

CUICursor* CUICursor::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CUICursor* pUICursor = new CUICursor(pGraphicDev);

	if (FAILED(pUICursor->Ready_GameObject()))
	{
		Safe_Release(pUICursor);
		MSG_BOX("pUICursor Create Failed");
		return nullptr;
	}

	return pUICursor;
}

void CUICursor::Free()
{
	Engine::CGameObject::Free();
}
