/** Actor manager: heap selection, movement, culling and event orders.
 * WWHD source, verified against the native unit.
 * HD: heap selection depends on the stage and actor profile; culling also tests the second view. */
#include "f_op/f_op_actor_mng.h"



static inline u32 player() {
    return (u32)(actorMngView<dComIfG_inf_c>(dComIfGp_ea())->play.mPlayerInfo[0].mpPlayer.v);
}

// HD: picks the parent heap by stage ("sea") and actor, and ignores the estimate except
// for a few actors with fixed sizes; on failure it retries in the other heap
/* 025D63E8 */
static BOOL fopAcM_entrySolidHeapRegister(u32 i_this, u32 createHeapCB, u32 estimatedHeapSize) {
    fopAc_ac_c* actorView = actorMngActor(i_this);
    (void)estimatedHeapSize;
    u32 name = i_this != 0 ? (u32)(s32)(s16)(u16)actorView->mProfName : 0x7FFF; /* fopAcM_GetProfName */
    actMngGuest_fopAcM_getProcNameString(i_this);
    actMngGuest_mDoExt_getHeap(0);
    actMngGuest_mDoExt_getHeap(1);
    u32 play = dComIfGp_ea();
    gabi::Local<SafeString> stage, sea;        /* sead::SafeString temporaries */
    sea->mStringTop = gabi::guest_addr("sea"); /* "sea" */
    sea->__vtbl = gc_tu.safeString;
    stage->mStringTop = play + 0x5134; /* dComIfGp_getStartStageName() */
    stage->__vtbl = gc_tu.safeString;
    actorMngAssureTermination(gabi::ea(stage.get()));
    gabi::call_ptr((u32)actorMngView<ActorManagerSafeStringVtable>(stage->__vtbl)->assureTermination,
                   gabi::ea(stage.get()));
    u32 s1 = stage->mStringTop;
    gabi::call_ptr((u32)actorMngView<ActorManagerSafeStringVtable>(sea->__vtbl)->assureTermination,
                   gabi::ea(sea.get()));
    u32 s2 = sea->mStringTop;
    bool isSea = false;
    if (s1 == s2) {
        isSea = true;
    } else {
        u32 p = s1, q = s2;
        for (u32 n = 0; n < 0x40001; n++, p++, q++) {
            u8 c = (u8)*actorMngView<be<u8>>(p);
            if (c != (u8)*actorMngView<be<u8>>(q)) break;
            if (c == 0) {
                isSea = true;
                break;
            }
        }
    }
    u32 heap0, heap1;
    if (isSea || !((u32)stageHeapFlags & 1)) {
        heap0 = actMngGuest_mDoExt_getHeap(0);
        heap1 = actMngGuest_mDoExt_getHeap(1);
    } else {
        heap0 = actMngGuest_mDoExt_getHeap(1);
        heap1 = actMngGuest_mDoExt_getHeap(0);
    }
    /* which heap and size: 0 = the first heap, 1 = the second, 2 = a fixed size (special) */
    u32 heap = heap1, size = 0;
    bool special = false;
    if (name < 0x191) {
        if (name == 0x26 || name == 0xA5 || name == 0xA8)
            heap = heap0;
        else if (name == 0xC2)
            special = true;
    } else {
        if (name == 0x191)
            heap = heap0;
        else if (name < 0x1B5)
            heap = heap1;
        else if (name < 0x1B7)
            heap = heap0;
        else if (name == 0x1B7)
            heap = (u32)actorView->mParameters != 0 ? heap1 : heap0;
        else if (name == 0x1BD) {
            u32 prm = (u32)actorView->mParameters;
            if (prm == 1)
                size = 0x68CC;
            else if (prm == 0xB)
                size = 0x9ECC;
            else if (prm == 0xD)
                size = 0x6FA0;
            else
                special = true;
        }
    }
    BOOL ret;
    bool attemptedSpecialHeap = false;
    if (special) {
        u32 h = (u32)specialActorHeap;
        size = 0x4FA0;
        if (h != 0) {
            heap = h;
            ret = actMngGuest_fopAcM_entrySolidHeap_(i_this, createHeapCB, size, heap);
            if (ret != 0) return ret;
            attemptedSpecialHeap = true;
        }
        if (!attemptedSpecialHeap) heap = heap0;
    }
    if (!attemptedSpecialHeap) {
        if (heap == 0)
            actMngGuest_JUT_ASSERT(gabi::guest_addr("f_op_actor_mng.cpp"), 0x4F3,
                                   gabi::guest_addr("parentHeap != (0)"));
        ret = actMngGuest_fopAcM_entrySolidHeap_(i_this, createHeapCB, size, heap);
        if (ret != 0) return ret;
    }
    if (heap == heap1 || heap1 == 0) return ret;
    gabi::call_ptr(
        (u32)actorMngView<ActorManagerSafeStringVtable>(actorMngView<ActorManagerHeap>(heap)->mNameVtable)
            ->assureTermination,
        heap + 0x10);
    u32 n0 = (u32)actorMngView<ActorManagerHeap>(heap)->mName;
    gabi::call_ptr(
        (u32)actorMngView<ActorManagerSafeStringVtable>(actorMngView<ActorManagerHeap>(heap1)->mNameVtable)
            ->assureTermination,
        heap1 + 0x10);
    actorMngReportWarningHeaps(
        gabi::guest_addr(
            "%s "
            "\203q\201["
            "\203v\202\305\203G\203\223\203g\203\212\202\305\202\253\202\310\202\251\202\301\202\275\202\314 "
            "%s \203q\201[\203v\202\305\203\212\203g\203\211\203C\202\265\202\334\202\267\012"),
        n0, (u32)actorMngView<ActorManagerHeap>(heap1)->mName);
    return actMngGuest_fopAcM_entrySolidHeap_(i_this, createHeapCB, size, heap1);
}

