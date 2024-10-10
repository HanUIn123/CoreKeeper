#include "pch.h"
#include "../Header/Mushroom.h"
#include "Export_System.h"
#include "Export_Utility.h"

CMushroom::CMushroom(LPDIRECT3DDEVICE9 _pGraphicDev)
    : CObject(_pGraphicDev)
    , m_iMushTypeIndex(0)
{
    m_eObjType = Engine::MUSHROOM;
}

CMushroom::~CMushroom()
{
}

HRESULT CMushroom::Ready_GameObject(_vec3 _vPos, _int _iTypeNum, const wstring _pickedMushName)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
    
    m_vMushroomPos.x = _vPos.x;
    m_vMushroomPos.y = 0.6f;
    m_vMushroomPos.z = _vPos.z;

    m_iMushTypeIndex = _iTypeNum;

    m_strPickedObjectName = _pickedMushName;

    m_pTransformCom->Set_Pos(_vPos.x, 0.6f, _vPos.z);


    m_pTransformCom->Set_Scale(1.0f, 1.0f, 1.0f);

    return S_OK;
}

_int CMushroom::Update_GameObject(const _float& fTimeDelta)
{
    Add_RenderGroup(RENDER_ALPHA, this);

    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CMushroom::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CMushroom::Render_GameObject()
{
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, true);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    FAILED_CHECK_RETURN(Setup_Material(), );

    m_pTextureCom->Set_Texture(m_iMushTypeIndex);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CMushroom::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pBufferCom = dynamic_cast<CObjectTex*>(Engine::Clone_Proto(L"Proto_MushroomTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MushroomTexture"));
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

CMushroom* CMushroom::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _int _iTypeNum, const wstring _pickedMushName)
{
    CMushroom* pMushroom = new CMushroom(pGraphicDev);

    if (FAILED(pMushroom->Ready_GameObject(vPos, _iTypeNum, _pickedMushName)))
    {
        Safe_Release(pMushroom);
        MSG_BOX("pMushroom Create Failed");
        return nullptr;
    }

    return pMushroom;
}

void CMushroom::Free()
{
    Engine::CGameObject::Free();
}
