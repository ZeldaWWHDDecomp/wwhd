/* f_op_actor_mng part 2: 025D63E8..025D7D3C — entry heaps, cull sizes, movement, distances,
 * culling, event orders and partners. WWHD. 
 * See f_op_actor_mng.cpp for the unit's range.
 *
 * HD event types (dEvt_control_c::order): TALK 0, DOOR 1, OTHER 2, POTENTIAL 4, ITEM 5,
 * SHOWITEM X/Y/Z 6/7/8, a new type 9 (HD), CATCH 0xA, TREASURE 0xB, CHANGE 0xD (GameCube
 * CATCH 9, TREASURE 0xA, CHANGE 0xC). order() takes the info index as its 9th (stack) argument.
 * The player is play+0x5B2C, the event control play+0x51D0, the event manager play+0x52C4.
 *
 * Culling (HD): after the J3D clipper (1048CFF0) accepts a box or sphere, a second test runs
 * through a pointer-to-member function of the object at **(*(101F95D0)+0x1024) (box: member at
 * +0x7DC, sphere: +0x7E4) on the world-space bounds; the actor is culled only if both fail
 * (probably the GamePad / second view). */
#include "bindings.h"

namespace f_op_actor_mng_2_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void JUT_ASSERT_l(u32 file, s32 line, u32 msg) { gabi::call(0x0273AA24, file, line, msg); }
static inline void OSReport_Warning_l(u32 fmt, u32 a, u32 b) { gabi::call(0x025F27E8, fmt, a, b); }
static inline u32 mDoExt_getHeap_l(s32 i) { return gabi::call<u32>(0x025E2F64, i); } /* HD: heap table 1048CF28[i] */
static inline BOOL fopAcM_entrySolidHeap__l(u32 a, u32 cb, u32 size, u32 heap) { return gabi::call<BOOL>(0x025D6174, a, cb, size, heap); }
static inline u32 fopAcM_getProcNameString_l(u32 a) { return gabi::call<u32>(0x025D5FA4, a); }
static inline BOOL cLib_chaseAngleS_l(u32 p, u32 target, u32 step) { return gabi::call<BOOL>(0x0200F8D0, p, target, step); }
static inline s16 cLib_targetAngleY_l(u32 a, u32 b) { return gabi::call<s16>(0x0200F93C, a, b); }
static inline void cXyz_mi_l(u32 self, u32 out, u32 other) { gabi::call(0x0201ADE0, self, out, other); }
static inline f32 PSVECSquareMag_l(u32 v) { return gabi::call<f32>(0x028E8DD0, v); }
static inline f32 sqrtf_l(f32 x) { return gabi::call<f32>(0x028F4384, x); }
static inline void PSMTXConcat_l(u32 a, u32 b, u32 out) { gabi::call(0x028E9108, a, b, out); }
static inline void PSMTXMultVec_l(u32 m, u32 in, u32 out) { gabi::call(0x028E8F64, m, in, out); }
static inline BOOL J3DUClipper_clipBox_l(u32 self, u32 mtx, u32 max, u32 min) { return gabi::call<BOOL>(0x02838274, self, mtx, max, min); }
static inline BOOL J3DUClipper_clipSphere_l(u32 self, u32 mtx, u32 c, f32 r) { return gabi::call<BOOL>(0x02838148, self, mtx, c, r); }
static inline void J3DUClipper_calcViewFrustum_l(u32 self) { gabi::call(0x0283801C, self); }
static inline void box_init_l(u32 box) { gabi::call(0x0286DE88, box); }                    /* HD AABB helper */
static inline void box_transform_l(u32 out, u32 in, u32 mtx) { gabi::call(0x0286DC38, out, in, mtx); }
static inline s32 dEvt_order_l(u32 evt, u32 type, u32 prio, u32 flag, u32 hind, u32 a1, u32 a2, u32 evId, u32 info) {
    return gabi::call<s32>(0x0253EC0C, evt, type, prio, flag, hind, a1, a2, evId, info);
}
static inline s32 dEvt_orderOld_l(u32 evt, u32 type, u32 prio, u32 flag, u32 hind, u32 a1, u32 a2, u32 name) {
    return gabi::call<s32>(0x0253ED80, evt, type, prio, flag, hind, a1, a2, name);
}
static inline u32 dEvt_convPId_l(u32 evt, u32 id) { return gabi::call<u32>(0x0253EE04, evt, id); }
static inline u32 dEvmng_getEventIdx_l(u32 mng, u32 name, u32 room) { return gabi::call<u32>(0x02543F10, mng, name, room); }
static inline u32 dEvmng_getEventPrio_l(u32 mng, u32 idx) { return gabi::call<u32>(0x025445F4, mng, idx); }

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline f32 ldf(u32 a) { return gabi::load<f32>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }

