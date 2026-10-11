#pragma once
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/actor/d_a_player.h"
#include "gc_api/f_op/f_op_msg.h"
struct daObj_hsh_c;
using HshAction = gabi::ptmf<s32 (daObj_hsh_c::*)(void *)>;
struct HshEmitter;
struct daObj_hsh_c : fopAc_ac_c {
    request_of_phase_process_class mPhase;
    gptr<J3DModel> mpModel;
    dBgS_ObjAcch mObjAcch;
    dBgS_AcchCir mAcchCir;
    gptr<HshEmitter> mpEmitter;
    Mtx34 mMtx;
    gptr<dBgW> mpBgW;
    u8 field_0x4d8[12];
    cBgS_PolyInfo mPolyInfo;
    HshAction mAction;
    be<u32> mFlags, mMsgNo;
    be<s32> mSwitchNo;
    be<u32> mPrmMsgNo;
    be<s8> field_0x514, mEventSelector;
    u8 field_0x516;
    be<s8> mActionMode;
    be<u8> mAttentionLatch, mAppearDeleteTimer;
    u8 field_0x51a[2];
    be<s16> field_0x51c, field_0x51e, field_0x520, field_0x522;
    u8 field_0x524[4];
    be<f32> field_0x528;
    u8 field_0x52c[4];
    be<s16> mEventId[2];
    s16 XyCheckCB(s32);
    s16 XyEventCB(s32);
    void particle_set(u16);
    void particle_setEmitter(gptr<HshEmitter> *, u16);
    void emitterDelete(gptr<HshEmitter> *);
    void setAttention(bool);
    void onOffDraw();
    void offOffDraw();
    void drawStop();
    void drawStart();
    void setBaseMtx();
    s32 createHeap();
    s32 create();
    s32 init();
    s32 execute();
    s32 draw();
    void action(void *);
    s32 setAction(HshAction *, void *);
    s32 waitAction(void *);
    s32 talkAction(void *);
    s32 offAction(void *);
    s32 deleteAction(void *);
    void eventOrder();
    void checkOrder();
    s32 checkCommandTalk();
    bool chkAttention(cXyz *, s16);
    s32 eventProc();
    void eventEnd();
    void initialDefault(s32);
    s32 actionDefault(s32);
    void initialLinkDispEvent(s32);
    void initialMsgSetEvent(s32);
    s32 actionMsgSetEvent(s32);
    s32 actionMessageEvent(s32);
    s32 actionTactEvent(s32);
    void initialJudgeEvent(s32);
    void initialAppearEvent(s32);
    s32 actionAppearEvent(s32);
    void initialDeleteEvent(s32);
    s32 actionDeleteEvent(s32);
    s32 talk_init();
    s32 talk(s32);
    u32 getMsg();
    u32 next_msgStatus(be<u32> *);
};
WWHD_OFFSET(daObj_hsh_c, mObjAcch, 0x3B8);
WWHD_OFFSET(daObj_hsh_c, mAcchCir, 0x57C);
WWHD_OFFSET(daObj_hsh_c, mAction, 0x610);
WWHD_OFFSET(daObj_hsh_c, mFlags, 0x618);
WWHD_OFFSET(daObj_hsh_c, mEventId, 0x644);
WWHD_SIZE(daObj_hsh_c, 0x648);

// HD: HIO starts with mNo; its vtable follows the data at +0x18.
struct daObj_hsh_HIO_c {
    be<s8> mNo;
    u8 _01[3];
    struct hio_prm_c {
        be<f32> mAttnDist, field_0x0c, field_0x10;
        be<s16> mAttnAngle;
        be<s8> field_0x16;
        u8 _0F;
    } prm;
    gptr<daObj_hsh_c> mpActor;
    be<u32> __vtbl;
};
WWHD_SIZE(daObj_hsh_HIO_c, 0x1C);
