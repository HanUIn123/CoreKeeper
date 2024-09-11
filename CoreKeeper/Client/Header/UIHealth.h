#pragma once
#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CAnimTex;
class CTransform;
class CTexture;
class CAnimator;

END

class CUIHealth : public Engine::CGameObject
{
private:
	explicit CUIHealth(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUIHealth();

public:
	virtual			HRESULT			Ready_GameObject(_vec2 vPos, _vec2 vSize, const _uint iIndex);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

public:
	_bool           Map_Picked(POINT _screenPos) {
		return  ::PtInRect(&m_BRect, _screenPos);
	}

	/*
	_bool           Get_Exit() { return m_bExit; }
	void            Set_Exit() { m_bExit = true; } // 인벤토리 열렸을때 호출할 함수
	*/
private:
	HRESULT			Add_Component();



private:
	RECT m_BRect;
	_matrix ButtonWorld;
	_int  m_iIndex;

	_bool m_bCollapse;

	_bool m_bExit;
private:
	Engine::CAnimTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CTexture* m_pColTextureCom;
	Engine::CAnimator* m_pAnimatorCom;

public:
	static CUIHealth* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize, const _uint iIndex);

private:
	virtual void		Free();
};
