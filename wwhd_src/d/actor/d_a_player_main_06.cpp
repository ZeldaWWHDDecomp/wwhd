/**
 * d_a_player_main_06.cpp (WWHD)
 * Player - Link (daPy_lk_c), address range #06 (02423D10..0242DAC7): the tail of the demo procs
 * (d_a_player_dproc.inc), ladder, hang, climb, wall hide, crawl and the first grab functions.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_player_main.cpp and its .inc files) to the WWHD layout and code,
 * verified against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * Functions of other ranges are called by address (gabi::call), not as C++ members.
 */
#include "d/actor/d_a_player_main.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* play + 0x52B1: dComIfGp_event_getPreItemNo() */
static inline u8 dComIfGp_event_getPreItemNo_l() { return gabi::load<u8>(dComIfGp_ea() + 0x52B1); }

/* JAIZelBasic::seStart (mDoAud_seStart without NULL checks) */
static inline void mDoAud_seStart_l(u32 id, cXyz* pos, u32 mtrl, s32 reverb) { gabi::call(0x025E1A40, id, pos, mtrl, reverb); }
/* fopAcM_seStartCurrent(actor, id, mtrl): pos NULL check, room reverb */
static inline void fopAcM_seStartCurrent_l(fopAc_ac_c* a, u32 id, u32 mtrl) {
    if (gabi::ea(a) + 0x314 != 0) { /* &current.pos != NULL */
        s32 rev = dComIfGp_getReverb(a->current.roomNo);
        mDoAud_seStart_l(id, &a->current.pos, mtrl, rev);
    }
}
/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty; block at +0x2C, flags +4, matrices +0x10) */
static inline Mtx34* lk_getAnmMtx(J3DModel* m, s32 jnt) {
    u32 blk = gabi::load<u32>(gabi::ea(m) + 0x2C);
    gabi::store<u16>(blk + 4, gabi::load<u16>(blk + 4) | 0x10);
    return gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jnt * 0x30);
}
/* J3DAnmBase::getFrameMax (HD: virtual, vtable at +4, slot 0x14) */
static inline s32 anm_getFrameMax(u32 anm) { return gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(anm + 4) + 0x14), anm); }
/* play + 0x5CD8: dComIfGp_setPlayerStatus0(0, flag) */
static inline void dComIfGp_onPlayerStatus0_l(u32 flag) {
    u32 a = dComIfGp_ea() + 0x5CD8;
    gabi::store<u32>(a, gabi::load<u32>(a) | flag);
}
/* play + 0x5CD8: dComIfGp_checkPlayerStatus0(0, flag) */
static inline u32 dComIfGp_checkPlayerStatus0_l(u32 flag) { return gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & flag; }

/* dComIfGs: the save data object *(0x101F84DC) */
static inline u32 dComIfGs_base_l() { return gabi::load<u32>(0x101F84DC); }
/* camera of the camera info index: play + 0x5AF8 + idx * 0x34; its Y angle at +0x236 (fopCamM_GetAngleY) */
static inline s16 dComIfGp_getCameraAngleY_l(s32 idx) {
    u32 cam = gabi::load<u32>(dComIfGp_ea() + idx * 0x34 + 0x5AF8);
    return gabi::load<s16>(cam + 0x236);
}

/* daPy_lk_c members in the opaque blocks of the shared header (offsets measured in this range) */
#define LK_FIELD(T, off) (*gabi::at<be<T>>(gabi::ea(this) + (off)))
#define mpBottleContentsModel LK_FIELD(u32, 0x4970) /* GameCube J3DModel* in 0x974..0x2FAC */
#define mBodyAngleX LK_FIELD(s16, 0x3D0) /* daPy_py_c mBodyAngle.x (GameCube 0x2B4) */
#define mLinkLinChkPoly (gabi::ea(this) + 0x9E4) /* mLinkLinChk's cBgS_PolyInfo */
#define mAcchGndPoly (gabi::ea(this) + 0x8F4) /* mAcch.m_gnd (cBgS_PolyInfo, dBgS_Acch + 0xE8) */
#define mGndChkPos (*gabi::at<cXyz>(gabi::ea(this) + 0xB38)) /* mGndChk's position (dBgS_GndChk + 0x24) */
#define mGndChkPoly (gabi::ea(this) + 0xB28) /* mGndChk's cBgS_PolyInfo */
#define mLinkLinChkCross (*gabi::at<cXyz>(gabi::ea(this) + 0xA00)) /* mLinkLinChk.GetCross() (+0x30) */
#define mFootData1_18y LK_FIELD(f32, 0x7524) /* mFootData[1].field_0x018.y */
#define mRightHandPos (*gabi::at<cXyz>(gabi::ea(this) + 0x3FC)) /* daPy_py_c (GameCube 0x2E0) */
#define mRoofChkPos (*gabi::at<cXyz>(gabi::ea(this) + 0xBA0)) /* mRoofChk's position (+0x38) */
#define mMaxNormalSpeed LK_FIELD(f32, 0x3C4) /* daPy_py_c (GameCube 0x2A8) */
#define mHDThrowActor LK_FIELD(u32, 0x8268)   /* HD-only (tail): the thrown actor followed by the camera */
#define mHDThrowAngle LK_FIELD(s16, 0x826C)   /* HD-only (tail) */
#define mHDThrowHeight LK_FIELD(f32, 0x8274)  /* HD-only (tail) */
#define mLeftHandPos (*gabi::at<cXyz>(gabi::ea(this) + 0x3F0)) /* daPy_py_c (GameCube 0x2D4) */

/* 0200ECD4 cLib_addCalc(f32* value, f32 target, f32 scale, f32 maxStep, f32 minStep) */
static inline f32 cLib_addCalc_l(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200ECD4, v, target, scale, maxStep, minStep);
}
/* dBgS (play + 0x12A0) checks */
static inline BOOL dBgS_ChkPolySafe_l(void* poly) { return gabi::call<BOOL>(0x02008254, dComIfG_Bgsp(), poly); } /* cBgS::ChkPolySafe */
static inline BOOL dBgS_ChkMoveBG_l(void* poly) { return gabi::call<BOOL>(0x024EEABC, dComIfG_Bgsp(), poly); }
static inline void dBgS_MoveBgTransPos_l(void* poly, bool b, cXyz* pos, csXyz* angle, csXyz* shape) {
    gabi::call(0x024EFA38, dComIfG_Bgsp(), poly, b, pos, angle, shape);
}
static inline s32 dBgS_GetWallCode_l(void* poly) { return gabi::call<s32>(0x024EF080, dComIfG_Bgsp(), poly); }
/* cBgS_PolyInfo assignment (poly/bg index, actor id; not the vtable at +0xC) */
static inline void polyInfo_copy_l(u32 dst, u32 src) {
    gabi::store<u16>(dst + 0, gabi::load<u16>(src + 0));
    gabi::store<u32>(dst + 8, gabi::load<u32>(src + 8));
    gabi::store<u16>(dst + 2, gabi::load<u16>(src + 2));
    gabi::store<u32>(dst + 4, gabi::load<u32>(src + 4));
}
/* cBgS::GroundCross: f1 as the callee returns it (the callers compare and store it) */
static inline f64 dBgS_GroundCross_l(void* chk) { return gabi::call<f64>(0x02008974, dComIfG_Bgsp(), chk); }
static inline s32 dBgS_GetAttributeCode_l(void* poly) { return gabi::call<s32>(0x024EF0F4, dComIfG_Bgsp(), poly); }
/* cBgS::GetTriPla(bg, poly) of a cBgS_PolyInfo; the cM3dGPla normal at +0 */
static inline u32 dBgS_GetTriPla_l(u32 poly) {
    return gabi::call<u32>(0x020084C8, dComIfG_Bgsp(), (u32)gabi::load<u16>(poly + 2), (u32)gabi::load<u16>(poly + 0));
}
static inline void PSVECSubtract_l(const cXyz* a, const cXyz* b, cXyz* out) { gabi::call(0x028E8DAC, a, b, out); }
static inline f64 dBgS_RoofChk_l(void* chk) { return gabi::call<f64>(0x024EF6E8, dComIfG_Bgsp(), chk); }
static inline void PSVECAdd_l(const cXyz* a, const cXyz* b, cXyz* out) { gabi::call(0x028E8D88, a, b, out); }
/* play + 0x5CDC: dComIfGp_setPlayerStatus1(0, flag) */
static inline void dComIfGp_onPlayerStatus1_l(u32 flag) {
    u32 a = dComIfGp_ea() + 0x5CDC;
    gabi::store<u32>(a, gabi::load<u32>(a) | flag);
}
static inline void cXyz_pl_l(const cXyz* a, cXyz* res, const cXyz* b) { gabi::call(0x0201AD78, a, res, b); }
static inline void cXyz_ml_l(const cXyz* a, cXyz* res, f32 s) { gabi::call(0x0201AE48, a, res, s); }
/* camera attention status of a camera info index: play + 0x5B00 + idx * 0x34 */
static inline u32 dComIfGp_getCameraAttentionStatus_l(s32 idx) { return gabi::load<u32>(dComIfGp_ea() + idx * 0x34 + 0x5B00); }
/* play + 0x5BB5: dComIfGp_setRStatus */
static inline void dComIfGp_setRStatus_l(u8 s) { gabi::store<u8>(dComIfGp_ea() + 0x5BB5, s); }
/* play + 0x5BB7: dComIfGp_setDoStatus */
static inline void dComIfGp_setDoStatus_l(u8 s) { gabi::store<u8>(dComIfGp_ea() + 0x5BB7, s); }

/* the model's base matrix (J3DModel + 0xC8, NULL-preserving) times a static offset vector */
#define LK_baseMultVec(fn, off, out) do { \
        u32 m_ = gabi::ea(mpCLModel.get()); \
        fn(gabi::at<Mtx34>(m_ ? m_ + 0xC8 : 0), gabi::at<cXyz>(off), out); \
    } while (0)

/* ---- daPy_lk_c functions of other ranges (by address) ---- */
enum : u32 {
    LK_setShapeAngleToTalkActor = 0x0241FB78,
    LK_dProcPresent_init_sub = 0x02423BCC,
    LK_setSingleMoveAnime = 0x023E0A04,
    LK_setBottleModel = 0x023DF0FC,
    LK_commonProcInit = 0x023DFDD8,
    LK_checkShipRideUseItem = 0x023E26EC,
    LK_initShipRideUseItem = 0x023E2E18,
    LK_setShipRidePos = 0x023E287C,
    LK_keepItemData = 0x023EEB10,
    LK_freeGrabItem = 0x023DCF8C,
    LK_actorKeep_setData = 0x023DE638,
    LK_mtxFollow_makeEmitter = 0x023D457C,
    LK_setTextureAnime = 0x023DD768,
    LK_deleteEquipItem = 0x023DC7AC,
    LK_setAnimeEquipItem = 0x023EF750,
    LK_setShipRidePosUseItem = 0x023E2DC4,
    LK_setBlendMoveAnime = 0x023E14A0,
    LK_setOldRootQuaternion = 0x023E278C,
    LK_setLetterModel = 0x0241FA98,
    LK_loadTextureScrollResource = 0x023DC4DC, /* unnamed by the matcher */
    LK_setTextureScrollResource = 0x023DC43C,
    LK_setTactModel = 0x023DEF04,
    LK_checkEndMessage = 0x0241FBCC,
    LK_checkNextMode = 0x023F14E0,
    LK_setSwordModel = 0x023DE0B4,
    LK_getItemAnimeResource = 0x023DDEEC, /* unnamed by the matcher */
    LK_setBlurPosResource = 0x023E55A4,
    LK_resetFootEffect = 0x023DF9C0,
    LK_endDemoMode = 0x023F2048,
    LK_resetActAnimeUpper = 0x023DC6A4,
    LK_procWait_init = 0x023E2FF4,
    LK_procFall_init = 0x023F6564,
    LK_procLadderUpEnd_init = 0x02418178,
    LK_procLadderMove_init = 0x02418230,
    LK_procLadderUpStart_init_sub = 0x024186A4,
    LK_procLadderDownStart_init_sub = 0x024188F0,
    LK_getLadderMoveAnmSpeed = 0x02417FF4,
    LK_procLand_init = 0x0241C714,
    LK_procClimbMoveUpDown_init = 0x02418008,
    LK_setMoveAnime = 0x023E0E50,
    LK_changeSlideProc = 0x023E4CD0,
    LK_setNormalSpeedF = 0x02416230,
    LK_getWHideModePolygon = 0x023E33AC,
    LK_procWHideReady_init = 0x023ED788,
    LK_getCrawlMoveVec = 0x02415FB0,
    LK_checkSubjectEnd = 0x02415EF0,
    LK_setBodyAngleToCamera = 0x02416E90,
    LK_procCrawlMove_init = 0x02412F68,
    LK_getSlidePolygon = 0x023E4A58,
    LK_getCrawlMoveAnmSpeed = 0x02412F2C,
    LK_initSeAnime = 0x023E0660,
    LK_HD_023F42EC = 0x023F42EC, /* unnamed by the matcher (the HD letter procs end with it) */
};
/* a float copy through an FPR that the recompiled original keeps bit-exact (no SNaN quieting) */
static inline void fcopy_l(be<f32>* dst, be<f32>* src) { gmem_stf32(gabi::ea(dst), gmem_ld32(gabi::ea(src))); }
static inline void fcpy_l(u32 dst, u32 src) { gmem_stf32(dst, gmem_ld32(src)); }
/* fsel: a >= 0 ? b : c (NaN -> c) */
static inline f32 fsel_l(f32 a, f32 b, f32 c) { return a >= 0.0f ? b : c; }
/* virtual daPy_lk_c::voiceStart(u32) (vtable slot 0xE4) */
#define LK_voiceStart(n) gabi::call_ptr(gabi::load<u32>(__vtbl + 0xE4), this, (u32)(n))
#define LK_doTrigger() (mItemTrigger & 1)
#define LK_checkNoUpperAnime() (gabi::load<u16>(gabi::ea(this) + 0x5888) == 0xFFFF) /* m_anm_heap_upper[UPPER_MOVE2_e].mIdx */
#define LK_spActionButton() (mItemButton & 0x40)
#define LK_doButton() (mItemButton & 1)
#define LK_swordTrigger() (mItemTrigger & 2)
#define LK_cutEnd() dComIfGp_evmng_cutEnd(mStaffIdx)

/* 02423D10 */
BOOL daPy_lk_c::dProcPresent() {
    WWHD_FUNC(0x02423D10, BOOL, this);
    gabi::call(LK_setShapeAngleToTalkActor, this);
    if (mProcVar6 != 0) {
        gabi::Local<cXyz> d;
        cXyz_mi(&m370C, d, &current.pos);
        if (cLib_distanceAngleS(cM_atan2s(d->x, d->z), current.angle.y) > 0x4000 || (mAcch.m_flags & dBgS_Acch::WALL_HIT)) {
            current.pos.x = m370C.x;
            current.pos.z = m370C.z;
            mProcVar6 = 0;
            gabi::call(LK_dProcPresent_init_sub, this);
            mNormalSpeed = 0.0f;
        }
    } else {
        current.angle.y = shape_angle.y;
        if (mProcVar7 != 0) {
            dComIfGp_evmng_cutEnd(mStaffIdx);
        } else if (mFrameCtrlUnder[0].getRate() < 0.01f) {
            dComIfGp_evmng_cutEnd(mStaffIdx);
            mProcVar7 = 1;
            gabi::call(LK_setSingleMoveAnime, this, 0xD9 /* ANM_PRESENTATIONAWAIT */, 1.0f, 0.0f, -1, 3.0f);
            mModeFlg = (mModeFlg | 0x180) & ~0x400u;
        } else if (mFrameCtrlUnder[0].checkPass(52.0f /* HD: m_HIO->mTurn.m.field_0xE - 10.0f folded */)) {
            if (checkBottleItem(dComIfGp_event_getPreItemNo_l())) {
                if (mEquipItem == 0x100 /* daPyItem_NONE_e */) {
                    gabi::call(LK_setBottleModel, this, (u32)dComIfGp_event_getPreItemNo_l());
                }
            } else {
                fopAc_ac_c* item = gabi::call<fopAc_ac_c*>(0x025D7C98 /* fopAcM_getItemEventPartner */, this);
                if (item != nullptr && (fpcM_GetName(item) == 0xFF /* ITEM */ || fpcM_GetName(item) == 0x101 /* Demo_Item */)) {
                    gabi::call(0x021842C8 /* daItemBase_c::show */, item);
                }
            }
        }
    }
    return TRUE;
}
VERIFY(0x02423D10, &daPy_lk_c::dProcPresent);

/* 02423F08 */
BOOL daPy_lk_c::dProcWindChange_init() {
    WWHD_FUNC(0x02423F08, BOOL, this);
    int use = gabi::call<int>(LK_checkShipRideUseItem, this, 0);
    if (mCurProc == 0xC4 /* daPyProc_DEMO_WIND_CHANGE_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0xC4);
    mNormalSpeed = 0.0f;
    int anm = 0xB4; /* ANM_WINDR */
    if (gabi::call<s32>(0x0257E560 /* dKyw_get_tactwind_dir */)) {
        anm = 0xB3; /* ANM_WINDL */
    }
    gabi::call(LK_setSingleMoveAnime, this, anm, 1.0f, 0.0f, -1, 5.0f);
    gabi::call(LK_initShipRideUseItem, this, use, 0);
    return TRUE;
}
VERIFY(0x02423F08, &daPy_lk_c::dProcWindChange_init);

/* 02424000 */
BOOL daPy_lk_c::dProcWindChange() {
    WWHD_FUNC(0x02424000, BOOL, this);
    if (dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
        gabi::call(LK_setShipRidePos, this, 0);
    }
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        LK_cutEnd();
    }
    return TRUE;
}
VERIFY(0x02424000, &daPy_lk_c::dProcWindChange);

/* 02424070 */
BOOL daPy_lk_c::dProcStandItemPut_init() {
    WWHD_FUNC(0x02424070, BOOL, this);
    if (mCurProc == 0xC5 /* daPyProc_DEMO_STAND_ITEM_PUT_e */) {
        return TRUE;
    }
    gabi::call(LK_commonProcInit, this, 0xC5);
    mNormalSpeed = 0.0f;
    gabi::call(LK_keepItemData, this);
    mProcVar6 = 0;
    fopAc_ac_c* partner = gabi::call<fopAc_ac_c*>(0x025D7CC4 /* fopAcM_getEventPartner */, this);
    csXyz* angle;
    if (partner != nullptr) {
        gabi::Local<cXyz> d;
        cXyz_mi(&partner->current.pos, d, &current.pos);
        shape_angle.y = fopAcM_searchActorAngleY(this, partner);
        gabi::Local<cXyz> xz; /* absXZ() */
        xz->x = d->x;
        xz->y = 0.0f;
        xz->z = d->z;
        f32 dist = std_sqrtf(PSVECSquareMag(xz));
        mProcVar6 = 1;
        m35C8 = dist - 47.0f;
        angle = &partner->shape_angle;
    } else {
        m35C8 = 15.0f;
        mProcVar6 = 1;
        angle = &shape_angle;
    }
    gabi::call(LK_setSingleMoveAnime, this, 0x68 /* ANM_GRABWAIT */, 1.0f /* HD: HIO folded */, 0.0f, -1, -1.0f);
    fopAc_ac_c* item = gabi::call<fopAc_ac_c*>(0x025D5928 /* fopAcM_fastCreate */, 0x1CE /* STANDITEM */,
                                               (u32)dComIfGp_event_getPreItemNo_l(), &current.pos, -1, angle,
                                               nullptr, -1, 0, 0);
    if (item != nullptr) {
        gabi::call(LK_actorKeep_setData, &mActorKeepGrab, item);
        gabi::call(0x025D9D0C /* fopAcM_setCarryNow */, item, 0);
    }
    return TRUE;
}
VERIFY(0x02424070, &daPy_lk_c::dProcStandItemPut_init);

/* 02424240 */
BOOL daPy_lk_c::dProcStandItemPut() {
    WWHD_FUNC(0x02424240, BOOL, this);
    if (mProcVar6 == 1) {
        if (gabi::load<f32>(m_old_fdata + 0xC) < 0.01f) { /* m_old_fdata->getOldFrameRate() */
            gabi::call(LK_setSingleMoveAnime, this, 0x66 /* ANM_GRABUP */, -1.1f, 0.0f, 7, 0.0f); /* HD: HIO folded */
            mProcVar6 = 2;
            LK_voiceStart(7);
        }
    } else if (mFrameCtrlUnder[0].getRate() > -0.01f) {
        fopAc_ac_c* grab = mActorKeepGrab.mActor;
        if (grab != nullptr) {
            fopAcM_seStartCurrent_l(grab, 0x2870 /* JA_SE_LK_W_DAIZA_ATTACH */, 0);
        }
        gabi::call(LK_freeGrabItem, this);
        LK_cutEnd();
    }
    return TRUE;
}
VERIFY(0x02424240, &daPy_lk_c::dProcStandItemPut);

/* 02424348 */
BOOL daPy_lk_c::dProcVorcanoFail_init() {
    WWHD_FUNC(0x02424348, BOOL, this);
    if (mCurProc == 0xC6 /* daPyProc_DEMO_VORCANO_FAIL_e */) {
        return TRUE;
    }
    gabi::call(LK_commonProcInit, this, 0xC6);
    mNormalSpeed = 0.0f;
    speed.y = 18.0f;
    gravity = 0.0f;
    gabi::call(LK_setSingleMoveAnime, this, 0x61 /* ANM_LAVADAM */, 1.0f, 0.0f, -1, 5.0f);
    Mtx34* mtx = lk_getAnmMtx(mpCLModel, 0 /* CL_JNT_LINK_ROOT_e */);
    gabi::call(LK_mtxFollow_makeEmitter, &m32E4, 0x8078 /* ID_AK_SN_HIDARUMAFIRE */, mtx, &current.pos, nullptr);
    return TRUE;
}
VERIFY(0x02424348, &daPy_lk_c::dProcVorcanoFail_init);

/* 024243F8 */
BOOL daPy_lk_c::dProcVorcanoFail() {
    WWHD_FUNC(0x024243F8, BOOL, this);
    LK_cutEnd();
    return TRUE;
}
VERIFY(0x024243F8, &daPy_lk_c::dProcVorcanoFail);

/* 02424434 */
BOOL daPy_lk_c::dProcSlightSurprised_init() {
    WWHD_FUNC(0x02424434, BOOL, this);
    if (mCurProc == 0xC7 /* daPyProc_DEMO_SLIGHT_SURPRISED_e */) {
        return FALSE;
    }
    int use = gabi::call<int>(LK_checkShipRideUseItem, this, 1);
    gabi::call(LK_commonProcInit, this, 0xC7);
    gabi::call(LK_setSingleMoveAnime, this, 0xBF /* ANM_SURPRISEDB */, 1.0f, 0.0f, -1, 5.0f);
    s32 p0 = demoParam0();
    mNormalSpeed = 0.0f;
    mProcVar6 = 0;
    if (p0 == 1) {
        LK_voiceStart(28);
    } else if (p0 == 2) {
        LK_voiceStart(45);
    } else if (p0 == 3) {
        LK_voiceStart(49);
    }
    gabi::call(LK_initShipRideUseItem, this, use, 0);
    return TRUE;
}
VERIFY(0x02424434, &daPy_lk_c::dProcSlightSurprised_init);

