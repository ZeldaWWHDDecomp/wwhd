/**
 * d_a_player_main_04b.cpp (WWHD)
 * Player - Link (daPy_lk_c), address range #04, second half (0240EE2C..02419FEB): the create phases,
 * constructor, playerInit, makeBgWait, the emitter callbacks, and the first move/wait/ladder procs.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_player_main.cpp and its .inc files) to the WWHD layout and code,
 * verified against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * Functions of other ranges are called by address (gabi::call), not as C++ members.
 */
#include "d/actor/d_a_player_main.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
#define LK_SAFESTRING_VTBL 0x10034B24u /* sead::SafeString vtable of this translation unit */

/* sead::SafeString {const char*; vtable}; assureTermination is virtual (+0x14) */
struct lk_SafeString_l {
    be<u32> mStr;
    be<u32> __vtbl;
};
static inline void lk_ss_assure(lk_SafeString_l* s) { gabi::call_ptr(gabi::load<u32>(s->__vtbl + 0x14), s); }
/* inline sead::SafeString::operator== */
static inline bool lk_ss_cmp(lk_SafeString_l* a, lk_SafeString_l* b) {
    lk_ss_assure(a);
    lk_ss_assure(a);
    u32 pa = a->mStr;
    lk_ss_assure(b);
    u32 pb = b->mStr;
    if (pa == pb)
        return true;
    pa = a->mStr;
    pb = b->mStr;
    for (u32 n = 0; n < 0x40001; n++) {
        u8 ca = gabi::load<u8>(pa + n);
        u8 cb = gabi::load<u8>(pb + n);
        if (ca != cb)
            return false;
        if (ca == 0)
            return true;
    }
    return false;
}
/* strcmp(dComIfGp_getStartStageName(), lit) == 0 (HD: SafeString comparison; start stage name at play + 0x5134) */
static inline bool lk_isStartStage(u32 lit) {
    gabi::Local<lk_SafeString_l> a;
    a->__vtbl = LK_SAFESTRING_VTBL;
    a->mStr = lit;
    u32 play = dComIfGp_ea();
    gabi::Local<lk_SafeString_l> b;
    b->__vtbl = LK_SAFESTRING_VTBL;
    b->mStr = play + 0x5134;
    return lk_ss_cmp(a.get(), b.get());
}
/* bits of an f32 value */
static inline u32 lk_fbits(f32 f) {
    u32 v;
    memcpy(&v, &f, 4);
    return v;
}
/* a float copy through an FPR that the recompiled original keeps bit-exact (no SNaN quieting) */
static inline void fcpy_l(u32 dst, u32 src) { gmem_stf32(dst, gmem_ld32(src)); }

/* daPy_lk_c members in the opaque blocks of the shared header (offsets measured in this range) */
#define LK_FIELD(T, off) (*gabi::at<be<T>>(gabi::ea(this) + (off)))
#define LK_demoMode() LK_FIELD(u32, 0x430) /* mDemo.getDemoMode() (GameCube 0x314) */

/* ---- daPy_lk_c functions of other ranges (by address) ---- */
enum : u32 {
    LK_getDirectionFromCurrentAngle = 0x023E3398,
    LK_setSpeedAndAngleNormal = 0x0241650C,
    LK_setNormalSpeedF = 0x02416230,
    LK_setBlendMoveAnime = 0x023E14A0,
    LK_setBlendAtnMoveAnime = 0x023E81E4,
    LK_setBlendAtnBackMoveAnime = 0x023E7E5C,
    LK_checkAtnWaitAnime = 0x023F0CE4,
    LK_checkNextMode = 0x023F14E0,
    LK_changeFrontWallTypeProc = 0x02418E00,
    LK_checkIceSlipFall = 0x024197A0,
    LK_checkBowAnime = 0x023D6A18,
    LK_checkNextActionBowReady = 0x023EB208,
    LK_itemTrigger = 0x023EAB40,
    LK_getReadyItem = 0x023EAA80,
    LK_setBowReadyAnime = 0x023E7BEC,
    LK_checkNextActionBoomerangReady = 0x023EB510,
    LK_setActAnimeUpper = 0x023DE7E8,
    LK_setShipRidePosUseItem = 0x023E2DC4,
    LK_procShipPaddle_init = 0x023F1E60,
    LK_commonProcInit = 0x023DFDD8,
    LK_setShipRidePos = 0x023E287C,
    LK_setSingleMoveAnime = 0x023E0A04,
    LK_getSwimTimerRate = 0x023F8858,
    LK_deleteEquipItem = 0x023DC7AC,
    LK_getDirectionFromShapeAngle = 0x023E4E18,
    LK_checkHeavyStateOn = 0x023DBC24,
    LK_getSlidePolygon = 0x023E4A58,
    LK_changeSlideProc = 0x023E4CD0,
    LK_procCrouch_init = 0x023EE2B8,
    LK_procCrouchDefense_init = 0x023EE510,
    LK_changeWaitProc = 0x023E32AC,
    LK_getDirectionFromAngle = 0x023E3358,
    LK_setTextureAnime = 0x023DD768,
    LK_procWait_init = 0x023E2FF4,
    LK_checkRestHPAnime = 0x023DD12C,
    LK_setOldRootQuaternion = 0x023E278C,
    LK_checkEquipAnime = 0x023D794C,
    LK_resetActAnimeUpper = 0x023DC6A4,
    LK_setAnimeUnequip = 0x023ECEF0,
    LK_setFrontWallType = 0x023E3850,
    LK_procFall_init = 0x023F6564,
    LK_checkShipRideUseItem = 0x023E26EC,
    LK_initShipRideUseItem = 0x023E2E18,
};
/* play + 0x5CD8: dComIfGp_checkPlayerStatus0(0, flag) */
static inline u32 dComIfGp_checkPlayerStatus0_l(u32 flag) { return gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & flag; }

/* play + 0x5CD8: dComIfGp_setPlayerStatus0(0, flag) */
static inline void dComIfGp_onPlayerStatus0_l(u32 flag) {
    u32 a = dComIfGp_ea() + 0x5CD8;
    gabi::store<u32>(a, gabi::load<u32>(a) | flag);
}
/* virtual daPy_lk_c::voiceStart(u32) (vtable slot 0xE4) */
#define LK_voiceStart(n) gabi::call_ptr(gabi::load<u32>(__vtbl + 0xE4), this, (u32)(n))
/* camera attention status of a camera info index: play + 0x5B00 + idx * 0x34 */
static inline u32 dComIfGp_getCameraAttentionStatus_l(s32 idx) { return gabi::load<u32>(dComIfGp_ea() + idx * 0x34 + 0x5B00); }
/* 0200ECD4 cLib_addCalc(f32* value, f32 target, f32 scale, f32 maxStep, f32 minStep) */
static inline f32 cLib_addCalc_l(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200ECD4, v, target, scale, maxStep, minStep);
}
/* cBgS::GetTriPla(bg, poly) of a cBgS_PolyInfo; the cM3dGPla normal at +0 */
static inline u32 dBgS_GetTriPla_l(u32 poly) {
    return gabi::call<u32>(0x020084C8, dComIfG_Bgsp(), (u32)gabi::load<u16>(poly + 2), (u32)gabi::load<u16>(poly + 0));
}
static inline s32 dBgS_GetSpecialCode_l(u32 poly) { return gabi::call<s32>(0x024EF09C, dComIfG_Bgsp(), poly); }
#define mLinkLinChkPoly (gabi::ea(this) + 0x9E4) /* mLinkLinChk's cBgS_PolyInfo */
#define mLinkLinChkCross (*gabi::at<cXyz>(gabi::ea(this) + 0xA00)) /* mLinkLinChk.GetCross() (+0x30) */
/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty; block at +0x2C, flags +4, matrices +0x10) */
static inline Mtx34* lk_getAnmMtx(J3DModel* m, s32 jnt) {
    u32 blk = gabi::load<u32>(gabi::ea(m) + 0x2C);
    gabi::store<u16>(blk + 4, gabi::load<u16>(blk + 4) | 0x10);
    return gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jnt * 0x30);
}
#define mBodyAngleX LK_FIELD(s16, 0x3D0) /* daPy_py_c mBodyAngle.x (GameCube 0x2B4) */
#define mGndChkPos (*gabi::at<cXyz>(gabi::ea(this) + 0xB38)) /* mGndChk's position (dBgS_GndChk + 0x24) */
/* daPy_lk_c::checkAttentionLock(): mpAttention->LockonTruth() or its flag 0x20000000 (+0x20) */
static inline bool lk_checkAttentionLock(daPy_lk_c* p) {
    u32 atn = p->mpAttention;
    return gabi::call<BOOL>(0x024EDFCC /* dAttention_c::LockonTruth */, atn) || (gabi::load<u32>(atn + 0x20) & 0x20000000);
}
/* play + 0x5CDC: dComIfGp_setPlayerStatus1(0, flag) */
static inline void dComIfGp_onPlayerStatus1_l(u32 flag) {
    u32 a = dComIfGp_ea() + 0x5CDC;
    gabi::store<u32>(a, gabi::load<u32>(a) | flag);
}
/* 025E3F3C mDoExt_MtxCalcOldFrame::initOldFrameMorf(f32 morf, u16 start, u16 end) on m_old_fdata */
static inline void lk_initOldFrameMorf(daPy_lk_c* p, f32 morf, u32 start, u32 end) {
    gabi::call(0x025E3F3C, (u32)p->m_old_fdata, morf, start, end);
}
/* mEquipItem == getReadyItem() (the item is loaded after the call) */
static inline bool lk_equipIsReadyItem(daPy_lk_c* p) {
    u32 ready = gabi::call<u32>(LK_getReadyItem, p);
    return (u32)p->mEquipItem == ready;
}
/* JPABaseEmitter (HD layout): fields used by the emitter callbacks */
enum : u32 {
    JPA_RTMTX = 0x1F0,      /* global rotation matrix */
    JPA_TRANS = 0x22C,      /* global translation */
    JPA_ALPHA = 0x247,      /* global alpha (u8) */
    JPA_STATUS = 0x254,     /* status flags: 1 invalid, 8 enable-delete, 0x40 immortal */
    JPA_PTCL_N0 = 0x1B4,    /* particle list counts (isEnableDeleteEmitter: both lists empty) */
    JPA_PTCL_N1 = 0x1C0,
    JPA_ECB = 0x1E4,        /* emitter callback pointer */
    JPA_PCB = 0x1E8,        /* particle callback pointer */
};
static inline bool JPA_isEnableDeleteEmitter(u32 e) {
    return (gabi::load<u32>(e + JPA_STATUS) & 8) && gabi::load<s32>(e + JPA_PTCL_N0) + gabi::load<s32>(e + JPA_PTCL_N1) == 0;
}

/* daPy_followEcallBack_c (HD: vtable at +0) */
struct daPy_followEcallBack_l {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<u32> mpEmitter;
    /* 0x08 */ cXyz mPos;
    /* 0x14 */ csXyz mAngle;
    /* 0x1A */ u8 _1A[2];
};
struct daPy_waterDropEcallBack_l : daPy_followEcallBack_l {
    /* 0x1C */ be<u32> field_0x1C;
};
WWHD_SIZE(daPy_waterDropEcallBack_l, 0x20);
/* daPy_swimTailEcallBack_c (HD: vtable at +0) */
struct daPy_swimTailEcallBack_l {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<u8> mbEnd;
    /* 0x05 */ be<u8> field_0x05;
    /* 0x06 */ u8 _06[2];
    /* 0x08 */ be<f32> mSpeedRate;
    /* 0x0C */ be<f32> mWaterY;
    /* 0x10 */ be<f32> mWaterFlatY;
    /* 0x14 */ cXyz mPos;
    /* 0x20 */ be<u32> field_0x20; /* const csXyz* */
    /* 0x24 */ be<u32> mpEmitter;
};
WWHD_SIZE(daPy_swimTailEcallBack_l, 0x28);
/* daPy_fanSwingEcallBack_c (HD: vtable at +0) */
struct daPy_fanSwingEcallBack_l {
    /* 0x0 */ be<u32> __vtbl;
    /* 0x4 */ be<s32> mAlphaOutFlg;
    /* 0x8 */ be<u32> mpEmitter;
};
/* daPy_mtxPosFollowEcallBack_c (HD: vtable at +0) */
struct daPy_mtxPosFollowEcallBack_l {
    /* 0x0 */ be<u32> __vtbl;
    /* 0x4 */ be<u32> mpEmitter;
    /* 0x8 */ be<u32> mpMtx;
    /* 0xC */ be<u32> mpAngle;
};

/* 0240EBB0 */
static BOOL daPy_Execute(daPy_lk_c* i_this) {
    WWHD_FUNC(0x0240EBB0, BOOL, i_this);
    return gabi::call<BOOL>(0x0240CDD0 /* daPy_lk_c::execute */, i_this);
}
VERIFY(0x0240EBB0, daPy_Execute);

/* 0240EBB4 */
static BOOL daPy_IsDelete(daPy_lk_c*) {
    WWHD_FUNC(0x0240EBB4, BOOL, 0);
    return TRUE;
}
VERIFY(0x0240EBB4, daPy_IsDelete);

/* 0240EBBC */
static void daPy_swimTailEcallBack_remove(daPy_swimTailEcallBack_l* cb) {
    WWHD_FUNC(0x0240EBBC, void, cb);
    if (cb->mpEmitter != 0) {
        gabi::store<u32>(cb->mpEmitter + JPA_ECB, 0); /* setEmitterCallBackPtr(NULL) */
        u32 e = cb->mpEmitter;                        /* becomeInvalidEmitter (HD: also a -1 at +0x5C) */
        gabi::store<s32>(e + 0x5C, -1);
        gabi::store<u32>(e + JPA_STATUS, gabi::load<u32>(e + JPA_STATUS) | 1);
        cb->mpEmitter = 0;
    }
}
VERIFY(0x0240EBBC, daPy_swimTailEcallBack_remove);

/* 0240EE28 */
static BOOL daPy_Delete(daPy_lk_c* i_this) {
    WWHD_FUNC(0x0240EE28, BOOL, i_this);
    return gabi::call<BOOL>(0x0240EBF0 /* daPy_lk_c::playerDelete */, i_this);
}
VERIFY(0x0240EE28, daPy_Delete);

/* 0240EE2C */
static cPhs_State phase_1(daPy_lk_c* i_this) {
    WWHD_FUNC(0x0240EE2C, cPhs_State, i_this);
    gabi::store<u32>(dComIfGp_ea() + 0x5B2C, gabi::ea(i_this)); /* dComIfGp_setPlayer(0, i_this) */
    gabi::store<u32>(dComIfGp_ea() + 0x5B34, gabi::ea(i_this)); /* dComIfGp_setLinkPlayer(i_this) */
    fopAcM_setStageLayer(i_this);
    u32 b = gabi::ea(i_this);
    gabi::store<u32>(b + 0x39C, 0xFFFFFFFF); /* attention_info.flags */
    gabi::store<f32>(b + 0x390, i_this->current.pos.x); /* attention_info.position */
    gabi::store<f32>(b + 0x394, i_this->current.pos.y + 125.0f);
    gabi::store<f32>(b + 0x398, i_this->current.pos.z);
    return cPhs_NEXT_e;
}
VERIFY(0x0240EE2C, phase_1);

/* 02412884 */
static cPhs_State phase_2(daPy_lk_c* i_this) {
    WWHD_FUNC(0x02412884, cPhs_State, i_this);
    s32 camId = gabi::load<s8>(dComIfGp_ea() + 0x5B30); /* dComIfGp_getPlayerCameraID(0) */
    if (gabi::load<u32>(dComIfGp_ea() + camId * 0x34 + 0x5AF8) == 0) { /* dComIfGp_getCamera(..) */
        return cPhs_INIT_e;
    }
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) { /* fopAcM_ct(i_this, daPy_lk_c) */
        if (i_this != NULL) {
            gabi::call(0x0240FE40 /* daPy_lk_c::daPy_lk_c */, i_this);
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    gabi::call(0x02410BE8 /* daPy_lk_c::playerInit */, i_this);
    /* HD: notify an HD system object (*(0x101F8344) + 0x218) */
    u32 sys = gabi::load<u32>(0x101F8344);
    if (sys != 0) {
        u32 o = gabi::load<u32>(sys + 0x218);
        if (o != 0) {
            gabi::call(0x0268899C, o);
        }
    }
    return cPhs_NEXT_e;
}
VERIFY(0x02412884, phase_2);

/* 02413E68 */
static cPhs_State phase_3(daPy_lk_c* i_this) {
    WWHD_FUNC(0x02413E68, cPhs_State, i_this);
    return gabi::call<cPhs_State>(0x024131D0 /* daPy_lk_c::makeBgWait */, i_this);
}
VERIFY(0x02413E68, phase_3);

/* 02413E6C */
static cPhs_State daPy_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02413E6C, cPhs_State, i_this);
    /* dComLbG_PhaseHandler(&mPhase, l_method {phase_1, phase_2, phase_3}, i_this) */
    return gabi::call<cPhs_State>(0x02525FE4, gabi::ea(i_this) + 0x43C, 0x101CEEB0u, i_this);
}
VERIFY(0x02413E6C, daPy_Create);

/* 024140F8 */
static void daPy_followEcallBack_execute(daPy_followEcallBack_l* cb, JPABaseEmitter* emitter) {
    WWHD_FUNC(0x024140F8, void, cb, emitter);
    u32 e = gabi::ea(emitter);
    f32 x = cb->mPos.x;
    f32 y = cb->mPos.y;
    f32 z = cb->mPos.z;
    if (gabi::load<u8>(e + 0x262) >= 7) { /* HD: the emitter's Y is mirrored for these resource versions */
        y = -y;
    }
    gabi::store<f32>(e + JPA_TRANS + 4, y); /* setGlobalTranslation */
    gabi::store<f32>(e + JPA_TRANS + 0, x);
    gabi::store<f32>(e + JPA_TRANS + 8, z);
    /* setGlobalRotation */
    gabi::call(0x028245AC /* JPAGetXYZRotateMtx */, (s32)cb->mAngle.x, (s32)cb->mAngle.y, (s32)cb->mAngle.z, e + JPA_RTMTX);
}
VERIFY(0x024140F8, daPy_followEcallBack_execute);

/* 0241413C */
static void daPy_followEcallBack_setup(daPy_followEcallBack_l* cb, JPABaseEmitter* emitter, const cXyz*, const csXyz*, s8) {
    WWHD_FUNC(0x0241413C, void, cb, emitter, 0, 0, 0);
    cb->mpEmitter = gabi::ea(emitter);
}
VERIFY(0x0241413C, daPy_followEcallBack_setup);

/* 02414144 */
static void daPy_waterDropEcallBack_execute(daPy_waterDropEcallBack_l* cb, JPABaseEmitter* emitter) {
    WWHD_FUNC(0x02414144, void, cb, emitter);
    gabi::call(0x024140F8 /* daPy_followEcallBack_c::execute */, cb, emitter);
    if (JPA_isEnableDeleteEmitter(gabi::ea(emitter))) {
        gabi::call(0x0240CA4C /* daPy_waterDropEcallBack_c::end */, cb);
    }
}
VERIFY(0x02414144, daPy_waterDropEcallBack_execute);

/* 024141A0 */
static void daPy_waterDropEcallBack_setup(daPy_waterDropEcallBack_l* cb, JPABaseEmitter* emitter, const cXyz*, const csXyz*, s8) {
    WWHD_FUNC(0x024141A0, void, cb, emitter, 0, 0, 0);
    cb->mpEmitter = gabi::ea(emitter);
    u32 e = gabi::ea(emitter); /* becomeImmortalEmitter */
    gabi::store<u32>(e + JPA_STATUS, gabi::load<u32>(e + JPA_STATUS) | 0x40);
}
VERIFY(0x024141A0, daPy_waterDropEcallBack_setup);

/* 024141B4 */
static void daPy_fanSwingEcallBack_execute(daPy_fanSwingEcallBack_l* cb, JPABaseEmitter* emitter) {
    WWHD_FUNC(0x024141B4, void, cb, emitter);
    u32 e = gabi::ea(emitter);
    if (JPA_isEnableDeleteEmitter(e)) {
        gabi::store<u32>(e + JPA_STATUS, gabi::load<u32>(e + JPA_STATUS) & ~0x40u); /* quitImmortalEmitter */
        gabi::store<u32>(e + JPA_ECB, 0);
        cb->mpEmitter = 0;
    } else if (cb->mAlphaOutFlg != FALSE) {
        gabi::Local<be<s16>> alpha;
        *alpha = (s16)gabi::load<u8>(e + JPA_ALPHA);
        gabi::call(0x0200F564 /* cLib_chaseS */, alpha.get(), 0, 50);
        s16 a = *alpha;
        gabi::store<u8>(e + JPA_ALPHA, (u8)a);
        if (a == 0) {
            gabi::store<u32>(e + JPA_STATUS, gabi::load<u32>(e + JPA_STATUS) & ~0x40u);
            gabi::store<u32>(e + JPA_ECB, 0);
            cb->mpEmitter = 0;
        }
    }
}
VERIFY(0x024141B4, daPy_fanSwingEcallBack_execute);

/* 02414DD8 */
static void daPy_swimTailEcallBack_getMaxWaterY(daPy_swimTailEcallBack_l* cb, cXyz* pos) {
    WWHD_FUNC(0x02414DD8, void, cb, pos);
    if (daSea_ChkArea(pos->x, pos->z)) {
        pos->y = daSea_calcWave(pos->x, pos->z) + 2.0f;
        if (cb->mWaterFlatY > pos->y) {
            pos->y = cb->mWaterFlatY + 2.0f;
        }
    } else if (cb->mWaterFlatY != -1000000000.0f /* -G_CM3D_F_INF */) {
        pos->y = cb->mWaterFlatY + 2.0f;
    } else {
        pos->y = cb->mWaterY;
    }
}
VERIFY(0x02414DD8, daPy_swimTailEcallBack_getMaxWaterY);

/* 02415134 */
static void daPy_swimTailEcallBack_draw(daPy_swimTailEcallBack_l*, JPABaseEmitter*) {
    WWHD_FUNC(0x02415134, void, 0, 0);
    /* HD: empty (GameCube: GXSetZMode when dPa_control_c::isStatus(1)) */
}
VERIFY(0x02415134, daPy_swimTailEcallBack_draw);

/* HD: the material parameter block passed to the emitter by the HD setup (0281E5A8) */
struct lk_emitterParam_l {
    /* 0x00 */ be<u32> mName; /* sead::SafeString */
    /* 0x04 */ be<u32> __vtbl;
    /* 0x08 */ be<f32> m08[4];
    /* 0x18 */ be<u8> m18[4];
};
/* 02415138 */
static void daPy_swimTailEcallBack_setup(daPy_swimTailEcallBack_l* cb, JPABaseEmitter* emitter, const cXyz* pos, const csXyz* angle, s8) {
    WWHD_FUNC(0x02415138, void, cb, emitter, pos, angle, 0);
    cb->mpEmitter = gabi::ea(emitter);
    cb->field_0x20 = gabi::ea(angle);
    cb->field_0x05 = 0;
    cb->mbEnd = 0;
    /* HD: the emitter's material parameter "jparticle_type_b" */
    gabi::Local<lk_emitterParam_l> prm;
    prm->mName = 0x10035EDC;
    prm->__vtbl = LK_SAFESTRING_VTBL;
    prm->m08[0] = 1.0f;
    prm->m08[1] = 1.0f;
    prm->m08[2] = 1.0f;
    prm->m08[3] = 1.0f;
    prm->m18[0] = 1;
    prm->m18[1] = 0;
    prm->m18[2] = 0;
    prm->m18[3] = 0;
    gabi::call(0x0281E5A8, emitter, prm.get());
}
VERIFY(0x02415138, daPy_swimTailEcallBack_setup);

/* 024151B8 */
static void daPy_mtxPosFollowEcallBack_execute(daPy_mtxPosFollowEcallBack_l* cb, JPABaseEmitter* emitter) {
    WWHD_FUNC(0x024151B8, void, cb, emitter);
    Mtx34* mtx = gabi::at<Mtx34>(cb->mpMtx);
    mDoMtx_stack_c::transS(mtx->m[0][3], mtx->m[1][3], mtx->m[2][3]);
    if (cb->mpAngle != 0) {
        mDoMtx_stack_c::YrotM(gabi::load<s16>(cb->mpAngle + 2));
    }
    /* setGlobalRTMatrix */
    gabi::call(0x028249B0 /* JPASetRMtxTVecfromMtx */, mDoMtx_stack_c::get(), gabi::ea(emitter) + JPA_RTMTX, gabi::ea(emitter) + JPA_TRANS);
}
VERIFY(0x024151B8, daPy_mtxPosFollowEcallBack_execute);

/* 02417FF4 */
f32 daPy_lk_c::getLadderMoveAnmSpeed() {
    WWHD_FUNC(0x02417FF4, f32, this);
    return 0.5f + mStickDistance; /* HD: getAnmSpeedStickRate(HIO) folded */
}
VERIFY(0x02417FF4, &daPy_lk_c::getLadderMoveAnmSpeed);

/* 02412F2C */
f32 daPy_lk_c::getCrawlMoveAnmSpeed() {
    WWHD_FUNC(0x02412F2C, f32, this);
    if (mProcVar6 != 0) {
        return gabi::fmadds(1.5f, mStickDistance, 0.5f); /* getAnmSpeedStickRate(0.5f, 2.0f) */
    }
    f32 s = mStickDistance;
    return 1.0f + (s + s); /* HD: getAnmSpeedStickRate(HIO) folded */
}
VERIFY(0x02412F2C, &daPy_lk_c::getCrawlMoveAnmSpeed);

/* 0241699C */
void daPy_lk_c::setSpeedAndAngleAtnBack() {
    WWHD_FUNC(0x0241699C, void, this);
    f32 f1;
    if (mStickDistance > 0.05f) {
        if (gabi::call<int>(LK_getDirectionFromCurrentAngle, this) == 1 /* DIR_BACKWARD */) {
            current.angle.y = (s16)(current.angle.y - 0x8000);
            mNormalSpeed = -mNormalSpeed;
        }
        s16 origAngleY = current.angle.y;
        cLib_addCalcAngleS(&current.angle.y, m34E8, 6, 0xBB8, 0x7D0); /* HD: HIO folded */
        f1 = (2.5f * mStickDistance) * cM_scos((s16)(current.angle.y - origAngleY));
    } else {
        f1 = 0.0f;
    }
    shape_angle.y = m34E6;
    gabi::call(LK_setNormalSpeedF, this, f1, 0.5f, 8.0f, 2.0f);
}
VERIFY(0x0241699C, &daPy_lk_c::setSpeedAndAngleAtnBack);

/* 02416AB4: HD, new: the direction/angle part of setSpeedAndAngleAtn without the speed (used by
 * procSubjectivity) */
void daPy_lk_c::setAngleAtnHD() {
    WWHD_FUNC(0x02416AB4, void, this);
    u8 dir = mDirection;
    if (dir == 0 /* DIR_FORWARD */) {
        gabi::call(LK_setSpeedAndAngleNormal, this, 0xBB8);
        return;
    }
    if (dir == 1 /* DIR_BACKWARD */) {
        setSpeedAndAngleAtnBack();
        return;
    }
    if (mStickDistance > 0.05f) {
        int d = gabi::call<int>(LK_getDirectionFromCurrentAngle, this);
        s16 target = m34E8;
        if (d == 1) {
            current.angle.y = (s16)(current.angle.y - 0x8000);
            mNormalSpeed = -mNormalSpeed;
        }
        cLib_addCalcAngleS(&current.angle.y, target, 6, 0xBB8, 0x7D0);
    }
}
VERIFY(0x02416AB4, &daPy_lk_c::setAngleAtnHD);

/* 02416B70: HD, new (the matcher's "setSpeedAndAngleAtnActor"; that one is 02419CC8): the
 * attention move of setSpeedAndAngleAtnActor without the shape angle */
void daPy_lk_c::setSpeedAndAngleAtnNoShapeHD() {
    WWHD_FUNC(0x02416B70, void, this);
    if (mStickDistance > 0.05f) {
        if (gabi::call<int>(LK_getDirectionFromCurrentAngle, this) == 1) {
            current.angle.y = (s16)(current.angle.y - 0x8000);
            mNormalSpeed = -mNormalSpeed;
        }
        s16 origAngleY = current.angle.y;
        cLib_addCalcAngleS(&current.angle.y, m34E8, 6, 0xBB8, 0x7D0);
        f32 f1 = (5.0f * mStickDistance) * cM_scos((s16)(current.angle.y - origAngleY));
        gabi::call(LK_setNormalSpeedF, this, f1, 0.5f, 7.5f, 4.0f);
    } else {
        gabi::call(LK_setNormalSpeedF, this, 0.0f, 0.5f, 7.5f, 4.0f);
    }
}
VERIFY(0x02416B70, &daPy_lk_c::setSpeedAndAngleAtnNoShapeHD);

/* 02416FA8: HD, new: the move animation of the subjective (first person) mode */
void daPy_lk_c::setBlendSubjectMoveAnimeHD() {
    WWHD_FUNC(0x02416FA8, void, this);
    u32 flg = mModeFlg;
    if (std::fabs((f32)mNormalSpeed) < 0.001f) {
        mModeFlg = flg | 1;
    } else {
        mModeFlg = flg & ~1u;
    }
    s32 a = m34DC;
    if (a < 0) {
        a = -a;
    }
    if (a >= 0x2000) {
        gabi::call(LK_setBlendAtnBackMoveAnime, this, 2.4f);
    } else {
        gabi::call(LK_setBlendMoveAnime, this, 2.4f);
    }
}
VERIFY(0x02416FA8, &daPy_lk_c::setBlendSubjectMoveAnimeHD);

/* 02417388 */
BOOL daPy_lk_c::procControllWait() {
    WWHD_FUNC(0x02417388, BOOL, this);
    if (dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
        gabi::call(LK_setShipRidePosUseItem, this);
        if (gabi::load<u32>(dComIfGp_ea() + 0x5B2C) == gabi::ea(this)) { /* daPy_getPlayerActorClass() == this */
            gabi::call(LK_procShipPaddle_init, this);
        }
    } else {
        gabi::call<BOOL>(LK_checkNextMode, this, 0);
    }
    return TRUE;
}
VERIFY(0x02417388, &daPy_lk_c::procControllWait);

/* 02417538 */
void daPy_lk_c::setSpeedAndAngleAtn() {
    WWHD_FUNC(0x02417538, void, this);
    if (mDirection == 0 /* DIR_FORWARD */) {
        gabi::call(LK_setSpeedAndAngleNormal, this, 0xBB8); /* HD: m_HIO->mMove.m.field_0x0 folded */
        return;
    }
    if (mDirection == 1 /* DIR_BACKWARD */) {
        setSpeedAndAngleAtnBack();
        return;
    }
    f32 f1;
    if (mStickDistance > 0.05f) {
        if (gabi::call<int>(LK_getDirectionFromCurrentAngle, this) == 1) {
            current.angle.y = (s16)(current.angle.y - 0x8000);
            mNormalSpeed = -mNormalSpeed;
        }
        s16 origAngleY = current.angle.y;
        cLib_addCalcAngleS(&current.angle.y, m34E8, 6, 0xBB8, 0x7D0);
        f1 = (5.0f * mStickDistance) * cM_scos((s16)(current.angle.y - origAngleY));
    } else {
        f1 = 0.0f;
    }
    shape_angle.y = m34E6;
    gabi::call(LK_setNormalSpeedF, this, f1, 0.5f, 7.5f, 4.0f);
}
VERIFY(0x02417538, &daPy_lk_c::setSpeedAndAngleAtn);

