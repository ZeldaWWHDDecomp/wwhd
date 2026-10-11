/**
 * d_a_player_main_04.cpp (WWHD)
 * Player - Link (daPy_lk_c), address range #04, first half (02400B14..0240EE2B): neck/hat angles,
 * joint callback, item models, foot effects, collision, attention, water/aura effects, execute.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_player_main.cpp and its .inc files) to the WWHD layout and code,
 * verified against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * The first part covers attention/demo look targets, neck/hat pose, leg joints and roof restart.
 * Its private support header exposes the HD blocks used by those seven entries.
 * The second part covers item models, foot effects, light and sword collision, and blur history.
 * The third part covers collision setup, attention/item positions, water/aura effects,
 * the frame update and player teardown. Its private views preserve HD callback and renderer tails.
 */
#include "d/actor/d_a_player_main_04.h"
#include "d/actor/d_a_player_main_04_item.h"
#include "d/actor/d_a_player_main_04_effect.h"
static cXyz l_attention_crawl_offset = {0.0f, 30.0f, 20.0f};
static cXyz l_attention_subject_offset = {0.0f, 70.0f, 0.0f};
static cXyz l_hammer_splash_particle_scale;
static cXyz l_sword_d_scale;
static cXyz l_sword_p_scale;
static u32 l_sword_d_scale_initialized;
static u32 l_sword_p_scale_initialized;
static GXColor l_sword_g_prm0 = {0x09, 0x14, 0x06, 0xFF};
static GXColor l_sword_g_env0 = {0x0C, 0x18, 0x00, 0xFF};
static GXColor l_sword_y_prm0 = {0x14, 0x12, 0x0C, 0xFF};
static GXColor l_sword_y_env0 = {0x18, 0x18, 0x00, 0xFF};
static GXColor l_sword_g_prm1 = {0x80, 0xFF, 0x64, 0xFF};
static GXColor l_sword_y_prm1 = {0xFF, 0xFF, 0xC8, 0xFF};
static GXColor l_sword_y_env1 = {0xA5, 0xA5, 0x10, 0xFF};
static GXColor l_sword_s_prm1 = {0xC8, 0xC8, 0xFF, 0xFF};
static GXColor l_sword_s_env1 = {0x60, 0x60, 0xA5, 0xFF};

#include "d/actor/d_a_player_main_04_aura.h"

static cXyz l_normal_ripple_scale;
static cXyz l_small_ripple_scale;
static cXyz l_demo07_ripple_pos;
static cXyz l_waterfall_splash_trans;
static u32 l_normal_ripple_initialized;
static u32 l_small_ripple_initialized;
static u32 l_demo07_ripple_initialized;
static u32 l_waterfall_splash_initialized;
static LinkAuraSavePointer l_aura_save;
static u8 l_water_particle_status;
static cXyz l_wave_offset = {0.0f, 18.0f, 0.0f};
static cXyz l_swim_offset = {0.0f, 30.0f, 0.0f};
static cXyz l_swim_side_offset = {0.0f, 0.0f, 30.0f};
static cXyz l_waterfall_ripple_scale = {0.75f, 0.75f, 0.75f};
static LinkAuraSingleRippleCallback l_waterfall_ripple_callback;
// HD: immutable view of the TU's joint table; no second TU-static definition.
static const be<u16>* const aura_model_joint = gabi::at<be<u16>>(0x10035AA0);
#include "d/actor/d_a_player_main_04_water.h"
#include "d/actor/d_a_player_main_04_collision.h"
#include "d/actor/d_a_player_main_04_update.h"

static cXyz l_debug_keep_pos;
static csXyz l_debug_shape_angle;
static csXyz l_debug_current_angle;
static u8 l_maba_timer;
static GXColor l_freeze_fade_color;
static f32 l_event_link_scale;
static s16 l_damage_emitter_timer;
static cXyz l_execute_nsword_top;
static cXyz l_execute_msword_top;
static cXyz l_execute_boomerang_catch;
static cXyz l_execute_hookshot_root;
static cXyz l_execute_hammer_top;
static cXyz l_execute_hammer_root;
static cXyz l_execute_fan_top;
static cXyz l_execute_head_offset;

static u16 l_damage_emitter_type;
static LinkWaterParticleCallback l_waterDropPcallBack;

static f32 l_boko_jump_blur_rate[6] = {0.0f, -0.10000000149011612f, 0.4000000059604645f, 0.5f, 1.399999976158142f, 0.5f};
static f32 l_boko_blur_rate[6] = {0.0f, -0.10000000149011612f, 0.10000000149011612f, 0.20000000298023224f, 1.0f, 0.20000000298023224f};
static cXyz l_light_normal_scale;
static cXyz l_light_boss_scale;
static u32 l_light_normal_scale_initialized;
static u32 l_light_boss_scale_initialized;
static cXyz l_run_splash_scale;
static cXyz l_run_grass_scale;
static cXyz l_heavy_emit_smoke_scale;
static cXyz l_heavy_dyn_smoke_scale;
static cXyz l_heavy_pat_smoke_scale;
static u32 l_run_splash_scale_initialized;
static u32 l_run_grass_scale_initialized;
static u32 l_heavy_emit_smoke_scale_initialized;
static u32 l_heavy_dyn_smoke_scale_initialized;
static u32 l_heavy_pat_smoke_scale_initialized;
static cXyz l_foot_grass_scale;
static cXyz l_foot_smoke_scale;
static cXyz l_ms_light_local_vec;

static const cXyz l_head_center_offset = {0.0f, 0.0f, 0.0f};
static const cXyz l_eye_offset = {0.0f, 0.0f, 0.0f};
static const cXyz l_neck_front = {0.0f, 1.0f, 0.0f};
static const cXyz l_neck_top = {1.0f, 0.0f, 0.0f};
static const cXyz l_tact_top = {38.0f, 7.5f, 12.5f};
static cXyz l_baseZ;
static u8 m_eye_move_flg;
static u8 m_morf_frame;

// HD: Link's voice remains a virtual dispatch through the original vtable slot.
static inline void LinkPose_voiceStart(daPy_lk_c* player, u32 voice) {
    gabi::call_ptr(gabi::load<u32>(player->__vtbl + 0xE4), player, voice);
}
static inline void LinkPose_transQuatM(LinkPoseTransformInfo* transInfo, Quaternion* quat, s32 joint) {
    cXyz& translation = transInfo[joint].mTranslate;
    mDoMtx_stack_c::transM(translation.x, translation.y, translation.z);
    mDoMtx_stack_c::quatM(&quat[joint]);
}

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
    if (actor) {
        s16 targetAngle = cLib_targetAngleY(&current.pos, &actor->eyePos);
        int angleDiff = cLib_distanceAngleS(targetAngle, m34DE);
        if (angleDiff <= 0x6000) {
            // HD: the output is a guest pointer slot, with the original word-sized store.
            *reinterpret_cast<gptr<cXyz>*>(pOutPos) = &actor->eyePos;
            if (actor->group == fopAc_ENEMY_e) {
                onNoResetFlg1(daPyFlg1_UNK400);
            }
            return TRUE;
        }
    }
    return FALSE;
}

/* 02400BC4 */
fopAc_ac_c* daPy_lk_c::getDemoLookActor() {
    if (mDemo.getParam0() == 1) {
        return dComIfGp_event_getPt1();
    } else if (mDemo.getParam0() == 2) {
        return dComIfGp_event_getPt2();
    } else if (mDemo.getParam0() == 3) {
        return LinkPoseApi::fopAcM_getTalkEventPartner(this);
    }
    return NULL;
}

/* 02402FFC */
void daPy_lk_c::checkOriginalHatAnimation() {
    if ((mModeFlg & (LinkPoseConstants::ModeFlg_WHIDE | LinkPoseConstants::ModeFlg_SWIM |
                     LinkPoseConstants::ModeFlg_CRAWL)) ||
        mCurProc == LinkPoseConstants::daPyProc_VOMIT_WAIT_e ||
        mCurProc == LinkPoseConstants::daPyProc_ELEC_DAMAGE_e ||
        mCurProc == LinkPoseConstants::daPyProc_DEMO_DOOR_OPEN_e ||
        (lk_isStartStage(gabi::guest_str("GTower")) && dComIfGp_getStartStageLayer() == 9)) {
        onResetFlg0(daPyRFlg0_ORIGINAL_HAT_ANIM);
    }
}

// HD: gamma conversion maps particle color channels to linear light.
/* 02406D60 */
static f32 lk_colorGamma(u32 c) {
    f32 in = (f32)c / 255.0f;
    f32 out = LinkItemApi::powf(in, 2.2f);
    if (out < 0.0f) {
        return 0.0f;
    }
    if (out > 1.0f) {
        return 1.0f;
    }
    return out;
}

/* 02406DE0 */
void daPy_lk_c::setCutWaterSplash() {
    if (LinkItem(this).m336C.mpEmitter != nullptr) { /* m336C.getEmitter() */
        gabi::Local<cXyz> pos;
        Mtx34* mtx = lk_getAnmMtx(mpCLModel, LinkItemConstants::CL_JNT_CHEST_JNT_e);
        pos->x = mtx->m[0][3]; /* mDoMtx_multVecZero */
        pos->y = mtx->m[1][3];
        pos->z = mtx->m[2][3];
        /* dComIfGp_particle_setP1(dPa_name::ID_IT_JN_LK_NURE_SHIBUKI00, &pos) */
        dPa_control_set(dComIfGp_getParticle(), 1, 0x39, pos, nullptr, nullptr, 0xFF, nullptr, -1, nullptr,
                        nullptr, nullptr);
    }
}

/* 02407178 */
f32 daPy_lk_c::getBlurTopRate() {
    s32 proc = mCurProc;
    if (proc == LinkItemConstants::daPyProc_DEMO_LAST_COMBO_e) {
        return 0.0f;
    }
    if (mEquipItem == LinkItemConstants::daPyItem_SWORD_e) {
        if (lk_checkNormalSwordEquip()) {
            return 0.5f;
        } else {
            return 1.0f;
        }
    }
    if (mEquipItem == LinkItemConstants::daPyItem_BOKO_e) {
        fopAc_ac_c* boko = mActorKeepEquip.mActor;
        if (boko == NULL) {
            return 0.0f;
        }
        if (proc == LinkItemConstants::daPyProc_JUMP_CUT_e ||
            proc == LinkItemConstants::daPyProc_JUMP_CUT_LAND_e) {
            return l_boko_jump_blur_rate[fopAcM_GetParam(boko)]; /* daBoko_c::getJumpBlurRate() */
        } else {
            return l_boko_blur_rate[fopAcM_GetParam(boko)]; /* daBoko_c::getBlurRate() */
        }
    }
    return 0.0f;
}

/* 02407270 */
int daPy_lk_c::getSwordBlurColor() {
    if (mEquipItem == LinkItemConstants::daPyItem_SWORD_e) {
        if (checkChanceMode()) {
            return 2;
        }
        if (noResetFlg1() & LinkItemConstants::daPyFlg1_SOUP_POWER_UP) {
            return 1;
        }
    }
    return 0;
}

/* 02407678 */
BOOL daPy_lk_c::fanWindCrashEffectDraw() {
    f32 end = (f32)LinkItem(this).mpYbafo00Btk.mFrameCtrl.mEnd - 0.5f;
    return !(LinkItem(this).mpYbafo00Btk.mFrameCtrl.mFrame > end);
}

/* 0240CA4C */
static void daPy_waterDropEcallBack_end(void* cb) {
    LinkWaterCallback& callBack = LinkWaterDrop(cb);
    if (callBack.mpEmitter != nullptr) {
        LinkWaterEmitter& emitter = LinkWaterEmitterView(callBack.mpEmitter);
        emitter.mStatus = emitter.mStatus & ~LinkWaterConstants::IMMORTAL_EMITTER;
        // HD: reload the callback's emitter after calls that can replace it.
        LinkWaterApi::deleteAllParticle(callBack.mpEmitter, 1);
        LinkWaterEmitterView(callBack.mpEmitter).mpParticleCallBack = nullptr;
        LinkItemApi::endFollow(&callBack);
    }
}


/* 0240EBF0 */
BOOL daPy_lk_c::playerDelete() {
    for (int i = 0; i < 2; i++) {
        LinkItemFootEffect& effect = LinkItem(this).mFootEffect[i];
        LinkUpdate_removeCallback(effect.mSmokeCb);
        LinkUpdate_removeCallback(effect.mFollowCb);
    }
    auto& fanSwing = *reinterpret_cast<LinkUpdateFanSwingCallback*>(mFanSwingCb);
    if (fanSwing.mpEmitter != nullptr) {
        auto* emitter = reinterpret_cast<LinkUpdateEmitter*>(fanSwing.mpEmitter.get());
        emitter->mFlags = emitter->mFlags & ~0x40u;
        // HD: reload the callback's emitter after clearing its flags.
        reinterpret_cast<LinkUpdateEmitter*>(fanSwing.mpEmitter.get())->mpEmitterCallback = nullptr;
        fanSwing.mpEmitter = nullptr;
    }
    LinkItemApi::endFollow(reinterpret_cast<LinkItemFollowCallback*>(m338C));
    LinkItemApi::endMtxFollow(reinterpret_cast<LinkItemMtxCallback*>(m33A8));
    LinkUpdateApi::endRipple(m3280);
    LinkUpdateApi::endWaterDrop(m334C);
    LinkUpdateApi::endWaterDrop(m336C);
    LinkUpdateApi::removeSwimTail(&LinkAura(this).mSwimTailEcallBack[0]);
    LinkUpdateApi::removeSwimTail(&LinkAura(this).mSwimTailEcallBack[1]);
    LinkItemApi::endMtxFollow(reinterpret_cast<LinkItemMtxCallback*>(&m32E4));
    LinkItemApi::endMtxFollow(reinterpret_cast<LinkItemMtxCallback*>(&m32F0));
    LinkItemApi::endMtxFollow(reinterpret_cast<LinkItemMtxCallback*>(&m33E8));
    LinkUpdate_removeCallback(mSmokeEcallBack);
    LinkUpdateApi::endCutTurn(m331C);
    LinkUpdateApi::endCutTurn(m332C);
    LinkUpdateApi::endCutTurn(m333C);
    LinkItemApi::endMtxFollow(reinterpret_cast<LinkItemMtxCallback*>(m3400));
    LinkItemApi::endFollow(&LinkItem(this).m3410);
    LinkItemApi::endFollow(&LinkItem(this).m3438);
    LinkItemApi::endMtxFollow(reinterpret_cast<LinkItemMtxCallback*>(&m342C));
    LinkItemApi::endMtxFollow(reinterpret_cast<LinkItemMtxCallback*>(&m3454));
    LinkItemApi::endMtxFollow(reinterpret_cast<LinkItemMtxCallback*>(m3460));
    LinkItemApi::endMtxFollow(reinterpret_cast<LinkItemMtxCallback*>(m3460 + 0x10));
    LinkUpdateApi::endDamageEmitter(this);
    dKy_plight_cut(reinterpret_cast<LIGHT_INFLUENCE*>(mLightInfluence));
    mDoAud_seDeleteObject(&mSwordTopPos);
    mDoAud_seDeleteObject(&mRopePos);
    mDoAud_seDeleteObject(&reinterpret_cast<LinkItemFollowCallback*>(m338C)->mPos);
    mDoAud_seDeleteObject(&reinterpret_cast<LinkItemCapsuleGeometry*>(mFanWindCps.mCps)->mEnd);
    for (int i = 0; i < 2; i++) {
        mDoExt_destroySolidHeap(LinkPose(this).m_anm_heap_under[i].mpAnimeHeap);
    }
    for (int i = 0; i < 3; i++) {
        mDoExt_destroySolidHeap(LinkPose(this).m_anm_heap_upper[i].mpAnimeHeap);
    }
    mDoExt_destroySolidHeap(LinkPose(this).m_tex_anm_heap.mpAnimeHeap);
    mDoExt_destroySolidHeap(reinterpret_cast<LinkPoseAnmHeap*>(m_tex_scroll_heap)->mpAnimeHeap);
    mDoExt_destroySolidHeap(gabi::at<JKRSolidHeap>(LinkItem(this).mpItemHeaps[0]));
    mDoExt_destroySolidHeap(gabi::at<JKRSolidHeap>(LinkItem(this).mpItemHeaps[1]));
    mDoExt_destroySolidHeap(gabi::object_at<LinkUpdateItemHeap>(gabi::ea(this)).mpItemAnimeHeap);
    LinkUpdateApi::stopAnimeSound(mJAIZelAnime);
    if (noResetFlg1() & daPyFlg1_CASUAL_CLOTHES) {
        // HD: restoring the other texture uses the renderer's texture swap operation.
        LinkUpdateApi::swapTexture(mpCurrLinktex, m48C);
    }
    dComIfGp_clearPlayerStatus0(0, 0x400000); // daPyStts0_BOOMERANG_WAIT_e
    dComIfGp_clearPlayerStatus1(0, 0x40000); // daPyStts1_UNK40000_e
    ((dComIfG_inf_c*)dComIfGp_get())->play.mMetronome = 0;
    cancelNoDamageMode();
    return TRUE;
}

/* 02405BC0 */
void daPy_lk_c::setFootMark(cXyz* i_pos) {
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
    f64 gy = LinkItemApi::groundCross(dComIfG_Bgsp(), mGndChk);
    pos->y = (f32)gy;
    if (gy != -1000000000.0 /* -G_CM3D_F_INF */ &&
        dBgS_GetAttributeCode_l(mGndChkPoly) == LinkItemConstants::dBgS_Attr_SAND_e) {
        LinkItemApi::createFootMark(LinkItemConstants::fpcNm_WATER_MARK_e,
                                    ((u32)(s32)shape_angle.y << 16) | 2, pos.get(), &scale, 0);
    }
    if (LinkItem(this).m336C.mpEmitter != nullptr) { /* m336C.getEmitter() */
        LinkItemApi::createFootMark(LinkItemConstants::fpcNm_WATER_MARK_e, (u32)(s32)shape_angle.y << 16,
                                    i_pos, &scale, 0);
    }
}

/* 02407070 */
void daPy_swBlur_c::initSwBlur(Mtx34* mtx, int idx, f32 blurTopRate, int blurColorType) {
    int i = idx * 2;
    mBlurTopRate = blurTopRate;
    mBlurColorType = blurColorType;
    PSMTXMultVec(mtx, mpPosBuffer.get() + i, &field_0x034[0]);
    PSMTXMultVec(mtx, mpPosBuffer.get() + i + 1, &field_0x304[0]);
    gabi::Local<cXyz> d;
    cXyz_mi(&field_0x034[0], d, &field_0x304[0]);
    gabi::Local<cXyz> s;
    cXyz_ml(d, s, mBlurTopRate);
    PSVECAdd(&field_0x034[0], s, &field_0x034[0]);
    field_0x034[1].copy(field_0x034[0]);
    field_0x304[1].copy(field_0x304[0]);
    field_0x014 = 0;
    field_0x018 = idx;
    u32 m = gabi::ea(mtx); /* field_0x028.set(mtx[0][3], mtx[1][3], mtx[2][3]) */
    fcpy_l(gabi::ea(&field_0x028) + 0, m + 0x0C);
    fcpy_l(gabi::ea(&field_0x028) + 8, m + 0x2C);
    fcpy_l(gabi::ea(&field_0x028) + 4, m + 0x1C);
}

/* 02407310 */
void daPy_swBlur_c::copySwBlur(Mtx34* mtx, int index) {
    int sampleCount = (s32)((u32)index - (u32)(s32)field_0x018);
    field_0x018 = index;
    int sampleStep;
    if (sampleCount > 0) {
        sampleStep = -2;
        if (sampleCount > 60) {
            sampleCount = 60;
        }
    } else if (sampleCount < 0) {
        sampleStep = 2;
        if (sampleCount < -60) {
            sampleCount = 60;
        } else {
            sampleCount = -sampleCount;
        }
    } else {
        sampleCount = 10;
        sampleStep = 0;
    }
    for (int i = 59 - sampleCount; i >= 0; i--) {
        field_0x034[i + sampleCount].copy(field_0x034[i]);
        field_0x304[i + sampleCount].copy(field_0x304[i]);
    }
    f32 interpolation = 0.0f;
    f32 frac = 1.0f / (f32)sampleCount;
    s32 buffIdx = (s32)((u32)(s32)field_0x018 << 1); /* wraps like the original */
    gabi::Local<cXyz> origin;
    u32 m = gabi::ea(mtx);
    fcpy_l(gabi::ea(origin.get()) + 4, m + 0x1C);
    fcpy_l(gabi::ea(origin.get()) + 0, m + 0x0C);
    fcpy_l(gabi::ea(origin.get()) + 8, m + 0x2C);
    gabi::Local<cXyz> d;
    cXyz_mi(&field_0x028, d, origin);
    gabi::Local<cXyz> originDelta;
    originDelta->copy(*d);
    field_0x028.copy(*origin);
    gabi::Local<cXyz> t;
    gabi::Local<cXyz> u;
    for (int i = 0; i < sampleCount && buffIdx >= 0; i++) {
        PSMTXMultVec(mtx, mpPosBuffer.get() + (u32)buffIdx, &field_0x034[i]);
        PSMTXMultVec(mtx, mpPosBuffer.get() + (u32)buffIdx + 1, &field_0x304[i]);
        cXyz_mi(&field_0x034[i], t, &field_0x304[i]);
        cXyz_ml(t, u, mBlurTopRate);
        PSVECAdd(&field_0x034[i], u, &field_0x034[i]);
        cXyz_ml(originDelta, t, interpolation);
        PSVECAdd(&field_0x034[i], t, &field_0x034[i]);
        cXyz_ml(originDelta, t, interpolation);
        PSVECAdd(&field_0x304[i], t, &field_0x304[i]);
        interpolation += frac;
        buffIdx = (s32)((u32)buffIdx + (u32)sampleStep);
    }
    s32 n = (s32)((u32)(s32)field_0x014 + (u32)sampleCount);
    if (n >= 59) {
        n = 58;
    }
    field_0x014 = n;
}

/* 024036E4 */
void daPy_lk_c::checkRoofRestart() {
    u32 fl = mAcch.m_flags;
    if (!(fl & dBgS_Acch::ROOF_HIT) || !(fl & dBgS_Acch::GROUND_HIT)) {
        return;
    }
    if (mCurProc == LinkPoseConstants::daPyProc_CRAWL_START_e ||
        mCurProc == LinkPoseConstants::daPyProc_CRAWL_END_e ||
        mCurProc == LinkPoseConstants::daPyProc_WHIDE_READY_e) {
        return;
    }
    LinkPoseStageInfo* stagInfo = reinterpret_cast<LinkPoseStageInfo*>(dComIfGp_getStageStagInfo());
    if (((stagInfo->mStageTypeAndSchbit >> 16) & 7) == LinkPoseConstants::dStageType_BOSS_e) {
        return;
    }
    // HD: this inline uses the first words of the roof-check object as polygon information.
    cBgS_PolyInfo* roofPoly = reinterpret_cast<cBgS_PolyInfo*>(&mAcch.m_roof);
    if (LinkPoseApi::ChkMoveBG(dComIfG_Bgsp(), roofPoly)) {
        fopAc_ac_c* actor = LinkPoseApi::GetActorPointer(dComIfG_Bgsp(), (u32)roofPoly->mBgIndex);
        if (actor != NULL && actor->mProcName == LinkPoseConstants::fpcNm_BRIDGE_e) {
            return;
        }
    }
    f32 roof_height = mAcch.m_roof_height; /* mAcch.GetRoofHeight() */
    if (roof_height > mAcch.m_ground_h /* mAcch.GetGroundH() */) {
        f32 y = current.pos.y;
        f32 dVar10 = mHeadTopPos.y /* mHeadTopPos.y */ - 10.0f;
        f32 lo = y + 10.0f;
        if (dVar10 < lo) {
            dVar10 = lo;
        }
        if (roof_height < dVar10) {
            if (mModeFlg & LinkPoseConstants::ModeFlg_WHIDE) {
                u16 a = (u16)shape_angle.y;
                f32 s = cM_ssin(a);
                f32 c = cM_scos(a);
                cXyz& roofPos = LinkPose(this).mRoofChk.m_pos;
                roofPos.y = y;
                // HD: the folded HIO offset is 26.5, rather than GameCube's 35.0.
                roofPos.x = gabi::fmadds(26.5f, s, current.pos.x);
                roofPos.z = gabi::fmadds(26.5f, c, current.pos.z);
                f64 r = LinkPoseApi::RoofChk(dComIfG_Bgsp(), &LinkPose(this).mRoofChk);
                if (!(r < (f64)dVar10)) {
                    return;
                }
            }
            if (LinkPoseApi::startRestartRoom(this, 5, 0xC9, -1.0f, 0)) {
                LinkPose_voiceStart(this, 43);
            }
        }
    }
}

