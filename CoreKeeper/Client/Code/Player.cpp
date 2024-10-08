#include "pch.h"
#include "..\Header\Player.h"

#include "Export_Utility.h"

#include "..\Header\PlayerInclude.h"
#include "..\Header\DynamicCamera.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_fLightRange(0.f)
{
	m_eDir = DIRECTION_END;
	m_eState = STATE_END;
	m_fSpeed = 5.f;
	m_fDiagSpeed = sqrt(pow(m_fSpeed, 2) / 2);
	m_fNormalSpeed = m_fSpeed;

	m_bSwing = false;
	m_bShoot = false;
	m_fClickTime = 0.f;

	m_fFirstY = 0.8f;
	m_fTimeAcc = 0.f;
	m_fWalkYSpeed = 1.8f;
	m_iSpeedWeight = 1;

	m_pHandedItem = nullptr;
	m_pHandedTransformCom = nullptr;
	m_iHandNum = 0;
	ZeroMemory(&m_tBasicStat, sizeof(STAT));
	ZeroMemory(&m_tEquipmentStat, sizeof(STAT));
	ZeroMemory(&m_tBuffStat, sizeof(STAT));

	for (_int i = 0; i < BUFFTYPE_END; i++)
		m_arrBuffState[i] = false;

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
	m_bCookingPot = false;

	m_vStartPoint = { 0, 0, 0 };
	m_vKnockBackDir = { 0, 0, 0 };
	m_bKnockBackStart = false;
	m_bKnockBackEnd = true;
	m_fKnockBackDist = 0.f;
	m_bNude = true;

	m_bRespawned = false;
	m_vRespawnPoint = { VTXCNTX / 2, 0, 12.f };
	m_bRespawnFirstFrame = true;
	m_fRespawnProgress = 0.f;

	m_bBleed = false;
	m_fBleedTime = 0.f;
	m_bFire = false;
	m_fFireTickTime = 0.f;

	m_bShootOnce = false;
	m_vMouseWorldPos = { 0, 0, 0 };

	m_bDash = false;
	m_fDashTime = 0.2f;
	m_fDashTimeAcc = 0.f;
	m_bDashCool = false;
	m_fDashCoolTime = 0.5f;

	m_bImmune = false;
	m_fImmuneTimeAcc = 0.f;
	m_bImmuneByTime = false;

	m_vecInstallObjectName.reserve(16);
	m_iInstallNumber = 0;
	m_iLightNum = g_iLightNum++;

	m_fHungerTime = 0.f;
	m_fManaTime = 0.f;
	m_fMiningBuff[0] = 0.f;
	m_fMiningBuff[1] = 0.f;

	m_bDestroyWall = false;
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_tBasicStat = STAT(400, 100, 20, 0);
	m_pStateCom->Set_Stat(m_tBasicStat.iMaxHp, m_tBasicStat.iMaxMp, m_tBasicStat.iAttack, m_tBasicStat.iDefense);
	m_pStateCom->Set_MaxHunger(100);
	m_pEquipInventoryCom->Set_SlotCount(10);

	m_pFireParticleCom->init(L"../Bin/Resource/Texture/Particle/Basic_Particle.png", 1, 0.1f); // 파티클 시작
	m_pFollowParticleCom->init(L"../Bin/Resource/Texture/Particle/Fire_Particle/Fire_Particle_%d.png", 5); // 파티클 시작
	m_pDirtParticleCom->init(L"../Bin/Resource/Texture/Particle/Basic_Particle.png", 1, 0.1f);
	return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
	if (m_pStateCom->Get_Dead())
	{
		Set_UI();
		Respawn_Progress(fTimeDelta);
		return 0;
	}
	// 랜턴
	SetUp_Light();

	if (m_bNude)
		Set_Clothes();
	Set_MouseWorldPos();

	KnockBack(fTimeDelta);

	Set_Equipment();
	Show_Equipment();
	Equipment_Function(fTimeDelta);
	Set_EquippedStatus();
	Set_ManaRecover(fTimeDelta);
	// 버프 스탯 처리
	Set_Hungry(fTimeDelta);
	Set_Buff(fTimeDelta);


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
	Set_ImmuneByToggle();

	// 시간제 무적용
	if (m_bImmuneByTime)
	{
		m_fImmuneTimeAcc += fTimeDelta;
		if (m_fImmuneTimeAcc >= m_fImmuneTime)
		{
			m_bImmuneByTime = false;
			m_fImmuneTimeAcc = 0.f;
		}
	}

	if (!m_bRespawned)
	{
		m_bRespawned = true;
		m_pTransformCom->Set_Pos(m_vRespawnPoint.x, m_fFirstY, m_vRespawnPoint.z);
	}

	m_pAnimatorCom->Update_Animation();
	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	Set_UI();
	Particle_Update(fTimeDelta);
	Engine::Add_RenderGroup(RENDER_ALPHA, this);
	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CPlayer::LateUpdate_GameObject()
{
	Set_WallProjection();
	Engine::CGameObject::LateUpdate_GameObject();
}

void CPlayer::Render_GameObject()
{
	if (m_pStateCom->Get_Dead())
		return;

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	FAILED_CHECK_RETURN(Setup_Material(), );

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());
	m_pBufferCom->Render_Buffer();
	m_pColliderCom->Render_Collider();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	if (m_bBleed)
		m_pFireParticleCom->render(); // 파티클 렌더

	if (m_bFire)
		m_pFollowParticleCom->render();

	if (m_bDestroyWall)
	{
		m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_bPickaxeMatrix);
		m_pDirtParticleCom->render();
	}
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

	pComponent = m_pDirtParticleCom = dynamic_cast<CFall*>(Engine::Clone_Proto(L"Proto_DirtFall"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_DirtFall", pComponent });
	///m_pFireParticleCom
	return S_OK;
}

HRESULT CPlayer::Setup_Material()
{
	D3DMATERIAL9		tMtrl;
	ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

	tMtrl.Diffuse = { 1.f, 1.f, 1.f, 1.f };
	tMtrl.Specular = { 1.f, 1.f, 1.f, 1.f };
	tMtrl.Ambient = { 0.9f, 0.9f, 0.9f, 0.9f };

	tMtrl.Emissive = { 0.01f, 0.01f, 0.01f, 0.01f };
	tMtrl.Power = 0.f;

	m_pGraphicDev->SetMaterial(&tMtrl);

	return S_OK;
}

void CPlayer::SetUp_Light()
{
	//D3DLIGHT9 light;
	//ZeroMemory(&light, sizeof(D3DLIGHT9));

	//light.Type = D3DLIGHT_POINT; // 포인트 조명
	//light.Diffuse = { 1.f, 1.f, 1.f, 1.f }; // 확산 색상
	//light.Specular = { 1.f, 1.f, 1.f, 1.f }; // 반사 색상
	//light.Ambient = { 1.f, 1.f, 1.f, 1.f }; // 주변광

	//_vec3 vPos;
	//m_pTransformCom->Get_Info(INFO_POS, &vPos);

	//light.Position = vPos; // 횃불의 위치
	//light.Range = m_fLightRange; // 조명의 범위
	//light.Falloff = 1.f; // 감쇠
	//light.Attenuation0 = 1.0f; // 감쇠 계수
	//light.Attenuation1 = 0.01f;
	//light.Attenuation2 = 0.0f;

	//m_pGraphicDev->SetLight(m_iLightNum, &light); // 조명 설정
	//m_pGraphicDev->LightEnable(m_iLightNum, TRUE); // 조명 활성화

	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(D3DLIGHT9));

	light.Type = D3DLIGHT_SPOT;
	light.Diffuse = { 1.f, 1.f, 1.f, 1.f }; // 확산 색상
	light.Specular = { 1.f, 1.f, 1.f, 1.f }; // 반사 색상
	light.Ambient = { 1.f, 1.f, 1.f, 1.f }; // 주변광

	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	light.Position = { vPos.x, vPos.y + 10.f, vPos.z };

	// 아래쪽을 향하는 스포트라이트
	_vec3 vDir = { 0.0f, -1.0f, 0.0f };
	light.Direction = vDir;

	light.Range = m_fLightRange * 10.f; // 조명의 범위
	light.Falloff = 1.0f; // 감쇠
	light.Attenuation0 = 1.0f; // 감쇠 계수
	light.Attenuation1 = 0.01f;
	light.Attenuation2 = 0.0f;

	// 스포트라이트의 내부 및 외부 각도 설정
	light.Theta = D3DXToRadian(20.0f); // 내부 각도 (작은 값일수록 집중된 조명)
	light.Phi = D3DXToRadian(80.0f); // 외부 각도 (큰 값일수록 퍼지는 조명)

	m_pGraphicDev->SetLight(m_iLightNum, &light);
	m_pGraphicDev->LightEnable(m_iLightNum, TRUE);
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
		if (Engine::Button_Down(DIM_LB))
		{
			if (m_pHandedItem)
			{
				ITEMNUM eHandedNum = m_pHandedItem->Get_ItemNum();
				switch (eHandedNum)
				{
					// 전투 관련
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
				case ITEM_BOW:
					m_eState = SHOOT;
					m_bShoot = true;
					Shoot_Equipment();
					break;
				case ITEM_STAFF:
					if (m_pStateCom->Get_Stat()->iMp >= 20)
					{
						m_eState = SHOOT;
						m_bShoot = true;
						Shoot_Equipment();
						m_pStateCom->Set_UseMP(20);
					}
					break;

					// 농사 관련
				case ITEM_HOE:
					m_eState = SWING;
					m_bSwing = true;
					Swing_Equipment();
					Hoe();
					break;
				case ITEM_WATERINGCAN:
					Watering();
					break;
				case ITEM_BERRY_SEED:
				case ITEM_PEPPER_SEED:
				case ITEM_CARROT_SEED:
				case ITEM_FIBER_SEED:
					Plant(eHandedNum);
					m_pHandedItem->Set_Use(false);
					m_pHandedItem->Set_Active(false);
					m_pHandedItem->Set_Drop(false);
					break;

					// 설치 관련
				case ITEM_TABLE:
				case ITEM_POTION_TABLE:
				case ITEM_ACCESSORY_TABLE:
				case ITEM_MUSIC_TABLE:
				case ITEM_ANVIL:
				case ITEM_FURNACE:
				case ITEM_COOKINGPOT:
				case ITEM_TORCH:
				case ITEM_BOX:
				case ITEM_GRAVESTONE:
				case ITEM_SPRINKLER:
					Install(eHandedNum);
					break;

				case ITEM_BERRY:
				case ITEM_PEPPER:
				case ITEM_CARROT:
				case ITEM_MUSHROOM:
				case ITEM_BERRY_BERRY_FOOD:
				case ITEM_BERRY_PEPPER_FOOD:
				case ITEM_BERRY_CARROT_FOOD:
				case ITEM_BERRY_MUSHROOM_FOOD:
				case ITEM_PEPPER_PEPPER_FOOD:
				case ITEM_PEPPER_CARROT_FOOD:
				case ITEM_PEPPER_MUSHROOM_FOOD:
				case ITEM_CARROT_CARROT_FOOD:
				case ITEM_CARROT_MUSHROOM_FOOD:
				case ITEM_MUSHROOM_MUSHROOM_FOOD:
				case ITEM_LUNCH:
				case ITEM_CHOCOBAR:
				case ITEM_POTION_HP:
				case ITEM_POTION_ATT:
				case ITEM_POTION_DEF:
					Eat(eHandedNum);
					break;

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

	m_pTransformCom->Move_Pos(&vUp, fTimeDelta, m_fWalkYSpeed);
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

void CPlayer::Equipment_Function(const _float& fTimeDelta)
{
	Auxiliary(fTimeDelta);
	Lantern();
	Bag();
	Necklace();
	Ring();
	Ring_Second();
}

void CPlayer::Auxiliary(const _float& fTimeDelta)
{
	CItem* pAux = m_pEtcItems[CUIItemSlot::SLOT_WEAPON]->Get_Item();
	if (!pAux)
		return;

	ASSISTANCE assistance = dynamic_cast<CAssistance*>(pAux)->Get_Assistance();

	// 깃털 장착
	if (assistance == ASSISTANCE_FEATHER || assistance == ASSISTANCE_AZEOS_FEATHER)
	{
		// 스페이스바를 누르면 대쉬
		if (Engine::Key_Down(DIK_SPACE))
		{
			if (!m_bDash && !m_bDashCool)
				m_bDash = true;
		}

		// 대쉬하는 동안 플레이어 콜라이더 끄기
		if (m_bDash)
		{
			m_fDashTimeAcc += fTimeDelta;
			m_pColliderCom->Set_Offset(_vec3(0, -100, 0));
			// 대쉬 중 스피드 조절
			_float fProgress = m_fDashTimeAcc / m_fDashTime;
			if (assistance == ASSISTANCE_FEATHER)
			{
				if (fProgress < 0.5f)
					m_iSpeedWeight += 1;
				else
					m_iSpeedWeight -= 1;
			}
			else
			{
				if (fProgress < 0.5f)
					m_iSpeedWeight += 2;
				else
					m_iSpeedWeight -= 2;
			}

			if (fProgress >= 1)
			{
				m_bDashCool = true;
				m_bDash = false;
				m_iSpeedWeight = 1;
				m_fDashTimeAcc = 0.f;
			}
		}
		else
			m_pColliderCom->Set_Offset(_vec3(0, 0, 0));

		// 대쉬 쿨타임 설정
		if (m_bDashCool)
		{
			m_fDashTimeAcc += fTimeDelta;
			if (m_fDashTimeAcc >= m_fDashCoolTime)
			{
				m_bDashCool = false;
				m_fDashTimeAcc = 0.f;
			}
		}
	}
	// 책 장착
	else if (assistance == ASSISTANCE_BOOK)
		m_tEquipmentStat.iAttack += 50;
	// 방패 장착
	else if (assistance == ASSISTANCE_WOOD_SHIELD)
		m_tEquipmentStat.iDefense += 10;
	else if (assistance == ASSISTANCE_IRON_SHIELD)
	{
		m_tEquipmentStat.iDefense += 20;
		m_tEquipmentStat.iMaxHp += 20;
	}

}

void CPlayer::Lantern()
{
	CItem* pLantern = m_pEtcItems[CUIItemSlot::SLOT_LANTERN]->Get_Item();
	if (!pLantern)
	{
		m_fLightRange = 0.f;
		return;
	}

	if (pLantern->Get_ItemNum() == ITEM_LANTERN)
	{
		switch (pLantern->Get_ItemMaterial())
		{
		case MATERIAL_WOOD:
			m_fLightRange = 3.f;
			break;
		case MATERIAL_COPPER:
			m_fLightRange = 5.f;
			break;
		case MATERIAL_IRON:
			m_fLightRange = 7.f;
			break;
		}
	}
}

void CPlayer::Bag()
{
	CItem* pBag = m_pEtcItems[CUIItemSlot::SLOT_BAG]->Get_Item();
	if (!pBag)
	{
		m_pInventoryCom->Set_SlotCount(30);
		return;
	}

	if (pBag->Get_ItemNum() == ITEM_BAG)
	{
		Reset_Inventory();
		switch (pBag->Get_ItemMaterial())
		{
		case MATERIAL_COPPER:
			m_pInventoryCom->Set_SlotCount(35);
			break;
		case MATERIAL_IRON:
			m_pInventoryCom->Set_SlotCount(40);
			break;
		}
		Reset_Inventory();
	}
}

void CPlayer::Necklace()
{
	CItem* pNeck = m_pEtcItems[CUIItemSlot::SLOT_NECKLACE]->Get_Item();
	if (!pNeck)
		return;

	switch (pNeck->Get_ItemMaterial())
	{
	case MATERIAL_COPPER:
		// 치명타 확률 + 4%
		m_tEquipmentStat.iAttack += 25;
		break;
	case MATERIAL_IRON:
		// 방어력 + 12
		m_tEquipmentStat.iDefense += 20;
		break;
	case MATERIAL_SPECIAL:
		// 최대 마나 + 32
		m_tEquipmentStat.iMaxMp += 30;
		break;
	}
}

void CPlayer::Ring()
{
	CItem* pRing = m_pEtcItems[CUIItemSlot::SLOT_RING1]->Get_Item();
	m_fMiningBuff[0] = 0.f;
	if (!pRing)
		return;

	switch (pRing->Get_ItemMaterial())
	{
	case MATERIAL_COPPER:
		// 채굴 피해 + 14
		m_fMiningBuff[0] = 1.f;
		break;
	case MATERIAL_IRON:
		// 이동 속도 + 6.9%
		if (m_iSpeedWeight == 1)
			m_iSpeedWeight = 1.4f;
		break;
	case MATERIAL_SPECIAL:
		// 최대 마나 + 14
		m_fMiningBuff[0] = 0.f;
		m_tEquipmentStat.iMaxMp += 20;
		break;
	}
}

void CPlayer::Ring_Second()
{
	CItem* pRing = m_pEtcItems[CUIItemSlot::SLOT_RING2]->Get_Item();
	if (!pRing)
		return;

	switch (pRing->Get_ItemMaterial())
	{
	case MATERIAL_COPPER:
		if (m_fMiningBuff[0] == 1.f)
			m_fMiningBuff[0] = 2.f;
		else
			m_fMiningBuff[0] = 1.f;
		// 채굴 피해 + 14
		break;
	case MATERIAL_IRON:
		// 이동 속도 + 6.9%
		if (m_iSpeedWeight == 1.4f)
			m_iSpeedWeight = 1.8f;
		else
			m_iSpeedWeight = 1.4f;
		break;
	case MATERIAL_SPECIAL:
		// 최대 마나 + 14
		m_tEquipmentStat.iMaxMp += 20;
		break;
	}
}

void CPlayer::Set_ImmuneByTime(_float fImmuneTime)
{
	if (m_bImmune || m_bImmuneByTime)
		return;
	m_bImmuneByTime = true;
	m_fImmuneTime = fImmuneTime;
}

void CPlayer::Set_ImmuneByToggle()
{
	if (Engine::Key_Down(DIK_F1))
	{
		m_bImmune = m_bImmune ? false : true;
		if (m_bImmune)
			CBuffMgr::GetInstance()->Set_BuffStart(BUFF_IMMUNE, 999);
		else
			CBuffMgr::GetInstance()->Set_BuffEnd(BUFF_IMMUNE);
	}

	if (Engine::Key_Down(DIK_F2))
		dynamic_cast<CDynamicCamera*>(Engine::Get_GameObject(L"Layer_Environment", L"DynamicCamera"))->Set_ShakeInfo(3.f, 5.f);
	if (Engine::Key_Down(DIK_F3))
		CBuffMgr::GetInstance()->Set_BuffStart(DEBUFF_FIRE, 5.f);
	if (Engine::Key_Down(DIK_F4))
		CBuffMgr::GetInstance()->Set_BuffStart(DEBUFF_SLOW, 5.f);
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

	_vec3 vCheckPos{};
	m_pTransformCom->Get_Info(INFO_POS, &vCheckPos);

	// 미래의 캐릭터 중점 좌표
	vCheckPos += *vDir1 * fDirSpeed1 * 0.1f;
	if (vDir2)
		vCheckPos += *vDir2 * fDirSpeed2 * 0.1f;

	// 미래 중점 좌표 기준 인덱스 값
	_int iIndex = _int(vCheckPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + (vCheckPos.x + 0.5f * VTXITV);
	CTerrain* pTerrain = dynamic_cast<CTerrain*>(Engine::Get_GameObject(L"Layer_Environment", L"Terrain"));
	if (0 <= iIndex && iIndex < (VTXCNTX - 1) * (VTXCNTZ - 1))
		if (pTerrain->Get_UnreachableByIndex(iIndex))
			m_iSpeedWeight = 0;
		else if (!m_bDash)
			m_iSpeedWeight = 1;
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
		if (m_eState != SWING)
		{
			m_pHandedTransformCom->Set_Angle(0, 0, 0);
			_vec3 vPlayerPos;
			m_pTransformCom->Get_Info(INFO_POS, &vPlayerPos);

			if (g_bIsTopCamera)
			{
				if (m_pHandedItem->Get_ItemNum() == ITEM_BOW)
				{
					if (m_eState == SHOOT)
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
						switch (m_eDir)
						{
						case FRONT:
							m_pHandedTransformCom->Set_Pos(vPlayerPos.x - 0.3f, vPlayerPos.y, vPlayerPos.z - 0.2f);
							break;
						case RIGHT:
							m_pHandedTransformCom->Set_Pos(vPlayerPos.x - 0.2f, vPlayerPos.y, vPlayerPos.z - 0.2f);
							break;
						case BACK:
							m_pHandedTransformCom->Set_Pos(vPlayerPos.x + 0.3f, vPlayerPos.y, vPlayerPos.z + 0.2f);
							break;
						case LEFT:
							m_pHandedTransformCom->Set_Pos(vPlayerPos.x + 0.2f, vPlayerPos.y, vPlayerPos.z - 0.2f);
							break;
						}
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
		if (!g_bIsTopCamera)
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
	MATERIAL eAxeMaterial = m_pHandedItem->Get_ItemMaterial();

	if (g_bIsTopCamera)
	{
		_vec3 vCheckPos, vLook, vRight;
		m_pTransformCom->Get_Info(INFO_POS, &vCheckPos);
		m_pTransformCom->Get_Info(INFO_LOOK, &vLook);
		m_pTransformCom->Get_Info(INFO_RIGHT, &vRight);

		switch (m_eDir)
		{
		case FRONT:
			vCheckPos -= vLook * 1.2f;
			break;
		case BACK:
			vCheckPos += vLook * 1.2f;
			break;
		case RIGHT:
			vCheckPos += vRight * 1.2f;
			break;
		case LEFT:
			vCheckPos += vRight * 1.2f;
			break;
		}
		_int iIndex = _int(vCheckPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + (vCheckPos.x + 0.5f * VTXITV);
		CTerrain* pTerrain = dynamic_cast<CTerrain*>(Engine::Get_GameObject(L"Layer_Environment", L"Terrain"));
		if (0 <= iIndex && iIndex < (VTXCNTX - 1) * (VTXCNTZ - 1))
		{
			if (pTerrain->Get_UnreachableByIndex(iIndex))
			{
				CScene* pCurScene = Engine::Get_Scene();
				dynamic_cast<CStage*>(pCurScene)->Get_WallNameByIndex(iIndex);

				CWall* pWall = dynamic_cast<CWall*>(Engine::Get_GameObject(L"Layer_Environment", dynamic_cast<CStage*>(pCurScene)->Get_WallNameByIndex(iIndex)->c_str()));
				if (pWall)
				{
					_bool bIsBreakable = false;
					switch (eAxeMaterial)
					{
					case MATERIAL_WOOD:
						if (pWall->Get_WallNumber() < 15)
							bIsBreakable = true;
						break;
					case MATERIAL_COPPER:
						if (pWall->Get_WallNumber() < 30)
							bIsBreakable = true;
						break;
					case MATERIAL_IRON:
						if (pWall->Get_WallNumber() < 45)
							bIsBreakable = true;
						break;
					}
					if (bIsBreakable)
						pWall->Set_DurabiliryMinus((eAxeMaterial + 1) * (m_fMiningBuff[0] + m_fMiningBuff[1]));
					if (pWall->Get_Durability() <= 0)
					{
						/*
						m_pHandedItem = m_pInventoryCom->Get_HandedItem(m_iHandNum);

						_vec3 vPos = dynamic_cast<CPickaxe*>(m_pHandedItem)->Get_PickaxePos();
						*/
						m_pTransformCom->Get_WorldMatrix(&m_bPickaxeMatrix);

						_vec3 vAxePos, vAxeRight;
						m_pTransformCom->Get_Info(INFO_POS, &vAxePos);
						m_pTransformCom->Get_Info(INFO_RIGHT, &vAxeRight);
						switch (m_eDir)
						{
						case FRONT:
							vAxePos -= vLook * 1.4f;
							break;
						case BACK:
							vAxePos += vLook * 1.4f;
							break;
						case RIGHT:
							vAxePos += vAxeRight * 1.4f;
							break;
						case LEFT:
							vAxePos += vAxeRight * 1.4f;
							break;
						}
						m_bPickaxeMatrix._41 = vAxePos.x;
						//m_bPickaxeMatrix._42 = vAxePos.y;
						m_bPickaxeMatrix._43 = vAxePos.z;

						m_pDirtParticleCom->reset();
						pWall->Drop_Item();
						pWall->Set_Destroy();
						m_bDestroyWall = true;
						pCurScene->Delete_GameObject(L"Layer_Environment", pWall, dynamic_cast<CStage*>(pCurScene)->Get_WallNameByIndex(iIndex)->c_str());
						pTerrain->Set_Unreachable(iIndex, false);

					}
				}
			}
		}
	}
	else
	{
		_vec3 vCheckPos, vLook, vRight;
		m_pTransformCom->Get_Info(INFO_POS, &vCheckPos);
		m_pTransformCom->Get_Info(INFO_LOOK, &vLook);

		vCheckPos += vLook * 1.2f;

		_int iIndex = _int(vCheckPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + (vCheckPos.x + 0.5f * VTXITV);
		CTerrain* pTerrain = dynamic_cast<CTerrain*>(Engine::Get_GameObject(L"Layer_Environment", L"Terrain"));
		if (0 <= iIndex && iIndex < (VTXCNTX - 1) * (VTXCNTZ - 1))
		{
			if (pTerrain->Get_UnreachableByIndex(iIndex))
			{
				CScene* pCurScene = Engine::Get_Scene();
				dynamic_cast<CStage*>(pCurScene)->Get_WallNameByIndex(iIndex);

				CWall* pWall = dynamic_cast<CWall*>(Engine::Get_GameObject(L"Layer_Environment", dynamic_cast<CStage*>(pCurScene)->Get_WallNameByIndex(iIndex)->c_str()));
				if (pWall)
				{
					_bool bIsBreakable = false;
					switch (eAxeMaterial)
					{
					case MATERIAL_WOOD:
						if (pWall->Get_WallNumber() < 15)
							bIsBreakable = true;
						break;
					case MATERIAL_COPPER:
						if (pWall->Get_WallNumber() < 30)
							bIsBreakable = true;
						break;
					case MATERIAL_IRON:
						if (pWall->Get_WallNumber() < 45)
							bIsBreakable = true;
						break;
					}
					if (bIsBreakable)
						pWall->Set_DurabiliryMinus((eAxeMaterial + 1) * (m_fMiningBuff[0] + m_fMiningBuff[1]));
					if (pWall->Get_Durability() <= 0)
					{
						m_pTransformCom->Get_WorldMatrix(&m_bPickaxeMatrix);

						_vec3 vAxePos, vAxeRight;
						m_pTransformCom->Get_Info(INFO_POS, &vAxePos);
						m_pTransformCom->Get_Info(INFO_RIGHT, &vAxeRight);
						switch (m_eDir)
						{
						case FRONT:
							vAxePos -= vLook * 1.4f;
							break;
						case BACK:
							vAxePos += vLook * 1.4f;
							break;
						case RIGHT:
							vAxePos += vAxeRight * 1.4f;
							break;
						case LEFT:
							vAxePos += vAxeRight * 1.4f;
							break;
						}
						m_bPickaxeMatrix._41 = vAxePos.x;
						m_bPickaxeMatrix._42 = vAxePos.y;
						m_bPickaxeMatrix._43 = vAxePos.z;

						m_pDirtParticleCom->reset();
						m_bDestroyWall = true;
						pWall->Drop_Item();
						pWall->Set_Destroy();
						pCurScene->Delete_GameObject(L"Layer_Environment", pWall, dynamic_cast<CStage*>(pCurScene)->Get_WallNameByIndex(iIndex)->c_str());
						pTerrain->Set_Unreachable(iIndex, false);
					}
				}
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
		// 씨앗 심은 땅 or 덜 자란 식물이면 해당 씨앗 아이템 생성
		// 씨앗 안심은 땅이면 다시 원래 타일로 되돌리기
		// 다 자란 식물이면 해당 농사 결과물(Ingredient) 아이템 생성
		// 0 ~ 8번 : 27번
		// 9 ~ 17번 : 29번
		if (m_pCalculatorCom->Check_Distance2D(&vPos, &m_vMouseWorldPos, 5.f))
		{
			_int iIndex = _int(m_vMouseWorldPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + (m_vMouseWorldPos.x + 0.5f * VTXITV);
			switch (m_pHandedItem->Get_ItemMaterial())
			{
			case MATERIAL_WOOD: // 1 x 1
				if (!m_pTerrain->Get_UnreachableByIndex(iIndex))
				{
					if (!CFarmMgr::GetInstance()->Harvest_Plant(iIndex))
						m_pTerrain->Set_TextureNumber(iIndex, 27);
				}
				break;
			case MATERIAL_COPPER: // 3 x 3
				for (_int i = -1; i <= 1; i++)
				{
					for (_int j = -1; j <= 1; j++)
					{
						if (!m_pTerrain->Get_UnreachableByIndex(iIndex + i + j * (VTXCNTX - 1)))
						{
							if (!CFarmMgr::GetInstance()->Harvest_Plant(iIndex + i + j * (VTXCNTX - 1)))
								m_pTerrain->Set_TextureNumber(iIndex + i + j * (VTXCNTX - 1), 27);
						}
					}
				}
				break;
			case MATERIAL_IRON: // 5 x 5
				for (_int i = -2; i <= 2; i++)
				{
					for (_int j = -2; j <= 2; j++)
					{
						if (!m_pTerrain->Get_UnreachableByIndex(iIndex + i + j * (VTXCNTX - 1)))
						{
							if (!CFarmMgr::GetInstance()->Harvest_Plant(iIndex + i + j * (VTXCNTX - 1)))
								m_pTerrain->Set_TextureNumber(iIndex + i + j * (VTXCNTX - 1), 27);
						}
					}
				}
				break;
			}
		}
	}
}

void CPlayer::Watering()
{
	if (g_bIsTopCamera)
	{
		_vec3 vPos;
		m_pTransformCom->Get_Info(INFO_POS, &vPos);
		// 씨앗이 심어져 있는지 확인 추가
		// 씨앗이 심어져 있을 경우 타일 변경과 동시에 해당 타일에 심겨진 씨앗 물에 젖은 상태로 변경(성장 시작)
		if (m_pCalculatorCom->Check_Distance2D(&vPos, &m_vMouseWorldPos, 5.f))
		{
			_int iIndex = _int(m_vMouseWorldPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + (m_vMouseWorldPos.x + 0.5f * VTXITV);
			switch (m_pHandedItem->Get_ItemMaterial())
			{
			case MATERIAL_COPPER: // 1 x 1
				CFarmMgr::GetInstance()->Watering_Plant(iIndex);
				break;
			case MATERIAL_IRON: // 3 x 3
				for (_int i = -1; i <= 1; i++)
				{
					for (_int j = -1; j <= 1; j++)
					{
						if (!m_pTerrain->Get_UnreachableByIndex(iIndex + i + j * (VTXCNTX - 1)))
						{
							CFarmMgr::GetInstance()->Watering_Plant(iIndex + i + j * (VTXCNTX - 1));
						}
					}
				}
				break;
			}
		}
	}
}

void CPlayer::Plant(ITEMNUM eHandedNum)
{
	if (g_bIsTopCamera)
	{
		_vec3 vPos;
		m_pTransformCom->Get_Info(INFO_POS, &vPos);
		if (m_pCalculatorCom->Check_Distance2D(&vPos, &m_vMouseWorldPos, 5.f))
		{
			_int iIndex = _int(m_vMouseWorldPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + (m_vMouseWorldPos.x + 0.5f * VTXITV);
			if (!m_pTerrain->Get_UnreachableByIndex(iIndex))
			{
				CFarmMgr::GetInstance()->Create_Plant(iIndex, eHandedNum);
				m_pInventoryCom->Minus_Item(eHandedNum, 1);
			}
		}
	}
}

void CPlayer::Install(ITEMNUM eHandedNum)
{
	if (g_bIsTopCamera)
	{
		_vec3 vPos;
		m_pTransformCom->Get_Info(INFO_POS, &vPos);
		if (m_pCalculatorCom->Check_Distance2D(&vPos, &m_vMouseWorldPos, 5.f))
		{
			_int iIndex = _int(m_vMouseWorldPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + (m_vMouseWorldPos.x + 0.5f * VTXITV);
			if (!m_pTerrain->Get_UnreachableByIndex(iIndex))
			{
				CScene* pScene = Engine::Get_Scene();
				CGameObject* pInstallObject = nullptr;
				_vec3 vInstallPos = { _float((iIndex % (VTXCNTX - 1)) * VTXITV), 0.5f, _float((iIndex / (VTXCNTX - 1)) * VTXITV) };
				MATERIAL mat;
				switch (eHandedNum)
				{
				case ITEM_TABLE:
					mat = m_pHandedItem->Get_ItemMaterial();
					pInstallObject = CTableObject::Create(m_pGraphicDev, vInstallPos, mat);
					break;
				case ITEM_POTION_TABLE:
					pInstallObject = CPotionTableObject::Create(m_pGraphicDev, vInstallPos);
					break;
				case ITEM_ACCESSORY_TABLE:
					pInstallObject = CAccessoryTableObject::Create(m_pGraphicDev, vInstallPos);
					break;
				case ITEM_MUSIC_TABLE:
					pInstallObject = CMusicTableObject::Create(m_pGraphicDev, vInstallPos);
					break;
				case ITEM_ANVIL:
					mat = m_pHandedItem->Get_ItemMaterial();
					pInstallObject = CAnvilObject::Create(m_pGraphicDev, vInstallPos, mat);
					break;
				case ITEM_FURNACE:
					pInstallObject = CFurnaceObject::Create(m_pGraphicDev, vInstallPos);
					break;
				case ITEM_COOKINGPOT:
					pInstallObject = CCookingPotObject::Create(m_pGraphicDev, vInstallPos);
					break;
				case ITEM_TORCH:
					pInstallObject = CTorchObject::Create(m_pGraphicDev, vInstallPos);
					break;
				case ITEM_BOX:
					pInstallObject = CBoxObject::Create(m_pGraphicDev, vInstallPos);
					break;
				case ITEM_GRAVESTONE:
					pInstallObject = CGravestoneObject::Create(m_pGraphicDev, vInstallPos);
					break;
				case ITEM_SPRINKLER:
					pInstallObject = CSprinklerObject::Create(m_pGraphicDev, vInstallPos);
					break;
				default:
					return;
				}
				m_vecInstallObjectName.push_back(L"Install_Object" + std::to_wstring(m_iInstallNumber++));
				FAILED_CHECK_RETURN(pScene->Create_GameObject(L"Layer_GameLogic", pInstallObject, m_vecInstallObjectName.back().c_str()));

				if (eHandedNum != ITEM_TORCH)
					m_pTerrain->Set_Unreachable(iIndex, true);

				m_pHandedItem->Set_Use(false);
				m_pHandedItem->Set_Active(false);
				m_pHandedItem->Set_Drop(false);
				m_pInventoryCom->Minus_Item(eHandedNum);
			}
		}
	}
}

void CPlayer::Eat(ITEMNUM eHandedNum)
{
	switch (eHandedNum)
	{
	case ITEM_BERRY:
		m_pStateCom->Set_Recover(28);
		m_pStateCom->Set_HungerPlus(9);
		break;
	case ITEM_PEPPER:
		m_pStateCom->Set_Recover(-11);
		m_pStateCom->Set_HungerPlus(5);
		break;
	case ITEM_CARROT:
		m_pStateCom->Set_Recover(41);
		m_pStateCom->Set_HungerPlus(7);
		break;
	case ITEM_MUSHROOM:
		m_pStateCom->Set_Recover(21);
		m_pStateCom->Set_HungerPlus(9);
		break;
	case ITEM_BERRY_BERRY_FOOD:
		m_pStateCom->Set_Recover(28);
		m_pStateCom->Set_HungerPlus(19);
		CBuffMgr::GetInstance()->Set_BuffStart(BUFF_HP, 60);
		break;
	case ITEM_BERRY_PEPPER_FOOD:
		m_pStateCom->Set_Recover(28);
		m_pStateCom->Set_HungerPlus(19);
		CBuffMgr::GetInstance()->Set_BuffStart(BUFF_SPEED, 60);
		CBuffMgr::GetInstance()->Set_BuffStart(BUFF_HP, 60);
		break;
	case ITEM_BERRY_CARROT_FOOD:
		m_pStateCom->Set_Recover(42);
		m_pStateCom->Set_HungerPlus(19);
		CBuffMgr::GetInstance()->Set_BuffStart(BUFF_SPEED, 60);
		CBuffMgr::GetInstance()->Set_BuffStart(BUFF_DEF, 60);
		break;
	case ITEM_BERRY_MUSHROOM_FOOD:
		m_pStateCom->Set_Recover(42);
		m_pStateCom->Set_HungerPlus(19);
		CBuffMgr::GetInstance()->Set_BuffStart(BUFF_HP, 60);
		break;
	case ITEM_PEPPER_PEPPER_FOOD:
		m_pStateCom->Set_HungerPlus(15);
		CBuffMgr::GetInstance()->Set_BuffStart(BUFF_SPEED, 60);
		break;
	case ITEM_PEPPER_CARROT_FOOD:
		m_pStateCom->Set_HungerPlus(15);
		CBuffMgr::GetInstance()->Set_BuffStart(BUFF_SPEED, 60);
		CBuffMgr::GetInstance()->Set_BuffStart(BUFF_DEF, 60);
		break;
	case ITEM_PEPPER_MUSHROOM_FOOD:
		m_pStateCom->Set_Recover(42);
		m_pStateCom->Set_HungerPlus(19);
		CBuffMgr::GetInstance()->Set_BuffStart(BUFF_SPEED, 60);
		break;
	case ITEM_CARROT_CARROT_FOOD:
		m_pStateCom->Set_HungerPlus(15);
		CBuffMgr::GetInstance()->Set_BuffStart(BUFF_DEF, 60);
		break;
	case ITEM_CARROT_MUSHROOM_FOOD:
		m_pStateCom->Set_Recover(42);
		m_pStateCom->Set_HungerPlus(20);
		CBuffMgr::GetInstance()->Set_BuffStart(BUFF_DEF, 60);
		break;
	case ITEM_MUSHROOM_MUSHROOM_FOOD:
		m_pStateCom->Set_Recover(42);
		m_pStateCom->Set_HungerPlus(20);
		break;
	case ITEM_LUNCH:
		m_pStateCom->Set_Recover(28);
		m_pStateCom->Set_HungerPlus(12);
		break;
	case ITEM_CHOCOBAR:
		m_pStateCom->Set_HungerPlus(19);
		CBuffMgr::GetInstance()->Set_BuffStart(BUFF_MINING, 120);
		break;
	case ITEM_POTION_HP:
		m_pStateCom->Set_Recover(200);
		break;
	case ITEM_POTION_ATT:
		CBuffMgr::GetInstance()->Set_BuffStart(BUFF_ATT, 300);
		break;
	case ITEM_POTION_DEF:
		CBuffMgr::GetInstance()->Set_BuffStart(BUFF_DEF, 300);
		break;
	}
	m_pHandedItem->Set_Use(false);
	m_pHandedItem->Set_Active(false);
	m_pHandedItem->Set_Drop(false);
	m_pInventoryCom->Minus_Item(eHandedNum);
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

	for (_int i = 0; i < CUIItemSlot::SLOT_END; i++)
	{
		wstring	strObjectTag = L"UIItemSlot_" + std::to_wstring(i);;
		m_pEtcItems[i] = dynamic_cast<CUIItemSlot*>(Engine::Get_GameObject(L"Layer_UI", strObjectTag.c_str()));
	}
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

	pHunger->Set_InfoH(m_pStateCom->Get_Hunger(), m_pStateCom->Get_MaxHunger()); // (諛곌퀬??, 理쒕?諛곌퀬??

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
		if (m_bMap || m_bChestInventory || m_bCraft || m_bInventory || m_bStatue || m_bGraveInventory || m_bFurnace || m_bCookingPot)
		{
			UI_Disable();
		}
		else if (!m_bMap && !m_bChestInventory && !m_bCraft && !m_bInventory && !m_bStatue && !m_bGraveInventory && !m_bFurnace && !m_bCookingPot)
		{
			Set_Inventory();
			Set_Craft();
			Set_Status();
		}
	}

	if (m_bInventory && (Engine::Key_Down(DIK_E)))
	{
		UI_Disable();
	}

	if ((m_bMap || m_bChestInventory || m_bCraft || m_bStatue || m_bGraveInventory || m_bFurnace || m_bCookingPot) && (Engine::Key_Down(DIK_E)))
		UI_Disable();
}

void CPlayer::Set_WallProjection()
{
	_vec3 vPos, vLook, vRight;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	m_pTransformCom->Get_Info(INFO_LOOK, &vLook);
	m_pTransformCom->Get_Info(INFO_RIGHT, &vRight);
	_int iCurIndex = _int(vPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + (vPos.x + 0.5f * VTXITV);
	_int iIndex;
	// 다음 프레임 벽과 현재 프레임 벽이 똑같으면 알파값 0.5, 아니면 다시 1
	if (g_bIsTopCamera)
	{
		// 현재 프레임 벽 = 플레이어 위치 + z축으로 -1의 방향, -2의 방향
		iIndex = iCurIndex - (VTXCNTX - 1);
		if (0 <= iIndex && iIndex < (VTXCNTX - 1) * (VTXCNTZ - 1))
		{
			if (m_pTerrain->Get_UnreachableByIndex(iIndex))
			{
				CScene* pCurScene = Engine::Get_Scene();
				CStage* pCurStage = dynamic_cast<CStage*>(pCurScene);
				CWall* pWall = dynamic_cast<CWall*>(Engine::Get_GameObject(L"Layer_Environment", pCurStage->Get_WallNameByIndex(iIndex)->c_str()));
				if (pWall)
				{
					pWall->Set_RenderAlpha(true);
					iIndex = iCurIndex - (VTXCNTX - 1) * 2;
					if (0 <= iIndex && iIndex < (VTXCNTX - 1) * (VTXCNTZ - 1))
					{
						if (m_pTerrain->Get_UnreachableByIndex(iIndex))
						{
							pWall = dynamic_cast<CWall*>(Engine::Get_GameObject(L"Layer_Environment", pCurStage->Get_WallNameByIndex(iIndex)->c_str()));
							if (pWall)
								pWall->Set_RenderAlpha(true);
						}
					}
				}
			}
		}
		// 플레이어 왼쪽에 벽이 없으면 그 아래를 투명하게
		iIndex = iCurIndex - 1;
		if (0 <= iIndex && iIndex < (VTXCNTX - 1) * (VTXCNTZ - 1))
		{
			if (!m_pTerrain->Get_UnreachableByIndex(iIndex))
			{
				iIndex = iCurIndex - (VTXCNTX - 1) - 1;
				CScene* pCurScene = Engine::Get_Scene();
				CStage* pCurStage = dynamic_cast<CStage*>(pCurScene);
				CWall* pWall = dynamic_cast<CWall*>(Engine::Get_GameObject(L"Layer_Environment", pCurStage->Get_WallNameByIndex(iIndex)->c_str()));
				if (pWall)
				{
					pWall->Set_RenderAlpha(true);
					iIndex = iCurIndex - (VTXCNTX - 1) * 2 - 1;
					if (0 <= iIndex && iIndex < (VTXCNTX - 1) * (VTXCNTZ - 1))
					{
						if (m_pTerrain->Get_UnreachableByIndex(iIndex))
						{
							pWall = dynamic_cast<CWall*>(Engine::Get_GameObject(L"Layer_Environment", pCurStage->Get_WallNameByIndex(iIndex)->c_str()));
							if (pWall)
								pWall->Set_RenderAlpha(true);
						}
					}
				}
			}
		}

		// 플레이어 오른쪽에 벽이 없으면 그 아래를 투명하게
		if (vPos.x != 0)
		{
			iIndex = iCurIndex + 1;
			if (0 <= iIndex && iIndex < (VTXCNTX - 1) * (VTXCNTZ - 1))
			{
				if (!m_pTerrain->Get_UnreachableByIndex(iIndex))
				{
					iIndex = iCurIndex - (VTXCNTX - 1) + 1;
					CScene* pCurScene = Engine::Get_Scene();
					CStage* pCurStage = dynamic_cast<CStage*>(pCurScene);
					CWall* pWall = dynamic_cast<CWall*>(Engine::Get_GameObject(L"Layer_Environment", pCurStage->Get_WallNameByIndex(iIndex)->c_str()));
					if (pWall)
					{
						pWall->Set_RenderAlpha(true);
						iIndex = iCurIndex - (VTXCNTX - 1) * 2 + 1;
						if (0 <= iIndex && iIndex < (VTXCNTX - 1) * (VTXCNTZ - 1))
						{
							if (m_pTerrain->Get_UnreachableByIndex(iIndex))
							{
								pWall = dynamic_cast<CWall*>(Engine::Get_GameObject(L"Layer_Environment", pCurStage->Get_WallNameByIndex(iIndex)->c_str()));
								if (pWall)
									pWall->Set_RenderAlpha(true);
							}
						}
					}
				}
			}
		}
	}
	else
	{
		_vec3 vCheckPos;

		for (_int i = 0; i < 20; i++)
		{
			vCheckPos = vPos - vLook * 0.1f * i;
			iIndex = _int(vCheckPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + (vCheckPos.x + 0.5f * VTXITV);

			// 현재 프레임 벽 = 플레이어 위치 + Look 반대 방향
			if (0 <= iIndex && iIndex < (VTXCNTX - 1) * (VTXCNTZ - 1))
			{
				if (m_pTerrain->Get_UnreachableByIndex(iIndex))
				{
					CScene* pCurScene = Engine::Get_Scene();
					dynamic_cast<CStage*>(pCurScene)->Get_WallNameByIndex(iIndex);

					CWall* pWall = dynamic_cast<CWall*>(Engine::Get_GameObject(L"Layer_Environment", dynamic_cast<CStage*>(pCurScene)->Get_WallNameByIndex(iIndex)->c_str()));
					if (pWall)
						pWall->Set_RenderAlpha(true);
				}
			}

			vCheckPos += vRight;
			iIndex = _int(vCheckPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + (vCheckPos.x + 0.5f * VTXITV);

			// 현재 프레임 벽 = 플레이어 위치 + Look 반대 방향
			if (0 <= iIndex && iIndex < (VTXCNTX - 1) * (VTXCNTZ - 1))
			{
				if (m_pTerrain->Get_UnreachableByIndex(iIndex))
				{
					CScene* pCurScene = Engine::Get_Scene();
					dynamic_cast<CStage*>(pCurScene)->Get_WallNameByIndex(iIndex);

					CWall* pWall = dynamic_cast<CWall*>(Engine::Get_GameObject(L"Layer_Environment", dynamic_cast<CStage*>(pCurScene)->Get_WallNameByIndex(iIndex)->c_str()));
					if (pWall)
						pWall->Set_RenderAlpha(true);
				}
			}

			vCheckPos -= vRight * 2;
			iIndex = _int(vCheckPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + (vCheckPos.x + 0.5f * VTXITV);

			// 현재 프레임 벽 = 플레이어 위치 + Look 반대 방향

			if (0 <= iIndex && iIndex < (VTXCNTX - 1) * (VTXCNTZ - 1))
			{
				if (m_pTerrain->Get_UnreachableByIndex(iIndex))
				{
					CScene* pCurScene = Engine::Get_Scene();
					dynamic_cast<CStage*>(pCurScene)->Get_WallNameByIndex(iIndex);

					CWall* pWall = dynamic_cast<CWall*>(Engine::Get_GameObject(L"Layer_Environment", dynamic_cast<CStage*>(pCurScene)->Get_WallNameByIndex(iIndex)->c_str()));
					if (pWall)
						pWall->Set_RenderAlpha(true);
				}
			}
		}
	}
}

void CPlayer::Set_Buff(const _float& fTimeDelta)
{
	ZeroMemory(&m_tBuffStat, sizeof(STAT));
	Set_Speed(m_fNormalSpeed);
	for (_int i = 0; i < BUFFTYPE_END; i++)
	{
		if (m_arrBuffState[i])
		{
			_vec2 vBuffInfo = CBuffMgr::GetInstance()->Get_BuffInfo((BUFFTYPE)i);
			_float fLeftTime = vBuffInfo.x;
			_float fMaxTime = vBuffInfo.y;
			switch (i)
			{
			case BUFF_SPEED:
				Set_Speed(m_fNormalSpeed * 1.4f);
				break;
			case BUFF_HP:
				m_tBuffStat.iMaxHp = m_pStateCom->Get_Stat()->iMaxHp * 0.05f;
				break;
			case BUFF_ATT:
				m_tBuffStat.iAttack = m_pStateCom->Get_Stat()->iAttack * 0.05f;
				break;
			case BUFF_DEF:
				m_tBuffStat.iDefense = m_pStateCom->Get_Stat()->iDefense * 0.05f;
				break;
			case BUFF_MINING:
				m_fMiningBuff[1] = 2.f;
				break;
			case DEBUFF_FIRE:
				m_bFire = true;
				m_fFireTickTime += fTimeDelta;
				if (m_fFireTickTime >= 1.f)
				{
					m_fFireTickTime = 0.f;
					if(!m_bImmune)
						m_pStateCom->Set_Damaged(10);
				}
				break;
			case DEBUFF_SLOW:
				if (m_arrBuffState[BUFF_SPEED])
					Set_Speed(m_fNormalSpeed);
				else
					Set_Speed(m_fNormalSpeed * 0.6f);
				break;
			case DEBUFF_STUN:
				if (!m_bDash)
					m_iSpeedWeight = 0;
				break;
			default:
				break;
			}
		}
		else
		{
			switch (i)
			{
			case BUFF_SPEED:
				Set_Speed(m_fNormalSpeed);
				break;
			case BUFF_HP:
			case BUFF_ATT:
			case BUFF_DEF:
			case DEBUFF_SLOW:
				break;
			case BUFF_MINING:
				m_fMiningBuff[1] = 1.f;
				break;
			case DEBUFF_FIRE:
				if (m_bFire)
				{
					m_bFire = false;
					m_fFireTickTime = false;
				}
				break;
			case DEBUFF_STUN:
				if (!m_bDash)
					m_iSpeedWeight = 1;
				break;
			default:
				break;
			}
		}
	}
	m_pStateCom->Set_BuffStat(&m_tBuffStat);
}

void CPlayer::Set_Hungry(const _float& fTimeDelta)
{
	if (m_eState != IDLE)
	{
		m_fHungerTime += fTimeDelta;
		if (m_fHungerTime >= 5.f)
		{
			m_fHungerTime = 0.f;
			m_pStateCom->Set_HungerMinus(1);
		}
	}

	if (m_pStateCom->Get_Hunger() >= 75)
	{
		if (!m_arrBuffState[BUFF_FULL])
			CBuffMgr::GetInstance()->Set_BuffStart(BUFF_FULL, 999);
	}
	else
	{
		if (m_arrBuffState[BUFF_FULL])
			CBuffMgr::GetInstance()->Set_BuffEnd(BUFF_FULL);
	}
}

void CPlayer::Set_ManaRecover(const _float& fTimeDelta)
{
	if (m_pStateCom->Get_Stat()->iMp < m_pStateCom->Get_Stat()->iMaxMp)
	{
		m_fManaTime += fTimeDelta;
		if (m_fManaTime >= 2)
		{
			m_fManaTime = 0.f;
			m_pStateCom->Set_UseMP(-5);
		}
	}
	else
		m_pStateCom->Set_MP();
}

void CPlayer::Respawn_Progress(const _float& fTimeDelta)
{
	m_fRespawnProgress += fTimeDelta;

	if (m_fRespawnProgress <= 3.f)
	{
		_vec3 vPos;
		m_pTransformCom->Get_Info(INFO_POS, &vPos);
		m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
		if (m_bRespawnFirstFrame)
		{
			m_bRespawnFirstFrame = false;

			// 장착 장비 off
			m_pHandedItem->Set_Use(false);
			m_pHandedItem->Set_Active(false);
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
						pArmor->Set_Active(false);
					else
					{
						m_pClothes[0]->Set_Active(false);
						m_pClothes[1]->Set_Active(false);
						m_pClothes[2]->Set_Active(false);
					}
					break;
				case CUIItemSlot::SLOT_CHEST:
					strObjectTag += std::to_wstring(i);
					pArmor = dynamic_cast<CUIItemSlot*>(Engine::Get_GameObject(L"Layer_UI", strObjectTag.c_str()))->Get_Item();
					if (pArmor)
						pArmor->Set_Active(false);
					else
						m_pClothes[3]->Set_Active(false);
					break;
				case CUIItemSlot::SLOT_LEGGINGS:
					strObjectTag += std::to_wstring(i);
					pArmor = dynamic_cast<CUIItemSlot*>(Engine::Get_GameObject(L"Layer_UI", strObjectTag.c_str()))->Get_Item();
					if (pArmor)
						pArmor->Set_Active(false);
					else
						m_pClothes[4]->Set_Active(false);
					break;
				}
			}
			// 묘비 생성 및 미니맵 표시
			CScene* pScene = Engine::Get_Scene();
			CGameObject* pGraveStone = CGravestoneObject::Create(m_pGraphicDev, vPos);
			m_vecInstallObjectName.push_back(L"Player_Created_Gravestone_" + std::to_wstring(m_iInstallNumber));
			pScene->Create_GameObject(L"Layer_GameLogic", pGraveStone, m_vecInstallObjectName.back().c_str());
			// 인벤토리 아이템 전부 묘비로 옮기기
		}
	}
	else if (m_fRespawnProgress <= 5.f)
	{
		if (!m_bRespawnFirstFrame)
		{
			m_bRespawnFirstFrame = true;
			// 리스폰 포인트로 이동 후 이펙트 생성

		}
		m_pTransformCom->Set_Pos(m_vRespawnPoint.x, m_vRespawnPoint.y, m_vRespawnPoint.z);
	}
	else if (m_fRespawnProgress <= 6.f)
	{
		// 리스폰 완료
		m_pStateCom->Set_Revive();
		m_fRespawnProgress = 0.f;
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

void CPlayer::Reset_Inventory()
{
	for (int i = 10; i < m_pInventoryCom->Get_SlotCount(); i++)
	{
		wstring string;

		string = L"UI_Inventory_" + std::to_wstring(i);

		CUIInventory* pInventory = dynamic_cast<CUIInventory*>(Engine::Get_GameObject(L"Layer_UI", string.c_str()));

		pInventory->Set_Show();
	}
}

void CPlayer::Set_Craft(TABLETYPE eTableType, MATERIAL _eMaterial)
{
	if (m_bMap || m_bStatue || m_bCookingPot || m_bFurnace || m_bGraveInventory || m_bChestInventory)
	{
		UI_Disable();
		return;
	}

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

	//CInventory* pPlayer = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"Player", L"Com_Inventory"));

	for (int i = 10; i < m_pInventoryCom->Get_SlotCount(); i++)
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

	if (m_bMap || m_bStatue || m_bCookingPot || m_bFurnace || m_bGraveInventory || m_bChestInventory || !m_bInventory)
	{
		UI_Disable();
		return;
	}

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
	if (m_bMap || m_bCraft || m_bStatue || m_bCookingPot || m_bFurnace || m_bGraveInventory)
	{
		UI_Disable();
		return;
	}

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

void CPlayer::Set_GraveInventory(CInventory* pInventory)
{
	if (m_bMap || m_bChestInventory || m_bCraft || m_bStatue || m_bCookingPot || m_bFurnace)
	{
		UI_Disable();
		return;
	}

	if (m_bGraveInventory)
	{
		//CInventory* pGraveInventory = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_Environment", L"AheadGrave", L"Com_Inventory"));

		for (int i = 0; i < 30; ++i)
		{
			wstring string;

			string = L"UI_GraveInventory_" + std::to_wstring(i);

			CUIChestInv* pChestInventory = dynamic_cast<CUIChestInv*>(Engine::Get_GameObject(L"Layer_UI", string.c_str()));

			pChestInventory->Set_Disable();
		}

		if (m_bInventory)
			Set_Inventory();

		m_bGraveInventory = false;
	}
	else
	{
		//CInventory* pGraveInventory = dynamic_cast<CInventory*>(Engine::Get_Component(ID_STATIC, L"Layer_Environment", L"AheadGrave", L"Com_Inventory"));

		for (int i = 0; i < 30; ++i)
		{
			wstring string;

			string = L"UI_GraveInventory_" + std::to_wstring(i);

			CUIChestInv* pChestInventory = dynamic_cast<CUIChestInv*>(Engine::Get_GameObject(L"Layer_UI", string.c_str()));

			pChestInventory->Set_Show(pInventory);
		}

		Set_Inventory();

		m_bGraveInventory = true;
	}
}

void CPlayer::Set_Statue(_int _StatueNum, _bool _bInteractioned)
{
	CUIStatue* pStatue = dynamic_cast<CUIStatue*>(Engine::Get_GameObject(L"Layer_UI", L"UI_Statue"));

	pStatue->Set_Window(_StatueNum, _bInteractioned);

	CUIJemSlot* pSlot = dynamic_cast<CUIJemSlot*>(Engine::Get_GameObject(L"Layer_UI", L"UI_JemSlot"));

	pSlot->Set_Window(_StatueNum);

	CUIStatueCraft* pSCraft = dynamic_cast<CUIStatueCraft*>(Engine::Get_GameObject(L"Layer_UI", L"UI_StatueCraft"));

	pSCraft->Set_Window();

	if (m_bStatue)
		m_bStatue = false;
	else
		m_bStatue = true;
}

void CPlayer::Set_Furnace(CInventory* pInventory1, CInventory* pInventory2, _float* fTime)
{
	if (m_bMap || m_bChestInventory || m_bCraft || m_bStatue || m_bGraveInventory || m_bCookingPot)
	{
		UI_Disable();
		return;
	}

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

		pFurnace->Set_Window(pInventory1, pInventory2, fTime);

		Set_Inventory();

		m_bFurnace = true;
	}
}

void CPlayer::Set_CookingPot(CInventory* pInventory1, CInventory* pInventory2, _float* fTime)
{
	if (m_bMap || m_bChestInventory || m_bCraft || m_bStatue || m_bGraveInventory || m_bFurnace)
	{
		UI_Disable();
		return;
	}

	if (m_bCookingPot)
	{
		CUICookingPot* pPot = dynamic_cast<CUICookingPot*>(Engine::Get_GameObject(L"Layer_UI", L"UI_CookingPot"));

		pPot->Set_Disable();

		if (m_bInventory)
		{
			Set_Inventory();
		}

		m_bCookingPot = false;
	}
	else
	{
		CUICookingPot* pPot = dynamic_cast<CUICookingPot*>(Engine::Get_GameObject(L"Layer_UI", L"UI_CookingPot"));

		pPot->Set_Render(pInventory1, pInventory2, fTime);

		Set_Inventory();

		m_bCookingPot = true;
	}
}

void CPlayer::UI_Disable()
{
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

	if (m_bCookingPot)
	{
		Set_CookingPot();
	}

	if (m_bInventory)
	{
		Set_Inventory();

		m_bInventory = false;
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

	if (m_bDestroyWall)
	{
		m_pDirtParticleCom->update(fTimeDelta);

		if (m_pDirtParticleCom->isDead())
		{
			m_pDirtParticleCom->reset();
			m_bDestroyWall = false;
		}
	}
}

void CPlayer::Set_KnockBack(_vec3 vEnemyPos, _int iDamage, _float fDist, PLAYERHITTYPE eHit)
{
	if (!m_bImmune && !m_bImmuneByTime)
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
		Set_ImmuneByTime();

		// 여기에 이펙트 추가
		switch (eHit)
		{
		case HIT_NORMAL:
			m_bBleed = true;
			break;
		case HIT_FIRE:
			CBuffMgr::GetInstance()->Set_BuffStart(DEBUFF_FIRE, 5.f);
			break;
		case HIT_ELECTRIC:
			break;
		case HIT_BULLET:
			break;
		default:
			break;
		}
	}
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