/* 02419BF0 */
BOOL daPy_lk_c::procAtnMove() {
    WWHD_FUNC(0x02419BF0, BOOL, this);
    setSpeedAndAngleAtn();
    if (!gabi::call<BOOL>(LK_checkNextMode, this, 0) &&
        (mDirection != 0 /* DIR_FORWARD */ || !gabi::call<BOOL>(LK_changeFrontWallTypeProc, this)) &&
        !gabi::call<BOOL>(LK_checkIceSlipFall, this)) {
        gabi::call(LK_setBlendAtnMoveAnime, this, -1.0f);
    }
    return TRUE;
}
VERIFY(0x02419BF0, &daPy_lk_c::procAtnMove);

/* 02419C70 */
void daPy_lk_c::setShapeAngleToAtnActor() {
    WWHD_FUNC(0x02419C70, void, this);
    if (mpAttnActorLockOn != NULL) {
        s16 targetAngle = cLib_targetAngleY(&current.pos, &mpAttnActorLockOn->eyePos);
        cLib_addCalcAngleS(&shape_angle.y, targetAngle, 2, 0x2000, 0x800);
    }
}
VERIFY(0x02419C70, &daPy_lk_c::setShapeAngleToAtnActor);

/* 02419CC8 (not named by the matcher, which put the name on 02416B70) */
void daPy_lk_c::setSpeedAndAngleAtnActor() {
    WWHD_FUNC(0x02419CC8, void, this);
    f32 f1;
    if (mStickDistance > 0.05f) {
        if (gabi::call<int>(LK_getDirectionFromCurrentAngle, this) == 1 /* DIR_BACKWARD */) {
            current.angle.y = (s16)(current.angle.y - 0x8000);
            mNormalSpeed = -mNormalSpeed;
        }
        s16 origAngleY = current.angle.y;
        cLib_addCalcAngleS(&current.angle.y, m34E8, 6, 0xBB8, 0x7D0);
        f1 = (5.0f * mStickDistance) * cM_scos((s16)(current.angle.y - origAngleY));
    } else {
        f1 = 0.0f;
    }
    setShapeAngleToAtnActor();
    gabi::call(LK_setNormalSpeedF, this, f1, 0.5f, 7.5f, 4.0f);
}
VERIFY(0x02419CC8, &daPy_lk_c::setSpeedAndAngleAtnActor);

/* 02419E00 */
BOOL daPy_lk_c::procAtnActorWait() {
    WWHD_FUNC(0x02419E00, BOOL, this);
    setSpeedAndAngleAtnActor();
    if (!gabi::call<BOOL>(LK_checkNextMode, this, 0)) {
        if (gabi::call<BOOL>(LK_checkAtnWaitAnime, this)) {
            gabi::call(LK_setBlendAtnMoveAnime, this, -1.0f);
        } else {
            gabi::call(LK_setBlendMoveAnime, this, -1.0f);
        }
    }
    return TRUE;
}
VERIFY(0x02419E00, &daPy_lk_c::procAtnActorWait);

/* 02419E70 */
BOOL daPy_lk_c::procAtnActorMove() {
    WWHD_FUNC(0x02419E70, BOOL, this);
    setSpeedAndAngleAtnActor();
    if (!gabi::call<BOOL>(LK_checkNextMode, this, 0) && !gabi::call<BOOL>(LK_checkIceSlipFall, this)) {
        gabi::call(LK_setBlendAtnMoveAnime, this, -1.0f);
    }
    return TRUE;
}
VERIFY(0x02419E70, &daPy_lk_c::procAtnActorMove);

/* 02419ED4 */
void daPy_lk_c::checkNextActionBowFly() {
    WWHD_FUNC(0x02419ED4, void, this);
    if (gabi::call<BOOL>(LK_checkBowAnime, this)) {
        gabi::call(LK_checkNextActionBowReady, this);
    } else if (gabi::call<BOOL>(LK_itemTrigger, this) && lk_equipIsReadyItem(this)) {
        gabi::call(LK_setBowReadyAnime, this);
        m355E = 0;
    }
}
VERIFY(0x02419ED4, &daPy_lk_c::checkNextActionBowFly);

/* 02419F54 */
void daPy_lk_c::checkNextActionBoomerangFly() {
    WWHD_FUNC(0x02419F54, void, this);
    if (gabi::load<u16>(gabi::ea(this) + 0x5888) == 0x35 /* checkBoomerangReadyAnime(): BOOMWAIT */) {
        gabi::call(LK_checkNextActionBoomerangReady, this);
    } else if (gabi::call<BOOL>(LK_itemTrigger, this) && lk_equipIsReadyItem(this)) {
        gabi::call(LK_setActAnimeUpper, this, 0x35 /* dRes_INDEX_LKANM_BCK_BOOMWAIT_e */, 2 /* UPPER_MOVE2_e */, 0.8f, 0.0f, -1, 2.4f);
    }
}
VERIFY(0x02419F54, &daPy_lk_c::checkNextActionBoomerangFly);

/* 02412B7C */
BOOL daPy_lk_c::procShipRestart_init() {
    WWHD_FUNC(0x02412B7C, BOOL, this);
    u32 ship = gabi::load<u32>(dComIfGp_ea() + 0x5B3C); /* dComIfGp_getShipActor() */
    gabi::call(LK_commonProcInit, this, 0x91 /* daPyProc_SHIP_RESTART_e */);
    gravity = 0.0f;
    gabi::store<u8>(ship + 0x636, 2); /* ship->setPaddleMove() */
    gabi::call(LK_setShipRidePos, this, 1);
    gabi::call(LK_setSingleMoveAnime, this, 0xD5 /* ANM_SEARESET */, 1.0f, 0.0f, -1, -1.0f);
    dComIfGp_onPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */);
    return TRUE;
}
VERIFY(0x02412B7C, &daPy_lk_c::procShipRestart_init);

/* 02412C30 */
void daPy_lk_c::setSwimMoveAnime(int swimMoveAnm) {
    WWHD_FUNC(0x02412C30, void, this, swimMoveAnm);
    f32 rate = 0.6f + (0.5f * std::fabs((f32)mNormalSpeed)) / LK_FIELD(f32, 0x3C4) /* mMaxNormalSpeed */; /* HD: HIO folded */
    f32 endFrame = mFrameCtrlUnder[0].getFrame() * (f32)mFrameCtrlUnder[0].getEnd();
    f32 timer = gabi::call<f32>(LK_getSwimTimerRate, this);
    gabi::call(LK_setSingleMoveAnime, this, swimMoveAnm, gabi::fmadds(timer, 1.0f, rate), 0.0f, -1, 5.3f);
    f32 frame = endFrame * (f32)mFrameCtrlUnder[0].getEnd();
    mFrameCtrlUnder[0].setFrame(frame);
    gabi::store<f32>(gabi::load<u32>(gabi::ea(this) + 0x57FC), frame); /* mAnmRatioUnder[0].getAnmTransform()->setFrame() */
}
VERIFY(0x02412C30, &daPy_lk_c::setSwimMoveAnime);

/* 02412D54 */
void daPy_lk_c::setSwimTail() {
    WWHD_FUNC(0x02412D54, void, this);
    /* static JGeometry::TVec3<f32> tail_scale(1.0f, 1.0f, -1.0f) (HD: initialised on first use, unused) */
    if (gabi::load<u32>(0x1046D1E4) == 0) {
        gabi::store<f32>(0x1046D1E8, 1.0f);
        gabi::store<f32>(0x1046D1F0, -1.0f);
        gabi::store<f32>(0x1046D1EC, 1.0f);
        gabi::store<u32>(0x1046D1E4, 1);
    }
    u32 cb0 = gabi::ea(this) + 0x66A8; /* mSwimTailEcallBack[0] */
    if (gabi::load<u32>(cb0 + 0x24) == 0) {
        /* dComIfGp_particle_setShipTail(dPa_name::ID_IT_JN_LK_SWIMK_L, &cb.getPos(), &current.angle, &scale, 0xFF, &cb) */
        dPa_control_set(dComIfGp_getParticle(), 5, 0x3A, gabi::at<cXyz>(cb0 + 0x14), &current.angle, &scale, 0xFF,
                        gabi::at<dPa_levelEcallBack>(cb0), -1, nullptr, nullptr, nullptr);
        gabi::store<u8>(cb0 + 5, 1); /* onRightFlg() */
    }
    u32 cb1 = gabi::ea(this) + 0x66D0; /* mSwimTailEcallBack[1] */
    if (gabi::load<u32>(cb1 + 0x24) == 0) {
        dPa_control_set(dComIfGp_getParticle(), 5, 0x3A, gabi::at<cXyz>(cb1 + 0x14), &current.angle, &scale, 0xFF,
                        gabi::at<dPa_levelEcallBack>(cb1), -1, nullptr, nullptr, nullptr);
    }
}
VERIFY(0x02412D54, &daPy_lk_c::setSwimTail);

/* 02412E58 */
BOOL daPy_lk_c::procSwimMove_init(int param_1) {
    WWHD_FUNC(0x02412E58, BOOL, this, param_1);
    gabi::call(LK_commonProcInit, this, 0x37 /* daPyProc_SWIM_MOVE_e */);
    if (!param_1) {
        mFrameCtrlUnder[0].setFrame(0.0f);
    }
    gravity = 0.0f;
    setSwimMoveAnime(0x83 /* ANM_SWIMING */);
    mDirection = 0; /* DIR_FORWARD */
    if (mNoResetFlg0 & 0x100 /* daPyFlg0_UNK100 */) {
        if (mEquipItem != 0x100 /* daPyItem_NONE_e */) {
            gabi::call(LK_deleteEquipItem, this, 1);
        }
        f32 y = mWaterY;
        speed.y = 0.0f;
        current.pos.y = y;
        setSwimTail();
    }
    dComIfGp_onPlayerStatus0_l(0x100000 /* daPyStts0_SWIM_e */);
    mProcVar6 = 0;
    m35C4 = 0.0f; /* HD: HIO folded */
    return TRUE;
}
VERIFY(0x02412E58, &daPy_lk_c::procSwimMove_init);

/* 02412F68 */
BOOL daPy_lk_c::procCrawlMove_init(s16 param_0, s16 param_1) {
    WWHD_FUNC(0x02412F68, BOOL, this, param_0, param_1);
    BOOL var_r29 = mCurProc != 0x11 /* daPyProc_CRAWL_AUTO_MOVE_e */;
    BOOL var_r27 = dComIfGp_checkPlayerStatus0_l(0x2000 /* daPyStts0_SUBJECT_e */) != 0;
    gabi::call(LK_commonProcInit, this, 0x10 /* daPyProc_CRAWL_MOVE_e */);
    if (var_r29 != 0) {
        f32 dVar4 = getCrawlMoveAnmSpeed();
        if (gabi::call<int>(LK_getDirectionFromShapeAngle, this) == 1 /* DIR_BACKWARD */) {
            dVar4 = -dVar4;
        }
        current.angle.y = shape_angle.y;
        gabi::call(LK_setSingleMoveAnime, this, 0x41 /* ANM_LIEFORWARD */, dVar4, 0.0f, -1, 5.0f);
    } else {
        setResetFlg0(resetFlg0() | 0x1000); /* daPyRFlg0_CRAWL_AUTO_MOVE */
    }
    m35A0 = -1.0f;
    mProcVar6 = var_r29 ^ 1;
    shape_angle.x = param_0;
    shape_angle.z = param_1;
    m35E4 = 1.0f;
    dComIfGp_onPlayerStatus0_l(0x8000000 /* daPyStts0_CRAWL_e */);
    if (var_r27 != 0) {
        dComIfGp_onPlayerStatus0_l(0x2000 /* daPyStts0_SUBJECT_e */);
    }
    return TRUE;
}
VERIFY(0x02412F68, &daPy_lk_c::procCrawlMove_init);

/* 024130B0 */
BOOL daPy_lk_c::procSmallJump_init(int param_1) {
    WWHD_FUNC(0x024130B0, BOOL, this, param_1);
    gabi::call(LK_commonProcInit, this, 0x29 /* daPyProc_SMALL_JUMP_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x3C /* ANM_JMPST */, 0.2f, 0.0f, 6, 0.0f); /* HD: HIO folded */
    if (param_1 == 0) {
        f32 t = ((m3724.y - current.pos.y) + 45.0f) * gravity;
        mNormalSpeed = 5.0f;
        speed.y = std_sqrtf(-(t + t));
        mProcVar6 = 2;
    } else {
        mNormalSpeed = 10.0f;
        speed.y = 20.0f;
        mProcVar6 = 0;
    }
    if (gabi::call<BOOL>(LK_checkHeavyStateOn, this)) {
        gravity = gravity * 0.44444445f;
    }
    LK_voiceStart(5);
    return TRUE;
}
VERIFY(0x024130B0, &daPy_lk_c::procSmallJump_init);

/* 02415EF0 */
BOOL daPy_lk_c::checkSubjectEnd(BOOL i_playSound) {
    WWHD_FUNC(0x02415EF0, BOOL, this, i_playSound);
    if (gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0 /* dComIfGp_event_runCheck() */ ||
        (mItemTrigger & 3 /* BTN_A | BTN_B */) ||
        (mItemButton & 0x20 /* BTN_L */) ||
        (dComIfGp_getCameraAttentionStatus_l(mCameraInfoIdx) & 0x2000)) {
        if (i_playSound) {
            gabi::call(0x025E1988 /* seStartSystem */, 0x8FB /* JA_SE_SUBJ_VIEW_OUT */);
        }
        setResetFlg0(resetFlg0() | 0x10000000); /* daPyRFlg0_UNK10000000 */
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x02415EF0, &daPy_lk_c::checkSubjectEnd);

/* 02416160 */
BOOL daPy_lk_c::procCrawlStart_init() {
    WWHD_FUNC(0x02416160, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0xF /* daPyProc_CRAWL_START_e */);
    m3700.copy(*cXyz_Zero);
    m34C2 = 1;
    gabi::call(LK_setSingleMoveAnime, this, 0x40 /* ANM_LIE */, 1.0f, 0.0f, 9, 3.0f); /* HD: HIO folded */
    mNormalSpeed = 0.0f;
    current.angle.y = shape_angle.y;
    m35A0 = 0.14285715f; /* 1.0f / (HIO 0x28 - 0x24) */
    m35E4 = 0.0f;
    dComIfGp_onPlayerStatus0_l(0x8000000 /* daPyStts0_CRAWL_e */);
    return TRUE;
}
VERIFY(0x02416160, &daPy_lk_c::procCrawlStart_init);

/* 02415FB0 */
int daPy_lk_c::getCrawlMoveVec(cXyz* param_0, cXyz* param_1, cXyz* param_2) {
    WWHD_FUNC(0x02415FB0, int, this, param_0, param_1, param_2);
    dBgS_LinChk_Set(mLinkLinChk, param_0, param_1, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        u32 triPla = dBgS_GetTriPla_l(mLinkLinChkPoly);
        if (triPla == 0) { /* HD: NULL check */
            return FALSE;
        }
        int iVar4 = dBgS_GetSpecialCode_l(mLinkLinChkPoly);
        f32 temp = gabi::load<f32>(triPla + 4);
        if (temp < 0.5f && !(temp < -0.8f)) { /* cBgW_CheckBWall(temp) */
            gabi::Local<cXyz> local_3c;
            cXyz_mi(param_1, local_3c, &mLinkLinChkCross);
            gabi::Local<cXyz> xz; /* absXZ() */
            xz->x = local_3c->x;
            xz->y = 0.0f;
            xz->z = local_3c->z;
            f32 dVar7 = -std_sqrtf(PSVECSquareMag(xz));
            param_2->y = local_3c->y;
            param_2->x = dVar7 * gabi::load<f32>(triPla + 0);
            param_2->z = dVar7 * gabi::load<f32>(triPla + 8);
            return TRUE;
        } else if (iVar4 == 1 || (temp < 0.643832f && iVar4 == 2)) {
            gabi::Local<cXyz> d;
            cXyz_mi(param_1, d, &mLinkLinChkCross);
            param_2->copy(*d);
            return TRUE;
        }
    }
    return FALSE;
}
VERIFY(0x02415FB0, &daPy_lk_c::getCrawlMoveVec);

/* 02416230 */
void daPy_lk_c::setNormalSpeedF(f32 param_1, f32 param_2, f32 param_3, f32 param_4) {
    WWHD_FUNC(0x02416230, void, this, param_1, param_2, param_3, param_4);
    f32 dVar10;
    if (gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0 /* dComIfGp_event_runCheck() */ ||
        gabi::call<BOOL>(LK_checkHeavyStateOn, this) ||
        LK_FIELD(f32, 0x3CC) < 0.0f /* checkGrabWear() */) {
        dVar10 = LK_FIELD(f32, 0x3C4) /* mMaxNormalSpeed */ * mStickDistance;
    } else {
        f32 st = mStickDistance;
        dVar10 = (LK_FIELD(f32, 0x3C4) * st) * st;
    }
    if (LK_demoMode() == 0xE /* daPy_demo_c::DEMO_KEEP_e */ || gabi::call<u32>(LK_getSlidePolygon, this) != 0) {
        return;
    }
    if (mAcch.m_flags & dBgS_Acch::WALL_HIT) {
        s16 uVar2 = 0;
        u32 cir = gabi::ea(this) + 0x74C; /* mAcchCir[0] */
        for (int i = 0; i < 3; i++, cir += 0x40) {
            if (gabi::load<u32>(cir + 0x10) & 2) { /* ChkWallHit() */
                uVar2 = (s16)((current.angle.y + 0x8000) - gabi::load<s16>(cir + 0x3C) /* GetWallAngleY() */);
                break;
            }
        }
        s32 a = uVar2;
        if (a < 0) {
            a = -a;
        }
        if (a < 0x4000) {
            dVar10 *= gabi::fnmsubs(0.6f, cM_scos(uVar2), 1.0f); /* 1.0f - cM_scos(uVar2) * HIO */
        }
    }
    f32 temp_f3;
    f32 dVar6;
    f32 ns = mNormalSpeed;
    if (dVar10 < ns) {
        temp_f3 = ns - dVar10;
        if (temp_f3 > param_3) {
            temp_f3 = param_3;
        }
        if (temp_f3 < param_4) {
            temp_f3 = param_4;
        }
        param_1 = 0.0f;
        dVar6 = dVar10;
    } else {
        temp_f3 = param_3;
        dVar6 = 0.0f;
    }
    if (!(std::fabs(param_1) < 3.8146973e-06f)) { /* !cM3d_IsZero(param_1) */
        f32 s = ns + param_1;
        if (s > dVar10) {
            mNormalSpeed = dVar10;
        } else {
            mNormalSpeed = s;
        }
    } else {
        cLib_addCalc_l(&mNormalSpeed, dVar6, param_2, temp_f3, param_4);
    }
}
VERIFY(0x02416230, &daPy_lk_c::setNormalSpeedF);

/* 02416C78 */
s16 daPy_lk_c::checkBodyAngleX(s16 param_1) {
    WWHD_FUNC(0x02416C78, s16, this, param_1);
    gabi::Local<cXyz> sp38;
    gabi::Local<cXyz> sp2C;
    Mtx34* m = lk_getAnmMtx(mpCLModel, 2 /* CL_JNT_BODY_CHN_e */);
    sp38->x = m->m[0][3]; /* mDoMtx_multVecZero */
    sp38->y = m->m[1][3];
    sp38->z = m->m[2][3];
    /* static Vec top_vec = {0.0f, 70.0f, 0.0f} (0x101CEECC) */
    u32 top_vec = 0x101CEECC;
    if (param_1 >= 0) {
        gabi::store<f32>(top_vec + 8, 25.0f);
    } else {
        gabi::store<f32>(top_vec + 8, -25.0f);
    }
    mDoMtx_stack_c::transS(sp38->x, sp38->y, sp38->z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), param_1, shape_angle.y, 0);
    PSMTXMultVec(mDoMtx_stack_c::get(), gabi::at<cXyz>(top_vec), sp2C);
    dBgS_LinChk_Set(mLinkLinChk, sp38, sp2C, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        gabi::Local<cXyz> local_4c;
        cXyz_mi(&mLinkLinChkCross, local_4c, sp38);
        f32 s = cM_ssin(param_1);
        f32 f1 = s * std_sqrtf(PSVECSquareMag(local_4c));
        f1 = f1 / std_sqrtf(5525.0f); /* SQUARE(70.0f) + SQUARE(25.0f) */
        if (f1 > 1.0f) {
            f1 = 1.0f;
        } else if (f1 < -1.0f) {
            f1 = -1.0f;
        }
        f32 f2 = std_sqrtf(gabi::fnmsubs(f1, f1, 1.0f));
        param_1 = cM_atan2s(f1, f2);
    }
    return param_1;
}
VERIFY(0x02416C78, &daPy_lk_c::checkBodyAngleX);

/* 02416E90 */
BOOL daPy_lk_c::setBodyAngleToCamera() {
    WWHD_FUNC(0x02416E90, BOOL, this);
    if (dComIfGp_getCameraAttentionStatus_l(mCameraInfoIdx) & 0x10 /* dCamAttnStts_00000010_e */) {
        gabi::Local<be<s16>> local_16; /* x */
        gabi::Local<be<s16>> local_18; /* y */
        u32 body = gabi::call<u32>(0x024F8044 /* dCam_getBody */);
        BOOL bVar1 = gabi::call<BOOL>(0x02506964 /* dCamera_c::CalcSubjectAngle */, body, local_16.get(), local_18.get());
        if (bVar1 != 0) {
            shape_angle.y = *local_18; /* HD: current.angle.y is not set */
            u32 x0 = gabi::call<u32>(0x02416C78 /* checkBodyAngleX */, this, (s32)mBodyAngleX);
            mBodyAngleX = (s16)x0;
            u32 x1 = gabi::call<u32>(0x02416C78 /* checkBodyAngleX */, this, (s32)*local_16);
            if (x1 == (u32)(s32)*local_16) {
                mBodyAngleX = (s16)x1;
            } else {
                mBodyAngleX = (s16)x0;
            }
        } else {
            s32 idx = mCameraInfoIdx;
            u32 cam = gabi::load<u32>(dComIfGp_ea() + idx * 0x34 + 0x5AF8); /* dComIfGp_getCamera(mCameraInfoIdx) */
            s16 y = gabi::load<s16>(cam + 0x236);
            shape_angle.y = y;
            current.angle.y = y;
            mBodyAngleX = gabi::load<s16>(cam + 0x234);
        }
        return bVar1;
    }
    return FALSE;
}
VERIFY(0x02416E90, &daPy_lk_c::setBodyAngleToCamera);

/* 0241701C */
BOOL daPy_lk_c::procSubjectivity() {
    WWHD_FUNC(0x0241701C, BOOL, this);
    gabi::store<u8>(dComIfGp_ea() + 0x5BB6, 7); /* dComIfGp_setAStatus(dActStts_RETURN_e) */
    bool end;
    if (checkSubjectEnd(FALSE)) {
        end = true;
    } else if (mProcVar6 != 0 && !(mItemButton & 0x40) /* spActionButton(): R released while crouching */) {
        end = true;
    } else {
        /* HD: a slide polygon also ends the subjective mode */
        end = gabi::call<BOOL>(LK_changeSlideProc, this) != 0;
    }
    if (end) {
        gabi::call(0x025E1988 /* seStartSystem */, 0x8FB /* JA_SE_SUBJ_VIEW_OUT */);
        if (mProcVar6 != 0) {
            if (mEquipItem == 0x100 /* daPyItem_NONE_e */) {
                return gabi::call<BOOL>(LK_procCrouch_init, this);
            }
            return gabi::call<BOOL>(LK_procCrouchDefense_init, this);
        }
        gabi::call(LK_changeWaitProc, this);
        return TRUE;
    }
    if (mProcVar6 == 0) {
        /* HD: the subjective view moves Link (attention move without the shape angle) */
        setSpeedAndAngleAtnNoShapeHD();
        setBodyAngleToCamera();
        setBlendSubjectMoveAnimeHD();
        return TRUE;
    }
    /* HD: crouching in the subjective view with the stick held starts crawling */
    if ((mItemButton & 0x40) && gabi::load<f32>(m_old_fdata + 0xC) < 0.01f && mStickDistance > 0.05f &&
        !(mWaterY > current.pos.y + 15.0f) && mEquipItem == 0x100 /* daPyItem_NONE_e */) {
        gabi::Local<cXyz> sp2c;
        gabi::Local<cXyz> sp38;
        gabi::Local<cXyz> sp44;
        f32 y = current.pos.y;
        mDoMtx_stack_c::transS(current.pos.x, y, current.pos.z);
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), m34E2, shape_angle.y, 0);
        PSMTXMultVec(mDoMtx_stack_c::get(), gabi::at<cXyz>(0x101CEBC0), sp2c);
        PSMTXMultVec(mDoMtx_stack_c::get(), gabi::at<cXyz>(0x101CEBE4), sp38);
        if (getCrawlMoveVec(sp38, sp2c, sp44)) {
            gabi::Local<cXyz> sp8;
            cXyz_mi(&current.pos, sp8, sp44);
            f32 gy = sp8->y + 5.0f;
            mGndChkPos.z = sp8->z;
            mGndChkPos.x = sp8->x;
            sp8->y = gy;
            mGndChkPos.y = gy;
            sp8->y = cBgS_GroundCross(dComIfG_Bgsp(), mGndChk);
            gabi::Local<cXyz> sp20;
            cXyz_mi(&current.pos, sp20, sp8);
            gabi::Local<cXyz> sp14; /* absXZ() */
            sp14->x = sp20->x;
            sp14->y = 0.0f;
            sp14->z = sp20->z;
            f32 xz = std_sqrtf(PSVECSquareMag(sp14));
            s16 a = cM_atan2s(-sp20->y, xz);
            if (cLib_distanceAngleS(a, m34E2) > 0x100) {
                return TRUE;
            }
        }
        return procCrawlStart_init();
    }
    setAngleAtnHD();
    setBodyAngleToCamera();
    setBlendSubjectMoveAnimeHD();
    return TRUE;
}
VERIFY(0x0241701C, &daPy_lk_c::procSubjectivity);

/* 024172D4 */
BOOL daPy_lk_c::procCall() {
    WWHD_FUNC(0x024172D4, BOOL, this);
    if (gabi::load<u32>(dComIfGp_ea() + 0x5B38) != 0) { /* dComIfGp_getCb1Player() */
        fopAc_ac_c* cb1 = gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + 0x5B38));
        s16 targetAngle = cLib_targetAngleY(&current.pos, &cb1->eyePos);
        cLib_addCalcAngleS(&shape_angle.y, targetAngle, 2, 0x2000, 0x800);
        current.angle.y = shape_angle.y;
    }
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        if (gabi::load<u32>(dComIfGp_ea() + 0x5B38) != 0) {
            u32 partner = gabi::load<u32>(dComIfGp_ea() + 0x5B38); /* partner->onNpcCallCommand() */
            gabi::store<u32>(partner + 0x3BC, gabi::load<u32>(partner + 0x3BC) | 2);
        }
        gabi::call<BOOL>(LK_checkNextMode, this, 0);
    }
    return TRUE;
}
VERIFY(0x024172D4, &daPy_lk_c::procCall);

/* 024173F4 */
BOOL daPy_lk_c::procIceSlipAlmostFall_init() {
    WWHD_FUNC(0x024173F4, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0xA0 /* daPyProc_ICE_SLIP_ALMOST_FALL_e */);
    current.angle.y = cM_atan2s(m36AC.x, m36AC.z);
    int direction = gabi::call<int>(LK_getDirectionFromAngle, this, (s32)(s16)(current.angle.y - shape_angle.y));
    int anm;
    if (direction == 1 /* DIR_BACKWARD */) {
        anm = 0x57; /* ANM_DAMF */
    } else if (direction == 2 /* DIR_LEFT */) {
        anm = 0x56; /* ANM_DAMR */
    } else if (direction == 3 /* DIR_RIGHT */) {
        anm = 0x55; /* ANM_DAML */
    } else {
        anm = 0x58; /* ANM_DAMB */
    }
    gabi::call(LK_setSingleMoveAnime, this, anm, 0.6f, 1.0f, 9, 1.0f); /* HD: HIO folded */
    gabi::call(LK_setTextureAnime, this, 6, 0);
    mNormalSpeed = 0.0f;
    return TRUE;
}
VERIFY(0x024173F4, &daPy_lk_c::procIceSlipAlmostFall_init);

/* 024176A8 */
BOOL daPy_lk_c::procFreeWait_init() {
    WWHD_FUNC(0x024176A8, BOOL, this);
    gabi::call(LK_commonProcInit, this, 5 /* daPyProc_FREE_WAIT_e */);
    mNormalSpeed = 0.0f;
    f32 dVar2 = cM_rnd();
    int anm;
    if (dVar2 < 0.3333f) {
        anm = 0xE1; /* ANM_FREEA */
        mProcVar6 = 0;
    } else if (dVar2 < 0.6666f) {
        anm = 0xE2; /* ANM_FREEB */
        mProcVar6 = 1;
    } else {
        anm = 0xE3; /* ANM_FREED */
        mProcVar6 = 0;
    }
    gabi::call(LK_setSingleMoveAnime, this, anm, 1.0f, 0.0f, -1, 5.0f);
    mDirection = 4; /* DIR_NONE */
    current.angle.y = shape_angle.y;
    return TRUE;
}
VERIFY(0x024176A8, &daPy_lk_c::procFreeWait_init);

/* 02417BFC */
BOOL daPy_lk_c::procFreeWait() {
    WWHD_FUNC(0x02417BFC, BOOL, this);
    if (lk_checkAttentionLock(this)) {
        setSpeedAndAngleAtn();
    } else {
        gabi::call(LK_setSpeedAndAngleNormal, this, 0xBB8); /* HD: m_HIO->mMove.m.field_0x0 folded */
    }
    if (mProcVar6 != 0) {
        if (mFrameCtrlUnder[0].checkPass(168.0f)) {
            LK_voiceStart(48);
        } else if (mFrameCtrlUnder[0].checkPass(105.0f)) {
            LK_voiceStart(47);
        }
    }
    if (!gabi::call<BOOL>(LK_checkNextMode, this, 0) && gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0 /* dComIfGp_event_runCheck() */) {
        mFrameCtrlUnder[0].setRate(0.0f);
        gabi::call(LK_procWait_init, this);
    }
    return TRUE;
}
VERIFY(0x02417BFC, &daPy_lk_c::procFreeWait);

