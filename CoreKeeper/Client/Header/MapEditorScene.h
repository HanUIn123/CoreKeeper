#pragma once
#include "Scene.h"
#include "../Client/Header/ImguiMgr.h"

class CMapEditorScene : public Engine::CScene
{
private:
	explicit CMapEditorScene(LPDIRECT3DDEVICE9 _pGraphicDevice);
	virtual ~CMapEditorScene();

public:
	virtual			HRESULT			Ready_Scene();
	virtual			_int			Update_Scene(const _float& fTimeDelta);
	virtual			void			LateUpdate_Scene();
	virtual			void			Render_Scene();

private:
	HRESULT		Ready_Layer_Environment(const _tchar* pLayerTag);
	HRESULT		Ready_Layer_GameLogic(const _tchar* pLayerTag);
	HRESULT		Ready_Layer_UI(const _tchar* pLayerTag);

public:
	static	CMapEditorScene* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void		Free();

private:
	void Show_ImguiWindow();
	void Setting_Menu();
	void Setting_TileList();



};

