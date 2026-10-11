/* Bokoblin execution bindings and verified HD prefix layouts. */
#ifndef D_A_BK_EXEC_H
#define D_A_BK_EXEC_H
#include "d/actor/d_a_bk_local.h"
#include "d/d_path.h"
#include "gc_api/d/d_com_inf_game.h"

// HD: native scratch covers every u16 path count; no guest address escapes this buffer.
static inline u8* bkExec_pathScratch() {
    static u8 scratch[0x10000];
    return scratch;
}

struct bkExecCameraView {
    u8 padding[0xDC];
    cXyz mEye;
    cXyz mCenter;
    u8 paddingF4[0x248 - 0xF4];
    u8 mCamera;
};
WWHD_OFFSET(bkExecCameraView, mEye, 0xDC);
WWHD_OFFSET(bkExecCameraView, mCenter, 0xE8);
WWHD_OFFSET(bkExecCameraView, mCamera, 0x248);
WWHD_SIZE(bkExecCameraView, 0x249);
struct bkExecBokoView : fopAc_ac_c {
    u8 padding3AC[8];
    gptr<J3DModel> mpModel;
    u8 padding3B8[0x43A - 0x3B8];
    be<s16> m2C2;
};
WWHD_OFFSET(bkExecBokoView, mpModel, 0x3B4);
WWHD_OFFSET(bkExecBokoView, m2C2, 0x43A);
WWHD_SIZE(bkExecBokoView, 0x43C);
struct bkExecMatrixBlockView {
    u8 padding[4];
    be<u16> mFlags;
    u8 padding6[0x10 - 6];
    gptr<Mtx34> mAnmMtx;
};
WWHD_OFFSET(bkExecMatrixBlockView, mFlags, 4);
WWHD_OFFSET(bkExecMatrixBlockView, mAnmMtx, 0x10);
WWHD_SIZE(bkExecMatrixBlockView, 0x14);
struct bkExecModelView {
    u8 padding[0x2C];
    gptr<bkExecMatrixBlockView> mMatrixBlock;
    u8 padding30[0xBC - 0x30];
    cXyz mBaseScale;
};
WWHD_OFFSET(bkExecModelView, mMatrixBlock, 0x2C);
WWHD_OFFSET(bkExecModelView, mBaseScale, 0xBC);
WWHD_SIZE(bkExecModelView, 0xC8);
struct bkExecSearchLightView {
    u8 padding[0x780];
    u8 mBkControl;
};
WWHD_OFFSET(bkExecSearchLightView, mBkControl, 0x780);
WWHD_SIZE(bkExecSearchLightView, 0x781);
struct bkExecCollisionStatusView {
    u8 padding[0xC];
    gptr<fopAc_ac_c> mp_actor;
};
WWHD_OFFSET(bkExecCollisionStatusView, mp_actor, 0xC);
WWHD_SIZE(bkExecCollisionStatusView, 0x10);
struct bkExecCollisionObjectView {
    u8 padding[0x44];
    gptr<bkExecCollisionStatusView> mStts;
};
WWHD_OFFSET(bkExecCollisionObjectView, mStts, 0x44);
WWHD_SIZE(bkExecCollisionObjectView, 0x48);
// HD: guest loads/stores preserve null-address and wrapped-address operations.
static inline cXyz* bkExec_cameraEye(u32 camera) {
    return gabi::at<cXyz>(camera + offsetof(bkExecCameraView, mEye));
}
static inline cXyz* bkExec_cameraCenter(u32 camera) {
    return gabi::at<cXyz>(camera + offsetof(bkExecCameraView, mCenter));
}
static inline cXyz* bkExec_baseScale(J3DModel* model) {
    return gabi::at<cXyz>(gabi::ea(model) + offsetof(bkExecModelView, mBaseScale));
}
static inline void bkExec_setEyeFrame(mDoExt_btpAnm* animation, f32 frame) {
    gabi::store<f32>(gabi::ea(animation) + offsetof(mDoExt_btpAnm, mFrameCtrl) +
                         offsetof(J3DFrameCtrl, mFrame),
                     frame);
}
static inline void bkExec_setBkControl(daObj_Search_Act_c* search, u8 control) {
    gabi::store<u8>(gabi::ea(search) + offsetof(bkExecSearchLightView, mBkControl), control);
}
static inline void bkExec_setFindFlag(u8 flag) { gabi::store<u8>(0x1046BE20, flag); }
static inline void bkExec_setRotAngleSpeed(fopAc_ac_c* boko, s16 speed) {
    gabi::store<s16>(gabi::ea(boko) + offsetof(bkExecBokoView, m2C2), speed);
}
static inline fopAc_ac_c* bkExec_collisionActor(void* object) {
    u32 status = gabi::load<u32>(gabi::ea(object) + offsetof(bkExecCollisionObjectView, mStts));
    return status ? gabi::at<fopAc_ac_c>(
                        gabi::load<u32>(status + offsetof(bkExecCollisionStatusView, mp_actor)))
                  : nullptr;
}

