/**
 * d_a_bgn3.cpp (WWHD)
 * Boss - Puppet Ganon (Phase 3: the worm)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww d_a_bgn3.cpp) and the WWHD code.
 */
#include "d/actor/d_a_bgn3.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include <bit>
#include <cmath>

const gc_tu_data gc_tu = {.safeString = 0x10008F7C};
static Bgn3BossView* bgn;
static fopAc_ac_c* esa;
static GXColor ke_color;
static daBgn3_HIO_c l_HIO;
static u8 hio_set;
static u32 ki_all_count;
static cXyz zero;
static Bgn3HeaderStatics headerStatics;
static Bgn3PiConstants piConstants;
static Bgn3LightInit defaultLightInit;

static f32 size_d[] = {1.0f, 0.9f, 0.8f, 0.7f, 0.6f, 0.5f, 0.4f, 0.3f, 0.6f};
static u16 ke_max[] = {40, 35, 30, 25, 20, 15, 15};
static s16 fl_check_d[] = {21, 19, 17, 15, 13, 11, 9, 7, 5};

void end(u32 a);
static f32 singleAdd(f32 a, f32 b);
void move_se_set(u32 a);
void move(u32 a);
f32 tableSin(u32 a, u32 angle);
f32 tableCos(u32 a, u32 angle);
u32 actorCtor(u32 a);

/* 020914F4 */
u32 hioCtor(u32 a) {
    if (!a)
        a = Bgn3Bindings::operator_new(0x58);
    if (!a)
        return 0;
    auto* hio = gabi::at<daBgn3_HIO_c>(a);
    hio->m1C = 1200;
    hio->m10 = 60.0f;
    hio->m05 = 0;
    hio->m08 = 50.0f;
    hio->m06 = 0;
    hio->mNo = -1;
    hio->m16 = 3;
    hio->m0E = 3;
    hio->m14 = 1000;
    hio->m1E = 3;
    hio->m0C = 800;
    hio->__vtbl = 0x10008FF4;
    hio->m22 = 40;
    hio->m26 = 10;
    hio->m20 = 60;
    hio->m2A = 45;
    hio->m2C = 120;
    hio->m28 = 20;
    hio->m24 = 30;
    hio->m18 = 80.0f;
    hio->m3E = 1;
    hio->m34 = 5.0f;
    hio->m30 = 250;
    hio->m4C = -8.0f;
    hio->m38 = 0.5f;
    hio->m44 = 45.0f;
    hio->m2E = 200;
    hio->m52 = 152;
    hio->m40 = 150.0f;
    hio->m50 = 188;
    hio->m3C = 3;
    hio->m48 = 8.0f;
    hio->m54 = 66;
    return a;
}

/* 0208FC14 */
u32 bgn_s_sub(u32 a, u32 ignored) {
    auto* actor = gabi::at<fopAc_ac_c>(a);
    if (fopAc_IsActor(actor) && a && fpcM_GetName(actor) == fpcNm_BGN_e)
        return a;
    return 0;
}

/* 0208FC64 */
bool daBgn3_Draw(u32 a) {
    auto* i_this = gabi::at<bgn3_class>(a);
    u32 boss = Bgn3Bindings::fpcM_Search(gabi::guest_fn((const void*)&bgn_s_sub), a);
    bgn = gabi::at<Bgn3BossView>(boss);
    if (boss && static_cast<s8>(gabi::at<Bgn3BossView>(boss)->m02B5) == 2 &&
        static_cast<u8>(l_HIO.m3E)) {
        ke_color.r = 188;
        ke_color.g = 152;
        ke_color.b = 66;
        ke_color.a = 255;
        Bgn3Bindings::mDoExt_3DlineMat0_update(a + 0x19D8, 5, gabi::ea(&ke_color), 3, a + 0x110,
                                               static_cast<f32>(l_HIO.m48));
        u32 play = dComIfGp_ea();
        u32 index = bgn3LineMaterialID(&i_this->mLineMat);
        Bgn3Bindings::mDoExt_3DlineMatSortPacket_setMat(play + 0x5FB4 + index * 0x9C, a + 0x19D8);
        for (int i = 0; i < 6; ++i) {
            u32 part = a + 0x1BE8 + 0x19D8 * i;
            Bgn3Bindings::mDoExt_3DlineMat0_update(part + 0x1890, 5, gabi::ea(&ke_color), 3,
                                                   a + 0x110, static_cast<f32>(l_HIO.m48));
            play = dComIfGp_ea();
            index = bgn3LineMaterialID(&bgn3Part(part)->mPart3LineMat);
            Bgn3Bindings::mDoExt_3DlineMatSortPacket_setMat(play + 0x5FB4 + index * 0x9C,
                                                            part + 0x1890);
        }
    }
    return true;
}

/* 0208FD9C */
bool gr_check(u32 a, u32 pos) {
    auto* position = gabi::at<cXyz>(pos);
    Bgn3LinChk line;
    cXyz start;
    cXyz end;
    Bgn3Bindings::cBgS_LinChk_ct(gabi::ea(&line));
    f32 x = static_cast<f32>(position->x), y = static_cast<f32>(position->y),
        z = static_cast<f32>(position->z);
    line.mGrp = 1;
    line.mPass[1] = 0;
    line.mPass[6] = 0;
    line.mPass[4] = 0;
    line.__vtbl_64 = 0x10008FD4;
    line.__vtbl_20 = 0x10008FC4;
    line.mPass[5] = 0;
    start.z = z;
    end.y = y - 1000.0f;
    start.x = x;
    line.mpPolyPassChk = gabi::ea(&line) + 0x58;
    end.z = z;
    line.__vtbl_10 = 0x10008FB4;
    line.mPass[3] = 0;
    line.mpGrpPassChk = gabi::ea(&line) + 0x64;
    end.x = x;
    line.mPass[0] = 0;
    line.mPass[2] = 0;
    line.__vtbl_58 = 0x10008FE4;
    start.y = y + 200.0f;
    Bgn3Bindings::dBgS_LinChk_Set(gabi::ea(&line), gabi::ea(&start), gabi::ea(&end), a);
    s32 hit = Bgn3Bindings::cBgS_LineCross(dComIfGp_ea() + 0x12A0, gabi::ea(&line));
    bool water = false;
    if (hit) {
        u32 zc = static_cast<u32>(bgn3Word(line.mCross.z)),
            xc = static_cast<u32>(bgn3Word(line.mCross.x)),
            yc = static_cast<u32>(bgn3Word(line.mCross.y));
        bgn3Word(position->x) = xc;
        bgn3Word(position->y) = yc;
        bgn3Word(position->z) = zc;
        position->y = REG_F(0, 8) - 2.0f;
        water = Bgn3Bindings::dBgS_GetAttributeCode(dComIfGp_ea() + 0x12A0,
                                                    gabi::ea(&line) + 0x14) == 0x13;
    }
    line.__vtbl_58 = 0x10008FE4;
    line.__vtbl_64 = 0x10008FA4;
    line.__vtbl_20 = 0x10008F94;
    Bgn3Bindings::cBgS_LinChk_dt(gabi::ea(&line), 0);
    return !water;
}

/* 0208FF44 */
void drop_eff_set(u32 a) {
    auto* i_this = gabi::at<bgn3_class>(a);
    cXyz pos;
    f32 x = static_cast<f32>(i_this->current.pos.x), y = static_cast<f32>(i_this->current.pos.y);
    pos.x = x;
    f32 z = static_cast<f32>(i_this->current.pos.z);
    pos.y = y;
    pos.z = z;
    if (gr_check(a, gabi::ea(&pos)))
        return;
    for (int i = 0; i < 3; ++i)
        Bgn3Bindings::dPa_control_c_set(gabi::ea(dComIfGp_getParticle()), 0, 0x8414 + i,
                                        gabi::ea(&pos), 0, 0, 255, 0, -1, 0, 0, 0);
    Bgn3Bindings::dPa_control_c_setSimple(gabi::ea(dComIfGp_getParticle()), 0x840E, gabi::ea(&pos),
                                          255, 0x101D5E98u, 0x101D5E98u, 0);
    Bgn3Bindings::dPa_control_c_setSimple(gabi::ea(dComIfGp_getParticle()), 0x8408, gabi::ea(&pos),
                                          255, 0x101D5E98u, 0x101D5E98u, 0);
}

/* 020900A4 */
u32 esa_s_sub(u32 a, u32 ignored) {
    auto* actor = gabi::at<fopAc_ac_c>(a);
    if (fopAc_IsActor(actor) && a && fpcM_GetName(actor) == fpcNm_ESA_e)
        return a;
    return 0;
}

void copyModelMatrix(u32 model, bool alternateOrder = false) {
    Mtx34* matrix = calc_mtx();
    auto* object = gabi::at<J3DModel>(model);
    f32 cells[12];
    for (int i = 0; i < 12; ++i)
        cells[i] = matrix->m[i / 4][i % 4];
    // HD: GHS emitted two store orders at the inline assignment sites.
    if (alternateOrder) {
        for (int i : {5, 6, 7, 9, 10, 11, 0, 1, 2, 3, 4, 8})
            object->mBaseTransformMtx.m[i / 4][i % 4] = cells[i];
    } else {
        for (int i : {0, 1, 2, 3, 4, 8, 5, 6, 7, 9, 10, 11})
            object->mBaseTransformMtx.m[i / 4][i % 4] = cells[i];
    }
}

