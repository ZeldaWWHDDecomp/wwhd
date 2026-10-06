/**
 * d_a_ep.cpp (WWHD)
 * Object - Torches
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_ep.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 *
 * HD changes: no alpha-model glow in Draw (ep_draw is gone), a flicker factor (+0x600) on the
 * point light, a darker light in some stage types, fire/heat-haze particles suppressed on the
 * "sea" stage outside a few areas, the light-info block copied to two HD-only slots.
 */
#include "bindings.h"

#define SAFESTRING_VTBL 0x1000E25C
#define EP_VTBL 0x1000E3D4 /* HD: ep_class vtable */
#define FILE_NAME STR(0x1000E50C) /* "d_a_ep.cpp" */
static const dBgS_ObjAcch_vt EP_OBJACCH_VT = {0x1000E364, 0x1000E384, 0x1000E374};
static const dBgS_LinChk_vt EP_LINCHK_VT = {0x1000E394, 0x1000E3A4, 0x1000E3C4, 0x1000E3B4};
static const dBgS_GndChk_vt EP_GNDCHK_VT = {0x1000E324, 0x1000E334, 0x1000E354, 0x1000E344};

enum { /* HD archive order */
    dRes_INDEX_EP_BDL_EP_GA_e = 3,
    dRes_INDEX_EP_BDL_OBM_SHOKUDAI1_e = 4,
    dRes_INDEX_EP_BDL_VKTSD_e = 5,
};
enum {
    JA_SE_OBJ_TORCH_BURNING = 0x6103,
    JA_SE_OBJ_TORCH_IGNITION = 0x6902,
    JA_SE_OBJ_TORCH_OFF = 0x6904,
};
enum { fpcNm_BDK_e = 0xEE };

struct ep_ga_s {
    /* 0x00 */ gptr<J3DModel> mpModel;
    /* 0x04 */ cXyz mPos;
    /* 0x10 */ cXyz m10;
    /* 0x1C */ cXyz m1C;
    /* 0x28 */ be<s16> rotX;
    /* 0x2A */ be<s16> rotY;
    /* 0x2C */ u8 m2C[4];
    /* 0x30 */ be<f32> scaleXZ;
    /* 0x34 */ be<f32> scaleY;
    /* 0x38 */ be<s16> m38;
    /* 0x3A */ be<u8> mbEnabled;
    /* 0x3B */ be<u8> m3B;
};
WWHD_SIZE(ep_ga_s, 0x3C);

struct ep_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ Mtx34 mAlphaModelMtx;
    /* 0x3E4 */ gptr<J3DModel> mpModel;
    /* 0x3E8 */ dBgS_AcchCir mAcchCir;
    /* 0x428 */ dBgS_ObjAcch mAcch;
    /* 0x5EC */ be<s32> m4D0;
    /* 0x5F0 */ u8 m4D4[8];
    /* 0x5F8 */ be<f32> mLightPower;
    /* 0x5FC */ be<f32> m4E0;
    /* 0x600 */ be<f32> mFlicker; /* HD */
    /* 0x604 */ be<f32> mAlphaModelScale;
    /* 0x608 */ be<f32> mAlphaModelScaleTarget;
    /* 0x60C */ be<f32> mAlphaModelAlpha;
    /* 0x610 */ be<f32> mAlphaModelAlphaTarget;
    /* 0x614 */ be<s16> mTimers[4];
    /* 0x61C */ u8 m4FC;
    /* 0x61D */ be<u8> mbNoEp;
    /* 0x61E */ be<u8> mbShouldDrawModel;
    /* 0x61F */ u8 m4FF;
    /* 0x620 */ be<s16> mAlphaModelRotX;
    /* 0x622 */ be<s16> mAlphaModelRotY;
    /* 0x624 */ be<u8> mType;
    /* 0x625 */ be<u8> m505;
    /* 0x626 */ be<u8> mOnSwitchNo;
    /* 0x627 */ be<u8> m507;
    /* 0x628 */ be<u8> m508;
    /* 0x629 */ u8 m509[3];
    /* 0x62C */ gptr<JPABaseEmitter> mpEmitter;
    /* 0x630 */ LIGHT_INFLUENCE mLight;
    /* 0x654 */ cXyz mPosTop;
    /* 0x660 */ dCcD_Stts mStts;
    /* 0x69C */ dCcD_Cyl mCyl;
    /* 0x7CC */ dCcD_Sph mSph1;
    /* 0x8F8 */ be<s16> m7D4;
    /* 0x8FA */ u8 m7D6[2];
    /* 0x8FC */ be<f32> m7D8;
    /* 0x900 */ be<s16> m7DC;
    /* 0x902 */ be<s8> mbHasObm;
    /* 0x903 */ u8 m7DF;
    /* 0x904 */ be<s32> m7E0;
    /* 0x908 */ be<u8> mbHasGa;
    /* 0x909 */ u8 m7E5[3];
    /* 0x90C */ ep_ga_s mEpGa[2];
    /* 0x984 */ u8 m860[0x44];        /* J3DLightInfo */
    /* 0x9C8 */ u8 m9C8[0xA44 - 0x9C8];
    /* 0xA44 */ u8 mA44[0x44];        /* HD: J3DLightInfo copy */
    /* 0xA88 */ u8 mA88[0xAC8 - 0xA88];
    /* 0xAC8 */ u8 mAC8[0x44];        /* HD: J3DLightInfo copy */
    /* 0xB0C */ u8 mB0C[0xB4C - 0xB0C];
    /* 0xB4C */ be<u8> mGroundCheckTimer;
    /* 0xB4D */ u8 mB4D[3];
};
WWHD_OFFSET(ep_class, m4D0, 0x5EC);
WWHD_OFFSET(ep_class, mTimers, 0x614);
WWHD_OFFSET(ep_class, mLight, 0x630);
WWHD_OFFSET(ep_class, mSph1, 0x7CC);
WWHD_OFFSET(ep_class, mEpGa, 0x90C);
WWHD_OFFSET(ep_class, mGroundCheckTimer, 0xB4C);
WWHD_SIZE(ep_class, 0xB50);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025E19CC mDoAud_seStart(id, pos) (two-argument form) */
static inline void mDoAud_seStart2(u32 id, cXyz* pos) { gabi::call(0x025E19CC, id, pos); }
/* 0254457C dEvent_manager_c::endCheckOld(const char*) */
static inline BOOL dComIfGp_evmng_endCheckOld(const char* name) { return gabi::call<BOOL>(0x0254457C, dComIfGp_getPEvtManager(), name); }
/* 02542EDC dEvent_manager_c::getMyActIdx(staffId, actions, n, force, nameType) */
static inline s32 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 actions, s32 n, s32 force, s32 nameType) {
    return gabi::call<s32>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, actions, n, force, nameType);
}
/* 025D77DC fopAcM_orderOtherEvent2(actor, name, flag, hind) */
static inline BOOL fopAcM_orderOtherEvent2(fopAc_ac_c* a, const char* name, u16 flag, u16 hind) {
    return gabi::call<BOOL>(0x025D77DC, a, name, flag, hind);
}
/* 023129C4 daObj::HitSeStart(const cXyz*, int roomNo, const dCcD_GObjInf*, u32) */
static inline void daObj_HitSeStart(cXyz* pos, s32 roomNo, dCcD_GObjInf* obj, u32 p) { gabi::call(0x023129C4, pos, roomNo, obj, p); }
/* 025D69FC fopAcM_rollPlayerCrash(actor, f32, u32) */
static inline void fopAcM_rollPlayerCrash(fopAc_ac_c* a, f32 r, u32 p) { gabi::call(0x025D69FC, a, r, p); }
/* 025D6B70 fopAcM_checkCullingBox(mtx, x0, y0, z0, x1, y1, z1) */
static inline u8 fopAcM_checkCullingBox(Mtx34* m, f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1) {
    return gabi::call<u8>(0x025D6B70, m, x0, y0, z0, x1, y1, z1);
}
/* mDoLib_clipper (HD): the J3DUClipper at 0x1048CFF0, far plane at +0x54, the default far at
 * +0x5C; 0283801C J3DUClipper::calcViewFrustum */
