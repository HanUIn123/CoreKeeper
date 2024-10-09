#pragma once
#include "Object.h"

BEGIN(Engine)

class CObjectTex;
class CTransform;
class CCalculator;
class CTexture;
class CColliderCube;
class CInventory;

END

class CCore : public CObject
{
private:
	explicit						CCore(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual							~CCore();
public:
	virtual			HRESULT			Ready_GameObject(_vec3 vPos = { 0.f, 0.f, 0.f });
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

	virtual         void            Interaction();

	virtual			_int			Get_BuildImgNum() { return m_iBuildingImgNum; }
	virtual			void			Set_BuildImgNum(_int _iBuildingNum) { m_iBuildingImgNum = _iBuildingNum; }

	virtual			_vec3			Get_ObjectPos() { return m_vBuildPosition; }
	virtual			void			Set_ObjectPos(_vec3 _iBuildingPos) { m_vBuildPosition = _iBuildingPos; }

	void							Set_ActiveCore(int _iIndex) { m_bActiveCore[_iIndex] = true; }
	bool							Get_ActiveCore(int _iIndex) { return m_bActiveCore[_iIndex]; }

private:
	HRESULT							Add_Component();
	void							SetUp_Light();

public:
	static CCore* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos = { 0.f, 0.f, 0.f });

private:
	virtual void					Free();

private:
	_int			m_iLightNum;
	bool			m_bActiveCore[3];

	int				m_iRange;
};

