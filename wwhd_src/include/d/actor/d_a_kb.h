/* kb_class (pig), WWHD layout. 
 *
 * GameCube -> WWHD: the two dKy_tevstr_c grew (0xB0 -> 0x1C8 each), mShadowId is gone (HD: no
 * blob shadow), so the members from mPhs on are +0x348. The eye animation (GameCube m500
 * J3DAnmTexPattern*, mpTexNoAnm, m508..m50A) is an inline mDoExt_btpAnm (0x74) in HD, so the
 * members from m50C on are +0x3B0; the particle callbacks are +0x39C and mpMaterialTable is
 * gone, so the collision members are +0x398. Size 0xE2C (constructor 02193AC4).
 * Offsets from the verified functions. */
#pragma once
#include "bindings.h"
#include "d/actor/d_a_player.h"
#include "d/d_cc_uty.h"
#include "gc_api/JSystem/JPAEmitter.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"

/* dPa_rippleEcallBack (HD 0x14): vtable, emitter, ..., rate at +0x10 */
struct KbRippleCallbackHD {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ gptr<JPABaseEmitter> mpEmitter;
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<f32> mRate;
};
struct kb_class : fopAc_ac_c {
    /* 0x3AC */ dKy_tevstr_c mTevStr;
    /* 0x574 */ dKy_tevstr_c m340;
    /* 0x73C */ request_of_phase_process_class mPhs;
    /* 0x744 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x748 */ be<u8> mShapeType;
    /* 0x749 */ be<u8> m401;
    /* 0x74A */ be<u8> mbCanBeBigPig;
    /* 0x74B */ be<u8> m403;
    /* 0x74C */ be<u8> m404;
    /* 0x74D */ be<u8> m405;
    /* 0x74E */ be<u8> m406;
    /* 0x74F */ be<u8> m407;
    /* 0x750 */ be<u8> m408;
    /* 0x751 */ be<u8> m409;
    /* 0x752 */ be<u8> m40A;
    /* 0x753 */ be<u8> m40B;
    /* 0x754 */ gptr<dPath> mpPath;
    /* 0x758 */ u8 m410[0x765 - 0x758];
    /* 0x765 */ be<u8> m41D;
    /* 0x766 */ be<s16> m41E;
    /* 0x768 */ be<s16> m420;
    /* 0x76A */ be<s16> m422;
    /* 0x76C */ be<s16> m424;
    /* 0x76E */ be<s16> m426[8];
    /* 0x77E */ be<s16> m436;
    /* 0x780 */ be<s16> m438;
    /* 0x782 */ be<s16> m43A;
    /* 0x784 */ u8 m43C[4];
    /* 0x788 */ be<s16> m440;
    /* 0x78A */ be<s16> m442;
    /* 0x78C */ be<s16> m444;
    /* 0x78E */ be<s16> m446;
    /* 0x790 */ be<s16> m448;
    /* 0x792 */ be<s16> m44A;
    /* 0x794 */ be<s16> m44C;
    /* 0x796 */ be<s16> m44E;
    /* 0x798 */ cXyz m450;
    /* 0x7A4 */ cXyz m45C;
    /* 0x7B0 */ cXyz field_0x468[2];
    /* 0x7C8 */ cXyz field_0x480[2];
    /* 0x7E0 */ cXyz field_0x498;
    /* 0x7EC */ cXyz m4A4;
    /* 0x7F8 */ be<f32> m4B0;
    /* 0x7FC */ be<f32> m4B4;
    /* 0x800 */ be<f32> m4B8;
    /* 0x804 */ be<f32> m4BC;
    /* 0x808 */ be<f32> m4C0;
    /* 0x80C */ be<f32> m4C4;
    /* 0x810 */ be<f32> m4C8;
    /* 0x814 */ be<f32> m4CC;
    /* 0x818 */ u8 m4D0[4];
    /* 0x81C */ be<f32> m4D4;
    /* 0x820 */ be<u32> m4D8;
    /* 0x824 */ cXyz m4DC;
    /* 0x830 */ cXyz m4E8;
    /* 0x83C */ csXyz m4F4;
    /* 0x842 */ csXyz m4FA;
    /* 0x848 */ mDoExt_btpAnm mBtp; /* HD: GameCube m500/mpTexNoAnm/m508..m50A */
    /* 0x8BC */ be<s32> m50C;
    /* 0x8C0 */ cXyz m514;    /* demo camera eye */
    /* 0x8CC */ cXyz m520;    /* demo camera centre */
    /* 0x8D8 */ be<f32> m53C; /* demo camera fovy */
    /* 0x8DC */ KbRippleCallbackHD m540;
    /* 0x8F0 */ dPa_smokeEcallBack m554;
    /* 0x910 */ dPa_smokeEcallBack m574;
    /* 0x930 */ dPa_followEcallBack m594;
    /* 0x944 */ dPa_followEcallBack m5A8[2];
    /* 0x96C */ be<f32> m5D0;
    /* 0x970 */ cXyz m5D4[2];
    /* 0x988 */ csXyz m5EC[2];
    /* 0x994 */ dBgS_AcchCir mAcchCir;
    /* 0x9D4 */ dBgS_ObjAcch mAcch;
    /* 0xB98 */ dCcD_Stts mStts;
    /* 0xBD4 */ dCcD_Sph mSph;
    /* 0xD00 */ dCcD_Sph m968;
};
WWHD_OFFSET(kb_class, mPhs, 0x73C);
WWHD_OFFSET(kb_class, m41E, 0x766);
WWHD_OFFSET(kb_class, m4D8, 0x820);
WWHD_OFFSET(kb_class, mBtp, 0x848);
WWHD_OFFSET(kb_class, m540, 0x8DC);
WWHD_OFFSET(kb_class, m5D0, 0x96C);
WWHD_OFFSET(kb_class, mAcchCir, 0x994);
WWHD_OFFSET(kb_class, mStts, 0xB98);
WWHD_OFFSET(kb_class, m968, 0xD00);
WWHD_SIZE(kb_class, 0xE2C);

