#pragma once
#include "Component.h"

BEGIN(Engine)

class CCollider;

END

BEGIN(Engine)

class ENGINE_DLL CColliderCube : public CComponent
{
private:
    explicit CColliderCube(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CColliderCube();

public:
    HRESULT		Ready_Collider(const _vec3& vMin, const _vec3& vMax);
    void		Update_Collider(const _matrix* pWorldMatrix);
    bool		Check_Collision(CColliderCube* pTarget);
    bool		Check_Sphere_Collision(CCollider* pTarget);
    void		Render_Collider();

public:
    static CColliderCube* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vMin, const _vec3& vMax);
    virtual CComponent* Clone();

private:
    virtual void Free();

public:
    _vec3	Get_Min() { return m_vMin; }
    _vec3	Get_Max() { return m_vMax; }
    _vec3	Get_CenterPos() { return m_vCenterPos; }
    void	Set_Offset(const _vec3 vOffset) { m_vOffset = vOffset; }

private:
    _vec3 m_vMin;       // 큐브의 최소 좌표
    _vec3 m_vMax;       // 큐브의 최대 좌표
    _matrix m_matWorld; 
    _vec3 m_vCenterPos;
    _vec3 m_vOffset;			// 콜라이더 오프셋

    static bool	m_bShow;
    LPD3DXMESH	m_pCube;
};

END