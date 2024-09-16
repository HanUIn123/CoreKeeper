#pragma once
#include "Scene.h"
#include "Engine_Define.h"
#include "../Client/Header/ImguiMgr.h"
#include "Engine_Define.h"

#include "MapToolTerrain.h"
#include "Tile.h"

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
	static	CMapEditorScene* Create(LPDIRECT3DDEVICE9 _pGraphicDeivce);

private:
	virtual void							Free();

public:
	// ImGui 기초 세팅 함수
	void									Show_ImguiWindow();
	void									Setting_Menu();
	void									Setting_TerrainList();
	void									Setting_TileList();

	// Tile 이미지 등록.
	HRESULT									Resister_TerrainImage_ImGui(LPDIRECT3DDEVICE9 _pGraphicDeivce, const _tchar* _ImageFilePath, TEXTUREID _eTextureId, const int& _iImageNumber);
	HRESULT									Resister_TileImage_ImGui(LPDIRECT3DDEVICE9 _pGraphicDeivce, const _tchar* _ImageFilePath, TEXTUREID _eTextureId, const int& _iImageNumber);

	// 일단 만들어 둠. (아직은 안씀)
	void									Set_Texture(const _uint& iIndex = 0);

private:
	vector<IDirect3DBaseTexture9*>			m_vecTerrainTexture;
	vector<IDirect3DBaseTexture9*>			m_vecTileTexture;

	LPDIRECT3DTEXTURE9						m_TerrainTextureInfo = NULL;
	LPDIRECT3DTEXTURE9						m_TileTextureInfo = NULL;

	D3DXIMAGE_INFO							m_tImageInfo;


	// ImGui 창 위 마우스 존재 판단 bool변수
	bool									m_bGuiHovered;

public:
	// Tile Object는 이 함수를 통해 생성.(레이어 / 중복방지 Count -> ex Tile1, Tile2 .. 키값 변경 / 피킹한 MaptoolTerrain의 좌표)

private:
	Engine::CGameObject*					m_pMTGameObjectCom;
	Engine::CGameObject*					m_pTileCom;
	Engine::CGameObject*					m_pWallCom;

	int										m_iTileCreateCount;
	int										m_iPikingCount;
	bool									m_bPushed;

	// Picking 가능한 타일 키 값 개수. 현재는 360개
	wstring									m_wsTileNameString[VTXCNTX * VTXCNTZ];
	bool									m_bSaved;

	HANDLE									m_hFile;

	vector<Engine::CGameObject*>			m_vecTileObject;

	_vec3                                   m_vPickPos;
};

