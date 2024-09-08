#include "..\..\Header\AnimTex.h"

CAnimTex::CAnimTex()
	: m_iTexWidth(0), m_iTexHeight(0), m_iCurIndex(0)
{
}

CAnimTex::CAnimTex(LPDIRECT3DDEVICE9 pGraphicDev, int _iTexWidth, int _iTexHeight)
	: CVIBuffer(pGraphicDev),
	m_iTexWidth(_iTexWidth), m_iTexHeight(_iTexHeight), m_iCurIndex(0)
{
}

CAnimTex::CAnimTex(const CAnimTex& rhs)
	: CVIBuffer(rhs),
	m_iTexWidth(rhs.m_iTexWidth), m_iTexHeight(rhs.m_iTexHeight), m_iCurIndex(rhs.m_iCurIndex)
{
}

CAnimTex::~CAnimTex()
{
}

HRESULT CAnimTex::Ready_Buffer()
{
	m_dwTriCnt = 2;
	m_dwVtxCnt = 4;
	m_dwVtxSize = sizeof(VTXTEX);
	m_dwFVF = FVF_TEX;

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	VTXTEX* pVertex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPosition = { -1.f, 1.f, 0.f };
	pVertex[0].vTexUV = { float(m_iCurIndex % m_iTexWidth) / m_iTexWidth, float(m_iCurIndex / m_iTexWidth) / m_iTexHeight };

	pVertex[1].vPosition = { 1.f, 1.f, 0.f };
	pVertex[1].vTexUV = { float((m_iCurIndex % m_iTexWidth) + 1) / m_iTexWidth, float(m_iCurIndex / m_iTexWidth) / m_iTexHeight };

	pVertex[2].vPosition = { 1.f, -1.f, 0.f };
	pVertex[2].vTexUV = { float((m_iCurIndex % m_iTexWidth) + 1) / m_iTexWidth, float((m_iCurIndex / m_iTexWidth) + 1) / m_iTexHeight };

	pVertex[3].vPosition = { -1.f, -1.f, 0.f };
	pVertex[3].vTexUV = { float(m_iCurIndex % m_iTexWidth) / m_iTexWidth, float((m_iCurIndex / m_iTexWidth) + 1) / m_iTexHeight };

	m_pVB->Unlock();

	INDEX32* pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	// 오른쪽 위
	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	// 왼쪽 아래
	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	m_pIB->Unlock();

	return S_OK;
}

void CAnimTex::Render_Buffer()
{
	VTXTEX* pVertex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vTexUV = { float(m_iCurIndex % m_iTexWidth) / m_iTexWidth, float(m_iCurIndex / m_iTexWidth) / m_iTexHeight };
	pVertex[1].vTexUV = { float((m_iCurIndex % m_iTexWidth) + 1) / m_iTexWidth, float(m_iCurIndex / m_iTexWidth) / m_iTexHeight };
	pVertex[2].vTexUV = { float((m_iCurIndex % m_iTexWidth) + 1) / m_iTexWidth, float((m_iCurIndex / m_iTexWidth) + 1) / m_iTexHeight };
	pVertex[3].vTexUV = { float(m_iCurIndex % m_iTexWidth) / m_iTexWidth, float((m_iCurIndex / m_iTexWidth) + 1) / m_iTexHeight };

	m_pVB->Unlock();
	
	CVIBuffer::Render_Buffer();
}

CAnimTex* CAnimTex::Create(LPDIRECT3DDEVICE9 pGraphicDev, int _iTexWidth, int _iTexHeight)
{
	CAnimTex* pInstance = new CAnimTex(pGraphicDev, _iTexWidth, _iTexHeight);

	if (FAILED(pInstance->Ready_Buffer()))
	{
		Safe_Release(pInstance);
		MSG_BOX("RcTex Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CAnimTex::Clone()
{
	return new CAnimTex(*this);
}

void CAnimTex::Free()
{
	CVIBuffer::Free();
}
