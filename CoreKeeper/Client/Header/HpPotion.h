#pragma once
#include "Item.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
class CCollider;
class CShadowTex;

END

class CHpPotion : public CItem
{
private:
	explicit CHpPotion(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CHpPotion();

public:
	virtual			HRESULT			Ready_GameObject(_vec3 vPos);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();

public:
	static CHpPotion* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos = { 0.f, 0.f, 0.f });

private:
	virtual void		Free();
};
