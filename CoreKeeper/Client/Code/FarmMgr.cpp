#include "pch.h"
#include "../Header/FarmMgr.h"
#include "Export_Utility.h"
#include "GameObject.h"
#include "../Header/Plant.h"
#include "../Header/Seed.h"
#include "..\Header\Ingredient.h"
#include "../Header/Terrain.h"

IMPLEMENT_SINGLETON(CFarmMgr)

CFarmMgr::CFarmMgr() : m_pGraphicDev(nullptr), m_pTerrain(nullptr)
{
}

CFarmMgr::~CFarmMgr()
{
    Free();
}

HRESULT CFarmMgr::Ready_Farm(LPDIRECT3DDEVICE9 pGraphicDev)
{
    m_pGraphicDev = pGraphicDev;
    m_pGraphicDev->AddRef();
    m_iGrowTime = 5;
    return S_OK;
}

void CFarmMgr::Update_Farm(const _float& fTimeDelta)
{
    if (!m_pTerrain)
        m_pTerrain = dynamic_cast<CTerrain*>(Engine::Get_GameObject(L"Layer_Environment", L"Terrain"));
    Grow_Plant();
}

void CFarmMgr::Plant_Job(_int iIndex, ITEMNUM eIngredient)
{
    if(!Create_Plant(iIndex, eIngredient))
        Harvest_Plant(iIndex);
}

_bool CFarmMgr::Create_Plant(_int iIndex, ITEMNUM eIngredient)
{
    // 돌당근 : 해당 인덱스의 타일이 29일 때 심기 가능
    if (eIngredient == ITEM_CARROT_SEED)
    {
        if (m_pTerrain->Get_TextureNumber(iIndex) == 29)
        {
            // 해당 인덱스에 오브젝트가 없을 때
            wstring strBuffer = L"Plant_" + std::to_wstring(iIndex);
            if (m_mapPlant.find(strBuffer) == m_mapPlant.end())
            {
                // 해당 식물 오브젝트 생성
                CScene* pScene = Engine::Get_Scene();
                CGameObject* pPlant = CPlant::Create(m_pGraphicDev, eIngredient, iIndex);
                m_listPlantedName.push_back(L"Plant" + std::to_wstring(iIndex));
                pScene->Create_GameObject(L"Layer_GameLogic", pPlant, m_listPlantedName.back().c_str());
                m_mapPlant.emplace(std::to_wstring(iIndex), pPlant);
                CAnimator* pPlantAnimator = dynamic_cast<CAnimator*>(pPlant->Get_Component(ID_STATIC, L"Com_Animator"));
                pPlantAnimator->Set_CurState(IDLE, 0, 0, 20);
                POINT tState = { iIndex, 0 };
                m_vecPlantedState.push_back(tState);
                return true;
            }
        }
    }
    else
    {
        // 돌당근 제외 : 해당 인덱스의 타일이 27 or 28일 때 심기 가능
        if (m_pTerrain->Get_TextureNumber(iIndex) == 27 || m_pTerrain->Get_TextureNumber(iIndex) == 28)
        {
            // 해당 인덱스에 오브젝트가 없을 때
            if (m_mapPlant.find(std::to_wstring(iIndex)) == m_mapPlant.end())
            {
                // 해당 식물 오브젝트 생성
                CScene* pScene = Engine::Get_Scene();
                CGameObject* pPlant = CPlant::Create(m_pGraphicDev, eIngredient, iIndex);
                m_listPlantedName.push_back(L"Plant_" + std::to_wstring(iIndex));
                pScene->Create_GameObject(L"Layer_GameLogic", pPlant, m_listPlantedName.back().c_str());
                m_mapPlant.emplace(std::to_wstring(iIndex), pPlant);
                CAnimator* pPlantAnimator = dynamic_cast<CAnimator*>(pPlant->Get_Component(ID_STATIC, L"Com_Animator"));
                // 타일이 28이면 바로 애니메이션 + 1
                if (m_pTerrain->Get_TextureNumber(iIndex) == 27)
                    pPlantAnimator->Set_CurState(IDLE, 0, 0, 20);
                else
                    pPlantAnimator->Set_CurState(IDLE, 1, 1, 20);
                POINT tState = { iIndex, 0 };
                m_vecPlantedState.push_back(tState);
                return true;
            }
        }
    }
    return false;
}

