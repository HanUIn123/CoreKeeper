#include "pch.h"
#include "../Header/HunterRender.h"
#include "Export_System.h"
#include "Export_Utility.h"

CHunterRender::CHunterRender(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{

}

CHunterRender::~CHunterRender()
{
}

HRESULT CHunterRender::Ready_GameObject(_int iIndex, const wstring _pickedSFName)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_strPickedMonsterName = _pickedSFName;

    _float fX = (iIndex % (VTXCNTX - 1)) * VTXITV;
    _float fZ = (iIndex / (VTXCNTX - 1)) * VTXITV;

    m_pTransformCom->Set_Pos(fX, 0.8f, fZ);
    return S_OK;
}

_int CHunterRender::Update_GameObject(const _float& fTimeDelta)
{
    Add_RenderGroup(RENDER_ALPHA, this);
    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CHunterRender::LateUpdate_GameObject()
{

    Engine::CGameObject::LateUpdate_GameObject();
}

void CHunterRender::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture();

    m_pBufferCom->Set_Index(0);
    m_pBufferCom->Render_First();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CHunterRender::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_HunterAnimTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_HunterTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;

}

CHunterRender* CHunterRender::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iIndex, const wstring _pickedSFName)
{
    CHunterRender* pHunterRender = new CHunterRender(pGraphicDev);

    if (FAILED(pHunterRender->Ready_GameObject(iIndex, _pickedSFName)))
    {
        Safe_Release(pHunterRender);
        MSG_BOX("Render HunterRender Create Failed");
        return nullptr;
    }
    return pHunterRender;
}

void CHunterRender::Free()
{
    Engine::CGameObject::Free();
}