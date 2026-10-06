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
#include "bindings.h"

#define m_arcname STR(0x1000B4F0)     /* "Kwood_00" */
#define SAFESTRING_VTBL 0x1000B464    /* this TU's sead::SafeString vtable */
#define BRANCH_VTBL 0x1000B47C        /* daBranch_c vtable (HD virtual destructor) */
#define anim_table 0x101926E0         /* u16[6] */
#define solidHeapCB 0x020DF530        /* daBranch_c::solidHeapCB (tail branch to CreateHeap) */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 026067F4 dRes_control_c::getIDRes(const sead::SafeString& arc, u16 id) */
static inline void* dComIfG_getObjectIDRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::ea(arc);
    key->__vtbl = SAFESTRING_VTBL;
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), id);
}
/* 025F19F8 mDoMtx_XYZrotM(Mtx, s16, s16, s16) */
static inline void mDoMtx_XYZrotM(Mtx34* m, s16 x, s16 y, s16 z) { gabi::call(0x025F19F8, m, x, y, z); }
/* 02527028 dDemo_setDemoData(actor, flags, morf, arc, n, ids, HD p6, HD p7) */
static inline BOOL dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* m, const char* arc, s32 n, u32 ids, u32 p6, s8 p7) {
    return gabi::call<BOOL>(0x02527028, a, flags, m, arc, n, ids, p6, p7);
}
/* dComIfGp_demo_getActor(id) (HD inline): ids above 0x20 give NULL; the demo object at *0x101D5FFC
 * (asserted), 02526E70 dDemo_object_c::getActor(u8) */
static inline void* dComIfGp_demo_getActor(u8 id) {
    if (id > 0x20)
        return nullptr;
    u32 obj = gabi::load<u32>(0x101D5FFC);
    if (obj == 0) { /* JUT_ASSERT(0x23a, ...) */
        JUT_ASSERT_fail(STR(0x1000B49C), 0x23A, STR(0x1000B48C));
        obj = gabi::load<u32>(0x101D5FFC);
    }
    return gabi::call<void*>(0x02526E70, obj, id);
}

struct daBranch_c : fopAc_ac_c {
    cPhs_State create();
    BOOL draw();
    BOOL execute();
    void set_mtx();
    void set_anim(int, int, int);
    u32 demoPlay(mDoExt_McaMorf*);
    BOOL CreateHeap();
    Mtx34* getJointMtx(const char*);

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mModel[2];
    /* 0x3BC */ u8 dummy2[0x08];
    /* 0x3C4 */ gptr<mDoExt_McaMorf> mAnims[2];
    /* 0x3CC */ u8 dummy[0x08];
    /* 0x3D4 */ be<u32> m02B8;
    /* 0x3D8 */ be<u8> m02BC;
    /* 0x3D9 */ be<u8> m02BD;
};
WWHD_OFFSET(daBranch_c, mModel, 0x3B4);
WWHD_OFFSET(daBranch_c, mAnims, 0x3C4);
WWHD_OFFSET(daBranch_c, m02B8, 0x3D4);

/* 020DF054 */
void daBranch_c::set_mtx() {
    WWHD_FUNC(0x020DF054, void, this);
    J3DModel* pMdl;
    for (int i = 0; i < 2; i++) {
        pMdl = mModel[i];
        if (pMdl) {
            J3DModel_setBaseScale(pMdl, &scale);
            mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
            mDoMtx_XYZrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
            J3DModel_setBaseTRMtx(pMdl, mDoMtx_stack_c::get());
        }
    }
}
VERIFY(0x020DF054, &daBranch_c::set_mtx);

/* 020DF290 */
void daBranch_c::set_anim(int i_animIdx, int i_bckId, int i_basId) {
    WWHD_FUNC(0x020DF290, void, this, i_animIdx, i_bckId, i_basId);
    if (i_bckId > 0 && i_basId > 0) {
        J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectIDRes(m_arcname, i_bckId);
        void* bas = dComIfG_getObjectIDRes(m_arcname, i_basId);
        mDoExt_McaMorf* morf = mAnims[i_animIdx];
        morf->setAnm(bck, -1, 0.0f, 1.0f, 0.0f, -1.0f, bas);
    }
}
VERIFY(0x020DF290, &daBranch_c::set_anim);

/* 020DF02C */
u32 daBranch_c::demoPlay(mDoExt_McaMorf* morf) {
    WWHD_FUNC(0x020DF02C, u32, this, morf);
    return dDemo_setDemoData(this, 0x20 /* dDemo_actor_c::ENABLE_ANM_e */, morf, m_arcname, 3, anim_table, 0, 0);
}
VERIFY(0x020DF02C, &daBranch_c::demoPlay);

/* 020DF530: daBranch_c::solidHeapCB (HD: tail branch) */
static BOOL daBranch_solidHeapCB(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x020DF530, BOOL, i_this);
    return static_cast<daBranch_c*>(i_this)->CreateHeap();
}
VERIFY(0x020DF530, daBranch_solidHeapCB);