static inline void mDoLib_clipper_changeFar(f32 f) {
    gabi::store<f32>(0x1048CFF0 + 0x54, f);
    gabi::call(0x0283801C, 0x1048CFF0u);
}
static inline void mDoLib_clipper_resetFar() {
    f32 f = gabi::load<f32>(0x1048D04C);
    gabi::store<f32>(0x1048CFF0 + 0x54, f);
    gabi::call(0x0283801C, 0x1048CFF0u);
}
/* 025D672C / 025D673C fopAcM_SetMin / SetMax */
static inline void fopAcM_SetMin(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D672C, a, x, y, z); }
static inline void fopAcM_SetMax(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D673C, a, x, y, z); }
/* 025166F0 dCcD_Sph::dCcD_Sph (out of line) */
static inline void dCcD_Sph_ct(dCcD_Sph* s) { gabi::call(0x025166F0, s); }
/* daPy_py_c::getBokoFlamePos(cXyz*): virtual, HD vtable (+0xB4) slot +0x74 */
static inline void daPy_getBokoFlamePos(fopAc_ac_c* player, cXyz* pos) {
    gabi::call_ptr(gabi::load<u32>(player->__vtbl + 0x74), player, pos);
}
/* the stage data (play+0x5150) virtual at +0x15C: returns the stage info, whose word +0xC holds
 * the stage type in bits 16..18 */
static inline u32 dComIfGp_getStageStagInfo() {
    u32 st = dComIfGp_ea() + PLAY_STAGEDATA;
    return gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(st) + 0x15C), st);
}
/* sead::SafeString::assureTerminationImpl_ through this TU's SafeString vtable (+0x14) */
static inline void SafeString_assure(SafeString* s) { gabi::call_ptr(gabi::load<u32>(s->__vtbl + 0x14), s); }

/* HD: SafeString(lit) == the current stage name (play+0x5134), and the HD flag byte at
 * 0x1047E6C8 is 1 */
static bool ep_chk_sea_stage(u32 lit) {
    gabi::Local<SafeString> a;
    a->mStringTop = lit;
    a->__vtbl = SAFESTRING_VTBL;
    bool r = false;
    u32 play = dComIfGp_ea();
    gabi::Local<SafeString> b;
    b->mStringTop = play + 0x5134;
    b->__vtbl = SAFESTRING_VTBL;
    SafeString_assure(a.get());
    SafeString_assure(a.get());
    u32 s1 = a->mStringTop;
    SafeString_assure(b.get());
    u32 s2 = b->mStringTop;
    bool eq = false;
    if (s1 == s2) {
        eq = true;
    } else {
        for (u32 i = 0; i < 0x40001; i++) {
            u8 c0 = gabi::load<u8>(s1 + i);
            u8 c1 = gabi::load<u8>(s2 + i);
            if (c0 != c1)
                break;
            if (c0 == 0) {
                eq = true;
                break;
            }
        }
    }
    if (eq && (s8)gabi::load<u8>(0x1047E6C8) == 1)
        r = true;
    return r;
}
/* HD: the squared XZ distance of the player from (-300000, 0, -300000) */
static f32 ep_sea_dist2(cXyz* player_pos) {
    gabi::Local<cXyz> c;
    c->x = -300000.0f;
    c->y = 0.0f;
    c->z = -300000.0f;
    gabi::Local<cXyz> d;
    cXyz_mi(player_pos, d.get(), c.get());
    gabi::Local<cXyz> xz;
    f32 dz = d->z, dx = d->x;
    xz->z = dz;
    xz->x = dx;
    xz->y = 0.0f;
    return PSVECSquareMag(xz.get());
}
static void ep_player_pos(gabi::Local<cXyz>& p) {
    fopAc_ac_c* pl = dComIfGp_getPlayer(0);
    p->x = pl->current.pos.x;
    p->y = pl->current.pos.y;
    p->z = pl->current.pos.z;
}

