#include "pch.h"
#include "..\Header\Player.h"

#include "Export_Utility.h"
#include "..\Header\UIStatusBar.h"
#include "..\Header\Sword.h"

#include "..\Header\UIPlayerCraft.h" // UI 헤더 추가
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

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
	m_eDir = DIRECTION_END;
	m_eState = STATE_END;
	m_fSpeed = 5.f;
	m_fDiagSpeed = sqrt(pow(m_fSpeed, 2) / 2);
	m_bSwing = false;
	m_fFirstY = 1.f;
	m_fTimeAcc = 0.f;
	m_fWalkYSpeed = 1.8f;

	m_iHandNum = 0;

	m_bMap = false;
	m_bInventory = false;
	m_bCraft = false;
	m_bFlip = false;
	m_bNoMove = false;

	m_vStartPoint = { 0, 0, 0 };
	m_vKnockBackDir = { 0, 0, 0 };
	m_bKnockBackStart = false;
	m_bKnockBackEnd = true;
	m_fKnockBackDist = 0.f;
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	m_pTransformCom->Set_Pos(vPos.x, m_fFirstY, vPos.z);
	m_pStateCom->Set_Stat(400, 100, 20, 0);
	m_pStateCom->Set_Damaged(250);
	m_pStateCom->Set_UseMP(60);
	m_pEquipInventoryCom->Set_SlotCount(10);

	return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
	KnockBack(fTimeDelta);

	Set_Equipment();
	if (m_eState != SWING)
		Show_Equipment();

	if (!m_bNoMove && !m_bInventory && !m_bCraft && !m_bMap) // m_bNoMove -> UICursor에서 적용
		Mouse_Click();
	else
		m_bSwing = false;

	if (m_bKnockBackEnd)
	{
		if (m_eState == WALK)
			Walk_Y(fTimeDelta);
		else
		{
			_vec3 vPos;
			m_pTransformCom->Get_Info(INFO_POS, &vPos);
			m_pTransformCom->Set_Pos(vPos.x, m_fFirstY, vPos.z);
		}
		if (g_bIsTopCamera)
		{
			if (!m_bSwing)
			{
				Key_Position(fTimeDelta);
				Mouse_Direction();
			}
			Animation_SetUp(m_eState, m_eDir);
		}
		else
		{
			if (!m_bSwing)
				ShoulderView_Control(fTimeDelta);
			else
				ShoulderView_Swing();
		}
	}

	Flip();
	m_pAnimatorCom->Update_Animation();
	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());
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
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, fLookSpeed);
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, fRightSpeed * iWeight);
	}
}

void CPlayer::Mouse_Click()
{
	if (m_bSwing)
	{
		if (m_pAnimatorCom->Get_MotionEnd())
			m_bSwing = false;
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
				case ITEM_SEED:
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
		m_eDir = FRONT;
		m_pAnimatorCom->Set_CurState(IDLE, 0, 0, 20);
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
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, fLookSpeed);
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, fRightSpeed);
	}

}

void CPlayer::ShoulderView_Swing()
{
	if (m_pHandedItem)
		m_pAnimatorCom->Set_CurState(SWING, 40, 41, 5);
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
	if (m_pHandedItem)
	{
		m_pHandedItem->Set_Use(true);
		m_pHandedItem->Set_Active(true);
		m_pHandedTransformCom->Set_Angle(0, 0, 0);
		_vec3 vPlayerPos;
		m_pTransformCom->Get_Info(INFO_POS, &vPlayerPos);

		if (g_bIsTopCamera)
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
		else
		{
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
			_vec3 vPlayerLook, vPlayerRight, vPlayerUp;
			m_pTransformCom->Get_Info(INFO_LOOK, &vPlayerLook);
			m_pTransformCom->Get_Info(INFO_RIGHT, &vPlayerRight);
			m_pTransformCom->Get_Info(INFO_UP, &vPlayerUp);


			//_vec3 vPlayerAngle = *(m_pTransformCom->Get_Angle());

			//_matrix matRotation;
			//D3DXMatrixRotationAxis(&matRotation, &vPlayerRight, D3DXToRadian(90.f));


			//_vec3 vHandedLook;
			//m_pHandedTransformCom->Get_Info(INFO_LOOK, &vHandedLook);
			//D3DXVec3TransformNormal(&vHandedLook, &vHandedLook, &matRotation);
			m_pHandedTransformCom->Set_Pos(vPlayerPos.x + vPlayerLook.x * 0.2f + vPlayerRight.x * 0.5f, 1.f, vPlayerPos.z + vPlayerLook.z * 0.2f + vPlayerRight.z * 0.5f);
		}
		m_pHandedItem->Set_Swing(m_eDir, true);
	}
}

