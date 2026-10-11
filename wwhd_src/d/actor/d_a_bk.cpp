/**
 * d_a_bk.cpp (WWHD)
 * Enemy - Bokoblin
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bk_main.h"

static bkMainSphereSource head_sph_src = {
    0x0, 0x0, 0,       {0, 0, 0}, 0x0, 0xff1dfeff,         0x3,  0x0,
    0x0, 0x0, 0x10000, 0x2,       0x0, {0.0f, 0.0f, 0.0f}, 37.5f};
static bkMainSphereSource wepon_sph_src = {
    0x0, 0x800, 2,   {0, 0, 0},          0xf,   0x0, 0x0, 0x0, 0xd0000, 0x1,
    0x0, 0x0,   0x0, {0.0f, 0.0f, 0.0f}, 100.0f};
static bkMainSphereSource defence_sph_src = {
    0x0, 0x0, 0,         {0, 0, 0}, 0x0, 0xff1dfeff,         0x3,  0x0,
    0x0, 0x0, 0x60c0000, 0x3,       0x0, {0.0f, 0.0f, 0.0f}, 62.5f};
static bkMainCylinderSource co_cyl_src = {
    0x0, 0x0, 0,   {0, 0, 0},          0x0,   0x0,   0x0, 0x75, 0x0, 0x0,
    0x0, 0x2, 0x0, {0.0f, 0.0f, 0.0f}, 62.5f, 100.0f};
static bkMainCylinderSource tg_cyl_src = {
    0x0, 0x0, 0,       {0, 0, 0}, 0x0, 0xff1dfeff,         0x3,   0x0,
    0x0, 0x0, 0x10000, 0x2,       0x0, {0.0f, 0.0f, 0.0f}, 30.0f, 112.5f};
static bkMainJointHitSource search_data[15] = {
    {1, 1, 20.0f, 0x10191640},  {0, 3, 5.0f, 0x1019164c},   {0, 4, 2.5f, 0x10191664},
    {0, 8, 5.0f, 0x1019164c},   {0, 9, 2.5f, 0x10191664},   {0, 12, 3.0f, 0x1019167c},
    {0, 13, 2.5f, 0x1019167c},  {0, 14, 1.5f, 0x1019167c},  {0, 15, 2.5f, 0x1019167c},
    {0, 16, 20.0f, 0x10191694}, {0, 16, 15.0f, 0x101916ac}, {0, 33, 6.0f, 0x101916c4},
    {0, 34, 3.0f, 0x101916c4},  {0, 41, 6.0f, 0x101916dc},  {0, 42, 3.0f, 0x101916dc}};

static s8 joint_check[52] = {-1, -1, 11, 7,  3,  15, -1, 10, 6,  2,  14, -1, -1, -1, -1, -1, 12, 13,
                             18, 50, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 9,  5,  1,  17,
                             -1, -1, -1, -1, 8,  4,  0,  16, -1, -1, -1, -1, -1, -1, -1, 19};
static u8 check_bit[4] = {1, 2, 4, 8};
static f32 xad[4] = {0.0f, 0.0f, 1.0f, -1.0f};
static f32 zad[4] = {1.0f, -1.0f, 0.0f, 0.0f};
static f32 fire_sc[10] = {2.0f,
                          2.0f,
                          1.2000000476837158f,
                          1.0f,
                          1.2000000476837158f,
                          1.0f,
                          1.2000000476837158f,
                          1.0f,
                          1.2000000476837158f,
                          1.0f};
static u8 fire_j[10] = {18, 16, 41, 42, 33, 34, 8, 9, 3, 4};

/* 02098DA4 */
void anm_init(bk_class* i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 speed,
              int soundFileIdx) {
    if (i_this->dr.mAction == 19 && bckFileIdx != dRes_INDEX_BK_BCK_BK_OTISOU1_e &&
        bckFileIdx != dRes_INDEX_BK_BCK_BK_OTISOU2_e) {
        return;
    }
    if (soundFileIdx >= 0) {
        J3DAnmTransform* anm =
            (J3DAnmTransform*)dComIfG_getObjectRes("Bk", bckFileIdx, SAFESTRING_VTBL);
        void* sound = dComIfG_getObjectRes("Bk", soundFileIdx, SAFESTRING_VTBL);
        i_this->mpMorf->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, sound);
    } else {
        J3DAnmTransform* anm =
            (J3DAnmTransform*)dComIfG_getObjectRes("Bk", bckFileIdx, SAFESTRING_VTBL);
        i_this->mpMorf->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, nullptr);
    }
}

/* 02098EEC */
void smoke_set_s(bk_class* i_this, f32 rate) {
    fopAc_ac_c* i_actor = i_this;
    gabi::Local<dBgS_LinChk_l> linChk;
    dBgS_LinChk_ct(linChk);
    s32 attribCode = 0;
    gabi::Local<cXyz> startPos;
    *startPos = i_this->m0338.get();
    startPos->y += 100.0f;
    gabi::Local<cXyz> endPos;
    *endPos = i_this->m0338.get();
    endPos->y -= 100.0f;
    dBgS_LinChk_Set_l(linChk, startPos, endPos, i_actor);

    if (LineCross(linChk)) {
        *endPos = bkMain_lineCross(linChk)->get(); /* GetCross() */
        i_this->m0338.y = endPos->y;
        attribCode = dBgS_GetAttributeCode(bkMain_polyInfo(linChk));
    } else {
        i_this->m0338.y -= 20000.0f;
    }

    if (i_this->m034F != 0 && attribCode != dBgS_Attr_GRASS_e) {
        dBgS_LinChk_dt(linChk);
        return;
    }

    i_this->m034F++;

    switch (attribCode) {
    case 0: /* NORMAL, DIRT, WOOD, STONE, SAND */
    case 1:
    case 2:
    case 3:
    case dBgS_Attr_SAND_e: {
        dPa_smokeEcallBack_end((dPa_smokeEcallBack*)&i_this->m0350);
        s8 roomNo = fopAcM_GetRoomNo(i_this);
        /* dComIfGp_particle_setToon: group 2 */
        JPABaseEmitter* emitter1 = dPa_control_set(
            dComIfGp_getParticle(), 2, dPa_ID_AK_JT_ELEMENTSMOKE00, &i_this->m0338, &i_this->m0344,
            nullptr, 0xB9, (dPa_levelEcallBack*)&i_this->m0350, roomNo, nullptr, nullptr, nullptr);
        if (emitter1) {
            JPA_setRate(emitter1, rate);
            JPA_setSpread(emitter1, 1.0f);
            JPA_setGlobalScale(emitter1, 1.2f, 1.2f, 1.2f);
            f32 s = 1.5f + REG0_F(16);
            JPA_setGlobalParticleScale(emitter1, s, s, s);
        }
        break;
    }
    case dBgS_Attr_GRASS_e: {
        JPABaseEmitter* emitter2 =
            dComIfGp_particle_set(dPa_ID_AK_JN_ELEMENTKUSA00, &i_this->m0338, &i_this->m0344);
        if (emitter2) {
            JPA_setRate(emitter2, rate * 0.5f);
            JPA_setMaxFrame(emitter2, 3);
        }
        break;
    }
    }
    dBgS_LinChk_dt(linChk);
}

