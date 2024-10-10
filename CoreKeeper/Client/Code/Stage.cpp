#include "pch.h"
#include "..\Header\Stage.h"
#include "Export_Utility.h"
#include "..\Header\DynamicCamera.h"


CStage::CStage(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CScene(pGraphicDev)
    , m_bInvCheck(false)
    , m_iLoadTileCount(0)
    , m_iLoadWallCount(0)
{
    m_vecWall.resize((VTXCNTX - 1) * (VTXCNTZ - 1));
    m_vecCore.resize((VTXCNTX - 1) * (VTXCNTZ - 1));
    m_vecUnreachable.resize((VTXCNTX - 1) * (VTXCNTZ - 1));

    for(_int i = 0; i < ((VTXCNTX - 1) * (VTXCNTZ - 1)); i++)
        m_wsWallNameString[i] = L"Wall_" + std::to_wstring(i);
}


CStage::~CStage()
{
	CCraftMgr::DestroyInstance();
	CFarmMgr::DestroyInstance();
	CBuffMgr::DestroyInstance();
}

HRESULT CStage::Ready_Scene()
{

	CCraftMgr::GetInstance()->Ready_Craft(m_pGraphicDev);
	CFarmMgr::GetInstance()->Ready_Farm(m_pGraphicDev);
	CBuffMgr::GetInstance()->Ready_Buff(m_pGraphicDev);

    //FAILED_CHECK_RETURN(Ready_LightInfo(), E_FAIL);
    FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Layer_Environment"), E_FAIL);

    FAILED_CHECK_RETURN(Ready_Layer_GameLogic(L"Layer_GameLogic"), E_FAIL);


    FAILED_CHECK_RETURN(Ready_Layer_UI(L"Layer_UI"), E_FAIL);
    Load_MapFile();
    Load_MonsterData();
    Load_ObjectData();
    // 이거랑 Render_Scene() 주석 풀면 일단 stage를 위에서 꽂아서 보게됨.
    //FAILED_CHECK_RETURN(Ready_Layer_MiniMap(L"Layer_MiniMap"), E_FAIL);

    // 묘비에 넣고싶은 아이템 있으면 묘비 SetUpItem에서 하기
    CGravestoneObject* pGameObject = CGravestoneObject::Create(m_pGraphicDev, { VTXCNTX / 2 - 2.f, 0.5f, 13.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    pGameObject->SetUp_Item(this);
    this->Create_GameObject(L"Layer_Environment", pGameObject, L"AheadGrave");

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    return S_OK;
}

_int CStage::Update_Scene(const _float& fTimeDelta)
{

	_int	iExit = Engine::CScene::Update_Scene(fTimeDelta);
	CFarmMgr::GetInstance()->Update_Farm(fTimeDelta);
	CBuffMgr::GetInstance()->Update_Buff(fTimeDelta);
	return iExit;
}

void CStage::LateUpdate_Scene()
{
    Engine::CScene::LateUpdate_Scene();
}

void CStage::Render_Scene()
{
}

HRESULT CStage::Ready_LightInfo()
{

    D3DLIGHT9		tLightInfo;
    ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

    tLightInfo.Type = D3DLIGHT_DIRECTIONAL;

    tLightInfo.Diffuse = { 1.f, 1.f, 1.f, 1.f };
    tLightInfo.Specular = { 1.f, 1.f, 1.f, 1.f };
    tLightInfo.Ambient = { 1.f, 1.f, 1.f, 1.f };
    tLightInfo.Direction = { 1.f, -1.f, 1.f };

    FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 0), E_FAIL);

    return S_OK;
}

HRESULT CStage::Ready_Layer_Environment(const _tchar* pLayerTag)
{
    Engine::CLayer* pLayer = CLayer::Create();
    NULL_CHECK_RETURN(pLayer, E_FAIL);

    Engine::CGameObject* pGameObject = nullptr;

    pGameObject = CSkyBox::Create(m_pGraphicDev);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SkyBox", pGameObject), E_FAIL);

    _vec3 eye(0.f, 10.f, -10.f);
    _vec3 at(0.f, 0.f, 1.f);
    _vec3 up(0.f, 1.f, 0.f);

    pGameObject = CDynamicCamera::Create(m_pGraphicDev,
        &eye,
        &at,
        &up);

    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"DynamicCamera", pGameObject), E_FAIL);

    m_pTerrainObject = CTerrain::Create(m_pGraphicDev);
    NULL_CHECK_RETURN(m_pTerrainObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Terrain", m_pTerrainObject), E_FAIL);

    pGameObject = CMiniTerrain::Create(m_pGraphicDev);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MiniTerrain", pGameObject), E_FAIL);

#pragma region BASECAMP

    pGameObject = CCore::Create(m_pGraphicDev, { VTXCNTX / 2, 2.3f, 11.f + 5.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Core", pGameObject), E_FAIL);

    pGameObject = CCoreBase::Create(m_pGraphicDev, { VTXCNTX / 2, 0.1f, 11.f + 5.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"CoreBase", pGameObject), E_FAIL);

    pGameObject = CStatue::Create(m_pGraphicDev, { VTXCNTX / 2, 1.3f, 17.f + 5.f }, 0);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SlimeStatue", pGameObject), E_FAIL);

    pGameObject = CStatueBase::Create(m_pGraphicDev, { VTXCNTX / 2, 0.1f, 17.f + 5.f }, 0);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SlimeStatueBase", pGameObject), E_FAIL);

    // 슬라임 밑 세로 타일
    pGameObject = CTile::Create(m_pGraphicDev, 0, 3, { VTXCNTX / 2, 0.1f, 15.f + 5.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Tile0", pGameObject), E_FAIL);

    pGameObject = CTile::Create(m_pGraphicDev, 0, 3, { VTXCNTX / 2, 0.1f, 14.f + 5.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Tile1", pGameObject), E_FAIL);

    pGameObject = CStatue::Create(m_pGraphicDev, { VTXCNTX / 2 - 8.f, 1.3f, 13.f + 5.f }, 1);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"LarvaStatue", pGameObject), E_FAIL);

    pGameObject = CStatueBase::Create(m_pGraphicDev, { VTXCNTX / 2 - 8.f, 0.1f, 13.f + 5.f }, 1);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"LarvaStatueBase", pGameObject), E_FAIL);

    // 벌레 밑 가로 타일
    pGameObject = CTile::Create(m_pGraphicDev, 1, 0, { VTXCNTX / 2 - 9.f, 0.1f, 11.f + 5.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Tile2", pGameObject), E_FAIL);

    pGameObject = CTile::Create(m_pGraphicDev, 1, 1, { VTXCNTX / 2 - 8.f, 0.1f, 11.f + 5.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Tile3", pGameObject), E_FAIL);

    pGameObject = CTile::Create(m_pGraphicDev, 1, 2, { VTXCNTX / 2 - 7.f, 0.1f, 11.f + 5.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Tile4", pGameObject), E_FAIL);

    pGameObject = CTile::Create(m_pGraphicDev, 1, 2, { VTXCNTX / 2 - 6.f, 0.1f, 11.f + 5.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Tile5", pGameObject), E_FAIL);

    pGameObject = CTile::Create(m_pGraphicDev, 1, 2, { VTXCNTX / 2 - 5.f, 0.1f, 11.f + 5.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Tile6", pGameObject), E_FAIL);

    pGameObject = CTile::Create(m_pGraphicDev, 1, 2, { VTXCNTX / 2 - 4.f, 0.1f, 11.f + 5.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Tile7", pGameObject), E_FAIL);

    pGameObject = CTile::Create(m_pGraphicDev, 1, 2, { VTXCNTX / 2 - 3.f, 0.1f, 11.f + 5.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Tile8", pGameObject), E_FAIL);

    pGameObject = CStatue::Create(m_pGraphicDev, { VTXCNTX / 2 + 8.f, 1.3f, 13.f + 5.f }, 2);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MalugazStatue", pGameObject), E_FAIL);

    pGameObject = CStatueBase::Create(m_pGraphicDev, { VTXCNTX / 2 + 8.f, 0.1f, 13.f + 5.f }, 2);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MalugazStatueBase", pGameObject), E_FAIL);

    // 말루가즈 밑 가로 타일
    pGameObject = CTile::Create(m_pGraphicDev, 2, 4, { VTXCNTX / 2 + 9.f, 0.1f, 11.f + 5.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Tile9", pGameObject), E_FAIL);

    pGameObject = CTile::Create(m_pGraphicDev, 2, 1, { VTXCNTX / 2 + 8.f, 0.1f, 11.f + 5.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Tile10", pGameObject), E_FAIL);

    pGameObject = CTile::Create(m_pGraphicDev, 2, 2, { VTXCNTX / 2 + 7.f, 0.1f, 11.f + 5.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Tile11", pGameObject), E_FAIL);

    pGameObject = CTile::Create(m_pGraphicDev, 2, 2, { VTXCNTX / 2 + 6.f, 0.1f, 11.f + 5.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Tile12", pGameObject), E_FAIL);

    pGameObject = CTile::Create(m_pGraphicDev, 2, 2, { VTXCNTX / 2 + 5.f, 0.1f, 11.f + 5.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Tile13", pGameObject), E_FAIL);

    pGameObject = CTile::Create(m_pGraphicDev, 2, 2, { VTXCNTX / 2 + 4.f, 0.1f, 11.f + 5.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Tile14", pGameObject), E_FAIL);

    pGameObject = CTile::Create(m_pGraphicDev, 2, 2, { VTXCNTX / 2 + 3.f, 0.1f, 11.f + 5.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Tile15", pGameObject), E_FAIL);

    pGameObject = CSpawnPoint::Create(m_pGraphicDev, { VTXCNTX / 2, 0.1f, 7.f + 5.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"BaseSpawnPoint", pGameObject), E_FAIL);

    pGameObject = CSpawnPoint::Create(m_pGraphicDev, { 51.f, 0.1f, 60.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"StoneSpawnPoint", pGameObject), E_FAIL);

    pGameObject = CSpawnPoint::Create(m_pGraphicDev, { 14.f, 0.1f, 105.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"GrassSpawnPoint", pGameObject), E_FAIL);

    pGameObject = CSkeleton::Create(m_pGraphicDev, { VTXCNTX / 2 - 3.f, 0.1f, 13.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Skeleton", pGameObject), E_FAIL);

    pGameObject = CFurnaceObject::Create(m_pGraphicDev, { VTXCNTX / 2 - 3.f, 0.6f, 11.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"FurnaceObject", pGameObject), E_FAIL);


    pGameObject = CFurnaceObject::Create(m_pGraphicDev, { VTXCNTX / 2 - 3.f - 1.f, 0.6f, 11.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"FurnaceObject2", pGameObject), E_FAIL);


    pGameObject = CCookingPotObject::Create(m_pGraphicDev, { VTXCNTX / 2 - 3.f, 0.6f, 10.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"CookingPotObject", pGameObject), E_FAIL);

    pGameObject = CCookingPotObject::Create(m_pGraphicDev, { VTXCNTX / 2 - 3.f - 1.f, 0.6f, 10.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"CookingPotObject2", pGameObject), E_FAIL);


    


    _vec3   spawnPos = { 14.0f, 0.1f, 71.0f };
    pGameObject = CBossSpawnPoint::Create(m_pGraphicDev, spawnPos);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MalgaSummonPoint", pGameObject), E_FAIL);


#pragma endregion

    m_mapLayer.insert({ pLayerTag , pLayer });

    return S_OK;
}

HRESULT CStage::Ready_Layer_GameLogic(const _tchar* pLayerTag)
{
    Engine::CLayer* pLayer = CLayer::Create();
    NULL_CHECK_RETURN(pLayer, E_FAIL);

    Engine::CGameObject* pGameObject = nullptr;

    pGameObject = CPlayer::Create(m_pGraphicDev);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Player", pGameObject), E_FAIL);

    pGameObject = CEye::Create(m_pGraphicDev);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Player_Eye", pGameObject), E_FAIL);

    pGameObject = CShirt::Create(m_pGraphicDev);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Player_Shirt", pGameObject), E_FAIL);

    pGameObject = CPants::Create(m_pGraphicDev);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Player_Pants", pGameObject), E_FAIL);

    pGameObject = CHair::Create(m_pGraphicDev);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Player_Hair", pGameObject), E_FAIL);

    pGameObject = CHairShade::Create(m_pGraphicDev);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Player_HairShade", pGameObject), E_FAIL);

    CPet* pPet = CPet::Create(m_pGraphicDev);
    NULL_CHECK_RETURN(pPet, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Pet", pPet), E_FAIL);

    pGameObject = CPetTail::Create(m_pGraphicDev);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    dynamic_cast<CPetTail*>(pGameObject)->Set_Pet(pPet);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"PetTail", pGameObject), E_FAIL);

    pGameObject = CIngredient::Create(m_pGraphicDev, ITEM_BERRY_SEED);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Berry", pGameObject), E_FAIL);
    dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

    pGameObject = CIngredient::Create(m_pGraphicDev, ITEM_PEPPER_SEED);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Pepper", pGameObject), E_FAIL);
    dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

    pGameObject = COre::Create(m_pGraphicDev, MATERIAL_COPPER);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"CopperOre", pGameObject), E_FAIL);
    dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

    pGameObject = COre::Create(m_pGraphicDev, MATERIAL_SCARLET);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"ScarletOre", pGameObject), E_FAIL);
    dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

    pGameObject = CLantern::Create(m_pGraphicDev, MATERIAL_WOOD);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Player's_Lantern", pGameObject), E_FAIL);
    dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

    pGameObject = CLunch::Create(m_pGraphicDev);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Player's_Lunch", pGameObject), E_FAIL);
    dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);
    dynamic_cast<CItem*>(pGameObject)->Add_Count(2);

    pGameObject = CChocoBar::Create(m_pGraphicDev);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Player's_ChocoBar", pGameObject), E_FAIL);
    dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);
    dynamic_cast<CItem*>(pGameObject)->Add_Count(1);

    pGameObject = CMalugaz::Create(m_pGraphicDev, {14.0f, 0.0f,71.0f});
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Malugaz", pGameObject), E_FAIL);


    m_mapLayer.insert({ pLayerTag , pLayer });

    return S_OK;
}

HRESULT CStage::Ready_Layer_UI(const _tchar* pLayerTag)
{
    Engine::CLayer* pLayer = CLayer::Create();
    NULL_CHECK_RETURN(pLayer, E_FAIL);

    Engine::CGameObject* pGameObject = nullptr;

    _vec2 vPos = { 844.f, 190.f };
    _vec2 vSize = { 150.f, 150.f };

    pGameObject = CUIPlayerStatus::Create(m_pGraphicDev, vPos, vSize);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UIPlayerStatus", pGameObject), E_FAIL); // UI 추가

    for (int i = 0; i < 10; i++)
    {
        if (i < 3)
        {
            vPos = { 745.f + i * 89.f , 85.f };

        }
        else if (i >= 3 && i < 5)
        {
            vPos = { 745.f + (i - 3) * 178.f , 85.f + 70.f };
        }
        else if (i >= 5 && i < 7)
        {
            vPos = { 745.f + (i - 5) * 178.f , 85.f + 140.f };
        }
        else if (i >= 7 && i < 10)
        {
            vPos = { 745.f + (i - 7) * 89.f , 85.f + 210.f };
        }
        vSize = { 30.f, 30.f };

        m_ItemSlot[i] = L"UIItemSlot_" + std::to_wstring(i);

        pGameObject = CUIItemSlot::Create(m_pGraphicDev, vPos, vSize, (CUIItemSlot::SLOTTYPE)i);
        NULL_CHECK_RETURN(pGameObject, E_FAIL);
        FAILED_CHECK_RETURN(pLayer->Add_GameObject(m_ItemSlot[i].c_str(), pGameObject), E_FAIL);
    }

    vPos = { 970.f, 190.f };
    vSize = { 10.f, 135 };

    pGameObject = CUIPlayerStats::Create(m_pGraphicDev, vPos, vSize);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UIPlayerStats", pGameObject), E_FAIL);

    vPos = { 470.f, 255.f };
    vSize = { 120.f, 90.f };

    pGameObject = CUICraft::Create(m_pGraphicDev, vPos, vSize, true);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UILeftCraft", pGameObject), E_FAIL);


    vPos = { 800.f, 255.f };
    vSize = { 120.f, 90.f };

    pGameObject = CUICraft::Create(m_pGraphicDev, vPos, vSize, false);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UIRightCraft", pGameObject), E_FAIL);

    for (int i = 0; i < 6; i++)
    {
        if (i < 3)
        {
            vPos = { 400.f + i * 70.f , 220.f };
        }
        else
            vPos = { 400.f + (i - 3) * 70.f, 220.f + (i / 3) * 70.f };

        vSize = { 30.f, 30.f };

        m_CraftLSlot[i] = L"UICraftLSlot_" + std::to_wstring(i);

        pGameObject = CUICraftSlot::Create(m_pGraphicDev, vPos, vSize, i, true);
        NULL_CHECK_RETURN(pGameObject, E_FAIL);
        FAILED_CHECK_RETURN(pLayer->Add_GameObject(m_CraftLSlot[i].c_str(), pGameObject), E_FAIL);
    }

    for (int i = 0; i < 6; i++)
    {
        if (i < 3)
        {
            vPos = { 730.f + i * 70.f , 220.f };
        }
        else
            vPos = { 730.f + (i - 3) * 70.f, 220.f + (i / 3) * 70.f };

        vSize = { 30.f, 30.f };

        m_CraftRSlot[i] = L"UICraftRSlot_" + std::to_wstring(i);

        pGameObject = CUICraftSlot::Create(m_pGraphicDev, vPos, vSize, i, true);
        NULL_CHECK_RETURN(pGameObject, E_FAIL);
        FAILED_CHECK_RETURN(pLayer->Add_GameObject(m_CraftRSlot[i].c_str(), pGameObject), E_FAIL);
    }

    vPos = { 1100.f, 540.f };
    vSize = { 35.f, 45.f };

    pGameObject = CUIScreenIcon::Create(m_pGraphicDev, vPos, vSize, CUIScreenIcon::ICON_MAP);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UIScreenicon_Map", pGameObject), E_FAIL);

    vSize = { 45.f, 45.f };
    vPos = { 1200.f , 540.f };

    pGameObject = CUIScreenIcon::Create(m_pGraphicDev, vPos, vSize, CUIScreenIcon::ICON_BAG);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UIScreenicon_Bag", pGameObject), E_FAIL);

    vSize = { 35.f , 35.f };
    vPos = { 1200.f , 640.f };

    pGameObject = CUIScreenIcon::Create(m_pGraphicDev, vPos, vSize, CUIScreenIcon::ICON_HAND);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UIScreenicon_Hand", pGameObject), E_FAIL);

    /*
    vPos = { 780.f , 550.f };

    pGameObject = CUIScreenIcon::Create(m_pGraphicDev, vPos, vSize, 4);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UIScreenicon_Install", pGameObject), E_FAIL);
    */

    vPos = { 460.f, 40.f };
    vSize = { 420.f, 14.f };

    pGameObject = CUIStatusBar::Create(m_pGraphicDev, vPos, vSize, 1);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_Health", pGameObject), E_FAIL);

    vPos = { 460.f, 70.f };
    vSize = { 420.f, 12.f };

    pGameObject = CUIStatusBar::Create(m_pGraphicDev, vPos, vSize, 4);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_Mp", pGameObject), E_FAIL);

    vPos = { 460.f, 92.f };
    vSize = { 420.f, 7.f };

    pGameObject = CUIStatusBar::Create(m_pGraphicDev, vPos, vSize, 7);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_Hunger", pGameObject), E_FAIL);

    for (int i = 0; i < 10; i++)
    {
        if (i == 0)
        {
            vPos = { 960.f, 650.f };
        }
        else
            vPos = { 330.f + (63.f * i), 650.f };

        m_Invstring[i] = L"UI_ScreenInv_" + std::to_wstring(i);

        pGameObject = CUIScreenInv::Create(m_pGraphicDev, vPos, i);
        NULL_CHECK_RETURN(pGameObject, E_FAIL);
        FAILED_CHECK_RETURN(pLayer->Add_GameObject(m_Invstring[i].c_str(), pGameObject), E_FAIL);
    }

    // 좌표 수정
    for (int i = 10; i < 40; i++)
    {
        if (i % 10 == 0)
            vPos = { 393.f + (63.f * (_float)(i - (i / 10) * 10)), 413.f + (_float)(i / 10) * 63.f };
        else
            vPos = { 393.f + (63.f * (_float)(i - (((i - 1) / 10) * 10))), 413.f + (_float)((i - 1) / 10) * 63.f };

        m_Invstring[i] = L"UI_Inventory_" + std::to_wstring(i);

        pGameObject = CUIInventory::Create(m_pGraphicDev, vPos, i);
        NULL_CHECK_RETURN(pGameObject, E_FAIL);
        FAILED_CHECK_RETURN(pLayer->Add_GameObject(m_Invstring[i].c_str(), pGameObject), E_FAIL);
    }

    for (int i = 0; i < 18; i++)
    {
        if (i % 6 == 0)
            vPos = { 519.f + (63.f * (_float)(i - (i / 6) * 6)), 180.f + (_float)(i / 6) * 63.f };
        else
            vPos = { 519.f + (63.f * (_float)(i - (((i - 1) / 6) * 6))), 180.f + (_float)((i - 1) / 6) * 63.f };

        m_ChestInvstring[i] = L"UI_ChestInventory_" + std::to_wstring(i);

        pGameObject = CUIChestInv::Create(m_pGraphicDev, vPos, i);
        NULL_CHECK_RETURN(pGameObject, E_FAIL);
        FAILED_CHECK_RETURN(pLayer->Add_GameObject(m_ChestInvstring[i].c_str(), pGameObject), E_FAIL);
    }

    for (int i = 0; i < 30; i++)
    {
        if (i % 10 == 0)
            vPos = { 393.f + (63.f * (_float)(i - (i / 10) * 10)), 180.f + (_float)(i / 10) * 63.f };
        else
            vPos = { 393.f + (63.f * (_float)(i - (((i - 1) / 10) * 10))), 180.f + (_float)((i - 1) / 10) * 63.f };

        m_GraveInvstring[i] = L"UI_GraveInventory_" + std::to_wstring(i);

        pGameObject = CUIChestInv::Create(m_pGraphicDev, vPos, i);
        NULL_CHECK_RETURN(pGameObject, E_FAIL);
        FAILED_CHECK_RETURN(pLayer->Add_GameObject(m_GraveInvstring[i].c_str(), pGameObject), E_FAIL);
    }

    vPos = { 675.f, 398.f };
    vSize = { 325.f, 40.f };

    pGameObject = CUIInvPlate::Create(m_pGraphicDev, vPos, vSize);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_Plate", pGameObject), E_FAIL);

    vPos = { 300.f, 508.f };
    vSize = { 35.f, 68.f };

    pGameObject = CUITrashCan::Create(m_pGraphicDev, vPos, vSize);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_TrashCan", pGameObject), E_FAIL);

    vPos = { 300.f, 476.f };
    vSize = { 30.f, 30.f };

    pGameObject = CUITrashSlot::Create(m_pGraphicDev, vPos, vSize);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_TrashSlot", pGameObject), E_FAIL);

    vPos = { 1040.f, 476.f };
    vSize = { 30.f, 30.f };

    pGameObject = CUISort::Create(m_pGraphicDev, vPos, vSize, CUISort::TYPE_PLAYER);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_Sort", pGameObject), E_FAIL);


    vPos = { 914.f, 243.f };
    vSize = { 30.f, 30.f };

    pGameObject = CUISort::Create(m_pGraphicDev, vPos, vSize, CUISort::TYPE_CHEST);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_ChestSort", pGameObject), E_FAIL);

    vPos = { 914.f, 306.f };
    vSize = { 30.f, 30.f };

    pGameObject = CUIChestSort::Create(m_pGraphicDev, vPos, vSize);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_ChestAddItem", pGameObject), E_FAIL);

    vPos = { 1130.f, 190.f };
    vSize = { 130.f, 150.f };

    pGameObject = CUIStatusFrame::Create(m_pGraphicDev, vPos, vSize);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_StatusFrame", pGameObject), E_FAIL);

    vPos = { 1000.f, 510.f };
    vSize = { 250.f, 180.f };

    pGameObject = CUIItemFrame::Create(m_pGraphicDev, vPos, vSize);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_ItemFrame", pGameObject), E_FAIL);

    /*
    pGameObject = CMiniMap::Create(m_pGraphicDev, vPos, vSize);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_MiniMap", pGameObject), E_FAIL);
    */

    pGameObject = CMiniMapFrame::Create(m_pGraphicDev);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MiniFrame", pGameObject), E_FAIL);

    //pGameObject = CWorldMap::Create(m_pGraphicDev);
    //NULL_CHECK_RETURN(pGameObject, E_FAIL);
    //FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"WorldMap", pGameObject), E_FAIL);

    pGameObject = CMiniObject::Create(m_pGraphicDev, { 128.5f, 0.1f, 128.5f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MiniPlayer", pGameObject), E_FAIL);

    _vec3	vSpawnPos[3] =
    {
       // { VTXCNTX / 2 + 65.f, 0.1f, 21.5f + 80.f },
        { 51.0f, 0.1f, 60.0f},
        {  VTXCNTX / 2, 0.1f, 7.f + 5.f },
        {14.0f, 0.1f, 105.0f}
    };

    wstring wsMiniSpawnName[3];
    for (_int i = 0; i < 3; ++i)
    {
        wsMiniSpawnName[i] = L"MiniSpawn_" + std::to_wstring(i);
        CMiniSpawn* pMiniSpawn = CMiniSpawn::Create(m_pGraphicDev, vSpawnPos[i]);
        NULL_CHECK_RETURN(pMiniSpawn, E_FAIL);
        FAILED_CHECK_RETURN(pLayer->Add_GameObject(wsMiniSpawnName[i].c_str(), pMiniSpawn), E_FAIL);

        m_vecMiniSpawn.push_back(pMiniSpawn);
    }

    pGameObject = CMiniCore::Create(m_pGraphicDev, { VTXCNTX / 2, 2.3f, 11.f + 5.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MiniCore", pGameObject), E_FAIL);

    vPos = { WINCX / 2.f, 200.f };
    vSize = { 150.f, 130.f };

    pGameObject = CUIStatue::Create(m_pGraphicDev, vPos, vSize);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_Statue", pGameObject), E_FAIL);

    vPos = { WINCX / 2.f, 170.f };
    vSize = { 30.f, 30.f };

    pGameObject = CUIJemSlot::Create(m_pGraphicDev, vPos, vSize);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_JemSlot", pGameObject), E_FAIL);

    vPos = { WINCX / 2.f + 3.f, 273.f };
    vSize = { 32.f, 32.f };

    pGameObject = CUIStatueCraft::Create(m_pGraphicDev, vPos, vSize);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_StatueCraft", pGameObject), E_FAIL);

    vPos = { 376.f , 260.f };
    vSize = { 16.f, 16.f };

    pGameObject = CUICraftButton::Create(m_pGraphicDev, vPos, vSize);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_CraftButton", pGameObject), E_FAIL);


    pGameObject = CUICursor::Create(m_pGraphicDev);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_Cursor", pGameObject), E_FAIL);


    vPos = { WINCX / 2.f , 230.f };
    vSize = { 140.f, 115.f };

    pGameObject = CUIFurnace::Create(m_pGraphicDev, vPos, vSize);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_Furnace", pGameObject), E_FAIL);


    vPos = { WINCX / 2.f , 230.f };
    vSize = { 140.f, 115.f };

    pGameObject = CUICookingPot::Create(m_pGraphicDev, vPos, vSize);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_CookingPot", pGameObject), E_FAIL);

    for (_int i = 0; i < BUFFTYPE_END; i++)
    {
        pGameObject = CUIBuff::Create(m_pGraphicDev, _vec2(0, 0), _vec2(20, 20));
        NULL_CHECK_RETURN(pGameObject, E_FAIL);
        m_wsBuffNameString[i] = L"UI_Buff" + std::to_wstring(i);
        FAILED_CHECK_RETURN(pLayer->Add_GameObject(m_wsBuffNameString[i].c_str(), pGameObject), E_FAIL);
        CBuffMgr::GetInstance()->Set_UIBuff((BUFFTYPE)i, dynamic_cast<CUIBuff*>(pGameObject));
    }

    m_mapLayer.insert({ pLayerTag , pLayer });

    return S_OK;
}

CStage* CStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CStage* pStage = new CStage(pGraphicDev);

    if (FAILED(pStage->Ready_Scene()))
    {
        Safe_Release(pStage);
        MSG_BOX("Stage Create Failed");
        return nullptr;
    }

    return pStage;
}

HRESULT CStage::Load_MapFile()
{
    auto	iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(L"Layer_Environment"));

    if (iter == m_mapLayer.end())
        return E_FAIL;

    Engine::CGameObject* pGameObject = nullptr;

    const _tchar* strFileName = L"../../Data/TileData.txt";
    const _tchar* strWallFileName = L"../../Data/WallData.txt";
    const _tchar* strObjectFileName = L"../../Data/ObjectData.txt";
    const _tchar* strBigWallFileName = L"../../Data/BigWallData.txt";

    m_hFile = CreateFile(strFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    m_hWallFile = CreateFile(strWallFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    //m_hObjectFile = CreateFile(strObjectFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    m_hBigWallFile = CreateFile(strBigWallFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);


    if (INVALID_HANDLE_VALUE == m_hFile)
    {
        MSG_BOX("Fail Open Terrain file");
        return E_FAIL;
    }

    if (INVALID_HANDLE_VALUE == m_hWallFile)
    {
        MSG_BOX("Fail Open Wall file");
        return E_FAIL;
    }

    /*if (INVALID_HANDLE_VALUE == m_hObjectFile)
    {
        MSG_BOX("Fail Open Object file");
        return E_FAIL;
    }*/

    if (INVALID_HANDLE_VALUE == m_hBigWallFile)
    {
        MSG_BOX("Fail Open BigWall file");
        return E_FAIL;
    }

    // ========================================================

    DWORD dwByte = 0;
    CTerrain* pTerrain = dynamic_cast<CTerrain*>(this->Get_GameObject(L"Layer_Environment", L"Terrain"));

    if (!pTerrain)
    {
        MSG_BOX("Fail Open Terrain");
        CloseHandle(m_hFile);
        return E_FAIL;
    }

    auto vec = pTerrain->Get_TextureNumber();
    auto vecReach = pTerrain->Get_Unreachable();

    for (_int i = 0; i < vec.size(); ++i)
    {
        ReadFile(m_hFile, &vec[i], sizeof(_int), &dwByte, nullptr);

        bool	bTemp(false);
        ReadFile(m_hFile, &bTemp, sizeof(bool), &dwByte, nullptr);
        vecReach[i] = bTemp;
    }

    pTerrain->Set_TextureNumber(vec);
    pTerrain->Set_Unreachable(vecReach);

    // ========================================================

    /*

    _vec3	vSpawnPos[3] =
    {
        { VTXCNTX / 2 + 65.f, 0.1f, 21.5f + 80.f },
        { VTXCNTX / 2, 0.1f, 17.f},
        { VTXCNTX / 2 - 40.f, 0.1f, 21.5f + 60.f }
    };

    wstring wsMiniSpawnName[3];
    for (_int i = 0; i < 3; ++i)
    {
        wsMiniSpawnName[i] = L"MiniSpawn_" + std::to_wstring(i);
        CMiniSpawn* pMiniSpawn = CMiniSpawn::Create(m_pGraphicDev, vSpawnPos[i]);
        NULL_CHECK_RETURN(pMiniSpawn, E_FAIL);
        FAILED_CHECK_RETURN(pLayer->Add_GameObject(wsMiniSpawnName[i].c_str(), pMiniSpawn), E_FAIL);

        m_vecMiniSpawn.push_back(pMiniSpawn);
    }
    */




    DWORD dwByte4 = 0;
    _vec3 vTempBigWallPos(0.0f, 0.0f, 0.0f);
    _int  vTempBigWallType(0);

    wstring wsBigWallName[4];
    for (_int i = 0; i < 4; ++i)
    {
        ReadFile(m_hBigWallFile, &vTempBigWallPos, sizeof(_vec3), &dwByte4, nullptr);
        ReadFile(m_hBigWallFile, &vTempBigWallType, sizeof(_int), &dwByte4, nullptr);

        if (dwByte4 == 0)
            break;

        wsBigWallName[i] = L"BigWall_" + std::to_wstring(i);
        CMapToolWall* pBigWall = CMapToolWall::Create(m_pGraphicDev, vTempBigWallPos.x, vTempBigWallPos.z, vTempBigWallType);
        NULL_CHECK_RETURN(pBigWall, E_FAIL);
        FAILED_CHECK_RETURN(iter->second->Add_GameObject(wsBigWallName[i].c_str(), pBigWall), E_FAIL);
    }

    // ========================================================	

    DWORD dwByte2 = 0;
    _vec3 vTempWallPos(0.0f, 0.0f, 0.0f);
    _int vTempWallImgNum(0);
    _int vTempIndex(0);

    while (true)
    {
        ReadFile(m_hWallFile, &vTempWallPos, sizeof(_vec3), &dwByte2, nullptr);
        ReadFile(m_hWallFile, &vTempWallImgNum, sizeof(_int), &dwByte2, nullptr);
        ReadFile(m_hWallFile, &vTempIndex, sizeof(_int), &dwByte2, nullptr);

        if (dwByte2 == 0)
            break;

        CWall* pWall = CWall::Create(m_pGraphicDev, vTempWallPos.x, vTempWallPos.z, vTempWallImgNum, m_wsWallNameString[vTempIndex].c_str());
        pWall->Set_PickedWallName(m_wsWallNameString[vTempIndex].c_str());
        NULL_CHECK_RETURN(pWall, E_FAIL);
        FAILED_CHECK_RETURN(iter->second->Add_GameObject(m_wsWallNameString[vTempIndex].c_str(), pWall), E_FAIL);

        m_vecWall[vTempIndex] = pWall;
        m_vecUnreachable[vTempIndex] = true;

    }

    for (int i = 0; i < VTXCNTZ - 1; i++)
    {
        for (int j = 0; j < VTXCNTX - 1; j++)
        {
            int iIndex = i * (VTXCNTX - 1) + j;

            if (!m_vecWall[iIndex])
                continue;

            // 하단
            if (i > 0)
            {
                m_vecWall[iIndex]->Add_WallArray(0, m_vecWall[iIndex - (VTXCNTX - 1)]);
            }


            // 하단 우측
            if (i > 0 && j < VTXCNTX - 1 - 1)
            {
                m_vecWall[iIndex]->Add_WallArray(1, m_vecWall[iIndex - (VTXCNTX - 1) + 1]);
            }


            // 우측
            if (j < VTXCNTX - 1 - 1)
            {
                m_vecWall[iIndex]->Add_WallArray(2, m_vecWall[iIndex + 1]);
            }


            // 상단 우측
            if (i < VTXCNTZ - 1 - 1 && j < VTXCNTX - 1 - 1)
            {
                m_vecWall[iIndex]->Add_WallArray(3, m_vecWall[iIndex + (VTXCNTX - 1) + 1]);
            }


            // 상단
            if (i < VTXCNTZ - 1 - 1)
            {
                m_vecWall[iIndex]->Add_WallArray(4, m_vecWall[iIndex + (VTXCNTX - 1)]);
            }


            // 상단 좌측
            if (i < VTXCNTZ - 1 - 1 && j > 0)
            {
                m_vecWall[iIndex]->Add_WallArray(5, m_vecWall[iIndex + (VTXCNTX - 1) - 1]);
            }


            // 좌측
            if (j > 0)
            {
                m_vecWall[iIndex]->Add_WallArray(6, m_vecWall[iIndex - 1]);
            }


            // 하단 좌측
            if (i > 0 && j > 0)
            {
                m_vecWall[iIndex]->Add_WallArray(7, m_vecWall[iIndex - (VTXCNTX - 1) - 1]);
            }

        }
    }

    // ========================================================

    CloseHandle(m_hFile);
    CloseHandle(m_hWallFile);

    return S_OK;
}

HRESULT CStage::Load_MonsterData()
{
    auto	iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(L"Layer_GameLogic"));

    if (iter == m_mapLayer.end())
        return E_FAIL;

    const _tchar* strFileName = L"../../Data/MonsterData.txt";

    m_hFile = CreateFile(strFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == m_hFile)
    {
        MSG_BOX("Fail Open Monster file");
        return E_FAIL;
    }

    // ========================================================

    Engine::CGameObject* pGameObject = nullptr;
    Engine::CGameObject* pEye = nullptr;
    _int	iType, iIndex;
    _float	fX, fZ;
    DWORD	dwByte = 0;

    while (true)
    {
        ReadFile(m_hFile, &iType, sizeof(_int), &dwByte, nullptr);
        ReadFile(m_hFile, &iIndex, sizeof(_int), &dwByte, nullptr);

        if (dwByte == 0)
            break;

        fX = (iIndex % (VTXCNTX - 1)) * VTXITV;
        fZ = (iIndex / (VTXCNTX - 1)) * VTXITV;

        switch (iType)
        {
        case MON_SLIME:
            m_wsMonsterNameString[iIndex] = L"Slime_" + std::to_wstring(iIndex);
            pGameObject = CSlime::Create(m_pGraphicDev, _vec3(fX, 10, fZ));
            NULL_CHECK_RETURN(pGameObject, E_FAIL);
            FAILED_CHECK_RETURN(iter->second->Add_GameObject(m_wsMonsterNameString[iIndex].c_str(), pGameObject), E_FAIL);
            break;
        case MON_SHROOMMAN:
            m_wsMonsterNameString[iIndex] = L"ShroomMan_" + std::to_wstring(iIndex);
            pGameObject = CShroomMan::Create(m_pGraphicDev, _vec3(fX, 10, fZ));
            NULL_CHECK_RETURN(pGameObject, E_FAIL);
            FAILED_CHECK_RETURN(iter->second->Add_GameObject(m_wsMonsterNameString[iIndex].c_str(), pGameObject), E_FAIL);
            break;
        case MON_SHAMAN:
            m_wsMonsterNameString[iIndex] = L"Shaman_" + std::to_wstring(iIndex);
            pGameObject = CShaman::Create(m_pGraphicDev, _vec3(fX, 10, fZ));
            NULL_CHECK_RETURN(pGameObject, E_FAIL);
            FAILED_CHECK_RETURN(iter->second->Add_GameObject(m_wsMonsterNameString[iIndex].c_str(), pGameObject), E_FAIL);
            break;
        case MON_HUNTER:
            m_wsMonsterNameString[iIndex] = L"Hunter_" + std::to_wstring(iIndex);
            pGameObject = CHunter::Create(m_pGraphicDev, _vec3(fX, 10, fZ));
            NULL_CHECK_RETURN(pGameObject, E_FAIL);
            FAILED_CHECK_RETURN(iter->second->Add_GameObject(m_wsMonsterNameString[iIndex].c_str(), pGameObject), E_FAIL);

            m_wsMonsterNameString[iIndex] = L"HunterEye_" + std::to_wstring(iIndex);
            pEye = CHunterEye::Create(m_pGraphicDev);
            NULL_CHECK_RETURN(pEye, E_FAIL);
            dynamic_cast<CHunterEye*>(pEye)->Set_Hunter(pGameObject);
            FAILED_CHECK_RETURN(iter->second->Add_GameObject(m_wsMonsterNameString[iIndex].c_str(), pEye), E_FAIL);
            break;
        }

    }

    CloseHandle(m_hFile);
}

HRESULT CStage::Load_ObjectData()
{
    auto	iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(L"Layer_GameLogic"));

    if (iter == m_mapLayer.end())
        return E_FAIL;

    const _tchar* strFileName = L"../../Data/ObjectData.txt";

    m_hObjectFile = CreateFile(strFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == m_hObjectFile)
    {
        MSG_BOX("Fail Open Object file");
        return E_FAIL;
    }

    Engine::CGameObject* pGameObject = nullptr;
    _vec3	vObjectPos(0, 0, 0);
    _int	iType(0);
    _int	iTypeNumber(0);
    _int	iIndex(0);
    DWORD	dwByte = 0;

    while (true)
    {
        ReadFile(m_hObjectFile, &vObjectPos, sizeof(_vec3), &dwByte, nullptr);
        ReadFile(m_hObjectFile, &iTypeNumber, sizeof(_int), &dwByte, nullptr);
        ReadFile(m_hObjectFile, &iIndex, sizeof(_int), &dwByte, nullptr);
        ReadFile(m_hObjectFile, &iType, sizeof(_int), &dwByte, nullptr);

        if (dwByte == 0)
            break;

        switch (iType)
        {
        case SLIME_FLOOR:
            m_wsObjectNameString[iIndex] = L"SlimeFloor_" + std::to_wstring(iIndex);
            pGameObject = CSlimeFloor::Create(m_pGraphicDev, _vec3(vObjectPos.x, vObjectPos.y, vObjectPos.z), iTypeNumber, m_wsObjectNameString[iIndex].c_str());
            break;
        case MUSHROOM:
            m_wsObjectNameString[iIndex] = L"Mushroom_" + std::to_wstring(iIndex);
            pGameObject = CMushroom::Create(m_pGraphicDev, _vec3(vObjectPos.x, vObjectPos.y, vObjectPos.z), iTypeNumber, m_wsObjectNameString[iIndex].c_str());
            break;
        case AZEOS_POOP:
            m_wsObjectNameString[iIndex] = L"AzeosPoop_" + std::to_wstring(iIndex);
            pGameObject = CAzeosPoop::Create(m_pGraphicDev, _vec3(vObjectPos.x, vObjectPos.y, vObjectPos.z), iTypeNumber, m_wsObjectNameString[iIndex].c_str());
            break;
        case AZEOS_FEATHER:
            m_wsObjectNameString[iIndex] = L"AzeosFeather_" + std::to_wstring(iIndex);
            pGameObject = CAzeosFeather::Create(m_pGraphicDev, _vec3(vObjectPos.x, vObjectPos.y, vObjectPos.z), iTypeNumber, m_wsObjectNameString[iIndex].c_str());
            break;
        //case MALGA_SUMMON:
        //    m_wsObjectNameString[iIndex] = L"MalgaSpawnPoint_" + std::to_wstring(iIndex);
        //    pGameObject = CBossSpawnPoint::Create(m_pGraphicDev, _vec3(vObjectPos.x, vObjectPos.y, vObjectPos.z), iTypeNumber, m_wsObjectNameString[iIndex].c_str());
        //    break;
        case AZEOS_SUMMON:
            m_wsObjectNameString[iIndex] = L"AzeosSpawnPoint_" + std::to_wstring(iIndex);
            pGameObject = CAzeosSpawnPoint::Create(m_pGraphicDev, _vec3(vObjectPos.x, vObjectPos.y, vObjectPos.z), iTypeNumber, m_wsObjectNameString[iIndex].c_str());
            break;

        }
        NULL_CHECK_RETURN(pGameObject, E_FAIL);
        FAILED_CHECK_RETURN(iter->second->Add_GameObject(m_wsObjectNameString[iIndex].c_str(), pGameObject), E_FAIL);

    }

    CloseHandle(m_hObjectFile);
}

void CStage::Free()
{
    Engine::CScene::Free();
}
