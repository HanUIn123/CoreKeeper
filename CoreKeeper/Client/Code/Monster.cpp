#include "pch.h"
#include "..\Header\Monster.h"
#include "Export_Utility.h"
#include "..\Header\Player.h"
#include "DropItem.h"


int	CMonster::m_iTagNumber = 10;

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
	m_eType = MON_END;
	m_eState = STATE_END;

	m_bFlip = false;

	m_fIdleY = 0.f;
	m_bIdling = false;
	m_fIdleTime = 0.f;
	m_fIdleTimeLimit = 0.f;

	m_fJumpY = 0.f;
	m_bJumping = false;
	m_fJumpHeight = 0.f;
	m_fJumpSpeed = 0.f;
	m_fJumpTime = 0.f;
	m_fJumpFrame = 0.f;

	m_vStartPoint = { 0, 0, 0 };

	m_fAttackDistance = 0.f;
	m_fAttackTime = 0.f;
	m_vAttackPoint = { 0, 0, 0 };
	m_bAttackSuccess = false;
	m_bAttackFailed = false;

	m_bKnockBackStart = false;
	m_bKnockBackEnd = true;

	m_bFallStart = false;
	m_vFallDir = { 0, 0, 0 };

	m_iDir = 0;
	m_eDir = DIRECTION_END;
	m_fSpeed = 0.f;
	m_fDiagSpeed = 0.f;
	m_fSpeedWeight = 0.f;

	m_fAggroDistance = 0.f;

	m_bStopDraw = false;

	m_fImmuneTime = 0.f;
	m_fImmuneTimeLimit = 0.f;

	m_iSpeedWeight = 1;

	m_vecDropItem.reserve(3);
}

CMonster::~CMonster()
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

void CMonster::Flip()
{
	if (m_eDir == LEFT && !m_bFlip)
	{
		m_bFlip = true;
		_vec3 vSize;
		vSize = *(m_pTransformCom->Get_Scale());
		m_pTransformCom->Set_Scale(-vSize.x, vSize.y, vSize.z);
	}
	if (m_eDir != LEFT && m_bFlip)
	{
		m_bFlip = false;
		_vec3 vSize;
		vSize = *(m_pTransformCom->Get_Scale());
		m_pTransformCom->Set_Scale(-vSize.x, vSize.y, vSize.z);
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

	m_fJumpSpeed = m_fJumpHeight * fWeight * 1.5f;
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
	Set_Stop(&m_vFallDir, m_fSpeedWeight);
	m_pTransformCom->Move_Pos(&m_vFallDir, fTimeDelta, m_fSpeedWeight * m_iSpeedWeight);
}

void CMonster::KnockBack(const _float& fTimeDelta, const _float& fDist)
{
	if (fDist > 0.f)
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
		if (D3DXVec3Length(&vLength) >= fDist || m_iSpeedWeight == 0)
		{
			m_bKnockBackStart = false;
			m_bKnockBackEnd = true;
			if (m_eState != DEAD)
				m_eState = WALK;
			m_pTransformCom->Set_Pos(vPos.x, m_fIdleY, vPos.z);
			return;
		}

		Set_Stop(&m_vFallDir, m_fSpeedWeight);
		m_pTransformCom->Move_Pos(&m_vFallDir, fTimeDelta, m_fSpeedWeight * m_iSpeedWeight);
	}
}