/* fopAcM_SetMin (inline on GameCube) */
/* 025D672C */
static void fopAcM_SetMin(u32 i_this, f32 x, f32 y, f32 z) {
    fopAc_ac_c* actorView = actorMngActor(i_this);
    actorView->cull.box.min.y = y;
    actorView->cull.box.min.z = z;
    actorView->cull.box.min.x = x;
}

/* fopAcM_SetMax (inline on GameCube) */
/* 025D673C */
static void fopAcM_SetMax(u32 i_this, f32 x, f32 y, f32 z) {
    fopAc_ac_c* actorView = actorMngActor(i_this);
    actorView->cull.box.max.y = y;
    actorView->cull.box.max.z = z;
    actorView->cull.box.max.x = x;
}

/* 025D674C */
static void fopAcM_setCullSizeBoxRegister(u32 i_this, f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1) {
    fopAc_ac_c* actorView = actorMngActor(i_this);
    actorView->cull.box.min.x = x0;
    actorView->cull.box.min.y = y0;
    actorView->cull.box.min.z = z0;
    actorView->cull.box.max.x = x1;
    actorView->cull.box.max.y = y1;
    actorView->cull.box.max.z = z1;
}

/* 025D6768 */
static void fopAcM_setCullSizeSphere(u32 i_this, f32 x, f32 y, f32 z, f32 r) {
    fopAc_ac_c* actorView = actorMngActor(i_this);
    actorView->cull.box.min.x = x;
    actorView->cull.box.min.y = y;
    actorView->cull.box.min.z = z;
    actorView->cull.box.max.x = r;
}

/* 025D677C */
static BOOL fopAcM_addAngleY(u32 i_this, u32 target, u32 step) {
    return actMngGuest_cLib_chaseAngleS(i_this + 0x322, target, step) != 0;
}

