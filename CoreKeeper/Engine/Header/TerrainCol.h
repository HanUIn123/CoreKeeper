#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CTerrainCol : public CVIBuffer
{
private:
	explicit CTerrainCol();
	explicit CTerrainCol(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTerrainCol(const CTerrainCol& rhs);
	virtual ~CTerrainCol();

public:
	virtual		HRESULT			Ready_Buffer();
	virtual		void			Render_Buffer();

public:
	static CTerrainCol* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	CComponent* Clone();

private:
	virtual void	Free();

};

END