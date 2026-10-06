/**
 * d_a_nz.cpp (WWHD)
 * Enemy - Rat
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_nz.cpp) has only "Nonmatching" stubs for this actor: the functions here are
 * written from the WWHD code (cking.rpx), with the GameCube names and signatures, and verified
 * against it.
 */
#include "d/actor/d_a_nz.h"

#define NZ_SAFESTRING_VTBL 0x100242E4 /* this TU's sead::SafeString vtable */
#define NZ_VTBL 0x1002442C            /* nz_class vtable (HD virtual destructor) */
#define NZ_HIO_VTBL 0x1002441C
#define NZ_AAB_VTBL 0x100242FC        /* this TU's cM3dGAab vtable */

enum { PROC_ITEM = 0xFF, PROC_BOMB = 0x126, PROC_NZ_HOLE = 0xC7 };

/* ---- file statics ---- */
static inline daNZ_HIO_c& l_nzHIO() { return *gabi::at<daNZ_HIO_c>(0x10468CF4); }
static inline be<s32>& obj_count() { return *gabi::at<be<s32>>(0x10468CE4); }
static inline be<u32>* obj_list() { return gabi::at<be<u32>>(0x10468D1C); } /* [100] */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void mDoMtx_XrotS(Mtx34* m, s16 x) { gabi::call(0x025F18EC, m, x); }
static inline csXyz* csXyz_ct_r(csXyz* p, s16 x, s16 y, s16 z) { return gabi::call<csXyz*>(0x0201A478, p, x, y, z); }
/* daBomb_c / daItem_c */
static inline BOOL daBomb_getBombCheck_Flag(void* b) { return gabi::call<BOOL>(0x020CB678, b); }
static inline void daBomb_setBombCheck_Flag(void* b) { gabi::call(0x020CB6A8, b); }
static inline void daBomb_setBombFire_ON(void* b) { gabi::call(0x020CB6DC, b); }
static inline void daBomb_setBombOffCoSet(void* b) { gabi::call(0x020CB768, b); }
static inline void daBomb_setBombOnCoSet(void* b) { gabi::call(0x020CB7A0, b); }
static inline BOOL daItem_checkLock(void* i) { return gabi::call<BOOL>(0x021831AC, i); }
static inline void daItem_setLock(void* i) { gabi::call(0x02183204, i); }
static inline void daItem_0218319C(void* i) { gabi::call(0x0218319C, i); } /* unnamed (releases the lock?) */
static inline void daItem_02183250(void* i) { gabi::call(0x02183250, i); } /* unnamed */
static inline void cc_at_check_nz(fopAc_ac_c* a, void* info) { gabi::call(0x025192A8, a, info); }
static inline void mDoAud_monsSeStart_nz(u32 id, cXyz* pos, u32 pid, u32 param, s32 reverb) {
    gabi::call(0x025E1AA4, id, pos, pid, param, reverb);
}
static inline void nz_mons_se_start(nz_class* i_this, u32 id) {
    cXyz* eye = &i_this->eyePos;
    if (gabi::ea(eye) != 0) {
        s8 roomNo = fopAcM_GetRoomNo(i_this);
        u32 pid = i_this != nullptr ? gabi::load<u32>(gabi::ea(i_this) + 4) : 0xFFFFFFFFu;
        mDoAud_monsSeStart_nz(id, eye, pid, 0, dComIfGp_getReverb(roomNo));
    }
}

/* dBgS_LinChk on the stack (this TU's vtables) */
static inline void nz_LinChk_ct(dBgS_LinChk_l* c) {
    cBgS_LinChk_ct(c);
    for (int i = 0; i < 7; i++) c->mPass[i] = 0;
    c->mpPolyPassChk = gabi::ea(c) + 0x58;
    c->__vtbl_10 = 0x1002439C;
    c->__vtbl_64 = 0x100243BC;
    c->mpGrpPassChk = gabi::ea(c) + 0x64;
    c->__vtbl_20 = 0x100243AC;
    c->__vtbl_58 = 0x100243CC;
    c->mGrp = 1;
}
static inline void nz_LinChk_dt(dBgS_LinChk_l* c) {
    c->__vtbl_58 = 0x100243CC;
    c->__vtbl_20 = 0x1002430C;
    c->__vtbl_64 = 0x1002431C;
    cBgS_LinChk_dt(c, 0);
}
/* J3D (HD): j3dSys.mModel at 0x104B462C, J3DSys::mCurrentMtx at 0x104B4868 */
static inline J3DModel* j3dSys_getModel() { return gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); }
static inline nz_class* model_userArea(J3DModel* m) { return gabi::at<nz_class>(gabi::load<u32>(gabi::ea(m) + 0xB8)); }

/* 0230A30C */
static BOOL nodeCallBack_tail(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x0230A30C, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DJoint* joint = J3DNode_toJoint(node);
        s32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        J3DModel* model = j3dSys_getModel();
        int idx = jntNo == 9 ? 1 : 0;
        nz_class* i_this = model_userArea(model);
        if (i_this != nullptr) {
            PSMTXCopy(getAnmMtx(model, jntNo), calc_mtx());
            gabi::Local<cXyz> zero;
            zero->z = 0.0f;
            zero->y = 0.0f;
            zero->x = 0.0f;
            MtxPosition(zero, &i_this->mTailEnd[idx]);
        }
    }
    return TRUE;
}
VERIFY(0x0230A30C, nodeCallBack_tail);

/* 0230A3C0: the tail follows the body (a chain of 10 points) */
static void tail_control(nz_class* i_this) {
    WWHD_FUNC(0x0230A3C0, void, i_this);
    f32 dx = i_this->mTailEnd[1].x - i_this->mTailEnd[0].x;
    f32 dz = i_this->mTailEnd[1].z - i_this->mTailEnd[0].z;
    f32 dy = i_this->mTailEnd[1].y - i_this->mTailEnd[0].y;
    i_this->mTailCounter = i_this->mTailCounter + 1;
    s16 ay = cM_atan2s(dx, dz);
    s16 ax = -cM_atan2s(dy, std_sqrtf(gabi::fmadds(dx, dx, dz * dz)));
    gabi::Local<cXyz> step;
    step->y = 0.0f;
    step->x = 0.0f;
    step->z = 5.0f;
    cMtx_YrotS(calc_mtx(), ay);
    cMtx_XrotM(calc_mtx(), ax);
    MtxPosition(step, &i_this->mTailStep);

    cXyz* line = gabi::at<cXyz>(gabi::load<u32>(i_this->mLineMat.mpLines));
    gabi::Local<dBgS_GndChk_l> gndChk;
    gabi::call(0x02008E0C, gndChk.get()); /* cBgS_GndChk::cBgS_GndChk */
    for (int k = 0; k < 7; k++) gndChk->mPass[k] = 0;
    gndChk->__vtbl_10 = 0x1002432C;
    gndChk->__vtbl_4C = 0x1002434C;
    gndChk->mGrp = 1;
    gndChk->mpPolyPassChk = gabi::ea(gndChk.get()) + 0x40;
    gndChk->__vtbl_40 = 0x1002435C;
    gndChk->__vtbl_20 = 0x1002433C;
    gndChk->mpGrpPassChk = gabi::ea(gndChk.get()) + 0x4C;

    for (int i = 0; i < 9; i++) {
        cXyz* prev = &i_this->mTailPos[i];
        cXyz* p = &i_this->mTailPos[i + 1];
        cXyz* v = &i_this->mTailVel[i + 1];
        gndChk->m_pos.x = p->x;
        gndChk->m_pos.z = p->z;
        gndChk->m_pos.y = p->y + 30.0f;
        f32 gy = cBgS_GroundCross(dComIfG_Bgsp(), gndChk);
        f32 scale = gabi::fnmsubs((f32)i, REG0_F(4) + 0.1f, 1.0f); /* 1 - i * (REG0_F(4) + 0.1) */
        f32 vx = gabi::fmadds(i_this->mTailStep.x, scale, v->x);
        f32 vy = gabi::fmadds(i_this->mTailStep.y, scale, v->y);
        f32 vz = gabi::fmadds(i_this->mTailStep.z, scale, v->z);
        f32 ny = p->y + vy - 2.0f;
        f32 floor = gy + 5.0f;
        if (ny < floor) ny = floor;
        dx = (p->x - prev->x) + vx;
        dz = (p->z - prev->z) + vz;
        dy = ny - prev->y;
        ay = cM_atan2s(dx, dz);
        ax = -cM_atan2s(dy, std_sqrtf(gabi::fmadds(dx, dx, dz * dz)));
        step->x = 0.0f;
        step->y = 0.0f;
        step->z = 20.0f;
        cMtx_YrotS(calc_mtx(), ay);
        cMtx_XrotM(calc_mtx(), ax);
        gabi::Local<cXyz> d;
        MtxPosition(step, d);
        v->copy(*p);
        p->x = prev->x + d->x;
        p->y = prev->y + d->y;
        p->z = prev->z + d->z;
        v->x = (p->x - v->x) * 0.8f;
        v->y = (p->y - v->y) * 0.8f;
        v->z = (p->z - v->z) * 0.8f;
    }
    for (int i = 0; i < 10; i++) line[i].copy(i_this->mTailPos[i]);
    gndChk->__vtbl_20 = 0x1002433C;
    gndChk->__vtbl_40 = 0x1002435C;
    gndChk->__vtbl_4C = 0x1002431C;
    gabi::call(0x02008DAC, gndChk.get(), 0);
}
VERIFY(0x0230A3C0, tail_control);

/* 0230A828 */
static BOOL nodeCallBack_head(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x0230A828, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel* model = j3dSys_getModel();
        nz_class* i_this = model_userArea(model);
        s32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        if (i_this != nullptr) {
            PSMTXCopy(getAnmMtx(model, jntNo), calc_mtx());
            cMtx_YrotM(calc_mtx(), i_this->mHeadRotY);
            cMtx_XrotM(calc_mtx(), i_this->mHeadRotX);
            cMtx_ZrotM(calc_mtx(), i_this->mHeadRotZ);
            Mtx34* dst = getAnmMtx(model, jntNo);
            mtx_copy(dst, calc_mtx());
            PSMTXCopy(calc_mtx(), gabi::at<Mtx34>(0x104B4868));
        }
    }
    return TRUE;
}
VERIFY(0x0230A828, nodeCallBack_head);

/* 0230A964 */
static BOOL nodeCallBack_hand(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x0230A964, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel* model = j3dSys_getModel();
        nz_class* i_this = model_userArea(model);
        s32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        if (i_this != nullptr) {
            PSMTXCopy(getAnmMtx(model, jntNo), calc_mtx());
            gabi::Local<cXyz> zero;
            gabi::Local<cXyz> pos;
            zero->y = 0.0f;
            zero->z = 0.0f;
            zero->x = 0.0f;
            MtxPosition(zero, pos);
            if (jntNo == 0x12) {
                i_this->mHandPos1.copy(*pos);
            } else {
                i_this->mHandPos0.copy(*pos);
            }
            Mtx34* dst = getAnmMtx(model, jntNo);
            mtx_copy(dst, calc_mtx());
            PSMTXCopy(calc_mtx(), gabi::at<Mtx34>(0x104B4868));
        }
    }
    return TRUE;
}
VERIFY(0x0230A964, nodeCallBack_hand);

/* 0230AB4C */
static void smoke_set(nz_class* i_this) {
    WWHD_FUNC(0x0230AB4C, void, i_this);
    u32 cb = gabi::ea(&i_this->mSmokeCb);
    if (gabi::load<u32>(cb + 4) == 0) {
        s8 roomNo = fopAcM_GetRoomNo(i_this);
        dPa_control_set(dComIfGp_getParticle(), 2, 0x2022, &i_this->mSmokePos, &i_this->mSmokeAngle, nullptr, 0xB9,
                        (dPa_levelEcallBack*)&i_this->mSmokeCb, roomNo, nullptr, nullptr, nullptr);
        if (gabi::load<u32>(cb + 4) == 0) return;
    }
    u32 e = gabi::load<u32>(cb + 4); /* mSmokeCb.getEmitter() */
    gabi::store<f32>(e + 0x240, 1.5f);
    gabi::store<f32>(e + 0x238, 1.5f);
    gabi::store<f32>(e + 0x23C, 1.5f);
    gabi::store<f32>(e + 0x220, 1.5f);
    gabi::store<f32>(e + 0x224, 1.5f);
    gabi::store<f32>(e + 0x228, 1.5f);
    gabi::store<f32>(e + 0x58, 0.5f);
    gabi::store<f32>(e + 0x70, 15.0f);
    gabi::store<u32>(e + 0x5C, 4);
    gabi::store<f32>(e + 0x34, 2.0f);
}
VERIFY(0x0230AB4C, smoke_set);

/* 0230AC30: the ground below (smoke where it lands) */
static BOOL rakka_line_check(nz_class* i_this) {
    WWHD_FUNC(0x0230AC30, BOOL, i_this);
    gabi::Local<dBgS_LinChk_l> linChk;
    nz_LinChk_ct(linChk);
    mDoMtx_XrotS(calc_mtx(), 0);
    gabi::Local<cXyz> start;
    gabi::Local<cXyz> end;
    start->x = 0.0f;
    start->y = REG_F(8, 11) + -32.0f;
    start->z = 0.0f;
    MtxPosition(start, end);
    start->copy(i_this->current.pos);
    PSVECAdd(end, &i_this->current.pos, end);
    i_this->mProbeEnd[7].copy(i_this->current.pos);
    i_this->mProbeHit[7].copy(*end);
    dBgS_LinChk_Set(linChk, start, end, i_this);
    if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
        i_this->mSmokePos.copy(*LinChk_GetCross(linChk));
        gabi::Local<csXyz> a;
        csXyz* r = csXyz_ct_r(a, 0, 0, 0);
        i_this->mSmokeAngle.x = (s16)r->x;
        i_this->mSmokeAngle.y = (s16)r->y;
        i_this->mSmokeAngle.z = (s16)r->z;
        smoke_set(i_this);
        nz_LinChk_dt(linChk);
        return TRUE;
    }
    nz_LinChk_dt(linChk);
    return FALSE;
}
VERIFY(0x0230AC30, rakka_line_check);

/* 0230B108: drop what it holds */
static void item_poi(nz_class* i_this) {
    WWHD_FUNC(0x0230B108, void, i_this);
    fopAc_ac_c* item = fopAcM_SearchByID(i_this->mItemId);
    if (item != nullptr) {
        if (item != nullptr && fpcM_GetName(item) == PROC_BOMB) {
            item->current.angle.y = i_this->mMoveAngle;
            item->speed.y = 45.0f;
            item->speedF = 20.0f;
            item->gravity = -5.0f;
            daBomb_setBombOnCoSet(item);
            if (i_this->mType != 0) {
                i_this->m400 = 0;
                item->shape_angle.z = 0;
                daBomb_setBombFire_ON(item);
                i_this->m547 = 0;
                i_this->mItemId = 0xFFFFFFFF;
                return;
            }
        } else {
            item->speedF = 20.0f;
            item->speed.y = 20.0f;
            item->gravity = -3.0f;
            item->current.angle.y = i_this->current.angle.y;
            if (!daItem_checkLock(item)) {
                daItem_0218319C(item);
                daItem_02183250(item);
            }
        }
    }
    i_this->m547 = 0;
    i_this->mItemId = 0xFFFFFFFF;
}
VERIFY(0x0230B108, item_poi);

