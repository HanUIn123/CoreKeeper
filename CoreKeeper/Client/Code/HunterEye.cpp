#include"pch.h"
#include "..\Header\HunterEye.h"
#include "Export_System.h"

#include "Export_Utility.h"
#include "..\Header\Hunter.h"

CHunterEye::CHunterEye(LPDIRECT3DDEVICE9 pGraphicDev) : CGameObject(pGraphicDev)
{
	m_pHunter = nullptr;
	m_pHunterTransform = nullptr;
	m_pHunterAnimator = nullptr;
	m_pPlayerTransform = nullptr;
}

CHunterEye::~CHunterEye()
{
}

HRESULT CHunterEye::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

_int CHunterEye::Update_GameObject(const _float& fTimeDelta)
{
	Set_Cast();
	Follow_Hunter();
	m_pAnimatorCom->Update_Animation();
	Add_RenderGroup(RENDER_ALPHA, this);
	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CHunterEye::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CHunterEye::Render_GameObject()
{
	if (m_pHunter->Get_StopDraw())
		return;

	_vec3		vPos, vPlayerPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

	if (!m_pCalculatorCom->Check_Distance2D(&vPos, &vPlayerPos, 40.f))
		return;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CHunterEye::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_HunterEyeAnimTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_HunterEyeTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pAnimatorCom = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_Animator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });

	pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	return S_OK;
}

void CHunterEye::Follow_Hunter()
{
	if (m_pHunter)
	{
		_vec3 vPos, vAngle, vScale;
		m_pHunterTransform->Get_Info(INFO_POS, &vPos);

		if (g_bIsTopCamera)
		{
			m_pTransformCom->Set_Angle(0, 0, 0);
			m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z - 0.001f);
		}
		else
		{
			_vec3 vLook;
			m_pHunterTransform->Get_Info(INFO_LOOK, &vLook);
			m_pTransformCom->Set_Pos(vPos.x - vLook.x * 0.001f, vPos.y, vPos.z - vLook.z * 0.001f);
			vAngle = *(m_pHunterTransform->Get_Angle());
			m_pTransformCom->Set_Angle(vAngle.x, vAngle.y, vAngle.z);
		}

		vScale = *(m_pHunterTransform->Get_Scale());
		m_pTransformCom->Set_Scale(vScale.x, vScale.y, vScale.z);

		_int iCurIndex = m_pHunterAnimator->Get_MotionIndex();
		m_pAnimatorCom->Set_CurState(STATE_END, iCurIndex, iCurIndex, 1);
	}
}

void CHunterEye::Set_Cast()
{
	if (!m_pPlayerTransform)
		m_pPlayerTransform = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));
}

void CHunterEye::Set_Hunter(CGameObject* pHunter)
{
	m_pHunter = dynamic_cast<CHunter*>(pHunter);
	m_pHunterTransform = dynamic_cast<CTransform*>(pHunter->Get_Component(ID_DYNAMIC, L"Com_Transform"));
	m_pHunterAnimator = dynamic_cast<CAnimator*>(pHunter->Get_Component(ID_STATIC, L"Com_Animator"));
}

CHunterEye* CHunterEye::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CHunterEye* pEye = new CHunterEye(pGraphicDev);

	if (FAILED(pEye->Ready_GameObject()))
	{
		Safe_Release(pEye);
		MSG_BOX("pEye Create Failed");
		return nullptr;
	}

	return pEye;
}

void CHunterEye::Free()
{
	Engine::CGameObject::Free();
}