/* ga_draw, inlined in daEp_Draw */
static void ga_draw(ep_class* i_this) {
    ep_ga_s* ga = i_this->mEpGa;
    for (s32 i = 0; i < 2; i++, ga++) {
        if (ga->mbEnabled == 1) {
            MtxTrans(ga->mPos.x, ga->mPos.y, ga->mPos.z, false);
            cMtx_YrotM(calc_mtx(), ga->rotY);
            cMtx_XrotM(calc_mtx(), ga->rotX);
            f32 s = ga->scaleXZ;
            MtxScale(s, s * ga->scaleY, s, true);
            MtxTrans(0.0f, REG_F(10, 9) + -2.0f, 0.0f, true); /* HD: GameCube -18.0f */
            J3DModel_setBaseTRMtx(ga->mpModel, calc_mtx());
            mDoExt_modelUpdateDL(ga->mpModel);
        }
    }
}

/* 0212D8DC. HD: no ep_draw (alpha-model glow); the model is drawn when mbShouldDrawModel is 0 */
static BOOL daEp_Draw(ep_class* i_this) {
    WWHD_FUNC(0x0212D8DC, BOOL, i_this);
    if ((i_this->mType == 0) || (i_this->mType == 3)) {
        if (i_this->mbShouldDrawModel == 0) {
            dComIfGd_setListBG();
            settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
            setLightTevColorType(dKy_getEnvlight(), i_this->mpModel, &i_this->tevStr);
            mDoExt_modelUpdateDL(i_this->mpModel);
            dComIfGd_setList();
        }
        if (i_this->mbHasGa != 0) {
            ga_draw(i_this);
        }
    }
    return TRUE;
}
VERIFY(0x0212D8DC, daEp_Draw);

/* 0212DAE8. HD: endCheckOld(name) */
BOOL ep_switch_event_end(ep_class* i_this) {
    WWHD_FUNC(0x0212DAE8, BOOL, i_this);
    BOOL ret = FALSE;
    if (dComIfGp_evmng_endCheckOld(STR(0x1000E418) /* "SHOKUDAI_SWITCH" */)) {
        dComIfGp_event_reset();
        ret = TRUE;
    }
    return ret;
}
VERIFY(0x0212DAE8, ep_switch_event_end);

/* 0212DB44 */
BOOL ep_switch_event_move(ep_class* i_this) {
    WWHD_FUNC(0x0212DB44, BOOL, i_this);
    BOOL ret = FALSE;
    if (dComIfGp_evmng_getIsAddvance(i_this->m7E0)) {
        /* static char* actions[] = { "WAIT", "FIRE" }; (0x101B48C8) */
        ret = dComIfGp_evmng_getMyActIdx(i_this->m7E0, 0x101B48C8, 2, FALSE, 0);
        switch (ret) {
        case 1:
            mDoAud_seStart2(JA_SE_OBJ_TORCH_IGNITION, &i_this->mPosTop);
            break;
        case 0:
        default:
            ret = FALSE;
            dComIfGp_evmng_cutEnd(i_this->m7E0);
            break;
        }
    }
    return ret;
}
VERIFY(0x0212DB44, ep_switch_event_move);

/* ga_move, inlined in daEp_Execute */
static void ga_move(ep_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    ep_ga_s* ga = &i_this->mEpGa[0];
    gabi::Local<cXyz> sp88;
    gabi::Local<cXyz> sp7C;
    gabi::Local<cXyz> sp58;
    sp88->set(0.0f, 0.0f, 10.0f);
    sp58->set(10000.0f, 10000.0f, 10000.0f);
    daPy_getBokoFlamePos(player, sp58.get());
    gabi::Local<dBgS_LinChk> linChk;
    dBgS_LinChk_ct(linChk.get(), EP_LINCHK_VT, false);
    for (s32 i = 0; i < 2; i++, ga++) {
        if (ga->mbEnabled != 0) {
            if (ga->m3B != 0) {
                ga->m3B = ga->m3B - 1;
            } else {
                ga->m3B = (u8)gabi::ftoi(cM_rndF(10.0f));
                f32 r = cM_rndFX(150.0f);
                ga->m1C.x = i_this->mPosTop.x + r;
                r = cM_rndFX(100.0f);
                ga->m1C.y = i_this->mPosTop.y + r;
                r = cM_rndFX(150.0f);
                ga->m1C.z = i_this->mPosTop.z + r;
                if (i == 0) {
                    gabi::Local<cXyz> tmp;
                    cXyz_mi(sp58.get(), tmp.get(), &ga->mPos);
                    gabi::Local<cXyz> sp64;
                    sp64->copy(*tmp.get());
                    if (std_sqrtf(PSVECSquareMag(sp64.get())) < 300.0f) { /* sp64.abs() */
                        r = cM_rndFX(100.0f);
                        ga->m1C.x = sp58->x + r;
                        r = cM_rndFX(100.0f);
                        ga->m1C.y = sp58->y + r;
                        r = cM_rndFX(100.0f);
                        ga->m1C.z = sp58->z + r;
                    }
                }
                f32 gy = ga->mPos.y;
                if (i_this->m4D0 == 0) {
                    ga->m1C.y = ga->m1C.y + 1000.0f;
                }
                if (std::fabs(i_this->mPosTop.y - gy) > 500.0f) {
                    ga->mbEnabled = 2;
                } else {
                    ga->mbEnabled = 1;
                }
            }
            gabi::Local<cXyz> sp70;
            cXyz_mi(&ga->m1C, sp70.get(), &ga->mPos);
            f32 x = sp70->x, z = sp70->z, y = sp70->y;
            cLib_addCalcAngleS2(&ga->rotY, cM_atan2s(x, z), 2, 0x1000);
            f32 d = std_sqrtf(gabi::fmadds(x, x, z * z));
            cLib_addCalcAngleS2(&ga->rotX, (s16)-cM_atan2s(y, d), 2, 0x1000);

            cMtx_YrotS(calc_mtx(), ga->rotY);
            cMtx_XrotM(calc_mtx(), ga->rotX);
            MtxPosition(sp88.get(), sp7C.get());
            ga->m10.copy(ga->mPos);
            PSVECAdd(&ga->mPos, sp7C.get(), &ga->mPos); /* ga->mPos += sp7C */
            if (i_this->m7D4 != 0) {
                gabi::Local<cXyz> sp4C;
                cMtx_YrotS(calc_mtx(), i_this->m7DC);
                f32 k = REG0_F(9) + 5.0f;
                sp4C->x = 0.0f;
                sp4C->y = 1.0f;
                sp4C->z = i_this->m7D8 * k;
                MtxPosition(sp4C.get(), sp7C.get());
                PSVECAdd(&ga->mPos, sp7C.get(), &ga->mPos);
                /* sp7C = ga->m10 + (ga->mPos - ga->m10) * 1.05f */
                gabi::Local<cXyz> t0, t1, t2;
                cXyz_mi(&ga->mPos, t0.get(), &ga->m10);
                cXyz_ml(t0.get(), t1.get(), 1.05f);
                cXyz_pl(&ga->m10, t2.get(), t1.get());
                sp7C->copy(*t2.get());
                dBgS_LinChk_Set(linChk.get(), &ga->m10, sp7C.get(), i_this);
                if (cBgS_LineCross(dComIfG_Bgsp(), linChk.get())) {
                    ga->mPos.copy(*gabi::at<cXyz>(gabi::ea(linChk.get()) + 0x30)); /* linChk.GetCross() */
                }
            }
            u16 a = (u16)ga->m38;
            ga->m38 = (s16)(a + 0x3E00);
            ga->scaleY = cM_ssin(a);
        }
    }
    /* ~dBgS_LinChk */
    u32 b = gabi::ea(linChk.get());
    gabi::store<u32>(b + 0x58, EP_LINCHK_VT.v58);
    gabi::store<u32>(b + 0x64, 0x1000E294);
    gabi::store<u32>(b + 0x20, 0x1000E284);
    cBgS_LinChk_dt(linChk.get(), 0);
}

