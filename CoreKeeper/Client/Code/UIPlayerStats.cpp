#include "pch.h"
#include "..\Header\UIPlayerStats.h"
#include "..\Header\UIStatusFrame.h"
#include "Export_System.h"
#include "Export_Utility.h"

CUIPlayerStats::CUIPlayerStats(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bWindow(false), m_bCollapse(false), m_bPushed(false)

{
}

CUIPlayerStats::~CUIPlayerStats()
{
}

HRESULT CUIPlayerStats::Ready_GameObject(_vec2 vPos, _vec2 vSize)
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

	m_BRect.left = vPos.x - vSize.x / 2;
	m_BRect.right = vPos.x + vSize.x / 2;
	m_BRect.top = vPos.y - vSize.y / 2;
	m_BRect.bottom = vPos.y + vSize.y / 2;

	return S_OK;
}

_int CUIPlayerStats::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	if (m_bWindow)
	{
		if (Map_Picked(pt))
		{
			m_bCollapse = true;

			if (Button_Down(DIM_LB))
			{
				CUIStatusFrame* pFrame = dynamic_cast<CUIStatusFrame*>(Engine::Get_GameObject(L"Layer_UI", L"UI_StatusFrame"));

				pFrame->Set_Window();
			}
		}
		else
		{
			m_bCollapse = false;
		}

		Add_RenderGroup(RENDER_UI, this);
	}
	if (!m_bWindow)
	{
		CUIStatusFrame* pFrame = dynamic_cast<CUIStatusFrame*>(Engine::Get_GameObject(L"Layer_UI", L"UI_StatusFrame"));

		pFrame->Set_WindowDis();
	}
	return iExit;
}

void CUIPlayerStats::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIPlayerStats::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	if (m_bCollapse)
	{
		m_pSelTextureCom->Set_Texture();

		m_pBufferCom->Render_Buffer();
	}

}

HRESULT CUIPlayerStats::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pSelTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIStatusSelect"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_SelTexture", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIPlayerStat"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });

	return S_OK;
}

CUIPlayerStats* CUIPlayerStats::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize)
{
	CUIPlayerStats* pUIPlayerStats = new CUIPlayerStats(pGraphicDev);

	if (FAILED(pUIPlayerStats->Ready_GameObject(vPos, vSize)))
	{
		Safe_Release(pUIPlayerStats);
		MSG_BOX("UIStatus Create Failed");
		return nullptr;
	}

	return pUIPlayerStats;
}

void CUIPlayerStats::Free()
{
	Engine::CGameObject::Free();
}
