#pragma once
#include "Item.h"

class CArrow;

class CBow : public CItem
{
private:
	explicit CBow(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBow();

public:
	virtual			HRESULT			Ready_GameObject(_vec3 vPos);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();

private:
	wstring			m_strArrow;
	CArrow*			m_pArrow;
	CTransform*		m_pTransformArrow;
	_bool			m_bShot;

	static			_int		m_iArrowNum;
public:
	static CBow* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos = { 0.f, 0.f, 0.f });

private:
	virtual void		Free();
};
