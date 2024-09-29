#include "pch.h"
#include "..\Header\UIStatue.h"
#include "Export_System.h"
#include "Export_Utility.h"

CUIStatue::CUIStatue(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bWindow(false), m_eType(STATUE_GHORM)

{
}

CUIStatue::~CUIStatue()
{
}

HRESULT CUIStatue::Ready_GameObject(_vec2 vPos, _vec2 vSize)
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

	m_bRect.left = vPos.x - vSize.x;
	m_bRect.right = vPos.x + vSize.x;
	m_bRect.top = vPos.y - vSize.y;
	m_bRect.bottom = vPos.y + vSize.y;

	return S_OK;
}

_int CUIStatue::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (m_bWindow)
	{
		Add_RenderGroup(RENDER_UI, this);
	}

	return iExit;
}

void CUIStatue::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIStatue::Render_GameObject()
{	
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture(m_eType);

	m_pBufferCom->Render_Buffer();
}

void CUIStatue::Set_Window(STATUE_TYPE _eType)
{
	m_eType = _eType;

	if (m_bWindow)
		m_bWindow = false;
	else
		m_bWindow = true;
}

HRESULT CUIStatue::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIStatue"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pColTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIInvSelected"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_ColTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });
	
	return S_OK;
}

CUIStatue* CUIStatue::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize)
{
	CUIStatue* pUIStatue = new CUIStatue(pGraphicDev);

	if (FAILED(pUIStatue->Ready_GameObject(vPos, vSize)))
	{
		Safe_Release(pUIStatue);
		MSG_BOX("UIStatus Create Failed");
		return nullptr;
	}

	return pUIStatue;
}

void CUIStatue::Free()
{
	Engine::CGameObject::Free();
}
