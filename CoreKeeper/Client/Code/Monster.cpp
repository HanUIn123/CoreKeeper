#include "pch.h"
#include "..\Header\Monster.h"
#include "Export_Utility.h"
#include "..\Header\Player.h"
#include "DropItem.h"

int	CMonster::m_iTagNumber = 0;

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

	m_fAttackDistance = 0.f;
	m_fAttackTime = 0.f;
	m_vAttackPoint = { 0, 0, 0 };
	m_bAttackSuccess = false;
	m_bAttackFailed = false;
	
	m_bFallStart = false;
	m_vFallDir = { 0, 0, 0 };

	m_iDir = 0;
	m_fSpeed = 0.f;
	m_fDiagSpeed = 0.f; 
	m_fSpeedWeight = 0.f;

	m_bKnockBackStart = false;
	m_bKnockBackEnd = true;

	m_bStopDraw = false;

	m_vecDropItem.reserve(3);
}

CMonster::~CMonster()
{
}

HRESULT CMonster::Ready_GameObject(_vec3 vPos)
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
	D3DXMatrixScaling(&matScale, m_pTransformCom->Get_Scale()->x, m_pTransformCom->Get_Scale()->y, m_pTransformCom->Get_Scale()->z);

	D3DXMATRIX matInverseScale;
	D3DXMatrixInverse(&matInverseScale, 0, &matScale);

	// 최종 월드 행렬: 스케일 적용 후 빌보드 회전 적용
	D3DXMATRIX matFinal = matScale * matBill * matInverseScale * matWorld;

	m_pTransformCom->Set_WorldMatrix(&matFinal);
}

void CMonster::Pattern_Idle(const _float& fTimeDelta)
{
	// 벽 확인 추가할 것
	

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
			m_eDir = BACK;
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, m_fSpeed);
			break;
		case 2:
			// 우상
			m_eDir = RIGHT;
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, m_fDiagSpeed);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, m_fDiagSpeed);
			break;
		case 3:
			// 우
			m_eDir = RIGHT;
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, m_fSpeed);
			break;
		case 4:
			// 우하
			m_eDir = RIGHT;
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, -m_fDiagSpeed);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, m_fDiagSpeed);
			break;
		case 5:
			// 하
			m_eDir = FRONT;
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, -m_fSpeed);
			break;
		case 6:
			// 좌하
			m_eDir = LEFT;
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, -m_fDiagSpeed);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, -m_fDiagSpeed);
			break;
		case 7:
			// 좌
			m_eDir = LEFT;
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, -m_fSpeed);
			break;
		case 8:
			// 좌상
			m_eDir = LEFT;
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, m_fDiagSpeed);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, -m_fDiagSpeed);
			break;
		}
	}

	switch (m_eType)
	{
	case MON_SLIME:
		m_pAnimatorCom->Set_CurState(IDLE, 0, 8, 12);
		break;
	case MON_MUSHROOM:
		if(!m_iDir)
			m_pAnimatorCom->Set_CurState(IDLE, 0, 5, 12);
		else
			m_pAnimatorCom->Set_CurState(IDLE, 9, 17, 12);
		break;
	default:
		break;
	}
}

void CMonster::Pattern_Chase(const _float& fTimeDelta)
{
	Engine::CTransform* pPlayerTransform = dynamic_cast<Engine::CTransform*>
		(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));
	NULL_CHECK(pPlayerTransform);

	_vec3		vPlayerPos, vPos;
	pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	// 플레이어 방향으로 이동, 추후 A스타 알고리즘으로 변경
	switch (m_eType)
	{
	case MON_SLIME:
		m_pAnimatorCom->Set_CurState(WALK, 12, 21, 8);
		m_pTransformCom->Chase_Target(&vPlayerPos, fTimeDelta);
		if (m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, 5.f))
			m_eState = SWING;
		else
			m_eState = WALK;
		break;
	case MON_MUSHROOM:
		m_pAnimatorCom->Set_CurState(WALK, 27, 33, 8);
		if (m_pAnimatorCom->Get_MotionEnd())
		{
			m_fAttackDistance = 0.f;
			m_bAttackFailed = false;
			m_bAttackSuccess = false;
			m_fAttackTime = 0.f;
			m_vAttackPoint = vPlayerPos - vPos;
			D3DXVec3Normalize(&m_vAttackPoint, &m_vAttackPoint);
			m_vAttackPoint.y = 0.f;
			m_eState = SWING;
		}
		break;
	default:
		break;
	}
}

