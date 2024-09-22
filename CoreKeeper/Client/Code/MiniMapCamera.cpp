#include "pch.h"
#include "../Header/MiniMapCamera.h"

CMiniMapCamera::CMiniMapCamera(LPDIRECT3DDEVICE9 _pGraphicDevice)
    :CCamera(_pGraphicDevice)
{
}

CMiniMapCamera::~CMiniMapCamera()
{
}

HRESULT CMiniMapCamera::Ready_GameObject(const _vec3* pEye, const _vec3* pAt, const _vec3* pUp, const _float& _fFov, const _float& _fAspect, const _float& _fNear, const _float& _fFar)
{
    m_vEye = *pEye;
    m_vAt = *pAt;
    m_vUp = *pUp;

    m_fFov = _fFov;
    m_fAspect = _fAspect;
    m_fNear = _fNear;
    m_fFar = _fFar;

    FAILED_CHECK_RETURN(CCamera::Ready_GameObject(), E_FAIL);

    Setting_MiniMapCamera(&m_vEye, &m_vAt, &m_vUp);

    return S_OK;
}

_int CMiniMapCamera::Update_GameObject(const _float& _fTimeDelta)
{
    _int iExit = CCamera::Update_GameObject(_fTimeDelta);

    return iExit;
}

void CMiniMapCamera::LateUpdate_GameObject()
{
    CCamera::LateUpdate_GameObject();
}

void CMiniMapCamera::Setting_MiniMapCamera(const _vec3* pEye, const _vec3* pAt, const _vec3* pUp)
{    
    D3DXMatrixLookAtLH(&m_matView, pEye, pAt, pUp);
    m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

    D3DXMatrixOrthoLH(&m_matProj, 200.f, 200.0f, 0.f, 1.f);
    m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);
}

CMiniMapCamera* CMiniMapCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pEye, const _vec3* pAt, const _vec3* pUp, const _float& _fFov, const _float& _fAspect, const _float& _fNear, const _float& _fFar)
{
    CMiniMapCamera* pInstance = new CMiniMapCamera(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject(pEye, pAt, pUp, _fFov, _fAspect, _fNear, _fFar)))
    {
        Safe_Release(pInstance);
        MSG_BOX("MiniMap Camera Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CMiniMapCamera::Free()
{
    CCamera::Free();
}
