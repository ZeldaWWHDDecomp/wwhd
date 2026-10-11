/**
 * d_a_sitem.cpp (WWHD)
 * Enemy item: a hand-held object (pot, barrel, ...) hanging on a chain, dropped and broken
 * when its chain is cut.
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_sitem.cpp) has only "Nonmatching" stubs for this unit (and no layout), so
 * every function here is written from the WWHD code (cking.rpx) and verified against it.
 */
#include "d/actor/d_a_sitem.h"

const gc_tu_data gc_tu = {.safeString = 0x1003CD4C};

static u16 l_bmd_idx[4] = {4, 5, 6, 0};
static f32 l_hang_y[4] = {100.0f, 250.0f, 400.0f, 600.0f};
static f32 l_swing[4] = {10.0f, 25.0f, 40.0f, 65.0f};
static f32 l_seg_ofs_y[10] = {50.0f, 50.0f, 35.0f, 25.0f, 15.0f, 9.0f, 6.0f, 6.0f, 6.0f, 6.0f};
static f32 l_seg_size[10] = {10.0f, 10.0f, 9.5f, 9.0f, 8.5f, 8.0f, 7.5f, 7.0f, 6.5f, 6.5f};
static GXColor l_chain_color = {0, 180, 50, 255};
static GXColor l_chain2_color = {0, 180, 50, 255};

static const char* M_arcname = "Sitem";
static SitemSphereSource tg_sph_src = {0x0,  0x800, 0x0, 0x0,  0x0,  0x0,  0x5,  0xff1dfebf, 0x3,
                                       0x75, 0x0,   0x0, 0x0,  0x0,  0x0,  0x0,  0x0,        0x0,
                                       0x0,  0x2,   0x0, 0.0f, 0.0f, 0.0f, 20.0f};
static SitemSphereSource bm_sph_src = {0x0, 0x0, 0x0, 0x0,  0x0,  0x0,  0x0,  0x200040, 0x3,
                                       0x0, 0x0, 0x0, 0x0,  0x0,  0x0,  0x0,  0x0,      0x0,
                                       0x0, 0x2, 0x0, 0.0f, 0.0f, 0.0f, 80.0f};
#define SAFESTRING_VTBL 0x1003CD4C
#define SITEM_VTBL 0x1003CDE4 /* sitem_class vtable (HD virtual destructor) */

/* 02486390 */
static BOOL daSitem_Draw(sitem_class* i_this) {

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->eyePos, &i_this->tevStr);
    if (i_this->mMode < 6) {
        J3DModel* model = i_this->mpModel;
        setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
        mDoExt_modelUpdateDL(model);
    }
    mDoExt_3DlineMat0_update2(&i_this->mChain, 10, &l_chain_color, &i_this->tevStr);
    dComIfGd_set3DlineMat(reinterpret_cast<mDoExt_3DlineMat1_c*>(&i_this->mChain));
    if (i_this->mCutChainLength > 0.1f) {
        mDoExt_3DlineMat0_update2(&i_this->mChain2, 5, &l_chain2_color, &i_this->tevStr);
        dComIfGd_set3DlineMat(reinterpret_cast<mDoExt_3DlineMat1_c*>(&i_this->mChain2));
    }
    return TRUE;
}

/* 024864A0 */
void hand_mtx_set(sitem_class* i_this) {

    MtxTrans(i_this->mHandPos.x, i_this->mHandPos.y, i_this->mHandPos.z, 0);
    mDoMtx_XrotM(calc_mtx(), i_this->mHandAngX);
    mDoMtx_YrotM(calc_mtx(), i_this->mHandAngY);
    mDoMtx_XrotM(calc_mtx(), 0x4000);
    gabi::Local<cXyz> off;
    off->x = 0.0f;
    off->y = -60.0f;
    off->z = 0.0f;
    MtxPosition(off, &i_this->mHandCenter);
    MtxTrans(0.0f, -50.0f, 0.0f, 1);
    J3DModel_setBaseTRMtx(i_this->mpModel, calc_mtx());
}

