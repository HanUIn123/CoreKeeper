#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera : public CGameObject
{
protected:
	explicit CCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCamera();

public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();

public:
	void            Set_Render(RENDERTYPE _eType); // 렌더 타입 결정

	_matrix			Get_ViewMat() { return m_curView; }
	_matrix			Get_ProjMat() { return m_curProj; }

protected:
	_vec3			m_vEye, m_vAt, m_vUp;
	_float			m_fFov, m_fAspect, m_fNear, m_fFar;
	_matrix			m_matView, m_matProj;

	_matrix			m_curView, m_curProj;

protected:
	virtual			void		Free();

};

END