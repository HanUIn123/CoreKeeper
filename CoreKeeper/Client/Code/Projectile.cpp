#include "pch.h"
#include "../Header/Projectile.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "../Header/Player.h"

CProjectile::CProjectile(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev), m_bChargeActive(true), m_iChargingTime(45)
{
    m_eType = Engine::MON_PROJECTILE;
    m_fIdleY = 0.2f;
    m_eState = IDLE;

    m_bFlip = false;

    m_fAttackTime = 0.f;
    m_fAttackTimeLimit = 4.f;
    m_vAttackPoint = { 0, 0, 0 };

    m_fAggroDistance = 10.f;
    m_bCharging = true;
    m_bAttackSuccess = false;
    m_bCollideWithPlayer = false;

    m_bLightEnable = true;
    m_iLightNum = g_iLightNum++;

    m_bSmog = false;
}

CProjectile::~CProjectile()
{
}

HRESULT CProjectile::Ready_GameObject(_vec3 vPos)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
    m_pTransformCom->Set_Scale(0.6f, 0.6f, 0.6f);
    m_pStateCom->Set_Stat(100, 0, 50, 0);
    Set_Speed(8.f);

    m_pSmogParticleCom->init(L"../Bin/Resource/Texture/Particle/Smog_Particle/Big_Smog_%d.png", 6, 1.f);

    return S_OK;
}

