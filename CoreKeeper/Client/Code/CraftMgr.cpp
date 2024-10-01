#include "pch.h"
#include "../Header/CraftMgr.h"
#include "Export_Utility.h"

IMPLEMENT_SINGLETON(CCraftMgr)

CCraftMgr::CCraftMgr()
    : m_pGraphicDev(nullptr), m_iCraftCount(0)
{
}

CCraftMgr::~CCraftMgr()
{
    Free();
}

HRESULT CCraftMgr::Ready_Craft(LPDIRECT3DDEVICE9 pGraphicDev)
{
    m_pGraphicDev = pGraphicDev;
    m_pGraphicDev->AddRef();

    Set_Recipe();

	return S_OK;
}

bool CCraftMgr::Craftable(CInventory* _pInventory, ITEMNUM _eItemNum, MATERIAL _eMaterial)
{
    auto iter = m_mapRecipes.find({ _eItemNum, _eMaterial });
    if (iter == m_mapRecipes.end())
        return false; // 해당 아이템의 레시피가 없음

    const Recipe& recipe = iter->second;

    // 필요한 재료가 인벤토리에 있는지 확인
    for (const auto& ingredient : recipe.vecIngredients)
    {
        if (!_pInventory->Enough_Item(ingredient.eIngredient, ingredient.iQuantity))
            return false; // 재료가 부족할 경우
    }

    return true; // 모든 재료가 충분하면 제작 가능
}

CItem* CCraftMgr::Craft(CInventory* _pInventory, ITEMNUM _eItemNum, MATERIAL _eMaterial)
{
    auto iter = m_mapRecipes.find({ _eItemNum, _eMaterial });
    if (iter == m_mapRecipes.end())
        return nullptr; // 해당 아이템의 레시피가 없음

    Recipe& recipe = iter->second;

    for (const auto& ingredient : recipe.vecIngredients)
    {
        _pInventory->Minus_Item(ingredient.eIngredient, ingredient.iQuantity);
    }

    CItem* pItem = nullptr;

    switch (_eItemNum)
    {
    case ITEM_PICKAXE:
        pItem = CPickaxe::Create(m_pGraphicDev, _eMaterial);
        break;
    case ITEM_HOE:
        pItem = CHoe::Create(m_pGraphicDev, _eMaterial);
        break;
    case ITEM_WATERINGCAN:
        pItem = CWateringCan::Create(m_pGraphicDev, _eMaterial);
        break;

    case ITEM_SWORD:
        pItem = CSword::Create(m_pGraphicDev, _eMaterial);
        break;
    case ITEM_BOW:
        pItem = CBow::Create(m_pGraphicDev, _eMaterial);
        break;
    case ITEM_STAFF:
        pItem = CStaff::Create(m_pGraphicDev);
        break;

    case ITEM_HELMET:
        pItem = CHelmet::Create(m_pGraphicDev, _eMaterial);
        break;
    case ITEM_CHEST:
        pItem = CChest::Create(m_pGraphicDev, _eMaterial);
        break;
    case ITEM_LEG:
        pItem = CLeg::Create(m_pGraphicDev, _eMaterial);
        break;

    //case ITEM_NECKLACE:
    //    pItem = CNecklace::Create(m_pGraphicDev, _eMaterial);
    //    break;
    //case ITEM_RING:
    //    pItem = CRing::Create(m_pGraphicDev, _eMaterial);
    //    break;
    //case ITEM_BAG:
    //    pItem = CBag::Create(m_pGraphicDev, _eMaterial);
    //    break;
    //case ITEM_LANTERN:
    //    pItem = CLantern::Create(m_pGraphicDev, _eMaterial);
    //    break;
    //case ITEM_ASSISTANCE:
    //    pItem = CAssistance::Create(m_pGraphicDev, _eMaterial);
    //    break;
        /*
    case ITEM_TABLE:
        pItem = CTable::Create(m_pGraphicDev, _eMaterial);
        break;
    case ITEM_POTION_TABLE:
        pItem = CPotionTable::Create(m_pGraphicDev, _eMaterial);
        break;
    case ITEM_ACCESSORY_TABLE:
        pItem = CAccessoryTable::Create(m_pGraphicDev, _eMaterial);
        break;
    case ITEM_MUSIC_TABLE:
        pItem = CLeg::Create(m_pGraphicDev, _eMaterial);
        break;
    case ITME_ANIMAL_TABLE:
        pItem = CLeg::Create(m_pGraphicDev, _eMaterial);
        break;

    case ITEM_ANVIL:
        pItem = CLeg::Create(m_pGraphicDev, _eMaterial);
        break;
    case ITEM_FURNACE:
        pItem = CLeg::Create(m_pGraphicDev, _eMaterial);
        break;
    case ITEM_COOKINGPOT:
        pItem = CLeg::Create(m_pGraphicDev, _eMaterial);
        break;*/

    case ITEM_TORCH:
        pItem = CTorch::Create(m_pGraphicDev);
        pItem->Add_Count(2);
        break;
    //case ITEM_BOX:
    //    pItem = CBox::Create(m_pGraphicDev);
    //    break;
    //case ITEM_SPRINKLER:
    //    pItem = CTorch::Create(m_pGraphicDev);
    //    break;


    //case ITEM_POTION_HP:
    //case ITEM_POTION_ATT:
    //case ITEM_POTION_DEF:

        // 요리 추가

    //case ITEM_PLAYER_SPAWNER:
    //case ITEM_MAL_SPAWNER:
    //case ITEM_AZEOS_SPAWNER:
    }

    strCraftName[m_iCraftCount] = L"Craft_Item" + to_wstring(m_iCraftCount);

    CScene* pScene = Engine::Get_Scene();

    pScene->Create_GameObject(L"Layer_UI", pItem, strCraftName[m_iCraftCount++].c_str());

	return pItem;
}

