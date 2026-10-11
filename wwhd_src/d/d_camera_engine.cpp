/**
 * d_camera_engine.cpp (WWHD)
 * Follow camera dCamera_c: camera engines (engine_tbl) - non-owner, fixed frame/position, crawl,
 * hookshot and the other smaller algorithms.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/d_camera.cpp) to the WWHD layout and code, verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/d_camera_engine.h"

/* stack dBgS_CamLinChk (same as d_camera_bg.cpp camLinChk_ct / dt) */
namespace camera_engine {
constexpr f32 kOffsets[4][3] = {{180.0f, 40.0f, -120.0f},
                                {160.0f, 80.0f, -160.0f},
                                {150.0f, -80.0f, -100.0f},
                                {100.0f, -120.0f, -140.0f}};
static CanonPointer canonPointer;
static void camLinChk_ct_engine(dBgS_LinChk* o) {
    cameraEngineLineCtor(o);
    o->mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk*>(o));
    o->mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk*>(o));
    o->cBgS_Chk::__vtbl = CAM_LINE;
    o->cBgS_PolyInfo::__vtbl = CAM_POLY_INFO;
    o->dBgS_PolyPassChk::__vtbl = CAM_POLY_PASS;
    o->dBgS_GrpPassChk::__vtbl = CAM_GROUP_PASS;
    o->mbObjThrough = 0;
    o->mbCamThrough = 1;
    o->mbLinkThrough = 0;
    o->mbArrowThrough = 0;
    o->mbBombThrough = 0;
    o->mbBoomerangThrough = 0;
    o->mbRopeThrough = 0;
    o->mGrp = 3;
}
static void camLinChk_dt_engine(dBgS_LinChk* o) {
    o->dBgS_PolyPassChk::__vtbl = BASE_POLY_PASS;
    o->cBgS_PolyInfo::__vtbl = BASE_POLY_INFO;
    o->dBgS_GrpPassChk::__vtbl = BASE_GROUP_PASS;
    cameraEngineLineDtor(o, 0);
}
} // namespace camera_engine
using namespace camera_engine;
/* 0251485C */
// HD: the per-frame direction follows the cannon's pitch on V and its yaw on U (the GameCube source
// swapped the setters' names)
bool dCamera_c::nonOwnerCamera(s32 style) {
    fopAc_ac_c* canon = canonPointer.get(); /* daCanon_c::getCanonPtr() */
    mpLockonTarget = canon;
    if (canon == nullptr)
        return false;
    cXyz ofs;
    cXyz c;
    cXyz x;
    cXyz p;
    cXyz d;
    cXyz s;
    u32 first = m11C;
    ofs.y = camParamVal(style, 5);
    f32 ang = camParamVal(style, 23);
    f32 vAng = camParamVal(style, 15);
    f32 speed = camParamVal(style, 20);
    f32 fovy = camParamVal(style, 25);
    ofs.x = camParamVal(style, 1);
    f32 dist = camParamVal(style, 10);
    ofs.z = camParamVal(style, 0);
    f32 vScale = camParamVal(style, 18);
    if (first == 0) {
        cSAngle_l dir;
        relationalPos(&c, canon, &ofs);
        mViewCache.mCenter.copy(c);
        directionOf(&dir, mpLockonTarget.get());
        s16 inv = cSAngle_Inv(&dir);
        s16 v = (s16)gabi::ftoi(vAng * 182.04445f);
        cameraEngineGlobeValShort(&mViewCache.mDirection, dist, v,
                                  inv); /* mViewCache.mDirection.Val(dist, v, inv) */
        cameraEngineGlobeXyz(&mViewCache.mDirection, &x);
        cXyz_pl(&mViewCache.mCenter, &c, &x);
        m100 = 1;
        m101 = 1;
        mViewCache.mEye.copy(c);
        m102 = 1;
        mViewCache.mFovy = fovy;
        return true;
    }
    cSGlobe_l g;
    cSAngle_l t;
    cSAngle_l a;
    cSAngle_l b;
    cSAngle_l cc;
    cSAngle_l dd;
    cSAngle_l u;
    cSAngle_l w;
    cSAngle_l xx;
    cSAngle_l y;
    cSAngle_l tmp;
    cSAngle_l dir;
    cameraEngineGlobeFromVector(&g, &ofs);
    cameraEngineAngleSubtractShort(&g.mV, &t,
                                   (s16)mpLockonTarget->shape_angle.x); /* V - shape_angle.x */
    cSAngle_l* r = cameraEngineAngleShortCtor(&tmp, (s16)t);
    g.mV = (s16)*r;
    cameraEngineAngleAddShort(&g.mU, &t,
                              (s16)mpLockonTarget->shape_angle.y); /* U + shape_angle.y */
    r = cameraEngineAngleShortCtor(&tmp, (s16)t);
    g.mU = (s16)*r;
    attentionPos(&c, mpLockonTarget.get());
    cameraEngineGlobeXyz(&g, &x);
    cXyz_pl(&c, &p, &x);
    cXyz_mi(&p, &d, &mViewCache.mCenter);
    cXyz_ml(&d, &s, 0.2f);
    PSVECAdd(&mViewCache.mCenter, &s, &mViewCache.mCenter);
    cameraEngineAngleFloatCtor(&a, vAng); /* cSAngle(f32) */
    cSAngle_ct(&b);
    cameraEngineAngleShortCtor(&cc, (s16)0);
    cSAngle_ct(&dd);
    cSAngle_Val(&b, (s16)mpLockonTarget->shape_angle.x);
    directionOf(&dir, mpLockonTarget.get());
    cSAngle_Val(&dd, cSAngle_Inv(&dir));
    cameraEngineAngleScale(&b, &t, vScale);
    cameraEngineAngleAdd(&a, &u, &t);
    cameraEngineAngleSubtract(&u, &w, &mViewCache.mDirection.mV);
    cameraEngineAngleScale(&w, &xx, speed);
    cameraEngineAngleAdd(&mViewCache.mDirection.mV, &y, &xx);
    r = cameraEngineAngleShortCtor(&tmp, (s16)y);
    mViewCache.mDirection.mV = (s16)*r;
    cameraEngineAngleScale(&dd, &y, ang);
    cameraEngineAngleAdd(&cc, &xx, &y);
    cameraEngineAngleSubtract(&xx, &w, &mViewCache.mDirection.mU);
    cameraEngineAngleScale(&w, &u, speed);
    cameraEngineAngleAdd(&mViewCache.mDirection.mU, &t, &u);
    r = cameraEngineAngleShortCtor(&tmp, (s16)t);
    f32 rr = mViewCache.mDirection.mRadius;
    mViewCache.mDirection.mU = (s16)*r;
    mViewCache.mDirection.mRadius = gabi::fmadds(dist - rr, speed, rr);
    cameraEngineGlobeXyz(&mViewCache.mDirection, &x);
    cXyz_pl(&mViewCache.mCenter, &d, &x);
    f32 fv = mViewCache.mFovy;
    mViewCache.mEye.copy(d);
    mViewCache.mFovy = gabi::fmadds(fovy - fv, speed, fv);
    return true;
}

