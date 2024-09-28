#pragma once
#include "Item.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
class CCollider;
class CShadowTex;

END

class CAnvil : public CItem
{
private:
	explicit CAnvil(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CAnvil();

public:
	virtual			HRESULT			Ready_GameObject(MATERIAL _eMaterial, _vec3 vPos);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();

public:
	static CAnvil* Create(LPDIRECT3DDEVICE9 pGraphicDev,  MATERIAL _eMaterial = MATERIAL_END, _vec3 vPos = { 0.f, 0.f, 0.f });

private:
	virtual void		Free();
};
