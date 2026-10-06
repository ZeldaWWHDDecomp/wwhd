/**
 * d_a_cc.cpp (WWHD)
 * Enemy - ChuChu
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_cc.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * daCC_Execute: d_a_cc_exec.cpp; action_noboru, action_up_check, body_atari_check: d_a_cc_act.cpp.
 */
#include "d/actor/d_a_cc.h"

#define SAFESTRING_VTBL 0x1000C548 /* this TU's sead::SafeString vtable */
#define CC_VTBL 0x1000C5A0         /* cc_class vtable (HD virtual destructor) */
#define CC_ARC STR(0x1000C6CF)     /* "CC" (useHeapInit) */

enum { fpcNm_BOMB_e = 0x126, fpcNm_Bomb2_e = 0x127, fpcNm_TSUBO_e = 0x1C5 };
enum { DSNAP_TYPE_CC = 0xAC };
enum { CC_JNT_CENTER_e = 0, CC_JNT_BODY03_e = 3 };
enum { JA_SE_OBJ_FALL_WATER_M = 0x6919, JA_SE_CM_CC_LIE_TO_STAND = 0x5860 };

/* ---- file statics ---- */
static be<s32>& target_info_count() { return *gabi::at<be<s32>>(0x10462CF0); }
static gptr<fopAc_ac_c>* target_info() { return gabi::at<gptr<fopAc_ac_c>>(0x10462D00); } /* [10] */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02041570 enemy_fire(enemyfire*), 02041C30 enemy_fire_remove(enemyfire*) */
static inline void enemy_fire(enemyfire_l* f) { gabi::call(0x02041570, f); }
static inline void enemy_fire_remove(enemyfire_l* f) { gabi::call(0x02041C30, f); }
/* 02591284 dMat_ice_c::updateDL(mDoExt_McaMorf*, int, mDoExt_invisibleModel*) */
static inline void dMat_control_iceUpdateDL(mDoExt_McaMorf* m, s32 p, void* inv) { gabi::call(0x02591284, m, p, inv); }
/* 025E83FC brkAnm::entry / 025E7FC4 btkAnm::entry with the anm's own frame (+4) (HD) */
static inline f32 anm_frame(void* anm) { return gabi::load<f32>(gabi::ea(anm) + 4); }
static inline void anm_setFrame(void* anm, f32 f) { gabi::store<f32>(gabi::ea(anm) + 4, f); }
/* HD: mDoExt_brkAnm::remove / mDoExt_btkAnm::remove inline: clear the model data's tev-register
 * (+0x48) and texture-SRT (+0x44) animation */
static inline void brk_remove(J3DModel* model) { gabi::store<u32>(gabi::load<u32>(gabi::ea(model) + 0xAC) + 0x48, 0); }
static inline void btk_remove(J3DModel* model) { gabi::store<u32>(gabi::load<u32>(gabi::ea(model) + 0xAC) + 0x44, 0); }
static inline J3DModelData* modelData(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
/* 020CB92C daBomb_c::chk_state(int), 020CBAAC daBomb2::Act_c::chk_explode() */
static inline BOOL daBomb_chk_state(void* b, s32 st) { return gabi::call<BOOL>(0x020CB92C, b, st); }
static inline BOOL daBomb2_chk_explode(void* b) { return gabi::call<BOOL>(0x020CBAAC, b); }
/* 02008254 cBgS::ChkPolySafe, 024EF0BC dBgS::GetGroundCode */
static inline BOOL cBgS_ChkPolySafe(dBgS* bgs, void* poly) { return gabi::call<BOOL>(0x02008254, bgs, poly); }
static inline s32 dBgS_GetGroundCode(dBgS* bgs, void* poly) { return gabi::call<s32>(0x024EF0BC, bgs, poly); }
/* 025A9270 dPa_rippleEcallBack::end (remove) */
static inline void ripple_remove(void* r) { gabi::call(0x025A9270, r); }
/* 0252A038 dDetect_c::chk_light(cXyz*) (play+0x5A20) */
static inline BOOL dComIfGp_getDetect_chk_light(cXyz* pos) { return gabi::call<BOOL>(0x0252A038, dComIfGp_ea() + PLAY_DETECT, pos); }
/* dComIfGp_roomControl_getStayNo: s8 at 0x1047E6C8 (HD) */
static inline s32 dComIfGp_roomControl_getStayNo() { return gabi::load<s8>(0x1047E6C8); }
/* HD fopAcM_seStart inline where the actor is known non-null (only &eyePos is checked) */
static inline void fopAcM_seStart_nn(fopAc_ac_c* a, u32 id, u32 param) {
    if (gabi::ea(&a->eyePos) != 0)
        mDoAud_seStart(id, &a->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
/* 025E80D0 mDoExt_brkAnm::mDoExt_brkAnm (matcher: "mDoExt_brkAnm::init"), 025E8154 brkAnm::init,
 * 025E7C6C mDoExt_btkAnm::mDoExt_btkAnm (matcher: "init"), 025E7CE0 btkAnm::init,
 * 025E8A48 mDoExt_invisibleModel::create */
static inline BOOL mDoExt_anm_init(u32 fn, void* a, J3DModelData* d, void* key, s32 anmPlay, s32 attr, f32 rate, s16 start,
                                   s16 end, bool modify, s32 entry) {
    return gabi::call<BOOL>(fn, a, d, key, anmPlay, attr, start, end, modify, entry, rate);
}

/* harness workaround: the matcher names the anm constructors "::init", so the harness compares a
 * GameCube stack parameter at SP+8; the original still holds the last outgoing stack argument there */
static void* new_anm(u32 size, u32 ctor, u32 stale_sp8) {
    void* p = operator_new(size);
    struct Frame { be<u32> w[4]; };
    gabi::Local<Frame> fr;
    fr->w[2] = stale_sp8;
    return p != nullptr ? gabi::call<void*>(ctor, p) : nullptr;
}

/* HD J3D joint matrices (see d_a_ki.cpp) */
static Mtx34* cc_getAnmMtx(u32 model, s32 jntNo) {
    u32 blk = gabi::load<u32>(model + 0x2C);
    gabi::store<u16>(blk + 4, gabi::load<u16>(blk + 4) | 0x10);
    return gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jntNo * 0x30);
}

/* 0210B874 */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x0210B874, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DJoint* joint = J3DNode_toJoint(node);
        u32 model = gabi::load<u32>(0x104B462C);
        cc_class* i_this = gabi::at<cc_class>(gabi::load<u32>(model + 0xB8));
        s32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);

        if (i_this != nullptr) {
            if (jntNo == CC_JNT_CENTER_e) {
                PSMTXCopy(cc_getAnmMtx(model, jntNo), calc_mtx());
                cMtx_ZrotM(calc_mtx(), i_this->m3BA);
                Mtx34* dst = cc_getAnmMtx(model, jntNo);
                mtx_copy(dst, calc_mtx());
                PSMTXCopy(calc_mtx(), gabi::at<Mtx34>(0x104B4868));
            }

            if (jntNo <= CC_JNT_BODY03_e) {
                PSMTXCopy(cc_getAnmMtx(model, jntNo), calc_mtx());
                f32 fVar1 = (f32)jntNo * 0.5f;
                cMtx_YrotM(calc_mtx(), (s16)gabi::ftoi((f32)i_this->m310.y * fVar1));
                cMtx_XrotM(calc_mtx(), (s16)gabi::ftoi((f32)i_this->m310.x * fVar1));
                cMtx_ZrotM(calc_mtx(), (s16)gabi::ftoi((f32)i_this->m310.z * fVar1));

                if (jntNo == CC_JNT_BODY03_e) {
                    gabi::Local<cXyz> sp08;
                    sp08->set(0.0f, 0.0f, 0.0f);
                    MtxPosition(sp08, &i_this->m470);
                }
                Mtx34* dst = cc_getAnmMtx(model, jntNo);
                mtx_copy(dst, calc_mtx());
                PSMTXCopy(calc_mtx(), gabi::at<Mtx34>(0x104B4868));
            }
        }
    }
    return TRUE;
}
VERIFY(0x0210B874, nodeCallBack);

