#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;

END

class CShadow : public Engine::CGameObject
{
private:
	explicit CShadow(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CShadow();

public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();
	void			Apply_Billboard();

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

public:
	static CShadow* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();

private:
	int		m_iTextureNumber;
};