// HD: these private prefix views avoid changing the shared actor and particle layouts.
struct bkExecEmitterView {
    u8 padding[0x34];
    be<f32> mRate;
    u8 padding38[0x58 - 0x38];
    be<f32> mSpread;
    be<s32> mMaxFrame;
    u8 padding60[0x7C - 0x60];
    be<f32> mVolumeSweep;
    u8 padding80[0x238 - 0x80];
    cXyz mGlobalParticleScale;
};
WWHD_OFFSET(bkExecEmitterView, mRate, 0x34);
WWHD_OFFSET(bkExecEmitterView, mSpread, 0x58);
WWHD_OFFSET(bkExecEmitterView, mMaxFrame, 0x5C);
WWHD_OFFSET(bkExecEmitterView, mVolumeSweep, 0x7C);
WWHD_OFFSET(bkExecEmitterView, mGlobalParticleScale, 0x238);
WWHD_SIZE(bkExecEmitterView, 0x244);
struct bkExecPlayerView : fopAc_ac_c {
    u8 mCutType;
    u8 padding3AD[0x420 - 0x3AD];
    be<s16> mDemoType;
    u8 padding422[6];
    be<s32> mParam0;
    u8 padding42C[4];
    be<s32> mDemoMode;
};
WWHD_OFFSET(bkExecPlayerView, mCutType, 0x3AC);
WWHD_OFFSET(bkExecPlayerView, mDemoType, 0x420);
WWHD_OFFSET(bkExecPlayerView, mParam0, 0x428);
WWHD_OFFSET(bkExecPlayerView, mDemoMode, 0x430);
WWHD_SIZE(bkExecPlayerView, 0x434);
static inline bkExecPlayerView* bkExec_player(fopAc_ac_c* player) {
    return gabi::at<bkExecPlayerView>(gabi::ea(player));
}

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline be<u8>& another_hit() { return *gabi::at<be<u8>>(0x1046232C); }
static inline fopAc_ac_c* ken() { return gabi::at<fopAc_ac_c>(gabi::load<u32>(0x1046231C)); }
/* dComIfGp_roomControl_getStayNo: s8 at 0x1047E6C8 */
/* CPad_CHECK_TRIG_B(0) / CPad_CHECK_HOLD_Y(0) (out of line in HD) */
static inline BOOL CPad_CHECK_TRIG_B(s32 port) { return gabi::call<BOOL>(0x020078BC, port); }
static inline BOOL CPad_CHECK_HOLD_Y(s32 port) { return gabi::call<BOOL>(0x02007738, port); }
static inline BOOL enemy_ice(enemyice_l* e) { return gabi::call<BOOL>(0x020402C8, e); }
static inline void enemy_fire(enemyfire_l* e) { gabi::call(0x02041570, e); }
/* c_damagereaction: damage_reaction (the matcher calls 02041F94 dr_damage_anime) */
static inline s32 damage_reaction(damagereaction_l* dr) { return gabi::call<s32>(0x02041F94, dr); }
static inline void fopAcM_setCarryNow(fopAc_ac_c* a, BOOL b) { gabi::call(0x025D9D0C, a, b); }
static inline void fopAcM_orderPotentialEvent(fopAc_ac_c* a, u16 flag, u16 p2, u16 p3) {
    gabi::call(0x025D7B24, a, flag, p2, p3);
}
/* out-of-line TU copies */
static inline fopAc_ac_c* fopAcM_SearchByID_ool(u32 id) {
    return gabi::call<fopAc_ac_c*>(0x020A0634, id);
}
static inline void fopAcM_monsSeStart_ool(fopAc_ac_c* a, u32 id, u32 prm) {
    gabi::call(0x020A0670, a, id, prm);
}
/* mDoAud_monsSeStart(id, pos, procId, param, reverb) */
static inline void mDoAud_monsSeStart(u32 id, cXyz* pos, u32 pid, u32 prm, s32 reverb) {
    gabi::call(0x025E1AA4, id, pos, pid, prm, reverb);
}
/* fopAcM_monsSeStart, inline (HD: eyePos null check) */
static inline void fopAcM_monsSeStart(fopAc_ac_c* a, u32 id, u32 prm) {
    if (gabi::ea(&a->eyePos) != 0) {
        s8 room = fopAcM_GetRoomNo(a);
        u32 pid = fopAcM_GetID(a);
        mDoAud_monsSeStart(id, &a->eyePos, pid, prm, dComIfGp_getReverb(room));
    }
}
static inline void mDoAud_seStop(u32 id, s32 t) { gabi::call(0x025E1AE0, id, t); }
static inline void mDoAud_bgmAllMute(s32 t) { gabi::call(0x025E1960, t); }
static inline void mDoAud_subBgmStart(u32 id) { gabi::call(0x025E1918, id); }
static inline void JUTReport(s32 x, s32 y, const char* fmt, s32 v) {
    gabi::call(0x027EC9E8, x, y, fmt, v);
}
static inline void dKy_Sound_set(cXyz* pos, s32 p, u32 pid, s32 t) {
    gabi::call(0x0255F458, pos, p, pid, t);
}
static inline void def_se_set(fopAc_ac_c* a, void* obj, u32 p) {
    gabi::call(0x02518CC8, a, obj, p);
}
struct bkExecAtInfo {
    /* 0x00 */ be<u32> mpObj;
    /* 0x04 */ be<u32> mpActor;
    /* 0x08 */ be<u8> mDamage;
    /* 0x09 */ be<u8> mbDead;
    /* 0x0A */ be<u8> mResultingAttackType;
    /* 0x0B */ u8 _0B;
    /* 0x0C */ csXyz m0C;
    /* 0x12 */ be<u16> mPlCutBit;
    /* 0x14 */ be<u32> pParticlePos;
    /* 0x18 */ be<s32> mHitSoundId;
};
WWHD_OFFSET(bkExecAtInfo, mDamage, 8);
WWHD_OFFSET(bkExecAtInfo, m0C, 0xC);
WWHD_OFFSET(bkExecAtInfo, pParticlePos, 0x14);
WWHD_SIZE(bkExecAtInfo, 0x1C);
static inline void at_power_check(bkExecAtInfo* i) { gabi::call(0x02518DB0, i); }
static inline u32 cc_at_check(fopAc_ac_c* a, bkExecAtInfo* i) {
    return gabi::call<u32>(0x025192A8, a, i);
}
static inline u32 CcObj_AtType(u32 obj) { return gabi::load<u32>(obj + 0x10); }
static inline void dSmokeEcallBack_end(dPa_smokeEcallBack_l* cb) { gabi::call(0x025A5F88, cb); }
static inline void* GetCoHitObj(dCcD_GObjInf* o) { return gabi::call<void*>(0x025163BC, o); }
static inline void dCcMassS_Set(void* obj, u8 prio) {
    gabi::call(0x02516C14, gabi::at<u8>(dComIfGp_ea() + PLAY_CCMASS), obj, prio);
}
/* camera_process_class: dCamera_c at +0x248; view eye +0xDC, center +0xE8 */
static inline void dCam_Stop(u32 cam) {
    gabi::call(0x02514F2C, cam + offsetof(bkExecCameraView, mCamera));
}
static inline void dCam_Start(u32 cam) {
    gabi::call(0x02514F38, cam + offsetof(bkExecCameraView, mCamera));
}
static inline void dCam_SetTrimSize(u32 cam, s32 s) {
    gabi::call(0x02515280, cam + offsetof(bkExecCameraView, mCamera), s);
}
static inline void dCam_Reset(u32 cam, cXyz* center, cXyz* eye) {
    gabi::call(0x0251510C, cam + offsetof(bkExecCameraView, mCamera), center, eye);
}
static inline void dCam_Set(u32 cam, cXyz* center, cXyz* eye, f32 fovy, s16 bank) {
    gabi::call(0x02514F88, cam + offsetof(bkExecCameraView, mCamera), center, eye, fovy, bank);
}
/* daPy_py_c virtuals (vtable at +0xB4) and demo fields */
static inline void daPy_vcall_setPlayerPosAndAngle(fopAc_ac_c* p, cXyz* pos, s16 ang) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 0xB4);
    gabi::call_ptr(gabi::load<u32>(vt + 0x114), p, pos, ang);
}
static inline void daPy_vcall_voiceStart(fopAc_ac_c* p, u32 id) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 0xB4);
    gabi::call_ptr(gabi::load<u32>(vt + 0xE4), p, id);
}
static inline u8 daPy_getCutType(fopAc_ac_c* p) {
    return gabi::load<u8>(gabi::ea(p) + offsetof(bkExecPlayerView, mCutType));
}
/* daBoko_c: model at +0x3B4; setMatrix copies to the model's base matrix (HD: null check) */
static inline void daBoko_setMatrix(fopAc_ac_c* boko, Mtx34* m) {
    J3DModel* model =
        gabi::at<J3DModel>(gabi::load<u32>(gabi::ea(boko) + offsetof(bkExecBokoView, mpModel)));
    if (model != nullptr)
        J3DModel_setBaseTRMtx(model, m);
}
/* J3DModel::getAnmMtx (HD matrix block, marks it dirty) */
static inline Mtx34* J3DModel_getAnmMtx(J3DModel* model, s32 jnt) {
    u32 blk = gabi::load<u32>(gabi::ea(model) + offsetof(bkExecModelView, mMatrixBlock));
    gabi::store<u16>(blk + offsetof(bkExecMatrixBlockView, mFlags),
                     gabi::load<u16>(blk + offsetof(bkExecMatrixBlockView, mFlags)) | 0x10);
    return gabi::at<Mtx34>(gabi::load<u32>(blk + offsetof(bkExecMatrixBlockView, mAnmMtx)) +
                           0x30 * jnt);
}
/* The shared path layout keeps guest-address arithmetic for null path pointers. */
static inline u16 path_num(bk_class* i_this) {
    return gabi::load<u16>(gabi::ea(i_this->ppd.get()) + offsetof(path_layout::dPath, m_num));
}
static inline path_layout::dPnt* path_points(bk_class* i_this) {
    return gabi::at<path_layout::dPnt>(
        gabi::load<u32>(gabi::ea(i_this->ppd.get()) + offsetof(path_layout::dPath, m_points)));
}
/* fopAcM_prm_class */
struct bkExecCreateParams {
    /* 0x00 */ be<u32> parameters;
    /* 0x04 */ cXyz position;
    /* 0x10 */ csXyz angle;
    /* 0x16 */ u8 _16[0x21 - 0x16];
    /* 0x21 */ be<s8> room_no;
};
WWHD_OFFSET(bkExecCreateParams, parameters, 0x00);
WWHD_OFFSET(bkExecCreateParams, position, 0x04);
WWHD_OFFSET(bkExecCreateParams, angle, 0x10);
WWHD_OFFSET(bkExecCreateParams, room_no, 0x21);
WWHD_SIZE(bkExecCreateParams, 0x22);
static inline bkExecCreateParams* fopAcM_CreateAppend() {
    return gabi::call<bkExecCreateParams*>(0x025D5600);
}
static inline void fopAcM_Create(s16 name, bkExecCreateParams* p) {
    u32 layer = gabi::call<u32>(0x025DED64);
    gabi::call(0x025E14A8, layer, name, 0, 0, p);
}
/* dBgS_GndChk (stack object), this TU's vtables */
struct bkExecGroundCheck {
    /* 0x00 */ be<u32> mpPolyPassChk; /* -> +0x40 */
    /* 0x04 */ be<u32> mpGrpPassChk;  /* -> +0x4C */
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ cXyz m_pos;
    /* 0x30 */ u8 _30[0x40 - 0x30];
    /* 0x40 */ be<u32> __vtbl_40;
    /* 0x44 */ be<u8> mPass[7];
    /* 0x4B */ u8 _4B;
    /* 0x4C */ be<u32> __vtbl_4C;
    /* 0x50 */ be<u32> mGrp;
};
WWHD_OFFSET(bkExecGroundCheck, m_pos, 0x24);
WWHD_OFFSET(bkExecGroundCheck, mPass, 0x44);
WWHD_OFFSET(bkExecGroundCheck, mGrp, 0x50);
WWHD_SIZE(bkExecGroundCheck, 0x54);
static inline void dBgS_GndChk_ct(bkExecGroundCheck* c) {
    gabi::call(0x02008E0C, c);
    c->__vtbl_10 = 0x100094BC;
    for (int i = 0; i < 7; i++)
        c->mPass[i] = 0;
    c->mpPolyPassChk = gabi::ea(c) + 0x40;
    c->mpGrpPassChk = gabi::ea(c) + 0x4C;
    c->__vtbl_4C = 0x100094DC;
    c->mGrp = 1;
    c->__vtbl_40 = 0x100094EC;
    c->__vtbl_20 = 0x100094CC;
}
static inline void dBgS_GndChk_dt(bkExecGroundCheck* c) {
    c->__vtbl_20 = 0x100094CC;
    c->__vtbl_40 = 0x100094EC;
    c->__vtbl_4C = 0x100094AC;
    gabi::call(0x02008DAC, c, 0);
}
static inline f32 GroundCross(bkExecGroundCheck* c) { return cBgS_GroundCross(dComIfG_Bgsp(), c); }

