#include "pch.h"
#include "..\Header\UIJemSlot.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "../Header/Statue.h"

CUIJemSlot::CUIJemSlot(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bCollapse(false), m_bFirst(false), m_bWindow(false)

{
}

CUIJemSlot::~CUIJemSlot()
{
}

HRESULT CUIJemSlot::Ready_GameObject(_vec2 vPos, _vec2 vSize)
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

_int CUIJemSlot::Update_GameObject(const _float& fTimeDelta)
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

			if (Engine::Button_Up(DIM_LB))
			{
				CUIStatue* pUIStatue = dynamic_cast<CUIStatue*>(Engine::Get_GameObject(L"Layer_UI", L"UI_Statue"));
				CInventory* pCursor = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_UI", L"UI_Cursor", L"Com_Inventory"));

				CItem* pItem = pCursor->Get_Item(0);

				Engine::ITEMNUM eNum = pItem->Get_ItemNum();

				_int i = pUIStatue->Get_StatueType();

				if (i == 0 && eNum == ITEM_SLIME_CORE || i == 2 && eNum == ITEM_LARVA_CORE || i == 4 && eNum == ITEM_MAL_CORE)
				{
					switch (i)
					{
					case 0:
					{
						CStatue* pStatue = dynamic_cast<CStatue*>(Engine::Get_GameObject(L"Layer_Environment", L"SlimeStatue"));
						pStatue->Set_Active();
						pCursor->Minus_Item(ITEM_SLIME_CORE, 1);
						pUIStatue->Set_eType(1);
						break;
					}
					case 2:
					{
						CStatue* pStatue = dynamic_cast<CStatue*>(Engine::Get_GameObject(L"Layer_Environment", L"LarvaStatue"));
						pStatue->Set_Active();
						pCursor->Minus_Item(ITEM_LARVA_CORE, 1);
						pUIStatue->Set_eType(3);
						break;
					}
					case 4:
					{
						CStatue* pStatue = dynamic_cast<CStatue*>(Engine::Get_GameObject(L"Layer_Environment", L"MalugazStatue"));
						pStatue->Set_Active();
						pCursor->Minus_Item(ITEM_MAL_CORE, 1);
						pUIStatue->Set_eType(5);
						break;
					}
					}
				}
			}
		}
		else
			m_bCollapse = false;
	}
	return iExit;
}

void CUIJemSlot::LateUpdate_GameObject()
{
	if(m_bWindow)
		Engine::Add_RenderGroup(RENDER_UI, this);
	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIJemSlot::Render_GameObject()
{
	_matrix matWorld;

	m_pTransformCom->Get_WorldMatrix(&matWorld);

	if (m_iIndex == 0)
	{
		matWorld._42 -= 18.f;
	}

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	if (m_bCollapse)
	{
		m_pColTextureCom->Set_Texture();

		m_pBufferCom->Render_Buffer();
	}

}

HRESULT CUIJemSlot::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });
	
	pComponent = m_pColTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIInvSelected"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_ColTexture", pComponent });

	return S_OK;
}

CUIJemSlot* CUIJemSlot::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize)
{
	CUIJemSlot* pUIJemSlot = new CUIJemSlot(pGraphicDev);

	if (FAILED(pUIJemSlot->Ready_GameObject(vPos, vSize)))
	{
		Safe_Release(pUIJemSlot);
		MSG_BOX("UIStatus Create Failed");
		return nullptr;
	}

	return pUIJemSlot;
}

void CUIJemSlot::Free()
{
	Engine::CGameObject::Free();
}
