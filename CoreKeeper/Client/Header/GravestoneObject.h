#pragma once
#include "Object.h"

BEGIN(Engine)

class CObjectTex;
class CTransform;
class CCalculator;
class CTexture;
class CInventory;

END

class CGravestoneObject : public CObject
{
private:
	explicit						CGravestoneObject(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual							~CGravestoneObject();
public:
	virtual			HRESULT			Ready_GameObject(_vec3 vPos);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

	virtual			_int			Get_BuildImgNum() { return m_iBuildingImgNum; }
	virtual			void			Set_BuildImgNum(_int _iBuildingNum) { m_iBuildingImgNum = _iBuildingNum; }

	virtual			_vec3			Get_ObjectPos() { return m_vBuildPosition; }
	virtual			void			Set_ObjectPos(_vec3 _iBuildingPos) { m_vBuildPosition = _iBuildingPos; }

private:
	Engine::CInventory*				m_pInventoryCom;

private:
	HRESULT							Add_Component();

public:
	static CGravestoneObject* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

private:
	virtual void					Free();
};

