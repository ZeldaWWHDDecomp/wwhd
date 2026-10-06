/**
 * d_a_bk.cpp (WWHD)
 * Enemy - Bokoblin
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_bk_local.h"

/* 02098DA4 */
void anm_init(bk_class* i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 speed, int soundFileIdx) {
    WWHD_FUNC(0x02098DA4, void, i_this, bckFileIdx, morf, loopMode, speed, soundFileIdx);
    if (i_this->dr.mAction == 19 && bckFileIdx != dRes_INDEX_BK_BCK_BK_OTISOU1_e && bckFileIdx != dRes_INDEX_BK_BCK_BK_OTISOU2_e) {
        return;
    }
    if (soundFileIdx >= 0) {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x100095C8), bckFileIdx, SAFESTRING_VTBL);
        void* sound = dComIfG_getObjectRes(STR(0x100095C8), soundFileIdx, SAFESTRING_VTBL);
        i_this->mpMorf->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, sound);
    } else {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x100095C8), bckFileIdx, SAFESTRING_VTBL);
        i_this->mpMorf->setAnm(anm, loopMode, morf, speed, 0.0f, -1.0f, nullptr);
    }
}
VERIFY(0x02098DA4, anm_init);

/* 0209A624 */
void* s_w_sub(void* param_1, void*) {
    WWHD_FUNC(0x0209A624, void*, param_1, (void*)nullptr);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == fpcNm_BOKO_e) {
        fopAc_ac_c* boko = (fopAc_ac_c*)param_1;
        if (fopAcM_GetParam(boko) != 4 /* daBoko_c::Type_MOBLIN_SPEAR_e */ && !fopAcM_checkCarryNow(boko)) {
            s32 n = target_info_count();
            if (n < 10) {
                target_info_count() = n + 1;
                target_info()[n] = boko;
            }
        }
    }
    return nullptr;
}
VERIFY(0x0209A624, s_w_sub);

/* 0209A6AC */
void* s_b_sub(void* param_1, void*) {
    WWHD_FUNC(0x0209A6AC, void*, param_1, (void*)nullptr);
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
VERIFY(0x0209A6AC, s_b_sub);

/* 0209AE40 */
BOOL daBk_bomb_view_check(bk_class* i_this) {
    WWHD_FUNC(0x0209AE40, BOOL, i_this);
    fopAc_ac_c* bomb = search_bomb(i_this, 1);
    i_this->m11F8 = bomb;
    return bomb != nullptr ? TRUE : FALSE;
}
VERIFY(0x0209AE40, daBk_bomb_view_check);

/* 0209AE7C */
BOOL daBk_player_bg_check(bk_class* i_this, cXyz* r22) {
    WWHD_FUNC(0x0209AE7C, BOOL, i_this, r22);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    if (search_sp() != 0 || i_this->mType == 0xA) {
        return FALSE;
    }
    if (i_this->dr.m713 == 0 && std::fabs((f32)player->speedF) < 0.1f && daPy_getGrabWearTimer(player) < 0.0f) {
        return TRUE;
    }
    gabi::Local<dBgS_LinChk_l> linChk;
    dBgS_LinChk_ct(linChk);
    gabi::Local<cXyz> sp14;
    gabi::Local<cXyz> sp08;
    Vec3f p = r22->get();
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
VERIFY(0x0209AE7C, daBk_player_bg_check);

/* 0209B05C */
BOOL daBk_player_view_check(bk_class* i_this, cXyz* r30, s16 r27, s16 r31) {
    WWHD_FUNC(0x0209B05C, BOOL, i_this, r30, r27, r31);
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
    if (std::fabs((f32)sp08->x) < l_bkHIO().m03C &&
        std::fabs((f32)sp08->y) < l_bkHIO().m040 &&
        sp08->z > l_bkHIO().m048 &&
        sp08->z < l_bkHIO().m044
    ) {
        i_this->dr.m713 = 1;
        return TRUE;
    } else {
        i_this->dr.m713 = 0;
        return FALSE;
    }
}
VERIFY(0x0209B05C, daBk_player_view_check);

/* 0209B208 */
BOOL daBk_player_way_check(bk_class* i_this) {
    WWHD_FUNC(0x0209B208, BOOL, i_this);
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
VERIFY(0x0209B208, daBk_player_way_check);

/* 0209B264 */
void wait_set(bk_class* i_this) {
    WWHD_FUNC(0x0209B264, void, i_this);
    if (i_this->m0B30 != 0 || i_this->m11F3 != 0) {
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_KYORO1_e, 10.0f, 2 /* J3DFrameCtrl::EMode_LOOP */, 1.0f, dRes_INDEX_BK_BAS_BK_KYORO1_e);
    } else if (i_this->dr.mAction >= 4) {
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_KYORO1_e, 10.0f, 2, 1.0f, dRes_INDEX_BK_BAS_BK_KYORO1_e);
    } else {
        anm_init(i_this, dRes_INDEX_BK_BCK_BK_KYORO2_e, 10.0f, 2, 1.0f, dRes_INDEX_BK_BAS_BK_KYORO2_e);
    }
    i_this->m0300[1] = (s16)gabi::ftoi(40.0f + cM_rndF(60.0f));
}
VERIFY(0x0209B264, wait_set);

/* 0209F15C */
BOOL daBk_IsDelete(bk_class* i_this) {
    WWHD_FUNC(0x0209F15C, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0209F15C, daBk_IsDelete);

/* 0209F164 */
BOOL daBk_Delete(bk_class* i_this) {
    WWHD_FUNC(0x0209F164, BOOL, i_this);
    dPa_EcallBack_remove(&i_this->mHdFollowCb); /* HD */
    dComIfG_resDelete(&i_this->mPhase, STR(0x1000978C));
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
VERIFY(0x0209F164, daBk_Delete);

/* ---- local helpers (SHARED-CANDIDATE: HD J3D, as in d_a_kamome) ----
 * j3dSys.mModel at 0x104B462C, J3DSys::mCurrentMtx at 0x104B4868; a model's joint matrices live
 * in a block at +0x2C (+0x4 flags, +0x10 matrices); getAnmMtx marks the block dirty. */
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
    /* 0xAC */ be<u32> mModelData;
    /* 0xB0 */ u8 _B0[8];
    /* 0xB8 */ be<u32> mUserArea;
};
static Mtx34* bk_getAnmMtx(J3DModel* m, s32 jntNo) {
    J3DMtxBlock_l* blk = ((J3DModel_l*)m)->mpMtxBlock;
    blk->mFlags |= 0x10; /* HD: marks the joint matrices dirty */
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30);
}
enum { fpcNm_OBJ_SEARCH_e = 0x42, fpcNm_ITEM_e = 0xFF, dItemNo_DROPPED_SWORD_e = 0x3D };
static inline bool daObj_Search_getFindFlag() { return gabi::load<u8>(0x1046BE20) != 0; } /* daObj_Search::Act_c static */
static inline u8 daItem_getItemNo(void* item) { return gabi::load<u8>(gabi::ea(item) + 0x74E); }
static inline f32 REG7_F(int i) { return REG_F(7, i); }
static inline s16 REG7_S(int i) { return REG_S(7, i); }
static inline f32 REG8_F(int i) { return REG_F(8, i); }

/* 0209B67C */
void* ken_s_sub(void* param_1, void*) {
    WWHD_FUNC(0x0209B67C, void*, param_1, (void*)nullptr);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == fpcNm_ITEM_e) {
        if (daItem_getItemNo(param_1) == dItemNo_DROPPED_SWORD_e) {
            return param_1;
        }
    }
    return nullptr;
}
VERIFY(0x0209B67C, ken_s_sub);

