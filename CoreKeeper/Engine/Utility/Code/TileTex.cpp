#include "../../Header/TileTex.h"

CTileTex::CTileTex()
{
}

CTileTex::CTileTex(LPDIRECT3DDEVICE9 pGraphicDev)
    : CVIBuffer(pGraphicDev)
{
}

CTileTex::CTileTex(const CTileTex& rhs)
    : CVIBuffer(rhs)
{
}

CTileTex::~CTileTex()
{
}

HRESULT CTileTex::Ready_Buffer()
{
    m_dwVtxCnt = 4;
    m_dwTriCnt = 2;

    m_dwVtxSize = sizeof(VTXTOOLTEX);
    m_dwFVF = FVF_TOOLTEX;

    m_dwIdxSize = sizeof(INDEX32);
    m_IdxFmt = D3DFMT_INDEX32;



    FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

    VTXTOOLTEX* pVertex = NULL;

    _ulong  dwIndex(0);

    m_pVB->Lock(0, 0, (void**)&pVertex, 0);

    pVertex[0].vPosition = { -0.5f, 0.0f, 0.5f };
    pVertex[0].vTextUV = { 0.f, 0.f };

    pVertex[1].vPosition = { 0.5f, 0.f, 0.5f };
    pVertex[1].vTextUV = { 1.f, 0.f };

    pVertex[2].vPosition = { 0.5f, 0.f, -0.5f };
    pVertex[2].vTextUV = { 1.f, 1.f };

    pVertex[3].vPosition = { -0.5f, 0.f, -0.5f };
    pVertex[3].vTextUV = { 0.f, 1.f };

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

void CTileTex::Render_Buffer()
{
    CVIBuffer::Render_Buffer();
}

CTileTex* CTileTex::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CTileTex* pInstance = new CTileTex(pGraphicDev);

    if (FAILED(pInstance->Ready_Buffer()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CTileTex Create Failed");
        return nullptr;
    }

    return pInstance;
}

CComponent* CTileTex::Clone()
{
    return new CTileTex(*this);
}

void CTileTex::Free()
{
    CVIBuffer::Free();
}
