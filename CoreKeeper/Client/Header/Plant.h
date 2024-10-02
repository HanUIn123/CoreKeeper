#pragma once
#include "Object.h"

BEGIN(Engine)

class CObjectTex;
class CTransform;
class CCalculator;
class CTexture;
class CColliderCube;
class CInventory;
class CAnimTex;
class CAnimator;

END

class CPlant : public CObject
{
private:
	explicit						CPlant(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual							~CPlant();
public:
	virtual			HRESULT			Ready_GameObject(ITEMNUM _eItemNum, _vec3 vPos);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

	virtual			_int			Get_BuildImgNum() { return m_iBuildingImgNum; }
	virtual			void			Set_BuildImgNum(_int _iBuildingNum) { m_iBuildingImgNum = _iBuildingNum; }

	virtual			_vec3			Get_ObjectPos() { return m_vBuildPosition; }
	virtual			void			Set_ObjectPos(_vec3 _iBuildingPos) { m_vBuildPosition = _iBuildingPos; }

private:
	HRESULT							Add_Component();

private:
	Engine::CAnimTex* m_pAnimBufferCom;
	Engine::CAnimator* m_pAnimatorCom;

public:
	static CPlant* Create(LPDIRECT3DDEVICE9 pGraphicDev, ITEMNUM _eItemNum, _vec3 vPos);

private:
	virtual void					Free();
};

