/**
 * d_a_player_main_04.cpp (WWHD)
 * Player - Link (daPy_lk_c), address range #04, first half (02400B14..0240EE2B): neck/hat angles,
 * joint callback, item models, foot effects, collision, attention, water/aura effects, execute.
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
/* HD: the material parameter block passed to an emitter (0281E5A8) */
struct lk_emitterParam_l {
    /* 0x00 */ be<u32> mName; /* sead::SafeString */
    /* 0x04 */ be<u32> __vtbl;
    /* 0x08 */ be<f32> m08[4];
    /* 0x18 */ be<u8> m18[4];
};
/* play + 0x513F: dComIfGp_getStartStageLayer() */
static inline s8 dComIfGp_getStartStageLayer_l() { return gabi::load<s8>(dComIfGp_ea() + 0x513F); }

/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty; block at +0x2C, flags +4, matrices +0x10) */
static inline Mtx34* lk_getAnmMtx(J3DModel* m, s32 jnt) {
    u32 blk = gabi::load<u32>(gabi::ea(m) + 0x2C);
    gabi::store<u16>(blk + 4, gabi::load<u16>(blk + 4) | 0x10);
    return gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jnt * 0x30);
}
/* daPy_lk_c::checkNormalSwordEquip(): the selected sword (save + 0x2E) is the Master Sword (0x38) or
 * the mini game type (play + 0x5CEA) is 2 */
static inline bool lk_checkNormalSwordEquip() {
    return gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x2E) == 0x38 || gabi::load<u8>(dComIfGp_ea() + 0x5CEA) == 2;
}

/* bits of an f32 value */
static inline u32 lk_fbits(f32 f) {
    u32 v;
    memcpy(&v, &f, 4);
    return v;
}
/* a float copy through an FPR that the recompiled original keeps bit-exact (no SNaN quieting) */
static inline void fcpy_l(u32 dst, u32 src) { gmem_stf32(dst, gmem_ld32(src)); }
#define mGndChkPos (*gabi::at<cXyz>(gabi::ea(this) + 0xB38)) /* mGndChk's position (dBgS_GndChk + 0x24) */
#define mGndChkPoly (gabi::ea(this) + 0xB28)                 /* mGndChk's cBgS_PolyInfo */
static inline s32 dBgS_GetAttributeCode_l(u32 poly) { return gabi::call<s32>(0x024EF0F4, dComIfG_Bgsp(), poly); }

/* virtual daPy_lk_c::voiceStart(u32) (vtable slot 0xE4) */
#define LK_voiceStart(n) gabi::call_ptr(gabi::load<u32>(__vtbl + 0xE4), this, (u32)(n))
/* 02018808 cM3dGLin::SetStartEnd(start, end) (dCcD_Cps + 0x118) */
static inline void cps_SetStartEnd_l(u32 cps, const cXyz* s, const cXyz* e) { gabi::call(0x02018808, cps + 0x118, s, e); }
/* dCcD_Cps at-vector (+0x7C): SetAtVec (word copies) */
static inline void cps_SetAtVec_l(u32 cps, const cXyz* v) { gabi::at<cXyz>(cps + 0x7C)->copy(*v); }

/* play + 0x5CD8: dComIfGp_checkPlayerStatus0(0, flag) */
static inline u32 dComIfGp_checkPlayerStatus0_l(u32 flag) { return gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & flag; }
/* 0200ECD4 cLib_addCalc(f32* value, f32 target, f32 scale, f32 maxStep, f32 minStep) */
static inline f32 cLib_addCalc_l(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200ECD4, v, target, scale, maxStep, minStep);
}
/* fsel: a >= 0 ? b : c (NaN -> c) */
static inline f32 fsel_l(f32 a, f32 b, f32 c) { return a >= 0.0f ? b : c; }

/* ---- functions of other ranges / classes (by address) ---- */
enum : u32 {
    LK_followEcallBack_end = 0x023DFA48,    /* daPy_followEcallBack_c::end */
    LK_mtxFollowEcallBack_end = 0x023D4538, /* daPy_mtxFollowEcallBack_c::end */
    LK_checkHeavyStateOn = 0x023DBC24,
};

/* daPy_lk_c members in the opaque blocks of the shared header (offsets measured in this range) */
#define LK_FIELD(T, off) (*gabi::at<be<T>>(gabi::ea(this) + (off)))
#define mBodyAngleX LK_FIELD(s16, 0x3D0) /* daPy_py_c mBodyAngle.x (GameCube 0x2B4) */

/* 02400B14 */
BOOL daPy_lk_c::checkAttentionPosAngle(fopAc_ac_c* actor, cXyz** pOutPos) {
    WWHD_FUNC(0x02400B14, BOOL, this, actor, pOutPos);
    if (actor) {
        s16 targetAngle = cLib_targetAngleY(&current.pos, &actor->eyePos);
        int angleDiff = cLib_distanceAngleS(targetAngle, m34DE);
        if (angleDiff <= 0x6000) {
            *gabi::at<be<u32>>(gabi::ea(pOutPos)) = gabi::ea(&actor->eyePos);
            if (actor->group == 2 /* fopAc_ENEMY_e */) {
                setNoResetFlg1(noResetFlg1() | 0x400); /* daPyFlg1_UNK400 */
            }
            return TRUE;
        }
    }
    return FALSE;
}
VERIFY(0x02400B14, &daPy_lk_c::checkAttentionPosAngle);

/* 02400BC4 */
fopAc_ac_c* daPy_lk_c::getDemoLookActor() {
    WWHD_FUNC(0x02400BC4, fopAc_ac_c*, this);
    if (demoParam0() == 1) {
        u32 play = dComIfGp_ea(); /* dComIfGp_event_getPt1() */
        return gabi::call<fopAc_ac_c*>(0x0253EE04 /* dEvt_control_c::convPId */, play + 0x51D0, gabi::load<u32>(play + 0x5294));
    } else if (demoParam0() == 2) {
        u32 play = dComIfGp_ea(); /* dComIfGp_event_getPt2() */
        return gabi::call<fopAc_ac_c*>(0x0253EE04, play + 0x51D0, gabi::load<u32>(play + 0x5298));
    } else if (demoParam0() == 3) {
        return gabi::call<fopAc_ac_c*>(0x025D7C6C /* fopAcM_getTalkEventPartner */, this);
    }
    return NULL;
}
VERIFY(0x02400BC4, &daPy_lk_c::getDemoLookActor);

/* 02402FFC */
void daPy_lk_c::checkOriginalHatAnimation() {
    WWHD_FUNC(0x02402FFC, void, this);
    if ((mModeFlg & 0x01040010 /* ModeFlg_WHIDE | ModeFlg_SWIM | ModeFlg_CRAWL */) ||
        mCurProc == 0x97 /* daPyProc_VOMIT_WAIT_e */ ||
        mCurProc == 0x6C /* daPyProc_ELEC_DAMAGE_e */ ||
        mCurProc == 0xC1 /* daPyProc_DEMO_DOOR_OPEN_e */ ||
        (lk_isStartStage(0x100359CC /* "GTower" */) && dComIfGp_getStartStageLayer_l() == 9)) {
        setResetFlg0(resetFlg0() | 0x800000); /* daPyRFlg0_ORIGINAL_HAT_ANIM */
    }
}
VERIFY(0x02402FFC, &daPy_lk_c::checkOriginalHatAnimation);

/* 02406D60: HD, new: a colour channel (0..255) to linear light, clamped to [0, 1] (pow(c / 255, 2.2));
 * used for the HD emitter colours of setCollision's function-local statics */
static f32 lk_colorGamma(u32 c) {
    WWHD_FUNC(0x02406D60, f32, c);
    f32 in = (f32)c / 255.0f;
    f32 out = gabi::call<f32>(0x028F4560 /* powf */, in, 2.2f);
    if (out < 0.0f) {
        return 0.0f;
    }
    if (out > 1.0f) {
        return 1.0f;
    }
    return out;
}
VERIFY(0x02406D60, lk_colorGamma);

/* 02406DE0 */
void daPy_lk_c::setCutWaterSplash() {
    WWHD_FUNC(0x02406DE0, void, this);
    if (gabi::load<u32>(gabi::ea(this) + 0x6784) != 0) { /* m336C.getEmitter() */
        gabi::Local<cXyz> pos;
        Mtx34* mtx = lk_getAnmMtx(mpCLModel, 4 /* CL_JNT_CHEST_JNT_e */);
        pos->x = mtx->m[0][3]; /* mDoMtx_multVecZero */
        pos->y = mtx->m[1][3];
        pos->z = mtx->m[2][3];
        /* dComIfGp_particle_setP1(dPa_name::ID_IT_JN_LK_NURE_SHIBUKI00, &pos) */
        dPa_control_set(dComIfGp_getParticle(), 1, 0x39, pos, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
    }
}
VERIFY(0x02406DE0, &daPy_lk_c::setCutWaterSplash);

/* 02407178 */
f32 daPy_lk_c::getBlurTopRate() {
    WWHD_FUNC(0x02407178, f32, this);
    s32 proc = mCurProc;
    if (proc == 0xD8 /* daPyProc_DEMO_LAST_COMBO_e */) {
        return 0.0f;
    }
    if (mEquipItem == 0x103 /* daPyItem_SWORD_e */) {
        if (lk_checkNormalSwordEquip()) {
            return 0.5f;
        } else {
            return 1.0f;
        }
    }
    if (mEquipItem == 0x101 /* daPyItem_BOKO_e */) {
        fopAc_ac_c* boko = mActorKeepEquip.mActor;
        if (boko == NULL) {
            return 0.0f;
        }
        if (proc == 0x5B /* daPyProc_JUMP_CUT_e */ || proc == 0x5C /* daPyProc_JUMP_CUT_LAND_e */) {
            return gabi::load<f32>(0x10192150 + (fopAcM_GetParam(boko) << 2)); /* daBoko_c::getJumpBlurRate() */
        } else {
            return gabi::load<f32>(0x10192168 + (fopAcM_GetParam(boko) << 2)); /* daBoko_c::getBlurRate() */
        }
    }
    return 0.0f;
}
VERIFY(0x02407178, &daPy_lk_c::getBlurTopRate);

/* 02407270 */
int daPy_lk_c::getSwordBlurColor() {
    WWHD_FUNC(0x02407270, int, this);
    if (mEquipItem == 0x103 /* daPyItem_SWORD_e */) {
        if (gabi::call<BOOL>(0x023D937C /* checkChanceMode */, this)) {
            return 2;
        }
        if (noResetFlg1() & 0x8000 /* daPyFlg1_SOUP_POWER_UP */) {
            return 1;
        }
    }
    return 0;
}
VERIFY(0x02407270, &daPy_lk_c::getSwordBlurColor);

/* 02407678: daPy_lk_c::fanWindCrashEffectDraw (the matcher had no name; the GameCube function:
 * mpYbafo00Btk's frame (+0x5574) <= its frame count (+0x557A) - 0.5) */
BOOL daPy_lk_c::fanWindCrashEffectDraw() {
    WWHD_FUNC(0x02407678, BOOL, this);
    f32 end = (f32)LK_FIELD(s16, 0x557A) - 0.5f;
    return !(LK_FIELD(f32, 0x5574) > end);
}
VERIFY(0x02407678, &daPy_lk_c::fanWindCrashEffectDraw);

/* 0240CA4C */
static void daPy_waterDropEcallBack_end(void* cb) {
    WWHD_FUNC(0x0240CA4C, void, cb);
    u32 p = gabi::ea(cb);
    if (gabi::load<u32>(p + 4) != 0) {
        u32 e = gabi::load<u32>(p + 4); /* quitImmortalEmitter */
        gabi::store<u32>(e + 0x254, gabi::load<u32>(e + 0x254) & ~0x40u);
        gabi::call(0x0281DE68 /* JPABaseEmitter::deleteAllParticle */, gabi::load<u32>(p + 4), 1);
        gabi::store<u32>(gabi::load<u32>(p + 4) + 0x1E8, 0); /* setParticleCallBackPtr(NULL) */
        gabi::call(0x023DFA48 /* daPy_followEcallBack_c::end */, cb);
    }
}
VERIFY(0x0240CA4C, daPy_waterDropEcallBack_end);

/* 0240EBF0 */
BOOL daPy_lk_c::playerDelete() {
    WWHD_FUNC(0x0240EBF0, BOOL, this);
    u32 b = gabi::ea(this);
    for (int i = 0; i < 2; i++) {
        u32 fe = b + 0x65FC + i * 0x4C; /* mFootEffect[i] */
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(fe) + 0x44), fe);               /* getSmokeCallBack()->remove() */
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(fe + 0x20) + 0x44), fe + 0x20); /* getOtherCallBack()->remove() */
    }
    if (gabi::load<u32>(b + 0x6810) != 0) { /* mFanSwingCb.deleteCallBack() */
        u32 e = gabi::load<u32>(b + 0x6810);
        gabi::store<u32>(e + 0x254, gabi::load<u32>(e + 0x254) & ~0x40u); /* quitImmortalEmitter */
        gabi::store<u32>(gabi::load<u32>(b + 0x6810) + 0x1E4, 0);           /* setEmitterCallBackPtr(NULL) */
        gabi::store<u32>(b + 0x6810, 0);
    }
    gabi::call(LK_followEcallBack_end, b + 0x67A0);    /* m338C */
    gabi::call(LK_mtxFollowEcallBack_end, b + 0x67BC); /* m33A8 */
    gabi::call(0x025A9270 /* dPa_rippleEcallBack::end */, b + 0x6694);
    gabi::call(0x0240CA4C /* daPy_waterDropEcallBack_c::end */, b + 0x6760);
    gabi::call(0x0240CA4C, b + 0x6780);
    gabi::call(0x0240EBBC /* daPy_swimTailEcallBack_c::remove */, b + 0x66A8);
    gabi::call(0x0240EBBC, b + 0x66D0);
    gabi::call(LK_mtxFollowEcallBack_end, b + 0x66F8); /* m32E4 */
    gabi::call(LK_mtxFollowEcallBack_end, b + 0x6704); /* m32F0 */
    gabi::call(LK_mtxFollowEcallBack_end, b + 0x67FC); /* m33E8 */
    gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(b + 0x6710) + 0x44), b + 0x6710); /* mSmokeEcallBack.remove() */
    gabi::call(0x025A9D64 /* dPa_cutTurnEcallBack_c::end */, b + 0x6730);
    gabi::call(0x025A9D64, b + 0x6740);
    gabi::call(0x025A9D64, b + 0x6750);
    gabi::call(LK_mtxFollowEcallBack_end, b + 0x6814); /* m3400 */
    gabi::call(LK_followEcallBack_end, b + 0x6824);    /* m3410 */
    gabi::call(LK_followEcallBack_end, b + 0x684C);    /* m3438 */
    gabi::call(LK_mtxFollowEcallBack_end, b + 0x6840); /* m342C */
    gabi::call(LK_mtxFollowEcallBack_end, b + 0x6868); /* m3454 */
    gabi::call(LK_mtxFollowEcallBack_end, b + 0x6874); /* m3460[0] */
    gabi::call(LK_mtxFollowEcallBack_end, b + 0x6884); /* m3460[1] */
    gabi::call(0x023DCA08 /* endDamageEmitter */, this);
    dKy_plight_cut(gabi::at<LIGHT_INFLUENCE>(b + 0x68B0));
    mDoAud_seDeleteObject(gabi::at<cXyz>(b + 0x3E4));  /* mSwordTopPos */
    mDoAud_seDeleteObject(gabi::at<cXyz>(b + 0x408));  /* mRopePos */
    mDoAud_seDeleteObject(gabi::at<cXyz>(b + 0x67A8)); /* m338C.getPos() */
    mDoAud_seDeleteObject(gabi::at<cXyz>(b + 0x7FE8)); /* mFanWindCps.GetEndP() */
    for (int i = 0; i < 2; i++) {
        gabi::call(0x025E3868 /* mDoExt_destroySolidHeap */, gabi::load<u32>(b + 0x5854 + i * 0x10)); /* m_anm_heap_under[i].mpAnimeHeap */
    }
    for (int i = 0; i < 3; i++) {
        gabi::call(0x025E3868, gabi::load<u32>(b + 0x5874 + i * 0x10)); /* m_anm_heap_upper[i].mpAnimeHeap */
    }
    gabi::call(0x025E3868, gabi::load<u32>(b + 0x65DC)); /* m_tex_anm_heap.mpAnimeHeap */
    gabi::call(0x025E3868, gabi::load<u32>(b + 0x65EC)); /* m_tex_scroll_heap.mpAnimeHeap */
    gabi::call(0x025E3868, gabi::load<u32>(b + 0x4438)); /* mpItemHeaps[0] */
    gabi::call(0x025E3868, gabi::load<u32>(b + 0x443C)); /* mpItemHeaps[1] */
    gabi::call(0x025E3868, gabi::load<u32>(b + 0x4808)); /* mpItemAnimeHeap */
    gabi::call(0x02801D3C /* JAIAnimeSound::stop */, b + 0x64F0); /* mJAIZelAnime.stop() */
    if (noResetFlg1() & 8 /* daPyFlg1_CASUAL_CLOTHES */) {
        /* HD: *mpCurrLinktex = mOtherLinktex became a texture swap call on the HD object at 0x48C */
        gabi::call(0x0272B8B8, (u32)mpCurrLinktex, b + 0x48C);
    }
    u32 s0 = dComIfGp_ea() + 0x5CD8; /* dComIfGp_clearPlayerStatus0(0, daPyStts0_BOOMERANG_WAIT_e) */
    gabi::store<u32>(s0, gabi::load<u32>(s0) & ~0x400000u);
    u32 s1 = dComIfGp_ea() + 0x5CDC; /* dComIfGp_clearPlayerStatus1(0, daPyStts1_UNK40000_e) */
    gabi::store<u32>(s1, gabi::load<u32>(s1) & ~0x40000u);
    gabi::store<u8>(dComIfGp_ea() + 0x5BD1, 0); /* dComIfGp_setMetronomeOff() */
    cancelNoDamageMode();
    return TRUE;
}
VERIFY(0x0240EBF0, &daPy_lk_c::playerDelete);

/* 02405BC0 */
void daPy_lk_c::setFootMark(cXyz* i_pos) {
    WWHD_FUNC(0x02405BC0, void, this, i_pos);
    gabi::Local<cXyz> pos;
    f32 y = i_pos->y + 5.0f;
    f32 z = i_pos->z;
    f32 x = i_pos->x;
    pos->z = z;
    mGndChkPos.x = x; /* mGndChk.SetPos(&pos) */
    mGndChkPos.z = z;
    pos->y = y;
    pos->x = x;
    mGndChkPos.y = y;
    f64 gy = gabi::call<f64>(0x02008974 /* cBgS::GroundCross */, dComIfG_Bgsp(), mGndChk);
    pos->y = (f32)gy;
    if (gy != -1000000000.0 /* -G_CM3D_F_INF */ && dBgS_GetAttributeCode_l(mGndChkPoly) == 0xB /* dBgS_Attr_SAND_e */) {
        gabi::call(0x025DADA4 /* fopKyM_create */, 0x1D4 /* fpcNm_WATER_MARK_e */, ((u32)(s32)shape_angle.y << 16) | 2, pos.get(), &scale, 0);
    }
    if (gabi::load<u32>(gabi::ea(this) + 0x6784) != 0) { /* m336C.getEmitter() */
        gabi::call(0x025DADA4, 0x1D4, (u32)(s32)shape_angle.y << 16, i_pos, &scale, 0);
    }
}
VERIFY(0x02405BC0, &daPy_lk_c::setFootMark);

/* daPy_swBlur_c (HD layout; J3DPacket base, the GameCube members +0x88) */
struct daPy_swBlur_l {
    /* 0x000 */ u8 _000[0x9C];
    /* 0x09C */ be<s32> field_0x014;
    /* 0x0A0 */ be<s32> field_0x018;
    /* 0x0A4 */ be<s32> mBlurColorType;
    /* 0x0A8 */ be<f32> mBlurTopRate;
    /* 0x0AC */ be<u32> mpPosBuffer;
    /* 0x0B0 */ cXyz field_0x028;
    /* 0x0BC */ cXyz field_0x034[60];
    /* 0x38C */ cXyz field_0x304[60];
};
WWHD_OFFSET(daPy_swBlur_l, field_0x304, 0x38C);

/* 02407070 */
static void daPy_swBlur_initSwBlur(daPy_swBlur_l* b, Mtx34* mtx, int idx, f32 blurTopRate, int blurColorType) {
    WWHD_FUNC(0x02407070, void, b, mtx, idx, blurTopRate, blurColorType);
    int i = idx * 2;
    b->mBlurTopRate = blurTopRate;
    b->mBlurColorType = blurColorType;
    PSMTXMultVec(mtx, gabi::at<cXyz>(b->mpPosBuffer + i * 0xC), &b->field_0x034[0]);
    PSMTXMultVec(mtx, gabi::at<cXyz>(b->mpPosBuffer + i * 0xC + 0xC), &b->field_0x304[0]);
    gabi::Local<cXyz> d;
    cXyz_mi(&b->field_0x034[0], d, &b->field_0x304[0]);
    gabi::Local<cXyz> s;
    cXyz_ml(d, s, b->mBlurTopRate);
    PSVECAdd(&b->field_0x034[0], s, &b->field_0x034[0]);
    b->field_0x034[1].copy(b->field_0x034[0]);
    b->field_0x304[1].copy(b->field_0x304[0]);
    b->field_0x014 = 0;
    b->field_0x018 = idx;
    u32 m = gabi::ea(mtx); /* field_0x028.set(mtx[0][3], mtx[1][3], mtx[2][3]) */
    fcpy_l(gabi::ea(&b->field_0x028) + 0, m + 0x0C);
    fcpy_l(gabi::ea(&b->field_0x028) + 8, m + 0x2C);
    fcpy_l(gabi::ea(&b->field_0x028) + 4, m + 0x1C);
}
VERIFY(0x02407070, daPy_swBlur_initSwBlur);

/* 02407310 */
static void daPy_swBlur_copySwBlur(daPy_swBlur_l* b, Mtx34* mtx, int param_2) {
    WWHD_FUNC(0x02407310, void, b, mtx, param_2);
    int var_r31 = (s32)((u32)param_2 - (u32)(s32)b->field_0x018);
    b->field_0x018 = param_2;
    int var_r30;
    if (var_r31 > 0) {
        var_r30 = -2;
        if (var_r31 > 60) {
            var_r31 = 60;
        }
    } else if (var_r31 < 0) {
        var_r30 = 2;
        if (var_r31 < -60) {
            var_r31 = 60;
        } else {
            var_r31 = -var_r31;
        }
    } else {
        var_r31 = 10;
        var_r30 = 0;
    }
    for (int i = 59 - var_r31; i >= 0; i--) {
        b->field_0x034[i + var_r31].copy(b->field_0x034[i]);
        b->field_0x304[i + var_r31].copy(b->field_0x304[i]);
    }
    f32 var_f31 = 0.0f;
    f32 frac = 1.0f / (f32)var_r31;
    s32 buffIdx = (s32)((u32)(s32)b->field_0x018 << 1); /* wraps like the original */
    gabi::Local<cXyz> sp50;
    u32 m = gabi::ea(mtx);
    fcpy_l(gabi::ea(sp50.get()) + 4, m + 0x1C);
    fcpy_l(gabi::ea(sp50.get()) + 0, m + 0x0C);
    fcpy_l(gabi::ea(sp50.get()) + 8, m + 0x2C);
    gabi::Local<cXyz> d;
    cXyz_mi(&b->field_0x028, d, sp50);
    gabi::Local<cXyz> sp38;
    sp38->copy(*d);
    b->field_0x028.copy(*sp50);
    gabi::Local<cXyz> t;
    gabi::Local<cXyz> u;
    for (int i = 0; i < var_r31 && buffIdx >= 0; i++) {
        PSMTXMultVec(mtx, gabi::at<cXyz>(b->mpPosBuffer + (u32)buffIdx * 0xC), &b->field_0x034[i]);
        PSMTXMultVec(mtx, gabi::at<cXyz>(b->mpPosBuffer + (u32)buffIdx * 0xC + 0xC), &b->field_0x304[i]);
        cXyz_mi(&b->field_0x034[i], t, &b->field_0x304[i]);
        cXyz_ml(t, u, b->mBlurTopRate);
        PSVECAdd(&b->field_0x034[i], u, &b->field_0x034[i]);
        cXyz_ml(sp38, t, var_f31);
        PSVECAdd(&b->field_0x034[i], t, &b->field_0x034[i]);
        cXyz_ml(sp38, t, var_f31);
        PSVECAdd(&b->field_0x304[i], t, &b->field_0x304[i]);
        var_f31 += frac;
        buffIdx = (s32)((u32)buffIdx + (u32)var_r30);
    }
    s32 n = (s32)((u32)(s32)b->field_0x014 + (u32)var_r31);
    if (n >= 59) {
        n = 58;
    }
    b->field_0x014 = n;
}
VERIFY(0x02407310, daPy_swBlur_copySwBlur);

/* 024036E4 */
void daPy_lk_c::checkRoofRestart() {
    WWHD_FUNC(0x024036E4, void, this);
    u32 fl = mAcch.m_flags;
    if (!(fl & 0x200) /* !mAcch.ChkRoofHit() */ || !(fl & 0x20) /* !mAcch.ChkGroundHit() */) {
        return;
    }
    if (mCurProc == 0xF /* daPyProc_CRAWL_START_e */ || mCurProc == 0x12 /* daPyProc_CRAWL_END_e */ ||
        mCurProc == 0x13 /* daPyProc_WHIDE_READY_e */) {
        return;
    }
    u32 stageDt = dComIfGp_ea() + 0x5150; /* dComIfGp_getStageStagInfo() (virtual, +0x15C) */
    u32 stagInfo = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(stageDt) + 0x15C), stageDt);
    if (((gabi::load<u32>(stagInfo + 0xC) >> 16) & 7) == 3 /* dStage_stagInfo_GetSTType() == dStageType_BOSS_e */) {
        return;
    }
    if (gabi::call<BOOL>(0x024EEABC /* dBgS::ChkMoveBG */, dComIfG_Bgsp(), gabi::ea(this) + 0x934 /* mAcch.m_roof */)) {
        u32 actor = gabi::call<u32>(0x02008438 /* cBgS::GetActorPointer */, dComIfG_Bgsp(), (u32)LK_FIELD(u16, 0x936));
        if (actor != 0 && gabi::load<s16>(actor + 8) == 0x59 /* fpcNm_BRIDGE_e */) {
            return;
        }
    }
    f32 roof_height = LK_FIELD(f32, 0x8C8); /* mAcch.GetRoofHeight() */
    if (roof_height > LK_FIELD(f32, 0x8A0) /* mAcch.GetGroundH() */) {
        f32 y = current.pos.y;
        f32 dVar10 = LK_FIELD(f32, 0x3DC) /* mHeadTopPos.y */ - 10.0f;
        f32 lo = y + 10.0f;
        if (dVar10 < lo) {
            dVar10 = lo;
        }
        if (roof_height < dVar10) {
            if (mModeFlg & 0x10 /* ModeFlg_WHIDE */) {
                u16 a = (u16)shape_angle.y;
                f32 s = cM_ssin(a);
                f32 c = cM_scos(a);
                u32 rp = gabi::ea(this) + 0xBA0; /* mRoofChk.SetPos(local_8) */
                gabi::store<f32>(rp + 4, y);
                gabi::store<f32>(rp + 0, gabi::fmadds(26.5f, s, current.pos.x)); /* HD: 35.0f - HIO folded */
                gabi::store<f32>(rp + 8, gabi::fmadds(26.5f, c, current.pos.z));
                f64 r = gabi::call<f64>(0x024EF6E8 /* dBgS::RoofChk */, dComIfG_Bgsp(), gabi::ea(this) + 0xB68);
                if (!(r < (f64)dVar10)) {
                    return;
                }
            }
            if (gabi::call<BOOL>(0x023FD4E4 /* startRestartRoom */, this, 5, 0xC9, -1.0f, 0)) {
                LK_voiceStart(43);
            }
        }
    }
}
VERIFY(0x024036E4, &daPy_lk_c::checkRoofRestart);

/* 02406E74 */
void daPy_lk_c::setSwordAtCollision() {
    WWHD_FUNC(0x02406E74, void, this);
    u32 b = gabi::ea(this);
    cXyz* swordTop = gabi::at<cXyz>(b + 0x3E4); /* mSwordTopPos */
    gabi::Local<cXyz> local_34;
    gabi::Local<cXyz> local_58;
    if (mEquipItem == 0x103 /* daPyItem_SWORD_e */) {
        gabi::Local<cXyz> t0;
        gabi::Local<cXyz> t1;
        gabi::Local<cXyz> t2;
        cXyz_mi(swordTop, t0, &m36C4);
        cXyz_ml(t0, t1, m35FC);
        cXyz_pl(t1, t2, &m36C4);
        local_34->copy(*t2);
        cXyz_mi(&m36D0, t2, &m36DC);
        cXyz_ml(t2, t1, m35FC);
        cXyz_pl(t1, t0, &m36DC);
        local_58->copy(*t0);
    } else {
        local_34->copy(*swordTop);
        local_58->copy(m36D0);
    }
    cps_SetStartEnd_l(b + 0x7B1C, &m36C4, local_34); /* mAtCps[0] */
    cps_SetStartEnd_l(b + 0x7C54, local_58, local_34);
    cps_SetStartEnd_l(b + 0x7D8C, &m36C4, local_58);
    gabi::Local<cXyz> local_7c;
    if (resetFlg0() & 1 /* daPyRFlg0_UNK1 */) {
        local_7c->copy(*cXyz_Zero);
    } else {
        gabi::Local<cXyz> d;
        cXyz_mi(swordTop, d, &m36D0);
        local_7c->copy(*d);
    }
    cps_SetAtVec_l(b + 0x7B1C, local_7c);
    cps_SetAtVec_l(b + 0x7C54, local_7c);
    cps_SetAtVec_l(b + 0x7D8C, local_7c);
}
VERIFY(0x02406E74, &daPy_lk_c::setSwordAtCollision);

/* 0240CAAC */
void daPy_lk_c::setSwimWaterDrop(void* i_cb) {
    WWHD_FUNC(0x0240CAAC, void, this, i_cb);
    u32 cb = gabi::ea(i_cb); /* daPy_waterDropEcallBack_c: emitter +4, pos +8, ready +0x1C */
    f32 waterY = mWaterY;
    f32 tmp = waterY + 10.0f;
    u32 waterDrop = noResetFlg1() & 0x80000; /* daPyFlg1_WATER_DROP */
    if (waterDrop || !(mNoResetFlg0 & 0x80) /* daPyFlg0_UNK80 */ ||
        (gabi::load<f32>(cb + 0xC) > tmp && !(mModeFlg & 0x40000) /* ModeFlg_SWIM */)) {
        if (gabi::load<u32>(cb + 4) == 0 && (gabi::load<u32>(cb + 0x1C) != 0 || waterDrop)) {
            /* dComIfGp_particle_setP1(dPa_name::ID_IT_JN_LK_NURE_POTA00, &callBack->getPos(), NULL, NULL, 0xFF, callBack) */
            JPABaseEmitter* emitter = dPa_control_set(dComIfGp_getParticle(), 1, 0x38, gabi::at<cXyz>(cb + 8), nullptr, nullptr, 0xFF,
                                                      gabi::at<dPa_levelEcallBack>(cb), -1, nullptr, nullptr, nullptr);
            if (emitter != NULL) {
                gabi::store<u32>(gabi::ea(emitter) + 0x1E8, 0x1046D29C); /* setParticleCallBackPtr(getPcallBack()) */
            }
        }
        gabi::store<u32>(cb + 0x1C, 0); /* offReady() */
        setNoResetFlg1(noResetFlg1() & ~0x80000u);
    } else if (gabi::load<f32>(cb + 0xC) < waterY) {
        gabi::call(0x0240CA4C /* daPy_waterDropEcallBack_c::end */, cb);
        if (mCurProc != 0x97 /* daPyProc_VOMIT_WAIT_e */) {
            gabi::store<u32>(cb + 0x1C, 1); /* onReady() */
        }
    }
}
VERIFY(0x0240CAAC, &daPy_lk_c::setSwimWaterDrop);

