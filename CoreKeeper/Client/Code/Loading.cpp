#include "pch.h"
#include "..\Header\Loading.h"
#include "Export_Utility.h"

CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev)
    : m_pGraphicDev(pGraphicDev)
    , m_bFinish(false)
{
    m_pGraphicDev->AddRef();
}


CLoading::~CLoading()
{
}

HRESULT CLoading::Ready_Loading(LOADINGID eLoading)
{
    InitializeCriticalSection(&m_Crt);

    m_eID = eLoading;

    m_hThread = (HANDLE)_beginthreadex(NULL, 0, Thread_Main, this, 0, NULL);

    return S_OK;
}

_uint CLoading::Loading_Stage()
{
    lstrcpy(m_szLoading, L"Etc Loading...");

    // ETC
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Transform", Engine::CTransform::Create(m_pGraphicDev)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Calculator", Engine::CCalculator::Create(m_pGraphicDev)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Animator", Engine::CAnimator::Create(m_pGraphicDev)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_State", Engine::CState::Create(m_pGraphicDev)), E_FAIL);

    // INVENTORY
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerInventory", Engine::CInventory::Create(m_pGraphicDev, 30)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_EquipInventory", Engine::CInventory::Create(m_pGraphicDev, 10)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MouseInventory", Engine::CInventory::Create(m_pGraphicDev, 1)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TrashInventory", Engine::CInventory::Create(m_pGraphicDev, 1)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ChestInventory", Engine::CInventory::Create(m_pGraphicDev, 18)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_GravestoneInventory", Engine::CInventory::Create(m_pGraphicDev, 30)), E_FAIL);

    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_FurnaceInventory", Engine::CInventory::Create(m_pGraphicDev, 1)), E_FAIL);

    lstrcpy(m_szLoading, L"Buffer Loading...");

    // BUFFER
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RcCol", Engine::CRcCol::Create(m_pGraphicDev)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RcTex", Engine::CRcTex::Create(m_pGraphicDev)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TerrainTex", Engine::CTerrainTex::Create(m_pGraphicDev)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CubeTex", Engine::CCubeTex::Create(m_pGraphicDev)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MapToolTex", Engine::CMapToolTex::Create(m_pGraphicDev)), E_FAIL);
    // Wall 전용, WallTex
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_WallTex", Engine::CWallTex::Create(m_pGraphicDev)), E_FAIL);

    //이미지 개수 넣으면 됩니다 ex) Miner.png보면 가로 15개 세로 2개
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_AnimTex", Engine::CAnimTex::Create(m_pGraphicDev, 9, 6)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_NormalAnimTex", Engine::CAnimTex::Create(m_pGraphicDev, 1, 1)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ToolAnimTex", Engine::CAnimTex::Create(m_pGraphicDev, 3, 3)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SwordAnimTex", Engine::CAnimTex::Create(m_pGraphicDev, 3, 3)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BowAnimTex", Engine::CAnimTex::Create(m_pGraphicDev, 5, 4)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ArrowAnimTex", Engine::CAnimTex::Create(m_pGraphicDev, 4, 5)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_StaffAnimTex", Engine::CAnimTex::Create(m_pGraphicDev, 5, 4)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MagicAnimTex", Engine::CAnimTex::Create(m_pGraphicDev, 6, 1)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TorchAnimTex", Engine::CAnimTex::Create(m_pGraphicDev, 6, 1)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SeedAnimTex", Engine::CAnimTex::Create(m_pGraphicDev, 2, 1)), E_FAIL);
    
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SlimeAnimTex", Engine::CAnimTex::Create(m_pGraphicDev, 12, 4)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ShroomManAnimTex", Engine::CAnimTex::Create(m_pGraphicDev, 9, 5)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ShamanAnimTex", Engine::CAnimTex::Create(m_pGraphicDev, 8, 9)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ShamProjectileAnimTex", Engine::CAnimTex::Create(m_pGraphicDev, 6, 1)), E_FAIL);

    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIScreenIconTex", Engine::CAnimTex::Create(m_pGraphicDev, 11, 1)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UISilhouettes", Engine::CAnimTex::Create(m_pGraphicDev, 5, 2)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BuffIcons", Engine::CAnimTex::Create(m_pGraphicDev, 14, 4)), E_FAIL);


    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RangeTex", Engine::CRangeTex::Create(m_pGraphicDev)), E_FAIL);

    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ShadowTex", Engine::CShadowTex::Create(m_pGraphicDev)), E_FAIL);


    // Object Tex
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ObjectTex", Engine::CObjectTex::Create(m_pGraphicDev, 0.5f, 0.5f, 0.0f)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CoreTex", Engine::CObjectTex::Create(m_pGraphicDev, 3.f, 3.f, 0.0f)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_StatueTex", Engine::CObjectTex::Create(m_pGraphicDev, 2.f, 2.f, 0.0f)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CoreBaseTex", Engine::CObjectTex::Create(m_pGraphicDev, 19.f, 0.0f, 11.f)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SkeletonTex", Engine::CObjectTex::Create(m_pGraphicDev, 0.5f, 0.0f, 0.5f)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MiniPlayerTex", Engine::CObjectTex::Create(m_pGraphicDev, 0.5f, 0.0f, 0.5f)), E_FAIL);


    // 파티클

    _vec3 vPPos = { 0.f, 0.f, 0.f };
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Firework", Engine::CFirework::Create(m_pGraphicDev, &vPPos, 600)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Fall", Engine::CFall::Create(m_pGraphicDev, &vPPos, 8)), E_FAIL);


    lstrcpy(m_szLoading, L"Texture Loading...");

    // TEXTURE
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTex", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Player/Miner_skin.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTex_Eye", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Player/Miner_eyes.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTex_Shirt", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Player/Miner_shirt.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTex_Pants", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Player/Miner_pants.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTex_Hair", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Player/Miner_hair1.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTex_HairShade", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Player/Miner_hairShade1.png", TEX_NORMAL)), E_FAIL);

    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SlimeTex", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/Slime.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ShroomManTex", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/Mushroom.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ShamanTex", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/Shaman.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ShamProjectileTex", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/fireballChargedProjectile_idle.png", TEX_NORMAL)), E_FAIL);

    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TerrainTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/MapTerrain/Terrain_%d.png", TEX_NORMAL, 18)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SkyBox", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/SkyBox/burger%d.dds", TEX_CUBE, 4)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_DarkBox", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/SkyBox/DarkSky.dds", TEX_CUBE, 1)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_WallCube", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Wall/Wall_%d.dds", TEX_CUBE, 3)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_DarkWallCube", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/DarkWall/Brick_Cube_%d.dds", TEX_CUBE, 15)), E_FAIL);

    //MapTool TEXTURE
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BasicTileTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Tile/BasicTile/BasicTile_%d.png", TEX_NORMAL, 11)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_GrassTileTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Tile/GrassTile/Grass_Tile_%d.png", TEX_NORMAL, 9)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_DustTileTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Tile/DustTile/Dust_Tile_%d.png", TEX_NORMAL, 9)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MapToolTerrainTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/MapTerrain/Terrain_%d.png", TEX_NORMAL, 18)), E_FAIL);