static constexpr u32 sincosTable = 0x104A44F8; /* {sin, cos} per (u16)angle >> 3 */
static constexpr u32 viewMtx = 0x104B45F8;     /* j3dSys view matrix */
static constexpr u32 clipper = 0x1048CFF0;     /* mDoLib_clipper (J3DUClipper), far at +0x54 */
static constexpr u32 systemFar = 0x1048D04C;   /* mDoLib_clipper::mSystemFar */
static constexpr u32 l_cullSizeBox = 0x101F30F0;    /* 14 x {min, max} */
static constexpr u32 l_cullSizeSphere = 0x101F3150; /* indexed by the cull type (0xF..0x16) x 0x10 */

struct u32_l { be<u32> v; };
struct cXyz_l { be<f32> x, y, z; };
struct box_l { be<f32> v[6]; };
struct sphere_l { be<f32> v[4]; };
struct mtx_l { be<f32> m[12]; };
struct sstr_l { be<u32> str, vt; };

static inline u32 player() { return ld(dComIfGp_ea() + 0x5B2C); }

/* 025D63E8: HD: picks the parent heap by stage ("sea") and actor, and ignores the estimate except
 * for a few actors with fixed sizes; on failure it retries in the other heap */
static BOOL fopAcM_entrySolidHeap(u32 i_this, u32 createHeapCB, u32 estimatedHeapSize) {
    WWHD_FUNC(0x025D63E8, BOOL, i_this, createHeapCB, estimatedHeapSize);
    (void)estimatedHeapSize;
    u32 name = i_this != 0 ? (u32)(s32)(s16)ld16(i_this + 0xE) : 0x7FFF; /* fopAcM_GetProfName */
    fopAcM_getProcNameString_l(i_this);
    mDoExt_getHeap_l(0);
    mDoExt_getHeap_l(1);
    u32 play = dComIfGp_ea();
    gabi::Local<sstr_l> stage, sea; /* sead::SafeString temporaries */
    sea->str = 0x10057434; /* "sea" */
    sea->vt = 0x1005716C;
    stage->str = play + 0x5134; /* dComIfGp_getStartStageName() */
    stage->vt = 0x1005716C;
    gabi::call(0x025DA2F4, gabi::ea(stage.get()));
    gabi::call_ptr(ld(stage->vt + 0x14), gabi::ea(stage.get()));
    u32 s1 = stage->str;
    gabi::call_ptr(ld(sea->vt + 0x14), gabi::ea(sea.get()));
    u32 s2 = sea->str;
    bool isSea = false;
    if (s1 == s2) {
        isSea = true;
    } else {
        u32 p = s1, q = s2;
        for (u32 n = 0; n < 0x40001; n++, p++, q++) {
            u8 c = ld8(p);
            if (c != ld8(q)) break;
            if (c == 0) { isSea = true; break; }
        }
    }
    u32 heap0, heap1;
    if (isSea || !(ld(0x1047E6C0) & 1)) {
        heap0 = mDoExt_getHeap_l(0);
        heap1 = mDoExt_getHeap_l(1);
    } else {
        heap0 = mDoExt_getHeap_l(1);
        heap1 = mDoExt_getHeap_l(0);
    }
    /* which heap and size: 0 = the first heap, 1 = the second, 2 = a fixed size (special) */
    u32 heap = heap1, size = 0;
    bool special = false;
    if (name < 0x191) {
        if (name == 0x26 || name == 0xA5 || name == 0xA8) heap = heap0;
        else if (name == 0xC2) special = true;
    } else {
        if (name == 0x191) heap = heap0;
        else if (name < 0x1B5) heap = heap1;
        else if (name < 0x1B7) heap = heap0;
        else if (name == 0x1B7) heap = ld(i_this + 0xB0) != 0 ? heap1 : heap0;
        else if (name == 0x1BD) {
            u32 prm = ld(i_this + 0xB0);
            if (prm == 1) size = 0x68CC;
            else if (prm == 0xB) size = 0x9ECC;
            else if (prm == 0xD) size = 0x6FA0;
            else special = true;
        }
    }
    BOOL ret;
    if (special) {
        u32 h = ld(0x1047C8B4);
        size = 0x4FA0;
        if (h != 0) {
            heap = h;
            ret = fopAcM_entrySolidHeap__l(i_this, createHeapCB, size, heap);
            if (ret != 0) return ret;
            goto retry;
        }
        heap = heap0;
    }
    if (heap == 0) JUT_ASSERT_l(0x1005744C, 0x4F3, 0x10057438);
    ret = fopAcM_entrySolidHeap__l(i_this, createHeapCB, size, heap);
    if (ret != 0) return ret;
retry:
    if (heap == heap1 || heap1 == 0) return ret;
    gabi::call_ptr(ld(ld(heap + 0x14) + 0x14), heap + 0x10);
    u32 n0 = ld(heap + 0x10);
    gabi::call_ptr(ld(ld(heap1 + 0x14) + 0x14), heap1 + 0x10);
    OSReport_Warning_l(0x10057460, n0, ld(heap1 + 0x10));
    return fopAcM_entrySolidHeap__l(i_this, createHeapCB, size, heap1);
}
VERIFY(0x025D63E8, fopAcM_entrySolidHeap);