/* 0240CBE0 */
void daPy_lk_c::setWaterDrop() {
    WWHD_FUNC(0x0240CBE0, void, this);
    u32 b = gabi::ea(this);
    u32 m = gabi::ea(lk_getAnmMtx(mpCLModel, 0xE /* CL_JNT_NECK_JNT_e */));
    fcpy_l(b + 0x6770, m + 0x2C); /* m334C.setPos(mDoMtx_multVecZero(..)) */
    fcpy_l(b + 0x676C, m + 0x1C);
    fcpy_l(b + 0x6768, m + 0x0C);
    m = gabi::ea(lk_getAnmMtx(mpCLModel, 2 /* CL_JNT_BODY_CHN_e */));
    fcpy_l(b + 0x6790, m + 0x2C); /* m336C.setPos(..) */
    fcpy_l(b + 0x678C, m + 0x1C);
    fcpy_l(b + 0x6788, m + 0x0C);
    gabi::store<u32>(0x1046D2A0, mModeFlg & 1); /* getPcallBack()->on/offWaterMark() */
    if (gabi::load<u16>(0x101CEF16) == 0 /* daPy_dmEcallBack_c::checkFlame() */) {
        gabi::call(0x0240CA4C /* daPy_waterDropEcallBack_c::end */, b + 0x6760);
        gabi::call(0x0240CA4C, b + 0x6780);
        gabi::call(LK_mtxFollowEcallBack_end, b + 0x6874);
        gabi::call(LK_mtxFollowEcallBack_end, b + 0x6884);
        return;
    }
    if (gabi::call<BOOL>(0x02560CD0 /* dKyr_player_overhead_bg_chk */)) {
        u32 stageDt = dComIfGp_ea() + 0x5150;
        u32 stagInfo = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(stageDt) + 0x15C), stageDt);
        if (((gabi::load<u32>(stagInfo + 0xC) >> 16) & 7) != 2 /* dStageType_MISC_e */ && mCurProc != 0xD8 /* daPyProc_DEMO_LAST_COMBO_e */ &&
            LK_FIELD(f32, 0x8C8) == 1000000000.0f /* mAcch.GetRoofHeight() == G_CM3D_F_INF */) {
            if (gabi::load<u32>(b + 0x6878) == 0) {
                Mtx34* nm = lk_getAnmMtx(mpCLModel, 0xE);
                gabi::call(0x023D457C /* daPy_mtxFollowEcallBack_c::makeEmitter */, b + 0x6874, 0x432 /* ID_AK_JN_LK_NURE_POTA01 */, nm, &current.pos, 0);
            }
            if (gabi::load<u32>(b + 0x6888) == 0) {
                Mtx34* bm = lk_getAnmMtx(mpCLModel, 2);
                gabi::call(0x023D457C, b + 0x6884, 0x432, bm, &current.pos, 0);
            }
            gabi::call(0x0240CA4C, b + 0x6760);
            gabi::call(0x0240CA4C, b + 0x6780);
            return;
        }
    }
    gabi::call(LK_mtxFollowEcallBack_end, b + 0x6874);
    gabi::call(LK_mtxFollowEcallBack_end, b + 0x6884);
    gabi::call(0x0240CAAC /* setSwimWaterDrop */, this, b + 0x6760);
    gabi::call(0x0240CAAC, this, b + 0x6780);
}
VERIFY(0x0240CBE0, &daPy_lk_c::setWaterDrop);

/* 0240C0A8 */
void daPy_lk_c::setHammerWaterSplash() {
    WWHD_FUNC(0x0240C0A8, void, this);
    if (mCurProc != 0x53 /* daPyProc_HAMMER_FRONT_SWING_e */ || !(m35EC > mFrameCtrlUnder[0].getRate())) {
        return;
    }
    if (m355C != 0) {
        return;
    }
    cXyz* swordTop = gabi::at<cXyz>(gabi::ea(this) + 0x3E4); /* mSwordTopPos */
    gabi::Local<cXyz> t;
    cXyz_mi(&m36C4, t, swordTop);
    gabi::Local<cXyz> local_28;
    local_28->copy(*t);
    gabi::call(0x0201B31C /* cXyz::normalize (HD: in place, a copy to the result slot) */, local_28.get(), t.get());
    gabi::Local<cXyz> s;
    cXyz_ml(local_28, s, 30.0f);
    cXyz_pl(swordTop, t, s);
    local_28->copy(*t);
    gabi::Local<be<f32>> local_50;
    if (!gabi::call<BOOL>(0x025D9F70 /* fopAcM_getWaterY */, local_28.get(), local_50.get())) {
        return;
    }
    f32 y = local_28->y;
    if (*local_50 < y + 5.0f) {
        return;
    }
    f32 y2 = y + 150.0f;
    mGndChkPos.z = local_28->z; /* mGndChk.SetPos(&local_28) */
    mGndChkPos.x = local_28->x;
    mGndChkPos.y = y2;
    local_28->y = y2;
    f64 gy = gabi::call<f64>(0x02008974 /* cBgS::GroundCross */, dComIfG_Bgsp(), mGndChk);
    if (*local_50 < (f32)(gy + 5.0)) {
        return;
    }
    fcpy_l(gabi::ea(local_28.get()) + 4, gabi::ea(local_50.get()));
    /* dComIfGp_particle_setP1(dPa_name::ID_IT_JN_HM_WP00, &local_28) */
    dPa_control_set(dComIfGp_getParticle(), 1, 0x27C, local_28, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
    /* dComIfGp_particle_setShipTail(dPa_name::ID_IT_JN_WP_HAMON01, .., &dPa_control_c::mSingleRippleEcallBack) */
    dPa_control_set(dComIfGp_getParticle(), 5, 0x3D, local_28, nullptr, nullptr, 0xFF, gabi::at<dPa_levelEcallBack>(0x1047B2E4), -1,
                    nullptr, nullptr, nullptr);
    gabi::Local<GXColor> amb;
    gabi::Local<GXColor> dif;
    gabi::call(0x025602F0 /* dKy_get_seacolor */, amb.get(), dif.get());
    JPABaseEmitter* emitter = dPa_control_set(dComIfGp_getParticle(), 1, 0x23 /* ID_AK_JN_ELEMENTSHIBUKI00 */, local_28, nullptr,
                                              nullptr, 0xFF, nullptr, -1, amb, nullptr, nullptr);
    if (emitter != NULL) {
        u32 e = gabi::ea(emitter);
        gabi::store<f32>(e + 0x58, 1.0f);  /* setSpread(1.0f) */
        gabi::store<f32>(e + 0x34, 60.0f); /* setRate(60.0f) */
        gabi::store<u32>(e + 0x5C, 1);     /* setMaxFrame(1) */
        gabi::store<f32>(e + 0x70, 10.0f); /* setDirectionalSpeed(10.0f) */
        fcpy_l(e + 0x238, 0x1046CD3C);     /* setGlobalParticleScale(l_hammer_splash_particle_scale) */
        fcpy_l(e + 0x23C, 0x1046CD40);
        fcpy_l(e + 0x240, 0x1046CD44);
    }
    m355C = 1;
}
VERIFY(0x0240C0A8, &daPy_lk_c::setHammerWaterSplash);

/* 0240584C */
void daPy_lk_c::checkLightHit() {
    WWHD_FUNC(0x0240584C, void, this);
    /* static JGeometry::TVec3<f32> normal_scale(1.0f, 1.0f, 1.0f), boss_scale(2.5f, 2.5f, 2.5f) (HD: on first use) */
    u32 normal_scale = 0x1046D104;
    u32 boss_scale = 0x1046D110;
    if (gabi::load<u32>(0x1046D0FC) == 0) {
        gabi::store<u32>(0x1046D0FC, 1);
        gabi::store<f32>(normal_scale + 0, 1.0f);
        gabi::store<f32>(normal_scale + 8, 1.0f);
        gabi::store<f32>(normal_scale + 4, 1.0f);
    }
    if (gabi::load<u32>(0x1046D100) == 0) {
        gabi::store<u32>(0x1046D100, 1);
        gabi::store<f32>(boss_scale + 0, 2.5f);
        gabi::store<f32>(boss_scale + 8, 2.5f);
        gabi::store<f32>(boss_scale + 4, 2.5f);
    }
    BOOL lightHit = FALSE;
    if (gabi::call<BOOL>(0x0252A038 /* dDetect_c::chk_light */, dComIfGp_ea() + 0x5A20, &current.pos)) {
        lightHit = TRUE;
        resetCurse();
    }
    u32 b = gabi::ea(this);
    u32 fanLightCps = b + 0x8128; /* mFanLightCps */
    bool doEnd = true;
    if (gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x2F) == 0x3C /* checkMirrorShieldEquip() */) {
        gabi::Local<cXyz> lightVec;
        u32 shield = gabi::load<u32>(b + 0xE80); /* mpEquippedShieldModel->getBaseTRMtx() */
        PSMTXMultVecSR(gabi::at<Mtx34>(shield ? shield + 0xC8 : 0), gabi::at<cXyz>(0x10034FE4) /* l_ms_light_local_vec */, lightVec);
        if (lightHit) {
            setResetFlg0(resetFlg0() | 0x200000); /* daPyRFlg0_LIGHT_REFLECT */
        } else if (gabi::call<BOOL>(0x025162A4 /* dCcD_GObjInf::ChkTgHit */, b + 0x79EC /* mLightCyl */) &&
                   PSVECDotProduct(gabi::at<cXyz>(b + 0x7AAC) /* GetTgRVecP() */, lightVec) < 0.0f) {
            setResetFlg0(resetFlg0() | 0x200000);
        }
        if (gabi::call<BOOL>(0x025160DC /* dCcD_GObjInf::ChkAtHit */, fanLightCps)) {
            doEnd = false;
            if (gabi::load<u32>(b + 0x6850) == 0) { /* m3438.getEmitter() */
                /* dComIfGp_particle_setP1(dPa_name::ID_AK_SN_HITSHIELDLIGHT00, &current.pos, NULL, NULL, 0xFF, &m3438) */
                dPa_control_set(dComIfGp_getParticle(), 1, 0x8232, &current.pos, nullptr, nullptr, 0xFF,
                                gabi::at<dPa_levelEcallBack>(b + 0x684C), -1, nullptr, nullptr, nullptr);
            }
            gabi::call(0x0201794C /* cM3d_lineVsPosSuisenCross */, fanLightCps + 0x118, fanLightCps + 0x124, fanLightCps + 0x70, b + 0x6854);
            gabi::Local<cXyz> xz; /* lightVec.absXZ() */
            xz->x = lightVec->x;
            xz->y = 0.0f;
            xz->z = lightVec->z;
            f32 d = std_sqrtf(PSVECSquareMag(xz));
            s16 ax = cM_atan2s(-lightVec->y, d);
            s16 angleY = cM_atan2s(lightVec->x, lightVec->z);
            gabi::store<s16>(b + 0x6864, 0); /* m3438.setAngle(ax, angleY, 0) */
            gabi::store<s16>(b + 0x6862, angleY);
            gabi::store<s16>(b + 0x6860, ax);
            if (gabi::load<u32>(b + 0x6850) != 0) {
                u32 sc = normal_scale;
                if (gabi::call<u32>(0x02515BBC /* GetAtHitAc */, fanLightCps + 0x50) != 0) {
                    u32 ac = gabi::call<u32>(0x02515BBC, fanLightCps + 0x50);
                    if (ac != 0 && gabi::load<s16>(ac + 8) == 0xD3 /* fpcNm_BPW_e */) {
                        sc = boss_scale;
                    }
                }
                u32 e = gabi::load<u32>(b + 0x6850); /* setGlobalScale (HD: also the particle scale) */
                fcpy_l(e + 0x220, sc + 0);
                fcpy_l(e + 0x224, sc + 4);
                fcpy_l(e + 0x238, sc + 0);
                fcpy_l(e + 0x228, sc + 8);
                fcpy_l(e + 0x23C, sc + 4);
                fcpy_l(e + 0x240, sc + 8);
            }
        }
    }
    if (doEnd) {
        gabi::call(LK_followEcallBack_end, b + 0x684C); /* m3438.end() */
    }
    if (resetFlg0() & 0x200000 /* daPyRFlg0_LIGHT_REFLECT */) {
        if (!(gabi::load<u32>(fanLightCps) & 1) /* !mFanLightCps.ChkAtSet() */) {
            seStartOnlyReverb(0x6961 /* JA_SE_OBJ_MIRROR_REFLECT */);
        } else {
            seStartOnlyReverb(0x7028 /* JA_SE_OBJ_MIRROR_LIGHT */);
        }
        resetCurse();
    }
    if (!(gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & 0x2000) /* daPyStts0_SUBJECT_e */ && (resetFlg0() & 0x200000)) {
        if (gabi::load<u32>(b + 0x6844) == 0) { /* m342C.getEmitter() */
            u32 shield = gabi::load<u32>(b + 0xE80);
            gabi::call(0x023D457C /* daPy_mtxFollowEcallBack_c::makeEmitter */, b + 0x6840, 0x8226 /* ID_AK_SN_MIRRORSHIELD00 */,
                       shield ? shield + 0xC8 : 0, &current.pos, 0);
        }
    } else if (gabi::load<u32>(b + 0x6844) != 0) {
        gabi::store<u8>(gabi::load<u32>(b + 0x6844) + 0x247, 0); /* setGlobalAlpha(0) */
        gabi::call(LK_mtxFollowEcallBack_end, b + 0x6840);
    }
}
VERIFY(0x0240584C, &daPy_lk_c::checkLightHit);

/* 0240A0A4 */
void daPy_lk_c::setAttentionPos() {
    WWHD_FUNC(0x0240A0A4, void, this);
    u32 b = gabi::ea(this);
    u32 atn = b + 0x390; /* attention_info.position */
    fcpy_l(atn + 0, b + 0x314);
    fcpy_l(atn + 8, b + 0x31C);
    u32 ship = gabi::load<u32>(dComIfGp_ea() + 0x5B3C); /* dComIfGp_getShipActor() */
    if (mModeFlg & 0x01000000 /* ModeFlg_CRAWL */) {
        u32 m = gabi::ea(mpCLModel.get());
        PSMTXMultVec(gabi::at<Mtx34>(m ? m + 0xC8 : 0), gabi::at<cXyz>(0x10035A64) /* offset {0, 30, 20} */, gabi::at<cXyz>(atn));
        return;
    }
    if (gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & 0x2000 /* daPyStts0_SUBJECT_e */) {
        gabi::Local<cXyz> afStack_18;
        gabi::call(0x025F1AA4 /* mDoMtx_ZXYrotS */, mDoMtx_stack_c::get(), (s32)mBodyAngleX, (s32)shape_angle.y, 0);
        PSMTXMultVec(mDoMtx_stack_c::get(), gabi::at<cXyz>(0x10035A70) /* offset {0, 70, 0} */, afStack_18);
        gabi::store<f32>(atn + 4, current.pos.y + 26.3f);
        PSVECAdd(gabi::at<cXyz>(atn), afStack_18, gabi::at<cXyz>(atn));
        return;
    }
    u32 mf = mModeFlg;
    if (mf & 0x00800000 /* ModeFlg_CROUCH */) {
        u16 a = (u16)shape_angle.y;
        f32 ax = gabi::load<f32>(atn + 0);
        f32 s = cM_ssin(a);
        f32 az = gabi::load<f32>(atn + 8);
        gabi::store<f32>(atn + 4, current.pos.y + 46.25f);
        gabi::store<f32>(atn + 0, gabi::fmadds(20.0f, s, ax));
        gabi::store<f32>(atn + 8, gabi::fmadds(20.0f, cM_scos(a), az));
        return;
    }
    if (mf & 0x800 /* ModeFlg_ROPE */) {
        gabi::at<cXyz>(atn)->copy(current.pos);
        return;
    }
    if (mf & 0x20 /* ModeFlg_HANG */) {
        gabi::store<f32>(atn + 4, LK_FIELD(f32, 0x7778) /* mCyl.GetCP()->y */ + 92.5f);
        return;
    }
    if (mf & 0x40000 /* ModeFlg_SWIM */) {
        gabi::store<f32>(atn + 4, current.pos.y + 20.05f);
        return;
    }
    u32 m = gabi::ea(mpCLModel.get()); /* getBaseTRMtx() */
    if ((mf & 0x10 /* ModeFlg_WHIDE */) && (mNoResetFlg0 & 0x10000 /* daPyFlg0_UNK10000 */)) {
        gabi::store<f32>(atn + 4, gabi::load<f32>((m ? m + 0xC8 : 0) + 0x1C) + 62.5f); /* HD: (92.5f + y) - 30.0f folded */
        return;
    }
    if ((mf & 0x2000 /* ModeFlg_IN_SHIP */) && ship != 0 && !(gabi::load<u32>(ship + 0x644) & 1) /* !ship->getFlyFlg() */) {
        f32 s = cM_ssin((u16)gabi::load<s16>(ship + 0x32A));
        gabi::store<f32>(atn + 0, gabi::fmadds(-35.0f, s, gabi::load<f32>(ship + 0x314))); /* l_ship_offset.z folded */
        if (mNoResetFlg0 & 0x80 /* daPyFlg0_UNK80 */) {
            gabi::store<f32>(atn + 4, (mWaterY + 15.0f) + 92.5f);
        } else {
            u32 m2 = gabi::ea(mpCLModel.get());
            gabi::store<f32>(atn + 4, gabi::load<f32>((m2 ? m2 + 0xC8 : 0) + 0x1C) + 92.5f);
        }
        f32 c = cM_scos((u16)gabi::load<s16>(ship + 0x32A));
        gabi::store<f32>(atn + 8, gabi::fmadds(-35.0f, c, gabi::load<f32>(ship + 0x31C)));
        return;
    }
    u32 m3 = gabi::ea(mpCLModel.get());
    gabi::store<f32>(atn + 4, gabi::load<f32>((m3 ? m3 + 0xC8 : 0) + 0x1C) + 92.5f);
}
VERIFY(0x0240A0A4, &daPy_lk_c::setAttentionPos);

/* Link's joint matrix sj copied into joint dj of another model (both marked dirty; twelve FPR
 * loads, then the stores) */
static inline void lk_copyJntMtx(J3DModel* link, s32 sj, J3DModel* dst, s32 dj) {
    u32 s = gabi::ea(lk_getAnmMtx(link, sj));
    u32 d = gabi::ea(lk_getAnmMtx(dst, dj));
    u32 t[12];
    for (u32 i = 0; i < 12; i++) {
        t[i] = lk_fbits(gabi::load<f32>(s + i * 4)); /* lfs (quiets a signalling NaN) */
    }
    for (u32 i = 0; i < 12; i++) {
        gmem_stf32(d + i * 4, t[i]);
    }
}
/* 024038A0: daPy_lk_c::setBootsModel (the matcher had no name: the GameCube function; copies Link's
 * leg/foot joint matrices into the two boots models (models[0] the right side: joints 38 and 40/39;
 * models[1]: 33 and 35/34); with the heavy boots (daPyFlg0 0x2000000) the first joint also follows the
 * leg and the copies move one joint on) */
void daPy_lk_c::setBootsModel(J3DModel** models) {
    WWHD_FUNC(0x024038A0, void, this, models);
    u32 mp = gabi::ea(models);
    s32 j = 1;
    if (mNoResetFlg0 & 0x2000000) {
        lk_copyJntMtx(mpCLModel, 38, gabi::at<J3DModel>(gabi::load<u32>(mp + 0)), 1);
        lk_copyJntMtx(mpCLModel, 33, gabi::at<J3DModel>(gabi::load<u32>(mp + 4)), 1);
        j = 2;
    }
    J3DModel* m0 = gabi::at<J3DModel>(gabi::load<u32>(mp + 0));
    lk_copyJntMtx(mpCLModel, 40, m0, j);
    lk_copyJntMtx(mpCLModel, 39, m0, j + 1);
    J3DModel* m1 = gabi::at<J3DModel>(gabi::load<u32>(mp + 4));
    lk_copyJntMtx(mpCLModel, 35, m1, j);
    lk_copyJntMtx(mpCLModel, 34, m1, j + 1);
}
VERIFY(0x024038A0, &daPy_lk_c::setBootsModel);

/* J3DModel::setAnmMtx(jnt, mDoMtx_stack_c::get()) (HD: marks the joint matrices dirty; twelve FPR
 * loads, then the stores) */
static inline void lk_setAnmMtx(J3DModel* m, s32 jnt) {
    u32 d = gabi::ea(lk_getAnmMtx(m, jnt));
    u32 s = gabi::ea(mDoMtx_stack_c::get());
    u32 t[12];
    for (u32 i = 0; i < 12; i++) {
        t[i] = lk_fbits(gabi::load<f32>(s + i * 4)); /* lfs (quiets a signalling NaN) */
    }
    for (u32 i = 0; i < 12; i++) {
        gmem_stf32(d + i * 4, t[i]);
    }
}
/* mDoMtx_stack_c::transM(trans_info[jnt].mTranslate) then quatM(&quaternion[jnt]) */
static inline void lk_transQuatM(u32 transInfo, u32 quat, s32 jnt) {
    u32 t = transInfo + jnt * 0x20 + 0x14;
    mDoMtx_stack_c::transM(gabi::load<f32>(t + 0), gabi::load<f32>(t + 4), gabi::load<f32>(t + 8));
    gabi::call(0x025F25CC /* mDoMtx_stack_c::quatM */, quat + jnt * 0x10);
}

/* 02403150 */
BOOL daPy_lk_c::jointCB1() {
    WWHD_FUNC(0x02403150, BOOL, this);
    u32 fdata = m_old_fdata;
    if (gabi::load<u8>(fdata) == 0 /* m_old_fdata->getOldFrameFlg() */) {
        return FALSE;
    }
    u32 b = gabi::ea(this);
    u32 trans_info = gabi::load<u32>(fdata + 0x1C); /* getOldFrameTransInfo(0) */
    u32 quaternion = gabi::load<u32>(fdata + 0x20); /* getOldFrameQuaternion(0) */
    gabi::call(0x025F181C /* mDoMtx_ZrotS */, mDoMtx_stack_c::get(), (s32)gabi::load<s16>(b + 0x7510) /* mFootData[1].field_0x008 */);
    gabi::call(0x028E9108 /* PSMTXConcat */, lk_getAnmMtx(mpCLModel, 32 /* CL_JNT_LLEGA_JNT_e */), mDoMtx_stack_c::get(), mDoMtx_stack_c::get());
    lk_setAnmMtx(mpCLModel, 32);
    lk_transQuatM(trans_info, quaternion, 33);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), gabi::load<s16>(b + 0x7512) /* mFootData[1].field_0x00A */);
    lk_setAnmMtx(mpCLModel, 33 /* CL_JNT_LLEGB_JNT_e */);
    lk_transQuatM(trans_info, quaternion, 34);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), gabi::load<s16>(b + 0x750A) /* mFootData[1].field_0x002 */);
    lk_setAnmMtx(mpCLModel, 34 /* CL_JNT_LFOOT_JNT_e */);
    lk_transQuatM(trans_info, quaternion, 35);
    lk_setAnmMtx(mpCLModel, 35 /* CL_JNT_LTOE_JNT_e */);

    fdata = m_old_fdata;
    trans_info = gabi::load<u32>(fdata + 0x1C);
    quaternion = gabi::load<u32>(fdata + 0x20);
    gabi::call(0x025F181C /* mDoMtx_ZrotS */, mDoMtx_stack_c::get(), (s32)gabi::load<s16>(b + 0x73F8) /* mFootData[0].field_0x008 */);
    gabi::call(0x028E9108 /* PSMTXConcat */, lk_getAnmMtx(mpCLModel, 37 /* CL_JNT_RLEGA_JNT_e */), mDoMtx_stack_c::get(), mDoMtx_stack_c::get());
    lk_setAnmMtx(mpCLModel, 37);
    lk_transQuatM(trans_info, quaternion, 38);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), gabi::load<s16>(b + 0x73FA) /* mFootData[0].field_0x00A */);
    lk_setAnmMtx(mpCLModel, 38 /* CL_JNT_RLEGB_JNT_e */);
    lk_transQuatM(trans_info, quaternion, 39);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), gabi::load<s16>(b + 0x73F2) /* mFootData[0].field_0x002 */);
    lk_setAnmMtx(mpCLModel, 39 /* CL_JNT_RFOOT_JNT_e */);
    lk_transQuatM(trans_info, quaternion, 40);
    lk_setAnmMtx(mpCLModel, 40 /* CL_JNT_RTOE_JNT_e */);
    return TRUE;
}
VERIFY(0x02403150, &daPy_lk_c::jointCB1);

/* static JGeometry::TVec3<f32> v(x, y, z) initialised on first use (guard word) */
static inline void lk_staticVec(u32 guard, u32 v, f32 x, f32 y, f32 z) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        gabi::store<f32>(v + 0, x);
        gabi::store<f32>(v + 8, z);
        gabi::store<f32>(v + 4, y);
    }
}
/* JPABaseEmitter::setGlobalScale (HD: also the particle scale at +0x238) */
static inline void lk_emitterSetGlobalScale(u32 e, u32 v) {
    f32 x = gabi::load<f32>(v + 0);
    gabi::store<f32>(e + 0x220, x);
    f32 y = gabi::load<f32>(v + 4);
    gabi::store<f32>(e + 0x224, y);
    f32 z = gabi::load<f32>(v + 8);
    gabi::store<f32>(e + 0x238, x);
    gabi::store<f32>(e + 0x228, z);
    gabi::store<f32>(e + 0x23C, y);
    gabi::store<f32>(e + 0x240, z);
}
static inline void lk_storeVec(u32 dst, u32 v) {
    gabi::store<f32>(dst + 0, gabi::load<f32>(v + 0));
    gabi::store<f32>(dst + 4, gabi::load<f32>(v + 4));
    gabi::store<f32>(dst + 8, gabi::load<f32>(v + 8));
}

/* 02405CB4 */
void daPy_lk_c::setFootEffectType(int effectID, cXyz* i_pos, int param_2, int param_3) {
    WWHD_FUNC(0x02405CB4, void, this, effectID, i_pos, param_2, param_3);
    /* function-local statics (HD: initialised on first use) */
    u32 run_splash_scale = 0x1046D130;
    lk_staticVec(0x1046D11C, run_splash_scale, 0.6f, 0.6f, 0.6f);
    u32 run_grass_scale = 0x1046D13C;
    lk_staticVec(0x1046D120, run_grass_scale, 0.65f, 0.65f, 0.65f);
    u32 heavy_emit_smoke_scale = 0x1046D148;
    if (gabi::load<u32>(0x1046D124) == 0) {
        gabi::store<f32>(heavy_emit_smoke_scale + 8, 1.0f);
        gabi::store<f32>(heavy_emit_smoke_scale + 0, 1.0f);
        gabi::store<f32>(heavy_emit_smoke_scale + 4, 0.0f);
        gabi::store<u32>(0x1046D124, 1);
    }
    u32 heavy_dyn_smoke_scale = 0x1046D154;
    lk_staticVec(0x1046D128, heavy_dyn_smoke_scale, 0.25f, 0.25f, 0.25f);
    u32 heavy_pat_smoke_scale = 0x1046D160;
    lk_staticVec(0x1046D12C, heavy_pat_smoke_scale, 0.75f, 0.75f, 0.75f);
    u32 grass_scale = 0x101CEE94; /* static Vec {1.5f, 1.5f, 1.5f} */
    u32 smoke_scale = 0x101CEEA0; /* static Vec {1.25f, 1.25f, 1.25f} */

    JPABaseEmitter* emitter = NULL;
    u32 fe = gabi::ea(this) + 0x65FC + param_2 * 0x4C; /* &mFootEffect[param_2] */
    s16 angleX = 0;
    if ((mAcch.m_flags & 0x20 /* GROUND_HIT */) && !(mNoResetFlg0 & 0xA0000000)) {
        angleX = gabi::call<s16>(0x023E7D6C /* getGroundAngle */, this, gabi::ea(this) + 0x8F4 /* &mAcch.m_gnd */, (s32)current.angle.y);
    }
    u32 pp = gabi::ea(i_pos);
    gabi::store<u32>(fe + 0x34, gabi::load<u32>(pp + 0)); /* footEffect->setPos(i_pos) */
    gabi::store<u32>(fe + 0x38, gabi::load<u32>(pp + 4));
    gabi::store<s16>(fe + 0x42, current.angle.y); /* footEffect->setAngle(&angle) */
    gabi::store<u32>(fe + 0x3C, gabi::load<u32>(pp + 8));
    gabi::store<s16>(fe + 0x44, 0);
    gabi::store<s16>(fe + 0x40, angleX);
    if (effectID == 0x23 /* dPa_name::ID_AK_JN_ELEMENTSHIBUKI00 */) {
        if (mNoResetFlg0 & 0x80 /* daPyFlg0_UNK80 */) {
            f32 y = i_pos->y;
            f32 w = mWaterY;
            if (w > y + 10.0f) {
                gabi::store<f32>(fe + 0x38, w - 10.0f);
            }
        }
    } else if (mCurProc == 0x56 /* daPyProc_CUT_ROLL_e */ && effectID == 0x2022 /* ID_AK_JT_ELEMENTSMOKE00 */) {
        effectID = -2;
    } else if (param_3 == 5) {
        effectID = 0x2027; /* ID_AK_JT_ELEMENTSMOKE01 */
    }
    s32 oldEffectID = gabi::load<s32>(fe + 0x48);
    if (effectID == oldEffectID) {
        return;
    }
    if (oldEffectID != -1) {
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(fe) + 0x44), fe);               /* getSmokeCallBack()->remove() */
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(fe + 0x20) + 0x44), fe + 0x20); /* getOtherCallBack()->remove() */
    }
    cXyz* pos = gabi::at<cXyz>(fe + 0x34);
    csXyz* angle = gabi::at<csXyz>(fe + 0x40);
    if (effectID == -2) {
        s8 room = current.roomNo;
        emitter = dPa_control_set(dComIfGp_getParticle(), 1, 0x2022, pos, angle, gabi::at<cXyz>(smoke_scale), 0xA0,
                                  gabi::at<dPa_levelEcallBack>(fe), room, nullptr, nullptr, nullptr);
        if (emitter != NULL) {
            gabi::store<s32>(fe + 0x48, effectID);
        }
    } else if (effectID == 0x2022 || effectID == 0x2027) {
        s8 room = current.roomNo; /* dComIfGp_particle_setToonP1 */
        emitter = dPa_control_set(dComIfGp_getParticle(), 3, (u16)effectID, pos, angle, gabi::at<cXyz>(smoke_scale), 0xA0,
                                  gabi::at<dPa_levelEcallBack>(fe), room, nullptr, nullptr, nullptr);
        if (emitter != NULL) {
            gabi::store<s32>(fe + 0x48, effectID);
        }
    } else if (effectID == 0x24 /* ID_AK_JN_ELEMENTKUSA00 */) {
        gabi::Local<GXColor> color; /* tevStr.mColorC0 */
        u32 b = gabi::ea(this);
        color->g = (u8)gabi::load<s16>(b + 0x1A2);
        color->b = (u8)gabi::load<s16>(b + 0x1A4);
        color->r = (u8)gabi::load<s16>(b + 0x1A0);
        color->a = (u8)gabi::load<s16>(b + 0x1A6);
        emitter = dPa_control_set(dComIfGp_getParticle(), 1, 0x24, pos, angle, gabi::at<cXyz>(grass_scale), 0xFF,
                                  gabi::at<dPa_levelEcallBack>(fe + 0x20), -1, color, gabi::at<GXColor>(b + 0x1A8) /* mColorK0 */, nullptr);
        if (emitter == NULL) {
            return;
        }
        /* HD: the emitter's material parameter block, colour from the particle manager (+0x68) */
        gabi::Local<lk_emitterParam_l> prm;
        prm->m08[1] = 1.0f;
        prm->mName = 0x10000160;
        prm->m08[2] = 1.0f;
        prm->__vtbl = LK_SAFESTRING_VTBL;
        prm->m08[3] = 1.0f;
        prm->m08[0] = 1.0f;
        prm->m18[3] = 0;
        prm->m18[0] = 1;
        prm->m18[2] = 0;
        prm->m18[1] = 0;
        u32 pa = gabi::ea(dComIfGp_getParticle());
        u32 pr = gabi::ea(prm.get());
        for (u32 i = 0; i < 4; i++) {
            gabi::store<u32>(pr + 8 + i * 4, gabi::load<u32>(pa + 0x68 + i * 4));
        }
        gabi::call(0x0281E5A8, emitter, prm.get());
        gabi::store<s32>(fe + 0x48, effectID);
    } else {
        u32 cam = gabi::load<u32>(dComIfGp_ea() + 0x5AF8);
        u32 mode = gabi::load<u32>(cam + 0x384);
        if (mode == 4) {
            return;
        }
        if (mode >= 0xA && (mode <= 0xB || mode == 0xE)) {
            return;
        }
        gabi::Local<GXColor> amb;
        gabi::Local<GXColor> dif;
        gabi::call(0x025602F0 /* dKy_get_seacolor */, amb.get(), dif.get());
        emitter = dPa_control_set(dComIfGp_getParticle(), 1, (u16)effectID, pos, angle, nullptr, 0xFF,
                                  gabi::at<dPa_levelEcallBack>(fe + 0x20), -1, amb, nullptr, nullptr);
        if (emitter != NULL) {
            gabi::store<s32>(fe + 0x48, effectID);
        }
    }
    if (effectID == -2) {
        effectID = 0x2022;
    }
    if (emitter == NULL) {
        return;
    }
    u32 e = gabi::ea(emitter);
    if (param_3 == 5) {
        if (effectID == 0x2027) {
            gabi::store<f32>(e + 0x34, 10.0f); /* setRate */
            gabi::store<u16>(e + 0x60, 0x28);  /* setLifeTime(40) */
            lk_storeVec(e + 0x08, heavy_emit_smoke_scale); /* setEmitterScale */
            lk_storeVec(e + 0x220, heavy_dyn_smoke_scale); /* setGlobalDynamicsScale */
            lk_storeVec(e + 0x238, heavy_pat_smoke_scale); /* setGlobalParticleScale */
        } else {
            gabi::store<f32>(e + 0x58, 1.0f); /* setSpread */
            gabi::store<f32>(e + 0x34, 15.0f);
            if (effectID == 0x24) {
                gabi::store<f32>(e + 0x70, 12.0f); /* setDirectionalSpeed */
            }
        }
    } else if (param_3 == 1) {
        gabi::store<f32>(e + 0x58, 1.0f);
        gabi::store<f32>(e + 0x34, 16.0f);
    } else if (param_3 == 3) {
        gabi::store<f32>(e + 0x34, 8.0f);
        gabi::store<f32>(e + 0x58, 0.3f);
    } else if (param_3 == 2) {
        if (effectID == 0x24) {
            lk_emitterSetGlobalScale(e, run_grass_scale);
            gabi::store<u16>(e + 0x60, 0xF);
            gabi::store<f32>(e + 0x58, 1.0f);
            gabi::store<f32>(e + 0x34, 10.0f);
        } else {
            gabi::store<f32>(e + 0x58, 1.0f);
            gabi::store<f32>(e + 0x34, 18.0f);
            lk_emitterSetGlobalScale(e, run_splash_scale);
        }
    } else if (param_3 == 4 && effectID == 0x23) {
        gabi::store<f32>(e + 0x34, 3.0f);
        gabi::store<f32>(e + 0x58, 0.2f);
    } else {
        gabi::store<f32>(e + 0x34, 3.0f);
        gabi::store<f32>(e + 0x58, 0.05f);
    }
}
VERIFY(0x02405CB4, &daPy_lk_c::setFootEffectType);