/* the point-light update at the end of ep_move */
static void ep_light(ep_class* i_this) {
    i_this->mLight.mPos.copy(i_this->mPosTop);
    i_this->mLight.mColorR = 600;
    i_this->mLight.mColorG = 400;
    i_this->mLight.mColorB = 0x78;
    f32 k = 1.0f;
    if (dComIfGp_getStageStagInfo() != 0) {
        u32 info = dComIfGp_getStageStagInfo();
        u32 type = (gabi::load<u32>(info + 0xC) >> 16) & 7;
        if (type == 1 || type == 4) /* HD: dimmer in these stage types */
            k = 0.6521739363670349f; /* 15/23 */
    }
    i_this->mLight.mPower = ((150.0f * i_this->mLightPower) * i_this->mFlicker) * k; /* HD: flicker, k */
    i_this->mLight.mFluctuation = 250.0f;
    cLib_addCalc2(&i_this->mFlicker, cM_rndF(0.2f) + 0.9f, 0.25f, 0.01f);
}

/* ep_move, inlined in daEp_Execute */
static void ep_move(ep_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<cXyz> pos;
    pos->x = i_this->mPosTop.x;
    pos->z = i_this->mPosTop.z;
    pos->y = (i_this->mPosTop.y + 10.0f) + REG_F(6, 0); /* -240 + 235 + 15 */
    switch ((u32)(s32)i_this->m4D0) {
    case 0:
        if (i_this->m508 != 0) {
            cLib_addCalc0(&i_this->mLightPower, 0.5f, 0.05f);
            if (i_this->mSph1.ChkTgHit()) {
                void* hitObj = i_this->mSph1.GetTgHitObj();
                if (hitObj == nullptr || !cCcD_Obj_ChkAtType(hitObj, 0x600000)) {
                    if (i_this->m505 != 0xff) {
                        i_this->mTimers[3] = i_this->m505 * 10 + 1;
                    }
                    dComIfGs_onSwitch(i_this->m508 - 1, fopAcM_GetRoomNo(i_this));
                }
            }
            if (dComIfGs_isSwitch(i_this->m508 - 1, fopAcM_GetRoomNo(i_this))) {
                if (i_this->mType != 3) {
                    /* dComIfGp_evmng_existence("SHOKUDAI_SWITCH") */
                    u32 play = dComIfGp_ea();
                    u32 idx = gabi::call<u32>(0x02543F10, dComIfGp_getPEvtManager(), STR(0x1000E3F4), 0xFF); /* getEventIdx */
                    if (gabi::call<u32>(0x02544044, play + PLAY_EVTMANAGER, idx) != 0) {
                        /* ep_switch_event_begin, inlined */
                        s32 begin = FALSE;
                        if (!eventInfo_checkCommandDemoAccrpt(i_this)) {
                            fopAcM_orderOtherEvent2(i_this, STR(0x1000E3E4) /* "SHOKUDAI_SWITCH" */, 1, 0xFFFF);
                        } else {
                            eventInfo_onCondition(i_this, 2 /* dEvtCnd_UNK2_e */);
                            begin = dComIfGp_evmng_getMyStaffId(STR(0x1000E404) /* "SHOKUDAI" */, nullptr, 0);
                            if (begin == -1) {
                                ep_switch_event_end(i_this);
                            }
                        }
                        i_this->m7E0 = begin;
                        if (begin != 0) {
                            i_this->m508 = 0;
                            if (i_this->m7E0 != -1) {
                                ep_switch_event_move(i_this);
                            }
                        }
                        break;
                    }
                }
                mDoAud_seStart2(JA_SE_OBJ_TORCH_IGNITION, &i_this->mPosTop);
                i_this->m508 = 0;
            }
        } else if (i_this->mType != 3 && i_this->m7E0 != -1) {
            if (ep_switch_event_move(i_this)) {
                i_this->m4D0 = 3;
                i_this->m4E0 = i_this->scale.x;
            }
        } else {
            i_this->m4D0 = 3;
            i_this->m4E0 = i_this->scale.x;
        }
        break;
    case 3:
        i_this->m4D0 = 4;
        [[fallthrough]];
    case 4:
        cLib_addCalc2(&i_this->mLightPower, i_this->m4E0, 0.5f, 0.2f);
        if (i_this->mType != 2) {
            if (i_this->m7D4 < (s16)(REG0_S(7) + 7)) {
                /* HD: no fire on the sea stage outside a few areas */
                bool suppress = false;
                if (ep_chk_sea_stage(0x1000E258 /* "sea" */)) {
                    gabi::Local<cXyz> pp;
                    ep_player_pos(pp);
                    f32 py = pp->y, px = pp->x, pz = pp->z;
                    if (py < 300.0f && px < -290000.0f && pz > -297500.0f) {
                        f32 y = i_this->current.pos.y;
                        if (!(y < 500.0f) && !(2750.0f < y && y < 2800.0f) && !(1900.0f < y && y < 1950.0f) &&
                            !(2350.0f < y && y < 2400.0f))
                        {
                            if (!(ep_sea_dist2(pp.get()) < 12250000.0f))
                                suppress = true;
                        }
                    }
                }
                if (!suppress)
                    dComIfGp_particle_setSimple(1 /* ID_AK_JN_O_FIRE00 */, pos.get());
                if (i_this->m7D4 == 0 && i_this->mSph1.ChkTgHit()) {
                    void* hitObj = i_this->mSph1.GetTgHitObj();
                    if (hitObj != nullptr && cCcD_Obj_ChkAtType(hitObj, 0x600000)) {
                        /* hitObj->GetAc() (HD: through its dCcD_Stts, NULL-checked) */
                        u32 stts = gabi::load<u32>(gabi::ea(hitObj) + 0x44);
                        fopAc_ac_c* ac = stts != 0 ? gabi::at<fopAc_ac_c>(gabi::load<u32>(stts + 0xC)) : nullptr;
                        if (ac != nullptr && fpcM_GetName(ac) == fpcNm_BDK_e) {
                            i_this->m7DC = ac->shape_angle.y;
                        } else {
                            i_this->m7DC = player->shape_angle.y;
                        }
                        i_this->m7D4 = REG0_S(2) + 40;
                    }
                }
            }
            pos->y = pos->y + 20.0f;
            /* HD: no heat haze on the sea stage away from the area */
            gabi::Local<cXyz> pp;
            ep_player_pos(pp);
            bool suppress = false;
            if (ep_chk_sea_stage(0x1000E254 /* "sea" */)) {
                if (pp->y < 300.0f && !(ep_sea_dist2(pp.get()) < 12250000.0f))
                    suppress = true;
            }
            if (!suppress)
                dComIfGp_particle_setSimple(0x4004 /* ID_AK_JP_O_KAGEROU00 */, pos.get());
        }
        if (i_this->mTimers[3] == 1 && (i_this->m507 == 0xFF || !dComIfGs_isSwitch(i_this->m507, fopAcM_GetRoomNo(i_this)))) {
            dComIfGs_offSwitch(i_this->mOnSwitchNo, fopAcM_GetRoomNo(i_this));
        }
        if (i_this->mOnSwitchNo != 0xFF && !dComIfGs_isSwitch(i_this->mOnSwitchNo, fopAcM_GetRoomNo(i_this))) {
            i_this->m4D0 = 0;
            if (i_this->mOnSwitchNo != 0xff) {
                i_this->m508 = i_this->mOnSwitchNo + 1;
            }
            mDoAud_seStart2(JA_SE_OBJ_TORCH_OFF, &i_this->mPosTop);
        } else {
            mDoAud_seStart2(JA_SE_OBJ_TORCH_BURNING, &i_this->mPosTop);
        }
        [[fallthrough]];
    default:
        if (i_this->mType != 3 && i_this->m7E0 != -1) {
            if (ep_switch_event_end(i_this)) {
                i_this->m7E0 = -1;
            } else {
                dComIfGp_evmng_cutEnd(i_this->m7E0);
            }
        }
        break;
    case 10:
        cLib_addCalc0(&i_this->mLightPower, 1.0f, 0.1f);
        if (i_this->mLightPower < 0.05f) {
            if (i_this->mType == 0 || i_this->mType == 3) {
                i_this->m4D0 = 0;
                if (i_this->mOnSwitchNo != 0xFF) {
                    dComIfGs_offSwitch(i_this->mOnSwitchNo, fopAcM_GetRoomNo(i_this));
                    i_this->m508 = i_this->mOnSwitchNo + 1;
                }
            } else {
                fopAcM_delete(i_this);
            }
        }
        break;
    }
    ep_light(i_this);
    f32 lp = i_this->mLightPower;
    i_this->scale.y = lp;
    if (lp > 0.5f) {
        i_this->mSph1.OnAtSPrmBit(1); /* OnAtSetBit */
    } else {
        i_this->mSph1.OffAtSPrmBit(1);
    }
    gabi::Local<cXyz> sp3C;
    sp3C->x = i_this->mPosTop.x;
    sp3C->y = i_this->mPosTop.y + 30.0f;
    sp3C->z = i_this->mPosTop.z;
    i_this->mSph1.SetC(sp3C.get());
    dComIfG_Ccsp_Set(&i_this->mSph1);
    if (i_this->m7D4 != 0) {
        s16 tmp = REG0_S(2) + 40;
        if (i_this->m7D4 == tmp && i_this->mpEmitter == nullptr) {
            gabi::Local<cXyz> scale;
            f32 s = REG0_F(6) + 1.0f;
            scale->z = s;
            scale->y = s;
            scale->x = s;
            pos->z = i_this->mPosTop.z;
            pos->x = i_this->mPosTop.x;
            pos->y = (i_this->mPosTop.y + 3.0f) + REG_F(6, 4); /* -240 + 235 + 8 */
            i_this->mpEmitter = dComIfGp_particle_set(0x1EA /* ID_AK_JN_TORCH */, pos.get(), nullptr, scale.get());
        }
        if (i_this->mpEmitter != nullptr) {
            f32 target = (i_this->m7D4 > (s16)(REG0_S(3) + 10)) ? REG0_F(5) + 4.0f : 0.0f;
            cLib_addCalc2(&i_this->m7D8, target, 1.0f, REG0_F(8) + 0.5f);
            cMtx_YrotS(calc_mtx(), i_this->m7DC);
            gabi::Local<cXyz> sp24;
            gabi::Local<cXyz> sp18;
            sp24->set(0.0f, 1.0f, i_this->m7D8);
            MtxPosition(sp24.get(), sp18.get());
            f32 dx = sp18->x, dz = sp18->z, dy = sp18->y;
            JPABaseEmitter_setDirection(i_this->mpEmitter, dx, dy, dz);
            if (i_this->m7D4 == 1) {
                /* becomeInvalidEmitter() */
                u32 e = gabi::ea(i_this->mpEmitter.get());
                gabi::store<s32>(e + 0x5C, -1);
                gabi::store<u32>(e + 0x254, gabi::load<u32>(e + 0x254) | 1);
                i_this->mpEmitter = nullptr;
            }
        }
        i_this->m7D4 = i_this->m7D4 - 1;
    }
}

