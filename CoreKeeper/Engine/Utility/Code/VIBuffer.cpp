#include "..\..\Header\VIBuffer.h"



CVIBuffer::CVIBuffer()
	: m_pVB(nullptr), m_pIB(nullptr), m_dwFVF(0), m_dwTriCnt(0), m_dwVtxCnt(0), m_dwVtxSize(0), m_dwIdxSize(0)
{
}

CVIBuffer::CVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev), m_pVB(nullptr), m_pIB(nullptr), m_dwFVF(0), m_dwTriCnt(0), m_dwVtxCnt(0), m_dwVtxSize(0), m_dwIdxSize(0)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer & rhs)
	: CComponent(rhs), m_pVB(rhs.m_pVB), m_pIB(rhs.m_pIB),
	m_dwFVF(rhs.m_dwFVF), m_dwTriCnt(rhs.m_dwTriCnt), m_dwVtxCnt(rhs.m_dwVtxCnt), m_dwVtxSize(rhs.m_dwVtxSize)
	, m_dwIdxSize(rhs.m_dwIdxSize), m_IdxFmt(rhs.m_IdxFmt)
{
	m_pVB->AddRef();
}


CVIBuffer::~CVIBuffer()
{
}

HRESULT CVIBuffer::Ready_Buffer()
{
	// 버텍스 버퍼를 생성하는 함수
	FAILED_CHECK_RETURN(m_pGraphicDev->CreateVertexBuffer(m_dwVtxCnt * m_dwVtxSize,	// 버텍스 버퍼의 크기
															0,						// 사용 용도(0인 경우 정적 버퍼)
															m_dwFVF,				// 버텍스의 속성 값
															D3DPOOL_MANAGED,
															&m_pVB, NULL), E_FAIL);

	FAILED_CHECK_RETURN(m_pGraphicDev->CreateIndexBuffer(m_dwTriCnt * m_dwIdxSize,	// 인덱스 버퍼의 크기
															0,						// 사용 용도(0인 경우 정적 버퍼)
															m_IdxFmt,				// 인덱스의 속성 값
															D3DPOOL_MANAGED,
															&m_pIB, NULL), E_FAIL);


	return S_OK;
}

void CVIBuffer::Render_Buffer()
{
	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);

	m_pGraphicDev->SetFVF(m_dwFVF);

	//m_pGraphicDev->DrawPrimitive(D3DPT_LINELIST, 0, m_dwTriCnt);

	m_pGraphicDev->SetIndices(m_pIB);
	m_pGraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwVtxCnt, 0, m_dwTriCnt);
}

void CVIBuffer::Free()
{
	Safe_Release(m_pIB);
	Safe_Release(m_pVB);

	CComponent::Free();
}
