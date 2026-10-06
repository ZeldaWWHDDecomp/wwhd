/**
 * d_a_bwdg.cpp (WWHD)
 * Molgera's sand floor (height-field background, waves driven by the boss).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_bwdg.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 *
 * HD: daBwdg_packet_c is an HD render packet (GPU vertex buffers, 0x1980C bytes, constructor
 * 021045E8, prepare 02103B30, draw 0210508C, destructor 02105608); it holds one vertex buffer
 * (positions at packet+0xCC, normals at packet+0xC6D8, 0x1081 each) instead of two.
 */
#include "bindings.h"

#define BWDG_VTBL 0x1000C12C   /* HD: bwdg_class vtable */
#define SAFESTRING_VTBL 0x1000C114
#define boss_ea 0x10462C04     /* static bwd_class* boss */

/* bwd_class fields read here (GameCube +0x284) */
enum : u32 {
    BWD_m17C8 = 0x1A4C, /* cXyz: wave centre (x, z used) */
    BWD_m17D4 = 0x1A58,
    BWD_m17D8 = 0x1A5C, /* s16 */
    BWD_m17DC = 0x1A60,
    BWD_m17E0 = 0x1A64,
};

struct bwdg_class : fopAc_ac_c {
    /* 0x3AC */ u8 m00290[0x3C8 - 0x3AC];
    /* 0x3C8 */ be<u32> mHD3C8;          /* HD: set to `this` by the constructor */
    /* 0x3CC */ request_of_phase_process_class mPhase;
    /* 0x3D4 */ be<s16> m002B4;
    /* 0x3D6 */ u8 _3D6[2];
    /* 0x3D8 */ be<s16> m002B8;
    /* 0x3DA */ u8 _3DA[2];
    /* 0x3DC */ gptr<dBgW> mpBgW;         /* dBgWHf */
    /* 0x3E0 */ be<f32> m002C0[0x1081];
    /* 0x45E4 */ u8 mBwdgPacket[0x1980C]; /* HD daBwdg_packet_c */

    cXyz* getPos() { return gabi::at<cXyz>(gabi::ea(this) + 0x45E4 + 0xCC); }
    cXyz* getNrm() { return gabi::at<cXyz>(gabi::ea(this) + 0x45E4 + 0xC6D8); }
    void* packet() { return gabi::at<void>(gabi::ea(this) + 0x45E4); }
};
WWHD_OFFSET(bwdg_class, mPhase, 0x3CC);
WWHD_OFFSET(bwdg_class, mpBgW, 0x3DC);
WWHD_OFFSET(bwdg_class, mBwdgPacket, 0x45E4);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 024F5800 dBgWHf::dBgWHf (allocates when this == NULL) */
static inline dBgW* new_dBgWHf() { return gabi::call<dBgW*>(0x024F5800, (u32)0); }
/* 024F5870 dBgWHf::Set(cBgD_t*, u16* grid, f32 size, int, int, int) */
static inline BOOL dBgWHf_Set(dBgW* w, cBgD_t* d, void* grid, f32 s, s32 a, s32 b, s32 c) {
    return gabi::call<BOOL>(0x024F5870, w, d, grid, s, a, b, c);
}
/* 024F5B08 dBgWSv::CopyBackVtx, 024F5910 dBgWHf::MoveHf */
static inline void dBgWSv_CopyBackVtx(dBgW* w) { gabi::call(0x024F5B08, w); }
static inline void dBgWHf_MoveHf(dBgW* w) { gabi::call(0x024F5910, w); }
/* dBgWSv::SetVtx (HD inline): the first vertex table also takes a back copy */
static inline void dBgWSv_SetVtx(dBgW* w, cXyz* vtx) {
    u32 old = gabi::load<u32>(gabi::ea(w) + 0x90);
    gabi::store<u32>(gabi::ea(w) + 0x90, gabi::ea(vtx));
    if (old == 0)
        dBgWSv_CopyBackVtx(w);
}
/* 025F18EC mDoMtx_XrotS (also local in mo2) */
static inline void mDoMtx_XrotS_l(Mtx34* m, s16 x) { gabi::call(0x025F18EC, m, x); }
/* 027F0E04 J3DDrawBuffer::entryImm(packet, u16); j3dSys opa buffer at 0x104B4634 */
static inline void opa_entryImm(void* packet, u16 idx) { gabi::call(0x027F0E04, gabi::load<u32>(0x104B4634), packet, idx); }
enum { TEV_TYPE_BG0_FULL = 5 };

/* 021039CC: HD helper (this TU): 3x4 matrix assignment through a float temporary */
static void bwdg_mtx_assign(Mtx34* dst, const Mtx34* src) {
    WWHD_FUNC(0x021039CC, void, dst, src);
    f32 t[12];
    t[5] = src->m[1][1];
    t[4] = src->m[1][0];
    t[6] = src->m[1][2];
    t[7] = src->m[1][3];
    t[8] = src->m[2][0];
    t[9] = src->m[2][1];
    t[11] = src->m[2][3];
    t[0] = src->m[0][0];
    t[3] = src->m[0][3];
    t[1] = src->m[0][1];
    t[10] = src->m[2][2];
    t[2] = src->m[0][2];
    for (int i = 0; i < 12; i++) gabi::store<f32>(gabi::ea(dst) + 4 * i, t[i]);
}
VERIFY(0x021039CC, bwdg_mtx_assign);

