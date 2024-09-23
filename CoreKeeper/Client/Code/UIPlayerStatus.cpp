#include "pch.h"
#include "..\Header\UIPlayerStatus.h"
#include "Export_System.h"
#include "Export_Utility.h"

CUIPlayerStatus::CUIPlayerStatus(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bWindow(false)

{
}

CUIPlayerStatus::~CUIPlayerStatus()
{
}

HRESULT CUIPlayerStatus::Ready_GameObject(_vec2 vPos, _vec2 vSize)
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

	return S_OK;
}

_int CUIPlayerStatus::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (m_bWindow)
	{
		Add_RenderGroup(RENDER_UI, this);
	}

	return iExit;
}

void CUIPlayerStatus::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIPlayerStatus::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

}

HRESULT CUIPlayerStatus::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIStatusBoard"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });

	return S_OK;
}

CUIPlayerStatus* CUIPlayerStatus::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize)
{
	CUIPlayerStatus* pUIPlayerStatus = new CUIPlayerStatus(pGraphicDev);

	if (FAILED(pUIPlayerStatus->Ready_GameObject(vPos, vSize)))
	{
		Safe_Release(pUIPlayerStatus);
		MSG_BOX("UIStatus Create Failed");
		return nullptr;
	}

	return pUIPlayerStatus;
}

void CUIPlayerStatus::Free()
{
	Engine::CGameObject::Free();
}