// HD: named virtual slots retain the native dispatch and object argument.
struct KbCallbackVtable {
    u8 prefix[0x44];
    gfn remove;
};
struct KbCallbackObject {
    gptr<KbCallbackVtable> __vtbl;
};
struct KbSafeStringVtable {
    u8 prefix[0x14];
    gfn assureTermination;
};
struct KbGrabVtable {
    u8 prefix[0x44];
    gfn getGrabMissActor;
};
WWHD_OFFSET(KbCallbackVtable, remove, 0x44);
WWHD_OFFSET(KbSafeStringVtable, assureTermination, 0x14);
WWHD_OFFSET(KbGrabVtable, getGrabMissActor, 0x44);
struct KbJ3DSysState {
    gptr<J3DModel> mModel;
};
extern KbJ3DSysState &l_j3dSys;
extern Mtx34 &l_currentMtx;
inline dComIfG_play_c *kbPlay();

/* acch members */
inline f32 kb_wtr_height(kb_class *i_this) {
    return i_this->mAcch.m_wtr.GetHeight();
} /* m_wtr.GetHeight() */
inline cBgS_PolyInfo *kb_gnd_poly(kb_class *i_this) {
    return &i_this->mAcch.m_gnd;
}

void kbGuest_cMtx_YrotS(Mtx34 *matrix, u32 angle);
u32 kbGuest_followEcallBack_ct(void *p, u8 a, u8 b);
void kbGuest_dCcD_Sph_ct(dCcD_Sph *p);
void kbGuest_dCcD_Sph_dt(dCcD_Sph *p, s32 flags);
void kbGuest_dBgS_Acch_dt(u32 p, s32 flags);
void kbGuest_cM3dGCir_dt(u32 p, s32 flags);
void kbGuest_fopAc_ac_c_dt(kb_class *p, s32 flags);
void kbGuest_cBgS_GndChk_dt(void *p, s32 flags);
void kbGuest_dCamera_ForceLockOff(u32 camera, u32 id);
void *kbGuest_getJointTree(J3DModelData *p);
/* ---- this TU's constants ---- */
#define SAFESTRING_VTBL 0x100127E0 /* this TU's sead::SafeString vtable */
enum { PROC_ESA = 0xDD, PROC_TAG_KB_ITEM = 0x1A1 };
enum { DSNAP_TYPE_KB = 0x54 };
enum { PG_JNT_J_PG_TAIL_e = 12 };
enum {
    dRes_INDEX_KB_BAS_JITA2_e = 9,
    dRes_INDEX_KB_BCK_JITA2_e = 0x14,
};
enum {
    JA_SE_CV_PG_NORMAL = 0x4820,
    JA_SE_CV_PG_TURN = 0x4821,
    JA_SE_CV_PG_CATCH = 0x4822,
    JA_SE_CV_PG_CARRY = 0x4823,
    JA_SE_CV_PG_L_NORMAL = 0x4915,
    JA_SE_CV_PG_L_TURN = 0x4916,
    JA_SE_CV_PG_L_CATCH = 0x4917,
    JA_SE_CV_PG_L_CARRY = 0x4918,
    JA_SE_OBJ_FALL_WATER_S = 0x6918,
};
/* file statics */
struct KbStatics {
    be<u8> ALL_ANGER, DEMO_START;
};
WWHD_SIZE(KbStatics, 2);
extern KbStatics &l_kbStatics;

