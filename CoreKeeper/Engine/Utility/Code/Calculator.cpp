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

	return _vec3(0.f, 0.f, 0.f);
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

CCalculator* CCalculator::Clone()
{
	return new CCalculator(*this);
}

void CCalculator::Free()
{
	CComponent::Free();
}
