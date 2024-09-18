#pragma once
#include "Scene.h"
#include "BackGround.h"

#include "Player.h"
#include "Eye.h"
#include "Shirt.h"
#include "Pants.h"
#include "Hair.h"
#include "HairShade.h"
#include "Slime.h"
#include "Monster.h"
#include "Sword.h"
#include "Seed.h"

#include "Terrain.h"
#include "SkyBox.h"
#include "Tile.h"
#include "Wall.h"

#include "UIPlayerStatus.h"
#include "UIScreenIcon.h"
#include "UIStatusBar.h"
#include "UIScreenInv.h"
#include "UICursor.h"
#include "UIInvPlate.h"
#include "UIInventory.h"
#include "UIItemSlot.h"
#include "UIPlayerStats.h"

#include "Inventory.h"

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
	HRESULT							Create_Inventory(const _tchar* pLayerTag); 

private:
	HRESULT							Ready_LightInfo();
	HRESULT							Ready_Layer_Environment(const _tchar* pLayerTag);
	HRESULT							Ready_Layer_GameLogic(const _tchar* pLayerTag);
	HRESULT							Ready_Layer_UI(const _tchar* pLayerTag);

public:
	static	CStage*					Create(LPDIRECT3DDEVICE9 pGraphicDev);


	HRESULT							Load_MapFile();



private:
	virtual void					Free();

private:
	wstring							m_Invstring[50];
	wstring							m_ItemSlot[10];
	_bool							m_bInvCheck;
	HANDLE							m_hFile;
	HANDLE							m_hWallFile;

	wstring							m_wsTileNameString[VTXCNTX * VTXCNTZ];
	wstring							m_wsWallNameString[VTXCNTX * VTXCNTZ];
	_int							m_iLoadTileCount;
	_int							m_iLoadWallCount;
};

