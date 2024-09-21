#pragma once
#include "Building.h"

BEGIN(Engine)

class CObjectTex;
class CTransform;
class CCalculator;


END


class CCore : public CBuilding
{
private:
	explicit						CCore(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual							~CCore();
public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT							Add_Component();

public:
	static CBuilding* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void					Free();
};

