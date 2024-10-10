#pragma once
#include "Object.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
class CCollider;
class CShadowTex;

END
class CAzeosSpawner : public CObject
{
private:
	explicit						CAzeosSpawner(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual							~CAzeosSpawner();

public:
	virtual			HRESULT			Ready_GameObject(_vec3 vPos);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

	virtual			void			Interaction();
	_bool							Check_Object_Interaction();

private:
	HRESULT							Add_Component();

public:
	static CAzeosSpawner*			Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos = { 0.f, 0.f, 0.f });

private:
	virtual void					Free();
	_bool							m_bIsAlreadySpawn;
};

