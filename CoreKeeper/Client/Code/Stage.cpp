#include "pch.h"
#include "..\Header\Stage.h"
#include "Export_Utility.h"
#include "..\Header\DynamicCamera.h"

CStage::CStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
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

HRESULT CStage::Create_GameObject(const _tchar* pLayerTag) // CGameObject* _pObject, const _tchar* pObjectTag
{
	auto	iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(pLayerTag));

	if (iter == m_mapLayer.end())
		return E_FAIL;

	Engine::CGameObject* pGameObject = nullptr;

	_vec2 vPos = { 50.f,  50.f };
	_vec2 vSize = { 2.f, 6.f };


	wstring string = L"UIHpDivider";


	pGameObject = CHpDivider::Create(m_pGraphicDev, vPos, vSize, 0);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(iter->second->Add_GameObject(L"UIHpDivider", pGameObject), E_FAIL);


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

	pGameObject = CSlime::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Slime", pGameObject), E_FAIL);
	
	pGameObject = CSword::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Sword", pGameObject), E_FAIL);
	 
	//pGameObject = CItem::Create(m_pGraphicDev);
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
	/*
	pGameObject = CUIStatus::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UIStatus", pGameObject), E_FAIL); // UI Ãß°¡
	*/
	_vec2 vPos = { 780.f, 480.f };
	_vec2 vSize = { 35.f, 35.f };

	pGameObject = CUIScreenIcon::Create(m_pGraphicDev, vPos, vSize, 0);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UIScreenicon_Bag", pGameObject), E_FAIL);

	vPos = { 850.f , 480.f };

	pGameObject = CUIScreenIcon::Create(m_pGraphicDev, vPos, vSize, 1);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UIScreenicon_Map", pGameObject), E_FAIL);

	vPos = { 850.f , 550.f };

	pGameObject = CUIScreenIcon::Create(m_pGraphicDev, vPos, vSize, 2);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UIScreenicon_Hand", pGameObject), E_FAIL);

	vPos = { 780.f , 550.f };

	pGameObject = CUIScreenIcon::Create(m_pGraphicDev, vPos, vSize, 4);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UIScreenicon_Install", pGameObject), E_FAIL);

	vPos = { 347.f, 50.f };
	vSize = { 293.f, 9.f };

	pGameObject = CUIHealth::Create(m_pGraphicDev, vPos, vSize, 1);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_Health", pGameObject), E_FAIL);

	vPos = { 50.f,  50.f };
	vSize = { 2.f, 6.f };
	
	pGameObject = CHpDivider::Create(m_pGraphicDev, vPos, vSize, 2);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	/*
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_Health_Divider_1", pGameObject), E_FAIL);

	pGameObject = CHpDivider::Create(m_pGraphicDev, vPos, vSize, 2);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_Health_Divider_2", pGameObject), E_FAIL);

	pGameObject = CHpDivider::Create(m_pGraphicDev, vPos, vSize, 2);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_Health_Divider_3", pGameObject), E_FAIL);

	pGameObject = CHpDivider::Create(m_pGraphicDev, vPos, vSize, 2);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_Health_Divider_4", pGameObject), E_FAIL);

	pGameObject = CHpDivider::Create(m_pGraphicDev, vPos, vSize, 2);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_Health_Divider_5", pGameObject), E_FAIL);

	pGameObject = CHpDivider::Create(m_pGraphicDev, vPos, vSize, 2);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_Health_Divider_6", pGameObject), E_FAIL);

	pGameObject = CHpDivider::Create(m_pGraphicDev, vPos, vSize, 2);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_Health_Divider_7", pGameObject), E_FAIL);
	*/
	vPos = { 347.f, 80.f };
	vSize = { 293.f, 9.f };

	pGameObject = CUIHealth::Create(m_pGraphicDev, vPos, vSize, 4);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_Mp", pGameObject), E_FAIL);


	vPos = { 340.f, 550.f };

	pGameObject = CUIScreenInv::Create(m_pGraphicDev, vPos, 1);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_ScreenInv1", pGameObject), E_FAIL);

	vPos = { 383.f, 550.f };

	pGameObject = CUIScreenInv::Create(m_pGraphicDev, vPos, 2);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI_ScreenInv2", pGameObject), E_FAIL);

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
