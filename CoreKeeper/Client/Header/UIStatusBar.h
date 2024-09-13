#pragma once
#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CRangeTex;
class CTransform;
class CTexture;
class CAnimator;

END

class CUIStatusBar : public Engine::CGameObject
{
private:
	explicit CUIStatusBar(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUIStatusBar();

public:
	virtual			HRESULT			Ready_GameObject(_vec2 vPos, _vec2 vSize, const _uint iIndex);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

public:
	_bool           Map_Picked(POINT _screenPos) {
		return  ::PtInRect(&m_BRect, _screenPos);
	}

	void            Set_Hp(_int _iMaxHp, _int _iCurHp);
	_int            Get_MaxHp() { return m_iMaxHp; }
	_int            Get_CurHp() { return m_iHp; }

	_int            Get_MaxMp() { return m_iMaxHp; }
	_int            Get_CurMp() { return m_iHp; }

private:
	HRESULT			Add_Component();



private:
	RECT m_BRect;
	_matrix ButtonWorld;
	_int  m_iIndex;

	_int m_iMaxHp, m_iHp;
	_int m_iPreHp, m_iCurHp;

	_bool m_bCollapse;

	_bool m_bExit;

	_float m_fLength;
private:
	Engine::CRangeTex* m_pBufferCom;
	Engine::CRangeTex* m_pBarBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CAnimator* m_pAnimatorCom;

public:
	static CUIStatusBar* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize, const _uint iIndex);

private:
	virtual void		Free();
};