/* 025D672C: fopAcM_SetMin (inline on GameCube) */
static void fopAcM_SetMin(u32 i_this, f32 x, f32 y, f32 z) {
    WWHD_FUNC(0x025D672C, void, i_this, x, y, z);
    stf(i_this + 0x350, y);
    stf(i_this + 0x354, z);
    stf(i_this + 0x34C, x);
}
VERIFY(0x025D672C, fopAcM_SetMin);

/* 025D673C: fopAcM_SetMax (inline on GameCube) */
static void fopAcM_SetMax(u32 i_this, f32 x, f32 y, f32 z) {
    WWHD_FUNC(0x025D673C, void, i_this, x, y, z);
    stf(i_this + 0x35C, y);
    stf(i_this + 0x360, z);
    stf(i_this + 0x358, x);
}
VERIFY(0x025D673C, fopAcM_SetMax);

/* 025D674C */
static void fopAcM_setCullSizeBox(u32 i_this, f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1) {
    WWHD_FUNC(0x025D674C, void, i_this, x0, y0, z0, x1, y1, z1);
    stf(i_this + 0x34C, x0);
    stf(i_this + 0x350, y0);
    stf(i_this + 0x354, z0);
    stf(i_this + 0x358, x1);
    stf(i_this + 0x35C, y1);
    stf(i_this + 0x360, z1);
}
VERIFY(0x025D674C, fopAcM_setCullSizeBox);

/* 025D6768 */
static void fopAcM_setCullSizeSphere(u32 i_this, f32 x, f32 y, f32 z, f32 r) {
    WWHD_FUNC(0x025D6768, void, i_this, x, y, z, r);
    stf(i_this + 0x34C, x);
    stf(i_this + 0x350, y);
    stf(i_this + 0x354, z);
    stf(i_this + 0x358, r);
}
VERIFY(0x025D6768, fopAcM_setCullSizeSphere);

/* 025D677C */
static BOOL fopAcM_addAngleY(u32 i_this, u32 target, u32 step) {
    WWHD_FUNC(0x025D677C, BOOL, i_this, target, step);
    return cLib_chaseAngleS_l(i_this + 0x322, target, step) != 0;
}
VERIFY(0x025D677C, fopAcM_addAngleY);

/* 025D67A8 */
static void fopAcM_calcSpeed(u32 i_this) {
    WWHD_FUNC(0x025D67A8, void, i_this);
    u32 t = sincosTable + ((u32)(ld16(i_this + 0x322) >> 3) << 3);
    f32 speedF = ldf(i_this + 0x370);
    f32 vy = gabi::fadds_ppc(ldf(i_this + 0x340), ldf(i_this + 0x374));
    f32 maxFall = ldf(i_this + 0x378);
    f32 vx = gabi::fmuls_ppc(speedF, ldf(t));
    f32 vz = gabi::fmuls_ppc(speedF, ldf(t + 4));
    if (vy < maxFall) vy = maxFall;
    stf(i_this + 0x340, vy);
    stf(i_this + 0x344, vz);
    stf(i_this + 0x33C, vx);
}
VERIFY(0x025D67A8, fopAcM_calcSpeed);

/* 025D6800 */
static void fopAcM_posMove(u32 i_this, u32 i_movePos) {
    WWHD_FUNC(0x025D6800, void, i_this, i_movePos);
    f32 sy = ldf(i_this + 0x340), sx = ldf(i_this + 0x33C);
    f32 py = ldf(i_this + 0x318), px = ldf(i_this + 0x314), pz = ldf(i_this + 0x31C);
    f32 sz = ldf(i_this + 0x344);
    stf(i_this + 0x318, gabi::fadds_ppc(py, sy));
    stf(i_this + 0x314, gabi::fadds_ppc(px, sx));
    stf(i_this + 0x31C, gabi::fadds_ppc(pz, sz));
    if (i_movePos != 0) {
        u32 p = i_this + 0x314;
        f32 x = gabi::fadds_ppc(ldf(p), ldf(i_movePos));
        f32 y = ldf(p + 4);
        stf(p, x);
        y = gabi::fadds_ppc(y, ldf(i_movePos + 4));
        f32 z = ldf(p + 8);
        stf(p + 4, y);
        z = gabi::fadds_ppc(z, ldf(i_movePos + 8));
        stf(p + 8, z);
    }
}
VERIFY(0x025D6800, fopAcM_posMove);

