#pragma once
#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CTransform;
class CTexture;
class CObjectTex;
class CHit;

END
class CMiniSpawn : public Engine::CGameObject
{
private:
    explicit												CMiniSpawn(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual													~CMiniSpawn();

public:
    virtual			HRESULT									Ready_GameObject(_vec3 vPos);
    virtual			_int									Update_GameObject(const _float& fTimeDelta);
    virtual			void									LateUpdate_GameObject();
    virtual			void									Render_GameObject();

private:
    HRESULT													Add_Component();

    _bool                                                   m_bRevealed;

    _matrix                                                 m_TeleportWorld;
    _matrix                                                 m_matEffectWorld;
    _bool                                                   m_bTeleport;

private:
    Engine::CTransform*                                     m_pTransformCom;
    Engine::CTexture*                                       m_pTextureCom;
    Engine::CObjectTex*                                     m_pBufferCom;
    Engine::CHit*                                           m_pHitEffectCom;

public:
    static CMiniSpawn*                                      Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);
    _bool                                                   Piking_Teleport();
    _bool                                                   Check_PlayerPos(_bool _bRevealed);

private:
    virtual void											Free();

};

