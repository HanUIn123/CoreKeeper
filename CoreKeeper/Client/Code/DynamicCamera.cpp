#include "pch.h"
#include "..\Header\DynamicCamera.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\UICursor.h"

CDynamicCamera::CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCamera(pGraphicDev), m_bFix(false), m_bIsWorldMap(false), m_pTargetTransform(nullptr), m_bTargetPlayer(true)
{
}


CDynamicCamera::~CDynamicCamera()
{
}

HRESULT CDynamicCamera::Ready_GameObject(const _vec3* pEye, const _vec3* pAt, const _vec3* pUp, const _float& _fFov, const _float& _fAspect, const _float& _fNear, const _float& _fFar)
{
	m_vEye = *pEye;
	m_vAt = *pAt;
	m_vUp = *pUp;

	m_fFov = _fFov;
	m_fAspect = _fAspect;
	m_fNear = _fNear;
	m_fFar = _fFar;

	FAILED_CHECK_RETURN(CCamera::Ready_GameObject(), E_FAIL);

	return S_OK;
}

_int CDynamicCamera::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = CCamera::Update_GameObject(fTimeDelta);
	if(!m_pTargetTransform)
		m_pTargetTransform = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));

	return iExit;
}

void CDynamicCamera::LateUpdate_GameObject()
{
	if (m_pTargetTransform)
	{
		Key_Input();
		if (g_bIsTopCamera)
		{
			m_pTargetTransform->Set_Angle(0, 0, 0);

			_vec3 vTargetPos;
			m_pTargetTransform->Get_Info(INFO_POS, &vTargetPos);

			m_vAt = vTargetPos;
			m_vAt.y = 0.f;
			vTargetPos.z -= 10.f;
			m_vEye = vTargetPos;
			m_vEye.y = 12.f;
		}
	/*	 if (m_bIsWorldMap)
		{
			pPlayerTransform->Set_Angle(0, 0, 0);

			_vec3 vPlayerPos;
			pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

			m_vAt = { 128.5f, 0, 128.5f };
			vPlayerPos.z -= 0.01f;
			m_vEye = { 128.5f, 350.0f, 128.499f };

			_long dwMouseWheel = Engine::Get_DIMouseMove(DIMS_Z);

			if (dwMouseWheel > 0)
			{
				Zoom_In();
			}
			else if (dwMouseWheel < 0)
			{
				Zoom_Out();
			}
		}*/
		//else if (!m_bIsWorldMap)
		//{
		//	pPlayerTransform->Set_Angle(0, 0, 0);

		//	_vec3 vPlayerPos;
		//	pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

		//	m_vAt = vPlayerPos;
		//	m_vAt.y = 0.f;
		//	vPlayerPos.z -= 10.f;
		//	m_vEye = vPlayerPos;
		//	m_vEye.y = 12.f;
		//}
		
		if(!g_bIsTopCamera)
		{
			Mouse_Fix();
			Mouse_Move();
		}
	}
	CCamera::LateUpdate_GameObject();
}

CDynamicCamera* CDynamicCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev,
	const _vec3* pEye, const _vec3* pAt, const _vec3* pUp,
	const _float& _fFov, const _float& _fAspect,
	const _float& _fNear, const _float& _fFar)
{
	CDynamicCamera* pCamera = new CDynamicCamera(pGraphicDev);

	if (FAILED(pCamera->Ready_GameObject(pEye, pAt, pUp, _fFov, _fAspect, _fNear, _fFar)))
	{
		Safe_Release(pCamera);
		MSG_BOX("Dynamic Camera Create Failed");
		return nullptr;
	}

	return pCamera;
}

void CDynamicCamera::Free()
{
	CCamera::Free();
}

void CDynamicCamera::Key_Input()
{
	if (!m_bIsWorldMap)
	{
		if (Engine::Key_Down(DIK_C)) // 잠시 변경 하겠습니다 (TAB->LALT)
		{
			g_bIsTopCamera = g_bIsTopCamera ? false : true;
			m_bFix = !g_bIsTopCamera;

			CUICursor* pCursor = dynamic_cast<CUICursor*>(Engine::Get_GameObject(L"Layer_UI", L"UI_Cursor"));

			pCursor->Set_Cursor_Disable();
		}

		if (Engine::Key_Down(DIK_F6))
		{
			m_bTargetPlayer = m_bTargetPlayer ? false : true;
			if(m_bTargetPlayer)
				m_pTargetTransform = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));
			else
				m_pTargetTransform = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Hunter", L"Com_Transform"));

			if(!m_pTargetTransform)
				m_pTargetTransform = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));

		}
	}

	//if (g_bIsTopCamera)
	//{
	//	if (Engine::Key_Down(DIK_N))
	//	{
	//		m_bIsWorldMap = m_bIsWorldMap ? false : true;

	//		m_fFov = D3DXToRadian(60.f);

	//		//g_bIsTopCamera = g_bIsTopCamera ? false : true;
	//	}
	//}

}

void CDynamicCamera::Mouse_Move()
{
	_vec3 vTargetPos;
	m_pTargetTransform->Get_Info(INFO_POS, &vTargetPos);

	_matrix		matTargetWorld;
	memcpy(&matTargetWorld, m_pTargetTransform->Get_WorldMatrix(), sizeof(_matrix));

	_long	dwMouseMove(0);
	if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_X))
		m_pTargetTransform->Rotation(ROT_Y, D3DXToRadian(dwMouseMove / 10.f));

	_vec3	vLook;
	memcpy(&vLook, &matTargetWorld.m[INFO_LOOK][0], sizeof(_vec3));

	m_vAt.x = vTargetPos.x + vLook.x * 2;
	m_vAt.y = 0.8f;
	m_vAt.z = vTargetPos.z + vLook.z * 2;

	m_vEye.x = vTargetPos.x - vLook.x * 3;
	m_vEye.y = 3.f;
	m_vEye.z = vTargetPos.z - vLook.z * 3;
}

void CDynamicCamera::Mouse_Fix()
{
	POINT	ptMouse{ WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);

}

void CDynamicCamera::Zoom_In()
{
	if (m_fFov > D3DXToRadian(10.0f))
	{
		m_fFov -= D3DXToRadian(5.0f);

		D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);
		m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);
	}
}

void CDynamicCamera::Zoom_Out()
{

	if (m_fFov < D3DXToRadian(60.0f))
	{
		m_fFov += D3DXToRadian(5.0f);

		D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);
		m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);
	}
}
