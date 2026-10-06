/**
 * d_a_obj_buoyflag.cpp (WWHD)
 * Object - Buoy flag (a cloth flag on a buoy or barrel; jumps into the sea when released).
 *
 * The GameCube TU is "Nonmatching": written from the
 * WWHD code (cking.rpx) and verified against it, with the GameCube names. Part files:
 * d_a_obj_buoyflag_calc.cpp (cloth simulation), d_a_obj_buoyflag_draw.cpp (HD drawing),
 * d_a_obj_buoyflag_pending.cpp (weak guest-call stubs for the functions of the other parts).
 */
#include "d/actor/d_a_obj_buoyflag.h"

#define M_arcname STR(0x10026530) /* "Cloth" */
#define ACT_VTBL 0x10026570
#define PACKET_VTBL 0x10026860

namespace daObjBuoyflag {

/* 0232EDC4: daObj::PrmAbstract<Act_c::Prm_e>(actor, width, shift) (per TU) */
u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    WWHD_FUNC(0x0232EDC4, u32, a, width, shift);
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
VERIFY(0x0232EDC4, PrmAbstract);

/* 0232CD40 */
void Act_c::mtx_init() {
    WWHD_FUNC(0x0232CD40, void, this);
    PSMTXTrans(mDoMtx_stack_c::get(), current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
    PSMTXCopy(mDoMtx_stack_c::get(), &mOldMtx);
}
VERIFY(0x0232CD40, &Act_c::mtx_init);

/* 0232CDB4: afloat (follows the setup matrix given by the parent) */
BOOL Act_c::mode_afl() {
    WWHD_FUNC(0x0232CDB4, BOOL, this);
    f32 z = mMtx.m[2][3];
    f32 y = mMtx.m[1][3];
    f32 x = mMtx.m[0][3];
    current.pos.y = y;
    current.pos.x = x;
    current.pos.z = z;
    mPacket.calc(this);
    fopAcM_setCullSizeSphere(this, 0.0f, 50.0f * scale.y, 0.0f, 90.0f * scale.x);
    if (attr_type(this, 0x10026730, 0x10026748)->mCc) {
        f32 sx = scale.x;
        f32 r = sx * 5.0f;
        f32 sy = scale.y;
        gabi::Local<cXyz> center;
        center->x = 0.0f * sx;
        center->y = 12.0f * sy;
        center->z = 0.0f * (f32)scale.z;
        f32 h = sy * 88.0f;
        PSVECAdd(center, &current.pos, center);
        cM3dGCyl_Set(&mCyl.mCyl, center, r, h);
        dComIfG_Ccsp_Set(&mCyl);
    }
    return TRUE;
}
VERIFY(0x0232CDB4, &Act_c::mode_afl);

/* 0232CF30: thrown into the sea (spins about a random axis, sinks) */
BOOL Act_c::mode_jumpToSea() {
    WWHD_FUNC(0x0232CF30, BOOL, this);
    if (mbInit) {
        mRotSpeed = 1000.0f;
        gravity = -8.0f;
        mRotAngle = 0;
        s16 rx = (s16)gabi::ftoi(cM_rndFX(4000.0f));
        s16 ry = (s16)gabi::ftoi(cM_rndFX(32768.0f));
        mDoMtx_ZXYrotS(mDoMtx_stack_c::get(), rx, ry, 0);
        PSMTXMultVecSR(mDoMtx_stack_c::get(), cXyz_BaseZ, &mRotAxis);
    }
    f32 wave = daSea_calcWave(current.pos.x, current.pos.z);
    f32 y = current.pos.y;
    if (y < gabi::fnmsubs(180.0f, scale.y, wave))
        return FALSE;
    f32 spd = gabi::fnmsubs((f32)(s16)mRotAngle, 0.005f, mRotSpeed);
    f32 k1, k2;
    if (y > wave) {
        spd = spd * 0.97f;
        k2 = 0.002f;
        k1 = 0.006f;
    } else {
        spd = spd * 0.92f;
        k2 = 0.015f;
        k1 = 0.04f;
    }
    s32 add = gabi::ftoi(spd);
    mRotSpeed = spd;
    mRotAngle = (s16)(mRotAngle + add);
    daObj_posMoveF_stream(this, nullptr, cXyz_Zero, k1, k2);
    PSMTXTrans(mDoMtx_stack_c::get(), current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_transM(0.0f, 60.0f, 0.0f);
    C_QUATRotAxisRad(mDoMtx_quatStack_get(), &mRotAxis, (f32)(s16)mRotAngle * 9.58738e-05f);
    mDoMtx_stack_quatM(mDoMtx_quatStack_get());
    PSMTXConcat_l(mDoMtx_stack_c::get(), &mJumpMtx, mDoMtx_stack_c::get());
    mDoMtx_stack_transM(0.0f, -60.0f, 0.0f);
    PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
    mPacket.calc(this);
    return TRUE;
}
VERIFY(0x0232CF30, &Act_c::mode_jumpToSea);

void hasi_nrm_init_call() { gabi::call(0x0232BCE0); }

/* GHS inline constructor of a sub-object at `p`: allocates when the address is NULL */
static inline u32 inl_ct(u32 p, u32 size) {
    if (p == 0) p = gabi::ea(operator_new(size));
    return p;
}
static inline void bzero_l(u32 p, u32 n) { gabi::call(0x028F521C, p, n); }
static inline void construct_array(u32 p, u32 n, u32 size, u32 ct) { gabi::call(0x028EFFD0, p, n, size, ct); }

/* the HD GPU state's initial values (after the bzero): a zero cXyz, then 1.0 at every 0x10 bytes */
static inline void gpu_state_init(u32 M) {
    for (u32 o = 0; o < 0xB0; o += 4) gabi::store<f32>(M + o, (o >= 0xC && ((o - 0xC) & 0xF) == 0) ? 1.0f : 0.0f);
}

/* 0232D1F8 Mthd_Create (Act_c::_create inlined) */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x0232D1F8, cPhs_State, i_this);
    Act_c* a = (Act_c*)i_this;
    u32 cond = a->actor_condition;
    if (!(cond & fopAcCnd_INIT_e)) {
        if (a != nullptr) {
            u32 A = gabi::ea(a);
            fopAc_ac_c_ct(a);
            a->__vtbl = ACT_VTBL;
            dCcD_Stts_ct(&a->mStts);
            dCcD_Cyl_ct(&a->mCyl, 0x10026560);
            gabi::call(0x027F1278, &a->mPacket); /* J3DPacket */
            u32 P = A + 0x520;
            /* the flag's vertex buffers: {s32, sead::Buffer {count, ptr}, elements[4], ...} */
            gabi::store<u32>(P + 0xD18, 0);
            gabi::store<u32>(P + 0xC, PACKET_VTBL);
            {
                u32 b = P + 0xD1C;
                if (b == 0) b = gabi::ea(operator_new(8));
                if (b != 0) {
                    gabi::store<u32>(b + 4, 0);
                    gabi::store<u32>(b, 0);
                }
            }
            {
                u32 v = P + 0xD24;
                if (v == 0) v = gabi::ea(operator_new(0x968));
                if (v != 0) {
                    construct_array(v, 4, 0x254, 0x0232E550);
                    gabi::store<u32>(v + 0x950, 0);
                    gabi::store<u32>(v + 0x960, 0);
                    gabi::store<u32>(v + 0x958, 0x20);
                    gabi::store<u8>(v + 0x964, 0);
                    gabi::store<u32>(v + 0x954, 0);
                    for (int k = 0; k < 2; k++)
                        for (int m = 0; m < 2; m++) gabi::store<u32>(v + k * 0x254 + m * 0x4A8, 0);
                }
            }
            /* the flag's GPU state */
            u32 B = A + 0x1BAC;
            gabi::call(0x027B5430, B);
            gabi::call(0x027FD6F4, A + 0x1BC4);
            gabi::call(0x027FB40C, A + 0x1BD0);
            gabi::store<u32>(A + 0x1BDC, 0x1016EF84);
            bzero_l(A + 0x1C44, 0x34);
            inl_ct(A + 0x1BD0 + 0x74, 0x30);
            gabi::call(0x027FB40C, A + 0x1C78);
            gabi::store<u32>(A + 0x1C84, 0x1016EFB4);
            u32 M = A + 0x1CEC;
            bzero_l(M, 0x2F0);
            gpu_state_init(M);
            construct_array(A + 0x1D9C, 2, 0x10, 0x0232DCC4);
            construct_array(A + 0x1DBC, 2, 0x10, 0x0232DCC4);
            construct_array(A + 0x1DDC, 2, 0x10, 0x0232DCC4);
            for (u32 o = 0x110; o <= 0x260; o += 0x30) inl_ct(M + o, 0x30);
            for (u32 o = 0x290; o <= 0x2E0; o += 0x10) inl_ct(M + o, 0x10);
            gabi::store<u8>(A + 0x1FDC, 0);
            gabi::call(0x027BE6B8, A + 0x1FE0);
            gabi::call(0x027BE6B8, A + 0x2070);
            gabi::call(0x027BDF7C, A + 0x2100);
            gabi::call(0x027BDF7C, A + 0x2298);
            /* the pole's vertex buffers */
            gabi::store<u32>(A + 0x2430, 0);
            {
                u32 b = A + 0x2434;
                if (b == 0) b = gabi::ea(operator_new(8));
                if (b != 0) {
                    gabi::store<u32>(b + 4, 0);
                    gabi::store<u32>(b, 0);
                }
            }
            {
                u32 v = A + 0x520 + 0x1F1C;
                if (v == 0) v = gabi::ea(operator_new(0x4C0));
                if (v != 0) {
                    construct_array(v, 2, 0x254, 0x0232E5AC);
                    gabi::store<u32>(v + 0x4A8, 0);
                    gabi::store<u32>(v + 0x4AC, 0);
                    gabi::store<u32>(v + 0x4B8, 0);
                    gabi::store<u32>(v + 0x4B0, 0x20);
                    gabi::store<u8>(v + 0x4BC, 0);
                    gabi::store<u32>(v, 0);
                    gabi::store<u32>(v + 0x254, 0);
                }
            }
            gabi::call(0x027B5430, A + 0x28FC);
            gabi::call(0x027FD6F4, A + 0x2914);
            gabi::call(0x027FB40C, A + 0x2920);
            gabi::store<u32>(A + 0x292C, 0x1016EF84);
            bzero_l(A + 0x2994, 0x34);
            inl_ct(A + 0x2920 + 0x74, 0x30);
            gabi::call(0x027FB40C, A + 0x29C8);
            u32 M2 = A + 0x2A3C;
            gabi::store<u32>(A + 0x29D4, 0x1016EFB4);
            bzero_l(M2, 0x2F0);
            gpu_state_init(M2);
            construct_array(A + 0x2AEC, 2, 0x10, 0x0232DCC4);
            construct_array(A + 0x2B0C, 2, 0x10, 0x0232DCC4);
            construct_array(A + 0x2B2C, 2, 0x10, 0x0232DCC4);
            for (u32 o = 0x110; o <= 0x260; o += 0x30) inl_ct(M2 + o, 0x30);
            for (u32 o = 0x290; o <= 0x2E0; o += 0x10) inl_ct(M2 + o, 0x10);
            gabi::store<u8>(A + 0x2D2C, 0);
            gabi::call(0x027BE6B8, A + 0x2D30);
            gabi::call(0x027BDF7C, A + 0x2DC0);
            gabi::call(0x027BE6B8, A + 0x2F58);
            gabi::call(0x027BDF7C, A + 0x2FE8);
            hasi_nrm_init_call();
            a->mPacket.gpu_init_hata();
            a->mPacket.gpu_init_hasi();
            cond = a->actor_condition;
        }
        a->actor_condition = cond | fopAcCnd_INIT_e;
    }
    a->mType = PrmAbstract(a, 2, 0);
    cPhs_State phase = dComIfG_resLoad(&a->mPhs, M_arcname);
    if (phase == cPhs_COMPLEATE_e) {
        f32 s = attr_type(a, 0x100267A8, 0x100267C0)->mScale;
        a->scale.x = s;
        a->cullMtx = gabi::ea(&a->mMtx); /* fopAcM_SetMtx */
        a->scale.y = s;
        a->scale.z = s;
        fopAcM_setCullSizeSphere(a, 0.0f, 50.0f, 0.0f, 2000.0f);
        a->mtx_init();
        if (PrmAbstract(a, 1, 0x1F)) /* prm_get_noCull */
            a->actor_status &= ~0x180u;
        if (attr_type(a, 0x100267A8, 0x100267C0)->mCc) {
            a->mStts.Init(0xFF, 0xFF, a);
            a->mCyl.Set(gabi::at<dCcD_SrcCyl>(0x1002681C)); /* M_cyl_src */
            a->mCyl.mStts = &a->mStts;
        }
        a->mPacket.mpActor = a;
        a->mPacket.init(a);
        a->mMode = 0;
        a->mbInit = 1;
    }
    return phase;
}
VERIFY(0x0232D1F8, Mthd_Create);

/* 0232DABC */
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x0232DABC, BOOL, i_this);
    dComIfG_resDelete(&((Act_c*)i_this)->mPhs, M_arcname);
    return TRUE;
}
VERIFY(0x0232DABC, Mthd_Delete);

