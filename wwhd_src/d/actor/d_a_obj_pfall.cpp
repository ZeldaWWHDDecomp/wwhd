/* Rat-operated trapdoor. GameCube structure corrected against WWHD.
 */
#include "d/actor/d_a_obj_pfall.h"
#include "gc_api/JSystem/JUTAssert.h"
#include "gc_api/d/d_lib.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/m_Do/m_Do_mtx.h"
#include "gc_api/SSystem/c_lib.h"
const gc_tu_data gc_tu = {.safeString = 0x1002E65C};
enum PfallMode { MODE_WAIT, MODE_EVENT };
enum PfallAction { ACT_WAIT, ACT_OPEN, ACT_HIKU };
enum PfallAnimation { ANM_NONE, ANM_WAIT, ANM_HIKU };
enum PfallResource { BDL_AOTSI = 9, BDL_NZ = 10, BDL_PSUZU = 11, DZB_AOTOSI = 14 };
enum PfallSound {
    JA_SE_CV_NZ_LAUGH = 0x48A4,
    JA_SE_OBJ_TC_JAIL_STRING = 0x699B,
    JA_SE_OBJ_TC_JAIL_FLR_BRK = 0x699C
};
static f32 zeroValue;
static f32 leftX;
static f32 leftZ;
static f32 rightX;
static f32 rightZ;
static f32 hingeDown;
static f32 hingeUp;
static f32 ratOffsetZ;
static f32 ratScale;
static f32 ropeStep;
static f32 pullFrame;
static f32 releaseFrame;
static f32 endFrame;
static f32 ropeDrop;
static u32 leftInitialized;
static u32 rightInitialized;
static u32 ratOffsetInitialized;
static u32 ratScaleInitialized;
static u32 debugInitialized;
static cXyz offset_pos_left;
static cXyz offset_pos_right;
static cXyz offset_pos;
static cXyz rat_scale;
static daObj_PfallHIO_c l_HIO;
static PfallActionName action_table[3] = {};
static s32 a_anm_bcks_tbl[2] = {};
static dLib_anm_prm_c a_anm_prm_tbl[3] = {};
BOOL CallbackCreateHeap(fopAc_ac_c *);
static PfallModeFunc mode_proc[2] = {};
static u32 staticHeader[4] = {};
static u32 staticRegistration0[3] = {};
static u32 staticRegistration1[3] = {};
static u32 staticRegistration2[3] = {};
static u32 staticRegistration3[3] = {};

static f32 staticFloat0;
static f32 staticFloat1;
static f32 initialFloat0;
static f32 initialFloat1;

