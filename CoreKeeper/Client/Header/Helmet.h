#pragma once
#include "Item.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
class CCollider;
class CShadowTex;

END

class CHelmet : public CItem
{
private:
	explicit CHelmet(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CHelmet();

public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();

public:
	static CHelmet* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};
