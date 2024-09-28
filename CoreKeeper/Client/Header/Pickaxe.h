#pragma once
#include "Item.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
class CCollider;
class CShadowTex;

END

class CPickaxe : public CItem
{
private:
	explicit CPickaxe(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPickaxe();

public:
	virtual			HRESULT			Ready_GameObject(MATERIAL _eMaterial, _vec3 vPos);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();

public:
	static CPickaxe* Create(LPDIRECT3DDEVICE9 pGraphicDev, MATERIAL _eMaterial = MATERIAL_END, _vec3 vPos = { 0, 0, 0 });

private:
	virtual void		Free();
};
