#pragma once
#include "Scene.h"
#include "Engine_Define.h"
#include "../Client/Header/ImguiMgr.h"
#include "Engine_Define.h"

#include "MapToolTerrain.h"

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


	void									InClude_GameObject();

public:
	static	CMapEditorScene* Create(LPDIRECT3DDEVICE9 _pGraphicDeivce);

private:
	virtual void							Free();

public:
	// ImGui 기초 세팅 함수
	void									Show_ImguiWindow();
	void									Setting_Menu();
	void									Setting_TileList();

	// Tile 이미지 등록.
	HRESULT									Resister_TileImage_ImGui(LPDIRECT3DDEVICE9 _pGraphicDeivce, const _tchar* _ImageFilePath, TEXTUREID _eTextureId, const int& _iImageNumber);

	// 일단 만들어 둠. (아직은 안씀)
	void									Set_Texture(const _uint& iIndex = 0);

private:
	vector<IDirect3DBaseTexture9*>			m_vecTexture;
	LPDIRECT3DTEXTURE9						m_TextureInfo = NULL;
	D3DXIMAGE_INFO							m_tImageInfo;


	// ImGui 창 위 마우스 존재 판단 bool변수
	bool									m_bGuiHovered;

private:
	Engine::CGameObject* m_pMTGameObjectCom;
};