#pragma region Item TEXTURE

    //아이템 종류별로 텍스쳐 저장하게 할건데 일단 임시
    // 도구
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PickaxeTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Tool/Pickaxe_%d.png", TEX_NORMAL, 3)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_HoeTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Tool/Hoe_%d.png", TEX_NORMAL, 3)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_WateringCanTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Tool/WateringCan.png", TEX_NORMAL)), E_FAIL);

    // 무기
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SwordTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Weapon/Sword_%d.png", TEX_NORMAL, 3)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BowTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Weapon/Bow_%d.png", TEX_NORMAL, 3)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ArrowTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Weapon/Arrow_%d.png", TEX_NORMAL, 3)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_StaffTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Weapon/Suncaller_staff.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MagicTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Weapon/Magic.png", TEX_NORMAL)), E_FAIL);

    // 장비
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_HelmetTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Equip/Helm_%d.png", TEX_NORMAL, 4)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ChestTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Equip/Chest_%d.png", TEX_NORMAL, 4)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LegTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Equip/Pants_%d.png", TEX_NORMAL, 4)), E_FAIL);

    // 아직 이미지가 없음
    // 보조장비
    //FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_NecklaceTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Equip/Pants_%d.png", TEX_NORMAL, 4)), E_FAIL);
    //FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RingTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Equip/Pants_%d.png", TEX_NORMAL, 4)), E_FAIL);
    //FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_AssistanceTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Equip/Pants_%d.png", TEX_NORMAL, 4)), E_FAIL);

    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BagTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Bag/Bag_%d.png", TEX_NORMAL, 2)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LanternTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Lantern/Lantern_%d.png", TEX_NORMAL, 2)), E_FAIL);

    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TableTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Table/Table_%d.png", TEX_NORMAL, 3)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_AnvilTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Anvil/Anvil_%d.png", TEX_NORMAL, 3)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_FurnaceTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Furnace/Furnace_%d.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CookingPotTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/CookingPot/CookingPot_%d.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_WallPieceTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/WallPiece/WallPiece_%d.png", TEX_NORMAL, 4)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TorchTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Torch.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BoxTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Box.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_GravestoneTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Gravestone.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SprinklerTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Sprinkler.png", TEX_NORMAL)), E_FAIL);

    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_OreTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Ore/Ore_%d.png", TEX_NORMAL, 3)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BarTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Bar/Bar_%d.png", TEX_NORMAL, 3)), E_FAIL);
    //FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PotionTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Sprinkler.png", TEX_NORMAL)), E_FAIL);
    //FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_FoodTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Sprinkler.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SeedTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Seed/Seed_%d.png", TEX_NORMAL, 3)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_StatueCoreTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Core/Core_%d.png", TEX_NORMAL, 3)), E_FAIL);
    //FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SpawnerTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Sprinkler.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_WoodTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Wood.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MucusTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Mucus.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PieceTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Piece.png", TEX_NORMAL)), E_FAIL);

    // SHADOW TEXTURE
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ShadowTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Shadow/shadow%d.png", TEX_NORMAL, 3)), E_FAIL);

    // OBJECT TEXTURE
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CoreTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/BaseCamp/Core.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_StatueTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/BaseCamp/Statue_%d.png", TEX_NORMAL, 3)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CoreBaseTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/BaseCamp/BaseCamp.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SkeletonTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/BaseCamp/Skeleton.png", TEX_NORMAL)), E_FAIL);

    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SpawnerTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/Spawner/Spawner_%d.png", TEX_NORMAL, 3)), E_FAIL);

