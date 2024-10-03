#pragma once
#include "PSystem.h"

BEGIN(Engine)

class ENGINE_DLL CHit : public PSystem
{
protected:
	explicit CHit();
	explicit CHit(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CHit(const CHit& rhs);
	virtual ~CHit();

public:
	virtual     HRESULT         Ready_Particles(D3DXVECTOR3* origin, _int numParticles);
	virtual		void			resetParticle(Attribute* attribute);
	virtual		void			update(float timeDelta);
	
	virtual     void            preRender();
	virtual     void            postRender();
public:
	static		CHit* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3* origin, _int numParticles);
	virtual		CComponent* Clone();

public:
	virtual void		Free();

private:
	_float  m_fTime;
};

END