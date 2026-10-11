/* Rising sea surfaces and the Tower of the Gods water rescue sequence.
 * GameCube methods and asserted HD layouts; original call/store order is verified unchanged. */
#include "bindings.h"
#include "d/actor/d_a_obj_tide.h"
using Tide = daObjTide::Act_c;
static const u32 dBgS_MoveBGProc_Trans = 0x024EE76C;
dComIfG_inf_c* tideGetGameInfo();
void* tideGetObjectResource(void*, void*, s32);
void tideAssert(const char*, s32, const char*);
s32 tideIsDungeonItem(void*, s32);
void tideConstructMoveBg(void*);
void tideClearTmpBit(void*, s32);
s32 tideLoadResource(void*, const char*);
s32 tideCreateMoveBg(void*, const char*, s32, u32, u32);
s32 tideDeleteMoveBg(void*);
void tideDeleteResource(void*, const char*);
J3DModel* tideCreateModel(void*, s32, u32);
void* tideAllocate(u32);
void tideInitFrame(void*, s32);
void tideConstructMaya(void*);
s32 tideInitBck(void*, void*, void*, s32, s32, s32, s32, s32, f32);
mDoExt_brkAnm* tideConstructBrk(void*);
s32 tideInitBrk(void*, void*, void*, s32, s32, s32, s32, s32, s32, f32);
mDoExt_btkAnm* tideConstructBtk(void*);
s32 tideInitBtk(void*, void*, void*, s32, s32, s32, s32, s32, s32, f32);
void tideRotateZXY(void*, s32, s32, s32);
void tideCopyMatrix(void*, void*);
void tideSetCullBox(void*, f32, f32, f32, f32, f32, f32);
void tideDeleteActor(void*);
void tideSetLighting(void*, s32, void*, void*);
dScnKy_env_light_c* tideGetEnvironment();
void tideSetModelLighting(void*, void*, void*);
void tideEntryBck(void*, void*, f32);
void tideEntryBrk(void*, void*, f32);
void tideEntryBtk(void*, void*, f32);
void tideUpdateModel(void*, s32);
tideHD::EnvironmentActor* tideFindEnvironmentActor(s32);
void tideDeleteEnvironmentActor(void*);
void tideDeleteSound(void*);
void tideAddCalc(void*, f32, f32, f32, f32);
void tideStartSound(s32);
s32 tideCheckEventStart(void*, const char*);
tideHD::DemoActor* tideGetDemoActor(void*, u32);
s32 tidePlayAnimation(void*);
void tideReleaseBackground(void*, void*);
s32 tideCreateEnvironmentActor(s32, s32, void*, s32, s32);
s32 tideGetReverb(s32);
void tideStartPositionSound(s32, void*, s32, s32);
s32 tideIsSwitch(void*, u32, s32);
s32 tideIsTmpBit(void*, s32);
void tideSetParticle(void*, s32, s32, void*, s32, s32, s32, s32, s32, s32, s32, s32);
void tideRegisterBackground(void*, void*, void*);
s32 tideCheckFrame(void*, f32);
s32 tideExecuteMoveBg(void*);
void tideRegisterDestructor(void*);
void tideConstructAngleTag(void*);
void tideConstructByteTag(void*);
void tideFree(void*);
void tideDestroyActor(void*, s32);
static s32 Mthd_Create(Tide* tide);
static s32 Mthd_Delete(Tide* tide);
static s32 Mthd_Execute(Tide* tide);
static s32 Mthd_Draw(Tide* tide);
static s32 Mthd_IsDelete(Tide* tide);
void __sinit_d_a_obj_tide_cpp();
void tideHioDestructor(void* object, s32 flags);
BOOL tideMoveBGIsDelete(Tide* tide);
void tideHioGenMessage(Tide* tide);
void tideActorDestructor(Tide* tide, s32 flags);
u32 tidePrmAbstract(Tide* tide, u32 bits, u32 shift);