/* 024865CC */
void my_break(sitem_class* i_this) {

    i_this->mTimer[0] = 50;
    i_this->mMode = 6;
    gabi::Local<csXyz> angle;
    csXyz_ct(angle, 0, i_this->current.angle.y, 0);
    fopAcM_createItemFromTable(&i_this->mPos, i_this->mItemTbl, i_this->mItemBitNo & 0x7F,
                               i_this->home.roomNo, 0, angle, 1, nullptr);

    u8 type = i_this->mType;
    GXColor* color = &i_this->tevStr.mColorK0; /* tevStr.mColorK0 */
    if (type <= 1) {
        dComIfGp_particle_set(0x8168, &i_this->mPos);
        dComIfGp_particle_set(0x8167, &i_this->mPos, nullptr, nullptr, 0xFF, nullptr, -1, color,
                              color);
        type = i_this->mType;
        if (type == 1) {
            dComIfGp_particle_set(0x8169, &i_this->mPos, nullptr, nullptr, 0xFF, nullptr, -1, color,
                                  color);
            type = i_this->mType;
        }
    }
    if (type == 2)
        dComIfGp_particle_set(0x816A, &i_this->mPos, nullptr, nullptr, 0xFF, nullptr, -1, color,
                              color);

    fopAcM_seStart(i_this, 0x6936, 0);
    fopAcM_create(0x1D3, 4, &i_this->mPos, -1, nullptr, nullptr, -1, 0);
}

/* 02487E94 */
static BOOL daSitem_IsDelete(sitem_class*) { return TRUE; }

/* 02487E9C */
static BOOL daSitem_Delete(sitem_class* i_this) {

    dComIfG_resDelete(&i_this->mPhs, M_arcname);
    for (int i = 0; i < 2; i++) {
        /* mFollow[i].remove() (virtual, slot 0x44) */
        sitemFollowRemove(&i_this->mFollow[i]);
    }
    return TRUE;
}

// HD: useHeapInit, also the solid heap callback (daSitem_solidHeapCB merged)
/* 02487F00 */
static BOOL useHeapInit(sitem_class* i_this) {

    s32 idx = l_bmd_idx[i_this->mType];
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname, idx, SAFESTRING_VTBL);
    J3DModel* model = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    i_this->mpModel = model;
    if (model == nullptr)
        return FALSE;
    if (!mDoExt_3DlineMat0_init(&i_this->mChain, 1, 10, TRUE))
        return FALSE;
    if (!mDoExt_3DlineMat0_init(&i_this->mChain2, 1, 5, TRUE))
        return FALSE;
    return TRUE;
}

// HD: dPa_followEcallBack array element constructor (__construct_array)
/* 02487FD4 */
static u32 followEcallBack_ct(dPa_followEcallBack* p) { return sitemFollowConstruct(p); }

// HD: sitem_class::sitem_class (inline in fopAcM_ct; HD: allocates when this == NULL)
/* 02487FE0 */
static sitem_class* sitem_class_ct(sitem_class* self) {

    if (self == nullptr) {
        self = (sitem_class*)operator_new(sizeof(sitem_class));
        if (self == nullptr)
            return self;
    }
    fopAc_ac_c_ct(self);
    self->__vtbl = SITEM_VTBL;
    mDoExt_3DlineMat0_ct(&self->mChain);
    mDoExt_3DlineMat0_ct(&self->mChain2);
    dCcD_Stts_ct(&self->mStts);
    sitemConstructArray(gabi::ea(self->mSph), 4, sizeof(dCcD_Sph), SITEM_SPHERE_CONSTRUCTOR);
    sitemSphereConstruct(&self->mBmSph);
    dBgS_AcchCir_ct(&self->mAcchCir);
    dBgS_ObjAcch_ct(&self->mAcch, sitemAcchVt);
    sitemConstructArray(gabi::ea(self->mFollow), 2, sizeof(dPa_followEcallBack),
                        gabi::guest_fn(reinterpret_cast<const void*>(followEcallBack_ct)));
    return self;
}

