#include "pch.h"
#include "../Header/AzeosPoop.h"
#include "Export_System.h"
#include "Export_Utility.h"

CAzeosPoop::CAzeosPoop(LPDIRECT3DDEVICE9 _pGraphicDev)
    : CObject(_pGraphicDev)
    , m_iTileTypeIndex(0)
{
    m_eObjType = Engine::AZEOS_POOP;
}

CAzeosPoop::~CAzeosPoop()
{
}

HRESULT CAzeosPoop::Ready_GameObject(_vec3 _vPos)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_vPoopPos.x = _vPos.x;
    m_vPoopPos.y = 0.1f;
    m_vPoopPos.z = _vPos.z;

    m_pTransformCom->Set_Pos(_vPos.x, 0.1f, _vPos.z);

    m_pTransformCom->Set_Scale(1.0f, 1.0f, 1.0f);

    return S_OK;
}

_int CAzeosPoop::Update_GameObject(const _float& fTimeDelta)
{
    Add_RenderGroup(RENDER_ALPHA, this);

    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CAzeosPoop::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CAzeosPoop::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture(m_iTileTypeIndex);

    m_pBufferCom->Render_Buffer();

    //m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CAzeosPoop::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pBufferCom = dynamic_cast<CObjectTex*>(Engine::Clone_Proto(L"Proto_AzeosPoopTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_AzeosPoopTexture"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    pComponent = m_pColliderCom = dynamic_cast<CColliderCube*>(Engine::Clone_Proto(L"Proto_WallCollider"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });
}

CAzeosPoop* CAzeosPoop::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos)
{
    CAzeosPoop* pAzeosPoop = new CAzeosPoop(pGraphicDev);

    if (FAILED(pAzeosPoop->Ready_GameObject(_vPos)))
    {
        Safe_Release(pAzeosPoop);
        MSG_BOX("pAzeosPoop Create Failed");
        return nullptr;
    }

    return pAzeosPoop;
}

void CAzeosPoop::Free()
{
    Engine::CGameObject::Free();
}
