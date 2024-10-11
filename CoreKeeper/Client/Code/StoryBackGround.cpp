#include "pch.h"
#include "..\Header\StoryBackGround.h"
#include "Export_Utility.h"
#include "Export_System.h"

CStoryBackGround::CStoryBackGround(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_iTextureNum(0), m_fElapsedTime(0.f), m_iCurrentIndex(0)
{
	m_strMent[0] = L"머나먼 외딴 곳.      ";
	m_strMent[1] = L"당신은 다른 탐험가들과 함께 숲을 지나고 있습니다.      ";
	m_strMent[2] = L"이상한 존재를 느낀 당신은, \n마루 사이에 무언가가 있다는 것을 알아챕니다.      ";
	m_strMent[3] = L"당신은 그것에 이끌려, \n이상한 유물에 다가가 손을 뻗습니다.      ";
	m_strMent[4] = L"유물의 표면에 손이 닿자 눈부신 빛이 당신을 에워쌉니다.      ";
}

CStoryBackGround::~CStoryBackGround()
{
}

HRESULT CStoryBackGround::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	return S_OK;
}

_int CStoryBackGround::Update_GameObject(const _float& fTimeDelta)
{
	_int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_fElapsedTime += fTimeDelta;

	Engine::Add_RenderGroup(RENDER_PRIORITY, this);

	return iExit;
}

void CStoryBackGround::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CStoryBackGround::Render_GameObject()
{
	m_pTextureCom->Set_Texture(m_iTextureNum);
	m_pBufferCom->Render_Buffer();

	float typingSpeed = 0.1f;

	if (m_fElapsedTime >= typingSpeed)
	{
		m_fElapsedTime = 0.0f;
		if (m_iCurrentIndex < m_strMent[m_iTextureNum].length())
		{
			m_iCurrentIndex++;
			
			if (m_strMent[m_iTextureNum][m_iCurrentIndex-1] != L' ' && m_strMent[m_iTextureNum][m_iCurrentIndex - 1] != L',' && m_strMent[m_iTextureNum][m_iCurrentIndex - 1] != L'.')
			{
				Engine::Play(L"FIXME_menu_select.wav", SOUND_EFFECT, 0.1f);
			}
		}
		else
		{
			if (m_iTextureNum < 4)
			{
				m_iTextureNum++;
				m_iCurrentIndex = 0;
			}
		}

		//if (m_iCurrentIndex < m_strMent[m_iTextureNum].length() - 6)
		//{
		//	Engine::Play(L"FIXME_menu_select.wav", SOUND_EFFECT, 0.1f);
		//}
	}

	wstring currentText = m_strMent[m_iTextureNum].substr(0, m_iCurrentIndex);

	_vec2 LTpos(0, 550);
	_vec2 RBpos(WINCX, 550);

	Engine::Render_Font_Custom(L"Font_Default", currentText.c_str(), &LTpos, &RBpos, DT_CENTER, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
}

HRESULT CStoryBackGround::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_StoryTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_StoryTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

CStoryBackGround* CStoryBackGround::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStoryBackGround* pStoryBackGround = new CStoryBackGround(pGraphicDev);

	if (FAILED(pStoryBackGround->Ready_GameObject()))
	{
		Safe_Release(pStoryBackGround);
		MSG_BOX("pStoryBackGround Create Failed");
		return nullptr;
	}

	return pStoryBackGround;
}

void CStoryBackGround::Free()
{
	Engine::CGameObject::Free();
}
