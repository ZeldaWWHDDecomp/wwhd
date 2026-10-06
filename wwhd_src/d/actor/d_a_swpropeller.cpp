/**
 * d_a_swpropeller.cpp (WWHD)
 * Object - Propeller switch (spun by wind / struck by weapons).
 *
 * The GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_swpropeller.cpp) has only "Nonmatching" stubs: every member
 * function here is written from the WWHD code (cking.rpx) and verified against it, keeping the
 * GameCube names and header layout where they match.
 */
#include "bindings.h"

#define SAFESTRING_VTBL 0x1003EC2C
#define SWPROP_VTBL 0x1003EC84 /* HD: daSwProp_c vtable (only the deleting destructor) */
#define AAB_VTBL 0x1003EC44     /* this TU's cM3dGAab vtable */
#define l_cyl_src gabi::at<dCcD_SrcCyl>(0x101D14CC)

/* static tables indexed with mType (0..1) */
#define l_arcname(t) STR(gabi::load<u32>(0x101D1534 + 4 * (t))) /* .data */
#define l_bdlidx(t) gabi::load<s16>(0x1003ED40 + 2 * (t))
#define l_heapsize(t) gabi::load<u32>(0x1003ED38 + 4 * (t))

enum {
    JA_SE_OBJ_PROPELLER_SPIN = 0x6165, /* wind hit */
    JA_SE_OBJ_PROPELLER_ROT = 0x61B1,  /* level sound while turning (type 1) */
};
/* At types */
enum : u32 {
    AT_TYPE_WIND = 0x00200000,
    AT_TYPE_PROP_HIT = 0xFF1DFEFF,   /* every type but the wind/boomerang/fan ones: turns the propeller */
    AT_TYPE_PROP_HIT_SE = 0x10010402, /* the hits that play a hit sound */
};

struct daSwProp_c : fopAc_ac_c {
    bool _delete();
    BOOL CreateHeap();
    void CreateInit();
    cPhs_State _create();
    void set_mtx();
    bool _execute();
    bool _draw();

    /* 0x3AC */ request_of_phase_process_class mPhs; /* GameCube 0x290 (+0x11C) */
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ dCcD_Cyl mCyl;
    /* 0x524 */ u8 m524[0x558 - 0x524];
    /* 0x558 */ dBgS_ObjAcch mAcch;
    /* 0x71C */ dBgS_AcchCir mAcchCir;
    /* 0x75C */ be<s16> mRotY;
    /* 0x75E */ be<s16> mRotYVel;
    /* 0x760 */ be<s16> mRotYVelTarget;
    /* 0x762 */ u8 _762[2];
    /* 0x764 */ be<u32> mSwitchNo;      /* GameCube m648 */
    /* 0x768 */ be<u8> mWindHit;        /* wind hit in the previous frame */
    /* 0x769 */ be<u8> mType;
    /* 0x76A */ be<u8> mSpinDown;       /* turning after a weapon hit */
    /* 0x76B */ u8 _76B;
};
WWHD_OFFSET(daSwProp_c, mStts, 0x3B8);
WWHD_OFFSET(daSwProp_c, mCyl, 0x3F4);
WWHD_OFFSET(daSwProp_c, mAcch, 0x558);
WWHD_OFFSET(daSwProp_c, mAcchCir, 0x71C);
WWHD_OFFSET(daSwProp_c, mRotY, 0x75C);
WWHD_OFFSET(daSwProp_c, mType, 0x769);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 023129C4 daObj::HitSeStart(const cXyz* pos, s32 roomNo, const dCcD_GObjInf* cc, u32 seId) */
static inline void daObj_HitSeStart(const cXyz* pos, s32 roomNo, const dCcD_GObjInf* cc, u32 se) { gabi::call(0x023129C4, pos, roomNo, cc, se); }
/* 025BA20C dSv_info_c::revSwitch */
static inline void dComIfGs_revSwitch(s32 no, s32 roomNo) { gabi::call(0x025BA20C, dComIfGs_info(), no, roomNo); }
/* 025E1A04 mDoAud_seStart(id, pos, param) (HD: no reverb argument; also local in npc_ji1) */
static inline void mDoAud_seStart3(u32 id, cXyz* pos, u32 param) { gabi::call(0x025E1A04, id, pos, param); }
/* HD J3D (as in d_a_mo2 / d_a_kamome): j3dSys.mModel at 0x104B462C, J3DSys::mCurrentMtx at
 * 0x104B4868; the model's joint matrices in a block at +0x2C (+0x4 flags, +0x10 matrices) */
