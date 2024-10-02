#include "pch.h"
#include "..\Header\Player.h"

#include "Export_Utility.h"
#include "..\Header\UIStatusBar.h"
#include "..\Header\Sword.h"
#include "..\Header\Terrain.h"

#include "..\Header\UICraft.h" // UI 헤더 추가
#include "..\Header\UIScreenIcon.h"
#include "..\Header\UIScreenInv.h"
#include "..\Header\UIInventory.h"
#include "..\Header\UIPlayerStatus.h"
#include "..\Header\UIInvPlate.h"
#include "..\Header\UIItemSlot.h"
#include "..\Header\UIPlayerStats.h"
#include "..\Header\UICraftSlot.h"
#include "..\Header\UITrashCan.h"
#include "..\Header\UITrashSlot.h"
#include "..\Header\UISort.h"
#include "..\Header\UIBuff.h"
#include "..\Header\UIChestInv.h"
#include "..\Header\UIStatue.h"
#include "..\Header\UIJemSlot.h"
#include "..\Header\UIStatueCraft.h"
#include "..\Header\UIChestSort.h"
#include "..\Header\Stage.h"
#include "..\Header\GravestoneObject.h"
#include "..\Header\UIFurnace.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
	m_eDir = DIRECTION_END;
	m_eState = STATE_END;
	m_fSpeed = 5.f;
	m_fDiagSpeed = sqrt(pow(m_fSpeed, 2) / 2);

	m_bSwing = false;
	m_bShoot = false;
	m_fClickTime = 0.f;

	m_fFirstY = 1.f;
	m_fTimeAcc = 0.f;
	m_fWalkYSpeed = 1.8f;
	m_iSpeedWeight = 1;

	m_pHandedItem = nullptr;
	m_pHandedTransformCom = nullptr;
	m_iHandNum = 0;
	ZeroMemory(&m_tBasicStat, sizeof(STAT));
	ZeroMemory(&m_tEquipmentStat, sizeof(STAT));

	for (int i = 0; i < 5; i++)
		m_pClothes[i] = nullptr;

	m_bMap = false;
	m_bInventory = false;
	m_bCraft = false;
	m_bFlip = false;
	m_bNoMove = false;
	m_bStatus = false;
	m_bChestInventory = false;
	m_bTableCraft = false;
	m_bAnvil = false;
	m_bStatue = false;
	m_bGraveInventory = false;
	m_bFurnace = false;

	m_vStartPoint = { 0, 0, 0 };
	m_vKnockBackDir = { 0, 0, 0 };
	m_bKnockBackStart = false;
	m_bKnockBackEnd = true;
	m_fKnockBackDist = 0.f;
	m_bNude = true;

	m_bRespawned = false;
	m_vRespawnPoint = { VTXCNTX * 0.5f, 0, VTXCNTZ * 0.5f };

	m_bBleed = false;
	m_fBleedTime = 0.f;

	m_bShootOnce = false;
	m_vMouseWorldPos = { 0, 0, 0 };
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_tBasicStat = STAT( 400, 100, 20, 0 );
	m_pStateCom->Set_Stat(m_tBasicStat.iMaxHp, m_tBasicStat.iMaxMp, m_tBasicStat.iAttack, m_tBasicStat.iDefense);
	m_pEquipInventoryCom->Set_SlotCount(10);

	m_pFireParticleCom->init(L"../Bin/Resource/Texture/Particle/Basic_Particle.png"); // 파티클 시작
	m_pFollowParticleCom->init(L"../Bin/Resource/Texture/Particle/Fire_Particle/Fire_Particle_%d.png", 5); // 파티클 시작
	return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bNude)
		Set_Clothes();
	Set_MouseWorldPos();

	KnockBack(fTimeDelta);
	
	Set_Equipment();
	if (m_eState != SWING)
		Show_Equipment();
	Set_EquippedStatus();

	if (!m_bNoMove && !m_bInventory && !m_bCraft && !m_bMap) // m_bNoMove -> UICursor에서 적용
		Mouse_Click(fTimeDelta);
	else
	{
		m_bSwing = false;
		m_bShoot = false;
	}

	if (m_bKnockBackEnd)
	{
		if (g_bIsTopCamera)
		{
			if (!m_bSwing && !m_bShoot)
			{
				Key_Position(fTimeDelta);
				Mouse_Direction();
			}
			Animation_SetUp(m_eState, m_eDir);
		}
		else
		{
			if (!m_bSwing && !m_bShoot)
				ShoulderView_Control(fTimeDelta);
			else
				ShoulderView_Swing();
		}
		if (m_eState == WALK)
			Walk_Y(fTimeDelta);
		else
		{
			_vec3 vPos;
			m_pTransformCom->Get_Info(INFO_POS, &vPos);
			m_pTransformCom->Set_Pos(vPos.x, m_fFirstY, vPos.z);
		}
	}

	Flip();

	if (!m_bRespawned)
	{
		m_bRespawned = true;
		m_pTransformCom->Set_Pos(m_vRespawnPoint.x, m_fFirstY, m_vRespawnPoint.z);
	}

	m_pAnimatorCom->Update_Animation();
	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	Particle_Update(fTimeDelta);

	Add_RenderGroup(RENDER_ALPHA, this);

	Set_UI();


	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CPlayer::LateUpdate_GameObject()
{
	
	Engine::CGameObject::LateUpdate_GameObject();
}

