/* Heat-floor particles following an aim matrix. Private WWHD reconstruction. */
#include "d/actor/d_a_hot_floor.h"
namespace dPa_name {
enum { ID_AK_SN_HEATFLOOR00 = 0x8120, ID_AK_SN_HEATFLOOR01 = 0x814C };
}
/* 0217A0F4 */
void daHot_Floor_c::set_mtx_init() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    for (s32 i = 0; i < 5; i++)
        PSMTXCopy(mDoMtx_stack_c::get(), &mtx[i]); /* mDoMtx_copy */
}

/* 0217A280 */
void daHot_Floor_c::set_mtx() {
    for (s32 i = 4; i > 0; i--)
        PSMTXCopy(&mtx[i - 1], &mtx[i]);
    if (mtx_p != nullptr) {
        gabi::Local<cXyz> pos;
        f32 x = cXyz_Zero->x, y = cXyz_Zero->y, z = cXyz_Zero->z; /* cXyz pos = cXyz::Zero */
        pos->x = x;
        pos->y = y;
        pos->z = z;
        PSMTXCopy(mtx_p, mDoMtx_stack_c::get()); /* mDoMtx_stack_c::copy */
        mDoMtx_stack_c::transM(0.0f, 5.0f, -5.0f);
        PSMTXCopy(mDoMtx_stack_c::get(), &mtx[0]);
        PSMTXMultVec(mDoMtx_stack_c::get(), pos.get(), &current.pos); /* mDoMtx_stack_c::multVec */
        if (mEmitter2 != nullptr)
            mEmitter2->setGlobalRTMatrix(&mtx[0]);
        if (mEmitter1 != nullptr)
            mEmitter1->setGlobalRTMatrix(&mtx[0]);
    }
    mtx_p = nullptr;
}

/* 0217A178 */
cPhs_State daHot_Floor_c::CreateInit() {
    mbSpawnParticle = true;
    mSpawnTimer = 0.0f;
    set_mtx_init();
    fopAcM_SetMtx(this, &mtx[0]);
    return cPhs_COMPLEATE_e;
}

/* 0217A1C4 */
// HD: daHot_FloorCreate (_create inlined)
static cPhs_State daHot_FloorCreate(void* i_this) {
    daHot_Floor_c* a = (daHot_Floor_c*)i_this;
    fopAcM_ct(a, daHot_Floor_c);
    return a->CreateInit();
}

/* 0217A238 */
// HD: daHot_FloorDelete (_delete inlined)
static BOOL daHot_FloorDelete(void* i_this) {
    daHot_Floor_c* a = (daHot_Floor_c*)i_this;
    if (a->mEmitter2 != nullptr)
        a->mEmitter2->becomeInvalidEmitter();
    if (a->mEmitter1 != nullptr)
        a->mEmitter1->becomeInvalidEmitter();
    return TRUE;
}

/* 0217A380 */
// HD: daHot_FloorExecute (_execute inlined)
static BOOL daHot_FloorExecute(void* i_this) {
    daHot_Floor_c* a = (daHot_Floor_c*)i_this;
    if (a->mbSpawnParticle) {
        if (a->mEmitter2 == nullptr && !(fopAcM_GetParam(a) & 1))
            a->mEmitter2 = (hot_floor_emitter_c*)dComIfGp_particle_set(dPa_name::ID_AK_SN_HEATFLOOR01, &a->current.pos);
        if (a->mEmitter1 == nullptr && !(fopAcM_GetParam(a) & 2))
            a->mEmitter1 = (hot_floor_emitter_c*)dComIfGp_particle_set(dPa_name::ID_AK_SN_HEATFLOOR00, &a->current.pos);
        cLib_chaseF(&a->mSpawnTimer, 60.0f, 5.0f);
        a->mbSpawnParticle = false;
    } else {
        cLib_chaseF(&a->mSpawnTimer, 0.0f, 0.4f);
        if (a->mSpawnTimer < 20.0f) {
            if (a->mEmitter2 != nullptr) {
                a->mEmitter2->becomeInvalidEmitter();
                a->mEmitter2 = nullptr;
            }
            if (a->mEmitter1 != nullptr) {
                a->mEmitter1->becomeInvalidEmitter();
                a->mEmitter1 = nullptr;
            }
        }
        // HD: unordered (NaN) also deletes, unlike the GameCube <= comparison.
        if (!(a->mSpawnTimer > 0.4f))
            fopAcM_delete(a);
    }

    a->set_mtx();
    return FALSE;
}

/* 0217A55C */
static BOOL daHot_FloorDraw(void* i_this) {
    return TRUE;
}

/* 0217A5F8 */
static BOOL daHot_FloorIsDelete(void* i_this) {
    return TRUE;
}

/* 0217A564 */
static void __sinit_d_a_hot_floor_cpp() {
    sinit_header_statics(0x10464760, 0x101B7770);
}

/* 0217A600 */
// HD: daHot_Floor_c deleting destructor (the animation members have trivial destructors)
static void daHot_Floor_c_dt(daHot_Floor_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        hot_floor_actor_dt(i_this, 0);
        if (flags & 1)
            operator_delete(i_this);
    }
}

daHot_Floor_c::daHot_Floor_c() {
    // HD: explicit base and animation construction, with the HD virtual table.
    fopAc_ac_c_ct(this);
    __vtbl = __vt;
    hot_floor_brk_ct(&mBrkAnm);
    mDoExt_btkAnm::ct(&mBtkAnm);
}
/* WWHD glue (tools/verify/autoglue.py): private HD bindings
ext 025E80D0 void hot_floor_brk_ct(void*)
ext 025D50BC void hot_floor_actor_dt(void*, s32)
ext 028249B0 void hot_floor_setRTMatrix(Mtx34*, hot_floor_rotation_c*, cXyz*)
*/
