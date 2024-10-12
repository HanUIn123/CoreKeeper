#include "pch.h"
#include "../Header/MiniSpawn.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "../Header/BlackPlaneMgr.h"
#include "../Header/Player.h"

CMiniSpawn::CMiniSpawn(LPDIRECT3DDEVICE9 pGraphicDev)
    :Engine::CGameObject(pGraphicDev)
    , m_pTransformCom(nullptr)
    , m_pTextureCom(nullptr)
    , m_bRevealed(false)
    , m_bTeleport(false)
{
}

CMiniSpawn::~CMiniSpawn()
{
}

HRESULT CMiniSpawn::Ready_GameObject(_vec3 vPos)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

    m_pTransformCom->Set_Scale(3.5f, 0.0f, 3.5f);

    m_pHitEffectCom->init(L"../Bin/Resource/Texture/Effect/PlayerTeleport/Teleport_%d.png", 12, 3.5f);

    return S_OK;
}

_int CMiniSpawn::Update_GameObject(const _float& fTimeDelta)
{
    if (CBlackPlaneMgr::GetInstance()->Is_FadeOutComplete())
    {
    }

    Check_PlayerPos(m_bRevealed);

    Engine::Add_RenderGroup(RENDER_MAP, this);


    if (m_bTeleport)
    {
        m_pHitEffectCom->update(fTimeDelta);

        if (m_pHitEffectCom->isDead())
        {
            m_pHitEffectCom->reset();

            CTransform* pPlayerTransform = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));

            CBlackPlaneMgr::GetInstance()->StartFadeOut();

            pPlayerTransform->Set_WorldMatrix(&m_TeleportWorld);

            m_bTeleport = false;
        }

        Engine::Add_RenderGroup(RENDER_ALPHA, this);
    }

    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CMiniSpawn::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CMiniSpawn::Render_GameObject()
{
    if (m_bTeleport)
    {
        m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_matEffectWorld);

        m_pHitEffectCom->render();

        return;
    }


    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    if (m_bRevealed)
    {
        m_pTextureCom->Set_Texture();
        m_pBufferCom->Render_Buffer();
    }

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CMiniSpawn::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pBufferCom = dynamic_cast<CObjectTex*>(Engine::Clone_Proto(L"Proto_MiniSpawnTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MiniSpawn"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    pComponent = m_pHitEffectCom = dynamic_cast<CHit*>(Engine::Clone_Proto(L"Proto_Hit"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Effect", pComponent });

    return S_OK;
}

_bool CMiniSpawn::Piking_Teleport()
{
    bool bClicked = false;

    CTransform* pPlayerTransform = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));

    CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player"));

    _vec3 vTeleportPos;
    m_pTransformCom->Get_Info(INFO_POS, &vTeleportPos);

    _vec3 vPlayerPos;
    pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

    POINT ptMouse;
    GetCursorPos(&ptMouse);

    RECT windowRect;
    GetWindowRect(g_hWnd, &windowRect);

    ptMouse.x -= windowRect.left;
    ptMouse.y -= windowRect.top;

    _vec2 spawnInMapPos[3] =
    {
       { 817, 419 },
       { 678, 558 },
       { 547, 289 }
    };

    _vec3 spawnInGamePos[3] =
    {
        { 51.0f, 0.1f, 60.0f},
        {  VTXCNTX / 2, 0.1f, 7.f + 5.f },
        {14.0f, 0.1f, 105.0f}
    };

    for (int i = 0; i < 3; ++i)
    {
        if (abs(ptMouse.x - spawnInMapPos[i].x) <= 5 && abs(ptMouse.y - spawnInMapPos[i].y) <= 5)
        {
            if (CRenderer::GetInstance()->Get_ExpandMap())
                m_pTransformCom->Set_Scale(5.0f, 0.0f, 5.0f);
            else
                m_pTransformCom->Set_Scale(3.5f, 0.0f, 3.5f);

            if (Engine::Button_Down(DIM_LB))
            {
                pPlayer->Check_MapOff();

                _matrix matWorld;
                pPlayerTransform->Get_WorldMatrix(&matWorld);

                m_matEffectWorld = matWorld;

                if (i == 0)
                {
                    matWorld._41 = spawnInGamePos[0].x;
                    matWorld._42 = spawnInGamePos[0].y;
                    matWorld._43 = spawnInGamePos[0].z;
                }
                else if(i == 1)
                {
                    matWorld._41 = spawnInGamePos[1].x;
                    matWorld._42 = spawnInGamePos[1].y;
                    matWorld._43 = spawnInGamePos[1].z;
                }
                else
                {
                    matWorld._41 = spawnInGamePos[2].x;
                    matWorld._42 = spawnInGamePos[2].y;
                    matWorld._43 = spawnInGamePos[2].z;
                }

                m_TeleportWorld = matWorld;

               // pPlayerTransform->Set_WorldMatrix(&matWorld);
                m_bTeleport = true;

                bClicked = true;
            }
            else if (!(Engine::Get_DIMouseState(DIM_RB) & 0x80))
            {
                bClicked = false;
            }
            return true;
        }
    }
    m_pTransformCom->Set_Scale(3.5f, 0.0f, 3.5f);

    return false;
}

_bool CMiniSpawn::Check_PlayerPos(_bool _bRevealed)
{
    CTransform* pPlayerTransform = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));

    if (pPlayerTransform)
    {
        _vec3 vPlayerPos;
        pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

        _vec3 vSpawnPos;
        m_pTransformCom->Get_Info(INFO_POS, &vSpawnPos);

        _vec3 vResult = vPlayerPos - vSpawnPos;
        _float fDistance = D3DXVec3Length(&(vResult));

        if (fDistance <= 3.0f)
        {
            m_bRevealed = true;
            Piking_Teleport();
        }
        else
            m_pTransformCom->Set_Scale(3.5f, 0.0f, 3.5f);
    }

    return m_bRevealed;
}

CMiniSpawn* CMiniSpawn::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
    CMiniSpawn* pSpawn = new CMiniSpawn(pGraphicDev);

    if (FAILED(pSpawn->Ready_GameObject(vPos)))
    {
        Safe_Release(pSpawn);
        MSG_BOX("pSpawn Create Failed");
        return nullptr;
    }

    return pSpawn;
}

void CMiniSpawn::Free()
{
    Engine::CGameObject::Free();
}
