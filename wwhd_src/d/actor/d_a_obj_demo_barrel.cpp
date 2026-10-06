/**
 * d_a_obj_demo_barrel.cpp (WWHD)
 * Object - Demo barrel (the barrel Link escapes Forsaken Fortress in; breaks and splashes in the demo).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_demo_barrel.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define M_arcname STR(0x10027228) /* "DBarrel" (static member array) */
#define SAFESTRING_VTBL 0x100271BC
#define BARREL_VTBL 0x100271D4 /* HD: daObj_Demo_Barrel_c vtable */

enum {
    dRes_ID_DBARREL_BCK_02_TR_CD_e = 0,
    dRes_ID_DBARREL_BDL_KTARU_02_e = 5,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 026067F4 HD: dRes_control_c::getIDRes(const sead::SafeString& arc, s32 index) (as in d_a_npc_ob1.h) */
static inline void* dComIfG_getObjectIDRes(const char* arc, s32 index, u32 safestring_vtbl) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::ea(arc);
    key->__vtbl = safestring_vtbl;
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), index);
}
/* 02526E70 dDemo_object_c::getActor(u8); the demo object pointer is at 101D5FFC */
static inline void* dDemo_object_getActor(u32 obj, u8 id) { return gabi::call<void*>(0x02526E70, obj, id); }
/* 02527028 dDemo_setDemoData(actor, flags, morf, arc, n, ids, p6, p7) (as in d_a_npc_zl1.h) */
static inline BOOL dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, void* ids, u32 p6, s8 p7) {
    return gabi::call<BOOL>(0x02527028, a, flags, morf, arc, n, ids, p6, p7);
}
/* 025F19F8 mDoMtx_XYZrotM(Mtx, s16, s16, s16) */
static inline void mDoMtx_XYZrotM(Mtx34* m, s16 x, s16 y, s16 z) { gabi::call(0x025F19F8, m, x, y, z); }
/* 024F1478 dBgS_ObjGndChk_Wtr_Func(cXyz*) -> water height (as in d_a_salvage) */
static inline f32 dBgS_ObjGndChk_Wtr_Func(cXyz* pos) { return gabi::call<f32>(0x024F1478, pos); }
/* dSv_player_collect_c (save info + 0xD4): 025B7944 onCollect(idx, item), 025B79B8 offCollect(idx, item) */
static inline u32 dComIfGs_collect() { return gabi::load<u32>(0x101F84DC) + 0xD4; }
static inline void dComIfGs_onCollect(s32 idx, u8 item) { gabi::call(0x025B7944, dComIfGs_collect(), idx, item); }
static inline void dComIfGs_offCollect(s32 idx, u8 item) { gabi::call(0x025B79B8, dComIfGs_collect(), idx, item); }
/* 02522398 dComIfGs_setSelectEquip(s32 type, u8 itemNo) */
static inline void dComIfGs_setSelectEquip(s32 type, u8 itemNo) { gabi::call(0x02522398, type, itemNo); }

struct daObj_Demo_Barrel_c : fopAc_ac_c {
    cPhs_State _create();
    bool _delete();
    bool _draw();
    bool _execute();
    void setParticleHahen();
    void setParticleSibuki();
    BOOL CreateHeap();

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3BC */ u8 _3BC[0x3EC - 0x3BC];
    /* 0x3EC */ cXyz m2D0;
    /* 0x3F8 */ be<s16> m2DC;
    /* 0x3FA */ be<s16> m2DE;
};
WWHD_OFFSET(daObj_Demo_Barrel_c, m2D0, 0x3EC);
WWHD_OFFSET(daObj_Demo_Barrel_c, m2DE, 0x3FA);

/* 02333FC4 */
void daObj_Demo_Barrel_c::setParticleHahen() {
    WWHD_FUNC(0x02333FC4, void, this);
    const GXColor* k0 = gabi::at<GXColor>(gabi::ea(this) + 0x1A8); /* &tevStr.mColorK0 */
    u32 emitter = gabi::ea(dPa_control_set(dComIfGp_getParticle(), 0, 0x3E5 /* ID_IT_JN_TR_HAHEN_A */, &current.pos, nullptr,
                                           nullptr, 0xFF, nullptr, -1, k0, k0, nullptr));
    if (emitter != 0) {
        /* static JGeometry::TVec3<f32> em_scl(1.0f, 0.8f, 1.0f) (guard 10469614, value 10469618) */
        be<u32>& guard = *gabi::at<be<u32>>(0x10469614);
        cXyz* em_scl = gabi::at<cXyz>(0x10469618);
        if (guard != 0) {
            f32 x = em_scl->x, y = em_scl->y, z = em_scl->z;
            gabi::store<f32>(emitter + 0xC, y); /* emitter->setEmitterScale(em_scl) */
            gabi::store<f32>(emitter + 0x8, x);
            gabi::store<f32>(emitter + 0x10, z);
        } else {
            guard = 1;
            em_scl->x = 1.0f;
            em_scl->y = 0.8f;
            em_scl->z = 1.0f;
            gabi::store<f32>(emitter + 0x10, 1.0f);
            gabi::store<f32>(emitter + 0x8, 1.0f);
            gabi::store<f32>(emitter + 0xC, 0.8f);
        }
    }
}
VERIFY(0x02333FC4, &daObj_Demo_Barrel_c::setParticleHahen);

