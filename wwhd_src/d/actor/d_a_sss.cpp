/**
 * d_a_sss.cpp (WWHD)
 * Enemy - Dexivine (Forbidden Woods, Wind Temple).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_sss.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_sss.h"

// HD: this translation unit's copies of inline vtables and common-header statics.
const gc_tu_data gc_tu = {
    .safeString = 0x1003D39C,
    .gndChk = {0x1003D3C4, 0x1003D3D4, 0x1003D3F4, 0x1003D3E4},
    .gndChkDt = {0x1003D3D4, 0x1003D3F4, 0x1003D3B4},
    .headerStatics = {0x1046DFAC, 0x101D0E20, 0},
};
constexpr u32 SSS_VTBL = 0x1003D404;
#define non_pos gabi::at<cXyz>(0x1046DFC8)

static const char* l_arcName = "Sss";
static GXColor l_color = {255, 255, 255, 255};
static GXColor l_cutColor = {255, 255, 255, 255};
static f32 size_d[10] = {10.0f, 10.0f, 9.5f, 9.0f, 8.5f, 8.0f, 7.5f, 7.0f, 6.5f, 6.5f};
static f32 g_d[10] = {50.0f, 50.0f, 35.0f, 25.0f, 15.0f, 9.0f, 6.0f, 6.0f, 6.0f, 6.0f};
static SssSphereSource tg_sph_src = {0x0,  0x0, 0x0, 0x0,  0x0,  0x0,  0x5,  0xff1dfebf, 0x3,
                                     0x75, 0x0, 0x0, 0x0,  0x0,  0x0,  0x0,  0x0,        0x0,
                                     0x0,  0x2, 0x0, 0.0f, 0.0f, 0.0f, 20.0f};
static SssSphereSource bm_sph_src = {0x0, 0x0, 0x0, 0x0,  0x0,  0x0,  0x5,  0x200040, 0x3,
                                     0x0, 0x0, 0x0, 0x0,  0x0,  0x0,  0x0,  0x0,      0x0,
                                     0x0, 0x2, 0x0, 0.0f, 0.0f, 0.0f, 80.0f};

/* 0248EC3C */
static BOOL daSss_Draw(sss_class* i_this) {
    if (i_this->mType != 0) {
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->eyePos, &i_this->tevStr);
        /* hand_draw */
        J3DModel* model = i_this->mpMorf->getModel();
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, model, &i_this->tevStr);
        i_this->mpMorf->updateDL();
        i_this->mVineLine.update(10, l_color, &i_this->tevStr);
        dComIfGd_set3DlineMat(reinterpret_cast<mDoExt_3DlineMat1_c*>(&i_this->mVineLine));
        if (i_this->mCutSegmentLen > 0.1f) {
            i_this->mCutLine.update(5, l_cutColor, &i_this->tevStr);
            dComIfGd_set3DlineMat(reinterpret_cast<mDoExt_3DlineMat1_c*>(&i_this->mCutLine));
        }
    }
    return TRUE;
}

/* 0248ED4C */
static void hand_open(sss_class* i_this) {
    void* anm = dComIfG_getObjectRes(l_arcName, dRes_INDEX_SSS_BCK_SSS_HIRAKU_e);
    i_this->mpMorf->setAnm(static_cast<J3DAnmTransform*>(anm), J3DFrameCtrl::EMode_NONE, 1.0f, 1.0f,
                           0.0f, -1.0f, nullptr);
}

// HD: retains REG12_S(1); other GameCube debug matrix adjustments are folded out.
/* 0248EDD0 */
static void hand_mtx_set(sss_class* i_this) {
    MtxTrans(i_this->mHandMtxPos.x, i_this->mHandMtxPos.y, i_this->mHandMtxPos.z, false);
    mDoMtx_XrotM(calc_mtx(), i_this->mHandAngleX);
    mDoMtx_YrotM(calc_mtx(), i_this->mHandAngleY);
    s16 reg = REG_S(12, 1);
    mDoMtx_XrotM(calc_mtx(), (s16)(reg - 0x4000));
    MtxScale(0.5f, 0.2f, 0.5f, true);
    MtxTrans(0.0f, -130.0f, 0.0f, true);
    J3DModel* model = i_this->mpMorf->getModel();
    mtx_copy(&model->mBaseTransformMtx, calc_mtx());
}