/* 02103A6C: HD helper (this TU): GXColorS10 -> 4 floats in 0..1 (/255) */
static void bwdg_color_s10_to_f(be<f32>* out, const be<s16>* in) {
    WWHD_FUNC(0x02103A6C, void, out, in);
    f32 r = (f32)(s32)in[0] / 255.0f;
    f32 g = (f32)(s32)in[1] / 255.0f;
    f32 b = (f32)(s32)in[2] / 255.0f;
    f32 a = (f32)(s32)in[3] / 255.0f;
    out[0] = r;
    out[1] = g;
    out[2] = b;
    out[3] = a;
}
VERIFY(0x02103A6C, bwdg_color_s10_to_f);

/* 02103F14: HD: the packet takes the model matrix (no view concat) and prepares its GPU buffers
 * (02103B30) right after the entry */
static BOOL daBwdg_Draw(bwdg_class* i_this) {
    WWHD_FUNC(0x02103F14, BOOL, i_this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0_FULL, &i_this->current.pos, &i_this->tevStr);
    MtxTrans(0.0f, 10.0f, 0.0f, 0);
    PSMTXCopy(calc_mtx(), gabi::at<Mtx34>(gabi::ea(i_this) + 0x45E4 + 0x98)); /* packet mMtx */
    gabi::store<u32>(gabi::ea(i_this) + 0x45E4 + 0xC8, gabi::ea(&i_this->tevStr)); /* setTevStr */
    opa_entryImm(i_this->packet(), 0);
    gabi::call(0x02103B30, i_this->packet()); /* HD: packet prepare */
    return TRUE;
}
VERIFY(0x02103F14, daBwdg_Draw);

/* 02103FA4 */
static void wave_cont(bwdg_class* i_this, u8 r4) {
    WWHD_FUNC(0x02103FA4, void, i_this, r4);
    cXyz* posVtx = i_this->getPos();
    f32 cx, cz;
    f32 f31, f30, f29, f28, f27;
    if (r4 != 0) {
        f30 = 1.0f;
        f31 = 2000.0f;
        cx = 0.0f;
        cz = 0.0f;
        f28 = -0.75f;
        f29 = 1.0f;
        f27 = 600.0f;
    } else {
        f30 = 0.5f;
        f31 = 0.0f;
        u32 boss = gabi::load<u32>(boss_ea);
        i_this->m002B8 = (s16)(i_this->m002B8 + gabi::load<s16>(boss + BWD_m17D8));
        boss = gabi::load<u32>(boss_ea);
        f27 = gabi::load<f32>(boss + BWD_m17E0);
        cx = gabi::load<f32>(boss + BWD_m17C8);
        f29 = gabi::load<f32>(boss + BWD_m17DC);
        cz = gabi::load<f32>(boss + BWD_m17C8 + 8);
        f28 = gabi::load<f32>(boss + BWD_m17D4);
    }

    for (int i = 0; i < 0x1081; posVtx++, i++) {
        f32 px = posVtx->x;
        f32 pz = posVtx->z;
        f32 dz = pz - cz;
        f32 dx = px - cx;
        f32 f0 = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
        f32 f1 = 3000.0f - f0;
        f32 f4 = f1 >= 0.0f ? f1 : 0.0f; /* fsel */
        f1 = f1 >= 0.0f ? f1 : 0.0f;
        f32 f3 = gabi::fmadds(f1, 0.00667f, 10.0f);
        f4 *= 0.01f;
        if (f3 > 30.0f) {
            f3 = 30.0f;
        }
        s16 a = (s16)gabi::ftoi(f0 * 200.0f);
        f32 step = gabi::fmadds(f3, 0.3f, f31) * f29;
        f32 f2 = cM_ssin(i_this->m002B8 + a);
        cLib_addCalc2(&i_this->m002C0[i], gabi::fmadds(f4 * f4, f28, f2 * f3), f30, step);
        posVtx->y = i_this->m002C0[i] + f27;
    }

    /* HD: the rotated normal is a function-local static, (0, 1, 0) rotated by -0x4A38 about X */
    cXyz* sp0C = gabi::at<cXyz>(0x10462C24);
    if (gabi::load<u32>(0x10462C30) == 0) {
        gabi::store<u32>(0x10462C30, 1);
        sp0C->x = 0.0f;
        sp0C->y = 0.9687129855155945f;
        sp0C->z = -0.24818499386310577f;
    }
    cXyz* nrmVtx = i_this->getNrm();
    posVtx = i_this->getPos();
    /* HD: the slope is taken from the next row (z + 130), not from the height above the base */
    for (int i = 0; i < 0x1081; i++, posVtx++, nrmVtx++) {
        if (i >= 0x1040) {
            continue;
        }
        gabi::Local<cXyz> d;
        cXyz_mi(posVtx + 0x41, d.get(), posVtx);
        f32 dy = d->y;
        Mtx34* m = calc_mtx();
        s16 ang = cM_atan2s(dy, 130.0f);
        mDoMtx_XrotS_l(m, (s16)-ang);
        MtxPosition(sp0C, nrmVtx);
    }
}
VERIFY(0x02103FA4, wave_cont);

/* 0210439C: HD: fpcM_GetName checks the pointer */
static void* boss_a_d_sub(void* param_1, void* param_2) {
    WWHD_FUNC(0x0210439C, void*, param_1, param_2);
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == 0xD9 /* PROC_BWD */) {
        return param_1;
    }
    return nullptr;
}
VERIFY(0x0210439C, boss_a_d_sub);

