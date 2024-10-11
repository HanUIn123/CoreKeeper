#include "pch.h"
#include "../Header/ShroomMan.h"
#include "../Header/Player.h"
#include "Export_System.h"
#include "Export_Utility.h"

CShroomMan::CShroomMan(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev)
{
    m_eType = Engine::MON_SHROOMMAN;
    m_fIdleY = 0.65f;
    m_eState = IDLE;
    m_bFlip = false;
    m_fAggroDistance = 8.f;
    m_fImmuneTimeLimit = 0.5f;
    m_bHit = false;
}

CShroomMan::~CShroomMan()
{
}

HRESULT CShroomMan::Ready_GameObject(_vec3 vPos)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_vRespawnPoint = { vPos.x, m_fIdleY, vPos.z };
    m_pTransformCom->Set_Pos(m_vRespawnPoint.x, m_vRespawnPoint.y, m_vRespawnPoint.z);
    m_pTransformCom->Set_Scale(0.6f, 0.6f, 0.6f);
    m_pStateCom->Set_Stat(200, 0, 25, 0);
    m_vecDropItem.push_back(ITEM_MUSHROOM);
    m_vecDropItem.push_back(ITEM_WOOD);
    m_vecDropItem.push_back(ITEM_PEPPER_SEED);
    Set_Speed(1.0f);

    m_pSmokeParticleCom->init(L"../Bin/Resource/Texture/Particle/Puff_Particle/Puff_Particle_%d.png", 3, 0.3f);

    m_pHitParticleCom->init(L"../Bin/Resource/Texture/Effect/Hit_%d.png", 5, 1.0f);

    return S_OK;
}

_int CShroomMan::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);
    if (m_bStopDraw)
    {
        Set_RespawnTimer(fTimeDelta);
        return 0;
    }

    Set_Cast();
    Set_SoundVolumeByDistance();
    if (g_bIsTopCamera)
    {
        if (m_vAttackPoint.x < 0)
            m_eDir = LEFT;
        else
            m_eDir = RIGHT;
    }

    if (m_eState != DEAD && m_bImmuneEnd)
        Check_Hitted();

    if (!m_bImmuneEnd)
    {
        m_fImmuneTime += fTimeDelta;
        if (m_fImmuneTime > m_fImmuneTimeLimit)
        {
            m_bImmuneEnd = true;
            m_fImmuneTime = 0.f;
        }
    }

    if (m_bKnockBackEnd)
    {
        if (m_eState != DEAD && !Check_Wall())
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
    }
    else
        KnockBack(fTimeDelta, 1.8f);

    Flip();
    Set_StuckFree(fTimeDelta);
    m_pAnimatorCom->Update_Animation();

    if (m_eState == SWING)
    {
        _vec3 vDir, vCurPos;
        m_pTransformCom->Get_Info(INFO_POS, &vCurPos);

        vDir = vCurPos - vPrePos;

        if (vDir.x > 0 && vDir.z > 0)
        {
            //vDir.x *= -1.f;

            m_pSmokeParticleCom->update(fTimeDelta, -vDir);

        }
        else if (vDir.x < 0 && vDir.z > 0)
        {
            vDir.z *= -1.f;
            m_pSmokeParticleCom->update(fTimeDelta, vDir);
        }
        else if (vDir.x < 0 && vDir.z < 0)
        {
            vDir.z *= -1.f;
            m_pSmokeParticleCom->update(fTimeDelta, vDir);
        }
        else 
             m_pSmokeParticleCom->update(fTimeDelta, -vDir);

        vPrePos = vCurPos;
    }
    else
        m_pSmokeParticleCom->update(fTimeDelta);
    //else if (m_eState != SWING)
   // {

    if (m_pSmokeParticleCom->isDead())
        m_pSmokeParticleCom->reset();
   // }
    
    if (m_bHit)
    {
        m_pHitParticleCom->update(fTimeDelta);

        if (m_pHitParticleCom->isDead())
        {
            m_pHitParticleCom->reset();
            m_bHit = false;
        }
    }

    Add_RenderGroup(RENDER_ALPHA, this);
    return iExit;
}