/* mDoMtx_multVecZero(mpCLModel->getAnmMtx(jnt), pos) (FPR copies) */
static inline void lk_jntPos(J3DModel* m, s32 jnt, cXyz* pos) {
    Mtx34* mtx = lk_getAnmMtx(m, jnt);
    pos->x = mtx->m[0][3];
    pos->y = mtx->m[1][3];
    pos->z = mtx->m[2][3];
}
/* HD: the camera mode (camera 0, +0x384) of the views without water splashes (4, 10, 11, 14) */
static inline bool lk_checkNoSplashCamera() {
    u32 mode = gabi::load<u32>(gabi::load<u32>(dComIfGp_ea() + 0x5AF8) + 0x384);
    return mode == 4 || (mode >= 0xA && (mode <= 0xB || mode == 0xE));
}

/* 02406480 */
void daPy_lk_c::setFootEffect() {
    WWHD_FUNC(0x02406480, void, this);
    u32 b = gabi::ea(this);
    if (!(mAcch.m_flags & 0x20) /* !mAcch.ChkGroundHit() */ || (mNoResetFlg0 & 0x80000000)) {
        gabi::call(0x023DF9C0 /* resetFootEffect */, this);
        gabi::call(LK_followEcallBack_end, b + 0x6824); /* m3410.end() */
        return;
    }
    gabi::Local<cXyz> pos;
    if (resetFlg0() & 0x400 /* getRightFootOnGround() */) {
        lk_jntPos(mpCLModel, 39 /* CL_JNT_RFOOT_JNT_e */, pos);
        setFootMark(pos);
    }
    if (resetFlg0() & 0x800 /* getLeftFootOnGround() */) {
        lk_jntPos(mpCLModel, 34 /* CL_JNT_LFOOT_JNT_e */, pos);
        setFootMark(pos);
    }
    if ((resetFlg0() & 0x10 /* daPyRFlg0_UNK10 */) && mCurrAttributeCode == 0x13 /* dBgS_Attr_WATER_e */ &&
        mWaterY > current.pos.y + 25.0f) {
        u32 p = b + 0x682C; /* m3410.getPos() */
        u16 a = (u16)current.angle.y;
        f32 s = cM_ssin(a);
        gabi::store<f32>(p + 4, mWaterY);
        gabi::store<f32>(p + 0, gabi::fmadds(45.0f, s, current.pos.x));
        gabi::store<s16>(b + 0x683A, (s16)(a + 0x8000)); /* m3410.setAngle(0, current.angle.y + 0x8000, 0) */
        gabi::store<s16>(b + 0x683C, 0);
        gabi::store<s16>(b + 0x6838, 0);
        gabi::store<f32>(p + 8, gabi::fmadds(45.0f, cM_scos(a), current.pos.z));
        bool noSplash = lk_checkNoSplashCamera();
        if (gabi::load<u32>(b + 0x6828) == 0) { /* m3410.getEmitter() */
            if (!noSplash) {
                gabi::Local<GXColor> amb;
                gabi::Local<GXColor> dif;
                gabi::call(0x025602F0 /* dKy_get_seacolor */, amb.get(), dif.get());
                JPABaseEmitter* emitter = dPa_control_set(dComIfGp_getParticle(), 1, 0x23 /* ID_AK_JN_ELEMENTSHIBUKI00 */, gabi::at<cXyz>(p),
                                                          nullptr, nullptr, 0xFF, gabi::at<dPa_levelEcallBack>(b + 0x6824), -1, amb,
                                                          nullptr, nullptr);
                if (emitter != NULL) {
                    u32 e = gabi::ea(emitter);
                    gabi::store<f32>(e + 0x70, 11.0f); /* setDirectionalSpeed */
                    gabi::store<f32>(e + 0x58, 0.6f);  /* setSpread */
                    gabi::store<f32>(e + 0x34, 4.0f);  /* setRate */
                    gabi::store<u16>(e + 0x60, 15);    /* setLifeTime */
                }
                if (gabi::load<u32>(b + 0x6828) != 0) {
                    goto seacolor;
                }
            }
        } else {
        seacolor:
            /* HD: the splash follows the sea colour */
            gabi::Local<GXColor> amb2;
            gabi::Local<GXColor> dif2;
            gabi::call(0x025602F0 /* dKy_get_seacolor */, amb2.get(), dif2.get());
            u32 e = gabi::load<u32>(b + 0x6828);
            gabi::store<u8>(e + 0x244, amb2->r);
            gabi::store<u8>(e + 0x246, amb2->b);
            gabi::store<u8>(e + 0x245, amb2->g);
        }
    } else {
        gabi::call(LK_followEcallBack_end, b + 0x6824); /* m3410.end() */
    }
    {
        /* strcmp(dComIfGp_getStartStageName(), "Adanmae") == 0 */
        u32 sp = dComIfGp_ea() + 0x5134;
        u32 lp = 0x10035A3C;
        u8 c0, c1;
        do {
            c0 = gabi::load<u8>(sp++);
            c1 = gabi::load<u8>(lp++);
        } while (c0 == c1 && c0 != 0);
        if (c0 == c1) {
            u32 rf = resetFlg0();
            if ((rf & 0x10) && (rf & 0xC00) /* getFootOnGround() */ && !(mWaterY > current.pos.y)) {
                /* dComIfGp_particle_setP1(dPa_name::ID_AK_SN_VOLCANICASHESRUN00, &pos, &current.angle) */
                dPa_control_set(dComIfGp_getParticle(), 1, 0x8237, pos, &current.angle, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
            }
        }
    }
    s32 attr = mCurrAttributeCode;
    s32 effectID = gabi::call<s32>(0x025A8770 /* dPa_control_c::checkAtrCodeEffect */, dComIfGp_getParticle(), attr);
    if (effectID == -1) {
        return;
    }
    s32 var_r29;
    u32 rf = resetFlg0();
    if ((rf & 0x10) && (rf & 0xC00) &&
        (gabi::call<BOOL>(LK_checkHeavyStateOn, this) || effectID == 0x23 || effectID == 0x24)) {
        mFootEffectPosType = (resetFlg0() & 0x400) ? 2 : 1;
        var_r29 = 2;
        if (gabi::call<BOOL>(LK_checkHeavyStateOn, this)) {
            var_r29 = 5;
            gabi::Local<cXyz> v;
            v->x = 0.0f;
            v->y = 1.0f;
            v->z = 0.0f;
            dComIfGp_getVibration_StartShock(2, -0x31, v);
        }
    } else if (mCurProc == 0x5C /* daPyProc_JUMP_CUT_LAND_e */) {
        var_r29 = 3;
    } else if (mCurProc == 0x1E /* daPyProc_FRONT_ROLL_e */) {
        var_r29 = 4;
    } else {
        var_r29 = 0;
    }
    u8 posType = mFootEffectPosType;
    if (posType == 0) {
        return;
    }
    if (mCurProc == 0xD8 /* daPyProc_DEMO_LAST_COMBO_e */) {
        f32 rnd = cM_rndFX(17.5f);
        gabi::Local<cXyz> sp44;
        sp44->x = m370C.x;
        sp44->y = m370C.y;
        sp44->z = m370C.z;
        lk_jntPos(mpCLModel, 1 /* CL_JNT_CENTER_e */, &m370C);
        s16 a;
        if (mProcVar6 != 0) {
            a = (s16)(shape_angle.y - 0x4000);
            current.angle.y = a;
            mProcVar6 = 0;
        } else {
            gabi::Local<cXyz> sp38;
            cXyz_mi(&m370C, sp38, sp44);
            a = cM_atan2s(sp38->x, sp38->z);
            current.angle.y = a;
        }
        f32 x = m370C.x;
        f32 c = cM_scos((u16)a);
        pos->y = current.pos.y;
        pos->x = gabi::fmadds(rnd, c, x);
        pos->z = gabi::fnmsubs(rnd, cM_scos((u16)a), m370C.z); /* (the GameCube source uses the cosine here too) */
        setFootEffectType(effectID, pos, 0, var_r29);
    } else if (posType == 4) {
        f32 rnd = cM_rndFX(17.5f);
        u16 a = (u16)current.angle.y;
        f32 x = gabi::fmadds(rnd, cM_scos(a), current.pos.x);
        pos->x = x;
        pos->y = current.pos.y;
        pos->z = gabi::fnmsubs(rnd, cM_scos(a), current.pos.z);
        setFootEffectType(effectID, pos, 0, var_r29);
    } else if (posType == 3) {
        if (mCurrAttributeCode == 4 /* dBgS_Attr_GRASS_e */ || mCurrAttributeCode == 0x13 /* dBgS_Attr_WATER_e */) {
            lk_jntPos(mpCLModel, 39 /* CL_JNT_RFOOT_JNT_e */, pos);
            setFootEffectType(effectID, pos, 0, var_r29);
            lk_jntPos(mpCLModel, 34 /* CL_JNT_LFOOT_JNT_e */, pos);
            setFootEffectType(effectID, pos, 1, var_r29);
        } else {
            setFootEffectType(effectID, &current.pos, 0, var_r29);
        }
    } else if (posType == 1) {
        lk_jntPos(mpCLModel, 34 /* CL_JNT_LFOOT_JNT_e */, pos);
        setFootEffectType(effectID, pos, 0, var_r29);
    } else if (posType == 2) {
        lk_jntPos(mpCLModel, 39 /* CL_JNT_RFOOT_JNT_e */, pos);
        setFootEffectType(effectID, pos, 0, var_r29);
    } else if (posType == 5) {
        setFootEffectType(effectID, &current.pos, 0, 1);
    } else if (posType == 6) {
        Mtx34* mtx = lk_getAnmMtx(mpCLModel, 0 /* CL_JNT_LINK_ROOT_e */);
        f32 x = mtx->m[0][3];
        f32 y = mtx->m[1][3];
        f32 z = mtx->m[2][3];
        pos->x = x;
        pos->y = y;
        mGndChkPos.x = x; /* mGndChk.SetPos(&pos) */
        pos->z = z;
        mGndChkPos.y = y;
        mGndChkPos.z = z;
        f64 groundY = gabi::call<f64>(0x02008974 /* cBgS::GroundCross */, dComIfG_Bgsp(), mGndChk);
        if (groundY != -1000000000.0) {
            pos->y = (f32)groundY;
        }
        setFootEffectType(effectID, pos, 0, 1);
    }
}
VERIFY(0x02406480, &daPy_lk_c::setFootEffect);

/* 0240A41C */
void daPy_lk_c::setGrabItemPos() {
    WWHD_FUNC(0x0240A41C, void, this);
    fopAc_ac_c* grab_actor = mActorKeepGrab.mActor;
    if (grab_actor == NULL) {
        return;
    }
    u32 g = gabi::ea(grab_actor);
    if (mCurProc == 0x6E /* daPyProc_GRAB_READY_e */) {
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_stack_c::YrotM((s16)(shape_angle.y - mProcVar2));
        PSMTXMultVec(mDoMtx_stack_c::get(), &m370C, &grab_actor->current.pos);
        return;
    }
    s8 sVar2 = current.roomNo;
    gabi::store<s8>(g + 0x1C9, sVar2); /* grab_actor->tevStr.mRoomNo */
    u8 envr = gabi::load<u8>(gabi::ea(this) + 0x1CA); /* tevStr.mEnvrIdxOverride */
    grab_actor->current.roomNo = sVar2;
    gabi::store<u8>(g + 0x1CA, envr);
    if (!(mNoResetFlg0 & 0x400) && gabi::call<BOOL>(0x023DCF08 /* checkGrabBarrelSearch */, this, 0) && !(mItemButton & 1) /* !doButton() */) {
        mNoResetFlg0 = mNoResetFlg0 | 0x400;
        for (int i = 0; i < 3; i++) {
            gabi::call(0x024EFF3C /* dBgS_AcchCir::SetWallR */, &mAcchCir[i], 50.0f);
        }
    }
    be<f32>& field_0x2b0 = LK_FIELD(f32, 0x3CC);
    gabi::Local<cXyz> cStack_58;
    if (mCurProc == 0x6F /* daPyProc_GRAB_UP_e */) {
        cLib_chaseAngleS(&grab_actor->shape_angle.y, shape_angle.y, 0x1000);
    } else {
        if (mCurProc != 0xC5 /* daPyProc_DEMO_STAND_ITEM_PUT_e */) {
            grab_actor->shape_angle.y = shape_angle.y;
        }
        grab_actor->shape_angle.z = m351C;
        if (mNoResetFlg0 & 0x400) {
            f32 dVar10;
            if (mCurProc == 0x72 /* daPyProc_GRAB_PUT_e */) {
                dVar10 = 0.0f;
            } else if (mCurProc == 0x73 /* daPyProc_GRAB_WAIT_e */ || mCurProc == 0x17 /* daPyProc_WAIT_TURN_e */) {
                dVar10 = -115.0f;
            } else {
                dVar10 = -85.0f;
            }
            field_0x2b0 = field_0x2b0 + m35D8;
            gabi::call<f32>(0x0200ECD4 /* cLib_addCalc */, &field_0x2b0, dVar10, 0.5f, 20.0f, 1.0f);
            f32 v = field_0x2b0;
            f32 spd = speedF;
            if (v < -85.0f) {
                field_0x2b0 = -85.0f;
                m35D8 = v - -85.0f;
            } else {
                m35D8 = 0.0f;
            }
            s16 target_slant_angle;
            if (spd > 0.0f && m34C3 != 0) {
                f32 rad_angle = (6.2831855f * mFrameCtrlUnder[1].getFrame()) / (f32)mFrameCtrlUnder[1].getEnd();
                s16 a = gabi::call<s16>(0x02019510 /* cM_rad2s */, rad_angle);
                target_slant_angle = (s16)gabi::ftoi(2048.0f * cM_ssin((u16)a));
            } else {
                target_slant_angle = 0;
            }
            gabi::Local<be<s16>> curr_slant_angle; /* barrel->get_slant_angle() (+0x738) */
            *curr_slant_angle = gabi::load<s16>(g + 0x738);
            gabi::call<s16>(0x0200F378 /* cLib_addCalcAngleS */, curr_slant_angle.get(), (s32)target_slant_angle, 5, 0x200, 0x80);
            gabi::store<s16>(g + 0x738, *curr_slant_angle);
        }
    }
    if (mCurProc == 0x6F) {
        cStack_58->copy(grab_actor->current.pos);
    }
    gabi::Local<cXyz> sum;
    cXyz_pl(gabi::at<cXyz>(gabi::ea(this) + 0x3F0) /* mLeftHandPos */, sum, gabi::at<cXyz>(gabi::ea(this) + 0x3FC) /* mRightHandPos */);
    gabi::Local<cXyz> half;
    cXyz_ml(sum, half, 0.5f);
    grab_actor->current.pos.x = half->x;
    f32 hy = half->y;
    grab_actor->current.pos.y = hy;
    grab_actor->current.pos.z = half->z;
    grab_actor->current.pos.y = hy + field_0x2b0;
    if (gabi::load<u16>(gabi::ea(this) + 0x5848) == 0x94 /* m_anm_heap_under[0].mIdx == dRes_INDEX_LKANM_BCK_GRABUP_e */) {
        s16 st = mFrameCtrlUnder[0].getStart();
        s16 en = mFrameCtrlUnder[0].getEnd();
        f32 dVar10 = 1.0f - ((mFrameCtrlUnder[0].getFrame() - (f32)st) / (f32)(s32)(en - st));
        f32 fVar1 = m35C8 * dVar10;
        grab_actor->current.pos.x = gabi::fmadds(fVar1, cM_ssin((u16)shape_angle.y), grab_actor->current.pos.x);
        grab_actor->current.pos.z = gabi::fmadds(fVar1, cM_scos((u16)shape_angle.y), grab_actor->current.pos.z);
        if (mCurProc == 0x72 /* daPyProc_GRAB_PUT_e */) {
            grab_actor->current.pos.y = gabi::fmadds(-18.07f, dVar10, grab_actor->current.pos.y);
        } else {
            grab_actor->current.pos.y = gabi::fmadds(-16.41f, dVar10, grab_actor->current.pos.y);
        }
        if (mCurProc == 0x72) {
            f32 x = grab_actor->current.pos.x;
            f32 y = current.pos.y + 125.0f;
            f32 z = grab_actor->current.pos.z;
            mGndChkPos.x = x; /* mGndChk.SetPos(&local_64) */
            mGndChkPos.y = y;
            mGndChkPos.z = z;
            f64 dVar9 = gabi::call<f64>(0x02008974 /* cBgS::GroundCross */, dComIfG_Bgsp(), mGndChk);
            if (dVar9 > (f64)(f32)grab_actor->current.pos.y) {
                grab_actor->speedF = 0.0f;
                grab_actor->current.pos.y = (f32)dVar9;
                gabi::call(0x023DCF8C /* freeGrabItem */, this);
                return;
            }
        }
        if (fpcM_GetName(grab_actor) == 0xD8 /* fpcNm_MT_e */) {
            grab_actor->current.pos.y = gabi::fmadds(40.0f /* magtail_offset */, 1.0f - dVar10, grab_actor->current.pos.y);
        }
    } else if (fpcM_GetName(grab_actor) == 0xD8 /* fpcNm_MT_e */) {
        f32 fVar1 = -40.0f * cM_ssin((u16)m351C);
        grab_actor->current.pos.x = gabi::fmadds(fVar1, cM_scos((u16)shape_angle.y), grab_actor->current.pos.x);
        grab_actor->current.pos.y = gabi::fmadds(40.0f, cM_scos((u16)m351C), grab_actor->current.pos.y);
        grab_actor->current.pos.z = gabi::fnmsubs(fVar1, cM_ssin((u16)shape_angle.y), grab_actor->current.pos.z);
    }
    if (mCurProc == 0x6F && grab_actor->current.pos.y < cStack_58->y) {
        grab_actor->current.pos.y = cStack_58->y;
        grab_actor->current.pos.x = cStack_58->x;
        grab_actor->current.pos.z = cStack_58->z;
    }
    if (mNoResetFlg0 & 0x400) {
        m35CC = 0.0f;
        grab_actor->current.pos.y = grab_actor->current.pos.y + 0.0f;
    } else if ((mModeFlg & 2 /* ModeFlg_MIDAIR */) && speed.y < 0.0f) {
        f32 v = gabi::fnmsubs(speed.y, 0.2f, m35CC);
        if (v > 40.0f) {
            m35CC = 40.0f;
            grab_actor->current.pos.y = grab_actor->current.pos.y + 40.0f;
        } else {
            m35CC = v;
            grab_actor->current.pos.y = grab_actor->current.pos.y + v;
        }
    } else {
        cLib_chaseF(&m35CC, 0.0f, 5.0f);
        f32 v = m35CC;
        grab_actor->current.pos.y = grab_actor->current.pos.y + v;
    }
}
VERIFY(0x0240A41C, &daPy_lk_c::setGrabItemPos);

/* HD: the animation objects with a frame callback (value = fn(ctx, frame, a, b), +0x10 fn, +0x14 ctx,
 * +4 a, +8 b, the value at +0) behind a pointer at this + field; then 027DF40C(this + field) */
static inline void lk_hdAnmFrame(u32 self, u32 field, f32 frame) {
    u32 o = gabi::load<u32>(self + field);
    f32 r = gabi::call_ptr<f32>(gabi::load<u32>(o + 0x10), gabi::load<u32>(o + 0x14), frame, gabi::load<f32>(o + 4), gabi::load<f32>(o + 8));
    gabi::store<f32>(o, r);
    gabi::call(0x027DF40C, self + field);
}
/* the selected sword (save + 0x2E) */
static inline u8 lk_selectSword() { return gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x2E); }
/* daPy_lk_c::checkMasterSwordEquip(): 0x39, 0x3A or 0x3E */
static inline bool lk_checkMasterSwordEquip() {
    u8 s = lk_selectSword();
    return s == 0x39 || s == 0x3A || s == 0x3E;
}

/* 0240C32C */
void daPy_lk_c::setLightSaver() {
    WWHD_FUNC(0x0240C32C, void, this);
    u32 b = gabi::ea(this);
    u32 d_scale = 0x1046D1CC; /* static JGeometry::TVec3<f32> d_scale(1.4f, 1.4f, 1.4f) */
    if (gabi::load<u32>(0x1046D1C4) == 0) {
        gabi::store<f32>(d_scale + 0, 1.4f);
        gabi::store<f32>(d_scale + 8, 1.4f);
        gabi::store<u32>(0x1046D1C4, 1);
        gabi::store<f32>(d_scale + 4, 1.4f);
    }
    u32 p_scale = 0x1046D1D8; /* static JGeometry::TVec3<f32> p_scale(1.6f, 1.4f, 1.4f) */
    if (gabi::load<u32>(0x1046D1C8) == 0) {
        gabi::store<f32>(p_scale + 4, 1.4f);
        gabi::store<f32>(p_scale + 8, 1.4f);
        gabi::store<f32>(p_scale + 0, 1.6f);
        gabi::store<u32>(0x1046D1C8, 1);
    }
    if (mEquipItem != 0x103 /* daPyItem_SWORD_e */ || LK_FIELD(u32, 0x4908) == 0 /* mpEquipItemBrk */ ||
        LK_FIELD(u32, 0x48EC) == 0 /* mpSwordBtk */ || LK_FIELD(u32, 0x4978) == 0 /* mpSwordModel1 */ ||
        LK_FIELD(u32, 0x4440) == 0 /* mpEquipItemModel */) {
        return;
    }
    /* mpEquipItemBrk->setFrame(v) (HD: the brk object at 0x48F8 and its frame callback object) */
    f32 v;
    if (gabi::call<BOOL>(0x023D937C /* checkChanceMode */, this)) {
        v = 1.0f;
    } else if (noResetFlg1() & 0x8000 /* daPyFlg1_SOUP_POWER_UP */) {
        v = 0.0f;
    } else {
        v = 2.0f;
    }
    LK_FIELD(f32, 0x48FC) = v;
    gabi::store<f32>(LK_FIELD(u32, 0x4908), v);
    lk_hdAnmFrame(b, 0x4918, v);
    {
        /* mpSwordModel1->setBaseTRMtx(mpEquipItemModel->getBaseTRMtx()) */
        u32 m = LK_FIELD(u32, 0x4440);
        u32 dst = LK_FIELD(u32, 0x4978) + 0xC8;
        u32 src = m ? m + 0xC8 : 0;
        u32 t[12];
        for (u32 i = 0; i < 12; i++) {
            t[i] = lk_fbits(gabi::load<f32>(src + i * 4));
        }
        for (u32 i = 0; i < 12; i++) {
            gmem_stf32(dst + i * 4, t[i]);
        }
    }
    if (!gabi::call<BOOL>(0x023D8F6C /* checkDemoSwordNoDraw */, this, 0) &&
        (gabi::call<BOOL>(0x023D937C, this) || (noResetFlg1() & 0x8000) || lk_selectSword() == 0x3E /* checkFinalMasterSwordEquip() */)) {
        /* simpleAnmPlay(mpSwordBtk) (HD: the btk object at 0x4884 and its frame callback object) */
        gabi::call(0x025E742C /* mDoExt_baseAnm::play */, b + 0x4884);
        f32 fr = LK_FIELD(f32, 0x4888);
        gabi::store<f32>(LK_FIELD(u32, 0x48EC), fr);
        lk_hdAnmFrame(b, 0x4894, fr);
        JPABaseEmitter* e;
        if (LK_FIELD(u32, 0x686C) == 0 /* m3454.getEmitter() */ && (gabi::call<BOOL>(0x023D937C, this) || (noResetFlg1() & 0x8000))) {
            Mtx34* mtx = lk_getAnmMtx(gabi::at<J3DModel>(LK_FIELD(u32, 0x4978)), 2 /* YMSLI00_JNT_SWMSA_JNT_e */);
            e = gabi::call<JPABaseEmitter*>(0x023D457C /* daPy_mtxFollowEcallBack_c::makeEmitter */, b + 0x6868, 0x309 /* ID_AK_JN_LIGHTSAVER00 */,
                                            mtx, &current.pos, 0);
            if (e != NULL && lk_checkMasterSwordEquip()) {
                lk_storeVec(gabi::ea(e) + 0x220, d_scale); /* setGlobalDynamicsScale */
                lk_storeVec(gabi::ea(e) + 0x238, p_scale); /* setGlobalParticleScale */
            }
        } else if (!gabi::call<BOOL>(0x023D937C, this) && !(noResetFlg1() & 0x8000) && lk_selectSword() == 0x3E) {
            gabi::call(LK_mtxFollowEcallBack_end, b + 0x6868); /* m3454.end() */
        }
        BOOL chance = gabi::call<BOOL>(0x023D937C, this);
        u32 em = LK_FIELD(u32, 0x686C);
        u32 prm0;
        u32 env0;
        if (chance) {
            prm0 = 0x10035AB0; /* g_prm0 */
            env0 = 0x10035AB4; /* g_env0 */
        } else {
            prm0 = 0x10035AB8; /* y_prm0 */
            env0 = 0x10035ABC; /* y_env0 */
        }
        if (em != 0) {
            u8 g = gabi::load<u8>(prm0 + 1);
            u8 r = gabi::load<u8>(prm0 + 0);
            u8 bl = gabi::load<u8>(prm0 + 2);
            gabi::store<u8>(em + 0x244, r); /* setGlobalPrmColor */
            gabi::store<u8>(em + 0x245, g);
            gabi::store<u8>(em + 0x246, bl);
            u8 r2 = gabi::load<u8>(env0 + 0);
            u8 g2 = gabi::load<u8>(env0 + 1);
            u8 b2 = gabi::load<u8>(env0 + 2);
            gabi::store<u8>(em + 0x249, g2); /* setGlobalEnvColor */
            gabi::store<u8>(em + 0x248, r2);
            gabi::store<u8>(em + 0x24A, b2);
        }
        bool parry = gabi::load<u8>(dComIfGp_ea() + 0x5BB7) == 0x1A /* dActStts_PARRY_e */ && m355C == 0;
        bool go = parry;
        if (!go) {
            u32 f1 = noResetFlg1();
            go = ((f1 & 0x8000) || lk_selectSword() == 0x3E) && !(f1 & 0x200000 /* daPyFlg1_UNK200000 */);
        }
        if (go) {
            u32 prm;
            u32 env;
            if (gabi::load<u8>(dComIfGp_ea() + 0x5BB7) == 0x1A && m355C == 0) {
                prm = 0x10035AC0; /* g_prm1 */
                env = prm;
                gabi::Local<cXyz> v1;
                v1->x = 0.0f;
                v1->y = 1.0f;
                v1->z = 0.0f;
                dComIfGp_getVibration_StartShock(6, 1, v1);
            } else if (noResetFlg1() & 0x8000) {
                prm = 0x10035AC4; /* y_prm1 */
                env = 0x10035AC8; /* y_env1 */
            } else {
                prm = 0x10035ACC; /* s_prm1 */
                env = 0x10035AD0; /* s_env1 */
            }
            /* dComIfGp_particle_setP1(dPa_name::ID_AK_JN_LIGHTSAVER01, &current.pos, .., pbVar9, pbVar8) */
            e = dPa_control_set(dComIfGp_getParticle(), 1, 0x30A, &current.pos, nullptr, nullptr, 0xFF, nullptr, -1,
                                gabi::at<GXColor>(prm), gabi::at<GXColor>(env), nullptr);
            if (e != NULL) {
                u32 ee = gabi::ea(e);
                Mtx34* mtx = lk_getAnmMtx(gabi::at<J3DModel>(LK_FIELD(u32, 0x4440)), 2 /* SWMS_JNT_SWMSA_JNT_e */);
                gabi::call(0x028249B0 /* JPASetRMtxTVecfromMtx (setGlobalRTMatrix) */, mtx, ee + 0x1F0, ee + 0x22C);
                if (lk_checkMasterSwordEquip()) {
                    lk_storeVec(ee + 0x220, d_scale);
                    lk_storeVec(ee + 0x238, p_scale);
                }
                if (prm == env) { /* pbVar9 == &g_prm1 */
                    m355C = 0x1E;
                    if (!gabi::call<BOOL>(0x023D937C, this)) {
                        m355C = m355C - 1;
                    }
                    return;
                }
                setNoResetFlg1(noResetFlg1() | 0x200000);
            }
        }
    } else {
        gabi::call(LK_mtxFollowEcallBack_end, b + 0x6868); /* m3454.end() */
        LK_FIELD(f32, 0x4888) = 0.0f;                     /* mpSwordBtk->setFrame(0.0f) */
        gabi::store<f32>(LK_FIELD(u32, 0x48EC), 0.0f);
        lk_hdAnmFrame(b, 0x4894, 0.0f);
    }
    if (m355C > 0 && !gabi::call<BOOL>(0x023D937C, this)) {
        m355C = m355C - 1;
    }
}
VERIFY(0x0240C32C, &daPy_lk_c::setLightSaver);

/* J3DAnmBase::getFrameMax (HD: virtual, vtable at +4, slot 0x14) */
static inline s32 anm_getFrameMax(u32 anm) { return gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(anm + 4) + 0x14), anm); }
/* model->setBaseTRMtx(src) (twelve FPR loads, then the stores) */
static inline void lk_setBaseTRMtx(u32 model, u32 src) {
    u32 t[12];
    for (u32 i = 0; i < 12; i++) {
        t[i] = lk_fbits(gabi::load<f32>(src + i * 4));
    }
    for (u32 i = 0; i < 12; i++) {
        gmem_stf32(model + 0xC8 + i * 4, t[i]);
    }
}
/* mDoExt_brkAnm::init(modelData, brk, FALSE, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, true, 0) (HD 025E8154) */
static inline void lk_brkInit(u32 brkAnm, u32 modelData, u32 brk) {
    gabi::call(0x025E8154, brkAnm, modelData, brk, 0, 0, 1.0f, 0, -1, 1, 0);
}

