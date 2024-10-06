#include "pch.h"
#include "../Header/Azeos.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "../Header/Player.h"
#include "../Header/Fire.h"
#include "../Header/Thunder.h"
#include "../Header/Crystal.h"
#include <math.h>

CAzeos::CAzeos(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev)
{
    m_eType = Engine::MON_AZEOS;
    m_fIdleY = 0.4f;
    m_fJumpY = 6.f;
    m_eState = IDLE;
    m_fAggroDistance = 32.f;
    m_iTagNumber = 10;

    m_iIdleCount = 0;
    m_bCrystal = false;
    m_iRandomPattern = 0;
    m_iRandom = 0;
    m_iEndCount = 0;

    m_iCrystalNumber = 0;

    m_bAttack = false;
}

CAzeos::~CAzeos()
{
}

HRESULT CAzeos::Ready_GameObject(_vec3 vPos)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);


    m_pTransformCom->Set_Pos(vPos.x, 3.f, vPos.z);
    m_pTransformCom->Set_Scale(3.2f, 3.2f, 3.2f);
    m_pColliderCom->Set_Offset(_vec3(-0.25f, -0.5f, 0));
    m_pStateCom->Set_Stat(1000, 0, 25, 0);
    //m_vecDropItem.push_back(ITEM_STAFF);
    //m_vecDropItem.push_back(ITEM_WOOD);
    Set_Speed(6.0f);

    m_pHitParticleCom->init(L"../Bin/Resource/Texture/Effect/Hit_%d.png", 5, 2.0f);

    m_vFirstPos = vPos;

    return S_OK;
}

