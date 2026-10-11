/* Thunder environment process, GameCube names and WWHD layout.
 * HD: environment base grows by four bytes and animation wrappers grow to 0x74/0x78. */
#pragma once
#include "bindings.h"
#include "gc_api/f_op/f_op_kankyo.h"

// HD: keep this binding local; a shared inline changes d_wind_arrow's existing ext binding.
void mDoExt_restoreCurrentHeap();

struct dThunder_modelInfo_c {
    /* 0x000 */ gptr<J3DModel> mpModel;
    /* 0x004 */ u8 mInvisModel[8];
    /* 0x00C */ mDoExt_btkAnm mBtk;
    /* 0x080 */ mDoExt_brkAnm mBrk;
};
WWHD_OFFSET(dThunder_modelInfo_c, mInvisModel, 4);
WWHD_OFFSET(dThunder_modelInfo_c, mBtk, 0xC);
WWHD_OFFSET(dThunder_modelInfo_c, mBrk, 0x80);
WWHD_SIZE(dThunder_modelInfo_c, 0xF8);

class dThunder_c : public kankyo_class {
  public:
    enum : u32 { __vt = 0x1005042C }; // HD: per-unit virtual destructor table.
    /* 0x0FC */ gptr<JKRSolidHeap> solid_heap;
    /* 0x100 */ dThunder_modelInfo_c mModelInfo;
    /* 0x1F8 */ cXyz mScale;
    /* 0x204 */ cXyz mPos;
    /* 0x210 */ cXyz mOffsetPos; // HD: camera-relative displacement retained for draw.
    /* 0x21C */ cXyz mPosNeg;
    /* 0x228 */ be<f32> mRot;
    /* 0x22C */ be<f32> mBtkTime;
    u32 draw();
    u32 execute();
    u32 createHeap();
    void adjustHeap();
};
WWHD_OFFSET(dThunder_c, solid_heap, 0xFC);
WWHD_OFFSET(dThunder_c, mModelInfo, 0x100);
WWHD_OFFSET(dThunder_c, mScale, 0x1F8);
WWHD_OFFSET(dThunder_c, mPos, 0x204);
WWHD_OFFSET(dThunder_c, mOffsetPos, 0x210);
WWHD_OFFSET(dThunder_c, mPosNeg, 0x21C);
WWHD_OFFSET(dThunder_c, mRot, 0x228);
WWHD_OFFSET(dThunder_c, mBtkTime, 0x22C);
WWHD_SIZE(dThunder_c, 0x230);

// HD: private partial views used by this unit; complete camera/light headers are separate.
struct dThunder_camera_c {
    u8 _0[0xDC];
    cXyz mEye;
    cXyz mCenter;
};
WWHD_OFFSET(dThunder_camera_c, mEye, 0xDC);
WWHD_OFFSET(dThunder_camera_c, mCenter, 0xE8);
WWHD_SIZE(dThunder_camera_c, 0xF4);
struct dThunder_env_c {
    u8 _0[0xAB0];
    struct {
        u8 _0;
        be<u8> mState;
    } mThunderEff;
};
WWHD_OFFSET(dThunder_env_c, mThunderEff, 0xAB0);
WWHD_SIZE(dThunder_env_c, 0xAB2);