/* inlined into caller */
void part_control(u32 a) {
    auto* i_this = gabi::at<bgn3_class>(a);
    cLib_addCalcAngleS2(&i_this->shape_angle.y, static_cast<s16>(i_this->current.angle.y), 4,
                        0x1000);
    cLib_addCalcAngleS2(&i_this->shape_angle.x, static_cast<s16>(i_this->current.angle.x), 8,
                        0x400);
    cMtx_YrotS(calc_mtx(), static_cast<s16>(i_this->shape_angle.y));
    cMtx_XrotM(calc_mtx(), static_cast<s16>(i_this->shape_angle.x));
    cXyz direction;
    cXyz delta;
    cXyz output;
    direction.x = 0.0f;
    direction.y = 0.0f;
    direction.z = singleAdd(REG_F(0, 3), -300.0f);
    MtxPosition(&direction, &delta);
    Bgn3Bindings::cXyz_add(a + 0x314, gabi::ea(&output), gabi::ea(&delta));
    bgn3Word(i_this->m0FD7C.x) = static_cast<u32>(bgn3Word(output.x));
    bgn3Word(i_this->m0FD7C.y) = static_cast<u32>(bgn3Word(output.y));
    bgn3Word(i_this->m0FD7C.z) = static_cast<u32>(bgn3Word(output.z));
    for (int i = 0; i < 9; ++i) {
        f32 size = singleAdd(REG_F(0, 11), 1.7f);
        f32 factor = size_d[i];
        i_this->mParts[i].m00F4 = factor * size;
    }
    f32 separation = singleAdd(REG_F(0, 8), 150.0f);
    f32 stiffness = static_cast<f32>(i_this->m0FDB8);
    Bgn3LinChk line;
    Bgn3Bindings::cBgS_LinChk_ct(gabi::ea(&line));
    f32 gravity = singleAdd(REG_F(0, 6), -5.0f);
    for (u32 i = 0; i < 8; ++i)
        line.mPass[i] = 0;
    line.mpPolyPassChk = gabi::ea(&line) + 0x58;
    line.mpGrpPassChk = gabi::ea(&line) + 0x64;
    line.__vtbl_10 = 0x10008FB4;
    line.mGrp = 1;
    line.__vtbl_64 = 0x10008FD4;
    line.__vtbl_58 = 0x10008FE4;
    line.__vtbl_20 = 0x10008FC4;
    u32 firstPart = a + 0x1BE8;
    bgn3Word(bgn3Part(firstPart)->m00D4.x) = static_cast<u32>(bgn3Word(i_this->m0FD7C.x));
    bgn3Word(bgn3Part(firstPart)->m00D4.y) = static_cast<u32>(bgn3Word(i_this->m0FD7C.y));
    bgn3Word(bgn3Part(firstPart)->m00D4.z) = static_cast<u32>(bgn3Word(i_this->m0FD7C.z));
    cMtx_YrotS(calc_mtx(), static_cast<s16>(i_this->shape_angle.y));
    cMtx_XrotM(calc_mtx(), static_cast<s16>(i_this->shape_angle.x));
    direction.x = 0.0f;
    direction.y = 0.0f;
    direction.z = -static_cast<f32>(i_this->m0FDBC);
    MtxPosition(&direction, &delta);
    for (int i = 1; i < 10; ++i) {
        u32 part = firstPart + i * 0x19D8;
        u32 prev = part - 0x19D8;
        f32 floor = singleAdd(REG_F(0, 19), 70.0f) * static_cast<f32>(bgn3Part(prev)->m00F4);
        f32 y = singleAdd(singleAdd(static_cast<f32>(bgn3Part(part)->m00D4.y), gravity),
                          static_cast<f32>(bgn3Part(part)->m00E8.y));
        f32 x = static_cast<f32>(bgn3Part(part)->m00D4.x);
        f32 prevZ = static_cast<f32>(bgn3Part(prev)->m00D4.z);
        f32 prevX = static_cast<f32>(bgn3Part(prev)->m00D4.x);
        if (!(y > floor))
            y = floor;
        f32 z = static_cast<f32>(bgn3Part(part)->m00D4.z);
        f32 dx = singleAdd(x - prevX, static_cast<f32>(bgn3Part(part)->m00E8.x));
        f32 dz = singleAdd(z - prevZ, static_cast<f32>(bgn3Part(part)->m00E8.z));
        dx = singleAdd(dx, static_cast<f32>(delta.x));
        dz = singleAdd(dz, static_cast<f32>(delta.z));
        f32 dy = y - static_cast<f32>(bgn3Part(prev)->m00D4.y);
        s16 heading = cM_atan2s(dx, dz);
        bgn3Part(prev)->m00E2 = heading;
        f32 horizontal = Bgn3Bindings::sqrtf(gabi::fmadds(dx, dx, dz * dz));
        u32 pitch = cM_atan2s(dy, horizontal);
        bgn3Part(prev)->m00E0 = (s16)(0u - pitch);
        cMtx_YrotS(calc_mtx(), static_cast<s16>(bgn3Part(prev)->m00E2));
        cMtx_XrotM(calc_mtx(), static_cast<s16>(bgn3Part(prev)->m00E0));
        direction.y = 0.0f;
        direction.x = 0.0f;
        direction.z = separation * static_cast<f32>(bgn3Part(prev)->m00F4);
        cXyz segment;
        MtxPosition(&direction, &segment);
        u32 oldX = static_cast<u32>(bgn3Word(bgn3Part(part)->m00D4.x));
        u32 oldY = static_cast<u32>(bgn3Word(bgn3Part(part)->m00D4.y));
        bgn3Word(bgn3Part(part)->m00E8.x) = oldX;
        u32 oldZ = static_cast<u32>(bgn3Word(bgn3Part(part)->m00D4.z));
        bgn3Word(bgn3Part(part)->m00E8.y) = oldY;
        bgn3Word(bgn3Part(part)->m00E8.z) = oldZ;
        Bgn3Bindings::cXyz_add(prev + 0x288, gabi::ea(&output), gabi::ea(&segment));
        bgn3Word(bgn3Part(part)->m00D4.x) = static_cast<u32>(bgn3Word(output.x));
        f32 vx =
            static_cast<f32>(bgn3Part(part)->m00D4.x) - static_cast<f32>(bgn3Part(part)->m00E8.x);
        bgn3Word(bgn3Part(part)->m00D4.y) = static_cast<u32>(bgn3Word(output.y));
        bgn3Word(bgn3Part(part)->m00D4.z) = static_cast<u32>(bgn3Word(output.z));
        f32 vy =
            static_cast<f32>(bgn3Part(part)->m00D4.y) - static_cast<f32>(bgn3Part(part)->m00E8.y);
        f32 vz =
            static_cast<f32>(bgn3Part(part)->m00D4.z) - static_cast<f32>(bgn3Part(part)->m00E8.z);
        bgn3Part(part)->m00E8.x = vx * stiffness;
        bgn3Part(part)->m00E8.y = vy * stiffness;
        bgn3Part(part)->m00E8.z = vz * stiffness;
    }
    cLib_addCalc2(&i_this->m0FDB8, static_cast<f32>(l_HIO.m38), 1.0f, 0.002f);
    cLib_addCalc2(&i_this->m0FDBC, static_cast<f32>(l_HIO.m34), 1.0f, 0.5f);
    line.__vtbl_58 = 0x10008FE4;
    line.__vtbl_64 = 0x10008FA4;
    line.__vtbl_20 = 0x10008F94;
    Bgn3Bindings::cBgS_LinChk_dt(gabi::ea(&line), 0);
}

/* 020900F4 */
bool checkWall(u32 a) {
    auto* i_this = gabi::at<bgn3_class>(a);
    cXyz delta;
    cXyz v;
    cXyz out;
    Bgn3Bindings::cXyz_subtract(a + 0x314, gabi::ea(&delta), a + 0x300);
    u32 z = static_cast<u32>(bgn3Word(delta.z)), x = static_cast<u32>(bgn3Word(delta.x));
    bgn3Word(v.z) = z;
    bgn3Word(v.x) = x;
    u32 y = static_cast<u32>(bgn3Word(delta.y));
    bgn3Word(v.y) = y;
    v.y = 0.0f;
    f32 sq = PSVECSquareMag(&v);
    f32 len = Bgn3Bindings::sqrtf(sq);
    if (!(len > 0.0f))
        return false;
    Bgn3LinChk line;
    Bgn3Bindings::cBgS_LinChk_ct(gabi::ea(&line));
    line.mPass[5] = 0;
    line.mPass[2] = 0;
    line.mpGrpPassChk = gabi::ea(&line) + 0x64;
    line.mPass[1] = 0;
    line.mpPolyPassChk = gabi::ea(&line) + 0x58;
    line.__vtbl_58 = 0x10008FE4;
    line.mPass[0] = 0;
    line.__vtbl_20 = 0x10008FC4;
    line.__vtbl_10 = 0x10008FB4;
    u32 mat = gabi::ea(calc_mtx());
    line.mPass[4] = 0;
    line.mPass[6] = 0;
    line.__vtbl_64 = 0x10008FD4;
    line.mPass[3] = 0;
    line.mGrp = 1;
    s16 angle = cM_atan2s(static_cast<f32>(v.x), static_cast<f32>(v.z));
    cMtx_YrotS(gabi::at<Mtx34>(mat), angle);
    f32 forward = REG_F(10, 2) + 300.0f, up = REG_F(0, 4) + 150.0f;
    v.x = 0.0f;
    v.y = forward;
    v.z = up;
    MtxPosition(&v, &out);
    PSVECAdd(&out, &i_this->current.pos, &out);
    f32 px = static_cast<f32>(i_this->current.pos.x), height = REG_F(10, 2) + 300.0f;
    v.x = px;
    f32 py = static_cast<f32>(i_this->current.pos.y);
    v.y = py;
    f32 pz = static_cast<f32>(i_this->current.pos.z);
    v.z = pz;
    v.y = py + height;
    Bgn3Bindings::dBgS_LinChk_Set(gabi::ea(&line), gabi::ea(&v), gabi::ea(&out), a);
    bool hit = Bgn3Bindings::cBgS_LineCross(dComIfGp_ea() + 0x12A0, gabi::ea(&line)) != 0;
    if (hit) {
        f32 oldx = static_cast<f32>(i_this->old.pos.x), oldz = static_cast<f32>(i_this->old.pos.z);
        i_this->current.pos.x = oldx;
        i_this->current.pos.z = oldz;
    }
    line.__vtbl_58 = 0x10008FE4;
    line.__vtbl_64 = 0x10008FA4;
    line.__vtbl_20 = 0x10008F94;
    Bgn3Bindings::cBgS_LinChk_dt(gabi::ea(&line), 0);
    return hit;
}

/* 02090340 */
bool checkGround(u32 a) {
    auto* i_this = gabi::at<bgn3_class>(a);
    f32 floor = REG0_F(9) + 150.0f;
    if (!(i_this->current.pos.y > floor)) {
        i_this->current.pos.y = floor;
        i_this->speed.y = 0.0f;
        return true;
    }
    return false;
}

/* 02090384 */
void pos_move(u32 a) {
    auto* i_this = gabi::at<bgn3_class>(a);
    cXyz v;
    cXyz out;
    v.x = 0.0f;
    v.y = 0.0f;
    v.z = i_this->speedF;
    cMtx_YrotS(calc_mtx(), i_this->current.angle.y);
    MtxPosition(&v, &out);
    i_this->speed.x = out.x;
    i_this->speed.z = out.z;
    PSVECAdd(&i_this->current.pos, &i_this->speed, &i_this->current.pos);
    f32 delta = REG0_F(3) - 5.0f;
    i_this->speed.y = static_cast<f32>(i_this->speed.y) + delta;
}

// Inlined move states in the HD move routine.
/* inlined into caller */
void moveShock(u32 a, s32 strength) {
    u32 play = dComIfGp_ea();
    strength += REG0_S(2);
    cXyz direction;
    direction.x = 0.0f;
    direction.y = 1.0f;
    direction.z = 0.0f;
    Bgn3Bindings::dVibration_c_StartShock(play + 0x599C, strength, -33, gabi::ea(&direction));
}

s16 centerAngle(u32 a) {
    auto* i_this = gabi::at<bgn3_class>(a);
    f32 x = -static_cast<f32>(i_this->current.pos.x);
    f32 z = -static_cast<f32>(i_this->current.pos.z);
    return cM_atan2s(x, z);
}

void moveSound(u32 a, u32 id, u32 value) {
    auto* i_this = gabi::at<bgn3_class>(a);
    if (a + 0x37C) {
        s32 reverb = dComIfGp_getReverb(static_cast<s8>(i_this->current.roomNo));
        Bgn3Bindings::mDoAud_seStart(id, a + 0x37C, value, reverb);
    }
}

void monsterSound(u32 a, u32 id) {
    auto* i_this = gabi::at<bgn3_class>(a);
    if (a + 0x37C) {
        u32 identity = a ? static_cast<u32>(i_this->mBsPcId) : 0xFFFFFFFFu;
        s32 reverb = dComIfGp_getReverb(static_cast<s8>(i_this->current.roomNo));
        Bgn3Bindings::mDoAud_monsSeStartAtActor(id, a + 0x37C, identity, 0, reverb);
    }
}

u32 soundVolume(f32 value) {
    u32 integer;
    if (!(value < 2147483648.0f))
        integer = (u32)gabi::ftoi(value - 2147483648.0f) + 0x80000000u;
    else
        integer = (u32)gabi::ftoi(value);
    return integer > 100 ? 100 : integer;
}

