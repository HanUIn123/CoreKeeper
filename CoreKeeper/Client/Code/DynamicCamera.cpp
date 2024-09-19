#include "pch.h"
#include "..\Header\DynamicCamera.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\UICursor.h"

CDynamicCamera::CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCamera(pGraphicDev), m_bFix(false)
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

	return iExit;
}

void CDynamicCamera::LateUpdate_GameObject()
{
	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));
	if (pPlayerTransform)
	{
		Key_Input();
		if (g_bIsTopCamera)
		{
			_matrix		matPlayerWorld;
			memcpy(&matPlayerWorld, pPlayerTransform->Get_WorldMatrix(), sizeof(_matrix));

			_matrix		matWorld;
			D3DXMatrixIdentity(&matWorld);

			for (_int i = 0; i < INFO_POS; ++i)
			{
				memcpy(&matPlayerWorld.m[i][0], &matWorld.m[i][0], sizeof(_vec3));
			}
			pPlayerTransform->Set_WorldMatrix(&matPlayerWorld);

			_vec3 vPlayerPos;
			pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

			m_vAt = vPlayerPos;
			m_vAt.y = 0.f;
			vPlayerPos.z -= 10.f;
			m_vEye = vPlayerPos;
			m_vEye.y = 12.f;
		}
		else
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
	if (Engine::Key_Down(DIK_C)) // 잠시 변경 하겠습니다 (TAB->LALT)
	{
		g_bIsTopCamera = g_bIsTopCamera ? false : true;
		m_bFix = !g_bIsTopCamera;

		CUICursor* pCursor = dynamic_cast<CUICursor*>(Engine::Get_GameObject(L"Layer_UI", L"UI_Cursor"));

		pCursor->Set_Cursor_Disable();
	}
}

void CDynamicCamera::Mouse_Move()
{
	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));
	NULL_CHECK(pPlayerTransform);

	_vec3 vPlayerPos;
	pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

	_matrix		matPlayerWorld;
	memcpy(&matPlayerWorld, pPlayerTransform->Get_WorldMatrix(), sizeof(_matrix));

	_long	dwMouseMove(0);
	if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_X))
		pPlayerTransform->Rotation(ROT_Y, D3DXToRadian(dwMouseMove / 10.f));

	_vec3	vLook;
	memcpy(&vLook, &matPlayerWorld.m[INFO_LOOK][0], sizeof(_vec3));

	m_vAt.x = vPlayerPos.x + vLook.x * 2;
	m_vAt.y = vPlayerPos.y + 0.2f;
	m_vAt.z = vPlayerPos.z + vLook.z * 2;

	m_vEye.x = vPlayerPos.x - vLook.x * 3;
	m_vEye.y = vPlayerPos.y + 1.5f;
	m_vEye.z = vPlayerPos.z - vLook.z * 3;
}

void CDynamicCamera::Mouse_Fix()
{
	POINT	ptMouse{ WINCX >> 1, WINCY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);

}
