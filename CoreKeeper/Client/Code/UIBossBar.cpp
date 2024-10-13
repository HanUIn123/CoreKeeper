#include "pch.h"
#include "..\Header\UIBossBar.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\Stage.h"

CUIBossBar::CUIBossBar(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bWindow(false), m_iHp(80), m_iMaxHp(100), m_iCurHp(0), m_iPreHp(0)

{
}

CUIBossBar::~CUIBossBar()
{
}

HRESULT CUIBossBar::Ready_GameObject(_vec2 vPos, _vec2 vSize, const _uint iIndex)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	D3DXMatrixIdentity(&ButtonWorld);

	_D3DVIEWPORT9 Viewport;

	m_pGraphicDev->GetViewport(&Viewport);
	float height = (_float)Viewport.Height;
	float width = (_float)Viewport.Width;

	float x = vPos.x - width / 2;
	float y = height / 2 - vPos.y;

	m_pTransformCom->Set_Scale(vSize.x, vSize.y , 1.f);
	m_pTransformCom->Set_Pos(x, y, 0);

	m_BRect.left = _long(vPos.x - vSize.x / 2 - 180.f);
	m_BRect.right = _long(vPos.x + vSize.x / 2 - 180.f);
	m_BRect.top = _long(vPos.y - vSize.y);
	m_BRect.bottom = _long(vPos.y + vSize.y);

	m_iIndex = iIndex;

	m_fLength = _float(m_BRect.right - m_BRect.left);

	m_pBarBufferCom->Set_Height(100);
	m_pBufferCom->Set_Height(100);

	return S_OK;
}

_int CUIBossBar::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bWindow)
		return 0;

	m_sState = m_pMonster->Get_State();

	m_iHp    = m_sState.iHp;
	m_iMaxHp = m_sState.iMaxHp;

	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CUIBossBar::LateUpdate_GameObject()
{
	m_pAnimatorCom->Update_Animation();

	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIBossBar::Render_GameObject()
{
	_matrix matWorld;

	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pTextureCom->Set_Texture(m_iIndex);

	m_pBufferCom->Render_Buffer();

	matWorld._41 -= 3.5f;

	matWorld._11 -= 7.f;
	matWorld._22 -= 3.8f;

	if (m_iIndex == 7)
	{
		matWorld._11 -= 1.5f;
		matWorld._22 += 2.f;
		matWorld._41 -= 0.2f;
		matWorld._42 += 0.5f;
	}

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pBarBufferCom->Set_Width(((_float)m_iHp / (_float)m_iMaxHp) * 100.f);

	m_pTextureCom->Set_Texture(m_iIndex - 1);

	m_pBarBufferCom->Render_Buffer();
}

void CUIBossBar::Set_Bar(_matrix matWorld, CMonster* pMonster)
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

	m_pTransformCom->Set_Pos(f_ScreenX, f_ScreenY - 40.f, 0.f );

	m_pMonster = pMonster;

	m_bWindow = true;
}

void CUIBossBar::Set_Pos(_matrix matWorld)
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

	m_pTransformCom->Set_Pos(f_ScreenX, f_ScreenY - 40.f, 0.f);
}

HRESULT CUIBossBar::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRangeTex*>(Engine::Clone_Proto(L"Proto_RangeTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pBarBufferCom = dynamic_cast<CRangeTex*>(Engine::Clone_Proto(L"Proto_RangeTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_BarBuffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIHealthTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });

	pComponent = m_pAnimatorCom = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_Animator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });

	return S_OK;
}

CUIBossBar* CUIBossBar::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize, const _uint iIndex)
{
	CUIBossBar* pUIStatusBar = new CUIBossBar(pGraphicDev);

	if (FAILED(pUIStatusBar->Ready_GameObject(vPos, vSize, iIndex)))
	{
		Safe_Release(pUIStatusBar);
		MSG_BOX("UIHealth Create Failed");
		return nullptr;
	}

	return pUIStatusBar;
}

void CUIBossBar::Free()
{
	Engine::CGameObject::Free();
}
