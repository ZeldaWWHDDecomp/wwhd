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

/* dPa_rippleEcallBack (HD 0x14): vtable, emitter, ..., rate at +0x10 */
struct dPa_rippleEcallBack_l {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ gptr<JPABaseEmitter> mpEmitter;
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<f32> mRate;
};
/* dPa_smokeEcallBack (HD 0x20): vtable, emitter, ..., wind-off flag at +0x15 */
struct dPa_smokeEcallBack_l {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ gptr<JPABaseEmitter> mpEmitter;
    /* 0x08 */ u8 _08[0x15 - 0x08];
    /* 0x15 */ be<u8> mWindOff;
    /* 0x16 */ u8 _16[0x20 - 0x16];
};
/* mDoExt_btpAnm (HD 0x74): J3DFrameCtrl first */
struct mDoExt_btpAnm_l {
    /* 0x00 */ J3DFrameCtrl mFrameCtrl;
    /* 0x10 */ u8 _10[0x74 - 0x10];
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
    /* 0x848 */ mDoExt_btpAnm_l mBtp;      /* HD: GameCube m500/mpTexNoAnm/m508..m50A */
    /* 0x8BC */ be<s32> m50C;
    /* 0x8C0 */ cXyz m514;                 /* demo camera eye */
    /* 0x8CC */ cXyz m520;                 /* demo camera centre */
    /* 0x8D8 */ be<f32> m53C;              /* demo camera fovy */
    /* 0x8DC */ dPa_rippleEcallBack_l m540;
    /* 0x8F0 */ dPa_smokeEcallBack_l m554;
    /* 0x910 */ dPa_smokeEcallBack_l m574;
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

/* acch members */
inline f32 kb_wtr_height(kb_class* i_this) { return gabi::load<f32>(gabi::ea(&i_this->mAcch) + 0x174 + 0x48); } /* m_wtr.GetHeight() */
inline cBgS_PolyInfo* kb_gnd_poly(kb_class* i_this) { return gabi::at<cBgS_PolyInfo>(gabi::ea(&i_this->mAcch) + 0xD4 + 0x14); }

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
    JA_SE_CV_PG_NORMAL = 0x4820, JA_SE_CV_PG_TURN = 0x4821, JA_SE_CV_PG_CATCH = 0x4822, JA_SE_CV_PG_CARRY = 0x4823,
    JA_SE_CV_PG_L_NORMAL = 0x4915, JA_SE_CV_PG_L_TURN = 0x4916, JA_SE_CV_PG_L_CATCH = 0x4917, JA_SE_CV_PG_L_CARRY = 0x4918,
    JA_SE_OBJ_FALL_WATER_S = 0x6918,
};
enum { fopAc_Attn_LOCKON_MISC_e = 1, fopAc_Attn_LOCKON_BATTLE_e = 4, fopAc_Attn_ACTION_CARRY_e = 0x10 };
/* file statics */
static inline be<u8>& ALL_ANGER() { return *gabi::at<be<u8>>(0x101B7FA0); }
static inline be<u8>& DEMO_START() { return *gabi::at<be<u8>>(0x101B7FA1); }
/* this TU's dBgS_LinChk vtables (constructor and destructor) */
static const dBgS_LinChk_vt LINCHK_VT = {0x10012888, 0x10012898, 0x100128B8, 0x100128A8};
static inline void dBgS_LinChk_dt(void* c) {
    u32 b = gabi::ea(c);
    gabi::store<u32>(b + 0x58, 0x100128B8);
    gabi::store<u32>(b + 0x64, 0x10012808);
    gabi::store<u32>(b + 0x20, 0x100127F8);
    cBgS_LinChk_dt(c, 0);
}

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* esa_class (HD +0x11C): field_0x298 (u8) at 0x3B4, mState (s8) at 0x3BD */
static inline be<u8>& esa_field_0x298(fopAc_ac_c* esa) { return *gabi::at<be<u8>>(gabi::ea(esa) + 0x3B4); }
static inline s8 esa_mState(fopAc_ac_c* esa) { return gabi::load<s8>(gabi::ea(esa) + 0x3BD); }
/* attention_info.flags (+0x39C) */
static inline be<u32>& attn_flags(fopAc_ac_c* a) { return *gabi::at<be<u32>>(gabi::ea(a) + 0x39C); }
static inline bool fopAcM_checkCarryNow(fopAc_ac_c* a) { return (a->actor_status & 0x2000) != 0; }
/* 025E1AA4 mDoAud_monsSeStart(id, pos, procId, param, reverb) */
static inline void mDoAud_monsSeStart(u32 id, cXyz* pos, u32 pid, u32 param, s32 reverb) {
    gabi::call(0x025E1AA4, id, pos, pid, param, reverb);
}
/* fopAcM_seStart / fopAcM_monsSeStart: the HD inlines test &eyePos only */
static inline void kb_se_start(fopAc_ac_c* a, u32 id) {
    cXyz* eye = &a->eyePos;
    if (gabi::ea(eye) != 0) mDoAud_seStart(id, eye, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
static inline void kb_mons_se_start(fopAc_ac_c* a, u32 id) {
    cXyz* eye = &a->eyePos;
    if (gabi::ea(eye) != 0) {
        u32 pid = a != nullptr ? gabi::load<u32>(gabi::ea(a) + 4) : 0xFFFFFFFFu; /* fopAcM_GetID */
        mDoAud_monsSeStart(id, eye, pid, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
    }
}
static inline void kb_catch_se(kb_class* i_this) {
    if (i_this->mShapeType >= 8) {
        kb_mons_se_start(i_this, JA_SE_CV_PG_L_CATCH);
    } else {
        kb_mons_se_start(i_this, JA_SE_CV_PG_CATCH);
    }
}
/* particle emitters: JPABaseEmitter::setGlobalScale (HD: +0x220.. and +0x238..) */
template <class CB> static inline u32 emitter_ea(CB& cb) { return gabi::ea(cb.mpEmitter.get()); }
static inline void set_global_scale(u32 e, f32 x, f32 y, f32 z) {
    gabi::store<f32>(e + 0x220, x);
    gabi::store<f32>(e + 0x224, y);
    gabi::store<f32>(e + 0x228, z);
    gabi::store<f32>(e + 0x238, x);
    gabi::store<f32>(e + 0x23C, y);
    gabi::store<f32>(e + 0x240, z);
}
/* virtual remove() (vtable +0x44) of the particle callbacks */
template <class CB> static inline void vremove(CB* cb) {
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(gabi::ea(cb)) + 0x44), cb);
}
static inline void JPASetRMtxTVecfromMtx(Mtx34* m, u32 r, u32 t) { gabi::call(0x028249B0, m, r, t); }
/* J3D (HD): j3dSys.mModel 0x104B462C, J3DSys::mCurrentMtx 0x104B4868, joint block at model+0x2C */
struct J3DMtxBlock_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
struct J3DModel_l {
    /* 0x00 */ u8 _00[0x2C];
    /* 0x2C */ gptr<J3DMtxBlock_l> mpMtxBlock;
    /* 0x30 */ u8 _30[0xAC - 0x30];
    /* 0xAC */ gptr<J3DModelData> mModelData;
    /* 0xB0 */ u8 _B0[8];
    /* 0xB8 */ be<u32> mUserArea;
};
/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty) */
static inline Mtx34* getAnmMtx(J3DModel* m, s32 jnt) {
    J3DMtxBlock_l* blk = ((J3DModel_l*)m)->mpMtxBlock;
    blk->mFlags |= 0x10;
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jnt * 0x30);
}
static inline J3DModel_l* j3dSys_getModel() { return gabi::at<J3DModel_l>(gabi::load<u32>(0x104B462C)); }
static inline Mtx34* J3DSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
/* dBgS::GetRoomId / GetPolyColor */
static inline s32 dBgS_GetRoomId(dBgS* bgs, cBgS_PolyInfo* p) { return gabi::call<s32>(0x024EF130, bgs, p); }
static inline s32 dBgS_GetPolyColor(dBgS* bgs, cBgS_PolyInfo* p) { return gabi::call<s32>(0x024EEEB8, bgs, p); }
/* 0200FAAC cLib_distanceAngleS: HD returns abs((s16)(a - b)) as an int (0..0x8000); callers compare
 * the whole register (the shared binding's s16 result would turn 0x8000 negative) */
