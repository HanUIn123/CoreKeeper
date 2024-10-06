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
}


CStage::~CStage()
{
	CCraftMgr::DestroyInstance();
	CFarmMgr::DestroyInstance();
}

HRESULT CStage::Ready_Scene()
{
	CCraftMgr::GetInstance()->Ready_Craft(m_pGraphicDev);
	CFarmMgr::GetInstance()->Ready_Farm(m_pGraphicDev);

	//FAILED_CHECK_RETURN(Ready_LightInfo(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Layer_Environment"), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_GameLogic(L"Layer_GameLogic"), E_FAIL);


	FAILED_CHECK_RETURN(Ready_Layer_UI(L"Layer_UI"), E_FAIL);
	Load_MapFile();
	Load_MonsterData();
	// 이거랑 Render_Scene() 주석 풀면 일단 stage를 위에서 꽂아서 보게됨.
	//FAILED_CHECK_RETURN(Ready_Layer_MiniMap(L"Layer_MiniMap"), E_FAIL);
	
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	return S_OK;
}

_int CStage::Update_Scene(const _float& fTimeDelta)
{
	_int	iExit = Engine::CScene::Update_Scene(fTimeDelta);
	CFarmMgr::GetInstance()->Update_Farm(fTimeDelta);
	return iExit;
}

void CStage::LateUpdate_Scene()
{
	Engine::CScene::LateUpdate_Scene();
}

void CStage::Render_Scene()
{
	///// 미니맵 뷰포트.. 창 나누기.. 아직 구현중...
	// 시작 위치 x / 시작 위치 y / 너비 / 높이 / 최소 깊이 / 최대 깊이
	// 이 위치에 현재 stage의 모습을 그대로 보여주고있음.

	//D3DVIEWPORT9 miniMapViewport = { 1280 - 220, 20, 200, 150, 0.0f, 1.0f };  // 우측 상단
	//m_pGraphicDev->SetViewport(&miniMapViewport);
}

HRESULT CStage::Create_Inventory(const _tchar* pLayerTag, const _tchar* pGameObjectTag) 
{
	auto	iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(pLayerTag));

	if (iter == m_mapLayer.end())
		return E_FAIL;
		
	iter->second->Delete_GameMap(pGameObjectTag);
	
	return S_OK;
}

