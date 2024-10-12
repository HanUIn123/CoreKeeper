#include "pch.h"
#include "..\Header\Monster.h"
#include "Export_Utility.h"
#include "..\Header\Player.h"
#include "DropItem.h"
#include "../Header/UIFont.h"


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
	m_bImmuneEnd = true;

	m_iSpeedWeight = 1;

	m_pTerrain = nullptr;
	m_pPlayer = nullptr;
	m_pPlayerTransform = nullptr;
	m_pPlayerState = nullptr;
	m_pPlayerCollider = nullptr;
	m_bCheckWall = false;

	m_bRespawned = false;
	m_vRespawnPoint = { 0, 0, 0 };
	m_fRespawnTimer = 0.f;

	m_fSoundVolume = 0.f;

	m_vecDropItem.reserve(3);

	m_vecFont.reserve(10);
	m_iFontNum = 0;
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
		Engine::CSoundMgr::GetInstance()->Play(L"slimeJump1.wav", SOUND_SLIME, m_fSoundVolume);
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
	Set_Stop(fTimeDelta , &m_vFallDir, m_fSpeedWeight);
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

		Set_Stop(fTimeDelta, &m_vFallDir, m_fSpeedWeight);
		m_pTransformCom->Move_Pos(&m_vFallDir, fTimeDelta, m_fSpeedWeight * m_iSpeedWeight);
	}
}