/* 0210BB58 */
static void draw_SUB(cc_class* i_this) {
    WWHD_FUNC(0x0210BB58, void, i_this);
    if (i_this->mBehaviorType == 2 && i_this->m2FB != 3) {
        i_this->m2B4->calc();
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->actor.current.pos, &i_this->actor.tevStr);
        enemy_fire(&i_this->mEnemyFire);
        return;
    }

    J3DModel* pJVar1;
    if (i_this->m2F5 != 0x35) {
        if (i_this->m301 == 0) {
            pJVar1 = i_this->m2B4->getModel();
        } else {
            pJVar1 = i_this->m2D8->getModel();
        }
    } else {
        pJVar1 = i_this->m2BC->getModel();
    }

    Mtx34* now = mDoMtx_stack_c::get();
    PSMTXTrans(now, i_this->actor.current.pos.x, i_this->actor.current.pos.y - i_this->m328, i_this->actor.current.pos.z);
    mDoMtx_XrotM(now, i_this->m316.x);
    mDoMtx_ZrotM(now, i_this->m316.z);
    mDoMtx_YrotM(now, i_this->actor.shape_angle.y);
    mDoMtx_XrotM(now, i_this->actor.shape_angle.x);
    mDoMtx_ZrotM(now, i_this->actor.shape_angle.z);

    f32 sx, sy, sz;
    if (i_this->mCurrAction != 5 && i_this->m2F5 != 0x35 && i_this->m301 == 0 && i_this->mCurrAction != 6 && i_this->m34E[4] == 0) {
        sx = 1.1f;
        sy = 1.0f;
        sz = 0.9f;
        i_this->m31C += 1000;
    } else {
        sx = sy = sz = i_this->m334;
        i_this->m31C = 0;
    }

    mDoMtx_YrotM(now, i_this->m31C);
    mDoMtx_stack_scaleM(sx, sy, sz);
    mDoMtx_YrotM(now, -i_this->m31C);
    J3DModel_setBaseTRMtx(pJVar1, now);

    if (i_this->m301 == 0) {
        i_this->m2B4->calc();
        enemy_fire(&i_this->mEnemyFire);
    }

    if (i_this->m2FE != 0) {
        J3DModel* model = i_this->m2C4->getModel();
        J3DModel_setBaseTRMtx(model, now);
    }

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->actor.current.pos, &i_this->actor.tevStr);
}
VERIFY(0x0210BB58, draw_SUB);

/* 0210BFEC */
static BOOL daCC_Draw(cc_class* i_this) {
    WWHD_FUNC(0x0210BFEC, BOOL, i_this);
    fopAc_ac_c* a_this = &i_this->actor;
    J3DModel* pJVar3 = i_this->m2B4->getModel();

    if (i_this->mBehaviorType == 4) {
        return TRUE;
    }

    dSnap_RegistFig(DSNAP_TYPE_CC, a_this, 1.0f, 1.0f, 1.0f);

    if (i_this->m2F5 != 0x35) {
        mDoExt_brkAnm_entry(i_this->m2B8, modelData(pJVar3), anm_frame(i_this->m2B8));
        anm_setFrame(i_this->m2B8, (f32)(u8)i_this->mColorType);

        if (i_this->m2FE != 0) {
            pJVar3 = i_this->m2C4->getModel();

            switch (i_this->m2FE) {
            case 1:
                mDoExt_brkAnm_entry(i_this->m2C8, modelData(pJVar3), anm_frame(i_this->m2C8));
                anm_setFrame(i_this->m2C8, (f32)(u8)i_this->m2FF);
                mDoExt_btkAnm_entry(i_this->m2D0, modelData(pJVar3), anm_frame(i_this->m2D0));
                anm_setFrame(i_this->m2D0, (f32)(u8)i_this->m2FF);
                break;
            case 2:
                mDoExt_brkAnm_entry(i_this->m2CC, modelData(pJVar3), anm_frame(i_this->m2CC));
                anm_setFrame(i_this->m2CC, (f32)(u8)i_this->m2FF);
                mDoExt_btkAnm_entry(i_this->m2D4, modelData(pJVar3), anm_frame(i_this->m2D4));
                anm_setFrame(i_this->m2D4, (f32)(u8)i_this->m2FF);
                break;
            }
        }

        if (i_this->m301 != 0) {
            pJVar3 = i_this->m2D8->getModel();
        }
    } else {
        pJVar3 = i_this->m2BC->getModel();
        mDoExt_brkAnm_entry(i_this->m2C0, modelData(pJVar3), anm_frame(i_this->m2C0));
        anm_setFrame(i_this->m2C0, (f32)(u8)i_this->mColorType);
    }

    setLightTevColorType(dKy_getEnvlight(), pJVar3, &a_this->tevStr);

    if (i_this->mEnemyIce.mFreezeTimer > 0x14) {
        dMat_control_iceUpdateDL(i_this->m2B4, -1, i_this->mDFC);
        return TRUE;
    }

    dComIfGp_get(); /* HD: the play object is fetched, its result unused (GameCube: shadow) */
    if (i_this->m2F5 != 0x35) {
        if (i_this->m301 == 0) {
            i_this->m2B4->entryDL();
            a_this->model = gabi::ea(i_this->m2B4->getModel());
        } else {
            i_this->m2D8->updateDL();
        }

        if (i_this->m2FE != 0) {
            i_this->m2C4->updateDL();
        }
    } else {
        i_this->m2BC->updateDL();
    }

    /* HD: no simple shadow */

    if (i_this->m2F5 != 0x35) {
        brk_remove(i_this->m2B4->getModel());
        if (i_this->m2FE != 0) {
            pJVar3 = i_this->m2C4->getModel();
            switch (i_this->m2FE) {
            case 1:
            case 2:
                brk_remove(pJVar3);
                btk_remove(pJVar3);
                break;
            }
        }
    } else {
        brk_remove(pJVar3);
    }
    return TRUE;
}
VERIFY(0x0210BFEC, daCC_Draw);