// Resource indices follow the Humi/Gmtw enums in the header; numeric entries permit image checks.
static daObjTide::Attr_c tideAttributes[6] = {
    {"Humi",     0xB,    0x20,  -1,     -1,   0x19,  {0, 0}, 0x11000222, 0x6,   0x12,
     0x11000022, 0x8000, -460,  {0, 0}, 1.0f, 12.0f, 1.0f,   0.13f,      -4000, -1700,
     -3450,      8800,   -1600, 4600,   1,    0,     1,      0.45f},
    {"Humi",     0xE,    0x23,  -1,     -1,   0x1C,  {0, 0}, 0x11000222, 0x9,  0x15,
     0x11000022, 0x4000, -700,  {0, 0}, 1.0f, 14.0f, 1.0f,   0.15f,      5600, -1650,
     -3300,      7300,   -1550, -1800,  1,    0,     5,      0.5f},
    {"Humi",     0xF,    0x24,  -1,     -1,   0x1D,  {0, 0}, 0x11000222, 0xA,  0x16,
     0x11000022, 0x4000, -600,  {0, 0}, 1.0f, 10.0f, 1.0f,   0.11f,      5700, -1650,
     1400,       7800,   -1550, 4800,   1,    0,     5,      0.5f},
    {"Gmtw", 0x9,    0xF,   0x6,    -1,    0xC,  {0, 0}, 0x11001222, -1,    -1,
     0x0,    0x4000, -6000, {0, 0}, 0.01f, 1.0f, 1.0f,   0.01f,      -1800, -300,
     -1800,  1800,   150,   1800,   0,     0,    0,      1.0f},
    {"Humi",     0xC,    0x21,  -1,     -1,   0x1A,  {0, 0}, 0x11000222, 0x7,   0x13,
     0x11000022, 0x4000, -550,  {0, 0}, 1.0f, 12.0f, 1.0f,   0.13f,      -7200, -1650,
     -300,       -3900,  -1550, 1700,   1,    0,     3,      0.5f},
    {"Humi",     0xD,    0x22,  -1,     -1,   0x1B,  {0, 0}, 0x11000222, 0x8,  0x14,
     0x11000022, 0x4000, -550,  {0, 0}, 1.0f, 12.0f, 1.0f,   0.13f,      3800, -1650,
     400,        6000,   -1550, 1500,   1,    0,     5,      0.5f},
};
static TideModeEntry tideModeProc[4] = {
    {&daObjTide::Act_c::mode_norm},
    {&daObjTide::Act_c::mode_gmtw_pre},
    {&daObjTide::Act_c::mode_gmtw},
    {&daObjTide::Act_c::mode_gmtw_demo},
};
static void* tideResources;
static tideHD::SaveBlock* tideSave;
static void* tideDemoManager;
static f32 tideWaterLevel;
static u32 tideWaterState;
static Mtx34 tideTemporaryMatrix;
static tideHD::DrawBufferPair tideDrawBuffers;
static tideHD::HeaderZeros tideHeaderZeros;
static f32 tideNegativePi;
static f32 tidePositivePi;
static f32 tidePiNegative;
static f32 tidePiPositive;
static u8 tideAngleTag;
static u8 tideByteTag;
static tideHD::DestructorRecord tideRecord0;
static tideHD::DestructorRecord tideRecord1;
static tideHD::DestructorRecord tideRecord2;

static daObjTide::Attr_c* attr(Tide* tide) {
    return &tideAttributes[u32(tide->M_type)];
}
static void* resource(const char* archive, s32 index) {
    gabi::Local<SafeString> name;
    name->mStringTop = gabi::ea(archive);
    name->__vtbl = 0x10030B60;
    return tideGetObjectResource(tideResources, name.get(), index);
}
static void assertResource(void* data, s32 line, const char* text) {
    if (!data)
        tideAssert("d_a_obj_tide.cpp", line, text);
}

// HD: bck embeds its calculator and constructor stores; animation allocation sizes differ.
/* 023A06C0 */
BOOL daObjTide::Act_c::CreateHeap() {
    const char* archive = attr(this)->arcName;
    J3DModelData* modelData =
        static_cast<J3DModelData*>(resource(archive, attr(this)->bdl_res_index));
    assertResource(modelData, 0x1BA, "model_data != 0");
    u32 flags = attr(this)->bdl_dlist_flags;
    mModel1 = flags ? tideCreateModel(modelData, 0x80000, flags)
                    : tideCreateModel(modelData, 0, 0x11020203);
    bool bckOK = true, brkOK = true, btkOK = true, secondModelOK = true, secondBrkOK = true;
    s32 index = attr(this)->bck_res_index;
    if (index >= 0) {
        void* animation = resource(archive, index);
        assertResource(animation, 0x1CB, "bck != 0");
        mDoExt_bckAnm* bck = static_cast<mDoExt_bckAnm*>(tideAllocate(0x8C));
        if (bck) {
            tideInitFrame(bck, 0);
            tideHD::BckConstruction* construction = reinterpret_cast<tideHD::BckConstruction*>(bck);
            construction->__vtbl = 0x1016E54C;
            tideConstructMaya(construction->mMaya);
            construction->__vtblMaya = 0x1016D820;
            construction->__vtbl = 0x10030B78;
            construction->mpAnm = 0;
            construction->mpMtxCalc = 0;
            construction->mpMtxCalc2 = 0;
            construction->mpTransform = 0;
            construction->mAnm = 0;
        }
        mBck1 = bck;
        if (!bck || !tideInitBck(bck, modelData, animation, 1, 0, 0, 0x12B, 0, 1.f))
            bckOK = false;
    }
    index = attr(this)->brk_res_index;
    if (index >= 0) {
        void* animation = resource(archive, index);
        assertResource(animation, 0x1DE, "brk != 0");
        mDoExt_brkAnm* brk = static_cast<mDoExt_brkAnm*>(tideAllocate(0x78));
        if (brk)
            brk = tideConstructBrk(brk);
        mBrk1 = brk;
        if (!brk || !tideInitBrk(brk, modelData, animation, 1, 2, 0, -1, 0, 0, 1.f))
            brkOK = false;
    }
    index = attr(this)->btk_res_index;
    if (index >= 0) {
        void* animation = resource(archive, index);
        assertResource(animation, 0x1EB, "btk != 0");
        mDoExt_btkAnm* btk = static_cast<mDoExt_btkAnm*>(tideAllocate(0x74));
        if (btk)
            btk = tideConstructBtk(btk);
        mBtk = btk;
        if (!btk || !tideInitBtk(btk, modelData, animation, 1, 2, 0, -1, 0, 0, 1.f))
            btkOK = false;
    }
    daObjTide::Attr_c* data = attr(this);
    if (data->nure_bdl_res_index >= 0 && data->nure_brk_res_index >= 0) {
        J3DModelData* secondData =
            static_cast<J3DModelData*>(resource(archive, data->nure_bdl_res_index));
        assertResource(secondData, 0x1FC, "mdl_nure_data != 0");
        mModel2 = tideCreateModel(secondData, 0x80000, attr(this)->nure_bdl_dlist_flags);
        if (!mModel2)
            secondModelOK = false;
        void* animation = resource(archive, attr(this)->nure_brk_res_index);
        assertResource(animation, 0x206, "brk_nure != 0");
        mDoExt_brkAnm* brk = static_cast<mDoExt_brkAnm*>(tideAllocate(0x78));
        if (brk)
            brk = tideConstructBrk(brk);
        mBrk2 = brk;
        if (!brk || !tideInitBrk(brk, secondData, animation, 1, 0, 0, -1, 0, 0, attr(this)->m44))
            secondBrkOK = false;
        else {
            mDoExt_brkAnm* result = mBrk2;
            result->mFrameCtrl.mFrame = (f32)result->mFrameCtrl.mEnd;
        }
    }
    return mModel1 && bckOK && brkOK && btkOK && secondModelOK && secondBrkOK;
}

