#pragma once
#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
//class CCalculator;
//class CAnimator;

END

class CUIStatusFrame : public Engine::CGameObject
{
private:
	explicit CUIStatusFrame(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUIStatusFrame();

public:
	virtual			HRESULT			Ready_GameObject(_vec2 vPos, _vec2 vSize);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

public:
	void            Set_Window() {
		if (m_bWindow)
			m_bWindow = false;
		else
			m_bWindow = true;
	}

	void           Set_WindowDis() {
		m_bWindow = false;
	}

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

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CTexture* m_pSelTextureCom;

public:
	static CUIStatusFrame* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize);

private:
	virtual void		Free();
};
