#pragma once
#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;

END

class CWhitePlane : public CGameObject
{
private:
	explicit						CWhitePlane(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual							~CWhitePlane();

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
	Engine::CRcTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CTransform* m_pTransformCom;

public:
	static CWhitePlane* Create(LPDIRECT3DDEVICE9 pGraphicDev);

	_int							Fade_In();
	_int							Fade_Out();
	void							Set_Opacity(_int _iOpacity);
	_bool							Get_FadeOver() { return m_bFadeOver; }
	void							Set_FadeOver(bool _bFadeOver) { m_bFadeOver = _bFadeOver; }

	_bool							Get_Stop() { return m_bStop; }
	void							Set_Stop(_bool _bStop) { m_bStop = _bStop; }


private:
	virtual void					Free();

	_int							m_iCount;
	_int							m_iOpacity;
	_bool							m_bFadeOver;
	_bool							m_bStop;
};