void CMonster::Pattern_Attack(const _float& fTimeDelta)
{
	_vec3		vPos, vPlayerPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	Engine::CTransform* pPlayerTransform = dynamic_cast<Engine::CTransform*>
		(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));
	NULL_CHECK(pPlayerTransform);
	Engine::CCollider* pPlayerCollider = dynamic_cast<Engine::CCollider*>
		(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Collider"));
	switch (m_eType)
	{
	case MON_SLIME:
		m_pAnimatorCom->Set_CurState(SWING, 24, 35, 8);
		pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
		m_fSpeedWeight = 0.f;
		if (m_pAnimatorCom->Get_MotionIndex() <= 29 || m_pAnimatorCom->Get_MotionIndex() >= 34)
		{
			m_bAttackSuccess = false;
			m_bFallStart = false;
			m_vStartPoint = vPos;
			m_vAttackPoint = vPlayerPos;
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
		}
		else
		{
			JumpY(fTimeDelta);
			m_fSpeedWeight = 6.f;
			if (vPos.y > m_fIdleY)
			{
				// 공격 성공
				if (m_pColliderCom->Check_Collision(pPlayerCollider))
				{
					if (!m_bAttackSuccess)
					{
						m_bAttackSuccess = true;
						dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player"))
							->Set_KnockBack(vPos, m_pStateCom->Get_Stat()->iAttack, (1 - (m_fJumpTime / m_fJumpFrame)) * 3.f + 1.f);
					}
				}
			}
			if(m_bAttackSuccess)
				FallDir(fTimeDelta);
			else
				m_pTransformCom->Chase_Target(&m_vAttackPoint, fTimeDelta * m_fSpeedWeight);
		}
		break;
	case MON_MUSHROOM:
		// 일정 스피드 이상일 때 충돌 처리
		if (m_fAttackTime > 0.2f)
		{
			// 플레이어 충돌 시 공격 성공
			if (m_pColliderCom->Check_Collision(pPlayerCollider))
			{
				m_bAttackSuccess = true;
				dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player"))
					->Set_KnockBack(vPos, m_pStateCom->Get_Stat()->iAttack);
			}			
			// 벽 충돌 추가하기
		}
		// 공격 중 : 돌진
		if (!m_bAttackSuccess)
		{
			// 가속
			if (m_fAttackDistance < 6.f)
			{
				m_fAttackTime += fTimeDelta * 1.5f;
				m_pAnimatorCom->Set_CurState(SWING, 9, 17, 3);
				m_pTransformCom->Move_Pos(&m_vAttackPoint, m_fAttackTime * 0.08f, m_fSpeed);
				m_fAttackDistance += m_fSpeed * m_fAttackTime * 0.08f;
			}
			// 감속
			else
			{
				if (m_fAttackDistance < 8.f)
					m_fAttackTime -= fTimeDelta * 2;
				m_pAnimatorCom->Set_CurState(SWING, 9, 17, 3);
				m_pTransformCom->Move_Pos(&m_vAttackPoint, m_fAttackTime * 0.1f, m_fSpeed);
				m_fAttackDistance += m_fSpeed * m_fAttackTime * 0.1f;
			}
			// 정지
			if(m_fAttackDistance > 12.f)
				m_bAttackFailed = true;
		}
		// 공격 성공 : 엉덩방아
		else
		{
			m_pAnimatorCom->Set_CurState(SWING, 36, 44, 6);
			if (m_pAnimatorCom->Get_MotionEnd())
			{
				if (m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, 12.f))
					m_eState = WALK;
				else
				{
					m_eState = IDLE;
					m_iDir = 1;
				}
			}
		}
		
		if (m_bAttackFailed)
		{
			if (m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, 12.f))
				m_eState = WALK;
			else
			{
				m_eState = IDLE;
				m_iDir = 1;
			}
		}

		break;
	default:
		break;
	}
}

