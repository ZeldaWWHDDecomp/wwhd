/** Scheduled quake/sound actor. */
#include "d/actor/d_a_obj_quake.h"
static daObjQuake_HIO_c l_HIO;
static QuakeHeaderStatics l_headerStatics;
static const f32 l_cycleLength = 6.0f;
static const f32 l_zero = 0.0f;
static const f32 l_one = 1.0f;

/* 02387230 */
u32 PrmAbstract(daObjQuake_c *actor, u32 width, u32 shift) {
    u32 parameters = actor->mParameters;
    width &= 63;
    shift &= 63;
    u32 mask = (width < 32 ? 1u << width : 0u) - 1u;
    return (shift < 32 ? parameters >> shift : 0u) & mask;
}

/* 02386B20 */
u8 daObjQuake_c::getPrmType() {
    return (u8)PrmAbstract(this, PRM_TYPE_W, PRM_TYPE_S);
}

/* 02386B4C */
cPhs_State daObjQuake_c::_create() {
    u32 flags = actor_condition;
    if (!(flags & fopAcCnd_INIT_e)) {
        if (this) {
            fopAc_ac_c_ct(this);
            flags = actor_condition;
            __vtbl = VTABLE;
        }
        actor_condition = flags | fopAcCnd_INIT_e;
    }
    if (dComIfGs_isSymbol(dSymbol_DIN_e))
        return cPhs_STOP_e;
    if (getPrmType() >= 3)
        return cPhs_ERROR_e;
    fopDwTg_DrawQTo(&draw_tag);
    mType = getPrmType();
    mActive = 0;
    // HD: obtain schedule seconds through the stage object's virtual method.
    QuakeStageInfo* info = static_cast<QuakeStageInfo*>(dComIfGp_getStageStagInfo());
    f32 seconds = static_cast<f32>(info->mSchSec);
    mDuration = seconds * 30.0f;
    u8 type = mType;
    if (type == TYPE_QUAKE || type == TYPE_CAMERA) {
        mStart = 4.0f;
        mEnd = 6.0f;
    } else if (type == TYPE_SOUND) {
        mStart = 0.0f;
        mEnd = 6.0f;
    }
    if (l_HIO.mNo < 0)
        l_HIO.mNo = mDoHIO_createChild("Vibration Effect", &l_HIO); // "振動効果" (Vibration Effect)
    l_HIO.mCount = (u32)l_HIO.mCount + 1u;
    return cPhs_COMPLEATE_e;
}

/* 02386CE8 */
cPhs_State daObjQuakeCreate(daObjQuake_c *actor) {
    return actor->_create();
}

/* 02386CEC */
BOOL daObjQuakeDelete(daObjQuake_c *actor) {
    return TRUE;
}

/* 02386CF4 */
s32 daObjQuake_c::getPrmPower() {
    return (s32)(PrmAbstract(this, PRM_POWER_W, PRM_POWER_S) + 1u);
}

/* 02386D20 */
u8 daObjQuake_c::getPrmSch() {
    return (u8)PrmAbstract(this, PRM_SCH_W, PRM_SCH_S);
}

/* 02386D4C */
BOOL daObjQuake_c::_execute() {
    mTimer = (f32)dKy_get_schbit_timer();
    u32 mask = QuakeFlag_BASE | QuakeFlag_CAMERA | QuakeFlag_SOUND | QuakeFlag_MOTOR;
    // HD: evaluate parameter power once, then share it between camera and motor.
    s32 power = getPrmPower(), cameraPower = power, motorPower = power;
    u8 cameraOverride = l_HIO.mCameraOverride,
          motorOverride = l_HIO.mMotorOverride;
    if (cameraOverride)
        cameraPower = (u8)l_HIO.mCameraPower + 1;
    if (motorOverride) {
        motorPower = (u8)l_HIO.mMotorPower + 1;
        mask = l_HIO.mCamera ? (mask | QuakeFlag_CAMERA) : (mask & ~QuakeFlag_CAMERA);
        mask = l_HIO.mSound ? (mask | QuakeFlag_SOUND) : (mask & ~QuakeFlag_SOUND);
        mask = l_HIO.mMotor ? (mask | QuakeFlag_MOTOR) : (mask & ~QuakeFlag_MOTOR);
        mask = l_HIO.mExtra ? (mask | QuakeFlag_EXTRA) : (mask & ~QuakeFlag_EXTRA);
    }
    u8 schedule = getPrmSch();
    BOOL active = FALSE;
    if (schedule & dKy_get_schbit()) {
        dComIfG_inf_c* gameInfo = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        if (!(gameInfo->play.mEvtCtrl.mEventFlag & dEvtFlag_TALK_e)) {
            f32 end = mEnd, divisor = l_cycleLength;
            end = end / divisor;
            f32 start = mStart;
            start = start / divisor;
            f32 duration = mDuration, lower = start * duration, timer = mTimer,
                    upper = end * duration;
            // HD: GHS uses a negated lower comparison, retaining its NaN behaviour.
            if (!(timer < lower) && timer < upper)
                active = TRUE;
        }
    }
    if (mActive) {
        if (mType == TYPE_SOUND)
            mDoAud_seStartQuake(JA_SE_ATM_JINARI);
        if (!active) {
            u8 type = mType;
            if (type == TYPE_QUAKE || type == TYPE_CAMERA) {
                dVibration_c* vibration = dComIfGp_getVibration();
                dVibration_StopQuake(vibration, type == TYPE_QUAKE ? mask : QuakeFlag_CAMERA_ONLY);
            }
            mActive = 0;
        }
    } else if (active) {
        u8 type = mType;
        f32 zero = l_zero, one = l_one;
        if (type == TYPE_QUAKE || type == TYPE_CAMERA) {
            dVibration_c* vibration = dComIfGp_getVibration();
            cXyz direction;
            direction.x = zero;
            direction.y = one;
            direction.z = zero;
            dVibration_StartQuake(vibration, type == TYPE_QUAKE ? motorPower : cameraPower,
                                                      type == TYPE_QUAKE ? mask : QuakeFlag_CAMERA_ONLY, &direction);
        }
        mActive = 1;
    }
    return TRUE;
}

