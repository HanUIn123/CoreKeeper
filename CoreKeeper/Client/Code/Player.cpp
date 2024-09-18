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

	m_iHandNum = 1;
	m_bPushed = false;
	m_bMap = false;
	m_bInventory = false;
	m_bCraft = false;
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

	return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
	m_pAnimatorCom->Update_Animation();
	Mouse_Click();
	Show_Equipment();
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
		{
			Key_Position(fTimeDelta);
			ShoulderView_Control(fTimeDelta);
		}
		else
			ShoulderView_Swing();
	}

	Set_UI();

	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());
	
	Add_RenderGroup(RENDER_ALPHA, this);

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

	return S_OK;
}

void CPlayer::Key_Position(const _float& fTimeDelta)
{
	_vec3	vLook, vRight;

	m_pTransformCom->Get_Info(INFO_LOOK, &vLook);
	m_pTransformCom->Get_Info(INFO_RIGHT, &vRight);

	// TOP (RIGHT, LEFT)
	if (Engine::Get_DIKeyState(DIK_W))
	{
		m_eState = WALK;
		if (Engine::Get_DIKeyState(DIK_D))
		{
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, m_fDiagSpeed);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, m_fDiagSpeed);
		}
		else if (Engine::Get_DIKeyState(DIK_A))
		{
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, m_fDiagSpeed);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, -m_fDiagSpeed);
		}
		else
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, m_fSpeed);
	}
	// BOTTOM (RIGHT, LEFT)
	else if (Engine::Get_DIKeyState(DIK_S))
	{
		m_eState = WALK;
		if (Engine::Get_DIKeyState(DIK_D))
		{
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, -m_fDiagSpeed);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, m_fDiagSpeed);
		}
		else if (Engine::Get_DIKeyState(DIK_A))
		{
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, -m_fDiagSpeed);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, -m_fDiagSpeed);
		}
		else
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, -m_fSpeed);
	}
	// L
	else if (Engine::Get_DIKeyState(DIK_A))
	{
		m_eState = WALK;
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, -m_fSpeed);
	}
	// R
	else if (Engine::Get_DIKeyState(DIK_D))
	{
		m_eState = WALK;
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, m_fSpeed);
	}
	else
	{
		m_eState = IDLE;
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
			m_eState = SWING;
			m_bSwing = true;
			Swing_Equipment();
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
	dynamic_cast<CItem*>(m_pWeapon)->Walk_Equipped(fTimeDelta);
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
		else if (m_eDir == RIGHT)
			m_pAnimatorCom->Set_CurState(st, 1, 1, 20);
		else if (m_eDir == BACK)
			m_pAnimatorCom->Set_CurState(st, 2, 2, 20);
		else if (m_eDir == LEFT)
			m_pAnimatorCom->Set_CurState(st, 3, 3, 20);
		break;
	case WALK:
		if (m_eDir == FRONT)
			m_pAnimatorCom->Set_CurState(st, 4, 7, 6);
		else if (m_eDir == RIGHT)
			m_pAnimatorCom->Set_CurState(st, 8, 11, 6);
		else if (m_eDir == BACK)
			m_pAnimatorCom->Set_CurState(st, 12, 15, 6);
		else if (m_eDir == LEFT)
			m_pAnimatorCom->Set_CurState(st, 16, 19, 6);
		break;
	case SWING:
		if (m_eDir == FRONT)
			m_pAnimatorCom->Set_CurState(st, 20, 20, 10);
		else if (m_eDir == RIGHT)
			m_pAnimatorCom->Set_CurState(st, 21, 23, 10);
		else if (m_eDir == BACK)
			m_pAnimatorCom->Set_CurState(st, 24, 25, 10);
		else if (m_eDir == LEFT)
			m_pAnimatorCom->Set_CurState(st, 26, 28, 10);
		break;
	}
}

void CPlayer::ShoulderView_Control(const _float& fTimeDelta)
{
	_vec3	vLook, vRight;

	m_pTransformCom->Get_Info(INFO_LOOK, &vLook);
	m_pTransformCom->Get_Info(INFO_RIGHT, &vRight);

	// TOP (RIGHT, LEFT)
	if (Engine::Get_DIKeyState(DIK_W))
	{
		m_eState = WALK;
		if (Engine::Get_DIKeyState(DIK_D))
		{
			m_pAnimatorCom->Set_CurState(WALK, 8, 11, 6);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, m_fDiagSpeed);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, m_fDiagSpeed);

		}
		else if (Engine::Get_DIKeyState(DIK_A))
		{
			m_pAnimatorCom->Set_CurState(WALK, 16, 19, 6);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, m_fDiagSpeed);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, -m_fDiagSpeed);
		}
		else
		{
			m_pAnimatorCom->Set_CurState(WALK, 12, 15, 6);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, m_fSpeed);
		}
	}
	// BOTTOM (RIGHT, LEFT)
	else if (Engine::Get_DIKeyState(DIK_S))
	{
		m_eState = WALK;
		if (Engine::Get_DIKeyState(DIK_D))
		{
			m_pAnimatorCom->Set_CurState(WALK, 8, 11, 6);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, -m_fDiagSpeed);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, m_fDiagSpeed);
		}
		else if (Engine::Get_DIKeyState(DIK_A))
		{
			m_pAnimatorCom->Set_CurState(WALK, 16, 19, 6);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, -m_fDiagSpeed);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, -m_fDiagSpeed);
		}
		else
		{
			m_pAnimatorCom->Set_CurState(WALK, 4, 7, 6);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, -m_fSpeed);
		}
	}
	// L
	else if (Engine::Get_DIKeyState(DIK_A))
	{
		m_eState = WALK;
		m_pAnimatorCom->Set_CurState(WALK, 16, 19, 6);
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, -m_fSpeed);
	}
	// R
	else if (Engine::Get_DIKeyState(DIK_D))
	{
		m_eState = WALK;
		m_pAnimatorCom->Set_CurState(WALK, 8, 11, 6);
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, m_fSpeed);
	}
	else
	{
		m_eState = IDLE;
		m_pAnimatorCom->Set_CurState(IDLE, 0, 0, 6);
	}
}