/* inlined into caller */
void splash(u32 a, u32 headEffect, u32 partEffect, bool ripples) {
    auto* i_this = gabi::at<bgn3_class>(a);
    u32 s = a + 0x11E58;
    cXyz p;
    if (((static_cast<s16>(i_this->m0FD88) + 2) & 3) == 0) {
        bgn3Word(p.x) = static_cast<u32>(bgn3Word(i_this->current.pos.x));
        bgn3Word(p.y) = static_cast<u32>(bgn3Word(i_this->current.pos.y));
        bgn3Word(p.z) = static_cast<u32>(bgn3Word(i_this->current.pos.z));
        if (!gr_check(a, gabi::ea(&p))) {
            u32 play = dComIfGp_ea();
            Bgn3Bindings::dPa_control_c_setSimple(gabi::ea(bgn3GameInfo(play)->play.mParticle),
                                                  headEffect, gabi::ea(&p), 255, 0x101D5E98u,
                                                  0x101D5E98u, 0);
            if (ripples && static_cast<s16>(i_this->m0FD94) == 0) {
                f32 random = cM_rndF(20.0f);
                i_this->m0FD94 = (s16)gabi::ftoi(singleAdd(random, 20.0f));
                moveSound(a, 0x6A44, 0);
            }
        }
    }
    for (int i = 0; i < 9; ++i) {
        if (((static_cast<s16>(i_this->m0FD88) + i) & (ripples ? 3 : 7)) == 0) {
            u32 part = a + 0x1BE8 + i * 0x19D8;
            bgn3Word(p.x) = static_cast<u32>(bgn3Word(bgn3Part(part)->m00D4.x));
            bgn3Word(p.y) = static_cast<u32>(bgn3Word(bgn3Part(part)->m00D4.y));
            bgn3Word(p.z) = static_cast<u32>(bgn3Word(bgn3Part(part)->m00D4.z));
            if (!gr_check(a, gabi::ea(&p))) {
                u32 play = dComIfGp_ea();
                Bgn3Bindings::dPa_control_c_setSimple(gabi::ea(bgn3GameInfo(play)->play.mParticle),
                                                      partEffect, gabi::ea(&p), 255, 0x101D5E98u,
                                                      0x101D5E98u, 0);
                if (ripples && i == 8 && static_cast<s16>(i_this->m0FD94) == 0) {
                    f32 random = cM_rndF(20.0f);
                    i_this->m0FD94 = (s16)gabi::ftoi(singleAdd(random, 20.0f));
                    s32 reverb = dComIfGp_getReverb(static_cast<s8>(i_this->current.roomNo));
                    Bgn3Bindings::mDoAud_seStart(0x6A45u, s + 0x2D8, 0, reverb);
                }
            }
        }
    }
}

/* inlined into caller */
bool move0(u32 a) {
    auto* i_this = gabi::at<bgn3_class>(a);
    dComIfGp_ea();
    u32 bait = gabi::ea(esa);
    s16 turn = 0;
    s16 center = 0;
    bool chase = false;
    if (bait) {
        turn = static_cast<s16>(i_this->m0FD8E);
        i_this->m0FD90 = 1;
        center = static_cast<s16>(i_this->m0FDB0);
        chase = true;
    } else {
        s16 timer = static_cast<s16>(i_this->m0FD90);
        if (timer) {
            turn = static_cast<s16>(i_this->m0FD8E);
            i_this->m0FD90 = (s16)(timer - 1);
            center = static_cast<s16>(i_this->m0FDB0);
            chase = true;
        } else {
            i_this->current.angle.y =
                (s16)(static_cast<s16>(i_this->current.angle.y) + static_cast<s16>(i_this->m0FD8E));
        }
    }
    if (chase) {
        s16 amount = turn < 0 ? (s16)(0u - (u32)turn) : turn;
        if (center) {
            s16 heading = centerAngle(a);
            cLib_addCalcAngleS2(&i_this->current.angle.y, heading, 4, amount);
        } else if ((bait = gabi::ea(esa))) {
            cXyz delta;
            Bgn3Bindings::cXyz_subtract(bait + 0x314, gabi::ea(&delta), a + 0x314);
            f32 x = static_cast<f32>(delta.x);
            f32 z = static_cast<f32>(delta.z);
            s16 heading = cM_atan2s(x, z);
            i_this->m0FDBC = singleAdd(REG_F(10, 7), 30.0f);
            cLib_addCalcAngleS2(&i_this->current.angle.y, heading, 4, (s16)(amount * 2));
        } else {
            u32 play = dComIfGp_ea();
            s16 heading = Bgn3Bindings::fopAcM_searchActorAngleY(
                a, gabi::ea(bgn3GameInfo(play)->play.mPlayerInfo[0].mpPlayer));
            s32 step = REG0_S(9) + 0x900;
            s16 counter = static_cast<s16>(i_this->m0FD88);
            f32 amplitude = singleAdd(REG_F(0, 18), 4000.0f);
            f32 sine = cM_ssin(counter * step);
            s16 wobble = (s16)gabi::ftoi(sine * amplitude);
            cLib_addCalcAngleS2(&i_this->current.angle.y, (s16)(heading + wobble), 4, amount);
        }
    }
    s8 health = static_cast<s8>(i_this->health);
    f32 speed = (health == 3   ? static_cast<f32>(l_HIO.m08)
                 : health == 2 ? static_cast<f32>(l_HIO.m10)
                               : static_cast<f32>(l_HIO.m18));
    cLib_addCalc2(&i_this->speedF, speed, 1.0f, 3.0f);
    pos_move(a);
    checkGround(a);
    bool struckWall = false;
    if (checkWall(a) && static_cast<s16>(i_this->m0FDAA[1]) == 0) {
        i_this->m0FD90 = 0;
        s16 heading = centerAngle(a);
        i_this->current.angle.y = heading;
        f32 stiffness = singleAdd(REG_F(0, 5), 0.9f);
        i_this->m0FDAA[1] = 20;
        i_this->m0FDB8 = stiffness;
        struckWall = true;
        u32 play = dComIfGp_ea();
        s32 strength = REG0_S(2) + 5;
        cXyz direction;
        direction.z = 0.0f;
        direction.y = 1.0f;
        direction.x = 0.0f;
        Bgn3Bindings::dVibration_c_StartShock(play + 0x599C, strength, -33, gabi::ea(&direction));
        moveSound(a, 0x5981, 0);
        i_this->m100AC = static_cast<s16>(l_HIO.m3C);
        health = static_cast<s8>(i_this->health);
        s16 threshold = (health == 3   ? static_cast<s16>(l_HIO.m0E)
                         : health == 2 ? static_cast<s16>(l_HIO.m16)
                                       : static_cast<s16>(l_HIO.m1E));
        s16 count = (s16)(static_cast<s16>(i_this->m0FD92) + 1);
        if (count >= threshold) {
            i_this->m0FD92 = 0;
            health = static_cast<s8>(i_this->health);
            i_this->m0FD90 = (health == 3   ? static_cast<s16>(l_HIO.m20)
                              : health == 2 ? static_cast<s16>(l_HIO.m22)
                                            : static_cast<s16>(l_HIO.m24));
        } else {
            i_this->m0FD92 = count;
        }
    }
    bool newTurn = struckWall;
    if (static_cast<s16>(i_this->m0FDAE) == 0) {
        f32 random = cM_rndF(50.0f);
        i_this->m0FDAE = (s16)gabi::ftoi(singleAdd(random, 30.0f));
        health = static_cast<s8>(i_this->health);
        newTurn = true;
    }
    if (newTurn) {
        i_this->m0FD8E = (health == 3   ? static_cast<s16>(l_HIO.m0C)
                          : health == 2 ? static_cast<s16>(l_HIO.m14)
                                        : static_cast<s16>(l_HIO.m1C));
        f32 random = cM_rndF(1.0f);
        if (random < 0.5f)
            i_this->m0FD8E = (s16)(0u - (u32) static_cast<s16>(i_this->m0FD8E));
    }
    u32 second = static_cast<u32>(i_this->m0FF28.mObjAt.mSPrm);
    u32 first = static_cast<u32>(i_this->m0FDFC.mObjAt.mSPrm);
    i_this->m0FF28.mObjAt.mSPrm = second | 1;
    i_this->m0FDFC.mObjAt.mSPrm = first | 1;
    i_this->current.angle.x = 0;
    if (static_cast<f32>(i_this->speedF) > 10.0f) {
        splash(a, 0x840E, 0x840D, false);
        moveSound(a, 0x705D, 0);
        move_se_set(a);
        return true;
    }
    return false;
}

void mahi(u32 a) {
    auto* i_this = gabi::at<bgn3_class>(a);
    dComIfGp_ea();
    u16 phase = static_cast<u16>(i_this->m0FD8C);
    if (phase == 0) {
        i_this->m0FD8C = 1;
        phase = 1;
    }
    if (phase == 1) {
        Bgn3Bindings::mDoExt_McaMorf_play(static_cast<u32>(bgn3Word(i_this->mpMorf)), a + 0x314, 0,
                                          0);
        if (static_cast<s16>(i_this->m0FDAA[0]) == 0) {
            i_this->m0FD8A = 0;
            i_this->m0FD8C = 0;
        }
    }
    cLib_addCalc0(&i_this->speedF, 1.0f, 3.0f);
    pos_move(a);
    i_this->current.angle.x = 0;
    checkGround(a);
    if (checkWall(a) && static_cast<s16>(i_this->m0FDAA[1]) == 0) {
        i_this->m0FD90 = 0;
        s16 heading = centerAngle(a);
        i_this->current.angle.y = heading;
        i_this->m0FDB8 = singleAdd(REG_F(0, 5), 0.9f);
    }
}

/* inlined into caller */
void movementPitch(u32 a) {
    auto* i_this = gabi::at<bgn3_class>(a);
    cXyz difference;
    Bgn3Bindings::cXyz_subtract(a + 0x314, gabi::ea(&difference), a + 0x300);
    f32 z = static_cast<f32>(difference.z);
    f32 x = static_cast<f32>(difference.x);
    f32 y = static_cast<f32>(difference.y);
    f32 square = gabi::fmadds(x, x, z * z);
    f32 horizontal = Bgn3Bindings::sqrtf(square);
    s16 angle = (s16)(0u - cM_atan2s(y, horizontal));
    if (angle > 0x3000)
        angle = 0x3000;
    else if (angle < -0x3000)
        angle = -0x3000;
    i_this->current.angle.x = angle;
}

void damage(u32 a) {
    auto* i_this = gabi::at<bgn3_class>(a);
    dComIfGp_ea();
    u16 phase = static_cast<u16>(i_this->m0FD8C);
    if (phase == 0) {
        i_this->speed.y = 150.0f;
        i_this->speedF = 30.0f;
        i_this->m0FDB8 = 1.0f;
        i_this->m0FD8C = 1;
    } else if (phase == 1) {
        f32 target = singleAdd(REG_F(0, 9), 350.0f);
        f32 step = singleAdd(REG_F(0, 10), 50.0f);
        cLib_addCalc2(&i_this->m100A0, target, 1.0f, step);
        if (checkGround(a)) {
            i_this->speed.y = 50.0f;
            i_this->m0FD8C = 2;
            f32 random = cM_rndF(65536.0f);
            i_this->current.angle.y = (s16)gabi::ftoi(random);
            i_this->m0FDB8 = 1.0f;
            moveShock(a, 5);
            drop_eff_set(a);
        }
    } else if (phase == 2 && checkGround(a)) {
        i_this->m0FD8A = 0;
        i_this->m0FD8C = 0;
    }
    pos_move(a);
    movementPitch(a);
    checkGround(a);
    if (checkWall(a) && static_cast<s16>(i_this->m0FDAA[1]) == 0) {
        i_this->current.angle.y = centerAngle(a);
        i_this->m0FDAA[1] = 20;
    }
    u32 first = static_cast<u32>(i_this->m0FDFC.mObjAt.mSPrm);
    u32 second = static_cast<u32>(i_this->m0FF28.mObjAt.mSPrm);
    i_this->m0FDFC.mObjAt.mSPrm = first & ~1u;
    i_this->m0FF28.mObjAt.mSPrm = second & ~1u;
}