void CMonster::Check_Hitted()
{
	_vec3 vPos, vPlayerPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	CItem* pPlayerHandedItem = m_pPlayer->Get_HandedItem();

	if (m_pPlayer->Get_CurState() == SWING || m_pPlayer->Get_CurState() == SHOOT)
	{
		CColliderCube* pHandedItemCollider = dynamic_cast<CColliderCube*>(pPlayerHandedItem->Get_Component(ID_DYNAMIC, L"Com_ColliderCube"));
		m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
		if (m_pColliderCom->Check_Cube_Collision(pHandedItemCollider))
		{

			m_bHit = true;
			m_bImmuneEnd = false;

			if(pPlayerHandedItem->Get_ItemNum() == ITEM_STAFF)
				Engine::CSoundMgr::GetInstance()->Play(L"sunStaffProjectileImpact.wav", SOUND_PLAYER, 0.2f);
			if (!m_bKnockBackStart)
			{
				switch (m_eType)
				{
				case MON_SLIME:
					Engine::CSoundMgr::GetInstance()->Play(L"slimehurt.wav", SOUND_SLIME, m_fSoundVolume);
					break;
				case MON_SHROOMMAN:
					Engine::CSoundMgr::GetInstance()->Play(L"damage.wav", SOUND_MUSHROOM, m_fSoundVolume);
					break;
				case MON_SHAMAN:
					Engine::CSoundMgr::GetInstance()->Play(L"CavelingHurt.wav", SOUND_SHAMAN, m_fSoundVolume);
					break;
				case MON_HUNTER:
					Engine::CSoundMgr::GetInstance()->Play(L"CavelingHurt.wav", SOUND_HUNTER, m_fSoundVolume);
					break;
				case MON_MALUGAZ:
					Engine::CSoundMgr::GetInstance()->Play(L"Malguaz_Hurt3.wav", SOUND_MALUGAZ, m_fSoundVolume);
					break;
				}
				pPlayerHandedItem->Set_ProjectileAttackSuccess(true);
				m_bKnockBackStart = true;
				m_bKnockBackEnd = false;
				m_fSpeedWeight = 8.f;
				m_fJumpHeight = vPos.y - m_fIdleY;
				m_vStartPoint = vPos;
				m_vFallDir = vPos - vPlayerPos;
				D3DXVec3Normalize(&m_vFallDir, &m_vFallDir);
				m_vFallDir.y = 0;
				m_pStateCom->Set_Damaged(m_pPlayerState->Get_Stat()->iAttack);

				wstring string = std::to_wstring(m_pPlayerState->Get_Stat()->iAttack);
		
				//m_vecFont.push_back(string.c_str());

				m_Font[m_iFontNum] = string.c_str();

				CUIFont* pFont = dynamic_cast<CUIFont*>(Engine::Get_GameObject(L"Layer_UI", L"UI_Font"));

				_matrix matWorld;
				m_pTransformCom->Get_WorldMatrix(&matWorld);

				pFont->Set_Font(matWorld, m_Font[m_iFontNum]);

				m_iFontNum++;

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

	switch (eItem)
	{
	case ITEM_MUSHROOM:
		pGameObject = CIngredient::Create(m_pGraphicDev, ITEM_END, vPos);
		NULL_CHECK(pGameObject);
		m_vecItemName.push_back(L"Monster_Created_Mushroom" + std::to_wstring(m_iTagNumber++));
		break;
	case ITEM_BERRY_SEED:
	case ITEM_PEPPER_SEED:
	case ITEM_CARROT_SEED:
		pGameObject = CSeed::Create(m_pGraphicDev, eItem, vPos);
		NULL_CHECK(pGameObject);
		m_vecItemName.push_back(L"Monster_Created_Ingredients" + std::to_wstring(m_iTagNumber++));
		break;
	case ITEM_SCARLET:
		pGameObject = COre::Create(m_pGraphicDev, MATERIAL_SCARLET, vPos);
		NULL_CHECK(pGameObject);
		m_vecItemName.push_back(L"Monster_Created_Ore" + std::to_wstring(m_iTagNumber++));
		break;
	case ITEM_MUCUS:
		pGameObject = CMucus::Create(m_pGraphicDev, vPos);
		NULL_CHECK(pGameObject);
		m_vecItemName.push_back(L"Monster_Created_Mucus" + std::to_wstring(m_iTagNumber++));
		break;
	case ITEM_SKULL_PIECE:
	case ITEM_FEATHER_PIECE:
		pGameObject = CPiece::Create(m_pGraphicDev, eItem, vPos);
		NULL_CHECK(pGameObject);
		m_vecItemName.push_back(L"Monster_Created_Piece" + std::to_wstring(m_iTagNumber++));
		break;
	case ITEM_ASSISTANCE:
		pGameObject = CAssistance::Create(m_pGraphicDev, ASSISTANCE_FEATHER, vPos);
		NULL_CHECK(pGameObject);
		m_vecItemName.push_back(L"Monster_Created_Feather" + std::to_wstring(m_iTagNumber++));
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

void CMonster::Drop_All_Item()
{
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	// x와 z좌표를 조금씩 밀어서 안겹치게 할까나
	vPos.y = 0.6f;

	CScene* pScene = Engine::Get_Scene();
	CItem* pGameObject = nullptr;
	_int iSize = m_vecDropItem.size();

	for (int i = 0; i < iSize; i++)
	{
		ITEMNUM eItem = m_vecDropItem[i];

		switch (eItem)
		{
		case ITEM_HELMET:
			pGameObject = CHelmet::Create(m_pGraphicDev, MATERIAL_SPECIAL, vPos);
			NULL_CHECK(pGameObject);
			m_vecItemName.push_back(L"Monster_Created_Helmet" + std::to_wstring(m_iTagNumber++));
			break;
		case ITEM_CHEST:
			pGameObject = CChest::Create(m_pGraphicDev, MATERIAL_SPECIAL, vPos);
			NULL_CHECK(pGameObject);
			m_vecItemName.push_back(L"Monster_Created_Chest" + std::to_wstring(m_iTagNumber++));
			break;
		case ITEM_LEG:
			pGameObject = CLeg::Create(m_pGraphicDev, MATERIAL_SPECIAL, vPos);
			NULL_CHECK(pGameObject);
			m_vecItemName.push_back(L"Monster_Created_Chest" + std::to_wstring(m_iTagNumber++));
			break;
		case ITEM_NECKLACE:
			pGameObject = CNecklace::Create(m_pGraphicDev, MATERIAL_SPECIAL, vPos);
			NULL_CHECK(pGameObject);
			m_vecItemName.push_back(L"Monster_Created_Necklace" + std::to_wstring(m_iTagNumber++));
			break;
		case ITEM_RING:
			pGameObject = CRing::Create(m_pGraphicDev, MATERIAL_SPECIAL, vPos);
			NULL_CHECK(pGameObject);
			m_vecItemName.push_back(L"Monster_Created_Ring" + std::to_wstring(m_iTagNumber++));
			break;
		case ITEM_STAFF:
			pGameObject = CStaff::Create(m_pGraphicDev, vPos);
			NULL_CHECK(pGameObject);
			m_vecItemName.push_back(L"Monster_Created_Staff" + std::to_wstring(m_iTagNumber++));
			break;
		case ITEM_ASSISTANCE:
			if (m_eType == MON_MALUGAZ)
			{
				pGameObject = CAssistance::Create(m_pGraphicDev, ASSISTANCE_BOOK, vPos);
				NULL_CHECK(pGameObject);
				m_vecItemName.push_back(L"Monster_Created_Book" + std::to_wstring(m_iTagNumber++));
			}
			else if (m_eType == MON_AZEOS)
			{
				pGameObject = CAssistance::Create(m_pGraphicDev, ASSISTANCE_AZEOS_FEATHER, vPos);
				NULL_CHECK(pGameObject);
				m_vecItemName.push_back(L"Monster_Created_Feather" + std::to_wstring(m_iTagNumber++));
			}
			break;
		case ITEM_INSTRUMENT_HARP:
		case ITEM_INSTRUMENT_CELLO:
		case ITEM_INSTRUMENT_FLUTE:
		case ITEM_INSTRUMENT_OCARINA:
		case ITEM_INSTRUMENT_DRUM:
		case ITEM_INSTRUMENT_PIANO:
			pGameObject = CInstrument::Create(m_pGraphicDev, eItem, vPos);
			NULL_CHECK(pGameObject);
			m_vecItemName.push_back(L"Monster_Created_Instrument" + std::to_wstring(m_iTagNumber++));
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
}

void CMonster::Set_Stop(const _float& fTimeDelta, _vec3* vDir1, _float fDirSpeed1, _vec3* vDir2, _float fDirSpeed2)
{
	m_iSpeedWeight = 1;
	_vec3 vCheckPos{};
	m_pTransformCom->Get_Info(INFO_POS, &vCheckPos);

	// 미래의 캐릭터 중점 좌표
	vCheckPos += *vDir1 * fDirSpeed1 * fTimeDelta * 10;

	if (vDir2)
		vCheckPos += *vDir2 * fDirSpeed2 * fTimeDelta * 10;

	// 미래 중점 좌표 기준 인덱스 값
	_int iIndex = _int(vCheckPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + (vCheckPos.x + 0.5f * VTXITV);
	if (0 <= iIndex && iIndex < (VTXCNTX -1) * (VTXCNTZ - 1))
		if (m_pTerrain->Get_UnreachableByIndex(iIndex))
			m_iSpeedWeight = 0;
}

void CMonster::Set_StuckFree(const _float& fTimeDelta)
{
	_vec3 vCheckPos, vDir;
	m_pTransformCom->Get_Info(INFO_POS, &vCheckPos);
	_int iIndex = _int(vCheckPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + (vCheckPos.x + 0.5f * VTXITV);

	if (0 <= iIndex && iIndex < (VTXCNTX - 1) * (VTXCNTZ - 1))
	{
		if (m_pTerrain->Get_UnreachableByIndex(iIndex))
		{

			_int iLeft = iIndex - 1;
			_int iRight = iIndex + 1;
			_int iBottom = iIndex - (VTXCNTX - 1);
			_int iTop = iIndex + (VTXCNTX - 1);
			_int iTopLeft = iIndex - 1 + (VTXCNTX - 1);
			_int iTopRight = iIndex + 1 + (VTXCNTX - 1);
			_int iBottomLeft = iIndex - 1 - (VTXCNTX - 1);
			_int iBottomRight = iIndex + 1 - (VTXCNTX - 1);
			_float fX = 0.f, fZ = 0.f;
			_vec3 vDestPoint;
			if (0 <= iLeft && iLeft < (VTXCNTX - 1) * (VTXCNTZ - 1))
			{
				if (!m_pTerrain->Get_UnreachableByIndex(iLeft))
				{
					fX = (iLeft % (VTXCNTX - 1)) * VTXITV;
					fZ = (iLeft / (VTXCNTX - 1)) * VTXITV;
				}
			}
			if (0 <= iRight && iRight < (VTXCNTX - 1) * (VTXCNTZ - 1))
			{
				if (!m_pTerrain->Get_UnreachableByIndex(iRight))
				{
					fX = (iRight % (VTXCNTX - 1)) * VTXITV;
					fZ = (iRight / (VTXCNTX - 1)) * VTXITV;
				}
			}
			if (0 <= iBottom && iBottom < (VTXCNTX - 1) * (VTXCNTZ - 1))
			{
				if (!m_pTerrain->Get_UnreachableByIndex(iBottom))
				{
					fX = (iBottom % (VTXCNTX - 1)) * VTXITV;
					fZ = (iBottom / (VTXCNTX - 1)) * VTXITV;
				}
			}
			if (0 <= iTop && iTop < (VTXCNTX - 1) * (VTXCNTZ - 1))
			{
				if (!m_pTerrain->Get_UnreachableByIndex(iTop))
				{
					fX = (iTop % (VTXCNTX - 1)) * VTXITV;
					fZ = (iTop / (VTXCNTX - 1)) * VTXITV;
				}
			}
			if (0 <= iTopLeft && iTopLeft < (VTXCNTX - 1) * (VTXCNTZ - 1))
			{
				if (!m_pTerrain->Get_UnreachableByIndex(iTopLeft))
				{
					fX = (iTopLeft % (VTXCNTX - 1)) * VTXITV;
					fZ = (iTopLeft / (VTXCNTX - 1)) * VTXITV;
				}
			}
			if (0 <= iTopRight && iTopRight < (VTXCNTX - 1) * (VTXCNTZ - 1))
			{
				if (!m_pTerrain->Get_UnreachableByIndex(iTopRight))
				{
					fX = (iTopRight % (VTXCNTX - 1)) * VTXITV;
					fZ = (iTopRight / (VTXCNTX - 1)) * VTXITV;
				}
			}
			if (0 <= iBottomLeft && iBottomLeft < (VTXCNTX - 1) * (VTXCNTZ - 1))
			{
				if (!m_pTerrain->Get_UnreachableByIndex(iBottomLeft))
				{
					fX = (iBottomLeft % (VTXCNTX - 1)) * VTXITV;
					fZ = (iBottomLeft / (VTXCNTX - 1)) * VTXITV;
				}
			}
			if (0 <= iBottomRight && iBottomRight < (VTXCNTX - 1) * (VTXCNTZ - 1))
			{
				if (!m_pTerrain->Get_UnreachableByIndex(iBottomRight))
				{
					fX = (iBottomRight % (VTXCNTX - 1)) * VTXITV;
					fZ = (iBottomRight / (VTXCNTX - 1)) * VTXITV;
				}
			}
			if (fX == 0.f && fZ == 0.f)
			{
				m_pTransformCom->Set_Pos(m_vRespawnPoint.x, m_vRespawnPoint.y, m_vRespawnPoint.z);
			}
			else
			{
				vDestPoint = { fX, 0, fZ };
				vDir = vDestPoint - vCheckPos;
				D3DXVec3Normalize(&vDir, &vDir);
				vDir.y = 0;
				m_iSpeedWeight = 1;
				m_pTransformCom->Move_Pos(&vDir, fTimeDelta, m_fSpeed * 2);
			}
			
		}
	}
}

_bool CMonster::Check_Wall()
{
	m_bCheckWall = false;
	switch (m_eType)
	{
	case MON_SLIME:
		if (m_eState != IDLE && m_pAnimatorCom->Get_MotionEnd())
			Check_WallWithPlayer();
		break;
	case MON_SHROOMMAN:
		if ((m_bAttackSuccess && m_pAnimatorCom->Get_MotionEnd()) || m_eState == IDLE)
			Check_WallWithPlayer();
		break;
	case MON_SHAMAN:
	case MON_HUNTER:
	case MON_MALUGAZ:
		Check_WallWithPlayer();
		break;
	case MON_AZEOS:
		break;
	default:
		Check_WallWithPlayer();
		break;
	}
	return m_bCheckWall;
}

void CMonster::Set_Cast()
{
	if (!m_pPlayerTransform)
		m_pPlayerTransform = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));
	if (!m_pTerrain)
		m_pTerrain = dynamic_cast<CTerrain*>(Engine::Get_GameObject(L"Layer_Environment", L"Terrain"));
	if(!m_pPlayer)
		m_pPlayer = dynamic_cast<CPlayer*>(Get_GameObject(L"Layer_GameLogic", L"Player"));
	if(!m_pPlayerState)
		m_pPlayerState = dynamic_cast<CState*>(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"Player", L"Com_State"));
	if(!m_pPlayerCollider)
		m_pPlayerCollider = dynamic_cast<Engine::CCollider*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Collider"));
}

void CMonster::Respawn(const _float& fTimeDelta)
{
	if (!m_bRespawned)
	{
		m_bRespawned = true;
		m_pTransformCom->Set_Pos(m_vRespawnPoint.x, m_vRespawnPoint.y, m_vRespawnPoint.z);
	}
}

void CMonster::Set_RespawnTimer(const _float& fTimeDelta)
{
	if (m_bStopDraw)
	{
		m_fRespawnTimer += fTimeDelta;
		if (m_fRespawnTimer >= 20.f)
		{
			_vec3 vPos, vPlayerPos, vPlayerDir;
			m_pPlayerTransform->Get_Info(INFO_POS, &vPos);
			m_pTransformCom->Get_Info(INFO_POS, &vPlayerPos);
			if (!m_pCalculatorCom->Check_Distance2D(&vPos, &vPlayerPos, 25.f))
			{
				m_fRespawnTimer = 0.f;
				m_pTransformCom->Set_Pos(m_vRespawnPoint.x, m_vRespawnPoint.y, m_vRespawnPoint.z);
				m_pStateCom->Set_Revive();
				m_bStopDraw = false;
				m_fImmuneTime = 0.f;
				m_bKnockBackStart = false;
				m_bKnockBackEnd = true;
				m_pColliderCom->Set_Offset(_vec3(0, 0, 0));
				m_eState = IDLE;
			}
		}
	}
}

void CMonster::Set_SoundVolumeByDistance()
{
	_vec3 vPos, vPlayerPos;
	m_pPlayerTransform->Get_Info(INFO_POS, &vPos);
	m_pTransformCom->Get_Info(INFO_POS, &vPlayerPos);
	_vec3 vLength = vPlayerPos - vPos;
	_float fLength = D3DXVec3Length(&vLength);
	
	if (fLength < 10.f)
	{
		m_fSoundVolume = (10 - fLength) * 0.05f;
	}
	else
		m_fSoundVolume = 0.f;
}

void CMonster::Check_WallWithPlayer()
{
	_vec3 vPos, vPlayerPos, vPlayerDir;
	m_pPlayerTransform->Get_Info(INFO_POS, &vPos);
	m_pTransformCom->Get_Info(INFO_POS, &vPlayerPos);
	if (m_pCalculatorCom->Check_Distance2D(&vPos, &vPlayerPos, m_fAggroDistance))
	{
		_vec3 vPlayerDistance = vPlayerPos - vPos;
		D3DXVec3Normalize(&vPlayerDir, &vPlayerDistance);
		_float fPlayerDistance = D3DXVec3Length(&vPlayerDistance);
		for (_int i = 0; i < (_int)fPlayerDistance * 10; i++)
		{
			_vec3 vCheckPos = vPos + vPlayerDir * i * 0.1f;
			_int iIndex = _int(vCheckPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + (vCheckPos.x + 0.5f * VTXITV);
			if (0 <= iIndex && iIndex < (VTXCNTX - 1) * (VTXCNTZ - 1))
			{
				if (m_pTerrain->Get_UnreachableByIndex(iIndex))
				{
					m_eState = IDLE;
					m_iDir = 0;
					m_bCheckWall = true;
				}
			}
		}
	}
}

void CMonster::Free()
{
	Engine::CGameObject::Free();
}
