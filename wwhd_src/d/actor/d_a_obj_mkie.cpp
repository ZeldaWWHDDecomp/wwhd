/* WWHD light-sensitive breakable statue. */
#include "d/actor/d_a_obj_mkie.h"
using daObjMkie::Act_c;
static MkieGuestText M_arcname;
static MkieGuestText assertText1;
static MkieGuestText assertText2;
static MkieGuestText assertText3;
static MkieGuestText assertText4;
static MkieGuestText assertText5;
static MkieGuestText assertText6;
static MkieGuestText assertText7;
static MkieGuestText assertText8;
static MkieGuestText assertText9;
static MkieGuestText assertText10;
static MkieGuestText assertText11;
static MkieGuestText assertText12;
static MkieGuestText assertText13;
static daObjMkie::Attr_c M_attr[2] = {};
using ModeProc = gabi::ptmf<void (Act_c::*)()>;
static ModeProc mode_proc[3] = {};
static Mtx34 M_tmp_mtx;
static u32 se_break[2] = {};

static const f32 vertices[2][8][9] = {{{-30, 190, 40, -65, 50, 75, 65, 50, 75},
                                       {-30, 190, 40, 65, 50, 75, 30, 190, 40},
                                       {38, 190, 20, 75, 50, 65, 75, 50, -65},
                                       {38, 190, 20, 75, 50, -65, 38, 190, -40},
                                       {30, 190, -60, 65, 50, -75, -65, 50, -75},
                                       {30, 190, -60, -65, 50, -75, -30, 190, -60},
                                       {-38, 190, -40, -75, 50, -65, -75, 50, 65},
                                       {-38, 190, -40, -75, 50, 65, -38, 190, 20}},
                                      {{-90, 570, 170, -195, 150, 225, 195, 150, 225},
                                       {-90, 570, 170, 195, 150, 225, 90, 570, 170},
                                       {120, 570, 60, 225, 150, 195, 225, 150, -195},
                                       {120, 570, 60, 225, 150, -195, 120, 570, -120},
                                       {90, 570, -180, 195, 150, -225, -195, 150, -225},
                                       {90, 570, -180, -195, 150, -225, -90, 570, -180},
                                       {-120, 570, -120, -225, 150, -195, -225, 150, 195},
                                       {-120, 570, -120, -225, 150, 195, -120, 570, 60}}};
