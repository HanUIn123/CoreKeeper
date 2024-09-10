#pragma once
#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CAnimTex;
class CTransform;
class CTexture;
//class CCalculator;
//class CAnimator;

END

class CUIStatus : public Engine::CGameObject
{
private:
	explicit CUIStatus(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUIStatus();

public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();

private:
	_vec3 vUIPos;
	_matrix m_UIMatrix;

private:
	Engine::CAnimTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	//Engine::CCalculator* m_pCalculatorCom;
	//Engine::CAnimator* m_pAnimatorCom;

public:
	static CUIStatus* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};
