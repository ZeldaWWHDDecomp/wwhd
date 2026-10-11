/** Yellow floor switch. WWHD port; */
#include "d/actor/d_a_obj_swpush.h"
#include "gc_api/f_op/f_op_actor.h"
using namespace daObjSwpush;
void swpushOrderPotentialEvent(void*, s32, s32, s32);
s32 swpushReleaseBackground(void*, void*);
void swpushDestroyActor(void*, s32);
void swpushRegisterDestructor(void*);
void swpushConstructAngleTag(void*);
void swpushConstructByteTag(void*);
void swpushEntryBtp(void*, void*, s16);
void swpushCopyBackVertices(void*);
void swpushMoveBackground(void*);
s32 swpushInitBtp(void*, void*, void*, s32, s32, s32, s32, s32, s32, f32);
void* swpushConstructBackground(u32);
s32 swpushSetBackground(void*, void*, s32);
void swpushConstructBtp(void*);
s32 swpushEnterHeap(void*, u32, u32);
s32 swpushRegisterBackground(void*, void*, void*);
void swpushTransformVertices(void*);
s16 swpushGetEventIndex(void*, u32, u8);
void* swpushGetJoint(void*);
void swpushTranslate(f32, f32, f32);
void swpushConstructTriangle(void*);
void swpushPushMatrix();
void swpushRotateY(void*, s16);
void swpushTransformVector(void*, void*, void*);
void swpushPopMatrix();
void swpushSetTriangle(void*, void*, void*, void*);
s32 swpushTriangleCrossY(void*, void*);

static daObjSwpush::Attr_c swpushAttributes[4] = {
    {2784,
     3,
     1.0f,
     "Kbota_00",
     0,
     "Kbota_00",
     "Kbota_00",
     0,
     7,
     {4, 4},
     -1,
     0.8999999761581421f,
     0.6000000238418579f,
     0.0f,
     -0.30000001192092896f,
     1,
     6,
     6,
     13,
     1.0f,
     2.5f,
     10,
     {0, 0}},
    {2784,
     32,
     1.0f,
     "Kbota_00",
     0,
     "Kbota_00",
     "Kbota_00",
     0,
     7,
     {4, 4},
     -1,
     0.8999999761581421f,
     0.6000000238418579f,
     0.0f,
     -0.30000001192092896f,
     1,
     6,
     6,
     13,
     1.0f,
     2.5f,
     10,
     {0, 0}},
    {2784,
     19,
     1.0f,
     "Kbota_00",
     0,
     "Kbota_00",
     "Kbota_00",
     0,
     7,
     {4, 4},
     -1,
     0.8999999761581421f,
     0.6000000238418579f,
     0.0f,
     -0.30000001192092896f,
     1,
     6,
     6,
     13,
     1.0f,
     2.5f,
     10,
     {0, 0}},
    {32768,
     11,
     1.5f,
     "Kbota_00",
     "Hhbot",
     "Kbota_00",
     "Hhbot",
     0,
     7,
     {4, 4},
     -1,
     0.9300000071525574f,
     0.41999998688697815f,
     0.0f,
     0.0f,
     4,
     4,
     15,
     4,
     1.0f,
     2.5f,
     10,
     {0, 0}},
};
static SwpushProcEntry swpushDemoProc[5] = {
    {&daObjSwpush::Act_c::demo_non},      {&daObjSwpush::Act_c::demo_reqPause},
    {&daObjSwpush::Act_c::demo_runPause}, {&daObjSwpush::Act_c::demo_reqSw},
    {&daObjSwpush::Act_c::demo_runSw},
};
static SwpushProcEntry swpushModeProc[4] = {
    {&daObjSwpush::Act_c::mode_upper},
    {&daObjSwpush::Act_c::mode_u_l},
    {&daObjSwpush::Act_c::mode_lower},
    {&daObjSwpush::Act_c::mode_l_u},
};
static u8 swpushOpVertices[4] = {6, 10, 11, 7};
static s8 swpushTriangleIds[2][3] = {{0, 1, 2}, {3, 0, 2}};
static swpushHD::SaveBlock* swpushSave;
static J3DModel* swpushCurrentModel;
static Mtx34 swpushCurrentMatrix;
static swpushHD::RideVectors swpushNoPushVectors;
static u32 swpushNoPushGuard;
static swpushHD::HeaderZeros swpushHeaderZeros;
static f32 swpushNegativePi;
static f32 swpushPositivePi;
static u8 swpushAngleTag;
static u8 swpushByteTag;
static swpushHD::DestructorRecord swpushRecord0;
static swpushHD::DestructorRecord swpushRecord1;
static swpushHD::DestructorRecord swpushRecord2;
using SwpushSwitchCall = void (*)(void*, s32, s32);
void swpushOffSwitch(void*, s32, s32);
void swpushOnSwitch(void*, s32, s32);
void swpushReverseSwitch(void*, s32, s32);
dComIfG_inf_c* swpushGetGameInfo();
static BOOL jnodeCB(void*, s32);
static bool solidHeapCB(Act_c*);
static void rideCB(void*, Act_c*, fopAc_ac_c*);

