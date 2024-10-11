#include "pch.h"
#include "../Header/AzeosSpawnPoint.h"
#include "Export_System.h"
#include "Export_Utility.h"

CAzeosSpawnPoint::CAzeosSpawnPoint(LPDIRECT3DDEVICE9 pGraphicDev)
    : CObject(pGraphicDev), m_iTextureNum(0)
{
}

CAzeosSpawnPoint::~CAzeosSpawnPoint()
{
}

HRESULT CAzeosSpawnPoint::Ready_GameObject(_vec3 vPos)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_vAzeSpawnPos = vPos;

    m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

    return S_OK;
}

_int CAzeosSpawnPoint::Update_GameObject(const _float& fTimeDelta)
{
    Add_RenderGroup(RENDER_ALPHA, this);

    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CAzeosSpawnPoint::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CAzeosSpawnPoint::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

    m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

    FAILED_CHECK_RETURN(Setup_Material(), );

    m_pTextureCom->Set_Texture(0);

    m_pBufferCom->Render_Buffer();

    m_pColliderCom->Render_Collider();

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CAzeosSpawnPoint::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pBufferCom = dynamic_cast<CObjectTex*>(Engine::Clone_Proto(L"Proto_AzeosSpawnPointTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_AzeosSpawnPointTexture"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

    pComponent = m_pColliderCom = dynamic_cast<CColliderCube*>(Engine::Clone_Proto(L"Proto_AzeosSpawnPointCollider"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    return S_OK;
}

CAzeosSpawnPoint* CAzeosSpawnPoint::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CAzeosSpawnPoint* pAzeSpawn = new CAzeosSpawnPoint(pGraphicDev);

    if (FAILED(pAzeSpawn->Ready_GameObject(vPos)))
    {
        Safe_Release(pAzeSpawn);
        MSG_BOX("pAzeSpawn Create Failed");
        return nullptr;
    }

    return pAzeSpawn;
}

void CAzeosSpawnPoint::Free()
{
    Engine::CGameObject::Free();
}