/* 0240B7F4 */
void daPy_lk_c::setAuraEffect() {
    WWHD_FUNC(0x0240B7F4, void, this);
    u32 b = gabi::ea(this);
    f32 y00BtkFrameMax = (f32)LK_FIELD(s16, 0x5696); /* mpYaura00Btk->getFrameMax() */
    u32 curYmgcs00Brk = LK_FIELD(u32, 0x5714);       /* mYmgcs00Brk.getBrkAnm() */
    u32 curYaura00Brk = LK_FIELD(u32, 0x5624);       /* mYaura00rBrk.getBrkAnm() */
    u32 flg1 = noResetFlg1();
    f32 yauraFrame = gabi::load<f32>(curYaura00Brk);
    f32 ymgcsFrame = gabi::load<f32>(curYmgcs00Brk);
    if ((flg1 & 1) || mTinkleShieldTimer != 0) { /* checkNoDamageMode() */
        u32 pYaura00Brk;
        u32 pYmgcs00Brk;
        if (flg1 & 1) { /* checkEquipDragonShield() */
            /* HD: no magic consumption here; the shield ends when the magic is empty */
            if (gabi::load<u16>(gabi::load<u32>(0x101F84DC) + 0x24) == 0 /* dComIfGs_getMagic() */) {
                setNoResetFlg1(flg1 & ~1u);
            }
            pYaura00Brk = gabi::ea(dComIfG_getObjectRes(STR(0x101CEB48) /* l_arcName "Link" */, 0x58 /* YAURA00_R */, LK_SAFESTRING_VTBL));
            pYmgcs00Brk = gabi::ea(dComIfG_getObjectRes(STR(0x101CEB48), 0x59 /* YMGCS00_MS */, LK_SAFESTRING_VTBL));
        } else {
            if (gabi::load<u8>(dComIfGp_ea() + 0x5292) == 0 /* !dComIfGp_event_runCheck() */) {
                mTinkleShieldTimer = mTinkleShieldTimer - 1;
            }
            pYaura00Brk = gabi::ea(dComIfG_getObjectRes(STR(0x101CEB48), 0x57 /* YAURA00_G */, LK_SAFESTRING_VTBL));
            pYmgcs00Brk = gabi::ea(dComIfG_getObjectRes(STR(0x101CEB48), 0x5A /* YMGCS00_TS */, LK_SAFESTRING_VTBL));
        }
        if (pYaura00Brk != curYaura00Brk) {
            /* mYaura00rBrk.init(mMagicArmorAuraEntries[0].getModel()->getModelData(), ..) */
            lk_brkInit(b + 0x5614, gabi::load<u32>(LK_FIELD(u32, 0x55E4) + 0xAC), pYaura00Brk);
            curYaura00Brk = pYaura00Brk;
        }
        if (pYmgcs00Brk != curYmgcs00Brk) {
            lk_brkInit(b + 0x5704, gabi::load<u32>(LK_FIELD(u32, 0x5700) + 0xAC) /* mpYmgcs00Model->getModelData() */, pYmgcs00Brk);
            curYmgcs00Brk = pYmgcs00Brk;
        }
    }
    if (((noResetFlg1() & 1) || mTinkleShieldTimer != 0) &&
        (gabi::load<u8>(dComIfGp_ea() + 0x5292) == 0 ||
         (gabi::load<u16>(b + 0xF8) == 3 /* eventInfo.checkCommandDoor() */ && !(gabi::load<u16>(dComIfGp_ea() + 0x52B8) & 4) /* dEvtFlag_TALK_e */))) {
        if (yauraFrame < 0.01f) {
            for (int idx = 0; idx < 6; idx++) {
                gabi::store<f32>(b + 0x55E4 + idx * 8 + 4, cM_rndF(y00BtkFrameMax)); /* mMagicArmorAuraEntries[idx].setFrame(..) */
            }
        }
        yauraFrame = yauraFrame + 1.0f;
        if (!(yauraFrame < (f32)anm_getFrameMax(curYaura00Brk))) {
            yauraFrame = (f32)anm_getFrameMax(curYaura00Brk) - 0.001f;
        }
        ymgcsFrame = ymgcsFrame + 1.0f;
        if (!(ymgcsFrame < (f32)anm_getFrameMax(curYmgcs00Brk))) {
            ymgcsFrame = (f32)anm_getFrameMax(curYmgcs00Brk) - 0.001f;
        }
        seStartMapInfo(0x107E /* JA_SE_LK_MG_SHELD_USING */);
    } else {
        yauraFrame = yauraFrame - 1.0f;
        yauraFrame = fsel_l(yauraFrame, yauraFrame, 0.0f);
        ymgcsFrame = ymgcsFrame - 1.0f;
        ymgcsFrame = fsel_l(ymgcsFrame, ymgcsFrame, 0.0f);
    }
    /* mYaura00rBrk.entryFrame(yauraFrame); mYmgcs00Brk.entryFrame(ymgcsFrame) (HD: frame callback objects) */
    LK_FIELD(f32, 0x5618) = yauraFrame;
    gabi::store<f32>(LK_FIELD(u32, 0x5624), yauraFrame);
    lk_hdAnmFrame(b, 0x5634, yauraFrame);
    LK_FIELD(f32, 0x5708) = ymgcsFrame;
    gabi::store<f32>(LK_FIELD(u32, 0x5714), ymgcsFrame);
    lk_hdAnmFrame(b, 0x5724, ymgcsFrame);
    if (yauraFrame > 0.0f) {
        f32 var_f28;
        f32 var_f31;
        if (dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */) && gabi::load<u32>(dComIfGp_ea() + 0x5B3C) != 0) {
            var_f31 = 0.0f;
            u32 ship = gabi::load<u32>(dComIfGp_ea() + 0x5B3C);
            var_f28 = std::fabs(gabi::load<f32>(ship + 0x370)) / 15.0f + 1.0f;
            if (var_f28 > 1.5f) {
                var_f28 = 1.5f;
            }
        } else {
            var_f31 = std::fabs((f32)mNormalSpeed) / 17.0f; /* HD: m_HIO->mMove.m.field_0x18 folded */
            if (var_f31 > 1.0f) {
                var_f31 = 1.0f;
            }
            var_f28 = 1.0f;
        }
        s16 a = gabi::call<s16>(0x02019510 /* cM_rad2s */, (var_f31 * 0.5f) * 3.1415927f); /* cM_fcos(M_PI * (0.5f * var_f31)) */
        gabi::Local<cXyz> localScale;
        cXyz_ml(&scale, localScale, cM_scos((u16)a) * var_f28);
        f32 f25 = gabi::fnmsubs(var_f31, 0.3f, 1.0f);
        gabi::Local<cXyz> t0;
        gabi::Local<cXyz> t1;
        u32 entry = b + 0x55E4; /* mMagicArmorAuraEntries */
        for (int idx = 0; idx < 6; idx++, entry += 8) {
            f32 var_f0 = gabi::load<f32>(entry + 4) + 1.0f;
            if (!(var_f0 < y00BtkFrameMax)) {
                var_f0 -= y00BtkFrameMax;
            }
            gabi::store<f32>(entry + 4, var_f0);
            u16 jnt = gabi::load<u16>(0x10035AA0 + idx * 2); /* aura_model_joint[idx] */
            u32 blk = gabi::load<u32>(gabi::ea(mpCLModel.get()) + 0x2C);
            u32 mtx = gabi::load<u32>(blk + 0x10) + jnt * 0x30;
            u32 model = gabi::load<u32>(entry);
            gabi::store<u16>(blk + 4, gabi::load<u16>(blk + 4) | 0x10);
            lk_setBaseTRMtx(model, mtx);
            u32 m2 = gabi::load<u32>(entry);
            u32 sc;
            if (idx == 0) {
                cXyz_ml(&scale, t0, var_f31);
                cXyz_ml(t0, t1, 1.2f);
                sc = gabi::ea(t1.get());
            } else if (idx != 1) {
                sc = gabi::ea(localScale.get());
            } else {
                cXyz_ml(&scale, t0, f25);
                cXyz_ml(t0, t1, 2.0f);
                sc = gabi::ea(t1.get());
            }
            f32 sz = gabi::load<f32>(sc + 8); /* setBaseScale */
            f32 sy = gabi::load<f32>(sc + 4);
            f32 sx = gabi::load<f32>(sc + 0);
            gabi::store<f32>(m2 + 0xC0, sy);
            gabi::store<f32>(m2 + 0xC4, sz);
            gabi::store<f32>(m2 + 0xBC, sx);
        }
    }
    if ((noResetFlg1() & 1) || mTinkleShieldTimer != 0 || gabi::load<f32>(LK_FIELD(u32, 0x5714)) > 0.0f) {
        u32 m = gabi::ea(mpCLModel.get());
        lk_setBaseTRMtx(LK_FIELD(u32, 0x5700), m ? m + 0xC8 : 0); /* mpYmgcs00Model->setBaseTRMtx(mpCLModel->getBaseTRMtx()) */
        J3DModel_calc(gabi::at<J3DModel>(LK_FIELD(u32, 0x5700)));
        /* simpleAnmPlay(mpYmgcs00Btk) (HD: the btk object at 0x577C and its frame callback object) */
        gabi::call(0x025E742C /* mDoExt_baseAnm::play */, b + 0x577C);
        f32 fr = LK_FIELD(f32, 0x5780);
        gabi::store<f32>(LK_FIELD(u32, 0x57E4), fr);
        lk_hdAnmFrame(b, 0x578C, fr);
    }
}
VERIFY(0x0240B7F4, &daPy_lk_c::setAuraEffect);

/* JPABaseEmitter::setGlobalScale (HD: also the particle scale; the static is read twice) */
static inline void lk_emitterSetGlobalScale2(u32 e, u32 v) {
    gabi::store<f32>(e + 0x220, gabi::load<f32>(v + 0));
    gabi::store<f32>(e + 0x224, gabi::load<f32>(v + 4));
    gabi::store<f32>(e + 0x228, gabi::load<f32>(v + 8));
    gabi::store<f32>(e + 0x238, gabi::load<f32>(v + 0));
    gabi::store<f32>(e + 0x23C, gabi::load<f32>(v + 4));
    gabi::store<f32>(e + 0x240, gabi::load<f32>(v + 8));
}
/* HD: the "Demo07" water ripple: in that demo, with a demo actor id and after frame 2600 */
static inline bool lk_checkDemo07Ripple(daPy_lk_c* p) {
    u8 id = p->demoActorID;
    if (id == 0 || id > 0x20) {
        return false;
    }
    u32 obj = gabi::load<u32>(0x101D5FFC); /* dComIfGp_demo_getActor(id) */
    if (obj == 0) {
        JUT_ASSERT_fail(STR(0x10035038) /* "d_demo.h" */, 0x23A, STR(0x10034F74) /* "m_object != (0)" */);
        obj = gabi::load<u32>(0x101D5FFC);
    }
    if (gabi::call<u32>(0x02526E70 /* dDemo_object_c::getActor */, obj, (u32)id) == 0) {
        return false;
    }
    gabi::Local<lk_SafeString_l> b; /* {0x1047E6B8} == "Demo07" */
    b->__vtbl = LK_SAFESTRING_VTBL;
    b->mStr = 0x1047E6B8;
    gabi::Local<lk_SafeString_l> a;
    a->mStr = 0x10035A98;
    a->__vtbl = LK_SAFESTRING_VTBL;
    gabi::call(0x02444F48, a.get()); /* (empty) */
    lk_ss_assure(a.get());
    u32 pa = a->mStr;
    lk_ss_assure(b.get());
    u32 pb = b->mStr;
    if (pa != pb) {
        pa = a->mStr;
        pb = b->mStr;
        u32 n;
        for (n = 0; n < 0x40001; n++) {
            u8 ca = gabi::load<u8>(pa + n);
            u8 cb = gabi::load<u8>(pb + n);
            if (ca != cb) {
                return false;
            }
            if (ca == 0) {
                break;
            }
        }
        if (n == 0x40001) {
            return false;
        }
    }
    return gabi::load<u32>(0x101D600C) > 0xA28;
}

/* 0240AB88 */
void daPy_lk_c::setWaterRipple() {
    WWHD_FUNC(0x0240AB88, void, this);
    u32 b = gabi::ea(this);
    u32 normal_ripple_scale = 0x1046CD54;
    if (gabi::load<u32>(0x1046D1B4) == 0) {
        gabi::store<f32>(normal_ripple_scale + 0, 1.0f);
        gabi::store<f32>(normal_ripple_scale + 8, 1.0f);
        gabi::store<u32>(0x1046D1B4, 1);
        gabi::store<f32>(normal_ripple_scale + 4, 1.0f);
    }
    u32 small_ripple_scale = 0x1046CD60;
    lk_staticVec(0x1046D1B8, small_ripple_scale, 0.4f, 0.4f, 0.4f);

    BOOL var_r3 = mCurProc == 0xB2 /* daPyProc_DEMO_DEAD_e */ && dComIfGp_checkPlayerStatus0_l(0x100000 /* daPyStts0_SWIM_e */);
    bool ripple;
    bool forced = false;
    if (var_r3 && mProcVar3 != 0) {
        ripple = true;
    } else {
        ripple = false;
        if (mCurProc != 0x97 /* daPyProc_VOMIT_WAIT_e */ && !var_r3 &&
            (!(mModeFlg & 0x2020 /* ModeFlg_IN_SHIP | ModeFlg_HANG */) || (mModeFlg & 0x40000 /* ModeFlg_SWIM */))) {
            if ((mNoResetFlg0 & 0x80) && mWaterY > mCyl.mCyl.mCenter.y + 5.0f && mWaterY < mCyl.mCyl.mCenter.y + mCyl.mCyl.mHeight) {
                ripple = true;
            } else if (mCurrAttributeCode == 0x13 /* dBgS_Attr_WATER_e */ && (mAcch.m_flags & 0x20)) {
                ripple = true;
            }
        }
        if (!ripple) {
            forced = lk_checkDemo07Ripple(this);
            ripple = forced;
        }
    }
    f32 rippleScale5 = 5.0f;
    if (ripple) {
        u32 pos = gabi::ea(&current.pos);
        if (forced) {
            /* HD: the Demo07 ripple stays at a fixed place */
            pos = 0x1046CD78;
            if (gabi::load<u32>(0x1046D1C0) == 0) {
                gabi::store<u32>(0x1046D1C0, 1);
                gabi::store<f32>(pos + 8, -10430.0f);
                gabi::store<f32>(pos + 0, -26680.0f);
                gabi::store<f32>(pos + 4, -3555.0f);
            }
        }
        if (gabi::load<u32>(b + 0x6698) == 0) { /* m3280.getEmitter() */
            /* dComIfGp_particle_setShipTail(dPa_name::ID_AK_JN_HAMON00, &pos, NULL, NULL, 0xFF, &m3280) */
            dPa_control_set(dComIfGp_getParticle(), 5, 0x33, gabi::at<cXyz>(pos), nullptr, nullptr, 0xFF, gabi::at<dPa_levelEcallBack>(b + 0x6694),
                            -1, nullptr, nullptr, nullptr);
        }
        f32 var_f1 = std::fabs((f32)speedF) * 0.1f;
        var_f1 = var_f1 * var_f1;
        if (var_f1 > 1.0f) {
            var_f1 = 1.0f;
        }
        gabi::store<f32>(b + 0x66A4, var_f1); /* m3280.setRate(var_f1) */
        /* simpleAnmPlay(mpSuimenMunyaBtk) (HD: the btk object at 0x537C and its frame callback object) */
        gabi::call(0x025E742C /* mDoExt_baseAnm::play */, b + 0x537C);
        f32 fr = LK_FIELD(f32, 0x5380);
        gabi::store<f32>(LK_FIELD(u32, 0x53E4), fr);
        lk_hdAnmFrame(b, 0x538C, fr);
        u32 e = gabi::load<u32>(b + 0x6698);
        if (e != 0) {
            f32 sx = gabi::load<f32>(e + 0x238); /* getGlobalParticleScale().x */
            u32 f80 = mNoResetFlg0 & 0x80;
            bool remove;
            if (sx < 0.8f && f80) {
                remove = true;
            } else if (!(sx > 0.8f)) {
                remove = false;
            } else {
                remove = f80 == 0;
            }
            if (remove) {
                gabi::call(0x025A9270 /* dPa_rippleEcallBack::end */, b + 0x6694);
                e = gabi::ea(dPa_control_set(dComIfGp_getParticle(), 5, 0x33, gabi::at<cXyz>(pos), nullptr, nullptr, 0xFF,
                                             gabi::at<dPa_levelEcallBack>(b + 0x6694), -1, nullptr, nullptr, nullptr));
                if (e != 0) {
                    f80 = mNoResetFlg0 & 0x80;
                }
            }
            if (e != 0) {
                if (f80) {
                    lk_emitterSetGlobalScale2(e, normal_ripple_scale);
                } else {
                    lk_emitterSetGlobalScale2(e, small_ripple_scale);
                }
            }
        }
    } else if (gabi::load<u32>(b + 0x6698) != 0) {
        gabi::call(0x025A9270 /* dPa_rippleEcallBack::end */, b + 0x6694); /* m3280.remove() */
    }

    if (mModeFlg & 0x40000 /* ModeFlg_SWIM */) {
        u8 end0 = gabi::load<u8>(b + 0x66AC);
        u8 end1 = gabi::load<u8>(b + 0x66D4);
        if (!end0) {
            gabi::store<f32>(b + 0x66B0, std::fabs(mNormalSpeed / LK_FIELD(f32, 0x3C4))); /* mSwimTailEcallBack[0].setSpeedRate */
        }
        if (!end1) {
            gabi::store<f32>(b + 0x66D8, std::fabs(mNormalSpeed / LK_FIELD(f32, 0x3C4)));
        }
        gabi::Local<cXyz> local_1c;
        PSMTXMultVec(lk_getAnmMtx(mpCLModel, 15 /* CL_JNT_HEAD_JNT_e */), gabi::at<cXyz>(0x10034FFC) /* wave_offset */, local_1c);
        u16 a = (u16)shape_angle.y;
        f32 lz = local_1c->z;
        f32 lx = local_1c->x;
        f32 water = mWaterY;
        f32 flat = LK_FIELD(f32, 0x9C8); /* mAcch.m_wtr.GetHeight() */
        f32 c5 = rippleScale5 * cM_scos(a);
        f32 s5 = rippleScale5 * cM_ssin(a);
        gabi::store<f32>(b + 0x66C0, water); /* mSwimTailEcallBack[0].setPos(local_20).y */
        gabi::store<f32>(b + 0x66E8, water);
        local_1c->y = water;
        f32 w2 = water + 2.0f;
        gabi::store<f32>(b + 0x66E0, flat); /* [1].setWaterFlatY */
        gabi::store<f32>(b + 0x66B8, flat); /* [0].setWaterFlatY */
        gabi::store<f32>(b + 0x66B4, w2);   /* [0].setWaterY(mWaterY + 2.0f) */
        gabi::store<f32>(b + 0x66DC, w2);
        gabi::store<f32>(b + 0x66C4, lz - s5);
        gabi::store<f32>(b + 0x66BC, lx + c5);
        gabi::store<f32>(b + 0x66E4, lx - c5);
        gabi::store<f32>(b + 0x66EC, lz + s5);
        gabi::Local<cXyz> local_40;
        PSMTXMultVec(lk_getAnmMtx(mpCLModel, 15), gabi::at<cXyz>(0x10035008) /* swim_offset */, local_40);
        u32 munya = LK_FIELD(u32, 0x5378); /* mpSuimenMunyaModel */
        u32 mm = munya ? munya + 0xC8 : 0;
        gabi::Local<cXyz> local_4c; /* mDoMtx_multVecZero(suimenMunyaMtx, &local_4c) */
        local_4c->x = gabi::load<f32>(mm + 0xC);
        local_4c->y = gabi::load<f32>(mm + 0x1C);
        local_40->y = mWaterY;
        local_4c->z = gabi::load<f32>(mm + 0x2C);
        gabi::call(0x028E8DAC /* PSVECSubtract */, local_4c.get(), local_40.get(), local_4c.get());
        gabi::Local<cXyz> xz; /* local_4c.absXZ() */
        xz->z = local_4c->z;
        xz->y = 0.0f;
        xz->x = local_4c->x;
        f32 d = std_sqrtf(PSVECSquareMag(xz));
        f32 sz = gabi::fmadds(d / LK_FIELD(f32, 0x3C4), 0.5f, 1.0f);
        if (sz > 1.5f) {
            sz = 1.5f;
        }
        if (mCurProc == 0xB2 /* daPyProc_DEMO_DEAD_e */) {
            u16 a2 = (u16)shape_angle.y;
            f32 px = current.pos.x;
            f32 pz = current.pos.z;
            f32 ly = local_40->y;
            mDoMtx_stack_c::transS(gabi::fmadds(60.0f, cM_ssin(a2), px), ly + 2.0f, gabi::fmadds(60.0f, cM_scos(a2), pz));
        } else {
            mDoMtx_stack_c::transS(local_40->x, local_40->y + 2.0f, local_40->z);
        }
        if (gabi::load<u8>(0x101EA4E4) & 1 /* dPa_control_c::isStatus(0x01) */) {
            Mtx34* hm = lk_getAnmMtx(mpCLModel, 15);
            f32 x5c = hm->m[0][3];
            f32 z5c = hm->m[2][3];
            f32 y5c = hm->m[1][3];
            gabi::Local<cXyz> local_68;
            PSMTXMultVec(lk_getAnmMtx(mpCLModel, 15), gabi::at<cXyz>(0x10035014) /* swim_side_offset */, local_68);
            f32 x2 = x5c + x5c;
            f32 z2 = z5c + z5c;
            gabi::Local<cXyz> local_74;
            gabi::Local<cXyz> local_80;
            local_80->y = y5c;
            local_74->x = x2 - local_68->x;
            local_74->y = y5c;
            local_74->z = z2 - local_68->z;
            local_80->z = z2 - local_40->z;
            local_80->x = x2 - local_40->x;
            if (daSea_ChkArea(local_68->x, local_68->z)) {
                local_68->y = daSea_calcWave(local_68->x, local_68->z);
            }
            if (daSea_ChkArea(local_74->x, local_74->z)) {
                local_74->y = daSea_calcWave(local_74->x, local_74->z);
            }
            if (daSea_ChkArea(local_80->x, local_80->z)) {
                local_80->y = daSea_calcWave(local_80->x, local_80->z);
            }
            gabi::Local<cXyz> local_b8;
            cXyz_mi(local_40, local_b8, local_80);
            gabi::Local<cXyz> local_c4;
            cXyz_mi(local_74, local_c4, local_68);
            gabi::Local<cXyz> xz1;
            xz1->x = local_b8->x;
            xz1->y = 0.0f;
            xz1->z = local_b8->z;
            f32 d1 = std_sqrtf(PSVECSquareMag(xz1));
            s16 ax = cM_atan2s(-local_b8->y, d1);
            s16 ay = shape_angle.y;
            gabi::Local<cXyz> xz2;
            xz2->x = local_c4->x;
            xz2->y = 0.0f;
            xz2->z = local_c4->z;
            f32 d2 = std_sqrtf(PSVECSquareMag(xz2));
            s16 az = cM_atan2s(-local_c4->y, d2);
            mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), ax, ay, az);
        } else {
            mDoMtx_stack_c::YrotM(shape_angle.y);
        }
        lk_setBaseTRMtx(LK_FIELD(u32, 0x5378), gabi::ea(mDoMtx_stack_c::get()));
        u32 m2 = LK_FIELD(u32, 0x5378); /* setBaseScale(local_50) */
        gabi::store<f32>(m2 + 0xBC, 1.0f);
        gabi::store<f32>(m2 + 0xC4, sz);
        gabi::store<f32>(m2 + 0xC0, 1.0f);
    }
    /* static JGeometry::TVec3<f32> waterfall_splash_trans(0.0f, 15.0f, 0.0f) */
    u32 waterfall_splash_trans = 0x1046CD6C;
    if (gabi::load<u32>(0x1046D1BC) == 0) {
        gabi::store<f32>(waterfall_splash_trans + 8, 0.0f);
        gabi::store<f32>(waterfall_splash_trans + 0, 0.0f);
        gabi::store<f32>(waterfall_splash_trans + 4, 15.0f);
        gabi::store<u32>(0x1046D1BC, 1);
    }
    bool wf;
    if (((mAcch.m_flags & 0x20) || (mModeFlg & 0x40000)) && mCurrAttributeCode == 0x17 /* dBgS_Attr_WATERFALL_e */) {
        wf = true;
    } else {
        wf = (mModeFlg & 0x20 /* ModeFlg_HANG */) && m3588 == 0x17;
    }
    if (wf) {
        if (gabi::load<u32>(b + 0x6818) == 0) { /* m3400.getEmitter() */
            Mtx34* hm = lk_getAnmMtx(mpCLModel, 15 /* CL_JNT_HEAD_JNT_e */);
            gabi::call(0x023D457C /* daPy_mtxFollowEcallBack_c::makeEmitter */, b + 0x6814, 0x23 /* ID_AK_JN_ELEMENTSHIBUKI00 */, hm, &current.pos, 0);
            u32 e = gabi::load<u32>(b + 0x6818);
            if (e != 0) {
                gabi::store<f32>(e + 0x58, 1.0f); /* setSpread */
                gabi::store<f32>(e + 0x34, 4.0f); /* setRate */
                lk_storeVec(e + 0x14, waterfall_splash_trans); /* setEmitterTranslation */
            }
        }
        if (m34CD == 0) {
            gabi::Local<cXyz> local_ac;
            lk_jntPos(mpCLModel, 0 /* CL_JNT_LINK_ROOT_e */, local_ac);
            /* dComIfGp_particle_setSingleRipple(dPa_name::ID_IT_JN_WP_HAMON03, &local_ac, NULL, &waterfall_ripple_scale) */
            dPa_control_set(dComIfGp_getParticle(), 5, 0x3F, local_ac, nullptr, gabi::at<cXyz>(0x101CEC68), 0xFF,
                            gabi::at<dPa_levelEcallBack>(0x1047B2E4), -1, nullptr, nullptr, nullptr);
            m34CD = 15;
            gabi::Local<cXyz> v;
            v->x = 0.0f;
            v->y = 1.0f;
            v->z = 0.0f;
            gabi::call(0x025CB408 /* dVibration_c::StartQuake */, dComIfGp_getVibration(), 4, 1, v.get());
        } else {
            u8 n = (u8)(m34CD - 1);
            m34CD = n;
            if (n == 5) {
                gabi::call(0x025CB610 /* dVibration_c::StopQuake */, dComIfGp_getVibration(), -1);
            }
        }
    } else {
        if (gabi::load<u32>(b + 0x6818) != 0) {
            gabi::call(0x025CB610 /* dVibration_c::StopQuake */, dComIfGp_getVibration(), -1);
        }
        gabi::call(LK_mtxFollowEcallBack_end, b + 0x6814); /* m3400.end() */
    }
    if (gabi::load<u32>(b + 0x6818) != 0) {
        gabi::Local<GXColor> amb;
        gabi::Local<GXColor> dif;
        gabi::call(0x025602F0 /* dKy_get_seacolor */, amb.get(), dif.get());
        u32 e = gabi::load<u32>(b + 0x6818); /* setGlobalPrmColor(amb.r, amb.g, amb.b) */
        u8 bl = amb->b;
        u8 g = amb->g;
        u8 r = amb->r;
        gabi::store<u8>(e + 0x245, g);
        gabi::store<u8>(e + 0x246, bl);
        gabi::store<u8>(e + 0x244, r);
    }
}
VERIFY(0x0240AB88, &daPy_lk_c::setWaterRipple);

/* (s16)ftoi(f) (stfiwx, then the low halfword) */
static inline s16 lk_ftos(f32 f) { return (s16)gabi::ftoi(f); }

/* 024022D0 */
void daPy_lk_c::setHatAngle() {
    WWHD_FUNC(0x024022D0, void, this);
    if (gabi::load<u8>(m_old_fdata) == 0 /* m_old_fdata->getOldFrameFlg() */ || (noResetFlg1() & 0x800) /* checkFreezeState() */) {
        return;
    }
    Mtx34* head_jnt_mtx = lk_getAnmMtx(mpCLModel, 15 /* CL_JNT_HEAD_JNT_e */);
    BOOL r31 = TRUE;
    if (!gabi::call<BOOL>(0x025445B8 /* dEvent_manager_c::startCheckOld */, dComIfGp_getPEvtManager(), 0x101CECCCu /* l_tact_wind_change_event_label */) &&
        !gabi::call<BOOL>(0x025445B8, dComIfGp_getPEvtManager(), 0x101CECACu /* l_tact_wind_change_event_label2 */) &&
        mCurProc != 0x93 /* daPyProc_FAN_GLIDE_e */ &&
        !gabi::call<BOOL>(0x025162A4 /* dCcD_GObjInf::ChkTgHit */, &mWindCyl)) {
        r31 = FALSE;
    }
    gabi::Local<cXyz> sp70;
    gabi::Local<cXyz> sp64;
    PSMTXMultVec(head_jnt_mtx, gabi::at<cXyz>(0x101CEBB4) /* l_head_center_offset */, sp70);
    PSMTXMultVec(head_jnt_mtx, gabi::at<cXyz>(0x101CEBA8) /* l_eye_offset */, sp64);
    gabi::Local<cXyz> sp58;
    cXyz_mi(sp64, sp58, sp70);
    s16 r3_3 = cM_atan2s(sp58->x, sp58->z);
    if (sp70->y - head_jnt_mtx->m[1][3] < 0.0f) {
        r3_3 = (s16)(r3_3 - 0x8000);
    }
    f32 f31 = cM_ssin((u16)r3_3);
    f32 f30 = cM_scos((u16)r3_3);
    gabi::Local<cXyz> spA0;
    lk_jntPos(mpCLModel, 26 /* CL_JNT_HATA_JNT_e */, spA0);
    gabi::Local<cXyz> spAC;
    gabi::Local<be<f32>> sp18;
    gabi::call(0x0257E1B8 /* dKyw_get_AllWind_vec */, spA0.get(), spAC.get(), sp18.get());
    if (r31) {
        *sp18 = 1.0f;
    }
    f32 w = *sp18;
    f32 f29 = (w * w) * 25.0f;
    gabi::Local<cXyz> sp88;
    gabi::Local<cXyz> sp7C;
    PSMTXMultVecSR(lk_getAnmMtx(mpCLModel, 15), gabi::at<cXyz>(0x10034FC0) /* l_neck_front */, sp88);
    PSMTXMultVecSR(lk_getAnmMtx(mpCLModel, 15), gabi::at<cXyz>(0x10034FCC) /* l_neck_top */, sp7C);
    s16 r25 = m3528;
    s16 r26 = m352A;
    s16 newX;
    gabi::Local<cXyz> xz;
    if (sp7C->y < 0.0f) {
        xz->y = 0.0f;
        xz->x = sp88->x;
        xz->z = sp88->z;
        f32 d = std_sqrtf(PSVECSquareMag(xz));
        m3528 = cM_atan2s(sp88->y, -d);
        newX = (s16)(cM_atan2s(sp88->x, sp88->z) + 0x8000);
    } else {
        xz->y = 0.0f;
        xz->x = sp88->x;
        xz->z = sp88->z;
        f32 d = std_sqrtf(PSVECSquareMag(xz));
        m3528 = cM_atan2s(sp88->y, d);
        newX = cM_atan2s(sp88->x, sp88->z);
    }
    if (std::fabs((f32)sp88->y) > 0.7f) {
        newX = r26;
    }
    s16 cur3528 = m3528;
    m352A = newX;
    s16 r28_r29 = (s16)(cur3528 - r25) >> 1;
    s16 r25_r28 = (s16)(newX - r26) >> 1;
    if (r28_r29 > 0x200) {
        r28_r29 = 0x200;
    } else if (r28_r29 < -0x200) {
        r28_r29 = -0x200;
    }
    if (r25_r28 > 0x800) {
        r25_r28 = 0x800;
    } else if (r25_r28 < -0x800) {
        r25_r28 = -0x800;
    }
    gabi::Local<cXyz> sp94;
    sp94->y = (m3718.y - spA0->y) - 7.5f;
    sp94->x = m3718.x - spA0->x;
    sp94->z = m3718.z - spA0->z;
    if (!(mModeFlg & 0x10 /* ModeFlg_WHIDE */) || LK_FIELD(f32, 0x3CC) < 0.0f /* checkGrabWear() */) {
        gabi::Local<cXyz> t;
        cXyz_ml(spAC, t, f29);
        PSVECAdd(sp94, t, sp94);
    }
    if (std::fabs((f32)sp94->x) < 0.01f) {
        sp94->x = 0.0f;
    }
    if (std::fabs((f32)sp94->z) < 0.01f) {
        sp94->z = 0.0f;
    }
    f32 px = sp94->x;
    f32 py = sp94->y;
    f32 pz = sp94->z;
    f32 f28 = gabi::fmadds(pz, f30, px * f31);
    s16 r26_2 = m34F6;
    s16 r27 = m34F8;
    s32 r4_3 = cM_atan2s(-f28, -py);
    if (m34F6 < 0) {
        r4_3 = 0x7800;
    } else if (r4_3 < 0x800 && -0x7800 < r4_3) {
        r4_3 = 0x800;
    } else if (0x7800 < r4_3 || r4_3 <= -0x7800) {
        r4_3 = 0x7800;
    }
    cLib_addCalcAngleS2(&m34F6, (s16)r4_3, 5, 0x400);
    {
        s32 v = m34F6 + (m3502 + r28_r29);
        s16 r0 = m3528;
        s16 t = (s16)(v - 0x4000 + r0);
        m34F6 = (s16)v;
        if (t < -0x3000) {
            m34F6 = (s16)(0x1000 - r0);
        } else if (t > 0x3800) {
            m34F6 = (s16)(0x7800 - r0);
        }
    }
    f32 f0_2 = std_sqrtf(gabi::fmadds(f28, f28, py * py));
    s32 r4_5 = cM_atan2s(-gabi::fmsubs(sp94->x, f30, sp94->z * f31), f0_2);
    if (r4_5 > 0x3800) {
        r4_5 = 0x3800;
    } else if (r4_5 < -0x3800) {
        r4_5 = -0x3800;
    }
    cLib_addCalcAngleS2(&m34F8, (s16)r4_5, 5, 0x400);
    {
        s16 v = (s16)(m34F8 + (m3504 - r25_r28));
        if (v > 0x3800) {
            v = 0x3800;
        } else if (v < -0x3800) {
            v = -0x3800;
        }
        s16 r3 = (s16)(m34F6 - r26_2);
        m34F8 = v;
        m3502 = lk_ftos((f32)r3 * 0.2f);
        s16 r4 = (s16)(m34F8 - r27);
        m3504 = lk_ftos((f32)r4 * 0.2f);
        m34FA = (s16)(m34FA - r3);
        m34FC = (s16)(m34FC - r4);
    }
    s16 r27_2 = m34FA;
    s16 r26_3 = m34FC;
    cLib_addCalcAngleS2(&m34FA, 0, 5, 0x400);
    cLib_addCalcAngleS2(&m34FC, 0, 5, 0x400);
    {
        s16 a = (s16)(m34FA + (m3506 + r28_r29));
        s16 c = (s16)(m34FC + (m3508 - r25_r28));
        m34FA = a;
        m34FC = c;
        if (a > 0x1000) {
            m34FA = 0x1000;
        } else if (a < -0x800) {
            m34FA = -0x800;
        }
        s16 f8v = m34F8;
        s16 r0 = (s16)(m34FC + f8v);
        if (r0 > 0x3800) {
            m34FC = (s16)(0x3800 - f8v);
        } else if (r0 < -0x3800) {
            m34FC = (s16)(-0x3800 - f8v);
        }
        s32 d1 = m34FA - r27_2;
        m3506 = lk_ftos((f32)d1 * 0.2f);
        s32 d2 = m34FC - r26_3;
        m3508 = lk_ftos((f32)d2 * 0.2f);
        m34FE = (s16)(m34FE - (s16)(m34FA - r27_2));
        m3500 = (s16)(m3500 - (s16)d2);
    }
    s16 r27_3 = m34FE;
    s16 r26_4 = m3500;
    cLib_addCalcAngleS2(&m34FE, 0, 5, 0x400);
    cLib_addCalcAngleS2(&m3500, 0, 5, 0x400);
    {
        s16 a = (s16)(m34FE + (m350A + r28_r29));
        s16 c = (s16)(m3500 + (m350C - r25_r28));
        m34FE = a;
        m3500 = c;
        if (a > 0x1000) {
            m34FE = 0x1000;
        } else if (a < -0x800) {
            m34FE = -0x800;
        }
        s16 fc = m34FC;
        s16 f8v = m34F8;
        s16 r4_6 = (s16)(m3500 + fc + f8v);
        if (r4_6 > 0x3800) {
            m3500 = (s16)((0x3800 - f8v) - fc);
        } else if (r4_6 < -0x3800) {
            m3500 = (s16)((-0x3800 - f8v) - fc);
        }
        s32 d1 = m34FE - r27_3;
        m350A = lk_ftos((f32)d1 * 0.2f);
        s32 d2 = m3500 - r26_4;
        m350C = lk_ftos((f32)d2 * 0.2f);
    }
    f32 dist = std_sqrtf(gabi::call<f32>(0x028E8DE8 /* PSVECSquareDistance */, &m3718, spA0.get()));
    f32 f28_2 = gabi::fmadds(dist, 0.65f, f29) / 30.0f;
    if (f28_2 > 1.0f) {
        f28_2 = 1.0f;
    }
    f32 f29_2 = f28_2;
    if (r31 && !(noResetFlg1() & 8 /* daPyFlg1_CASUAL_CLOTHES */)) {
        f28_2 = 3.5f;
        f29_2 = 1.0f;
        mDoAud_seStart(0x2056 /* JA_SE_LK_HAT_SWING */, &eyePos, 0, (s32)(s8)mReverb);
    }
    s16 r25_2 = lk_ftos(gabi::fmadds(f28_2, 4060.0f, 1500.0f));
    s32 m14 = m3514 + r25_2;
    m3514 = (s16)m14;
    f32 f0 = cM_scos((u16)m14);
    m350E = lk_ftos((f29_2 * 2280.0f) * f0);
    f32 fr = (f32)(s32)r25_2;
    f32 fm = (f32)(s32)(s16)m14;
    u16 i1 = (u16)gabi::ftoi(gabi::fnmsubs(3.0f, fr, fm));
    m3510 = lk_ftos((f29_2 * 3908.0f) * cM_scos(i1));
    u16 i2 = (u16)gabi::ftoi(gabi::fnmsubs(6.0f, fr, fm));
    u32 mf = mModeFlg;
    m3718.copy(*spA0);
    m3512 = lk_ftos((f29_2 * 7568.0f) * cM_scos(i2));
    if (mf & 0x40000 /* ModeFlg_SWIM */) {
        m351A = 0;
        m3516 = 0;
        m3518 = 0;
        return;
    }
    f32 ax = spAC->x;
    f32 az = spAC->z;
    f32 sz = gabi::fmadds(ax, f31, az * f30);  /* sp4C.z */
    f32 sx = gabi::fmsubs(ax, f30, az * f31);  /* sp4C.x */
    f32 f5 = gabi::fmadds(f0 + 1.0f, 0.25f, 0.5f);
    f32 ws = *sp18;
    s16 v16 = lk_ftos(((-8192.0f * sx) * ws) * f5);
    s16 v1a = lk_ftos(((-8192.0f * sz) * ws) * f5);
    m3516 = v16;
    m3518 = v16;
    m351A = v1a;
    if (v16 > 0x1000) {
        m3516 = 0x1000;
    } else if (m3518 < -0x1000) {
        m3518 = -0x1000;
    }
}
VERIFY(0x024022D0, &daPy_lk_c::setHatAngle);

