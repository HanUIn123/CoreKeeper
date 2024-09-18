#include "pch.h"
#include "..\Header\UIItemSlot.h"
#include "Export_System.h"
#include "Export_Utility.h"

CUIItemSlot::CUIItemSlot(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bCollapse(false), m_bFirst(false), m_bWindow(false)

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

	return S_OK;
}

_int CUIItemSlot::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (m_bWindow)
	{

		CInventory* pInventory = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"Player", L"Com_Inventory"));

		m_vecItem = pInventory->Get_VecItem();


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
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	if (m_bCollapse)
	{
		m_pColTextureCom->Set_Texture();
	}
	m_pBufferCom->Render_Buffer();

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

	/*
	pComponent = m_pSTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UISlot"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });*/

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });

	pComponent = m_pColTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIInvSelected"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_ColTexture", pComponent });

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