void CMonster::Pattern_Dead()
{
	switch (m_eType)
	{
	case MON_SLIME:
		m_pAnimatorCom->Set_CurState(DEAD, 36, 41, 8);
		break;
	case MON_MUSHROOM:
		m_pAnimatorCom->Set_CurState(DEAD, 40, 44, 4);
		break;
	default:
		break;
	}
	if (m_pAnimatorCom->Get_MotionEnd())
	{
		m_bStopDraw = true;
		Drop_Item();
	}
}

void CMonster::JumpY(const _float& fTimeDelta)
{
	_vec3 vPos, vUp;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	m_pTransformCom->Get_Info(INFO_UP, &vUp);
	if (!m_bJumping)
	{
		m_bJumping = true;
		m_fJumpTime = 0;
		m_fJumpHeight = m_fJumpY - m_fIdleY;
		// 각 몬스터마다 점프 프레임 계산
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

void CMonster::FallDir(const _float& fTimeDelta)
{
	if (!m_bFallStart)
	{
		_vec3 vPos;
		m_pTransformCom->Get_Info(INFO_POS, &vPos);
		m_bFallStart = true;
		m_vFallDir = m_vStartPoint - vPos;
		D3DXVec3Normalize(&m_vFallDir, &m_vFallDir);
		m_vFallDir.y = 0;
	}
	m_pTransformCom->Move_Pos(&m_vFallDir, fTimeDelta, m_fSpeedWeight);
}

void CMonster::KnockBack(const _float& fTimeDelta, const _float& fDist)
{
	// 모든 행동보다 우선 시 할 것
	m_bJumping = false;
	m_bIdling = false;
	m_bAttackSuccess = false;
	m_bFallStart = false;

	_vec3 vPos, vUp;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	m_pTransformCom->Get_Info(INFO_UP, &vUp);
	// 검 <> 몬스터 방향 구해서 반대 방향으로 쭉 보내면 될듯


	// m_fIdleY보다 클 경우 Y값도 내려주기
	m_fJumpHeight = vPos.y - m_fIdleY;
	if (m_fJumpHeight > fTimeDelta * m_fSpeedWeight)
	{
		m_fJumpFrame = m_fSpeedWeight / m_fJumpHeight;
		m_pTransformCom->Move_Pos(&vUp, fTimeDelta, -m_fJumpFrame);
	}
	else
		m_pTransformCom->Set_Pos(vPos.x, m_fIdleY, vPos.z);

	// 지정한 거리 이상으로 물러나면 다시 패턴 시작할 수 있도록
	// 각 몬스터 클래스에서 m_bKnockBackEnd 참고할 것
	_vec3 vLength = m_vStartPoint - vPos;
	if (D3DXVec3Length(&vLength) >= fDist)
	{
		m_bKnockBackStart = false;
		m_bKnockBackEnd = true;
		if(m_eState != DEAD)
			m_eState = WALK;
		m_pTransformCom->Set_Pos(vPos.x, m_fIdleY, vPos.z);
		return;
	}
	m_pTransformCom->Move_Pos(&m_vFallDir, fTimeDelta, m_fSpeedWeight);
}

void CMonster::Check_Hitted()
{
	_vec3 vPos, vPlayerPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_GameObject(L"Layer_GameLogic", L"Player"));
	CItem* pPlayerHandedItem = pPlayer->Get_HandedItem();
	
	if (pPlayer->Get_CurState() == SWING)
	{
		CColliderCube* pHandedItemCollider = dynamic_cast<CColliderCube*>(pPlayerHandedItem->Get_Component(ID_DYNAMIC, L"Com_ColliderCube"));
	
		CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pPlayer->Get_Component(ID_DYNAMIC, L"Com_Transform"));
		pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
		if (m_pColliderCom->Check_Cube_Collision(pHandedItemCollider))
		{
			if (!m_bKnockBackStart)
			{
				m_bKnockBackStart = true;
				m_bKnockBackEnd = false;
				m_fSpeedWeight = 8.f;
				m_fJumpHeight = vPos.y - m_fIdleY;
				m_vStartPoint = vPos;
				m_vFallDir = vPos - vPlayerPos;
				D3DXVec3Normalize(&m_vFallDir, &m_vFallDir);
				m_vFallDir.y = 0;
				CState* pPlayerState = dynamic_cast<CState*>(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"Player", L"Com_State"));
				m_pStateCom->Set_Damaged(pPlayerState->Get_Stat()->iAttack);
				if (m_pStateCom->Get_Dead())
					m_eState = DEAD;
			}
		}
	}
}

