#include "pch.h"
#include "..\Header\Player.h"

#include "Export_Utility.h"
#include "..\Header\UIHealth.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
	m_eDir = DIRECTION_END;
	m_eState = STATE_END;
	m_fSpeed = 5.f;
	m_fDiagSpeed = sqrt(pow(m_fSpeed, 2) / 2);
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(m_pTransformCom->m_vInfo->x, m_pTransformCom->m_vInfo->y + 1.0f, m_pTransformCom->m_vInfo->z);

	return S_OK;
}

_int CPlayer::Update_GameObject(const _float& fTimeDelta)
{
	if (g_bIsTopCamera)
	{
		Mouse_Direction();
		Key_Position(fTimeDelta);
		Animation_SetUp(m_eState, m_eDir);
	}
	else
	{
		Key_Position(fTimeDelta);
		ShoulderView_Control(fTimeDelta);
	}

	CUIHealth* pUI = dynamic_cast<CUIHealth*>
		(Engine::Get_GameObject(L"Layer_UI", L"UI_Health"));
	NULL_CHECK_RETURN(pUI, -1);

	pUI->Set_Hp(100, 100);

	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());
	Add_RenderGroup(RENDER_ALPHA, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CPlayer::LateUpdate_GameObject()
{
	m_pAnimatorCom->Update_Animation();

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
	if (Engine::Get_DIMouseState(DIM_LB) & 0x80)
	{

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
		else if (m_eDir == RIGHT)
			m_pAnimatorCom->Set_CurState(st, 1, 1, 20);
		else if (m_eDir == BACK)
			m_pAnimatorCom->Set_CurState(st, 2, 2, 20);
		else if (m_eDir == LEFT)
			m_pAnimatorCom->Set_CurState(st, 3, 3, 20);
		break;
	case WALK:
		if (m_eDir == FRONT)
			m_pAnimatorCom->Set_CurState(st, 4, 7, 20);
		else if (m_eDir == RIGHT)
			m_pAnimatorCom->Set_CurState(st, 8, 11, 20);
		else if (m_eDir == BACK)
			m_pAnimatorCom->Set_CurState(st, 12, 15, 20);
		else if (m_eDir == LEFT)
			m_pAnimatorCom->Set_CurState(st, 16, 19, 20);
		break;
	case SWING:
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
			m_pAnimatorCom->Set_CurState(WALK, 8, 11, 20);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, m_fDiagSpeed);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, m_fDiagSpeed);

		}
		else if (Engine::Get_DIKeyState(DIK_A))
		{
			m_pAnimatorCom->Set_CurState(WALK, 16, 19, 20);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, m_fDiagSpeed);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, -m_fDiagSpeed);
		}
		else
		{
			m_pAnimatorCom->Set_CurState(WALK, 12, 15, 20);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, m_fSpeed);
		}
	}
	// BOTTOM (RIGHT, LEFT)
	else if (Engine::Get_DIKeyState(DIK_S))
	{
		m_eState = WALK;
		if (Engine::Get_DIKeyState(DIK_D))
		{
			m_pAnimatorCom->Set_CurState(WALK, 8, 11, 20);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, -m_fDiagSpeed);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, m_fDiagSpeed);
		}
		else if (Engine::Get_DIKeyState(DIK_A))
		{
			m_pAnimatorCom->Set_CurState(WALK, 16, 19, 20);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, -m_fDiagSpeed);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, -m_fDiagSpeed);
		}
		else
		{
			m_pAnimatorCom->Set_CurState(WALK, 4, 7, 20);
			m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, -m_fSpeed);
		}
	}
	// L
	else if (Engine::Get_DIKeyState(DIK_A))
	{
		m_eState = WALK;
		m_pAnimatorCom->Set_CurState(WALK, 16, 19, 20);
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, -m_fSpeed);
	}
	// R
	else if (Engine::Get_DIKeyState(DIK_D))
	{
		m_eState = WALK;
		m_pAnimatorCom->Set_CurState(WALK, 8, 11, 20);
		m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, m_fSpeed);
	}
	else
	{
		m_eState = IDLE;
		m_pAnimatorCom->Set_CurState(IDLE, 0, 0, 20);
		// m_pAnimatorCom->Set_CurState(IDLE, 2, 2, 20);
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