/* 020A070C */
void ground_smoke_set(bk_class* i_this) {
    if (i_this->m034C == 0) {
        return;
    }

    i_this->m034C--;

    if (i_this->m034C >= l_bkHIO().m00C) {
        i_this->m0344.x = 0;
        i_this->m0344.z = 0;
        gabi::Local<cXyz> sp8;
        sp8->x = 0.0f;
        sp8->y = 0.0f;
        MtxTrans(i_this->current.pos.x, i_this->current.pos.y + 7.5f, i_this->current.pos.z, 0);
        if (i_this->m034E == 0) {
            sp8->z = -350.0f;
            cMtx_YrotM(calc_mtx(), i_this->m034A);
            MtxPosition(sp8, &i_this->m0338);
            i_this->m0344.y = i_this->m034A;
            smoke_set_s(i_this, 6.0f);
            i_this->m034A = i_this->m034A + 2000 + REG0_S(7);
        } else if (i_this->m034E == 1) {
            cMtx_YrotM(calc_mtx(), i_this->current.angle.y);
            cMtx_YrotM(calc_mtx(), i_this->m034A);
            sp8->z = -55.0f;
            MtxPosition(sp8, &i_this->m0338);
            i_this->m0344.y = i_this->m034A;
            smoke_set_s(i_this, 5.0f);
            i_this->m034A += 0x1FA0;
        } else if (i_this->m034E == 2) {
            MtxTrans(i_this->m11A8.x, i_this->m11A8.y + 7.5f, i_this->m11A8.z, 0);
            cMtx_YrotM(calc_mtx(), i_this->m034A);
            sp8->z = -12.5f;
            MtxPosition(sp8, &i_this->m0338);
            i_this->m0344.y = i_this->m034A;
            smoke_set_s(i_this, 6.0f);
            i_this->m034A += 0x2000;
        } else if (i_this->m034E == 3) {
            cMtx_YrotM(calc_mtx(), i_this->current.angle.y);
            cMtx_YrotM(calc_mtx(), i_this->m034A);
            sp8->z = -37.5f;
            MtxPosition(sp8, &i_this->m0338);
            i_this->m0344.y = i_this->m034A;
            smoke_set_s(i_this, 2.0f);
            i_this->m034A += 0x1FA0;
        } else if (i_this->m034E == 4) {
            /* lfs/stfs copy (an SNaN is quieted, as in the recompiled code) */
            auto p = Vec3f{};
            if (i_this->m02F8 & 1) {
                p = i_this->dr.m100[14].get();
            } else {
                p = i_this->dr.m100[15].get();
            }
            i_this->m0338.x = p.x;
            i_this->m0338.y = p.y;
            i_this->m0338.z = p.z;
            f32 y = p.y;
            if (i_this->dr.m712 != 0) {
                i_this->m0338.y = 512.5f;
            } else {
                i_this->m0338.y = y - 12.5f;
            }
            i_this->m0344.y = cM_atan2s(i_this->speed.x, i_this->speed.z);
            smoke_set_s(i_this, 1.0f);
        }
    } else {
        i_this->m0338.y = i_this->dr.mSpawnY + 25000.0f;
    }

    if (i_this->m034C == 0) {
        dPa_smokeEcallBack_end((dPa_smokeEcallBack*)&i_this->m0350);
        i_this->m034F = 0;
    }
}

/* 020991CC */
BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    // HD: validate joint and joint-check indices before accessing the tables.
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(node);
        u32 jntNo = bkMain_jointNo(joint);
        if (!(jntNo < 0x34)) {
            JUT_ASSERT_fail(
                "d_a_bk.cpp", 0x5AA,
                "no2 < sizeof(joint_check) / sizeof(joint_check[0])"); /* "no2 < sizeof(joint_check)
                                                                          / ..." */
            if (!(jntNo < 0x34))
                return TRUE;
        }
        J3DModel* model = j3dSys_getModel();
        bk_class* i_this = bkMain_actor(model);
        int r28 = joint_check[jntNo];
        if (i_this) {
            PSMTXCopy(bk_getAnmMtx(model, jntNo), calc_mtx());
            if (jntNo == BK_JNT_AGO_e) {
                cMtx_ZrotM(calc_mtx(), i_this->m11F4);
                bk_setAnmMtx(model, jntNo, calc_mtx());
                PSMTXCopy(calc_mtx(), J3DSys_mCurrentMtx());
            } else {
                if (!(r28 < 20 + 1)) {
                    JUT_ASSERT_fail("d_a_bk.cpp", 0x5BD, "no < 20+1"); /* "no < 20+1" */
                    return TRUE;
                }
                cMtx_YrotM(calc_mtx(), i_this->dr.m088[r28].y);
                cMtx_XrotM(calc_mtx(), i_this->dr.m088[r28].x);
                cMtx_ZrotM(calc_mtx(), i_this->dr.m088[r28].z);

                bk_setAnmMtx(model, jntNo, calc_mtx());
                PSMTXCopy(calc_mtx(), J3DSys_mCurrentMtx());

                gabi::Local<cXyz> offset;
                offset->x = 0.0f;
                offset->y = 0.0f;
                offset->z = 0.0f;
                gabi::Local<cXyz> sp08;
                if ((u32)r28 <= 7) { /* KOSI, HIP1, KOKAL, MOMOL, SUNEL1, SUNEL2, ASIL, KOKAR */
                    offset->x = 0.0f;
                } else if (r28 == BK_JNT_HEAD_e) {
                    offset->x = 200.0f;
                    offset->y = -100.0f;
                    MtxPosition(offset, sp08);
                    offset->x = 0.0f;
                    offset->y = 0.0f;
                    MtxPosition(offset, &i_this->eyePos);
                    cXyz* attnPos = &i_this->attention_info.position;
                    *attnPos = i_this->eyePos.get();
                    attnPos->y += l_bkHIO().m024;
                    if (l_bkHIO().m009 == 0) {
                        i_this->m0330 =
                            cM_atan2s(sp08->x - i_this->eyePos.x, sp08->z - i_this->eyePos.z);
                    } else {
                        i_this->m0330 = i_this->current.angle.y;
                    }
                    offset->x = 20.75f;
                    offset->y = 18.5f;
                    offset->z = 0.0f;
                    MtxPosition(offset, &i_this->m116C);
                    offset->y = -45.0f;
                }
                MtxPosition(offset, &i_this->dr.m100[r28]);
            }
        }
    }
    return TRUE;
}

/* 02099584 */
BOOL nodeCallBack_P(J3DNode* node, int calcTiming) {
    // HD: validate the joint number before accessing the table.
    if (calcTiming == 0) {
        J3DJoint* joint = J3DNode_toJoint(node);
        u32 jntNo = bkMain_jointNo(joint);
        if (!(jntNo < 0x34)) {
            JUT_ASSERT_fail("d_a_bk.cpp", 0x60C,
                            "no2 < sizeof(joint_check) / sizeof(joint_check[0])");
            if (!(jntNo < 0x34))
                return TRUE;
        }
        J3DModel* model = j3dSys_getModel();
        bk_class* i_this = bkMain_actor(model);
        int r30 = joint_check[jntNo];
        if (i_this) {
            PSMTXCopy(bk_getAnmMtx(model, jntNo), calc_mtx());
            gabi::Local<cXyz> offset;
            offset->x = 0.0f;
            offset->z = 0.0f;
            offset->y = 0.0f;
            if (r30 == BK_JNT_KUBI_e) {
                offset->x = 17.5f;
                offset->y = -8.75f;
                offset->z = 0.0f;
                MtxPosition(offset, &i_this->m1190);
            } else if (r30 == BK_JNT_MUNE_e) {
                MtxPosition(offset, &i_this->m119C);
            } else if (r30 == BK_JNT_SIPPO3_e || r30 == BK_JNT_SIPPO4_e) {
                offset->y = 25.0f;
            } else {
                offset->y = 0.0f;
            }
            MtxPosition(offset, &i_this->dr.m100[r30]);
        }
    }
    return TRUE;
}

