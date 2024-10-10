#include "pch.h"
#include "../Header/SpawnerObject.h"
#include "Export_System.h"
#include "Export_Utility.h"

CSpawnerObject::CSpawnerObject(LPDIRECT3DDEVICE9 pGraphicDev)
    : CObject(pGraphicDev), m_iTextureNumber(0)
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
    Add_RenderGroup(RENDER_ALPHA, this);

    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CSpawnerObject::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CSpawnerObject::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

    FAILED_CHECK_RETURN(Setup_Material(), );

    m_pTextureCom->Set_Texture(1);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
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