void end(u32 a) {
    auto* i_this = gabi::at<bgn3_class>(a);
    u32 play = dComIfGp_ea();
    u32 player = gabi::ea(bgn3GameInfo(play)->play.mPlayerInfo[0].mpPlayer);
    u16 phase = static_cast<u16>(i_this->m0FD8C);
    if (phase == 0) {
        i_this->speed.y = 150.0f;
        i_this->speedF = 0.0f;
        i_this->m0FD8C = 1;
        bgn->mCSMode = 20;
        i_this->m0FDB8 = 1.0f;
        if (player && player + 0x37C)
            monsterSound(player, 0x4830);
    } else if (phase >= 1 && phase <= 4) {
        f32 height = static_cast<f32>(i_this->current.pos.y) - 150.0f;
        if (height > 350.0f)
            height = 350.0f;
        i_this->m100A0 = height;
        if (checkGround(a)) {
            i_this->m0FD8C = (s16)(static_cast<s16>(i_this->m0FD8C) + 1);
            f32 random = cM_rndF(65536.0f);
            i_this->current.angle.y = (s16)gabi::ftoi(random);
            i_this->m0FDB8 = 1.0f;
            moveShock(a, 5);
            drop_eff_set(a);
            i_this->speed.y = singleAdd(REG_F(0, 11), 130.0f);
            i_this->speedF = 0.0f;
            random = cM_rndF(65536.0f);
            i_this->current.angle.y = (s16)gabi::ftoi(random);
            monsterSound(a, 0x496E);
        }
        if (static_cast<s16>(i_this->m0FD8C) == 4 &&
            !(static_cast<f32>(i_this->speed.y) > -30.0f)) {
            i_this->m0FD8C = 5;
            i_this->m0FD8E = 0;
        }
    } else if (phase == 5) {
        f32 rise = singleAdd(REG_F(0, 12), 5.5f);
        f32 speed = singleAdd(static_cast<f32>(i_this->speed.y), rise);
        i_this->speed.y = speed;
        if (speed > 0.0f)
            cLib_addCalc2(&i_this->m10060, 1.0f, 1.0f, 0.015f);
        i_this->current.angle.y =
            (s16)(static_cast<s16>(i_this->current.angle.y) + static_cast<s16>(i_this->m0FD8E));
        i_this->shape_angle.y =
            (s16)(static_cast<s16>(i_this->shape_angle.y) + static_cast<s16>(i_this->m0FD8E));
        cLib_addCalcAngleS2(&i_this->m0FD8E, 5000, 4, 100);
        f32 target = singleAdd(REG_F(0, 6), 50.0f);
        cLib_addCalc2(&i_this->m0FDBC, target, 1.0f, 1.0f);
        f32 step = singleAdd(REG_F(0, 10), 50.0f);
        cLib_addCalc0(&i_this->m100A0, 1.0f, step);
    }
    pos_move(a);
    movementPitch(a);
    checkGround(a);
    u32 second = static_cast<u32>(i_this->m0FF28.mObjAt.mSPrm);
    u32 first = static_cast<u32>(i_this->m0FDFC.mObjAt.mSPrm);
    i_this->m0FDFC.mObjAt.mSPrm = first & ~1u;
    i_this->m0FF28.mObjAt.mSPrm = second & ~1u;
    cLib_addCalc2(&i_this->current.pos.x, -15.0f, 0.05f, 30.0f);
    cLib_addCalc2(&i_this->current.pos.z, 375.17f, 0.05f, 30.0f);
}

u32 movementParticle(u32 position, u32 effect, u32 rotation = 0, u32 scale = 0) {
    u32 play = dComIfGp_ea();
    return Bgn3Bindings::dPa_control_c_set(gabi::ea(bgn3GameInfo(play)->play.mParticle), 0, effect,
                                           position, rotation, scale, 255, 0, -1, 0, 0, 0);
}

void emitterScale(u32 emitter, f32 size) {
    auto* particle = gabi::at<Bgn3EmitterView>(emitter);
    particle->mGlobalScale.x = size;
    particle->mGlobalScale.y = size;
    particle->mGlobalScale.z = size;
    particle->mGlobalParticleScale.x = size;
    particle->mGlobalParticleScale.y = size;
    particle->mGlobalParticleScale.z = size;
}

void emitterMove(u32 ownerSlot, u32 position, f32 scale) {
    auto* slot = gabi::at<be<u32>>(ownerSlot);
    u32 emitter = *slot;
    auto* particle = gabi::at<Bgn3EmitterView>(emitter);
    u8 type = particle->mType;
    auto* pos = gabi::at<cXyz>(position);
    f32 y = pos->y;
    f32 x = pos->x;
    f32 z = pos->z;
    // HD: the emitter coordinate convention depends on its type.
    if (type >= 7)
        y = -y;
    particle->mGlobalTranslation.x = x;
    particle->mGlobalTranslation.y = y;
    particle->mGlobalTranslation.z = z;
    // Reload the owner after writing the translation: its slot may alias emitter storage.
    emitterScale(*slot, scale);
}

void retireEmitter(u32 ownerSlot) {
    auto* slot = gabi::at<be<u32>>(ownerSlot);
    u32 emitter = *slot;
    if (emitter) {
        auto* particle = gabi::at<Bgn3EmitterView>(emitter);
        u32 flags = particle->mFlags;
        particle->mMaxFrame = -1;
        particle->mFlags = flags | 1;
        *slot = 0;
    }
}

/* inlined into caller */
void damage_check(u32 a) {
    auto* i_this = gabi::at<bgn3_class>(a);
    u32 s = a + 0x11E58;
    if (static_cast<s16>(i_this->m0FDB4))
        return;
    Bgn3AtInfo hit;
    if (Bgn3Bindings::dCcD_GObjInf_ChkTgHit(s + 0x80)) {
        u32 object = Bgn3Bindings::dCcD_GObjInf_GetTgHitObj(s + 0x80);
        hit.mpObj = object;
        u32 source = Bgn3Bindings::at_power_check(gabi::ea(&hit));
        hit.mpActor = source;
        if (a && a + 0x37C) {
            monsterSound(a, 0x4973);
            moveSound(a, 0x5982, 0);
        }
        u8 kind = static_cast<u8>(hit.mResultingAttackType);
        s32 severity;
        s16 duration;
        if (kind == 9) {
            severity = 6;
            u32 play = dComIfGp_ea();
            cXyz direction;
            direction.x = 0.0f;
            direction.y = 1.0f;
            direction.z = 0.0f;
            Bgn3Bindings::dVibration_c_StartShock(play + 0x599C, REG0_S(2) + 3, -33,
                                                  gabi::ea(&direction));
            bgn3PauseTimer() = 5;
            i_this->m100A8 = 30;
            duration = static_cast<s16>(l_HIO.m30);
        } else if (kind == 2) {
            i_this->m100A8 = 25;
            severity = 5;
            duration = static_cast<s16>(l_HIO.m2E);
        } else if (kind == 1) {
            u32 objectInfo = Bgn3Bindings::dCcD_GetGObjInf(static_cast<u32>(hit.mpObj));
            if (static_cast<u8>(gabi::at<dCcD_GObjInf>(objectInfo)->mGObjAt.mSpl) == 1) {
                bgn3PauseTimer() = 3;
                i_this->m100A8 = 20;
                severity = 4;
                duration = static_cast<s16>(l_HIO.m2C);
            } else {
                bgn3PauseTimer() = 2;
                i_this->m100A8 = 15;
                severity = 3;
                duration = static_cast<s16>(l_HIO.m2A);
            }
        } else if (static_cast<u32>(hit.mpActor) &&
                   fpcM_GetName(gabi::at<fopAc_ac_c>(hit.mpActor)) == fpcNm_ARROW_e) {
            i_this->m100A8 = 10;
            severity = 2;
            duration = static_cast<s16>(l_HIO.m28);
        } else {
            i_this->m100A8 = 7;
            severity = 1;
            duration = static_cast<s16>(l_HIO.m26);
        }
        if (static_cast<s16>(i_this->m0FD8A) != 1) {
            i_this->m0FDAA[0] = duration;
            i_this->m0FD8A = 1;
        }
        i_this->m0FD8C = 0;
        i_this->m0FDB4 = 20;
        if (severity >= 4) {
            movementParticle(s + 0x14C, 0x10);
            cXyz size;
            csXyz angle;
            size.x = 2.0f;
            size.z = 2.0f;
            size.y = 2.0f;
            angle.z = 0;
            angle.x = 0;
            u32 play = dComIfGp_ea();
            s16 heading = Bgn3Bindings::fopAcM_searchActorAngleY(
                a, gabi::ea(bgn3GameInfo(play)->play.mPlayerInfo[0].mpPlayer));
            angle.y = heading;
            movementParticle(s + 0x14C, 0xD, gabi::ea(&angle), gabi::ea(&size));
            cXyz p;
            p.x = static_cast<f32>(i_this->m0FDFC.mGObjTg.mHitPos.x);
            p.y = static_cast<f32>(i_this->m0FDFC.mGObjTg.mHitPos.y);
            p.z = static_cast<f32>(i_this->m0FDFC.mGObjTg.mHitPos.z);
            Bgn3Bindings::dKy_SordFlush_set(gabi::ea(&p), 1);
        }
        Bgn3Bindings::def_se_set(a, static_cast<u32>(hit.mpObj), 0x40);
        return;
    }
    if (Bgn3Bindings::dCcD_GObjInf_ChkTgHit(a + 0xED54)) {
        i_this->m0FDB4 = 20;
        i_this->m0FD9E = 1;
        i_this->m0FD8A = 2;
        i_this->m0FD8C = 0;
        s8 health = static_cast<s8>(i_this->health);
        if (health) {
            i_this->health = (s8)(health - 1);
            s32 reverb = dComIfGp_getReverb(static_cast<s8>(i_this->current.roomNo));
            Bgn3Bindings::mDoAud_seStart(0x2879u, 0, 0x35, reverb);
            f32 size = singleAdd(REG_F(0, 5), 2.0f);
            health = static_cast<s8>(i_this->health);
            u32 play = dComIfGp_ea();
            if (health == 0) {
                bgn3GameInfo(play)->play.mItemLifeCount = 0.0f;
                Bgn3Bindings::mDoAud_bgmStop(30);
                u32 emitter = movementParticle(s + 0x2D8, 0x8457);
                if (emitter)
                    emitterScale(emitter, size);
                emitter = movementParticle(s + 0x2D8, 0x8458);
                if (emitter)
                    emitterScale(emitter, size);
                i_this->m0FD8A = 3;
                i_this->m0FD8C = 0;
                i_this->m0FDB4 = 10000;
                reverb = dComIfGp_getReverb(static_cast<s8>(i_this->current.roomNo));
                Bgn3Bindings::mDoAud_monsSeStart(0x4970u, 0, 0, reverb);
                reverb = dComIfGp_getReverb(static_cast<s8>(i_this->current.roomNo));
                Bgn3Bindings::mDoAud_seStart(0x5983u, 0, 0, reverb);
            } else {
                u32 emitter =
                    Bgn3Bindings::dPa_control_c_set(gabi::ea(bgn3GameInfo(play)->play.mParticle), 0,
                                                    0x8459, s + 0x2D8, 0, 0, 255, 0, -1, 0, 0, 0);
                if (emitter)
                    emitterScale(emitter, size);
                reverb = dComIfGp_getReverb(static_cast<s8>(i_this->current.roomNo));
                Bgn3Bindings::mDoAud_monsSeStart(0x496Eu, 0, 0, reverb);
            }
        }
    }
    bool hitAny = false;
    if (Bgn3Bindings::dCcD_GObjInf_ChkTgHit(s + 0x1AC)) {
        hit.mpObj = Bgn3Bindings::dCcD_GObjInf_GetTgHitObj(s + 0x1AC);
        hitAny = true;
    }
    for (int i = 0; i < 8; ++i) {
        u32 sphere = a + 0x1BE8 + i * 0x19D8 + 0x2AC;
        if (Bgn3Bindings::dCcD_GObjInf_ChkTgHit(sphere)) {
            hit.mpObj = Bgn3Bindings::dCcD_GObjInf_GetTgHitObj(sphere);
            hitAny = true;
        }
    }
    if (hitAny && static_cast<s16>(i_this->m0FDB6) == 0) {
        i_this->m0FDB6 = 10;
        Bgn3Bindings::def_se_set(a, static_cast<u32>(hit.mpObj), 0x44);
    }
}

