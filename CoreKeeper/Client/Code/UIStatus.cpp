#include "pch.h"
#include "..\Header\UIStatus.h"
#include "Export_System.h"
#include "Export_Utility.h"

CUIStatus::CUIStatus(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)

{
}

CUIStatus::~CUIStatus()
{
}

HRESULT CUIStatus::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->m_vScale = { 50.f, 50.f, 1.f }; // 스케일값 조정

	m_pTransformCom->Set_Pos(300.f, -200.f, 0.f); // 위치값 조정 (크로스헤어 기준 (0, 0))

	return S_OK;
}

_int CUIStatus::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	//m_pAnimatorCom->Set_CurState(IDLE, 0, 0, 11);

	Add_RenderGroup(RENDER_UI, this);

	return iExit;
}

void CUIStatus::LateUpdate_GameObject()
{
	//m_pAnimatorCom->Update_Animation();

	Engine::CGameObject::LateUpdate_GameObject();
}

void CUIStatus::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//m_pBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());
	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CUIStatus::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_UITex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_UITransform", pComponent });

	/*
	pComponent = m_pAnimatorCom = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_Animator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });
	*/
	return S_OK;
}

CUIStatus* CUIStatus::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CUIStatus* pUIStatus = new CUIStatus(pGraphicDev);

	if (FAILED(pUIStatus->Ready_GameObject()))
	{
		Safe_Release(pUIStatus);
		MSG_BOX("UIStatus Create Failed");
		return nullptr;
	}

	return pUIStatus;
}

void CUIStatus::Free()
{
	Engine::CGameObject::Free();
}