void CPlayer::ShoulderView_Swing()
{
	m_pAnimatorCom->Set_CurState(SWING, 24, 25, 10);
}

void CPlayer::Show_Equipment()
{
	m_pWeapon = Get_GameObject(L"Layer_GameLogic", L"Sword2");
	CTransform* weaponTransform = dynamic_cast<CTransform*>(m_pWeapon->Get_Component(ID_DYNAMIC, L"Com_Transform"));
	_vec3 vPlayerPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPlayerPos);
	dynamic_cast<CSword*>(m_pWeapon)->Set_Use(true);

	if (g_bIsTopCamera)
	{
		switch (m_eDir)
		{
		case FRONT:
			weaponTransform->Set_Pos(vPlayerPos.x - 0.5f, 0.85f, vPlayerPos.z - 0.1f);
			break;
		case RIGHT:
			weaponTransform->Set_Pos(vPlayerPos.x - 0.4f, 0.8f, vPlayerPos.z - 0.1f);
			break;
		case BACK:
			weaponTransform->Set_Pos(vPlayerPos.x + 0.5f, 0.8f, vPlayerPos.z + 0.1f);
			break;
		case LEFT:
			weaponTransform->Set_Pos(vPlayerPos.x + 0.5f, 0.8f, vPlayerPos.z - 0.1f);
			break;
		}
	}
	else
	{

	}
}
void CPlayer::Swing_Equipment()
{
	dynamic_cast<CItem*>(m_pWeapon)->Set_Swing(m_eDir, true);
}

void CPlayer::Set_UI()
{
	
	CUIStatusBar* pHp = dynamic_cast<CUIStatusBar*>
		(Engine::Get_GameObject(L"Layer_UI", L"UI_Health"));
	NULL_CHECK_RETURN(pHp);

	pHp->Set_InfoH(150, 400); // (체력 , 최대체력)

	CUIStatusBar* pMp = dynamic_cast<CUIStatusBar*>
		(Engine::Get_GameObject(L"Layer_UI", L"UI_Mp"));
	NULL_CHECK_RETURN(pMp);

	pMp->Set_InfoH(40, 100); // (마나 , 최대마나)

	if (Engine::Get_DIMouseMove(DIMS_Z))
	{
		if (Engine::Get_DIMouseMove(DIMS_Z) < 0)
			m_iHandNum++;
		else
			m_iHandNum--;


		if (m_iHandNum > 10)
		{
			m_iHandNum = 1;
		}
		else if (m_iHandNum <= 0)
		{
			m_iHandNum = 10;
		}
	}

	if (Engine::Get_DIKeyState(DIK_M))
	{

	}
	
	if (Engine::Get_DIKeyState(DIK_TAB))
	{
		m_bPushed = true;
	}
	if (!Engine::Get_DIKeyState(DIK_TAB) && m_bPushed)
	{
		m_bPushed = false;

		Set_Inventory();
		Set_Craft();

		if (m_bInventory)
			m_bInventory = false;
		else
			m_bInventory = true;

		if (m_bCraft)
			m_bCraft = false;
		else
			m_bCraft = true;
	}
	
	/*
	if (Engine::Get_DIKeyState(DIK_E))
	{
		m_bPushed = true;
	}
	else if (!Engine::Get_DIKeyState(DIK_E) && m_bPushed)
	{
		m_bPushed = false;

		if (m_bCraft)
			Set_Craft();

		if (m_bInventory)
			Set_Inventory();

		if (m_bMap)
			Set_Map();
	}*/
}

void CPlayer::Set_Craft()
{
	CUIPlayerCraft* pCraft = dynamic_cast<CUIPlayerCraft*>(Engine::Get_GameObject(L"Layer_UI", L"UIPlayerCraft"));
	pCraft->Set_Window();
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

	for (int i = 11; i < pPlayer->Get_Slot() + 1; i++)
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
}

void CPlayer::Set_Map()
{
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
