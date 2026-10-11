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

#include "gc_api/tu.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
using namespace CcResources;
const gc_tu_data gc_tu = {.safeString = 0x1000C548,
                          .safeStringAssure = 0x02113218,
                          .objAcch = {0x1000C570, 0x1000C590, 0x1000C580},
                          .objAcchDt = {0x1000C580, 0x1000C590},
                          .headerStatics = {0x10462CF4, 0x101B3BC8, 0x10462D28}};
static const u32 l_actorVtable = 0x1000C5A0;
static const u32 l_cylinderVtable = 0x1000C560;
static s32 target_info_count = 0;
static u32 target_info[10] = {};
static u32 l_currentModel = 0;
static Mtx34 l_currentMatrix;
static u8 DEMO_COME_START_FLAG = 0;
static u8 DEMO_RET_START_FLAG = 0;
static u8 DEMO_SHORT_CUT_FLAG = 0;
static s32 cc_atsp_kind[5] = {0, 0, 11, 0, 11};
static s32 cc_atatp_kind[5] = {1, 1, 2, 2, 2};
static s8 cc_HP_kind[5] = {2, 1, 2, 4, 1};
static s8 fire_j[10] = {0, 1, 2, 3, 4, 5, -1, -1, -1, -1};
static f32 fire_sc[10] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
static CcCylinderSource body_co_cyl = {
    0x0,
    0x2000,
    0x0,
    0x5,
    0xFFBDFEFF,
    0xB,
    0x75,
    0x6000000,
    0x0,
    0x0,
    0x6,
    0x0,
    0.0f,
    0.0f,
    0.0f,
    15.0f,
    0.0f
};

enum { fpcNm_BOMB_e = 0x126, fpcNm_Bomb2_e = 0x127, fpcNm_TSUBO_e = 0x1C5 };
enum { DSNAP_TYPE_CC = 0xAC };
enum { CC_JNT_CENTER_e = 0, CC_JNT_BODY03_e = 3 };
enum { JA_SE_OBJ_FALL_WATER_M = 0x6919, JA_SE_CM_CC_LIE_TO_STAND = 0x5860 };

static inline f32 anm_frame(void *anm) {
    return Cc_view<CcAnimation>(gabi::ea(anm))->mFrame;
}
static inline void anm_setFrame(void *anm, f32 frame) {
    Cc_view<CcAnimation>(gabi::ea(anm))->mFrame = frame;
}
static inline void brk_remove(J3DModel *model) {
    Cc_view<CcModelData>(Cc_view<CcModel>(gabi::ea(model))->mModelData)->mTevRegAnimation = 0;
}
static inline void btk_remove(J3DModel *model) {
    Cc_view<CcModelData>(Cc_view<CcModel>(gabi::ea(model))->mModelData)->mTexMtxAnimation = 0;
}
static inline J3DModelData *modelData(J3DModel *model) {
    return Cc_view<J3DModelData>(Cc_view<CcModel>(gabi::ea(model))->mModelData);
}
static inline BOOL Cc_checkLight(cXyz *pos) {
    return Cc_detectLight(dComIfGp_ea() + PLAY_DETECT, pos);
}
// HD: only the eye-position address is checked at these known-actor sound calls.
static inline void fopAcM_seStart_nn(fopAc_ac_c *a, u32 id, u32 param) {
    if (gabi::ea(&a->eyePos) != 0)
        mDoAud_seStart(id, &a->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
// HD: GHS leaves the preceding outgoing argument in SP+8 for the animation constructor.
static void *new_anm(u32 size, CcAnimationKind kind, u32 stale_sp8) {
    void *p = operator_new(size);
    struct Frame {
        be<u32> w[4];
    };
    gabi::Local<Frame> frame;
    frame->w[2] = stale_sp8;
    if (p == nullptr)
        return nullptr;
    return kind == CC_ANM_BRK ? Cc_brkCtor(p) : Cc_btkCtor(p);
}
// HD: matrix reads also mark the model's matrix block dirty.
static Mtx34 *cc_getAnmMtx(u32 model, s32 jointNo) {
    u32 block = Cc_view<CcModel>(model)->mMatrixBlock;
    auto *matrices = Cc_view<CcMatrixBlock>(block);
    matrices->mFlags = matrices->mFlags | 0x10;
    return Cc_view<Mtx34>(matrices->mMatrices + jointNo * sizeof(Mtx34));
}
/* 0210B874 */
static BOOL nodeCallBack(J3DNode *node, int calcTiming) {
    if (calcTiming == 0) {
        J3DJoint *joint = J3DNode_toJoint(node);
        u32 model = l_currentModel;
        cc_class *i_this = Cc_view<cc_class>(Cc_view<CcModel>(model)->mUserArea);
        s32 jntNo = Cc_view<CcJoint>(gabi::ea(joint))->mJointNo;

        if (i_this != nullptr) {
            if (jntNo == CC_JNT_CENTER_e) {
                PSMTXCopy(cc_getAnmMtx(model, jntNo), calc_mtx());
                cMtx_ZrotM(calc_mtx(), i_this->m3BA);
                Mtx34 *dst = cc_getAnmMtx(model, jntNo);
                mtx_copy(dst, calc_mtx());
                PSMTXCopy(calc_mtx(), &l_currentMatrix);
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
                Mtx34 *dst = cc_getAnmMtx(model, jntNo);
                mtx_copy(dst, calc_mtx());
                PSMTXCopy(calc_mtx(), &l_currentMatrix);
            }
        }
    }
    return TRUE;
}

/* 0210BB58 */
static void draw_SUB(cc_class *i_this) {
    if (i_this->mBehaviorType == CC_BEHAVIOR_DEKU_TREE && i_this->m2FB != 3) {
        i_this->m2B4->calc();
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->actor.current.pos,
                         &i_this->actor.tevStr);
        Cc_enemyFire(&i_this->mEnemyFire);
        return;
    }

    J3DModel *pJVar1;
    if (i_this->m2F5 != CC_MODE_FLATTENED) {
        if (i_this->m301 == 0) {
            pJVar1 = i_this->m2B4->getModel();
        } else {
            pJVar1 = i_this->m2D8->getModel();
        }
    } else {
        pJVar1 = i_this->m2BC->getModel();
    }

    Mtx34 *now = mDoMtx_stack_c::get();
    PSMTXTrans(now, i_this->actor.current.pos.x, i_this->actor.current.pos.y - i_this->m328,
               i_this->actor.current.pos.z);
    mDoMtx_XrotM(now, i_this->m316.x);
    mDoMtx_ZrotM(now, i_this->m316.z);
    mDoMtx_YrotM(now, i_this->actor.shape_angle.y);
    mDoMtx_XrotM(now, i_this->actor.shape_angle.x);
    mDoMtx_ZrotM(now, i_this->actor.shape_angle.z);

    f32 sx, sy, sz;
    if (i_this->mCurrAction != CC_ACTION_CLIMB && i_this->m2F5 != CC_MODE_FLATTENED && i_this->m301 == 0 &&
        i_this->mCurrAction != CC_ACTION_STONE && i_this->m34E[4] == 0) {
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
        Cc_enemyFire(&i_this->mEnemyFire);
    }

    if (i_this->m2FE != 0) {
        J3DModel *model = i_this->m2C4->getModel();
        J3DModel_setBaseTRMtx(model, now);
    }

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->actor.current.pos, &i_this->actor.tevStr);
}

