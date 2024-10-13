#include "pch.h"
#include "../Header/AzeosShadow.h"
#include "../Header/Player.h"
#include "Export_System.h"
#include "Export_Utility.h"

CAzeosShadow::CAzeosShadow(LPDIRECT3DDEVICE9 _pGraphicDev)
    : CObject(_pGraphicDev)
    , m_iShadowTextureNumber(0)
    , m_bInFrustum(false)
    , m_bShow(false)
    , m_bChangePattern(false)
    , m_vMoveDirection(0, 0, 0)
    , m_fSpeed(15.0f)
    , m_fRotationAngle(0.0f)
    , m_fCoolTime(0.0f)
    , m_fAppearTime(0.0f)
    , m_eShadowDirection(SHADOW_RIGHT)
{
}

CAzeosShadow::~CAzeosShadow()
{
}

HRESULT CAzeosShadow::Ready_GameObject(_vec3 _vPos)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_vShadowPos = _vPos;

    m_pTransformCom->Set_Pos(_vPos.x, _vPos.y, _vPos.z);

    m_pTransformCom->Set_Scale(1.0f, 1.0f, 1.0f);

    return S_OK;
}

_int CAzeosShadow::Update_GameObject(const _float& fTimeDelta)
{
    if (m_fAppearTime >= 300)
        m_bShow = true;

    if (g_bFight)
        return 0;

    if (m_bShow)
    {
        if (!m_bChangePattern)
        {
            switch (m_eShadowDirection)
            {
                // 오른쪽 이동.
            case SHADOW_RIGHT:
                m_vMoveDirection.x = m_fSpeed;
                m_vShadowPos.x += m_vMoveDirection.x * fTimeDelta;
                m_fRotationAngle = D3DXToRadian(90.0f);
                break;

                // 왼쪽 이동
            case SHADOW_LEFT:
                m_vMoveDirection.x = -m_fSpeed;
                m_vShadowPos.x += m_vMoveDirection.x * fTimeDelta;
                m_fRotationAngle = D3DXToRadian(-90.0f);
                break;

                // 위쪽 이동
            case SHADOW_TOP:
                m_vMoveDirection.z = m_fSpeed;
                m_vShadowPos.z += m_vMoveDirection.z * fTimeDelta;
                m_fRotationAngle = D3DXToRadian(0.0f);
                break;

                // 아래쪽 이동
            case SHADOW_BOTTOM:
                m_vMoveDirection.z = -m_fSpeed;
                m_vShadowPos.z += m_vMoveDirection.z * fTimeDelta;
                m_fRotationAngle = D3DXToRadian(180.0f);
                break;

                // 오른쪽 위 이동
            case SHADOW_RIGHT_TOP:
                m_vMoveDirection.x = m_fSpeed;
                m_vMoveDirection.z = m_fSpeed;
                m_vShadowPos.x += m_vMoveDirection.x * fTimeDelta;
                m_vShadowPos.z += m_vMoveDirection.z * fTimeDelta;
                m_fRotationAngle = D3DXToRadian(45.0f);
                break;

                // 오른쪽 아래 이동
            case SHADOW_RIGHT_BOTTOM:
                m_vMoveDirection.x = m_fSpeed;
                m_vMoveDirection.z = -m_fSpeed;
                m_vShadowPos.x += m_vMoveDirection.x * fTimeDelta;
                m_vShadowPos.z += m_vMoveDirection.z * fTimeDelta;
                m_fRotationAngle = D3DXToRadian(135.0f);
                break;

                // 왼쪽 위 이동
            case SHADOW_LEFT_TOP:
                m_vMoveDirection.x = -m_fSpeed;
                m_vMoveDirection.z = m_fSpeed;
                m_vShadowPos.x += m_vMoveDirection.x * fTimeDelta;
                m_vShadowPos.z += m_vMoveDirection.z * fTimeDelta;
                m_fRotationAngle = D3DXToRadian(-45.0f);
                break;

                // 왼쪽 아래 이동
            case SHADOW_LEFT_BOTTOM:
                m_vMoveDirection.x = -m_fSpeed;
                m_vMoveDirection.z = -m_fSpeed;
                m_vShadowPos.x += m_vMoveDirection.x * fTimeDelta;
                m_vShadowPos.z += m_vMoveDirection.z * fTimeDelta;
                m_fRotationAngle = D3DXToRadian(-135.0f);
                break;
            }

            m_pTransformCom->Set_Angle(0.0f, m_fRotationAngle, 0.0f);
            m_pTransformCom->Set_Pos(m_vShadowPos.x, 0.1f, m_vShadowPos.z);

            m_fCoolTime++;

            if (m_fCoolTime >= 600.f)
            {
                m_fCoolTime = 0.0f;
                m_bChangePattern = true;
            }
        }

        Set_Direction();
    }


    int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

    Add_RenderGroup(RENDER_ALPHA, this);

    return iExit;
}