/* 025D6870 */
static void fopAcM_posMoveF(u32 i_this, u32 i_movePos) {
    WWHD_FUNC(0x025D6870, void, i_this, i_movePos);
    fopAcM_calcSpeed(i_this);
    fopAcM_posMove(i_this, i_movePos);
}
VERIFY(0x025D6870, fopAcM_posMoveF);

/* 025D6894 */
static s16 fopAcM_searchActorAngleY(u32 i_this, u32 i_other) {
    WWHD_FUNC(0x025D6894, s16, i_this, i_other);
    return cLib_targetAngleY_l(i_this + 0x314, i_other + 0x314);
}
VERIFY(0x025D6894, fopAcM_searchActorAngleY);

/* 025D68A0 */
static s32 fopAcM_seenActorAngleY(u32 i_this, u32 i_other) {
    WWHD_FUNC(0x025D68A0, s32, i_this, i_other);
    s16 target = cLib_targetAngleY_l(i_this + 0x314, i_other + 0x314);
    s16 d = (s16)(target - (s16)ld16(i_this + 0x32A));
    return d < 0 ? -(s32)d : (s32)d;
}
VERIFY(0x025D68A0, fopAcM_seenActorAngleY);

/* 025D68EC */
static f32 fopAcM_searchActorDistance(u32 i_this, u32 i_other) {
    WWHD_FUNC(0x025D68EC, f32, i_this, i_other);
    gabi::Local<cXyz_l> d;
    cXyz_mi_l(i_other + 0x314, gabi::ea(d.get()), i_this + 0x314);
    return sqrtf_l(PSVECSquareMag_l(gabi::ea(d.get())));
}
VERIFY(0x025D68EC, fopAcM_searchActorDistance);

/* 025D6924 */
static f32 fopAcM_searchActorDistance2(u32 i_this, u32 i_other) {
    WWHD_FUNC(0x025D6924, f32, i_this, i_other);
    gabi::Local<cXyz_l> d;
    cXyz_mi_l(i_other + 0x314, gabi::ea(d.get()), i_this + 0x314);
    return PSVECSquareMag_l(gabi::ea(d.get()));
}
VERIFY(0x025D6924, fopAcM_searchActorDistance2);

/* 025D6958 */
static f32 fopAcM_searchActorDistanceXZ(u32 i_this, u32 i_other) {
    WWHD_FUNC(0x025D6958, f32, i_this, i_other);
    gabi::Local<cXyz_l> xz, d;
    cXyz_mi_l(i_other + 0x314, gabi::ea(d.get()), i_this + 0x314);
    xz->x = (f32)d->x;
    xz->y = 0.0f;
    xz->z = (f32)d->z;
    return sqrtf_l(PSVECSquareMag_l(gabi::ea(xz.get())));
}
VERIFY(0x025D6958, fopAcM_searchActorDistanceXZ);

/* 025D69AC */
static f32 fopAcM_searchActorDistanceXZ2(u32 i_this, u32 i_other) {
    WWHD_FUNC(0x025D69AC, f32, i_this, i_other);
    gabi::Local<cXyz_l> xz, d;
    cXyz_mi_l(i_other + 0x314, gabi::ea(d.get()), i_this + 0x314);
    xz->x = (f32)d->x;
    xz->y = 0.0f;
    xz->z = (f32)d->z;
    return PSVECSquareMag_l(gabi::ea(xz.get()));
}
VERIFY(0x025D69AC, fopAcM_searchActorDistanceXZ2);

