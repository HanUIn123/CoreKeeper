#pragma once
#include "PSystem.h"

BEGIN(Engine)

class ENGINE_DLL CHeal : public PSystem
{
protected:
	explicit CHeal();
	explicit CHeal(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CHeal(const CHeal& rhs);
	virtual ~CHeal();

public:
	virtual     HRESULT         Ready_Particles(D3DXVECTOR3* origin, _int numParticles, _bool _bStatic);
	virtual		void			resetParticle(Attribute* attribute);
	virtual		void			update(float timeDelta, _vec3 vDir = { 0.f, 0.f, 0.f });
	
	virtual     void            preRender();
	virtual     void            postRender();
private:
	_bool       m_bStatic;

public:
	static		CHeal* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3* origin, _int numParticles, _bool _bStatic = false);
	virtual		CComponent* Clone();

public:
	virtual void		Free();
};

END