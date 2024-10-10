#include "pch.h"
#include "..\Header\PetTail.h"
#include "Export_System.h"

#include "Export_Utility.h"
#include "..\Header\Pet.h"

CPetTail::CPetTail(LPDIRECT3DDEVICE9 pGraphicDev) : CGameObject(pGraphicDev)
{
	m_pPet = nullptr;
	m_pPetTransform = nullptr;
	m_pPetAnimator = nullptr;
}

CPetTail::~CPetTail()
{
}

HRESULT CPetTail::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

_int CPetTail::Update_GameObject(const _float& fTimeDelta)
{
	Follow_Pet();
	m_pAnimatorCom->Update_Animation();
	Add_RenderGroup(RENDER_ALPHA, this);
	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CPetTail::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CPetTail::Render_GameObject()
{
	if (m_pPet->Get_StopDraw())
		return;

	_vec3		vPos, vPlayerPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CPetTail::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_PetAnimTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PetTailTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pAnimatorCom = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_Animator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });

	return S_OK;
}

void CPetTail::Follow_Pet()
{
	if (m_pPet)
	{
		_vec3 vPos, vAngle, vScale;
		m_pPetTransform->Get_Info(INFO_POS, &vPos);

		if (g_bIsTopCamera)
		{
			m_pTransformCom->Set_Angle(0, 0, 0);
			m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z - 0.001f);
		}
		else
		{
			_vec3 vLook;
			m_pPetTransform->Get_Info(INFO_LOOK, &vLook);
			m_pTransformCom->Set_Pos(vPos.x - vLook.x * 0.001f, vPos.y, vPos.z - vLook.z * 0.001f);
			vAngle = *(m_pPetTransform->Get_Angle());
			m_pTransformCom->Set_Angle(vAngle.x, vAngle.y, vAngle.z);
		}

		vScale = *(m_pPetTransform->Get_Scale());
		m_pTransformCom->Set_Scale(vScale.x, vScale.y, vScale.z);

		_int iCurIndex = m_pPetAnimator->Get_MotionIndex();
		m_pAnimatorCom->Set_CurState(STATE_END, iCurIndex, iCurIndex, 1);
	}
}

void CPetTail::Set_Pet(CGameObject* pPet)
{
	m_pPet = dynamic_cast<CPet*>(pPet);
	m_pPetTransform = dynamic_cast<CTransform*>(pPet->Get_Component(ID_DYNAMIC, L"Com_Transform"));
	m_pPetAnimator = dynamic_cast<CAnimator*>(pPet->Get_Component(ID_STATIC, L"Com_Animator"));
}

CPetTail* CPetTail::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPetTail* pEye = new CPetTail(pGraphicDev);

	if (FAILED(pEye->Ready_GameObject()))
	{
		Safe_Release(pEye);
		MSG_BOX("pEye Create Failed");
		return nullptr;
	}

	return pEye;
}

void CPetTail::Free()
{
	Engine::CGameObject::Free();
}
