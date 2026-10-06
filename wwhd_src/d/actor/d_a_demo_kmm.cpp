/**
 * d_a_demo_kmm.cpp (WWHD)
 * Demo actor - Kamome (a seagull model used in cutscenes).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_demo_kmm.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define M_arcname STR(0x1000DC98) /* "Demo_Kmm" (static const char[]: one address) */
#define SAFESTRING_VTBL 0x1000DC34
#define KMM_VTBL 0x1000DC4C

enum {
    dRes_ID_DEMO_KMM_BMD_KA_e = 0,
    dRes_ID_DEMO_KMM_BCK_KA_WAIT1_e = 2,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* dRes_control_c::getIDRes through a sead::SafeString key (this TU's vtable) */
static inline void* dComIfG_getObjectIDRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::ea(arc);
    key->__vtbl = SAFESTRING_VTBL;
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), id);
}
/* 02527028 dDemo_setDemoData(actor, flags, morf, arc, n, ids, p6, p7) */
static inline BOOL dDemo_setDemoData(fopAc_ac_c* a, u32 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, void* ids, u32 p6, s8 p7) {
    return gabi::call<BOOL>(0x02527028, a, flags, morf, arc, n, ids, p6, p7);
}
/* 025D5928 fopAcM_fastCreate(name, param, pos, roomNo, angle, scale, subtype, createFunc, data) */
static inline fopAc_ac_c* fopAcM_fastCreate(s16 name, u32 param, cXyz* pos, s32 roomNo, csXyz* angle, cXyz* scale, s8 subtype,
                                            u32 createFunc, void* data) {
    return gabi::call<fopAc_ac_c*>(0x025D5928, name, param, pos, roomNo, angle, scale, subtype, createFunc, data);
}
/* HD: strcmp(dComIfGp_getStartStageName() (0x1047E6B8), name) == 0 through two sead::SafeString
 * temporaries (this TU's assureTerminationImpl_ 02124510, called directly and through the vtable)
 * (as d_a_npc_bmsw's startStageIs) */
static inline bool startStageIs(u32 name) {
    gabi::Local<SafeString> a;
    gabi::Local<SafeString> b;
    a->__vtbl = SAFESTRING_VTBL;
    b->__vtbl = SAFESTRING_VTBL;
    a->mStringTop = name;
    b->mStringTop = 0x1047E6B8;
    gabi::call(0x02124510, a.get());
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
    u32 s1 = a->mStringTop;
    gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b.get());
    u32 s2 = b->mStringTop;
    if (s1 == s2)
        return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c1 = gabi::load<u8>(s1 + i);
        u8 c2 = gabi::load<u8>(s2 + i);
        if (c1 != c2)
            return false;
        if (c1 == 0)
            return true;
    }
    return false;
}

struct daDemo_Kmm_c : fopAc_ac_c {
    BOOL draw();
    BOOL execute();
    void setAction(u8 idx) { unk_29C = idx; }

    BOOL CreateHeap();
    void calcMtx();
    void setAnime(s32, s32, f32, f32);
    BOOL CreateInit();
    cPhs_State create();

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3B8 */ be<u8> unk_29C;
    /* 0x3B9 */ u8 _3B9;
    /* 0x3BA */ be<s16> unk_29E;
    /* 0x3BC */ u8 unk2A0[4];
};
WWHD_OFFSET(daDemo_Kmm_c, unk_29E, 0x3BA);
WWHD_SIZE(daDemo_Kmm_c, 0x3C0);

/* 02123E98 */
static BOOL CheckCreateHeap(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x02123E98, BOOL, a_this);
    return ((daDemo_Kmm_c*)a_this)->CreateHeap();
}
VERIFY(0x02123E98, CheckCreateHeap);

/* 02123D88 */
BOOL daDemo_Kmm_c::CreateHeap() {
    WWHD_FUNC(0x02123D88, BOOL, this);
    J3DModelData* data = (J3DModelData*)dComIfG_getObjectIDRes(M_arcname, dRes_ID_DEMO_KMM_BMD_KA_e);
    mpMorf = mDoExt_McaMorf::create(nullptr, data, nullptr, nullptr, nullptr, 2 /* J3DFrameCtrl::EMode_LOOP */, 1.0f, 0, -1, 0,
                                    nullptr, 0, 0x11020203);
    if (mpMorf == nullptr || mpMorf->mpModel == nullptr) {
        return FALSE;
    }
    setAnime(dRes_ID_DEMO_KMM_BCK_KA_WAIT1_e, 2 /* J3DFrameCtrl::EMode_LOOP */, 0.0f, 1.0f);
    return TRUE;
}
VERIFY(0x02123D88, &daDemo_Kmm_c::CreateHeap);

