#include "pch.h"
#include "../Header/MapEditorScene.h"
#include "Export_Utility.h"
#include "Export_System.h"
#include "..\Header\DynamicCamera.h"

CMapEditorScene::CMapEditorScene(LPDIRECT3DDEVICE9 _pGraphicDevice)
    :Engine::CScene(_pGraphicDevice)
{
}

CMapEditorScene::~CMapEditorScene()
{
}


HRESULT CMapEditorScene::Ready_LightInfo()
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

HRESULT CMapEditorScene::Ready_Scene()
{

    FAILED_CHECK_RETURN(Ready_LightInfo(), E_FAIL);
    FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Layer_Environment"), E_FAIL);
    FAILED_CHECK_RETURN(Ready_Layer_GameLogic(L"Layer_GameLogic"), E_FAIL);
    FAILED_CHECK_RETURN(Ready_Layer_UI(L"Layer_UI"), E_FAIL);

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    return S_OK;
}

_int CMapEditorScene::Update_Scene(const _float& fTimeDelta)
{
    _int	iExit = Engine::CScene::Update_Scene(fTimeDelta);


    return iExit;
}

void CMapEditorScene::LateUpdate_Scene()
{
    Engine::CScene::LateUpdate_Scene();
}

void CMapEditorScene::Render_Scene()
{
    //ImGui::Begin("Window2");
    //ImGui::Text("This is Sample Window");
    //ImGui::End();

    Show_ImguiWindow();
}

HRESULT CMapEditorScene::Ready_Layer_Environment(const _tchar* pLayerTag)
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

    m_mapLayer.insert({ pLayerTag , pLayer });

    return S_OK;
}

HRESULT CMapEditorScene::Ready_Layer_GameLogic(const _tchar* pLayerTag)
{
    Engine::CLayer* pLayer = CLayer::Create();
    NULL_CHECK_RETURN(pLayer, E_FAIL);

    Engine::CGameObject* pGameObject = nullptr;

    pGameObject = CTerrain::Create(m_pGraphicDev);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"CTerrain", pGameObject), E_FAIL);

    m_mapLayer.insert({ pLayerTag , pLayer });
    return S_OK;
}

HRESULT CMapEditorScene::Ready_Layer_UI(const _tchar* pLayerTag)
{
    Engine::CLayer* pLayer = CLayer::Create();
    NULL_CHECK_RETURN(pLayer, E_FAIL);

    m_mapLayer.insert({ pLayerTag , pLayer });

    return S_OK;
}

CMapEditorScene* CMapEditorScene::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMapEditorScene* pMapEditorScene = new CMapEditorScene(pGraphicDev);

    if (FAILED(pMapEditorScene->Ready_Scene()))
    {
        Safe_Release(pMapEditorScene);
        MSG_BOX("Stage Create Failed");
        return nullptr;
    }

    return pMapEditorScene;
}

void CMapEditorScene::Free()
{
    Engine::CScene::Free();
}

void CMapEditorScene::Show_ImguiWindow()
{
    bool bOpen = true;
    ImGui::Begin("Map Editor", NULL, ImGuiWindowFlags_MenuBar);

    // Menu Bar
    if (ImGui::BeginMenuBar())
    {
        // Menus
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save"))
            {
                printf("Save Clicked\n");

            }
            ImGui::Separator();
            if (ImGui::MenuItem("Open"))
            {
                printf("Open Clicked\n");

            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    Setting_Menu();
    Setting_TileList();

    ImGui::End();
}

void CMapEditorScene::Setting_Menu()
{
    if (!ImGui::CollapsingHeader("Setting"))
    {
        return;
    }

    ImGui::Text("Coordinate");
    ImGui::SameLine(0.0f, 0.0f);

    static _vec2  vPosition = _vec2(300.0f, 100.0f);
    ImGui::SliderFloat2("##1", vPosition, -400.0f, 400.0f);

    ImGui::Text("Position  ");
    ImGui::SameLine(0.0f, 0.0f);
    static int pos[2] = { 0,0 };
    ImGui::SliderInt2("##", pos, 0, 10);
}

void CMapEditorScene::Setting_TileList()
{
    if (!ImGui::CollapsingHeader("Tile List"))
    {
        return;
    }

    const char* items[] = { "Tile01","Tile02","Tile03" };
    static int nCurrentItem = 1;
    ImGui::Combo("##2", &nCurrentItem, items, IM_ARRAYSIZE(items));
}
