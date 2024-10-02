#pragma once
#include "Scene.h"
#include "Engine_Define.h"
#include "../Client/Header/ImguiMgr.h"
#include "Engine_Define.h"

#include "MapToolTerrain.h"

#include "Include.h"


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

	void									Setting_ObjectList();
	HRESULT									Piking_Object();

	void									Setting_MonsterList();
	HRESULT									Piking_Monster();


	HRESULT									Delete_Object(const _tchar* pLayerTag, const _tchar* pGameObjectTag);

	// ImGui에 Tile 미리보기 이미지 등록.
	HRESULT									Resister_ImguiImage_ImGui(LPDIRECT3DDEVICE9 _pGraphicDeivce, const _tchar* _ImageFilePath, IMGUITEXTUREID _eTextureId, const int& _iImageNumber);
private:
	_int									m_iStandardIndex;
	_int									m_iUpIndex;
	_int									m_iDownIndex;
	_int									m_iRightIndex;
	_int									m_iLeftIndex;

private:
	vector<IDirect3DBaseTexture9*>			m_vecTileTexture;
	vector<IDirect3DBaseTexture9*>			m_vecWallTexture;
	vector<IDirect3DBaseTexture9*>			m_vecObjectTexture;
	vector<IDirect3DBaseTexture9*>			m_vecMonsterTexture;

	//LPDIRECT3DTEXTURE9						m_TileTextureInfo = NULL;
	LPDIRECT3DTEXTURE9						m_TextureInfo = NULL;
	D3DXIMAGE_INFO							m_tImageInfo;


	// ImGui 창 위 마우스 존재 판단 bool변수
	bool									m_bGuiHovered;
	bool									m_bSwitch;
	bool									m_bReachable;
	wstring									m_iImGuiTileListNum;

	// n번 째 타일인지 담는 변수.
	_int									m_iTileNumber;

	// n번 째 벽인지 담는 변수.
	_int									m_iWallImgNumber;

	// n번 째 건설물인지 담는 변수.
	_int									m_iBuildingNumber;

	// n번 째 몬스터인지 담는 변수.
	_int									m_iMonsterNumber;
public:
	void									MapFile_Save();
	HRESULT									MapFile_Load();
private:
	Engine::CGameObject*					m_pMTGameObjectCom;
	Engine::CGameObject*					m_pWallCom;
	Engine::CGameObject*					m_pObjectCom;

	int										m_iTileCreateCount;
	int										m_iWallCreateCount;
	int										m_iBuildCreateCount;
	bool									m_bPushed;
	bool									m_bWallClickPushed;
	bool									m_bBuildingClick;
	bool									m_bSelectTile;
	bool									m_bSelectWall;
	bool									m_bSelectBuilding;
	bool									m_bSelectMonster;
	bool									m_bCanInstall;
	bool									m_bAlreadyInstalled;
	bool									m_bReposed;

	// Picking 가능한 타일 키 값 개수. 현재는 VTXCNTX * VTXCNTZ개
	wstring									m_wsTileNameString[VTXCNTX * VTXCNTZ];
	wstring									m_wsWallNameString[VTXCNTX * VTXCNTZ];
	wstring									m_wsObjectNameString[VTXCNTX * VTXCNTZ];

	HANDLE									m_hFile;
	HANDLE									m_hWallFile;
	HANDLE									m_hObjectFile;

	vector<CWall*>							m_vecWallObject;
	//vector<CBuilding*>						m_vecBuildingObject;

	list<CCore*>							m_listBuildingObject;

	_vec3									m_vCheckPos;
	_vec3                                   m_vPickPos;

	_vec3									m_vObPickPos;

	_int									m_iLoadTileCount;
	_int									m_iLoadWallCount;
	_int									m_iPikingIndex;


};