/* 0230B230: collects bombs and items that can be picked up */
static void* s_a_d_sub(void* ac1, void*) {
    WWHD_FUNC(0x0230B230, void*, ac1, (u32)0);
    if (obj_count() < 100 && fopAc_IsActor(ac1) && ac1 != nullptr) {
        fopAc_ac_c* a = (fopAc_ac_c*)ac1;
        bool bomb = false;
        if (fpcM_GetName(ac1) == PROC_BOMB) {
            if (!daBomb_getBombCheck_Flag(ac1) && fopAcM_GetParam(a) == 1 && a->speedF == 0.0f) bomb = true;
        }
        if ((fpcM_GetName(ac1) == PROC_ITEM && daItem_checkLock(ac1)) || bomb) {
            s32 n = obj_count();
            obj_count() = n + 1;
            obj_list()[n] = gabi::ea(ac1);
        }
    }
    return nullptr;
}
VERIFY(0x0230B230, s_a_d_sub);

/* 0230B320: the nearest reachable bomb or item */
static fopAc_ac_c* search_get_obj(nz_class* i_this) {
    WWHD_FUNC(0x0230B320, fopAc_ac_c*, i_this);
    gabi::Local<dBgS_LinChk_l> linChk;
    nz_LinChk_ct(linChk);
    obj_count() = 0;
    fpcM_Search(0x0230B230 /* s_a_d_sub */, i_this);
    if (obj_count() != 0) {
        f32 range = 50.0f;
        s32 i = 0;
        while (i < obj_count()) {
            fopAc_ac_c* obj = gabi::at<fopAc_ac_c>(obj_list()[i]);
            f32 dx = obj->current.pos.x - i_this->current.pos.x;
            f32 dz = obj->current.pos.z - i_this->current.pos.z;
            f32 dy = obj->current.pos.y - i_this->current.pos.y;
            if (std_sqrtf(gabi::fmadds(dx, dx, dz * dz)) < range && std_sqrtf(dy * dy) < 100.0f) {
                gabi::Local<cXyz> start;
                gabi::Local<cXyz> end;
                end->x = obj->current.pos.x;
                f32 oy = obj->current.pos.y;
                end->y = oy;
                end->z = obj->current.pos.z;
                end->y = oy + 50.0f;
                start->x = i_this->current.pos.x;
                f32 ty = i_this->current.pos.y;
                start->y = ty;
                start->z = i_this->current.pos.z;
                start->y = ty + 50.0f;
                dBgS_LinChk_Set(linChk, start, end, i_this);
                if (!cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
                    if (obj != nullptr && fpcM_GetName(obj) == PROC_BOMB && !daBomb_getBombCheck_Flag(obj)) {
                        obj->speed.y = 0.0f;
                        obj->speed.z = 0.0f;
                        obj->gravity = 0.0f;
                        obj->speed.x = 0.0f;
                        obj->speedF = 0.0f;
                        daBomb_setBombOffCoSet(obj);
                        daBomb_setBombCheck_Flag(obj);
                        nz_LinChk_dt(linChk);
                        return obj;
                    }
                    if (daItem_checkLock(obj)) {
                        daItem_setLock(obj);
                        nz_LinChk_dt(linChk);
                        return obj;
                    }
                }
            }
            i++;
            s32 n = obj_count();
            if (i > n) break;
            if (i == n) {
                range += 50.0f;
                i = 0;
                if (range > 2000.0f) break;
            }
        }
    }
    nz_LinChk_dt(linChk);
    return nullptr;
}
VERIFY(0x0230B320, search_get_obj);

/* 0230B6CC: finds the rat hole */
static void* s_ana_sub(void* ac1, void* ac2) {
    WWHD_FUNC(0x0230B6CC, void*, ac1, ac2);
    if (fopAc_IsActor(ac1) && ac1 != nullptr && fpcM_GetName(ac1) == PROC_NZ_HOLE) {
        ((nz_class*)ac2)->mHoleId = fopAcM_GetID(ac1);
    }
    return nullptr;
}
VERIFY(0x0230B6CC, s_ana_sub);

/* 0230B738 */
static void anm_init(nz_class* i_this, int bckFileIdx, float morf, unsigned char loopMode, float playSpeed, int soundFileIdx) {
    WWHD_FUNC(0x0230B738, void, i_this, bckFileIdx, morf, loopMode, playSpeed, soundFileIdx);
    i_this->mAnmIdx = bckFileIdx;
    if (soundFileIdx >= 0) {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x100244B8) /* "Nz" */, bckFileIdx, NZ_SAFESTRING_VTBL);
        void* sound = dComIfG_getObjectRes(STR(0x100244B8), soundFileIdx, NZ_SAFESTRING_VTBL);
        i_this->mpMorf->setAnm(anm, loopMode, morf, playSpeed, 0.0f, -1.0f, sound);
    } else {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x100244B8), bckFileIdx, NZ_SAFESTRING_VTBL);
        i_this->mpMorf->setAnm(anm, loopMode, morf, playSpeed, 0.0f, -1.0f, nullptr);
    }
}
VERIFY(0x0230B738, anm_init);

/* 0230B864: 0 nothing, 1 an object to fetch, 2 a free direction away from the player */
static int search_check(nz_class* i_this) {
    WWHD_FUNC(0x0230B864, int, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<dBgS_LinChk_l> linChk;
    nz_LinChk_ct(linChk);
    if (fopAcM_searchActorDistanceXZ(i_this, player) < 1200.0f) {
        fopAc_ac_c* obj = (fopAc_ac_c*)gabi::call<fopAc_ac_c*>(0x0230B320, i_this); /* search_get_obj */
        if (obj != nullptr) {
            i_this->mMoveAngle = fopAcM_searchActorAngleY(i_this, obj);
            i_this->mItemId = fopAcM_GetID(obj);
            nz_LinChk_dt(linChk);
            return 1;
        }
        s16 a = fopAcM_searchPlayerAngleY(i_this);
        a += (s16)gabi::ftoi(cM_rndFX(8000.0f));
        cMtx_YrotS(calc_mtx(), a);
        gabi::Local<cXyz> ofs;
        gabi::Local<cXyz> end;
        ofs->y = 0.0f;
        ofs->z = 100.0f;
        ofs->x = 0.0f;
        MtxPosition(ofs, end);
        PSVECAdd(end, &i_this->current.pos, end);
        dBgS_LinChk_Set(linChk, &i_this->current.pos, end, i_this);
        if (!cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
            i_this->mMoveAngle = fopAcM_searchPlayerAngleY(i_this);
            nz_LinChk_dt(linChk);
            return 2;
        }
    }
    nz_LinChk_dt(linChk);
    return 0;
}
VERIFY(0x0230B864, search_check);

/* 0230BA9C: 0 no hit, 1 hit by an attack, 2 the player's spin attack nearby */
static int body_atari_check(nz_class* i_this) {
    WWHD_FUNC(0x0230BA9C, int, i_this);
    bool spin = true;
    if (i_this->mCyl.ChkTgHit()) {
        void* hitObj = i_this->mCyl.GetTgHitObj();
        if (hitObj == nullptr) return 0;
        i_this->mHitPos.copy(i_this->mCyl.mGObjTg.mHitPos);
        item_poi(i_this);
        u32 type = gabi::load<u32>(gabi::ea(hitObj) + 0x10);
        spin = false;
        if (type == 0x200 || type == 0x40000) {
            i_this->mEnemyFire.mFireDuration = 100;
        } else if (type == 0x80000) {
            i_this->mEnemyIce.mFreezeDuration = 200;
            i_this->mbFrozen = 1;
            if (gabi::load<u32>(gabi::ea(hitObj) + 0x10) & 0x8000) {
                spin = true;
            } else {
                i_this->m3D8 = 4;
                i_this->m3D9 = 0x32;
                return 1;
            }
        } else if (type == 0x100000) {
            i_this->mEnemyIce.mLightShrinkTimer = 1;
            i_this->mEnemyIce.mYOffset = 0.0f;
            gabi::store<u32>(gabi::ea(i_this) + 0x39C, 0); /* attention_info.flags */
            i_this->mEnemyIce.mParticleScale = 1.0f;
            nz_mons_se_start(i_this, 0x481B);
        }
        if (!spin) {
            if (i_this->mbFrozen == 0) {
                gabi::Local<be<u32>[7]> atInfo; /* CcAtInfo */
                (*atInfo)[5] = 0;
                (*atInfo)[0] = gabi::ea(i_this->mCyl.GetTgHitObj());
                cc_at_check_nz(i_this, atInfo.get());
            }
            if (gabi::load<u32>(gabi::ea(hitObj) + 0x10) & 0x8000) {
                spin = true;
            } else {
                i_this->m3D8 = 4;
                i_this->m3D9 = 0x32;
                return 1;
            }
        }
    }
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    if (i_this->current.angle.x == 0 && i_this->current.angle.z == 0 && (gabi::load<u32>(gabi::ea(player) + 0x3C0) & 0x20000)) {
        f32 dz = gabi::load<f32>(gabi::ea(player) + 0x3EC) - i_this->current.pos.z;
        f32 dx = gabi::load<f32>(gabi::ea(player) + 0x3E4) - i_this->current.pos.x;
        if (std_sqrtf(gabi::fmadds(dx, dx, dz * dz)) < 1000.0f) {
            item_poi(i_this);
            i_this->m3D8 = 4;
            i_this->m3D9 = 0x3C;
            return 2;
        }
    }
    return 0;
}
VERIFY(0x0230BA9C, body_atari_check);

/* 0230BD24: a free direction from `angle` on (0: found, 1: none) */
static int nezumi_move(nz_class* i_this, s16 angle) {
    WWHD_FUNC(0x0230BD24, int, i_this, angle);
    gabi::Local<dBgS_LinChk_l> linChk;
    nz_LinChk_ct(linChk);
    for (int i = 8; i != 0; i--) {
        cMtx_YrotS(calc_mtx(), angle);
        gabi::Local<cXyz> ofs;
        gabi::Local<cXyz> end;
        ofs->x = 0.0f;
        ofs->y = 0.0f;
        ofs->z = 100.0f;
        MtxPosition(ofs, end);
        PSVECAdd(end, &i_this->current.pos, end);
        dBgS_LinChk_Set(linChk, &i_this->current.pos, end, i_this);
        if (!cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
            i_this->mMoveAngle = angle;
            nz_LinChk_dt(linChk);
            return 0;
        }
        angle += 0x1000;
    }
    i_this->mMoveAngle = i_this->mMoveAngle - 0x8000;
    nz_LinChk_dt(linChk);
    return 1;
}
VERIFY(0x0230BD24, nezumi_move);

/* 0230EDC8 */
static BOOL daNZ_IsDelete(nz_class*) {
    WWHD_FUNC(0x0230EDC8, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0230EDC8, daNZ_IsDelete);

/* 0230EDD0 */
static BOOL daNZ_Delete(nz_class* i_this) {
    WWHD_FUNC(0x0230EDD0, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhase, STR(0x10024530) /* "Nz" */);
    if (i_this->heap != nullptr) {
        i_this->mpMorf->stopZelAnime();
    }
    gabi::call_ptr(gabi::load<u32>(i_this->mSmokeCb.__vtbl + 0x44), &i_this->mSmokeCb);   /* remove() */
    dPa_rippleEcallBack_end((dPa_rippleEcallBack*)&i_this->mRippleCb);
    gabi::call_ptr(gabi::load<u32>(i_this->mFollowCb.__vtbl + 0x44), &i_this->mFollowCb); /* remove() */
    enemy_fire_remove(&i_this->mEnemyFire);
    return TRUE;
}
VERIFY(0x0230EDD0, daNZ_Delete);

/* J3DModelData joint nodes (HD): count +4, nodes +8 (0x1C each; out-of-range indices give node 0) */
static inline void setJointCallBack(nz_class* i_this, u32 j, u32 cb) {
    u32 data = gabi::ea(getModelData(i_this->mpMorf->getModel()));
    u32 n = gabi::load<u32>(data + 4);
    u32 p = gabi::load<u32>(data + 8);
    if (j < n) p += j * 0x1C;
    gabi::store<u32>(p + 8, cb);
}
static inline u16 getJointNum(nz_class* i_this) {
    /* 027F3F94 (matcher: __nw) returns the joint tree; its +8 is the joint count */
    u32 tree = gabi::call<u32>(0x027F3F94, getModelData(i_this->mpMorf->getModel()));
    return gabi::load<u16>(tree + 8);
}

/* 0230EE58 */
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x0230EE58, BOOL, a_this);
    nz_class* i_this = (nz_class*)a_this;
    u16 bdl = gabi::load<u16>(0x101C7620 + (i_this->mType & 1) * 2);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x10024533) /* "Nz" */, bdl, NZ_SAFESTRING_VTBL);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x10024533), 0x23, NZ_SAFESTRING_VTBL);
    i_this->mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1,
                                            nullptr, 0x80000, 0x37441422);
    if (i_this->mpMorf == nullptr || i_this->mpMorf->getModel() == nullptr) {
        return FALSE;
    }
    gabi::store<u32>(gabi::ea(i_this->mpMorf->getModel()) + 0xB8, gabi::ea(i_this)); /* setUserArea */

    for (u16 j = 0; j < getJointNum(i_this); j++) {
        if (j == 1 || j == 9) {
            setJointCallBack(i_this, j, 0x0230A30C /* nodeCallBack_tail */);
        } else if (j == 0xB) {
            u32 data = gabi::ea(getModelData(i_this->mpMorf->getModel()));
            u32 n = gabi::load<u32>(data + 4);
            u32 p = gabi::load<u32>(data + 8);
            if (n > 0xB) p += 0xB * 0x1C;
            gabi::store<u32>(p + 8, 0x0230A828 /* nodeCallBack_head */);
        } else if (j == 0x12 || j == 0x15) {
            setJointCallBack(i_this, j, 0x0230A964 /* nodeCallBack_hand */);
        }
    }

    void* tex;
    if (i_this->mType == 0) {
        tex = dComIfG_getObjectRes(STR(0x10024533), 0x2E, NZ_SAFESTRING_VTBL);
    } else {
        tex = dComIfG_getObjectRes(STR(0x10024533), 0x2D, NZ_SAFESTRING_VTBL);
    }
    /* mDoExt_3DlineMat1_c::init(1 line, 10 segments, texture, 0) */
    if (!gabi::call<BOOL>(0x025EBA58, &i_this->mLineMat, 1, 10, tex, 0)) {
        return FALSE;
    }
    /* dMat_ice-style material object at 0x11C4: init(model) */
    if (!gabi::call<BOOL>(0x025E8A48, i_this->mIceMat, i_this->mpMorf->getModel())) {
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x0230EE58, useHeapInit);

