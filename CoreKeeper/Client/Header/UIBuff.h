#pragma once
#include "GameObject.h"
#include "Define.h"

class CUIBuffFrame;

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
class CAnimTex;
class CRangeTex;

END

class CUIBuff : public Engine::CGameObject
{
public:
	enum BUFFICONTYPE 
	{
		BUFF_ICON_HP = 0,
		BUFF_ICON_ATT = 1,
		BUFF_ICON_MINING = 6,
		BUFF_ICON_DEF = 15,
		BUFF_ICON_FULL = 17,
		DEBUFF_ICON_FIRE = 24,
		BUFF_ICON_SPEED = 29,
		DEBUFF_ICON_SLOW = 30,
		DEBUFF_ICON_HUNGER = 31,
		DEBUFF_ICON_STUN = 38,
		BUFF_ICON_IMMUNE = 40,
		BUFF_ICON_END
	 };
	enum BUFFSLOTTYPE { BUFF, DEBUFF};

private:
	explicit CUIBuff(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUIBuff();

public:
	virtual			HRESULT			Ready_GameObject(_vec2 vPos, _vec2 vSize);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

public:
	void            Set_Window(BUFFTYPE _eType);

	_bool           Map_Picked(POINT _screenPos) { return  ::PtInRect(&m_BRect, _screenPos); }

	void            Set_BuffTime(_float _fCurTime, _float _fBuffTime) { m_fCurTime = _fCurTime; m_fBuffTime = _fBuffTime; }

	void			Set_Position(_vec2 vPos)
	{
		_D3DVIEWPORT9 Viewport;	m_pGraphicDev->GetViewport(&Viewport);
		m_vPos.x = vPos.x - (_float)Viewport.Width * 0.5f; m_vPos.y = (_float)Viewport.Height * 0.5f - vPos.y;

		m_BRect.left = vPos.x - 20.f;
		m_BRect.right = vPos.x + 20.f;
		m_BRect.top = vPos.y - 20.f;
		m_BRect.bottom = vPos.y + 20.f;

		m_Rect = m_BRect;
	}


	void			Set_Allocate(_bool bAlloc) { m_bAllocated = bAlloc; }
	_bool			Get_Allocate() { return m_bAllocated; }

private:
	HRESULT			Add_Component();

private:
	_bool	m_bAllocated;

	_vec2 m_vPos;
	_bool m_bWindow;
	static _bool m_bCollapse[BUFF_ICON_END];

	RECT m_BRect;
	RECT m_Rect;

	BUFFICONTYPE m_eBuffType;
	BUFFSLOTTYPE m_eBuffIndex;

	_float   m_fBuffTime;
	_float   m_fCurTime;

	CUIBuffFrame* m_pFrame;

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CRangeTex* m_pRangeBufferCom;
	Engine::CAnimTex* m_pAnimBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CTexture* m_pBuffTextureCom;

public:
	static CUIBuff* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize);

private:
	virtual void		Free();
};
