/**
 * d_a_obj_dmgroom.cpp (WWHD)
 * Object - Damaged room (demo object with a TEV register animation).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_dmgroom.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define SAFESTRING_VTBL 0x1002723C
#define DMGROOM_VTBL 0x10027254 /* HD: daObjDmgroom_c vtable */
#define FILE_NAME STR(0x1002728C)

enum {
    dRes_INDEX_DMGROOM_BDL_DMGROOM_e = 4,
    dRes_INDEX_DMGROOM_BRK_DMGROOM_e = 7,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025E80D0 mDoExt_brkAnm::mDoExt_brkAnm (HD size 0x78; the matcher calls it init) */
static inline void mDoExt_brkAnm_ct(void* p) { gabi::call<u32>(0x025E80D0, p); }
/* 025E8154 mDoExt_brkAnm::init(data, brk, anmPlay, attr, rate, start, end, modify, entry) (unnamed) */
static inline BOOL mDoExt_brkAnm_init(void* a, J3DModelData* d, void* brk, s32 anmPlay, s32 attr, f32 rate, s16 start, s16 end,
                                      s32 modify, s32 entry) {
    return gabi::call<BOOL>(0x025E8154, a, d, brk, anmPlay, attr, rate, start, end, modify, entry);
}
/* 02526E70 dDemo_object_c::getActor(u8); the demo object pointer is at 101D5FFC */
static inline void* dDemo_object_getActor(u32 obj, u8 id) { return gabi::call<void*>(0x02526E70, obj, id); }

struct daObjDmgroom_c : fopAc_ac_c {
    cPhs_State _create();
    bool _execute();
    bool _draw();
    bool _delete();
    BOOL CreateHeap();
    void CreateInit();
    void set_mtx();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ u8 mBrkAnm[4]; /* mDoExt_brkAnm (HD 0x78): J3DFrameCtrl first, frame at +4 */
    /* 0x3BC */ be<f32> mBrkFrame;
    /* 0x3C0 */ u8 _3C0[0x430 - 0x3C0];
};
WWHD_OFFSET(daObjDmgroom_c, mBrkFrame, 0x3BC);

static inline J3DModelData* model_getModelData(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }

/* 02334608 */
BOOL daObjDmgroom_c::CreateHeap() {
    WWHD_FUNC(0x02334608, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x10027284) /* "dmgroom" */,
                                                                  dRes_INDEX_DMGROOM_BDL_DMGROOM_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(82, modelData != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x52, STR(0x100272A0));
    J3DModel* model = mDoExt_J3DModel__create(modelData, 0x00, 0x11020203);
    mpModel = model;
    if (model == nullptr) /* HD: tests the returned pointer */
        return FALSE;

    void* brk = dComIfG_getObjectRes(STR(0x10027284), dRes_INDEX_DMGROOM_BRK_DMGROOM_e, SAFESTRING_VTBL);
    if (brk == nullptr) /* JUT_ASSERT(92, brk != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x5C, STR(0x100272B4));
    if (!mDoExt_brkAnm_init(mBrkAnm, modelData, brk, true, 0 /* EMode_NONE */, 1.0f, 0, -1, false, 0))
        return FALSE;
    return TRUE;
}
VERIFY(0x02334608, &daObjDmgroom_c::CreateHeap);

/* 02334724 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02334724, BOOL, i_this);
    return ((daObjDmgroom_c*)i_this)->CreateHeap();
}
VERIFY(0x02334724, CheckCreateHeap);

/* 02334800 */
void daObjDmgroom_c::CreateInit() {
    WWHD_FUNC(0x02334800, void, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -600.0f, -0.0f, -600.0f, 600.0f, 900.0f, 600.0f);
    cullSizeFar = 1.0f;
    set_mtx();
}
VERIFY(0x02334800, &daObjDmgroom_c::CreateInit);

