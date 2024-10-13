#include "pch.h"
#include "../Header/SpawnerObject.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "../Header/Malugaz.h"

CSpawnerObject::CSpawnerObject(LPDIRECT3DDEVICE9 pGraphicDev)
    : CObject(pGraphicDev), m_iTextureNumber(0), m_bIsAlreadySpawn(false)
{
}

CSpawnerObject::~CSpawnerObject()
{
}

HRESULT CSpawnerObject::Ready_GameObject(_vec3 vPos, _int _iTypeNum)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_vMaluSpawnPos = vPos;

    m_iTextureNumber = _iTypeNum;

    m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

    return S_OK;
}

_int CSpawnerObject::Update_GameObject(const _float& fTimeDelta)
{
    // 말루가즈 스폰포인트와 충돌되고 있는지?
    // 충돌중이면 말루가즈 활성화
    
    if (m_bIsAlreadySpawn)
        return 0;

    if (Check_Object_Interaction())
    {
        Engine::CSoundMgr::GetInstance()->Play(L"Bell.wav", SOUND_SPAWN_MALU, 0.3f);
        Interaction();
    }

    Add_RenderGroup(RENDER_ALPHA, this);

    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CSpawnerObject::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CSpawnerObject::Render_GameObject()
{
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

    m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    FAILED_CHECK_RETURN(Setup_Material(), );

    m_pTextureCom->Set_Texture(1);

    m_pBufferCom->Render_Buffer();

    m_pColliderCom->Render_Collider();

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CSpawnerObject::Interaction()
{
    CMalugaz* pMalugaz = dynamic_cast<CMalugaz*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Malugaz"));

    pMalugaz->Set_StopDraw(false);

    m_bIsAlreadySpawn = true;
    g_bFight = true;

    //Engine::CSoundMgr::GetInstance()->StopSound(SOUND_BGM);
}

_bool CSpawnerObject::Check_Object_Interaction()
{
    Engine::CColliderCube* pMalgaLuzCollider = dynamic_cast<Engine::CColliderCube*>
        (Engine::Get_Component(ID_DYNAMIC, L"Layer_Environment", L"MalgaSummonPoint", L"Com_Collider"));

    // 말가루즈 소환진과 충돌
    if (m_pColliderCom->Check_Collision(pMalgaLuzCollider))
    {
        return true;
    }

    return false;
}

HRESULT CSpawnerObject::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pBufferCom = dynamic_cast<CObjectTex*>(Engine::Clone_Proto(L"Proto_MaluSpawnTex"));
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

    pComponent = m_pColliderCom = dynamic_cast<CColliderCube*>(Engine::Clone_Proto(L"Proto_MaluSpawnerCollider"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

    return S_OK;
}

CSpawnerObject* CSpawnerObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _int _iTypeNum)
{
    CSpawnerObject* pSpawner = new CSpawnerObject(pGraphicDev);

    if (FAILED(pSpawner->Ready_GameObject(vPos, _iTypeNum)))
    {
        Safe_Release(pSpawner);
        MSG_BOX("pSpawner Create Failed");
        return nullptr;
    }

    return pSpawner;
}

void CSpawnerObject::Free()
{
    Engine::CGameObject::Free();
}
