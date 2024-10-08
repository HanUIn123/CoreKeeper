HRESULT	Ready_GraphicDev(HWND hWnd, WINMODE eMode, const _uint& iSizeX, const _uint& iSizeY, CGraphicDev** ppGraphicDev)
{
	return CGraphicDev::GetInstance()->Ready_GraphicDev(hWnd, eMode, iSizeX, iSizeY, ppGraphicDev);
}
void		Render_Begin(D3DXCOLOR Color)
{
	CGraphicDev::GetInstance()->Render_Begin(Color);
}
void		Render_End()
{
	CGraphicDev::GetInstance()->Render_End();
}

_float			Get_TimeDelta(const _tchar* pTimerTag)
{
	return CTimerMgr::GetInstance()->Get_TimeDelta(pTimerTag);
}
void				Set_TimeDelta(const _tchar* pTimerTag)
{
	CTimerMgr::GetInstance()->Set_TimeDelta(pTimerTag);
}
HRESULT			Ready_Timer(const _tchar* pTimerTag)
{
	return CTimerMgr::GetInstance()->Ready_Timer(pTimerTag);
}

HRESULT			Ready_Frame(const _tchar* pFrameTag, const _float& fCallLimit)
{
	return CFrameMgr::GetInstance()->Ready_Frame(pFrameTag, fCallLimit);
}
_bool			IsPermit_Call(const _tchar* pFrameTag, const _float& fTimeDelta)
{
	return CFrameMgr::GetInstance()->IsPermit_Call(pFrameTag, fTimeDelta);
}

inline HRESULT				Ready_Font(LPDIRECT3DDEVICE9 pGraphicDev,
	const _tchar* pFontTag,
	const _tchar* pFontType,
	const _uint& iWidth,
	const _uint& iHeight,
	const _uint& iWeight)
{
	return CFontMgr::GetInstance()->Ready_Font(pGraphicDev, pFontTag, pFontType, iWidth, iHeight, iWeight);
}

inline void				Render_Font(const _tchar* pFontTag,
	const _tchar* pString,
	const _vec2* pPos,
	D3DXCOLOR Color)
{
	CFontMgr::GetInstance()->Render_Font(pFontTag, pString, pPos, Color);
}

inline void				Render_Font_Custom(const _tchar* pFontTag,
	const _tchar* pString,
	const _vec2* pPos,
	DWORD format,
	D3DXCOLOR Color)
{
	CFontMgr::GetInstance()->Render_Font_Custom(pFontTag, pString, pPos, format, Color);
}

_byte	Get_DIKeyState(_ubyte byKeyID)
{
	return CInputDev::GetInstance()->Get_DIKeyState(byKeyID);
}
_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
	return CInputDev::GetInstance()->Get_DIMouseState(eMouse);
}
_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	return CInputDev::GetInstance()->Get_DIMouseMove(eMouseState);
}
_bool   Key_Down(_ubyte byKeyID)
{
	return CInputDev::GetInstance()->Key_Down(byKeyID);
}
_bool    Button_Down(MOUSEKEYSTATE eMouse)
{
	return CInputDev::GetInstance()->Button_Down(eMouse);
}
_bool    Button_Up(MOUSEKEYSTATE eMouseState)
{
	return CInputDev::GetInstance()->Button_Up(eMouseState);
}
HRESULT Ready_InputDev(HINSTANCE hInst, HWND hWnd)
{
	return CInputDev::GetInstance()->Ready_InputDev(hInst, hWnd);
}
void	Update_InputDev(void)
{
	CInputDev::GetInstance()->Update_InputDev();
}

HRESULT Ready_Sound()
{
	return CSoundMgr::GetInstance()->Ready_Sound();
}

void Play(const TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
	CSoundMgr::GetInstance()->Play(pSoundKey, eID, fVolume);
}

void PlayBGM(const TCHAR* pSoundKey, float fVolume)
{
	CSoundMgr::GetInstance()->PlayBGM(pSoundKey, fVolume);
}

void PlayBGMOnce(const TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
	CSoundMgr::GetInstance()->PlayBGMOnce(pSoundKey, eID, fVolume);
}

void StopSound(CHANNELID eID)
{
	CSoundMgr::GetInstance()->StopSound(eID);
}
void StopAll()
{
	CSoundMgr::GetInstance()->StopAll();
}

void SetChannelVolume(CHANNELID eID, float fVolume)
{
	CSoundMgr::GetInstance()->SetChannelVolume(eID, fVolume);
}

void		Release_System()
{
	CInputDev::GetInstance()->DestroyInstance();
	CSoundMgr::GetInstance()->DestroyInstance();
	CFontMgr::GetInstance()->DestroyInstance();
	CFrameMgr::GetInstance()->DestroyInstance();
	CTimerMgr::GetInstance()->DestroyInstance();
	CGraphicDev::GetInstance()->DestroyInstance();
}
