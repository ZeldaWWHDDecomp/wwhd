// Waterfall basin particles: WWHD actor and unit-local JPA/JSU views.
#pragma once
#include "gc_api/gc_api.h"
// HD: these views preserve library member names at the offsets used by this unit.
struct TpotaParticleView {
    u8 _00[0x28];
    cXyz mGlobalPosition;
};
struct TpotaParticleLink {
    gptr<TpotaParticleView> mObject;
    be<u32> mList;
    be<u32> mPrev;
    gptr<TpotaParticleLink> mNext;
    TpotaParticleView* getObject() { return mObject; }
    TpotaParticleLink* getNext() { return mNext; }
};
struct TpotaParticleList {
    gptr<TpotaParticleLink> mHead;
    be<u32> mTail;
    be<u32> mLength;
    TpotaParticleLink* getFirst() { return mHead; }
};
struct TpotaEmitterView : JPABaseEmitter {
    u8 _70[0x1AC - 0x70];
    TpotaParticleList mActiveParticles;
    u8 _1B8[0x254 - 0x1B8];
    be<u32> mFlags;
    TpotaParticleList* getParticleList() { return &mActiveParticles; }
    void becomeInvalidEmitter() {
        // HD: load flags before storing the maximum frame.
        u32 flags = mFlags;
        mMaxFrame = -1;
        mFlags = flags | 1;
    }
};
WWHD_OFFSET(TpotaEmitterView, mActiveParticles, 0x1AC);
WWHD_OFFSET(TpotaEmitterView, mFlags, 0x254);
class daTpota_c : public fopAc_ac_c {
public:
    daTpota_c() { fopAc_ac_c_ct(this); __vtbl = 0x10040A58; } // HD: virtual destructor

    struct unknown_struct {
        /* 0x0 */ gptr<TpotaParticleView> ptcl;
        /* 0x4 */ be<f32> pos_y;
    };

    cPhs_State _create();
    bool _delete();
    void make_ripple(cXyz* i_position);
    BOOL check_water_h(TpotaParticleView* i_ptcl, f32 i_position_y);
    void clear_splash();
    void renew_splash();
    bool _execute();

    /* 0x3AC */ request_of_phase_process_class mPhase; /* unused */
    /* 0x3B4 */ gptr<TpotaEmitterView> mpEmitters[2];
    /* 0x3BC */ cXyz mPositions[2];
    /* 0x3D4 */ csXyz mAngles[2];
    /* 0x3E0 */ unknown_struct field_0x2C4[30];
    /* 0x4D0 */ u8 field_0x3B4[0x500 - 0x4D0];
};
WWHD_OFFSET(daTpota_c, mpEmitters, 0x3B4);
WWHD_OFFSET(daTpota_c, mAngles, 0x3D4);
WWHD_OFFSET(daTpota_c, field_0x2C4, 0x3E0);


enum { ID_IT_SN_TAKIURA_HAMON00 = 0x82B0 };