/* work area of fixedFrameCamera (mWork, GC Work::FixedFrame) */

/* 0250C77C */
// HD: the nearest point on the arrow line and the minimum distance are applied to the centre (m39C;
// GameCube used m3A8); the frame count is k+1 with a weight sum (k+1)(k+2)/2
bool dCamera_c::fixedFrameCamera(s32 style) {
    f32 v24 = camParamVal(style, 24);
    FixedFrame* p = (FixedFrame*)mWork;
    f32 fovy = camParamVal(style, 25);
    if (mCurArrowIdx == 0xFF)
        return false;
    if (m11C == 0) {
        cXyz t;
        cXyz x;
        cXyz att;
        cXyz out;
        cXyz d;
        cXyz t2;
        cSGlobe_l g;
        LineFrame line;
        GuestFloat tparam;
        VectorWords* arrowWords = reinterpret_cast<VectorWords*>(
            &arrow(this)->position); /* mCurRoomArrowEntry.position */
        u32 ax = (u32)arrowWords->x, ay = (u32)arrowWords->y, az = (u32)arrowWords->z;
        reinterpret_cast<VectorWords*>(&p->m378)->x = ax;
        reinterpret_cast<VectorWords*>(&p->m378)->y = ay;
        reinterpret_cast<VectorWords*>(&p->m378)->z = az;
        s16 angX = arrow(this)->angle.x;
        cameraEngineGlobeCtor(&g, 3000.0f, (s16)-angX,
                              (s16)arrow(this)->angle.y); /* cSGlobe(3000, -angle.x, angle.y) */
        cameraEngineGlobeXyz(&g, &x);
        cXyz_pl(&p->m378, &t, &x);
        p->m3A8.copy(t);
        p->m39C.copy(t);
        cameraEngineLineSet(&line, &p->m39C, &p->m378); /* cM3dGLin line(m39C, m378) */
        attentionPos(&att, mpPlayerActor.get());
        if (cameraEnginePointAndLine(&line, &att, &out, &tparam)) { /* cM3d_Len3dSqPntAndSegLine */
            p->m39C.copy(out);
        }
        cXyz_mi(&p->m378, &d, &p->m39C);
        cSGlobe_Val(&p->m384, &d);
        if (m530 != 0 && p->m384.mRadius < 350.0f) {
            g.mRadius = 350.0f;
            cameraEngineGlobeXyz(&g, &x);
            cXyz_pl(&p->m378, &t2, &x);
            p->m39C.copy(t2);
            p->m384.mRadius = 350.0f;
        }
        u32 fv = entry(this)->field_0x12; /* mCurRoomCamEntry.field_0x12 */
        if (fv != 0xFF)
            fovy = (f32)fv;
        p->m398 = fovy;
        u32 n = entry(this)->field_0x13; /* mCurRoomCamEntry.field_0x13 */
        if (n == 0xFF) {
            cXyz_mi(&p->m378, &d, &mEye);
            f32 len = std_sqrtf(PSVECSquareMag(&d));
            f32 h = heightOf(mpPlayerActor.get());
            f32 r = std_sqrtf(len / h);
            s32 k = gabi::ftoi(v24 * r);
            s32 frames = k + 1;
            p->m38C = frames;
            p->m390 = (f32)((frames * (k + 2)) >> 1);
        } else {
            s32 frames = (s32)n;
            p->m38C = frames;
            p->m390 = (f32)((frames * (frames + 1)) >> 1);
        }
    }
    if (m100 == 0) {
        cXyz d;
        cXyz s;
        cXyz cr;
        cSAngle_l t;
        cSAngle_l u;
        cSAngle_l w;
        cSAngle_l tmp;
        f32 r = (f32)((s32)p->m38C - (s32)m11C);
        f32 sum = p->m390;
        p->m394 = r;
        f32 k = r / sum;
        p->m390 = sum - r;
        cXyz_mi(&p->m39C, &d, &mViewCache.mCenter);
        cXyz_ml(&d, &s, k);
        PSVECAdd(&mViewCache.mCenter, &s, &mViewCache.mCenter);
        f32 rr = mViewCache.mDirection.mRadius;
        mViewCache.mDirection.mRadius = gabi::fmadds(p->m384.mRadius - rr, k, rr);
        cameraEngineAngleSubtract(&p->m384.mV, &t, &mViewCache.mDirection.mV);
        cameraEngineAngleScale(&t, &u, k);
        cameraEngineAngleAdd(&mViewCache.mDirection.mV, &w, &u);
        cSAngle_l* a = cameraEngineAngleShortCtor(&tmp, (s16)w);
        mViewCache.mDirection.mV = (s16)*a;
        cameraEngineAngleSubtract(&p->m384.mU, &w, &mViewCache.mDirection.mU);
        cameraEngineAngleScale(&w, &u, k);
        cameraEngineAngleAdd(&mViewCache.mDirection.mU, &t, &u);
        a = cameraEngineAngleShortCtor(&tmp, (s16)t);
        mViewCache.mDirection.mU = (s16)*a;
        cameraEngineGlobeXyz(&mViewCache.mDirection, &s);
        cXyz_pl(&mViewCache.mCenter, &d, &s);
        mViewCache.mEye.copy(d);
        if (cameraEngineLineCross(this, &mViewCache.mEye, &mViewCache.mCenter, &cr, 0x7F)) {
            mViewCache.mCenter.copy(cr);
        }
        f32 fv = mViewCache.mFovy;
        mViewCache.mFovy = gabi::fmadds(p->m398 - fv, k, fv);
        if ((u32)m11C >= (u32)((s32)p->m38C - 1)) {
            m102 = 1;
            m101 = 1;
            m100 = 1;
        }
    }
    return true;
}

