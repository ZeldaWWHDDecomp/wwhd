/* NPC path-following and runaway utility. Based on zeldaret/tww d/d_a_npc_mk_static.cpp,
 * verified against the HD executable. */
#include "d/actor/d_a_npc_mk_static.h"
#include "gc_api/gc_api.h"
#include "gc_api/d/d_npc.h"
#include <cstdlib>
static MkHeaderStatics l_headerStatics = {};

static cXyz *mk_pos(fopAc_ac_c *a) {
    return &a->current.pos;
}
static void mk_floatCopy(cXyz *d, const cXyz *s) {
    f32 x = s->x, y = s->y, z = s->z;
    d->x = x;
    d->y = y;
    d->z = z;
}
// HD: Mk/Uk retain the legacy path type; its layout is identical to the shared path.
static dNpc_PathRun_c* mk_path(MkPathRun* p) {
    return reinterpret_cast<dNpc_PathRun_c*>(p);
}
static void mk_point(MkPathRun *p, cXyz *out, u8 index) {
    mk_path(p)->getPoint(out, index);
}
static void mk_inc(MkPathRun *p) { mk_path(p)->incIdxLoop(); }
static void mk_dec(MkPathRun *p) { mk_path(p)->decIdxLoop(); }
static void mk_info(MkPathRun *p, void *path) {
    mk_path(p)->setInfDrct(static_cast<dPath*>(path));
}
static s16 mk_angle(cXyz *a, cXyz *b) {
    return Mk_targetAngle(a, b);
}
static f32 mk_distance(cXyz *a, cXyz *b) {
    gabi::Local<cXyz> diff, flat;
    Mk_subtract(a, diff.get(), b);
    flat->x = diff->x;
    flat->y = 0.0f;
    flat->z = diff->z;
    f32 sq = Mk_squareMagnitude(flat.get());
    return Mk_sqrt(sq);
}
static bool mk_segment(fopAc_ac_c *player, cXyz *a, cXyz *b) {
    gabi::Local<cXyz> hit;
    gabi::Local<be<f32>> distance;
    f32 ax = a->x, az = a->z, bx = b->x, bz = b->z;
    f32 px = mk_pos(player)->x, pz = mk_pos(player)->z;
    s32 ok = Mk_segmentDistance(px, pz, ax, az, bx, bz, &hit->x, &hit->z,
                                                      distance.get());
    return ok && (f32)*distance < 10000.0f;
}
static fopAc_ac_c *mk_player() {
    return dComIfGp_getLinkPlayer();
}

/* 0229A8EC */
u32 daNpc_Mk_Static_c::turnPath(fopAc_ac_c *actor, MkPathRun *path, u8 options) {
    gabi::Local<cXyz> point, pos;
    gabi::Local<be<s16>> angle;
    mk_point(path, point.get(), path->mIdx);
    mk_floatCopy(pos.get(), mk_pos(actor));
    // HD: getPoint's return is copied through floating point registers.
    gabi::Local<cXyz> target;
    mk_floatCopy(target.get(), point.get());
    dNpc_calc_DisXZ_AngY(pos.get(), target.get(), nullptr, angle.get());
    be<s16> *yaw = &actor->current.angle.y;
    if (options & 2)
        Mk_addAngle(yaw, (s16)*angle, 2, 0x2000, 0x400);
    else
        Mk_addAngle(yaw, (s16)*angle, 8, 0x800, 0x200);
    f32 cosine = cM_scos((s16)*yaw - (s16)*angle);
    if (cosine < 0.0f)
        m4 = gabi::fadds_ppc(cosine, 1.0f);
    return 0;
}

