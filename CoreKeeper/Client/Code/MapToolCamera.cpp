#include "pch.h"
#include "../Header/MapToolCamera.h"
#include "Export_System.h"
#include "Export_Utility.h"

CMapToolCamera::CMapToolCamera(LPDIRECT3DDEVICE9 _pGraphicDevice)
    : CCamera(_pGraphicDevice), m_bFix(false), m_bCheck(false), m_CameraSpeed(20.0f)
{
}

CMapToolCamera::~CMapToolCamera()
{
}

HRESULT CMapToolCamera::Ready_GameObject(const _vec3* pEye, const _vec3* pAt, const _vec3* pUp, const _float& _fFov, const _float& _fAspect, const _float& _fNear, const _float& _fFar)
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

_int CMapToolCamera::Update_GameObject(const _float& _fTimeDelta)
{
    _int iExit = CCamera::Update_GameObject((_int)_fTimeDelta);

    Key_Input(_fTimeDelta);

    return iExit;
}

void CMapToolCamera::LateUpdate_GameObject()
{
    if (false == m_bFix)
    {
        Mouse_Fix();
        Mouse_Move();
    }

    CCamera::LateUpdate_GameObject();
}

void CMapToolCamera::Key_Input(const _float& _fTimeDelta)
{
    _matrix		matCamWorld;
    D3DXMatrixInverse(&matCamWorld, 0, &m_matView);

    if (Engine::Get_DIKeyState(DIK_W) & 0x80)
    {
        _vec3	vLook;
        memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

        _vec3	vLength = *D3DXVec3Normalize(&vLook, &vLook) * _fTimeDelta * m_CameraSpeed;

        m_vEye += vLength;
        m_vAt += vLength;
    }

    if (Engine::Get_DIKeyState(DIK_S) & 0x80)
    {
        _vec3 vLook;
        memcpy(&vLook, &matCamWorld.m[2][0], sizeof(_vec3));

        _vec3 vLength = *D3DXVec3Normalize(&vLook, &vLook) * _fTimeDelta * m_CameraSpeed;

        m_vEye -= vLength;
        m_vAt -= vLength;
    }

    if (Engine::Get_DIKeyState(DIK_D) & 0x80)
    {
        _vec3	vRight;
        memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

        _vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * _fTimeDelta * m_CameraSpeed;

        m_vEye += vLength;
        m_vAt += vLength;
    }

    if (Engine::Get_DIKeyState(DIK_A) & 0x80)
    {
        _vec3	vRight;
        memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

        _vec3	vLength = *D3DXVec3Normalize(&vRight, &vRight) * _fTimeDelta * m_CameraSpeed;

        m_vEye -= vLength;
        m_vAt -= vLength;
    }

    if (Engine::Get_DIKeyState(DIK_TAB) & 0x80)
    {
        if (m_bCheck)
            return;

        m_bCheck = true;

        if (m_bFix)
            m_bFix = false;
        else
            m_bFix = true;
    }
    else
    {
        m_bCheck = false;
    }

    if (false == m_bFix)
        return;
}

void CMapToolCamera::Mouse_Move()
{
    _matrix matCamWorld;
    D3DXMatrixInverse(&matCamWorld, 0, &m_matView);

    _long	dwMouseMove(0);

    // y축을 기준이기에, 마우스를 좌우로 흔들면 카메라도 따라 좌우로 움직인다는 함수
    if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_Y))
    {
        _vec3	vRight;
        memcpy(&vRight, &matCamWorld.m[0][0], sizeof(_vec3));

        _vec3	vLook = m_vAt - m_vEye;

        _matrix		matRot;
        D3DXMatrixRotationAxis(&matRot, &vRight, D3DXToRadian(dwMouseMove / 10.f));

        D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

        m_vAt = m_vEye + vLook;
    }

    if (dwMouseMove = Engine::Get_DIMouseMove(DIMS_X))
    {
        _vec3	vUp{ 0.f, 1.f, 0.f };

        _vec3	vLook = m_vAt - m_vEye;

        _matrix		matRot;
        D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(dwMouseMove / 10.f));
        D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

        m_vAt = m_vEye + vLook;
    }
}

void CMapToolCamera::Mouse_Fix()
{
    POINT	ptMouse{ WINCX >> 1, WINCY >> 1 };	//  / 2 라는 비트 연산.

    ClientToScreen(g_hWnd, &ptMouse);
    SetCursorPos(ptMouse.x, ptMouse.y);
}

CMapToolCamera* CMapToolCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pEye, const _vec3* pAt, const _vec3* pUp, const _float& _fFov, const _float& _fAspect, const _float& _fNear, const _float& _fFar)
{
    CMapToolCamera* pMapToolCamera = new CMapToolCamera(pGraphicDev);

    if (FAILED(pMapToolCamera->Ready_GameObject(pEye, pAt, pUp, _fFov, _fAspect, _fNear, _fFar)))
    {
        Safe_Release(pMapToolCamera);
        MSG_BOX("Dynamic Camera Create Failed");
        return nullptr;
    }

    return pMapToolCamera;
}

void CMapToolCamera::Free()
{
    CCamera::Free();
}
