#include "pch.h"
#include "../Header/MiniObject.h"
#include "Export_Utility.h"
#include "Export_System.h"
#include "../../Client/Header/Player.h"
#include "../../Client/Header/DynamicCamera.h"

CMiniObject::CMiniObject(LPDIRECT3DDEVICE9 pGraphicDev)
    :Engine::CGameObject(pGraphicDev)
    , m_pTransformCom(nullptr)
    , m_pTextureCom(nullptr)
    , m_pBufferCom(nullptr)
{
}

CMiniObject::~CMiniObject()
{
}

HRESULT CMiniObject::Ready_GameObject(_vec3 vPos)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

    return S_OK;
}

_int CMiniObject::Update_GameObject(const _float& fTimeDelta)
{
    CDynamicCamera* pCamera = dynamic_cast<CDynamicCamera*>(Engine::Get_GameObject(L"Layer_Environment", L"DynamicCamera"));
    CTransform* pPlayerTransform = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));

    if (CRenderer::GetInstance()->Get_ExpandMap())
    {
        //float zoomRatio = CRenderer::GetInstance()->Get_ZoomRatio();

        if (pPlayerTransform)
        {
            _vec3 vPlayerPos;
            pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

            m_pTransformCom->Set_Scale(3.0f, 3.0f, 3.0f);

            m_pTransformCom->Set_Pos((vPlayerPos.x),0,((vPlayerPos.z)));
        }
    }
    else
    {
        if (pPlayerTransform)
        {
            _vec3 vPlayerPos;
            pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

            m_pTransformCom->Set_Scale(2.0f, 2.0f, 2.0f);

            m_pTransformCom->Set_Pos((vPlayerPos.x), 0, ((vPlayerPos.z)));
        }
    }
    Add_RenderGroup(RENDER_MAP, this);

    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CMiniObject::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CMiniObject::Render_GameObject()
{
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture();

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
}

HRESULT CMiniObject::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pBufferCom = dynamic_cast<CObjectTex*>(Engine::Clone_Proto(L"Proto_MiniPlayerTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MiniPlayer"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    return S_OK;
}

CMiniObject* CMiniObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CMiniObject* pMiniObject = new CMiniObject(pGraphicDev);

    if (FAILED(pMiniObject->Ready_GameObject(vPos)))
    {
        Safe_Release(pMiniObject);
        MSG_BOX("pMiniObject Create Failed");
        return nullptr;
    }

    return pMiniObject;
}

void CMiniObject::Free()
{
    Engine::CGameObject::Free();
}