/* inlined into daBk_Draw */
static inline void search_check_draw(bk_class* i_this) {
    if (l_bkHIO().m005 == 0) {
        return;
    }
    gabi::Local<cXyz> sp08;
    gabi::Local<cXyz> sp14[8];
    sp08->z = l_bkHIO().m028;
    sp08->y = 0.0f;
    sp08->x = 0.0f;
    int i;
    s16 r26 = 0;
    for (i = 0; i < 0x10; i++, r26 += 0x1000) {
        MtxTrans(i_this->current.pos.x, i_this->dr.mSpawnY + 2.5f, i_this->current.pos.z, 0);
        cMtx_YrotM(calc_mtx(), r26);
        MtxPosition(sp08, sp14[0]);
        cMtx_YrotM(calc_mtx(), 0x1000);
        MtxPosition(sp08, sp14[1]);
    }
    sp08->z = l_bkHIO().m02C;
    for (i = 0; i < 0x10; i++, r26 += 0x1000) {
        MtxTrans(i_this->current.pos.x, i_this->dr.mSpawnY + 2.5f, i_this->current.pos.z, 0);
        cMtx_YrotM(calc_mtx(), r26);
        MtxPosition(sp08, sp14[0]);
        cMtx_YrotM(calc_mtx(), 0x1000);
        MtxPosition(sp08, sp14[1]);
    }

    sp08->x = 0.0f;
    sp08->z = l_bkHIO().m028;
    MtxTrans(i_this->eyePos.x, i_this->eyePos.y, i_this->eyePos.z, 0);

    MtxPush();
    cMtx_YrotM(calc_mtx(), (s16)(i_this->m0330 - l_bkHIO().m034));
    sp08->y = l_bkHIO().m038;
    MtxPosition(sp08, sp14[1]);
    MtxPull();

    MtxPush();
    sp08->y = l_bkHIO().m038;
    cMtx_YrotM(calc_mtx(), (s16)(i_this->m0330 + l_bkHIO().m034));
    MtxPosition(sp08, sp14[2]);
    MtxPull();

    MtxPush();
    cMtx_YrotM(calc_mtx(), (s16)(i_this->m0330 - l_bkHIO().m034));
    sp08->y = -l_bkHIO().m038;
    MtxPosition(sp08, sp14[4]);
    MtxPull();

    sp08->y = -l_bkHIO().m038;
    cMtx_YrotM(calc_mtx(), (s16)(i_this->m0330 + l_bkHIO().m034));
    MtxPosition(sp08, sp14[5]);

    *sp14[0] = i_this->eyePos.get();
    sp14[0]->y += l_bkHIO().m038;
    *sp14[3] = i_this->eyePos.get();
    sp14[3]->y -= l_bkHIO().m038;
    sp08->x = 0.0f;
    sp08->z = l_bkHIO().m02C;
    MtxTrans(i_this->eyePos.x, i_this->eyePos.y, i_this->eyePos.z, 0);
    cMtx_YrotM(calc_mtx(), i_this->current.angle.y);

    sp08->x = l_bkHIO().m03C;
    sp08->y = l_bkHIO().m040;
    sp08->z = l_bkHIO().m044;
    MtxPosition(sp08, sp14[7]);
    sp08->y = -l_bkHIO().m040;
    MtxPosition(sp08, sp14[5]);
    sp08->x = -l_bkHIO().m03C;
    sp08->y = l_bkHIO().m040;
    MtxPosition(sp08, sp14[6]);
    sp08->y = -l_bkHIO().m040;
    MtxPosition(sp08, sp14[4]);
    sp08->x = l_bkHIO().m03C;
    sp08->y = l_bkHIO().m040;
    sp08->z = l_bkHIO().m048;
    MtxPosition(sp08, sp14[1]);
    sp08->y = -l_bkHIO().m040;
    MtxPosition(sp08, sp14[3]);
    sp08->x = -l_bkHIO().m03C;
    sp08->y = l_bkHIO().m040;
    MtxPosition(sp08, sp14[0]);
    sp08->y = -l_bkHIO().m040;
    MtxPosition(sp08, sp14[2]);
}

/* inlined into daBk_Draw */
static inline void br_draw(bk_class* i_this) {
    if (i_this->m02F0 == 0) {
        return;
    }
    if (REG6_S(3) != 0) {
        return;
    }

    J3DModel* model = i_this->m02E8;
    for (u16 i = 0; i < bkMain_jointCount(model); i++) {
        u32 md = J3DModel_modelData(model);
        u32 joint = bkMain_joint(md, i);
        u32 mat = bkMain_mesh(joint); /* getMesh() */
        while (mat) {
            u32 shape = bkMain_shape(mat);
            if (i_this->m02F4 == 0) {
                if (i == BK_KB_JNT_BLURA_e) {
                    J3DShape_show(shape);
                } else {
                    J3DShape_hide(shape);
                }
            } else {
                if (i == BK_KB_JNT_BLURB_e) {
                    J3DShape_show(shape);
                } else {
                    J3DShape_hide(shape);
                }
            }
            mat = bkMain_nextMaterial(mat); /* getNext() */
        }
    }

    PSMTXCopy(bk_getAnmMtx(i_this->mpMorf->getModel(), BK_JNT_BUKI_e), calc_mtx());
    MtxTrans(150.0f + l_bkHIO().m100, REG8_F(1), REG8_F(2), 1);
    cMtx_XrotM(calc_mtx(), (s16)(REG8_S(6) + 0x4000));
    cMtx_ZrotM(calc_mtx(), (s16)(REG8_S(7) + 0x4000));
    MtxScale(l_bkHIO().m018 * i_this->m02EC, l_bkHIO().m018, l_bkHIO().m018, 1);
    J3DModel_setBaseTRMtx(model, calc_mtx());

    setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
    mDoExt_modelUpdateDL(model);
}

/* 0209971C */
BOOL daBk_Draw(bk_class* i_this) {
    J3DModel* model = i_this->mpMorf->getModel();
    if (i_this->m02B7 != 0xFF && i_this->mType == 6 &&
        dComIfGs_isSwitch(i_this->m02B7, dComIfGp_roomControl_getStayNo())) {
        return TRUE;
    }
    if (i_this->m02BA != 0 || i_this->mType == 8 || i_this->m121C != 0 || i_this->m02DE != 0) {
        return TRUE;
    }

    setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
    if (i_this->mEnemyIce.mFreezeTimer > 20) {
        dMat_ice_entryDL(i_this->mpMorf, -1, nullptr);
        // HD: no daBk_shadowDraw
        return TRUE;
    }

    br_draw(i_this);

    search_check_draw(i_this);

    u32 md = J3DModel_modelData(model);
    u32 rootJoint = bkMain_rootJoint(md);
    u32 eyeShape = bk_getMaterialShape(md, "SC_eye_MX"); /* "SC_eye_MX" */
    u32 hatShape = bk_getMaterialShape(md, "boushi");    /* "boushi" */
    u32 bodyShape = bk_getMaterialShape(md, "fuku");     /* "fuku" */
    J3DShape_hide(eyeShape);
    mDoExt_btpAnm* btp = i_this->m02C4;
    s16 frame = (s16)gabi::ftoi(bkMain_eyeFrame(btp));
    mDoExt_btpAnm_entry(btp, bkMain_modelData(model), frame);
    i_this->mpMorf->entryDL(); // HD: no material-table variant (m1230)
    dComIfGd_setListMaskOff();
    J3DShape_show(eyeShape);
    // HD: no getMatPacket(0)->unlock()
    J3DShape_hide(hatShape);
    J3DShape_hide(bodyShape);
    bkMain_entryIn(model, rootJoint); /* rootJoint->entryIn() (HD: with the model) */
    J3DShape_show(hatShape);
    J3DShape_show(bodyShape);
    dComIfGd_setList();

    // HD: no daBk_shadowDraw

    if (i_this->m02D4 != 0) {
        setLightTevColorType(dKy_getEnvlight(), i_this->m02D0, &i_this->tevStr);
        mDoExt_modelUpdateDL(i_this->m02D0);
    }

    if (i_this->m02DC != 0) {
        setLightTevColorType(dKy_getEnvlight(), i_this->m02D8, &i_this->tevStr);
        mDoExt_modelUpdateDL(i_this->m02D8);
    }

    dSnap_RegistFig(DSNAP_TYPE_BK, i_this, 1.0f, 1.0f, 1.0f);

    return TRUE;
}

/* 02099FF0 */
void way_pos_check(bk_class* i_this, cXyz* r31) {
    fopAc_ac_c* i_actor = i_this;
    gabi::Local<dBgS_LinChk_l> linChk;
    dBgS_LinChk_ct(linChk);
    gabi::Local<bkMainGroundCheck> gndChk;
    bk_GndChk_ct(gndChk);
    gabi::Local<cXyz> sp2C;
    gabi::Local<cXyz> sp20;
    gabi::Local<cXyz> sp14;
    gabi::Local<cXyz> sp08;
    sp2C->x = 0.0f;
    sp2C->y = 50.0f;
    *sp14 = i_this->current.pos.get();
    sp14->y += 50.0f;
    for (int i = 0; i < 100; i++) {
        sp2C->z = 300.0f + cM_rndF(200.0f);
        MtxRotY(cM_rndF(6.2831855f), 0);
        MtxPosition(sp2C, sp20);
        sp08->x = i_this->current.pos.x + sp20->x;
        sp08->y = i_this->current.pos.y + sp20->y;
        sp08->z = i_this->current.pos.z + sp20->z;
        *r31 = sp08->get();
        dBgS_LinChk_Set_l(linChk, sp14, sp08, i_actor);
        if (!LineCross(linChk)) {
            gndChk->m_pos.copy(*sp08);
            f32 gnd = bk_GroundCross(gndChk);
            if (i_this->dr.mAcch.GetGroundH() - gnd < 200.0f) {
                break;
            }
        }
    }
    bk_GndChk_dt(gndChk);
    dBgS_LinChk_dt(linChk);
}

