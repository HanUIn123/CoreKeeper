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
    , m_iWallCreateCount(0)
    , m_pTileCom(nullptr)
    , m_pWallCom(nullptr)
    , m_bPushed(false)
    , m_bWallClickPushed(false)
    , m_bSwitch(false)
    , m_bSelectTile(false)
    , m_bSelectWall(false)
    , m_bCanInstall(false)
    , m_iLoadTileCount(0)
    , m_iLoadWallCount(0)
    , m_iPikingIndex(0)
    , m_iImageNumber(0)
    , m_iWallImgNumber(0)
{
    ZeroMemory(&m_tImageInfo, sizeof(D3DXIMAGE_INFO));

    // 시작할 때, ImGui에 Tile 이미지 등록함.
    if (!m_TileTextureInfo)
    {
        Resister_TileImage_ImGui(_pGraphicDevice, L"../Bin/Resource/Texture/Tile/BasicTile/BasicTile_%d.png", TEX_NORMAL, 11);
        Resister_TileImage_ImGui(_pGraphicDevice, L"../Bin/Resource/Texture/Wall/Wall_%d.dds", TEX_CUBE, 3);
    }

    m_vecWallObject.resize(VTXCNTX * VTXCNTZ);
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

    Piking_Tile();

    Piking_Wall();

    return iExit;
}

void CMapEditorScene::LateUpdate_Scene()
{
    // MapTool 에서 사용할 타일 고르는 함수.

    ImGui::Begin("Object List", NULL, ImGuiWindowFlags_MenuBar);
    Setting_TileList();
    Setting_WallList();
    ImGui::End();


    ImGui::Begin("Switch Terrain", NULL, ImGuiWindowFlags_MenuBar);
    ImGui::Checkbox("Switcing Terrain", &m_bSwitch);
    if (ImGui::Button("Switch!"))
        m_bSwitch = true;
    if (ImGui::Button("Switch Off"))
        m_bSwitch = false;
    if (m_bSwitch)
        m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    else
        m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    ImGui::End();

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
            if (ImGui::MenuItem("Save File"))
            {
                MapFile_Save();
                MSG_BOX("Save Complete!");
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Load File"))
            {
                //printf("Open Clicked\n");
                MapFile_Load();
                MSG_BOX("Load Complete!");
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    Setting_Menu();

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

    const char* items[] = { "Tile01","Tile02","Tile03", "Tile04","Tile05","Tile06","Tile07","Tile08","Tile09","Tile10" };

    static int	nCurrentItem = 0;
    ImGui::Combo("##", &nCurrentItem, items, IM_ARRAYSIZE(items));

    for (_int i = 0; i < m_vecTileTexture.size(); ++i)
    {
        if (nCurrentItem == i)
        {
            if (ImGui::ImageButton("Tile", m_vecTileTexture[i], ImVec2(50.0f, 50.0f)))
            {
                if (m_pTileCom != nullptr)       //-> 이거 안하면 터짐.
                {
                    m_iImageNumber = nCurrentItem;
                    m_bSelectWall = false;
                    m_bSelectTile = true;
                }
            }
        }
    }
}

void CMapEditorScene::Piking_Tile()
{
    if (!m_bGuiHovered)
    {
        if (Engine::Get_DIMouseState(DIM_LB) & 0x80 && m_bSelectTile)
        {
            m_bPushed = true;

            CMapToolTerrain* pTerrain = dynamic_cast<CMapToolTerrain*>(Engine::Get_GameObject(L"Layer_GameLogic", L"MapToolTerrain"));
            CCalculator* pPickPos = dynamic_cast<CCalculator*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"MapToolTerrain", L"Com_Calculator"));
            CMapToolTex* pMapToolBufferCom = dynamic_cast<CMapToolTex*>(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"MapToolTerrain", L"Com_Buffer"));
            CTransform* pMapToolTransformCom = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"MapToolTerrain", L"Com_Transform"));

            m_vPickPos = pPickPos->Picking_OnTerrain(g_hWnd, pMapToolBufferCom, pMapToolTransformCom);

            m_vecTileObject[m_vPickPos.z * VTXCNTX + m_vPickPos.x]->Set_TileNumber(m_iImageNumber);
        }
        if (!(Engine::Get_DIMouseState(DIM_LB) & 0x80))
            m_bPushed = false;
    }
}

void CMapEditorScene::Setting_WallList()
{
    if (!ImGui::CollapsingHeader("Wall List"))
        return;

    CComponent* pComponent = NULL;

    const char* items[] = { "Wall01","Wall02","Wall03", "Wall04","Wall05" };

    static int	nCurrentItem = 0;
    ImGui::Combo("##3", &nCurrentItem, items, IM_ARRAYSIZE(items));

    for (_int i = 0; i < m_vecWallTexture.size(); ++i)
    {
        if (nCurrentItem == i)
        {
            if (ImGui::ImageButton("Wall", m_vecWallTexture[i], ImVec2(50.0f, 50.0f)))
            {
                m_bSelectTile = false;
                m_bSelectWall = true;

                //if (m_pWallCom != nullptr)       //-> 이거 안하면 터짐.
                m_iWallImgNumber = nCurrentItem;
            }
        }
    }
}

