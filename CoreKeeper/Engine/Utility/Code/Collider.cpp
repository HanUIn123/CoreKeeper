#include "Export_Utility.h"

CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphicDev)
    : CComponent(pGraphicDev), m_fRadius(1.f), m_vCenterPos(0.f, 0.f, 0.f)
{
}

CCollider::~CCollider()
{
}

HRESULT CCollider::Ready_Collider(float fRadius)
{
    m_fRadius = fRadius;

    return S_OK;
}

void CCollider::Update_Collider(const _matrix* pWorldMatrix)
{
    m_matWorld = *pWorldMatrix;
    m_vCenterPos = _vec3(m_matWorld._41, m_matWorld._42, m_matWorld._43);
}

bool CCollider::Check_Collision(CCollider* pTarget)
{
    if (pTarget == nullptr)
        return false;

    // 두 콜라이더의 중심 간의 거리 계산
    _vec3 vDistance = pTarget->m_vCenterPos - m_vCenterPos;
    float fDistance = D3DXVec3Length(&vDistance);

    // 거리가 두 콜라이더의 반지름 합보다 작으면 충돌
    return fDistance < (m_fRadius + pTarget->m_fRadius);
}

bool CCollider::Check_Cube_Collision(CColliderCube* pCube)
{
    if (pCube == nullptr)
        return false;

    _vec3 vCubeMin = pCube->Get_Min() + pCube->Get_CenterPos();
    _vec3 vCubeMax = pCube->Get_Max() + pCube->Get_CenterPos();

    // 가장 가까운 점 찾기
    _vec3 vClosestPoint;
    vClosestPoint.x = max(vCubeMin.x, min(m_vCenterPos.x, vCubeMax.x));
    vClosestPoint.y = max(vCubeMin.y, min(m_vCenterPos.y, vCubeMax.y));
    vClosestPoint.z = max(vCubeMin.z, min(m_vCenterPos.z, vCubeMax.z));

    // 가장 가까운 점과 구의 중심 사이의 거리
    _vec3 vDifference = vClosestPoint - m_vCenterPos;
    float fDistance = D3DXVec3Length(&vDifference);

    return fDistance <= m_fRadius;
}

void CCollider::Render_Collider()
{
#ifdef _DEBUG
    m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld);

    LPD3DXMESH pSphereMesh = nullptr;
    D3DXCreateSphere(m_pGraphicDev, m_fRadius, 20, 20, &pSphereMesh, NULL);

    m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    pSphereMesh->DrawSubset(0);
    m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

    Safe_Release(pSphereMesh);
#endif
}

CCollider* CCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev, float fRadius)
{
    CCollider* pInstance = new CCollider(pGraphicDev);

    if (FAILED(pInstance->Ready_Collider(fRadius)))
    {
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

CCollider* CCollider::Clone()
{
    return new CCollider(*this);
}

void CCollider::Free()
{
    CComponent::Free();
}