/* 025D67A8 */
static void fopAcM_calcSpeed(u32 i_this) {
    fopAc_ac_c* actorView = actorMngActor(i_this);
    u32 t = gabi::ea(&sincosTable) + ((u32)((u16)actorView->current.angle.y >> 3) << 3);
    f32 speedF = (f32)actorView->speedF;
    f32 vy = gabi::fadds_ppc((f32)actorView->speed.y, (f32)actorView->gravity);
    f32 maxFall = (f32)actorView->maxFallSpeed;
    f32 vx = gabi::fmuls_ppc(speedF, (f32)(actorMngView<ActorManagerSinCos>(t)->sin));
    f32 vz = gabi::fmuls_ppc(speedF, (f32)(actorMngView<ActorManagerSinCos>(t)->cos));
    if (vy < maxFall) vy = maxFall;
    actorView->speed.y = vy;
    actorView->speed.z = vz;
    actorView->speed.x = vx;
}

/* 025D6800 */
static void fopAcM_posMoveRegister(u32 i_this, u32 i_movePos) {
    fopAc_ac_c* actorView = actorMngActor(i_this);
    f32 sy = (f32)actorView->speed.y, sx = (f32)actorView->speed.x;
    f32 py = (f32)actorView->current.pos.y, px = (f32)actorView->current.pos.x,
        pz = (f32)actorView->current.pos.z;
    f32 sz = (f32)actorView->speed.z;
    actorView->current.pos.y = gabi::fadds_ppc(py, sy);
    actorView->current.pos.x = gabi::fadds_ppc(px, sx);
    actorView->current.pos.z = gabi::fadds_ppc(pz, sz);
    if (i_movePos != 0) {
        u32 p = i_this + 0x314;
        f32 x = gabi::fadds_ppc((f32)actorMngView<cXyz>(p)->x, (f32)actorMngView<cXyz>(i_movePos)->x);
        f32 y = (f32)actorMngView<cXyz>(p)->y;
        actorMngView<cXyz>(p)->x = x;
        y = gabi::fadds_ppc(y, (f32)(actorMngView<cXyz>(i_movePos)->y));
        f32 z = (f32)actorMngView<cXyz>(p)->z;
        actorMngView<cXyz>(p)->y = y;
        z = gabi::fadds_ppc(z, (f32)(actorMngView<cXyz>(i_movePos)->z));
        actorMngView<cXyz>(p)->z = z;
    }
}

/* 025D6870 */
static void fopAcM_posMoveFRegister(u32 i_this, u32 i_movePos) {
    fopAcM_calcSpeed(i_this);
    fopAcM_posMoveRegister(i_this, i_movePos);
}

/* 025D6894 */
static s16 fopAcM_searchActorAngleYRegister(u32 i_this, u32 i_other) {
    return actMngGuest_cLib_targetAngleY(i_this + 0x314, i_other + 0x314);
}

/* 025D68A0 */
static s32 fopAcM_seenActorAngleY(u32 i_this, u32 i_other) {
    s16 target = actMngGuest_cLib_targetAngleY(i_this + 0x314, i_other + 0x314);
    s16 d = (s16)(target - (s16)(u16)actorMngActor(i_this)->shape_angle.y);
    return d < 0 ? -(s32)d : (s32)d;
}

/* 025D68EC */
static f32 fopAcM_searchActorDistanceRegister(u32 i_this, u32 i_other) {
    gabi::Local<cXyz> d;
    actMngGuest_cXyz_mi(i_other + 0x314, gabi::ea(d.get()), i_this + 0x314);
    return std_sqrtf(actMngGuest_PSVECSquareMag(gabi::ea(d.get())));
}

/* 025D6924 */
static f32 fopAcM_searchActorDistance2Register(u32 i_this, u32 i_other) {
    gabi::Local<cXyz> d;
    actMngGuest_cXyz_mi(i_other + 0x314, gabi::ea(d.get()), i_this + 0x314);
    return actMngGuest_PSVECSquareMag(gabi::ea(d.get()));
}