void CShroomMan::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CShroomMan::Render_GameObject()
{
    if (m_bStopDraw)
        return;

    _vec3		vPos, vPlayerPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
    if (!m_pCalculatorCom->Check_Distance2D(&vPos, &vPlayerPos, 40.f))
        return;

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
    
    m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture();

    m_pBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());
    m_pBufferCom->Render_Buffer();
    m_pColliderCom->Render_Collider();

    if (m_eState != IDLE)
    {
        m_pSmokeParticleCom->render();
    }

    if (m_bHit)
        m_pHitParticleCom->render();

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CShroomMan::Add_Component()
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

    pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_ShroomManAnimTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ShroomManTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pColliderCom = dynamic_cast<CCollider*>(Engine::Clone_Proto(L"Proto_ShroomManCollider"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    pComponent = m_pSmokeParticleCom = dynamic_cast<CSmoke*>(Engine::Clone_Proto(L"Proto_Smoke"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Smoke", pComponent });

    pComponent = m_pHitParticleCom = dynamic_cast<CHit*>(Engine::Clone_Proto(L"Proto_Hit"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Hit", pComponent });
    return S_OK;
}

// 일정 시간마다 타일 한칸 이동 or 정지
void CShroomMan::Pattern_Idle(const _float& fTimeDelta)
{
    if (!m_bIdling)
    {
        m_bIdling = true;
        if (m_iDir)
        {
            m_fIdleTimeLimit = rand() % 3 + 1; // 1 ~ 3초
            m_iDir = 0;
        }
        else
        {
            m_fIdleTimeLimit = rand() % 4 + 2; // 1 ~ 5초
            m_iDir = rand() % 8 + 1;
        }
    }
    if (m_fIdleTime <= m_fIdleTimeLimit)
    {
        m_fIdleTime += fTimeDelta;
        _vec3	vLook, vRight;
        _float  fLookSpeed = 0, fRightSpeed = 0;
        m_pTransformCom->Get_Info(INFO_LOOK, &vLook);
        m_pTransformCom->Get_Info(INFO_RIGHT, &vRight);

        switch (m_iDir)
        {
        case 0:
            // 정지
            break;
        case 1:
            // 상
            m_eDir = BACK;
            fLookSpeed = m_fSpeed;
            break;
        case 2:
            // 우상
            m_eDir = RIGHT;
            fLookSpeed = m_fDiagSpeed;
            fRightSpeed = m_fDiagSpeed;
            break;
        case 3:
            // 우
            m_eDir = RIGHT;
            fRightSpeed = m_fSpeed;
            break;
        case 4:
            // 우하
            m_eDir = RIGHT;
            fLookSpeed = -m_fDiagSpeed;
            fRightSpeed = m_fDiagSpeed;
            break;
        case 5:
            // 하
            m_eDir = FRONT;
            fLookSpeed = m_fSpeed;
            break;
        case 6:
            // 좌하
            m_eDir = LEFT;
            fLookSpeed = -m_fDiagSpeed;
            fRightSpeed = -m_fDiagSpeed;
            break;
        case 7:
            // 좌
            m_eDir = LEFT;
            fRightSpeed = -m_fSpeed;
            break;
        case 8:
            // 좌상
            m_eDir = LEFT;
            fLookSpeed = m_fDiagSpeed;
            fRightSpeed = -m_fDiagSpeed;
            break;
        }
        Set_Stop(fTimeDelta, &vLook, fLookSpeed, &vRight, fRightSpeed);

        if (m_iDir)
        {
            _int iWeight = 1;
            if (m_eDir == LEFT)
                iWeight = -1;
            m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vLook, &vLook), fTimeDelta, fLookSpeed * m_iSpeedWeight);
            m_pTransformCom->Move_Pos(D3DXVec3Normalize(&vRight, &vRight), fTimeDelta, fRightSpeed * iWeight * m_iSpeedWeight);
        }
    }
    else
    {
        m_bIdling = false;
        m_fIdleTime = 0.f;
    }

    if (!m_iDir)
        m_pAnimatorCom->Set_CurState(IDLE, 0, 5, 12);
    else
        m_pAnimatorCom->Set_CurState(IDLE, 9, 17, 12);
}

// 플레이어 방향으로 돌진 설정 및 돌진 준비 애니메이션 재생
void CShroomMan::Pattern_Chase(const _float& fTimeDelta)
{
    _vec3		vPlayerPos, vPos;
    m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
    m_pTransformCom->Get_Info(INFO_POS, &vPos);

    m_pAnimatorCom->Set_CurState(WALK, 27, 33, 8);
    if(m_pAnimatorCom->Get_MotionIndex() == 30 && m_pAnimatorCom->Get_CurCount() == 0)
        Engine::CSoundMgr::GetInstance()->Play(L"dansk1.wav", SOUND_MUSHROOM, m_fSoundVolume);

    if (m_pAnimatorCom->Get_MotionEnd())
    {
        m_fAttackDistance = 0.f;
        m_bAttackFailed = false;
        m_bAttackSuccess = false;
        m_fAttackTime = 0.f;
        m_vStartPoint = vPos;
        m_vAttackPoint = vPlayerPos - vPos;
        D3DXVec3Normalize(&m_vAttackPoint, &m_vAttackPoint);
        m_vAttackPoint.y = 0.f;
        m_eState = SWING;
    }
}

// 돌진 및 돌진(걷기) 애니메이션 재생
void CShroomMan::Pattern_Attack(const _float& fTimeDelta)
{
    _vec3		vPos, vPlayerPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

    // 일정 스피드 이상일 때 충돌 처리
    if (!m_bAttackSuccess)
    {
        if (m_fAttackTime > 0.2f)
        {
            _vec3 vCheckPos = vPos + m_vAttackPoint * m_fSpeed * m_fAttackTime * 0.1f;
            _int iIndex = _int(vCheckPos.z + 0.5f * VTXITV) * (VTXCNTX - 1) + (vCheckPos.x + 0.5f * VTXITV);

            // 플레이어 충돌 시 공격 성공
            if (m_pColliderCom->Check_Collision(m_pPlayerCollider))
            {
                m_bAttackSuccess = true;
                m_pPlayer->Set_KnockBack(vPos, m_pStateCom->Get_Stat()->iAttack, 1.5f, HIT_NORMAL);
                Engine::CSoundMgr::GetInstance()->Play(L"damagePlayer.wav", SOUND_MUSHROOM, m_fSoundVolume);
            }
            // 벽 충돌
            else if (0 <= iIndex && iIndex < VTXCNTX * VTXCNTZ)
            {
                if (m_pTerrain->Get_UnreachableByIndex(iIndex))
                {
                    m_bAttackSuccess = true;
                    Engine::CSoundMgr::GetInstance()->Play(L"Wall_Clay_dmg_1.wav", SOUND_MUSHROOM, m_fSoundVolume);
                }
            }
        }
    }
    // 공격 중 : 돌진
    if (!m_bAttackSuccess)
    {
        // 가속
        if (m_fAttackDistance < 6.f)
        {
            m_fAttackTime += fTimeDelta * 1.5f;
            m_pAnimatorCom->Set_CurState(SWING, 9, 17, 3);
            m_pTransformCom->Move_Pos(&m_vAttackPoint, m_fAttackTime * 0.08f, m_fSpeed);
            m_fAttackDistance += m_fSpeed * m_fAttackTime * 0.08f;
        }
        // 감속
        else
        {
            if (m_fAttackDistance < 8.f)
                m_fAttackTime -= fTimeDelta * 2;
            m_pAnimatorCom->Set_CurState(SWING, 9, 17, 3);
            m_pTransformCom->Move_Pos(&m_vAttackPoint, m_fAttackTime * 0.1f, m_fSpeed);
            m_fAttackDistance += m_fSpeed * m_fAttackTime * 0.1f;
        }
        if(m_pAnimatorCom->Get_CurCount() == 0 && m_pAnimatorCom->Get_MotionIndex() % 3 == 0)
            Engine::CSoundMgr::GetInstance()->Play(L"Footstep_Dirt.wav", SOUND_MUSHROOM, m_fSoundVolume);
            
        // 정지
        if (m_fAttackDistance > 12.f)
            m_bAttackFailed = true;
    }
    // 공격 성공 : 엉덩방아
    else
    {
        m_pAnimatorCom->Set_CurState(SWING, 36, 44, 6);            
        if (m_pAnimatorCom->Get_MotionEnd())
        {
            if (m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, m_fAggroDistance))
                m_eState = WALK;
            else
            {
                m_eState = IDLE;
                m_iDir = 1;
            }
        }
        _vec3 vDir = m_vStartPoint - vPos;
        D3DXVec3Normalize(&vDir, &vDir);
        m_pTransformCom->Move_Pos(&vDir, fTimeDelta, 0.1f);
    }

    if (m_bAttackFailed)
    {
        if (m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, 12.f))
            m_eState = WALK;
        else
        {
            m_eState = IDLE;
            m_iDir = 1;
        }
    }
}

void CShroomMan::Pattern_Dead()
{
    m_pAnimatorCom->Set_CurState(DEAD, 40, 44, 4);
    if(m_pAnimatorCom->Get_CurCount() == 0 && m_pAnimatorCom->Get_MotionIndex() == 41)
        Engine::CSoundMgr::GetInstance()->Play(L"dansk2.wav", SOUND_MUSHROOM, m_fSoundVolume);
    if (m_pAnimatorCom->Get_MotionEnd())
    {
        m_bStopDraw = true;
        m_pColliderCom->Set_Offset(_vec3(0, -100.f, 0));
        Drop_Item();
    }
}

STATE CShroomMan::State_Change()
{
    if (m_eState == IDLE)
    {
        CGameObject* pWeapon = m_pPlayer->Get_HandedItem();
        _vec3 vPlayerPos, vPos;
        m_pTransformCom->Get_Info(INFO_POS, &vPos);
        m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
        // 플레이어가 어그로 범위 내에 들어올 경우(선공)
        if (m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, m_fAggroDistance))
            return WALK;
    }
    return m_eState;
}

CShroomMan* CShroomMan::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CShroomMan* pMushroom = new CShroomMan(pGraphicDev);

    if (FAILED(pMushroom->Ready_GameObject(vPos)))
    {
        Safe_Release(pMushroom);
        MSG_BOX("Mushroom Create Failed");
        return nullptr;
    }
    return pMushroom;
}

void CShroomMan::Free()
{
    Engine::CGameObject::Free();
}