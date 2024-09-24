#include "pch.h"
#include "..\Header\Hair.h"

#include "Export_Utility.h"

CHair::CHair(LPDIRECT3DDEVICE9 pGraphicDev) : CItem(pGraphicDev)
{
}

CHair::~CHair()
{
}

HRESULT CHair::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

_int CHair::Update_GameObject(const _float& fTimeDelta)
{
	Follow_Player();
	m_pAnimatorCom->Update_Animation();
	if (m_bActive)
		Add_RenderGroup(RENDER_ALPHA, this);
	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CHair::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CHair::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CHair::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_AnimTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTex_Hair"));
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

void CHair::Follow_Player()
{
	CTransform* playerTransform = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));
	if (playerTransform)
	{
		_vec3 vPlayerPos, vPlayerAngle, vPlayerScale;
		playerTransform->Get_Info(INFO_POS, &vPlayerPos);

		if (g_bIsTopCamera)
		{
			m_pTransformCom->Set_Angle(0, 0, 0);
			m_pTransformCom->Set_Pos(vPlayerPos.x, vPlayerPos.y - 0.04f, vPlayerPos.z - 0.001f);
		}
		else
		{
			_vec3 vPlayerLook;
			playerTransform->Get_Info(INFO_LOOK, &vPlayerLook);
			m_pTransformCom->Set_Pos(vPlayerPos.x - vPlayerLook.x * 0.002f, vPlayerPos.y - 0.04f, vPlayerPos.z - vPlayerLook.z * 0.002f);
			vPlayerAngle = *(playerTransform->Get_Angle());
			m_pTransformCom->Set_Angle(vPlayerAngle.x, vPlayerAngle.y, vPlayerAngle.z);
		}

		vPlayerScale = *(playerTransform->Get_Scale());
		m_pTransformCom->Set_Scale(vPlayerScale.x, vPlayerScale.y, vPlayerScale.z);

		CAnimator* playerAnimator = dynamic_cast<CAnimator*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Animator"));
		int iCurIndex = playerAnimator->Get_MotionIndex();
		m_pAnimatorCom->Set_CurState(STATE_END, iCurIndex, iCurIndex, 1);
	}
}

CHair* CHair::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CHair* pHair = new CHair(pGraphicDev);

	if (FAILED(pHair->Ready_GameObject()))
	{
		Safe_Release(pHair);
		MSG_BOX("pHair Create Failed");
		return nullptr;
	}

	return pHair;
}

void CHair::Free()
{
	Engine::CGameObject::Free();
}