/* ---- hand_move's inlined helpers ---- */
// HD: GameCube debug tuning terms in the vine controls are folded into their defaults.
/* inlined into hand_move */
static void control1(sss_class* i_this) {
    i_this->mVine[0].field_0x00.copy(i_this->current.pos);
    mDoMtx_YrotS(calc_mtx(), i_this->current.angle.y);
    mDoMtx_XrotM(calc_mtx(), i_this->current.angle.x);
    gabi::Local<cXyz> sp3C, sp30, sp24, sp18, tmp;
    sp3C->y = 0.0f;
    sp3C->z = i_this->mReachPull;
    sp3C->x = 0.0f;
    MtxPosition(sp3C, sp24);
    sp3C->z = i_this->mSegmentLen;
    f32 f27 = i_this->mSwayAmp;
    for (s32 i = 1; i < 9; i++) {
        s32 t = i_this->mFrameCounter;
        sp30->x = cM_ssin(t * 1100 + i * 4000) * f27;
        sp30->y = g_d[i];
        sp30->z = cM_scos(t * 800 + i * 4000) * f27;
        MtxPosition(sp30, sp18);
        sss_s* cur = &i_this->mVine[i];
        sss_s* prev = &i_this->mVine[i - 1];
        f32 x = ((cur->field_0x00.x - prev->field_0x00.x) + sp24->x) + sp18->x;
        f32 y = ((cur->field_0x00.y - prev->field_0x00.y) + sp24->y) + sp18->y;
        f32 z = ((cur->field_0x00.z - prev->field_0x00.z) + sp24->z) + sp18->z;
        s16 xz = cM_atan2s(x, z);
        s16 ya = (s16)-cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z)));
        MtxPush();
        mDoMtx_YrotS(calc_mtx(), xz);
        mDoMtx_XrotM(calc_mtx(), ya);
        MtxPosition(sp3C, sp30);
        MtxPull();
        cXyz_pl(&prev->field_0x00, tmp, sp30);
        cur->field_0x00.copy(*tmp.get());
    }
}

/* inlined into hand_move */
static void control2(sss_class* i_this) {
    i_this->mVine[9].field_0x00.copy(i_this->mHandPos);
    gabi::Local<cXyz> sp34, sp28, tmp;
    f32 e0 = i_this->mSegmentLen;
    sp34->y = 0.0f;
    sp34->x = 0.0f;
    sp34->z = e0;
    for (s32 i = 8; i >= 1; i--) {
        sss_s* cur = &i_this->mVine[i];
        sss_s* next = &i_this->mVine[i + 1];
        f32 x = cur->field_0x00.x - next->field_0x00.x;
        f32 z = cur->field_0x00.z - next->field_0x00.z;
        f32 y = cur->field_0x00.y - next->field_0x00.y;
        s16 r27 = cM_atan2s(x, z);
        s16 r28 = (s16)-cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z)));
        mDoMtx_YrotS(calc_mtx(), r27);
        mDoMtx_XrotM(calc_mtx(), r28);
        f32 e = i_this->mSegmentLen;
        if (i == 8) {
            f32 zz = e - 10.0f;
            if (zz < 0.0f)
                zz = 0.0f;
            sp34->z = zz;
        } else {
            sp34->z = e;
        }
        MtxPosition(sp34, sp28);
        cXyz_pl(&next->field_0x00, tmp, sp28);
        cur->field_0x00.copy(*tmp.get());
    }
    i_this->mHandMtxPos.copy(i_this->mVine[9].field_0x00);
    cXyz_mi(&i_this->mVine[8].field_0x00, tmp, &i_this->mVine[9].field_0x00);
    f32 x = tmp->x, y = tmp->y, z = tmp->z;
    sp34->x = x;
    sp34->y = y;
    sp34->z = z;
    i_this->mHandAngleX = (s16)-cM_atan2s(y, z);
    i_this->mHandAngleY =
        cM_atan2s(sp34->x, std_sqrtf(gabi::fmadds(sp34->y, sp34->y, sp34->z * sp34->z)));
    hand_mtx_set(i_this);
}

