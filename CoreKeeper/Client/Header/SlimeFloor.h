#pragma once
#include "Object.h"

BEGIN(Engine)

class CObjectTex;
class CTransform;
class CCalculator;
class CTexture;
class CCollider;
class CColliderCube;

END
class CSlimeFloor : public CObject
{
private:
    explicit    CSlimeFloor(LPDIRECT3DDEVICE9 _pGraphicDev);
    virtual     ~CSlimeFloor();

public:
	virtual			HRESULT			Ready_GameObject(_vec3 _vPos, _int _iTypeNum, const wstring _pickedFeatherName);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

	_int							Get_ObjectType() { return SLIME_FLOOR; }

	_int							Get_TileTypeIndex() { return m_iTileTypeIndex; }
	void							Set_TileTypeIndex(_int _iTileTypeIndex) { m_iTileTypeIndex = _iTileTypeIndex; }

	_vec3							Get_FloorPos() { return m_vFloorPos; }

private:
	HRESULT							Add_Component();
	void							Set_Cast();

public:
	static CSlimeFloor*				Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos, _int _iTypeNum, const wstring _pickedFeatherName);

private:
	virtual void					Free();

	CCollider*						m_pPlayerCollider;
	_int							m_iTileTypeIndex;
	_vec3							m_vFloorPos;
};

