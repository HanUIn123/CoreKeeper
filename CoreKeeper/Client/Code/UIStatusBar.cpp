#include "pch.h"
#include "..\Header\UIStatusBar.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\Stage.h"

CUIStatusBar::CUIStatusBar(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bCollapse(false), m_bExit(false), m_iHp(100), m_iMaxHp(100), m_iCurHp(0), m_iPreHp(0)

{
}

CUIStatusBar::~CUIStatusBar()
{
}

HRESULT CUIStatusBar::Ready_GameObject(_vec2 vPos, _vec2 vSize, const _uint iIndex)
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

	m_BRect.left = _long(vPos.x - vSize.x / 2);
	m_BRect.right = _long(vPos.x + vSize.x / 2);
	m_BRect.top = _long(vPos.y - vSize.y / 2);
	m_BRect.bottom = _long(vPos.y + vSize.y / 2);

	m_iIndex = iIndex;

	m_fLength = _float(m_BRect.right - m_BRect.left);

	return S_OK;
}

_int CUIStatusBar::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	if (Map_Picked(pt))
	{
		m_bCollapse = true;
	}
	else
		m_bCollapse = false;

	Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CUIStatusBar::LateUpdate_GameObject()
{
	m_pAnimatorCom->Update_Animation();

	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIStatusBar::Render_GameObject()
{
	_matrix matWorld;

	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pTextureCom->Set_Texture(m_iIndex);

	m_pBufferCom->Render_Buffer();

	matWorld._41 -= 3.5f;

	matWorld._11 -= 7.f;
	matWorld._22 -= 3.8f;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pBarBufferCom->Set_Width(((_float)m_iHp / (_float)m_iMaxHp) * 100.f);

	m_pTextureCom->Set_Texture(m_iIndex - 1);

	m_pBarBufferCom->Render_Buffer();

	m_pTextureCom->Set_Texture(m_iIndex + 1);

	matWorld._41 += 3.5f;

	matWorld._11 = 2.f;
	matWorld._22 = 8.f;

	_int _iCurHp = m_iCurHp;
	_int _iMaxHp = m_iMaxHp;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	_int iCount;

	if (_iCurHp % 25 == 0)
	{
		iCount = _iMaxHp / 25 - 1;
	}
	else
		iCount = _iMaxHp / 25;

	_float _fCurLength = m_fLength / (iCount + 1);

	matWorld._41 -= m_fLength - 5.f;

	iCount = m_iHp / 25.f;

	for (int i = 0; i < iCount; ++i)
	{
		matWorld._41 += _fCurLength - 1.5f;
		//matWorld->_42 = 0.f;

		m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

		m_pDividerBufferCom[i]->Render_Buffer();
	}
	//Set_Pos(&matWorld);

	if (m_bCollapse && (m_iIndex - 1) == 0) // 체력
	{
		std::wstring sFront = L"체력                    " + std::to_wstring(m_iHp);

		wstring sBack = L"/" + std::to_wstring(m_iMaxHp);

		wstring sTotal = sFront + sBack;

		const _tchar* tTotal = sTotal.c_str();

		_vec2 pos(m_BRect.left - 130.f, m_BRect.top);

		Engine::Render_Font(L"Font_HP", tTotal, &pos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	}
	else if (m_bCollapse && (m_iIndex - 1) == 3) // 마나 표시용
	{
		std::wstring sFront = L"마나                    " + std::to_wstring(m_iHp);

		wstring sBack = L"/" + std::to_wstring(m_iMaxHp);

		wstring sTotal = sFront + sBack;

		const _tchar* tTotal = sTotal.c_str();

		_vec2 pos(m_BRect.left - 130.f, m_BRect.top);

		Engine::Render_Font(L"Font_HP", tTotal, &pos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	}
}

HRESULT CUIStatusBar::Add_Component()
{
	CComponent* pComponent = NULL;


	wstring string[20];
	for (_int i = 0; i < 10; i++)
	{
		pComponent = m_pDividerBufferCom[i] = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
        NULL_CHECK_RETURN(pComponent, E_FAIL);
		
		string[i] = L"Com_DividerBuffer_%d" + std::to_wstring(i);
		
		m_mapComponent[ID_STATIC].insert({ string[i].c_str(), pComponent});
	}
	pComponent = m_pBufferCom = dynamic_cast<CRangeTex*>(Engine::Clone_Proto(L"Proto_UIHealthTexRc"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pBarBufferCom = dynamic_cast<CRangeTex*>(Engine::Clone_Proto(L"Proto_UIHealthTexRc"));
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

CUIStatusBar* CUIStatusBar::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize, const _uint iIndex)
{
	CUIStatusBar* pUIStatusBar = new CUIStatusBar(pGraphicDev);

	if (FAILED(pUIStatusBar->Ready_GameObject(vPos, vSize, iIndex)))
	{
		Safe_Release(pUIStatusBar);
		MSG_BOX("UIHealth Create Failed");
		return nullptr;
	}

	return pUIStatusBar;
}

void CUIStatusBar::Free()
{
	Engine::CGameObject::Free();
}