/* 02387008 */
BOOL daObjQuakeExecute(daObjQuake_c *actor) {
    return actor->_execute();
}

/* 0238700C */
BOOL daObjQuake_c::_is_delete() {
    if (mActive) {
        u8 type = mType;
        if (type == TYPE_QUAKE || type == TYPE_CAMERA) {
            dVibration_c* vibration = dComIfGp_getVibration();
            dVibration_StopQuake(vibration, type == TYPE_QUAKE ? QuakeFlag_ALL : QuakeFlag_CAMERA_ONLY);
        }
    }
    s32 number = (s8)l_HIO.mNo;
    if (number >= 0) {
        u32 count = (u32)l_HIO.mCount - 1u;
        l_HIO.mCount = count;
        if (count == 0) {
            mDoHIO_deleteChild(number);
            l_HIO.mNo = -1;
        }
    }
    return TRUE;
}

/* 023870B8 */
BOOL daObjQuakeIsDelete(daObjQuake_c *actor) {
    return actor->_is_delete();
}

// HD: constructor may allocate and returns the HIO pointer; its vtable is the last word.
/* 023870BC */
daObjQuake_HIO_c* daObjQuake_HIO_c__ct(daObjQuake_HIO_c* p) {
    if (!p)
        p = static_cast<daObjQuake_HIO_c*>(operator_new(sizeof(daObjQuake_HIO_c)));
    if (p) {
        p->mCount = 0;
        p->mNo = -1;
        p->mVtable = daObjQuake_HIO_c::VTABLE;
        p->mMotorOverride = 0;
        p->mSound = 1;
        p->mMotorPower = 3;
        p->mMotor = 1;
        p->mCameraPower = 3;
        p->mCamera = 1;
        p->mExtra = 0;
        p->mCameraOverride = 0;
    }
    return p;
}

// HD: compiler-generated and template code of this translation unit.
/* 02387134 */
static void __sinit_d_a_obj_quake_cpp() {
    l_headerStatics.zero[2] = 0;
    l_headerStatics.zero[0] = 0;
    l_headerStatics.zero[3] = 0;
    l_headerStatics.zero[1] = 0;
    __register_global_object(0x101CC718);
    l_headerStatics.negativePi = -3.1415927410125732f;
    l_headerStatics.positivePi = 3.1415927410125732f;
    QuakeFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101CC724);
    QuakeSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101CC730);
    daObjQuake_HIO_c__ct(&l_HIO);
}

/* 023871D4 */
BOOL daObjQuakeDraw(daObjQuake_c *actor) {
    return TRUE;
}

// HD: deleting destructor for the virtual actor base.
/* 023871DC */
void daObjQuake_c__dt(daObjQuake_c *actor, s32 flags) {
    if (actor) {
        __dt(actor);
        if (flags & 1)
            operator_delete(actor);
    }
}

/* WWHD glue (tools/verify/autoglue.py): HIO, constants and external calls
  data l_HIO 1046BD08
  data l_headerStatics 1046BCEC
  data l_zero 1002EC00
  data l_cycleLength 1002EC04
  data l_one 1002EC20
  str 1002EC08 "Vibration Effect" renamed # HD: Shift-JIS "振動効果", translated source label; guest bytes unchanged.
  ext 025DA884 void fopDwTg_DrawQTo(void*)
  ext 025602CC s32 dKy_get_schbit_timer()
  ext 025602A8 u32 dKy_get_schbit()
  ext 025CB610 void dVibration_StopQuake(dVibration_c*, u32)
  ext 025CB408 void dVibration_StartQuake(dVibration_c*, s32, u32, cXyz*)
  ext 025E1988 void mDoAud_seStartQuake(u32)
  ext 028ED6F8 void QuakeFirstHeaderObject_ct(u8*)
  ext 028EAB2C void QuakeSecondHeaderObject_ct(u8*)
*/
