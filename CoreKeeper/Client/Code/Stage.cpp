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
	m_vecUnreachable.resize((VTXCNTX - 1) * (VTXCNTZ - 1));
}


CStage::~CStage()
{
}

HRESULT CStage::Ready_Scene()
{
	//FAILED_CHECK_RETURN(Ready_LightInfo(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Layer_Environment"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_GameLogic(L"Layer_GameLogic"), E_FAIL);

	Load_MapFile();

	FAILED_CHECK_RETURN(Ready_Layer_UI(L"Layer_UI"), E_FAIL);

	// 이거랑 Render_Scene() 주석 풀면 일단 stage를 위에서 꽂아서 보게됨.
	//FAILED_CHECK_RETURN(Ready_Layer_MiniMap(L"Layer_MiniMap"), E_FAIL);
	
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	return S_OK;
}

_int CStage::Update_Scene(const _float& fTimeDelta)
{
	_int	iExit = Engine::CScene::Update_Scene(fTimeDelta);

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

	_vec3 eye(0.f, 10.f, -10.f);
	_vec3 at(0.f, 0.f, 1.f);
	_vec3 up(0.f, 1.f, 0.f);

	pGameObject = CDynamicCamera::Create(m_pGraphicDev,
		&eye,
		&at,
		&up);

	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"DynamicCamera", pGameObject), E_FAIL);


	pGameObject = CSkyBox::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SkyBox", pGameObject), E_FAIL);


	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CStage::Ready_Layer_GameLogic(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	pGameObject = CTerrain::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Terrain", pGameObject), E_FAIL);

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

	pGameObject = CSlime::Create(m_pGraphicDev, _vec3(5, 5, 5));
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Slime", pGameObject), E_FAIL);

	pGameObject = CMushroom::Create(m_pGraphicDev, _vec3(10, 10, 10));
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Mushroom", pGameObject), E_FAIL);

	//pGameObject = CTorch::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Torch", pGameObject), E_FAIL);

	pGameObject = CPickaxe::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Pickaxe", pGameObject), E_FAIL);

	pGameObject = CHoe::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Hoe", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CShovel::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Shovel", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CSword::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Sword", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CSword::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Sword2", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CBow::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Bow1", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CStaff::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Staff1", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CSeed::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Seed", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CSeed::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Seed1", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CSeed::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Seed2", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CSeed::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Seed3", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CSeed::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Seed4", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CWood::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Wood1", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CWood::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Wood2", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CWood::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Wood3", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CWood::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Wood4", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CWood::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Wood5", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CWood::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Wood6", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CHelmet::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Helmet", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CChest::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Chest", pGameObject), E_FAIL);
	dynamic_cast<CItem*>(pGameObject)->Set_Drop(true);

	pGameObject = CLeg::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Leg", pGameObject), E_FAIL);
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

	vPos = { 470.f, 225.f };
	vSize = { 120.f, 120.f };

	pGameObject = CUIPlayerCraft::Create(m_pGraphicDev, vPos, vSize);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UIPlayerCraft", pGameObject), E_FAIL);

	for (int i = 0; i < 5; i++)
	{
		if (i < 3)
		{
			vPos = { 400.f + i * 70.f , 220.f };
		}
		else
			vPos = { 400.f + (i - 3) * 70.f, 220.f + (i / 3) * 70.f };

		vSize = { 30.f, 30.f };

		m_CraftSlot[i] = L"UICraftSlot_" + std::to_wstring(i);

		pGameObject = CUICraftSlot::Create(m_pGraphicDev, vPos, vSize, i);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(m_CraftSlot[i].c_str(), pGameObject), E_FAIL);
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

	pGameObject = CUICursor::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_Cursor", pGameObject), E_FAIL)

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

	pGameObject = CUISort::Create(m_pGraphicDev, vPos, vSize);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_Sort", pGameObject), E_FAIL);

	vPos = { 1130.f, 190.f };
	vSize = { 130.f, 150.f };

	pGameObject = CUIStatusFrame::Create(m_pGraphicDev, vPos, vSize);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_StatusFrame", pGameObject), E_FAIL);

	vPos = { WINCX / 2.f, WINCY / 2.f };
	vSize = { 130.f, 150.f };

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

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CStage::Ready_Layer_MiniMap(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	_vec3 miniMapEye = { 0.f, 20.f, 0.f };  
	_vec3 miniMapAt = { 0.f, 0.f, 0.f };     
	_vec3 miniMapUp = { 0.f, 0.f, 1.f };     

	m_pMiniMapCamera = CMiniMapCamera::Create(m_pGraphicDev, &miniMapEye, &miniMapAt, &miniMapUp);
	NULL_CHECK_RETURN(m_pMiniMapCamera, E_FAIL);

	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MiniMapCamera", m_pMiniMapCamera), E_FAIL);

	m_mapLayer.insert({ pLayerTag, pLayer });

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

	m_hFile = CreateFile(strFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	m_hWallFile = CreateFile(strWallFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == m_hFile)
	{
		MSG_BOX("Fail Open file");
		return E_FAIL;
	}

	if (INVALID_HANDLE_VALUE == m_hWallFile)
	{
		MSG_BOX("Fail Open Wall file");
		return E_FAIL;
	}

	_vec3 vTempTilePos(0.0f, 0.0f, 0.0f);
	_int vTempTileImgNum(0);

	DWORD dwByte = 0;

	_vec3 vTempWallPos(0.0f, 0.0f, 0.0f);
	_int vTempWallImgNum(0);
	_int vTempIndex(0);

	DWORD dwByte2 = 0;


	//while (true)
	//{
	//	ReadFile(m_hFile, &vTempTilePos, sizeof(_vec3), &dwByte, nullptr);
	//	ReadFile(m_hFile, &vTempTileImgNum, sizeof(_int), &dwByte, nullptr);

	//	if (dwByte == 0)  
	//		break;

	//	CTile* pTile = CTile::Create(m_pGraphicDev, vTempTilePos.x, vTempTilePos.z, vTempTileImgNum);
	//	NULL_CHECK_RETURN(pTile, E_FAIL);
	//	m_wsTileNameString[m_iLoadTileCount] = L"Tile_" + std::to_wstring(m_iLoadTileCount);
	//	FAILED_CHECK_RETURN(iter->second->Add_GameObject(m_wsTileNameString[m_iLoadTileCount].c_str(), pTile), E_FAIL);
	//	m_iLoadTileCount++;		
	//}

	while (true)
	{
		ReadFile(m_hWallFile, &vTempWallPos, sizeof(_vec3), &dwByte2, nullptr);
		ReadFile(m_hWallFile, &vTempWallImgNum, sizeof(_int), &dwByte2, nullptr);
		ReadFile(m_hWallFile, &vTempIndex, sizeof(_int), &dwByte2, nullptr);

		if (dwByte2 == 0)
			break;

		m_wsWallNameString[vTempIndex] = L"Wall_" + std::to_wstring(vTempIndex);
		CWall* pWall = CWall::Create(m_pGraphicDev, vTempWallPos.x, vTempWallPos.z, vTempWallImgNum, m_wsWallNameString[vTempIndex].c_str());
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
				m_vecWall[iIndex]->Add_WallArray(0, m_vecWall[iIndex - VTXCNTX - 1]);

			// 하단 우측
			if (i > 0 && j < VTXCNTX - 1 - 1)
				m_vecWall[iIndex]->Add_WallArray(1, m_vecWall[iIndex - VTXCNTX - 1 + 1]);

			// 우측
			if (j < VTXCNTX - 1 - 1)
				m_vecWall[iIndex]->Add_WallArray(2, m_vecWall[iIndex + 1]);

			// 상단 우측
			if (i < VTXCNTZ - 1 - 1 && j < VTXCNTX - 1 - 1)
				m_vecWall[iIndex]->Add_WallArray(3, m_vecWall[iIndex + VTXCNTX - 1 + 1]);

			// 상단
			if (i < VTXCNTZ - 1 - 1)
				m_vecWall[iIndex]->Add_WallArray(4, m_vecWall[iIndex + VTXCNTX - 1]);

			// 상단 좌측
			if (i < VTXCNTZ - 1 - 1 && j > 0)
				m_vecWall[iIndex]->Add_WallArray(5, m_vecWall[iIndex + VTXCNTX - 1 - 1]);

			// 좌측
			if (j > 0)
				m_vecWall[iIndex]->Add_WallArray(6, m_vecWall[iIndex - 1]);

			// 하단 좌측
			if (i > 0 && j > 0)
				m_vecWall[iIndex]->Add_WallArray(7, m_vecWall[iIndex - VTXCNTX - 1 - 1]);
		}
	}

	CloseHandle(m_hFile);
	CloseHandle(m_hWallFile);
	MSG_BOX("Success Load File");

	return S_OK;
}

void CStage::Free()
{
	Engine::CScene::Free();
}
