/* WWHD traveling merchant hats. */
#include "d/actor/d_a_obj_hat.h"

const gc_tu_data gc_tu = {
    .safeString = 0x10029EAC, .objAcch = {0x10029ED4, 0x10029EF4, 0x10029EE4}, .objAcchDt = {0x10029EE4, 0x10029EF4}};
static const char* M_arcname = "Ro";

// HD: numeric resource IDs and packed collision words are checked against the image.
static const s32 l_bmd_ix_tbl[] = {23, 24, 25, 23};
static const s32 l_bck_ix_tbl[] = {18, 16, 17, 18};
static const u32 l_cyl_src[] = {0, 0, 0, 0, 2097152, 3, 121, 0, 0, 0, 6, 0, 0, 0, 0, 1116471296, 1117782016};
static daObjHat_c::MoveEntry moveProc[] = {{&daObjHat_c::executeNormal}};
/* 02350DA0 */
static u32 PrmAbstract(daObjHat_c* p, u32 width, u32 shift) {
    u32 parameters = p->mParameters;
    u32 mask = (width & 32) ? 0 : (1U << (width & 31));
    u32 value = (shift & 32) ? 0 : (parameters >> (shift & 31));
    return value & (mask - 1);
}

/* 023505B4 */
daObjHat_c::daObjHat_c() {
    fopAc_ac_c_ct(this);
    __vtbl = 0x10029F24;
    __ct(&mAcch);
    dBgS_AcchCir_ct(&mAcchCir);
    dCcD_Stts_ct(&mStts);
    dCcD_Cyl_ct(&mCyl, 0x10029EC4);
    mHatNo = getPrmHatNo();
    mState = 0;
}

/* 02350584 */
static BOOL CheckCreateHeap(fopAc_ac_c* p) { return static_cast<daObjHat_c*>(p)->createHeap(); }

/* 0235096C */
s32 daObjHat_c::_create() {
    // HD: construction is an out-of-line guest call, not an inlined placement new.
    if (!(actor_condition & fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0)
            daObjHat_construct(this);
        actor_condition = actor_condition | fopAcCnd_INIT_e;
    }
    s32 phaseResult = dComIfG_resLoad(&mPhs, M_arcname);
    if (phaseResult == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(this, CheckCreateHeap, 0))
            return createInit();
        return cPhs_ERROR_e;
    }
    return phaseResult;
}

/* 0235042C */
BOOL daObjHat_c::createHeap() {
    s32 modelID = l_bmd_ix_tbl[mHatNo];
    auto data = static_cast<J3DModelData*>(dComIfG_getObjectIDRes(M_arcname, modelID));
    if (data == nullptr)
        return FALSE;
    s32 animationID = l_bck_ix_tbl[mHatNo];
    auto animation = static_cast<J3DAnmTransform*>(dComIfG_getObjectIDRes(M_arcname, animationID));
    auto created = mDoExt_McaMorf::create(nullptr, data, nullptr, nullptr, animation, 2, 1.0f, 0, -1, 1, nullptr,
                                          0x80000, 0x37441422);
    mpMorf = created;
    if (created == nullptr || created->mpModel == nullptr)
        return FALSE;
    mpModel = created->mpModel;
    mAcchCir.SetWall(30.0f, 30.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, &current.angle, &shape_angle);
    return TRUE;
}

/* 02350878 */
s32 daObjHat_c::createInit() {
    mStts.Init(2, 0xFF, this);
    mCyl.Set((const dCcD_SrcCyl*)l_cyl_src);
    mCyl.SetStts(&mStts);
    fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
    fopAcM_setCullSizeBox(this, -50.0f, 0.0f, -50.0f, 50.0f, 200.0f, 50.0f);
    gravity = -5.0f;
    gabi::Local<gptr<fopAc_ac_c>> parent;
    if (fopAcM_SearchByID(parentActorID, parent.get())) {
        fopAc_ac_c* actor = *parent;
        if (actor != nullptr) {
            gabi::Local<cXyz> wind;
            wind->x = static_cast<HatMerchantView*>(actor)->mWindVec.x;
            wind->y = static_cast<HatMerchantView*>(actor)->mWindVec.y;
            wind->z = static_cast<HatMerchantView*>(actor)->mWindVec.z;
            setSpeed(wind);
        }
    }
    setMtx();
    return cPhs_COMPLEATE_e;
}

/* 02350A18 */
BOOL daObjHat_c::_delete() {
    dComIfG_resDelete(&mPhs, M_arcname);
    return TRUE;
}

/* 02350BF0 */
BOOL daObjHat_c::_draw() {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    auto light = dKy_getEnvlight();
    setLightTevColorType(light, mpModel, &tevStr);
    mDoExt_McaMorf* animation = mpMorf;
    animation->updateDL();
    return TRUE;
}