// HD: the phase terms are subtracted; GameCube adds them.
/* inlined into hand_move */
static void cut_control1(sss_class* i_this) {
    i_this->mCutVine[0].field_0x00.copy(i_this->current.pos);
    mDoMtx_YrotS(calc_mtx(), i_this->current.angle.y);
    mDoMtx_XrotM(calc_mtx(), i_this->current.angle.x);
    gabi::Local<cXyz> sp50, sp44, sp38, tmp;
    f32 z0 = i_this->mCutSegmentLen;
    sp50->y = 0.0f; /* HD: x and y are zeroed */
    sp50->x = 0.0f;
    sp50->z = z0;
    for (s32 i = 1; i < 5; i++) {
        s32 t = i_this->mFrameCounter;
        sp44->x = cM_ssin(t * 4100 - i * 10000) * 50.0f;
        sp44->y = 50.0f;
        sp44->z = cM_scos(t * 4400 - i * 10000) * 50.0f;
        MtxPosition(sp44, sp38);
        sss_s* cur = &i_this->mCutVine[i];
        sss_s* prev = &i_this->mCutVine[i - 1];
        f32 x = (cur->field_0x00.x - prev->field_0x00.x) + sp38->x;
        f32 y = (cur->field_0x00.y - prev->field_0x00.y) + sp38->y;
        f32 z = (cur->field_0x00.z - prev->field_0x00.z) + sp38->z;
        s16 xz = cM_atan2s(x, z);
        s16 ya = (s16)-cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z)));
        MtxPush();
        mDoMtx_YrotS(calc_mtx(), xz);
        mDoMtx_XrotM(calc_mtx(), ya);
        MtxPosition(sp50, sp44);
        MtxPull();
        cXyz_pl(&prev->field_0x00, tmp, sp44);
        cur->field_0x00.copy(*tmp.get());
    }
    if (i_this->mpEmitter1 == nullptr) {
        if ((s8)i_this->mCutEffectOn == 0)
            return;
        JPABaseEmitter* e =
            dComIfGp_particle_set(sss::ID_IT_SN_TSURU_TAIEKI00, &i_this->mCutVine[4].field_0x00);
        i_this->mpEmitter1 = reinterpret_cast<SssEmitter*>(e);
        i_this->mTimers[SSS_EMITTER_TIMER_e] = 60;
        if (e == nullptr)
            return;
    }
    cXyz_mi(&i_this->mCutVine[4].field_0x00, tmp, &i_this->mCutVine[3].field_0x00);
    f32 x = tmp->x, y = tmp->y, z = tmp->z;
    sp50->x = x;
    sp50->y = y;
    sp50->z = z;
    s16 rot_y = cM_atan2s(x, z);
    s16 rot_x =
        (s16)-cM_atan2s(sp50->y, std_sqrtf(gabi::fmadds(sp50->x, sp50->x, sp50->z * sp50->z)));
    SssEmitter* e = i_this->mpEmitter1;
    f32 tx = i_this->mCutVine[4].field_0x00.x, ty = i_this->mCutVine[4].field_0x00.y,
        tz = i_this->mCutVine[4].field_0x00.z;
    e->setGlobalTranslation(tx, ty, tz);
    JPAGetXYZRotateMtx(rot_x, rot_y, 0, &i_this->mpEmitter1->mGlobalRotation);
    if (i_this->mTimers[SSS_EMITTER_TIMER_e] == 1) {
        i_this->mpEmitter1->becomeInvalidEmitter();
        i_this->mpEmitter1 = nullptr;
    }
}

// HD: the cut-vine phase terms are subtracted; GameCube adds them.
/* inlined into hand_move */
static void cut_control2(sss_class* i_this) {
    gabi::Local<cXyz> sp60, sp54, tmp;
    sp60->x = 0.0f;
    i_this->mVine[9].field_0x00.copy(i_this->mHandPos);
    f32 f27 = i_this->mSwayAmp;
    sp60->z = i_this->mSegmentLen;
    sp60->y = 0.0f;
    for (s32 i = 8; i >= 0; i--) {
        s32 t = i_this->mFrameCounter;
        sss_s* cur = &i_this->mVine[i];
        sss_s* next = &i_this->mVine[i + 1];
        f32 sA = cM_ssin(t * 2500 - i * 3000);
        f32 sB = cM_ssin(t * 2950 - i * 4000);
        f32 cC = cM_scos(t * 2800 - i * 3500);
        f32 x_val = gabi::fmadds(sA, f27, cur->field_0x00.x - next->field_0x00.x);
        f32 y_val = gabi::fmadds(sB, f27, cur->field_0x00.y - 10.0f);
        f32 f0 = i_this->mCutGroundY + 5.0f;
        if (y_val < f0)
            y_val = f0;
        f32 z_val = (cur->field_0x00.z - next->field_0x00.z) + cC * f27;
        f32 f28 = y_val - next->field_0x00.y;
        s16 xz = cM_atan2s(x_val, z_val);
        s16 ya = (s16)-cM_atan2s(f28, std_sqrtf(gabi::fmadds(x_val, x_val, z_val * z_val)));
        mDoMtx_YrotS(calc_mtx(), xz);
        mDoMtx_XrotM(calc_mtx(), ya);
        f32 e = i_this->mSegmentLen;
        if (i == 8) {
            f32 zz = e - 10.0f;
            if (zz < 0.0f)
                zz = 0.0f;
            sp60->z = zz;
        } else {
            sp60->z = e;
        }
        MtxPosition(sp60, sp54);
        cXyz_pl(&next->field_0x00, tmp, sp54);
        cur->field_0x00.copy(*tmp.get());
    }
    i_this->mHandMtxPos.copy(i_this->mVine[9].field_0x00);
    cXyz_mi(&i_this->mVine[8].field_0x00, tmp, &i_this->mVine[9].field_0x00);
    {
        f32 x = tmp->x, y = tmp->y, z = tmp->z;
        sp60->x = x;
        sp60->y = y;
        sp60->z = z;
        i_this->mHandAngleX = (s16)-cM_atan2s(y, z);
        i_this->mHandAngleY =
            cM_atan2s(sp60->x, std_sqrtf(gabi::fmadds(sp60->y, sp60->y, sp60->z * sp60->z)));
    }
    hand_mtx_set(i_this);
    if (i_this->mpEmitter2 == nullptr) {
        if ((s8)i_this->mCutEffectOn == 0)
            return;
        JPABaseEmitter* e =
            dComIfGp_particle_set(sss::ID_IT_SN_TSURU_TAIEKI00, &i_this->mVine[0].field_0x00);
        i_this->mpEmitter2 = reinterpret_cast<SssEmitter*>(e);
        if (e == nullptr)
            return;
    }
    cXyz_mi(&i_this->mVine[0].field_0x00, tmp, &i_this->mVine[1].field_0x00);
    f32 x = tmp->x, y = tmp->y, z = tmp->z;
    sp60->x = x;
    sp60->y = y;
    sp60->z = z;
    s16 rot_y = cM_atan2s(x, z);
    s16 rot_x =
        (s16)-cM_atan2s(sp60->y, std_sqrtf(gabi::fmadds(sp60->x, sp60->x, sp60->z * sp60->z)));
    SssEmitter* e = i_this->mpEmitter2;
    f32 tx = i_this->mVine[0].field_0x00.x, ty = i_this->mVine[0].field_0x00.y,
        tz = i_this->mVine[0].field_0x00.z;
    e->setGlobalTranslation(tx, ty, tz);
    JPAGetXYZRotateMtx(rot_x, rot_y, 0, &i_this->mpEmitter2->mGlobalRotation);
    if (i_this->mTimers[SSS_EMITTER_TIMER_e] == 1) {
        i_this->mpEmitter2->becomeInvalidEmitter();
        i_this->mpEmitter2 = nullptr;
        i_this->mCutEffectOn = 0;
    }
}

