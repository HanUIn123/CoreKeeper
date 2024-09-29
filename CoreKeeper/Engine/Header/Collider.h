#pragma once
#include "Component.h"

BEGIN(Engine)

class CColliderCube;

END

BEGIN(Engine)

class ENGINE_DLL CCollider :	public CComponent
{
private:
	explicit CCollider(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCollider();

public:
	HRESULT		Ready_Collider(float fRadius);
	void		Update_Collider(const _matrix* pWorldMatrix);
	void		Render_Collider();

public:
	static CCollider*		Create(LPDIRECT3DDEVICE9 pGraphicDev, float fRadius);
	virtual CComponent*		Clone();

private:
	virtual void Free();

public:
	bool		Check_Collision(CCollider* pTarget);
	bool		Check_Cube_Collision(CColliderCube* pCube);

private:
	HRESULT		Setup_Material();

public:
	_vec3	Get_CenterPos() { return m_vCenterPos; }
	void	Set_Offset(const _vec3 vOffset) { m_vOffset = vOffset; }
	float	Get_Radius() { return m_fRadius; }

private:
	float m_fRadius;            // 충돌 반지름
	_vec3 m_vCenterPos;			// 콜라이더 중심 좌표
	_vec3 m_vOffset;			// 콜라이더 오프셋
	_matrix m_matWorld;			

	static bool	m_bShow;
	LPD3DXMESH	m_pSphere;
};

END