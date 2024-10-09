#include "pch.h"
#include "../Header/MiniTerrain.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "../../Client/Header/Player.h"

CMiniTerrain::CMiniTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CGameObject(pGraphicDev)
    , m_iMiniTerrainIndex(0)
    , m_fLightRange(0)
{
    m_vecTextureNumber.resize((VTXCNTX - 1) * (VTXCNTZ - 1));

    m_vecIlluminated.resize((VTXCNTX - 1) * (VTXCNTZ - 1), false);
}

CMiniTerrain::~CMiniTerrain()
{
}

HRESULT CMiniTerrain::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
    return S_OK;
}

_int CMiniTerrain::Update_GameObject(const _float& fTimeDelta)
{
    Add_RenderGroup(RENDER_MAP, this);
    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CMiniTerrain::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CMiniTerrain::Render_GameObject()
{
    CTransform* pPlayerTransform = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Transform"));

    if (pPlayerTransform)
    {
        _vec3 vPlayerPos;
        pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);

        //int idx = i * (VTXCNTX - 1) + j;
        m_iMiniTerrainIndex = _int((_int(vPlayerPos.z) * (VTXCNTX - 1) + vPlayerPos.x));

        _float fPlayerLightRange = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player"))->Get_LightRange();
   
        // 0.15f 원래 terrain에서 미니맵은 좀 줄은 비율이니 0.15일단 곱해줌
        Illuminate_TerrainIndex(m_iMiniTerrainIndex, fPlayerLightRange * 0.15f);
    }

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

    D3DMATERIAL9		backGroundMaterial;
    ZeroMemory(&backGroundMaterial, sizeof(D3DMATERIAL9));

    backGroundMaterial.Diffuse = D3DXCOLOR(0, 0, 0, 1);
    backGroundMaterial.Specular = { 0.f, 0.f, 0.f, 1.f };
    backGroundMaterial.Ambient = { 0.f, 0.f, 0.f, 1.f };
    backGroundMaterial.Emissive = { 0.f, 0.f, 0.f, 1.f };
    backGroundMaterial.Power = 0.f;

    
    for (int iIndex = 0; iIndex < m_vecIlluminated.size(); ++iIndex)
    {
        if (!m_vecIlluminated[iIndex])
        {
            m_pGraphicDev->SetMaterial(&backGroundMaterial);
            m_pGraphicDev->SetTexture(0, nullptr);
            m_pBufferCom->Render_Texture(iIndex * 6);
        }
    }

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CMiniTerrain::Add_Component()
{
    CComponent* pComponent = NULL;

    pComponent = m_pBufferCom = dynamic_cast<CTerrainTex*>(Engine::Clone_Proto(L"Proto_TerrainTex"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_TerrainTexture"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    pComponent = m_pCalculatorCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
    NULL_CHECK_RETURN(pComponent, E_FAIL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Calculator", pComponent });

    return S_OK;
}

void CMiniTerrain::Illuminate_TerrainIndex(_int _iIndex, _float _fLightRange)
{
    int iOriginPlayerX = _iIndex % (VTXCNTX - 1);
    int iOriginPlayerZ = _iIndex / (VTXCNTX - 1);

    // 원형 꼴로 비춰야하니, -길이 ~ +길이 해서, 위아래좌우 인덱스 검사 
    for (int i = -_fLightRange; i <= _fLightRange; ++i)
    {
        for (int j = -_fLightRange; j <= _fLightRange; ++j)
        {
            int iResultPlayerX = iOriginPlayerX + i;
            int iResultPlayerZ = iOriginPlayerZ + j;
            if (iResultPlayerX >= 0 && iResultPlayerX < VTXCNTX - 1 && iResultPlayerZ >= 0 && iResultPlayerZ < VTXCNTZ - 1)
            {
                int iIndex = iResultPlayerZ * (VTXCNTX - 1) + iResultPlayerX;

                // 조명 범위 안에 있으면 true로 해서 밝히자.
                if (sqrtf((i * i) + (j * j)) <= _fLightRange)
                {
                    m_vecIlluminated[iIndex] = true;
                }
            }
        }
    }

    // 랜턴 같은거 없이 다닐 때, 
    if (_iIndex >= 0 && _iIndex <= m_vecIlluminated.size())
    {
        m_vecIlluminated[_iIndex] = true;
        if(_iIndex + 1 < m_vecIlluminated.size())
            m_vecIlluminated[_iIndex + 1] = true;
        if (_iIndex + VTXCNTX < m_vecIlluminated.size())
            m_vecIlluminated[_iIndex + VTXCNTX] = true;
    }
}

CMiniTerrain* CMiniTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CMiniTerrain* pMiniTerrain = new CMiniTerrain(pGraphicDev);

    if (FAILED(pMiniTerrain->Ready_GameObject()))
    {
        Safe_Release(pMiniTerrain);
        MSG_BOX("pMiniTerrain Create Failed");
        return nullptr;
    }
    return pMiniTerrain;
}

void CMiniTerrain::Free()
{
    Engine::CGameObject::Free();
}
