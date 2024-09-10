#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CShadowTex : public CVIBuffer
{
private:
	explicit CShadowTex();
	explicit CShadowTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CShadowTex(const CShadowTex& rhs);
	virtual ~CShadowTex();

public:
	virtual		HRESULT		Ready_Buffer();
	virtual		void		Render_Buffer();

public:
	static		CShadowTex*			Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual		CComponent*		Clone();

private:
	virtual void		Free();
};

END