/* work area of fixedPositionCamera (mWork, GC Work::FixedPos) */

/* moves mViewCache.mDirection towards g by k and rebuilds the eye and fovy */
/* inlined into dCamera_c::fixedPositionCamera,dCamera_c::hookshotCamera */
static void fixedPosApproach(dCamera_c* c, cSGlobe_l* g, f32 k, f32 fovy, f32 minR, f32 maxR) {
    cSAngle_l t;
    cSAngle_l u;
    cSAngle_l w;
    cSAngle_l tmp;
    cXyz x;
    cXyz e;
    f32 r = g->mRadius;
    if (r < minR) {
        r = minR;
        g->mRadius = r;
    }
    f32 vr = c->mViewCache.mDirection.mRadius;
    if (r > maxR) {
        r = maxR;
        g->mRadius = r;
    }
    c->mViewCache.mDirection.mRadius = gabi::fmadds(r - vr, k, vr);
    cameraEngineAngleSubtract(&g->mV, &t, &c->mViewCache.mDirection.mV);
    cameraEngineAngleScale(&t, &u, k);
    cameraEngineAngleAdd(&c->mViewCache.mDirection.mV, &w, &u);
    cSAngle_l* a = cameraEngineAngleShortCtor(&tmp, (s16)w);
    c->mViewCache.mDirection.mV = (s16)*a;
    cameraEngineAngleSubtract(&g->mU, &w, &c->mViewCache.mDirection.mU);
    cameraEngineAngleScale(&w, &u, k);
    cameraEngineAngleAdd(&c->mViewCache.mDirection.mU, &t, &u);
    a = cameraEngineAngleShortCtor(&tmp, (s16)t);
    c->mViewCache.mDirection.mU = (s16)*a;
    cameraEngineGlobeXyz(&c->mViewCache.mDirection, &x);
    cXyz_pl(&c->mViewCache.mCenter, &e, &x);
    f32 fv = c->mViewCache.mFovy;
    c->mViewCache.mEye.copy(e);
    c->mViewCache.mFovy = gabi::fmadds(fovy - fv, k, fv);
}