void CAzeosShadow::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CAzeosShadow::Render_GameObject()
{
    if (m_bShow)
    {
        m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

        m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

        m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

        m_pTextureCom->Set_Texture(m_iShadowTextureNumber);

        m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
        m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

        m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(100, 255, 255, 255));
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
        m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

        m_pBufferCom->Render_Buffer();

        m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);

        m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

        m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

        m_pColliderCom->Render_Collider();

        //m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

        m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
        /*
        
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(100, 255, 255, 255));
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);*/
    }
}



void CAzeosShadow::Set_Direction()
{
    if (m_bChangePattern)
    {
        m_eShadowDirection = (AZEOSSHADOW_DIRECTION(rand() % SHADOWDIRECTION_END));
        
        switch (m_eShadowDirection)
        {
        case Engine::SHADOW_RIGHT:
            m_vShadowPos.x = 36.0f;
            m_vShadowPos.y = 0.2f;
            m_vShadowPos.z = 106.0f;
            break;
        case Engine::SHADOW_LEFT:
            m_vShadowPos.x = 54.0f;
            m_vShadowPos.y = 0.2f;
            m_vShadowPos.z = 106.0f;
            break;
        case Engine::SHADOW_TOP:
            m_vShadowPos.x = 45.0f;
            m_vShadowPos.y = 0.2f;
            m_vShadowPos.z = 98.0f;
            break;
        case Engine::SHADOW_BOTTOM:
            m_vShadowPos.x = 45.0f;
            m_vShadowPos.y = 0.2f;
            m_vShadowPos.z = 114.0f;
            break;
        case Engine::SHADOW_RIGHT_TOP:
            m_vShadowPos.x = 36.0f;
            m_vShadowPos.y = 0.2f;
            m_vShadowPos.z = 98.0f;
            break;
        case Engine::SHADOW_RIGHT_BOTTOM:
            m_vShadowPos.x = 36.0f;
            m_vShadowPos.y = 0.2f;
            m_vShadowPos.z = 114.0f;
            break;
        case Engine::SHADOW_LEFT_TOP:
            m_vShadowPos.x = 54.0f;
            m_vShadowPos.y = 0.2f;
            m_vShadowPos.z = 98.0f;
            break;
        case Engine::SHADOW_LEFT_BOTTOM:
            m_vShadowPos.x = 54.0f;
            m_vShadowPos.y = 0.2f;
            m_vShadowPos.z = 114.0f;
            break;
        }

        m_bChangePattern = false;
    }
}

HRESULT CAzeosShadow::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pBufferCom = dynamic_cast<CObjectTex*>(Engine::Clone_Proto(L"Proto_AzeosShadowTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_AzeosShadowTexture"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

    pComponent = m_pColliderCom = dynamic_cast<CColliderCube*>(Engine::Clone_Proto(L"Proto_AzeosShadowCollider"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    return S_OK;
}

CAzeosShadow* CAzeosShadow::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos)
{
    CAzeosShadow* pAzeosShadow = new CAzeosShadow(pGraphicDev);

    if (FAILED(pAzeosShadow->Ready_GameObject(_vPos)))
    {
        Safe_Release(pAzeosShadow);
        MSG_BOX("pAzeosShadow Create Failed");
        return nullptr;
    }

    return pAzeosShadow;
}

void CAzeosShadow::Free()
{
    Engine::CGameObject::Free();
}