/* 025D6958 */
static f32 fopAcM_searchActorDistanceXZRegister(u32 i_this, u32 i_other) {
    gabi::Local<cXyz> xz, d;
    actMngGuest_cXyz_mi(i_other + 0x314, gabi::ea(d.get()), i_this + 0x314);
    xz->x = (f32)d->x;
    xz->y = 0.0f;
    xz->z = (f32)d->z;
    return std_sqrtf(actMngGuest_PSVECSquareMag(gabi::ea(xz.get())));
}

/* 025D69AC */
static f32 fopAcM_searchActorDistanceXZ2Register(u32 i_this, u32 i_other) {
    gabi::Local<cXyz> xz, d;
    actMngGuest_cXyz_mi(i_other + 0x314, gabi::ea(d.get()), i_this + 0x314);
    xz->x = (f32)d->x;
    xz->y = 0.0f;
    xz->z = (f32)d->z;
    return actMngGuest_PSVECSquareMag(gabi::ea(xz.get()));
}

// HD: onFrollCrashFlg is a virtual of the player (vtable +0xF4)
/* 025D69FC */
static BOOL fopAcM_rollPlayerCrash(u32 i_this, f32 distAdjust, u32 flag) {
    f32 maxDist = gabi::fadds_ppc(distAdjust, 40.0f);
    f32 xzDist2 = fopAcM_searchActorDistanceXZ2Register(i_this, player());
    u32 pl = player();
    f32 mm = gabi::fmuls_ppc(maxDist, maxDist);
    f32 yDist =
        gabi::fsubs_ppc((f32)actorMngActor(pl)->current.pos.y, (f32)actorMngActor(i_this)->current.pos.y);
    if (!(xzDist2 < mm)) return FALSE;
    if (!(yDist > -100.0f)) return FALSE;
    if (!(yDist < 200.0f)) return FALSE;
    u32 py = player();
    u32 other = player();
    s16 angle = fopAcM_searchActorAngleYRegister(i_this, other);
    u32 t = gabi::ea(&sincosTable) +
            ((u32)((u16)((s16)(u16)actorMngActor(py)->current.angle.y - angle) >> 3) << 3);
    if (!((f32)(actorMngView<ActorManagerSinCos>(t)->cos) < -0.9f)) return FALSE;
    if (py == 0) return FALSE;
    if ((s16)(u16)actorMngActor(py)->mProcName != fpcNm_PLAYER_e) return FALSE; /* fpcNm_PLAYER */
    gabi::call_ptr((u32)actorMngView<ActorManagerPlayerVtable>(actorMngActor(py)->__vtbl)->onFrollCrashFlg,
                   py, flag);
    return TRUE;
}

/* the second culling test: a pointer to member function at obj+off, on the world-space bounds */
static inline u32 pmf_call(u32 obj, u32 off, u32 arg) {
    auto* member = actorMngView<gabi::ptmf<u32 (ActorManagerCullObject::*)(u32, u32)>>(obj + off);
    s16 vidx = member->i;
    s16 delta = member->d;
    u32 self = gabi::ea(&actorMngView<ActorManagerSecondView>(obj)->mCullObject) + (s32)delta;
    u32 target;
    if (vidx < 0) {
        target = member->f;
    } else {
        s16 vtableOffset = actorMngView<be<s16>>(gabi::ea(&member->f))[1];
        u32 vtable = *actorMngView<be<u32>>(self + (s32)vtableOffset);
        target = actorMngView<ActorManagerVirtualEntry>(vtable)[vidx].mFunction;
    }
    return gabi::call_ptr<u32>(target, self, arg, 0);
}
static inline u32 second_view() {
    return gabi::ea(viewRoot->mpSecondView->get());
}

