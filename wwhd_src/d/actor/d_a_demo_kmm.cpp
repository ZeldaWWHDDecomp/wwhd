/**
 * d_a_demo_kmm.cpp (WWHD)
 * Demo actor - Kamome (a seagull model used in cutscenes).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_demo_kmm.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_demo_kmm.h"

const gc_tu_data gc_tu = {
    .safeString = 0x1000DC34,
    .headerStatics = {0x10463B5C, 0x101B445C},
};
const char daDemo_Kmm_c::M_arcname[] = "Demo_Kmm";
static s16 demoDrawDisabled;
static u32 demoFrameCounter;
static DemoKmmStatics executeStatics;
// HD: the checked one-entry function table is copied to guarded BSS before dispatch.
static u32 l_action_src[1] = {0x02123E9C};

/* 02123E98 */
static BOOL CheckCreateHeap(fopAc_ac_c* a_this) {
    return ((daDemo_Kmm_c*)a_this)->CreateHeap();
}

/* 02123D88 */
BOOL daDemo_Kmm_c::CreateHeap() {
    J3DModelData* data = (J3DModelData*)dComIfG_getObjectIDRes(M_arcname, dRes_ID_DEMO_KMM_BMD_KA_e);
    mpMorf = mDoExt_McaMorf::create(nullptr, data, nullptr, nullptr, nullptr, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 0,
                                    nullptr, 0, 0x11020203);
    if (mpMorf == nullptr || mpMorf->mpModel == nullptr) {
        return FALSE;
    }
    setAnime(dRes_ID_DEMO_KMM_BCK_KA_WAIT1_e, J3DFrameCtrl::EMode_LOOP, 0.0f, 1.0f);
    return TRUE;
}

/* 02123F18 */
void daDemo_Kmm_c::calcMtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    mpMorf->getModel()->setBaseTRMtx(mDoMtx_stack_c::get());
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    eyePos.x = x;
    eyePos.y = y;
    eyePos.z = z;
    // HD: reuse the captured position components for both destinations.
    attention_info.position.x = x;
    attention_info.position.y = y;
    attention_info.position.z = z;
}

/* 02123CC8 */
void daDemo_Kmm_c::setAnime(s32 animId, s32 loopMode, f32 morf, f32 playSpeed) {
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectIDRes(M_arcname, animId);
    mpMorf->setAnm(anm, loopMode, morf, playSpeed, 0.0f, -1.0f, nullptr);
}

/* 02124324 */
BOOL daDemo_Kmm_c::CreateInit() {
    tevStr.mRoomNo = current.roomNo;
    setAction(0);
    unk_29E = 0;
    calcMtx();
    return TRUE;
}

/* 0212435C */
cPhs_State daDemo_Kmm_c::create() {
    fopAcM_ct(this, daDemo_Kmm_c);
    cPhs_State ret = dComIfG_resLoad(&mPhase, M_arcname);
    if (ret != cPhs_COMPLEATE_e) {
        return ret;
    }
    if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, 0x5700)) {
        return cPhs_ERROR_e;
    }
    CreateInit();
    return cPhs_COMPLEATE_e;
}

/* 02123E9C */
static BOOL daDemo_Kmm_actionWait(daDemo_Kmm_c* i_this) {
    return TRUE;
}

/* 02123EA4 */
static BOOL daDemo_Kmm_Draw(daDemo_Kmm_c* i_this) {
    /* HD: nothing is drawn while the s16 at 0x1047B7AE is nonzero */
    if (demoDrawDisabled == 0) {
        g_env_light.settingTevStruct(TEV_TYPE_BG0, &i_this->current.pos, &i_this->tevStr);
        mDoExt_McaMorf* morf = i_this->mpMorf;
        DemoKmmEnv_view* env = (DemoKmmEnv_view*)dKy_getEnvlight();
        env->setLightTevColorType(morf->mpModel, &i_this->tevStr);
        i_this->mpMorf->entryDL();
    }
    return TRUE;
}

