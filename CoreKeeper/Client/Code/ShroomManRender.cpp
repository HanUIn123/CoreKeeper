#include "pch.h"
#include "../Header/ShroomManRender.h"
#include "Export_System.h"
#include "Export_Utility.h"

CShroomManRender::CShroomManRender(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{

}

CShroomManRender::~CShroomManRender()
{
}

HRESULT CShroomManRender::Ready_GameObject(_int iIndex)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
    _float fX = (iIndex % (VTXCNTX - 1)) * VTXITV;
    _float fZ = (iIndex / (VTXCNTX - 1)) * VTXITV;

    m_pTransformCom->Set_Pos(fX, 0.65f, fZ);
    return S_OK;
}

_int CShroomManRender::Update_GameObject(const _float& fTimeDelta)
{
    Add_RenderGroup(RENDER_ALPHA, this);
    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CShroomManRender::LateUpdate_GameObject()
{

    Engine::CGameObject::LateUpdate_GameObject();
}

void CShroomManRender::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture();

    m_pBufferCom->Set_Index(0);
    m_pBufferCom->Render_First();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CShroomManRender::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_SlimeAnimTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_SlimeTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;

}

CShroomManRender* CShroomManRender::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iIndex)
{
    CShroomManRender* pShroomMan = new CShroomManRender(pGraphicDev);

    if (FAILED(pShroomMan->Ready_GameObject(iIndex)))
    {
        Safe_Release(pShroomMan);
        MSG_BOX("Render ShroomMan Create Failed");
        return nullptr;
    }
    return pShroomMan;
}

void CShroomManRender::Free()
{
    Engine::CGameObject::Free();
}