/* 024880E0 */
static cPhs_State daSitem_Create(fopAc_ac_c* a_this) {

    sitem_class* i_this = (sitem_class*)a_this;
    /* fopAcM_ct(i_this, sitem_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr)
            sitem_class_ct(i_this);
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    cPhs_State phase = dComIfG_resLoad(&i_this->mPhs, M_arcname);
    if (phase != cPhs_COMPLEATE_e)
        return phase;

    u8 type = fopAcM_GetParam(a_this) & 0xFF;
    i_this->mType = (type == 0xFF) ? 0 : type;
    u32 p = (fopAcM_GetParam(a_this) >> 8) & 0xFF;
    i_this->mSizeType = (p <= 3) ? p : 0;
    p = fopAcM_GetParam(a_this) >> 24;
    i_this->mItemTbl = (p == 0xFF) ? 0 : p;
    i_this->mItemBitNo = (fopAcM_GetParam(a_this) >> 16) & 0xFF;
    if (!fopAcM_entrySolidHeap(a_this, gabi::guest_fn(reinterpret_cast<const void*>(useHeapInit)), 0x3040))
        return cPhs_ERROR_e;

    /* HD: a static cXyz {0, 30000, -20000} (role unknown) */
    sitemFarPosition()->x = 0.0f;
    sitemFarPosition()->y = 30000.0f;
    sitemFarPosition()->z = -20000.0f;
    a_this->health = 2;
    i_this->mCount = (s16)gabi::ftoi(cM_rndF(10000.0f));
    i_this->mStts.Init(0xFF, 0xFF, a_this);
    for (int i = 0; i < 4; i++) {
        dCcD_Sph* sph = &i_this->mSph[i];
        sph->Set(reinterpret_cast<dCcD_SrcSph*>(&tg_sph_src));
        sph->SetStts(&i_this->mStts);
        if (i_this->mType != 1)
            sph->OffAtSPrmBit(1); /* OffAtSPrmBit(cCcD_AtSPrm_Set_e) */
    }
    i_this->mBmSph.Set(reinterpret_cast<dCcD_SrcSph*>(&bm_sph_src));
    i_this->mBmSph.SetStts(&i_this->mStts);
    i_this->mPos.copy(a_this->current.pos);
    i_this->mAcch.Set(&i_this->mPos, &i_this->mOldPos, a_this, 1, &i_this->mAcchCir,
                      &a_this->speed);
    i_this->mAcchCir.SetWall(30.0f, 50.0f);
    for (int i = 2; i != 0; i--)
        sitemExecuteCall(i_this);
    return phase;
}

/* 02488348 */
static void __sinit_d_a_sitem_cpp() { sinit_header_statics(0x1046DEBC, 0x101D08A4); }

