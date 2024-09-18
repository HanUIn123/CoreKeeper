#pragma once
#include "GameObject.h"
#include "Define.h"
#include "Export_System.h"

BEGIN(Engine)

class CAnimTex;
class CTransform;
class CTexture;
class CAnimator;

END

class CHair : public CGameObject
{
private:
	explicit CHair(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CHair();
public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();
	void			Follow_Player();

private:
	Engine::CAnimTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CAnimator* m_pAnimatorCom;

public:
	static CHair* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

