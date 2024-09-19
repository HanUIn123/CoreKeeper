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
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerInventory", Engine::CInventory::Create(m_pGraphicDev, 30)), E_FAIL);

	lstrcpy(m_szLoading, L"Buffer Loading...");
	// BUFFER

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RcCol", Engine::CRcCol::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RcTex", Engine::CRcTex::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TerrainTex", Engine::CTerrainTex::Create(m_pGraphicDev, VTXCNTX, VTXCNTZ, VTXITV)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CubeTex", Engine::CCubeTex::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MapToolTex", Engine::CMapToolTex::Create(m_pGraphicDev, VTXCNTX, VTXCNTZ, VTXITV)), E_FAIL);
	// Wall 전용, WallTex
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_WallTex", Engine::CWallTex::Create(m_pGraphicDev)), E_FAIL);

	// Tile전용, TileTex 한 칸 씩만 출력.
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TileTex", Engine::CTileTex::Create(m_pGraphicDev, 2, 2, VTXITV)), E_FAIL);


	//이미지 개수 넣으면 됩니다 ex) Miner.png보면 가로 15개 세로 2개
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_AnimTex", Engine::CAnimTex::Create(m_pGraphicDev, 20, 2)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_NormalAnimTex", Engine::CAnimTex::Create(m_pGraphicDev, 1, 1)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SwordAnimTex", Engine::CAnimTex::Create(m_pGraphicDev, 3, 3)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SlimeAnimTex", Engine::CAnimTex::Create(m_pGraphicDev, 12, 4)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIScreenIconTex", Engine::CAnimTex::Create(m_pGraphicDev, 11, 1)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TileAnimTex", Engine::CTileTex::Create(m_pGraphicDev, 16,20)), E_FAIL);


	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIHealthTexRc", Engine::CRangeTex::Create(m_pGraphicDev)), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ShadowTex", Engine::CShadowTex::Create(m_pGraphicDev)), E_FAIL);





	lstrcpy(m_szLoading, L"Texture Loading...");

	// TEXTURE
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerTex", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Player/Miner.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SlimeTex", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Monster/Slime/usable/Slime.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TerrainTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Terrain/Terrain1.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SkyBox", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/SkyBox/burger%d.dds", TEX_CUBE, 4)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_WallCube", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Wall/Wall_2.dds", TEX_CUBE, 1)), E_FAIL);

	//MapTool TEXTURE
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BasicTileTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Tile/BasicTile/BasicTile_%d.png", TEX_NORMAL, 11)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MapToolTerrainTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/MapTerrain/Terrain_%d.png", TEX_NORMAL, 9)), E_FAIL);
	//FAILED_CHECK_RETURN(Engine:::Ready_Proto(L"Proto_WallTexture", Engine::CTexture::Create(m_pGraphicDev,L"")))


	//아이템 종류별로 텍스쳐 저장하게 할건데 일단 임시
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SeedTex", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/seed_%d.png", TEX_NORMAL, 2)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SwordTex", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Item/sword_legendary.png", TEX_NORMAL)), E_FAIL);
	
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ShadowTexture", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/Shadow/shadow%d.png", TEX_NORMAL, 3)), E_FAIL);

	//UI TEXTURE
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UITex", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_%d.png", TEX_NORMAL, 2)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIIconTex", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_Icon/UI_Icon_%d.png", TEX_NORMAL, 7)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIColTex", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_Collapse.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIHealthTex", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_Health_%d.png", TEX_NORMAL, 6)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIScreenInvTex", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_HandInv_%d.png", TEX_NORMAL, 2)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UINumber", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_Font_%d.png", TEX_NORMAL, 10)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIItemNumber", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_Item_Font_%d.png", TEX_NORMAL, 10)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UICursor", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_Cursor.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIInvSelected", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_Selected.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIBoard", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/UI_Board.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIStatusBoard", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Character_Status_Window.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UISlot", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Status_Slot.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIPetSlot", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Pet_Select.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIPlayerStat", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Player_Stat_Select.png", TEX_NORMAL)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UIPetStat", Engine::CTexture::Create(m_pGraphicDev, L"../Bin/Resource/Texture/UI/Pet_Select.png", TEX_NORMAL)), E_FAIL);
	
	//COLLIDER
	//반지름 넣으면 됩니다. 콜라이더 RENDER 기능 아직 안됨 ㅠ
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerCollider", Engine::CCollider::Create(m_pGraphicDev, 0.5f)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ItemCollider", Engine::CCollider::Create(m_pGraphicDev, 0.2f)), E_FAIL);

	lstrcpy(m_szLoading, L"PRESS ENTER");

	m_bFinish = true;

	return 0;
}

unsigned int CLoading::Thread_Main(void* pArg)
{
	CLoading* pLoading = reinterpret_cast<CLoading*>(pArg);

	_uint	iFlag(0);

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
