#include "..\..\Header\RangeTex.h"

CRangeTex::CRangeTex()
	: m_iTexWidth(0), m_iCurIndex(0), m_iCurWidth(0)
{
}

CRangeTex::CRangeTex(LPDIRECT3DDEVICE9 pGraphicDev, _int _iTexWidth)
	: CVIBuffer(pGraphicDev),
	m_iTexWidth(_iTexWidth), m_iCurIndex(0), m_iCurWidth(0)
{
}

CRangeTex::CRangeTex(const CRangeTex& rhs)
	: CVIBuffer(rhs),
	m_iTexWidth(rhs.m_iTexWidth), m_iCurIndex(rhs.m_iCurIndex), m_iCurWidth(m_iCurWidth)
{
}

CRangeTex::~CRangeTex()
{
}

HRESULT CRangeTex::Ready_Buffer()
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
	pVertex[0].vTexUV = { 0.f, 0.f };

	pVertex[1].vPosition = { (_float)m_iTexWidth, 1.f, 0.f}; // ³ÐÀÌ ÁöÁ¤
	pVertex[1].vTexUV = { 1.f, 0.f };

	pVertex[2].vPosition = { (_float)m_iTexWidth, -1.f, 0.f };
	pVertex[2].vTexUV = { 1.f, 1.f };

	pVertex[3].vPosition = { -1.f, -1.f, 0.f };
	pVertex[3].vTexUV = { 0.f, 1.f };

	m_pVB->Unlock();

	INDEX32* pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	// ¿À¸¥ÂÊ À§
	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	// ¿ÞÂÊ ¾Æ·¡
	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	m_pIB->Unlock();

	return S_OK;
}

void CRangeTex::Render_Buffer()
{
	VTXTEX* pVertex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);
	
	pVertex[1].vPosition = { (_float)m_iCurWidth  * (100.f / (_float)m_iTexWidth), 1.f, 0.f}; // ÅØ½ºÃÄ ³ÐÀÌ ¹éºÐÀ²·Î ³ª´²ÁÜ
	pVertex[1].vTexUV = { (_float)m_iCurWidth / 100.f, 0.f };
	pVertex[2].vPosition = { (_float)m_iCurWidth * (100.f / (_float)m_iTexWidth), -1.f, 0.f};
	pVertex[2].vTexUV = { (_float)m_iCurWidth / 100.f, 1.f };

	m_pVB->Unlock();
	
	CVIBuffer::Render_Buffer();
}

CRangeTex* CRangeTex::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int _iTexWidth)
{
	CRangeTex* pInstance = new CRangeTex(pGraphicDev, _iTexWidth);

	if (FAILED(pInstance->Ready_Buffer()))
	{
		Safe_Release(pInstance);
		MSG_BOX("RangeTex Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CRangeTex::Clone()
{
	return new CRangeTex(*this);
}

void CRangeTex::Free()
{
	CVIBuffer::Free();
}
