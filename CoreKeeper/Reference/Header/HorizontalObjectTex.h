#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CHorizontalObjectTex : public CVIBuffer
{
private:
	explicit									CHorizontalObjectTex();
	explicit									CHorizontalObjectTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit									CHorizontalObjectTex(const CHorizontalObjectTex& rhs);
	virtual										~CHorizontalObjectTex();

public:
	virtual		HRESULT							Ready_Buffer(_float _fRadiusX, _float _fRadiusZ);
	virtual		void							Render_Buffer();

public:
	static		CHorizontalObjectTex* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _fRadiusX, _float _fRadiusZ);
	virtual		CComponent* Clone();

private:
	virtual void								Free();
};

END