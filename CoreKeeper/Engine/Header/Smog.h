#pragma once
#include "PSystem.h"

BEGIN(Engine)

class ENGINE_DLL CSmog : public PSystem
{
protected:
	explicit CSmog();
	explicit CSmog(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSmog(const CSmog& rhs);
	virtual ~CSmog();

public:
	virtual     HRESULT         Ready_Particles(D3DXVECTOR3* origin, _int numParticles);
	virtual		void			resetParticle(Attribute* attribute);
	virtual		void			update(_float timeDelta, _vec3 vDir = { 0.f, 0.f, 0.f });
	
	virtual     void            preRender();
	virtual     void            postRender();
public:
	static		CSmog* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3* origin, _int numParticles);
	virtual		CComponent* Clone();

public:
	virtual void		Free();
};

END