/* 0209A29C */
u8 ground_4_check(bk_class* i_this, int r18, s16 r20, f32 f29) {
    gabi::Local<bkMainGroundCheck> gndChk;
    bk_GndChk_ct(gndChk);
    int i;
    u8 r19 = 0;
    cMtx_YrotS(calc_mtx(), r20);
    gabi::Local<cXyz> sp14;
    sp14->y = 100.0f;
    for (i = 0; i < r18; i++) {
        sp14->x = xad[i] * f29;
        sp14->z = zad[i] * f29;
        gabi::Local<cXyz> sp8;
        MtxPosition(sp14, sp8);
        PSVECAdd(sp8, &i_this->current.pos, sp8);
        gndChk->m_pos.copy(*sp8);
        f32 y = bk_GroundCross(gndChk);
        if (y == -1000000000.0f) {
            y = 1000000000.0f;
        }
        sp8->y = y;
        if (i_this->dr.mAcch.GetGroundH() - y > 200.0f) {
            r19 |= check_bit[i];
        }
    }
    bk_GndChk_dt(gndChk);
    return r19;
}

/* 0209A4BC */
BOOL daBk_other_bg_check(bk_class* i_this, fopAc_ac_c* r23) {
    fopAc_ac_c* i_actor = i_this;
    gabi::Local<dBgS_LinChk_l> linChk;
    dBgS_LinChk_ct(linChk);
    gabi::Local<cXyz> sp14;
    gabi::Local<cXyz> sp08;
    if (r23) {
        *sp08 = r23->current.pos.get();
        sp08->y += 50.0f;
        sp14->copy(i_actor->eyePos);
        dBgS_LinChk_Set_l(linChk, sp14, sp08, i_actor);
        if (LineCross(linChk)) {
            dBgS_LinChk_dt(linChk);
            return TRUE;
        } else {
            dBgS_LinChk_dt(linChk);
            return FALSE;
        }
    }
    dBgS_LinChk_dt(linChk);
    return TRUE;
}

/* 0209A624 */
void* s_w_sub(void* param_1, void*) {
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == fpcNm_BOKO_e) {
        fopAc_ac_c* boko = (fopAc_ac_c*)param_1;
        if (fopAcM_GetParam(boko) != 4 /* daBoko_c::Type_MOBLIN_SPEAR_e */ &&
            !fopAcM_checkCarryNow(boko)) {
            s32 n = target_info_count();
            if (n < 10) {
                target_info_count() = n + 1;
                target_info()[n] = boko;
            }
        }
    }
    return nullptr;
}

/* inlined into daBk_wepon_view_check */
static u32 search_wepon_inline(bk_class* i_this) {
    target_info_count() = 0;
    for (int i = 0; i < 10; i++) {
        target_info()[i] = nullptr;
    }

    fpcM_Search(gabi::guest_fn((const void*)&s_w_sub), i_this);

    f32 f29 = 50.0f;
    if (target_info_count() != 0) {
        fopAc_ac_c* r25;
        int i = 0;
        while (i < target_info_count()) {
            r25 = target_info()[i];
            gabi::Local<cXyz> sp18;
            sp18->x = r25->current.pos.x - i_this->eyePos.x;
            sp18->y = 50.0f + r25->current.pos.y - i_this->eyePos.y;
            sp18->z = r25->current.pos.z - i_this->eyePos.z;
            f32 f4 = std_sqrtf(gabi::fmadds(sp18->x, sp18->x, sp18->z * sp18->z));
            if (f4 < f29 && !daBk_other_bg_check(i_this, r25)) {
                /* GHS: `bgt` skips, so NaN passes */
                if (!(std::fabs(r25->current.pos.y + 50.0f - i_this->eyePos.y) > l_bkHIO().m038)) {
                    s16 ang = cM_atan2s(sp18->x, sp18->z);
                    s16 angleDiff = i_this->m0330 - ang;
                    if (angleDiff < 0) {
                        angleDiff = -angleDiff;
                    }
                    if ((u16)angleDiff < 0x1800) {
                        return fopAcM_GetID(r25);
                    }
                    cMtx_YrotS(calc_mtx(), -i_this->current.angle.y);
                    gabi::Local<cXyz> sp0C;
                    MtxPosition(sp18, sp0C);
                    if (std::fabs((f32)sp0C->x) < l_bkHIO().m03C &&
                        std::fabs((f32)sp0C->y) < l_bkHIO().m040 && sp0C->z > l_bkHIO().m048 &&
                        sp0C->z < l_bkHIO().m044) {
                        return fopAcM_GetID(r25);
                    }
                }
            }
            i++;
            if (i == target_info_count()) {
                i = 0;
                f29 += 50.0f;
                if (f29 > 1500.0f) {
                    return fpcM_ERROR_PROCESS_ID_e;
                }
            }
        }
    } else {
        return fpcM_ERROR_PROCESS_ID_e;
    }

    return fpcM_ERROR_PROCESS_ID_e;
}

/* 0209A6AC */
void* s_b_sub(void* param_1, void*) {
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == fpcNm_BOMB_e) {
        fopAc_ac_c* bomb = (fopAc_ac_c*)param_1;
        if (fopAcM_GetParam(bomb) != 0) {
            s32 n = target_info_count();
            if (n < 10) {
                target_info_count() = n + 1;
                target_info()[n] = bomb;
            }
        }
    }
    return nullptr;
}

/* 0209A728 */
fopAc_ac_c* search_bomb(bk_class* i_this, BOOL r26) {
    if (!(i_this->m1208 & 0x0200)) {
        return nullptr;
    }

    target_info_count() = 0;
    for (int i = 0; i < 10; i++) {
        target_info()[i] = nullptr;
    }

    fpcM_Search(gabi::guest_fn((const void*)&s_b_sub), i_this);

    f32 f29 = 50.0f;
    if (target_info_count() != 0) {
        fopAc_ac_c* r24;
        int i = 0;
        while (i < target_info_count()) {
            r24 = target_info()[i];
            gabi::Local<cXyz> sp28;
            sp28->x = r24->current.pos.x - i_this->eyePos.x;
            sp28->y = 50.0f + r24->current.pos.y - i_this->eyePos.y;
            sp28->z = r24->current.pos.z - i_this->eyePos.z;
            f32 sx = r24->current.pos.x - i_this->current.pos.x;
            f32 sz = r24->current.pos.z - i_this->current.pos.z;
            f32 f0 = std_sqrtf(gabi::fmadds(sp28->x, sp28->x, sp28->z * sp28->z));
            f32 f5 = std_sqrtf(gabi::fmadds(sx, sx, sz * sz));
            if (f0 < f29 && !(f5 > 30.0f + i_this->mPlayerDistance) &&
                !(daBk_other_bg_check(i_this, r24) && r26)) {
                if (r26) {
                    /* GHS: `bgt` skips, so NaN passes */
                    if (!(std::fabs(r24->current.pos.y + 50.0f - i_this->eyePos.y) >
                          l_bkHIO().m038)) {
                        s16 ang = cM_atan2s(sp28->x, sp28->z);
                        s16 angleDiff = i_this->m0330 - ang;
                        if (angleDiff < 0) {
                            angleDiff = -angleDiff;
                        }
                        if ((u16)angleDiff < l_bkHIO().m034) {
                            return r24;
                        }
                        cMtx_YrotS(calc_mtx(), -i_this->current.angle.y);
                        gabi::Local<cXyz> sp10;
                        MtxPosition(sp28, sp10);
                        if (std::fabs((f32)sp10->x) < l_bkHIO().m03C &&
                            std::fabs((f32)sp10->y) < l_bkHIO().m040 && sp10->z > l_bkHIO().m048 &&
                            sp10->z < l_bkHIO().m044) {
                            return r24;
                        }
                    }
                } else {
                    return r24;
                }
            }
            i++;
            if (i == target_info_count()) {
                i = 0;
                f29 += 50.0f;
                if (f29 > 1500.0f) {
                    return nullptr;
                }
            }
        }
    } else {
        return nullptr;
    }

    return nullptr;
}

