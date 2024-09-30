#include "Export_Utility.h"

CCalculator::CCalculator(LPDIRECT3DDEVICE9 pGraphicDev)
	:CComponent(pGraphicDev)
{
}


CCalculator::~CCalculator()
{
}

HRESULT CCalculator::Ready_Calculator()
{
	return S_OK;
}

_vec3 CCalculator::Picking_OnTerrain(HWND _hWnd, CMapToolTex* _pMapToolBufferCom, CTransform* _pWireTerrainTransform)
{
	POINT	ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(_hWnd, &ptMouse);

	_vec3			vMousePos;

	D3DVIEWPORT9	ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
	m_pGraphicDev->GetViewport(&ViewPort);

	// 뷰 포트 -> 투영
	vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	// 투영 -> 뷰 스페이스
	_matrix matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	_vec3	vRayPos, vRayDir;
	vRayPos = { 0.0f, 0.0f, 0.0f };
	vRayDir = vMousePos - vRayPos;
	D3DXVec3Normalize(&vRayDir, &vRayDir);

	// 뷰 스페이스 -> 월드
	_matrix matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);

	// 월드 -> 로컬
	//_matrix	matWorld;
	//_pMapToolTerrainTransform->Get_WorldMatrix(&matWorld);
	//D3DXMatrixInverse(&matWorld, NULL, &matWorld);
	//
	//D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matWorld);
	//D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matWorld);

	const _vec3* pMapToolTerrainTexPos = _pMapToolBufferCom->Get_VtxPos();

	_ulong dwVtxId[3]{};
	_float fU, fV, fDist;

	for (_ulong i = 0; i < VTXCNTZ - 1; ++i)
	{
		for (_ulong j = 0; j < VTXCNTX - 1; ++j)
		{
			_ulong dwIndex = i * VTXCNTX + j;

			// 오른쪽 위
			dwVtxId[0] = dwIndex + VTXCNTX;
			dwVtxId[1] = dwIndex + VTXCNTX + 1;
			dwVtxId[2] = dwIndex + 1;

			if (D3DXIntersectTri(&pMapToolTerrainTexPos[dwVtxId[1]],
				&pMapToolTerrainTexPos[dwVtxId[2]],
				&pMapToolTerrainTexPos[dwVtxId[0]],
				&vRayPos, &vRayDir, &fU, &fV, &fDist))
			{
				_float x1, z1;

				x1 = pMapToolTerrainTexPos[dwVtxId[1]].x + fU * (pMapToolTerrainTexPos[dwVtxId[2]].x - pMapToolTerrainTexPos[dwVtxId[1]].x);
				z1 = pMapToolTerrainTexPos[dwVtxId[1]].z + fV * (pMapToolTerrainTexPos[dwVtxId[0]].z - pMapToolTerrainTexPos[dwVtxId[1]].z);

				return _vec3(x1 - 1, 0.f, z1 - 1);
			}

			// 왼쪽 아래
			dwVtxId[0] = dwIndex + VTXCNTX;
			dwVtxId[1] = dwIndex + 1;
			dwVtxId[2] = dwIndex;

			if (D3DXIntersectTri(&pMapToolTerrainTexPos[dwVtxId[2]],
				&pMapToolTerrainTexPos[dwVtxId[0]],
				&pMapToolTerrainTexPos[dwVtxId[1]],
				&vRayPos, &vRayDir, &fU, &fV, &fDist))
			{
				// V1 + U(V2 - V1) + V(V3 - V1)
				return _vec3(pMapToolTerrainTexPos[dwVtxId[2]].x + fU * (pMapToolTerrainTexPos[dwVtxId[0]].x - pMapToolTerrainTexPos[dwVtxId[2]].x),
					0.f,
					pMapToolTerrainTexPos[dwVtxId[2]].z + fV * (pMapToolTerrainTexPos[dwVtxId[1]].z - pMapToolTerrainTexPos[dwVtxId[2]].z));
			}
		}
	}

	return _vec3(0.f, -1.f, 0.f);
}

_bool CCalculator::Check_Distance2D(_vec3* _vPos1, _vec3* _vPos2, _float _fDist)
{
	_vec2 vDistance = { _vPos1->x - _vPos2->x, _vPos1->z - _vPos2->z };
	return D3DXVec2Length(&vDistance) < _fDist;
}

