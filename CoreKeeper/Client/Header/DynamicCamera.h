#pragma once

#include "Camera.h"
#include "Base.h"
#include "Define.h"


BEGIN(Engine)

class CTransform;

END
class CDynamicCamera : public CCamera
{
private:
	explicit CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDynamicCamera();

public:
	HRESULT			Ready_GameObject(const _vec3* pEye,
		const _vec3* pAt,
		const _vec3* pUp,
		const _float& _fFov,
		const _float& _fAspect,
		const _float& _fNear,
		const _float& _fFar);

	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject() {}

public:
	void							Shaking(_float fDuration, _float fPower);
	//_bool							Get_IsWorldMap() { return m_bIsWorldMap; }
private:
	void							Key_Input();
	void							Mouse_Move();
	void							Mouse_Fix();

	void							Zoom_In();
	void							Zoom_Out();

private:
	_bool			m_bFix;
	_bool			m_bIsWorldMap;
	CTransform*		m_pTargetTransform;
	_bool			m_bTargetPlayer;

public:
	static CDynamicCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev,
		const _vec3* pEye,
		const _vec3* pAt,
		const _vec3* pUp,
		const _float& _fFov = D3DXToRadian(60.f),
		const _float& _fAspect = (float)WINCX / WINCY,
		const _float& _fNear = 0.1f,
		const _float& _fFar = 1000.f);

public:
	_bool					Get_IsWorldMap() { return m_bIsWorldMap; }
	void					Set_IsWorldMap(_bool _bIsWorldMap) { m_bIsWorldMap = _bIsWorldMap; }

	float					Get_ZoomRatio() { return D3DXToRadian(60.0f) / m_fFov; }

private:
	virtual void	Free();

};