/* 0250CC80 */
// HD: the DMC angle is only taken when none is active; the first-frame distance uses max(height,
// 10); the map frame count is used as the weight sum directly
bool dCamera_c::fixedPositionCamera(s32 style) {
    FixedPos* p = (FixedPos*)mWork;
    u32 first = m11C;
    f32 v0 = camParamVal(style, 0), v1 = camParamVal(style, 1), v10 = camParamVal(style, 10),
        v11 = camParamVal(style, 11);
    f32 v24 = camParamVal(style, 24), v4 = camParamVal(style, 4), v5 = camParamVal(style, 5),
        fovy = camParamVal(style, 25);
    f32 v23 = camParamVal(style, 23), v3 = camParamVal(style, 3);
    if (first == 0)
        p->m390.copy(zeroVector()); /* cXyz::Zero */
    p->m39C = 0;
    cXyz pos;
    cXyz ofs;
    cXyz mul;
    cXyz target;
    cXyz d;
    cXyz s;
    cXyz c;
    s32 frames = -1;
    if ((styleEntry(style)->mFlags & dCamPrmFlg_UNK040) && mCurArrowIdx != 0xFF) {
        pos.copy(arrow(this)->position); /* mCurRoomArrowEntry.position */
        if (cameraEngineVectorNotEqual(&p->m390, &pos) && mDMCSystem.field_0x0 == 0)
            setDMCAngle(); /* cXyz::operator!= */
        p->m390.copy(pos);
        u32 fv = entry(this)->field_0x12;
        u32 n = entry(this)->field_0x13;
        if (fv != 0xFF)
            fovy = (f32)fv;
        if (n != 0xFF) {
            p->m39C = 1;
            frames = (s32)n;
        }
    } else {
        pos.copy(mEye);
    }
    ofs.y = v5;
    ofs.x = v1;
    ofs.z = v0;
    if (m11C == 0) {
        if (p->m39C == 0) {
            cXyz_mi(&pos, &d, &mEye);
            f32 l = std_sqrtf(PSVECSquareMag(&d));
            if (l > v10)
                l = v10;
            relationalPos(&c, mpPlayerActor.get(), &ofs);
            cXyz_mi(&mCenter, &d, &c);
            f32 l2 = std_sqrtf(PSVECSquareMag(&d));
            f32 m = (l2 - l >= 0.0f) ? l2 : l; /* fsel */
            f32 h = heightOf(mpPlayerActor.get());
            h = (h - 10.0f >= 0.0f) ? h : 10.0f; /* fsel */
            f32 r = std_sqrtf(m / h);
            s32 k = gabi::ftoi(v24 * r);
            s32 n = k + 1;
            p->m378 = n;
            p->m37C = (f32)((n * (k + 2)) >> 1);
        } else {
            p->m378 = frames;
            p->m37C = (f32)frames;
        }
        p->m384.copy(mViewCache.mCenter);
    }
    mul.x = v3;
    mul.y = v4;
    mul.z = v3;
    relationalPos(&target, mpPlayerActor.get(), &ofs);
    cSGlobe_l g;
    if (m100 == 0) {
        f32 k;
        if (p->m39C == 0) {
            f32 r = (f32)((s32)p->m378 - (s32)m11C);
            f32 sum = p->m37C;
            p->m380 = r;
            k = r / sum;
            p->m37C = sum - r;
        } else {
            f32 sum = p->m37C;
            k = 1.0f / sum;
            p->m37C = sum - 1.0f;
        }
        cXyz_mi(&target, &d, &p->m384);
        cXyz_ml(&d, &s, k);
        PSVECAdd(&p->m384, &s, &p->m384);
        cXyz_mi(&p->m384, &s, &mViewCache.mCenter);
        cameraEngineVectorMultiply(&s, &d, &mul); /* cXyz::operator*(const cXyz&) */
        PSVECAdd(&mViewCache.mCenter, &d, &mViewCache.mCenter);
        cXyz_mi(&pos, &d, &mViewCache.mCenter);
        cameraEngineGlobeFromVector(&g, &d);
        fixedPosApproach(this, &g, k, fovy, v11, v10);
        if ((u32)m11C >= (u32)((s32)p->m378 - 1)) {
            m102 = 1;
            m101 = 1;
            m100 = 1;
        }
        return true;
    }
    cXyz_mi(&target, &d, &mViewCache.mCenter);
    cameraEngineVectorMultiply(&d, &s, &mul);
    PSVECAdd(&mViewCache.mCenter, &s, &mViewCache.mCenter);
    cXyz_mi(&pos, &d, &mViewCache.mCenter);
    cameraEngineGlobeFromVector(&g, &d);
    fixedPosApproach(this, &g, v23, fovy, v11, v10);
    return true;
}

