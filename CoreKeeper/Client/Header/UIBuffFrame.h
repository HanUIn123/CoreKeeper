#pragma once
#include "GameObject.h"
#include "Define.h"
#include "Item.h"
#include "UICursor.h"
#include "UIBuff.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
//class CCalculator;
//class CAnimator;

END

class CUIBuffFrame : public Engine::CGameObject
{
private:
	explicit CUIBuffFrame(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUIBuffFrame();

public:
	virtual			HRESULT			Ready_GameObject(_vec2 vPos, _vec2 vSize);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

public:
	void            Set_Window(CUIBuff::BUFFICONTYPE _BuffType, POINT _pt, _bool _bCraft = false);
	void            Set_WindowDis() { m_bWindow = false; }

	_bool           Map_Picked(POINT _screenPos) {
		return  ::PtInRect(&m_BRect, _screenPos);
	}

private:
	void            Set_BuffExplain();

private:
	HRESULT			Add_Component();

private:
	_vec2 m_vPos;

	_bool m_bWindow;
	_bool m_bCollapse;

	RECT m_BRect;

	CUIBuff::BUFFICONTYPE m_eType;

	map<_int, wstring> m_mapType;

	_bool m_bCraft;

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CRcTex* m_pItemBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CTexture* m_pSelTextureCom;

public:
	static CUIBuffFrame* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize);

private:
	virtual void		Free();
};