struct J3DMtxBlock_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
struct J3DModel_l {
    /* 0x00 */ u8 _00[0x2C];
    /* 0x2C */ gptr<J3DMtxBlock_l> mpMtxBlock;
    /* 0x30 */ u8 _30[0xB8 - 0x30];
    /* 0xB8 */ be<u32> mUserArea;
};
static inline J3DModel_l* j3dSys_getModel() { return gabi::at<J3DModel_l>(gabi::load<u32>(0x104B462C)); }
static inline Mtx34* J3DSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
/* getAnmMtx: HD marks the joint matrices dirty */
static inline Mtx34* model_getAnmMtx(J3DModel_l* model, u32 jntNo) {
    J3DMtxBlock_l* blk = model->mpMtxBlock;
    blk->mFlags |= 0x10;
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30);
}
/* J3DModelData::getJointName() (HD: 027F68FC returns the header; name table offset at +0x10) */
static inline u32 J3DModelData_getJointName_l(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    s32 off = gabi::load<s32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s32 JUTNameTab_getIndex_l(u32 tab, const char* name) { return gabi::call<s32>(0x027DF9B0, tab, name); }
/* J3DModelData::getJointNodePointer(i)->setCallBack(cb) (HD: 0x1C-byte joint records, count +4, array +8) */
static inline void J3DModelData_setJointCallBack_l(J3DModelData* d, u32 i, u32 cb) {
    u32 n = gabi::load<u32>(gabi::ea(d) + 4);
    u32 p = gabi::load<u32>(gabi::ea(d) + 8);
    if (i < n) {
        p += i * 0x1C;
    }
    gabi::store<u32>(p + 8, cb);
}
/* float -> u32 (GHS: fctiwz with a 2^31 bias for large values) */
static inline u32 ftou(f32 v) {
    if (v >= 2147483648.0f)
        return (u32)gabi::ftoi(v - 2147483648.0f) + 0x80000000u;
    return (u32)gabi::ftoi(v);
}

/* 024A3144 */
bool daSwProp_c::_delete() {
    WWHD_FUNC(0x024A3144, bool, this);
    dComIfG_resDelete(&mPhs, l_arcname(mType));
    return true;
}
VERIFY(0x024A3144, &daSwProp_c::_delete);

/* 024A2C00 (CheckCreateHeap: tail branch) */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x024A2C00, BOOL, i_this);
    return static_cast<daSwProp_c*>(i_this)->CreateHeap();
}
VERIFY(0x024A2C00, CheckCreateHeap);

/* 024A2B2C */
BOOL daSwProp_c::CreateHeap() {
    WWHD_FUNC(0x024A2B2C, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(l_arcname(mType), l_bdlidx(mType), SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0x101, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x1003ECA0), 0x101, STR(0x1003ECB4));
    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    if (mpModel == nullptr)
        return FALSE;
    /* mpModel->setUserArea((u32)this) */
    gabi::store<u32>(gabi::ea(mpModel.get()) + 0xB8, gabi::ea(this));
    return TRUE;
}
VERIFY(0x024A2B2C, &daSwProp_c::CreateHeap);

