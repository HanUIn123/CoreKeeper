#pragma once
#include "PSystem.h"

BEGIN(Engine)

class ENGINE_DLL CFirework : public PSystem
{
protected:
	explicit CFirework();
	explicit CFirework(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CFirework(const CFirework& rhs);
	virtual ~CFirework();

public:
	virtual     HRESULT         Ready_Particles(D3DXVECTOR3* origin, _int numParticles);
	virtual		void			resetParticle(Attribute* attribute);
	virtual		void			update(float timeDelta, _vec3 vDir = { 0.f, 0.f, 0.f });
	
	virtual     void            preRender();
	virtual     void            postRender();
public:
	static		CFirework* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3* origin, _int numParticles);
	virtual		CComponent* Clone();

public:
	virtual void		Free();
};

END