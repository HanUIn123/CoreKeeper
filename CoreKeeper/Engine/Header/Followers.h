#pragma once
#include "PSystem.h"

BEGIN(Engine)

class ENGINE_DLL CFollow : public PSystem
{
protected:
	explicit CFollow();
	explicit CFollow(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CFollow(const CFollow& rhs);
	virtual ~CFollow();

public:
	virtual     HRESULT         Ready_Particles(D3DXVECTOR3* origin, _int numParticles);
	virtual		void			resetParticle(Attribute* attribute);
	virtual		void			update(float timeDelta, _vec3 vDir = { 0.f, 0.f, 0.f });
	
	virtual     void            preRender();
	virtual     void            postRender();
public:
	static		CFollow* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3* origin, _int numParticles);
	virtual		CComponent* Clone();

private:
	_float      _fTime;
public:
	virtual void		Free();
};

END