/* 0209AB64 */
BOOL daBk_wepon_view_check(bk_class* i_this) {
    if (i_this->m02DC != 0 || i_this->m02CC != 0) { // HD: m02DC
        return FALSE;
    }
    i_this->m1200 = search_wepon_inline(i_this);
    if (i_this->m1200 != fpcM_ERROR_PROCESS_ID_e) {
        if (fopAcM_SearchByID(i_this->m1200)) {
            return TRUE;
        }
    }
    return FALSE;
}

/* 0209AE40 */
BOOL daBk_bomb_view_check(bk_class* i_this) {
    fopAc_ac_c* bomb = search_bomb(i_this, 1);
    i_this->m11F8 = bomb;
    return bomb != nullptr ? TRUE : FALSE;
}

/* 0209AE7C */
BOOL daBk_player_bg_check(bk_class* i_this, cXyz* r22) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    if (search_sp() != 0 || i_this->mType == 0xA) {
        return FALSE;
    }
    if (i_this->dr.m713 == 0 && std::fabs((f32)player->speedF) < 0.1f &&
        daPy_getGrabWearTimer(player) < 0.0f) {
        return TRUE;
    }
    gabi::Local<dBgS_LinChk_l> linChk;
    dBgS_LinChk_ct(linChk);
    gabi::Local<cXyz> sp14;
    gabi::Local<cXyz> sp08;
    auto p = r22->get();
    p.y += 75.0f;
    *sp08 = p;
    sp14->copy(i_this->eyePos);
    dBgS_LinChk_Set_l(linChk, sp14, sp08, i_this);
    if (LineCross(linChk)) {
        i_this->dr.m713 = 0;
        dBgS_LinChk_dt(linChk);
        return TRUE;
    }
    dBgS_LinChk_dt(linChk);
    return FALSE;
}

/* 0209B05C */
BOOL daBk_player_view_check(bk_class* i_this, cXyz* r30, s16 r27, s16 r31) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    if (search_sp() != 0 || i_this->mType == 0xA) {
        return TRUE;
    }
    if (daBk_player_bg_check(i_this, r30)) {
        return FALSE;
    }
    if (std::fabs(player->current.pos.y + 50.0f - i_this->eyePos.y) > l_bkHIO().m038) {
        return FALSE;
    }
    s16 angleDiff = i_this->m0330 - r27;
    if (angleDiff < 0) {
        angleDiff = -angleDiff;
    }
    if ((u16)angleDiff < r31) {
        i_this->dr.m713 = 1;
        return TRUE;
    }
    cMtx_YrotS(calc_mtx(), -i_this->current.angle.y);
    gabi::Local<cXyz> sp14;
    sp14->x = r30->x - i_this->current.pos.x;
    sp14->y = r30->y - i_this->current.pos.y;
    sp14->z = r30->z - i_this->current.pos.z;
    gabi::Local<cXyz> sp08;
    MtxPosition(sp14, sp08);
    if (std::fabs((f32)sp08->x) < l_bkHIO().m03C && std::fabs((f32)sp08->y) < l_bkHIO().m040 &&
        sp08->z > l_bkHIO().m048 && sp08->z < l_bkHIO().m044) {
        i_this->dr.m713 = 1;
        return TRUE;
    } else {
        i_this->dr.m713 = 0;
        return FALSE;
    }
}

/* 0209B208 */
BOOL daBk_player_way_check(bk_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s16 angleDiff = i_this->current.angle.y - player->shape_angle.y;
    if (angleDiff < 0) {
        angleDiff = -angleDiff;
    }
    if ((u16)angleDiff < 0x4000) {
        return FALSE;
    }
    return TRUE;
}

/* 0209B264 */
void wait_set(bk_class* i_this) {
    if (i_this->m0B30 != 0 || i_this->m11F3 != 0) {
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_KYORO1_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f,
                 dRes_INDEX_BK_BAS_BK_KYORO1_e);
    } else if (i_this->dr.mAction >= 4) {
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_KYORO1_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f,
                 dRes_INDEX_BK_BAS_BK_KYORO1_e);
    } else {
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_KYORO2_e, 10.0f, J3DFrameCtrl::EMode_LOOP, 1.0f,
                 dRes_INDEX_BK_BAS_BK_KYORO2_e);
    }
    i_this->m0300[1] = (s16)gabi::ftoi(40.0f + cM_rndF(60.0f));
}

/* 0209B354 */
void path_check(bk_class* i_this, u8 r19) {
    fopAc_ac_c* i_actor = i_this;

    if (i_this->ppd == nullptr) {
        return;
    }
    if (i_this->m0B30 == 0 && i_this->m11F3 == 0 && i_this->mType != 4 && i_this->mType != 10 &&
        i_this->mType != 6) {
        return;
    }

    gabi::Local<bkMainPathFlags> sp90;
    gabi::Local<dBgS_LinChk_l> linChk;
    dBgS_LinChk_ct(linChk);
    gabi::Local<cXyz> sp18;
    *sp18 = i_this->current.pos.get();
    sp18->y += 100.0f;
    gabi::Local<cXyz> spc;

    bkMainPath* ppd = (bkMainPath*)(dPath*)i_this->ppd;
    bkMainPathPoint* pnt = ppd->m_points;
    for (int i = 0; i < ((bkMainPath*)(dPath*)i_this->ppd)->m_num; i++, pnt++) {
        spc->x = pnt->m_position.x;
        spc->y = pnt->m_position.y + 100.0f;
        spc->z = pnt->m_position.z;
        dBgS_LinChk_Set_l(linChk, sp18, spc, i_actor);
        sp90->v[i] = !LineCross(linChk);
    }

    f32 f0 = 0.0f;
    bool r6 = false;
    for (int i2 = 0; i2 < 100; i2++, f0 += 50.0f) {
        pnt = ((bkMainPath*)(dPath*)i_this->ppd)->m_points;
        for (int j = 0; j < ((bkMainPath*)(dPath*)i_this->ppd)->m_num; j++, pnt++) {
            if (sp90->v[j] == 0) {
                continue;
            }
            f32 distX = i_this->current.pos.x - pnt->m_position.x;
            f32 distY = i_this->current.pos.y - pnt->m_position.y;
            f32 distZ = i_this->current.pos.z - pnt->m_position.z;
            if (std_sqrtf(gabi::fmadds(distZ, distZ, gabi::fmadds(distX, distX, distY * distY))) <
                f0) {
                if (r19) {
                    i_this->m1216 = j;
                } else {
                    i_this->m1216 = j - i_this->m1217;
                    if (i_this->m1216 >= (s8)((bkMainPath*)(dPath*)i_this->ppd)->m_num) {
                        i_this->m1216 = ((bkMainPath*)(dPath*)i_this->ppd)->m_num;
                    } else if (i_this->m1216 < 0) {
                        i_this->m1216 = 0;
                    }
                }
                r6 = true;
                break;
            }
        }
        if (r6) {
            break;
        }
    }

    if (!r6) {
        i_this->m1215 = 0;
    } else {
        i_this->m1215 = i_this->m02B6 + 1;
    }
    dBgS_LinChk_dt(linChk);
}

/* 0209B67C */
void* ken_s_sub(void* param_1, void*) {
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == fpcNm_ITEM_e) {
        if (daItem_getItemNo(param_1) == dItemNo_DROPPED_SWORD_e) {
            return param_1;
        }
    }
    return nullptr;
}

/* 0209B6D8 */
void* s_s_sub(void* r29, void* r30) {
    bk_class* i_this = (bk_class*)r30;
    if (fopAc_IsActor(r29) && r29 != nullptr && fpcM_GetName(r29) == fpcNm_OBJ_SEARCH_e) {
        fopAc_ac_c* search = (fopAc_ac_c*)r29;
        gabi::Local<cXyz> sp18;
        cXyz_mi(&i_this->home.pos, sp18, &search->current.pos);
        if (std_sqrtf(PSVECSquareMag(sp18)) < 600.0f) {
            return r29;
        }
    }
    return nullptr;
}

/* 0209BA78 */
void* s_s2_sub(void* param_1, void*) {
    if (fopAc_IsActor(param_1) && param_1 != nullptr &&
        fpcM_GetName(param_1) == fpcNm_OBJ_SEARCH_e) {
        if (daObj_Search_getFindFlag()) {
            return param_1;
        }
    }
    return nullptr;
}

