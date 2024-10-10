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
    Set_Name();

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
        pItem = CBow::Create(m_pGraphicDev);
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

    case ITEM_NECKLACE:
        pItem = CNecklace::Create(m_pGraphicDev, _eMaterial);
        break;
    case ITEM_RING:
        pItem = CRing::Create(m_pGraphicDev, _eMaterial);
        break;

    case ITEM_ASSISTANCE:
        pItem = CAssistance::Create(m_pGraphicDev, (ASSISTANCE)_eMaterial);
        break;

    case ITEM_BAG:
        pItem = CBag::Create(m_pGraphicDev, _eMaterial);
        break;
    case ITEM_LANTERN:
        pItem = CLantern::Create(m_pGraphicDev, _eMaterial);
        break;

    case ITEM_TABLE:
        pItem = CTable::Create(m_pGraphicDev, _eMaterial);
        break;

    case ITEM_POTION_TABLE:
        pItem = CPotionTable::Create(m_pGraphicDev);
        break;
    case ITEM_ACCESSORY_TABLE:
        pItem = CAccessoryTable::Create(m_pGraphicDev);
        break;
    case ITEM_MUSIC_TABLE:
        pItem = CMusicTable::Create(m_pGraphicDev);
        break;
    case ITEM_ANIMAL_TABLE:
        pItem = CAnimalTable::Create(m_pGraphicDev);
        break;

    case ITEM_ANVIL:
        pItem = CAnvil::Create(m_pGraphicDev, _eMaterial);
        break;
    case ITEM_FURNACE:
        pItem = CFurnace::Create(m_pGraphicDev);
        break;
    case ITEM_COOKINGPOT:
        pItem = CCookingPot::Create(m_pGraphicDev);
        break;

    case ITEM_TORCH:
        pItem = CTorch::Create(m_pGraphicDev);
        pItem->Add_Count(2);
        break;
    case ITEM_BOX:
        pItem = CBox::Create(m_pGraphicDev);
        break;
    case ITEM_SPRINKLER:
        pItem = CSprinkler::Create(m_pGraphicDev);
        break;


    case ITEM_POTION_HP:
    case ITEM_POTION_ATT:
    case ITEM_POTION_DEF:
        pItem = CPotion::Create(m_pGraphicDev, _eItemNum);
        break;

    case ITEM_PLAYER_SPAWNER:
    case ITEM_MAL_SPAWNER:
    case ITEM_AZEOS_SPAWNER:
        pItem = CSpawner::Create(m_pGraphicDev, _eItemNum);
        break;

    case ITEM_COPPER_BAR:
        pItem = CBar::Create(m_pGraphicDev, MATERIAL_COPPER);
        break;
    case ITEM_IRON_BAR:
        pItem = CBar::Create(m_pGraphicDev, MATERIAL_IRON);
        break;
    case ITEM_SCARLET_BAR:
        pItem = CBar::Create(m_pGraphicDev, MATERIAL_SCARLET);
        break;
    }

    strCraftName[m_iCraftCount] = L"Craft_Item" + to_wstring(m_iCraftCount);

    CScene* pScene = Engine::Get_Scene();

    pScene->Create_GameObject(L"Layer_UI", pItem, strCraftName[m_iCraftCount++].c_str());

	return pItem;
}

