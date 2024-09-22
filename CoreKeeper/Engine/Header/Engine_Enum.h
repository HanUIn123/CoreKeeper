#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
    enum WINMODE { MODE_FULL, MODE_WIN };

    // Dynamic 컴포넌트 경우 매 프레임마다 갱신해야하는 컴포넌트 집단
    enum COMPONENTID { ID_DYNAMIC, ID_STATIC, ID_END };

    enum INFO { INFO_RIGHT, INFO_UP, INFO_LOOK, INFO_POS, INFO_END };
    enum ROTATION { ROT_X, ROT_Y, ROT_Z, ROT_END };

    enum TEXTUREID { TEX_NORMAL, TEX_CUBE, TEX_OBJECT, TEX_END };

    enum RENDERID { RENDER_PRIORITY, RENDER_NONALPHA, RENDER_ALPHA, RENDER_UI, RENDER_UIALPHA, RENDER_END };

    enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
    enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

    enum STATE { IDLE, WALK, SWING, DEAD, STATE_END };

    enum DIRECTION { FRONT, RIGHT, BACK, LEFT, DIRECTION_END };

    enum RENDERTYPE { TYPE_PERSPECTIVE, TYPE_ORTHOGRAPHIC, TYPE_END };

    // 사운드 채널
    enum CHANNELID { SOUND_EFFECT, SOUND_IMPORTANT, SOUND_BGM, MAXCHANNEL };

    // 아이템 고유값, 서로 다른 아이템이면 겹칠 수 없음
    // 장비는 50 이하의 값을 갖도록 설정하여 인벤토리에서 같은 아이템이어도 겹칠 수 없게 할 것
    enum ITEMNUM
    {
        // 도구
        ITEM_PICKAXE, ITEM_HOE, ITEM_SHOVEL,
        // 무기
        ITEM_SWORD = 5, ITEM_BOW, ITEM_STAFF,
        // 장비
        ITEM_HELMET = 10, ITEM_CHEST, ITEM_LEG,
        // 악세서리
        ITEM_NECKLACE = 15, ITEM_RING, ITEM_BAG, ITEM_LANTERN,
        // 보조장비
        ITEM_ASSISTANCE = 20,
        ITEM_ETC = 50,
        ITEM_SEED,
        ITEM_WOOD,
        ITEM_TORCH,
        ITEM_MUCUS,
        ITEM_END
    };
    enum MONSTERTYPE { MON_SLIME, MON_END };

}
#endif // Engine_Enum_h__   