#pragma endregion

#pragma region UI TEXTURE

    //UI TEXTURE
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UITex", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_%d.png", TEX_NORMAL, 2)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIIconTex", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_Icon/UI_Icon_%d.png", TEX_NORMAL, 7)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIColTex", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_Collapse.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIHealthTex", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_Health_%d.png", TEX_NORMAL, 9)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIScreenInvTex", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_HandInv_%d.png", TEX_NORMAL, 2)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UINumber", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_Font_%d.png", TEX_NORMAL, 10)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIItemNumber", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_Item_Font_%d.png", TEX_NORMAL, 10)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UICursor", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_Cursor.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIInvSelected", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_Selected_0.png", TEX_NORMAL, 2)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIBoard", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_Board.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIStatusBoard", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Character_Status_Window.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UISlot", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Status_Slot.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIPlayerStat", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Player_Stat_Select.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UICraft", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_Craft_%d.png", TEX_NORMAL, 2)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UICraftTitle", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Craft_Title_%d.png", TEX_NORMAL, 4)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UISilhouette", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/equipment_slot_silhouettes.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIStatusSelect", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_Status_Select.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UITrashCan", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_TrashCan_%d.png", TEX_NORMAL, 5)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UITrashSlot", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_TrashSlot.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UISort", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_Align_%d.png", TEX_NORMAL, 2)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIStatusFrame", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_StatusFrame_%d.png", TEX_NORMAL, 2)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIItemFrame", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/128x128_itemui_border_0.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIBuffSlot", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Buff_Slot_%d.png", TEX_NORMAL, 2)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_Buff", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/buff_debuff_icons.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MapFrame", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Map/minimapframe.png", TEX_NORMAL)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UITable", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Table/Table_%d.png", TEX_NORMAL, 3)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIStatue", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_Statue_Craft_%d.png", TEX_NORMAL, 4)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIChestSort", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Chest_Sort_%d.png", TEX_NORMAL, 2)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIPlayerCraftItem", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Craft_Item/Craft_Item_%d.png", TEX_NORMAL, 14)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIFurnace", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Furnace_Craft/UI_Furnace_%d.png", TEX_NORMAL, 3)), E_FAIL);

    // Mini Ui Texture
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MiniPlayer", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/MiniUI/MiniPlayer.png", TEX_NORMAL)), E_FAIL);
 