CItem* CCraftMgr::CraftExp(ITEMNUM _eItemNum, MATERIAL _eMaterial)
{
    /*
    auto iter = m_mapRecipes.find({ _eItemNum, _eMaterial });
    if (iter == m_mapRecipes.end())
        return nullptr; // 해당 아이템의 레시피가 없음

    Recipe& recipe = iter->second;
    
    for (const auto& ingredient : recipe.vecIngredients)
    {
        _pInventory->Minus_Item(ingredient.eIngredient, ingredient.iQuantity);
    }*/

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
        pItem = CBow::Create(m_pGraphicDev);
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

    case ITEM_NECKLACE:
        pItem = CNecklace::Create(m_pGraphicDev, _eMaterial);
        break;
    case ITEM_RING:
        pItem = CRing::Create(m_pGraphicDev, _eMaterial);
        break;

    case ITEM_ASSISTANCE:
        pItem = CAssistance::Create(m_pGraphicDev, (ASSISTANCE)_eMaterial);
        break;

    case ITEM_BAG:
        pItem = CBag::Create(m_pGraphicDev, _eMaterial);
        break;
    case ITEM_LANTERN:
        pItem = CLantern::Create(m_pGraphicDev, _eMaterial);
        break;

    case ITEM_TABLE:
        pItem = CTable::Create(m_pGraphicDev, _eMaterial);
        break;

    case ITEM_POTION_TABLE:
        pItem = CPotionTable::Create(m_pGraphicDev);
        break;
    case ITEM_ACCESSORY_TABLE:
        pItem = CAccessoryTable::Create(m_pGraphicDev);
        break;
    case ITEM_MUSIC_TABLE:
        pItem = CMusicTable::Create(m_pGraphicDev);
        break;
    case ITEM_ANIMAL_TABLE:
        pItem = CAnimalTable::Create(m_pGraphicDev);
        break;

    case ITEM_ANVIL:
        pItem = CAnvil::Create(m_pGraphicDev, _eMaterial);
        break;
    case ITEM_FURNACE:
        pItem = CFurnace::Create(m_pGraphicDev);
        break;
    case ITEM_COOKINGPOT:
        pItem = CCookingPot::Create(m_pGraphicDev);
        break;

    case ITEM_TORCH:
        pItem = CTorch::Create(m_pGraphicDev);
        pItem->Add_Count(2);
        break;
    case ITEM_BOX:
        pItem = CBox::Create(m_pGraphicDev);
        break;
    case ITEM_SPRINKLER:
        pItem = CSprinkler::Create(m_pGraphicDev);
        break;


    case ITEM_POTION_HP:
    case ITEM_POTION_ATT:
    case ITEM_POTION_DEF:
        pItem = CPotion::Create(m_pGraphicDev, _eItemNum);
        break;

    case ITEM_PLAYER_SPAWNER:
    case ITEM_MAL_SPAWNER:
    case ITEM_AZEOS_SPAWNER:
        pItem = CSpawner::Create(m_pGraphicDev, _eItemNum);
        break;

    case ITEM_COPPER_BAR:
        pItem = CBar::Create(m_pGraphicDev, MATERIAL_COPPER);
        break;
    case ITEM_IRON_BAR:
        pItem = CBar::Create(m_pGraphicDev, MATERIAL_IRON);
        break;
    case ITEM_SCARLET_BAR:
        pItem = CBar::Create(m_pGraphicDev, MATERIAL_SCARLET);
        break;
    }

    strCraftName[m_iCraftCount] = L"Craft_Item" + to_wstring(m_iCraftCount);

    CScene* pScene = Engine::Get_Scene();

    pScene->Create_GameObject(L"Layer_UI", pItem, strCraftName[m_iCraftCount++].c_str());

    return pItem;
}

