/* Ice-arrow effect: GameCube field names, WWHD base +0x11C and bck size0x8C.
 * Private partial HD views expose only this unit's verified fields. */
#pragma once
#include "gc_api/gc_api.h"
enum {
    dRes_INDEX_LINK_BCK_GICER01_e = 0x0D,
    dRes_INDEX_LINK_BDL_GICER00_e = 0x40,
    dRes_INDEX_LINK_BDL_GICER01_e = 0x41,
};

struct ArrowIceBtkHD {
    u8 mPad0[4];
    be<f32> mFrame;
    u8 mPad8[2];
    be<s16> mFrameMax;
};
WWHD_OFFSET(ArrowIceBtkHD, mFrame, 4);
WWHD_OFFSET(ArrowIceBtkHD, mFrameMax, 0xA);
WWHD_SIZE(ArrowIceBtkHD, 0xC);
struct daArrow_Iceeff_c : fopAc_ac_c {
    BOOL CreateHeap();
    void CreateInit();
    void set_mtx();
    cPhs_State _create();
    bool _delete();
    bool _draw();
    bool _execute();

    /* 0x3AC */ be<u32> field_0x290;
    /* 0x3B0 */ be<u32> field_0x294;
    /* 0x3B4 */ gptr<J3DModel> mpIceModel[30];  /* was field_0x298: CreateHeap 02055B84 creates the 30 ice chunk models (only for the arrow version, mWaterType 0); set_mtx 02055D38 places them, _draw lights and draws the first min(mFrameCount, 30) (tww the same) */
    /* 0x42C */ gptr<J3DModel> mpModel;
    /* 0x430 */ Mtx34 mIceMtx[30];  /* was field_0x314: CreateInit 02055F04 builds each ice chunk's random rotation and offset matrix (in the arrow's frame); set_mtx 02055D38 concatenates it to place chunk i (tww the same) */
    /* 0x9D0 */ cXyz mIceScale[30];  /* was field_0x8B4: CreateInit 02055F04 sets chunk i's random scale (6.3 .. 10.8); set_mtx 02055D38 gives it to the chunk model (tww the same) */
    /* 0xB38 */ gptr<ArrowIceBtkHD> mpIceBtk;  /* was field_0xA1C: CreateInit 02055F04 points it at Link's ice particle btk (mWaterType 0) or ice-water particle btk (1); _draw sets its frame to mBtkFrame, advance_frame stops at its last frame (tww the same); J3DAnmTextureSRTKey* */
    /* 0xB3C */ mDoExt_bckAnm mBck;              /* HD 0x8C */
    /* 0xBC8 */ be<s32> mFrameCount;  /* was field_0xA30: _execute 020564D0 counts it up: the arrow version draws min(it, 30) chunks, advances the btk from 30 and breaks at 60; the water version starts the hit effect at 0, the ripples at 0x23 / 0x28 and ends at 300; CreateInit 02055F04 clears it (tww the same) */
    /* 0xBCC */ be<f32> mBtkFrame;  /* was field_0xA34: advance_frame (in _execute 020564D0) adds 1 up to the btk's last frame; _draw sets mpIceBtk's frame to it; CreateInit 02055F04 clears it (tww the same) */
    /* 0xBD0 */ be<s32> mWaterType;  /* was field_0xA38: _create sets 1 when the arrow's parameter is 4 (the ice arrow hit the water surface), else 0; CreateHeap / CreateInit / set_mtx / _draw / _execute 020564D0 switch between the 30 ice chunks (0) and the single water model (1) (tww the same) */
    /* 0xBD4 */ be<u8> mVisible;  /* was field_0xA3C: CreateInit 02055F04 sets 1; _draw draws the models only while it is set; _execute 020564D0 clears it at frame 0x28 of the water version (the break) (tww the same) */
    /* 0xBD5 */ u8 _BD5[3];
};
WWHD_OFFSET(daArrow_Iceeff_c, mIceMtx, 0x430);
WWHD_OFFSET(daArrow_Iceeff_c, mIceScale, 0x9D0);
WWHD_OFFSET(daArrow_Iceeff_c, mBck, 0xB3C);
WWHD_OFFSET(daArrow_Iceeff_c, mVisible, 0xBD4);
WWHD_SIZE(daArrow_Iceeff_c, 0xBD8);

