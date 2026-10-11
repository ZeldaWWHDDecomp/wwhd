/**
 * d_a_branch.cpp (WWHD)
 * Branch (Forest Haven/Kwood: tree branch that swings and breaks in a demo).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_branch.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * Also contains the two functions of the HD translation unit d_a_branch_static.cpp
 * (daBranch_c::getJointMtx and its __sinit), which follow d_a_branch.cpp in the image.
 */
#include "d/actor/d_a_branch.h"

const gc_tu_data gc_tu = {.safeString = 0x1000B464};
const char daBranch_c::m_arcname[] = "Kwood_00";
// HD: resource IDs differ from the GameCube archive; checked against the image.
static u16 anim_table[] = { 7, 8, 3, 1, 2, 0 };
static BranchDemoObject* l_demoObject = {0};
enum {
    dRes_ID_KWOOD_00_BMD_WS_e = 6, dRes_ID_KWOOD_00_BMD_WB_e = 5,
    dRes_ID_KWOOD_00_BCK_SWING02_e = 7, dRes_ID_KWOOD_00_BCK_BREAK_e = 2,
    dRes_ID_KWOOD_00_BAS_SWING02_e = 8, dRes_ID_KWOOD_00_BAS_BREAK_e = 0
};

// HD: demo IDs above 0x20 are rejected; the object pointer is reread after an assert.
static BranchDemoActor* dComIfGp_demo_getActor(u8 id) {
    if (id > 0x20)
        return nullptr;
    BranchDemoObject* obj = l_demoObject;
    if (obj == nullptr) {
        JUT_ASSERT(0x23A, obj != nullptr);
        obj = l_demoObject;
    }
    return obj->getActor(id);
}

// HD: the base construction is inline and installs this TU's vtable.
daBranch_c::daBranch_c() {
    fopAc_ac_c_ct(this);
    __vtbl = 0x1000B47C;
}

/* 020DF054 */
void daBranch_c::set_mtx() {
    J3DModel* pMdl;
    for (int i = 0; i < 2; i++) {
        pMdl = mModel[i];
        if (pMdl) {
            pMdl->setBaseScale(scale);
            mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
            mDoMtx_XYZrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
            pMdl->setBaseTRMtx(mDoMtx_stack_c::get());
        }
    }
}

/* 020DF290 */
void daBranch_c::set_anim(int i_animIdx, int i_bckId, int i_basId) {
    if (i_bckId > 0 && i_basId > 0) {
        J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectIDRes(m_arcname, i_bckId);
        void* bas = dComIfG_getObjectIDRes(m_arcname, i_basId);
        mDoExt_McaMorf* morf = mAnims[i_animIdx];
        morf->setAnm(bck, -1, 0.0f, 1.0f, 0.0f, -1.0f, bas);
    }
}

/* 020DF02C */
u32 daBranch_c::demoPlay(mDoExt_McaMorf* morf) {
    return dDemo_setDemoData(this, BranchDemoActor::ENABLE_ANM_e, morf, m_arcname, 3, anim_table, 0, 0);
}

// HD: daBranch_c::solidHeapCB (HD: tail branch)
/* 020DF530 */
static BOOL daBranch_solidHeapCB(fopAc_ac_c* i_this) {
    return static_cast<daBranch_c*>(i_this)->CreateHeap();
}

// HD: HD: resource ids {6, 5} / {7, 2} / {8, 0}; asserts modelData (line 0x1CE)
/* 020DF34C */
BOOL daBranch_c::CreateHeap() {
    const int bmd[] = {dRes_ID_KWOOD_00_BMD_WS_e, dRes_ID_KWOOD_00_BMD_WB_e};
    const int bck[] = {dRes_ID_KWOOD_00_BCK_SWING02_e, dRes_ID_KWOOD_00_BCK_BREAK_e};
    const int bas[] = {dRes_ID_KWOOD_00_BAS_SWING02_e, dRes_ID_KWOOD_00_BAS_BREAK_e};
    BOOL status = TRUE;
    for (int i = 0; i < 2; i++) {
        J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectIDRes(m_arcname, bmd[i]);
        dComIfG_getObjectIDRes(m_arcname, bck[i]); /* bckData (unused) */
        JUT_ASSERT(0x1CE, modelData != nullptr);
        J3DModelData* md = (J3DModelData*)dComIfG_getObjectIDRes(m_arcname, bmd[i]);
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectIDRes(m_arcname, bck[i]);
        mDoExt_McaMorf* morf = mDoExt_McaMorf::create(nullptr, md, nullptr, nullptr, anm, -1, 1.0f, 0, -1, 1, nullptr, 0, 0x11020203);
        mAnims[i] = morf;
        if (!morf) {
            status = FALSE;
            break;
        }
        J3DModel* model = morf->getModel();
        mModel[i] = model;
        if (!model) {
            status = FALSE;
            break;
        }
        mAnims[i]->setFrame(0.0f);
        set_anim(i, bck[i], bas[i]);
    }
    return status;
}

/* draw() inlined */
BOOL daBranch_c::draw() {
    int activeIdx = 0;
    if (m02B8 == 5) {
        activeIdx = 1;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mModel[activeIdx], &tevStr);
    ((mDoExt_McaMorf*)mAnims[activeIdx])->updateDL();
    return TRUE;
}

