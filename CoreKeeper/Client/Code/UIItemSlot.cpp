#include "pch.h"
#include "..\Header\UIItemSlot.h"
#include "Export_System.h"
#include "Export_Utility.h"

CUIItemSlot::CUIItemSlot(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bCollapse(false), m_bFirst(false), m_bWindow(false), m_iIndex(0)

{
}

CUIItemSlot::~CUIItemSlot()
{
}

HRESULT CUIItemSlot::Ready_GameObject(_vec2 vPos, _vec2 vSize, SLOTTYPE _eType)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	_D3DVIEWPORT9 Viewport;

	m_pGraphicDev->GetViewport(&Viewport);
	float height = (_float)Viewport.Height;
	float width = (_float)Viewport.Width;

	float x = vPos.x - width / 2;
	float y = height / 2 - vPos.y;

	m_pTransformCom->m_vScale = { vSize.x, vSize.y , 1.f };
	m_pTransformCom->Set_Pos(x, y, 0);


	m_BRect.left = vPos.x - vSize.x / 2;
	m_BRect.right = vPos.x + vSize.x / 2;
	m_BRect.top = vPos.y - vSize.y / 2;
	m_BRect.bottom = vPos.y + vSize.y / 2;

	m_eSlotType = _eType;

	switch (m_eSlotType)
	{
	case SLOT_HELM:
		m_iIndex = 0;
		break;

	case SLOT_CHEST:
		m_iIndex = 1;
		break;

	case SLOT_LEGGINGS:
		m_iIndex = 5;
		break;

	case SLOT_WEAPON:
		m_iIndex = 2;
		break;

	case SLOT_NECKLACE:
		m_iIndex = 6;
		break;

	case SLOT_RING1:
		m_iIndex = 7;
		break;

	case SLOT_RING2:
		m_iIndex = 7;
		break;

	case SLOT_BAG:
		m_iIndex = 3;
		break;

	case SLOT_LANTTERN:
		m_iIndex = 4;
		break;

	case SLOT_PET:
		m_iIndex = 8;
		break;

	default:
		break;
	}


	return S_OK;
}

_int CUIItemSlot::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (m_bWindow)
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(g_hWnd, &pt);

		if (Map_Picked(pt))
		{
			m_bCollapse = true;

			if (Engine::Get_DIMouseState(DIM_LB))
			{
				switch (m_eSlotType)
				{
				case SLOT_HELM:

					break;

				case SLOT_CHEST:

					break;

				case SLOT_LEGGINGS:

					break;

				case SLOT_WEAPON:

					break;

				case SLOT_NECKLACE:

					break;

				case SLOT_RING1:

					break;

				case SLOT_RING2:

					break;

				case SLOT_BAG:

					break;

				case SLOT_LANTTERN:

					break;

				case SLOT_PET:

					break;

				default:
					break;
				}
			}

		}
		else
			m_bCollapse = false;

	}
	return iExit;
}

void CUIItemSlot::LateUpdate_GameObject()
{
	if (m_bWindow)
	{
		Engine::Add_RenderGroup(RENDER_UI, this);
	}
	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIItemSlot::Render_GameObject()	
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

	CInventory* pPlayerInv = dynamic_cast<Engine::CInventory*>
		(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"Player", L"Com_EquipInventory"));

	vector<CItem*> vecItem = pPlayerInv->Get_VecItem();

	if (!pPlayerInv->Check_Empty(m_eSlotType))
	{
		_int iCount = vecItem[m_iIndex - 1]->Get_Count();

		vecItem[m_iIndex - 1]->Get_Texture()->Set_Texture();

		Engine::ITEMNUM eNum = vecItem[m_iIndex - 1]->Get_ItemNum();

		switch (eNum)
		{
		case ITEM_HELMET:
			break;

		case ITEM_CHEST:
			break;

		dafault:
			break;
		}

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

		vecItem[m_iIndex - 1]->Get_Buffer()->Render_Buffer();
	}
	else
	{
		m_pSlotBufferCom->Set_Index(m_iIndex);

		m_pSlotTextureCom->Set_Texture();

		m_pSlotBufferCom->Render_Buffer();
	}
	//m_pTextureCom->Set_Texture();

}

HRESULT CUIItemSlot::Add_Component()
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

	return S_OK;
}

CUIItemSlot* CUIItemSlot::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize, SLOTTYPE _eType)
{
	CUIItemSlot* pUIItemslot = new CUIItemSlot(pGraphicDev);

	if (FAILED(pUIItemslot->Ready_GameObject(vPos, vSize, _eType)))
	{
		Safe_Release(pUIItemslot);
		MSG_BOX("UIStatus Create Failed");
		return nullptr;
	}

	return pUIItemslot;
}

void CUIItemSlot::Free()
{
	Engine::CGameObject::Free();
}