/* 024245AC */
BOOL daPy_lk_c::dProcSlightSurprised() {
    WWHD_FUNC(0x024245AC, BOOL, this);
    if (dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
        gabi::call(LK_setShipRidePos, this, 0);
    }
    if (mProcVar6 != 0 || mFrameCtrlUnder[0].getRate() < 0.01f) {
        LK_cutEnd();
        if (mProcVar6 == 0) {
            mProcVar6 = 1;
            gabi::call(LK_setSingleMoveAnime, this, 0xD8 /* ANM_SURPRISEDWAIT */, 1.0f, 0.0f, -1, 3.0f);
            if (anm_getFrameMax(mpAnmTexPatternData) == 0) {
                m3530 = 0;
            } else {
                m3530 = anm_getFrameMax(mpAnmTexPatternData) - 1;
            }
            if (anm_getFrameMax(mpTexScrollResData) == 0) {
                m3532 = 0;
            } else {
                m3532 = anm_getFrameMax(mpTexScrollResData) - 1;
            }
            mModeFlg = mModeFlg & ~0x400u;
        }
    }
    return TRUE;
}
VERIFY(0x024245AC, &daPy_lk_c::dProcSlightSurprised);

/* 02424724 */
BOOL daPy_lk_c::dProcSmile_init() {
    WWHD_FUNC(0x02424724, BOOL, this);
    if (mCurProc == 0xC8 /* daPyProc_DEMO_SMILE_e */) {
        return FALSE;
    }
    int use = gabi::call<int>(LK_checkShipRideUseItem, this, 1);
    gabi::call(LK_commonProcInit, this, 0xC8);
    gabi::call(LK_setSingleMoveAnime, this, 0 /* ANM_WAITS */, 1.0f, 0.0f, -1, 5.0f);
    gabi::call(LK_setTextureAnime, this, 143, 0);
    mNormalSpeed = 0.0f;
    mProcVar6 = 0;
    gabi::call(LK_initShipRideUseItem, this, use, 1);
    return TRUE;
}
VERIFY(0x02424724, &daPy_lk_c::dProcSmile_init);

/* 02424818 */
BOOL daPy_lk_c::dProcSmile() {
    WWHD_FUNC(0x02424818, BOOL, this);
    if (dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
        gabi::call(LK_setShipRidePos, this, 1);
    }
    m3530 = m3530 + 1;
    if ((s32)(u16)m3530 >= anm_getFrameMax(mpAnmTexPatternData)) {
        if (mProcVar6 == 0) {
            mProcVar6 = 1;
        } else {
            LK_cutEnd();
        }
    }
    return TRUE;
}
VERIFY(0x02424818, &daPy_lk_c::dProcSmile);

/* 024248C0 */
BOOL daPy_lk_c::dProcBossWarp_init() {
    WWHD_FUNC(0x024248C0, BOOL, this);
    if (mCurProc == 0xC9 /* daPyProc_DEMO_BOSS_WARP_e */) {
        return TRUE;
    }
    gabi::call(LK_commonProcInit, this, 0xC9);
    if (demoParam0() == 0) {
        gabi::call(LK_setSingleMoveAnime, this, 0xDF /* ANM_WARPOUTFIRST */, 1.0f, 0.0f, -1, 3.0f);
        mProcVar6 = 1;
        gabi::call(LK_deleteEquipItem, this, 0);
    } else {
        gabi::call(LK_setSingleMoveAnime, this, 0xD6 /* ANM_WARPIN */, 1.0f, 0.0f, -1, -1.0f);
        mProcVar6 = 0;
    }
    mNormalSpeed = 0.0f;
    if (demoParam0() == 2) {
        gabi::call(LK_initShipRideUseItem, this, 2, 1);
        m353E = m353C = 0;
    }
    if (mEquipItem == 0x101 /* daPyItem_BOKO_e */) {
        gabi::call(LK_deleteEquipItem, this, 0);
    }
    return TRUE;
}
VERIFY(0x024248C0, &daPy_lk_c::dProcBossWarp_init);

/* 024249DC */
BOOL daPy_lk_c::dProcBossWarp() {
    WWHD_FUNC(0x024249DC, BOOL, this);
    if (dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
        gabi::call(LK_setShipRidePos, this, 1);
        m353E = m353C = 0;
    }
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        if (mProcVar6 == 0) {
            LK_cutEnd();
        } else {
            mProcVar6 = 0;
            gabi::call(LK_setSingleMoveAnime, this, 0xD7 /* ANM_WARPOUT */, 1.0f, 0.0f, -1, 10.0f);
        }
    }
    return TRUE;
}
VERIFY(0x024249DC, &daPy_lk_c::dProcBossWarp);

/* 02424AA0 */
BOOL daPy_lk_c::dProcAgbUse_init() {
    WWHD_FUNC(0x02424AA0, BOOL, this);
    if (mCurProc == 0xCA /* daPyProc_DEMO_AGB_USE_e */) {
        return TRUE;
    }
    int use = gabi::call<int>(LK_checkShipRideUseItem, this, 1);
    mProcVar6 = 0;
    gabi::call(LK_commonProcInit, this, 0xCA);
    gabi::call(LK_keepItemData, this);
    gabi::call(LK_setAnimeEquipItem, this);
    mNormalSpeed = 0.0f;
    gabi::call(LK_setSingleMoveAnime, this, 0xC1 /* ANM_USETCEIVER */, 1.0f, 0.0f, -1, 2.4f /* HD: HIO folded */);
    gabi::call(0x025E1988 /* seStartSystem */, 0x80A /* JA_SE_TALK_START */);
    gabi::call(LK_initShipRideUseItem, this, use, 2);
    return TRUE;
}
VERIFY(0x02424AA0, &daPy_lk_c::dProcAgbUse_init);

/* 02424B58 */
BOOL daPy_lk_c::dProcAgbUse() {
    WWHD_FUNC(0x02424B58, BOOL, this);
    gabi::call(LK_setShipRidePosUseItem, this);
    LK_cutEnd();
    return TRUE;
}
VERIFY(0x02424B58, &daPy_lk_c::dProcAgbUse);

/* 02424B9C */
BOOL daPy_lk_c::dProcLookTurn_init() {
    WWHD_FUNC(0x02424B9C, BOOL, this);
    if (mCurProc == 0xCB /* daPyProc_DEMO_LOOK_TURN_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0xCB);
    mNormalSpeed = 0.0f;
    int anm = demoParam0() ? 0xB3 /* ANM_WINDL */ : 0xB4 /* ANM_WINDR */;
    gabi::call(LK_setSingleMoveAnime, this, anm, 1.0f, 0.0f, -1, 5.0f);
    mProcVar6 = 0;
    return TRUE;
}
VERIFY(0x02424B9C, &daPy_lk_c::dProcLookTurn_init);

/* 02424C3C */
BOOL daPy_lk_c::dProcLookTurn() {
    WWHD_FUNC(0x02424C3C, BOOL, this);
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        gabi::call(LK_setBlendMoveAnime, this, 5.0f);
        shape_angle.y = (s16)(shape_angle.y - 0x8000);
        current.angle.y = shape_angle.y;
        gabi::call(LK_setOldRootQuaternion, this, 0, -0x8000, 0);
        mProcVar6 = 1;
    }
    if (mProcVar6 != 0 && gabi::load<f32>(m_old_fdata + 0xC) < 0.01f) {
        LK_cutEnd();
    }
    return TRUE;
}
VERIFY(0x02424C3C, &daPy_lk_c::dProcLookTurn);

/* 02424D14 */
BOOL daPy_lk_c::dProcLetterOpen_init() {
    WWHD_FUNC(0x02424D14, BOOL, this);
    if (mCurProc == 0xCC /* daPyProc_DEMO_LETTER_OPEN_e */) {
        return TRUE;
    }
    int use = gabi::call<int>(LK_checkShipRideUseItem, this, 1);
    gabi::call(LK_commonProcInit, this, 0xCC);
    mNormalSpeed = 0.0f;
    gabi::call(LK_setSingleMoveAnime, this, 0xC6 /* ANM_GETLETTER */, 1.0f, 0.0f, -1, 3.0f);
    gabi::call(LK_deleteEquipItem, this, 0);
    gabi::call(LK_initShipRideUseItem, this, use, 2);
    return TRUE;
}
VERIFY(0x02424D14, &daPy_lk_c::dProcLetterOpen_init);

/* 02424DB8 */
BOOL daPy_lk_c::dProcLetterOpen() {
    WWHD_FUNC(0x02424DB8, BOOL, this);
    gabi::call(LK_setShipRidePosUseItem, this);
    m35EC = mFrameCtrlUnder[0].getFrame();
    if (gabi::load<f32>(m_old_fdata + 0xC) < 0.01f) {
        gabi::call(LK_setLetterModel, this);
    }
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        gabi::call(LK_setLetterModel, this);
        LK_cutEnd();
    }
    return TRUE;
}
VERIFY(0x02424DB8, &daPy_lk_c::dProcLetterOpen);

/* 02424E50 */
BOOL daPy_lk_c::dProcLetterRead_init() {
    WWHD_FUNC(0x02424E50, BOOL, this);
    if (mCurProc == 0xCD /* daPyProc_DEMO_LETTER_READ_e */) {
        return TRUE;
    }
    f32 keep = m35EC;
    int use = gabi::call<int>(LK_checkShipRideUseItem, this, 0);
    gabi::call(LK_commonProcInit, this, 0xCD);
    mNormalSpeed = 0.0f;
    m35EC = keep;
    gabi::call(LK_setSingleMoveAnime, this, 0xC7 /* ANM_WAITLETTER */, 1.0f, 0.0f, -1, 5.0f);
    setNoResetFlg1(noResetFlg1() & ~0x4000u); /* offNoResetFlg1(daPyFlg1_LETTER_READ_EYE_MOVE) */
    mProcVar0 = 0;
    gabi::call(LK_initShipRideUseItem, this, use, 2);
    return TRUE;
}
VERIFY(0x02424E50, &daPy_lk_c::dProcLetterRead_init);

/* 02424F1C */
BOOL daPy_lk_c::dProcLetterRead() {
    WWHD_FUNC(0x02424F1C, BOOL, this);
    gabi::call(LK_setShipRidePosUseItem, this);
    be<u16>* texScrollIdx = gabi::at<be<u16>>(gabi::ea(this) + 0x65E0); /* m_tex_scroll_heap.mIdx */
    if (noResetFlg1() & 0x4000) {
        setNoResetFlg1(noResetFlg1() & ~0x4000u);
        *texScrollIdx = 0x181; /* dRes_INDEX_LKANM_BTK_TEDL_e */
        u32 res = gabi::call<u32>(LK_loadTextureScrollResource, this, 0x181, 0);
        gabi::call(LK_setTextureScrollResource, this, res, 0);
        mProcVar0 = 4;
    } else if (mProcVar0 > 0) {
        mProcVar0 = mProcVar0 - 1;
        if (mProcVar0 == 0) {
            if (*texScrollIdx == 0x181) {
                mProcVar0 = 4;
                *texScrollIdx = 0x182; /* dRes_INDEX_LKANM_BTK_TEDR_e */
                u32 res = gabi::call<u32>(LK_loadTextureScrollResource, this, 0x182, 0);
                gabi::call(LK_setTextureScrollResource, this, res, 0);
            } else {
                *texScrollIdx = 0x183; /* dRes_INDEX_LKANM_BTK_TEDW_e */
                u32 res = gabi::call<u32>(LK_loadTextureScrollResource, this, 0x183, 0);
                gabi::call(LK_setTextureScrollResource, this, res, 0);
            }
        }
    }
    LK_cutEnd();
    return TRUE;
}
VERIFY(0x02424F1C, &daPy_lk_c::dProcLetterRead);

/* 02425048 */
BOOL daPy_lk_c::dProcRedeadStop_init() {
    WWHD_FUNC(0x02425048, BOOL, this);
    if (mCurProc == 0xCE /* daPyProc_DEMO_REDEAD_STOP_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0xCE);
    mNormalSpeed = 0.0f;
    gabi::call(LK_setSingleMoveAnime, this, 0xC8 /* ANM_LINK_FREEZ */, 1.0f, 0.0f, -1, 5.0f);
    return TRUE;
}
VERIFY(0x02425048, &daPy_lk_c::dProcRedeadStop_init);

/* 024250D0 */
BOOL daPy_lk_c::dProcRedeadStop() {
    WWHD_FUNC(0x024250D0, BOOL, this);
    LK_cutEnd();
    return TRUE;
}
VERIFY(0x024250D0, &daPy_lk_c::dProcRedeadStop);

/* 0242510C */
BOOL daPy_lk_c::dProcRedeadCatch_init() {
    WWHD_FUNC(0x0242510C, BOOL, this);
    if (mCurProc == 0xCF /* daPyProc_DEMO_REDEAD_CATCH_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0xCF);
    mNormalSpeed = 0.0f;
    gabi::call(LK_setSingleMoveAnime, this, 0xC9 /* ANM_LINK_MOGAKI */, 1.0f, 0.0f, -1, 5.0f);
    return TRUE;
}
VERIFY(0x0242510C, &daPy_lk_c::dProcRedeadCatch_init);

/* 02425194 */
BOOL daPy_lk_c::dProcRedeadCatch() {
    WWHD_FUNC(0x02425194, BOOL, this);
    LK_cutEnd();
    return TRUE;
}
VERIFY(0x02425194, &daPy_lk_c::dProcRedeadCatch);

/* 024251D0 */
BOOL daPy_lk_c::dProcGetDance_init() {
    WWHD_FUNC(0x024251D0, BOOL, this);
    if (mCurProc == 0xD0 /* daPyProc_DEMO_GET_DANCE_e */) {
        return TRUE;
    }
    int use = gabi::call<int>(LK_checkShipRideUseItem, this, 0);
    gabi::call(LK_commonProcInit, this, 0xD0);
    mNormalSpeed = 0.0f;
    gabi::call(LK_setSingleMoveAnime, this, 0xCA /* ANM_TAKTDGE */, 1.0f, 0.0f, -1, 3.0f);
    mProcVar2 = 0;
    s32 camIdx = mCameraInfoIdx;
    s16 camAngleY = dComIfGp_getCameraAngleY_l(camIdx);
    mProcVar0 = 0;
    current.angle.y = shape_angle.y;
    mProcVar3 = (s16)(shape_angle.y - camAngleY);
    mGameOverId = 0xFFFFFFFF; /* fpcM_ERROR_PROCESS_ID_e */
    mProcVar6 = demoParam0() + 0xD2;
    gabi::call(0x025B7AA8 /* dSv_player_collect_c::onTact */, dComIfGs_base_l() + 0xD4, (u32)gabi::load<u8>(gabi::ea(this) + 0x42B));
    gabi::call(LK_initShipRideUseItem, this, use, 0);
    gabi::call(0x0255F378 /* dKy_Itemgetcol_chg_on */);
    gabi::call(LK_setTactModel, this);
    mProcVar7 = 0;
    return TRUE;
}
VERIFY(0x024251D0, &daPy_lk_c::dProcGetDance_init);

/* 024252E4 */
BOOL daPy_lk_c::dProcGetDance() {
    WWHD_FUNC(0x024252E4, BOOL, this);
    if (dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
        gabi::call(LK_setShipRidePos, this, 0);
    }
    if (mProcVar7 != 0) {
        LK_cutEnd();
        return TRUE;
    }
    f32 frame = mFrameCtrlUnder[0].getFrame();
    if (frame > 17.0f) {
        shape_angle.y = (s16)(current.angle.y - mProcVar3);
    } else if (!(frame < 11.0f)) { /* HD: the frame range test as the code branches */
        if (mProcVar0 == 0) {
            mProcVar0 = 1;
            gabi::call(0x025E1918 /* mDoAud_subBgmStart */, 0x80000027u /* JA_BGM_GET_SONG */);
            gabi::store<u8>(dComIfGp_ea() + 0x5C20, 1); /* dComIfGp_setMesgBgmOn() */
            frame = mFrameCtrlUnder[0].getFrame();
        }
        f32 t = frame - 10.0f;
        if (mProcVar3 > 0x4000) {
            shape_angle.y = (s16)gabi::ftoi((f32)(s32)current.angle.y + ((f32)(s32)(0x10000 - mProcVar3) * t) / 7.0f);
        } else {
            shape_angle.y = (s16)gabi::ftoi((f32)(s32)current.angle.y - ((f32)(s32)mProcVar3 * t) / 7.0f);
        }
    }
    gabi::call(0x025D7C98 /* fopAcM_getItemEventPartner */, this);
    if (mFrameCtrlUnder[0].getRate() < 0.01f && gabi::call<BOOL>(LK_checkEndMessage, this, (s32)mProcVar6)) {
        shape_angle.y = (s16)(shape_angle.y + 0x8000);
        m34DE = shape_angle.y;
        current.angle.y = shape_angle.y;
        gabi::call(LK_setOldRootQuaternion, this, 0, -0x8000, 0);
        u32 info = gabi::load<u32>(m_old_fdata + 0x1C); /* m_old_fdata->getOldFrameTransInfo(0) */
        f32 x = gabi::load<f32>(info + 0x14);
        f32 z = gabi::load<f32>(info + 0x1C);
        gabi::store<f32>(info + 0x14, -x);
        gabi::store<f32>(info + 0x1C, -z);
        LK_cutEnd();
        mProcVar7 = 1;
        gabi::call(LK_setBlendMoveAnime, this, 2.4f /* HD: HIO folded */);
    }
    return TRUE;
}
VERIFY(0x024252E4, &daPy_lk_c::dProcGetDance);

/* 02425548 */
BOOL daPy_lk_c::dProcBottleOpenFairy_init() {
    WWHD_FUNC(0x02425548, BOOL, this);
    if (mCurProc == 0xD1 /* daPyProc_DEMO_BOTTLE_OPEN_FAIRY_e */) {
        return TRUE;
    }
    gabi::call(LK_commonProcInit, this, 0xD1);
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    gabi::call(LK_setSingleMoveAnime, this, 0xB9 /* ANM_BINOPENPRE */, 1.0f, 0.0f, 0x2D, 2.0f); /* HD: HIO folded */
    gabi::call(LK_keepItemData, this);
    gabi::call(LK_setBottleModel, this, 0x57 /* dItemNo_FAIRY_BOTTLE_e */);
    u32 st1 = dComIfGp_ea() + 0x5CDC; /* dComIfGp_setPlayerStatus1(0, daPyStts1_UNK4000_e) */
    gabi::store<u32>(st1, gabi::load<u32>(st1) | 0x4000);
    mProcVar6 = 0;
    return TRUE;
}
VERIFY(0x02425548, &daPy_lk_c::dProcBottleOpenFairy_init);

/* 024255EC */
BOOL daPy_lk_c::dProcBottleOpenFairy() {
    WWHD_FUNC(0x024255EC, BOOL, this);
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    if (gabi::load<u16>(gabi::ea(this) + 0x5848) == 0x30 /* m_anm_heap_under[0].mIdx == BINOPENPRE */) {
        if (frameCtrl.getRate() < 0.01f) {
            gabi::call(LK_setSingleMoveAnime, this, 0xBB /* ANM_BINOPENB */, 1.0f, 0.0f, 0x7C, 5.0f);
        } else if (frameCtrl.checkPass(32.0f)) {
            m355E = 0;
            mRightHandIdx = 8;
        }
    } else if (frameCtrl.getRate() < 0.01f) {
        LK_cutEnd();
    } else if (mpBottleContentsModel != 0) {
        if (!(frameCtrl.getFrame() < 0.0f)) { /* HD: HIO field_0x58 folded */
            mpBottleContentsModel = 0;
            u32 btn;
            if (gabi::load<u8>(dComIfGp_ea() + 0x52B0) == 1 /* dTalkBtn_X_e */) {
                btn = 0;
            } else {
                btn = gabi::load<u8>(dComIfGp_ea() + 0x52B0) == 2 ? 1 : 2;
            }
            gabi::call(0x025B58B8 /* dSv_player_item_c::setEquipBottleItemEmpty */, dComIfGs_base_l() + 0x5C, btn);
            gabi::call(0x025D5928 /* fopAcM_fastCreate */, 0x168 /* NPC_FA1 */, 3 /* Type_BABA_e */, &mLeftHandPos,
                       (s32)current.roomNo, &shape_angle, nullptr, -1, 0, 0);
        }
    }
    return TRUE;
}
VERIFY(0x024255EC, &daPy_lk_c::dProcBottleOpenFairy);

/* 02425788 */
BOOL daPy_lk_c::dProcWarpShort_init() {
    WWHD_FUNC(0x02425788, BOOL, this);
    if (mCurProc == 0xD2 /* daPyProc_DEMO_WARP_SHORT_e */) {
        return TRUE;
    }
    gabi::call(LK_commonProcInit, this, 0xD2);
    gabi::call(LK_setSingleMoveAnime, this, 0xC0 /* ANM_RISE */, 1.0f, 0.0f, -1, 40.0f);
    gravity = mNormalSpeed = 0.0f;
    speed.y = 0.5f; /* HD: HIO folded */
    if (mEquipItem == 0x101 /* daPyItem_BOKO_e */) {
        gabi::call(LK_deleteEquipItem, this, 0);
    }
    return TRUE;
}
VERIFY(0x02425788, &daPy_lk_c::dProcWarpShort_init);

/* 0242583C */
BOOL daPy_lk_c::dProcWarpShort() {
    WWHD_FUNC(0x0242583C, BOOL, this);
    f32 v = gabi::fadds_ppc(speed.y, 0.1f);
    if (v > 1.35f) {
        v = 1.35f;
    }
    speed.y = v;
    LK_cutEnd();
    return TRUE;
}
VERIFY(0x0242583C, &daPy_lk_c::dProcWarpShort);

/* 024258A0 */
BOOL daPy_lk_c::dProcOpenSalvageTreasure_init() {
    WWHD_FUNC(0x024258A0, BOOL, this);
    if (mCurProc == 0xD3 /* daPyProc_DEMO_OPEN_SALVAGE_TREASURE_e */) {
        return TRUE;
    }
    int use = gabi::call<int>(LK_checkShipRideUseItem, this, 1);
    gabi::call(LK_commonProcInit, this, 0xD3);
    gabi::call(LK_setSingleMoveAnime, this, 0xD4 /* ANM_BOXOPENSLINK */, 1.0f, 0.0f, -1, -1.0f);
    gabi::call(LK_deleteEquipItem, this, 0);
    gabi::call(LK_initShipRideUseItem, this, use, 1);
    fopAc_ac_c* ship = gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + 0x5B3C)); /* dComIfGp_getShipActor() */
    if (ship != nullptr) {
        shape_angle.y = ship->shape_angle.y;
        current.angle.y = shape_angle.y;
    }
    return TRUE;
}
VERIFY(0x024258A0, &daPy_lk_c::dProcOpenSalvageTreasure_init);

/* 0242595C */
BOOL daPy_lk_c::dProcOpenSalvageTreasure() {
    WWHD_FUNC(0x0242595C, BOOL, this);
    if (dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
        gabi::call(LK_setShipRidePos, this, 1);
    }
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        LK_cutEnd();
    }
    return TRUE;
}
VERIFY(0x0242595C, &daPy_lk_c::dProcOpenSalvageTreasure);

/* 024259D8 */
BOOL daPy_lk_c::dProcSurprisedWait_init() {
    WWHD_FUNC(0x024259D8, BOOL, this);
    if (mCurProc == 0xD4 /* daPyProc_DEMO_SURPRISED_WAIT_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0xD4);
    int use = gabi::call<int>(LK_checkShipRideUseItem, this, 0);
    gabi::call(LK_setSingleMoveAnime, this, 0xD8 /* ANM_SURPRISEDWAIT */, 1.0f, 0.0f, -1, 3.0f);
    if (anm_getFrameMax(mpAnmTexPatternData) >= 1) {
        m3530 = anm_getFrameMax(mpAnmTexPatternData) - 1;
    }
    if (anm_getFrameMax(mpTexScrollResData) >= 1) {
        m3532 = anm_getFrameMax(mpTexScrollResData) - 1;
    }
    mNormalSpeed = 0.0f;
    gabi::call(LK_initShipRideUseItem, this, use, 0);
    return TRUE;
}
VERIFY(0x024259D8, &daPy_lk_c::dProcSurprisedWait_init);