/* 0237167C */
// HD: parameter helper retains the full register and PowerPC shift edge behavior.
static u32 PrmAbstract(daObjMkie::Act_c *p, u32 width, u32 shift) {
    u32 parameters = p->mParameters;
    u32 mask = (width & 32) ? 0 : (1U << (width & 31));
    u32 value = (shift & 32) ? 0 : (parameters >> (shift & 31));
    return value & (mask - 1);
}
static daObjMkie::Attr_c *attribute(daObjMkie::Act_c *p, const char *file, const char *expression) {
    u32 type = p->mType;
    if (type >= 2) {
        JUT_ASSERT_fail(file, 0x183, expression);
        type = p->mType;
    }
    return &M_attr[type];
}
/* 0236FFCC */
s32 daObjMkie::Act_c::Mthd_Create() {
    s32 phaseState = cPhs_ERROR_e;
    u32 condition = actor_condition;
    if (!(condition & fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            dBgS_MoveBgActor::ct(this);
            __vtbl = 0x1002C744;
            mkie_call::brkConstruct(&mBrkVan);
            mkie_call::constructArray(&mTri[0], 8, 0x150, 0x02371548);
            mkie_call::constructArray(&mStts[0], 8, 0x3C, 0x023714E0);
            condition = actor_condition;
        }
        actor_condition = condition | fopAcCnd_INIT_e;
    }
    u32 sw = PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
    mSwitch = dComIfGs_isSwitch(sw, home.roomNo) == 0;
    if (!mSwitch)
        return phaseState;
    phaseState = dComIfG_resLoad(&mPhs, M_arcname.bytes);
    if (phaseState == cPhs_COMPLEATE_e) {
        mType = PrmAbstract(this, PRM_TYPE_W, PRM_TYPE_S);
        daObjMkie::Attr_c *a = attribute(this, assertText1.bytes, assertText9.bytes);
        f32 size = (f32)a->mScale;
        scale.y = size;
        scale.x = size;
        scale.z = size;
        phaseState = MoveBGCreate(M_arcname.bytes, dRes_INDEX_MKIEB_DZB_MKIEB_e, 0x024EE76C, 0x1B20);
        if (phaseState != cPhs_COMPLEATE_e && phaseState != cPhs_ERROR_e)
            JUT_ASSERT_fail(assertText1.bytes, 0x217, assertText2.bytes);
    }
    return phaseState;
}
/* 02370170 */
BOOL daObjMkie::Act_c::Mthd_Delete() {
    BOOL result = TRUE;
    if (mSwitch) {
        result = MoveBGDelete();
        dComIfG_resDelete(&mPhs, M_arcname.bytes);
    }
    return result;
}
/* 023701D0 */
BOOL daObjMkie::Act_c::CreateHeap() {
    auto baseData = gabi::at<J3DModelData>(
        gabi::ea(dComIfG_getObjectRes(M_arcname.bytes, dRes_INDEX_MKIEB_BDL_MKIEB_e, 0x1002C480)));
    if (!baseData)
        JUT_ASSERT_fail(assertText3.bytes, 0x194, assertText4.bytes);
    mModelBase = mDoExt_J3DModel__create(baseData, 0x80000, 0x11000022);
    auto vanishData = gabi::at<J3DModelData>(
        gabi::ea(dComIfG_getObjectRes(M_arcname.bytes, dRes_INDEX_MKIEB_BDL_YLSMF00_e, 0x1002C480)));
    if (!vanishData)
        JUT_ASSERT_fail(assertText3.bytes, 0x19D, assertText5.bytes);
    mModelVan = mDoExt_J3DModel__create(vanishData, 0x80000, 0x11000022);
    void *animation = dComIfG_getObjectRes(M_arcname.bytes, dRes_INDEX_MKIEB_BRK_YLSMF00_e, 0x1002C480);
    if (!animation)
        JUT_ASSERT_fail(assertText3.bytes, 0x1A7, assertText6.bytes);
    s32 ready = mkie_call::brkInitHD(&mBrkVan, vanishData, animation, 1, 0, 1.0f, 0, -1, 0, 0);
    if (!mModelBase || !mModelVan || !ready)
        return FALSE;
    return TRUE;
}
/* 02370338 */
void daObjMkie::Act_c::set_mtx() {
    f32 x = current.pos.x, z = current.pos.z, y = current.pos.y;
    mDoMtx_stack_c::transS(x, y, z);
    s16 rx = shape_angle.x, rz = shape_angle.z, ry = shape_angle.y;
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), rx, ry, rz);
    mtx_copy(&mModelBase->mBaseTransformMtx, mDoMtx_stack_c::get());
    mtx_copy(&mModelVan->mBaseTransformMtx, mDoMtx_stack_c::get());
    x = scale.x;
    y = scale.y;
    z = scale.z;
    mkie_call::scaleM(x, y, z);
    mkie_call::matrixCopy(mDoMtx_stack_c::get(), &M_tmp_mtx);
}
/* 02370480 */
void daObjMkie::Act_c::init_mtx() {
    // HD: snapshot each model scale before the corresponding stores.
    J3DModel *m = mModelBase;
    f32 y = scale.y, x = scale.x, z = scale.z;
    m->mBaseScale.x = x;
    m->mBaseScale.y = y;
    m->mBaseScale.z = z;
    m = mModelVan;
    x = scale.x;
    z = scale.z;
    y = scale.y;
    m->mBaseScale.z = z;
    m->mBaseScale.x = x;
    m->mBaseScale.y = y;
    set_mtx();
}
/* 023704BC */
void daObjMkie::Act_c::set_cc_pos() {
    gabi::Local<cXyz[3]> source, destination;
    for (u32 i = 0; i < 8; ++i) {
        for (u32 vertex = 0; vertex < 3; ++vertex) {
            cXyz *a = &attribute(this, assertText10.bytes, assertText11.bytes)->table[i][vertex];
            cXyz *v = &(*source)[vertex];
            v->x = (f32)a->x;
            v->y = (f32)a->y;
            v->z = (f32)a->z;
        }
        auto matrix = J3DModel_getBaseTRMtx(mModelBase);
        mkie_call::matrixMultVecArray(matrix, &(*source)[0], &(*destination)[0], 3);
        mkie_call::triangleSetPos(gabi::at<MkieTriView>(gabi::ea(&mTri[i]))->mGeometry,
                                  &(*destination)[0], &(*destination)[1], &(*destination)[2]);
    }
}
/* 02370618 */
void daObjMkie::Act_c::init_cc() {
    for (u32 i = 0; i < 8; ++i) {
        mStts[i].Init(255, 255, this);
        mkie_call::triangleSet(&mTri[i], gabi::at<dCcD_SrcTri>(0x1002C6F0));
        u32 flags = mTri[i].mGObjTg.mSPrm;
        mTri[i].mStts = &mStts[i];
        mTri[i].mGObjTg.mSPrm = flags | 4;
    }
    set_cc_pos();
}
/* 02370698 */
BOOL daObjMkie::Act_c::Create() {
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mModelBase));
    init_mtx();
    daObjMkie::Attr_c *box[6];
    for (u32 i = 0; i < 6; ++i)
        box[i] = attribute(this, assertText7.bytes, assertText8.bytes);
    f32 x0 = -80.0f * (f32)box[0]->mScale, x1 = 80.0f * (f32)box[3]->mScale;
    f32 y0 = -(f32)box[1]->mScale, z0 = -80.0f * (f32)box[2]->mScale;
    f32 y1 = 230.0f * (f32)box[4]->mScale, z1 = 80.0f * (f32)box[5]->mScale;
    fopAcM_setCullSizeBox(this, x0, y0, z0, x1, y1, z1);
    init_cc();
    if (PrmAbstract(this, PRM_FLAG_W, PRM_FLAG_S)) {
        u32 value = actor_status;
        gbaName = 0;
        actor_status = value & 0xFFFFFFC0;
    }
    u32 attributeType = mType;
    mCcInit = 0;
    mBaseAnmPlaying = 0;
    if (attributeType >= 2) {
        JUT_ASSERT_fail(assertText7.bytes, 0x183, assertText8.bytes);
        attributeType = mType;
    }
    daObjMkie::Attr_c *a = &M_attr[attributeType];
    f32 y = current.pos.y, z = (f32)attention_info.position.z, x = (f32)attention_info.position.x;
    // HD: fused height arithmetic and load order retain NaN behavior.
    f32 attentionY = gabi::fmadds(130.0f, (f32)a->mScale, y);
    eyePos.z = z;
    eyePos.x = x;
    attention_info.position.y = attentionY;
    eyePos.y = attentionY;
    a = attribute(this, assertText7.bytes, assertText8.bytes);
    const char *name = a->name;
    u8 event = PrmAbstract(this, PRM_EVID_W, PRM_EVID_S);
    auto manager = dComIfGp_getPEvtManager();
    mEvtIdx = mkie_call::eventGetIndex(manager, name, event);
    return TRUE;
}
/* 02370914 */
void daObjMkie::Act_c::mode_proc_call() {
    u32 value = mModeProc;
    (this->*mode_proc[value])();
}
/* 02370960 */
BOOL daObjMkie::Act_c::Execute(be<u32> *outMtx) {
    // HD: attention X begins with a word copy, before any floating loads.
    *reinterpret_cast<be<u32> *>(&attention_info.position.x) =
        (u32) * reinterpret_cast<be<u32> *>(&current.pos.x);
    daObjMkie::Attr_c *a = attribute(this, assertText12.bytes, assertText13.bytes);
    f32 z = current.pos.z, x = (f32)attention_info.position.x, y = current.pos.y;
    f32 height = gabi::fmadds(130.0f, (f32)a->mScale, y);
    eyePos.z = z;
    attention_info.position.z = z;
    eyePos.x = x;
    attention_info.position.y = height;
    eyePos.y = height;
    mode_proc_call();
    if (mStatueDelete)
        fopAcM_delete(this);
    mCcInit = 0;
    set_mtx();
    *outMtx = gabi::ea(&M_tmp_mtx);
    return TRUE;
}
/* 02370A3C */
BOOL daObjMkie::Act_c::Draw() {
    if (!mBaseAnmPlaying) {
        bool vanish = mModeProc == DEMO && mTimer == 0;
        settingTevStruct(dKy_getEnvlight(), 0, &current.pos, &tevStr);
        auto env = dKy_getEnvlight();
        setLightTevColorType(env, vanish ? (J3DModel *)mModelVan : (J3DModel *)mModelBase, &tevStr);
        if (vanish) {
            auto model = (J3DModel *)mModelVan;
            f32 frame = mBrkVan.mFrameCtrl.mFrame;
            mDoExt_brkAnm_entry((mDoExt_brkAnm *)&mBrkVan, J3DModel_getModelData(model), frame);
        }
        dComIfGd_setListBG();
        mDoExt_modelUpdateDL(vanish ? (J3DModel *)mModelVan : (J3DModel *)mModelBase);
        dComIfGd_setList();
    }
    return TRUE;
}
/* 02370B64 */
BOOL daObjMkie::Act_c::chk_light() {
    auto play = dComIfGp_get();
    if (mkie_call::detectLight(&reinterpret_cast<dComIfG_inf_c *>(play)->play.mDetect, &eyePos))
        return TRUE;
    BOOL hit = FALSE;
    for (u32 i = 0; i < 8; ++i)
        if (mkie_call::triangleCheckHit(&mTri[i])) {
            mkie_call::triangleClearHit(&mTri[i]);
            hit = TRUE;
        }
    return hit;
}
/* 02370C00 */
void daObjMkie::Act_c::eff_break() {
    gabi::Local<GXColor> color;
    color->g = (u8)(s16)tevStr.mColorC0.g;
    color->b = (u8)(s16)tevStr.mColorC0.b;
    color->a = (u8)(s16)tevStr.mColorC0.a;
    color->r = (u8)(s16)tevStr.mColorC0.r;
    dComIfGp_particle_set(0x81A0, &current.pos, &shape_angle, &scale, 255, nullptr, -1,
                          &tevStr.mColorK0, (GXColor *)color);
}
/* 02370C90 */
void daObjMkie::Act_c::sound_melt() {
    s32 reverb = dComIfGp_getReverb(current.roomNo);
    mDoAud_seStart(0x69C2, &eyePos, 0, reverb);
}
/* 02370CD8 */
void daObjMkie::Act_c::sound_break() {
    u32 value = mType;
    s8 room = current.roomNo;
    u32 sound = (u32)se_break[value];
    s32 reverb = dComIfGp_getReverb(room);
    mDoAud_seStart(sound, &eyePos, 0, reverb);
}
/* 02370D38 */
void daObjMkie::Act_c::vib_break() {
    auto vibration = dComIfGp_getVibration();
    gabi::Local<cXyz> direction;
    direction->z = 0.0f;
    direction->y = 1.0f;
    direction->x = 0.0f;
    mkie_call::startShock(vibration, 4, -33, (cXyz *)direction);
}
/* 02370D88 */
void daObjMkie::Act_c::mode_demoWait_init() {
    mEvtPlaying = 0;
    mModeProc = DEMO_WAIT;
}
/* 02370D9C */
void daObjMkie::Act_c::mode_wait() {
    // HD: test the full BOOL register, rather than GameCube's low-byte mask.
    if (chk_light()) {
        s16 count = (s16)((s32)mLightCnt + 1);
        mLightCnt = count;
        if (count > 20) {
            mode_demoWait_init();
            return;
        }
    } else
        mLightCnt = 0;
    if (mCcInit)
        set_cc_pos();
    if (!(actor_condition & fopAcCnd_NODRAW_e))
        for (u32 i = 0; i < 8; ++i) {
            auto system = dComIfG_Ccsp();
            mkie_call::collisionSet(system, &mTri[i]);
        }
}
/* 02370E6C */
void daObjMkie::Act_c::mode_demo_init() {
    mModeProc = DEMO;
    mTimer = 10;
    if (!PrmAbstract(this, PRM_CORRECT_W, PRM_CORRECT_S))
        mkie_call::playRiddle(0x806);
    mBroken = 0;
    mBaseAnmPlaying = 0;
}
/* 02370ECC */
void daObjMkie::Act_c::mode_demoWait() {
    s16 index = mEvtIdx;
    if (mkie_call::eventExists(dComIfGp_getPEvtManager(), index)) {
        if ((u16)eventInfo.mCommand == dEvtCmd_INDEMO_e) {
            mEvtPlaying = 1;
            mode_demo_init();
        } else {
            u8 event = PrmAbstract(this, PRM_EVID_W, PRM_EVID_S);
            index = mEvtIdx;
            fopAcM_orderOtherEventId(this, index, event, 0xFFFF, 0, 1);
            u16 condition = (u16)eventInfo.mCondition;
            eventInfo.mCondition = condition | 2;
        }
    } else
        mode_demo_init();
}
/* 02370F98 */
void daObjMkie::Act_c::mode_demo() {
    // HD: retain timer/event reloads across the clobbering helper calls.
    s16 count = mTimer;
    bool play = count == 0;
    if (count > 0) {
        count = (s16)(count - 1);
        mTimer = count;
        if (count == 0) {
            eff_break();
            sound_melt();
            u32 sw = PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
            dComIfGs_onSwitch(sw, home.roomNo);
            play = mTimer == 0;
        }
    }
    if (play) {
        u32 done = mkie_call::brkPlayHD(&mBrkVan);
        u8 wasBroken = mBroken;
        mBaseAnmPlaying = done != 0;
        if (!wasBroken && done) {
            sound_break();
            vib_break();
            mBroken = 1;
        }
    }
    if (mEvtPlaying) {
        s16 index = mEvtIdx;
        if (mkie_call::eventEndCheck(dComIfGp_getPEvtManager(), index)) {
            dComIfGp_event_reset();
            s16 remaining = mTimer;
            mEvtPlaying = 0;
            if (remaining != 0)
                return;
        } else if (mEvtPlaying)
            return;
    }
    if (mTimer == 0 && mBaseAnmPlaying)
        mStatueDelete = 1;
}
/* 023710EC */
static s32 Mkie_Create(daObjMkie::Act_c *p) {
    return p->Mthd_Create();
}
/* 023710F0 */
static BOOL Mkie_Delete(daObjMkie::Act_c *p) {
    return p->Mthd_Delete();
}
/* 023710F4 */
static BOOL Mkie_Execute(daObjMkie::Act_c *p) {
    return p->MoveBGExecute();
}
/* 023710F8 */
static BOOL Mkie_Draw(daObjMkie::Act_c *p) {
    return p->Draw_v();
}
/* 02371108 */
static BOOL Mkie_IsDelete(daObjMkie::Act_c *p) {
    return p->IsDelete_v();
}
/* 02371118 */
// HD: compiler-generated initialization and constructor/destructor entries.
static void __sinit_d_a_obj_mkie_cpp() {
    sinit_header_statics(0x1046A53C, 0x101CAB7C);
    for (u32 type = 0; type < 2; ++type)
        for (u32 i = 0; i < 8; ++i)
            for (u32 coordinate = 0; coordinate < 9; ++coordinate)
                reinterpret_cast<be<f32> *>(M_attr[type].table[i])[coordinate] =
                    vertices[type][i][coordinate];
}
/* 023714CC */
static void MkieTrivial_dtor(void *p, s32 flags) {
    if (p && (flags & 1))
        operator_delete(p);
}
/* 023714E0 */
static dCcD_Stts *MkieStatus_ctor(dCcD_Stts *p) {
    if (!p)
        p = mkie_call::allocate<dCcD_Stts *>(0x3C);
    if (p) {
        mkie_call::statusConstruct(p);
        mkie_call::globalStatusConstruct(&p->__vtbl_gstts);
        p->__vtbl = 0x1004AE88;
        p->__vtbl_gstts = 0x1004AEC0;
    }
    return p;
}
/* 02371548 */
static dCcD_Tri *MkieTriangle_ctor(dCcD_Tri *p) {
    if (!p)
        p = mkie_call::allocate<dCcD_Tri *>(0x150);
    if (p) {
        MkieTriView *a = reinterpret_cast<MkieTriView *>(p);
        mkie_call::objectInfoConstruct(p);
        a->mShapeVtable = 0x100015A8;
        a->mBoundsVtable = 0x1002C498;
        mkie_call::triangleGeometryConstruct(a->mGeometry);
        p->__vtbl_hitinf = 0x1004B010;
        a->mPlaneVtable = 0x1004B058;
        a->mShapeVtable = 0x1004B068;
    }
    return p;
}
/* 023715D4 */
static BOOL IsDelete(daObjMkie::Act_c *p) {
    return TRUE;
}
/* 023715DC */
static void MkieEmptyVirtual(daObjMkie::Act_c *p) {}
/* 023715E0 */
static BOOL Delete(daObjMkie::Act_c *p) {
    return TRUE;
}
/* 023715E8 */
static void Act_c_dtor(daObjMkie::Act_c *p, s32 flags) {
    if (p) {
        mkie_call::destroyArray(&p->mStts[0], 8, 0x3C, 0x02515860, 0, 0);
        mkie_call::destroyArray(&p->mTri[0], 8, 0x150, 0x025159F8, 0, 0);
        mkie_call::actorDestroy(p, 0);
        if (flags & 1)
            operator_delete(p);
    }
}

/* WWHD glue (tools/verify/autoglue.py)
data M_arcname 1002C6E8 nocheck
data assertText1 1002C4C0 nocheck
data assertText2 1002C4FC nocheck
data assertText3 1002C564 nocheck
data assertText4 1002C540 nocheck
data assertText5 1002C550 nocheck
data assertText6 1002C578 nocheck
data assertText7 1002C5D4 nocheck
data assertText8 1002C5E8 nocheck
data assertText9 1002C4D4 nocheck
data assertText10 1002C588 nocheck
data assertText11 1002C59C nocheck
data assertText12 1002C628 nocheck
data assertText13 1002C63C nocheck
data mode_proc 1002C610 nocheck
data M_attr 101CABF0 nocheck
data M_tmp_mtx 1046A558
data se_break 1002C664 nocheck
*/
