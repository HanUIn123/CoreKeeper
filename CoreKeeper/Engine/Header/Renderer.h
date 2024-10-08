#pragma once
#include "Base.h"

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer : public CBase
{
	DECLARE_SINGLETON(CRenderer)

private:
	explicit CRenderer();
	virtual ~CRenderer();

public:
	void			Add_RenderGroup(RENDERID eType, CGameObject* pGameObject);
	void			Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Clear_RenderGroup();
	void            Delete_Renderer(RENDERID _eType, CGameObject* pGameObject);
	void			Expand_MiniMap(LPDIRECT3DDEVICE9& pGraphicDev);
	_float			Get_ZoomRatio();
	_bool			Get_ExpandMap();

private:
	void			Render_Priority(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_NonAlpha(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_Alpha(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_Wall(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_UI(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_UIALPHA(LPDIRECT3DDEVICE9& pGraphicDev);
	void            Render_Window(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_MiniMap(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_WorldMap(LPDIRECT3DDEVICE9& pGraphicDev);
	void            Render_Subordinate(LPDIRECT3DDEVICE9& pGraphicDev);

private:
	list<CGameObject*>			m_RenderGroup[RENDER_END];

	_D3DVIEWPORT9 m_MainViewport;
	_D3DVIEWPORT9 m_MiniViewport;

private:
	_bool						m_bExpandMinimap;
	_float						m_fZoomRatio;
private:
	virtual void		Free();

};



END