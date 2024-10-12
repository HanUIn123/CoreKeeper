#pragma once
#include "PSystem.h"

BEGIN(Engine)

class ENGINE_DLL CTeleport : public PSystem
{
protected:
	explicit CTeleport();
	explicit CTeleport(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTeleport(const CTeleport& rhs);
	virtual ~CTeleport();

public:
	virtual     HRESULT         Ready_Particles(D3DXVECTOR3* origin, _int numParticles);
	virtual		void			resetParticle(Attribute* attribute);
	virtual		void			update(float timeDelta, _vec3 vDir = { 0.f, 0.f, 0.f });
	
	virtual     void            preRender();
	virtual     void            postRender();
public:
	static		CTeleport* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3* origin, _int numParticles);
	virtual		CComponent* Clone();

public:
	virtual void		Free();
};

END