/* 024177A8 */
BOOL daPy_lk_c::procWait() {
    WWHD_FUNC(0x024177A8, BOOL, this);
    gabi::Local<cXyz> xz0; /* m36A0.abs2XZ() */
    xz0->y = 0.0f;
    xz0->x = m36A0.x;
    xz0->z = m36A0.z;
    if (!(PSVECSquareMag(xz0) > 1.0000001e-06f)) {
        gabi::Local<cXyz> xz1; /* m36AC.abs2XZ() */
        xz1->x = m36AC.x;
        xz1->y = 0.0f;
        xz1->z = m36AC.z;
        if (!(PSVECSquareMag(xz1) < 25.0f)) {
            return procIceSlipAlmostFall_init();
        }
    }
    if (gabi::load<u8>(dComIfGp_ea() + 0x5292) == 0 /* !dComIfGp_event_runCheck() */ &&
        LK_FIELD(u16, 0x420) == 0 /* !checkPlayerDemoMode() */ &&
        (mItemTrigger & 0x20) /* spLTrigger() */ &&
        (mAcch.m_flags & dBgS_Acch::WALL_HIT)) {
        u32 cir = gabi::ea(this) + 0x74C; /* mAcchCir[0] */
        for (int i = 0; i < 3; i++, cir += 0x40) {
            if (gabi::load<u32>(cir + 0x10) & 2) { /* ChkWallHit() */
                s16 sVar4 = (s16)(gabi::load<s16>(cir + 0x3C) + 0x8000);
                if (cLib_distanceAngleS(shape_angle.y, sVar4) <= 0x2000) {
                    shape_angle.y = sVar4;
                    current.angle.y = sVar4;
                    m34E6 = sVar4;
                }
                break;
            }
        }
    }
    s16 sVar2 = shape_angle.y;
    if (lk_checkAttentionLock(this)) {
        setSpeedAndAngleAtn();
    } else {
        gabi::call(LK_setSpeedAndAngleNormal, this, 0xBB8); /* HD: m_HIO->mMove.m.field_0x0 folded */
    }
    m35A0 = 0.0f;
    if (gabi::call<BOOL>(LK_checkNextMode, this, 0)) {
        return TRUE;
    }
    if (m34C3 == 0) {
        if (mFrameCtrlUnder[0].getRate() < 0.01f || !gabi::call<BOOL>(LK_checkRestHPAnime, this)) {
            gabi::call(LK_setBlendMoveAnime, this, 2.4f); /* HD: m_HIO->mBasic.m.field_0xC folded */
            mModeFlg = (mModeFlg & ~0x400u) | 0x100; /* offModeFlg(ModeFlg_00000400); onModeFlg(ModeFlg_00000100) */
        }
    } else if (gabi::call<BOOL>(LK_checkRestHPAnime, this) && gabi::load<u16>(gabi::ea(this) + 0x5848) != 0x11F /* dRes_INDEX_LKANM_BCK_WAITB_e */) {
        BOOL uVar3 = gabi::load<u16>(gabi::ea(this) + 0x65D0) == 0x234; /* m_tex_anm_heap.mIdx == mTexAnmIndexTable[daPyFace_TMABAF].mBtpIdx */
        u16 uVar1 = m3530;
        gabi::call(LK_setSingleMoveAnime, this, 0x1D /* ANM_WAITATOB */, 0.6f, 0.0f, 0xC, 6.0f);
        if (uVar3 == 0) {
            mModeFlg = (mModeFlg | 0x400) & ~0x100u;
        } else {
            gabi::call(LK_setTextureAnime, this, 0xE, (u32)uVar1);
        }
    } else {
        m35A0 = 0.005f * (f32)(s16)(shape_angle.y - sVar2);
        gabi::call(LK_setBlendMoveAnime, this, -1.0f);
    }
    if (LK_demoMode() == 0x2A /* daPy_demo_c::DEMO_KM_WAIT_e */) {
        if (mProcVar0 != 0) {
            mProcVar0 = mProcVar0 - 1;
        } else if (cM_rnd() < 0.05f) {
            LK_voiceStart(38);
            mProcVar0 = 20;
        }
    }
    if (gabi::load<u8>(dComIfGp_ea() + 0x5292) == 0 &&
        gabi::load<u16>(gabi::ea(this) + 0x5848) == 0x126 /* dRes_INDEX_LKANM_BCK_WAITS_e */ &&
        gabi::load<u16>(gabi::ea(this) + 0x5888) == 0xFFFF /* checkNoUpperAnime() */ &&
        gabi::load<u8>(0x101CEF1A) == 0 /* daPy_matAnm_c::getEyeMoveFlg() */ &&
        m3564.y == 0 && m3564.z == 0 && m3564.x == 0) {
        mProcVar1 = mProcVar1 - 1;
        if (mProcVar1 == 0) {
            procFreeWait_init();
        }
    } else {
        mProcVar1 = (s16)gabi::ftoi(cM_rndF(150.0f) + 300.0f);
    }
    return TRUE;
}
VERIFY(0x024177A8, &daPy_lk_c::procWait);

/* 02418178 */
BOOL daPy_lk_c::procLadderUpEnd_init(int param_0) {
    WWHD_FUNC(0x02418178, BOOL, this, param_0);
    gabi::call(LK_commonProcInit, this, 0x39 /* daPyProc_LADDER_UP_END_e */);
    int anm = param_0 != 0 ? 0x86 /* ANM_LADDERUPEDL */ : 0x85 /* ANM_LADDERUPEDR */;
    gabi::call(LK_setSingleMoveAnime, this, anm, 0.9f, 0.0f, -1, 3.0f); /* HD: HIO folded */
    speedF = 0.0f;
    gravity = 0.0f;
    m34C2 = 7;
    speed.y = 0.0f;
    mNormalSpeed = 0.0f;
    dComIfGp_onPlayerStatus0_l(0x2000000 /* daPyStts0_UNK2000000_e */);
    return TRUE;
}
VERIFY(0x02418178, &daPy_lk_c::procLadderUpEnd_init);

/* 024184C8 */
void daPy_lk_c::procClimbUpStart_init_sub() {
    WWHD_FUNC(0x024184C8, void, this);
    gabi::call(LK_setSingleMoveAnime, this, 0x84 /* ANM_LADDERUPST */, 1.0f, 0.0f, -1, 1.0f); /* HD: HIO folded */
    mProcVar6 = 1;
    m34C2 = 4;
    dComIfGp_onPlayerStatus1_l(0x10000 /* daPyStts1_UNK10000_e */);
}
VERIFY(0x024184C8, &daPy_lk_c::procClimbUpStart_init_sub);

/* 024186A4 */
void daPy_lk_c::procLadderUpStart_init_sub() {
    WWHD_FUNC(0x024186A4, void, this);
    gabi::call(LK_setSingleMoveAnime, this, 0x84 /* ANM_LADDERUPST */, 1.0f, 0.0f, -1, 1.0f); /* HD: HIO folded */
    mProcVar6 = 1;
    m34C2 = 4;
    dComIfGp_onPlayerStatus0_l(0x2000000 /* daPyStts0_UNK2000000_e */);
}
VERIFY(0x024186A4, &daPy_lk_c::procLadderUpStart_init_sub);

/* 024188F0 */
void daPy_lk_c::procLadderDownStart_init_sub() {
    WWHD_FUNC(0x024188F0, void, this);
    gabi::call(LK_setSingleMoveAnime, this, 0x87 /* ANM_LADDERDWST */, 1.3f, 0.0f, -1, 3.0f); /* HD: HIO folded */
    mProcVar6 = 1;
    m34C2 = 4;
    dComIfGp_onPlayerStatus0_l(0x2000000 /* daPyStts0_UNK2000000_e */);
    gabi::call(LK_setOldRootQuaternion, this, 0, -0x8000, 0);
    shape_angle.y = (s16)(shape_angle.y - 0x8000);
    current.angle.y = shape_angle.y;
}
VERIFY(0x024188F0, &daPy_lk_c::procLadderDownStart_init_sub);

/* 02418D58 */
BOOL daPy_lk_c::procVerticalJump_init() {
    WWHD_FUNC(0x02418D58, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x2A /* daPyProc_VERTICAL_JUMP_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x4C /* ANM_VJMP */, 1.0f, 0.0f, 0xD, 1.0f); /* HD: HIO folded */
    mProcVar0 = 0;
    mProcVar2 = (s16)(m352C + 0x8000);
    mProcVar6 = mFrontWallType == 9 ? 1 : 0;
    mNormalSpeed = 0.0f;
    speed.y = 0.0f;
    return TRUE;
}
VERIFY(0x02418D58, &daPy_lk_c::procVerticalJump_init);

/* 02418008 */
BOOL daPy_lk_c::procClimbMoveUpDown_init(int param_0) {
    WWHD_FUNC(0x02418008, BOOL, this, param_0);
    f32 dVar3 = getLadderMoveAnmSpeed();
    gabi::call(LK_commonProcInit, this, 0x3F /* daPyProc_CLIMB_MOVE_UP_DOWN_e */);
    current.angle.y = shape_angle.y;
    if (mDirection == 1 /* DIR_BACKWARD */) {
        dVar3 = -dVar3;
    }
    int anm;
    if (param_0 != 0) {
        mProcVar6 = 0;
        anm = mDirection == 0 /* DIR_FORWARD */ ? 0x8B /* ANM_LADDERLTOR */ : 0x8A /* ANM_LADDERRTOL */;
    } else {
        mProcVar6 = 1;
        anm = mDirection == 0 ? 0x8A : 0x8B;
    }
    gabi::call(LK_setSingleMoveAnime, this, anm, dVar3, 0.0f, -1, 0.0f); /* HD: HIO folded */
    if (mDirection == 0) {
        gabi::call(LK_setTextureAnime, this, 10, 0);
    } else {
        gabi::call(LK_setTextureAnime, this, 11, 0);
    }
    gravity = 0.0f;
    speed.y = 0.0f;
    mProcVar0 = 1;
    speedF = 0.0f;
    mNormalSpeed = 0.0f;
    m34C2 = 7;
    dComIfGp_onPlayerStatus1_l(0x10000 /* daPyStts1_UNK10000_e */);
    return TRUE;
}
VERIFY(0x02418008, &daPy_lk_c::procClimbMoveUpDown_init);

/* 02418530 */
BOOL daPy_lk_c::procClimbUpStart_init() {
    WWHD_FUNC(0x02418530, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x3D /* daPyProc_CLIMB_UP_START_e */);
    speed.y = 0.0f;
    gravity = 0.0f;
    mNormalSpeed = 0.0f;
    speedF = 0.0f;
    if (mEquipItem == 0x100 /* daPyItem_NONE_e */) {
        if (gabi::call<BOOL>(LK_checkEquipAnime, this)) {
            gabi::call(LK_resetActAnimeUpper, this, 2 /* UPPER_MOVE2_e */, -1.0f);
        }
        procClimbUpStart_init_sub();
    } else {
        gabi::call(LK_setBlendMoveAnime, this, 2.4f); /* HD: m_HIO->mBasic.m.field_0xC folded */
        gabi::call(LK_setAnimeUnequip, this);
        mProcVar6 = 0;
        m3598 = 0.0f;
    }
    u16 a = (u16)m352C;
    f32 fVar2 = cM_ssin(a);
    f32 fVar1 = cM_scos(a);
    current.pos.x = gabi::fmadds(25.0f, fVar2, m3724.x);
    current.pos.z = gabi::fmadds(25.0f, fVar1, m3724.z);
    shape_angle.y = (s16)(a + 0x8000);
    current.angle.y = shape_angle.y;
    mProcVar2 = 0;
    return TRUE;
}
VERIFY(0x02418530, &daPy_lk_c::procClimbUpStart_init);

/* 02418230 */
BOOL daPy_lk_c::procLadderMove_init(int param_0, int direction, cXyz* param_2) {
    WWHD_FUNC(0x02418230, BOOL, this, param_0, direction, param_2);
    int uVar3 = (mModeFlg & 2 /* ModeFlg_MIDAIR */) ? 1 : 0;
    f32 dVar4 = getLadderMoveAnmSpeed();
    gabi::call(LK_commonProcInit, this, 0x3C /* daPyProc_LADDER_MOVE_e */);
    f32 y = param_2->y;
    if (uVar3 == 0) {
        if (direction == 0 /* DIR_FORWARD */) {
            m370C.y = y + 37.5f;
        } else {
            m370C.y = y - 37.5f;
            dVar4 = -dVar4;
        }
    } else {
        m370C.y = y;
    }
    m370C.x = param_2->x;
    m370C.z = param_2->z;
    int anm;
    if (param_0 != 0) {
        if (uVar3 != 0) {
            mProcVar6 = 1;
        } else {
            mProcVar6 = 0;
            u16 a = (u16)shape_angle.y;
            f32 c = cM_scos(a);
            m370C.x = m370C.x - (c + c);
            m370C.z = gabi::fnmsubs(-2.0f, cM_ssin(a), m370C.z);
        }
        anm = dVar4 < 0.0f ? 0x8A /* ANM_LADDERRTOL */ : 0x8B /* ANM_LADDERLTOR */;
    } else {
        mProcVar6 = 1;
        anm = dVar4 < 0.0f ? 0x8B : 0x8A;
        u16 a = (u16)shape_angle.y;
        f32 c = cM_scos(a);
        m370C.x = m370C.x + (c + c);
        m370C.z = gabi::fmadds(-2.0f, cM_ssin(a), m370C.z);
    }
    gabi::call(LK_setSingleMoveAnime, this, anm, dVar4, 0.0f, -1, 0.0f); /* HD: HIO folded */
    mDirection = direction;
    if (direction == 0) {
        gabi::call(LK_setTextureAnime, this, 10, 0);
    } else {
        gabi::call(LK_setTextureAnime, this, 11, 0);
    }
    gravity = 0.0f;
    speed.y = 0.0f;
    mProcVar0 = 1;
    speedF = 0.0f;
    mNormalSpeed = 0.0f;
    m34C2 = 7;
    dComIfGp_onPlayerStatus0_l(0x2000000 /* daPyStts0_UNK2000000_e */);
    return TRUE;
}
VERIFY(0x02418230, &daPy_lk_c::procLadderMove_init);

/* 0241870C */
BOOL daPy_lk_c::procLadderUpStart_init() {
    WWHD_FUNC(0x0241870C, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x38 /* daPyProc_LADDER_UP_START_e */);
    speed.y = 0.0f;
    gravity = 0.0f;
    mNormalSpeed = 0.0f;
    speedF = 0.0f;
    if (mEquipItem == 0x100 /* daPyItem_NONE_e */) {
        if (gabi::call<BOOL>(LK_checkEquipAnime, this)) {
            gabi::call(LK_resetActAnimeUpper, this, 2 /* UPPER_MOVE2_e */, -1.0f);
        }
        procLadderUpStart_init_sub();
    } else {
        gabi::call(LK_setBlendMoveAnime, this, 2.4f); /* HD: m_HIO->mBasic.m.field_0xC folded */
        gabi::call(LK_setAnimeUnequip, this);
        m3598 = 0.0f;
        mProcVar6 = 0;
    }
    u16 a = (u16)m352C;
    f32 fVar1 = cM_ssin(a);
    f32 fVar2 = cM_scos(a);
    f32 x = m3724.x;
    f32 y = m3724.y;
    f32 z = m3724.z;
    current.pos.x = gabi::fmadds(25.0f, fVar1, x);
    current.pos.y = y;
    current.pos.z = gabi::fmadds(25.0f, fVar2, z);
    shape_angle.y = (s16)(a + 0x8000);
    current.angle.y = shape_angle.y;
    m370C.x = gabi::fmadds(20.5f, fVar1, x) - fVar2;
    m370C.y = y + 37.5f;
    m370C.z = gabi::fmadds(20.5f, fVar2, z) + fVar1;
    mProcVar2 = 0;
    return TRUE;
}
VERIFY(0x0241870C, &daPy_lk_c::procLadderUpStart_init);

/* 02418980 */
BOOL daPy_lk_c::procLadderDownStart_init() {
    WWHD_FUNC(0x02418980, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x3A /* daPyProc_LADDER_DOWN_START_e */);
    s16 a0 = m352C;
    gravity = 0.0f;
    speed.y = 0.0f;
    speedF = 0.0f;
    shape_angle.y = (s16)(a0 + 0x8000);
    current.angle.y = a0;
    mNormalSpeed = 0.0f;
    if (mEquipItem == 0x100 /* daPyItem_NONE_e */) {
        if (gabi::call<BOOL>(LK_checkEquipAnime, this)) {
            gabi::call(LK_resetActAnimeUpper, this, 2 /* UPPER_MOVE2_e */, -1.0f);
        }
        procLadderDownStart_init_sub();
    } else {
        gabi::call(LK_setBlendMoveAnime, this, 2.4f); /* HD: m_HIO->mBasic.m.field_0xC folded */
        gabi::call(LK_setAnimeUnequip, this);
        mProcVar6 = 0;
        m3598 = 0.0f;
    }
    u16 a = (u16)m352C;
    f32 fVar1 = cM_ssin(a);
    f32 fVar2 = cM_scos(a);
    f32 x = m3724.x;
    f32 y = m3724.y;
    f32 z = m3724.z;
    current.pos.y = y;
    current.pos.x = gabi::fmadds(25.0f, fVar1, x);
    current.pos.z = gabi::fmadds(25.0f, fVar2, z);
    m370C.x = gabi::fnmsubs(30.5f, fVar1, x) + fVar2;
    m370C.y = y - 112.5f;
    m370C.z = gabi::fnmsubs(30.5f, fVar2, z) - fVar1;
    return TRUE;
}
VERIFY(0x02418980, &daPy_lk_c::procLadderDownStart_init);

/* 02418B48 */
BOOL daPy_lk_c::procHangWallCatch_init() {
    WWHD_FUNC(0x02418B48, BOOL, this);
    gabi::Local<cXyz> local_34;
    gabi::Local<cXyz> local_28;
    local_34->set(m3724.x, 0.0f, m3724.z);
    local_28->set(current.pos.x, 0.0f, current.pos.z);
    f32 dist = std_sqrtf(gabi::call<f32>(0x028E8DE8 /* PSVECSquareDistance */, local_34.get(), local_28.get()));
    if (dist > LK_FIELD(f32, 0x780) /* mAcchCir[0].GetWallR() */ + 20.0f) {
        return FALSE;
    }
    u16 a = (u16)m352C;
    f32 x = gabi::fnmsubs(1.5f, cM_ssin(a), m3724.x);
    f32 y = m3724.y + 10.0f;
    f32 z = gabi::fnmsubs(1.5f, cM_scos(a), m3724.z);
    mGndChkPos.y = y; /* mGndChk.SetPos(&local_1c) */
    mGndChkPos.x = x;
    mGndChkPos.z = z;
    f64 gy = gabi::call<f64>(0x02008974 /* cBgS::GroundCross */, dComIfG_Bgsp(), mGndChk);
    if (std::fabs((f32)(gy - (f64)(f32)m3724.y)) > 30.1f) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x31 /* daPyProc_HANG_WALL_CATCH_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x4E /* ANM_VJMPCHB */, 0.8f, 0.0f, 5, 2.5f); /* HD: HIO folded */
    speed.y = 0.0f;
    mNormalSpeed = 0.0f;
    current.pos.y = (f32)gy;
    shape_angle.y = (s16)(m352C + 0x8000);
    current.angle.y = shape_angle.y;
    current.pos.z = z;
    current.pos.x = x;
    dComIfGp_onPlayerStatus0_l(0x100 /* daPyStts0_HANG_e */);
    return TRUE;
}
VERIFY(0x02418B48, &daPy_lk_c::procHangWallCatch_init);

/* 02417D30 */
BOOL daPy_lk_c::procHangStart_init() {
    WWHD_FUNC(0x02417D30, BOOL, this);
    if (mCurProc != 0x7F /* daPyProc_ROPE_UP_HANG_e */ && mCurProc != 0x7A /* daPyProc_ROPE_UP_e */) {
        gabi::Local<cXyz> local_34;
        gabi::Local<cXyz> local_28;
        local_34->set(m3724.x, 0.0f, m3724.z);
        local_28->set(current.pos.x, 0.0f, current.pos.z);
        f32 dist = std_sqrtf(gabi::call<f32>(0x028E8DE8 /* PSVECSquareDistance */, local_34.get(), local_28.get()));
        if (dist > LK_FIELD(f32, 0x780) /* mAcchCir[0].GetWallR() */ + 20.0f) {
            return FALSE;
        }
    }
    s32 sVar3 = 0;
    f64 y;
    f32 x, z;
    if (mCurProc != 0x7F && mCurProc != 0x7A) {
        u16 a = (u16)m352C;
        f32 y0 = m3724.y + 10.0f;
        x = gabi::fnmsubs(1.5f, cM_ssin(a), m3724.x);
        z = gabi::fnmsubs(1.5f, cM_scos(a), m3724.z);
        mGndChkPos.y = y0; /* mGndChk.SetPos(&local_1c) */
        mGndChkPos.x = x;
        mGndChkPos.z = z;
        y = gabi::call<f64>(0x02008974 /* cBgS::GroundCross */, dComIfG_Bgsp(), mGndChk);
        if (std::fabs((f32)(y - (f64)(f32)m3724.y)) > 30.1f) {
            return FALSE;
        }
    } else {
        u16 a = (u16)m352C;
        x = gabi::fnmsubs(2.25f, cM_ssin(a), m3724.x);
        y = m3724.y + 10.0f;
        z = gabi::fnmsubs(2.25f, cM_scos(a), m3724.z);
        /* !strcmp(dComIfGp_getStartStageName(), "GanonK") */
        u32 p = dComIfGp_ea() + 0x5134;
        u32 q = 0x10035F30;
        for (;;) {
            u8 c0 = gabi::load<u8>(p++);
            u8 c1 = gabi::load<u8>(q++);
            if (c0 != c1) {
                break;
            }
            if (c0 == 0) {
                sVar3 = 1;
                break;
            }
        }
    }
    gabi::call(LK_commonProcInit, this, 0x2B /* daPyProc_HANG_START_e */);
    mProcVar6 = sVar3;
    gabi::call(LK_setSingleMoveAnime, this, 0x4D /* ANM_VJMPCHA */, 0.8f, 1.0f, 7, 2.0f); /* HD: HIO folded */
    current.pos.x = x;
    current.pos.y = (f32)y;
    mNormalSpeed = 0.0f;
    current.angle.y = (s16)(m352C + 0x8000);
    speed.y = 0.0f;
    shape_angle.y = current.angle.y;
    current.pos.z = z;
    dComIfGp_onPlayerStatus0_l(0x100 /* daPyStts0_HANG_e */);
    mHangGroundH = LK_FIELD(f32, 0x8A0); /* mAcch.GetGroundH() */
    return TRUE;
}
VERIFY(0x02417D30, &daPy_lk_c::procHangStart_init);

/* 02419514 */
BOOL daPy_lk_c::procIceSlipFall_init() {
    WWHD_FUNC(0x02419514, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x9E /* daPyProc_ICE_SLIP_FALL_e */);
    s16 a = cM_atan2s(-m36A0.x, -m36A0.z);
    int direction = gabi::call<int>(LK_getDirectionFromAngle, this, (s32)(s16)(a - shape_angle.y));
    if (direction == 0 /* DIR_FORWARD */) {
        mProcVar6 = 0x5C; /* mDamageAnm = ANM_DAMFB */
        mProcVar2 = 0x3FFF;
        mProcVar3 = 1;
    } else if (direction == 3 /* DIR_RIGHT */) {
        mProcVar6 = 0x59; /* ANM_DAMFL */
        mProcVar2 = 0x3FFF;
        mProcVar3 = 0;
    } else if (direction == 2 /* DIR_LEFT */) {
        mProcVar6 = 0x5A; /* ANM_DAMFR */
        mProcVar2 = -0x3FFF;
        mProcVar3 = 0;
    } else {
        mProcVar6 = 0x5B; /* ANM_DAMFF */
        mProcVar2 = -0x3FFF;
        mProcVar3 = 1;
    }
    gabi::call(LK_setSingleMoveAnime, this, (s32)mProcVar6, 1.0f, 0.0f, -1, 3.0f); /* HD: HIO folded */
    mNormalSpeed = 0.0f;
    speed.y = 12.0f;
    LK_FIELD(s16, 0x3D4) = 0; /* mBodyAngle */
    LK_FIELD(s16, 0x3D2) = 0;
    mBodyAngleX = 0;
    LK_voiceStart(36);
    return TRUE;
}
VERIFY(0x02419514, &daPy_lk_c::procIceSlipFall_init);

/* 024197A0 */
BOOL daPy_lk_c::checkIceSlipFall() {
    WWHD_FUNC(0x024197A0, BOOL, this);
    s16 sVar3 = cM_atan2s(m36A0.x, m36A0.z);
    f32 fVar1;
    f32 fVar2;
    if (m34C3 == 1) {
        fVar1 = 15.0f;
        fVar2 = 169.0f; /* SQUARE(13.0f) */
    } else {
        fVar1 = 10.0f;
        fVar2 = 49.0f; /* SQUARE(7.0f) */
    }
    if (cLib_distanceAngleS(sVar3, current.angle.y) > 0x7000 && !(mNormalSpeed < fVar1)) {
        gabi::Local<cXyz> xz; /* m36A0.abs2XZ() */
        xz->x = m36A0.x;
        xz->y = 0.0f;
        xz->z = m36A0.z;
        if (!(PSVECSquareMag(xz) < fVar2)) {
            mProcVar0 = mProcVar0 - 1;
            if (mProcVar0 == 0) {
                return procIceSlipFall_init();
            }
            return FALSE;
        }
    }
    mProcVar0 = 20;
    return FALSE;
}
VERIFY(0x024197A0, &daPy_lk_c::checkIceSlipFall);

/* 024198D4 */
BOOL daPy_lk_c::procMove() {
    WWHD_FUNC(0x024198D4, BOOL, this);
    gabi::call(LK_setSpeedAndAngleNormal, this, 0xBB8); /* HD: m_HIO->mMove.m.field_0x0 folded */
    if (!gabi::call<BOOL>(LK_checkNextMode, this, 0) && !gabi::call<BOOL>(LK_changeFrontWallTypeProc, this) &&
        !checkIceSlipFall()) {
        if (LK_demoMode() == 2 /* daPy_demo_c::DEMO_N_WALK_e */) {
            f32 lim = LK_FIELD(f32, 0x3C4) /* mMaxNormalSpeed */ * 0.5f;
            if (mNormalSpeed > lim) {
                mNormalSpeed = lim;
            }
            /* HD: walking at the height 1750..1755 is faster in room 0 of MajyuE and in room 1 of the sea */
            f32 y = current.pos.y;
            if (1750.0f < y && y < 1755.0f) {
                if (lk_isStartStage(0x10035F50 /* "MajyuE" */) && current.roomNo == 0) {
                    mNormalSpeed = mNormalSpeed * 1.5f;
                }
                if (lk_isStartStage(0x10035F4C /* "sea" */) && current.roomNo == 1) {
                    mNormalSpeed = mNormalSpeed * 1.5f;
                }
            }
        }
        gabi::call(LK_setBlendMoveAnime, this, -1.0f);
    }
    return TRUE;
}
VERIFY(0x024198D4, &daPy_lk_c::procMove);

/* 0241650C */
void daPy_lk_c::setSpeedAndAngleNormal(s16 param_1) {
    WWHD_FUNC(0x0241650C, void, this, param_1);
    f32 dVar9 = 0.0f;
    if (mStickDistance > 0.05f) {
        f32 st = mStickDistance;
        f32 dVar11 = st * st;
        BOOL heavy = gabi::call<BOOL>(LK_checkHeavyStateOn, this);
        f32 wear = LK_FIELD(f32, 0x3CC);
        if (heavy) {
            dVar11 *= 4.0f; /* 1.0f / SQUARE(m_HIO->mMove.m.field_0x80) */
        }
        if (wear < 0.0f /* checkGrabWear() */) {
            dVar11 *= 1.5624999f; /* 1.0f / SQUARE(m_HIO->mMove.m.field_0x74) */
        }
        bool turn = false;
        if (!lk_checkAttentionLock(this) && cLib_distanceAngleS(m34E8, current.angle.y) > 0x7800 &&
            mCurProc != 0x18 /* daPyProc_MOVE_TURN_e */) {
            turn = true;
        }
        bool cosPart = true;
        if (turn) {
            if (mModeFlg & 1 /* ModeFlg_00000001 */) {
                return;
            }
            if (mCurProc == 6 /* daPyProc_MOVE_e */) {
                if (speedF / LK_FIELD(f32, 0x3C4) > 0.6f) { /* m_HIO->mSlip.m.field_0x4 */
                    if (gabi::call<int>(LK_getDirectionFromAngle, this, (s32)(s16)(m34EA - m34DC)) == 1 /* DIR_BACKWARD */) {
                        return;
                    }
                    cosPart = false; /* bVar2: dVar9 = 0.0f */
                } else {
                    cLib_addCalcAngleS(&current.angle.y, m34E8, 5, param_1, 100); /* HD: HIO folded */
                    return;
                }
            } else {
                cLib_addCalcAngleS(&current.angle.y, m34E8, 5, param_1, 100);
            }
        } else {
            s16 sVar6;
            s16 sVar7;
            if (gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0 /* dComIfGp_event_runCheck() */) {
                sVar6 = param_1;
                sVar7 = 100;
            } else {
                sVar6 = (s16)gabi::ftoi((f32)param_1 * dVar11);
                if (sVar6 < 10) {
                    sVar6 = 10;
                }
                sVar7 = (s16)gabi::ftoi(100.0f * dVar11);
                if (!(sVar7 > 0)) {
                    sVar7 = 1;
                }
            }
            cLib_addCalcAngleS(&current.angle.y, m34E8, 5, sVar6, sVar7);
        }
        if (cosPart) {
            f32 c = cM_scos((u16)(m34E8 - current.angle.y));
            f32 half = LK_FIELD(f32, 0x3C4) * 0.5f;
            if (mNormalSpeed > half) {
                if (c < 0.7f) {
                    c = 0.7f;
                }
            } else {
                c = c >= 0.0f ? c : 0.0f; /* fsel */
            }
            if (gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0 /* dComIfGp_event_runCheck() */) {
                dVar9 = (3.5f * mStickDistance) * c; /* HD: m_HIO->mMove.m.field_0x14 folded */
            } else {
                f32 r = mNormalSpeed / LK_FIELD(f32, 0x3C4);
                f32 dVar10 = gabi::fnmsubs(std::fabs(r), 0.5f, 0.5f); /* 0.5f - 0.5f * abs(..) */
                BOOL heavy2 = gabi::call<BOOL>(LK_checkHeavyStateOn, this);
                f32 wear2 = LK_FIELD(f32, 0x3CC);
                f32 st2 = mStickDistance;
                if (heavy2) {
                    dVar10 *= 0.5f;
                }
                if (wear2 < 0.0f) {
                    dVar10 *= 0.8f;
                }
                if (st2 > dVar10) {
                    dVar9 = (3.5f * dVar11) * c;
                }
            }
        }
    }
    if (!lk_checkAttentionLock(this) && mCurProc != 0x18 /* daPyProc_MOVE_TURN_e */ && mStickDistance > 0.05f) {
        s16 sVar6 = shape_angle.y;
        cLib_addCalcAngleS(&shape_angle.y, m34E8, 5, (s16)(param_1 << 1), 200);
        s16 cur = current.angle.y;
        s32 temp = (s16)(sVar6 - cur);
        s32 temp2 = (s16)(shape_angle.y - cur);
        if (!(temp * temp2 > 0)) {
            shape_angle.y = cur;
        }
    }
    gabi::call(LK_setNormalSpeedF, this, dVar9, 0.6f, 2.5f, 1.8f); /* HD: HIO folded */
}
VERIFY(0x0241650C, &daPy_lk_c::setSpeedAndAngleNormal);

