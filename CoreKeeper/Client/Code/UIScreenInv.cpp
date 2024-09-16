#include "pch.h"
#include "..\Header\UIScreenInv.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "Engine_Enum.h"
#include "..\Header\Item.h"
#include "..\Header\Player.h"

CUIScreenInv::CUIScreenInv(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_iCurInv(0), m_bFirst(true), m_bCollapse(false), m_bMove(false)

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

	_vec2 vSize = { 20.f, 20.f };

	m_pTransformCom->m_vScale = { vSize.x, vSize.y, 1.f };

	m_BRect.left = vPos.x - vSize.x / 2;
	m_BRect.right = vPos.x + vSize.x / 2;
	m_BRect.top = vPos.y - vSize.y / 2;
	m_BRect.bottom = vPos.y + vSize.y / 2;

	m_iIndex = _iIndex;

	if (m_iIndex == 0)
	{
		m_iIndex = 10;
	}

	m_fPosX = x;

	return S_OK;
}

_int CUIScreenInv::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player"));
	NULL_CHECK_RETURN(pPlayer, -1);

	m_iCurInv = pPlayer->Get_iHandNum();

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	_byte byKey;

	switch (m_iIndex)
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

	if (Engine::Get_DIKeyState(byKey))
	{
		pPlayer->Set_iHandNum(m_iIndex);
	}


	if (Map_Picked(pt))
	{
		if (Engine::Get_DIMouseState(DIM_LB))
		{
			m_iCurInv = m_iIndex;

			pPlayer->Set_iHandNum(m_iCurInv);
		}

		m_bCollapse = true;
	}
	else
	{
		m_bCollapse = false;
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

			matWorld._11 = 2.f;
			matWorld._22 = 2.f;

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
			}
		}

		vecItem[m_iIndex - 1]->Get_Texture()->Set_Texture(0);
		
		matWorld._11 = 30.f;
		matWorld._22 = 30.f;

		matWorld._42 -= 5.f;

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

		vecItem[m_iIndex - 1]->Get_Buffer()->Set_Index(vecItem[m_iIndex - 1]->Get_Animator()->Get_MotionIndex());
		
		vecItem[m_iIndex - 1]->Get_Buffer()->Render_Buffer();

		matWorld._42 += 5.f;
	}

	if (m_iIndex == 10)
	{
		m_pNumTextureCom->Set_Texture(0);
	}
	else
		m_pNumTextureCom->Set_Texture(m_iIndex);

	matWorld._11 = 4.f;
	matWorld._22 = 5.f;

	matWorld._41 += 12.f;
	matWorld._42 += 12.f;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pRcTextureCom->Render_Buffer();

}

void CUIScreenInv::Move_Pos()
{

	_float y;

	if (!m_bMove)
	{
		m_pTransformCom->Set_Pos(m_fPosX, 0.f, 0);
		m_bMove = true;

		y = 300.f;
	}
	else
	{
		m_pTransformCom->Set_Pos(m_fPosX, -250.f, 0);
		m_bMove = false;

		y = 550.f;
	}

	_vec3 vPos, vSize;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);


	vSize = { m_pTransformCom->m_vScale.x, m_pTransformCom->m_vScale.y, m_pTransformCom->m_vScale.z };

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