// HD: a null model keeps the culling matrix null.
/* 023A0DCC */
BOOL daObjTide::Act_c::Create() {
    J3DModel* model = mModel1;
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(model));
    init_mtx();
    daObjTide::Attr_c* data = attr(this);
    tideSetCullBox(this, (f32)data->min_x, (f32)data->min_y, (f32)data->min_z, (f32)data->max_x,
                   (f32)data->max_y, (f32)data->max_z);
    m2F4 = 0;
    m2F6 = 0;
    m30A = 0;
    m2F0 = 0.f;
    if (M_type == TYPE_3_e)
        mode_gmtw_pre_init();
    else
        mode_norm_init();
    return 1;
}

// HD: clear the rescue temporary flag; keep the post-call readiness check.
/* 023A0484 */
s32 daObjTide::Act_c::Mthd_Create() {
    u32 flags = actor_condition;
    if (!(flags & fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            tideConstructMoveBg(this);
            flags = actor_condition;
            __vtbl = 0x10030FD8;
        }
        actor_condition = flags | fopAcCnd_INIT_e;
    }
    M_type = tidePrmAbstract(this, PRM_TYPE_W, PRM_TYPE_S);
    se_init_gmtw();
    m309 = 1;
    if (M_type == TYPE_3_e) {
        tideClearTmpBit(&tideSave->mSaveInfo.mTmp, tideHD::RESCUE_COMPLETE);
        if (!m309)
            return cPhs_ERROR_e;
    }
    const char* archive = attr(this)->arcName;
    s32 phase = tideLoadResource(&mPhase, archive);
    if (phase == cPhs_COMPLEATE_e) {
        daObjTide::Attr_c* data = attr(this);
        if (data->sch_flag) {
            f32 factor = 1.f - tideWaterLevel;
            f32 height = (f32)data->m20;
            current.pos.y = gabi::fmadds(factor, height, home.pos.y);
            data = attr(this);
        }
        phase = tideCreateMoveBg(this, data->arcName.get(), (s32)data->dzb_res_index,
                                 dBgS_MoveBGProc_Trans, data->res_size);
        m2F7 = 1;
        if (phase != cPhs_COMPLEATE_e && phase != cPhs_ERROR_e)
            tideAssert("d_a_obj_tide.cpp", 0x283,
                       "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)");
    }
    return phase;
}

/* 023A1210 */
BOOL daObjTide::Act_c::Delete() {
    se_stop_gopo();
    se_stop_out();
    se_stop_up();
    tideDeleteSound(&home.pos);
    return 1;
}

/* 023A0658 */
s32 daObjTide::Act_c::Mthd_Delete() {
    s32 result = 1;
    if (m309) {
        result = tideDeleteMoveBg(this);
        tideDeleteResource(&mPhase, attr(this)->arcName.get());
    }
    return result;
}