/* 0209B6D8 */
void* s_s_sub(void* r29, void* r30) {
    WWHD_FUNC(0x0209B6D8, void*, r29, r30);
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
VERIFY(0x0209B6D8, s_s_sub);

/* 0209BA78 */
void* s_s2_sub(void* param_1, void*) {
    WWHD_FUNC(0x0209BA78, void*, param_1, (void*)nullptr);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == fpcNm_OBJ_SEARCH_e) {
        if (daObj_Search_getFindFlag()) {
            return param_1;
        }
    }
    return nullptr;
}
VERIFY(0x0209BA78, s_s2_sub);

/* 0209BAD8 */
void tate_mtx_set(bk_class* i_this) {
    WWHD_FUNC(0x0209BAD8, void, i_this);
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
VERIFY(0x0209BAD8, tate_mtx_set);

/* 0209BBD4 */
void bou_mtx_set(bk_class* i_this) {
    WWHD_FUNC(0x0209BBD4, void, i_this);
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
VERIFY(0x0209BBD4, bou_mtx_set);

/* dBgS_GndChk (stack object, 0x54): HD layout, this TU's vtables (SHARED-CANDIDATE layout) */
struct dBgS_GndChk_l {
    /* 0x00 */ be<u32> mpPolyPassChk; /* -> +0x40 */
    /* 0x04 */ be<u32> mpGrpPassChk;  /* -> +0x4C */
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ cXyz m_pos;
    /* 0x30 */ u8 _30[0x40 - 0x30];
    /* 0x40 */ be<u32> __vtbl_40;     /* dBgS_PolyPassChk */
    /* 0x44 */ be<u8> mPass[7];
    /* 0x4B */ u8 _4B;
    /* 0x4C */ be<u32> __vtbl_4C;     /* dBgS_GrpPassChk */
    /* 0x50 */ be<u32> mGrp;
};
WWHD_SIZE(dBgS_GndChk_l, 0x54);
static void bk_GndChk_ct(dBgS_GndChk_l* c) {
    gabi::call(0x02008E0C, c); /* cBgS_GndChk::cBgS_GndChk */
    c->__vtbl_10 = 0x100094BC;
    c->__vtbl_20 = 0x100094CC;
    for (int i = 0; i < 7; i++) c->mPass[i] = 0;
    c->__vtbl_4C = 0x100094DC;
    c->__vtbl_40 = 0x100094EC;
    c->mpPolyPassChk = gabi::ea(c) + 0x40;
    c->mpGrpPassChk = gabi::ea(c) + 0x4C;
    c->mGrp = 1;
}
static void bk_GndChk_dt(dBgS_GndChk_l* c) {
    c->__vtbl_20 = 0x100094CC;
    c->__vtbl_40 = 0x100094EC;
    c->__vtbl_4C = 0x100094AC;
    gabi::call(0x02008DAC, c, 0); /* cBgS_GndChk::~cBgS_GndChk */
}
static f32 bk_GroundCross(dBgS_GndChk_l* c) { return cBgS_GroundCross(dComIfG_Bgsp(), c); }
static inline void MtxRotY(f32 rad, u8 concat) { gabi::call(0x0200FBA4, rad, concat); }

/* 02099FF0 */
void way_pos_check(bk_class* i_this, cXyz* r31) {
    WWHD_FUNC(0x02099FF0, void, i_this, r31);
    fopAc_ac_c* i_actor = i_this;
    gabi::Local<dBgS_LinChk_l> linChk;
    dBgS_LinChk_ct(linChk);
    gabi::Local<dBgS_GndChk_l> gndChk;
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
VERIFY(0x02099FF0, way_pos_check);

/* 0209A29C */
u8 ground_4_check(bk_class* i_this, int r18, s16 r20, f32 f29) {
    WWHD_FUNC(0x0209A29C, u8, i_this, r18, r20, f29);
    be<f32>* xad = gabi::at<be<f32>>(0x1019156C);
    be<f32>* zad = gabi::at<be<f32>>(0x1019157C);
    be<u8>* check_bit = gabi::at<be<u8>>(0x10191568);
    gabi::Local<dBgS_GndChk_l> gndChk;
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
VERIFY(0x0209A29C, ground_4_check);

/* 0209A4BC */
BOOL daBk_other_bg_check(bk_class* i_this, fopAc_ac_c* r23) {
    WWHD_FUNC(0x0209A4BC, BOOL, i_this, r23);
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
VERIFY(0x0209A4BC, daBk_other_bg_check);

/* 0209A728 */
fopAc_ac_c* search_bomb(bk_class* i_this, BOOL r26) {
    WWHD_FUNC(0x0209A728, fopAc_ac_c*, i_this, r26);
    if (!(i_this->m1208 & 0x0200)) {
        return nullptr;
    }

    target_info_count() = 0;
    for (int i = 0; i < 10; i++) {
        target_info()[i] = nullptr;
    }

    fpcM_Search(0x0209A6AC /* s_b_sub */, i_this);

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
                !(daBk_other_bg_check(i_this, r24) && r26)
            ) {
                if (r26) {
                    /* GHS: `bgt` skips, so NaN passes */
                    if (!(std::fabs(r24->current.pos.y + 50.0f - i_this->eyePos.y) > l_bkHIO().m038)) {
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
                            std::fabs((f32)sp10->y) < l_bkHIO().m040 &&
                            sp10->z > l_bkHIO().m048 &&
                            sp10->z < l_bkHIO().m044
                        ) {
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
VERIFY(0x0209A728, search_bomb);

/* 0209AB64: daBk_wepon_view_check with search_wepon inlined (the matcher names it search_wepon).
 * HD: also returns FALSE while m02DC (the Bokoblin carries a torch/bouen) is set. */
static u32 search_wepon_inline(bk_class* i_this) {
    target_info_count() = 0;
    for (int i = 0; i < 10; i++) {
        target_info()[i] = nullptr;
    }

    fpcM_Search(0x0209A624 /* s_w_sub */, i_this);

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
                        std::fabs((f32)sp0C->y) < l_bkHIO().m040 &&
                        sp0C->z > l_bkHIO().m048 &&
                        sp0C->z < l_bkHIO().m044
                    ) {
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
BOOL daBk_wepon_view_check(bk_class* i_this) {
    WWHD_FUNC(0x0209AB64, BOOL, i_this);
    if (i_this->m02DC != 0 || i_this->m02CC != 0) { /* HD: m02DC */
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
VERIFY(0x0209AB64, daBk_wepon_view_check);

/* dPath / dPnt (d_path) */
struct dPnt_l {
    /* 0x0 */ u8 mArg[4];
    /* 0x4 */ cXyz m_position;
};
struct dPath_l {
    /* 0x0 */ be<u16> m_num;
    /* 0x2 */ u8 _2[6];
    /* 0x8 */ gptr<dPnt_l> m_points;
};
struct bk_u8x100 { be<u8> v[0x100]; };

/* 0209B354 */
void path_check(bk_class* i_this, u8 r19) {
    WWHD_FUNC(0x0209B354, void, i_this, r19);
    fopAc_ac_c* i_actor = i_this;

    if (i_this->ppd == nullptr) {
        return;
    }
    if (i_this->m0B30 == 0 && i_this->m11F3 == 0 && i_this->mType != 4 && i_this->mType != 10 && i_this->mType != 6) {
        return;
    }

    gabi::Local<bk_u8x100> sp90;
    gabi::Local<dBgS_LinChk_l> linChk;
    dBgS_LinChk_ct(linChk);
    gabi::Local<cXyz> sp18;
    *sp18 = i_this->current.pos.get();
    sp18->y += 100.0f;
    gabi::Local<cXyz> spc;

    dPath_l* ppd = (dPath_l*)(dPath*)i_this->ppd;
    dPnt_l* pnt = ppd->m_points;
    for (int i = 0; i < ((dPath_l*)(dPath*)i_this->ppd)->m_num; i++, pnt++) {
        spc->x = pnt->m_position.x;
        spc->y = pnt->m_position.y + 100.0f;
        spc->z = pnt->m_position.z;
        dBgS_LinChk_Set_l(linChk, sp18, spc, i_actor);
        sp90->v[i] = !LineCross(linChk);
    }

    f32 f0 = 0.0f;
    bool r6 = false;
    for (int i2 = 0; i2 < 100; i2++, f0 += 50.0f) {
        pnt = ((dPath_l*)(dPath*)i_this->ppd)->m_points;
        for (int j = 0; j < ((dPath_l*)(dPath*)i_this->ppd)->m_num; j++, pnt++) {
            if (sp90->v[j] == 0) {
                continue;
            }
            f32 distX = i_this->current.pos.x - pnt->m_position.x;
            f32 distY = i_this->current.pos.y - pnt->m_position.y;
            f32 distZ = i_this->current.pos.z - pnt->m_position.z;
            if (std_sqrtf(gabi::fmadds(distZ, distZ, gabi::fmadds(distX, distX, distY * distY))) < f0) {
                if (r19) {
                    i_this->m1216 = j;
                } else {
                    i_this->m1216 = j - i_this->m1217;
                    if (i_this->m1216 >= (s8)((dPath_l*)(dPath*)i_this->ppd)->m_num) {
                        i_this->m1216 = ((dPath_l*)(dPath*)i_this->ppd)->m_num;
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
VERIFY(0x0209B354, path_check);

/* JPABaseEmitter (HD offsets): the fields set inline here */
static inline void JPA_setRate(JPABaseEmitter* e, f32 r) { gabi::store<f32>(gabi::ea(e) + 0x34, r); }
static inline void JPA_setSpread(JPABaseEmitter* e, f32 s) { gabi::store<f32>(gabi::ea(e) + 0x58, s); }
static inline void JPA_setMaxFrame(JPABaseEmitter* e, s32 f) { gabi::store<s32>(gabi::ea(e) + 0x5C, f); }
/* setGlobalScale: HD sets the global scale (0x220) and the particle scale (0x238) */
static inline void JPA_setGlobalScale(JPABaseEmitter* e, f32 x, f32 y, f32 z) {
    u32 a = gabi::ea(e);
    gabi::store<f32>(a + 0x220, x); gabi::store<f32>(a + 0x224, y); gabi::store<f32>(a + 0x228, z);
    gabi::store<f32>(a + 0x238, x); gabi::store<f32>(a + 0x23C, y); gabi::store<f32>(a + 0x240, z);
}
static inline void JPA_setGlobalParticleScale(JPABaseEmitter* e, f32 x, f32 y, f32 z) {
    u32 a = gabi::ea(e);
    gabi::store<f32>(a + 0x238, x); gabi::store<f32>(a + 0x23C, y); gabi::store<f32>(a + 0x240, z);
}
static inline s32 dBgS_GetAttributeCode(void* polyInfo) { return gabi::call<s32>(0x024EF0F4, dComIfG_Bgsp(), polyInfo); }
enum { dBgS_Attr_GRASS_e = 4, dBgS_Attr_SAND_e = 0xB };
enum { dPa_ID_AK_JT_ELEMENTSMOKE00 = 0x2022, dPa_ID_AK_JN_ELEMENTKUSA00 = 0x24 };

/* 02098EEC */
void smoke_set_s(bk_class* i_this, f32 rate) {
    WWHD_FUNC(0x02098EEC, void, i_this, rate);
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
        *endPos = gabi::at<cXyz>(gabi::ea(linChk.get()) + 0x30)->get(); /* GetCross() */
        i_this->m0338.y = endPos->y;
        attribCode = dBgS_GetAttributeCode(gabi::at<u8>(gabi::ea(linChk.get()) + 0x14));
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
        JPABaseEmitter* emitter1 = dPa_control_set(dComIfGp_getParticle(), 2, dPa_ID_AK_JT_ELEMENTSMOKE00, &i_this->m0338,
                                                   &i_this->m0344, nullptr, 0xB9, (dPa_levelEcallBack*)&i_this->m0350,
                                                   roomNo, nullptr, nullptr, nullptr);
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
        JPABaseEmitter* emitter2 = dComIfGp_particle_set(dPa_ID_AK_JN_ELEMENTKUSA00, &i_this->m0338, &i_this->m0344);
        if (emitter2) {
            JPA_setRate(emitter2, rate * 0.5f);
            JPA_setMaxFrame(emitter2, 3);
        }
        break;
    }
    }
    dBgS_LinChk_dt(linChk);
}
VERIFY(0x02098EEC, smoke_set_s);

static inline s8 joint_check(s32 i) { return gabi::load<s8>(0x10191534 + i); } /* s8[0x34] */
static inline J3DModel* j3dSys_getModel() { return gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); }
static inline Mtx34* J3DSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
static inline void bk_setAnmMtx(J3DModel* m, s32 jntNo, Mtx34* src) { mtx_copy(bk_getAnmMtx(m, jntNo), src); }

/* 020991CC. HD: debug asserts on the joint number and the joint_check index */
BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x020991CC, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(node);
        u32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        if (!(jntNo < 0x34)) {
            JUT_ASSERT_fail(STR(0x10009604), 0x5AA, STR(0x10009610)); /* "no2 < sizeof(joint_check) / ..." */
            if (!(jntNo < 0x34)) return TRUE;
        }
        J3DModel* model = j3dSys_getModel();
        bk_class* i_this = gabi::at<bk_class>(((J3DModel_l*)model)->mUserArea);
        int r28 = joint_check(jntNo);
        if (i_this) {
            PSMTXCopy(bk_getAnmMtx(model, jntNo), calc_mtx());
            if (jntNo == BK_JNT_AGO_e) {
                cMtx_ZrotM(calc_mtx(), i_this->m11F4);
                bk_setAnmMtx(model, jntNo, calc_mtx());
                PSMTXCopy(calc_mtx(), J3DSys_mCurrentMtx());
            } else {
                if (!(r28 < 20 + 1)) {
                    JUT_ASSERT_fail(STR(0x10009604), 0x5BD, STR(0x100095F8)); /* "no < 20+1" */
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
                    cXyz* attnPos = gabi::at<cXyz>(gabi::ea(i_this) + 0x390); /* attention_info.position */
                    *attnPos = i_this->eyePos.get();
                    attnPos->y += l_bkHIO().m024;
                    if (l_bkHIO().m009 == 0) {
                        i_this->m0330 = cM_atan2s(sp08->x - i_this->eyePos.x, sp08->z - i_this->eyePos.z);
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
VERIFY(0x020991CC, nodeCallBack);

/* 02099584. HD: debug assert on the joint number */
BOOL nodeCallBack_P(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x02099584, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DJoint* joint = J3DNode_toJoint(node);
        u32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        if (!(jntNo < 0x34)) {
            JUT_ASSERT_fail(STR(0x10009650), 0x60C, STR(0x1000965C));
            if (!(jntNo < 0x34)) return TRUE;
        }
        J3DModel* model = j3dSys_getModel();
        bk_class* i_this = gabi::at<bk_class>(((J3DModel_l*)model)->mUserArea);
        int r30 = joint_check(jntNo);
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
VERIFY(0x02099584, nodeCallBack_P);

/* 020A070C */
void ground_smoke_set(bk_class* i_this) {
    WWHD_FUNC(0x020A070C, void, i_this);
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
            Vec3f p;
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
VERIFY(0x020A070C, ground_smoke_set);

/* ---- daBk_Draw (HD: no blob shadow, no material-table variants; the eye/hat/body shapes are
 * found by material name) ---- */
static inline s8 dComIfGp_roomControl_getStayNo() { return gabi::load<s8>(0x1047E6C8); }
static inline s16 REG6_S(int i) { return REG_S(6, i); }
static inline s16 REG8_S(int i) { return REG_S(8, i); }
static inline void dMat_ice_entryDL(mDoExt_McaMorf* morf, s32 p, void* p2) { gabi::call(0x0259138C, morf, p, p2); }
/* 027F3F94 (matcher: __nw): self-relative pointer at *this + 0xC (the joint table header; +8 joint count) */
static inline u32 J3DModelData_jointTable(u32 md) { return gabi::call<u32>(0x027F3F94, md); }
static inline u32 J3DModel_modelData(J3DModel* m) { return gabi::load<u32>(gabi::ea(m) + 0xAC); }
/* sead::Buffer-style element access: out-of-range indices give element 0 */
static inline u32 bk_bufAt(u32 base, u32 count, u32 i, u32 size) { return i < count ? base + i * size : base; }
/* 020A8A98: this TU's copy of SafeString::assureTerminationImpl_ (empty virtual), called through
 * a pointer before the string is used */
void SafeString_assureTerminationImpl(SafeString* s) {
    WWHD_FUNC(0x020A8A98, void, s);
}
VERIFY(0x020A8A98, SafeString_assureTerminationImpl);
/* J3DModelData: material by name (JUTNameTab::getIndex 027DF9B0), its shape (+8) */
static u32 bk_getMaterialShape(u32 md, const char* name) {
    u32 res = gabi::load<u32>(md);
    gabi::Local<SafeString> key;
    key->__vtbl = SAFESTRING_VTBL;
    key->mStringTop = gabi::ea(name);
    gabi::call_ptr(0x020A8A98, key.get());
    u32 off = gabi::load<u32>(res + 0x18);
    u32 tab = off != 0 ? res + 0x18 + off : 0;
    s32 idx = gabi::call<s32>(0x027DF9B0, tab, key->mStringTop.get());
    u32 mat;
    if (idx >= 0) {
        u32 count = gabi::load<u32>(md + 0xC);
        u32 base = gabi::load<u32>(md + 0x10);
        mat = bk_bufAt(base, count, (u32)idx, 0x39C);
    } else {
        mat = 0;
    }
    return gabi::load<u32>(mat + 8);
}
static inline void J3DShape_show(u32 shape) { gabi::store<u8>(shape + 4, 1); }
static inline void J3DShape_hide(u32 shape) { gabi::store<u8>(shape + 4, 0); }
/* dComIfGd_setListMaskOff (setList is shared): j3dSys draw buffers (0x104B4634/38) from the play object's lists */
static inline void dComIfGd_setListMaskOff() {
    gabi::store<u32>(0x104B4634, gabi::load<u32>(dComIfGp_ea() + 0x5D84));
    gabi::store<u32>(0x104B4638, gabi::load<u32>(dComIfGp_ea() + 0x5D88));
}
enum { DSNAP_TYPE_BK = 0xAA };

static inline void br_draw(bk_class* i_this) {
    if (i_this->m02F0 == 0) {
        return;
    }
    if (REG6_S(3) != 0) {
        return;
    }

    J3DModel* model = i_this->m02E8;
    for (u16 i = 0; i < gabi::load<u16>(J3DModelData_jointTable(J3DModel_modelData(model)) + 8); i++) {
        u32 md = J3DModel_modelData(model);
        u32 joint = bk_bufAt(gabi::load<u32>(md + 8), gabi::load<u32>(md + 4), i, 0x1C);
        u32 mat = gabi::load<u32>(joint + 0x10); /* getMesh() */
        while (mat) {
            u32 shape = gabi::load<u32>(mat + 8);
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
            mat = gabi::load<u32>(mat + 4); /* getNext() */
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

/* 0209971C */
BOOL daBk_Draw(bk_class* i_this) {
    WWHD_FUNC(0x0209971C, BOOL, i_this);
    J3DModel* model = i_this->mpMorf->getModel();
    if (i_this->m02B7 != 0xFF && i_this->mType == 6 && dComIfGs_isSwitch(i_this->m02B7, dComIfGp_roomControl_getStayNo())) {
        return TRUE;
    }
    if (i_this->m02BA != 0 || i_this->mType == 8 || i_this->m121C != 0 || i_this->m02DE != 0) {
        return TRUE;
    }

    setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
    if (i_this->mEnemyIce.mFreezeTimer > 20) {
        dMat_ice_entryDL(i_this->mpMorf, -1, nullptr);
        /* HD: no daBk_shadowDraw */
        return TRUE;
    }

    br_draw(i_this);

    search_check_draw(i_this);

    u32 md = J3DModel_modelData(model);
    u32 rootJoint = gabi::load<u32>(md + 8);
    u32 eyeShape = bk_getMaterialShape(md, STR(0x100096A8));  /* "SC_eye_MX" */
    u32 hatShape = bk_getMaterialShape(md, STR(0x10009698));  /* "boushi" */
    u32 bodyShape = bk_getMaterialShape(md, STR(0x100096A0)); /* "fuku" */
    J3DShape_hide(eyeShape);
    mDoExt_btpAnm* btp = i_this->m02C4;
    s16 frame = (s16)gabi::ftoi(gabi::load<f32>(gabi::ea(btp) + 4));
    mDoExt_btpAnm_entry(btp, gabi::at<J3DModelData>(J3DModel_modelData(model)), frame);
    i_this->mpMorf->entryDL(); /* HD: no material-table variant (m1230) */
    dComIfGd_setListMaskOff();
    J3DShape_show(eyeShape);
    /* HD: no getMatPacket(0)->unlock() */
    J3DShape_hide(hatShape);
    J3DShape_hide(bodyShape);
    gabi::call(0x027F583C, model, rootJoint); /* rootJoint->entryIn() (HD: with the model) */
    J3DShape_show(hatShape);
    J3DShape_show(bodyShape);
    dComIfGd_setList();

    /* HD: no daBk_shadowDraw */

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
VERIFY(0x0209971C, daBk_Draw);

/* ---- useHeapInit ---- */
static inline void* bk_getRes(s32 idx) { return dComIfG_getObjectRes(STR(0x1000978F), idx, SAFESTRING_VTBL); }
static inline mDoExt_btpAnm* mDoExt_btpAnm_ct(void* p) { return gabi::call<mDoExt_btpAnm*>(0x025E7820, p); }
static inline BOOL mDoExt_btpAnm_init(mDoExt_btpAnm* a, J3DModelData* md, void* btp, BOOL anmPlay, s32 loopMode, f32 speed,
                                      s16 startF, s16 endF, bool modify, BOOL entry) {
    return gabi::call<BOOL>(0x025E789C, a, md, btp, anmPlay, loopMode, speed, startF, endF, modify, entry);
}
static inline JntHit_c* JntHit_create(J3DModel* m, u32 data, s16 num) { return gabi::call<JntHit_c*>(0x02552B60, m, data, num); }
/* HD resource indices of the Bk archive (HD has separate models instead of material tables) */
enum {
    dRes_HD_BK_BDL_BK_PINK = 0x61, dRes_HD_BK_BDL_BK_GREEN = 0x60,
    dRes_HD_BK_BMD_KB_KEN = 0x5F, dRes_HD_BK_BMD_KB_BOKO = 0x5E,
};

/* 0209F220. HD: the body model is chosen by colour (pink / green / normal BDL) instead of a
 * material table, and the weapon blur model by weapon (two BMDs instead of setMaterialTable). */
BOOL useHeapInit(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x0209F220, BOOL, i_actor);
    bk_class* i_this = (bk_class*)i_actor;

    J3DModelData* bodyData;
    if (i_this->m02DC != 0) {
        bodyData = (J3DModelData*)bk_getRes(dRes_HD_BK_BDL_BK_PINK);
    } else if (i_this->m02D4 != 0) {
        bodyData = (J3DModelData*)bk_getRes(dRes_HD_BK_BDL_BK_GREEN);
    } else {
        bodyData = (J3DModelData*)bk_getRes(dRes_INDEX_BK_BDL_BK_e);
    }
    if (bodyData == nullptr) {
        JUT_ASSERT_fail(STR(0x10009794), 0x256F, STR(0x100097A0)); /* modelData != 0 */
    }
    J3DAnmTransform* anm = (J3DAnmTransform*)bk_getRes(dRes_INDEX_BK_BCK_BK_SUWARI_e);
    void* bas = bk_getRes(dRes_INDEX_BK_BAS_BK_SUWARI_e);
    i_this->mpMorf = mDoExt_McaMorf::create(nullptr, bodyData, nullptr, nullptr, anm, 2 /* EMode_LOOP */, 1.0f, 0, -1, 1,
                                            bas, 0x00080000, 0x37221203);
    if (i_this->mpMorf == nullptr || i_this->mpMorf->getModel() == nullptr) {
        return FALSE;
    }

    J3DModel* model = i_this->mpMorf->getModel();
    for (u16 i = 0; i < gabi::load<u16>(J3DModelData_jointTable(J3DModel_modelData(model)) + 8); i++) {
        if (i != 0 && !(i < 0x34)) { /* HD: JUT_ASSERT on the joint_check index (no report) */
            continue;
        }
        s32 r3 = joint_check(i);
        if (r3 < 0) {
            continue;
        }
        u32 md = J3DModel_modelData(model);
        u32 joint = bk_bufAt(gabi::load<u32>(md + 8), gabi::load<u32>(md + 4), i, 0x1C);
        if (r3 == 0x0E || r3 == 0x0F || r3 == 0x10 || r3 == 0x11 || r3 == 0x14) {
            gabi::store<u32>(joint + 8, 0x02099584 /* nodeCallBack_P */);
        } else {
            gabi::store<u32>(joint + 8, 0x020991CC /* nodeCallBack */);
        }
    }

    /* HD: no m1230 material tables */

    void* p = operator_new(0x74);
    i_this->m02C4 = p ? mDoExt_btpAnm_ct(p) : nullptr;
    if (i_this->m02C4 == nullptr) {
        return cPhs_ERROR_e; /* GameCube bug kept: a phase state from a BOOL function */
    }
    void* btp = bk_getRes(dRes_INDEX_BK_BTP_TMABATAKI_e);
    if (!mDoExt_btpAnm_init(i_this->m02C4, gabi::at<J3DModelData>(J3DModel_modelData(model)), btp, TRUE, 0 /* EMode_NONE */,
                            1.0f, 0, -1, false, FALSE)) {
        return cPhs_ERROR_e;
    }

    J3DModelData* modelData;
    if (i_this->m02D5 & 0x40) {
        modelData = (J3DModelData*)bk_getRes(dRes_HD_BK_BMD_KB_KEN);
    } else {
        modelData = (J3DModelData*)bk_getRes(dRes_HD_BK_BMD_KB_BOKO);
    }
    if (modelData == nullptr) {
        JUT_ASSERT_fail(STR(0x10009794), 0x25FC, STR(0x100097A0));
    }
    i_this->m02E8 = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (i_this->m02E8 == nullptr) {
        return cPhs_ERROR_e;
    }
    /* setBaseScale(scale) */
    cXyz* baseScale = gabi::at<cXyz>(gabi::ea(i_this->m02E8.get()) + 0xBC);
    *baseScale = i_this->scale.get();

    if (i_this->m02D4 != 0) {
        modelData = (J3DModelData*)bk_getRes(dRes_INDEX_BK_BMD_BK_TATE_e);
        i_this->m02D0 = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        if (modelData == nullptr) {
            JUT_ASSERT_fail(STR(0x10009794), 0x261D, STR(0x100097A0));
        }
    }

    if (i_this->m02DC != 0) {
        modelData = (J3DModelData*)bk_getRes(dRes_INDEX_BK_BDL_BOUEN_e);
        i_this->m02D8 = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
        if (modelData == nullptr) {
            JUT_ASSERT_fail(STR(0x10009794), 0x2626, STR(0x100097A0));
        }
    }

    i_this->mpJntHit = JntHit_create(i_this->mpMorf->getModel(), 0x1019158C /* search_data */, 15);
    if (i_this->mpJntHit) {
        i_this->jntHit = gabi::ea(i_this->mpJntHit.get());
    } else {
        return FALSE;
    }

    return TRUE;
}
VERIFY(0x0209F220, useHeapInit);

/* ---- compiler-generated / out-of-line inline copies of this TU ---- */

/* 020A0634: this TU's out-of-line fopAcM_SearchByID */
fopAc_ac_c* fopAcM_SearchByID_bk(u32 id) {
    WWHD_FUNC(0x020A0634, fopAc_ac_c*, id);
    return fopAcM_SearchByID(id);
}
VERIFY(0x020A0634, fopAcM_SearchByID_bk);

/* 020A0670: this TU's out-of-line fopAcM_monsSeStart (HD inline: null checks on the actor and
 * &eyePos; mDoAud_monsSeStart 025E1AA4(id, pos, procId, param, reverb)) */
void fopAcM_monsSeStart_bk(fopAc_ac_c* a, u32 id, u32 param) {
    WWHD_FUNC(0x020A0670, void, a, id, param);
    if (a != nullptr && gabi::ea(&a->eyePos) != 0) {
        s8 room = fopAcM_GetRoomNo(a);
        u32 pid = fopAcM_GetID(a);
        s32 reverb = dComIfGp_getReverb(room);
        gabi::call(0x025E1AA4, id, &a->eyePos, pid, param, reverb);
    }
}
VERIFY(0x020A0670, fopAcM_monsSeStart_bk);

/* 020A06F8: bkHIO_c deleting destructor (trivial body) */
void bkHIO_c_dt(bkHIO_c* i_this, s32 flags) {
    WWHD_FUNC(0x020A06F8, void, i_this, flags);
    if (i_this != nullptr && (flags & 1)) {
        operator_delete(i_this);
    }
}
VERIFY(0x020A06F8, bkHIO_c_dt);

/* 020A0364: __sinit_d_a_bk_cpp (the matcher names it bkHIO_c::bkHIO_c): the per-TU header statics
 * (same sequence as sinit_header_statics, but this TU's .bss objects are not laid out as
 * P/P+8/P+9/P+0xC), then l_bkHIO's inline constructor (no destructor registered). */
void __sinit_d_a_bk_cpp() {
    WWHD_FUNC(0x020A0364, void, (u32)0);
    for (int i = 0; i < 4; i++) gabi::store<u32>(0x10462358 + 4 * i, 0);
    __register_global_object(0x10191870);
    gabi::store<f32>(0x10462324, -3.1415927f);
    gabi::store<f32>(0x10462328, 3.1415927f);
    gabi::call(0x028ED6F8, 0x1046232E);
    __register_global_object(0x1019187C);
    gabi::call(0x028EAB2C, 0x1046232F);
    __register_global_object(0x10191888);

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
    static const f32 a8[10] = {1.0f, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    for (int i = 0; i < 10; i++) h.m0A8[i] = a8[i];
    h.m0D0 = 0x5;
    h.m0D2 = 0x1;
    h.m0D4 = 0x1;
    h.m0D6 = 0x1;
    static const f32 d8[10] = {1.0f, 1.0f, 1.0f, 0.8f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    for (int i = 0; i < 10; i++) h.m0D8[i] = d8[i];
    h.m100 = -60.0f;
    h.m018 = 0.7f;
    h.m104 = 0xc8;
    h.m106 = 0x1f4;
    h.m108 = 0x12c;
    h.m10A = 0x258;
    h.m010 = 0.0f;
    h.m014 = 400.0f;
    h.__vtbl = 0x1000956C; /* bkHIO_c vtable */
}
VERIFY(0x020A0364, __sinit_d_a_bk_cpp);

/* ---- constructors / destructor ---- */
#define BK_AAB_VTBL 0x1000948C /* this TU's cM3dGAab vtable */
static inline void dPa_smokeEcallBack_ct(void* p, u8 a) { gabi::call(0x025A5B18, p, a); }
static inline void dCcD_Sph_ct(dCcD_Sph* s) { gabi::call(0x025166F0, s); }
/* dBgS_ObjAcch::dBgS_ObjAcch (inline): dBgS_Acch, then the vtables and a flag (this TU's copies) */
static inline void dBgS_ObjAcch_ct(dBgS_ObjAcch* a) {
    gabi::call(0x024F0474, a); /* dBgS_Acch::dBgS_Acch */
    u32 p = gabi::ea(a);
    gabi::store<u32>(p + 0x10, 0x100094FC);
    gabi::store<u32>(p + 0x14, 0x1000951C);
    gabi::store<u32>(p + 0x20, 0x1000950C);
    gabi::store<u8>(p + 0x18, 1);
}
static inline void dBgS_ObjAcch_dt(dBgS_ObjAcch* a) {
    u32 p = gabi::ea(a);
    gabi::store<u32>(p + 0x20, 0x1000950C);
    gabi::store<u32>(p + 0x14, 0x1000951C);
    gabi::call(0x024EFD9C, a, 0); /* dBgS_Acch::~dBgS_Acch */
}
/* dBgS_AcchCir::~dBgS_AcchCir (inline): its cM3dGCir member (+0x14) */
static inline void dBgS_AcchCir_dt(dBgS_AcchCir* c) { gabi::call(0x02018034, gabi::ea(c) + 0x14, 2); }
static inline void dCcD_Sph_dt(dCcD_Sph* s, s32 flags) { gabi::call(0x02515AE8, s, flags); }

/* 0209F6C0: enemyfire::enemyfire (compiler-generated; HD LIGHT_INFLUENCE +0x20 = 1.0f) */
enemyfire_l* enemyfire_ct(enemyfire_l* i_this) {
    WWHD_FUNC(0x0209F6C0, enemyfire_l*, i_this);
    if (i_this == nullptr) {
        i_this = (enemyfire_l*)operator_new(0x22C);
        if (i_this == nullptr) return nullptr;
    }
    if (gabi::ea(&i_this->mDirection) == 0) { /* JGeometry::TVec3 constructor (GHS: allocates for NULL) */
        operator_new(0xC);
    }
    dCcD_Stts_ct(&i_this->mStts);
    dCcD_Sph_ct(&i_this->mSph);
    gabi::store<f32>(gabi::ea(i_this) + 0x228, 1.0f);
    return i_this;
}
VERIFY(0x0209F6C0, enemyfire_ct);

/* 0209F74C: bk_class::bk_class (compiler-generated; HD: member constructors and vtables) */
bk_class* bk_class_ct(bk_class* i_this) {
    WWHD_FUNC(0x0209F74C, bk_class*, i_this);
    if (i_this == nullptr) {
        i_this = (bk_class*)operator_new(0x19A4);
        if (i_this == nullptr) return nullptr;
    }
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = 0x1000957C; /* bk_class vtable */
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
VERIFY(0x0209F74C, bk_class_ct);

/* 020A896C: bk_class deleting destructor (compiler-generated) */
void bk_class_dt(bk_class* i_this, s32 flags) {
    WWHD_FUNC(0x020A896C, void, i_this, flags);
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
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) {
            operator_delete(i_this);
        }
    }
}
VERIFY(0x020A896C, bk_class_dt);

/* ---- daBk_Create ---- */
/* HD strcmp(dComIfGp_getStartStageName(), name) == 0: SafeString compare (the virtual at vtable
 * +0x14 twice on the literal, once on the stage name at play+0x5134, then at most 0x40001 bytes) */
static inline void bk_SafeString_vcall(SafeString* s) { gabi::call_ptr(gabi::load<u32>(s->__vtbl + 0x14), s); }
static bool bk_isStartStage(const char* name) {
    gabi::Local<SafeString> a;
    a->__vtbl = SAFESTRING_VTBL;
    a->mStringTop = gabi::ea(name);
    u32 stage = dComIfGp_ea() + 0x5134;
    gabi::Local<SafeString> b;
    b->mStringTop = stage;
    b->__vtbl = SAFESTRING_VTBL;
    bk_SafeString_vcall(a);
    bk_SafeString_vcall(a);
    u32 pa = a->mStringTop;
    bk_SafeString_vcall(b);
    u32 pb = b->mStringTop;
    if (pa == pb) return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = gabi::load<u8>(pa + i);
        u8 cb = gabi::load<u8>(pb + i);
        if (ca != cb) return false;
        if (ca == 0) return true;
    }
    return false;
}
static inline void fopAcM_SetMin(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D672C, a, x, y, z); }
static inline void fopAcM_SetMax(fopAc_ac_c* a, f32 x, f32 y, f32 z) { gabi::call(0x025D673C, a, x, y, z); }
static inline s32 dComIfGp_CharTbl_GetNameIndex(const char* name, s32 p) {
    return gabi::call<s32>(0x0200E814, gabi::at<u8>(dComIfGp_ea() + PLAY_NAMETBL), name, p);
}
static inline be<u32>& ken() { return *gabi::at<be<u32>>(0x1046231C); }
enum { fopAcStts_HD_4000 = 0x4000, fopAcStts_BOSS_e = 0x4000000 };

/* 0209F918 */
cPhs_State daBk_Create(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x0209F918, cPhs_State, i_actor);
    /* fopAcM_ct(i_actor, bk_class) */
    if (!fopAcM_CheckCondition(i_actor, fopAcCnd_INIT_e)) {
        if (i_actor != nullptr) {
            bk_class_ct((bk_class*)i_actor);
        }
        fopAcM_OnCondition(i_actor, fopAcCnd_INIT_e);
    }
    bk_class* i_this = (bk_class*)i_actor;

    cPhs_State phase_state = dComIfG_resLoad(&i_this->mPhase, STR(0x100097D4));
    if (phase_state == cPhs_COMPLEATE_e) {
        i_actor->gbaName = 1;

        if (bk_isStartStage(STR(0x100097CC) /* "ITest63" */) || bk_isStartStage(STR(0x100097D8) /* "GanonJ" */)) {
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

        i_actor->itemTableIdx = dComIfGp_CharTbl_GetNameIndex(STR(0x100097D4), 0);

        if (!fopAcM_entrySolidHeap(i_actor, 0x0209F220 /* useHeapInit */, 0x17B20)) {
            return cPhs_ERROR_e;
        }

        if (!hio_set()) {
            l_bkHIO().mNo = mDoHIO_createChild(STR(0x100097E8) /* "Boko-chan" */, &l_bkHIO());
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
        gabi::store<u32>(gabi::ea(i_this->mpMorf->getModel()) + 0xB8, gabi::ea(i_this)); /* setUserArea */
        i_this->mBtHeight = 162.5f; /* initBt */
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

        /* HD */
        i_actor->actor_status |= fopAcStts_HD_4000;
        i_this->m02DF = 5;

        if (i_this->mType == 4 || i_this->mType == 0xA) {
            i_this->dr.mAction = 1;
            if (i_this->mType == 0xA) {
                i_this->m02DF = 0; /* HD */
                i_this->dr.mMode = -20;
                i_actor->actor_status |= fopAcStts_BOSS_e;
                /* USA/HD: no search_sp = 1 */
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
            anm_init(i_this, dRes_INDEX_BK_BCK_BK_HAKOBI_e, 1.0f, 2 /* EMode_LOOP */, 1.0f, -1);
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

        if (i_this->mType != 8 && i_this->m02DC == 0 && !bk_isStartStage(STR(0x100097E0) /* "A_mori" */)) {
            u32 weaponType;
            if (i_this->m02D5 & 0x40) {
                weaponType = 1;
            } else if (i_this->m02D5 & 0x80) {
                weaponType = 7;
            } else {
                weaponType = 0;
            }
            i_this->m1200 = fopAcM_create(fpcNm_BOKO_e, weaponType, &i_actor->current.pos, fopAcM_GetRoomNo(i_actor), nullptr,
                                          nullptr, -1, 0);
            i_this->m1214 = 1;
            i_this->m02D5 &= 0x40;
        } else {
            i_this->m11F3 = 1;
        }

        i_this->dr.mAcch.Set(&i_actor->current.pos, &i_actor->old.pos, i_this, 1, &i_this->dr.mAcchCir, &i_actor->speed);
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
        i_this->m0B88.Set(gabi::at<dCcD_SrcCyl>(0x101917E8) /* co_cyl_src */);
        i_this->m0B88.SetStts(&i_this->dr.mStts);
        i_this->m0CB8.Set(gabi::at<dCcD_SrcCyl>(0x1019182C) /* tg_cyl_src */);
        i_this->m0CB8.SetStts(&i_this->dr.mStts);
        i_this->m0DE8.Set(gabi::at<dCcD_SrcSph>(0x101916F4) /* head_sph_src */);
        i_this->m0DE8.SetStts(&i_this->dr.mStts);
        i_this->m1040.Set(gabi::at<dCcD_SrcSph>(0x10191734) /* wepon_sph_src */);
        i_this->m1040.SetStts(&i_this->dr.mStts);
        i_this->m0F14.Set(gabi::at<dCcD_SrcSph>(0x10191774) /* defence_sph_src */);
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
            i_this->mEnemyFire.mFlameJntIdxs[i] = gabi::load<u8>(0x101917DC + i);      /* fire_j */
            i_this->mEnemyFire.mParticleScale[i] = gabi::load<f32>(0x101917B4 + 4 * i); /* fire_sc */
        }

        i_actor->stealItemLeft = 3;

        daBk_Execute(i_this);
    }

    return phase_state;
}
VERIFY(0x0209F918, daBk_Create);