/* 021043EC */
static BOOL daBwdg_Execute(bwdg_class* i_this) {
    WWHD_FUNC(0x021043EC, BOOL, i_this);
    cXyz* posVtx = i_this->getPos();
    if (gabi::load<u32>(boss_ea) == 0) {
        void* b = fpcM_Search(0x0210439C /* boss_a_d_sub */, i_this);
        gabi::store<u32>(boss_ea, gabi::ea(b));
        if (b == nullptr) {
            return TRUE;
        }
    }
    i_this->m002B4 = (s16)(i_this->m002B4 + 1);
    dBgWSv_CopyBackVtx(i_this->mpBgW);
    wave_cont(i_this, 0);
    dBgWSv_SetVtx(i_this->mpBgW, posVtx);
    dBgWHf_MoveHf(i_this->mpBgW);
    return TRUE;
}
VERIFY(0x021043EC, daBwdg_Execute);

/* 02104498 */
static BOOL daBwdg_IsDelete(bwdg_class* i_this) {
    WWHD_FUNC(0x02104498, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02104498, daBwdg_IsDelete);

/* 021044A0 */
static BOOL daBwdg_Delete(bwdg_class* i_this) {
    WWHD_FUNC(0x021044A0, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhase, STR(0x1000C188) /* "Bwdg" */);
    if (i_this->heap) {
        cBgS_Release(dComIfG_Bgsp(), i_this->mpBgW);
    }
    return TRUE;
}
VERIFY(0x021044A0, daBwdg_Delete);

/* 021044F8 */
static BOOL useHeapInit(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x021044F8, BOOL, i_actor);
    bwdg_class* i_this = (bwdg_class*)i_actor;
    i_this->mpBgW = new_dBgWHf();
    if (i_this->mpBgW == nullptr) {
        return FALSE;
    }
    cBgD_t* dzb = (cBgD_t*)dComIfG_getObjectRes(STR(0x1000C190), 7 /* dRes_INDEX_BWDG_DZB_HSAND1_e */, SAFESTRING_VTBL);
    void* grid = dComIfG_getObjectRes(STR(0x1000C190), 4 /* dRes_INDEX_BWDG_DAT_GRIDIDX_e */, SAFESTRING_VTBL);
    if (!dBgWHf_Set(i_this->mpBgW, dzb, grid, 130.0f, 0x40, 0x40, 0)) {
        return TRUE;
    } else {
        return FALSE;
    }
}
VERIFY(0x021044F8, useHeapInit);

/* 02104EA0: HD: one vertex buffer (base_xz_set once) */
static cPhs_State daBwdg_Create(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x02104EA0, cPhs_State, i_actor);
    bwdg_class* i_this = (bwdg_class*)i_actor;
    /* fopAcM_ct(i_this, bwdg_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->mHD3C8 = gabi::ea(i_this);
            i_this->__vtbl = BWDG_VTBL;
            gabi::call(0x021045E8, i_this->packet()); /* daBwdg_packet_c::daBwdg_packet_c (HD) */
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }

    cPhs_State phase_state = dComIfG_resLoad(&i_this->mPhase, STR(0x1000C1D4) /* "Bwdg" */);
    if (phase_state == cPhs_COMPLEATE_e) {
        gabi::store<u32>(boss_ea, 0);
        i_this->current.pos.set(0.0f, 0.0f, 0.0f);
        if (!fopAcM_entrySolidHeap(i_this, 0x021044F8 /* useHeapInit */, 0x96000)) {
            return cPhs_ERROR_e;
        }
        dBgS_Regist(dComIfG_Bgsp(), i_this->mpBgW, i_this);
        /* base_xz_set */
        cXyz* posVtx = i_this->getPos();
        for (int i = 0; i < 0x41; i++) {
            f32 z = gabi::fmsubs((f32)i, 130.0f, 4160.0f);
            for (int j = 0; j < 0x41; posVtx++, j++) {
                posVtx->x = gabi::fmsubs((f32)j, 130.0f, 4160.0f);
                posVtx->z = z;
            }
        }
        wave_cont(i_this, 1);
        dBgWSv_SetVtx(i_this->mpBgW, i_this->getPos());
        dBgWSv_CopyBackVtx(i_this->mpBgW);
        dBgW_Move(i_this->mpBgW);
    }
    return phase_state;
}
VERIFY(0x02104EA0, daBwdg_Create);

/* 02105478 */
static void __sinit_d_a_bwdg_cpp() {
    WWHD_FUNC(0x02105478, void, (u32)0);
    sinit_header_statics(0x10462C08, 0x101B3820);
}
VERIFY(0x02105478, __sinit_d_a_bwdg_cpp);

/* 0210550C: sead::SafeString deleting destructor (this TU's vtable slot) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0210550C, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x0210550C, SafeString_dt);

/* 02105DD4: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x02105DD4, void, (u32)0);
}
VERIFY(0x02105DD4, SafeString_assureTerminationImpl);

/* 021059E4: empty virtual of daBwdg_packet_c (HD vtable 0x1000C1F4 +0x24) */
static void packet_empty(void*) {
    WWHD_FUNC(0x021059E4, void, (u32)0);
}
VERIFY(0x021059E4, packet_empty);

/* 02105520: constructor of the packet's 0x254-byte GPU buffer element (HD; allocates when NULL) */
static void* packet_buf_ct(void* p) {
    WWHD_FUNC(0x02105520, void*, p);
    if (p == nullptr) {
        p = operator_new(0x254);
        if (p == nullptr)
            return nullptr;
    }
    u32 b = gabi::ea(p);
    gabi::call(0x027B5BD8, b + 4);
    gabi::call(0x027BF734, b + 0x158);
    gabi::store<u32>(b + 0x250, 0);
    gabi::store<u32>(b + 0x24C, 0);
    return p;
}
VERIFY(0x02105520, packet_buf_ct);

