#include "pch.h"
#include "..\Header\UIFont.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "../Header/Monster.h"

CUIFont::CUIFont(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)

{
}

CUIFont::~CUIFont()
{
}

HRESULT CUIFont::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

_int CUIFont::Update_GameObject(const _float& fTimeDelta)
{
	if (m_vecFontPos.size() <= 0)
		return 0;

	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);
	
	for (vector<FONT>::iterator iter = m_vecFontPos.begin(); iter != m_vecFontPos.end(); iter++)
	{
		iter->fCount -= fTimeDelta;

		if (iter->fCount <= 0.f)
			iter->bDead = true;
	}

	vector<FONT>::iterator i;

	i = m_vecFontPos.begin();

	while (i != m_vecFontPos.end())
	{
		if (i->bDead == true)// 죽은것 삭제
		{
			i = m_vecFontPos.erase(i);
		}
		else
		{
			i++;
		}
	}

	Engine::Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CUIFont::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIFont::Render_GameObject()
{
	for (const auto iter : m_vecFontPos)
	{
		Engine::Render_Font(L"Font_Default", iter.tFont.c_str(), &iter.vPos, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}
}

void CUIFont::Set_Font(_matrix matWorld, const _tchar* tFont)
{
	/*
	_float m_fRotAngle;

	_matrix m_World;
	D3DXMatrixIdentity(&m_World);

	_matrix m_Rot;
	D3DXMatrixRotationZ(&m_Rot, 0.f);

	_matrix m_Translation;
	D3DXMatrixTranslation(&m_Translation, vPos.x, vPos.y, vPos.z);

	m_World = m_Rot * m_Translation;
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_World);

	_matrix m_wvp = m_World * 
	*/
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

	_vec2 vPos2 = { f_ScreenX, f_ScreenY + 1.f };

	FONT sFont;
	sFont.vPos = vPos2;
	sFont.tFont = tFont;
	sFont.fCount = 1.f;
	sFont.bDead = false;

	m_vecFontPos.push_back(sFont);
}

void CUIFont::Set_Font_Center(_matrix matWorld, const _tchar* tFont)
{
	_matrix matView, matProj;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	_matrix m_WVP = matWorld * matView * matProj;

	_vec4 m_vWorldToScreen(0, 0, 0, 1);
	D3DXVec4Transform(&m_vWorldToScreen, &m_vWorldToScreen, &m_WVP);
	
	size_t length = wcslen(tFont);

	float f_WorldToScreenX = m_vWorldToScreen.x / m_vWorldToScreen.w;
	float f_WorldToScreenY = m_vWorldToScreen.y / m_vWorldToScreen.w;
	float f_WorldToScreenZ = m_vWorldToScreen.z / m_vWorldToScreen.w;

	float f_ScreenNormalX = (f_WorldToScreenX + 1) * 0.5f;
	float f_ScreenNormalY = (f_WorldToScreenY + 1) * 0.5f;
	f_ScreenNormalY = 1.f - f_ScreenNormalY;

	float f_ScreenX = f_ScreenNormalX * WINCX;
	float f_ScreenY = f_ScreenNormalY * WINCY;

	// 글자 길이의 반 * 폰트 너비만큼 빼주면 가운데에 옴
	_vec2 vPos2 = { f_ScreenX - length * 0.5f * 20.f, f_ScreenY};

	FONT sFont;
	sFont.vPos = vPos2;
	sFont.tFont = tFont;
	sFont.fCount = 1.f;
	sFont.bDead = false;

	m_vecFontPos.push_back(sFont);
}

void CUIFont::Set_Font_Up(_matrix matWorld, const _tchar* tFont)
{
	_matrix matView, matProj;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	_matrix m_WVP = matWorld * matView * matProj;

	_vec4 m_vWorldToScreen(0, 0, 0, 1);
	D3DXVec4Transform(&m_vWorldToScreen, &m_vWorldToScreen, &m_WVP);

	size_t length = wcslen(tFont);

	float f_WorldToScreenX = m_vWorldToScreen.x / m_vWorldToScreen.w;
	float f_WorldToScreenY = m_vWorldToScreen.y / m_vWorldToScreen.w;
	float f_WorldToScreenZ = m_vWorldToScreen.z / m_vWorldToScreen.w;

	float f_ScreenNormalX = (f_WorldToScreenX + 1) * 0.5f;
	float f_ScreenNormalY = (f_WorldToScreenY + 1) * 0.5f;
	f_ScreenNormalY = 1.f - f_ScreenNormalY;

	float f_ScreenX = f_ScreenNormalX * WINCX;
	float f_ScreenY = f_ScreenNormalY * WINCY;

	// 글자 길이의 반 * 폰트 너비만큼 빼주면 가운데에 옴
	_vec2 vPos2 = { f_ScreenX - length * 0.5f * 20.f, f_ScreenY - 60.f };

	FONT sFont;
	sFont.vPos = vPos2;
	sFont.tFont = tFont;
	sFont.fCount = 1.f;
	sFont.bDead = false;

	m_vecFontPos.push_back(sFont);
}

HRESULT CUIFont::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });
	
	return S_OK;
}

CUIFont* CUIFont::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CUIFont* pUIFont = new CUIFont(pGraphicDev);

	if (FAILED(pUIFont->Ready_GameObject()))
	{
		Safe_Release(pUIFont);
		MSG_BOX("UIStatus Create Failed");
		return nullptr;
	}

	return pUIFont;
}

void CUIFont::Free()
{
	Engine::CGameObject::Free();
}
