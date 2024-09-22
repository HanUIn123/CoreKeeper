#include "pch.h"
#include "..\Header\Monster.h"
#include "Export_Utility.h"
#include "..\Header\Player.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
	m_eType = MON_END;
	
	m_fIdleY = 0.f;
	m_bIdling = false;

	m_fJumpY = 0.f;
	m_bJumping = false;
	m_fJumpHeight = 0.f;
	m_fJumpSpeed = 0.f;
	m_fJumpFrame = 0.f;

	m_vStartPoint = { 0, 0, 0 };
	m_vAttackPoint = { 0, 0, 0 };
	m_bAttackSuccess = false;
	
	m_bFallStart = false;
	m_vFallDir = { 0, 0, 0 };

	m_iDir = 0;
	m_fSpeed = 0.f;
	m_fDiagSpeed = 0.f; 
	m_fSpeedWeight = 0.f;

	m_bKnockBackStart = false;
	m_bKnockBackEnd = true;

	m_bStopDraw = false;
	m_bDropSettings = false;
}

CMonster::~CMonster()
{
}

HRESULT CMonster::Ready_GameObject()
{
	return S_OK;
}

_int CMonster::Update_GameObject(const _float& fTimeDelta)
{
	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CMonster::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CMonster::Render_GameObject()
{

}

void CMonster::Apply_Billboard()
{
	D3DXMATRIX matWorld, matView, matBill, matScale;

	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixIdentity(&matBill);

	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, 0, &matBill);

	// 스케일 행렬을 따로 계산
	D3DXMatrixScaling(&matScale, m_pTransformCom->m_vScale.x, m_pTransformCom->m_vScale.y, m_pTransformCom->m_vScale.z);

	D3DXMATRIX matInverseScale;
	D3DXMatrixInverse(&matInverseScale, 0, &matScale);

	// 최종 월드 행렬: 스케일 적용 후 빌보드 회전 적용
	D3DXMATRIX matFinal = matScale * matBill * matInverseScale * matWorld;

	m_pTransformCom->Set_WorldMatrix(&matFinal);
}

void CMonster::Pattern_Idle(const _float& fTimeDelta)
{
	// 벽 확인 추가할 것
	//_float	fSpeedWeight = 1.f;
	switch (m_eType)
	{
	case MON_SLIME:
		m_pAnimatorCom->Set_CurState(IDLE, 0, 8, 12);
		break;
	default:
		break;
	}
	
	// 일정 시간마다 타일 한칸 이동 or 정지
	if (m_pAnimatorCom->Get_MotionEnd())
		m_bIdling = false;
	
	if (!m_bIdling)
	{
		m_bIdling = true;
		//m_iDir = rand() % 9;
		if (m_iDir)
			m_iDir = 0;
		else
			m_iDir = rand() % 8 + 1;
	}
	else
	{
		_vec3	vLook, vRight;
		m_pTransformCom->Get_Info(INFO_LOOK, &vLook);
		m_pTransformCom->Get_Info(INFO_RIGHT, &vRight);
		
		switch (m_iDir)
		{
		case 0:
			// 정지
			break;
		case 1:
			// 상
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, m_fSpeed);
			break;
		case 2:
			// 우상
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, m_fDiagSpeed);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, m_fDiagSpeed);
			break;
		case 3:
			// 우
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, m_fSpeed);
			break;
		case 4:
			// 우하
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, -m_fDiagSpeed);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, m_fDiagSpeed);
			break;
		case 5:
			// 하
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, -m_fSpeed);
			break;
		case 6:
			// 좌하
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, -m_fDiagSpeed);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, -m_fDiagSpeed);
			break;
		case 7:
			// 좌
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, -m_fSpeed);
			break;
		case 8:
			// 좌상
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, m_fDiagSpeed);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, -m_fDiagSpeed);
			break;
		}
	}
}

