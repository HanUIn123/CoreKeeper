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

private:
	void			Render_Priority(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_NonAlpha(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_Alpha(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_UI(LPDIRECT3DDEVICE9& pGraphicDev);
	void			Render_UIALPHA(LPDIRECT3DDEVICE9& pGraphicDev);
	void            Render_Window(LPDIRECT3DDEVICE9& pGraphicDev);

private:
	list<CGameObject*>			m_RenderGroup[RENDER_END];

	_D3DVIEWPORT9 m_MainViewport;

private:
	virtual void		Free();

};



END