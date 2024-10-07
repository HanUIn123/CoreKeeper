#pragma once
#include "Object.h"

BEGIN(Engine)

class CCollider;
class CObjectTex;
class CTransform;
class CCalculator;
class CTexture;
class CColliderCube;
class CInventory;

END

class CPlayer;
class CAzeos;
class CTerrain;
class CCrystal : public CObject
{
private:
	explicit						CCrystal(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual							~CCrystal();
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
	void		Set_Cast();

private:
	Engine::CCollider* m_pSphereColliderCom;

private:
	HRESULT							Add_Component();

public:
	static CCrystal* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

private:
	virtual void					Free();

private:
	CAzeos* m_pAzeos;
	CTransform* m_pAzeosTransform;
	CPlayer* m_pPlayer;
	CTransform* m_pPlayerTransform;
	CTerrain* m_pTerrain;

	_int m_iHp;
	_bool m_bDeadFirstFrame;
};

