#pragma once
#include "Scene.h"

// 게임오브젝트 개수가 많아지면서 인클루드가 너무 길어져서
// 인클루드 헤더를 따로 만들었습니다.
#include "Include.h"
#include "CraftMgr.h"
#include "FarmMgr.h"
#include "BuffMgr.h"

#include "MapToolTerrain.h"
#include "../Header/MiniMapCamera.h"

class CStage : public Engine::CScene
{
private:
	explicit CStage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStage();

public:
	virtual			HRESULT			Ready_Scene();
	virtual			_int			Update_Scene(const _float& fTimeDelta);
	virtual			void			LateUpdate_Scene();
	virtual			void			Render_Scene();

public:
	wstring*						Get_WallNameByIndex(_int iIndex) { return &m_wsWallNameString[iIndex]; }
	vector<CWall*>&					Get_WallVector() { return m_vecWall; }
	void							Set_WallVectorByIndex(_int iIndex, CWall* pWall) { m_vecWall[iIndex] = pWall; }
	void							Set_WallUnreachableByIndex(_int iIndex) { m_vecUnreachable[iIndex] = true; }
private:
	HRESULT							Ready_LightInfo();
	HRESULT							Ready_Layer_Environment(const _tchar* pLayerTag);
	HRESULT							Ready_Layer_GameLogic(const _tchar* pLayerTag);
	HRESULT							Ready_Layer_UI(const _tchar* pLayerTag);

public:
	static	CStage*					Create(LPDIRECT3DDEVICE9 pGraphicDev);


	HRESULT							Load_MapFile();
	HRESULT							Load_MonsterData();
	HRESULT							Load_ObjectData();

	wstring*						Get_BuffName() { return m_wsBuffNameString; }
	void							Set_WallDropItemName(wstring wStr) { m_wsWallDropItemNameString.push_back(wStr); }
	vector<wstring>*				Get_WallDropItemName() { return &m_wsWallDropItemNameString; }

	void							Set_BGMNumber(int _iBgmNumber) { m_iBgmNumber = _iBgmNumber; }
private:
	virtual void					Free();
	Engine::CGameObject*			m_pTerrainObject;
private:
	wstring							m_Invstring[50];
	wstring                         m_ChestInvstring[50];
	wstring                         m_GraveInvstring[30];
	wstring							m_ItemSlot[10];
	wstring                         m_CraftLSlot[6];
	wstring                         m_CraftRSlot[6];
	_bool							m_bInvCheck;

	HANDLE							m_hFile;
	HANDLE							m_hWallFile;
	HANDLE							m_hObjectFile;
	HANDLE							m_hBigWallFile;

	wstring							m_wsTileNameString[(VTXCNTX - 1) * (VTXCNTZ - 1)];
	wstring							m_wsWallNameString[(VTXCNTX - 1) * (VTXCNTZ - 1)];
	wstring							m_wsObjectNameString[(VTXCNTX - 1) * (VTXCNTZ - 1)];
	wstring							m_wsMonsterNameString[(VTXCNTX - 1) * (VTXCNTZ - 1)];
	vector<wstring>					m_wsWallDropItemNameString;
	wstring							m_wsBuffNameString[BUFFTYPE_END];
	_int							m_iLoadTileCount;
	_int							m_iLoadWallCount;

	vector<CWall*>					m_vecWall;
	vector<CCore*>					m_vecCore;
	vector<bool>					m_vecUnreachable;

	vector<CMiniSpawn*>				m_vecMiniSpawn;


	// 혹시 몰라, 일단 멤버로 빼서 만들어 둠.
	CMiniMapCamera*					m_pMiniMapCamera;

	int								m_iBgmNumber;
};

