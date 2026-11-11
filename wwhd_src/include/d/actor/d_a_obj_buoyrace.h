/* WWHD race buoy. */
#pragma once
#include "gc_api/gc_api.h"
#include "sead/ElementAt.h"
namespace daObjBuoyrace {
struct Act_c : fopAc_ac_c {
    Act_c();
    enum Prm_e { PRM_ID_W = 8, PRM_ID_S = 0, PRM_LINE_W = 8, PRM_LINE_S = 8 };
    be<f32> mMeanSeaHeight;
    cXyz mWaterNrm;  /* was m294: set_water_pos 0232EF34 stores the normalised cross product of the water surface around the buoy; afl_calc_sway 0232F554 pulls the tilt toward its x / z (* 0.35, at most 0.3) (tww the same) */
    be<s16> mBobPhase;  /* was m2A0: afl_calc 0232F650 adds 1000 .. 2000 at random each frame; the buoy height follows mMeanSeaHeight + 2 * (0.5 * sin(it) - 120) + mBobOffsetY; _create 0232F3D8 uses it for the start height (tww the same) */
    u8 m2A2[10];
    be<f32> mSwayX;  /* was m2AC: afl_calc_sway 0232F554 adds mSwaySpeedX each frame; set_mtx 0232F170 tilts the buoy by the up vector (mSwayX, 1, mSwayZ) (tww the same; as daObjMovebox::Act_c mSwayX) */
    be<f32> mSwayZ;  /* was m2B0: afl_calc_sway 0232F554 adds mSwaySpeedZ each frame; set_mtx 0232F170 uses it as the up vector's z (tww the same) */
    be<f32> mSwaySpeedX;  /* was m2B4: afl_calc_sway 0232F554: spring speed of mSwayX (pull 0.02 toward the water normal's x, damping 0.04) (tww the same) */
    be<f32> mSwaySpeedZ;  /* was m2B8: afl_calc_sway 0232F554: spring speed of mSwayZ (pull 0.02 toward the water normal's z, damping 0.04) (tww the same) */
    be<f32> mBobOffsetY;  /* was m2BC: afl_calc 0232F650 adds mBobSpeedY (at most 30) and adds it to the buoy's target height (tww the same) */
    be<f32> mBobSpeedY;  /* was m2C0: afl_calc 0232F650: spring speed of mBobOffsetY (pull -0.01 * it, * 0.94 damping) (tww the same) */
    request_of_phase_process_class mPhsKiba, mPhsHasi;
    gptr<J3DModel> mpModelKiba, mpModelHasi;
    s32 create_load();
    bool create_heap();
    void set_water_pos();
    void set_rope_pos();
    void set_mtx();
    void init_mtx();
    s32 _create();
    bool _delete();
    void afl_calc_sway();
    void afl_calc();
    bool _execute();
    bool _draw();
};
WWHD_OFFSET(Act_c, mMeanSeaHeight, 0x3AC);
WWHD_OFFSET(Act_c, mSwayX, 0x3C8);
WWHD_OFFSET(Act_c, mPhsKiba, 0x3E0);
WWHD_OFFSET(Act_c, mpModelKiba, 0x3F0);
WWHD_SIZE(Act_c, 0x3F8);

} // namespace daObjBuoyrace

struct BuoyRopeSegment {
    gptr<cXyz> mpPos;
};
struct BuoyRopeLine {
    gptr<BuoyRopeSegment> mpLines;
    u8 padding[0x144];
};
struct BuoyGoalFlag {
    u8 padding[0x2BD0];
    BuoyRopeLine mRopeLines[4];
    cXyz *getRopePos(u32 line, s32 index) {
        // Guest EA arithmetic: mRopeLines + line * sizeof(BuoyRopeLine) wraps at 32 bits as on the console (a generated line
        // reached host addresses far above the guest window).
        BuoyRopeSegment *segment = sead::elementAt<BuoyRopeLine>(gabi::ea(mRopeLines), line).mpLines;
        return gabi::at<cXyz>(gabi::ea(static_cast<cXyz *>(segment->mpPos)) + u32(index) * 0x30);
    }
};
struct BuoyHeaderZeros {
    be<u32> words[4];
};
void buoyOutprod(const cXyz *, cXyz *, const cXyz *);
void buoyNormalize(cXyz *, cXyz *);
void buoyScale(const cXyz *, cXyz *, f32);
void buoyQuatRotBaseY(Quaternion *, const cXyz *);
f32 buoySqrt(f32);
void buoyInitA(void *);
void buoyInitB(void *);
void buoyActorDt(void *, s32);