#pragma endregion

    lstrcpy(m_szLoading, L"Collider Loading...");

#pragma region Collider

    //COLLIDER
    //반지름 넣으면 됩니다. 콜라이더 RENDER 기능 아직 안됨 ㅠ
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerCollider", Engine::CCollider::Create(m_pGraphicDev, 0.6f)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ItemCollider", Engine::CCollider::Create(m_pGraphicDev, 0.4f)), E_FAIL);
    
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SlimeCollider", Engine::CCollider::Create(m_pGraphicDev, 0.5f)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ShroomManCollider", Engine::CCollider::Create(m_pGraphicDev, 0.7f)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ShamanCollider", Engine::CCollider::Create(m_pGraphicDev, 0.6f)), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ShamProjectileCollider", Engine::CCollider::Create(m_pGraphicDev, 0.7f)), E_FAIL);

    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SwordCollider", Engine::CCollider::Create(m_pGraphicDev, 0.4f)), E_FAIL);

    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_NormalCubeCollider", Engine::CColliderCube::Create(m_pGraphicDev, _vec3(-0.5f, -0.5f, -0.5f), _vec3(0.5f, 0.5f, 0.5f))), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerCubeCollider", Engine::CColliderCube::Create(m_pGraphicDev, _vec3(-0.5f, -0.5f, -0.5f), _vec3(0.5f, 0.5f, 0.5f))), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SwingCubeCollider", Engine::CColliderCube::Create(m_pGraphicDev, _vec3(-0.5f, -0.5f, -0.5f), _vec3(0.5f, 0.8f, 0.5f))), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ShootCubeCollider", Engine::CColliderCube::Create(m_pGraphicDev, _vec3(-1.6f, -1.6f, -0.5f), _vec3(1.6f, 1.6f, 0.5f))), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ItemCubeCollider", Engine::CColliderCube::Create(m_pGraphicDev, _vec3(-0.2f, -0.2f, -0.2f), _vec3(0.2f, 0.2f, 0.2f))), E_FAIL);
    FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_WallCollider", Engine::CColliderCube::Create(m_pGraphicDev, _vec3(-0.5f, -0.5f, -0.5f), _vec3(0.5f, 0.5f, 0.5f))), E_FAIL);

#pragma endregion

    lstrcpy(m_szLoading, L"PRESS ENTER");

    m_bFinish = true;

    return 0;
}

unsigned int CLoading::Thread_Main(void* pArg)
{
    CLoading* pLoading = reinterpret_cast<CLoading*>(pArg);

    _uint   iFlag(0);

    EnterCriticalSection(pLoading->Get_Crt());

    switch (pLoading->Get_LoadingID())
    {
    case LOADING_STAGE:
        iFlag = pLoading->Loading_Stage();
        break;

    case LOADING_BOSS:
        break;
    }

    LeaveCriticalSection(pLoading->Get_Crt());

    return iFlag;
}

CLoading* CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eID)
{
    CLoading* pLoading = new CLoading(pGraphicDev);

    if (FAILED(pLoading->Ready_Loading(eID)))
    {
        Safe_Release(pLoading);
        return nullptr;
    }

    return pLoading;
}

void CLoading::Free()
{
    WaitForSingleObject(m_hThread, INFINITE);
    CloseHandle(m_hThread);
    DeleteCriticalSection(&m_Crt);

    Safe_Release(m_pGraphicDev);
}
