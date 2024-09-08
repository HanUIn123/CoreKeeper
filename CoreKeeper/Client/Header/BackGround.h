#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CAnimTex;
class CTexture;
class CAnimator;

END

class CBackGround : public Engine::CGameObject
{
private:
	explicit CBackGround(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBackGround();

public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();

private:
	Engine::CAnimTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CAnimator* m_pAnimatorCom;

public:
	static CBackGround* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};