void CPlayer::Set_UI()
{

	CUIStatusBar* pHp = dynamic_cast<CUIStatusBar*>
		(Engine::Get_GameObject(L"Layer_UI", L"UI_Health"));
	NULL_CHECK_RETURN(pHp);

	pHp->Set_InfoH(m_pStateCom->Get_Stat()->iHp, m_pStateCom->Get_Stat()->iMaxHp); // (체력 , 최대체력)

	CUIStatusBar* pMp = dynamic_cast<CUIStatusBar*>
		(Engine::Get_GameObject(L"Layer_UI", L"UI_Mp"));
	NULL_CHECK_RETURN(pMp);

	pMp->Set_InfoH(m_pStateCom->Get_Stat()->iMp, m_pStateCom->Get_Stat()->iMaxMp); // (마나 , 최대마나)

	if (Engine::Get_DIMouseMove(DIMS_Z))
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
		Set_Map();
	}
	if (Engine::Key_Down(DIK_TAB))
	{
		Set_Inventory();
		Set_Craft();
	}
	
	if (Engine::Key_Down(DIK_E))
	{
		if (m_bCraft)
		{
			Set_Craft();

			m_bCraft = false;
		}
		if (m_bInventory)
		{
			Set_Inventory();

			m_bInventory = false;
		}
		if (m_bMap)
		{
			Set_Map();

			m_bMap = false;
		}
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

void CPlayer::Set_Craft()
{
	CUIPlayerCraft* pCraft = dynamic_cast<CUIPlayerCraft*>(Engine::Get_GameObject(L"Layer_UI", L"UIPlayerCraft"));
	pCraft->Set_Window();

	if (m_bCraft)
		m_bCraft = false;
	else
		m_bCraft = true;
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

	for (int i = 0; i < 10; i++)
	{
		wstring string;

		string = L"UIItemSlot_" + std::to_wstring(i);

		CUIItemSlot* pSlot = dynamic_cast<CUIItemSlot*>(Engine::Get_GameObject(L"Layer_UI", string.c_str()));

		pSlot->Set_Window();
	}

 	CUIPlayerStatus* pStatus = dynamic_cast<CUIPlayerStatus*>(Engine::Get_GameObject(L"Layer_UI", L"UIPlayerStatus"));
	pStatus->Set_Window();

	CUIPlayerStats* pStats = dynamic_cast<CUIPlayerStats*>(Engine::Get_GameObject(L"Layer_UI", L"UIPlayerStats"));
	pStats->Set_Window();

	for (int i = 0; i < 5; i++)
	{
		wstring string;

		string = L"UICraftSlot_" + std::to_wstring(i);

		CUICraftSlot* pSlot = dynamic_cast<CUICraftSlot*>(Engine::Get_GameObject(L"Layer_UI", string.c_str()));

		pSlot->Set_Window();
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

	m_bMap = true;
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
		if (fLength >= m_fKnockBackDist)
		{
			m_bKnockBackStart = false;
			m_bKnockBackEnd = true;
			if (m_eState != DEAD)
				m_eState = IDLE;
			return;
		}
		m_pTransformCom->Move_Pos(&m_vKnockBackDir, fTimeDelta, m_fSpeed * (m_fKnockBackDist / fLength));
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
