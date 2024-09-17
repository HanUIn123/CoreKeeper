#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CAnimTex;
class CTransform;
class CTexture;
class CAnimator;
class CCalculator;

END

class CSlime : public Engine::CGameObject
{
	explicit CSlime(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSlime();

public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();

private:
	Engine::CAnimTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CAnimator* m_pAnimatorCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CCalculator* m_pCalculatorCom;

public:
	static CSlime* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

