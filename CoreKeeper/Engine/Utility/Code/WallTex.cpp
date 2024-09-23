#include "../../Header/WallTex.h"

CWallTex::CWallTex()
{
}

CWallTex::CWallTex(LPDIRECT3DDEVICE9 pGraphicDev)
    : CVIBuffer(pGraphicDev)
{
}

CWallTex::CWallTex(const CWallTex& rhs)
    : CVIBuffer(rhs)
{
}

CWallTex::~CWallTex()
{
}

HRESULT CWallTex::Ready_Buffer()
{
	m_dwTriCnt = 12;
	m_dwVtxCnt = 8;
	m_dwVtxSize = sizeof(VTXCUBE);
	m_dwFVF = FVF_CUBE;

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	VTXCUBE* pVertex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	//_vec3	vMinusRatio = { -1.0f,-1.0f,-1.0f };
	// 전면
	pVertex[0].vPosition = { -0.5f, 0.5f, -0.5f };
	pVertex[0].vTexUV = { -1.f, 1.f, -1.f };

	pVertex[1].vPosition = { 0.5f, 0.5f, -0.5f };
	pVertex[1].vTexUV = { 1.f, 1.f, -1.f };

	pVertex[2].vPosition = { 0.5f, -0.5f, -0.5f };
	pVertex[2].vTexUV = { 1.f, -1.f, -1.f };

	pVertex[3].vPosition = { -0.5f, -0.5f, -0.5f };
	pVertex[3].vTexUV = { -1.f, -1.f, -1.f };

	// 후면
	pVertex[4].vPosition = { -0.5f, 0.5f, 0.5f };
	pVertex[4].vTexUV = { -1.f, 1.f, 1.f };

	pVertex[5].vPosition = { 0.5f, 0.5f, 0.5f };
	pVertex[5].vTexUV = { 1.f, 1.f, 1.f };

	pVertex[6].vPosition = { 0.5f, -0.5f, 0.5f };
	pVertex[6].vTexUV = { 1.f, -1.f, 1.f };

	pVertex[7].vPosition = { -0.5f, -0.5f, 0.5f };
	pVertex[7].vTexUV = { -1.f, -1.f, 1.f };

	m_pVB->Unlock();

	INDEX32* pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	// X+
	// 오른쪽 위
	pIndex[0]._0 = 1;
	pIndex[0]._1 = 5;
	pIndex[0]._2 = 6;

	// 왼쪽 아래
	pIndex[1]._0 = 1;
	pIndex[1]._1 = 6;
	pIndex[1]._2 = 2;

	// X-
	// 오른쪽 위
	pIndex[2]._0 = 4;
	pIndex[2]._1 = 0;
	pIndex[2]._2 = 3;

	// 왼쪽 아래
	pIndex[3]._0 = 4;
	pIndex[3]._1 = 3;
	pIndex[3]._2 = 7;

	// Y+
	// 오른쪽 위
	pIndex[4]._0 = 4;
	pIndex[4]._1 = 5;
	pIndex[4]._2 = 1;

	// 왼쪽 아래
	pIndex[5]._0 = 4;
	pIndex[5]._1 = 1;
	pIndex[5]._2 = 0;

	// Y-
	// 오른쪽 위
	pIndex[6]._0 = 3;
	pIndex[6]._1 = 2;
	pIndex[6]._2 = 6;

	// 왼쪽 아래
	pIndex[7]._0 = 3;
	pIndex[7]._1 = 6;
	pIndex[7]._2 = 7;

	// Z+
	// 오른쪽 위
	pIndex[8]._0 = 7;
	pIndex[8]._1 = 6;
	pIndex[8]._2 = 5;

	// 왼쪽 아래
	pIndex[9]._0 = 7;
	pIndex[9]._1 = 5;
	pIndex[9]._2 = 4;

	// Z-
	// 오른쪽 위
	pIndex[10]._0 = 0;
	pIndex[10]._1 = 1;
	pIndex[10]._2 = 2;

	// 왼쪽 아래
	pIndex[11]._0 = 0;
	pIndex[11]._1 = 2;
	pIndex[11]._2 = 3;

	m_pIB->Unlock();

	return S_OK;
}

void CWallTex::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

CWallTex* CWallTex::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CWallTex* pInstance = new CWallTex(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
	{
		Safe_Release(pInstance);
		MSG_BOX("WallTex Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CWallTex::Clone()
{
	return new CWallTex(*this);
}

void CWallTex::Free()
{
	CVIBuffer::Free();
}