/* 0212DBFC */
static BOOL daEp_Execute(ep_class* i_this) {
    WWHD_FUNC(0x0212DBFC, BOOL, i_this);
    fopAc_ac_c* a_this = i_this;
    i_this->mbNoEp = 0;
    i_this->mbShouldDrawModel = 0;
    if ((i_this->mType == 0) || (i_this->mType == 3)) {
        i_this->mCyl.SetC(&a_this->current.pos);
        dComIfG_Ccsp_Set(&i_this->mCyl);
        if (i_this->mCyl.ChkTgHit()) {
            daObj_HitSeStart(&a_this->current.pos, fopAcM_GetRoomNo(a_this), &i_this->mCyl, 11);
        }
        fopAcM_rollPlayerCrash(a_this, 35.0f, 0);
        mDoLib_clipper_changeFar(1000000.0f);
        i_this->mbShouldDrawModel =
            fopAcM_checkCullingBox(J3DModel_getBaseTRMtx(i_this->mpModel), -30.0f, 0.0f, -30.0f, 30.0f, 180.0f, 30.0f);
        mDoLib_clipper_resetFar();
        if (i_this->mbHasGa != 0) {
            ga_move(i_this);
        }
    }
    for (s32 i = 0; i < 3; i++) {
        if (i_this->mTimers[i] != 0) {
            i_this->mTimers[i] = i_this->mTimers[i] - 1;
        }
    }
    if ((i_this->mTimers[3] != 0) && (i_this->mTimers[3] < 10000)) {
        i_this->mTimers[3] = i_this->mTimers[3] - 1;
    }
    if (i_this->mTimers[0] == 0) {
        i_this->mTimers[0] = (s16)gabi::ftoi(cM_rndF(REG0_F(2) + 5.0f) + REG0_F(3));
        i_this->mAlphaModelAlphaTarget = (cM_rndF(REG0_F(6) + 4.0f) + 8.0f) + REG0_F(7);
    }
    if (i_this->mTimers[1] == 0) {
        if (i_this->m7D4 != 0) {
            i_this->mTimers[1] = (s16)gabi::ftoi(cM_rndF(5.0f));
            i_this->mAlphaModelScaleTarget = cM_rndF(0.2f) + 0.55f;
        } else {
            i_this->mTimers[1] = (s16)gabi::ftoi(cM_rndF(6.0f) + 3.0f);
            i_this->mAlphaModelScaleTarget = cM_rndF(0.075f) + 0.75f;
        }
    }
    /* HD: REG0_F(4) + 1.0f (GameCube REG0_F(4) + 0.1f + 0.9f) */
    cLib_addCalc2(&i_this->mAlphaModelAlpha, i_this->mAlphaModelAlphaTarget, 1.0f, REG0_F(4) + 1.0f);
    cLib_addCalc2(&i_this->mAlphaModelScale, i_this->mAlphaModelScaleTarget, 0.4f, 0.04f);
    MtxTrans(i_this->mPosTop.x, i_this->mPosTop.y, i_this->mPosTop.z, false);
    cMtx_YrotM(calc_mtx(), i_this->mAlphaModelRotY);
    cMtx_XrotM(calc_mtx(), i_this->mAlphaModelRotX);
    f32 scale = i_this->mAlphaModelScale * i_this->mLightPower;
    MtxScale(scale, scale, scale, true);
    PSMTXCopy(calc_mtx(), &i_this->mAlphaModelMtx); /* cMtx_copy */
    mDoLib_clipper_changeFar(1000000.0f);
    i_this->mbNoEp = fopAcM_checkCullingBox(&i_this->mAlphaModelMtx, -160.0f, -160.0f, -160.0f, 160.0f, 160.0f, 160.0f);
    mDoLib_clipper_resetFar();
    ep_move(i_this);
    i_this->mAlphaModelRotY = i_this->mAlphaModelRotY + 0xd0;
    i_this->mAlphaModelRotX = i_this->mAlphaModelRotX + 0x100;
    if (i_this->m4D0 > 0) {
        if (i_this->m4D0 < 10) {
            i_this->mGroundCheckTimer = i_this->mGroundCheckTimer + 1;
            if ((i_this->mGroundCheckTimer & 0xF) == 0) {
                /* dBgS_ObjGndChk_Spl gndChk */
                gabi::Local<dBgS_GndChk> gndChk;
                dBgS_GndChk_ct(gndChk.get(), EP_GNDCHK_VT, true);
                gabi::store<u32>(gabi::ea(gndChk.get()) + 0x50, 0xE); /* Spl group */
                gabi::Local<cXyz> sp08;
                f32 z = i_this->mPosTop.z;
                f32 y = i_this->mPosTop.y + 200.0f;
                f32 x = i_this->mPosTop.x;
                sp08->z = z;
                sp08->x = x;
                cXyz* p = gabi::at<cXyz>(gabi::ea(gndChk.get()) + 0x24); /* gndChk.SetPos(&sp08) */
                p->x = x;
                p->y = y;
                p->z = z;
                f32 fVar6 = cBgS_GroundCross(dComIfG_Bgsp(), gndChk.get());
                if ((fVar6 != -1000000000.0f) && (i_this->mPosTop.y < fVar6)) {
                    i_this->m4D0 = 10;
                }
                /* ~dBgS_ObjGndChk_Spl */
                u32 b = gabi::ea(gndChk.get());
                gabi::store<u32>(b + 0x20, 0x1000E2B4);
                gabi::store<u32>(b + 0x40, 0x1000E2D4);
                gabi::store<u32>(b + 0x4C, 0x1000E294);
                gabi::call(0x02008DAC, gndChk.get(), 0); /* cBgS_Chk::~cBgS_Chk */
            }
        }
    }
    return TRUE;
}
VERIFY(0x0212DBFC, daEp_Execute);