/* 02425B24 */
BOOL daPy_lk_c::dProcSurprisedWait() {
    WWHD_FUNC(0x02425B24, BOOL, this);
    if (dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
        gabi::call(LK_setShipRidePos, this, 0);
    }
    LK_cutEnd();
    return TRUE;
}
VERIFY(0x02425B24, &daPy_lk_c::dProcSurprisedWait);

/* 02425B80 */
BOOL daPy_lk_c::dProcPowerUpWait_init() {
    WWHD_FUNC(0x02425B80, BOOL, this);
    if (mCurProc == 0xD5 /* daPyProc_DEMO_POWER_UP_WAIT_e */) {
        return TRUE;
    }
    int use = gabi::call<int>(LK_checkShipRideUseItem, this, 1);
    gabi::call(LK_commonProcInit, this, 0xD5);
    gabi::call(LK_setSingleMoveAnime, this, 0xDA /* ANM_POWUPWAIT */, 1.0f, 0.0f, -1, 3.0f);
    mNormalSpeed = 0.0f;
    gabi::call(LK_initShipRideUseItem, this, use, 2);
    if (dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
        shape_angle.y = (s16)(shape_angle.y - 0x8000);
        current.angle.y = shape_angle.y;
    }
    return TRUE;
}
VERIFY(0x02425B80, &daPy_lk_c::dProcPowerUpWait_init);

/* 02425C54 */
BOOL daPy_lk_c::dProcPowerUp_init() {
    WWHD_FUNC(0x02425C54, BOOL, this);
    if (mCurProc == 0xD6 /* daPyProc_DEMO_POWER_UP_e */) {
        return TRUE;
    }
    int use = gabi::call<int>(LK_checkShipRideUseItem, this, 1);
    gabi::call(LK_commonProcInit, this, 0xD6);
    gabi::call(LK_setSingleMoveAnime, this, 0xDB /* ANM_POWUP */, 1.0f, 0.0f, -1, 3.0f);
    mNormalSpeed = 0.0f;
    m3624 = 0;
    mProcVar0 = 0;
    s32 p0 = demoParam0();
    if (p0 != 0) {
        if (p0 != 0x100) {
            mProcVar6 = p0;
        } else {
            mProcVar6 = gabi::load<u8>(dComIfGp_ea() + 0x52A4); /* dComIfGp_event_getGtItm() */
        }
    }
    mProcVar7 = 0;
    mGameOverId = 0xFFFFFFFF;
    gabi::call(LK_initShipRideUseItem, this, use, 2);
    if (dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
        shape_angle.y = (s16)(shape_angle.y - 0x8000);
        current.angle.y = shape_angle.y;
    }
    return TRUE;
}
VERIFY(0x02425C54, &daPy_lk_c::dProcPowerUp_init);

/* 02425D68 */
BOOL daPy_lk_c::dProcPowerUp() {
    WWHD_FUNC(0x02425D68, BOOL, this);
    gabi::call(LK_setShipRidePosUseItem, this);
    if (dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
        shape_angle.y = (s16)(shape_angle.y - 0x8000);
        current.angle.y = shape_angle.y;
    }
    if (mProcVar7 != 0) {
        LK_cutEnd();
        return TRUE;
    } else if (!(mFrameCtrlUnder[0].getFrame() < 11.0f) && mProcVar0 == 0) {
        mProcVar0 = 1;
        gabi::call(0x025E1918 /* mDoAud_subBgmStart */, 0x80000002u /* JA_BGM_ITEM_GET */);
        gabi::store<u8>(dComIfGp_ea() + 0x5C20, 1); /* dComIfGp_setMesgBgmOn() */
    } else if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        if (m3624 == 0) {
            m3624 = mProcVar6 + 0x65; /* MSG_NO_FOR_ITEM */
        }
        if (gabi::call<BOOL>(LK_checkEndMessage, this, (u32)m3624)) {
            LK_cutEnd();
            gabi::call(LK_setBlendMoveAnime, this, 2.4f /* HD: HIO folded */);
            mProcVar7 = 1;
        }
    }
    return TRUE;
}
VERIFY(0x02425D68, &daPy_lk_c::dProcPowerUp);

/* 02425E98 */
BOOL daPy_lk_c::dProcShipSit_init() {
    WWHD_FUNC(0x02425E98, BOOL, this);
    if (mCurProc == 0xD7 /* daPyProc_DEMO_SHIP_SIT_e */) {
        return TRUE;
    }
    int use = gabi::call<int>(LK_checkShipRideUseItem, this, 1);
    if (use == 0) {
        if (gabi::load<u32>(dComIfGp_ea() + 0x5B3C) == 0) { /* !dComIfGp_getShipActor() */
            return gabi::call<BOOL>(LK_checkNextMode, this, 0);
        }
        use = 1;
    }
    gabi::call(LK_commonProcInit, this, 0xD7);
    gabi::call(LK_setSingleMoveAnime, this, 0xDC /* ANM_KOSHIKAKE */, 1.0f, 0.0f, -1, -1.0f);
    mNormalSpeed = 0.0f;
    gabi::call(LK_initShipRideUseItem, this, use, 1);
    u32 ship = gabi::load<u32>(dComIfGp_ea() + 0x5B3C);
    gabi::store<u8>(ship + 0x636, 2);                                     /* ship->onLinkSit() */
    gabi::store<u32>(ship + 0x644, gabi::load<u32>(ship + 0x644) | 0x4000000); /* ship->setPaddleMove() */
    return TRUE;
}
VERIFY(0x02425E98, &daPy_lk_c::dProcShipSit_init);

/* 02425FD8 */
BOOL daPy_lk_c::dProcShipSit() {
    WWHD_FUNC(0x02425FD8, BOOL, this);
    if (dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
        gabi::call(LK_setShipRidePos, this, 1);
    }
    LK_cutEnd();
    return TRUE;
}
VERIFY(0x02425FD8, &daPy_lk_c::dProcShipSit);

/* 02426034 */
BOOL daPy_lk_c::dProcLastCombo_init() {
    WWHD_FUNC(0x02426034, BOOL, this);
    if (mCurProc == 0xD8 /* daPyProc_DEMO_LAST_COMBO_e */) {
        return TRUE;
    }
    gabi::call(LK_commonProcInit, this, 0xD8);
    gabi::call(LK_setSingleMoveAnime, this, 0xDD /* ANM_COMBO_LINK */, 1.0f, 0.0f, -1, -1.0f);
    mNormalSpeed = 0.0f;
    if (mEquipItem != 0x103 /* daPyItem_SWORD_e */) {
        gabi::call(LK_deleteEquipItem, this, 0);
        gabi::call(LK_setSwordModel, this, 0);
    }
    u32 bck = gabi::call<u32>(LK_getItemAnimeResource, this, 0x3B /* dRes_INDEX_LKANM_BCK_COMBO_KEN_e */);
    gabi::call(0x025E871C /* mDoExt_bckAnm::changeBckOnly */, gabi::ea(this) + 0x4444 /* mSwordAnim */, bck);
    u32 shms = LK_FIELD(u32, 0xE88); /* mpShmsModel */
    m35E8 = m35EC = 0.0f;
    bck = gabi::ea(dComIfG_getObjectRes(STR(0x101CEB48) /* l_arcName "Link" */, 0xC /* dRes_INDEX_LINK_BCK_COMBO_TATE_e */, 0x10034B24));
    /* mAtngshaBck.init(mpShmsModel->getModelData(), bck, false, EMode_NONE, 0.0f, 0, -1, true) */
    gabi::call(0x025E8508, gabi::ea(this) + 0xE8C, gabi::load<u32>(shms + 0xAC), bck, 0, 0, 0.0f, 0, -1, 1);
    mFootEffectPosType = 4;
    setNoResetFlg1(noResetFlg1() & ~0x20000000u); /* offNoResetFlg1(daPyFlg1_LAST_COMBO_WAIT) */
    gabi::call(LK_setBlurPosResource, this, 0x28A /* dRes_INDEX_LKANM__LASTCOMBO_POS_e */);
    mProcVar6 = 1;
    mProcVar7 = 0;
    seStartOnlyReverb(0x188A /* JA_SE_LK_V_TURN_INTO_GN */);
    return TRUE;
}
VERIFY(0x02426034, &daPy_lk_c::dProcLastCombo_init);

/* 024261A0 */
BOOL daPy_lk_c::dProcLastCombo() {
    WWHD_FUNC(0x024261A0, BOOL, this);
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    f32 frame = frameCtrl.getFrame();
    m35EC = frame;
    m35E8 = frame;
    if (frameCtrl.getRate() < 0.01f) {
        LK_cutEnd();
    } else if (frameCtrl.checkPass(27.0f)) {
        gabi::call(LK_resetFootEffect, this);
        seStartOnlyReverb(0x188B /* JA_SE_LK_V_GN_LAST_ATTACK */);
    }
    frame = frameCtrl.getFrame();
    if (!(frame < 26.0f) && frame < 35.0f) {
        u32 f = resetFlg0();
        if (!(mNoResetFlg0 & 0x40 /* daPyFlg0_CUT_AT_FLG */)) {
            f |= 1; /* daPyRFlg0_UNK1 */
        }
        setResetFlg0(f | 2);
    }
    return TRUE;
}
VERIFY(0x024261A0, &daPy_lk_c::dProcLastCombo);

/* 02426298 */
BOOL daPy_lk_c::dProcHandUp_init() {
    WWHD_FUNC(0x02426298, BOOL, this);
    if (mCurProc == 0xD9 /* daPyProc_DEMO_HAND_UP_e */) {
        return TRUE;
    }
    gabi::call(LK_commonProcInit, this, 0xD9);
    gabi::call(LK_setSingleMoveAnime, this, 0xE0 /* ANM_WAITAUCTION */, 1.0f, 0.0f, -1, 10.0f);
    mNormalSpeed = 0.0f;
    return TRUE;
}
VERIFY(0x02426298, &daPy_lk_c::dProcHandUp_init);

/* 02426324 */
BOOL daPy_lk_c::dProcHandUp() {
    WWHD_FUNC(0x02426324, BOOL, this);
    LK_cutEnd();
    return TRUE;
}
VERIFY(0x02426324, &daPy_lk_c::dProcHandUp);

/* 02426360 */
BOOL daPy_lk_c::dProcIceSlip_init() {
    WWHD_FUNC(0x02426360, BOOL, this);
    if (mCurProc == 0xDA /* daPyProc_DEMO_ICE_SLIP_e */) {
        return TRUE;
    }
    gabi::call(LK_commonProcInit, this, 0xDA);
    gabi::call(LK_setSingleMoveAnime, this, 0xA9 /* ANM_SLIPICE */, 1.0f, 0.0f, -1, 3.0f);
    mNormalSpeed = 0.0f;
    return TRUE;
}
VERIFY(0x02426360, &daPy_lk_c::dProcIceSlip_init);

/* 024263EC */
BOOL daPy_lk_c::dProcIceSlip() {
    WWHD_FUNC(0x024263EC, BOOL, this);
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        LK_cutEnd();
    }
    return TRUE;
}
VERIFY(0x024263EC, &daPy_lk_c::dProcIceSlip);

/* ---- HD-only demo procs (no GameCube source; written from the WWHD code). They use the
 * letter animations (ANM_GETLETTER) with a text-input step and an extra camera: probably the HD
 * Tingle Bottle (message bottle) feature. Names are descriptive, not original. ---- */

/* 0242643C HD: init of proc 0xDD (the letter animation played backwards: closing) */
BOOL daPy_lk_c::dProcHDLetterClose_init() {
    WWHD_FUNC(0x0242643C, BOOL, this);
    if (mCurProc == 0xDD) {
        return TRUE;
    }
    mCurProc = 0xDD; /* HD: stored before checkShipRideUseItem as well */
    int use = gabi::call<int>(LK_checkShipRideUseItem, this, 1);
    gabi::call(LK_commonProcInit, this, 0xDD);
    mNormalSpeed = 0.0f;
    gabi::call(LK_setSingleMoveAnime, this, 0xC6 /* ANM_GETLETTER */, -1.0f, 0.0f, -1, 5.0f);
    mFrameCtrlUnder[0].mAttribute = 1;
    s32 max = anm_getFrameMax(LK_FIELD(u32, 0x581C)); /* [?] the under-move animation (mAnmRatioUnder) */
    mFrameCtrlUnder[0].mFrame = (f32)max;
    gabi::call(LK_deleteEquipItem, this, 0);
    gabi::call(LK_setLetterModel, this);
    J3DFrameCtrl* sw = gabi::at<J3DFrameCtrl>(gabi::ea(this) + 0x4444); /* mSwordAnim's frame control */
    s16 end = sw->mEnd;
    fcopy_l(&m35EC, &mFrameCtrlUnder[0].mFrame); /* m35EC = frame (bit copy, as the original) */
    sw->mAttribute = 1;
    sw->mFrame = (f32)end;
    sw->mRate = -1.0f;
    gabi::call(LK_initShipRideUseItem, this, use, 2);
    mProcVar6 = 0;
    return TRUE;
}
VERIFY(0x0242643C, &daPy_lk_c::dProcHDLetterClose_init);

/* 024265A0 HD: the letter (message) is opened, a text input runs, then it is closed */
BOOL daPy_lk_c::dProcHDLetterWrite() {
    WWHD_FUNC(0x024265A0, BOOL, this);
    gabi::call(LK_setShipRidePosUseItem, this);
    J3DFrameCtrl& fc = mFrameCtrlUnder[0];
    s32 st = mProcVar6;
    if (st == 0) {
        f32 fr = fc.getFrame();
        if (fr > 40.0f) {
            fc.mRate = 0.75f;
            fr = fc.getFrame();
        }
        if (fr > 43.0f) {
            gabi::call(LK_setSingleMoveAnime, this, 0xC6 /* ANM_GETLETTER */, 1.0f, 0.0f, -1, 5.0f);
            m35A0 = 0.0f;
            mProcVar6 = 1;
        }
        return TRUE;
    }
    if (st == 1) {
        f32 fr = fc.getFrame();
        m35EC = fr;
        fc.mRate = fsel_l(fr - 6.0f, 1.0f, 0.5f);
        if (gabi::load<f32>(m_old_fdata + 0xC) < 0.01f) {
            gabi::call(LK_setLetterModel, this);
        }
        f32 end = (f32)(s32)(fc.mEnd - 1);
        f32 t = m35A0;
        if (!(fc.getFrame() < end)) {
            t = t + 1.0f;
            m35A0 = t;
        }
        if (t > 20.0f) {
            m35A0 = 0.0f;
            mProcVar6 = 2;
        }
        return TRUE;
    }
    if (st == 2) {
        f32 t = m35A0;
        if (t < 0.1f) {
            gabi::call(0x0203C118, gabi::load<u32>(0x1018F504), -1); /* HD: starts the text input (unnamed) */
            t = m35A0;
        }
        t = t + 1.0f;
        m35A0 = t;
        if (t < 2.1f) {
            return TRUE;
        }
        if (gabi::load<u8>(0x1018F510) != 0 && gabi::load<s16>(0x1047C7FA) == 0) {
            gabi::call(LK_setSingleMoveAnime, this, 0xC6 /* ANM_GETLETTER */, -1.0f, 0.0f, -1, 5.0f);
            fc.mAttribute = 1;
            s32 max = anm_getFrameMax(LK_FIELD(u32, 0x581C));
            fc.mFrame = (f32)max;
            gabi::call(LK_deleteEquipItem, this, 0);
            gabi::call(LK_setLetterModel, this);
            J3DFrameCtrl* sw = gabi::at<J3DFrameCtrl>(gabi::ea(this) + 0x4444); /* mSwordAnim */
            s16 end = sw->mEnd;
            mProcVar6 = 3;
            m35A0 = 0.0f;
            sw->mAttribute = 1;
            fcopy_l(&m35EC, &fc.mFrame); /* bit copy, as the original */
            sw->mFrame = (f32)end;
            sw->mRate = -1.0f;
            return TRUE;
        }
        return dProcHDLetterClose_init();
    }
    if (st == 3) {
        f32 fr = fc.getFrame();
        f32 rate = fsel_l(fr - 6.0f, -1.0f, -0.5f);
        m35EC = fr;
        fc.mRate = rate;
        if (fr > 0.001f) {
            return TRUE;
        }
        mProcVar6 = 4;
        return gabi::call<BOOL>(LK_HD_023F42EC, this);
    }
    return TRUE;
}
VERIFY(0x024265A0, &daPy_lk_c::dProcHDLetterWrite);

/* 0242705C HD: closes the letter, puts the item away and ends the event camera */
BOOL daPy_lk_c::dProcHDLetterEnd() {
    WWHD_FUNC(0x0242705C, BOOL, this);
    gabi::call(LK_setShipRidePosUseItem, this);
    J3DFrameCtrl& fc = mFrameCtrlUnder[0];
    s32 st = mProcVar6;
    f32 rate;
    if (st == 0) {
        f32 fr = fc.getFrame();
        fc.mRate = fsel_l(fr - 6.0f, -1.0f, -0.5f);
        m35EC = fr;
        if (fr > 0.001f) {
            st = mProcVar6;
            if (st != 1) {
                goto state2;
            }
            rate = fsel_l(40.0f - fc.getFrame(), -1.0f, -0.75f);
        } else {
            gabi::call(LK_deleteEquipItem, this, 0);
            gabi::call(LK_setSingleMoveAnime, this, 0xB2, -1.0f, 0.0f, -1, 5.0f);
            fc.mAttribute = 1;
            mProcVar6 = 1;
            fc.mFrame = 43.0f;
            rate = fsel_l(40.0f - 43.0f, -1.0f, -0.75f);
        }
    } else if (st == 1) {
        rate = fsel_l(40.0f - fc.getFrame(), -1.0f, -0.75f);
    } else {
        goto state2;
    }
    {
        f32 fr = fc.getFrame();
        fc.mRate = rate;
        if (!(fr > 25.0f)) {
            gabi::call(LK_setSingleMoveAnime, this, 0 /* ANM_WAITS */, 1.0f, 0.0f, -1, 5.0f);
            fc.mAttribute = 2;
            mProcVar6 = 2;
        }
    }
    return TRUE;
state2:
    if (st == 2) {
        u32 cam = gabi::call<u32>(0x024F8044 /* dCam_getBody */);
        gabi::call(0x0253E860 /* dCamera_c::EndEventCamera */, cam, gabi::load<u32>(gabi::ea(this) + 4) /* fopAcM_GetID */);
        u32 a = dComIfGp_ea() + 0x52B8;
        gabi::store<u16>(a, gabi::load<u16>(a) | 8);
        gabi::call(LK_endDemoMode, this);
        mProcVar6 = 3;
    }
    return TRUE;
}
VERIFY(0x0242705C, &daPy_lk_c::dProcHDLetterEnd);

/* 02427230 */
void daPy_lk_c::setBodyAngleXReadyAnime() {
    WWHD_FUNC(0x02427230, void, this);
    Mtx34* mtx = lk_getAnmMtx(mpCLModel, 2 /* CL_JNT_BODY_CHN_e */);
    u32 m = gabi::ea(mtx);
    gabi::Local<cXyz> pos; /* mDoMtx_multVecZero */
    pos->x = gabi::load<f32>(m + 0x0C);
    pos->y = gabi::load<f32>(m + 0x1C);
    pos->z = gabi::load<f32>(m + 0x2C);
    fopAc_ac_c* lockOn = mpAttnActorLockOn;
    s16 target;
    if (lockOn != nullptr) {
        gabi::Local<cXyz> d;
        cXyz_mi(&lockOn->eyePos, d, pos);
        d->y = d->y - 20.0f;
        if (std_sqrtf(PSVECSquareMag(d)) < 1.0f) {
            target = 0;
        } else {
            gabi::Local<cXyz> xz;
            xz->x = d->x;
            xz->y = 0.0f;
            xz->z = d->z;
            f32 h = std_sqrtf(PSVECSquareMag(xz));
            target = cM_atan2s(-d->y, h);
            if (target > 0x2000) {
                target = 0x2000;
            } else if (target < -0x2000) {
                target = -0x2000;
            }
        }
    } else {
        target = 0;
    }
    cLib_addCalcAngleS(&mBodyAngleX, target, 4, 0xC00, 0x180);
}
VERIFY(0x02427230, &daPy_lk_c::setBodyAngleXReadyAnime);

/* 02427370 */
BOOL daPy_lk_c::checkItemModeActorPointer() {
    WWHD_FUNC(0x02427370, BOOL, this);
    if (mActorKeepEquip.mActor != nullptr) {
        return TRUE;
    }
    gabi::call(0x023DC63C /* daPy_actorKeep_c::clearData */, &mActorKeepEquip);
    gabi::call(LK_resetActAnimeUpper, this, 2 /* UPPER_MOVE2_e */, -1.0f);
    mEquipItem = 0x100; /* daPyItem_NONE_e */
    gabi::call(LK_procWait_init, this);
    return FALSE;
}
VERIFY(0x02427370, &daPy_lk_c::checkItemModeActorPointer);

/* ---- d_a_player_ladder.inc ---- */

/* 024273EC */
void daPy_lk_c::setLadderFootSe() {
    WWHD_FUNC(0x024273EC, void, this);
    gabi::Local<cXyz> end;
    end->x = gabi::fmadds(50.0f, cM_ssin(shape_angle.y), current.pos.x);
    end->y = current.pos.y;
    end->z = gabi::fmadds(50.0f, cM_scos(shape_angle.y), current.pos.z);
    dBgS_LinChk_Set(mLinkLinChk, &current.pos, end, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        u32 mtrl = dBgS_GetMtrlSndId(dComIfG_Bgsp(), gabi::at<cBgS_PolyInfo>(mLinkLinChkPoly));
        mDoAud_seStart_l(0x3818 /* JA_SE_FT_LADDER_CLIMB_D */, &current.pos, mtrl, mReverb);
    } else {
        seStartOnlyReverb(0x3818);
    }
}
VERIFY(0x024273EC, &daPy_lk_c::setLadderFootSe);

/* 024274D0 */
BOOL daPy_lk_c::procLadderDownEnd_init(int param_0) {
    WWHD_FUNC(0x024274D0, BOOL, this, param_0);
    gabi::call(LK_commonProcInit, this, 0x3B /* daPyProc_LADDER_DOWN_END_e */);
    int anm = param_0 != 0 ? 0x89 /* ANM_LADDERDWEDL */ : 0x88 /* ANM_LADDERDWEDR */;
    gabi::call(LK_setSingleMoveAnime, this, anm, 1.0f, 0.0f, -1, 2.0f); /* HD: HIO folded */
    gravity = 0.0f;
    m34C2 = 7;
    dComIfGp_onPlayerStatus0_l(0x2000000 /* daPyStts0_UNK2000000_e */);
    return TRUE;
}
VERIFY(0x024274D0, &daPy_lk_c::procLadderDownEnd_init);

