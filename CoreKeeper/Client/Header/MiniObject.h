#pragma once
#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CTransform;
class CTexture;
class CObjectTex;

END


class CMiniObject : public Engine::CGameObject
{
private:
    explicit												CMiniObject(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual													~CMiniObject();

public:
    virtual			HRESULT									Ready_GameObject(_vec3 vPos);
    virtual			_int									Update_GameObject(const _float& fTimeDelta);
    virtual			void									LateUpdate_GameObject();
    virtual			void									Render_GameObject();

private:
    HRESULT													Add_Component();

private:
    Engine::CTransform* m_pTransformCom;
    Engine::CTexture* m_pTextureCom;
    Engine::CObjectTex* m_pBufferCom;

public:
    static CMiniObject* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

private:
    virtual void											Free();
};