/* work area of crawlCamera (mWork, GC Work::Crawl) */

static inline bool angGt(s16 a, cSAngle_l* lim) {
    return a > (s16)*lim;
}
static inline bool angLt(s16 a, cSAngle_l* lim) {
    return a < (s16)*lim;
}

/* 02511200 */
// HD: the tunnel camera eases U by 0.25 per frame (GameCube 0.2); the arrow eye is used while the
// camera faces along the map arrow
bool dCamera_c::crawlCamera(s32 style) {
    Crawl* p = (Crawl*)mWork;
    cXyz ofs;
    cXyz mul;
    cXyz c;
    cXyz d;
    cXyz s;
    cXyz x;
    cXyz e;
    cXyz pp;
    cSAngle_l dir;
    cSAngle_l rel;
    cSAngle_l t;
    cSAngle_l u;
    cSAngle_l w;
    cSAngle_l a;
    cSAngle_l vNew;
    cSAngle_l uNew;
    cSAngle_l sp70;
    cSAngle_l sp74;
    cSAngle_l sp78;
    cSAngle_l tmp;
    Style* cameraStyle = styleEntry(style);
    ofs.x = camParamVal(style, 1);
    f32 v3 = camParamVal(style, 3);
    f32 v4 = camParamVal(style, 4);
    ofs.z = camParamVal(style, 0);
    f32 fovy = camParamVal(style, 25);
    ofs.y = camParamVal(style, 5);
    f32 v10 = camParamVal(style, 10);
    directionOf(&dir, mpPlayerActor.get());
    cameraEngineAngleSubtract(&dir, &rel, &mViewCache.mDirection.mU); /* local_118 */
    u16 flags;
    if (m108 == 0) {
        p->m378 = CRAWL_WORK; /* 'CRWL' */
        flags = cameraStyle->mFlags;
        p->m37C = 10;
        p->m398 = (flags & dCamPrmFlg_UNK080) != 0;
        p->m399 = 0;
        s32 pad = mPadId;
        if ((dComIfGp_checkPlayerStatus0(pad, daPyStts0_CRAWL_e)) && m1AE != 0)
            p->m37C = 6;
        s32 n = p->m37C;
        p->m39C = 5;
        p->m380 = (f32)((n * (n + 1)) >> 1);
        if ((flags & dCamPrmFlg_UNK040) && angGt(rel, &angle270()) && angLt(rel, &angle90()))
            p->m388 = 0; /* _270 < rel < _90 */
        else
            p->m388 = 1;
    } else {
        flags = cameraStyle->mFlags;
        if (!(flags & dCamPrmFlg_UNK040)) {
            p->m388 = 1;
        } else {
            cSAngle_l* lim = cameraEngineAngleFloatCtor(&a, -75.0f);
            bool done = false;
            if ((s16)rel > (s16)*lim) {
                lim = cameraEngineAngleFloatCtor(&a, 75.0f);
                if ((s16)rel < (s16)*lim) {
                    p->m388 = 0;
                    done = true;
                }
            }
            if (!done) {
                lim = cameraEngineAngleFloatCtor(&a, -135.0f);
                if ((s16)rel < (s16)*lim) {
                    p->m388 = 1;
                } else {
                    lim = cameraEngineAngleFloatCtor(&a, 135.0f);
                    if ((s16)rel > (s16)*lim)
                        p->m388 = 1;
                }
            }
        }
    }
    if (m1AE != 0)
        setComStat(dCamAttnStts_00000080_e);
    bool arrowEye = false;
    if ((flags & dCamPrmFlg_UNK040) && mCurArrowIdx != 0xFF && entry(this)->field_0x12 != 0xFF) {
        cameraEngineAngleSubtractShort(&mViewCache.mDirection.mU, &t,
                                       (s16)arrow(this)->angle.y); /* U - arrow.angle.y */
        s16 r = t;
        rel = r;
        if (angGt(r, &angle270()) && angLt(r, &angle90())) {
            p->m38C.copy(arrow(this)->position); /* mCurRoomArrowEntry.position */
            p->m399 = 1;
            mul.x = v3;
            m102 = 1;
            m101 = 1;
            m100 = 1;
            mul.y = v4;
            mul.z = v3;
            arrowEye = true;
        }
    } else {
        p->m399 = 0;
    }
    if (!arrowEye && m100 == 0) {
        f32 r = (f32)((s32)p->m37C - (s32)m108);
        f32 k = r / p->m380;
        p->m384 = r;
        relationalPos(&c, mpPlayerActor.get(), &ofs);
        cXyz_mi(&c, &d, &mViewCache.mCenter);
        cXyz_ml(&d, &s, k);
        PSVECAdd(&mViewCache.mCenter, &s, &mViewCache.mCenter);
        f32 vr = mViewCache.mDirection.mRadius;
        s16 sx = mpPlayerActor->shape_angle.x; /* shape_angle.x */
        f32 dist = (flags & dCamPrmFlg_UNK080) ? 2.0f : v10;
        f32 rad = gabi::fmadds(dist - vr, k, vr);
        cameraEngineShortSubtractAngle(&t, sx, &mViewCache.mDirection.mV); /* shape_angle.x - V */
        cameraEngineAngleScale(&t, &u, k);
        cameraEngineAngleAdd(&mViewCache.mDirection.mV, &vNew, &u);
        cSAngle_ct(&sp70);
        if (p->m388 != 0) {
            directionOf(&dir, mpPlayerActor.get());
            cSAngle_Val(&sp70, cSAngle_Inv(&dir));
        } else {
            directionOf(&dir, mpPlayerActor.get());
            cameraEngineAngleVal(&sp70, &dir); /* Val(const cSAngle&) */
        }
        cameraEngineAngleSubtract(&sp70, &w, &mViewCache.mDirection.mU);
        cameraEngineAngleScale(&w, &t, k);
        cameraEngineAngleAdd(&mViewCache.mDirection.mU, &uNew, &t);
        cameraEngineGlobeValAngles(&mViewCache.mDirection, rad, &vNew, &uNew); /* Val(f32, V, U) */
        cameraEngineGlobeXyz(&mViewCache.mDirection, &x);
        cXyz_pl(&mViewCache.mCenter, &e, &x);
        f32 fv = mViewCache.mFovy;
        mViewCache.mEye.copy(e);
        mViewCache.mFovy = gabi::fmadds(fovy - fv, k, fv);
        if ((u32)m108 >= (u32)((s32)p->m37C - 1)) {
            m102 = 1;
            m101 = 1;
            m100 = 1;
        }
        p->m380 = p->m380 - p->m384;
        return true;
    }
    if (!arrowEye) {
        mul.x = v3;
        mul.y = v4;
        mul.z = v3;
    }
    relationalPos(&c, mpPlayerActor.get(), &ofs);
    cXyz_mi(&c, &d, &mViewCache.mCenter);
    cameraEngineVectorMultiply(&d, &s, &mul); /* * cXyz */
    PSVECAdd(&mViewCache.mCenter, &s, &mViewCache.mCenter);
    if (p->m399 == 1 && !cameraEngineLineBlocked(this, &mViewCache.mCenter, &p->m38C, 0x7F)) {
        mViewCache.mEye.copy(p->m38C);
        cXyz_mi(&mViewCache.mEye, &d, &mViewCache.mCenter);
        cSGlobe_Val(&mViewCache.mDirection, &d);
        if (mDMCSystem.field_0x0 == 0)
            setDMCAngle();
        f32 fv = mViewCache.mFovy;
        m1AE = 0;
        mViewCache.mFovy = gabi::fmadds(fovy - fv, 0.5f, fv);
        return true;
    }
    cSAngle_ct(&sp78);
    cSAngle_ct(&sp74);
    cSAngle_ct(&sp70);
    if (p->m388 != 0) {
        directionOf(&dir, mpPlayerActor.get());
        cSAngle_Val(&sp70, cSAngle_Inv(&dir));
    } else {
        directionOf(&dir, mpPlayerActor.get());
        cameraEngineAngleVal(&sp70, &dir);
    }
    f32 rad;
    if (p->m398 == 0) {
        f32 vr = mViewCache.mDirection.mRadius;
        rad = gabi::fmadds(v10 - vr, 0.25f, vr);
        cameraEngineAngleSubtract(&sp70, &w, &mViewCache.mDirection.mU);
        cameraEngineAngleScale(&w, &t, 0.25f);
        cameraEngineAngleAdd(&mViewCache.mDirection.mU, &uNew, &t);
        s16 nu = uNew;
        s16 sx = mpPlayerActor->shape_angle.x;
        sp78 = nu;
        cameraEngineShortSubtractAngle(&t, sx, &mViewCache.mDirection.mV);
    } else {
        positionOf(&pp, mpPlayerActor.get());
        f32 vr = mViewCache.mDirection.mRadius;
        mViewCache.mCenter.y = pp.y + 30.0f;
        rad = gabi::fmadds(2.0f - vr, 0.8f, vr);
        cameraEngineAngleSubtract(&sp70, &w, &mViewCache.mDirection.mU);
        cameraEngineAngleScale(&w, &t, 0.8f);
        cameraEngineAngleAdd(&mViewCache.mDirection.mU, &uNew, &t);
        u32 f = mEventFlags;
        s16 nu = uNew;
        s16 sx = mpPlayerActor->shape_angle.x;
        sp78 = nu;
        mEventFlags = f | 0x800;
        cameraEngineShortSubtractAngle(&t, sx, &mViewCache.mDirection.mV);
    }
    cameraEngineAngleScale(&t, &u, 0.25f);
    cameraEngineAngleAdd(&mViewCache.mDirection.mV, &vNew, &u);
    sp74 = (s16)vNew;
    cameraEngineGlobeValAngles(&mViewCache.mDirection, rad, &sp74, &sp78);
    cameraEngineGlobeXyz(&mViewCache.mDirection, &x);
    cXyz_pl(&mViewCache.mCenter, &e, &x);
    mViewCache.mEye.copy(e);
    f32 fv = mViewCache.mFovy;
    mViewCache.mFovy = gabi::fmadds(fovy - fv, 0.5f, fv);
    return true;
}

