#include "pch.h"
#include "..\Header\UIHealth.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\HpDivider.h"
#include "..\Header\Stage.h"

CUIHealth::CUIHealth(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bCollapse(false), m_bExit(false), m_iHp(100), m_iMaxHp(100)

{
}

CUIHealth::~CUIHealth()
{
}

HRESULT CUIHealth::Ready_GameObject(_vec2 vPos, _vec2 vSize, const _uint iIndex)
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

_int CUIHealth::Update_GameObject(const _float& fTimeDelta)
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

void CUIHealth::LateUpdate_GameObject()
{
	m_pAnimatorCom->Update_Animation();

	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIHealth::Render_GameObject()
{
	_matrix matWorld;

	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pBufferCom->Set_Width((m_iHp / m_iMaxHp) * 100.f);

	m_pTextureCom->Set_Texture(m_iIndex);

	m_pBufferCom->Render_Buffer();

	matWorld._41 -= 3.5f;

	matWorld._11 -= 7.f;
	matWorld._22 -= 2.25f;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pTextureCom->Set_Texture(m_iIndex - 1);

	m_pBarBufferCom->Render_Buffer();

	if (m_bCollapse && (m_iIndex - 1)== 0)
	{
		std::wstring sFront = L"체력                      " + std::to_wstring(m_iHp);

		wstring sBack = L"/" + std::to_wstring(m_iMaxHp);

		wstring sTotal = sFront + sBack;

		const _tchar* tTotal = sTotal.c_str();

		_vec2 pos(m_BRect.left - 140.f , m_BRect.bottom);

		Engine::Render_Font(L"Font_HP", tTotal, &pos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	}
	else if (m_bCollapse && (m_iIndex - 1) == 3)
	{
		std::wstring sFront = L"마나                      " + std::to_wstring(m_iHp);

		wstring sBack = L"/" + std::to_wstring(m_iMaxHp);

		wstring sTotal = sFront + sBack;

		const _tchar* tTotal = sTotal.c_str();

		_vec2 pos(m_BRect.left - 140.f, m_BRect.bottom);

		Engine::Render_Font(L"Font_HP", tTotal, &pos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	}
}

void CUIHealth::Set_Hp(_int _iMaxHp, _int _iCurHp)
{
	m_iMaxHp = _iMaxHp, m_iHp = _iCurHp;

	_int iCount;
	if (_iCurHp % 25 == 0)
	{
		iCount = _iMaxHp / 25;
	}
	else
		iCount = _iMaxHp / 25 + 1;

	for (int i = 0; i < iCount; i++ )
	{
	//	CStage* pScene = dynamic_cast<CStage*>(Engine::Get_Scene());
	//	NULL_CHECK_RETURN(pScene);

	//	pScene->Create_GameObject(L"Layer_UI");
	}
	_float _fCurLength = m_fLength / iCount;

	for (int i = 1; i < iCount + 1; i++)
	{
		std::wstring string;
		string = L"UI_Health_Divider_" + std::to_wstring(i);

		CHpDivider* pUI = dynamic_cast<CHpDivider*>(Engine::Get_GameObject(L"Layer_UI", string.c_str()));

		if (pUI)
		{
			pUI->Calculate_Pos(_fCurLength * i, _iCurHp, _iMaxHp);
		}
	}
}

HRESULT CUIHealth::Add_Component()
{
	CComponent* pComponent = NULL;

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

CUIHealth* CUIHealth::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize, const _uint iIndex)
{
	CUIHealth* pUIHealth = new CUIHealth(pGraphicDev);

	if (FAILED(pUIHealth->Ready_GameObject(vPos, vSize, iIndex)))
	{
		Safe_Release(pUIHealth);
		MSG_BOX("UIHealth Create Failed");
		return nullptr;
	}

	return pUIHealth;
}

void CUIHealth::Free()
{
	Engine::CGameObject::Free();
}
