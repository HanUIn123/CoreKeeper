#pragma once
#include "Camera.h"
#include "Base.h"
#include "Define.h"

class CMiniMapCamera : public CCamera
{
private:
    explicit                                    CMiniMapCamera(LPDIRECT3DDEVICE9 _pGraphicDevice);
    virtual                                     ~CMiniMapCamera();

public:
    HRESULT Ready_GameObject(const _vec3* pEye,
        const _vec3* pAt, 
        const _vec3* pUp,
        const _float& _fFov, 
        const _float& _fAspect,
        const _float& _fNear, 
        const _float& _fFar);

    virtual _int                                Update_GameObject(const _float& _fTimeDelta);
    virtual void                                LateUpdate_GameObject();
    virtual void                                Render_GameObject() {}

public:
    void                                        Setting_MiniMapCamera(const _vec3* pEye, const _vec3* pAt, const _vec3* pUp);

    // 플레이어 갖고와서, vEye 플레이어한테 세팅예정
    //void                                        Target_PlayeR();

public:
    static CMiniMapCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev,
        const _vec3* pEye,
        const _vec3* pAt,
        const _vec3* pUp,
        const _float& _fFov = D3DXToRadian(60.f),
        const _float& _fAspect = (float)WINCX / WINCY,
        const _float& _fNear = 0.1f,
        const _float& _fFar = 1000.f);

private:
    virtual void Free();
};
