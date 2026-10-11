/**
 * d_a_swpropeller.cpp (WWHD)
 * Object - Propeller switch (spun by wind / struck by weapons).
 *
 * The GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_swpropeller.cpp) has only "Nonmatching" stubs: every member
 * function here is written from the WWHD code (cking.rpx) and verified against it, keeping the
 * GameCube names and header layout where they match.
 */
#include "d/actor/d_a_swpropeller.h"
const gc_tu_data gc_tu = {
    .safeString = 0x1003EC2C,
    .objAcch = {0x1003EC54, 0x1003EC74, 0x1003EC64},
    .objAcchDt = {0x1003EC64, 0x1003EC74},
    .headerStatics = {0x1046E19C, 0x101D1510, 0x1046E1A8},
};

#define SWPROP_VTBL 0x1003EC84 /* HD: daSwProp_c vtable (only the deleting destructor) */
#define AAB_VTBL 0x1003EC44    /* this TU's cM3dGAab vtable */
static dCcD_SrcCyl l_cyl_src;

// HD: native table entries for type 0 (Hpbot1) and type 1 (Vpbot_00).
static const char* l_arcname[] = {"Hpbot1", "Vpbot_00"};
static const s16 l_bdlidx[] = {3, 3};
static const u32 l_heapsize[] = {0x880, 0x8C0};

enum {
    JA_SE_OBJ_PROPELLER_SPIN = 0x6165, /* wind hit */
    JA_SE_OBJ_PROPELLER_ROT = 0x61B1,  /* level sound while turning (type 1) */
};
/* At types */
enum : u32 {
    AT_TYPE_WIND = 0x00200000,
    AT_TYPE_PROP_HIT = 0xFF1DFEFF,    /* every type but the wind/boomerang/fan ones: turns the propeller */
    AT_TYPE_PROP_HIT_SE = 0x10010402, /* the hits that play a hit sound */
};

/* float -> u32 (GHS: fctiwz with a 2^31 bias for large values) */
static inline u32 ftou(f32 v) {
    if (v >= 2147483648.0f)
        return (u32)gabi::ftoi(v - 2147483648.0f) + 0x80000000u;
    return (u32)gabi::ftoi(v);
}

/* 024A3144 */
bool daSwProp_c::_delete() {
    dComIfG_resDelete(&mPhs, l_arcname[mType]);
    return true;
}

/* 024A2C00 */
static BOOL CheckCreateHeap(fopAc_ac_c *i_this) {
    return static_cast<daSwProp_c *>(i_this)->CreateHeap();
}

/* 024A2B2C */
BOOL daSwProp_c::CreateHeap() {
    J3DModelData *modelData = (J3DModelData *)dComIfG_getObjectRes(l_arcname[mType], l_bdlidx[mType]);
    if (modelData == nullptr) /* JUT_ASSERT(0x101, modelData != NULL) */
        JUT_ASSERT_fail("d_a_swpropeller.cpp", 0x101, "modelData != (0)");
    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    if (mpModel == nullptr)
        return FALSE;
    /* mpModel->setUserArea((u32)this) */
    mpModel->setUserArea((uintptr_t)this);
    return TRUE;
}

/* 024A2C04 */
static BOOL nodeCallBack(J3DNode *node, int calcTiming) {
    if (calcTiming == 0) {
        J3DJoint *joint = J3DNode_toJoint(node);
        J3DModel *model = j3dSys.getModel();
        daSwProp_c *i_this = (daSwProp_c *)model->getUserArea();
        u16 jntNo = ((SwPropJointDataHD *)joint)->mJntNo; /* joint->getJntNo() */
        if (i_this != nullptr) {
            i_this->mRotY = (s16)(i_this->mRotY + i_this->mRotYVel);
            PSMTXCopy(model->getAnmMtx(jntNo), mDoMtx_stack_c::get());
            mDoMtx_stack_c::YrotM(i_this->mRotY);
            model->setAnmMtx(jntNo, mDoMtx_stack_c::get()); /* model->setAnmMtx */
            PSMTXCopy(mDoMtx_stack_c::get(), J3DSys::mCurrentMtx);
        }
    }
    return TRUE;
}

/* 024A2D38 */
void daSwProp_c::set_mtx() {
    mpModel->setBaseScale(scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
}

/* 024A2E10 */
void daSwProp_c::CreateInit() {
    fopAcM_SetMtx(this, mpModel->getBaseTRMtx()); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -150.0f, -100.0f, -150.0f, 150.0f, 150.0f, 150.0f);
    cullSizeFar = 1.0f; /* fopAcM_setCullSizeFar */

    mAcchCir.SetWall(30.0f, 30.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed);
    mAcch.SetWallNone();
    mAcch.SetRoofNone();
    mAcch.SetWaterNone();

    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(&l_cyl_src);
    mCyl.SetStts(&mStts);

    m648 = fopAcM_GetParam(this) & 0xFF;
    set_mtx();

    /* mpModel->getModelData()->getJointName()->getIndex("kaiten") */
    s32 jnt = mpModel->getModelData()->getJointName()->getIndex("kaiten");
    if (jnt >= 0) {
        mpModel->getModelData()->getJointNodePointer((u16)jnt)->setCallBack(nodeCallBack);
    }
    J3DModel_calc(mpModel);
}