/* 0242757C */
int daPy_lk_c::changeLadderMoveProc(int param_0) {
    WWHD_FUNC(0x0242757C, int, this, param_0);
    cLib_addCalc_l(&current.pos.x, m370C.x, 0.5f, 0.5f, 0.05f);
    cLib_addCalc_l(&current.pos.y, m370C.y, 0.5f, 0.5f, 0.05f);
    cLib_addCalc_l(&current.pos.z, m370C.z, 0.5f, 0.5f, 0.05f);
    if (mStickDistance > 0.05f) {
        s32 d = (s16)(m34E8 - shape_angle.y);
        int iVar2 = d < 0 ? -d : d;
        if (iVar2 > 0x3C72 && iVar2 < 0x438E) {
            return FALSE;
        }
        gabi::Local<cXyz> start;
        gabi::Local<cXyz> end;
        f32 x = current.pos.x;
        f32 y = current.pos.y;
        f32 z = current.pos.z;
        start->x = x;
        f32 hdOff = gabi::load<f32>(0x1046CCE0); /* HD: a global tuning value (GameCube 50.0f) */
        start->z = z;
        int direction;
        if (iVar2 < 0x4000) {
            start->y = (y + 125.0f) + hdOff;
            direction = 0; /* DIR_FORWARD */
        } else {
            start->y = y - hdOff;
            direction = 1; /* DIR_BACKWARD */
        }
        end->y = start->y;
        end->x = gabi::fmadds(50.0f, cM_ssin(shape_angle.y), x);
        end->z = gabi::fmadds(50.0f, cM_scos(shape_angle.y), z);
        dBgS_LinChk_Set(mLinkLinChk, start, end, this);
        if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk) || dBgS_GetWallCode_l(gabi::at<void>(mLinkLinChkPoly)) != 4) {
            if (iVar2 < 0x4000) {
                gabi::call(LK_procLadderUpEnd_init, this, param_0);
            } else {
                procLadderDownEnd_init(param_0);
            }
        } else {
            gabi::call(LK_procLadderMove_init, this, param_0, direction, &m370C);
        }
    }
    return FALSE;
}
VERIFY(0x0242757C, &daPy_lk_c::changeLadderMoveProc);

/* 02427804 */
int daPy_lk_c::setMoveBGLadderCorrect() {
    WWHD_FUNC(0x02427804, int, this);
    if (dBgS_ChkPolySafe_l(mPolyInfo) && dBgS_ChkMoveBG_l(mPolyInfo)) {
        dBgS_MoveBgTransPos_l(mPolyInfo, true, &current.pos, &current.angle, &shape_angle);
        dBgS_MoveBgTransPos_l(mPolyInfo, true, &m370C, nullptr, nullptr);
    }
    gabi::Local<cXyz> end;
    end->x = gabi::fmadds(50.0f, cM_ssin(current.angle.y), m370C.x);
    end->y = m370C.y;
    end->z = gabi::fmadds(50.0f, cM_scos(current.angle.y), m370C.z);
    dBgS_LinChk_Set(mLinkLinChk, &m370C, end, this);
    if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk) || dBgS_GetWallCode_l(gabi::at<void>(mLinkLinChkPoly)) != 4) {
        return gabi::call<int>(LK_procFall_init, this, 1, 6.0f /* HD: HIO folded */);
    }
    polyInfo_copy_l(gabi::ea(mPolyInfo), mLinkLinChkPoly); /* mPolyInfo = mLinkLinChk */
    return FALSE;
}
VERIFY(0x02427804, &daPy_lk_c::setMoveBGLadderCorrect);

/* 02427974 */
BOOL daPy_lk_c::procLadderUpStart() {
    WWHD_FUNC(0x02427974, BOOL, this);
    if (setMoveBGLadderCorrect()) {
        return TRUE;
    }
    if (mProcVar6 == 0) {
        if (LK_checkNoUpperAnime()) {
            gabi::call(LK_procLadderUpStart_init_sub, this);
        }
    } else {
        J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
        m34C2 = 5;
        if (frameCtrl.getRate() < 0.01f) {
            dComIfGp_setDoStatus_l(6 /* dActStts_LET_GO_e */);
            if (mProcVar2 == 0) {
                setLadderFootSe();
                mProcVar2 = 1;
            }
            if (LK_doTrigger()) {
                m34C2 = 0;
                gabi::call(LK_procFall_init, this, 1, 6.0f);
            } else {
                changeLadderMoveProc(1);
            }
        } else if (frameCtrl.checkPass(9.0f)) {
            setLadderFootSe();
        }
    }
    return TRUE;
}
VERIFY(0x02427974, &daPy_lk_c::procLadderUpStart);

/* 02427A74 */
BOOL daPy_lk_c::procLadderUpEnd() {
    WWHD_FUNC(0x02427A74, BOOL, this);
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    if (setMoveBGLadderCorrect()) {
        return TRUE;
    }
    if (frameCtrl.getRate() < 0.01f) {
        gabi::call(LK_checkNextMode, this, 0);
    } else {
        if (frameCtrl.checkPass(9.0f) || frameCtrl.checkPass(19.0f)) {
            setLadderFootSe();
        }
        m34C2 = 5;
    }
    return TRUE;
}
VERIFY(0x02427A74, &daPy_lk_c::procLadderUpEnd);

/* 02427B20 */
BOOL daPy_lk_c::procLadderDownStart() {
    WWHD_FUNC(0x02427B20, BOOL, this);
    if (setMoveBGLadderCorrect()) {
        return TRUE;
    }
    if (mProcVar6 == 0) {
        if (LK_checkNoUpperAnime()) {
            gabi::call(LK_procLadderDownStart_init_sub, this);
        }
    } else {
        J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
        m34C2 = 5;
        if (frameCtrl.getRate() < 0.01f) {
            dComIfGp_setDoStatus_l(6 /* dActStts_LET_GO_e */);
            if (LK_doTrigger()) {
                m34C2 = 0;
                gabi::call(LK_procFall_init, this, 1, 6.0f);
            } else {
                changeLadderMoveProc(1);
            }
        } else if (frameCtrl.checkPass(26.0f) || frameCtrl.checkPass(36.0f) || frameCtrl.checkPass(43.0f)) {
            setLadderFootSe();
        }
    }
    return TRUE;
}
VERIFY(0x02427B20, &daPy_lk_c::procLadderDownStart);

/* 02427C40 */
BOOL daPy_lk_c::procLadderDownEnd() {
    WWHD_FUNC(0x02427C40, BOOL, this);
    if (setMoveBGLadderCorrect()) {
        return TRUE;
    }
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        gabi::call(LK_checkNextMode, this, 0);
    } else {
        m34C2 = 5;
    }
    return TRUE;
}
VERIFY(0x02427C40, &daPy_lk_c::procLadderDownEnd);

/* 02427CA8 */
BOOL daPy_lk_c::procLadderMove() {
    WWHD_FUNC(0x02427CA8, BOOL, this);
    if (setMoveBGLadderCorrect()) {
        return TRUE;
    }
    m34C2 = 5;
    dComIfGp_setDoStatus_l(6 /* dActStts_LET_GO_e */);
    if (LK_doTrigger()) {
        m34C2 = 0;
        gabi::call(LK_procFall_init, this, 1, 6.0f);
    } else {
        f32 rate = mFrameCtrlUnder[0].getRate();
        if (std::fabs(rate) < 0.01f) {
            if (mProcVar0 > 0) {
                mProcVar0 = mProcVar0 - 1;
                setLadderFootSe();
            } else {
                changeLadderMoveProc(mProcVar6);
            }
        } else {
            f32 spd = gabi::call<f32>(LK_getLadderMoveAnmSpeed, this);
            if (rate < 0.0f) { /* HD: the rate read before the call */
                spd = -spd;
            }
            mFrameCtrlUnder[0].setRate(spd);
        }
    }
    return TRUE;
}
VERIFY(0x02427CA8, &daPy_lk_c::procLadderMove);

/* ---- d_a_player_hang.inc ---- */

/* 02427D8C */
f32 daPy_lk_c::getHangMoveAnmSpeed() {
    WWHD_FUNC(0x02427D8C, f32, this);
    /* getAnmSpeedStickRate(0.7f, 1.4f) inlined, HIO folded: 0.7 + stick * 0.7 */
    return gabi::fmadds(0.7f, mStickDistance, 0.7f);
}
VERIFY(0x02427D8C, &daPy_lk_c::getHangMoveAnmSpeed);

/* 02427DA0 */
int daPy_lk_c::getHangDirectionFromAngle() {
    WWHD_FUNC(0x02427DA0, int, this);
    s32 d = (s16)(m34E8 - shape_angle.y);
    if ((d < 0 ? -d : d) > 0x78E4) {
        return 1; /* DIR_BACKWARD */
    } else if (d >= 0x71C) {
        return 2; /* DIR_LEFT */
    } else if (d > -0x71C) {
        return 0; /* DIR_FORWARD */
    }
    return 3; /* DIR_RIGHT */
}
VERIFY(0x02427DA0, &daPy_lk_c::getHangDirectionFromAngle);

/* 02427DF0 */
BOOL daPy_lk_c::changeHangMoveProc(int i_direction) {
    WWHD_FUNC(0x02427DF0, BOOL, this, i_direction);
    if (!(mAcch.m_flags & dBgS_Acch::GROUND_HIT) || !dBgS_ChkPolySafe_l(gabi::at<void>(mAcchGndPoly))) {
        return FALSE;
    }
    u32 pla = dBgS_GetTriPla_l(mAcchGndPoly);
    if (pla == 0 || gabi::load<f32>(pla + 4) < 0.9986f) { /* HD: NULL check */
        return FALSE;
    }
    s16 angle;
    if (i_direction == 2) {
        angle = shape_angle.y + 0x4000;
    } else if (i_direction == 3) {
        angle = shape_angle.y - 0x4000;
    } else {
        angle = current.angle.y;
    }
    gabi::Local<cXyz> local_34; /* sp+0x08 */
    gabi::Local<cXyz> local_28; /* sp+0x14 */
    f32 y = current.pos.y + 5.0f;
    local_28->x = current.pos.x;
    local_28->y = y;
    local_28->z = current.pos.z;
    local_34->x = gabi::fmadds(30.0f, cM_ssin(angle), local_28->x);
    local_34->y = y;
    local_34->z = gabi::fmadds(30.0f, cM_scos(angle), local_28->z);
    dBgS_LinChk_Set(mLinkLinChk, local_28, local_34, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        return FALSE;
    }
    mGndChkPos.copy(*local_34.get()); /* mGndChk.SetPos(&local_34) */
    f64 gc = dBgS_GroundCross_l(mGndChk);
    f32 posY = current.pos.y;
    if (std::fabs((f32)(gc - (f64)posY)) > 5.0f) {
        gabi::Local<cXyz> local_40; /* sp+0x2C */
        gabi::Local<cXyz> local_4c; /* sp+0x38 */
        f32 y2 = posY - 5.0f;
        local_40->x = local_34->x;
        local_40->y = y2;
        local_40->z = local_34->z;
        local_4c->x = gabi::fmadds(90.0f, cM_ssin(shape_angle.y), local_34->x);
        local_4c->y = y2;
        local_4c->z = gabi::fmadds(90.0f, cM_scos(shape_angle.y), local_34->z);
        dBgS_LinChk_Set(mLinkLinChk, local_40, local_4c, this);
        if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
            return FALSE;
        }
        u32 tri = dBgS_GetTriPla_l(mLinkLinChkPoly);
        if (tri == 0) { /* HD: NULL check */
            return FALSE;
        }
        if (cLib_distanceAngleS(cM_atan2s(gabi::load<f32>(tri + 0), gabi::load<f32>(tri + 8)), shape_angle.y) < 0x549F) {
            return FALSE;
        }
    }
    gabi::Local<cXyz> local_58; /* sp+0x20 */
    local_58->x = 4.5f * cM_ssin(shape_angle.y);
    local_58->y = 62.5f;
    local_58->z = 4.5f * cM_scos(shape_angle.y);
    PSVECSubtract_l(local_28, local_58, local_28);
    PSVECSubtract_l(local_34, local_58, local_34);
    dBgS_LinChk_Set(mLinkLinChk, local_28, local_34, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        u32 tri = dBgS_GetTriPla_l(mLinkLinChkPoly);
        if (tri == 0) { /* HD: NULL check */
            return FALSE;
        }
        if (cLib_distanceAngleS(cM_atan2s(gabi::load<f32>(tri + 0), gabi::load<f32>(tri + 8)), shape_angle.y) < 0x549F) {
            return FALSE;
        }
    }
    return TRUE;
}
VERIFY(0x02427DF0, &daPy_lk_c::changeHangMoveProc);

/* 02428230 */
int daPy_lk_c::changeHangEndProc(int param_0) {
    WWHD_FUNC(0x02428230, int, this, param_0);
    u32 mtx = gabi::ea(lk_getAnmMtx(mpCLModel, 0 /* CL_JNT_LINK_ROOT_e */));
    s16 a = shape_angle.y;
    /* mGndChk.SetPos(&pos): HD stores the position into the check directly */
    f32 x = gabi::fnmsubs(4.0f, cM_ssin(a), gabi::load<f32>(mtx + 0x0C));
    f32 z = gabi::fnmsubs(4.0f, cM_scos(a), gabi::load<f32>(mtx + 0x2C));
    mGndChkPos.y = gabi::load<f32>(mtx + 0x1C);
    mGndChkPos.x = x;
    mGndChkPos.z = z;
    f64 h = dBgS_GroundCross_l(mGndChk);
    mHangGroundH = (f32)h;
    if (h != -1000000000.0 /* -G_CM3D_F_INF */) {
        m3588 = dBgS_GetAttributeCode_l(gabi::at<void>(mGndChkPoly));
    } else {
        m3588 = 0x1B; /* dBgS_Attr_UNK1B_e */
    }
    if (mHangGroundH > mFootData1_18y + current.pos.y) {
        return gabi::call<int>(LK_procLand_init, this, 1.3f /* HD: HIO folded */, 0);
    }
    if (param_0 != 0) {
        dComIfGp_setDoStatus_l(6 /* dActStts_LET_GO_e */);
        if (LK_doTrigger()) {
            speed.y = 0.0f;
            return gabi::call<int>(LK_procFall_init, this, 1, 6.0f);
        }
    }
    return FALSE;
}
VERIFY(0x02428230, &daPy_lk_c::changeHangEndProc);

/* 024283D4 */
void daPy_lk_c::setHangShapeOffset() {
    WWHD_FUNC(0x024283D4, void, this);
    s16 a = (s16)(m34EC + shape_angle.y);
    f32 sn = cM_ssin(a);
    f32 cs = cM_scos(a);
    f32 y = current.pos.y - 5.0f;
    f32 s50 = 50.0f * sn;
    f32 c50 = 50.0f * cs;
    f32 dVar10 = 20.0f * cs;
    f32 dVar9 = -20.0f * sn;
    gabi::Local<cXyz> local_40; /* sp+0x08 */
    gabi::Local<cXyz> local_4c; /* sp+0x14 */
    gabi::Local<cXyz> local_58; /* sp+0x20 */
    local_40->x = (current.pos.x - s50) + dVar10;
    local_40->y = y;
    local_40->z = (current.pos.z - c50) + dVar9;
    local_4c->x = (current.pos.x + s50) + dVar10;
    local_4c->y = y;
    local_4c->z = (current.pos.z + c50) + dVar9;
    dBgS_LinChk_Set(mLinkLinChk, local_40, local_4c, this);
    if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        m34EC = 0;
        return;
    }
    local_58->copy(mLinkLinChkCross);
    f32 dx = dVar10 + dVar10;
    f32 dz = dVar9 + dVar9;
    local_4c->x = local_4c->x - dx;
    local_4c->z = local_4c->z - dz;
    local_40->z = local_40->z - dz;
    local_40->x = local_40->x - dx;
    dBgS_LinChk_Set(mLinkLinChk, local_40, local_4c, this);
    if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        m34EC = 0;
    } else {
        PSVECSubtract_l(local_58, &mLinkLinChkCross, local_58);
        m34EC = (s16)(cM_atan2s(local_58->x, local_58->z) - 0x4000 - shape_angle.y);
    }
}
VERIFY(0x024283D4, &daPy_lk_c::setHangShapeOffset);

/* 024285AC */
BOOL daPy_lk_c::procHangClimb_init(f32 param_0) {
    WWHD_FUNC(0x024285AC, BOOL, this, param_0);
    if (mAcch.m_flags & dBgS_Acch::ROOF_HIT) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x30 /* daPyProc_HANG_CLIMB_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x4F /* ANM_VJMPCL */, 0.75f, param_0, 0x18, 5.0f); /* HD: HIO folded */
    dComIfGp_onPlayerStatus0_l(0x200 /* daPyStts0_UNK200_e */);
    mNormalSpeed = 0.0f;
    LK_voiceStart(32);
    return TRUE;
}
VERIFY(0x024285AC, &daPy_lk_c::procHangClimb_init);

/* 02428688 */
BOOL daPy_lk_c::procHangWait_init() {
    WWHD_FUNC(0x02428688, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x2E /* daPyProc_HANG_WAIT_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x4D /* ANM_VJMPCHA */, 0.0f, 7.0f /* HD: HIO folded */, -1, 5.0f);
    gabi::call(LK_setTextureAnime, this, 9, 0);
    speed.y = 0.0f;
    mNormalSpeed = 0.0f;
    mpSeAnmFrameCtrl = 0;
    dComIfGp_onPlayerStatus0_l(0x100 /* daPyStts0_HANG_e */);
    return TRUE;
}
VERIFY(0x02428688, &daPy_lk_c::procHangWait_init);

/* 02428730 */
BOOL daPy_lk_c::procHangStart() {
    WWHD_FUNC(0x02428730, BOOL, this);
    if (changeHangEndProc(1)) {
        return TRUE;
    }
    f32 rate = mFrameCtrlUnder[0].getRate();
    if (!(rate < 0.01f) && !(mFrameCtrlUnder[0].getFrame() > 6.0f /* HD: HIO folded */)) {
        return TRUE;
    }
    if (mStickDistance > 0.05f || mProcVar6 != 0) {
        if (getDirectionFromShapeAngle() == 0 /* DIR_FORWARD */ || mProcVar6 != 0) {
            procHangClimb_init(0.0f);
        }
    } else if (rate < 0.01f) {
        procHangWait_init();
    }
    return TRUE;
}
VERIFY(0x02428730, &daPy_lk_c::procHangStart);

/* 02428804 */
BOOL daPy_lk_c::procHangUp_init(int param_0) {
    WWHD_FUNC(0x02428804, BOOL, this, param_0);
    gabi::call(LK_commonProcInit, this, 0x2D /* daPyProc_HANG_UP_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x51 /* ANM_HANGUP */, 0.7f, 1.0f, 0xD, 8.0f); /* HD: HIO folded */
    dComIfGp_onPlayerStatus0_l(0x100 /* daPyStts0_HANG_e */);
    mProcVar6 = param_0;
    mNormalSpeed = 0.0f;
    return TRUE;
}
VERIFY(0x02428804, &daPy_lk_c::procHangUp_init);

/* 0242888C */
BOOL daPy_lk_c::procHangFallStart() {
    WWHD_FUNC(0x0242888C, BOOL, this);
    int iVar2;
    if (mFrameCtrlUnder[0].getRate() < 0.01f || mFrameCtrlUnder[0].getFrame() > 10.0f /* HD: HIO folded */) {
        iVar2 = 1;
    } else {
        iVar2 = 0;
    }
    if (changeHangEndProc(iVar2)) {
        return TRUE;
    }
    if (iVar2 != 0) {
        u32 flg = mModeFlg;
        if (flg & 0x400) {
            flg = (flg & ~0x400u) | 0x100;
            mModeFlg = flg;
        }
        if (flg & 0x100) {
            gabi::call(LK_setTextureAnime, this, 7, 0);
        }
        int dir = getHangDirectionFromAngle();
        if (mStickDistance > 0.05f && dir != 1) {
            procHangUp_init(dir);
        }
    }
    return TRUE;
}
VERIFY(0x0242888C, &daPy_lk_c::procHangFallStart);

/* 02428988 */
BOOL daPy_lk_c::procHangMove_init(int param_0) {
    WWHD_FUNC(0x02428988, BOOL, this, param_0);
    gabi::call(LK_commonProcInit, this, 0x2F /* daPyProc_HANG_MOVE_e */);
    int anm;
    if (param_0 == 2) {
        anm = 0x52; /* ANM_HANGMOVEL */
        current.angle.y = shape_angle.y + 0x4000;
    } else {
        anm = 0x53; /* ANM_HANGMOVER */
        current.angle.y = shape_angle.y - 0x4000;
    }
    f64 spd = gabi::call<f64>(0x02427D8C /* getHangMoveAnmSpeed */, this); /* f1 passed on as returned */
    gabi::call(LK_setSingleMoveAnime, this, anm, spd, 0.0f, -1, 1.0f);
    dComIfGp_onPlayerStatus0_l(0x100 /* daPyStts0_HANG_e */);
    mNormalSpeed = 0.0f;
    gabi::Local<cXyz> xz;
    gabi::Local<cXyz> d;
    cXyz_mi(&mLeftHandPos, d, &mRightHandPos);
    xz->z = d->z;
    xz->y = 0.0f;
    xz->x = d->x;
    m35A0 = std_sqrtf(PSVECSquareMag(xz)); /* absXZ() */
    return TRUE;
}
VERIFY(0x02428988, &daPy_lk_c::procHangMove_init);

/* 02428AEC */
BOOL daPy_lk_c::procHangUp() {
    WWHD_FUNC(0x02428AEC, BOOL, this);
    if (changeHangEndProc(1)) {
        if (mCurProc == 0x27 /* daPyProc_FALL_e */) {
            current.pos.x = gabi::fnmsubs(8.0f, cM_ssin(shape_angle.y), current.pos.x);
            current.pos.z = gabi::fnmsubs(8.0f, cM_scos(shape_angle.y), current.pos.z);
        }
        return TRUE;
    }
    if (mFrameCtrlUnder[0].getRate() < 0.01f || mFrameCtrlUnder[0].getFrame() > 13.0f /* HD: HIO folded */) {
        if (mProcVar6 == 0) {
            procHangClimb_init(0.0f);
        } else if (changeHangMoveProc(mProcVar6)) {
            procHangMove_init(mProcVar6);
        } else {
            procHangWait_init();
        }
    }
    return TRUE;
}
VERIFY(0x02428AEC, &daPy_lk_c::procHangUp);

/* 02428BF4 */
BOOL daPy_lk_c::procHangWait() {
    WWHD_FUNC(0x02428BF4, BOOL, this);
    if (changeHangEndProc(1)) {
        return TRUE;
    }
    setHangShapeOffset();
    if (mStickDistance > 0.05f) {
        int dir = getHangDirectionFromAngle();
        if (dir == 0) {
            procHangClimb_init(0.0f);
        } else if (dir != 1 && changeHangMoveProc(dir)) {
            s16 keep = m34EC;
            procHangMove_init(dir);
            m34EC = keep;
        }
    }
    return TRUE;
}
VERIFY(0x02428BF4, &daPy_lk_c::procHangWait);