/* 023A0BA8 */
void daObjTide::Act_c::set_mtx() {
    if (mModel2) {
        mDoMtx_stack_c::transS(home.pos.x, home.pos.y, home.pos.z);
        tideRotateZXY(mDoMtx_stack_c::get(), (s32)(s16)shape_angle.x, (s32)(s16)shape_angle.y,
                      (s32)(s16)shape_angle.z);
        J3DModel_setBaseTRMtx(mModel2.get(), mDoMtx_stack_c::get());
    }
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    tideRotateZXY(mDoMtx_stack_c::get(), (s32)(s16)shape_angle.x, (s32)(s16)shape_angle.y,
                  (s32)(s16)shape_angle.z);
    J3DModel_setBaseTRMtx(mModel1.get(), mDoMtx_stack_c::get());
    tideCopyMatrix(mDoMtx_stack_c::get(), &tideTemporaryMatrix);
}

/* 023A0D14 */
void daObjTide::Act_c::init_mtx() {
    J3DModel_setBaseScale(mModel1.get(), &scale);
    if (mModel2)
        J3DModel_setBaseScale(mModel2.get(), &scale);
    set_mtx();
}

// HD: keep the full-register sound flag and the double-precision absolute comparison.
/* 023A1258 */
void daObjTide::Act_c::move_to_aim(BOOL sound, f32 aim) {
    f32 difference = aim - (f32)current.pos.y;
    if (std::fabs((f64)difference) < .1f) {
        m2F0 = 0.f;
        current.pos.y = aim;
        return;
    }
    daObjTide::Attr_c* data = attr(this);
    f32 step = (f32)m2F0 + data->m24;
    m2F0 = step;
    f32 maximum = data->m28;
    if (step > maximum) {
        step = maximum;
        m2F0 = step;
        data = attr(this);
    }
    f32 minimum = data->m2C;
    if (!(step > minimum)) {
        step = minimum + .001f;
        m2F0 = step;
        data = attr(this);
        minimum = data->m2C;
    }
    tideAddCalc(&current.pos.y, aim, data->m30, step, minimum);
    if (sound)
        tideStartSound(difference > 0.f ? tideHD::JA_SE_ATM_SHIREN_SEA_UP
                                        : tideHD::JA_SE_ATM_SHIREN_SEA_DOWN);
}

/* 023A1398 */
bool daObjTide::Act_c::chk_demo_rescue() {
    if (!tideGetGameInfo()->play.mEvtCtrl.mMode)
        return false;
    return tideCheckEventStart(&tideGetGameInfo()->play.mEvtManager, "rescue") != 0;
}

// HD: validate demo indices and diagnose a missing manager; mPrm is compared as a word.
/* 023A13F4 */
BOOL daObjTide::Act_c::gmtw_demo_up_start() {
    u32 index = demoActorID;
    if (index == 0 || index > 32)
        return 0;
    void* manager = tideDemoManager;
    if (!manager) {
        tideAssert("d_demo.h", 0x23A, "m_object != (0)");
        manager = tideDemoManager;
    }
    tideHD::DemoActor* actor = tideGetDemoActor(manager, index);
    if (!actor || !(actor->mFlags & 1))
        return 0;
    return actor->mPrm == 1;
}

/* 023A0DC0 */
void daObjTide::Act_c::mode_norm_init() {
    m2EC = MODE_NORM;
}

/* 023A14A8 */
void daObjTide::Act_c::mode_norm() {
    daObjTide::Attr_c* data = attr(this);
    f32 level = tideWaterLevel, homeHeight = home.pos.y;
    u32 flags = tideWaterState, low = flags & tideHD::STATE_1;
    current.pos.y = gabi::fmadds(1.f - level, (f32)data->m20, homeHeight);
    s16 timer = m2F4;
    if (timer > 0) {
        timer = (s16)(timer - 1);
        m2F4 = timer;
        if (!timer) {
            mBrk2->mFrameCtrl.mFrame = 0.f;
            mBrk2->mFrameCtrl.mRate = attr(this)->m44;
        }
    }
    mDoExt_baseAnm* animation = mBrk2;
    if (!low && (flags & tideHD::STATE_2))
        m2F4 = data->m42;
    if (animation)
        tidePlayAnimation(animation);
    animation = mBtk;
    if (animation)
        tidePlayAnimation(animation);
    if (!(flags & tideHD::STATE_4)) {
        if (low) {
            if (level != 1.f)
                tideStartSound(tideHD::JA_SE_ATM_SHIREN_SEA_UP);
        } else if (level != 0.f)
            tideStartSound(tideHD::JA_SE_ATM_SHIREN_SEA_DOWN);
    }
}

/* 023A0D58 */
void daObjTide::Act_c::mode_gmtw_pre_init() {
    if (attr(this)->sch_flag)
        tideAssert("d_a_obj_tide.cpp", 0x351, "attr(M_type).sch_flag == false");
    m2EC = MODE_GMTW_PRE;
    m2F6 = 0;
}

/* 023A1798 */
void daObjTide::Act_c::mode_gmtw_pre() {
    bool demo = chk_demo_rescue() != 0;
    f32 homeHeight = home.pos.y;
    if (demo) {
        current.pos.y = homeHeight;
        if (m2F7) {
            tideReleaseBackground(&tideGetGameInfo()->play.mBgS, mpBgW.get());
            m2F7 = 0;
        }
        mode_gmtw_demo_init();
    } else {
        current.pos.y = homeHeight + 100.f;
        mode_gmtw_init();
    }
}

