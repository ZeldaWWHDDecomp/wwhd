/** Eight model/collision test objects; GameCube actor tail 0x2C8 moves to HD 0x3E0. */
#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/gc_api.h"

namespace daObjMtest {
struct Act_c : dBgS_MoveBgActor {
    enum Type {
        Type_Mcube,
        Type_Mcyln,
        Type_Mcube10,
        Type_Mcyln10,
        Type_MwtrSB,
        Type_MygnSB,
        Type_Owater,
        Type_Astop,
        Type_Max
    };
    BOOL chk_appear();
    s32 Mthd_Create();
    s32 Mthd_Delete();
    BOOL CreateHeap();
    void set_mtx();
    void init_mtx();
    BOOL Create();
    BOOL Execute(be<u32> *pMtx);
    BOOL Draw();
    /* 0x3E0 */ Mtx34 mMtx;
    /* 0x410 */ request_of_phase_process_class mPhase;
    /* 0x418 */ gptr<J3DModel> mpModel;
    /* 0x41C */ dCcD_Stts mStts;
    /* 0x458 */ dCcD_Cyl mCyl;
    /* 0x588 */ be<s32> M_type;
    /* 0x58C */ be<u8> mbAppear;
    u8 _58D[3];
};
} // namespace daObjMtest
WWHD_SIZE(daObjMtest::Act_c, 0x590);
WWHD_OFFSET(daObjMtest::Act_c, mMtx, 0x3E0);
WWHD_OFFSET(daObjMtest::Act_c, mPhase, 0x410);
WWHD_OFFSET(daObjMtest::Act_c, mpModel, 0x418);
WWHD_OFFSET(daObjMtest::Act_c, mStts, 0x41C);
WWHD_OFFSET(daObjMtest::Act_c, mCyl, 0x458);
WWHD_OFFSET(daObjMtest::Act_c, M_type, 0x588);
WWHD_OFFSET(daObjMtest::Act_c, mbAppear, 0x58C);
struct MtestBackgroundHD {
    u8 _0[0x6C];
    be<u8> mFlags;
};
WWHD_OFFSET(MtestBackgroundHD, mFlags, 0x6C);
// HD: mapped header objects are kept separate to retain the original initialization order.
struct MtestHeaderZeroHD {
    be<u32> mWords[4];
};
struct MtestPiPairHD {
    be<f32> mMin, mMax;
};
struct MtestRegistrationHD {
    u8 mBytes[12];
};
struct MtestScaleTableHD {
    cXyz mScales[8];
};
using MtestHeaderByteHD = be<u8>;
using MtestCylinderSourceHD = dCcD_SrcCyl;

inline void Mtest_setScaleComponent(cXyz &scale, u32 component, f32 value) {
    if (component == 0)
        scale.x = value;
    else if (component == 1)
        scale.y = value;
    else
        scale.z = value;
}

WWHD_SIZE(MtestHeaderZeroHD, 0x10);
WWHD_SIZE(MtestPiPairHD, 8);
WWHD_SIZE(MtestRegistrationHD, 12);
WWHD_SIZE(MtestScaleTableHD, 0x60);
