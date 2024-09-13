#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CMapToolTex : public CVIBuffer
{
private:
	explicit CMapToolTex();
	explicit CMapToolTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMapToolTex(const CMapToolTex& rhs);
	virtual ~CMapToolTex();

public:
	const _vec3* Get_VtxPos() const { return m_pPos; }
public:
	virtual		HRESULT		Ready_Buffer(const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv);
	virtual		void		Render_Buffer();

private:
	_vec3* m_pPos;

public:
	static		CMapToolTex* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _ulong& dwCntX = VTXCNTX, const _ulong& dwCntZ = VTXCNTZ, const _ulong& dwVtxItv = VTXITV);
	virtual		CComponent* Clone();

private:
	virtual void		Free();
};

END
