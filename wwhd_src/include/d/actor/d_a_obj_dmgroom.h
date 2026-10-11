/* Damaged-room demo object, WWHD layout. */
#pragma once
#include "gc_api/gc_api.h"

/* Existing unit-local bindings; HD constructor and init have distinct addresses. */
void mDoExt_brkAnm_ct(void*);
BOOL mDoExt_brkAnm_init_dmgroom(void*, J3DModelData*, J3DAnmTevRegKey*, s32, s32, f32, s16, s16, s32, s32);
struct dDemo_actor_c;
dDemo_actor_c* dDemo_object_getActor(u32, u8);

/* HD: mDoExt_brkAnm is 0x78; its frame controller starts at zero.
 * The shared mDoExt_brkAnm is opaque, so this unit owns the measured layout view. */
struct mDoExt_brkAnm_dmgroom : mDoExt_baseAnm {
    /* 0x10 */ u8 _10[0x78 - 0x10];
    BOOL init(J3DModelData* data, J3DAnmTevRegKey* brk, s32 play, s32 mode) {
        return mDoExt_brkAnm_init_dmgroom(this, data, brk, play, mode, 1.0f, 0, -1, false, 0);
    }
    void entry(J3DModelData* data) {
        mDoExt_brkAnm_entry(reinterpret_cast<mDoExt_brkAnm*>(this), data, getFrame());
    }
    void setFrame(f32 frame) { mFrameCtrl.setFrame(frame); }
    void remove(J3DModelData* data) {
        // HD: the inline remove clears the model data's TEV register animator at +0x48.
        struct ModelDataAnimation {
            u8 _00[0x48];
            gptr<J3DAnmTevRegKey> mTevRegAnimator;
        };
        reinterpret_cast<ModelDataAnimation*>(data)->mTevRegAnimator = nullptr;
    }
};
WWHD_SIZE(mDoExt_brkAnm_dmgroom, 0x78);

/* Demo actor fields match the GameCube layout at the verified offsets. */
struct dDemo_actor_c {
    enum { ENABLE_ANM_FRAME_e = 1 << 6 };
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mEnable;
    /* 0x06 */ u8 _06[0x30 - 6];
    /* 0x30 */ be<f32> mAnmFrame;
    bool checkEnable(u16 flag) { return (mEnable & flag) != 0; }
    f32 getAnmFrame() { return mAnmFrame; }
};
struct dDemo_object_c {
    dDemo_actor_c* getActor(u8 id) { return dDemo_object_getActor(gabi::ea(this), id); }
};

enum {
    dRes_INDEX_DMGROOM_BDL_DMGROOM_e = 4,
    dRes_INDEX_DMGROOM_BRK_DMGROOM_e = 7,
};
class daObjDmgroom_c : public fopAc_ac_c {
public:
    daObjDmgroom_c() {
        fopAc_ac_c_ct(this);
        __vtbl = 0x10027254; // HD: this translation unit's actor vtable.
        mDoExt_brkAnm_ct(&mBrkAnm);
    }
    BOOL CreateHeap();
    void CreateInit();
    void set_mtx();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ mDoExt_brkAnm_dmgroom mBrkAnm;
};
WWHD_OFFSET(daObjDmgroom_c, mBrkAnm, 0x3B8);
WWHD_SIZE(daObjDmgroom_c, 0x430);