/* 02418E00 */
BOOL daPy_lk_c::changeFrontWallTypeProc() {
    WWHD_FUNC(0x02418E00, BOOL, this);
    gabi::call(LK_setFrontWallType, this);
    f32 st = mStickDistance;
    u32 modeFlg = mModeFlg;
    s16 sVar5 = m3544;
    m3544 = 0;
    u32 midair = modeFlg & 2; /* ModeFlg_MIDAIR */
    if (!(st > 0.05f) && midair == 0) {
        return FALSE;
    }
    u8 type = mFrontWallType;
    if (midair != 0) {
        if (type == 7) {
            LK_voiceStart(10);
            return procHangStart_init();
        }
        if (noResetFlg1() & 0x2000000 /* daPyFlg1_VINE_CATCH */) {
            return FALSE;
        }
        if (type == 3) {
            gabi::call(LK_deleteEquipItem, this, 1);
            shape_angle.y = (s16)(m352C + 0x8000);
            procClimbMoveUpDown_init(1);
            m35E0 = 43.67353f;
            lk_initOldFrameMorf(this, 5.0f, 0, 0x2A);
            mFrameCtrlUnder[0].setRate(0.0f);
            return TRUE;
        }
        if (type == 4) {
            f32 y = current.pos.y;
            s16 a0 = m352C;
            u16 a = (u16)a0;
            int iVar8 = (s32)((u32)gabi::ftoi((y - m3724.y) / 37.5f) - 1u); /* wraps like the original */
            f32 fVar2 = cM_ssin(a);
            f32 fVar3 = cM_scos(a);
            if (iVar8 < 1) {
                return gabi::call<BOOL>(LK_procFall_init, this, 1, 6.0f); /* HD: m_HIO->mWallCatch.m.field_0x54 folded */
            }
            f32 z = gabi::fmadds(20.5f, fVar3, m3724.z) + fVar2;
            f32 x = gabi::fmadds(20.5f, fVar2, m3724.x) - fVar3;
            shape_angle.y = (s16)(a0 + 0x8000);
            f32 ny = gabi::fmadds(37.5f, (f32)iVar8, m3724.y);
            current.angle.y = (s16)(a0 + 0x8000);
            current.pos.x = x;
            current.pos.y = ny;
            current.pos.z = z;
            gabi::call(LK_deleteEquipItem, this, 1);
            f32 lim = m35F8 - 150.0f;
            m35E0 = 43.67353f;
            if (current.pos.y > lim) {
                current.pos.y = lim;
                m370C.x = current.pos.x; /* m370C = current.pos */
                m370C.y = lim;
                m370C.z = current.pos.z;
                procLadderUpEnd_init(1);
                lk_initOldFrameMorf(this, 5.0f, 0, 0x2A);
            } else {
                procLadderMove_init(1, 0, &current.pos);
                mFrameCtrlUnder[0].setRate(0.0f);
                lk_initOldFrameMorf(this, 5.0f, 0, 0x2A);
            }
            return TRUE;
        }
        return FALSE;
    }
    /* HD: no daPyRFlg0_UNK8 check here; it lengthens the wall catch delay below */
    if (type == 3) {
        if (noResetFlg1() & 0x2000000 /* daPyFlg1_VINE_CATCH */) {
            return FALSE;
        }
        if (modeFlg & 0x40000 /* ModeFlg_SWIM */) {
            f32 y = current.pos.y - 60.0f;
            shape_angle.y = (s16)(m352C + 0x8000);
            current.pos.y = y;
            procClimbMoveUpDown_init(1);
            m35E0 = 43.67353f;
            lk_initOldFrameMorf(this, 5.0f, 0, 0x2A);
            mFrameCtrlUnder[0].setRate(0.0f);
            return TRUE;
        }
        return procClimbUpStart_init();
    }
    if (type == 4) {
        if (noResetFlg1() & 0x2000000) {
            return FALSE;
        }
        if (modeFlg & 0x40000 /* ModeFlg_SWIM */) {
            f32 ly = m3724.y;
            int iVar8 = (s32)((u32)gabi::ftoi((current.pos.y - ly) / 37.5f) - 2u);
            u16 a = (u16)m352C;
            f32 fVar3 = cM_ssin(a);
            f32 fVar2 = cM_scos(a);
            f32 x = gabi::fmadds(20.5f, fVar3, m3724.x) - fVar2;
            f32 y = gabi::fmadds(37.5f, (f32)iVar8, ly);
            f32 z = gabi::fmadds(20.5f, fVar2, m3724.z) + fVar3;
            shape_angle.y = (s16)(a + 0x8000);
            current.angle.y = (s16)(a + 0x8000);
            f32 d = mWaterY - y;
            current.pos.x = x;
            current.pos.y = y;
            current.pos.z = z;
            while (d > 90.0f) { /* m_HIO->mSwim.m.field_0x24 */
                y = y + 37.5f;
                d = mWaterY - y;
                current.pos.y = y;
            }
            procLadderMove_init(1, 0, &current.pos);
            m35E0 = 43.67353f;
            lk_initOldFrameMorf(this, 5.0f, 0, 0x2A);
            return TRUE;
        }
        return procLadderUpStart_init();
    }
    if (type == 5) {
        if (noResetFlg1() & 0x2000000) {
            return FALSE;
        }
        return procLadderDownStart_init();
    }
    type = mFrontWallType;
    s16 cnt = (s16)(sVar5 + 1);
    m3544 = cnt;
    if (type == 6) {
        if (cnt > 4) { /* m_HIO->mSmallJump.m.field_0x2 */
            return procSmallJump_init(0);
        }
        return FALSE;
    }
    /* HD: the wall catch waits 14 frames instead of 7 while daPyRFlg0_UNK8 is set */
    if (resetFlg0() & 8) {
        if (!(cnt > 0xE)) {
            return FALSE;
        }
    } else if (!(cnt > 7)) {
        return FALSE;
    }
    if (type == 7) {
        return procHangWallCatch_init();
    }
    if (type == 8 || type == 9) {
        return procVerticalJump_init();
    }
    m3544 = 0;
    return FALSE;
}
VERIFY(0x02418E00, &daPy_lk_c::changeFrontWallTypeProc);

/* 02412948 */
BOOL daPy_lk_c::procTactPlayEnd_init(int r30) {
    WWHD_FUNC(0x02412948, BOOL, this, r30);
    int r28 = gabi::call<int>(LK_checkShipRideUseItem, this, 0);
    gabi::call(LK_commonProcInit, this, 0x9C /* daPyProc_TACT_PLAY_END_e */);
    dComIfGp_onPlayerStatus1_l(1 /* daPyStts1_WIND_WAKER_CONDUCT_e */);
    gabi::call(LK_initShipRideUseItem, this, r28, 2);
    mProcVar6 = r30;
    mNormalSpeed = 0.0f;
    gabi::call(LK_setBlendMoveAnime, this, 2.4f); /* HD: m_HIO->mBasic.m.field_0xC folded */
    if (r30 == -1) {
        u32 body = gabi::call<u32>(0x024F8044 /* dCam_getBody */);
        gabi::call(0x0253E860 /* dCamera_c::EndEventCamera */, body, gabi::load<u32>(gabi::ea(this) + 4) /* fopAcM_GetID(this) */);
        fopAc_ac_c* partner = fopAcM_SearchByID(mTactZevPartnerId);
        gabi::call(0x025D79F4 /* fopAcM_orderChangeEvent */, this, partner, (u32)m3494, 0, 0xFFFF);
        LK_FIELD(u16, 0x420) = 2; /* mDemo.setSystemDemoType() */
    } else if (r30 == 0) {
        u32 body = gabi::call<u32>(0x024F8044);
        gabi::call(0x0253E860, body, gabi::load<u32>(gabi::ea(this) + 4));
        /* fopAcM_create(fpcNm_WBIRD_e, NULL, &current.pos, dComIfGp_roomControl_getStayNo()) */
        fopAcM_create(0xC4, 0, &current.pos, gabi::load<s8>(0x1047E6C8), nullptr, nullptr, -1, 0);
    } else if (r30 == 2) {
        u32 body = gabi::call<u32>(0x024F8044);
        gabi::call(0x0253E860, body, gabi::load<u32>(gabi::ea(this) + 4));
        /* dComIfGp_event_setTalkPartner(dComIfGp_getCb1Player()) */
        u32 cb1 = gabi::load<u32>(dComIfGp_ea() + 0x5B38);
        u32 evt = dComIfGp_ea() + 0x51D0;
        u32 pid = gabi::call<u32>(0x0253F124 /* dEvt_control_c::getPId */, evt, cb1);
        gabi::store<u32>(evt + 0xCC, pid);
        u32 cb1b = gabi::load<u32>(dComIfGp_ea() + 0x5B38);
        gabi::call(0x025D79F4 /* fopAcM_orderChangeEvent */, this, cb1b, 0x101CEC98u /* l_tact_event_label */, 0, 0xFFFF);
        gabi::call(0x025E1988 /* seStartSystem */, 0x885 /* JA_SE_CTRL_LINK_TO_NPC */);
    } else if (r30 == 5) {
        u32 body = gabi::call<u32>(0x024F8044);
        gabi::call(0x0253E860, body, gabi::load<u32>(gabi::ea(this) + 4));
        gabi::call(0x025D78FC /* fopAcM_orderChangeEvent */, this, 0x101CECC0u /* l_tact_night_event_label */, 0, 0xFFFF);
    }
    mProcVar7 = 0;
    mProcVar3 = (s16)(shape_angle.y + 0x4000);
    return TRUE;
}
VERIFY(0x02412948, &daPy_lk_c::procTactPlayEnd_init);

/* daPy_waterDropPcallBack_c (HD: vtable at +0) */
struct daPy_waterDropPcallBack_l {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s32> mbWaterMark;
    /* 0x08 */ u8 mGndChk[0x54]; /* dBgS_ObjGndChk; its position at +0x24 */
};
/* 02414048 */
static void daPy_waterDropPcallBack_execute(daPy_waterDropPcallBack_l* cb, JPABaseEmitter*, void* ptcl) {
    WWHD_FUNC(0x02414048, void, cb, 0, ptcl);
    if (cb->mbWaterMark) {
        u32 p = gabi::ea(ptcl); /* ptcl->getGlobalPosition() (HD +0x28) */
        f32 pz = gabi::load<f32>(p + 0x30);
        f32 py = gabi::load<f32>(p + 0x2C);
        f32 px = gabi::load<f32>(p + 0x28);
        f32 y = py + 25.0f;
        u32 chk = gabi::ea(cb) + 8;
        gabi::store<f32>(chk + 0x2C, pz); /* mGndChk.SetPos(&pos) */
        gabi::store<f32>(chk + 0x24, px);
        gabi::store<f32>(chk + 0x28, y);
        gabi::Local<cXyz> pos;
        pos->x = px;
        pos->z = pz;
        pos->y = y;
        f64 gy = gabi::call<f64>(0x02008974 /* cBgS::GroundCross */, dComIfG_Bgsp(), chk);
        pos->y = (f32)gy;
        if (gy > (f64)py) {
            gabi::call(0x025DADA4 /* fopKyM_create */, 0x1D4 /* fpcNm_WATER_MARK_e */, 1, pos.get(), 0, 0);
        }
    }
}
VERIFY(0x02414048, daPy_waterDropPcallBack_execute);

/* daPy_dmEcallBack_c (HD: vtable at +0) */
struct daPy_dmEcallBack_l {
    /* 0x0 */ be<u32> __vtbl;
    /* 0x4 */ be<u32> mpEmitter;
    /* 0x8 */ be<u32> mpMtx;
};
/* 02415238 */
static void daPy_dmEcallBack_execute(daPy_dmEcallBack_l* cb, JPABaseEmitter* emitter) {
    WWHD_FUNC(0x02415238, void, cb, emitter);
    u32 e = gabi::ea(emitter);
    u32 mtx = cb->mpMtx;
    if (gabi::load<u16>(0x101CEF16) == 0 /* checkFlame(): m_type == 0 */) {
        f32 oz = gabi::load<f32>(e + JPA_TRANS + 8); /* getGlobalTranslation */
        f32 ox = gabi::load<f32>(e + JPA_TRANS + 0);
        f32 ty = gabi::load<f32>(mtx + 0x1C);
        f32 tz = gabi::load<f32>(mtx + 0x2C);
        f32 tx = gabi::load<f32>(mtx + 0x0C);
        if (gabi::load<u8>(e + 0x262) >= 7) { /* HD: mirrored Y (see daPy_followEcallBack_c::execute) */
            ty = -ty;
        }
        gabi::store<f32>(e + JPA_TRANS + 4, ty); /* setGlobalTranslation */
        gabi::store<f32>(e + JPA_TRANS + 8, tz);
        gabi::store<f32>(e + JPA_TRANS + 0, tx);
        u32 m2 = cb->mpMtx;
        f32 x = (gabi::load<f32>(m2 + 0x0C) - ox) * -0.05f;
        f32 z = (gabi::load<f32>(m2 + 0x2C) - oz) * -0.05f;
        f32 sq = gabi::fmadds(x, x, z * z);
        if (sq > 1.0f) {
            f32 distFrac = 1.0f / std_sqrtf(sq);
            z = z * distFrac;
            x = x * distFrac;
        }
        gabi::store<f32>(e + 0x30, z); /* setDirection(x, 0.6f, z) (HD +0x28) */
        gabi::store<f32>(e + 0x2C, 0.6f);
        gabi::store<f32>(e + 0x28, x);
    } else {
        /* setGlobalRTMatrix(mpMtx) */
        gabi::call(0x028249B0 /* JPASetRMtxTVecfromMtx */, mtx, e + JPA_RTMTX, e + JPA_TRANS);
    }
}
VERIFY(0x02415238, daPy_dmEcallBack_execute);

/* 02414E8C */
static void daPy_swimTailEcallBack_execute(daPy_swimTailEcallBack_l* cb, JPABaseEmitter* emitter) {
    WWHD_FUNC(0x02414E8C, void, cb, emitter);
    u32 e = gabi::ea(emitter);
    /* static JGeometry::TVec3<f32> right_dir(-1.0f, 0.0f, 0.0f) (HD: initialised on first use) */
    u32 right_dir = 0x1046D1F8;
    if (gabi::load<u32>(0x1046D1F4) == 0) {
        gabi::store<f32>(right_dir + 4, 0.0f);
        gabi::store<u32>(0x1046D1F4, 1);
        gabi::store<f32>(right_dir + 0, -1.0f);
        gabi::store<f32>(right_dir + 8, 0.0f);
    }
    gabi::Local<GXColor> ambColor;
    gabi::Local<GXColor> difColor;
    gabi::call(0x025602F0 /* dKy_get_seacolor */, ambColor.get(), difColor.get());
    gabi::store<u8>(e + 0x244, ambColor->r); /* setGlobalPrmColor(r, g, b) */
    gabi::store<u8>(e + 0x245, ambColor->g);
    gabi::store<u8>(e + 0x246, ambColor->b);
    if (cb->mbEnd) {
        gabi::Local<be<s16>> sp8;
        *sp8 = (s16)gabi::load<u8>(e + JPA_ALPHA);
        gabi::call(0x0200F564 /* cLib_chaseS */, sp8.get(), 0, 0x14);
        s16 a = *sp8;
        gabi::store<u8>(e + JPA_ALPHA, (u8)a);
        if (a == 0) {
            gabi::store<s32>(e + 0x5C, -1); /* becomeInvalidEmitter() */
            gabi::store<u32>(e + JPA_ECB, 0);
            gabi::store<u32>(e + JPA_STATUS, gabi::load<u32>(e + JPA_STATUS) | 1);
            cb->mpEmitter = 0;
            return;
        }
    }
    if (cb->field_0x20 != 0) {
        f32 x = cb->mPos.x;
        f32 z = cb->mPos.z;
        f32 y = cb->mPos.y;
        if (gabi::load<u8>(e + 0x262) >= 7) { /* HD: mirrored Y */
            y = -y;
        }
        gabi::store<f32>(e + JPA_TRANS + 8, z); /* setGlobalTranslation */
        gabi::store<f32>(e + JPA_TRANS + 0, x);
        gabi::store<f32>(e + JPA_TRANS + 4, y);
        /* setGlobalRotation(TVec3<s16>(0, field_0x20->y, 0)) */
        gabi::call(0x028245AC /* JPAGetXYZRotateMtx */, 0, (s32)gabi::load<s16>(cb->field_0x20 + 2), 0, e + JPA_RTMTX);
    } else {
        gabi::Local<cXyz> sp20;
        u32 sp = gabi::ea(sp20.get());
        fcpy_l(sp + 8, e + JPA_TRANS + 8); /* getGlobalTranslation */
        fcpy_l(sp + 4, e + JPA_TRANS + 4);
        fcpy_l(sp + 0, e + JPA_TRANS + 0);
        gabi::call(0x02414DD8 /* getMaxWaterY */, cb, sp20.get());
        fcpy_l(e + JPA_TRANS + 4, sp + 4); /* setGlobalTranslation */
        fcpy_l(e + JPA_TRANS + 0, sp + 0);
        fcpy_l(e + JPA_TRANS + 8, sp + 8);
        if (gabi::load<u8>(e + 0x262) >= 7) { /* HD: mirrored Y */
            gabi::store<f32>(e + JPA_TRANS + 4, -gabi::load<f32>(e + JPA_TRANS + 4));
        }
        cLib_chaseF(&cb->mSpeedRate, 0.0f, 0.08f);
    }
    gabi::store<f32>(e + 0x70, gabi::fmadds(12.0f, cb->mSpeedRate, 1.0f)); /* setDirectionalSpeed(1.0f + 12.0f * mSpeedRate) */
    if (cb->field_0x05) {
        gabi::store<f32>(e + 0x28, gabi::load<f32>(right_dir + 0)); /* setDirection(right_dir) */
        gabi::store<f32>(e + 0x2C, gabi::load<f32>(right_dir + 4));
        gabi::store<f32>(e + 0x30, gabi::load<f32>(right_dir + 8));
    }
    u32 link = gabi::load<u32>(e + 0x1AC); /* getParticleList()->getFirst() */
    gabi::Local<cXyz> offsetPos;
    while (link != 0) {
        u32 particle = gabi::load<u32>(link + 0);
        u32 next = gabi::load<u32>(link + 0xC);
        /* getOffsetPosition */
        u32 op = gabi::ea(offsetPos.get());
        fcpy_l(op + 0, particle + 0x10);
        fcpy_l(op + 4, particle + 0x14);
        fcpy_l(op + 8, particle + 0x18);
        gabi::call(0x02414DD8 /* getMaxWaterY */, cb, offsetPos.get());
        fcpy_l(particle + 0x10, op + 0); /* setOffsetPosition */
        fcpy_l(particle + 0x14, op + 4);
        fcpy_l(particle + 0x18, op + 8);
        link = next;
    }
}
VERIFY(0x02414E8C, daPy_swimTailEcallBack_execute);

/* 02413E80: HD: the eye texture offset goes to the material's "texmtx1" SRT (found by name) and
 * marks the material's texture matrix dirty, instead of the GameCube loop over 8 texture matrices */
static void daPy_matAnm_calc(void* self, void* mat) {
    WWHD_FUNC(0x02413E80, void, self, mat);
    u32 t = gabi::ea(self);
    u32 m = gabi::ea(mat);
    u32 md = gabi::load<u32>(m);
    s32 off = gabi::load<s32>(md + 0x38);
    u32 nameTab = off != 0 ? md + 0x38 + off : 0;
    s32 idx = gabi::call<s32>(0x027DF9B0 /* JUTNameTab::getIndex */, nameTab, 0x10035ED4u /* "texmtx1" */);
    u32 md2 = gabi::load<u32>(m);
    s32 off2 = gabi::load<s32>(md2 + 0x34);
    u32 e = (off2 != 0 ? md2 + 0x34 + off2 : 0) + idx * 0x14;
    if (gabi::load<s32>(e + 4) >= 0) {
        gabi::store<u16>(m + 4, gabi::load<u16>(m + 4) | 4);
        u32 a = gabi::load<u32>(m + 0xC) + ((idx >> 5) << 2);
        gabi::store<u32>(a, gabi::load<u32>(a) | (1u << (idx & 31)));
        md2 = gabi::load<u32>(m);
    }
    s32 off3 = gabi::load<s32>(md2 + 0x34);
    u32 i2 = gabi::load<u16>(e + 0xC);
    u32 e2 = (off3 != 0 ? md2 + 0x34 + off3 : 0) + i2 * 0x14;
    if (gabi::load<s32>(e2 + 4) >= 0) {
        gabi::store<u16>(m + 4, gabi::load<u16>(m + 4) | 4);
        u32 a = gabi::load<u32>(m + 0xC) + ((i2 >> 5) << 2);
        gabi::store<u32>(a, gabi::load<u32>(a) | (1u << (i2 & 31)));
    }
    u32 srt = gabi::load<u32>(m + 0x28) + gabi::load<u16>(e + 2);
    u8 morf = gabi::load<u8>(0x101CEF1B); /* m_morf_frame */
    if (morf != 0) {
        f32 temp = 1.0f / (f32)(morf + 1);
        f32 inv = 1.0f - temp;
        f32 sx = gabi::load<f32>(srt + 0x10);
        f32 x = gabi::fmadds(gabi::load<f32>(t + 0x6C) /* mOldOffset.x */, inv, sx * temp);
        f32 sy = gabi::load<f32>(srt + 0x14);
        gabi::store<f32>(srt + 0x10, x);
        gabi::store<f32>(srt + 0x14, gabi::fmadds(gabi::load<f32>(t + 0x70), inv, sy * temp));
    } else if (gabi::load<u8>(0x101CEF1A) != 0 /* getEyeMoveFlg() */) {
        fcpy_l(srt + 0x10, t + 0x74); /* mNowOffset */
        fcpy_l(srt + 0x14, t + 0x78);
    }
    fcpy_l(t + 0x6C, srt + 0x10); /* mOldOffset */
    fcpy_l(t + 0x70, srt + 0x14);
}
VERIFY(0x02413E80, daPy_matAnm_calc);

/* HD: the J3D animation objects carry a sead::FixedSafeString<0x20> name at +0xC (inline
 * constructor: buffer at +0xC, length 0x20, terminator at +0x2B; vtables 0x10034B3C, then
 * 0x10034B54 and 0x10034B6C). GHS constructors allocate when called with NULL. */
static inline void lk_fixedSafeString32_ct(u32 s) {
    if (s == 0) {
        s = gabi::ea(operator_new(0x2C));
        if (s == 0) {
            return;
        }
    }
    u32 base = s; /* sead::BufferedSafeString part (allocates 0xC when NULL) */
    if (base == 0) {
        base = gabi::ea(operator_new(0xC));
    }
    if (base != 0) {
        gabi::store<u32>(base + 4, 0x10034B3C);
        gabi::store<u32>(base + 0, s + 0xC);
        gabi::store<u32>(base + 8, 0x20);
        gabi::store<u8>(s + 0x2B, 0);
    }
    u32 buf = gabi::load<u32>(s + 0);
    gabi::store<u32>(s + 4, 0x10034B54);
    gabi::store<u8>(buf, 0);
    gabi::store<u32>(s + 4, 0x10034B6C);
}
/* J3DAnmTransform-like HD base: frame 0.0f at +0, vtable 0x10034B84 at +4, 0 at +8, name at +0xC */
static inline void lk_anmTransformBase_ct(u32 obj) {
    gabi::store<f32>(obj + 0, 0.0f);
    gabi::store<u32>(obj + 4, 0x10034B84);
    gabi::store<u32>(obj + 8, 0);
    lk_fixedSafeString32_ct(obj + 0xC);
}

/* 02410888 */
void daPy_lk_c::createAnimeHeap(JKRSolidHeap** pHeap, int heapType) {
    WWHD_FUNC(0x02410888, void, this, pHeap, heapType);
    u32 heapSize;
    if (heapType == 3 /* HEAP_TYPE_ITEM_ANIME_e */) {
        heapSize = 0x50;
    } else if (heapType == 0 /* HEAP_TYPE_UNDER_UPPER_e */ || heapType == 1 /* HEAP_TYPE_TEXTURE_ANIME_e */) {
        heapSize = 0x40;
    } else {
        heapSize = 0xA0;
    }
    u32 pp = gabi::ea(pHeap);
    u32 heap = gabi::call<u32>(0x025E3630 /* mDoExt_createSolidHeapFromGameToCurrent */, heapSize, 0x20);
    gabi::store<u32>(pp, heap);
    if (heap == 0) {
        JUT_ASSERT_fail(STR(0x10035C18), 0x5E7D, STR(0x10035C4C) /* "*i_heap != (0)" */);
        heap = gabi::load<u32>(pp);
        if (heap == 0) {
            return;
        }
    }
    /* HD: the heap gets a name (+0x10) */
    if (heapType == 3) {
        gabi::store<u32>(heap + 0x10, 0x10035C5C); /* "daPy_lk_c::AnimeHeap TRANS_BAS_ANM" */
        u32 obj = gabi::ea(operator_new(0x3C)); /* new mDoExt_transAnmBas(NULL) */
        if (obj != 0) {
            lk_anmTransformBase_ct(obj);
            gabi::store<u32>(obj + 0x38, 0);
            gabi::store<u32>(obj + 4, 0x10034BE4);
            gabi::call(0x025E37D8 /* mDoExt_restoreCurrentHeap */);
            gabi::call(0x025E3678 /* mDoExt_adjustSolidHeap */, gabi::load<u32>(pp));
            return;
        }
        JUT_ASSERT_fail(STR(0x10035C18), 0x5E84, STR(0x10035C80));
    } else if (heapType == 0) {
        gabi::store<u32>(heap + 0x10, 0x10035C98); /* "daPy_lk_c::AnimeHeap TRANS_ANM" */
        u32 obj = gabi::ea(operator_new(0x38)); /* new J3DAnmTransformKey */
        if (obj != 0) {
            gabi::call(0x028F521C /* memset 0 */, obj, 0x38);
            lk_anmTransformBase_ct(obj);
            gabi::store<u32>(obj + 4, 0x10034BCC);
        } else {
            JUT_ASSERT_fail(STR(0x10035C18), 0x5E8A, STR(0x10035CB8));
        }
    } else if (heapType == 1) {
        gabi::store<u32>(heap + 0x10, 0x10035CCC); /* "daPy_lk_c::AnimeHeap TEX_PATTERN" */
        u32 obj = gabi::ea(operator_new(0x14)); /* new J3DAnmTexPattern */
        if (obj != 0) {
            gabi::store<u32>(obj + 0x10, 0);
            gabi::store<f32>(obj + 0, 0.0f);
            gabi::store<u32>(obj + 4, 0x1016E4EC);
            gabi::store<u32>(obj + 0xC, 0);
            gabi::store<u32>(obj + 8, 0);
            gabi::call(0x025E37D8);
            gabi::call(0x025E3678, gabi::load<u32>(pp));
            return;
        }
        JUT_ASSERT_fail(STR(0x10035C18), 0x5E90, STR(0x10035C2C));
    } else {
        gabi::store<u32>(heap + 0x10, 0x10035BF8); /* "daPy_lk_c::AnimeHeap TEX_SCROLL" */
        u32 obj = gabi::ea(operator_new(0x10)); /* new J3DAnmTextureSRTKey */
        if (obj != 0) {
            gabi::store<u32>(obj + 8, 0);
            gabi::store<u32>(obj + 0xC, 0);
            gabi::store<f32>(obj + 0, 0.0f);
            gabi::store<u32>(obj + 4, 0x1016E4AC);
            gabi::call(0x025E37D8);
            gabi::call(0x025E3678, gabi::load<u32>(pp));
            return;
        }
        JUT_ASSERT_fail(STR(0x10035C18), 0x5E96, STR(0x10035C3C));
    }
    gabi::call(0x025E37D8 /* mDoExt_restoreCurrentHeap */);
    gabi::call(0x025E3678 /* mDoExt_adjustSolidHeap */, gabi::load<u32>(pp));
}
VERIFY(0x02410888, &daPy_lk_c::createAnimeHeap);

/* 02414250: the out-of-line copy of the inline matrix assignment (twelve FPR loads to a stack
 * temporary, then word copies; bit-exact) */
static void lk_mtxCopy(Mtx34* dst, const Mtx34* src) {
    WWHD_FUNC(0x02414250, void, dst, src);
    u32 t[12];
    for (u32 i = 0; i < 12; i++) {
        t[i] = lk_fbits(gabi::load<f32>(gabi::ea(src) + i * 4)); /* lfs (quiets a signalling NaN) */
    }
    for (u32 i = 0; i < 12; i++) {
        gabi::store<u32>(gabi::ea(dst) + i * 4, t[i]); /* word copies from the stack temporary */
    }
}
VERIFY(0x02414250, lk_mtxCopy);

/* 02415A80: HD, new (called by other actors through dComIfGp_getLinkPlayer()): while the mirror
 * reflection list is enabled (+0x4BE4), store the object's value (+0x9C) and append the object to the
 * list (count +0x4B58, capacity +0x4B5C, array +0x4B60) */
void daPy_lk_c::entryMirrorObj9CHD(void* obj, u32 v) {
    WWHD_FUNC(0x02415A80, void, this, obj, v);
    if (LK_FIELD(u8, 0x4BE4) == 0) {
        return;
    }
    gabi::store<u32>(gabi::ea(obj) + 0x9C, v);
    s32 n = LK_FIELD(s32, 0x4B58);
    if (n < LK_FIELD(s32, 0x4B5C)) {
        gabi::store<u32>(LK_FIELD(u32, 0x4B60) + n * 4, gabi::ea(obj));
        LK_FIELD(s32, 0x4B58) = LK_FIELD(s32, 0x4B58) + 1;
    }
}
VERIFY(0x02415A80, &daPy_lk_c::entryMirrorObj9CHD);

/* 02415ABC: HD, new: the same with the value at +0xA0 */
void daPy_lk_c::entryMirrorObjA0HD(void* obj, u32 v) {
    WWHD_FUNC(0x02415ABC, void, this, obj, v);
    if (LK_FIELD(u8, 0x4BE4) == 0) {
        return;
    }
    gabi::store<u32>(gabi::ea(obj) + 0xA0, v);
    s32 n = LK_FIELD(s32, 0x4B58);
    if (n < LK_FIELD(s32, 0x4B5C)) {
        gabi::store<u32>(LK_FIELD(u32, 0x4B60) + n * 4, gabi::ea(obj));
        LK_FIELD(s32, 0x4B58) = LK_FIELD(s32, 0x4B58) + 1;
    }
}
VERIFY(0x02415ABC, &daPy_lk_c::entryMirrorObjA0HD);