void CPlayer::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());
	m_pBufferCom->Render_Buffer();
	m_pColliderCom->Render_Collider();
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	if(m_bBleed)
	   m_pFireParticleCom->render(); // 파티클 렌더

	m_pFollowParticleCom->render();
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

	pComponent = m_pColliderCom = dynamic_cast<CCollider*>(Engine::Clone_Proto(L"Proto_PlayerCollider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

	pComponent = m_pStateCom = dynamic_cast<CState*>(Engine::Clone_Proto(L"Proto_State"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_State", pComponent });

	pComponent = m_pInventoryCom = dynamic_cast<CInventory*>(Engine::Clone_Proto(L"Proto_PlayerInventory"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Inventory", pComponent });

	pComponent = m_pEquipInventoryCom = dynamic_cast<CInventory*>(Engine::Clone_Proto(L"Proto_EquipInventory"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_EquipInventory", pComponent });

	pComponent = m_pFireParticleCom = dynamic_cast<CFall*>(Engine::Clone_Proto(L"Proto_Fall"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Fall", pComponent });

	pComponent = m_pFollowParticleCom = dynamic_cast<CFollow*>(Engine::Clone_Proto(L"Proto_Followers"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Follow", pComponent });
	///m_pFireParticleCom
	return S_OK;
}

void CPlayer::Key_Position(const _float& fTimeDelta)
{
	_vec3	vLook, vRight;
	m_pTransformCom->Get_Info(INFO_LOOK, &vLook);
	m_pTransformCom->Get_Info(INFO_RIGHT, &vRight);

	m_eState = WALK;
	_float fLookSpeed(0.f), fRightSpeed(0.f);
	// TOP (RIGHT, LEFT)
	if (Engine::Get_DIKeyState(DIK_W))
	{
		if (Engine::Get_DIKeyState(DIK_D))
		{
			fLookSpeed = m_fDiagSpeed;
			fRightSpeed = m_fDiagSpeed;
		}
		else if (Engine::Get_DIKeyState(DIK_A))
		{
			fLookSpeed = m_fDiagSpeed;
			fRightSpeed = -m_fDiagSpeed;
		}
		else
			fLookSpeed = m_fSpeed;
	}
	// BOTTOM (RIGHT, LEFT)
	else if (Engine::Get_DIKeyState(DIK_S))
	{
		if (Engine::Get_DIKeyState(DIK_D))
		{
			fLookSpeed = -m_fDiagSpeed;
			fRightSpeed = m_fDiagSpeed;
		}
		else if (Engine::Get_DIKeyState(DIK_A))
		{
			fLookSpeed = -m_fDiagSpeed;
			fRightSpeed = -m_fDiagSpeed;
		}
		else
			fLookSpeed = -m_fSpeed;
	}
	// L
	else if (Engine::Get_DIKeyState(DIK_A))
		fRightSpeed = -m_fSpeed;
	// R
	else if (Engine::Get_DIKeyState(DIK_D))
		fRightSpeed = m_fSpeed;
	else
		m_eState = IDLE;
	
	if (m_eState == WALK)
	{
		_int iWeight = 1;
		if (m_eDir == LEFT)
			iWeight = -1;

		Set_Stop(&vLook, fLookSpeed, &vRight, fRightSpeed * iWeight);

		m_pTransformCom->Move_Pos(&vLook, fTimeDelta, fLookSpeed * m_iSpeedWeight);
		m_pTransformCom->Move_Pos(&vRight, fTimeDelta, fRightSpeed * iWeight * m_iSpeedWeight);
	}
}

void CPlayer::Mouse_Click(const _float& fTimeDelta)
{
	if (m_bSwing)
	{
		if (m_pAnimatorCom->Get_MotionEnd())
			m_bSwing = false;
	}
	else if (m_bShoot)
	{
		m_fClickTime += fTimeDelta;
		if (m_fClickTime > 0.5f)
		{
			m_bShoot = false;
			m_bShootOnce = false;
			m_fClickTime = 0.f;
		}
	}
	else
	{
		if (Engine::Get_DIMouseState(DIM_LB) & 0x80)
		{
			if (m_pHandedItem)
			{
				switch (m_pHandedItem->Get_ItemNum())
				{
				case ITEM_SWORD:
					m_eState = SWING;
					m_bSwing = true;
					Swing_Equipment();
					break;
				case ITEM_PICKAXE:
					m_eState = SWING;
					m_bSwing = true;
					Swing_Equipment();
					PickAxe();
					break;
				case ITEM_HOE:
					m_eState = SWING;
					m_bSwing = true;
					Swing_Equipment();
					Hoe();
					break;
				case ITEM_BOW:
				case ITEM_STAFF:
					m_eState = SHOOT;
					m_bShoot = true;
					Shoot_Equipment();
					break;
					//case ITEM_SEED:
					//	// 농사
					//	break;
				default:
					break;
				}
			}

		}
	}
}
void CPlayer::Walk_Y(const _float& fTimeDelta)
{
	m_fTimeAcc += fTimeDelta * 11.f;

	if (m_fTimeAcc >= 1.0f)
	{
		m_fWalkYSpeed *= -1;
		m_fTimeAcc = 0.0f;
	}
	_vec3 vUp;
	m_pTransformCom->Get_Info(INFO_UP, &vUp);

	m_pTransformCom->Move_Pos(&vUp, fTimeDelta, m_fWalkYSpeed * m_iSpeedWeight);
	if (m_pHandedItem)
		m_pHandedItem->Walk_Equipped(fTimeDelta);
}

void CPlayer::Flip()
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

void CPlayer::Mouse_Direction()
{
	POINT	ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	float diagLBtoRT = -((float)WINCY / WINCX * ptMouse.x) + WINCY - ptMouse.y;
	float diagLTtoRB = ((float)WINCY / WINCX * ptMouse.x) - ptMouse.y;

	// 우측
	if (diagLBtoRT < 0 && diagLTtoRB > 0)
		m_eDir = RIGHT;
	// 하단
	else if (diagLBtoRT <= 0 && diagLTtoRB <= 0)
		m_eDir = FRONT;
	// 좌측
	else if (diagLBtoRT >= 0 && diagLTtoRB <= 0)
		m_eDir = LEFT;
	// 상단
	else if (diagLBtoRT >= 0 && diagLTtoRB >= 0)
		m_eDir = BACK;
}

void CPlayer::Animation_SetUp(STATE st, DIRECTION dir)
{
	switch (st)
	{
	case IDLE:
		if (m_eDir == FRONT)
			m_pAnimatorCom->Set_CurState(st, 0, 0, 20);
		else if (m_eDir == BACK)
			m_pAnimatorCom->Set_CurState(st, 2, 2, 20);
		else
			m_pAnimatorCom->Set_CurState(st, 1, 1, 20);
		break;
	case WALK:
		if (m_eDir == FRONT)
			m_pAnimatorCom->Set_CurState(st, 9, 14, 6);
		else if (m_eDir == BACK)
			m_pAnimatorCom->Set_CurState(st, 27, 32, 6);
		else
			m_pAnimatorCom->Set_CurState(st, 18, 23, 6);
		break;
	case SWING:
		if (m_pHandedItem)
		{
			if (m_eDir == FRONT)
				m_pAnimatorCom->Set_CurState(st, 36, 36, 10);
			else if (m_eDir == BACK)
				m_pAnimatorCom->Set_CurState(st, 40, 41, 5);
			else
				m_pAnimatorCom->Set_CurState(st, 37, 39, 3);
		}
		break;
	case SHOOT:
		if (m_pHandedItem)
		{
			if (m_eDir == FRONT)
				m_pAnimatorCom->Set_CurState(st, 36, 36, 20);
			else if (m_eDir == BACK)
				m_pAnimatorCom->Set_CurState(st, 40, 41, 10);
			else
				m_pAnimatorCom->Set_CurState(st, 37, 39, 8);
		}
		break;
	}
}

void CPlayer::ShoulderView_Control(const _float& fTimeDelta)
{
	_vec3	vLook, vRight;
	m_pTransformCom->Get_Info(INFO_LOOK, &vLook);
	m_pTransformCom->Get_Info(INFO_RIGHT, &vRight);

	m_eState = WALK;
	_float fLookSpeed(0), fRightSpeed(0);
	// TOP (RIGHT, LEFT)
	if (Engine::Get_DIKeyState(DIK_W))
	{
		if (Engine::Get_DIKeyState(DIK_D))
		{
			m_eDir = RIGHT;
			fLookSpeed = m_fDiagSpeed;
			fRightSpeed = m_fDiagSpeed;
		}
		else if (Engine::Get_DIKeyState(DIK_A))
		{
			m_eDir = LEFT;
			fLookSpeed = m_fDiagSpeed;
			fRightSpeed = m_fDiagSpeed;
		}
		else
		{
			m_eDir = BACK;
			fLookSpeed = m_fSpeed;
		}
	}
	// BOTTOM (RIGHT, LEFT)
	else if (Engine::Get_DIKeyState(DIK_S))
	{
		if (Engine::Get_DIKeyState(DIK_D))
		{
			m_eDir = RIGHT;
			fLookSpeed = -m_fDiagSpeed;
			fRightSpeed = m_fDiagSpeed;
		}
		else if (Engine::Get_DIKeyState(DIK_A))
		{
			m_eDir = LEFT;
			fLookSpeed = -m_fDiagSpeed;
			fRightSpeed = m_fDiagSpeed;
		}
		else
		{
			m_eDir = FRONT;
			fLookSpeed = -m_fSpeed;
		}
	}
	// L
	else if (Engine::Get_DIKeyState(DIK_A))
	{
		m_eDir = LEFT;
		fRightSpeed = m_fSpeed;
	}
	// R
	else if (Engine::Get_DIKeyState(DIK_D))
	{
		m_eDir = RIGHT;
		fRightSpeed = m_fSpeed;
	}
	else
	{
		m_eState = IDLE;
		m_eDir = BACK;
		m_pAnimatorCom->Set_CurState(IDLE, 2, 2, 20);
	}
	if (m_eState == WALK)
	{
		switch (m_eDir)
		{
		case RIGHT:
		case LEFT:
			m_pAnimatorCom->Set_CurState(WALK, 18, 23, 6);
			break;
		case FRONT:
			m_pAnimatorCom->Set_CurState(WALK, 9, 14, 6);
			break;
		case BACK:
			m_pAnimatorCom->Set_CurState(WALK, 27, 32, 6);
			break;
		}
		Set_Stop(&vLook, fLookSpeed, &vRight, fRightSpeed);

		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, fLookSpeed * m_iSpeedWeight);
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, fRightSpeed * m_iSpeedWeight);
	}

}

void CPlayer::ShoulderView_Swing()
{
	if (m_pHandedItem)
	{
		if (m_bSwing)
			m_pAnimatorCom->Set_CurState(SWING, 40, 41, 5);
		else if (m_bShoot)
			m_pAnimatorCom->Set_CurState(SWING, 40, 41, 10);
	}
}

void CPlayer::Set_Stop(_vec3* vDir1, _float fDirSpeed1, _vec3* vDir2, _float fDirSpeed2)
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

void CPlayer::Set_Equipment()
{
	m_pHandedItem = m_pInventoryCom->Get_HandedItem(m_iHandNum);
	if (m_pHandedItem)
		m_pHandedTransformCom = dynamic_cast<CTransform*>(m_pHandedItem->Get_Component(ID_DYNAMIC, L"Com_Transform"));
}

void CPlayer::Show_Equipment()
{
	vector<CItem*>* pInventory = m_pInventoryCom->Get_VecItemP();
	for (auto iter = pInventory->begin(); iter != pInventory->end(); iter++)
	{
		if ((*iter))
		{
			(*iter)->Set_Use(false);
			(*iter)->Set_Active(false);
		}
	}
	ZeroMemory(&m_tEquipmentStat, sizeof(STAT));
	// 무기(손)
	if (m_pHandedItem)
	{
		m_tEquipmentStat.iAttack = m_pHandedItem->Get_Stat()->iAttack;
		m_pHandedItem->Set_Use(true);
		m_pHandedItem->Set_Active(true);
		m_pHandedTransformCom->Set_Angle(0, 0, 0);
		_vec3 vPlayerPos;
		m_pTransformCom->Get_Info(INFO_POS, &vPlayerPos);

		if (g_bIsTopCamera)
		{
			if (m_pHandedItem->Get_ItemNum() == ITEM_BOW)
			{
				switch (m_eDir)
				{
				case FRONT:
					m_pHandedTransformCom->Set_Pos(vPlayerPos.x - 0.3f, 1.2f, vPlayerPos.z - 0.2f);
					break;
				case RIGHT:
					m_pHandedTransformCom->Set_Pos(vPlayerPos.x - 0.2f, 1.2f, vPlayerPos.z - 0.2f);
					break;
				case BACK:
					m_pHandedTransformCom->Set_Pos(vPlayerPos.x + 0.3f, 1.2f, vPlayerPos.z + 0.2f);
					break;
				case LEFT:
					m_pHandedTransformCom->Set_Pos(vPlayerPos.x + 0.2f, 1.2f, vPlayerPos.z - 0.2f);
					break;
				}
			}
			else
			{
				switch (m_eDir)
				{
				case FRONT:
					m_pHandedTransformCom->Set_Pos(vPlayerPos.x - 0.5f, 1.f, vPlayerPos.z - 0.2f);
					break;
				case RIGHT:
					m_pHandedTransformCom->Set_Pos(vPlayerPos.x - 0.4f, 1.f, vPlayerPos.z - 0.2f);
					break;
				case BACK:
					m_pHandedTransformCom->Set_Pos(vPlayerPos.x + 0.5f, 1.f, vPlayerPos.z + 0.2f);
					break;
				case LEFT:
					m_pHandedTransformCom->Set_Pos(vPlayerPos.x + 0.4f, 1.f, vPlayerPos.z - 0.2f);
					break;
				}
			}

		}
		else
		{
			m_pHandedTransformCom->Set_ResetArbit();
			_vec3 vPlayerAngle = *(m_pTransformCom->Get_Angle());
			m_pHandedTransformCom->Set_Angle(vPlayerAngle.x, vPlayerAngle.y, vPlayerAngle.z);
			_vec3 vPlayerLook, vPlayerRight;
			m_pTransformCom->Get_Info(INFO_LOOK, &vPlayerLook);
			m_pTransformCom->Get_Info(INFO_RIGHT, &vPlayerRight);
			switch (m_eDir)
			{
			case FRONT:
				m_pHandedTransformCom->Set_Pos(vPlayerPos.x - vPlayerLook.x * 0.4f - vPlayerRight.x * 0.5f, 1.f, vPlayerPos.z - vPlayerLook.z * 0.4f - vPlayerRight.z * 0.5f);
				break;
			case RIGHT:
				m_pHandedTransformCom->Set_Pos(vPlayerPos.x - vPlayerLook.x * 0.4f - vPlayerRight.x * 0.2f, 1.f, vPlayerPos.z - vPlayerLook.z * 0.4f - vPlayerRight.z * 0.2f);
				break;
			case BACK:
				m_pHandedTransformCom->Set_Pos(vPlayerPos.x + vPlayerLook.x * 0.4f + vPlayerRight.x * 0.5f, 1.f, vPlayerPos.z + vPlayerLook.z * 0.4f + vPlayerRight.z * 0.5f);
				break;
			case LEFT:
				m_pHandedTransformCom->Set_Pos(vPlayerPos.x - vPlayerLook.x * 0.4f - vPlayerRight.x * 0.2f, 1.f, vPlayerPos.z - vPlayerLook.z * 0.4f - vPlayerRight.z * 0.2f);
				break;
			}
		}
	}

	// 방어구
	CItem* pArmor;
	for (_int i = 0; i < CUIItemSlot::SLOT_END; i++)
	{
		wstring	strObjectTag = L"UIItemSlot_";
		switch (i)
		{
		case CUIItemSlot::SLOT_HELM:
			strObjectTag += std::to_wstring(i);
			pArmor = dynamic_cast<CUIItemSlot*>(Engine::Get_GameObject(L"Layer_UI", strObjectTag.c_str()))->Get_Item();
			if (pArmor)
			{
				m_tEquipmentStat.iMaxHp += pArmor->Get_Stat()->iMaxHp;
				m_tEquipmentStat.iDefense += pArmor->Get_Stat()->iDefense;
				pArmor->Set_Active(true);
				m_pClothes[0]->Set_Active(false);
				m_pClothes[1]->Set_Active(false);
				m_pClothes[2]->Set_Active(false);
				pArmor->Set_Follow();
			}
			else
			{
				m_pClothes[0]->Set_Active(true);
				m_pClothes[1]->Set_Active(true);
				m_pClothes[2]->Set_Active(true);
			}
			break;
		case CUIItemSlot::SLOT_CHEST:
			strObjectTag += std::to_wstring(i);
			pArmor = dynamic_cast<CUIItemSlot*>(Engine::Get_GameObject(L"Layer_UI", strObjectTag.c_str()))->Get_Item();
			if (pArmor)
			{
				m_tEquipmentStat.iMaxHp += pArmor->Get_Stat()->iMaxHp;
				m_tEquipmentStat.iDefense += pArmor->Get_Stat()->iDefense;
				pArmor->Set_Active(true);
				m_pClothes[3]->Set_Active(false);
				pArmor->Set_Follow();
			}
			else
				m_pClothes[3]->Set_Active(true);
			break;
		case CUIItemSlot::SLOT_LEGGINGS:
			strObjectTag += std::to_wstring(i);
			pArmor = dynamic_cast<CUIItemSlot*>(Engine::Get_GameObject(L"Layer_UI", strObjectTag.c_str()))->Get_Item();
			if (pArmor)
			{
				m_tEquipmentStat.iMaxHp += pArmor->Get_Stat()->iMaxHp;
				m_tEquipmentStat.iDefense += pArmor->Get_Stat()->iDefense;
				pArmor->Set_Active(true);
				m_pClothes[4]->Set_Active(false);
				pArmor->Set_Follow();
			}
			else
				m_pClothes[4]->Set_Active(true);
			break;
		}
	}
}
void CPlayer::Swing_Equipment()
{
	if (m_pHandedItem)
	{
		_vec3 vPlayerPos;
		m_pTransformCom->Get_Info(INFO_POS, &vPlayerPos);

		if (g_bIsTopCamera)
		{
			switch (m_eDir)
			{
			case FRONT:
				m_pHandedTransformCom->Set_Pos(vPlayerPos.x + 0.2f, vPlayerPos.y, vPlayerPos.z - 0.4f);
				break;
			case RIGHT:
				m_pHandedTransformCom->Set_Pos(vPlayerPos.x + 0.6f, vPlayerPos.y, vPlayerPos.z + 0.4f);
				break;
			case BACK:
				m_pHandedTransformCom->Set_Pos(vPlayerPos.x - 0.2f, vPlayerPos.y, vPlayerPos.z + 0.4f);
				break;
			case LEFT:
				m_pHandedTransformCom->Set_Pos(vPlayerPos.x - 0.6f, vPlayerPos.y, vPlayerPos.z + 0.4f);
				break;
			}
		}
		else
		{
			m_eDir = BACK;
			_vec3 vPlayerLook, vPlayerRight;
			m_pTransformCom->Get_Info(INFO_LOOK, &vPlayerLook);
			m_pTransformCom->Get_Info(INFO_RIGHT, &vPlayerRight);
			m_pHandedTransformCom->Set_Pos(vPlayerPos.x + vPlayerLook.x * 0.2f + vPlayerRight.x * 0.3f, 1.f, vPlayerPos.z + vPlayerLook.z * 0.2f + vPlayerRight.z * 0.3f);
		}
		m_pHandedItem->Set_Swing(m_eDir, true);
	}
}
void CPlayer::Shoot_Equipment()
{
	if (m_pHandedItem)
	{
		_vec3 vPlayerPos;
		m_pTransformCom->Get_Info(INFO_POS, &vPlayerPos);
		if (g_bIsTopCamera)
		{
			switch (m_eDir)
			{
			case FRONT:
				m_pHandedTransformCom->Set_Pos(vPlayerPos.x, vPlayerPos.y, vPlayerPos.z - 0.4f);
				break;
			case RIGHT:
				m_pHandedTransformCom->Set_Pos(vPlayerPos.x + 0.4f, vPlayerPos.y, vPlayerPos.z);
				break;
			case BACK:
				m_pHandedTransformCom->Set_Pos(vPlayerPos.x + 0.4f, vPlayerPos.y, vPlayerPos.z + 0.4f);
				break;
			case LEFT:
				m_pHandedTransformCom->Set_Pos(vPlayerPos.x - 0.4f, vPlayerPos.y, vPlayerPos.z);
				break;
			}
		}
		else
		{
			m_eDir = BACK;
			_vec3 vPlayerLook, vPlayerRight;
			m_pTransformCom->Get_Info(INFO_LOOK, &vPlayerLook);
			m_pTransformCom->Get_Info(INFO_RIGHT, &vPlayerRight);
			m_pHandedTransformCom->Set_Pos(vPlayerPos.x + vPlayerLook.x * 0.2f + vPlayerRight.x * 0.3f, 1.f, vPlayerPos.z + vPlayerLook.z * 0.2f + vPlayerRight.z * 0.3f);
		}
		if (!m_bShootOnce)
		{
			m_bShootOnce = true;
			if (g_bIsTopCamera)
			{
				_vec3 vDir = m_vMouseWorldPos - vPlayerPos;
				D3DXVec3Normalize(&vDir, &vDir);
				vDir.y = 0.f;
				m_pHandedItem->Set_ProjectileDir(vDir);
				m_pHandedItem->Set_Shoot(m_eDir, true);
			}
			else
			{
				_vec3 vLook;
				m_pTransformCom->Get_Info(INFO_LOOK, &vLook);
				vLook.y = 0.f;
				m_pHandedItem->Set_ProjectileDir(vLook);
				m_pHandedItem->Set_Shoot(m_eDir, true);
			}
		}
	}
}

void CPlayer::PickAxe()
{
	if (g_bIsTopCamera)
	{
		_vec3 vCheckPos, vLook, vRight;
		m_pTransformCom->Get_Info(INFO_POS, &vCheckPos);
		m_pTransformCom->Get_Info(INFO_LOOK, &vLook);
		m_pTransformCom->Get_Info(INFO_RIGHT, &vRight);

		switch (m_eDir)
		{
		case FRONT:
			vCheckPos -= vLook;
			break;
		case BACK:
			vCheckPos += vLook;
			break;
		case RIGHT:
			vCheckPos += vRight;
			break;
		case LEFT:
			vCheckPos += vRight;
			break;
		}
		_int iIndex = _int(vCheckPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + (vCheckPos.x + 0.5f * VTXITV);
		CTerrain* pTerrain = dynamic_cast<CTerrain*>(Engine::Get_GameObject(L"Layer_Environment", L"Terrain"));
		if (0 <= iIndex && iIndex < VTXCNTX * VTXCNTZ)
		{
			if (pTerrain->Get_UnreachableByIndex(iIndex))
			{
				CScene* pCurScene = Engine::Get_Scene();
				dynamic_cast<CStage*>(pCurScene)->Get_WallNameByIndex(iIndex);
				
				CWall* pWall = dynamic_cast<CWall*>(Get_GameObject(L"Layer_Environment", dynamic_cast<CStage*>(pCurScene)->Get_WallNameByIndex(iIndex)->c_str()));
				pWall->Set_Destroy();
				pCurScene->Delete_GameObject(L"Layer_Environment", pWall, dynamic_cast<CStage*>(pCurScene)->Get_WallNameByIndex(iIndex)->c_str());
				pTerrain->Set_Unreachable(iIndex, false);
			}
		}
	}
	else
	{
		_vec3 vCheckPos, vLook, vRight;
		m_pTransformCom->Get_Info(INFO_POS, &vCheckPos);
		m_pTransformCom->Get_Info(INFO_LOOK, &vLook);

		vCheckPos += vLook;
		
		_int iIndex = _int(vCheckPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + (vCheckPos.x + 0.5f * VTXITV);
		CTerrain* pTerrain = dynamic_cast<CTerrain*>(Engine::Get_GameObject(L"Layer_Environment", L"Terrain"));
		if (0 <= iIndex && iIndex < VTXCNTX * VTXCNTZ)
		{
			if (pTerrain->Get_UnreachableByIndex(iIndex))
			{
				CScene* pCurScene = Engine::Get_Scene();
				dynamic_cast<CStage*>(pCurScene)->Get_WallNameByIndex(iIndex);

				CGameObject* pWall = Get_GameObject(L"Layer_Environment", dynamic_cast<CStage*>(pCurScene)->Get_WallNameByIndex(iIndex)->c_str());
				dynamic_cast<CWall*>(pWall)->Set_Destroy();
				pCurScene->Delete_GameObject(L"Layer_Environment", pWall, dynamic_cast<CStage*>(pCurScene)->Get_WallNameByIndex(iIndex)->c_str());
				Engine::Delete_Renderer(RENDER_PRIORITY, pWall);
				pTerrain->Set_Unreachable(iIndex, false);
			}
		}
	}
}

void CPlayer::Hoe()
{
	if (g_bIsTopCamera)
	{
		_vec3 vPos;
		m_pTransformCom->Get_Info(INFO_POS, &vPos);
		// 플레이어와 마우스 커서 사이의 거리가 n 이하일 경우 커서에 타일 UI 뜨게 하기
		// 해당 위치에 설치할 수 있을 경우 파란색 타일, 없을 경우 빨간색 타일
		// 설치할 수 있는 타일일 때 클릭하면 타일 텍스쳐 넘버 변경
		// 설치할 수 없는 타일일 때 클릭하면 경고 문구 띄울까 말까 : 대사 시스템 때 삽입하면 될듯
		if (m_pCalculatorCom->Check_Distance2D(&vPos, &m_vMouseWorldPos, 5.f))
		{
			_int iIndex = _int(m_vMouseWorldPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + (m_vMouseWorldPos.x + 0.5f * VTXITV);
			switch (m_pHandedItem->Get_ItemMaterial())
			{
			case MATERIAL_WOOD: // 1 x 1
				m_pTerrain->Set_TextureNumber(iIndex, 4);
				break;
			case MATERIAL_COPPER: // 3 x 3
				m_pTerrain->Set_TextureNumber(iIndex, 4);
				break;
			case MATERIAL_IRON: // 5 x 5
				m_pTerrain->Set_TextureNumber(iIndex, 4);
				break;
			}
		}
	}
}

void CPlayer::Set_EquippedStatus()
{
	m_pStateCom->Set_BasicStat(&m_tBasicStat);
	m_pStateCom->Set_EquippedStat(&m_tEquipmentStat);
}

void CPlayer::Set_Clothes()
{
	m_bNude = false;
	m_pClothes[0] = dynamic_cast<CItem*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player_Eye"));
	m_pClothes[1] = dynamic_cast<CItem*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player_Hair"));
	m_pClothes[2] = dynamic_cast<CItem*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player_HairShade"));
	m_pClothes[3] = dynamic_cast<CItem*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player_Shirt"));
	m_pClothes[4] = dynamic_cast<CItem*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player_Pants"));
	m_pTerrain = dynamic_cast<CTerrain*>(Engine::Get_GameObject(L"Layer_Environment", L"Terrain"));
}
void CPlayer::Set_MouseWorldPos()
{
	//_vec3	vPos;
	//m_pTransformCom->Get_Info(INFO_POS, &vPos);
	//_matrix matView;
	//m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	//D3DXVec3TransformCoord(&vPos, &vPos, &matView);

	//POINT	ptMouse{};
	//GetCursorPos(&ptMouse);
	//ScreenToClient(g_hWnd, &ptMouse);

	//_vec3			vMousePos;

	//D3DVIEWPORT9	ViewPort;
	//ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
	//m_pGraphicDev->GetViewport(&ViewPort);

	//// 뷰 포트 -> 투영
	//vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	//vMousePos.y = ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;
	//vMousePos.z = 0.f;

	//_matrix matProj;
	//m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	//D3DXMatrixInverse(&matProj, NULL, &matProj);
	//D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	//m_vMouseWorldPos = vMousePos - vPos;
	//D3DXVec3Normalize(&m_vMouseWorldPos, &m_vMouseWorldPos);
	//m_vMouseWorldPos.z = m_vMouseWorldPos.y;
	//m_vMouseWorldPos.y = 0;

	m_vMouseWorldPos = *(m_pTerrain->Get_PickPos());
}

void CPlayer::Set_UI()
{
	CUIStatusBar* pHp = dynamic_cast<CUIStatusBar*>
		(Engine::Get_GameObject(L"Layer_UI", L"UI_Health"));
	NULL_CHECK_RETURN(pHp);

	pHp->Set_InfoH(m_pStateCom->Get_Stat()->iHp, m_pStateCom->Get_Stat()->iMaxHp); // (泥대젰 , 理쒕?泥대젰)

	CUIStatusBar* pMp = dynamic_cast<CUIStatusBar*>
		(Engine::Get_GameObject(L"Layer_UI", L"UI_Mp"));
	NULL_CHECK_RETURN(pMp);

	pMp->Set_InfoH(m_pStateCom->Get_Stat()->iMp, m_pStateCom->Get_Stat()->iMaxMp); // (留덈굹 , 理쒕?留덈굹)

	CUIStatusBar* pHunger = dynamic_cast<CUIStatusBar*>
		(Engine::Get_GameObject(L"Layer_UI", L"UI_Hunger"));
	NULL_CHECK_RETURN(pMp);

	pHunger->Set_InfoH(m_pStateCom->Get_Stat()->iMp, m_pStateCom->Get_Stat()->iMaxMp); // (諛곌퀬??, 理쒕?諛곌퀬??

	if (Engine::Get_DIMouseMove(DIMS_Z) && !m_bInventory)
	{
		if (Engine::Get_DIMouseMove(DIMS_Z) < 0)
			m_iHandNum++;
		else
			m_iHandNum--;


		if (m_iHandNum > 9)
		{
			m_iHandNum = 0;
		}
		else if (m_iHandNum < 0)
		{
			m_iHandNum = 9;
		}
	}

	if (Engine::Key_Down(DIK_M))
	{
		//Set_Map();

		CRenderer::GetInstance()->Expand_MiniMap(m_pGraphicDev);
	}
	if (Engine::Key_Down(DIK_TAB))
	{
		if (m_bMap || m_bChestInventory || m_bCraft || m_bInventory || m_bStatue || m_bGraveInventory)
			UI_Disable();

		else if (!m_bMap && !m_bChestInventory)
		{
			Set_Inventory();
			Set_Craft();
			Set_Status();
		}
	}
	
	if(m_bInventory && (Engine::Key_Down(DIK_E)))
	{
		UI_Disable();
	}

	if (Engine::Key_Down(DIK_O))
	{
		CUIBuff* pBuff = dynamic_cast<CUIBuff*>(Engine::Get_GameObject(L"Layer_UI", L"UI_Buff0")); 

		pBuff->Set_Window(CUIBuff::BUFF_HEAL, CUIBuff::BUFF, 200.f); 

		CUIBuff* pDeBuff = dynamic_cast<CUIBuff*>(Engine::Get_GameObject(L"Layer_UI", L"UI_DeBuff0"));

		pDeBuff->Set_Window(CUIBuff::DEBUFF_BURN, CUIBuff::DEBUFF, 100.f);
		
		//Set_Statue();
	}
}

void CPlayer::Set_InvWindow()
{
	if (m_bInventory)
		m_bInventory = false;
	else
		m_bInventory = true;
}

void CPlayer::Set_CraftWindow()
{
	if (m_bCraft)
		m_bCraft = false;
	else
		m_bCraft = true;
}

void CPlayer::Set_MapWindow()
{
	if (m_bMap)
		m_bMap = false;
	else
		m_bMap = true;
}

void CPlayer::Set_Craft(TABLETYPE eTableType, MATERIAL _eMaterial)
{
	if (m_bCraft)
	{
		CUICraft* pCraft = dynamic_cast<CUICraft*>(Engine::Get_GameObject(L"Layer_UI", L"UILeftCraft"));
		pCraft->Set_Window(eTableType, _eMaterial);

		CUICraft* pRightCraft = dynamic_cast<CUICraft*>(Engine::Get_GameObject(L"Layer_UI", L"UIRightCraft"));
		pRightCraft->Set_Disable();

		for (int i = 0; i < 6; i++)
		{
			wstring string;

			string = L"UICraftLSlot_" + std::to_wstring(i);

			CUICraftSlot* pSlot = dynamic_cast<CUICraftSlot*>(Engine::Get_GameObject(L"Layer_UI", string.c_str()));

			pSlot->Set_DisableWindow();
		}

		for (int i = 0; i < 6; i++)
		{
			wstring string;

			string = L"UICraftRSlot_" + std::to_wstring(i);

			CUICraftSlot* pSlot = dynamic_cast<CUICraftSlot*>(Engine::Get_GameObject(L"Layer_UI", string.c_str()));

			pSlot->Set_DisableWindow();
		}

		m_bCraft = false;
	}
	else
	{
		CUICraft* pCraft = dynamic_cast<CUICraft*>(Engine::Get_GameObject(L"Layer_UI", L"UILeftCraft"));
		pCraft->Set_Window(eTableType, _eMaterial);

		for (int i = 0; i < 6; i++)
		{
			wstring string;

			string = L"UICraftLSlot_" + std::to_wstring(i);

			CUICraftSlot* pSlot = dynamic_cast<CUICraftSlot*>(Engine::Get_GameObject(L"Layer_UI", string.c_str()));

			pSlot->Set_Window(eTableType, _eMaterial, true);
		}

		if (eTableType == TABLE_CRAFT || eTableType == TABLE_ANVIL)
		{
			CUICraft* pRightCraft = dynamic_cast<CUICraft*>(Engine::Get_GameObject(L"Layer_UI", L"UIRightCraft"));
			pRightCraft->Set_Window(eTableType, _eMaterial);

			for (int i = 0; i < 6; i++)
			{
				wstring string;

				string = L"UICraftRSlot_" + std::to_wstring(i);

				CUICraftSlot* pSlot = dynamic_cast<CUICraftSlot*>(Engine::Get_GameObject(L"Layer_UI", string.c_str()));

				pSlot->Set_Window(eTableType, _eMaterial, false);
			}
		}

		m_bCraft = true;
	}
}

void CPlayer::Set_Inventory()
{
	CUIScreenIcon* pIcon = dynamic_cast<CUIScreenIcon*>(Engine::Get_GameObject(L"Layer_UI", L"UIScreenicon_Hand"));
	pIcon->Set_Exit();

	CUIInvPlate* pPlate = dynamic_cast<CUIInvPlate*>(Engine::Get_GameObject(L"Layer_UI", L"UI_Plate"));

	pPlate->Set_Render();

	for (int i = 0; i < 10; i++)
	{
		wstring string;

		string = L"UI_ScreenInv_" + std::to_wstring(i);

		CUIScreenInv* pInv = dynamic_cast<CUIScreenInv*>(Engine::Get_GameObject(L"Layer_UI", string.c_str()));

		pInv->Move_Pos();
	}

	CInventory* pPlayer = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"Player", L"Com_Inventory"));

	for (int i = 10; i < pPlayer->Get_SlotCount(); i++)
	{	
		wstring string;

		string = L"UI_Inventory_" + std::to_wstring(i);

		CUIInventory* pInventory = dynamic_cast<CUIInventory*>(Engine::Get_GameObject(L"Layer_UI", string.c_str()));

		pInventory->Set_Show();
	}

	CUITrashCan* pCan = dynamic_cast<CUITrashCan*>(Engine::Get_GameObject(L"Layer_UI", L"UI_TrashCan"));
	pCan->Set_Window();

	CUITrashSlot* pCanSlot = dynamic_cast<CUITrashSlot*>(Engine::Get_GameObject(L"Layer_UI", L"UI_TrashSlot"));
	pCanSlot->Set_Window();

	CUISort* pSort = dynamic_cast<CUISort*>(Engine::Get_GameObject(L"Layer_UI", L"UI_Sort"));
	pSort->Set_Window();
	
	if (m_bInventory)
		m_bInventory = false;
	else
		m_bInventory = true;
}

void CPlayer::Set_Map()
{
	if (m_bMap)
		m_bMap = false;
	else
		m_bMap = true;
}

void CPlayer::Set_Status()
{

	CUIPlayerStatus* pStatus = dynamic_cast<CUIPlayerStatus*>(Engine::Get_GameObject(L"Layer_UI", L"UIPlayerStatus"));
	pStatus->Set_Window();

	CUIPlayerStats* pStats = dynamic_cast<CUIPlayerStats*>(Engine::Get_GameObject(L"Layer_UI", L"UIPlayerStats"));
	pStats->Set_Window();

	for (int i = 0; i < 10; i++)
	{
		wstring string;

		string = L"UIItemSlot_" + std::to_wstring(i);

		CUIItemSlot* pSlot = dynamic_cast<CUIItemSlot*>(Engine::Get_GameObject(L"Layer_UI", string.c_str()));

		pSlot->Set_Window();
	}

	if (m_bStatus)
		m_bStatus = false;
	else
		m_bStatus = true;
}

void CPlayer::Set_ChestInventory(CInventory* pInventory)
{
	
	for (int i = 0; i < 18; ++i)
	{
		wstring string;

		string = L"UI_ChestInventory_" + std::to_wstring(i);

		CUIChestInv* pChestInventory = dynamic_cast<CUIChestInv*>(Engine::Get_GameObject(L"Layer_UI", string.c_str()));

		pChestInventory->Set_Show(pInventory);
	}

	CUISort* pSort = dynamic_cast<CUISort*>(Engine::Get_GameObject(L"Layer_UI", L"UI_ChestSort"));

	pSort->Set_Window();
	pSort->Set_Inventory(pInventory);

	CUIChestSort* pChestSort = dynamic_cast<CUIChestSort*>(Engine::Get_GameObject(L"Layer_UI", L"UI_ChestAddItem"));
	pChestSort->Set_Window(pInventory);

	Set_Inventory();

	if (m_bChestInventory)
		m_bChestInventory = false;
	else
		m_bChestInventory = true;
}

void CPlayer::Set_GraveInventory()
{
	if (m_bGraveInventory)
	{
		CInventory* pGraveInventory = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_Environment", L"AheadGrave", L"Com_Inventory"));

		for (int i = 0; i < 30; ++i)
		{
			wstring string;

			string = L"UI_GraveInventory_" + std::to_wstring(i);

			CUIChestInv* pChestInventory = dynamic_cast<CUIChestInv*>(Engine::Get_GameObject(L"Layer_UI", string.c_str()));

			pChestInventory->Set_Disable();
		}

		if(m_bInventory)
			Set_Inventory();

		m_bGraveInventory = false;
	}
	else
	{
		CInventory* pGraveInventory = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_Environment", L"AheadGrave", L"Com_Inventory"));

		for (int i = 0; i < 30; ++i)
		{
			wstring string;

			string = L"UI_GraveInventory_" + std::to_wstring(i);

			CUIChestInv* pChestInventory = dynamic_cast<CUIChestInv*>(Engine::Get_GameObject(L"Layer_UI", string.c_str()));

			pChestInventory->Set_Show(pGraveInventory);
		}

		Set_Inventory();

		m_bGraveInventory = true;
	}
}

void CPlayer::Set_Statue(_int _StatueNum)
{
	CUIStatue* pStatue = dynamic_cast<CUIStatue*>(Engine::Get_GameObject(L"Layer_UI", L"UI_Statue"));

	pStatue->Set_Window(_StatueNum);

	CUIJemSlot* pSlot = dynamic_cast<CUIJemSlot*>(Engine::Get_GameObject(L"Layer_UI", L"UI_JemSlot"));

	pSlot->Set_Window(_StatueNum);

	CUIStatueCraft* pSCraft = dynamic_cast<CUIStatueCraft*>(Engine::Get_GameObject(L"Layer_UI", L"UI_StatueCraft"));

	pSCraft->Set_Window();

	Set_Inventory();

	if (m_bStatue)
		m_bStatue = false;
	else
		m_bStatue = true;
}

void CPlayer::Set_Furnace()
{
	if (m_bFurnace)
	{
		CUIFurnace* pFurnace = dynamic_cast<CUIFurnace*>(Engine::Get_GameObject(L"Layer_UI", L"UI_Furnace"));

		pFurnace->Set_Diable();

		if (m_bInventory)
		{
			Set_Inventory();
		}

		m_bFurnace = false;
	}
	else
	{
		CUIFurnace* pFurnace = dynamic_cast<CUIFurnace*>(Engine::Get_GameObject(L"Layer_UI", L"UI_Furnace"));

		pFurnace->Set_Window();

		Set_Inventory();

		m_bFurnace = true;
	}
}

void CPlayer::UI_Disable()
{
	if (m_bInventory)
	{
		Set_Inventory();

		m_bInventory = false;
	}

	if (m_bCraft)
	{
		Set_Craft();

		m_bCraft = false;
	}
	if (m_bStatus)
	{
		Set_Status();
	}

	if (m_bMap)
	{
		Set_Map();

		m_bMap = false;
	}

	if (m_bStatue)
	{
		Set_Statue();

		m_bStatue = false;
	}

	if (m_bChestInventory)
	{
		Set_ChestInventory();
	}

	if (m_bGraveInventory)
	{
		Set_GraveInventory();
	}

	if (m_bFurnace)
	{
		Set_Furnace();
	}
}

void CPlayer::Particle_Update(_float fTimeDelta)
{
	if (m_bBleed)
	{
		m_pFireParticleCom->update(fTimeDelta); // 파티클 업데이트

		m_fBleedTime += fTimeDelta;
	}

	if (m_bBleed && m_pFireParticleCom->isDead())
	{
		m_fBleedTime = 0.f;
		m_bBleed = false;

		m_pFireParticleCom->reset();
	}

	m_pFollowParticleCom->update(fTimeDelta);
}

void CPlayer::Set_KnockBack(_vec3 vEnemyPos, _int iDamage, _float fDist)
{
	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	m_bKnockBackStart = true;
	m_bKnockBackEnd = false;
	m_vKnockBackDir = vPos - vEnemyPos;
	m_vKnockBackDir.y = 0;
	D3DXVec3Normalize(&m_vKnockBackDir, &m_vKnockBackDir);
	m_vStartPoint = vEnemyPos;
	m_fKnockBackDist = fDist;

	m_pStateCom->Set_Damaged(iDamage);
}

void CPlayer::KnockBack(const _float& fTimeDelta)
{
	if (m_bKnockBackStart)
	{
		_vec3 vPos;
		m_pTransformCom->Get_Info(INFO_POS, &vPos);

		_vec3 vLength = m_vStartPoint - vPos;
		_float fLength = D3DXVec3Length(&vLength);
		if (fLength >= m_fKnockBackDist || 0 == m_iSpeedWeight)
		{
			m_bKnockBackStart = false;
			m_bKnockBackEnd = true;
			if (m_eState != DEAD)
				m_eState = IDLE;
			return;
		}

		Set_Stop(&m_vKnockBackDir, m_fSpeed * (m_fKnockBackDist / fLength));

		m_pTransformCom->Move_Pos(&m_vKnockBackDir, fTimeDelta, m_fSpeed * (m_fKnockBackDist / fLength) * m_iSpeedWeight);

		m_bBleed = true;
	}
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