static u8 staticCharacter;
static u8 staticByte;
static u8 debugObject[4] = {};
static dEvent_manager_c *eventManager() { return dComIfGp_getPEvtManager(); }
static void cutEnd(s32 staff) { pfallCutEnd(eventManager(), staff); }
static void pfallEmpty(u8 *);
static void modelMatrix(J3DModel *model) {
    f32 matrix[12];
    const be<f32> *stack = reinterpret_cast<const be<f32> *>(mDoMtx_stack_c::get());
    be<f32> *dest = reinterpret_cast<be<f32> *>(&model->mBaseTransformMtx);
    for (u32 i = 0; i < 12; i++)
        matrix[i] = stack[i];
    for (u32 i = 0; i < 12; i++)
        dest[i] = matrix[i];
}
static J3DModelData *pfallResource(s32 index) {
    return static_cast<J3DModelData *>(dComIfG_getObjectRes("Pfall", index));
}
static void resourceAssert(J3DModelData *data, s32 line, const char *expression) {
    if (!data)
        JUTAssertion::showAssert("d_a_obj_pfall.cpp", line, expression);
}
static J3DShape *namedShape(J3DModelData *data, const char *name) {
    gabi::Local<SafeString> string;
    string->mStringTop = gabi::guest_addr(name);
    string->__vtbl = gc_tu.safeString;
    PfallResourceHeader *names = gabi::at<PfallResourceHeader>(data->mpResHeader);
    pfallEmpty(reinterpret_cast<u8 *>(string.get()));
    u32 offset = names->materialNames;
    JUTNameTab *table = gabi::at<JUTNameTab>(offset ? gabi::ea(names) + 0x18 + offset : 0);
    s32 index = table->getIndex(gabi::at<char>(string->mStringTop));
    u32 material = 0;
    if (index >= 0) {
        u32 count = data->mMaterialNum;
        material = data->mMaterials;
        if ((u32)index < count)
            material += (u32)index * 0x39C;
    }
    return gabi::at<J3DMaterial>(material)->mShape;
}
/* 023830C8 */
BOOL CallbackCreateHeap(fopAc_ac_c *self) {
    return static_cast<daObj_Pfall_c *>(self)->CreateHeap();
}
/* 02383EB0 */
static daObj_PfallHIO_c *pfallHioConstruct(daObj_PfallHIO_c *self) {
    if (!self)
        self = static_cast<daObj_PfallHIO_c *>(pfallAllocate(0x10));
    if (self) {
        self->mVtable = PFALL_HIO_VTABLE;
        self->field_0x08 = ropeDrop;
        self->field_0x0C = 0;
        self->field_0x06 = 0;
        self->mNo = -1;
        self->field_0x05 = 0;
    }
    return self;
}
/* 02383794 */
void daObj_Pfall_c::setAnm() {
    pfallSetAnm("Pfall", mpMorf, &mBckIdx, &field_0x481, &mOldAnmPrmIdx, a_anm_bcks_tbl,
                a_anm_prm_tbl, 0);
}
/* 0238299C */
void daObj_Pfall_c::set_mtx() {
    f32 zero = zeroValue;
    if (!leftInitialized) {
        offset_pos_left.x = leftX;
        offset_pos_left.z = leftZ;
        leftInitialized = 1;
        offset_pos_left.y = zero;
    }
    if (!rightInitialized) {
        offset_pos_right.x = rightX;
        offset_pos_right.z = rightZ;
        rightInitialized = 1;
        offset_pos_right.y = zero;
    }
    J3DModel *model = field_0x3C4;
    f32 x = scale.x, y = scale.y, z = scale.z;
    model->mBaseScale.x = x;
    model->mBaseScale.z = z;
    model->mBaseScale.y = y;
    mDoMtx_stack_c::transS((f32)current.pos.x, (f32)current.pos.y, (f32)current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    mDoMtx_stack_c::transM(offset_pos_left.x, offset_pos_left.y, offset_pos_left.z);
    f32 down = hingeDown;
    mDoMtx_stack_c::transM(zero, down, zero);
    mDoMtx_stack_c::ZrotM((s16)field_0x3B4);
    f32 up = hingeUp;
    mDoMtx_stack_c::transM(zero, up, zero);
    modelMatrix(field_0x3C4);
    MTXCopy(mDoMtx_stack_c::get(), &field_0x3CC);
    x = scale.x;
    y = scale.y;
    model = field_0x3FC;
    z = scale.z;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    mDoMtx_stack_c::transS((f32)current.pos.x, (f32)current.pos.y, (f32)current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    mDoMtx_stack_c::transM(offset_pos_right.x, offset_pos_right.y, offset_pos_right.z);
    mDoMtx_stack_c::YrotM(-0x8000);
    mDoMtx_stack_c::transM(zero, down, zero);
    mDoMtx_stack_c::ZrotM((s16)field_0x3B4);
    mDoMtx_stack_c::transM(zero, up, zero);
    modelMatrix(field_0x3FC);
    MTXCopy(mDoMtx_stack_c::get(), &field_0x404);
    model = mpMorf->getModel();
    if (!ratOffsetInitialized) {
        offset_pos.y = zero;
        ratOffsetInitialized = 1;
        offset_pos.x = zero;
        offset_pos.z = ratOffsetZ;
    }
    if (!ratScaleInitialized) {
        f32 half = ratScale;
        ratScaleInitialized = 1;
        rat_scale.z = half;
        rat_scale.x = half;
        rat_scale.y = half;
    }
    x = rat_scale.x;
    y = rat_scale.y;
    z = rat_scale.z;
    model->mBaseScale.x = x;
    model->mBaseScale.z = z;
    model->mBaseScale.y = y;
    mDoMtx_stack_c::transS((f32)current.pos.x, (f32)current.pos.y, (f32)current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    mDoMtx_stack_c::transM(offset_pos.x, offset_pos.y, offset_pos.z);
    modelMatrix(model);
    f32 lift = l_HIO.field_0x08;
    f32 offset = field_0x438;
    mDoMtx_stack_c::transM(zero, offset + lift, zero);
    mDoMtx_stack_c::YrotM(-0x8000);
    modelMatrix(mpHimoModel);
}

/* 02382E64 */
BOOL daObj_Pfall_c::CreateHeap() {
    J3DModelData *data = pfallResource(BDL_AOTSI);
    resourceAssert(data, 0xD1, "modelData != (0)");
    field_0x3C4 = mDoExt_J3DModel__create(data, 0, 0x11020203);
    field_0x3FC = mDoExt_J3DModel__create(data, 0, 0x11020203);
    if (field_0x3C4 == nullptr || field_0x3FC == nullptr)
        return 0;
    data = pfallResource(BDL_PSUZU);
    resourceAssert(data, 0xD9, "himoModelData != (0)");
    mpHimoModel = mDoExt_J3DModel__create(data, 0, 0x11020203);
    if (mpHimoModel == nullptr)
        return 0;
    data = pfallResource(BDL_NZ);
    resourceAssert(data, 0xE0, "nzModelData != (0)");
    mpMorf = mDoExt_McaMorf::create(nullptr, data, nullptr, nullptr, nullptr, -1, ropeStep, 0, -1,
                                    1, nullptr, 0x80000, 0x11000002);
    if (mpMorf == nullptr || mpMorf->getModel() == nullptr)
        return 0;
    mpMorf->getModel()->setUserArea(gabi::ea(this));
    set_mtx();
    mpMorf->calc();
    mpBgW = pfallNewBackground(nullptr);
    if (mpBgW == nullptr)
        return 0;
    data = pfallResource(DZB_AOTOSI);
    if (pfallSetBackground(mpBgW, reinterpret_cast<cBgD_t *>(data), 1, &field_0x3CC))
        return 0;
    mpBgW2 = pfallNewBackground(nullptr);
    if (mpBgW2 == nullptr)
        return 0;
    data = pfallResource(DZB_AOTOSI);
    return pfallSetBackground(mpBgW2, reinterpret_cast<cBgD_t *>(data), 1, &field_0x404) == 0;
}
/* 023830E4 */
void daObj_Pfall_c::CreateInit() {
    dComIfG_Bgsp()->Regist(mpBgW, this);
    dComIfG_Bgsp()->Regist(mpBgW2, this);
    set_mtx();
    field_0x481 = ANM_WAIT;
    mode_wait_init();
}
/* 0238314C */
s32 daObj_Pfall_c::_create() {
    u32 flags = actor_condition;
    if (!(flags & fopAcCnd_INIT_e)) {
        // HD: the constructor is skipped for a zero guest address.
        if (gabi::ea(this) != 0) {
            fopAc_ac_c_ct(this);
            __vtbl = PFALL_VTABLE;
            __ct(&field_0x444);
            flags = actor_condition;
        }
        actor_condition = flags | fopAcCnd_INIT_e;
    }
    s32 phase = dComIfG_resLoad(&mPhsPfall, "Pfall");
    if (phase == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, CallbackCreateHeap, 0x38E0))
            return cPhs_ERROR_e;
        CreateInit();
    }
    return phase;
}
/* 0238321C */
BOOL daObj_Pfall_c::_delete() {
    dComIfG_resDeleteDemo(&mPhsPfall, "Pfall");
    if (mpBgW != nullptr && mpBgW->ChkUsed())
        dComIfG_Bgsp()->Release(mpBgW);
    if (mpBgW2 != nullptr && mpBgW2->ChkUsed())
        dComIfG_Bgsp()->Release(mpBgW2);
    return 1;
}
/* 02383590 */
void daObj_Pfall_c::cutProc() {
    s32 staff = pfallStaffId(eventManager(), "Nzfall", 0, 0);
    if (staff == -1)
        return;
    s32 action = pfallActIdx(eventManager(), staff, action_table, 3, 0, 0);
    dEvent_manager_c *manager = eventManager();
    if (action == -1) {
        pfallCutEnd(manager, staff);
        return;
    }
    if (pfallIsAdvance(manager, staff)) {
        switch (action) {
        case ACT_WAIT:
            cutWaitStart(staff);
            break;
        case ACT_OPEN:
            cutOpenStart(staff);
            break;
        case ACT_HIKU:
            cutHikuStart(staff);
            break;
        }
    }
    switch (action) {
    case ACT_WAIT:
        cutWaitProc(staff);
        break;
    case ACT_OPEN:
        cutOpenProc(staff);
        break;
    case ACT_HIKU:
        cutHikuProc(staff);
        break;
    }
}
/* 023832AC */
void daObj_Pfall_c::cutWaitStart(s32 staff) {
    // HD: the original casts the integer to its low signed halfword.
    PfallTimerData *timer = pfallGetInteger(eventManager(), staff, "Timer", 3);
    mTimer = timer ? (s16)timer->low : 0;
}
/* 02383378 */
void daObj_Pfall_c::cutWaitProc(s32 staff) {
    if (!pfallCalcTimer(&mTimer)) {
        if (!field_0x483) {
            s8 room = current.roomNo;
            field_0x483 = 1;
            s32 reverb = pfallGetReverb(room);
            pfallMonsterSound(JA_SE_CV_NZ_LAUGH, &eyePos, (u32)mBsPcId, 0, reverb);
        }
        cutEnd(staff);
    }
}
/* 02383310 */
void daObj_Pfall_c::cutOpenStart(s32 staff) {
    s8 room = current.roomNo;
    field_0x3B4 = 0;
    field_0x3B0 = 0;
    s32 reverb = pfallGetReverb(room);
    mDoAud_seStart(JA_SE_OBJ_TC_JAIL_FLR_BRK, &eyePos, 0, reverb);
}
/* 02383404 */
void daObj_Pfall_c::cutOpenProc(s32 staff) {
    s32 step = field_0x3B0;
    if (step >= 7) {
        cutEnd(staff);
        return;
    }
    gabi::Local<be<s16>[7]> targets;
    (*targets)[0] = -0x3A98;
    (*targets)[1] = -0x2AF8;
    (*targets)[2] = -0x36B0;
    (*targets)[3] = -0x2EE0;
    (*targets)[4] = -0x32C8;
    (*targets)[5] = -0x30D4;
    (*targets)[6] = -0x31CE;
    s16 target = *gabi::at<be<s16>>(gabi::ea(targets.get()) + (u32)step * 2);
    s16 difference = cLib_addCalcAngleS(&field_0x3B4, target, 4, 0x2000, 0x100);
    s16 magnitude = (s16)(difference < 0 ? -(s32)difference : (s32)difference);
    if (magnitude <= 0x100)
        field_0x3B0 = (u32)field_0x3B0 + 1;
}
/* 0238336C */
void daObj_Pfall_c::cutHikuStart(s32 staff) { field_0x481 = ANM_HIKU; }
/* 02383510 */
void daObj_Pfall_c::cutHikuProc(s32 staff) {
    if (field_0x481 == ANM_HIKU) {
        if (mpMorf->isStop()) {
            cutEnd(staff);
            field_0x481 = ANM_WAIT;
        }
    }
}
/* 023830CC */
void daObj_Pfall_c::mode_wait_init() {
    s16 wait = l_HIO.field_0x0C;
    field_0x3A8 = MODE_WAIT;
    field_0x3B8 = wait;
}
/* 02383D24 */
void daObj_Pfall_c::mode_wait() {
    gabi::Local<cXyz> position;
    position->x = current.pos.x;
    position->y = current.pos.y;
    position->z = current.pos.z;
    if (pfallCheckPlayer(position.get(), 100.0f, 100.0f)) {
        fopAc_ac_c *player = dComIfGp_getPlayer(0);
        if (player->speedF == 0.0f && !pfallCalcTimer(&field_0x3B8))
            mode_event_init();
    }
    mpBgW->Move();
    mpBgW2->Move();
}
/* 02383D18 */
void daObj_Pfall_c::mode_event_init() { field_0x3A8 = MODE_EVENT; }
/* 02383DC4 */
void daObj_Pfall_c::mode_event() {
    if (eventInfo.mCommand == 2) {
        if (pfallEndCheck(eventManager(), "NZFALL")) {
            if (l_HIO.field_0x06)
                mode_wait_init();
            else
                pfallNextStage("sea", 15, 11, -1, 0.0f, 0, 1, 0);
        }
    } else
        pfallOrderEvent(this, "NZFALL", 1, 0xFFFF);
    mpBgW->Move();
    mpBgW2->Move();
}
/* 02383748 */
void daObj_Pfall_c::mode_proc_call() { (this->*mode_proc[(u32)field_0x3A8])(); }
/* 023837E4 */
BOOL daObj_Pfall_c::_execute() {
    // HD: copy position bits without quieting signaling NaNs; retain store order.
    PfallPositionBits *bits = reinterpret_cast<PfallPositionBits *>(this);
    u32 z = bits->current.z, y = bits->current.y, x = bits->current.x;
    bits->eye.z = z;
    bits->eye.y = y;
    bits->attention.x = x;
    bits->attention.z = z;
    bits->eye.x = x;
    bits->attention.y = y;
    if (dComIfGp_event_runCheck())
        cutProc();
    f32 zero = zeroValue;
    mDoExt_McaMorf *morph;
    if (field_0x481 == ANM_HIKU) {
        morph = mpMorf;
        f32 six = pullFrame;
        if (morph->getFrame() == six) {
            s32 reverb = pfallGetReverb(current.roomNo);
            mDoAud_seStart(JA_SE_OBJ_TC_JAIL_STRING, &eyePos, 0, reverb);
            morph = mpMorf;
        }
        f32 frame = morph->getFrame();
        if (!(frame > zero && frame <= six)) {
            f32 ten = hingeUp;
            if (frame > six && !(frame > ten)) {
                field_0x438 = (f32)field_0x438 + ropeStep;
                morph = mpMorf;
            } else {
                f32 twentyThree = releaseFrame;
                if (!(frame > ten && frame <= twentyThree) && frame > twentyThree &&
                    !(frame > endFrame)) {
                    field_0x438 = (f32)field_0x438 - ropeDrop;
                    morph = mpMorf;
                }
            }
        }
    } else {
        morph = mpMorf;
        field_0x438 = zero;
    }
    morph->play(nullptr, 0, 0);
    fopAcM_posMoveF(this, nullptr);
    set_mtx();
    mode_proc_call();
    setAnm();
    return 0;
}
/* 023839BC */
void daObj_Pfall_c::nz_draw() {
    J3DModel *model = mpMorf->getModel();
    J3DModelData *data = model->getModelData();
    J3DJoint *joint = gabi::at<J3DJoint>(data->mJointNodes);
    J3DShape *shape = namedShape(data, "SC_eye_MX"), *shape2 = namedShape(data, "lambert2"),
             *shape3 = namedShape(data, "lambert7");
    shape->hide();
    dScnKy_env_light_c *env = dKy_getEnvlight();
    settingTevStruct(env, 0, &current.pos, &tevStr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, model, &tevStr);
    mpMorf->updateDL();
    dComIfGd_setListMaskOff();
    shape->show();
    shape2->hide();
    shape3->hide();
    pfallJointEntry(model, joint);
    shape2->show();
    shape3->show();
    dComIfGd_setList();
}
/* 02383C24 */
BOOL daObj_Pfall_c::_draw() {
    dScnKy_env_light_c *env = dKy_getEnvlight();
    settingTevStruct(env, 1, &current.pos, &tevStr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, field_0x3C4.get(), &tevStr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, field_0x3FC.get(), &tevStr);
    mDoExt_modelUpdateDL(field_0x3C4);
    mDoExt_modelUpdateDL(field_0x3FC);
    nz_draw();
    if (l_HIO.field_0x05 && !debugInitialized) {
        debugInitialized = 1;
        pfallDebugInit(debugObject, "", 4);
    }
    env = dKy_getEnvlight();
    settingTevStruct(env, 0, &current.pos, &tevStr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, mpHimoModel.get(), &tevStr);
    mDoExt_modelUpdateDL(mpHimoModel);
    return 1;
}
/* 02383218 */
s32 daObj_PfallCreate(daObj_Pfall_c *self) { return self->_create(); }
/* 023832A8 */
BOOL daObj_PfallDelete(daObj_Pfall_c *self) { return self->_delete(); }
/* 023839B8 */
BOOL daObj_PfallExecute(daObj_Pfall_c *self) { return self->_execute(); }
// HD: material-name lookup uses a TU-specific SafeString temporary.

/* 02383D14 */
BOOL daObj_PfallDraw(daObj_Pfall_c *self) { return self->_draw(); }
/* 02383FD4 */
BOOL daObj_PfallIsDelete(daObj_Pfall_c *self) { return 1; }
/* 02383F14 */
static void pfallStaticInit() {
    for (u32 i = 0; i < 4; i++)
        staticHeader[i] = 0;
    pfallRegister(staticRegistration0);
    staticFloat0 = initialFloat0;
    staticFloat1 = initialFloat1;
    pfallInitChar(&staticCharacter);
    pfallRegister(staticRegistration1);
    pfallInitByte(&staticByte);
    pfallRegister(staticRegistration2);
    pfallHioConstruct(&l_HIO);
    pfallRegister(staticRegistration3);
}

/* 02383FC0 */
static void pfallSimpleDelete(u8 *self, s32 flags) {
    if (self && (flags & 1))
        pfallFree(self);
}
/* 02383FDC */
static void pfallDestructor(daObj_Pfall_c *self, s32 flags) {
    if (self) {
        __dt(&self->field_0x444);
        pfallDestroyActor(self, 0);
        if (flags & 1)
            pfallFree(self);
    }
}
/* 0238403C */
static void pfallHioDelete(daObj_PfallHIO_c *self, s32 flags) {
    if (self) {
        self->mNo = -1;
        self->mVtable = PFALL_HIO_VTABLE;
        if (flags & 1)
            pfallFree(self);
    }
}
/* 02384064 */
static void pfallEmpty(u8 *self) {}

/* WWHD glue (tools/verify/autoglue.py)
data zeroValue 1002E650 nocheck
data leftX 1002E694 nocheck
data leftZ 1002E698 nocheck
data rightX 1002E69C nocheck
data rightZ 1002E6A0 nocheck
data hingeDown 1002E6A4 nocheck
data hingeUp 1002E6A8 nocheck
data ratOffsetZ 1002E6AC nocheck
data ratScale 1002E6B0 nocheck
data ropeStep 1002E6B4 nocheck
data pullFrame 1002E7A8 nocheck
data releaseFrame 1002E7AC nocheck
data endFrame 1002E7B0 nocheck
data ropeDrop 1002E7B4 nocheck
data leftInitialized 1046BC58 nocheck
data rightInitialized 1046BC5C nocheck
data ratOffsetInitialized 1046BC60 nocheck
data ratScaleInitialized 1046BC64 nocheck
data debugInitialized 101FDA48 nocheck
data offset_pos_left 1046BC28 nocheck
data offset_pos_right 1046BC34 nocheck
data offset_pos 1046BC40 nocheck
data rat_scale 1046BC4C nocheck
data l_HIO 1046BC08 nocheck
str 1002E654
str 1002E6B8 in 02382E64
str 1002E6C0
str 1002E6D4
str 1002E6E8
str 1002E700
str 1002E714 in 0238314C
str 1002E71C in 0238321C
str 1002E724
str 1002E738
str 1002E770 in 02383794
str 1002E7B8
str 1002E7C4
str 1002E7D0
str 1002E7E0
str 1002E7E4
data action_table 101CC3C8 nocheck
data a_anm_bcks_tbl 1002E768 nocheck
data a_anm_prm_tbl 1002E778 nocheck
data mode_proc 1002E758 nocheck
data staticHeader 1046BC18 nocheck
data staticRegistration0 101CC3D4 nocheck
data staticRegistration1 101CC3E0 nocheck
data staticRegistration2 101CC3EC nocheck
data staticRegistration3 101CC3F8 nocheck
data staticFloat0 1046BBFC nocheck
data staticFloat1 1046BC00 nocheck
data initialFloat0 1002E7F0 nocheck
data initialFloat1 1002E7F4 nocheck
data staticCharacter 1046BC04 nocheck
data staticByte 1046BC05 nocheck
data debugObject 101FEBEC nocheck
ext 027F583C void pfallJointEntry(J3DModel*, J3DJoint*)
ext C000A848 void* pfallDebugInit(be<u8>*, const char*, u32)
ext 028F026C void pfallRegister(be<u32>*)
ext 028ED6F8 void pfallInitChar(be<u8>*)
ext 028EAB2C void pfallInitByte(be<u8>*)
ext 02520540 s32 pfallGetReverb(s32)
ext 02543280 void pfallCutEnd(dEvent_manager_c*, s32)
ext 0254487C PfallTimerData* pfallGetInteger(dEvent_manager_c*, s32, const char*, s32)
ext 0211D2F8 s32 pfallCalcTimer(be<s32>*)
ext 025E1AA4 void pfallMonsterSound(u32, cXyz*, u32, u32, s32)
ext 02542D88 s32 pfallStaffId(dEvent_manager_c*, const char*, u32, u32)
ext 02542EDC s32 pfallActIdx(dEvent_manager_c*, s32, gptr<char>*, s32, u32, u32)
ext 025447C8 s32 pfallIsAdvance(dEvent_manager_c*, s32)
ext 02587A0C void pfallSetAnm(const char*, mDoExt_McaMorf*, be<s8>*, be<s8>*, be<s8>*, be<s32>*, dLib_anm_prm_c*, u32)
ext 025881A4 s32 pfallCheckPlayer(cXyz*, f32, f32)
ext 0254457C s32 pfallEndCheck(dEvent_manager_c*, const char*)
ext 0252012C void pfallNextStage(const char*, s32, s32, s32, f32, u32, u32, u32)
ext 025D77DC void pfallOrderEvent(fopAc_ac_c*, const char*, u32, u32)
ext 0273AD10 void* pfallAllocate(u32)
ext 0273AF40 void pfallFree(void*)
ext 025D50BC void pfallDestroyActor(fopAc_ac_c*, s32)
ext 024F23F4 dBgW* pfallNewBackground(dBgW*)
ext 0200A030 s32 pfallSetBackground(dBgW*, cBgD_t*, u32, Mtx34*)
*/