/* 023A16AC */
void daObjTide::Act_c::mode_gmtw_init() {
    if (attr(this)->sch_flag)
        tideAssert("d_a_obj_tide.cpp", 0x373, "attr(M_type).sch_flag == false");
    mDoExt_baseAnm* animation = mBck1;
    if (animation)
        animation->mFrameCtrl.mFrame = (f32)animation->mFrameCtrl.mEnd;
    animation = mBrk1;
    if (animation) {
        animation->mFrameCtrl.mLoop = 400;
        mBrk1->mFrameCtrl.mFrame = 401.f;
    }
    animation = mBtk;
    if (animation) {
        animation->mFrameCtrl.mLoop = 400;
        mBtk->mFrameCtrl.mFrame = 401.f;
    }
    m2F6 = 2;
    m308 = 0;
    m30C = 300;
    m2EC = MODE_GMTW;
}

// HD: the completed-water height is 2923.5, followed by the rescue deletion countdown.
/* 023A1B78 */
void daObjTide::Act_c::mode_gmtw() {
    f32 aim = 2923.5f;
    bool stopped = false;
    bool latched = m308 != 0;
    if (!latched) {
        u32 sw = tidePrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
        bool on = tideIsSwitch(&tideSave->mSaveInfo, sw, (s32)(s8)home.roomNo) != 0;
        if (on) {
            aim = (f32)home.pos.y + aim;
            m308 = 1;
        } else
            aim = (f32)home.pos.y + 100.f;
    } else {
        aim = (f32)home.pos.y + aim;
        u32 sw = tidePrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
        if (!tideIsSwitch(&tideSave->mSaveInfo, sw, (s32)(s8)home.roomNo))
            current.pos.y = aim;
    }
    mDoExt_baseAnm* animation = mBck1;
    if (animation && tidePlayAnimation(animation))
        stopped = true;
    animation = mBrk1;
    if (animation)
        tidePlayAnimation(animation);
    animation = mBtk;
    if (animation)
        tidePlayAnimation(animation);
    if (stopped)
        move_to_aim(0, aim);
    se_set_gmtw();
    if (tideIsTmpBit(&tideSave->mSaveInfo.mTmp, tideHD::RESCUE_COMPLETE)) {
        s32 count = m30C;
        if (count > 0)
            m30C = count - 1;
        else
            m30A = 1;
    } else
        m30C = 300;
}

/* 023A1644 */
void daObjTide::Act_c::mode_gmtw_demo_init() {
    if (attr(this)->sch_flag)
        tideAssert("d_a_obj_tide.cpp", 0x3BF, "attr(M_type).sch_flag == false");
    m2EC = MODE_GMTW_DEMO;
    m2F6 = 0;
}

// HD: the two transition branches finish with sound updates before returning.
/* 023A1D68 */
void daObjTide::Act_c::mode_gmtw_demo() {
    u8 stage = m2F6;
    if (!stage) {
        if (gmtw_demo_up_start()) {
            m2F6 = 1;
            m2F4 = 90;
        }
        stage = m2F6;
    }
    f32 aim = (stage == 0 || stage == 1) ? (f32)home.pos.y : (f32)home.pos.y + 100.f;
    bool stopped = false;
    if (stage == 1) {
        s16 timer = (s16)((s32)(s16)m2F4 - 1);
        m2F4 = timer;
        if (timer <= 0) {
            m2F6 = 2;
            se_set_gmtw();
            return;
        }
        if (timer == 59) {
            dPa_control_c* particle = tideGetGameInfo()->play.mParticle;
            tideSetParticle(particle, 0, tideHD::ID_IT_SN_MT_WATER00, &current.pos, 0, 0, 255, 0,
                            -1, 0, 0, 0);
            tideRegisterBackground(&tideGetGameInfo()->play.mBgS, mpBgW.get(), this);
            m2F7 = 1;
            se_set_gmtw();
            return;
        }
    } else if (stage == 2 || stage == 3) {
        mDoExt_baseAnm* animation = mBck1;
        if (animation && tidePlayAnimation(animation))
            stopped = true;
        animation = mBrk1;
        if (animation) {
            tidePlayAnimation(animation);
            animation = mBrk1;
            if ((f32)animation->mFrameCtrl.mLoop == 0.f && tideCheckFrame(animation, 400.f))
                mBrk1->mFrameCtrl.mLoop = 400;
        }
        animation = mBtk;
        if (animation) {
            tidePlayAnimation(animation);
            animation = mBtk;
            if ((f32)animation->mFrameCtrl.mLoop == 0.f && tideCheckFrame(animation, 400.f))
                mBtk->mFrameCtrl.mLoop = 400;
        }
        if (stopped)
            move_to_aim(0, aim);
    }
    se_set_gmtw();
}