/* 0212F578 */
static BOOL daEp_IsDelete(ep_class*) {
    WWHD_FUNC(0x0212F578, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0212F578, daEp_IsDelete);

/* 0212F580 */
static BOOL daEp_Delete(ep_class* i_this) {
    WWHD_FUNC(0x0212F580, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhase, STR(0x1000E4F8) /* "Ep" */); /* dComIfG_resDeleteDemo */
    dKy_plight_cut(&i_this->mLight);
    mDoAud_seDeleteObject(&i_this->mPosTop);
    return TRUE;
}
VERIFY(0x0212F580, daEp_Delete);

/* 0212F5CC */
static BOOL daEp_CreateHeap(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x0212F5CC, BOOL, a_this);
    ep_class* i_this = (ep_class*)a_this;
    J3DModelData* modelData;
    if (i_this->mbHasObm == 0) {
        modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x1000E508) /* "Ep" */, dRes_INDEX_EP_BDL_VKTSD_e, SAFESTRING_VTBL);
    } else {
        modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x1000E508), dRes_INDEX_EP_BDL_OBM_SHOKUDAI1_e, SAFESTRING_VTBL);
    }
    if (modelData == nullptr) /* JUT_ASSERT(997, modelData != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x477, STR(0x1000E518));
    i_this->mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    if (i_this->mpModel == nullptr) {
        return FALSE;
    }
    if (i_this->mbHasGa != 0) {
        modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x1000E508), dRes_INDEX_EP_BDL_EP_GA_e, SAFESTRING_VTBL);
        if (modelData == nullptr) /* JUT_ASSERT(1010, modelData != NULL) */
            JUT_ASSERT_fail(FILE_NAME, 0x484, STR(0x1000E518));
        for (s32 i = 0; i < 2; i++) {
            i_this->mEpGa[i].mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
            if (i_this->mEpGa[i].mpModel == nullptr) {
                return FALSE;
            }
            if (i == 0 || cM_rndF(1.0f) < 0.5f) {
                ep_ga_s& ga = i_this->mEpGa[i];
                ga.mbEnabled = true;
                ga.mPos.x = a_this->current.pos.x;
                f32 y = a_this->current.pos.y;
                ga.mPos.y = y;
                ga.mPos.z = a_this->current.pos.z;
                ga.mPos.y = y + 140.0f;
                ga.scaleXZ = cM_rndF(0.3f) + 0.3f;
                ga.m38 = (s16)gabi::ftoi(cM_rndF(30000.0f));
            }
        }
    }
    return TRUE;
}
VERIFY(0x0212F5CC, daEp_CreateHeap);

