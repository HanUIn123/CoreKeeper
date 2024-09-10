#include "..\..\Header\Camera.h"

CCamera::CCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_vEye({0.f, 0.f, 0.f})
	, m_vAt({ 0.f, 0.f, 0.f })
	, m_vUp({ 0.f, 0.f, 0.f })
	, m_fFov(0.f), m_fAspect(0.f), m_fNear(0.f), m_fFar(0.f)
	 
{
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
}

CCamera::~CCamera()
{
}

HRESULT CCamera::Ready_GameObject()
{
	D3DXMatrixLookAtLH(&m_matView, &m_vEye,	&m_vAt,	&m_vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);

	return S_OK;
}

_int CCamera::Update_GameObject(const _float & fTimeDelta)
{
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	return 0;
}

void CCamera::LateUpdate_GameObject()
{
}



void CCamera::Set_Render(RENDERTYPE _eType)
{
	//원근 투영

	if (_eType == TYPE_PERSPECTIVE)
	{
		D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);
		m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);
	}
	// 직교 투영
	else
	{
		_vec3 _vEye = { 0, 0, 0 };
		_vec3 _vAt = { 0, 0, 1 };
		_vec3 _vUp = { 0, 1, 0 };

		//바라보는 방향 정면으로 고정

		D3DXMatrixLookAtLH(&m_matView, &_vEye, &_vAt, &_vUp);
		m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

		D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);
		m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);
	}
}
void CCamera::Free()
{
	CGameObject::Free();
}


