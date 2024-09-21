#pragma once
#include "Scene.h"

// 게임오브젝트 개수가 많아지면서 인클루드가 너무 길어져서
// 인클루드 헤더를 따로 만들었습니다.
#include "Include.h"


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
	HRESULT							Create_Inventory(const _tchar* pLayerTag, const _tchar* pGameObjectTag);
	HRESULT                         Create_Item(const _tchar* pLayerTag, CItem* pItem, const _tchar* pItemTag);

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
	wstring                         m_CraftSlot[10];
	_bool							m_bInvCheck;
	HANDLE							m_hFile;
	HANDLE							m_hWallFile;

	wstring							m_wsTileNameString[VTXCNTX * VTXCNTZ];
	wstring							m_wsWallNameString[VTXCNTX * VTXCNTZ];
	_int							m_iLoadTileCount;
	_int							m_iLoadWallCount;

	vector<CWall*>					m_vecWall;
};