// HD: also tests the second view
/* 025D6B70 */
static BOOL fopAcM_checkCullingBox(u32 m, f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1) {
    gabi::Local<cXyz> p0, p1;
    gabi::Local<ActorManagerMatrix> mtx;
    p1->z = z1;
    p0->y = y0;
    p0->z = z0;
    p1->y = y1;
    p0->x = x0;
    p1->x = x1;
    actMngGuest_PSMTXConcat(gabi::ea(&viewMtx), m, gabi::ea(mtx.get()));
    if (actMngGuest_J3DUClipper_clipBox(gabi::ea(&clipper), gabi::ea(mtx.get()), gabi::ea(p1.get()),
                                        gabi::ea(p0.get())) == 0)
        return FALSE;
    gabi::Local<ActorManagerCullBox> box, box2;
    for (int i = 0; i < 3; i++)
        actorMngBits(box->v[i]) = actorMngView<be<u32>>(gabi::ea(p0.get()))[i];
    for (int i = 0; i < 3; i++)
        actorMngBits(box->v[3 + i]) = actorMngView<be<u32>>(gabi::ea(p1.get()))[i];
    actMngGuest_box_init(gabi::ea(box.get()));
    actMngGuest_box_transform(gabi::ea(box.get()), gabi::ea(box.get()), m);
    u32 obj = second_view();
    for (int i = 0; i < 6; i++)
        box2->v[i] = (f32)box->v[i];
    return pmf_call(obj, 0x7DC, gabi::ea(box2.get())) == 0;
}

/* the box branch of fopAcM_cullingCheck: max/min of the J3D clip box */
static BOOL cull_box(u32 pMtx, u32 mtx, f32 cullFar, f32 sizeFar, u32 max, u32 min) {
    BOOL r;
    if (sizeFar > 0.0f) {
        clipper.mFar = gabi::fmuls_ppc((f32)(systemFar), cullFar);
        actMngGuest_J3DUClipper_calcViewFrustum(gabi::ea(&clipper));
        r = actMngGuest_J3DUClipper_clipBox(gabi::ea(&clipper), pMtx, max, min);
        clipper.mFar = (f32)(systemFar);
        actMngGuest_J3DUClipper_calcViewFrustum(gabi::ea(&clipper));
    } else {
        r = actMngGuest_J3DUClipper_clipBox(gabi::ea(&clipper), pMtx, max, min);
    }
    if (r == 0) return FALSE;
    gabi::Local<ActorManagerCullBox> box2;
    gabi::Local<ActorManagerCullBox> box;
    u32 b = gabi::ea(box.get());
    for (int i = 0; i < 3; i++)
        actorMngBits(box->v[i]) = actorMngView<be<u32>>(min)[i];
    for (int i = 0; i < 3; i++)
        actorMngBits(box->v[3 + i]) = actorMngView<be<u32>>(max)[i];
    actMngGuest_box_init(b);
    if (mtx != 0) actMngGuest_box_transform(b, b, mtx);
    u32 obj = second_view();
    for (int i = 0; i < 6; i++)
        box2->v[i] = (f32)box->v[i];
    return pmf_call(obj, 0x7DC, gabi::ea(box2.get())) == 0;
}

