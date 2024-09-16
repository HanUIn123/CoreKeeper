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

class CUIInvPlate : public Engine::CGameObject
{
private:
	explicit CUIInvPlate(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUIInvPlate();

public:
	virtual			HRESULT			Ready_GameObject(_vec2 vPos, _vec2 vSize);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();

private:
	_vec3 vUIPos;
	_matrix m_UIMatrix;

	_bool m_bRender;
private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

public:
	void Set_Render() { 
		if (m_bRender)
			m_bRender = false;
		else
			m_bRender = true; }

public:
	static CUIInvPlate* Create(LPDIRECT3DDEVICE9 pGraphicDe, _vec2 vPos, _vec2 vSize);

private:
	virtual void		Free();
};
