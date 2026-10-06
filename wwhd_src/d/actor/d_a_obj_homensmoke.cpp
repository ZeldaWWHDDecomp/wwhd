/**
 * d_a_obj_homensmoke.cpp (WWHD)
 * Object - Smoke and rubble of a destroyed (Wind Temple) wall/mask.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_homensmoke.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define ACT_VTBL 0x1002AD50  /* daObjHomensmoke::Act_c vtable (HD) */
#define culling_dat 0x101CA130 /* {Vec min, max}[2] */
#define rate_table 0x101CA128  /* f32[2] {1.0, 0.5} */

enum { dPa_name_ID_AK_JT_ELEMENTSMOKE01 = 0x2027, dPa_name_ID_AK_SN_KAZEMASKHAHEN00 = 0x81B1 }; /* HD ids */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02349DFC daObj::PrmAbstract<int>(actor, width, shift) (shared out-of-line copy) */
static inline u32 daObj_PrmAbstract_i(fopAc_ac_c* a, s32 w, s32 s) { return gabi::call<u32>(0x02349DFC, a, w, s); }
/* 025F1C5C mDoMtx_ZrotM(Mtx, s16) */
static inline void mDoMtx_ZrotM_(Mtx34* m, s16 z) { gabi::call(0x025F1C5C, m, z); }
/* 028249B0 JPASetRMtxTVecfromMtx(const Mtx src, Mtx rotOut, Vec* transOut) */
static inline void JPASetRMtxTVecfromMtx(const Mtx34* m, u32 r, u32 t) { gabi::call(0x028249B0, m, r, t); }

namespace daObjHomensmoke {
struct Act_c : fopAc_ac_c {
    int param_get_arg0() { return daObj_PrmAbstract_i(this, 1, 0) & 0x1; }
    int param_get_axis() { return daObj_PrmAbstract_i(this, 1, 1) & 0x1; }
    void set_mtx();
    cPhs_State _create();
    bool _delete();
    bool _execute();

