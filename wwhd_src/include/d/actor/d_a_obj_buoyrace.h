/* WWHD race buoy. */
#pragma once
#include "gc_api/gc_api.h"
namespace daObjBuoyrace {
struct Act_c : fopAc_ac_c {
    Act_c();
    enum Prm_e { PRM_ID_W = 8, PRM_ID_S = 0, PRM_LINE_W = 8, PRM_LINE_S = 8 };
    be<f32> mMeanSeaHeight;
    cXyz m294;
    be<s16> m2A0;
    u8 m2A2[10];
    be<f32> m2AC, m2B0, m2B4, m2B8, m2BC, m2C0;
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
WWHD_OFFSET(Act_c, m2AC, 0x3C8);
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
        BuoyRopeSegment *segment = mRopeLines[line].mpLines;
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