/* inlined into daDemo_Kmm_Execute */
// HD: function-local statics are initialized on first use.
BOOL daDemo_Kmm_c::execute() {
    // HD: guards are written before their values, as in the native first-use paths.
    if (executeStatics.guardUnused1 == 0) {
        executeStatics.guardUnused1 = 1;
        executeStatics.unused1 = -1;
    }
    if (executeStatics.guardUnused2 == 0) {
        executeStatics.guardUnused2 = 1;
        executeStatics.unused2 = 1000;
    }
    if (executeStatics.guardAction == 0) {
        executeStatics.guardAction = 1;
        memcpy_g(&executeStatics.l_action, l_action_src, 4);
    }
    // HD: GHS folds the one-entry action table index to zero.
    executeStatics.runAction(this);
    calcMtx();

    /* HD: in Demo08, once the counter at 0x101D600C reaches 1000, the gull is replaced by three
     * actors 0xC2 at fixed positions */
    if (startStageIs("Demo08") && demoFrameCounter >= 1000) {
        fopAcM_delete(this);
        if (executeStatics.guardSpawnPos == 0) {
            executeStatics.guardSpawnPos = 1;
            executeStatics.spawnPos[0].set(-782.0f, 2145.0f, -199589.0f);
            executeStatics.spawnPos[1].set(995.0f, 4195.0f, -201065.0f);
            executeStatics.spawnPos[2].set(2099.0f, 2895.0f, -200201.0f);
        }
        cXyz* pos = executeStatics.spawnPos;
        for (int i = 3; i != 0; i--) {
            fopAcM_fastCreate(fpcNm_KAMOME_e, 0xFFFFFFFF, pos, current.roomNo, nullptr, nullptr, -1, 0, nullptr);
            pos++;
        }
        mpMorf->calc();
        return TRUE;
    }

    if (!dDemo_setDemoData(this, dDemo_actor_c::ENABLE_TRANS_e | dDemo_actor_c::ENABLE_ROTATE_e |
                           dDemo_actor_c::ENABLE_ANM_e | dDemo_actor_c::ENABLE_ANM_FRAME_e, mpMorf, M_arcname, 0, nullptr, 0, 0)) {
        mpMorf->play(&eyePos, 0, 0);
    }
    mpMorf->calc();
    return TRUE;
}

/* 02123FFC */
static BOOL daDemo_Kmm_Execute(daDemo_Kmm_c* i_this) {
    return i_this->execute();
}

/* 021242EC */
static BOOL daDemo_Kmm_IsDelete(daDemo_Kmm_c* i_this) {
    return TRUE;
}

/* 021242F4 */
static BOOL daDemo_Kmm_Delete(daDemo_Kmm_c* i_this) {
    dComIfG_resDelete(&i_this->mPhase, daDemo_Kmm_c::M_arcname);
    /* HD: the (trivial) destructor call is gone */
    return TRUE;
}

/* 02124410 */
static cPhs_State daDemo_Kmm_Create(fopAc_ac_c* i_this) {
    return ((daDemo_Kmm_c*)i_this)->create();
}

/* 02124414 */
static void __sinit_d_a_demo_kmm_cpp() {
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
}

/* 021244A8 */
// HD: deleting destructor of a class with a trivial destructor (sead::SafeString, this TU's copy)
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 021244BC */
// HD: daDemo_Kmm_c deleting destructor
static void daDemo_Kmm_c_dt(daDemo_Kmm_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 02124510 */
// HD: sead::SafeString::assureTerminationImpl_ (this TU's copy): empty
static void SafeString_assureTerminationImpl(SafeString* s) {
}

/* 02124514 */
// HD: a virtual returning TRUE (per-TU copy)
static BOOL virtual_true(void* p) {
    return TRUE;
}

/* WWHD glue (tools/verify/autoglue.py)
str 1000DC98 "Demo_Kmm"
str 1000DC2C "Demo08"
data demoDrawDisabled 1047B7AE
data demoFrameCounter 101D600C
data executeStatics 101FDA54
data l_action_src 101B4438
ext 02527028 BOOL dDemo_setDemoData(fopAc_ac_c*, u32, mDoExt_McaMorf*, const char*, s32, void*, u32, s8)
ext 025D5928 fopAc_ac_c* fopAcM_fastCreate(s16, u32, cXyz*, s32, csXyz*, cXyz*, s8, u32, void*)
*/