/* 02428CB4 */
BOOL daPy_lk_c::procHangMove() {
    WWHD_FUNC(0x02428CB4, BOOL, this);
    int dir = getHangDirectionFromAngle();
    if (changeHangEndProc(1)) {
        return TRUE;
    }
    if (dir == 0 && mStickDistance > 0.05f) {
        procHangClimb_init(0.0f);
        return TRUE;
    }
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    frameCtrl.setRate(getHangMoveAnmSpeed());
    if (frameCtrl.checkPass(0.0f)) {
        f32 stick = mStickDistance;
        s16 keep = m34EC;
        if (stick > 0.05f && dir != 1 && changeHangMoveProc(dir)) {
            procHangMove_init(dir);
        } else {
            procHangWait_init();
        }
        m34EC = keep;
    } else {
        gabi::Local<cXyz> xz;    /* sp+0x08 */
        gabi::Local<cXyz> local_64; /* sp+0x2C */
        cXyz_mi(&mRightHandPos, local_64, &mLeftHandPos);
        xz->x = local_64->x;
        xz->y = 0.0f;
        xz->z = local_64->z;
        f32 dVar12 = std_sqrtf(PSVECSquareMag(xz)); /* absXZ() */
        if (changeHangMoveProc(4)) {
            f32 spd = std::fabs(dVar12 - m35A0); /* HD: HIO field_0x30 (1.0) folded */
            mNormalSpeed = spd;
            f32 dVar13 = cM_ssin(current.angle.y);
            f32 fVar1 = cM_ssin(shape_angle.y);
            f32 dVar11 = cM_scos(current.angle.y);
            f32 fVar2 = cM_scos(shape_angle.y);
            gabi::Local<cXyz> local_70; /* sp+0x14 */
            gabi::Local<cXyz> local_7c; /* sp+0x20 */
            f32 x70 = gabi::fnmsubs(30.0f, fVar1, gabi::fmadds(spd, dVar13, current.pos.x));
            f32 z70 = gabi::fnmsubs(30.0f, fVar2, gabi::fmadds(spd, dVar11, current.pos.z));
            f32 y70 = current.pos.y - 5.0f;
            local_70->x = x70;
            local_70->y = y70;
            local_70->z = z70;
            local_7c->x = gabi::fmadds(60.0f, fVar1, x70);
            local_7c->y = y70;
            local_7c->z = gabi::fmadds(60.0f, fVar2, z70);
            dBgS_LinChk_Set(mLinkLinChk, local_70, local_7c, this);
            u32 tri;
            if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk) || (tri = dBgS_GetTriPla_l(mLinkLinChkPoly)) == 0) { /* HD: NULL check */
                mNormalSpeed = 0.0f;
                m35A0 = dVar12;
            } else {
                s16 sVar6 = cM_atan2s(gabi::load<f32>(tri + 0), gabi::load<f32>(tri + 8)) + 0x8000;
                s16 shapeY = shape_angle.y;
                if (sVar6 != shapeY) {
                    s16 newAngle = (s16)(current.angle.y - shapeY) > 0 ? (s16)(sVar6 + 0x4000) : (s16)(sVar6 - 0x4000);
                    shape_angle.y = sVar6;
                    current.angle.y = newAngle;
                    m34EC = m34EC + (s16)(shapeY - sVar6);
                    /* local_88 = cross - np * 1.5 (+25 in y): HD stores it into mGndChk directly */
                    f32 x = gabi::fnmsubs(1.5f, gabi::load<f32>(tri + 0), mLinkLinChkCross.x);
                    f32 y = mLinkLinChkCross.y + 25.0f;
                    f32 z = gabi::fnmsubs(1.5f, gabi::load<f32>(tri + 8), mLinkLinChkCross.z);
                    mGndChkPos.x = x;
                    mGndChkPos.y = y;
                    mGndChkPos.z = z;
                    f64 gc = dBgS_GroundCross_l(mGndChk);
                    if (!(std::fabs((f32)(gc - (f64)(f32)current.pos.y)) > 5.0f)) {
                        m35A0 = dVar12;
                        current.pos.x = x;
                        current.pos.z = z;
                        current.pos.y = (f32)gc;
                    } else {
                        m35A0 = dVar12;
                    }
                } else {
                    f32 x = gabi::fnmsubs(1.5f, gabi::load<f32>(tri + 0), mLinkLinChkCross.x);
                    f32 spd2 = mNormalSpeed;
                    current.pos.x = gabi::fnmsubs(spd2, dVar13, x);
                    f32 z = gabi::fnmsubs(1.5f, gabi::load<f32>(tri + 8), mLinkLinChkCross.z);
                    m35A0 = dVar12;
                    current.pos.z = gabi::fnmsubs(spd2, dVar11, z);
                }
            }
        } else {
            mNormalSpeed = 0.0f;
            m35A0 = dVar12;
        }
    }
    if (mModeFlg & 0x20 /* ModeFlg_HANG */) {
        setHangShapeOffset();
    }
    return TRUE;
}
VERIFY(0x02428CB4, &daPy_lk_c::procHangMove);

/* 02429100 */
BOOL daPy_lk_c::procHangClimb() {
    WWHD_FUNC(0x02429100, BOOL, this);
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        gabi::call(LK_checkNextMode, this, 0);
    } else if (mFrameCtrlUnder[0].getFrame() > 23.0f /* HD: HIO folded */) {
        gabi::call(LK_checkNextMode, this, 1);
    }
    return TRUE;
}
VERIFY(0x02429100, &daPy_lk_c::procHangClimb);

/* 02429160 */
BOOL daPy_lk_c::procHangWallCatch() {
    WWHD_FUNC(0x02429160, BOOL, this);
    if (mFrameCtrlUnder[0].getRate() < 0.01f && !procHangClimb_init(2.0f /* HD: HIO folded */)) {
        return gabi::call<BOOL>(LK_procFall_init, this, 1, 6.0f);
    }
    return TRUE;
}
VERIFY(0x02429160, &daPy_lk_c::procHangWallCatch);

/* ---- d_a_player_climb.inc ---- */

/* 024291E0 */
f32 daPy_lk_c::getClimbMoveAnmSpeed() {
    WWHD_FUNC(0x024291E0, f32, this);
    /* getAnmSpeedStickRate(0.8f, 1.25f) inlined, HIO folded */
    return gabi::fmadds(0.45f, mStickDistance, 0.8f);
}
VERIFY(0x024291E0, &daPy_lk_c::getClimbMoveAnmSpeed);

/* 024291FC */
int daPy_lk_c::getClimbDirectionFromAngle() {
    WWHD_FUNC(0x024291FC, int, this);
    s32 d = (s16)(m34E8 - shape_angle.y);
    if ((d < 0 ? -d : d) > 0x7000) {
        return 1; /* DIR_BACKWARD */
    } else if (d >= 0x1000) {
        return 2; /* DIR_LEFT */
    } else if (d > -0x1000) {
        return 0; /* DIR_FORWARD */
    }
    return 3; /* DIR_RIGHT */
}
VERIFY(0x024291FC, &daPy_lk_c::getClimbDirectionFromAngle);

/* 0242924C */
BOOL daPy_lk_c::procClimbMoveSide_init(int param_0) {
    WWHD_FUNC(0x0242924C, BOOL, this, param_0);
    f64 spd = gabi::call<f64>(0x024291E0 /* getClimbMoveAnmSpeed */, this); /* f1 passed on as returned */
    gabi::call(LK_commonProcInit, this, 0x40 /* daPyProc_CLIMB_MOVE_SIDE_e */);
    m34C2 = 7;
    mProcVar6 = param_0;
    mProcVar2 = (s16)(m34E8 - shape_angle.y);
    int anm;
    if (param_0 != 0) {
        anm = 0x8C; /* ANM_FCLIMBSLIDELUP */
        if (mDirection == 3 /* DIR_RIGHT */) {
            spd = -spd;
        }
    } else {
        anm = 0x8D; /* ANM_FCLIMBSLIDERUP */
        if (mDirection == 2 /* DIR_LEFT */) {
            spd = -spd;
        }
    }
    gabi::call(LK_setSingleMoveAnime, this, anm, spd, 0.0f, 0xD, 0.0f); /* HD: HIO folded */
    gravity = 0.0f;
    speedF = 0.0f;
    mProcVar0 = 1;
    mNormalSpeed = 0.0f;
    speed.y = 0.0f;
    dComIfGp_onPlayerStatus1_l(0x10000 /* daPyStts1_UNK10000_e */);
    gabi::Local<cXyz> xz;
    gabi::Local<cXyz> d;
    cXyz_mi(&mRightHandPos, d, &mLeftHandPos);
    xz->x = d->x;
    xz->z = d->z;
    xz->y = 0.0f;
    m35A0 = std_sqrtf(PSVECSquareMag(xz)); /* absXZ() */
    return TRUE;
}
VERIFY(0x0242924C, &daPy_lk_c::procClimbMoveSide_init);

/* 02429388 */
void daPy_lk_c::changeClimbMoveProc(int param_0) {
    WWHD_FUNC(0x02429388, void, this, param_0);
    f32 stick = mStickDistance;
    s16 keep = m34EC;
    if (stick > 0.05f) {
        u8 dir = (u8)getClimbDirectionFromAngle();
        mDirection = dir;
        if (dir == 0 /* DIR_FORWARD */ || dir == 1 /* DIR_BACKWARD */) {
            gabi::call(LK_procClimbMoveUpDown_init, this, param_0);
        } else {
            procClimbMoveSide_init(param_0);
        }
    }
    m34EC = keep;
}
VERIFY(0x02429388, &daPy_lk_c::changeClimbMoveProc);

/* 02429408 */
BOOL daPy_lk_c::setMoveBGCorrectClimb() {
    WWHD_FUNC(0x02429408, BOOL, this);
    if (dBgS_ChkPolySafe_l(mPolyInfo) && dBgS_ChkMoveBG_l(mPolyInfo)) {
        dBgS_MoveBgTransPos_l(mPolyInfo, true, &current.pos, &current.angle, &shape_angle);
    }
    if (noResetFlg1() & 0x2000000 /* daPyFlg1_VINE_CATCH */) {
        return gabi::call<BOOL>(LK_procFall_init, this, 1, 6.0f);
    }
    if (mCurProc == 0x3E /* daPyProc_CLIMB_DOWN_START_e */ && mFrameCtrlUnder[0].getRate() > 0.01f) {
        return FALSE;
    }
    gabi::Local<cXyz> local_1c; /* sp+0x08 */
    gabi::Local<cXyz> local_28; /* sp+0x14 */
    f32 y = current.pos.y + 30.0f;
    local_1c->x = current.pos.x;
    local_1c->y = y;
    local_1c->z = current.pos.z;
    local_28->x = gabi::fmadds(80.0f, cM_ssin(shape_angle.y), current.pos.x);
    local_28->y = y;
    local_28->z = gabi::fmadds(80.0f, cM_scos(shape_angle.y), current.pos.z);
    dBgS_LinChk_Set(mLinkLinChk, local_1c, local_28, this);
    if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk) || dBgS_GetWallCode_l(gabi::at<void>(mLinkLinChkPoly)) != 1) {
        return gabi::call<BOOL>(LK_procFall_init, this, 1, 6.0f);
    }
    u32 tri = dBgS_GetTriPla_l(mLinkLinChkPoly);
    if (tri != 0 && std::fabs(gabi::load<f32>(tri + 4)) > 0.05f) { /* HD: NULL check */
        return gabi::call<BOOL>(LK_procFall_init, this, 1, 6.0f);
    }
    if ((mCurProc == 0x3F /* daPyProc_CLIMB_MOVE_UP_DOWN_e */ || mCurProc == 0x40 /* daPyProc_CLIMB_MOVE_SIDE_e */) &&
        current.pos.y - 15.0f < mAcch.m_ground_h) {
        return gabi::call<BOOL>(LK_procFall_init, this, 1, 6.0f);
    }
    tri = dBgS_GetTriPla_l(mLinkLinChkPoly);
    if (tri == 0) { /* HD: NULL check */
        return FALSE;
    }
    s16 uVar6 = cM_atan2s(gabi::load<f32>(tri + 0), gabi::load<f32>(tri + 8));
    if (cLib_distanceAngleS(uVar6, shape_angle.y) <= 0x549F) {
        return gabi::call<BOOL>(LK_procFall_init, this, 1, 6.0f);
    }
    current.pos.x = gabi::fmadds(20.5f, cM_ssin(uVar6), mLinkLinChkCross.x);
    s16 sVar1 = shape_angle.y;
    s16 newShape = uVar6 + 0x8000;
    shape_angle.y = newShape;
    current.angle.y = current.angle.y + (s16)(newShape - sVar1);
    current.pos.z = gabi::fmadds(20.5f, cM_scos(uVar6), mLinkLinChkCross.z);
    m34EC = m34EC + (s16)(sVar1 - newShape);
    return FALSE;
}
VERIFY(0x02429408, &daPy_lk_c::setMoveBGCorrectClimb);

/* 024296A8 */
void daPy_lk_c::checkBgCorrectClimbMove(cXyz* i_startPos, cXyz* i_endPos) {
    WWHD_FUNC(0x024296A8, void, this, i_startPos, i_endPos);
    dBgS_LinChk_Set(mLinkLinChk, i_startPos, i_endPos, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        u32 tri = dBgS_GetTriPla_l(mLinkLinChkPoly);
        if (tri != 0) { /* HD: NULL check */
            if (cLib_distanceAngleS(cM_atan2s(gabi::load<f32>(tri + 0), gabi::load<f32>(tri + 8)), shape_angle.y) < 0x549F) {
                gabi::Local<cXyz> d;
                cXyz_mi(&mLinkLinChkCross, d, i_endPos);
                PSVECAdd_l(&current.pos, d, &current.pos); /* current.pos += cross - *i_endPos */
            }
        }
    }
}
VERIFY(0x024296A8, &daPy_lk_c::checkBgCorrectClimbMove);

/* 02429758 */
void daPy_lk_c::setClimbShapeOffset() {
    WWHD_FUNC(0x02429758, void, this);
    s16 shapeY = shape_angle.y;
    s16 a = (s16)(m34EC + shapeY);
    f32 fVar4 = cM_ssin(a);
    f32 fVar3 = cM_scos(a);
    f32 x58 = gabi::fmadds(20.5f, cM_ssin(shapeY), current.pos.x);
    f32 y58 = current.pos.y - 62.5f;
    f32 z58 = gabi::fmadds(20.5f, cM_scos(shapeY), current.pos.z);
    f32 s50 = 50.0f * fVar4;
    f32 c50 = 50.0f * fVar3;
    f32 dVar10 = 20.0f * fVar3;
    f32 dVar9 = -20.0f * fVar4;
    gabi::Local<cXyz> local_40; /* sp+0x08 */
    gabi::Local<cXyz> local_4c; /* sp+0x14 */
    gabi::Local<cXyz> local_64; /* sp+0x20 */
    local_40->x = (x58 - s50) + dVar10;
    local_40->y = y58;
    local_40->z = (z58 - c50) + dVar9;
    local_4c->x = (x58 + s50) + dVar10;
    local_4c->y = y58;
    local_4c->z = (z58 + c50) + dVar9;
    dBgS_LinChk_Set(mLinkLinChk, local_40, local_4c, this);
    if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        m34EC = 0;
        return;
    }
    local_64->copy(mLinkLinChkCross);
    f32 dx = dVar10 + dVar10;
    f32 dz = dVar9 + dVar9;
    local_4c->x = local_4c->x - dx;
    local_4c->z = local_4c->z - dz;
    local_40->z = local_40->z - dz;
    local_40->x = local_40->x - dx;
    dBgS_LinChk_Set(mLinkLinChk, local_40, local_4c, this);
    if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        m34EC = 0;
    } else {
        PSVECSubtract_l(local_64, &mLinkLinChkCross, local_64);
        m34EC = (s16)(cM_atan2s(local_64->x, local_64->z) - 0x4000 - shape_angle.y);
    }
}
VERIFY(0x02429758, &daPy_lk_c::setClimbShapeOffset);

/* 02429950 */
void daPy_lk_c::checkBgClimbMove() {
    WWHD_FUNC(0x02429950, void, this);
    f32 dVar7 = cM_ssin(shape_angle.y);
    f32 dVar6 = cM_scos(shape_angle.y);
    f32 dVar4 = -30.0f * dVar7;
    f32 dVar5 = 30.0f * dVar6;
    gabi::Local<cXyz> local_5c; /* sp+0x08 */
    gabi::Local<cXyz> local_68; /* sp+0x14 */
    local_5c->z = current.pos.z + dVar4;
    local_5c->y = current.pos.y;
    local_5c->x = current.pos.x + dVar5;
    checkBgCorrectClimbMove(&current.pos, local_5c);
    local_5c->y = current.pos.y;
    local_5c->x = current.pos.x - dVar5;
    local_5c->z = current.pos.z - dVar4;
    checkBgCorrectClimbMove(&current.pos, local_5c);
    f32 z = current.pos.z;
    f32 y = current.pos.y + 125.0f;
    f32 x = current.pos.x;
    local_68->z = z;
    local_68->y = y;
    local_5c->y = y;
    local_5c->x = x + dVar5;
    local_5c->z = z + dVar4;
    local_68->x = x;
    checkBgCorrectClimbMove(local_68, local_5c);
    local_5c->y = local_68->y;
    local_5c->x = local_68->x - dVar5;
    local_5c->z = local_68->z - dVar4;
    checkBgCorrectClimbMove(local_68, local_5c);

    /* mRoofChk.SetPos(local_5c): HD stores the position into the check as well */
    f32 py = current.pos.y;
    mRoofChkPos.y = py;
    f32 px = current.pos.x + dVar5;
    local_5c->y = py;
    f32 pz = current.pos.z + dVar4;
    local_5c->x = px;
    mRoofChkPos.x = px;
    mRoofChkPos.z = pz;
    local_5c->z = pz;
    f32 roof = (f32)(dBgS_RoofChk_l(mRoofChk) - 125.0);
    py = current.pos.y;
    f32 px0 = current.pos.x;
    if (roof < py) {
        py = roof;
        current.pos.y = roof;
    }
    px = px0 - dVar5;
    pz = current.pos.z - dVar4;
    mRoofChkPos.y = py;
    mRoofChkPos.x = px;
    local_5c->y = py;
    local_5c->x = px;
    mRoofChkPos.z = pz;
    local_5c->z = pz;
    roof = (f32)(dBgS_RoofChk_l(mRoofChk) - 125.0);
    if (roof < current.pos.y) {
        current.pos.y = roof;
    }

    local_5c->y = local_68->y;
    local_5c->x = gabi::fmadds(100.0f, dVar7, local_68->x);
    local_5c->z = gabi::fmadds(100.0f, dVar6, local_68->z);
    dBgS_LinChk_Set(mLinkLinChk, local_68, local_5c, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        setClimbShapeOffset();
        return;
    }
    f32 gx = gabi::fmadds(25.0f, dVar7, current.pos.x);
    f32 gy = local_68->y + 30.0f;
    f32 gz = gabi::fmadds(25.0f, dVar6, current.pos.z);
    local_5c->x = gx;
    local_5c->y = gy;
    mGndChkPos.y = gy;
    mGndChkPos.z = gz;
    mGndChkPos.x = gx;
    local_5c->z = gz;
    f64 gc = dBgS_GroundCross_l(mGndChk);
    if (gc != -1000000000.0 && !(gc < (f64)(f32)(local_68->y - 30.0f))) {
        u32 tri = dBgS_GetTriPla_l(mGndChkPoly);
        if (tri != 0 && !(gabi::load<f32>(tri + 4) < 0.5f)) { /* cBgW_CheckBGround (HD: NULL check) */
            current.pos.y = (f32)gc;
            current.pos.x = gabi::fnmsubs(3.0f, dVar7, local_5c->x);
            current.pos.z = gabi::fnmsubs(3.0f, dVar6, local_5c->z);
            procHangClimb_init(0.0f);
            return;
        }
    }
    gabi::call(LK_procFall_init, this, 1, 6.0f);
}
VERIFY(0x02429950, &daPy_lk_c::checkBgClimbMove);

/* 02429D8C */
BOOL daPy_lk_c::procClimbUpStart() {
    WWHD_FUNC(0x02429D8C, BOOL, this);
    if (setMoveBGCorrectClimb()) {
        return TRUE;
    }
    if (mProcVar6 == 0) {
        if (LK_checkNoUpperAnime()) {
            gabi::call(LK_procLadderUpStart_init_sub, this);
        }
    } else {
        J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
        m34C2 = 5;
        if (frameCtrl.getRate() < 0.01f) {
            dComIfGp_setDoStatus_l(6 /* dActStts_LET_GO_e */);
            if (mProcVar2 == 0) {
                setLadderFootSe();
                mProcVar2 = 1;
            }
            if (LK_doTrigger()) {
                m34C2 = 0;
                gabi::call(LK_procFall_init, this, 1, 6.0f);
            } else {
                changeClimbMoveProc(1);
            }
        } else if (frameCtrl.checkPass(9.0f)) {
            setLadderFootSe();
        }
    }
    return TRUE;
}
VERIFY(0x02429D8C, &daPy_lk_c::procClimbUpStart);

/* 02429E8C */
BOOL daPy_lk_c::procClimbDownStart() {
    WWHD_FUNC(0x02429E8C, BOOL, this);
    if (setMoveBGCorrectClimb()) {
        return TRUE;
    }
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    m34C2 = 5;
    if (frameCtrl.getRate() < 0.01f) {
        dComIfGp_setDoStatus_l(6 /* dActStts_LET_GO_e */);
        if (LK_doTrigger()) {
            m34C2 = 0;
            gabi::call(LK_procFall_init, this, 1, 6.0f);
        } else {
            changeClimbMoveProc(1);
        }
    } else if (frameCtrl.checkPass(26.0f) || frameCtrl.checkPass(36.0f) || frameCtrl.checkPass(43.0f)) {
        setLadderFootSe();
    }
    return TRUE;
}
VERIFY(0x02429E8C, &daPy_lk_c::procClimbDownStart);

/* 02429F84 */
BOOL daPy_lk_c::procClimbMoveUpDown() {
    WWHD_FUNC(0x02429F84, BOOL, this);
    if (setMoveBGCorrectClimb()) {
        return TRUE;
    }
    m34C2 = 5;
    dComIfGp_setDoStatus_l(6 /* dActStts_LET_GO_e */);
    if (LK_doTrigger()) {
        m34C2 = 0;
        gabi::call(LK_procFall_init, this, 1, 6.0f);
    } else {
        f32 rate = mFrameCtrlUnder[0].getRate();
        if (std::fabs(rate) < 0.01f) {
            if (mProcVar0 != 0) {
                mProcVar0 = 0;
                setLadderFootSe();
            } else {
                changeClimbMoveProc(mProcVar6);
            }
        } else {
            f32 spd = gabi::call<f32>(LK_getLadderMoveAnmSpeed, this);
            if (rate < 0.0f) { /* HD: the rate read before the call */
                spd = -spd;
            }
            mFrameCtrlUnder[0].setRate(spd);
            checkBgClimbMove();
        }
    }
    return TRUE;
}
VERIFY(0x02429F84, &daPy_lk_c::procClimbMoveUpDown);

/* 0242A06C */
BOOL daPy_lk_c::procClimbMoveSide() {
    WWHD_FUNC(0x0242A06C, BOOL, this);
    if (setMoveBGCorrectClimb()) {
        return TRUE;
    }
    m34C2 = 5;
    dComIfGp_setDoStatus_l(6 /* dActStts_LET_GO_e */);
    if (LK_doTrigger()) {
        m34C2 = 0;
        gabi::call(LK_procFall_init, this, 1, 6.0f);
    } else {
        f32 rate = mFrameCtrlUnder[0].getRate();
        if (std::fabs(rate) < 0.01f) {
            if (mProcVar0 != 0) {
                mProcVar0 = 0;
                setLadderFootSe();
            } else {
                changeClimbMoveProc(mProcVar6);
            }
        } else {
            f32 spd = getClimbMoveAnmSpeed();
            if (rate < 0.0f) { /* HD: the rate read before the call */
                spd = -spd;
            }
            mFrameCtrlUnder[0].setRate(spd);
            gabi::Local<cXyz> xz;
            gabi::Local<cXyz> d;
            cXyz_mi(&mRightHandPos, d, &mLeftHandPos);
            xz->x = d->x;
            xz->y = 0.0f;
            xz->z = d->z;
            f32 dVar5 = std_sqrtf(PSVECSquareMag(xz)); /* absXZ() */
            f32 fVar1 = std::fabs(dVar5 - m35A0) * 0.8f; /* HD: HIO folded */
            f32 fVar2 = fVar1 * cM_ssin(mProcVar2);
            current.pos.y = gabi::fmadds(fVar1, cM_scos(mProcVar2), current.pos.y);
            current.pos.x = gabi::fmadds(fVar2, cM_scos(shape_angle.y), current.pos.x);
            f32 z = gabi::fnmsubs(fVar2, cM_ssin(shape_angle.y), current.pos.z);
            m35A0 = dVar5;
            current.pos.z = z;
            checkBgClimbMove();
        }
    }
    return TRUE;
}
VERIFY(0x0242A06C, &daPy_lk_c::procClimbMoveSide);