/* 02350A4C */
BOOL daObjHat_c::_execute() {
    // HD has one movement mState, so its PTMF entry is fixed.
    (this->*moveProc[0].proc)();
    f32 horizontal = speedF;
    f32 vx = mMoveNorm.x * horizontal;
    f32 vy = speed.y + gravity;
    f32 minimum = maxFallSpeed;
    f32 vz = mMoveNorm.z * horizontal;
    if (vy < minimum)
        vy = minimum;
    speed.x = vx;
    speed.y = vy;
    speed.z = vz;
    fopAcM_posMove(this, &mStts);
    mAcch.CrrPos(dComIfG_Bgsp());
    if (mAcch.ChkWallHit())
        speedF = 0.0f;
    if (mAcch.ChkGroundHit())
        cLib_addCalc(&speedF, 0.0f, 0.3f, 1000.0f, 1.0f);
    mCyl.SetC(&current.pos);
    dComIfG_Ccsp_Set(&mCyl);
    if (mCyl.ChkTgHit()) {
        gabi::Local<cXyz> wind;
        wind->z = mCyl.mGObjTg.mRVec.z;
        wind->y = mCyl.mGObjTg.mRVec.y;
        wind->x = mCyl.mGObjTg.mRVec.x;
        setSpeed(wind);
    }
    setMtx();
    return TRUE;
}

/* 02350CFC */
void daObjHat_c::executeNormal() {}

/* 02350588 */
u32 daObjHat_c::getPrmHatNo() { return PrmAbstract(this, PRM_HAT_NO_W, PRM_HAT_NO_S) & 3; }

/* 02350778 */
void daObjHat_c::setMtx() {
    // HD: retain all scale loads before model stores.
    f32 x = scale.x, y = scale.y, z = scale.z;
    J3DModel* m = mpModel;
    m->mBaseScale.x = x;
    m->mBaseScale.y = y;
    m->mBaseScale.z = z;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)(current.angle.y + 0x4000));
    mDoMtx_stack_c::XrotM(current.angle.x);
    mDoMtx_stack_c::ZrotM(current.angle.z + 0x4000);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
}

/* 023506CC */
void daObjHat_c::setSpeed(cXyz* velocity) {
    // HD: GHS passes tww's by-value vector through its mutable argument copy.
    velocity->y = 0.0f;
    cXyz normalized = velocity->normZP();
    f32 x = normalized.x, y = normalized.y, z = normalized.z;
    velocity->z = z;
    velocity->y = y;
    velocity->x = x;
    mMoveNorm.x = x;
    mMoveNorm.y = y;
    speedF = 20.0f;
    mMoveNorm.z = z;
    f32 vx = velocity->x * 20.0f, vz = velocity->z * 20.0f;
    velocity->x = vx;
    velocity->y = 50.0f;
    velocity->z = vz;
    speed.x = vx;
    speed.y = 50.0f;
    speed.z = vz;
}

/* 02350A14 */
static s32 daSampleCreate(daObjHat_c* p) { return p->_create(); }

/* 02350A48 */
static BOOL daSampleDelete(daObjHat_c* p) { return p->_delete(); }

/* 02350BEC */
static BOOL daSampleExecute(daObjHat_c* p) { return p->_execute(); }

/* 02350C48 */
static BOOL daSampleDraw(daObjHat_c* p) { return p->_draw(); }

/* 02350CF4 */
static BOOL daSampleIsDelete(daObjHat_c* p) { return TRUE; }

// HD: compiler-generated and template code of this translation unit.
/* 02350C4C */
static void __sinit_d_a_obj_hat_cpp() { sinit_header_statics(0x10469DB4, 0x101C9D1C); }

/* 02350CE0 */
static void trivial_dt(void* p, s32 flags) {
    if (p && (flags & 1))
        operator_delete(p);
}

/* 02350D00 */
static void destroy(daObjHat_c* p, s32 flags) {
    if (p != nullptr) {
        dCcD_Cyl_dt(&p->mCyl, 2);
        dCcD_Stts_dt(&p->mStts, 2);
        __dt(&p->mAcchCir);
        __dt(&p->mAcch);
        __dt(static_cast<fopAc_ac_c*>(p));
        if (flags & 1)
            operator_delete(p);
    }
}

/* 02350D9C */
static void emptyVirtual(void* p) {}

/* WWHD glue (tools/verify/autoglue.py)
str 10029F3C
str 10029F58 "Ro" in 0235096C
str 10029F5B "Ro" in 02350A18
data l_bmd_ix_tbl 10029F04
data l_bck_ix_tbl 10029F14
data l_cyl_src 101C9CD8
data moveProc 101C9CB0
ext 023505B4 daObjHat_c* daObjHat_construct(daObjHat_c*)
ext 025D54C4 BOOL fopAcM_SearchByID(fpc_ProcID, gptr<fopAc_ac_c>*)
ext 025D6800 void fopAcM_posMove(fopAc_ac_c*, dCcD_Stts*)
ext 0200ECD4 void cLib_addCalc(be<f32>*, f32, f32, f32, f32)
*/
