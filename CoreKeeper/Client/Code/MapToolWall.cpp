#include "pch.h"
#include "../Header/MapToolWall.h"
#include "Export_System.h"
#include "Export_Utility.h"

CMapToolWall::CMapToolWall(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CGameObject(pGraphicDev)
    , m_pCalculatorCom(nullptr)
    , m_pTransformCom(nullptr)
    , m_pTextureCom(nullptr)
    , m_pBufferCom(nullptr)
    , m_pColliderCom(nullptr)
{
}

CMapToolWall::~CMapToolWall()
{
}

HRESULT CMapToolWall::Ready_GameObject(_float _fWallX, _float _fWallZ, _int _iType)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_vBigWallPosition.x = _fWallX;
    m_vBigWallPosition.y = 1.0f;
    m_vBigWallPosition.z = _fWallZ;

    m_iWallType = _iType;

    m_pTransformCom->Set_Pos(_fWallX, 1.0f, _fWallZ);

    if(m_iWallType == 1)
        m_pTransformCom->Set_Scale(200.0f, 2.0f, 24.0f);
    else if(m_iWallType ==2)
        m_pTransformCom->Set_Scale(24.0f, 2.0f, 200.0f);
        
    
    return S_OK;
}

_int CMapToolWall::Update_GameObject(const _float& fTimeDelta)
{
    Add_RenderGroup(RENDER_PRIORITY, this);

    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CMapToolWall::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CMapToolWall::Render_GameObject()
{
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

    _matrix matWorld;
    m_pTransformCom->Get_WorldMatrix(&matWorld);
    m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

    matWorld._41 = m_vBigWallPosition.x;
    matWorld._42 = m_vBigWallPosition.y;
    matWorld._43 = m_vBigWallPosition.z;
    m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    FAILED_CHECK_RETURN(Setup_Material(), );


    if(m_iWallType == 1)
        m_pTextureCom->Set_Texture(0);
    else if(m_iWallType == 2)
        m_pTextureCom->Set_Texture(0);


    m_pBufferCom->Render_Buffer();

    m_pColliderCom->Render_Collider();

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CMapToolWall::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pBufferCom = dynamic_cast<CWallTex*>(Engine::Clone_Proto(L"Proto_WallTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MapToolWallCube"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    pComponent = m_pColliderCom = dynamic_cast<CColliderCube*>(Engine::Clone_Proto(L"Proto_WallCollider"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

    return S_OK;
}

HRESULT CMapToolWall::Setup_Material()
{
    D3DMATERIAL9		tMtrl;
    ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

    tMtrl.Diffuse = { 1.f, 1.f, 1.f, 1.f };
    tMtrl.Specular = { 1.f, 1.f, 1.f, 1.f };
    tMtrl.Ambient = { 0.7f, 0.7f, 0.7f, 0.7f };

    tMtrl.Emissive = { 0.01f, 0.01f, 0.01f, 0.01f };
    tMtrl.Power = 0.f;

    m_pGraphicDev->SetMaterial(&tMtrl);

    return S_OK;
}

CMapToolWall* CMapToolWall::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _fWallX, _float _fWallZ, _int _iType)
{
    CMapToolWall* pMapToolWall = new CMapToolWall(pGraphicDev);

    if (FAILED(pMapToolWall->Ready_GameObject(_fWallX, _fWallZ, _iType)))
    {
        Safe_Release(pMapToolWall);
        MSG_BOX("pMapToolWall Create Failed");
        return nullptr;
    }

    return pMapToolWall;
}

void CMapToolWall::Free()
{
    Engine::CGameObject::Free();
}
