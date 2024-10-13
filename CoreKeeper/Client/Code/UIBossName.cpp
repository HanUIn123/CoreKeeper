#include "pch.h"
#include "..\Header\UIBossName.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "../Header/Monster.h"

CUIBossName::CUIBossName(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bWindow(false)

{
}

CUIBossName::~CUIBossName()
{
}

HRESULT CUIBossName::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

_int CUIBossName::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bWindow)
		return 0;

	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);
	
	Engine::Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CUIBossName::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIBossName::Render_GameObject()
{
	Engine::Render_Font(L"Font_Default", m_tFont, &m_vPos, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
}

void CUIBossName::Set_Font(_matrix matWorld, const _tchar* _tFont)
{
	_matrix matView, matProj;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	_matrix m_WVP = matWorld * matView * matProj;

	_vec4 m_vWorldToScreen(0, 0, 0, 1);
	D3DXVec4Transform(&m_vWorldToScreen, &m_vWorldToScreen, &m_WVP);

	float f_WorldToScreenX = m_vWorldToScreen.x / m_vWorldToScreen.w;
	float f_WorldToScreenY = m_vWorldToScreen.y / m_vWorldToScreen.w;
	float f_WorldToScreenZ = m_vWorldToScreen.z / m_vWorldToScreen.w;

	float f_ScreenNormalX = (f_WorldToScreenX + 1) * 0.5f;
	float f_ScreenNormalY = (f_WorldToScreenY + 1) * 0.5f;
	f_ScreenNormalY = 1.f - f_ScreenNormalY;

	float f_ScreenX = f_ScreenNormalX * WINCX;
	float f_ScreenY = f_ScreenNormalY * WINCY;

	m_vPos = { f_ScreenX - 60.f , f_ScreenY - 120.f };

	m_tFont = _tFont;

	m_bWindow = true;
}

HRESULT CUIBossName::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });
	
	return S_OK;
}

CUIBossName* CUIBossName::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CUIBossName* pUIFont = new CUIBossName(pGraphicDev);

	if (FAILED(pUIFont->Ready_GameObject()))
	{
		Safe_Release(pUIFont);
		MSG_BOX("UIStatus Create Failed");
		return nullptr;
	}

	return pUIFont;
}

void CUIBossName::Free()
{
	Engine::CGameObject::Free();
}
