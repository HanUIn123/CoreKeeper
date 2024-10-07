#pragma once
#include "PSystem.h"

BEGIN(Engine)

class ENGINE_DLL CFall : public PSystem
{
protected:
	explicit CFall();
	explicit CFall(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CFall(const CFall& rhs);
	virtual ~CFall();

public:
	virtual     HRESULT         Ready_Particles(D3DXVECTOR3* origin, _int numParticles, _float fY);
	virtual		void			resetParticle(Attribute* attribute);
	virtual		void			update(float timeDelta, _vec3 vDir = { 0.f, 0.f, 0.f });
	
	virtual     void            preRender();
	virtual     void            postRender();

	virtual     void            reset();
private:
	_float fTime;
	_float m_fY;

public:
	static		CFall* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3* origin, _int numParticles, _float fY = 0.05f);
	virtual		CComponent* Clone();

public:
	virtual void		Free();
};

END