_vec3 CCalculator::Picking_OnTerrain(HWND hWnd, CTerrainTex* pTerrainBufferCom, CTransform* pTerrainTransCom)
{
	POINT	ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_vec3			vMousePos;

	D3DVIEWPORT9	ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
	m_pGraphicDev->GetViewport(&ViewPort);

	vMousePos.x = ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	_matrix matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, NULL, &matProj);
	D3DXVec3TransformCoord(&vMousePos, &vMousePos, &matProj);

	_vec3	vRayPos, vRayDir;
	vRayPos = { 0.0f, 0.0f, 0.0f };
	vRayDir = vMousePos - vRayPos;
	D3DXVec3Normalize(&vRayDir, &vRayDir);

	_matrix matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);


	const _vec3* pTerrainTexPos = pTerrainBufferCom->Get_VtxPos();

	_ulong	dwVtxId[3]{};
	_float	fU, fV, fDist;

	for (_ulong i = 0; i < VTXCNTZ - 1; ++i)
	{
		for (_ulong j = 0; j < VTXCNTX - 1; ++j)
		{
			_ulong	dwIndex = i * VTXCNTX + j;

			dwVtxId[0] = dwIndex + VTXCNTX;
			dwVtxId[1] = dwIndex + VTXCNTX + 1;
			dwVtxId[2] = dwIndex + 1;

			if (D3DXIntersectTri(&pTerrainTexPos[dwVtxId[1]],
				&pTerrainTexPos[dwVtxId[2]],
				&pTerrainTexPos[dwVtxId[0]],
				&vRayPos, &vRayDir, &fU, &fV, &fDist))
			{
				_float x1, z1;

				x1 = pTerrainTexPos[dwVtxId[1]].x + fU * (pTerrainTexPos[dwVtxId[2]].x - pTerrainTexPos[dwVtxId[1]].x);
				z1 = pTerrainTexPos[dwVtxId[1]].z + fV * (pTerrainTexPos[dwVtxId[0]].z - pTerrainTexPos[dwVtxId[1]].z);

				return _vec3(x1 - 1, 0.f, z1 - 1);
			}

			dwVtxId[0] = dwIndex + VTXCNTX;
			dwVtxId[1] = dwIndex + 1;
			dwVtxId[2] = dwIndex;

			if (D3DXIntersectTri(&pTerrainTexPos[dwVtxId[2]],
				&pTerrainTexPos[dwVtxId[0]],
				&pTerrainTexPos[dwVtxId[1]],
				&vRayPos, &vRayDir, &fU, &fV, &fDist))
			{
				// V1 + U(V2 - V1) + V(V3 - V1)
				return _vec3(pTerrainTexPos[dwVtxId[2]].x + fU * (pTerrainTexPos[dwVtxId[0]].x - pTerrainTexPos[dwVtxId[2]].x),
					0.f,
					pTerrainTexPos[dwVtxId[2]].z + fV * (pTerrainTexPos[dwVtxId[1]].z - pTerrainTexPos[dwVtxId[2]].z));
			}
		}
	}

	return _vec3(0.f, 0.f, 0.f);
}
//bool CCalculator::In_Frustum(CTransform* pTransform)
//{
//	_vec3 vWorld[8];
//	vWorld[0] = _vec3(-1.f, 1.f, 0.f);
//	vWorld[1] = _vec3(1.f, 1.f, 0.f);
//	vWorld[2] = _vec3(1.f, -1.f, 0.f);
//	vWorld[3] = _vec3(-1.f, -1.f, 0.f);
//	vWorld[4] = _vec3(-1.f, 1.f, 1.f);
//	vWorld[5] = _vec3(1.f, 1.f, 1.f);
//	vWorld[6] = _vec3(1.f, -1.f, 1.f);
//	vWorld[7] = _vec3(-1.f, -1.f, 1.f);
//
//	D3DXPLANE frustumPlane[6];
//
//	// near
//	D3DXPlaneFromPoints(&frustumPlane[0], &vWorld[0], &vWorld[1], &vWorld[2]);
//	// right
//	D3DXPlaneFromPoints(&frustumPlane[1], &vWorld[1], &vWorld[5], &vWorld[6]);
//	// up
//	D3DXPlaneFromPoints(&frustumPlane[2], &vWorld[4], &vWorld[5], &vWorld[1]);
//	// left
//	D3DXPlaneFromPoints(&frustumPlane[3], &vWorld[4], &vWorld[0], &vWorld[3]);
//	//// far
//	//D3DXPlaneFromPoints(&frustumPlane[4], &vWorld[5], &vWorld[4], &vWorld[7]);
//	//// down
//	//D3DXPlaneFromPoints(&frustumPlane[5], &vWorld[3], &vWorld[2], &vWorld[6]);
//
//	// 이게 맞나?
//	// far
//	D3DXPlaneFromPoints(&frustumPlane[4], &vWorld[4], &vWorld[5], &vWorld[6]);
//	// down
//	D3DXPlaneFromPoints(&frustumPlane[5], &vWorld[7], &vWorld[6], &vWorld[2]);
//
//	_vec3 vPos;
//	pTransform->Get_Info(INFO_POS, &vPos);
//
//	for (int i = 0; i < 6; ++i)
//	{
//		_float distance = frustumPlane[i].a * vPos.x + frustumPlane[i].b * vPos.y + frustumPlane[i].c * vPos.z + m_FrustumPlane[i].d;
//		if (distance > 1.5f)
//			return false;
//	}
//
//	return true;
//}
// 
bool CCalculator::In_Frustum(CTransform* pTransform)
{
	_matrix matView, matProj, matVP;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	// 뷰-투영 행렬 계산
	D3DXMatrixMultiply(&matVP, &matView, &matProj);

	// 절두체를 구성하는 평면들 계산
	D3DXPLANE planes[6];
	// Near Plane
	planes[0].a = matVP._14 + matVP._13;
	planes[0].b = matVP._24 + matVP._23;
	planes[0].c = matVP._34 + matVP._33;
	planes[0].d = matVP._44 + matVP._43;

	// Far Plane
	planes[1].a = matVP._14 - matVP._13; 
	planes[1].b = matVP._24 - matVP._23; 
	planes[1].c = matVP._34 - matVP._33; 
	planes[1].d = matVP._44 - matVP._43; 

	// Left Plane
	planes[2].a = matVP._14 + matVP._11; 
	planes[2].b = matVP._24 + matVP._21; 
	planes[2].c = matVP._34 + matVP._31; 
	planes[2].d = matVP._44 + matVP._41; 

	// Right Plane
	planes[3].a = matVP._14 - matVP._11; 
	planes[3].b = matVP._24 - matVP._21; 
	planes[3].c = matVP._34 - matVP._31; 
	planes[3].d = matVP._44 - matVP._41; 

	// Top Plane
	planes[4].a = matVP._14 - matVP._12; 
	planes[4].b = matVP._24 - matVP._22; 
	planes[4].c = matVP._34 - matVP._32; 
	planes[4].d = matVP._44 - matVP._42; 

	// Bottom Plane
	planes[5].a = matVP._14 + matVP._12; 
	planes[5].b = matVP._24 + matVP._22; 
	planes[5].c = matVP._34 + matVP._32; 
	planes[5].d = matVP._44 + matVP._42; 

	for (int i = 1; i < 6; ++i)
	{
		D3DXPlaneNormalize(&planes[i], &planes[i]);
	}

	_vec3 vPos;
	pTransform->Get_Info(INFO_POS, &vPos);

	// 위치와 크기를 기준으로 절두체 안에 있는지 확인
	for (int i = 1; i < 6; ++i)
	{
		if (D3DXPlaneDotCoord(&planes[i], &vPos) < -0.5f)
		{
			return false;
		}
	}

	return true;
}

CCalculator* CCalculator::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CCalculator* pCalculator = new CCalculator(pGraphicDev);

	if (FAILED(pCalculator->Ready_Calculator()))
	{
		Safe_Release(pCalculator);
		return nullptr;
	}


	return pCalculator;
}

CComponent* CCalculator::Clone()
{
	return new CCalculator(*this);
}

void CCalculator::Free()
{
	CComponent::Free();
}
