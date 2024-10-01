#pragma once
#include "Item.h"
class CMagic : public CItem
{
private:
	explicit CMagic(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMagic();

public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();

public:
	void			Set_Dir(_vec3 vDir) { m_vDir = vDir; }

private:
	_vec3			m_vDir;
	_float			m_fTime;
	_float			m_fDistance;

public:
	static CMagic* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