/* inlined into hand_move */
static void lines_copy(SssLineMaterial* l, sss_s* s, s32 n) {
    SssLine* line = lineMat_lines(l);
    be<u8>* sizes = line->mpSize;
    cXyz* segs = line->mpSegments;
    for (s32 i = 0; i < n; i++) {
        segs[i].copy(s[i].field_0x00);
        sizes[i] = (u8)gabi::ftoi(s[i].field_0x18);
    }
}

/* the inline dBgS_GndChk constructor / destructor (this TU's vtables) */
/* inlined into hand_move */
static void gndchk_dt(dBgS_GndChk* g) {
    g->cBgS_PolyInfo::__vtbl = gc_tu.gndChkDt[0];
    g->dBgS_PolyPassChk::__vtbl = gc_tu.gndChkDt[1];
    g->dBgS_GrpPassChk::__vtbl = gc_tu.gndChkDt[2];
    sss_bgCheck_dt(g, 0);
}

/* 0248F484 */
static void hand_move(sss_class* i_this) {
    fopAc_ac_c* a_this = i_this;
    fopAc_ac_c* a_player = dComIfGp_getPlayer(0);
    daPy_py_c* player = static_cast<daPy_py_c*>(a_player);
    u32 link = gabi::ea(dComIfGp_getLinkPlayer());
    f32 f30, f6, f28, f27, target, max_speed, f24, f5, f1;
    bool b1, b2;
    gabi::Local<cXyz> sp8C, sp80, sp74, tmp;

    gabi::Local<dBgS_GndChk> gnd_chk;
    dBgS_GndChk_ct(gnd_chk.get(), gc_tu.gndChk, false); /* MaskNormalGrp folded in */
    b1 = false;
    b2 = false;
    f30 = 0.1f;
    f6 = 8.0f;
    f28 = 0.1f;
    f27 = 0.0f;
    target = 30.0f;
    max_speed = 1.0f;
    f24 = 0.0f;
    f32 f4 = fopAcM_searchActorDistance(a_this, dComIfGp_getPlayer(0));
    f1 = 5.0f;
    if (i_this->mOpenRangePrm != 0xFF) {
        f5 = (f32)(u8)i_this->mOpenRangePrm * 10.0f;
    } else {
        f5 = 1000.0f;
    }
    mDoMtx_YrotS(calc_mtx(), i_this->current.angle.y);
    mDoMtx_XrotM(calc_mtx(), i_this->current.angle.x);
    s8 b = 0;
    switch ((u32)(s32)i_this->mAction) {
    case SSS_WAIT_e: {
        sp74->copy(i_this->current.pos);
        target = 0.0f;
        max_speed = 0.5f;
        f24 = -20.0f;
        if (!i_this->mTimers[SSS_ACTION_TIMER_e]) {
            if (i_this->mSwitchNo) {
                if (dComIfGs_isSwitch(i_this->mSwitchNo, i_this->current.roomNo)) {
                    i_this->mAction = SSS_OPEN_e;
                    i_this->mTimers[SSS_ACTION_TIMER_e] = 30;
                    hand_open(i_this);
                    b = 1;
                }
            } else {
                if (f4 < f5) {
                    i_this->mAction = SSS_OPEN_e;
                    i_this->mTimers[SSS_ACTION_TIMER_e] = 30;
                    hand_open(i_this);
                    b = 1;
                }
            }
        }
        if (std::fabs(i_this->mHandPos.y - i_this->current.pos.y) < 5.0f && b) {
            if (gabi::ea(&a_this->eyePos) != 0) {
                s8 room = a_this->current.roomNo;
                u32 id = i_this->mType == SSS_WATER_e ? JA_SE_OBJ_SVINE_OUT_WATER
                                                            : JA_SE_OBJ_SVINE_OUT;
                s32 rev = dComIfGp_getReverb(room);
                mDoAud_seStart(id, &a_this->eyePos, 0, rev);
            }
        }
        break;
    }
    case SSS_OPEN_e: {
        s32 t = i_this->mFrameCounter;
        sp8C->x = cM_ssin(t * 600) * 50.0f;
        sp8C->y = 250.0f;
        sp8C->z = cM_ssin(t * 700) * 50.0f;
        MtxPosition(sp8C, sp80);
        cXyz_pl(&i_this->current.pos, tmp, sp80);
        sp74->copy(*tmp.get());
        if (i_this->mTimers[SSS_ACTION_TIMER_e] == 0 && f4 < 300.0f) {
            i_this->mAction = SSS_REACH_e;
        }
        if (f4 > (f5 + 100.0f)) {
            i_this->mAction = SSS_WAIT_e;
        }
        break;
    }
    case SSS_REACH_e:
        sp74->x = a_player->current.pos.x;
        sp74->y = a_player->current.pos.y;
        sp74->z = a_player->current.pos.z;
        f6 = 15.0f;
        f28 = 0.5f;
        f27 = 10.0f;
        b1 = true;
        sp74->y = sp74->y + 70.0f;
        if (f4 > 450.0f) {
            i_this->speedF = 0.0f;
            i_this->mAction = SSS_OPEN_e;
        }
        cXyz_mi(sp74, tmp, &i_this->mHandPos);
        sp8C->copy(*tmp.get());
        if (std_sqrtf(PSVECSquareMag(sp8C)) < 20.0f && gabi::ea(a_player) == link) {
            i_this->mAction = SSS_CATCH_e;
            /* hand_close */
            void* anm = dComIfG_getObjectRes(l_arcName, dRes_INDEX_SSS_BCK_SSS_TOJIRU_e);
            i_this->mpMorf->setAnm(static_cast<J3DAnmTransform*>(anm), J3DFrameCtrl::EMode_NONE,
                                   1.0f, 1.0f, 0.0f, -1.0f, nullptr);
            sss_seStart(a_this, JA_SE_OBJ_SVINE_GRASP);
        } else {
            break;
        }
        // fall-through
    case SSS_CATCH_e: {
        player->onNoResetFlg1(daPy_py_c::daPyFlg1_VINE_CATCH);
        player->setFace(sss::daPyFace_TIYAYA);
        if (dComIfGs_getMagic()) { /* dComIfGs_getMagic() */
            SssGameMagicFlags* game = reinterpret_cast<SssGameMagicFlags*>(dComIfGp_get());
            game->mMagicDrainFlags |= 1;
            sss_seStart(a_this, JA_SE_OBJ_ATK_VINE_MP_SUCK);
            if ((i_this->mFrameCounter & 0x1F) == 0) {
                dComIfGp_setItemMagicCount(-1);
            }
        }
        if (f4 > 400.0f) {
            // HD: retains REG6_F(0), while the distance and maximum-power debug terms are folded
            // out.
            f32 p5 = (f4 - 400.0f) * (REG_F(6, 0) + 0.1f);
            if (p5 > 100.0f) {
                p5 = 100.0f;
            }
            SssPlayerVTable* vt = gabi::at<SssPlayerVTable>(a_player->__vtbl);
            s16 ang = fopAcM_searchActorAngleY(a_this, dComIfGp_getPlayer(0));
            sss_setOutPower(player, vt, p5, (s16)(ang + 0x8000), 0);
        }
        sp74->x = a_player->current.pos.x;
        sp74->y = a_player->current.pos.y;
        sp74->z = a_player->current.pos.z;
        f28 = 5.0f;
        i_this->mSwayAmp = 10.0f;
        f6 = 200.0f;
        f30 = 1.0f;
        sp74->y = sp74->y + 70.0f;
        if (f4 > 800.0f) {
            i_this->speedF = 30.0f;
            i_this->mAction = SSS_OPEN_e;
            hand_open(i_this);
        }
        break;
    }
    case SSS_CUT_e: {
        target = 25.0f;
        b2 = true;
        f1 = 50.0f;
        max_speed = 1.0f;
        i_this->mSwayAmp = 50.0f;
        PSVECAdd(&i_this->mHandPos, &i_this->speed, &i_this->mHandPos);
        i_this->speed.y = i_this->speed.y - 3.0f;
        i_this->mTimers[SSS_HIT_TIMER_e] = 5;
        cXyz* gpos = &gnd_chk->m_pos;
        f32 gx = i_this->mHandPos.x, gy = i_this->mHandPos.y, gz = i_this->mHandPos.z;
        gpos->x = gx;
        gpos->z = gz;
        gpos->y = gy + 200.0f;
        f32 g = cBgS_GroundCross(dComIfG_Bgsp(), gnd_chk.get());
        i_this->mCutGroundY = g;
        f32 g10 = g + 10.0f;
        // HD: preserve the negated comparison, including its unordered result.
        if (g == -1000000000.0f || !(i_this->mHandPos.y > g10)) {
            i_this->mHandPos.y = g10;
            i_this->mTimers[SSS_ACTION_TIMER_e] = 100;
            i_this->mAction = SSS_SINK_e;
        }
        break;
    }
    case SSS_SINK_e:
        b2 = true;
        f1 = 0.0f;
        i_this->mTimers[SSS_HIT_TIMER_e] = 10;
        if (i_this->mTimers[SSS_ACTION_TIMER_e] < 40) {
            target = 0.0f;
            max_speed = 1.0f;
            i_this->mHandPos.y = i_this->mHandPos.y - 2.0f;
            if (i_this->mTimers[SSS_ACTION_TIMER_e] == 0) {
                i_this->mHandPos.copy(i_this->current.pos);
                i_this->mSegmentLen = 0.0f;
                i_this->mCutSegmentLen = 0.0f;
                i_this->mAction = SSS_WAIT_e;
            }
        }
        break;
    }
    cLib_addCalc2(&i_this->mSegmentLen, target, 0.5f, max_speed);
    cLib_addCalc2(&i_this->mReachPull, f27, 1.0f, 0.2f);
    cLib_addCalc2(&i_this->mSwayAmp, f1, 1.0f, 1.5f);
    if (!b2) {
        cLib_addCalc2(&i_this->speedF, f6, 1.0f, f28);
        if (i_this->mPushDist > 1.0f && i_this->mAction != SSS_CATCH_e) {
            mDoMtx_YrotS(calc_mtx(), i_this->mPushAngleY);
            sp8C->x = 0.0f;
            sp8C->y = 100.0f;
            sp8C->z = i_this->mPushDist;
            MtxPosition(sp8C, sp80);
            gabi::Local<cXyz> t2;
            cXyz_pl(&i_this->current.pos, t2, sp80);
            sp74->copy(*t2.get());
            f30 = 0.1f;
            f32 s = i_this->mPushDist * 0.2f;
            if (s > 30.0f) {
                s = 30.0f;
            }
            i_this->speedF = s;
        }
        cLib_addCalc0(&i_this->mPushDist, 1.0f, 5.0f);
        cLib_addCalc2(&i_this->mHandPos.x, sp74->x, f30, i_this->speedF);
        cLib_addCalc2(&i_this->mHandPos.y, sp74->y, f30, i_this->speedF);
        cLib_addCalc2(&i_this->mHandPos.z, sp74->z, f30, i_this->speedF);
        cLib_addCalc2(&i_this->current.pos.y, i_this->home.pos.y + f24, 0.5f, 0.5f);
        if (b1 && i_this->current.angle.x == 0) {
            cLib_addCalcAngleS2(&i_this->current.angle.y,
                                fopAcM_searchActorAngleY(a_this, dComIfGp_getPlayer(0)), 16, 2048);
        }
        control1(i_this);
        control2(i_this);
    } else {
        cut_control1(i_this);
        cut_control2(i_this);
        lines_copy(&i_this->mCutLine, i_this->mCutVine, 5);
        cLib_addCalc0(&i_this->mCutSegmentLen, 1.0f, 1.0f);
    }
    /* control3 */
    for (s32 i = 0; i < 10; i++) {
        f32 temp = gabi::fmadds(cM_ssin(i_this->mFrameCounter * 500 + i * 100), 0.1f, 0.8f);
        i_this->mVine[i].field_0x18 = size_d[i] * temp;
    }
    i_this->mpMorf->play(nullptr, 0, 0);
    lines_copy(&i_this->mVineLine, i_this->mVine, 10);
    cXyz* r22 = lineMat_lines(&i_this->mVineLine)->mpSegments;
    i_this->eyePos.copy(r22[5]);
    i_this->attention_info.position.copy(i_this->eyePos);
    i_this->mStts.Move();
    i_this->mHandSph.SetC(b2 ? non_pos : &i_this->eyePos);
    u8 u1 = 0;
    dComIfG_Ccsp_Set(&i_this->mHandSph);
    for (s32 i = 0; i < 3; i++) {
        s32 r4 = ((i_this->mFrameCounter & 3) + i * 2) % 10;
        gabi::Local<cXyz> sp68;
        sp68->copy(r22[r4]);
        i_this->mVineSph[i].SetC(b2 ? non_pos : sp68.get());
        if (i_this->mAction == SSS_CATCH_e) {
            i_this->mVineSph[i].OffCoSPrmBit(1);
        } else {
            i_this->mVineSph[i].OnCoSPrmBit(1);
        }
        dComIfG_Ccsp_Set(&i_this->mVineSph[i]);
    }
    for (s32 i = 0; i < 3; i++) {
        if (i_this->mVineSph[i].ChkTgHit()) {
            u1 = (u8)(i + 1);
            break;
        }
    }
    if ((u1 || i_this->mHandSph.ChkTgHit() != 0) && i_this->mTimers[SSS_HIT_TIMER_e] == 0) {
        // HD: preserve the separate attack work area and its original stack lifetime.
        gabi::Local<SssAtInfo> at_info;
        at_info->pParticlePos = nullptr;
        i_this->mTimers[SSS_HIT_TIMER_e] = 20;
        if (!u1) {
            void* obj = i_this->mHandSph.GetTgHitObj();
            at_info->mpObj = obj;
            at_info->pParticlePos = i_this->mHandSph.GetTgHitPosP();
            sss_at_power_check(at_info.get());
            if (at_info->mResultingAttackType == 8) {
                i_this->mPushDist = 300.0f;
                i_this->mPushAngleY =
                    (s16)(fopAcM_searchActorAngleY(a_this, dComIfGp_getPlayer(0)) + 0x8000);
                gndchk_dt(gnd_chk.get());
                return;
            }
        } else {
            dCcD_GObjInf* objInf = &i_this->mVineSph[u1 - 1];
            void* obj = objInf->GetTgHitObj();
            at_info->mpObj = obj;
            at_info->pParticlePos = objInf->GetTgHitPosP();
        }
        sss_seStart(a_this, JA_SE_LK_LAST_HIT);
        sss_seStart(a_this, JA_SE_OBJ_SVINE_CRASH);
        i_this->mAction = SSS_CUT_e;
        i_this->speed.x = cM_rndFX(10.0f);
        i_this->speed.y = cM_rndF(10.0f) + 30.0f;
        i_this->speed.z = cM_rndFX(10.0f);
        gabi::Local<cXyz> scl;
        scl->set(0.3f, 0.3f, 0.3f);
        dComIfGp_particle_set(dPa_name::ID_AK_JN_SIBOUFLASH, &i_this->eyePos, nullptr, scl);
        i_this->mCutEffectOn = 1;
        for (s32 i = 0; i < 5; i++) {
            sss_s* r18 = &i_this->mVine[i];
            sss_s* r19 = &i_this->mCutVine[i];
            r19->field_0x00.copy(r18->field_0x00);
            r19->field_0x18 = (f32)r18->field_0x18;
            if (i == 4) {
                gabi::Local<cXyz> d;
                cXyz_mi(&r19[0].field_0x00, d, &r19[-1].field_0x00);
                i_this->mCutSegmentLen = std_sqrtf(PSVECSquareMag(d)) * 1.5f;
            }
        }
        hand_open(i_this);
    }
    gndchk_dt(gnd_chk.get());
}

