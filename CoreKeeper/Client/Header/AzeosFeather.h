#pragma once
#include "Object.h"
class CAzeosFeather :
    public CObject
{
private:
	explicit    CAzeosFeather(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual     ~CAzeosFeather();

public:
	virtual			HRESULT			Ready_GameObject(_vec3 _vPos, _int _iTypeNum, const wstring _pickedSFName);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

	_int							Get_ObjectType() { return AZEOS_FEATHER; }

	_int							Get_TileTypeIndex() { return m_iFeatherTypeIndex; }
	void							Set_TileTypeIndex(_int _iTileTypeIndex) { m_iFeatherTypeIndex = _iTileTypeIndex; }

	_vec3							Get_PoopPos() { return m_vPoopPos; }
private:
	HRESULT							Add_Component();
public:
	static CAzeosFeather*			Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos, _int _iTypeNum, const wstring _pickedSFName);

private:
	virtual void					Free();
	_int							m_iFeatherTypeIndex;
	_vec3							m_vPoopPos;
};