/* 020DEFB0 */
static BOOL daBranch_Draw(daBranch_c* i_this) {
    return i_this->draw();
}

/* execute() inlined */
BOOL daBranch_c::execute() {
    u8 demoId = demoActorID;
    if (demoId == 0) {
        if (m02B8 == 5) {
            if (mAnims[1]) {
                ((mDoExt_McaMorf*)mAnims[1])->play(nullptr, 0, 0);
            }
        } else if (m02B8 == 6 && mAnims[0]) {
            ((mDoExt_McaMorf*)mAnims[0])->play(nullptr, 0, 0);
        }
    } else {
        BranchDemoActor* demoActor = dComIfGp_demo_getActor(demoId);
        if (demoActor) {
            u32 shape = demoActor->getShapeId();
            m02B8 = shape;
            if (shape == 6) {
                demoPlay(mAnims[0]);
            } else if (shape == 5) {
                demoPlay(mAnims[1]);
            }
        }
    }
    set_mtx();
    return TRUE;
}

/* 020DF140 */
static BOOL daBranch_Execute(daBranch_c* i_this) {
    return i_this->execute();
}

/* 020DF280 */
static BOOL daBranch_IsDelete(daBranch_c*) {
    return TRUE;
}

// HD: HD: the destructor is the virtual destructor (020DF714), not called here
/* 020DF288 */
static BOOL daBranch_Delete(daBranch_c*) {
    return TRUE;
}

// HD: daBranch_c deleting destructor (~daBranch_c)
/* 020DF714 */
static void daBranch_c_dt(daBranch_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        i_this->__vtbl = 0x1000B47C;
        for (int i = 0; i < 2; i++) {
            mDoExt_McaMorf* anim = i_this->mAnims[i];
            if (anim != nullptr) {
                anim->stopZelAnime();
            }
        }
        dComIfG_resDelete(&i_this->mPhase, daBranch_c::m_arcname); /* dComIfG_resDeleteDemo */
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* create() inlined. HD: no fog-type loop over the materials */
cPhs_State daBranch_c::create() {
    fopAcM_ct(this, daBranch_c);
    cPhs_State phase_state = dComIfG_resLoad(&mPhase, m_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, daBranch_solidHeapCB, 0x4000)) {
            for (int i = 0; i < 2; i++) {
                mAnims[i] = nullptr;
            }
            phase_state = cPhs_ERROR_e;
        } else {
            fopAcM_SetMtx(this, mModel[0]->getBaseTRMtx());
            fopAcM_setCullSizeBox(this, 0.0f, 0.0f, -50.0f, 300.0f, 100.0f, 50.0f);
            m02B8 = 6;
            m02BC = 0;
            m02BD = 0;
            set_mtx();
        }
    }
    return phase_state;
}

/* 020DF534 */
static cPhs_State daBranch_Create(fopAc_ac_c* i_this) {
    return static_cast<daBranch_c*>(i_this)->create();
}

/* 020DF66C */
static void __sinit_d_a_branch_cpp() {
    sinit_header_statics(0x1046290C, 0x101926EC);
}

// HD: sead::SafeString deleting destructor (this TU's copy; trivial)
/* 020DF700 */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

// HD: sead::SafeString::assureTermination (this TU's copy; empty)
/* 020DF7A4 */
static void SafeString_assureTermination(void*) {
}

/* ---- d_a_branch_static.cpp (HD translation unit, no GameCube source) ---- */

// HD: separate translation unit; shared joint accessors mark the matrix buffer dirty.
/* 020DF7A8 */
Mtx34* daBranch_c::getJointMtx(const char* name) {
    J3DModel* model = mModel[0];
    JUT_ASSERT(0x16, model != nullptr);
    model = mModel[0]; // HD: reread after the assertion.
    s32 idx = model->getModelData()->getJointName()->getIndex(name);
    if (idx < 0)
        return nullptr;
    return mModel[0]->getAnmMtx(idx);
}

/* 020DF874 */
static void __sinit_d_a_branch_static_cpp() {
    sinit_header_statics(0x10462928, 0x10192740);
}

/* WWHD glue (tools/verify/autoglue.py)
str 1000B4F0
str 1000B49C "d_a_branch.cpp" renamed in 020DF140 # HD file d_demo.h
str 1000B48C "obj != nullptr" renamed # original HD spelling names the demo object
str 1000B4B4 in 020DF34C
str 1000B4C4 "modelData != nullptr" renamed
str 1000B500 "d_a_branch.cpp" renamed in 020DF7A8 # HD file d_a_branch_static.cpp
str 1000B518 "model != nullptr" renamed
data anim_table 101926E0
data l_demoObject 101D5FFC
ext 025F19F8 void mDoMtx_XYZrotM(Mtx34*, s16, s16, s16)
ext 02527028 BOOL dDemo_setDemoData(fopAc_ac_c*, u8, mDoExt_McaMorf*, const char*, s32, be<u16>*, u32, s8)
ext 02526E70 BranchDemoActor* BranchDemoObject::getActor(u8)
*/
