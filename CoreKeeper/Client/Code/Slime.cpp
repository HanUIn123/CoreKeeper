#include "pch.h"
#include "../Header/Slime.h"
#include "Export_System.h"
#include "Export_Utility.h"

CSlime::CSlime(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CGameObject(pGraphicDev)
{
}

CSlime::~CSlime()
{
}

HRESULT CSlime::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    //m_pTransformCom->m_vScale = { 0.8f, 0.8f, 0.8f };
    m_pTransformCom->Set_Pos(m_pTransformCom->m_vInfo->x, m_pTransformCom->m_vInfo->y + 1.0f, m_pTransformCom->m_vInfo->z);

    return S_OK;
}

_int CSlime::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

    m_pAnimatorCom->Set_CurState(WALK, 0, 8, 8);

    /*Engine::CTransform* pPlayerTransform = dynamic_cast<Engine::CTransform*>
        (Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK_RETURN(pPlayerTransform, -1);

    _vec3		vPlayerPos;
    pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

    m_pTransformCom->Chase_Target(&vPlayerPos, 5.f * fTimeDelta);

    Add_RenderGroup(RENDER_ALPHA, this);*/

    Add_RenderGroup(RENDER_ALPHA, this);

    return iExit;
}

void CSlime::LateUpdate_GameObject()
{
    m_pAnimatorCom->Update_Animation();

    Engine::CGameObject::LateUpdate_GameObject();
}

void CSlime::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture();

    m_pBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());
    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CSlime::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_SlimeAnimTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Transform", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_SlimeTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Calculator", pComponent });

    pComponent = m_pAnimatorCom = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_Animator"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Animator", pComponent });

    return S_OK;

}

CSlime* CSlime::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CSlime* pSlime = new CSlime(pGraphicDev);

    if (FAILED(pSlime->Ready_GameObject()))
    {
        Safe_Release(pSlime);
        MSG_BOX("슬라임 소환 실패");
        return nullptr;
    }
    return pSlime;
}

void CSlime::Free()
{
    Engine::CGameObject::Free();
}