/* 02334098 */
void daObj_Demo_Barrel_c::setParticleSibuki() {
    WWHD_FUNC(0x02334098, void, this);
    gabi::Local<cXyz> sp18;
    f32 py = current.pos.y, px = current.pos.x;
    m2D0.y = py;
    m2D0.x = px;
    f32 pz = current.pos.z;
    sp18->x = px;
    m2D0.z = pz;
    sp18->z = pz;
    sp18->y = 100.0f;
    m2D0.y = dBgS_ObjGndChk_Wtr_Func(sp18);
    static const u16 ids[] = {
        0x003D, /* ID_IT_JN_WP_HAMON01 */
        0x003E, /* ID_IT_JN_WP_HAMON02 */
        0x80CB, /* ID_IT_SN_DEMO_HAMON_S */
        0x80CC, /* ID_IT_SN_DEMO_SUIMEN_A */
        0x80CE, /* ID_IT_SN_MIZUBASHIRA2 */
        0x80DC, /* ID_IT_SN_DEMO_SHIBUKI_A */
        0x80DD, /* ID_IT_SN_DEMO_SHIBUKI_B */
    };
    for (int i = 0; i < 7; i++)
        dPa_control_set(dComIfGp_getParticle(), 0, ids[i], &m2D0, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
}
VERIFY(0x02334098, &daObj_Demo_Barrel_c::setParticleSibuki);

/* 02333CF4 */
BOOL daObj_Demo_Barrel_c::CreateHeap() {
    WWHD_FUNC(0x02333CF4, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectIDRes(M_arcname, dRes_ID_DBARREL_BDL_KTARU_02_e, SAFESTRING_VTBL);
    J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectIDRes(M_arcname, dRes_ID_DBARREL_BCK_02_TR_CD_e, SAFESTRING_VTBL);
    mDoExt_McaMorf* morf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, bck, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 0,
                                                  nullptr, 0, 0x11020203);
    mpMorf = morf;
    if (morf == nullptr || morf->getModel() == nullptr) {
        return FALSE;
    }
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectIDRes(M_arcname, dRes_ID_DBARREL_BCK_02_TR_CD_e, SAFESTRING_VTBL);
    mpMorf->setAnm(anm, 0, 0.0f, 1.0f, 0.0f, -1.0f, nullptr);
    morf = mpMorf;
    morf->setFrame(morf->getEndFrame() - 1.0f);
    mpModel = mpMorf->getModel();
    return TRUE;
}
VERIFY(0x02333CF4, &daObj_Demo_Barrel_c::CreateHeap);

/* 02333EB0 */
static BOOL CheckCreateHeap(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x02333EB0, BOOL, a_this);
    return ((daObj_Demo_Barrel_c*)a_this)->CreateHeap();
}
VERIFY(0x02333EB0, CheckCreateHeap);

/* 02333EB4: daObj_Demo_BarrelCreate (_create inlined) */
static cPhs_State daObj_Demo_BarrelCreate(void* v_this) {
    WWHD_FUNC(0x02333EB4, cPhs_State, v_this);
    daObj_Demo_Barrel_c* i_this = (daObj_Demo_Barrel_c*)v_this;
    /* fopAcM_ct_Retail(this, daObj_Demo_Barrel_c) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = BARREL_VTBL;
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    cPhs_State ret = dComIfG_resLoad(&i_this->mPhase, M_arcname);
    if (ret == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(i_this, 0x02333EB0 /* CheckCreateHeap */, 0x22E0)) {
            return cPhs_ERROR_e;
        }
        i_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpModel)); /* fopAcM_SetMtx */
        i_this->m2DC = 1;
        i_this->m2DE = 0;
    }
    return ret;
}
VERIFY(0x02333EB4, daObj_Demo_BarrelCreate);

