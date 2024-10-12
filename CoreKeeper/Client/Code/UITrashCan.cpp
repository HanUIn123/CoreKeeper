#include "pch.h"
#include "..\Header\UITrashCan.h"
#include "Export_System.h"
#include "Export_Utility.h"

CUITrashCan::CUITrashCan(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bCollapse(false), m_bFirst(false), m_bWindow(false), m_iIndex(0)

{
}

CUITrashCan::~CUITrashCan()
{
}

HRESULT CUITrashCan::Ready_GameObject(_vec2 vPos, _vec2 vSize)
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

	_float vPosy = vPos.y + 30.f;

	m_BRect.left = vPos.x - vSize.x / 2;
	m_BRect.right = vPos.x + vSize.x / 2;
	m_BRect.top = vPosy - vSize.y / 2;
	m_BRect.bottom = vPosy + vSize.y / 2;

	return S_OK;
}

_int CUITrashCan::Update_GameObject(const _float& fTimeDelta)
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

			if (Engine::Button_Down(DIM_LB))
			{
				Engine::CSoundMgr::GetInstance()->Play(L"thud.wav", SOUND_EFFECT, 0.2f);
				m_pInventory->Remove_Item(0);
			}
		}
		else
			m_bCollapse = false;

		Engine::Add_RenderGroup(RENDER_UI, this);
	}

	return iExit;
}

void CUITrashCan::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CUITrashCan::Render_GameObject()
{
	_matrix matWorld;

	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pCanTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	matWorld._11 = 18.f;
	matWorld._22 = 20.f;

	matWorld._42 -= 33.f;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	if (m_bCollapse)
	{
		m_pCanTextureCom->Set_Texture(3);

		m_pBufferCom->Render_Buffer();
	}
	else
	{
		m_pCanTextureCom->Set_Texture(2);

		m_pBufferCom->Render_Buffer();
	}

}

HRESULT CUITrashCan::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pCanTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UITrashCan"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_CanTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });

	pComponent = m_pInventory = dynamic_cast<CInventory*>(Engine::Clone_Proto(L"Proto_TrashInventory"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_TrashInventory", pComponent });

	/*
	pComponent = m_pColTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIInvSelected"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_ColTexture", pComponent });

	pComponent = m_pSlotTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UISilhouette"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_SlotTexture", pComponent });
	*/
	return S_OK;
}

CUITrashCan* CUITrashCan::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize)
{
	CUITrashCan* pUITrashCan = new CUITrashCan(pGraphicDev);

	if (FAILED(pUITrashCan->Ready_GameObject(vPos, vSize)))
	{
		Safe_Release(pUITrashCan);
		MSG_BOX("UIStatus Create Failed");
		return nullptr;
	}

	return pUITrashCan;
}

void CUITrashCan::Free()
{
	Engine::CGameObject::Free();
}
