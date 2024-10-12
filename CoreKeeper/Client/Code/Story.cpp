#include "pch.h"
#include "..\Header\Story.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\Stage.h"

#include "../Header/MapEditorScene.h"

#include "../Header/BlackPlaneMgr.h"

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
	Engine::CSoundMgr::GetInstance()->PlayBGM(L"Intro.wav", 0.1f);

	_int	iExit = Engine::CScene::Update_Scene(fTimeDelta);

    CWhitePlane* pWhitePlane = dynamic_cast<CWhitePlane*>(Engine::Get_GameObject(L"Layer_Environment", L"WhitePlane"));
    CStoryBackGround* pStoryBackGround = dynamic_cast<CStoryBackGround*>(Engine::Get_GameObject(L"Layer_Environment", L"StoryBackGround"));

    if (true == m_pLoading->Get_Finish())
    {
        if (GetAsyncKeyState(VK_RETURN) & 0x8000)
        {
            pStoryBackGround->Set_Stop(true);

            if (pStoryBackGround->Get_Stop())
            {
                pWhitePlane->Set_FadeOver(true);
            }
        }

        if (pWhitePlane->Get_Opacity() == 240)
        {
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
        _vec2 pos(975, 650);
        Engine::Render_Font(L"Font_Skip", L"ENTER 키를 눌러 스킵", &pos, D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f));
    }
}

HRESULT CStory::Ready_Prototype()
{
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_StoryTex", Engine::CRcTex::Create(m_pGraphicDev)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_StoryTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Logo/Story_%d.png", TEX_NORMAL, 5)), E_FAIL);

    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_WhiteTransform", Engine::CTransform::Create(m_pGraphicDev)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_WhiteTex", Engine::CRcTex::Create(m_pGraphicDev)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_WhiteTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Logo/BlackScreen2.png", TEX_NORMAL, 1)), E_FAIL);


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

    pGameObject = CWhitePlane::Create(m_pGraphicDev);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"WhitePlane", pGameObject), E_FAIL);

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
