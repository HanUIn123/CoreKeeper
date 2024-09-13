#include "..\..\Header\ColliderCol.h"

CColliderCol::CColliderCol()
{
}

CColliderCol::CColliderCol(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev), m_fRadius(0.f)
{
}

CColliderCol::CColliderCol(const CColliderCol & rhs)
	:CVIBuffer(rhs)
{
}


CColliderCol::~CColliderCol()
{
}

HRESULT CColliderCol::Ready_Buffer(float _fRadius)
{
	m_dwTriCnt = 2;
	m_dwVtxCnt = 4;
	m_dwVtxSize = sizeof(VTXCOL);
	m_dwFVF = FVF_COL;

	m_dwIdxSize = sizeof(INDEX32);
	m_IdxFmt = D3DFMT_INDEX32;

	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	VTXCOL*		pVertex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPosition = { -_fRadius, _fRadius, 0.f };
	pVertex[0].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	pVertex[1].vPosition = { _fRadius, _fRadius, 0.f };
	pVertex[1].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	pVertex[2].vPosition = { _fRadius, -_fRadius, 0.f };
	pVertex[2].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	pVertex[3].vPosition = { -_fRadius, -_fRadius, 0.f };
	pVertex[3].dwColor = D3DXCOLOR(0.f, 1.f, 0.f, 1.f);

	m_pVB->Unlock();

	INDEX32*	pIndex = nullptr;

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

void CColliderCol::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

CColliderCol * CColliderCol::Create(LPDIRECT3DDEVICE9 pGraphicDev, float _fRadius)
{
	CColliderCol *	pRcCol = new CColliderCol(pGraphicDev);

	if (FAILED(pRcCol->Ready_Buffer(_fRadius)))
	{
		Safe_Release(pRcCol);
		MSG_BOX("ColliderCol Create Failed");
		return nullptr;
	}
	
	return pRcCol;
}

CComponent * CColliderCol::Clone()
{
	return new CColliderCol(*this);
}

void CColliderCol::Free()
{
	CVIBuffer::Free();
}
