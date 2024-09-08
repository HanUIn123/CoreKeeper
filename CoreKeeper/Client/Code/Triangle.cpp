#include "pch.h"
#include "../Header/Triangle.h"

CTriangle::CTriangle(CGraphicDev* _pDeviceClass)
    : m_pDeviceClass(_pDeviceClass)
    , m_pGraphicDev(_pDeviceClass->Get_GraphicDev())
    , m_pVertexBuffer(nullptr)
{
    m_pDeviceClass->AddRef();
    m_pGraphicDev->AddRef();
}

CTriangle::~CTriangle()
{
}

HRESULT CTriangle::Ready_Triangle()
{
    Engine::VTXCOL vertices[] =
    {
        { D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DCOLOR_XRGB(0, 0, 0) }, 
        { D3DXVECTOR3(1.0f, -1.0f, 0.0f), D3DCOLOR_XRGB(0, 0, 0) }, 
        { D3DXVECTOR3(-1.0f, -1.0f, 0.0f), D3DCOLOR_XRGB(0, 0, 0) }
    };

    HRESULT hr = m_pGraphicDev->CreateVertexBuffer(
        sizeof(vertices),
        0,
        Engine::FVF_COL,
        D3DPOOL_MANAGED,
        &m_pVertexBuffer,
        0
    );

    if (FAILED(hr))
        return E_FAIL;

    void* pVertices;

    hr = m_pVertexBuffer->Lock(0, sizeof(vertices), (void**)&pVertices, 0);
    if (FAILED(hr))
        return E_FAIL;

    memcpy(pVertices, vertices, sizeof(vertices));
    m_pVertexBuffer->Unlock();

    return S_OK;
}

int CTriangle::Update_Triangle()
{
    return 0;
}

void CTriangle::LateUpdate_Triangle()
{
}

void CTriangle::Render_Triangle()
{
    m_pGraphicDev->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(Engine::VTXCOL));
    m_pGraphicDev->SetFVF(Engine::FVF_COL);

    m_pGraphicDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
}

CTriangle* CTriangle::Create(CGraphicDev* _pDeviceClass)
{
    CTriangle* pTriangle = new CTriangle(_pDeviceClass);

    if (FAILED(pTriangle->Ready_Triangle()))
    {
        Safe_Release(pTriangle);
    }

    return pTriangle;
}

void CTriangle::Free()
{
    Safe_Release(m_pVertexBuffer);
    Safe_Release(m_pGraphicDev);
    Safe_Release(m_pDeviceClass);
}