/* 0212F878: ep_class::ep_class() (HD: allocates when this == NULL) */
static ep_class* ep_class_ct(ep_class* self) {
    WWHD_FUNC(0x0212F878, ep_class*, self);
    if (self == nullptr) {
        self = (ep_class*)operator_new(0xB50);
        if (self == nullptr)
            return self;
    }
    fopAc_ac_c_ct(self);
    self->__vtbl = EP_VTBL;
    dBgS_AcchCir_ct(&self->mAcchCir);
    dBgS_ObjAcch_ct(&self->mAcch, EP_OBJACCH_VT);
    self->mLight.mHD20 = 1.0f;
    dCcD_Stts_ct(&self->mStts);
    dCcD_Cyl_ct(&self->mCyl, 0x1000E274);
    dCcD_Sph_ct(&self->mSph1);
    /* m860 = j3dDefaultLightInfo (0x1016E414); HD: also copied to +0xA44 and +0xAC8 */
    const u32 src = 0x1016E414;
    f32 fa[6], fb[8];
    u8 b[4];
    s16 h[4];
    for (int i = 0; i < 6; i++) fa[i] = gabi::load<f32>(src + 4 * i);
    for (int i = 0; i < 4; i++) b[i] = gabi::load<u8>(src + 0x18 + i);
    for (int i = 0; i < 4; i++) h[i] = gabi::load<s16>(src + 0x1C + 2 * i);
    for (int i = 0; i < 8; i++) fb[i] = gabi::load<f32>(src + 0x24 + 4 * i);
    const u32 dst[3] = {gabi::ea(self->m860), gabi::ea(self->mA44), gabi::ea(self->mAC8)};
    for (u32 d : dst) {
        for (int i = 0; i < 6; i++) gabi::store<f32>(d + 4 * i, fa[i]);
        for (int i = 0; i < 4; i++) gabi::store<u8>(d + 0x18 + i, b[i]);
        for (int i = 0; i < 4; i++) gabi::store<s16>(d + 0x1C + 2 * i, h[i]);
        for (int i = 0; i < 8; i++) gabi::store<f32>(d + 0x24 + 4 * i, fb[i]);
    }
    return self;
}
VERIFY(0x0212F878, ep_class_ct);

/* daEp_set_mtx, inlined */
static void daEp_set_mtx(ep_class* i_this) {
    if ((i_this->mType == 0) || (i_this->mType == 3)) {
        MtxTrans(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z, false);
        cMtx_YrotM(calc_mtx(), i_this->current.angle.y);
        cMtx_XrotM(calc_mtx(), i_this->current.angle.x);
        cMtx_ZrotM(calc_mtx(), i_this->current.angle.z);
        J3DModel_setBaseTRMtx(i_this->mpModel, calc_mtx());
        gabi::Local<cXyz> sp08;
        sp08->x = 0.0f;
        sp08->z = 0.0f;
        sp08->y = REG0_F(0) + 140.0f;
        MtxPosition(sp08.get(), &i_this->mPosTop);
    } else {
        i_this->mPosTop.copy(i_this->current.pos);
    }
}

