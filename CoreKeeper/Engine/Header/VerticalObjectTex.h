#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVerticalObjectTex : public CVIBuffer
{
private:
	explicit									CVerticalObjectTex();
	explicit									CVerticalObjectTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit									CVerticalObjectTex(const CVerticalObjectTex& rhs);
	virtual										~CVerticalObjectTex();

public:
	virtual		HRESULT							Ready_Buffer(_float _fRadiusX, _float _fRadiusY, _float _fRadiusZ);
	virtual		void							Render_Buffer();

public:
	static		CVerticalObjectTex* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _fRadiusX, _float _fRadiusY, _float _fRadiusZ);
	virtual		CComponent* Clone();

private:
	virtual void								Free();
};

END