/* GHS pointer to member function call returning BOOL */
static inline BOOL ptmf_call_ret(u32 entry, void* self) {
    s16 idx = gabi::load<s16>(entry + 2);
    s16 delta = gabi::load<s16>(entry);
    void* p = gabi::at<void>(gabi::ea(self) + delta);
    if (idx < 0)
        return gabi::call_ptr<BOOL>(gabi::load<u32>(entry + 4), p);
    u32 vt = gabi::load<u32>(gabi::ea(p) + gabi::load<s16>(entry + 6));
    return gabi::call_ptr<BOOL>(gabi::load<u32>(vt + idx * 8 + 4), p);
}

/* 0232DAEC */
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x0232DAEC, BOOL, i_this);
    Act_c* a = (Act_c*)i_this;
    /* static const ModeFunc mode_proc[] = {mode_afl, mode_jumpToSea}: guard 0x101FDB98, table 0x101FDB88 */
    if (gabi::load<u32>(0x101FDB98) == 0) {
        gabi::store<u32>(0x101FDB98, 1);
        memcpy_g(gabi::at<u8>(0x101FDB88), gabi::at<u8>(0x10026538), 0x10);
    }
    if (ptmf_call_ret(0x101FDB88 + 8 * (u32)a->mMode, a)) {
        a->mbInit = 0;
        PSMTXCopy(&a->mMtx, &a->mOldMtx);
        return TRUE;
    }
    fopAcM_delete(a);
    return TRUE;
}
VERIFY(0x0232DAEC, Mthd_Execute);

