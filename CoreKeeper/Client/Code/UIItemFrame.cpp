#include "pch.h"
#include "..\Header\UIItemFrame.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "../Header/CraftMgr.h"

CUIItemFrame::CUIItemFrame(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bWindow(false), m_bCollapse(false), m_bCraft(false)

{
	m_vPos = { 0, 0 };
}

CUIItemFrame::~CUIItemFrame()
{
}

HRESULT CUIItemFrame::Ready_GameObject(_vec2 vPos, _vec2 vSize)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	_D3DVIEWPORT9 Viewport;

	m_pGraphicDev->GetViewport(&Viewport);
	float height = (_float)Viewport.Height;
	float width = (_float)Viewport.Width;

	float x = vPos.x - width / 2;
	float y = height / 2 - vPos.y;

	m_pTransformCom->Set_Scale(vSize.x, vSize.y, 1.f);
	m_pTransformCom->Set_Pos(x, y, 0);

	m_BRect.left = _long(vPos.x - (vSize.x - 2.f));
	m_BRect.right = _long(vPos.x + (vSize.x - 2.f));
	m_BRect.top = _long(vPos.y - (vSize.y - 2.f));
	m_BRect.bottom = _long(vPos.y + (vSize.y - 2.f));

	m_vPos = { vPos.x - 240.f, vPos.y - 150.f };

	return S_OK;
}

_int CUIItemFrame::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (m_bWindow)
	{

		if (Engine::Get_DIMouseMove(DIMS_Z) < 0)
		{
			m_vPos.y += 8.f;
		}
		else if (Engine::Get_DIMouseMove(DIMS_Z) > 0 && m_vPos.y >= 350.f)
		{
			m_vPos.y -= 8.f;
		}
	}
	return iExit;
}

void CUIItemFrame::LateUpdate_GameObject()
{
	if (m_bWindow)
	{
		Engine::Add_RenderGroup(RENDER_SUBORDINATE, this);
	}
	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIItemFrame::Render_GameObject()
{
	if (m_pItem)
	{
		m_pGraphicDev->SetScissorRect(&m_BRect);

		_matrix matWorld;

		m_pTransformCom->Get_WorldMatrix(&matWorld);

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

		m_pSelTextureCom->Set_Texture();

		m_pBufferCom->Render_Buffer();

		_vec2 vFPos = m_vPos;
		wstring wsEmpty = L"";

		const wstring  sName = m_pItem->Get_Name();
		const wstring* sFont = m_pItem->Get_Explain();

		const _tchar* tFont[30];

		Engine::Render_Font(L"Font_Status", sName.c_str(), &vFPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
		vFPos.y += 40.f;

		for (int i = 0; i < 30; i++)
		{
			tFont[i] = sFont[i].c_str();

			if (i == 0) {
				Engine::Render_Font(L"Font_Status", tFont[i], &vFPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

				vFPos.y += 20.f;
			}
			else
			{
				Engine::Render_Font(L"Font_Status", tFont[i], &vFPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
				vFPos.y += 20.f;
			}


			if (sFont[i] == wsEmpty)
			{
				break;
			}
		}

		if (m_bCraft)
		{
			CCraftMgr::Recipe sItemRecipe;
			if(m_pItem->Get_ItemNum() == ITEM_COOKINGPOT)
				sItemRecipe = CCraftMgr::GetInstance()->Get_Recipe(make_pair(m_pItem->Get_ItemNum(), MATERIAL_COPPER));
			else if (m_pItem->Get_ItemNum() == ITEM_FURNACE)
				sItemRecipe = CCraftMgr::GetInstance()->Get_Recipe(make_pair(m_pItem->Get_ItemNum(), MATERIAL_WOOD));
			else if (m_pItem->Get_ItemNum() == ITEM_ACCESSORY_TABLE)
				sItemRecipe = CCraftMgr::GetInstance()->Get_Recipe(make_pair(m_pItem->Get_ItemNum(), MATERIAL_COPPER));
			else if (m_pItem->Get_ItemNum() == ITEM_POTION_TABLE)
				sItemRecipe = CCraftMgr::GetInstance()->Get_Recipe(make_pair(m_pItem->Get_ItemNum(), MATERIAL_IRON));
			else if (m_pItem->Get_ItemNum() == ITEM_SPRINKLER)
				sItemRecipe = CCraftMgr::GetInstance()->Get_Recipe(make_pair(m_pItem->Get_ItemNum(), MATERIAL_IRON));
			else if (m_pItem->Get_ItemNum() == ITEM_POTION_ATT || m_pItem->Get_ItemNum() == ITEM_POTION_HP || m_pItem->Get_ItemNum() == ITEM_POTION_DEF)
				sItemRecipe = CCraftMgr::GetInstance()->Get_Recipe(make_pair(m_pItem->Get_ItemNum(), MATERIAL_WOOD));
			else
				sItemRecipe = CCraftMgr::GetInstance()->Get_Recipe(make_pair(m_pItem->Get_ItemNum(), m_pItem->Get_ItemMaterial()));

			for (auto iter : sItemRecipe.vecIngredients)
			{
				const wstring sMFont = CCraftMgr::GetInstance()->Get_IngredientName(iter.eIngredient) + L" " + std::to_wstring(iter.iQuantity);

				const _tchar* tMFont = sMFont.c_str();

				Engine::Render_Font(L"Font_Status", tMFont, &vFPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

				vFPos.y += 20.f;
			}
		}

		m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
	}
}

void CUIItemFrame::Set_Window(CItem* _pItem, POINT _pt, _bool _bCraft)
{
	m_bWindow = true;

	m_pItem = _pItem;

	m_bCraft = _bCraft;

	if ((_float)_pt.x > WINCX / 2.f)
	{
		m_pTransformCom->Set_Pos(280.f - WINCX / 2, WINCY / 2 - 510.f, 0.f);

		m_vPos = { 280.f - 240.f, 510.f - 150.f };
	}
	else
	{
		m_pTransformCom->Set_Pos(1000.f - WINCX / 2, WINCY / 2 - 510.f,  0.f);

		m_vPos = { 1000.f - 240.f, 510.f - 150.f };
	}
}

HRESULT CUIItemFrame::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pItemBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_ItemBuffer", pComponent });

	pComponent = m_pSelTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIItemFrame"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_StatusTexture", pComponent });
	

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });

	return S_OK;
}

CUIItemFrame* CUIItemFrame::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize)
{
	CUIItemFrame* pUIItemFrame = new CUIItemFrame(pGraphicDev);

	if (FAILED(pUIItemFrame->Ready_GameObject(vPos, vSize)))
	{
		Safe_Release(pUIItemFrame);
		MSG_BOX("UIStatus Create Failed");
		return nullptr;
	}

	return pUIItemFrame;
}

void CUIItemFrame::Free()
{
	Engine::CGameObject::Free();
}