/* 0210557C: constructor of a trivial 16-byte element (HD; allocates when NULL) */
static void* packet_elem16_ct(void* p) {
    WWHD_FUNC(0x0210557C, void*, p);
    if (p == nullptr)
        p = operator_new(0x10);
    return p;
}
VERIFY(0x0210557C, packet_elem16_ct);

/* 021055A8: deleting destructor of the 0x254-byte GPU buffer element (HD) */
static void packet_buf_dt(void* p, s32 flags) {
    WWHD_FUNC(0x021055A8, void, p, flags);
    if (p != nullptr) {
        u32 b = gabi::ea(p);
        gabi::call(0x027BF880, b + 0x158, 2);
        gabi::call(0x027B5CBC, b + 4, 2);
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x021055A8, packet_buf_dt);

/* ---- HD daBwdg_packet_c: GPU buffers ----
 * packet + 0x18CE4 (B): +0x4/+0x8 a sead buffer of 0x14-byte entries (each two arrays of 0xF4-byte
 * objects with a vtable at +0xF0), +0xC two 0x254-byte vertex buffers (index +0x4A8 at B+0x4B4),
 * +0x4CC a container (count, array of 0x23C-byte entries), +0x8E8 / +0x900 more GPU objects. */
static inline void bwdg_heap_free(u32 field) {
    u32 pre = gabi::load<u32>(field); /* still in r4 at the heap lookup */
    u32 heap = gabi::call<u32>(0x02755FEC, gabi::load<u32>(0x101F8B4C), pre);
    u32 fn = gabi::load<u32>(gabi::load<u32>(heap + 0xC) + 0x3C);
    gabi::call_ptr(fn, heap, gabi::load<u32>(field)); /* the pointer is read after the heap lookup */
}
/* release of one 0x254-byte vertex buffer (inline) */
static inline void bwdg_gpubuf_release(u32 e) {
    gabi::call(0x027BF7E8, e + 0x158);
    u32 b = gabi::load<u32>(e + 0x250);
    gabi::store<u32>(e + 0, 0);
    if (b != 0) {
        bwdg_heap_free(e + 0x250);
        gabi::store<u32>(e + 0x24C, 0);
        gabi::store<u32>(e + 0x250, 0);
    }
}
static inline void bwdg_objarr_free(u32 e, u32 cnt_off, u32 arr_off) {
    s32 m = gabi::load<s32>(e + cnt_off);
    u32 a = gabi::load<u32>(e + arr_off);
    for (s32 j = 0; j < m;) {
        u32 obj = a + j * 0xF4;
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(obj + 0xF0) + 0xC), obj, 2); /* virtual deleting dtor */
        j++;
        m = gabi::load<s32>(e + cnt_off);
        a = gabi::load<u32>(e + arr_off);
    }
    bwdg_heap_free(e + arr_off);
    gabi::store<u32>(e + cnt_off, 0);
    gabi::store<u32>(e + arr_off, 0);
}
/* ~daBwdg_packet_c body (inline in both destructors) */
static inline void bwdg_packet_dt_body(u32 p) {
    u32 B = p + 0x18CE4;
    u32 c = B + 0xC;
    gabi::store<u32>(p + 0xC, 0x1000C1F4); /* packet vtable */
    bwdg_gpubuf_release(c);
    bwdg_gpubuf_release(c + 0x254);
    gabi::store<u32>(c + 0x4B8, 0);
    u32 q = B + 0x4CC;
    s32 n = gabi::load<s32>(q);
    for (s32 i = 0; i < n;) {
        u32 base = gabi::load<u32>(q + 4);
        u32 ent = ((u32)i < (u32)n) ? base + i * 0x23C : base;
        for (int k = 0; k < 2; k++) gabi::call(0x027BEBEC, ent + 0x10 + k * 0x1C);
        n = gabi::load<s32>(q);
        i++;
    }
    for (int k = 0; k < 2; k++) gabi::call(0x027BEBEC, q + 0x1C + k * 0x1C);
    for (int k = 0; k < 2; k++) gabi::call(0x027BEBEC, q + 0xC4 + k * 0x1C);
    gabi::call(0x027BE2B0, B + 0x900, 2);
    gabi::call(0x027B54A0, B + 0x8E8, 2);
    gabi::call(0x027FB528, q + 0xB4, 0);
    gabi::call(0x027FB528, q + 0xC, 0);
    gabi::call(0x027FD764, q, 2);
    if (c != 0) {
        bwdg_gpubuf_release(c);
        bwdg_gpubuf_release(c + 0x254);
        gabi::store<u32>(c + 0x4B8, 0);
        gabi::call(0x028F0164, c, 2, 0x254, 0x021055A8, 0, 0); /* __destroy_arr */
    }
    u32 arr = gabi::load<u32>(B + 8);
    if (arr != 0) {
        s32 n2 = gabi::load<s32>(B + 4);
        for (s32 i = 0; i < n2; i++) {
            u32 e = arr + i * 0x14;
            if (e == 0)
                continue;
            u32 a = gabi::load<u32>(e + 8);
            gabi::store<u32>(e + 0, 0);
            if (a != 0)
                bwdg_objarr_free(e, 4, 8);
            if (gabi::load<u32>(e + 0x10) != 0)
                bwdg_objarr_free(e, 0xC, 0x10);
            n2 = gabi::load<s32>(B + 4);
            arr = gabi::load<u32>(B + 8);
        }
        bwdg_heap_free(B + 8);
        gabi::store<u32>(B + 4, 0);
        gabi::store<u32>(B + 8, 0);
    }
    gabi::call(0x027F13DC, p, 0); /* J3DPacket::~J3DPacket */
}

