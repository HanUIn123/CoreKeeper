#include "pch.h"
#include "../Header/AzeosFeather.h"
#include "Export_System.h"
#include "Export_Utility.h"

CAzeosFeather::CAzeosFeather(LPDIRECT3DDEVICE9 _pGraphicDev)
    : CObject(_pGraphicDev)
    , m_iFeatherTypeIndex(0)
{
}

CAzeosFeather::~CAzeosFeather()
{
}

HRESULT CAzeosFeather::Ready_GameObject(_vec3 _vPos, _int _iTypeNum, const wstring _pickedFeatherName)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_vPoopPos.x = _vPos.x;
    m_vPoopPos.y = 0.1f;
    m_vPoopPos.z = _vPos.z;

    m_iFeatherTypeIndex = _iTypeNum;

    m_strPickedObjectName = _pickedFeatherName;

    m_pTransformCom->Set_Pos(_vPos.x, 0.1f, _vPos.z);

    m_pTransformCom->Set_Scale(1.0f, 1.0f, 1.0f);

    return S_OK;
}

_int CAzeosFeather::Update_GameObject(const _float& fTimeDelta)
{
    Add_RenderGroup(RENDER_ALPHA, this);

    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CAzeosFeather::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CAzeosFeather::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture(m_iFeatherTypeIndex);

    m_pBufferCom->Render_Buffer();

    //m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CAzeosFeather::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pBufferCom = dynamic_cast<CObjectTex*>(Engine::Clone_Proto(L"Proto_AzeosFeatherTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_AzeosFeatherTexture"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    pComponent = m_pColliderCom = dynamic_cast<CColliderCube*>(Engine::Clone_Proto(L"Proto_WallCollider"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

    return S_OK;
}

CAzeosFeather* CAzeosFeather::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos, _int _iTypeNum, const wstring _pickedFeatherName)
{
    CAzeosFeather* pAzeosFeather = new CAzeosFeather(pGraphicDev);

    if (FAILED(pAzeosFeather->Ready_GameObject(_vPos, _iTypeNum, _pickedFeatherName)))
    {
        Safe_Release(pAzeosFeather);
        MSG_BOX("pAzeosFeather Create Failed");
        return nullptr;
    }

    return pAzeosFeather;
}

void CAzeosFeather::Free()
{
    Engine::CGameObject::Free();
}
