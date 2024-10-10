#include "pch.h"
#include "../Header/SlimeFloor.h"
#include "Export_System.h"
#include "Export_Utility.h"

CSlimeFloor::CSlimeFloor(LPDIRECT3DDEVICE9 _pGraphicDev)
    : CObject(_pGraphicDev)
    , m_iTileTypeIndex(0)
{
    m_eObjType = Engine::SLIME_FLOOR;
}

CSlimeFloor::~CSlimeFloor()
{
}

HRESULT CSlimeFloor::Ready_GameObject(_vec3 _vPos, _int _iTypeNum, const wstring _pickedSFName)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);


    m_vFloorPos.x = _vPos.x;
    m_vFloorPos.y = 0.1f;
    m_vFloorPos.z = _vPos.z;

    m_iTileTypeIndex = _iTypeNum;

    m_strPickedObjectName = _pickedSFName;

    m_pTransformCom->Set_Pos(_vPos.x, 0.1f, _vPos.z);

    m_pTransformCom->Set_Scale(1.0f, 1.0f, 1.0f);

    return S_OK;
}

_int CSlimeFloor::Update_GameObject(const _float& fTimeDelta)
{
    Add_RenderGroup(RENDER_ALPHA, this);

    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CSlimeFloor::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CSlimeFloor::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture(m_iTileTypeIndex);

    m_pBufferCom->Render_Buffer();

    //m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CSlimeFloor::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pBufferCom = dynamic_cast<CObjectTex*>(Engine::Clone_Proto(L"Proto_SlimeFloorTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_SlimeFloorTexture"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

    pComponent = m_pColliderCom = dynamic_cast<CColliderCube*>(Engine::Clone_Proto(L"Proto_WallCollider"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

    return S_OK;
}

CSlimeFloor* CSlimeFloor::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos, _int _iTypeNum, const wstring _pickedSFName)
{
    CSlimeFloor* pSlimeFloor = new CSlimeFloor(pGraphicDev);

    if (FAILED(pSlimeFloor->Ready_GameObject(_vPos, _iTypeNum, _pickedSFName)))
    {
        Safe_Release(pSlimeFloor);
        MSG_BOX("pSlimeFloor Create Failed");
        return nullptr;
    }

    return pSlimeFloor;
}

void CSlimeFloor::Free()
{
    Engine::CGameObject::Free();
}