HRESULT CMapEditorScene::Piking_Wall()
{
    auto	iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(L"Layer_Environment"));

    if (iter == m_mapLayer.end())
        return E_FAIL;

    if (!m_bGuiHovered)
    {
        if (Engine::Get_DIMouseState(DIM_LB) & 0x80 && m_bSelectWall)
        {
            m_bWallClickPushed = true;

            CMapToolTerrain* pTerrain = dynamic_cast<CMapToolTerrain*>(Engine::Get_GameObject(L"Layer_GameLogic", L"MapToolTerrain"));
            CCalculator* pPickPos = dynamic_cast<CCalculator*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"MapToolTerrain", L"Com_Calculator"));
            CMapToolTex* pMapToolBufferCom = dynamic_cast<CMapToolTex*>(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"MapToolTerrain", L"Com_Buffer"));
            CTransform* pMapToolTransformCom = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"MapToolTerrain", L"Com_Transform"));

            m_vPickPos = pPickPos->Picking_OnTerrain(g_hWnd, pMapToolBufferCom, pMapToolTransformCom);

            // 이미 설치되어 있으면, 벽 옵젝 벡터 검사해서, 찍는 위치랑 같으면, 
            // 이미 설치되어있다는 불 값 true
            bool bAlreadyInstalled = false;

            if (m_vecWallObject[m_vPickPos.z * VTXCNTX + m_vPickPos.x])
                bAlreadyInstalled = true;

            // 이미 설치되어있따면, 설치할 수 있다는 값을 false로,
            // checkpos라는 픽하고 나서 딱 설치된 시점의(벽 전 위치값 느낌)
            // 둘이 같으면 설치 불가능 (설치된 전의 값하고 또 그자리에 피킹할 때 포지션값이 같으면 설치불가)
            // 그 외는 설치 가능.
            if (bAlreadyInstalled)
                m_bCanInstall = false;
            else if (m_vCheckPos == m_vPickPos)
                m_bCanInstall = false;
            else
                m_bCanInstall = true;

            if (m_bCanInstall)
            {
                int i = m_vPickPos.z * VTXCNTX + m_vPickPos.x;

                m_wsWallNameString[i] = L"Wall_" + std::to_wstring(i);

                m_pWallCom = CWall::Create(m_pGraphicDev, m_vPickPos.x, m_vPickPos.z, 0, m_wsWallNameString[i].c_str());
                m_vecWallObject[i] = dynamic_cast<CWall*>(m_pWallCom);

                NULL_CHECK_RETURN(m_pWallCom, E_FAIL);
                FAILED_CHECK_RETURN(iter->second->Add_GameObject(m_wsWallNameString[i].c_str(), m_pWallCom), E_FAIL);

                m_vecWallObject[i]->Set_WallNumber(m_iWallImgNumber);

                m_iWallCreateCount++;
                m_vCheckPos = m_vPickPos;
            }
        }
        if (!(Engine::Get_DIMouseState(DIM_LB) & 0x80))
            m_bWallClickPushed = false;

        if (Engine::Get_DIMouseState(DIM_RB) & 0x80)
        {
            CMapToolTerrain* pTerrain = dynamic_cast<CMapToolTerrain*>(Engine::Get_GameObject(L"Layer_GameLogic", L"MapToolTerrain"));
            CCalculator* pPickPos = dynamic_cast<CCalculator*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"MapToolTerrain", L"Com_Calculator"));
            CMapToolTex* pMapToolBufferCom = dynamic_cast<CMapToolTex*>(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"MapToolTerrain", L"Com_Buffer"));
            CTransform* pMapToolTransformCom = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"MapToolTerrain", L"Com_Transform"));

            m_vPickPos = pPickPos->Picking_OnTerrain(g_hWnd, pMapToolBufferCom, pMapToolTransformCom);

            if (m_vecWallObject[m_vPickPos.z * VTXCNTX + m_vPickPos.x])
            {
                Delete_Object(L"Layer_Environment", m_vecWallObject[m_vPickPos.z * VTXCNTX + m_vPickPos.x]->Get_PickedWallName().c_str());
                m_vecWallObject[m_vPickPos.z * VTXCNTX + m_vPickPos.x] = nullptr;
            }
        }
    }

    return S_OK;
}

void CMapEditorScene::Check_NextWall(CCalculator* _pPickPos, CMapToolTex* _pMapToolTex, CTransform* _pMapToolTrnasform, _int _iIndex)
{

}

