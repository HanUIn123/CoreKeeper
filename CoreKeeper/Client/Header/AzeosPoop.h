#pragma once
#include "Object.h"
BEGIN(Engine)

class CObjectTex;
class CTransform;
class CTexture;
class CColliderCube;

END

class CAzeosPoop : public CObject
{
private:
	explicit    CAzeosPoop(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual     ~CAzeosPoop();

public:
	virtual			HRESULT			Ready_GameObject(_vec3 _vPos);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

	_int							Get_ObjectType() { return AZEOS_POOP; }

	_int							Get_TileTypeIndex() { return m_iTileTypeIndex; }
	void							Set_TileTypeIndex(_int _iTileTypeIndex) { m_iTileTypeIndex = _iTileTypeIndex; }
	
	_vec3							Get_PoopPos() { return m_vPoopPos; }
private:
	HRESULT							Add_Component();
public:
	static CAzeosPoop* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos);

private:
	virtual void					Free();
	_int							m_iTileTypeIndex;
	_vec3							m_vPoopPos;
};

