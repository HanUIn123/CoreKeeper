#pragma once
#include "GameObject.h"
#include "../Header/Monster.h"
#include "Define.h"

BEGIN(Engine)

class CRcTex;
class CRangeTex;
class CTransform;
class CTexture;
class CAnimator;

END

class CUIBossBar : public Engine::CGameObject
{
private:
	explicit CUIBossBar(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUIBossBar();

public:
	virtual			HRESULT			Ready_GameObject(_vec2 vPos, _vec2 vSize, const _uint iIndex);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

public:
	void            Set_InfoH(_int _iCurHp, _int _iMaxHp) { m_iMaxHp = _iMaxHp, m_iHp = _iCurHp; }
	_int            Get_MaxHp() { return m_iMaxHp; }
	_int            Get_CurHp() { return m_iHp; }

	_int            Get_MaxMp() { return m_iMaxHp; }
	_int            Get_CurMp() { return m_iHp; }

	void            Set_Bar(_matrix matWorld, CMonster* pMonster);

	void            Set_Pos(_matrix matWorld);

	void            Set_Disable() { m_bWindow = false; }
private:
	HRESULT			Add_Component();

private:
	RECT m_BRect;
	_matrix ButtonWorld;
	_int  m_iIndex;

	_int m_iMaxHp, m_iHp;
	_int m_iPreHp, m_iCurHp;

	_bool m_bWindow;

	_float m_fLength;

	CMonster* m_pMonster;

	STAT m_sState;
private:
	Engine::CRangeTex* m_pBufferCom;
	Engine::CRangeTex* m_pBarBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CAnimator* m_pAnimatorCom;

public:
	static CUIBossBar* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize, const _uint iIndex);

private:
	virtual void		Free();
};