/* 0230F104: enemyfire::enemyfire (inline constructor, this TU's copy) */
static enemyfire* nz_enemyfire_ct(enemyfire* self) {
    WWHD_FUNC(0x0230F104, enemyfire*, self);
    if (self == nullptr) {
        self = (enemyfire*)operator_new(sizeof(enemyfire));
        if (self == nullptr) return self;
    }
    if (gabi::ea(&self->mDirection) == 0) operator_new(0xC);
    dCcD_Stts_ct(&self->mStts);
    gabi::call(0x025166F0, &self->mSph); /* dCcD_Sph::dCcD_Sph */
    self->m228 = 1.0f;
    return self;
}
VERIFY(0x0230F104, nz_enemyfire_ct);

static inline void objAcch_ct(dBgS_ObjAcch* acch) {
    gabi::call(0x024F0474, acch); /* dBgS_Acch::dBgS_Acch */
    u32 a = gabi::ea(acch);
    gabi::store<u32>(a + 0x10, 0x1002436C);
    gabi::store<u32>(a + 0x20, 0x1002437C);
    gabi::store<u32>(a + 0x14, 0x1002438C);
    gabi::store<u8>(a + 0x18, 1);
}

/* 0230F190: nz_class::nz_class */
static nz_class* nz_class_ct(nz_class* self) {
    WWHD_FUNC(0x0230F190, nz_class*, self);
    if (self == nullptr) {
        self = (nz_class*)operator_new(sizeof(nz_class));
        if (self == nullptr) return self;
    }
    fopAc_ac_c_ct(self);
    self->__vtbl = NZ_VTBL;
    gabi::call(0x025EB82C, &self->mLineMat);     /* mDoExt_3DlineMat1_c::mDoExt_3DlineMat1_c */
    gabi::call(0x025A9084, &self->mRippleCb);    /* dPa_rippleEcallBack */
    gabi::call(0x025A5B18, &self->mSmokeCb, 1);  /* dPa_smokeEcallBack(1) */
    gabi::call(0x025A5894, &self->mFollowCb, 0, 0); /* dPa_followEcallBack(0, 0) */
    gabi::call(0x024EFE94, &self->mAcchCir);
    objAcch_ct(&self->mAcch);
    dCcD_Stts_ct(&self->mStts);
    dCcD_Cyl_ct(&self->mCyl, NZ_AAB_VTBL);
    dCcD_Stts_ct(&self->mEnemyIce.mStts);
    dCcD_Cyl_ct(&self->mEnemyIce.mCyl, NZ_AAB_VTBL);
    gabi::call(0x024EFE94, &self->mEnemyIce.mBgAcchCir);
    objAcch_ct(&self->mEnemyIce.mBgAcch);
    nz_enemyfire_ct(&self->mEnemyFire);
    gabi::call(0x025E895C, self->mIceMat);
    return self;
}
VERIFY(0x0230F190, nz_class_ct);

/* 0230F750: daNZ_HIO_c::daNZ_HIO_c */
static daNZ_HIO_c* daNZ_HIO_c_ct(daNZ_HIO_c* self) {
    WWHD_FUNC(0x0230F750, daNZ_HIO_c*, self);
    if (self == nullptr) {
        self = (daNZ_HIO_c*)operator_new(sizeof(daNZ_HIO_c));
        if (self == nullptr) return self;
    }
    self->m08 = 70.0f;
    self->m0C = 20.0f;
    self->m14 = -5.0f;
    self->m04 = 40.0f;
    self->m10 = 45.0f;
    self->__vtbl = NZ_HIO_VTBL;
    return self;
}
VERIFY(0x0230F750, daNZ_HIO_c_ct);

/* 0230F7CC: static initialisation of the translation unit */
static void __sinit_d_a_nz_cpp() {
    WWHD_FUNC(0x0230F7CC, void, (u32)0);
    for (int i = 0; i < 4; i++) gabi::store<u32>(0x10468D0C + 4 * i, 0);
    __register_global_object(0x101C77BC);
    gabi::store<f32>(0x10468CE8, -3.1415927f);
    gabi::store<f32>(0x10468CEC, 3.1415927f);
    gabi::call(0x028ED6F8, (u32)0x10468CF0);
    __register_global_object(0x101C77C8);
    gabi::call(0x028EAB2C, (u32)0x10468CF1);
    __register_global_object(0x101C77D4);
    daNZ_HIO_c_ct(&l_nzHIO());
}
VERIFY(0x0230F7CC, __sinit_d_a_nz_cpp);

/* 0230F86C: fopAcM_SearchByID (this TU's out-of-line copy) */
static fopAc_ac_c* nz_SearchByID(u32 id) {
    WWHD_FUNC(0x0230F86C, fopAc_ac_c*, id);
    return fopAcM_SearchByID(id);
}
VERIFY(0x0230F86C, nz_SearchByID);

/* 0230F8A8: sead::SafeString deleting destructor (this TU's copy) */
static void nz_SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0230F8A8, void, p, flags);
    if (p != nullptr && (flags & 1)) operator_delete(p);
}
VERIFY(0x0230F8A8, nz_SafeString_dt);

/* 0230FD20 */
static void BG_check(nz_class* i_this) {
    WWHD_FUNC(0x0230FD20, void, i_this);
    f32 ofs = i_this->mBgOffsetY;
    i_this->current.pos.y = i_this->current.pos.y - ofs;
    i_this->old.pos.y = i_this->old.pos.y - ofs;
    i_this->mAcch.CrrPos(dComIfG_Bgsp());
    ofs = i_this->mBgOffsetY;
    i_this->current.pos.y = i_this->current.pos.y + ofs;
    i_this->old.pos.y = i_this->old.pos.y + ofs;
}
VERIFY(0x0230FD20, BG_check);

/* 023110C0: nz_class deleting destructor */
static void nz_class_dt(nz_class* self, s32 flags) {
    WWHD_FUNC(0x023110C0, void, self, flags);
    if (self == nullptr) return;
    gabi::call(0x025E89F8, self->mIceMat, 2);
    gabi::call(0x02515AE8, &self->mEnemyFire.mSph, 2); /* dCcD_Sph::~dCcD_Sph */
    dCcD_Stts_dt(&self->mEnemyFire.mStts, 2);
    u32 acch = gabi::ea(&self->mEnemyIce.mBgAcch);
    gabi::store<u32>(acch + 0x20, 0x1002437C);
    gabi::store<u32>(acch + 0x14, 0x1002438C);
    gabi::call(0x024EFD9C, acch, 0);
    gabi::call(0x02018034, gabi::ea(&self->mEnemyIce.mBgAcchCir) + 0x14, 2);
    dCcD_Cyl_dt(&self->mEnemyIce.mCyl, 2);
    dCcD_Stts_dt(&self->mEnemyIce.mStts, 2);
    dCcD_Cyl_dt(&self->mCyl, 2);
    dCcD_Stts_dt(&self->mStts, 2);
    acch = gabi::ea(&self->mAcch);
    gabi::store<u32>(acch + 0x20, 0x1002437C);
    gabi::store<u32>(acch + 0x14, 0x1002438C);
    gabi::call(0x024EFD9C, acch, 0);
    gabi::call(0x02018034, gabi::ea(&self->mAcchCir) + 0x14, 2);
    gabi::call(0x025EB8B8, &self->mLineMat, 2); /* ~mDoExt_3DlineMat1_c (matcher: draw) */
    gabi::call(0x025D50BC, self, 0);
    if (flags & 1) operator_delete(self);
}
VERIFY(0x023110C0, nz_class_dt);

/* 025ABBBC dADM_CharTbl::GetNameIndex2(name, n) on play+0x50A0 */
static inline s32 dComIfGp_CharTbl2_GetNameIndex(const char* name, s32 n) {
    return gabi::call<s32>(0x025ABBBC, gabi::at<u8>(dComIfGp_ea() + 0x50A0), name, n);
}
static inline void tail_control_(nz_class* i_this) { gabi::call(0x0230A3C0, i_this); }

/* 0230F32C */
static cPhs_State daNZ_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x0230F32C, cPhs_State, a_this);
    nz_class* i_this = (nz_class*)a_this;
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) nz_class_ct(i_this);
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    cPhs_State ret = dComIfG_resLoad(&i_this->mPhase, STR(0x10024536) /* "Nz" */);
    if (ret != cPhs_COMPLEATE_e) {
        return ret;
    }
    u32 prm = fopAcM_GetParam(a_this);
    i_this->mArg0 = prm;
    i_this->mType = prm >> 8;
    i_this->mArg3 = prm >> 24;
    if (((prm >> 8) & 0xFF) == 0xFF) i_this->mType = 0;
    if ((prm & 0xFF) == 0xFF) i_this->mArg0 = 0;
    if (!fopAcM_entrySolidHeap(a_this, 0x0230EE58 /* useHeapInit */, 0x3200)) {
        return cPhs_ERROR_e;
    }
    gabi::store<u32>(gabi::ea(a_this) + 0x39C, 4); /* attention_info.flags */
    a_this->health = 1;
    a_this->max_health = 1;
    i_this->mAcchCir.SetWall(60.0f, 60.0f);
    i_this->mAcch.Set(&a_this->current.pos, &a_this->old.pos, a_this, 1, &i_this->mAcchCir, &a_this->speed);
    i_this->mStts.Init(0xFF, 1, a_this);
    i_this->mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101C7778));
    i_this->mCyl.SetStts(&i_this->mStts);
    J3DModel* model = i_this->mpMorf->getModel();
    u32 m = gabi::ea(model);
    gabi::store<f32>(m + 0xC4, a_this->scale.z); /* setBaseScale(scale) */
    gabi::store<f32>(m + 0xC0, a_this->scale.y);
    gabi::store<f32>(m + 0xBC, a_this->scale.x);
    mDoMtx_stack_transS(a_this->current.pos);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), a_this->shape_angle.x, a_this->shape_angle.y, a_this->shape_angle.z);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    i_this->mpMorf->calc();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &a_this->current.pos, &a_this->tevStr);
    a_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpMorf->getModel()));

    i_this->mEnemyFire.mpActor = a_this;
    i_this->mEnemyIce.mWallRadius = 40.0f;
    i_this->mEnemyFire.mpMcaMorf = i_this->mpMorf;
    i_this->mEnemyIce.mpActor = a_this;
    i_this->mEnemyIce.mCylHeight = 40.0f;
    a_this->gravity = -5.0f;
    for (int i = 0; i < 10; i++) {
        i_this->mEnemyFire.mFlameJntIdxs[i] = gabi::load<s8>(0x101C76CC + i);
        i_this->mEnemyFire.mParticleScale[i] = gabi::load<f32>(0x101C7698 + 4 * i);
    }
    i_this->m3D8 = 0;
    i_this->m3D9 = 2;
    a_this->itemTableIdx = dComIfGp_CharTbl2_GetNameIndex(STR(0x100242DC), 0);

    bool carried = false;
    if (i_this->mArg0 != 0 || i_this->mType != 0) {
        /* spawned from a hole (parent) or carrying something */
        i_this->mHitPos.copy(a_this->current.pos);
        u32 parent = a_this->parentActorID;
        a_this->scale.y = 0.0f;
        a_this->scale.x = 0.0f;
        a_this->scale.z = 0.0f;
        if (parent != 0xFFFFFFFF) {
            a_this->gravity = 0.0f;
            a_this->gbaName = 0x18;
            i_this->mHoleId = parent;
            carried = true;
        } else {
            i_this->mHoleId = 0;
        }
    }
    if (!carried) {
        a_this->gravity = 0.0f;
        a_this->gbaName = 0x18;
    }
    if (i_this->mType != 0) {
        a_this->itemTableIdx = dComIfGp_CharTbl2_GetNameIndex(STR(0x100242DC), 1);
        a_this->gravity = -5.0f;
        i_this->mCyl.mTgType = 0xFF1DFEDF;
        i_this->mStts.m_weight = 100;
        i_this->m3D9 = 0x46;
        i_this->mHitPos.copy(a_this->current.pos);
        i_this->m3D8 = 5;
    }
    i_this->mTailPos[0].copy(i_this->mTailEnd[0]);
    tail_control_(i_this);
    a_this->speedF = 20.0f;
    i_this->mMoveAngle = a_this->current.angle.y;
    i_this->mCyl.mObjAt.mSPrm &= ~1u;
    return ret;
}
VERIFY(0x0230F32C, daNZ_Create);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline BOOL dBgS_ChkPolySafe(void* poly) { return gabi::call<BOOL>(0x02008254, dComIfG_Bgsp(), poly); }
static inline s32 dBgS_GetGroundCode(void* poly) { return gabi::call<s32>(0x024EF0BC, dComIfG_Bgsp(), poly); }
static inline BOOL dBgS_ChkGrpInf(void* poly, u32 grp) { return gabi::call<BOOL>(0x024EEE30, dComIfG_Bgsp(), poly, grp); }
static inline void dKy_get_seacolor(GXColor* amb, GXColor* dif) { gabi::call(0x025602F0, amb, dif); }
static inline u32 cb_emitter(void* cb) { return gabi::load<u32>(gabi::ea(cb) + 4); }
static inline void nz_item_poi(nz_class* i_this) { gabi::call(0x0230B108, i_this); }