/* 02400C4C */
void daPy_lk_c::setNeckAngle() {
    WWHD_FUNC(0x02400C4C, void, this);
    u32 b = gabi::ea(this);
    setNoResetFlg1(noResetFlg1() & ~0x400u); /* offNoResetFlg1(daPyFlg1_UNK400) */
    Mtx34* r25m = lk_getAnmMtx(mpCLModel, 15 /* CL_JNT_HEAD_JNT_e */);
    gabi::Local<be<u32>> sp18; /* the look target (cXyz*) */
    *sp18 = 0;
    s16 r30 = 0; /* (GameCube names) */
    s16 r29 = 0;
    BOOL r28 = FALSE;
    gabi::Local<cXyz> sp94;
    PSMTXMultVecSR(lk_getAnmMtx(mpCLModel, 1 /* CL_JNT_CENTER_e */), gabi::at<cXyz>(0x101FFBCC) /* cXyz::BaseZ */, sp94);
    u32 r24;
    if (dComIfGp_checkPlayerStatus0_l(0x10 /* daPyStts0_UNK10_e */)) {
        r24 = gabi::call<u32>(0x025D7C6C /* fopAcM_getTalkEventPartner */, this);
    } else {
        r24 = gabi::ea(mpAttnActorLockOn.get());
    }
    if (r24 == 0) {
        r24 = gabi::call<u32>(0x024FA018 /* dCamera_c::GetForceLockOnActor */, gabi::call<u32>(0x024F8044 /* dCam_getBody */));
    }
    u32 r23_2 = gabi::call<u32>(0x024EE058 /* dAttention_c::GetLockonList */, (u32)mpAttention, 0);
    if (r23_2 != 0) {
        r23_2 = gabi::call<u32>(0x024EBA14 /* dAttList_c::getActor */, r23_2);
    }
    gabi::Local<cXyz> spA0;
    gabi::Local<cXyz> sp98;
    if (gabi::load<u32>(dComIfGp_ea() + 0x5CDC) & 1 /* daPyStts1_WIND_WAKER_CONDUCT_e */) {
        PSMTXMultVec(lk_getAnmMtx(mpCLModel, 8 /* CL_JNT_CL_LHANDA_e */), gabi::at<cXyz>(0x10034FF0) /* l_tact_top */, spA0);
        *sp18 = gabi::ea(spA0.get());
        r28 = TRUE;
    } else if (mCurProc == 2 /* daPyProc_CALL_e */) {
        if (gabi::load<u32>(dComIfGp_ea() + 0x5B38) != 0) { /* dComIfGp_getCb1Player() */
            *sp18 = gabi::load<u32>(dComIfGp_ea() + 0x5B38) + 0x37C;
            r28 = TRUE;
        }
    } else if (gabi::load<u32>(dComIfGp_ea() + 0x5CDC) & 0x100000) {
        /* HD: the selfie pose looks at the camera (eye + offset); face 0x92 does not move the eyes */
        s32 idx = mCameraInfoIdx;
        u32 cam = gabi::load<u32>(dComIfGp_ea() + idx * 0x34 + 0x5AF8);
        cXyz_pl(gabi::at<cXyz>(cam + 0x264), sp98, gabi::at<cXyz>(cam + 0x7C0));
        *sp18 = gabi::ea(sp98.get());
        r28 = mFace != 0x92;
    } else if (dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */) && !gabi::call<BOOL>(0x023D69CC /* checkShipNotNormalMode */, this)) {
        bool shipLook = false;
        if (dComIfGp_checkPlayerStatus0_l(0x10) && gabi::call<u32>(0x025D7C6C, this) != 0 && gabi::load<u32>(dComIfGp_ea() + 0x5B3C) != 0) {
            u32 ship = gabi::load<u32>(dComIfGp_ea() + 0x5B3C);
            if (gabi::call<u32>(0x025D7C6C, this) == ship) {
                u32 ship2 = gabi::load<u32>(dComIfGp_ea() + 0x5B3C);
                if (cLib_distanceAngleS(cLib_targetAngleY(&current.pos, gabi::at<cXyz>(ship2 + 0x37C)), m34DE) <= 0x6000) {
                    shipLook = true;
                }
            }
        }
        if (shipLook) {
            *sp18 = gabi::load<u32>(dComIfGp_ea() + 0x5B3C) + 0x37C;
            r28 = TRUE;
        } else {
            u16 a = (u16)m34DE;
            f32 y = r25m->m[1][3];
            f32 x = r25m->m[0][3];
            f32 s = cM_ssin(a);
            f32 c = cM_scos(a);
            f32 z = r25m->m[2][3];
            spA0->y = y;
            spA0->x = gabi::fmadds(20000.0f, s, x);
            *sp18 = gabi::ea(spA0.get());
            spA0->z = gabi::fmadds(20000.0f, c, z);
        }
    } else if (mCurProc == 0x8F /* daPyProc_SHIP_CRANE_e */) {
        if (mProcVar2 == 0) {
            u32 ship = gabi::load<u32>(dComIfGp_ea() + 0x5B3C);
            if (ship != 0) {
                *sp18 = gabi::load<u32>(ship + 0x71C); /* getCraneTop() */
                r28 = TRUE;
            }
        }
    } else if (gabi::load<u16>(b + 0x65D0) == 0x208 /* m_tex_anm_heap.mIdx == dRes_INDEX_LKANM_BTP_TDASHKAZE_e */) {
        gabi::Local<cXyz> xz;
        bool done = false;
        if (gabi::call<BOOL>(LK_checkHeavyStateOn, this)) {
            xz->x = m373C.x;
            xz->y = 0.0f;
            xz->z = m373C.z;
            if (PSVECSquareMag(xz) > 25.0f) {
                f32 x = gabi::fnmsubs(100.0f, m373C.x, current.pos.x);
                f32 y = current.pos.y + 120.0f;
                spA0->x = x;
                spA0->y = y;
                spA0->z = gabi::fnmsubs(100.0f, m373C.z, current.pos.z);
                done = true;
            }
        }
        if (!done) {
            if (!gabi::call<BOOL>(LK_checkHeavyStateOn, this)) {
                gabi::Local<cXyz> xz2;
                xz2->x = m3730.x;
                xz2->y = 0.0f;
                xz2->z = m3730.z;
                if (PSVECSquareMag(xz2) > 25.0f) {
                    f32 y = current.pos.y + 120.0f;
                    spA0->y = y;
                    spA0->x = gabi::fnmsubs(100.0f, m3730.x, current.pos.x);
                    spA0->z = gabi::fnmsubs(100.0f, m3730.z, current.pos.z);
                    done = true;
                }
            }
        }
        if (!done) {
            f32 k = 100.0f * m3644;
            u16 a = (u16)m3640;
            f32 y = current.pos.y + 120.0f;
            spA0->y = y;
            spA0->x = gabi::fnmsubs(k, cM_ssin(a), current.pos.x);
            spA0->z = gabi::fnmsubs(k, cM_scos(a), current.pos.z);
        }
        *sp18 = gabi::ea(spA0.get());
        r28 = TRUE;
    } else if (r24 != 0 && cLib_distanceAngleS(cLib_targetAngleY(&current.pos, gabi::at<cXyz>(r24 + 0x37C)), m34DE) <= 0x6000) {
        *sp18 = r24 + 0x37C;
        r28 = TRUE;
    } else if (mModeFlg & 0x08000080 /* ModeFlg_00000080 | ModeFlg_08000000 */) {
        u32 mode = 0;
        if (gabi::load<u8>(dComIfGp_ea() + 0x5292) == 0 /* !dComIfGp_event_runCheck() */ || (mode = LK_FIELD(u32, 0x430)) == 0x39 /* DEMO_RD_STOP_e */) {
            /* dComIfGp_att_getLookTarget() */
            u32 play = dComIfGp_ea();
            if (gabi::call<u32>(0x024EC0A0 /* dAttHint_c::convPId */, play + 0x594C, gabi::load<u32>(play + 0x5804 + 0x154)) != 0) {
                u32 play2 = dComIfGp_ea();
                u32 lt = gabi::call<u32>(0x024EC0A0, play2 + 0x594C, gabi::load<u32>(play2 + 0x5804 + 0x154));
                *sp18 = lt + 0x37C;
                r28 = TRUE;
            } else if (gabi::call<BOOL>(0x02400B14, this, r23_2, sp18.get()) ||
                       gabi::call<BOOL>(0x02400B14, this, gabi::ea(mpAttnActorAction.get()), sp18.get()) ||
                       gabi::call<BOOL>(0x02400B14, this, gabi::ea(mpAttnActorX.get()), sp18.get()) ||
                       gabi::call<BOOL>(0x02400B14, this, gabi::ea(mpAttnActorY.get()), sp18.get()) ||
                       gabi::call<BOOL>(0x02400B14, this, gabi::ea(mpAttnActorZ.get()), sp18.get())) { /* checkAttentionPosAngle */
                r28 = TRUE;
            } else if (gabi::call<BOOL>(0x0252A068 /* dDetect_c::chk_attention */, dComIfGp_ea() + 0x5A20, spA0.get()) &&
                       cLib_distanceAngleS(cLib_targetAngleY(&current.pos, spA0), m34DE) <= 0x6000) {
                *sp18 = gabi::ea(spA0.get());
                r28 = TRUE;
            } else if (m34C3 == 10) {
                f32 sx = m3730.x + m36B8.x;
                f32 sz = m3730.z + m36B8.z;
                f32 y = current.pos.y + 120.0f;
                spA0->y = y;
                spA0->x = gabi::fnmsubs(10.0f, sx, current.pos.x);
                spA0->z = gabi::fnmsubs(10.0f, sz, current.pos.z);
                if (cLib_distanceAngleS(cLib_targetAngleY(&current.pos, spA0), m34DE) <= 0x6000) {
                    *sp18 = gabi::ea(spA0.get());
                    r28 = TRUE;
                }
            }
        } else if (mCurProc == 0xA4 /* daPyProc_BOTTLE_OPEN_e */) {
            if (mActorKeepRope.mActor != NULL) {
                *sp18 = gabi::ea(mActorKeepRope.mActor.get()) + 0x37C;
                r28 = TRUE;
            }
        } else if (mCurProc == 0xBE /* daPyProc_DEMO_LOOK_WAIT_e */ || (u32)(mode - 1) <= 3 /* INIT_WAIT, N_WAIT, N_WALK, N_DASH */) {
            u32 la = gabi::call<u32>(0x02400BC4 /* getDemoLookActor */, this);
            if (la != 0) {
                *sp18 = la + 0x37C;
                r28 = TRUE;
            }
        }
    }
    gabi::Local<cXyz> spC4;
    gabi::Local<cXyz> sp88;
    PSMTXMultVec(r25m, gabi::at<cXyz>(0x101CEBB4) /* l_head_center_offset */, spC4);
    PSMTXMultVec(r25m, gabi::at<cXyz>(0x101CEBA8) /* l_eye_offset */, sp88);
    gabi::Local<cXyz> spAC;
    cXyz_mi(sp88, spAC, spC4);
    f32 acx = spAC->x;
    f32 acy = spAC->y;
    f32 acz = spAC->z;
    gabi::Local<cXyz> xz0;
    xz0->y = 0.0f;
    xz0->x = acx;
    xz0->z = acz;
    s16 r24_4 = (s16)(cM_atan2s(-acy, std_sqrtf(PSVECSquareMag(xz0))) - m3564.x);
    s16 r25_3 = (s16)((cM_atan2s(acx, acz) - m34DE) - m3564.y);
    s16 r4;
    s16 r23;
    if ((mModeFlg & 0x08000080) && *sp18 != 0 && gabi::load<u16>(b + 0x5888) != 0x64 /* !checkUpperAnime(DAMDASH) */) {
        gabi::Local<cXyz> spB8;
        cXyz_mi(gabi::at<cXyz>(*sp18), spB8, spC4);
        f32 bx = spB8->x;
        f32 by = spB8->y;
        f32 bz = spB8->z;
        gabi::Local<cXyz> xz1;
        xz1->y = 0.0f;
        xz1->x = bx;
        xz1->z = bz;
        s16 r27 = cM_atan2s(-by, std_sqrtf(PSVECSquareMag(xz1)));
        s16 r23_3 = (s16)(cM_atan2s(bx, bz) - m34DE);
        gabi::Local<cXyz> xz2;
        xz2->x = bx;
        xz2->y = 0.0f;
        xz2->z = bz;
        if (std_sqrtf(PSVECSquareMag(xz2)) < 30.0f) {
            r23_3 = m3564.y;
        }
        if (r27 > 8000) {
            r27 = 8000;
        } else if (r27 < -10000) {
            r27 = -10000;
        }
        if (r23_3 > 0x3800) { /* m_HIO->mShip.m.field_0x0 folded */
            r23_3 = 0x3800;
        } else if (r23_3 < -0x3800) {
            r23_3 = -0x3800;
        }
        if ((mModeFlg & 0x80) && gabi::load<u16>(b + 0x5868) != 0x73 && gabi::load<u16>(b + 0x5878) != 0x73 /* DASHKAZE */) {
            if (r28) {
                r30 = (s16)((r27 >> 1) - r24_4);
                r29 = (s16)((r23_3 >> 1) - r25_3);
                r4 = r30;
                r23 = r29;
            } else {
                r4 = (s16)(r27 - r24_4);
                r23 = (s16)(r23_3 - r25_3);
            }
        } else {
            if (r28) {
                r30 = (s16)(r27 - r24_4);
                r29 = (s16)(r23_3 - r25_3);
            }
            r4 = 0;
            r23 = 0;
        }
    } else if (mCurProc == 0x66 /* daPyProc_DAMAGE_e */) {
        r4 = m3564.x;
        r23 = m3564.y;
    } else if ((mCurProc == 0x3C /* daPyProc_LADDER_MOVE_e */ || mCurProc == 0x3F /* daPyProc_CLIMB_MOVE_UP_DOWN_e */) &&
               mDirection == 0 /* DIR_FORWARD */) {
        r4 = -0x1800;
        r23 = 0;
    } else if (mCurProc == 0x40 /* daPyProc_CLIMB_MOVE_SIDE_e */) {
        s16 v = mProcVar2;
        s32 av = v < 0 ? -v : v;
        if (av <= 0x4000) {
            r4 = lk_ftos(-6144.0f * cM_scos((u16)v));
        } else {
            r4 = 0;
        }
        r23 = lk_ftos(6144.0f * cM_ssin((u16)v));
    } else if (m34C3 == 1 && m3580 != 8) {
        r28 = FALSE;
        r4 = (s16)(m34E2 >> 1);
        if (r4 > 8000) {
            r4 = 8000;
        } else if (r4 < -10000) {
            r4 = -10000;
        }
        r23 = 0;
    } else {
        r28 = FALSE;
        r4 = 0;
        r23 = 0;
    }
    if (mCurProc == 0x8F /* daPyProc_SHIP_CRANE_e */) {
        r23 = 0;
        r29 = 0;
    }
    cLib_addCalcAngleS(&m3564.x, r4, 3, 0x1000, 0x100);
    cLib_addCalcAngleS(&m3564.y, r23, 3, 0x1000, 0x100);
    if (mModeFlg & 0x80) {
        s16 v = (s16)(r25_3 + m3564.y);
        if (v > 0x3800) {
            m3564.y = (s16)(0x3800 - r25_3);
        } else if (v < -0x3800) {
            m3564.y = (s16)(-0x3800 - r25_3);
        }
    }
    if (mCurProc != 0x66) {
        cLib_addCalcAngleS(&m3564.z, 0, 3, 0x1000, 0x100);
    }
    f32 f31v = 0.0f; /* eye 0 x */
    f32 f30v = 0.0f; /* eye 1 x */
    f32 f29v = 0.0f; /* eye y */
    bool eyes;
    s32 proc = mCurProc;
    if (r28 || proc == 0x93 /* daPyProc_FAN_GLIDE_e */ || proc == 0xA0 /* daPyProc_ICE_SLIP_ALMOST_FALL_e */ || proc == 0x40 ||
        m34C3 == 9 || ((proc == 0x74 /* GRAB_HEAVY_WAIT */ || proc == 0x73 /* GRAB_WAIT */ || proc == 4 /* WAIT */) && !(m35A0 == 0.0f))) {
        f32 f0;
        f32 f1;
        if (r28) {
            f0 = (f32)(s32)r30 * 0.00012207031f;
            f1 = (f32)(s32)r29 * 0.00012207031f;
        } else if (proc == 0x93) {
            gabi::Local<cXyz> sp58;
            cXyz_mi(&current.pos, sp58, &old.pos);
            s16 a = cM_atan2s(sp58->x, sp58->z);
            f1 = (f32)(s32)(-(s32)m34F4) / 6144.0f;
            f0 = cM_scos((u16)(a - shape_angle.y));
        } else if (proc == 0xA0) {
            u16 a = (u16)(current.angle.y - shape_angle.y);
            f0 = std::fabs(cM_scos(a));
            f1 = cM_ssin(a);
        } else if (proc == 0x40) {
            u16 a = (u16)mProcVar2;
            f0 = -cM_scos(a);
            f1 = cM_ssin(a);
        } else if (proc == 0x74 || proc == 0x73 || proc == 4) {
            f0 = 0.0f;
            f1 = m35A0;
        } else {
            s16 a0 = cM_atan2s(m36A0.x, m36A0.z);
            u16 a = (u16)(a0 - shape_angle.y);
            f0 = std::fabs(cM_scos(a));
            f1 = cM_ssin(a);
        }
        f32 f6 = std_sqrtf(gabi::fmadds(f1, f1, f0 * f0));
        if (f6 > 1.0f) {
            f32 f2 = 1.0f / f6;
            f1 = f1 * f2;
            f0 = f0 * f2;
        }
        f32 fy;
        if (f1 > 0.0f) {
            f31v = f1 * 0.12f;
            fy = fsel_l(-f0, -0.07f, -0.12f);
            f30v = f1 * -0.16f;
        } else {
            f31v = f1 * 0.16f;
            fy = fsel_l(-f0, -0.07f, -0.12f);
            f30v = f1 * -0.12f;
        }
        gabi::store<u8>(0x101CEF1B, 0); /* daPy_matAnm_c::setMorfFrame(0) */
        gabi::store<u8>(0x101CEF1A, 1); /* daPy_matAnm_c::onEyeMoveFlg() */
        f29v = f0 * fy;
        eyes = true;
    } else {
        s16 m = m351C;
        f32 f1 = (f32)(s32)m * 0.00024414062f; /* m351C / 4096.0f */
        u32 fl = noResetFlg1();
        if (f1 > 1.0f) {
            f1 = 1.0f;
        } else if (f1 < -1.0f) {
            f1 = -1.0f;
        }
        if (!(fl & 0x01000100) /* !CONFUSE && !UNK1000000 */ && m != 0) {
            if (m < 0) {
                f31v = -0.12f * f1;
                f30v = 0.16f * f1;
            } else {
                f31v = -0.16f * f1;
                f30v = 0.12f * f1;
            }
            gabi::store<u8>(0x101CEF1A, 1);
            eyes = true;
        } else {
            u32 e0 = gabi::load<u32>(b + 0x604); /* m_tex_eye_scroll[0..1]->setNowOffsetX/Y(0.0f) */
            gabi::store<f32>(e0 + 0x74, 0.0f);
            gabi::store<f32>(gabi::load<u32>(b + 0x608) + 0x74, 0.0f);
            gabi::store<f32>(gabi::load<u32>(b + 0x604) + 0x78, 0.0f);
            gabi::store<f32>(gabi::load<u32>(b + 0x608) + 0x78, 0.0f);
            if (gabi::load<u8>(0x101CEF1A) != 0) {
                gabi::store<u8>(0x101CEF1B, 3); /* setMorfFrame(3) */
            }
            gabi::store<u8>(0x101CEF1A, 0); /* offEyeMoveFlg() */
            eyes = false;
        }
        f29v = 0.0f;
    }
    if (eyes) {
        cLib_addCalc_l(gabi::at<be<f32>>(gabi::load<u32>(b + 0x604) + 0x74), f31v, 0.5f, 0.1f, 0.03f);
        cLib_addCalc_l(gabi::at<be<f32>>(gabi::load<u32>(b + 0x608) + 0x74), f30v, 0.5f, 0.1f, 0.03f);
        cLib_addCalc_l(gabi::at<be<f32>>(gabi::load<u32>(b + 0x604) + 0x78), f29v, 0.5f, 0.08f, 0.02f);
        u32 e0 = gabi::load<u32>(b + 0x604);
        u32 e1 = gabi::load<u32>(b + 0x608);
        gabi::store<f32>(e1 + 0x78, gabi::load<f32>(e0 + 0x78));
    }
}
VERIFY(0x02400C4C, &daPy_lk_c::setNeckAngle);

/* HD simpleAnmPlay of a btk with a frame callback: the frame (self + fo) steps by one and wraps at the
 * s16 frame count (self + mo); the frame is also stored through the pointer at self + po and the
 * callback object at self + cbo is updated */
static inline void lk_btkStep(u32 self, u32 fo, u32 mo, u32 po, u32 cbo, f32 frame) {
    f32 mx = (f32)gabi::load<s16>(self + mo);
    f32 f = gabi::fadds_ppc(frame, 1.0f);
    f = fsel_l(gabi::fsubs_ppc(f, mx), 0.0f, f);
    u32 anm = gabi::load<u32>(self + po);
    gabi::store<f32>(self + fo, f);
    gabi::store<f32>(anm, f);
    lk_hdAnmFrame(self, cbo, f);
}
/* the sword grip's brk frame (+0xD94, through +0xDA0, callback object +0xDB0) after its bck entry */
static inline void lk_swordGripAnm(u32 self, f32 bckFrame, f32 brkFrame) {
    u32 md = gabi::load<u32>(gabi::load<u32>(self + 0xD00) + 0xAC);
    gabi::call(0x025E86B8 /* mDoExt_bckAnm::entry */, self + 0xD04, md, bckFrame);
    u32 anm = gabi::load<u32>(self + 0xDA0);
    gabi::store<f32>(self + 0xD94, brkFrame);
    gabi::store<f32>(anm, brkFrame);
    lk_hdAnmFrame(self, 0xDB0, brkFrame);
}
/* show (1) or hide (0) the blade joint's mesh of the sword model (joint looked up by name, HD) */
static inline void lk_swordBladeShow(u32 model, u8 show) {
    gabi::Local<lk_SafeString_l> nm;
    nm->__vtbl = LK_SAFESTRING_VTBL;
    nm->mStr = 0x10035A28;
    u32 md = gabi::load<u32>(model + 0xAC);
    u32 res = gabi::load<u32>(md);
    gabi::call(0x02444F48, nm.get());
    u32 off = gabi::load<u32>(res + 0x18);
    u32 tbl = off != 0 ? res + 0x18 + off : 0;
    s32 idx = gabi::call<s32>(0x027DF9B0, tbl, (u32)nm->mStr);
    u32 node;
    if (idx < 0) {
        node = 0;
    } else {
        u32 n = gabi::load<u32>(md + 0xC);
        u32 base = gabi::load<u32>(md + 0x10);
        node = (u32)idx < n ? base + (u32)idx * 0x39C : base;
    }
    if (node != 0) {
        gabi::store<u8>(gabi::load<u32>(node + 8) + 4, show);
    }
}
/* HD: the stage name (0x1047E6B8) compared with a literal (SafeString ==, the first operand made
 * terminated by 02444F48) */