    /* 0x3AC */ u8 m3AC[0x3B4 - 0x3AC];
    /* 0x3B4 */ Mtx34 mMtx;
    /* 0x3E4 */ be<s32> mType;
    /* 0x3E8 */ u8 mSmokeCb[0x20]; /* dPa_smokeEcallBack: vtable +0, emitter +4, end flag +0x10 bit 0 */
    /* 0x408 */ be<BOOL> mbInitialized;
    /* 0x40C */ cXyz mSmokePos;
};
WWHD_OFFSET(Act_c, mMtx, 0x3B4);
WWHD_OFFSET(Act_c, mSmokeCb, 0x3E8);
WWHD_OFFSET(Act_c, mSmokePos, 0x40C);

/* 02359ACC */
void Act_c::set_mtx() {
    WWHD_FUNC(0x02359ACC, void, this);
    int axis = param_get_axis();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    if (axis != 0) {
        mDoMtx_stack_c::transM(0.0f, 0.0f, 200.0f);
        mDoMtx_ZrotM_(mDoMtx_stack_c::get(), shape_angle.z);
        mDoMtx_YrotM(mDoMtx_stack_c::get(), shape_angle.y);
        mDoMtx_XrotM(mDoMtx_stack_c::get(), shape_angle.x);
        mDoMtx_stack_c::transM(0.0f, 0.0f, -200.0f);
        PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
    } else {
        mDoMtx_ZrotM_(mDoMtx_stack_c::get(), shape_angle.z);
        mDoMtx_YrotM(mDoMtx_stack_c::get(), shape_angle.y);
        mDoMtx_XrotM(mDoMtx_stack_c::get(), shape_angle.x);
        PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
    }
}
VERIFY(0x02359ACC, &Act_c::set_mtx);

/* tevStr = parent->tevStr: the HD dKy_tevstr_c copy assignment, member by member (floats through
 * FPRs, the rest as integers; some HD members are not copied) */
static void tevstr_copy(u32 dst, u32 src) {
    struct R { u16 off; u8 n; char k; };
    static const R runs[] = {
        {0x110, 6, 'f'}, {0x128, 4, 'b'}, {0x12C, 4, 'h'}, {0x134, 8, 'f'}, {0x194, 3, 'w'}, {0x1A0, 4, 'h'},
        {0x1A8, 2, 'w'}, {0x1B0, 4, 'h'}, {0x1B8, 3, 'f'}, {0x1C4, 9, 'b'}, {0x1D0, 6, 'f'}, {0x1E8, 4, 'b'},
        {0x1EC, 4, 'h'}, {0x1F4, 8, 'f'}, {0x254, 6, 'f'}, {0x26C, 4, 'b'}, {0x270, 4, 'h'}, {0x278, 8, 'f'},
    };
    for (const R& r : runs) {
        for (u32 i = 0; i < r.n; i++) {
            switch (r.k) {
            case 'f': gabi::store<f32>(dst + r.off + 4 * i, gabi::load<f32>(src + r.off + 4 * i)); break;
            case 'w': gabi::store<u32>(dst + r.off + 4 * i, gabi::load<u32>(src + r.off + 4 * i)); break;
            case 'h': gabi::store<u16>(dst + r.off + 2 * i, gabi::load<u16>(src + r.off + 2 * i)); break;
            default: gabi::store<u8>(dst + r.off + i, gabi::load<u8>(src + r.off + i)); break;
            }
        }
    }
}

/* 02359BE0 */
cPhs_State Act_c::_create() {
    WWHD_FUNC(0x02359BE0, cPhs_State, this);
    /* fopAcM_ct(this, Act_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = ACT_VTBL;
            gabi::call(0x025A5B18, mSmokeCb, 1); /* dPa_smokeEcallBack::dPa_smokeEcallBack(1) */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    mbInitialized = FALSE;
    set_mtx();
    cullMtx = gabi::ea(&mMtx); /* fopAcM_SetMtx */
    s32 type = param_get_arg0();
    mType = type;
    u32 c = culling_dat + type * 0x18;
    fopAcM_setCullSizeBox(this, gabi::load<f32>(c), gabi::load<f32>(c + 4), gabi::load<f32>(c + 8),
                          gabi::load<f32>(c + 0xC), gabi::load<f32>(c + 0x10), gabi::load<f32>(c + 0x14));
    fopAc_ac_c* parent = fopAcM_SearchByID(parentActorID);
    if (parent) {
        tevstr_copy(gabi::ea(this), gabi::ea(parent));
    }
    return cPhs_COMPLEATE_e;
}
VERIFY(0x02359BE0, &Act_c::_create);

/* 02359FC8 */
bool Act_c::_delete() {
    WWHD_FUNC(0x02359FC8, bool, this);
    if (gabi::load<u32>(gabi::ea(mSmokeCb) + 4) != 0) { /* mSmokeCb.getEmitter() */
        u32 vt = gabi::load<u32>(gabi::ea(mSmokeCb));
        gabi::call_ptr(gabi::load<u32>(vt + 0x44), mSmokeCb); /* mSmokeCb.remove() (virtual) */
    }
    return true;
}
VERIFY(0x02359FC8, &Act_c::_delete);

/* 0235A004 */
bool Act_c::_execute() {
    WWHD_FUNC(0x0235A004, bool, this);
    if (!mbInitialized) {
        /* static cXyz norse_offsetL(0, 300, 20), norse_offsetS(0, 70, 20) (initialised on first use) */
        const u32 offL = 0x10469FC4, guardL = 0x10469FDC, offS = 0x10469FD0, guardS = 0x10469FE0;
        if (gabi::load<u32>(guardL) == 0) {
            gabi::store<f32>(offL + 8, 20.0f);
            gabi::store<u32>(guardL, 1);
            gabi::store<f32>(offL + 4, 300.0f);
            gabi::store<f32>(offL, 0.0f);
        }
        if (gabi::load<u32>(guardS) == 0) {
            gabi::store<f32>(offS + 8, 20.0f);
            gabi::store<u32>(guardS, 1);
            gabi::store<f32>(offS + 4, 70.0f);
            gabi::store<f32>(offS, 0.0f);
        }
        u32 off = mType == 0 ? offL : offS;
        PSMTXMultVec(&mMtx, gabi::at<cXyz>(off), &mSmokePos);

        s8 room = fopAcM_GetRoomNo(this);
        JPABaseEmitter* smokeEmitter = dPa_control_set(dComIfGp_getParticle(), 2, dPa_name_ID_AK_JT_ELEMENTSMOKE01, &mSmokePos,
                                                       nullptr, nullptr, 0xFF, (dPa_levelEcallBack*)mSmokeCb, room, nullptr,
                                                       nullptr, nullptr);
        u32 e = gabi::ea(smokeEmitter);
        if (e) {
            f32 rate = gabi::load<f32>(rate_table + 4 * (mType & 1));
            gabi::store<s32>(e + 0x5C, 1);       /* setMaxFrame(1) */
            gabi::store<u8>(e + 0x247, 0xB4);    /* setGlobalAlpha */
            gabi::store<f32>(e + 0x34, 50.0f);   /* setRate */
            f32 d = 5.0f * rate, z = 0.0f * rate, p = 6.0f * rate;
            gabi::store<f32>(e + 0x8, rate);     /* setEmitterScale(rate, 0, rate) */
            gabi::store<f32>(e + 0xC, z);
            gabi::store<f32>(e + 0x10, rate);
            gabi::store<f32>(e + 0x220, d);      /* setGlobalDynamicsScale */
            gabi::store<f32>(e + 0x224, d);
            gabi::store<f32>(e + 0x228, d);
            gabi::store<f32>(e + 0x238, p);      /* setGlobalParticleScale */
            gabi::store<f32>(e + 0x23C, p);
            gabi::store<f32>(e + 0x240, p);
        }

        JPABaseEmitter* rubbleEmitter = dPa_control_set(dComIfGp_getParticle(), 2, dPa_name_ID_AK_SN_KAZEMASKHAHEN00, &current.pos,
                                                        nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
        u32 r = gabi::ea(rubbleEmitter);
        if (r) {
            /* setGlobalPrmColor(tevStr.mColorK0) (HD: tevStr+0x98) */
            gabi::store<u8>(r + 0x244, gabi::load<u8>(gabi::ea(this) + 0x1A8));
            gabi::store<u8>(r + 0x245, gabi::load<u8>(gabi::ea(this) + 0x1A9));
            gabi::store<u8>(r + 0x246, gabi::load<u8>(gabi::ea(this) + 0x1AA));
            if (mType == 1) {
                for (u32 o : {0x8u, 0xCu, 0x10u, 0x220u, 0x224u, 0x228u, 0x238u, 0x23Cu, 0x240u})
                    gabi::store<f32>(r + o, 0.6f);
            }
            JPASetRMtxTVecfromMtx(&mMtx, r + 0x1F0, r + 0x22C); /* setGlobalRTMatrix */
        }
        mbInitialized = TRUE;
    } else if (gabi::load<u8>(gabi::ea(mSmokeCb) + 0x10) & 1) { /* mSmokeCb.isEnd() */
        fopAcM_delete(this);
    }
    return true;
}
VERIFY(0x0235A004, &Act_c::_execute);

/* method table entries (HD: tail branches; _draw inlined) */
/* 0235A270 */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x0235A270, cPhs_State, i_this);
    return ((Act_c*)i_this)->_create();
}
VERIFY(0x0235A270, Mthd_Create);
/* 0235A274 */
static bool Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x0235A274, bool, i_this);
    return ((Act_c*)i_this)->_delete();
}
VERIFY(0x0235A274, Mthd_Delete);
/* 0235A278 */
static bool Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x0235A278, bool, i_this);
    return ((Act_c*)i_this)->_execute();
}
VERIFY(0x0235A278, Mthd_Execute);
/* 0235A27C */
static BOOL Mthd_Draw(void*) {
    WWHD_FUNC(0x0235A27C, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0235A27C, Mthd_Draw);
/* 0235A36C */
static BOOL Mthd_IsDelete(void*) {
    WWHD_FUNC(0x0235A36C, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0235A36C, Mthd_IsDelete);

/* 0235A284 */
static void __sinit_d_a_obj_homensmoke_cpp() {
    WWHD_FUNC(0x0235A284, void, (u32)0);
    sinit_header_statics(0x10469FA8, 0x101CA160);
}
VERIFY(0x0235A284, __sinit_d_a_obj_homensmoke_cpp);

/* 0235A318: Act_c deleting destructor (HD: virtual ~Act_c() {}; mSmokeCb has no destructor call) */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    WWHD_FUNC(0x0235A318, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0235A318, Act_c_dt);
}  // namespace daObjHomensmoke
