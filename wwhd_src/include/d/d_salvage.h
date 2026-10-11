/** Salvage records, WWHD layout.
 * HD retains tww's record and counter layout. Guest result adapters preserve GHS return storage. */
#pragma once
#include "gc_api/gc_api.h"
struct dSalvage_info_c {
    /* 0x00 */ cXyz mPos;
    /* 0x0C */ cXyz mScale;
    /* 0x18 */ be<f32> mR;
    /* 0x1C */ be<f32> mH;
    /* 0x20 */ be<s32> mSwitchNo;
    /* 0x24 */ gptr<JPABaseEmitter> mEmtr;
    /* 0x28 */ be<f32> mDistance;
    /* 0x2C */ be<s8> mRoomNo;
    /* 0x2D */ be<u8> mSaveNo, mType, mKind, mItemNo, mFlag;
    /* 0x32 */ u8 field_0x32;
    /* 0x33 */ be<u8> mNowAlpha, mDrawMode;
    /* 0x35 */ u8 pad[3];
};
WWHD_OFFSET(dSalvage_info_c, mRoomNo, 0x2C);
WWHD_OFFSET(dSalvage_info_c, mFlag, 0x31);
WWHD_OFFSET(dSalvage_info_c, mNowAlpha, 0x33);
WWHD_SIZE(dSalvage_info_c, 0x38);
class dSalvage_control_c {
public:
    enum { MAX_SEA=128, MAX_ROOM=32, MAX_TOTAL=160 };
    void entry(fopAc_ac_c*, JPABaseEmitter*);
    s32 checkRegist(s32); void calcDistanceXZ();
    void setPos(s32, cXyz*); void setNowAlpha(s32,u32); void setDrawMode(s32,s32);
    void init(); void init_one_sub(s32); void init_end(); void init_room(s32); void init_one(s32);
    void setFlag(s32,u32); void clrFlag(s32,u32);
    cXyz* getPosP(s32);
    f32 getR(s32); f32 getH(s32); s8 getRoomNo(s32); u8 getItemNo(s32); s32 getSwitchNo(s32);
    u8 getSaveNo(s32); u8 getType(s32); u8 getKind(s32); s32 getDrawMode(s32); f32 getDistance(s32);
    void* getAlphaPtr(s32); s32 checkUsed(s32);
    /* 0x00 */ be<u8> mSeaNum, mRoomNum;
    /* 0x02 */ u8 pad[2];
    /* 0x04 */ dSalvage_info_c mInfo[MAX_TOTAL];
};
WWHD_OFFSET(dSalvage_control_c, mInfo, 4);
WWHD_SIZE(dSalvage_control_c, 0x2304);
namespace salvage_layout {
struct SaveLabels { be<u16> values[256]; };
WWHD_SIZE(SaveLabels, 0x200);
struct Emitter {
    u8 pad[0x5C]; be<s32> mMaxFrame;
    u8 padFlags[0x254-0x60]; be<u32> mStatus;
};
WWHD_OFFSET(Emitter, mStatus, 0x254);
WWHD_SIZE(Emitter, 0x258);
}
