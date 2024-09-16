#include "pch.h"
#include "..\Header\UIScreenIcon.h"
#include "Export_System.h"
#include "Export_Utility.h"

CUIScreenIcon::CUIScreenIcon(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bCollapse(false), m_bExit(false)

{
}

CUIScreenIcon::~CUIScreenIcon()
{
}

HRESULT CUIScreenIcon::Ready_GameObject(_vec2 vPos, _vec2 vSize, const _uint iIndex)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	D3DXMatrixIdentity(&ButtonWorld);

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

	m_iIndex = iIndex;

	return S_OK;
}

_int CUIScreenIcon::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	if (Map_Picked(pt))
	{
		if (Engine::Get_DIMouseState(DIM_LB))
		{
			switch (m_iIndex)
			{
			case ICON_BAG:
				break;

			case ICON_MAP:
				break;

			case ICON_HAND:
				break;

			default:
				break;
			}
			//인덱스에 따라 출력되는 창 변경
		}

		if (m_iIndex == ICON_BAG || m_iIndex == ICON_MAP)
		{
			m_bCollapse = true;
		}
	}
	else
		m_bCollapse = false;

	Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CUIScreenIcon::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIScreenIcon::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	//인벤토리 오픈했을때 출력
	if (m_bExit && m_iIndex == ICON_HAND)
	{
		m_pTextureCom->Set_Texture(ICON_EXIT);
	}
	else
	{
		if (m_bCollapse)
			m_pTextureCom->Set_Texture(m_iIndex + 1);
		else
			m_pTextureCom->Set_Texture(m_iIndex);
	}
	m_pBufferCom->Render_Buffer();
}

HRESULT CUIScreenIcon::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIIconTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });

	return S_OK;
}

CUIScreenIcon* CUIScreenIcon::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize, const _uint iIndex)
{
	CUIScreenIcon* pUIScreenIcon = new CUIScreenIcon(pGraphicDev);

	if (FAILED(pUIScreenIcon->Ready_GameObject(vPos, vSize, iIndex)))
	{
		Safe_Release(pUIScreenIcon);
		MSG_BOX("UIScreenIcon Create Failed");
		return nullptr;
	}

	return pUIScreenIcon;
}

void CUIScreenIcon::Free()
{
	Engine::CGameObject::Free();
}