/* 02406E74 */
void daPy_lk_c::setSwordAtCollision() {
    cXyz* swordTop = &mSwordTopPos; /* mSwordTopPos */
    gabi::Local<cXyz> local_34;
    gabi::Local<cXyz> local_58;
    if (mEquipItem == LinkItemConstants::daPyItem_SWORD_e) {
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
    LinkItem_setStartEnd(&mAtCps[0], &m36C4, local_34); /* mAtCps[0] */
    LinkItem_setStartEnd(&mAtCps[1], local_58, local_34);
    LinkItem_setStartEnd(&mAtCps[2], &m36C4, local_58);
    gabi::Local<cXyz> local_7c;
    if (resetFlg0() & LinkItemConstants::daPyRFlg0_UNK1) {
        local_7c->copy(*cXyz_Zero);
    } else {
        gabi::Local<cXyz> d;
        cXyz_mi(swordTop, d, &m36D0);
        local_7c->copy(*d);
    }
    LinkItem_setAtVec(&mAtCps[0], local_7c);
    LinkItem_setAtVec(&mAtCps[1], local_7c);
    LinkItem_setAtVec(&mAtCps[2], local_7c);
}

/* 0240CAAC */
void daPy_lk_c::setSwimWaterDrop(void* i_cb) {
    LinkWaterCallback& callBack = LinkWaterDrop(i_cb);
    f32 waterY = mWaterY;
    f32 tmp = waterY + 10.0f;
    u32 waterDrop = noResetFlg1() & LinkWaterConstants::daPyFlg1_WATER_DROP;
    if (waterDrop || !(mNoResetFlg0 & LinkItemConstants::daPyFlg0_UNK80) ||
        (callBack.mPos.y > tmp && !(mModeFlg & LinkPoseConstants::ModeFlg_SWIM))) {
        if (callBack.mpEmitter == nullptr && (callBack.field_0x1C != 0 || waterDrop)) {
            JPABaseEmitter* emitter = dPa_control_set(
                dComIfGp_getParticle(), 1, LinkWaterConstants::ID_IT_JN_LK_NURE_POTA00,
                &callBack.mPos, nullptr, nullptr, 0xFF,
                reinterpret_cast<dPa_levelEcallBack*>(&callBack), -1, nullptr, nullptr, nullptr);
            if (emitter != nullptr) {
                LinkWaterEmitterView(emitter).mpParticleCallBack = (&l_waterDropPcallBack);
            }
        }
        callBack.field_0x1C = 0;
        setNoResetFlg1(noResetFlg1() & ~LinkWaterConstants::daPyFlg1_WATER_DROP);
    } else if (callBack.mPos.y < waterY) {
        daPy_waterDropEcallBack_end(&callBack);
        if (mCurProc != LinkPoseConstants::daPyProc_VOMIT_WAIT_e) {
            callBack.field_0x1C = 1;
        }
    }
}


/* 0240CBE0 */
void daPy_lk_c::setWaterDrop() {
    Mtx34* neck_jnt_mtx = lk_getAnmMtx(mpCLModel, LinkWaterConstants::CL_JNT_NECK_JNT_e);
    LinkWaterCallback& neckDrop = LinkWaterDrop(m334C);
    fcpy_l(gabi::ea(&neckDrop.mPos.z), gabi::ea(&neck_jnt_mtx->m[2][3]));
    fcpy_l(gabi::ea(&neckDrop.mPos.y), gabi::ea(&neck_jnt_mtx->m[1][3]));
    fcpy_l(gabi::ea(&neckDrop.mPos.x), gabi::ea(&neck_jnt_mtx->m[0][3]));
    Mtx34* body_jnt_mtx = lk_getAnmMtx(mpCLModel, LinkWaterConstants::CL_JNT_BODY_CHN_e);
    LinkWaterCallback& bodyDrop = LinkWaterDrop(m336C);
    fcpy_l(gabi::ea(&bodyDrop.mPos.z), gabi::ea(&body_jnt_mtx->m[2][3]));
    fcpy_l(gabi::ea(&bodyDrop.mPos.y), gabi::ea(&body_jnt_mtx->m[1][3]));
    fcpy_l(gabi::ea(&bodyDrop.mPos.x), gabi::ea(&body_jnt_mtx->m[0][3]));
    (&l_waterDropPcallBack)->mbWaterMark = mModeFlg & LinkWaterConstants::ModeFlg_00000001;
    // HD: the verified flame-state branch tests zero, unlike the GameCube checkFlame wrapper.
    if (l_damage_emitter_type == 0) {
        daPy_waterDropEcallBack_end(&neckDrop);
        daPy_waterDropEcallBack_end(&bodyDrop);
        LinkItemApi::endMtxFollow(&LinkWaterRain(this, 0));
        LinkItemApi::endMtxFollow(&LinkWaterRain(this, 1));
        return;
    }
    if (LinkWaterApi::playerOverheadBgCheck()) {
        // The stage accessor performs the original virtual dispatch through the play object.
        LinkPoseStageInfo* stagInfo = reinterpret_cast<LinkPoseStageInfo*>(dComIfGp_getStageStagInfo());
        if (((stagInfo->mStageTypeAndSchbit >> 16) & 7) != LinkWaterConstants::dStageType_MISC_e &&
            mCurProc != LinkWaterConstants::daPyProc_DEMO_LAST_COMBO_e &&
            mAcch.m_roof_height == 1000000000.0f) {
            if (LinkWaterRain(this, 0).mpEmitter == nullptr) {
                Mtx34* nm = lk_getAnmMtx(mpCLModel, LinkWaterConstants::CL_JNT_NECK_JNT_e);
                LinkItemApi::makeMtxEmitter(&LinkWaterRain(this, 0),
                    LinkWaterConstants::ID_AK_JN_LK_NURE_POTA01, nm, &current.pos, 0);
            }
            if (LinkWaterRain(this, 1).mpEmitter == nullptr) {
                Mtx34* bm = lk_getAnmMtx(mpCLModel, LinkWaterConstants::CL_JNT_BODY_CHN_e);
                LinkItemApi::makeMtxEmitter(&LinkWaterRain(this, 1),
                    LinkWaterConstants::ID_AK_JN_LK_NURE_POTA01, bm, &current.pos, 0);
            }
            daPy_waterDropEcallBack_end(&neckDrop);
            daPy_waterDropEcallBack_end(&bodyDrop);
            return;
        }
    }
    LinkItemApi::endMtxFollow(&LinkWaterRain(this, 0));
    LinkItemApi::endMtxFollow(&LinkWaterRain(this, 1));
    setSwimWaterDrop(&neckDrop);
    setSwimWaterDrop(&bodyDrop);
}


/* 0240C0A8 */
void daPy_lk_c::setHammerWaterSplash() {
    if (mCurProc != LinkEffectConstants::daPyProc_HAMMER_FRONT_SWING_e || !(m35EC > mFrameCtrlUnder[0].getRate())) {
        return;
    }
    if (m355C != 0) {
        return;
    }
    cXyz* swordTop = &mSwordTopPos;
    gabi::Local<cXyz> t;
    cXyz_mi(&m36C4, t, swordTop);
    gabi::Local<cXyz> splashPos;
    splashPos->copy(*t);
    LinkEffectApi::normalize(splashPos.get(), t.get());
    gabi::Local<cXyz> s;
    cXyz_ml(splashPos, s, 30.0f);
    cXyz_pl(swordTop, t, s);
    splashPos->copy(*t);
    gabi::Local<be<f32>> waterY;
    if (!LinkEffectApi::getWaterY(splashPos.get(), waterY.get())) {
        return;
    }
    f32 y = splashPos->y;
    if (*waterY < y + 5.0f) {
        return;
    }
    f32 y2 = y + 150.0f;
    mGndChkPos.z = splashPos->z; /* mGndChk.SetPos(&splashPos) */
    mGndChkPos.x = splashPos->x;
    mGndChkPos.y = y2;
    splashPos->y = y2;
    f64 groundY = LinkItemApi::groundCross(dComIfG_Bgsp(), mGndChk);
    if (*waterY < (f32)(groundY + 5.0)) {
        return;
    }
    fcpy_l(gabi::ea(&splashPos->y), gabi::ea(waterY.get()));
    /* dComIfGp_particle_setP1(dPa_name::ID_IT_JN_HM_WP00, &splashPos) */
    dPa_control_set(dComIfGp_getParticle(), 1, LinkEffectConstants::ID_IT_JN_HM_WP00, splashPos, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
    /* dComIfGp_particle_setShipTail(dPa_name::ID_IT_JN_WP_HAMON01, .., &dPa_control_c::mSingleRippleEcallBack) */
    dPa_control_set(dComIfGp_getParticle(), 5, LinkEffectConstants::ID_IT_JN_WP_HAMON01, splashPos, nullptr, nullptr, 0xFF, LinkEffect_singleRippleCallback(), -1,
                    nullptr, nullptr, nullptr);
    gabi::Local<GXColor> amb;
    gabi::Local<GXColor> dif;
    LinkItemApi::getSeaColor(amb.get(), dif.get());
    JPABaseEmitter* emitter = dPa_control_set(dComIfGp_getParticle(), 1, LinkEffectConstants::ID_AK_JN_ELEMENTSHIBUKI00, splashPos, nullptr,
                                              nullptr, 0xFF, nullptr, -1, amb, nullptr, nullptr);
    if (emitter != NULL) {
        LinkItemEmitter& effect = LinkEmitter(gabi::ea(emitter));
        effect.mSpread = 1.0f;  /* setSpread(1.0f) */
        effect.mRate = 60.0f; /* setRate(60.0f) */
        effect.mMaxFrame = 1;     /* setMaxFrame(1) */
        effect.mDirectionalSpeed = 10.0f; /* setDirectionalSpeed(10.0f) */
        fcpy_l(gabi::ea(&effect.mGlobalParticleScale.x), gabi::ea(&l_hammer_splash_particle_scale.x));     /* setGlobalParticleScale(l_hammer_splash_particle_scale) */
        fcpy_l(gabi::ea(&effect.mGlobalParticleScale.y), gabi::ea(&l_hammer_splash_particle_scale.y));
        fcpy_l(gabi::ea(&effect.mGlobalParticleScale.z), gabi::ea(&l_hammer_splash_particle_scale.z));
    }
    m355C = 1;
}

/* 0240584C */
void daPy_lk_c::checkLightHit() {
    /* static JGeometry::TVec3<f32> normal_scale(1.0f, 1.0f, 1.0f), boss_scale(2.5f, 2.5f, 2.5f) (HD: on first
     * use) */
    u32 normal_scale = gabi::ea(&l_light_normal_scale);
    u32 boss_scale = gabi::ea(&l_light_boss_scale);
    if ((u32)l_light_normal_scale_initialized == 0) {
        l_light_normal_scale_initialized = 1;
        l_light_normal_scale.x = 1.0f;
        l_light_normal_scale.z = 1.0f;
        l_light_normal_scale.y = 1.0f;
    }
    if ((u32)l_light_boss_scale_initialized == 0) {
        l_light_boss_scale_initialized = 1;
        l_light_boss_scale.x = 2.5f;
        l_light_boss_scale.z = 2.5f;
        l_light_boss_scale.y = 2.5f;
    }
    BOOL lightHit = FALSE;
    if (LinkItemApi::checkLight(dComIfGp_ea() + 0x5A20, &current.pos)) {
        lightHit = TRUE;
        resetCurse();
    }
    u32 b = gabi::ea(this);
    bool doEnd = true;
    if (dComIfGs_getSelectEquip(1) == 0x3C /* checkMirrorShieldEquip() */) {
        gabi::Local<cXyz> lightVec;
        u32 shield = (u32)LinkItem(this).mpEquippedShieldModel.v; /* mpEquippedShieldModel->getBaseTRMtx() */
        PSMTXMultVecSR(gabi::at<Mtx34>(shield ? shield + 0xC8 : 0),
                       gabi::at<cXyz>(gabi::ea(&l_ms_light_local_vec)) /* l_ms_light_local_vec */, lightVec);
        if (lightHit) {
            setResetFlg0(resetFlg0() | 0x200000); /* daPyRFlg0_LIGHT_REFLECT */
        } else if (mLightCyl.ChkTgHit() && PSVECDotProduct(&mLightCyl.mGObjTg.mRVec, lightVec) < 0.0f) {
            setResetFlg0(resetFlg0() | 0x200000);
        }
        if (mFanLightCps.ChkAtHit()) {
            doEnd = false;
            if ((u32)LinkItem(this).m3438.mpEmitter.v == 0) { /* m3438.getEmitter() */
                /* dComIfGp_particle_setP1(dPa_name::ID_AK_SN_HITSHIELDLIGHT00, &current.pos, NULL, NULL,
                 * 0xFF, &m3438) */
                dPa_control_set(dComIfGp_getParticle(), 1, 0x8232, &current.pos, nullptr, nullptr, 0xFF,
                                gabi::at<dPa_levelEcallBack>(b + 0x684C), -1, nullptr, nullptr, nullptr);
            }
            LinkItemApi::lineVsPositionCross(
                &reinterpret_cast<LinkItemCapsuleGeometry*>(mFanLightCps.mCps)->mStart,
                &reinterpret_cast<LinkItemCapsuleGeometry*>(mFanLightCps.mCps)->mEnd,
                &mFanLightCps.mGObjAt.mHitPos, &LinkItem(this).m3438.mPos);
            gabi::Local<cXyz> xz; /* lightVec.absXZ() */
            xz->x = lightVec->x;
            xz->y = 0.0f;
            xz->z = lightVec->z;
            f32 d = std_sqrtf(PSVECSquareMag(xz));
            s16 ax = cM_atan2s(-lightVec->y, d);
            s16 angleY = cM_atan2s(lightVec->x, lightVec->z);
            LinkItem(this).m3438.mAngle.z = 0; /* m3438.setAngle(ax, angleY, 0) */
            LinkItem(this).m3438.mAngle.y = angleY;
            LinkItem(this).m3438.mAngle.x = ax;
            if ((u32)LinkItem(this).m3438.mpEmitter.v != 0) {
                u32 sc = normal_scale;
                if (gabi::ea(mFanLightCps.GetAtHitAc()) != 0) {
                    u32 ac = gabi::ea(mFanLightCps.GetAtHitAc());
                    if (ac != 0 && gabi::at<fopAc_ac_c>(ac)->mProcName == LinkItemConstants::fpcNm_BPW_e) {
                        sc = boss_scale;
                    }
                }
                u32 e =
                    (u32)LinkItem(this).m3438.mpEmitter.v; /* setGlobalScale (HD: also the particle scale) */
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
        LinkItemApi::endFollow(&LinkItem(this).m3438); /* m3438.end() */
    }
    if (resetFlg0() & LinkItemConstants::daPyRFlg0_LIGHT_REFLECT) {
        if (!(mFanLightCps.mObjAt.mSPrm & 1) /* !mFanLightCps.ChkAtSet() */) {
            seStartOnlyReverb(LinkItemConstants::JA_SE_OBJ_MIRROR_REFLECT);
        } else {
            seStartOnlyReverb(LinkItemConstants::JA_SE_OBJ_MIRROR_LIGHT);
        }
        resetCurse();
    }
    if (!(dComIfGp_getPlayerStatus0() & 0x2000) /* daPyStts0_SUBJECT_e */ && (resetFlg0() & 0x200000)) {
        if ((u32)LinkItem(this).m342C.mpEmitter.v == 0) { /* m342C.getEmitter() */
            u32 shield = (u32)LinkItem(this).mpEquippedShieldModel.v;
            LinkItemApi::makeMtxEmitter(&LinkItem(this).m342C, LinkItemConstants::ID_AK_SN_MIRRORSHIELD00,
                                        shield != 0 ? gabi::at<J3DModel>(shield)->getBaseTRMtx() : nullptr,
                                        &current.pos, 0);
        }
    } else if ((u32)LinkItem(this).m342C.mpEmitter.v != 0) {
        LinkEmitter(LinkItem(this).m342C.mpEmitter.v).mGlobalColor.a = 0; /* setGlobalAlpha(0) */
        LinkItemApi::endMtxFollow(&LinkItem(this).m342C);
    }
}

// HD: GHS reads the null-preserving base matrix address, including guest address zero.
static inline f32 LinkEffect_baseMatrixY(J3DModel* model) {
    u32 matrix = gabi::ea(model->getBaseTRMtx());
    return gabi::load<f32>(matrix + offsetof(Mtx34, m[1][3]));
}
/* 0240A0A4 */
void daPy_lk_c::setAttentionPos() {
    cXyz& attentionPos = attention_info.position;
    fcpy_l(gabi::ea(&attentionPos.x), gabi::ea(&current.pos.x));
    fcpy_l(gabi::ea(&attentionPos.z), gabi::ea(&current.pos.z));
    fopAc_ac_c* ship = dComIfGp_getShipActor();
    if (mModeFlg & LinkEffectConstants::ModeFlg_CRAWL) {
        J3DModel* model = mpCLModel.get();
        PSMTXMultVec(model->getBaseTRMtx(), &l_attention_crawl_offset, &attentionPos);
        return;
    }
    if (dComIfGp_getPlayerStatus0() & LinkEffectConstants::daPyStts0_SUBJECT_e) {
        gabi::Local<cXyz> afStack_18;
        LinkEffectApi::ZXYrotS(mDoMtx_stack_c::get(), (s32)mBodyAngle.x, (s32)shape_angle.y, 0);
        PSMTXMultVec(mDoMtx_stack_c::get(), &l_attention_subject_offset, afStack_18);
        attentionPos.y = current.pos.y + 26.3f;
        PSVECAdd(&attentionPos, afStack_18, &attentionPos);
        return;
    }
    u32 modeFlags = mModeFlg;
    if (modeFlags & LinkEffectConstants::ModeFlg_CROUCH) {
        u16 a = (u16)shape_angle.y;
        f32 attentionX = attentionPos.x;
        f32 s = cM_ssin(a);
        f32 attentionZ = attentionPos.z;
        attentionPos.y = current.pos.y + 46.25f;
        attentionPos.x = gabi::fmadds(20.0f, s, attentionX);
        attentionPos.z = gabi::fmadds(20.0f, cM_scos(a), attentionZ);
        return;
    }
    if (modeFlags & LinkEffectConstants::ModeFlg_ROPE) {
        attentionPos.copy(current.pos);
        return;
    }
    if (modeFlags & LinkEffectConstants::ModeFlg_HANG) {
        attentionPos.y = mCyl.mCyl.mCenter.y + 92.5f;
        return;
    }
    if (modeFlags & LinkEffectConstants::ModeFlg_SWIM) {
        attentionPos.y = current.pos.y + 20.05f;
        return;
    }
    J3DModel* model = mpCLModel.get(); /* getBaseTRMtx() */
    if ((modeFlags & LinkEffectConstants::ModeFlg_WHIDE) && (mNoResetFlg0 & daPyFlg0_UNK10000)) {
        attentionPos.y = LinkEffect_baseMatrixY(model) + 62.5f; /* HD: (92.5f + y) - 30.0f folded */
        return;
    }
    if ((modeFlags & LinkEffectConstants::ModeFlg_IN_SHIP) && ship != 0 && !(gabi::at<LinkEffectShip>(gabi::ea(ship))->mStateFlag & 1)) {
        f32 s = cM_ssin((u16)ship->shape_angle.y);
        attentionPos.x = gabi::fmadds(-35.0f, s, ship->current.pos.x); /* l_ship_offset.z folded */
        if (mNoResetFlg0 & daPyFlg0_UNK80) {
            attentionPos.y = (mWaterY + 15.0f) + 92.5f;
        } else {
            J3DModel* m2 = mpCLModel.get();
            attentionPos.y = LinkEffect_baseMatrixY(m2) + 92.5f;
        }
        f32 c = cM_scos((u16)ship->shape_angle.y);
        attentionPos.z = gabi::fmadds(-35.0f, c, ship->current.pos.z);
        return;
    }
    J3DModel* m3 = mpCLModel.get();
    attentionPos.y = LinkEffect_baseMatrixY(m3) + 92.5f;
}

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
/* 024038A0 */
void daPy_lk_c::setBootsModel(J3DModel** pBootModels) {
    gptr<J3DModel>* modelSlots = reinterpret_cast<gptr<J3DModel>*>(pBootModels);
    s32 bootsJointIdx = 1;
    if (mNoResetFlg0 & LinkItemConstants::daPyFlg0_EQUIP_HEAVY_BOOTS) {
        lk_copyJntMtx(mpCLModel, 38, modelSlots[0].get(), 1);
        lk_copyJntMtx(mpCLModel, 33, modelSlots[1].get(), 1);
        bootsJointIdx = 2;
    }
    J3DModel* rightBootModel = modelSlots[0].get();
    lk_copyJntMtx(mpCLModel, 40, rightBootModel, bootsJointIdx);
    lk_copyJntMtx(mpCLModel, 39, rightBootModel, bootsJointIdx + 1);
    J3DModel* leftBootModel = modelSlots[1].get();
    lk_copyJntMtx(mpCLModel, 35, leftBootModel, bootsJointIdx);
    lk_copyJntMtx(mpCLModel, 34, leftBootModel, bootsJointIdx + 1);
}

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
    LinkPoseOldFrame* fdata = LinkPose(this).m_old_fdata;
    if (fdata->mOldFrameFlg.get() == 0 /* m_old_fdata->getOldFrameFlg() */) {
        return FALSE;
    }
    LinkPoseTransformInfo* trans_info = fdata->mOldFrameTransInfo; /* getOldFrameTransInfo(0) */
    Quaternion* quaternion = fdata->mOldFrameQuaternion;           /* getOldFrameQuaternion(0) */
    LinkPoseApi::mDoMtx_ZrotS(mDoMtx_stack_c::get(), (s32)LinkPose(this).mFootData[1].field_0x008.get());
    LinkPoseApi::PSMTXConcat(mpCLModel->getAnmMtx(LinkPoseConstants::CL_JNT_LLEGA_JNT_e),
                             mDoMtx_stack_c::get(), mDoMtx_stack_c::get());
    mpCLModel->setAnmMtx(32, mDoMtx_stack_c::get());
    LinkPose_transQuatM(trans_info, quaternion, 33);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), LinkPose(this).mFootData[1].field_0x00A.get());
    mpCLModel->setAnmMtx(LinkPoseConstants::CL_JNT_LLEGB_JNT_e, mDoMtx_stack_c::get());
    LinkPose_transQuatM(trans_info, quaternion, 34);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), LinkPose(this).mFootData[1].field_0x002.get());
    mpCLModel->setAnmMtx(LinkPoseConstants::CL_JNT_LFOOT_JNT_e, mDoMtx_stack_c::get());
    LinkPose_transQuatM(trans_info, quaternion, 35);
    mpCLModel->setAnmMtx(LinkPoseConstants::CL_JNT_LTOE_JNT_e, mDoMtx_stack_c::get());

    fdata = LinkPose(this).m_old_fdata;
    trans_info = fdata->mOldFrameTransInfo;
    quaternion = fdata->mOldFrameQuaternion;
    LinkPoseApi::mDoMtx_ZrotS(mDoMtx_stack_c::get(), (s32)LinkPose(this).mFootData[0].field_0x008.get());
    LinkPoseApi::PSMTXConcat(mpCLModel->getAnmMtx(LinkPoseConstants::CL_JNT_RLEGA_JNT_e),
                             mDoMtx_stack_c::get(), mDoMtx_stack_c::get());
    mpCLModel->setAnmMtx(37, mDoMtx_stack_c::get());
    LinkPose_transQuatM(trans_info, quaternion, 38);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), LinkPose(this).mFootData[0].field_0x00A.get());
    mpCLModel->setAnmMtx(LinkPoseConstants::CL_JNT_RLEGB_JNT_e, mDoMtx_stack_c::get());
    LinkPose_transQuatM(trans_info, quaternion, 39);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), LinkPose(this).mFootData[0].field_0x002.get());
    mpCLModel->setAnmMtx(LinkPoseConstants::CL_JNT_RFOOT_JNT_e, mDoMtx_stack_c::get());
    LinkPose_transQuatM(trans_info, quaternion, 40);
    mpCLModel->setAnmMtx(LinkPoseConstants::CL_JNT_RTOE_JNT_e, mDoMtx_stack_c::get());
    return TRUE;
}

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
void daPy_lk_c::setFootEffectType(int effectID, cXyz* i_pos, int footIndex, int effectType) {
    /* function-local statics (HD: initialised on first use) */
    u32 run_splash_scale = gabi::ea(&l_run_splash_scale);
    lk_staticVec(gabi::ea(&l_run_splash_scale_initialized), run_splash_scale, 0.6f, 0.6f, 0.6f);
    u32 run_grass_scale = gabi::ea(&l_run_grass_scale);
    lk_staticVec(gabi::ea(&l_run_grass_scale_initialized), run_grass_scale, 0.65f, 0.65f, 0.65f);
    u32 heavy_emit_smoke_scale = gabi::ea(&l_heavy_emit_smoke_scale);
    if ((u32)l_heavy_emit_smoke_scale_initialized == 0) {
        l_heavy_emit_smoke_scale.z = 1.0f;
        l_heavy_emit_smoke_scale.x = 1.0f;
        l_heavy_emit_smoke_scale.y = 0.0f;
        l_heavy_emit_smoke_scale_initialized = 1;
    }
    u32 heavy_dyn_smoke_scale = gabi::ea(&l_heavy_dyn_smoke_scale);
    lk_staticVec(gabi::ea(&l_heavy_dyn_smoke_scale_initialized), heavy_dyn_smoke_scale, 0.25f, 0.25f, 0.25f);
    u32 heavy_pat_smoke_scale = gabi::ea(&l_heavy_pat_smoke_scale);
    lk_staticVec(gabi::ea(&l_heavy_pat_smoke_scale_initialized), heavy_pat_smoke_scale, 0.75f, 0.75f, 0.75f);
    u32 grass_scale = gabi::ea(&l_foot_grass_scale); /* static Vec {1.5f, 1.5f, 1.5f} */
    u32 smoke_scale = gabi::ea(&l_foot_smoke_scale); /* static Vec {1.25f, 1.25f, 1.25f} */

    JPABaseEmitter* emitter = NULL;
    LinkItemFootEffect& footEffect = LinkItem(this).mFootEffect[footIndex];
    u32 fe = gabi::ea(&footEffect);
    s16 angleX = 0;
    if ((mAcch.m_flags & 0x20 /* GROUND_HIT */) && !(mNoResetFlg0 & 0xA0000000)) {
        angleX = LinkItemApi::getGroundAngle(this, static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd), (s32)current.angle.y);
    }
    gmem_st32(gabi::ea(&footEffect.mPos.x), gmem_ld32(gabi::ea(&i_pos->x))); /* footEffect->setPos(i_pos) */
    gmem_st32(gabi::ea(&footEffect.mPos.y), gmem_ld32(gabi::ea(&i_pos->y)));
    footEffect.mAngle.y = current.angle.y; /* footEffect->setAngle(&angle) */
    gmem_st32(gabi::ea(&footEffect.mPos.z), gmem_ld32(gabi::ea(&i_pos->z)));
    footEffect.mAngle.z = 0;
    footEffect.mAngle.x = angleX;
    if (effectID == 0x23 /* dPa_name::ID_AK_JN_ELEMENTSHIBUKI00 */) {
        if (mNoResetFlg0 & LinkItemConstants::daPyFlg0_UNK80) {
            f32 y = i_pos->y;
            f32 w = mWaterY;
            if (w > y + 10.0f) {
                footEffect.mPos.y = w - 10.0f;
            }
        }
    } else if (mCurProc == LinkItemConstants::daPyProc_CUT_ROLL_e &&
               effectID == LinkItemConstants::ID_AK_JT_ELEMENTSMOKE00) {
        effectID = -2;
    } else if (effectType == 5) {
        effectID = 0x2027; /* ID_AK_JT_ELEMENTSMOKE01 */
    }
    s32 oldEffectID = (s32)footEffect.mId;
    if (effectID == oldEffectID) {
        return;
    }
    if (oldEffectID != -1) {
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(fe) + 0x44), fe); /* getSmokeCallBack()->remove() */
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(fe + 0x20) + 0x44),
                       fe + 0x20); /* getOtherCallBack()->remove() */
    }
    cXyz* pos = &footEffect.mPos;
    csXyz* angle = &footEffect.mAngle;
    if (effectID == -2) {
        s8 room = current.roomNo;
        emitter = dPa_control_set(dComIfGp_getParticle(), 1, 0x2022, pos, angle, gabi::at<cXyz>(smoke_scale),
                                  0xA0, gabi::at<dPa_levelEcallBack>(fe), room, nullptr, nullptr, nullptr);
        if (emitter != NULL) {
            footEffect.mId = effectID;
        }
    } else if (effectID == 0x2022 || effectID == 0x2027) {
        s8 room = current.roomNo; /* dComIfGp_particle_setToonP1 */
        emitter =
            dPa_control_set(dComIfGp_getParticle(), 3, (u16)effectID, pos, angle, gabi::at<cXyz>(smoke_scale),
                            0xA0, gabi::at<dPa_levelEcallBack>(fe), room, nullptr, nullptr, nullptr);
        if (emitter != NULL) {
            footEffect.mId = effectID;
        }
    } else if (effectID == LinkItemConstants::ID_AK_JN_ELEMENTKUSA00) {
        gabi::Local<GXColor> color; /* tevStr.mColorC0 */
        color->g = (u8)(s16)tevStr.mColorC0.g;
        color->b = (u8)(s16)tevStr.mColorC0.b;
        color->r = (u8)(s16)tevStr.mColorC0.r;
        color->a = (u8)(s16)tevStr.mColorC0.a;
        emitter =
            dPa_control_set(dComIfGp_getParticle(), 1, 0x24, pos, angle, gabi::at<cXyz>(grass_scale), 0xFF,
                            gabi::at<dPa_levelEcallBack>(fe + 0x20), -1, color, &tevStr.mColorK0, nullptr);
        if (emitter == NULL) {
            return;
        }
        /* HD: the emitter's material parameter block, colour from the particle manager (+0x68) */
        gabi::Local<lk_emitterParam_l> prm;
        prm->m08[1] = 1.0f;
        prm->mName = gabi::guest_str("");
        prm->m08[2] = 1.0f;
        prm->__vtbl = LK_SAFESTRING_VTBL;
        prm->m08[3] = 1.0f;
        prm->m08[0] = 1.0f;
        prm->m18[3] = 0;
        prm->m18[0] = 1;
        prm->m18[2] = 0;
        prm->m18[1] = 0;
        u32 pa = gabi::ea(dComIfGp_getParticle());
        for (u32 i = 0; i < 4; i++) {
            gmem_st32(gabi::ea(&prm->m08[i]),
                      gmem_ld32(gabi::ea(&gabi::at<LinkItemParticleView>(pa)->mMaterialColor[i])));
        }
        LinkItemApi::setEmitterMaterial(emitter, prm.get());
        footEffect.mId = effectID;
    } else {
        u32 cam = gabi::ea(dComIfGp_getCamera(0));
        u32 mode = (u32)gabi::at<LinkItemCameraView>(cam)->mCurMode;
        if (mode == 4) {
            return;
        }
        if (mode >= 0xA && (mode <= 0xB || mode == 0xE)) {
            return;
        }
        gabi::Local<GXColor> amb;
        gabi::Local<GXColor> dif;
        LinkItemApi::getSeaColor(amb.get(), dif.get());
        emitter = dPa_control_set(dComIfGp_getParticle(), 1, (u16)effectID, pos, angle, nullptr, 0xFF,
                                  gabi::at<dPa_levelEcallBack>(fe + 0x20), -1, amb, nullptr, nullptr);
        if (emitter != NULL) {
            footEffect.mId = effectID;
        }
    }
    if (effectID == -2) {
        effectID = 0x2022;
    }
    if (emitter == NULL) {
        return;
    }
    u32 e = gabi::ea(emitter);
    if (effectType == 5) {
        if (effectID == 0x2027) {
            LinkEmitter(e).mRate = 10.0f;                  /* setRate */
            LinkEmitter(e).mLifeTime = 0x28;               /* setLifeTime(40) */
            lk_storeVec(e + 0x08, heavy_emit_smoke_scale); /* setEmitterScale */
            lk_storeVec(e + 0x220, heavy_dyn_smoke_scale); /* setGlobalDynamicsScale */
            lk_storeVec(e + 0x238, heavy_pat_smoke_scale); /* setGlobalParticleScale */
        } else {
            LinkEmitter(e).mSpread = 1.0f; /* setSpread */
            LinkEmitter(e).mRate = 15.0f;
            if (effectID == 0x24) {
                LinkEmitter(e).mDirectionalSpeed = 12.0f; /* setDirectionalSpeed */
            }
        }
    } else if (effectType == 1) {
        LinkEmitter(e).mSpread = 1.0f;
        LinkEmitter(e).mRate = 16.0f;
    } else if (effectType == 3) {
        LinkEmitter(e).mRate = 8.0f;
        LinkEmitter(e).mSpread = 0.3f;
    } else if (effectType == 2) {
        if (effectID == 0x24) {
            lk_emitterSetGlobalScale(e, run_grass_scale);
            LinkEmitter(e).mLifeTime = 0xF;
            LinkEmitter(e).mSpread = 1.0f;
            LinkEmitter(e).mRate = 10.0f;
        } else {
            LinkEmitter(e).mSpread = 1.0f;
            LinkEmitter(e).mRate = 18.0f;
            lk_emitterSetGlobalScale(e, run_splash_scale);
        }
    } else if (effectType == 4 && effectID == 0x23) {
        LinkEmitter(e).mRate = 3.0f;
        LinkEmitter(e).mSpread = 0.2f;
    } else {
        LinkEmitter(e).mRate = 3.0f;
        LinkEmitter(e).mSpread = 0.05f;
    }
}

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
    u32 b = gabi::ea(this);
    if (!(mAcch.m_flags & 0x20) /* !mAcch.ChkGroundHit() */ ||
        (mNoResetFlg0 & LinkItemConstants::daPyFlg0_UNK80000000)) {
        LinkItemApi::resetFootEffect(this);
        LinkItemApi::endFollow(&LinkItem(this).m3410); /* m3410.end() */
        return;
    }
    gabi::Local<cXyz> pos;
    if (resetFlg0() & 0x400 /* getRightFootOnGround() */) {
        lk_jntPos(mpCLModel, LinkItemConstants::CL_JNT_RFOOT_JNT_e, pos);
        setFootMark(pos);
    }
    if (resetFlg0() & 0x800 /* getLeftFootOnGround() */) {
        lk_jntPos(mpCLModel, LinkItemConstants::CL_JNT_LFOOT_JNT_e, pos);
        setFootMark(pos);
    }
    if ((resetFlg0() & LinkItemConstants::daPyRFlg0_UNK10) &&
        mCurrAttributeCode == LinkItemConstants::dBgS_Attr_WATER_e && mWaterY > current.pos.y + 25.0f) {
        cXyz& splashPosition = LinkItem(this).m3410.mPos;
        u32 p = gabi::ea(&splashPosition);
        u16 a = (u16)current.angle.y;
        f32 s = cM_ssin(a);
        splashPosition.y = mWaterY;
        splashPosition.x = gabi::fmadds(45.0f, s, current.pos.x);
        LinkItem(this).m3410.mAngle.y =
            (s16)(a + 0x8000); /* m3410.setAngle(0, current.angle.y + 0x8000, 0) */
        LinkItem(this).m3410.mAngle.z = 0;
        LinkItem(this).m3410.mAngle.x = 0;
        splashPosition.z = gabi::fmadds(45.0f, cM_scos(a), current.pos.z);
        bool noSplash = lk_checkNoSplashCamera();
        bool updateSeaColor = LinkItem(this).m3410.mpEmitter != nullptr;
        if (!updateSeaColor) {
            if (!noSplash) {
                gabi::Local<GXColor> amb;
                gabi::Local<GXColor> dif;
                LinkItemApi::getSeaColor(amb.get(), dif.get());
                JPABaseEmitter* emitter =
                    dPa_control_set(dComIfGp_getParticle(), 1, LinkItemConstants::ID_AK_JN_ELEMENTSHIBUKI00,
                                    gabi::at<cXyz>(p), nullptr, nullptr, 0xFF,
                                    gabi::at<dPa_levelEcallBack>(b + 0x6824), -1, amb, nullptr, nullptr);
                if (emitter != NULL) {
                    u32 e = gabi::ea(emitter);
                    LinkEmitter(e).mDirectionalSpeed = 11.0f; /* setDirectionalSpeed */
                    LinkEmitter(e).mSpread = 0.6f;            /* setSpread */
                    LinkEmitter(e).mRate = 4.0f;              /* setRate */
                    LinkEmitter(e).mLifeTime = 15;            /* setLifeTime */
                }
                updateSeaColor = LinkItem(this).m3410.mpEmitter != nullptr;
            }
        }
        if (updateSeaColor) {
            /* HD: the splash follows the sea colour */
            gabi::Local<GXColor> amb2;
            gabi::Local<GXColor> dif2;
            LinkItemApi::getSeaColor(amb2.get(), dif2.get());
            u32 e = (u32)LinkItem(this).m3410.mpEmitter.v;
            LinkEmitter(e).mGlobalColor.r = amb2->r;
            LinkEmitter(e).mGlobalColor.b = amb2->b;
            LinkEmitter(e).mGlobalColor.g = amb2->g;
        }
    } else {
        LinkItemApi::endFollow(&LinkItem(this).m3410); /* m3410.end() */
    }
    {
        /* strcmp(dComIfGp_getStartStageName(), "Adanmae") == 0 */
        u32 sp = dComIfGp_ea() + 0x5134;
        u32 lp = gabi::guest_str("Adanmae");
        u8 c0, c1;
        do {
            c0 = gabi::load<u8>(sp++);
            c1 = gabi::load<u8>(lp++);
        } while (c0 == c1 && c0 != 0);
        if (c0 == c1) {
            u32 rf = resetFlg0();
            if ((rf & 0x10) && (rf & 0xC00) /* getFootOnGround() */ && !(mWaterY > current.pos.y)) {
                /* dComIfGp_particle_setP1(dPa_name::ID_AK_SN_VOLCANICASHESRUN00, &pos, &current.angle) */
                dPa_control_set(dComIfGp_getParticle(), 1, 0x8237, pos, &current.angle, nullptr, 0xFF,
                                nullptr, -1, nullptr, nullptr, nullptr);
            }
        }
    }
    s32 attr = mCurrAttributeCode;
    s32 effectID = LinkItemApi::checkAttributeEffect(dComIfGp_getParticle(), attr);
    if (effectID == -1) {
        return;
    }
    s32 effectType;
    u32 rf = resetFlg0();
    if ((rf & 0x10) && (rf & 0xC00) &&
        (LinkPoseApi::checkHeavyStateOn(this) || effectID == 0x23 || effectID == 0x24)) {
        mFootEffectPosType = (resetFlg0() & 0x400) ? 2 : 1;
        effectType = 2;
        if (LinkPoseApi::checkHeavyStateOn(this)) {
            effectType = 5;
            gabi::Local<cXyz> v;
            v->x = 0.0f;
            v->y = 1.0f;
            v->z = 0.0f;
            dComIfGp_getVibration_StartShock(2, -0x31, v);
        }
    } else if (mCurProc == LinkItemConstants::daPyProc_JUMP_CUT_LAND_e) {
        effectType = 3;
    } else if (mCurProc == LinkItemConstants::daPyProc_FRONT_ROLL_e) {
        effectType = 4;
    } else {
        effectType = 0;
    }
    u8 posType = mFootEffectPosType;
    if (posType == 0) {
        return;
    }
    if (mCurProc == LinkItemConstants::daPyProc_DEMO_LAST_COMBO_e) {
        f32 rnd = cM_rndFX(17.5f);
        gabi::Local<cXyz> sp44;
        sp44->x = m370C.x;
        sp44->y = m370C.y;
        sp44->z = m370C.z;
        lk_jntPos(mpCLModel, LinkItemConstants::CL_JNT_CENTER_e, &m370C);
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
        pos->z =
            gabi::fnmsubs(rnd, cM_scos((u16)a), m370C.z); /* (the GameCube source uses the cosine here too) */
        setFootEffectType(effectID, pos, 0, effectType);
    } else if (posType == 4) {
        f32 rnd = cM_rndFX(17.5f);
        u16 a = (u16)current.angle.y;
        f32 x = gabi::fmadds(rnd, cM_scos(a), current.pos.x);
        pos->x = x;
        pos->y = current.pos.y;
        pos->z = gabi::fnmsubs(rnd, cM_scos(a), current.pos.z);
        setFootEffectType(effectID, pos, 0, effectType);
    } else if (posType == 3) {
        if (mCurrAttributeCode == LinkItemConstants::dBgS_Attr_GRASS_e ||
            mCurrAttributeCode == LinkItemConstants::dBgS_Attr_WATER_e) {
            lk_jntPos(mpCLModel, LinkItemConstants::CL_JNT_RFOOT_JNT_e, pos);
            setFootEffectType(effectID, pos, 0, effectType);
            lk_jntPos(mpCLModel, LinkItemConstants::CL_JNT_LFOOT_JNT_e, pos);
            setFootEffectType(effectID, pos, 1, effectType);
        } else {
            setFootEffectType(effectID, &current.pos, 0, effectType);
        }
    } else if (posType == 1) {
        lk_jntPos(mpCLModel, LinkItemConstants::CL_JNT_LFOOT_JNT_e, pos);
        setFootEffectType(effectID, pos, 0, effectType);
    } else if (posType == 2) {
        lk_jntPos(mpCLModel, LinkItemConstants::CL_JNT_RFOOT_JNT_e, pos);
        setFootEffectType(effectID, pos, 0, effectType);
    } else if (posType == 5) {
        setFootEffectType(effectID, &current.pos, 0, 1);
    } else if (posType == 6) {
        Mtx34* mtx = lk_getAnmMtx(mpCLModel, LinkItemConstants::CL_JNT_LINK_ROOT_e);
        f32 x = mtx->m[0][3];
        f32 y = mtx->m[1][3];
        f32 z = mtx->m[2][3];
        pos->x = x;
        pos->y = y;
        mGndChkPos.x = x; /* mGndChk.SetPos(&pos) */
        pos->z = z;
        mGndChkPos.y = y;
        mGndChkPos.z = z;
        f64 groundY = LinkItemApi::groundCross(dComIfG_Bgsp(), mGndChk);
        if (groundY != -1000000000.0) {
            pos->y = (f32)groundY;
        }
        setFootEffectType(effectID, pos, 0, 1);
    }
}

