#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider :	public CComponent
{
private:
	explicit CCollider(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCollider();

public:
	HRESULT		Ready_Collider(float fRadius);
	void		Update_Collider(const _matrix* pWorldMatrix);
	bool		Check_Collision(CCollider* pTarget);	
	void		Render_Collider();

public:
	static CCollider*		Create(LPDIRECT3DDEVICE9 pGraphicDev, float fRadius);
	virtual CCollider*		Clone();

private:
	virtual void Free();

public:
	_vec3	Get_CenterPos() { return m_vCenterPos; }
	float	Get_Radius() { return m_fRadius; }

private:
	float m_fRadius;            // 충돌 반지름
	_vec3 m_vCenterPos;			// 콜라이더 중심 좌표
	_matrix m_matWorld;			
};

END