/* 0230F8BC: abyss (ground code 4) and water surface effects */
static void naraku_water_check(nz_class* i_this) {
    WWHD_FUNC(0x0230F8BC, void, i_this);
    void* gndPoly = gabi::at<u8>(gabi::ea(&i_this->mAcch) + 0xE8);
    if (i_this->mAcch.m_ground_h != -1e9f && dBgS_ChkPolySafe(gndPoly) && dBgS_GetGroundCode(gndPoly) == 4) {
        s16 t = i_this->mDeadCounter + 1;
        f32 y = i_this->current.pos.y;
        i_this->mDeadCounter = t;
        if (y < -500.0f || t > 100) {
            nz_item_poi(i_this);
            fopAcM_delete(i_this);
            return;
        }
    }
    if (REG_S(8, 3) != 0) return;
    if (i_this->speedF == 0.0f && i_this->speed.y == 0.0f) {
        /* standing still: ripples */
        if (i_this->mbInWater == 0) return;
        i_this->mbInWater = 0;
        dPa_followEcallBack_end((dPa_followEcallBack*)&i_this->mFollowCb);
        i_this->mSmokePos.copy(i_this->mRipplePos);
        gabi::Local<cXyz> scale;
        scale->z = 1.0f;
        scale->x = 1.0f;
        scale->y = 1.0f;
        if (cb_emitter(&i_this->mRippleCb) != 0) return;
        dPa_control_set(dComIfGp_getParticle(), 5, 0x33, &i_this->mSmokePos, nullptr, scale, 0xFF,
                        (dPa_levelEcallBack*)&i_this->mRippleCb, -1, nullptr, nullptr, nullptr);
        return;
    }
    gabi::Local<dBgS_LinChk_l> linChk;
    cBgS_LinChk_ct(linChk);
    for (int i = 1; i < 7; i++) linChk->mPass[i] = 0;
    linChk->mpPolyPassChk = gabi::ea(linChk.get()) + 0x58;
    linChk->__vtbl_64 = 0x100243FC;
    linChk->__vtbl_20 = 0x100243EC;
    linChk->mPass[0] = 1;
    linChk->__vtbl_58 = 0x1002440C;
    linChk->__vtbl_10 = 0x100243DC;
    linChk->mpGrpPassChk = gabi::ea(linChk.get()) + 0x64;
    linChk->mGrp = 1;
    gabi::Local<cXyz> start;
    gabi::Local<cXyz> end;
    f32 up = REG_F(8, 10) + 100.0f;
    f32 down = REG_F(8, 11) + 100.0f;
    start->x = i_this->current.pos.x;
    f32 y0 = i_this->current.pos.y;
    start->y = y0;
    start->z = i_this->current.pos.z;
    end->x = i_this->current.pos.x;
    f32 y1 = i_this->current.pos.y;
    end->y = y1;
    end->z = i_this->current.pos.z;
    start->y = y0 + up;
    linChk->mGrp = 3;
    end->y = y1 - down;
    dBgS_LinChk_Set(linChk, start, end, i_this);
    if (cBgS_LineCross(dComIfG_Bgsp(), linChk) && abs((s32)(s16)i_this->current.angle.x) <= 0x500) {
        if (dBgS_ChkGrpInf(gabi::at<u8>(gabi::ea(linChk.get()) + 0x14), 0x100)) {
            /* on the water surface */
            if (i_this->mbInWater == 0 && i_this->mRippleTimer == 0) {
                i_this->mRippleTimer = 5;
                i_this->mbInWater = 1;
                dPa_rippleEcallBack_end((dPa_rippleEcallBack*)&i_this->mRippleCb);
                bool ok = true;
                if (cb_emitter(&i_this->mFollowCb) == 0) {
                    dPa_control_set(dComIfGp_getParticle(), 0, 0x442, &i_this->mFollowPos, (csXyz*)&i_this->mFollowAngle, nullptr,
                                    0xFF, (dPa_levelEcallBack*)&i_this->mFollowCb, -1, nullptr, nullptr, nullptr);
                    if (cb_emitter(&i_this->mFollowCb) == 0) ok = false;
                }
                if (!ok) {
                    nz_LinChk_dt(linChk);
                    return;
                }
                gabi::Local<GXColor> amb;
                gabi::Local<GXColor> dif;
                dKy_get_seacolor(amb, dif);
                u32 e = cb_emitter(&i_this->mFollowCb);
                gabi::store<u8>(e + 0x245, amb->g);
                gabi::store<u8>(e + 0x246, amb->b);
                gabi::store<u8>(e + 0x244, amb->r);
            }
            if (cb_emitter(&i_this->mFollowCb) != 0) {
                i_this->mFollowPos.x = i_this->current.pos.x;
                f32 y = i_this->current.pos.y;
                i_this->mFollowPos.y = y;
                i_this->mFollowPos.z = i_this->current.pos.z;
                i_this->mFollowPos.y = y + (REG_F(8, 7) + 20.0f);
                dPa_rippleEcallBack_end((dPa_rippleEcallBack*)&i_this->mRippleCb);
            }
        } else if (i_this->mbInWater != 0) {
            i_this->mbInWater = 0;
            dPa_rippleEcallBack_end((dPa_rippleEcallBack*)&i_this->mRippleCb);
            dPa_followEcallBack_end((dPa_followEcallBack*)&i_this->mFollowCb);
        }
    } else if (i_this->mbInWater != 0) {
        dPa_rippleEcallBack_end((dPa_rippleEcallBack*)&i_this->mRippleCb);
        dPa_followEcallBack_end((dPa_followEcallBack*)&i_this->mFollowCb);
        i_this->mbInWater = 0;
    }
    nz_LinChk_dt(linChk);
}
VERIFY(0x0230F8BC, naraku_water_check);

/* J3DModelData::getMaterialNodePointer(getMaterialName()->getIndex(name))->getShape(), HD inline:
 * the name is a sead::SafeString (its empty virtual 023111D4 is called first) */
static inline u32 nz_getMaterialShape(u32 modelData, u32 nameAddr) {
    gabi::Local<SafeString> key;
    key->mStringTop = nameAddr;
    key->__vtbl = NZ_SAFESTRING_VTBL;
    u32 matTable = gabi::load<u32>(modelData);
    gabi::call(0x023111D4, key.get()); /* SafeString: empty virtual (this TU's copy) */
    u32 off = gabi::load<u32>(matTable + 0x18);
    u32 nameTab = off != 0 ? matTable + 0x18 + off : 0;
    s32 idx = gabi::call<s32>(0x027DF9B0, nameTab, (u32)key->mStringTop); /* JUTNameTab::getIndex */
    u32 node;
    if (idx < 0) {
        node = 0;
    } else {
        u32 n = gabi::load<u32>(modelData + 0xC);
        node = gabi::load<u32>(modelData + 0x10);
        if ((u32)idx < n) node += idx * 0x39C;
    }
    return gabi::load<u32>(node + 8);
}

/* 0230AE30 */
static BOOL daNZ_Draw(nz_class* i_this) {
    WWHD_FUNC(0x0230AE30, BOOL, i_this);
    J3DModel* model = i_this->mpMorf->getModel();
    u32 modelData = gabi::ea(getModelData(model));
    if (i_this->mEnemyIce.mFreezeTimer > 20) {
        gabi::call(0x0259138C, i_this->mpMorf.get(), -1, i_this->mIceMat); /* dMat_ice_c::entryDL */
        return TRUE;
    }
    u32 shapeData = gabi::load<u32>(modelData + 8);
    u32 eye = nz_getMaterialShape(modelData, 0x10024478 /* "SC_eye_MX" */);
    u32 body2 = nz_getMaterialShape(modelData, 0x10024484 /* "lambert2" */);
    u32 body7 = nz_getMaterialShape(modelData, 0x10024490 /* "lambert7" */);
    gabi::store<u8>(eye + 4, 0);
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, model, &i_this->tevStr);
    i_this->mpMorf->entryDL();
    /* draw the eyes into the second buffer pair */
    gabi::store<u32>(0x104B4634, gabi::load<u32>(dComIfGp_ea() + 0x5D84));
    gabi::store<u32>(0x104B4638, gabi::load<u32>(dComIfGp_ea() + 0x5D88));
    gabi::store<u8>(eye + 4, 1);
    gabi::store<u8>(body2 + 4, 0);
    gabi::store<u8>(body7 + 4, 0);
    gabi::call(0x027F583C, model, shapeData);
    gabi::store<u8>(body2 + 4, 1);
    gabi::store<u8>(body7 + 4, 1);
    gabi::store<u32>(0x104B4634, gabi::load<u32>(dComIfGp_ea() + 0x5D78));
    gabi::store<u32>(0x104B4638, gabi::load<u32>(dComIfGp_ea() + 0x5D7C));
    /* tail */
    gabi::call(0x025EC62C, &i_this->mLineMat, 10, i_this->scale.x * 5.0f, (u32)0x101C7624, 6, &i_this->tevStr); /* update */
    u32 packets = dComIfGp_ea() + 0x5FB4;
    s32 matId = gabi::call_ptr<s32>(gabi::load<u32>(i_this->mLineMat.__vtbl + 0x14), &i_this->mLineMat);
    gabi::call(0x025EDD04, packets + matId * 0x9C, &i_this->mLineMat); /* mDoExt_3DlineMatSortPacket::setMat */
    dSnap_RegistFig(0xAD, i_this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}
VERIFY(0x0230AE30, daNZ_Draw);

static inline BOOL rakka_line_check_(nz_class* i_this) { return gabi::call<BOOL>(0x0230AC30, i_this); }
static inline void smoke_set_(nz_class* i_this) { gabi::call(0x0230AB4C, i_this); }
static inline void nz_anm_init(nz_class* i_this, int bck, f32 morf, u8 loop, f32 speed, int bas) {
    gabi::call(0x0230B738, i_this, bck, morf, loop, speed, bas);
}
static inline int body_atari_check_(nz_class* i_this) { return gabi::call<int>(0x0230BA9C, i_this); }
static inline bool groundHit(nz_class* i_this) { return (i_this->mAcch.m_flags & dBgS_Acch::GROUND_HIT) != 0; }
/* smoke where it landed (ground height under the actor) */
static inline void ground_smoke_pos(nz_class* i_this) {
    gabi::store<u32>(gabi::ea(&i_this->mSmokePos.x), gabi::load<u32>(gabi::ea(&i_this->current.pos.x)));
    i_this->mSmokePos.y = (f32)i_this->mAcch.m_ground_h;
    gabi::store<u32>(gabi::ea(&i_this->mSmokePos.z), gabi::load<u32>(gabi::ea(&i_this->current.pos.z)));
}
static inline void smoke_angle_zero(nz_class* i_this) {
    gabi::Local<csXyz> a;
    csXyz* r = csXyz_ct_r(a, 0, 0, 0);
    i_this->mSmokeAngle.x = (s16)r->x;
    i_this->mSmokeAngle.y = (s16)r->y;
    i_this->mSmokeAngle.z = (s16)r->z;
}

/* 0230FD90: knocked over / falling (states 0x32..0x3E of m3D9) */
static void nz5_move(nz_class* i_this) {
    WWHD_FUNC(0x0230FD90, void, i_this);
    dComIfGp_get();
    gabi::Local<cXyz> pos;
    pos->x = i_this->current.pos.x;
    pos->y = i_this->current.pos.y;
    pos->z = i_this->current.pos.z;
    u8 state = i_this->m3D9;
    enum { CHECK, CHECK_RELOAD, BODY } next = CHECK;
    switch (state) {
    case 0x32:
        i_this->scale.y = 1.0f;
        i_this->scale.x = 1.0f;
        i_this->mStts.m_weight = 100;
        i_this->mSmokeTimer = 0;
        i_this->scale.z = 1.0f;
        nz_mons_se_start(i_this, 0x481B);
        gabi::store<u32>(gabi::ea(i_this) + 0x39C, 0); /* attention_info.flags */
        if (i_this->mbFrozen != 0) {
            i_this->m3D9 = 0x39;
            return;
        }
        if (groundHit(i_this)) {
            i_this->shape_angle.x = 0;
            i_this->m3D9 = 0x34;
            return;
        }
        i_this->speedF = 0.0f;
        i_this->shape_angle.z = 0;
        i_this->m3D9 = i_this->m3D9 + 1;
        i_this->gravity = -3.0f;
        /* fallthrough */
    case 0x33:
        i_this->mpMorf->setPlaySpeed(0.0f);
        cLib_addCalcAngleS2(&i_this->shape_angle.x, -0x8000, 1, 0x200);
        if (!rakka_line_check_(i_this)) {
            next = CHECK_RELOAD;
            break;
        }
        i_this->current.angle.x = 0;
        i_this->mSmokeTimer = 1;
        i_this->m3D9 = i_this->m3D9 + 1;
        i_this->current.angle.z = 0;
        i_this->shape_angle.z = 0;
        i_this->shape_angle.x = 0;
        /* fallthrough */
    case 0x34:
        nz_anm_init(i_this, 0x18, 0.0f, 0, 1.0f, 9);
        i_this->current.angle.y = cLib_targetAngleY(&i_this->current.pos, &i_this->mHitPos) + 0x8000;
        nz_mons_se_start(i_this, 0x5803);
        i_this->speedF = 0.0f;
        i_this->m3D9 = i_this->m3D9 + 1;
        /* fallthrough */
    case 0x35: {
        if (i_this->mSmokeTimer != 0) i_this->mSmokeAngle.y = i_this->mSmokeAngle.y + 0x1500;
        if (!i_this->mpMorf->checkFrame(4.0f)) {
            next = CHECK_RELOAD;
            break;
        }
        if (i_this->mSmokeTimer != 0) {
            dPa_smokeEcallBack_end((dPa_smokeEcallBack*)&i_this->mSmokeCb);
            i_this->mSmokeTimer = 0;
        }
        u8 n = i_this->m3D9 + 1;
        i_this->speedF = 10.0f;
        i_this->m3D9 = n;
        i_this->speed.y = 45.0f;
        i_this->gravity = -3.0f;
        break;
    }
    case 0x36:
    case 0x38:
        if (groundHit(i_this)) {
            ground_smoke_pos(i_this);
            if (state == 0x36) {
                smoke_angle_zero(i_this);
                i_this->speedF = 0.0f;
                smoke_set_(i_this);
            } else {
                smoke_set_(i_this);
                i_this->speedF = 0.0f;
            }
        }
        if (i_this->mpMorf->isStop()) {
            dPa_smokeEcallBack_end((dPa_smokeEcallBack*)&i_this->mSmokeCb);
            i_this->m3D9 = i_this->m3D9 + 1;
        } else {
            next = CHECK_RELOAD;
        }
        break;
    case 0x37: {
        nz_anm_init(i_this, 0x19, 0.0f, 0, 1.0f, 0xA);
        nz_mons_se_start(i_this, 0x5804);
        i_this->speedF = 7.0f;
        i_this->speed.y = 30.0f;
        i_this->gravity = -3.0f;
        i_this->mSmokeAngle.y = i_this->mSmokeAngle.y + 0x1500;
        i_this->m3D9 = i_this->m3D9 + 1;
        break;
    }
    case 0x39:
        pos->y = i_this->mAcch.m_ground_h + 40.0f;
        fopAcM_createDisappear(i_this, pos, 5, 0, 0xFF);
        fopAcM_delete(i_this);
        next = CHECK_RELOAD;
        break;
    case 0x3C:
        i_this->shape_angle.x = 0;
        i_this->shape_angle.z = 0;
        nz_mons_se_start(i_this, 0x481B);
        i_this->mStts.m_weight = 100;
        i_this->speedF = 7.0f;
        i_this->speed.y = 40.0f;
        i_this->current.angle.y = fopAcM_searchPlayerAngleY(i_this) + 0x8000;
        nz_anm_init(i_this, 0x19, 0.0f, 0, 1.0f, 0xA);
        i_this->gravity = -3.0f;
        i_this->m3D9 = i_this->m3D9 + 1;
        break;
    case 0x3D:
        if (!groundHit(i_this)) break;
        ground_smoke_pos(i_this);
        smoke_angle_zero(i_this);
        i_this->speedF = 0.0f;
        smoke_set_(i_this);
        i_this->m3F2 = 0x2D;
        i_this->m3D9 = i_this->m3D9 + 1;
        /* fallthrough */
    case 0x3E:
        i_this->mSmokeAngle.y = i_this->mSmokeAngle.y + 0x1500;
        if (i_this->m3F2 != 0) {
            next = CHECK_RELOAD;
            break;
        }
        dPa_smokeEcallBack_end((dPa_smokeEcallBack*)&i_this->mSmokeCb);
        if (i_this->mType == 0) {
            i_this->m3D9 = 0;
            i_this->m3D8 = 0;
            return;
        }
        nz_anm_init(i_this, 0x23, 5.0f, 2, 1.0f, 0x12);
        i_this->m3F0 = 0x14;
        i_this->m3D8 = 5;
        i_this->m3D9 = 0x4D;
        body_atari_check_(i_this);
        next = BODY;
        break;
    }
    u8 n = i_this->m3D9;
    if (next != BODY && n >= 0x3C) {
        body_atari_check_(i_this);
        n = i_this->m3D9;
    }
    if ((n == 0x38 || n == 0x3E) && i_this->mSmokeTimer != 0) {
        u8 c = i_this->mSmokeTimer + 1;
        i_this->mSmokeAngle.y = i_this->mSmokeAngle.y + 0x1500;
        i_this->mSmokeTimer = c;
        if (c > 6) {
            dPa_smokeEcallBack_end((dPa_smokeEcallBack*)&i_this->mSmokeCb);
            i_this->mSmokeTimer = 0;
        }
    }
}
VERIFY(0x0230FD90, nz5_move);