static inline s32 cLib_distanceAngleS_i(s16 a, s16 b) { return gabi::call<s32>(0x0200FAAC, a, b); }
/* 025D8AB0 fopAcM_fastCreateItem(pos, itemNo, roomNo, rot, scale, speedF, speedY, gravity, bitNo, createFunc) */
static inline fopAc_ac_c* fopAcM_fastCreateItem(cXyz* pos, s32 itemNo, s32 roomNo, csXyz* rot, cXyz* scale, f32 speedF, f32 speedY,
                                                f32 gravity, s32 bitNo, u32 createFunc) {
    return gabi::call<fopAc_ac_c*>(0x025D8AB0, pos, itemNo, roomNo, rot, scale, speedF, speedY, gravity, bitNo, createFunc);
}
static inline void dKy_tevstr_init(dKy_tevstr_c* t, s8 roomNo, u8 p) { gabi::call(0x0255FFF4, t, roomNo, p); }
static inline dSv_event_c* dComIfGs_getEvent() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
/* 027F3F94 (the matcher names it __nw): returns the model data's joint tree; joint count (u16) at +8 */
static inline u16 J3DModelData_getJointNum(J3DModelData* d) { return gabi::load<u16>(gabi::ea(gabi::call<void*>(0x027F3F94, d)) + 8); }
/* J3DModelData::getJointNodePointer(i)->setCallBack(cb) (HD: nodes of 0x1C bytes from +8; index checked against +4) */
static inline void setJointCallBack(J3DModelData* d, u16 i, u32 cb) {
    u32 data = gabi::ea(d);
    u32 n = gabi::load<u32>(data + 4);
    u32 p = gabi::load<u32>(data + 8);
    if (i < n) p += i * 0x1C;
    gabi::store<u32>(p + 8, cb);
}
/* mDoExt_btpAnm (HD): constructor 025E7820, init 025E789C */
static inline void mDoExt_btpAnm_ct(void* p) { gabi::call(0x025E7820, p); }
static inline BOOL mDoExt_btpAnm_init(void* a, J3DModelData* d, void* btp, s32 anmPlay, s32 mode, f32 rate, s16 start, s16 end,
                                      bool modify, s32 entry) {
    return gabi::call<BOOL>(0x025E789C, a, d, btp, anmPlay, mode, rate, start, end, modify, entry);
}
/* 025A9084 dPa_rippleEcallBack::dPa_rippleEcallBack, 025A5B18 dPa_smokeEcallBack::dPa_smokeEcallBack(u8) */
static inline void dPa_rippleEcallBack_ct(void* p) { gabi::call(0x025A9084, p); }
static inline void dPa_smokeEcallBack_ct(void* p, u8 a) { gabi::call(0x025A5B18, p, a); }
/* GHS array helpers: __construct_array(ptr, n, size, ctor), __destroy_arr(ptr, n, size, dtor, flags) */
static inline void __construct_array(void* p, u32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }
static inline void __destroy_arr(void* p, u32 n, u32 size, u32 dtor, u32 flags) { gabi::call(0x028F0164, p, n, size, dtor, flags); }
/* sead::SafeString equality (HD inline): both sides' virtual assureTermination (+0x14), then a
 * byte compare bounded by 0x40001 */
