#pragma once
#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;

END

class CBlackPlane : public CGameObject
{
private:
	explicit						CBlackPlane(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual							~CBlackPlane();

public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

	void							StartFadeIn();  
	void							StartFadeOut(); 

	_int							Get_Opacity() { return m_iOpacity; }

private:
	HRESULT							Add_Component();

private:
	Engine::CRcTex*					m_pBufferCom;
	Engine::CTexture*				m_pTextureCom;
	Engine::CTransform*				m_pTransformCom;

public:
	static CBlackPlane*				Create(LPDIRECT3DDEVICE9 pGraphicDev);

	_int							Fade_In();
	_int							Fade_Out();
	void							Set_Opacity(_int _iOpacity);
	_bool							Get_FadeOver() { return m_bFadeOver; }


private:
	virtual void					Free();

	_int							m_iCount;
	_int							m_iOpacity;
	_bool							m_bFadeOver;

};

