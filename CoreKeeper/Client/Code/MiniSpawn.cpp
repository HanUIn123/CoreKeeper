#include "pch.h"
#include "../Header/MiniSpawn.h"
#include "Export_System.h"
#include "Export_Utility.h"

CMiniSpawn::CMiniSpawn(LPDIRECT3DDEVICE9 pGraphicDev)
    :Engine::CGameObject(pGraphicDev)
    , m_pTransformCom(nullptr)
    , m_pTextureCom(nullptr)
    , m_bRevealed(false)
{
}

CMiniSpawn::~CMiniSpawn()
{
}

HRESULT CMiniSpawn::Ready_GameObject(_vec3 vPos)
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

    m_pTransformCom->Set_Scale(5.0f, 0.0f, 5.0f);

    return S_OK;
}

_int CMiniSpawn::Update_GameObject(const _float& fTimeDelta)
{
    Check_PlayerPos(m_bRevealed);

    Add_RenderGroup(RENDER_MAP, this);

    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CMiniSpawn::LateUpdate_GameObject()
{
    Piking_Teleport();

    Engine::CGameObject::LateUpdate_GameObject();
}

void CMiniSpawn::Render_GameObject()
{

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

    return S_OK;
}

_bool CMiniSpawn::Piking_Teleport()
{
    bool bClicked = false;

    CTransform* pPlayerTransform = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));

    _vec3 vTeleportPos;
    m_pTransformCom->Get_Info(INFO_POS, &vTeleportPos);

    POINT ptMouse;
    GetCursorPos(&ptMouse);

    RECT windowRect;
    GetWindowRect(g_hWnd, &windowRect);

    ptMouse.x -= windowRect.left;
    ptMouse.y -= windowRect.top;

    _vec2 spawnInMapnPos[3] =
    {
       { 533, 425 },
       { 649, 570 },
       { 839, 381 }
    };

    _vec3 spawnInGamedPos[3] =
    {
        { VTXCNTX / 2 + 65.f, 0.1f, 21.5f + 80.f },
        { VTXCNTX / 2, 0.1f, 17.f },
        { VTXCNTX / 2 - 40.f, 0.1f, 21.5f + 60.f }
    };

    for (int i = 0; i < 3; ++i)
    {
        if (abs(ptMouse.x - spawnInMapnPos[i].x) <= 5 && abs(ptMouse.y - spawnInMapnPos[i].y) <= 5)
        {
            if (CRenderer::GetInstance()->Get_ExpandMap())
                m_pTransformCom->Set_Scale(10.0f, 0.0f, 10.0f);
            else
                m_pTransformCom->Set_Scale(5.0f, 0.0f, 5.0f);

            if (Engine::Get_DIMouseState(DIM_RB) & 0x80 && !bClicked)
            {
                _matrix matWorld;
                pPlayerTransform->Get_WorldMatrix(&matWorld);

                if (i == 0)
                {
                    matWorld._41 = spawnInGamedPos[2].x;
                    matWorld._42 = spawnInGamedPos[2].y;
                    matWorld._43 = spawnInGamedPos[2].z;
                }
                else if (i == 1)
                {
                    matWorld._41 = spawnInGamedPos[1].x;
                    matWorld._42 = spawnInGamedPos[1].y;
                    matWorld._43 = spawnInGamedPos[1].z;
                }
                else
                {
                    matWorld._41 = spawnInGamedPos[0].x;
                    matWorld._42 = spawnInGamedPos[0].y;
                    matWorld._43 = spawnInGamedPos[0].z;
                }

                pPlayerTransform->Set_WorldMatrix(&matWorld);

                bClicked = true;
            }
            else if (!(Engine::Get_DIMouseState(DIM_RB) & 0x80))
            {
                bClicked = false;
            }
            return true;
        }
    }
    m_pTransformCom->Set_Scale(5.0f, 0.0f, 5.0f);

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

        if (fDistance <= 20.0f)
        {
            m_bRevealed = true;
        }
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
