#include "FontMgr.h"
#include "..\..\Header\FontMgr.h"

IMPLEMENT_SINGLETON(CFontMgr)

CFontMgr::CFontMgr()
{
}


CFontMgr::~CFontMgr()
{
	Free();
}

HRESULT CFontMgr::Ready_Font(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * pFontTag, const _tchar * pFontType, const _uint & iWidth, const _uint & iHeight, const _uint & iWeight)
{
	CMyFont*		pMyFont = Find_Font(pFontTag);

	if (nullptr != pMyFont)
		return E_FAIL;

	pMyFont = CMyFont::Create(pGraphicDev, pFontType, iWidth, iHeight, iWeight);
	NULL_CHECK_RETURN(pMyFont, E_FAIL);

	m_mapFont.insert({ pFontTag, pMyFont });

	return S_OK;
}

void CFontMgr::Render_Font(const _tchar * pFontTag, const _tchar * pString, const _vec2 * pPos, D3DXCOLOR Color)
{
	CMyFont*		pMyFont = Find_Font(pFontTag);
	NULL_CHECK(pMyFont);

	pMyFont->Render_Font(pString, pPos, Color);
}

void CFontMgr::Render_Font_Custom(const _tchar* pFontTag, const _tchar* pString, const _vec2* pPos, DWORD format, D3DXCOLOR Color)
{
	CMyFont* pMyFont = Find_Font(pFontTag);
	NULL_CHECK(pMyFont);

	pMyFont->Render_Font_Custom(pString, pPos, format, Color);
}

CMyFont * CFontMgr::Find_Font(const _tchar * pFontTag)
{
	auto	iter = find_if(m_mapFont.begin(), m_mapFont.end(), CTag_Finder(pFontTag));

	if (iter == m_mapFont.end())
		return nullptr;

	return iter->second;
}

void CFontMgr::Free()
{
	for_each(m_mapFont.begin(), m_mapFont.end(), CDeleteMap());
	m_mapFont.clear();
}