static inline bool SafeString_eq(SafeString* a, SafeString* b) {
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a);
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a);
    u32 s1 = a->mStringTop;
    gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b);
    if (s1 == b->mStringTop) return true;
    u32 p = a->mStringTop, q = b->mStringTop;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c = gabi::load<u8>(p + i);
        if (c != gabi::load<u8>(q + i)) return false;
        if (c == 0) return true;
    }
    return false;
}
/* strcmp(dComIfGp_getStartStageName(), lit) == 0 (HD: SafeString comparison; the start stage name at play+0x5134) */
static inline bool dComIfGp_isStartStage(u32 lit) {
    gabi::Local<SafeString> a;
    a->mStringTop = lit;
    a->__vtbl = SAFESTRING_VTBL;
    gabi::Local<SafeString> b;
    b->mStringTop = dComIfGp_ea() + 0x5134;
    b->__vtbl = SAFESTRING_VTBL;
    return SafeString_eq(a, b);
}
/* dBgS_GndChk on the stack (this TU's vtables) */
static const dBgS_GndChk_vt GNDCHK_VT = {0x10012818, 0x10012828, 0x10012848, 0x10012838};
static inline void dBgS_GndChk_dt(void* c) {
    u32 b = gabi::ea(c);
    gabi::store<u32>(b + 0x20, 0x10012828);
    gabi::store<u32>(b + 0x40, 0x10012848);
    gabi::store<u32>(b + 0x4C, 0x10012808);
    gabi::call(0x02008DAC, c, 0); /* cBgS_GndChk::~cBgS_GndChk */
}
#define KB_VTBL 0x100128C8 /* kb_class vtable (HD virtual destructor) */
static const dBgS_ObjAcch_vt OBJACCH_VT = {0x10012858, 0x10012878, 0x10012868};
#define STR_SEA 0x100129E4u /* "sea" */
#define co_sph_src gabi::at<dCcD_SrcSph>(0x101B7FFC)
enum { dIsleRoom_WindfallIsland_e = 11, dIsleRoom_OutsetIsland_e = 44 };
enum { dRes_INDEX_KB_BAS_WAIT1_e = 0xC, dRes_INDEX_KB_BCK_WAIT1_e = 0x17 };
enum { JA_SE_OBJ_LUPY_OUT = 0x69E9, JA_SE_OBJ_ITEM_OUT = 0x69ED };
/* kb_btp_idx (0x101B7FC4) / kb_bdl_idx (HD, 0x101B7FD4): u16[7] */
static inline u16 kb_btp_idx(u32 i) { return gabi::load<u16>(0x101B7FC4 + 2 * i); }
static inline u16 kb_bdl_idx(u32 i) { return gabi::load<u16>(0x101B7FD4 + 2 * i); }
/* daPy_py_c::getGrabMissActor (virtual, HD vtable at +0xB4, slot +0x44) */
static inline fopAc_ac_c* daPy_getGrabMissActor(fopAc_ac_c* player) {
    return gabi::call_ptr<fopAc_ac_c*>(gabi::load<u32>(player->__vtbl + 0x44), player);
}
enum { dRes_INDEX_KB_BCK_DAMAGE1_e = 0x10, dRes_INDEX_KB_BAS_RUN1_e = 0xB, dRes_INDEX_KB_BCK_RUN1_e = 0x16 };
/* 02515BBC dCcD_GAtTgCoCommonBase::GetAc */
static inline fopAc_ac_c* dCcD_GAtTgCoCommonBase_GetAc(void* p) { return gabi::call<fopAc_ac_c*>(0x02515BBC, p); }
static inline void kb_mons_se2(kb_class* i_this, u32 big, u32 normal) {
    if (i_this->mShapeType >= 8) {
        kb_mons_se_start(i_this, big);
    } else {
        kb_mons_se_start(i_this, normal);
    }
}
enum {
    dRes_INDEX_KB_BAS_EAT1_e = 7, dRes_INDEX_KB_BAS_NAKU1_e = 0xA, dRes_INDEX_KB_BAS_WALK1_e = 0xD,
    dRes_INDEX_KB_BCK_EAT1_e = 0x12, dRes_INDEX_KB_BCK_NAKU1_e = 0x15, dRes_INDEX_KB_BCK_WALK1_e = 0x18,
};
enum {
    JA_SE_CM_PG_DIG = 0x5921, JA_SE_CM_PG_DIG_LANDING = 0x5922, JA_SE_CM_PG_L_JUMP = 0x593E, JA_SE_CM_PG_L_DIG = 0x593F,
};
enum { PROC_KS = 0xCD };
/* daPy_py_c demo (HD: mDemo at +0x420: type u16 +0x420, param0 +0x428, mode +0x430) */
enum { daPy_demo_DEMO_N_WAIT_e = 1, daPy_demo_DEMO_UNK_018_e = 0x12, daPy_demo_DEMO_SMILE_e = 0x32 };
static inline void daPy_changeOriginalDemo(fopAc_ac_c* pl) {
    gabi::store<u32>(gabi::ea(pl) + 0x428, 0);
    gabi::store<u16>(gabi::ea(pl) + 0x420, 3);
}
static inline void daPy_cancelOriginalDemo(fopAc_ac_c* pl) {
    gabi::store<u16>(gabi::ea(pl) + 0x420, 2);
    gabi::store<u32>(gabi::ea(pl) + 0x430, 1);
}
static inline void daPy_changeDemoMode(fopAc_ac_c* pl, u32 mode) { gabi::store<u32>(gabi::ea(pl) + 0x430, mode); }
/* camera_process_class* dComIfGp_getCamera(dComIfGp_getPlayerCameraID(0)): play+0x5AF8 + id * 0x34, id (s8) at play+0x5B30;
 * its dCamera_c at +0x248 */