CItem* CCraftMgr::Cook(ITEMNUM _eItemNum1, ITEMNUM _eItemNum2)
{
    // 요리 재료가 아니면 nullptr 반환
    if (_eItemNum1 < ITEM_BERRY || _eItemNum1 > ITEM_MUSHROOM || _eItemNum2 < ITEM_BERRY || _eItemNum2 > ITEM_MUSHROOM)
        return nullptr;

    CItem* pFood = nullptr;

    if (_eItemNum1 > _eItemNum2)
        pFood = CFood::Create(m_pGraphicDev, _eItemNum2, _eItemNum1);
    else
        pFood = CFood::Create(m_pGraphicDev, _eItemNum1, _eItemNum2);

    CScene* pScene = Engine::Get_Scene();

    pScene->Create_GameObject(L"Layer_UI", pFood, strCraftName[m_iCraftCount++].c_str());

    return pFood;
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
    copperPickAxeRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 2});
    m_mapRecipes[make_pair(ITEM_PICKAXE, MATERIAL_COPPER)] = copperPickAxeRecipe;

    // 철 곡괭이
    Recipe ironPickAxeRecipe;
    ironPickAxeRecipe.vecIngredients.push_back({ ITEM_WOOD, 4 });
    ironPickAxeRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 1 });
    ironPickAxeRecipe.vecIngredients.push_back({ ITEM_IRON_BAR, 4 });
    m_mapRecipes[make_pair(ITEM_PICKAXE, MATERIAL_IRON)] = ironPickAxeRecipe;

    // 나무 호미
    Recipe woodHoeRecipe;
    woodHoeRecipe.vecIngredients.push_back({ ITEM_WOOD, 4 });
    m_mapRecipes[make_pair(ITEM_HOE, MATERIAL_WOOD)] = woodHoeRecipe;

    Recipe wateringcan;
    woodHoeRecipe.vecIngredients.push_back({ ITEM_WOOD, 3 });
    m_mapRecipes[make_pair(ITEM_WATERINGCAN, MATERIAL_WOOD)] = wateringcan;

    // 구리 호미
    Recipe copperHoeRecipe;
    copperHoeRecipe.vecIngredients.push_back({ ITEM_WOOD, 4 });
    copperHoeRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 2 });
    m_mapRecipes[make_pair(ITEM_HOE, MATERIAL_COPPER)] = copperHoeRecipe;

    // 철 호미
    Recipe ironHoeRecipe;
    ironHoeRecipe.vecIngredients.push_back({ ITEM_WOOD, 4 });
    ironHoeRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 1 });
    ironHoeRecipe.vecIngredients.push_back({ ITEM_IRON_BAR, 4 });
    m_mapRecipes[make_pair(ITEM_HOE, MATERIAL_IRON)] = ironHoeRecipe;

    // 구리 물뿌리개
    Recipe copperWateringCanRecipe;
    copperWateringCanRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 4 });
    m_mapRecipes[make_pair(ITEM_WATERINGCAN, MATERIAL_COPPER)] = copperWateringCanRecipe;

    // 철 물뿌리개
    Recipe ironWateringCanRecipe;
    ironWateringCanRecipe.vecIngredients.push_back({ ITEM_IRON_BAR, 10 });
    m_mapRecipes[make_pair(ITEM_WATERINGCAN, MATERIAL_IRON)] = ironWateringCanRecipe;

    // 나무 칼
    Recipe woodSwordRecipe;
    woodSwordRecipe.vecIngredients.push_back({ ITEM_WOOD, 5 });
    m_mapRecipes[make_pair(ITEM_SWORD, MATERIAL_WOOD)] = woodSwordRecipe;

    // 구리 칼
    Recipe copperSwordRecipe;
    copperSwordRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 5 });
    m_mapRecipes[make_pair(ITEM_SWORD, MATERIAL_COPPER)] = copperSwordRecipe;

    // 철 칼
    Recipe ironSwordRecipe;
    ironSwordRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 1 });
    ironSwordRecipe.vecIngredients.push_back({ ITEM_IRON_BAR, 8 });
    m_mapRecipes[make_pair(ITEM_SWORD, MATERIAL_IRON)] = ironSwordRecipe;

    // 철 활
    Recipe ironBowRecipe;
    ironBowRecipe.vecIngredients.push_back({ ITEM_WOOD, 5 });
    ironBowRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 1 });
    ironBowRecipe.vecIngredients.push_back({ ITEM_IRON_BAR, 7 });
    m_mapRecipes[make_pair(ITEM_BOW, MATERIAL_IRON)] = ironBowRecipe;
    m_mapRecipes[make_pair(ITEM_BOW, MATERIAL_END)] = ironBowRecipe;

    // 나무 헬멧
    Recipe woodHelmetRecipe;
    woodHelmetRecipe.vecIngredients.push_back({ ITEM_WOOD, 4 });
    m_mapRecipes[make_pair(ITEM_HELMET, MATERIAL_WOOD)] = woodHelmetRecipe;

    // 구리 헬멧
    Recipe copperHelmetRecipe;
    copperHelmetRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 6 });
    m_mapRecipes[make_pair(ITEM_HELMET, MATERIAL_COPPER)] = copperHelmetRecipe;

    // 철 헬멧
    Recipe ironHelmetRecipe;
    ironHelmetRecipe.vecIngredients.push_back({ ITEM_FIBER, 5 });
    ironHelmetRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 1 });
    ironHelmetRecipe.vecIngredients.push_back({ ITEM_IRON_BAR, 4 });
    m_mapRecipes[make_pair(ITEM_HELMET, MATERIAL_IRON)] = ironHelmetRecipe;

    // 나무 상의
    Recipe woodChestRecipe;
    woodChestRecipe.vecIngredients.push_back({ ITEM_WOOD, 6 });
    m_mapRecipes[make_pair(ITEM_CHEST, MATERIAL_WOOD)] = woodChestRecipe;

    // 구리 상의
    Recipe copperChestRecipe;
    copperChestRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 8 });
    m_mapRecipes[make_pair(ITEM_CHEST, MATERIAL_COPPER)] = copperChestRecipe;

    // 철 상의
    Recipe ironChestRecipe;
    ironChestRecipe.vecIngredients.push_back({ ITEM_FIBER, 6 });
    ironChestRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 3 });
    ironChestRecipe.vecIngredients.push_back({ ITEM_IRON_BAR, 15 });
    m_mapRecipes[make_pair(ITEM_CHEST, MATERIAL_IRON)] = ironChestRecipe;

    // 나무 바지
    Recipe woodLegRecipe;
    woodLegRecipe.vecIngredients.push_back({ ITEM_WOOD, 5 });
    m_mapRecipes[make_pair(ITEM_LEG, MATERIAL_WOOD)] = woodLegRecipe;

    // 구리 바지
    Recipe copperLegRecipe;
    copperLegRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 7 });
    m_mapRecipes[make_pair(ITEM_LEG, MATERIAL_COPPER)] = copperLegRecipe;

    // 철 바지
    Recipe ironLegRecipe;
    ironLegRecipe.vecIngredients.push_back({ ITEM_FIBER, 6 });
    ironLegRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 2 });
    ironLegRecipe.vecIngredients.push_back({ ITEM_IRON_BAR, 13 });
    m_mapRecipes[make_pair(ITEM_LEG, MATERIAL_IRON)] = ironLegRecipe;

    // 구리 목걸이
    Recipe copperNecklaceRecipe;
    copperNecklaceRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 10 });
    m_mapRecipes[make_pair(ITEM_NECKLACE, MATERIAL_COPPER)] = copperNecklaceRecipe;

    // 철 덩어리 목걸이
    Recipe ironNecklaceRecipe;
    ironNecklaceRecipe.vecIngredients.push_back({ ITEM_IRON_BAR, 10 });
    m_mapRecipes[make_pair(ITEM_NECKLACE, MATERIAL_IRON)] = ironNecklaceRecipe;

    // 돌 반지
    Recipe magnetRingRecipe;
    magnetRingRecipe.vecIngredients.push_back({ ITEM_DIRTWALL, 5 });
    magnetRingRecipe.vecIngredients.push_back({ ITEM_STONEWALL, 5 });
    magnetRingRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 2 });
    m_mapRecipes[make_pair(ITEM_RING, MATERIAL_COPPER)] = magnetRingRecipe;

    // 신속 반지
    Recipe speedRingRecipe;
    speedRingRecipe.vecIngredients.push_back({ ITEM_FEATHER_PIECE, 3 });
    speedRingRecipe.vecIngredients.push_back({ ITEM_IRON_BAR, 10 });
    m_mapRecipes[make_pair(ITEM_RING, MATERIAL_IRON)] = speedRingRecipe;

    // 벨트 주머니
    Recipe copperBagRecipe;
    copperBagRecipe.vecIngredients.push_back({ ITEM_WOOD, 5 });
    copperBagRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 2 });
    m_mapRecipes[make_pair(ITEM_BAG, MATERIAL_COPPER)] = copperBagRecipe;

    // 탐험가 배낭
    Recipe ironBagRecipe;
    ironBagRecipe.vecIngredients.push_back({ ITEM_FIBER, 10 });
    ironBagRecipe.vecIngredients.push_back({ ITEM_IRON_BAR, 5 });
    m_mapRecipes[make_pair(ITEM_BAG, MATERIAL_IRON)] = ironBagRecipe;

    // 소형 등불
    Recipe copperLanternRecipe;
    copperLanternRecipe.vecIngredients.push_back({ ITEM_MUCUS, 3 });
    copperLanternRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 3 });
    m_mapRecipes[make_pair(ITEM_LANTERN, MATERIAL_COPPER)] = copperLanternRecipe;

    // 구체 등불
    Recipe ironLanternRecipe;
    ironLanternRecipe.vecIngredients.push_back({ ITEM_MUCUS, 8 });
    ironLanternRecipe.vecIngredients.push_back({ ITEM_IRON_BAR, 8 });
    m_mapRecipes[make_pair(ITEM_LANTERN, MATERIAL_IRON)] = ironLanternRecipe;

    // 나무 방패
    Recipe woodShieldRecipe;
    woodShieldRecipe.vecIngredients.push_back({ ITEM_WOOD, 10 });
    woodShieldRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 5 });
    m_mapRecipes[make_pair(ITEM_ASSISTANCE, MATERIAL_COPPER)] = woodShieldRecipe;

    // 철 방패
    Recipe ironShieldRecipe;
    ironShieldRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 2 });
    ironShieldRecipe.vecIngredients.push_back({ ITEM_IRON_BAR, 8 });
    m_mapRecipes[make_pair(ITEM_ASSISTANCE, MATERIAL_IRON)] = ironShieldRecipe;

    // 책
    Recipe bookRecipe;
    bookRecipe.vecIngredients.push_back({ ITEM_FIBER, 8 });
    bookRecipe.vecIngredients.push_back({ ITEM_SKULL_PIECE, 8 });
    m_mapRecipes[make_pair(ITEM_ASSISTANCE, MATERIAL_SPECIAL)] = bookRecipe;

    // 나무 작업대
    Recipe woodTableRecipe;
    woodTableRecipe.vecIngredients.push_back({ ITEM_WOOD, 8 });
    m_mapRecipes[make_pair(ITEM_TABLE, MATERIAL_WOOD)] = woodTableRecipe;

    // 구리 작업대
    Recipe copperTableRecipe;
    copperTableRecipe.vecIngredients.push_back({ ITEM_WOOD, 8 });
    copperTableRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 6 });
    m_mapRecipes[make_pair(ITEM_TABLE, MATERIAL_COPPER)] = copperTableRecipe;

    // 철제 작업대
    Recipe ironTableRecipe;
    ironTableRecipe.vecIngredients.push_back({ ITEM_WOOD, 10 });
    ironTableRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 10 });
    ironTableRecipe.vecIngredients.push_back({ ITEM_IRON_BAR, 15 });
    m_mapRecipes[make_pair(ITEM_TABLE, MATERIAL_IRON)] = ironTableRecipe;

    // 연금술 작업대
    Recipe potionTableRecipe;
    potionTableRecipe.vecIngredients.push_back({ ITEM_WOOD, 8 });
    potionTableRecipe.vecIngredients.push_back({ ITEM_MUCUS, 8 });
    potionTableRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 5 });
    m_mapRecipes[make_pair(ITEM_POTION_TABLE, MATERIAL_COPPER)] = potionTableRecipe;

    // 장신구 작업대
    Recipe accessoryTableRecipe;
    accessoryTableRecipe.vecIngredients.push_back({ ITEM_WOOD, 8 });
    accessoryTableRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 4 });
    accessoryTableRecipe.vecIngredients.push_back({ ITEM_IRON_BAR, 8 });
    m_mapRecipes[make_pair(ITEM_ACCESSORY_TABLE, MATERIAL_IRON)] = accessoryTableRecipe;

    // 음악 작업대
    Recipe musicTableRecipe;
    musicTableRecipe.vecIngredients.push_back({ ITEM_FIBER, 5 });
    musicTableRecipe.vecIngredients.push_back({ ITEM_WOOD, 8 });
    m_mapRecipes[make_pair(ITEM_MUSIC_TABLE, MATERIAL_IRON)] = musicTableRecipe;

    // 구리 모루
    Recipe copperAnvilRecipe;
    copperAnvilRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 8 });
    m_mapRecipes[make_pair(ITEM_ANVIL, MATERIAL_COPPER)] = copperAnvilRecipe;

    // 철제 모루
    Recipe ironAnvilRecipe;
    ironAnvilRecipe.vecIngredients.push_back({ ITEM_IRON_BAR, 10 });
    m_mapRecipes[make_pair(ITEM_ANVIL, MATERIAL_IRON)] = ironAnvilRecipe;

    // 용광로
    Recipe furnaceRecipe;
    furnaceRecipe.vecIngredients.push_back({ ITEM_DIRTWALL, 20 });
    m_mapRecipes[make_pair(ITEM_FURNACE, MATERIAL_WOOD)] = furnaceRecipe;

    // 요리솥
    Recipe cookingPotRecipe;
    cookingPotRecipe.vecIngredients.push_back({ ITEM_WOOD, 2 });
    cookingPotRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 4 });
    m_mapRecipes[make_pair(ITEM_COOKINGPOT, MATERIAL_WOOD)] = cookingPotRecipe;

    // 횃불
    Recipe torchRecipe;
    torchRecipe.vecIngredients.push_back({ ITEM_WOOD, 1 });
    m_mapRecipes[make_pair(ITEM_TORCH, MATERIAL_END)] = torchRecipe;

    // 상자
    Recipe boxRecipe;
    boxRecipe.vecIngredients.push_back({ ITEM_WOOD, 5 });
    m_mapRecipes[make_pair(ITEM_BOX, MATERIAL_END)] = boxRecipe;

    // 스프링쿨러
    Recipe sprinklerRecipe;
    sprinklerRecipe.vecIngredients.push_back({ ITEM_IRON_BAR, 8 });
    sprinklerRecipe.vecIngredients.push_back({ ITEM_SCARLET_BAR, 8 });
    m_mapRecipes[make_pair(ITEM_SPRINKLER, MATERIAL_IRON)] = sprinklerRecipe;

    // HP 포션
    Recipe hpPotionRecipe;
    hpPotionRecipe.vecIngredients.push_back({ ITEM_MUCUS, 3 });
    hpPotionRecipe.vecIngredients.push_back({ ITEM_BERRY, 3 });
    m_mapRecipes[make_pair(ITEM_POTION_HP, MATERIAL_WOOD)] = hpPotionRecipe;

    // ATT 포션
    Recipe attPotionRecipe;
    attPotionRecipe.vecIngredients.push_back({ ITEM_MUCUS, 3 });
    attPotionRecipe.vecIngredients.push_back({ ITEM_PEPPER, 2 });
    m_mapRecipes[make_pair(ITEM_POTION_ATT, MATERIAL_WOOD)] = attPotionRecipe;

    // DEF 포션
    Recipe defPotionRecipe;
    defPotionRecipe.vecIngredients.push_back({ ITEM_MUCUS, 3 });
    defPotionRecipe.vecIngredients.push_back({ ITEM_CARROT, 3 });
    m_mapRecipes[make_pair(ITEM_POTION_DEF, MATERIAL_WOOD)] = defPotionRecipe;

    // 플레이어 스포너
    Recipe playerSpawnerRecipe;
    playerSpawnerRecipe.vecIngredients.push_back({ ITEM_DIRTWALL, 10 });
    playerSpawnerRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 1 });
    playerSpawnerRecipe.vecIngredients.push_back({ ITEM_STONEWALL, 5 });
    playerSpawnerRecipe.vecIngredients.push_back({ ITEM_IRON_BAR, 1 });
    m_mapRecipes[make_pair(ITEM_PLAYER_SPAWNER, MATERIAL_END)] = playerSpawnerRecipe;

    // 말루가즈 스포너
    Recipe malugazSpawnerRecipe;
    malugazSpawnerRecipe.vecIngredients.push_back({ ITEM_SKULL_PIECE, 3 });
    m_mapRecipes[make_pair(ITEM_MAL_SPAWNER, MATERIAL_END)] = malugazSpawnerRecipe;

    // 아제오스 스포너
    Recipe azeosSpawnerRecipe;
    azeosSpawnerRecipe.vecIngredients.push_back({ ITEM_COPPER_BAR, 10 });
    azeosSpawnerRecipe.vecIngredients.push_back({ ITEM_IRON_BAR, 10 });
    azeosSpawnerRecipe.vecIngredients.push_back({ ITEM_SCARLET_BAR, 10 });
    azeosSpawnerRecipe.vecIngredients.push_back({ ITEM_FEATHER_PIECE, 3 });
    m_mapRecipes[make_pair(ITEM_AZEOS_SPAWNER, MATERIAL_END)] = azeosSpawnerRecipe;

    // 구리 주괴
    Recipe CopperBarRecipe;
    CopperBarRecipe.vecIngredients.push_back({ ITEM_COPPER, 1 });
    m_mapRecipes[make_pair(ITEM_COPPER_BAR, MATERIAL_COPPER)] = CopperBarRecipe;

    // 철 주괴
    Recipe IronBarRecipe;
    IronBarRecipe.vecIngredients.push_back({ ITEM_IRON, 1 });
    m_mapRecipes[make_pair(ITEM_IRON_BAR, MATERIAL_IRON)] = IronBarRecipe;

    // 진홍석 주괴
    Recipe ScarletBarRecipe;
    ScarletBarRecipe.vecIngredients.push_back({ ITEM_SCARLET, 1 });
    m_mapRecipes[make_pair(ITEM_SCARLET_BAR, MATERIAL_SCARLET)] = ScarletBarRecipe;
}

