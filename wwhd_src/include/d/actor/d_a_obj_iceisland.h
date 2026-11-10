#pragma once
#include "gc_api/gc_api.h"
// HD: particle colors and flags used by this actor.
struct IceEmitter {
    u8 pad0[0x5C];
    be<u32> mDeleteID;
    u8 pad60[0x244 - 0x60];
    be<u8> mPrmColor[4], mEnvColor[4];
    u8 pad24C[8];
    be<u32> mFlags;
};
struct IceBrkAnm : mDoExt_baseAnm {
    u8 pad[0x68];
};
struct IceLightRecord {
    be<f32> head[6];
    be<u32> colors[3];
    be<f32> tail[8];
};
struct IceTevStr {
    IceLightRecord first;
    u8 pad44[0x98 - 0x44];
    GXColor mColorK0;
    u8 pad9C[0xC0 - 0x9C];
    IceLightRecord second;
    u8 pad104[0x144 - 0x104];
    IceLightRecord third;
    u8 pad188[0x1C0 - 0x188];
    u8 color(u32 i) {
        return reinterpret_cast<be<u8> *>(&mColorK0)[i];
    }
};
struct daObjIceisland_c : fopAc_ac_c {
    enum State {
        Frozen,
        WaitMelt,
        MeltDemo,
        Melted,
        WaitFreeze,
        FreezeDemo,
        WaitFailure,
        FailureDemo
    };
    BOOL CreateHeap();
    void set_mtx();
    void daObjIceisland_particle_set();
    void CreateInit();
    s32 _create();
    BOOL _delete();
    BOOL _draw();
    void daObjIceisland_freeze_main();
    void daObjIceisland_melt_demo_wait();
    void daObjIceisland_melt_demo();
    void daObjIceisland_melt_main();
    void daObjIceisland_freeze_demo_wait();
    void daObjIceisland_freeze_demo();
    void daObjIceisland_fail_demo_wait();
    u32 daObjIceisland_fail_demo_main();
    BOOL _execute();
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ mDoExt_btkAnm mBtkAnm1;
    /* 0x42C */ mDoExt_btkAnm mBtkAnm2;
    /* 0x4A0 */ IceBrkAnm mBrkAnm;
    /* 0x518 */ gptr<IceEmitter> mEmitter1;
    /* 0x51C */ gptr<IceEmitter> mEmitter2;
    /* 0x520 */ IceTevStr mTevStr;
    /* 0x6E0 */ u8 field_0x394[10];
    /* 0x6EA */ be<s16> mWindSeVolume;  /* was field_0x396: _execute 0235F8F0: counted down to 0 while the switch is on and up to 100 while it is off, then passed as the iceberg wind sound's volume (tww fopAcM_seStartCurrent(this, JA_SE_ATM_ICEBERG_WIND, it)); CreateInit 0235F0A0 0 when melted, 100 when frozen (tww) */
    /* 0x6EC */ be<s16> mMeltIceEventIdx;
    /* 0x6EE */ be<s16> mFreezeIceEventIdx;
    /* 0x6F0 */ be<u32> mState;  /* was field_0x39C: the State enum (Frozen .. FailureDemo): CreateInit 0235F0A0 and the demo steps 0235F420 / 0235F498 / 0235F5E0 / 0235F658 / 0235F740 set it; _execute 0235F8F0 switches on it (tww switch (it) with 0 .. 7) */
};
WWHD_OFFSET(daObjIceisland_c, mpModel, 0x3B4);
WWHD_OFFSET(daObjIceisland_c, mEmitter1, 0x518);
WWHD_OFFSET(daObjIceisland_c, mWindSeVolume, 0x6EA);
WWHD_OFFSET(daObjIceisland_c, mState, 0x6F0);
WWHD_SIZE(daObjIceisland_c, 0x6F4);
struct IceModel {
    u8 pad[0x74];
    be<u32> mFlags;
};
struct IceParticleController;
struct IcePlay {
    u8 pad0[0x52C4];
    u8 mEventManager;
    u8 pad52C5[0x5AB0 - 0x52C5];
    gptr<IceParticleController> mParticle;
};
inline IcePlay *icePlay() {
    return gabi::at<IcePlay>(dComIfGp_ea());
}
struct IceSave {
    u8 pad0[0x1148];
    be<s8> mRestartRoomNo;
    u8 pad1149[0x115C - 0x1149];
    be<s16> mStartPoint;
};
inline IceSave *iceSave() {
    return gabi::at<IceSave>(dComIfGs_ea());
}
struct IceHeaderZeros {
    be<u32> words[4];
};
struct IcePiPair {
    be<f32> values[2];
};
struct IceResourceManager {
    gptr<void> mpManager;
};
struct ResourceName {
    be<u32> data, vtable;
};
J3DModelData *iceResource(void *, ResourceName *, s32);
void iceOrder(daObjIceisland_c *, s16, s32, s32, s32, s32);
s32 iceBtkInit(void *, void *, void *, s32, s32, s32, f32, s32, s32, s32);
s32 iceBrkInit(void *, void *, void *, s32, s32, s32, f32, s32, s32, s32);
void iceModelFlags(J3DModel *, u32);
void iceBrkCtor(IceBrkAnm *);
IceEmitter *iceParticleSet(IceParticleController *, s32, u32, cXyz *, csXyz *, s32, s32, s32, s32,
                           s32, s32, s32);
void iceTevInit(IceTevStr *, s8, s32);
void iceSettingTev(dScnKy_env_light_c *, s32, cXyz *, void *);
s16 iceEventIdx(u8 *, const char *, s32);
BOOL iceEventEnd(u8 *, s16);
void iceBtkEntry(mDoExt_btkAnm *, J3DModel *, f32);
void iceBrkEntry(IceBrkAnm *, J3DModelData *, f32);
u32 iceAnmPlay(void *);
f32 icePow(f32, f32);
void iceInitA(void *);
void iceInitB(void *);
void iceActorDt(void *, s32);

void iceRiddleSound(u32);