struct ArrowIceParentHD : fopAc_ac_c {
    u8 mPad3AC[0x7C0 - 0x3AC];
    cXyz mImpactPos;  /* was field_0x6a8: view of daArrow_c::mImpactPos (d_a_arrow.h) */
};
WWHD_OFFSET(ArrowIceParentHD, mImpactPos, 0x7C0);
WWHD_SIZE(ArrowIceParentHD, 0x7CC);
struct ArrowIceLinkHD {
    u8 mPad0[0x4638];
    ArrowIceBtkHD mIceParticleBtk;
    u8 mPad4644[0x4794 - 0x4644];
    ArrowIceBtkHD mIceWaterParticleBtk;
};
WWHD_OFFSET(ArrowIceLinkHD, mIceParticleBtk, 0x4638);
WWHD_OFFSET(ArrowIceLinkHD, mIceWaterParticleBtk, 0x4794);
WWHD_SIZE(ArrowIceLinkHD, 0x47A0);
struct ArrowIceModelHD {
    u8 mPad0[0x78];
    be<u8> mIceEffectHD;
};
WWHD_SIZE(ArrowIceModelHD, 0x79);
struct ArrowIceJointTreeHD {
    u8 mPad0[0x14];
    be<u32> mpMtxCalc;
};
WWHD_SIZE(ArrowIceJointTreeHD, 0x18);
struct ArrowIceEmitterHD {
    u8 mPad0[8];
    cXyz mEmitterScale;
    u8 mPad14[0x34 - 0x14];
    be<f32> mRate;
    u8 mPad38[0x68 - 0x38];
    be<f32> mInitialVelOmni, mInitialVelAxis, mInitialVelDir;
    u8 mPad74[0x220 - 0x74];
    cXyz mGlobalDynamicsScale, mGlobalTranslation, mGlobalParticleScale;
    be<u8> mGlobalPrmColor[4];
    u8 mPad248[0x258 - 0x248];
};
WWHD_OFFSET(ArrowIceEmitterHD, mRate, 0x34);
WWHD_OFFSET(ArrowIceEmitterHD, mGlobalDynamicsScale, 0x220);
WWHD_OFFSET(ArrowIceEmitterHD, mGlobalParticleScale, 0x238);
WWHD_OFFSET(ArrowIceEmitterHD, mGlobalPrmColor, 0x244);
WWHD_SIZE(ArrowIceEmitterHD, 0x258);
struct ArrowIceBckHD {
    u8 mFrameCtrl[0x10];
    be<u32> mFrameCtrlVtable;
    u8 mCalculator[0x34];
    be<u32> mCalculatorVtable;
    u8 mPad4C[0x58 - 0x4C];
    be<u32> mAnmTransform;
    u8 mPad5C[0x7C - 0x5C];
    be<u32> mMorf, mAnm, mOldFrame, mOldFrameMorf;
};
WWHD_OFFSET(ArrowIceBckHD, mCalculator, 0x14);
WWHD_OFFSET(ArrowIceBckHD, mCalculatorVtable, 0x48);
WWHD_OFFSET(ArrowIceBckHD, mAnmTransform, 0x58);
WWHD_OFFSET(ArrowIceBckHD, mOldFrame, 0x84);
WWHD_SIZE(ArrowIceBckHD, 0x8C);
struct ArrowIceRippleHD {
    be<u32> mGuard;
    cXyz mScale;
};
WWHD_SIZE(ArrowIceRippleHD, 0x10);
void mDoMtx_ZrotS(Mtx34*, s16);
void PSMTXConcat(const Mtx34*, const Mtx34*, Mtx34*);
BOOL fopAcM_getWaterY(const cXyz*, be<f32>*);
void ArrowIce_bckBase_ct(mDoExt_bckAnm*, s32);
void ArrowIce_mtxCalc_ct(void*);
void ArrowIce_frameCtrl_dt(void*, s32);
void ArrowIce_actor_dt(fopAc_ac_c*, s32);