static inline bool lk_stageIs_b(u32 lit) {
    gabi::Local<lk_SafeString_l> a;
    gabi::Local<lk_SafeString_l> b;
    a->__vtbl = LK_SAFESTRING_VTBL;
    b->__vtbl = LK_SAFESTRING_VTBL;
    b->mStr = 0x1047E6B8;
    a->mStr = lit;
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
static inline void lk_XYZrotM(s16 x, s16 y, s16 z) {
    gabi::call(0x025F19F8 /* mDoMtx_XYZrotM */, mDoMtx_stack_c::get(), (s32)x, (s32)y, (s32)z);
}
static inline void lk_stackRevConcat(u32 m) {
    gabi::call(0x028E9108 /* PSMTXConcat */, m, mDoMtx_stack_c::get(), mDoMtx_stack_c::get());
}

/* 02403C24 (HD: the hammer (0x33) swings with a decaying timer (+0x69EC) around the angle at +0x69EA;
 * in proc 0xD8 the sword follows the item model's root joint and the shield Link's base matrix; while a
 * demo runs in the stage at 0x10035A20 the sword counts as the normal one; the blade joint is found by
 * name) */
s32 daPy_lk_c::setItemModel() {
    WWHD_FUNC(0x02403C24, s32, this);
    u32 b = gabi::ea(this);
    u32 pod = gabi::ea(lk_getAnmMtx(mpCLModel, 13 /* CL_JNT_CL_PODA_e */));
    u32 lHand = gabi::ea(lk_getAnmMtx(mpCLModel, 8 /* CL_JNT_CL_LHANDA_e */));
    u32 rHand = gabi::ea(lk_getAnmMtx(mpCLModel, 12 /* CL_JNT_CL_RHANDA_e */));
    u32 stk = gabi::ea(mDoMtx_stack_c::get());
    u32 boko = LK_FIELD(u32, 0x6594); /* mActorKeepEquip.getActor() */
    if (boko != 0 && LK_FIELD(u16, 0x69B0) == 0x101 /* daPyItem_BOKO_e */) {
        PSMTXTrans(mDoMtx_stack_c::get(), 40.0f, 47.0f, 2.1f);
        lk_XYZrotM(0x3E38, -0x5D4, 0x66F8);
        u32 prm = gabi::load<u32>(boko + 0xB0);
        if (prm == 2 || prm == 3) {
            mDoMtx_stack_transM(0.0f, 0.0f, 20.0f);
        } else if (prm == 4) {
            mDoMtx_stack_transM(0.0f, 0.0f, 70.0f);
        } else if (prm == 5) {
            mDoMtx_stack_transM(0.0f, 0.0f, 30.0f);
        }
        lk_stackRevConcat(lHand);
        u32 bm = gabi::load<u32>(boko + 0x3B4); /* boko->setMatrix() */
        if (bm != 0) {
            lk_setBaseTRMtx(bm, stk);
        }
    }
    if (LK_FIELD(u32, 0x4878) != 0) { /* mpEquipItemBtk */
        f32 f = LK_FIELD(f32, 0x4814);
        if (f > 0.0f || cM_rnd() < 0.02f) {
            lk_btkStep(b, 0x4814, 0x481A, 0x4878, 0x4820, f);
        }
    }
    lk_setBaseTRMtx(LK_FIELD(u32, 0xE7C) /* mpPodmsModel */, pod);
    u32 model = LK_FIELD(u32, 0x4440); /* mpEquipItemModel */
    if (model != 0) {
        u32 item = LK_FIELD(u16, 0x69B0);
        bool lhand = false, bow = false, after = false; /* after: the bottle / hookshot / wind waker checks */
        if (item == 0x10A) {
            PSMTXTrans(mDoMtx_stack_c::get(), 50.0f, -48.0f, -14.0f);
            lk_XYZrotM(-0x33FC, -0x2CC, 0x3519);
            u32 ship = gabi::load<u32>(dComIfGp_ea() + 0x5B3C);
            u32 tact = gabi::load<u32>(gabi::load<u32>(ship + 0x3B8) + 0x90); /* getTactJntMtx() */
            lk_stackRevConcat(gabi::ea(lk_getAnmMtx(gabi::at<J3DModel>(tact), 10)));
            lk_setBaseTRMtx(LK_FIELD(u32, 0x4440), stk);
        } else if (item == 0x104) {
            PSMTXTrans(mDoMtx_stack_c::get(), 14.5f, 34.5f, 0.0f);
            lk_XYZrotM(0, -0x4000, -0x2AAA);
            lk_stackRevConcat(gabi::ea(lk_getAnmMtx(mpCLModel, 3 /* CL_JNT_STOMACH_JNT_e */)));
            lk_setBaseTRMtx(LK_FIELD(u32, 0x4440), stk);
        } else if (item == 0x20 /* dItemNo_TELESCOPE_e */) {
            PSMTXTrans(mDoMtx_stack_c::get(), 8.0f, 15.0f, 1.0f);
            lk_XYZrotM(-0x4000, 0x4444, -0x41F);
            lk_stackRevConcat(lHand);
            lk_setBaseTRMtx(LK_FIELD(u32, 0x4440), stk);
        } else if (item == 0x83 /* dItemNo_HYOI_PEAR_e */) {
            if (LK_FIELD(f32, 0x589C) < 9.0f) { /* mFrameCtrlUnder[UNDER_MOVE0_e].getFrame() */
                PSMTXTrans(mDoMtx_stack_c::get(), -1.5f, -44.0f, 16.0f);
                lk_XYZrotM(-0x6EEE, 0x9BE, 0x368A);
                lk_stackRevConcat(lHand);
                lk_setBaseTRMtx(LK_FIELD(u32, 0x4440), stk);
            } else {
                lk_setBaseTRMtx(model, gabi::ea(lk_getAnmMtx(mpCLModel, 15 /* CL_JNT_HEAD_JNT_e */)));
            }
        } else if (item == 0x33 /* dItemNo_SKULL_HAMMER_e */ && LK_FIELD(s16, 0x69EC) != 0) {
            s16 t = (s16)(LK_FIELD(s16, 0x69EC) - 1);
            u32 ang = (u16)((s32)t * 0x61A8);
            LK_FIELD(s16, 0x69EC) = t;
            f32 amp = (f32)t * 300.0f;
            f32 s = gabi::load<f32>(0x104A44F8 + (ang >> 3) * 8);
            s16 r = (s16)(LK_FIELD(s16, 0x69EA) + lk_ftos(s * amp));
            gabi::call(0x025F1954 /* mDoMtx_XYZrotS */, mDoMtx_stack_c::get(), 0, 0, (s32)r);
            lk_stackRevConcat(lHand);
            lk_setBaseTRMtx(LK_FIELD(u32, 0x4440), stk);
            item = LK_FIELD(u16, 0x69B0);
            after = true;
        } else if (item == 0x33 || item == 0x21 || item == 0x22 || item == 0x2F || item == 0x34 || item == 0x103 ||
                   checkBottleItem(item) || checkPhotoBoxItem(item)) {
            lhand = true;
        } else {
            bow = true;
        }
        if (lhand) {
            lk_setBaseTRMtx(model, lHand);
            item = LK_FIELD(u16, 0x69B0);
            after = true;
        }
        if (after) {
            bool done = false;
            if (item == 0x34 /* dItemNo_DEKU_LEAF_e */) {
                s32 proc = LK_FIELD(s32, 0x65F0);
                if (proc != 0x92 && proc != 0x93) {
                    gabi::call(0x023E771C /* setShapeFanLeaf */, b);
                    done = true;
                }
            }
            if (done) {
            } else if (checkBottleItem(item)) {
                u32 bc = LK_FIELD(u32, 0x4970); /* mpBottleContentsModel */
                if (bc != 0) {
                    if (item == 0x58 /* dItemNo_FIREFLY_BOTTLE_e */) {
                        s16 mx = LK_FIELD(s16, 0x4902);
                        f32 fr = LK_FIELD(f32, 0x48FC);
                        f32 a = (37.699112f * fr) / (f32)mx;
                        u32 ang = (u16)gabi::call<s16>(0x02019510 /* cM_rad2s */, a);
                        f32 sc = gabi::fmadds(0.1f, gabi::load<f32>(0x104A44F8 + (ang >> 3) * 8), 1.0f);
                        PSMTXTrans(mDoMtx_stack_c::get(), 11.0f, 0.0f, 0.0f);
                        lk_stackRevConcat(lHand);
                        lk_setBaseTRMtx(LK_FIELD(u32, 0x4970), stk);
                        u32 bc2 = LK_FIELD(u32, 0x4970); /* setBaseScale */
                        gabi::store<f32>(bc2 + 0xC0, sc);
                        gabi::store<f32>(bc2 + 0xBC, sc);
                        gabi::store<f32>(bc2 + 0xC4, sc);
                        gabi::call(0x025E742C /* mDoExt_baseAnm::play */, b + 0x48F8);
                        u32 md = gabi::load<u32>(LK_FIELD(u32, 0x4970) + 0xAC);
                        gabi::call(0x025E83FC /* mDoExt_brkAnm::entry */, b + 0x48F8, md, (f32)LK_FIELD(f32, 0x48FC));
                    } else {
                        lk_setBaseTRMtx(bc, lHand);
                    }
                }
                u32 cap = LK_FIELD(u32, 0x4974); /* mpBottleCapModel */
                if (cap != 0) {
                    lk_setBaseTRMtx(cap, lHand);
                }
            } else if (item == 0x2F /* dItemNo_HOOKSHOT_e */) {
                if (boko != 0) {
                    u32 prm = gabi::load<u32>(boko + 0xB0);
                    if (prm != 0) {
                        f32 v = LK_FIELD(f32, 0x6A44); /* m35EC */
                        u32 bck = LK_FIELD(u32, 0x44CC);
                        LK_FIELD(f32, 0x6A44) = prm == 1 ? gabi::fadds_ppc(v, 1.0f) : gabi::fsubs_ppc(v, 1.0f);
                        s32 n = gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(bck + 4) + 0x14), bck); /* getFrameMax() */
                        f32 mx = (f32)n;
                        f32 c = LK_FIELD(f32, 0x6A44);
                        if (c < 0.0f) {
                            LK_FIELD(f32, 0x6A44) = gabi::fadds_ppc(c, mx);
                        } else if (!(c < mx)) {
                            LK_FIELD(f32, 0x6A44) = gabi::fsubs_ppc(c, mx);
                        }
                    }
                }
            } else if (item == 0x22 /* dItemNo_WIND_WAKER_e */) {
                if (LK_FIELD(s32, 0x65F0) != 0x9A && LK_FIELD(u32, 0x4908) != 0) {
                    f32 f = gabi::fsubs_ppc(LK_FIELD(f32, 0x48FC), 1.0f);
                    f = fsel_l(f, f, 0.0f);
                    u32 brk = LK_FIELD(u32, 0x4908);
                    LK_FIELD(f32, 0x48FC) = f;
                    gabi::store<f32>(brk, f);
                    lk_hdAnmFrame(b, 0x4918, f);
                }
            }
        }
        if (bow) {
            if (checkBowItem(item)) {
                lk_setBaseTRMtx(model, rHand);
            } else if (item == 0x102) {
                lk_setBaseTRMtx(model, gabi::ea(lk_getAnmMtx(mpCLModel, 4 /* CL_JNT_CHEST_JNT_e */)));
            }
        }
        if (LK_FIELD(u32, 0x44CC) != 0 && LK_FIELD(s32, 0x65F0) != 0xB7) { /* mSwordAnim.getBckAnm() */
            gabi::call(0x025E86B8 /* mDoExt_bckAnm::entry */, b + 0x4444, gabi::load<u32>(LK_FIELD(u32, 0x4440) + 0xAC),
                       (f32)LK_FIELD(f32, 0x6A44));
            u32 s1 = LK_FIELD(u32, 0x4978); /* mpSwordModel1 */
            if (s1 != 0) {
                gabi::call(0x025E86B8 /* mDoExt_bckAnm::entry */, b + 0x4444, gabi::load<u32>(s1 + 0xAC), (f32)LK_FIELD(f32, 0x6A44));
            }
        }
        u32 morf = LK_FIELD(u32, 0x44D0); /* mpParachuteFanMorf */
        if (morf != 0) {
            gabi::call(0x025E55A0 /* mDoExt_McaMorf::calc */, morf);
        } else {
            gabi::call(0x027F4D5C /* J3DModel::calc */, (u32)LK_FIELD(u32, 0x4440));
        }
    }
    /* the sword */
    LK_FIELD(u32, 0xCF8) = LK_FIELD(u32, 0xD00); /* mpEquippedSwordModel = mpSwgripmsModel */
    s32 swType;
    s32 grip; /* -1: the normal sword; else the bck frame / brk frame choice below */
    u8 sel = lk_selectSword();
    if (sel == 0xFF) {
        if (gabi::load<u8>(dComIfGp_ea() + 0x5CEA) != 2) {
            grip = 0; /* no sword */
        } else if (lk_selectSword() == 0x38 || gabi::load<u8>(dComIfGp_ea() + 0x5CEA) == 2) {
            grip = -1;
        } else {
            grip = 1;
        }
    } else if (sel == 0x38 || gabi::load<u8>(dComIfGp_ea() + 0x5CEA) == 2) {
        grip = -1;
    } else {
        grip = 1;
    }
    if (grip == 1 && LK_FIELD(u16, 0x420) != 0 && lk_stageIs_b(0x10035A20)) {
        grip = -1;
    }
    f32 f27;
    if (grip == -1) { /* checkNormalSwordEquip() */
        f27 = LK_FIELD(f32, 0xE0C);
        LK_FIELD(u32, 0xCF8) = LK_FIELD(u32, 0xCFC); /* mpSwgripaModel */
        swType = 1;
    } else if (grip == 0) {
        swType = 0;
        lk_swordGripAnm(b, 0.0f, 0.0f);
        f27 = LK_FIELD(f32, 0xE0C);
    } else {
        u8 s = lk_selectSword();
        if (s == 0x3E) { /* checkFinalMasterSwordEquip() */
            swType = 3;
            lk_swordGripAnm(b, 1.0f, 1.0f);
        } else if (s == 0x3A) { /* dItemNo_MASTER_SWORD_2_e */
            swType = 4;
            lk_swordGripAnm(b, 1.0f, 0.0f);
        } else {
            swType = 2;
            lk_swordGripAnm(b, 0.0f, 0.0f);
        }
        u32 m = LK_FIELD(u32, 0x4440);
        if (m != 0 && LK_FIELD(u16, 0x69B0) == 0x103) {
            lk_swordBladeShow(m, swType == 3 ? 1 : 0);
        }
        f27 = LK_FIELD(f32, 0xE0C);
    }
    if (f27 > 0.1f || cM_rnd() < 0.02f) { /* mpTswgripmsBtk */
        lk_btkStep(b, 0xE0C, 0xE12, 0xE70, 0xE18, f27);
    }
    if (LK_FIELD(u16, 0x69B0) == 0x103 /* daPyItem_SWORD_e */) {
        s32 proc = LK_FIELD(s32, 0x65F0);
        u32 sm = LK_FIELD(u32, 0xCF8);
        if (proc == 0xD8) {
            lk_setBaseTRMtx(sm, gabi::ea(lk_getAnmMtx(gabi::at<J3DModel>(LK_FIELD(u32, 0x4440)), 0)));
        } else {
            lk_setBaseTRMtx(sm, lHand);
        }
        gabi::call(0x025E1E14 /* mDoAud_setLinkSwordType */, swType, 1);
    } else {
        PSMTXTrans(mDoMtx_stack_c::get(), -11.25f, 4.5f, 0.45f);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), -0x1C71);
        lk_stackRevConcat(pod);
        lk_setBaseTRMtx(LK_FIELD(u32, 0xCF8), stk);
        gabi::call(0x025E1E14 /* mDoAud_setLinkSwordType */, swType, 2);
    }
    gabi::call(0x027F4D5C /* J3DModel::calc */, (u32)LK_FIELD(u32, 0xCF8));
    /* the shield */
    s32 shType;
    u8 shield = gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x2F);
    if (shield == 0x3C) { /* checkMirrorShieldEquip() */
        u32 btk = LK_FIELD(u32, 0xF80); /* mpTshmsBtk */
        LK_FIELD(u32, 0xE80) = LK_FIELD(u32, 0xE88); /* mpEquippedShieldModel = mpShmsModel */
        shType = 2;
        if (btk != 0) {
            f32 f = LK_FIELD(f32, 0xF1C);
            if (f > 0.0f || cM_rnd() < 0.02f) {
                lk_btkStep(b, 0xF1C, 0xF22, 0xF80, 0xF28, f);
            }
        }
    } else {
        LK_FIELD(u32, 0xE80) = LK_FIELD(u32, 0xE84); /* mpShaModel */
        shType = shield == 0xFF ? 0 : 1;
    }
    s32 proc = LK_FIELD(s32, 0x65F0);
    s32 place; /* 0: Link's base matrix (HD proc 0xD8), 1: right hand, 2: on the back */
    if (proc == 0xD8) {
        place = 0;
    } else if (proc == 0xA9 /* daPyProc_DEMO_TOOL_e */ ? LK_FIELD(s16, 0x691A) == 1 : LK_FIELD(u16, 0x69B0) == 0x103) {
        place = 1;
    } else if (gabi::call_ptr<BOOL>(gabi::load<u32>(__vtbl + 0x3C), b) /* checkPlayerGuard() */ && LK_FIELD(u16, 0x69B0) != 0x33) {
        place = 1;
    } else if (LK_FIELD(s32, 0x65F0) == 0x65 /* daPyProc_GUARD_CRASH_e */) {
        place = 1;
    } else {
        place = 2;
    }
    if (place == 0) {
        u32 cl = LK_FIELD(u32, 0x448);
        u32 sh = LK_FIELD(u32, 0xE80);
        lk_setBaseTRMtx(sh, cl != 0 ? cl + 0xC8 : 0);
        gabi::call(0x025E1E2C /* mDoAud_setLinkShieldType */, shType, 1);
    } else if (place == 1) {
        lk_setBaseTRMtx(LK_FIELD(u32, 0xE80), rHand);
        gabi::call(0x025E1E2C /* mDoAud_setLinkShieldType */, shType, 1);
    } else {
        PSMTXTrans(mDoMtx_stack_c::get(), 15.5f, 4.75f, shType == 2 ? -0.2f : 0.0f);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), -0x4000);
        lk_stackRevConcat(pod);
        lk_setBaseTRMtx(LK_FIELD(u32, 0xE80), stk);
        u32 idx = LK_FIELD(u16, 0x5848); /* m_anm_heap_under[UNDER_MOVE0_e].mIdx */
        if (shType == 2 && (idx == 0x70 || idx == 0x74 || idx == 0x12E || idx == 0x135)) {
            f32 fr = LK_FIELD(f32, 0x589C);
            s16 end = LK_FIELD(s16, 0x58A2);
            f32 a = (6.2831855f * gabi::fsubs_ppc(fr, 22.0f)) / (f32)end;
            /* mDoMtx_stack_c::multVecZero(&temp) */
            u32 t0 = lk_fbits(gabi::load<f32>(stk + 0xC));
            u32 t1 = lk_fbits(gabi::load<f32>(stk + 0x1C));
            u32 t2 = lk_fbits(gabi::load<f32>(stk + 0x2C));
            u32 ang = (u16)gabi::call<s16>(0x02019510 /* cM_rad2s */, a);
            f32 r = LK_FIELD(f32, 0x6A14) / LK_FIELD(f32, 0x3C4); /* mNormalSpeed / mMaxNormalSpeed */
            f32 y = (-5500.0f * fabsf(r)) * gabi::load<f32>(0x104A44F8 + (ang >> 3) * 8);
            gabi::call(0x025F1884 /* mDoMtx_YrotS */, mDoMtx_stack_c::get(), (s32)lk_ftos(y));
            u32 sh = LK_FIELD(u32, 0xE80);
            gabi::call(0x028E9108 /* PSMTXConcat */, mDoMtx_stack_c::get(), sh != 0 ? sh + 0xC8 : 0, mDoMtx_stack_c::get());
            gmem_stf32(stk + 0xC, t0);
            gmem_stf32(stk + 0x1C, t1);
            gmem_stf32(stk + 0x2C, t2);
            lk_setBaseTRMtx(LK_FIELD(u32, 0xE80), stk);
        }
        gabi::call(0x025E1E2C /* mDoAud_setLinkShieldType */, shType, 2);
    }
    gabi::call(0x025E86B8 /* mDoExt_bckAnm::entry */, b + 0xE8C, gabi::load<u32>(LK_FIELD(u32, 0xE80) + 0xAC), (f32)LK_FIELD(f32, 0x6A40));
    gabi::call(0x027F4D5C /* J3DModel::calc */, (u32)LK_FIELD(u32, 0xE80));
    {
        u32 sh = LK_FIELD(u32, 0xE80);
        u32 ym = LK_FIELD(u32, 0x43B4); /* mpYmsls00Model */
        lk_setBaseTRMtx(ym, sh != 0 ? sh + 0xC8 : 0);
    }
    gabi::call(0x025E742C /* mDoExt_baseAnm::play */, b + 0x43B8); /* simpleAnmPlay(mpYmsls00Btk) */
    {
        u32 anm = LK_FIELD(u32, 0x4420);
        f32 f = LK_FIELD(f32, 0x43BC);
        gabi::store<f32>(anm, f);
        lk_hdAnmFrame(b, 0x43C8, f);
    }
    if (LK_FIELD(u32, 0x3B8) & 0x02000000) { /* checkEquipHeavyBoots() */
        gabi::call(0x024038A0 /* setBootsModel */, b, b + 0x442C);
        gabi::call(0x025E1E04 /* mDoAud_setLinkBootsType */, 1);
    } else {
        gabi::call(0x025E1E04 /* mDoAud_setLinkBootsType */, 0);
    }
    if (gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x30) == 0x28) { /* checkPowerGloveEquip() */
        lk_copyJntMtx(mpCLModel, 7 /* CL_JNT_LARMB_JNT_e */, gabi::at<J3DModel>(LK_FIELD(u32, 0x4434)), 1);
        lk_copyJntMtx(mpCLModel, 11 /* CL_JNT_RARMB_JNT_e */, gabi::at<J3DModel>(LK_FIELD(u32, 0x4434)), 2);
    }
    return 0;
}
VERIFY(0x02403C24, &daPy_lk_c::setItemModel);

/* HD: a GXColor component gamma-corrected once (lk_colorGamma 02406D60, value * 255 truncated) */
static inline void lk_gammaByte(u32 dst, u32 v) {
    f32 g = (f32)gabi::call<f64>(0x02406D60, v);
    gabi::store<u8>(dst, (u8)gabi::ftoi(g * 255.0f));
}
/* HD: the cut-turn colours are function-local statics corrected at first use (guard word, then the
 * components that are not 0xFF) */
static inline void lk_cutColorStatics() {
    struct C {
        u32 guard;
        u8 n;
        u32 addr[3];
        u8 v[3];
    };
    static const C tbl[] = {
        {0x1046D16C, 2, {0x101CEB00, 0x101CEB02}, {0xC8, 0x40}},
        {0x1046D170, 1, {0x101CEB06}, {0xDC}},
        {0x1046D174, 2, {0x101CEB08, 0x101CEB0A}, {0xC8, 0x78}},
        {0x1046D178, 2, {0x101CEB0C, 0x101CEB0E}, {0xC8, 0x40}},
        {0x1046D17C, 1, {0x101CEB12}, {0xDC}},
        {0x1046D180, 2, {0x101CEB14, 0x101CEB16}, {0xC8, 0x78}},
        {0x1046D184, 2, {0x101CEB18, 0x101CEB1A}, {0xC8, 0x40}},
        {0x1046D188, 1, {0x101CEB1E}, {0xDC}},
        {0x1046D18C, 2, {0x101CEB20, 0x101CEB22}, {0xC8, 0x78}},
        {0x1046D190, 3, {0x101CEB24, 0x101CEB25, 0x101CEB26}, {0x40, 0x60, 0x40}},
        {0x1046D194, 3, {0x101CEB28, 0x101CEB29, 0x101CEB2A}, {0xDC, 0xE6, 0xDC}},
        {0x1046D198, 3, {0x101CEB2C, 0x101CEB2D, 0x101CEB2E}, {0xA0, 0xE6, 0xA0}},
        {0x1046D19C, 3, {0x101CEB30, 0x101CEB31, 0x101CEB32}, {0x40, 0x40, 0x60}},
        {0x1046D1A0, 3, {0x101CEB34, 0x101CEB35, 0x101CEB36}, {0xDC, 0xDC, 0xE6}},
        {0x1046D1A4, 3, {0x101CEB38, 0x101CEB39, 0x101CEB3A}, {0xA0, 0xA0, 0xE6}},
        {0x1046D1A8, 2, {0x101CEB3C, 0x101CEB3D}, {0x40, 0x40}},
        {0x1046D1AC, 2, {0x101CEB40, 0x101CEB41}, {0xDC, 0xDC}},
        {0x1046D1B0, 2, {0x101CEB44, 0x101CEB45}, {0x78, 0x78}},
    };
    for (u32 i = 0; i < sizeof(tbl) / sizeof(tbl[0]); i++) {
        if (gabi::load<u32>(tbl[i].guard) == 0) {
            gabi::store<u32>(tbl[i].guard, 1);
            for (u32 k = 0; k < tbl[i].n; k++) {
                lk_gammaByte(tbl[i].addr[k], tbl[i].v[k]);
            }
        }
    }
}
/* dComIfGp_particle_setP1(id, &current.pos, &shape_angle, NULL, 0xFF, cb, roomNo, prm, env) */
static inline u32 lk_cutTurnEmitter(u32 self, u32 id, u32 cb, u32 prm, u32 env) {
    s8 room = gabi::load<s8>(self + 0x326);
    u32 pa = gabi::load<u32>(dComIfGp_ea() + 0x5AB0);
    return gabi::call<u32>(0x025A847C, pa, 1, id, self + 0x314, self + 0x328, 0, 0xFF, cb, (s32)room, prm, env, 0);
}
static inline void lk_ccsSet(u32 obj) { gabi::call(0x0200E240 /* cCcS::Set */, dComIfGp_ea() + 0x26A4, obj); }
static inline void lk_ccsSetMass(u32 obj) { gabi::call(0x02516C14 /* dCcMassS_Mng::Set */, dComIfGp_ea() + 0x4EF8, obj, 1); }
/* mWindCyl / mLightCyl follow mCyl */
static inline void lk_followCyl(u32 self, u32 obj, u32 c) {
    gabi::call(0x020182E0 /* cM3dGCyl::SetC */, obj + 0x118, c);
    gabi::call(0x02018428 /* cM3dGCyl::SetH */, obj + 0x118, (f32)gabi::load<f32>(self + 0x7784));
    gabi::call(0x020184DC /* cM3dGCyl::SetR */, obj + 0x118, (f32)gabi::load<f32>(self + 0x7780));
    lk_ccsSet(obj);
}
/* an HD frame-callback anm frame set: frame at self + fo, pointer at self + po, callback at self + cbo */
static inline void lk_anmSetFrame(u32 self, u32 fo, u32 po, u32 cbo, f32 f) {
    u32 anm = gabi::load<u32>(self + po);
    gabi::store<f32>(self + fo, f);
    gabi::store<f32>(anm, f);
    lk_hdAnmFrame(self, cbo, f);
}
/* mYuchw00Bck.play(); entry(model data, frame); simpleAnmPlay(mpYuchw00Btk) */
static inline void lk_yuchwPlay(u32 self) {
    gabi::call(0x025E742C /* mDoExt_baseAnm::play */, self + 0x53F4);
    gabi::call(0x025E86B8 /* mDoExt_bckAnm::entry */, self + 0x53F4, gabi::load<u32>(gabi::load<u32>(self + 0x53F0) + 0xAC),
               (f32)gabi::load<f32>(self + 0x53F8));
    gabi::call(0x025E742C /* mDoExt_baseAnm::play */, self + 0x5480);
    u32 anm = gabi::load<u32>(self + 0x54E8);
    f32 f = gabi::load<f32>(self + 0x5484);
    gabi::store<f32>(anm, f);
    lk_hdAnmFrame(self, 0x5490, f);
}

/* 024076CC (HD: the grab-wear radius test reads +0x3CC; the cut turn/roll widths and colours are
 * constants and the colours are gamma corrected once; the damage test adds procs 0x6D/0x0D/0x97; the
 * roll trail turns by a constant 0x578) */
