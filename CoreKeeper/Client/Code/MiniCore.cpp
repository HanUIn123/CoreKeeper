#include "pch.h"
#include "../Header/MiniCore.h"
#include "Export_System.h"
#include "Export_Utility.h"

CMiniCore::CMiniCore(LPDIRECT3DDEVICE9 pGraphicDev)
    :Engine::CGameObject(pGraphicDev)
    , m_pTransformCom(nullptr)
    , m_pTextureCom(nullptr)
    , m_pBufferCom(nullptr)
{
}

CMiniCore::~CMiniCore()
{
}

HRESULT CMiniCore::Ready_GameObject(_vec3 vPos)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

    m_pTransformCom->Set_Scale(4.0f, 0.0f, 4.0f);

    return S_OK;
}

_int CMiniCore::Update_GameObject(const _float& fTimeDelta)
{
    CTransform* pCoreTransform = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"MiniCore", L"Com_Transform"));

    if (CRenderer::GetInstance()->Get_ExpandMap())
    {
        if (pCoreTransform)
        {
            m_pTransformCom->Set_Scale(4.0f, 0.0f, 4.0f);
        }
    }
    else
    {
        if (pCoreTransform)
        {
            m_pTransformCom->Set_Scale(5.0f, 0.0f, 5.0f);
        }
    }
    Add_RenderGroup(RENDER_MAP, this);

    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CMiniCore::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CMiniCore::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture();

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CMiniCore::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pBufferCom = dynamic_cast<CObjectTex*>(Engine::Clone_Proto(L"Proto_MiniCoreTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MiniCore"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    return S_OK;
}

CMiniCore* CMiniCore::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CMiniCore* pMiniCore = new CMiniCore(pGraphicDev);

    if (FAILED(pMiniCore->Ready_GameObject(vPos)))
    {
        Safe_Release(pMiniCore);
        MSG_BOX("pMiniCore Create Failed");
        return nullptr;
    }

    return pMiniCore;
}

void CMiniCore::Free()
{
    Engine::CGameObject::Free();
}