/* 02415AF8: HD, new: the end check of the HD selfie pose proc (02415BA4): the action status is
 * 0xB and B was pressed (or daPyFlg0 0x80000 is set) */
BOOL daPy_lk_c::checkSelfieEndHD() {
    WWHD_FUNC(0x02415AF8, BOOL, this);
    if (gabi::load<u8>(dComIfGp_ea() + 0x5BB3) != 0xB) {
        return FALSE;
    }
    if (!(mItemTrigger & 2 /* BTN_B */) && !(mNoResetFlg0 & 0x80000)) {
        return FALSE;
    }
    if (dComIfGp_checkPlayerStatus0_l(0x200000) || (mItemTrigger & 2)) {
        gabi::call(0x025E1988 /* seStartSystem */, 0x824);
        u32 a = dComIfGp_ea() + 0x5CDC;
        gabi::store<u32>(a, gabi::load<u32>(a) & ~0x100000u);
        setNoResetFlg1(noResetFlg1() & ~0x100u);
    }
    return TRUE;
}
VERIFY(0x02415AF8, &daPy_lk_c::checkSelfieEndHD);

/* 02415BA4: HD, new proc (proc table entry before procSubjectivity): probably the Picto Box selfie
 * pose. Link faces the camera; the stick/pad direction (*(0x101F5088) + 0x124) picks one of eleven
 * face expressions (table 0x10035064: face, frame, keep flag) for the face texture animations. */
BOOL daPy_lk_c::procSelfieHD() {
    WWHD_FUNC(0x02415BA4, BOOL, this);
    if (checkSelfieEndHD()) {
        u32 a = dComIfGp_ea() + 0x5CDC;
        gabi::store<u32>(a, gabi::load<u32>(a) & ~0x100000u);
        gabi::call(LK_procWait_init, this);
        mNoResetFlg0 = mNoResetFlg0 & ~0x80000u;
        return TRUE;
    }
    if (dComIfGp_getCameraAttentionStatus_l(mCameraInfoIdx) & 0x10) {
        u32 st1 = gabi::load<u32>(dComIfGp_ea() + 0x5CDC);
        s32 idx = mCameraInfoIdx;
        if (st1 & 0x100000) {
            dComIfGp_get(); /* HD: an unused accessor call (inline dComIfGp_getCamera) */
            idx = mCameraInfoIdx;
            u32 cam = gabi::load<u32>(dComIfGp_ea() + idx * 0x34 + 0x5AF8);
            gabi::Local<be<s16>> ang;
            u32 p = gabi::call<u32>(0x0200658C /* cSAngle::cSAngle */, ang.get(), (s32)(s16)(gabi::load<s16>(cam + 0x236) + 0x1500));
            s16 inv = gabi::call<s16>(0x02006804 /* cSAngle::Inv */, p);
            shape_angle.y = inv;
            current.angle.y = inv;
            u32 pad = gabi::load<u32>(gabi::load<u32>(0x101F5088) + 0x124);
            s32 pose = (pad & 4) ? 5 : 0;
            for (int k = 1; k <= 4; k++) {
                if (gabi::load<u32>(0x10035EF0 + k * 4) & pad) {
                    pose += k;
                    break;
                }
            }
            if (pad & 0x80) {
                pose = 10;
            }
            u32 entry = 0x10035064 + pose * 0xC;
            u32 face = gabi::load<u32>(entry);
            mFace = face;
            f32 frame = gabi::load<f32>(entry + 4);
            gabi::call(LK_setTextureAnime, this, face & 0xFFFF, gabi::ftoi(frame));
            gabi::store<f32>(mpAnmTexPatternData, (f32)(u32)m3530);
            gabi::store<f32>(mpTexScrollResData, (f32)(u32)m3532);
            /* HD: the three animation objects at 0x644/0x69C/0x6F4 (value = fn(ctx, frame, a, b)) */
            static const u32 objs[3] = {0x644, 0x69C, 0x6F4};
            for (int i = 0; i < 3; i++) {
                f32 f = (f32)(u32)(i < 2 ? m3530 : m3532);
                u32 o = gabi::load<u32>(gabi::ea(this) + objs[i]);
                f32 r = gabi::call_ptr<f32>(gabi::load<u32>(o + 0x10), gabi::load<u32>(o + 0x14), f, gabi::load<f32>(o + 4), gabi::load<f32>(o + 8));
                gabi::store<f32>(o, r);
            }
            u8 keep = gabi::load<u8>(entry + 8);
            u32 mf = mModeFlg;
            if (keep != 0) {
                mModeFlg = mf | 0x100 | 0x08000080;
            } else {
                m3530 = (u16)gabi::ftoi(frame);
                mModeFlg = (mf & ~0x100u) | 0x08000080;
            }
            if (face == 0x92) {
                setNoResetFlg1(noResetFlg1() | 0x100);
            } else {
                setNoResetFlg1(noResetFlg1() & ~0x100u);
            }
        } else {
            u32 cam = gabi::load<u32>(dComIfGp_ea() + idx * 0x34 + 0x5AF8);
            mModeFlg = mModeFlg & 0xF7FFFE7F;
            s16 y = gabi::load<s16>(cam + 0x236);
            shape_angle.y = y;
            current.angle.y = y;
        }
    }
    mNoResetFlg0 = mNoResetFlg0 & ~0x80000u;
    return TRUE;
}
VERIFY(0x02415BA4, &daPy_lk_c::procSelfieHD);

/* HD: the inline constructor of the render shape object shared by the sight packet and the sword
 * blur (sub-objects at +0, +0xC, +0xB4; material block at +0x128 with its identity matrices) */
static inline void lk_hdShape_ct(u32 sub) {
    gabi::call(0x027FD6F4, sub);
    gabi::call(0x027FB40C, sub + 0xC);
    gabi::store<u32>(sub + 0x18, 0x1016EF84);
    gabi::call(0x028F521C /* memset 0 */, sub + 0x80, 0x34);
    if (sub + 0xC + 0x74 == 0) {
        operator_new(0x30);
    }
    gabi::call(0x027FB40C, sub + 0xB4);
    gabi::store<u32>(sub + 0xC0, 0x1016EFB4);
    gabi::call(0x028F521C /* memset 0 */, sub + 0x128, 0x2F0);
    {
        /* the material's matrices and colours: 1.0 at these offsets, 0.0 elsewhere in 0x128..0x1D4 */
        static const u16 ones[] = {0x154, 0x144, 0x1A4, 0x164, 0x1B4, 0x184, 0x194, 0x134, 0x174, 0x1C4, 0x1D4};
        for (u32 o = 0x128; o <= 0x1D4; o += 4) {
            bool one = false;
            for (u32 k = 0; k < sizeof(ones) / sizeof(ones[0]); k++) {
                if (ones[k] == o) {
                    one = true;
                }
            }
            gabi::store<f32>(sub + o, one ? 1.0f : 0.0f);
        }
    }
    gabi::call(0x028EFFD0 /* __construct_array */, sub + 0x1D8, 2, 0x10, 0x024443B8u);
    gabi::call(0x028EFFD0, sub + 0x1F8, 2, 0x10, 0x024443B8u);
    gabi::call(0x028EFFD0, sub + 0x218, 2, 0x10, 0x024443B8u);
    {
        /* inline member constructors (allocate when their address is NULL) */
        static const u16 offs30[] = {0x110, 0x140, 0x170, 0x1A0, 0x1D0, 0x200, 0x230, 0x260};
        static const u16 offs10[] = {0x290, 0x2A0, 0x2B0, 0x2C0, 0x2D0, 0x2E0};
        for (u32 k = 0; k < 8; k++) {
            if (sub + 0x128 + offs30[k] == 0) {
                operator_new(0x30);
            }
        }
        for (u32 k = 0; k < 6; k++) {
            if (sub + 0x128 + offs10[k] == 0) {
                operator_new(0x10);
            }
        }
    }
}
/* HD: "name" material lookup in the material manager (027FFCBC); loads it on first use */
static inline u32 lk_hdFindMaterial(u32 nameStr) {
    gabi::Local<lk_SafeString_l> name;
    name->__vtbl = LK_SAFESTRING_VTBL;
    name->mStr = nameStr;
    u32 mgr = gabi::call<u32>(0x027FFCBC);
    s32 idx = gabi::call<s32>(0x027B90AC, gabi::load<u32>(mgr + 4), name.get());
    if (idx < 0) {
        return 0;
    }
    u32 n = gabi::load<u32>(mgr + 8);
    u32 arr = gabi::load<u32>(mgr + 0xC);
    u32 e = (u32)idx < n ? arr + idx * 0x24 : arr;
    if (gabi::load<u8>(e + 0x20) == 0) {
        u32 m = gabi::load<u32>(mgr + 4);
        u32 src = (u32)idx < gabi::load<u32>(m + 0x1C) ? gabi::load<u32>(m + 0x20) + idx * 0x84 : 0;
        gabi::call(0x02800B0C, e, src, 0);
        n = gabi::load<u32>(mgr + 8);
        arr = gabi::load<u32>(mgr + 0xC);
    }
    return (u32)idx < n ? arr + idx * 0x24 : arr;
}
/* HD: a vertex buffer slot (+0) allocated from the graphics heap when empty, then bound */
static inline void lk_hdVtxBuf(u32 slot, u32 fmt, u32 mat, u32 curMat) {
    u32 v = gabi::load<u32>(slot);
    if (v == 0) {
        u32 r25 = slot + 0x24C;
        u32 heap = gabi::call<u32>(0x02756140, gabi::load<u32>(0x101F8B4C));
        u32 p = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(heap + 0xC) + 0x34), heap, 0x50, 0x40);
        if (p != 0) {
            gabi::store<u32>(r25 + 4, p);
            gabi::store<u32>(r25 + 0, 4);
        }
        v = gabi::load<u32>(r25 + 4);
        gabi::store<u32>(slot, v);
    }
    gabi::call(0x027FF478, slot + 4, v, 4, fmt);
    if (mat != 0 && mat != gabi::load<u32>(curMat)) {
        gabi::call(0x027FF530, mat, slot + 0x158, slot + 4, fmt, 0);
    }
}

/* 0240EE9C: daPy_sightPacket_c::daPy_sightPacket_c (HD: a 0xC08 render object with its own shape and
 * material data: two 0x254 vertex buffers at +0x54, the "default_sight" material, the Always 0x71 and
 * Link 0x72 sight textures). GHS: allocates when called with NULL. */
static void* daPy_sightPacket_ct(void* i_self) {
    WWHD_FUNC(0x0240EE9C, void*, i_self);
    u32 self = gabi::ea(i_self);
    if (self == 0) {
        self = gabi::ea(operator_new(0xC08));
        if (self == 0) {
            return nullptr;
        }
    }
    gabi::call(0x0252CCBC /* base constructor */, self);
    u32 buf = self + 0x54;
    gabi::store<u32>(self, 0x10037C18); /* vtable */
    u32 r30 = buf;
    if (buf == 0) {
        r30 = gabi::ea(operator_new(0x4C0));
    }
    if (r30 != 0) {
        gabi::call(0x028EFFD0 /* __construct_array */, r30, 2, 0x254, 0x0244435Cu);
        gabi::store<u32>(r30 + 0x4A8, 0);
        gabi::store<u32>(r30 + 0x4AC, 0);
        gabi::store<u32>(r30 + 0x4B8, 0);
        gabi::store<u32>(r30 + 0x4B0, 0x14);
        gabi::store<u8>(r30 + 0x4BC, 0);
        gabi::store<u32>(r30 + 0, 0);
        gabi::store<u32>(r30 + 0x254, 0);
    }
    lk_hdShape_ct(self + 0x514);
    gabi::store<u8>(self + 0x92C, 0);
    gabi::call(0x027B5430, self + 0x930);
    gabi::call(0x027BDF7C, self + 0x950);
    gabi::call(0x027BE6B8, self + 0xAE8);
    gabi::call(0x027BE6B8, self + 0xB78);
    gabi::store<u32>(self + 0x50, lk_hdFindMaterial(0x10035B78 /* "default_sight" */));
    gabi::store<u32>(buf + 0x4AC, 0x11);
    gabi::store<u32>(buf + 0x4B4, 0x10036240);
    u32 r26 = gabi::load<u32>(self + 0x50);
    for (u32 i = 0; i < 2; i++) {
        lk_hdVtxBuf(buf + i * 0x254, buf + 0x4AC, r26, buf + 0x4B8);
    }
    gabi::store<u32>(buf + 0x4B8, r26);
    gabi::store<u8>(buf + 0x4BC, 1);
    gabi::call(0x027FE084, self + 0x514, 1, 0);
    gabi::store<s16>(self + 0x948, 0);
    gabi::store<s16>(self + 0x94A, 2);
    gabi::store<s16>(self + 0x94C, 1);
    gabi::store<s16>(self + 0x94E, 3);
    gabi::call(0x027B54E0, self + 0x930, self + 0x948, 4, 4);
    gabi::store<u32>(self + 0x934, 6);
    {
        u32 vb = gabi::load<u32>(buf + gabi::load<u32>(buf + 0x4A8) * 0x254); /* the current vertex buffer: four corners */
        static const s8 vals[20] = {-1, 1, 0, 0, 0, 1, 1, 0, 1, 0, -1, -1, 0, 0, 1, 1, -1, 0, 1, 1};
        for (u32 k = 0; k < 20; k++) {
            gabi::store<f32>(vb + k * 4, (f32)vals[k]);
        }
    }
    {
        u32 cur = buf + gabi::load<u32>(buf + 0x4A8) * 0x254;
        gabi::call(0x027B5E94, cur + 4, 0, gabi::load<u32>(cur + 0x150));
    }
    gabi::store<u32>(buf + 0x4A8, gabi::load<u32>(buf + 0x4A8) == 0 ? 1 : 0);
    gabi::call(0x0274FBF8, gabi::load<u32>(0x101F8B18));
    gabi::Local<lk_SafeString_l> arc1;
    arc1->__vtbl = LK_SAFESTRING_VTBL;
    arc1->mStr = 0x10035B70; /* "Always" */
    u32 img = gabi::call<u32>(0x026066C4 /* dRes_control_c::getRes */, gabi::load<u32>(0x101F4F28), arc1.get(), 0x71);
    if (img == 0) {
        JUT_ASSERT_fail(STR(0x10035B88) /* "d_a_player_eff.inc" */, 0x124, STR(0x10035B9C) /* "tmp_img != (0)" */);
    }
    gabi::store<u32>(self + 0x4C, img + gabi::load<u32>(img + 0x1C)); /* setSightTex */
    gabi::call(0x02773798, self + 0xB78, gabi::load<u32>(img + 0x20));
    gabi::Local<lk_SafeString_l> arc2;
    arc2->__vtbl = LK_SAFESTRING_VTBL;
    arc2->mStr = 0x101CEB48; /* l_arcName "Link" */
    img = gabi::call<u32>(0x026066C4, gabi::load<u32>(0x101F4F28), arc2.get(), 0x72);
    if (img == 0) {
        JUT_ASSERT_fail(STR(0x10035B88), 0x12E, STR(0x10035B9C));
    }
    gabi::store<u32>(self + 0x48, img + gabi::load<u32>(img + 0x1C)); /* setLockTex */
    gabi::call(0x02773798, self + 0xAE8, gabi::load<u32>(img + 0x20));
    gabi::store<u32>(self + 0x44, img); /* setImage */
    gabi::call(0x0274FCCC, gabi::load<u32>(0x101F8B18));
    gabi::store<u32>(self + 0xAB4, 2);
    gabi::store<u32>(self + 0xAB0, 2);
    gabi::store<u8>(self + 0xAE0, gabi::load<u8>(self + 0xAE0) | 2);
    gabi::store<u32>(self + 0xAAC, 2);
    return gabi::at<void>(self);
}
VERIFY(0x0240EE9C, daPy_sightPacket_ct);

/* 0240F564: daPy_swBlur_c::daPy_swBlur_c (HD: the sword blur became a 0x1243C render object: 120 vertex
 * buffers of 0x254 at +0x660 (60 trail segments, double buffered), its own shape, the "blur_default"
 * material and the Link 0x70 blur texture). GHS: allocates when called with NULL. */
static void* daPy_swBlur_ct(void* i_self) {
    WWHD_FUNC(0x0240F564, void*, i_self);
    u32 self = gabi::ea(i_self);
    if (self == 0) {
        self = gabi::ea(operator_new(0x1243C));
        if (self == 0) {
            return nullptr;
        }
    }
    gabi::call(0x027F1278 /* J3DPacket base constructor */, self);
    gabi::store<u32>(self + 0xA4, 0);
    gabi::store<f32>(self + 0xA8, 0.0f);
    gabi::store<u32>(self + 0x65C, 0);
    gabi::store<u32>(self + 0xC, 0x10037BE8); /* vtable */
    gabi::store<u32>(self + 0xAC, 0);
    gabi::store<u32>(self + 0xA0, 0);
    gabi::store<u32>(self + 0x98, 0);
    gabi::store<u32>(self + 0x9C, 0);
    u32 buf = self + 0x660;
    u32 r28 = buf;
    if (buf == 0) {
        r28 = gabi::ea(operator_new(0x11778));
    }
    if (r28 != 0) {
        gabi::call(0x028EFFD0 /* __construct_array */, r28, 0x78, 0x254, 0x0244463Cu);
        gabi::store<u32>(r28 + 0x11760, 0);
        gabi::store<u8>(r28 + 0x11774, 0);
        gabi::store<u32>(r28 + 0x11768, 0x14);
        gabi::store<u32>(r28 + 0x11764, 0);
        gabi::store<u32>(r28 + 0x11770, 0);
        for (u32 i = 0; i < 2; i++) {
            for (u32 j = 0; j < 0x3C; j++) {
                gabi::store<u32>(r28 + i * 0x254 + j * 0x4A8, 0);
            }
        }
    }
    u32 shape = self + 0x11DD8;
    lk_hdShape_ct(shape);
    gabi::store<u8>(shape + 0x418, 0);
    u32 disp = self + 0x121F4;
    gabi::call(0x027B5430, disp);
    u32 idxs = self + 0x1220C;
    gabi::call(0x028F521C /* memset 0 */, idxs, 8);
    gabi::call(0x027BDF7C, self + 0x12214);
    gabi::call(0x027BE6B8, self + 0x123AC);
    gabi::store<u32>(self + 0x65C, lk_hdFindMaterial(0x10035BAC /* "blur_default" */));
    u32 fmt = buf + 0x11764;
    gabi::store<u32>(fmt, 0x11);
    gabi::store<u32>(buf + 0x1176C, 0x10036238);
    u32 mat = gabi::load<u32>(self + 0x65C);
    for (u32 i = 0; i < 2; i++) {
        for (u32 j = 0; j < 0x3C; j++) {
            lk_hdVtxBuf(buf + i * 0x254 + j * 0x4A8, fmt, mat, buf + 0x11770);
        }
    }
    gabi::store<u32>(buf + 0x11770, mat);
    gabi::store<u8>(buf + 0x11774, 1);
    gabi::call(0x027FE084, shape, 1, 0);
    for (u32 k = 0; k < 4; k++) {
        gabi::store<s16>(idxs + k * 2, (s16)k);
    }
    gabi::call(0x027B54E0, disp, idxs, 4, 4);
    gabi::store<u32>(disp + 4, 6);
    /* clear the current buffer of every segment: a quad strip edge (1.0 in the colour/alpha slots) */
    for (u32 k = 0; k < 0x3C; k++) {
        u32 idx = gabi::load<u32>(buf + 0x11760);
        u32 vb = gabi::load<u32>(buf + (k * 2 + idx) * 0x254);
        u32 end = vb + 0x40;
        if (vb < end) {
            for (u32 q = vb; q < end; q += 0x20) { /* dcbz */
                for (u32 b = 0; b < 0x20; b += 4) {
                    gabi::store<u32>((q & ~31u) + b, 0);
                }
            }
            idx = gabi::load<u32>(buf + 0x11760);
        }
        vb = gabi::load<u32>(buf + (k * 2 + idx) * 0x254);
        for (u32 o = 0; o < 0x50; o += 4) {
            bool one = o == 0x38 || o == 0x48 || o == 0x4C || o == 0x20;
            gabi::store<f32>(vb + o, one ? 1.0f : 0.0f);
        }
    }
    /* copy it into the other buffer of the segment */
    {
        u32 idx = gabi::load<u32>(buf + 0x11760);
        u32 other = buf + (idx == 0 ? 1u : 0u) * 0x254;
        for (u32 k = 0; k < 0x3C; k++) {
            u32 src = buf + (k * 2 + idx) * 0x254;
            u32 dst = other + k * 0x4A8;
            for (u32 g = 0; g < 4; g++) {
                u32 s = gabi::load<u32>(src);
                u32 d = gabi::load<u32>(dst);
                for (u32 w = 0; w < 5; w++) {
                    gabi::store<f32>(d + g * 0x14 + w * 4, gabi::load<f32>(s + g * 0x14 + w * 4));
                }
            }
            idx = gabi::load<u32>(buf + 0x11760);
        }
        u32 cur = buf + idx * 0x254 + 4;
        for (u32 k = 0; k < 0x3C; k++) {
            gabi::call(0x027B5E94, cur + k * 0x4A8, 0, gabi::load<u32>(cur + k * 0x4A8 + 0x14C));
        }
    }
    gabi::store<u32>(buf + 0x11760, gabi::load<u32>(buf + 0x11760) == 0 ? 1 : 0);
    gabi::call(0x0274FBF8, gabi::load<u32>(0x101F8B18));
    gabi::Local<lk_SafeString_l> arc;
    arc->__vtbl = LK_SAFESTRING_VTBL;
    arc->mStr = 0x101CEB48; /* l_arcName "Link" */
    u32 img = gabi::call<u32>(0x026066C4 /* dRes_control_c::getRes */, gabi::load<u32>(0x101F4F28), arc.get(), 0x70);
    if (img == 0) {
        JUT_ASSERT_fail(STR(0x10035BBC), 0x312, STR(0x10035BD0));
    }
    u32 tex = self + 0x123AC;
    gabi::call(0x02773798, tex, gabi::load<u32>(img + 0x20));
    gabi::call(0x0274FCCC, gabi::load<u32>(0x101F8B18));
    /* take the texture object over into the sampler (027BDEB4 unless only the image changed) */
    u32 smp = self + 0x12214;
    static const u8 cmpOffs[] = {4, 8, 0xC, 0x10, 0x14, 0x18, 0x38, 0x34, 0x1C};
    bool same = true;
    for (u32 k = 0; k < sizeof(cmpOffs); k++) {
        if (gabi::load<u32>(smp + cmpOffs[k]) != gabi::load<u32>(tex + cmpOffs[k])) {
            same = false;
            break;
        }
    }
    if (!same) {
        gabi::call(0x027BDEB4, smp, tex);
    } else {
        u32 a = gabi::load<u32>(tex + 0x28);
        u32 b = gabi::load<u32>(tex + 0x30);
        gabi::store<u32>(smp + 0x28, a);
        gabi::store<u32>(smp + 0xDC, b);
        gabi::store<u32>(smp + 0x30, b);
        gabi::store<u32>(smp + 0xD4, a);
    }
    gabi::store<u32>(smp + 0x160, 2);
    gabi::store<u32>(smp + 0x15C, 2);
    gabi::store<u32>(smp + 0x164, 2);
    gabi::store<u8>(smp + 0x190, gabi::load<u8>(smp + 0x190) | 2);
    return gabi::at<void>(self);
}
VERIFY(0x0240F564, daPy_swBlur_ct);

/* 0240FE40: daPy_lk_c::daPy_lk_c (member constructors in layout order; HD: the sword blur is allocated
 * inside a named heap scope "(剣ブラー)daPy_swBlur_c" (025F01D8/025F0270) with 32-byte alignment).
 * GHS: allocates when called with NULL; the inline member constructors allocate when their address is NULL. */