/* 0210C364 */
static void cc_eff_set(cc_class* i_this, unsigned char arg1) {
    WWHD_FUNC(0x0210C364, void, i_this, arg1);
    u16 userID = 0x3F0; /* dPa_name::ID_AK_JN_CCHEAD00 */
    /* The colour table is a stack local indexed by mColorType without a bounds check. To keep
     * out-of-range indices (only reachable with a clobbered mColorType) identical, the candidate
     * reproduces the original's frame above the table: table at entry SP - 0x24, then the saved
     * r28..r31, then the caller's frame with the LR save word. */
    struct Frame {
        u8 _00[0xC];
        GXColor c[5];      /* entry SP - 0x24 */
        be<u32> saved[4];  /* r28..r31 */
    };
    gabi::store<u32>(gabi::cpu->r[1] + 4, gabi::cpu->lr); /* the prologue's LR save (entry SP + 4) */
    gabi::Local<Frame> fr;
    for (int i = 0; i < 4; i++) fr->saved[i] = gabi::cpu->r[28 + i];
    gabi::Local<cXyz> sp1C;
    sp1C->x = i_this->m470.x;
    sp1C->y = i_this->m470.y;
    sp1C->z = i_this->m470.z;

    /* HD: the colour table is a local, its 4th entry taken from the particle manager */
    u32 pa = gabi::ea(dComIfGp_getParticle());
    if (pa == 0) {
        return;
    }
    GXColor* c = fr->c;
    c[0].r = 0x19; c[0].g = 0xBE; c[0].b = 0x05;
    c[1].r = 0xC8; c[1].g = 0x14; c[1].b = 0x00;
    c[2].r = 0x00; c[2].g = 0x64; c[2].b = 0xFF;
    c[3].r = gabi::load<u8>(pa + 0xF4); c[3].g = gabi::load<u8>(pa + 0xF5); c[3].b = gabi::load<u8>(pa + 0xF6);
    c[4].r = 0xFF; c[4].g = 0xFF; c[4].b = 0x00;

    if (arg1 != 0) {
        userID = 0x3EF; /* dPa_name::ID_AK_JN_CCFOOT00 */
        sp1C->copy(i_this->actor.current.pos);
    }

    JPABaseEmitter* pJVar6 = dComIfGp_particle_set(userID, sp1C, &i_this->actor.shape_angle);
    if (pJVar6 != nullptr) {
        u32 tev = gabi::ea(&i_this->actor.tevStr) + 0x98; /* mColorK0 */
        const GXColor& col = c[(u8)i_this->mColorType];
        f32 tmp = (f32)gabi::load<u8>(tev + 0) / 255.0f;
        u8 r = (u8)gabi::ftoi((f32)(u8)col.r * tmp);
        tmp = (f32)gabi::load<u8>(tev + 1) / 255.0f;
        u8 g = (u8)gabi::ftoi((f32)(u8)col.g * tmp);
        tmp = (f32)gabi::load<u8>(tev + 2) / 255.0f;
        u8 b = (u8)gabi::ftoi((f32)(u8)col.b * tmp);
        /* setGlobalPrmColor(r, g, b) */
        u32 e = gabi::ea(pJVar6);
        gabi::store<u8>(e + 0x244, r);
        gabi::store<u8>(e + 0x245, g);
        gabi::store<u8>(e + 0x246, b);
        /* HD: setGlobalEnvColor(K0.r, K0.g, K0.b) */
        gabi::store<u8>(e + 0x248, gabi::load<u8>(tev + 0));
        gabi::store<u8>(e + 0x249, gabi::load<u8>(tev + 1));
        gabi::store<u8>(e + 0x24A, gabi::load<u8>(tev + 2));
    }
}
VERIFY(0x0210C364, cc_eff_set);

/* 0210C5C8 */
static void anm_init(cc_class* i_this, int transformResIdx, float morf, unsigned char loopMode, float speed, int soundResIdx) {
    WWHD_FUNC(0x0210C5C8, void, i_this, transformResIdx, morf, loopMode, speed, soundResIdx);
    i_this->m320 = transformResIdx;
    if (soundResIdx >= 0) {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x1000C628), transformResIdx, SAFESTRING_VTBL);
        void* bas = dComIfG_getObjectRes(STR(0x1000C628), soundResIdx, SAFESTRING_VTBL);
        i_this->m2B4->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, bas);
    } else {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x1000C628), transformResIdx, SAFESTRING_VTBL);
        i_this->m2B4->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, nullptr);
    }
}
VERIFY(0x0210C5C8, anm_init);

/* 0210C6F4 */
static void damage_mode_move(cc_class* i_this) {
    WWHD_FUNC(0x0210C6F4, void, i_this);
    dComIfGp_get(); /* HD: unused player fetch */
    i_this->m2F6 = 2;
    if (i_this->m2F5 != 0x2D && i_this->m2F5 != 0x2E) {
        dComIfGp_particle_set(0x27B /* ID_IT_JN_PIYOHIT00 */, gabi::at<cXyz>(gabi::ea(&i_this->actor) + 0x390) /* attention_info.position */);
        i_this->mCurrAction = 3;
        i_this->m2F5 = 0x28;
    }
}
VERIFY(0x0210C6F4, damage_mode_move);

/* 0210C788 */
static void* s_b_sub(void* arg0, void*) {
    WWHD_FUNC(0x0210C788, void*, arg0);
    if (fopAc_IsActor(arg0) && arg0 != nullptr) {
        bool bVar2 = false;
        if (fpcM_GetName(arg0) == fpcNm_BOMB_e) {
            if (daBomb_chk_state(arg0, 0 /* daBomb_c::STATE_0 */)) {
                bVar2 = true;
            }
        } else if (fpcM_GetName(arg0) == fpcNm_Bomb2_e) {
            if (daBomb2_chk_explode(arg0)) {
                bVar2 = true;
            }
        }

        if (bVar2 && target_info_count() < 10) {
            s32 n = target_info_count();
            target_info_count() = n + 1;
            target_info()[n] = (fopAc_ac_c*)arg0;
        }
    }
    return nullptr;
}
VERIFY(0x0210C788, s_b_sub);

