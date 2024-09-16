#include "pch.h"
#include "..\Header\UIInventory.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Enum.h"
#include "..\Header\Item.h"
#include "..\Header\Player.h"

CUIInventory::CUIInventory(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_iCurInv(0), m_bFirst(true), m_bCollapse(false)

{
}

CUIInventory::~CUIInventory()
{
}

HRESULT CUIInventory::Ready_GameObject(_vec2 vPos, _int _iIndex)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	_D3DVIEWPORT9 Viewport;

	m_pGraphicDev->GetViewport(&Viewport);
	float height = (_float)Viewport.Height;
	float width = (_float)Viewport.Width;

	float x = vPos.x - width / 2;
	float y = height / 2 - vPos.y;

	m_pTransformCom->Set_Pos(x, y, 0);

	_vec2 vSize = { 25.f, 25.f };

	m_pTransformCom->m_vScale = { vSize.x, vSize.y, 1.f };

	m_BRect.left = vPos.x - vSize.x / 2;
	m_BRect.right = vPos.x + vSize.x / 2;
	m_BRect.top = vPos.y - vSize.y / 2;
	m_BRect.bottom = vPos.y + vSize.y / 2;

	m_iIndex = _iIndex;

	return S_OK;
}

_int CUIInventory::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	if (Map_Picked(pt))
	{
		if (Engine::Get_DIMouseState(DIM_LB))
		{
			m_iCurInv = m_iIndex;

		}

		m_bCollapse = true;
	}
	else
	{
		m_bCollapse = false;
	}

	Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CUIInventory::LateUpdate_GameObject()
{
	

	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIInventory::Render_GameObject()
{

	_matrix matWorld;
	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
	
	if (m_iCurInv == m_iIndex)
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

	vector<CItem*> vecItem = pPlayerInv->Get_VecItem();

	if (!pPlayerInv->Check_Empty(m_iIndex))
	{
		_int iCount = vecItem[m_iIndex - 1]->Get_Count();

		if (iCount != 1)
		{
			_int iFront = iCount % 10;
			_int iBack;

			if (iFront > 0)
			{
				iBack = iCount / 10;
			}

			m_pItemNumTextureCom->Set_Texture(iBack);

			matWorld._11 = 5.f;
			matWorld._22 = 5.f;

			//matWorld._41 +=
			matWorld._42 += 5.f;

			m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

			m_pRcTextureCom->Render_Buffer();

			if (iFront > 0)
			{
				m_pItemNumTextureCom->Set_Texture(iFront);

				matWorld._41 -= 2.f;

				m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

				m_pRcTextureCom->Render_Buffer();

				matWorld._41 += 2.f;
			}
			matWorld._42 -= 5.f;
		}

		vecItem[m_iIndex - 1]->Get_Texture()->Set_Texture();

		matWorld._11 = 30.f;
		matWorld._22 = 30.f;

		matWorld._42 -= 5.f;

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

		vecItem[m_iIndex - 1]->Get_Buffer()->Render_Buffer();

		matWorld._42 += 5.f;
	}

	m_pNumTextureCom->Set_Texture(m_iIndex);

	matWorld._11 = 5.f;
	matWorld._22 = 5.f;

	matWorld._41 += 15.f;
	matWorld._42 += 15.f;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pRcTextureCom->Render_Buffer();
	
}

HRESULT CUIInventory::Add_Component()
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

CUIInventory* CUIInventory::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _int _iIndex)
{
	CUIInventory* pUIInventory = new CUIInventory(pGraphicDev);

	if (FAILED(pUIInventory->Ready_GameObject(vPos, _iIndex)))
	{
		Safe_Release(pUIInventory);
		MSG_BOX("UIInventory Create Failed");
		return nullptr;
	}

	return pUIInventory;
}

void CUIInventory::Free()
{
	Engine::CGameObject::Free();
}
