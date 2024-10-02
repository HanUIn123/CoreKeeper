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
    , m_iBuildCreateCount(0)
    , m_pWallCom(nullptr)
    , m_pObjectCom(nullptr)
    , m_bPushed(false)
    , m_bWallClickPushed(false)
    , m_bBuildingClick(false)
    , m_bSwitch(false)
    , m_bReachable(false)
    , m_bSelectTile(false)
    , m_bSelectWall(false)
    , m_bSelectBuilding(false)
    , m_bSelectMonster(false)
    , m_bCanInstall(false)
    , m_bAlreadyInstalled(false)
    , m_bReposed(false)
    , m_iLoadTileCount(0)
    , m_iLoadWallCount(0)
    , m_iPikingIndex(0)
    , m_iTileNumber(0)
    , m_iWallImgNumber(0)
    , m_iBuildingNumber(0)
    , m_iMonsterNumber(0)
    , m_iStandardIndex(0)
    , m_iUpIndex(0)
    , m_iDownIndex(0)
    , m_iRightIndex(0)
    , m_iLeftIndex(0)
{
    ZeroMemory(&m_tImageInfo, sizeof(D3DXIMAGE_INFO));

    // 시작할 때, ImGui에 Tile 이미지 등록함.
    if (!m_TextureInfo)
    {
        Resister_ImguiImage_ImGui(_pGraphicDevice, L"../Bin/Resource/Texture/Tile/DustTile/Dust_Tile_%d.png", TEX_TILE, 9);
        Resister_ImguiImage_ImGui(_pGraphicDevice, L"../Bin/Resource/Texture/Tile/RockTile/Rock_Tile_%d.png", TEX_TILE, 9);
        Resister_ImguiImage_ImGui(_pGraphicDevice, L"../Bin/Resource/Texture/Tile/GrassTile/Grass_Tile_%d.png", TEX_TILE, 9);

        Resister_ImguiImage_ImGui(_pGraphicDevice, L"../Bin/Resource/Texture/ImGuiImage/Wall/Dust_Wall.png", TEX_WALL, 1);
        Resister_ImguiImage_ImGui(_pGraphicDevice, L"../Bin/Resource/Texture/ImGuiImage/Wall/Rock_Wall.png", TEX_WALL, 1);
        Resister_ImguiImage_ImGui(_pGraphicDevice, L"../Bin/Resource/Texture/ImGuiImage/Wall/Grass_Wall.png", TEX_WALL, 1);

        Resister_ImguiImage_ImGui(_pGraphicDevice, L"../Bin/Resource/Texture/BaseCamp/Core.png", TEX_OBJECT, 1);
        Resister_ImguiImage_ImGui(_pGraphicDevice, L"../Bin/Resource/Texture/BaseCamp/CoreBase.png", TEX_OBJECT, 1);
        Resister_ImguiImage_ImGui(_pGraphicDevice, L"../Bin/Resource/Texture/BaseCamp/SpawnPoint.png", TEX_OBJECT, 1);

        Resister_ImguiImage_ImGui(_pGraphicDevice, L"../Bin/Resource/Texture/ImGuiImage/Monster/Slime.png", TEX_MONSTER, 1);
        Resister_ImguiImage_ImGui(_pGraphicDevice, L"../Bin/Resource/Texture/ImGuiImage/Monster/Mushroom.png", TEX_MONSTER, 1);
        Resister_ImguiImage_ImGui(_pGraphicDevice, L"../Bin/Resource/Texture/ImGuiImage/Monster/Shaman.png", TEX_MONSTER, 1);
    }

    m_vecWallObject.resize((VTXCNTX - 1) * (VTXCNTZ - 1));
    //m_vecBuildingObject.resize((VTXCNTX - 1) * (VTXCNTZ - 1));
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

    Piking_Object();

    return iExit;
}

