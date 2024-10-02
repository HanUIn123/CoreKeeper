#pragma once
#include "Item.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
class CCollider;
class CShadowTex;

END

class CSeed : public CItem
{
private:
	explicit CSeed(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSeed();

public:
	virtual			HRESULT			Ready_GameObject(ITEMNUM _eItemNum, _vec3 vPos);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();

public:
	static CSeed* Create(LPDIRECT3DDEVICE9 pGraphicDev, ITEMNUM _eItemNum, _vec3 vPos = { 0.f, 0.f, 0.f });

private:
	virtual void		Free();
};
