#pragma once
#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CTransform;

END

class CUIFont : public Engine::CGameObject
{
private:
	typedef struct FONTPOS{
		_vec2 vPos;
		wstring tFont;
		_float fCount;

		_bool bDead;
	}FONT;
private:
	explicit CUIFont(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUIFont();

public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

public:
	void Set_Font(_matrix matWorld, const _tchar* tFont);
	void Set_Font_Center(_matrix matWorld, const _tchar* tFont);
	void Set_Font_Up(_matrix matWorld, const _tchar* tFont);
private:
	HRESULT			Add_Component();

private:
	_vec3 vUIPos;
	_matrix m_UIMatrix;

	vector<FONT> m_vecFontPos;

	_int m_iFontCount;

	const _tchar* m_tFont[100];
private:

	Engine::CTransform* m_pTransformCom;
public:
	static CUIFont* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};