HRESULT CMapEditorScene::Delete_Object(const _tchar* pLayerTag, const _tchar* pGameObjectTag)
{
    auto	iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(pLayerTag));

    if (iter == m_mapLayer.end())
        return E_FAIL;

    iter->second->Delete_GameMap(pGameObjectTag);

    return S_OK;
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
            m_vecTileTexture.emplace_back(m_TileTextureInfo);
            break;

        case TEX_CUBE:
            FAILED_CHECK_RETURN(D3DXCreateCubeTextureFromFile(m_pGraphicDev, szImageFileName, (LPDIRECT3DCUBETEXTURE9*)&m_TileTextureInfo), E_FAIL);
            m_vecWallTexture.emplace_back(m_TileTextureInfo);
            break;
        }
    }
    return S_OK;
}

void CMapEditorScene::MapFile_Save()
{
    const _tchar* strFileName = L"../../Data/TileData.txt";
    const _tchar* strWallFileName = L"../../Data/WallData.txt";

    m_hFile = CreateFile(strFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    m_hWallFile = CreateFile(strWallFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == m_hFile)
    {
        MSG_BOX("Failed Create File");
        return;
    }

    if (INVALID_HANDLE_VALUE == m_hWallFile)
    {
        MSG_BOX("Failed Wall Create File");
        return;
    }

    if (m_vecTileObject.empty())
        return;

    _vec3  vTempTilePos(0.0f, 0.0f, 0.0f);
    _int   vTempTileImgNum(0);
   

    DWORD	dwByte(0);

    for (auto& iter : m_vecTileObject)
    {
        vTempTilePos = (*iter).Get_TilePos();
        vTempTileImgNum = (*iter).Get_TileNumber();

        // 타일 저장.
        WriteFile(m_hFile, &vTempTilePos, sizeof(_vec3), &dwByte, nullptr);
        WriteFile(m_hFile, &vTempTileImgNum, sizeof(_int), &dwByte, nullptr);
    }

    _vec3 vTempWallPos(0.0f, 0.0f, 0.0f);
    _int  vTempWallImgNum(0);
    _int  vTempIndex(0);

    DWORD	dwByte2(0);

    for (auto& iter : m_vecWallObject)
    {
        if (iter == nullptr)
        {
            vTempIndex++;
            continue;
        }

        vTempWallPos = (*iter).Get_WallPos();
        vTempWallImgNum = (*iter).Get_WallNumber();

        // 벽 저장.
        WriteFile(m_hWallFile, &vTempWallPos, sizeof(_vec3), &dwByte2, nullptr);
        WriteFile(m_hWallFile, &vTempWallImgNum, sizeof(_int), &dwByte2, nullptr);
        WriteFile(m_hWallFile, &vTempIndex, sizeof(_int), &dwByte2, nullptr);

        vTempIndex++;
    }

    CloseHandle(m_hFile);
    CloseHandle(m_hWallFile);
}

HRESULT CMapEditorScene::MapFile_Load()
{
    for (auto& iter : m_vecWallObject)
    {
        if (iter == nullptr)
            continue;

        // 로드 전에 wall 모두 지우기.
        Delete_Object(L"Layer_Environment", (*iter).Get_PickedWallName().c_str());
    }

    auto	iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(L"Layer_Environment"));

    if (iter == m_mapLayer.end())
        return E_FAIL;


    Engine::CGameObject* pGameObject = nullptr;

    const _tchar* strWallFileName = L"../../Data/WallData.txt";

    m_hWallFile = CreateFile(strWallFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);


    if (INVALID_HANDLE_VALUE == m_hWallFile)
    {
        MSG_BOX("Fail Open Wall file");
        return E_FAIL;
    }

    _vec3 vTempWallPos(0.0f, 0.0f, 0.0f);
    _int vTempWallImgNum(0);
    _int vTempIndex(0);

    DWORD dwByte2 = 0;

    while (true)
    {
        ReadFile(m_hWallFile, &vTempWallPos, sizeof(_vec3), &dwByte2, nullptr);
        ReadFile(m_hWallFile, &vTempWallImgNum, sizeof(_int), &dwByte2, nullptr);
        ReadFile(m_hWallFile, &vTempIndex, sizeof(_int), &dwByte2, nullptr);

        if (dwByte2 == 0)
            break;

        m_wsWallNameString[vTempIndex] = L"Wall_" + std::to_wstring(vTempIndex);
        CWall* pWall = CWall::Create(m_pGraphicDev, vTempWallPos.x, vTempWallPos.z, vTempWallImgNum, m_wsWallNameString[vTempIndex].c_str());
        m_vecWallObject[vTempIndex] = pWall;

        NULL_CHECK_RETURN(pWall, E_FAIL);
        FAILED_CHECK_RETURN(iter->second->Add_GameObject(m_wsWallNameString[vTempIndex].c_str(), pWall), E_FAIL);
    }

    CloseHandle(m_hWallFile);
    MSG_BOX("Success Load File");

    return S_OK;
}

void CMapEditorScene::Free()
{
    Engine::CScene::Free();
}
