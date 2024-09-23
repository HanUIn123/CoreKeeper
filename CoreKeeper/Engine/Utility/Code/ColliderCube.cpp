#include "Export_Utility.h"
#include "Export_System.h"

bool CColliderCube::m_bShow = false;

CColliderCube::CColliderCube(LPDIRECT3DDEVICE9 pGraphicDev)
    : CComponent(pGraphicDev), m_vMin(-0.5f, -0.5f, -0.5f), m_vMax(0.5f, 0.5f, 0.5f), m_pCube(nullptr)
{
    m_vOffset = { 0, 0, 0 };
}

CColliderCube::~CColliderCube()
{
}

HRESULT CColliderCube::Ready_Collider(const _vec3& vMin, const _vec3& vMax)
{
    m_vMin = vMin;
    m_vMax = vMax;

    FAILED_CHECK_RETURN(D3DXCreateBox(m_pGraphicDev, vMax.x - vMin.x, vMax.y - vMin.y, vMax.z - vMin.z, &m_pCube, NULL), E_FAIL);

    return S_OK;
}

void CColliderCube::Update_Collider(const _matrix* pWorldMatrix)
{
    if (Key_Down(DIK_B))
    {
        m_bShow = !m_bShow;
    }

    m_matWorld = *pWorldMatrix;
    m_vCenterPos = _vec3(m_matWorld._41, m_matWorld._42, m_matWorld._43) + m_vOffset;
}

bool CColliderCube::Check_Collision(CColliderCube* pTarget)
{
    if (pTarget == nullptr)
        return false;

    // 나의 최소, 최대 좌표
    _vec3 vMinWorld = m_vMin + m_vCenterPos;
    _vec3 vMaxWorld = m_vMax + m_vCenterPos;

    // 타겟의 최소, 최대 좌표
    _vec3 vMinTarget = pTarget->m_vMin + pTarget->m_vCenterPos;
    _vec3 vMaxTarget = pTarget->m_vMax + pTarget->m_vCenterPos;

    // AABB 방식으로 충돌 검사 (X, Y, Z 축 모두 겹치는지 확인)
    if (vMaxWorld.x < vMinTarget.x || vMinWorld.x > vMaxTarget.x)
        return false;
    if (vMaxWorld.y < vMinTarget.y || vMinWorld.y > vMaxTarget.y)
        return false;
    if (vMaxWorld.z < vMinTarget.z || vMinWorld.z > vMaxTarget.z)
        return false;

    return true; // 모든 축에서 겹치면 충돌
}

bool CColliderCube::Check_Sphere_Collision(CCollider* pSphere)
{
    if (pSphere == nullptr)
        return false;

    _vec3 vSphereCenter = pSphere->Get_CenterPos();
    float fSphereRadius = pSphere->Get_Radius();

    _vec3 vMinWorld = m_vMin + m_vCenterPos;
    _vec3 vMaxWorld = m_vMax + m_vCenterPos;

    // 가장 가까운 점 찾기
    _vec3 vClosestPoint;
    vClosestPoint.x = max(vMinWorld.x, min(vSphereCenter.x, vMaxWorld.x));
    vClosestPoint.y = max(vMinWorld.y, min(vSphereCenter.y, vMaxWorld.y));
    vClosestPoint.z = max(vMinWorld.z, min(vSphereCenter.z, vMaxWorld.z));

    // 가장 가까운 점과 구의 중심 사이의 거리
    _vec3 vDifference = vClosestPoint - vSphereCenter;
    float fDistance = D3DXVec3Length(&vDifference);

    return fDistance <= fSphereRadius;
}

void CColliderCube::Render_Collider()
{
    if (!m_bShow)
        return;

    //FAILED_CHECK_RETURN(Setup_Material(), );

    m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld);

    DWORD preRenderState, preTextureStageState;

    m_pGraphicDev->GetRenderState(D3DRS_FILLMODE, &preRenderState);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &preTextureStageState);

    m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_ADD);

    m_pCube->DrawSubset(0);

    // 돌려놓음
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, preTextureStageState);
    m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, preRenderState);
}

CColliderCube* CColliderCube::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vMin, const _vec3& vMax)
{
    CColliderCube* pInstance = new CColliderCube(pGraphicDev);

    if (FAILED(pInstance->Ready_Collider(vMin, vMax)))
    {
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

CColliderCube* CColliderCube::Clone()
{
    return new CColliderCube(*this);
}

void CColliderCube::Free()
{
    CComponent::Free();
}
