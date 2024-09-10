#include "pch.h"
#include "..\Header\Player.h"
#include "Export_System.h"
#include "Export_Utility.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(m_pTransformCom->m_vInfo->x, m_pTransformCom->m_vInfo->y + 1.0f, m_pTransformCom->m_vInfo->z);

	return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{

	Key_Input(fTimeDelta);

	Add_RenderGroup(RENDER_ALPHA, this);


	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CPlayer::LateUpdate_GameObject()
{
	m_pAnimatorCom->Update_Animation();
	
	Engine::CGameObject::LateUpdate_GameObject();
}

void CPlayer::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());
	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CPlayer::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_AnimTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlayerTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Calculator", pComponent });

	pComponent = m_pAnimatorCom = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_Animator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });

	return S_OK;
}

void CPlayer::Key_Input(const _float& fTimeDelta)
{
	_vec3	vLook;
	_vec3   vRight;

	m_pTransformCom->Get_Info(INFO_LOOK, &vLook);
	m_pTransformCom->Get_Info(INFO_RIGHT, &vRight);

	if (Engine::Get_DIKeyState(DIK_W) & 0x80)
	{
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, 5.f);
		m_pAnimatorCom->Set_CurState(WALK, 11, 14, 15);
	}
	else if (Engine::Get_DIKeyState(DIK_S) & 0x80)
	{
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, -5.f);
		m_pAnimatorCom->Set_CurState(WALK, 3, 6, 15);
	}
	else if (Engine::Get_DIKeyState(DIK_A) & 0x80)
	{
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, -5.f);
		m_pAnimatorCom->Set_CurState(WALK, 7, 10, 15);
	}
	else if (Engine::Get_DIKeyState(DIK_D) & 0x80)
	{
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, 5.f);
		m_pAnimatorCom->Set_CurState(WALK, 7, 10, 15);
	}
	else
	{
		m_pAnimatorCom->Set_CurState(IDLE, 0, 0, 15);
	}

	/*if (Engine::Get_DIMouseState(DIM_LB) & 0x80)
	{
		
	}*/
}

CPlayer* CPlayer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPlayer* pPlayer = new CPlayer(pGraphicDev);

	if (FAILED(pPlayer->Ready_GameObject()))
	{
		Safe_Release(pPlayer);
		MSG_BOX("pPlayer Create Failed");
		return nullptr;
	}

	return pPlayer;
}

void CPlayer::Free()
{
	Engine::CGameObject::Free();
}
