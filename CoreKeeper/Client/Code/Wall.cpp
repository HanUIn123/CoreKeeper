#include "pch.h"
#include "../Header/Wall.h"
#include "Export_Utility.h"

CWall::CWall(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CGameObject(pGraphicDev)
    , m_pCalculatorCom(nullptr)
    , m_pTransformCom(nullptr)
    , m_pTextureCom(nullptr)
    , m_pBufferCom(nullptr)
    , m_bIsUpWall(false)
    , m_bActive(true)
{
    m_vecAroundWall.resize(8);
}

CWall::~CWall()
{
}

HRESULT CWall::Ready_GameObject(_float _fWallX, _float _fWallZ, _int iWallImageNum, const wstring _pickedWallName)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_strPickedWallName = _pickedWallName;

    m_iWallImageNum = iWallImageNum;

    m_vWallPosition.x = _fWallX;
    m_vWallPosition.y = 0.0f;
    m_vWallPosition.z = _fWallZ;

    m_pTransformCom->Set_Pos(_fWallX, 0.0f, _fWallZ);

    m_pTransformCom->m_vScale = { 1.0f, 2.0f, 1.0f };

    return S_OK;
}

_int CWall::Update_GameObject(const _float& fTimeDelta)
{
    Engine::CCollider* pPlayerCollider = dynamic_cast<Engine::CCollider*>
        (Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Collider"));

    // 플레이어와 충돌
    m_bActive = !(m_pColliderCom->Check_Sphere_Collision(pPlayerCollider));

    Update_Texture();

    Add_RenderGroup(RENDER_PRIORITY, this);

    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CWall::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();

    _matrix		matCamWorld;
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matCamWorld);
    D3DXMatrixInverse(&matCamWorld, NULL, &matCamWorld);

    //m_pTransformCom->Set_Pos(matCamWorld._41, matCamWorld._42 + 3.f, matCamWorld._43);
}

void CWall::Render_GameObject()
{
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

    _matrix matWorld;
    m_pTransformCom->Get_WorldMatrix(&matWorld);
    m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

    matWorld._41 = m_vWallPosition.x;
    matWorld._42 = m_vWallPosition.y;
    matWorld._43 = m_vWallPosition.z;
    m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

    //m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    FAILED_CHECK_RETURN(Setup_Material(), );

    m_pTextureCom->Set_Texture(m_iWallImageNum);

    if (m_bActive)
    {
        m_pBufferCom->Render_Buffer();
    }

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CWall::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pBufferCom = dynamic_cast<CWallTex*>(Engine::Clone_Proto(L"Proto_WallTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_WallCube"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    pComponent = m_pColliderCom = dynamic_cast<CColliderCube*>(Engine::Clone_Proto(L"Proto_WallCollider"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Transform", pComponent });

    return S_OK;
}

HRESULT CWall::Setup_Material()
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

void CWall::Update_Texture()
{

    //int wallMask = 0; // 벽 존재 여부를 나타내는 비트 마스크

    // 각 방향에 대해 벽 포인터를 확인
    //for (int dir = 0; dir < 8; dir++)
    //{
    //    if (m_vecAroundWall[dir] != nullptr) // 각 방향의 벽 포인터 검사
    //    {
    //        wallMask |= (1 << dir); // 비트 마스킹
    //    }
    //}

    //    // 텍스처 결정
    //    switch (wallMask)
    //    {
    //    // 벽 없음, 동그라미
    //    case 0b00000000: 
    //        break;

    //    // 하단과 연결
    //    case 0b00000010:
    //        break;
    //    //좌
    //    case 0b00001000:
    //        break; 
    //    // 우
    //    case 0b00010000:
    //        break;
    //    // 상
    //    case 0b01000000:
    //        break;

    //    //좌 하
    //    case 0b00001010:
    //    //우 하
    //    case 0b00010010:
    //    //좌 상
    //    case 0b01001000:
    //    //우 상
    //    case 0b01010000:

    //        // 상 하
    //    case 0b01000010:
    //        //좌 우
    //    case 0b00011000:	
    //        break;

    //    default:
    //        // 아예 까만거
    //        break;
    //    }
    //}
}

CWall* CWall::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _fWallX, _float _fWallZ, _int iWallImageNum,  wstring _pickedWallName)
{
    CWall* pWall = new CWall(pGraphicDev);

    if (FAILED(pWall->Ready_GameObject(_fWallX, _fWallZ, iWallImageNum, _pickedWallName)))
    {
        Safe_Release(pWall);
        MSG_BOX("pWall Create Failed");
        return nullptr;
    }

    return pWall;
}

void CWall::Free()
{
    Engine::CGameObject::Free();
}