/* 024A2C04 (not named by the matcher) */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x024A2C04, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel_l* model = j3dSys_getModel();
        daSwProp_c* i_this = gabi::at<daSwProp_c>(model->mUserArea);
        u16 jntNo = gabi::load<u16>(gabi::ea(joint) + 4); /* joint->getJntNo() */
        if (i_this != nullptr) {
            i_this->mRotY = (s16)(i_this->mRotY + i_this->mRotYVel);
            PSMTXCopy(model_getAnmMtx(model, jntNo), mDoMtx_stack_c::get());
            mDoMtx_stack_c::YrotM(i_this->mRotY);
            mtx_copy(model_getAnmMtx(model, jntNo), mDoMtx_stack_c::get()); /* model->setAnmMtx */
            PSMTXCopy(mDoMtx_stack_c::get(), J3DSys_mCurrentMtx());
        }
    }
    return TRUE;
}
VERIFY(0x024A2C04, nodeCallBack);

/* 024A2D38 */
void daSwProp_c::set_mtx() {
    WWHD_FUNC(0x024A2D38, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
VERIFY(0x024A2D38, &daSwProp_c::set_mtx);

/* 024A2E10 */
void daSwProp_c::CreateInit() {
    WWHD_FUNC(0x024A2E10, void, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -150.0f, -100.0f, -150.0f, 150.0f, 150.0f, 150.0f);
    cullSizeFar = 1.0f; /* fopAcM_setCullSizeFar */

    mAcchCir.SetWall(30.0f, 30.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed);
    mAcch.SetWallNone();
    mAcch.SetRoofNone();
    mAcch.SetWaterNone();

    mStts.Init(0xFF, 0xFF, this);
    mCyl.Set(l_cyl_src);
    mCyl.SetStts(&mStts);

    mSwitchNo = fopAcM_GetParam(this) & 0xFF;
    set_mtx();

    /* mpModel->getModelData()->getJointName()->getIndex("kaiten") */
    s32 jnt = JUTNameTab_getIndex_l(J3DModelData_getJointName_l(J3DModel_getModelData(mpModel)), STR(0x1003ECDC));
    if (jnt >= 0) {
        J3DModelData_setJointCallBack_l(J3DModel_getModelData(mpModel), (u16)jnt, 0x024A2C04 /* nodeCallBack */);
    }
    J3DModel_calc(mpModel);
}
VERIFY(0x024A2E10, &daSwProp_c::CreateInit);

/* 024A2F84 */
cPhs_State daSwProp_c::_create() {
    WWHD_FUNC(0x024A2F84, cPhs_State, this);
    /* fopAcM_ct(this, daSwProp_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = SWPROP_VTBL;
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, AAB_VTBL);
            dBgS_ObjAcch_ct(&mAcch, dBgS_ObjAcch_vt{0x1003EC54, 0x1003EC74, 0x1003EC64});
            dBgS_AcchCir_ct(&mAcchCir);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    mType = (fopAcM_GetParam(this) >> 8) & 0xF;
    if (!(mType < 2)) /* JUT_ASSERT(0x17E, mType < 2) */
        JUT_ASSERT_fail(STR(0x1003ECFC), 0x17E, STR(0x1003ECE4));

    cPhs_State ret = dComIfG_resLoad(&mPhs, l_arcname(mType));
    if (ret == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, 0x024A2C00 /* CheckCreateHeap */, l_heapsize(mType)))
            return cPhs_ERROR_e;
        CreateInit();
    }
    return ret;
}
VERIFY(0x024A2F84, &daSwProp_c::_create);