/* 0229AA38 */
BOOL daNpc_Mk_Static_c::chkPath(fopAc_ac_c *actor, MkPathRun *path, u8 options) {
    gabi::Local<cXyz> pos;
    mk_floatCopy(pos.get(), mk_pos(actor));
    if (!mk_path(path)->chkPointPass(pos.get(), (options & 1) ^ 1))
        return FALSE;
    if (options & 1)
        mk_dec(path);
    else
        mk_inc(path);
    mPointIndex2 = mPointIndex1;
    mPointIndex1 = path->mIdx;
    return TRUE;
}
/* 0229AB24 */
BOOL daNpc_Mk_Static_c::walkPath(fopAc_ac_c *actor, MkPathRun *path,
                                                                  u8 options) {
    BOOL result = chkPath(actor, path, options);
    turnPath(actor, path, options);
    return result;
}
/* 0229AB78 */
void daNpc_Mk_Static_c::aroundWalk(fopAc_ac_c *actor, fopAc_ac_c *target,
                                                                      u8 options) {
    gabi::Local<cXyz> a, b;
    gabi::Local<be<s16>> angle;
    mk_floatCopy(a.get(), mk_pos(actor));
    mk_floatCopy(b.get(), mk_pos(target));
    dNpc_calc_DisXZ_AngY(a.get(), b.get(), nullptr, angle.get());
    if (options) {
        *angle = (s16)((s16)*angle + 0x3000);
        Mk_addAngle(&actor->current.angle.y,
                              (s16)*angle, 2, 0x800, 0x400);
    } else {
        f32 distance = mk_distance(mk_pos(actor), mk_pos(target));
        int offset =
                distance > 150.0f ? 0x3400 : (distance < 120.0f ? 0x4000 : 0x3A00);
        *angle = (s16)((s16)*angle + offset);
        Mk_addAngle(&actor->current.angle.y,
                              (s16)*angle, 8, 0x800, 0x200);
    }
}
/* 0229ACC0 */
f32 daNpc_Mk_Static_c::getSpeedF(f32 normal, f32 running) {
    f32 speed = normal;
    if (md) {
        if ((u16)m8 >= ((u16)ma >> 1))
            speed = running;
        else {
            f32 delta = gabi::fsubs_ppc(running, normal);
            f32 factor = 2.0f / (f32)(u16)ma;
            speed =
                    gabi::fmadds(gabi::fmuls_ppc(delta, factor), (f32)(u16)m8, normal);
        }
    }
    f32 result = gabi::fmuls_ppc(speed, (f32)m4);
    m4 = 1.0f;
    return result;
}
/* 0229AD60 */
void daNpc_Mk_Static_c::init(u8 wait, u16 duration) {
    m0 = 0;
    me = wait;
    m8 = duration;
    ma = duration;
    md = 0;
    m4 = 1.0f;
}
/* 0229AD88 */
void daNpc_Mk_Static_c::runaway_com2(MkPathRun *path, u8 direction) {
    path->mIdx = mPointIndex2;
    if (direction == 1)
        mk_inc(path);
    else
        mk_dec(path);
    mPointIndex1 = path->mIdx;
}
/* 0229ADEC */
u8 daNpc_Mk_Static_c::goFarLink_2(fopAc_ac_c *actor, MkPathRun *source) {
    fopAc_ac_c *player = mk_player();
    gabi::Local<MkPathRun> path;
    void *resource = source->mPath;
    mk_info(path.get(), resource);
    gabi::Local<cXyz> origin, next, prev;
    path->mIdx = mPointIndex2;
    mk_point(path.get(), origin.get(), path->mIdx);
    mk_inc(path.get());
    mk_point(path.get(), next.get(), path->mIdx);
    mk_dec(path.get());
    mk_dec(path.get());
    mk_point(path.get(), prev.get(), path->mIdx);
    s16 playerAngle = mk_angle(origin.get(), mk_pos(player));
    s16 nextAngle = mk_angle(origin.get(), next.get());
    s16 prevAngle = mk_angle(origin.get(), prev.get());
    int dn = (int)nextAngle - playerAngle, dp = (int)prevAngle - playerAngle;
    if (std::abs(dn) < 0x1800 && mk_segment(player, origin.get(), next.get()))
        return 2;
    if (std::abs(dp) < 0x1800 && mk_segment(player, origin.get(), prev.get()))
        return 1;
    u8 old = mPointIndex2;
    if (mk_path(path.get())->setNearPathIndxMk2(mk_pos(player), old, 5)) {
        int delta = (int)(u8)path->mIdx - (u8)mPointIndex2;
        return ((delta > 0) == (std::abs(delta) <= 5)) ? 2 : 1;
    }
    return std::abs(dn) < std::abs(dp) ? 2 : 1;
}
/* 0229B010 */
u8 daNpc_Mk_Static_c::goFarLink_3(fopAc_ac_c *actor, MkPathRun *source) {
    fopAc_ac_c *player = mk_player();
    gabi::Local<MkPathRun> path;
    void *resource = source->mPath;
    mk_info(path.get(), resource);
    u8 old = mPointIndex2;
    if (old == (u8)mPointIndex1)
        return m0;
    gabi::Local<cXyz> a, b;
    mk_point(path.get(), a.get(), old);
    mk_point(path.get(), b.get(), mPointIndex1);
    s16 playerAngle = mk_angle(mk_pos(actor), mk_pos(player));
    s16 pathAngle = mk_angle(a.get(), b.get());
    u8 current = m0;
    if ((current == 1 || current == 2) &&
            std::abs((int)pathAngle - playerAngle) < 0x1800) {
        if (mk_segment(player, a.get(), b.get()))
            return (u8)m0 == 1 ? 2 : 1;
        return m0;
    }
    return current;
}
/* 0229B164 */
u8 daNpc_Mk_Static_c::runAwayProc(fopAc_ac_c *actor, MkPathRun *path,
                                                                    void *cylinder, be<s16> *outAngle) {
    fopAc_ac_c *player = mk_player();
    fopAc_ac_c *hit = static_cast<dCcD_Cyl*>(cylinder)->GetCoHitAc();
    if (hit == player) {
        *outAngle = mk_angle(mk_pos(player), mk_pos(actor));
        return 4;
    }
    u8 wait = me;
    if (wait)
        me = wait - 1;
    u8 current = m0;
    switch (current) {
    case 0: {
        f32 distance = mk_path(path)->setNearPathIndxMk(mk_pos(actor));
        u8 index = path->mIdx;
        mPointIndex1 = index;
        mPointIndex2 = index;
        return distance < 50.0f ? 3 : 5;
    }
    case 5: {
        gabi::Local<cXyz> point, copy;
        mk_point(path, point.get(), mPointIndex1);
        copy->copy(*point);
        if (mk_distance(copy.get(), mk_pos(actor)) < 20.0f)
            return 3;
        turnPath(actor, path, 2);
        return m0;
    }
    case 3: {
        f32 distance = mk_distance(mk_pos(player), mk_pos(actor));
        if (distance < 800.0f) {
            me = 10;
            u8 direction = goFarLink_2(actor, path);
            runaway_com2(path, direction);
            return direction;
        }
        u16 time = m8;
        if ((int)time + 3 < (u16)ma) {
            u8 result = m0;
            m8 = time + 3;
            return result;
        }
        return m0;
    }
    case 1:
    case 2: {
        f32 distance = mk_distance(mk_pos(player), mk_pos(actor));
        u8 option = (u8)m0 == 1 ? 2 : 3;
        if (walkPath(actor, path, option)) {
            if (distance > 900.0f)
                return 3;
            u8 direction = goFarLink_2(actor, path);
            if (direction != (u8)m0)
                runaway_com2(path, direction);
            return direction;
        }
        if (!(u8)me) {
            me = 10;
            u8 direction = goFarLink_3(actor, path);
            if (direction != (u8)m0) {
                mPointIndex2 = mPointIndex1;
                runaway_com2(path, direction);
            }
            return direction;
        }
        if (md) {
            u16 time = m8;
            if (time)
                m8 = time - 1;
            if (distance > 400.0f) {
                u8 result = m0;
                md = 0;
                return result;
            }
        } else if (distance < 200.0f)
            md = 1;
        return m0;
    }
    default:
        return current;
    }
}
/* 0229B54C */
BOOL daNpc_Mk_Static_c::chkGameSet() {
    for (u16 flag : {(u16)0x20, (u16)0x10, (u16)8, (u16)4}) {
        if (!dComIfGs_isTmpBit(flag))
            return FALSE;
    }
    return TRUE;
}
/* 0229B5F0 */
void daNpc_Mk_Static_c::setRndPathPos(fopAc_ac_c *actor, MkPathRun *path) {
    gabi::Local<dBgS_GndChk> ground;
    // HD: retain the inlined SDK construction and per-TU vtable transitions.
    Mk_groundConstruct(ground.get());
    ground->mGrp = 1;
    ground->cBgS_Chk::__vtbl = 0x1001E554;
    ground->mbObjThrough = 0;
    ground->mbCamThrough = 0;
    ground->mbLinkThrough = 0;
    ground->mbArrowThrough = 0;
    ground->mbBombThrough = 0;
    ground->mbBoomerangThrough = 0;
    ground->mbRopeThrough = 0;
    ground->mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk*>(ground.get()));
    ground->mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk*>(ground.get()));
    if (path->mPath) {
        ground->dBgS_PolyPassChk::__vtbl = 0x1001E584;
        ground->cBgS_PolyInfo::__vtbl = 0x1001E564;
        ground->dBgS_GrpPassChk::__vtbl = 0x1001E574;
        // HD: forward maxPoint's complete return register rather than narrowing to u8.
        s32 max = Mk_pathMax(path);
        f32 random = Mk_random((f32)max);
        gabi::Local<cXyz> point;
        mk_point(path, point.get(), (u8)gabi::ftoi(random));
        f32 y = point->y;
        mk_pos(actor)->y = y;
        ground->mFlag = (u32)ground->mFlag & ~2u;
        f32 high = gabi::fadds_ppc(y, 200.0f), z = point->z, x = point->x;
        mk_pos(actor)->z = z;
        ground->m_pos.x = x;
        ground->m_pos.y = high;
        ground->m_pos.z = z;
        mk_pos(actor)->x = x;
        dBgS* bgs = dComIfG_Bgsp();
        f32 floor = bgs->GroundCross(ground.get());
        f32 finalX = mk_pos(actor)->x, finalZ = mk_pos(actor)->z;
        actor->old.pos.x = finalX;
        actor->old.pos.y = floor;
        actor->old.pos.z = finalZ;
        mk_pos(actor)->y = floor;
    }
    ground->cBgS_PolyInfo::__vtbl = 0x1001E564;
    ground->dBgS_PolyPassChk::__vtbl = 0x1001E584;
    ground->dBgS_GrpPassChk::__vtbl = 0x1001E544;
    Mk_groundDestruct(ground.get(), 0);
}
/* 0229B768 */
BOOL daNpc_Mk_Static_c::chkPointPass(cXyz *a, cXyz *b, cXyz *c) {
    f32 bx = b->x;
    if ((f32)a->x == bx && (f32)a->z == (f32)b->z)
        return TRUE;
    if ((f32)c->x == bx && (f32)c->z == (f32)b->z)
        return TRUE;
    s16 first = mk_angle(a, b);
    s16 second = mk_angle(c, b);
    return cM_scos((int)first - second) < 0.0f;
}
// HD: compiler-generated initialization of the common header SDK objects.
/* 0229B84C */
static void __sinit_d_a_npc_mk_static_cpp() {
    for (int i = 0; i < 4; i++) l_headerStatics.mZeroVector[i] = 0;
    __register_global_object(0x101C1FF8);
    l_headerStatics.mMinAngle = -3.1415927410125732f;
    l_headerStatics.mMaxAngle = 3.1415927410125732f;
    Mk_sdkByteA(&l_headerStatics.mSdkByteA);
    __register_global_object(0x101C2004);
    Mk_sdkByteB(&l_headerStatics.mSdkByteB);
    __register_global_object(0x101C2010);
}

/* WWHD glue (tools/verify/autoglue.py)
data l_headerStatics 10467C88 nocheck # BSS SDK header objects have no file initializer.
ext 0200F93C s16 Mk_targetAngle(cXyz*, cXyz*)
ext 0201ADE0 void Mk_subtract(cXyz*, cXyz*, cXyz*)
ext 028E8DD0 f32 Mk_squareMagnitude(cXyz*)
ext 028F4384 f32 Mk_sqrt(f32)
ext 020109FC s32 Mk_segmentDistance(f32, f32, f32, f32, f32, f32, be<f32>*, be<f32>*, be<f32>*)
ext 0200F378 void Mk_addAngle(be<s16>*, s16, s16, s16, s16)
ext 0259EDB8 s32 Mk_pathMax(MkPathRun*)
ext 020198D8 f32 Mk_random(f32)
ext 02008E0C void Mk_groundConstruct(dBgS_GndChk*)
ext 02008DAC void Mk_groundDestruct(dBgS_GndChk*, s32)
ext 028ED6F8 void Mk_sdkByteA(u8*)
ext 028EAB2C void Mk_sdkByteB(u8*)
*/