static inline u32 daBomb_prm_make(s32 a, s32 b, s32 c) { return gabi::call<u32>(0x020CB8D8, a, b, c); }
static inline s32 daBomb_getBombRestTime(void* b) { return gabi::call<s32>(0x020CB648, b); }
static inline void daBomb_setBombRestTime(void* b, s16 t) { gabi::call(0x020CB860, b, t); }
static inline int nezumi_move_(nz_class* i_this, s16 a) { return gabi::call<int>(0x0230BD24, i_this, a); }

/* 0230F86C-style search (the HD inline stores the id on the stack, then tests it) */
static inline fopAc_ac_c* searchByID(u32 id) { return fopAcM_SearchByID(id); }

/* nz6_move tail: turn, carry the bomb */
static inline void nz6_carry(nz_class* i_this) {
    cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->mMoveAngle, 1, 0x1000);
    if (i_this->mSmokeTimer != 0) { /* carrying (m3DC) */
        fopAc_ac_c* bomb = searchByID(i_this->mItemId);
        if (bomb != nullptr) {
            if (i_this->mbBombLit == 0) daBomb_setBombRestTime(bomb, 0x96);
            daBomb_setBombOffCoSet(bomb);
            bomb->gravity = 0.0f;
            bomb->speedF = 0.0f;
            bomb->speed.x = 0.0f;
            bomb->speed.y = 0.0f;
            bomb->speed.z = 0.0f;
            bomb->current.angle.x = (s16)i_this->current.angle.x;
            bomb->current.angle.y = (s16)i_this->current.angle.y;
            bomb->current.angle.z = (s16)i_this->current.angle.z;
            bomb->shape_angle.x = (s16)i_this->shape_angle.x;
            bomb->shape_angle.y = (s16)i_this->shape_angle.y;
            bomb->shape_angle.z = (s16)i_this->shape_angle.z;
            bomb->shape_angle.z = (s16)i_this->m400;
            bomb->scale.copy(i_this->scale);
        }
        if (body_atari_check_(i_this) != 0 && i_this->mItemId != 0xFFFFFFFF) {
            bomb = searchByID(i_this->mItemId);
            if (bomb != nullptr) {
                daBomb_setBombRestTime(bomb, 1);
                bomb->scale.x = 1.0f;
                bomb->scale.y = 1.0f;
                bomb->scale.z = 1.0f;
            }
        }
    }
    i_this->shape_angle.y = (s16)i_this->current.angle.y;
}

/* 02310544: bomb rat (states 0x46..0x4E of m3D9) */
static void nz6_move(nz_class* i_this) {
    WWHD_FUNC(0x02310544, void, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<dBgS_LinChk_l> linChk;
    nz_LinChk_ct(linChk);
    switch (i_this->m3D9) {
    case 0x46: {
        i_this->mSmokeTimer = 0;
        nz_anm_init(i_this, 0x20, 0.0f, 2, 1.0f, 0x11);
        u32 prm = daBomb_prm_make(7, 1, 0);
        u32 id = fopAcM_create(PROC_BOMB, prm, &i_this->current.pos, -1, nullptr, nullptr, -1, 0);
        i_this->mItemId = id;
        if (id == 0xFFFFFFFF) {
            fopAcM_delete(i_this);
        } else {
            i_this->speedF = 20.0f;
            nz_mons_se_start(i_this, 0x4819);
            i_this->m3D9 = i_this->m3D9 + 1;
        }
        break;
    }
    case 0x47: {
        fopAc_ac_c* bomb = searchByID(i_this->mItemId);
        if (bomb == nullptr) break;
        bomb->scale.x = 0.0f;
        bomb->scale.y = 0.0f;
        bomb->scale.z = 0.0f;
        i_this->m547 = 1;
        i_this->m3D9 = i_this->m3D9 + 1;
        i_this->mSmokeTimer = 1;
        break;
    }
    case 0x48: {
        if (!(i_this->scale.x > 0.9f)) break;
        if (i_this->m3F2 == 0) {
            f32 a = (f32)(s16)i_this->current.angle.y;
            nezumi_move_(i_this, (s16)gabi::ftoi(a + cM_rndFX(16384.0f)));
            i_this->m3F2 = 10;
        }
        f32 dy = player->current.pos.y - i_this->current.pos.y;
        if (fopAcM_searchActorDistanceXZ(i_this, player) < 700.0f && std_sqrtf(dy * dy) < 100.0f) {
            dBgS_LinChk_Set(linChk, &i_this->current.pos, &player->current.pos, i_this);
            if (!cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
                s16 a = fopAcM_searchPlayerAngleY(i_this);
                if ((s16)cLib_distanceAngleS(i_this->shape_angle.y, a) < 0x2A71) {
                    i_this->speedF = 0.0f;
                    nz_anm_init(i_this, 0x21, 5.0f, 0, 1.0f, -1);
                    i_this->m3D9 = i_this->m3D9 + 1;
                }
            }
        }
        break;
    }
    case 0x49: {
        i_this->m400 = i_this->m400 + (REG_S(8, 6) + 3000);
        if (!i_this->mpMorf->isStop()) break;
        nz_anm_init(i_this, 0x22, 0.0f, 0, 1.0f, -1);
        dPa_control_set(dComIfGp_getParticle(), 0, 0x8188, &i_this->current.pos, &i_this->shape_angle, nullptr, 0xFF, nullptr, -1,
                        nullptr, nullptr, nullptr);
        fopAc_ac_c* bomb = searchByID(i_this->mItemId);
        if (bomb == nullptr) {
            i_this->mItemId = 0xFFFFFFFF;
        } else if (bomb != nullptr && fpcM_GetName(bomb) == PROC_BOMB) {
            daBomb_setBombFire_ON(bomb);
            i_this->mbBombLit = 1;
        }
        i_this->m3D9 = i_this->m3D9 + 1;
        break;
    }
    case 0x4A: {
        i_this->mpMorf->setPlaySpeed(REG_F(8, 16) + 4.0f);
        s32 v = i_this->m400;
        if (v < 60000) {
            i_this->m400 = v + REG_S(8, 6) + 9000;
        } else {
            i_this->m400 = 0x10000;
        }
        if (!i_this->mpMorf->isStop()) break;
        i_this->m400 = 0;
        i_this->mMoveAngle = fopAcM_searchPlayerAngleY(i_this);
        i_this->m3F2 = 0;
        nz_anm_init(i_this, 0x20, 0.0f, 2, 1.0f, 0x11);
        i_this->speedF = 20.0f;
        i_this->m3D9 = i_this->m3D9 + 1;
        break;
    }
    case 0x4B: {
        if (i_this->m3F2 == 0) {
            f32 a = (f32)fopAcM_searchPlayerAngleY(i_this);
            nezumi_move_(i_this, (s16)gabi::ftoi(a + cM_rndFX(8192.0f)));
            i_this->m3F2 = 10;
        }
        bool seen = false;
        if (fopAcM_searchActorDistanceXZ(i_this, player) < 400.0f) {
            dBgS_LinChk_Set(linChk, &i_this->current.pos, &player->current.pos, i_this);
            if (!cBgS_LineCross(dComIfG_Bgsp(), linChk)) seen = true;
        }
        fopAc_ac_c* bomb = searchByID(i_this->mItemId);
        bool go;
        if (bomb == nullptr) {
            i_this->mItemId = 0xFFFFFFFF;
            go = seen;
        } else if (bomb != nullptr && fpcM_GetName(bomb) == PROC_BOMB && daBomb_getBombRestTime(bomb) < REG_S(8, 7) + 30) {
            go = true;
        } else {
            go = seen;
        }
        if (go) {
            i_this->speedF = 0.0f;
            i_this->mMoveAngle = fopAcM_searchPlayerAngleY(i_this);
            i_this->m3D9 = i_this->m3D9 + 1;
        }
        break;
    }
    case 0x4C:
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->mMoveAngle, 1, 0x2000);
        if ((s16)cLib_distanceAngleS(i_this->mMoveAngle, i_this->current.angle.y) < 0x1000) {
            i_this->m3D8 = 3;
            i_this->m3D9 = 0x24;
        }
        break;
    case 0x4D:
        if (i_this->m3F0 != 0) break;
        nz_anm_init(i_this, 0x1F, 5.0f, 2, 1.0f, 0x10);
        i_this->speedF = 20.0f;
        i_this->m3F2 = 10;
        if (i_this->mHoleId == 0) fpcM_Search(0x0230B6CC /* s_ana_sub */, i_this);
        i_this->m3D9 = i_this->m3D9 + 1;
        /* fallthrough */
    case 0x4E: {
        /* run back into the hole */
        u32 holeId = i_this->mHoleId;
        if (holeId == 0 || holeId == 0xFFFFFFFF) break;
        gabi::Local<be<u32>> key;
        *key = holeId;
        fopAc_ac_c* hole = fopAcIt_Judge(0x025E1234 /* fpcSch_JudgeByID */, key.get());
        if (hole == nullptr) break;
        bool skip = false;
        s32 anm = i_this->mAnmIdx;
        f32 dy = hole->current.pos.y - i_this->current.pos.y;
        if (anm == 0x15) {
            if (groundHit(i_this)) {
                nz_anm_init(i_this, 0x1F, 1.0f, 2, 1.0f, 0x10);
                i_this->shape_angle.x = 0;
            } else {
                i_this->shape_angle.x = i_this->shape_angle.x + 0x2500;
            }
        }
        if (i_this->m3F2 != 0) skip = true;
        if (!skip) {
            i_this->m3F2 = 10;
            if (nezumi_move_(i_this, fopAcM_searchActorAngleY(i_this, hole)) == 0) {
                u32 flags = i_this->mAcch.m_flags;
                if (i_this->mAnmIdx != 0x15 && (i_this->mAcch.m_flags & dBgS_Acch::GROUND_HIT) &&
                    (i_this->mAcch.m_flags & dBgS_Acch::WALL_HIT)) {
                    (void)flags;
                    i_this->speed.y = REG_F(8, 17) + 60.0f;
                    nz_anm_init(i_this, 0x15, 5.0f, 0, 1.0f, 7);
                }
            }
        }
        if (fopAcM_searchActorDistance(i_this, hole) < REG_F(8, 19) + 40.0f && std_sqrtf(dy * dy) < 100.0f) {
            gabi::store<u32>(gabi::ea(i_this) + 0x39C, 0); /* attention_info.flags */
            i_this->current.angle.y = fopAcM_searchActorAngleY(i_this, hole);
            i_this->m3D8 = 3;
            i_this->m3D9 = 0x23;
            nz_LinChk_dt(linChk);
            return;
        }
        break;
    }
    }
    nz6_carry(i_this);
    nz_LinChk_dt(linChk);
}
VERIFY(0x02310544, nz6_move);

/* =====================================================================================
 * daNZ_Execute (0230BEF4, 12 KB): the movement modes (m3D8) are inlined; they are written
 * here as helpers named after what they do (no GameCube names exist for them).
 * ===================================================================================== */
static inline fopAc_ac_c* search_get_obj_(nz_class* i_this) { return gabi::call<fopAc_ac_c*>(0x0230B320, i_this); }
static inline int search_check_(nz_class* i_this) { return gabi::call<int>(0x0230B864, i_this); }
static inline void naraku_water_check_(nz_class* i_this) { gabi::call(0x0230F8BC, i_this); }
static inline void BG_check_(nz_class* i_this) { gabi::call(0x0230FD20, i_this); }
static inline void nz5_move_(nz_class* i_this) { gabi::call(0x0230FD90, i_this); }
static inline void nz6_move_(nz_class* i_this) { gabi::call(0x02310544, i_this); }
static inline fopAc_ac_c* nz_SearchByID_(u32 id) { return gabi::call<fopAc_ac_c*>(0x0230F86C, id); }
static inline fpc_ProcID fopAcM_createItem(cXyz* pos, s32 itemNo, s32 bitNo, s32 roomNo, s32 type, csXyz* angle, s32 action, cXyz* scale) {
    return gabi::call<fpc_ProcID>(0x025D8870, pos, itemNo, bitNo, roomNo, type, angle, action, scale);
}
static inline void daItem_02183150(void* i) { gabi::call(0x02183150, i); } /* unnamed: picked up */
static inline u32 dBgS_GetMtrlSndId(void* poly) { return gabi::call<u32>(0x024EECAC, dComIfG_Bgsp(), poly); }
static inline fopAc_ac_c* dCcD_GetAc(void* obj) { return gabi::call<fopAc_ac_c*>(0x02515BBC, obj); }
static inline s32 abs16(s16 v) { return v < 0 ? -(s32)v : (s32)v; }
static inline be<s16>* nz_timers(nz_class* i_this) { return &i_this->m3F0; } /* m3F0..m3F8 */
static inline void shape_from_angle(nz_class* i_this) {
    i_this->shape_angle.x = (s16)i_this->current.angle.x;
    i_this->shape_angle.y = (s16)i_this->current.angle.y;
    i_this->shape_angle.z = (s16)i_this->current.angle.z;
}
static inline fopAc_ac_c* judge_id(u32 id) {
    gabi::Local<be<u32>> key;
    *key = id;
    return fopAcIt_Judge(0x025E1234 /* fpcSch_JudgeByID */, key.get());
}
static inline bool hole_valid(u32 id) { return id != 0 && id != 0xFFFFFFFF; }