// HD: the stage-boss dungeon-item query uses its verified full-width result.
/* 023A0418 */
void daObjTide::Act_c::se_init_gmtw() {
    m2F9 = 0;
    M_id_gopo = -1;
    M_id_out = -1;
    m2FB = 0;
    m2FA = 0;
    m2F8 = 0;
    M_id_up = -1;
    m30B = (u8)(tideIsDungeonItem(&tideSave->mSaveInfo.mMemory.mMembit,
                                  dSv_memBit_c::STAGE_BOSS_ENEMY) != 0);
}

// HD: the completed-water height is 2923.5, with a double-precision threshold test.
/* 023A19AC */
void daObjTide::Act_c::se_set_gmtw() {
    m2FB = 0;
    u8 item = m30B;
    u8 oldGurgle = m2F8;
    m2F8 = 0;
    u8 oldOut = m2F9;
    m2F9 = 0;
    u8 oldUp = m2FA;
    m2FA = 0;
    bool finished = item != 0;
    if (!finished) {
        f32 distance = ((f32)home.pos.y + 2923.5f) - (f32)current.pos.y;
        finished = std::fabs((f64)distance) < .01f;
    }
    if (finished)
        m2FB = 1;
    else {
        m2F8 = 1;
        u8 stage = m2F6;
        if ((stage == 1 && m2F7) || stage == 2) {
            m2F9 = 1;
            m2FA = 1;
        }
    }
    if (m2F8) {
        if (!oldGurgle)
            se_start_gopo();
    } else if (oldGurgle)
        se_stop_gopo();
    if (m2F9) {
        if (!oldOut)
            se_start_out();
    } else if (oldOut)
        se_stop_out();
    if (m2FA) {
        if (!oldUp)
            se_start_up();
        else
            se_pos_up();
    } else if (oldUp)
        se_stop_up();
    if (m2FB) {
        s32 reverb = tideGetReverb((s32)(s8)current.roomNo);
        tideStartPositionSound(tideHD::JA_SE_ATM_MJ_WATER_PACHA, &eyePos, 0, reverb);
    }
}

/* 023A182C */
void daObjTide::Act_c::se_start_gopo() {
    if (M_id_gopo != -1)
        tideAssert("d_a_obj_tide.cpp", 0x461, "M_id_gopo == fpcM_ERROR_PROCESS_ID_e");
    M_id_gopo = tideCreateEnvironmentActor(tideHD::LEVEL_SE_PROCESS,
                                           tideHD::JA_SE_ATM_MJ_WATER_GOPO, &home.pos, 0, 0);
}

/* 023A1894 */
void daObjTide::Act_c::se_start_out() {
    if (M_id_out != -1)
        tideAssert("d_a_obj_tide.cpp", 0x473, "M_id_out == fpcM_ERROR_PROCESS_ID_e");
    M_id_out = tideCreateEnvironmentActor(tideHD::LEVEL_SE_PROCESS, tideHD::JA_SE_ATM_MJ_WATER_OUT,
                                          &home.pos, 0, 0);
}

/* 023A18FC */
void daObjTide::Act_c::se_start_up() {
    if (M_id_up != -1)
        tideAssert("d_a_obj_tide.cpp", 0x485, "M_id_up == fpcM_ERROR_PROCESS_ID_e");
    M_id_up = tideCreateEnvironmentActor(tideHD::LEVEL_SE_PROCESS, tideHD::JA_SE_ATM_MJ_WATER_UP,
                                         &current.pos, 0, 0);
}

/* 023A112C */
void daObjTide::Act_c::se_stop_gopo() {
    s32 id = M_id_gopo;
    if (id != -1) {
        tideHD::EnvironmentActor* effect = tideFindEnvironmentActor(id);
        if (effect)
            tideDeleteEnvironmentActor(effect);
        M_id_gopo = -1;
    }
}

/* 023A1178 */
void daObjTide::Act_c::se_stop_out() {
    s32 id = M_id_out;
    if (id != -1) {
        tideHD::EnvironmentActor* effect = tideFindEnvironmentActor(id);
        if (effect)
            tideDeleteEnvironmentActor(effect);
        M_id_out = -1;
    }
}

/* 023A11C4 */
void daObjTide::Act_c::se_stop_up() {
    s32 id = M_id_up;
    if (id != -1) {
        tideHD::EnvironmentActor* effect = tideFindEnvironmentActor(id);
        if (effect)
            tideDeleteEnvironmentActor(effect);
        M_id_up = -1;
    }
}

/* 023A1964 */
void daObjTide::Act_c::se_pos_up() {
    s32 id = M_id_up;
    if (id != -1) {
        tideHD::EnvironmentActor* effect = tideFindEnvironmentActor(id);
        if (effect)
            effect->mPos.y = current.pos.y;
    }
}

// HD: deleting the actor after the mode and matrix updates is part of Execute.
/* 023A0F08 */
BOOL daObjTide::Act_c::Execute(be<u32>* matrixOut) {
    (this->*tideModeProc[u32(m2EC)].run)();
    eyePos.y = current.pos.y;
    set_mtx();
    *matrixOut = gabi::ea(&tideTemporaryMatrix);
    if (m30A)
        tideDeleteActor(this);
    return 1;
}