/* 0210C828 */
static void naraku_check(cc_class* i_this) {
    WWHD_FUNC(0x0210C828, void, i_this);
    fopAc_ac_c* a_this = &i_this->actor;
    void* gnd = gabi::at<u8>(gabi::ea(&i_this->mAcch) + 0xE8); /* m_gnd (its cBgS_PolyInfo) */
    if (i_this->mAcch.GetGroundH() != -1000000000.0f && cBgS_ChkPolySafe(dComIfG_Bgsp(), gnd) &&
        dBgS_GetGroundCode(dComIfG_Bgsp(), gnd) == 4) {
        i_this->m2FA += 1;
        if (a_this->current.pos.y < -500.0f || i_this->m2FA > 0x32) {
            a_this->speedF = 0.0f;
            a_this->speed.x = 0.0f;
            a_this->speed.y = 0.0f;
            a_this->speed.z = 0.0f;
            a_this->gravity = 0.0f;
            fopAcM_delete(a_this);
            dComIfGs_onActor(a_this->setID, a_this->home.roomNo); /* fopAcM_onActor */
            return;
        }
    }

    if (a_this->current.pos.y < i_this->m3BC.y - 20000.0f) {
        fopAcM_delete(a_this);
        dComIfGs_onActor(a_this->setID, a_this->home.roomNo);
    } else if (i_this->mAcch.ChkWaterIn()) {
        if (i_this->mCurrAction != 1) {
            gabi::Local<cXyz> sp24;
            sp24->x = a_this->current.pos.x;
            sp24->y = a_this->current.pos.y;
            sp24->z = a_this->current.pos.z;
            sp24->y = gabi::load<f32>(gabi::ea(&i_this->mAcch) + 0x1BC); /* m_wtr.GetHeight() */
            if (!i_this->m300) {
                fopKyM_createWpillar(sp24, 1.0f, 0.4f, 0);
                fopAcM_seStart_nn(a_this, JA_SE_OBJ_FALL_WATER_M, 0);
                i_this->m300 = 1;
                gabi::Local<cXyz> sp18;
                sp18->set(0.5f, 0.5f, 0.5f);
                ripple_remove(i_this->m390);
                /* dComIfGp_particle_setShipTail(ID_AK_JN_HAMON00, &pos, NULL, &sp18, 0xFF, &m390) */
                dPa_control_set(dComIfGp_getParticle(), 5, 0x33, &a_this->current.pos, nullptr, sp18, 0xFF,
                                (dPa_levelEcallBack*)i_this->m390, -1, nullptr, nullptr, nullptr);
                gabi::store<f32>(gabi::ea(i_this->m390) + 0x10, 0.0f); /* m390.setRate(0.0f) */
            }

            f32 fVar1 = REG_F(12, 0) + 40.0f;
            if (i_this->m2FE != 0) {
                fVar1 = REG_F(12, 0) + 100.0f;
            }

            if (a_this->current.pos.y < sp24->y - fVar1) {
                if (i_this->m2FE != 0) {
                    a_this->speedF = 0.0f;
                    a_this->speed.x = 0.0f;
                    a_this->speed.y = 0.0f;
                    a_this->speed.z = 0.0f;
                    a_this->gravity = 0.0f;
                    fopAcM_delete(a_this);
                    dComIfGs_onActor(a_this->setID, a_this->home.roomNo);
                }

                i_this->mCurrAction = 1;
                i_this->m2F5 = 0x14;

                if (a_this->health <= 0 && i_this->mCurrAction != 4) {
                    i_this->mCurrAction = 4;
                    i_this->m2F5 = 0x32;
                }
            }
        }
    } else if (i_this->m300) {
        i_this->m2F8 = 0;
        i_this->m300 = 0;
        ripple_remove(i_this->m390);
    }
}
VERIFY(0x0210C828, naraku_check);

/* 0210CBC0 */
static void denki_start(cc_class* i_this) {
    WWHD_FUNC(0x0210CBC0, void, i_this);
    if (i_this->mColorType != 2 && i_this->mColorType != 4) {
        return;
    }
    if (i_this->m2F7 != 0) {
        return;
    }
    i_this->mCyl.OnAtSPrmBit(1 /* cCcD_AtSPrm_Set_e */);
    i_this->mCyl.mObjAt.mRPrm = 1; /* OnAtHitBit (HD: stores the bit) */
    i_this->m2F7 = 1;
    i_this->mCyl.SetTgSpl(1);
}
VERIFY(0x0210CBC0, denki_start);

/* 0210CC00 */
static void denki_end(cc_class* i_this) {
    WWHD_FUNC(0x0210CC00, void, i_this);
    if (i_this->mColorType == 2 || i_this->mColorType == 4) {
        /* HD: dPa_followEcallBack::end (GameCube: remove()) */
        dPa_followEcallBack_end(&i_this->m368);
        dPa_followEcallBack_end(&i_this->m37C);
        i_this->mCyl.SetTgSpl(0);
        i_this->m2F7 = 0;
    }
}
VERIFY(0x0210CC00, denki_end);

