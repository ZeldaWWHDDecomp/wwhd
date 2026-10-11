/* d_ev_camera, part 8: useItem0EvCamera. See
 * d_ev_camera.cpp for the unit's layout notes; written from the WWHD code (the GameCube
 * d_ev_camera.cpp is all "Nonmatching" stubs). The per-item offset tables live in the stack frame
 * as in the original (built field by field from .rodata); the frame is reproduced at the same
 * offsets (F). */
#include "d/d_ev_camera_8.h"

using namespace gabi;
namespace ev8Detail {
enum ItemCameraState { ITEM_INIT, ITEM_BLEND, ITEM_HOLD, ITEM_WAIT = 0xA, ITEM_DONE = 0x63 };

static ItemCameraRodataHD itemCameraData;
static const f32 itemValue15 = 15.0f;
static const f32 itemValue0 = 0.0f;
static const f32 itemValue70 = 70.0f;
static const f32 itemValue100 = 100.0f;
static const f32 itemValueMinus70 = -70.0f;
static const f32 itemValueMinus90 = -90.0f;
static const f32 itemValue55 = 55.0f;
static const f32 itemValueMinus50 = -50.0f;
static const f32 itemValue115 = 115.0f;
static const f32 itemValue60 = 60.0f;
static const f32 itemValue5 = 5.0f;
static const f32 itemValue110 = 110.0f;
static const f32 itemValueMinus40 = -40.0f;
static const f32 itemValueMinus25 = -25.0f;
static const f32 itemValueMinus10 = -10.0f;
static const f32 itemValueMinus5 = -5.0f;
static const f32 itemValueMinus30 = -30.0f;
static const f32 itemValue10 = 10.0f;
static const f32 itemValue85 = 85.0f;
static const f32 itemValue45 = 45.0f;

// HD: all indexed table arithmetic stays in 32-bit guest addresses. An unchecked Type can
// reach saved GPR/FPR or caller-frame words; do not replace it with bounded native indexing.
/* inlined into useItem0EvCamera/useItem1EvCamera */
static inline void copy3(u32 d, u32 s) {
    *at<be<u32>>(d + 0) = at<be<u32>>(s + 0)->get();
    *at<be<u32>>(d + 4) = at<be<u32>>(s + 4)->get();
    *at<be<u32>>(d + 8) = at<be<u32>>(s + 8)->get();
}

// HD: these copies retain float access, while copy3 retains word access/NaN payloads.
/* inlined into useItem0EvCamera/useItem1EvCamera */
static inline void copy3f(u32 destination, u32 source) {
    auto& dst = *at<cXyz>(destination);
    auto& src = *at<cXyz>(source);
    dst.x = src.x.get();
    dst.y = src.y.get();
    dst.z = src.z.get();
}
/* inlined into useItem0EvCamera/useItem1EvCamera */
static inline void setDone(u32 c) {
    at<dCamera_c>(c)->m102 = 1;
    at<dCamera_c>(c)->m101 = 1;
    at<dCamera_c>(c)->m100 = 1;
}
/* centre from the item's centre offset, eye = centre + direction */
/* inlined into useItem0EvCamera/useItem1EvCamera */
static inline void holdView(u32 c, u32 w, u32 F) {
    ev8_relationalPos(c, gabi::ea(&at<ItemCameraFrame0HD>(F)->mPoint),
                      at<dCamera_c>(c)->mpPlayerActor.v.get(),
                      at<be<u32>>(gabi::ea(&at<ItemCameraFrame0HD>(F)->mCenterOffsets[0]) +
                                  (at<ItemCameraWork>(w)->mType << 2))
                          ->get());
    copy3(gabi::ea(&at<dCamera_c>(c)->mViewCache.mCenter),
          gabi::ea(&at<ItemCameraFrame0HD>(F)->mPoint));
    ev8_cSGlobe_Xyz(gabi::ea(&at<dCamera_c>(c)->mViewCache.mDirection),
                    gabi::ea(&at<ItemCameraFrame0HD>(F)->mDelta));
    ev8_cXyz_pl(gabi::ea(&at<dCamera_c>(c)->mViewCache.mCenter),
                gabi::ea(&at<ItemCameraFrame0HD>(F)->mPoint),
                gabi::ea(&at<ItemCameraFrame0HD>(F)->mDelta));
    copy3(gabi::ea(&at<dCamera_c>(c)->mViewCache.mEye),
          gabi::ea(&at<ItemCameraFrame0HD>(F)->mPoint));
}

/* useItem0EvCamera (showing an item in use; "Type" selects one of seven offset sets).
 * work: +0 state (0xA: wait, then pick the first clear eye of the type's list; 1: blend over
 * work+4 frames; 2: hold (type 0 once more as type 4); 0x63: done), +8 Type, +0xC centre,
 * +0x18 target fovy, +0x1C/+0x20 counters, +0x24 target direction */
} // namespace ev8Detail
using namespace ev8Detail;
/* 02538DB4 */
u32 ItemEventCamera_c::useItem0EvCamera() {
    const u32 i_this = ea();
    u32 entry = gabi::cpu->r[1];
    gabi::Local<ItemCameraFrame0HD> frame; /* reserves the original frame [entry - 0x288, entry) */
    u32 F = entry - 0x288;
    (void)frame;
    *at<be<u32>>(F) =
        entry; // HD: original stwu back chain.                  /* back chain (stwu) */
    *at<be<u32>>(entry + 4) = gabi::cpu->lr; /* LR save word in the caller's frame */
    /* the register save area (stmw r20 at +0x238, f30/f31 at +0x268/+0x278): an out-of-range
     * "Type" indexes the tables into it as in the original */
    for (u32 i = 20; i < 32; i++)
        *at<be<u32>>(gabi::ea(&at<ItemCameraFrame0HD>(F)->mSavedGpr[0]) + (i - 20) * 4) =
            gabi::cpu->r[i];
    {
        u64 b;
        f64 d = gabi::cpu->f[30].ps0;
        memcpy(&b, &d, 8);
        at<ItemCameraFrame0HD>(F)->mSavedFpr[0].mDouble = b;
        at<ItemCameraFrame0HD>(F)->mSavedFpr[0].mPairedSingle = (f32)gabi::cpu->f[30].ps1;
        d = gabi::cpu->f[31].ps0;
        memcpy(&b, &d, 8);
        at<ItemCameraFrame0HD>(F)->mSavedFpr[1].mDouble = b;
        at<ItemCameraFrame0HD>(F)->mSavedFpr[1].mPairedSingle = (f32)gabi::cpu->f[31].ps1;
    }
    u32 w = gabi::ea(mWork);
    auto& work = *at<ItemCameraWork>(w);
    at<ItemCameraFrame0HD>(F)->mOffsetValues[10] = itemValue15;
    at<ItemCameraFrame0HD>(F)->mOffsetValues[8] = itemValue0;
    at<ItemCameraFrame0HD>(F)->mOffsetValues[9] = itemCameraData.mItem0Values[9].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[33] = itemCameraData.mItem0Values[7].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[30] = itemCameraData.mItem0Values[4].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[7] = itemCameraData.mItem0Values[2].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[31] = itemCameraData.mItem0Values[5].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[6] = itemCameraData.mItem0Values[1].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[29] = itemCameraData.mItem0Values[3].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[32] = itemCameraData.mItem0Values[6].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[5] = itemCameraData.mItem0Values[0].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[34] = itemCameraData.mItem0Values[8].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[56] = itemCameraData.mItem0Values[12].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[60] = itemCameraData.mItem0Values[14].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[53] = itemCameraData.mItem0Values[10].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[58] = itemCameraData.mItem0Values[10].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[64] = itemValue70;
    at<ItemCameraFrame0HD>(F)->mOffsetValues[62] = itemCameraData.mItem0Values[15].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[61] = itemValue100;
    at<ItemCameraFrame0HD>(F)->mOffsetValues[66] = itemValueMinus70;
    at<ItemCameraFrame0HD>(F)->mOffsetValues[57] = itemCameraData.mItem0Values[13].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[63] = itemCameraData.mItem0Values[16].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[65] = itemValueMinus90;
    at<ItemCameraFrame0HD>(F)->mOffsetValues[20] = itemCameraData.mItem0Values[18].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[59] = itemValue55;
    at<ItemCameraFrame0HD>(F)->mOffsetValues[54] = itemValueMinus50;
    at<ItemCameraFrame0HD>(F)->mOffsetValues[25] = itemValue115;
    at<ItemCameraFrame0HD>(F)->mOffsetValues[21] = itemValue60;
    at<ItemCameraFrame0HD>(F)->mOffsetValues[55] = itemCameraData.mItem0Values[11].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[26] = itemValue5;
    at<ItemCameraFrame0HD>(F)->mOffsetValues[73] = itemValue110;
    at<ItemCameraFrame0HD>(F)->mOffsetValues[0] = itemValue0;
    at<ItemCameraFrame0HD>(F)->mOffsetValues[71] = itemValue5;
    at<ItemCameraFrame0HD>(F)->mOffsetValues[22] = itemCameraData.mItem0Values[19].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[67] = itemCameraData.mItem0Values[22].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[72] = itemCameraData.mItem0Values[20].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[74] = itemCameraData.mItem0Values[25].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[76] = itemCameraData.mItem0Values[26].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[1] = itemValueMinus40;
    at<ItemCameraFrame0HD>(F)->mOffsetValues[24] = itemValueMinus25;
    at<ItemCameraFrame0HD>(F)->mOffsetValues[68] = itemCameraData.mItem0Values[20].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[23] = itemCameraData.mItem0Values[20].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[70] = itemCameraData.mItem0Values[24].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[75] = itemValueMinus50;
    at<ItemCameraFrame0HD>(F)->mOffsetValues[69] = itemCameraData.mItem0Values[23].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[11] = itemValue0;
    at<ItemCameraFrame0HD>(F)->mOffsetValues[12] = itemCameraData.mItem0Values[21].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[13] = itemCameraData.mItem0Values[17].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[2] = itemCameraData.mItem0Values[17].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[14] = itemCameraData.mItem0Values[27].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[28] = itemValue110;
    at<ItemCameraFrame0HD>(F)->mOffsetValues[15] = itemCameraData.mItem0Values[28].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[38] = itemCameraData.mItem0Values[33].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[27] = itemCameraData.mItem0Values[20].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[39] = itemCameraData.mItem0Values[34].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[37] = itemCameraData.mItem0Values[32].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[35] = itemCameraData.mItem0Values[30].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[16] = itemCameraData.mItem0Values[29].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[41] = itemCameraData.mItem0Values[30].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[44] = itemCameraData.mItem0Values[38].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[47] = itemCameraData.mItem0Values[41].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[48] = itemCameraData.mItem0Values[42].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[49] = itemCameraData.mItem0Values[43].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[36] = itemCameraData.mItem0Values[31].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[42] = itemCameraData.mItem0Values[31].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[40] = itemCameraData.mItem0Values[35].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[45] = itemCameraData.mItem0Values[39].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[46] = itemCameraData.mItem0Values[40].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[50] = itemCameraData.mItem0Values[44].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[51] = itemCameraData.mItem0Values[45].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[52] = itemCameraData.mItem0Values[46].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[17] = itemCameraData.mItem0Values[36].get();
    at<ItemCameraFrame0HD>(F)->mOffsetValues[43] = itemCameraData.mItem0Values[32].get();
    at<ItemCameraFrame0HD>(F)->mCenterOffsets[5] =
        gabi::ea(&at<ItemCameraFrame0HD>(F)->mOffsetValues[14]);
    at<ItemCameraFrame0HD>(F)->mCenterOffsets[0] =
        gabi::ea(&at<ItemCameraFrame0HD>(F)->mOffsetValues[0]);
    at<ItemCameraFrame0HD>(F)->mCenterOffsets[2] =
        gabi::ea(&at<ItemCameraFrame0HD>(F)->mOffsetValues[0]);
    at<ItemCameraFrame0HD>(F)->mEyeOffsets[0] =
        gabi::ea(&at<ItemCameraFrame0HD>(F)->mOffsetValues[20]);
    at<ItemCameraFrame0HD>(F)->mCenterOffsets[3] =
        gabi::ea(&at<ItemCameraFrame0HD>(F)->mOffsetValues[11]);
    at<ItemCameraFrame0HD>(F)->mCenterOffsets[1] =
        gabi::ea(&at<ItemCameraFrame0HD>(F)->mOffsetValues[8]);
    at<ItemCameraFrame0HD>(F)->mOffsetValues[19] = itemCameraData.mItem0Values[11].get();
    at<ItemCameraFrame0HD>(F)->mEyeOffsets[1] =
        gabi::ea(&at<ItemCameraFrame0HD>(F)->mOffsetValues[53]);
    at<ItemCameraFrame0HD>(F)->mEyeOffsets[3] =
        gabi::ea(&at<ItemCameraFrame0HD>(F)->mOffsetValues[65]);
    at<ItemCameraFrame0HD>(F)->mCenterOffsets[4] =
        gabi::ea(&at<ItemCameraFrame0HD>(F)->mOffsetValues[5]);
    at<ItemCameraFrame0HD>(F)->mEyeOffsets[4] =
        gabi::ea(&at<ItemCameraFrame0HD>(F)->mOffsetValues[29]);
    at<ItemCameraFrame0HD>(F)->mCenterOffsets[6] =
        gabi::ea(&at<ItemCameraFrame0HD>(F)->mOffsetValues[17]);
    at<ItemCameraFrame0HD>(F)->mEyeOffsets[2] =
        gabi::ea(&at<ItemCameraFrame0HD>(F)->mOffsetValues[20]);
    at<ItemCameraFrame0HD>(F)->mOffsetValues[18] = itemCameraData.mItem0Values[37].get();
    at<ItemCameraFrame0HD>(F)->mEyeCounts[0] = 0x2;
    at<ItemCameraFrame0HD>(F)->mEyeCounts[1] = 0x4;
    at<ItemCameraFrame0HD>(F)->mEyeCounts[2] = 0x3;
    at<ItemCameraFrame0HD>(F)->mEyeCounts[3] = 0x4;
    at<ItemCameraFrame0HD>(F)->mEyeCounts[4] = 0x3;
    at<ItemCameraFrame0HD>(F)->mFovyBits[0] = itemCameraData.mFovy0[0].get();
    at<ItemCameraFrame0HD>(F)->mFovyBits[1] = itemCameraData.mFovy0[1].get();
    at<ItemCameraFrame0HD>(F)->mFovyBits[2] = itemCameraData.mFovy0[2].get();
    at<ItemCameraFrame0HD>(F)->mFovyBits[3] = itemCameraData.mFovy0[3].get();
    at<ItemCameraFrame0HD>(F)->mFovyBits[4] = itemCameraData.mFovy0[4].get();
    at<ItemCameraFrame0HD>(F)->mEyeCounts[5] = 0x3;
    at<ItemCameraFrame0HD>(F)->mBlendFrames[0] = 0x2D;
    at<ItemCameraFrame0HD>(F)->mFovyBits[5] = itemCameraData.mFovy0[5].get();
    at<ItemCameraFrame0HD>(F)->mFovyBits[6] = itemCameraData.mFovy0[6].get();
    at<ItemCameraFrame0HD>(F)->mEyeCounts[6] = 0x3;
    at<ItemCameraFrame0HD>(F)->mBlendFrames[1] = 0x28;
    at<ItemCameraFrame0HD>(F)->mBlendFrames[2] = 0x28;
    at<ItemCameraFrame0HD>(F)->mBlendFrames[3] = 0x28;
    at<ItemCameraFrame0HD>(F)->mEyeOffsets[5] =
        gabi::ea(&at<ItemCameraFrame0HD>(F)->mOffsetValues[35]);
    at<ItemCameraFrame0HD>(F)->mBlendFrames[4] = 0xA;
    at<ItemCameraFrame0HD>(F)->mBlendFrames[5] = 0x2D;
    at<ItemCameraFrame0HD>(F)->mBlendFrames[6] = 0x37;
    at<ItemCameraFrame0HD>(F)->mWaitFrames[0] = 0x0;
    at<ItemCameraFrame0HD>(F)->mWaitFrames[1] = 0x0;
    at<ItemCameraFrame0HD>(F)->mWaitFrames[2] = 0x0;
    at<ItemCameraFrame0HD>(F)->mWaitFrames[3] = 0x0;
    at<ItemCameraFrame0HD>(F)->mWaitFrames[4] = 0x0;
    at<ItemCameraFrame0HD>(F)->mWaitFrames[5] = 0x49;
    at<ItemCameraFrame0HD>(F)->mEyeOffsets[6] =
        gabi::ea(&at<ItemCameraFrame0HD>(F)->mOffsetValues[44]);
    at<ItemCameraFrame0HD>(F)->mWaitFrames[6] = 0x0;
    ev8_cSAngle_default(gabi::ea(&at<ItemCameraFrame0HD>(F)->mYaw)); /* cSAngle() */
    ev8_cSAngle_default(gabi::ea(&at<ItemCameraFrame0HD>(F)->mPitch));
    u32 s;
    if (this->m11C == 0) {
        s = ITEM_INIT;
        work.mState = ITEM_INIT;
    } else {
        s = work.mState;
    }
    if (s == ITEM_BLEND) {
        /* blend towards the chosen view */
        f32 t = (f32)(f64)(s32)work.mFrame / (f32)(f64)(s32)work.mBlendFrames;
        f32 fv = this->mViewCache.mFovy;
        this->mViewCache.mFovy = gabi::fmadds(work.mFovy - fv, t, fv);
        ev8_relationalPos(
            i_this, gabi::ea(&at<ItemCameraFrame0HD>(F)->mDelta), this->mpPlayerActor.v.get(),
            at<be<u32>>(gabi::ea(&at<ItemCameraFrame0HD>(F)->mCenterOffsets[0]) + (work.mType << 2))
                ->get());
        copy3(gabi::ea(&work.mCenter), gabi::ea(&at<ItemCameraFrame0HD>(F)->mDelta));
        ev8_cXyz_mi(gabi::ea(&work.mCenter), gabi::ea(&at<ItemCameraFrame0HD>(F)->mPoint),
                    gabi::ea(&this->mViewCache.mCenter));
        ev8_cXyz_ml(gabi::ea(&at<ItemCameraFrame0HD>(F)->mPoint),
                    gabi::ea(&at<ItemCameraFrame0HD>(F)->mDelta), t);
        ev8_PSVECAdd(gabi::ea(&this->mViewCache.mCenter),
                     gabi::ea(&at<ItemCameraFrame0HD>(F)->mDelta),
                     gabi::ea(&this->mViewCache.mCenter)); /* PSVECAdd */
        at<ItemCameraFrame0HD>(F)->mPitch = (u16)this->mViewCache.mDirection.mV;
        at<ItemCameraFrame0HD>(F)->mYaw = (u16)this->mViewCache.mDirection.mU;
        f32 r0 = this->mViewCache.mDirection.mRadius;
        f32 rad = gabi::fmadds(work.mTargetDirection.mRadius - r0, t, r0);
        ev8_cSAngle_mi(gabi::ea(&work.mTargetDirection.mV),
                       gabi::ea(&at<ItemCameraFrame0HD>(F)->mAngleB),
                       gabi::ea(&at<ItemCameraFrame0HD>(F)->mPitch));
        ev8_cSAngle_ml(gabi::ea(&at<ItemCameraFrame0HD>(F)->mAngleB),
                       gabi::ea(&at<ItemCameraFrame0HD>(F)->mAngleA), t);
        ev8_cSAngle_addeq(gabi::ea(&at<ItemCameraFrame0HD>(F)->mPitch),
                          gabi::ea(&at<ItemCameraFrame0HD>(F)->mAngleA));
        ev8_cSAngle_mi(gabi::ea(&work.mTargetDirection.mU),
                       gabi::ea(&at<ItemCameraFrame0HD>(F)->mAngleA),
                       gabi::ea(&at<ItemCameraFrame0HD>(F)->mYaw));
        ev8_cSAngle_ml(gabi::ea(&at<ItemCameraFrame0HD>(F)->mAngleA),
                       gabi::ea(&at<ItemCameraFrame0HD>(F)->mAngleB), t);
        ev8_cSAngle_addeq(gabi::ea(&at<ItemCameraFrame0HD>(F)->mYaw),
                          gabi::ea(&at<ItemCameraFrame0HD>(F)->mAngleB));
        ev8_cSGlobe_ValRVU(gabi::ea(&this->mViewCache.mDirection), rad,
                           gabi::ea(&at<ItemCameraFrame0HD>(F)->mPitch),
                           gabi::ea(&at<ItemCameraFrame0HD>(F)->mYaw)); /* cSGlobe::Val(r, V, U) */
        ev8_cSGlobe_Xyz(gabi::ea(&this->mViewCache.mDirection),
                        gabi::ea(&at<ItemCameraFrame0HD>(F)->mDelta));
        ev8_cXyz_pl(gabi::ea(&this->mViewCache.mCenter),
                    gabi::ea(&at<ItemCameraFrame0HD>(F)->mPoint),
                    gabi::ea(&at<ItemCameraFrame0HD>(F)->mDelta));
        copy3(gabi::ea(&this->mViewCache.mEye), gabi::ea(&at<ItemCameraFrame0HD>(F)->mPoint));
        u32 c = work.mFrame;
        if ((s32)c < (s32)work.mBlendFrames) {
            work.mFrame = c + 1;
            return 0;
        }
        work.mState = ITEM_HOLD;
        s = ITEM_HOLD;
    }
    if (s == ITEM_HOLD) {
        holdView(i_this, w, F);
        u32 n = work.mHoldFrames + 1;
        u32 type = work.mType;
        work.mHoldFrames = n;
        if (type == 0 && n == 1) {
            u32 c = work.mFrame;
            work.mState = ITEM_WAIT;
            work.mType = 4;
            work.mFrame = c + 1;
            return 0;
        }
        work.mState = ITEM_DONE;
        s = ITEM_DONE;
    }
    if (s == ITEM_DONE) {
        setDone(i_this);
        holdView(i_this, w, F);
        work.mFrame = work.mFrame + 1;
        return 1;
    }
    if (s != ITEM_WAIT) {
        ev8_getEvIntData(i_this, gabi::ea(&work.mType),
                         gabi::ea(itemCameraData.mType0) /* "Type" */, 0);
        work.mHoldFrames = ITEM_INIT;
        work.mFrame = 0;
    }
    /* state 0xA: wait the type's frames, then choose the eye */
    u32 c = work.mFrame + 1;
    work.mState = ITEM_WAIT;
    work.mFrame = c;
    if ((s32)c <
        (s32)at<be<u32>>(gabi::ea(&at<ItemCameraFrame0HD>(F)->mWaitFrames[0]) + (work.mType << 2))
            ->get()) {
        work.mFrame = c + 1;
        return 0;
    }
    work.mFrame = 0;
    if ((this->m07C & 7) == 0) {
        u32 type = work.mType;
        if (type >= 2 && type <= 3) { /* swap the first two eye offsets */
            u32 p = at<be<u32>>(gabi::ea(&at<ItemCameraFrame0HD>(F)->mEyeOffsets[0]) + (type << 2))
                        ->get();
            u32 x1 = at<be<u32>>(p + 0x10)->get(), x0 = at<be<u32>>(p + 0xC)->get();
            f32 a0 = at<cXyz>(p)[0].x.get();
            *at<be<u32>>(p + 0) = x0;
            f32 a1 = at<cXyz>(p)[0].y.get();
            *at<be<u32>>(p + 4) = x1;
            u32 x2 = at<be<u32>>(p + 0x14)->get();
            f32 a2 = at<cXyz>(p)[0].z.get();
            *at<be<u32>>(p + 8) = x2;
            u32 q = at<be<u32>>(gabi::ea(&at<ItemCameraFrame0HD>(F)->mEyeOffsets[0]) +
                                (work.mType << 2))
                        ->get();
            at<cXyz>(q)[1].y = a1;
            at<cXyz>(q)[1].z = a2;
            at<cXyz>(q)[1].x = a0;
        }
    }
    ev8_relationalPos(
        i_this, gabi::ea(&at<ItemCameraFrame0HD>(F)->mDelta), this->mpPlayerActor.v.get(),
        at<be<u32>>(gabi::ea(&at<ItemCameraFrame0HD>(F)->mCenterOffsets[0]) + (work.mType << 2))
            ->get());
    copy3(gabi::ea(&work.mCenter), gabi::ea(&at<ItemCameraFrame0HD>(F)->mDelta));
    u32 E = gabi::ea(&at<ItemCameraFrame0HD>(F)->mEye);
    for (s32 i = 0; i < (s32)at<be<u32>>(gabi::ea(&at<ItemCameraFrame0HD>(F)->mEyeCounts[0]) +
                                         (work.mType << 2))
                            ->get();
         i++) {
        ev8_relationalPos(
            i_this, gabi::ea(&at<ItemCameraFrame0HD>(F)->mPosition), this->mpPlayerActor.v.get(),
            at<be<u32>>(gabi::ea(&at<ItemCameraFrame0HD>(F)->mEyeOffsets[0]) + (work.mType << 2))
                    ->get() +
                (u32)i * 12);
        copy3(E, gabi::ea(&at<ItemCameraFrame0HD>(F)->mPosition));
        ev8_positionOf(i_this, gabi::ea(&at<ItemCameraFrame0HD>(F)->mPosition),
                       this->mpPlayerActor.v.get());
        if (at<cXyz>(E)[0].y.get() <
            at<ItemCameraFrame0HD>(F)->mPosition.y + this->m368) { /* bge: taken on NaN */
            ev8_positionOf(i_this, gabi::ea(&at<ItemCameraFrame0HD>(F)->mPlayerPosition),
                           this->mpPlayerActor.v.get());
            at<cXyz>(E)[0].y = at<ItemCameraFrame0HD>(F)->mPlayerPosition.y + this->m368;
        }
        u32 item = 0;
        s32 pad = (s32)this->mPadId;
        u32 g = dComIfGp_ea();
        if (at<dComIfG_inf_c>(g)->play.mPlayerStatus[pad][0].get() & 0x10000) {
            at<ItemCameraFrame0HD>(F)->mItemProfile = 0xA5;
            item = ev8_JudgeActor(
                0x025E121C,
                gabi::ea(&at<ItemCameraFrame0HD>(F)
                              ->mItemProfile)); /* fopAcIt_Judge(fpcSch_JudgeForPName) */
        }
        if (ev8_lineBGCheck(i_this, gabi::ea(&work.mCenter), E, 0x8F))
            continue;
        copy3f(gabi::ea(&at<ItemCameraFrame0HD>(F)->mPosition), gabi::ea(&work.mCenter));
        copy3f(gabi::ea(&at<ItemCameraFrame0HD>(F)->mCollisionEye), E);
        u32 pl = this->mpPlayerActor.v.get();
        g = dComIfGp_ea();
        if (!ev8_ChkCamera(gabi::ea(&at<dComIfG_inf_c>(g)->play.mCcS),
                           gabi::ea(&at<ItemCameraFrame0HD>(F)->mPosition),
                           gabi::ea(&at<ItemCameraFrame0HD>(F)->mCollisionEye), itemValue15, pl,
                           item))
            break; /* dCcS::ChkCamera */
    }
    ev8_cXyz_mi(E, gabi::ea(&at<ItemCameraFrame0HD>(F)->mDelta), gabi::ea(&work.mCenter));
    ev8_cSGlobe_Val(gabi::ea(&work.mTargetDirection), gabi::ea(&at<ItemCameraFrame0HD>(F)->mDelta));
    u32 type = work.mType;
    u32 cnt = work.mFrame;
    work.mFovy =
        at<be<f32>>(gabi::ea(&at<ItemCameraFrame0HD>(F)->mFovyBits[0]) + (type << 2))->get();
    u32 fr =
        at<be<u32>>(gabi::ea(&at<ItemCameraFrame0HD>(F)->mBlendFrames[0]) + (type << 2))->get();
    work.mState = ITEM_BLEND;
    work.mBlendFrames = fr;
    work.mFrame = cnt + 1;
    return 0;
}
namespace ev8Detail {

/* the original's register save area at the top of a frame (stmw rFirst at saveOff; f30/f31 as
 * stfd + the paired-single slot) */
/* inlined into useItem0EvCamera/useItem1EvCamera */
static inline void frameSaves(u32 F, u32 first, u32 saveOff, u32 f30Off) {
    for (u32 i = first; i < 32; i++)
        *at<be<u32>>(F + saveOff + (i - first) * 4) = gabi::cpu->r[i];
    u64 b;
    f64 d = gabi::cpu->f[30].ps0;
    memcpy(&b, &d, 8);
    at<ItemCameraFprSaveHD>(F + f30Off)[0].mDouble = b;
    at<ItemCameraFprSaveHD>(F + f30Off)[0].mPairedSingle = (f32)gabi::cpu->f[30].ps1;
    d = gabi::cpu->f[31].ps0;
    memcpy(&b, &d, 8);
    at<ItemCameraFprSaveHD>(F + f30Off)[1].mDouble = b;
    at<ItemCameraFprSaveHD>(F + f30Off)[1].mPairedSingle = (f32)gabi::cpu->f[31].ps1;
}

/* useItem1EvCamera: as useItem0EvCamera without the initial wait and without the item
 * actor in the view check; tables: centre offsets +0x1E4, eye offsets +0x200, counts +0x190,
 * fovy +0x1AC, frames +0x1C8 */
} // namespace ev8Detail
using namespace ev8Detail;
/* 0253986C */
u32 ItemEventCamera_c::useItem1EvCamera() {
    const u32 i_this = ea();
    u32 entry = gabi::cpu->r[1];
    gabi::Local<ItemCameraFrame1HD> frame;
    u32 F = entry - 0x260;
    (void)frame;
    *at<be<u32>>(F) =
        entry; // HD: original stwu back chain.                  /* back chain (stwu) */
    *at<be<u32>>(entry + 4) = gabi::cpu->lr; /* LR save word in the caller's frame */
    frameSaves(F, 24, 0x220, 0x240);
    u32 w = gabi::ea(mWork);
    auto& work = *at<ItemCameraWork>(w);
    at<ItemCameraFrame1HD>(F)->mOffsetValues[31] = itemValue55;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[6] = itemValue0;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[30] = itemCameraData.mItem1Values[2].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[36] = itemValueMinus10;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[38] = itemValueMinus5;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[33] = itemCameraData.mItem1Values[2].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[32] = itemCameraData.mItem1Values[3].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[34] = itemCameraData.mItem1Values[4].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[9] = itemValue0;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[10] = itemCameraData.mItem0Values[9].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[37] = itemCameraData.mItem1Values[5].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[35] = itemValueMinus30;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[11] = itemValue15;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[8] = itemCameraData.mItem1Values[1].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[7] = itemCameraData.mItem1Values[0].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[70] = itemCameraData.mItem0Values[13].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[68] = itemCameraData.mItem0Values[11].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[67] = itemValueMinus50;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[12] = itemValue0;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[71] = itemCameraData.mItem0Values[10].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[73] = itemCameraData.mItem0Values[14].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[66] = itemCameraData.mItem0Values[10].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[74] = itemValue100;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[69] = itemCameraData.mItem0Values[12].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[1] = itemValueMinus40;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[72] = itemValue55;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[13] = itemCameraData.mItem0Values[21].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[2] = itemCameraData.mItem0Values[17].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[21] = itemCameraData.mItem0Values[18].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[0] = itemValue0;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[22] = itemValue60;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[75] = itemCameraData.mItem0Values[15].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[23] = itemCameraData.mItem0Values[19].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[26] = itemValue115;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[25] = itemValueMinus25;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[77] = itemValue70;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[14] = itemCameraData.mItem0Values[17].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[39] = itemValueMinus90;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[40] = itemValueMinus70;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[24] = itemCameraData.mItem0Values[20].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[41] = itemCameraData.mItem0Values[22].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[48] = itemValue15;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[76] = itemCameraData.mItem0Values[16].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[49] = itemCameraData.mItem0Values[10].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[15] = itemValue10;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[44] = itemCameraData.mItem0Values[24].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[42] = itemCameraData.mItem0Values[20].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[16] = itemCameraData.mItem1Values[6].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[19] = itemCameraData.mItem0Values[37].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[55] = itemValueMinus50;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[27] = itemValue5;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[45] = itemValue5;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[51] = itemCameraData.mItem1Values[7].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[52] = itemValue85;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[54] = itemCameraData.mItem1Values[9].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[43] = itemCameraData.mItem0Values[23].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[57] = itemCameraData.mItem0Values[38].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[50] = itemValue45;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[17] = itemCameraData.mItem1Values[6].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[56] = itemCameraData.mItem0Values[17].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[46] = itemCameraData.mItem0Values[20].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[28] = itemCameraData.mItem0Values[20].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[29] = itemValue110;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[20] = itemCameraData.mItem0Values[11].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[47] = itemValue110;
    at<ItemCameraFrame1HD>(F)->mOffsetValues[53] = itemCameraData.mItem1Values[8].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[18] = itemCameraData.mItem0Values[36].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[58] = itemCameraData.mItem0Values[39].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[59] = itemCameraData.mItem0Values[40].get();
    at<ItemCameraFrame1HD>(F)->mCenterOffsets[3] =
        gabi::ea(&at<ItemCameraFrame1HD>(F)->mOffsetValues[12]);
    at<ItemCameraFrame1HD>(F)->mCenterOffsets[0] =
        gabi::ea(&at<ItemCameraFrame1HD>(F)->mOffsetValues[0]);
    at<ItemCameraFrame1HD>(F)->mCenterOffsets[5] =
        gabi::ea(&at<ItemCameraFrame1HD>(F)->mOffsetValues[15]);
    at<ItemCameraFrame1HD>(F)->mEyeOffsets[0] =
        gabi::ea(&at<ItemCameraFrame1HD>(F)->mOffsetValues[21]);
    at<ItemCameraFrame1HD>(F)->mOffsetValues[60] = itemCameraData.mItem0Values[41].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[63] = itemCameraData.mItem0Values[44].get();
    at<ItemCameraFrame1HD>(F)->mEyeOffsets[1] =
        gabi::ea(&at<ItemCameraFrame1HD>(F)->mOffsetValues[66]);
    at<ItemCameraFrame1HD>(F)->mEyeOffsets[2] =
        gabi::ea(&at<ItemCameraFrame1HD>(F)->mOffsetValues[21]);
    at<ItemCameraFrame1HD>(F)->mOffsetValues[61] = itemCameraData.mItem0Values[42].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[62] = itemCameraData.mItem0Values[43].get();
    at<ItemCameraFrame1HD>(F)->mCenterOffsets[2] =
        gabi::ea(&at<ItemCameraFrame1HD>(F)->mOffsetValues[0]);
    at<ItemCameraFrame1HD>(F)->mEyeCounts[0] = 0x3;
    at<ItemCameraFrame1HD>(F)->mEyeCounts[1] = 0x4;
    at<ItemCameraFrame1HD>(F)->mCenterOffsets[4] =
        gabi::ea(&at<ItemCameraFrame1HD>(F)->mOffsetValues[6]);
    at<ItemCameraFrame1HD>(F)->mCenterOffsets[1] =
        gabi::ea(&at<ItemCameraFrame1HD>(F)->mOffsetValues[9]);
    at<ItemCameraFrame1HD>(F)->mEyeCounts[2] = 0x3;
    at<ItemCameraFrame1HD>(F)->mEyeOffsets[4] =
        gabi::ea(&at<ItemCameraFrame1HD>(F)->mOffsetValues[30]);
    at<ItemCameraFrame1HD>(F)->mOffsetValues[65] = itemCameraData.mItem0Values[46].get();
    at<ItemCameraFrame1HD>(F)->mEyeOffsets[3] =
        gabi::ea(&at<ItemCameraFrame1HD>(F)->mOffsetValues[39]);
    at<ItemCameraFrame1HD>(F)->mFovyBits[0] = itemCameraData.mFovy1[0].get();
    at<ItemCameraFrame1HD>(F)->mFovyBits[1] = itemCameraData.mFovy1[1].get();
    at<ItemCameraFrame1HD>(F)->mFovyBits[2] = itemCameraData.mFovy1[2].get();
    at<ItemCameraFrame1HD>(F)->mFovyBits[3] = itemCameraData.mFovy1[3].get();
    at<ItemCameraFrame1HD>(F)->mFovyBits[4] = itemCameraData.mFovy1[4].get();
    at<ItemCameraFrame1HD>(F)->mEyeCounts[3] = 0x3;
    at<ItemCameraFrame1HD>(F)->mEyeCounts[4] = 0x3;
    at<ItemCameraFrame1HD>(F)->mEyeCounts[5] = 0x3;
    at<ItemCameraFrame1HD>(F)->mCenterOffsets[6] =
        gabi::ea(&at<ItemCameraFrame1HD>(F)->mOffsetValues[18]);
    at<ItemCameraFrame1HD>(F)->mEyeOffsets[5] =
        gabi::ea(&at<ItemCameraFrame1HD>(F)->mOffsetValues[48]);
    at<ItemCameraFrame1HD>(F)->mFovyBits[5] = itemCameraData.mFovy1[5].get();
    at<ItemCameraFrame1HD>(F)->mOffsetValues[64] = itemCameraData.mItem0Values[45].get();
    at<ItemCameraFrame1HD>(F)->mBlendFrames[0] = 0x2D;
    at<ItemCameraFrame1HD>(F)->mBlendFrames[1] = 0x28;
    at<ItemCameraFrame1HD>(F)->mBlendFrames[2] = 0x28;
    at<ItemCameraFrame1HD>(F)->mBlendFrames[3] = 0x28;
    at<ItemCameraFrame1HD>(F)->mBlendFrames[4] = 0xA;
    at<ItemCameraFrame1HD>(F)->mBlendFrames[5] = 0x2D;
    at<ItemCameraFrame1HD>(F)->mBlendFrames[6] = 0x28;
    at<ItemCameraFrame1HD>(F)->mEyeCounts[6] = 0x3;
    at<ItemCameraFrame1HD>(F)->mEyeOffsets[6] =
        gabi::ea(&at<ItemCameraFrame1HD>(F)->mOffsetValues[57]);
    at<ItemCameraFrame1HD>(F)->mFovyBits[6] = itemCameraData.mFovy1[6].get();
    ev8_cSAngle_default(gabi::ea(&at<ItemCameraFrame1HD>(F)->mYaw)); /* cSAngle() */
    ev8_cSAngle_default(gabi::ea(&at<ItemCameraFrame1HD>(F)->mPitch));
    u32 ctrTab = gabi::ea(&at<ItemCameraFrame1HD>(F)->mCenterOffsets[0]),
        eyeTab = gabi::ea(&at<ItemCameraFrame1HD>(F)->mEyeOffsets[0]);
    u32 s;
    if (this->m11C == 0) {
        s = ITEM_INIT;
        work.mState = ITEM_INIT;
    } else {
        s = work.mState;
    }
    if (s == ITEM_BLEND) {
        f32 t = (f32)(f64)(s32)work.mFrame / (f32)(f64)(s32)work.mBlendFrames;
        f32 fv = this->mViewCache.mFovy;
        this->mViewCache.mFovy = gabi::fmadds(work.mFovy - fv, t, fv);
        ev8_relationalPos(i_this, gabi::ea(&at<ItemCameraFrame1HD>(F)->mDelta),
                          this->mpPlayerActor.v.get(),
                          at<be<u32>>(ctrTab + (work.mType << 2))->get());
        copy3(gabi::ea(&work.mCenter), gabi::ea(&at<ItemCameraFrame1HD>(F)->mDelta));
        ev8_cXyz_mi(gabi::ea(&work.mCenter), gabi::ea(&at<ItemCameraFrame1HD>(F)->mPoint),
                    gabi::ea(&this->mViewCache.mCenter));
        ev8_cXyz_ml(gabi::ea(&at<ItemCameraFrame1HD>(F)->mPoint),
                    gabi::ea(&at<ItemCameraFrame1HD>(F)->mDelta), t);
        ev8_PSVECAdd(gabi::ea(&this->mViewCache.mCenter),
                     gabi::ea(&at<ItemCameraFrame1HD>(F)->mDelta),
                     gabi::ea(&this->mViewCache.mCenter)); /* PSVECAdd */
        s16 v = this->mViewCache.mDirection.mV, u = this->mViewCache.mDirection.mU;
        at<ItemCameraFrame1HD>(F)->mYaw = (u16)u;
        at<ItemCameraFrame1HD>(F)->mPitch = (u16)v;
        f32 r0 = this->mViewCache.mDirection.mRadius;
        f32 rad = gabi::fmadds(work.mTargetDirection.mRadius - r0, t, r0);
        ev8_cSAngle_mi(gabi::ea(&work.mTargetDirection.mV),
                       gabi::ea(&at<ItemCameraFrame1HD>(F)->mAngleB),
                       gabi::ea(&at<ItemCameraFrame1HD>(F)->mPitch));
        ev8_cSAngle_ml(gabi::ea(&at<ItemCameraFrame1HD>(F)->mAngleB),
                       gabi::ea(&at<ItemCameraFrame1HD>(F)->mAngleA), t);
        ev8_cSAngle_addeq(gabi::ea(&at<ItemCameraFrame1HD>(F)->mPitch),
                          gabi::ea(&at<ItemCameraFrame1HD>(F)->mAngleA));
        ev8_cSAngle_mi(gabi::ea(&work.mTargetDirection.mU),
                       gabi::ea(&at<ItemCameraFrame1HD>(F)->mAngleA),
                       gabi::ea(&at<ItemCameraFrame1HD>(F)->mYaw));
        ev8_cSAngle_ml(gabi::ea(&at<ItemCameraFrame1HD>(F)->mAngleA),
                       gabi::ea(&at<ItemCameraFrame1HD>(F)->mAngleB), t);
        ev8_cSAngle_addeq(gabi::ea(&at<ItemCameraFrame1HD>(F)->mYaw),
                          gabi::ea(&at<ItemCameraFrame1HD>(F)->mAngleB));
        ev8_cSGlobe_ValRVU(gabi::ea(&this->mViewCache.mDirection), rad,
                           gabi::ea(&at<ItemCameraFrame1HD>(F)->mPitch),
                           gabi::ea(&at<ItemCameraFrame1HD>(F)->mYaw)); /* cSGlobe::Val(r, V, U) */
        ev8_cSGlobe_Xyz(gabi::ea(&this->mViewCache.mDirection),
                        gabi::ea(&at<ItemCameraFrame1HD>(F)->mDelta));
        ev8_cXyz_pl(gabi::ea(&this->mViewCache.mCenter),
                    gabi::ea(&at<ItemCameraFrame1HD>(F)->mPoint),
                    gabi::ea(&at<ItemCameraFrame1HD>(F)->mDelta));
        copy3(gabi::ea(&this->mViewCache.mEye), gabi::ea(&at<ItemCameraFrame1HD>(F)->mPoint));
        u32 c = work.mFrame;
        if ((s32)c < (s32)work.mBlendFrames) {
            work.mFrame = c + 1;
            return 0;
        }
        work.mState = ITEM_HOLD;
        s = ITEM_HOLD;
    }
    if (s == ITEM_HOLD || s == ITEM_DONE) {
        if (s == ITEM_HOLD) {
            ev8_relationalPos(i_this, gabi::ea(&at<ItemCameraFrame1HD>(F)->mPoint),
                              this->mpPlayerActor.v.get(),
                              at<be<u32>>(ctrTab + (work.mType << 2))->get());
            copy3(gabi::ea(&this->mViewCache.mCenter),
                  gabi::ea(&at<ItemCameraFrame1HD>(F)->mPoint));
            ev8_cSGlobe_Xyz(gabi::ea(&this->mViewCache.mDirection),
                            gabi::ea(&at<ItemCameraFrame1HD>(F)->mDelta));
            ev8_cXyz_pl(gabi::ea(&this->mViewCache.mCenter),
                        gabi::ea(&at<ItemCameraFrame1HD>(F)->mPoint),
                        gabi::ea(&at<ItemCameraFrame1HD>(F)->mDelta));
            copy3(gabi::ea(&this->mViewCache.mEye), gabi::ea(&at<ItemCameraFrame1HD>(F)->mPoint));
            u32 n = work.mHoldFrames + 1;
            u32 type = work.mType;
            work.mHoldFrames = n;
            if (type == 0 && n == 1) {
                work.mType = 4;
                u32 c = work.mFrame;
                work.mState = ITEM_WAIT;
                work.mFrame = c + 1;
                return 0;
            }
            work.mState = ITEM_DONE;
        }
        setDone(i_this);
        ev8_relationalPos(i_this, gabi::ea(&at<ItemCameraFrame1HD>(F)->mPoint),
                          this->mpPlayerActor.v.get(),
                          at<be<u32>>(ctrTab + (work.mType << 2))->get());
        copy3(gabi::ea(&this->mViewCache.mCenter), gabi::ea(&at<ItemCameraFrame1HD>(F)->mPoint));
        ev8_cSGlobe_Xyz(gabi::ea(&this->mViewCache.mDirection),
                        gabi::ea(&at<ItemCameraFrame1HD>(F)->mDelta));
        ev8_cXyz_pl(gabi::ea(&this->mViewCache.mCenter),
                    gabi::ea(&at<ItemCameraFrame1HD>(F)->mPoint),
                    gabi::ea(&at<ItemCameraFrame1HD>(F)->mDelta));
        copy3(gabi::ea(&this->mViewCache.mEye), gabi::ea(&at<ItemCameraFrame1HD>(F)->mPoint));
        work.mFrame = work.mFrame + 1;
        return 1;
    }
    if (s != ITEM_WAIT) {
        ev8_getEvIntData(i_this, gabi::ea(&work.mType),
                         gabi::ea(itemCameraData.mType1) /* "Type" */, 0);
        work.mHoldFrames = ITEM_INIT;
    }
    work.mFrame = 0;
    if ((this->m07C & 7) == 0) {
        u32 type = work.mType;
        if (type >= 2 && type <= 3) {
            u32 p = at<be<u32>>(eyeTab + (type << 2))->get();
            u32 x1 = at<be<u32>>(p + 0x10)->get(), x0 = at<be<u32>>(p + 0xC)->get();
            f32 a0 = at<cXyz>(p)[0].x.get();
            *at<be<u32>>(p + 0) = x0;
            f32 a1 = at<cXyz>(p)[0].y.get();
            *at<be<u32>>(p + 4) = x1;
            u32 x2 = at<be<u32>>(p + 0x14)->get();
            f32 a2 = at<cXyz>(p)[0].z.get();
            *at<be<u32>>(p + 8) = x2;
            u32 q = at<be<u32>>(eyeTab + (work.mType << 2))->get();
            at<cXyz>(q)[1].y = a1;
            at<cXyz>(q)[1].z = a2;
            at<cXyz>(q)[1].x = a0;
        }
    }
    ev8_relationalPos(i_this, gabi::ea(&at<ItemCameraFrame1HD>(F)->mDelta),
                      this->mpPlayerActor.v.get(), at<be<u32>>(ctrTab + (work.mType << 2))->get());
    copy3(gabi::ea(&work.mCenter), gabi::ea(&at<ItemCameraFrame1HD>(F)->mDelta));
    u32 E = gabi::ea(&at<ItemCameraFrame1HD>(F)->mEye);
    for (s32 i = 0; i < (s32)at<be<u32>>(gabi::ea(&at<ItemCameraFrame1HD>(F)->mEyeCounts[0]) +
                                         (work.mType << 2))
                            ->get();
         i++) {
        ev8_relationalPos(i_this, gabi::ea(&at<ItemCameraFrame1HD>(F)->mPosition),
                          this->mpPlayerActor.v.get(),
                          at<be<u32>>(eyeTab + (work.mType << 2))->get() + (u32)i * 12);
        copy3(E, gabi::ea(&at<ItemCameraFrame1HD>(F)->mPosition));
        ev8_positionOf(i_this, gabi::ea(&at<ItemCameraFrame1HD>(F)->mPosition),
                       this->mpPlayerActor.v.get());
        if (at<cXyz>(E)[0].y.get() <
            at<ItemCameraFrame1HD>(F)->mPosition.y + this->m368) { /* bge: taken on NaN */
            ev8_positionOf(i_this, gabi::ea(&at<ItemCameraFrame1HD>(F)->mPlayerPosition),
                           this->mpPlayerActor.v.get());
            at<cXyz>(E)[0].y = at<ItemCameraFrame1HD>(F)->mPlayerPosition.y + this->m368;
        }
        if (ev8_lineBGCheck(i_this, gabi::ea(&work.mCenter), E, 0x8F))
            continue;
        copy3f(gabi::ea(&at<ItemCameraFrame1HD>(F)->mPosition), gabi::ea(&work.mCenter));
        copy3f(gabi::ea(&at<ItemCameraFrame1HD>(F)->mCollisionEye), E);
        u32 pl = this->mpPlayerActor.v.get();
        u32 g = dComIfGp_ea();
        if (!ev8_ChkCamera(gabi::ea(&at<dComIfG_inf_c>(g)->play.mCcS),
                           gabi::ea(&at<ItemCameraFrame1HD>(F)->mPosition),
                           gabi::ea(&at<ItemCameraFrame1HD>(F)->mCollisionEye), itemValue15, pl, 0))
            break; /* dCcS::ChkCamera */
    }
    ev8_cXyz_mi(E, gabi::ea(&at<ItemCameraFrame1HD>(F)->mDelta), gabi::ea(&work.mCenter));
    ev8_cSGlobe_Val(gabi::ea(&work.mTargetDirection), gabi::ea(&at<ItemCameraFrame1HD>(F)->mDelta));
    u32 type = work.mType;
    u32 cnt = work.mFrame;
    work.mFovy =
        at<be<f32>>(gabi::ea(&at<ItemCameraFrame1HD>(F)->mFovyBits[0]) + (type << 2))->get();
    u32 fr =
        at<be<u32>>(gabi::ea(&at<ItemCameraFrame1HD>(F)->mBlendFrames[0]) + (type << 2))->get();
    work.mState = ITEM_BLEND;
    work.mBlendFrames = fr;
    work.mFrame = cnt + 1;
    return 0;
}

