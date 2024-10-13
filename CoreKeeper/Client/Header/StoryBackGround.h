#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;

END

class CStoryBackGround : public Engine::CGameObject
{
private:
	explicit CStoryBackGround(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStoryBackGround();

public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

	int								Get_TextureNum() { return m_iTextureNum; }
	bool							Get_End() { return m_bEnd; }
private:
	HRESULT			Add_Component();

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;

public:
	static CStoryBackGround* Create(LPDIRECT3DDEVICE9 pGraphicDev);

	bool				Get_Stop() { return m_bStop; }
	void				Set_Stop(bool _bStop) { m_bStop = _bStop; }

private:
	virtual void		Free();

private:
	wstring				m_strMent[5];
	int					m_iTextureNum;

	float				m_fElapsedTime;
	int					m_iCurrentIndex;


	bool				m_bStop;
	bool				m_bEnd;
};