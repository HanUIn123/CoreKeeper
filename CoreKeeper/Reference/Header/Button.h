#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CButton : public CGameObject
{
protected:
	explicit CButton(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CButton();

public:
	virtual			HRESULT			Ready_GameObject(_matrix* matWorld, _vec2 vPos, _vec2 vSize );

public:
	bool             Picked(POINT screenPos) { return ::PtInRect(&_rect, screenPos); }
	 
protected:
	virtual			void		Free();

private:
	RECT _rect;

};

END