/* 0209BAD8 */
void tate_mtx_set(bk_class* i_this) {
    if (i_this->m02D4 != 0) {
        PSMTXCopy(bk_getAnmMtx(i_this->mpMorf->getModel(), BK_JNT_TATE_e), calc_mtx());
        J3DModel_setBaseTRMtx(i_this->m02D0, calc_mtx());
        gabi::Local<cXyz> sp08;
        sp08->x = REG8_F(12);
        sp08->y = REG8_F(13);
        sp08->z = REG8_F(14);
        MtxPosition(sp08, &i_this->m11CC);
    }
}

/* 0209BBD4 */
void bou_mtx_set(bk_class* i_this) {
    if (i_this->m02DC != 0) {
        int jointIdx = BK_JNT_BUKI_e + REG7_S(4);
        PSMTXCopy(bk_getAnmMtx(i_this->mpMorf->getModel(), jointIdx), calc_mtx());
        cMtx_YrotM(calc_mtx(), (s16)(0x4000 + REG7_S(0)));
        cMtx_XrotM(calc_mtx(), REG7_S(1));
        cMtx_ZrotM(calc_mtx(), REG7_S(2));
        MtxTrans(0.01f * REG7_F(9), 0.01f * REG7_F(10), gabi::fmadds(REG7_F(11), 0.01f, 50.0f), 1);
        J3DModel_setBaseTRMtx(i_this->m02D8, calc_mtx());
    }
}

/* 0209F15C */
BOOL daBk_IsDelete(bk_class* i_this) { return TRUE; }

/* 0209F164 */
BOOL daBk_Delete(bk_class* i_this) {
    // HD: remove the additional follow callback.
    dPa_EcallBack_remove(&i_this->mHdFollowCb);
    dComIfG_resDelete(&i_this->mPhase, "Bk");
    if (i_this->heap) {
        i_this->mpMorf->stopZelAnime();
    }
    if (i_this->m121D) {
        hio_set() = 0;
        mDoHIO_deleteChild(l_bkHIO().mNo);
    }
    dPa_EcallBack_remove(&i_this->m0350);
    dPa_EcallBack_remove(&i_this->dr.mParticleCallBack);
    enemy_fire_remove(&i_this->mEnemyFire);
    return TRUE;
}

/* 0209F220 */
BOOL useHeapInit(fopAc_ac_c* i_actor) {
    bk_class* i_this = (bk_class*)i_actor;

    // HD: separate body models replace the GameCube color material tables.
    J3DModelData* bodyData;
    if (i_this->m02DC != 0) {
        bodyData = (J3DModelData*)bk_getRes(dRes_HD_BK_BDL_BK_PINK);
    } else if (i_this->m02D4 != 0) {
        bodyData = (J3DModelData*)bk_getRes(dRes_HD_BK_BDL_BK_GREEN);
    } else {
        bodyData = (J3DModelData*)bk_getRes(dRes_INDEX_BK_BDL_BK_e);
    }
    if (bodyData == nullptr) {
        JUT_ASSERT_fail("d_a_bk.cpp", 0x256F, "modelData != (0)"); /* modelData != 0 */
    }
    J3DAnmTransform* anm = (J3DAnmTransform*)bk_getRes(dRes_INDEX_BK_BCK_BK_SUWARI_e);
    void* bas = bk_getRes(dRes_INDEX_BK_BAS_BK_SUWARI_e);
    i_this->mpMorf =
        mDoExt_McaMorf::create(nullptr, bodyData, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP,
                               1.0f, 0, -1, 1, bas, 0x00080000, 0x37221203);
    if (i_this->mpMorf == nullptr || i_this->mpMorf->getModel() == nullptr) {
        return FALSE;
    }

    J3DModel* model = i_this->mpMorf->getModel();
    for (u16 i = 0; i < bkMain_jointCount(model); i++) {
        if (i != 0 && !(i < 0x34)) { // HD: JUT_ASSERT on the joint_check index (no report)
            continue;
        }
        s32 r3 = joint_check[i];
        if (r3 < 0) {
            continue;
        }
        u32 md = J3DModel_modelData(model);
        u32 joint = bkMain_joint(md, i);
        if (r3 == 0x0E || r3 == 0x0F || r3 == 0x10 || r3 == 0x11 || r3 == 0x14) {
            bkMain_setCallBack(joint, gabi::guest_fn((const void*)&nodeCallBack_P));
        } else {
            bkMain_setCallBack(joint, gabi::guest_fn((const void*)&nodeCallBack));
        }
    }

    // HD: no m1230 material tables

    void* p = operator_new(0x74);
    i_this->m02C4 = p ? mDoExt_btpAnm_ct(p) : nullptr;
    if (i_this->m02C4 == nullptr) {
        return cPhs_ERROR_e; /* GameCube bug kept: a phase state from a BOOL function */
    }
    void* btp = bk_getRes(dRes_INDEX_BK_BTP_TMABATAKI_e);
    if (!mDoExt_btpAnm_init(i_this->m02C4, bkMain_modelData(model), btp, TRUE,
                            J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, FALSE)) {
        return cPhs_ERROR_e;
    }

    // HD: separate sword and club blur models replace material-table selection.
    J3DModelData* modelData;
    if (i_this->m02D5 & 0x40) {
        modelData = (J3DModelData*)bk_getRes(dRes_HD_BK_BMD_KB_KEN);
    } else {
        modelData = (J3DModelData*)bk_getRes(dRes_HD_BK_BMD_KB_BOKO);
    }
    if (modelData == nullptr) {
        JUT_ASSERT_fail("d_a_bk.cpp", 0x25FC, "modelData != (0)");
    }
    i_this->m02E8 = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (i_this->m02E8 == nullptr) {
        return cPhs_ERROR_e;
    }
    /* setBaseScale(scale) */
    cXyz* baseScale = bkMain_baseScale(i_this->m02E8);
    *baseScale = i_this->scale.get();

    if (i_this->m02D4 != 0) {
        modelData = (J3DModelData*)bk_getRes(dRes_INDEX_BK_BMD_BK_TATE_e);
        i_this->m02D0 = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        if (modelData == nullptr) {
            JUT_ASSERT_fail("d_a_bk.cpp", 0x261D, "modelData != (0)");
        }
    }

    if (i_this->m02DC != 0) {
        modelData = (J3DModelData*)bk_getRes(dRes_INDEX_BK_BDL_BOUEN_e);
        i_this->m02D8 = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        if (modelData == nullptr) {
            JUT_ASSERT_fail("d_a_bk.cpp", 0x2626, "modelData != (0)");
        }
    }

    i_this->mpJntHit = JntHit_create(i_this->mpMorf->getModel(), gabi::ea(search_data), 15);
    if (i_this->mpJntHit) {
        i_this->jntHit = gabi::ea(i_this->mpJntHit.get());
    } else {
        return FALSE;
    }

    return TRUE;
}