/* ---- d_a_player_whide.inc ---- */

/* 0242A218 */
void daPy_lk_c::setBlendWHideMoveAnime(f32 param_0) {
    WWHD_FUNC(0x0242A218, void, this, param_0);
    /* HD: HIO folded (the two postures differ in the animation speed, the end ratio and the animations) */
    f32 fVar1, fVar6;
    int dVar5, dVar6;
    if (!(mNoResetFlg0 & 0x10000 /* daPyFlg0_UNK10000 */)) {
        fVar1 = 1.6f;
        dVar6 = 0x42; /* ANM_WALL */
        fVar6 = 1.0f;
        dVar5 = mDirection == 2 /* DIR_LEFT */ ? 0x45 /* ANM_WALLWR */ : 0x44 /* ANM_WALLWL */;
    } else {
        fVar1 = 1.8f;
        dVar6 = 0x43; /* ANM_WALLDW */
        fVar6 = 0.98f;
        dVar5 = mDirection == 2 ? 0x47 /* ANM_WALLWRDW */ : 0x46 /* ANM_WALLWLDW */;
    }
    f32 fVar7 = mNormalSpeed / mMaxNormalSpeed;
    f32 fVar5 = 5.0f * param_0;
    const f32 fVar3 = 0.9f;
    f32 fVar4;
    if (fVar7 < 0.0f) {
        fVar4 = 1.0f - 0.0f / fVar3;
        m3598 = 1.0f;
    } else if (fVar7 < fVar3) {
        fVar4 = 1.0f - fVar7 / fVar3;
        m3598 = 1.0f;
    } else {
        fVar4 = 0.0f;
        m3598 = 1.0f - ((1.0f - fVar6) * (fVar7 - fVar3)) / (1.0f - fVar3); /* GHS folds (1.0f - fVar3) */
    }
    gabi::call(LK_setMoveAnime, this, fVar4, fVar1, 0.0f, dVar5, dVar6, 5, fVar5);
    s16 end1 = mFrameCtrlUnder[1].mEnd;
    mFrameCtrlUnder[0].mEnd = 0x1C; /* HD: HIO folded */
    mFrameCtrlUnder[1].mRate = 0.0f;
    f32 frame = (f32)(s32)end1 - 0.001f;
    mFrameCtrlUnder[1].mFrame = frame;
    gabi::store<f32>(LK_FIELD(u32, 0x580C), frame); /* mAnmRatioUnder[UNDER_MOVE1_e].getAnmTransform()->setFrame() */
}
VERIFY(0x0242A218, &daPy_lk_c::setBlendWHideMoveAnime);

/* 0242A478 */
void daPy_lk_c::getWHideBasePos(cXyz* param_0) {
    WWHD_FUNC(0x0242A478, void, this, param_0);
    f32 h = (mNoResetFlg0 & 0x10000) ? 89.9f : 125.0f;
    f32 y = current.pos.y + h;
    param_0->x = current.pos.x;
    param_0->z = current.pos.z;
    param_0->y = y;
}
VERIFY(0x0242A478, &daPy_lk_c::getWHideBasePos);

/* 0242A4CC */
void daPy_lk_c::getWHideNextPos(cXyz* param_0, cXyz* param_1) {
    WWHD_FUNC(0x0242A4CC, void, this, param_0, param_1);
    s16 a = current.angle.y;
    f32 x = gabi::fmadds(25.0f, cM_ssin(a), param_0->x); /* HD: HIO field_0x54 folded */
    f32 z = gabi::fmadds(25.0f, cM_scos(a), param_0->z);
    f32 y = param_0->y;
    param_1->x = x;
    param_1->y = y;
    param_1->z = z;
}
VERIFY(0x0242A4CC, &daPy_lk_c::getWHideNextPos);

/* 0242A514 */
BOOL daPy_lk_c::checkWHideBackWall(cXyz* param_0) {
    WWHD_FUNC(0x0242A514, BOOL, this, param_0);
    if (param_0 == nullptr) {
        return TRUE;
    }
    gabi::Local<cXyz> end;
    end->x = gabi::fnmsubs(25.0f, cM_ssin(shape_angle.y), param_0->x);
    end->y = param_0->y;
    end->z = gabi::fnmsubs(25.0f, cM_scos(shape_angle.y), param_0->z);
    dBgS_LinChk_Set(mLinkLinChk, param_0, end, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x0242A514, &daPy_lk_c::checkWHideBackWall);

/* 0242A5D0 */
BOOL daPy_lk_c::checkWHideFrontFloor(cXyz* param_0) {
    WWHD_FUNC(0x0242A5D0, BOOL, this, param_0);
    /* mGndChk.SetPos(&local_10): HD stores the position into the check directly */
    f32 x = gabi::fmadds(40.0f, cM_ssin(shape_angle.y), param_0->x);
    f32 z = gabi::fmadds(40.0f, cM_scos(shape_angle.y), param_0->z);
    mGndChkPos.y = param_0->y;
    mGndChkPos.x = x;
    mGndChkPos.z = z;
    f64 g = dBgS_GroundCross_l(mGndChk);
    f32 y = current.pos.y;
    if ((mNoResetFlg0 & 0x80000000 /* daPyFlg0_UNK80000000 */) && y > g) {
        g = y;
    }
    return std::fabs((f32)(g - (f64)y)) < 5.0f;
}
VERIFY(0x0242A5D0, &daPy_lk_c::checkWHideFrontFloor);

/* 0242A6B4 */
int daPy_lk_c::checkWHideModeChange(cXyz* param_0) {
    WWHD_FUNC(0x0242A6B4, int, this, param_0);
    if (!(mNoResetFlg0 & 0x10000)) {
        if (!checkWHideFrontFloor(param_0)) {
            dComIfGp_setRStatus_l(0 /* dActStts_BLANK_e */);
        } else if (LK_spActionButton()) {
            return 1;
        }
    } else {
        gabi::Local<cXyz> local_18;
        local_18->x = param_0->x;
        local_18->y = current.pos.y + 125.0f;
        local_18->z = param_0->z;
        if (!LK_spActionButton() && !(mAcch.m_flags & dBgS_Acch::ROOF_HIT) && checkWHideBackWall(local_18)) {
            return -1;
        }
    }
    if ((mModeFlg & 1) && mCurProc != 0x16 /* daPyProc_WHIDE_PEEP_e */) {
        gabi::call(LK_setTextureAnime, this, 0xF /* mAnmDataTable[ANM_WALL].mTexAnmIdx */, 0);
    }
    return 0;
}
VERIFY(0x0242A6B4, &daPy_lk_c::checkWHideModeChange);

/* 0242A7CC */
int daPy_lk_c::changeWHideEndProc(cXyz* param_0) {
    WWHD_FUNC(0x0242A7CC, int, this, param_0);
    dComIfGp_setDoStatus_l(0x10 /* dActStts_SIDLE_e */);
    dComIfGp_setRStatus_l(0xF /* dActStts_CROUCH_e */);
    if ((!LK_doButton() && (!(mNoResetFlg0 & 0x10000) || !(mAcch.m_flags & dBgS_Acch::ROOF_HIT))) ||
        (gabi::load<u16>(gabi::ea(this) + 0x5888) != 0xD7 /* !checkUpperAnime(REST) */ && LK_swordTrigger()) ||
        !checkWHideBackWall(param_0)) {
        mNormalSpeed = mMaxNormalSpeed * 0.7f;
        current.angle.y = shape_angle.y;
        return gabi::call<int>(LK_checkNextMode, this, 0);
    }
    if ((mNoResetFlg0 & 0x10000) && (mAcch.m_flags & dBgS_Acch::ROOF_HIT)) {
        mModeFlg = mModeFlg | 0x4000000;
    } else {
        mModeFlg = mModeFlg & ~0x4000000u;
    }
    return FALSE;
}
VERIFY(0x0242A7CC, &daPy_lk_c::changeWHideEndProc);

/* 0242A8E4 */
BOOL daPy_lk_c::procWHideWait_init() {
    WWHD_FUNC(0x0242A8E4, BOOL, this);
    int anm;
    s16 end;
    if (!(mNoResetFlg0 & 0x10000)) {
        anm = 0x42; /* ANM_WALL */
        end = 0xA;  /* HD: HIO folded */
    } else {
        anm = 0x43; /* ANM_WALLDW */
        end = 7;
    }
    f32 start;
    if (mCurProc == 0x13 /* daPyProc_WHIDE_READY_e */) {
        start = 3.0f;
    } else {
        start = (f32)(s32)end - 0.001f;
    }
    gabi::call(LK_commonProcInit, this, 0x14 /* daPyProc_WHIDE_WAIT_e */);
    mNormalSpeed = 0.0f;
    gabi::call(LK_setSingleMoveAnime, this, anm, 1.3f, start, (s32)end, 5.0f);
    current.angle.y = shape_angle.y;
    dComIfGp_onPlayerStatus0_l(1 /* daPyStts0_UNK1_e */);
    return TRUE;
}
VERIFY(0x0242A8E4, &daPy_lk_c::procWHideWait_init);

/* 0242AA58 */
BOOL daPy_lk_c::procWHideReady() {
    WWHD_FUNC(0x0242AA58, BOOL, this);
    if (changeWHideEndProc(nullptr)) {
        return TRUE;
    }
    if (mProcVar6 == 0) {
        if (LK_checkNoUpperAnime()) {
            gabi::call(LK_setSingleMoveAnime, this, 0x42 /* ANM_WALL */, 0.0f, 3.0f, -1, 5.0f); /* HD: HIO folded */
            mProcVar6 = 1;
        }
        return TRUE;
    }
    cLib_addCalc_l(&current.pos.x, m370C.x, 0.25f, 10.0f, 4.0f);
    cLib_addCalc_l(&current.pos.z, m370C.z, 0.25f, 10.0f, 4.0f);
    if (!cLib_addCalcAngleS(&shape_angle.y, mProcVar2, 2, 0x2000, 0x800)) {
        gabi::store<u8>(0x101CEF18, 0); /* daPy_matAnm_c::offMabaFlg(): m_maba_flg = 0 */
        gabi::store<u8>(0x101CEF19, 1); /* m_maba_timer = 1 */
        procWHideWait_init();
    }
    return TRUE;
}
VERIFY(0x0242AA58, &daPy_lk_c::procWHideReady);

/* 0242ABA8 */
BOOL daPy_lk_c::procWHideMove_init() {
    WWHD_FUNC(0x0242ABA8, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x15 /* daPyProc_WHIDE_MOVE_e */);
    if (mDirection == 2 /* DIR_LEFT */) {
        current.angle.y = shape_angle.y + 0x4000;
    } else {
        current.angle.y = shape_angle.y - 0x4000;
    }
    setBlendWHideMoveAnime(1.0f);
    dComIfGp_onPlayerStatus0_l(1 /* daPyStts0_UNK1_e */);
    return TRUE;
}
VERIFY(0x0242ABA8, &daPy_lk_c::procWHideMove_init);

/* 0242AC48 */
BOOL daPy_lk_c::procWHidePeep_init() {
    WWHD_FUNC(0x0242AC48, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x16 /* daPyProc_WHIDE_PEEP_e */);
    /* HD: HIO folded (rate 0.8/0.9, start 1.0, end 14, morf 5.0) */
    f32 rate;
    int anm;
    if (!(mNoResetFlg0 & 0x10000)) {
        rate = 0.8f;
        anm = mDirection == 2 ? 0x49 /* ANM_WALLPR */ : 0x48 /* ANM_WALLPL */;
    } else {
        rate = 0.9f;
        anm = mDirection == 2 ? 0x4B /* ANM_WALLPRDW */ : 0x4A /* ANM_WALLPLDW */;
    }
    gabi::call(LK_setSingleMoveAnime, this, anm, rate, 1.0f, 0xE, 5.0f);
    dComIfGp_onPlayerStatus0_l(1 /* daPyStts0_UNK1_e */);
    return TRUE;
}
VERIFY(0x0242AC48, &daPy_lk_c::procWHidePeep_init);

/* 0242AD20 */
BOOL daPy_lk_c::procWHideWait() {
    WWHD_FUNC(0x0242AD20, BOOL, this);
    gabi::Local<cXyz> cStack_48; /* sp+0x08 */
    gabi::Local<cXyz> cStack_3c; /* sp+0x14 */
    getWHideBasePos(cStack_3c);
    if (changeWHideEndProc(cStack_3c)) {
        return TRUE;
    }
    int mode = checkWHideModeChange(cStack_3c);
    if (mode == 1) {
        gabi::call(LK_setSingleMoveAnime, this, 0x43 /* ANM_WALLDW */, 1.1f, 0.0f, 7, 5.0f); /* HD: HIO folded */
        mNoResetFlg0 = mNoResetFlg0 | 0x10000;
        LK_FIELD(f32, 0x7FC) = 89.9f; /* mAcchCir[2].SetWallH() */
    } else if (mode == -1) {
        gabi::call(LK_setSingleMoveAnime, this, 0x43 /* ANM_WALLDW */, -1.1f, 0.0f, 7, 5.0f);
        u32 anm = LK_FIELD(u32, 0x57FC); /* mAnmRatioUnder[UNDER_MOVE0_e].getAnmTransform() */
        mFrameCtrlUnder[0].mFrame = 6.999f;
        gabi::store<f32>(anm, 6.999f);
        mNoResetFlg0 = mNoResetFlg0 & ~0x10000u;
        LK_FIELD(f32, 0x7FC) = 125.0f;
    }
    if (mStickDistance > 0.05f && mAcch.m_ground_h != -1000000000.0f) {
        u32 tri = dBgS_GetTriPla_l(mAcchGndPoly);
        if (tri != 0 && !(gabi::load<f32>(tri + 4) < 0.5f)) { /* cBgW_CheckBGround (HD: NULL check) */
            int direction = getDirectionFromCurrentAngle();
            if (direction == 3 /* DIR_RIGHT */ || direction == 2 /* DIR_LEFT */) {
                if (direction == 3) {
                    mDirection = 3;
                    current.angle.y = shape_angle.y - 0x4000;
                } else {
                    mDirection = 2;
                    current.angle.y = shape_angle.y + 0x4000;
                }
                getWHideNextPos(cStack_3c, cStack_48);
                if (checkWHideBackWall(cStack_48)) {
                    if (!(mNoResetFlg0 & 0x10000) || checkWHideFrontFloor(cStack_48)) {
                        return procWHideMove_init();
                    }
                } else {
                    gabi::Local<cXyz> d;  /* sp+0x2C */
                    gabi::Local<cXyz> d4; /* sp+0x20 */
                    cXyz_mi(cStack_48, d, cStack_3c);
                    cXyz_ml(d, d4, 0.25f);
                    PSVECAdd_l(cStack_48, d4, cStack_48);
                    dBgS_LinChk_Set(mLinkLinChk, cStack_3c, cStack_48, this);
                    if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
                        return procWHidePeep_init();
                    }
                }
            }
        }
    }
    current.angle.y = shape_angle.y;
    return TRUE;
}
VERIFY(0x0242AD20, &daPy_lk_c::procWHideWait);

/* 0242AFB8 */
BOOL daPy_lk_c::procWHideMove() {
    WWHD_FUNC(0x0242AFB8, BOOL, this);
    if (gabi::call<BOOL>(LK_changeSlideProc, this)) {
        return TRUE;
    }
    gabi::Local<cXyz> acStack_24; /* sp+0x08 */
    gabi::Local<cXyz> cStack_30;  /* sp+0x14 */
    gabi::Local<cXyz> cStack_3c;  /* sp+0x20 */
    gabi::Local<cXyz> cStack_48;  /* sp+0x2C */
    getWHideBasePos(acStack_24);
    s16 uVar1 = current.angle.y;
    if (changeWHideEndProc(acStack_24)) {
        if (!LK_doButton() && mStickDistance > 0.05f) {
            current.pos.x = gabi::fmadds(50.0f, cM_ssin(uVar1), current.pos.x);
            current.pos.z = gabi::fmadds(50.0f, cM_scos(uVar1), current.pos.z);
        }
        return TRUE;
    }
    u32 tri = dBgS_GetTriPla_l(mLinkLinChkPoly);
    if (tri == 0) { /* HD: NULL check */
        return FALSE;
    }
    s16 sVar8 = cM_atan2s(gabi::load<f32>(tri + 0), gabi::load<f32>(tri + 8));
    f32 fVar2 = 0.0f;
    if (checkWHideModeChange(acStack_24) == 0 && mStickDistance > 0.05f && mAcch.m_ground_h != -1000000000.0f) {
        u32 gnd = dBgS_GetTriPla_l(mAcchGndPoly);
        if (gnd != 0 && !(gabi::load<f32>(gnd + 4) < 0.5f)) { /* cBgW_CheckBGround (HD: NULL check) */
            int direction = getDirectionFromCurrentAngle();
            if (direction == 1 /* DIR_BACKWARD */) {
                f32 spd = mNormalSpeed * 0.5f;
                current.angle.y = current.angle.y - 0x8000;
                mNormalSpeed = spd;
                mDirection = mDirection == 2 /* DIR_LEFT */ ? 3 : 2;
                setBlendWHideMoveAnime(2.0f);
                fVar2 = 3.5f * mStickDistance; /* HD: HIO folded */
            } else if (direction == 0 /* DIR_FORWARD */) {
                fVar2 = 3.5f * mStickDistance;
            }
        }
    }
    gabi::call(LK_setNormalSpeedF, this, fVar2, 0.5f, 2.0f, 0.25f);
    getWHideNextPos(acStack_24, cStack_30);
    if ((mNoResetFlg0 & 0x10000) && !checkWHideFrontFloor(cStack_30)) {
        mNormalSpeed = 0.0f;
        return procWHideWait_init();
    }
    u32 pla = gabi::call<u32>(LK_getWHideModePolygon, this, acStack_24.get(), cStack_30.get(), cStack_3c.get(), (u32)mDirection);
    if (pla != 0) {
        return gabi::call<BOOL>(LK_procWHideReady_init, this, pla, cStack_3c.get());
    }
    if (!(std::fabs(mNormalSpeed) > 0.001f)) {
        return procWHideWait_init();
    }
    bool stop = mAcch.m_ground_h == -1000000000.0f;
    if (!stop) {
        u32 gnd = dBgS_GetTriPla_l(mAcchGndPoly);
        stop = (gnd != 0 && gabi::load<f32>(gnd + 4) < 0.5f) || !checkWHideBackWall(cStack_30); /* HD: NULL check */
    }
    if (stop) {
        mNormalSpeed = 0.0f;
        return procWHideWait_init();
    }
    if (sVar8 != shape_angle.y &&
        gabi::call<u32>(LK_getWHideModePolygon, this, nullptr, nullptr, cStack_48.get(), (u32)mDirection) != 0) {
        s16 a = (s16)(current.angle.y - shape_angle.y) > 0 ? (s16)(sVar8 + 0x4000) : (s16)(sVar8 - 0x4000);
        current.angle.y = a;
        shape_angle.y = sVar8;
        current.pos.x = gabi::fmadds(8.0f, cM_ssin(a), current.pos.x);
        current.pos.z = gabi::fmadds(8.0f, cM_scos(a), current.pos.z);
    }
    /* 0.8f * m_HIO->mWall.m.field_0x50 folded to 6.8f */
    current.pos.x = gabi::fnmsubs(6.8f, cM_ssin(shape_angle.y), current.pos.x);
    current.pos.z = gabi::fnmsubs(6.8f, cM_scos(shape_angle.y), current.pos.z);
    setBlendWHideMoveAnime(-1.0f);
    return TRUE;
}
VERIFY(0x0242AFB8, &daPy_lk_c::procWHideMove);

/* 0242B648 */
BOOL daPy_lk_c::procWHidePeep() {
    WWHD_FUNC(0x0242B648, BOOL, this);
    gabi::Local<cXyz> cStack_38;
    getWHideBasePos(cStack_38);
    if (changeWHideEndProc(cStack_38)) {
        return TRUE;
    }
    J3DFrameCtrl& fc = mFrameCtrlUnder[0];
    f32 dVar3 = fc.getRate();
    if (std::fabs(dVar3) < 0.01f) {
        if (fc.getFrame() < (f32)(s32)(fc.mStart + 1)) {
            return procWHideWait_init();
        }
    }
    if (std::fabs(dVar3) < 0.01f) {
        dComIfGp_onPlayerStatus0_l(mDirection == 2 /* DIR_LEFT */ ? 0x40 /* daPyStts0_UNK40_e */ : 0x20 /* daPyStts0_UNK20_e */);
        dVar3 = (mNoResetFlg0 & 0x10000) ? 0.9f : 0.8f; /* HD: HIO folded */
    }
    if (checkWHideModeChange(cStack_38) != 0) {
        if (dVar3 > 0.0f) {
            fc.setRate(-dVar3);
        }
    } else if (mStickDistance > 0.05f) {
        int direction = getDirectionFromCurrentAngle();
        if ((direction != 0 && dVar3 > 0.0f) || (direction == 0 && dVar3 < 0.0f)) {
            fc.setRate(-dVar3);
        }
    } else if (dVar3 > 0.0f) {
        fc.setRate(-dVar3);
    }
    return TRUE;
}
VERIFY(0x0242B648, &daPy_lk_c::procWHidePeep);

/* ---- d_a_player_crawl.inc ---- */

/* 0242B800 */
f32 daPy_lk_c::getCrawlMoveSpeed() {
    WWHD_FUNC(0x0242B800, f32, this);
    f32 fVar1 = mFrameCtrlUnder[0].getFrame();
    if (!(fVar1 < 17.0f)) {
        fVar1 = fVar1 - 17.0f;
    }
    /* cM_fsin((M_PI / 17) * fVar1): cM_rad2s and the sine table */
    s16 a = gabi::call<s16>(0x02019510 /* cM_rad2s */, fVar1 * gabi::load<f32>(0x10036084) /* (f32)(M_PI / 17) */);
    f32 r = 3.0f * mFrameCtrlUnder[0].getRate(); /* HD: HIO field_0x3C folded */
    return r * cM_ssin(a);
}
VERIFY(0x0242B800, &daPy_lk_c::getCrawlMoveSpeed);

/* 0242B8BC */
void daPy_lk_c::setCrawlMoveDirectionArrow() {
    WWHD_FUNC(0x0242B8BC, void, this);
    u8 direction = 0;
    s32 camIdx = mCameraInfoIdx;
    BOOL bVar = cLib_distanceAngleS(dComIfGp_getCameraAngleY_l(camIdx), shape_angle.y) > 0x4000;
    s32 v = mProcVar6;
    if (v & 4) {
        direction |= !bVar ? 1 : 4;
    }
    if (v & 8) {
        direction |= !bVar ? 4 : 1;
    }
    if (v & 1) {
        if (shape_angle.y == current.angle.y) {
            direction |= !bVar ? 8 : 2;
        } else {
            direction |= !bVar ? 2 : 8;
        }
    }
    if (shape_angle.y == current.angle.y) {
        direction |= !bVar ? 2 : 8;
    } else {
        direction |= !bVar ? 8 : 2;
    }
    gabi::store<u8>(dComIfGp_ea() + 0x5BCB, direction); /* dComIfGp_setAdvanceDirection */
}
VERIFY(0x0242B8BC, &daPy_lk_c::setCrawlMoveDirectionArrow);