/* 0210CC58 */
static BOOL shock_damage_check(cc_class* i_this) {
    WWHD_FUNC(0x0210CC58, BOOL, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    target_info_count() = 0;
    for (s32 i = 0; i < 10; i++) {
        target_info()[i] = nullptr;
    }

    fpcM_Search(0x0210C788 /* s_b_sub */, &i_this->actor);

    if (target_info_count() != 0) {
        for (s32 i = 0; i < target_info_count(); i++) {
            fopAc_ac_c* t = target_info()[i];
            f32 x = t->current.pos.x - i_this->actor.current.pos.x;
            f32 z = t->current.pos.z - i_this->actor.current.pos.z;
            if (std_sqrtf(gabi::fmadds(x, x, z * z)) < 1000.0f) {
                if (i_this->m302 == 0) {
                    damage_mode_move(i_this);
                    i_this->m302 = 1;
                    return TRUE;
                }
            } else {
                i_this->m302 = 0;
            }
        }
    } else {
        i_this->m302 = 0;
    }

    /* player->checkHammerQuake(): status bit 0x20000 at +0x3C0; getSwordTopPos(): +0x3E4 */
    if (gabi::load<u32>(gabi::ea(player) + 0x3C0) & 0x20000) {
        f32 x = gabi::load<f32>(gabi::ea(player) + 0x3E4) - i_this->actor.current.pos.x;
        f32 z = gabi::load<f32>(gabi::ea(player) + 0x3EC) - i_this->actor.current.pos.z;
        if (std_sqrtf(gabi::fmadds(x, x, z * z)) < 1000.0f) {
            damage_mode_move(i_this);
            return TRUE;
        }
    }
    return FALSE;
}
VERIFY(0x0210CC58, shock_damage_check);

/* 0210CE2C */
static void black_light_check(cc_class* i_this) {
    WWHD_FUNC(0x0210CE2C, void, i_this);
    if (i_this->mColorType == 3) {
        if (i_this->mCurrAction != 6 && dComIfGp_getDetect_chk_light(&i_this->actor.current.pos)) {
            i_this->mStts.SetWeight(0xFE);
            i_this->mCurrAction = 6;
            i_this->m2F5 = 0x50;
        }
    }
}
VERIFY(0x0210CE2C, black_light_check);

/* 0210DA18 */
static BOOL search_angle_set(cc_class* i_this) {
    WWHD_FUNC(0x0210DA18, BOOL, i_this);
    if (i_this->mBehaviorType != 3) {
        if (fopAcM_searchPlayerDistance(&i_this->actor) > 40.0f) {
            s16 a = fopAcM_searchPlayerAngleY(&i_this->actor);
            i_this->actor.current.angle.y = a + i_this->m34C;
            return TRUE;
        }
    } else {
        f32 fVar7 = i_this->m3BC.x - i_this->actor.current.pos.x;
        f32 fVar1 = i_this->m3BC.z - i_this->actor.current.pos.z;
        f32 d2 = gabi::fmadds(fVar7, fVar7, fVar1 * fVar1);
        if (std_sqrtf(d2) > i_this->m340) {
            i_this->actor.current.angle.y = cM_atan2s(fVar7, fVar1);
        } else if (std_sqrtf(d2) < 100.0f && cM_rnd() < 0.1f &&
                   (s16)cLib_distanceAngleS(i_this->actor.shape_angle.y, i_this->actor.current.angle.y) < 0x100) {
            f32 y = (f32)i_this->actor.current.angle.y;
            i_this->actor.current.angle.y = (s16)gabi::ftoi(y + cM_rndFX(16384.0f));
        }
    }
    return FALSE;
}
VERIFY(0x0210DA18, search_angle_set);

/* 0210DBE0 */
static void* tsubo_search(void* arg1, void* arg2) {
    WWHD_FUNC(0x0210DBE0, void*, arg1, arg2);
    fopAc_ac_c* actor1 = (fopAc_ac_c*)arg1;
    cc_class* i_this = (cc_class*)arg2;
    if (fopAc_IsActor(actor1) && actor1 != nullptr && fpcM_GetName(actor1) == fpcNm_TSUBO_e &&
        actor1->current.pos.x == i_this->actor.current.pos.x &&
        std::fabs((f32)(actor1->current.pos.y - i_this->actor.current.pos.y)) < 10.0f &&
        actor1->current.pos.z == i_this->actor.current.pos.z) {
        i_this->m30C = fopAcM_GetID(actor1);
    }
    return nullptr;
}
VERIFY(0x0210DBE0, tsubo_search);

/* 0210DC8C */
static void BG_check(cc_class* i_this) {
    WWHD_FUNC(0x0210DC8C, void, i_this);
    if (i_this->m2F9 == 0) {
        i_this->mAcchCir.SetWall(20.0f, 30.0f);
        i_this->mAcch.CrrPos(dComIfG_Bgsp());
        if (i_this->m2F8 != 0 && i_this->mAcch.ChkWaterIn()) {
            i_this->actor.current.pos.y = gabi::load<f32>(gabi::ea(&i_this->mAcch) + 0x1BC) - 20.0f;
        }
        fopAcM_getGroundAngle(&i_this->actor, &i_this->m316);
        naraku_check(i_this);
    }
}
VERIFY(0x0210DC8C, BG_check);

/* 021104AC */
static BOOL daCC_IsDelete(cc_class*) {
    WWHD_FUNC(0x021104AC, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021104AC, daCC_IsDelete);

/* 021104B4 */
static BOOL daCC_Delete(cc_class* i_this) {
    WWHD_FUNC(0x021104B4, BOOL, i_this);
    i_this->m368.remove();
    i_this->m37C.remove();
    ripple_remove(i_this->m390);
    dComIfG_resDelete(&i_this->mPhase, STR(0x1000C6CC));
    enemy_fire_remove(&i_this->mEnemyFire);
    return TRUE;
}
VERIFY(0x021104B4, daCC_Delete);

static mDoExt_McaMorf* cc_new_morf(J3DModelData* data, J3DAnmTransform* anm, f32 speed, s32 start) {
    return mDoExt_McaMorf::create(nullptr, data, nullptr, nullptr, anm, J3DFrameCtrl::EMode_NONE, speed, start, -1, 1, nullptr, 0,
                                  0x11020203);
}

/* 02110528 */
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x02110528, BOOL, a_this);
    cc_class* i_this = (cc_class*)a_this;

    s32 fileIndex = 0x1E; /* dRes_INDEX_CC_BCK_START_e */
    if (i_this->mBehaviorType == 1) {
        fileIndex = 0x21; /* BCK_TSTART01 */
    } else if (i_this->mBehaviorType == 2) {
        fileIndex = 0x1B; /* BCK_HUSE2TACHI */
    }

    J3DModelData* data = (J3DModelData*)dComIfG_getObjectRes(CC_ARC, 0x2A /* BMD_CC */, SAFESTRING_VTBL);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(CC_ARC, fileIndex, SAFESTRING_VTBL);
    i_this->m2B4 = cc_new_morf(data, anm, 0.0f, 0);
    if (i_this->m2B4 == nullptr || i_this->m2B4->getModel() == nullptr) {
        return FALSE;
    }

    gabi::store<u32>(gabi::ea(i_this->m2B4->getModel()) + 0xB8, gabi::ea(i_this)); /* setUserArea */

    for (u16 i = 0; i < gabi::load<u16>(gabi::ea(gabi::call<void*>(0x027F3F94, modelData(i_this->m2B4->getModel()))) + 8); i++) {
        /* getJointNodePointer(i)->setCallBack(nodeCallBack) (HD: 0x1C-byte joints, element 0 past the end) */
        u32 md = gabi::ea(modelData(i_this->m2B4->getModel()));
        u32 n = gabi::load<u32>(md + 4);
        u32 p = gabi::load<u32>(md + 8);
        if (i < n) p += i * 0x1C;
        gabi::store<u32>(p + 8, 0x0210B874);
    }

    J3DModel* model = i_this->m2B4->getModel();

    i_this->m2B8 = (mDoExt_brkAnm*)new_anm(0x78, 0x025E80D0, 1);
    if (i_this->m2B8 == nullptr) {
        return FALSE;
    }
    void* key = dComIfG_getObjectRes(CC_ARC, 0x2D /* BRK_CC */, SAFESTRING_VTBL);
    if (!mDoExt_anm_init(0x025E8154, i_this->m2B8, modelData(model), key, 1, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }

    data = (J3DModelData*)dComIfG_getObjectRes(CC_ARC, 0x26 /* BDL_CC_IWA */, SAFESTRING_VTBL);
    i_this->m2C4 = cc_new_morf(data, nullptr, 0.0f, 0);
    if (i_this->m2C4 == nullptr || i_this->m2C4->getModel() == nullptr) {
        return FALSE;
    }

    model = i_this->m2C4->getModel();
    i_this->m2C8 = (mDoExt_brkAnm*)new_anm(0x78, 0x025E80D0, 1);
    if (i_this->m2C8 == nullptr) {
        return FALSE;
    }
    key = dComIfG_getObjectRes(CC_ARC, 0x2F /* BRK_CC_IWA01 */, SAFESTRING_VTBL);
    if (!mDoExt_anm_init(0x025E8154, i_this->m2C8, modelData(model), key, 1, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }

    i_this->m2CC = (mDoExt_brkAnm*)new_anm(0x78, 0x025E80D0, 0);
    if (i_this->m2CC == nullptr) {
        return FALSE;
    }
    key = dComIfG_getObjectRes(CC_ARC, 0x30 /* BRK_CC_IWA02 */, SAFESTRING_VTBL);
    if (!mDoExt_anm_init(0x025E8154, i_this->m2CC, modelData(model), key, 1, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }

    i_this->m2D0 = (mDoExt_btkAnm*)new_anm(0x74, 0x025E7C6C, 0);
    if (i_this->m2D0 == nullptr) {
        return FALSE;
    }
    key = dComIfG_getObjectRes(CC_ARC, 0x33 /* BTK_CC_IWA01 */, SAFESTRING_VTBL);
    if (!mDoExt_anm_init(0x025E7CE0, i_this->m2D0, modelData(model), key, 1, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }

    i_this->m2D4 = (mDoExt_btkAnm*)new_anm(0x74, 0x025E7C6C, 0);
    if (i_this->m2D4 == nullptr) {
        return FALSE;
    }
    key = dComIfG_getObjectRes(CC_ARC, 0x34 /* BTK_CC_IWA02 */, SAFESTRING_VTBL);
    if (!mDoExt_anm_init(0x025E7CE0, i_this->m2D4, modelData(model), key, 1, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }

    data = (J3DModelData*)dComIfG_getObjectRes(CC_ARC, 0x25 /* BDL_CC_BETA */, SAFESTRING_VTBL);
    i_this->m2BC = cc_new_morf(data, nullptr, 0.0f, 0);
    if (i_this->m2BC == nullptr || i_this->m2BC->getModel() == nullptr) {
        return FALSE;
    }

    model = i_this->m2BC->getModel();
    i_this->m2C0 = (mDoExt_brkAnm*)new_anm(0x78, 0x025E80D0, 1);
    if (i_this->m2C0 == nullptr) {
        return FALSE;
    }
    key = dComIfG_getObjectRes(CC_ARC, 0x2E /* BRK_CC_BETA */, SAFESTRING_VTBL);
    if (!mDoExt_anm_init(0x025E8154, i_this->m2C0, modelData(model), key, 1, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }

    data = (J3DModelData*)dComIfG_getObjectRes(CC_ARC, 0x27 /* BDL_CC_PTCL */, SAFESTRING_VTBL);
    anm = (J3DAnmTransform*)dComIfG_getObjectRes(CC_ARC, 0x18 /* BCK_CC_PTCL */, SAFESTRING_VTBL);
    i_this->m2D8 = cc_new_morf(data, anm, 1.0f, 1);
    if (i_this->m2D8 == nullptr || i_this->m2D8->getModel() == nullptr) {
        return FALSE;
    }

    if (!gabi::call<bool>(0x025E8A48, i_this->mDFC, i_this->m2B4->getModel())) { /* mDFC.create(model) */
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x02110528, useHeapInit);

/* 02110B28: enemyfire::enemyfire (compiler-generated; this TU's copy) */
static enemyfire_l* enemyfire_ct(enemyfire_l* p) {
    WWHD_FUNC(0x02110B28, enemyfire_l*, p);
    if (p == nullptr) {
        p = (enemyfire_l*)operator_new(0x22C);
        if (p == nullptr) return p;
    }
    if (gabi::ea(p) + 0x8C == 0)
        operator_new(0xC);
    dCcD_Stts_ct(&p->mStts);
    gabi::call(0x025166F0, &p->mSph);
    p->m228 = 1.0f;
    return p;
}
VERIFY(0x02110B28, enemyfire_ct);

static void cc_ObjAcch_ct(dBgS_ObjAcch* a) {
    gabi::call(0x024F0474, a);
    gabi::store<u32>(gabi::ea(a) + 0x10, 0x1000C570);
    gabi::store<u32>(gabi::ea(a) + 0x14, 0x1000C590);
    gabi::store<u8>(gabi::ea(a) + 0x18, 1);
    gabi::store<u32>(gabi::ea(a) + 0x20, 0x1000C580);
}

/* 02110BB4 */
static cc_class* cc_class_ct(cc_class* p) {
    WWHD_FUNC(0x02110BB4, cc_class*, p);
    if (p == nullptr) {
        p = (cc_class*)operator_new(0xF10);
        if (p == nullptr) return p;
    }
    fopAc_ac_c_ct(&p->actor);
    p->actor.__vtbl = CC_VTBL;
    dPa_followEcallBack_ct(&p->m368, 0, 0);
    dPa_followEcallBack_ct(&p->m37C, 0, 0);
    gabi::call(0x025A9084, p->m390); /* dPa_rippleEcallBack::dPa_rippleEcallBack */
    gabi::call(0x024EFE94, &p->mAcchCir);
    cc_ObjAcch_ct(&p->mAcch);
    dCcD_Stts_ct(&p->mStts);
    dCcD_Cyl_ct(&p->mCyl, 0x1000C560);
    dCcD_Stts_ct(&p->mEnemyIce.mStts);
    dCcD_Cyl_ct(&p->mEnemyIce.mCyl, 0x1000C560);
    gabi::call(0x024EFE94, &p->mEnemyIce.mBgAcchCir);
    cc_ObjAcch_ct(&p->mEnemyIce.mBgAcch);
    enemyfire_ct(&p->mEnemyFire);
    gabi::call(0x025E895C, p->mDFC); /* mDoExt_invisibleModel::mDoExt_invisibleModel */
    return p;
}
VERIFY(0x02110BB4, cc_class_ct);

/* 02110D4C */
static cPhs_State daCC_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x02110D4C, cPhs_State, a_this);
    cc_class* i_this = (cc_class*)a_this;

    /* fopAcM_ct(a_this, cc_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            cc_class_ct(i_this);
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    cPhs_State res = dComIfG_resLoad(&i_this->mPhase, STR(0x1000C700));
    if (res == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(a_this, 0x02110528 /* useHeapInit */, 0x47A0)) {
            return cPhs_ERROR_e;
        }

        u32 prm = fopAcM_GetParam(a_this);
        i_this->mBehaviorType = prm & 0xFF;
        i_this->mColorType = (prm >> 8) & 0xFF;
        i_this->mNoticeRangeByte = (prm >> 0x10) & 0xFF;
        i_this->mDisableSpawnSwitchNo = (prm >> 0x18) & 0xFF;

        if (REG_S(8, 9) != 0) {
            i_this->mColorType = REG_S(8, 9) - 1;
        }
        if (REG_S(8, 8) != 0) {
            i_this->mBehaviorType = 3;
        } else if (i_this->mBehaviorType == 0xFF) {
            i_this->mBehaviorType = 0;
        }
        if (i_this->mColorType == 0xFF) {
            i_this->mColorType = 0;
        }
        if (i_this->mNoticeRangeByte == 0xFF) {
            i_this->mNoticeRangeByte = 0;
        }

        if (i_this->mBehaviorType != 4) {
            if (i_this->mNoticeRangeByte == 0) {
                i_this->mNoticeRange = 1000.0f;
                if (i_this->mBehaviorType == 1) {
                    i_this->mNoticeRange = 1200.0f;
                }
            } else {
                i_this->mNoticeRange = (f32)(u8)i_this->mNoticeRangeByte * 10.0f;
            }

            if (i_this->mColorType >= 10) {
                i_this->mColorType -= 10;
                i_this->mNoticeRange = 60000.0f;
                if (i_this->mColorType == 5) {
                    i_this->m304 = 1;
                    i_this->mColorType = 1;
                }
            }
        } else {
            i_this->mNoticeRange = (f32)(u8)i_this->mNoticeRangeByte * 10.0f;
        }

        i_this->m3BC.copy(a_this->current.pos);
        a_this->actor_status &= ~0x20u; /* fopAcM_OffStatus(SHOWMAP) */
        a_this->stealItemLeft = 1;

        switch (i_this->mColorType) {
        case 0:
            a_this->gbaName = 0x16;
            a_this->itemTableIdx = gabi::call<s32>(0x0200E814, dComIfGp_ea() + PLAY_NAMETBL, STR(0x1000C6E0) /* "c_green" */, 0);
            break;
        case 1:
            a_this->gbaName = 0x15;
            a_this->itemTableIdx = gabi::call<s32>(0x0200E814, dComIfGp_ea() + PLAY_NAMETBL, STR(0x1000C6F8) /* "c_red" */, 0);
            break;
        case 2:
            a_this->gbaName = 0x1E;
            a_this->itemTableIdx = gabi::call<s32>(0x0200E814, dComIfGp_ea() + PLAY_NAMETBL, STR(0x1000C704) /* "c_blue" */, 0);
            break;
        case 3:
            a_this->gbaName = 0x10;
            a_this->stealItemLeft = 3;
            a_this->itemTableIdx = gabi::call<s32>(0x0200E814, dComIfGp_ea() + PLAY_NAMETBL, STR(0x1000C6E8) /* "c_black" */, 0);
            a_this->actor_status |= 0x8000000u;
            break;
        case 4:
            a_this->gbaName = 10;
            a_this->itemTableIdx = gabi::call<s32>(0x0200E814, dComIfGp_ea() + PLAY_NAMETBL, STR(0x1000C6F0) /* "c_kiiro" */, 0);
            break;
        }

        a_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->m2B4->getModel()));
        gabi::store<u32>(gabi::ea(a_this) + 0x39C, 0); /* attention_info.flags */

        i_this->mAcch.Set(&a_this->current.pos, &a_this->old.pos, a_this, 1, &i_this->mAcchCir, &a_this->speed, nullptr, nullptr);

        if (i_this->mBehaviorType == 1) {
            a_this->shape_angle.x = -0x8000;
        }

        i_this->mStts.Init(0x32, 4, a_this);

        if (i_this->m304) {
            /* SetTgGrp(IsOther | IsPlayer | IsEnemy | Set) */
            i_this->mCyl.mObjTg.mSPrm = (i_this->mCyl.mObjTg.mSPrm & ~0xEu) | 0xF;
        }

        i_this->mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101B3B84)); /* body_co_cyl */
        i_this->mCyl.SetStts(&i_this->mStts);
        u32 kind = i_this->mColorType;
        if (kind >= 5) { /* HD: JUT_ASSERT(a_arg1 < sizeof(cc_HP_kind) / sizeof(cc_HP_kind[0])), then index 0 */
            JUT_ASSERT_fail(STR(0x1000C740), 0xFB6, STR(0x1000C70C));
            kind = 0;
        }
        i_this->mCyl.SetAtSpl((u8)gabi::load<s32>(0x101B3B28 + 4 * kind));   /* cc_atsp_kind */
        i_this->mCyl.SetAtAtp((u8)gabi::load<s32>(0x101B3B3C + 4 * kind));   /* cc_atatp_kind */
        a_this->max_health = gabi::load<s8>(0x101B3B00 + kind);               /* cc_HP_kind */
        a_this->health = gabi::load<s8>(0x101B3B00 + kind);

        i_this->m34C = (s16)gabi::ftoi(cM_rndFX((f32)(s32)((fopAcM_GetID(a_this) & 7) * 0x500)));
        i_this->m334 = 1.0f;
        i_this->mCyl.OffAtSPrmBit(1); /* OffAtSPrmBit(Set), ClrAtSet */
        i_this->m2F9 = 1;
        i_this->mEnemyIce.mpActor = a_this;
        i_this->mEnemyIce.mWallRadius = 35.0f;
        i_this->mEnemyIce.mCylHeight = 100.0f;
        i_this->mEnemyFire.mpMcaMorf = i_this->m2B4;
        i_this->mEnemyFire.mpActor = a_this;

        for (int i = 0; i < 10; i++) {
            i_this->mEnemyFire.mFlameJntIdxs[i] = gabi::load<s8>(0x101B3B78 + i);         /* fire_j */
            i_this->mEnemyFire.mParticleScale[i] = gabi::load<f32>(0x101B3B50 + 4 * i);   /* fire_sc */
        }

        if (i_this->mBehaviorType == 2) {
            a_this->gravity = 0.0f;
            gabi::store<u8>(0x101B3AFD, 0); /* DEMO_COME_START_FLAG etc. (HD: in .data) */
            gabi::store<u8>(0x101B3AFE, 0);
            gabi::store<u8>(0x101B3AFC, 0);
            i_this->mCyl.OnTgSPrmBit(1); /* OnTgSetBit */
            i_this->mCyl.OnCoSPrmBit(1); /* OnCoSetBit */
            i_this->mDisableSpawnSwitchNo = 0xFF;
            i_this->mCurrAction = 5;
            i_this->m2F5 = 0x3C;
            J3DModel_setBaseTRMtx(i_this->m2B4->getModel(), &i_this->m7EC);
            draw_SUB(i_this);
            return res;
        }

        f32 r = (f32)a_this->current.angle.z * 10.0f;
        if (!(r >= 100.0f)) {
            r = 100.0f;
        }
        a_this->current.angle.z = 0;
        i_this->m340 = r;
        a_this->shape_angle.z = 0;

        if (i_this->mBehaviorType != 3) {
            if (i_this->mDisableSpawnSwitchNo != 0xFF) {
                if (i_this->mBehaviorType != 1) {
                    i_this->m2F9 = 0;
                    a_this->gravity = -3.0f;
                }
                i_this->mCyl.OffTgSPrmBit(1); /* OffTgSetBit */
                i_this->mCyl.OffCoSPrmBit(1); /* OffCoSetBit */
                i_this->mCyl.ClrTgHit();
                i_this->mCurrAction = 7;
                i_this->m2F5 = 100;
            }
        } else {
            a_this->stealItemBitNo = i_this->mDisableSpawnSwitchNo;
            i_this->mStts.SetWeight(0xFF);
            a_this->actor_status &= ~0x80000u;
        }

        if (i_this->mBehaviorType == 4) {
            i_this->mCurrAction = 8;
            i_this->m2F5 = 0x6E;
        }

        BG_check(i_this);
        draw_SUB(i_this);

        fopEn_enemy_c& en = i_this->actor;
        en.mBtHeight = REG_F(8, 4) + 150.0f; /* initBt */
        en.mBtBodyR = REG_F(8, 5) + 100.0f;
        en.mBtAttackType = 1;                /* setBtAttackData(0, 10, REG8_F(6) + 500, OPENING_JUMP_PARRY) */
        en.mBtStartFrame = 0.0f;
        en.mBtEndFrame = 10.0f;
        en.mBtMaxDis = REG_F(8, 6) + 500.0f;
        en.mBtNowFrame = 1000.0f;            /* setBtNowFrame */
    }
    return res;
}
VERIFY(0x02110D4C, daCC_Create);