/* m3D8 == 0: crawling on floors, walls and ceilings (6 probes keep it on the surface) */
static inline void nz_crawl(nz_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<dBgS_LinChk_l> linChk;
    nz_LinChk_ct(linChk);
    cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
    cMtx_XrotM(calc_mtx(), i_this->current.angle.x);
    cMtx_ZrotM(calc_mtx(), i_this->current.angle.z);
    gabi::Local<cXyz> v;
    v->x = 0.0f;
    v->y = 50.0f;
    v->z = 0.0f;
    gabi::Local<cXyz> center;
    MtxPosition(v, center);
    {
        gabi::Local<cXyz> t;
        cXyz_pl(center, t, &i_this->current.pos);
        center->copy(*t);
    }
    gabi::at<cXyz>(gabi::ea(i_this) + 0x478)->copy(*center);
    int cnt = abs16(i_this->current.angle.x) < 0x1000 ? 6 : 4;
    u32 mask = 0;
    for (int k = 0; k < 6; k++) {
        i_this->mProbeHit[k].copy(i_this->current.pos);
        f32 z = gabi::load<f32>(0x101C7680 + 4 * k);
        v->x = gabi::load<f32>(0x101C7650 + 4 * k);
        v->y = gabi::load<f32>(0x101C7668 + 4 * k);
        if (i_this->m3D9 == 1) v->z = z * 0.1f;
        else v->z = z;
        MtxPosition(v, &i_this->mProbeEnd[k]);
        PSVECAdd(&i_this->mProbeEnd[k], &i_this->current.pos, &i_this->mProbeEnd[k]);
    }
    for (int j = 0; j < cnt; j++) {
        dBgS_LinChk_Set(linChk, center, &i_this->mProbeEnd[j], i_this);
        if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
            i_this->mProbeHit[j].copy(*LinChk_GetCross(linChk));
            mask |= gabi::load<u8>(0x101C7628 + j);
        }
        i_this->mProbeHit[6].copy(i_this->mProbeHit[0]);
        i_this->mProbeEnd[6].copy(i_this->mProbeHit[1]);
        i_this->mProbeHit[7].copy(i_this->mProbeHit[4]);
        i_this->mProbeEnd[7].copy(i_this->mProbeHit[5]);
    }

    if ((mask & 3) == 3) {
        /* both front probes hit: follow the surface */
        f32 ay = i_this->mProbeHit[0].y;
        f32 dy = ay - i_this->mProbeHit[1].y;
        f32 dx = i_this->mProbeHit[0].x - i_this->mProbeHit[1].x;
        f32 dz = i_this->mProbeHit[0].z - i_this->mProbeHit[1].z;
        cLib_addCalc2(&i_this->current.pos.y, gabi::fnmsubs(dy, 0.5f, ay), 1.0f, 10.0f);
        if (!(i_this->speedF == 0.0f)) {
            cLib_addCalc2(&i_this->current.pos.x, gabi::fnmsubs(dx, 0.5f, i_this->mProbeHit[0].x), 1.0f, 10.0f);
            cLib_addCalc2(&i_this->current.pos.z, gabi::fnmsubs(dz, 0.5f, i_this->mProbeHit[0].z), 1.0f, 10.0f);
        }
        i_this->current.angle.x = -cM_atan2s(dy, std_sqrtf(gabi::fmadds(dx, dx, dz * dz)));
        s16 d;
        if (fabsf(dx) > 0.1f || fabsf(dz) > 0.1f) {
            s16 t = cM_atan2s(dx, dz);
            d = t - i_this->current.angle.y;
            i_this->mWallAngle = t;
        } else {
            d = i_this->mWallAngle - i_this->current.angle.y;
        }
        if (d < 0) d = -d;
        if ((u16)d > 0x4000) {
            i_this->current.angle.x = 0x8000 - i_this->current.angle.x;
        }
    } else if (!(mask & 1)) {
        i_this->current.angle.x = i_this->current.angle.x + 0x1000;
    }
    if ((mask & 0xC) == 0xC) {
        /* side probes: roll */
        gabi::Local<cXyz> t;
        cXyz_mi(&i_this->mProbeHit[2], t, &i_this->mProbeHit[3]);
        v->copy(*t);
        mDoMtx_XrotS(calc_mtx(), -i_this->current.angle.x);
        cMtx_YrotM(calc_mtx(), -i_this->current.angle.y);
        gabi::Local<cXyz> r;
        MtxPosition(v, r);
        i_this->current.angle.z = cM_atan2s(r->y, std_sqrtf(gabi::fmadds(r->x, r->x, r->z * r->z)));
    }

    be<s16>* tm = nz_timers(i_this);
    enum { T0, CE6C, CB58, CD0C, C984, FACING } next = T0;
    switch (i_this->m3D9) {
    case 0: {
        i_this->m3D9 = i_this->m3D9 + 1;
        nz_anm_init(i_this, 0x23, 5.0f, 2, 1.0f, 0x12);
        i_this->mHeadRotYTarget = 0;
        i_this->mHeadRotY = 0;
        i_this->mLookCount = (s16)gabi::ftoi(cM_rndF(8.0f));
        tm[2] = 0xF;
        tm[3] = i_this->mLookCount * 30;
        i_this->speed.y = 0.0f;
        i_this->current.angle.x = 0;
        i_this->mStts.m_weight = 0xFF;
        fopAc_ac_c* obj = search_get_obj_(i_this);
        if (obj != nullptr) {
            i_this->m548 = 1;
            i_this->mMoveAngle = fopAcM_searchActorAngleY(i_this, obj);
            i_this->mItemId = fopAcM_GetID(obj);
            i_this->m3D9 = 2;
            break;
        }
        i_this->m548 = 0;
        cLib_addCalc0(&i_this->speedF, 1.0f, 10.0f);
        cLib_addCalcAngleS2(&i_this->mHeadRotY, i_this->mHeadRotYTarget, 1, 0x2000);
        goto look;
    }
    case 1:
        cLib_addCalc0(&i_this->speedF, 1.0f, 10.0f);
        cLib_addCalcAngleS2(&i_this->mHeadRotY, i_this->mHeadRotYTarget, 1, 0x2000);
    look:
        if (i_this->m548 == 0 && tm[2] == 0) {
            int r;
            if (i_this->mLookCount != 0) {
                s16 t = i_this->mLookCount;
                i_this->mHeadRotYTarget = -0x3E80;
                if (!(t & 1)) {
                    t = i_this->mLookCount;
                    i_this->mHeadRotYTarget = 0x3E80;
                }
                i_this->mLookCount = t - 1;
                tm[2] = 0x1E;
                r = search_check_(i_this);
            } else {
                i_this->mHeadRotYTarget = 0;
                tm[2] = 0x1E;
                r = search_check_(i_this);
            }
            if (r != 0) {
                i_this->m548 = r;
                i_this->mLookCount = 0;
                i_this->mHeadRotYTarget = 0;
                tm[3] = 0;
                s16 a = cLib_targetAngleY(&i_this->current.pos, &i_this->mHitPos);
                i_this->mMoveAngle = a;
                f32 rnd = cM_rndFX(16000.0f);
                i_this->mStts.m_weight = 100;
                i_this->m3D9 = 2;
                i_this->mMoveAngle = a + (s16)gabi::ftoi(8000.0f - rnd);
                next = fopAcM_searchPlayerDistance(i_this) < 500.0f ? C984 : T0;
                break;
            }
        }
        if (tm[3] == 0) {
            s16 a = cLib_targetAngleY(&i_this->current.pos, &i_this->mHitPos);
            i_this->mMoveAngle = a;
            f32 rnd = cM_rndFX(16000.0f);
            i_this->mStts.m_weight = 100;
            i_this->m3D9 = 2;
            i_this->mMoveAngle = a + (s16)gabi::ftoi(8000.0f - rnd);
        }
        if (fopAcM_searchPlayerDistance(i_this) < 500.0f) next = C984;
        break;
    case 2:
        nz_anm_init(i_this, 0x1F, 0.0f, 2, 1.0f, 0x10);
        i_this->mHeadRotYTarget = 0;
        i_this->mHeadRotY = 0;
        i_this->mSearchCounter = 0;
        i_this->m3D9 = i_this->m3D9 + 1;
        i_this->speed.y = 0.0f;
        i_this->speedF = 20.0f;
        tm[0] = (s16)gabi::ftoi(cM_rndF(30.0f) + 30.0f);
        /* fallthrough */
    case 3: {
        if (i_this->m548 == 0 && abs16(i_this->current.angle.x) > 0x7F80 &&
            !(player->current.pos.y + 400.0f >= i_this->current.pos.y)) {
            s16 c = i_this->mSearchCounter + 1;
            i_this->mSearchCounter = c;
            if ((c & 0x1F) == 0) {
                int r = search_check_(i_this);
                if (r != 0) {
                    i_this->mLookCount = 0;
                    i_this->mHeadRotYTarget = 0;
                    i_this->m548 = r;
                }
                if (i_this->m548 != 0) {
                    next = CE6C;
                    break;
                }
            }
        }
        if (i_this->mHitPos.x == 0.0f && i_this->mHitPos.z == 0.0f) {
            i_this->mHitPos.copy(i_this->current.pos);
        }
        if (i_this->current.angle.x != 0 || i_this->current.angle.z != 0) break;
        if (i_this->mItemId == 0xFFFFFFFF) {
            if (fopAcM_searchPlayerDistance(i_this) < 500.0f) {
                fopAc_ac_c* obj = search_get_obj_(i_this);
                if (obj == nullptr) {
                    next = FACING; /* as C984, without the weight */
                } else {
                    i_this->mMoveAngle = fopAcM_searchActorAngleY(i_this, obj);
                    i_this->mItemId = fopAcM_GetID(obj);
                }
            } else if (tm[0] == 0) {
                i_this->m3D9 = 0;
            }
        } else {
            fopAc_ac_c* obj = judge_id(i_this->mItemId);
            if (obj != nullptr) {
                if (fopAcM_searchActorDistanceXZ(i_this, obj) < 60.0f) {
                    i_this->m3D8 = 3;
                    i_this->m3D9 = 0x1E;
                } else {
                    i_this->mMoveAngle = fopAcM_searchActorAngleY(i_this, obj);
                }
            } else {
                i_this->m547 = 0;
                i_this->mItemId = 0xFFFFFFFF;
                i_this->m3D8 = 0;
                i_this->m3D9 = 6;
            }
        }
        break;
    }
    case 4:
        tm[0] = (s16)gabi::ftoi(cM_rndF(10.0f) + 10.0f);
        nz_anm_init(i_this, 0x23, 5.0f, 2, 1.0f, 0x12);
        i_this->mHeadRotYTarget = 0;
        i_this->mHeadRotY = 0;
        i_this->m3D9 = i_this->m3D9 + 1;
        /* fallthrough */
    case 5:
        cLib_addCalc0(&i_this->speedF, 1.0f, 5.0f);
        i_this->mMoveAngle = fopAcM_searchPlayerAngleY(i_this);
        if (tm[0] == 0) next = CD0C;
        break;
    case 6: {
        nz_anm_init(i_this, 0x1F, 5.0f, 2, 1.0f, 0x10);
        s16 a = fopAcM_searchPlayerAngleY(i_this);
        f32 v = (f32)(s32)(a + 0x8000);
        i_this->mMoveAngle = (s16)gabi::ftoi(v + cM_rndFX(8000.0f));
        i_this->speedF = 20.0f;
        tm[1] = 0x1E;
        i_this->m548 = 0;
        i_this->m3D9 = i_this->m3D9 + 1;
        i_this->mSearchCounter = 0;
        break;
    }
    case 7: {
        if (abs16(i_this->current.angle.x) > 0x7F80 && !(player->current.pos.y + 400.0f >= i_this->current.pos.y)) {
            s16 c = i_this->mSearchCounter + 1;
            i_this->mSearchCounter = c;
            if ((c & 0x1F) == 0) {
                int r = search_check_(i_this);
                if (r != 0) {
                    i_this->mLookCount = 0;
                    i_this->mHeadRotYTarget = 0;
                    i_this->m548 = r;
                }
                if (i_this->m548 != 0) {
                    next = CE6C;
                    break;
                }
            }
        }
        if (i_this->current.angle.x != 0 || i_this->current.angle.z != 0) break;
        if (tm[1] == 0) {
            s16 a = fopAcM_searchPlayerAngleY(i_this);
            f32 v = (f32)(s32)(a + 0x8000);
            i_this->mMoveAngle = (s16)gabi::ftoi(v + cM_rndFX(8000.0f));
            tm[1] = 0x1E;
        }
        if (fopAcM_searchPlayerDistance(i_this) > 1200.0f) i_this->m3D9 = 0;
        break;
    }
    }

    if (next == C984 || next == FACING) {
        /* does the player face it: flee (6), or jump at the player (mode 1) */
        s16 a = fopAcM_searchActorAngleY(player, i_this);
        s16 d = (s16)cLib_distanceAngleS(player->shape_angle.y, a);
        if (next == C984) i_this->mStts.m_weight = 100;
        next = d < 0x2A71 ? CD0C : CB58;
    }
    if (next == CB58) {
        i_this->current.angle.z = 0;
        i_this->m3D8 = 1;
        i_this->m3D9 = 0x14;
    } else if (next == CD0C) {
        i_this->m3D9 = 6;
    }
    if (next == CE6C) {
        i_this->speed.y = 0.0f;
        i_this->speedF = 0.0f;
        i_this->shape_angle.z = 0;
        i_this->m3D8 = 2;
        i_this->m3D9 = 0xA;
        nz_LinChk_dt(linChk);
        return;
    }
    /* T0: turn towards mMoveAngle (along the wall when both side probes hit); fall when nothing hit */
    if (i_this->current.angle.x == 0 && i_this->current.angle.z == 0 && (mask & 0x30) == 0x30) {
        gabi::Local<cXyz> t;
        cXyz_mi(&i_this->mProbeHit[4], t, &i_this->mProbeHit[5]);
        f32 tx = t->x;
        f32 tz = t->z;
        v->x = tx;
        v->y = (f32)t->y;
        v->z = tz;
        s16 a = cM_atan2s(tx, tz) + 0x4000;
        i_this->mMoveAngle = a;
        cLib_addCalcAngleS2(&i_this->current.angle.y, a, 1, 0x1000);
    } else {
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->mMoveAngle, 1, 0x1000);
    }
    if (mask == 0) {
        i_this->speed.y = 0.0f;
        i_this->speedF = 0.0f;
        i_this->shape_angle.z = 0;
        i_this->m3D8 = 2;
        i_this->m3D9 = 0xA;
    }
    nz_LinChk_dt(linChk);
}

