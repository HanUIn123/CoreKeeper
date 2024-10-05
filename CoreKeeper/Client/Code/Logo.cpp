#include "pch.h"
#include "..\Header\Logo.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\Story.h"

#include "../Header/MapEditorScene.h"

CLogo::CLogo(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
}

CLogo::~CLogo()
{
}

HRESULT CLogo::Ready_Scene()
{
	FAILED_CHECK_RETURN(Ready_Prototype(), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Layer_Environment"), E_FAIL);

	Engine::PlayBGM(L"Main_Menu.wav", 0.1f);
	
	return S_OK;
}

_int CLogo::Update_Scene(const _float& fTimeDelta)
{
	_int	iExit = Engine::CScene::Update_Scene(fTimeDelta);

	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		Engine::StopSound(SOUND_BGM);
	
		Engine::CScene* pStage = CStory::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pStage, -1);
	
		FAILED_CHECK_RETURN(Engine::Set_Scene(pStage), E_FAIL);
	
		return 0;
	}
	
	if (GetAsyncKeyState('M') & 0x8000)
	{
		Engine::StopSound(SOUND_BGM);
	
		Engine::CScene* pStage = CMapEditorScene::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pStage, -1);
	
		FAILED_CHECK_RETURN(Engine::Set_Scene(pStage), E_FAIL);
	
		ShowCursor(true);
	
		return 0;
	}

	return iExit;
}

void CLogo::LateUpdate_Scene()
{
	Engine::CScene::LateUpdate_Scene();
}

void CLogo::Render_Scene()
{
	_vec2 pos(125, 400);

	// DEBUG용 출력
	Engine::Render_Font(L"Font_Default", L"시작하려면 아무키나 누르세요.", &pos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
}

HRESULT CLogo::Ready_Prototype()
{
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BackTex", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Logo/Background.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BackAnimTex", Engine::CAnimTex::Create(m_pGraphicDev, 4, 1)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BackAnim", Engine::CAnimator::Create(m_pGraphicDev)), E_FAIL);

	return S_OK;
}

HRESULT CLogo::Ready_Layer_Environment(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	pGameObject = CBackGround::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"BackGround", pGameObject), E_FAIL);

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

CLogo* CLogo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLogo* pLogo = new CLogo(pGraphicDev);

	if (FAILED(pLogo->Ready_Scene()))
	{
		Safe_Release(pLogo);
		MSG_BOX("Logo Create Failed");
		return nullptr;
	}

	return pLogo;
}

void CLogo::Free()
{
	Engine::CScene::Free();
}
