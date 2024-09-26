#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CObjectTex : public CVIBuffer
{
private:
	explicit									CObjectTex();
	explicit									CObjectTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit									CObjectTex(const CObjectTex& rhs);
	virtual										~CObjectTex();

public:
	virtual		HRESULT							Ready_Buffer(_float _fRadiusX, _float _fRadiusY, _float _fRadiusZ);
	virtual		void							Render_Buffer();

public:
	static		CObjectTex* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _fRadiusX, _float _fRadiusY,_float _fRadiusZ);
	virtual		CComponent* Clone();

private:
	virtual void								Free();
};

END