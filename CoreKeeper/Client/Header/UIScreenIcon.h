#pragma once
#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
class CAnimator;

END

class CUIScreenIcon : public Engine::CGameObject
{
public:

	enum ICONTYPE {ICON_HAND, ICON_HAND_COL, ICON_MAP, ICON_MAP_COL, ICON_BAG, ICON_BAG_COL, ICON_EXIT };
private:
	explicit CUIScreenIcon(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUIScreenIcon();

public:
	virtual			HRESULT			Ready_GameObject(_vec2 vPos, _vec2 vSize, const _uint iIndex);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

public:
	_bool           Map_Picked(POINT _screenPos) {
		return  ::PtInRect(&m_BRect, _screenPos);
	}

	_bool           Get_Exit() { return m_bExit; }
	void            Set_Exit() { m_bExit = true; } // 인벤토리 열렸을때 호출할 함수
private:
	HRESULT			Add_Component();



private:
	RECT m_BRect;
	_matrix ButtonWorld;
	_int  m_iIndex;

	_bool m_bCollapse;
	_bool m_bClicked;

	_bool m_bExit;
private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

public:
	static CUIScreenIcon* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize, const _uint iIndex);

private:
	virtual void		Free();
};
