#include "pch.h"
#include "../Header/Thunder.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "../Header/Azeos.h"

CThunder::CThunder(LPDIRECT3DDEVICE9 pGraphicDev)
	: CObject(pGraphicDev), m_bCollision(false), m_eState(IDLE), m_fBurnTime(0.f), m_bBurn(false), m_bActive(true), m_bLR(false), m_ePreState(STATE_END)
{
}

CThunder::~CThunder()
{
}

HRESULT CThunder::Ready_GameObject(_vec3 vPos, THUNDER_TYPE eType)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
	m_pAnimatorCom->Set_CurState(IDLE, 0, 3, 3);

	m_pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player"));

	m_eType = eType;

	m_pTransformCom->Get_Info(INFO_POS, &m_vPos);

	if (m_eType == THUNDER_LINE)
	{
		_vec3 vPlayerPos;

		CTransform* pPlayerTrans = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));
		pPlayerTrans->Get_Info(INFO_POS, &vPlayerPos);

		if ((vPlayerPos.x - m_vPos.x) > 0)
			m_bLR = true;
		else
			m_bLR = false;
	}

	return S_OK;
}

_int CThunder::Update_GameObject(const _float& fTimeDelta)
{
	if (m_eState == WALK)
	{
		m_fBurnTime += fTimeDelta;
	}

	if (m_fBurnTime > 7.f)
	{
		m_pAnimatorCom->Set_CurState(DEAD, 3, 4, 3);
		m_eState = DEAD;
	}

	if (m_eState == IDLE && m_pAnimatorCom->Get_MotionEnd())
	{

		m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
		m_pAnimatorCom->Set_CurState(WALK, 0, 2, 3);
		m_bBurn = true;
		Set_Move();
		m_eState = WALK;
	}

	if (m_eState != m_ePreState)
	{
		if (m_eState == WALK)
		{
			m_pAnimatorCom->Set_CurState(WALK, 0, 2, 3);

			m_eState = WALK;
		}
		else if (m_eState == DEAD)
		{
			m_pAnimatorCom->Set_CurState(DEAD, 3, 4, 3);

			m_bActive = false;
			m_bBurn = false;
			
		}

		m_ePreState = m_eState;
	}

	if (m_eState == WALK)
	{
		m_pTransformCom->Move_Pos(&m_vDir, fTimeDelta, 5.f);
	}

	// 플레이어와 충돌했으면 상호작용해라.
	if (m_bBurn && Check_Interaction())
	{
		Interaction();
	}

	m_pAnimatorCom->Update_Animation();

	Engine::Add_RenderGroup(RENDER_ALPHA, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CThunder::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CThunder::Render_GameObject()
{
	if (!m_bActive)
		return;

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	if (m_eState == IDLE)
		m_pElectricTextureCom->Set_Texture();
	else if (m_eState == WALK)
		m_pTextureCom->Set_Texture();

	if (m_eState == IDLE)
	{
		m_pElectAnimBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());
		m_pElectAnimBufferCom->Render_Buffer();
	}
	else if (m_eState == WALK)
	{
		m_pAnimBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());
		m_pAnimBufferCom->Render_Buffer();
	}

	m_pColliderCom->Render_Collider();

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CThunder::Interaction()
{
	m_pPlayer->Set_KnockBack(m_vPos, 15, 0.1f);
	// 넉백 없어야함
}

void CThunder::Set_Burn()
{
	m_pAnimatorCom->Set_CurState(WALK, 0, 2, 3);
	m_bBurn = true;
	m_eState = WALK;
}

void CThunder::Set_Move()
{
	switch (m_eType)
	{
	case THUNDER_LINE:
	{
		if (m_bLR)
		{
			m_vDir = { 1.f, 0.f, 0.f };
		}
		else
			m_vDir = { -1.f, 0.f, 0.f };

		break;

	}
	case THUNDER_RANDOM:
	{
		_int iRand = rand() % 4;

		switch (iRand)
		{
		case 0:
			m_vDir = { 0.f, 0.f, 1.f };
			break;

		case 1:
			m_vDir = { 0.f, 0.f, -1.f };
			break;

		case 2:
			m_vDir = { 1.f, 0.f, 0.f };
			break;

		case 3:
			m_vDir = { -1.f, 0.f, 0.f };
			break;
		}
		break;

	}
	case THUNDER_CIRCLE:
	{
		CTransform* pPlayer = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));

		_vec3 vPos;
		_vec3 vPlayerPos;

		pPlayer->Get_Info(INFO_POS, &vPlayerPos);
		m_pTransformCom->Get_Info(INFO_POS, &vPos);

		vPlayerPos -= vPos;

		D3DXVec3Normalize(&m_vDir, &vPlayerPos);
		break;
	}
	}
}

HRESULT CThunder::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pAnimBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_ThunderAnimTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pElectAnimBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_ElectricAnimTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_ElectBuffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ThunderTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pElectricTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ElectricTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_ElecTexture", pComponent });

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

CThunder* CThunder::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, THUNDER_TYPE eType)
{
	CThunder* pFire = new CThunder(pGraphicDev);

	if (FAILED(pFire->Ready_GameObject(vPos, eType)))
	{
		Safe_Release(pFire);
		MSG_BOX("pFire Create Failed");
		return nullptr;
	}

	return pFire;
}

void CThunder::Free()
{
	Engine::CGameObject::Free();
}