void CCraftMgr::Set_Recipe()
{
    // 나무 곡괭이
    Recipe woodPickAxeRecipe;
    woodPickAxeRecipe.vecIngredients.push_back({ITEM_WOOD, 4});
    m_mapRecipes[make_pair(ITEM_PICKAXE, MATERIAL_WOOD)] = woodPickAxeRecipe;

    // 구리 곡괭이
    Recipe copperPickAxeRecipe;
    copperPickAxeRecipe.vecIngredients.push_back({ ITEM_WOOD, 4 });
    m_mapRecipes[make_pair(ITEM_PICKAXE, MATERIAL_COPPER)] = copperPickAxeRecipe;

    // 철 곡괭이
    Recipe ironPickAxeRecipe;
    ironPickAxeRecipe.vecIngredients.push_back({ ITEM_WOOD, 4 });
    m_mapRecipes[make_pair(ITEM_PICKAXE, MATERIAL_IRON)] = ironPickAxeRecipe;

    // 나무 호미
    Recipe woodHoeRecipe;
    woodHoeRecipe.vecIngredients.push_back({ ITEM_WOOD, 4 });
    m_mapRecipes[make_pair(ITEM_HOE, MATERIAL_WOOD)] = woodHoeRecipe;

    // 구리 호미
    /*Recipe ironPickAxeRecipe;
    ironPickAxeRecipe.vecIngredients.push_back({ ITEM_WOOD, 4 });
    m_mapRecipes[make_pair(ITEM_PICKAXE, MATERIAL_WOOD)] = ironPickAxeRecipe;*/

    // 철 호미
    /*Recipe ironPickAxeRecipe;
    ironPickAxeRecipe.vecIngredients.push_back({ ITEM_WOOD, 4 });
    m_mapRecipes[make_pair(ITEM_PICKAXE, MATERIAL_WOOD)] = ironPickAxeRecipe;*/

    // 나무 칼
    /*Recipe ironPickAxeRecipe;
    ironPickAxeRecipe.vecIngredients.push_back({ ITEM_WOOD, 4 });
    m_mapRecipes[make_pair(ITEM_PICKAXE, MATERIAL_WOOD)] = ironPickAxeRecipe;*/

    // 구리 칼
    /*Recipe ironPickAxeRecipe;
    ironPickAxeRecipe.vecIngredients.push_back({ ITEM_WOOD, 4 });
    m_mapRecipes[make_pair(ITEM_PICKAXE, MATERIAL_WOOD)] = ironPickAxeRecipe;*/

    // 철 칼
    /*Recipe ironPickAxeRecipe;
    ironPickAxeRecipe.vecIngredients.push_back({ ITEM_WOOD, 4 });
    m_mapRecipes[make_pair(ITEM_PICKAXE, MATERIAL_WOOD)] = ironPickAxeRecipe;*/

    // 나무 활
    /*Recipe ironPickAxeRecipe;
    ironPickAxeRecipe.vecIngredients.push_back({ ITEM_WOOD, 4 });
    m_mapRecipes[make_pair(ITEM_PICKAXE, MATERIAL_WOOD)] = ironPickAxeRecipe;*/

    // 철 활
    /*Recipe ironPickAxeRecipe;
    ironPickAxeRecipe.vecIngredients.push_back({ ITEM_WOOD, 4 });
    m_mapRecipes[make_pair(ITEM_PICKAXE, MATERIAL_WOOD)] = ironPickAxeRecipe;*/

    // 나무 헬멧
    /*Recipe ironPickAxeRecipe;
    ironPickAxeRecipe.vecIngredients.push_back({ ITEM_WOOD, 4 });
    m_mapRecipes[make_pair(ITEM_PICKAXE, MATERIAL_WOOD)] = ironPickAxeRecipe;*/

    // 구리 헬멧
    /*Recipe ironPickAxeRecipe;
    ironPickAxeRecipe.vecIngredients.push_back({ ITEM_WOOD, 4 });
    m_mapRecipes[make_pair(ITEM_PICKAXE, MATERIAL_WOOD)] = ironPickAxeRecipe;*/

    // 철 헬멧
    /*Recipe ironPickAxeRecipe;
    ironPickAxeRecipe.vecIngredients.push_back({ ITEM_WOOD, 4 });
    m_mapRecipes[make_pair(ITEM_PICKAXE, MATERIAL_WOOD)] = ironPickAxeRecipe;*/

    // 나무 상의
    /*Recipe ironPickAxeRecipe;
    ironPickAxeRecipe.vecIngredients.push_back({ ITEM_WOOD, 4 });
    m_mapRecipes[make_pair(ITEM_PICKAXE, MATERIAL_WOOD)] = ironPickAxeRecipe;*/

    // 구리 상의
    /*Recipe ironPickAxeRecipe;
    ironPickAxeRecipe.vecIngredients.push_back({ ITEM_WOOD, 4 });
    m_mapRecipes[make_pair(ITEM_PICKAXE, MATERIAL_WOOD)] = ironPickAxeRecipe;*/

    // 철 상의
    /*Recipe ironPickAxeRecipe;
    ironPickAxeRecipe.vecIngredients.push_back({ ITEM_WOOD, 4 });
    m_mapRecipes[make_pair(ITEM_PICKAXE, MATERIAL_WOOD)] = ironPickAxeRecipe;*/

    // 나무 바지
    /*Recipe ironPickAxeRecipe;
    ironPickAxeRecipe.vecIngredients.push_back({ ITEM_WOOD, 4 });
    m_mapRecipes[make_pair(ITEM_PICKAXE, MATERIAL_WOOD)] = ironPickAxeRecipe;*/

    // 구리 바지
    /*Recipe ironPickAxeRecipe;
    ironPickAxeRecipe.vecIngredients.push_back({ ITEM_WOOD, 4 });
    m_mapRecipes[make_pair(ITEM_PICKAXE, MATERIAL_WOOD)] = ironPickAxeRecipe;*/

    // 철 바지
    /*Recipe ironPickAxeRecipe;
    ironPickAxeRecipe.vecIngredients.push_back({ ITEM_WOOD, 4 });
    m_mapRecipes[make_pair(ITEM_PICKAXE, MATERIAL_WOOD)] = ironPickAxeRecipe;*/







    // 나무 테이블
    Recipe woodTableRecipe;
    woodTableRecipe.vecIngredients.push_back({ ITEM_WOOD, 8 });
    m_mapRecipes[make_pair(ITEM_TABLE, MATERIAL_WOOD)] = woodTableRecipe;


    // 상자
    Recipe boxRecipe;
    boxRecipe.vecIngredients.push_back({ ITEM_WOOD, 5 });
    m_mapRecipes[make_pair(ITEM_BOX, MATERIAL_WOOD)] = boxRecipe;






    // 횃불
    Recipe torchRecipe;
    torchRecipe.vecIngredients.push_back({ ITEM_WOOD, 1 });
    m_mapRecipes[make_pair(ITEM_TORCH, MATERIAL_WOOD)] = torchRecipe;
}

void CCraftMgr::Free()
{
    Safe_Release(m_pGraphicDev);
}