/* 02511B5C */
/* HD: (GameCube: not decompiled; written from the WWHD code). First frame: when the hookshot
 * tip is far enough away, up to eight eye candidates around the tip (four offsets, mirrored every
 * 16 frames and after the fourth try) are tested for a clear line to the player; the first clear
 * one is kept. Later frames: the centre follows the player and the direction eases towards the
 * kept eye (immediately after 8 frames) or the current eye. */
bool dCamera_c::hookshotCamera(s32 style) {

    HookWork* work = reinterpret_cast<HookWork*>(
        mWork); /* mWork: 'HOOK', +8 centre, +0x14 eye, +0x20 eye found */
    cXyz ofs;
    cXyz mul;
    cXyz pp;
    cXyz tp;
    cXyz d;
    cXyz v;
    cXyz x;
    cXyz e;
    cXyz c;
    cXyz s;
    cXyz eye;
    f32 fovy = camParamVal(style, 25);
    f32 maxR = camParamVal(style, 10);
    f32 minDist = camParamVal(style, 14);
    f32 v4 = camParamVal(style, 4);
    f32 minR = camParamVal(style, 11);
    f32 v3 = camParamVal(style, 3);
    f32 speed = camParamVal(style, 23);
    ofs.x = camParamVal(style, 1);
    ofs.y = camParamVal(style, 5);
    ofs.z = camParamVal(style, 0);
    if (m11C == 0) {
        work->m378 = HOOKSHOT_WORK; /* 'HOOK' */
        work->m380.copy(mViewCache.mCenter);
        fopAc_ac_c* player = mpPlayerActor.get();
        u32 id = hookshotTipID(player);
        work->m398 = 0;
        if (id != 0xFFFFFFFFu) {
            GuestWord key;
            key = id;
            fopAc_ac_c* tip = fopAcIt_Judge(JUDGE_BY_ID, &key);
            TipView* tipView = reinterpret_cast<TipView*>(tip); /* tip angles (s16 x, s16 y) */
            positionOf(&pp, mpPlayerActor.get());
            positionOf(&tp, tip);
            cXyz_mi(&tp, &d, &pp);
            if (std_sqrtf(PSVECSquareMag(&d)) > minDist) {
                u32 mirror = ((u32)m07C >> 4) & 1;
                u32 cnt = (u32)m07C;
                for (s32 i = 0; i < 8; i++) {
                    cSGlobe_l g;
                    cSAngle_l t;
                    cSAngle_l tmp;
                    dBgS_LinChk lc;
                    const f32* o = kOffsets[(cnt + i) & 3];
                    v.y = o[1];
                    v.z = o[2];
                    v.x = mirror ? -o[0] : o[0];
                    cameraEngineGlobeFromVector(&g, &v);
                    cameraEngineAngleAddShort(&g.mU, &t,
                                              (s16)tipView->angle.y); /* U + tip angle y */
                    cSAngle_l* a = cameraEngineAngleShortCtor(&tmp, (s16)t);
                    g.mU = (s16)*a;
                    cameraEngineAngleAddShort(&g.mV, &t,
                                              (s16)tipView->angle.x); /* V + tip angle x */
                    a = cameraEngineAngleShortCtor(&tmp, (s16)t);
                    g.mV = (s16)*a;
                    positionOf(&tp, tip);
                    cameraEngineGlobeXyz(&g, &x);
                    cXyz_pl(&tp, &e, &x);
                    work->m38C.copy(e);
                    dBgS_LinChk* lo = &lc;
                    camLinChk_ct_engine(lo);
                    if (!lineBGCheck(&pp, &work->m38C, (u8*)&lc, 0xF)) {
                        work->m398 = 1;
                        camLinChk_dt_engine(lo);
                        break;
                    }
                    if (i == 3)
                        mirror ^= 1;
                    camLinChk_dt_engine(lo);
                    if (i < 7)
                        cnt = (u32)m07C;
                }
            }
        }
        m100 = 1;
        m101 = 1;
        m102 = 1;
        return true;
    }
    f32 k;
    if (work->m398 != 0 && (u32)m11C > 8) {
        eye.copy(work->m38C);
        k = 1.0f;
    } else {
        eye.copy(mEye);
        k = speed;
    }
    mul.y = v4;
    mul.x = v3;
    mul.z = v3;
    relationalPos(&c, mpPlayerActor.get(), &ofs);
    if (m31D != 0) {
        cameraEngineMoveBgPosition(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mBG.m5C.m04), 1,
                                   &work->m380, 0, 0); /* MoveBgMatrixCrrPos */
        cameraEngineMoveBgPosition(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mBG.m5C.m04), 1,
                                   &work->m38C, 0, 0);
    }
    cXyz_mi(&c, &d, &mViewCache.mCenter);
    cameraEngineVectorMultiply(&d, &s, &mul);
    PSVECAdd(&mViewCache.mCenter, &s, &mViewCache.mCenter);
    cXyz_mi(&eye, &d, &mViewCache.mCenter);
    cSGlobe_l g;
    cameraEngineGlobeFromVector(&g, &d);
    fixedPosApproach(this, &g, k, fovy, minR, maxR);
    return true;
}