static inline void lk_ct_listInit(u32 p, u32 n, u32 nullSize) {
    if (p == 0) {
        p = gabi::ea(operator_new(nullSize));
        if (p == 0) {
            return;
        }
    }
    gabi::store<u32>(p + 0, 0);
    gabi::store<u32>(p + 4, 0);
    gabi::store<u32>(p + 8, 0);
    gabi::call(0x0273B560, p, n, p + 0xC);
}
static void* daPy_lk_ct(void* i_self) {
    WWHD_FUNC(0x0240FE40, void*, i_self);
    u32 self = gabi::ea(i_self);
    if (self == 0) {
        self = gabi::ea(operator_new(0x8284));
        if (self == 0) {
            return nullptr;
        }
    }
    gabi::call(0x023D47E8 /* daPy_py_c base constructor */, self);
    gabi::store<u32>(self + 0xB4, 0x10037CF0); /* vtable */
    lk_ct_listInit(self + 0x458, 0xA, 0x34);
    {
        u32 p = self + 0x48C;
        if (p == 0) {
            p = gabi::ea(operator_new(0x168));
        }
        if (p != 0) {
            gabi::call(0x028EFFD0 /* __construct_array */, p, 0xA, 0x24, 0x02444280u);
        }
    }
gabi::call(0x027DA984, self + 0x644);
    gabi::store<u32>(self + 0x688, 0x0);
    gabi::store<u32>(self + 0x678, 0x1016D860);
    gabi::call(0x027DA984, self + 0x69C);
    gabi::store<u32>(self + 0x6D0, 0x1016D860);
    gabi::store<u32>(self + 0x6E0, 0x0);
    gabi::call(0x027DA984, self + 0x6F4);
    gabi::store<u32>(self + 0x738, 0x0);
    gabi::store<u32>(self + 0x728, 0x1016D7E0);
    gabi::call(0x028EFFD0 /* __construct_array */, self + 0x74C, 0x3, 0x40, 0x24EFE94u);
    gabi::call(0x024F0474 /* dBgS_Acch::dBgS_Acch */, self + 0x80C);
    gabi::store<u32>(self + 0x81C, 0x10034DA4);
    gabi::store<u32>(self + 0x82C, 0x10034DB4);
    gabi::store<u32>(self + 0x820, 0x10034DC4);
    gabi::store<u8>(self + 0x826, 0x1);
    gabi::call(0x02008FEC /* cBgS_LinChk::ct */, self + 0x9D0);
    gabi::store<u32>(self + 0xA38, 0x1);
    gabi::store<u8>(self + 0xA2E, 0x1);
    gabi::store<u8>(self + 0xA30, 0x0);
    gabi::store<u32>(self + 0x9D0, self + 0xA28);
    gabi::store<u8>(self + 0xA2D, 0x0);
    gabi::store<u8>(self + 0xA2C, 0x0);
    gabi::store<u32>(self + 0xA34, 0x10034E34);
    gabi::store<u8>(self + 0xA31, 0x0);
    gabi::store<u8>(self + 0xA32, 0x0);
    gabi::store<u32>(self + 0x9E0, 0x10034E14);
    gabi::store<u32>(self + 0xA28, 0x10034E44);
    gabi::store<u32>(self + 0x9D4, self + 0xA34);
    gabi::store<u8>(self + 0xA2F, 0x0);
    gabi::store<u32>(self + 0x9F0, 0x10034E24);
    gabi::call(0x02008FEC /* cBgS_LinChk::ct */, self + 0xA3C);
    gabi::store<u8>(self + 0xA99, 0x0);
    gabi::store<u8>(self + 0xA9A, 0x0);
    gabi::store<u8>(self + 0xA98, 0x0);
    gabi::store<u32>(self + 0xAA4, 0x1);
    gabi::store<u32>(self + 0xA3C, self + 0xA94);
    gabi::store<u8>(self + 0xA9E, 0x1);
    gabi::store<u32>(self + 0xA4C, 0x10034F14);
    gabi::store<u32>(self + 0xAA0, 0x10034F34);
    gabi::store<u8>(self + 0xA9B, 0x0);
    gabi::store<u8>(self + 0xA9C, 0x0);
    gabi::store<u32>(self + 0xA40, self + 0xAA0);
    gabi::store<u32>(self + 0xA94, 0x10034F44);
    gabi::store<u8>(self + 0xA9D, 0x0);
    gabi::store<u32>(self + 0xA5C, 0x10034F24);
    gabi::call(0x02008FEC /* cBgS_LinChk::ct */, self + 0xAA8);
    gabi::store<u8>(self + 0xB0A, 0x0);
    gabi::store<u8>(self + 0xB04, 0x0);
    gabi::store<u8>(self + 0xB09, 0x1);
    gabi::store<u8>(self + 0xB08, 0x0);
    gabi::store<u32>(self + 0xB0C, 0x10034EF4);
    gabi::store<u32>(self + 0xB00, 0x10034F04);
    gabi::store<u32>(self + 0xAB8, 0x10034ED4);
    gabi::store<u8>(self + 0xB05, 0x0);
    gabi::store<u32>(self + 0xAA8, self + 0xB00);
    gabi::store<u32>(self + 0xAAC, self + 0xB0C);
    gabi::store<u32>(self + 0xB10, 0x1);
    gabi::store<u8>(self + 0xB07, 0x0);
    gabi::store<u32>(self + 0xAC8, 0x10034EE4);
    gabi::store<u8>(self + 0xB06, 0x0);
    gabi::call(0x02008E0C /* cBgS_GndChk::cBgS_GndChk */, self + 0xB14);
    gabi::store<u8>(self + 0xB5D, 0x0);
    gabi::store<u8>(self + 0xB5E, 0x0);
    gabi::store<u32>(self + 0xB64, 0x1);
    gabi::store<u8>(self + 0xB59, 0x0);
    gabi::store<u8>(self + 0xB5C, 0x0);
    gabi::store<u32>(self + 0xB60, 0x10034C94);
    gabi::store<u32>(self + 0xB18, self + 0xB60);
    gabi::store<u8>(self + 0xB5A, 0x1);
    gabi::store<u32>(self + 0xB24, 0x10034C74);
    gabi::store<u32>(self + 0xB14, self + 0xB54);
    gabi::store<u8>(self + 0xB58, 0x0);
    gabi::store<u32>(self + 0xB34, 0x10034C84);
    gabi::store<u32>(self + 0xB54, 0x10034CA4);
    gabi::store<u8>(self + 0xB5B, 0x0);
    gabi::call(0x024EE7AC, self + 0xB68);
    gabi::store<u32>(self + 0xB74, 0x10034D64);
    gabi::store<u32>(self + 0xB88, 0x10034D74);
    gabi::store<u8>(self + 0xB92, 0x1);
    gabi::store<u32>(self + 0xB98, 0x10034D84);
    gabi::store<u32>(self + 0xB8C, 0x10034D94);
    gabi::call(0x02008FEC /* cBgS_LinChk::ct */, self + 0xBB4);
    gabi::store<u8>(self + 0xC15, 0x0);
    gabi::store<u8>(self + 0xC14, 0x0);
    gabi::store<u8>(self + 0xC11, 0x0);
    gabi::store<u32>(self + 0xC0C, 0x10034E84);
    gabi::store<u8>(self + 0xC12, 0x0);
    gabi::store<u8>(self + 0xC10, 0x0);
    gabi::store<u32>(self + 0xBC4, 0x10034E54);
    gabi::store<u32>(self + 0xBB8, self + 0xC18);
    gabi::store<u32>(self + 0xC18, 0x10034E74);
    gabi::store<u32>(self + 0xBB4, self + 0xC0C);
    gabi::store<u32>(self + 0xBD4, 0x10034E64);
    gabi::store<u8>(self + 0xC16, 0x0);
    gabi::store<u8>(self + 0xC13, 0x1);
    gabi::store<u32>(self + 0xC1C, 0x5);
    gabi::call(0x02008FEC /* cBgS_LinChk::ct */, self + 0xC20);
    gabi::store<u8>(self + 0xC7D, 0x0);
    gabi::store<u8>(self + 0xC81, 0x0);
    gabi::store<u8>(self + 0xC7C, 0x0);
    gabi::store<u32>(self + 0xC88, 0x1F);
    gabi::store<u32>(self + 0xC30, 0x10034E94);
    gabi::store<u8>(self + 0xC80, 0x0);
    gabi::store<u8>(self + 0xC7E, 0x0);
    gabi::store<u32>(self + 0xC84, 0x10034EB4);
    gabi::store<u8>(self + 0xC7F, 0x1);
    gabi::store<u32>(self + 0xC20, self + 0xC78);
    gabi::store<u32>(self + 0xC78, 0x10034EC4);
    gabi::store<u8>(self + 0xC82, 0x0);
    gabi::store<u32>(self + 0xC40, 0x10034EA4);
    gabi::store<u32>(self + 0xC24, self + 0xC84);
    gabi::call(0x02008E0C /* cBgS_GndChk::cBgS_GndChk */, self + 0xC8C);
    gabi::store<u8>(self + 0xCD0, 0x1);
    gabi::store<u8>(self + 0xCD3, 0x0);
    gabi::store<u8>(self + 0xCD4, 0x0);
    gabi::store<u8>(self + 0xCD1, 0x0);
    gabi::store<u8>(self + 0xCD2, 0x0);
    gabi::store<u32>(self + 0xCCC, 0x10034D24);
    gabi::store<u32>(self + 0xCE4, 0x0);
    gabi::store<u32>(self + 0xC8C, self + 0xCCC);
    gabi::store<u32>(self + 0xC9C, 0x10034CF4);
    gabi::store<u8>(self + 0xCD6, 0x0);
    gabi::store<u16>(self + 0xCE0, 0xFFFF);
    gabi::store<u32>(self + 0xC90, self + 0xCD8);
    gabi::store<u32>(self + 0xCDC, 0xE);
    gabi::store<u32>(self + 0xCE8, 0xFFFFFFFF);
    gabi::store<u8>(self + 0xCD5, 0x0);
    gabi::store<u32>(self + 0xCAC, 0x10034D04);
    gabi::store<u16>(self + 0xCE2, 0x100);
    gabi::store<u32>(self + 0xCD8, 0x10034D14);
    gabi::store<u32>(self + 0xCEC, 0x10034BAC);
    gabi::call(0x027F2BC0 /* J3DFrameCtrl::init */, self + 0xD04, 0x0);
    gabi::store<u32>(self + 0xD14, 0x1016E54C);
    gabi::call(0x027DA984, self + 0xD18);
    gabi::store<u32>(self + 0xD88, 0x0);
    gabi::store<u32>(self + 0xD5C, 0x0);
    gabi::store<u32>(self + 0xD4C, 0x1016D820);
    gabi::store<u32>(self + 0xD8C, 0x0);
    gabi::store<u32>(self + 0xD84, 0x0);
    gabi::store<u32>(self + 0xD80, 0x0);
    gabi::store<u32>(self + 0xD14, 0x10034BFC);
    gabi::call(0x025E80D0 /* mDoExt_brkAnm::init */, self + 0xD90);
    gabi::call(0x025E7C6C /* mDoExt_btkAnm::init */, self + 0xE08);
    gabi::call(0x027F2BC0 /* J3DFrameCtrl::init */, self + 0xE8C, 0x0);
    gabi::store<u32>(self + 0xE9C, 0x1016E54C);
    gabi::call(0x027DA984, self + 0xEA0);
    gabi::store<u32>(self + 0xF0C, 0x0);
    gabi::store<u32>(self + 0xF08, 0x0);
    gabi::store<u32>(self + 0xF10, 0x0);
    gabi::store<u32>(self + 0xED4, 0x1016D820);
    gabi::store<u32>(self + 0xEE4, 0x0);
    gabi::store<u32>(self + 0xE9C, 0x10034BFC);
    gabi::store<u32>(self + 0xF14, 0x0);
    gabi::call(0x025E7C6C /* mDoExt_btkAnm::init */, self + 0xF18);
    gabi::call(0x0252CE74, self + 0xF8C);
    gabi::call(0x025E7C6C /* mDoExt_btkAnm::init */, self + 0x43B8);
    gabi::call(0x027F2BC0 /* J3DFrameCtrl::init */, self + 0x4444, 0x0);
    gabi::store<u32>(self + 0x4454, 0x1016E54C);
    gabi::call(0x027DA984, self + 0x4458);
    gabi::store<u32>(self + 0x448C, 0x1016D820);
    gabi::store<u32>(self + 0x44C8, 0x0);
    gabi::store<u32>(self + 0x4454, 0x10034BFC);
    gabi::store<u32>(self + 0x44C0, 0x0);
    gabi::store<u32>(self + 0x44C4, 0x0);
    gabi::store<u32>(self + 0x44CC, 0x0);
    gabi::store<u32>(self + 0x449C, 0x0);
    gabi::call(0x025E80D0 /* mDoExt_brkAnm::init */, self + 0x44D4);
    gabi::call(0x025E80D0 /* mDoExt_brkAnm::init */, self + 0x454C);
    gabi::call(0x025E7C6C /* mDoExt_btkAnm::init */, self + 0x45C4);
    gabi::call(0x025E7C6C /* mDoExt_btkAnm::init */, self + 0x4638);
    gabi::call(0x025E7C6C /* mDoExt_btkAnm::init */, self + 0x46AC);
    gabi::call(0x025E7C6C /* mDoExt_btkAnm::init */, self + 0x4720);
    gabi::call(0x025E7C6C /* mDoExt_btkAnm::init */, self + 0x4794);
    gabi::call(0x025E7C6C /* mDoExt_btkAnm::init */, self + 0x4810);
    gabi::call(0x025E7C6C /* mDoExt_btkAnm::init */, self + 0x4884);
    gabi::call(0x025E80D0 /* mDoExt_brkAnm::init */, self + 0x48F8);
    gabi::call(0x025E7480, self + 0x4980);
    gabi::call(0x025E7C6C /* mDoExt_btkAnm::init */, self + 0x49F4);
    gabi::call(0x025E80D0 /* mDoExt_brkAnm::init */, self + 0x4A68);
    gabi::call(0x025E80D0 /* mDoExt_brkAnm::init */, self + 0x4AE0);
    lk_ct_listInit(self + 0x4B58, 0x20, 0x8C);
gabi::call(0x02080404, self + 0x4BE8);
    gabi::call(0x02080404, self + 0x4C98);
    gabi::call(0x02080404, self + 0x4D48);
    gabi::call(0x02080404, self + 0x4DF8);
    gabi::call(0x02080404, self + 0x4EA8);
    gabi::call(0x02080404, self + 0x4F58);
    gabi::call(0x028EFFD0 /* __construct_array */, self + 0x5008, 0x2, 0xB0, 0x2080404u);
    gabi::call(0x02080404, self + 0x5168);
    gabi::call(0x02080404, self + 0x5218);
    gabi::call(0x02080404, self + 0x52C8);
    gabi::call(0x025E7C6C /* mDoExt_btkAnm::init */, self + 0x537C);
    gabi::call(0x027F2BC0 /* J3DFrameCtrl::init */, self + 0x53F4, 0x0);
    gabi::store<u32>(self + 0x5404, 0x1016E54C);
    gabi::call(0x027DA984, self + 0x5408);
    gabi::store<u32>(self + 0x544C, 0x0);
    gabi::store<u32>(self + 0x543C, 0x1016D820);
    gabi::store<u32>(self + 0x547C, 0x0);
    gabi::store<u32>(self + 0x5478, 0x0);
    gabi::store<u32>(self + 0x5404, 0x10034BFC);
    gabi::store<u32>(self + 0x5470, 0x0);
    gabi::store<u32>(self + 0x5474, 0x0);
    gabi::call(0x025E7C6C /* mDoExt_btkAnm::init */, self + 0x5480);
    gabi::call(0x025E80D0 /* mDoExt_brkAnm::init */, self + 0x54F4);
    gabi::call(0x025E7C6C /* mDoExt_btkAnm::init */, self + 0x5570);
    gabi::call(0x025E80D0 /* mDoExt_brkAnm::init */, self + 0x5614);
    gabi::call(0x025E7C6C /* mDoExt_btkAnm::init */, self + 0x568C);
    gabi::call(0x025E80D0 /* mDoExt_brkAnm::init */, self + 0x5704);
    gabi::call(0x025E7C6C /* mDoExt_btkAnm::init */, self + 0x577C);
    gabi::call(0x028EFFD0 /* __construct_array */, self + 0x57F8, 0x2, 0x10, 0x24442C0u);
    gabi::call(0x028EFFD0 /* __construct_array */, self + 0x5818, 0x3, 0x10, 0x24442C0u);
    gabi::call(0x028EFFD0 /* __construct_array */, self + 0x5898, 0x2, 0x10, 0x2444310u);
    gabi::call(0x028EFFD0 /* __construct_array */, self + 0x58B8, 0x3, 0x10, 0x2444310u);
    gabi::call(0x0240EE9C, self + 0x58E8);
    gabi::call(0x02801DA0 /* JAIAnimeSound::JAIAnimeSound */, self + 0x64F0);
    gabi::store<u32>(self + 0x6588, 0x0);
    gabi::store<u32>(self + 0x6584, 0x100037A0);
    gabi::call(0x028EFFD0 /* __construct_array */, self + 0x65FC, 0x2, 0x4C, 0x24443E4u);
    gabi::call(0x025A9084, self + 0x6694);
    gabi::call(0x028EFFD0 /* __construct_array */, self + 0x66A8, 0x2, 0x28, 0x2444440u);
    gabi::store<u32>(self + 0x6704, 0x100347F0);
    gabi::store<u32>(self + 0x66F8, 0x100347F0);
    gabi::call(0x025A5B18 /* dPa_smokeEcallBack::dPa_smokeEcallBack */, self + 0x6710, 0x1);
    gabi::store<u32>(self + 0x6740, 0x10052228);
    gabi::store<u32>(self + 0x6730, 0x10052228);
    gabi::store<u32>(self + 0x6760, 0x10037B68);
    gabi::store<u32>(self + 0x6780, 0x10037B68);
    gabi::store<u32>(self + 0x67BC, 0x10037CB0);
    gabi::store<u32>(self + 0x6750, 0x10052228);
    gabi::store<u32>(self + 0x67A0, 0x10037B28);
    gabi::call(0x028EFFD0 /* __construct_array */, self + 0x67CC, 0x4, 0xC, 0x2444480u);
    gabi::store<u32>(self + 0x67FC, 0x100347F0);
    gabi::store<u32>(self + 0x684C, 0x10037B28);
    gabi::store<u32>(self + 0x6824, 0x10037B28);
    gabi::store<u32>(self + 0x6840, 0x100347F0);
    gabi::store<u32>(self + 0x6808, 0x10037BA8);
    gabi::store<u32>(self + 0x6814, 0x10037CB0);
    gabi::store<u32>(self + 0x6868, 0x100347F0);
    gabi::call(0x028EFFD0 /* __construct_array */, self + 0x6874, 0x2, 0x10, 0x24444C0u);
    gabi::store<f32>(self + 0x68D0, 1.0f);
    {
        u32 p = self + 0x6938; /* SafeString member */
        if (p == 0) {
            p = gabi::ea(operator_new(8));
        }
        if (p != 0) {
            gabi::store<u32>(p + 4, LK_SAFESTRING_VTBL);
            gabi::store<u32>(p + 0, 0x10000160);
        }
    }
gabi::call(0x028EFFD0 /* __construct_array */, self + 0x73F0, 0x2, 0x118, 0x2444500u);
    gabi::call(0x0200BD2C, self + 0x7620);
    gabi::call(0x02515DA0 /* dCcD_GStts::dCcD_GStts */, self + 0x763C);
    gabi::store<u32>(self + 0x7638, 0x1004AE88);
    gabi::store<u32>(self + 0x763C, 0x1004AEC0);
    gabi::call(0x02515FB8 /* dCcD_GObjInf::dCcD_GObjInf */, self + 0x765C);
    gabi::store<u32>(self + 0x7770, 0x100015A8);
    gabi::store<u32>(self + 0x776C, 0x10034B9C);
    gabi::call(0x02018590, self + 0x7774);
    gabi::store<u32>(self + 0x7698, 0x1004B108);
    gabi::store<u32>(self + 0x7770, 0x1004B160);
    gabi::store<u32>(self + 0x7788, 0x1004B150);
    gabi::call(0x02515FB8 /* dCcD_GObjInf::dCcD_GObjInf */, self + 0x778C);
    gabi::store<u32>(self + 0x789C, 0x10034B9C);
    gabi::store<u32>(self + 0x78A0, 0x100015A8);
    gabi::call(0x02018590, self + 0x78A4);
    gabi::store<u32>(self + 0x78B8, 0x1004B150);
    gabi::store<u32>(self + 0x78A0, 0x1004B160);
    gabi::store<u32>(self + 0x77C8, 0x1004B108);
    gabi::call(0x02515FB8 /* dCcD_GObjInf::dCcD_GObjInf */, self + 0x78BC);
    gabi::store<u32>(self + 0x79CC, 0x10034B9C);
    gabi::store<u32>(self + 0x79D0, 0x100015A8);
    gabi::call(0x02018590, self + 0x79D4);
    gabi::store<u32>(self + 0x78F8, 0x1004B108);
    gabi::store<u32>(self + 0x79E8, 0x1004B150);
    gabi::store<u32>(self + 0x79D0, 0x1004B160);
    gabi::call(0x02515FB8 /* dCcD_GObjInf::dCcD_GObjInf */, self + 0x79EC);
    gabi::store<u32>(self + 0x7AFC, 0x10034B9C);
    gabi::store<u32>(self + 0x7B00, 0x100015A8);
    gabi::call(0x02018590, self + 0x7B04);
    gabi::store<u32>(self + 0x7B18, 0x1004B150);
    gabi::store<u32>(self + 0x7A28, 0x1004B108);
    gabi::store<u32>(self + 0x7B00, 0x1004B160);
    gabi::call(0x028EFFD0 /* __construct_array */, self + 0x7B1C, 0x3, 0x138, 0x24445B0u);
    gabi::call(0x02515FB8 /* dCcD_GObjInf::dCcD_GObjInf */, self + 0x7EC4);
    gabi::store<u32>(self + 0x7FD8, 0x100015A8);
    gabi::store<u32>(self + 0x7FD4, 0x10034B9C);
    gabi::call(0x02018150, self + 0x7FDC);
    gabi::store<u32>(self + 0x7FF4, 0x1004AF60);
    gabi::store<u32>(self + 0x7F00, 0x1004AF18);
    gabi::store<u32>(self + 0x7FD8, 0x1004AF70);
    gabi::call(0x025166F0, self + 0x7FFC);
    u32 cyl = self + 0x8128;
    gabi::call(0x02515FB8 /* dCcD_GObjInf::dCcD_GObjInf */, cyl);
    gabi::store<u32>(cyl + 0x114, 0x100015A8);
    gabi::store<u32>(cyl + 0x110, 0x10034B9C);
    gabi::call(0x02018150, cyl + 0x118);
    gabi::store<u32>(cyl + 0x3C, 0x1004AF18);
    gabi::store<u32>(cyl + 0x130, 0x1004AF60);
    gabi::store<u32>(cyl + 0x114, 0x1004AF70);
    gabi::Local<u32> heapScope;
    gabi::call(0x025F01D8, heapScope.get(), 0x10035BE0u /* "(剣ブラー)daPy_swBlur_c" */, 0x22700, 0);
    u32 blur = gabi::call<u32>(0x0273AE48 /* operator new(size, align) */, 0x1243C, 0x20);
    if (blur != 0) {
        blur = gabi::call<u32>(0x0240F564 /* daPy_swBlur_c::daPy_swBlur_c */, blur);
    }
    gabi::store<u32>(self + 0x73EC, blur);
    gabi::call(0x025F0270, heapScope.get(), 2);
    return gabi::at<void>(self);
}
VERIFY(0x0240FE40, daPy_lk_ct);

/* dBgS_GndChk (stack object, 0x54; HD layout, this TU's vtables) */
struct lk_GndChk_l {
    /* 0x00 */ be<u32> mpPolyPassChk; /* -> +0x40 */
    /* 0x04 */ be<u32> mpGrpPassChk;  /* -> +0x4C */
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ be<f32> m_pos[3];
    /* 0x30 */ u8 _30[0x40 - 0x30];
    /* 0x40 */ be<u32> __vtbl_40;     /* dBgS_PolyPassChk */
    /* 0x44 */ be<u8> mPass[7];
    /* 0x4B */ u8 _4B;
    /* 0x4C */ be<u32> __vtbl_4C;     /* dBgS_GrpPassChk */
    /* 0x50 */ be<u32> mGrp;
};
WWHD_SIZE(lk_GndChk_l, 0x54);
/* model->setBaseTRMtx(src) (twelve FPR loads, then the stores) */
static inline void lk_setBaseTRMtx_b(u32 model, u32 src) {
    u32 t[12];
    for (u32 i = 0; i < 12; i++) {
        t[i] = lk_fbits(gabi::load<f32>(src + i * 4));
    }
    for (u32 i = 0; i < 12; i++) {
        gmem_stf32(model + 0xC8 + i * 4, t[i]);
    }
}
/* if (changeSwimProc()) { procSwimWait_init(0); m34C2 = 0; } else procWait_init(); */
static inline void lk_swimWaitOrWait(daPy_lk_c* p) {
    u32 self = gabi::ea(p);
    if (gabi::call<BOOL>(0x023F8F80 /* changeSwimProc */, self)) {
        gabi::call(0x023F8B00 /* procSwimWait_init */, self, 0);
        gabi::store<u8>(self + 0x68DE, 0);
    } else {
        gabi::call(0x023E2FF4 /* procWait_init */, self);
    }
}

/* 024131D0: daPy_lk_c::makeBgWait (HD: the treasure chest check became a ground check under Link (a
 * moving ground of actor 0x124 lowers Link onto it and waits while there is no ground below); the start
 * speeds are constants; the hat animation and 023FC06C run before the model update) */
BOOL daPy_lk_c::makeBgWait() {
    WWHD_FUNC(0x024131D0, BOOL, this);
    u32 self = gabi::ea(this);
    if (LK_FIELD(s16, 0x697E) != 0) { /* m352E */
        LK_FIELD(s16, 0x697E) = (s16)(LK_FIELD(s16, 0x697E) - 1);
    }
    gabi::call(0x024F08A8 /* dBgS_Acch::CrrPos */, self + 0x80C, dComIfG_Bgsp());
    if (LK_FIELD(f32, 0x8A0) == -1000000000.0f) { /* mAcch.GetGroundH() == -G_CM3D_F_INF */
        return 0;
    }
    if (LK_FIELD(s16, 0x697E) != 0 && gabi::call<BOOL>(0x024EEABC /* dBgS::ChkMoveBG */, dComIfG_Bgsp(), self + 0x8F4)) {
        u32 ap = gabi::call<u32>(0x02008438 /* cBgS::GetActorPointer */, dComIfG_Bgsp(), (u32)LK_FIELD(u16, 0x8F6));
        if (ap != 0 && gabi::load<s16>(ap + 8) == 0x124) {
            gabi::Local<lk_GndChk_l> chk;
            gabi::call(0x02008E0C /* cBgS_GndChk::cBgS_GndChk */, chk.get());
            chk->__vtbl_4C = 0x10034C54;
            fcpy_l(gabi::ea(chk.get()) + 0x2C, self + 0x31C);
            chk->mPass[0] = 0;
            chk->__vtbl_10 = 0x10034C34;
            chk->mpPolyPassChk = gabi::ea(chk.get()) + 0x40;
            chk->mPass[4] = 0;
            chk->mpGrpPassChk = gabi::ea(chk.get()) + 0x4C;
            fcpy_l(gabi::ea(chk.get()) + 0x24, self + 0x314);
            chk->__vtbl_20 = 0x10034C44;
            u32 gh = gmem_ld32(self + 0x8A0);
            chk->mPass[2] = 0;
            chk->mGrp = 1;
            gmem_stf32(self + 0x318, gh); /* current.pos.y = mAcch.GetGroundH() */
            chk->__vtbl_40 = 0x10034C64;
            chk->mPass[5] = 0;
            chk->mPass[3] = 0;
            gmem_stf32(gabi::ea(chk.get()) + 0x28, gh);
            chk->mPass[1] = 0;
            chk->mPass[6] = 0;
            f32 y = (f32)gabi::call<f64>(0x02008974 /* cBgS::GroundCross */, dComIfG_Bgsp(), chk.get());
            chk->__vtbl_20 = 0x10034C44;
            chk->__vtbl_40 = 0x10034C64;
            chk->__vtbl_4C = 0x10034C24;
            gabi::call(0x02008DAC /* cBgS_Chk::~cBgS_Chk */, chk.get(), 0);
            if (y == -1000000000.0f) {
                return 0;
            }
        }
    }
    if ((LK_FIELD(u32, 0xB0) & 0x80) && LK_FIELD(s16, 0x697E) != 0) {
        if (!gabi::call<BOOL>(0x024EEABC /* dBgS::ChkMoveBG */, dComIfG_Bgsp(), self + 0x8F4)) {
            return 0;
        }
        u32 ap = gabi::call<u32>(0x02008438 /* cBgS::GetActorPointer */, dComIfG_Bgsp(), (u32)LK_FIELD(u16, 0x8F6));
        if (ap == 0 || gabi::load<s16>(ap + 8) != 0x44 /* fpcNm_OBJ_IKADA_e */) {
            return 0;
        }
        u32 gh = gmem_ld32(self + 0x8A0);
        LK_FIELD(s16, 0x697E) = 0;
        gmem_stf32(self + 0x318, gh);
    }
    gabi::call(0x023FE224 /* setWaterY */, self);
    u32 tactStart = LK_FIELD(u32, 0xB0) & 0x40;
    if (tactStart == 0) {
        /* dComIfGs_setRestartRoom(current.pos, shape_angle.y, getStartRoomNo()) */
        gabi::call(0x025B9810 /* dSv_restart_c::setRoom */, gabi::load<u32>(0x101F84DC) + 0x1148, self + 0x314,
                   (s32)LK_FIELD(s16, 0x32A), LK_FIELD(u32, 0xB0) & 0x3F);
    }
    gabi::call(0x023FE960 /* setRoomInfo */, self);
    u32 startMode = (LK_FIELD(u32, 0xB0) >> 12) & 0xF;
    u32 sceneMode = gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x1177); /* dComIfGs_getLastSceneMode() */
    bool ship;
    if (startMode == 2 || startMode == 9 || sceneMode == 6 || (startMode == 5 && sceneMode == 1)) {
        ship = true;
    } else if (sceneMode != 5) {
        ship = false;
    } else if (lk_isStartStage(0x10035EBC /* "Pjavdou" */) || lk_isStartStage(0x10035EC4 /* "ShipD" */)) {
        ship = true;
    } else if (lk_isStartStage(0x10035ECC /* "Siren" */)) {
        ship = LK_FIELD(s8, 0x326) == 0 && LK_FIELD(f32, 0x31C) > 1400.0f && LK_FIELD(f32, 0x314) < 400.0f &&
               LK_FIELD(f32, 0x314) > -400.0f;
    } else {
        ship = false;
    }
    if (ship) {
        if (!gabi::call<BOOL>(0x025B8B94 /* isEventBit */, gabi::load<u32>(0x101F84DC) + 0x644, 0xF80 /* MET_KORL */)) {
            lk_swimWaitOrWait(this);
        } else {
            u32 shipAc = gabi::load<u32>(dComIfGp_ea() + 0x5B3C); /* dComIfGp_getShipActor() */
            if (shipAc == 0) {
                return 0;
            }
            if (gabi::call<BOOL>(0x025B8B94 /* isEventBit */, gabi::load<u32>(0x101F84DC) + 0x644, 0x3E10)) {
                gabi::call(0x025B8B68 /* onEventBit */, gabi::load<u32>(0x101F84DC) + 0x644, 0x3E01);
                gabi::call(0x025B8B68 /* onEventBit */, gabi::load<u32>(0x101F84DC) + 0x644, 0x3F80);
            }
            s16 ang = LK_FIELD(s16, 0x32A);
            if (tactStart != 0) {
                LK_FIELD(s16, 0x6926) = ang; /* m34DE */
                gabi::call(0x024832E0 /* daShip_c::initStartPos */, shipAc, self + 0x314, (s32)(s16)(ang + 0x4000));
            } else {
                gabi::call(0x024832E0 /* daShip_c::initStartPos */, shipAc, self + 0x314, (s32)ang);
            }
            if (gabi::load<u32>(shipAc + 0x644) & 0x200) { /* getSailOn() */
                gabi::call(0x023F1D84 /* procShipSteer_init */, self);
            } else {
                gabi::call(0x023F1E60 /* procShipPaddle_init */, self);
            }
            if (tactStart != 0) {
                gabi::call(0x02412948 /* procTactPlayEnd_init */, self, -2);
            } else if (sceneMode == 6) {
                gabi::store<u8>(shipAc + 0x636, 0x10); /* setStartModeThrow() */
            } else if (sceneMode == 4 || sceneMode == 5) {
                LK_FIELD(u16, 0x420) = 4; /* mDemo.setStartDemoType() */
                gabi::call(0x025E1928 /* mDoAud_subBgmStop */);
                gabi::call(0x02412B7C /* procShipRestart_init */, self);
            } else if (startMode == 9) {
                gabi::store<u8>(shipAc + 0x636, 0xD); /* setStartModeWarp() */
                gabi::Local<cXyz> pos;
                f32 y = LK_FIELD(f32, 0x318);
                s8 room = LK_FIELD(s8, 0x326);
                fcpy_l(gabi::ea(pos.get()) + 0, self + 0x314);
                fcpy_l(gabi::ea(pos.get()) + 8, self + 0x31C);
                pos->y = gabi::fadds_ppc(y, 1500.0f);
                u32 id = gabi::call<u32>(0x025D5834 /* fopAcM_create */, 0x1BB /* fpcNm_TORNADO_e */, 2, pos.get(), (s32)room, 0, 0, -1, 0);
                gabi::store<u32>(shipAc + 0x714, id); /* setTactWarpID */
            }
        }
    } else if (startMode == 4) {
        LK_FIELD(u16, 0x420) = 4;
        gabi::call(0x025E18EC /* mDoAud_bgmStart */, 0x8000000Fu /* JA_BGM_I_MAJU_JAIL */);
        gabi::call(0x023F54E8 /* procLargeDamage_init */, self, -7, 0, 0, 0);
    } else if (sceneMode == 4 || sceneMode == 5) {
        if (gabi::call<BOOL>(0x023F8F80 /* changeSwimProc */, self)) {
            gabi::call(0x023F8B00 /* procSwimWait_init */, self, 0);
            gabi::store<u8>(self + 0x68DE, 0);
        } else {
            gmem_stf32(self + 0x318, gmem_ld32(self + 0x8A0));
            LK_FIELD(u16, 0x420) = 4;
            gabi::call(0x025E1928 /* mDoAud_subBgmStop */);
            gabi::call(0x023F6020 /* procLargeDamageUp_init */, self, sceneMode == 4 ? -1 : -2, 1, 0, 0);
        }
    } else if (startMode == 1 || startMode == 5) {
        if (LK_FIELD(u32, 0x69E0) == 0xFF) { /* mEventIdx */
            u32 save = gabi::load<u32>(0x101F84DC);
            s16 ang = LK_FIELD(s16, 0x322);
            u32 speed = gmem_ld32(save + 0x1170); /* dComIfGs_getLastSceneSpeedF() */
            LK_FIELD(s16, 0x422) = ang;      /* mDemo.setMoveAngle */
            LK_FIELD(s16, 0x424) = 0x23;     /* mDemo.setTimer */
            LK_FIELD(u16, 0x420) = 4;        /* mDemo.setStartDemoType */
            LK_FIELD(u32, 0x430) = 0xE;      /* DEMO_KEEP_e */
            gmem_stf32(self + 0x6A14, speed); /* mNormalSpeed */
            if (gabi::call<BOOL>(0x023F8F80 /* changeSwimProc */, self)) {
                if (startMode == 1) {
                    LK_FIELD(f32, 0x6A14) = 9.0f;
                    LK_FIELD(f32, 0x370) = 9.0f;
                } else {
                    gmem_stf32(self + 0x370, gmem_ld32(self + 0x6A14));
                }
                gabi::call(0x02412E58 /* procSwimMove_init */, self, 0);
            } else if (startMode == 1) {
                LK_FIELD(f32, 0x6A14) = 8.5f;
                LK_FIELD(f32, 0x370) = 8.5f;
                gabi::call(0x023F1154 /* procMove_init */, self);
            } else if (sceneMode == 2) {
                LK_FIELD(s16, 0x6930) = LK_FIELD(s16, 0x32A); /* m34E8 */
                gabi::call(0x02412F68 /* procCrawlMove_init */, self, 0, 0);
            } else if (sceneMode == 3) {
                s16 a = (s16)(LK_FIELD(s16, 0x32A) - 0x8000);
                LK_FIELD(s16, 0x6926) = a;
                LK_FIELD(s16, 0x322) = a;
                LK_FIELD(s16, 0x32A) = a;
                LK_FIELD(s16, 0x6930) = (s16)(a + 0x8000);
                gabi::call(0x02412F68 /* procCrawlMove_init */, self, 0, 0);
            } else {
                u32 v = gmem_ld32(self + 0x6A14);
                f32 f;
                memcpy(&f, &v, 4);
                if (f > 17.0f) {
                    LK_FIELD(f32, 0x6A14) = 17.0f;
                    v = lk_fbits(17.0f);
                }
                gmem_stf32(self + 0x370, v);
                gabi::call(0x023F1154 /* procMove_init */, self);
            }
        } else {
            lk_swimWaitOrWait(this);
        }
    } else if (startMode == 7) {
        s16 ang = LK_FIELD(s16, 0x32A);
        u32 idx = (u32)(u16)ang >> 3;
        f32 x = LK_FIELD(f32, 0x314);
        f32 y = LK_FIELD(f32, 0x318);
        u32 prm = LK_FIELD(u32, 0xB0);
        f32 s = gabi::load<f32>(0x104A44F8 + idx * 8);
        f32 z = LK_FIELD(f32, 0x31C);
        f32 c = gabi::load<f32>(0x104A44F8 + idx * 8 + 4);
        gabi::Local<cXyz> pos;
        pos->x = gabi::fmadds(200.0f, s, x);
        pos->y = gabi::fadds_ppc(y, 150.0f);
        pos->z = gabi::fmadds(200.0f, c, z);
        gabi::call(0x025B9810 /* dSv_restart_c::setRoom */, gabi::load<u32>(0x101F84DC) + 0x1148, pos.get(), (s32)ang, prm & 0x3F);
        gabi::call(0x023E47D4 /* procVomitJump_init */, self, 1);
        LK_FIELD(u32, 0x834) = LK_FIELD(u32, 0x834) & ~0x20u; /* mAcch.ClrGroundHit() */
    } else if (startMode == 0xD) {
        gabi::call(0x024130B0 /* procSmallJump_init */, self, 1);
    } else if (startMode == 0xF) {
        gabi::call(0x023F6794 /* procSlowFall_init */, self);
    } else if (startMode == 0xE) {
        LK_FIELD(u16, 0x420) = 4;
        LK_FIELD(u32, 0x430) = 1; /* DEMO_N_WAIT_e */
        gabi::call(0x023E2FF4 /* procWait_init */, self);
        gabi::Local<csXyz> ang;
        gabi::call(0x0201A478 /* csXyz::csXyz */, ang.get(), 0, 0, 0);
        s32 id = gabi::call<s32>(0x025D5A20 /* fopAcM_createChild */, 0x77 /* fpcNm_FM_e */, (u32)LK_FIELD(u32, 4), 0,
                                 self + 0x314, (s32)LK_FIELD(s8, 0x326), ang.get(), 0, -1, 0);
        if (id != -1) {
            LK_FIELD(u32, 0x3B8) = LK_FIELD(u32, 0x3B8) | 0x08000000; /* onNoResetFlg0(daPyFlg0_NO_DRAW) */
        }
    } else if (gabi::call<BOOL>(0x023F8F80 /* changeSwimProc */, self)) {
        gabi::call(0x023F8B00 /* procSwimWait_init */, self, 0);
        gabi::store<u8>(self + 0x68DE, 0);
    } else if (LK_FIELD(f32, 0x318) - LK_FIELD(f32, 0x8A0) > 30.1f) {
        gabi::call(0x023F6564 /* procFall_init */, self, 1, 0.0f);
    } else {
        gabi::call(0x023E2FF4 /* procWait_init */, self);
    }
    lk_initOldFrameMorf(this, 0.0f, 0, 0x2A);
    gabi::call(0x023FF6A0 /* setWorldMatrix */, self);
    gabi::call(0x02402FFC /* checkOriginalHatAnimation */, self);
    gabi::call(0x023DD054 /* animeUpdate */, self);
    gabi::call(0x023FC06C, self);
    gabi::call(0x027F4D5C /* J3DModel::calc */, (u32)LK_FIELD(u32, 0x448));
    {
        u32 m = gabi::ea(lk_getAnmMtx(gabi::at<J3DModel>(LK_FIELD(u32, 0x448)), 0xF /* CL_JNT_HEAD_JNT_e */));
        lk_setBaseTRMtx_b(LK_FIELD(u32, 0x44C), m); /* mpKatsuraModel */
    }
    gabi::call(0x027F4D5C /* J3DModel::calc */, (u32)LK_FIELD(u32, 0x44C));
    {
        u32 m = gabi::ea(lk_getAnmMtx(gabi::at<J3DModel>(LK_FIELD(u32, 0x448)), 0xF));
        lk_setBaseTRMtx_b(LK_FIELD(u32, 0x450), m); /* mpYamuModel */
    }
    gabi::call(0x027F4D5C /* J3DModel::calc */, (u32)LK_FIELD(u32, 0x450));
    gabi::call(0x02403C24 /* setItemModel */, self);
    gabi::call(0x020182E0 /* cM3dGCyl::SetC */, self + 0x7774, self + 0x314);
    gabi::call(0x0240A0A4 /* setAttentionPos */, self);
    u32 code = gabi::call<u32>(0x024EF0BC /* dBgS::GetGroundCode */, dComIfG_Bgsp(), self + 0x8F4);
    LK_FIELD(u32, 0x69D0) = code;
    LK_FIELD(u32, 0x69CC) = code;
    if (sceneMode == 4) {
        u32 m = gabi::ea(lk_getAnmMtx(gabi::at<J3DModel>(LK_FIELD(u32, 0x448)), 0 /* CL_JNT_LINK_ROOT_e */));
        gabi::Local<cXyz> pos;
        fcpy_l(gabi::ea(pos.get()) + 0, m + 0xC);
        fcpy_l(gabi::ea(pos.get()) + 4, m + 0x1C);
        fcpy_l(gabi::ea(pos.get()) + 8, m + 0x2C);
        /* dComIfGp_particle_setP1(dPa_name::ID_AK_SN_OSHIRIKUROKOGE, &pos) */
        dPa_control_set(gabi::at<dPa_control_c>(gabi::load<u32>(dComIfGp_ea() + 0x5AB0)), 1, 0x8089, pos, nullptr, nullptr, 0xFF,
                        nullptr, -1, nullptr, nullptr, nullptr);
    }
    if (gabi::load<u32>(gabi::load<u32>(0x101F84DC) + 0x1174) & 0x8000) {
        gabi::call(0x023E9D50 /* changeDragonShield */, self, 0);
    } else {
        LK_FIELD(u16, 0x699E) = (u16)(gabi::load<u32>(gabi::load<u32>(0x101F84DC) + 0x1174) >> 16); /* mTinkleShieldTimer */
    }
    if (gabi::load<u32>(gabi::load<u32>(0x101F84DC) + 0x1174) & 0x4000) {
        LK_FIELD(u32, 0x3BC) = LK_FIELD(u32, 0x3BC) | 0x8000; /* onNoResetFlg1(daPyFlg1_SOUP_POWER_UP) */
    }
    /* l_debug_keep_pos / l_debug_shape_angle / l_debug_current_angle */
    for (u32 k = 0; k < 3; k++) {
        gabi::store<u32>(0x1046CD48 + k * 4, gabi::load<u32>(self + 0x314 + k * 4));
    }
    for (u32 k = 0; k < 3; k++) {
        gabi::store<u16>(0x1046CD10 + k * 2, gabi::load<u16>(self + 0x328 + k * 2));
    }
    for (u32 k = 0; k < 3; k++) {
        gabi::store<u16>(0x1046CD08 + k * 2, gabi::load<u16>(self + 0x320 + k * 2));
    }
    return 2; /* cPhs_NEXT_e */
}
VERIFY(0x024131D0, &daPy_lk_c::makeBgWait);

