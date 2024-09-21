#include "pch.h"
#include "../Header/Tile.h"
#include "Export_System.h"
#include "Export_Utility.h"


CTile::CTile(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CGameObject(pGraphicDev)
    , m_iTileImageNum(0)
    , m_pTransformCom(nullptr)
    , m_pTextureCom(nullptr)
    , m_pCalculatorCom(nullptr)
    , m_pTileTexCom(nullptr)
    , m_pMapToolBufferCom(nullptr)
{
}

CTile::~CTile()
{
}

HRESULT CTile::Ready_GameObject(_float _fTileX, _float _fTileZ, _int iTileImageNum)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_vTilePosition.x = _fTileX;
    m_vTilePosition.y = 0.0f;
    m_vTilePosition.z = _fTileZ;

    m_pTransformCom->Set_Pos(_fTileX, 0.0f, _fTileZ);

    m_iTileImageNum = iTileImageNum;

    return S_OK;
}

_int CTile::Update_GameObject(const _float& fTimeDelta)
{
    // 이 Set_Pos를 통해서, 최초 피킹 시, 해당 타일의 그 위치에, Set_Pos가 안된다 -> Render에서 
    // 직접적으로 행렬 원소에 피킹 좌표 주고 있음.
    //m_pTransformCom->Set_Pos(m_pTransformCom->m_vInfo->x, m_pTransformCom->m_vInfo->y, m_pTransformCom->m_vInfo->z);
    Add_RenderGroup(RENDER_NONALPHA, this);

    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CTile::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CTile::Render_GameObject()
{
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

    _matrix matWorld;
    m_pTransformCom->Get_WorldMatrix(&matWorld);

    matWorld._41 = m_vTilePosition.x;
    matWorld._42 = m_vTilePosition.y + 0.01f;
    matWorld._43 = m_vTilePosition.z;
    m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
    
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    FAILED_CHECK_RETURN(Setup_Material(), );

    m_pTextureCom->Set_Texture(m_iTileImageNum);

    m_pTileTexCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CTile::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    //m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });
    m_mapComponent[ID_STATIC].insert({ L"Com_Transform", pComponent });

    pComponent = m_pTileTexCom = dynamic_cast<CTileTex*>(Engine::Clone_Proto(L"Proto_TileTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_TileTex", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_BasicTileTexture"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    //m_mapComponent[ID_DYNAMIC].insert({ L"Com_Calculator", pComponent });
    m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

    return S_OK;
}

HRESULT CTile::Setup_Material()
{
    D3DMATERIAL9		tMtrl;
    ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

    tMtrl.Diffuse = { 1.f, 1.f, 1.f, 1.f };
    tMtrl.Specular = { 1.f, 1.f, 1.f, 1.f };
    tMtrl.Ambient = { 0.7f, 0.7f, 0.7f, 1.f };

    tMtrl.Emissive = { 0.2f, 0.2f, 0.2f, 0.2f };
    tMtrl.Power = 0.f;

    m_pGraphicDev->SetMaterial(&tMtrl);

    return S_OK;
}

CTile* CTile::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _fTileX, _float _fTileZ, _int iTileImageNum)
{
    CTile* pTile = new CTile(pGraphicDev);

    if (FAILED(pTile->Ready_GameObject(_fTileX,_fTileZ, iTileImageNum)))
    {
        Safe_Release(pTile);
        MSG_BOX("Failed Create Tile");
        return nullptr;
    }
    return pTile;
}

void CTile::Free()
{
    Engine::CGameObject::Free();
}
