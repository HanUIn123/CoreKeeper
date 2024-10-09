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

class CTorchObject : public CObject
{
private:
	explicit						CTorchObject(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual							~CTorchObject();
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
	HRESULT							Add_Component();
	void							SetUp_Light();

private:
	Engine::CAnimTex* m_pAnimBufferCom;
	Engine::CAnimator* m_pAnimatorCom;

public:
	static CTorchObject* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

private:
	virtual void					Free();

private:
	_int			m_iLightNum;
	bool			m_bInFrustum;
};