void CMonster::Drop_Item()
{
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	CScene* pScene = Engine::Get_Scene();
	CItem* pGameObject = nullptr;
	_int iSize = m_vecDropItem.size();
	if (0 == iSize)
		return;
	_int iRand = rand() % iSize;

	ITEMNUM eItem = m_vecDropItem[iRand];
	wstring tagName;

	switch (eItem)
	{
	case ITEM_PICKAXE:
		pGameObject = CPickaxe::Create(m_pGraphicDev, vPos);
		NULL_CHECK_RETURN(pGameObject, );
		tagName = L"Pickaxe" + std::to_wstring(m_iTagNumber++);
		break;
	case ITEM_HOE:
		pGameObject = CHoe::Create(m_pGraphicDev, vPos);
		NULL_CHECK_RETURN(pGameObject, );
		tagName = L"Hoe" + std::to_wstring(m_iTagNumber++);
		break;
	case ITEM_SHOVEL:
		pGameObject = CShovel::Create(m_pGraphicDev, vPos);
		NULL_CHECK_RETURN(pGameObject, );
		tagName = L"Shovel" + std::to_wstring(m_iTagNumber++);
		break;

	case ITEM_SWORD:
		pGameObject = CSword::Create(m_pGraphicDev, vPos);
		NULL_CHECK_RETURN(pGameObject, );
		tagName = L"Sword" + std::to_wstring(m_iTagNumber++);
		break;
	case ITEM_BOW:
		pGameObject = CBow::Create(m_pGraphicDev, vPos);
		NULL_CHECK_RETURN(pGameObject, );
		tagName = L"Bow" + std::to_wstring(m_iTagNumber++);
		break;
	case ITEM_STAFF:
		pGameObject = CStaff::Create(m_pGraphicDev, vPos);
		NULL_CHECK_RETURN(pGameObject, );
		tagName = L"Staff" + std::to_wstring(m_iTagNumber++);
		break;

	case ITEM_NECKLACE:
		break;
	case ITEM_RING:
		break;
	case ITEM_BAG:
		break;
	case ITEM_LANTERN:
		break;

	case ITEM_SEED:
		pGameObject = CSeed::Create(m_pGraphicDev, vPos);
		NULL_CHECK_RETURN(pGameObject, );
		tagName = L"Seed" + std::to_wstring(m_iTagNumber++);
		break;
	case ITEM_WOOD:
		pGameObject = CWood::Create(m_pGraphicDev, vPos);
		NULL_CHECK_RETURN(pGameObject, );
		tagName = L"Wood" + std::to_wstring(m_iTagNumber++);
		break;
	case ITEM_TORCH:
		pGameObject = CTorch::Create(m_pGraphicDev, vPos);
		NULL_CHECK_RETURN(pGameObject, );
		tagName = L"Torch" + std::to_wstring(m_iTagNumber++);
		break;
	case ITEM_MUCUS:
		pGameObject = CMucus::Create(m_pGraphicDev, vPos);
		NULL_CHECK_RETURN(pGameObject, );
		tagName = L"Mucus" + std::to_wstring(m_iTagNumber++);
		break;
	case ITEM_END:
		break;
	default:
		break;
	}

	if (pGameObject)
	{
		pGameObject->Set_Active(true);
		dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);
		FAILED_CHECK_RETURN(pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, tagName.c_str()), );
	}
}

CMonster* CMonster::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
	CMonster* pMonster = new CMonster(pGraphicDev);

	if (FAILED(pMonster->Ready_GameObject(vPos)))
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