// HD: background/default draw-buffer pairs are reloaded one word per game-info call.
/* 023A0FE8 */
BOOL daObjTide::Act_c::Draw() {
    tideSetLighting(tideGetEnvironment(), 2, &current.pos, &tevStr);
    dScnKy_env_light_c* environment = tideGetEnvironment();
    tideSetModelLighting(environment, mModel1.get(), &tevStr);
    if (mModel2)
        tideSetModelLighting(tideGetEnvironment(), mModel2.get(), &tevStr);
    mDoExt_baseAnm* animation = mBck1;
    if (animation)
        tideEntryBck(animation, mModel1->mModelData.get(), animation->mFrameCtrl.mFrame);
    animation = mBrk1;
    if (animation)
        tideEntryBrk(animation, mModel1->mModelData.get(), animation->mFrameCtrl.mFrame);
    animation = mBtk;
    if (animation)
        tideEntryBtk(animation, mModel1->mModelData.get(), animation->mFrameCtrl.mFrame);
    animation = mBrk2;
    if (animation && mModel2)
        tideEntryBrk(animation, mModel2->mModelData.get(), animation->mFrameCtrl.mFrame);
    tideDrawBuffers.mDrawBuffer[0] =
        reinterpret_cast<tideHD::DrawLists*>(tideGetGameInfo())->mListBG[0];
    tideDrawBuffers.mDrawBuffer[1] =
        reinterpret_cast<tideHD::DrawLists*>(tideGetGameInfo())->mListBG[1];
    tideUpdateModel(mModel1.get(), 0);
    tideDrawBuffers.mDrawBuffer[0] =
        reinterpret_cast<tideHD::DrawLists*>(tideGetGameInfo())->mList[0];
    tideDrawBuffers.mDrawBuffer[1] =
        reinterpret_cast<tideHD::DrawLists*>(tideGetGameInfo())->mList[1];
    if (mModel2)
        tideUpdateModel(mModel2.get(), 0);
    return 1;
}

/* 023A2004 */
static s32 Mthd_Create(Tide* tide) {
    return tide->Mthd_Create();
}

/* 023A2008 */
static s32 Mthd_Delete(Tide* tide) {
    return tide->Mthd_Delete();
}

/* 023A200C */
static s32 Mthd_Execute(Tide* tide) {
    return tideExecuteMoveBg(tide);
}

/* 023A2010 */
static s32 Mthd_Draw(Tide* tide) {
    return gabi::call_ptr<s32>(reinterpret_cast<tideHD::ActorVtableView*>(tide)->mVtable->Draw,
                               tide);
}

/* 023A2020 */
static s32 Mthd_IsDelete(Tide* tide) {
    return gabi::call_ptr<s32>(reinterpret_cast<tideHD::ActorVtableView*>(tide)->mVtable->IsDelete,
                               tide);
}

// HD: compiler-generated and template code of this translation unit.
/* 023A2030 */
void __sinit_d_a_obj_tide_cpp() {
    for (u32 i = 0; i < 4; i++)
        tideHeaderZeros.words[i] = 0;
    tideRegisterDestructor(&tideRecord0);
    tideNegativePi = tidePiNegative;
    tidePositivePi = tidePiPositive;
    tideConstructAngleTag(&tideAngleTag);
    tideRegisterDestructor(&tideRecord1);
    tideConstructByteTag(&tideByteTag);
    tideRegisterDestructor(&tideRecord2);
}

/* 023A20C4 */
void tideHioDestructor(void* object, s32 flags) {
    if (object && (flags & 1))
        tideFree(object);
}

/* 023A20D8 */
BOOL tideMoveBGIsDelete(Tide* tide) {
    return 1;
}

/* 023A20E0 */
void tideHioGenMessage(Tide* tide) {}

/* 023A20E4 */
void tideActorDestructor(Tide* tide, s32 flags) {
    if (tide) {
        tideDestroyActor(tide, 0);
        if (flags & 1)
            tideFree(tide);
    }
}