/* Float moves in the recompiled code: the host compiler folds some lfs/stfs pairs into bit
 * copies (mostly when both are in one basic block and close together) and leaves others going
 * through a double (a signalling NaN is quieted). Which one applies was measured per move:
 * bitcopy_f32 models the former, be<f32> the latter. */
static inline void bitcopy_f32(be<f32>* dst, const be<f32>* src) {
    gabi::store<u32>(gabi::ea(dst), gabi::load<u32>(gabi::ea(src)));
}

static inline void setBaseTRMtx_calc(J3DModel* m) { J3DModel_setBaseTRMtx(m, calc_mtx()); }

#define REG8_S(i) REG_S(8, i)
#define REG8_F(i) REG_F(8, i)
#define REG13_F(i) REG_F(13, i)

static inline void bkExec_anm_init(bk_class* a, int b, f32 m, u8 l, f32 s, int f) {
    gabi::call(0x02098DA4, a, b, m, l, s, f);
}
static inline void bkExec_wait_set(bk_class* a) { gabi::call(0x0209B264, a); }
#endif

namespace bkExec {
enum {
    fpcNm_BK_e = 0xBD,
    JA_SE_MAJUTOU_ALERM = 0x834,
    JA_SE_CV_BK_DAMAGE_S = 0x4827,
    JA_SE_CV_BK_DAMAGE_L = 0x4828,
    JA_SE_CV_BK_FAINTED = 0x4829,
    JA_BGM_MBOSS = 0x80000019u,
    ID_AK_JN_ELEMENTKIKUZU00 = 0x2B,
    ID_AK_JN_TUBA00 = 0xE,
    CUT_TYPE_HAMMER_SIDESWING = 0x11,
    dEvtFlag_STAFF_ALL_e = 2,
};
}
// HD: the callback is compiled in the companion source; retain its guest entry point.
static inline daObj_Search_Act_c* bkExec_searchLight(bk_class* actor) {
    return static_cast<daObj_Search_Act_c*>(fpcM_Search(0x0209BA78, actor));
}