/* 024A2F84 */
cPhs_State daSwProp_c::_create() {
    /* fopAcM_ct(this, daSwProp_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = SWPROP_VTBL;
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, AAB_VTBL);
            __ct(&mAcch);
            __ct(&mAcchCir);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    mType = (fopAcM_GetParam(this) >> 8) & 0xF;
    if (!(mType < 2)) /* JUT_ASSERT(0x17E, mType < 2) */
        JUT_ASSERT_fail("d_a_swpropeller.cpp", 0x17E, "mType >= 0 && mType < 2");

    cPhs_State ret = dComIfG_resLoad(&mPhs, l_arcname[mType]);
    if (ret == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, l_heapsize[mType]))
            return cPhs_ERROR_e;
        CreateInit();
    }
    return ret;
}

/* 024A31E4 */
bool daSwProp_c::_execute() {
    bool windHit = false;
    mAcch.CrrPos(dComIfG_Bgsp());

    if (mCyl.ChkTgHit()) {
        dCcD_GObjInf *obj = (dCcD_GObjInf *)mCyl.GetTgHitObj();
        if (obj != nullptr) {
            if (obj->ChkAtType(AT_TYPE_WIND)) {
                windHit = true;
                mSpinDown = 0;
                fopAcM_seStart(this, JA_SE_OBJ_PROPELLER_SPIN, 0);
                mRotYVel = 0x1000;
                mRotYVelTarget = 0;
            } else if (obj->ChkAtType(AT_TYPE_PROP_HIT)) {
                mRotYVel = 0x300;
                mRotYVelTarget = -0x159;
                mSpinDown = 1;
                if (obj->ChkAtType(AT_TYPE_PROP_HIT_SE)) {
                    if (mType == 1) {
                        daObj_HitSeStart(&current.pos, fopAcM_GetRoomNo(this), &mCyl, 0xB);
                    } else if (mType == 0) {
                        daObj_HitSeStart(&current.pos, fopAcM_GetRoomNo(this), &mCyl, 0x11);
                    }
                }
            }
        }
    }

    /* the wind stopped: toggle the switch */
    if (!windHit && mWindHit != 0) {
        dComIfGs_revSwitch(m648, home.roomNo);
    }

    s16 rest = cLib_addCalcAngleS(&mRotYVel, mRotYVelTarget, mSpinDown ? 10 : 30, 100, 10);
    if (mSpinDown != 0 && rest == 0) {
        s16 v = (s16)gabi::ftoi(-0.6f * (f32)mRotYVelTarget);
        if ((v < 0 ? -(s32)v : (s32)v) >= 0x20) {
            mRotYVelTarget = v;
        } else {
            mSpinDown = 0;
            mRotYVelTarget = 0;
        }
    }

    if (mType == 1) {
        mDoAud_seStart3(JA_SE_OBJ_PROPELLER_ROT, &current.pos,
                        ftou((f32)mRotYVel * (1.0f / 4096.0f) * 100.0f));
    }

    mWindHit = windHit;
    set_mtx();

    gabi::Local<cXyz> c;
    c->x = current.pos.x;
    c->y = current.pos.y + 50.0f;
    c->z = current.pos.z;
    mCyl.SetC(c.get());
    dComIfG_Ccsp_Set(&mCyl);
    return true;
}

/* 024A3184 */
bool daSwProp_c::_draw() {
    g_env_light.settingTevStruct(TEV_TYPE_ACTOR, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    mDoExt_modelUpdateDL(mpModel);
    return true;
}

/* method table entries (HD: tail branches) */
/* 024A3140 */
static cPhs_State daSwProp_Create(void *i_this) {
    return ((daSwProp_c *)i_this)->_create();
}

/* 024A3180 */
static BOOL daSwProp_Delete(void *i_this) {
    return ((daSwProp_c *)i_this)->_delete();
}

/* 024A31E0 */
static BOOL daSwProp_Draw(void *i_this) {
    return ((daSwProp_c *)i_this)->_draw();
}

/* 024A3520 */
static BOOL daSwProp_Execute(void *i_this) {
    return ((daSwProp_c *)i_this)->_execute();
}

/* 024A35CC */
static BOOL daSwProp_IsDelete(void *) {
    return TRUE;
}

// HD: compiler-generated header objects and deleting destructors.
/* 024A3524 */
static void __sinit_d_a_swpropeller_cpp() {
    __sinit_header_statics();
}

/* 024A35B8 */
static void SafeString_dt(void *p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 024A35D4 */
static void daSwProp_c_dt(daSwProp_c *i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(&i_this->mAcchCir);
        __dt(&i_this->mAcch);
        __dt(&i_this->mCyl);
        __dt(&i_this->mStts);
        __dt(i_this);
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 024A3670 */
static void SafeString_assureTerminationImpl(void *) {}

/* WWHD glue
data l_cyl_src 101D14CC
data l_arcname 101D1534
data l_bdlidx 1003ED40
data l_heapsize 1003ED38
str 1003ECA0 in 024A2B2C # "d_a_swpropeller.cpp"
str 1003ECFC in 024A2F84 # "d_a_swpropeller.cpp"
str 1003ECB4 "modelData != (0)"
str 1003ECE4 "mType >= 0 && mType < 2"
str 1003ECDC "kaiten"
ext 023129C4 void daObj_HitSeStart(const cXyz*, s32, const dCcD_GObjInf*, u32)
ext 025E1A04 void mDoAud_seStart3(u32, cXyz*, u32)
*/