/* 0209F918 */
cPhs_State daBk_Create(fopAc_ac_c* i_actor) {
    /* fopAcM_ct(i_actor, bk_class) */
    if (!fopAcM_CheckCondition(i_actor, fopAcCnd_INIT_e)) {
        if (i_actor != nullptr) {
            bk_class_ct((bk_class*)i_actor);
        }
        fopAcM_OnCondition(i_actor, fopAcCnd_INIT_e);
    }
    bk_class* i_this = (bk_class*)i_actor;

    cPhs_State phase_state = dComIfG_resLoad(&i_this->mPhase, "Bk");
    if (phase_state == cPhs_COMPLEATE_e) {
        i_actor->gbaName = 1;

        if (bk_isStartStage("ITest63" /* "ITest63" */) ||
            bk_isStartStage("GanonJ" /* "GanonJ" */)) {
            search_sp() = 1;
        } else {
            search_sp() = 0;
        }

        i_this->mType = fopAcM_GetParam(i_actor) & 0xF;
        i_this->m02B9 = fopAcM_GetParam(i_actor) & 0x10;
        i_this->m02D4 = fopAcM_GetParam(i_actor) & 0x20;
        if (i_this->mType == 0xB) {
            i_this->m02D4 = 0;
            i_this->m02DC = 1;
            i_this->mType = 4;
        }
        i_this->m02D5 = fopAcM_GetParam(i_actor) & 0xC0;
        i_this->m02B5 = fopAcM_GetParam(i_actor) >> 8 & 0xFF;
        i_this->m02B6 = fopAcM_GetParam(i_actor) >> 16 & 0xFF;
        i_this->m02B7 = fopAcM_GetParam(i_actor) >> 24 & 0xFF;
        i_this->m02B8 = i_actor->current.angle.z;
        i_actor->current.angle.x = i_actor->current.angle.z = 0;
        if (i_this->m02B8 == 0xFF) {
            i_this->m02B8 = 0;
        }

        if (i_this->m02B8 != 0) {
            if (dComIfGs_isSwitch(i_this->m02B8, fopAcM_GetRoomNo(i_actor))) {
                return cPhs_ERROR_e;
            }
        }
        if (i_this->m02B9 != 0) {
            if (dComIfGs_isSwitch(i_this->m02B7, fopAcM_GetRoomNo(i_actor))) {
                return cPhs_ERROR_e;
            }
            i_this->m02B7 = 0xFF;
        }

        i_actor->itemTableIdx = dComIfGp_CharTbl_GetNameIndex("Bk", 0);

        if (!fopAcM_entrySolidHeap(i_actor, gabi::guest_fn((const void*)&useHeapInit), 0x17B20)) {
            return cPhs_ERROR_e;
        }

        if (!hio_set()) {
            l_bkHIO().mNo = mDoHIO_createChild(
                "\u0083{\u0083R\u0082\u00bf\u0082\u00e1\u0082\u00f1" /* "Boko-chan" */, &l_bkHIO());
            i_this->m121D = 1;
            hio_set() = 1;
        }

        ken() = 0;

        if (!i_this->mpMorf || !i_this->mpMorf->getModel()) {
            return cPhs_ERROR_e;
        }

        fopAcM_SetMin(i_actor, -200.0f, -50.0f, -100.0f);
        fopAcM_SetMax(i_actor, 125.0f, 250.0f, 250.0f);
        i_actor->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpMorf->getModel()));
        bkMain_setUserArea(i_this->mpMorf->getModel(), i_this); /* setUserArea */
        i_this->mBtHeight = 162.5f;                             /* initBt */
        i_this->mBtBodyR = 125.0f;

        i_this->dr.m70C = 1;
        i_this->dr.mSpawnY = i_actor->current.pos.y;
        i_this->dr.mMaxFallDistance = 1000.0f;

        if (i_this->m02B6 != 0xFF) {
            i_this->ppd = dPath_GetRoomPath(i_this->m02B6, fopAcM_GetRoomNo(i_actor));
            if (i_this->ppd == nullptr) {
                return cPhs_ERROR_e;
            }
            i_this->m1215 = i_this->m02B6 + 1;
            i_this->m1217 = 1;
        }

        // HD: the spawn timer temporarily sets the status bit.
        i_actor->actor_status |= fopAcStts_HD_4000;
        i_this->m02DF = 5;

        if (i_this->mType == 4 || i_this->mType == 0xA) {
            i_this->dr.mAction = 1;
            if (i_this->mType == 0xA) {
                i_this->m02DF = 0; // HD: this type starts without the spawn timer.
                i_this->dr.mMode = -20;
                i_actor->actor_status |= fopAcStts_BOSS_e;
                // HD: follows the USA version without setting search_sp here.
            } else {
                i_this->dr.mMode = -1;
            }
            i_this->m0300[1] = (s16)gabi::ftoi(1000.0f + cM_rndF(1000.0f));
        } else if (i_this->mType == 6) {
            i_this->dr.mAction = 2;
            i_this->dr.mMaxFallDistance = 300.0f;
        } else if (i_this->mType == 7) {
            i_this->dr.mAction = 29;
            i_this->dr.mMaxFallDistance = 300.0f;
        } else if (i_this->mType == 5) {
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_HAKOBI_e, 1.0f, J3DFrameCtrl::EMode_LOOP, 1.0f,
                     -1);
            i_this->dr.mAction = 30;
            i_this->dr.mMaxFallDistance = 100000.0f;
        } else if (i_this->mType == 2 || i_this->mType == 3) {
            i_this->m02BA = 0xFF;
            i_this->dr.mAction = 15;
            i_this->m030E = 0xA;
        } else if (i_this->mType == 9) {
            i_this->dr.mAction = 3;
            i_this->m1216 = i_actor->current.angle.z;
            i_this->m1217 = i_actor->current.angle.y;
            i_actor->current.angle.z = 0;
            i_actor->current.angle.y = 0;
        }

        if (i_this->m02B7 != 0xFF) {
            if (i_this->mType != 6) {
                i_this->m02BA = i_this->m02B7 + 1;
            }
        }

        if (i_this->mType == 0xF) {
            i_this->dr.mAction = 23;
            i_this->m02BA = 0;
        }

        if (i_this->mType != 8 && i_this->m02DC == 0 && !bk_isStartStage("A_mori" /* "A_mori" */)) {
            u32 weaponType;
            if (i_this->m02D5 & 0x40) {
                weaponType = 1;
            } else if (i_this->m02D5 & 0x80) {
                weaponType = 7;
            } else {
                weaponType = 0;
            }
            i_this->m1200 = fopAcM_create(fpcNm_BOKO_e, weaponType, &i_actor->current.pos,
                                          fopAcM_GetRoomNo(i_actor), nullptr, nullptr, -1, 0);
            i_this->m1214 = 1;
            i_this->m02D5 &= 0x40;
        } else {
            i_this->m11F3 = 1;
        }

        i_this->dr.mAcch.Set(&i_actor->current.pos, &i_actor->old.pos, i_this, 1,
                             &i_this->dr.mAcchCir, &i_actor->speed);
        i_this->dr.mAcchCir.SetWall(40.0f, 40.0f);
        i_this->dr.mAcch.m_flags &= ~(u32)dBgS_Acch::ROOF_NONE; /* ClrRoofNone */
        i_this->dr.mAcch.SetRoofCrrHeight(REG0_F(7) + 80.0f);
        i_this->dr.mAcch.OnLineCheck();
        i_this->dr.mInvincibleTimer = 5;

        if (i_this->m02D4 != 0) {
            i_actor->max_health = i_actor->health = 7;
        } else {
            i_actor->max_health = i_actor->health = 5;
        }

        i_this->dr.mStts.Init(200, 0xFF, i_this);
        i_this->m0B88.Set((const dCcD_SrcCyl*)&co_cyl_src);
        i_this->m0B88.SetStts(&i_this->dr.mStts);
        i_this->m0CB8.Set((const dCcD_SrcCyl*)&tg_cyl_src);
        i_this->m0CB8.SetStts(&i_this->dr.mStts);
        i_this->m0DE8.Set((const dCcD_SrcSph*)&head_sph_src);
        i_this->m0DE8.SetStts(&i_this->dr.mStts);
        i_this->m1040.Set((const dCcD_SrcSph*)&wepon_sph_src);
        i_this->m1040.SetStts(&i_this->dr.mStts);
        i_this->m0F14.Set((const dCcD_SrcSph*)&defence_sph_src);
        i_this->m0F14.SetStts(&i_this->dr.mStts);

        i_this->m02CC = 5;
        i_this->model = gabi::ea(i_this->mpMorf->getModel());

        i_this->mEnemyIce.mpActor = i_this;
        i_this->mEnemyIce.mWallRadius = 50.0f + REG0_F(4);
        i_this->mEnemyIce.mCylHeight = 180.0f + REG0_F(5);
        i_this->mEnemyIce.mDeathSwitch = i_this->m02B8;

        i_this->mEnemyFire.mpMcaMorf = i_this->mpMorf;
        i_this->mEnemyFire.mpActor = i_this;

        for (int i = 0; i < 10; i++) {
            i_this->mEnemyFire.mFlameJntIdxs[i] = fire_j[i];   /* fire_j */
            i_this->mEnemyFire.mParticleScale[i] = fire_sc[i]; /* fire_sc */
        }

        i_actor->stealItemLeft = 3;

        daBk_Execute(i_this);
    }

    return phase_state;
}

// HD: compiler-generated and template code of this translation unit

/* 020A0634 */
fopAc_ac_c* fopAcM_SearchByID_bk(u32 id) { return fopAcM_SearchByID(id); }