void CCraftMgr::Set_Name()
{
    // 재료들의 이름
    // 재료로 쓰이는 것만 일단 추가해두겠습니다.
        
    m_mapName[ITEM_BERRY] = L"원기 베리";
    m_mapName[ITEM_PEPPER] = L"폭탄 후추";
    m_mapName[ITEM_CARROT] = L"돌당근";
    m_mapName[ITEM_MUSHROOM] = L"버섯";

    m_mapName[ITEM_COPPER] = L"구리광석";
    m_mapName[ITEM_IRON] = L"철광석";
    m_mapName[ITEM_SCARLET] = L"진홍석";

    m_mapName[ITEM_COPPER_BAR] = L"구리 주괴";
    m_mapName[ITEM_IRON_BAR] = L"철 주괴";
    m_mapName[ITEM_SCARLET_BAR] = L"진홍 바";

    m_mapName[ITEM_DIRTWALL] = L"진흙 블록";
    m_mapName[ITEM_STONEWALL] = L"돌 블록";
    m_mapName[ITEM_GRASSWALL] = L"잔디 블록";

    m_mapName[ITEM_WOOD] = L"나무";
    m_mapName[ITEM_MUCUS] = L"슬라임";
    m_mapName[ITEM_FIBER] = L"섬유질";
    m_mapName[ITEM_SKULL_PIECE] = L"크리스탈 해골 조각";
    m_mapName[ITEM_FEATHER_PIECE] = L"고대 깃털";
}

void CCraftMgr::Free()
{
    Safe_Release(m_pGraphicDev);
}