// Preserve the original add's first-NaN payload when mocked vector output
// and the previous point both contain NaNs; ordinary C++ may commute operands.
static f32 singleAdd(f32 a, f32 b) {
    if (std::isnan(a))
        return std::bit_cast<f32>(std::bit_cast<u32>(a) | 0x00400000u);
    if (std::isnan(b))
        return std::bit_cast<f32>(std::bit_cast<u32>(b) | 0x00400000u);
    return (f32)((f64)a + (f64)b);
}

/* 02090420 */
void ke_move(u32 mat, u32 hair, s32 index, f32 length) {
    if (!static_cast<u8>(l_HIO.m3E))
        return;
    cXyz direction;
    cXyz out;
    direction.x = 0.0f;
    direction.y = 0.0f;
    direction.z = length;
    auto* hairs = gabi::at<bgn3_ke_s>(hair);
    cXyz* point = hairs->m00;
    for (int j = 4; j > 0; --j) {
        f32 step = REG_F(10, 1) + 0.1f;
        f32 curx = static_cast<f32>(point[1].x), vy = static_cast<f32>(hairs->m78.y),
            curz = static_cast<f32>(point[1].z), cury = static_cast<f32>(point[1].y);
        f32 gain = (f32)j * step;
        f32 prevz = static_cast<f32>(point->z);
        f32 yz = gabi::fmadds(vy, gain, cury);
        f32 bias = static_cast<f32>(l_HIO.m4C);
        f32 vz = static_cast<f32>(hairs->m78.z);
        f32 yy = yz + bias;
        f32 prevx = static_cast<f32>(point->x), prevy = static_cast<f32>(point->y);
        f32 dz = gabi::fmadds(vz, gain, curz - prevz);
        f32 dy = yy - prevy;
        f32 vx = static_cast<f32>(hairs->m78.x);
        f32 dx = gabi::fmadds(vx, gain, curx - prevx);
        s16 rx = (s16)(0u - cM_atan2s(dy, dz));
        f32 horizontal = Bgn3Bindings::sqrtf(gabi::fmadds(dy, dy, dz * dz));
        s16 ry = cM_atan2s(dx, horizontal);
        cMtx_XrotS(calc_mtx(), rx);
        cMtx_YrotM(calc_mtx(), ry);
        MtxPosition(&direction, &out);
        ++point;
        f32 ox = static_cast<f32>(out.x), px = static_cast<f32>(point[-1].x),
            py = static_cast<f32>(point[-1].y);
        point->x = singleAdd(px, ox);
        f32 oy = static_cast<f32>(out.y);
        f32 pz = static_cast<f32>(point[-1].z);
        point->y = singleAdd(py, oy);
        f32 oz = static_cast<f32>(out.z);
        point->z = singleAdd(pz, oz);
    }
    cXyz* dest = bgn3LinePositions(gabi::at<Bgn3LineMat0>(mat), index);
    for (int i = 0; i < 5; ++i) {
        u32 x = bgn3Word(hairs->m00[i].x);
        bgn3Word(dest[i].x) = x;
        u32 y = bgn3Word(hairs->m00[i].y);
        bgn3Word(dest[i].y) = y;
        u32 z = bgn3Word(hairs->m00[i].z);
        bgn3Word(dest[i].z) = z;
    }
}

/* inlined into caller */
void move_se_set(u32 a) {
    cXyz delta;
    cXyz output;
    Bgn3Bindings::cXyz_subtract(a + 0x314, gabi::ea(&delta), a + 0x300);
    delta.y = 0.0f;
    f32 square = PSVECSquareMag(&delta);
    f32 length = Bgn3Bindings::sqrtf(square);
    u32 volume = soundVolume(length * 3.5f);
    if (a && a + 0x37C)
        moveSound(a, 0x705A, volume);
    Bgn3Bindings::cXyz_subtract(a + 0x314, gabi::ea(&output), a + 0x300);
    bgn3Word(delta.y) = static_cast<u32>(bgn3Word(output.y));
    delta.x = 0.0f;
    delta.z = 0.0f;
    square = PSVECSquareMag(&delta);
    length = Bgn3Bindings::sqrtf(square);
    volume = soundVolume(singleAdd(length, length));
    if (a && a + 0x37C)
        moveSound(a, 0x705B, volume);
}

void hairRotations(int i, bool reverse) {
    s32 x = i * (REG0_S(4) + 1000);
    x &= REG0_S(5) + 0x3FFF;
    x -= REG0_S(6) + 0x2000;
    if (reverse)
        x = -x;
    cMtx_XrotM(calc_mtx(), (s16)x);
    s32 z = i * (REG0_S(7) + 5000);
    z &= REG0_S(8) + 0xFFF;
    z -= REG0_S(9) + 0x800;
    if (reverse)
        z = -z;
    cMtx_ZrotM(calc_mtx(), (s16)z);
}