/* 021115A4: static initialisation (header statics; HD: the zeroed object is at 0x10462D28) */
static void __sinit_d_a_cc_cpp() {
    WWHD_FUNC(0x021115A4, void, (u32)0);
    const u32 P = 0x10462CF4, D = 0x101B3BC8;
    for (int i = 0; i < 4; i++) gabi::store<u32>(0x10462D28 + 4 * i, 0);
    __register_global_object(D);
    gabi::store<f32>(P, -3.1415927f);
    gabi::store<f32>(P + 4, 3.1415927f);
    gabi::call(0x028ED6F8, P + 8);
    __register_global_object(D + 0xC);
    gabi::call(0x028EAB2C, P + 9);
    __register_global_object(D + 0x18);
}
VERIFY(0x021115A4, __sinit_d_a_cc_cpp);

/* 02111638: out-of-line copy of fopAcM_seStart(actor, se, param) (HD inline) */
static void cc_fopAcM_seStart(fopAc_ac_c* a, u32 id, u32 param) {
    WWHD_FUNC(0x02111638, void, a, id, param);
    fopAcM_seStart(a, id, param);
}
VERIFY(0x02111638, cc_fopAcM_seStart);

/* 021116A4: sead::SafeString deleting destructor (this TU's vtable slot) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x021116A4, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x021116A4, SafeString_dt);

/* 02112DA0 */
static void action_tomaru(cc_class* i_this) {
    WWHD_FUNC(0x02112DA0, void, i_this);
    dComIfGp_get(); /* HD: unused player fetch */
    switch (i_this->m2F5) {
    case 100:
        if (dComIfGs_isSwitch(i_this->mDisableSpawnSwitchNo, dComIfGp_roomControl_getStayNo())) {
            i_this->m34E[6] = 0x28;
            i_this->m2F5 += 1;
        }
        break;
    case 101:
        if (i_this->m34E[6] == 0) {
            i_this->actor.actor_status |= 0x20; /* fopAcStts_SHOWMAP_e */
            i_this->mCyl.OnTgSPrmBit(1);         /* OnTgSetBit */
            i_this->mCurrAction = 0;
            i_this->m2F5 = 0;
        }
        break;
    }
}
VERIFY(0x02112DA0, action_tomaru);

