#include "pch.h"
#include "../Header/Wall.h"
#include "Export_Utility.h"
#include "../Header/Ore.h"
#include "../Header/WallPiece.h"

_long CWall::m_iItemNumber = 0;

CWall::CWall(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CGameObject(pGraphicDev)
    , m_pCalculatorCom(nullptr)
    , m_pTransformCom(nullptr)
    , m_pTextureCom(nullptr)
    , m_pBufferCom(nullptr)
    , m_bIsUpWall(false)
    , m_bActive(true)
    , m_iCurImgNum(0)
    , m_bWallDestroyed(false)
    , m_iDurability(0)
{
    m_vecAroundWall.resize(8);
    m_vecItemName.reserve(100);
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
    m_vWallPosition.y = 1.0f;
    m_vWallPosition.z = _fWallZ;

    if (m_iWallImageNum <= 14)
        m_iDurability = 3;
    else if (m_iWallImageNum <= 29)
        m_iDurability = 6;
    else
        m_iDurability = 9;

    m_pTransformCom->Set_Pos(_fWallX, 1.0f, _fWallZ);

    m_pTransformCom->Set_Scale(1.0f, 2.0f, 1.0f);

    m_pDustParticlesCom->init(L"../Bin/Resource/Texture/Particle/Basic_Particle.png");

    return S_OK;
}

_int CWall::Update_GameObject(const _float& fTimeDelta)
{
    if (m_bWallDestroyed)
    {

    }

    Update_Texture();

    Engine::Add_RenderGroup(RENDER_PRIORITY, this);

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
    if (!m_pCalculatorCom->In_Frustum(m_pTransformCom))
        return;

    if (m_bWallDestroyed)
        return;

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

    if (m_iWallImageNum > 44)
        m_pTextureCom->Set_Texture(m_iWallImageNum);
    else
        m_pTextureCom->Set_Texture(m_iCurImgNum);

    //m_pTextureCom->Set_Texture(m_iWallImageNum);
    //m_pTextureCom->Set_Texture(m_iWallImageNum * 15);
    //m_pTextureCom->Set_Texture(2);

    m_pBufferCom->Render_Buffer();

    m_pColliderCom->Render_Collider();

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CWall::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pBufferCom = dynamic_cast<CWallTex*>(Engine::Clone_Proto(L"Proto_WallTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_DarkWallCube"));
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

    pComponent = m_pDustParticlesCom = dynamic_cast<CFall*>(Engine::Clone_Proto(L"Proto_DirtFall"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Particle", pComponent });

    return S_OK;
}

HRESULT CWall::Setup_Material()
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

void CWall::Update_Texture()
{
    int wallMask = 0;
    m_pTransformCom->Set_Angle(0.f, 0.f, 0.f);

    //각 방향에 대해 벽 포인터를 확인
    for (int dir = 0; dir < 8; dir++)
    {
        // 각 방향의 벽 포인터 검사
        if (m_vecAroundWall[dir] != nullptr)
        {
            wallMask |= (1 << dir); // 비트 마스킹
        }
    }

    switch (0b01010101 & wallMask)
    {
        // 상하좌우에 모두 벽이 없음
    case 0b00000000:
        m_iCurImgNum = m_iWallImageNum;
        break;

        // 좌 상 우 하
    case 0b00000001:
        m_iCurImgNum = m_iWallImageNum + 1;
        m_pTransformCom->Set_Angle(0.f, D3DXToRadian(90.f), 0.f);
        break;
    case 0b00000100:
        m_iCurImgNum = m_iWallImageNum + 1;
        break;
    case 0b00010000:
        m_iCurImgNum = m_iWallImageNum + 1;
        m_pTransformCom->Set_Angle(0.f, D3DXToRadian(-90.f), 0.f);
        break;
    case 0b01000000:
        m_iCurImgNum = m_iWallImageNum + 1;
        m_pTransformCom->Set_Angle(0.f, D3DXToRadian(180.f), 0.f);
        break;
        // 상하
    case 0b00010001:
        m_iCurImgNum = m_iWallImageNum + 2;
        m_pTransformCom->Set_Angle(0.f, D3DXToRadian(90.f), 0.f);
        break;
        // 좌우
    case 0b01000100:
        m_iCurImgNum = m_iWallImageNum + 2;
        break;

        // 우 하
    case 0b00000101:
        if ((wallMask & 0b00000111) == 0b00000111)
        {
            m_iCurImgNum = m_iWallImageNum + 4;
        }
        else
        {
            m_iCurImgNum = m_iWallImageNum + 3;
        }
        break;
        // 상 우
    case 0b00010100:
        if ((wallMask & 0b00011100) == 0b00011100)
        {
            m_iCurImgNum = m_iWallImageNum + 4;
            m_pTransformCom->Set_Angle(0.f, D3DXToRadian(-90.f), 0.f);
        }
        else
        {
            m_iCurImgNum = m_iWallImageNum + 3;
            m_pTransformCom->Set_Angle(0.f, D3DXToRadian(-90.f), 0.f);
        }
        break;
        // 좌 상
    case 0b01010000:
        if ((wallMask & 0b01110000) == 0b01110000)
        {
            m_iCurImgNum = m_iWallImageNum + 4;
        }
        else
        {
            m_iCurImgNum = m_iWallImageNum + 3;
        }
        m_pTransformCom->Set_Angle(0.f, D3DXToRadian(180.f), 0.f);
        break;
        // 좌 하
    case 0b01000001:
        if ((wallMask & 0b11000001) == 0b11000001)
        {
            m_iCurImgNum = m_iWallImageNum + 4;
        }
        else
        {
            m_iCurImgNum = m_iWallImageNum + 3;
        }
        m_pTransformCom->Set_Angle(0.f, D3DXToRadian(90.f), 0.f);
        break;

        // 상 우 하
    case 0b00010101:
        if ((wallMask & 0b00011111) == 0b00011111)
        {
            m_iCurImgNum = m_iWallImageNum + 7;
            m_pTransformCom->Set_Angle(0.f, D3DXToRadian(180.f), 0.f);
        }
        else if ((wallMask & 0b00010111) == 0b00010111)
        {
            m_iCurImgNum = m_iWallImageNum + 6;
            m_pTransformCom->Set_Angle(0.f, D3DXToRadian(-90.f), 0.f);
        }
        else if ((wallMask & 0b00011101) == 0b00011101)
        {
            m_iCurImgNum = m_iWallImageNum + 8;
        }
        else
        {
            m_iCurImgNum = m_iWallImageNum + 5;
            m_pTransformCom->Set_Angle(0.f, D3DXToRadian(-90.f), 0.f);
        }
        break;
        //좌 상 우
    case 0b01010100:
        if ((wallMask & 0b01111100) == 0b01111100)
        {
            m_iCurImgNum = m_iWallImageNum + 7;
            m_pTransformCom->Set_Angle(0.f, D3DXToRadian(90.f), 0.f);
        }
        else if ((wallMask & 0b01011100) == 0b01011100)
        {
            m_iCurImgNum = m_iWallImageNum + 6;
            m_pTransformCom->Set_Angle(0.f, D3DXToRadian(90.f), 0.f);
        }
        else if ((wallMask & 0b01110100) == 0b01110100)
        {
            m_iCurImgNum = m_iWallImageNum + 8;
        }
        else
        {
            m_iCurImgNum = m_iWallImageNum + 5;
            m_pTransformCom->Set_Angle(0.f, D3DXToRadian(180.f), 0.f);
        }
        break;
        // 좌 상 하
    case 0b01010001:
        if ((wallMask & 0b11110001) == 0b11110001)
        {
            m_iCurImgNum = m_iWallImageNum + 7;
        }
        else if ((wallMask & 0b11010001) == 0b11010001)
        {
            m_iCurImgNum = m_iWallImageNum + 8;
            m_pTransformCom->Set_Angle(0.f, D3DXToRadian(180.f), 0.f);
        }
        else if ((wallMask & 0b01110001) == 0b01110001)
        {
            m_iCurImgNum = m_iWallImageNum + 6;
            m_pTransformCom->Set_Angle(0.f, D3DXToRadian(90.f), 0.f);
        }
        else
        {
            m_iCurImgNum = m_iWallImageNum + 5;
            m_pTransformCom->Set_Angle(0.f, D3DXToRadian(90.f), 0.f);
        }
        break;
        // 좌 우 하
    case 0b01000101:
        if ((wallMask & 0b11000111) == 0b11000111)
        {
            m_iCurImgNum = m_iWallImageNum + 7;
            m_pTransformCom->Set_Angle(0.f, D3DXToRadian(-90.f), 0.f);
        }
        else if ((wallMask & 0b01000111) == 0b01000111)
        {
            m_iCurImgNum = m_iWallImageNum + 8;
            m_pTransformCom->Set_Angle(0.f, D3DXToRadian(90.f), 0.f);
        }
        else if ((wallMask & 0b11000101) == 0b11000101)
        {
            m_iCurImgNum = m_iWallImageNum + 6;
            m_pTransformCom->Set_Angle(0.f, D3DXToRadian(180.f), 0.f);
        }
        else
        {
            m_iCurImgNum = m_iWallImageNum + 5;
        }
        break;

        // 상 하 좌 우가 다 있음!
    case 0b01010101:
        if (wallMask == 0b01010101)
        {
            m_iCurImgNum = m_iWallImageNum + 9;
        }
        // 대각선 1개
        else if (wallMask == 0b01010111)
        {
            m_iCurImgNum = m_iWallImageNum + 10;
            m_pTransformCom->Set_Angle(0.f, D3DXToRadian(90.f), 0.f);
        }
        else if (wallMask == 0b01011101)
        {
            m_iCurImgNum = m_iWallImageNum + 10;
        }
        else if (wallMask == 0b01110101)
        {
            m_iCurImgNum = m_iWallImageNum + 10;
            m_pTransformCom->Set_Angle(0.f, D3DXToRadian(-90.f), 0.f);
        }
        else if (wallMask == 0b11010101)
        {
            m_iCurImgNum = m_iWallImageNum + 10;
            m_pTransformCom->Set_Angle(0.f, D3DXToRadian(180.f), 0.f);
        }
        // 대각선 2개
        else if (wallMask == 0b01011111)
        {
            m_iCurImgNum = m_iWallImageNum + 11;
            m_pTransformCom->Set_Angle(0.f, D3DXToRadian(90.f), 0.f);
        }
        else if (wallMask == 0b01111101)
        {
            m_iCurImgNum = m_iWallImageNum + 11;
        }
        else if (wallMask == 0b11110101)
        {
            m_iCurImgNum = m_iWallImageNum + 11;
            m_pTransformCom->Set_Angle(0.f, D3DXToRadian(-90.f), 0.f);
        }
        else if (wallMask == 0b11010111)
        {
            m_iCurImgNum = m_iWallImageNum + 11;
            m_pTransformCom->Set_Angle(0.f, D3DXToRadian(180.f), 0.f);
        }
        else if (wallMask == 0b01110111)
        {
            m_iCurImgNum = m_iWallImageNum + 12;
        }
        else if (wallMask == 0b11011101)
        {
            m_iCurImgNum = m_iWallImageNum + 12;
            m_pTransformCom->Set_Angle(0.f, D3DXToRadian(90.f), 0.f);
        }
        // 대각선 3개
        else if (wallMask == 0b11111101)
        {
            m_iCurImgNum = m_iWallImageNum + 13;
        }
        else if (wallMask == 0b11110111)
        {
            m_iCurImgNum = m_iWallImageNum + 13;
            m_pTransformCom->Set_Angle(0.f, D3DXToRadian(-90.f), 0.f);
        }
        else if (wallMask == 0b11011111)
        {
            m_iCurImgNum = m_iWallImageNum + 13;
            m_pTransformCom->Set_Angle(0.f, D3DXToRadian(180.f), 0.f);
        }
        else if (wallMask == 0b01111111)
        {
            m_iCurImgNum = m_iWallImageNum + 13;
            m_pTransformCom->Set_Angle(0.f, D3DXToRadian(90.f), 0.f);
        }
        // 전부!
        else if (wallMask == 0b11111111)
        {
            m_iCurImgNum = m_iWallImageNum + 14;
        }
        break;
    default:
        m_iWallImageNum = 0;
        break;
    }
}

void CWall::Set_Destroy()
{
   // m_bWallDestroyed = true;

    for (int i = 0; i < 8; i++)
    {
        if (!m_vecAroundWall[i])
            continue;

        if (i < 4)
        {
            m_vecAroundWall[i]->Set_DestoryWall(i + 4);
        }
        else
        {
            m_vecAroundWall[i]->Set_DestoryWall(i - 4);
        }
    }
}

void CWall::Drop_Item()
{
    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);

    CScene* pScene = Engine::Get_Scene();
    CItem* pGameObject = nullptr;

    if (m_iWallImageNum < 15)
    {
        vPos.x += rand() % 3 * 0.1f;
        vPos.z += rand() % 3 * 0.1f;
        pGameObject = CWallPiece::Create(m_pGraphicDev, ITEM_DIRTWALL, vPos);
        NULL_CHECK(pGameObject);
        m_vecItemName.push_back(L"Wall_Created_WallPiece" + std::to_wstring(m_iItemNumber++));

        if (pGameObject)
        {
            FAILED_CHECK_RETURN(pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, m_vecItemName.back().c_str()), );
            pGameObject->Set_Active(true);
            pGameObject->Set_Drop(true);
        }

        if (rand() % 2)
        {
            vPos.x -= rand() % 3 * 0.2f;
            vPos.z -= rand() % 3 * 0.2f;
            pGameObject = COre::Create(m_pGraphicDev, MATERIAL_COPPER, vPos);
            NULL_CHECK(pGameObject);
            m_vecItemName.push_back(L"Wall_Created_Ore" + std::to_wstring(m_iItemNumber++));

            if (pGameObject)
            {
                FAILED_CHECK_RETURN(pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, m_vecItemName.back().c_str()), );
                pGameObject->Set_Active(true);
                pGameObject->Set_Drop(true);
            }
        }
    }
    else if (m_iWallImageNum < 30)
    {
        vPos.x += rand() % 3 * 0.1f;
        vPos.z += rand() % 3 * 0.1f;
        pGameObject = CWallPiece::Create(m_pGraphicDev, ITEM_STONEWALL, vPos);
        NULL_CHECK(pGameObject);
        m_vecItemName.push_back(L"Wall_Created_WallPiece" + std::to_wstring(m_iItemNumber++));

        if (pGameObject)
        {
            FAILED_CHECK_RETURN(pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, m_vecItemName.back().c_str()), );
            pGameObject->Set_Active(true);
            pGameObject->Set_Drop(true);
        }
        if (rand() % 2)
        {
            vPos.x -= rand() % 3 * 0.2f;
            vPos.z -= rand() % 3 * 0.2f;
            pGameObject = COre::Create(m_pGraphicDev, MATERIAL_IRON, vPos);
            NULL_CHECK(pGameObject);
            m_vecItemName.push_back(L"Wall_Created_Ore" + std::to_wstring(m_iItemNumber++));

            if (pGameObject)
            {
                FAILED_CHECK_RETURN(pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, m_vecItemName.back().c_str()), );
                pGameObject->Set_Active(true);
                pGameObject->Set_Drop(true);
            }
        }
    }
    else
    {
        vPos.x += rand() % 3 * 0.1f;
        vPos.z += rand() % 3 * 0.1f;
        pGameObject = CWallPiece::Create(m_pGraphicDev, ITEM_GRASSWALL, vPos);
        NULL_CHECK(pGameObject);
        m_vecItemName.push_back(L"Wall_Created_WallPiece" + std::to_wstring(m_iItemNumber++));

        if (pGameObject)
        {
            FAILED_CHECK_RETURN(pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, m_vecItemName.back().c_str()), );
            pGameObject->Set_Active(true);
            pGameObject->Set_Drop(true);
        }

        if (rand() % 2)
        {
            vPos.x -= rand() % 3 * 0.2f;
            vPos.z -= rand() % 3 * 0.2f;
            pGameObject = COre::Create(m_pGraphicDev, MATERIAL_SCARLET, vPos);
            NULL_CHECK(pGameObject);
            m_vecItemName.push_back(L"Wall_Created_Ore" + std::to_wstring(m_iItemNumber++));

            if (pGameObject)
            {
                FAILED_CHECK_RETURN(pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, m_vecItemName.back().c_str()), );
                pGameObject->Set_Active(true);
                pGameObject->Set_Drop(true);
            }
        }
    }
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
