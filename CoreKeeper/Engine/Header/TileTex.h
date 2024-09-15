#pragma once
#include "VIBuffer.h"
BEGIN(Engine)

class ENGINE_DLL CTileTex : public CVIBuffer
{
private:
	explicit									CTileTex();
	explicit									CTileTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit									CTileTex(const CTileTex& rhs);
	virtual										~CTileTex();

public:
	virtual		HRESULT							Ready_Buffer(const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv);
	virtual void								Render_Buffer();

public:
	static		CTileTex* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _ulong& dwCntX = VTXCNTX, const _ulong& dwCntZ = VTXCNTZ, const _ulong& dwVtxItv = VTXITV);
	virtual		CComponent* Clone();

private:
	virtual void								Free();

};

END