void CMonster::Check_Hitted()
{
	_vec3 vPos, vPlayerPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_GameObject(L"Layer_GameLogic", L"Player"));
	CItem* pPlayerHandedItem = pPlayer->Get_HandedItem();

	if (pPlayer->Get_CurState() == SWING || pPlayer->Get_CurState() == SHOOT)
	{
		CColliderCube* pHandedItemCollider = dynamic_cast<CColliderCube*>(pPlayerHandedItem->Get_Component(ID_DYNAMIC, L"Com_ColliderCube"));

		CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pPlayer->Get_Component(ID_DYNAMIC, L"Com_Transform"));
		pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
		if (m_pColliderCom->Check_Cube_Collision(pHandedItemCollider))
		{

			m_bHit = true;
			
			if (!m_bKnockBackStart)
			{
				pPlayerHandedItem->Set_ProjectileAttackSuccess(true);
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
		pGameObject = CPickaxe::Create(m_pGraphicDev, MATERIAL_WOOD, vPos);
		NULL_CHECK(pGameObject);
		m_vecItemName.push_back(L"Monster_Created_Pickaxe" + std::to_wstring(m_iTagNumber++));
		break;
	case ITEM_HOE:
		pGameObject = CHoe::Create(m_pGraphicDev, MATERIAL_WOOD, vPos);
		NULL_CHECK(pGameObject);
		m_vecItemName.push_back(L"Monster_Created_Hoe" + std::to_wstring(m_iTagNumber++));
		break;

	case ITEM_SWORD:
		pGameObject = CSword::Create(m_pGraphicDev, MATERIAL_WOOD, vPos);
		NULL_CHECK(pGameObject);
		m_vecItemName.push_back(L"Monster_Created_Sword" + std::to_wstring(m_iTagNumber++));
		break;
	case ITEM_BOW:
		pGameObject = CBow::Create(m_pGraphicDev, MATERIAL_IRON, vPos);
		NULL_CHECK(pGameObject);
		m_vecItemName.push_back(L"Monster_Created_Bow" + std::to_wstring(m_iTagNumber++));
		break;
	case ITEM_STAFF:
		pGameObject = CStaff::Create(m_pGraphicDev, vPos);
		NULL_CHECK(pGameObject);
		m_vecItemName.push_back(L"Monster_Created_Staff" + std::to_wstring(m_iTagNumber++));
		break;

	case ITEM_NECKLACE:
		break;
	case ITEM_RING:
		break;
	case ITEM_BAG:
		break;
	case ITEM_LANTERN:
		break;

	case ITEM_WOOD:
		pGameObject = CWood::Create(m_pGraphicDev, vPos);
		NULL_CHECK(pGameObject);
		m_vecItemName.push_back(L"Monster_Created_Wood" + std::to_wstring(m_iTagNumber++));
		break;
	case ITEM_TORCH:
		pGameObject = CTorch::Create(m_pGraphicDev, vPos);
		NULL_CHECK(pGameObject);
		m_vecItemName.push_back(L"Monster_Created_Torch" + std::to_wstring(m_iTagNumber++));
		break;
	case ITEM_MUCUS:
		pGameObject = CMucus::Create(m_pGraphicDev, vPos);
		NULL_CHECK(pGameObject);
		m_vecItemName.push_back(L"Monster_Created_Mucus" + std::to_wstring(m_iTagNumber++));
		break;
	case ITEM_END:
		break;
	default:
		break;
	}

	if (pGameObject)
	{
		FAILED_CHECK_RETURN(pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, m_vecItemName.back().c_str()), );
		pGameObject->Set_Active(true);
		pGameObject->Set_Drop(true);
	}
}

void CMonster::Set_Stop(_vec3* vDir1, _float fDirSpeed1, _vec3* vDir2, _float fDirSpeed2)
{
	m_iSpeedWeight = 1;
	_vec3 vCheckPos{};
	m_pTransformCom->Get_Info(INFO_POS, &vCheckPos);

	// 미래의 캐릭터 중점 좌표
	vCheckPos += *vDir1 * fDirSpeed1 * 0.1f;

	if (vDir2)
		vCheckPos += *vDir2 * fDirSpeed2 * 0.1f;

	// 미래 중점 좌표 기준 인덱스 값
	_int iIndex = _int(vCheckPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + (vCheckPos.x + 0.5f * VTXITV);
	CTerrain* pTerrain = dynamic_cast<CTerrain*>(Engine::Get_GameObject(L"Layer_Environment", L"Terrain"));
	if (0 <= iIndex && iIndex < VTXCNTX * VTXCNTZ)
		if (pTerrain->Get_UnreachableByIndex(iIndex))
			m_iSpeedWeight = 0;
}

void CMonster::Set_StuckFree(const _float& fTimeDelta)
{
	CTerrain* pTerrain = dynamic_cast<CTerrain*>(Engine::Get_GameObject(L"Layer_Environment", L"Terrain"));
	_vec3 vCheckPos, vDir;
	m_pTransformCom->Get_Info(INFO_POS, &vCheckPos);
	_int iIndex = _int(vCheckPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + (vCheckPos.x + 0.5f * VTXITV);

	if (0 <= iIndex && iIndex < VTXCNTX * VTXCNTZ)
	{
		if (pTerrain->Get_UnreachableByIndex(iIndex))
		{
			vDir = vCheckPos - _vec3(_int(vCheckPos.x + 0.5f * VTXITV), 0, _int(vCheckPos.z + 0.5f * VTXITV) * (VTXCNTX - 1));
			D3DXVec3Normalize(&vDir, &vDir);
			vDir.y = 0;
			m_iSpeedWeight = 1;
			m_pTransformCom->Move_Pos(&vDir, fTimeDelta, m_fSpeed * 10);
		}
	}
}

void CMonster::Render_HitEffect()
{
}

void CMonster::Free()
{
	Engine::CGameObject::Free();
}
