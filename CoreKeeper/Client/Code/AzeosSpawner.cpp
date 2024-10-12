#include "pch.h"
#include "../Header/AzeosSpawner.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "../Header/Azeos.h"

CAzeosSpawner::CAzeosSpawner(LPDIRECT3DDEVICE9 pGraphicDev)
    : CObject(pGraphicDev), m_bIsAlreadySpawn(false)
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
    if (m_bIsAlreadySpawn)
        return 0;

    if (Check_Object_Interaction())
    {
        Engine::CSoundMgr::GetInstance()->Play(L"Bell.wav", SOUND_SPAWN_AZEOS, 0.3f);
        Interaction();
    }

    Add_RenderGroup(RENDER_ALPHA, this);

    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CAzeosSpawner::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CAzeosSpawner::Render_GameObject()
{
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

    m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    FAILED_CHECK_RETURN(Setup_Material(), );

    m_pTextureCom->Set_Texture(2);

    m_pBufferCom->Render_Buffer();

    m_pColliderCom->Render_Collider();

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CAzeosSpawner::Interaction()
{
    CAzeos* pAzeos = dynamic_cast<CAzeos*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Azeos"));

    pAzeos->Set_StopDraw(false);

    m_bIsAlreadySpawn = true;
    g_bFight = true;

    Engine::CSoundMgr::GetInstance()->StopSound(SOUND_BGM);
    Engine::CSoundMgr::GetInstance()->PlayBGM(L"Malguaz_the_Corrupted_Shaman_R1.wav", 0.1f);
}

_bool CAzeosSpawner::Check_Object_Interaction()
{
    Engine::CColliderCube* pAzeosCollider = dynamic_cast<Engine::CColliderCube*>
        (Engine::Get_Component(ID_DYNAMIC, L"Layer_Environment", L"AzeosSummonPoint", L"Com_Collider"));

    // 말가루즈 소환진과 충돌
    if (m_pColliderCom->Check_Collision(pAzeosCollider))
    {
        return true;
    }

    return false;
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

    pComponent = m_pColliderCom = dynamic_cast<CColliderCube*>(Engine::Clone_Proto(L"Proto_AzeosSpawnerCollider"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

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