/* the sphere branch: center/radius of the J3D clip, then the second view with the actor's radius */
static BOOL cull_sphere(u32 i_this, u32 pMtx, u32 mtx, f32 cullFar, f32 sizeFar, u32 center, u32 radius) {
    BOOL r;
    gabi::Local<cXyz> c;
    if (sizeFar > 0.0f) {
        clipper.mFar = gabi::fmuls_ppc((f32)(systemFar), cullFar);
        actMngGuest_J3DUClipper_calcViewFrustum(gabi::ea(&clipper));
        for (int i = 0; i < 3; i++)
            actorMngView<be<u32>>(gabi::ea(c.get()))[i] = actorMngView<be<u32>>(center)[i];
        r = actMngGuest_J3DUClipper_clipSphere(gabi::ea(&clipper), pMtx, gabi::ea(c.get()),
                                               (f32)*actorMngView<be<f32>>(radius));
        clipper.mFar = (f32)(systemFar);
        actMngGuest_J3DUClipper_calcViewFrustum(gabi::ea(&clipper));
    } else {
        for (int i = 0; i < 3; i++)
            actorMngView<be<u32>>(gabi::ea(c.get()))[i] = actorMngView<be<u32>>(center)[i];
        r = actMngGuest_J3DUClipper_clipSphere(gabi::ea(&clipper), pMtx, gabi::ea(c.get()),
                                               (f32)*actorMngView<be<f32>>(radius));
    }
    if (r == 0) return FALSE;
    gabi::Local<cXyz> w;
    gabi::Local<ActorManagerCullSphere> sph;
    f32 z = (f32)(actorMngView<cXyz>(center)->z), y = (f32)(actorMngView<cXyz>(center)->y),
        x = (f32)(actorMngView<cXyz>(center)->x);
    w->y = y;
    w->x = x;
    w->z = z;
    if (mtx != 0) {
        actMngGuest_PSMTXMultVec(mtx, gabi::ea(w.get()), gabi::ea(w.get()));
        y = w->y;
        x = w->x;
        z = w->z;
    }
    u32 obj = second_view();
    f32 rr = (f32)actorMngActor(i_this)
                 ->cull.box.max.x; // HD: the actor's custom radius, also for the table spheres
    sph->v[0] = x;
    sph->v[1] = y;
    sph->v[2] = z;
    sph->v[3] = rr;
    return pmf_call(obj, 0x7E4, gabi::ea(sph.get())) == 0;
}

/* 025D6CE8 */
static BOOL fopAcM_cullingCheck(u32 i_this) {
    fopAc_ac_c* actorView = actorMngActor(i_this);
    u32 mtx = (u32)actorView->cullMtx;
    gabi::Local<ActorManagerMatrix> tmp;
    u32 pMtx;
    if (mtx == 0) {
        pMtx = gabi::ea(&viewMtx);
    } else {
        actMngGuest_PSMTXConcat(gabi::ea(&viewMtx), mtx, gabi::ea(tmp.get()));
        pMtx = gabi::ea(tmp.get());
    }
    f32 cullFar = (f32)actorView->cullSizeFar;
    if ((u8)(actorMngView<dComIfG_inf_c>(dComIfGp_ea())->play.mEvtCtrl.mMode) != 0)
        cullFar =
            gabi::fmuls_ppc(cullFar, (f32)(actorMngView<dComIfG_inf_c>(dComIfGp_ea())
                                               ->play.mEvtCtrl.mCullFarClipRatio)); /* event cull rate */
    u8 cull = (u8)actorView->cullType;
    f32 sizeFar = (f32)actorView->cullSizeFar;
    if (cull <= fopAc_CULLBOX_CUSTOM_e) {
        if (cull == fopAc_CULLBOX_CUSTOM_e)
            return cull_box(pMtx, mtx, cullFar, sizeFar, i_this + 0x358, i_this + 0x34C);
        u32 box = gabi::ea(&l_cullSizeBox.entries[cull]);
        return cull_box(pMtx, mtx, cullFar, sizeFar, box + 0xC, box);
    }
    if (cull == fopAc_CULLSPHERE_CUSTOM_e)
        return cull_sphere(i_this, pMtx, mtx, cullFar, sizeFar, i_this + 0x34C, i_this + 0x358);
    u32 sph = gabi::ea(&l_cullSizeSphere.entries[(s32)cull - fopAc_CULLSPHERE_0_e]);
    return cull_sphere(i_this, pMtx, mtx, cullFar, sizeFar, sph, sph + 0xC);
}

