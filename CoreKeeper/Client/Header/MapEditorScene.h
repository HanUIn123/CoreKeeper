#pragma once
#include "Scene.h"
#include "Engine_Define.h"
#include "../Client/Header/ImguiMgr.h"
#include "Engine_Define.h"

#include "MapToolTerrain.h"
#include "Tile.h"
#include "Wall.h"

BEGIN(Engine)

class CTexture;

END

class CMapEditorScene : public Engine::CScene
{
private:
	explicit								CMapEditorScene(LPDIRECT3DDEVICE9 _pGraphicDevice);
	virtual									~CMapEditorScene();

public:
	virtual			HRESULT					Ready_Scene();
	virtual			_int					Update_Scene(const _float& fTimeDelta);
	virtual			void					LateUpdate_Scene();
	virtual			void					Render_Scene();

private:
	HRESULT									Ready_LightInfo();
	HRESULT									Ready_Layer_Environment(const _tchar* pLayerTag);
	HRESULT									Ready_Layer_GameLogic(const _tchar* pLayerTag);
	HRESULT									Ready_Layer_UI(const _tchar* pLayerTag);


public:
	static	CMapEditorScene*				Create(LPDIRECT3DDEVICE9 _pGraphicDeivce);

private:
	virtual void							Free();

public:
	// ImGui 기초 세팅 함수
	void									Show_ImguiWindow();
	void									Setting_Menu();

	void									Setting_TileList();
	void									Piking_Tile();

	void									Setting_WallList();
	HRESULT									Piking_Wall();

	// Tile 이미지 등록.
	HRESULT									Resister_TileImage_ImGui(LPDIRECT3DDEVICE9 _pGraphicDeivce, const _tchar* _ImageFilePath, TEXTUREID _eTextureId, const int& _iImageNumber);

private:
	vector<IDirect3DBaseTexture9*>			m_vecTileTexture;
	vector<IDirect3DBaseTexture9*>			m_vecWallTexture;

	LPDIRECT3DTEXTURE9						m_TileTextureInfo = NULL;

	D3DXIMAGE_INFO							m_tImageInfo;


	// ImGui 창 위 마우스 존재 판단 bool변수
	bool									m_bGuiHovered;
	bool									m_bSwitch;

	// n번 째 타일인지 담는 변수.
	_int									m_iImageNumber;
public:
	void									MapFile_Save();
	HRESULT									MapFile_Load();
private:
	Engine::CGameObject*					m_pMTGameObjectCom;
	Engine::CGameObject*					m_pTileCom;
	Engine::CGameObject*					m_pWallCom;

	int										m_iTileCreateCount;
	int										m_iWallCreateCount;
	bool									m_bPushed;
	bool									m_bWallClickPushed;
	bool									m_bSelectTile;
	bool									m_bSelectWall;
	bool									m_bCanInstall;

	// Picking 가능한 타일 키 값 개수. 현재는 VTXCNTX * VTXCNTZ개
	wstring									m_wsTileNameString[VTXCNTX * VTXCNTZ];
	wstring									m_wsWallNameString[VTXCNTX * VTXCNTZ];

	HANDLE									m_hFile;
	HANDLE									m_hWallFile;

	vector<CTile*>							m_vecTileObject;
	vector<CWall*>							m_vecWallObject;

	_vec3									m_vCheckPos;
	_vec3                                   m_vPickPos;

	_int									m_iLoadTileCount;
	_int									m_iLoadWallCount;
};