/* 0232DBD0 */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x0232DBD0, BOOL, i_this);
    Act_c* a = (Act_c*)i_this;
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &a->current.pos, &a->tevStr);
    a->mPacket.update(a);
    return TRUE;
}
VERIFY(0x0232DBD0, Mthd_Draw);

/* 0232EDBC */
static BOOL Mthd_IsDelete(void* i_this) {
    WWHD_FUNC(0x0232EDBC, BOOL, i_this);
    return TRUE;
}
VERIFY(0x0232EDBC, Mthd_IsDelete);

/* 0232DC1C */
static void __sinit_d_a_obj_buoyflag_cpp() {
    WWHD_FUNC(0x0232DC1C, void, (u32)0);
    sinit_header_statics(0x1046934C, 0x101C81B4);
}
VERIFY(0x0232DC1C, __sinit_d_a_obj_buoyflag_cpp);

/* 0232DCB0: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0232DCB0, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x0232DCB0, trivial_dt);

/* 0232DCC4: constructor of a trivial 0x10-byte element (allocates when NULL) */
static void* elem10_ct(void* p) {
    WWHD_FUNC(0x0232DCC4, void*, p);
    if (p == nullptr)
        p = operator_new(0x10);
    return p;
}
VERIFY(0x0232DCC4, elem10_ct);

