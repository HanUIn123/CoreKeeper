#include "pch.h"
#include "../Header/Slime.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "../Header/Item.h"

CSlime::CSlime(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CGameObject(pGraphicDev), m_dwIdleTime(0.f),m_dwWalkTime(0.f),
    m_dwAttackTime(0.f),m_dwDeadTime(0.f), m_bIsWalking(false), m_bDead(false)
{
    m_eState = IDLE;
    m_vPos = D3DXVECTOR3(3.f, 1.f, 3.f);
    srand(static_cast<unsigned>(time(0)));    
    m_tStat.iHp = 100;
    m_tStat.iMaxHp = 310;
    m_tStat.iAttack = 31;
    m_tStat.iDefense = 0;
}

CSlime::~CSlime()
{
}

HRESULT CSlime::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    //m_pTransformCom->Set_Pos(6.f, 6.f, 1.f);

    m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

    SetRandomDestination();

    //m_pTransformCom->Set_Pos(m_vPos.x, m_vPos.y, m_vPos.z);
    //m_pTransformCom->m_vScale = { 0.8f, 0.8f, 0.8f };
    //m_pTransformCom->Set_Pos(m_pTransformCom->m_vInfo->x, m_pTransformCom->m_vInfo->y + 1.0f, m_pTransformCom->m_vInfo->z);

    return S_OK;
}

_int CSlime::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

    if (m_bDead)
        return 0;

    if (Is_Dead())
    {
        Drop_Items(m_pGraphicDev);
        return 0;
    }

    Focus_Camera();

    Slime_Animation(m_eState);

    if (m_bIsWalking)
    {
        MoveTowardDestination(fTimeDelta);
    }

    Ego_Behavior(fTimeDelta);

    m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

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

    pComponent = m_pColliderCom = dynamic_cast<CCollider*>(Engine::Clone_Proto(L"Proto_PlayerCollider"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    return S_OK;

}

void CSlime::Slime_Animation(STATE st)
{
    switch (st)
    {
    case IDLE:
        m_pAnimatorCom->Set_CurState(st, 0, 8, 9);
        break;

    case WALK:
        m_pAnimatorCom->Set_CurState(st, 0, 9, 9);
        break;
    case SWING:
        m_pAnimatorCom->Set_CurState(st, 0, 11, 9);
        break;
    case DEAD:
        m_pAnimatorCom->Set_CurState(st, 0, 5, 9);
        break;
    }
}

void CSlime::Focus_Camera()
{
    D3DXMATRIX matWorld, matView, matBillboard, matTranslation;

    m_pTransformCom->Get_WorldMatrix(&matWorld);

    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

    D3DXMatrixIdentity(&matBillboard);
    matBillboard._11 = matView._11;
    matBillboard._12 = matView._12;
    matBillboard._13 = matView._13;

    matBillboard._21 = matView._21;
    matBillboard._22 = matView._22;
    matBillboard._23 = matView._23;

    matBillboard._31 = matView._31;
    matBillboard._32 = matView._32;
    matBillboard._33 = matView._33;

    D3DXMatrixInverse(&matBillboard, nullptr, &matBillboard);

    D3DXMatrixTranslation(&matTranslation, matWorld._41, matWorld._42, matWorld._43);

    D3DXMATRIX matFinal = matBillboard * matTranslation;

    m_pTransformCom->Set_WorldMatrix(&matFinal);
}

HRESULT CSlime::Drop_Items(LPDIRECT3DDEVICE9 pGraphicDev)
{
    ITEMDESC ItemDesc;
    ZeroMemory(&ItemDesc, sizeof(ITEMDESC));

    D3DXVECTOR3 vPosition = m_vPos;
    ItemDesc.fPosition = vPosition; 

    ItemDesc.pTextureComponent = (L"Com_Texture_Slime_Gel"); 
    ItemDesc.pTexturePrototype = (L"Prototype_Component_Texture_Item");
    ItemDesc.eItemName = 1; 

    CItem* pNewItem = CItem::Create(pGraphicDev);

    NULL_CHECK_RETURN(pNewItem, E_FAIL);;

    Safe_Release(pNewItem);

    return S_OK;
}

void CSlime::Ego_Behavior(_float fTimeDelta)
{
    _vec3	vLook, vRight;

    m_pTransformCom->Get_Info(INFO_LOOK, &vLook);
    m_pTransformCom->Get_Info(INFO_RIGHT, &vRight);

    DWORD m_dwIdleTime = GetTickCount();

    if (IDLE)
    {
        if (m_dwIdleTime + 1000 < GetTickCount())
        {
            m_eState = WALK;
            m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, 2.f);
            m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, 2.f);
            m_dwIdleTime = GetTickCount();
        }
    }
}

_float CSlime::On_Damaged(_float damage)
{
    int finalDamage = damage - m_tStat.iDefense;
    if (finalDamage < 0)
    { 
        finalDamage = 0;
    }

    m_tStat.iHp -= finalDamage;
    if (m_tStat.iHp <= 0) {
        m_bDead = true;
        m_eState = DEAD;
        Slime_Animation(DEAD);
    }

    return damage;
}

_bool CSlime::Is_Dead()
{
    if (m_bDead && m_eState == STATE::DEAD && GetTickCount() > m_dwDeadTime + 1500)
        return true;
    else if (m_bDead && m_eState != STATE::DEAD)
    {
        m_dwDeadTime = GetTickCount();
        m_eState = DEAD;
    }

    return false;
}

void CSlime::SetRandomDestination()
{
    m_vDestPos.x = static_cast<float>(rand() % 7 - 3);
    m_vDestPos.z = static_cast<float>(rand() % 7 - 3);
    m_vDestPos.y = m_vPos.y;

    m_bIsWalking = true;
    Slime_Animation(WALK);
}

void CSlime::MoveTowardDestination(const _float& fTimeDelta)
{
    D3DXVECTOR3 vDir = m_vDestPos - m_vPos;
    D3DXVec3Normalize(&vDir, &vDir);

    D3DXVECTOR3 vDistanceVec = m_vDestPos - m_vPos;
    float fDistance = D3DXVec3Length(&vDistanceVec);

    m_vPos += vDir * m_fSpeed * fTimeDelta;

    if (fDistance <= 0.1f)
    {
        m_bIsWalking = false; 
        Slime_Animation(IDLE);  

        int waitTime = rand() % 3 + 1;
        Sleep(waitTime * 1000);   

        SetRandomDestination();
    }

    float fAngle = atan2f(vDir.x, vDir.z);

    D3DXMATRIX matRot;
    D3DXMatrixRotationY(&matRot, fAngle);

    D3DXMATRIX matWorld, matFinal;
    m_pTransformCom->Get_WorldMatrix(&matWorld);
    D3DXMatrixMultiply(&matFinal, &matRot, &matWorld);

    m_pTransformCom->Set_WorldMatrix(&matFinal);
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

