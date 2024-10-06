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

class CStatue : public CObject
{
private:
	explicit						CStatue(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual							~CStatue();
public:
	virtual			HRESULT			Ready_GameObject(_vec3 vPosm, int iImgNum);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

	virtual         void            Interaction();

	virtual			_int			Get_BuildImgNum() { return m_iBuildingImgNum; }
	virtual			void			Set_BuildImgNum(_int _iBuildingNum) { m_iBuildingImgNum = _iBuildingNum; }

	virtual			_vec3			Get_ObjectPos() { return m_vBuildPosition; }
	virtual			void			Set_ObjectPos(_vec3 _iBuildingPos) { m_vBuildPosition = _iBuildingPos; }

private:
	HRESULT							Add_Component();
	void							SetUp_Light();

public:
	static CStatue* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, int iImgNum);

private:
	virtual void					Free();

private:
	int								m_iTextureNum;
	bool							m_bActive;
	int								m_iLightNum;

};