/* HD: bind a material's shader for drawing (the inline cache update at 027F29D4's state: +0 the
 * current program, +4 the current shader) */
static inline void lk_hdBindShader(u32 cache, u32 sh) {
    if (sh == gabi::load<u32>(cache + 4)) {
        return;
    }
    u8 fl = gabi::load<u8>(sh);
    u32 cur = gabi::load<u32>(cache);
    if (fl & 2) {
        gabi::store<u8>(sh, fl & ~2);
        gabi::call(0x027BB9E0, sh, 0);
    }
    u32 prog = gabi::load<u32>(gabi::load<u32>(sh + 0x7C) + 0x28);
    if (cur != prog) {
        gabi::call(0x027B9F68, prog);
    }
    u32 n = gabi::load<u32>(sh + 0xC);
    if (n != 0) {
        gabi::call(0xC00060E0 /* GX2CallDisplayList */, gabi::load<u32>(sh + 4), n);
        gabi::store<u32>(cache, prog);
        gabi::store<u32>(cache + 4, sh);
    } else {
        gabi::call(0x027BB7CC, sh);
        gabi::store<u32>(cache + 4, sh);
        gabi::store<u32>(cache, prog);
    }
}
/* HD GX2 state block on the stack (0274FDBC constructs, 0274FEB0 applies) */
struct lk_gx2State_l {
    u8 b[0x74];
};
/* 024142F0: daPy_swBlur_c::draw (HD: GX2 rendering. Binds the "blur_default" material, sets the colour
 * (by blur type at +0xA4: three colour words at 0x101CEEC0) as material constants, fills the strip of
 * quads from the two point lists at +0xBC and +0x38C with alpha fading along the blur, then draws each
 * segment) */
static void daPy_swBlur_draw(void* i_self) {
    WWHD_FUNC(0x024142F0, void, i_self);
    u32 self = gabi::ea(i_self);
    u32 cache = gabi::call<u32>(0x027F29D4, 0x104B45C0u);
    lk_hdBindShader(cache, gabi::load<u32>(gabi::load<u32>(self + 0x65C)));
    gabi::Local<Mtx34> mtx;
    gabi::call(0x02414250 /* matrix copy */, mtx.get(), 0x104B45F8u);
    u32 shape = self + 0x11DD8;
    gabi::call(0x027FDA54, shape, 0, mtx.get(), 0x104B470Cu, gabi::load<u32>(0x104B4708) + 0x240);
    gabi::call(0x027FDFF4, shape, 0);
    {
        u32 s = gabi::load<u32>(shape + 4);
        u32 mat = gabi::load<u32>(self + 0x65C);
        u32 e = s + 0x10 + gabi::load<u32>(s + 0x4C) * 0x1C;
        u32 t = gabi::load<u32>(mat + 0xC) != 0 ? gabi::load<u32>(mat + 0x10) : 0;
        s32 a = gabi::load<s16>(t + 0xC);
        u32 r24 = gabi::load<u32>(e + 0xC);
        s32 b = gabi::load<s16>(t + 0xE);
        u32 r29 = gabi::load<u32>(e + 4);
        s32 c = gabi::load<s16>(t + 0x10);
        if (b != -1) {
            gabi::call(0xC0006900 /* GX2SetPixelUniformBlock */, b, r24, r29);
        }
        if (a != -1) {
            gabi::call(0xC0006A38 /* GX2SetVertexUniformBlock */, a, r24, r29);
        }
        if (c != -1) {
            gabi::call(0xC00068A8 /* GX2SetGeometryUniformBlock */, c, r24, r29);
        }
    }
    {
        u32 type = gabi::load<u32>(self + 0xA4);
        u32 col = type == 0 ? gabi::load<u32>(0x101CEEC0) : type == 1 ? gabi::load<u32>(0x101CEEC4) : gabi::load<u32>(0x101CEEC8);
        gabi::store<f32>(shape + 0x168, (f32)(col >> 24) / 255.0f);
        gabi::store<f32>(shape + 0x16C, (f32)((col >> 16) & 0xFF) / 255.0f);
        gabi::store<f32>(shape + 0x170, (f32)((col >> 8) & 0xFF) / 255.0f);
        gabi::store<f32>(shape + 0x174, (f32)(col & 0xFF) / 255.0f);
    }
    gabi::call(0x027FB678, shape + 0xB4);
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(shape + 0xC0) + 0x2C), shape + 0xB4, gabi::load<u32>(self + 0x65C));
    gabi::call(0x027FB678, shape + 0xC);
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(shape + 0x18) + 0x2C), shape + 0xC, gabi::load<u32>(self + 0x65C));
    {
        u32 mat = gabi::load<u32>(self + 0x65C);
        u32 t = gabi::load<u32>(mat + 0x14) != 0 ? gabi::load<u32>(mat + 0x18) : 0;
        gabi::call(0x027BE53C, self + 0x12214, t + 4, -1, 0);
    }
    {
        gabi::Local<lk_gx2State_l> st;
        u32 sp = gabi::ea(st.get());
        gabi::call(0x0274FDBC, sp);
        gabi::store<u8>(sp + 0x38, 0);
        gabi::store<u8>(sp + 0x1, 0);
        gabi::store<u8>(sp + 0x44, 1);
        gabi::store<u8>(sp + 0x46, 1);
        gabi::store<u32>(sp + 0x8, 2);
        gabi::store<u8>(sp + 0x45, 1);
        gabi::store<u8>(sp + 0x0, 1);
        gabi::store<u8>(sp + 0xC, 1);
        gabi::store<u8>(sp + 0x47, 0);
        gabi::call(0x0274FEB0, sp);
    }
    u32 buf = self + 0x660;
    u32 idxp = buf + 0x11760;
    s32 n = gabi::load<s32>(self + 0x9C);
    f32 step = (255.0f / (f32)(s32)((n >> 1) + 1)) / 255.0f;
    f32 prev = 0.0f;
    f32 cur = step;
    if (n >= 0) {
        u32 cnt = (u32)n + 1;
        for (s32 i = n; ; i--) {
            u32 vb = gabi::load<u32>(buf + ((u32)i * 2 + gabi::load<u32>(idxp)) * 0x254);
            u32 end = vb + 0x40;
            if (vb < end) {
                for (u32 q = vb; q < end; q += 0x20) { /* dcbz */
                    for (u32 b = 0; b < 0x20; b += 4) {
                        gabi::store<u32>((q & ~31u) + b, 0);
                    }
                }
            }
            vb = gabi::load<u32>(buf + ((u32)i * 2 + gabi::load<u32>(idxp)) * 0x254);
            u32 pa = self + 0xBC + (u32)i * 0xC;
            u32 pb = self + 0xC8 + (u32)i * 0xC;
            u32 pc = self + 0x38C + (u32)i * 0xC;
            u32 pd = self + 0x398 + (u32)i * 0xC;
            f32 ay = gabi::load<f32>(pa + 4), ax = gabi::load<f32>(pa), az = gabi::load<f32>(pa + 8);
            gabi::store<f32>(vb + 0, ax);
            gabi::store<f32>(vb + 4, ay);
            gabi::store<f32>(vb + 0xC, cur);
            gabi::store<f32>(vb + 0x10, 0.0f);
            gabi::store<f32>(vb + 8, az);
            f32 bx = gabi::load<f32>(pb), by = gabi::load<f32>(pb + 4), bz = gabi::load<f32>(pb + 8);
            gabi::store<f32>(vb + 0x18, by);
            gabi::store<f32>(vb + 0x14, bx);
            gabi::store<f32>(vb + 0x1C, bz);
            gabi::store<f32>(vb + 0x24, 0.0f);
            gabi::store<f32>(vb + 0x20, prev);
            f32 cx = gabi::load<f32>(pc), cy = gabi::load<f32>(pc + 4), cz = gabi::load<f32>(pc + 8);
            gabi::store<f32>(vb + 0x2C, cy);
            gabi::store<f32>(vb + 0x30, cz);
            gabi::store<f32>(vb + 0x28, cx);
            gabi::store<f32>(vb + 0x34, cur);
            gabi::store<f32>(vb + 0x38, 1.0f);
            f32 dy = gabi::load<f32>(pd + 4), dx = gabi::load<f32>(pd), dz = gabi::load<f32>(pd + 8);
            gabi::store<f32>(vb + 0x3C, dx);
            gabi::store<f32>(vb + 0x48, prev);
            gabi::store<f32>(vb + 0x40, dy);
            prev = cur;
            gabi::store<f32>(vb + 0x4C, 1.0f);
            gabi::store<f32>(vb + 0x44, dz);
            cur = gabi::fadds_ppc(cur, step);
            if (--cnt == 0) {
                break;
            }
        }
    }
    {
        u32 c = buf + gabi::load<u32>(idxp) * 0x254 + 4;
        for (u32 k = 0; k < 0x3C; k++) {
            gabi::call(0x027B5E94, c + k * 0x4A8, 0, gabi::load<u32>(c + k * 0x4A8 + 0x14C));
        }
    }
    u32 idx = gabi::load<u32>(idxp) == 0 ? 1 : 0;
    gabi::store<u32>(idxp, idx);
    s32 m = gabi::load<s32>(self + 0x9C);
    if (m >= 0) {
        u32 disp = self + 0x121F4;
        u32 cnt = (u32)m + 1;
        for (s32 i = m; ; i--) {
            u32 other = idx == 0 ? 1 : 0;
            gabi::call(0x027BFE5C, buf + ((u32)i * 2 + other) * 0x254 + 0x158);
            u32 nIdx = gabi::load<u32>(disp + 0xC);
            if (nIdx != 0) {
                gabi::call(0xC0006178 /* GX2DrawIndexedEx */, gabi::load<u32>(disp + 4), nIdx, gabi::load<u32>(disp), gabi::load<u32>(disp + 8), 0, 1);
            }
            if (--cnt == 0) {
                break;
            }
            idx = gabi::load<u32>(idxp);
        }
    }
}
VERIFY(0x024142F0, daPy_swBlur_draw);

/* HD: set a shape's uniform blocks from the material's shader program (s16 slots -1 = unused) */
static inline void lk_hdUniformBlocks(u32 shapeData, u32 mat) {
    u32 e = shapeData + 0x10 + gabi::load<u32>(shapeData + 0x4C) * 0x1C;
    u32 t = gabi::load<u32>(mat + 0xC) != 0 ? gabi::load<u32>(mat + 0x10) : 0;
    s32 a = gabi::load<s16>(t + 0xC);
    u32 data = gabi::load<u32>(e + 4);
    s32 b = gabi::load<s16>(t + 0xE);
    u32 size = gabi::load<u32>(e + 0xC);
    s32 c = gabi::load<s16>(t + 0x10);
    if (b != -1) {
        gabi::call(0xC0006900 /* GX2SetPixelUniformBlock */, b, size, data);
    }
    if (a != -1) {
        gabi::call(0xC0006A38 /* GX2SetVertexUniformBlock */, a, size, data);
    }
    if (c != -1) {
        gabi::call(0xC00068A8 /* GX2SetGeometryUniformBlock */, c, size, data);
    }
}
/* the sampler at smp takes the texture object over (027BDEB4 unless only the image changed) */
static inline void lk_hdSamplerSet(u32 smp, u32 tex) {
    static const u8 cmpOffs[] = {4, 8, 0xC, 0x10, 0x14, 0x18, 0x38, 0x34, 0x1C};
    bool same = true;
    for (u32 k = 0; k < sizeof(cmpOffs); k++) {
        if (gabi::load<u32>(smp + cmpOffs[k]) != gabi::load<u32>(tex + cmpOffs[k])) {
            same = false;
            break;
        }
    }
    if (!same) {
        gabi::call(0x027BDEB4, smp, tex);
    } else {
        u32 a = gabi::load<u32>(tex + 0x28);
        u32 b = gabi::load<u32>(tex + 0x30);
        gabi::store<u32>(smp + 0xD4, a);
        gabi::store<u32>(smp + 0xDC, b);
        gabi::store<u32>(smp + 0x28, a);
        gabi::store<u32>(smp + 0x30, b);
    }
}
struct lk_Mtx44_l {
    be<f32> m[4][4];
};
/* 02414834: daPy_sightPacket_c::draw (HD: GX2 rendering of the bow/hookshot sight: the "default_sight"
 * material, the lock-on texture (+0xAE8, red-orange with the alpha at +7 while locked (+5)) or the
 * normal one (+0xB78, black), the quad from the vertex buffer at +0x54 and the matrix at +0x14) */
static void daPy_sightPacket_draw(void* i_self) {
    WWHD_FUNC(0x02414834, void, i_self);
    u32 self = gabi::ea(i_self);
    u32 cache = gabi::call<u32>(0x027F29D4, 0x104B45C0u);
    lk_hdBindShader(cache, gabi::load<u32>(gabi::load<u32>(self + 0x50)));
    {
        gabi::Local<lk_gx2State_l> st;
        u32 sp = gabi::ea(st.get());
        gabi::call(0x0274FDBC, sp);
        gabi::store<u8>(sp + 0x1, 0);
        gabi::store<u8>(sp + 0x47, 0);
        gabi::store<u8>(sp + 0x46, 1);
        gabi::store<u8>(sp + 0x38, 0);
        gabi::store<u8>(sp + 0x45, 1);
        gabi::store<u8>(sp + 0x0, 0);
        gabi::store<u8>(sp + 0xC, 1);
        gabi::store<u32>(sp + 0x8, 2);
        gabi::store<u8>(sp + 0x44, 1);
        gabi::call(0x0274FEB0, sp);
    }
    gabi::Local<Mtx34> mtx;
    gabi::Local<lk_Mtx44_l> proj;
    gabi::call(0x02414250 /* matrix copy */, mtx.get(), 0x104B45F8u);
    gabi::call(0x028E9098 /* PSMTXIdentity */, mtx.get());
    u32 p = gabi::call<u32>(0x0274D80C, 0x104B465Cu);
    gabi::call(0x028E8970, p, proj.get());
    gabi::call(0x027FDA54, self + 0x514, 0, mtx.get(), proj.get(), gabi::load<u32>(0x104B4708) + 0x240);
    gabi::call(0x027FDFF4, self + 0x514, 0);
    lk_hdUniformBlocks(gabi::load<u32>(self + 0x518), gabi::load<u32>(self + 0x50));
    u32 g, b, a;
    if (gabi::load<u8>(self + 5) != 0) { /* locked on */
        g = 0xFF;
        b = 0x32;
        a = gabi::load<u8>(self + 7);
        lk_hdSamplerSet(self + 0x950, self + 0xAE8);
    } else {
        g = 0;
        b = 0;
        a = 0xFF;
        lk_hdSamplerSet(self + 0x950, self + 0xB78);
    }
    {
        u32 mat = gabi::load<u32>(self + 0x50);
        u32 t = gabi::load<u32>(mat + 0x14) != 0 ? gabi::load<u32>(mat + 0x18) : 0;
        gabi::call(0x027BE53C, self + 0x950, t + 4, -1, 0);
    }
    u32 cb = self + 0x63C; /* material constants: the colour (1, g, b, a) twice, then a fixed colour */
    gabi::store<f32>(cb + 0x44, (f32)g / 255.0f);
    gabi::store<u32>(cb + 0x40, 0x3F800000);
    gabi::store<u32>(cb + 0x50, gabi::load<u32>(cb + 0x40));
    gabi::store<u32>(cb + 0x54, gabi::load<u32>(cb + 0x44));
    gabi::store<f32>(cb + 0x48, (f32)b / 255.0f);
    gabi::store<u32>(cb + 0x58, gabi::load<u32>(cb + 0x48));
    gabi::store<f32>(cb + 0x4C, (f32)a / 255.0f);
    gabi::store<u32>(cb + 0x5C, gabi::load<u32>(cb + 0x4C));
    for (u32 k = 0; k < 4; k++) {
        gabi::store<u32>(cb + 0x20 + k * 4, gabi::load<u32>(0x104A01EC + k * 4));
    }
    gabi::call(0x027FB678, self + 0x5C8);
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(self + 0x5D4) + 0x2C), self + 0x5C8, gabi::load<u32>(self + 0x50));
    {
        gabi::Local<Mtx34> m;
        u32 t[12];
        for (u32 k = 0; k < 12; k++) {
            t[k] = lk_fbits(gabi::load<f32>(self + 0x14 + k * 4));
        }
        for (u32 k = 0; k < 12; k++) {
            gmem_stf32(gabi::ea(m.get()) + k * 4, t[k]);
        }
        gabi::call(0x028E90D4 /* PSMTXCopy */, m.get(), self + 0x594);
    }
    gabi::call(0x027FB678, self + 0x520);
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(self + 0x52C) + 0x2C), self + 0x520, gabi::load<u32>(self + 0x50));
    gabi::call(0x027BFE5C, self + 0x54 + (gabi::load<u32>(self + 0x4FC) == 0 ? 1u : 0u) * 0x254 + 0x158);
    u32 n = gabi::load<u32>(self + 0x93C);
    if (n != 0) {
        gabi::call(0xC0006178 /* GX2DrawIndexedEx */, gabi::load<u32>(self + 0x934), n, gabi::load<u32>(self + 0x930),
                   gabi::load<u32>(self + 0x938), 0, 1);
    }
}
VERIFY(0x02414834, daPy_sightPacket_draw);

/* HD: release a vertex buffer slot (0x254): its GX2 buffer object (+0x158) and, when allocated, the
 * memory from the graphics heap (+0x24C state, +0x250 pointer) */
static inline void lk_hdVtxFree(u32 slot) {
    gabi::call(0x027BF7E8, slot + 0x158);
    u32 p = gabi::load<u32>(slot + 0x250);
    gabi::store<u32>(slot, 0);
    if (p != 0) {
        u32 heap = gabi::call<u32>(0x02755FEC, gabi::load<u32>(0x101F8B4C), p); /* (GHS: the loaded pointer stays in r4) */
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(heap + 0xC) + 0x3C), heap, gabi::load<u32>(slot + 0x250));
        gabi::store<u32>(slot + 0x24C, 0);
        gabi::store<u32>(slot + 0x250, 0);
    }
}
/* HD: the destructor of the double vertex buffer (two slots and the current index at +0x4B8) */
static inline void lk_hdVtxBufPair_dt(u32 b, u32 elemDtor) {
    lk_hdVtxFree(b);
    lk_hdVtxFree(b + 0x254);
    gabi::store<u32>(b + 0x4B8, 0);
    gabi::call(0x028F0164 /* __destroy_arr */, b, 2, 0x254, elemDtor, 0, 0);
}
/* HD: the destructor of the render shape object (see lk_hdShape_ct) */
static inline void lk_hdShape_dt(u32 sub) {
    for (s32 i = 0; i < gabi::load<s32>(sub); i++) {
        u32 arr = gabi::load<u32>(sub + 4);
        if ((u32)i < gabi::load<u32>(sub)) {
            arr += (u32)i * 0x23C;
        }
        for (u32 k = 0; k < 2; k++) {
            gabi::call(0x027BEBEC, arr + 0x10 + k * 0x1C);
        }
    }
    for (u32 k = 0; k < 2; k++) {
        gabi::call(0x027BEBEC, sub + 0x1C + k * 0x1C);
    }
    for (u32 k = 0; k < 2; k++) {
        gabi::call(0x027BEBEC, sub + 0xC4 + k * 0x1C);
    }
    gabi::call(0x027BE2B0, sub + 0x43C, 2);
    gabi::call(0x027B54A0, sub + 0x41C, 2);
    gabi::call(0x027FB528, sub + 0xB4, 0);
    gabi::call(0x027FB528, sub + 0xC, 0);
    gabi::call(0x027FD764, sub, 2);
}
/* 02415384: daPy_lk_c::~daPy_lk_c (members destroyed in reverse order; HD: the sword blur is deleted
 * through its virtual destructor and freed from its named heap (025F0148); the sight packet and the HD
 * render members have their own GX2 resources) */
static void daPy_lk_dt(void* i_self, s32 flags) {
    WWHD_FUNC(0x02415384, void, i_self, flags);
    u32 self = gabi::ea(i_self);
    if (self == 0) {
        return;
    }
    gabi::store<u32>(self + 0xB4, 0x10037CF0); /* vtable */
    u32 blur = gabi::load<u32>(self + 0x73EC);
    if (blur != 0) {
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(blur + 0xC) + 0xC), blur, 2);
        gabi::store<u32>(self + 0x73EC, 0);
        gabi::call(0x025F0148, blur);
    }
    gabi::call(0x02515980 /* dCcD_GObjInf::~dCcD_GObjInf */, self + 0x8128, 2);
    gabi::call(0x02515AE8, self + 0x7FFC, 2);
    gabi::call(0x02515980 /* dCcD_GObjInf::~dCcD_GObjInf */, self + 0x7EC4, 2);
    gabi::call(0x028F0164 /* __destroy_arr */, self + 0x7B1C, 3, 0x138, 0x02515980u, 0, 0);
    gabi::call(0x02515A70, self + 0x79EC, 2);
    gabi::call(0x02515A70, self + 0x78BC, 2);
    gabi::call(0x02515A70, self + 0x778C, 2);
    gabi::call(0x02515A70, self + 0x765C, 2);
    gabi::call(0x02515860, self + 0x7620, 2);
    gabi::call(0x028F0164 /* __destroy_arr */, self + 0x73F0, 2, 0x118, 0x02444868u, 0, 0);
    gabi::call(0x028F0164 /* __destroy_arr */, self + 0x6874, 2, 0x10, 0x02444D38u, 0, 0);
    gabi::call(0x028F0164 /* __destroy_arr */, self + 0x67CC, 4, 0xC, 0x02444D4Cu, 0, 0);
    gabi::call(0x028F0164 /* __destroy_arr */, self + 0x66A8, 2, 0x28, 0x02444D24u, 0, 0);
    gabi::call(0x028F0164 /* __destroy_arr */, self + 0x65FC, 2, 0x4C, 0x024448E0u, 0, 0);
    /* daPy_sightPacket_c::~daPy_sightPacket_c (inline) */
    {
        u32 sight = self + 0x58E8;
        u32 b = sight + 0x54;
        gabi::store<u32>(sight, 0x10037C18);
        lk_hdVtxFree(b);
        lk_hdVtxFree(b + 0x254);
        gabi::store<u32>(b + 0x4B8, 0);
        lk_hdShape_dt(sight + 0x514);
        if (b != 0) {
            lk_hdVtxBufPair_dt(b, 0x02444CC4u);
        }
        gabi::call(0x0252CCFC /* base destructor */, sight, 0);
    }
    gabi::call(0x027F3628, self + 0x5404, 0);
    gabi::call(0x02082DDC, self + 0x52C8, 2);
    gabi::call(0x02082DDC, self + 0x5218, 2);
    gabi::call(0x02082DDC, self + 0x5168, 2);
    gabi::call(0x028F0164 /* __destroy_arr */, self + 0x5008, 2, 0xB0, 0x02082DDCu, 0, 0);
    gabi::call(0x02082DDC, self + 0x4F58, 2);
    gabi::call(0x02082DDC, self + 0x4EA8, 2);
    gabi::call(0x02082DDC, self + 0x4DF8, 2);
    gabi::call(0x02082DDC, self + 0x4D48, 2);
    gabi::call(0x02082DDC, self + 0x4C98, 2);
    gabi::call(0x02082DDC, self + 0x4BE8, 2);
    gabi::call(0x027F3628, self + 0x4454, 0);
    gabi::store<u32>(self + 0x43A0, 0x10034E04);
    gabi::store<u32>(self + 0x43AC, 0x10034C24);
    gabi::store<u32>(self + 0x4368, 0x10034BBC);
    gabi::call(0x02008B4C, self + 0x4348, 0);
    /* the HD render member at +0xF8C: its shape (+0x2FA0) and vertex buffers (+0x2AE0) */
    gabi::call(0x027FB528, self + 0x3FE0, 0);
    gabi::call(0x027FB528, self + 0x3F38, 0);
    gabi::call(0x027FD764, self + 0x3F2C, 2);
    if (self + 0x3A6C != 0) {
        lk_hdVtxBufPair_dt(self + 0x3A6C, 0x024447E8u);
    }
    gabi::call(0x027BE2B0, self + 0x38D0, 2);
    gabi::call(0x027BE2B0, self + 0x3738, 2);
    gabi::call(0x027BE2B0, self + 0x35A0, 2);
    gabi::call(0x027F13DC, self + 0xF8C, 0);
    gabi::call(0x027F3628, self + 0xE9C, 0);
    gabi::call(0x027F3628, self + 0xD14, 0);
    /* the ground/line check members (dBgS_*Chk destructors: this TU's vtables, then the base) */
    gabi::store<u32>(self + 0xCD8, 0x10034C24);
    gabi::store<u32>(self + 0xCAC, 0x10034C44);
    gabi::store<u32>(self + 0xCCC, 0x10034C64);
    gabi::call(0x02008DAC /* cBgS_Chk::~cBgS_Chk */, self + 0xC8C, 0);
    gabi::store<u32>(self + 0xC78, 0x10034E04);
    gabi::store<u32>(self + 0xC40, 0x10034BBC);
    gabi::store<u32>(self + 0xC84, 0x10034C24);
    gabi::call(0x02008B4C, self + 0xC20, 0);
    gabi::store<u32>(self + 0xBD4, 0x10034BBC);
    gabi::store<u32>(self + 0xC18, 0x10034C24);
    gabi::store<u32>(self + 0xC0C, 0x10034E04);
    gabi::call(0x02008B4C, self + 0xBB4, 0);
    gabi::store<u32>(self + 0xB98, 0x10034C24);
    gabi::store<u32>(self + 0xB88, 0x10034D34);
    gabi::store<u32>(self + 0xB8C, 0x10034D54);
    gabi::call(0x02008B4C, self + 0xB78, 0);
    gabi::store<u32>(self + 0xB60, 0x10034C24);
    gabi::store<u32>(self + 0xB54, 0x10034C64);
    gabi::store<u32>(self + 0xB34, 0x10034C44);
    gabi::call(0x02008DAC /* cBgS_Chk::~cBgS_Chk */, self + 0xB14, 0);
    gabi::store<u32>(self + 0xAC8, 0x10034BBC);
    gabi::store<u32>(self + 0xB00, 0x10034E04);
    gabi::store<u32>(self + 0xB0C, 0x10034C24);
    gabi::call(0x02008B4C, self + 0xAA8, 0);
    gabi::store<u32>(self + 0xA94, 0x10034E04);
    gabi::store<u32>(self + 0xAA0, 0x10034C24);
    gabi::store<u32>(self + 0xA5C, 0x10034BBC);
    gabi::call(0x02008B4C, self + 0xA3C, 0);
    gabi::store<u32>(self + 0x9F0, 0x10034BBC);
    gabi::store<u32>(self + 0xA28, 0x10034E04);
    gabi::store<u32>(self + 0xA34, 0x10034C24);
    gabi::call(0x02008B4C, self + 0x9D0, 0);
    gabi::store<u32>(self + 0x82C, 0x10034DB4);
    gabi::store<u32>(self + 0x820, 0x10034DC4);
    gabi::call(0x024EFD9C /* dBgS_Acch::~dBgS_Acch */, self + 0x80C, 0);
    gabi::call(0x028F0164 /* __destroy_arr */, self + 0x74C, 3, 0x40, 0x02444794u, 0, 0);
    gabi::call(0x023D483C /* daPy_py_c::~daPy_py_c */, self, 0);
    if (flags & 1) {
        gabi::call(0x0273AF40 /* __dl */, self);
    }
}
VERIFY(0x02415384, daPy_lk_dt);

