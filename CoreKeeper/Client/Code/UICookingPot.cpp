#include "pch.h"
#include "..\Header\UIInvPlate.h"
#include "Export_System.h"
#include "Export_Utility.h"

CUIInvPlate::CUIInvPlate(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_bRender(false)

{
}

CUIInvPlate::~CUIInvPlate()
{
}

HRESULT CUIInvPlate::Ready_GameObject(_vec2 vPos, _vec2 vSize)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	D3DVIEWPORT9 Viewport;

	m_pGraphicDev->GetViewport(&Viewport);
	float height = (_float)Viewport.Height;
	float width = (_float)Viewport.Width;

	float x = vPos.x - width / 2;
	float y = height / 2 - vPos.y;

	m_pTransformCom->Set_Pos(x, y, 0);

	m_pTransformCom->Set_Scale(vSize.x, vSize.y, 1.f);

	return S_OK;
}

_int CUIInvPlate::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (m_bRender)
	{
		Add_RenderGroup(RENDER_UI, this);
	}
	return iExit;
}

void CUIInvPlate::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIInvPlate::Render_GameObject()
{

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

}

HRESULT CUIInvPlate::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_PlateBuffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UIBoard"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_PlateTexture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UIPlateTransform", pComponent });

	/*
	pComponent = m_pAnimatorCom = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_Animator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });
	*/
	return S_OK;
}

CUIInvPlate* CUIInvPlate::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize)
{
	CUIInvPlate* pUIPlate = new CUIInvPlate(pGraphicDev);

	if (FAILED(pUIPlate->Ready_GameObject(vPos, vSize)))
	{
		Safe_Release(pUIPlate);
		MSG_BOX("UIStatus Create Failed");
		return nullptr;
	}

	return pUIPlate;
}

void CUIInvPlate::Free()
{
	Engine::CGameObject::Free();
}
