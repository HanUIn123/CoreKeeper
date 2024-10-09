#pragma once
#include "GameObject.h"
#include "Define.h"
#include "Item.h"
#include "UICursor.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
//class CCalculator;
//class CAnimator;

END

class CUIItemFrame : public Engine::CGameObject
{
private:
	explicit CUIItemFrame(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUIItemFrame();

public:
	virtual			HRESULT			Ready_GameObject(_vec2 vPos, _vec2 vSize);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

public:
	void            Set_Window(CItem* _pItem, POINT _pt);
	void            Set_WindowDis() { m_bWindow = false; }

	_bool           Map_Picked(POINT _screenPos) {
		return  ::PtInRect(&m_BRect, _screenPos);
	}


private:
	HRESULT			Add_Component();

private:
	_vec2 m_vPos;

	_bool m_bWindow;
	_bool m_bCollapse;

	RECT m_BRect;

	CItem* m_pItem;

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CRcTex* m_pItemBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CTexture* m_pSelTextureCom;

public:
	static CUIItemFrame* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize);

private:
	virtual void		Free();
};
