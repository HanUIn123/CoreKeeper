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
	virtual			HRESULT			Ready_GameObject(_vec3 vPos, MATERIAL _eMaterial);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();

public:
	static CAnvil* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos = { 0.f, 0.f, 0.f }, MATERIAL _eMaterial = MATERIAL_END);

private:
	virtual void		Free();
};
