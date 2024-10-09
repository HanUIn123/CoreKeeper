#pragma once
#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CTerrainTex;
class CTransform;
class CTexture;
class CCalculator;

END

class CMiniTerrain : public Engine::CGameObject
{
private:
    explicit												CMiniTerrain(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual													~CMiniTerrain();

public:
    virtual			HRESULT									Ready_GameObject();
    virtual			_int									Update_GameObject(const _float& fTimeDelta);
    virtual			void									LateUpdate_GameObject();
    virtual			void									Render_GameObject();

private:
    HRESULT													Add_Component();

public:
    void                                                    Illuminate_TerrainIndex(_int _iIndex, _float _fLightRange);
    bool                                                    Get_IlluminatedIndex(_int _iIndex) { return m_vecIlluminated[_iIndex]; }
private:
    Engine::CTransform* m_pTransformCom;
    Engine::CTexture* m_pTextureCom;
    Engine::CTerrainTex* m_pBufferCom;
    Engine::CCalculator* m_pCalculatorCom;

    Engine::CGameObject* m_pPlayerObject;

public:
    static CMiniTerrain* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    virtual void											Free();

private:
    vector<_int>                                            m_vecTextureNumber;
    vector<_bool>                                           m_vecIlluminated;

    _int                                                    m_iMiniTerrainIndex;
    _float                                                  m_fLightRange;
};

