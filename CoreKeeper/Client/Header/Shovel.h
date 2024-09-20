#pragma once
#include "Item.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
class CCollider;
class CShadowTex;

END

class CShovel : public CItem
{
private:
	explicit CShovel(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CShovel();

public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();

public:
	static CShovel* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};
