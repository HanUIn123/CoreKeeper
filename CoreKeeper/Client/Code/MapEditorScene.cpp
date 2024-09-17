#include "pch.h"
#include "../Header/MapEditorScene.h"
#include "Export_Utility.h"
#include "Export_System.h"
#include "../Header/MapToolCamera.h"


CMapEditorScene::CMapEditorScene(LPDIRECT3DDEVICE9 _pGraphicDevice)
    : Engine::CScene(_pGraphicDevice)
    , m_bGuiHovered(false)
    , m_pMTGameObjectCom(nullptr)
    , m_iTileCreateCount(0)
    , m_pTileCom(nullptr)
    , m_bPushed(false)
{
    ZeroMemory(&m_tImageInfo, sizeof(D3DXIMAGE_INFO));

    // 시작할 때, ImGui에 Tile 이미지 등록함.
    if (!m_TileTextureInfo)
    {
        Resister_TileImage_ImGui(_pGraphicDevice, L"../Bin/Resource/Texture/Tile/BasicTile/BasicTile_%d.png", TEX_NORMAL, 11);
        //Resister_TileImage_ImGui(_pGraphicDevice, L"../Bin/Resource/Texture/MapTerrain/Terrain_%d.png", TEX_NORMAL, 8);
    }
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

    // ImGui창에 마우스 있나 없나 체크하는 부분. 더 수정해야함.
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemHovered())
        m_bGuiHovered = true;
    else
        m_bGuiHovered = false;

    if (!m_bGuiHovered)
    {
        // 피킹으로 Terrain에 Tile 피킹되게 처리.
        if (Engine::Get_DIMouseState(DIM_LB) & 0x80)
        {
            m_bPushed = true;
        }
        if (!(Engine::Get_DIMouseState(DIM_LB) & 0x80) && m_bPushed)
        {
            CMapToolTerrain* pTerrain = dynamic_cast<CMapToolTerrain*>(Engine::Get_GameObject(L"Layer_GameLogic", L"MapToolTerrain"));
            CCalculator* pPickPos = dynamic_cast<CCalculator*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"MapToolTerrain", L"Com_Calculator"));
            CMapToolTex* pMapToolBufferCom = dynamic_cast<CMapToolTex*>(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"MapToolTerrain", L"Com_Buffer"));
            CTransform* pMapToolTransformCom = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"MapToolTerrain", L"Com_Transform"));

            m_vPickPos = pPickPos->Picking_OnTerrain(g_hWnd, pMapToolBufferCom, pMapToolTransformCom);

            m_vecTileObject[m_vPickPos.z * VTXCNTX + m_vPickPos.x]->Set_TileNumber(m_iImageNumber);

            m_bPushed = false;

           // m_listChangedTile.push_back(m_vecTileObject[m_vPickPos.z * VTXCNTX + m_vPickPos.x]);
        }
    }

    if (Engine::Get_DIKeyState(DIK_V))
    {
        MapFile_Save();
    }

    return iExit;
}

void CMapEditorScene::LateUpdate_Scene()
{
    Setting_TileList();

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

    
    for (_ulong i = 0; i < VTXCNTX; ++i)
    {
        for (_ulong j = 0; j < VTXCNTZ; ++j)
        {
            _float fX = (VTXITV >> 1) + _float(VTXITV * j);
            _float fZ = (VTXITV >> 1) + _float(VTXITV * i);

            m_pTileCom = CTile::Create(m_pGraphicDev, fX, fZ, 0);

            m_vecTileObject.push_back(dynamic_cast<CTile*>(m_pTileCom));

            NULL_CHECK_RETURN(m_pTileCom, E_FAIL);

            m_wsTileNameString[m_iTileCreateCount] = L"Tile_" + std::to_wstring(m_iTileCreateCount);
            FAILED_CHECK_RETURN(pLayer->Add_GameObject(m_wsTileNameString[m_iTileCreateCount].c_str(), m_pTileCom), E_FAIL);
            m_iTileCreateCount++;
        }
    }
    m_iTileCreateCount = 0;
    

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

    // MapTool 에서 사용할 지형terrain을 고르는 함수.
    //Setting_TerrainList();

    //// MapTool 에서 사용할 타일 고르는 함수.
    //Setting_TileList();

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
    //bool ImGui::Combo(const char* label, int* current_item, const char* (*getter)(void* user_data, int idx), void* user_data, int items_count, int popup_max_height_in_items)

    //for (int i = 0; i < 9; ++i)
    //{
    //    wstring items[] = { L"Tile0" + std::to_wstring(i) };
    //    static int nCurrentItem = 0;
    //    ImGui::Combo("##3", &nCurrentItem, items, IM_ARRAYSIZE(items));
    //}
    //ImGui::Combo("##3", &nCurrentItem, items, IM_ARRAYSIZE(items))


    const char* items[] = { "Tile01","Tile02","Tile03", "Tile04","Tile05","Tile06","Tile07","Tile08","Tile09","Tile10" };

    static int	nCurrentItem = 0;
    ImGui::Combo("##3", &nCurrentItem, items, IM_ARRAYSIZE(items));

    for (_int i = 0; i < m_vecTileTexture.size(); ++i)
    {
        if (nCurrentItem == i)
        {
            if (ImGui::ImageButton("Tile", m_vecTileTexture[i], ImVec2(50.0f, 50.0f)))
            {
                if (m_pTileCom != nullptr)       //-> 이거 안하면 터짐.
                {
                    m_iImageNumber = nCurrentItem;

                }
            }
        }
    }
}

