/**
 * d_a_mmusic.cpp (WWHD)
 * Makar's music ("Macore" playing the cello: practice music and note particles)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_mmusic.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define ACT_VTBL 0x10014D58 /* HD: daMmusic::Act_c vtable */

enum { dItemNo_MASTER_SWORD_2_e = 0x3A };
enum { dPa_name_ID_AK_SN_MACOREMUSIC00 = 0x826C };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02520A84 dComIfGs_isStageBossEnemy(stageNo) */
static inline BOOL dComIfGs_isStageBossEnemy(s32 no) { return gabi::call<BOOL>(0x02520A84, no); }
/* 02520C0C dComIfGs_checkGetItem(u8) */
static inline BOOL dComIfGs_checkGetItem(u8 item) { return gabi::call<BOOL>(0x02520C0C, item); }
/* HD: dComIfGs_isEventBit: the dSv_event_c block is at *(0x101F84DC) + 0x644 (save info + 0x624) */
static inline BOOL dComIfGs_isEventBit_l(u16 flag) {
    return dSv_event_isEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), flag);
}
/* 025E1FCC mDoAud_checkCbPracticePlay(), 025E1FB0 mDoAud_cbPracticePlay(const cXyz*), 025E1FC0 mDoAud_cbPracticeStop() */
static inline BOOL mDoAud_checkCbPracticePlay() { return gabi::call<BOOL>(0x025E1FCC); }
static inline void mDoAud_cbPracticePlay(cXyz* pos) { gabi::call(0x025E1FB0, pos); }
static inline void mDoAud_cbPracticeStop() { gabi::call(0x025E1FC0); }
/* 025D6768 fopAcM_setCullSizeSphere, 025D6CE8 fopAcM_cullingCheck */
static inline void fopAcM_setCullSizeSphere(fopAc_ac_c* a, f32 x, f32 y, f32 z, f32 r) { gabi::call(0x025D6768, a, x, y, z, r); }
static inline s32 fopAcM_cullingCheck(fopAc_ac_c* a) { return gabi::call<s32>(0x025D6CE8, a); }
/* 028249B0 JPASetRMtxTVecfromMtx(mtx, rotMtx, transVec); JPABaseEmitter::setGlobalRTMatrix passes the
 * emitter's +0x1F0 / +0x22C (HD) */
static inline void JPASetRMtxTVecfromMtx(Mtx34* m, u32 r, u32 t) { gabi::call(0x028249B0, m, r, t); }
/* JPABaseEmitter flags (HD +0x254): stopCreateParticle |= 1, playCreateParticle &= ~1;
 * becomeInvalidEmitter: max frame (+0x5C) = -1, then stopCreateParticle */
static inline void JPABaseEmitter_stopCreateParticle(JPABaseEmitter* e) {
    u32 p = gabi::ea(e) + 0x254;
    gabi::store<u32>(p, gabi::load<u32>(p) | 1);
}
static inline void JPABaseEmitter_playCreateParticle(JPABaseEmitter* e) {
    u32 p = gabi::ea(e) + 0x254;
    gabi::store<u32>(p, gabi::load<u32>(p) & ~1u);
}
static inline void JPABaseEmitter_becomeInvalidEmitter(JPABaseEmitter* e) {
    u32 p = gabi::ea(e) + 0x254;
    u32 flags = gabi::load<u32>(p);
    gabi::store<s32>(gabi::ea(e) + 0x5C, -1);
    gabi::store<u32>(p, flags | 1);
}

namespace daMmusic {
struct Act_c : fopAc_ac_c {
    bool create_heap() { return true; }
    BOOL Macore_is_playing();
    void set_mtx();
    cPhs_State _create();
    bool _delete();
    void init_se();
    void manage_se(int cull);
    void delete_se();
    bool _execute();
    bool _draw() { return true; }

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ be<s32> field_0x298;
    /* 0x3B8 */ be<s32> field_0x29c;
    /* 0x3BC */ gptr<JPABaseEmitter> mpEmitter;
    /* 0x3C0 */ Mtx34 mMtx;
    /* 0x3F0 */ be<s16> mTimer;
};
WWHD_OFFSET(Act_c, mpEmitter, 0x3BC);
WWHD_OFFSET(Act_c, mTimer, 0x3F0);
}  // namespace daMmusic
using daMmusic::Act_c;

/* 021C5DBC: solidHeapCB (create_heap inlined) */
static BOOL solidHeapCB(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x021C5DBC, BOOL, (u32)0);
    return ((Act_c*)i_this)->create_heap();
}
VERIFY(0x021C5DBC, solidHeapCB);

/* 021C5E2C */
BOOL Act_c::Macore_is_playing() {
    WWHD_FUNC(0x021C5E2C, BOOL, this);
    if (dComIfGs_isStageBossEnemy(7 /* dSv_save_c::STAGE_WT */) ||
        dComIfGs_isEventBit_l(0x2910) ||
        dComIfGs_isEventBit_l(0x2E02) ||
        dComIfGs_isEventBit_l(0x1610) ||
        !dComIfGs_checkGetItem(dItemNo_MASTER_SWORD_2_e))
        return FALSE;

    return TRUE;
}
VERIFY(0x021C5E2C, &Act_c::Macore_is_playing);

/* 021C5DC4 */
void Act_c::set_mtx() {
    WWHD_FUNC(0x021C5DC4, void, this);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
}
VERIFY(0x021C5DC4, &Act_c::set_mtx);