void daPy_lk_c::setCollision() {
    WWHD_FUNC(0x024076CC, void, this);
    u32 b = gabi::ea(this);
    u32 root = gabi::ea(lk_getAnmMtx(mpCLModel, 0 /* CL_JNT_LINK_ROOT_e */));
    u32 neck = gabi::ea(lk_getAnmMtx(mpCLModel, 14 /* CL_JNT_NECK_JNT_e */));
    u32 ltoe = gabi::ea(lk_getAnmMtx(mpCLModel, 35 /* CL_JNT_LTOE_JNT_e */));
    u32 rtoe = gabi::ea(lk_getAnmMtx(mpCLModel, 40 /* CL_JNT_RTOE_JNT_e */));
    u32 stk = gabi::ea(mDoMtx_stack_c::get());
    gabi::Local<cXyz> spD0;
    u32 c = gabi::ea(spD0.get());
    {
        f32 x = gabi::fmuls_ppc(gabi::fadds_ppc(gabi::load<f32>(root + 0xC), gabi::load<f32>(neck + 0xC)), 0.5f);
        gabi::store<f32>(c + 0, x);
        f32 z = gabi::fmuls_ppc(gabi::fadds_ppc(gabi::load<f32>(root + 0x2C), gabi::load<f32>(neck + 0x2C)), 0.5f);
        if (LK_FIELD(s32, 0x65F0) == 0x5B /* daPyProc_JUMP_CUT_e */) {
            u32 i = (u32)LK_FIELD(u16, 0x32A) >> 3;
            gabi::store<f32>(c + 0, gabi::fnmsubs(15.0f, gabi::load<f32>(0x104A44F8 + i * 8), x));
            z = gabi::fnmsubs(15.0f, gabi::load<f32>(0x104A44F8 + i * 8 + 4), z);
        }
        gabi::store<f32>(c + 8, z);
    }
    u32 cyl = b + 0x7774;
    gabi::call(0x020184DC /* cM3dGCyl::SetR */, cyl, fsel_l(LK_FIELD(f32, 0x3CC), 30.0f, 50.0f)); /* checkGrabWear() ? 50 : 30 */
    u32 mode = LK_FIELD(u32, 0x6A70);
    if (mode & 0x2000) { /* ModeFlg_IN_SHIP: mCyl.OnCoSPrmBit(NoCrr) */
        LK_FIELD(u32, 0x7688) = LK_FIELD(u32, 0x7688) | 0x100;
    } else {
        LK_FIELD(u32, 0x7688) = LK_FIELD(u32, 0x7688) & ~0x100u;
    }
    s32 proc;
    if (mode & 0x01000000) { /* ModeFlg_CRAWL */
        f32 s = gabi::load<f32>(0x104A44F8 + ((u32)LK_FIELD(u16, 0x328) >> 3) * 8);
        f32 py = LK_FIELD(f32, 0x318);
        if (!(s < 0.0f)) {
            gabi::store<f32>(c + 4, gabi::fnmsubs(50.0f, s, py));
            gabi::call(0x02018428 /* cM3dGCyl::SetH */, cyl, gabi::fmadds(s, 35.0f, 50.0f));
        } else {
            gabi::store<f32>(c + 4, gabi::fmadds(25.0f, s, py));
            gabi::call(0x02018428 /* cM3dGCyl::SetH */, cyl, gabi::fnmsubs(s, 65.0f, 50.0f));
        }
    } else if ((proc = LK_FIELD(s32, 0x65F0)) == 0x1E /* daPyProc_FRONT_ROLL_e */) {
        gabi::store<f32>(c + 4, LK_FIELD(f32, 0x318));
        gabi::call(0x02018428 /* cM3dGCyl::SetH */, cyl, 81.25f);
    } else if (proc == 0x22 /* daPyProc_BACK_JUMP_e */) {
        gabi::store<f32>(c + 4, gabi::fadds_ppc(LK_FIELD(f32, 0x318), 30.0f));
        gabi::call(0x02018428 /* cM3dGCyl::SetH */, cyl, 81.25f);
    } else if (proc == 0x5E || (proc == 0x5D && !(LK_FIELD(f32, 0x340) > 0.0f))) { /* BT_JUMP_CUT / BT_JUMP */
        gabi::store<f32>(c + 4, gabi::load<f32>(root + 0x1C));
        f32 d = gabi::fsubs_ppc(gabi::load<f32>(root + 0x1C), LK_FIELD(f32, 0x318));
        gabi::call(0x02018428 /* cM3dGCyl::SetH */, cyl, gabi::fsubs_ppc(125.0f, d));
        gabi::call(0x020184DC /* cM3dGCyl::SetR */, cyl, 22.5f);
    } else {
        f32 l = gabi::load<f32>(ltoe + 0x1C);
        f32 o = proc == 0x78 /* daPyProc_ROPE_SWING_e */ ? gabi::load<f32>(root + 0x1C) : gabi::load<f32>(rtoe + 0x1C);
        gabi::store<f32>(c + 4, fsel_l(gabi::fsubs_ppc(l, o), o, l));
        f32 ny = gabi::load<f32>(gabi::ea(lk_getAnmMtx(mpCLModel, 14)) + 0x1C);
        gabi::call(0x02018428 /* cM3dGCyl::SetH */, cyl, gabi::fadds_ppc(gabi::fsubs_ppc(ny, gabi::load<f32>(c + 4)), 40.1f));
    }
    gabi::call(0x020182E0 /* cM3dGCyl::SetC */, cyl, c);
    proc = LK_FIELD(s32, 0x65F0);
    if ((LK_FIELD(u32, 0x6A70) & 8) || proc == 0x6D || proc == 0xD || proc == 0x97 || LK_FIELD(s16, 0x3B0) != 0) {
        LK_FIELD(u32, 0x7674) = LK_FIELD(u32, 0x7674) & ~1u; /* mCyl.OffTgSetBit() */
        gabi::call(0x0251621C /* dCcD_GObjInf::ClrTgHit */, b + 0x765C);
    } else {
        LK_FIELD(u32, 0x7674) = LK_FIELD(u32, 0x7674) | 1; /* OnTgSetBit() */
    }
    lk_ccsSet(b + 0x765C);
    lk_ccsSetMass(b + 0x765C);
    lk_followCyl(b, b + 0x778C, c); /* mWindCyl */
    lk_followCyl(b, b + 0x79EC, c); /* mLightCyl */
    lk_cutColorStatics();
    gabi::Local<cXyz> sp98; /* spB8 */
    u32 rflg = LK_FIELD(u32, 0x3C0);
    if (rflg & 2) { /* daPyRFlg0_UNK2 */
        if (rflg & 1) { /* daPyRFlg0_UNK1 */
            gabi::call(0x02406DE0 /* setCutWaterSplash */, b);
            proc = LK_FIELD(s32, 0x65F0);
            if (proc == 0x55 || proc == 0x56) {
                gabi::call(0x0251655C /* dCcD_Cyl::StartCAt */, b + 0x78BC, b + 0x314);
            } else {
                gabi::call(0x02406E74 /* setSwordAtCollision */, b);
            }
            LK_FIELD(u32, 0x3B8) = LK_FIELD(u32, 0x3B8) | 0x40; /* daPyFlg0_CUT_AT_FLG */
            if (LK_FIELD(u32, 0x4A78) != 0) { /* mpCutfBrk */
                f32 f;
                if (LK_FIELD(u32, 0x6A70) & 0x80000000) { /* ModeFlg_PARRY */
                    f = 2.0f;
                } else if (LK_FIELD(u32, 0x3BC) & 0x8000) { /* daPyFlg1_SOUP_POWER_UP */
                    f = 1.0f;
                } else {
                    f = 0.0f;
                }
                lk_anmSetFrame(b, 0x4A6C, 0x4A78, 0x4A88, f);
            }
            proc = LK_FIELD(s32, 0x65F0);
            if (proc != 0x42 && proc != 0x63 && proc != 0x92) {
                f32 fr = LK_FIELD(f32, 0x589C);
                s32 n = (proc == 0x53 || proc == 0x4E) ? gabi::ftoi(fr * 2.5f) : gabi::ftoi(fr * 10.0f);
                u32 cl = LK_FIELD(u32, 0x448);
                u32 mtx = cl != 0 ? cl + 0xC8 : 0;
                f32 rate = (f32)gabi::call<f64>(0x02407178 /* getBlurTopRate */, b);
                s32 col = gabi::call<s32>(0x02407270 /* getSwordBlurColor */, b);
                gabi::call(0x02407070 /* daPy_swBlur_c::initSwBlur */, (u32)LK_FIELD(u32, 0x73EC), mtx, n, col, rate);
                proc = LK_FIELD(s32, 0x65F0);
                if (proc == 0x55 || proc == 0x56) {
                    f32 w;
                    u32 prm0, prm1, env;
                    if (LK_FIELD(u16, 0x69B0) == 0x101 /* daPyItem_BOKO_e */) {
                        u32 boko = LK_FIELD(u32, 0x6594);
                        u32 prm = boko != 0 ? gabi::load<u32>(boko + 0xB0) : 0;
                        f32 k = prm == 0 ? 1.0f : prm == 1 ? 1.3f : prm == 3 ? 1.7f : prm == 4 ? 2.2f : 1.5f;
                        w = gabi::fmuls_ppc(k, 1.5f);
                        prm0 = 0x101CEB30;
                        prm1 = 0x101CEB34;
                        env = 0x101CEB38;
                    } else {
                        bool roll = proc == 0x56 || (LK_FIELD(u32, 0x3BC) & 0x8000);
                        if (lk_selectSword() == 0x38 || gabi::load<u8>(dComIfGp_ea() + 0x5CEA) == 2) { /* checkNormalSwordEquip() */
                            w = 1.0f;
                            prm0 = roll ? 0x101CEB00 : 0x101CEB24;
                        } else if (lk_selectSword() == 0x3E) { /* checkFinalMasterSwordEquip() */
                            w = 1.5f;
                            prm0 = roll ? 0x101CEB18 : 0x101CEB3C;
                        } else {
                            w = 1.5f;
                            prm0 = roll ? 0x101CEB0C : 0x101CEB30;
                        }
                        prm1 = prm0 + 4;
                        env = prm0 + 8;
                    }
                    u32 e1 = lk_cutTurnEmitter(b, 0x25, b + 0x6730, prm0, env);
                    u32 e2 = lk_cutTurnEmitter(b, 0x26, b + 0x6740, prm1, env);
                    u32 e3 = lk_cutTurnEmitter(b, 0x28, b + 0x6750, prm0, env);
                    if (e1 != 0) { /* setGlobalParticleWidthScale */
                        gabi::store<f32>(e1 + 0x238, w);
                    }
                    if (e2 != 0) {
                        gabi::store<f32>(e2 + 0x238, w);
                    }
                    if (e3 != 0) {
                        gabi::store<f32>(e3 + 0x238, w);
                    }
                }
            }
        } else if (LK_FIELD(s32, 0x65F0) == 0x56 /* daPyProc_CUT_ROLL_e */) {
            u32 pos = LK_FIELD(u32, 0x73EC) + 0x38C; /* mSwBlur.field_0x304 */
            gabi::Local<cXyz> d1;
            gabi::Local<cXyz> d2;
            gabi::Local<cXyz> spAC;
            gabi::Local<cXyz> spA0;
            gabi::Local<cXyz> t;
            gabi::call(0x0201ADE0 /* cXyz::operator- */, b + 0x300, d1.get(), b + 0x314);
            gabi::call(0x0201AE48 /* cXyz::operator* */, d1.get(), d2.get(), 0.1f);
            for (u32 k = 0; k < 3; k++) {
                gabi::store<u32>(gabi::ea(spAC.get()) + k * 4, gabi::load<u32>(gabi::ea(d2.get()) + k * 4));
            }
            u32 cl = LK_FIELD(u32, 0x448);
            gabi::call(0x028E90D4 /* PSMTXCopy */, cl != 0 ? cl + 0xC8 : 0, stk);
            gabi::call(0x025F23EC /* mDoMtx_stack_c::push */);
            gabi::call(0x028E91EC /* PSMTXInverse */, stk, stk);
            gabi::call(0x028E8F64 /* PSMTXMultVec */, stk, b + 0x72CC, spA0.get());
            gabi::call(0x025F2468 /* mDoMtx_stack_c::pop */);
            for (s32 i = 0; i < 10; i++, pos += 0xC) {
                gabi::call(0x028E8F64 /* PSMTXMultVec */, stk, spA0.get(), pos);
                gabi::call(0x0201AE48 /* cXyz::operator* */, spAC.get(), t.get(), (f32)i);
                gabi::call(0x028E8D88 /* PSVECAdd */, pos, t.get(), pos);
                gabi::call(0x025F1C28 /* mDoMtx_YrotM */, stk, 0x578);
            }
            cl = LK_FIELD(u32, 0x448);
            gabi::call(0x028E90D4 /* PSMTXCopy */, cl != 0 ? cl + 0xC8 : 0, stk);
            gabi::call(0x025F1C28 /* mDoMtx_YrotM */, stk, -0x2BC);
            gabi::call(0x028E8F64 /* PSMTXMultVec */, stk, spA0.get(), sp98.get());
            if (LK_FIELD(u8, 0x6734) == 0xFF) { /* m331C.getAlpha() */
                u32 arr = LK_FIELD(u32, 0x73EC) + 0x38C;
                LK_FIELD(s16, 0x6736) = 10;
                LK_FIELD(s16, 0x6746) = 10;
                LK_FIELD(s16, 0x6756) = 10;
                LK_FIELD(u32, 0x6738) = arr;
                LK_FIELD(u32, 0x6748) = arr;
                LK_FIELD(u32, 0x6758) = arr;
            }
            gabi::call(0x02516618 /* dCcD_Cyl::MoveCAt */, b + 0x78BC, b + 0x314);
            gabi::call(0x024072F4 /* seStartSwordCut */, b, 0x203A /* JA_SE_LK_SUGOI_KAITEN */);
        } else {
            u32 blur = LK_FIELD(u32, 0x73EC);
            proc = LK_FIELD(s32, 0x65F0);
            f32 fr = LK_FIELD(f32, 0x589C);
            s32 n = gabi::ftoi(fr * 10.0f);
            s32 r26 = gabi::load<s32>(blur + 0xA0); /* mSwBlur.field_0x018 */
            if (proc != 0x42 && proc != 0x63 && proc != 0x92) {
                if (proc == 0x5C) {
                    n = 0x95;
                } else if (proc == 0x53 || proc == 0x4E) {
                    n = gabi::ftoi(fr * 2.5f);
                } else if (proc == 0x4C) {
                    n = (s32)((u32)n - 5);
                }
                u32 cl = LK_FIELD(u32, 0x448);
                gabi::call(0x02407310 /* daPy_swBlur_c::copySwBlur */, blur, cl != 0 ? cl + 0xC8 : 0, n);
                proc = LK_FIELD(s32, 0x65F0);
            }
            if (proc == 0x55 /* daPyProc_CUT_TURN_e */) {
                u32 cl = LK_FIELD(u32, 0x448);
                u32 pb = gabi::load<u32>(LK_FIELD(u32, 0x73EC) + 0xAC); /* mpPosBuffer */
                gabi::call(0x028E8F64 /* PSMTXMultVec */, cl != 0 ? cl + 0xC8 : 0, pb + (u32)n * 0x18 + 0x24, sp98.get());
                gabi::call(0x02516618 /* dCcD_Cyl::MoveCAt */, b + 0x78BC, b + 0x314);
                u8 alpha = LK_FIELD(u8, 0x6734);
                u32 bl = LK_FIELD(u32, 0x73EC);
                if (alpha == 0xFF) {
                    u32 bl2 = LK_FIELD(u32, 0x73EC);
                    u32 arr = bl2 + 0x38C;
                    s16 cnt = (s16)(gabi::load<s32>(bl2 + 0xA0) - r26);
                    LK_FIELD(u32, 0x6738) = arr;
                    LK_FIELD(u32, 0x6748) = arr;
                    LK_FIELD(s16, 0x6736) = cnt;
                    LK_FIELD(s16, 0x6746) = cnt;
                    LK_FIELD(s16, 0x6756) = cnt;
                    LK_FIELD(u32, 0x6758) = arr;
                }
                gabi::store<u32>(bl + 0x9C, 0); /* mSwBlur.field_0x014 */
            } else {
                gabi::call(0x02406E74 /* setSwordAtCollision */, b);
            }
        }
        proc = LK_FIELD(s32, 0x65F0);
        if (proc == 0x55 || proc == 0x56) {
            gabi::call(0x025E1CFC /* mDoAud_bgmNowKaitengiri */);
            u32 a0 = LK_FIELD(u32, 0x78BC), a2 = LK_FIELD(u32, 0x7C54), a1 = LK_FIELD(u32, 0x7B1C), a3 = LK_FIELD(u32, 0x7D8C);
            LK_FIELD(u32, 0x78BC) = a0 | 1; /* mAtCyl.OnAtSetBit() */
            LK_FIELD(u32, 0x7C54) = a2 & ~1u;
            LK_FIELD(u32, 0x7B1C) = a1 & ~1u;
            LK_FIELD(u32, 0x7D8C) = a3 & ~1u;
        } else {
            u32 rf = LK_FIELD(u32, 0x3C0);
            u32 a1 = LK_FIELD(u32, 0x7B1C), a0 = LK_FIELD(u32, 0x78BC), a2 = LK_FIELD(u32, 0x7C54);
            LK_FIELD(u32, 0x7B1C) = a1 | 1;
            LK_FIELD(u32, 0x78BC) = a0 & ~1u;
            u32 a3 = LK_FIELD(u32, 0x7D8C);
            if (rf & 1) {
                LK_FIELD(u32, 0x7C54) = a2 & ~1u;
                LK_FIELD(u32, 0x7D8C) = a3 & ~1u;
            } else {
                LK_FIELD(u32, 0x7C54) = a2 | 1;
                LK_FIELD(u32, 0x7D8C) = a3 | 1;
            }
        }
        lk_ccsSet(b + 0x78BC);
        lk_ccsSetMass(b + 0x78BC);
        for (u32 i = 0; i < 3; i++) {
            lk_ccsSet(b + 0x7B1C + i * 0x138);
            lk_ccsSetMass(b + 0x7B1C + i * 0x138);
        }
    } else {
        u32 blur = LK_FIELD(u32, 0x73EC);
        s32 v = gabi::load<s32>(blur + 0x9C);
        gabi::store<s32>(blur + 0x9C, v < 10 ? 0 : v - 10);
        if (LK_FIELD(s32, 0x65F0) == 0xA4 /* daPyProc_BOTTLE_OPEN_e */ && LK_FIELD(s16, 0x691C) != 0) {
            LK_FIELD(u32, 0x7B1C) = LK_FIELD(u32, 0x7B1C) | 1;
            lk_ccsSet(b + 0x7B1C);
            u32 a = LK_FIELD(u32, 0x7C54);
            if (LK_FIELD(s16, 0x691A) != 0) {
                LK_FIELD(u32, 0x7C54) = a | 1;
                lk_ccsSet(b + 0x7C54);
            } else {
                LK_FIELD(u32, 0x7C54) = a & ~1u;
            }
        } else {
            for (u32 i = 0; i < 3; i++) {
                u32 cps = b + 0x7B1C + i * 0x138;
                gabi::call(0x02516138 /* dCcD_GObjInf::ResetAtHit */, cps);
                gabi::store<u32>(cps, gabi::load<u32>(cps) & ~1u);
            }
        }
        gabi::call(0x02516138 /* dCcD_GObjInf::ResetAtHit */, b + 0x78BC);
        LK_FIELD(u32, 0x78BC) = LK_FIELD(u32, 0x78BC) & ~1u;
        LK_FIELD(u32, 0x3B8) = LK_FIELD(u32, 0x3B8) & ~0x40u; /* offNoResetFlg0(daPyFlg0_CUT_AT_FLG) */
        gabi::call(0x025A9D64 /* dPa_cutTurnEcallBack_c::end */, b + 0x6730);
        gabi::call(0x025A9D64, b + 0x6740);
        gabi::call(0x025A9D64, b + 0x6750);
    }
    if (gabi::call<BOOL>(0x02407678 /* fanWindCrashEffectDraw */, b)) {
        s16 mx = LK_FIELD(s16, 0x557A);
        f32 fm = (f32)mx;
        f32 f = gabi::fadds_ppc(LK_FIELD(f32, 0x5574), 1.0f);
        if (f > gabi::fsubs_ppc(fm, 0.5f)) {
            f = gabi::fsubs_ppc(fm, 0.001f);
        }
        lk_anmSetFrame(b, 0x5574, 0x55D8, 0x5580, f);
    }
    /* the fan wind */
    bool wind = false;
    gabi::Local<cXyz> sp94;
    gabi::Local<cXyz> sp88;
    gabi::Local<cXyz> spC4;
    u32 p94 = gabi::ea(sp94.get()), p88 = gabi::ea(sp88.get()), pC4 = gabi::ea(spC4.get());
    f32 radius = 0.0f;
    if (LK_FIELD(s16, 0x6984) != 0) { /* m3534 */
        for (u32 k = 0; k < 3; k++) { /* *mFanWindCps.GetEndP() */
            gabi::store<u32>(p94 + k * 4, gabi::load<u32>(b + 0x7FE8 + k * 4));
        }
        f32 t = (f32)LK_FIELD(s16, 0x6984) / 10.0f;
        u32 ia = (u32)LK_FIELD(u16, 0x6986) >> 3;
        u32 ib = (u32)LK_FIELD(u16, 0x6988) >> 3;
        f32 len = gabi::fmadds(t, 100.0f, 100.0f);
        f32 sa = gabi::load<f32>(0x104A44F8 + ia * 8);
        f32 sb = gabi::load<f32>(0x104A44F8 + ib * 8);
        f32 ca = gabi::load<f32>(0x104A44F8 + ia * 8 + 4);
        f32 cb = gabi::load<f32>(0x104A44F8 + ib * 8 + 4);
        gabi::store<f32>(pC4 + 0, gabi::fmuls_ppc(gabi::fmuls_ppc(len, sa), cb));
        gabi::store<f32>(pC4 + 4, -gabi::fmuls_ppc(len, sb));
        gabi::store<f32>(pC4 + 8, gabi::fmuls_ppc(gabi::fmuls_ppc(len, ca), cb));
        {
            gabi::Local<cXyz> s;
            gabi::call(0x0201AD78 /* cXyz::operator+ */, p94, s.get(), pC4);
            for (u32 k = 0; k < 3; k++) {
                gabi::store<u32>(p88 + k * 4, gabi::load<u32>(gabi::ea(s.get()) + k * 4));
            }
        }
        gabi::call(0x024F1AFC /* dBgS_LinChk::Set */, b + 0xBB4, p94, p88, b);
        if (gabi::call<BOOL>(0x02008860 /* cBgS::LineCross */, dComIfG_Bgsp(), b + 0xBB4)) {
            u32 pla = gabi::call<u32>(0x020084C8 /* cBgS::GetTriPla */, dComIfG_Bgsp(), (u32)LK_FIELD(u16, 0xBCA), (u32)LK_FIELD(u16, 0xBC8));
            f32 cx = gabi::load<f32>(b + 0xBE4); /* mArrowLinChk.GetCrossP() */
            f32 cy = gabi::load<f32>(b + 0xBE8);
            f32 cz = gabi::load<f32>(b + 0xBEC);
            gabi::store<f32>(p88 + 0, cx);
            gabi::store<f32>(p88 + 8, cz);
            bool crash = false;
            if (pla != 0) {
                gabi::store<f32>(p88 + 4, cy);
                crash = gabi::load<f32>(pla + 4) < 0.5f; /* !cBgW_CheckBGround(ny) */
            }
            if (crash) {
                LK_FIELD(s16, 0x6984) = 0;
                gabi::call(LK_mtxFollowEcallBack_end, b + 0x67FC); /* m33E8.end() */
                lk_anmSetFrame(b, 0x54F8, 0x5504, 0x5514, gabi::fsubs_ppc((f32)LK_FIELD(s16, 0x54FE), 0.001f));
                lk_anmSetFrame(b, 0x5574, 0x55D8, 0x5580, 0.0f);
                PSMTXTrans(mDoMtx_stack_c::get(), gabi::load<f32>(p88 + 0), gabi::load<f32>(p88 + 4), gabi::load<f32>(p88 + 8));
                gabi::Local<cXyz> nxz;
                f32 nx = gabi::load<f32>(pla + 0);
                f32 nz = gabi::load<f32>(pla + 8);
                nxz->y = 0.0f;
                nxz->x = nx;
                nxz->z = nz;
                f32 sq = gabi::call<f32>(0x028E8DD0 /* PSVECSquareMag */, nxz.get());
                f32 axz = (f32)gabi::call<f64>(0x028F4384 /* sqrtf */, sq);
                s16 a1 = gabi::call<s16>(0x020195B0 /* cM_atan2s */, -gabi::load<f32>(pla + 4), axz);
                s16 a2 = gabi::call<s16>(0x020195B0 /* cM_atan2s */, -gabi::load<f32>(pla + 0), -gabi::load<f32>(pla + 8));
                gabi::call(0x025F1B48 /* mDoMtx_ZXYrotM */, stk, (s32)a1, (s32)a2, 0);
                lk_setBaseTRMtx(LK_FIELD(u32, 0x556C) /* mpYbafo00Model */, stk);
                u32 pa = gabi::load<u32>(dComIfGp_ea() + 0x5AB0);
                u32 e = gabi::call<u32>(0x025A847C, pa, 1, 0x49 /* ID_AK_JN_UCHIWAWIND01 */, p88, 0, 0, 0xFF, 0, -1, 0, 0, 0);
                if (e != 0) {
                    gabi::call(0x028249B0 /* JPASetRMtxTVecfromMtx */, stk, e + 0x1F0, e + 0x22C); /* setGlobalRTMatrix */
                    u8 g = LK_FIELD(u8, 0x1A9), r = LK_FIELD(u8, 0x1A8), bb = LK_FIELD(u8, 0x1AA); /* tevStr.mColorK0 */
                    gabi::store<u8>(e + 0x244, r);
                    gabi::store<u8>(e + 0x245, g);
                    gabi::store<u8>(e + 0x246, bb);
                }
            } else {
                gabi::store<f32>(p88 + 4, gabi::fadds_ppc(cy, 70.0f));
                LK_FIELD(s16, 0x6988) = gabi::call<s16>(0x023E7D6C /* getGroundAngle */, b, b + 0xBC8, (s32)LK_FIELD(s16, 0x6986));
            }
        }
        wind = true;
        radius = gabi::fmadds(gabi::fsubs_ppc(1.0f, t), 230.0f, 70.0f);
        if (LK_FIELD(s16, 0x6984) != 0) {
            LK_FIELD(s16, 0x6984) = (s16)(LK_FIELD(s16, 0x6984) - 1);
            PSMTXTrans(mDoMtx_stack_c::get(), gabi::load<f32>(p88 + 0), gabi::load<f32>(p88 + 4), gabi::load<f32>(p88 + 8));
            gabi::call(0x025F1B48 /* mDoMtx_ZXYrotM */, stk, (s32)LK_FIELD(s16, 0x6988), (s32)LK_FIELD(s16, 0x6986), 0);
            lk_setBaseTRMtx(LK_FIELD(u32, 0x53F0) /* mpYuchw00Model */, stk);
            lk_yuchwPlay(b);
        }
    } else if (gabi::call<BOOL>(0x023D93DC /* fanWindEffectDraw */, b)) {
        lk_yuchwPlay(b);
        f32 mx = (f32)LK_FIELD(s16, 0x54FE);
        f32 f = gabi::fadds_ppc(LK_FIELD(f32, 0x54F8), 1.0f);
        if (f > gabi::fsubs_ppc(mx, 0.5f)) {
            f = gabi::fsubs_ppc(mx, 0.001f);
            gabi::call(LK_mtxFollowEcallBack_end, b + 0x67FC); /* m33E8.end() */
        }
        lk_anmSetFrame(b, 0x54F8, 0x5504, 0x5514, f);
    } else if (LK_FIELD(f32, 0x5574) < 5.0f) {
        u32 m = LK_FIELD(u32, 0x556C);
        u32 mtx = m != 0 ? m + 0xC8 : 0;
        f32 x = gabi::load<f32>(mtx + 0xC);
        gabi::store<f32>(p94 + 0, x);
        f32 y = gabi::load<f32>(mtx + 0x1C);
        gabi::store<f32>(p94 + 4, y);
        f32 z = gabi::load<f32>(mtx + 0x2C);
        gabi::store<f32>(p94 + 8, z);
        gabi::store<u32>(pC4 + 0, gabi::load<u32>(0x101FFBA8)); /* cXyz::Zero */
        gabi::store<f32>(p88 + 4, gabi::fadds_ppc(y, 10.0f));
        gabi::store<u32>(pC4 + 4, gabi::load<u32>(0x101FFBAC));
        gabi::store<u32>(pC4 + 8, gabi::load<u32>(0x101FFBB0));
        gabi::store<f32>(p88 + 8, z);
        radius = 120.0f;
        gabi::store<f32>(p88 + 0, x);
        wind = true;
    }
    if (wind) {
        cps_SetStartEnd_l(b + 0x7EC4, sp94.get(), sp88.get()); /* mFanWindCps */
        u32 v0 = gabi::load<u32>(pC4 + 8), v1 = gabi::load<u32>(pC4 + 0), v2 = gabi::load<u32>(pC4 + 4);
        LK_FIELD(u32, 0x7F40) = v1; /* SetAtVec */
        LK_FIELD(u32, 0x7F44) = v2;
        LK_FIELD(f32, 0x7FF8) = radius; /* SetR */
        LK_FIELD(u32, 0x7F48) = v0;
        lk_ccsSet(b + 0x7EC4);
        lk_ccsSetMass(b + 0x7EC4);
    } else {
        gabi::call(0x02516138 /* dCcD_GObjInf::ResetAtHit */, b + 0x7EC4);
    }
    if (gabi::call<BOOL>(0x023D93DC /* fanWindEffectDraw */, b)) {
        s8 rev = LK_FIELD(s8, 0x68DB);
        gabi::call(0x025E1A40 /* mDoAud_seStart */, 0x2054 /* JA_SE_LK_FAN_WIND */, b + 0x7FE8, 0, (s32)rev);
    }
    if (LK_FIELD(s16, 0x698A) != 0) { /* m353A */
        u32 sph = b + 0x7FFC;
        if (LK_FIELD(s32, 0x65F0) == 0x93 /* daPyProc_FAN_GLIDE_e */) {
            u32 m = gabi::ea(lk_getAnmMtx(mpCLModel, 0));
            gabi::Local<cXyz> p;
            fcpy_l(gabi::ea(p.get()) + 0, m + 0xC);
            fcpy_l(gabi::ea(p.get()) + 4, m + 0x1C);
            fcpy_l(gabi::ea(p.get()) + 8, m + 0x2C);
            gabi::call(0x02018D40 /* cM3dGSph::SetC */, sph + 0x118, p.get());
        } else {
            gabi::call(0x02018D40 /* cM3dGSph::SetC */, sph + 0x118, b + 0x314);
        }
        lk_ccsSet(sph);
        lk_ccsSetMass(sph);
        LK_FIELD(s16, 0x698A) = (s16)(LK_FIELD(s16, 0x698A) - 1);
    } else {
        gabi::call(0x02516138 /* dCcD_GObjInf::ResetAtHit */, b + 0x7FFC);
    }
    u32 lcps = b + 0x8128; /* mFanLightCps */
    if (LK_FIELD(u32, 0x3C0) & 0x00200000) { /* daPyRFlg0_LIGHT_REFLECT */
        gabi::Local<cXyz> lv;
        u32 sh = LK_FIELD(u32, 0xE80);
        gabi::call(0x028E8F64 /* PSMTXMultVec */, sh != 0 ? sh + 0xC8 : 0, 0x10034FD8u /* l_ms_light_local_start */, p94);
        sh = LK_FIELD(u32, 0xE80);
        gabi::call(0x028E9044 /* PSMTXMultVecSR */, sh != 0 ? sh + 0xC8 : 0, 0x10034FE4u /* l_ms_light_local_vec */, lv.get());
        {
            gabi::Local<cXyz> s;
            gabi::call(0x0201AD78 /* cXyz::operator+ */, p94, s.get(), lv.get());
            for (u32 k = 0; k < 3; k++) {
                gabi::store<u32>(p88 + k * 4, gabi::load<u32>(gabi::ea(s.get()) + k * 4));
            }
        }
        gabi::call(0x024F1AFC /* dBgS_LinChk::Set */, b + 0xC20, p94, p88, b);
        if (gabi::call<BOOL>(0x02008860 /* cBgS::LineCross */, dComIfG_Bgsp(), b + 0xC20)) {
            for (u32 k = 0; k < 3; k++) {
                gabi::store<u32>(p88 + k * 4, gabi::load<u32>(b + 0xC50 + k * 4));
            }
            gabi::Local<cXyz> s;
            gabi::call(0x0201ADE0 /* cXyz::operator- */, p88, s.get(), p94);
            for (u32 k = 0; k < 3; k++) {
                gabi::store<u32>(gabi::ea(lv.get()) + k * 4, gabi::load<u32>(gabi::ea(s.get()) + k * 4));
            }
        }
        cps_SetStartEnd_l(lcps, sp94.get(), sp88.get());
        u32 f0 = gabi::load<u32>(lcps);
        u32 v0 = gabi::load<u32>(gabi::ea(lv.get()) + 0), v1 = gabi::load<u32>(gabi::ea(lv.get()) + 4);
        gabi::store<u32>(lcps + 0x7C, v0);
        gabi::store<u32>(lcps + 0x80, v1);
        u32 v2 = gabi::load<u32>(gabi::ea(lv.get()) + 8);
        gabi::store<u32>(lcps, f0 | 1); /* OnAtSetBit */
        gabi::store<u32>(lcps + 0x84, v2);
        lk_ccsSet(lcps);
        lk_ccsSetMass(lcps);
    } else {
        gabi::call(0x02516138 /* dCcD_GObjInf::ResetAtHit */, lcps);
        gabi::store<u32>(lcps, gabi::load<u32>(lcps) & ~1u);
    }
}
VERIFY(0x024076CC, &daPy_lk_c::setCollision);

/* joint matrix column 3 (mDoMtx_multVecZero) stored at dst (float loads/stores) */
static inline void lk_jntTrans(u32 m, u32 dst) {
    gabi::store<f32>(dst + 0, gabi::load<f32>(m + 0xC));
    gabi::store<f32>(dst + 4, gabi::load<f32>(m + 0x1C));
    gabi::store<f32>(dst + 8, gabi::load<f32>(m + 0x2C));
}
/* play + o (one dComIfGp_get() per access) */
static inline u8 lk_playU8(u32 o) { return gabi::load<u8>(dComIfGp_ea() + o); }
static inline void lk_playSetU8(u32 o, u8 v) { gabi::store<u8>(dComIfGp_ea() + o, v); }
static inline s8 lk_playU8_s(u32 o) { return gabi::load<s8>(dComIfGp_ea() + o); }

/* 0240CDD0 (HD: a heavy-state counter (+0x69EE) gates the heavy flag before setGetDemo; the forest water
 * expiry only swaps the bottle and sets an event register; "no control" is Link not being the play's
 * player; an event order from play flag 0x200000; the warp/pitfall events shrink Link's models; a
 * frame counter at +0x8260; no separate A-button logic for the sword) */