/* 0248EEEC */
static BOOL daSss_Execute(sss_class* i_this) {
    i_this->mFrameCounter = i_this->mFrameCounter + 1;
    for (int i = 0; i < 2; i++) {
        if (i_this->mTimers[i] != 0) {
            i_this->mTimers[i] = i_this->mTimers[i] - 1;
        }
    }
    if (i_this->mTimers[SSS_HIT_TIMER_e] != 0) {
        i_this->mTimers[SSS_HIT_TIMER_e] = i_this->mTimers[SSS_HIT_TIMER_e] - 1;
    }
    if (i_this->mType) {
        hand_move(i_this);
    }
    return TRUE;
}

/* 0248EF68 */
static BOOL daSss_IsDelete(sss_class* i_this) { return TRUE; }

/* 0248EF70 */
static BOOL daSss_Delete(sss_class* i_this) {
    dComIfG_resDelete(&i_this->mPhs, l_arcName);
    SssEmitter* emitter = i_this->mpEmitter1;
    if (emitter) {
        emitter->becomeInvalidEmitter();
    }
    emitter = i_this->mpEmitter2;
    if (emitter) {
        emitter->becomeInvalidEmitter();
    }
    return TRUE;
}

/* 0248EFEC */
static BOOL useHeapInit(sss_class* i_this) {
    const char* arc = l_arcName;
    J3DModelData* modelData =
        static_cast<J3DModelData*>(dComIfG_getObjectRes(arc, dRes_INDEX_SSS_BMD_SSS_HAND_e));
    J3DAnmTransform* anm =
        static_cast<J3DAnmTransform*>(dComIfG_getObjectRes(arc, dRes_INDEX_SSS_BCK_SSS_HIRAKU_e));
    i_this->mpMorf =
        mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, J3DFrameCtrl::EMode_NONE,
                               1.0f, 0, -1, TRUE, nullptr, 0, 0x11020203);
    if (!i_this->mpMorf->getModel()) {
        return FALSE;
    }
    void* tex = dComIfG_getObjectRes(arc, dRes_INDEX_SSS_BTI_SSS_e);
    if (!i_this->mVineLine.init(1, 10, tex, TRUE)) {
        return FALSE;
    }
    tex = dComIfG_getObjectRes(arc, dRes_INDEX_SSS_BTI_SSS_e);
    if (!i_this->mCutLine.init(1, 5, tex, TRUE)) {
        return FALSE;
    }
    return TRUE;
}

