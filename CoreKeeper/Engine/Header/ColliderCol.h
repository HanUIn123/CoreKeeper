#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CColliderCol : public CVIBuffer
{
private:
	explicit CColliderCol();
	explicit CColliderCol(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CColliderCol(const CColliderCol& rhs);
	virtual ~CColliderCol();

public:
	virtual		HRESULT			Ready_Buffer(float _fRadius);
	virtual		void			Render_Buffer();

public:
	static CColliderCol*			Create(LPDIRECT3DDEVICE9 pGraphicDev, float _fRadius);
	CComponent*				Clone();

private:
	virtual void	Free();

private:
	float m_fRadius;
};

END