/* m3D8 == 1: jumps at the player and steals rupees */
static inline void nz_steal(nz_class* i_this) {
    u32 play = dComIfGp_ea();
    u32 rupees = gabi::load<u16>(gabi::load<u32>(0x101F84DC) + 0x24);
    fopAc_ac_c* player = gabi::at<fopAc_ac_c>(gabi::load<u32>(play + PLAY_PLAYER));
    int base;
    if (rupees < 50) base = 0;
    else if (rupees < 100) base = 8;
    else if (rupees < 300) base = 0x10;
    else base = ((rupees < 500 ? 3 : 4) << 3) & 0xF8;
    s32 total = 0;
    int n = gabi::ftoi(cM_rnd() * 3.0f) + 5;
    for (; n > 0; n--) {
        int k = gabi::ftoi(cM_rnd() * 7.0f);
        s32 item = gabi::load<s32>(0x101C76D8 + (base + k) * 4);
        u32 costAddr = 0x101C76C0 + item * 4;
        s32 have = gabi::load<u16>(gabi::load<u32>(0x101F84DC) + 0x24);
        if (have > gabi::load<s32>(costAddr)) {
            s16 a = (s16)gabi::ftoi(cM_rndFX(32000.0f));
            cMtx_YrotS(calc_mtx(), a);
            gabi::Local<cXyz> ofs;
            gabi::Local<cXyz> d;
            ofs->x = 0.0f;
            ofs->y = 0.0f;
            ofs->z = 40.0f;
            MtxPosition(ofs, d);
            gabi::Local<cXyz> p;
            cXyz_pl(&player->current.pos, p, d);
            gabi::Local<cXyz> pos;
            pos->x = (f32)p->x;
            f32 py = p->y;
            pos->y = py;
            pos->z = (f32)p->z;
            f32 h = cM_rndF(60.0f) + 40.0f;
            pos->y = py + h;
            fopAcM_createItem(pos, item + 1, -1, -1, -1, nullptr, 4, nullptr);
            total += gabi::load<s32>(costAddr);
        }
    }
    play = dComIfGp_ea();
    gabi::store<s32>(play + 0x5B48, gabi::load<s32>(play + 0x5B48) - total);
    i_this->mbStole = 1;
}

static inline void nz_jump_end(nz_class* i_this) {
    if (body_atari_check_(i_this) != 1) return;
    i_this->scale.x = 1.0f;
    i_this->scale.y = 1.0f;
    i_this->mStts.m_weight = 100;
    i_this->scale.z = 1.0f;
    i_this->mSmokeTimer = 0;
    nz_mons_se_start(i_this, 0x481B);
    i_this->shape_angle.x = 0;
    i_this->gravity = -5.0f;
    i_this->m3D9 = 0x34;
}

static inline void nz_jump(nz_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    u8 st = i_this->m3D9;
    if (st < 0x14) {
        nz_jump_end(i_this);
        return;
    }
    switch (st) {
    case 0x14:
        nz_anm_init(i_this, 0x15, 5.0f, 0, 1.0f, 7);
        if (i_this != nullptr) nz_mons_se_start(i_this, 0x481A);
        i_this->m3D9 = i_this->m3D9 + 1;
        i_this->speed.y = 35.0f;
        i_this->current.angle.x = 0;
        i_this->current.angle.z = 0;
        i_this->speedF = 40.0f;
        i_this->current.angle.y = fopAcM_searchPlayerAngleY(i_this);
        i_this->mbStole = 0;
        i_this->mSpinSpeed = 0x1200;
        i_this->mCyl.mObjAt.mSPrm |= 1u;
        i_this->mCyl.mObjAt.mRPrm = 1;
        nz_jump_end(i_this);
        return;
    case 0x15: {
        i_this->shape_angle.y = (s16)i_this->current.angle.y;
        i_this->shape_angle.x = i_this->shape_angle.x + i_this->mSpinSpeed;
        i_this->shape_angle.z = (s16)i_this->current.angle.z;
        if (i_this->mbStole == 0 && i_this->mCyl.ChkAtHit() && !(i_this->mCyl.mGObjAt.mRPrm & 1)) {
            fopAc_ac_c* ac = dCcD_GetAc(&i_this->mCyl.mGObjAt);
            if (ac != nullptr && ac == player) nz_steal(i_this);
        }
        u32 flags = i_this->mAcch.m_flags;
        if (flags & dBgS_Acch::WALL_HIT) {
            i_this->speedF = 0.0f;
            flags = i_this->mAcch.m_flags;
        }
        if (!(flags & dBgS_Acch::GROUND_HIT)) {
            nz_jump_end(i_this);
            return;
        }
        shape_from_angle(i_this);
        i_this->mCyl.mObjAt.mSPrm &= ~1u;
        i_this->m3D9 = i_this->m3D9 + 1;
    }
        /* fallthrough */
    case 0x16: {
        f32 sp = i_this->speedF * 0.5f;
        shape_from_angle(i_this);
        i_this->speedF = sp;
        i_this->speed.y = 20.0f;
        nz_anm_init(i_this, 0x16, 0.0f, 0, 1.0f, -1);
        i_this->m3D9 = i_this->m3D9 + 1;
    }
        /* fallthrough */
    case 0x17:
        shape_from_angle(i_this);
        if (i_this->mAcch.m_flags & dBgS_Acch::WALL_HIT) i_this->speedF = 0.0f;
        cLib_addCalc0(&i_this->speedF, 1.0f, 5.0f);
        if (i_this->mpMorf->isStop()) {
            i_this->speed.y = 0.0f;
            nz_timers(i_this)[0] = (s16)gabi::ftoi(cM_rndF(60.0f) + 60.0f);
            i_this->current.angle.x = 0;
            i_this->current.angle.z = 0;
            i_this->m3D8 = 0;
            i_this->m3D9 = 6;
        }
        nz_jump_end(i_this);
        return;
    default:
        nz_jump_end(i_this);
        return;
    }
}

/* m3D8 == 2: dropped off a ceiling */
static inline void nz_drop(nz_class* i_this) {
    u8 st = i_this->m3D9;
    switch (st) {
    case 0xA: {
        i_this->m3D9 = st + 1;
        s16 a = i_this->current.angle.y - 0x8000;
        i_this->current.angle.y = a;
        i_this->mMoveAngle = a;
        nz_anm_init(i_this, 0x15, 5.0f, 0, 1.0f, 7);
        i_this->current.angle.x = 0;
        i_this->current.angle.z = 0;
        i_this->gravity = -3.0f;
        nz_timers(i_this)[0] = 10;
    }
        /* fallthrough */
    case 0xB:
        i_this->shape_angle.y = (s16)i_this->current.angle.y;
        i_this->shape_angle.x = i_this->shape_angle.x + 0x2500;
        i_this->shape_angle.z = (s16)i_this->current.angle.z;
        if (rakka_line_check_(i_this) || (i_this->mAcch.m_flags & dBgS_Acch::GROUND_HIT)) {
            nz_anm_init(i_this, 0x16, 0.0f, 0, 1.0f, -1);
            i_this->shape_angle.x = 0;
            i_this->shape_angle.y = (s16)i_this->current.angle.y;
            i_this->current.angle.x = 0;
            i_this->shape_angle.z = 0;
            i_this->current.angle.z = 0;
            i_this->speed.y = 0.0f;
            i_this->m3D9 = i_this->m3D9 + 1;
        }
        break;
    case 0xC:
        if (i_this->mpMorf->isStop()) {
            dPa_smokeEcallBack_end((dPa_smokeEcallBack*)&i_this->mSmokeCb);
            nz_timers(i_this)[0] = (s16)gabi::ftoi(cM_rndF(60.0f) + 60.0f);
            i_this->m3D8 = 0;
            i_this->m3D9 = 0;
        }
        break;
    }
}

/* m3D8 == 3 helpers: what it holds */
static inline void nz_carry_end(nz_class* i_this, dBgS_LinChk_l* linChk) {
    u32 id = i_this->mItemId;
    i_this->m547 = 0;
    fopAc_ac_c* item = fopAcM_SearchByID(id);
    if (item != nullptr) {
        if (fopAc_IsActor(item)) {
            i_this->m547 = 1;
        } else {
            i_this->mItemId = 0xFFFFFFFF;
        }
    }
    nz_LinChk_dt(linChk);
}

/* m3D8 == 3: carrying an item (or a bomb) back to its hole */
static inline void nz_carry(nz_class* i_this) {
    dComIfGp_get();
    gabi::Local<dBgS_LinChk_l> linChk;
    nz_LinChk_ct(linChk);
    be<s16>* tm = nz_timers(i_this);
    if (i_this->mType == 0 && fopAcM_SearchByID(i_this->mItemId) == nullptr) {
        i_this->m3D8 = 0;
        i_this->m3D9 = 6;
        i_this->mItemId = 0xFFFFFFFF;
        nz_LinChk_dt(linChk);
        return;
    }
    switch (i_this->m3D9) {
    case 0x1E: {
        fopAc_ac_c* item = fopAcM_SearchByID(i_this->mItemId);
        if (item == nullptr) {
            i_this->m3D8 = 0;
            i_this->speedF = 0.0f;
            i_this->mItemId = 0xFFFFFFFF;
            i_this->m3D9 = 6;
            break;
        }
        i_this->m3D9 = 0x1F;
        if (i_this != nullptr) nz_mons_se_start(i_this, 0x4819);
        s16 a = fopAcM_searchActorAngleY(i_this, item);
        item->gravity = 0.0f;
        item->speedF = 0.0f;
        item->speed.y = 0.0f;
        item->current.angle.y = a;
        if (item != nullptr && fpcM_GetName(item) == PROC_ITEM) daItem_02183150(item);
        i_this->current.angle.x = 0;
        i_this->speed.y = 0.0f;
        i_this->speedF = 0.0f;
        i_this->current.angle.z = 0;
        break;
    }
    case 0x1F:
        nz_anm_init(i_this, 0x1E, 5.0f, 0, 1.0f, 0xF);
        i_this->m3D9 = i_this->m3D9 + 1;
        break;
    case 0x20:
        if (!i_this->mpMorf->isStop()) break;
        nz_anm_init(i_this, 0x20, 0.0f, 2, 1.0f, 0x11);
        i_this->m3D9 = i_this->m3D9 + 1;
        break;
    case 0x21: {
        fopAc_ac_c* item = fopAcM_SearchByID(i_this->mItemId);
        if (item != nullptr && item != nullptr && fpcM_GetName(item) == PROC_BOMB) {
            i_this->mMoveAngle = fopAcM_searchPlayerAngleY(i_this);
            i_this->m3D9 = 0x28;
            break;
        }
        if (item == nullptr) i_this->mItemId = 0xFFFFFFFF;
        if (i_this->mHoleId == 0) {
            fpcM_Search(0x0230B6CC /* s_ana_sub */, i_this);
            fopAcM_SearchByID(i_this->mHoleId);
            i_this->mMoveAngle = fopAcM_searchPlayerAngleY(i_this);
        }
        f32 r = cM_rndF(200.0f);
        tm[1] = 0;
        tm[0] = (s16)gabi::ftoi(r + 200.0f);
        i_this->speedF = 20.0f;
        i_this->m3D9 = i_this->m3D9 + 1;
    }
        /* fallthrough */
    case 0x22: {
        /* run to the hole */
        if (hole_valid(i_this->mHoleId)) {
            fopAc_ac_c* h = judge_id(i_this->mHoleId);
            if (h != nullptr) {
                f32 dy = h->current.pos.y - i_this->current.pos.y;
                if (fopAcM_searchActorDistance(i_this, h) < 40.0f && std_sqrtf(dy * dy) < 100.0f) {
                    gabi::store<u32>(gabi::ea(i_this) + 0x39C, 0); /* attention_info.flags */
                    i_this->current.angle.y = fopAcM_searchActorAngleY(i_this, h);
                    i_this->m3D9 = 0x23;
                    nz_LinChk_dt(linChk);
                    return;
                }
            }
        }
        if (i_this->mCyl.ChkCoHit()) {
            fopAc_ac_c* ac = dCcD_GetAc(&i_this->mCyl.mGObjCo);
            fopAc_ac_c* h = nullptr;
            if (hole_valid(i_this->mHoleId)) h = judge_id(i_this->mHoleId);
            if (ac != nullptr && (i_this->mHoleId == 0 || ac != h)) {
                /* bumped into something: hop back */
                nz_anm_init(i_this, 0x17, 5.0f, 0, 1.0f, 8);
                i_this->current.angle.y = fopAcM_searchActorAngleY(i_this, ac);
                i_this->speedF = -25.0f;
                i_this->m3D9 = 0x29;
                nz_LinChk_dt(linChk);
                return;
            }
        }
        if (tm[1] == 0) {
            s16 pa = fopAcM_searchPlayerAngleY(i_this);
            f32 v = (f32)(s32)(pa + 0x8000);
            s16 ang = (s16)gabi::ftoi(v + cM_rndFX(4000.0f));
            if (hole_valid(i_this->mHoleId)) {
                fopAc_ac_c* h = judge_id(i_this->mHoleId);
                if (h != nullptr) {
                    f32 dy = h->current.pos.y - i_this->current.pos.y;
                    if (fopAcM_searchPlayerDistance(i_this) > 400.0f && std_sqrtf(dy * dy) < 100.0f) {
                        ang = fopAcM_searchActorAngleY(i_this, h);
                    }
                }
            }
            cMtx_YrotS(calc_mtx(), ang);
            gabi::Local<cXyz> ofs;
            gabi::Local<cXyz> p;
            ofs->x = 0.0f;
            ofs->y = 0.0f;
            ofs->z = 300.0f;
            MtxPosition(ofs, p);
            PSVECAdd(p, &i_this->current.pos, p);
            i_this->mProbeHit[7].copy(*p);
            i_this->mProbeEnd[7].copy(i_this->current.pos);
            dBgS_LinChk_Set(linChk, &i_this->current.pos, p, i_this);
            i_this->mMoveAngle = ang;
            tm[1] = 8;
            if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) {
                bool nearHole = false;
                if (hole_valid(i_this->mHoleId)) {
                    fopAc_ac_c* h = judge_id(i_this->mHoleId);
                    if (h != nullptr) {
                        f32 dy = p->y - h->current.pos.y;
                        f32 dx = p->x - h->current.pos.x;
                        f32 dz = p->z - h->current.pos.z;
                        if (std_sqrtf(gabi::fmadds(dz, dz, gabi::fmadds(dx, dx, dy * dy))) < 200.0f) nearHole = true;
                    }
                }
                if (!nearHole) {
                    /* a wall ahead: go along it, away from the player */
                    cXyz* c = LinChk_GetCross(linChk);
                    f32 cx = c->x;
                    f32 cz = c->z;
                    f32 dx = cx - i_this->current.pos.x;
                    f32 dz = cz - i_this->current.pos.z;
                    s16 a1 = cM_atan2s(dx, dz) + 0x6000;
                    s16 a2 = cM_atan2s(dx, dz) - 0x6000;
                    i_this->mMoveAngle = a2;
                    s16 p1 = fopAcM_searchPlayerAngleY(i_this);
                    s16 p2 = fopAcM_searchPlayerAngleY(i_this);
                    s16 d1 = (s16)cLib_distanceAngleS(a1, p1 + 0x8000);
                    s16 d2 = (s16)cLib_distanceAngleS(a2, p2 + 0x8000);
                    if (d1 > d2) i_this->mMoveAngle = a1;
                    tm[1] = 10;
                }
            }
        }
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->mMoveAngle, 1, 0x2000);
        if (tm[0] != 0) break;
        s16 ay = i_this->current.angle.y;
        i_this->mMoveAngle = ay;
        cMtx_YrotS(calc_mtx(), ay);
        gabi::Local<cXyz> ofs;
        gabi::Local<cXyz> p;
        ofs->x = 0.0f;
        ofs->y = 0.0f;
        ofs->z = 400.0f;
        MtxPosition(ofs, p);
        PSVECAdd(p, &i_this->current.pos, p);
        i_this->mProbeHit[7].copy(*p);
        i_this->mProbeEnd[7].copy(i_this->current.pos);
        dBgS_LinChk_Set(linChk, &i_this->current.pos, p, i_this);
        if (cBgS_LineCross(dComIfG_Bgsp(), linChk)) break;
        i_this->m3D9 = 0x24;
        break;
    }
    case 0x23: {
        /* into the hole */
        cLib_addCalc0(&i_this->speedF, 1.0f, 10.0f);
        cLib_addCalc0(&i_this->scale.x, 1.0f, 0.4f);
        f32 s = i_this->scale.x;
        i_this->scale.y = s;
        i_this->scale.z = s;
        fopAc_ac_c* item = fopAcM_SearchByID(i_this->mItemId);
        if (item != nullptr) item->scale.copy(i_this->scale);
        if (!(i_this->scale.x < 0.2f)) break;
        if (hole_valid(i_this->mHoleId)) {
            fopAc_ac_c* h = judge_id(i_this->mHoleId);
            if (h != nullptr) {
                u32 hh = gabi::ea(h);
                if (i_this->parentActorID != 0) {
                    s16 c = gabi::load<s16>(hh + 0x3DA);
                    if (c > 0) gabi::store<s16>(hh + 0x3DA, c - 1);
                } else {
                    gabi::store<s16>(hh + 0x3DC, gabi::load<s16>(hh + 0x3DC) + 1);
                }
                gabi::store<s16>(hh + 0x3DE, 0x14);
            }
        }
        item = fopAcM_SearchByID(i_this->mItemId);
        if (item != nullptr) fopAcM_delete(item);
        i_this->m547 = 0;
        i_this->mItemId = 0xFFFFFFFF;
        fopAcM_delete(i_this);
        break;
    }
    case 0x24:
        nz_anm_init(i_this, 0x1C, 5.0f, 0, 1.0f, 0xD);
        i_this->m3D9 = i_this->m3D9 + 1;
        break;
    case 0x25: {
        /* drops the item */
        if (i_this->mpMorf->checkFrame(5.0f)) {
            nz_item_poi(i_this);
            i_this->mSmokePos.copy(i_this->current.pos);
            smoke_angle_zero(i_this);
            smoke_set_(i_this);
        }
        if (!(i_this->mpMorf->getFrame() < 10.0f)) {
            cLib_addCalc0(&i_this->speedF, 1.0f, 2.0f);
            i_this->mSmokePos.copy(i_this->current.pos);
        }
        if (!i_this->mpMorf->isStop()) break;
        dPa_smokeEcallBack_end((dPa_smokeEcallBack*)&i_this->mSmokeCb);
        tm[2] = 0x1E;
        i_this->m3D9 = i_this->m3D9 + 1;
        break;
    }
    case 0x26:
        cLib_addCalc0(&i_this->speedF, 1.0f, 2.0f);
        if (tm[2] != 0) break;
        nz_anm_init(i_this, 0x1D, 0.0f, 0, 1.0f, 0xE);
        i_this->m3D9 = i_this->m3D9 + 1;
        break;
    case 0x27: {
        if (!i_this->mpMorf->isStop()) break;
        fopAc_ac_c* item = fopAcM_SearchByID(i_this->mItemId);
        u8 type = i_this->mType;
        if (item != nullptr) {
            i_this->m547 = 0;
            i_this->mItemId = 0xFFFFFFFF;
        }
        if (type == 0) {
            i_this->m3D8 = 0;
            i_this->m3D9 = 6;
        } else {
            nz_anm_init(i_this, 0x23, 5.0f, 2, 1.0f, 0x12);
            tm[0] = 0x14;
            i_this->m3D8 = 5;
            i_this->m3D9 = 0x4D;
        }
        break;
    }
    case 0x28:
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->mMoveAngle, 1, 0x2000);
        if ((s16)cLib_distanceAngleS(i_this->mMoveAngle, i_this->current.angle.y) < 0x1000) i_this->m3D9 = 0x24;
        break;
    case 0x29: {
        cLib_addCalc0(&i_this->speedF, 1.0f, 2.0f);
        if (!i_this->mpMorf->checkFrame(17.0f)) break;
        i_this->speedF = 0.0f;
        i_this->m547 = 0;
        fopAc_ac_c* item = fopAcM_SearchByID(i_this->mItemId);
        if (item != nullptr) {
            item->speed.y = 12.0f;
            item->gravity = -3.0f;
            item->current.angle.y = (s16)i_this->current.angle.y;
            daItem_0218319C(item);
            daItem_02183250(item);
        }
        i_this->mItemId = 0xFFFFFFFF;
        i_this->m3D9 = 0x27;
        break;
    }
    }
    nz_carry_end(i_this, linChk);
}

