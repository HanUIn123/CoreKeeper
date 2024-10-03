#include "pch.h"
#include "../Header/Fire.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\Player.h"

CFire::CFire(LPDIRECT3DDEVICE9 pGraphicDev)
	: CObject(pGraphicDev), m_bCollision(false), m_eState(IDLE), m_fBurnTime(0.f), m_bBurn(false), m_bActive(true)
{
}

CFire::~CFire()
{
}

HRESULT CFire::Ready_GameObject(_vec3 vPos)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
	m_pAnimatorCom->Set_CurState(IDLE, 0, 12, 8);
	return S_OK;
}

_int CFire::Update_GameObject(const _float& fTimeDelta)
{
	if (m_eState == SWING)
	{
		m_fBurnTime += fTimeDelta;
	}

	if (m_fBurnTime > 5.f)
	{
		m_pAnimatorCom->Set_CurState(DEAD, 39, 46, 8);
		m_eState = DEAD;
	}

	if (m_pAnimatorCom->Get_MotionEnd())
	{
		if (m_eState == IDLE)
		{
			m_pAnimatorCom->Set_CurState(WALK, 13, 17, 8);
			m_bBurn = true;
			m_eState = WALK;
		}
		else if (m_eState == WALK)
		{
			m_pAnimatorCom->Set_CurState(SWING, 26, 30, 8);
			m_eState = SWING;
		}
		else if (m_eState == DEAD)
		{
			m_pAnimatorCom->Set_CurState(DEAD, 47, 47, 8);
			m_bActive = false;
			m_bBurn = false;
		}
	}
	

	// 플레이어와 충돌했으면 상호작용해라.
	if (m_bBurn && Check_Interaction())
	{
		Interaction();
	}

	m_pAnimatorCom->Update_Animation();

	Add_RenderGroup(RENDER_ALPHA, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CFire::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CFire::Render_GameObject()
{
	if (!m_bActive)
		return;

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture();
	m_pAnimBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());
	m_pAnimBufferCom->Render_Buffer();

	m_pColliderCom->Render_Collider();

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CFire::Interaction()
{
	// 플레이어쉒 화상입히기!!!!
}

HRESULT CFire::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pAnimBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_FireAnimTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_FireTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	pComponent = m_pColliderCom = dynamic_cast<CColliderCube*>(Engine::Clone_Proto(L"Proto_NormalCubeCollider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

	pComponent = m_pAnimatorCom = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_Animator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Animator", pComponent });

	return S_OK;
}

CFire* CFire::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
	CFire* pFire = new CFire(pGraphicDev);

	if (FAILED(pFire->Ready_GameObject(vPos)))
	{
		Safe_Release(pFire);
		MSG_BOX("pFire Create Failed");
		return nullptr;
	}

	return pFire;
}

void CFire::Free()
{
	Engine::CGameObject::Free();
}
