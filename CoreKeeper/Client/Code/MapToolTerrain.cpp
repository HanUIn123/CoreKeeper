#include "pch.h"
#include "../Header/MapToolTerrain.h"

CMapToolTerrain::CMapToolTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CGameObject(pGraphicDev)
    , m_pBufferCom(nullptr)
    , m_pTransformCom(nullptr)
    //, m_pTextureCom(nullptr)
    , m_pCalculatorCom(nullptr)
    , vPickPos(0, 0, 0)
    //, m_iTerrainImageNum(0)
{
}

CMapToolTerrain::~CMapToolTerrain()
{
}

HRESULT CMapToolTerrain::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    return S_OK;
}

_int CMapToolTerrain::Update_GameObject(const _float& fTimeDelta)
{
    Add_RenderGroup(RENDER_NONALPHA, this);

    //m_pTransformCom->Set_Pos(0, 0, 0);



    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CMapToolTerrain::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CMapToolTerrain::Render_GameObject()
{
    // 일단 기본 조명 세팅 끔 -> 이러니, 밝게 잘 나오긴함;
    //m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

    //_matrix matWorld;
    //m_pTransformCom->Get_WorldMatrix(&matWorld);

    //matWorld._41 -= 0.5f;
    //matWorld._42 = 0.0f;
    //matWorld._43 -= 0.5f;
    //m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    //m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

    // 와이어 프레임으로 출력도 걍 꺼둠
    //m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

    //FAILED_CHECK_RETURN(Setup_Material(), );

    //m_pTextureCom->Set_Texture(m_iTerrainImageNum);

    m_pBufferCom->Render_Buffer();

    //m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    //m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    //m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

HRESULT CMapToolTerrain::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pBufferCom = dynamic_cast<CMapToolTex*>(Engine::Clone_Proto(L"Proto_MapToolTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    //pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MapToolTerrainTexture"));
    //NULL_CHECK_RETURN(pComponent, E_FAIL);
    //m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Calculator", pComponent });

    return S_OK;
}

HRESULT CMapToolTerrain::Setup_Material()
{
    D3DMATERIAL9		tMtrl;
    ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

    tMtrl.Diffuse = { 1.f, 1.f, 1.f, 1.f };
    tMtrl.Specular = { 1.f, 1.f, 1.f, 1.f };
    tMtrl.Ambient = { 0.2f, 0.2f, 0.2f, 1.f };

    tMtrl.Emissive = { 0.f, 0.f, 0.f, 0.f };
    tMtrl.Power = 0.f;

    m_pGraphicDev->SetMaterial(&tMtrl);


    return S_OK;
}

_vec3 CMapToolTerrain::Picking_OnTerrain()
{   
    //return m_pCalculatorCom->Picking_OnTerrain(g_hWnd, m_pBufferCom, m_pTransformCom);
    //return _vec3(0, 0, 0);

    //return m_pCalculatorCom->Picking_OnTile(g_hWnd, m_pBufferCom, m_pTransformCom);

    _vec3 vPickPos = m_pCalculatorCom->Picking_OnTerrain(g_hWnd, m_pBufferCom, m_pTransformCom);
    return vPickPos;
    //return _vec3(0, 0, 0);
}

CMapToolTerrain* CMapToolTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMapToolTerrain* pMaptoolTerrain = new CMapToolTerrain(pGraphicDev);

    if (FAILED(pMaptoolTerrain->Ready_GameObject()))
    {
        Safe_Release(pMaptoolTerrain);
        MSG_BOX("pTerrain Create Failed");
        return nullptr;
    }

    return pMaptoolTerrain;
}

void CMapToolTerrain::Free()
{
    Engine::CGameObject::Free();
}
