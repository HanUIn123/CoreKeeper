#include "pch.h"
#include "../Header/SprinklerObject.h"
#include "Export_System.h"
#include "Export_Utility.h"

CSprinklerObject::CSprinklerObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: CObject(pGraphicDev)
{
}

CSprinklerObject::~CSprinklerObject()
{
}

HRESULT CSprinklerObject::Ready_GameObject(_vec3 vPos)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(vPos.x, 0.001f, vPos.z);
	m_pTransformCom->Set_Scale(0.5f, 0.5f, 0.5f);

	m_pTransformCom->Set_Angle(D3DXToRadian(90.f), 0.f, 0.f);

	m_pAnimatorCom->Set_CurState(IDLE, 0, 7, 3);

	return S_OK;
}

_int CSprinklerObject::Update_GameObject(const _float& fTimeDelta)
{
	m_pAnimatorCom->Update_Animation();

	Add_RenderGroup(RENDER_ALPHA, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CSprinklerObject::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CSprinklerObject::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	FAILED_CHECK_RETURN(Setup_Material(), );

	m_pTextureCom->Set_Texture();

	m_pAnimBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());
	m_pAnimBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CSprinklerObject::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_SprinklerTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pAnimBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_SprinklerAnimTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pAnimatorCom = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_Animator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Animator", pComponent });

	return S_OK;
}

CSprinklerObject* CSprinklerObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
	CSprinklerObject* pCore = new CSprinklerObject(pGraphicDev);

	if (FAILED(pCore->Ready_GameObject(vPos)))
	{
		Safe_Release(pCore);
		MSG_BOX("pCore Create Failed");
		return nullptr;
	}

	return pCore;
}

void CSprinklerObject::Free()
{
	Engine::CGameObject::Free();
}
