#pragma once
#include "GameObject.h"
#include "../Client/Header/ImguiMgr.h"
#include "Export_Utility.h"
#include "Export_System.h"

BEGIN(Engine)

class CTransform;
class CTexture;
class CCalculator;
class CWallTex;
class CMapToolWallTex;
class CColliderCube;

END

class CMapToolWall : public Engine::CGameObject
{
private:
    explicit												CMapToolWall(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual													~CMapToolWall();

public:

public:
    virtual			HRESULT									Ready_GameObject(_float _fWallX, _float _fWallZ, _int _iType);
    virtual			_int									Update_GameObject(const _float& fTimeDelta);
    virtual			void									LateUpdate_GameObject();
    virtual			void									Render_GameObject();

    _vec3													Get_WallPos() { return m_vBigWallPosition; }
    void													Set_WallPos(_vec3 _iWallPos) { m_vBigWallPosition = _iWallPos; }


private:
    HRESULT													Add_Component();
    HRESULT                                                 Setup_Material();

private:
    Engine::CTransform* m_pTransformCom;
    Engine::CTexture* m_pTextureCom;
    Engine::CCalculator* m_pCalculatorCom;
    Engine::CWallTex* m_pBufferCom;
    Engine::CMapToolWallTex* m_pBufferCom2;
    Engine::CColliderCube* m_pColliderCom;
public:
    static CMapToolWall* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _fWallX, _float _fWallZ, _int _iType);

private:
    virtual void											Free();
    _vec3													m_vBigWallPosition;

    // 세로인지 가로인지 구별하는 int 변수
    _int                                                    m_iWallType;


};