/* 02333F94 */
static BOOL daObj_Demo_BarrelDelete(void* v_this) {
    WWHD_FUNC(0x02333F94, BOOL, v_this);
    dComIfG_resDelete(&((daObj_Demo_Barrel_c*)v_this)->mPhase, M_arcname); /* dComIfG_resDeleteDemo */
    return TRUE;
}
VERIFY(0x02333F94, daObj_Demo_BarrelDelete);

/* 02334294: daObj_Demo_BarrelExecute (_execute inlined) */
static BOOL daObj_Demo_BarrelExecute(void* v_this) {
    WWHD_FUNC(0x02334294, BOOL, v_this);
    daObj_Demo_Barrel_c* i_this = (daObj_Demo_Barrel_c*)v_this;
    u8 id = i_this->demoActorID;
    if (id != 0) {
        void* ac = nullptr;
        if (id <= 0x20) { /* dComIfGp_demo_getActor(id) */
            if (gabi::load<u32>(0x101D5FFC) == 0) /* JUT_ASSERT(0x23A, m_object != NULL) */
                JUT_ASSERT_fail(STR(0x100271F4), 0x23A, STR(0x100271E4));
            ac = dDemo_object_getActor(gabi::load<u32>(0x101D5FFC), id);
        }
        /* HD: null check on the demo actor */
        if (ac != nullptr && (gabi::load<u16>(gabi::ea(ac) + 4) & 1) /* checkEnable(ENABLE_UNK_e) */) {
            u32 prm = gabi::ea(ac) + 0x4C; /* ac->getPrm() */
            if (i_this->m2DC == 1 && gabi::load<s32>(prm) == 1) {
                i_this->m2DC = 0;
                dComIfGs_onCollect(3, 0);
                dComIfGs_offCollect(0, 0);
                dComIfGs_setSelectEquip(0, 0xFF /* dItemNo_NONE_e */);
                i_this->setParticleHahen();
            }
            if (i_this->m2DE == 0 && gabi::load<s32>(prm) == 2) {
                i_this->setParticleSibuki();
                i_this->m2DE = 1;
            }
        }
    }
    dDemo_setDemoData(i_this, 0x6A, i_this->mpMorf, STR(0x100271B0) /* "DBarrel" */, 0, nullptr, 0, 0);
    J3DModel_setBaseScale(i_this->mpModel, &i_this->scale);
    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    mDoMtx_XYZrotM(mDoMtx_stack_c::get(), i_this->shape_angle.x, i_this->shape_angle.y, i_this->shape_angle.z);
    J3DModel_setBaseTRMtx(i_this->mpModel, mDoMtx_stack_c::get());
    return FALSE;
}
VERIFY(0x02334294, daObj_Demo_BarrelExecute);

/* 02334494: daObj_Demo_BarrelDraw (_draw inlined) */
static BOOL daObj_Demo_BarrelDraw(void* v_this) {
    WWHD_FUNC(0x02334494, BOOL, v_this);
    daObj_Demo_Barrel_c* i_this = (daObj_Demo_Barrel_c*)v_this;
    if (i_this->m2DC == 0) {
        return FALSE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), i_this->mpModel, &i_this->tevStr);
    i_this->mpMorf->updateDL();
    return TRUE;
}
VERIFY(0x02334494, daObj_Demo_BarrelDraw);

/* 023345A8 */
static BOOL daObj_Demo_BarrelIsDelete(void* v_this) {
    WWHD_FUNC(0x023345A8, BOOL, v_this);
    return TRUE;
}
VERIFY(0x023345A8, daObj_Demo_BarrelIsDelete);

/* 02334500 */
static void __sinit_d_a_obj_demo_barrel_cpp() {
    WWHD_FUNC(0x02334500, void, (u32)0);
    sinit_header_statics(0x104695F8, 0x101C8AE0);
}
VERIFY(0x02334500, __sinit_d_a_obj_demo_barrel_cpp);

/* 02334594: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02334594, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02334594, trivial_dt);

/* 023345B0: daObj_Demo_Barrel_c deleting destructor (in the class vtable 100271D4) */
static void daObj_Demo_Barrel_c_dt(daObj_Demo_Barrel_c* i_this, s32 flags) {
    WWHD_FUNC(0x023345B0, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x023345B0, daObj_Demo_Barrel_c_dt);

/* 02334604: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x02334604, void, p);
}
VERIFY(0x02334604, empty_virtual);