/* 0232DCF0, 0232E54C: empty virtuals */
static void empty_virtual(void* p) { WWHD_FUNC(0x0232DCF0, void, p); }
VERIFY(0x0232DCF0, empty_virtual);
static void empty_virtual2(void* p) { WWHD_FUNC(0x0232E54C, void, p); }
VERIFY(0x0232E54C, empty_virtual2);

/* 0232DCF4, 0232DD54: deleting destructors of the 0x254-byte vertex buffer element (two copies) */
static void vtx_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0232DCF4, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x027BF880, gabi::ea(p) + 0x158, 2);
        gabi::call(0x027B5CBC, gabi::ea(p) + 4, 2);
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x0232DCF4, vtx_dt);
static void vtx_dt2(void* p, s32 flags) {
    WWHD_FUNC(0x0232DD54, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x027BF880, gabi::ea(p) + 0x158, 2);
        gabi::call(0x027B5CBC, gabi::ea(p) + 4, 2);
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x0232DD54, vtx_dt2);

/* 0232E550, 0232E5AC: constructors of the 0x254-byte vertex buffer element (two copies) */
static void* vtx_ct(void* p) {
    WWHD_FUNC(0x0232E550, void*, p);
    if (p == nullptr) {
        p = operator_new(0x254);
        if (p == nullptr)
            return p;
    }
    gabi::call(0x027B5BD8, gabi::ea(p) + 4);
    gabi::call(0x027BF734, gabi::ea(p) + 0x158);
    gabi::store<u32>(gabi::ea(p) + 0x250, 0);
    gabi::store<u32>(gabi::ea(p) + 0x24C, 0);
    return p;
}
VERIFY(0x0232E550, vtx_ct);
static void* vtx_ct2(void* p) {
    WWHD_FUNC(0x0232E5AC, void*, p);
    if (p == nullptr) {
        p = operator_new(0x254);
        if (p == nullptr)
            return p;
    }
    gabi::call(0x027B5BD8, gabi::ea(p) + 4);
    gabi::call(0x027BF734, gabi::ea(p) + 0x158);
    gabi::store<u32>(gabi::ea(p) + 0x250, 0);
    gabi::store<u32>(gabi::ea(p) + 0x24C, 0);
    return p;
}
VERIFY(0x0232E5AC, vtx_ct2);

}  // namespace daObjBuoyflag