/* 0240A41C */
void daPy_lk_c::setGrabItemPos() {
    fopAc_ac_c* grab_actor = mActorKeepGrab.mActor;
    if (grab_actor == NULL) {
        return;
    }
    if (mCurProc == LinkEffectConstants::daPyProc_GRAB_READY_e) {
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_stack_c::YrotM((s16)(shape_angle.y - mProcVar2));
        PSMTXMultVec(mDoMtx_stack_c::get(), &m370C, &grab_actor->current.pos);
        return;
    }
    s8 roomNo = current.roomNo;
    grab_actor->tevStr.mRoomNo = roomNo;
    u8 envr = tevStr.mEnvrIdxOverride;
    grab_actor->current.roomNo = roomNo;
    grab_actor->tevStr.mEnvrIdxOverride = envr;
    if (!(mNoResetFlg0 & LinkEffectConstants::daPyFlg0_UNK400) && checkGrabBarrelSearch(0) && !(mItemButton & 1) /* !doButton() */) {
        mNoResetFlg0 = mNoResetFlg0 | LinkEffectConstants::daPyFlg0_UNK400;
        for (int i = 0; i < 3; i++) {
            LinkEffectApi::setWallRadius(&mAcchCir[i], 50.0f);
        }
    }
    gabi::Local<cXyz> cStack_58;
    if (mCurProc == LinkEffectConstants::daPyProc_GRAB_UP_e) {
        cLib_chaseAngleS(&grab_actor->shape_angle.y, shape_angle.y, 0x1000);
    } else {
        if (mCurProc != LinkEffectConstants::daPyProc_DEMO_STAND_ITEM_PUT_e) {
            grab_actor->shape_angle.y = shape_angle.y;
        }
        grab_actor->shape_angle.z = m351C;
        if (mNoResetFlg0 & LinkEffectConstants::daPyFlg0_UNK400) {
            f32 grabOffset;
            if (mCurProc == LinkEffectConstants::daPyProc_GRAB_PUT_e) {
                grabOffset = 0.0f;
            } else if (mCurProc == LinkEffectConstants::daPyProc_GRAB_WAIT_e || mCurProc == LinkEffectConstants::daPyProc_WAIT_TURN_e) {
                grabOffset = -115.0f;
            } else {
                grabOffset = -85.0f;
            }
            field_0x2b0 = field_0x2b0 + m35D8;
            LinkPoseApi::addCalc(&field_0x2b0, grabOffset, 0.5f, 20.0f, 1.0f);
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
                s16 a = LinkItemApi::rad2s(rad_angle);
                target_slant_angle = (s16)gabi::ftoi(2048.0f * cM_ssin((u16)a));
            } else {
                target_slant_angle = 0;
            }
            gabi::Local<be<s16>> curr_slant_angle; /* barrel->get_slant_angle() */
            *curr_slant_angle = gabi::at<LinkEffectBarrel>(gabi::ea(grab_actor))->mSlantAngle;
            LinkEffectApi::addCalcAngleS(curr_slant_angle.get(), (s32)target_slant_angle, 5, 0x200, 0x80);
            gabi::at<LinkEffectBarrel>(gabi::ea(grab_actor))->mSlantAngle = *curr_slant_angle;
        }
    }
    if (mCurProc == LinkEffectConstants::daPyProc_GRAB_UP_e) {
        cStack_58->copy(grab_actor->current.pos);
    }
    gabi::Local<cXyz> sum;
    cXyz_pl(&mLeftHandPos, sum, &mRightHandPos);
    gabi::Local<cXyz> half;
    cXyz_ml(sum, half, 0.5f);
    grab_actor->current.pos.x = half->x;
    f32 hy = half->y;
    grab_actor->current.pos.y = hy;
    grab_actor->current.pos.z = half->z;
    grab_actor->current.pos.y = hy + field_0x2b0;
    if (LinkEffect(this).mUnderAnimationIndex == LinkEffectConstants::dRes_INDEX_LKANM_BCK_GRABUP_e /* m_anm_heap_under[0].mIdx == dRes_INDEX_LKANM_BCK_GRABUP_e */) {
        s16 st = mFrameCtrlUnder[0].getStart();
        s16 en = mFrameCtrlUnder[0].getEnd();
        f32 grabOffset = 1.0f - ((mFrameCtrlUnder[0].getFrame() - (f32)st) / (f32)(s32)(en - st));
        f32 horizontalOffset = m35C8 * grabOffset;
        grab_actor->current.pos.x = gabi::fmadds(horizontalOffset, cM_ssin((u16)shape_angle.y), grab_actor->current.pos.x);
        grab_actor->current.pos.z = gabi::fmadds(horizontalOffset, cM_scos((u16)shape_angle.y), grab_actor->current.pos.z);
        if (mCurProc == LinkEffectConstants::daPyProc_GRAB_PUT_e) {
            grab_actor->current.pos.y = gabi::fmadds(-18.07f, grabOffset, grab_actor->current.pos.y);
        } else {
            grab_actor->current.pos.y = gabi::fmadds(-16.41f, grabOffset, grab_actor->current.pos.y);
        }
        if (mCurProc == LinkEffectConstants::daPyProc_GRAB_PUT_e) {
            f32 x = grab_actor->current.pos.x;
            f32 y = current.pos.y + 125.0f;
            f32 z = grab_actor->current.pos.z;
            mGndChkPos.x = x; /* mGndChk.SetPos(&local_64) */
            mGndChkPos.y = y;
            mGndChkPos.z = z;
            // HD: GroundCross remains double until the comparison.
            f64 groundY = LinkItemApi::groundCross(dComIfG_Bgsp(), mGndChk);
            if (groundY > (f64)(f32)grab_actor->current.pos.y) {
                grab_actor->speedF = 0.0f;
                grab_actor->current.pos.y = (f32)groundY;
                freeGrabItem();
                return;
            }
        }
        if (fpcM_GetName(grab_actor) == LinkEffectConstants::fpcNm_MT_e) {
            grab_actor->current.pos.y = gabi::fmadds(40.0f /* magtail_offset */, 1.0f - grabOffset, grab_actor->current.pos.y);
        }
    } else if (fpcM_GetName(grab_actor) == LinkEffectConstants::fpcNm_MT_e) {
        f32 horizontalOffset = -40.0f * cM_ssin((u16)m351C);
        grab_actor->current.pos.x = gabi::fmadds(horizontalOffset, cM_scos((u16)shape_angle.y), grab_actor->current.pos.x);
        grab_actor->current.pos.y = gabi::fmadds(40.0f, cM_scos((u16)m351C), grab_actor->current.pos.y);
        grab_actor->current.pos.z = gabi::fnmsubs(horizontalOffset, cM_ssin((u16)shape_angle.y), grab_actor->current.pos.z);
    }
    if (mCurProc == LinkEffectConstants::daPyProc_GRAB_UP_e && grab_actor->current.pos.y < cStack_58->y) {
        grab_actor->current.pos.y = cStack_58->y;
        grab_actor->current.pos.x = cStack_58->x;
        grab_actor->current.pos.z = cStack_58->z;
    }
    if (mNoResetFlg0 & LinkEffectConstants::daPyFlg0_UNK400) {
        m35CC = 0.0f;
        grab_actor->current.pos.y = grab_actor->current.pos.y + 0.0f;
    } else if ((mModeFlg & LinkEffectConstants::ModeFlg_MIDAIR) && speed.y < 0.0f) {
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

// HD: update the renderer's interpolation value before notifying its animation channel.
static inline void lk_setAnimationFrame(gptr<LinkItemAnimationValue>* slot, f32 frame) {
    LinkItemAnimationValue* value = *slot;
    f32 result = gabi::call_ptr<f32>(value->mpInterpolate, (u32)value->mpContext, frame,
                                   (f32)value->mStart, (f32)value->mEnd);
    value->mFrame = result;
    LinkItemApi::updateAnimationValue(slot);
}
static inline void lk_hdAnmFrame(u32 self, u32 field, f32 frame) {
    lk_setAnimationFrame(gabi::at<gptr<LinkItemAnimationValue>>(self + field), frame);
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
    // HD: preserve the two GHS guards and their distinct initialization store order.
    if (l_sword_d_scale_initialized == 0) {
        l_sword_d_scale.x = 1.4f;
        l_sword_d_scale.z = 1.4f;
        l_sword_d_scale_initialized = 1;
        l_sword_d_scale.y = 1.4f;
    }
    if (l_sword_p_scale_initialized == 0) {
        l_sword_p_scale.y = 1.4f;
        l_sword_p_scale.z = 1.4f;
        l_sword_p_scale.x = 1.6f;
        l_sword_p_scale_initialized = 1;
    }
    if (mEquipItem != LinkEffectConstants::daPyItem_SWORD_e || LinkItem(this).mpEquipItemBrk.mpAnm.v == 0 /* mpEquipItemBrk */ ||
        LinkItem(this).mpSwordBtk.mpAnm == 0 /* mpSwordBtk */ || LinkItem(this).mpSwordModel1.v == 0 /* mpSwordModel1 */ ||
        LinkItem(this).mpEquipItemModel.v == 0 /* mpEquipItemModel */) {
        return;
    }
    // HD: BRK frame also passes through the renderer interpolation callback.
    f32 v;
    if (checkChanceMode()) {
        v = 1.0f;
    } else if (noResetFlg1() & daPyFlg1_SOUP_POWER_UP) {
        v = 0.0f;
    } else {
        v = 2.0f;
    }
    LinkItem(this).mpEquipItemBrk.mFrameCtrl.mFrame = v;
    LinkEffect_resourceFrame(LinkItem(this).mpEquipItemBrk.mpAnm.v) = v;
    lk_setAnimationFrame(&LinkItem(this).mpEquipItemBrk.mpFrameValue, v);
    {
        // HD: twelve FPR loads quiet signaling NaNs before the first store.
        u32 modelAddress = LinkItem(this).mpEquipItemModel.v;
        u32 dst = LinkItem(this).mpSwordModel1.v + offsetof(J3DModel, mBaseTransformMtx);
        u32 src = modelAddress ? modelAddress + offsetof(J3DModel, mBaseTransformMtx) : 0;
        u32 matrixWords[12];
        for (u32 i = 0; i < 12; i++) {
            matrixWords[i] = lk_fbits(gabi::load<f32>(src + i * 4));
        }
        for (u32 i = 0; i < 12; i++) {
            gmem_stf32(dst + i * 4, matrixWords[i]);
        }
    }
    if (!checkDemoSwordNoDraw(0) &&
        (checkChanceMode() || (noResetFlg1() & daPyFlg1_SOUP_POWER_UP) || lk_selectSword() == LinkEffectConstants::dItemNo_MASTER_SWORD_3_e)) {
        // HD: BTK playback updates the resource and renderer callback frame.
        LinkItemApi::playAnimation(&LinkItem(this).mpSwordBtk);
        f32 fr = LinkItem(this).mpSwordBtk.mFrameCtrl.mFrame;
        LinkEffect_resourceFrame(LinkItem(this).mpSwordBtk.mpAnm) = fr;
        lk_setAnimationFrame(&LinkItem(this).mpSwordBtk.mpFrameValue, fr);
        JPABaseEmitter* e;
        if (LinkEffect(this).m3454.mpEmitter.v == 0 /* m3454.getEmitter() */ && (checkChanceMode() || (noResetFlg1() & daPyFlg1_SOUP_POWER_UP))) {
            Mtx34* mtx = lk_getAnmMtx(gabi::at<J3DModel>(LinkItem(this).mpSwordModel1.v), LinkEffectConstants::YMSLI00_JNT_SWMSA_JNT_e);
            e = LinkEffectApi::makeMtxEmitter(&LinkEffect(this).m3454, LinkEffectConstants::ID_AK_JN_LIGHTSAVER00,
                                            mtx, &current.pos, 0);
            if (e != NULL && lk_checkMasterSwordEquip()) {
                LinkEffect_storeScale(&LinkEffectParticle(e).mGlobalDynamicsScale, &l_sword_d_scale); /* setGlobalDynamicsScale */
                LinkEffect_storeScale(&LinkEffectParticle(e).mGlobalParticleScale, &l_sword_p_scale); /* setGlobalParticleScale */
            }
        } else if (!checkChanceMode() && !(noResetFlg1() & daPyFlg1_SOUP_POWER_UP) && lk_selectSword() == LinkEffectConstants::dItemNo_MASTER_SWORD_3_e) {
            LinkItemApi::endMtxFollow(&LinkEffect(this).m3454); /* m3454.end() */
        }
        BOOL chance = checkChanceMode();
        u32 em = LinkEffect(this).m3454.mpEmitter.v;
        GXColor* prm0;
        GXColor* env0;
        if (chance) {
            prm0 = &l_sword_g_prm0;
            env0 = &l_sword_g_env0;
        } else {
            prm0 = &l_sword_y_prm0;
            env0 = &l_sword_y_env0;
        }
        if (em != 0) {
            u8 g = prm0->g;
            u8 r = prm0->r;
            u8 bl = prm0->b;
            LinkEffectParticle(gabi::at<JPABaseEmitter>(em)).mGlobalPrmColor.r = r; /* setGlobalPrmColor */
            LinkEffectParticle(gabi::at<JPABaseEmitter>(em)).mGlobalPrmColor.g = g;
            LinkEffectParticle(gabi::at<JPABaseEmitter>(em)).mGlobalPrmColor.b = bl;
            u8 r2 = env0->r;
            u8 g2 = env0->g;
            u8 b2 = env0->b;
            LinkEffectParticle(gabi::at<JPABaseEmitter>(em)).mGlobalEnvColor.g = g2; /* setGlobalEnvColor */
            LinkEffectParticle(gabi::at<JPABaseEmitter>(em)).mGlobalEnvColor.r = r2;
            LinkEffectParticle(gabi::at<JPABaseEmitter>(em)).mGlobalEnvColor.b = b2;
        }
        bool parry = dComIfGp_getDoStatus() == LinkEffectConstants::dActStts_PARRY_e && m355C == 0;
        bool go = parry;
        if (!go) {
            u32 f1 = noResetFlg1();
            go = ((f1 & daPyFlg1_SOUP_POWER_UP) || lk_selectSword() == LinkEffectConstants::dItemNo_MASTER_SWORD_3_e) && !(f1 & daPyFlg1_UNK200000);
        }
        if (go) {
            GXColor* prm;
            GXColor* env;
            if (dComIfGp_getDoStatus() == LinkEffectConstants::dActStts_PARRY_e && m355C == 0) {
                prm = &l_sword_g_prm1;
                env = prm;
                gabi::Local<cXyz> v1;
                v1->x = 0.0f;
                v1->y = 1.0f;
                v1->z = 0.0f;
                dComIfGp_getVibration_StartShock(6, 1, v1);
            } else if (noResetFlg1() & daPyFlg1_SOUP_POWER_UP) {
                prm = &l_sword_y_prm1;
                env = &l_sword_y_env1;
            } else {
                prm = &l_sword_s_prm1;
                env = &l_sword_s_env1;
            }
            /* dComIfGp_particle_setP1(dPa_name::ID_AK_JN_LIGHTSAVER01, &current.pos, .., pbVar9, pbVar8) */
            e = dPa_control_set(dComIfGp_getParticle(), 1, LinkEffectConstants::ID_AK_JN_LIGHTSAVER01, &current.pos, nullptr, nullptr, 0xFF, nullptr, -1,
                                prm, env, nullptr);
            if (e != NULL) {
                Mtx34* mtx = lk_getAnmMtx(gabi::at<J3DModel>(LinkItem(this).mpEquipItemModel.v), LinkEffectConstants::SWMS_JNT_SWMSA_JNT_e);
                LinkEffectApi::setGlobalRTMatrix(mtx, &LinkEffectParticle(e).mGlobalRotation, &LinkEffectParticle(e).mGlobalTranslation);
                if (lk_checkMasterSwordEquip()) {
                    LinkEffect_storeScale(&LinkEffectParticle(e).mGlobalDynamicsScale, &l_sword_d_scale);
                    LinkEffect_storeScale(&LinkEffectParticle(e).mGlobalParticleScale, &l_sword_p_scale);
                }
                if (prm == env) {
                    // HD: the parry branch returns after its chance-dependent countdown.
                    m355C = 0x1E;
                    if (!checkChanceMode()) {
                        m355C = m355C - 1;
                    }
                    return;
                }
                setNoResetFlg1(noResetFlg1() | daPyFlg1_UNK200000);
            }
        }
    } else {
        LinkItemApi::endMtxFollow(&LinkEffect(this).m3454); /* m3454.end() */
        LinkItem(this).mpSwordBtk.mFrameCtrl.mFrame = 0.0f;                     /* mpSwordBtk->setFrame(0.0f) */
        LinkEffect_resourceFrame(LinkItem(this).mpSwordBtk.mpAnm) = 0.0f;
        lk_setAnimationFrame(&LinkItem(this).mpSwordBtk.mpFrameValue, 0.0f);
    }
    if (m355C > 0 && !checkChanceMode()) {
        m355C = m355C - 1;
    }
}

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

// HD: animation resources expose their frame limit through the renderer vtable.
static inline s32 LinkAura_getFrameMax(u32 animation) {
    LinkItemAnimationResource* resource = gabi::at<LinkItemAnimationResource>(animation);
    return gabi::call_ptr<s32>(gabi::at<be<u32>>(resource->__vtbl)[5], animation);
}

/* 0240B7F4 */
void daPy_lk_c::setAuraEffect() {
    LinkAuraLayout& effects = LinkAura(this);
    f32 y00BtkFrameMax = (f32)effects.mpYaura00Btk.mFrameCtrl.mEnd;
    u32 curYmgcs00Brk = gabi::ea(effects.mYmgcs00Brk.mpAnm.get());
    u32 curYaura00Brk = gabi::ea(effects.mYaura00rBrk.mpAnm.get());
    u32 flg1 = noResetFlg1();
    f32 yauraFrame = gabi::at<LinkItemAnimationResource>(curYaura00Brk)->mFrame;
    f32 ymgcsFrame = gabi::at<LinkItemAnimationResource>(curYmgcs00Brk)->mFrame;
    if ((flg1 & daPyFlg1_EQUIP_DRAGON_SHIELD) || mTinkleShieldTimer != 0) { /* checkNoDamageMode() */
        u32 pYaura00Brk;
        u32 pYmgcs00Brk;
        if (flg1 & daPyFlg1_EQUIP_DRAGON_SHIELD) { /* checkEquipDragonShield() */
            /* HD: no magic consumption here; the shield ends when the magic is empty */
            if (l_aura_save.mpSave->mMagic == 0 /* dComIfGs_getMagic() */) {
                setNoResetFlg1(flg1 & ~u32(daPyFlg1_EQUIP_DRAGON_SHIELD));
            }
            pYaura00Brk = gabi::ea(
                dComIfG_getObjectRes("Link", LinkAuraConstants::dRes_INDEX_LINK_BRK_YAURA00_R_e, LK_SAFESTRING_VTBL));
            pYmgcs00Brk = gabi::ea(
                dComIfG_getObjectRes("Link", LinkAuraConstants::dRes_INDEX_LINK_BRK_YMGCS00_MS_e, LK_SAFESTRING_VTBL));
        } else {
            if (((dComIfG_inf_c*)dComIfGp_get())->play.mEvtCtrl.mMode == 0 /* !dComIfGp_event_runCheck() */) {
                mTinkleShieldTimer = mTinkleShieldTimer - 1;
            }
            pYaura00Brk = gabi::ea(
                dComIfG_getObjectRes("Link", LinkAuraConstants::dRes_INDEX_LINK_BRK_YAURA00_G_e, LK_SAFESTRING_VTBL));
            pYmgcs00Brk = gabi::ea(
                dComIfG_getObjectRes("Link", LinkAuraConstants::dRes_INDEX_LINK_BRK_YMGCS00_TS_e, LK_SAFESTRING_VTBL));
        }
        if (pYaura00Brk != curYaura00Brk) {
            /* mYaura00rBrk.init(mMagicArmorAuraEntries[0].getModel()->getModelData(), ..) */
            LinkAuraApi::initBrk(&effects.mYaura00rBrk, effects.mMagicArmorAuraEntries[0].mpModel->mModelData,
                                 pYaura00Brk, 0, 0, 1.0f, 0, -1, 1, 0);
            curYaura00Brk = pYaura00Brk;
        }
        if (pYmgcs00Brk != curYmgcs00Brk) {
            LinkAuraApi::initBrk(&effects.mYmgcs00Brk, effects.mpYmgcs00Model->mModelData, pYmgcs00Brk, 0, 0, 1.0f, 0,
                                 -1, 1, 0);
            curYmgcs00Brk = pYmgcs00Brk;
        }
    }
    if (((noResetFlg1() & daPyFlg1_EQUIP_DRAGON_SHIELD) || mTinkleShieldTimer != 0) &&
        (((dComIfG_inf_c*)dComIfGp_get())->play.mEvtCtrl.mMode == 0 ||
         (eventInfo.mCommand == dEvtCmd_INDOOR_e && !(((dComIfG_inf_c*)dComIfGp_get())->play.mEvtCtrl.mEventFlag &
                                                      LinkAuraConstants::dEvtFlag_TALK_e) /* dEvtFlag_TALK_e */))) {
        if (yauraFrame < 0.01f) {
            for (int idx = 0; idx < 6; idx++) {
                effects.mMagicArmorAuraEntries[idx].mFrame =
                    cM_rndF(y00BtkFrameMax); /* mMagicArmorAuraEntries[idx].setFrame(..) */
            }
        }
        // HD: unordered values take the clamp path; preserve both virtual calls.
        yauraFrame = yauraFrame + 1.0f;
        if (!(yauraFrame < (f32)LinkAura_getFrameMax(curYaura00Brk))) {
            yauraFrame = (f32)LinkAura_getFrameMax(curYaura00Brk) - 0.001f;
        }
        ymgcsFrame = ymgcsFrame + 1.0f;
        if (!(ymgcsFrame < (f32)LinkAura_getFrameMax(curYmgcs00Brk))) {
            ymgcsFrame = (f32)LinkAura_getFrameMax(curYmgcs00Brk) - 0.001f;
        }
        seStartMapInfo(LinkAuraConstants::JA_SE_LK_MG_SHELD_USING);
    } else {
        yauraFrame = yauraFrame - 1.0f;
        yauraFrame = fsel_l(yauraFrame, yauraFrame, 0.0f);
        ymgcsFrame = ymgcsFrame - 1.0f;
        ymgcsFrame = fsel_l(ymgcsFrame, ymgcsFrame, 0.0f);
    }
    /* mYaura00rBrk.entryFrame(yauraFrame); mYmgcs00Brk.entryFrame(ymgcsFrame) (HD: frame callback objects) */
    effects.mYaura00rBrk.mFrameCtrl.mFrame = yauraFrame;
    effects.mYaura00rBrk.mpAnm->mFrame = yauraFrame;
    lk_setAnimationFrame(&effects.mYaura00rBrk.mpFrameValue, yauraFrame);
    effects.mYmgcs00Brk.mFrameCtrl.mFrame = ymgcsFrame;
    effects.mYmgcs00Brk.mpAnm->mFrame = ymgcsFrame;
    lk_setAnimationFrame(&effects.mYmgcs00Brk.mpFrameValue, ymgcsFrame);
    if (yauraFrame > 0.0f) {
        f32 var_f28;
        f32 var_f31;
        if (dComIfGp_checkPlayerStatus0_l(LinkPoseConstants::daPyStts0_SHIP_RIDE_e) &&
            gabi::ea(((dComIfG_inf_c*)dComIfGp_get())->play.mpPlayerPtr[2].get()) != 0) {
            var_f31 = 0.0f;
            u32 ship = gabi::ea(((dComIfG_inf_c*)dComIfGp_get())->play.mpPlayerPtr[2].get());
            var_f28 = std::fabs((f32)gabi::at<fopAc_ac_c>(ship)->speedF) / 15.0f + 1.0f;
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
        s16 a = LinkItemApi::rad2s((var_f31 * 0.5f) * 3.1415927f); /* cM_fcos(M_PI * (0.5f * var_f31)) */
        gabi::Local<cXyz> localScale;
        cXyz_ml(&scale, localScale, cM_scos((u16)a) * var_f28);
        f32 f25 = gabi::fnmsubs(var_f31, 0.3f, 1.0f);
        gabi::Local<cXyz> t0;
        gabi::Local<cXyz> t1;
        LinkAuraEntry* entry = effects.mMagicArmorAuraEntries;
        for (int idx = 0; idx < 6; idx++, entry++) {
            f32 var_f0 = entry->mFrame + 1.0f;
            if (!(var_f0 < y00BtkFrameMax)) {
                var_f0 -= y00BtkFrameMax;
            }
            entry->mFrame = var_f0;
            u16 jnt = aura_model_joint[idx];
            J3DMtxBuffer_hd* blk = mpCLModel->mpMtxBuffer;
            u32 mtx = (u32)blk->mpAnmMtx + jnt * 0x30;
            u32 model = gabi::ea(entry->mpModel.get());
            blk->mFlags = blk->mFlags | LinkAuraConstants::J3DMtxBuffer_DIRTY;
            lk_setBaseTRMtx(model, mtx);
            u32 m2 = gabi::ea(entry->mpModel.get());
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
            f32 sz = gabi::at<cXyz>(sc)->z; /* setBaseScale */
            f32 sy = gabi::at<cXyz>(sc)->y;
            f32 sx = gabi::at<cXyz>(sc)->x;
            gabi::at<J3DModel>(m2)->mBaseScale.y = sy;
            gabi::at<J3DModel>(m2)->mBaseScale.z = sz;
            gabi::at<J3DModel>(m2)->mBaseScale.x = sx;
        }
    }
    if ((noResetFlg1() & daPyFlg1_EQUIP_DRAGON_SHIELD) || mTinkleShieldTimer != 0 ||
        effects.mYmgcs00Brk.mpAnm->mFrame > 0.0f) {
        u32 m = gabi::ea(mpCLModel.get());
        lk_setBaseTRMtx(
            gabi::ea(effects.mpYmgcs00Model.get()),
            gabi::ea(
                gabi::at<J3DModel>(m)->getBaseTRMtx())); /* mpYmgcs00Model->setBaseTRMtx(mpCLModel->getBaseTRMtx()) */
        J3DModel_calc(gabi::at<J3DModel>(gabi::ea(effects.mpYmgcs00Model.get())));
        // HD: advance the embedded animation, then update its renderer channel.
        LinkItemApi::playAnimation(&effects.mpYmgcs00Btk);
        f32 fr = effects.mpYmgcs00Btk.mFrameCtrl.mFrame;
        gabi::at<LinkItemAnimationResource>(effects.mpYmgcs00Btk.mpAnm)->mFrame = fr;
        lk_setAnimationFrame(&effects.mpYmgcs00Btk.mpFrameValue, fr);
    }
}

/* JPABaseEmitter::setGlobalScale (HD: also the particle scale; the static is read twice) */
static inline void LinkAura_setGlobalScale(LinkAuraEmitter* emitter, const cXyz* scale) {
    emitter->mGlobalScale.x = scale->x;
    emitter->mGlobalScale.y = scale->y;
    emitter->mGlobalScale.z = scale->z;
    emitter->mGlobalParticleScale.x = scale->x;
    emitter->mGlobalParticleScale.y = scale->y;
    emitter->mGlobalParticleScale.z = scale->z;
}
/* HD: the "Demo07" water ripple: in that demo, with a demo actor id and after frame 2600 */
static inline bool lk_checkDemo07Ripple(daPy_lk_c* p) {
    u8 id = p->demoActorID;
    if (id == 0 || id > 0x20) {
        return false;
    }
    LinkAuraDemoObject* obj = LinkAura_demoState().mDemoObj;
    if (obj == 0) {
        JUT_ASSERT_fail("d_demo.h", 0x23A, "m_object != (0)");
        obj = LinkAura_demoState().mDemoObj;
    }
    if (LinkAuraDemoApi::getActor(obj, (u32)id) == 0) {
        return false;
    }
    gabi::Local<lk_SafeString_l> b;
    b->__vtbl = LK_SAFESTRING_VTBL;
    b->mStr = gabi::ea(LinkAura_demoArchive().mName);
    gabi::Local<lk_SafeString_l> a;
    a->mStr = gabi::guest_str("Demo07");
    a->__vtbl = LK_SAFESTRING_VTBL;
    LinkAuraDemoApi::assureTermination(a.get());
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
    return LinkAura_demoState().mFrameNoMsg > 2600;
}

/* 0240AB88 */
void daPy_lk_c::setWaterRipple() {
    LinkAuraLayout& effects = LinkAura(this);
    // HD: GHS writes each guard between specific component stores.
    if (l_normal_ripple_initialized == 0) {
        l_normal_ripple_scale.x = 1.0f;
        l_normal_ripple_scale.z = 1.0f;
        l_normal_ripple_initialized = 1;
        l_normal_ripple_scale.y = 1.0f;
    }
    if (l_small_ripple_initialized == 0) {
        l_small_ripple_initialized = 1;
        l_small_ripple_scale.x = 0.4f;
        l_small_ripple_scale.z = 0.4f;
        l_small_ripple_scale.y = 0.4f;
    }

    BOOL var_r3 = mCurProc == LinkAuraConstants::daPyProc_DEMO_DEAD_e &&
                  dComIfGp_checkPlayerStatus0_l(LinkAuraConstants::daPyStts0_SWIM_e);
    bool ripple;
    bool forced = false;
    if (var_r3 && mProcVar3 != 0) {
        ripple = true;
    } else {
        ripple = false;
        if (mCurProc != LinkPoseConstants::daPyProc_VOMIT_WAIT_e && !var_r3 &&
            (!(mModeFlg & (LinkAuraConstants::ModeFlg_IN_SHIP | LinkAuraConstants::ModeFlg_HANG)) ||
             (mModeFlg & LinkPoseConstants::ModeFlg_SWIM))) {
            if ((mNoResetFlg0 & daPyFlg0_UNK80) && mWaterY > mCyl.mCyl.mCenter.y + 5.0f &&
                mWaterY < mCyl.mCyl.mCenter.y + mCyl.mCyl.mHeight) {
                ripple = true;
            } else if (mCurrAttributeCode == LinkItemConstants::dBgS_Attr_WATER_e && (mAcch.ChkGroundHit())) {
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
            pos = gabi::ea(&l_demo07_ripple_pos);
            if (l_demo07_ripple_initialized == 0) {
                l_demo07_ripple_initialized = 1;
                l_demo07_ripple_pos.z = -10430.0f;
                l_demo07_ripple_pos.x = -26680.0f;
                l_demo07_ripple_pos.y = -3555.0f;
            }
        }
        if (gabi::ea(effects.m3280.mpEmitter.get()) == 0) { /* m3280.getEmitter() */
            /* dComIfGp_particle_setShipTail(dPa_name::ID_AK_JN_HAMON00, &pos, NULL, NULL, 0xFF, &m3280) */
            dPa_control_set(dComIfGp_getParticle(), 5, LinkAuraConstants::ID_AK_JN_HAMON00, gabi::at<cXyz>(pos),
                            nullptr, nullptr, 0xFF, reinterpret_cast<dPa_levelEcallBack*>(&effects.m3280), -1, nullptr,
                            nullptr, nullptr);
        }
        f32 var_f1 = std::fabs((f32)speedF) * 0.1f;
        var_f1 = var_f1 * var_f1;
        if (var_f1 > 1.0f) {
            var_f1 = 1.0f;
        }
        effects.m3280.mRate = var_f1; /* m3280.setRate(var_f1) */
        // HD: advance the embedded animation, then update its renderer channel.
        LinkItemApi::playAnimation(&effects.mpSuimenMunyaBtk);
        f32 fr = effects.mpSuimenMunyaBtk.mFrameCtrl.mFrame;
        gabi::at<LinkItemAnimationResource>(effects.mpSuimenMunyaBtk.mpAnm)->mFrame = fr;
        lk_setAnimationFrame(&effects.mpSuimenMunyaBtk.mpFrameValue, fr);
        u32 e = gabi::ea(effects.m3280.mpEmitter.get());
        if (e != 0) {
            f32 sx = LinkAura_emitter(e)->mGlobalParticleScale.x; /* getGlobalParticleScale().x */
            u32 f80 = mNoResetFlg0 & daPyFlg0_UNK80;
            bool remove;
            if (sx < 0.8f && f80) {
                remove = true;
            } else if (!(sx > 0.8f)) {
                remove = false;
            } else {
                remove = f80 == 0;
            }
            if (remove) {
                LinkAuraApi::endRipple(&effects.m3280);
                e = gabi::ea(dPa_control_set(dComIfGp_getParticle(), 5, LinkAuraConstants::ID_AK_JN_HAMON00,
                                             gabi::at<cXyz>(pos), nullptr, nullptr, 0xFF,
                                             reinterpret_cast<dPa_levelEcallBack*>(&effects.m3280), -1, nullptr,
                                             nullptr, nullptr));
                if (e != 0) {
                    f80 = mNoResetFlg0 & daPyFlg0_UNK80;
                }
            }
            if (e != 0) {
                if (f80) {
                    LinkAura_setGlobalScale(LinkAura_emitter(e), &l_normal_ripple_scale);
                } else {
                    LinkAura_setGlobalScale(LinkAura_emitter(e), &l_small_ripple_scale);
                }
            }
        }
    } else if (gabi::ea(effects.m3280.mpEmitter.get()) != 0) {
        LinkAuraApi::endRipple(&effects.m3280); /* m3280.remove() */
    }

    if (mModeFlg & LinkPoseConstants::ModeFlg_SWIM) {
        u8 end0 = effects.mSwimTailEcallBack[0].mEnd;
        u8 end1 = effects.mSwimTailEcallBack[1].mEnd;
        if (!end0) {
            effects.mSwimTailEcallBack[0].mSpeedRate =
                std::fabs(mNormalSpeed / mMaxNormalSpeed); /* mSwimTailEcallBack[0].setSpeedRate */
        }
        if (!end1) {
            effects.mSwimTailEcallBack[1].mSpeedRate = std::fabs(mNormalSpeed / mMaxNormalSpeed);
        }
        gabi::Local<cXyz> local_1c;
        PSMTXMultVec(lk_getAnmMtx(mpCLModel, LinkItemConstants::CL_JNT_HEAD_JNT_e), &l_wave_offset /* wave_offset */,
                     local_1c);
        u16 a = (u16)shape_angle.y;
        f32 lz = local_1c->z;
        f32 lx = local_1c->x;
        f32 water = mWaterY;
        f32 flat = mAcch.m_wtr.m_height; /* mAcch.m_wtr.GetHeight() */
        f32 c5 = rippleScale5 * cM_scos(a);
        f32 s5 = rippleScale5 * cM_ssin(a);
        effects.mSwimTailEcallBack[0].mPos.y = water; /* mSwimTailEcallBack[0].setPos(local_20).y */
        effects.mSwimTailEcallBack[1].mPos.y = water;
        local_1c->y = water;
        f32 w2 = water + 2.0f;
        effects.mSwimTailEcallBack[1].mWaterFlatY = flat; /* [1].setWaterFlatY */
        effects.mSwimTailEcallBack[0].mWaterFlatY = flat; /* [0].setWaterFlatY */
        effects.mSwimTailEcallBack[0].mWaterY = w2;       /* [0].setWaterY(mWaterY + 2.0f) */
        effects.mSwimTailEcallBack[1].mWaterY = w2;
        effects.mSwimTailEcallBack[0].mPos.z = lz - s5;
        effects.mSwimTailEcallBack[0].mPos.x = lx + c5;
        effects.mSwimTailEcallBack[1].mPos.x = lx - c5;
        effects.mSwimTailEcallBack[1].mPos.z = lz + s5;
        gabi::Local<cXyz> local_40;
        PSMTXMultVec(lk_getAnmMtx(mpCLModel, LinkItemConstants::CL_JNT_HEAD_JNT_e), &l_swim_offset /* swim_offset */,
                     local_40);
        u32 munya = gabi::ea(effects.mpSuimenMunyaModel.get()); /* mpSuimenMunyaModel */
        Mtx34* mm = gabi::at<J3DModel>(munya)->getBaseTRMtx();
        gabi::Local<cXyz> local_4c; /* mDoMtx_multVecZero(suimenMunyaMtx, &local_4c) */
        local_4c->x = mm->m[0][3];
        local_4c->y = mm->m[1][3];
        local_40->y = mWaterY;
        local_4c->z = mm->m[2][3];
        LinkAuraApi::subtractVector(local_4c.get(), local_40.get(), local_4c.get());
        gabi::Local<cXyz> xz; /* local_4c.absXZ() */
        xz->z = local_4c->z;
        xz->y = 0.0f;
        xz->x = local_4c->x;
        f32 d = std_sqrtf(PSVECSquareMag(xz));
        f32 sz = gabi::fmadds(d / mMaxNormalSpeed, 0.5f, 1.0f);
        if (sz > 1.5f) {
            sz = 1.5f;
        }
        if (mCurProc == LinkAuraConstants::daPyProc_DEMO_DEAD_e) {
            u16 a2 = (u16)shape_angle.y;
            f32 px = current.pos.x;
            f32 pz = current.pos.z;
            f32 ly = local_40->y;
            mDoMtx_stack_c::transS(gabi::fmadds(60.0f, cM_ssin(a2), px), ly + 2.0f,
                                   gabi::fmadds(60.0f, cM_scos(a2), pz));
        } else {
            mDoMtx_stack_c::transS(local_40->x, local_40->y + 2.0f, local_40->z);
        }
        if (l_water_particle_status & LinkAuraConstants::dPaStatus_WAVE_e /* dPa_control_c::isStatus(0x01) */) {
            Mtx34* hm = lk_getAnmMtx(mpCLModel, LinkItemConstants::CL_JNT_HEAD_JNT_e);
            f32 x5c = hm->m[0][3];
            f32 z5c = hm->m[2][3];
            f32 y5c = hm->m[1][3];
            gabi::Local<cXyz> local_68;
            PSMTXMultVec(lk_getAnmMtx(mpCLModel, LinkItemConstants::CL_JNT_HEAD_JNT_e),
                         &l_swim_side_offset /* swim_side_offset */, local_68);
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
        lk_setBaseTRMtx(gabi::ea(effects.mpSuimenMunyaModel.get()), gabi::ea(mDoMtx_stack_c::get()));
        u32 m2 = gabi::ea(effects.mpSuimenMunyaModel.get()); /* setBaseScale(local_50) */
        gabi::at<J3DModel>(m2)->mBaseScale.x = 1.0f;
        gabi::at<J3DModel>(m2)->mBaseScale.z = sz;
        gabi::at<J3DModel>(m2)->mBaseScale.y = 1.0f;
    }
    /* static JGeometry::TVec3<f32> waterfall_splash_trans(0.0f, 15.0f, 0.0f) */
    u32 waterfall_splash_trans = gabi::ea(&l_waterfall_splash_trans);
    if (l_waterfall_splash_initialized == 0) {
        l_waterfall_splash_trans.z = 0.0f;
        l_waterfall_splash_trans.x = 0.0f;
        l_waterfall_splash_trans.y = 15.0f;
        l_waterfall_splash_initialized = 1;
    }
    bool wf;
    if (((mAcch.ChkGroundHit()) || (mModeFlg & LinkPoseConstants::ModeFlg_SWIM)) &&
        mCurrAttributeCode == LinkAuraConstants::dBgS_Attr_WATERFALL_e) {
        wf = true;
    } else {
        wf = (mModeFlg & LinkAuraConstants::ModeFlg_HANG) && m3588 == LinkAuraConstants::dBgS_Attr_WATERFALL_e;
    }
    if (wf) {
        if (gabi::ea(effects.m3400.mpEmitter.get()) == 0) { /* m3400.getEmitter() */
            Mtx34* hm = lk_getAnmMtx(mpCLModel, LinkItemConstants::CL_JNT_HEAD_JNT_e);
            LinkItemApi::makeMtxEmitter(&effects.m3400, LinkItemConstants::ID_AK_JN_ELEMENTSHIBUKI00, hm, &current.pos,
                                        0);
            u32 e = gabi::ea(effects.m3400.mpEmitter.get());
            if (e != 0) {
                LinkAura_emitter(e)->mSpread = 1.0f; /* setSpread */
                LinkAura_emitter(e)->mRate = 4.0f;   /* setRate */
                lk_storeVec(gabi::ea(&LinkAura_emitter(e)->mEmitterTranslation),
                            waterfall_splash_trans); /* setEmitterTranslation */
            }
        }
        if (m34CD == 0) {
            gabi::Local<cXyz> local_ac;
            lk_jntPos(mpCLModel, LinkItemConstants::CL_JNT_LINK_ROOT_e, local_ac);
            /* dComIfGp_particle_setSingleRipple(dPa_name::ID_IT_JN_WP_HAMON03, &local_ac, NULL,
             * &waterfall_ripple_scale) */
            dPa_control_set(dComIfGp_getParticle(), 5, LinkAuraConstants::ID_IT_JN_WP_HAMON03, local_ac, nullptr,
                            &l_waterfall_ripple_scale, 0xFF,
                            reinterpret_cast<dPa_levelEcallBack*>(&l_waterfall_ripple_callback), -1, nullptr, nullptr,
                            nullptr);
            m34CD = 15;
            gabi::Local<cXyz> v;
            v->x = 0.0f;
            v->y = 1.0f;
            v->z = 0.0f;
            LinkAuraApi::StartQuake(dComIfGp_getVibration(), 4, 1, v.get());
        } else {
            u8 n = (u8)(m34CD - 1);
            m34CD = n;
            if (n == 5) {
                LinkAuraApi::StopQuake(dComIfGp_getVibration(), -1);
            }
        }
    } else {
        if (gabi::ea(effects.m3400.mpEmitter.get()) != 0) {
            LinkAuraApi::StopQuake(dComIfGp_getVibration(), -1);
        }
        LinkItemApi::endMtxFollow(&effects.m3400); /* m3400.end() */
    }
    if (gabi::ea(effects.m3400.mpEmitter.get()) != 0) {
        gabi::Local<GXColor> amb;
        gabi::Local<GXColor> dif;
        LinkItemApi::getSeaColor(amb.get(), dif.get());
        u32 e = gabi::ea(effects.m3400.mpEmitter.get()); /* setGlobalPrmColor(amb.r, amb.g, amb.b) */
        u8 bl = amb->b;
        u8 g = amb->g;
        u8 r = amb->r;
        LinkAura_emitter(e)->mGlobalPrmColor.g = g;
        LinkAura_emitter(e)->mGlobalPrmColor.b = bl;
        LinkAura_emitter(e)->mGlobalPrmColor.r = r;
    }
}

/* (s16)ftoi(f) (stfiwx, then the low halfword) */
static inline s16 lk_ftos(f32 f) { return (s16)gabi::ftoi(f); }

/* 024022D0 */
void daPy_lk_c::setHatAngle() {
    if (LinkPose(this).m_old_fdata->mOldFrameFlg.get() == 0 /* m_old_fdata->getOldFrameFlg() */ ||
        (mNoResetFlg1.get() & daPyFlg1_FREEZE_STATE)) {
        return;
    }
    Mtx34* head_jnt_mtx = mpCLModel->getAnmMtx(LinkPoseConstants::CL_JNT_HEAD_JNT_e);
    BOOL r31 = TRUE;
    if (!LinkPoseApi::startCheckOld(dComIfGp_getPEvtManager(), "TACT_WINDOW2") &&
        !LinkPoseApi::startCheckOld(dComIfGp_getPEvtManager(), "TACT_WINDOW2_SHIP") &&
        mCurProc != LinkPoseConstants::daPyProc_FAN_GLIDE_e && !LinkPoseApi::ChkTgHit(&mWindCyl)) {
        r31 = FALSE;
    }
    gabi::Local<cXyz> sp70;
    gabi::Local<cXyz> sp64;
    PSMTXMultVec(head_jnt_mtx, &l_head_center_offset, sp70);
    PSMTXMultVec(head_jnt_mtx, &l_eye_offset, sp64);
    gabi::Local<cXyz> sp58;
    cXyz_mi(sp64, sp58, sp70);
    s16 r3_3 = cM_atan2s(sp58->x, sp58->z);
    if (sp70->y - head_jnt_mtx->m[1][3] < 0.0f) {
        r3_3 = (s16)(r3_3 - 0x8000);
    }
    f32 f31 = cM_ssin((u16)r3_3);
    f32 f30 = cM_scos((u16)r3_3);
    gabi::Local<cXyz> spA0;
    lk_jntPos(mpCLModel, LinkPoseConstants::CL_JNT_HATA_JNT_e, spA0);
    gabi::Local<cXyz> spAC;
    gabi::Local<be<f32>> sp18;
    LinkPoseApi::dKyw_get_AllWind_vec(spA0.get(), spAC.get(), sp18.get());
    if (r31) {
        *sp18 = 1.0f;
    }
    f32 w = *sp18;
    f32 f29 = (w * w) * 25.0f;
    gabi::Local<cXyz> sp88;
    gabi::Local<cXyz> sp7C;
    PSMTXMultVecSR(mpCLModel->getAnmMtx(LinkPoseConstants::CL_JNT_HEAD_JNT_e), &l_neck_front, sp88);
    PSMTXMultVecSR(mpCLModel->getAnmMtx(LinkPoseConstants::CL_JNT_HEAD_JNT_e), &l_neck_top, sp7C);
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
    if (!(mModeFlg & LinkPoseConstants::ModeFlg_WHIDE) || field_0x2b0 < 0.0f /* checkGrabWear() */) {
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
    f32 dist = std_sqrtf(LinkPoseApi::PSVECSquareDistance(&m3718, spA0.get()));
    f32 f28_2 = gabi::fmadds(dist, 0.65f, f29) / 30.0f;
    if (f28_2 > 1.0f) {
        f28_2 = 1.0f;
    }
    f32 f29_2 = f28_2;
    if (r31 && !(mNoResetFlg1.get() & daPyFlg1_CASUAL_CLOTHES)) {
        f28_2 = 3.5f;
        f29_2 = 1.0f;
        mDoAud_seStart(LinkPoseConstants::JA_SE_LK_HAT_SWING, &eyePos, 0, (s32)(s8)mReverb);
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
    if (mf & LinkPoseConstants::ModeFlg_SWIM) {
        m351A = 0;
        m3516 = 0;
        m3518 = 0;
        return;
    }
    f32 ax = spAC->x;
    f32 az = spAC->z;
    f32 sz = gabi::fmadds(ax, f31, az * f30); /* sp4C.z */
    f32 sx = gabi::fmsubs(ax, f30, az * f31); /* sp4C.x */
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

/* 02400C4C */
void daPy_lk_c::setNeckAngle() {
    offNoResetFlg1(daPyFlg1_UNK400);
    Mtx34* r25m = mpCLModel->getAnmMtx(LinkPoseConstants::CL_JNT_HEAD_JNT_e);
    gabi::Local<be<u32>> sp18; // HD: guest cXyz* slot; retain the original four-byte stack object.
    *sp18 = 0;
    s16 r30 = 0; /* (GameCube names) */
    s16 r29 = 0;
    BOOL r28 = FALSE;
    gabi::Local<cXyz> sp94;
    PSMTXMultVecSR(mpCLModel->getAnmMtx(LinkPoseConstants::CL_JNT_CENTER_e), &l_baseZ, sp94);
    fopAc_ac_c* r24;
    if (dComIfGp_checkPlayerStatus0(0, LinkPoseConstants::daPyStts0_UNK10_e)) {
        r24 = LinkPoseApi::fopAcM_getTalkEventPartner(this);
    } else {
        r24 = mpAttnActorLockOn;
    }
    if (r24 == 0) {
        r24 = LinkPoseApi::getForceLockOnActor(LinkPoseApi::getCameraBody());
    }
    void* lockonList = LinkPoseApi::getLockonList(gabi::at<void>(mpAttention), 0);
    fopAc_ac_c* r23_2 = NULL;
    // HD: getActor is called once for the selected list entry.
    if (lockonList != NULL) {
        r23_2 = LinkPoseApi::getLockonActor(lockonList);
    }
    gabi::Local<cXyz> spA0;
    gabi::Local<cXyz> sp98;
    if (dComIfGp_getPlayerStatus1() & LinkPoseConstants::daPyStts1_WIND_WAKER_CONDUCT_e) {
        PSMTXMultVec(mpCLModel->getAnmMtx(LinkPoseConstants::CL_JNT_CL_LHANDA_e), &l_tact_top, spA0);
        *sp18 = gabi::ea(spA0.get());
        r28 = TRUE;
    } else if (mCurProc == LinkPoseConstants::daPyProc_CALL_e /* daPyProc_CALL_e */) {
        if (gabi::ea(dComIfGp_getCb1Player()) != 0) { /* dComIfGp_getCb1Player() */
            *sp18 = gabi::ea(dComIfGp_getCb1Player()) + 0x37C;
            r28 = TRUE;
        }
    } else if (dComIfGp_getPlayerStatus1() & 0x100000) {
        /* HD: the selfie pose looks at the camera (eye + offset); face 0x92 does not move the eyes */
        s32 idx = mCameraInfoIdx;
        LinkPoseCamera* cam = reinterpret_cast<LinkPoseCamera*>(dComIfGp_getCamera(idx));
        cXyz_pl(&cam->mEye, sp98, &cam->mSelfieOffset);
        *sp18 = gabi::ea(sp98.get());
        r28 = mFace != 0x92;
    } else if (dComIfGp_checkPlayerStatus0(0, LinkPoseConstants::daPyStts0_SHIP_RIDE_e) &&
               !LinkPoseApi::checkShipNotNormalMode(this)) {
        bool shipLook = false;
        if (dComIfGp_checkPlayerStatus0(0, LinkPoseConstants::daPyStts0_UNK10_e) &&
            LinkPoseApi::fopAcM_getTalkEventPartner(this) != 0 && gabi::ea(dComIfGp_getShipActor()) != 0) {
            fopAc_ac_c* ship = dComIfGp_getShipActor();
            if (LinkPoseApi::fopAcM_getTalkEventPartner(this) == ship) {
                fopAc_ac_c* ship2 = dComIfGp_getShipActor();
                if (cLib_distanceAngleS(cLib_targetAngleY(&current.pos, &ship2->eyePos), m34DE) <= 0x6000) {
                    shipLook = true;
                }
            }
        }
        if (shipLook) {
            *sp18 = gabi::ea(dComIfGp_getShipActor()) + 0x37C;
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
    } else if (mCurProc == LinkPoseConstants::daPyProc_SHIP_CRANE_e) {
        if (mProcVar2 == 0) {
            fopAc_ac_c* ship = dComIfGp_getShipActor();
            if (ship != 0) {
                *sp18 = reinterpret_cast<LinkPoseShip*>(ship)->mpCraneTop.v.get(); /* getCraneTop() */
                r28 = TRUE;
            }
        }
    } else if (LinkPose(this).m_tex_anm_heap.mIdx.get() ==
               LinkPoseConstants::dRes_INDEX_LKANM_BTP_TDASHKAZE_e) {
        gabi::Local<cXyz> xz;
        bool done = false;
        if (LinkPoseApi::checkHeavyStateOn(this)) {
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
            if (!LinkPoseApi::checkHeavyStateOn(this)) {
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
    } else if (r24 != 0 &&
               cLib_distanceAngleS(cLib_targetAngleY(&current.pos, &r24->eyePos), m34DE) <= 0x6000) {
        *sp18 = gabi::ea(&r24->eyePos);
        r28 = TRUE;
    } else if (mModeFlg & (LinkPoseConstants::ModeFlg_00000080 | LinkPoseConstants::ModeFlg_08000000)) {
        u32 mode = 0;
        if (!dComIfGp_event_runCheck() /* !dComIfGp_event_runCheck() */ ||
            (mode = mDemo.getDemoMode()) == daPy_demo_c::DEMO_RD_STOP_e) {
            /* dComIfGp_att_getLookTarget() */
            if (LinkPose_getLookTarget() != NULL) {
                fopAc_ac_c* lt = LinkPose_getLookTarget();
                *sp18 = gabi::ea(&lt->eyePos);
                r28 = TRUE;
            } else if (LinkPoseApi::checkAttentionPosAngle(this, r23_2, sp18.get()) ||
                       LinkPoseApi::checkAttentionPosAngle(this, mpAttnActorAction.get(), sp18.get()) ||
                       LinkPoseApi::checkAttentionPosAngle(this, mpAttnActorX.get(), sp18.get()) ||
                       LinkPoseApi::checkAttentionPosAngle(this, mpAttnActorY.get(), sp18.get()) ||
                       LinkPoseApi::checkAttentionPosAngle(this, mpAttnActorZ.get(),
                                                           sp18.get())) { /* checkAttentionPosAngle */
                r28 = TRUE;
            } else if (LinkPoseApi::detectAttention(LinkPose_getDetect(), spA0.get()) &&
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
        } else if (mCurProc == LinkPoseConstants::daPyProc_BOTTLE_OPEN_e) {
            if (mActorKeepRope.mActor != NULL) {
                *sp18 = gabi::ea(mActorKeepRope.mActor.get()) + 0x37C;
                r28 = TRUE;
            }
        } else if (mCurProc == LinkPoseConstants::daPyProc_DEMO_LOOK_WAIT_e ||
                   (u32)(mode - 1) <= 3 /* INIT_WAIT, N_WAIT, N_WALK, N_DASH */) {
            fopAc_ac_c* la = LinkPoseApi::getDemoLookActor(this);
            if (la != 0) {
                *sp18 = gabi::ea(&la->eyePos);
                r28 = TRUE;
            }
        }
    }
    gabi::Local<cXyz> spC4;
    gabi::Local<cXyz> sp88;
    PSMTXMultVec(r25m, &l_head_center_offset, spC4);
    PSMTXMultVec(r25m, &l_eye_offset, sp88);
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
    if ((mModeFlg & (LinkPoseConstants::ModeFlg_00000080 | LinkPoseConstants::ModeFlg_08000000)) &&
        *sp18 != 0 &&
        LinkPose(this).m_anm_heap_upper[2].mIdx.get() != LinkPoseConstants::dRes_INDEX_LKANM_BCK_DAMDASH_e) {
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
        if ((mModeFlg & LinkPoseConstants::ModeFlg_00000080) &&
            LinkPose(this).m_anm_heap_upper[0].mIdx.get() !=
                LinkPoseConstants::dRes_INDEX_LKANM_BCK_DASHKAZE_e &&
            LinkPose(this).m_anm_heap_upper[1].mIdx.get() !=
                LinkPoseConstants::dRes_INDEX_LKANM_BCK_DASHKAZE_e) {
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
    } else if (mCurProc == LinkPoseConstants::daPyProc_DAMAGE_e) {
        r4 = m3564.x;
        r23 = m3564.y;
    } else if ((mCurProc == LinkPoseConstants::daPyProc_LADDER_MOVE_e ||
                mCurProc == LinkPoseConstants::daPyProc_CLIMB_MOVE_UP_DOWN_e) &&
               mDirection == 0 /* DIR_FORWARD */) {
        r4 = -0x1800;
        r23 = 0;
    } else if (mCurProc == LinkPoseConstants::daPyProc_CLIMB_MOVE_SIDE_e) {
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
    if (mCurProc == LinkPoseConstants::daPyProc_SHIP_CRANE_e) {
        r23 = 0;
        r29 = 0;
    }
    cLib_addCalcAngleS(&m3564.x, r4, 3, 0x1000, 0x100);
    cLib_addCalcAngleS(&m3564.y, r23, 3, 0x1000, 0x100);
    if (mModeFlg & LinkPoseConstants::ModeFlg_00000080) {
        s16 v = (s16)(r25_3 + m3564.y);
        if (v > 0x3800) {
            m3564.y = (s16)(0x3800 - r25_3);
        } else if (v < -0x3800) {
            m3564.y = (s16)(-0x3800 - r25_3);
        }
    }
    if (mCurProc != LinkPoseConstants::daPyProc_DAMAGE_e) {
        cLib_addCalcAngleS(&m3564.z, 0, 3, 0x1000, 0x100);
    }
    f32 f31v = 0.0f; /* eye 0 x */
    f32 f30v = 0.0f; /* eye 1 x */
    f32 f29v = 0.0f; /* eye y */
    bool eyes;
    s32 proc = mCurProc;
    if (r28 || proc == LinkPoseConstants::daPyProc_FAN_GLIDE_e ||
        proc == LinkPoseConstants::daPyProc_ICE_SLIP_ALMOST_FALL_e ||
        proc == LinkPoseConstants::daPyProc_CLIMB_MOVE_SIDE_e || m34C3 == 9 ||
        ((proc == LinkPoseConstants::daPyProc_GRAB_HEAVY_WAIT_e /* GRAB_HEAVY_WAIT */ ||
          proc == LinkPoseConstants::daPyProc_GRAB_WAIT_e /* GRAB_WAIT */ ||
          proc == LinkPoseConstants::daPyProc_WAIT_e /* WAIT */) &&
         !(m35A0 == 0.0f))) {
        f32 f0;
        f32 f1;
        if (r28) {
            f0 = (f32)(s32)r30 * 0.00012207031f;
            f1 = (f32)(s32)r29 * 0.00012207031f;
        } else if (proc == LinkPoseConstants::daPyProc_FAN_GLIDE_e) {
            gabi::Local<cXyz> sp58;
            cXyz_mi(&current.pos, sp58, &old.pos);
            s16 a = cM_atan2s(sp58->x, sp58->z);
            f1 = (f32)(s32)(-(s32)m34F4) / 6144.0f;
            f0 = cM_scos((u16)(a - shape_angle.y));
        } else if (proc == LinkPoseConstants::daPyProc_ICE_SLIP_ALMOST_FALL_e) {
            u16 a = (u16)(current.angle.y - shape_angle.y);
            f0 = std::fabs(cM_scos(a));
            f1 = cM_ssin(a);
        } else if (proc == LinkPoseConstants::daPyProc_CLIMB_MOVE_SIDE_e) {
            u16 a = (u16)mProcVar2;
            f0 = -cM_scos(a);
            f1 = cM_ssin(a);
        } else if (proc == LinkPoseConstants::daPyProc_GRAB_HEAVY_WAIT_e ||
                   proc == LinkPoseConstants::daPyProc_GRAB_WAIT_e ||
                   proc == LinkPoseConstants::daPyProc_WAIT_e) {
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
        m_morf_frame = 0;   /* daPy_matAnm_c::setMorfFrame(0) */
        m_eye_move_flg = 1; /* daPy_matAnm_c::onEyeMoveFlg() */
        f29v = f0 * fy;
        eyes = true;
    } else {
        s16 m = m351C;
        f32 f1 = (f32)(s32)m * 0.00024414062f; /* m351C / 4096.0f */
        u32 fl = mNoResetFlg1.get();
        if (f1 > 1.0f) {
            f1 = 1.0f;
        } else if (f1 < -1.0f) {
            f1 = -1.0f;
        }
        if (!(fl & (daPyFlg1_CONFUSE | daPyFlg1_UNK1000000)) && m != 0) {
            if (m < 0) {
                f31v = -0.12f * f1;
                f30v = 0.16f * f1;
            } else {
                f31v = -0.16f * f1;
                f30v = 0.12f * f1;
            }
            m_eye_move_flg = 1;
            eyes = true;
        } else {
            LinkPoseMatAnm* e0 = LinkPose(this).m_tex_eye_scroll[0];
            e0->mNowOffsetX = 0.0f;
            LinkPose(this).m_tex_eye_scroll[1]->mNowOffsetX = 0.0f;
            LinkPose(this).m_tex_eye_scroll[0]->mNowOffsetY = 0.0f;
            LinkPose(this).m_tex_eye_scroll[1]->mNowOffsetY = 0.0f;
            if (m_eye_move_flg != 0) {
                m_morf_frame = 3; /* setMorfFrame(3) */
            }
            m_eye_move_flg = 0; /* offEyeMoveFlg() */
            eyes = false;
        }
        f29v = 0.0f;
    }
    if (eyes) {
        LinkPoseApi::addCalc(&LinkPose(this).m_tex_eye_scroll[0]->mNowOffsetX, f31v, 0.5f, 0.1f, 0.03f);
        LinkPoseApi::addCalc(&LinkPose(this).m_tex_eye_scroll[1]->mNowOffsetX, f30v, 0.5f, 0.1f, 0.03f);
        LinkPoseApi::addCalc(&LinkPose(this).m_tex_eye_scroll[0]->mNowOffsetY, f29v, 0.5f, 0.08f, 0.02f);
        LinkPoseMatAnm* e0 = LinkPose(this).m_tex_eye_scroll[0];
        LinkPoseMatAnm* e1 = LinkPose(this).m_tex_eye_scroll[1];
        e1->mNowOffsetY = (f32)e0->mNowOffsetY;
    }
}

// HD: texture animation frames also update the renderer's interpolation channel.
static inline void lk_btkStep(LinkItemBtk* btk, f32 frame) {
    f32 maxFrame = (f32)btk->mFrameCtrl.mEnd;
    f32 nextFrame = gabi::fadds_ppc(frame, 1.0f);
    nextFrame = fsel_l(gabi::fsubs_ppc(nextFrame, maxFrame), 0.0f, nextFrame);
    u32 animation = btk->mpAnm;
    btk->mFrameCtrl.mFrame = nextFrame;
    gabi::at<LinkItemAnimationResource>(animation)->mFrame = nextFrame;
    lk_setAnimationFrame(&btk->mpFrameValue, nextFrame);
}
static inline void lk_swordGripAnm(daPy_lk_c* player, f32 bckFrame, f32 brkFrame) {
    J3DModelData* modelData = player->mpSwgripmsModel->mModelData;
    LinkItemApi::entryBck(&LinkItem(player).mSwgripmsabBckAnim, modelData, bckFrame);
    LinkItemAnimationResource* animation = LinkItem(player).mpTswgripmsabBrk.mpAnm;
    LinkItem(player).mpTswgripmsabBrk.mFrameCtrl.mFrame = brkFrame;
    animation->mFrame = brkFrame;
    lk_setAnimationFrame(&LinkItem(player).mpTswgripmsabBrk.mpFrameValue, brkFrame);
}
// HD: show or hide the blade material's shape, looked up by name.
static inline void lk_swordBladeShow(u32 model, u8 show) {
    gabi::Local<lk_SafeString_l> nm;
    nm->__vtbl = LK_SAFESTRING_VTBL;
    nm->mStr = gabi::guest_str("SwMSCMAT");
    J3DModelData* modelData = gabi::at<J3DModel>(model)->mModelData;
    u32 res = modelData->mpResHeader;
    gabi::call(0x02444F48, nm.get());
    u32 off = gabi::load<u32>(res + 0x18);
    u32 tbl = off != 0 ? res + 0x18 + off : 0;
    s32 idx = gabi::call<s32>(0x027DF9B0, tbl, (u32)nm->mStr);
    u32 node;
    if (idx < 0) {
        node = 0;
    } else {
        u32 n = modelData->mMaterialNum;
        u32 base = modelData->mMaterials;
        node = (u32)idx < n ? base + (u32)idx * 0x39C : base;
    }
    if (node != 0) {
        gabi::at<J3DMaterial>(node)->mShape->mVisible = show;
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

/* 02403C24 */
s32 daPy_lk_c::setItemModel() {
    u32 b = gabi::ea(this);
    u32 pod = gabi::ea(lk_getAnmMtx(mpCLModel, LinkItemConstants::CL_JNT_CL_PODA_e));
    u32 lHand = gabi::ea(lk_getAnmMtx(mpCLModel, LinkItemConstants::CL_JNT_CL_LHANDA_e));
    u32 rHand = gabi::ea(lk_getAnmMtx(mpCLModel, LinkItemConstants::CL_JNT_CL_RHANDA_e));
    u32 stk = gabi::ea(mDoMtx_stack_c::get());
    u32 boko = mActorKeepEquip.mActor.v; /* mActorKeepEquip.getActor() */
    if (boko != 0 && mEquipItem == LinkItemConstants::daPyItem_BOKO_e) {
        PSMTXTrans(mDoMtx_stack_c::get(), 40.0f, 47.0f, 2.1f);
        lk_XYZrotM(0x3E38, -0x5D4, 0x66F8);
        u32 prm = fopAcM_GetParam(gabi::at<fopAc_ac_c>(boko));
        if (prm == 2 || prm == 3) {
            mDoMtx_stack_transM(0.0f, 0.0f, 20.0f);
        } else if (prm == 4) {
            mDoMtx_stack_transM(0.0f, 0.0f, 70.0f);
        } else if (prm == 5) {
            mDoMtx_stack_transM(0.0f, 0.0f, 30.0f);
        }
        lk_stackRevConcat(lHand);
        u32 bm = gabi::at<LinkItemBokoView>(boko)->mpModel.v; /* boko->setMatrix() */
        if (bm != 0) {
            lk_setBaseTRMtx(bm, stk);
        }
    }
    if (LinkItem(this).mpEquipItemBtk.mpAnm != 0) { /* mpEquipItemBtk */
        f32 f = LinkItem(this).mpEquipItemBtk.mFrameCtrl.mFrame;
        if (f > 0.0f || cM_rnd() < 0.02f) {
            lk_btkStep(&LinkItem(this).mpEquipItemBtk, f);
        }
    }
    lk_setBaseTRMtx(LinkItem(this).mpPodmsModel.v /* mpPodmsModel */, pod);
    u32 model = LinkItem(this).mpEquipItemModel.v; /* mpEquipItemModel */
    if (model != 0) {
        u32 item = mEquipItem;
        bool lhand = false, bow = false, after = false; /* after: the bottle / hookshot / wind waker checks */
        if (item == LinkItemConstants::daPyItem_UNK10A_e) {
            PSMTXTrans(mDoMtx_stack_c::get(), 50.0f, -48.0f, -14.0f);
            lk_XYZrotM(-0x33FC, -0x2CC, 0x3519);
            u32 ship = gabi::ea(dComIfGp_getShipActor());
            u32 tact = gabi::at<LinkItemShipView>(ship)->mTactMorf->mpModel.v; /* getTactJntMtx() */
            lk_stackRevConcat(gabi::ea(lk_getAnmMtx(gabi::at<J3DModel>(tact), 10)));
            lk_setBaseTRMtx(LinkItem(this).mpEquipItemModel.v, stk);
        } else if (item == LinkItemConstants::daPyItem_UNK104_e) {
            PSMTXTrans(mDoMtx_stack_c::get(), 14.5f, 34.5f, 0.0f);
            lk_XYZrotM(0, -0x4000, -0x2AAA);
            lk_stackRevConcat(gabi::ea(lk_getAnmMtx(mpCLModel, LinkItemConstants::CL_JNT_STOMACH_JNT_e)));
            lk_setBaseTRMtx(LinkItem(this).mpEquipItemModel.v, stk);
        } else if (item == LinkItemConstants::dItemNo_TELESCOPE_e) {
            PSMTXTrans(mDoMtx_stack_c::get(), 8.0f, 15.0f, 1.0f);
            lk_XYZrotM(-0x4000, 0x4444, -0x41F);
            lk_stackRevConcat(lHand);
            lk_setBaseTRMtx(LinkItem(this).mpEquipItemModel.v, stk);
        } else if (item == LinkItemConstants::dItemNo_HYOI_PEAR_e) {
            if (mFrameCtrlUnder[0].mFrame < 9.0f) { /* mFrameCtrlUnder[UNDER_MOVE0_e].getFrame() */
                PSMTXTrans(mDoMtx_stack_c::get(), -1.5f, -44.0f, 16.0f);
                lk_XYZrotM(-0x6EEE, 0x9BE, 0x368A);
                lk_stackRevConcat(lHand);
                lk_setBaseTRMtx(LinkItem(this).mpEquipItemModel.v, stk);
            } else {
                lk_setBaseTRMtx(model,
                                gabi::ea(lk_getAnmMtx(mpCLModel, LinkItemConstants::CL_JNT_HEAD_JNT_e)));
            }
        } else if (item == LinkItemConstants::dItemNo_SKULL_HAMMER_e &&
                   LinkItem(this).mHammerSwingTimer != 0) {
            s16 t = (s16)(LinkItem(this).mHammerSwingTimer - 1);
            u32 ang = (u16)((s32)t * 0x61A8);
            LinkItem(this).mHammerSwingTimer = t;
            f32 amp = (f32)t * 300.0f;
            f32 s = cM_ssin(ang);
            s16 r = (s16)(LinkItem(this).mHammerSwingAngle + lk_ftos(s * amp));
            LinkItemApi::XYZrotS(mDoMtx_stack_c::get(), 0, 0, (s32)r);
            lk_stackRevConcat(lHand);
            lk_setBaseTRMtx(LinkItem(this).mpEquipItemModel.v, stk);
            item = mEquipItem;
            after = true;
        } else if (item == LinkItemConstants::dItemNo_SKULL_HAMMER_e ||
                   item == LinkItemConstants::dItemNo_TINGLE_TUNER_e ||
                   item == LinkItemConstants::dItemNo_WIND_WAKER_e ||
                   item == LinkItemConstants::dItemNo_HOOKSHOT_e ||
                   item == LinkItemConstants::dItemNo_DEKU_LEAF_e ||
                   item == LinkItemConstants::daPyItem_SWORD_e || checkBottleItem(item) ||
                   checkPhotoBoxItem(item)) {
            lhand = true;
        } else {
            bow = true;
        }
        if (lhand) {
            lk_setBaseTRMtx(model, lHand);
            item = mEquipItem;
            after = true;
        }
        if (after) {
            bool done = false;
            if (item == LinkItemConstants::dItemNo_DEKU_LEAF_e) {
                s32 proc = mCurProc;
                if (proc != LinkItemConstants::daPyProc_FAN_SWING_e &&
                    proc != LinkItemConstants::daPyProc_FAN_GLIDE_e) {
                    LinkItemApi::setShapeFanLeaf(this);
                    done = true;
                }
            }
            if (done) {
            } else if (checkBottleItem(item)) {
                u32 bc = LinkItem(this).mpBottleContentsModel.v; /* mpBottleContentsModel */
                if (bc != 0) {
                    if (item == LinkItemConstants::dItemNo_FIREFLY_BOTTLE_e) {
                        s16 mx = LinkItem(this).mpEquipItemBrk.mFrameCtrl.mEnd;
                        f32 fr = LinkItem(this).mpEquipItemBrk.mFrameCtrl.mFrame;
                        f32 a = (37.699112f * fr) / (f32)mx;
                        u32 ang = (u16)LinkItemApi::rad2s(a);
                        f32 sc = gabi::fmadds(0.1f, cM_ssin(ang), 1.0f);
                        PSMTXTrans(mDoMtx_stack_c::get(), 11.0f, 0.0f, 0.0f);
                        lk_stackRevConcat(lHand);
                        lk_setBaseTRMtx(LinkItem(this).mpBottleContentsModel.v, stk);
                        u32 bc2 = LinkItem(this).mpBottleContentsModel.v; /* setBaseScale */
                        gabi::at<J3DModel>(bc2)->mBaseScale.y = sc;
                        gabi::at<J3DModel>(bc2)->mBaseScale.x = sc;
                        gabi::at<J3DModel>(bc2)->mBaseScale.z = sc;
                        LinkItemApi::playAnimation(&LinkItem(this).mpEquipItemBrk);
                        u32 md = (u32)LinkItem(this).mpBottleContentsModel->mModelData.v;
                        LinkItemApi::entryBrk(&LinkItem(this).mpEquipItemBrk, gabi::at<J3DModelData>(md),
                                              (f32)LinkItem(this).mpEquipItemBrk.mFrameCtrl.mFrame);
                    } else {
                        lk_setBaseTRMtx(bc, lHand);
                    }
                }
                u32 cap = LinkItem(this).mpBottleCapModel.v; /* mpBottleCapModel */
                if (cap != 0) {
                    lk_setBaseTRMtx(cap, lHand);
                }
            } else if (item == LinkItemConstants::dItemNo_HOOKSHOT_e) {
                if (boko != 0) {
                    u32 prm = fopAcM_GetParam(gabi::at<fopAc_ac_c>(boko));
                    if (prm != 0) {
                        f32 v = m35EC; /* m35EC */
                        u32 bck = LinkItem(this).mSwordAnim.mpAnm;
                        m35EC = prm == 1 ? gabi::fadds_ppc(v, 1.0f) : gabi::fsubs_ppc(v, 1.0f);
                        s32 n = gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(bck + 4) + 0x14),
                                                    bck); /* getFrameMax() */
                        f32 mx = (f32)n;
                        f32 c = m35EC;
                        if (c < 0.0f) {
                            m35EC = gabi::fadds_ppc(c, mx);
                        } else if (!(c < mx)) {
                            m35EC = gabi::fsubs_ppc(c, mx);
                        }
                    }
                }
            } else if (item == LinkItemConstants::dItemNo_WIND_WAKER_e) {
                if (mCurProc != LinkItemConstants::daPyProc_TACT_WAIT_e &&
                    LinkItem(this).mpEquipItemBrk.mpAnm.v != 0) {
                    f32 f = gabi::fsubs_ppc(LinkItem(this).mpEquipItemBrk.mFrameCtrl.mFrame, 1.0f);
                    f = fsel_l(f, f, 0.0f);
                    u32 brk = LinkItem(this).mpEquipItemBrk.mpAnm.v;
                    LinkItem(this).mpEquipItemBrk.mFrameCtrl.mFrame = f;
                    gabi::at<LinkItemAnimationResource>(brk)->mFrame = f;
                    lk_setAnimationFrame(&LinkItem(this).mpEquipItemBrk.mpFrameValue, f);
                }
            }
        }
        if (bow) {
            if (checkBowItem(item)) {
                lk_setBaseTRMtx(model, rHand);
            } else if (item == LinkItemConstants::daPyItem_UNK102_e) {
                lk_setBaseTRMtx(model,
                                gabi::ea(lk_getAnmMtx(mpCLModel, LinkItemConstants::CL_JNT_CHEST_JNT_e)));
            }
        }
        if (LinkItem(this).mSwordAnim.mpAnm != 0 &&
            mCurProc != LinkItemConstants::daPyProc_DEMO_TALISMAN_WAIT_e) { /* mSwordAnim.getBckAnm() */
            LinkItemApi::entryBck(&LinkItem(this).mSwordAnim, LinkItem(this).mpEquipItemModel->mModelData,
                                  (f32)m35EC);
            u32 s1 = LinkItem(this).mpSwordModel1.v; /* mpSwordModel1 */
            if (s1 != 0) {
                LinkItemApi::entryBck(&LinkItem(this).mSwordAnim, gabi::at<J3DModel>(s1)->mModelData,
                                      (f32)m35EC);
            }
        }
        u32 morf = LinkItem(this).mpParachuteFanMorf.v; /* mpParachuteFanMorf */
        if (morf != 0) {
            gabi::at<mDoExt_McaMorf>(morf)->calc();
        } else {
            LinkItem(this).mpEquipItemModel->calc();
        }
    }
    /* the sword */
    mpEquippedSwordModel.v = mpSwgripmsModel.v; /* mpEquippedSwordModel = mpSwgripmsModel */
    s32 swordType;
    s32 grip; /* -1: the normal sword; else the bck frame / brk frame choice below */
    u8 sel = lk_selectSword();
    if (sel == 0xFF) {
        if (dComIfGp_getMiniGameType() != 2) {
            grip = 0; /* no sword */
        } else if (lk_selectSword() == 0x38 || dComIfGp_getMiniGameType() == 2) {
            grip = -1;
        } else {
            grip = 1;
        }
    } else if (sel == 0x38 || dComIfGp_getMiniGameType() == 2) {
        grip = -1;
    } else {
        grip = 1;
    }
    if (grip == 1 && mDemo.mDemoType != 0 && lk_stageIs_b(gabi::guest_str("Demo50"))) {
        grip = -1;
    }
    f32 gripFrame;
    if (grip == -1) { /* checkNormalSwordEquip() */
        gripFrame = LinkItem(this).mpTswgripmsBtk.mFrameCtrl.mFrame;
        mpEquippedSwordModel.v = mpSwgripaModel.v; /* mpSwgripaModel */
        swordType = 1;
    } else if (grip == 0) {
        swordType = 0;
        lk_swordGripAnm(this, 0.0f, 0.0f);
        gripFrame = LinkItem(this).mpTswgripmsBtk.mFrameCtrl.mFrame;
    } else {
        u8 s = lk_selectSword();
        if (s == 0x3E) { /* checkFinalMasterSwordEquip() */
            swordType = 3;
            lk_swordGripAnm(this, 1.0f, 1.0f);
        } else if (s == 0x3A) { /* dItemNo_MASTER_SWORD_2_e */
            swordType = 4;
            lk_swordGripAnm(this, 1.0f, 0.0f);
        } else {
            swordType = 2;
            lk_swordGripAnm(this, 0.0f, 0.0f);
        }
        u32 m = LinkItem(this).mpEquipItemModel.v;
        if (m != 0 && mEquipItem == LinkItemConstants::daPyItem_SWORD_e) {
            lk_swordBladeShow(m, swordType == 3 ? 1 : 0);
        }
        gripFrame = LinkItem(this).mpTswgripmsBtk.mFrameCtrl.mFrame;
    }
    if (gripFrame > 0.1f || cM_rnd() < 0.02f) { /* mpTswgripmsBtk */
        lk_btkStep(&LinkItem(this).mpTswgripmsBtk, gripFrame);
    }
    if (mEquipItem == LinkItemConstants::daPyItem_SWORD_e) {
        s32 proc = mCurProc;
        u32 sm = mpEquippedSwordModel.v;
        if (proc == LinkItemConstants::daPyProc_DEMO_LAST_COMBO_e) {
            lk_setBaseTRMtx(sm,
                            gabi::ea(lk_getAnmMtx(gabi::at<J3DModel>(LinkItem(this).mpEquipItemModel.v), 0)));
        } else {
            lk_setBaseTRMtx(sm, lHand);
        }
        LinkItemApi::setSwordSoundType(swordType, 1);
    } else {
        PSMTXTrans(mDoMtx_stack_c::get(), -11.25f, 4.5f, 0.45f);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), -0x1C71);
        lk_stackRevConcat(pod);
        lk_setBaseTRMtx(mpEquippedSwordModel.v, stk);
        LinkItemApi::setSwordSoundType(swordType, 2);
    }
    mpEquippedSwordModel->calc();
    /* the shield */
    s32 shieldType;
    u8 shield = dComIfGs_getSelectEquip(1);
    if (shield == 0x3C) {                          /* checkMirrorShieldEquip() */
        u32 btk = LinkItem(this).mpTshmsBtk.mpAnm; /* mpTshmsBtk */
        LinkItem(this).mpEquippedShieldModel.v =
            LinkItem(this).mpShmsModel.v; /* mpEquippedShieldModel = mpShmsModel */
        shieldType = 2;
        if (btk != 0) {
            f32 f = LinkItem(this).mpTshmsBtk.mFrameCtrl.mFrame;
            if (f > 0.0f || cM_rnd() < 0.02f) {
                lk_btkStep(&LinkItem(this).mpTshmsBtk, f);
            }
        }
    } else {
        LinkItem(this).mpEquippedShieldModel.v = LinkItem(this).mpShaModel.v; /* mpShaModel */
        shieldType = shield == 0xFF ? 0 : 1;
    }
    s32 proc = mCurProc;
    s32 place; /* 0: Link's base matrix (HD proc 0xD8), 1: right hand, 2: on the back */
    if (proc == LinkItemConstants::daPyProc_DEMO_LAST_COMBO_e) {
        place = 0;
    } else if (proc == LinkItemConstants::daPyProc_DEMO_TOOL_e
                   ? mProcVar2 == 1
                   : mEquipItem == LinkItemConstants::daPyItem_SWORD_e) {
        place = 1;
    } else if (gabi::call_ptr<BOOL>(gabi::load<u32>(__vtbl + 0x3C), b) /* checkPlayerGuard() */ &&
               mEquipItem != LinkItemConstants::dItemNo_SKULL_HAMMER_e) {
        place = 1;
    } else if (mCurProc == LinkItemConstants::daPyProc_GUARD_CRASH_e) {
        place = 1;
    } else {
        place = 2;
    }
    if (place == 0) {
        u32 cl = mpCLModel.v;
        u32 sh = LinkItem(this).mpEquippedShieldModel.v;
        lk_setBaseTRMtx(sh, cl != 0 ? cl + 0xC8 : 0);
        LinkItemApi::setShieldSoundType(shieldType, 1);
    } else if (place == 1) {
        lk_setBaseTRMtx(LinkItem(this).mpEquippedShieldModel.v, rHand);
        LinkItemApi::setShieldSoundType(shieldType, 1);
    } else {
        PSMTXTrans(mDoMtx_stack_c::get(), 15.5f, 4.75f, shieldType == 2 ? -0.2f : 0.0f);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), -0x4000);
        lk_stackRevConcat(pod);
        lk_setBaseTRMtx(LinkItem(this).mpEquippedShieldModel.v, stk);
        u32 idx = LinkPose(this).m_anm_heap_under[0].mIdx; /* m_anm_heap_under[UNDER_MOVE0_e].mIdx */
        if (shieldType == 2 && (idx == 0x70 || idx == 0x74 || idx == 0x12E || idx == 0x135)) {
            f32 fr = mFrameCtrlUnder[0].mFrame;
            s16 end = mFrameCtrlUnder[0].mEnd;
            f32 a = (6.2831855f * gabi::fsubs_ppc(fr, 22.0f)) / (f32)end;
            /* mDoMtx_stack_c::multVecZero(&temp) */
            u32 t0 = lk_fbits(gabi::load<f32>(stk + 0xC));
            u32 t1 = lk_fbits(gabi::load<f32>(stk + 0x1C));
            u32 t2 = lk_fbits(gabi::load<f32>(stk + 0x2C));
            u32 ang = (u16)LinkItemApi::rad2s(a);
            f32 r = mNormalSpeed / mMaxNormalSpeed; /* mNormalSpeed / mMaxNormalSpeed */
            f32 y = (-5500.0f * fabsf(r)) * cM_ssin(ang);
            LinkItemApi::YrotS(mDoMtx_stack_c::get(), (s32)lk_ftos(y));
            u32 sh = LinkItem(this).mpEquippedShieldModel.v;
            LinkItemApi::concatShieldMtx(mDoMtx_stack_c::get(),
                                         sh != 0 ? gabi::at<J3DModel>(sh)->getBaseTRMtx() : nullptr,
                                         mDoMtx_stack_c::get());
            gmem_stf32(stk + 0xC, t0);
            gmem_stf32(stk + 0x1C, t1);
            gmem_stf32(stk + 0x2C, t2);
            lk_setBaseTRMtx(LinkItem(this).mpEquippedShieldModel.v, stk);
        }
        LinkItemApi::setShieldSoundType(shieldType, 2);
    }
    LinkItemApi::entryBck(&LinkItem(this).mAtngshaBck, LinkItem(this).mpEquippedShieldModel->mModelData,
                          (f32)m35E8);
    LinkItem(this).mpEquippedShieldModel->calc();
    {
        u32 sh = LinkItem(this).mpEquippedShieldModel.v;
        u32 ym = LinkItem(this).mpYmsls00Model.v; /* mpYmsls00Model */
        lk_setBaseTRMtx(ym, sh != 0 ? sh + 0xC8 : 0);
    }
    LinkItemApi::playAnimation(&LinkItem(this).mpYmsls00Btk); /* simpleAnmPlay(mpYmsls00Btk) */
    {
        u32 anm = LinkItem(this).mpYmsls00Btk.mpAnm;
        f32 f = LinkItem(this).mpYmsls00Btk.mFrameCtrl.mFrame;
        gabi::store<f32>(anm, f);
        lk_setAnimationFrame(&LinkItem(this).mpYmsls00Btk.mpFrameValue, f);
    }
    if (mNoResetFlg0 & LinkItemConstants::daPyFlg0_EQUIP_HEAVY_BOOTS) { /* checkEquipHeavyBoots() */
        LinkItemApi::setBootsModel(this, LinkItem(this).mpHbootsModels);
        LinkItemApi::setBootsSoundType(1);
    } else {
        LinkItemApi::setBootsSoundType(0);
    }
    if (dComIfGs_getSelectEquip(2) == 0x28) { /* checkPowerGloveEquip() */
        lk_copyJntMtx(mpCLModel, LinkItemConstants::CL_JNT_LARMB_JNT_e,
                      gabi::at<J3DModel>(LinkItem(this).mpPringModel.v), 1);
        lk_copyJntMtx(mpCLModel, LinkItemConstants::CL_JNT_RARMB_JNT_e,
                      gabi::at<J3DModel>(LinkItem(this).mpPringModel.v), 2);
    }
    return 0;
}

static LinkCollisionSinCosTable l_collisionSinCos;
static cXyz l_ms_light_local_start;
static GXColor n_roll_prm0;
static u32 n_roll_prm0_initialized;
static GXColor n_roll_prm1;
static u32 n_roll_prm1_initialized;
static GXColor n_roll_env;
static u32 n_roll_env_initialized;
static GXColor nm_roll_prm0;
static u32 nm_roll_prm0_initialized;
static GXColor nm_roll_prm1;
static u32 nm_roll_prm1_initialized;
static GXColor nm_roll_env;
static u32 nm_roll_env_initialized;
static GXColor fm_roll_prm0;
static u32 fm_roll_prm0_initialized;
static GXColor fm_roll_prm1;
static u32 fm_roll_prm1_initialized;
static GXColor fm_roll_env;
static u32 fm_roll_env_initialized;
static GXColor n_turn_prm0;
static u32 n_turn_prm0_initialized;
static GXColor n_turn_prm1;
static u32 n_turn_prm1_initialized;
static GXColor n_turn_env;
static u32 n_turn_env_initialized;
static GXColor nm_turn_prm0;
static u32 nm_turn_prm0_initialized;
static GXColor nm_turn_prm1;
static u32 nm_turn_prm1_initialized;
static GXColor nm_turn_env;
static u32 nm_turn_env_initialized;
static GXColor fm_turn_prm0;
static u32 fm_turn_prm0_initialized;
static GXColor fm_turn_prm1;
static u32 fm_turn_prm1_initialized;
static GXColor fm_turn_env;
static u32 fm_turn_env_initialized;

// HD: cut colors are gamma corrected once, after setting each guard word.
static inline void LinkCollision_gammaByte(be<u8>* dst, u32 value) {
    f32 gamma = (f32)LinkCollisionApi::colorGamma(value);
    *dst = (u8)gabi::ftoi(gamma * 255.0f);
}
static inline void LinkCollision_cutColorStatics() {
    if (n_roll_prm0_initialized == 0) {
        n_roll_prm0_initialized = 1;
        LinkCollision_gammaByte(&n_roll_prm0.r, 0xC8);
        LinkCollision_gammaByte(&n_roll_prm0.b, 0x40);
    }
    if (n_roll_prm1_initialized == 0) {
        n_roll_prm1_initialized = 1;
        LinkCollision_gammaByte(&n_roll_prm1.b, 0xDC);
    }
    if (n_roll_env_initialized == 0) {
        n_roll_env_initialized = 1;
        LinkCollision_gammaByte(&n_roll_env.r, 0xC8);
        LinkCollision_gammaByte(&n_roll_env.b, 0x78);
    }
    if (nm_roll_prm0_initialized == 0) {
        nm_roll_prm0_initialized = 1;
        LinkCollision_gammaByte(&nm_roll_prm0.r, 0xC8);
        LinkCollision_gammaByte(&nm_roll_prm0.b, 0x40);
    }
    if (nm_roll_prm1_initialized == 0) {
        nm_roll_prm1_initialized = 1;
        LinkCollision_gammaByte(&nm_roll_prm1.b, 0xDC);
    }
    if (nm_roll_env_initialized == 0) {
        nm_roll_env_initialized = 1;
        LinkCollision_gammaByte(&nm_roll_env.r, 0xC8);
        LinkCollision_gammaByte(&nm_roll_env.b, 0x78);
    }
    if (fm_roll_prm0_initialized == 0) {
        fm_roll_prm0_initialized = 1;
        LinkCollision_gammaByte(&fm_roll_prm0.r, 0xC8);
        LinkCollision_gammaByte(&fm_roll_prm0.b, 0x40);
    }
    if (fm_roll_prm1_initialized == 0) {
        fm_roll_prm1_initialized = 1;
        LinkCollision_gammaByte(&fm_roll_prm1.b, 0xDC);
    }
    if (fm_roll_env_initialized == 0) {
        fm_roll_env_initialized = 1;
        LinkCollision_gammaByte(&fm_roll_env.r, 0xC8);
        LinkCollision_gammaByte(&fm_roll_env.b, 0x78);
    }
    if (n_turn_prm0_initialized == 0) {
        n_turn_prm0_initialized = 1;
        LinkCollision_gammaByte(&n_turn_prm0.r, 0x40);
        LinkCollision_gammaByte(&n_turn_prm0.g, 0x60);
        LinkCollision_gammaByte(&n_turn_prm0.b, 0x40);
    }
    if (n_turn_prm1_initialized == 0) {
        n_turn_prm1_initialized = 1;
        LinkCollision_gammaByte(&n_turn_prm1.r, 0xDC);
        LinkCollision_gammaByte(&n_turn_prm1.g, 0xE6);
        LinkCollision_gammaByte(&n_turn_prm1.b, 0xDC);
    }
    if (n_turn_env_initialized == 0) {
        n_turn_env_initialized = 1;
        LinkCollision_gammaByte(&n_turn_env.r, 0xA0);
        LinkCollision_gammaByte(&n_turn_env.g, 0xE6);
        LinkCollision_gammaByte(&n_turn_env.b, 0xA0);
    }
    if (nm_turn_prm0_initialized == 0) {
        nm_turn_prm0_initialized = 1;
        LinkCollision_gammaByte(&nm_turn_prm0.r, 0x40);
        LinkCollision_gammaByte(&nm_turn_prm0.g, 0x40);
        LinkCollision_gammaByte(&nm_turn_prm0.b, 0x60);
    }
    if (nm_turn_prm1_initialized == 0) {
        nm_turn_prm1_initialized = 1;
        LinkCollision_gammaByte(&nm_turn_prm1.r, 0xDC);
        LinkCollision_gammaByte(&nm_turn_prm1.g, 0xDC);
        LinkCollision_gammaByte(&nm_turn_prm1.b, 0xE6);
    }
    if (nm_turn_env_initialized == 0) {
        nm_turn_env_initialized = 1;
        LinkCollision_gammaByte(&nm_turn_env.r, 0xA0);
        LinkCollision_gammaByte(&nm_turn_env.g, 0xA0);
        LinkCollision_gammaByte(&nm_turn_env.b, 0xE6);
    }
    if (fm_turn_prm0_initialized == 0) {
        fm_turn_prm0_initialized = 1;
        LinkCollision_gammaByte(&fm_turn_prm0.r, 0x40);
        LinkCollision_gammaByte(&fm_turn_prm0.g, 0x40);
    }
    if (fm_turn_prm1_initialized == 0) {
        fm_turn_prm1_initialized = 1;
        LinkCollision_gammaByte(&fm_turn_prm1.r, 0xDC);
        LinkCollision_gammaByte(&fm_turn_prm1.g, 0xDC);
    }
    if (fm_turn_env_initialized == 0) {
        fm_turn_env_initialized = 1;
        LinkCollision_gammaByte(&fm_turn_env.r, 0x78);
        LinkCollision_gammaByte(&fm_turn_env.g, 0x78);
    }
}
static inline JPABaseEmitter* LinkCollision_cutTurnEmitter(daPy_lk_c* player, u32 id,
                                                           LinkCollisionCutCallback* callback, GXColor* prm,
                                                           GXColor* env) {
    s8 room = player->current.roomNo;
    dPa_control_c* particle = dComIfGp_getParticle();
    return dPa_control_set(particle, 1, id, &player->current.pos, &player->shape_angle, nullptr, 0xFF,
                           reinterpret_cast<dPa_levelEcallBack*>(callback), (s32)room, prm, env, nullptr);
}
static inline void LinkCollision_setCollider(dCcD_GObjInf* collider) {
    LinkCollisionApi::setCollider(&LinkCollisionPlay().mCollisionSpace, collider);
}
static inline void LinkCollision_setMass(dCcD_GObjInf* collider) {
    LinkCollisionApi::setMass(&LinkCollisionPlay().mMassManager, collider, 1);
}
static inline void LinkCollision_followCyl(daPy_lk_c* player, dCcD_Cyl* collider, cXyz* center) {
    collider->mCyl.SetC(center);
    collider->mCyl.SetH(player->mCyl.mCyl.mHeight);
    collider->mCyl.SetR(player->mCyl.mCyl.mRadius);
    LinkCollision_setCollider(collider);
}
template <class Animation> static inline void LinkCollision_anmSetFrame(Animation& animation, f32 frame) {
    LinkItemAnimationResource* resource = LinkCollision_animationResource(animation);
    animation.mFrameCtrl.mFrame = frame;
    resource->mFrame = frame;
    lk_setAnimationFrame(&animation.mpFrameValue, frame);
}
static inline void LinkCollision_yuchwPlay(daPy_lk_c* player) {
    auto& view = LinkCollision(player);
    LinkItemApi::playAnimation(&view.mYuchw00Bck);
    LinkItemApi::entryBck(&view.mYuchw00Bck, view.mpYuchw00Model->mModelData,
                          view.mYuchw00Bck.mFrameCtrl.mFrame);
    LinkItemApi::playAnimation(&view.mpYuchw00Btk);
    LinkItemAnimationResource* resource = gabi::at<LinkItemAnimationResource>(view.mpYuchw00Btk.mpAnm);
    f32 frame = view.mpYuchw00Btk.mFrameCtrl.mFrame;
    resource->mFrame = frame;
    lk_setAnimationFrame(&view.mpYuchw00Btk.mpFrameValue, frame);
}
static inline void LinkCollision_modelMatrix(J3DModel* model, Mtx34* source) {
    u32 words[12];
    for (u32 row = 0; row < 3; row++) {
        for (u32 column = 0; column < 4; column++) {
            words[row * 4 + column] = lk_fbits(source->m[row][column]);
        }
    }
    // HD: keep the guest-null destination arithmetic used by the inlined matrix assignment.
    Mtx34* destination = gabi::at<Mtx34>(gabi::ea(model) + 0xC8);
    for (u32 row = 0; row < 3; row++) {
        for (u32 column = 0; column < 4; column++) {
            LinkCollisionWord(destination->m[row][column], words[row * 4 + column]);
        }
    }
}
static inline void LinkCollision_setStartEnd(dCcD_Cps* collider, const cXyz* start, const cXyz* end) {
    LinkItem_setStartEnd(collider, start, end);
}

/* 024076CC */
// HD: grab-wear uses the height field; cut widths are constants and colors are gamma corrected.
// HD: the damage test includes extra procedures, and the roll trail turns by a constant angle.
void daPy_lk_c::setCollision() {
    Mtx34* root_jnt_mtx = lk_getAnmMtx(mpCLModel, LinkCollisionConstants::CL_JNT_LINK_ROOT_e);
    Mtx34* neck_jnt_mtx = lk_getAnmMtx(mpCLModel, LinkCollisionConstants::CL_JNT_NECK_JNT_e);
    Mtx34* ltoe_jnt_mtx = lk_getAnmMtx(mpCLModel, LinkCollisionConstants::CL_JNT_LTOE_JNT_e);
    Mtx34* rtoe_jnt_mtx = lk_getAnmMtx(mpCLModel, LinkCollisionConstants::CL_JNT_RTOE_JNT_e);
    Mtx34* stk = mDoMtx_stack_c::get();
    gabi::Local<cXyz> spD0;
    cXyz* c = spD0.get();
    {
        f32 x =
            gabi::fmuls_ppc(gabi::fadds_ppc((f32)root_jnt_mtx->m[0][3], (f32)neck_jnt_mtx->m[0][3]), 0.5f);
        c->x = x;
        f32 z =
            gabi::fmuls_ppc(gabi::fadds_ppc((f32)root_jnt_mtx->m[2][3], (f32)neck_jnt_mtx->m[2][3]), 0.5f);
        if (mCurProc == LinkCollisionConstants::daPyProc_JUMP_CUT_e /* daPyProc_JUMP_CUT_e */) {
            u32 i = (u32)(u16)shape_angle.y >> 3;
            c->x = gabi::fnmsubs(15.0f, (f32)l_collisionSinCos.m[i].sin, x);
            z = gabi::fnmsubs(15.0f, (f32)l_collisionSinCos.m[i].cos, z);
        }
        c->z = z;
    }
    cM3dGCyl* cyl = &mCyl.mCyl;
    cyl->SetR(fsel_l(field_0x2b0, 30.0f, 50.0f)); /* checkGrabWear() ? 50 : 30 */
    u32 mode = mModeFlg;
    if (mode & LinkCollisionConstants::ModeFlg_IN_SHIP) { /* ModeFlg_IN_SHIP: mCyl.OnCoSPrmBit(NoCrr) */
        mCyl.mObjCo.mSPrm = mCyl.mObjCo.mSPrm | 0x100;
    } else {
        mCyl.mObjCo.mSPrm = mCyl.mObjCo.mSPrm & ~0x100u;
    }
    s32 proc;
    if (mode & LinkCollisionConstants::ModeFlg_CRAWL) { /* ModeFlg_CRAWL */
        f32 s = (f32)l_collisionSinCos.m[((u32)(u16)shape_angle.x >> 3)].sin;
        f32 py = current.pos.y;
        if (!(s < 0.0f)) {
            c->y = gabi::fnmsubs(50.0f, s, py);
            cyl->SetH(gabi::fmadds(s, 35.0f, 50.0f));
        } else {
            c->y = gabi::fmadds(25.0f, s, py);
            cyl->SetH(gabi::fnmsubs(s, 65.0f, 50.0f));
        }
    } else if ((proc = mCurProc) == 0x1E /* daPyProc_FRONT_ROLL_e */) {
        c->y = current.pos.y;
        cyl->SetH(81.25f);
    } else if (proc == LinkCollisionConstants::daPyProc_BACK_JUMP_e /* daPyProc_BACK_JUMP_e */) {
        c->y = gabi::fadds_ppc(current.pos.y, 30.0f);
        cyl->SetH(81.25f);
    } else if (proc == LinkCollisionConstants::daPyProc_BT_JUMP_CUT_e ||
               (proc == LinkCollisionConstants::daPyProc_BT_JUMP_e &&
                !(speed.y > 0.0f))) { /* BT_JUMP_CUT / BT_JUMP */
        c->y = (f32)root_jnt_mtx->m[1][3];
        f32 d = gabi::fsubs_ppc((f32)root_jnt_mtx->m[1][3], current.pos.y);
        cyl->SetH(gabi::fsubs_ppc(125.0f, d));
        cyl->SetR(22.5f);
    } else {
        f32 l = (f32)ltoe_jnt_mtx->m[1][3];
        f32 o = proc == LinkCollisionConstants::daPyProc_ROPE_SWING_e /* daPyProc_ROPE_SWING_e */
                    ? (f32)root_jnt_mtx->m[1][3]
                    : (f32)rtoe_jnt_mtx->m[1][3];
        c->y = fsel_l(gabi::fsubs_ppc(l, o), o, l);
        f32 ny = (f32)lk_getAnmMtx(mpCLModel, LinkCollisionConstants::CL_JNT_NECK_JNT_e)->m[1][3];
        cyl->SetH(gabi::fadds_ppc(gabi::fsubs_ppc(ny, (f32)c->y), 40.1f));
    }
    cyl->SetC(c);
    proc = mCurProc;
    if ((mModeFlg & LinkCollisionConstants::ModeFlg_DAMAGE) ||
        proc == LinkCollisionConstants::daPyProc_GUARD_SLIP_e ||
        proc == LinkCollisionConstants::daPyProc_CROUCH_DEFENSE_SLIP_e ||
        proc == LinkCollisionConstants::daPyProc_VOMIT_WAIT_e || mDamageWaitTimer != 0) {
        mCyl.mObjTg.mSPrm = mCyl.mObjTg.mSPrm & ~1u; /* mCyl.OffTgSetBit() */
        mCyl.ClrTgHit();
    } else {
        mCyl.mObjTg.mSPrm = mCyl.mObjTg.mSPrm | 1; /* OnTgSetBit() */
    }
    LinkCollision_setCollider(&mCyl);
    LinkCollision_setMass(&mCyl);
    LinkCollision_followCyl(this, &mWindCyl, c);  /* mWindCyl */
    LinkCollision_followCyl(this, &mLightCyl, c); /* mLightCyl */
    LinkCollision_cutColorStatics();
    gabi::Local<cXyz> sp98; /* spB8 */
    u32 rflg = mResetFlg0;
    if (rflg & LinkCollisionConstants::daPyRFlg0_UNK2) {     /* daPyRFlg0_UNK2 */
        if (rflg & LinkCollisionConstants::daPyRFlg0_UNK1) { /* daPyRFlg0_UNK1 */
            setCutWaterSplash();
            proc = mCurProc;
            if (proc == LinkCollisionConstants::daPyProc_CUT_TURN_e ||
                proc == LinkCollisionConstants::daPyProc_CUT_ROLL_e) {
                LinkCollisionApi::StartCAt(&mAtCyl, &current.pos);
            } else {
                setSwordAtCollision();
            }
            mNoResetFlg0 =
                mNoResetFlg0 | LinkCollisionConstants::daPyFlg0_CUT_AT_FLG; /* daPyFlg0_CUT_AT_FLG */
            if (LinkCollision(this).mpCutfBrk.mpAnm != 0) {                 /* mpCutfBrk */
                f32 f;
                if (mModeFlg & LinkCollisionConstants::ModeFlg_PARRY) { /* ModeFlg_PARRY */
                    f = 2.0f;
                } else if (mNoResetFlg1 &
                           LinkCollisionConstants::daPyFlg1_SOUP_POWER_UP) { /* daPyFlg1_SOUP_POWER_UP */
                    f = 1.0f;
                } else {
                    f = 0.0f;
                }
                LinkCollision_anmSetFrame(LinkCollision(this).mpCutfBrk, f);
            }
            proc = mCurProc;
            if (proc != LinkCollisionConstants::daPyProc_CUT_F_e &&
                proc != LinkCollisionConstants::daPyProc_BT_VERTICAL_JUMP_CUT_e &&
                proc != LinkCollisionConstants::daPyProc_FAN_SWING_e) {
                f32 fr = mFrameCtrlUnder[0].mFrame;
                s32 n = (proc == LinkCollisionConstants::daPyProc_HAMMER_FRONT_SWING_e ||
                         proc == LinkCollisionConstants::daPyProc_WEAPON_FRONT_SWING_e)
                            ? gabi::ftoi(fr * 2.5f)
                            : gabi::ftoi(fr * 10.0f);
                J3DModel* cl = mpCLModel;
                Mtx34* mtx = cl != nullptr ? cl->getBaseTRMtx() : nullptr;
                f32 rate = (f32)LinkCollisionApi::getBlurTopRate(this);
                s32 col = getSwordBlurColor();
                LinkCollisionApi::initSwBlur(LinkCollisionBlur(mpSwBlur), mtx, n, col, rate);
                proc = mCurProc;
                if (proc == LinkCollisionConstants::daPyProc_CUT_TURN_e ||
                    proc == LinkCollisionConstants::daPyProc_CUT_ROLL_e) {
                    f32 w;
                    GXColor* prm0;
                    GXColor* prm1;
                    GXColor* env;
                    if (mEquipItem == LinkCollisionConstants::daPyItem_BOKO_e /* daPyItem_BOKO_e */) {
                        fopAc_ac_c* boko = mActorKeepEquip.mActor;
                        u32 prm = boko != 0 ? (u32)boko->mParameters : 0;
                        f32 k = prm == 0 ? 1.0f : prm == 1 ? 1.3f : prm == 3 ? 1.7f : prm == 4 ? 2.2f : 1.5f;
                        w = gabi::fmuls_ppc(k, 1.5f);
                        prm0 = &nm_turn_prm0;
                        prm1 = &nm_turn_prm1;
                        env = &nm_turn_env;
                    } else {
                        bool roll = proc == LinkCollisionConstants::daPyProc_CUT_ROLL_e ||
                                    (mNoResetFlg1 & LinkCollisionConstants::daPyFlg1_SOUP_POWER_UP);
                        if (lk_selectSword() == LinkCollisionConstants::dItemNo_SWORD_e ||
                            LinkCollisionPlay().mSwordEquipState == 2) { /* checkNormalSwordEquip() */
                            w = 1.0f;
                            prm0 = roll ? &n_roll_prm0 : &n_turn_prm0;
                        } else if (lk_selectSword() ==
                                   LinkCollisionConstants::dItemNo_MASTER_SWORD_3_e) { /* checkFinalMasterSwordEquip()
                                                                                        */
                            w = 1.5f;
                            prm0 = roll ? &fm_roll_prm0 : &fm_turn_prm0;
                        } else {
                            w = 1.5f;
                            prm0 = roll ? &nm_roll_prm0 : &nm_turn_prm0;
                        }
                        prm1 = prm0 + 1;
                        env = prm0 + 2;
                    }
                    JPABaseEmitter* e1 =
                        LinkCollision_cutTurnEmitter(this, LinkCollisionConstants::ID_AK_JN_ROUNDATTACK00,
                                                     &LinkCollision(this).m331C, prm0, env);
                    JPABaseEmitter* e2 =
                        LinkCollision_cutTurnEmitter(this, LinkCollisionConstants::ID_AK_JN_ROUNDATTACK01,
                                                     &LinkCollision(this).m332C, prm1, env);
                    JPABaseEmitter* e3 =
                        LinkCollision_cutTurnEmitter(this, LinkCollisionConstants::ID_AK_JN_ROUNDATTACK02,
                                                     &LinkCollision(this).m333C, prm0, env);
                    if (e1 != 0) { /* setGlobalParticleWidthScale */
                        LinkCollisionEmitterView(e1).mGlobalParticleWidthScale = w;
                    }
                    if (e2 != 0) {
                        LinkCollisionEmitterView(e2).mGlobalParticleWidthScale = w;
                    }
                    if (e3 != 0) {
                        LinkCollisionEmitterView(e3).mGlobalParticleWidthScale = w;
                    }
                }
            }
        } else if (mCurProc == LinkCollisionConstants::daPyProc_CUT_ROLL_e /* daPyProc_CUT_ROLL_e */) {
            cXyz* pos = LinkCollisionBlur(mpSwBlur)->field_0x304; /* mSwBlur.field_0x304 */
            gabi::Local<cXyz> d1;
            gabi::Local<cXyz> d2;
            gabi::Local<cXyz> spAC;
            gabi::Local<cXyz> spA0;
            gabi::Local<cXyz> t;
            cXyz_mi(&old.pos, d1.get(), &current.pos);
            cXyz_ml(d1.get(), d2.get(), 0.1f);
            spAC->copy(*d2);
            J3DModel* cl = mpCLModel;
            PSMTXCopy(cl != nullptr ? cl->getBaseTRMtx() : nullptr, stk);
            LinkCollisionApi::pushMtx();
            LinkCollisionApi::inverseMtx(stk, stk);
            PSMTXMultVec(stk, &m36C4, spA0.get());
            LinkCollisionApi::popMtx();
            for (s32 i = 0; i < 10; i++, pos++) {
                PSMTXMultVec(stk, spA0.get(), pos);
                cXyz_ml(spAC.get(), t.get(), (f32)i);
                PSVECAdd(pos, t.get(), pos);
                mDoMtx_YrotM(stk, 0x578);
            }
            cl = mpCLModel;
            PSMTXCopy(cl != nullptr ? cl->getBaseTRMtx() : nullptr, stk);
            mDoMtx_YrotM(stk, -0x2BC);
            PSMTXMultVec(stk, spA0.get(), sp98.get());
            if (LinkCollision(this).m331C.mAlpha == 0xFF) { /* m331C.getAlpha() */
                cXyz* arr = LinkCollisionBlur(mpSwBlur)->field_0x304;
                LinkCollision(this).m331C.mCount = 10;
                LinkCollision(this).m332C.mCount = 10;
                LinkCollision(this).m333C.mCount = 10;
                LinkCollision(this).m331C.mpPos = arr;
                LinkCollision(this).m332C.mpPos = arr;
                LinkCollision(this).m333C.mpPos = arr;
            }
            LinkCollisionApi::MoveCAt(&mAtCyl, &current.pos);
            seStartSwordCut(LinkCollisionConstants::JA_SE_LK_SUGOI_KAITEN /* JA_SE_LK_SUGOI_KAITEN */);
        } else {
            daPy_swBlur_c* blur = LinkCollisionBlur(mpSwBlur);
            proc = mCurProc;
            f32 fr = mFrameCtrlUnder[0].mFrame;
            s32 n = gabi::ftoi(fr * 10.0f);
            s32 r26 = (s32)blur->field_0x018; /* mSwBlur.field_0x018 */
            if (proc != LinkCollisionConstants::daPyProc_CUT_F_e &&
                proc != LinkCollisionConstants::daPyProc_BT_VERTICAL_JUMP_CUT_e &&
                proc != LinkCollisionConstants::daPyProc_FAN_SWING_e) {
                if (proc == LinkCollisionConstants::daPyProc_JUMP_CUT_LAND_e) {
                    n = 0x95;
                } else if (proc == LinkCollisionConstants::daPyProc_HAMMER_FRONT_SWING_e ||
                           proc == LinkCollisionConstants::daPyProc_WEAPON_FRONT_SWING_e) {
                    n = gabi::ftoi(fr * 2.5f);
                } else if (proc == LinkCollisionConstants::daPyProc_WEAPON_SIDE_SWING_e) {
                    n = (s32)((u32)n - 5);
                }
                J3DModel* cl = mpCLModel;
                blur->copySwBlur(cl != nullptr ? cl->getBaseTRMtx() : nullptr, n);
                proc = mCurProc;
            }
            if (proc == LinkCollisionConstants::daPyProc_CUT_TURN_e /* daPyProc_CUT_TURN_e */) {
                J3DModel* cl = mpCLModel;
                cXyz* pb = LinkCollisionBlur(mpSwBlur)->mpPosBuffer; /* mpPosBuffer */
                PSMTXMultVec(cl != nullptr ? cl->getBaseTRMtx() : nullptr,
                             gabi::at<cXyz>(gabi::ea(pb) + (u32)n * sizeof(cXyz) * 2 + sizeof(cXyz) * 3),
                             sp98.get());
                LinkCollisionApi::MoveCAt(&mAtCyl, &current.pos);
                u8 alpha = LinkCollision(this).m331C.mAlpha;
                daPy_swBlur_c* bl = LinkCollisionBlur(mpSwBlur);
                if (alpha == 0xFF) {
                    daPy_swBlur_c* bl2 = LinkCollisionBlur(mpSwBlur);
                    cXyz* arr = bl2->field_0x304;
                    s16 cnt = (s16)((s32)bl2->field_0x018 - r26);
                    LinkCollision(this).m331C.mpPos = arr;
                    LinkCollision(this).m332C.mpPos = arr;
                    LinkCollision(this).m331C.mCount = cnt;
                    LinkCollision(this).m332C.mCount = cnt;
                    LinkCollision(this).m333C.mCount = cnt;
                    LinkCollision(this).m333C.mpPos = arr;
                }
                bl->field_0x014 = 0; /* mSwBlur.field_0x014 */
            } else {
                setSwordAtCollision();
            }
        }
        proc = mCurProc;
        if (proc == LinkCollisionConstants::daPyProc_CUT_TURN_e ||
            proc == LinkCollisionConstants::daPyProc_CUT_ROLL_e) {
            LinkCollisionApi::bgmNowKaitengiri();
            u32 a0 = mAtCyl.mObjAt.mSPrm, a2 = mAtCps[1].mObjAt.mSPrm, a1 = mAtCps[0].mObjAt.mSPrm,
                a3 = mAtCps[2].mObjAt.mSPrm;
            mAtCyl.mObjAt.mSPrm = a0 | 1; /* mAtCyl.OnAtSetBit() */
            mAtCps[1].mObjAt.mSPrm = a2 & ~1u;
            mAtCps[0].mObjAt.mSPrm = a1 & ~1u;
            mAtCps[2].mObjAt.mSPrm = a3 & ~1u;
        } else {
            u32 rf = mResetFlg0;
            u32 a1 = mAtCps[0].mObjAt.mSPrm, a0 = mAtCyl.mObjAt.mSPrm, a2 = mAtCps[1].mObjAt.mSPrm;
            mAtCps[0].mObjAt.mSPrm = a1 | 1;
            mAtCyl.mObjAt.mSPrm = a0 & ~1u;
            u32 a3 = mAtCps[2].mObjAt.mSPrm;
            if (rf & 1) {
                mAtCps[1].mObjAt.mSPrm = a2 & ~1u;
                mAtCps[2].mObjAt.mSPrm = a3 & ~1u;
            } else {
                mAtCps[1].mObjAt.mSPrm = a2 | 1;
                mAtCps[2].mObjAt.mSPrm = a3 | 1;
            }
        }
        LinkCollision_setCollider(&mAtCyl);
        LinkCollision_setMass(&mAtCyl);
        for (u32 i = 0; i < 3; i++) {
            LinkCollision_setCollider(&mAtCps[i]);
            LinkCollision_setMass(&mAtCps[i]);
        }
    } else {
        daPy_swBlur_c* blur = LinkCollisionBlur(mpSwBlur);
        s32 v = (s32)blur->field_0x014;
        blur->field_0x014 = v < 10 ? 0 : v - 10;
        if (mCurProc == LinkCollisionConstants::daPyProc_BOTTLE_OPEN_e /* daPyProc_BOTTLE_OPEN_e */ &&
            mProcVar3 != 0) {
            mAtCps[0].mObjAt.mSPrm = mAtCps[0].mObjAt.mSPrm | 1;
            LinkCollision_setCollider(&mAtCps[0]);
            u32 a = mAtCps[1].mObjAt.mSPrm;
            if (mProcVar2 != 0) {
                mAtCps[1].mObjAt.mSPrm = a | 1;
                LinkCollision_setCollider(&mAtCps[1]);
            } else {
                mAtCps[1].mObjAt.mSPrm = a & ~1u;
            }
        } else {
            for (u32 i = 0; i < 3; i++) {
                dCcD_Cps* cps = &mAtCps[i];
                LinkCollisionApi::ResetAtHit(cps);
                cps->mObjAt.mSPrm = cps->mObjAt.mSPrm & ~1u;
            }
        }
        LinkCollisionApi::ResetAtHit(&mAtCyl);
        mAtCyl.mObjAt.mSPrm = mAtCyl.mObjAt.mSPrm & ~1u;
        mNoResetFlg0 = mNoResetFlg0 &
                       ~LinkCollisionConstants::daPyFlg0_CUT_AT_FLG; /* offNoResetFlg0(daPyFlg0_CUT_AT_FLG) */
        LinkCollisionApi::endCutTurn(&LinkCollision(this).m331C);
        LinkCollisionApi::endCutTurn(&LinkCollision(this).m332C);
        LinkCollisionApi::endCutTurn(&LinkCollision(this).m333C);
    }
    if (fanWindCrashEffectDraw()) {
        s16 mx = LinkCollision(this).mpYbafo00Btk.mFrameCtrl.mEnd;
        f32 fm = (f32)mx;
        f32 f = gabi::fadds_ppc(LinkCollision(this).mpYbafo00Btk.mFrameCtrl.mFrame, 1.0f);
        if (f > gabi::fsubs_ppc(fm, 0.5f)) {
            f = gabi::fsubs_ppc(fm, 0.001f);
        }
        LinkCollision_anmSetFrame(LinkCollision(this).mpYbafo00Btk, f);
    }
    /* the fan wind */
    bool wind = false;
    gabi::Local<cXyz> sp94;
    gabi::Local<cXyz> sp88;
    gabi::Local<cXyz> spC4;
    cXyz* p94 = sp94.get();
    cXyz* p88 = sp88.get();
    cXyz* pC4 = spC4.get();
    f32 radius = 0.0f;
    if (m3534 != 0) { /* m3534 */
        p94->copy(LinkCollisionCapsule(mFanWindCps).mEnd);
        f32 t = (f32)m3534 / 10.0f;
        u32 ia = (u32)(u16)m3536 >> 3;
        u32 ib = (u32)(u16)m3538 >> 3;
        f32 len = gabi::fmadds(t, 100.0f, 100.0f);
        f32 sa = (f32)l_collisionSinCos.m[ia].sin;
        f32 sb = (f32)l_collisionSinCos.m[ib].sin;
        f32 ca = (f32)l_collisionSinCos.m[ia].cos;
        f32 cb = (f32)l_collisionSinCos.m[ib].cos;
        pC4->x = gabi::fmuls_ppc(gabi::fmuls_ppc(len, sa), cb);
        pC4->y = -gabi::fmuls_ppc(len, sb);
        pC4->z = gabi::fmuls_ppc(gabi::fmuls_ppc(len, ca), cb);
        {
            gabi::Local<cXyz> s;
            cXyz_pl(p94, s.get(), pC4);
            p88->copy(*s);
        }
        LinkCollision(this).mArrowLinChk.Set(p94, p88, this);
        if (dComIfG_Bgsp()->LineCross(&LinkCollision(this).mArrowLinChk)) {
            LinkCollisionPlane* pla = reinterpret_cast<LinkCollisionPlane*>(
                dComIfG_Bgsp()->GetTriPla((u32)LinkCollision(this).mArrowLinChk.mBgIndex,
                                          (u32)LinkCollision(this).mArrowLinChk.mPolyIndex));
            f32 cx = (f32)LinkCollision(this).mArrowLinChk.mLin.mEnd.x; /* mArrowLinChk.GetCrossP() */
            f32 cy = (f32)LinkCollision(this).mArrowLinChk.mLin.mEnd.y;
            f32 cz = (f32)LinkCollision(this).mArrowLinChk.mLin.mEnd.z;
            p88->x = cx;
            p88->z = cz;
            bool crash = false;
            if (pla != 0) {
                p88->y = cy;
                crash = (f32)pla->mNormal.y < 0.5f; /* !cBgW_CheckBGround(ny) */
            }
            if (crash) {
                m3534 = 0;
                LinkItemApi::endMtxFollow(reinterpret_cast<LinkItemMtxCallback*>(&m33E8)); /* m33E8.end() */
                LinkCollision_anmSetFrame(
                    LinkCollision(this).mpYuchw00Brk,
                    gabi::fsubs_ppc((f32)LinkCollision(this).mpYuchw00Brk.mFrameCtrl.mEnd, 0.001f));
                LinkCollision_anmSetFrame(LinkCollision(this).mpYbafo00Btk, 0.0f);
                PSMTXTrans(mDoMtx_stack_c::get(), (f32)p88->x, (f32)p88->y, (f32)p88->z);
                gabi::Local<cXyz> nxz;
                f32 nx = (f32)pla->mNormal.x;
                f32 nz = (f32)pla->mNormal.z;
                nxz->y = 0.0f;
                nxz->x = nx;
                nxz->z = nz;
                f32 sq = LinkCollisionApi::squareMagnitude(nxz.get());
                f32 axz = (f32)LinkCollisionApi::sqrtf(sq);
                s16 a1 = cM_atan2s(-(f32)pla->mNormal.y, axz);
                s16 a2 = cM_atan2s(-(f32)pla->mNormal.x, -(f32)pla->mNormal.z);
                LinkCollisionApi::ZXYrotM(stk, (s32)a1, (s32)a2, 0);
                LinkCollision_modelMatrix(LinkCollision(this).mpYbafo00Model, stk);
                dPa_control_c* pa = dComIfGp_getParticle();
                JPABaseEmitter* e = dPa_control_set(
                    pa, 1, LinkCollisionConstants::ID_AK_JN_UCHIWAWIND01 /* ID_AK_JN_UCHIWAWIND01 */, p88, 0,
                    0, 0xFF, 0, -1, 0, 0, 0);
                if (e != 0) {
                    LinkCollisionApi::setEmitterMatrix(
                        stk, &LinkCollisionEmitterView(e).mGlobalRot,
                        &LinkCollisionEmitterView(e).mGlobalTrans); /* setGlobalRTMatrix */
                    u8 g = tevStr.mColorK0.g, r = tevStr.mColorK0.r,
                       bb = tevStr.mColorK0.b; /* tevStr.mColorK0 */
                    LinkCollisionEmitterView(e).mGlobalColorPrm.r = r;
                    LinkCollisionEmitterView(e).mGlobalColorPrm.g = g;
                    LinkCollisionEmitterView(e).mGlobalColorPrm.b = bb;
                }
            } else {
                p88->y = gabi::fadds_ppc(cy, 70.0f);
                m3538 = LinkItemApi::getGroundAngle(
                    this, static_cast<cBgS_PolyInfo*>(&LinkCollision(this).mArrowLinChk), (s32)m3536);
            }
        }
        wind = true;
        radius = gabi::fmadds(gabi::fsubs_ppc(1.0f, t), 230.0f, 70.0f);
        if (m3534 != 0) {
            m3534 = (s16)(m3534 - 1);
            PSMTXTrans(mDoMtx_stack_c::get(), (f32)p88->x, (f32)p88->y, (f32)p88->z);
            LinkCollisionApi::ZXYrotM(stk, (s32)m3538, (s32)m3536, 0);
            LinkCollision_modelMatrix(LinkCollision(this).mpYuchw00Model, stk);
            LinkCollision_yuchwPlay(this);
        }
    } else if (LinkCollisionApi::fanWindEffectDraw(this)) {
        LinkCollision_yuchwPlay(this);
        f32 mx = (f32)LinkCollision(this).mpYuchw00Brk.mFrameCtrl.mEnd;
        f32 f = gabi::fadds_ppc(LinkCollision(this).mpYuchw00Brk.mFrameCtrl.mFrame, 1.0f);
        if (f > gabi::fsubs_ppc(mx, 0.5f)) {
            f = gabi::fsubs_ppc(mx, 0.001f);
            LinkItemApi::endMtxFollow(reinterpret_cast<LinkItemMtxCallback*>(&m33E8)); /* m33E8.end() */
        }
        LinkCollision_anmSetFrame(LinkCollision(this).mpYuchw00Brk, f);
    } else if (LinkCollision(this).mpYbafo00Btk.mFrameCtrl.mFrame < 5.0f) {
        J3DModel* m = LinkCollision(this).mpYbafo00Model;
        Mtx34* mtx = m != nullptr ? m->getBaseTRMtx() : nullptr;
        f32 x = (f32)mtx->m[0][3];
        p94->x = x;
        f32 y = (f32)mtx->m[1][3];
        p94->y = y;
        f32 z = (f32)mtx->m[2][3];
        p94->z = z;
        LinkCollisionWord(pC4->x, LinkCollisionWord(cXyz_Zero->x)); /* cXyz::Zero */
        p88->y = gabi::fadds_ppc(y, 10.0f);
        LinkCollisionWord(pC4->y, LinkCollisionWord(cXyz_Zero->y));
        LinkCollisionWord(pC4->z, LinkCollisionWord(cXyz_Zero->z));
        p88->z = z;
        radius = 120.0f;
        p88->x = x;
        wind = true;
    }
    if (wind) {
        LinkCollision_setStartEnd(&mFanWindCps, sp94.get(), sp88.get()); /* mFanWindCps */
        u32 v0 = LinkCollisionWord(pC4->z), v1 = LinkCollisionWord(pC4->x), v2 = LinkCollisionWord(pC4->y);
        LinkCollisionWord(mFanWindCps.mGObjAt.mVec.x, v1); /* SetAtVec */
        LinkCollisionWord(mFanWindCps.mGObjAt.mVec.y, v2);
        LinkCollisionCapsule(mFanWindCps).mRadius = radius; /* SetR */
        LinkCollisionWord(mFanWindCps.mGObjAt.mVec.z, v0);
        LinkCollision_setCollider(&mFanWindCps);
        LinkCollision_setMass(&mFanWindCps);
    } else {
        LinkCollisionApi::ResetAtHit(&mFanWindCps);
    }
    if (LinkCollisionApi::fanWindEffectDraw(this)) {
        s8 rev = mReverb;
        mDoAud_seStart(LinkCollisionConstants::JA_SE_LK_FAN_WIND /* JA_SE_LK_FAN_WIND */,
                       &LinkCollisionCapsule(mFanWindCps).mEnd, 0, (s32)rev);
    }
    if (m353A != 0) { /* m353A */
        dCcD_Sph* sph = &mFanWindSph;
        if (mCurProc == LinkCollisionConstants::daPyProc_FAN_GLIDE_e /* daPyProc_FAN_GLIDE_e */) {
            Mtx34* m = lk_getAnmMtx(mpCLModel, LinkCollisionConstants::CL_JNT_LINK_ROOT_e);
            gabi::Local<cXyz> p;
            fcpy_l(gabi::ea(&p->x), gabi::ea(&m->m[0][3]));
            fcpy_l(gabi::ea(&p->y), gabi::ea(&m->m[1][3]));
            fcpy_l(gabi::ea(&p->z), gabi::ea(&m->m[2][3]));
            sph->mSph.SetC(p.get());
        } else {
            sph->mSph.SetC(&current.pos);
        }
        LinkCollision_setCollider(sph);
        LinkCollision_setMass(sph);
        m353A = (s16)(m353A - 1);
    } else {
        LinkCollisionApi::ResetAtHit(&mFanWindSph);
    }
    dCcD_Cps* lcps = &mFanLightCps;                                     /* mFanLightCps */
    if (mResetFlg0 & LinkCollisionConstants::daPyRFlg0_LIGHT_REFLECT) { /* daPyRFlg0_LIGHT_REFLECT */
        gabi::Local<cXyz> lv;
        J3DModel* sh = LinkItem(this).mpEquippedShieldModel;
        PSMTXMultVec(sh != nullptr ? sh->getBaseTRMtx() : nullptr, &l_ms_light_local_start, p94);
        sh = LinkItem(this).mpEquippedShieldModel;
        PSMTXMultVecSR(sh != nullptr ? sh->getBaseTRMtx() : nullptr, &l_ms_light_local_vec, lv.get());
        {
            gabi::Local<cXyz> s;
            cXyz_pl(p94, s.get(), lv.get());
            p88->copy(*s);
        }
        LinkCollision(this).mMirLightLinChk.Set(p94, p88, this);
        if (dComIfG_Bgsp()->LineCross(&LinkCollision(this).mMirLightLinChk)) {
            p88->copy(LinkCollision(this).mMirLightLinChk.mLin.mEnd);
            gabi::Local<cXyz> s;
            cXyz_mi(p88, s.get(), p94);
            lv->copy(*s);
        }
        LinkCollision_setStartEnd(lcps, sp94.get(), sp88.get());
        u32 f0 = lcps->mObjAt.mSPrm;
        u32 v0 = LinkCollisionWord(lv->x), v1 = LinkCollisionWord(lv->y);
        LinkCollisionWord(lcps->mGObjAt.mVec.x, v0);
        LinkCollisionWord(lcps->mGObjAt.mVec.y, v1);
        u32 v2 = LinkCollisionWord(lv->z);
        lcps->mObjAt.mSPrm = f0 | 1; /* OnAtSetBit */
        LinkCollisionWord(lcps->mGObjAt.mVec.z, v2);
        LinkCollision_setCollider(lcps);
        LinkCollision_setMass(lcps);
    } else {
        LinkCollisionApi::ResetAtHit(lcps);
        lcps->mObjAt.mSPrm = lcps->mObjAt.mSPrm & ~1u;
    }
}
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

// HD: heavy-state and frame counters precede setGetDemo; forest water expires by
// swapping the bottle and setting the event register. Warp/pitfall events shrink
// Link's models, and the HD event-order state has no GameCube counterpart.
/* 0240CDD0 */
BOOL daPy_lk_c::execute() {
    u32 b = gabi::ea(this);
    {
        s32 c = (s32)((u32)LinkUpdate(this).mFrameCounter - 1);
        s16 heavy = gabi::object_at<LinkUpdateHeavyState>(gabi::ea(this)).mHeavyStateCounter;
        LinkUpdate(this).mFrameCounter = c;
        if (c < 0) {
            LinkUpdate(this).mFrameCounter = 0;
        }
        bool clearCheck = true;
        if (heavy >= 5) {
            if (!LinkPoseApi::checkHeavyStateOn(this)) {
                mNoResetFlg0 = mNoResetFlg0 | daPyFlg0_HEAVY_STATE;
                clearCheck = false;
            } else if (gabi::object_at<LinkUpdateHeavyState>(gabi::ea(this)).mHeavyStateCounter >= 5) {
                clearCheck = false;
            }
        }
        if (clearCheck && LinkPoseApi::checkHeavyStateOn(this)) {
            mNoResetFlg0 = mNoResetFlg0 & ~daPyFlg0_HEAVY_STATE;
        }
        gabi::object_at<LinkUpdateHeavyState>(gabi::ea(this)).mHeavyStateCounter = 0;
        if (setGetDemo()) {
            return TRUE;
        }
    }
    if (LinkUpdateApi::checkBottle(&dComIfGs_info()->mSavedata.mPlayer.mPlayerItem, 0x59 /* dItemNo_FOREST_WATER_e */) &&
        LinkUpdateApi::getForestWaterTimer(&dComIfGs_info()->mSavedata.mPlayer.mItemRecord) == 0) {
        LinkUpdateApi::setBottleItemIn(&dComIfGs_info()->mSavedata.mPlayer.mPlayerItem, 0x59, 0x56 /* dItemNo_WATER_BOTTLE_e */);
        LinkUpdateApi::setEventReg(&dComIfGs_info()->mSavedata.mEvent, 0x9EFF, 0);
        LinkUpdateApi::seStartSystem(0x8B2);
    }
    /* checkNoControll() */
    if (gabi::ea(dComIfGp_getPlayer(0)) != b) {
        u32 st = actor_status;
        attention_info.flags = 0; /* attention_info.flags */
        actor_status = (st & ~0x3Fu) | 0x30;
        if (!(dComIfGp_getPlayerStatus0() & LinkPoseConstants::daPyStts0_SHIP_RIDE_e)) { /* daPyStts0_SHIP_RIDE_e */
            mStts.m_cc_move.z = 0.0f; /* mStts.ClrCcMove() */
            mStts.m_cc_move.x = 0.0f;
            mStts.m_weight = 0xFF; /* SetWeight */
            mStts.m_cc_move.y = 0.0f;
        } else {
            mStts.m_weight = 0x78;
        }
    } else {
        u32 st = actor_status;
        attention_info.flags = 0xFFFFFFFF;
        actor_status = (st & ~0x3Fu) | 0x21;
        mStts.m_weight = 0x78;
    }
    if ((mNoResetFlg0 & daPyFlg0_EQUIP_HEAVY_BOOTS) && dComIfGp_getSelectItem(0) != 0x29 && dComIfGp_getSelectItem(1) != 0x29 && dComIfGp_getSelectItem(2) != 0x29) {
        mNoResetFlg0 = mNoResetFlg0 & ~daPyFlg0_EQUIP_HEAVY_BOOTS; /* offNoResetFlg0(EQUIP_HEAVY_BOOTS) */
    } else if ((mNoResetFlg1 & daPyFlg1_EQUIP_DRAGON_SHIELD) && dComIfGp_getSelectItem(0) != 0x2A && dComIfGp_getSelectItem(1) != 0x2A && dComIfGp_getSelectItem(2) != 0x2A) {
        mNoResetFlg1 = mNoResetFlg1 & ~daPyFlg1_EQUIP_DRAGON_SHIELD; /* offNoResetFlg1(EQUIP_DRAGON_SHIELD) */
    }
    if (((dComIfG_inf_c*)dComIfGp_get())->play.mEvtCtrl.mMode != 0) { /* dComIfGp_event_runCheck() */
        mStaffIdx = dComIfGp_evmng_getMyStaffId("Link", this, 0);
        if (eventInfo.mCommand == dEvtCmd_INDOOR_e /* checkCommandDoor() */ && !(gabi::load<u16>(dComIfGp_ea() + 0x52B8) & 4) &&
            mEquipItem == LinkItemConstants::daPyItem_BOKO_e) {
            fopAc_ac_c* boko = mActorKeepEquip.mActor;
            if (boko != 0) { /* boko->moveStateInit(5.0f, 0.0f, shape_angle.y + 0x8000) */
                s16 a = shape_angle.y;
                boko->speed.y = 0.0f;
                boko->speedF = 5.0f;
                boko->current.angle.y = (s16)(a + 0x8000);
            }
            deleteEquipItem(0);
        }
    }
    /* l_debug_keep_pos / l_debug_shape_angle / l_debug_current_angle */
    current.pos.copy(l_debug_keep_pos);
    LinkUpdate_copyAngles(shape_angle, l_debug_shape_angle);
    LinkUpdate_copyAngles(current.angle, l_debug_current_angle);
    mCameraInfoIdx = (s32)dComIfGp_getPlayerCameraID(0); /* mCameraInfoIdx */
    m3748.copy(current.pos);
    if (mCurProc != LinkUpdateConstants::daPyProc_HOOKSHOT_FLY_e /* daPyProc_HOOKSHOT_FLY_e */ && !(dComIfGp_getPlayerStatus0() & LinkPoseConstants::daPyStts0_SHIP_RIDE_e) &&
        !(mModeFlg & (LinkUpdateConstants::ModeFlg_ROPE | LinkUpdateConstants::ModeFlg_CLIMB | LinkUpdateConstants::ModeFlg_LADDER)) && mCurProc != LinkUpdateConstants::daPyProc_DEMO_TOOL_e && mAcch.m_ground_h != -1000000000.0f &&
        !(mNoResetFlg0 & (daPyFlg0_UNK20000000 | daPyFlg0_UNK80000000)) && LinkUpdateApi::ChkPolySafe(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd)) &&
        LinkPoseApi::ChkMoveBG(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd))) {
        u32 bgs = gabi::ea(dComIfG_Bgsp());
        LinkUpdateApi::MoveBgCrrPos(gabi::at<dBgS>(bgs), static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd), (mAcch.m_flags >> 5) & 1, &current.pos, &current.angle, &shape_angle);
        bgs = gabi::ea(dComIfG_Bgsp());
        LinkUpdateApi::MoveBgCrrPos(gabi::at<dBgS>(bgs), static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd), (mAcch.m_flags >> 5) & 1, &old.pos, 0, 0);
    }
    gabi::Local<cXyz> oldPos;
    fcpy_l(gabi::ea(oldPos.get()) + 8, gabi::ea(&old.pos.z));
    fcpy_l(gabi::ea(oldPos.get()) + 4, gabi::ea(&old.pos.y));
    m34DE = shape_angle.y; /* m34DE */
    m34EA = m34DC; /* m34EA = m34DC */
    fcpy_l(gabi::ea(oldPos.get()) + 0, gabi::ea(&old.pos.x));
    m35B4 = mStickDistance; /* m35B4 = mStickDistance */
    dComIfGp_setDoStatus(0); /* dComIfGp_setDoStatus(dActStts_BLANK_e) */
    dComIfGp_setRStatus(0); /* dComIfGp_setRStatus */
    dComIfGp_setAStatus(0); /* dComIfGp_setAStatus */
    lk_playSetU8(0x5C2E, 0x4D);
    mFrontWallType = 0; /* mFrontWallType */
    u32 rf = mResetFlg0;
    if (rf & daPyRFlg0_POISON_CURSE) { /* daPyRFlg0_POISON_CURSE */
        setDamageCurseEmitter();
        rf = mResetFlg0;
    }
    if (rf & daPyRFlg0_NOT_ATTACKING) { /* daPyRFlg0_NOT_ATTACKING */
        *reinterpret_cast<be<u8>*>(&mCutType) = 0; /* mCutType */
    }
    mResetFlg0 = 0;
    LinkUpdateApi::calcByteTimer(&m_morf_frame); /* daPy_matAnm_c::decMorfFrame() */
    LinkUpdateApi::calcByteTimer(&l_maba_timer); /* decMabaTimer() */
    if (mCurProc == LinkUpdateConstants::daPyProc_BOTTLE_SWING_e /* daPyProc_BOTTLE_SWING_e */ && eventInfo.mCommand == dEvtCmd_INCATCH_e /* checkCommandCatch() */) {
        mDemo.mDemoType = 5; /* mDemo.setSpecialDemoType() */
    }
    setActorPointer();
    setAtnList();
    {
        u32 play = dComIfGp_ea();
        u32 z = LinkUpdateApi::convAttentionPId(gabi::at<void>(play + 0x5928), gabi::load<u32>(play + 0x5930)); /* dComIfGp_att_getZHint() */
        LinkUpdateApi::stopDoButtonQuake(this, z != 0 ? 0 : 1);
    }
    {
        u8 m = m34C2; /* m34C2 */
        if (m == 8) {
            if (!((f32)LinkPose(this).m_old_fdata->mOldFrameMorfCounter > 0.0f)) { /* m_old_fdata->getOldFrameMorfCounter() */
                m34C2 = 9;
            }
        } else if (m != 0xC) {
            m34C2 = 0;
        }
    }
    {
        fopAc_ac_c* grab = mActorKeepGrab.mActor; /* mActorKeepGrab.getActor() */
        fopAc_ac_c* equip = mActorKeepEquip.mActor;
        if (grab != 0 && !((u32)grab->actor_status & 0x2000) /* fopAcM_checkCarryNow */) {
            freeGrabItem();
        }
        if (mEquipItem == LinkItemConstants::daPyItem_BOKO_e && (equip == 0 || !((u32)equip->actor_status & 0x2000))) {
            deleteEquipItem(0);
        }
    }
    {
        u32 f1 = mNoResetFlg1;
        if (f1 & daPyFlg1_FREEZE_STATE) { /* checkFreezeState() */
            if (!(f1 & daPyFlg1_UNK40000)) {
                LinkUpdateApi::fadeOut(&l_freeze_fade_color, 0x10030000u /* (GHS: r4 keeps the constant base) */, -0.02f);
                mNoResetFlg1 = mNoResetFlg1 | daPyFlg1_UNK40000;
            }
        } else if (!(f1 & daPyFlg1_UNK40000000)) {
            animeUpdate();
        }
    }
    setDemoData();
    setStickData();
    if (mEquipItem == LinkUpdateConstants::dItemNo_GRAPPLING_HOOK_e /* dItemNo_GRAPPLING_HOOK_e */ && (mCurProc == LinkUpdateConstants::daPyProc_ROPE_SUBJECT_e || mCurProc == LinkUpdateConstants::daPyProc_ROPE_MOVE_e)) {
        fopAc_ac_c* eq = mActorKeepEquip.mActor;
        if (eq != 0 && (u32)eq->mParameters == 2) {
            procRopeReady_init();
        }
    }
    if ((dComIfGp_getPlayerStatus1() & 0x200000) && ((dComIfG_inf_c*)dComIfGp_get())->play.mEvtCtrl.mMode == 0) {
        u32 play = dComIfGp_ea();
        if (LinkUpdateApi::orderEvent(gabi::at<dEvt_control_c>(play + 0x51D0), 3, 1, 0, 0xFFFF, this, reinterpret_cast<u8*>(&LinkUpdate(this).mEventOrder), -1, 0xFF)) {
            LinkUpdate(this).mEventOrderAccepted = 1;
            LinkUpdate(this).mEventOrder = 1;
        }
    }
    {
        u32 play = dComIfGp_ea();
        gabi::store<u32>(play + 0x5CDC, gabi::load<u32>(play + 0x5CDC) & ~0x200000u);
        u32 on = (dComIfGp_getPlayerStatus1() & 0x100000) != 0 ? 1 : 0;
        if (on == 0 && LinkUpdate(this).mConfuseStatus != 0) {
            mNoResetFlg1 = mNoResetFlg1 & ~daPyFlg1_CONFUSE;
            mModeFlg = mModeFlg | LinkUpdateConstants::ModeFlg_00000100;
        }
        LinkUpdate(this).mConfuseStatus = (u8)on;
    }
    if (mDemo.mDemoType != 5) { /* !checkSpecialDemoMode() */
        if (!changeDemoProc() && !changeDeadProc() &&
            mCurProc != LinkUpdateConstants::daPyProc_DEMO_LAVA_DAMAGE_e && mCurProc != LinkUpdateConstants::daPyProc_DEMO_FREEZE_DAMAGE_e) {
            if (!changeAutoJumpProc()) {
                changeSwimProc();
            }
        }
        changeDamageProc();
        changeBoomerangCatchProc();
    }
    checkItemAction();
    if (mEquipItem == LinkItemConstants::daPyItem_SWORD_e && (m34C4 == 2 || m34C4 == 3)) { /* m34C4 */
        if (mAtCps[0].ChkAtHit() || mAtCps[1].ChkAtHit() ||
            mAtCps[2].ChkAtHit()) {
            m34C4 = (u8)(m34C4 + 2);
        }
    }
    {
        s16 t = m3522; /* m3522 */
        if (t > 0) {
            m3522 = (s16)(t - 1);
        } else {
            m34C4 = 0;
        }
    }
    setShieldGuard();
    if (checkAtHitEnemy(&mAtCps[0]) || checkAtHitEnemy(&mAtCps[1]) ||
        checkAtHitEnemy(&mAtCps[2])) {
        mNoResetFlg0 = mNoResetFlg0 | daPyFlg0_UNK10000000;
    }
    {
        s16 idx = mCurProcFunc.i; /* (this->*mCurProcFunc)() */
        if (idx != 0) {
            u32 obj = b + (s32)mCurProcFunc.d;
            if (idx < 0) {
                gabi::call_ptr(mCurProcFunc.f, obj);
            } else {
                u32 vt = gabi::load<u32>(obj + (s32)(*reinterpret_cast<be<s16>*>(reinterpret_cast<u8*>(&mCurProcFunc.f) + 2)));
                gabi::call_ptr(gabi::load<u32>(vt + (u32)(s32)idx * 8 + 4), obj);
            }
        }
    }
    if (mEquipItem == LinkItemConstants::daPyItem_BOKO_e && dComIfGp_getDoStatus() == 0) {
        dComIfGp_setDoStatus(0x2F);
    }
    if ((lk_selectSword() != 0xFF || dComIfGp_getMiniGameType() == 2) && dComIfGp_getAStatus() == 0 && /* checkSwordEquip() */
        !(dComIfGp_getPlayerStatus0() & (LinkUpdateConstants::daPyStts0_SUBJECT_e | LinkPoseConstants::daPyStts0_SHIP_RIDE_e | LinkUpdateConstants::daPyStts0_UNK1000000_e)) && !(dComIfGp_getPlayerStatus1() & (LinkUpdateConstants::daPyStts1_PICTO_BOX_AIM_e | LinkUpdateConstants::daPyStts1_UNK80_e))) {
        dComIfGp_setAStatus(0x35);
    }
    playTextureAnime();
    u32 sp14z, sp14x, sp14y;
    sp14z = lk_fbits(current.pos.z);
    sp14x = lk_fbits(current.pos.x);
    sp14y = lk_fbits(current.pos.y);
    posMove();
    mOldSpeed.copy(speed);
    u32 sp8z = lk_fbits(current.pos.z);
    u32 acf = mAcch.m_flags;
    u32 sp8y = lk_fbits(current.pos.y);
    u32 sp8x = lk_fbits(current.pos.x);
    mAcch.m_flags = acf & ~0x20u; /* mAcch.ClrGroundHit() */
    mAcch.CrrPos(dComIfG_Bgsp());
    setWaterY();
    autoGroundHit();
    checkLavaFace(oldPos.get(), 0);
    if (mModeFlg & LinkPoseConstants::ModeFlg_SWIM) { /* ModeFlg_SWIM */
        if ((mNoResetFlg0 & daPyFlg0_UNK100) && !checkSwimFallCheck()) {
            current.pos.y = mWaterY; /* mWaterY */
        }
        if (mCurProc != LinkUpdateConstants::daPyProc_DEMO_DEAD_e /* daPyProc_DEMO_DEAD_e */) {
            f32 h = gabi::fadds_ppc(mAcch.m_ground_h, 84.9f);
            if (h > current.pos.y && (mAcch.m_flags & 0x20)) {
                f32 sy = speed.y;
                current.pos.y = h;
                if (sy < 0.0f) {
                    speed.y = 0.0f;
                }
            }
        } else if (mAcch.m_flags & 0x20) {
            f32 sy = speed.y;
            current.pos.y = mAcch.m_ground_h;
            if (sy < 0.0f) {
                speed.y = 0.0f;
            }
        }
    } else if (mCurProc == LinkUpdateConstants::daPyProc_DEMO_TOOL_e /* daPyProc_DEMO_TOOL_e */) {
        gmem_stf32(gabi::ea(&current.pos.x), sp14x);
        gmem_stf32(gabi::ea(&current.pos.y), sp14y);
        gmem_stf32(gabi::ea(&current.pos.z), sp14z);
        if ((*reinterpret_cast<be<u32>*>(&mProcVar7)) != 0 && mAcch.m_ground_h != -1000000000.0f) { /* mProcVar7.m3574 */
            current.pos.y = mAcch.m_ground_h;
        }
    } else if (mCurProc == LinkUpdateConstants::daPyProc_HOOKSHOT_FLY_e || (dComIfGp_getPlayerStatus0() & LinkPoseConstants::daPyStts0_SHIP_RIDE_e) || (mModeFlg & (LinkUpdateConstants::ModeFlg_ROPE | LinkUpdateConstants::ModeFlg_LADDER | LinkUpdateConstants::ModeFlg_CAUGHT))) {
        gmem_stf32(gabi::ea(&current.pos.y), sp8y);
        gmem_stf32(gabi::ea(&current.pos.x), sp8x);
        gmem_stf32(gabi::ea(&current.pos.z), sp8z);
    } else {
        u32 p = (*reinterpret_cast<be<u32>*>(&mCurProc));
        if ((p >= LinkUpdateConstants::daPyProc_HANG_START_e && p <= LinkUpdateConstants::daPyProc_HANG_MOVE_e) || p == LinkUpdateConstants::daPyProc_DEMO_DOOR_OPEN_e) { /* door open / hang procs */
            gmem_stf32(gabi::ea(&current.pos.z), sp8z);
            gmem_stf32(gabi::ea(&current.pos.x), sp8x);
        }
    }
    s32 roomNo;
    if (mAcch.m_ground_h != -1000000000.0f) {
        roomNo = setRoomInfo();
        (*reinterpret_cast<be<u32>*>(&m357C)) = (*reinterpret_cast<be<u32>*>(&m3580)); /* m357C = m3580 */
        (*reinterpret_cast<be<u32>*>(&m3580)) = LinkUpdateApi::GetGroundCode(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd));
        if (mNoResetFlg0 & daPyFlg0_UNK80000000) {
            (*reinterpret_cast<be<u32>*>(&mCurrAttributeCode)) = 0; /* mCurrAttributeCode = dBgS_Attr_NORMAL_e */
        } else {
            (*reinterpret_cast<be<u32>*>(&mCurrAttributeCode)) = LinkUpdateApi::GetAttributeCode(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd));
        }
        LinkUpdateApi::RoomCheck(&mAcch.m_gnd);
        checkFallCode();
        u32 ac = mAcch.m_flags;
        u32 mode = mModeFlg;
        if (ac & 0x20) { /* mAcch.ChkGroundHit() */
            u32 r0;
            s32 attr = mCurrAttributeCode;
            if (!(mode & 0x20) && (attr == 6 || attr == 8)) { /* lava / void */
                checkLavaFace(0, attr);
                r0 = mNoResetFlg0;
            } else {
                r0 = mNoResetFlg0;
                if (r0 & daPyFlg0_UNK20000) {
                    u32 rr = mResetFlg0;
                    r0 = mNoResetFlg0;
                    mResetFlg0 = rr | daPyRFlg0_AUTO_JUMP_LAND; /* daPyRFlg0_AUTO_JUMP_LAND */
                }
                r0 &= ~daPyFlg0_UNK20000;
                if (r0 & daPyFlg0_UNK400000) {
                    u32 rr = mResetFlg0;
                    mNoResetFlg0 = r0;
                    mResetFlg0 = rr | daPyRFlg0_ROPE_JUMP_LAND; /* daPyRFlg0_ROPE_JUMP_LAND */
                }
                f32 py = current.pos.y;
                r0 &= ~daPyFlg0_UNK400000;
                mNoResetFlg0 = r0;
                if (py < 2000.0f) {
                    if (LinkUpdateApi::GetSpecialCode(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd)) != 1 &&
                        LinkUpdateApi::GetTriPla(dComIfG_Bgsp(), (u32)mAcch.m_gnd.mBgIndex,
                                                        (u32)mAcch.m_gnd.mPolyIndex)->y >= 0.5f) {
                        r0 = mNoResetFlg0 & ~daPyFlg0_DEKU_SP_RETURN_FLG; /* offNoResetFlg0(DEKU_SP_RETURN_FLG) */
                        mNoResetFlg0 = r0;
                    } else {
                        r0 = mNoResetFlg0;
                    }
                }
            }
            if (!(r0 & 0xA0000000)) {
                if (LinkPoseApi::ChkMoveBG(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd))) {
                    LinkUpdateApi::MoveBgTransPos(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd), 1, &m3724, 0, 0);
                }
                r0 = mNoResetFlg0;
            }
            if (r0 & 0x80000000) {
                mMtrlSndId = 0; /* mMtrlSndId */
                m34E2 = 0; /* m34E2 */
            } else {
                mMtrlSndId = LinkUpdateApi::GetMtrlSndId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd));
                s16 ga = LinkItemApi::getGroundAngle(this, &mAcch.m_gnd, (s32)shape_angle.y);
                m34E2 = ga;
            }
            if (mModeFlg & LinkUpdateConstants::ModeFlg_02000000) {
                setShapeAngleOnGround();
            }
        } else {
            if ((mode & LinkPoseConstants::ModeFlg_SWIM) && m3580 == 4) {
                startRestartRoom(5, 0xC9, -1.0f, 0);
            }
            s32 p = mCurProc;
            m34E2 = 0;
            if (p == LinkUpdateConstants::daPyProc_DEMO_TOOL_e) {
                mMtrlSndId = LinkUpdateApi::GetMtrlSndId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd));
            } else if (p == LinkUpdateConstants::daPyProc_SHIP_READY_e || p == LinkUpdateConstants::daPyProc_SHIP_GET_OFF_e) {
                mMtrlSndId = 9;
            } else {
                mMtrlSndId = 0;
            }
        }
        u32 se = mpSeAnmFrameCtrl; /* mpSeAnmFrameCtrl */
        if (se != 0 && !LinkUpdateApi::checkDemoSound()) {
            u32 nm = (u32)LinkUpdateSound(this).mpAnimationSoundResource->mpNameEntry->mNameOffset;
            u32 r9 = nm != 0 ? gabi::ea(&LinkUpdateSound(this).mpAnimationSoundResource->mpNameEntry->mNameOffset) + nm : 0;
            gabi::Local<lk_SafeString_l> s;
            s->mStr = r9;
            s->__vtbl = LK_SAFESTRING_VTBL;
            LinkUpdateApi::assureSoundName(s.get());
            s8 rev = mReverb;
            LinkUpdateSoundFrame* sc = gabi::at<LinkUpdateSoundFrame>(mpSeAnmFrameCtrl);
            LinkUpdateApi::setAnimSound(mJAIZelAnime, &current.pos, (f32)sc->mFrame,
                       (f32)sc->mRate, (u32)mMtrlSndId, (s32)rev);
        }
        u8 st = m34C3; /* m34C3 */
        if (!(mModeFlg & LinkUpdateConstants::PlayerFlyModeMask) /* !checkPlayerFly() */ && mCurProc != LinkUpdateConstants::daPyProc_DEMO_TOOL_e &&
            (st == 1 || st == 4 || (st >= 9 && st <= 10))) {
            setStepsOffset();
        }
        u8 grp = LinkUpdateApi::GetGrpSoundId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd));
        LinkUpdateApi::setLinkGroupInfo((u32)grp);
    } else {
        u32 c3580 = (*reinterpret_cast<be<u32>*>(&m3580));
        m34E2 = 0;
        (*reinterpret_cast<be<u32>*>(&mCurrAttributeCode)) = 0x1B; /* dBgS_Attr_UNK1B_e */
        (*reinterpret_cast<be<u32>*>(&m357C)) = c3580;
        mMtrlSndId = 0;
        (*reinterpret_cast<be<u32>*>(&m3580)) = 0xFFFFFFFF;
        (*reinterpret_cast<be<u32>*>(&mRestartPoint)) = 0xFF; /* mRestartPoint */
        roomNo = current.roomNo;
        checkFallCode();
    }
    {
        fopAc_ac_c* eq = mActorKeepEquip.mActor;
        if (eq != 0) {
            eq->tevStr.mRoomNo = tevStr.mRoomNo; /* tevStr.mRoomNo */
            u8 ov = tevStr.mEnvrIdxOverride;
            eq->current.roomNo = (s8)(u8)roomNo;
            eq->tevStr.mEnvrIdxOverride = ov; /* tevStr.mEnvrIdxOverride */
        }
    }
    setWorldMatrix();
    setWaistAngle();
    if ((u8)LinkPose(this).m_old_fdata->mOldFrameFlg != 0) { /* m_old_fdata->getOldFrameFlg() */
        footBgCheck();
    }
    if (mCurProc != LinkUpdateConstants::daPyProc_DAMAGE_e /* daPyProc_DAMAGE_e */) {
        cLib_addCalcAngleS(&mBodyAngle.z, 0, 4, 0xC00, 0x180);
        if (mCurProc != LinkUpdateConstants::daPyProc_FAN_GLIDE_e) {
            setMoveSlantAngle();
        }
        if (!(mModeFlg & LinkUpdateConstants::ModeFlg_SUBJECT)) { /* ModeFlg_SUBJECT */
            cLib_addCalcAngleS(&mBodyAngle.x, 0, 4, 0xC00, 0x180);
        }
        if (!(mModeFlg & LinkUpdateConstants::ModeFlg_40000000)) {
            cLib_addCalcAngleS(&mBodyAngle.y, 0, 4, 0xC00, 0x180);
        }
    }
    setNeckAngle();
    setHatAngle();
    /* HD: the warp / pitfall events shrink Link (global scale at 0x101CEEAC) and pull him to the event actor */
    f32 g;
    if (LinkPoseApi::startCheckOld(dComIfGp_getPEvtManager(), "DEFAULT_WARP") ||
        LinkPoseApi::startCheckOld(dComIfGp_getPEvtManager(), "DEFAULT_PITFALL")) {
        l_event_link_scale = (f32)((f64)l_event_link_scale * 0.95);
        u32 play = dComIfGp_ea();
        fopAc_ac_c* actor = LinkUpdateApi::convEventPId(gabi::at<dEvt_control_c>(play + 0x51D0), gabi::load<u32>(play + 0x5294));
        if (actor != nullptr) {
            gabi::Local<cXyz> t;
            fcpy_l(gabi::ea(&t->x), gabi::ea(&actor->current.pos.x));
            fcpy_l(gabi::ea(&t->y), gabi::ea(&actor->current.pos.y));
            fcpy_l(gabi::ea(&t->z), gabi::ea(&actor->current.pos.z));
            LinkUpdateApi::addCalcPosXZ(&current.pos, t.get(), 5.0f, 5.0f, 0.5f);
        }
        g = l_event_link_scale;
    } else {
        g = 1.0f;
        l_event_link_scale = 1.0f;
    }
    {
        // HD: the renderer stores base scale in y/x/z order.
        auto setModelScale = [g](J3DModel* model) {
            if (model != nullptr) {
                model->mBaseScale.y = g;
                model->mBaseScale.x = g;
                model->mBaseScale.z = g;
            }
        };
        setModelScale(mpCLModel);
        setModelScale(LinkItem(this).mpPodmsModel);
        setModelScale(mpHandsModel);
        setModelScale(mpKatsuraModel);
        setModelScale(LinkItem(this).mpEquipItemModel);
        setModelScale(mpSwgripmsModel);
        setModelScale(mpSwgripaModel);
        setModelScale(LinkItem(this).mpShmsModel);
        setModelScale(LinkItem(this).mpShaModel);
        setModelScale(LinkItem(this).mpPringModel);
    }
    setAnimeRatioHD();
    checkOriginalHatAnimation();
    mpCLModel->calc();
    jointCB1();
    LinkUpdateApi::decOldFrameMorfCounter(LinkPose(this).m_old_fdata);
    PSMTXMultVec(lk_getAnmMtx(mpCLModel, 15), &l_execute_head_offset, &mHeadTopPos); /* mHeadTopPos */
    checkRoofRestart();
    lk_jntTrans(gabi::ea(lk_getAnmMtx(mpCLModel, 8)), gabi::ea(&mLeftHandPos)); /* mLeftHandPos */
    lk_jntTrans(gabi::ea(lk_getAnmMtx(mpCLModel, 12)), gabi::ea(&mRightHandPos)); /* mRightHandPos */
    PSMTXMultVec(lk_getAnmMtx(mpCLModel, 0), &l_execute_boomerang_catch, &mBoomerangCatchPos);
    PSMTXMultVec(lk_getAnmMtx(mpCLModel, 8), &l_execute_hookshot_root, &mHookshotRootPos);
    lk_setBaseTRMtx(mpKatsuraModel.v, gabi::ea(lk_getAnmMtx(mpCLModel, 15)));
    mpKatsuraModel->calc();
    lk_setBaseTRMtx(mpYamuModel.v, gabi::ea(lk_getAnmMtx(mpCLModel, 15)));
    mpYamuModel->calc();
    setItemModel();
    {
        s32 p = mCurProc;
        if (p == LinkUpdateConstants::daPyProc_CUT_F_e || p == LinkUpdateConstants::daPyProc_BT_VERTICAL_JUMP_CUT_e) { /* CUT_F / BT_VERTICAL_JUMP_CUT */
            bool normal = lk_selectSword() == 0x38 || dComIfGp_getMiniGameType() == 2;
            u32 m = gabi::ea(lk_getAnmMtx(gabi::at<J3DModel>(LinkItem(this).mpEquipItemModel.v), normal ? 3 : 4));
            lk_setBaseTRMtx(LinkUpdateModels(this).mpSwordTipStabModel.v /* mpSwordTipStabModel */, m);
        }
    }
    PSMTXMultVec(lk_getAnmMtx(mpCLModel, 19 /* CL_JNT_CL_EYE_e */), &l_eye_offset, &eyePos);
    {
        u32 t0 = (*reinterpret_cast<be<u32>*>(&mSwordTopPos.x)), c0 = (*reinterpret_cast<be<u32>*>(&m36C4.x));
        (*reinterpret_cast<be<u32>*>(&m36D0.x)) = t0; /* m36D0 = mSwordTopPos */
        u32 t1 = (*reinterpret_cast<be<u32>*>(&mSwordTopPos.y));
        (*reinterpret_cast<be<u32>*>(&m36D0.y)) = t1;
        u32 t2 = (*reinterpret_cast<be<u32>*>(&mSwordTopPos.z));
        (*reinterpret_cast<be<u32>*>(&m36DC.x)) = c0; /* m36DC = m36C4 */
        (*reinterpret_cast<be<u32>*>(&m36D0.z)) = t2;
        u32 c2 = (*reinterpret_cast<be<u32>*>(&m36C4.z)), c1 = (*reinterpret_cast<be<u32>*>(&m36C4.y));
        (*reinterpret_cast<be<u32>*>(&m36DC.z)) = c2;
        (*reinterpret_cast<be<u32>*>(&m36DC.y)) = c1;
    }
    {
        u32 item = mEquipItem;
        if (item == LinkItemConstants::daPyItem_BOKO_e && mActorKeepEquip.mActor.v != 0) {
            fopAc_ac_c* boko = mActorKeepEquip.mActor;
            LinkUpdateApi::getBokoTopPos(boko, &mSwordTopPos);
            LinkUpdateApi::getBokoBlurRootPos(boko, &m36C4);
        } else if (item == LinkItemConstants::dItemNo_SKULL_HAMMER_e) {
            u32 m = LinkItem(this).mpEquipItemModel.v;
            PSMTXMultVec(gabi::at<Mtx34>(m != 0 ? m + 0xC8 : 0), &l_execute_hammer_top, &mSwordTopPos);
            m = LinkItem(this).mpEquipItemModel.v;
            PSMTXMultVec(gabi::at<Mtx34>(m != 0 ? m + 0xC8 : 0), &l_execute_hammer_root, &m36C4);
        } else if (item == LinkItemConstants::daPyItem_SWORD_e) {
            if (lk_selectSword() == 0x38 || dComIfGp_getMiniGameType() == 2) {
                PSMTXMultVec(lk_getAnmMtx(gabi::at<J3DModel>(LinkItem(this).mpEquipItemModel.v), 1), &l_execute_nsword_top, &mSwordTopPos);
                lk_jntTrans(gabi::ea(lk_getAnmMtx(gabi::at<J3DModel>(LinkItem(this).mpEquipItemModel.v), 2)), gabi::ea(&m36C4));
            } else {
                lk_jntTrans(gabi::ea(lk_getAnmMtx(gabi::at<J3DModel>(LinkItem(this).mpEquipItemModel.v), 1)), gabi::ea(&m36C4));
                PSMTXMultVec(lk_getAnmMtx(gabi::at<J3DModel>(LinkItem(this).mpEquipItemModel.v), 1), &l_execute_msword_top, &mSwordTopPos);
            }
        } else if (mCurProc == LinkUpdateConstants::daPyProc_FAN_SWING_e /* daPyProc_FAN_SWING_e */) {
            u32 m = LinkItem(this).mpEquipItemModel.v;
            lk_jntTrans(m != 0 ? m + 0xC8 : 0, gabi::ea(&m36C4));
            PSMTXMultVec(lk_getAnmMtx(gabi::at<J3DModel>(LinkItem(this).mpEquipItemModel.v), 6), &l_execute_fan_top, &mSwordTopPos);
        } else {
            lk_jntTrans(gabi::ea(lk_getAnmMtx(mpCLModel, 13)), gabi::ea(&m36C4));
            lk_jntTrans(gabi::ea(lk_getAnmMtx(mpCLModel, 13)), b + 0x3E4);
        }
    }
    checkLightHit();
    setFootEffect();
    setCollision();
    setAttentionPos();
    setGrabItemPos();
    setWaterRipple();
    setAuraEffect();
    setHammerWaterSplash();
    setLightSaver();
    {
        u16 cnd = 0;
        bool none = false;
        if (!(mNoResetFlg0 & (daPyFlg0_UNK20000000 | daPyFlg0_UNK80000000))) {
            if ((mAcch.m_flags & 0x20) && !(mModeFlg & LinkUpdateConstants::PlayerFlyModeMask)) {
                cnd = dEvtCnd_CANTALK_e | dEvtCnd_CANDOOR_e | dEvtCnd_CANGETITEM_e | dEvtCnd_UNK10_e | dEvtCnd_CANCATCH_e;
            } else if ((dComIfGp_getPlayerStatus0() & LinkPoseConstants::daPyStts0_SHIP_RIDE_e) ||
                       ((mModeFlg & LinkPoseConstants::ModeFlg_SWIM) && (mNoResetFlg0 & daPyFlg0_UNK100))) {
                cnd = dEvtCnd_CANTALK_e;
            } else {
                none = true;
            }
        } else {
            none = true;
        }
        if (none) {
            eventInfo.mCondition = eventInfo.mCondition | dEvtCnd_CANGETITEM_e; /* eventInfo.onCondition(dEvtCnd_CANGETITEM_e) */
        } else {
            eventInfo.mCondition = (u16)((eventInfo.mCondition | cnd) | dEvtCnd_CANGETITEM_e);
        }
    }
    {
        u32 e = reinterpret_cast<LinkUpdateSmokeCallback*>(mSmokeEcallBack)->mpEmitter.v; /* mSmokeEcallBack.getEmitter() */
        if (e != 0) {
            LinkUpdateApi::setEmitterMatrix(lk_getAnmMtx(mpCLModel, 35), gabi::at<void>(e + 0x1F0), gabi::at<cXyz>(e + 0x22C));
        }
    }
    setWaterDrop();
    {
        s16 t = l_damage_emitter_timer; /* daPy_dmEcallBack_c::getTimer() */
        if (t > 0) {
            t = (s16)(t - 1);
            u16 type = l_damage_emitter_type;
            l_damage_emitter_timer = t;
            if (type == 0) { /* checkFlame() */
                seStartOnlyReverb(0x2041 /* JA_SE_LK_BURNING */);
                u32 m = gabi::ea(lk_getAnmMtx(mpCLModel, 0));
                reinterpret_cast<LIGHT_INFLUENCE*>(mLightInfluence)->mPos.x = gabi::load<f32>(m + 0xC); /* mLightInfluence.mPos */
                reinterpret_cast<LIGHT_INFLUENCE*>(mLightInfluence)->mPos.y = gabi::load<f32>(m + 0x1C);
                reinterpret_cast<LIGHT_INFLUENCE*>(mLightInfluence)->mPower = 150.0f; /* mPower */
                reinterpret_cast<LIGHT_INFLUENCE*>(mLightInfluence)->mPos.z = gabi::load<f32>(m + 0x2C);
                t = l_damage_emitter_timer;
            } else if (type == 1) { /* checkCurse() */
                seStartOnlyReverb(0x2058 /* JA_SE_LK_CURSE_BURNING */);
                t = l_damage_emitter_timer;
            }
            if (t == 0) {
                endDamageEmitter();
            }
        }
    }
    mNoResetFlg1 = mNoResetFlg1 & ~(daPyFlg1_NPC_CALL_COMMAND | daPyFlg1_VINE_CATCH);
    if (((dComIfG_inf_c*)dComIfGp_get())->play.mEvtCtrl.mMode != 0 || gabi::ea(dComIfGp_getPlayer(0)) != b) {
        dComIfGp_setDoStatus(0);
        dComIfGp_setRStatus(0);
    } else {
        if (dComIfGp_getDoStatus() == 0x51) {
            dComIfGp_setDoStatus(0x12);
        }
        if (mResetFlg0 & daPyRFlg0_UNK10000000) {
            dComIfGp_setAStatus(0);
        }
    }
    mNoResetFlg1 = mNoResetFlg1 & ~(daPyFlg1_UNK4 | daPyFlg1_FORCE_VOMIT_JUMP | daPyFlg1_FORCE_VOMIT_JUMP_SHORT | daPyFlg1_UNK10000000);
    if (dComIfGp_getPlayerStatus0() & LinkPoseConstants::daPyStts0_SHIP_RIDE_e) {
        LinkUpdateApi::setLinkOnBoard(1);
        actor_status = actor_status | 0x4000;
    } else {
        LinkUpdateApi::setLinkOnBoard(2);
        actor_status = actor_status & ~0x4000u;
    }
    {
        dSv_info_c* save = dComIfGs_info();
        LinkUpdateApi::setLinkHp((u32)save->mSavedata.mPlayer.mPlayerStatusA.mLife,
                                (u32)save->mSavedata.mPlayer.mPlayerStatusA.mMaxLife);
    }
    mShieldFrontRangeYAngle = (s16)(shape_angle.y + mBodyAngle.y); /* mShieldFrontRangeYAngle */
    {
        u32 f1 = mNoResetFlg1;
        (*reinterpret_cast<be<u32>*>(&mFace)) = 0xAD; /* mFace = daPyFace_NONE */
        if (f1 & daPyFlg1_CONFUSE) { /* checkConfuse() */
            seStartMapInfo(0x107F /* JA_SE_LK_NOW_CURSE_PW */);
        }
    }
    mWhirlId = 0xFFFFFFFF; /* mWhirlId */
    l_debug_keep_pos.copy(current.pos);
    LinkUpdate_copyAngles(l_debug_shape_angle, shape_angle);
    LinkUpdate_copyAngles(l_debug_current_angle, current.angle);
    return TRUE;
}

