#include "pch.h"
#include "../Header/MapEditorScene.h"
#include "Export_Utility.h"
#include "Export_System.h"
#include "../Header/MapToolCamera.h"

CMapEditorScene::CMapEditorScene(LPDIRECT3DDEVICE9 _pGraphicDevice)
    : Engine::CScene(_pGraphicDevice)
    , m_bGuiHovered(false)
    , m_pMTGameObjectCom(nullptr)
    , m_iPikingCount(0)
    , m_pTileCom(nullptr)
    , m_bPushed(false)
    , m_bSaved(false)

{
    ZeroMemory(&m_tImageInfo, sizeof(D3DXIMAGE_INFO));

    // 시작할 때, ImGui에 이미지 등록함.
    if (!m_TextureInfo)
        Resister_TileImage_ImGui(_pGraphicDevice, L"../Bin/Resource/Texture/Tile/Tile_%d.png", TEX_NORMAL, 6);
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

    if (Engine::Get_DIMouseState(DIM_LB) & 0x80)
    {
        m_bPushed = true;
    }
    if (!(Engine::Get_DIMouseState(DIM_LB) & 0x80) && m_bPushed)
    {
        m_iPikingCount++;

        Create_TileObject(L"Layer_GameLogic", m_iPikingCount, dynamic_cast<CMapToolTerrain*>(m_pMTGameObjectCom)->Get_PikingPos());
        m_bPushed = false;
    }

    // 나중에 Save기능 추가해서 변경 예정.
    //if (Engine::Get_DIKeyState(DIK_V) & 0x80)
    //{
    //    m_bSaved = true;
    //}
    //if (!(Engine::Get_DIKeyState(DIK_V) & 0x80) && m_bSaved)
    //{
    //    //Save_MapFile();
    //    m_bSaved = false;
    //}

    return iExit;
}

void CMapEditorScene::LateUpdate_Scene()
{
    Engine::CScene::LateUpdate_Scene();
}

void CMapEditorScene::Render_Scene()
{
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

    pGameObject = CMapToolCamera::Create(m_pGraphicDev,
        &eye,
        &at,
        &up);

    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MapToolCamera", pGameObject), E_FAIL);

    m_mapLayer.insert({ pLayerTag , pLayer });

    return S_OK;
}

HRESULT CMapEditorScene::Ready_Layer_GameLogic(const _tchar* pLayerTag)
{
    Engine::CLayer* pLayer = CLayer::Create();
    NULL_CHECK_RETURN(pLayer, E_FAIL);

    m_pMTGameObjectCom = CMapToolTerrain::Create(m_pGraphicDev);
    NULL_CHECK_RETURN(m_pMTGameObjectCom, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MapToolTerrain", m_pMTGameObjectCom), E_FAIL);

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

CMapEditorScene* CMapEditorScene::Create(LPDIRECT3DDEVICE9 _pGraphicDeivce)
{
    CMapEditorScene* pMapEditorScene = new CMapEditorScene(_pGraphicDeivce);

    if (FAILED(pMapEditorScene->Ready_Scene()))
    {
        Safe_Release(pMapEditorScene);
        MSG_BOX("Stage Create Failed");
        return nullptr;
    }

    return pMapEditorScene;
}

void CMapEditorScene::Show_ImguiWindow()
{
    ImGui::GetIO().NavActive = false;
    ImGui::GetIO().WantCaptureMouse = true;

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

    // ImGui창에 마우스 있나 없나 체크하는 부분. 더 수정해야함.
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemHovered())
        m_bGuiHovered = true;
    else
        m_bGuiHovered = false;

    ImGui::End();
}

void CMapEditorScene::Setting_Menu()
{
    if (!ImGui::CollapsingHeader("Setting"))
        return;

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
        return;

    CComponent* pComponent = NULL;

    // 여기서 Imgui에서 직접적으로 몇 개의 타일을 등록할 것인지,
    // 추가 공부해서 몇 개의 타일을 등록한 뒤에, 그 만큼 타일리스트 나와서
    // 각 타일 항목마다 맞는 이미지 출력하게 해야함.
    const char* items[] = { "Tile01","Tile02","Tile03" };

    static int nCurrentItem = 0;
    ImGui::Combo("##2", &nCurrentItem, items, IM_ARRAYSIZE(items));

    for (_int i = 0; i < m_vecTexture.size(); ++i)
    {
        if (nCurrentItem == i)
        {
            if (ImGui::ImageButton("Tile", m_vecTexture[i], ImVec2(50.0f, 50.0f)))
            {
                dynamic_cast<CMapToolTerrain*>(m_pMTGameObjectCom)->Set_TileNumber(i);
            }
        }
    }
}

HRESULT CMapEditorScene::Resister_TileImage_ImGui(LPDIRECT3DDEVICE9 _pGraphicDeivce, const _tchar* _ImageFilePath, TEXTUREID _eTextureId, const int& _iImageNumber)
{
    m_vecTexture.reserve(_iImageNumber);

    for (_int i = 0; i < _iImageNumber; ++i)
    {
        TCHAR       szImageFileName[128] = L"";

        wsprintf(szImageFileName, _ImageFilePath, i);

        switch (_eTextureId)
        {
        case TEX_NORMAL:
            FAILED_CHECK_RETURN(D3DXCreateTextureFromFile(m_pGraphicDev, szImageFileName, &m_TextureInfo), E_FAIL);
            break;

        case TEX_CUBE:
            FAILED_CHECK_RETURN(D3DXCreateCubeTextureFromFile(m_pGraphicDev, szImageFileName, (LPDIRECT3DCUBETEXTURE9*)&m_TextureInfo), E_FAIL);
            break;
        }
        m_vecTexture.emplace_back(m_TextureInfo);
    }
    return S_OK;
}

void CMapEditorScene::Set_Texture(const _uint& iIndex)
{
    if (m_vecTexture.size() < iIndex)
        return;

    m_pGraphicDev->SetTexture(0, m_vecTexture[iIndex]);
}

HRESULT CMapEditorScene::Create_TileObject(const _tchar* pLayerTag, _int _iCount, _vec3 _vTilePos)
{
    auto	iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(pLayerTag));

    if (iter == m_mapLayer.end())
        return E_FAIL;

    m_pTileCom = CTile::Create(m_pGraphicDev, _vTilePos);
    NULL_CHECK_RETURN(m_pTileCom, E_FAIL);

    m_wsTileNameString[_iCount] = L"Tile_" + std::to_wstring(_iCount);

    FAILED_CHECK_RETURN(iter->second->Add_GameObject(m_wsTileNameString[_iCount].c_str(), m_pTileCom), E_FAIL);

    m_mapLayer.insert({ pLayerTag, iter->second });

    return S_OK;
}

void CMapEditorScene::Free()
{
    Engine::CScene::Free();
}
