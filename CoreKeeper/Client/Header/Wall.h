#pragma once
#include "GameObject.h"
#include "Define.h"
#include "Export_System.h"

BEGIN(Engine)

class CTransform;
class CTexture;
class CCalculator;
class CWallTex;
class CColliderCube;

END

class CWall : public Engine::CGameObject
{
private:
    explicit												CWall(LPDIRECT3DDEVICE9 pGraphicDev);
public:
    virtual													~CWall();

public:
    virtual			HRESULT									Ready_GameObject(_float _fWallX, _float _fWallZ, _int iWallImageNum, const wstring _pickedWallName);
    virtual			_int									Update_GameObject(const _float& fTimeDelta);
    virtual			void									LateUpdate_GameObject();
    virtual			void									Render_GameObject();

    _vec3													Get_WallPos() { return m_vWallPosition; }
    void													Set_WallPos(_vec3 _iWallPos) { m_vWallPosition = _iWallPos; }

    _int													Get_WallNumber() { return m_iWallImageNum; }
    void													Set_WallNumber(_int _iWallNum) { m_iWallImageNum = _iWallNum; }

    // Delete_GameObject 함수를 사용하려면, 키값이 필요한데, 그 키값(이름)을 받아올 함수
    wstring													Get_PickedWallName() { return m_strPickedWallName; }

    _int                                                    Get_PikingIndex() { return m_iPikingWallIndex; }
    void                                                    Set_PikingIndex(_int _iPikingIndex) { m_iPikingWallIndex = _iPikingIndex; }


    _bool                                                   Get_IsUpWall() { return m_bIsUpWall; }
    void                                                    Set_IsUpWall(_bool _bIsUpWall) { m_bIsUpWall = _bIsUpWall; }

    void                                                    Add_WallArray(int _iIndex, CWall* _pWall) { m_vecAroundWall[_iIndex] = _pWall; }
    vector<CWall*>                                          Get_Around() { return m_vecAroundWall; }
private:
    HRESULT													Add_Component();
    HRESULT                                                 Setup_Material();

    void                                                    Update_Texture();
private:
    Engine::CTransform*                                     m_pTransformCom;
    Engine::CTexture*                                       m_pTextureCom;
    Engine::CCalculator*                                    m_pCalculatorCom;
    Engine::CWallTex*                                       m_pBufferCom;
    Engine::CColliderCube*                                  m_pColliderCom;
public:
    static CWall* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _fWallX, _float _fWallZ, _int iWallImageNum, const wstring _pickedWallName);

private:
    virtual void											Free();

    _vec3													m_vWallPosition;
    _int													m_iWallImageNum;

    wstring											        m_strPickedWallName;
    _int                                                    m_iPikingWallIndex;

    _bool                                                   m_bIsUpWall;

    vector<CWall*>                                          m_vecAroundWall;
private:
    bool    m_bActive;
};

