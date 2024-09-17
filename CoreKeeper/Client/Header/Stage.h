#pragma once
#include "Scene.h"
#include "BackGround.h"

#include "Player.h"
#include "Slime.h"
#include "Monster.h"
#include "Sword.h"

#include "Terrain.h"
#include "SkyBox.h"
#include "Tile.h"

#include "UIStatus.h"
#include "UIScreenIcon.h"
#include "UIStatusBar.h"
#include "UIScreenInv.h"
#include "UICursor.h"
#include "UIInvPlate.h"

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
	_bool							m_bInvCheck;
	HANDLE							m_hFile;
	

	// Load 한 타일의 정보들을 담아줄 멤버 변수들.
	wstring							m_wsTileNameString[VTXCNTX * VTXCNTZ];
	_int							m_iLoadTileCount;
};