/* WWHD glue (tools/verify/autoglue.py)
ext 02008FEC void cameraEngineLineCtor(dBgS_LinChk*)
ext 02008B4C void cameraEngineLineDtor(dBgS_LinChk*, s32)
ext 02006FE4 void cameraEngineGlobeValShort(cSGlobe_l*, f32, s16, s16)
ext 020073AC void cameraEngineGlobeXyz(cSGlobe_l*, cXyz*)
ext 02007324 void cameraEngineGlobeFromVector(cSGlobe_l*, cXyz*)
ext 02006908 void cameraEngineAngleSubtractShort(cSAngle_l*, cSAngle_l*, s16)
ext 0200658C cSAngle_l* cameraEngineAngleShortCtor(cSAngle_l*, s32)
ext 020068F4 void cameraEngineAngleAddShort(cSAngle_l*, cSAngle_l*, s16)
ext 020066C0 cSAngle_l* cameraEngineAngleFloatCtor(cSAngle_l*, f32)
ext 0200693C void cameraEngineAngleScale(cSAngle_l*, cSAngle_l*, f32)
ext 02006894 void cameraEngineAngleAdd(cSAngle_l*, cSAngle_l*, cSAngle_l*)
ext 020068B0 void cameraEngineAngleSubtract(cSAngle_l*, cSAngle_l*, cSAngle_l*)
ext 02007068 void cameraEngineGlobeCtor(cSGlobe_l*, f32, s16, s16)
ext 02018780 void cameraEngineLineSet(LineFrame*, cXyz*, cXyz*)
ext 02010AE4 bool cameraEnginePointAndLine(LineFrame*, cXyz*, cXyz*, GuestFloat*)
ext 024FB608 bool cameraEngineLineCross(dCamera_c*, cXyz*, cXyz*, cXyz*, u32)
ext 0201AFD8 bool cameraEngineVectorNotEqual(cXyz*, cXyz*)
ext 0201AF1C void cameraEngineVectorMultiply(cXyz*, cXyz*, cXyz*)
ext 020065EC void cameraEngineShortSubtractAngle(cSAngle_l*, s16, cSAngle_l*)
ext 02006638 void cameraEngineAngleVal(cSAngle_l*, cSAngle_l*)
ext 020071E0 void cameraEngineGlobeValAngles(cSGlobe_l*, f32, cSAngle_l*, cSAngle_l*)
ext 024FCBE8 bool cameraEngineLineBlocked(dCamera_c*, cXyz*, cXyz*, u32)
ext 024EFB08 void cameraEngineMoveBgPosition(dBgS*, cBgS_PolyInfo*, s32, cXyz*, u32, u32)
data canonPointer 101D5F20
*/