/* 020A0670 */
void fopAcM_monsSeStart_bk(fopAc_ac_c* a, u32 id, u32 param) {
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        s8 room = fopAcM_GetRoomNo(a);
        u32 pid = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        bkMain_monsSeStart(id, &a->eyePos, pid, param, reverb);
    }
}

/* 020A06F8 */
void bkHIO_c_dt(bkHIO_c* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1)) {
        operator_delete(i_this);
    }
}

/* 020A0364 */
void __sinit_d_a_bk_cpp() {
    for (int i = 0; i < 4; i++)
        bkMain_headerZeros()[i] = 0;
    __register_global_object(gabi::ea(bkMain_headerRecord(0)));
    bkMain_headerStatics().mNegativePi = -3.1415927f;
    bkMain_headerStatics().mPositivePi = 3.1415927f;
    bkMain_firstHeaderObject(&bkMain_headerStatics().mFirstObject);
    __register_global_object(gabi::ea(bkMain_headerRecord(1)));
    bkMain_secondHeaderObject(&bkMain_headerStatics().mSecondObject);
    __register_global_object(gabi::ea(bkMain_headerRecord(2)));

    /* bkHIO_c::bkHIO_c() */
    bkHIO_c& h = l_bkHIO();
    h.m005 = 0x0;
    h.m006 = 0x0;
    h.m007 = 0x0;
    h.m008 = 0x1;
    h.m009 = 0x0;
    h.m00A = 0x0;
    h.m01C = 1.0f;
    h.m020 = 25.0f;
    h.m00C = 0x4;
    h.m024 = 25.0f;
    h.m028 = 1000.0f;
    h.m02C = 400.0f;
    h.m030 = 240.0f;
    h.m034 = 0x6590;
    h.m038 = 300.0f;
    h.m03C = 500.0f;
    h.m040 = 300.0f;
    h.m044 = 500.0f;
    h.m048 = -125.0f;
    h.m04C = 12.0f;
    h.m050 = 60.0f;
    h.m054 = 45.0f;
    h.m058 = 70.0f;
    h.m05C = 90.0f;
    h.m068 = 90.0f;
    h.m06C = 2.0f;
    h.m070 = 1.0f;
    h.m074 = 1.5f;
    h.m060 = 20.0f;
    h.m064 = 20.0f;
    h.m078 = 0x23;
    h.m07C = 50.0f;
    h.m080 = 25.0f;
    h.m084 = 25.0f;
    h.m088 = 50.0f;
    h.m08C = 0x1e;
    h.m08E = 0x12c;
    h.m090 = 1.2f;
    h.m094 = 1.0f;
    h.m098 = 1.0f;
    h.m09C = 10.0f;
    h.m0A0 = 0x17;
    h.m0A2 = 0x1;
    h.m0A4 = 0x1;
    h.m0A6 = 0x0;

    for (int i = 0; i < 10; i++)
        h.m0A8[i] = bkHioDefaultsA8[i];
    h.m0D0 = 0x5;
    h.m0D2 = 0x1;
    h.m0D4 = 0x1;
    h.m0D6 = 0x1;

    for (int i = 0; i < 10; i++)
        h.m0D8[i] = bkHioDefaultsD8[i];
    h.m100 = -60.0f;
    h.m018 = 0.7f;
    h.m104 = 0xc8;
    h.m106 = 0x1f4;
    h.m108 = 0x12c;
    h.m10A = 0x258;
    h.m010 = 0.0f;
    h.m014 = 400.0f;
    h.__vtbl = bkMain_hioVtable();
}

/* 0209F6C0 */
enemyfire_l* enemyfire_ct(enemyfire_l* i_this) {
    if (i_this == nullptr) {
        i_this = (enemyfire_l*)operator_new(0x22C);
        if (i_this == nullptr)
            return nullptr;
    }
    if (gabi::ea(&i_this->mDirection) ==
        0) { /* JGeometry::TVec3 constructor (GHS: allocates for NULL) */
        operator_new(0xC);
    }
    dCcD_Stts_ct(&i_this->mStts);
    dCcD_Sph_ct(&i_this->mSph);
    bkMain_initializeFireLight(i_this);
    return i_this;
}

/* 0209F74C */
bk_class* bk_class_ct(bk_class* i_this) {
    if (i_this == nullptr) {
        i_this = (bk_class*)operator_new(0x19A4);
        if (i_this == nullptr)
            return nullptr;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = bkMain_actorVtable();
    dPa_smokeEcallBack_ct(&i_this->m0350, 1);
    dPa_followEcallBack_ct((dPa_followEcallBack*)&i_this->mHdFollowCb, 0, 0);
    dBgS_AcchCir_ct(&i_this->dr.mAcchCir);
    dBgS_ObjAcch_ct(&i_this->dr.mAcch);
    dCcD_Stts_ct(&i_this->dr.mStts);
    dPa_smokeEcallBack_ct(&i_this->dr.mParticleCallBack, 1);
    dCcD_Cyl_ct(&i_this->m0B88, BK_AAB_VTBL);
    dCcD_Cyl_ct(&i_this->m0CB8, BK_AAB_VTBL);
    dCcD_Sph_ct(&i_this->m0DE8);
    dCcD_Sph_ct(&i_this->m0F14);
    dCcD_Sph_ct(&i_this->m1040);
    dCcD_Stts_ct(&i_this->mEnemyIce.mStts);
    dCcD_Cyl_ct(&i_this->mEnemyIce.mCyl, BK_AAB_VTBL);
    dBgS_AcchCir_ct(&i_this->mEnemyIce.mBgAcchCir);
    dBgS_ObjAcch_ct(&i_this->mEnemyIce.mBgAcch);
    enemyfire_ct(&i_this->mEnemyFire);
    return i_this;
}

/* 020A896C */
void bk_class_dt(bk_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Sph_dt(&i_this->mEnemyFire.mSph, 2);
        dCcD_Stts_dt(&i_this->mEnemyFire.mStts, 2);
        dBgS_ObjAcch_dt(&i_this->mEnemyIce.mBgAcch);
        dBgS_AcchCir_dt(&i_this->mEnemyIce.mBgAcchCir);
        dCcD_Cyl_dt(&i_this->mEnemyIce.mCyl, 2);
        dCcD_Stts_dt(&i_this->mEnemyIce.mStts, 2);
        dCcD_Sph_dt(&i_this->m1040, 2);
        dCcD_Sph_dt(&i_this->m0F14, 2);
        dCcD_Sph_dt(&i_this->m0DE8, 2);
        dCcD_Cyl_dt(&i_this->m0CB8, 2);
        dCcD_Cyl_dt(&i_this->m0B88, 2);
        dCcD_Stts_dt(&i_this->dr.mStts, 2);
        dBgS_ObjAcch_dt(&i_this->dr.mAcch);
        dBgS_AcchCir_dt(&i_this->dr.mAcchCir);
        bkMain_actorDestructor(i_this); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) {
            operator_delete(i_this);
        }
    }
}

/* 020A8A98 */
void SafeString_assureTerminationImpl(SafeString* s) {}

// clang-format off
/* WWHD glue
data head_sph_src 101916F4
data wepon_sph_src 10191734
data defence_sph_src 10191774
data co_cyl_src 101917E8
data tg_cyl_src 1019182C
data search_data 1019158C
data joint_check 10191534
data check_bit 10191568
data xad 1019156C
data zad 1019157C
data fire_sc 101917B4
data fire_j 101917DC
str 100095C8 "Bk" in 02098DA4
str 100095F8 "no < 20+1"
str 10009604 "d_a_bk.cpp" in 020991CC
str 10009610 "no2 < sizeof(joint_check) / sizeof(joint_check[0])" in 020991CC
str 10009650 "d_a_bk.cpp" in 02099584
str 1000965C "no2 < sizeof(joint_check) / sizeof(joint_check[0])" in 02099584
str 10009698 "boushi"
str 100096A0 "fuku"
str 100096A8 "SC_eye_MX"
str 1000978C "Bk" in 0209F164
str 1000978F "Bk" in 0209F220
str 10009794 "d_a_bk.cpp" in 0209F220
str 100097A0 "modelData != (0)"
str 100097CC "ITest63"
str 100097D4 "Bk" in 0209F918
str 100097D8 "GanonJ"
str 100097E0 "A_mori"
str 100097E8 "\u0083{\u0083R\u0082\u00bf\u0082\u00e1\u0082\u00f1"
*/
// clang-format on