HRESULT CStage::Create_Item(const _tchar* pLayerTag, CItem* pItem, const _tchar* pItemTag)
{
	auto	iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(pLayerTag));

	if (iter == m_mapLayer.end())
		return E_FAIL;

	FAILED_CHECK_RETURN(iter->second->Add_GameObject(pItemTag, pItem), E_FAIL);

	m_mapLayer.insert({ pLayerTag, iter->second });

	return S_OK;
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


	pGameObject = CCore::Create(m_pGraphicDev, { VTXCNTX / 2, 2.3f, 21.f });
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Core", pGameObject), E_FAIL);

	pGameObject = CCoreBase::Create(m_pGraphicDev, { VTXCNTX / 2, 0.1f, 21.f });
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"CoreBase", pGameObject), E_FAIL);

	pGameObject = CStatue::Create(m_pGraphicDev, { VTXCNTX / 2, 1.3f, 27.f }, 0);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SlimeStatue", pGameObject), E_FAIL);

	pGameObject = CStatueBase::Create(m_pGraphicDev, { VTXCNTX / 2, 0.1f, 27.f }, 0);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SlimeStatueBase", pGameObject), E_FAIL);

	pGameObject = CStatue::Create(m_pGraphicDev, { VTXCNTX / 2 - 8.f, 1.3f, 23.f }, 1);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"LarvaStatue", pGameObject), E_FAIL);

	pGameObject = CStatueBase::Create(m_pGraphicDev, { VTXCNTX / 2 - 8.f, 0.1f, 23.f }, 0);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"LarvaStatueBase", pGameObject), E_FAIL);

	pGameObject = CStatue::Create(m_pGraphicDev, { VTXCNTX / 2 + 8.f, 1.3f, 23.f }, 2);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MalugazStatue", pGameObject), E_FAIL);

	pGameObject = CStatueBase::Create(m_pGraphicDev, { VTXCNTX / 2 + 8.f, 0.1f, 23.f }, 2);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MalugazStatueBase", pGameObject), E_FAIL);

	pGameObject = CSpawnPoint::Create(m_pGraphicDev, { VTXCNTX / 2, 0.1f, 17.f});
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SpawnPoint", pGameObject), E_FAIL);

	pGameObject = CWood::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Wood", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Add_Count(99);

	CItem* pItem = dynamic_cast<CItem*>(pGameObject);

	pGameObject = CGravestoneObject::Create(m_pGraphicDev, { 78.f, 0.5f, 18.f });
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"AheadGrave", pGameObject), E_FAIL);
	dynamic_cast<CGravestoneObject*>(pGameObject)->Add_Item(pItem);

	pGameObject = CSkeleton::Create(m_pGraphicDev, { 77.f, 0.5f, 18.f });
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Skeleton", pGameObject), E_FAIL);

	pGameObject = CTableObject::Create(m_pGraphicDev, { 83.f, 0.5f, 16.f }, MATERIAL_WOOD);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"WoodCraftTable", pGameObject), E_FAIL);

	pGameObject = CTableObject::Create(m_pGraphicDev, { 81.f, 0.5f, 16.f }, MATERIAL_COPPER);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"CopperCraftTable", pGameObject), E_FAIL);

	pGameObject = CTableObject::Create(m_pGraphicDev, { 79.f, 0.5f, 16.f }, MATERIAL_IRON);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"IronCraftTable", pGameObject), E_FAIL);

	pGameObject = CFurnaceObject::Create(m_pGraphicDev, { 77.f, .5f, 16.f });
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Furnace", pGameObject), E_FAIL);

	pGameObject = CCookingPotObject::Create(m_pGraphicDev, { 75.f, .5f, 16.f });
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"CookingPot", pGameObject), E_FAIL);

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

	/*pGameObject = CMalugaz::Create(m_pGraphicDev, _vec3(VTXCNTX * 0.5f - 5.f, 10, VTXCNTZ * 0.5f - 5.f));
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Malugaz", pGameObject), E_FAIL);*/

	//CGameObject* pHunter = CHunter::Create(m_pGraphicDev, _vec3(VTXCNTX / 2, 0, 17.f));
	//NULL_CHECK_RETURN(pHunter, E_FAIL);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Hunter", pHunter), E_FAIL);

	//pGameObject = CHunterEye::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//dynamic_cast<CHunterEye*>(pGameObject)->Set_Hunter(pHunter);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"HunterEye", pGameObject), E_FAIL);

	pGameObject = CTorch::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Torch", pGameObject), E_FAIL);

	pGameObject = CPickaxe::Create(m_pGraphicDev, MATERIAL_COPPER);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Pickaxe", pGameObject), E_FAIL);

	pGameObject = CHoe::Create(m_pGraphicDev, MATERIAL_COPPER);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Hoe", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CSword::Create(m_pGraphicDev, MATERIAL_COPPER);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Sword", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);


	pGameObject = CBow::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Bow1", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CArrow::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Arrow", pGameObject), E_FAIL);

	pGameObject = CMagic::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Magic", pGameObject), E_FAIL);

	pGameObject = CStaff::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Staff1", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CHelmet::Create(m_pGraphicDev, MATERIAL_COPPER);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Helmet", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CChest::Create(m_pGraphicDev, MATERIAL_COPPER);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Chest", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CLeg::Create(m_pGraphicDev, MATERIAL_COPPER);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Leg", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CBar::Create(m_pGraphicDev, MATERIAL_COPPER);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"CopperBar", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = COre::Create(m_pGraphicDev, MATERIAL_COPPER);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"CopperOre", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = COre::Create(m_pGraphicDev, MATERIAL_COPPER);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"CopperOre2", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = COre::Create(m_pGraphicDev, MATERIAL_IRON);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"IronOre", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = COre::Create(m_pGraphicDev, MATERIAL_SCARLET);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"ScarletOre", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CAssistance::Create(m_pGraphicDev, ASSISTANCE_FEATHER);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Feather", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CWateringCan::Create(m_pGraphicDev, MATERIAL_COPPER);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"WateringCan", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CSeed::Create(m_pGraphicDev, ITEM_PEPPER_SEED);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Seed", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CSeed::Create(m_pGraphicDev, ITEM_PEPPER_SEED);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Seed1", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CSeed::Create(m_pGraphicDev, ITEM_PEPPER_SEED);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Seed2", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CIngredient::Create(m_pGraphicDev, ITEM_BERRY_SEED);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Berry", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);


	pGameObject = CIngredient::Create(m_pGraphicDev, ITEM_PEPPER_SEED);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Pepper", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CTable::Create(m_pGraphicDev, MATERIAL_WOOD);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Table_Test", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);


	pGameObject = CMusicTable::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MusicTable_Test", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);


	//pGameObject = CCore::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Core", pGameObject), E_FAIL);

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
	for (int i = 10; i < 30; i++)
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

	vPos = { 60.f, 130.f };
	vSize = { 20.f, 20.f };

	pGameObject = CUIBuff::Create(m_pGraphicDev, vPos, vSize);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_Buff0", pGameObject), E_FAIL);

	vPos = { 110.f, 130.f };
	vSize = { 20.f, 20.f };

	pGameObject = CUIBuff::Create(m_pGraphicDev, vPos, vSize);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_DeBuff0", pGameObject), E_FAIL);

	pGameObject = CMiniMapFrame::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MiniFrame", pGameObject), E_FAIL);

	//pGameObject = CWorldMap::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"WorldMap", pGameObject), E_FAIL);

	pGameObject = CMiniObject::Create(m_pGraphicDev , { 128.5f, 0.1f, 128.5f });
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MiniPlayer", pGameObject), E_FAIL);

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

	pGameObject = CMiniCore::Create(m_pGraphicDev, { VTXCNTX / 2, 1.5f, 21.5f });
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

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CStage::Ready_Layer_MiniMap(const _tchar* pLayerTag)
{
	//Engine::CLayer* pLayer = CLayer::Create();
	//NULL_CHECK_RETURN(pLayer, E_FAIL);

	//_vec3 miniMapEye = { 0.f, 20.f, 0.f };  
	//_vec3 miniMapAt = { 0.f, 0.f, 0.f };     
	//_vec3 miniMapUp = { 0.f, 0.f, 1.f };     

	//m_pMiniMapCamera = CMiniMapCamera::Create(m_pGraphicDev, &miniMapEye, &miniMapAt, &miniMapUp);
	//NULL_CHECK_RETURN(m_pMiniMapCamera, E_FAIL);

	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MiniMapCamera", m_pMiniMapCamera), E_FAIL);

	//m_mapLayer.insert({ pLayerTag, pLayer });

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
	m_hObjectFile = CreateFile(strObjectFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
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

	if (INVALID_HANDLE_VALUE == m_hObjectFile)
	{
		MSG_BOX("Fail Open Object file");
		return E_FAIL;
	}

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

		m_wsWallNameString[vTempIndex] = L"Wall_" + std::to_wstring(vTempIndex);
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
				m_vecWall[iIndex]->Add_WallArray(0, m_vecWall[iIndex - (VTXCNTX-1)]);
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

	DWORD dwByte3 = 0;
	_vec3 vTempBuildingPos(0.0f, 0.0f, 0.0f);
	_int vTempBuildingImgNum(0);
	_int vTempBuildingIndex(0);

	while (true)
	{
		ReadFile(m_hObjectFile, &vTempBuildingPos, sizeof(_vec3), &dwByte3, nullptr);
		ReadFile(m_hObjectFile, &vTempBuildingImgNum, sizeof(_int), &dwByte3, nullptr);
		ReadFile(m_hObjectFile, &vTempBuildingIndex, sizeof(_int), &dwByte3, nullptr);

		if (dwByte3 == 0)
			break;

		m_wsBuildingNameString[vTempBuildingIndex] = L"Building_" + std::to_wstring(vTempBuildingIndex);

		switch (vTempBuildingImgNum)
		{
		case 0:
			pGameObject = CCore::Create(m_pGraphicDev, { vTempBuildingPos.x, 3.0f, vTempBuildingPos.z });
			break;
		case 1:
			break;
		case 2:
			break;
		}
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(iter->second->Add_GameObject(m_wsBuildingNameString[vTempBuildingIndex].c_str(), pGameObject), E_FAIL);
	}

	CloseHandle(m_hFile);
	CloseHandle(m_hWallFile);
	CloseHandle(m_hObjectFile);

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
			break;
		case MON_SHROOMMAN:
			m_wsMonsterNameString[iIndex] = L"ShroomMan_" + std::to_wstring(iIndex);
			pGameObject = CShroomMan::Create(m_pGraphicDev, _vec3(fX, 10, fZ));
			break;
		case MON_SHAMAN:
			m_wsMonsterNameString[iIndex] = L"Shaman_" + std::to_wstring(iIndex);
			pGameObject = CShaman::Create(m_pGraphicDev, _vec3(fX, 10, fZ));
			break;
		case MON_HUNTER:
			m_wsMonsterNameString[iIndex] = L"Hunter_" + std::to_wstring(iIndex);
			pGameObject = CHunter::Create(m_pGraphicDev, _vec3(fX, 10, fZ));
			break;
		}
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(iter->second->Add_GameObject(m_wsMonsterNameString[iIndex].c_str(), pGameObject), E_FAIL);
	}

	CloseHandle(m_hFile);
}

void CStage::Free()
{
	Engine::CScene::Free();
}
