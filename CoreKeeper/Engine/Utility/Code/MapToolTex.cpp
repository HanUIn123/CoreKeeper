#include "../../Header/MapToolTex.h"

CMapToolTex::CMapToolTex() : m_pPos(nullptr)
{
}

CMapToolTex::CMapToolTex(LPDIRECT3DDEVICE9 pGraphicDev)
    : CVIBuffer(pGraphicDev), m_pPos(nullptr)
{
}

CMapToolTex::CMapToolTex(const CMapToolTex& rhs)
    : CVIBuffer(rhs), m_pPos(rhs.m_pPos)
{
}

CMapToolTex::~CMapToolTex()
{
}

HRESULT CMapToolTex::Ready_Buffer(const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv)
{
    m_dwVtxCnt = dwCntX * dwCntZ;
    m_dwTriCnt = (dwCntX - 1) * (dwCntZ - 1) * 2;

    m_dwVtxSize = sizeof(VTXTOOLTEX);
    m_dwFVF = FVF_TOOLTEX;

    m_dwIdxSize = sizeof(INDEX32);
    m_IdxFmt = D3DFMT_INDEX32;

    m_pPos = new _vec3[m_dwVtxCnt];

    FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

    VTXTOOLTEX* pVertex = NULL;

    _ulong  dwIndex(0);

    m_pVB->Lock(0, 0, (void**)&pVertex, 0);

    /*pVertex[0].vPosition = { -1.0f, 0.0f, 1.0f };
    pVertex[0].vTextUV = { 0.f,0.0f };

    pVertex[1].vPosition = { 1.f, 0.f, 1.f };
    pVertex[1].vTextUV = { 1.f, 0.f };

    pVertex[2].vPosition = { 1.f, 0.f, -1.f };
    pVertex[2].vTextUV = { 1.f, 1.f };

    pVertex[3].vPosition = { -1.f, 0.f, -1.f };
    pVertex[3].vTextUV = { 0.f, 1.f };*/

    for (_ulong i = 0; i < dwCntZ; ++i)
    {
        for (_ulong j = 0; j < dwCntX; ++j)
        {
            dwIndex = i * dwCntX + j;

            pVertex[dwIndex].vPosition = _vec3((_float(j) - 0.5f) * dwVtxItv, 0.f, (_float(i) - 0.5f) * dwVtxItv);

            pVertex[dwIndex].vTextUV = _vec2(_float(j), _float(i));

            m_pPos[dwIndex] = pVertex[dwIndex].vPosition;
        }
    }

    m_pVB->Unlock();

    INDEX32* pIndex = nullptr;
    _ulong		dwTriCnt(0);

    m_pIB->Lock(0, 0, (void**)&pIndex, 0);

    for (_ulong i = 0; i < dwCntZ - 1; ++i)
    {
        for (_ulong j = 0; j < dwCntX - 1; ++j)
        {
            dwIndex = i * dwCntX + j;

            // 오른쪽 위
            pIndex[dwTriCnt]._0 = dwIndex + dwCntX;
            pIndex[dwTriCnt]._1 = dwIndex + dwCntX + 1;
            pIndex[dwTriCnt]._2 = dwIndex + 1;
            dwTriCnt++;

            // 왼쪽 아래
            pIndex[dwTriCnt]._0 = dwIndex + dwCntX;
            pIndex[dwTriCnt]._1 = dwIndex + 1;
            pIndex[dwTriCnt]._2 = dwIndex;
            dwTriCnt++;
        }
    }

    m_pIB->Unlock();

    return S_OK;
}

void CMapToolTex::Render_Buffer()
{
    CVIBuffer::Render_Buffer();
}

CMapToolTex* CMapToolTex::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv)
{
    CMapToolTex* pInstance = new CMapToolTex(pGraphicDev);

    if (FAILED(pInstance->Ready_Buffer(dwCntX, dwCntZ, dwVtxItv)))
    {
        Safe_Release(pInstance);
        MSG_BOX("MapToolTex Create Failed");
        return nullptr;
    }

    return pInstance;
}

CComponent* CMapToolTex::Clone()
{
    return new CMapToolTex(*this);
}

void CMapToolTex::Free()
{
    if (false == m_bClone)
        Safe_Delete_Array(m_pPos);

    CVIBuffer::Free();
}