/* 02123F18 */
void daDemo_Kmm_c::calcMtx() {
    WWHD_FUNC(0x02123F18, void, this);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpMorf->mpModel, mDoMtx_stack_c::get());
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    eyePos.x = x;
    eyePos.y = y;
    eyePos.z = z;
    /* attention_info.position (attention_info + 0x8) */
    gabi::store<f32>(gabi::ea(this) + 0x390, x);
    gabi::store<f32>(gabi::ea(this) + 0x394, y);
    gabi::store<f32>(gabi::ea(this) + 0x398, z);
}
VERIFY(0x02123F18, &daDemo_Kmm_c::calcMtx);

/* 02123CC8 */
void daDemo_Kmm_c::setAnime(s32 animId, s32 loopMode, f32 morf, f32 playSpeed) {
    WWHD_FUNC(0x02123CC8, void, this, animId, loopMode, morf, playSpeed);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectIDRes(M_arcname, animId);
    mpMorf->setAnm(anm, loopMode, morf, playSpeed, 0.0f, -1.0f, nullptr);
}
VERIFY(0x02123CC8, &daDemo_Kmm_c::setAnime);

/* 02124324 */
BOOL daDemo_Kmm_c::CreateInit() {
    WWHD_FUNC(0x02124324, BOOL, this);
    tevStr.mRoomNo = current.roomNo;
    setAction(0);
    unk_29E = 0;
    calcMtx();
    return TRUE;
}
VERIFY(0x02124324, &daDemo_Kmm_c::CreateInit);

/* 0212435C */
cPhs_State daDemo_Kmm_c::create() {
    WWHD_FUNC(0x0212435C, cPhs_State, this);
    /* fopAcM_ct(this, daDemo_Kmm_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = KMM_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State ret = dComIfG_resLoad(&mPhase, M_arcname);
    if (ret != cPhs_COMPLEATE_e) {
        return ret;
    }
    if (!fopAcM_entrySolidHeap(this, 0x02123E98 /* CheckCreateHeap */, 0x5700)) {
        return cPhs_ERROR_e;
    }
    CreateInit();
    return cPhs_COMPLEATE_e;
}
VERIFY(0x0212435C, &daDemo_Kmm_c::create);

/* 02123E9C */
static BOOL daDemo_Kmm_actionWait(daDemo_Kmm_c* i_this) {
    WWHD_FUNC(0x02123E9C, BOOL, i_this);
    return TRUE;
}
VERIFY(0x02123E9C, daDemo_Kmm_actionWait);

/* 02123EA4 */
static BOOL daDemo_Kmm_Draw(daDemo_Kmm_c* i_this) {
    WWHD_FUNC(0x02123EA4, BOOL, i_this);
    /* HD: nothing is drawn while the s16 at 0x1047B7AE is nonzero */
    if (gabi::load<s16>(0x1047B7AE) == 0) {
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
        mDoExt_McaMorf* morf = i_this->mpMorf;
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, morf->mpModel, &i_this->tevStr);
        i_this->mpMorf->entryDL();
    }
    return TRUE;
}
VERIFY(0x02123EA4, daDemo_Kmm_Draw);

