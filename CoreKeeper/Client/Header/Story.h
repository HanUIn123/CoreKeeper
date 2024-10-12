#pragma once
#include "Scene.h"
#include "StoryBackGround.h"
#include "Loading.h"
#include "WhitePlane.h"

class CStory : public Engine::CScene
{
private:
	explicit CStory(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStory();

public:
	virtual			HRESULT			Ready_Scene();
	virtual			_int			Update_Scene(const _float& fTimeDelta);
	virtual			void			LateUpdate_Scene();
	virtual			void			Render_Scene();

private:
	HRESULT		Ready_Prototype();

	HRESULT		Ready_Layer_Environment(const _tchar* pLayerTag);
	HRESULT		Ready_Layer_GameLogic(const _tchar* pLayerTag) { return S_OK; }
	HRESULT		Ready_Layer_UI(const _tchar* pLayerTag) { return S_OK; }

private:
	CLoading* m_pLoading;

public:
	static	CStory* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void		Free();
};