/* 025D69FC: HD: onFrollCrashFlg is a virtual of the player (vtable +0xF4) */
static BOOL fopAcM_rollPlayerCrash(u32 i_this, f32 distAdjust, u32 flag) {
    WWHD_FUNC(0x025D69FC, BOOL, i_this, distAdjust, flag);
    f32 maxDist = gabi::fadds_ppc(distAdjust, 40.0f);
    f32 xzDist2 = fopAcM_searchActorDistanceXZ2(i_this, player());
    u32 pl = player();
    f32 mm = gabi::fmuls_ppc(maxDist, maxDist);
    f32 yDist = gabi::fsubs_ppc(ldf(pl + 0x318), ldf(i_this + 0x318));
    if (!(xzDist2 < mm)) return FALSE;
    if (!(yDist > -100.0f)) return FALSE;
    if (!(yDist < 200.0f)) return FALSE;
    u32 py = player();
    u32 other = player();
    s16 angle = fopAcM_searchActorAngleY(i_this, other);
    u32 t = sincosTable + ((u32)((u16)((s16)ld16(py + 0x322) - angle) >> 3) << 3);
    if (!(ldf(t + 4) < -0.9f)) return FALSE;
    if (py == 0) return FALSE;
    if ((s16)ld16(py + 8) != 0xA8) return FALSE; /* fpcNm_PLAYER */
    gabi::call_ptr(ld(ld(py + 0xB4) + 0xF4), py, flag);
    return TRUE;
}
VERIFY(0x025D69FC, fopAcM_rollPlayerCrash);

/* the second culling test: a pointer to member function at obj+off, on the world-space bounds */
static inline u32 pmf_call(u32 obj, u32 off, u32 arg) {
    s16 vidx = (s16)ld16(obj + off + 2);
    s16 delta = (s16)ld16(obj + off);
    u32 self = obj + 0x610 + (s32)delta;
    u32 fn;
    if (vidx < 0) fn = ld(obj + off + 4);
    else fn = ld(ld(self + (s32)(s16)ld16(obj + off + 6)) + (u32)((s32)vidx << 3) + 4);
    return gabi::call_ptr<u32>(fn, self, arg, 0);
}
static inline u32 second_view() { return ld(ld(ld(0x101F95D0) + 0x1024)); }

/* 025D6B70: HD: also tests the second view */
static BOOL fopAcM_checkCullingBox(u32 m, f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1) {
    WWHD_FUNC(0x025D6B70, BOOL, m, x0, y0, z0, x1, y1, z1);
    gabi::Local<cXyz_l> p0, p1;
    gabi::Local<mtx_l> mtx;
    p1->z = z1; p0->y = y0; p0->z = z0; p1->y = y1; p0->x = x0; p1->x = x1;
    PSMTXConcat_l(viewMtx, m, gabi::ea(mtx.get()));
    if (J3DUClipper_clipBox_l(clipper, gabi::ea(mtx.get()), gabi::ea(p1.get()), gabi::ea(p0.get())) == 0) return FALSE;
    gabi::Local<box_l> box, box2;
    for (int i = 0; i < 3; i++) gabi::store<u32>(gabi::ea(box.get()) + 4 * i, gabi::load<u32>(gabi::ea(p0.get()) + 4 * i));
    for (int i = 0; i < 3; i++) gabi::store<u32>(gabi::ea(box.get()) + 12 + 4 * i, gabi::load<u32>(gabi::ea(p1.get()) + 4 * i));
    box_init_l(gabi::ea(box.get()));
    box_transform_l(gabi::ea(box.get()), gabi::ea(box.get()), m);
    u32 obj = second_view();
    for (int i = 0; i < 6; i++) box2->v[i] = (f32)box->v[i];
    return pmf_call(obj, 0x7DC, gabi::ea(box2.get())) == 0;
}
VERIFY(0x025D6B70, fopAcM_checkCullingBox);

/* the box branch of fopAcM_cullingCheck: max/min of the J3D clip box */
static BOOL cull_box(u32 pMtx, u32 mtx, f32 cullFar, f32 sizeFar, u32 max, u32 min) {
    BOOL r;
    if (sizeFar > 0.0f) {
        stf(clipper + 0x54, gabi::fmuls_ppc(ldf(systemFar), cullFar));
        J3DUClipper_calcViewFrustum_l(clipper);
        r = J3DUClipper_clipBox_l(clipper, pMtx, max, min);
        stf(clipper + 0x54, ldf(systemFar));
        J3DUClipper_calcViewFrustum_l(clipper);
    } else {
        r = J3DUClipper_clipBox_l(clipper, pMtx, max, min);
    }
    if (r == 0) return FALSE;
    gabi::Local<box_l> box2;
    gabi::Local<box_l> box;
    u32 b = gabi::ea(box.get());
    for (int i = 0; i < 3; i++) st(b + 4 * i, ld(min + 4 * i));
    for (int i = 0; i < 3; i++) st(b + 12 + 4 * i, ld(max + 4 * i));
    box_init_l(b);
    if (mtx != 0) box_transform_l(b, b, mtx);
    u32 obj = second_view();
    for (int i = 0; i < 6; i++) box2->v[i] = (f32)box->v[i];
    return pmf_call(obj, 0x7DC, gabi::ea(box2.get())) == 0;
}

