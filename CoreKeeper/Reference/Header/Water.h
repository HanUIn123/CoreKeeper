#pragma once
#include "PSystem.h"

BEGIN(Engine)

class ENGINE_DLL CWater : public PSystem
{
protected:
	explicit CWater();
	explicit CWater(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CWater(const CWater& rhs);
	virtual ~CWater();

public:
	virtual     HRESULT         Ready_Particles(D3DXVECTOR3* origin, _int numParticles);
	virtual		void			resetParticle(Attribute* attribute);
	virtual		void			update(float timeDelta, _vec3 vDir = { 0.f, 0.f, 0.f });
	
	virtual     void            preRender();
	virtual     void            postRender();

	virtual     void            reset();

	void                        resetParticles(Attribute* attribute);

	virtual     void            AddParticles();

public:
	static		CWater* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3* origin, _int numParticles);
	virtual		CComponent* Clone();

public:
	virtual void		Free();

private:
	_float fTime;
	_bool m_bFirst;
	_int m_iParticleNum;
	_int m_iAliveNum;
};

END