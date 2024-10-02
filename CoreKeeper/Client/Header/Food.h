#pragma once
#include "Item.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
class CCollider;
class CShadowTex;

END

class CFood : public CItem
{
private:
	explicit CFood(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFood();

public:
	virtual			HRESULT			Ready_GameObject(ITEMNUM _eIngredient1, ITEMNUM _eIngredient2, _vec3 vPos);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();

public:
	static CFood* Create(LPDIRECT3DDEVICE9 pGraphicDev, ITEMNUM _eIngredient1, ITEMNUM _eIngredient2, _vec3 vPos = { 0.f, 0.f, 0.f });

private:
	virtual void		Free();
};