_bool CFarmMgr::Harvest_Plant(_int iIndex)
{
    wstring strBuffer = std::to_wstring(iIndex);
    if (m_mapPlant.find(strBuffer) == m_mapPlant.end())
        return false;
    CGameObject* pPlant = m_mapPlant.find(std::to_wstring(iIndex))->second;
    CTransform* pPlantTransform = dynamic_cast<CTransform*>(pPlant->Get_Component(ID_DYNAMIC, L"Com_Transform"));
    CAnimator* pPlantAnimator = dynamic_cast<CAnimator*>(pPlant->Get_Component(ID_STATIC, L"Com_Animator"));
    _int    iResult = pPlantAnimator->Get_MotionIndex();
    ITEMNUM eResultItem = dynamic_cast<CPlant*>(pPlant)->Get_ItemNum();
    _vec3  vPos;
    pPlantTransform->Get_Info(INFO_POS, &vPos);
    // 해당 인덱스의 식물이 덜 자란 상태면 씨앗 아이템 생성
    if (iResult < 5)
        Create_Seed(eResultItem, vPos);
    // 해당 인덱스의 식물이 다 자란 상태면 결과 아이템 생성
    else
        Create_Result(eResultItem, vPos);
    
    // 팜 매니저에서 삭제
    m_mapPlant.erase(std::to_wstring(iIndex));
    for (_int i = 0; i < m_vecPlantedState.size(); i++)
    {
        if (m_vecPlantedState[i].x == iIndex)
        {
            m_vecPlantedState.erase(m_vecPlantedState.begin() + i);
            break;
        }
    }
    strBuffer = L"Plant_" + std::to_wstring(iIndex);
    for (auto iter = m_listPlantedName.begin(); iter != m_listPlantedName.end(); iter++)
    {
        if (!lstrcmp(strBuffer.c_str(), (*iter).c_str()))
        {
            // 스테이지에서 삭제
            CScene* pScene = Engine::Get_Scene();
            pScene->Delete_GameObject(L"Layer_GameLogic", pPlant, (*iter).c_str());
            // 네임 리스트에서 삭제
            m_listPlantedName.erase(iter);
            // 타일 인덱스값 바꿔주기
            m_pTerrain->Set_TextureNumber(iIndex, 27);
            break;
        }
    }
    return true;
}

void CFarmMgr::Watering_Plant(_int iIndex)
{
    // 타일이 27이면 28로 변경
    if (0 <= iIndex && iIndex < (VTXCNTX - 1) * (VTXCNTZ - 1))
    {
        if (m_pTerrain->Get_TextureNumber(iIndex) == 27)
            m_pTerrain->Set_TextureNumber(iIndex, 28);
    
        wstring strBuffer = std::to_wstring(iIndex);
        if (m_mapPlant.find(strBuffer) == m_mapPlant.end())
            return;
        // 작물이 씨앗 상태로(애니메이션 인덱스 0) 심어져 있을 경우 애니메이션 인덱스 1로 변경
        CGameObject* pPlant = m_mapPlant.find(strBuffer)->second;
        CAnimator* pPlantAnimator = dynamic_cast<CAnimator*>(pPlant->Get_Component(ID_STATIC, L"Com_Animator"));
        if (pPlantAnimator->Get_MotionIndex() == 0)
            pPlantAnimator->Set_CurState(IDLE, 1, 1, 20);
    }
}

_bool CFarmMgr::Get_GrownUp(_int iIndex)
{
    wstring strBuffer = std::to_wstring(iIndex);
    if (m_mapPlant.find(strBuffer) == m_mapPlant.end())
        return false;
    CGameObject* pPlant = m_mapPlant.find(strBuffer)->second;
    CAnimator* pPlantAnimator = dynamic_cast<CAnimator*>(pPlant->Get_Component(ID_STATIC, L"Com_Animator"));
    return pPlantAnimator->Get_MotionIndex() == 5;
}

void CFarmMgr::Grow_Plant()
{
    // 현재 애니메이션 상태가 1 이상인 각 작물의 지난 시간 ++
    // 일정 시간이 지나면 해당 게임 오브젝트의 위치값, 애니메이션 인덱스 변경
    for (_int i = 0; i < m_vecPlantedState.size(); i++)
    {
        _int iIndex = m_vecPlantedState[i].x;
        wstring strBuffer = std::to_wstring(iIndex);
        CGameObject* pPlant = m_mapPlant.find(strBuffer)->second;
        CAnimator* pPlantAnimator = dynamic_cast<CAnimator*>(pPlant->Get_Component(ID_STATIC, L"Com_Animator"));
        _int iMotion = pPlantAnimator->Get_MotionIndex();
        if (iMotion > 0 && iMotion < 5)
            m_vecPlantedState[i].y += 1;

        if (m_vecPlantedState[i].y > 60 * m_iGrowTime)
        {
            m_vecPlantedState[i].y = 0;
            iMotion++;
            pPlantAnimator->Set_CurState(IDLE, iMotion, iMotion, 20);
        }
    }
}

void CFarmMgr::Create_Seed(ITEMNUM eItemNum, _vec3 vPos)
{
    CScene* pScene = Engine::Get_Scene();
    CGameObject* pSeed = CSeed::Create(m_pGraphicDev, eItemNum, vPos);
    m_vecSeedName.push_back(L"Seed_" + std::to_wstring(iCreateNumber++));
    pScene->Create_GameObject(L"Layer_GameLogic", pSeed, m_vecSeedName.back().c_str());
    dynamic_cast<CItem*>(pSeed)->Set_Drop(true);
}

void CFarmMgr::Create_Result(ITEMNUM eItemNum, _vec3 vPos)
{
    CScene* pScene = Engine::Get_Scene();
    CGameObject* pIngredient = CIngredient::Create(m_pGraphicDev, eItemNum, vPos);
    m_vecResultName.push_back(L"Result_" + std::to_wstring(iCreateNumber++));
    pScene->Create_GameObject(L"Layer_GameLogic", pIngredient, m_vecResultName.back().c_str());
    dynamic_cast<CItem*>(pIngredient)->Set_Drop(true);
}

void CFarmMgr::Free()
{
    Safe_Release(m_pGraphicDev);
}