/* ---- event orders ---- */
static inline s32 order(u32 type, u32 prio, u32 flag, u32 hind, u32 a1, u32 a2) {
    return actMngGuest_dEvt_order(dComIfGp_ea() + 0x51D0, type, prio, flag, hind, a1, a2, (u32)-1, 0xFF);
}

/* 025D744C */
static s32 fopAcM_orderTalkEvent(u32 i_this, u32 i_partner) {
    return order(actorEvent_TALK, 0x1FF, 0, 0x14F, i_this, i_partner);
}

/* 025D74B0 */
static s32 fopAcM_orderTalkXBtnEvent(u32 i_this, u32 i_partner) {
    return order(actorEvent_SHOWITEM_X, 0x1F4, 0, 0x14F, i_this, i_partner);
}

/* 025D7514 */
static s32 fopAcM_orderTalkYBtnEvent(u32 i_this, u32 i_partner) {
    return order(actorEvent_SHOWITEM_Y, 0x1F4, 0, 0x14F, i_this, i_partner);
}

/* 025D7578 */
static s32 fopAcM_orderTalkZBtnEvent(u32 i_this, u32 i_partner) {
    return order(actorEvent_SHOWITEM_Z, 0x1F4, 0, 0x14F, i_this, i_partner);
}

// HD-only: the same for the new event type 9 (probably a fourth item button)
/* 025D75DC */
static s32 fopAcM_orderTalkBtn9Event(u32 i_this, u32 i_partner) {
    return order(actorEvent_SHOWITEM_HD, 0x1F4, 0, 0x14F, i_this, i_partner);
}

/* 025D7640 */
static s32 fopAcM_orderZHintEvent(u32 i_this, u32 i_partner) {
    return order(actorEvent_TALK, 0x1FF, 0, 0xFFFF, i_this, i_partner);
}

/* 025D76A8 */
static s32 fopAcM_orderSpeakEventRegister(u32 i_this) {
    u32 pl = player();
    return order(actorEvent_TALK, 0x1EA, 0, 0x14F, pl, i_this);
}

/* 025D7710 */
static s32 fopAcM_orderDoorEvent(u32 i_this, u32 i_partner) {
    return order(actorEvent_DOOR, 0xFF, 0, 0xFFFF, i_this, i_partner);
}

/* 025D7774 */
static s32 fopAcM_orderCatchEvent(u32 i_this, u32 i_partner) {
    return order(actorEvent_CATCH, 1, 0, 0xFFFF, i_this, i_partner);
}

static inline u32 event_prio(u32 idx) {
    u32 prio = actMngGuest_dEvmng_getEventPrio(dComIfGp_ea() + 0x52C4, idx) & 0xFFFF;
    if (prio == 0) prio = 0xFF;
    return prio;
}

/* 025D77DC */
static s32 fopAcM_orderOtherEvent2(u32 i_this, u32 pEventName, u32 flag, u32 hind) {
    u32 idx = actMngGuest_dEvmng_getEventIdx(dComIfGp_ea() + 0x52C4, pEventName, 0xFF);
    u32 prio = event_prio(idx);
    u32 pl = player();
    return actMngGuest_dEvt_orderOld(dComIfGp_ea() + 0x51D0, actorEvent_OTHER, prio, flag, hind, i_this, pl,
                                     pEventName);
}

/* 025D7874 */
static s32 fopAcM_orderChangeEventId(u32 i_this, u32 eventIdx, u32 flag, u32 hind) {
    u32 prio = event_prio(eventIdx);
    u32 pl = player();
    return actMngGuest_dEvt_order(dComIfGp_ea() + 0x51D0, actorEvent_CHANGE, prio, flag, hind, i_this, pl,
                                  eventIdx, 0xFF);
}

/* 025D78FC */
static s32 fopAcM_orderChangeEvent(u32 i_this, u32 pEventName, u32 flag, u32 hind) {
    u32 idx = actMngGuest_dEvmng_getEventIdx(dComIfGp_ea() + 0x52C4, pEventName, 0xFF);
    return fopAcM_orderChangeEventId(i_this, idx, flag, hind);
}

