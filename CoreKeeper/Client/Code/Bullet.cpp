#include "pch.h"
#include "../Header/Bullet.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "../Header/Player.h"

CBullet::CBullet(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev)
{
    m_eType = Engine::MON_BULLET;
    m_fIdleY = 0.2f;
    m_eState = WALK;

    m_bFlip = false;

    m_fAttackTime = 0.f;
    m_fAttackTimeLimit = 4.f;
    m_vAttackPoint = { 0, 0, 0 };

    m_fAggroDistance = 10.f;
    m_bFirstFrame = true;
    m_bAttackSuccess = false;
    m_bCollideWithPlayer = false;

    m_bSmog = false;
}

CBullet::~CBullet()
{
}

HRESULT CBullet::Ready_GameObject(_vec3 vPos)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_vRespawnPoint = { vPos.x, vPos.y, vPos.z };
    m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
    m_pTransformCom->Set_Scale(0.2f, 0.2f, 0.2f);
    m_pStateCom->Set_Stat(100, 0, 50, 0);
    Set_Speed(8.f);

    m_pSmogParticleCom->init(L"../Bin/Resource/Texture/Particle/Smog_Particle/Big_Smog_%d.png", 6, 1.f);
    m_pFlameParticleCom->init(L"../Bin/Resource/Texture/Particle/FirePulse/Fire_Pulse_%d.png", 4, 0.1f);

    return S_OK;
}

_int CBullet::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);
    if (m_bSmog)
    {
        m_pSmogParticleCom->update(fTimeDelta);

        m_pFlameParticleCom->update(fTimeDelta);

        Engine::Add_RenderGroup(RENDER_ALPHA, this);


        if (m_pFlameParticleCom->isDead())
        {

            m_pFlameParticleCom->reset();
        }

        if (m_pSmogParticleCom->isDead())
        {
            m_bSmog = false;

            m_pSmogParticleCom->reset();
        }

    }

    if (m_bStopDraw)
        return 0;

    Set_Cast();
    Set_SoundVolumeByDistance();

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

    m_pAnimatorCom->Set_CurState(WALK, 0, 3, 4);
    m_pAnimatorCom->Update_Animation();
    Engine::Add_RenderGroup(RENDER_ALPHA, this);
    return iExit;
}

void CBullet::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CBullet::Render_GameObject()
{
    if (m_bSmog)
    {
        m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_SmogMatrix);
        m_pSmogParticleCom->render();

        m_pFlameParticleCom->render();
    }

    if (m_bStopDraw)
        return;

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

HRESULT CBullet::Add_Component()
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

    pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_HunterBulletAnimTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_HunterBulletTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pColliderCom = dynamic_cast<CCollider*>(Engine::Clone_Proto(L"Proto_HunterBulletCollider"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    pComponent = m_pSmogParticleCom = dynamic_cast<CSmog*>(Engine::Clone_Proto(L"Proto_Smog"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Particle", pComponent });

    pComponent = m_pFlameParticleCom = dynamic_cast<CFirework*>(Engine::Clone_Proto(L"Proto_Firework"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_FireParticle", pComponent });

    return S_OK;

}

void CBullet::Pattern_Idle(const _float& fTimeDelta)
{
   
}

// m_vAttackPoint를 향해 발사
void CBullet::Pattern_Chase(const _float& fTimeDelta)
{
    if (m_bFirstFrame)
    {
        m_bFirstFrame = false;
        Engine::CSoundMgr::GetInstance()->Play(L"musketFire2.wav", SOUND_HUNTER, m_fSoundVolume);
        _vec3 vPlayerPos;
        m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
        m_vAttackPoint = vPlayerPos - m_vRespawnPoint;
        D3DXVec3Normalize(&m_vAttackPoint, &m_vAttackPoint);
        m_vAttackPoint.y = 0;
    }
    m_fAttackTime += fTimeDelta;
    m_pTransformCom->Move_Pos(&m_vAttackPoint, fTimeDelta, m_fSpeed);
}

// 부딪혔을 때
void CBullet::Pattern_Attack(const _float& fTimeDelta)
{
    // 플레이어 피격 설정
    if (!m_bAttackSuccess)
    {
        m_bAttackSuccess = true;
        if (m_bCollideWithPlayer)
        {
            _vec3		vPos;
            m_pTransformCom->Get_Info(INFO_POS, &vPos);
            m_pPlayer->Set_KnockBack(vPos, m_pStateCom->Get_Stat()->iAttack, 2.f, HIT_BULLET);
        }
    }
}

// 죽어버리기
void CBullet::Pattern_Dead()
{
    // 폭발 이펙트 생성
    m_pTransformCom->Get_WorldMatrix(&m_SmogMatrix);
    m_bSmog = true;
    m_bStopDraw = true;
}

STATE CBullet::State_Change()
{
    _vec3 vPlayerPos, vPos, vCheckPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
    _int iIndex;
    switch (m_eState)
    {
    case IDLE:

        break;
    case WALK:
        // 날아가다가 플레이어 혹은 벽에 부딪히면 SWING(폭발)
        vCheckPos = vPos + m_vAttackPoint * m_fSpeed * m_fAttackTime * 0.1f;
        iIndex = _int(vCheckPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + (vCheckPos.x + 0.5f * VTXITV);
        if (m_pColliderCom->Check_Collision(m_pPlayerCollider))
        {
            m_eState = SWING;
            m_bCollideWithPlayer = true;
        }
        else if (0 <= iIndex && iIndex < (VTXCNTX - 1) * (VTXCNTZ - 1))
        {
            if (m_pTerrain->Get_UnreachableByIndex(iIndex))
            {
                Engine::CSoundMgr::GetInstance()->Play(L"impactSweetener1.wav", SOUND_HUNTER, m_fSoundVolume);
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

CBullet* CBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CBullet* pBullet = new CBullet(pGraphicDev);

    if (FAILED(pBullet->Ready_GameObject(vPos)))
    {
        Safe_Release(pBullet);
        MSG_BOX("Hunter's Bullet Create Failed");
        return nullptr;
    }
    return pBullet;
}

void CBullet::Free()
{
    Engine::CGameObject::Free();
}