/* 0248F140 */
static cPhs_State daSss_Create(fopAc_ac_c* i_actor) {
    sss_class* i_this = (sss_class*)i_actor;
    /* fopAcM_ct(i_actor, sss_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = SSS_VTBL;
            sss_line_ct(&i_this->mVineLine);
            sss_line_ct(&i_this->mCutLine);
            dCcD_Stts_ct(&i_this->mStts);
            sss_constructSpheres(i_this->mVineSph, 3);
            sss_sphere_ct(&i_this->mHandSph);
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&i_this->mPhs, l_arcName);
    if (state == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(
                i_actor, gabi::guest_fn(reinterpret_cast<const void*>(useHeapInit)), 0x3040)) {
            non_pos->x = 0.0f;
            non_pos->y = 30000.0f;
            non_pos->z = -20000.0f;
            u8 p = fopAcM_GetParam(i_this) & 0xFF;
            // GameCube folds the unspecified kind into the land variant in two steps.
            i_this->mType = (p != 0xFF && p == SSS_WATER_e) ? SSS_WATER_e : SSS_LAND_e;
            i_this->mOpenRangePrm = (u8)(fopAcM_GetParam(i_this) >> 8);
            u8 p2 = (u8)(fopAcM_GetParam(i_this) >> 16);
            i_this->mSwitchNo = p2 == 0xFF ? 0 : p2;
            i_this->health = 2;
            i_this->mFrameCounter = (s16)gabi::ftoi(cM_rndF(10000.0f));
            i_this->mStts.Init(0xFF, 0xFF, i_this);
            for (int i = 0; i < 3; i++) {
                i_this->mVineSph[i].Set(reinterpret_cast<const dCcD_SrcSph*>(&tg_sph_src));
                i_this->mVineSph[i].SetStts(&i_this->mStts);
            }
            i_this->mHandSph.Set(reinterpret_cast<const dCcD_SrcSph*>(&bm_sph_src));
            i_this->mHandSph.SetStts(&i_this->mStts);
            i_this->mHandPos.x = i_this->current.pos.x;
            f32 y = i_this->current.pos.y;
            i_this->mHandPos.y = y;
            i_this->mHandPos.z = i_this->current.pos.z;
            if (!i_this->mSwitchNo) {
                i_this->mHandPos.y = y + 230.0f;
            }
            daSss_Execute(i_this);
        } else {
            state = cPhs_ERROR_e;
        }
    }
    return state;
}

// HD: compiler-generated and template code of this translation unit.

/* 0248F3DC */
static void __sinit_d_a_sss_cpp() {
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
}

/* 0248F470 */
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02491160 */
static void sss_class_dt(sss_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        sss_sphere_dt(&i_this->mHandSph, 2);
        sss_destroySpheres(i_this->mVineSph, 3);
        dCcD_Stts_dt(&i_this->mStts, 2);
        sss_line_dt(&i_this->mCutLine, 2);
        sss_line_dt(&i_this->mVineLine, 2);
        sss_actor_dt(i_this, 0);
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 02491204 */
static void empty_virtual(void* p) {}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
str 1003D424 in 0248ED4C
str 1003D394 in 0248F484
str 1003D434 in 0248EF70
str 1003D398 in 0248EFEC
str 1003D448 in 0248F140
data l_color 101D0D28
data l_cutColor 101D0D2C
data size_d 101D0D30
data g_d 101D0D78
data tg_sph_src 101D0DA0
data bm_sph_src 101D0DE0
*/