/* 0210BFEC */
static BOOL daCC_Draw(cc_class *i_this) {
    fopAc_ac_c *a_this = &i_this->actor;
    J3DModel *pJVar3 = i_this->m2B4->getModel();

    if (i_this->mBehaviorType == CC_BEHAVIOR_POT) {
        return TRUE;
    }

    dSnap_RegistFig(DSNAP_TYPE_CC, a_this, 1.0f, 1.0f, 1.0f);

    if (i_this->m2F5 != CC_MODE_FLATTENED) {
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
        Cc_iceUpdateDL(i_this->m2B4, -1, i_this->mDFC);
        return TRUE;
    }

    dComIfGp_get(); // HD: the play object is fetched, its result unused (GameCube: shadow)
    if (i_this->m2F5 != CC_MODE_FLATTENED) {
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

    // HD: no simple shadow

    if (i_this->m2F5 != CC_MODE_FLATTENED) {
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

/* 0210C364 */
static void cc_eff_set(cc_class *i_this, unsigned char arg1) {
    u16 userID = CC_PARTICLE_HEAD;
    /* The colour table is a stack local indexed by mColorType without a bounds check. To keep
     * out-of-range indices (only reachable with a clobbered mColorType) identical, the candidate
     * reproduces the original's frame above the table: table at entry SP - 0x24, then the saved
     * r28..r31, then the caller's frame with the LR save word. */
    struct Frame {
        u8 _00[0xC];
        GXColor c[5];     /* entry SP - 0x24 */
        be<u32> saved[4]; /* r28..r31 */
    };
    *Cc_view<be<u32>>(gabi::cpu->r[1] + 4) = gabi::cpu->lr; /* the prologue's LR save (entry SP + 4) */
    gabi::Local<Frame> fr;
    for (int i = 0; i < 4; i++)
        fr->saved[i] = gabi::cpu->r[28 + i];
    gabi::Local<cXyz> sp1C;
    sp1C->x = i_this->m470.x;
    sp1C->y = i_this->m470.y;
    sp1C->z = i_this->m470.z;

    // HD: the colour table is a local, its 4th entry taken from the particle manager
    u32 pa = gabi::ea(dComIfGp_getParticle());
    if (pa == 0) {
        return;
    }
    GXColor *c = fr->c;
    c[0].r = 0x19;
    c[0].g = 0xBE;
    c[0].b = 0x05;
    c[1].r = 0xC8;
    c[1].g = 0x14;
    c[1].b = 0x00;
    c[2].r = 0x00;
    c[2].g = 0x64;
    c[2].b = 0xFF;
    c[3].r = Cc_view<CcParticle>(pa)->mToonColor.r;
    c[3].g = Cc_view<CcParticle>(pa)->mToonColor.g;
    c[3].b = Cc_view<CcParticle>(pa)->mToonColor.b;
    c[4].r = 0xFF;
    c[4].g = 0xFF;
    c[4].b = 0x00;

    if (arg1 != 0) {
        userID = CC_PARTICLE_FOOT;
        sp1C->copy(i_this->actor.current.pos);
    }

    JPABaseEmitter *pJVar6 = dComIfGp_particle_set(userID, sp1C, &i_this->actor.shape_angle);
    if (pJVar6 != nullptr) {
        const GXColor &col = c[(u8)i_this->mColorType];
        f32 tmp = (f32)i_this->actor.tevStr.mColorK0.r / 255.0f;
        u8 r = (u8)gabi::ftoi((f32)(u8)col.r * tmp);
        tmp = (f32)i_this->actor.tevStr.mColorK0.g / 255.0f;
        u8 g = (u8)gabi::ftoi((f32)(u8)col.g * tmp);
        tmp = (f32)i_this->actor.tevStr.mColorK0.b / 255.0f;
        u8 b = (u8)gabi::ftoi((f32)(u8)col.b * tmp);
        /* setGlobalPrmColor(r, g, b) */
        u32 e = gabi::ea(pJVar6);
        Cc_view<CcEmitter>(e)->mPrmColor.r = r;
        Cc_view<CcEmitter>(e)->mPrmColor.g = g;
        Cc_view<CcEmitter>(e)->mPrmColor.b = b;
        // HD: setGlobalEnvColor(K0.r, K0.g, K0.b)
        Cc_view<CcEmitter>(e)->mEnvColor.r = i_this->actor.tevStr.mColorK0.r;
        Cc_view<CcEmitter>(e)->mEnvColor.g = i_this->actor.tevStr.mColorK0.g;
        Cc_view<CcEmitter>(e)->mEnvColor.b = i_this->actor.tevStr.mColorK0.b;
    }
}

/* 0210C5C8 */
static void anm_init(cc_class *i_this, int transformResIdx, float morf, unsigned char loopMode, float speed,
                     int soundResIdx) {
    i_this->m320 = transformResIdx;
    if (soundResIdx >= 0) {
        J3DAnmTransform *anm =
            (J3DAnmTransform *)dComIfG_getObjectRes("Cc", transformResIdx, gc_tu.safeString);
        void *bas = dComIfG_getObjectRes("Cc", soundResIdx, gc_tu.safeString);
        i_this->m2B4->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, bas);
    } else {
        J3DAnmTransform *anm =
            (J3DAnmTransform *)dComIfG_getObjectRes("Cc", transformResIdx, gc_tu.safeString);
        i_this->m2B4->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, nullptr);
    }
}

/* 0210C6F4 */
static void damage_mode_move(cc_class *i_this) {
    dComIfGp_get(); // HD: unused player fetch
    i_this->m2F6 = 2;
    if (i_this->m2F5 != CC_MODE_SPLIT_START && i_this->m2F5 != CC_MODE_SPLIT_WAIT) {
        dComIfGp_particle_set(0x27B /* ID_IT_JN_PIYOHIT00 */,
                              &i_this->actor.attention_info.position /* attention_info.position */);
        i_this->mCurrAction = CC_ACTION_DAMAGE;
        i_this->m2F5 = CC_MODE_DAMAGE_START;
    }
}

/* 0210C788 */
static void *s_b_sub(void *arg0, void *) {
    if (fopAc_IsActor(arg0) && arg0 != nullptr) {
        bool bVar2 = false;
        if (fpcM_GetName(arg0) == fpcNm_BOMB_e) {
            if (Cc_bombCheckState(arg0, 0 /* daBomb_c::STATE_0 */)) {
                bVar2 = true;
            }
        } else if (fpcM_GetName(arg0) == fpcNm_Bomb2_e) {
            if (Cc_bomb2CheckExplode(arg0)) {
                bVar2 = true;
            }
        }

        if (bVar2 && target_info_count < 10) {
            s32 n = target_info_count;
            target_info_count = n + 1;
            target_info[n] = gabi::ea(arg0);
        }
    }
    return nullptr;
}

/* 0210C828 */
static void naraku_check(cc_class *i_this) {
    fopAc_ac_c *a_this = &i_this->actor;
    void *gnd = static_cast<cBgS_PolyInfo *>(&i_this->mAcch.m_gnd); /* m_gnd (its cBgS_PolyInfo) */
    if (i_this->mAcch.GetGroundH() != -1000000000.0f && Cc_checkPolySafe(dComIfG_Bgsp(), gnd) &&
        Cc_getGroundCode(dComIfG_Bgsp(), gnd) == 4) {
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
        if (i_this->mCurrAction != CC_ACTION_SWIM) {
            gabi::Local<cXyz> sp24;
            sp24->x = a_this->current.pos.x;
            sp24->y = a_this->current.pos.y;
            sp24->z = a_this->current.pos.z;
            sp24->y = i_this->mAcch.m_wtr.GetHeight(); /* m_wtr.GetHeight() */
            if (!i_this->m300) {
                fopKyM_createWpillar(sp24, 1.0f, 0.4f, 0);
                fopAcM_seStart_nn(a_this, JA_SE_OBJ_FALL_WATER_M, 0);
                i_this->m300 = 1;
                gabi::Local<cXyz> sp18;
                sp18->set(0.5f, 0.5f, 0.5f);
                Cc_rippleRemove(i_this->m390);
                /* dComIfGp_particle_setShipTail(ID_AK_JN_HAMON00, &pos, NULL, &sp18, 0xFF, &m390) */
                dPa_control_set(dComIfGp_getParticle(), 5, 0x33, &a_this->current.pos, nullptr, sp18, 0xFF,
                                (dPa_levelEcallBack *)i_this->m390, -1, nullptr, nullptr, nullptr);
                Cc_view<CcRipple>(gabi::ea(i_this->m390))->mRate = 0.0f; /* m390.setRate(0.0f) */
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

                i_this->mCurrAction = CC_ACTION_SWIM;
                i_this->m2F5 = CC_MODE_SWIM_START;

                if (a_this->health <= 0 && i_this->mCurrAction != CC_ACTION_DEAD) {
                    i_this->mCurrAction = CC_ACTION_DEAD;
                    i_this->m2F5 = CC_MODE_DEAD_START;
                }
            }
        }
    } else if (i_this->m300) {
        i_this->m2F8 = 0;
        i_this->m300 = 0;
        Cc_rippleRemove(i_this->m390);
    }
}

/* 0210CBC0 */
static void denki_start(cc_class *i_this) {
    if (i_this->mColorType != CC_BLUE && i_this->mColorType != CC_YELLOW) {
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

/* 0210CC00 */
static void denki_end(cc_class *i_this) {
    if (i_this->mColorType == CC_BLUE || i_this->mColorType == CC_YELLOW) {
        // HD: dPa_followEcallBack::end (GameCube: remove())
        dPa_followEcallBack_end(&i_this->m368);
        dPa_followEcallBack_end(&i_this->m37C);
        i_this->mCyl.SetTgSpl(0);
        i_this->m2F7 = 0;
    }
}

/* 0210CC58 */
static BOOL shock_damage_check(cc_class *i_this) {
    fopAc_ac_c *player = dComIfGp_getPlayer(0);
    target_info_count = 0;
    for (s32 i = 0; i < 10; i++) {
        target_info[i] = 0;
    }

    fpcM_Search(gabi::guest_fn((const void *)s_b_sub), &i_this->actor);

    if (target_info_count != 0) {
        for (s32 i = 0; i < target_info_count; i++) {
            fopAc_ac_c *t = Cc_view<fopAc_ac_c>(target_info[i]);
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
    if (Cc_view<CcPlayer>(gabi::ea(player))->mNoResetFlg2 & 0x20000) {
        f32 x = Cc_view<CcPlayer>(gabi::ea(player))->mSwordTopPos.x - i_this->actor.current.pos.x;
        f32 z = Cc_view<CcPlayer>(gabi::ea(player))->mSwordTopPos.z - i_this->actor.current.pos.z;
        if (std_sqrtf(gabi::fmadds(x, x, z * z)) < 1000.0f) {
            damage_mode_move(i_this);
            return TRUE;
        }
    }
    return FALSE;
}

/* 0210CE2C */
static void black_light_check(cc_class *i_this) {
    if (i_this->mColorType == CC_BLACK) {
        if (i_this->mCurrAction != CC_ACTION_STONE && Cc_checkLight(&i_this->actor.current.pos)) {
            i_this->mStts.SetWeight(0xFE);
            i_this->mCurrAction = CC_ACTION_STONE;
            i_this->m2F5 = CC_MODE_STONE_START;
        }
    }
}

/* 0210DA18 */
static BOOL search_angle_set(cc_class *i_this) {
    if (i_this->mBehaviorType != CC_BEHAVIOR_FIXED) {
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
                   (s16)cLib_distanceAngleS(i_this->actor.shape_angle.y, i_this->actor.current.angle.y) <
                       0x100) {
            f32 y = (f32)i_this->actor.current.angle.y;
            i_this->actor.current.angle.y = (s16)gabi::ftoi(y + cM_rndFX(16384.0f));
        }
    }
    return FALSE;
}

/* 0210DBE0 */
static void *tsubo_search(void *arg1, void *arg2) {
    fopAc_ac_c *actor1 = (fopAc_ac_c *)arg1;
    cc_class *i_this = (cc_class *)arg2;
    if (fopAc_IsActor(actor1) && actor1 != nullptr && fpcM_GetName(actor1) == fpcNm_TSUBO_e &&
        actor1->current.pos.x == i_this->actor.current.pos.x &&
        std::fabs((f32)(actor1->current.pos.y - i_this->actor.current.pos.y)) < 10.0f &&
        actor1->current.pos.z == i_this->actor.current.pos.z) {
        i_this->m30C = fopAcM_GetID(actor1);
    }
    return nullptr;
}

/* 0210DC8C */
static void BG_check(cc_class *i_this) {
    if (i_this->m2F9 == 0) {
        i_this->mAcchCir.SetWall(20.0f, 30.0f);
        i_this->mAcch.CrrPos(dComIfG_Bgsp());
        if (i_this->m2F8 != 0 && i_this->mAcch.ChkWaterIn()) {
            i_this->actor.current.pos.y = i_this->mAcch.m_wtr.GetHeight() - 20.0f;
        }
        fopAcM_getGroundAngle(&i_this->actor, &i_this->m316);
        naraku_check(i_this);
    }
}

/* 021104AC */
static BOOL daCC_IsDelete(cc_class *) {
    return TRUE;
}

/* 021104B4 */
static BOOL daCC_Delete(cc_class *i_this) {
    i_this->m368.remove();
    i_this->m37C.remove();
    Cc_rippleRemove(i_this->m390);
    dComIfG_resDelete(&i_this->mPhase, "Cc");
    Cc_enemyFireRemove(&i_this->mEnemyFire);
    return TRUE;
}

static mDoExt_McaMorf *cc_new_morf(J3DModelData *data, J3DAnmTransform *anm, f32 speed, s32 start) {
    return mDoExt_McaMorf::create(nullptr, data, nullptr, nullptr, anm, J3DFrameCtrl::EMode_NONE, speed,
                                  start, -1, 1, nullptr, 0, 0x11020203);
}

/* 02110528 */
static BOOL useHeapInit(fopAc_ac_c *a_this) {
    cc_class *i_this = (cc_class *)a_this;

    s32 fileIndex = dRes_INDEX_CC_BCK_START_e;
    if (i_this->mBehaviorType == CC_BEHAVIOR_CEILING) {
        fileIndex = 0x21; /* BCK_TSTART01 */
    } else if (i_this->mBehaviorType == CC_BEHAVIOR_DEKU_TREE) {
        fileIndex = 0x1B; /* BCK_HUSE2TACHI */
    }

    J3DModelData *data = (J3DModelData *)dComIfG_getObjectRes("Cc", dRes_INDEX_CC_BMD_CC_e, gc_tu.safeString);
    J3DAnmTransform *anm = (J3DAnmTransform *)dComIfG_getObjectRes("Cc", fileIndex, gc_tu.safeString);
    i_this->m2B4 = cc_new_morf(data, anm, 0.0f, 0);
    if (i_this->m2B4 == nullptr || i_this->m2B4->getModel() == nullptr) {
        return FALSE;
    }

    Cc_view<CcModel>(gabi::ea(i_this->m2B4->getModel()))->mUserArea = gabi::ea(i_this); /* setUserArea */

    for (u16 i = 0;
         i < Cc_view<CcJointTree>(gabi::ea(Cc_jointTree(modelData(i_this->m2B4->getModel()))))->mJointCount;
         i++) {
        /* getJointNodePointer(i)->setCallBack(nodeCallBack) (HD: 0x1C-byte joints, element 0 past the end) */
        u32 md = gabi::ea(modelData(i_this->m2B4->getModel()));
        u32 n = Cc_view<CcModelData>(md)->mJointCount;
        u32 p = Cc_view<CcModelData>(md)->mJoints;
        if (i < n)
            p += i * 0x1C;
        Cc_view<CcJoint>(p)->mCallback = gabi::guest_fn((const void *)nodeCallBack);
    }

    J3DModel *model = i_this->m2B4->getModel();

    i_this->m2B8 = (mDoExt_brkAnm *)new_anm(0x78, CC_ANM_BRK, 1);
    if (i_this->m2B8 == nullptr) {
        return FALSE;
    }
    void *key = dComIfG_getObjectRes("Cc", dRes_INDEX_CC_BRK_CC_e, gc_tu.safeString);
    if (!Cc_brkInit(i_this->m2B8, modelData(model), key, 1, J3DFrameCtrl::EMode_NONE, 0, -1, false, 0,
                    1.0f)) {
        return FALSE;
    }

    data = (J3DModelData *)dComIfG_getObjectRes("Cc", dRes_INDEX_CC_BDL_CC_IWA_e, gc_tu.safeString);
    i_this->m2C4 = cc_new_morf(data, nullptr, 0.0f, 0);
    if (i_this->m2C4 == nullptr || i_this->m2C4->getModel() == nullptr) {
        return FALSE;
    }

    model = i_this->m2C4->getModel();
    i_this->m2C8 = (mDoExt_brkAnm *)new_anm(0x78, CC_ANM_BRK, 1);
    if (i_this->m2C8 == nullptr) {
        return FALSE;
    }
    key = dComIfG_getObjectRes("Cc", dRes_INDEX_CC_BRK_CC_IWA01_e, gc_tu.safeString);
    if (!Cc_brkInit(i_this->m2C8, modelData(model), key, 1, J3DFrameCtrl::EMode_NONE, 0, -1, false, 0,
                    1.0f)) {
        return FALSE;
    }

    i_this->m2CC = (mDoExt_brkAnm *)new_anm(0x78, CC_ANM_BRK, 0);
    if (i_this->m2CC == nullptr) {
        return FALSE;
    }
    key = dComIfG_getObjectRes("Cc", dRes_INDEX_CC_BRK_CC_IWA02_e, gc_tu.safeString);
    if (!Cc_brkInit(i_this->m2CC, modelData(model), key, 1, J3DFrameCtrl::EMode_NONE, 0, -1, false, 0,
                    1.0f)) {
        return FALSE;
    }

    i_this->m2D0 = (mDoExt_btkAnm *)new_anm(0x74, CC_ANM_BTK, 0);
    if (i_this->m2D0 == nullptr) {
        return FALSE;
    }
    key = dComIfG_getObjectRes("Cc", dRes_INDEX_CC_BTK_CC_IWA01_e, gc_tu.safeString);
    if (!Cc_btkInit(i_this->m2D0, modelData(model), key, 1, J3DFrameCtrl::EMode_NONE, 0, -1, false, 0,
                    1.0f)) {
        return FALSE;
    }

    i_this->m2D4 = (mDoExt_btkAnm *)new_anm(0x74, CC_ANM_BTK, 0);
    if (i_this->m2D4 == nullptr) {
        return FALSE;
    }
    key = dComIfG_getObjectRes("Cc", dRes_INDEX_CC_BTK_CC_IWA02_e, gc_tu.safeString);
    if (!Cc_btkInit(i_this->m2D4, modelData(model), key, 1, J3DFrameCtrl::EMode_NONE, 0, -1, false, 0,
                    1.0f)) {
        return FALSE;
    }

    data = (J3DModelData *)dComIfG_getObjectRes("Cc", dRes_INDEX_CC_BDL_CC_BETA_e, gc_tu.safeString);
    i_this->m2BC = cc_new_morf(data, nullptr, 0.0f, 0);
    if (i_this->m2BC == nullptr || i_this->m2BC->getModel() == nullptr) {
        return FALSE;
    }

    model = i_this->m2BC->getModel();
    i_this->m2C0 = (mDoExt_brkAnm *)new_anm(0x78, CC_ANM_BRK, 1);
    if (i_this->m2C0 == nullptr) {
        return FALSE;
    }
    key = dComIfG_getObjectRes("Cc", dRes_INDEX_CC_BRK_CC_BETA_e, gc_tu.safeString);
    if (!Cc_brkInit(i_this->m2C0, modelData(model), key, 1, J3DFrameCtrl::EMode_NONE, 0, -1, false, 0,
                    1.0f)) {
        return FALSE;
    }

    data = (J3DModelData *)dComIfG_getObjectRes("Cc", dRes_INDEX_CC_BDL_CC_PTCL_e, gc_tu.safeString);
    anm = (J3DAnmTransform *)dComIfG_getObjectRes("Cc", dRes_INDEX_CC_BCK_CC_PTCL_e, gc_tu.safeString);
    i_this->m2D8 = cc_new_morf(data, anm, 1.0f, 1);
    if (i_this->m2D8 == nullptr || i_this->m2D8->getModel() == nullptr) {
        return FALSE;
    }

    if (!Cc_invisibleCreate(i_this->mDFC, i_this->m2B4->getModel())) { /* mDFC.create(model) */
        return FALSE;
    }
    return TRUE;
}

/* 02110B28 */
static enemyfire_l *enemyfire_ct(enemyfire_l *p) {
    if (p == nullptr) {
        p = (enemyfire_l *)operator_new(0x22C);
        if (p == nullptr)
            return p;
    }
    if (gabi::ea(&p->mDirection) == 0)
        operator_new(0xC);
    dCcD_Stts_ct(&p->mStts);
    Cc_sphereCtor(&p->mSph);
    p->m228 = 1.0f;
    return p;
}

static void cc_ObjAcch_ct(dBgS_ObjAcch *a) {
    Cc_acchCtor(a);
    Cc_view<CcAcchVtables>(gabi::ea(a))->mCheckVtable = gc_tu.objAcch.v10;
    Cc_view<CcAcchVtables>(gabi::ea(a))->mPassVtable = gc_tu.objAcch.v14;
    Cc_view<CcAcchVtables>(gabi::ea(a))->mObjectPass = 1;
    Cc_view<CcAcchVtables>(gabi::ea(a))->mObjectVtable = gc_tu.objAcch.v20;
}

/* 02110BB4 */
static cc_class *cc_class_ct(cc_class *p) {
    if (p == nullptr) {
        p = (cc_class *)operator_new(0xF10);
        if (p == nullptr)
            return p;
    }
    fopAc_ac_c_ct(&p->actor);
    p->actor.__vtbl = l_actorVtable;
    dPa_followEcallBack_ct(&p->m368, 0, 0);
    dPa_followEcallBack_ct(&p->m37C, 0, 0);
    Cc_rippleCtor(p->m390); /* dPa_rippleEcallBack::dPa_rippleEcallBack */
    Cc_acchCirCtor(&p->mAcchCir);
    cc_ObjAcch_ct(&p->mAcch);
    dCcD_Stts_ct(&p->mStts);
    dCcD_Cyl_ct(&p->mCyl, l_cylinderVtable);
    dCcD_Stts_ct(&p->mEnemyIce.mStts);
    dCcD_Cyl_ct(&p->mEnemyIce.mCyl, l_cylinderVtable);
    Cc_acchCirCtor(&p->mEnemyIce.mBgAcchCir);
    cc_ObjAcch_ct(&p->mEnemyIce.mBgAcch);
    enemyfire_ct(&p->mEnemyFire);
    Cc_invisibleCtor(p->mDFC); /* mDoExt_invisibleModel::mDoExt_invisibleModel */
    return p;
}

/* 02110D4C */
static cPhs_State daCC_Create(fopAc_ac_c *a_this) {
    cc_class *i_this = (cc_class *)a_this;

    /* fopAcM_ct(a_this, cc_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            cc_class_ct(i_this);
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    cPhs_State res = dComIfG_resLoad(&i_this->mPhase, "Cc");
    if (res == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(a_this, gabi::guest_fn((const void *)useHeapInit), 0x47A0)) {
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
            i_this->mBehaviorType = CC_BEHAVIOR_FIXED;
        } else if (i_this->mBehaviorType == 0xFF) {
            i_this->mBehaviorType = CC_BEHAVIOR_GROUND;
        }
        if (i_this->mColorType == 0xFF) {
            i_this->mColorType = CC_GREEN;
        }
        if (i_this->mNoticeRangeByte == 0xFF) {
            i_this->mNoticeRangeByte = 0;
        }

        if (i_this->mBehaviorType != CC_BEHAVIOR_POT) {
            if (i_this->mNoticeRangeByte == 0) {
                i_this->mNoticeRange = 1000.0f;
                if (i_this->mBehaviorType == CC_BEHAVIOR_CEILING) {
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
                    i_this->mColorType = CC_RED;
                }
            }
        } else {
            i_this->mNoticeRange = (f32)(u8)i_this->mNoticeRangeByte * 10.0f;
        }

        i_this->m3BC.copy(a_this->current.pos);
        a_this->actor_status &= ~u32(fopAcStts_SHOWMAP_e); /* fopAcM_OffStatus(SHOWMAP) */
        a_this->stealItemLeft = 1;

        switch (i_this->mColorType) {
        case CC_GREEN:
            a_this->gbaName = 0x16;
            a_this->itemTableIdx = Cc_getNameIndex(dComIfGp_ea() + PLAY_NAMETBL, "c_green", 0);
            break;
        case CC_RED:
            a_this->gbaName = 0x15;
            a_this->itemTableIdx = Cc_getNameIndex(dComIfGp_ea() + PLAY_NAMETBL, "c_red", 0);
            break;
        case CC_BLUE:
            a_this->gbaName = 0x1E;
            a_this->itemTableIdx = Cc_getNameIndex(dComIfGp_ea() + PLAY_NAMETBL, "c_blue", 0);
            break;
        case CC_BLACK:
            a_this->gbaName = 0x10;
            a_this->stealItemLeft = 3;
            a_this->itemTableIdx = Cc_getNameIndex(dComIfGp_ea() + PLAY_NAMETBL, "c_black", 0);
            a_this->actor_status |= fopAcStts_UNK8000000_e;
            break;
        case CC_YELLOW:
            a_this->gbaName = 10;
            a_this->itemTableIdx = Cc_getNameIndex(dComIfGp_ea() + PLAY_NAMETBL, "c_kiiro", 0);
            break;
        }

        a_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->m2B4->getModel()));
        a_this->attention_info.flags = 0; /* attention_info.flags */

        i_this->mAcch.Set(&a_this->current.pos, &a_this->old.pos, a_this, 1, &i_this->mAcchCir,
                          &a_this->speed, nullptr, nullptr);

        if (i_this->mBehaviorType == CC_BEHAVIOR_CEILING) {
            a_this->shape_angle.x = -0x8000;
        }

        i_this->mStts.Init(0x32, 4, a_this);

        if (i_this->m304) {
            /* SetTgGrp(IsOther | IsPlayer | IsEnemy | Set) */
            i_this->mCyl.mObjTg.mSPrm = (i_this->mCyl.mObjTg.mSPrm & ~0xEu) | 0xF;
        }

        i_this->mCyl.Set(reinterpret_cast<dCcD_SrcCyl *>(&body_co_cyl)); /* body_co_cyl */
        i_this->mCyl.SetStts(&i_this->mStts);
        u32 kind = i_this->mColorType;
        if (kind >=
            5) { // HD: JUT_ASSERT(a_arg1 < sizeof(cc_HP_kind) / sizeof(cc_HP_kind[0])), then index 0
            JUT_ASSERT_fail("d_a_cc.cpp", 0xFB6, "a_arg1 < sizeof(cc_HP_kind) / sizeof(cc_HP_kind[0])");
            kind = 0;
        }
        i_this->mCyl.SetAtSpl((u8)cc_atsp_kind[kind]);  /* cc_atsp_kind */
        i_this->mCyl.SetAtAtp((u8)cc_atatp_kind[kind]); /* cc_atatp_kind */
        a_this->max_health = cc_HP_kind[kind];          /* cc_HP_kind */
        a_this->health = cc_HP_kind[kind];

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
            i_this->mEnemyFire.mFlameJntIdxs[i] = fire_j[i];   /* fire_j */
            i_this->mEnemyFire.mParticleScale[i] = fire_sc[i]; /* fire_sc */
        }

        if (i_this->mBehaviorType == CC_BEHAVIOR_DEKU_TREE) {
            a_this->gravity = 0.0f;
            DEMO_COME_START_FLAG = 0; /* DEMO_COME_START_FLAG etc. (HD: in .data) */
            DEMO_RET_START_FLAG = 0;
            DEMO_SHORT_CUT_FLAG = 0;
            i_this->mCyl.OnTgSPrmBit(1); /* OnTgSetBit */
            i_this->mCyl.OnCoSPrmBit(1); /* OnCoSetBit */
            i_this->mDisableSpawnSwitchNo = 0xFF;
            i_this->mCurrAction = CC_ACTION_CLIMB;
            i_this->m2F5 = CC_MODE_CLIMB_START;
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

        if (i_this->mBehaviorType != CC_BEHAVIOR_FIXED) {
            if (i_this->mDisableSpawnSwitchNo != 0xFF) {
                if (i_this->mBehaviorType != CC_BEHAVIOR_CEILING) {
                    i_this->m2F9 = 0;
                    a_this->gravity = -3.0f;
                }
                i_this->mCyl.OffTgSPrmBit(1); /* OffTgSetBit */
                i_this->mCyl.OffCoSPrmBit(1); /* OffCoSetBit */
                i_this->mCyl.ClrTgHit();
                i_this->mCurrAction = CC_ACTION_WAIT_SWITCH;
                i_this->m2F5 = CC_MODE_WAIT_SWITCH;
            }
        } else {
            a_this->stealItemBitNo = i_this->mDisableSpawnSwitchNo;
            i_this->mStts.SetWeight(0xFF);
            a_this->actor_status &= ~u32(fopAcStts_UNK80000_e);
        }

        if (i_this->mBehaviorType == CC_BEHAVIOR_POT) {
            i_this->mCurrAction = CC_ACTION_POT;
            i_this->m2F5 = CC_MODE_POT_SEARCH;
        }

        BG_check(i_this);
        draw_SUB(i_this);

        fopEn_enemy_c &en = i_this->actor;
        en.mBtHeight = REG_F(8, 4) + 150.0f; /* initBt */
        en.mBtBodyR = REG_F(8, 5) + 100.0f;
        en.mBtAttackType = 1; /* setBtAttackData(0, 10, REG8_F(6) + 500, OPENING_JUMP_PARRY) */
        en.mBtStartFrame = 0.0f;
        en.mBtEndFrame = 10.0f;
        en.mBtMaxDis = REG_F(8, 6) + 500.0f;
        en.mBtNowFrame = 1000.0f; /* setBtNowFrame */
    }
    return res;
}

// HD: compiler-generated header statics and template copies.
/* 021115A4 */
static void __sinit_d_a_cc_cpp() {
    sinit_header_statics_z(gc_tu.headerStatics[0], gc_tu.headerStatics[1], gc_tu.headerStatics[2]);
}

/* 02111638 */
static void cc_fopAcM_seStart(fopAc_ac_c *a, u32 id, u32 param) {
    fopAcM_seStart(a, id, param);
}

/* 021116A4 */
static void SafeString_dt(void *p, s32 flags) {
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

/* 02112DA0 */
static void action_tomaru(cc_class *i_this) {
    dComIfGp_get(); // HD: unused player fetch
    switch (i_this->m2F5) {
    case CC_MODE_WAIT_SWITCH:
        if (dComIfGs_isSwitch(i_this->mDisableSpawnSwitchNo, dComIfGp_roomControl_getStayNo())) {
            i_this->m34E[6] = 0x28;
            i_this->m2F5 += 1;
        }
        break;
    case CC_MODE_WAIT_SWITCH_DELAY:
        if (i_this->m34E[6] == 0) {
            i_this->actor.actor_status |= fopAcStts_SHOWMAP_e; /* fopAcStts_SHOWMAP_e */
            i_this->mCyl.OnTgSPrmBit(1);                       /* OnTgSetBit */
            i_this->mCurrAction = CC_ACTION_NORMAL;
            i_this->m2F5 = CC_MODE_WAIT;
        }
        break;
    }
}

/* 02112E74 */
static void action_tubo_search(cc_class *i_this) {
    fopAc_ac_c *a_this = &i_this->actor;
    switch (i_this->m2F5) {
    case CC_MODE_POT_SEARCH:
        i_this->m30C = fpcM_ERROR_PROCESS_ID_e;
        fpcM_Search(gabi::guest_fn((const void *)tsubo_search), a_this);
        if (i_this->m30C == fpcM_ERROR_PROCESS_ID_e) {
            fopAcM_delete(a_this);
        } else {
            i_this->m2F5 += 1;
        }
        break;

    case CC_MODE_POT_WAIT: {
        fopAc_ac_c *pfVar2 = fopAcM_SearchByID(i_this->m30C);

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

        a_this->actor_status |= fopAcStts_SHOWMAP_e;
        i_this->m2F9 = 0;
        s16 ang = fopAcM_searchPlayerAngleY(a_this);
        a_this->current.angle.y = ang;
        a_this->shape_angle.y = ang;
        i_this->mCyl.OnAtSPrmBit(1);
        i_this->mCyl.mObjAt.mRPrm = 1; /* OnAtHitBit */
        i_this->mCyl.OnAtSPrmBit(8);   /* OnAtVsBitSet(cCcD_TgSPrm_IsOther_e) */
        i_this->mCyl.OffAtSPrmBit(4);  /* OffAtVsBitSet(cCcD_TgSPrm_IsPlayer_e) */

        anm_init(i_this, dRes_INDEX_CC_BCK_HUSE2TACHI_e, 0.0f, 0, 1.0f, -1);
        fopAcM_seStart_nn(a_this, JA_SE_CM_CC_LIE_TO_STAND, 0);
        denki_start(i_this);
        a_this->gravity = -3.0f;
        i_this->mBehaviorType = CC_BEHAVIOR_GROUND;
        i_this->m2F5 += 1;
        break;
    }

    case CC_MODE_POT_EMERGE:
        i_this->mCyl.OffAtSPrmBit(8);
        i_this->mCyl.OnAtSPrmBit(4);
        a_this->attention_info.flags =
            fopAc_Attn_LOCKON_BATTLE_e; /* attention_info.flags = fopAc_Attn_LOCKON_BATTLE_e */
        i_this->mCurrAction = CC_ACTION_NORMAL;
        i_this->m2F5 = CC_MODE_STAND_END;
        break;
    }
}

/* 0211310C */
static void cc_debugHook() {}

static void cc_ObjAcch_dt(dBgS_ObjAcch *a) {
    Cc_view<CcAcchVtables>(gabi::ea(a))->mObjectVtable = gc_tu.objAcch.v20;
    Cc_view<CcAcchVtables>(gabi::ea(a))->mPassVtable = gc_tu.objAcch.v14;
    Cc_acchDtor(a, 0);
}

/* 02113110 */
static void cc_class_dt(cc_class *p, s32 flags) {
    if (p != nullptr) {
        Cc_invisibleDtor(p->mDFC, 2); /* mDoExt_invisibleModel::~ */
        Cc_sphereDtor(&p->mEnemyFire.mSph, 2);
        dCcD_Stts_dt(&p->mEnemyFire.mStts, 2);
        cc_ObjAcch_dt(&p->mEnemyIce.mBgAcch);
        Cc_circleDtor(&p->mEnemyIce.mBgAcchCir.m_cir, 2);
        dCcD_Cyl_dt(&p->mEnemyIce.mCyl, 2);
        dCcD_Stts_dt(&p->mEnemyIce.mStts, 2);
        dCcD_Cyl_dt(&p->mCyl, 2);
        dCcD_Stts_dt(&p->mStts, 2);
        cc_ObjAcch_dt(&p->mAcch);
        Cc_circleDtor(&p->mAcchCir.m_cir, 2);
        Cc_actorDtor(p, 0);
        if (flags & 1)
            operator_delete(p);
    }
}

/* 02113218 */
static void SafeString_assureTerminationImpl(void *) {}

/* WWHD glue
str 1000C628 "Cc" in 0210C5C8
str 1000C6CC "Cc" in 021104B4
str 1000C6CF "Cc" in 02110528
str 1000C6E0 "c_green"
str 1000C6E8 "c_black"
str 1000C6F0 "c_kiiro"
str 1000C6F8 "c_red"
str 1000C700 "Cc" in 02110D4C
str 1000C704 "c_blue"
str 1000C70C "a_arg1 < sizeof(cc_HP_kind) / sizeof(cc_HP_kind[0])"
str 1000C740 "d_a_cc.cpp"
data cc_atsp_kind 101B3B28
data cc_atatp_kind 101B3B3C
data cc_HP_kind 101B3B00
data fire_j 101B3B78
data fire_sc 101B3B50
data body_co_cyl 101B3B84
data target_info_count 10462CF0 nocheck
data target_info 10462D00 nocheck
data l_currentModel 104B462C nocheck
data l_currentMatrix 104B4868 nocheck
data DEMO_COME_START_FLAG 101B3AFD nocheck
data DEMO_RET_START_FLAG 101B3AFE nocheck
data DEMO_SHORT_CUT_FLAG 101B3AFC nocheck
ext 02041570 void Cc_enemyFire(enemyfire_l*)
ext 02041C30 void Cc_enemyFireRemove(enemyfire_l*)
ext 02591284 void Cc_iceUpdateDL(mDoExt_McaMorf*, s32, void*)
ext 020CB92C BOOL Cc_bombCheckState(void*, s32)
ext 020CBAAC BOOL Cc_bomb2CheckExplode(void*)
ext 02008254 BOOL Cc_checkPolySafe(dBgS*, void*)
ext 024EF0BC s32 Cc_getGroundCode(dBgS*, void*)
ext 025A9270 void Cc_rippleRemove(void*)
ext 0252A038 BOOL Cc_detectLight(u32, cXyz*)
ext 025E80D0 void* Cc_brkCtor(void*)
ext 025E7C6C void* Cc_btkCtor(void*)
ext 025E8154 BOOL Cc_brkInit(void*, J3DModelData*, void*, s32, s32, s16, s16, bool, s32, f32)
ext 025E7CE0 BOOL Cc_btkInit(void*, J3DModelData*, void*, s32, s32, s16, s16, bool, s32, f32)
ext 027F3F94 void* Cc_jointTree(J3DModelData*)
ext 025E8A48 bool Cc_invisibleCreate(void*, J3DModel*)
ext 025166F0 void Cc_sphereCtor(dCcD_Sph*)
ext 024F0474 void Cc_acchCtor(dBgS_ObjAcch*)
ext 025A9084 void Cc_rippleCtor(void*)
ext 024EFE94 void Cc_acchCirCtor(dBgS_AcchCir*)
ext 025E895C void Cc_invisibleCtor(void*)
ext 0200E814 s32 Cc_getNameIndex(u32, const char*, s32)
ext 024EFD9C void Cc_acchDtor(dBgS_ObjAcch*, s32)
ext 025E89F8 void Cc_invisibleDtor(void*, s32)
ext 02515AE8 void Cc_sphereDtor(dCcD_Sph*, s32)
ext 02018034 void Cc_circleDtor(void*, s32)
ext 025D50BC void Cc_actorDtor(void*, s32)
*/