/* 0242B9E0 */
BOOL daPy_lk_c::checkCrawlSideWall(cXyz* param_1, cXyz* param_2, cXyz* param_3, cXyz* param_4, s16* param_5, s16* param_6) {
    WWHD_FUNC(0x0242B9E0, BOOL, this, param_1, param_2, param_3, param_4, param_5, param_6);
    dBgS_LinChk_Set(mLinkLinChk, param_1, param_2, this);
    if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        return FALSE;
    }
    param_3->copy(mLinkLinChkCross);
    u32 pla = dBgS_GetTriPla_l(mLinkLinChkPoly);
    if (pla == 0) { /* HD: NULL check */
        return FALSE;
    }
    gabi::store<s16>(gabi::ea(param_5), cM_atan2s(gabi::load<f32>(pla + 0), gabi::load<f32>(pla + 8)));
    gabi::Local<cXyz> t14;      /* sp+0x14 */
    gabi::Local<cXyz> t20;      /* sp+0x20 */
    gabi::Local<cXyz> local_90; /* sp+0x2C */
    gabi::Local<cXyz> local_78; /* sp+0x44 */
    cXyz_ml_l(gabi::at<cXyz>(pla), t20, 75.0f);
    cXyz_pl_l(param_3, t14, t20);
    local_90->copy(*t14.get());
    cXyz_pl_l(local_90, t14, param_3);
    cXyz_ml_l(t14, t20, 0.5f);
    local_78->copy(*t20.get());
    dBgS_LinChk_Set(mLinkLinChk, local_78, local_90, this);
    if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        return FALSE;
    }
    param_4->copy(mLinkLinChkCross);
    pla = dBgS_GetTriPla_l(mLinkLinChkPoly);
    if (pla == 0) {
        return FALSE;
    }
    gabi::store<s16>(gabi::ea(param_6), cM_atan2s(gabi::load<f32>(pla + 0), gabi::load<f32>(pla + 8)));
    gabi::Local<cXyz> xz;       /* sp+0x08 */
    gabi::Local<cXyz> local_9c; /* sp+0x38 */
    cXyz_mi(param_3, local_9c, param_4);
    xz->y = 0.0f;
    xz->x = local_9c->x;
    xz->z = local_9c->z;
    f32 d2 = PSVECSquareMag(xz); /* abs2XZ() */
    if (cLib_distanceAngleS(gabi::load<s16>(gabi::ea(param_5)), gabi::load<s16>(gabi::ea(param_6))) > 0x7F00 &&
        d2 < 5625.0f && d2 > 3600.0f) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x0242B9E0, &daPy_lk_c::checkCrawlSideWall);

/* 0242BC1C */
BOOL daPy_lk_c::procCrawlAutoMove_init(int param_0, cXyz* param_1) {
    WWHD_FUNC(0x0242BC1C, BOOL, this, param_0, param_1);
    u32 subject = dComIfGp_checkPlayerStatus0_l(0x2000 /* daPyStts0_SUBJECT_e */);
    gabi::call(LK_commonProcInit, this, 0x11 /* daPyProc_CRAWL_AUTO_MOVE_e */);
    m370C.copy(*param_1);
    mProcVar3 = 300;
    m35E4 = 1.0f;
    mProcVar6 = param_0;
    mProcVar0 = 20;
    dComIfGp_onPlayerStatus0_l(0x8000000 /* daPyStts0_CRAWL_e */);
    setCrawlMoveDirectionArrow();
    setResetFlg0(resetFlg0() | 0x1000);
    mNormalSpeed = 0.0f;
    for (int i = 0; i < 3; i++) {
        /* mAcchCir[i].SetWallR(): HD reads the radius from a global */
        gabi::call(0x024EFF3C /* dBgS_AcchCir::SetWallR */, &mAcchCir[i], gabi::load<f32>(0x1046CCE4));
    }
    if (subject) {
        dComIfGp_onPlayerStatus0_l(0x2000);
    }
    return TRUE;
}
VERIFY(0x0242BC1C, &daPy_lk_c::procCrawlAutoMove_init);

/* 0242BD24 */
BOOL daPy_lk_c::changeCrawlAutoMoveProc(cXyz* param_1) {
    WWHD_FUNC(0x0242BD24, BOOL, this, param_1);
    f32 temp_f31 = cM_ssin(current.angle.y);
    f32 temp_f30 = cM_scos(current.angle.y);
    f32 temp_f29 = cM_ssin(shape_angle.y);
    f32 temp_f28 = cM_scos(shape_angle.y);
    f32 s75 = 75.0f * temp_f31;
    f32 c75 = 75.0f * temp_f30;
    int var_r29 = 0;
    gabi::Local<cXyz> sp4C;   /* sp+0x08 */
    gabi::Local<u32> angles;  /* sp+0x14: spA (+0), sp8 (+2) */
    gabi::Local<cXyz> sp58;   /* sp+0x18 */
    gabi::Local<cXyz> sp70;   /* sp+0x24 */
    gabi::Local<cXyz> sp40;   /* sp+0x30 */
    gabi::Local<cXyz> sp64;   /* sp+0x3C */
    gabi::Local<cXyz> sp34;   /* sp+0x48 */
    gabi::Local<cXyz> sp28;   /* sp+0x54 */
    gabi::Local<f32> spC;     /* sp+0x60 */
    gabi::Local<cXyz> tmp64;  /* sp+0x64 */
    gabi::Local<cXyz> tmp70;  /* sp+0x70 */
    s16* spA = gabi::at<s16>(angles.a);
    s16* sp8 = gabi::at<s16>(angles.a + 2);
    sp70->y = param_1->y;
    sp70->x = param_1->x + s75;
    sp70->z = param_1->z + c75;
    dBgS_LinChk_Set(mLinkLinChk, param_1, sp70, this);
    if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        sp4C->x = gabi::fmadds(50.0f, temp_f30, sp70->x);
        sp4C->y = sp70->y;
        sp4C->z = gabi::fnmsubs(50.0f, temp_f31, sp70->z);
        if (!checkCrawlSideWall(sp70, sp4C, sp34, sp28, spA, sp8)) {
            return FALSE;
        }
        var_r29 = 1;
    }
    sp4C->x = gabi::fmadds(50.0f, temp_f28, param_1->x);
    sp4C->y = param_1->y;
    sp4C->z = gabi::fnmsubs(50.0f, temp_f29, param_1->z);
    cXyz_ml_l(param_1, tmp70, 2.0f);
    cXyz_mi(tmp70, tmp64, sp4C);
    sp40->copy(*tmp64.get()); /* sp40 = (*param_1 * 2.0f) - sp4C */
    dBgS_LinChk_Set(mLinkLinChk, param_1, sp4C, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        var_r29 |= 8;
        sp58->x = gabi::fmadds(75.0f, temp_f29, sp40->x);
        sp58->y = sp40->y;
        sp58->z = gabi::fmadds(75.0f, temp_f28, sp40->z);
        if (!checkCrawlSideWall(sp40, sp58, sp34, sp28, spA, sp8)) {
            return FALSE;
        }
    } else {
        var_r29 |= 4;
        sp58->x = gabi::fnmsubs(75.0f, temp_f29, sp4C->x);
        sp58->y = sp4C->y;
        sp58->z = gabi::fnmsubs(75.0f, temp_f28, sp4C->z);
        if (!checkCrawlSideWall(sp4C, sp58, sp34, sp28, spA, sp8)) {
            return FALSE;
        }
        dBgS_LinChk_Set(mLinkLinChk, param_1, sp40, this);
        if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
            var_r29 |= 8;
        }
    }
    f32 x1 = param_1->x - s75;
    f32 y1 = param_1->z - c75;
    f32 mx = (sp34->x + sp28->x) * 0.5f;
    f32 mz = (sp34->z + sp28->z) * 0.5f;
    if (gabi::call<BOOL>(0x020109FC /* cM3d_Len2dSqPntAndSegLine */, mx, mz, (f32)sp70->x, (f32)sp70->z, x1, y1,
                         &sp64->x, &sp64->z, spC.get())) {
        sp64->y = current.pos.y;
        return procCrawlAutoMove_init(var_r29, sp64);
    }
    return FALSE;
}
VERIFY(0x0242BD24, &daPy_lk_c::changeCrawlAutoMoveProc);

/* 0242C10C */
void daPy_lk_c::crawlBgCheck(cXyz* param_0, cXyz* param_1) {
    WWHD_FUNC(0x0242C10C, void, this, param_0, param_1);
    gabi::Local<cXyz> cStack_50; /* sp+0x08 */
    gabi::Local<cXyz> cStack_44; /* sp+0x14 */
    gabi::Local<cXyz> cStack_38; /* sp+0x20 */
    u32 m = gabi::ea(mpCLModel.get());
    PSMTXMultVec(gabi::at<Mtx34>(m ? m + 0xC8 : 0) /* getBaseTRMtx() */, gabi::at<cXyz>(0x101CEBE4) /* l_crawl_top_offset */, cStack_50);
    int iVar1 = gabi::call<int>(LK_getCrawlMoveVec, this, cStack_50.get(), param_0, cStack_44.get());
    m = gabi::ea(mpCLModel.get());
    PSMTXMultVec(gabi::at<Mtx34>(m ? m + 0xC8 : 0), gabi::at<cXyz>(0x101CEC14) /* l_crawl_top_up_offset */, cStack_50);
    int iVar2 = gabi::call<int>(LK_getCrawlMoveVec, this, cStack_50.get(), param_1, cStack_38.get());
    if (iVar1 != 0) {
        if (iVar2 != 0) {
            f64 a2 = gabi::call<f64>(0x028E8DD0 /* PSVECSquareMag */, cStack_44.get());
            f64 b2 = gabi::call<f64>(0x028E8DD0, cStack_38.get());
            f64 b = gabi::call<f64>(0x028F4384 /* std::sqrtf */, b2);
            if (!(a2 > b)) { /* GameCube compares abs2() with abs() */
                PSVECSubtract_l(&current.pos, cStack_38, &current.pos);
                return;
            }
        }
        PSVECSubtract_l(&current.pos, cStack_44, &current.pos);
    } else if (iVar2 != 0) {
        PSVECSubtract_l(&current.pos, cStack_38, &current.pos);
    }
}
VERIFY(0x0242C10C, &daPy_lk_c::crawlBgCheck);

/* 0242C234 */
void daPy_lk_c::setDoStatusCrawl() {
    WWHD_FUNC(0x0242C234, void, this);
    dComIfGp_setRStatus_l(0xF /* dActStts_CROUCH_e */);
    if (dComIfGp_getCameraAttentionStatus_l(mCameraInfoIdx) & 0x80) {
        gabi::store<u8>(dComIfGp_ea() + 0x5BB6, 7); /* dComIfGp_setAStatus(dActStts_RETURN_e) */
        if (gabi::call<BOOL>(LK_checkSubjectEnd, this, 1) || mWaterY > current.pos.y) {
            u32 a = dComIfGp_ea() + 0x5CD8; /* dComIfGp_clearPlayerStatus0(0, daPyStts0_SUBJECT_e) */
            gabi::store<u32>(a, gabi::load<u32>(a) & ~0x2000u);
        } else {
            dComIfGp_onPlayerStatus0_l(0x2000);
        }
    } else if (gabi::call<BOOL>(0x02514E24 /* dCamera_c::ChangeModeOK */, gabi::call<u32>(0x024F8044 /* dCam_getBody */), 4)) {
        if (!(mWaterY > current.pos.y)) {
            setResetFlg0(resetFlg0() | 0x4000000 /* daPyRFlg0_SUBJECT_ACCEPT */);
            if ((dComIfGp_getCameraAttentionStatus_l(mCameraInfoIdx) & 0x1000) &&
                gabi::load<u8>(dComIfGp_ea() + 0x5292) == 0 /* !dComIfGp_event_runCheck() */) {
                setSubjectMode();
            }
        }
    }
    if ((dComIfGp_getCameraAttentionStatus_l(mCameraInfoIdx) & 0x80) && (mCurProc != 0x10 /* daPyProc_CRAWL_MOVE_e */ || mProcVar6 == 0)) {
        gabi::call(LK_setBodyAngleToCamera, this);
        if (mBodyAngleX > 0) {
            mBodyAngleX = 0;
        }
    } else {
        mBodyAngleX = 0;
    }
}
VERIFY(0x0242C234, &daPy_lk_c::setDoStatusCrawl);

/* 0242C3D0 */
BOOL daPy_lk_c::checkNotCrawlStand(cXyz* param_0) {
    WWHD_FUNC(0x0242C3D0, BOOL, this, param_0);
    mRoofChkPos.copy(*param_0); /* mRoofChk.SetPos(*param_0) */
    f64 r = dBgS_RoofChk_l(mRoofChk);
    return !((f32)(r - (f64)(f32)current.pos.y) > 125.0f);
}
using checkNotCrawlStand1_t = BOOL (daPy_lk_c::*)(cXyz*);
static constexpr checkNotCrawlStand1_t kCheckNotCrawlStand1 = &daPy_lk_c::checkNotCrawlStand;
VERIFY(0x0242C3D0, kCheckNotCrawlStand1);

/* 0242C440 */
BOOL daPy_lk_c::checkNotCrawlStand(cXyz* param_0, cXyz* param_1) {
    WWHD_FUNC(0x0242C440, BOOL, this, param_0, param_1);
    gabi::Local<cXyz> tmp;      /* sp+0x08 */
    gabi::Local<cXyz> local_2c; /* sp+0x14 */
    cXyz_pl_l(param_0, tmp, param_1);
    local_2c->copy(*tmp.get());
    if (checkNotCrawlStand(local_2c.get())) {
        return TRUE;
    }
    cXyz_mi(param_0, tmp, param_1);
    local_2c->copy(*tmp.get());
    if (checkNotCrawlStand(local_2c.get())) {
        return TRUE;
    }
    return FALSE;
}
using checkNotCrawlStand2_t = BOOL (daPy_lk_c::*)(cXyz*, cXyz*);
static constexpr checkNotCrawlStand2_t kCheckNotCrawlStand2 = &daPy_lk_c::checkNotCrawlStand;
VERIFY(0x0242C440, kCheckNotCrawlStand2);

/* 0242C518 */
BOOL daPy_lk_c::procCrawlEnd_init(int param_1, s16 param_2, s16 param_3) {
    WWHD_FUNC(0x0242C518, BOOL, this, param_1, param_2, param_3);
    f32 dVar4 = mFrameCtrlUnder[0].getFrame();
    gabi::call(LK_commonProcInit, this, 0x12 /* daPyProc_CRAWL_END_e */);
    m34C2 = 1;
    current.angle.y = shape_angle.y;
    /* HD: HIO folded (rate -1.0, start 0.0, end 8, morf 2.0) */
    gabi::call(LK_setSingleMoveAnime, this, 0x40 /* ANM_LIE */, -1.0f, 0.0f, 8, param_1 != 0 ? 2.0f : -1.0f);
    if (param_1 == 0) {
        if (0.0f > dVar4) {
            dVar4 = 0.0f;
        } else if (!(8.0f > dVar4)) {
            dVar4 = 7.999f;
        }
        mFrameCtrlUnder[0].setFrame(dVar4);
        gabi::store<f32>(LK_FIELD(u32, 0x57FC), dVar4); /* mAnmRatioUnder[UNDER_MOVE0_e].getAnmTransform()->setFrame() */
    }
    /* getTransform(0, &info) inlined: the translation of joint 0 (HD: the calc object of m_pbCalc[0], marked dirty) */
    u32 obj = gabi::load<u32>(LK_FIELD(u32, 0x57F0) + 0x84);
    gabi::store<u32>(obj + 0x50, gabi::load<u32>(obj + 0x50) | 8);
    u32 tr = gabi::load<u32>(obj + 0x2C);
    m3700.x = gabi::load<f32>(tr + 0x10);
    m3700.y = gabi::load<f32>(tr + 0x14);
    mNormalSpeed = 0.0f;
    m3700.z = gabi::load<f32>(tr + 0x18);
    shape_angle.z = param_3;
    m35A0 = 0.2f; /* 1.0f / (8 - 3) folded */
    shape_angle.x = param_2;
    return TRUE;
}
VERIFY(0x0242C518, &daPy_lk_c::procCrawlEnd_init);

/* 0242C688 */
BOOL daPy_lk_c::procCrawlStart() {
    WWHD_FUNC(0x0242C688, BOOL, this);
    setDoStatusCrawl();
    f32 r = gabi::fnmsubs(7.0f - mFrameCtrlUnder[0].getFrame(), m35A0, 1.0f); /* HD: HIO field_0x28 (7.0) folded */
    m35E4 = r;
    u32 off = 0x101CEBCC; /* l_crawl_front_offset */
    gabi::Local<cXyz> local_20; /* sp+0x08 */
    gabi::Local<cXyz> cStack_38; /* sp+0x14 */
    gabi::Local<cXyz> afStack_2c; /* sp+0x20 */
    local_20->x = gabi::load<f32>(off + 0);
    local_20->y = gabi::load<f32>(off + 4);
    local_20->z = gabi::load<f32>(off + 8) * r;
    u32 m = gabi::ea(mpCLModel.get());
    PSMTXMultVec(gabi::at<Mtx34>(m ? m + 0xC8 : 0), local_20, afStack_2c);
    m = gabi::ea(mpCLModel.get());
    local_20->y = 50.0f;
    PSMTXMultVec(gabi::at<Mtx34>(m ? m + 0xC8 : 0), local_20, cStack_38);
    crawlBgCheck(cStack_38, cStack_38);
    if (!LK_spActionButton() || mWaterY > current.pos.y + 15.0f /* checkCrawlWaterIn() */) {
        procCrawlEnd_init(0, shape_angle.x, shape_angle.z);
    } else if (mStickDistance > 0.05f &&
               (mFrameCtrlUnder[0].getRate() < 0.01f || mFrameCtrlUnder[0].getFrame() > 7.0f)) {
        gabi::call(LK_procCrawlMove_init, this, (s32)shape_angle.x, (s32)shape_angle.z);
    } else {
        m34C2 = 1;
    }
    return TRUE;
}
VERIFY(0x0242C688, &daPy_lk_c::procCrawlStart);

/* 0242C80C */
BOOL daPy_lk_c::procCrawlMove() {
    WWHD_FUNC(0x0242C80C, BOOL, this);
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    gabi::Local<u32> angles;  /* sp+0x08: sp0A (+0), sp08 (+2) */
    gabi::Local<cXyz> spF4;   /* sp+0x0C */
    gabi::Local<cXyz> spA0;   /* sp+0x18 */
    gabi::Local<cXyz> spAC;   /* sp+0x24 */
    gabi::Local<cXyz> sp124;  /* sp+0x30 */
    gabi::Local<cXyz> sp10C;  /* sp+0x3C */
    gabi::Local<cXyz> spE8;   /* sp+0x48 */
    gabi::Local<cXyz> spB8;   /* sp+0x54 */
    gabi::Local<cXyz> sp100;  /* sp+0x60 */
    gabi::Local<cXyz> sp88;   /* sp+0x6C */
    gabi::Local<cXyz> sp94;   /* sp+0x78 */
    gabi::Local<cXyz> sp118;  /* sp+0x84 */
    gabi::Local<cXyz> spD0;   /* sp+0x90 */
    gabi::Local<cXyz> spC4;   /* sp+0x9C */
    gabi::Local<cXyz> t1;     /* temporaries */
    gabi::Local<cXyz> t2;
    gabi::Local<cXyz> spDC;   /* sp+0xC0 */
    s16* sp0A = gabi::at<s16>(angles.a);
    s16* sp08 = gabi::at<s16>(angles.a + 2);
    setDoStatusCrawl();
    LK_baseMultVec(PSMTXMultVec, 0x101CEC08 /* l_crawl_stand_up_offset */, sp118);
    LK_baseMultVec(PSMTXMultVec, 0x101CEBF0 /* l_crawl_front_up_offset */, sp124);
    LK_baseMultVec(PSMTXMultVec, 0x101CEBFC /* l_crawl_back_up_offset */, sp10C);
    LK_baseMultVec(PSMTXMultVecSR, 0x101CEC5C /* l_crawl_min_side_offset */, sp100);
    int iVar6 = checkNotCrawlStand(sp118.get());
    int iVar7 = checkNotCrawlStand(sp124.get());
    int iVar8 = checkNotCrawlStand(sp10C.get());
    BOOL bVar5;
    bool end;
    if ((iVar6 | iVar7 | iVar8) != 0 || checkNotCrawlStand(sp124, sp100) || checkNotCrawlStand(sp10C, sp100) ||
        checkNotCrawlStand(sp118, sp100)) {
        bVar5 = FALSE;
        mModeFlg = mModeFlg | 0x4000000;
        end = gabi::call<u32>(LK_getSlidePolygon, this) != 0;
    } else {
        bVar5 = TRUE;
        mModeFlg = mModeFlg & ~0x4000000u;
        end = !LK_spActionButton() || gabi::call<u32>(LK_getSlidePolygon, this) != 0;
    }
    if (end || mWaterY > current.pos.y + 15.0f /* checkCrawlWaterIn() */) {
        procCrawlEnd_init(1, shape_angle.x, shape_angle.z);
        return TRUE;
    }
    BOOL bVar4 = FALSE;
    f32 dVar11 = gabi::call<f32>(LK_getCrawlMoveAnmSpeed, this);
    f32 rate = frameCtrl.getRate();
    bool pass;
    if (rate > 0.0f) {
        frameCtrl.setRate(dVar11);
        pass = false;
    } else if (rate < 0.0f) {
        frameCtrl.setRate(-dVar11);
        pass = false;
    } else {
        bVar4 = TRUE;
        pass = true;
    }
    if (pass || frameCtrl.checkPass(0.0f) || frameCtrl.checkPass(17.0f)) {
        if (mStickDistance > 0.05f) {
            if (getDirectionFromShapeAngle() != 1) {
                frameCtrl.setRate(dVar11);
                frameCtrl.mLoop = 0;
            } else {
                frameCtrl.mLoop = frameCtrl.mEnd;
                frameCtrl.setRate(-dVar11);
            }
            gabi::call(LK_initSeAnime, this);
        } else if (!bVar4) {
            f32 f = frameCtrl.checkPass(0.0f) ? 0.0f : 17.0f;
            frameCtrl.setRate(0.0f);
            frameCtrl.setFrame(f);
            gabi::store<f32>(LK_FIELD(u32, 0x57FC), f); /* mAnmRatioUnder[UNDER_MOVE0_e].getAnmTransform()->setFrame() */
        }
    }
    int iVar9 = mProcVar6;
    mProcVar6 = 0;
    LK_baseMultVec(PSMTXMultVec, 0x101CEC20 /* l_crawl_side_offset */, spE8);
    LK_baseMultVec(PSMTXMultVec, 0x101CEC14 /* l_crawl_top_up_offset */, spF4);
    if (!(frameCtrl.getRate() < 0.0f)) { /* HD: as the code branches */
        cXyz_mi(sp124, t1, spF4);
    } else {
        cXyz_mi(sp10C, t1, spF4);
    }
    cXyz_ml_l(t1, t2, 0.5f);
    spB8->copy(*t2.get());
    PSVECAdd_l(spE8, spB8, spE8);
    PSVECAdd_l(spF4, spB8, spF4);
    cXyz_ml_l(spF4, t1, 2.0f);
    cXyz_mi(t1, t2, spE8);
    spDC->copy(*t2.get());
    if (!bVar5 && (checkCrawlSideWall(spF4, spE8, spD0, spC4, sp0A, sp08) ||
                   checkCrawlSideWall(spF4, spDC, spC4, spD0, sp08, sp0A))) {
        mProcVar6 = 1;
        setResetFlg0(resetFlg0() | 0x1000 /* daPyRFlg0_CRAWL_AUTO_MOVE */);
        cXyz_pl_l(spD0, t1, spC4);
        cXyz_ml_l(t1, t2, 0.5f);
        gabi::Local<cXyz> t3;
        cXyz_mi(t2, t3, spB8);
        s16 a = gabi::load<s16>(gabi::ea(sp0A));
        m370C.copy(*t3.get());
        mProcVar2 = a + 0x4000;
    }
    if (mProcVar6 != 0) {
        if (frameCtrl.getRate() != 0.0f) {
            cLib_addCalcAngleS(&shape_angle.y, mProcVar2, 5, 0x1000, 0x800);
            current.angle.y = shape_angle.y;
        }
        if (std::fabs(current.pos.x - m370C.x) > 1.0f) {
            cLib_addCalc_l(&current.pos.x, m370C.x, 0.5f, 10.0f, 1.0f);
        }
        if (std::fabs(current.pos.z - m370C.z) > 1.0f) {
            cLib_addCalc_l(&current.pos.z, m370C.z, 0.5f, 10.0f, 1.0f);
        }
    } else {
        if (iVar9 != 0 && iVar6 != 0 && iVar7 != 0 && iVar8 != 0 && changeCrawlAutoMoveProc(spF4)) {
            return TRUE;
        }
        if (mStickDistance > 0.05f && frameCtrl.getRate() > 0.0f) {
            s16 r24 = shape_angle.y;
            if (dComIfGp_checkPlayerStatus0_l(0x2000 /* daPyStts0_SUBJECT_e */)) {
                /* HD: in first-person view the crawl turns only towards a side direction, 45 degrees short */
                int dir = getDirectionFromShapeAngle();
                if (dir == 2 || dir == 3) {
                    s16 target = m34E8;
                    s16 off = (s16)(target - shape_angle.y) > 0 ? 0x2000 : -0x2000;
                    cLib_addCalcAngleS(&shape_angle.y, (s16)(target - off), 0x20, 0x1B58, 0x1F4);
                }
            } else {
                cLib_addCalcAngleS(&shape_angle.y, m34E8, 0x20, 0x1B58, 0x1F4); /* HD: HIO folded */
            }
            s16 sy = shape_angle.y;
            if (sy != r24) {
                if ((s16)(sy - r24) > 0) {
                    LK_baseMultVec(PSMTXMultVec, 0x101CEC44 /* l_crawl_lside_front_offset */, spAC);
                    LK_baseMultVec(PSMTXMultVec, 0x101CEC2C /* l_crawl_lside_offset */, sp94);
                } else {
                    LK_baseMultVec(PSMTXMultVec, 0x101CEC50 /* l_crawl_rside_front_offset */, spAC);
                    LK_baseMultVec(PSMTXMultVec, 0x101CEC38 /* l_crawl_rside_offset */, sp94);
                }
                /* mGndChk.SetPos(&spAC): HD stores the position into the check directly */
                mGndChkPos.y = spAC->y;
                mGndChkPos.x = spAC->x;
                f32 dx = spAC->x - sp94->x;
                mGndChkPos.z = spAC->z;
                f64 gc = dBgS_GroundCross_l(mGndChk);
                f32 dz = spAC->z - sp94->z;
                spA0->x = dx;
                f32 dy = (f32)(gc - (f64)(f32)sp94->y);
                spA0->y = 0.0f; /* HD: absXZ() built in place */
                spA0->z = dz;
                f32 h = std_sqrtf(PSVECSquareMag(spA0));
                if (cLib_distanceAngleS(cM_atan2s(-dy, h), shape_angle.x) > 0x800) {
                    shape_angle.y = r24;
                    current.angle.y = r24;
                } else {
                    current.angle.y = shape_angle.y;
                }
            } else {
                current.angle.y = sy;
            }
        }
    }
    f32 spd = getCrawlMoveSpeed();
    if (spd < 0.0f) {
        mNormalSpeed = -spd;
        current.angle.y = shape_angle.y + 0x8000;
        LK_baseMultVec(PSMTXMultVec, 0x101CEBD8 /* l_crawl_back_offset */, sp88);
        crawlBgCheck(sp88, sp10C);
    } else {
        mNormalSpeed = spd;
        LK_baseMultVec(PSMTXMultVec, 0x101CEBCC /* l_crawl_front_offset */, sp88);
        crawlBgCheck(sp88, sp124);
    }
    return TRUE;
}
VERIFY(0x0242C80C, &daPy_lk_c::procCrawlMove);