/* 02105608: daBwdg_packet_c deleting destructor (HD) */
static void bwdg_packet_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02105608, void, p, flags);
    if (p != nullptr) {
        bwdg_packet_dt_body(gabi::ea(p));
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x02105608, bwdg_packet_dt);

/* 021059E8: bwdg_class deleting destructor (HD virtual; the packet destructor inline) */
static void bwdg_class_dt(bwdg_class* i_this, s32 flags) {
    WWHD_FUNC(0x021059E8, void, i_this, flags);
    if (i_this != nullptr) {
        bwdg_packet_dt_body(gabi::ea(i_this) + 0x45E4);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x021059E8, bwdg_class_dt);

struct bwdg_col4 { be<f32> v[4]; };

/* 02103B30: HD packet prepare (after the draw-buffer entry): fills the current vertex buffer
 * from the position/normal arrays through the model's index table, flips the buffer, sets the
 * fog/light state and the shader constants (view matrix, tev colours) */
static void bwdg_packet_prepare(void* pkt) {
    WWHD_FUNC(0x02103B30, void, pkt);
    u32 p = gabi::ea(pkt);
    u32 c = p + 0x18CF0; /* the two vertex buffers */
    u32 buf = gabi::load<u32>(c + gabi::load<s32>(c + 0x4A8) * 0x254);
    for (u32 q = buf; q < buf + 0x41000; q += 0x20) { /* dcbz */
        for (u32 i = 0; i < 0x20; i += 4) gabi::store<u32>((q & ~31u) + i, 0);
    }
    u32 out = gabi::load<u32>(c + gabi::load<s32>(c + 0x4A8) * 0x254);
    u32 n = gabi::load<u32>(0x101930F0);
    for (u32 k = 0; k < n; k++) {
        u32 tri = 0x1019B520 + k * 6;
        u32 o = out + k * 0x20;
        cXyz* pos = gabi::at<cXyz>(p + 0xCC + gabi::load<u16>(tri) * 0xC);
        f32 pz = pos->z, px = pos->x, py = pos->y;
        gabi::store<f32>(o + 0, px);
        gabi::store<f32>(o + 4, py);
        gabi::store<f32>(o + 8, pz);
        cXyz* nrm = gabi::at<cXyz>(p + 0xC6D8 + gabi::load<u16>(tri + 2) * 0xC);
        f32 nz = nrm->z, nx = nrm->x, ny = nrm->y;
        gabi::store<f32>(o + 0xC, nx);
        gabi::store<f32>(o + 0x10, ny);
        gabi::store<f32>(o + 0x14, nz);
        u32 t = gabi::load<u16>(tri + 4) * 8;
        f32 s0 = gabi::load<f32>(0x10193118 + t);
        f32 t0 = gabi::load<f32>(0x1019311C + t);
        gabi::store<f32>(o + 0x18, s0);
        gabi::store<f32>(o + 0x1C, t0);
        n = gabi::load<u32>(0x101930F0);
    }
    u32 e = c + gabi::load<s32>(c + 0x4A8) * 0x254;
    gabi::call(0x027B5E94, e + 4, 0, gabi::load<u32>(e + 0x150)); /* buffer flush */
    gabi::store<u32>(c + 0x4A8, gabi::load<u32>(c + 0x4A8) == 0);
    gabi::call(0x0255F8F4, gabi::load<u32>(p + 0xC8)); /* dKy_GxFog_tevstr_set */
    gabi::call(0x0255FE90, gabi::load<u32>(p + 0xC8)); /* dKy_setLight_mine */
    gabi::Local<Mtx34> view;
    bwdg_mtx_assign(view.get(), gabi::at<Mtx34>(0x104B45C0 + 0x38)); /* j3dSys view matrix */
    u32 q = p + 0x191B0;
    gabi::call(0x027FDA54, q, 0, view.get(), 0x104B45C0 + 0x14C, gabi::load<u32>(0x104B45C0 + 0x148) + 0x240);
    {
        u32 tev = gabi::load<u32>(p + 0xC8);
        u32 obj = gabi::load<u32>(q + 4);
        gabi::Local<bwdg_col4> col;
        gabi::Local<bwdg_col4> res;
        bwdg_color_s10_to_f(col->v, gabi::at<be<s16>>(tev + 0x90));
        tev = gabi::load<u32>(p + 0xC8);
        gabi::call(0x0274D458, res.get(), col.get(), gabi::load<f32>(tev + 0x28));
        for (int i = 0; i < 4; i++) gabi::store<u32>(obj + 0x1C4 + 4 * i, gabi::load<u32>(gabi::ea(res.get()) + 4 * i));
        tev = gabi::load<u32>(p + 0xC8);
        obj = gabi::load<u32>(q + 4);
        bwdg_color_s10_to_f(col->v, gabi::at<be<s16>>(tev + 0x160));
        tev = gabi::load<u32>(p + 0xC8);
        gabi::Local<bwdg_col4> res2;
        gabi::call(0x0274D458, res2.get(), col.get(), gabi::load<f32>(tev + 0x16C));
        for (int i = 0; i < 4; i++) gabi::store<u32>(obj + 0x1D4 + 4 * i, gabi::load<u32>(gabi::ea(res2.get()) + 4 * i));
    }
    gabi::store<u32>(q + 0xB0, 0);
    {
        gabi::Local<Mtx34> m;
        for (int i = 0; i < 12; i++) gabi::store<f32>(gabi::ea(m.get()) + 4 * i, gabi::load<f32>(p + 0x98 + 4 * i));
        PSMTXCopy(m.get(), gabi::at<Mtx34>(q + 0x80));
    }
    u32 tev = gabi::load<u32>(p + 0xC8);
    for (int i = 0; i < 4; i++) gabi::store<f32>(q + 0x168 + 4 * i, (f32)gabi::load<s16>(tev + 0x90 + 2 * i) / 255.0f);
    for (int i = 0; i < 4; i++) gabi::store<f32>(q + 0x178 + 4 * i, (f32)gabi::load<u8>(tev + 0x98 + i) / 255.0f);
    tev = gabi::load<u32>(p + 0xC8);
    gabi::call(0x0274D2AC, q + 0x178, gabi::load<f32>(tev + 0x24));
    u32 obj = gabi::load<u32>(q + 4);
    for (int i = 0; i < 4; i++) gabi::store<u32>(q + 0x148 + 4 * i, gabi::load<u32>(obj + 0x1C4 + 4 * i));
    gabi::call(0x027FE0DC, q, 0);
}
VERIFY(0x02103B30, bwdg_packet_prepare);

struct bwdg_draw_state { u8 _[0x11C]; }; /* HD sizeof (ctor 02750250 allocates 0x11C, vtable at +0x118) */

/* shader / buffer bind of one draw mode (inline twice in the packet draw) */
static inline void bwdg_bind_shader(u32 q, u32 shp) {
    u32 tbl = gabi::load<u32>(q + 4);
    u32 ent = tbl + 0x10 + gabi::load<u32>(tbl + 0x4C) * 0x1C;
    u32 prog = gabi::load<u32>(shp + 0xC) != 0 ? gabi::load<u32>(shp + 0x10) : 0;
    s32 a = gabi::load<s16>(prog + 0xC);
    u32 r23 = gabi::load<u32>(ent + 4);
    u32 r22 = gabi::load<u32>(ent + 0xC);
    s32 b = gabi::load<s16>(prog + 0xE);
    s32 d = gabi::load<s16>(prog + 0x10);
    if (a == -1 && b == -1 && d == -1)
        return;
    if (b != -1)
        gabi::call(0xC0006900 /* GX2SetPixelUniformBlock */, b, r22, r23);
    if (a != -1)
        gabi::call(0xC0006A38 /* GX2SetVertexUniformBlock */, a, r22, r23);
    if (d != -1)
        gabi::call(0xC00068A8 /* GX2SetGeometryUniformBlock */, d, r22, r23);
}

/* 0210508C: daBwdg_packet_c::draw (HD virtual, vtable 0x1000C1F4 +0x2C; (this, draw context)) */
static void bwdg_packet_draw(void* pkt, void* ctx) {
    WWHD_FUNC(0x0210508C, void, pkt, ctx);
    u32 p = gabi::ea(pkt);
    u32 x = gabi::ea(ctx);
    u32 B = p + 0x18CE4;
    u32 shp = 0;
    s32 mode = gabi::load<s32>(x + 0xC);
    if (mode < 4) {
        u32 n = gabi::load<u32>(B + 4);
        u32 arr = gabi::load<u32>(B + 8);
        if ((u32)mode < n)
            arr += mode * 0x14;
        shp = gabi::load<u32>(arr);
    }
    u32 st = gabi::call<u32>(0x027F29D4, 0x104B45C0);
    u32 prog = gabi::load<u32>(shp);
    if (prog != gabi::load<u32>(st + 4)) {
        u8 f = gabi::load<u8>(prog);
        u32 cur = gabi::load<u32>(st);
        if (f & 2) {
            gabi::store<u8>(prog, (u8)(f & ~2));
            gabi::call(0x027BB9E0, prog, 0);
        }
        u32 grp = gabi::load<u32>(gabi::load<u32>(prog + 0x7C) + 0x28);
        if (cur != grp)
            gabi::call(0x027B9F68, grp);
        if (gabi::load<u32>(prog + 0xC) != 0) {
            gabi::call(0xC00060E0 /* GX2CallDisplayList */, gabi::load<u32>(prog + 4), gabi::load<u32>(prog + 0xC));
            gabi::store<u32>(st, grp);
            gabi::store<u32>(st + 4, prog);
        } else {
            gabi::call(0x027BB7CC, prog);
            gabi::store<u32>(st + 4, prog);
            gabi::store<u32>(st, grp);
        }
    }
    mode = gabi::load<s32>(x + 0xC);
    if (mode == 0)
        return;
    if (mode == 1 || mode == 2) {
        u32 q = B + 0x4CC;
        bwdg_bind_shader(q, shp);
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(q + 0xC0) + 0x2C), q + 0xB4, shp);
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(q + 0x18) + 0x2C), q + 0xC, shp);
        if (mode == 2) {
            u32 o = gabi::load<u32>(x + 0x30);
            if (o != 0)
                gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(o + 0xC) + 0x2C), o, shp);
            gabi::call(0x027FFE54, x, shp);
        }
    }
    u32 vb = gabi::load<u32>(shp + 0x14) != 0 ? gabi::load<u32>(shp + 0x18) : 0;
    gabi::call(0x027BE53C, B + 0x900, vb + 4, 0, 0);
    {
        gabi::Local<bwdg_draw_state> s;
        u32 a = gabi::ea(s.get());
        gabi::call(0x02750250, s.get());
        u32 v = gabi::load<u32>(a + 0xEC);
        gabi::store<u32>(a + 0xC, 2);
        gabi::store<u8>(a + 0xE0, 0);
        gabi::store<u32>(a + 0x8, 0);
        gabi::store<u32>(a + 0xEC, ((((v & ~0xFu) + 7) & ~0xF0u) + 0x10));
        gabi::call(0x0280037C, gabi::load<u32>(x + 0xC), s.get());
        gabi::call(0x02750370, s.get());
    }
    {
        u32 m = gabi::load<u32>(x + 0xC);
        u32 n = gabi::load<u32>(B + 4);
        u32 arr = gabi::load<u32>(B + 8);
        if (m < n)
            arr += m * 0x14;
        u32 off = gabi::load<u32>(p + 0x19198) == 0 ? 8 : 0; /* the buffer just filled */
        gabi::call(0x027BFE5C, gabi::load<u32>(arr + off + 8));
    }
    if (gabi::load<u32>(B + 0x8F4) != 0) {
        u32 g = B + 0x8E8;
        gabi::call(0xC0006178 /* GX2DrawIndexedEx */, gabi::load<u32>(g + 4), gabi::load<u32>(g + 0xC), gabi::load<u32>(g), gabi::load<u32>(g + 8), 0, 1);
    }
    gabi::call(0x02750370, 0x104B45C0 + 0x18C);
}
VERIFY(0x0210508C, bwdg_packet_draw);

