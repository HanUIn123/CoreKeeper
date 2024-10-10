#include "pch.h"
#include "../Header/AzeosSpawner.h"
#include "Export_System.h"
#include "Export_Utility.h"

CAzeosSpawner::CAzeosSpawner(LPDIRECT3DDEVICE9 pGraphicDev)
    : CObject(pGraphicDev)
{
}

CAzeosSpawner::~CAzeosSpawner()
{
}

HRESULT CAzeosSpawner::Ready_GameObject(_vec3 vPos)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

    return S_OK;
}

_int CAzeosSpawner::Update_GameObject(const _float& fTimeDelta)
{
    Add_RenderGroup(RENDER_ALPHA, this);

    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CAzeosSpawner::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CAzeosSpawner::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

    FAILED_CHECK_RETURN(Setup_Material(), );

    m_pTextureCom->Set_Texture(2);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CAzeosSpawner::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pBufferCom = dynamic_cast<CObjectTex*>(Engine::Clone_Proto(L"Proto_AzeosSpawnTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_SpawnerTexture"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

    return S_OK;
}

CAzeosSpawner* CAzeosSpawner::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CAzeosSpawner* pAzeosSpawner = new CAzeosSpawner(pGraphicDev);

    if (FAILED(pAzeosSpawner->Ready_GameObject(vPos)))
    {
        Safe_Release(pAzeosSpawner);
        MSG_BOX("pAzeosSpawner Create Failed");
        return nullptr;
    }

    return pAzeosSpawner;
}

void CAzeosSpawner::Free()
{
    Engine::CGameObject::Free();
}