/* inlined into caller */
void setModels(u32 a, bool moving) {
    auto* i_this = gabi::at<bgn3_class>(a);
    u32 s = a + 0x11E58;
    cXyz vector;
    cXyz world;
    Bgn3Bindings::mDoExt_McaMorf_play(static_cast<u32>(bgn3Word(i_this->mpMorf)), a + 0x314, 0, 0);
    Bgn3Bindings::mDoExt_baseAnm_play(static_cast<u32>(bgn3Word(i_this->mJyakutenCBrkAnm)));
    Bgn3Bindings::mDoExt_baseAnm_play(static_cast<u32>(bgn3Word(i_this->mJyakutenBBrkAnm)));
    f32 y = static_cast<f32>(i_this->current.pos.y);
    f32 x = static_cast<f32>(i_this->current.pos.x);
    f32 z = static_cast<f32>(i_this->current.pos.z);
    MtxTrans(x, y, z, 0);
    cMtx_YrotM(calc_mtx(), static_cast<s16>(i_this->shape_angle.y));
    cMtx_XrotM(calc_mtx(), static_cast<s16>(i_this->shape_angle.x));
    cMtx_ZrotM(calc_mtx(), static_cast<s16>(i_this->shape_angle.z));
    MtxPush();
    f32 size = singleAdd(REG_F(0, 10), 2.0f);
    u32 model = static_cast<u32>(bgn3Word(i_this->m002CC));
    MtxScale(size, size, size, 1);
    copyModelMatrix(model);
    vector.x = 0.0f;
    vector.y = 0.0f;
    vector.z = 0.0f;
    MtxPosition(&vector, &world);
    i_this->m0FF28.SetC(&world);
    i_this->m0FF28.SetR(singleAdd(REG_F(0, 1), 200.0f));
    Bgn3Bindings::cCcS_Set(dComIfGp_ea() + 0x26A4, s + 0x1AC);
    bool effect;
    s16 timer;
    if (static_cast<s16>(i_this->m0FD9E) == 23) {
        i_this->mArrowHitFlashTimer = 50;
        i_this->mArrowHitEffectTimer = 99;
        timer = 99;
        effect = true;
    } else {
        timer = static_cast<s16>(i_this->mArrowHitEffectTimer);
        effect = timer != 0;
        if (effect) {
            timer = (s16)(timer - 1);
            i_this->mArrowHitEffectTimer = timer;
        }
    }
    if (effect) {
        f32 factor = singleAdd(REG_F(8, 8), 0.001f);
        f32 scale = (100.0f * (f32)timer) * factor;
        if (static_cast<u32>(bgn3Word(i_this->mpArrowHitEmitter1)) == 0)
            bgn3Word(i_this->mpArrowHitEmitter1) = movementParticle(gabi::ea(&world), 0x3ED);
        else
            emitterMove(s + 0x24, gabi::ea(&world), scale);
        if (static_cast<u32>(bgn3Word(i_this->mpArrowHitEmitter2)) == 0) {
            vector.y = scale;
            vector.z = scale;
            vector.x = scale;
            bgn3Word(i_this->mpArrowHitEmitter2) = movementParticle(gabi::ea(&world), 0x3EE);
        } else {
            emitterMove(s + 0x28, gabi::ea(&world), scale);
        }
    } else {
        retireEmitter(s + 0x24);
        retireEmitter(s + 0x28);
    }
    MtxPull();
    for (int i = 0; i < 40; ++i) {
        MtxPush();
        hairRotations(i, true);
        vector.y = singleAdd(REG_F(13, 13), 100.0f);
        u32 hair = a + 0x538 + i * 0x84;
        MtxPosition(&vector, &gabi::at<bgn3_ke_s>(hair)->m00[0]);
        vector.y = static_cast<f32>(l_HIO.m40);
        MtxPosition(&vector, &gabi::at<bgn3_ke_s>(hair)->m78);
        Bgn3Bindings::PSVECSubtract(hair + 0x78, gabi::ea(&world), hair + 0x78);
        f32 length = static_cast<f32>(l_HIO.m44);
        ke_move(a + 0x19D8, hair, i, singleAdd(length, length));
        MtxPull();
    }
    MtxTrans(0.0f, 0.0f, singleAdd(REG_F(0, 2), 250.0f), 1);
    s16 shake = static_cast<s16>(i_this->m100A8);
    if (shake) {
        shake = (s16)(shake - 1);
        i_this->m100A8 = shake;
    }
    f32 magnitude = (f32)shake * singleAdd(REG_F(0, 14), 500.0f);
    s16 count = static_cast<s16>(i_this->m0FD88);
    f32 sine = tableSin(0x104A44F8, (s16)(count * 0x2100));
    s16 yaw = (s16)gabi::ftoi(sine * magnitude);
    f32 cosine = tableCos(0x104A44F8, (s16)(count * 0x2300));
    s16 pitch = (s16)gabi::ftoi(cosine * magnitude);
    cMtx_YrotM(calc_mtx(), yaw);
    cMtx_XrotM(calc_mtx(), pitch);
    copyModelMatrix(gabi::ea(i_this->mpMorf->getModel()));
    vector.x = 0.0f;
    vector.z = 0.0f;
    vector.y = 0.0f;
    MtxPosition(&vector, &world);
    i_this->m0FDFC.SetC(&world);
    i_this->m0FDFC.SetR(singleAdd(REG_F(0, 0), 150.0f));
    Bgn3Bindings::cCcS_Set(dComIfGp_ea() + 0x26A4, s + 0x80);
    u32 ex = static_cast<u32>(bgn3Word(world.x));
    bgn3Word(i_this->eyePos.x) = ex;
    u32 ey = static_cast<u32>(bgn3Word(world.y));
    bgn3Word(i_this->eyePos.y) = ey;
    u32 ez = static_cast<u32>(bgn3Word(world.z));
    bgn3Word(i_this->eyePos.z) = ez;
    bgn3Word(i_this->attention_info.position.x) = ex;
    bgn3Word(i_this->attention_info.position.y) = ey;
    bgn3Word(i_this->attention_info.position.z) = ez;
    f32 eyeHeight = singleAdd(REG_F(0, 7), 100.0f);
    i_this->attention_info.position.y =
        singleAdd(static_cast<f32>(i_this->attention_info.position.y), eyeHeight);
    Bgn3Bindings::mDoExt_McaMorf_calc(static_cast<u32>(bgn3Word(i_this->mpMorf)));
    s16 hit = static_cast<s16>(i_this->m0FD9E);
    if (hit) {
        hit = (s16)(hit + 1);
        i_this->m0FD9E = hit > 100 ? 0 : hit;
    }
    for (int i = 0; i < 9; ++i) {
        u32 part = a + 0x1BE8 + i * 0x19D8;
        x = static_cast<f32>(bgn3Part(part)->m00D4.x);
        z = static_cast<f32>(bgn3Part(part)->m00D4.z);
        y = static_cast<f32>(bgn3Part(part)->m00D4.y);
        MtxTrans(x, y, z, 0);
        cMtx_YrotM(calc_mtx(), static_cast<s16>(bgn3Part(part)->m00E2));
        cMtx_XrotM(calc_mtx(), static_cast<s16>(bgn3Part(part)->m00E0));
        MtxPush();
        size = static_cast<f32>(bgn3Part(part)->m00F4);
        MtxScale(size, size, size, 1);
        vector.x = 0.0f;
        vector.y = 0.0f;
        vector.z = 0.0f;
        MtxPosition(&vector, &world);
        if (i == 8) {
            s8 health = static_cast<s8>(i_this->health);
            model = health == 3   ? static_cast<u32>(bgn3Word(i_this->mpJyakutenAModel))
                    : health == 2 ? static_cast<u32>(bgn3Word(i_this->mpJyakutenBModel))
                    : health == 1 ? static_cast<u32>(bgn3Word(i_this->mpJyakutenCModel))
                                  : 0;
            bgn3Word(bgn3Part(part)->mpPart3Model) = model;
            bgn3Word(i_this->m10054.x) = static_cast<u32>(bgn3Word(world.x));
            bgn3Word(i_this->m10054.y) = static_cast<u32>(bgn3Word(world.y));
            bgn3Word(i_this->m10054.z) = static_cast<u32>(bgn3Word(world.z));
        }
        model = static_cast<u32>(bgn3Word(bgn3Part(part)->mpPart3Model));
        if (model)
            copyModelMatrix(model, true);
        bgn3Part(part)->mPart3Sph.SetC(&world);
        f32 radius = singleAdd(REG_F(0, 12), 100.0f);
        radius = static_cast<f32>(bgn3Part(part)->m00F4) * radius;
        bgn3Part(part)->mPart3Sph.SetR(radius);
        u32 flags = static_cast<u32>(bgn3Part(part)->mPart3Sph.mObjAt.mSPrm);
        bgn3Part(part)->mPart3Sph.mObjAt.mSPrm = moving ? flags | 1 : flags & ~1u;
        Bgn3Bindings::cCcS_Set(dComIfGp_ea() + 0x26A4, part + 0x2AC);
        MtxPull();
        if (i < 7 && static_cast<u8>(l_HIO.m3E)) {
            for (int j = 0; j < ke_max[i]; ++j) {
                MtxPush();
                hairRotations(j, false);
                f32 unusedHeight = singleAdd(REG_F(13, 13), 100.0f);
                vector.y = unusedHeight;
                f32 hairLength = singleAdd(REG_F(13, 12), 60.0f);
                vector.y = static_cast<f32>(bgn3Part(part)->m00F4) * hairLength;
                u32 hair = part + 0x3F0 + j * 0x84;
                MtxPosition(&vector, &gabi::at<bgn3_ke_s>(hair)->m00[0]);
                vector.y = static_cast<f32>(l_HIO.m40);
                MtxPosition(&vector, &gabi::at<bgn3_ke_s>(hair)->m78);
                Bgn3Bindings::PSVECSubtract(hair + 0x78, gabi::ea(&world), hair + 0x78);
                ke_move(part + 0x1890, hair, j,
                        static_cast<f32>(bgn3Part(part)->m00F4) * static_cast<f32>(l_HIO.m44));
                MtxPull();
            }
        }
        if (static_cast<s16>(i_this->m0FD9E) == fl_check_d[i]) {
            bgn3Part(part)->mPart3ArrowHitFlashTimer = 50;
            bgn3Part(part)->mPart3ArrowHitEffectTimer = 100;
        }
        s16 flash = static_cast<s16>(bgn3Part(part)->mPart3ArrowHitFlashTimer);
        s16 timer = static_cast<s16>(bgn3Part(part)->mPart3ArrowHitEffectTimer);
        if (flash)
            bgn3Part(part)->mPart3ArrowHitFlashTimer = (s16)(flash - 1);
        if (timer) {
            timer = (s16)(timer - 1);
            bgn3Part(part)->mPart3ArrowHitEffectTimer = timer;
            f32 factor = singleAdd(REG_F(8, 0), 0.04f);
            // Both products keep the original operand order (first-NaN payload).
            f32 scale = gabi::fmuls_ppc(
                gabi::fmuls_ppc(static_cast<f32>(bgn3Part(part)->m00F4), (f32)timer), factor);
            if (static_cast<u32>(bgn3Word(bgn3Part(part)->mpPart3ArrowHitEmitter1)) == 0)
                bgn3Word(bgn3Part(part)->mpPart3ArrowHitEmitter1) =
                    movementParticle(part + 0x288, 0x3ED);
            else
                emitterMove(part + 0x3E4, part + 0x288, scale);
            if (static_cast<u32>(bgn3Word(bgn3Part(part)->mpPart3ArrowHitEmitter2)) == 0) {
                vector.y = scale;
                vector.z = scale;
                vector.x = scale;
                bgn3Word(bgn3Part(part)->mpPart3ArrowHitEmitter2) =
                    movementParticle(part + 0x288, 0x3EE);
            } else {
                emitterMove(part + 0x3E8, part + 0x288, scale);
            }
        } else {
            retireEmitter(part + 0x3E4);
            retireEmitter(part + 0x3E8);
        }
    }
    u32 first = static_cast<u32>(i_this->m0FDFC.mObjAt.mSPrm);
    u32 second = static_cast<u32>(i_this->m0FF28.mObjAt.mSPrm);
    i_this->m0FDFC.mObjAt.mSPrm = moving ? first | 1 : first & ~1u;
    i_this->m0FF28.mObjAt.mSPrm = moving ? second | 1 : second & ~1u;
    model = static_cast<u32>(bgn3Word(i_this->m002CC));
    PSMTXCopy(J3DModel_getBaseTRMtx(gabi::at<J3DModel>(model)), calc_mtx());
    vector.x = 0.0f;
    vector.y = 0.0f;
    vector.z = 0.0f;
    MtxPosition(&vector, &world);
    cLib_addCalc0(&i_this->m100A0, 1.0f, 25.0f);
    u32 packet = i_this->mRedRopeMat.mpLines;
    auto* line = gabi::at<Bgn3RopeLineRecord>(packet);
    cXyz* points = line->mpPositions;
    be<u8>* sizes = line->mpSizes;
    for (int i = 0; i < 60; ++i) {
        u32 angle = Bgn3Bindings::cM_rad2s((f32)i * 0.05324733629822731f) & 65535;
        f32 amplitude = cM_ssin(angle) * static_cast<f32>(i_this->m100A0);
        s16 frequency = REG0_S(3);
        s16 counter = static_cast<s16>(i_this->m0FD88);
        s16 stagger = REG0_S(4);
        u32 xAngle = (u32)(counter * (frequency + 300) + i * (stagger + 2000));
        s16 zStagger = REG0_S(6);
        s16 zFrequency = REG0_S(5);
        u32 zAngle = (u32)(counter * (zFrequency + 250) + i * (zStagger + 2000));
        f32 attenuation = (f32)(59 - i) * 0.01666666939854622f;
        amplitude = amplitude * attenuation;
        f32 vx = cM_ssin(xAngle) * amplitude;
        f32 vz = cM_ssin(zAngle) * amplitude;
        vector.y = 0.0f;
        vector.x = vx;
        vector.z = vz;
        cXyz result;
        Bgn3Bindings::cXyz_add(gabi::ea(&world), gabi::ea(&result), gabi::ea(&vector));
        bgn3Word(points[i].x) = static_cast<u32>(bgn3Word(result.x));
        bgn3Word(points[i].y) = static_cast<u32>(bgn3Word(result.y));
        bgn3Word(points[i].z) = static_cast<u32>(bgn3Word(result.z));
        sizes[i] = REG0_S(3) + 10;
        world.y = singleAdd(static_cast<f32>(world.y), 80.0f);
    }
}

/* 02091730 */
void move(u32 a) {
    auto* i_this = gabi::at<bgn3_class>(a);
    dComIfGp_ea();
    bool moving = false;
    if (static_cast<u8>(l_HIO.m05) == 0) {
        switch (static_cast<u16>(i_this->m0FD8A)) {
        case 0:
            moving = move0(a);
            break;
        case 1:
            mahi(a);
            break;
        case 2:
            damage(a);
            break;
        case 3:
            end(a);
            break;
        default:
            break;
        }
    }
    splash(a, 0x8443, 0x8407, true);
    dComIfGp_ea();
    damage_check(a);
    part_control(a);
    setModels(a, moving);
}

/* 02090624 */
u32 ki_c_sub(u32 a, u32 ignored) {
    auto* actor = gabi::at<fopAc_ac_c>(a);
    if (fopAc_IsActor(actor) && a && fpcM_GetName(actor) == fpcNm_KS_e)
        ki_all_count = static_cast<u32>(ki_all_count) + 1;
    return 0;
}

/* 02090680 */
bool daBgn3_Execute(u32 a) {
    auto* i_this = gabi::at<bgn3_class>(a);
    dComIfGp_ea();
    u32 boss = Bgn3Bindings::fpcM_Search(gabi::guest_fn((const void*)&bgn_s_sub), a);
    bgn = gabi::at<Bgn3BossView>(boss);
    if (!boss)
        return true;
    u8 dbg = static_cast<u8>(l_HIO.m06);
    if (dbg)
        i_this->health = dbg;
    u32 bait = Bgn3Bindings::fpcM_Search(gabi::guest_fn((const void*)&esa_s_sub), a);
    boss = gabi::ea(bgn);
    esa = gabi::at<fopAc_ac_c>(bait);
    if (static_cast<s8>(gabi::at<Bgn3BossView>(boss)->m02B5) != 2) {
        i_this->m0FD8A = 10;
        i_this->current.pos.z = 0.0f;
        i_this->attention_info.flags = 0;
        i_this->current.pos.x = 0.0f;
        i_this->current.pos.y = 30000.0f;
        return true;
    }
    if (static_cast<s16>(i_this->m0FD8A) == 10) {
        u32 z = static_cast<u32>(bgn3Word(i_this->home.pos.z)),
            x = static_cast<u32>(bgn3Word(i_this->home.pos.x));
        bgn3Word(i_this->current.pos.z) = z;
        u32 y = static_cast<u32>(bgn3Word(i_this->home.pos.y));
        bgn3Word(i_this->current.pos.x) = x;
        bgn3Word(i_this->current.pos.y) = y;
        i_this->m0FD8A = 0;
        i_this->m0FDB0 = 180;
        i_this->m0FD90 = 180;
    }
    i_this->attention_info.flags = 4;
    cLib_addCalc2(&i_this->m10060, 0.0f, 1.0f, 0.01f);
    s16 counter = static_cast<s16>(i_this->m0FD88);
    i_this->m0FD88 = counter + 1;
    i_this->attention_info.distances[2] = 0x22;
    // HD retains the five-halfword timer sweep across the following timer fields.
    auto* timers = gabi::at<Bgn3TimerView>(gabi::ea(i_this->m0FDAA));
    for (int i = 0; i < 5; ++i) {
        s16 n = timers->mTimers[i];
        if (n)
            timers->mTimers[i] = n - 1;
    }
    s16 t = static_cast<s16>(i_this->m0FDB4);
    if (t)
        i_this->m0FDB4 = t - 1;
    t = static_cast<s16>(i_this->m0FDB6);
    s16 first = static_cast<s16>(i_this->m0FD94);
    if (t)
        i_this->m0FDB6 = t - 1;
    s16 second = static_cast<s16>(i_this->mArrowHitFlashTimer);
    if (first)
        i_this->m0FD94 = first - 1;
    if (second)
        i_this->mArrowHitFlashTimer = second - 1;
    move(a);
    u32 count = static_cast<u32>(i_this->m100AC);
    if (count) {
        i_this->m100AC = count - 1;
        ki_all_count = 0;
        Bgn3Bindings::fpcM_Search(gabi::guest_fn((const void*)&ki_c_sub), a);
        if (static_cast<s32>(ki_all_count) < 20) {
            cXyz pos;
            pos.x = cM_rndFX(2500.0f);
            pos.y = cM_rndF(500.0f) + 3500.0f;
            pos.z = cM_rndFX(2500.0f);
            Bgn3Bindings::fopAcM_create(0xCD, 3, gabi::ea(&pos),
                                        static_cast<s8>(i_this->current.roomNo), 0, 0, -1, 0);
        }
    }
    return true;
}

