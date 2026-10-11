// Makar's cello music actor. Private WWHD layout.
#pragma once
#include "gc_api/gc_api.h"

struct MmusicRotation { be<f32> m[3][3]; };
void JPASetRMtxTVecfromMtx(Mtx34*, MmusicRotation*, cXyz*);
// HD: emitter view includes the members used by inline particle controls.
struct MmusicEmitter {
    u8 mPadding0[0x5C];
    be<s32> mMaxFrame;
    u8 mPadding60[0x1F0 - 0x60];
    MmusicRotation mGlobalRot;
    u8 mPadding214[0x22C - 0x214];
    cXyz mGlobalTrs;
    u8 mPadding238[0x254 - 0x238];
    be<u32> mFlags;
    void stopCreateParticle() { mFlags = (u32)mFlags | 1; }
    void playCreateParticle() { mFlags = (u32)mFlags & ~1u; }
    void becomeInvalidEmitter() {
        u32 flags = mFlags;
        mMaxFrame = -1;
        mFlags = flags | 1;
    }
    void setGlobalRTMatrix(Mtx34* matrix) {
        JPASetRMtxTVecfromMtx(matrix, &mGlobalRot, &mGlobalTrs);
    }
};
WWHD_OFFSET(MmusicEmitter, mFlags, 0x254);
WWHD_OFFSET(MmusicEmitter, mGlobalRot, 0x1F0);
WWHD_OFFSET(MmusicEmitter, mGlobalTrs, 0x22C);
WWHD_SIZE(MmusicEmitter, 0x258);

namespace daMmusic {
class Act_c : public fopAc_ac_c {
public:
    Act_c();
    bool create_heap() { return true; }
    BOOL Macore_is_playing();
    void set_mtx();
    cPhs_State _create();
    bool _delete();
    void init_se();
    void manage_se(int cull);
    void delete_se();
    bool _execute();
    bool _draw() { return true; }
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ be<s32> field_0x298;
    /* 0x3B8 */ be<s32> field_0x29c;
    /* 0x3BC */ gptr<MmusicEmitter> mpEmitter;
    /* 0x3C0 */ Mtx34 mMtx;
    /* 0x3F0 */ be<s16> mTimer;
    u8 mPadding3F2[2];
};
WWHD_OFFSET(Act_c, mpEmitter, 0x3BC);
WWHD_OFFSET(Act_c, mTimer, 0x3F0);
WWHD_SIZE(Act_c, 0x3F4);
}

BOOL mDoAud_checkCbPracticePlay();
void mDoAud_cbPracticePlay(cXyz* pos);
void mDoAud_cbPracticeStop();
void fopAcM_setCullSizeSphere(fopAc_ac_c*, f32, f32, f32, f32);
s32 fopAcM_cullingCheck(fopAc_ac_c*);