/* the sphere branch: center/radius of the J3D clip, then the second view with the actor's radius */
static BOOL cull_sphere(u32 i_this, u32 pMtx, u32 mtx, f32 cullFar, f32 sizeFar, u32 center, u32 radius) {
    BOOL r;
    gabi::Local<cXyz_l> c;
    if (sizeFar > 0.0f) {
        stf(clipper + 0x54, gabi::fmuls_ppc(ldf(systemFar), cullFar));
        J3DUClipper_calcViewFrustum_l(clipper);
        for (int i = 0; i < 3; i++) st(gabi::ea(c.get()) + 4 * i, ld(center + 4 * i));
        r = J3DUClipper_clipSphere_l(clipper, pMtx, gabi::ea(c.get()), ldf(radius));
        stf(clipper + 0x54, ldf(systemFar));
        J3DUClipper_calcViewFrustum_l(clipper);
    } else {
        for (int i = 0; i < 3; i++) st(gabi::ea(c.get()) + 4 * i, ld(center + 4 * i));
        r = J3DUClipper_clipSphere_l(clipper, pMtx, gabi::ea(c.get()), ldf(radius));
    }
    if (r == 0) return FALSE;
    gabi::Local<cXyz_l> w;
    gabi::Local<sphere_l> sph;
    f32 z = ldf(center + 8), y = ldf(center + 4), x = ldf(center + 0);
    w->y = y; w->x = x; w->z = z;
    if (mtx != 0) {
        PSMTXMultVec_l(mtx, gabi::ea(w.get()), gabi::ea(w.get()));
        y = w->y; x = w->x; z = w->z;
    }
    u32 obj = second_view();
    f32 rr = ldf(i_this + 0x358); /* HD: the actor's custom radius, also for the table spheres */
    sph->v[0] = x; sph->v[1] = y; sph->v[2] = z; sph->v[3] = rr;
    return pmf_call(obj, 0x7E4, gabi::ea(sph.get())) == 0;
}

/* 025D6CE8 */
static BOOL fopAcM_cullingCheck(u32 i_this) {
    WWHD_FUNC(0x025D6CE8, BOOL, i_this);
    u32 mtx = ld(i_this + 0x348);
    gabi::Local<mtx_l> tmp;
    u32 pMtx;
    if (mtx == 0) {
        pMtx = viewMtx;
    } else {
        PSMTXConcat_l(viewMtx, mtx, gabi::ea(tmp.get()));
        pMtx = gabi::ea(tmp.get());
    }
    f32 cullFar = ldf(i_this + 0x364);
    if (ld8(dComIfGp_ea() + 0x5292) != 0) cullFar = gabi::fmuls_ppc(cullFar, ldf(dComIfGp_ea() + 0x52B4)); /* event cull rate */
    u8 cull = ld8(i_this + 0x2DB);
    f32 sizeFar = ldf(i_this + 0x364);
    if (cull <= 0xE) {
        if (cull == 0xE) return cull_box(pMtx, mtx, cullFar, sizeFar, i_this + 0x358, i_this + 0x34C);
        u32 box = l_cullSizeBox + cull * 0x18;
        return cull_box(pMtx, mtx, cullFar, sizeFar, box + 0xC, box);
    }
    if (cull == 0x17) return cull_sphere(i_this, pMtx, mtx, cullFar, sizeFar, i_this + 0x34C, i_this + 0x358);
    u32 sph = l_cullSizeSphere + ((u32)cull << 4);
    return cull_sphere(i_this, pMtx, mtx, cullFar, sizeFar, sph, sph + 0xC);
}
VERIFY(0x025D6CE8, fopAcM_cullingCheck);

/* ---- event orders ---- */
static inline s32 order(u32 type, u32 prio, u32 flag, u32 hind, u32 a1, u32 a2) {
    return dEvt_order_l(dComIfGp_ea() + 0x51D0, type, prio, flag, hind, a1, a2, (u32)-1, 0xFF);
}

/* 025D744C */
static s32 fopAcM_orderTalkEvent(u32 i_this, u32 i_partner) {
    WWHD_FUNC(0x025D744C, s32, i_this, i_partner);
    return order(0, 0x1FF, 0, 0x14F, i_this, i_partner);
}
VERIFY(0x025D744C, fopAcM_orderTalkEvent);

/* 025D74B0 */
static s32 fopAcM_orderTalkXBtnEvent(u32 i_this, u32 i_partner) {
    WWHD_FUNC(0x025D74B0, s32, i_this, i_partner);
    return order(6, 0x1F4, 0, 0x14F, i_this, i_partner);
}
VERIFY(0x025D74B0, fopAcM_orderTalkXBtnEvent);