/* 023A2138 */
u32 tidePrmAbstract(Tide* tide, u32 bits, u32 shift) {
    u32 mask = (bits & 32) ? 0u : 1u << (bits & 31),
        value = (shift & 32) ? 0u : tide->mParameters >> (shift & 31);
    return value & (mask - 1);
}
/* WWHD glue
ext 025200D4 dComIfG_inf_c* tideGetGameInfo()
ext 026066C4 void* tideGetObjectResource(void*,void*,s32)
ext 0273AA24 void tideAssert(const char*,s32,const char*)
ext 025B9100 s32 tideIsDungeonItem(void*,s32)
ext 024F1D40 void tideConstructMoveBg(void*)
ext 025B8B7C void tideClearTmpBit(void*,s32)
ext 02520460 s32 tideLoadResource(void*,const char*)
ext 024F1D9C s32 tideCreateMoveBg(void*,const char*,s32,u32,u32)
ext 024F1F64 s32 tideDeleteMoveBg(void*)
ext 025204C8 void tideDeleteResource(void*,const char*)
ext 025E38E0 J3DModel* tideCreateModel(void*,s32,u32)
ext 0273AD10 void* tideAllocate(u32)
ext 027F2BC0 void tideInitFrame(void*,s32)
ext 027DA984 void tideConstructMaya(void*)
ext 025E8508 s32 tideInitBck(void*,void*,void*,s32,s32,s32,s32,s32,f32)
ext 025E80D0 mDoExt_brkAnm* tideConstructBrk(void*)
ext 025E8154 s32 tideInitBrk(void*,void*,void*,s32,s32,s32,s32,s32,s32,f32)
ext 025E7C6C mDoExt_btkAnm* tideConstructBtk(void*)
ext 025E7CE0 s32 tideInitBtk(void*,void*,void*,s32,s32,s32,s32,s32,s32,f32)
ext 025F1B48 void tideRotateZXY(void*,s32,s32,s32)
ext 028E90D4 void tideCopyMatrix(void*,void*)
ext 025D674C void tideSetCullBox(void*,f32,f32,f32,f32,f32,f32)
ext 025D57E0 void tideDeleteActor(void*)
ext 025626A4 void tideSetLighting(void*,s32,void*,void*)
ext 02555D0C dScnKy_env_light_c* tideGetEnvironment()
ext 02562F5C void tideSetModelLighting(void*,void*,void*)
ext 025E86B8 void tideEntryBck(void*,void*,f32)
ext 025E83FC void tideEntryBrk(void*,void*,f32)
ext 025E7FC4 void tideEntryBtk(void*,void*,f32)
ext 025E2DE0 void tideUpdateModel(void*,s32)
ext 025DAC50 tideHD::EnvironmentActor* tideFindEnvironmentActor(s32)
ext 025DAD48 void tideDeleteEnvironmentActor(void*)
ext 025E1B34 void tideDeleteSound(void*)
ext 0200ECD4 void tideAddCalc(void*,f32,f32,f32,f32)
ext 025E1988 void tideStartSound(s32)
ext 025445B8 s32 tideCheckEventStart(void*,const char*)
ext 02526E70 tideHD::DemoActor* tideGetDemoActor(void*,u32)
ext 025E742C s32 tidePlayAnimation(void*)
ext 020087EC void tideReleaseBackground(void*,void*)
ext 025DADA4 s32 tideCreateEnvironmentActor(s32,s32,void*,s32,s32)
ext 02520540 s32 tideGetReverb(s32)
ext 025E1A40 void tideStartPositionSound(s32,void*,s32,s32)
ext 025BA0C0 s32 tideIsSwitch(void*,u32,s32)
ext 025B8B94 s32 tideIsTmpBit(void*,s32)
ext 025A847C void tideSetParticle(void*,s32,s32,void*,s32,s32,s32,s32,s32,s32,s32,s32)
ext 024EEA6C void tideRegisterBackground(void*,void*,void*)
ext 027F2BF8 s32 tideCheckFrame(void*,f32)
ext 024F1E9C s32 tideExecuteMoveBg(void*)
ext 028F026C void tideRegisterDestructor(void*)
ext 028ED6F8 void tideConstructAngleTag(void*)
ext 028EAB2C void tideConstructByteTag(void*)
ext 0273AF40 void tideFree(void*)
ext 025D50BC void tideDestroyActor(void*,s32)
data tideModeProc 10030E74
data tideAttributes 10030BBC
data tideResources 101F4F28
data tideSave 101F84DC
data tideDemoManager 101D5FFC
data tideWaterLevel 101D5F28
data tideWaterState 101D5F2C
data tideTemporaryMatrix 1046C4BC
data tideDrawBuffers 104B4634
data tideHeaderZeros 1046C4AC
data tideNegativePi 1046C4A0
data tidePositivePi 1046C4A4
data tidePiNegative 10030FCC
data tidePiPositive 10030FD0
data tideAngleTag 1046C4A8
data tideByteTag 1046C4A9
data tideRecord0 101CD23C
data tideRecord1 101CD248
data tideRecord2 101CD254
str 10030BA0 in 023A13F4
str 10030BB0 in 023A13F4
str 10030D78 in 023A0484
str 10030D8C in 023A0484
str 10030E3C in 023A0D58
str 10030E50 in 023A0D58
str 10030E9C in 023A1398
str 10030EA4 in 023A1644
str 10030EB8 in 023A1644
str 10030EDC in 023A16AC
str 10030EF0 in 023A16AC
str 10030F14 in 023A182C
str 10030F28 in 023A182C
str 10030F50 in 023A1894
str 10030F74 in 023A1894
str 10030F88 in 023A18FC
str 10030F9C in 023A18FC
str 10030DD0
str 10030E04
str 10030E10
str 10030E1C
str 10030E28
str 10030DE0
str 10030DF0
str 10030B50
str 10030B58
*/
