#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum WINMODE { MODE_FULL, MODE_WIN };

	// Dynamic 컴포넌트 경우 매 프레임마다 갱신해야하는 컴포넌트 집단
	enum COMPONENTID { ID_DYNAMIC, ID_STATIC, ID_END };

	enum INFO { INFO_RIGHT, INFO_UP, INFO_LOOK, INFO_POS, INFO_END };
	enum ROTATION { ROT_X, ROT_Y, ROT_Z, ROT_END };

	enum TEXTUREID { TEX_NORMAL, TEX_CUBE, TEX_END };

	enum RENDERID { RENDER_PRIORITY, RENDER_NONALPHA, RENDER_ALPHA, RENDER_UI, RENDER_END };

	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

	enum STATE { IDLE, WALK, SWING, DEAD, STATE_END };

	enum DIRECTION { FRONT, RIGHT, BACK, LEFT, DIRECTION_END };

	enum RENDERTYPE { TYPE_PERSPECTIVE, TYPE_ORTHOGRAPHIC, TYPE_END };

	// 아이템 고유값, 서로 다른 아이템이면 겹칠 수 없음
	// 장비는 0~100의 값을 갖도록 설정하여 인벤토리에서 같은 아이템이어도 겹칠 수 없게 할 것
	// 헬멧은 0~10, 상의는 11~20, 하의는 21~30 이런식으로 설정해서 아무칸에나 장비할 수 없도록 설정
	enum ITEMNUM { ITEM_SWORD, ITEM_BOW, ITEM_HELM, ITEM_CHEST, ITEM_SEED = 40, ITEM_END };

	// 사운드 채널
	enum CHANNELID { SOUND_EFFECT, SOUND_IMPORTANT, SOUND_BGM, MAXCHANNEL };
}
#endif // Engine_Enum_h__	