/* 024A31E4 */
bool daSwProp_c::_execute() {
    WWHD_FUNC(0x024A31E4, bool, this);
    bool windHit = false;
    mAcch.CrrPos(dComIfG_Bgsp());

    if (mCyl.ChkTgHit()) {
        void* obj = mCyl.GetTgHitObj();
        if (obj != nullptr) {
            if (cCcD_Obj_ChkAtType(obj, AT_TYPE_WIND)) {
                windHit = true;
                mSpinDown = 0;
                fopAcM_seStart(this, JA_SE_OBJ_PROPELLER_SPIN, 0);
                mRotYVel = 0x1000;
                mRotYVelTarget = 0;
            } else if (cCcD_Obj_ChkAtType(obj, AT_TYPE_PROP_HIT)) {
                mRotYVel = 0x300;
                mRotYVelTarget = -0x159;
                mSpinDown = 1;
                if (cCcD_Obj_ChkAtType(obj, AT_TYPE_PROP_HIT_SE)) {
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
        dComIfGs_revSwitch(mSwitchNo, home.roomNo);
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
        mDoAud_seStart3(JA_SE_OBJ_PROPELLER_ROT, &current.pos, ftou((f32)mRotYVel * (1.0f / 4096.0f) * 100.0f));
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
VERIFY(0x024A31E4, &daSwProp_c::_execute);

/* 024A3184 (not named by the matcher) */
bool daSwProp_c::_draw() {
    WWHD_FUNC(0x024A3184, bool, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    mDoExt_modelUpdateDL(mpModel);
    return true;
}
VERIFY(0x024A3184, &daSwProp_c::_draw);

/* method table entries (HD: tail branches) */
/* 024A3140 */
static cPhs_State daSwProp_Create(void* i_this) {
    WWHD_FUNC(0x024A3140, cPhs_State, i_this);
    return ((daSwProp_c*)i_this)->_create();
}
VERIFY(0x024A3140, daSwProp_Create);

/* 024A3180 */
static BOOL daSwProp_Delete(void* i_this) {
    WWHD_FUNC(0x024A3180, bool, i_this);
    return ((daSwProp_c*)i_this)->_delete();
}
VERIFY(0x024A3180, daSwProp_Delete);

/* 024A31E0 */
static BOOL daSwProp_Draw(void* i_this) {
    WWHD_FUNC(0x024A31E0, bool, i_this);
    return ((daSwProp_c*)i_this)->_draw();
}
VERIFY(0x024A31E0, daSwProp_Draw);

/* 024A3520 */
static BOOL daSwProp_Execute(void* i_this) {
    WWHD_FUNC(0x024A3520, bool, i_this);
    return ((daSwProp_c*)i_this)->_execute();
}
VERIFY(0x024A3520, daSwProp_Execute);

/* 024A35CC */
static BOOL daSwProp_IsDelete(void*) {
    WWHD_FUNC(0x024A35CC, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x024A35CC, daSwProp_IsDelete);

/* ---- compiler-generated (no GameCube source) ---- */
/* 024A3524 */
static void __sinit_d_a_swpropeller_cpp() {
    WWHD_FUNC(0x024A3524, void, (u32)0);
    sinit_header_statics(0x1046E19C, 0x101D1510);
}
VERIFY(0x024A3524, __sinit_d_a_swpropeller_cpp);

/* 024A35B8: sead::SafeString deleting destructor (SafeString vtable +0xC) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x024A35B8, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x024A35B8, SafeString_dt);

/* 024A35D4: daSwProp_c deleting destructor (vtable +0xC); members destroyed in reverse order */
static void daSwProp_c_dt(daSwProp_c* i_this, s32 flags) {
    WWHD_FUNC(0x024A35D4, void, i_this, flags);
    if (i_this != nullptr) {
        /* ~dBgS_AcchCir: its cM3dGCir */
        gabi::call(0x02018034, gabi::at<u8>(gabi::ea(&i_this->mAcchCir) + 0x14), 2);
        /* ~dBgS_ObjAcch: restore this TU's vtables, then ~dBgS_Acch */
        gabi::store<u32>(gabi::ea(&i_this->mAcch) + 0x20, 0x1003EC64);
        gabi::store<u32>(gabi::ea(&i_this->mAcch) + 0x14, 0x1003EC74);
        gabi::call(0x024EFD9C, &i_this->mAcch, 0);
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x024A35D4, daSwProp_c_dt);

/* 024A3670: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x024A3670, void, (u32)0);
}
VERIFY(0x024A3670, SafeString_assureTerminationImpl);
