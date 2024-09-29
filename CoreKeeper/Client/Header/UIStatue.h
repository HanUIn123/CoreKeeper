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

class CUIStatue : public Engine::CGameObject
{
public:
	enum STATUE_TYPE {STATUE_GHORM, STATUE_GLURCH, STATUE_MALUGAZ, STATUE_MALUGAZ_EMPTY };

private:
	explicit CUIStatue(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUIStatue();

public:
	virtual			HRESULT			Ready_GameObject(_vec2 vPos, _vec2 vSize);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

public:
	void            Set_Window(STATUE_TYPE eType);
	STATUE_TYPE     Get_StatueType() { return m_eType; }
	void            Set_eType(STATUE_TYPE eType) { m_eType == eType; }

private:
	HRESULT			Add_Component();

private:
	_vec2 m_vPos;
	_bool m_bWindow;

	RECT m_bRect;

	STATUE_TYPE m_eType;
private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CTexture* m_pColTextureCom;

public:
	static CUIStatue* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize);

private:
	virtual void		Free();
};
