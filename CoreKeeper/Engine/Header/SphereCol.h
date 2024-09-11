#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CSphereCol : public CVIBuffer
{
private:
	explicit CSphereCol();
	explicit CSphereCol(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSphereCol(const CSphereCol& rhs);
	virtual ~CSphereCol();

public:
	virtual		HRESULT			Ready_Buffer();
	virtual		void			Render_Buffer();

public:
	static CSphereCol* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	CComponent* Clone();

private:
	virtual void	Free();

};

END

//아직 구 그리는거 공부중, 일단 RcCol 복붙해옴