/* 02334728 */
void daObjDmgroom_c::set_mtx() {
    WWHD_FUNC(0x02334728, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
VERIFY(0x02334728, &daObjDmgroom_c::set_mtx);

/* 02334880: daObjDmgroom_Create (_create inlined) */
static cPhs_State daObjDmgroom_Create(void* p) {
    WWHD_FUNC(0x02334880, cPhs_State, p);
    daObjDmgroom_c* i_this = (daObjDmgroom_c*)p;
    /* fopAcM_ct(this, daObjDmgroom_c) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = DMGROOM_VTBL;
            mDoExt_brkAnm_ct(i_this->mBrkAnm);
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    cPhs_State ret = dComIfG_resLoad(&i_this->mPhs, STR(0x10027230));
    if (ret == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(i_this, 0x02334724 /* CheckCreateHeap */, 0x1460) == 0) {
            return cPhs_ERROR_e;
        }
        i_this->CreateInit();
    }
    return ret;
}
VERIFY(0x02334880, daObjDmgroom_Create);

/* 0233494C */
static BOOL daObjDmgroom_Delete(void* p) {
    WWHD_FUNC(0x0233494C, BOOL, p);
    dComIfG_resDelete(&((daObjDmgroom_c*)p)->mPhs, STR(0x100272D0)); /* dComIfG_resDeleteDemo */
    return TRUE;
}
VERIFY(0x0233494C, daObjDmgroom_Delete);

/* 0233497C */
static BOOL daObjDmgroom_Draw(void* p) {
    WWHD_FUNC(0x0233497C, BOOL, p);
    daObjDmgroom_c* i_this = (daObjDmgroom_c*)p;
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), i_this->mpModel, &i_this->tevStr);
    dComIfGd_setListBG();
    mDoExt_brkAnm_entry((mDoExt_brkAnm*)i_this->mBrkAnm, model_getModelData(i_this->mpModel), i_this->mBrkFrame);
    mDoExt_modelUpdateDL(i_this->mpModel);
    /* mBrkAnm.remove(modelData): HD inline, clears the model data's TEV register animator */
    gabi::store<u32>(gabi::ea(model_getModelData(i_this->mpModel)) + 0x48, 0);
    dComIfGd_setList();
    return TRUE;
}
VERIFY(0x0233497C, daObjDmgroom_Draw);

/* 02334A38 */
static BOOL daObjDmgroom_Execute(void* p) {
    WWHD_FUNC(0x02334A38, BOOL, p);
    daObjDmgroom_c* i_this = (daObjDmgroom_c*)p;
    u8 id = i_this->demoActorID;
    if (id != 0) {
        void* demoAc = nullptr;
        if (id <= 0x20) { /* dComIfGp_demo_getActor(id) */
            if (gabi::load<u32>(0x101D5FFC) == 0) /* JUT_ASSERT(0x23A, m_object != NULL) */
                JUT_ASSERT_fail(STR(0x10027274), 0x23A, STR(0x10027264));
            demoAc = dDemo_object_getActor(gabi::load<u32>(0x101D5FFC), id);
        }
        if (demoAc != nullptr) {
            if (gabi::load<u16>(gabi::ea(demoAc) + 4) & 0x40) /* checkEnable(ENABLE_ANM_FRAME_e) */
                i_this->mBrkFrame = gabi::load<f32>(gabi::ea(demoAc) + 0x30); /* mBrkAnm.setFrame(getAnmFrame()) */
        }
    }
    i_this->set_mtx();
    return TRUE;
}
VERIFY(0x02334A38, daObjDmgroom_Execute);

/* 02334B88 */
static BOOL daObjDmgroom_IsDelete(void* p) {
    WWHD_FUNC(0x02334B88, BOOL, p);
    return TRUE;
}
VERIFY(0x02334B88, daObjDmgroom_IsDelete);

/* 02334AE0 */
static void __sinit_d_a_obj_dmgroom_cpp() {
    WWHD_FUNC(0x02334AE0, void, (u32)0);
    sinit_header_statics(0x10469624, 0x101C8B54);
}
VERIFY(0x02334AE0, __sinit_d_a_obj_dmgroom_cpp);

/* 02334B74: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02334B74, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02334B74, trivial_dt);

/* 02334B90: daObjDmgroom_c deleting destructor (mDoExt_brkAnm has a trivial destructor) */
static void daObjDmgroom_c_dt(daObjDmgroom_c* i_this, s32 flags) {
    WWHD_FUNC(0x02334B90, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02334B90, daObjDmgroom_c_dt);

/* 02334BE4: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x02334BE4, void, p);
}
VERIFY(0x02334BE4, empty_virtual);
