/** Destroyed Wind Temple wall smoke/rubble. Local only. */
#include "d/actor/d_a_obj_homensmoke.h"
namespace daObjHomensmoke {
static CullBoxes culling_dat;
static RateTable rate_table;
static cXyz norse_offsetL;
static cXyz norse_offsetS;
static u32 norse_guardL;
static u32 norse_guardS;
enum { ID_AK_JT_ELEMENTSMOKE01 = 0x2027, ID_AK_SN_KAZEMASKHAHEN00 = 0x81B1 };
/* 02359ACC */
void Act_c::set_mtx() {
    int axis = param_get_axis();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    if (axis != 0) {
        mDoMtx_stack_c::transM(0.0f, 0.0f, 200.0f);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), shape_angle.z);
        mDoMtx_YrotM(mDoMtx_stack_c::get(), shape_angle.y);
        mDoMtx_XrotM(mDoMtx_stack_c::get(), shape_angle.x);
        mDoMtx_stack_c::transM(0.0f, 0.0f, -200.0f);
        PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
    } else {
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), shape_angle.z);
        mDoMtx_YrotM(mDoMtx_stack_c::get(), shape_angle.y);
        mDoMtx_XrotM(mDoMtx_stack_c::get(), shape_angle.x);
        PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
    }
}

/* 02359BE0 */
cPhs_State Act_c::_create() {
    /* fopAcM_ct(this, Act_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = 0x1002AD50;
            daObjHomensmoke_smokeCallback_ct(&mSmokeCb, 1);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    mbInitialized = FALSE;
    set_mtx();
    cullMtx = gabi::ea(&mMtx); /* fopAcM_SetMtx */
    s32 type = param_get_arg0();
    mType = type;
    const CullBox& c = culling_dat.boxes[type];
    fopAcM_setCullSizeBox(this, c.mMin.x, c.mMin.y, c.mMin.z, c.mMax.x, c.mMax.y, c.mMax.z);
    fopAc_ac_c* parent = fopAcM_SearchByID(parentActorID);
    if (parent) {
        // HD: fieldwise assignment omits unused light-object bytes.
        copyTev(tevStr, parent->tevStr);
    }
    return cPhs_COMPLEATE_e;
}

/* 02359FC8 */
bool Act_c::_delete() {
    if (mSmokeCb.getEmitter()) mSmokeCb.remove();
    return true;
}

/* 0235A004 */
bool Act_c::_execute() {
    if (!mbInitialized) {
        // HD: preserve first-use guard and component store ordering.
        if (!norse_guardL) {
            norse_offsetL.z = 20.0f; norse_guardL = 1;
            norse_offsetL.y = 300.0f; norse_offsetL.x = 0.0f;
        }
        if (!norse_guardS) {
            norse_offsetS.z = 20.0f; norse_guardS = 1;
            norse_offsetS.y = 70.0f; norse_offsetS.x = 0.0f;
        }
        const cXyz* offset = mType == 0 ? &norse_offsetL : &norse_offsetS;
        PSMTXMultVec(&mMtx, offset, &mSmokePos);

        s8 room = fopAcM_GetRoomNo(this);
        Emitter* smokeEmitter = reinterpret_cast<Emitter*>(dPa_control_set(dComIfGp_getParticle(), 2, ID_AK_JT_ELEMENTSMOKE01, &mSmokePos,
                                                       nullptr, nullptr, 0xFF, reinterpret_cast<dPa_levelEcallBack*>(&mSmokeCb), room, nullptr,
                                                       nullptr, nullptr));
        Emitter* e = smokeEmitter;
        if (e) {
            f32 rate = rate_table.rates[mType & 1];
            e->mMaxFrame = 1;       /* setMaxFrame(1) */
            e->mGlobalPrmColor.a = 0xB4;    /* setGlobalAlpha */
            e->mRate = 50.0f;   /* setRate */
            f32 d = 5.0f * rate, z = 0.0f * rate, p = 6.0f * rate;
            e->mEmitterScale.x = rate;     /* setEmitterScale(rate, 0, rate) */
            e->mEmitterScale.y = z;
            e->mEmitterScale.z = rate;
            e->mGlobalDynamicsScale.x = d;      /* setGlobalDynamicsScale */
            e->mGlobalDynamicsScale.y = d;
            e->mGlobalDynamicsScale.z = d;
            e->mGlobalParticleScale.x = p;      /* setGlobalParticleScale */
            e->mGlobalParticleScale.y = p;
            e->mGlobalParticleScale.z = p;
        }

        Emitter* rubbleEmitter = reinterpret_cast<Emitter*>(dPa_control_set(dComIfGp_getParticle(), 2, ID_AK_SN_KAZEMASKHAHEN00, &current.pos,
                                                        nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr));
        Emitter* r = rubbleEmitter;
        if (r) {
            r->mGlobalPrmColor.r = tevStr.mColorK0.r;
            r->mGlobalPrmColor.g = tevStr.mColorK0.g;
            r->mGlobalPrmColor.b = tevStr.mColorK0.b;
            if (mType == 1) {
                r->mEmitterScale.x = 0.6f; r->mEmitterScale.y = 0.6f; r->mEmitterScale.z = 0.6f;
                r->mGlobalDynamicsScale.x = 0.6f; r->mGlobalDynamicsScale.y = 0.6f; r->mGlobalDynamicsScale.z = 0.6f;
                r->mGlobalParticleScale.x = 0.6f; r->mGlobalParticleScale.y = 0.6f; r->mGlobalParticleScale.z = 0.6f;
            }
            daObjHomensmoke_setRTMatrix(&mMtx, &r->mGlobalRotation, &r->mGlobalTranslation); /* setGlobalRTMatrix */
        }
        mbInitialized = TRUE;
    } else if (mSmokeCb.m10 & 1) { /* mSmokeCb.isEnd() */
        fopAcM_delete(this);
    }
    return true;
}

/* method table entries (HD: tail branches; _draw inlined) */
/* 0235A270 */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->_create();
}
/* 0235A274 */
static bool Mthd_Delete(void* i_this) {
    return ((Act_c*)i_this)->_delete();
}
/* 0235A278 */
static bool Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->_execute();
}
/* 0235A27C */
static BOOL Mthd_Draw() {
    return TRUE;
}
/* 0235A36C */
static BOOL Mthd_IsDelete() {
    return TRUE;
}

/* 0235A284 */
static void __sinit_d_a_obj_homensmoke_cpp() {
    sinit_header_statics(0x10469FA8, 0x101CA160);
}

// HD: deleting destructor has no smoke-callback destructor call.
/* 0235A318 */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        daObjHomensmoke_actor_dt(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
}  // namespace daObjHomensmoke
using namespace daObjHomensmoke;

/* WWHD glue (tools/verify/autoglue.py)
data culling_dat 101CA130
data rate_table 101CA128
data norse_offsetL 10469FC4
data norse_offsetS 10469FD0
data norse_guardL 10469FDC
data norse_guardS 10469FE0
ext 02349DFC u32 daObjHomensmoke_getParam(fopAc_ac_c*, s32, s32)
ext 025A5B18 void daObjHomensmoke_smokeCallback_ct(dPa_smokeEcallBack*, u32)
ext 025D50BC void daObjHomensmoke_actor_dt(fopAc_ac_c*, u32)
ext 028249B0 void daObjHomensmoke_setRTMatrix(const Mtx34*, Mtx34*, cXyz*)
*/