/* 025D7514 */
static s32 fopAcM_orderTalkYBtnEvent(u32 i_this, u32 i_partner) {
    WWHD_FUNC(0x025D7514, s32, i_this, i_partner);
    return order(7, 0x1F4, 0, 0x14F, i_this, i_partner);
}
VERIFY(0x025D7514, fopAcM_orderTalkYBtnEvent);

/* 025D7578 */
static s32 fopAcM_orderTalkZBtnEvent(u32 i_this, u32 i_partner) {
    WWHD_FUNC(0x025D7578, s32, i_this, i_partner);
    return order(8, 0x1F4, 0, 0x14F, i_this, i_partner);
}
VERIFY(0x025D7578, fopAcM_orderTalkZBtnEvent);

/* 025D75DC: HD-only: the same for the new event type 9 (probably a fourth item button) */
static s32 fopAcM_orderTalkBtn9Event(u32 i_this, u32 i_partner) {
    WWHD_FUNC(0x025D75DC, s32, i_this, i_partner);
    return order(9, 0x1F4, 0, 0x14F, i_this, i_partner);
}
VERIFY(0x025D75DC, fopAcM_orderTalkBtn9Event);

/* 025D7640 */
static s32 fopAcM_orderZHintEvent(u32 i_this, u32 i_partner) {
    WWHD_FUNC(0x025D7640, s32, i_this, i_partner);
    return order(0, 0x1FF, 0, 0xFFFF, i_this, i_partner);
}
VERIFY(0x025D7640, fopAcM_orderZHintEvent);

/* 025D76A8 */
static s32 fopAcM_orderSpeakEvent(u32 i_this) {
    WWHD_FUNC(0x025D76A8, s32, i_this);
    u32 pl = player();
    return order(0, 0x1EA, 0, 0x14F, pl, i_this);
}
VERIFY(0x025D76A8, fopAcM_orderSpeakEvent);

/* 025D7710 */
static s32 fopAcM_orderDoorEvent(u32 i_this, u32 i_partner) {
    WWHD_FUNC(0x025D7710, s32, i_this, i_partner);
    return order(1, 0xFF, 0, 0xFFFF, i_this, i_partner);
}
VERIFY(0x025D7710, fopAcM_orderDoorEvent);

/* 025D7774 */
static s32 fopAcM_orderCatchEvent(u32 i_this, u32 i_partner) {
    WWHD_FUNC(0x025D7774, s32, i_this, i_partner);
    return order(0xA, 1, 0, 0xFFFF, i_this, i_partner);
}
VERIFY(0x025D7774, fopAcM_orderCatchEvent);

static inline u32 event_prio(u32 idx) {
    u32 prio = dEvmng_getEventPrio_l(dComIfGp_ea() + 0x52C4, idx) & 0xFFFF;
    if (prio == 0) prio = 0xFF;
    return prio;
}

/* 025D77DC */
static s32 fopAcM_orderOtherEvent2(u32 i_this, u32 pEventName, u32 flag, u32 hind) {
    WWHD_FUNC(0x025D77DC, s32, i_this, pEventName, flag, hind);
    u32 idx = dEvmng_getEventIdx_l(dComIfGp_ea() + 0x52C4, pEventName, 0xFF);
    u32 prio = event_prio(idx);
    u32 pl = player();
    return dEvt_orderOld_l(dComIfGp_ea() + 0x51D0, 2, prio, flag, hind, i_this, pl, pEventName);
}
VERIFY(0x025D77DC, fopAcM_orderOtherEvent2);

/* 025D7874 */
static s32 fopAcM_orderChangeEventId(u32 i_this, u32 eventIdx, u32 flag, u32 hind) {
    WWHD_FUNC(0x025D7874, s32, i_this, eventIdx, flag, hind);
    u32 prio = event_prio(eventIdx);
    u32 pl = player();
    return dEvt_order_l(dComIfGp_ea() + 0x51D0, 0xD, prio, flag, hind, i_this, pl, eventIdx, 0xFF);
}
VERIFY(0x025D7874, fopAcM_orderChangeEventId);

/* 025D78FC */
static s32 fopAcM_orderChangeEvent(u32 i_this, u32 pEventName, u32 flag, u32 hind) {
    WWHD_FUNC(0x025D78FC, s32, i_this, pEventName, flag, hind);
    u32 idx = dEvmng_getEventIdx_l(dComIfGp_ea() + 0x52C4, pEventName, 0xFF);
    return fopAcM_orderChangeEventId(i_this, idx, flag, hind);
}
VERIFY(0x025D78FC, fopAcM_orderChangeEvent);