/* 0242D1A4 */
BOOL daPy_lk_c::procCrawlAutoMove() {
    WWHD_FUNC(0x0242D1A4, BOOL, this);
    dComIfGp_setRStatus_l(0xF /* dActStts_CROUCH_e */);
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    s16 var0 = mProcVar0;
    s16 sVar4 = shape_angle.y;
    s16 sVar5 = current.angle.y;
    setResetFlg0(resetFlg0() | 0x1000 /* daPyRFlg0_CRAWL_AUTO_MOVE */);
    if (var0 > 0) {
        if (frameCtrl.checkPass(0.0f) || frameCtrl.checkPass(17.0f)) {
            f32 f = frameCtrl.checkPass(0.0f) ? 0.0f : 17.0f;
            frameCtrl.setRate(0.0f);
            frameCtrl.setFrame(f);
            gabi::store<f32>(LK_FIELD(u32, 0x57FC), f); /* mAnmRatioUnder[UNDER_MOVE0_e].getAnmTransform()->setFrame() */
            mNormalSpeed = 0.0f;
        } else if (std::fabs(frameCtrl.getRate()) < 0.01f) {
            mNormalSpeed = 0.0f;
            mProcVar0 = mProcVar0 - 1;
        }
        setCrawlMoveDirectionArrow();
        setDoStatusCrawl();
        shape_angle.y = sVar4;
        current.angle.y = sVar5;
    } else if (var0 == 0) {
        setCrawlMoveDirectionArrow();
        if (mStickDistance > 0.05f) {
            int direction = getDirectionFromShapeAngle();
            f32 fVar2 = cM_ssin(shape_angle.y);
            f32 fVar3 = cM_scos(shape_angle.y);
            s32 v6 = mProcVar6;
            if (direction == 2 /* DIR_LEFT */ && (v6 & 4)) {
                m370C.x = gabi::fmadds(75.0f, fVar3, m370C.x);
                mProcVar2 = current.angle.y + 0x4000;
                m370C.z = gabi::fnmsubs(75.0f, fVar2, m370C.z);
                m35A0 = current.angle.y == shape_angle.y ? 1.0f : -1.0f;
                mProcVar0 = -1;
            } else if (direction == 3 /* DIR_RIGHT */ && (v6 & 8)) {
                m370C.x = gabi::fnmsubs(75.0f, fVar3, m370C.x);
                mProcVar2 = current.angle.y - 0x4000;
                m370C.z = gabi::fmadds(75.0f, fVar2, m370C.z);
                m35A0 = current.angle.y == shape_angle.y ? 1.0f : -1.0f;
                mProcVar0 = -1;
            } else if ((v6 & 1) && ((shape_angle.y == current.angle.y && direction == 0) ||
                                    (shape_angle.y != current.angle.y && direction == 1))) {
                s16 a = current.angle.y;
                m370C.x = gabi::fmadds(75.0f, cM_ssin(a), m370C.x);
                mProcVar0 = -1;
                m370C.z = gabi::fmadds(75.0f, cM_scos(a), m370C.z);
                mProcVar2 = shape_angle.y;
                m35A0 = 0.0f;
            } else if ((shape_angle.y == current.angle.y && direction == 1) ||
                       (shape_angle.y != current.angle.y && direction == 0)) {
                s16 a = current.angle.y + 0x8000;
                current.angle.y = a;
                m370C.x = gabi::fnmsubs(75.0f, cM_ssin(a), m370C.x);
                mProcVar0 = -1;
                m370C.z = gabi::fnmsubs(75.0f, cM_scos(a), m370C.z);
                mProcVar2 = shape_angle.y;
                m35A0 = 0.0f;
            }
            if (mProcVar0 == -1) {
                gabi::store<u8>(dComIfGp_ea() + 0x5BCB, 0); /* dComIfGp_setAdvanceDirection(0) */
                if (shape_angle.y == current.angle.y) {
                    frameCtrl.mLoop = 0;
                    frameCtrl.setRate(2.0f);
                } else {
                    frameCtrl.mLoop = frameCtrl.mEnd;
                    frameCtrl.setRate(-2.0f);
                }
                gabi::call(LK_initSeAnime, this);
            }
        } else {
            setDoStatusCrawl();
            shape_angle.y = sVar4;
            current.angle.y = sVar5;
        }
    } else {
        BOOL bVar1 = TRUE;
        s16 v3 = mProcVar3;
        f32 r = m35A0;
        if (v3 > 0) {
            mProcVar3 = v3 - 1;
        }
        if (std::fabs(r) > 0.5f) {
            if (cLib_addCalcAngleS(&shape_angle.y, mProcVar2, 5, 0x480, 0x80)) {
                bVar1 = FALSE;
            }
            if (m35A0 < -0.5f) {
                current.angle.y = shape_angle.y + 0x8000;
            } else {
                current.angle.y = shape_angle.y;
            }
            cLib_addCalc_l(&current.pos.x, m370C.x, 0.5f, 3.0f, 1.0f);
            cLib_addCalc_l(&current.pos.z, m370C.z, 0.5f, 3.0f, 1.0f);
        }
        gabi::Local<cXyz> local_28;
        cXyz_mi(&m370C, local_28, &current.pos);
        if (cLib_distanceAngleS(cM_atan2s(local_28->x, local_28->z), current.angle.y) < 0x6000) {
            bVar1 = FALSE;
        }
        f32 spd = getCrawlMoveSpeed();
        if (spd < 0.0f) {
            mNormalSpeed = -spd;
            current.angle.y = shape_angle.y + 0x8000;
        } else {
            mNormalSpeed = spd;
        }
        if (bVar1 || mProcVar3 == 0) {
            gabi::call(LK_procCrawlMove_init, this, (s32)shape_angle.x, (s32)shape_angle.z);
        }
    }
    return TRUE;
}
VERIFY(0x0242D1A4, &daPy_lk_c::procCrawlAutoMove);

/* 0242D7EC */
BOOL daPy_lk_c::procCrawlEnd() {
    WWHD_FUNC(0x0242D7EC, BOOL, this);
    m35E4 = (mFrameCtrlUnder[0].getFrame() - 3.0f) * m35A0; /* HD: HIO field_0x48 (3.0) folded */
    if (mFrameCtrlUnder[0].getRate() > -0.01f) {
        gabi::call(LK_checkNextMode, this, 0);
    } else if (mFrameCtrlUnder[0].getFrame() < 3.0f) {
        if (!gabi::call<BOOL>(LK_checkNextMode, this, 1)) {
            m34C2 = 1;
        }
    } else {
        m34C2 = 1;
    }
    return TRUE;
}
VERIFY(0x0242D7EC, &daPy_lk_c::procCrawlEnd);

/* ---- d_a_player_grab.inc (start) ---- */

/* 0242D888 */
void daPy_lk_c::setWeaponBlur() {
    WWHD_FUNC(0x0242D888, void, this);
    J3DModel* heap = setItemHeap(); /* JKRHeap* */
    u32 blur = mpSwBlur; /* HD: daPy_swBlur_c is allocated */
    u32 buf = gabi::call<u32>(0x0273AEC4 /* operator new(size, align) */, 0x4800, 0x20); /* new (0x20) Vec[2 * 0x300] */
    gabi::store<u32>(blur + 0xAC, buf); /* mSwBlur.mpPosBuffer */
    gabi::call(0x025E3570 /* mDoExt_setCurrentHeap */, heap);
}
VERIFY(0x0242D888, &daPy_lk_c::setWeaponBlur);

/* 0242D8DC */
BOOL daPy_lk_c::procGrabUp_init() {
    WWHD_FUNC(0x0242D8DC, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x6F /* daPyProc_GRAB_UP_e */);
    s32 end = (mActorKeepGrab.mActor->actor_status & 0x08010000) ? 5 : 9; /* HD: HIO folded */
    gabi::call(LK_setSingleMoveAnime, this, 0x66 /* ANM_GRABUP */, 0.9f, 0.0f, end, 1.0f);
    gabi::Local<cXyz> xz; /* sp+0x08 */
    gabi::Local<cXyz> d;  /* sp+0x14 */
    cXyz_mi(&mActorKeepGrab.mActor->current.pos, d, &m3748);
    m370C.x = d->x;
    m370C.z = d->z;
    xz->y = 0.0f;
    xz->z = d->z;
    xz->x = d->x;
    m370C.y = d->y;
    m35C8 = std_sqrtf(PSVECSquareMag(xz)) - 47.0f; /* m370C.absXZ() - 47.0f */
    LK_voiceStart((mActorKeepGrab.mActor->actor_status & 0x10000) ? 16 : 15);
    gabi::call(0x025D537C /* fopAcM_setStageLayer */, mActorKeepGrab.mActor.get());
    setResetFlg0(resetFlg0() | 0x8000 /* daPyRFlg0_GRAB_UP_START */);
    fopAc_ac_c* grab = mActorKeepGrab.mActor;
    if (grab != nullptr && fpcM_GetName(grab) == 0x1C7 /* fpcNm_Stone2_e */) { /* HD: NULL check */
        dComIfGp_onPlayerStatus1_l(0x40000 /* daPyStts1_UNK40000_e */);
    }
    return TRUE;
}
VERIFY(0x0242D8DC, &daPy_lk_c::procGrabUp_init);

/* 0242DA4C */
BOOL daPy_lk_c::procGrabMiss_init() {
    WWHD_FUNC(0x0242DA4C, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x70 /* daPyProc_GRAB_MISS_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x67 /* ANM_GRABNG */, 1.0f, 0.0f, 6, 1.0f); /* HD: HIO folded */
    mProcVar6 = 0;
    mProcVar0 = 0xC;
    gabi::call(0x025B8B68 /* dSv_event_c::onEventBit */, dComIfGs_base_l() + 0x644, 0x4020 /* UNK_4020 */);
    return TRUE;
}
VERIFY(0x0242DA4C, &daPy_lk_c::procGrabMiss_init);

/* the camera of the camera info index (play + 0x5AF8 + idx * 0x34) */
static inline u32 dComIfGp_getCamera_l(s32 idx) { return gabi::load<u32>(dComIfGp_ea() + idx * 0x34 + 0x5AF8); }
/* the inline eye getter (camera + 0x264 plus camera + 0x7C0, a cXyz returned by value) */
static inline void lk_camEye(u32 cam, cXyz* out) { cXyz_pl_l(gabi::at<cXyz>(cam + 0x264), out, gabi::at<cXyz>(cam + 0x7C0)); }
/* dBgS_LinChk inline constructor / destructor with this TU's vtables */
static const dBgS_LinChk_vt lk_linChkVt = {0x10034DD4, 0x10034DE4, 0x10034E04, 0x10034DF4};
static inline void lk_linChk_dt(void* chk) {
    u32 b = gabi::ea(chk);
    gabi::store<u32>(b + 0x58, 0x10034E04);
    gabi::store<u32>(b + 0x64, 0x10034C24);
    gabi::store<u32>(b + 0x20, 0x10034BBC);
    cBgS_LinChk_dt(chk, 0);
}

/* 02426964 HD: the thrown message bottle (probably the HD Tingle Bottle): the throw animation, then an
 * event camera follows the bottle until it is far or hidden, then the camera is reset */
BOOL daPy_lk_c::dProcHDBottleThrow() {
    WWHD_FUNC(0x02426964, BOOL, this);
    gabi::call(LK_setShipRidePosUseItem, this);
    const u32 G = 0x1047B608; /* HD global (tuning values) */
    s32 st = mProcVar6;
    if (st == 3) {
        f32 t = m35A0 + 1.0f;
        if (t > 7.0f) {
            m35A0 = 0.0f;
            mProcVar6 = 4;
        } else {
            m35A0 = t;
            st = mProcVar6;
            if (st < 4) {
                return TRUE;
            }
        }
    } else if (st == 4) {
        f32 a = m35A0 + 1.0f;
        f32 b = m35A4 + 0.1f;
        m35A0 = a;
        m35A4 = b;
        f32 f0 = b > 1.0f ? 1.0f : fsel_l(b, b, 0.0f);
        for (s32 i = 0; i < LK_FIELD(s32, 0x5840); i++) { /* [?] an HD float array (count +0x5840, data +0x5844) */
            gabi::store<f32>(LK_FIELD(u32, 0x5844) + i * 4, f0);
        }
        if (m35A0 > 17.0f && gabi::load<f32>(m_old_fdata + 0xC) < 0.01f) {
            mProcVar6 = 5;
            gabi::call(LK_setSingleMoveAnime, this, 0x6A, 0.8f, 1.0f, 0xE, 0.0f);
            gabi::call(0x025D9D24 /* fopAcM_cancelCarryNow */, mHDThrowActor.get());
            gabi::store<u8>(mHDThrowActor + 0x787, 1);
        }
        st = mProcVar6;
        if (st < 4) {
            return TRUE;
        }
    } else if (st == 5) {
        J3DFrameCtrl& fc = mFrameCtrlUnder[0];
        if (!(fc.getFrame() < (f32)(s32)(fc.mEnd - 1))) {
            gabi::call(LK_setSingleMoveAnime, this, 0 /* ANM_WAITS */, 1.0f, 0.0f, -1, 5.0f);
            mProcVar6 = 6;
        }
    } else if (st == 6) {
        BOOL r21 = FALSE;
        s32 idx = mCameraInfoIdx;
        u32 cam = dComIfGp_getCamera_l(idx);
        u32 actor = mHDThrowActor;
        if (actor != 0) {
            gabi::Local<cXyz> d;    /* sp+0x08 */
            gabi::Local<cXyz> e1;   /* sp+0x50 */
            gabi::Local<cXyz> e2;   /* sp+0x5C */
            gabi::Local<cXyz> e3;   /* sp+0x68 */
            lk_camEye(cam, e1);
            lk_camEye(cam, e2);
            lk_camEye(cam, e3);
            d->x = gabi::load<f32>(actor + 0x314) - e1->x;
            d->y = gabi::load<f32>(actor + 0x318) - e2->y;
            d->z = gabi::load<f32>(actor + 0x31C) - e3->z;
            f64 len = gabi::call<f64>(0x028E8E10 /* PSVECMag */, d.get()); /* f1 compared as returned */
            if (len > (f64)(f32)(gabi::load<f32>(G + 0x1188) + 1500.0f) || d->y < gabi::load<f32>(G + 0x118C) + -500.0f) {
                r21 = TRUE;
            }
            gabi::Local<dBgS_LinChk> chk; /* sp+0x80 */
            gabi::Local<cXyz> eye;        /* sp+0x74 */
            dBgS_LinChk_ct(chk, lk_linChkVt, false);
            lk_camEye(cam, eye);
            dBgS_LinChk_Set(chk, gabi::at<cXyz>(mHDThrowActor + 0x314), eye, this);
            if (cBgS_LineCross(dComIfG_Bgsp(), chk)) {
                r21 = TRUE;
            }
            lk_linChk_dt(chk);
            actor = mHDThrowActor;
            if (!r21 && gabi::load<u8>(actor + 0x787) != 5) {
                st = mProcVar6;
                if (st < 4) {
                    return TRUE;
                }
                goto tail;
            }
        }
        {
            u32 id = actor != 0 ? gabi::load<u32>(actor + 4) : 0xFFFFFFFF;
            gabi::call(0x0253E860 /* dCamera_c::EndEventCamera */, gabi::call<u32>(0x024F8044 /* dCam_getBody */), id);
            actor = mHDThrowActor;
            if (actor != 0) {
                gabi::Local<cXyz> center; /* sp+0x14 */
                gabi::Local<cXyz> eyeP;   /* sp+0x20 */
                fcpy_l(gabi::ea(&center->x), actor + 0x314);
                fcpy_l(gabi::ea(&center->y), actor + 0x318);
                fcpy_l(gabi::ea(&eyeP->y), gabi::ea(&m370C.y));
                fcpy_l(gabi::ea(&center->z), actor + 0x31C);
                fcpy_l(gabi::ea(&eyeP->z), gabi::ea(&m370C.z));
                fcpy_l(gabi::ea(&eyeP->x), gabi::ea(&m370C.x));
                gabi::call(0x0251510C /* dCamera_c::Reset */, cam + 0x248, center.get(), eyeP.get());
                actor = mHDThrowActor;
            }
            u32 id2 = actor != 0 ? gabi::load<u32>(actor + 4) : 0xFFFFFFFF;
            gabi::call(0x025052BC /* dCamera_c::ForceLockOff */, cam + 0x248, id2);
            gabi::call(0x02514F38 /* dCamera_c::Start */, cam + 0x248);
            u32 a = dComIfGp_ea() + 0x52B8;
            gabi::store<u16>(a, gabi::load<u16>(a) | 8);
            gabi::call(LK_endDemoMode, this);
            mHDThrowActor = 0;
            mProcVar6 = 7;
        }
    } else if (st < 4) {
        return TRUE;
    }
tail:
    if (mHDThrowActor != 0) {
        s32 idx = mCameraInfoIdx;
        u32 cam = dComIfGp_getCamera_l(idx);
        s16 maxStep = (s16)(gabi::load<s16>(G + 0x11F4) + 0x800);
        cLib_addCalcAngleS(&mHDThrowAngle, shape_angle.y, 6, maxStep, 0x40);
        cLib_addCalc_l(&m35A8, 1.0f, 5.0f, 0.25f, 0.1f);
        s16 a = mHDThrowAngle;
        f32 r = gabi::fmadds(120.0f, m35A8, 100.0f);
        f32 y = gabi::fmadds(gabi::load<f32>(G + 0x1180) + 130.0f, m35A8, mHDThrowHeight + 70.0f);
        gabi::Local<cXyz> target; /* sp+0x08 */
        target->y = y;
        target->x = gabi::fnmsubs(cM_ssin(a), r, current.pos.x);
        target->z = gabi::fnmsubs(cM_scos(a), r, current.pos.z);
        gabi::call(0x0200EE00 /* cLib_addCalcPos */, &m370C, target.get(), 5.0f, gabi::load<f32>(G + 0x1184) + 38.0f, 20.0f);
        gabi::Local<dBgS_LinChk> chk; /* sp+0x74 */
        dBgS_LinChk_ct(chk, lk_linChkVt, false);
        dBgS_LinChk_Set(chk, gabi::at<cXyz>(mHDThrowActor + 0x314), &m370C, this);
        gabi::Local<cXyz> center; /* sp+0x2C */
        gabi::Local<cXyz> eyeP;   /* sp+0x38 */
        if (!cBgS_LineCross(dComIfG_Bgsp(), chk)) {
            u32 actor = mHDThrowActor;
            fcpy_l(gabi::ea(&center->x), actor + 0x314);
            fcpy_l(gabi::ea(&center->y), actor + 0x318);
            fcpy_l(gabi::ea(&eyeP->y), gabi::ea(&m370C.y));
            fcpy_l(gabi::ea(&center->z), actor + 0x31C);
            fcpy_l(gabi::ea(&eyeP->z), gabi::ea(&m370C.z));
            fcpy_l(gabi::ea(&eyeP->x), gabi::ea(&m370C.x));
        } else {
            u32 c = gabi::ea(chk.get()) + 0x30; /* GetCross(): float copies kept bit-exact as in the original */
            fcpy_l(gabi::ea(&m370C.y), c + 4);
            fcpy_l(gabi::ea(&m370C.z), c + 8);
            fcpy_l(gabi::ea(&m370C.x), c + 0);
            u32 actor = mHDThrowActor;
            fcpy_l(gabi::ea(&center->x), actor + 0x314);
            fcpy_l(gabi::ea(&center->y), actor + 0x318);
            fcpy_l(gabi::ea(&eyeP->z), c + 8);
            fcpy_l(gabi::ea(&center->z), actor + 0x31C);
            fcpy_l(gabi::ea(&eyeP->x), c + 0);
            fcpy_l(gabi::ea(&eyeP->y), c + 4);
        }
        gabi::call(0x02514F50 /* dCamera_c::Set */, cam + 0x248, center.get(), eyeP.get());
        lk_linChk_dt(chk);
    }
    return TRUE;
}
VERIFY(0x02426964, &daPy_lk_c::dProcHDBottleThrow);