void CMapEditorScene::LateUpdate_Scene()
{
    // MapTool 에서 사용할 타일 고르는 함수.

    ImGui::Begin("Object List", NULL, ImGuiWindowFlags_MenuBar);
    Setting_TileList();
    Setting_WallList();
    Setting_ObjectList();
    Setting_MonsterList();
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

    ImGui::Begin("Piking Reachable", NULL, ImGuiWindowFlags_MenuBar);
    ImGui::Checkbox("Setting Reachable", &m_bReachable);
    if (ImGui::Button("Switch!"))
        m_bReachable = true;
    if (ImGui::Button("Switch Off"))
        m_bReachable = false;
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

    _vec3 eye(128.5f, 20.f, 128.5f);
    _vec3 at(128.5f, 0.f, 128.5f);
    _vec3 up(0.f, 1.f, 0.f);

    pGameObject = CMapToolCamera::Create(m_pGraphicDev,
        &eye,
        &at,
        &up);

    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MapToolCamera", pGameObject), E_FAIL);

    pGameObject = CCore::Create(m_pGraphicDev, { 128.5f, 3.0f, 132.f + 3.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Core", pGameObject), E_FAIL);

    pGameObject = CStatue::Create(m_pGraphicDev, { 128.5f, 2.0f, 144.f + 2.f }, 0);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SlimeStatue", pGameObject), E_FAIL);

    pGameObject = CStatue::Create(m_pGraphicDev, { 112.5f, 2.0f, 137.f + 2.f }, 1);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"LarvaStatue", pGameObject), E_FAIL);

    pGameObject = CStatue::Create(m_pGraphicDev, { 144.5f, 2.0f, 137.f + 2.5f }, 2);
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MalugazStatue", pGameObject), E_FAIL);

    pGameObject = CCoreBase::Create(m_pGraphicDev, { 128.5f, 0.1f, 137.f });
    NULL_CHECK_RETURN(pGameObject, E_FAIL);
    FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"CoreBase", pGameObject), E_FAIL);

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

    const char* items[] =
    {
       "Dust_Tile", "Rock_Tile", "Green_Tile"
    };

    static int	nCurrentItem = 0;
    ImGui::Combo("##3", &nCurrentItem, items, IM_ARRAYSIZE(items));

    int iCount(0);

    for (_int i = 0; i < 9; ++i)
    {
        int textureIndex = nCurrentItem * 9 + i;

        if (textureIndex < m_vecTileTexture.size())
        {
            // 여기서도 중복 방지.
            if (ImGui::ImageButton(("Tile" + std::to_string(textureIndex)).c_str(), m_vecTileTexture[textureIndex], ImVec2(50.0f, 50.0f)))
            {
                m_bSelectTile = true;
                m_bSelectWall = false;
                m_bSelectBuilding = false;

                m_iTileNumber = textureIndex;
            }

            // Imgui 줄 3개 같은 가로줄 
            if ((i + 1) % 3 != 0)
            {
                ImGui::SameLine();
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

            // 터레인 아닌 곳 피킹
            if (m_vPickPos.y < 0)
                return;

            int iIndex = (m_vPickPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + m_vPickPos.x + 0.5f * VTXITV;

            pTerrain->Set_TextureNumber(iIndex, m_iTileNumber);

            if (m_bReachable)
                pTerrain->Set_Unreachable(iIndex, true);
            else
                pTerrain->Set_Unreachable(iIndex, false);

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

    const char* items[] =
    {
        "Wall_List"
    };

    static int	nCurrentItem = 0;
    ImGui::Combo("##3", &nCurrentItem, items, IM_ARRAYSIZE(items));

    int iCount(0);

    for (_int i = 0; i < 15; ++i)
    {
        int textureIndex = nCurrentItem * 15 + i;

        if (textureIndex < m_vecWallTexture.size())
        {
            if (ImGui::ImageButton(("Wall" + std::to_string(textureIndex)).c_str(), m_vecWallTexture[textureIndex], ImVec2(50.0f, 50.0f)))
            {
                m_bSelectTile = false;
                m_bSelectWall = true;
                m_bSelectBuilding = false;

                m_iWallImgNumber = textureIndex;
            }

            // Imgui 줄 3개 같은 가로줄 
            if ((i + 1) % 3 != 0)
            {
                ImGui::SameLine();
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

            // 터레인 아닌 곳 피킹
            if (m_vPickPos.y < 0)
                return S_OK;

            // 이미 설치되어 있으면, 벽 옵젝 벡터 검사해서, 찍는 위치랑 같으면, 
            // 이미 설치되어있다는 불 값 true
            m_bAlreadyInstalled = false;

            if (m_vecWallObject[unsigned __int64(m_vPickPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + m_vPickPos.x + 0.5f * VTXITV])
                m_bAlreadyInstalled = true;

            // 이미 설치되어있따면, 설치할 수 있다는 값을 false로,
            // checkpos라는 픽하고 나서 딱 설치된 시점의(벽 전 위치값 느낌)
            // 둘이 같으면 설치 불가능 (설치된 전의 값하고 또 그자리에 피킹할 때 포지션값이 같으면 설치불가)
            // 그 외는 설치 가능.
            if (m_bAlreadyInstalled)
                m_bCanInstall = false;
            else if (m_vCheckPos == m_vPickPos)
                m_bCanInstall = false;
            else
                m_bCanInstall = true;

            if (m_bCanInstall)
            {
                _int iIndex = _int(m_vPickPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + m_vPickPos.x + 0.5f * VTXITV;

                m_wsWallNameString[iIndex] = L"Wall_" + std::to_wstring(iIndex);
                m_pWallCom = CWall::Create(m_pGraphicDev, m_vPickPos.x + 0.5f * VTXITV, m_vPickPos.z + 0.5f * VTXITV, 0, m_wsWallNameString[iIndex].c_str());
                m_vecWallObject[iIndex] = dynamic_cast<CWall*>(m_pWallCom);

                NULL_CHECK_RETURN(m_pWallCom, E_FAIL);
                FAILED_CHECK_RETURN(iter->second->Add_GameObject(m_wsWallNameString[iIndex].c_str(), m_pWallCom), E_FAIL);

                m_vecWallObject[iIndex]->Set_WallNumber(m_iWallImgNumber * 15);
                //m_vecWallObject[iIndex]->Set_WallNumber(m_iWallImgNumber);
                pTerrain->Set_Unreachable(iIndex, true);

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

            _int iIndex = _int(m_vPickPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + m_vPickPos.x + 0.5f * VTXITV;

            if (m_vecWallObject[iIndex])
            {
                Delete_Object(L"Layer_Environment", m_vecWallObject[iIndex]->Get_PickedWallName().c_str());
                m_vecWallObject[iIndex] = nullptr;
                pTerrain->Set_Unreachable(iIndex, false);

            }
        }
    }

    return S_OK;
}

void CMapEditorScene::Setting_ObjectList()
{
    if (!ImGui::CollapsingHeader("Object List"))
        return;

    CComponent* pComponent = NULL;

    const char* items[] = { "Core","CoreBase", "SpawnPoint", "Box","Object2", "Object3" };

    static int	nCurrentItem = 0;
    ImGui::Combo("##4", &nCurrentItem, items, IM_ARRAYSIZE(items));

    for (_int i = 0; i < m_vecObjectTexture.size(); ++i)
    {
        if (nCurrentItem == i)
        {
            if (ImGui::ImageButton("Object", m_vecObjectTexture[i], ImVec2(50.0f, 50.0f)))
            {
                m_bSelectTile = false;
                m_bSelectWall = false;
                m_bSelectBuilding = true;

                //if (m_pWallCom != nullptr)       //-> 이거 안하면 터짐.
                m_iBuildingNumber = nCurrentItem;
            }
        }
    }
}

HRESULT CMapEditorScene::Piking_Object()
{
    //auto	iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(L"Layer_Environment"));

    //if (iter == m_mapLayer.end())
    //    return E_FAIL;

    //if (!m_bGuiHovered)
    //{
    //    if (Engine::Get_DIMouseState(DIM_LB) & 0x80)
    //    {
    //        m_bBuildingClick = true;
    //    }
    //    if (!(Engine::Get_DIMouseState(DIM_LB) & 0x80) && m_bSelectBuilding && m_bBuildingClick)
    //    {
    //        m_bBuildingClick = false;

    //        CMapToolTerrain* pTerrain = dynamic_cast<CMapToolTerrain*>(Engine::Get_GameObject(L"Layer_GameLogic", L"MapToolTerrain"));
    //        CCalculator* pPickPos = dynamic_cast<CCalculator*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"MapToolTerrain", L"Com_Calculator"));
    //        CMapToolTex* pMapToolBufferCom = dynamic_cast<CMapToolTex*>(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"MapToolTerrain", L"Com_Buffer"));
    //        CTransform* pMapToolTransformCom = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"MapToolTerrain", L"Com_Transform"));

    //        m_vObPickPos = pPickPos->Picking_OnTerrain(g_hWnd, pMapToolBufferCom, pMapToolTransformCom);

    //        if (m_vObPickPos.y < 0)
    //            return S_OK;

    //        m_bAlreadyInstalled = false;

    //        if (m_vecBuildingObject[unsigned __int64(m_vObPickPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + m_vObPickPos.x + 0.5f * VTXITV])
    //            m_bAlreadyInstalled = true;

    //        if (m_bAlreadyInstalled)
    //            m_bCanInstall = false;
    //        else if (m_vCheckPos == m_vObPickPos)
    //            m_bCanInstall = false;
    //        else
    //            m_bCanInstall = true;

    //        if (m_bCanInstall)
    //        {
    //            _int iIndex = _int(m_vObPickPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + m_vObPickPos.x + 0.5f * VTXITV;

    //            switch (m_iBuildingNumber)
    //            {
    //            case 0:
    //                //m_pObjectCom = nullptr;
    //                m_wsObjectNameString[iIndex] = L"Object_" + std::to_wstring(iIndex);
    //                m_pObjectCom = CCore::Create(m_pGraphicDev, m_vObPickPos.x, m_vObPickPos.z, m_bReposed, m_iBuildingNumber, m_wsObjectNameString[iIndex].c_str());
    //                m_vecBuildingObject[iIndex] = dynamic_cast<CCore*>(m_pObjectCom);

    //                NULL_CHECK_RETURN(m_pObjectCom, E_FAIL);
    //                FAILED_CHECK_RETURN(iter->second->Add_GameObject(m_wsObjectNameString[iIndex].c_str(), m_pObjectCom), E_FAIL);

    //                dynamic_cast<CCore*>(m_vecBuildingObject[iIndex])->Set_BuildImgNum(0);
    //                m_vCheckPos = m_vObPickPos;
    //                break;
    //            case 1:
    //                //m_pObjectCom = nullptr;
    //                m_wsObjectNameString[iIndex] = L"Object_" + std::to_wstring(iIndex);
    //                m_pObjectCom = CCoreBase::Create(m_pGraphicDev, m_vObPickPos.x, m_vObPickPos.z, m_bReposed, m_iBuildingNumber, m_wsObjectNameString[iIndex].c_str());
    //                m_vecBuildingObject[iIndex] = dynamic_cast<CCoreBase*>(m_pObjectCom);

    //                NULL_CHECK_RETURN(m_pObjectCom, E_FAIL);
    //                FAILED_CHECK_RETURN(iter->second->Add_GameObject(m_wsObjectNameString[iIndex].c_str(), m_pObjectCom), E_FAIL);

    //                dynamic_cast<CCoreBase*>(m_vecBuildingObject[iIndex])->Set_BuildImgNum(0);
    //                m_vCheckPos = m_vObPickPos;
    //                break;
    //            case 2:
    //                //m_pObjectCom = nullptr;
    //                m_wsObjectNameString[iIndex] = L"Object_" + std::to_wstring(iIndex);
    //                m_pObjectCom = CSpawnPoint::Create(m_pGraphicDev, m_vObPickPos.x, m_vObPickPos.z, m_bReposed, m_iBuildingNumber, m_wsObjectNameString[iIndex].c_str());
    //                m_vecBuildingObject[iIndex] = dynamic_cast<CSpawnPoint*>(m_pObjectCom);

    //                NULL_CHECK_RETURN(m_pObjectCom, E_FAIL);
    //                FAILED_CHECK_RETURN(iter->second->Add_GameObject(m_wsObjectNameString[iIndex].c_str(), m_pObjectCom), E_FAIL);

    //                dynamic_cast<CSpawnPoint*>(m_vecBuildingObject[iIndex])->Set_BuildImgNum(0);
    //                m_vCheckPos = m_vObPickPos;
    //                break;
    //            }
    //        }
    //    }

    //    if (Engine::Get_DIMouseState(DIM_RB) & 0x80)
    //    {
    //        CMapToolTerrain* pTerrain = dynamic_cast<CMapToolTerrain*>(Engine::Get_GameObject(L"Layer_GameLogic", L"MapToolTerrain"));
    //        CCalculator* pPickPos = dynamic_cast<CCalculator*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"MapToolTerrain", L"Com_Calculator"));
    //        CMapToolTex* pMapToolBufferCom = dynamic_cast<CMapToolTex*>(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"MapToolTerrain", L"Com_Buffer"));
    //        CTransform* pMapToolTransformCom = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"MapToolTerrain", L"Com_Transform"));

    //        m_vObPickPos = pPickPos->Picking_OnTerrain(g_hWnd, pMapToolBufferCom, pMapToolTransformCom);

    //        _float fXMin = m_vObPickPos.x - 5.0f;
    //        _float fXMax = m_vObPickPos.x + 5.0f;
    //        _float fZMin = m_vObPickPos.z - 5.0f;
    //        _float fZMax = m_vObPickPos.z + 5.0f;

    //        if (m_vecBuildingObject[unsigned __int64((m_vObPickPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + m_vObPickPos.x + 0.5f * VTXITV)])
    //        {
    //            if ((fXMin < m_vObPickPos.x && fXMax > m_vObPickPos.x) || (fZMin < m_vObPickPos.z && fZMax > m_vObPickPos.z))
    //            {
    //                //Delete_Object(L"Layer_Environment", dynamic_cast<CCore*>(m_vecBuildingObject[unsigned __int64(m_vObPickPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + m_vObPickPos.x + 0.5f *  VTXITV])->Get_PickedBuildingName().c_str());
    //                Delete_Object(L"Layer_Environment", (m_vecBuildingObject[unsigned __int64(m_vObPickPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + m_vObPickPos.x + 0.5f * VTXITV])->Get_PickedBuildingName().c_str());
    //                m_vecBuildingObject[unsigned __int64((m_vObPickPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + m_vObPickPos.x + 0.5f * VTXITV)] = nullptr;
    //            }
    //        }
    //    }
    //}

    return S_OK;
}

void CMapEditorScene::Setting_MonsterList()
{
    if (!ImGui::CollapsingHeader("Monster List"))
        return;

    CComponent* pComponent = NULL;

    const char* items[] =
    {
        "Monster List"
    };

    static int	nCurrentItem = 0;
    ImGui::Combo("##5", &nCurrentItem, items, IM_ARRAYSIZE(items));

    int iCount(0);

    for (_int i = 0; i < 3; ++i)
    {
        int textureIndex = nCurrentItem * 3 + i;

        if (textureIndex < m_vecMonsterTexture.size())
        {
            // 여기서도 중복 방지.
            if (ImGui::ImageButton(("Monster" + std::to_string(textureIndex)).c_str(), m_vecMonsterTexture[textureIndex], ImVec2(50.0f, 50.0f)))
            {
                m_bSelectTile = false;
                m_bSelectWall = false;
                m_bSelectBuilding = false;
                m_bSelectMonster = true;

                m_iMonsterNumber = textureIndex;
            }

            // Imgui 줄 3개 같은 가로줄 
            if ((i + 1) % 3 != 0)
            {
                ImGui::SameLine();
            }
        }
    }
}

HRESULT CMapEditorScene::Piking_Monster()
{
    return E_NOTIMPL;
}



HRESULT CMapEditorScene::Delete_Object(const _tchar* pLayerTag, const _tchar* pGameObjectTag)
{
    auto	iter = find_if(m_mapLayer.begin(), m_mapLayer.end(), CTag_Finder(pLayerTag));

    if (iter == m_mapLayer.end())
        return E_FAIL;

    iter->second->Delete_GameMap(pGameObjectTag);

    return S_OK;
}

HRESULT CMapEditorScene::Resister_ImguiImage_ImGui(LPDIRECT3DDEVICE9 _pGraphicDeivce, const _tchar* _ImageFilePath, IMGUITEXTUREID _eTextureId, const int& _iImageNumber)
{
    for (_int i = 0; i < _iImageNumber; ++i)
    {
        TCHAR       szImageFileName[128] = L"";

        wsprintf(szImageFileName, _ImageFilePath, i);

        switch (_eTextureId)
        {
        case TEX_TILE:
            FAILED_CHECK_RETURN(D3DXCreateTextureFromFile(m_pGraphicDev, szImageFileName, &m_TextureInfo), E_FAIL);
            m_vecTileTexture.emplace_back(m_TextureInfo);
            break;

        case TEX_WALL:
            // FAILED_CHECK_RETURN(D3DXCreateCubeTextureFromFile(m_pGraphicDev, szImageFileName, (LPDIRECT3DCUBETEXTURE9*)&m_TextureInfo), E_FAIL);
            FAILED_CHECK_RETURN(D3DXCreateTextureFromFile(m_pGraphicDev, szImageFileName, &m_TextureInfo), E_FAIL);
            m_vecWallTexture.emplace_back(m_TextureInfo);
            break;

        case TEX_OBJECT:
            //FAILED_CHECK_RETURN(D3DXCreateCubeTextureFromFile(m_pGraphicDev, szImageFileName, (LPDIRECT3DCUBETEXTURE9*)&m_TextureInfo), E_FAIL);
            FAILED_CHECK_RETURN(D3DXCreateTextureFromFile(m_pGraphicDev, szImageFileName, &m_TextureInfo), E_FAIL);
            m_vecObjectTexture.emplace_back(m_TextureInfo);
            break;

        case TEX_MONSTER:
            //FAILED_CHECK_RETURN(D3DXCreateCubeTextureFromFile(m_pGraphicDev, szImageFileName, (LPDIRECT3DCUBETEXTURE9*)&m_TextureInfo), E_FAIL);
            FAILED_CHECK_RETURN(D3DXCreateTextureFromFile(m_pGraphicDev, szImageFileName, &m_TextureInfo), E_FAIL);
            m_vecMonsterTexture.emplace_back(m_TextureInfo);
            break;
        }
    }
    return S_OK;
}

void CMapEditorScene::MapFile_Save()
{
    const _tchar* strFileName = L"../../Data/TileData.txt";
    const _tchar* strWallFileName = L"../../Data/WallData.txt";
    const _tchar* strObjectFileName = L"../../Data/ObjectData.txt";


    m_hFile = CreateFile(strFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    m_hWallFile = CreateFile(strWallFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    m_hObjectFile = CreateFile(strObjectFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

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

    if (INVALID_HANDLE_VALUE == m_hObjectFile)
    {
        MSG_BOX("Failed Object Create File");
        return;
    }

    // ========================================================

    CMapToolTerrain* pTerrain = dynamic_cast<CMapToolTerrain*>(Engine::Get_GameObject(L"Layer_GameLogic", L"MapToolTerrain"));

    auto vec = pTerrain->Get_TextureNumber();

    auto vecBoolReach = pTerrain->Get_Unreachable();

    DWORD	dwByte(0);

    for (_int i = 0; i < vec.size(); ++i)
    {
        WriteFile(m_hFile, &vec[i], sizeof(_int), &dwByte, nullptr);

        bool bReachable = vecBoolReach[i];
        WriteFile(m_hFile, &bReachable, sizeof(bool), &dwByte, nullptr);
    }

    // ========================================================

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

    // ========================================================

    //_vec3 vTempObjectPos(0.0f, 0.0f, 0.0f);
    //_int  vTempObjectImgNum(0);
    //_int  vTempObjectIndex(0);

    //DWORD	dwByte3(0);

    //for (auto& iter : m_vecBuildingObject)
    //{
    //    if (iter == nullptr)
    //    {
    //        vTempObjectIndex++;
    //        continue;
    //    }

    //    vTempObjectPos = (*iter).Get_ObjectPos();
    //    vTempObjectImgNum = (*iter).Get_BuildImgNum();

    //    // 오브젝트 저장.
    //    WriteFile(m_hObjectFile, &vTempObjectPos, sizeof(_vec3), &dwByte3, nullptr);
    //    WriteFile(m_hObjectFile, &vTempObjectImgNum, sizeof(_int), &dwByte3, nullptr);
    //    WriteFile(m_hObjectFile, &vTempObjectIndex, sizeof(_int), &dwByte3, nullptr);

    //    vTempObjectIndex++;
    //}

    CloseHandle(m_hFile);
    CloseHandle(m_hWallFile);
    CloseHandle(m_hObjectFile);
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

    const _tchar* strFileName = L"../../Data/TileData.txt";
    m_hFile = CreateFile(strFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == m_hFile)
    {
        MSG_BOX("Fail Open Terrain file");
        return E_FAIL;
    }

    DWORD dwByte = 0;
    CMapToolTerrain* pTerrain = dynamic_cast<CMapToolTerrain*>(Get_GameObject(L"Layer_GameLogic", L"MapToolTerrain"));

    if (!pTerrain)
    {
        MSG_BOX("Fail Open Terrain");
        CloseHandle(m_hFile);
        return E_FAIL;
    }

    auto vec = pTerrain->Get_TextureNumber();
    auto vecReach = pTerrain->Get_Unreachable();

    for (_int i = 0; i < vec.size(); ++i)
    {
        ReadFile(m_hFile, &vec[i], sizeof(_int), &dwByte, nullptr);

        bool bReachable = false;
        ReadFile(m_hFile, &bReachable, sizeof(bool), &dwByte, nullptr);
        pTerrain->Set_Unreachable(i, bReachable);
    }

    pTerrain->Set_TextureNumber(vec);

    CloseHandle(m_hFile);
    CloseHandle(m_hWallFile);
    MSG_BOX("Success Load File");

    return S_OK;
}

void CMapEditorScene::Free()
{
    Engine::CScene::Free();
}