/* HD: a SafeString literal compared with a string (SafeString ==, the literal made terminated by
 * 02444F48) */
static inline bool lk_ssEq(u32 lit, u32 str) {
    gabi::Local<lk_SafeString_l> a;
    gabi::Local<lk_SafeString_l> b;
    a->__vtbl = LK_SAFESTRING_VTBL;
    a->mStr = lit;
    b->__vtbl = LK_SAFESTRING_VTBL;
    b->mStr = str;
    gabi::call(0x02444F48, a.get());
    lk_ss_assure(a.get());
    u32 pa = a->mStr;
    lk_ss_assure(b.get());
    u32 pb = b->mStr;
    if (pa == pb) {
        return true;
    }
    pa = a->mStr;
    pb = b->mStr;
    for (u32 n = 0; n < 0x40001; n++) {
        u8 ca = gabi::load<u8>(pa + n);
        u8 cb = gabi::load<u8>(pb + n);
        if (ca != cb) {
            return false;
        }
        if (ca == 0) {
            return true;
        }
    }
    return false;
}
/* a material's name (J3DMaterial +0: name table entry, relative offset at +4) */
static inline u32 lk_mtlName(u32 mtl) {
    u32 nm = gabi::load<u32>(mtl);
    u32 off = gabi::load<u32>(nm + 4);
    return off != 0 ? nm + 4 + off : 0;
}
static inline bool lk_mtlNameIs4(u32 mtl, u32 l0, u32 l1, u32 l2, u32 l3) {
    return lk_ssEq(l0, lk_mtlName(mtl)) || lk_ssEq(l1, lk_mtlName(mtl)) || lk_ssEq(l2, lk_mtlName(mtl)) || lk_ssEq(l3, lk_mtlName(mtl));
}
/* J3DModelData joint node j (0x1C each; the first one when out of range) */
static inline u32 lk_jointNode(u32 md, u32 j) {
    u32 n = gabi::load<u32>(md + 4);
    u32 base = gabi::load<u32>(md + 8);
    return j < n ? base + j * 0x1C : base;
}
static inline void lk_copy24(u32 dst, u32 src) {
    for (u32 k = 0; k < 0x24; k += 4) {
        gabi::store<u32>(dst + k, gabi::load<u32>(src + k));
    }
}
/* dEvent_exception_c::setStartDemo, then the rest of the init (HD: start mode 0xA also sets
 * daPyFlg0 0x08000000) */
static inline void lk_playerInitTail(u32 self, u32 demo, u32 extraFlg) {
    u32 r = gabi::call<u32>(0x02543534 /* dEvent_exception_c::setStartDemo */, dComIfGp_ea() + 0x52E8, demo);
    u32 f0 = gabi::load<u32>(self + 0x3B8);
    gabi::store<u32>(self + 0x69E0, r); /* mEventIdx */
    gabi::store<u32>(self + 0x3B8, f0 | extraFlg | 0x100); /* onNoResetFlg0(daPyFlg0_UNK100) */
    gabi::store<f32>(self + 0x434, 1.0f); /* mDemo.setStick(1.0f) */
    gabi::store<u32>(self + 0xCDC, gabi::load<u32>(self + 0xCDC) & ~2u); /* mLavaGndChk.OffWaterGrp() */
    gabi::call(0x023DCA08 /* endDamageEmitter */, self);
    gabi::call(0x0252D6E8 /* mMirrorPacket.init */, self + 0xF8C, 0);
    gabi::store<s16>(self + 0x68BC, 600); /* mLightInfluence */
    gabi::store<s16>(self + 0x68BE, 400);
    gabi::store<s16>(self + 0x68C0, 120);
    gabi::store<f32>(self + 0x68C8, 250.0f);
    for (u32 k = 0; k < 3; k++) {
        gabi::store<u32>(self + 0x68B0 + k * 4, gabi::load<u32>(self + 0x314 + k * 4));
    }
    gabi::store<f32>(self + 0x68C4, 0.0f);
    gabi::call(0x025564B4 /* dKy_plight_set */, self + 0x68B0);
    gabi::store<s16>(self + 0x697E, 900); /* m352E */
    gabi::store<s16>(self + 0x6926, gabi::load<s16>(self + 0x32A)); /* m34DE */
    gabi::store<u32>(self + 0x3B4, 0xAD); /* mFace = daPyFace_NONE */
}

/* 02410BE8: daPy_lk_c::playerInit (HD: the eye/eyebrow shapes are sorted by material name instead of
 * z-mode; the hands model's joints are hidden and their matrices zeroed; no item heap dummies or blur
 * texture; every "linktexS3TC" texture is replaced by the casual one, the originals kept in the
 * object at +0x458 (copies in +0x48C) and swapped back for the hero's clothes) */
void daPy_lk_c::playerInit() {
    WWHD_FUNC(0x02410BE8, void, this);
    u32 b = gabi::ea(this);
    if (!gabi::call<BOOL>(0x025D63E8 /* fopAcM_entrySolidHeap */, b, 0x023D6184u /* daPy_createHeap */, 0)) {
        JUT_ASSERT_fail(STR(0x10035D20), 0x5F3B, STR(0x10035CF0));
    }
    {
        u32 m = LK_FIELD(u32, 0x448);
        LK_FIELD(u32, 0x348) = m != 0 ? m + 0xC8 : 0; /* fopAcM_SetMtx */
        gabi::store<u32>(m + 0xB8, b); /* setUserArea */
        gabi::store<u32>(LK_FIELD(u32, 0x5700) + 0xB8, b);
    }
    for (u16 j = 0; j < gabi::load<u16>(gabi::call<u32>(0x027F3F94, gabi::load<u32>(LK_FIELD(u32, 0x5700) + 0xAC)) + 8); j = (u16)(j + 1)) {
        u32 node = lk_jointNode(gabi::load<u32>(LK_FIELD(u32, 0x5700) + 0xAC), j);
        gabi::store<u32>(node + 8, 0x023D6980u); /* daPy_auraCallback */
    }
    {
        static const u8 jnts[] = {2, 14, 5, 9, 13, 29, 15, 1};
        for (u32 k = 0; k < sizeof(jnts); k++) {
            gabi::store<u32>(lk_jointNode(LK_FIELD(u32, 0x444), jnts[k]) + 8, 0x023D80F0u); /* daPy_jointCallback0 */
        }
    }
    for (u32 k = 0; k < 2; k++) { /* m_pbCalc */
        u32 c = gabi::load<u32>(b + 0x57F0 + k * 4);
        gabi::store<u32>(c + 0xA8, b);
        gabi::store<u32>(gabi::load<u32>(b + 0x57F0 + k * 4) + 0xB0, 0x023D7194u); /* daPy_jointBeforeCallback */
        gabi::store<u32>(gabi::load<u32>(b + 0x57F0 + k * 4) + 0xB4, 0x023D78E0u); /* daPy_jointAfterCallback */
    }
    s32 zon = 0, zoffNone = 0, zoffBlend = 0;
    {
        u32 mtl = gabi::load<u32>(lk_jointNode(LK_FIELD(u32, 0x444), 0x13 /* CL_JNT_CL_EYE_e */) + 0x10);
        for (u32 pass = 0; pass < 2; pass++) {
            while (mtl != 0) {
                if (lk_mtlNameIs4(mtl, 0x10035DF0 /* "eyeLdamA" */, 0x10035DFC /* "eyeRdamA" */, 0x10035D68 /* "mayuLdamA" */, 0x10035D74 /* "mayuRdamA" */)) {
                    gabi::store<u32>(b + 0x62C + zon * 4, gabi::load<u32>(mtl + 8)); /* mpZOnShape */
                    zon++;
                    if (zon > 4) {
                        JUT_ASSERT_fail(STR(0x10035D20), 0x5F96, STR(0x10035E08));
                    }
                } else if (lk_mtlNameIs4(mtl, 0x10035E18 /* "eyeLdamB" */, 0x10035E24 /* "eyeRdamB" */, 0x10035D80 /* "mayuLdamB" */, 0x10035D8C /* "mayuRdamB" */)) {
                    gabi::store<u32>(b + 0x61C + zoffNone * 4, gabi::load<u32>(mtl + 8)); /* mpZOffNoneShape */
                    zoffNone++;
                    if (zoffNone > 4) {
                        JUT_ASSERT_fail(STR(0x10035D20), 0x5F9E, STR(0x10035E30));
                    }
                } else if (lk_mtlNameIs4(mtl, 0x10035D08 /* "eyeL" */, 0x10035D10 /* "eyeR" */, 0x10035CF4 /* "mayuL" */, 0x10035CFC /* "mayuR" */)) {
                    gabi::store<u32>(b + 0x60C + zoffBlend * 4, gabi::load<u32>(mtl + 8)); /* mpZOffBlendShape */
                    zoffBlend++;
                    if (zoffBlend > 4) {
                        JUT_ASSERT_fail(STR(0x10035D20), 0x5FA6, STR(0x10035D34));
                    }
                }
                mtl = gabi::load<u32>(mtl + 4); /* getNext() */
            }
            mtl = gabi::load<u32>(lk_jointNode(LK_FIELD(u32, 0x444), 0x15 /* CL_JNT_CL_MAYU_e */) + 0x10);
        }
    }
    if (zon != 4) {
        JUT_ASSERT_fail(STR(0x10035D20), 0x5FAF, STR(0x10035E44));
    }
    if (zoffNone != 4) {
        JUT_ASSERT_fail(STR(0x10035D20), 0x5FB0, STR(0x10035E54));
    }
    if (zoffBlend != 4) {
        JUT_ASSERT_fail(STR(0x10035D20), 0x5FB1, STR(0x10035D48));
    }
    LK_FIELD(u32, 0x63C) = gabi::load<u32>(gabi::load<u32>(lk_jointNode(LK_FIELD(u32, 0x444), 8) + 0x10) + 8); /* mpLhandShape */
    LK_FIELD(u32, 0x640) = gabi::load<u32>(gabi::load<u32>(lk_jointNode(LK_FIELD(u32, 0x444), 12) + 0x10) + 8); /* mpRhandShape */
    {
        u32 hmd = gabi::load<u32>(LK_FIELD(u32, 0xCF4) + 0xAC); /* mpHandsModel->getModelData() */
        gabi::Local<Mtx34> zero;
        u32 z = gabi::ea(zero.get());
        for (u32 j = 1; j <= 10; j++) { /* HANDS_JNT_CL_LHANDB_e .. CL_RHANDE: hidden, matrices zeroed */
            u32 node = lk_jointNode(hmd, j);
            gabi::store<u8>(gabi::load<u32>(gabi::load<u32>(node + 0x10) + 8) + 4, 0);
            for (u32 k = 0; k < 12; k++) {
                gabi::store<u32>(z + k * 4, 0);
            }
            u32 m = gabi::ea(lk_getAnmMtx(gabi::at<J3DModel>(LK_FIELD(u32, 0xCF4)), (s32)j));
            for (u32 k = 0; k < 12; k++) {
                gabi::store<f32>(m + k * 4, gabi::load<f32>(z + k * 4));
            }
        }
    }
    {
        u32 buf = LK_FIELD(u32, 0x5850); /* m_anm_heap_under[UNDER_MOVE0_e].m_buffer */
        LK_FIELD(u32, 0x5860) = buf + 0x2400;
        LK_FIELD(u32, 0x5870) = buf + 0x4800;
        for (u32 i = 1; i <= 2; i++) {
            LK_FIELD(u32, 0x5870 + i * 0x10) = LK_FIELD(u32, 0x5870) + i * 0x2400;
        }
    }
    gabi::call(0x024F06B4 /* dBgS_Acch::Set */, b + 0x80C, b + 0x314, b + 0x300, b, 3, b + 0x74C, b + 0x33C, b + 0x320, b + 0x328);
    LK_FIELD(u32, 0x834) = ((LK_FIELD(u32, 0x834) & ~0x400u) | 0x2000) & ~8u; /* ClrWaterNone, OnLineCheck, ClrRoofNone */
    LK_FIELD(f32, 0x3C4) = 17.0f;   /* mMaxNormalSpeed */
    LK_FIELD(f32, 0x378) = -175.0f; /* maxFallSpeed */
    LK_FIELD(f32, 0x374) = -2.5f;   /* gravity */
    LK_FIELD(f32, 0x8D4) = 500.0f;  /* SetWaterCheckOffset */
    LK_FIELD(f32, 0x8CC) = 125.0f;  /* SetRoofCrrHeight */
    gabi::call(0x024EFF44 /* dBgS_AcchCir::SetWall */, b + 0x74C, 30.1f, 35.0f);
    gabi::call(0x024EFF44 /* dBgS_AcchCir::SetWall */, b + 0x78C, 89.9f, 35.0f);
    gabi::call(0x024EFF44 /* dBgS_AcchCir::SetWall */, b + 0x7CC, 125.0f, 35.0f);
    LK_FIELD(f32, 0x3C8) = 125.0f; /* mHeight */
    LK_FIELD(u16, 0x69A2) = 0x100; /* mKeepItem = daPyItem_NONE_e */
    LK_FIELD(f32, 0x3E0) = 0.0f;   /* mHeadTopPos */
    LK_FIELD(u16, 0x69B0) = 0x100; /* mEquipItem */
    LK_FIELD(f32, 0x3DC) = 0.0f;
    LK_FIELD(f32, 0x3D8) = 0.0f;
    LK_FIELD(s32, 0x69BC) = gabi::load<s8>(dComIfGp_ea() + 0x5B30); /* mCameraInfoIdx */
    {
        u32 att = dComIfGp_ea() + 0x5804;
        u32 stts = b + 0x7620;
        LK_FIELD(u32, 0x6894) = att; /* mpAttention */
        gabi::call(0x02515F14 /* dCcD_Stts::Init */, stts, 0x78, 0xFF, b);
        gabi::call(0x02516518 /* dCcD_Cyl::Set */, b + 0x765C, 0x101CECDCu /* l_cyl_src */);
        s16 by = LK_FIELD(s16, 0x3D2);
        LK_FIELD(u32, 0x7734) = b + 0x6996; /* SetTgShieldFrontRangeYAngle */
        u32 t = LK_FIELD(u32, 0x76F0);
        s16 sy = LK_FIELD(s16, 0x32A);
        LK_FIELD(u32, 0x76F0) = t | 8; /* OnTgShieldFrontRange */
        LK_FIELD(u32, 0x76A0) = stts;
        LK_FIELD(s16, 0x6996) = (s16)(sy + by); /* mShieldFrontRangeYAngle */
        gabi::call(0x02516518 /* dCcD_Cyl::Set */, b + 0x778C, 0x101CED20u /* l_wind_cyl_src */);
        LK_FIELD(u32, 0x77D0) = stts;
        gabi::call(0x02516518 /* dCcD_Cyl::Set */, b + 0x79EC, 0x101CED20u);
        LK_FIELD(u32, 0x7A30) = stts;
        LK_FIELD(u32, 0x7A14) = 0x00800000; /* SetTgType(AT_TYPE_LIGHT) */
        for (u32 i = 0; i < 3; i++) {
            gabi::call(0x025164C0 /* dCcD_Cps::Set */, b + 0x7B1C + i * 0x138, 0x101CEDA8u /* l_at_cps_src */);
            LK_FIELD(u32, 0x7B60 + i * 0x138) = stts;
        }
        gabi::call(0x02516518 /* dCcD_Cyl::Set */, b + 0x78BC, 0x101CED64u /* l_at_cyl_src */);
        LK_FIELD(u32, 0x7900) = stts;
        gabi::call(0x025164C0 /* dCcD_Cps::Set */, b + 0x7EC4, 0x101CEDF4u /* l_fan_wind_cps_src */);
        LK_FIELD(u32, 0x7F08) = stts;
        LK_FIELD(f32, 0x7FF8) = 70.0f;
        gabi::call(0x0251677C /* dCcD_Sph::Set */, b + 0x7FFC, 0x101CEB50u /* l_fan_wind_sph_src */);
        LK_FIELD(u32, 0x8040) = stts;
        gabi::call(0x025164C0 /* dCcD_Cps::Set */, b + 0x8128, 0x101CEDF4u);
        LK_FIELD(u32, 0x8138) = 0x00800000; /* SetAtType(AT_TYPE_LIGHT) */
        LK_FIELD(u32, 0x816C) = stts;
        LK_FIELD(f32, 0x825C) = 20.0f;
    }
    for (u32 i = 0; i < 2; i++) { /* m_anm_heap_under */
        u32 h = b + 0x5848 + i * 0x10;
        gabi::call(0x02410888 /* createAnimeHeap */, b, h + 0xC, 0);
        gabi::store<s16>(h + 0, -1);
        gabi::store<s16>(h + 2, -1);
        gabi::store<s16>(h + 4, -1);
    }
    for (u32 i = 0; i < 3; i++) { /* m_anm_heap_upper */
        u32 h = b + 0x5868 + i * 0x10;
        gabi::call(0x02410888 /* createAnimeHeap */, b, h + 0xC, 0);
        gabi::store<s16>(h + 0, -1);
        gabi::store<s16>(h + 2, -1);
        gabi::store<s16>(h + 4, -1);
    }
    gabi::call(0x02410888 /* createAnimeHeap */, b, b + 0x65DC, 1); /* m_tex_anm_heap */
    LK_FIELD(s16, 0x65D6) = -1;
    LK_FIELD(s16, 0x65D4) = -1;
    LK_FIELD(s16, 0x65D0) = -1;
    LK_FIELD(s16, 0x65D2) = -1;
    gabi::call(0x02410888 /* createAnimeHeap */, b, b + 0x65EC, 2); /* m_tex_scroll_heap */
    LK_FIELD(s16, 0x65E2) = -1;
    LK_FIELD(s16, 0x65E0) = -1;
    LK_FIELD(s16, 0x6940) = -1; /* mSeAnmIdx */
    LK_FIELD(u32, 0x6938) = 0x10035D04; /* (HD SafeString member: "") */
    LK_FIELD(s16, 0x65E4) = -1;
    LK_FIELD(s16, 0x65E6) = -1;
    for (u32 i = 0; i < 2; i++) { /* mpItemHeaps (HD: named, no dummy allocation) */
        u32 h = gabi::call<u32>(0x025E3630 /* mDoExt_createSolidHeapFromGameToCurrent */, 0x1E600, 0x20);
        LK_FIELD(u32, 0x4438 + i * 4) = h;
        if (h == 0) {
            JUT_ASSERT_fail(STR(0x10035D20), i == 0 ? 0x605A : 0x6064, STR(i == 0 ? 0x10035D98 : 0x10035DC4));
            h = LK_FIELD(u32, 0x4438 + i * 4);
        }
        gabi::store<u32>(h + 0x10, i == 0 ? 0x10035DB0 : 0x10035DDC); /* heap name */
        gabi::call(0x025E37D8 /* mDoExt_restoreCurrentHeap */);
    }
    gabi::call(0x02410888 /* createAnimeHeap */, b, b + 0x4808, 3); /* mpItemAnimeHeap */
    gabi::call(0x023DC63C /* daPy_actorKeep_c::clearData */, b + 0x6590);
    gabi::call(0x023DC63C, b + 0x6598);
    gabi::call(0x023DC63C, b + 0x65A0);
    gabi::call(0x023DC63C, b + 0x65A8);
    LK_FIELD(s32, 0x6A8C) = -1; /* mWhirlId */
    LK_FIELD(s16, 0x324) = 0;   /* current.angle.z */
    LK_FIELD(s16, 0x32C) = 0;   /* shape_angle.z */
    LK_FIELD(s32, 0x6A88) = -1;
    LK_FIELD(s16, 0x6976) = 8;  /* m3526 */
    LK_FIELD(s32, 0x6A80) = -1;
    LK_FIELD(s32, 0x6A84) = -1;
    LK_FIELD(f32, 0x6AAC) = 1.0f; /* m3648.w */
    gabi::call(0x023E07C0 /* resetSeAnime */, b);
    {
        gabi::Local<lk_SafeString_l> arc;
        arc->__vtbl = LK_SAFESTRING_VTBL;
        arc->mStr = 0x10035D18; /* "Always" */
        u32 tex = gabi::call<u32>(0x026066C4 /* dRes_control_c::getRes */, gabi::load<u32>(0x101F4F28), arc.get(), 0x71);
        if (tex == 0) {
            JUT_ASSERT_fail(STR(0x10035D20), 0x6087, STR(0x10035E68));
        }
        LK_FIELD(u32, 0x5934) = tex; /* mSightPacket.setSightTex */
    }
    {
        gabi::Local<lk_SafeString_l> arc;
        arc->__vtbl = LK_SAFESTRING_VTBL;
        arc->mStr = 0x101CEB48; /* l_arcName "Link" */
        u32 img = gabi::call<u32>(0x026066C4 /* dRes_control_c::getRes */, gabi::load<u32>(0x101F4F28), arc.get(), 0x72);
        if (img == 0) {
            JUT_ASSERT_fail(STR(0x10035D20), 0x608D, STR(0x10035E78));
        }
        u32 off = gabi::load<u32>(img + 0x1C);
        LK_FIELD(u32, 0x592C) = img;       /* mSightPacket.setImage */
        LK_FIELD(u32, 0x5930) = img + off; /* setLockTex */
    }
    {
        u32 startMode = (LK_FIELD(u32, 0xB0) >> 12) & 0xF;
        u32 prm = LK_FIELD(u32, 0xB0);
        u8 lastMode = gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x1177);
        if (startMode == 0xE) {
            lk_playerInitTail(b, 0xD4, 0);
        } else if (startMode == 0xC) {
            lk_playerInitTail(b, 0xD0, 0);
        } else if (startMode == 6) {
            lk_playerInitTail(b, 0xCF, 0);
        } else if (startMode == 0xA) {
            lk_playerInitTail(b, 0xD2, 0x08000000);
        } else if (startMode == 0xB) {
            lk_playerInitTail(b, 0xD3, 0);
        } else if (startMode == 9 || lastMode == 6) {
            lk_playerInitTail(b, 0xCB, 0);
        } else if (startMode == 2 && (lastMode == 4 || lastMode == 5)) {
            lk_playerInitTail(b, 0xC9, 0);
        } else if (startMode == 0xF) {
            lk_playerInitTail(b, 0xD5, 0);
        } else {
            lk_playerInitTail(b, prm >> 24 /* getStartEvent() */, 0);
        }
    }
    /* the Link texture: casual clothes */
    u32 casual;
    {
        gabi::Local<lk_SafeString_l> arc;
        arc->__vtbl = LK_SAFESTRING_VTBL;
        arc->mStr = 0x101CEB48;
        casual = gabi::call<u32>(0x026066C4 /* dRes_control_c::getRes */, gabi::load<u32>(0x101F4F28), arc.get(), 0x71);
    }
    LK_FIELD(u32, 0x454) = b + 0x458;
    u32 texture = gabi::load<u32>(LK_FIELD(u32, 0x444) + 0x30); /* getTexture() */
    if (texture == 0) {
        JUT_ASSERT_fail(STR(0x10035D20), 0x60DA, STR(0x10035E88));
    }
    u32 names = gabi::load<u32>(LK_FIELD(u32, 0x444) + 0x34); /* getTextureName() */
    if (names == 0) {
        JUT_ASSERT_fail(STR(0x10035D20), 0x60DC, STR(0x10035E98));
    }
    for (u16 i = 0; i < gabi::load<u16>(texture); i = (u16)(i + 1)) {
        u32 nm = gabi::call<u32>(0x027ED1F0 /* JUTNameTab::getName */, names, (u32)i);
        if (nm == 0) {
            continue;
        }
        bool same;
        {
            u32 k = 0;
            u8 c1, c2;
            do {
                c1 = gabi::load<u8>(nm + k);
                c2 = gabi::load<u8>(0x10035D5C + k);
                k++;
            } while (c1 == c2 && c1 != 0);
            same = c1 == c2;
        }
        if (!same) {
            continue;
        }
        u32 res = gabi::load<u32>(texture + 4) + (u32)i * 0x24;
        {
            s32 cnt = gabi::load<s32>(b + 0x458);
            if (cnt < gabi::load<s32>(b + 0x45C)) { /* the originals list: push_back */
                gabi::store<u32>(gabi::load<u32>(b + 0x460) + (u32)cnt * 4, res);
                cnt = gabi::load<s32>(b + 0x458) + 1;
                gabi::store<s32>(b + 0x458, cnt);
            }
            u32 last = (u32)cnt - 1;
            u32 v = LK_FIELD(u32, 0x454);
            u32 n = gabi::load<u32>(v);
            u32 dst = b + 0x48C + (last < 10 ? last * 0x24 : 0);
            u32 src = last < n ? gabi::load<u32>(gabi::load<u32>(v + 8) + last * 4) : 0;
            lk_copy24(dst, src); /* keep a copy of the original header */
        }
        lk_copy24(gabi::load<u32>(texture + 4) + (u32)i * 0x24, casual); /* texture->setResTIMG(i, *casual) */
        {
            u32 r = gabi::load<u32>(texture + 4) + (u32)i * 0x24;
            gabi::store<u32>(r + 0x1C, gabi::load<u32>(r + 0x1C) + casual - r);
            r = gabi::load<u32>(texture + 4) + (u32)i * 0x24;
            gabi::store<u32>(r + 0xC, gabi::load<u32>(r + 0xC) + casual - r);
            r = gabi::load<u32>(texture + 4) + (u32)i * 0x24;
            gabi::store<u32>(r + 0x20, gabi::load<u32>(casual + 0x20));
        }
        {
            u32 hi = gabi::load<u32>(texture + 0x18), lo = gabi::load<u32>(texture + 0x1C);
            auto slw = [](u32 v, u32 s) -> u32 { s &= 0x3F; return s >= 32 ? 0 : v << s; };
            auto srw = [](u32 v, u32 s) -> u32 { s &= 0x3F; return s >= 32 ? 0 : v >> s; };
            if (i < 0x40) {
                u32 sh = i;
                u32 h = slw(hi, sh) | srw(lo, 32 - sh) | slw(lo, sh + 32);
                gabi::store<u32>(texture + 8, gabi::load<u32>(texture + 8) | h);
                gabi::store<u32>(texture + 0xC, gabi::load<u32>(texture + 0xC) | slw(lo, sh));
            } else {
                u32 sh = i - 0x40;
                u32 w10 = gabi::load<u32>(texture + 0x10);
                u32 h = slw(hi, sh) | srw(lo, 32 - sh) | slw(lo, sh + 32);
                u32 w14 = gabi::load<u32>(texture + 0x14) | slw(lo, sh);
                gabi::store<u32>(texture + 0x14, w14);
                gabi::store<u32>(texture + 0x10, w10 | h);
            }
        }
    }
    {
        u32 save = gabi::load<u32>(0x101F84DC);
        if (!gabi::call<BOOL>(0x025B8B94 /* isEventBit */, save + 0x644, 0x2A80) || gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x1C0) != 0) {
            LK_FIELD(u32, 0x3BC) = LK_FIELD(u32, 0x3BC) | 8; /* onNoResetFlg1(daPyFlg1_CASUAL_CLOTHES) */
        }
    }
    if (!(LK_FIELD(u32, 0x3BC) & 8)) {
        /* the hero's clothes: swap the original headers back */
        struct Tmp {
            u8 b[0x168];
        };
        gabi::Local<Tmp> tmp;
        u32 t = gabi::ea(tmp.get());
        u32 v = LK_FIELD(u32, 0x454);
        gabi::call(0x028EFFD0 /* __construct_array */, t, 0xA, 0x24, 0x02444280u);
        for (u32 k = 0; k < 10; k++) {
            u32 src = k < gabi::load<u32>(v) ? gabi::load<u32>(gabi::load<u32>(v + 8) + k * 4) : 0;
            lk_copy24(t + k * 0x24, src);
        }
        gabi::call(0x0272B8B8, (u32)LK_FIELD(u32, 0x454), b + 0x48C);
        for (u32 k = 0; k < 10; k++) {
            lk_copy24(b + 0x48C + k * 0x24, t + k * 0x24);
        }
    }
    LK_FIELD(u8, 0x8282) = 0;
    LK_FIELD(u8, 0x8265) = 0;
    LK_FIELD(u16, 0x8280) = 0;
    LK_FIELD(u8, 0x827C) = 0;
    LK_FIELD(u32, 0x8268) = 0;
    LK_FIELD(u8, 0x8264) = 0;
    {
        u32 play = dComIfGp_ea();
        gabi::store<u32>(play + 0x5CDC, gabi::load<u32>(play + 0x5CDC) & ~0x100000u);
    }
}
VERIFY(0x02410BE8, &daPy_lk_c::playerInit);
