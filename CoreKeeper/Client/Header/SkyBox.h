#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CCubeTex;
class CTransform;
class CTexture;

END

class CSkyBox : public Engine::CGameObject
{
private:
	explicit CSkyBox(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSkyBox();

public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();

private:
	Engine::CCubeTex*			m_pBufferCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CTexture*		m_pTextureCom;

public:
	static CSkyBox*			Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};