/* 020DF34C HD: resource ids {6, 5} / {7, 2} / {8, 0}; asserts modelData (line 0x1CE) */
BOOL daBranch_c::CreateHeap() {
    WWHD_FUNC(0x020DF34C, BOOL, this);
    static const int bmd[] = {6, 5};
    static const int bck[] = {7, 2};
    static const int bas[] = {8, 0};
    BOOL status = TRUE;
    for (int i = 0; i < 2; i++) {
        J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectIDRes(m_arcname, bmd[i]);
        dComIfG_getObjectIDRes(m_arcname, bck[i]); /* bckData (unused) */
        if (modelData == nullptr) /* JUT_ASSERT(0x1CE, modelData != NULL) */
            JUT_ASSERT_fail(STR(0x1000B4B4), 0x1CE, STR(0x1000B4C4));
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
        gabi::store<f32>(gabi::ea((mDoExt_McaMorf*)mAnims[i]) + 0x9C, 0.0f); /* mAnims[i]->setFrame(0.0f) */
        set_anim(i, bck[i], bas[i]);
    }
    return status;
}
VERIFY(0x020DF34C, &daBranch_c::CreateHeap);

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
    WWHD_FUNC(0x020DEFB0, BOOL, i_this);
    return i_this->draw();
}
VERIFY(0x020DEFB0, daBranch_Draw);

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
        void* demoActor = dComIfGp_demo_getActor(demoId);
        if (demoActor) {
            u32 shape = gabi::load<u32>(gabi::ea(demoActor) + 0x28); /* getShapeId() */
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
    WWHD_FUNC(0x020DF140, BOOL, i_this);
    return i_this->execute();
}
VERIFY(0x020DF140, daBranch_Execute);

/* 020DF280 */
static BOOL daBranch_IsDelete(daBranch_c*) {
    WWHD_FUNC(0x020DF280, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x020DF280, daBranch_IsDelete);

/* 020DF288 HD: the destructor is the virtual destructor (020DF714), not called here */
static BOOL daBranch_Delete(daBranch_c*) {
    WWHD_FUNC(0x020DF288, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x020DF288, daBranch_Delete);

/* 020DF714: daBranch_c deleting destructor (~daBranch_c) */
static void daBranch_c_dt(daBranch_c* i_this, s32 flags) {
    WWHD_FUNC(0x020DF714, void, i_this, flags);
    if (i_this != nullptr) {
        i_this->__vtbl = BRANCH_VTBL;
        for (int i = 0; i < 2; i++) {
            mDoExt_McaMorf* anim = i_this->mAnims[i];
            if (anim != nullptr) {
                anim->stopZelAnime();
            }
        }
        dComIfG_resDelete(&i_this->mPhase, m_arcname); /* dComIfG_resDeleteDemo */
        gabi::call(0x025D50BC, i_this, 0);              /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x020DF714, daBranch_c_dt);

/* create() inlined. HD: no fog-type loop over the materials */
cPhs_State daBranch_c::create() {
    /* fopAcM_ct(this, daBranch_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = BRANCH_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State phase_state = dComIfG_resLoad(&mPhase, m_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, solidHeapCB, 0x4000)) {
            for (int i = 0; i < 2; i++) {
                mAnims[i] = nullptr;
            }
            phase_state = cPhs_ERROR_e;
        } else {
            cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mModel[0])); /* fopAcM_SetMtx */
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
    WWHD_FUNC(0x020DF534, cPhs_State, i_this);
    return static_cast<daBranch_c*>(i_this)->create();
}
VERIFY(0x020DF534, daBranch_Create);

/* 020DF66C */
static void __sinit_d_a_branch_cpp() {
    WWHD_FUNC(0x020DF66C, void, (u32)0);
    sinit_header_statics(0x1046290C, 0x101926EC);
}
VERIFY(0x020DF66C, __sinit_d_a_branch_cpp);

/* 020DF700: sead::SafeString deleting destructor (this TU's copy; trivial) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x020DF700, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x020DF700, SafeString_dt);

/* 020DF7A4: sead::SafeString::assureTermination (this TU's copy; empty) */
static void SafeString_assureTermination(void*) {
    WWHD_FUNC(0x020DF7A4, void, (u32)0);
}
VERIFY(0x020DF7A4, SafeString_assureTermination);

/* ---- d_a_branch_static.cpp (HD translation unit, no GameCube source) ---- */

/* 020DF7A8 daBranch_c::getJointMtx(const char* name): the joint's matrix in mModel[0]'s
 * matrix buffer, NULL when the name is not found. 027F68FC (on model+0xAC) returns the joint
 * name table holder (offset-relative JUTNameTab at +0x10), 027DF9B0 JUTNameTab::getIndex.
 * Marks the buffer (+0x2C) as used (flag 0x10 at +4). */
Mtx34* daBranch_c::getJointMtx(const char* name) {
    WWHD_FUNC(0x020DF7A8, Mtx34*, this, name);
    u32 model = gabi::ea((J3DModel*)mModel[0]);
    if (model == 0) { /* JUT_ASSERT(0x16, mModel[0] != 0) */
        JUT_ASSERT_fail(STR(0x1000B500), 0x16, STR(0x1000B518));
        model = gabi::ea((J3DModel*)mModel[0]);
    }
    u32 holder = gabi::call<u32>(0x027F68FC, gabi::load<u32>(model + 0xAC));
    s32 off = gabi::load<s32>(holder + 0x10);
    u32 nameTab = 0;
    if (off != 0)
        nameTab = holder + 0x10 + off;
    s32 idx = gabi::call<s32>(0x027DF9B0, nameTab, name);
    if (idx < 0)
        return nullptr;
    u32 buf = gabi::load<u32>(gabi::ea((J3DModel*)mModel[0]) + 0x2C);
    gabi::store<u16>(buf + 4, (u16)(gabi::load<u16>(buf + 4) | 0x10));
    return gabi::at<Mtx34>(idx * 0x30 + gabi::load<u32>(buf + 0x10));
}
VERIFY(0x020DF7A8, &daBranch_c::getJointMtx);

/* 020DF874 */
static void __sinit_d_a_branch_static_cpp() {
    WWHD_FUNC(0x020DF874, void, (u32)0);
    sinit_header_statics(0x10462928, 0x10192740);
}
VERIFY(0x020DF874, __sinit_d_a_branch_static_cpp);