// HD: Assert on the type before every attribute access; HD calls it M_type.
daObjSwpush::Attr_c& daObjSwpush::Act_c::attr(const char* file, const char* msg) {
    if (u32(mType) >= 4)
        JUT_ASSERT_fail(file, 0x1BA, msg);
    return swpushAttributes[u32(mType)];
}
static Mtx34* swpushAnimationMatrix(J3DModel* model, u32 index) {
    swpushHD::MatrixBuffer* block =
        reinterpret_cast<swpushHD::MatrixBuffer*>(model->mpMtxBuffer.get());
    Mtx34* matrices = block->mpAnmMtx;
    block->mFlags = u16(block->mFlags) | 16;
    return matrices + index;
}
/* 02399090 */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    u32 w = (u32)width & 63, s = (u32)shift & 63;
    return (s < 32 ? (u32)a->mParameters >> s : 0) & ((w < 32 ? 1u << w : 0) - 1);
}
static bool is_switch(Act_c* a) {
    s32 sw = PrmAbstract(a, Act_c::PRM_SWSAVE_W, Act_c::PRM_SWSAVE_S);
    return fopAcM_isSwitch(a, sw) != 0;
}
static void change_switch(Act_c* a, SwpushSwitchCall fn) {
    s32 sw = PrmAbstract(a, Act_c::PRM_SWSAVE_W, Act_c::PRM_SWSAVE_S);
    swpushHD::SaveBlock* save = swpushSave;
    s32 room = a->home.roomNo;
    fn(&save->mSaveInfo, sw, room);
}
/* 02396B98 */
void daObjSwpush::Act_c::prmZ_init() {
    if (mPrmZInit)
        return;
    s16 z = home.angle.z;
    mPrmZInit = 1;
    home.angle.z = 0;
    mPrmZ = z;
    current.angle.z = 0;
    shape_angle.z = 0;
}
/* 02396B14 */
bool daObjSwpush::Act_c::is_switch2() {
    s32 ver = PrmAbstract(this, Act_c::PRM_VERSION_W, Act_c::PRM_VERSION_S);
    s32 sw = ver > 0 ? (u8)mPrmZ : 255;
    if (sw == 255)
        return false;
    return fopAcM_isSwitch(this, sw) != 0;
}
/* 023973C8 */
void daObjSwpush::Act_c::set_mtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
/* 0239748C */
void daObjSwpush::Act_c::init_mtx() {
    J3DModel* model = mpModel;
    f32 y = scale.y, x = scale.x, z = scale.z;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    set_mtx();
}
/* 023974AC */
void daObjSwpush::Act_c::set_btp_frame() {
    if (attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mBtpArcName) {
        bool sw = is_switch(this);
        mBtpAnm.mFrameCtrl.mFrame = sw ? 1.f : 0.f;
    }
}
/* 0239833C */
void daObjSwpush::Act_c::top_bg_aim_req(f32 h, s16 timer) {
    m328 = h;
    m324 = timer;
}
/* 02397564 */
void daObjSwpush::Act_c::mode_upper_init() {
    mDebounceTimer = 0;
    mTargetHFrac = 1.0f;
    mMode = MODE_UPPER;
    mChangingState = 0;
}
/* 02397584 */
void daObjSwpush::Act_c::mode_lower_init() {
    mChangingState = 0;
    mMode = MODE_LOWER;
    mTargetHFrac = 0.0f;
}
/* 02398348 */
void daObjSwpush::Act_c::mode_u_l_init() {
    mMode = MODE_U_L;
    mTargetHFrac = 0.0f;
    mSpeed = attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mPushSpeed0;
    s16 delay = attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").m38;
    top_bg_aim_req(0.0f, delay);
}
/* 02398A98 */
void daObjSwpush::Act_c::mode_l_u_init() {
    mMode = MODE_L_U;
    mTargetHFrac = 1.0f;
    top_bg_aim_req(1.0f, 1);
}
/* 023975A4 */
void daObjSwpush::Act_c::demo_non_init() {
    mDemoMode = DEMO_MODE_NON;
}
// HD: An additional no-op function is emitted beside demo_non.
/* 0239902C */
static void demoNonThunk(void* p) {}
/* 02398E80 */
void daObjSwpush::Act_c::demo_runSw_init() {
    mDemoMode = DEMO_MODE_RUN_SW;
}
/* 0239842C */
void daObjSwpush::Act_c::demo_runPause_init() {
    mDemoMode = DEMO_MODE_RUN_PAUSE;
    mPauseTimer = attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mPauseDuration;
}
/* 02398498 */
void daObjSwpush::Act_c::demo_reqPause() {
    if (eventInfo_checkCommandDemoAccrpt(this))
        demo_runPause_init();
    else
        demo_non_init();
}
/* 023985AC */
void daObjSwpush::Act_c::demo_reqPause_init() {
    if (mDemoMode == DEMO_MODE_NON) {
        mDemoMode = DEMO_MODE_REQ_PAUSE;
        swpushOrderPotentialEvent(this, 2, 0, 0);
        eventInfo_onCondition(this, dEvtCnd_UNK2_e);
    }
}
/* 023984AC */
void daObjSwpush::Act_c::demo_stop_puase() {
    if (mDemoMode == DEMO_MODE_REQ_PAUSE)
        demo_reqPause();
    if (mDemoMode == DEMO_MODE_RUN_PAUSE)
        dComIfGp_event_reset();
}
/* 02398E2C */
void daObjSwpush::Act_c::demo_runPause() {
    s16 timer = (s16)((s16)mPauseTimer - 1);
    mPauseTimer = timer;
    if (timer <= 0) {
        dComIfGp_event_reset();
        demo_non_init();
    }
}
/* 02398504 */
void daObjSwpush::Act_c::demo_reqSw_init() {
    if (dComIfGp_evmng_existence(mEventID) && (u32)mDemoMode <= 2) {
        demo_stop_puase();
        mDemoMode = DEMO_MODE_REQ_SW;
        u8 ev = PrmAbstract(this, Act_c::PRM_EVID_W, Act_c::PRM_EVID_S);
        fopAcM_orderOtherEventId(this, mEventID, ev, 0xffff, 0, 1);
        eventInfo_onCondition(this, dEvtCnd_UNK2_e);
    }
}
/* 02398E8C */
void daObjSwpush::Act_c::demo_reqSw() {
    if (eventInfo_checkCommandDemoAccrpt(this))
        demo_runSw_init();
    else {
        u8 ev = PrmAbstract(this, Act_c::PRM_EVID_W, Act_c::PRM_EVID_S);
        fopAcM_orderOtherEventId(this, mEventID, ev, 0xffff, 0, 1);
        eventInfo_onCondition(this, dEvtCnd_UNK2_e);
    }
}
/* 02398F10 */
void daObjSwpush::Act_c::demo_runSw() {
    if (dComIfGp_evmng_endCheck(mEventID)) {
        dComIfGp_event_reset();
        demo_non_init();
    }
}
/* 02398608 */
void daObjSwpush::Act_c::mode_upper() {
    bool pause = false, pressing = false;
    f32 height = 1.0f;
    if (mMiniPushFlg) {
        if ((u32)attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mFlags &
            FLAG_REQ_HEAVY) {
            if (mPushFlg)
                pressing = true;
            else
                height = 0.9f;
        } else
            pressing = true;
    }
    auto& a = attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)");
    if (mDebounceTimer <= 0) {
        if (!((u32)a.mFlags & FLAG_REQ_HEAVY) && mRidingMode && !mPrevRiding) {
            mSpeed = attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").m34;
            mDebounceTimer = 30;
            pause = true;
        }
    } else
        mDebounceTimer = (s16)((s16)mDebounceTimer - 1);
    bool sw = is_switch(this), lower = pressing;
    if (!pressing && ((u32)attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mFlags &
                      FLAG_OBEY_SAVE)) {
        bool onUp = (u32)attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mFlags &
                    FLAG_ON_IS_UP;
        lower = sw ? !onUp : onUp;
    }
    if (lower) {
        if (pressing)
            mChangingState = 1;
        mode_u_l_init();
        demo_reqSw_init();
    } else {
        mTargetHFrac = height;
        top_bg_aim_req(height, 1);
        if (pause)
            demo_reqPause_init();
    }
}
/* 02398AB4 */
void daObjSwpush::Act_c::mode_lower() {
    bool pressing = false;
    if (mMiniPushFlg)
        pressing = !((u32)attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mFlags &
                     FLAG_REQ_HEAVY) ||
                   mPushFlg;
    bool sw = is_switch(this);
    bool sw2 = is_switch2();
    bool obey = (u32)attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mFlags &
                FLAG_OBEY_SAVE;
    bool stay = (u32)attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mFlags &
                FLAG_STAY_PRESSED;
    bool toggle = (u32)attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mFlags &
                  FLAG_IS_TOGGLE;
    bool onUp = (u32)attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mFlags &
                FLAG_ON_IS_UP;
    bool lock =
        (u32)attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mFlags & FLAG_UNK20;
    bool pop = !stay && !pressing, match = obey && (sw ? onUp : !onUp) && !pressing;
    if ((pop || match) && !(lock && sw2)) {
        if (pop && !toggle && !pressing)
            mChangingState = 1;
        demo_reqSw_init();
        mode_l_u_init();
    }
}
/* 023988E8 */
void daObjSwpush::Act_c::mode_u_l() {
    if (!(mCurHFrac > 0.0f)) {
        if (mChangingState) {
            if ((u32)attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mFlags &
                FLAG_IS_TOGGLE)
                change_switch(this, swpushReverseSwitch);
            else {
                bool onUp =
                    (u32)attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mFlags &
                    FLAG_ON_IS_UP;
                change_switch(this, onUp ? swpushOffSwitch : swpushOnSwitch);
            }
            if (mVibTimer) {
                gabi::Local<cXyz> dir;
                dir->set(0.0f, 1.0f, 0.0f);
                dComIfGp_getVibration_StartShock(4, -0x21, dir);
            }
        }
        fopAcM_seStart(this, 0x6930, 0);
        mode_lower_init();
    }
}
/* 02398D54 */
void daObjSwpush::Act_c::mode_l_u() {
    if (!(mCurHFrac < 1.0f)) {
        if (mChangingState) {
            bool onUp =
                (u32)attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mFlags &
                FLAG_ON_IS_UP;
            change_switch(this, onUp ? swpushOnSwitch : swpushOffSwitch);
        }
        mode_upper_init();
    }
}
/* 02397E70 */
void daObjSwpush::Act_c::calc_top_pos() {
    f32 diff = (f32)mCurHFrac - (f32)mTargetHFrac;
    f32 decay = attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mSpeedDecay;
    f32 spring = attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mSpring;
    f32 v = gabi::fnmsubs(diff, spring, mSpeed);
    v = gabi::fnmsubs(v, decay, v);
    f32 h = (f32)mCurHFrac + v;
    mCurHFrac = h;
    mSpeed = v;
    f32 y = (1.0f - h) * -35.5f;
    if (y < -36.5f)
        y = -36.5f;
    else
        y = (y - 1.0f) >= 0 ? 1.0f : y;
    m31C = y;
    if (m324 > 0) {
        s16 timer = (s16)((s16)m324 - 1);
        m324 = timer;
        if (timer == 0)
            m320 = (f32)m328;
    }
    f32 top = mMode == MODE_UPPER ? (f32)mCurHFrac : (f32)m320;
    m32C = top;
    f32 ty = (1.0f - top) * -35.5f;
    mTopPos = ty;
    if (ty < (f32)m31C)
        mTopPos = (f32)m31C;
}
/* 02397BFC */
void daObjSwpush::Act_c::set_push_flag() {
    if (mVibTimer)
        mVibTimer = (u8)((u8)mVibTimer - 1);
    if (mMiniPushFlg) {
        if (mRidingMode) {
            if (mRidingMode == 2)
                mMiniPushTimer = attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)")
                                     .mMiniPushDelay2;
            else
                mMiniPushTimer = attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)")
                                     .mMiniPushDelay1;
        } else {
            s16 t = (s16)((s16)mMiniPushTimer - 1);
            mMiniPushTimer = t;
            if (t <= 0)
                mMiniPushFlg = 0;
        }
    } else {
        if (mRidingMode) {
            auto& a = attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)");
            s16 t = (s16)((s16)mMiniPushTimer + 1);
            mMiniPushTimer = t;
            if (t >= (s16)a.mMiniPushDelay1)
                mMiniPushFlg = 1;
        } else
            mMiniPushTimer = 0;
    }
    if (mPushFlg) {
        if (mHeavyRiding)
            mPushTimer =
                attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mPushDelay;
        else {
            s16 t = (s16)((s16)mPushTimer - 1);
            mPushTimer = t;
            if (t <= 0)
                mPushFlg = 0;
        }
    } else {
        if (mHeavyRiding) {
            auto& a = attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)");
            s16 t = (s16)((s16)mPushTimer + 1);
            mPushTimer = t;
            if (t >= (s16)a.mPushDelay)
                mPushFlg = 1;
        } else
            mPushTimer = 0;
    }
}
/* 023969BC */
s32 daObjSwpush::Act_c::create_res_load() {
    if (attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mKbotaResName) {
        const char* name =
            attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mKbotaResName.get();
        s32 result = dComIfG_resLoad(&mKbotaPhs, name);
        if (result != cPhs_COMPLEATE_e)
            return result;
    }
    if (attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mHhbotResName) {
        const char* name =
            attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mHhbotResName.get();
        s32 result = dComIfG_resLoad(&mHhbotPhs, name);
        if (result != cPhs_COMPLEATE_e)
            return result;
    }
    return cPhs_COMPLEATE_e;
}
/* 02397AD0 */
s32 daObjSwpush::Act_c::Mthd_Delete() {
    dComIfG_inf_c* play = swpushGetGameInfo();
    swpushHD::Background* bg = mpBgW;
    swpushReleaseBackground(&play->play.mBgS, bg);
    if (attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mKbotaResName) {
        const char* name =
            attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mKbotaResName.get();
        dComIfG_resDelete(&mKbotaPhs, name);
    }
    if (attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mHhbotResName) {
        const char* name =
            attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mHhbotResName.get();
        dComIfG_resDelete(&mHhbotPhs, name);
    }
    return TRUE;
}
/* 02398F78 */
static BOOL Delete_wrapper(Act_c* a) {
    return a->Mthd_Delete();
}
/* 02399018 */
static void swpushHioDestructor(void* p, s32 flags) {
    if (p && (flags & 1))
        operator_delete(p);
}
/* 02399034 */
static void swpushActorDestructor(void* p, s32 flags) {
    if (p) {
        swpushDestroyActor(p, 0);
        if (flags & 1)
            operator_delete(p);
    }
}
/* 02399030 */
void daObjSwpush::Act_c::demo_non() {}
/* 02399088 */
static BOOL isDelete(void* p) {
    return TRUE;
}
/* 02398F84 */
static void __sinit_d_a_obj_swpush_cpp() {
    swpushHeaderZeros.words[2] = 0;
    swpushHeaderZeros.words[0] = 0;
    swpushHeaderZeros.words[3] = 0;
    swpushHeaderZeros.words[1] = 0;
    swpushRegisterDestructor(&swpushRecord0);
    swpushNegativePi = -3.1415927410125732f;
    swpushPositivePi = 3.1415927410125732f;
    swpushConstructAngleTag(&swpushAngleTag);
    swpushRegisterDestructor(&swpushRecord1);
    swpushConstructByteTag(&swpushByteTag);
    swpushRegisterDestructor(&swpushRecord2);
}
/* 02398240 */
s32 daObjSwpush::Act_c::Mthd_Draw() {
    settingTevStruct(dKy_getEnvlight(), 1, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    if (attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mBtpArcName) {
        s16 frame = (s16)gabi::ftoi(mBtpAnm.mFrameCtrl.mFrame);
        J3DModelData* data = mpModel->mModelData;
        swpushEntryBtp(&mBtpAnm, data, frame);
    }
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}
/* 02398F80 */
static s32 Draw_wrapper(Act_c* a) {
    return a->Mthd_Draw();
}
/* 02398014 */
s32 daObjSwpush::Act_c::Mthd_Execute() {
    (this->*swpushDemoProc[u32(mDemoMode)].run)();
    set_push_flag();
    (this->*swpushModeProc[u32(mMode)].run)();
    calc_top_pos();
    u8 riding = mRidingMode;
    u8 heavy = mHeavyRiding;
    mHeavyRiding = 0;
    mRidingMode = 0;
    mPrevHeavyRiding = heavy;
    mPrevRiding = riding != 0;
    set_mtx();
    swpushCopyBackVertices(mpBgW.get());
    swpushHD::Background* bg = mpBgW;
    swpushHD::BackgroundData* countPtr = bg->pm_bgd;
    cXyz* vertices = bg->pm_vtx_tbl;
    s32 count = countPtr->m_v_num;
    for (u32 i = 0; i < 4; i++) {
        u32 vertex = swpushOpVertices[i];
        if ((s32)vertex >= count)
            JUT_ASSERT_fail("d_a_obj_swpush.cpp", 0x57B, "M_op_vtx[i] < i_max");
        f32 y = (f32)m2D4 + (f32)mTopPos;
        vertices[vertex].y = y;
    }
    swpushMoveBackground(mpBgW.get());
    f32 y = (f32)current.pos.y + (f32)m31C - -35.5f;
    f32 x = current.pos.x, z = current.pos.z;
    eyePos.x = x;
    eyePos.z = z;
    eyePos.y = y;
    set_btp_frame();
    return TRUE;
}
/* 02398F7C */
static s32 Execute_wrapper(Act_c* a) {
    return a->Mthd_Execute();
}
/* 02396CF0 */
bool daObjSwpush::Act_c::create_heap() {
    u32 modelIndex = PrmAbstract(this, Act_c::PRM_MDL_W, Act_c::PRM_MDL_S);
    if (modelIndex >= 2)
        JUT_ASSERT_fail("d_a_obj_swpush.cpp", 0x203, "(mdl_no >= 0) && (mdl_no < Mdl_Max)");
    auto& modelAttr = attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)");
    s16 resource = modelAttr.mModelResIndices[modelIndex];
    const char* name =
        attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mModelArcName.get();
    auto* data = (J3DModelData*)dComIfG_getObjectRes(name, resource, 0x1002FF84);
    if (!data)
        JUT_ASSERT_fail("d_a_obj_swpush.cpp", 0x209, "model_data != 0");
    u32 flags = attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mBtpArcName
                    ? 0x11020022
                    : 0x11000022;
    J3DModel* model = mDoExt_J3DModel__create(data, 0x80000, flags);
    mpModel = model;
    if (model) {
        u32 count = data->mJointNum;
        swpushHD::Joint* joint = reinterpret_cast<swpushHD::ModelData*>(data)->mJointNodes;
        if (count > 1)
            ++joint;
        joint->mCallBack = gabi::guest_fn(reinterpret_cast<const void*>(jnodeCB));
        mpModel->mUserArea = gabi::ea(this);
    }
    s32 btp = 1;
    if (attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mBtpArcName) {
        const char* btpName =
            attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mBtpArcName.get();
        s16 index = attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mBtpResIndex;
        void* animation = dComIfG_getObjectRes(btpName, index, 0x1002FF84);
        if (!animation)
            JUT_ASSERT_fail("d_a_obj_swpush.cpp", 0x222, "btp_data != 0");
        btp = swpushInitBtp(&mBtpAnm, data, animation, 1, 0, 0, -1, 0, 0, 1.0f);
    }
    const char* bgName =
        attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mBgArcName.get();
    s16 bgIndex = attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mBgResIndex;
    void* bgData = dComIfG_getObjectRes(bgName, bgIndex, 0x1002FF84);
    if (!bgData)
        JUT_ASSERT_fail("d_a_obj_swpush.cpp", 0x22F, "bg_data != 0");
    swpushHD::Background* bg =
        static_cast<swpushHD::Background*>(swpushConstructBackground((u32)0));
    mpBgW = bg;
    bool bgOk = false;
    if (bg)
        bgOk = swpushSetBackground(bg, bgData, 0) == 0;
    bool result = mpModel != nullptr && btp && mpBgW.v && bgOk;
    if (!result)
        mpBgW = 0;
    return result;
}
/* 02397058 */
static bool solidHeapCB(Act_c* a) {
    return a->create_heap();
}
/* 023975B0 */
s32 daObjSwpush::Act_c::Mthd_Create() {
    if (!((u32)actor_condition & 8)) {
        if (gabi::ea(this)) {
            fopAc_ac_c_ct(this);
            __vtbl = 0x1002FF9C;
            swpushConstructBtp(&mBtpAnm);
        }
        actor_condition = (u32)actor_condition | 8;
    }
    prmZ_init();
    mType = PrmAbstract(this, Act_c::PRM_TYPE_W, Act_c::PRM_TYPE_S);
    s32 phase = create_res_load();
    if (phase != 4)
        return phase;
    f32 scaleX = attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mScale;
    scale.x = (f32)scale.x * scaleX;
    f32 scaleZ = attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mScale;
    scale.z = (f32)scale.z * scaleZ;
    u32 heap = attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mHeapSize;
    if (!swpushEnterHeap(this, gabi::guest_fn(reinterpret_cast<const void*>(solidHeapCB)), heap))
        return 5;
    dComIfG_inf_c* play = swpushGetGameInfo();
    swpushHD::Background* bg = mpBgW;
    if (swpushRegisterBackground(&play->play.mBgS, bg, this))
        return 5;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    mDoMtx_stack_c::scaleM(scale.x, scale.y, scale.z);
    PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
    bg = mpBgW;
    bg->m_flag = u8(bg->m_flag) & 0xFD;
    bg = mpBgW;
    bg->pm_base = &mMtx;
    swpushTransformVertices(mpBgW.get());
    bg = mpBgW;
    bg->pm_base = nullptr;
    bg = mpBgW;
    bg->m_flag = u8(bg->m_flag) | 2;
    bg = mpBgW;
    cXyz* vertices = bg->pm_vtx_tbl;
    m2D4 = vertices[6].y;
    bg->mRideCallback = gabi::guest_fn(reinterpret_cast<const void*>(rideCB));
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel));
    init_mtx();
    auto* a = &attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)");
    auto* b = &attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)");
    auto* c = &attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)");
    auto* d = &attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)");
    f32 x0 = -60 * (f32)a->mScale, z0 = -60 * (f32)b->mScale, x1 = 60 * (f32)c->mScale,
        z1 = 60 * (f32)d->mScale;
    fopAcM_setCullSizeBox(this, x0, -2, z0, x1, 39, z1);
    mPushTimer = 0;
    mHeavyRiding = 0;
    mPrevRiding = 0;
    mSpeed = 0;
    mMiniPushTimer = 0;
    mPrevHeavyRiding = 0;
    m324 = 0;
    mPushFlg = 0;
    mMiniPushFlg = 0;
    mVibTimer = 0;
    mRidingMode = 0;
    m328 = 0;
    set_btp_frame();
    bool toggle = (u32)attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mFlags &
                  FLAG_IS_TOGGLE;
    bool onUp = (u32)attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mFlags &
                FLAG_ON_IS_UP;
    bool second =
        (u32)attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)").mFlags & FLAG_UNK20;
    bool on = is_switch(this);
    bool on2 = is_switch2();
    if ((toggle || (on == onUp)) && !(second && on2)) {
        mTopPos = 0;
        m31C = 0;
        mCurHFrac = 1;
        m320 = 1;
        m32C = 1;
        mTargetHFrac = 1;
        mode_upper_init();
    } else {
        mCurHFrac = 0;
        m32C = 0;
        m320 = 0;
        m31C = -35.5f;
        mTargetHFrac = 0;
        mTopPos = -35.5f;
        mode_lower_init();
    }
    u8 event = PrmAbstract(this, Act_c::PRM_EVID_W, Act_c::PRM_EVID_S);
    dComIfG_inf_c* evPlay = swpushGetGameInfo();
    mEventID = swpushGetEventIndex(&evPlay->play.mEvtManager, (u32)0, event);
    demo_non_init();
    return phase;
}
/* 02398F74 */
static s32 Create_wrapper(Act_c* a) {
    return a->Mthd_Create();
}
/* 02396BC8 */
static BOOL jnodeCB(void* node, s32 timing) {
    if (timing == 0) {
        J3DModel* model = swpushCurrentModel;
        Act_c* actor = static_cast<Act_c*>(model->getUserArea());
        swpushHD::JointData* joint = static_cast<swpushHD::JointData*>(swpushGetJoint(node));
        u16 index = joint->mJntNo;
        Mtx34* matrix = swpushAnimationMatrix(model, index);
        PSMTXCopy(matrix, mDoMtx_stack_c::get());
        swpushTranslate(actor->m31C, 0.f, 0.f);
        matrix = swpushAnimationMatrix(model, index);
        mtx_copy(matrix, mDoMtx_stack_c::get());
        PSMTXCopy(mDoMtx_stack_c::get(), &swpushCurrentMatrix);
    }
    return TRUE;
}
/* 0239705C */
static void rideCB(void* bg, Act_c* a, fopAc_ac_c* rider) {
    if (!((u32)rider->actor_status & fopAcStts_FREEZE_e))
        return;
    if (!swpushNoPushGuard) {
        swpushNoPushGuard = 1;
        const f32 xyz[12] = {-1, 0, -1, -1, 0, 1, 1, 0, 1, 1, 0, -1};
        for (u32 i = 0; i < 4; ++i) {
            swpushNoPushVectors.point[i].x = xyz[i * 3];
            swpushNoPushVectors.point[i].y = xyz[i * 3 + 1];
            swpushNoPushVectors.point[i].z = xyz[i * 3 + 2];
        }
    }
    cXyz* vertices = a->mpBgW->pm_vtx_tbl;
    gabi::Local<cM3dGTri> tri;
    swpushConstructTriangle((cM3dGTri*)tri);
    bool previous = a->mPrevRiding;
    auto& attr = a->attr("d_a_obj_swpush.cpp", "(M_type >=0) && (M_type <Type_Max)");
    f32 scale = previous ? (f32)attr.m44 : (f32)attr.m40;
    swpushPushMatrix();
    swpushRotateY(mDoMtx_stack_c::get(), (s16)a->shape_angle.y);
    mDoMtx_stack_c::scaleM(scale, scale, scale);
    gabi::Local<swpushHD::RideVectors> points;
    gabi::Local<cXyz> out;
    for (u32 i = 0; i < 4; i++) {
        u32 vertex = swpushOpVertices[i];
        cXyz* source = vertices + vertex;
        points->point[i].x = source->x;
        points->point[i].y = source->y;
        points->point[i].z = source->z;
        swpushTransformVector(mDoMtx_stack_c::get(), &swpushNoPushVectors.point[i], (cXyz*)out);
        PSVECAdd(&points->point[i], (cXyz*)out, &points->point[i]);
    }
    swpushPopMatrix();
    for (u32 i = 0; i < 2; i++) {
        s32 v0 = swpushTriangleIds[i][0], v1 = swpushTriangleIds[i][1],
            v2 = swpushTriangleIds[i][2];
        swpushSetTriangle((cM3dGTri*)tri, &points->point[v0], &points->point[v1],
                          &points->point[v2]);
        if (swpushTriangleCrossY((cM3dGTri*)tri, &rider->current.pos)) {
            a->mRidingMode = ((u32)rider->actor_status & fopAcStts_UNK8000000_e) ? 2 : 1;
            if (rider && rider->mProfName == 0xA8) {
                a->mVibTimer = 4;
                if (reinterpret_cast<swpushHD::PlayerStatus*>(rider)->mPlayerStatus & 0x02000000)
                    a->mHeavyRiding = 1;
            }
            break;
        }
    }
}
/* WWHD glue
ext 025D7B24 void swpushOrderPotentialEvent(void*,s32,s32,s32)
ext 020087EC s32 swpushReleaseBackground(void*,void*)
ext 025D50BC void swpushDestroyActor(void*,s32)
ext 028F026C void swpushRegisterDestructor(void*)
ext 028ED6F8 void swpushConstructAngleTag(void*)
ext 028EAB2C void swpushConstructByteTag(void*)
ext 025E7B3C void swpushEntryBtp(void*,void*,s16)
ext 024F5B08 void swpushCopyBackVertices(void*)
ext 024F43DC void swpushMoveBackground(void*)
ext 025E789C s32 swpushInitBtp(void*,void*,void*,s32,s32,s32,s32,s32,s32,f32)
ext 024F5A18 void* swpushConstructBackground(u32)
ext 024F5A78 s32 swpushSetBackground(void*,void*,s32)
ext 025E7820 void swpushConstructBtp(void*)
ext 025D63E8 s32 swpushEnterHeap(void*,u32,u32)
ext 024EEA6C s32 swpushRegisterBackground(void*,void*,void*)
ext 020095BC void swpushTransformVertices(void*)
ext 02543F10 s16 swpushGetEventIndex(void*,u32,u8)
ext 027F7878 void* swpushGetJoint(void*)
ext 025F24E0 void swpushTranslate(f32,f32,f32)
ext 02019040 void swpushConstructTriangle(void*)
ext 025F23EC void swpushPushMatrix()
ext 025F1884 void swpushRotateY(void*,s16)
ext 028E8F64 void swpushTransformVector(void*,void*,void*)
ext 025F2468 void swpushPopMatrix()
ext 0201924C void swpushSetTriangle(void*,void*,void*,void*)
ext 0201204C s32 swpushTriangleCrossY(void*,void*)
ext 025200D4 dComIfG_inf_c* swpushGetGameInfo()
ext 025B9F7C void swpushOffSwitch(void*,s32,s32)
ext 025B9E38 void swpushOnSwitch(void*,s32,s32)
ext 025BA20C void swpushReverseSwitch(void*,s32,s32)
data swpushAttributes 10030400
data swpushDemoProc 100301E0
data swpushModeProc 10030208
data swpushOpVertices 100303E8
data swpushTriangleIds 1002FF7C
data swpushSave 101F84DC
data swpushCurrentModel 104B462C
data swpushCurrentMatrix 104B4868
data swpushNoPushVectors 1046C05C
data swpushNoPushGuard 1046C09C
data swpushHeaderZeros 1046C08C
data swpushNegativePi 1046C050
data swpushPositivePi 1046C054
data swpushAngleTag 1046C058
data swpushByteTag 1046C059
data swpushRecord0 101CD010
data swpushRecord1 101CD01C
data swpushRecord2 101CD028
str 10030078 in 0239705C
str 1003008C in 0239705C
str 100300FC in 023975B0
str 10030110 in 023975B0
str 1003002C in 02396CF0
str 10030040 in 02396CF0
str 10030250 in 02398240
str 10030264 in 02398240
str 10030134 in 02397AD0
str 10030148 in 02397AD0
str 1002FFAC in 023969BC
str 1002FFC0 in 023969BC
str 1003016C in 02397BFC
str 10030180 in 02397BFC
str 100301A8 in 02397E70
str 100301BC in 02397E70
str 100303A4 in 02398D54
str 100303B8 in 02398D54
str 10030334 in 023988E8
str 10030348 in 023988E8
str 1003036C in 02398AB4
str 10030380 in 02398AB4
str 100302FC in 02398608
str 10030310 in 02398608
str 100302C0 in 0239842C
str 100302D4 in 0239842C
str 10030288 in 02398348
str 1003029C in 02398348
str 100300B0 in 023974AC
str 100300C4 in 023974AC
str 10030064 in 02396CF0
str 1003001C in 02396CF0
str 1002FFE8 in 02396CF0
str 1002FFF8 in 02396CF0
str 10030228 in 02398014
str 1003023C in 02398014
str 100303F4
str 100303EC
*/
