/* WWHD tower pedestal. */
#pragma once
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/m_Do/m_Do_ext.h"
namespace daPedestal {
struct daPds_c;
struct PdsEmitter;
using Action = gabi::ptmf<BOOL (daPds_c::*)(void *)>;
// HD: the shared callback base is opaque; keep its verified four-byte vtable slot here.
struct daPds_infiniteEcallBack_c {
    be<u32> __vtbl;
    gptr<PdsEmitter> mpEmitter;
    gptr<cXyz> mpPos;
    gptr<csXyz> mpAngle;
    void execute(PdsEmitter *);
    void end();
    void makeEmitter(u16, const cXyz *, const csXyz *, const cXyz *);
    void setup(PdsEmitter *);
};
struct daPds_c : fopAc_ac_c {
    enum ActionStatus { ACTION_STARTING = 0, ACTION_ONGOING = 1, ACTION_ENDING = -1 };
    request_of_phase_process_class mPhase;
    gptr<J3DModel> mpModel;
    mDoExt_brkAnm mBrk;
    Mtx34 mMtx;
    gptr<dBgW> mpBgW;
    daPds_infiniteEcallBack_c mOctagonGlowCb;
    Action mAction;
    be<f32> mTargetYPos, mBrkFrame;  /* was unk308: initBrkAnm 023C85F8 stores mBrk's frame after the init (end frame for a reversed table entry); playBrkAnm 023C8D18 stores the frame after each play; nothing reads it (tww the same) */
    be<s8> mActionStatus;
    be<u8> mType, mBrkAnmIdx;  /* was unk30E: initBrkAnm 023C85F8 stores the brkAnmTbl index it initialised; nothing reads it (tww the same); after daNpc_Os_c::mBrkAnmIdx (same initBrkAnm code) */
    be<s8> mBrkAnmEnd;  /* was unk30F: playBrkAnm 023C8D18 stores mBrk's play() result (animation end); nothing reads it (tww the same); after daNpc_Os_c::mBrkAnmEnd (same playBrkAnm code) */
    be<s16> unk310, unk312, unk314, unk316;
    u8 _490[4];
    be<f32> unk31C;
    u8 _498[4];
    BOOL initBrkAnm(u8, BOOL);
    BOOL CreateHeap();
    BOOL wakeupCheck();
    BOOL finishCheck();
    void set_mtx();
    void CreateInit();
    s32 _create();
    BOOL _delete();
    void playBrkAnm();
    s32 getMyStaffId();
    BOOL eventProc();
    BOOL setAction(Action *, void *);
    void action(void *);
    BOOL _execute();
    BOOL _draw();
    void initialDefault(s32);
    BOOL actionDefault(s32);
    void initialMoveEvent(s32);
    BOOL actionMoveEvent(s32);
    void initialEffectSet(s32);
    void initialEffectEnd(s32);
    BOOL waitAction(void *);
};
WWHD_SIZE(Action, 8);
WWHD_SIZE(daPds_infiniteEcallBack_c, 0x10);
WWHD_OFFSET(daPds_c, mBrk, 0x3B8);
WWHD_OFFSET(daPds_c, mMtx, 0x430);
WWHD_OFFSET(daPds_c, mOctagonGlowCb, 0x464);
WWHD_OFFSET(daPds_c, mAction, 0x474);
WWHD_OFFSET(daPds_c, mActionStatus, 0x484);
WWHD_OFFSET(daPds_c, unk31C, 0x494);
WWHD_SIZE(daPds_c, 0x49C);
} // namespace daPedestal
