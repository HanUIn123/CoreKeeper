#include "Export_Utility.h"
#include "Export_System.h"

bool CCollider::m_bShow = false;

CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphicDev)
    : CComponent(pGraphicDev), m_fRadius(1.f), m_vCenterPos(0.f, 0.f, 0.f), m_pSphere(nullptr)
{
}

CCollider::~CCollider()
{
}

HRESULT CCollider::Ready_Collider(float fRadius)
{
    m_fRadius = fRadius;
    m_vOffset = { 0, 0, 0 };

    FAILED_CHECK_RETURN(D3DXCreateSphere(m_pGraphicDev, m_fRadius, 10, 10, &m_pSphere, NULL), E_FAIL);

    return S_OK;
}

void CCollider::Update_Collider(const _matrix* pWorldMatrix)
{
    if (Key_Down(DIK_V))
    {
        m_bShow = !m_bShow;
    }

    m_matWorld = *pWorldMatrix;
    m_matWorld._41 += m_vOffset.x;
    m_matWorld._42 += m_vOffset.y;
    m_matWorld._43 += m_vOffset.z;
    m_vCenterPos = _vec3(m_matWorld._41, m_matWorld._42, m_matWorld._43);
}

void CCollider::Render_Collider()
{
    if (!m_bShow)
        return;

    //FAILED_CHECK_RETURN(Setup_Material(), );


    // 원래 월드 행렬에서 위치는 유지하고, 스케일 요소는 제거
    D3DXMATRIX matNoScale = m_matWorld;
    matNoScale._11 = matNoScale._22 = matNoScale._33 = 1.0f; // 스케일을 1로 고정

    m_matWorld._41 -= m_vOffset.x;
    m_matWorld._42 -= m_vOffset.y;
    m_matWorld._43 -= m_vOffset.z;

    m_pGraphicDev->SetTransform(D3DTS_WORLD, &matNoScale);

    DWORD preRenderState, preTextureStageState;

    m_pGraphicDev->GetRenderState(D3DRS_FILLMODE, &preRenderState);
    m_pGraphicDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &preTextureStageState);

    m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_ADD);

    m_pSphere->DrawSubset(0);

    // 돌려놓음
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, preTextureStageState);
    m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, preRenderState);
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

HRESULT CCollider::Setup_Material()
{
    D3DMATERIAL9		tMtrl;
    ZeroMemory(&tMtrl, sizeof(D3DMATERIAL9));

    tMtrl.Diffuse = { 1.f, 1.f, 1.f, 1.f };
    tMtrl.Specular = { 1.f, 1.f, 1.f, 1.f };
    tMtrl.Ambient = { 0.2f, 0.2f, 0.2f, 1.f };

    tMtrl.Emissive = { 0.f, 1.f, 0.f, 1.f };
    tMtrl.Power = 0.f;

    m_pGraphicDev->SetMaterial(&tMtrl);

    return S_OK;
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

CComponent* CCollider::Clone()
{
    return new CCollider(*this);
}

void CCollider::Free()
{
    CComponent::Free();
}
