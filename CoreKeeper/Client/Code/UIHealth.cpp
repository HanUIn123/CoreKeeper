#include "pch.h"
#include "..\Header\UIHealth.h"
#include "Export_System.h"
#include "Export_Utility.h"

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

	return S_OK;
}

_int CUIHealth::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (m_iIndex == 1) // 체력 바 위에 마우스 커서가 있을시 (체력 바 껍데기 X)
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(g_hWnd, &pt);


		if (Map_Picked(pt))
		{
			m_bCollapse = true;
		}
		else
			m_bCollapse = false;
	}

	if (m_iIndex == 1) // 렌더 순서 정하기용 (이거 안하면 현재 체력 출력 된 후에 체력 바가 출력 되서 현재 체력이 안 보입니다)
	{
		Add_RenderGroup(RENDER_UI, this);
	}

	return iExit;
}

void CUIHealth::LateUpdate_GameObject()
{
	m_pAnimatorCom->Update_Animation();

	if (m_iIndex == 0)
	{
		Add_RenderGroup(RENDER_UI, this);
	}

	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIHealth::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pBufferCom->Set_Width((m_iHp / m_iMaxHp) * 100.f);
	m_pTextureCom->Set_Texture(m_iIndex);

	if (m_bCollapse)
	{

		/*
		string sFront = "체력              ";
		sFront += to_string(m_iHp);

		string sBack = "/";
		sBack += to_string(m_iMaxHp);
		
		string sTotal = sFront + sBack;
		const char* cTotal = sTotal.c_str();



		_tchar tText[128] = ;
	

		_vec2 pos(125, 400);

		Engine::Render_Font(L"Font_Default", , &pos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
		*/
	}

	m_pBufferCom->Render_Buffer();
}

HRESULT CUIHealth::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRangeTex*>(Engine::Clone_Proto(L"Proto_UIHealthTexRc"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

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