void CMonster::Pattern_Chase(const _float& fTimeDelta)
{
	// 플레이어 방향으로 이동, 추후 A스타 알고리즘으로 변경
	switch (m_eType)
	{
	case MON_SLIME:
		m_pAnimatorCom->Set_CurState(WALK, 12, 21, 8);
		break;
	default:
		break;
	}
	
	Engine::CTransform* pPlayerTransform = dynamic_cast<Engine::CTransform*>
		(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));
	NULL_CHECK(pPlayerTransform);

	_vec3		vPlayerPos, vPos;
	pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	m_pTransformCom->Chase_Target(&vPlayerPos, fTimeDelta);

	
	if (m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, 5.f))
		m_eState = SWING;
	else
		m_eState = WALK;

}

void CMonster::Pattern_Attack(const _float& fTimeDelta)
{
	_float		fSpeedWeight;
	_vec3		vPos, vPlayerPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	Engine::CTransform* pPlayerTransform = dynamic_cast<Engine::CTransform*>
		(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));
	NULL_CHECK(pPlayerTransform);
	switch (m_eType)
	{
	case MON_SLIME:
		m_pAnimatorCom->Set_CurState(SWING, 24, 35, 8);
		pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
		if (m_pAnimatorCom->Get_MotionIndex() <= 29 || m_pAnimatorCom->Get_MotionIndex() >= 34)
		{
			if (m_pAnimatorCom->Get_MotionIndex() == 25 || m_pAnimatorCom->Get_MotionIndex() == 34)
				m_pTransformCom->Set_Pos(vPos.x, m_fIdleY - 0.1f, vPos.z);
			else if (m_pAnimatorCom->Get_MotionIndex() == 35)
			{
				m_pTransformCom->Set_Pos(vPos.x, m_fIdleY, vPos.z);
				m_bJumping = false;
				if (m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, 5.f))
					m_eState = SWING;
				else
					m_eState = WALK;
			}
			fSpeedWeight = 0.f;
		}
		else
		{
			fSpeedWeight = 6.f;
			Jump(fTimeDelta);
		}
		m_pTransformCom->Chase_Target(&vPlayerPos, fTimeDelta * fSpeedWeight);
		break;
	default:
		break;
	}
}

void CMonster::Pattern_Dead()
{
	if (!m_pStateCom->Get_Dead())
		m_pStateCom->Set_Dead();

	switch (m_eType)
	{
	case MON_SLIME:
		m_pAnimatorCom->Set_CurState(DEAD, 36, 41, 8);
		break;
	default:
		break;
	}
}

void CMonster::Jump(const _float& fTimeDelta)
{
	_vec3 vPos, vUp;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	m_pTransformCom->Get_Info(INFO_UP, &vUp);
	if (!m_bJumping)
	{
		m_bJumping = true;
		m_fJumpTime = 0;
		m_fJumpHeight = m_fJumpY - m_fIdleY;
		switch (m_eType)
		{
		case MON_SLIME:
			m_fJumpFrame = 4 * 8 * fTimeDelta;
			break;
		default:
			break;
		}
	}

	m_fJumpTime += fTimeDelta;
	_float fProgress = (m_fJumpTime / m_fJumpFrame);
	_float fWeight = 0.7f;
	if (fProgress > 0.25f && fProgress <= 0.75f)
		fWeight = 0.3f;

	m_fJumpSpeed = m_fJumpHeight * fWeight;
	if (fProgress > 0.5f)
		m_fJumpSpeed *= -1;

	if (fProgress < 1.f)
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vUp, &vUp), fTimeDelta, m_fJumpSpeed);
	else
		m_pTransformCom->Set_Pos(vPos.x, m_fIdleY, vPos.z);
}

void CMonster::KnockBack(_float fDist)
{
	// 모든 행동보다 우선 시 할 것
	m_bJumping = false;
	m_bIdling = false;
	// m_fIdleY보다 클 경우 Y값도 내려주기
	

}

CMonster* CMonster::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMonster* pMonster = new CMonster(pGraphicDev);

	if (FAILED(pMonster->Ready_GameObject()))
	{
		Safe_Release(pMonster);
		MSG_BOX("pMonster Create Failed");
		return nullptr;
	}

	return pMonster;
}

void CMonster::Free()
{
	Engine::CGameObject::Free();
}