/* heap allocation through the sead heap (vtable +0x34: alloc(size, align)) */
static inline u32 bwdg_heap_alloc(u32 size, s32 align) {
    u32 heap = gabi::call<u32>(0x02756140, gabi::load<u32>(0x101F8B4C));
    return gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(heap + 0xC) + 0x34), heap, size, align);
}

/* 021045E8: daBwdg_packet_c::daBwdg_packet_c (HD; allocates when this == NULL): J3DPacket base,
 * the shader/GPU state objects and two 0x41000-byte vertex buffers */
static void* bwdg_packet_ct(void* pkt) {
    WWHD_FUNC(0x021045E8, void*, pkt);
    if (pkt == nullptr) {
        pkt = operator_new(0x1980C);
        if (pkt == nullptr)
            return nullptr;
    }
    u32 p = gabi::ea(pkt);
    gabi::call(0x027F1278, p); /* J3DPacket::J3DPacket */
    u32 B = p + 0x18CE4;
    gabi::store<u32>(p + 0xC, 0x1000C1F4);
    gabi::store<u32>(B, 0);
    {
        u32 b4 = B + 4;
        if (b4 == 0) {
            b4 = gabi::ea(operator_new(8));
            if (b4 != 0) {
                gabi::store<u32>(b4 + 4, 0);
                gabi::store<u32>(b4, 0);
            }
        } else {
            gabi::store<u32>(b4 + 4, 0);
            gabi::store<u32>(b4, 0);
        }
    }
    u32 c = B + 0xC;
    {
        u32 r31 = c;
        if (r31 == 0)
            r31 = gabi::ea(operator_new(0x4C0));
        if (r31 != 0) {
            gabi::call(0x028EFFD0, r31, 2, 0x254, 0x02105520); /* __construct_array */
            gabi::store<u32>(r31 + 0x4A8, 0);
            gabi::store<u32>(r31 + 0x4AC, 0);
            gabi::store<u32>(r31 + 0x4B8, 0);
            gabi::store<u32>(r31 + 0x4B0, 0x20);
            gabi::store<u8>(r31 + 0x4BC, 0);
            gabi::store<u32>(r31, 0);
            gabi::store<u32>(r31 + 0x254, 0);
        }
    }
    u32 q = B + 0x4CC;
    gabi::call(0x027FD6F4, q);
    gabi::call(0x027FB40C, q + 0xC);
    gabi::store<u32>(q + 0x18, 0x1016EF84);
    gabi::call(0x028F521C, q + 0x80, 0x34);
    if (q + 0x80 == 0)
        operator_new(0x30);
    gabi::call(0x027FB40C, q + 0xB4);
    gabi::store<u32>(q + 0xC0, 0x1016EFB4);
    gabi::call(0x028F521C, q + 0x128, 0x2F0);
    {
        f32 z = gabi::load<f32>(0x10145180), o = gabi::load<f32>(0x1014517C);
        static const u16 ones[] = {0x154, 0x144, 0x1A4, 0x164, 0x1B4, 0x184, 0x194, 0x134, 0x174, 0x1C4, 0x1D4};
        for (u32 off = 0x128; off <= 0x1D4; off += 4) {
            bool one = false;
            for (u16 v : ones) one |= (v == off);
            gabi::store<f32>(q + off, one ? o : z);
        }
    }
    gabi::call(0x028EFFD0, q + 0x1D8, 2, 0x10, 0x0210557C);
    gabi::call(0x028EFFD0, q + 0x1F8, 2, 0x10, 0x0210557C);
    gabi::call(0x028EFFD0, q + 0x218, 2, 0x10, 0x0210557C);
    {
        static const u16 offs[] = {0x110, 0x140, 0x170, 0x1A0, 0x1D0, 0x200, 0x230, 0x260, 0x290, 0x2A0, 0x2B0, 0x2C0, 0x2D0, 0x2E0};
        for (int i = 0; i < 14; i++)
            if (q + 0x128 + offs[i] == 0)
                operator_new(i < 8 ? 0x30 : 0x10);
    }
    u32 g8e8 = B + 0x8E8, g900 = B + 0x900, ga98 = B + 0xA98;
    gabi::store<u8>(q + 0x418, 0);
    gabi::call(0x027B5430, g8e8);
    gabi::call(0x027BDF7C, g900);
    gabi::call(0x027BE6B8, ga98);
    {
        gabi::Local<SafeString> name;
        name->mStringTop = 0x1000C1A0;
        name->__vtbl = 0x1000C114;
        u32 r21 = gabi::call<u32>(0x027FFCBC);
        s32 idx = gabi::call<s32>(0x027B90AC, gabi::load<u32>(r21 + 4), name.get());
        u32 r12;
        if (idx < 0) {
            r12 = 0;
        } else {
            u32 n = gabi::load<u32>(r21 + 8);
            r12 = gabi::load<u32>(r21 + 0xC);
            u32 ent = (u32)idx < n ? r12 + idx * 0x24 : r12;
            if (gabi::load<u8>(ent + 0x20) == 0) {
                u32 t = gabi::load<u32>(r21 + 4);
                u32 prm = (u32)idx < gabi::load<u32>(t + 0x1C) ? gabi::load<u32>(t + 0x20) + idx * 0x84 : 0;
                gabi::call(0x02800B0C, ent, prm, 0);
                n = gabi::load<u32>(r21 + 8);
                r12 = gabi::load<u32>(r21 + 0xC);
            }
            if ((u32)idx < n)
                r12 += idx * 0x24;
        }
        gabi::call(0x0280068C, B, r12, 0);
        gabi::store<u32>(c + 0x4AC, 0x13);
        gabi::store<u32>(c + 0x4B4, 0x1000C1E8);
    }
    for (int k = 0; k < 2; k++) {
        u32 e = c + k * 0x254;
        if (gabi::load<u32>(e) == 0) {
            u32 r = bwdg_heap_alloc(0x41000, 0x40);
            if (r != 0) {
                gabi::store<u32>(e + 0x250, r);
                gabi::store<u32>(e + 0x24C, 0x2080);
            }
            gabi::store<u32>(e, gabi::load<u32>(e + 0x250));
        }
        gabi::call(0x027FF478, e + 4, gabi::load<u32>(e), 0x2080, c + 0x4AC);
    }
    gabi::store<u32>(c + 0x4B8, 0);
    gabi::store<u8>(c + 0x4BC, 1);
    for (u32 i = 0; i < gabi::load<u32>(B); i++) {
        u32 e = gabi::load<u32>(B + 8);
        if (i < gabi::load<u32>(B + 4))
            e += i * 0x14;
        u32 a = gabi::load<u32>(e + 8);
        u32 r21 = gabi::load<u32>(e);
        gabi::store<u32>(e, 0);
        if (a != 0)
            bwdg_objarr_free(e, 4, 8);
        if (gabi::load<u32>(e + 0x10) != 0)
            bwdg_objarr_free(e, 0xC, 0x10);
        gabi::store<u32>(e, r21);
        for (int k = 0; k < 2; k++) {
            u32 o = bwdg_heap_alloc(0xF4, 4);
            if (o != 0)
                gabi::call(0x027BF734, o);
            if (o != 0) {
                gabi::store<u32>(e + 8 + 8 * k, o);
                gabi::store<u32>(e + 4 + 8 * k, 1);
            }
        }
        for (int k = 0; k < 2; k++)
            gabi::call(0x027FF530, r21, gabi::load<u32>(e + 8 + 8 * k), c + 4 + k * 0x254, c + 0x4AC, 0);
    }
    gabi::call(0x027FE084, q, 1, 0);
    gabi::call(0x027B54E0, g8e8, 0x101A7820, 4, gabi::load<u32>(0x101930F4));
    gabi::store<u32>(g8e8 + 4, 4);
    u32 shader;
    {
        gabi::Local<SafeString> s1;
        gabi::Local<SafeString> s2;
        u32 mgr = gabi::load<u32>(0x101F4F7C);
        s2->mStringTop = 0x1000C1B8;
        s2->__vtbl = 0x1000C114;
        s1->mStringTop = 0x1000C1A8;
        s1->__vtbl = 0x1000C114;
        shader = gabi::call<u32>(0x026124B0, mgr, s1.get(), s2.get(), 0);
    }
    gabi::call(0x0274FBF8, gabi::load<u32>(0x101F8B18));
    gabi::call(0x02773870, ga98, shader, 0x1000C198);
    gabi::call(0x0274FCCC, gabi::load<u32>(0x101F8B18));
    static const u8 cmp[] = {0x4, 0x8, 0xC, 0x10, 0x14, 0x18, 0x38, 0x34, 0x1C};
    bool same = true;
    for (u8 o : cmp)
        if (same && gabi::load<u32>(g900 + o) != gabi::load<u32>(ga98 + o))
            same = false;
    if (!same) {
        gabi::call(0x027BDEB4, g900, ga98);
        u8 b = gabi::load<u8>(g900 + 0x190);
        gabi::store<u32>(g900 + 0x160, 1);
        gabi::store<u32>(g900 + 0x15C, 1);
        gabi::store<u32>(g900 + 0x164, 1);
        gabi::store<u8>(g900 + 0x190, (u8)(b | 2));
    } else {
        u8 b = gabi::load<u8>(g900 + 0x190);
        u32 v28 = gabi::load<u32>(ga98 + 0x28), v30 = gabi::load<u32>(ga98 + 0x30);
        gabi::store<u32>(g900 + 0x28, v28);
        gabi::store<u8>(g900 + 0x190, (u8)(b | 2));
        gabi::store<u32>(g900 + 0xDC, v30);
        gabi::store<u32>(g900 + 0x30, v30);
        gabi::store<u32>(g900 + 0x164, 1);
        gabi::store<u32>(g900 + 0xD4, v28);
        gabi::store<u32>(g900 + 0x160, 1);
        gabi::store<u32>(g900 + 0x15C, 1);
    }
    return pkt;
}
VERIFY(0x021045E8, bwdg_packet_ct);
