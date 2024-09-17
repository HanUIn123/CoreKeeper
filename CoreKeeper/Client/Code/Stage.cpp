#include "pch.h"
#include "..\Header\Stage.h"
#include "Export_Utility.h"
#include "..\Header\DynamicCamera.h"

CStage::CStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev), m_bInvCheck(false)
{
}


CStage::~CStage()
{
}

HRESULT CStage::Ready_Scene()
{
	FAILED_CHECK_RETURN(Ready_LightInfo(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Layer_Environment"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_GameLogic(L"Layer_GameLogic"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_UI(L"Layer_UI"), E_FAIL);

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

}

HRESULT CStage::Create_Inventory(const _tchar* pLayerTag) 
{
	/*if (!m_bInvCheck)
	{
		auto	iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(pLayerTag));

		if (iter == m_mapLayer.end())
			return E_FAIL;

		Engine::CGameObject* pGameObject = nullptr;

		_vec2 vPos = { 500.f, 200.f };
		_vec2 vSize = { 230.f, 25.f };

		pGameObject = CUIInvPlate::Create(m_pGraphicDev, vPos, vSize);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(iter->second->Add_GameObject(L"UI_Plate", pGameObject), E_FAIL);

		m_mapLayer.insert({ pLayerTag, iter->second });

		m_bInvCheck = true;
	}
	else
	{
		m_bInvCheck = false;

		auto	iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(pLayerTag));

		if (iter == m_mapLayer.end())
			return E_FAIL;
			
		iter->second->Delete_GameMap(L"UI_Plate");
	}
	*/
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

	pGameObject = CSlime::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Slime", pGameObject), E_FAIL);


	pGameObject = CSword::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Sword", pGameObject), E_FAIL);

	pGameObject = CSword::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Sword2", pGameObject), E_FAIL);


	pGameObject = CSword::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Sword3", pGameObject), E_FAIL);

	pGameObject = CSword::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Sword4", pGameObject), E_FAIL);

	pGameObject = CSword::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Sword5", pGameObject), E_FAIL);

	/*
	pGameObject = CSword::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Sword6", pGameObject), E_FAIL);
	pGameObject = CSword::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Sword7", pGameObject), E_FAIL);
	pGameObject = CSword::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Sword8", pGameObject), E_FAIL);
	pGameObject = CSword::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Sword9", pGameObject), E_FAIL);
	pGameObject = CSword::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Sword10", pGameObject), E_FAIL);
	pGameObject = CSword::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Sword11", pGameObject), E_FAIL);*/

	
	pGameObject = CSeed::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Seed", pGameObject), E_FAIL);

	pGameObject = CSeed::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Seed1", pGameObject), E_FAIL);

	pGameObject = CSeed::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Seed2", pGameObject), E_FAIL);

	pGameObject = CSeed::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Seed3", pGameObject), E_FAIL);

	pGameObject = CSeed::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Seed4", pGameObject), E_FAIL);
	// 
	// 	//pGameObject = CItem::Create(m_pGraphicDev);
	//NULL_CHECK_RETURN(pGameObject, E_FAIL);
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Item", pGameObject), E_FAIL);

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

HRESULT CStage::Ready_Layer_UI(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	_vec2 vPos = { 580.f, 200.f };
	_vec2 vSize = {100.f, 100.f};
	
	pGameObject = CUIPlayerStatus::Create(m_pGraphicDev, vPos, vSize);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UIPlayerStatus", pGameObject), E_FAIL); // UI Ãß°¡

	for (int i = 0; i < 10; i++)
	{
		if (i < 3)
		{
			vPos = { 514.f + i * 58.f , 132.f };
		}
		else if( i >= 3 && i < 5)
		{
			vPos = { 514.f + (i - 3) * 116.f , 132.f + 45.f };
		}
		else if( i >= 5 && i <7)
		{
			vPos = { 514.f + (i - 5) * 116.f , 132.f + 90.f };
		}
		else if (i >= 7 && i < 10)
		{
			vPos = { 514.f + (i - 7) * 58.f , 132.f + 135.f };
		}
		vSize = { 20.f, 20.f };

		m_ItemSlot[i] = L"UIItemSlot_" + std::to_wstring(i);

		CUIItemSlot::SLOTTYPE eType;
		switch (i)
		{
		case 0:
			eType =	CUIItemSlot::SLOT_HELM;
			break;

		case 1:
			eType = CUIItemSlot::SLOT_NECKLACE;
			break;

		case 2:
			eType = CUIItemSlot::SLOT_RING1;
			break;

		case 3:
			eType = CUIItemSlot::SLOT_CHEST;
			break;

		case 4:
			eType = CUIItemSlot::SLOT_RING2;
			break;

		case 5:
			eType = CUIItemSlot::SLOT_LEGGINGS;
			break;

		case 6:
			eType = CUIItemSlot::SLOT_WEAPON;
			break;

		case 7:
			eType = CUIItemSlot::SLOT_LANTTERN;
			break;

		case 8:
			eType = CUIItemSlot::SLOT_BAG;
			break;

		case 9:
			eType = CUIItemSlot::SLOT_PET;
			break;

		}
		pGameObject = CUIItemSlot::Create(m_pGraphicDev, vPos, vSize, eType);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(m_ItemSlot[i].c_str(), pGameObject), E_FAIL);
	}

	vPos = { 663.f, 200.f };
	vSize = { 7.f, 90.f };

	pGameObject = CUIPlayerStats::Create(m_pGraphicDev, vPos, vSize);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UIPlayerStats", pGameObject), E_FAIL);

	/*
	vPos = { 663.f, 269.f };
	vSize = { 7.f, 20.f };

	pGameObject = CUIPlayerStats::Create(m_pGraphicDev, vPos, vSize);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UIPetStats", pGameObject), E_FAIL);
	*/

	vPos = { 780.f, 480.f };
	vSize = { 25.f, 35.f };

	pGameObject = CUIScreenIcon::Create(m_pGraphicDev, vPos, vSize, CUIScreenIcon::ICON_MAP);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UIScreenicon_Map", pGameObject), E_FAIL);

	vSize = { 35.f, 35.f };
	vPos = { 850.f , 480.f };

	pGameObject = CUIScreenIcon::Create(m_pGraphicDev, vPos, vSize, CUIScreenIcon::ICON_BAG);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UIScreenicon_Bag", pGameObject), E_FAIL);

	vSize = { 25.f , 25.f };
	vPos = { 845.f , 550.f };

	pGameObject = CUIScreenIcon::Create(m_pGraphicDev, vPos, vSize, CUIScreenIcon::ICON_HAND);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UIScreenicon_Hand", pGameObject), E_FAIL);

	/*
	vPos = { 780.f , 550.f };

	pGameObject = CUIScreenIcon::Create(m_pGraphicDev, vPos, vSize, 4);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UIScreenicon_Install", pGameObject), E_FAIL);
	*/

	vPos = { 347.f, 50.f };
	vSize = { 293.f, 9.f };

	pGameObject = CUIStatusBar::Create(m_pGraphicDev, vPos, vSize, 1);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_Health", pGameObject), E_FAIL);

	vPos = { 347.f, 80.f };
	vSize = { 293.f, 9.f };

	pGameObject = CUIStatusBar::Create(m_pGraphicDev, vPos, vSize, 4);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_Mp", pGameObject), E_FAIL);

	for (int i = 0; i < 10; i++)
	{
		if (i == 0)
		{
			vPos = { 670.f, 550.f };
		}
		else
			vPos = { 240.f + (43.f * i), 550.f };
			
		m_Invstring[i] = L"UI_ScreenInv_" + std::to_wstring(i);

		pGameObject = CUIScreenInv::Create(m_pGraphicDev, vPos, i);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(m_Invstring[i].c_str(), pGameObject), E_FAIL);
	}

	for (int i = 11; i < 31; i++)
	{
		if(i % 10 != 0)
			vPos = { 240.f + (43.f * (_float)(i - (i / 10) * 10)), 350.f + (_float)(i / 10) * 43.f};
		else
			vPos = { 240.f + (43.f * (_float)(i - (((i - 1)  / 10) * 10 ))), 350.f + (_float)((i - 1) / 10) * 43.f };

		m_Invstring[i] = L"UI_Inventory_" + std::to_wstring(i);

		pGameObject = CUIInventory::Create(m_pGraphicDev, vPos, i);
		NULL_CHECK_RETURN(pGameObject, E_FAIL);
		FAILED_CHECK_RETURN(pLayer->Add_GameObject(m_Invstring[i].c_str(), pGameObject), E_FAIL);
	}

	pGameObject = CUICursor::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_Cursor", pGameObject), E_FAIL)

	vPos = { 475.f, 339.f };
	vSize = { 220.f, 25.f };

	pGameObject = CUIInvPlate::Create(m_pGraphicDev, vPos, vSize);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_Plate", pGameObject), E_FAIL);

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

void CStage::Free()
{
	Engine::CScene::Free();
}
