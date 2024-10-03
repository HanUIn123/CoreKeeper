#pragma once
#include "Object.h"

BEGIN(Engine)

class CObjectTex;
class CTransform;
class CCalculator;
class CTexture;
class CInventory;
class CColliderCube;
class CAnimTex;
class CAnimator;

END

class CFire : public CObject
{
private:
	explicit						CFire(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual							~CFire();
public:
	virtual			HRESULT			Ready_GameObject(_vec3 vPos);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

	virtual			void			Interaction();


	virtual			_int			Get_BuildImgNum() { return m_iBuildingImgNum; }
	virtual			void			Set_BuildImgNum(_int _iBuildingNum) { m_iBuildingImgNum = _iBuildingNum; }

	virtual			_vec3			Get_ObjectPos() { return m_vBuildPosition; }
	virtual			void			Set_ObjectPos(_vec3 _iBuildingPos) { m_vBuildPosition = _iBuildingPos; }

	void							Set_Burn();
private:
	HRESULT							Add_Component();

private:
	Engine::CAnimTex* m_pAnimBufferCom;
	Engine::CAnimator* m_pAnimatorCom;

public:
	static CFire* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

private:
	virtual void					Free();

private:
	_bool m_bCollision;
	STATE m_eState;
	float	  m_fBurnTime;
	bool  m_bActive;
	bool  m_bBurn;
};