_int CProjectile::Update_GameObject(const _float& fTimeDelta)
{
    if (m_bSmog)
    {
        m_pSmogParticleCom->update(fTimeDelta);

        Add_RenderGroup(RENDER_ALPHA, this);

        if (m_pSmogParticleCom->isDead())
        {
            m_bSmog = false;

            m_pSmogParticleCom->reset();
        }
    }

    if (m_bStopDraw)
        return 0;

    Set_Light();

    if (m_eState != DEAD)
        m_eState = State_Change();
    switch (m_eState)
    {
    case IDLE:
        Pattern_Idle(fTimeDelta);
        break;
    case WALK:
        Pattern_Chase(fTimeDelta);
        break;
    case SWING:
        Pattern_Attack(fTimeDelta);
        break;
    case DEAD:
        Pattern_Dead();
        break;
    }


    Flip();
    m_pAnimatorCom->Set_CurState(IDLE, 0, 5, 8);
    m_pAnimatorCom->Update_Animation();
    Add_RenderGroup(RENDER_ALPHA, this);
    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CProjectile::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CProjectile::Render_GameObject()
{
    if (m_bSmog)
    {
        m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_SmogMatrix);
        m_pSmogParticleCom->render();
    }
    if (!m_bChargeActive && m_eState == IDLE)
        return;

    if (m_bStopDraw)
    {
        return;
    }
    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

    m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture();

    m_pBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());
    m_pBufferCom->Render_Buffer();
    m_pColliderCom->Render_Collider();

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CProjectile::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    pComponent = m_pStateCom = dynamic_cast<CState*>(Engine::Clone_Proto(L"Proto_State"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_State", pComponent });

    pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

    pComponent = m_pAnimatorCom = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_Animator"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Animator", pComponent });

    pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_ShamProjectileAnimTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ShamProjectileTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pColliderCom = dynamic_cast<CCollider*>(Engine::Clone_Proto(L"Proto_ShamProjectileCollider"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    pComponent = m_pSmogParticleCom = dynamic_cast<CSmog*>(Engine::Clone_Proto(L"Proto_Smog"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Particle", pComponent });

    return S_OK;

}

// 차징(정지)
void CProjectile::Pattern_Idle(const _float& fTimeDelta)
{
    m_fIdleTime += 1;
}

// m_vAttackPoint를 향해 발사
void CProjectile::Pattern_Chase(const _float& fTimeDelta)
{
    if (m_bCharging)
    {
        m_bCharging = false;
        _vec3 vPos, vPlayerPos;
        m_pTransformCom->Get_Info(INFO_POS, &vPos);
        Engine::CTransform * pPlayerTransform = dynamic_cast<Engine::CTransform*>
            (Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));
        pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
        m_vAttackPoint = vPlayerPos - vPos;
        D3DXVec3Normalize(&m_vAttackPoint, &m_vAttackPoint);
        if (m_vAttackPoint.x < 0)
            m_eDir = LEFT;
    }
    m_fAttackTime += fTimeDelta;
    m_pTransformCom->Move_Pos(&m_vAttackPoint, fTimeDelta, m_fSpeed);
}

// 부딪혔을 때
void CProjectile::Pattern_Attack(const _float& fTimeDelta)
{
    // 플레이어 피격 설정
    if (!m_bAttackSuccess)
    {
        m_bAttackSuccess = true;
        if (m_bCollideWithPlayer)
        {
            _vec3		vPos;
            m_pTransformCom->Get_Info(INFO_POS, &vPos);
            dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player"))
                ->Set_KnockBack(vPos, m_pStateCom->Get_Stat()->iAttack);
            // 플레이어 화상 상태 이상 추가
        }
    }
}

// 죽어버리기
void CProjectile::Pattern_Dead()
{
    // 폭발 이펙트 생성
    m_pTransformCom->Get_WorldMatrix(&m_SmogMatrix);

    m_bSmog = true;

    m_bStopDraw = true;
    m_pGraphicDev->LightEnable(m_iLightNum, FALSE);


}

STATE CProjectile::State_Change()
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player"));
    _vec3 vPlayerPos, vPos, vCheckPos;
    dynamic_cast<CTransform*>(pPlayer->Get_Component(ID_DYNAMIC, L"Com_Transform"))->Get_Info(INFO_POS, &vPlayerPos);
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    CCollider* pPlayerCollider;
    CTerrain* pTerrain;
    _int iIndex;
    switch (m_eState)
    {
    case IDLE:
        // 차징 끝나면 WALK(공격)
        if(m_fIdleTime > m_iChargingTime * 3)
            m_eState = WALK;
        break;
    case WALK:
        // 날아가다가 플레이어랑 부딪히면 SWING(폭발)
        pPlayerCollider = dynamic_cast<Engine::CCollider*>(pPlayer->Get_Component(ID_DYNAMIC, L"Com_Collider"));
        vCheckPos = vPos + m_vAttackPoint * m_fSpeed * m_fAttackTime * 0.1f;
        iIndex = _int(vCheckPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + (vCheckPos.x + 0.5f * VTXITV);
        pTerrain = dynamic_cast<CTerrain*>(Engine::Get_GameObject(L"Layer_Environment", L"Terrain"));
        if (m_pColliderCom->Check_Collision(pPlayerCollider))
        {
            m_eState = SWING;
            m_bCollideWithPlayer = true;
        }
        else if (0 <= iIndex && iIndex < (VTXCNTX - 1) * (VTXCNTZ - 1))
        {
            if (pTerrain->Get_UnreachableByIndex(iIndex))
            {
                m_eState = SWING;
                m_bCollideWithPlayer = false;
            }
        }
        else
        {
            // 날아가다가 사정거리 이상 넘어가거나 지면에 닿으면 DEAD
            if (m_fAttackTime > m_fAttackTimeLimit || vPos.y < 0.5f)
                m_eState = DEAD;
        }
        break;
    case SWING:
        // 공격 성공했으면 죽어야지~~
        if (m_bAttackSuccess)
            m_eState = DEAD;
        break;
    }
    if (vPos.y < 0.f)
        m_eState = DEAD;
    return m_eState;
}

void CProjectile::Set_Light()
{
    D3DLIGHT9 light;
    ZeroMemory(&light, sizeof(D3DLIGHT9));

    light.Type = D3DLIGHT_POINT; // 포인트 조명
    light.Diffuse = { 1.f, 1.f, 1.f, 1.f }; // 확산 색상
    light.Specular = { 1.f, 1.f, 1.f, 1.f }; // 반사 색상
    light.Ambient = { 1.f, 1.f, 1.f, 1.f }; // 주변광

    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);

    light.Position = vPos; // 횃불의 위치
    light.Range = 4.0f; // 조명의 범위
    light.Falloff = 1.f; // 감쇠
    light.Attenuation0 = 1.0f; // 감쇠 계수
    light.Attenuation1 = 0.01f;
    light.Attenuation2 = 0.0f;

    m_pGraphicDev->SetLight(m_iLightNum, &light); // 조명 설정
    if(m_bLightEnable)
        m_pGraphicDev->LightEnable(m_iLightNum, TRUE);
    else
        m_pGraphicDev->LightEnable(m_iLightNum, FALSE);
}

CProjectile* CProjectile::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CProjectile* pProjectile = new CProjectile(pGraphicDev);

    if (FAILED(pProjectile->Ready_GameObject(vPos)))
    {
        Safe_Release(pProjectile);
        MSG_BOX("Shaman Create Failed");
        return nullptr;
    }
    return pProjectile;
}

void CProjectile::Free()
{
    Engine::CGameObject::Free();
}