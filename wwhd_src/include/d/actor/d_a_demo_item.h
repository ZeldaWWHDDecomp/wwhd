/** Cutscene item: GameCube actor tail 0x63C moves to WWHD 0x750. */
#pragma once
#include "d/actor/d_a_itembase.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/gc_api.h"
// HD: the emitter adds a translation mode at 0x262 that can invert global Y.
struct DitemEmitterHD {
    u8 _0[0x5C];
    be<s32> mMaxFrame;
    u8 _60[0x1F0 - 0x60];
    Mtx34 mGlobalRotation;
    u8 _220[0x22C - 0x220];
    cXyz mGlobalTranslation;
    u8 _238[0x247 - 0x238];
    be<u8> mGlobalAlpha;
    u8 _248[0x254 - 0x248];
    be<u32> mFlags;
    u8 _258[0x262 - 0x258];
    be<u8> mTranslationMode;
};
WWHD_OFFSET(DitemEmitterHD, mMaxFrame, 0x5C);
WWHD_OFFSET(DitemEmitterHD, mGlobalRotation, 0x1F0);
WWHD_OFFSET(DitemEmitterHD, mGlobalTranslation, 0x22C);
WWHD_OFFSET(DitemEmitterHD, mGlobalAlpha, 0x247);
WWHD_OFFSET(DitemEmitterHD, mFlags, 0x254);
WWHD_OFFSET(DitemEmitterHD, mTranslationMode, 0x262);
WWHD_SIZE(DitemEmitterHD, 0x263);
struct DitemOffsetTableHD {
    cXyz mOffsets[3];
    be<u32> mInitialized;
};
WWHD_SIZE(DitemOffsetTableHD, 0x28);
WWHD_OFFSET(DitemOffsetTableHD, mInitialized, 0x24);
struct DitemResourceHD {
    gptr<const char> mArcname;
    u8 _4[4];
    be<s16> mBmdIdx;
    u8 _A[0x20 - 0xA];
    be<u16> mHeapSize;
    u8 _22[2];
};
WWHD_SIZE(DitemResourceHD, 0x24);
WWHD_OFFSET(DitemResourceHD, mBmdIdx, 8);
WWHD_OFFSET(DitemResourceHD, mHeapSize, 0x20);
struct DitemResourceTableHD {
    DitemResourceHD mItems[256];
};
WWHD_SIZE(DitemResourceTableHD, 0x2400);
// HD: the item draw entry calls the DrawBase virtual slot and preserves its full return register.
struct DitemVtableHD {
    u8 _0[0x14];
    be<u32> mDrawBase;
};
WWHD_SIZE(DitemVtableHD, 0x18);
inline BOOL Ditem_drawBaseVirtual(daItemBase_c *item) {
    return gabi::call_ptr<BOOL>(gabi::at<DitemVtableHD>(item->__vtbl)->mDrawBase, item);
}
s16 Ditem_getPriority(fopAc_ac_c *);
void Ditem_addToDrawQueue(create_tag_class *, s16);
inline void Ditem_onDraw(fopAc_ac_c *item) {
    s16 priority = Ditem_getPriority(item);
    Ditem_addToDrawQueue(&item->draw_tag, priority);
}
struct daDitem_c : daItemBase_c {
    enum ArgFlag { FLAG_UNK01 = 0x01, FLAG_UNK02 = 0x02, FLAG_UNK04 = 0x04, FLAG_UNK08 = 0x08 };

    BOOL chkArgFlag(u8 flag) {
        return mArgFlag & flag;
    }

    BOOL Delete();
    cPhs_State create();
    BOOL execute();
    void setParticle();
    bool CreateInit();
    void set_effect();
    void set_pos();
    void anim_control();
    void set_mtx();
    void settingBeforeDraw();

    /* 0x750 */ gptr<JPABaseEmitter> mpEmitters[4];
    /* 0x760 */ cXyz mOffsetPos;
    /* 0x76C */ be<u8> mFlag;
    /* 0x76D */ be<u8> mArgFlag;
    /* 0x76E */ u8 _76E[2];
};
WWHD_OFFSET(daDitem_c, mpEmitters, 0x750);
WWHD_OFFSET(daDitem_c, mArgFlag, 0x76D);
WWHD_SIZE(daDitem_c, 0x770);

namespace dPa_name {
enum {
    ID_IT_JN_GETITEM_FLASH_L00 = 0x1F7,
    ID_IT_JN_GETITEM_FLASH_S00 = 0x1F8,
    ID_IT_JN_GETITEM_HALO00 = 0x1F9,
    ID_IT_JN_GETITEM_STAR00 = 0x1FA
};
}
enum {
    dItemNo_GREEN_RUPEE_e = 1,
    dItemNo_SMALL_KEY_e = 0x15,
    dItemNo_SKULL_HAMMER_e = 0x33,
    dItemNo_EMPTY_BSHIP_e = 0x4F,
    dItemNo_UNK_BOTTLE_60_e = 0x60
};
