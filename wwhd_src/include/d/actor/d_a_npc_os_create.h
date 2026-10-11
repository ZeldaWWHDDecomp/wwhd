// Os creation/animation helpers. Private supplement; shared actor declarations stay unchanged.
// HD: Reuse measured HIO and lighting views from the other merged parts of this GameCube TU.
#pragma once
#include "d/actor/d_a_npc_os_a.h"
#include "d/actor/d_a_npc_os_b.h"

// HD: unique bindings retain the original call and return widths.
BOOL OsMain_draw(daNpc_Os_c *actor);
BOOL OsMain_initBrk(mDoExt_brkAnm_os *animation, J3DModelData *data, void *key, bool play, s32 mode,
                    f32 rate, s16 start, s16 end, u32 modify, s32 entry);
void OsMain_registerActor(daNpc_Os_c *actor, s32 argument);
s16 OsMain_eventIndex(u32 manager, u32 name, s32 mapTool);
void OsMain_constructNpc(daNpc_Os_c *actor);
void OsMain_constructBrk(mDoExt_brkAnm_os *animation);
void OsMain_constructArray(void *array, s32 count, s32 size, u32 constructor);
void OsMain_constructJointCtrl(dNpc_JntCtrl_c *control);
void OsMain_checkRestart(daNpc_Os_c *actor, s32 restart);
u32 OsMain_playBrk(mDoExt_brkAnm_os *animation);
BOOL OsMain_polygonSafe(dBgS *background, cBgS_PolyInfo_l *polygon);
s32 OsMain_staffId(u32 manager, u32 name, s32 actor, s32 flag);
void OsMain_changePlayer(daNpc_Os_c *actor, fopAc_ac_c *player);
void OsMain_constructNpcHio(u32 hio);
void OsMain_constructHeaderA(u32 object);
void OsMain_constructHeaderB(u32 object);
void OsMain_destroyCircle(u32 circle, s32 flags);

struct OsMainBrkEntry {
    be<u8> brkFileIndex;
    u8 mPadding[3];
    be<s32> loopMode;
    be<f32> speed;
    be<s32> unk_4;
};
WWHD_SIZE(OsMainBrkEntry, 0x10);
struct OsMainAnmPrm {
    be<s8> mAnmTblIdx;
    u8 mPadding[3];
    be<s32> mLoopMode;
    be<f32> mMorf, mPlaySpeed;
    be<s32> m10;
};
WWHD_SIZE(OsMainAnmPrm, 0x14);
struct OsMainHeaderStatics {
    be<f32> mMinAngle, mMaxAngle;
    be<f32> mFarDistances[4];
    u8 mHeaderA, mHeaderB;
    u8 mPadding[2];
    be<u32> mHeaderWords[4];
};
WWHD_OFFSET(OsMainHeaderStatics, mHeaderWords, 0x1C);
WWHD_SIZE(OsMainHeaderStatics, 0x2C);
// HD: only the emitter members used by the infinite callback are exposed.
struct OsMainEmitter {
    u8 mPrefix[0x5C];
    be<s32> mMaxFrame;
    u8 mMiddle[0x1E4 - 0x60];
    be<u32> mEmitterCallBack;
    u8 mTail[0x254 - 0x1E8];
    be<u32> mFlags;
};
WWHD_OFFSET(OsMainEmitter, mEmitterCallBack, 0x1E4);
WWHD_OFFSET(OsMainEmitter, mFlags, 0x254);
WWHD_SIZE(OsMainEmitter, 0x258);

struct OsMainTables {
    struct Brk {
        OsMainBrkEntry entries[9];
    };
    struct Anm {
        OsMainAnmPrm entries[6];
    };
    struct Indices {
        be<s8> entries[3];
    };
    struct Names {
        be<u32> entries[16];
    };
    struct Staff {
        be<u32> entries[3];
    };
    struct Positions {
        cXyz entries[3];
    };
};
WWHD_SIZE(OsMainTables::Brk, 0x90);
WWHD_SIZE(OsMainTables::Anm, 0x78);
WWHD_SIZE(OsMainTables::Positions, 0x24);
enum OsMainResource { BDL_OS = 9, BCK_OS_MOVE01 = 6 };
enum OsMainTargets : u32 {
    ACCH_CIRCLE_CTOR_HD = 0x024EFE94,
    SAFE_STRING_VTABLE_HD = 0x1001F34C,
    POLY_INFO_VTABLE_HD = 0x1001F374,
    AAB_VTABLE_HD = 0x1001F364,
    INFINITE_CALLBACK_VTABLE_HD = 0x1001F424,
    HIO_CHILD_VTABLE_HD = 0x1001F464,
    HIO_VTABLE_HD = 0x1001F474,
};

struct OsMainGlobalRecord {
    be<u32> words[3];
};
WWHD_SIZE(OsMainGlobalRecord, 0xC);

WWHD_OFFSET(OsMainBrkEntry, loopMode, 0x4);
WWHD_OFFSET(OsMainBrkEntry, speed, 0x8);
WWHD_OFFSET(OsMainBrkEntry, unk_4, 0xC);
WWHD_OFFSET(OsMainAnmPrm, mLoopMode, 0x4);
WWHD_OFFSET(OsMainAnmPrm, mPlaySpeed, 0xC);
WWHD_OFFSET(OsMainAnmPrm, m10, 0x10);
WWHD_SIZE(OsMainTables::Indices, 0x3);
WWHD_SIZE(OsMainTables::Names, 0x40);
WWHD_SIZE(OsMainTables::Staff, 0xC);