/* HD: function-local statics are initialised on first use */
BOOL daDemo_Kmm_c::execute() {
    /* two unused function-local statics (-1, 1000) */
    be<u32>& g1 = *gabi::at<be<u32>>(0x101FDA54);
    if (g1 == 0) {
        g1 = 1;
        gabi::store<s32>(0x101FDA58, -1);
    }
    be<u32>& g2 = *gabi::at<be<u32>>(0x101FDA5C);
    if (g2 == 0) {
        g2 = 1;
        gabi::store<s32>(0x101FDA60, 1000);
    }
    /* static action l_action[] = { daDemo_Kmm_actionWait } (0x101FDA68, copied from 0x101B4438) */
    be<u32>& g3 = *gabi::at<be<u32>>(0x101FDA64);
    if (g3 == 0) {
        g3 = 1;
        memcpy_g(gabi::at<u8>(0x101FDA68), gabi::at<u8>(0x101B4438), 4); /* 028FEAC0 memcpy */
    }
    /* HD: l_action[0] (GameCube: l_action[unk_29C]; the table has one entry) */
    gabi::call_ptr(gabi::load<u32>(0x101FDA68), this);
    calcMtx();

    /* HD: in Demo08, once the counter at 0x101D600C reaches 1000, the gull is replaced by three
     * actors 0xC2 at fixed positions */
    if (startStageIs(0x1000DC2C /* "Demo08" */) && gabi::load<u32>(0x101D600C) >= 1000) {
        fopAcM_delete(this);
        be<u32>& g4 = *gabi::at<be<u32>>(0x101FDA6C);
        if (g4 == 0) {
            g4 = 1;
            static const f32 init[9] = {-782.0f, 2145.0f, -199589.0f, 995.0f, 4195.0f, -201065.0f, 2099.0f, 2895.0f, -200201.0f};
            for (int i = 0; i < 9; i++)
                gabi::store<f32>(0x101FDA70 + 4 * i, init[i]);
        }
        u32 pos = 0x101FDA70;
        for (int i = 3; i != 0; i--) {
            fopAcM_fastCreate(0xC2, 0xFFFFFFFF, gabi::at<cXyz>(pos), current.roomNo, nullptr, nullptr, -1, 0, nullptr);
            pos += 0xC;
        }
        mpMorf->calc();
        return TRUE;
    }

    if (!dDemo_setDemoData(this, 0x6A, mpMorf, M_arcname, 0, nullptr, 0, 0)) {
        mpMorf->play(&eyePos, 0, 0);
    }
    mpMorf->calc();
    return TRUE;
}

/* 02123FFC */
static BOOL daDemo_Kmm_Execute(daDemo_Kmm_c* i_this) {
    WWHD_FUNC(0x02123FFC, BOOL, i_this);
    return i_this->execute();
}
VERIFY(0x02123FFC, daDemo_Kmm_Execute);

/* 021242EC */
static BOOL daDemo_Kmm_IsDelete(daDemo_Kmm_c* i_this) {
    WWHD_FUNC(0x021242EC, BOOL, i_this);
    return TRUE;
}
VERIFY(0x021242EC, daDemo_Kmm_IsDelete);

/* 021242F4 */
static BOOL daDemo_Kmm_Delete(daDemo_Kmm_c* i_this) {
    WWHD_FUNC(0x021242F4, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhase, M_arcname);
    /* HD: the (trivial) destructor call is gone */
    return TRUE;
}
VERIFY(0x021242F4, daDemo_Kmm_Delete);

/* 02124410 */
static cPhs_State daDemo_Kmm_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02124410, cPhs_State, i_this);
    return ((daDemo_Kmm_c*)i_this)->create();
}
VERIFY(0x02124410, daDemo_Kmm_Create);

/* 02124414 */
static void __sinit_d_a_demo_kmm_cpp() {
    WWHD_FUNC(0x02124414, void, (u32)0);
    sinit_header_statics(0x10463B5C, 0x101B445C);
}
VERIFY(0x02124414, __sinit_d_a_demo_kmm_cpp);

/* 021244A8: deleting destructor of a class with a trivial destructor (sead::SafeString, this TU's copy) */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x021244A8, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x021244A8, trivial_dt);

/* 021244BC: daDemo_Kmm_c deleting destructor */
static void daDemo_Kmm_c_dt(daDemo_Kmm_c* i_this, s32 flags) {
    WWHD_FUNC(0x021244BC, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x021244BC, daDemo_Kmm_c_dt);

/* 02124510: sead::SafeString::assureTerminationImpl_ (this TU's copy): empty */
static void SafeString_assureTerminationImpl(SafeString* s) {
    WWHD_FUNC(0x02124510, void, s);
}
VERIFY(0x02124510, SafeString_assureTerminationImpl);

/* 02124514: a virtual returning TRUE (per-TU copy) */
static BOOL virtual_true(void* p) {
    WWHD_FUNC(0x02124514, BOOL, p);
    return TRUE;
}
VERIFY(0x02124514, virtual_true);
