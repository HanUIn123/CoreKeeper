#include "Export_Utility.h"

CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphicDev)
    : CComponent(pGraphicDev), m_fRadius(0.f), m_vCenterPos(0.f, 0.f, 0.f)
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

//test, 렌더 안됨 ㅠㅠ
void CCollider::Render_Collider()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matWorld);
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
