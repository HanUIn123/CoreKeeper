#include "pch.h"
#include "../Header/BlackPlaneMgr.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "../Header/BlackPlane.h"

IMPLEMENT_SINGLETON(CBlackPlaneMgr)

CBlackPlaneMgr::CBlackPlaneMgr()
    : m_pGraphicDev(nullptr)
    , m_pBlackPlane(nullptr)
    , m_bIsFadingIn(false)
    , m_bIsFadingOut(false)
    , m_bFadeOver(false)
{
}

CBlackPlaneMgr::~CBlackPlaneMgr()
{
    Free();
}

HRESULT CBlackPlaneMgr::Ready_BlackPlane(LPDIRECT3DDEVICE9 pGraphicDev)
{
    m_pGraphicDev = pGraphicDev;
    m_pGraphicDev->AddRef();

    Create_BlackPlane();

    NULL_CHECK_RETURN(m_pBlackPlane, E_FAIL);

    return S_OK;
}

void CBlackPlaneMgr::Update_BlackPlane(const _float& fTimeDelta)
{
    if (m_pBlackPlane)
    {
        m_pBlackPlane->Update_GameObject(fTimeDelta);
    }
}

void CBlackPlaneMgr::StartFadeIn()
{ 
    if (m_pBlackPlane)
    {
        dynamic_cast<CBlackPlane*>(m_pBlackPlane)->StartFadeIn(); 
    }
}

void CBlackPlaneMgr::StartFadeOut()
{
    if (m_pBlackPlane)
    {
        dynamic_cast<CBlackPlane*>(m_pBlackPlane)->StartFadeOut();
    }
}


_bool CBlackPlaneMgr::Create_BlackPlane()
{
    if (m_pBlackPlane == nullptr)
    {
        m_pBlackPlane = CBlackPlane::Create(m_pGraphicDev);
        NULL_CHECK_RETURN(m_pBlackPlane, false);
    }

    return true;
}

bool CBlackPlaneMgr::Is_FadeOutComplete()
{
    return dynamic_cast<CBlackPlane*>(m_pBlackPlane)->Get_FadeOver();
}


void CBlackPlaneMgr::Free()
{
    Safe_Release(m_pGraphicDev);
    Safe_Release(m_pBlackPlane);
}