/* 02112E74 */
static void action_tubo_search(cc_class* i_this) {
    WWHD_FUNC(0x02112E74, void, i_this);
    fopAc_ac_c* a_this = &i_this->actor;
    switch (i_this->m2F5) {
    case 0x6E:
        i_this->m30C = fpcM_ERROR_PROCESS_ID_e;
        fpcM_Search(0x0210DBE0 /* tsubo_search */, a_this);
        if (i_this->m30C == fpcM_ERROR_PROCESS_ID_e) {
            fopAcM_delete(a_this);
        } else {
            i_this->m2F5 += 1;
        }
        break;

    case 0x6F: {
        fopAc_ac_c* pfVar2 = fopAcM_SearchByID(i_this->m30C);

        if (REG_F(8, 0) != 0.0f) {
            i_this->mNoticeRange = 400.0f;
        }

        if (pfVar2 != nullptr) {
            a_this->current.pos.copy(pfVar2->current.pos);
            if (i_this->mNoticeRange == 0.0f) {
                return;
            }
            if (fopAcM_searchPlayerDistance(a_this) > i_this->mNoticeRange) {
                return;
            }
        }

        a_this->actor_status |= 0x20;
        i_this->m2F9 = 0;
        s16 ang = fopAcM_searchPlayerAngleY(a_this);
        a_this->current.angle.y = ang;
        a_this->shape_angle.y = ang;
        i_this->mCyl.OnAtSPrmBit(1);
        i_this->mCyl.mObjAt.mRPrm = 1; /* OnAtHitBit */
        i_this->mCyl.OnAtSPrmBit(8);   /* OnAtVsBitSet(cCcD_TgSPrm_IsOther_e) */
        i_this->mCyl.OffAtSPrmBit(4);  /* OffAtVsBitSet(cCcD_TgSPrm_IsPlayer_e) */

        anm_init(i_this, 0x1B /* BCK_HUSE2TACHI */, 0.0f, 0, 1.0f, -1);
        fopAcM_seStart_nn(a_this, JA_SE_CM_CC_LIE_TO_STAND, 0);
        denki_start(i_this);
        a_this->gravity = -3.0f;
        i_this->mBehaviorType = 0;
        i_this->m2F5 += 1;
        break;
    }

    case 0x70:
        i_this->mCyl.OffAtSPrmBit(8);
        i_this->mCyl.OnAtSPrmBit(4);
        gabi::store<u32>(gabi::ea(a_this) + 0x39C, 4); /* attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e */
        i_this->mCurrAction = 0;
        i_this->m2F5 = 0xB;
        break;
    }
}
VERIFY(0x02112E74, action_tubo_search);