/* 02090908 */
bool daBgn3_IsDelete(u32 a) { return true; }

/* 02090910 */
bool daBgn3_Delete(u32 a) {
    auto* i_this = gabi::at<bgn3_class>(a);
    dComIfG_resDelete(&i_this->mPhase, "Bgn");
    if (i_this->m100B0) {
        s8 n = l_HIO.mNo;
        hio_set = 0;
        mDoHIO_deleteChild(n);
    }
    mDoAud_seDeleteObject(&i_this->m10054);
    return true;
}

static u32 resource(s32 index) { return gabi::ea(dComIfG_getObjectRes("Bgn", index)); }

/* 02090984 */
bool useHeapInit(u32 a) {
    auto* i_this = gabi::at<bgn3_class>(a);
    u32 head = resource(dRes_INDEX_BGN_BDL_BGN_HEAD1_e),
        anim = resource(dRes_INDEX_BGN_BCK_BGN_HEAD1_e);
    u32 morf =
        Bgn3Bindings::mDoExt_McaMorf_ct(0, head, 0, 0, anim, 2, 0, -1, 1, 0, 0, 0x11020203u, 1.0f);
    bgn3Word(i_this->mpMorf) = morf;
    if (!morf || !gabi::at<mDoExt_McaMorf>(morf)->getModel())
        return false;
    Bgn3Bindings::mDoExt_J3DModelPacketS_setMaterial(a + 0x3D4, 0);
    u32 partData = resource(dRes_INDEX_BGN_BDL_BGN_DEKU1_e);
    u32 model = Bgn3Bindings::mDoExt_J3DModel_create(partData, 0, 0x11020203u);
    bgn3Word(i_this->m002CC) = model;
    if (!model)
        return false;
    Bgn3Bindings::mDoExt_J3DModelPacketS_setMaterial(a + 0x488, 0);
    if (!Bgn3Bindings::mDoExt_3DlineMat0_init(a + 0x19D8, 40, 5, 0))
        return false;
    for (int i = 0; i < 9; ++i) {
        u32 part = a + 0x1BE8 + 0x19D8 * i;
        model = Bgn3Bindings::mDoExt_J3DModel_create(partData, 0, 0x11020203u);
        bgn3Word(bgn3Part(part)->mpPart3Model) = model;
        if (!model)
            return false;
        Bgn3Bindings::mDoExt_J3DModelPacketS_setMaterial(part + 4, 0);
        if (i < 7 &&
            !Bgn3Bindings::mDoExt_3DlineMat0_init(part + 0x1890, static_cast<u16>(ke_max[i]), 5, 0))
            return false;
    }
    model = Bgn3Bindings::mDoExt_J3DModel_create(resource(dRes_INDEX_BGN_BDL_BGN_JYAKUTENA3_e), 0,
                                                 0x11020203u);
    bgn3Word(i_this->mpJyakutenAModel) = model;
    if (!model)
        return false;
    u32 weakB = resource(dRes_INDEX_BGN_BDL_BGN_JYAKUTENB3_e);
    model = Bgn3Bindings::mDoExt_J3DModel_create(weakB, 0, 0x11020203u);
    bgn3Word(i_this->mpJyakutenBModel) = model;
    if (!model)
        return false;
    u32 brk = Bgn3Bindings::operator_new(0x78);
    if (brk)
        brk = Bgn3Bindings::mDoExt_brkAnm_ct(brk);
    bgn3Word(i_this->mJyakutenBBrkAnm) = brk;
    if (!brk)
        return false;
    anim = resource(dRes_INDEX_BGN_BRK_BGN_JYAKUTENB3_e);
    if (!Bgn3Bindings::mDoExt_brkAnm_init(static_cast<u32>(bgn3Word(i_this->mJyakutenBBrkAnm)),
                                          weakB, anim, 1, 2, 0, -1, 0, 0, 1.0f))
        return false;
    u32 weakA = resource(dRes_INDEX_BGN_BDL_BGN_JYAKUTENC3_e);
    model = Bgn3Bindings::mDoExt_J3DModel_create(weakA, 0, 0x11020203u);
    bgn3Word(i_this->mpJyakutenCModel) = model;
    if (!model)
        return false;
    brk = Bgn3Bindings::operator_new(0x78);
    if (brk)
        brk = Bgn3Bindings::mDoExt_brkAnm_ct(brk);
    bgn3Word(i_this->mJyakutenCBrkAnm) = brk;
    if (!brk)
        return false;
    anim = resource(dRes_INDEX_BGN_BRK_BGN_JYAKUTENC3_e);
    if (!Bgn3Bindings::mDoExt_brkAnm_init(static_cast<u32>(bgn3Word(i_this->mJyakutenCBrkAnm)),
                                          weakA, anim, 1, 2, 0, -1, 0, 0, 1.0f))
        return false;
    Bgn3Bindings::mDoExt_J3DModelPacketS_setMaterial(a + 0x1B20, 0);
    u32 rope = resource(dRes_INDEX_BGN_BTI_NOT_CUT1_e);
    if (!Bgn3Bindings::mDoExt_3DlineMat1_init(a + 0x12140, 1, 60, rope, 1))
        return false;
    Bgn3Bindings::mDoExt_J3DModelPacketS_setMaterial(a + 0x122CC, 0);
    return true;
}

static void copyLightInit(Bgn3LightInit* dest, const Bgn3LightInit* src) {
    for (int i = 0; i < 6; ++i)
        dest->mPositionDirection[i] = static_cast<f32>(src->mPositionDirection[i]);
    for (int i = 0; i < 4; ++i)
        dest->mColorBytes[i] = static_cast<u8>(src->mColorBytes[i]);
    for (int i = 0; i < 4; ++i)
        dest->mColorWords[i] = static_cast<s16>(src->mColorWords[i]);
    for (int i = 0; i < 8; ++i)
        dest->mParameters[i] = static_cast<f32>(src->mParameters[i]);
}

static void copyActorTev(u32 a, u32 destination) {
    auto* actor = gabi::at<bgn3_class>(a);
    auto* src = gabi::at<Bgn3TevCopyView>(gabi::ea(&actor->tevStr));
    auto* dest = gabi::at<Bgn3TevCopyView>(destination);
    copyLightInit(&dest->mLight, &src->mLight);
    // GHS copies these positions and packed colours as words, preserving NaN payloads.
    bgn3Word(dest->mLightPosWorld.x) = static_cast<u32>(bgn3Word(src->mLightPosWorld.x));
    bgn3Word(dest->mLightPosWorld.y) = static_cast<u32>(bgn3Word(src->mLightPosWorld.y));
    bgn3Word(dest->mLightPosWorld.z) = static_cast<u32>(bgn3Word(src->mLightPosWorld.z));
    for (int i = 0; i < 4; ++i)
        dest->mColorC0[i] = static_cast<u16>(src->mColorC0[i]);
    dest->mColorK0 = static_cast<u32>(src->mColorK0);
    dest->mColorK1 = static_cast<u32>(src->mColorK1);
    for (int i = 0; i < 4; ++i)
        dest->mFogColor[i] = static_cast<u16>(src->mFogColor[i]);
    for (int i = 0; i < 3; ++i)
        dest->mFogValues[i] = static_cast<f32>(src->mFogValues[i]);
    for (int i = 0; i < 9; ++i)
        dest->mEnvironment[i] = static_cast<u8>(src->mEnvironment[i]);
    copyLightInit(&dest->mLightCopy1, &src->mLightCopy1);
    copyLightInit(&dest->mLightCopy2, &src->mLightCopy2);
}

/* 02091008 */
s32 daBgn3_Create(u32 a) {
    auto* i_this = gabi::at<bgn3_class>(a);
    u32 flags = static_cast<u32>(i_this->actor_condition);
    if (!(flags & fopAcCnd_INIT_e)) {
        if (a) {
            actorCtor(a);
            flags = static_cast<u32>(i_this->actor_condition);
        }
        i_this->actor_condition = flags | fopAcCnd_INIT_e;
    }
    s32 phase = dComIfG_resLoad(&i_this->mPhase, "Bgn");
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    if (!Bgn3Bindings::fopAcM_entrySolidHeap(a, gabi::guest_fn((const void*)&useHeapInit), 0x96000))
        return cPhs_ERROR_e;
    if (!static_cast<u8>(hio_set)) {
        i_this->m100B0 = 1;
        hio_set = 1;
        l_HIO.mNo =
            Bgn3Bindings::mDoHIO_createChild(gabi::guest_addr("Ｇ（ハヤムシ）"), gabi::ea(&l_HIO));
    }
    u32 st = a + 0x11E9C;
    Bgn3Bindings::dCcD_Stts_Init(st, 255, 255, a);
    Bgn3Bindings::dCcD_Sph_Set(a + 0x12004, 0x10191208u);
    u32 bits = static_cast<u32>(i_this->m0FF28.mObjAt.mSPrm);
    bgn3Word(i_this->m0FF28.mStts) = st;
    i_this->m0FF28.mObjAt.mSPrm = bits & ~1u;
    Bgn3Bindings::dCcD_Sph_Set(a + 0x11ED8, 0x10191208u);
    bits = static_cast<u32>(i_this->m0FDFC.mObjAt.mSPrm);
    bgn3Word(i_this->m0FDFC.mStts) = st;
    i_this->m0FDFC.mObjAt.mSPrm = bits & ~1u;
    for (int i = 0; i < 9; ++i) {
        u32 part = a + 0x1BE8 + i * 0x19D8;
        Bgn3Bindings::dCcD_Sph_Set(part + 0x2AC, i == 8 ? 0x10191248u : 0x10191208u);
        bits = static_cast<u32>(bgn3Part(part)->mPart3Sph.mObjAt.mSPrm);
        bgn3Word(bgn3Part(part)->mPart3Sph.mStts) = st;
        bgn3Part(part)->mPart3Sph.mObjAt.mSPrm = bits & ~1u;
    }
    i_this->max_health = 3;
    i_this->health = 3;
    for (int i = 0; i < 9; ++i)
        copyActorTev(a, a + 0x1BE8 + 0x19D8 * i + 0xB4);
    daBgn3_Execute(a);
    return phase;
}

// HD: compiler-generated and template code of this translation unit.

/* 02091704 */
f32 tableSin(u32 a, u32 angle) { return gabi::at<Bgn3SinCos>(a)[(angle & 65535) >> 3].mSin; }

/* 02091718 */
f32 tableCos(u32 a, u32 angle) { return gabi::at<Bgn3SinCos>(a)[(angle & 65535) >> 3].mCos; }

/* 02094448 */
void hioDtor(u32 a, u32 flags) {
    if (a && (flags & 1))
        Bgn3Bindings::operator_delete(a);
}