// HD: sead::SafeString deleting destructor (this TU's copy; trivial)
/* 024883DC */
static void SafeString_dt(void* p, s32 flags) {

    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

// HD: dPa_followEcallBack array element deleting destructor (__destroy_arr; trivial)
/* 024883F0 */
static void followEcallBack_dt(void* p, s32 flags) {

    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

// HD: sitem_class deleting destructor (compiler-generated, HD virtual destructor)
/* 02488404 */
static void sitem_class_dt(sitem_class* i_this, s32 flags) {

    if (i_this != nullptr) {
        sitemDestroyArray(gabi::ea(i_this->mFollow), 2, sizeof(dPa_followEcallBack),
                          gabi::guest_fn(reinterpret_cast<const void*>(followEcallBack_dt)));
        /* ~dBgS_ObjAcch: this TU's vtables, then ~dBgS_Acch */
        i_this->mAcch.dBgS_GrpPassChk::__vtbl = 0x1003CDC4;
        i_this->mAcch.dBgS_PolyPassChk::__vtbl = 0x1003CDD4;
        sitemAcchDestroy(&i_this->mAcch);
        sitemCircleDestroy(&i_this->mAcchCir.m_cir);
        sitemSphereDestroy(&i_this->mBmSph);
        sitemDestroyArray(gabi::ea(i_this->mSph), 4, sizeof(dCcD_Sph), SITEM_SPHERE_DESTRUCTOR);
        dCcD_Stts_dt(&i_this->mStts, 2);
        mDoExt_3DlineMat0_dt(&i_this->mChain2, 2);
        mDoExt_3DlineMat0_dt(&i_this->mChain, 2);
        sitemActorDestroy(i_this);
        if (flags & 1)
            operator_delete(i_this);
    }
}

// HD: empty virtual in this TU's sead::SafeString vtable
/* 024884F8 */
static void SafeString_empty(void* p) {}

/* ======================================================================================
 * daSitem_Execute (024867D0, 5.8 KB). The GameCube unit has hand_move, control1/2/3 and
 * cut_control1/2 as separate (stub) functions; WWHD inlines all of them here. The helpers
 * below follow the WWHD code's blocks; GHS duplicated the shared tails (addCalc2 x3, the
 * ground-check destructor) several times.
 * ====================================================================================== */
static const dBgS_GndChk_vt GNDCHK_VT = {0x1003CD74, 0x1003CD84, 0x1003CDA4, 0x1003CD94};

static inline cXyz* seg(sitem_class* i, int k) { return &i->mChainSeg[k].mPos; }
static inline cXyz* seg2(sitem_class* i, int k) { return &i->mChain2Seg[k].mPos; }
static inline be<f32>& segSize(cXyz* s) { return reinterpret_cast<SitemChainSegment*>(s)->mSize; }
static inline s16 neg_s16(s16 v) { return (s16) - (s32)v; }

/* the two angles of a direction: y from (x, z), x from (y, |xz|) */
static inline void dir_angles(f32 x, f32 y, f32 z, s16* ay, s16* ax) {
    *ay = cM_atan2s(x, z);
    f32 r = std_sqrtf(gabi::fmadds(x, x, z * z));
    *ax = neg_s16(cM_atan2s(y, r));
}

/* hand_move: the item's own movement by mode; returns `fall` (mode 5/6) */
static inline bool hand_move(sitem_class* i, cXyz* target, dBgS_GndChk* gnd) {
    f32 hangY = l_hang_y[i->mSizeType];
    f32 calcTarget = l_swing[i->mSizeType];
    f32 mSwingAmplitudeTarget = sitemDebug()->mSwingAmplitude + 5.0f;
    f32 calcStep = 1.0f;
    bool fall = false;
    mDoMtx_YrotS(calc_mtx(), i->current.angle.y);
    mDoMtx_XrotM(calc_mtx(), i->current.angle.x);
    s32 mode = i->mMode;
    if ((u32)mode <= 1) {
        s16 cnt = i->mCount;
        gabi::Local<cXyz> ofs, rot, res;
        ofs->y = hangY;
        ofs->x = cM_ssin((s16)(cnt * 600)) * 25.0f;
        ofs->z = cM_ssin((s16)(cnt * 700)) * 25.0f;
        MtxPosition(ofs, rot);
        cXyz_pl(&i->current.pos, res, rot);
        f32 z = res->z, x = res->x, y = res->y;
        target->y = y;
        target->x = x;
        target->z = z;
        if (i->mMode == 0) {
            i->mPos.x = x;
            i->mPos.y = y;
            i->mMode = 1;
            i->mPos.z = z;
        }
    } else if (mode == 5) {
        i->mOldPos.copy(i->mPos);
        i->mSwingAmplitude = 50.0f;
        mSwingAmplitudeTarget = 50.0f;
        calcTarget = 25.0f;
        fall = true;
        PSVECAdd(&i->mPos, &i->speed, &i->mPos);
        f32 vy = i->speed.y - 3.0f;
        if (vy < -90.0f)
            vy = -90.0f;
        i->speed.y = vy;
        i->mAcch.CrrPos(dComIfG_Bgsp());
        f32 sy = i->speed.y;
        i->mTimer2 = 5;
        if (sy < 0.0f) {
            f32 y = i->mPos.y, z = i->mPos.z, x = i->mPos.x;
            cXyz* gpos = &gnd->m_pos;
            gpos->z = z;
            gpos->x = x;
            gpos->y = y + 100.0f;
            f32 gy = cBgS_GroundCross(dComIfG_Bgsp(), gnd);
            i->mGroundY = gy;
            if (gy == -1000000000.0f) {
                i->mPos.y = gy + 30.0f;
                my_break(i);
            } else {
                f32 g = gy + 30.0f;
                if (!(i->mPos.y > g)) {
                    i->mPos.y = g;
                    my_break(i);
                }
            }
        }
    } else if (mode == 6) {
        i->mTimer2 = 10;
        fall = true;
        mSwingAmplitudeTarget = 0.0f;
        s16 t = i->mTimer[0];
        if (t < 40) {
            calcStep = calcTarget * 0.05f;
            calcTarget = 0.0f;
            if (t == 0)
                fopAcM_delete(i);
        }
    }
    cLib_addCalc2(&i->mChainLength, calcTarget, 0.5f, calcStep);
    cLib_addCalc2(&i->mChainDrift, 0.0f, 1.0f, 0.2f);
    cLib_addCalc2(&i->mSwingAmplitude, mSwingAmplitudeTarget, 1.0f, 1.5f);
    return fall;
}

/* the hand: position at the last chain link, angles from the second-to-last one */
static inline void hand_set(sitem_class* i) {
    i->mHandPos.copy(*seg(i, 9));
    gabi::Local<cXyz> res;
    cXyz_mi(seg(i, 8), res, seg(i, 9));
    f32 x = res->x, y = res->y, z = res->z;
    i->mHandAngX = neg_s16(cM_atan2s(y, z));
    f32 r = std_sqrtf(gabi::fmadds(y, y, z * z));
    i->mHandAngY = cM_atan2s(x, r);
    hand_mtx_set(i);
}

/* control1 / control2: the chain while the item hangs (pulled towards the target) */
static inline void control1(sitem_class* i, cXyz* target) {
    cLib_addCalc2(&i->speedF, 8.0f, 1.0f, 0.1f);
    if (i->mWindPush > 1.0f && i->mMode != 3) {
        mDoMtx_YrotS(calc_mtx(), i->mWindAngle);
        gabi::Local<cXyz> v, off;
        v->y = 0.0f;
        v->z = i->mWindPush;
        v->x = 0.0f;
        MtxPosition(v, off);
        PSVECAdd(target, off, target);
        f32 s = i->mWindPush * 0.2f;
        if (s > 30.0f)
            i->speedF = 30.0f;
        else
            i->speedF = s;
    }
    cLib_addCalc0(&i->mWindPush, 1.0f, 5.0f);
    cLib_addCalc2(&i->mPos.x, target->x, 0.1f, i->speedF);
    cLib_addCalc2(&i->mPos.y, target->y, 0.1f, i->speedF);
    cLib_addCalc2(&i->mPos.z, target->z, 0.1f, i->speedF);

    seg(i, 0)->copy(i->current.pos);
    mDoMtx_YrotS(calc_mtx(), i->current.angle.y);
    mDoMtx_XrotM(calc_mtx(), i->current.angle.x);
    gabi::Local<cXyz> len, dir;
    len->x = 0.0f;
    len->y = 0.0f;
    len->z = i->mChainDrift;
    MtxPosition(len, dir);
    f32 step = i->mSwingAmplitude;
    len->z = i->mChainLength;
    gabi::Local<cXyz> p, q, res;
    for (int k = 1; k <= 8; k++) {
        s16 cnt = i->mCount;
        p->x = cM_ssin((s16)(cnt * 0x44C + 4000 * k)) * step;
        p->y = l_seg_ofs_y[k];
        p->z = cM_scos((s16)(cnt * 800 + 4000 * k)) * step;
        MtxPosition(p, q);
        cXyz* prev = seg(i, k - 1);
        cXyz* cur = seg(i, k);
        f32 dx = ((cur->x - prev->x) + dir->x) + q->x;
        f32 dy = ((cur->y - prev->y) + dir->y) + q->y;
        f32 dz = ((cur->z - prev->z) + dir->z) + q->z;
        s16 ay, ax;
        dir_angles(dx, dy, dz, &ay, &ax);
        MtxPush();
        mDoMtx_YrotS(calc_mtx(), ay);
        mDoMtx_XrotM(calc_mtx(), ax);
        MtxPosition(len, p);
        MtxPull();
        cXyz_pl(prev, res, p);
        cur->copy(*res);
    }

    /* control2: from the item back up the chain */
    seg(i, 9)->copy(i->mPos);
    gabi::Local<cXyz> len2, d2, res2;
    len2->y = 0.0f;
    len2->x = 0.0f;
    len2->z = i->mChainLength;
    for (int k = 8; k >= 1; k--) {
        cXyz* cur = seg(i, k);
        cXyz* next = seg(i, k + 1);
        f32 dx = cur->x - next->x;
        f32 dz = cur->z - next->z;
        f32 dy = cur->y - next->y;
        s16 ay, ax;
        dir_angles(dx, dy, dz, &ay, &ax);
        mDoMtx_YrotS(calc_mtx(), ay);
        mDoMtx_XrotM(calc_mtx(), ax);
        len2->z = i->mChainLength;
        MtxPosition(len2, d2);
        cXyz_pl(next, res2, d2);
        cur->copy(*res2);
    }
    hand_set(i);
}

/* follow effect at a chain end */
static inline void follow_set(sitem_class* i, int n, cXyz* pos, csXyz* angle, cXyz* a, cXyz* b) {
    dPa_followEcallBack* cb = &i->mFollow[n];
    if (cb->mpEmitter == nullptr && i->mFollowActive != 0) {
        dPa_followEcallBack_end(cb);
        s8 room = i->current.roomNo;
        dPa_control_c* pa = dComIfGp_getParticle();
        dPa_control_set(pa, 0, 0x8184, pos, angle, nullptr, 0xFF,
                        gabi::at<dPa_levelEcallBack>(gabi::ea(cb)), room, nullptr, nullptr,
                        nullptr);
        if (n == 0)
            i->mTimer[1] = 60;
    } else {
        gabi::Local<cXyz> res;
        cXyz_mi(a, res, b);
        f32 x = res->x, y = res->y, z = res->z;
        angle->y = cM_atan2s(x, z);
        f32 r = std_sqrtf(gabi::fmadds(x, x, z * z));
        angle->x = neg_s16(cM_atan2s(y, r));
        if (i->mTimer[1] == 1 && cb->mpEmitter != nullptr) {
            dPa_followEcallBack_end(cb);
            if (n == 1)
                i->mFollowActive = 0;
        }
    }
}

/* cut_control1 / cut_control2: the cut chain (falling or broken item) */
static inline void cut_control(sitem_class* i) {
    seg2(i, 0)->copy(i->current.pos);
    mDoMtx_YrotS(calc_mtx(), i->current.angle.y);
    mDoMtx_XrotM(calc_mtx(), i->current.angle.x);
    gabi::Local<cXyz> len, p, q, res;
    len->y = 0.0f;
    len->x = 0.0f;
    len->z = i->mCutChainLength;
    for (int k = 1; k <= 4; k++) {
        s16 cnt = i->mCount;
        p->x = cM_ssin((s16)(cnt * 0x1004 - 10000 * k)) * 50.0f;
        p->y = sitemDebug()->mCutChainBias + 50.0f;
        p->z = cM_scos((s16)(cnt * 0x1130 - 10000 * k)) * 50.0f;
        MtxPosition(p, q);
        cXyz* prev = seg2(i, k - 1);
        cXyz* cur = seg2(i, k);
        f32 dx = (cur->x - prev->x) + q->x;
        f32 dz = (cur->z - prev->z) + q->z;
        f32 dy = (cur->y - prev->y) + q->y;
        s16 ay, ax;
        dir_angles(dx, dy, dz, &ay, &ax);
        MtxPush();
        mDoMtx_YrotS(calc_mtx(), ay);
        mDoMtx_XrotM(calc_mtx(), ax);
        MtxPosition(len, p);
        MtxPull();
        cXyz_pl(prev, res, p);
        cur->copy(*res);
    }
    follow_set(i, 0, seg2(i, 4), &i->mFollowAngle[0], seg2(i, 4), seg2(i, 3));

    /* the chain left on the holder hangs down from the item */
    seg(i, 9)->copy(i->mPos);
    gabi::Local<cXyz> len2, d2, res2;
    len2->x = 0.0f;
    len2->y = 0.0f;
    len2->z = i->mChainLength;
    f32 step = i->mSwingAmplitude;
    s32 ra = 24000, rb = 32000, rc = 28000;
    for (int k = 9; k >= 1; k--) {
        cXyz* cur = seg(i, k - 1);
        cXyz* next = seg(i, k);
        s16 cnt = i->mCount;
        f32 y = gabi::fmadds(cM_ssin((s16)(cnt * 0xB86 - rb)), step, cur->y - 10.0f);
        f32 ground = i->mGroundY + 5.0f;
        f32 dx = gabi::fmadds(cM_ssin((s16)(cnt * 0x9C4 - ra)), step, cur->x - next->x);
        if (y < ground)
            y = ground;
        f32 dz = (cur->z - next->z) + cM_scos((s16)(cnt * 0xAF0 - rc)) * step;
        f32 dy = y - next->y;
        s16 ay, ax;
        dir_angles(dx, dy, dz, &ay, &ax);
        mDoMtx_YrotS(calc_mtx(), ay);
        mDoMtx_XrotM(calc_mtx(), ax);
        len2->z = i->mChainLength;
        MtxPosition(len2, d2);
        cXyz_pl(next, res2, d2);
        cur->copy(*res2);
        rc -= 3500;
        ra -= 3000;
        rb -= 4000;
    }
    hand_set(i);
    follow_set(i, 1, seg(i, 0), &i->mFollowAngle[1], seg(i, 0), seg(i, 1));

    /* the cut chain's links into the second line material */
    SitemLine* lines = i->mChain2.mpLines;
    be<u8>* sizes = lines->mpSize;
    cXyz* pos = lines->mpSegments;
    for (int k = 0; k < 5; k++) {
        pos[k].copy(*seg2(i, k));
        sizes[k] = (u8)gabi::ftoi(static_cast<f32>(segSize(seg2(i, k))));
    }
    cLib_addCalc0(&i->mCutChainLength, 1.0f, 1.0f);
}

/* control3: link sizes and the line material, eye position, colliders */
static inline void control3(sitem_class* i, bool fall) {
    for (int k = 0; k < 10; k++) {
        s16 cnt = i->mCount;
        f32 f = gabi::fmadds(cM_ssin((s16)(cnt * 500 + 100 * k)), 0.1f, 0.8f);
        segSize(seg(i, k)) = l_seg_size[k] * f;
    }
    SitemLine* lines = i->mChain.mpLines;
    be<u8>* sizes = lines->mpSize;
    cXyz* pos = lines->mpSegments;
    for (int k = 0; k < 10; k++) {
        pos[k].copy(*seg(i, k));
        sizes[k] = (u8)gabi::ftoi(static_cast<f32>(segSize(seg(i, k))));
    }
    pos = i->mChain.mpLines->mpSegments;
    u32 ex = sitemWord(pos[5].x), ey = sitemWord(pos[5].y), ez = sitemWord(pos[5].z);
    sitemWord(i->eyePos.x) = ex;
    sitemWord(i->eyePos.y) = ey;
    sitemWord(i->eyePos.z) = ez;
    sitemWord(i->attention_info.position.x) = ex; /* attention_info.position */
    sitemWord(i->attention_info.position.y) = ey;
    sitemWord(i->attention_info.position.z) = ez;

    i->mStts.Move();
    i->mBmSph.SetC(fall ? sitemFarPosition() : &i->eyePos);
    dComIfG_Ccsp_Set(&i->mBmSph);
    gabi::Local<cXyz> c;
    for (int k = 0; k < 3; k++) {
        u32 n = (u32)(2 * k + (i->mCount & 3));
        n = n - ((n * 0x6667) >> 18) * 10;
        c->copy(pos[n]);
        dCcD_Sph* sph = &i->mSph[k];
        sph->SetC(fall ? sitemFarPosition() : (cXyz*)c);
        if (i->mMode == 3)
            sph->OffCoSPrmBit(1);
        else
            sph->OnCoSPrmBit(1);
        dComIfG_Ccsp_Set(sph);
    }
    i->mSph[3].SetR(50.0f);
    i->mSph[3].SetC(&i->mHandCenter);
    i->mSph[3].OnCoSPrmBit(1);
    dComIfG_Ccsp_Set(&i->mSph[3]);
}

static inline void gndchk_dt(dBgS_GndChk* gnd) {
    gnd->dBgS_PolyPassChk::__vtbl = 0x1003CDA4;
    gnd->cBgS_PolyInfo::__vtbl = 0x1003CD84;
    gnd->dBgS_GrpPassChk::__vtbl = 0x1003CD64;
    sitemGroundDestroy(gnd);
}

/* the cut chain takes over the upper five links */
static inline void chain_cut(sitem_class* i) {
    for (int k = 0; k < 5; k++) {
        seg2(i, k)->copy(*seg(i, k));
        segSize(seg2(i, k)) = static_cast<f32>(segSize(seg(i, k)));
        if (k == 4) {
            gabi::Local<cXyz> d;
            cXyz_mi(seg2(i, 4), d, seg2(i, 3));
            i->mCutChainLength = std_sqrtf(PSVECSquareMag(d)) * 1.5f;
        }
    }
}

/* hits: returns FALSE for the early return (blown away by wind) */
static inline BOOL hit_check(sitem_class* i, CcAtInfo_sitem* at) {
    at->mpObj = 0;
    if (i->mTimer2 != 0)
        return TRUE;
    BOOL chainHit = FALSE;
    dCcD_Sph* hand = &i->mSph[3];
    if (hand->ChkTgHit() || (i->mWindPush > 100.0f && hand->ChkCoHit())) {
        at->mpObj = gabi::ea(hand->GetTgHitObj());
        if (at->mpObj != 0 && (gabi::at<dCcD_GObjInf>(at->mpObj)->mAtType & 0x08000000))
            return TRUE;
        my_break(i);
    } else {
        bool hit = false;
        for (int k = 0; k < 3; k++) {
            if (i->mSph[k].ChkTgHit()) {
                at->mpObj = gabi::ea(i->mSph[k].GetTgHitObj());
                if (at->mpObj != 0 && (gabi::at<dCcD_GObjInf>(at->mpObj)->mAtType & 0x08000000))
                    return TRUE;
                chainHit = TRUE;
                hit = true;
                break;
            }
        }
        if (!hit && !i->mBmSph.ChkTgHit())
            return TRUE;
        if (i->mTimer2 != 0) {
            if (!chainHit)
                return TRUE;
        } else {
            i->mTimer2 = 20;
            if (!chainHit) {
                at->mpObj = gabi::ea(i->mBmSph.GetTgHitObj());
                sitemAttackPowerCheck(at);
                if (at->mResultingAttackType == 8) {
                    i->mWindPush = 300.0f;
                    i->mWindAngle =
                        (s16)(fopAcM_searchActorAngleY(i, dComIfGp_getPlayer(0)) + 0x8000);
                    return FALSE;
                }
            }
            i->mMode = 5;
            i->speed.x = cM_rndFX(10.0f);
            i->speed.y = cM_rndF(5.0f) + 10.0f;
            i->speed.z = cM_rndFX(10.0f);
            gabi::Local<cXyz> sc;
            sc->x = 0.3f;
            sc->y = 0.3f;
            sc->z = 0.3f;
            dComIfGp_particle_set(0x16, &i->eyePos, nullptr, sc);
            i->mFollowActive = 1;
        }
    }
    if (at->mpObj != 0)
        sitemDefenseSound(i, at->mpObj, 0x21);
    chain_cut(i);
    return TRUE;
}

/* 024867D0 */
static BOOL daSitem_Execute(sitem_class* i_this) {

    i_this->mCount = i_this->mCount + 1;
    for (int k = 0; k < 2; k++) {
        if (i_this->mTimer[k] != 0)
            i_this->mTimer[k] = i_this->mTimer[k] - 1;
    }
    if (i_this->mTimer2 != 0)
        i_this->mTimer2 = i_this->mTimer2 - 1;

    dComIfGp_get();
    gabi::Local<dBgS_GndChk> gnd;
    dBgS_GndChk_ct(gnd, GNDCHK_VT, false);

    gabi::Local<cXyz> target;
    bool fall = hand_move(i_this, target, gnd);
    if (!fall) {
        control1(i_this, target);
    } else {
        cut_control(i_this);
    }
    control3(i_this, fall);

    gabi::Local<CcAtInfo_sitem> at;
    BOOL ok = hit_check(i_this, at);
    gndchk_dt(gnd);
    (void)ok;
    return TRUE;
}

// clang-format off
/* WWHD glue
data tg_sph_src 101D0824
data bm_sph_src 101D0864
data l_bmd_idx 101D078C
data l_hang_y 101D07CC
data l_swing 101D07BC
data l_seg_ofs_y 101D07FC
data l_seg_size 101D0794
data l_chain_color 101D0784
data l_chain2_color 101D0788
str 1003CD44 "Sitem" in 02487F00
str 1003CE4C "Sitem" in 02487E9C
str 1003CE60 "Sitem" in 024880E0
*/
// clang-format on