static inline u8* kb_getPlayerCamera() {
    s8 id = gabi::load<s8>(dComIfGp_ea() + 0x5B30);
    return gabi::at<u8>(gabi::load<u32>(dComIfGp_ea() + 0x5AF8 + id * 0x34));
}
static inline dCamera_c* camera_body(u8* cam) { return gabi::at<dCamera_c>(gabi::ea(cam) + 0x248); }
static inline void dCamera_Stop(dCamera_c* c) { gabi::call(0x02514F2C, c); }
static inline void dCamera_Start(dCamera_c* c) { gabi::call(0x02514F38, c); }
static inline void dCamera_SetTrimSize(dCamera_c* c, s32 s) { gabi::call(0x02515280, c, s); }
/* Set(cXyz center, cXyz eye, f32 fovy, s16 bank) / Reset(cXyz center, cXyz eye): by value (pointers to copies) */
static inline void dCamera_Set(dCamera_c* c, cXyz* center, cXyz* eye, f32 fovy, s16 bank) { gabi::call(0x02514F88, c, center, eye, fovy, bank); }
static inline void dCamera_Reset(dCamera_c* c, cXyz* center, cXyz* eye) { gabi::call(0x0251510C, c, center, eye); }
static inline BOOL fopAcM_orderPotentialEvent(fopAc_ac_c* a, u16 type, u16 flag, u16 p) { return gabi::call<BOOL>(0x025D7B24, a, type, flag, p); }
static inline s32 dBgS_GetSpecialCode(dBgS* bgs, cBgS_PolyInfo* p) { return gabi::call<s32>(0x024EF09C, bgs, p); }
static inline s32 dBgS_GetAttributeCode(dBgS* bgs, cBgS_PolyInfo* p) { return gabi::call<s32>(0x024EF0F4, bgs, p); }
/* 02526560 daTagKbItem_c::kb_dig(fopAc_ac_c*) (unnamed by the matcher) */
static inline void daTagKbItem_kb_dig(void* item, fopAc_ac_c* pig) { gabi::call(0x02526560, item, pig); }
/* fopAcM_seStart / monsSeStart variants that also test the actor */
static inline void kb_se_start_a(fopAc_ac_c* a, u32 id) {
    if (a != nullptr) kb_se_start(a, id);
}
static inline void kb_mons_se_start_a(fopAc_ac_c* a, u32 id) {
    if (a != nullptr) kb_mons_se_start(a, id);
}
enum { dRes_INDEX_KB_BAS_JITA1_e = 8, dRes_INDEX_KB_BCK_DASSUI_e = 0x11, dRes_INDEX_KB_BCK_JITA1_e = 0x13 };
enum { daPyStts0_CRAWL_e = 0x08000000 };
/* 025192A8 cc_at_check(fopAc_ac_c*, CcAtInfo*) (SHARED-CANDIDATE, as in d_a_pt/d_a_ph) */
struct CcAtInfo_l {
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
static inline void cc_at_check(fopAc_ac_c* a, CcAtInfo_l* info) { gabi::call(0x025192A8, a, info); }
/* dComIfGs_getSelectEquip(0): save info (0x101F84DC) +0x2E */
static inline u8 dComIfGs_getSelectEquip0() { return gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x2E); }
static inline void cBgS_GetTriPnt(dBgS* bgs, void* poly, cXyz* a, cXyz* b, cXyz* c) { gabi::call(0x02008570, bgs, poly, a, b, c); }
/* dComIfGp_getCamera(0)->mCamera.ForceLockOff(id) */
static inline void dCamera_ForceLockOff(fopAc_ac_c* a) {
    u32 cam = gabi::load<u32>(dComIfGp_ea() + 0x5AF8);
    gabi::call(0x025052BC, cam + 0x248, fopAcM_GetID(a));
}
static inline void dCcMassS_Mng_Set(void* obj, u8 p) { gabi::call(0x02516C14, gabi::at<u8>(dComIfGp_ea() + PLAY_CCMASS), obj, p); }
