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

HRESULT CTileTex::Ready_Buffer(const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv)
{
    m_dwVtxCnt = dwCntX * dwCntZ;
    m_dwTriCnt = (dwCntX - 1) * (dwCntZ - 1) * 2;

    m_dwVtxSize = sizeof(VTXTOOLTEX);
    m_dwFVF = FVF_TOOLTEX;

    m_dwIdxSize = sizeof(INDEX32);
    m_IdxFmt = D3DFMT_INDEX32;



    FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

    VTXTOOLTEX* pVertex = NULL;

    _ulong  dwIndex(0);

    m_pVB->Lock(0, 0, (void**)&pVertex, 0);

    for (_ulong i = 0; i < dwCntZ; ++i)
    {
        for (_ulong j = 0; j < dwCntX; ++j)
        {
            dwIndex = i * dwCntX + j;

            pVertex[dwIndex].vPosition = _vec3(_float(j) * dwVtxItv, 0.f, _float(i) * dwVtxItv);

            pVertex[dwIndex].vTextUV = _vec2(_float(j) / (dwCntX - 1), _float(i) / (dwCntZ - 1));
        }
    }

    m_pVB->Unlock();

    INDEX32* pIndex = nullptr;
    _ulong	dwTriCnt = 0;

    m_pIB->Lock(0, 0, (void**)&pIndex, 0);

    for (_ulong i = 0; i < dwCntZ - 1; ++i)
    {
        for (_ulong j = 0; j < dwCntX - 1; ++j)
        {
            dwIndex = i * dwCntX + j;

            // 오른쪽 위
            pIndex[dwTriCnt]._0 = dwIndex + dwCntX;         // 129
            pIndex[dwTriCnt]._1 = dwIndex + dwCntX + 1;     // 130
            pIndex[dwTriCnt]._2 = dwIndex + 1;              // 1 
            dwTriCnt++;

            // 왼쪽 아래
            pIndex[dwTriCnt]._0 = dwIndex + dwCntX;         // 129
            pIndex[dwTriCnt]._1 = dwIndex + 1;              // 1 
            pIndex[dwTriCnt]._2 = dwIndex;                  // 0
            dwTriCnt++;
        }
    }

    m_pIB->Unlock();

    return S_OK;
}

void CTileTex::Render_Buffer()
{
    //m_pGraphicDev->SetStreamSource(0, m_pVB, 0, m_dwVtxSize);

    //m_pGraphicDev->SetFVF(m_dwFVF);

    //m_pGraphicDev->SetIndices(m_pIB);

    ////m_pGraphicDev->SetTexture();

    //m_pGraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwVtxCnt, 0, m_dwTriCnt);

    CVIBuffer::Render_Buffer();
}

CTileTex* CTileTex::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv)
{
    CTileTex* pInstance = new CTileTex(pGraphicDev);

    if (FAILED(pInstance->Ready_Buffer(dwCntX, dwCntZ, dwVtxItv)))
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