/* 0211310C: empty function (called from daCC_Execute; debug hook) */
static void cc_empty_0211310C() {
    WWHD_FUNC(0x0211310C, void, (u32)0);
}
VERIFY(0x0211310C, cc_empty_0211310C);

static void cc_ObjAcch_dt(dBgS_ObjAcch* a) {
    gabi::store<u32>(gabi::ea(a) + 0x20, 0x1000C580);
    gabi::store<u32>(gabi::ea(a) + 0x14, 0x1000C590);
    gabi::call(0x024EFD9C, a, 0);
}

/* 02113110: cc_class deleting destructor (compiler-generated, HD virtual destructor) */
static void cc_class_dt(cc_class* p, s32 flags) {
    WWHD_FUNC(0x02113110, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x025E89F8, p->mDFC, 2); /* mDoExt_invisibleModel::~ */
        gabi::call(0x02515AE8, &p->mEnemyFire.mSph, 2);
        dCcD_Stts_dt(&p->mEnemyFire.mStts, 2);
        cc_ObjAcch_dt(&p->mEnemyIce.mBgAcch);
        gabi::call(0x02018034, gabi::at<u8>(gabi::ea(&p->mEnemyIce.mBgAcchCir) + 0x14), 2);
        dCcD_Cyl_dt(&p->mEnemyIce.mCyl, 2);
        dCcD_Stts_dt(&p->mEnemyIce.mStts, 2);
        dCcD_Cyl_dt(&p->mCyl, 2);
        dCcD_Stts_dt(&p->mStts, 2);
        cc_ObjAcch_dt(&p->mAcch);
        gabi::call(0x02018034, gabi::at<u8>(gabi::ea(&p->mAcchCir) + 0x14), 2);
        gabi::call(0x025D50BC, p, 0);
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x02113110, cc_class_dt);

/* 02113218: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x02113218, void, (u32)0);
}
VERIFY(0x02113218, SafeString_assureTerminationImpl);