BOOL daPy_lk_c::execute() {
    WWHD_FUNC(0x0240CDD0, BOOL, this);
    u32 b = gabi::ea(this);
    {
        s32 c = (s32)((u32)LK_FIELD(s32, 0x8260) - 1);
        s16 heavy = LK_FIELD(s16, 0x69EE);
        LK_FIELD(s32, 0x8260) = c;
        if (c < 0) {
            LK_FIELD(s32, 0x8260) = 0;
        }
        bool clearCheck = true;
        if (heavy >= 5) {
            if (!gabi::call<BOOL>(LK_checkHeavyStateOn, b)) {
                LK_FIELD(u32, 0x3B8) = LK_FIELD(u32, 0x3B8) | 0x40000000;
                clearCheck = false;
            } else if (LK_FIELD(s16, 0x69EE) >= 5) {
                clearCheck = false;
            }
        }
        if (clearCheck && gabi::call<BOOL>(LK_checkHeavyStateOn, b)) {
            LK_FIELD(u32, 0x3B8) = LK_FIELD(u32, 0x3B8) & ~0x40000000u;
        }
        LK_FIELD(s16, 0x69EE) = 0;
        if (gabi::call<BOOL>(0x023DBDD0 /* setGetDemo */, b)) {
            return TRUE;
        }
    }
    if (gabi::call<BOOL>(0x025B5C80 /* dSv_player_item_c::checkBottle */, gabi::load<u32>(0x101F84DC) + 0x5C, 0x59 /* dItemNo_FOREST_WATER_e */) &&
        gabi::call<s32>(0x025B61C8 /* dSv_player_item_record_c::getTimer */, gabi::load<u32>(0x101F84DC) + 0x86) == 0) {
        gabi::call(0x025B51DC /* setBottleItemIn */, gabi::load<u32>(0x101F84DC) + 0x5C, 0x59, 0x56 /* dItemNo_WATER_BOTTLE_e */);
        gabi::call(0x025B8AF4 /* dSv_event_c::setEventReg */, gabi::load<u32>(0x101F84DC) + 0x644, 0x9EFF, 0);
        gabi::call(0x025E1988, 0x8B2);
    }
    /* checkNoControll() */
    if (gabi::load<u32>(dComIfGp_ea() + 0x5B2C) != b) {
        u32 st = LK_FIELD(u32, 0x2E0);
        LK_FIELD(u32, 0x39C) = 0; /* attention_info.flags */
        LK_FIELD(u32, 0x2E0) = (st & ~0x3Fu) | 0x30;
        if (!(gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & 0x10000)) { /* daPyStts0_SHIP_RIDE_e */
            LK_FIELD(f32, 0x7628) = 0.0f; /* mStts.ClrCcMove() */
            LK_FIELD(f32, 0x7620) = 0.0f;
            LK_FIELD(u8, 0x7634) = 0xFF; /* SetWeight */
            LK_FIELD(f32, 0x7624) = 0.0f;
        } else {
            LK_FIELD(u8, 0x7634) = 0x78;
        }
    } else {
        u32 st = LK_FIELD(u32, 0x2E0);
        LK_FIELD(u32, 0x39C) = 0xFFFFFFFF;
        LK_FIELD(u32, 0x2E0) = (st & ~0x3Fu) | 0x21;
        LK_FIELD(u8, 0x7634) = 0x78;
    }
    if ((LK_FIELD(u32, 0x3B8) & 0x02000000) && lk_playU8(0x5BBB) != 0x29 && lk_playU8(0x5BBC) != 0x29 && lk_playU8(0x5BBD) != 0x29) {
        LK_FIELD(u32, 0x3B8) = LK_FIELD(u32, 0x3B8) & ~0x02000000u; /* offNoResetFlg0(EQUIP_HEAVY_BOOTS) */
    } else if ((LK_FIELD(u32, 0x3BC) & 1) && lk_playU8(0x5BBB) != 0x2A && lk_playU8(0x5BBC) != 0x2A && lk_playU8(0x5BBD) != 0x2A) {
        LK_FIELD(u32, 0x3BC) = LK_FIELD(u32, 0x3BC) & ~1u; /* offNoResetFlg1(EQUIP_DRAGON_SHIELD) */
    }
    if (lk_playU8(0x5292) != 0) { /* dComIfGp_event_runCheck() */
        LK_FIELD(s32, 0x69DC) = gabi::call<s32>(0x02542D88 /* dEvent_manager_c::getMyStaffId */, dComIfGp_ea() + 0x52C4, 0x10035AE8u /* "Link" */, b, 0);
        if (LK_FIELD(u16, 0xF8) == 3 /* checkCommandDoor() */ && !(gabi::load<u16>(dComIfGp_ea() + 0x52B8) & 4) &&
            LK_FIELD(u16, 0x69B0) == 0x101) {
            u32 boko = LK_FIELD(u32, 0x6594);
            if (boko != 0) { /* boko->moveStateInit(5.0f, 0.0f, shape_angle.y + 0x8000) */
                s16 a = LK_FIELD(s16, 0x32A);
                gabi::store<f32>(boko + 0x340, 0.0f);
                gabi::store<f32>(boko + 0x370, 5.0f);
                gabi::store<s16>(boko + 0x322, (s16)(a + 0x8000));
            }
            gabi::call(0x023DC7AC /* deleteEquipItem */, b, 0);
        }
    }
    /* l_debug_keep_pos / l_debug_shape_angle / l_debug_current_angle */
    for (u32 k = 0; k < 3; k++) {
        gabi::store<u32>(b + 0x314 + k * 4, gabi::load<u32>(0x1046CD48 + k * 4));
    }
    for (u32 k = 0; k < 3; k++) {
        gabi::store<u16>(b + 0x328 + k * 2, gabi::load<u16>(0x1046CD10 + k * 2));
    }
    for (u32 k = 0; k < 3; k++) {
        gabi::store<u16>(b + 0x320 + k * 2, gabi::load<u16>(0x1046CD08 + k * 2));
    }
    LK_FIELD(s32, 0x69BC) = (s32)lk_playU8_s(0x5B30); /* mCameraInfoIdx */
    for (u32 k = 0; k < 3; k++) { /* m3748 = current.pos */
        gabi::store<u32>(b + 0x7350 + k * 4, gabi::load<u32>(b + 0x314 + k * 4));
    }
    if (LK_FIELD(s32, 0x65F0) != 0x85 /* daPyProc_HOOKSHOT_FLY_e */ && !(gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & 0x10000) &&
        !(LK_FIELD(u32, 0x6A70) & 0x410800) && LK_FIELD(s32, 0x65F0) != 0xA9 && LK_FIELD(f32, 0x8A0) != -1000000000.0f &&
        !(LK_FIELD(u32, 0x3B8) & 0xA0000000) && gabi::call<BOOL>(0x02008254 /* cBgS::ChkPolySafe */, dComIfG_Bgsp(), b + 0x8F4) &&
        gabi::call<BOOL>(0x024EEABC /* dBgS::ChkMoveBG */, dComIfG_Bgsp(), b + 0x8F4)) {
        u32 bgs = gabi::ea(dComIfG_Bgsp());
        gabi::call(0x024EF968 /* dBgS::MoveBgCrrPos */, bgs, b + 0x8F4, (LK_FIELD(u32, 0x834) >> 5) & 1, b + 0x314, b + 0x320, b + 0x328);
        bgs = gabi::ea(dComIfG_Bgsp());
        gabi::call(0x024EF968 /* dBgS::MoveBgCrrPos */, bgs, b + 0x8F4, (LK_FIELD(u32, 0x834) >> 5) & 1, b + 0x300, 0, 0);
    }
    gabi::Local<cXyz> oldPos;
    fcpy_l(gabi::ea(oldPos.get()) + 8, b + 0x308);
    fcpy_l(gabi::ea(oldPos.get()) + 4, b + 0x304);
    LK_FIELD(s16, 0x6926) = LK_FIELD(s16, 0x32A); /* m34DE */
    LK_FIELD(s16, 0x6932) = LK_FIELD(s16, 0x6924); /* m34EA = m34DC */
    fcpy_l(gabi::ea(oldPos.get()) + 0, b + 0x300);
    LK_FIELD(f32, 0x6A0C) = LK_FIELD(f32, 0x6A08); /* m35B4 = mStickDistance */
    lk_playSetU8(0x5BB7, 0); /* dComIfGp_setDoStatus(dActStts_BLANK_e) */
    lk_playSetU8(0x5BB5, 0); /* dComIfGp_setRStatus */
    lk_playSetU8(0x5BB6, 0); /* dComIfGp_setAStatus */
    lk_playSetU8(0x5C2E, 0x4D);
    LK_FIELD(u8, 0x68D5) = 0; /* mFrontWallType */
    u32 rf = LK_FIELD(u32, 0x3C0);
    if (rf & 0x00100000) { /* daPyRFlg0_POISON_CURSE */
        gabi::call(0x023DCB54 /* setDamageCurseEmitter */, b);
        rf = LK_FIELD(u32, 0x3C0);
    }
    if (rf & 0x08000000) { /* daPyRFlg0_NOT_ATTACKING */
        LK_FIELD(u8, 0x3AC) = 0; /* mCutType */
    }
    LK_FIELD(u32, 0x3C0) = 0;
    gabi::call(0x0207A9A0 /* cLib_calcTimer<u8> */, 0x101CEF1Bu); /* daPy_matAnm_c::decMorfFrame() */
    gabi::call(0x0207A9A0 /* cLib_calcTimer<u8> */, 0x101CEF19u); /* decMabaTimer() */
    if (LK_FIELD(s32, 0x65F0) == 0xA5 /* daPyProc_BOTTLE_SWING_e */ && LK_FIELD(u16, 0xF8) == 6 /* checkCommandCatch() */) {
        LK_FIELD(u16, 0x420) = 5; /* mDemo.setSpecialDemoType() */
    }
    gabi::call(0x023DCC90 /* setActorPointer */, b);
    gabi::call(0x023DCD30 /* setAtnList */, b);
    {
        u32 play = dComIfGp_ea();
        u32 z = gabi::call<u32>(0x024EBAE8 /* dAttCatch_c::convPId */, play + 0x5928, gabi::load<u32>(play + 0x5930)); /* dComIfGp_att_getZHint() */
        gabi::call(0x023D4768 /* stopDoButtonQuake */, b, z != 0 ? 0 : 1);
    }
    {
        u8 m = LK_FIELD(u8, 0x68DE); /* m34C2 */
        if (m == 8) {
            if (!(gabi::load<f32>(LK_FIELD(u32, 0x65CC) + 4) > 0.0f)) { /* m_old_fdata->getOldFrameMorfCounter() */
                LK_FIELD(u8, 0x68DE) = 9;
            }
        } else if (m != 0xC) {
            LK_FIELD(u8, 0x68DE) = 0;
        }
    }
    {
        u32 grab = LK_FIELD(u32, 0x65A4); /* mActorKeepGrab.getActor() */
        u32 equip = LK_FIELD(u32, 0x6594);
        if (grab != 0 && !(gabi::load<u32>(grab + 0x2E0) & 0x2000) /* fopAcM_checkCarryNow */) {
            gabi::call(0x023DCF8C /* freeGrabItem */, b);
        }
        if (LK_FIELD(u16, 0x69B0) == 0x101 && (equip == 0 || !(gabi::load<u32>(equip + 0x2E0) & 0x2000))) {
            gabi::call(0x023DC7AC /* deleteEquipItem */, b, 0);
        }
    }
    {
        u32 f1 = LK_FIELD(u32, 0x3BC);
        if (f1 & 0x800) { /* checkFreezeState() */
            if (!(f1 & 0x40000)) {
                gabi::call(0x025F0658 /* mDoGph_gInf_c::fadeOut */, 0x101CEAFCu /* l_freeze_fade_color */, 0x10030000u /* (GHS: r4 keeps the constant base) */, -0.02f);
                LK_FIELD(u32, 0x3BC) = LK_FIELD(u32, 0x3BC) | 0x40000;
            }
        } else if (!(f1 & 0x40000000)) {
            gabi::call(0x023DD054 /* animeUpdate */, b);
        }
    }
    gabi::call(0x023F22FC /* setDemoData */, b);
    gabi::call(0x023F32E4 /* setStickData */, b);
    if (LK_FIELD(u16, 0x69B0) == 0x25 /* dItemNo_GRAPPLING_HOOK_e */ && (LK_FIELD(s32, 0x65F0) == 0x76 || LK_FIELD(s32, 0x65F0) == 0x7D)) {
        u32 eq = LK_FIELD(u32, 0x6594);
        if (eq != 0 && gabi::load<u32>(eq + 0xB0) == 2) {
            gabi::call(0x023EB864 /* procRopeReady_init */, b);
        }
    }
    if ((gabi::load<u32>(dComIfGp_ea() + 0x5CDC) & 0x200000) && lk_playU8(0x5292) == 0) {
        u32 play = dComIfGp_ea();
        if (gabi::call<BOOL>(0x0253EC0C /* dEvt_control_c::order */, play + 0x51D0, 3, 1, 0, 0xFFFF, b, b + 0x8264, -1, 0xFF)) {
            LK_FIELD(u8, 0x8265) = 1;
            LK_FIELD(u8, 0x8264) = 1;
        }
    }
    {
        u32 play = dComIfGp_ea();
        gabi::store<u32>(play + 0x5CDC, gabi::load<u32>(play + 0x5CDC) & ~0x200000u);
        u32 on = (gabi::load<u32>(dComIfGp_ea() + 0x5CDC) & 0x100000) != 0 ? 1 : 0;
        if (on == 0 && LK_FIELD(u8, 0x8282) != 0) {
            LK_FIELD(u32, 0x3BC) = LK_FIELD(u32, 0x3BC) & ~0x100u;
            LK_FIELD(u32, 0x6A70) = LK_FIELD(u32, 0x6A70) | 0x100;
        }
        LK_FIELD(u8, 0x8282) = (u8)on;
    }
    if (LK_FIELD(u16, 0x420) != 5) { /* !checkSpecialDemoMode() */
        if (!gabi::call<BOOL>(0x023F695C /* changeDemoProc */, b) && !gabi::call<BOOL>(0x023F7820 /* changeDeadProc */, b) &&
            LK_FIELD(s32, 0x65F0) != 0xB0 && LK_FIELD(s32, 0x65F0) != 0xB1) {
            if (!gabi::call<BOOL>(0x023F81A4 /* changeAutoJumpProc */, b)) {
                gabi::call(0x023F8F80 /* changeSwimProc */, b);
            }
        }
        gabi::call(0x023FA578 /* changeDamageProc */, b);
        gabi::call(0x023FB020 /* changeBoomerangCatchProc */, b);
    }
    gabi::call(0x023FB230 /* checkItemAction */, b);
    if (LK_FIELD(u16, 0x69B0) == 0x103 && (LK_FIELD(u8, 0x68E0) == 2 || LK_FIELD(u8, 0x68E0) == 3)) { /* m34C4 */
        if (gabi::call<BOOL>(0x025160DC /* ChkAtHit */, b + 0x7B1C) || gabi::call<BOOL>(0x025160DC, b + 0x7C54) ||
            gabi::call<BOOL>(0x025160DC, b + 0x7D8C)) {
            LK_FIELD(u8, 0x68E0) = (u8)(LK_FIELD(u8, 0x68E0) + 2);
        }
    }
    {
        s16 t = LK_FIELD(s16, 0x6972); /* m3522 */
        if (t > 0) {
            LK_FIELD(s16, 0x6972) = (s16)(t - 1);
        } else {
            LK_FIELD(u8, 0x68E0) = 0;
        }
    }
    gabi::call(0x023FB8A0 /* setShieldGuard */, b);
    if (gabi::call<BOOL>(0x023FBC7C /* checkAtHitEnemy */, b, b + 0x7B1C) || gabi::call<BOOL>(0x023FBC7C, b, b + 0x7C54) ||
        gabi::call<BOOL>(0x023FBC7C, b, b + 0x7D8C)) {
        LK_FIELD(u32, 0x3B8) = LK_FIELD(u32, 0x3B8) | 0x10000000;
    }
    {
        s16 idx = LK_FIELD(s16, 0x65F6); /* (this->*mCurProcFunc)() */
        if (idx != 0) {
            u32 obj = b + (s32)LK_FIELD(s16, 0x65F4);
            if (idx < 0) {
                gabi::call_ptr(LK_FIELD(u32, 0x65F8), obj);
            } else {
                u32 vt = gabi::load<u32>(obj + (s32)LK_FIELD(s16, 0x65FA));
                gabi::call_ptr(gabi::load<u32>(vt + (u32)(s32)idx * 8 + 4), obj);
            }
        }
    }
    if (LK_FIELD(u16, 0x69B0) == 0x101 && lk_playU8(0x5BB7) == 0) {
        lk_playSetU8(0x5BB7, 0x2F);
    }
    if ((lk_selectSword() != 0xFF || lk_playU8(0x5CEA) == 2) && lk_playU8(0x5BB6) == 0 && /* checkSwordEquip() */
        !(gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & 0x01012000) && !(gabi::load<u32>(dComIfGp_ea() + 0x5CDC) & 0x88)) {
        lk_playSetU8(0x5BB6, 0x35);
    }
    gabi::call(0x023FBCEC /* playTextureAnime */, b);
    u32 sp14z, sp14x, sp14y;
    sp14z = lk_fbits(LK_FIELD(f32, 0x31C));
    sp14x = lk_fbits(LK_FIELD(f32, 0x314));
    sp14y = lk_fbits(LK_FIELD(f32, 0x318));
    gabi::call(0x023FDA70 /* posMove */, b);
    for (u32 k = 0; k < 3; k++) { /* mOldSpeed = speed */
        gabi::store<u32>(b + 0x729C + k * 4, gabi::load<u32>(b + 0x33C + k * 4));
    }
    u32 sp8z = lk_fbits(LK_FIELD(f32, 0x31C));
    u32 acf = LK_FIELD(u32, 0x834);
    u32 sp8y = lk_fbits(LK_FIELD(f32, 0x318));
    u32 sp8x = lk_fbits(LK_FIELD(f32, 0x314));
    LK_FIELD(u32, 0x834) = acf & ~0x20u; /* mAcch.ClrGroundHit() */
    gabi::call(0x024F08A8 /* dBgS_Acch::CrrPos */, b + 0x80C, dComIfG_Bgsp());
    gabi::call(0x023FE224 /* setWaterY */, b);
    gabi::call(0x023FE2F0 /* autoGroundHit */, b);
    gabi::call(0x023FE7C0 /* checkLavaFace */, b, oldPos.get(), 0);
    if (LK_FIELD(u32, 0x6A70) & 0x40000) { /* ModeFlg_SWIM */
        if ((LK_FIELD(u32, 0x3B8) & 0x100) && !gabi::call<BOOL>(0x023FE918 /* checkSwimFallCheck */, b)) {
            LK_FIELD(f32, 0x318) = LK_FIELD(f32, 0x6A28); /* mWaterY */
        }
        if (LK_FIELD(s32, 0x65F0) != 0xB2 /* daPyProc_DEMO_DEAD_e */) {
            f32 h = gabi::fadds_ppc(LK_FIELD(f32, 0x8A0), 84.9f);
            if (h > LK_FIELD(f32, 0x318) && (LK_FIELD(u32, 0x834) & 0x20)) {
                f32 sy = LK_FIELD(f32, 0x340);
                LK_FIELD(f32, 0x318) = h;
                if (sy < 0.0f) {
                    LK_FIELD(f32, 0x340) = 0.0f;
                }
            }
        } else if (LK_FIELD(u32, 0x834) & 0x20) {
            f32 sy = LK_FIELD(f32, 0x340);
            LK_FIELD(f32, 0x318) = LK_FIELD(f32, 0x8A0);
            if (sy < 0.0f) {
                LK_FIELD(f32, 0x340) = 0.0f;
            }
        }
    } else if (LK_FIELD(s32, 0x65F0) == 0xA9 /* daPyProc_DEMO_TOOL_e */) {
        gmem_stf32(b + 0x314, sp14x);
        gmem_stf32(b + 0x318, sp14y);
        gmem_stf32(b + 0x31C, sp14z);
        if (LK_FIELD(u32, 0x69C4) != 0 && LK_FIELD(f32, 0x8A0) != -1000000000.0f) { /* mProcVar7.m3574 */
            LK_FIELD(f32, 0x318) = LK_FIELD(f32, 0x8A0);
        }
    } else if (LK_FIELD(s32, 0x65F0) == 0x85 || (gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & 0x10000) || (LK_FIELD(u32, 0x6A70) & 0x10400800)) {
        gmem_stf32(b + 0x318, sp8y);
        gmem_stf32(b + 0x314, sp8x);
        gmem_stf32(b + 0x31C, sp8z);
    } else {
        u32 p = LK_FIELD(u32, 0x65F0);
        if ((p >= 0x2B && p <= 0x2F) || p == 0xC1) { /* door open / hang procs */
            gmem_stf32(b + 0x31C, sp8z);
            gmem_stf32(b + 0x314, sp8x);
        }
    }
    s32 roomNo;
    if (LK_FIELD(f32, 0x8A0) != -1000000000.0f) {
        roomNo = gabi::call<s32>(0x023FE960 /* setRoomInfo */, b);
        LK_FIELD(u32, 0x69CC) = LK_FIELD(u32, 0x69D0); /* m357C = m3580 */
        LK_FIELD(u32, 0x69D0) = gabi::call<u32>(0x024EF0BC /* dBgS::GetGroundCode */, dComIfG_Bgsp(), b + 0x8F4);
        if (LK_FIELD(u32, 0x3B8) & 0x80000000) {
            LK_FIELD(u32, 0x69D4) = 0; /* mCurrAttributeCode = dBgS_Attr_NORMAL_e */
        } else {
            LK_FIELD(u32, 0x69D4) = gabi::call<u32>(0x024EF0F4 /* dBgS::GetAttributeCode */, dComIfG_Bgsp(), b + 0x8F4);
        }
        gabi::call(0x025C35E8 /* dStage_RoomCheck */, b + 0x8E0);
        gabi::call(0x023FEA6C /* checkFallCode */, b);
        u32 ac = LK_FIELD(u32, 0x834);
        u32 mode = LK_FIELD(u32, 0x6A70);
        if (ac & 0x20) { /* mAcch.ChkGroundHit() */
            u32 r0;
            s32 attr = LK_FIELD(s32, 0x69D4);
            if (!(mode & 0x20) && (attr == 6 || attr == 8)) { /* lava / void */
                gabi::call(0x023FE7C0 /* checkLavaFace */, b, 0, attr);
                r0 = LK_FIELD(u32, 0x3B8);
            } else {
                r0 = LK_FIELD(u32, 0x3B8);
                if (r0 & 0x20000) {
                    u32 rr = LK_FIELD(u32, 0x3C0);
                    r0 = LK_FIELD(u32, 0x3B8);
                    LK_FIELD(u32, 0x3C0) = rr | 0x40; /* daPyRFlg0_AUTO_JUMP_LAND */
                }
                r0 &= ~0x20000u;
                if (r0 & 0x400000) {
                    u32 rr = LK_FIELD(u32, 0x3C0);
                    LK_FIELD(u32, 0x3B8) = r0;
                    LK_FIELD(u32, 0x3C0) = rr | 0x200; /* daPyRFlg0_ROPE_JUMP_LAND */
                }
                f32 py = LK_FIELD(f32, 0x318);
                r0 &= ~0x400000u;
                LK_FIELD(u32, 0x3B8) = r0;
                if (py < 2000.0f) {
                    if (gabi::call<s32>(0x024EF09C /* dBgS::GetSpecialCode */, dComIfG_Bgsp(), b + 0x8F4) != 1 &&
                        gabi::load<f32>(gabi::call<u32>(0x020084C8 /* cBgS::GetTriPla */, dComIfG_Bgsp(), (u32)LK_FIELD(u16, 0x8F6),
                                                        (u32)LK_FIELD(u16, 0x8F4)) + 4) >= 0.5f) {
                        r0 = LK_FIELD(u32, 0x3B8) & ~0x10u; /* offNoResetFlg0(DEKU_SP_RETURN_FLG) */
                        LK_FIELD(u32, 0x3B8) = r0;
                    } else {
                        r0 = LK_FIELD(u32, 0x3B8);
                    }
                }
            }
            if (!(r0 & 0xA0000000)) {
                if (gabi::call<BOOL>(0x024EEABC /* dBgS::ChkMoveBG */, dComIfG_Bgsp(), b + 0x8F4)) {
                    gabi::call(0x024EFA38 /* dBgS::MoveBgTransPos */, dComIfG_Bgsp(), b + 0x8F4, 1, b + 0x732C, 0, 0);
                }
                r0 = LK_FIELD(u32, 0x3B8);
            }
            if (r0 & 0x80000000) {
                LK_FIELD(u32, 0x6A74) = 0; /* mMtrlSndId */
                LK_FIELD(s16, 0x692A) = 0; /* m34E2 */
            } else {
                LK_FIELD(u32, 0x6A74) = gabi::call<u32>(0x024EECAC /* dBgS::GetMtrlSndId */, dComIfG_Bgsp(), b + 0x8F4);
                s16 ga = gabi::call<s16>(0x023E7D6C /* getGroundAngle */, b, b + 0x8F4, (s32)LK_FIELD(s16, 0x32A));
                LK_FIELD(s16, 0x692A) = ga;
            }
            if (LK_FIELD(u32, 0x6A70) & 0x02000000) {
                gabi::call(0x023FF020 /* setShapeAngleOnGround */, b);
            }
        } else {
            if ((mode & 0x40000) && LK_FIELD(s32, 0x69D0) == 4) {
                gabi::call(0x023FD4E4 /* startRestartRoom */, b, 5, 0xC9, -1.0f, 0);
            }
            s32 p = LK_FIELD(s32, 0x65F0);
            LK_FIELD(s16, 0x692A) = 0;
            if (p == 0xA9) {
                LK_FIELD(u32, 0x6A74) = gabi::call<u32>(0x024EECAC /* dBgS::GetMtrlSndId */, dComIfG_Bgsp(), b + 0x8F4);
            } else if (p == 0x86 || p == 0x90) {
                LK_FIELD(u32, 0x6A74) = 9;
            } else {
                LK_FIELD(u32, 0x6A74) = 0;
            }
        }
        u32 se = LK_FIELD(u32, 0x6A94); /* mpSeAnmFrameCtrl */
        if (se != 0 && !gabi::call<BOOL>(0x025E20F8)) {
            u32 nm = gabi::load<u32>(gabi::load<u32>(LK_FIELD(u32, 0x77FC) + 8) + 4);
            u32 r9 = nm != 0 ? gabi::load<u32>(LK_FIELD(u32, 0x77FC) + 8) + 4 + nm : 0;
            gabi::Local<lk_SafeString_l> s;
            s->mStr = r9;
            s->__vtbl = LK_SAFESTRING_VTBL;
            gabi::call(0x02444F48, s.get());
            s8 rev = LK_FIELD(s8, 0x68DB);
            u32 sc = LK_FIELD(u32, 0x6A94);
            gabi::call(0x0201BF08 /* JAIZelAnime::setAnimSound */, b + 0x64F0, b + 0x314, (f32)gabi::load<f32>(sc + 4),
                       (f32)gabi::load<f32>(sc + 0), (u32)LK_FIELD(u32, 0x6A74), (s32)rev);
        }
        u8 st = LK_FIELD(u8, 0x68DF); /* m34C3 */
        if (!(LK_FIELD(u32, 0x6A70) & 0x10452822) /* !checkPlayerFly() */ && LK_FIELD(s32, 0x65F0) != 0xA9 &&
            (st == 1 || st == 4 || (st >= 9 && st <= 10))) {
            gabi::call(0x023FF47C /* setStepsOffset */, b);
        }
        u8 grp = gabi::call<u8>(0x024EEDB8 /* dBgS::GetGrpSoundId */, dComIfG_Bgsp(), b + 0x8F4);
        gabi::call(0x025E1DF4 /* mDoAud_setLinkGroupInfo */, (u32)grp);
    } else {
        u32 c3580 = LK_FIELD(u32, 0x69D0);
        LK_FIELD(s16, 0x692A) = 0;
        LK_FIELD(u32, 0x69D4) = 0x1B; /* dBgS_Attr_UNK1B_e */
        LK_FIELD(u32, 0x69CC) = c3580;
        LK_FIELD(u32, 0x6A74) = 0;
        LK_FIELD(u32, 0x69D0) = 0xFFFFFFFF;
        LK_FIELD(u32, 0x69E4) = 0xFF; /* mRestartPoint */
        roomNo = LK_FIELD(s8, 0x326);
        gabi::call(0x023FEA6C /* checkFallCode */, b);
    }
    {
        u32 eq = LK_FIELD(u32, 0x6594);
        if (eq != 0) {
            gabi::store<u8>(eq + 0x1C9, LK_FIELD(u8, 0x1C9)); /* tevStr.mRoomNo */
            u8 ov = LK_FIELD(u8, 0x1CA);
            gabi::store<u8>(eq + 0x326, (u8)roomNo);
            gabi::store<u8>(eq + 0x1CA, ov); /* tevStr.mEnvrIdxOverride */
        }
    }
    gabi::call(0x023FF6A0 /* setWorldMatrix */, b);
    gabi::call(0x023FFB08 /* setWaistAngle */, b);
    if (gabi::load<u8>(LK_FIELD(u32, 0x65CC)) != 0) { /* m_old_fdata->getOldFrameFlg() */
        gabi::call(0x0240005C /* footBgCheck */, b);
    }
    if (LK_FIELD(s32, 0x65F0) != 0x66 /* daPyProc_DAMAGE_e */) {
        gabi::call(0x0200F378 /* cLib_addCalcAngleS */, b + 0x3D4, 0, 4, 0xC00, 0x180);
        if (LK_FIELD(s32, 0x65F0) != 0x93) {
            gabi::call(0x02400990 /* setMoveSlantAngle */, b);
        }
        if (!(LK_FIELD(u32, 0x6A70) & 0x20000000)) { /* ModeFlg_SUBJECT */
            gabi::call(0x0200F378 /* cLib_addCalcAngleS */, b + 0x3D0, 0, 4, 0xC00, 0x180);
        }
        if (!(LK_FIELD(u32, 0x6A70) & 0x40000000)) {
            gabi::call(0x0200F378 /* cLib_addCalcAngleS */, b + 0x3D2, 0, 4, 0xC00, 0x180);
        }
    }
    gabi::call(0x02400C4C /* setNeckAngle */, b);
    gabi::call(0x024022D0 /* setHatAngle */, b);
    /* HD: the warp / pitfall events shrink Link (global scale at 0x101CEEAC) and pull him to the event actor */
    f32 g;
    if (gabi::call<BOOL>(0x025445B8 /* dEvent_manager_c::startCheckOld */, dComIfGp_ea() + 0x52C4, 0x10035B60u /* "DEFAULT_WARP" */) ||
        gabi::call<BOOL>(0x025445B8, dComIfGp_ea() + 0x52C4, 0x10035AF0u /* "DEFAULT_PITFALL" */)) {
        gabi::store<f32>(0x101CEEAC, (f32)((f64)gabi::load<f32>(0x101CEEAC) * 0.95));
        u32 play = dComIfGp_ea();
        u32 ac = gabi::call<u32>(0x0253EE04 /* dEvt_control_c::convPId */, play + 0x51D0, gabi::load<u32>(play + 0x5294));
        if (ac != 0) {
            gabi::Local<cXyz> t;
            fcpy_l(gabi::ea(t.get()) + 0, ac + 0x314);
            fcpy_l(gabi::ea(t.get()) + 4, ac + 0x318);
            fcpy_l(gabi::ea(t.get()) + 8, ac + 0x31C);
            gabi::call(0x0200EF78 /* cLib_addCalcPosXZ */, b + 0x314, t.get(), 5.0f, 5.0f, 0.5f);
        }
        g = gabi::load<f32>(0x101CEEAC);
    } else {
        g = 1.0f;
        gabi::store<f32>(0x101CEEAC, 1.0f);
    }
    {
        static const u16 models[] = {0x448, 0xE7C, 0xCF4, 0x44C, 0x4440, 0xD00, 0xCFC, 0xE88, 0xE84, 0x4434};
        for (u32 k = 0; k < sizeof(models) / sizeof(models[0]); k++) {
            u32 m = gabi::load<u32>(b + models[k]);
            if (m != 0) { /* setBaseScale(g) */
                gabi::store<f32>(m + 0xC0, g);
                gabi::store<f32>(m + 0xBC, g);
                gabi::store<f32>(m + 0xC4, g);
            }
        }
    }
    gabi::call(0x023FC06C, b);
    gabi::call(0x02402FFC /* checkOriginalHatAnimation */, b);
    gabi::call(0x027F4D5C /* J3DModel::calc */, (u32)LK_FIELD(u32, 0x448));
    gabi::call(0x02403150 /* jointCB1 */, b);
    gabi::call(0x025E3EC8 /* mDoExt_MtxCalcOldFrame::decOldFrameMorfCounter */, (u32)LK_FIELD(u32, 0x65CC));
    gabi::call(0x028E8F64 /* PSMTXMultVec */, lk_getAnmMtx(mpCLModel, 15), 0x10035B54u /* head_offset */, b + 0x3D8); /* mHeadTopPos */
    gabi::call(0x024036E4 /* checkRoofRestart */, b);
    lk_jntTrans(gabi::ea(lk_getAnmMtx(mpCLModel, 8)), b + 0x3F0); /* mLeftHandPos */
    lk_jntTrans(gabi::ea(lk_getAnmMtx(mpCLModel, 12)), b + 0x3FC); /* mRightHandPos */
    gabi::call(0x028E8F64 /* PSMTXMultVec */, lk_getAnmMtx(mpCLModel, 0), 0x10035B18u /* boomerang_catch */, b + 0x72FC);
    gabi::call(0x028E8F64 /* PSMTXMultVec */, lk_getAnmMtx(mpCLModel, 8), 0x10035B24u /* hookshot_root */, b + 0x72F0);
    lk_setBaseTRMtx(LK_FIELD(u32, 0x44C), gabi::ea(lk_getAnmMtx(mpCLModel, 15)));
    gabi::call(0x027F4D5C /* J3DModel::calc */, (u32)LK_FIELD(u32, 0x44C));
    lk_setBaseTRMtx(LK_FIELD(u32, 0x450), gabi::ea(lk_getAnmMtx(mpCLModel, 15)));
    gabi::call(0x027F4D5C /* J3DModel::calc */, (u32)LK_FIELD(u32, 0x450));
    gabi::call(0x02403C24 /* setItemModel */, b);
    {
        s32 p = LK_FIELD(s32, 0x65F0);
        if (p == 0x42 || p == 0x63) { /* CUT_F / BT_VERTICAL_JUMP_CUT */
            bool normal = lk_selectSword() == 0x38 || lk_playU8(0x5CEA) == 2;
            u32 m = gabi::ea(lk_getAnmMtx(gabi::at<J3DModel>(LK_FIELD(u32, 0x4440)), normal ? 3 : 4));
            lk_setBaseTRMtx(LK_FIELD(u32, 0x497C) /* mpSwordTipStabModel */, m);
        }
    }
    gabi::call(0x028E8F64 /* PSMTXMultVec */, lk_getAnmMtx(mpCLModel, 19 /* CL_JNT_CL_EYE_e */), 0x101CEBA8u /* l_eye_offset */, b + 0x37C);
    {
        u32 t0 = LK_FIELD(u32, 0x3E4), c0 = LK_FIELD(u32, 0x72CC);
        LK_FIELD(u32, 0x72D8) = t0; /* m36D0 = mSwordTopPos */
        u32 t1 = LK_FIELD(u32, 0x3E8);
        LK_FIELD(u32, 0x72DC) = t1;
        u32 t2 = LK_FIELD(u32, 0x3EC);
        LK_FIELD(u32, 0x72E4) = c0; /* m36DC = m36C4 */
        LK_FIELD(u32, 0x72E0) = t2;
        u32 c2 = LK_FIELD(u32, 0x72D4), c1 = LK_FIELD(u32, 0x72D0);
        LK_FIELD(u32, 0x72EC) = c2;
        LK_FIELD(u32, 0x72E8) = c1;
    }
    {
        u32 item = LK_FIELD(u16, 0x69B0);
        if (item == 0x101 && LK_FIELD(u32, 0x6594) != 0) {
            u32 boko = LK_FIELD(u32, 0x6594);
            gabi::call(0x020C4084 /* daBoko_c::getTopPos */, boko, b + 0x3E4);
            gabi::call(0x020C4128 /* daBoko_c::getBlurRootPos */, boko, b + 0x72CC);
        } else if (item == 0x33) {
            u32 m = LK_FIELD(u32, 0x4440);
            gabi::call(0x028E8F64 /* PSMTXMultVec */, m != 0 ? m + 0xC8 : 0, 0x10035B30u /* hammer_top */, b + 0x3E4);
            m = LK_FIELD(u32, 0x4440);
            gabi::call(0x028E8F64 /* PSMTXMultVec */, m != 0 ? m + 0xC8 : 0, 0x10035B3Cu /* hammer_root */, b + 0x72CC);
        } else if (item == 0x103) {
            if (lk_selectSword() == 0x38 || lk_playU8(0x5CEA) == 2) {
                gabi::call(0x028E8F64 /* PSMTXMultVec */, lk_getAnmMtx(gabi::at<J3DModel>(LK_FIELD(u32, 0x4440)), 1), 0x10035B00u /* nsword_top */, b + 0x3E4);
                lk_jntTrans(gabi::ea(lk_getAnmMtx(gabi::at<J3DModel>(LK_FIELD(u32, 0x4440)), 2)), b + 0x72CC);
            } else {
                lk_jntTrans(gabi::ea(lk_getAnmMtx(gabi::at<J3DModel>(LK_FIELD(u32, 0x4440)), 1)), b + 0x72CC);
                gabi::call(0x028E8F64 /* PSMTXMultVec */, lk_getAnmMtx(gabi::at<J3DModel>(LK_FIELD(u32, 0x4440)), 1), 0x10035B0Cu /* msword_top */, b + 0x3E4);
            }
        } else if (LK_FIELD(s32, 0x65F0) == 0x92 /* daPyProc_FAN_SWING_e */) {
            u32 m = LK_FIELD(u32, 0x4440);
            lk_jntTrans(m != 0 ? m + 0xC8 : 0, b + 0x72CC);
            gabi::call(0x028E8F64 /* PSMTXMultVec */, lk_getAnmMtx(gabi::at<J3DModel>(LK_FIELD(u32, 0x4440)), 6), 0x10035B48u /* fan_top */, b + 0x3E4);
        } else {
            lk_jntTrans(gabi::ea(lk_getAnmMtx(mpCLModel, 13)), b + 0x72CC);
            lk_jntTrans(gabi::ea(lk_getAnmMtx(mpCLModel, 13)), b + 0x3E4);
        }
    }
    gabi::call(0x0240584C /* checkLightHit */, b);
    gabi::call(0x02406480 /* setFootEffect */, b);
    gabi::call(0x024076CC /* setCollision */, b);
    gabi::call(0x0240A0A4 /* setAttentionPos */, b);
    gabi::call(0x0240A41C /* setGrabItemPos */, b);
    gabi::call(0x0240AB88 /* setWaterRipple */, b);
    gabi::call(0x0240B7F4 /* setAuraEffect */, b);
    gabi::call(0x0240C0A8 /* setHammerWaterSplash */, b);
    gabi::call(0x0240C32C /* setLightSaver */, b);
    {
        u16 cnd = 0;
        bool none = false;
        if (!(LK_FIELD(u32, 0x3B8) & 0xA0000000)) {
            if ((LK_FIELD(u32, 0x834) & 0x20) && !(LK_FIELD(u32, 0x6A70) & 0x10452822)) {
                cnd = 0x5D;
            } else if ((gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & 0x10000) ||
                       ((LK_FIELD(u32, 0x6A70) & 0x40000) && (LK_FIELD(u32, 0x3B8) & 0x100))) {
                cnd = 1;
            } else {
                none = true;
            }
        } else {
            none = true;
        }
        if (none) {
            LK_FIELD(u16, 0xFA) = LK_FIELD(u16, 0xFA) | 8; /* eventInfo.onCondition(dEvtCnd_CANGETITEM_e) */
        } else {
            LK_FIELD(u16, 0xFA) = (u16)((LK_FIELD(u16, 0xFA) | cnd) | 8);
        }
    }
    {
        u32 e = LK_FIELD(u32, 0x6714); /* mSmokeEcallBack.getEmitter() */
        if (e != 0) {
            gabi::call(0x028249B0 /* JPASetRMtxTVecfromMtx */, lk_getAnmMtx(mpCLModel, 35), e + 0x1F0, e + 0x22C);
        }
    }
    gabi::call(0x0240CBE0 /* setWaterDrop */, b);
    {
        s16 t = gabi::load<s16>(0x101CEF14); /* daPy_dmEcallBack_c::getTimer() */
        if (t > 0) {
            t = (s16)(t - 1);
            u16 type = gabi::load<u16>(0x101CEF16);
            gabi::store<s16>(0x101CEF14, t);
            if (type == 0) { /* checkFlame() */
                gabi::call(0x023DC0F4 /* seStartOnlyReverb */, b, 0x2041 /* JA_SE_LK_BURNING */);
                u32 m = gabi::ea(lk_getAnmMtx(mpCLModel, 0));
                LK_FIELD(f32, 0x68B0) = gabi::load<f32>(m + 0xC); /* mLightInfluence.mPos */
                LK_FIELD(f32, 0x68B4) = gabi::load<f32>(m + 0x1C);
                LK_FIELD(f32, 0x68C4) = 150.0f; /* mPower */
                LK_FIELD(f32, 0x68B8) = gabi::load<f32>(m + 0x2C);
                t = gabi::load<s16>(0x101CEF14);
            } else if (type == 1) { /* checkCurse() */
                gabi::call(0x023DC0F4 /* seStartOnlyReverb */, b, 0x2058 /* JA_SE_LK_CURSE_BURNING */);
                t = gabi::load<s16>(0x101CEF14);
            }
            if (t == 0) {
                gabi::call(0x023DCA08 /* endDamageEmitter */, b);
            }
        }
    }
    LK_FIELD(u32, 0x3BC) = LK_FIELD(u32, 0x3BC) & 0xFDFFFFFD;
    if (lk_playU8(0x5292) != 0 || gabi::load<u32>(dComIfGp_ea() + 0x5B2C) != b) {
        lk_playSetU8(0x5BB7, 0);
        lk_playSetU8(0x5BB5, 0);
    } else {
        if (lk_playU8(0x5BB7) == 0x51) {
            lk_playSetU8(0x5BB7, 0x12);
        }
        if (LK_FIELD(u32, 0x3C0) & 0x10000000) {
            lk_playSetU8(0x5BB6, 0);
        }
    }
    LK_FIELD(u32, 0x3BC) = LK_FIELD(u32, 0x3BC) & 0xEFFEFFEB;
    if (gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & 0x10000) {
        gabi::call(0x025E1E44 /* mDoAud_setLinkOnBoard */, 1);
        LK_FIELD(u32, 0x2E0) = LK_FIELD(u32, 0x2E0) | 0x4000;
    } else {
        gabi::call(0x025E1E44 /* mDoAud_setLinkOnBoard */, 2);
        LK_FIELD(u32, 0x2E0) = LK_FIELD(u32, 0x2E0) & ~0x4000u;
    }
    {
        u32 save = gabi::load<u32>(0x101F84DC);
        gabi::call(0x025E1CE4 /* mDoAud_setLinkHp */, (u32)gabi::load<u16>(save + 0x22), (u32)gabi::load<u16>(save + 0x20));
    }
    LK_FIELD(s16, 0x6996) = (s16)(LK_FIELD(s16, 0x32A) + LK_FIELD(s16, 0x3D2)); /* mShieldFrontRangeYAngle */
    {
        u32 f1 = LK_FIELD(u32, 0x3BC);
        LK_FIELD(u32, 0x3B4) = 0xAD; /* mFace = daPyFace_NONE */
        if (f1 & 0x100) { /* checkConfuse() */
            gabi::call(0x023E0470 /* seStartMapInfo */, b, 0x107F /* JA_SE_LK_NOW_CURSE_PW */);
        }
    }
    LK_FIELD(u32, 0x6A8C) = 0xFFFFFFFF; /* mWhirlId */
    for (u32 k = 0; k < 3; k++) {
        gabi::store<u32>(0x1046CD48 + k * 4, gabi::load<u32>(b + 0x314 + k * 4));
    }
    for (u32 k = 0; k < 3; k++) {
        gabi::store<u16>(0x1046CD10 + k * 2, gabi::load<u16>(b + 0x328 + k * 2));
    }
    for (u32 k = 0; k < 3; k++) {
        gabi::store<u16>(0x1046CD08 + k * 2, gabi::load<u16>(b + 0x320 + k * 2));
    }
    return TRUE;
}
VERIFY(0x0240CDD0, &daPy_lk_c::execute);
