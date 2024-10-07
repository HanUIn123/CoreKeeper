#pragma once
#include "Object.h"

BEGIN(Engine)

class CObjectTex;
class CTransform;
class CCalculator;
class CTexture;
class CColliderCube;
class CInventory;
class CLightBuffer;

END

class CSpotlite : public CObject
{
private:
	explicit						CSpotlite(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual							~CSpotlite();
public:
	virtual			HRESULT			Ready_GameObject(float m_fRange, _vec3 vPos);
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
	Engine::CLightBuffer* m_pLightBufferCom;

public:
	static CSpotlite* Create(LPDIRECT3DDEVICE9 pGraphicDev, float m_fRange, _vec3 vPos);

private:
	virtual void					Free();

private:
	int								m_iLightNum;
	float							m_fRange;
};