/* 025D7970 */
static s32 fopAcM_orderChangeEventId2(u32 i_this, u32 i_partner, u32 eventIdx, u32 flag, u32 hind) {
    u32 prio = event_prio(eventIdx);
    return actMngGuest_dEvt_order(dComIfGp_ea() + 0x51D0, actorEvent_CHANGE, prio, flag, hind, i_this,
                                  i_partner, eventIdx, 0xFF);
}

/* 025D79F4 */
static s32 fopAcM_orderChangeEvent2(u32 i_this, u32 i_partner, u32 pEventName, u32 flag, u32 hind) {
    u32 idx = actMngGuest_dEvmng_getEventIdx(dComIfGp_ea() + 0x52C4, pEventName, 0xFF);
    return fopAcM_orderChangeEventId2(i_this, i_partner, idx, flag, hind);
}

/* 025D7A58 */
static s32 fopAcM_orderOtherEventIdRegister(u32 i_this, u32 eventIdx, u32 infoIdx, u32 hind, u32 priority,
                                            u32 flag) {
    u32 prio = priority;
    if (priority == 0) prio = event_prio(eventIdx);
    u32 pl = player();
    return actMngGuest_dEvt_order(dComIfGp_ea() + 0x51D0, actorEvent_OTHER, prio, flag, hind, i_this, pl,
                                  eventIdx, infoIdx);
}

/* 025D7B24 */
static s32 fopAcM_orderPotentialEvent(u32 i_this, u32 flag, u32 hind, u32 priority) {
    if (priority == 0) priority = 0xFF;
    u32 pl = player();
    return order(actorEvent_POTENTIAL, priority, flag, hind, i_this, pl);
}

// HD: the hind flag is an argument (GameCube 0xFFFF)
/* 025D7B98 */
static s32 fopAcM_orderItemEvent(u32 i_this, u32 hind) {
    u32 pl = player();
    return order(actorEvent_ITEM, 0xFF, 0, hind, pl, i_this);
}

/* 025D7C08 */
static s32 fopAcM_orderTreasureEvent(u32 i_this, u32 i_partner) {
    return order(actorEvent_TREASURE, 0xFF, 0, 0xFFFF, i_this, i_partner);
}

/* 025D7C6C */
static u32 fopAcM_getTalkEventPartner(u32 i_this) {
    u32 play = dComIfGp_ea();
    return actMngGuest_dEvt_convPId(play + 0x51D0,
                                    (u32)(actorMngView<dComIfG_inf_c>(play)->play.mEvtCtrl.mPtTalk));
}

/* 025D7C98 */
static u32 fopAcM_getItemEventPartner(u32 i_this) {
    u32 play = dComIfGp_ea();
    return actMngGuest_dEvt_convPId(play + 0x51D0,
                                    (u32)(actorMngView<dComIfG_inf_c>(play)->play.mEvtCtrl.mPtItem));
}

/* 025D7CC4 */
static u32 fopAcM_getEventPartner(u32 i_this) {
    u32 play = dComIfGp_ea();
    u32 pt1 =
        actMngGuest_dEvt_convPId(play + 0x51D0, (u32)(actorMngView<dComIfG_inf_c>(play)->play.mEvtCtrl.mPt1));
    u32 evt = dComIfGp_ea() + 0x51D0;
    if (pt1 != i_this) return actMngGuest_dEvt_convPId(evt, (u32)(actorMngView<dEvt_control_c>(evt)->mPt1));
    return actMngGuest_dEvt_convPId(evt, (u32)(actorMngView<dEvt_control_c>(evt)->mPt2));
}

/* WWHD glue
str 10057434 in 025D63E8
str 10057438 in 025D63E8
str 1005744C in 025D63E8
str 10057460 in 025D63E8
*/