// clang-format off
/* WWHD glue
ext 02526E70 u32 LinkAuraDemoApi::getActor(LinkAuraDemoObject*, u32)
ext 02444F48 void LinkAuraDemoApi::assureTermination(void*)
str 10035A98
str 10035038
str 10034F74

ext 023D8F6C BOOL daPy_lk_c::checkDemoSwordNoDraw(BOOL)
ext 023DCF08 BOOL daPy_lk_c::checkGrabBarrelSearch(int)

ext 023DCD30 void daPy_lk_c::setAtnList()
ext 023DBDD0 BOOL daPy_lk_c::setGetDemo()
ext 0240005C void daPy_lk_c::footBgCheck()
ext 023F22FC void daPy_lk_c::setDemoData()
ext 023DCF8C void daPy_lk_c::freeGrabItem()
ext 023F32E4 void daPy_lk_c::setStickData()
ext 023FE2F0 void daPy_lk_c::autoGroundHit()
ext 023FEA6C void daPy_lk_c::checkFallCode()
ext 023FE7C0 BOOL daPy_lk_c::checkLavaFace(cXyz*, int)
ext 023FFB08 void daPy_lk_c::setWaistAngle()
ext 023F7820 BOOL daPy_lk_c::changeDeadProc()
ext 023F695C BOOL daPy_lk_c::changeDemoProc()
ext 023FB8A0 void daPy_lk_c::setShieldGuard()
ext 023FF47C void daPy_lk_c::setStepsOffset()
ext 023FBC7C BOOL daPy_lk_c::checkAtHitEnemy(dCcD_GObjInf*)
ext 023FB230 void daPy_lk_c::checkItemAction()
ext 023DCC90 void daPy_lk_c::setActorPointer()
ext 023FA578 BOOL daPy_lk_c::changeDamageProc()
ext 023FBCEC void daPy_lk_c::playTextureAnime()
ext 023FD4E4 BOOL daPy_lk_c::startRestartRoom(u32, int, f32, int)
ext 02400990 void daPy_lk_c::setMoveSlantAngle()
ext 023F81A4 BOOL daPy_lk_c::changeAutoJumpProc()
ext 023FE918 BOOL daPy_lk_c::checkSwimFallCheck()
ext 023EB864 BOOL daPy_lk_c::procRopeReady_init()
ext 023DCB54 void daPy_lk_c::setDamageCurseEmitter()
ext 023FF020 void daPy_lk_c::setShapeAngleOnGround()
ext 023FB020 BOOL daPy_lk_c::changeBoomerangCatchProc()
ext 023FDA70 void daPy_lk_c::posMove()
ext 023D4768 void LinkUpdateApi::stopDoButtonQuake(daPy_lk_c*, s32)
ext 0200EF78 void LinkUpdateApi::addCalcPosXZ(cXyz*, const cXyz*, f32, f32, f32)
ext 028249B0 void LinkUpdateApi::setEmitterMatrix(const Mtx34*, void*, cXyz*)
  data l_attention_crawl_offset 10035A64
  data l_attention_subject_offset 10035A70
  data l_hammer_splash_particle_scale 1046CD3C
  data l_sword_d_scale 1046D1CC
  data l_sword_p_scale 1046D1D8
  data l_sword_d_scale_initialized 1046D1C4
  data l_sword_p_scale_initialized 1046D1C8
  data l_sword_g_prm0 10035AB0
  data l_sword_g_env0 10035AB4
  data l_sword_y_prm0 10035AB8
  data l_sword_y_env0 10035ABC
  data l_sword_g_prm1 10035AC0
  data l_sword_y_prm1 10035AC4
  data l_sword_y_env1 10035AC8
  data l_sword_s_prm1 10035ACC
  data l_sword_s_env1 10035AD0
  ext 025F1AA4 void LinkEffectApi::ZXYrotS(Mtx34*, s32, s32, s32)
  ext 024EFF3C void LinkEffectApi::setWallRadius(dBgS_AcchCir*, f32)
  ext 0200F378 s16 LinkEffectApi::addCalcAngleS(be<s16>*, s32, s32, s32, s32)
  ext 0201B31C void LinkEffectApi::normalize(cXyz*, cXyz*)
  ext 025D9F70 BOOL LinkEffectApi::getWaterY(cXyz*, be<f32>*)
  ext 023D457C JPABaseEmitter* LinkEffectApi::makeMtxEmitter(LinkItemMtxCallback*, u32, Mtx34*, cXyz*, u32)
  ext 028249B0 void LinkEffectApi::setGlobalRTMatrix(Mtx34*, Mtx34*, cXyz*)

ext 025B5C80 BOOL LinkUpdateApi::checkBottle(void*, u32)
ext 025B61C8 s32 LinkUpdateApi::getForestWaterTimer(void*)
ext 025B51DC void LinkUpdateApi::setBottleItemIn(void*, u32, u32)
ext 025B8AF4 void LinkUpdateApi::setEventReg(void*, u32, u32)
ext 025E1988 void LinkUpdateApi::seStartSystem(u32)
ext 0207A9A0 void LinkUpdateApi::calcByteTimer(be<u8>*)
ext 02008254 BOOL LinkUpdateApi::ChkPolySafe(dBgS*, cBgS_PolyInfo*)
ext 024EF968 void LinkUpdateApi::MoveBgCrrPos(dBgS*, cBgS_PolyInfo*, u32, cXyz*, csXyz*, csXyz*)
ext 024EFA38 void LinkUpdateApi::MoveBgTransPos(dBgS*, cBgS_PolyInfo*, u32, cXyz*, csXyz*, csXyz*)
ext 024EF0BC u32 LinkUpdateApi::GetGroundCode(dBgS*, cBgS_PolyInfo*)
ext 024EF0F4 u32 LinkUpdateApi::GetAttributeCode(dBgS*, cBgS_PolyInfo*)
ext 024EF09C s32 LinkUpdateApi::GetSpecialCode(dBgS*, cBgS_PolyInfo*)
ext 020084C8 cXyz* LinkUpdateApi::GetTriPla(dBgS*, u32, u32)
ext 025C35E8 void LinkUpdateApi::RoomCheck(dBgS_GndChk*)
ext 024EECAC u32 LinkUpdateApi::GetMtrlSndId(dBgS*, cBgS_PolyInfo*)
ext 024EEDB8 u8 LinkUpdateApi::GetGrpSoundId(dBgS*, cBgS_PolyInfo*)
ext 024EBAE8 u32 LinkUpdateApi::convAttentionPId(void*, u32)
ext 0253EC0C BOOL LinkUpdateApi::orderEvent(dEvt_control_c*, u32, u32, u32, u32, daPy_lk_c*, u8*, s32, u32)
ext 025E20F8 BOOL LinkUpdateApi::checkDemoSound()
ext 02444F48 void LinkUpdateApi::assureSoundName(void*)
ext 0201BF08 void LinkUpdateApi::setAnimSound(void*, cXyz*, f32, f32, u32, s32)
ext 025E1DF4 void LinkUpdateApi::setLinkGroupInfo(u32)
ext 025E1E44 void LinkUpdateApi::setLinkOnBoard(u32)
ext 025E1CE4 void LinkUpdateApi::setLinkHp(u32, u32)
ext 0253EE04 fopAc_ac_c* LinkUpdateApi::convEventPId(dEvt_control_c*, u32)
ext 025E3EC8 void LinkUpdateApi::decOldFrameMorfCounter(void*)
ext 020C4084 void LinkUpdateApi::getBokoTopPos(fopAc_ac_c*, cXyz*)
ext 020C4128 void LinkUpdateApi::getBokoBlurRootPos(fopAc_ac_c*, cXyz*)
ext 025F0658 void LinkUpdateApi::fadeOut(const GXColor*, u32, f32)
data l_debug_keep_pos 1046CD48
data l_debug_shape_angle 1046CD10
data l_debug_current_angle 1046CD08
data l_maba_timer 101CEF19
data l_freeze_fade_color 101CEAFC
data l_event_link_scale 101CEEAC
data l_damage_emitter_timer 101CEF14
data l_execute_nsword_top 10035B00
data l_execute_msword_top 10035B0C
data l_execute_boomerang_catch 10035B18
data l_execute_hookshot_root 10035B24
data l_execute_hammer_top 10035B30
data l_execute_hammer_root 10035B3C
data l_execute_fan_top 10035B48
data l_execute_head_offset 10035B54
str 10035AE8 in 0240CDD0
str 10035B60 in 0240CDD0
str 10035AF0 in 0240CDD0
data l_normal_ripple_scale 1046CD54
data l_small_ripple_scale 1046CD60
data l_demo07_ripple_pos 1046CD78
data l_waterfall_splash_trans 1046CD6C
data l_normal_ripple_initialized 1046D1B4
data l_small_ripple_initialized 1046D1B8
data l_demo07_ripple_initialized 1046D1C0
data l_waterfall_splash_initialized 1046D1BC
data l_aura_save 101F84DC
data l_water_particle_status 101EA4E4
data l_wave_offset 10034FFC
data l_swim_offset 10035008
data l_swim_side_offset 10035014
data l_waterfall_ripple_scale 101CEC68
data l_waterfall_ripple_callback 1047B2E4
str 101CEB48 in 0240B7F4
ext 025E8154 void LinkAuraApi::initBrk(LinkItemBrk*, J3DModelData*, u32, s32, s32, f32, s32, s32, s32, s32)
ext 025A9270 void LinkAuraApi::endRipple(LinkAuraRippleCallback*)
ext 028E8DAC void LinkAuraApi::subtractVector(const cXyz*, const cXyz*, cXyz*)
ext 025CB408 void LinkAuraApi::StartQuake(dVibration_c*, s32, s32, cXyz*)
ext 025CB610 void LinkAuraApi::StopQuake(dVibration_c*, s32)
ext 025A9270 void LinkUpdateApi::endRipple(void*)
ext 0240CA4C void LinkUpdateApi::endWaterDrop(void*)
ext 0240EBBC void LinkUpdateApi::removeSwimTail(void*)
ext 025A9D64 void LinkUpdateApi::endCutTurn(void*)
ext 023DCA08 void LinkUpdateApi::endDamageEmitter(daPy_lk_c*)
ext 02801D3C void LinkUpdateApi::stopAnimeSound(void*)
ext 0272B8B8 void LinkUpdateApi::swapTexture(u32, void*)
ext 027DF40C void LinkItemApi::updateAnimationValue(gptr<LinkItemAnimationValue>*)
str 10035A28
str 10035A20 in 02403C24
str 10035A3C in 02406480
str 10000160 in 02405CB4
data l_boko_jump_blur_rate 10192150
data l_boko_blur_rate 10192168
ext 023DFA48 void LinkItemApi::endFollow(LinkItemFollowCallback*)
ext 023D4538 void LinkItemApi::endMtxFollow(LinkItemMtxCallback*)
data l_light_normal_scale 1046D104
data l_light_boss_scale 1046D110
data l_light_normal_scale_initialized 1046D0FC
data l_light_boss_scale_initialized 1046D100
data l_run_splash_scale 1046D130
data l_run_grass_scale 1046D13C
data l_heavy_emit_smoke_scale 1046D148
data l_heavy_dyn_smoke_scale 1046D154
data l_heavy_pat_smoke_scale 1046D160
data l_run_splash_scale_initialized 1046D11C
data l_run_grass_scale_initialized 1046D120
data l_heavy_emit_smoke_scale_initialized 1046D124
data l_heavy_dyn_smoke_scale_initialized 1046D128
data l_heavy_pat_smoke_scale_initialized 1046D12C
data l_foot_grass_scale 101CEE94
data l_foot_smoke_scale 101CEEA0
data l_ms_light_local_vec 10034FE4
ext 02008974 f64 LinkItemApi::groundCross(dBgS*, void*)
ext 025DADA4 u32 LinkItemApi::createFootMark(u32, u32, cXyz*, cXyz*, u32)
ext 0252A038 BOOL LinkItemApi::checkLight(u32, cXyz*)
ext 0201794C void LinkItemApi::lineVsPositionCross(const cXyz*, const cXyz*, const cXyz*, cXyz*)
ext 023D457C void LinkItemApi::makeMtxEmitter(LinkItemMtxCallback*, u32, Mtx34*, cXyz*, u32)
ext 023E7D6C s16 LinkItemApi::getGroundAngle(daPy_lk_c*, cBgS_PolyInfo*, s32)
ext 0281E5A8 void LinkItemApi::setEmitterMaterial(JPABaseEmitter*, void*)
ext 025602F0 void LinkItemApi::getSeaColor(GXColor*, GXColor*)
ext 023DF9C0 void LinkItemApi::resetFootEffect(daPy_lk_c*)
ext 025A8770 s32 LinkItemApi::checkAttributeEffect(dPa_control_c*, s32)
ext 025F1954 void LinkItemApi::XYZrotS(Mtx34*, s32, s32, s32)
ext 023E771C void LinkItemApi::setShapeFanLeaf(daPy_lk_c*)
ext 02019510 s16 LinkItemApi::rad2s(f32)
ext 025E742C void LinkItemApi::playAnimation(void*)
ext 025E83FC void LinkItemApi::entryBrk(LinkItemBrk*, J3DModelData*, f32)
ext 025E86B8 void LinkItemApi::entryBck(LinkItemBck*, J3DModelData*, f32)
ext 025E1E14 void LinkItemApi::setSwordSoundType(s32, s32)
ext 025E1E2C void LinkItemApi::setShieldSoundType(s32, s32)
ext 025F1884 void LinkItemApi::YrotS(Mtx34*, s32)
ext 028E9108 void LinkItemApi::concatShieldMtx(Mtx34*, const Mtx34*, Mtx34*)
ext 024038A0 void LinkItemApi::setBootsModel(daPy_lk_c*, gptr<J3DModel>*)
ext 025E1E04 void LinkItemApi::setBootsSoundType(s32)
ext 02018808 void LinkItemApi::SetStartEnd(void*, const cXyz*, const cXyz*)
ext 028F4560 f32 LinkItemApi::powf(f32, f32)
ext 023D937C BOOL daPy_lk_c::checkChanceMode()
str 100359CC in 02402FFC
ext 024F8044 void* LinkPoseApi::getCameraBody()
ext 024FA018 fopAc_ac_c* LinkPoseApi::getForceLockOnActor(void*)
ext 024EE058 void* LinkPoseApi::getLockonList(void*, int)
ext 024EBA14 fopAc_ac_c* LinkPoseApi::getLockonActor(void*)
ext 023D69CC BOOL LinkPoseApi::checkShipNotNormalMode(daPy_lk_c*)
ext 023DBC24 BOOL LinkPoseApi::checkHeavyStateOn(daPy_lk_c*)
ext 024EC0A0 fopAc_ac_c* LinkPoseApi::hintActor(void*, u32)
ext 02400B14 BOOL LinkPoseApi::checkAttentionPosAngle(daPy_lk_c*, fopAc_ac_c*, be<u32>*)
ext 0252A068 BOOL LinkPoseApi::detectAttention(void*, cXyz*)
ext 02400BC4 fopAc_ac_c* LinkPoseApi::getDemoLookActor(daPy_lk_c*)
ext 0200ECD4 f32 LinkPoseApi::addCalc(be<f32>*, f32, f32, f32, f32)

str 101CECCC in 024022D0
str 101CECAC in 024022D0
data l_head_center_offset 101CEBB4
data l_eye_offset 101CEBA8
data l_neck_front 10034FC0
data l_neck_top 10034FCC
data l_tact_top 10034FF0
data l_baseZ 101FFBCC
data m_eye_move_flg 101CEF1A
data m_morf_frame 101CEF1B
ext 025445B8 BOOL LinkPoseApi::startCheckOld(void*, const char*)
ext 025162A4 BOOL LinkPoseApi::ChkTgHit(dCcD_Cyl*)
ext 0257E1B8 void LinkPoseApi::dKyw_get_AllWind_vec(const cXyz*, cXyz*, be<f32>*)
ext 028E8DE8 f32 LinkPoseApi::PSVECSquareDistance(const cXyz*, const cXyz*)
ext 025F181C void LinkPoseApi::mDoMtx_ZrotS(Mtx34*, s32)
ext 028E9108 void LinkPoseApi::PSMTXConcat(const Mtx34*, const Mtx34*, Mtx34*)
ext 024EEABC BOOL LinkPoseApi::ChkMoveBG(dBgS*, cBgS_PolyInfo*)
ext 02008438 fopAc_ac_c* LinkPoseApi::GetActorPointer(dBgS*, u32)
ext 024EF6E8 f64 LinkPoseApi::RoofChk(dBgS*, dBgS_RoofChk*)
ext 023FD4E4 BOOL LinkPoseApi::startRestartRoom(daPy_lk_c*, u32, int, f32, int)
ext 025D7C6C fopAc_ac_c* LinkPoseApi::fopAcM_getTalkEventPartner(daPy_lk_c*)
data l_damage_emitter_type 101CEF16
data l_waterDropPcallBack 1046D29C
ext 0281DE68 void LinkWaterApi::deleteAllParticle(JPABaseEmitter*, u32)
ext 02560CD0 BOOL LinkWaterApi::playerOverheadBgCheck()
data l_collisionSinCos 104A44F8
data l_ms_light_local_start 10034FD8
data n_roll_prm0 101CEB00
data n_roll_prm0_initialized 1046D16C
data n_roll_prm1 101CEB04
data n_roll_prm1_initialized 1046D170
data n_roll_env 101CEB08
data n_roll_env_initialized 1046D174
data nm_roll_prm0 101CEB0C
data nm_roll_prm0_initialized 1046D178
data nm_roll_prm1 101CEB10
data nm_roll_prm1_initialized 1046D17C
data nm_roll_env 101CEB14
data nm_roll_env_initialized 1046D180
data fm_roll_prm0 101CEB18
data fm_roll_prm0_initialized 1046D184
data fm_roll_prm1 101CEB1C
data fm_roll_prm1_initialized 1046D188
data fm_roll_env 101CEB20
data fm_roll_env_initialized 1046D18C
data n_turn_prm0 101CEB24
data n_turn_prm0_initialized 1046D190
data n_turn_prm1 101CEB28
data n_turn_prm1_initialized 1046D194
data n_turn_env 101CEB2C
data n_turn_env_initialized 1046D198
data nm_turn_prm0 101CEB30
data nm_turn_prm0_initialized 1046D19C
data nm_turn_prm1 101CEB34
data nm_turn_prm1_initialized 1046D1A0
data nm_turn_env 101CEB38
data nm_turn_env_initialized 1046D1A4
data fm_turn_prm0 101CEB3C
data fm_turn_prm0_initialized 1046D1A8
data fm_turn_prm1 101CEB40
data fm_turn_prm1_initialized 1046D1AC
data fm_turn_env 101CEB44
data fm_turn_env_initialized 1046D1B0
ext 02407178 f64 LinkCollisionApi::getBlurTopRate(daPy_lk_c*)
ext 02407070 void LinkCollisionApi::initSwBlur(daPy_swBlur_c*, Mtx34*, s32, s32, f32)
ext 0251655C void LinkCollisionApi::StartCAt(dCcD_Cyl*, cXyz*)
ext 02516618 void LinkCollisionApi::MoveCAt(dCcD_Cyl*, cXyz*)
ext 02516138 void LinkCollisionApi::ResetAtHit(dCcD_GObjInf*)
ext 025A9D64 void LinkCollisionApi::endCutTurn(LinkCollisionCutCallback*)
ext 0200E240 void LinkCollisionApi::setCollider(void*, dCcD_GObjInf*)
ext 02516C14 void LinkCollisionApi::setMass(void*, dCcD_GObjInf*, u32)
ext 025F23EC void LinkCollisionApi::pushMtx()
ext 025F2468 void LinkCollisionApi::popMtx()
ext 028E91EC void LinkCollisionApi::inverseMtx(Mtx34*, Mtx34*)
ext 028E8DD0 f32 LinkCollisionApi::squareMagnitude(const cXyz*)
ext 028F4384 f64 LinkCollisionApi::sqrtf(f32)
ext 028249B0 void LinkCollisionApi::setEmitterMatrix(Mtx34*, Mtx34*, cXyz*)
ext 02406D60 f64 LinkCollisionApi::colorGamma(u32)
ext 025E1CFC void LinkCollisionApi::bgmNowKaitengiri()
ext 025F1B48 void LinkCollisionApi::ZXYrotM(Mtx34*, s32, s32, s32)
ext 023D93DC BOOL LinkCollisionApi::fanWindEffectDraw(daPy_lk_c*)
*/
// clang-format on