/* 0230BEF4 */
static BOOL daNZ_Execute(nz_class* i_this) {
    WWHD_FUNC(0x0230BEF4, BOOL, i_this);
    i_this->mTailPos[0].copy(i_this->mTailEnd[0]);
    tail_control_(i_this);
    if (enemy_ice(&i_this->mEnemyIce)) {
        J3DModel_setBaseTRMtx(i_this->mpMorf->getModel(), mDoMtx_stack_c::get());
        i_this->mpMorf->calc();
        enemy_fire_remove(&i_this->mEnemyFire);
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
        f32 y = i_this->current.pos.y + 10.0f;
        f32 z = i_this->current.pos.z;
        f32 x = i_this->current.pos.x;
        cXyz* attn = gabi::at<cXyz>(gabi::ea(i_this) + 0x390);
        i_this->eyePos.z = z;
        attn->z = z;
        i_this->eyePos.y = y;
        i_this->eyePos.x = x;
        attn->y = y;
        attn->x = x;
        return TRUE;
    }
    be<s16>* tm = nz_timers(i_this);
    for (int i = 0; i < 5; i++) {
        if (tm[i] != 0) tm[i] = tm[i] - 1;
    }
    i_this->mAttnOffsetY = 70.0f;
    if (i_this->mType == 0) i_this->mBodyOffsetTarget = -50.0f;

    switch (i_this->m3D8) {
    case 0: nz_crawl(i_this); break;
    case 1: nz_jump(i_this); break;
    case 2: nz_drop(i_this); break;
    case 3:
        nz_carry(i_this);
        if (i_this->mType == 0) i_this->mBodyOffsetTarget = 0.0f;
        break;
    case 4:
        i_this->mAttnOffsetY = 10.0f;
        nz5_move_(i_this);
        break;
    case 5:
        i_this->mAttnOffsetY = 90.0f;
        nz6_move_(i_this);
        break;
    }

    u8 mode = i_this->m3D8;
    if (mode == 0 || mode == 3) shape_from_angle(i_this);
    if (i_this->m3D9 != 0x23 && i_this->m3D8 != 4 && i_this->m3D8 != 1) {
        body_atari_check_(i_this);
        if (i_this->scale.x < 1.0f) {
            cLib_addCalc2(&i_this->scale.x, 1.0f, 1.0f, 0.4f);
            f32 s = i_this->scale.x;
            i_this->scale.y = s;
            i_this->scale.z = s;
        }
    }
    cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
    cMtx_XrotM(calc_mtx(), i_this->current.angle.x);
    {
        u8 m = i_this->m3D8;
        gabi::Local<cXyz> fwd;
        fwd->x = 0.0f;
        fwd->y = 0.0f;
        fwd->z = i_this->speedF;
        if ((u32)(m - 1) <= 4) {
            gabi::Local<cXyz> s;
            MtxPosition(fwd, s);
            i_this->speed.x = (f32)s->x;
            f32 vy = i_this->speed.y + -5.0f;
            i_this->speed.z = (f32)s->z;
            i_this->gravity = -5.0f;
            if (vy < -30.0f) i_this->speed.y = -30.0f;
            else i_this->speed.y = vy;
        } else {
            MtxPosition(fwd, &i_this->speed);
        }
    }
    if (i_this->mHitPos.y - 1000.0f > i_this->current.pos.y) fopAcM_delete(i_this);
    fopAcM_posMove(i_this, &i_this->mStts.m_cc_move);

    bool bg = false;
    f32 wallR = 60.0f;
    u8 m8 = i_this->m3D8;
    u8 s9 = i_this->m3D9;
    if (m8 != 0 && m8 != 3) bg = true;
    if (s9 == 0x22 || s9 == 0x4E) {
        wallR = 30.0f;
        bg = true;
    }
    if (m8 == 3 && s9 != 0x23) bg = true;
    naraku_water_check_(i_this);
    bool done = false;
    if (bg) {
        i_this->mAcchCir.SetWall(wallR, wallR);
        BG_check_(i_this);
        if ((i_this->m3D8 == 1 || i_this->m3D8 == 4) && i_this->mType == 0) {
            f32 target = REG_F(8, 12) + 30.0f;
            i_this->mAcch.m_flags |= dBgS_Acch::LINE_CHECK;
            cLib_addCalc2(&i_this->mBgOffsetY, target, 1.0f, 4.5f);
            done = true;
        }
    }
    if (!done) {
        i_this->mAcch.m_flags |= dBgS_Acch::LINE_CHECK;
        cLib_addCalc2(&i_this->mBgOffsetY, 0.0f, 1.0f, 4.5f);
    }
    bool stuck = false;
    if (i_this->m3D8 == 0 && i_this->m3D9 == 3) {
        if (abs16(i_this->current.angle.x) > 0x1000) {
            gabi::Local<cXyz> d;
            cXyz_mi(&i_this->old.pos, d, &i_this->current.pos);
            if (std_sqrtf(PSVECSquareMag(d)) < 1.0f) {
                u8 c = i_this->m3DA + 1;
                i_this->m3DA = c;
                if (c > 10) stuck = true;
            }
        } else {
            i_this->m3DA = 0;
        }
    }
    if (stuck) {
        /* stuck on a wall or ceiling: drop */
        i_this->m3D8 = 2;
        i_this->m3D9 = 0;
        s16 ay = i_this->current.angle.y;
        i_this->shape_angle.z = 0;
        i_this->speed.y = 0.0f;
        i_this->speedF = 0.0f;
        cMtx_YrotS(calc_mtx(), ay);
    } else {
        cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
    }
    cMtx_XrotM(calc_mtx(), i_this->current.angle.x);
    cMtx_ZrotM(calc_mtx(), i_this->current.angle.z);
    cLib_addCalc2(&i_this->mBodyOffset, i_this->mBodyOffsetTarget, 1.0f, 10.0f);
    {
        gabi::Local<cXyz> v;
        gabi::Local<cXyz> r;
        gabi::Local<cXyz> t;
        v->x = 0.0f;
        v->y = 0.0f;
        v->z = i_this->mBodyOffset;
        MtxPosition(v, r);
        cXyz_pl(r, t, &i_this->current.pos);
        i_this->mRipplePos.copy(*t);
    }
    u32 snd = 0;
    if (i_this->mAcch.m_flags & dBgS_Acch::GROUND_HIT) snd = dBgS_GetMtrlSndId(gabi::at<u8>(gabi::ea(&i_this->mAcch) + 0xE8));
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(i_this));
    i_this->mpMorf->play(&i_this->eyePos, snd, reverb);
    i_this->mpMorf->calc();
    i_this->mCyl.SetC(&i_this->mRipplePos);
    i_this->mCyl.SetH(40.0f);
    i_this->mCyl.SetR(40.0f);
    dComIfG_Ccsp_Set(&i_this->mCyl);
    f32 ex = i_this->current.pos.x;
    f32 ez = i_this->current.pos.z;
    i_this->eyePos.x = ex;
    i_this->eyePos.z = ez;
    i_this->eyePos.y = i_this->current.pos.y + 10.0f;
    cXyz* attn = gabi::at<cXyz>(gabi::ea(i_this) + 0x390);
    attn->x = (f32)i_this->mRipplePos.x;
    f32 ay2 = i_this->mRipplePos.y;
    attn->y = ay2;
    attn->z = (f32)i_this->mRipplePos.z;
    attn->y = ay2 + i_this->mAttnOffsetY;
    J3DModel* model = i_this->mpMorf->getModel();
    u32 m = gabi::ea(model);
    gabi::store<f32>(m + 0xBC, i_this->scale.x); /* setBaseScale(scale) */
    gabi::store<f32>(m + 0xC0, i_this->scale.y);
    gabi::store<f32>(m + 0xC4, i_this->scale.z);
    mDoMtx_stack_transS(i_this->mRipplePos);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), i_this->shape_angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->shape_angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), i_this->shape_angle.z);
    mDoMtx_stack_transM(0.0f, -i_this->mBgOffsetY, 0.0f);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    i_this->mpMorf->calc();
    enemy_fire(&i_this->mEnemyFire);

    if (i_this->m547 != 0) {
        fopAc_ac_c* item = nz_SearchByID_(i_this->mItemId);
        if (item != nullptr) {
            /* the item sits between the hands */
            f32 dy = i_this->mHandPos1.y - i_this->mHandPos0.y;
            f32 dx = i_this->mHandPos1.x - i_this->mHandPos0.x;
            f32 dz = i_this->mHandPos1.z - i_this->mHandPos0.z;
            cMtx_YrotS(calc_mtx(), i_this->shape_angle.y);
            gabi::Local<cXyz> zero;
            gabi::Local<cXyz> ofs;
            zero->x = 0.0f;
            zero->y = 0.0f;
            zero->z = 0.0f;
            MtxPosition(zero, ofs);
            ofs->y = i_this->mType != 0 ? -10.0f : -20.0f;
            item->current.pos.x = gabi::fnmsubs(dx, 0.5f, i_this->mHandPos1.x) + ofs->x;
            item->current.pos.y = gabi::fnmsubs(dy, 0.5f, i_this->mHandPos1.y) + ofs->y;
            item->current.pos.z = gabi::fnmsubs(dz, 0.5f, i_this->mHandPos1.z) + ofs->z;
        }
    }
    return TRUE;
}
VERIFY(0x0230BEF4, daNZ_Execute);

/* ---- leftover functions of the translation unit ---- */

/* 023111D4 sead::SafeStringBase<char>::assureTerminationImpl_ (empty); vtable slot 100242F8, after the destructor 0230F8A8 */
static void nz_SafeString_assureTermination(void* p) {
    WWHD_FUNC(0x023111D4, void, p);
}
VERIFY(0x023111D4, nz_SafeString_assureTermination);
