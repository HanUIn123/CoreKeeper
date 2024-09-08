#pragma once

#include "Base.h"
#include "Engine_Struct.h"
#include "GraphicDev.h"

class CTriangle : public CBase
{
private:
    explicit CTriangle(CGraphicDev* _pDeviceClass);
    virtual ~CTriangle();

public:
    HRESULT Ready_Triangle();       
    int     Update_Triangle();
    void    LateUpdate_Triangle();
    void    Render_Triangle();        

private:    
    CGraphicDev*            m_pDeviceClass;    // 싱글톤 주소 저장용 포인터
    LPDIRECT3DDEVICE9       m_pGraphicDev;     // 실제 그리기 객체 주소

    LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;    // 정점 버퍼 포인터

public:    // 객체 생성 함수
    static CTriangle* Create(CGraphicDev* _pDeviceClass);

private:    // 객체 소멸 함수
    virtual void Free();
};