/* 021C5EE4 */
cPhs_State Act_c::_create() {
    WWHD_FUNC(0x021C5EE4, cPhs_State, this);
    /* fopAcM_ct(this, Act_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = ACT_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    cPhs_State ret = cPhs_COMPLEATE_e;
    if (fopAcM_entrySolidHeap(this, 0x021C5DBC /* solidHeapCB */, 0)) {
        set_mtx();
        cullMtx = gabi::ea(&mMtx); /* fopAcM_SetMtx */
        field_0x298 = Macore_is_playing();
        fopAcM_setCullSizeSphere(this, 0.0f, 0.0f, 0.0f, 300.0f);
        init_se();
    } else {
        ret = cPhs_ERROR_e;
    }
    return ret;
}
VERIFY(0x021C5EE4, &Act_c::_create);

/* 021C5FB8 (HD: a tail branch) */
void Act_c::delete_se() {
    WWHD_FUNC(0x021C5FB8, void, (u32)0);
    mDoAud_cbPracticeStop();
}
VERIFY(0x021C5FB8, &Act_c::delete_se);

/* 021C5FBC */
bool Act_c::_delete() {
    WWHD_FUNC(0x021C5FBC, bool, this);
    if (mpEmitter != nullptr) {
        JPABaseEmitter_becomeInvalidEmitter(mpEmitter);
        mpEmitter = nullptr;
    }
    delete_se();
    return true;
}
VERIFY(0x021C5FBC, &Act_c::_delete);

/* 021C5ED8 */
void Act_c::init_se() {
    WWHD_FUNC(0x021C5ED8, void, this);
    mTimer = 120;
}
VERIFY(0x021C5ED8, &Act_c::init_se);

/* 021C6008 */
void Act_c::manage_se(int cull) {
    WWHD_FUNC(0x021C6008, void, this, cull);
    if (!mDoAud_checkCbPracticePlay()) {
        s16 t = mTimer;
        if (t > 0) {
            t = (s16)(t - 1);
            mTimer = t;
        }
        if (t == 0) {
            mDoAud_cbPracticePlay(&current.pos);
            mTimer = (s16)gabi::ftoi(gabi::fmadds(60.0f, cM_rndF(1.0f), 120.0f));
        } else if (cull == 0 && field_0x29c == 1) {
            JPABaseEmitter_stopCreateParticle(mpEmitter);
            field_0x29c = 0;
        }
    } else {
        if (cull == 0 && field_0x29c == 0) {
            JPABaseEmitter_playCreateParticle(mpEmitter);
            field_0x29c = 1;
        }
    }
}
VERIFY(0x021C6008, &Act_c::manage_se);

/* 021C612C */
bool Act_c::_execute() {
    WWHD_FUNC(0x021C612C, bool, this);
    if (mpEmitter == nullptr) {
        if (field_0x298 == 1) {
            set_mtx();
            /* GameCube: an unused cXyz scale(1, 1, 1) */
            JPABaseEmitter* emtr = dComIfGp_particle_set(dPa_name_ID_AK_SN_MACOREMUSIC00, &current.pos);
            mpEmitter = emtr;
            if (emtr != nullptr) {
                JPASetRMtxTVecfromMtx(&mMtx, gabi::ea(emtr) + 0x1F0, gabi::ea(emtr) + 0x22C); /* setGlobalRTMatrix */
                field_0x298 = 0;
                field_0x29c = 1;
            }
        } else {
            field_0x298 = Macore_is_playing();
        }
    } else {
        int cull = fopAcM_cullingCheck(this);
        if (cull == 1) {
            if (field_0x29c == 1) {
                JPABaseEmitter_stopCreateParticle(mpEmitter);
                field_0x29c = 0;
            }
        } else {
            if (field_0x29c == 0) {
                JPABaseEmitter_playCreateParticle(mpEmitter);
                field_0x29c = 1;
            }
        }
        manage_se(cull);
    }
    return true;
}
VERIFY(0x021C612C, &Act_c::_execute);

/* 021C6260 (out of line for the Mthd_Draw tail branch) */
static bool Act_c_draw(Act_c* i_this) {
    WWHD_FUNC(0x021C6260, bool, (u32)0);
    return i_this->_draw();
}
VERIFY(0x021C6260, Act_c_draw);

/* method table entries (HD: tail branches) */
/* 021C6268 */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x021C6268, cPhs_State, i_this);
    return ((Act_c*)i_this)->_create();
}
VERIFY(0x021C6268, Mthd_Create);
/* 021C626C */
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x021C626C, BOOL, i_this);
    return ((Act_c*)i_this)->_delete();
}
VERIFY(0x021C626C, Mthd_Delete);
/* 021C6270 */
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x021C6270, BOOL, i_this);
    return ((Act_c*)i_this)->_execute();
}
VERIFY(0x021C6270, Mthd_Execute);
/* 021C6274 */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x021C6274, BOOL, i_this);
    return Act_c_draw((Act_c*)i_this);
}
VERIFY(0x021C6274, Mthd_Draw);
/* 021C6360 */
static BOOL Mthd_IsDelete(void*) {
    WWHD_FUNC(0x021C6360, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021C6360, Mthd_IsDelete);

/* 021C6278 */
static void __sinit_d_a_mmusic_cpp() {
    WWHD_FUNC(0x021C6278, void, (u32)0);
    sinit_header_statics(0x1046547C, 0x101BA618);
}
VERIFY(0x021C6278, __sinit_d_a_mmusic_cpp);

/* 021C630C: Act_c deleting destructor (HD: virtual; ~fopAc_ac_c) */
static void Act_c_dt(Act_c* p, s32 flags) {
    WWHD_FUNC(0x021C630C, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c(this, 0) */
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x021C630C, Act_c_dt);