/* this TU's dBgS_LinChk vtables (constructor and destructor) */
static const dBgS_LinChk_vt LINCHK_VT = {0x10012888, 0x10012898, 0x100128B8, 0x100128A8};
static inline void dBgS_LinChk_dt(void *c) {
    auto *check = (dBgS_LinChk *)c;
    check->dBgS_PolyPassChk::__vtbl = LINCHK_VT.v58;
    check->dBgS_GrpPassChk::__vtbl = 0x10012808;
    check->cBgS_PolyInfo::__vtbl = 0x100127F8;
    cBgS_LinChk_dt(c, 0);
}

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* esa_class (HD +0x11C): field_0x298 (u8) at 0x3B4, mState (s8) at 0x3BD */

/* attention_info.flags (+0x39C) */
static inline be<u32> &attn_flags(fopAc_ac_c *a) {
    return a->attention_info.flags;
}
static inline bool fopAcM_checkCarryNow(fopAc_ac_c *a) {
    return (a->actor_status & 0x2000) != 0;
}
/* 025E1AA4 kbGuest_mDoAud_monsSeStart(id, pos, procId, param, reverb) */
void kbGuest_mDoAud_monsSeStart(u32 id, cXyz *pos, u32 pid, u32 param, s32 reverb);
/* fopAcM_seStart / fopAcM_monsSeStart: the HD inlines test &eyePos only */
static inline void kb_se_start(fopAc_ac_c *a, u32 id) {
    cXyz *eye = &a->eyePos;
    if (gabi::ea(eye) != 0)
        mDoAud_seStart(id, eye, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
static inline void kb_mons_se_start(fopAc_ac_c *a, u32 id) {
    cXyz *eye = &a->eyePos;
    if (gabi::ea(eye) != 0) {
        u32 pid = a != nullptr ? (u32)a->mBsPcId : 0xFFFFFFFFu; /* fopAcM_GetID */
        kbGuest_mDoAud_monsSeStart(id, eye, pid, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
    }
}
static inline void kb_catch_se(kb_class *i_this) {
    if (i_this->mShapeType >= 8) {
        kb_mons_se_start(i_this, JA_SE_CV_PG_L_CATCH);
    } else {
        kb_mons_se_start(i_this, JA_SE_CV_PG_CATCH);
    }
}
// HD: full projection of the emitter fields used by the pig's smoke and water effects.
struct KbEmitterHD {
    u8 prefix[0x34];
    be<f32> mRate;
    u8 pad38[0x20];
    be<f32> mSpread;
    be<s32> mMaxFrame;
    be<s16> mLifeTime;
    u8 pad62[6];
    be<f32> mInitialVelOmni, mInitialVelAxis, mInitialVelDir;
    u8 pad74[0x1F0 - 0x74];
    be<f32> mGlobalRotation[9];
    u8 pad214[0xC];
    cXyz mGlobalScale, mGlobalTranslation, mGlobalParticleScale;
};
WWHD_OFFSET(KbEmitterHD, mSpread, 0x58);
WWHD_OFFSET(KbEmitterHD, mGlobalRotation, 0x1F0);
WWHD_OFFSET(KbEmitterHD, mGlobalScale, 0x220);
WWHD_OFFSET(KbEmitterHD, mGlobalTranslation, 0x22C);
WWHD_OFFSET(KbEmitterHD, mGlobalParticleScale, 0x238);
inline KbEmitterHD *kbEmitter(u32 address) {
    return gabi::at<KbEmitterHD>(address);
}

/* particle emitters: JPABaseEmitter::setGlobalScale (HD: +0x220.. and +0x238..) */
template <class CB> static inline u32 emitter_ea(CB &cb) {
    return gabi::ea(cb.mpEmitter.get());
}
static inline void set_global_scale(u32 e, f32 x, f32 y, f32 z) {
    kbEmitter(e)->mGlobalScale.x = x;
    kbEmitter(e)->mGlobalScale.y = y;
    kbEmitter(e)->mGlobalScale.z = z;
    kbEmitter(e)->mGlobalParticleScale.x = x;
    kbEmitter(e)->mGlobalParticleScale.y = y;
    kbEmitter(e)->mGlobalParticleScale.z = z;
}
/* virtual remove() (vtable +0x44) of the particle callbacks */
template <class CB> static inline void vremove(CB *cb) {
    gabi::call_ptr(gabi::at<KbCallbackObject>(gabi::ea(cb))->__vtbl->remove.get(), cb);
}
void kbGuest_JPASetRMtxTVecfromMtx(Mtx34 *m, u32 r, u32 t);
/* J3D (HD): j3dSys.mModel 0x104B462C, J3DSys::mCurrentMtx 0x104B4868, joint block at model+0x2C */
static inline Mtx34 *getAnmMtx(J3DModel *model, s32 joint) {
    return model->getAnmMtx(joint);
}
static inline J3DModel *j3dSys_getModel() {
    return l_j3dSys.mModel;
}
static inline Mtx34 *J3DSys_mCurrentMtx() {
    return &l_currentMtx;
}
/* dBgS::GetRoomId / GetPolyColor */
s32 kbGuest_dBgS_GetRoomId(dBgS *bgs, cBgS_PolyInfo *p);
s32 kbGuest_dBgS_GetPolyColor(dBgS *bgs, cBgS_PolyInfo *p);
/* 0200FAAC cLib_distanceAngleS: HD returns abs((s16)(a - b)) as an int (0..0x8000); callers compare
 * the whole register (the shared binding's s16 result would turn 0x8000 negative) */
s32 kbGuest_cLib_distanceAngleS_i(s16 a, s16 b);
/* 025D8AB0 kbGuest_fopAcM_fastCreateItem(pos, itemNo, roomNo, rot, scale, speedF, speedY, gravity,
 * bitNo, createFunc) */
fopAc_ac_c *kbGuest_fopAcM_fastCreateItem(cXyz *pos, s32 itemNo, s32 roomNo, csXyz *rot,
                                          cXyz *scale, f32 speedF, f32 speedY, f32 gravity,
                                          s32 bitNo, u32 createFunc);
void kbGuest_dKy_tevstr_init(dKy_tevstr_c *t, s8 roomNo, u8 p);
/* 027F3F94 (the matcher names it __nw): returns the model data's joint tree; joint count (u16) at
 * +8 */
static inline u16 J3DModelData_getJointNum(J3DModelData *d) {
    return d->getJointNum();
}
/* J3DModelData::getJointNodePointer(i)->setCallBack(cb) (HD: nodes of 0x1C bytes from +8; index
 * checked against +4) */
static inline void setJointCallBack(J3DModelData *d, u16 i, u32 cb) {
    d->getJointNodePointer(i)->mCallBack = cb;
}
/* mDoExt_btpAnm (HD): constructor 025E7820, init 025E789C */
void kbGuest_mDoExt_btpAnm_ct(void *p);
BOOL kbGuest_mDoExt_btpAnm_init(void *a, J3DModelData *d, void *btp, s32 anmPlay, s32 mode,
                                f32 rate, s16 start, s16 end, bool modify, s32 entry);
/* 025A9084 dPa_rippleEcallBack::dPa_rippleEcallBack, 025A5B18
 * dPa_smokeEcallBack::dPa_smokeEcallBack(u8) */
void kbGuest_dPa_rippleEcallBack_ct(void *p);
void kbGuest_dPa_smokeEcallBack_ct(void *p, u8 a);
/* GHS array helpers: kbGuest___construct_array(ptr, n, size, ctor), kbGuest___destroy_arr(ptr, n,
 * size, dtor, flags) */
void kbGuest___construct_array(void *p, u32 n, u32 size, u32 ctor);
void kbGuest___destroy_arr(void *p, u32 n, u32 size, u32 dtor, u32 flags);
/* sead::SafeString equality (HD inline): both sides' virtual assureTermination (+0x14), then a
 * byte compare bounded by 0x40001 */
static inline bool SafeString_eq(SafeString *a, SafeString *b) {
    gabi::call_ptr(gabi::at<KbSafeStringVtable>(a->__vtbl)->assureTermination.get(), a);
    gabi::call_ptr(gabi::at<KbSafeStringVtable>(a->__vtbl)->assureTermination.get(), a);
    u32 s1 = a->mStringTop;
    gabi::call_ptr(gabi::at<KbSafeStringVtable>(b->__vtbl)->assureTermination.get(), b);
    if (s1 == b->mStringTop)
        return true;
    u32 p = a->mStringTop, q = b->mStringTop;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c = gabi::load<u8>(p + i);
        if (c != gabi::load<u8>(q + i))
            return false;
        if (c == 0)
            return true;
    }
    return false;
}
/* strcmp(dComIfGp_getStartStageName(), lit) == 0 (HD: SafeString comparison; the start stage name
 * at play+0x5134) */
static inline bool kbIsStartStage(const char *lit) {
    gabi::Local<SafeString> a;
    a->mStringTop = gabi::guest_str(lit);
    a->__vtbl = SAFESTRING_VTBL;
    gabi::Local<SafeString> b;
    b->mStringTop = gabi::ea(kbPlay()->mCurStage.mName);
    b->__vtbl = SAFESTRING_VTBL;
    return SafeString_eq(a, b);
}
/* dBgS_GndChk on the stack (this TU's vtables) */
static const dBgS_GndChk_vt GNDCHK_VT = {0x10012818, 0x10012828, 0x10012848, 0x10012838};
static inline void dBgS_GndChk_dt(void *c) {
    auto *check = (dBgS_GndChk *)c;
    check->cBgS_PolyInfo::__vtbl = GNDCHK_VT.v20;
    check->dBgS_PolyPassChk::__vtbl = GNDCHK_VT.v40;
    check->dBgS_GrpPassChk::__vtbl = 0x10012808;
    kbGuest_cBgS_GndChk_dt(c, 0); /* cBgS_GndChk::~cBgS_GndChk */
}
#define KB_VTBL 0x100128C8 /* kb_class vtable (HD virtual destructor) */
static const dBgS_ObjAcch_vt OBJACCH_VT = {0x10012858, 0x10012878, 0x10012868};

enum { dIsleRoom_WindfallIsland_e = 11, dIsleRoom_OutsetIsland_e = 44 };
enum { dRes_INDEX_KB_BAS_WAIT1_e = 0xC, dRes_INDEX_KB_BCK_WAIT1_e = 0x17 };
enum { JA_SE_OBJ_LUPY_OUT = 0x69E9, JA_SE_OBJ_ITEM_OUT = 0x69ED };
/* kb_btp_idx (0x101B7FC4) / kb_bdl_idx (HD, 0x101B7FD4): u16[7] */

/* daPy_py_c::getGrabMissActor (virtual, HD vtable at +0xB4, slot +0x44) */
static inline fopAc_ac_c *daPy_getGrabMissActor(fopAc_ac_c *player) {
    return gabi::call_ptr<fopAc_ac_c *>(
        gabi::at<KbGrabVtable>(player->__vtbl)->getGrabMissActor.get(), player);
}
enum {
    dRes_INDEX_KB_BCK_DAMAGE1_e = 0x10,
    dRes_INDEX_KB_BAS_RUN1_e = 0xB,
    dRes_INDEX_KB_BCK_RUN1_e = 0x16
};
/* 02515BBC dCcD_GAtTgCoCommonBase::GetAc */
fopAc_ac_c *kbGuest_dCcD_GAtTgCoCommonBase_GetAc(void *p);
static inline void kb_mons_se2(kb_class *i_this, u32 big, u32 normal) {
    if (i_this->mShapeType >= 8) {
        kb_mons_se_start(i_this, big);
    } else {
        kb_mons_se_start(i_this, normal);
    }
}
enum {
    dRes_INDEX_KB_BAS_EAT1_e = 7,
    dRes_INDEX_KB_BAS_NAKU1_e = 0xA,
    dRes_INDEX_KB_BAS_WALK1_e = 0xD,
    dRes_INDEX_KB_BCK_EAT1_e = 0x12,
    dRes_INDEX_KB_BCK_NAKU1_e = 0x15,
    dRes_INDEX_KB_BCK_WALK1_e = 0x18,
};
enum {
    JA_SE_CM_PG_DIG = 0x5921,
    JA_SE_CM_PG_DIG_LANDING = 0x5922,
    JA_SE_CM_PG_L_JUMP = 0x593E,
    JA_SE_CM_PG_L_DIG = 0x593F,
};
enum { PROC_KS = 0xCD };
/* daPy_py_c demo (HD: mDemo at +0x420: type u16 +0x420, param0 +0x428, mode +0x430) */
enum {
    daPy_demo_DEMO_N_WAIT_e = 1,
    daPy_demo_DEMO_UNK_018_e = 0x12,
    daPy_demo_DEMO_SMILE_e = 0x32
};
static inline void daPy_changeOriginalDemo(fopAc_ac_c *pl) {
    ((daPy_py_c *)pl)->mDemo.mParam0 = 0;
    ((daPy_py_c *)pl)->mDemo.mDemoType = 3;
}
static inline void daPy_cancelOriginalDemo(fopAc_ac_c *pl) {
    ((daPy_py_c *)pl)->mDemo.mDemoType = 2;
    ((daPy_py_c *)pl)->mDemo.mDemoMode = 1;
}
static inline void daPy_changeDemoMode(fopAc_ac_c *pl, u32 mode) {
    ((daPy_py_c *)pl)->mDemo.mDemoMode = mode;
}
/* camera_process_class* dComIfGp_getCamera(dComIfGp_getPlayerCameraID(0)): play+0x5AF8 + id * 0x34,
 * id (s8) at play+0x5B30; its dCamera_c at +0x248 */
static inline u8 *kb_getPlayerCamera() {
    s8 id = dComIfGp_getPlayerCameraID(0);
    return reinterpret_cast<u8 *>(dComIfGp_getCamera(id));
}
// HD: the camera process prefix is opaque in the shared framework layout.
struct KbCameraProcessHD {
    u8 prefix[0x248];
    u8 mCamera;
};
WWHD_OFFSET(KbCameraProcessHD, mCamera, 0x248);
static inline dCamera_c *camera_body(u8 *cam) {
    return reinterpret_cast<dCamera_c *>(&gabi::at<KbCameraProcessHD>(gabi::ea(cam))->mCamera);
}
void kbGuest_dCamera_Stop(dCamera_c *c);
void kbGuest_dCamera_Start(dCamera_c *c);
void kbGuest_dCamera_SetTrimSize(dCamera_c *c, s32 s);
/* Set(cXyz center, cXyz eye, f32 fovy, s16 bank) / Reset(cXyz center, cXyz eye): by value (pointers
 * to copies) */
void kbGuest_dCamera_Set(dCamera_c *c, cXyz *center, cXyz *eye, f32 fovy, s16 bank);
void kbGuest_dCamera_Reset(dCamera_c *c, cXyz *center, cXyz *eye);
BOOL kbGuest_fopAcM_orderPotentialEvent(fopAc_ac_c *a, u16 type, u16 flag, u16 p);
s32 kbGuest_dBgS_GetSpecialCode(dBgS *bgs, cBgS_PolyInfo *p);
s32 kbGuest_dBgS_GetAttributeCode(dBgS *bgs, cBgS_PolyInfo *p);
/* 02526560 daTagKbItem_c::kb_dig(fopAc_ac_c*) (unnamed by the matcher) */
void kbGuest_daTagKbItem_kb_dig(void *item, fopAc_ac_c *pig);
/* fopAcM_seStart / monsSeStart variants that also test the actor */
static inline void kb_se_start_a(fopAc_ac_c *a, u32 id) {
    if (a != nullptr)
        kb_se_start(a, id);
}
static inline void kb_mons_se_start_a(fopAc_ac_c *a, u32 id) {
    if (a != nullptr)
        kb_mons_se_start(a, id);
}
enum {
    dRes_INDEX_KB_BAS_JITA1_e = 8,
    dRes_INDEX_KB_BCK_DASSUI_e = 0x11,
    dRes_INDEX_KB_BCK_JITA1_e = 0x13
};
enum { daPyStts0_CRAWL_e = 0x08000000 };
/* 025192A8 kbGuest_cc_at_check(fopAc_ac_c*, CcAtInfo*) (SHARED-CANDIDATE, as in d_a_pt/d_a_ph) */
void kbGuest_cc_at_check(fopAc_ac_c *a, CcAtInfo *info);
/* dComIfGs_getSelectEquip(0): save info (0x101F84DC) +0x2E */
static inline u8 dComIfGs_getSelectEquip0() {
    return dComIfGs_getSelectEquip(0);
}
void kbGuest_cBgS_GetTriPnt(dBgS *bgs, void *poly, cXyz *a, cXyz *b, cXyz *c);
/* dComIfGp_getCamera(0)->mCamera.ForceLockOff(id) */
static inline void dCamera_ForceLockOff(fopAc_ac_c *a) {
    dCamera_c *camera = camera_body(reinterpret_cast<u8 *>(dComIfGp_getCamera(0)));
    kbGuest_dCamera_ForceLockOff(gabi::ea(camera), fopAcM_GetID(a));
}
void kbGuest_dCcMassS_Mng_Set(void *manager, void *obj, u8 p);

// Cross-file calls in this translation unit.
u32 kbGuest_way_check_raw(kb_class *i_this, s16 angle, u8 p);
void money_drop(kb_class *i_this);
void smoke_set3(kb_class *i_this);
void smoke_set2(kb_class *i_this);
BOOL swim_mode_change_check(kb_class *i_this);
void smoke_set(kb_class *i_this);
void anm_init(kb_class *i_this, int bck, f32 morf, u8 loop, f32 speed, int bas);
void esa_demo_move(kb_class *i_this);
void attack_move(kb_class *i_this);
f32 kbGuest_JMASinShort_(u32 table, s16 a);
void speed_pos_set(kb_class *i_this);
void cLib_offBit_u32(be<u32> *v, u32 bit);
BOOL kb_eye_tex_anm(kb_class *i_this);
void BG_check(kb_class *i_this);
BOOL esa_demo_check(kb_class *i_this);
void target_set(kb_class *i_this, u8 p);
void draw_SUB(kb_class *i_this);
void he_set(kb_class *i_this);
void sibuki_set(kb_class *i_this);
BOOL carry_check(kb_class *i_this);

inline dComIfG_play_c *kbPlay() {
    return gabi::at<dComIfG_play_c>(dComIfGp_ea() + offsetof(dComIfG_inf_c, play));
}

// HD: mass manager inside the collision scene; shared cCcS leaves this member opaque.
struct KbCollisionSceneHD {
    u8 prefix[0x2854];
    u8 mMassManager;
};
WWHD_OFFSET(KbCollisionSceneHD, mMassManager, 0x2854);
inline void *kbMassManager() {
    return &gabi::at<KbCollisionSceneHD>(gabi::ea(&kbPlay()->mCcS))->mMassManager;
}

// HD: three enlarged light objects in tev state; typed fields preserve the native copy order.
struct KbLightObjectHD {
    cXyz position, direction;
    be<u8> color[4];
    be<s16> ambient[4];
    be<f32> intensity[2], cosAtten[3], distanceAtten[3];
    u8 unchangedTail[0x40];
};
WWHD_SIZE(KbLightObjectHD, 0x84);
WWHD_OFFSET(KbLightObjectHD, intensity, 0x24);
struct KbLightingHD {
    KbLightObjectHD light;
    u8 colorAndFog[0x3C];
    KbLightObjectHD materialLight, secondMaterialLight;
};
WWHD_SIZE(KbLightingHD, 0x1C8);
WWHD_OFFSET(KbLightingHD, materialLight, 0xC0);
WWHD_OFFSET(KbLightingHD, secondMaterialLight, 0x144);
struct KbAmbientBytes {
    be<u8> bytes[8];
};
struct KbTevPositionColorBytes {
    be<u8> bytes[0x30];
};
struct KbTevControlBytes {
    be<u8> bytes[9];
};
inline KbLightingHD *kbLighting(dKy_tevstr_c *t) {
    return gabi::at<KbLightingHD>(gabi::ea(t));
}
inline KbAmbientBytes *kbAmbientBytes(KbLightObjectHD *l) {
    return gabi::at<KbAmbientBytes>(gabi::ea(&l->ambient[0]));
}
inline KbTevPositionColorBytes *kbPositionColorBytes(dKy_tevstr_c *t) {
    return gabi::at<KbTevPositionColorBytes>(gabi::ea(&t->mLightPosWorld));
}
inline KbTevControlBytes *kbControlBytes(dKy_tevstr_c *t) {
    return gabi::at<KbTevControlBytes>(gabi::ea(&t->mInitTimer));
}

// HD: food actor members used by the pig; the shared actor remains opaque.
struct KbFoodActorHD : fopAc_ac_c {
    u8 prefix[8];
    be<u8> field_0x298;
    u8 pad3B5[8];
    be<s8> mState;
};
WWHD_OFFSET(KbFoodActorHD, field_0x298, 0x3B4);
WWHD_OFFSET(KbFoodActorHD, mState, 0x3BD);
struct KbJointDataHD {
    u8 prefix[4];
    be<u16> mJntNo;
};
struct KbPathPointHD {
    be<u8> mArg[4];
    cXyz m_position;
};
struct KbPathHD {
    be<u16> m_num, m_nextID;
    be<u8> mArg0, m_closed, field4_0x6, field5_0x7;
    gptr<KbPathPointHD> m_points;
};
WWHD_SIZE(KbPathPointHD, 0x10);
WWHD_OFFSET(KbPathHD, m_points, 8);
struct KbSineEntry {
    be<f32> sine, cosine;
};
struct KbPlayerVtableHD {
    u8 prefix[0x114];
    gfn setPlayerPosAndAngle;
};
WWHD_OFFSET(KbPlayerVtableHD, setPlayerPosAndAngle, 0x114);
inline be<u8> &esa_field_0x298(fopAc_ac_c *esa) {
    return ((KbFoodActorHD *)esa)->field_0x298;
}
inline s8 esa_mState(fopAc_ac_c *esa) {
    return ((KbFoodActorHD *)esa)->mState;
}

void *item_tag_search(void *param_1, void *param_2);

WWHD_SIZE(KbRippleCallbackHD, 0x14);
WWHD_OFFSET(KbRippleCallbackHD, mRate, 0x10);

extern KbSineEntry &l_sincosTable;
