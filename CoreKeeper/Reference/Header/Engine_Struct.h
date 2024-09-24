#ifndef Engine_Struct_h__
#define Engine_Struct_h__

#include "Engine_Typedef.h"

namespace Engine
{
	typedef struct tagVertexColor
	{
		_vec3		vPosition;			
		_ulong		dwColor;
	
	}VTXCOL;

	const _ulong	FVF_COL = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX0;

	typedef struct tagVertexTexture
	{
		_vec3		vPosition;
		_vec3		vNormal;
		_vec2		vTexUV;

	}VTXTEX;

	const _ulong	FVF_TEX = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

	typedef struct tagMapToolVertexTexure
	{
		_vec3		vPosition;
		_vec2		vTextUV;
	}VTXTOOLTEX;

	const _ulong FVF_TOOLTEX = D3DFVF_XYZ | D3DFVF_TEX1;

	typedef struct tagVertexCubeTexture
	{
		_vec3		vPosition;
		_vec3		vTexUV;

	}VTXCUBE;

	const _ulong	FVF_CUBE = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0); // 텍스처의 UV 좌표 값을 FLOAT형 3개로 표현하겠다는 매크로(괄호안의 숫자 0의 의미는 본래 버텍스에 텍스쳐 UV값이 여러개가 올 수 있는데 그중 0번째 값을 지정하겠다는 의미)

	typedef struct tagIndex16
	{
		_ushort  _0;
		_ushort  _1;
		_ushort  _2;

	}INDEX16;

	typedef struct tagIndex32
	{
		_ulong	_0;
		_ulong	_1;
		_ulong	_2;

	}INDEX32;	

	typedef struct tagAnimator
	{
		STATE   eCurState;
		STATE   ePreState;

		int		iStart;
		int		iEnd;
		int		iMotion;
		int		iCount;
		int		iCurCount;

	}ANIM;

	typedef struct tagStat
	{
		int iHp;
		int iMaxHp;
		int iMp;
		int iMaxMp;
		int iAttack;
		int iDefense;

		tagStat()
		{
			ZeroMemory(this, sizeof(tagStat));
		}

		tagStat(int _iMaxHP, int _iMaxMP, int _iAttack, int _iDefense)
		{
			iHp = _iMaxHP;
			iMaxHp = _iMaxHP;
			iMp = _iMaxMP;
			iMaxMp = _iMaxMP;
			iAttack = _iAttack;
			iDefense = _iDefense;
		}

	}STAT;
}


#endif // Engine_Struct_h__
