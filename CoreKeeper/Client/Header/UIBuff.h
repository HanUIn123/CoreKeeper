#pragma once
#include "GameObject.h"
#include "Define.h"

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
	enum BUFFTYPE { BUFF_HEAL = 17, DEBUFF_BURN = 24, DEBUFF_HUNGER = 31, BUFF_END };
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
	void            Set_Window(BUFFTYPE _eType, BUFFSLOTTYPE _eSlotType, _float _fTime);

	_bool           Map_Picked(POINT _screenPos) {
		return  ::PtInRect(&m_BRect, _screenPos);
	}

	void            Set_BuffType(BUFFTYPE _eType)     { m_eBuffType = _eType; }
	void            Set_BuffSlot(BUFFSLOTTYPE _eType) { m_eBuffIndex = _eType; }
	void            Set_BuffTime(_float _fTime)       { m_fBuffTime = _fTime; }

private:
	HRESULT			Add_Component();

private:
	_vec2 m_vPos;
	_bool m_bWindow;
	_bool m_bCollapse;

	RECT m_BRect;
	RECT m_Rect;

	BUFFSLOTTYPE m_eBuffIndex;

	BUFFTYPE m_eBuffType;

	_float   m_fTime;
	_float   m_fBuffTime;
	_float   m_fCurTime;
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