HRESULT CMapEditorScene::Resister_TileImage_ImGui(LPDIRECT3DDEVICE9 _pGraphicDeivce, const _tchar* _ImageFilePath, TEXTUREID _eTextureId, const int& _iImageNumber)
{
    for (_int i = 0; i < _iImageNumber; ++i)
    {
        TCHAR       szImageFileName[128] = L"";

        wsprintf(szImageFileName, _ImageFilePath, i);

        switch (_eTextureId)
        {
        case TEX_NORMAL:
            FAILED_CHECK_RETURN(D3DXCreateTextureFromFile(m_pGraphicDev, szImageFileName, &m_TileTextureInfo), E_FAIL);
            break;

        case TEX_CUBE:
            FAILED_CHECK_RETURN(D3DXCreateCubeTextureFromFile(m_pGraphicDev, szImageFileName, (LPDIRECT3DCUBETEXTURE9*)&m_TileTextureInfo), E_FAIL);
            break;
        }
        m_vecTileTexture.emplace_back(m_TileTextureInfo);
    }
    return S_OK;
}

void CMapEditorScene::MapFile_Save()
{
    const _tchar* strFileName = L"../../Data/MapData.txt";

    m_hFile = CreateFile(strFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == m_hFile)
    {
        MSG_BOX("Failed Create File");
        return;
    }

    if (m_vecTileObject.empty())
        return;

    _vec3   vTempTilePos(0.0f,0.0f,0.0f);
    _int    vTempTileImgNum(0);

    _vec3   vTempChangeTilePos(0.0f,0.0f,0.0f);
    _int   vTempChangeTileNum(0);

    list<CTile*> vTempChangedTile;
    

    DWORD	dwByte(0);

    for (auto& iter : m_vecTileObject)
    {
        vTempTilePos = (*iter).Get_TilePos();
        vTempTileImgNum = (*iter).Get_TileNumber();

        WriteFile(m_hFile, &vTempTilePos, sizeof(_vec3), &dwByte, nullptr);
        WriteFile(m_hFile, &vTempTileImgNum, sizeof(_int), &dwByte, nullptr);
    }

    CloseHandle(m_hFile);
}

void CMapEditorScene::MapFile_Load()
{
    const _tchar* strFileName = L"../../Data/MapData.txt";

    m_hFile = CreateFile(strFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == m_hFile)
    {
        MSG_BOX("Failed Open file");
        return;
    }

    _vec3   vTempTilePos(0.0f, 0.0f, 0.0f);
    _int    vTempTileImgNum(0);

    DWORD   dwByte = 0;

    while (true)
    {
        ReadFile(m_hFile, &vTempTilePos, sizeof(_vec3), &dwByte, NULL);
        ReadFile(m_hFile, &vTempTileImgNum, sizeof(_int), &dwByte, NULL);

        if (0 == dwByte)
            break;

        CTile* pTile = CTile::Create(m_pGraphicDev, vTempTilePos.x, vTempTilePos.z, vTempTileImgNum);

        dynamic_cast<CTile*>(pTile)->Set_TilePos(vTempTilePos);
        dynamic_cast<CTile*>(pTile)->Set_TileNumber(vTempTileImgNum);

        m_vecTileObject.push_back(pTile);
    }

    CloseHandle(m_hFile);
    MSG_BOX("Succed Load Tile");
}

void CMapEditorScene::Free()
{
    Engine::CScene::Free();
}