_int CAzeos::Update_GameObject(const _float& fTimeDelta)
{
    if (m_bStopDraw)
        return 0;

    Set_Cast();
    //Set_Light();

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

    if (m_eState != DEAD && m_bKnockBackEnd)
        Check_Hitted();

    if (m_bKnockBackStart)
        m_fImmuneTime += fTimeDelta;

    if (m_fImmuneTime > m_fImmuneTimeLimit)
    {
        m_fImmuneTime = 0.f;
        m_bKnockBackStart = false;
        m_bKnockBackEnd = true;
    }

    if (m_bHit)
    {
        m_pHitParticleCom->update(fTimeDelta);

        if (m_pHitParticleCom->isDead())
        {
            m_pHitParticleCom->reset();
            m_bHit = false;
        }
    }
    m_pTransformCom->Get_Info(INFO_POS, &m_vPos);

    Flip();
    Set_StuckFree(fTimeDelta);
    m_pAnimatorCom->Update_Animation();
    Add_RenderGroup(RENDER_ALPHA, this);
    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CAzeos::LateUpdate_GameObject()
{

    Engine::CGameObject::LateUpdate_GameObject();
}

void CAzeos::Render_GameObject()
{
    if (m_bStopDraw)
        return;
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
    m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture();

    m_pBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());
    m_pBufferCom->Render_Buffer();
    m_pColliderCom->Render_Collider();

    if (m_bHit)
    {
        m_pHitParticleCom->render();
    }

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CAzeos::Add_Component()
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

    pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_AzeosAnimTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_AzeosTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pColliderCom = dynamic_cast<CCollider*>(Engine::Clone_Proto(L"Proto_MalugazCollider"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    pComponent = m_pHitParticleCom = dynamic_cast<CHit*>(Engine::Clone_Proto(L"Proto_Hit"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Hit", pComponent });

    return S_OK;

}

void CAzeos::Pattern_Idle(const _float& fTimeDelta)
{
    // 벽 확인 추가할 것

    if (!g_bIsTopCamera)
        m_eDir = FRONT;

    m_pAnimatorCom->Set_CurState(IDLE, 0, 7, 8);
}

// 텔레포트
void CAzeos::Pattern_Chase(const _float& fTimeDelta)
{
    _vec3		vPlayerPos, vPos, vDir;
    m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    /* vDir = vPlayerPos - vPos;
    D3DXVec3Normalize(&vDir, &vDir);
    m_pAnimatorCom->Set_CurState(WALK, 12, 21, 8);
    Set_Stop(&vDir, m_fSpeed);
    m_pTransformCom->Move_Pos(&vDir, fTimeDelta, m_fSpeed * m_iSpeedWeight);*/

    // 플레이어 보다 밖에 있을 경우
   // if (m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, m_fAggroDdwistance))
    //    m_eState = IDLE;
   // else // 아닐 경우 텔레포트
   // {
        Pattern_Teleport();

        m_eState = IDLE;
    //}
}

void CAzeos::Pattern_Attack(const _float& fTimeDelta)
{
    m_pAnimatorCom->Set_CurState(SWING, 8, 20, 8);


    if (m_pAnimatorCom->Get_MotionIndex() == 20)
    {
        m_eState = IDLE;

        m_bAttack = false;
        return;
    }

    if (m_pAnimatorCom->Get_MotionIndex() <= 8 && !m_bAttack)
    {
        _int iRand = rand() % 2;

        while (true) //  똑같은 함수 반복되지 않게 함
        {
            if (iRand == 0 && m_iRandomPattern != iRand)
            {
                Pattern_GenerateThunder();
                m_iRandomPattern = iRand;

                m_bAttack = true;
                return;
            }
            else if (iRand == 1 && m_iRandomPattern != iRand)
            {
                Pattern_GenerateCrystal();
                m_iRandomPattern = iRand;

                m_bAttack = true;
                return;
            }

            iRand = rand() % 2;
        }
    }
    /*
    _vec3		vPos, vPlayerPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    Engine::CTransform* pPlayerTransform = dynamic_cast<Engine::CTransform*>
        (Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK(pPlayerTransform);
    Engine::CCollider* pPlayerCollider = dynamic_cast<Engine::CCollider*>
        (Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Collider"));
    m_pAnimatorCom->Set_CurState(SWING, 24, 35, 8);
    pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
    m_fSpeedWeight = 0.f;
    // 점프 중이 아닐 때
    if (m_pAnimatorCom->Get_MotionIndex() <= 29 || m_pAnimatorCom->Get_MotionIndex() >= 34)
    {
        m_bAttackSuccess = false;
        m_bFallStart = false;
        m_vStartPoint = vPos;
        m_vAttackPoint = vPlayerPos - vPos;
        D3DXVec3Normalize(&m_vAttackPoint, &m_vAttackPoint);
        if (m_pAnimatorCom->Get_MotionIndex() == 25 || m_pAnimatorCom->Get_MotionIndex() == 34)
            m_pTransformCom->Set_Pos(vPos.x, m_fIdleY - 0.001f, vPos.z);
        else if (m_pAnimatorCom->Get_MotionIndex() == 35)
        {
            m_pTransformCom->Set_Pos(vPos.x, m_fIdleY, vPos.z);
            m_bJumping = false;
            if (m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, m_fAggroDistance))
                m_eState = SWING;
            else
                m_eState = WALK;
        }
    }
    else
        // 점프 할 때
    {
        JumpY(fTimeDelta);
        m_fSpeedWeight = 8.f;
        if (vPos.y > m_fIdleY)
        {
            // 공격 성공
            if (m_pColliderCom->Check_Collision(pPlayerCollider))
            {
                if (!m_bAttackSuccess)
                {
                    m_bAttackSuccess = true;
                    dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player"))
                        ->Set_KnockBack(vPos, m_pStateCom->Get_Stat()->iAttack, (1 - (m_fJumpTime / m_fJumpFrame)) * 3.f + 1.f);
                }
            }
        }
        if (m_bAttackSuccess)
            FallDir(fTimeDelta);
        else
        {
            Set_Stop(&m_vAttackPoint, m_fSpeedWeight);
            m_pTransformCom->Move_Pos(&m_vAttackPoint, fTimeDelta, m_fSpeedWeight * m_iSpeedWeight);
        }
    }*/
}

void CAzeos::Pattern_Dead()
{  
    m_pAnimatorCom->Set_CurState(DEAD, 9, 18, 12);

    m_iEndCount++;

    if (m_pAnimatorCom->Get_MotionIndex() == 18 && m_iEndCount >= 500)
    {
        m_iEndCount = 0;
       // m_pGraphicDev->LightEnable(m_iLightNum, FALSE); // 조명 비활성화
        m_bLightEnable = false;
        m_bStopDraw = true;
        Drop_Item();
    }
}

STATE CAzeos::State_Change()
{
    // 공격하고 IDLE 순간이동하고 IDLE 반복
    _vec3 vPlayerPos, vPos;
    CGameObject* pWeapon;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

    switch (m_eState)
    {
    case IDLE:
    {
        if (m_iIdleCount >= 200)
        {
            _int iRandom = rand() % 3;
            // 플레이어가 어그로 범위 내에 들어올 경우(선공)
            if (m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, m_fAggroDistance) && iRandom <= 1)
            {
                m_iIdleCount = 0;

                return SWING;
            }
            else if (iRandom == 2)
            {
                m_iIdleCount = 0;

                return WALK;
            }
      
        }
        else if (m_bAttackSuccess)
        {
            m_iIdleCount = 0;
        }
        else
        {
            m_iIdleCount++;

            return IDLE;
        }

        break;
    }
    case WALK:
    {
        if (!m_pCalculatorCom->Check_Distance2D(&vPlayerPos, &vPos, m_fAggroDistance))
            return WALK;

        return IDLE;
        break;
    }
    case SWING:
        // 공격 모션이 끝났을 때
        if (m_bAttackSuccess)
        {
            return IDLE;       
        }
        break;
    }
    return m_eState;
}

void CAzeos::Pattern_Teleport()
{
    _vec3 vPlayerPos;

    _int iRand = rand() % 5;

    if (iRand <= 3) // 75% 확률?
    {
        m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

        float randomAngle = static_cast<float>(rand()) / RAND_MAX * 360.0f;

        _float fX = vPlayerPos.x + 5.f * cosf(D3DXToRadian(randomAngle));
        _float fZ = vPlayerPos.z + 5.f * sinf(D3DXToRadian(randomAngle));

        m_pTransformCom->Set_Pos(fX, 2.6f, fZ);
        //플레이어와 일정한 거리 안에서 랜덤하게 텔레포트
    }

    else if (iRand == 4) // 25% 확률?
    {
        m_bCrystal = true;
        //크리스탈 쪽으로 
    }
}

void CAzeos::Pattern_GenerateThunder()
{
    _int iRandom = rand() % 3;

    while (true)
    {
        if (iRandom == 0 && m_iRandom != iRandom)  // 이전에 나왔던 수와 같으면 실행 안되게함
        {
            Generate_Line();

            m_iRandom = iRandom;
            return;
        }
        else if (iRandom == 1 && m_iRandom != iRandom)
        {
            Generate_Circle();

            m_iRandom = iRandom;
            return;
        }
        else if (iRandom == 2 && m_iRandom != iRandom)
        {
            Generate_Random();

            m_iRandom = iRandom;
            return;
        }

        iRandom = rand() % 3;
    }
}

//플레이어 중심으로 일렬로 20개 생성
void CAzeos::Generate_Line()
{
    _vec3 vPlayerPos;
    m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

    _vec3 vThunderPos[15];

    _float fFirstZPos = vPlayerPos.z + 5.f;

    for (int i = 0; i < 7; i++)
    {
        vThunderPos[i].z = fFirstZPos - (i * 2.f);
        vThunderPos[i].x = vPlayerPos.x - 3.f;

        vThunderPos[i].y = vPlayerPos.y;
    }

    for (int i = 7; i < 15; i++)
    {
        vThunderPos[i].z = fFirstZPos - ((i + 1) * 2.f);
        vThunderPos[i].x = vPlayerPos.x + 3.f;

        vThunderPos[i].y = vPlayerPos.y;
    }

    CGameObject* pThunder;

    CScene* pScene = Engine::Get_Scene();
    for (int i = 0; i < 15; i++)
    {
        pThunder = CThunder::Create(m_pGraphicDev, vThunderPos[i], CThunder::THUNDER_LINE);
        NULL_CHECK(pThunder);
        m_vecProjectileName.push_back(L"Monster_Created_Thunder" + std::to_wstring(m_iTagNumber++));
        FAILED_CHECK_RETURN(pScene->Create_GameObject(L"Layer_GameLogic", pThunder, m_vecProjectileName.back().c_str()), );
    }
}

// 플레이어 중심으로 랜덤한 거리에 20개 생성
void CAzeos::Generate_Random()
{
    m_bAttackSuccess = false;

    _vec3 vPlayerPos;
    m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

    // 플레이어 주위 위치 10개 지정하기
    _vec3 vFirePos[15];
    float fRadius = 7.0f;  // 플레이어 주변의 거리(반경)를 설정
    float fMinRadius = 2.0f; // 최소 반경 설정



    for (int i = 0; i < 15; i++)
    {
        // 랜덤 반경(최소 반경과 최대 반경 사이의 값)
        float randomRadius = fMinRadius + static_cast<float>(rand()) / RAND_MAX * (fRadius - fMinRadius);

        // 랜덤 각도 (0 ~ 360도 사이)
        float randomAngle = static_cast<float>(rand()) / RAND_MAX * 360.0f;


        // 각도와 반경을 기반으로 X, Z 좌표 계산
        vFirePos[i].x = vPlayerPos.x + randomRadius * cosf(D3DXToRadian(randomAngle));
        vFirePos[i].z = vPlayerPos.z + randomRadius * sinf(D3DXToRadian(randomAngle));

        // Y 좌표는 플레이어의 Y 좌표와 동일하게 설정
        vFirePos[i].y = vPlayerPos.y;
    }

    CGameObject* pThunder;

    CScene* pScene = Engine::Get_Scene();
    for (int i = 0; i < 15; i++)
    {
        pThunder = CThunder::Create(m_pGraphicDev, vFirePos[i], CThunder::THUNDER_RANDOM);
        NULL_CHECK(pThunder);
        m_vecProjectileName.push_back(L"Monster_Created_Thunder" + std::to_wstring(m_iTagNumber++));
        FAILED_CHECK_RETURN(pScene->Create_GameObject(L"Layer_GameLogic", pThunder, m_vecProjectileName.back().c_str()), );
    }
}

void CAzeos::Generate_Circle()
{
    m_bAttackSuccess = false;

    _vec3 vPlayerPos;
    m_pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

    // 플레이어 주위 위치 20개 지정하기
    _vec3 vFirePos[15];
    float fRadius = 10.0f;  // 플레이어 주변의 거리(반경)를 설정

    for (int i = 0; i < 15; i++)
    {

        // 랜덤 각도 (0 ~ 360도 사이)
        float randomAngle = static_cast<float>(rand()) / RAND_MAX * 360.0f;


        // 각도와 반경을 기반으로 X, Z 좌표 계산
        vFirePos[i].x = vPlayerPos.x + fRadius * cosf(D3DXToRadian(randomAngle));
        vFirePos[i].z = vPlayerPos.z + fRadius * sinf(D3DXToRadian(randomAngle));

        // Y 좌표는 플레이어의 Y 좌표와 동일하게 설정
        vFirePos[i].y = vPlayerPos.y;
    }

    CGameObject* pThunder;

    CScene* pScene = Engine::Get_Scene();
    for (int i = 0; i < 15; i++)
    {
        pThunder = CThunder::Create(m_pGraphicDev, vFirePos[i], CThunder::THUNDER_CIRCLE);
        NULL_CHECK(pThunder);
        m_vecProjectileName.push_back(L"Monster_Created_Fireball" + std::to_wstring(m_iTagNumber++));
        FAILED_CHECK_RETURN(pScene->Create_GameObject(L"Layer_GameLogic", pThunder, m_vecProjectileName.back().c_str()), );
    }
}

void CAzeos::Pattern_GenerateCrystal()
{
    m_bAttackSuccess = false;

    // 보스 처음생성 위치로 부터 랜덤한 거리에 3개 지정하기
    _vec3 vFirePos[3];
    float fRadius = 10.0f;  // 플레이어 주변의 거리(반경)를 설정
    float fMinRadius = 5.0f; // 최소 반경 설정

    for (int i = 0; i < 3; i++)
    {
        // 랜덤 반경(최소 반경과 최대 반경 사이의 값)
        float randomRadius = fMinRadius + static_cast<float>(rand()) / RAND_MAX * (fRadius - fMinRadius);

        // 랜덤 각도 (0 ~ 360도 사이)
        float randomAngle = static_cast<float>(rand()) / RAND_MAX * 360.0f;

        // 각도와 반경을 기반으로 X, Z 좌표 계산
        vFirePos[i].x = m_vFirstPos.x + randomRadius * cosf(D3DXToRadian(randomAngle));
        vFirePos[i].z = m_vFirstPos.z + randomRadius * sinf(D3DXToRadian(randomAngle));

        // Y 좌표는 플레이어의 Y 좌표와 동일하게 설정
        vFirePos[i].y = 0.5f;
    }

    CGameObject* pCrystal;

    CScene* pScene = Engine::Get_Scene();
    for (int i = 0; i < 3; i++)
    {
        pCrystal = CCrystal::Create(m_pGraphicDev, vFirePos[i]);
        NULL_CHECK(pCrystal);
        m_vecProjectileName.push_back(L"Crystal_" + std::to_wstring(m_iCrystalNumber++));
        FAILED_CHECK_RETURN(pScene->Create_GameObject(L"Layer_GameLogic", pCrystal, m_vecProjectileName.back().c_str()), );
    }
}

CAzeos* CAzeos::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CAzeos* pSlime = new CAzeos(pGraphicDev);

    if (FAILED(pSlime->Ready_GameObject(vPos)))
    {
        Safe_Release(pSlime);
        MSG_BOX("Mushroom Create Failed");
        return nullptr;
    }
    return pSlime;
}

void CAzeos::Free()
{
    Engine::CGameObject::Free();
}