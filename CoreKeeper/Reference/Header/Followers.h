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
	virtual     HRESULT         Ready_Particles(D3DXVECTOR3* origin, _int numParticles, _int _iMaxTexture);
	virtual		void			resetParticle(Attribute* attribute);
	virtual		void			update(float timeDelta);
	
	virtual     void            preRender();
	virtual     void            postRender();
public:
	static		CFollow* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3* origin, _int numParticles, _int iMaxTexture);
	virtual		CComponent* Clone();

public:
	virtual void		Free();
};

END