/* WWHD glue
ext 02530634 u32 ev8_getEvIntData(u32 c, u32 out, u32 name, s32 def)
ext 0250242C void ev8_relationalPos(u32 c, u32 out, u32 actor, u32 ofs)
ext 024F8D5C void ev8_positionOf(u32 c, u32 out, u32 actor)
ext 024FCBE8 bool ev8_lineBGCheck(u32 c, u32 a, u32 b, u32 flags)
ext 0201ADE0 void ev8_cXyz_mi(u32 a, u32 out, u32 b)
ext 0201AD78 void ev8_cXyz_pl(u32 a, u32 out, u32 b)
ext 0201AE48 void ev8_cXyz_ml(u32 a, u32 out, f32 s)
ext 020072E0 void ev8_cSGlobe_Val(u32 g, u32 v)
ext 020073AC void ev8_cSGlobe_Xyz(u32 g, u32 out)
ext 020068B0 void ev8_cSAngle_mi(u32 a, u32 out, u32 b)
ext 0200693C void ev8_cSAngle_ml(u32 a, u32 out, f32 f)
ext 020068CC void ev8_cSAngle_addeq(u32 a, u32 b)
ext 020065FC void ev8_cSAngle_default(u32)
ext 028E8D88 void ev8_PSVECAdd(u32,u32,u32)
ext 020071E0 void ev8_cSGlobe_ValRVU(u32,f32,u32,u32)
ext 025D5218 u32 ev8_JudgeActor(u32,u32)
ext 025187D8 bool ev8_ChkCamera(u32,u32,u32,f32,u32,u32)
data itemCameraData 1004D2A8
data itemValue15 1004CF80
data itemValue0 1004CE58
data itemValue70 1004CF70
data itemValue100 1004CEB4
data itemValueMinus70 1004D0D4
data itemValueMinus90 1004D1CC
data itemValue55 1004D1C0
data itemValueMinus50 1004D0BC
data itemValue115 1004D0E8
data itemValue60 1004D004
data itemValue5 1004CEB8
data itemValue110 1004D0D8
data itemValueMinus40 1004D0DC
data itemValueMinus25 1004D0B4
data itemValueMinus10 1004D1C8
data itemValueMinus5 1004CF68
data itemValueMinus30 1004D1D4
data itemValue10 1004CF74
data itemValue85 1004D0B8
data itemValue45 1004D008
*/
