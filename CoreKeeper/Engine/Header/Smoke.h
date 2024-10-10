#pragma once
#include "PSystem.h"

BEGIN(Engine)

class ENGINE_DLL CSmoke : public PSystem
{
protected:
	explicit CSmoke();
	explicit CSmoke(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSmoke(const CSmoke& rhs);
	virtual ~CSmoke();

public:
	virtual     HRESULT         Ready_Particles(D3DXVECTOR3* origin, _int numParticles, _int _iMaxTexture, _float fY, _bool bAlpha);
	virtual		void			resetParticle(Attribute* attribute);
	virtual		void			update(float timeDelta, _vec3 vDir = { 0.f, 0.f, 0.f });
	
	virtual     void            preRender();
	virtual     void            postRender();
public:
	static		CSmoke* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3* origin, _int numParticles, _int _iMaxTexture = 0, _float fY = -0.8f, _bool bAlpha = false);
	virtual		CComponent* Clone();

public:
	virtual void		Free();

private:
	_bool m_bAlpha;
};

END