/* 0209445C */
void partDtor(u32 a, u32 flags) {
    if (a) {
        Bgn3Bindings::mDoExt_3DlineMat0_dt(a + 0x1890, 2);
        Bgn3Bindings::dCcD_Sph_dt(a + 0x2AC, 2);
        Bgn3Bindings::mDoExt_J3DModelPacketS_dt(a + 4, 2);
        if (flags & 1)
            Bgn3Bindings::operator_delete(a);
    }
}

/* 020944C8 */
void actorDtor(u32 a, u32 flags) {
    if (a) {
        Bgn3Bindings::mDoExt_J3DModelPacketS_dt(a + 0x122CC, 2);
        Bgn3Bindings::mDoExt_3DlineMat1_dt(a + 0x12140, 2);
        Bgn3Bindings::dCcD_Sph_dt(a + 0x12004, 2);
        Bgn3Bindings::dCcD_Sph_dt(a + 0x11ED8, 2);
        Bgn3Bindings::dCcD_Stts_dt(a + 0x11E9C, 2);
        Bgn3Bindings::destroy_array(a + 0x1BE8, 10, 0x19D8, gabi::guest_fn((const void*)&partDtor),
                                    0, 0);
        Bgn3Bindings::mDoExt_J3DModelPacketS_dt(a + 0x1B20, 2);
        Bgn3Bindings::mDoExt_3DlineMat0_dt(a + 0x19D8, 2);
        Bgn3Bindings::mDoExt_J3DModelPacketS_dt(a + 0x488, 2);
        Bgn3Bindings::mDoExt_J3DModelPacketS_dt(a + 0x3D4, 2);
        Bgn3Bindings::fopAc_ac_c_dt(a, 0);
        if (flags & 1)
            Bgn3Bindings::operator_delete(a);
    }
}

/* 020945C8 */
void genMessage(u32 a, u32 context) {}

/* 02091648 */
void staticInit() {
    headerStatics.mZeroWords[2] = 0;
    headerStatics.mZeroWords[0] = 0;
    headerStatics.mZeroWords[3] = 0;
    headerStatics.mZeroWords[1] = 0;
    Bgn3Bindings::register_global_object(0x10191288u);
    f32 low = static_cast<f32>(piConstants.mNegativePi), high = static_cast<f32>(piConstants.mPi);
    headerStatics.mNegativePi = low;
    headerStatics.mPi = high;
    Bgn3Bindings::std_char_traits_ct(gabi::ea(&headerStatics.mFlag));
    Bgn3Bindings::register_global_object(0x10191294u);
    Bgn3Bindings::std_allocator_ct(gabi::ea(&headerStatics.mByte));
    Bgn3Bindings::register_global_object(0x101912A0u);
    zero.z = 0.0f;
    zero.x = 0.0f;
    zero.y = 0.0f;
    hioCtor(gabi::ea(&l_HIO));
}

/* 02090D4C */
u32 partCtor(u32 a) {
    if (!a)
        a = Bgn3Bindings::operator_new(0x19D8);
    if (!a)
        return 0;
    auto* part = gabi::at<part_s3>(a);
    auto* tev = gabi::at<Bgn3TevCopyView>(gabi::ea(&part->mPart3TevStr));
    Bgn3Bindings::mDoExt_J3DModelPacketS_ct(a + 4);
    // The constructor retains a shared default tev snapshot for three blocks.
    f32 tev00 = static_cast<f32>(defaultLightInit.mPositionDirection[0]);
    tev->mLight.mPositionDirection[0] = tev00;
    f32 tev04 = static_cast<f32>(defaultLightInit.mPositionDirection[1]);
    tev->mLight.mPositionDirection[1] = tev04;
    f32 tev08 = static_cast<f32>(defaultLightInit.mPositionDirection[2]);
    tev->mLight.mPositionDirection[2] = tev08;
    f32 tev0C = static_cast<f32>(defaultLightInit.mPositionDirection[3]);
    tev->mLight.mPositionDirection[3] = tev0C;
    f32 tev10 = static_cast<f32>(defaultLightInit.mPositionDirection[4]);
    tev->mLight.mPositionDirection[4] = tev10;
    f32 tev14 = static_cast<f32>(defaultLightInit.mPositionDirection[5]);
    tev->mLight.mPositionDirection[5] = tev14;
    u8 tev18 = static_cast<u8>(defaultLightInit.mColorBytes[0]);
    tev->mLight.mColorBytes[0] = tev18;
    u8 tev19 = static_cast<u8>(defaultLightInit.mColorBytes[1]);
    tev->mLight.mColorBytes[1] = tev19;
    u8 tev1A = static_cast<u8>(defaultLightInit.mColorBytes[2]);
    tev->mLight.mColorBytes[2] = tev1A;
    u8 tev1B = static_cast<u8>(defaultLightInit.mColorBytes[3]);
    tev->mLight.mColorBytes[3] = tev1B;
    s16 tev1C = static_cast<s16>(defaultLightInit.mColorWords[0]);
    tev->mLight.mColorWords[0] = tev1C;
    s16 tev1E = static_cast<s16>(defaultLightInit.mColorWords[1]);
    tev->mLight.mColorWords[1] = tev1E;
    s16 tev20 = static_cast<s16>(defaultLightInit.mColorWords[2]);
    tev->mLight.mColorWords[2] = tev20;
    s16 tev22 = static_cast<s16>(defaultLightInit.mColorWords[3]);
    tev->mLight.mColorWords[3] = tev22;
    f32 tev24 = static_cast<f32>(defaultLightInit.mParameters[0]);
    tev->mLight.mParameters[0] = tev24;
    f32 tev28 = static_cast<f32>(defaultLightInit.mParameters[1]);
    tev->mLight.mParameters[1] = tev28;
    f32 tev2C = static_cast<f32>(defaultLightInit.mParameters[2]);
    tev->mLight.mParameters[2] = tev2C;
    f32 tev30 = static_cast<f32>(defaultLightInit.mParameters[3]);
    tev->mLight.mParameters[3] = tev30;
    f32 tev34 = static_cast<f32>(defaultLightInit.mParameters[4]);
    tev->mLight.mParameters[4] = tev34;
    f32 tev38 = static_cast<f32>(defaultLightInit.mParameters[5]);
    tev->mLight.mParameters[5] = tev38;
    f32 tev3C = static_cast<f32>(defaultLightInit.mParameters[6]);
    tev->mLight.mParameters[6] = tev3C;
    f32 tev40 = static_cast<f32>(defaultLightInit.mParameters[7]);
    tev->mLightCopy1.mColorBytes[0] = tev18;
    tev->mLightCopy1.mParameters[7] = tev40;
    tev->mLightCopy1.mColorBytes[2] = tev1A;
    tev->mLightCopy2.mColorBytes[2] = tev1A;
    tev->mLightCopy1.mPositionDirection[2] = tev08;
    tev->mLightCopy1.mPositionDirection[4] = tev10;
    tev->mLightCopy1.mColorBytes[3] = tev1B;
    tev->mLightCopy1.mParameters[3] = tev30;
    tev->mLightCopy1.mParameters[0] = tev24;
    tev->mLightCopy1.mPositionDirection[3] = tev0C;
    tev->mLightCopy2.mPositionDirection[4] = tev10;
    tev->mLightCopy1.mColorBytes[1] = tev19;
    tev->mLightCopy1.mParameters[4] = tev34;
    tev->mLightCopy2.mPositionDirection[1] = tev04;
    tev->mLightCopy2.mPositionDirection[3] = tev0C;
    tev->mLightCopy1.mParameters[1] = tev28;
    tev->mLightCopy2.mColorBytes[1] = tev19;
    tev->mLightCopy2.mPositionDirection[5] = tev14;
    tev->mLightCopy1.mPositionDirection[1] = tev04;
    tev->mLightCopy2.mColorBytes[3] = tev1B;
    tev->mLightCopy1.mColorWords[0] = tev1C;
    tev->mLightCopy2.mPositionDirection[2] = tev08;
    tev->mLightCopy2.mPositionDirection[0] = tev00;
    tev->mLightCopy1.mParameters[2] = tev2C;
    tev->mLightCopy1.mPositionDirection[5] = tev14;
    tev->mLightCopy1.mParameters[6] = tev3C;
    tev->mLightCopy1.mPositionDirection[0] = tev00;
    tev->mLight.mParameters[7] = tev40;
    tev->mLightCopy1.mParameters[5] = tev38;
    tev->mLightCopy1.mColorWords[2] = tev20;
    tev->mLightCopy1.mColorWords[1] = tev1E;
    tev->mLightCopy2.mColorWords[0] = tev1C;
    tev->mLightCopy2.mColorBytes[0] = tev18;
    tev->mLightCopy1.mColorWords[3] = tev22;
    tev->mLightCopy2.mColorWords[1] = tev1E;
    tev->mLightCopy2.mColorWords[2] = tev20;
    tev->mLightCopy2.mColorWords[3] = tev22;
    tev->mLightCopy2.mParameters[0] = tev24;
    tev->mLightCopy2.mParameters[1] = tev28;
    tev->mLightCopy2.mParameters[2] = tev2C;
    tev->mLightCopy2.mParameters[3] = tev30;
    tev->mLightCopy2.mParameters[4] = tev34;
    tev->mLightCopy2.mParameters[5] = tev38;
    tev->mLightCopy2.mParameters[6] = tev3C;
    tev->mLightCopy2.mParameters[7] = tev40;
    Bgn3Bindings::dCcD_Sph_ct(a + 0x2AC);
    Bgn3Bindings::mDoExt_3DlineMat0_ct(a + 0x1890);
    return a;
}

/* 02090F08 */
u32 actorCtor(u32 a) {
    if (!a)
        a = Bgn3Bindings::operator_new(0x1238C);
    if (!a)
        return 0;
    auto* i_this = gabi::at<bgn3_class>(a);
    Bgn3Bindings::fopAc_ac_c_ct(a);
    i_this->__vtbl = 0x10009004;
    Bgn3Bindings::mDoExt_J3DModelPacketS_ct(a + 0x3D4);
    Bgn3Bindings::mDoExt_J3DModelPacketS_ct(a + 0x488);
    Bgn3Bindings::mDoExt_3DlineMat0_ct(a + 0x19D8);
    Bgn3Bindings::mDoExt_J3DModelPacketS_ct(a + 0x1B20);
    Bgn3Bindings::construct_array(a + 0x1BE8, 10, 0x19D8, gabi::guest_fn((const void*)&partCtor));
    Bgn3Bindings::cCcD_Stts_ct(a + 0x11E9C);
    Bgn3Bindings::dCcD_GStts_ct(a + 0x11EB8);
    i_this->mStts.__vtbl = 0x1004AE88;
    i_this->mStts.__vtbl_gstts = 0x1004AEC0;
    Bgn3Bindings::dCcD_Sph_ct(a + 0x11ED8);
    Bgn3Bindings::dCcD_Sph_ct(a + 0x12004);
    Bgn3Bindings::mDoExt_3DlineMat1_ct(a + 0x12140);
    Bgn3Bindings::mDoExt_J3DModelPacketS_ct(a + 0x122CC);
    return a;
}

/* WWHD glue
str 10009058 "Bgn" in 02090910
str 1000905C "Bgn" in 02090984
str 10009060 "Bgn" in 02091008
str 10009064 "\uff27\uff08\u30cf\u30e4\u30e0\u30b7\uff09" renamed in 02091008
data bgn 10462250
data esa 10462254
data ke_color 10462258
data ki_all_count 1046225C
data headerStatics 10462260
data zero 1046227C
data l_HIO 10462288
data hio_set 1019119C
data piConstants 10009098
data defaultLightInit 1016E414
data size_d 101911C0
data ke_max 101911E4
data fl_check_d 101911F4
*/
