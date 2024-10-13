#pragma once
#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CTransform;

END

class CUIBossName : public Engine::CGameObject
{
private:
	explicit CUIBossName(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUIBossName();

public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

public:
	void Set_Font(_matrix matWorld, const _tchar* tFont);
	void Set_Disable() { m_bWindow = false; }
private:
	HRESULT			Add_Component();

private:
	_vec3 vUIPos;
	_matrix m_UIMatrix;

	_bool m_bWindow;

	_int m_iFontCount;

	const _tchar* m_tFont;

	_vec2 m_vPos;
private:
	Engine::CTransform* m_pTransformCom;

public:
	static CUIBossName* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};