/* 025D7970 */
static s32 fopAcM_orderChangeEventId2(u32 i_this, u32 i_partner, u32 eventIdx, u32 flag, u32 hind) {
    WWHD_FUNC(0x025D7970, s32, i_this, i_partner, eventIdx, flag, hind);
    u32 prio = event_prio(eventIdx);
    return dEvt_order_l(dComIfGp_ea() + 0x51D0, 0xD, prio, flag, hind, i_this, i_partner, eventIdx, 0xFF);
}
VERIFY(0x025D7970, fopAcM_orderChangeEventId2);

/* 025D79F4 */
static s32 fopAcM_orderChangeEvent2(u32 i_this, u32 i_partner, u32 pEventName, u32 flag, u32 hind) {
    WWHD_FUNC(0x025D79F4, s32, i_this, i_partner, pEventName, flag, hind);
    u32 idx = dEvmng_getEventIdx_l(dComIfGp_ea() + 0x52C4, pEventName, 0xFF);
    return fopAcM_orderChangeEventId2(i_this, i_partner, idx, flag, hind);
}
VERIFY(0x025D79F4, fopAcM_orderChangeEvent2);

/* 025D7A58 */
static s32 fopAcM_orderOtherEventId(u32 i_this, u32 eventIdx, u32 infoIdx, u32 hind, u32 priority, u32 flag) {
    WWHD_FUNC(0x025D7A58, s32, i_this, eventIdx, infoIdx, hind, priority, flag);
    u32 prio = priority;
    if (priority == 0) prio = event_prio(eventIdx);
    u32 pl = player();
    return dEvt_order_l(dComIfGp_ea() + 0x51D0, 2, prio, flag, hind, i_this, pl, eventIdx, infoIdx);
}
VERIFY(0x025D7A58, fopAcM_orderOtherEventId);

/* 025D7B24 */
static s32 fopAcM_orderPotentialEvent(u32 i_this, u32 flag, u32 hind, u32 priority) {
    WWHD_FUNC(0x025D7B24, s32, i_this, flag, hind, priority);
    if (priority == 0) priority = 0xFF;
    u32 pl = player();
    return order(4, priority, flag, hind, i_this, pl);
}
VERIFY(0x025D7B24, fopAcM_orderPotentialEvent);

/* 025D7B98: HD: the hind flag is an argument (GameCube 0xFFFF) */
static s32 fopAcM_orderItemEvent(u32 i_this, u32 hind) {
    WWHD_FUNC(0x025D7B98, s32, i_this, hind);
    u32 pl = player();
    return order(5, 0xFF, 0, hind, pl, i_this);
}
VERIFY(0x025D7B98, fopAcM_orderItemEvent);

/* 025D7C08 */
static s32 fopAcM_orderTreasureEvent(u32 i_this, u32 i_partner) {
    WWHD_FUNC(0x025D7C08, s32, i_this, i_partner);
    return order(0xB, 0xFF, 0, 0xFFFF, i_this, i_partner);
}
VERIFY(0x025D7C08, fopAcM_orderTreasureEvent);

/* 025D7C6C */
static u32 fopAcM_getTalkEventPartner(u32 i_this) {
    WWHD_FUNC(0x025D7C6C, u32, i_this);
    u32 play = dComIfGp_ea();
    return dEvt_convPId_l(play + 0x51D0, ld(play + 0x529C));
}
VERIFY(0x025D7C6C, fopAcM_getTalkEventPartner);

/* 025D7C98 */
static u32 fopAcM_getItemEventPartner(u32 i_this) {
    WWHD_FUNC(0x025D7C98, u32, i_this);
    u32 play = dComIfGp_ea();
    return dEvt_convPId_l(play + 0x51D0, ld(play + 0x52A0));
}
VERIFY(0x025D7C98, fopAcM_getItemEventPartner);

/* 025D7CC4 */
static u32 fopAcM_getEventPartner(u32 i_this) {
    WWHD_FUNC(0x025D7CC4, u32, i_this);
    u32 play = dComIfGp_ea();
    u32 pt1 = dEvt_convPId_l(play + 0x51D0, ld(play + 0x5294));
    u32 evt = dComIfGp_ea() + 0x51D0;
    if (pt1 != i_this) return dEvt_convPId_l(evt, ld(evt + 0xC4));
    return dEvt_convPId_l(evt, ld(evt + 0xC8));
}
VERIFY(0x025D7CC4, fopAcM_getEventPartner);

} // namespace f_op_actor_mng_2_cpp
