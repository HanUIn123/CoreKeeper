#include "pch.h"
#include "..\Header\Story.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\Stage.h"

#include "../Header/MapEditorScene.h"

CStory::CStory(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev), m_pLoading(nullptr)
{
}

CStory::~CStory()
{
}

HRESULT CStory::Ready_Scene()
{
	FAILED_CHECK_RETURN(Ready_Prototype(), E_FAIL);

	m_pLoading = CLoading::Create(m_pGraphicDev, CLoading::LOADING_STAGE);
	NULL_CHECK_RETURN(m_pLoading, E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Layer_Environment"), E_FAIL);

	//Engine::PlayBGM(L"Main_Menu.wav", 0.1f);
	
	return S_OK;
}

_int CStory::Update_Scene(const _float& fTimeDelta)
{
	_int	iExit = Engine::CScene::Update_Scene(fTimeDelta);

	if (true == m_pLoading->Get_Finish())
	{
		if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		{
			//Engine::StopSound(SOUND_BGM);

			Engine::CScene* pStage = CStage::Create(m_pGraphicDev);
			NULL_CHECK_RETURN(pStage, -1);

			FAILED_CHECK_RETURN(Engine::Set_Scene(pStage), E_FAIL);

			return 0;
		}
	}

	return iExit;
}

void CStory::LateUpdate_Scene()
{
	Engine::CScene::LateUpdate_Scene();
}

void CStory::Render_Scene()
{
	if (true == m_pLoading->Get_Finish())
	{
		_vec2 pos(725, 650);
		Engine::Render_Font(L"Font_Default", L"ENTER키를 눌러 스킵", &pos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	}
}

HRESULT CStory::Ready_Prototype()
{
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_StoryTex", Engine::CRcTex::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_StoryTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Logo/Story_%d.png", TEX_NORMAL, 5)), E_FAIL);

	return S_OK;
}

HRESULT CStory::Ready_Layer_Environment(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject* pGameObject = nullptr;

	pGameObject = CStoryBackGround::Create(m_pGraphicDev);
	NULL_CHECK_RETURN(pGameObject, E_FAIL);
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"StoryBackGround", pGameObject), E_FAIL);

	m_mapLayer.insert({ pLayerTag , pLayer });

	return S_OK;
}

CStory* CStory::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStory* pStory = new CStory(pGraphicDev);

	if (FAILED(pStory->Ready_Scene()))
	{
		Safe_Release(pStory);
		MSG_BOX("Story Create Failed");
		return nullptr;
	}

	return pStory;
}

void CStory::Free()
{
	Safe_Release(m_pLoading);

	Engine::CScene::Free();
}