/* daEp_CreateInit, inlined */
static void daEp_CreateInit(fopAc_ac_c* a_this) {
    ep_class* i_this = (ep_class*)a_this;
    i_this->m4D0 = 0;
    i_this->m508 = 0;
    a_this->cullMtx = gabi::ea(&i_this->mAlphaModelMtx); /* fopAcM_SetMtx */
    fopAcM_SetMin(a_this, -160.0f, -160.0f, -160.0f);
    fopAcM_SetMax(a_this, 160.0f, 160.0f, 160.0f);
    i_this->mAlphaModelRotX = (s16)gabi::ftoi(cM_rndF(0x8000));
    i_this->mAlphaModelRotY = (s16)gabi::ftoi(cM_rndF(0x8000));
    cXyz* att = gabi::at<cXyz>(gabi::ea(a_this) + 0x390); /* attention_info.position */
    f32 x = a_this->current.pos.x, y = a_this->current.pos.y;
    att->x = x;
    att->y = y + 100.0f;
    f32 z = a_this->current.pos.z;
    att->z = z;
    a_this->eyePos.x = x;
    a_this->eyePos.y = y + 130.0f;
    a_this->eyePos.z = z;
    daEp_set_mtx(i_this);
    i_this->mbNoEp = 0;
    i_this->mbShouldDrawModel = 0;
}

/* 0212FAF4 */
static cPhs_State daEp_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x0212FAF4, cPhs_State, a_this);
    ep_class* i_this = (ep_class*)a_this;
    /* fopAcM_ct(a_this, ep_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            ep_class_ct(i_this);
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    cPhs_State ret = dComIfG_resLoad(&i_this->mPhase, STR(0x1000E538) /* "Ep" */);
    if (ret == cPhs_COMPLEATE_e) {
        i_this->mType = fopAcM_GetParam(a_this) & 0x3F;
        if (i_this->mType == 0x3F) {
            i_this->mType = 0;
        }
        i_this->mbHasGa = (fopAcM_GetParam(a_this) >> 6) & 1;
        i_this->mbHasObm = (fopAcM_GetParam(a_this) >> 7) & 1;
        i_this->m505 = fopAcM_GetParam(a_this) >> 8;
        i_this->m507 = fopAcM_GetParam(a_this) >> 0x10;
        i_this->mOnSwitchNo = fopAcM_GetParam(a_this) >> 0x18;
        i_this->mStts.Init(0xFF, 0xFF, a_this);
        i_this->mSph1.Set(gabi::at<dCcD_SrcSph>(0x101B48D0) /* sph_src */);
        i_this->mSph1.SetStts(&i_this->mStts);
        if ((i_this->mType == 0) || (i_this->mType == 3)) {
            u32 maxHeapSize;
            if (i_this->mbHasObm == 0) {
                maxHeapSize = 0x4C0;
            } else {
                maxHeapSize = 0x4E0;
            }
            if (i_this->mbHasGa != 0) {
                maxHeapSize += 0x9A0;
            }
            if (!fopAcM_entrySolidHeap(a_this, 0x0212F5CC /* daEp_CreateHeap */, maxHeapSize)) {
                return cPhs_ERROR_e;
            }
            i_this->mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101B4910) /* co_cyl_src */);
            i_this->mCyl.SetStts(&i_this->mStts);
            i_this->mAcch.Set(&a_this->current.pos, &a_this->old.pos, a_this, 1, &i_this->mAcchCir, &a_this->speed, nullptr, nullptr);
            i_this->mAcchCir.SetWall(10.0f, 20.0f);
            i_this->mAcch.CrrPos(dComIfG_Bgsp());
        }
        daEp_CreateInit(a_this);
        if (i_this->mOnSwitchNo != 0xFF && !dComIfGs_isSwitch(i_this->mOnSwitchNo, fopAcM_GetRoomNo(a_this))) {
            i_this->m508 = i_this->mOnSwitchNo + 1;
        }
        i_this->mTimers[3] = 20000;
        dKy_plight_set(&i_this->mLight);
        i_this->mGroundCheckTimer = (u8)gabi::ftoi(cM_rndF(255.0f));
    }

    i_this->m7E0 = -1;
    i_this->mFlicker = 1.0f; /* HD */
    return ret;
}
VERIFY(0x0212FAF4, daEp_Create);

/* 0212FF28 */
static void __sinit_d_a_ep_cpp() {
    WWHD_FUNC(0x0212FF28, void, (u32)0);
    sinit_header_statics(0x10463C78, 0x101B4954);
}
VERIFY(0x0212FF28, __sinit_d_a_ep_cpp);

/* 0212FFBC: sead::SafeString deleting destructor (this TU's SafeString vtable) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0212FFBC, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x0212FFBC, SafeString_dt);

/* 0212FFD0: ep_class deleting destructor (vtable +0xC) */
static void ep_class_dt(ep_class* i_this, s32 flags) {
    WWHD_FUNC(0x0212FFD0, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x02515AE8, &i_this->mSph1, 2); /* dCcD_Sph::~dCcD_Sph */
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        u32 acch = gabi::ea(&i_this->mAcch); /* ~dBgS_ObjAcch (inline) */
        gabi::store<u32>(acch + 0x20, EP_OBJACCH_VT.v20);
        gabi::store<u32>(acch + 0x14, EP_OBJACCH_VT.v14);
        gabi::call(0x024EFD9C, &i_this->mAcch, 0);                     /* dBgS_Acch::~dBgS_Acch */
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir */
        gabi::call(0x025D50BC, i_this, 0);                             /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0212FFD0, ep_class_dt);

/* 02130078: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void* p) { WWHD_FUNC(0x02130078, void, p); }
VERIFY(0x02130078, SafeString_assureTerminationImpl);
