/**
 * d_a_mmusic.cpp (WWHD)
 * Makar's music ("Macore" playing the cello: practice music and note particles)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_mmusic.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_mmusic.h"

namespace dSv_event_flag_c {
enum { UNK_2910 = 0x2910, UNK_2E02 = 0x2E02, UNK_1610 = 0x1610 };
}
enum { dItemNo_MASTER_SWORD_2_e = 0x3A };
namespace dPa_name { enum { ID_AK_SN_MACOREMUSIC00 = 0x826C }; }
namespace { enum { STAGE_WT = 7 }; }
using daMmusic::Act_c;

// HD: inline base construction installs this TU's virtual destructor table.
Act_c::Act_c() {
    fopAc_ac_c_ct(this);
    __vtbl = 0x10014D58;
}

// HD:solidHeapCB (create_heap inlined)
/* 021C5DBC */
static BOOL solidHeapCB(fopAc_ac_c* i_this) {
    return ((Act_c*)i_this)->create_heap();
}

// HD:
/* 021C5E2C */
BOOL Act_c::Macore_is_playing() {
    if (dComIfGs_isStageBossEnemy(STAGE_WT) ||
        dComIfGs_isEventBit(dSv_event_flag_c::UNK_2910) ||
        dComIfGs_isEventBit(dSv_event_flag_c::UNK_2E02) ||
        dComIfGs_isEventBit(dSv_event_flag_c::UNK_1610) ||
        !dComIfGs_checkGetItem(dItemNo_MASTER_SWORD_2_e))
        return FALSE;

    return TRUE;
}

// HD:
/* 021C5DC4 */
void Act_c::set_mtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
}

// HD:
/* 021C5EE4 */
cPhs_State Act_c::_create() {
    fopAcM_ct(this, Act_c);

    cPhs_State ret = cPhs_COMPLEATE_e;
    if (fopAcM_entrySolidHeap(this, solidHeapCB, 0)) {
        set_mtx();
        fopAcM_SetMtx(this, &mMtx);
        field_0x298 = Macore_is_playing();
        fopAcM_setCullSizeSphere(this, 0.0f, 0.0f, 0.0f, 300.0f);
        init_se();
    } else {
        ret = cPhs_ERROR_e;
    }
    return ret;
}

// HD:(HD: a tail branch)
/* 021C5FB8 */
void Act_c::delete_se() {
    mDoAud_cbPracticeStop();
}

// HD:
/* 021C5FBC */
bool Act_c::_delete() {
    if (mpEmitter != nullptr) {
        mpEmitter->becomeInvalidEmitter();
        mpEmitter = nullptr;
    }
    delete_se();
    return true;
}

// HD:
/* 021C5ED8 */
void Act_c::init_se() {
    mTimer = 120;
}

// HD:
/* 021C6008 */
void Act_c::manage_se(int cull) {
    if (!mDoAud_checkCbPracticePlay()) {
        s16 t = mTimer;
        if (t > 0) {
            t = (s16)(t - 1);
            mTimer = t;
        }
        if (t == 0) {
            mDoAud_cbPracticePlay(&current.pos);
            // HD: fused multiply-add and PowerPC integer conversion.
            mTimer = (s16)gabi::ftoi(gabi::fmadds(60.0f, cM_rndF(1.0f), 120.0f));
        } else if (cull == 0 && field_0x29c == 1) {
            mpEmitter->stopCreateParticle();
            field_0x29c = 0;
        }
    } else {
        if (cull == 0 && field_0x29c == 0) {
            mpEmitter->playCreateParticle();
            field_0x29c = 1;
        }
    }
}

// HD:
/* 021C612C */
bool Act_c::_execute() {
    if (mpEmitter == nullptr) {
        if (field_0x298 == 1) {
            set_mtx();
            /* GameCube: an unused cXyz scale(1, 1, 1) */
            MmusicEmitter* emtr = reinterpret_cast<MmusicEmitter*>(dComIfGp_particle_set(dPa_name::ID_AK_SN_MACOREMUSIC00, &current.pos));
            mpEmitter = emtr;
            if (emtr != nullptr) {
                emtr->setGlobalRTMatrix(&mMtx);
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
                mpEmitter->stopCreateParticle();
                field_0x29c = 0;
            }
        } else {
            if (field_0x29c == 0) {
                mpEmitter->playCreateParticle();
                field_0x29c = 1;
            }
        }
        manage_se(cull);
    }
    return true;
}

// HD:(out of line for the Mthd_Draw tail branch)
/* 021C6260 */
static bool Act_c_draw(Act_c* i_this) {
    return i_this->_draw();
}

/* method table entries (HD: tail branches) */
// HD:
/* 021C6268 */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->_create();
}
// HD:
/* 021C626C */
static BOOL Mthd_Delete(void* i_this) {
    return ((Act_c*)i_this)->_delete();
}
// HD:
/* 021C6270 */
static BOOL Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->_execute();
}
// HD:
/* 021C6274 */
static BOOL Mthd_Draw(void* i_this) {
    return Act_c_draw((Act_c*)i_this);
}
// HD:
/* 021C6360 */
static BOOL Mthd_IsDelete(void*) {
    return TRUE;
}

// HD:
/* 021C6278 */
static void __sinit_d_a_mmusic_cpp() {
    sinit_header_statics(0x1046547C, 0x101BA618);
}

// HD:Act_c deleting destructor (HD: virtual; ~fopAc_ac_c)
/* 021C630C */
static void Act_c_dt(Act_c* p, s32 flags) {
    if (p != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(p));
        if (flags & 1)
            operator_delete(p);
    }
}

/* WWHD glue (tools/verify/autoglue.py)
ext 025E1FCC BOOL mDoAud_checkCbPracticePlay()
ext 025E1FB0 void mDoAud_cbPracticePlay(cXyz*)
ext 025E1FC0 void mDoAud_cbPracticeStop()
ext 025D6768 void fopAcM_setCullSizeSphere(fopAc_ac_c*, f32, f32, f32, f32)
ext 025D6CE8 s32 fopAcM_cullingCheck(fopAc_ac_c*)
ext 028249B0 void JPASetRMtxTVecfromMtx(Mtx34*, MmusicRotation*, cXyz*)
*/
