/* Windfall ferris wheel: GameCube reconstruction corrected against WWHD
 * disassembly. */
#include "d/actor/d_a_obj_ferris.h"
#include "gc_api/JSystem/JUTAssert.h"
#include "gc_api/m_Do/m_Do_mtx.h"
#include "gc_api/d/d_kankyo.h"
#include "gc_api/d/d_kankyo_wether.h"
const gc_tu_data gc_tu = {.safeString = 0x10028048};
using daObjFerris::Act_c;
const char Act_c::M_arcname[8] = "Skanran";
u32 param(Act_c *, u32, u32);
u8 *statusConstructor(u8 *);
u8 *cylinderConstructor(u8 *);
static dCcD_SrcSph M_sph_src = {};
static dCcD_SrcCyl M_cyl_src = {};
static FerrisVectorBits zeroVectorBits = {};
static u32 hioStorage[4] = {};
static FerrisRegistration hioRegistration = {};
static FerrisRegistration signedCharRegistration = {};
static FerrisRegistration unsignedCharRegistration = {};
static f32 minPi = {};
static f32 maxPi = {};
static u8 signedCharStorage = {};
static u8 unsignedCharStorage = {};
static cXyz gondolaOffsets[6] = {};
static u32 offsetsInitialized = {};
static cXyz collisionOrigin = {};
static u32 collisionOriginInitialized = {};
static cXyz sphereOrigin = {};
static u32 sphereOriginInitialized = {};

BOOL solidHeapCB(fopAc_ac_c *);
void ride_call_back(dBgW *, Act_c *, fopAc_ac_c *);
/* 0233DD0C */
BOOL solidHeapCB(fopAc_ac_c *self) { return static_cast<Act_c *>(self)->create_heap(); }

// HD: full-width BOOL results are retained throughout the actor wrappers.
/* 0233DA04 */
BOOL Act_c::create_heap() {
    // HD: resource requests retain their two-word SafeString temporary.
    auto resource = [&](s32 idx, s32 line, const char *assertion) {
        void *res = dComIfG_getObjectRes(M_arcname, idx);
        if (!res)
            JUTAssertion::showAssert("d_a_obj_ferris.cpp", line, assertion);
        return res;
    };
    J3DModelData *gondola = static_cast<J3DModelData *>(
        resource(dRes_INDEX_SKANRAN_BDL_SGONDOR_e, 0x182, "mdl_data_gondola != 0"));
    if (gondola)
        for (s32 i = 0; i < 5; i++)
            mpModel[i] = mDoExt_J3DModel__create(gondola, 0, 0x11020203);
    J3DModelData *wheel = static_cast<J3DModelData *>(
        resource(dRes_INDEX_SKANRAN_BDL_SKANRAN_e, 0x18B, "mdl_data_wheelbase != 0"));
    if (wheel)
        mpModel[5] = mDoExt_J3DModel__create(wheel, 0, 0x11020203);
    if (gondola && wheel) {
        mRotAngle = 0x1800;
        mGondolaWaveAngle = 0;
        init_mtx();
    }
    cBgD_t *bgGondola = static_cast<cBgD_t *>(
        resource(dRes_INDEX_SKANRAN_DZB_SGONDOR_e, 0x1A7, "bgw_data_gondola != 0"));
    if (bgGondola)
        for (s32 i = 0; i < 5; i++) {
            dBgW *bg = ferrisConstructBg(nullptr);
            mpBgW[i] = bg;
            if (bg && ferrisSetBg(bg, bgGondola, dBgW::MOVE_BG_e, &mMtx[i]))
                return FALSE;
        }
    cBgD_t *bgWheel = static_cast<cBgD_t *>(
        resource(dRes_INDEX_SKANRAN_DZB_SKANRAN_e, 0x1B7, "bgw_data_wheelbase != 0"));
    if (bgWheel) {
        dBgW *bg = ferrisConstructBg(nullptr);
        mpBgW[5] = bg;
        if (bg && ferrisSetBg(bg, bgWheel, dBgW::MOVE_BG_e, &mMtx[5]))
            return FALSE;
    }
    if (!gondola)
        return FALSE;
    for (s32 i = 0; i < 5; i++)
        if (mpModel[i] == nullptr)
            return FALSE;
    if (!wheel || mpModel[5] == nullptr || !bgGondola)
        return FALSE;
    for (s32 i = 0; i < 5; i++)
        if (mpBgW[i] == nullptr)
            return FALSE;
    return bgWheel && mpBgW[5] != nullptr;
}

/* 0233DD10 */
void ride_call_back(dBgW *bg, Act_c *self, fopAc_ac_c *rider) {
    for (s32 i = 0; i < 5; i++)
        if (self->mpBgW[i] == bg) {
            self->mRidePos.copy(rider->current.pos);
            self->mRideState[i] = 1;
            break;
        }
}

/* 0233DD60 */
BOOL Act_c::_create() {
    u32 flags = actor_condition;
    if (!(flags & FerrisInitialized)) {
        if (gabi::ea(this)) {
            ferrisConstructActor(this);
            __vtbl = FerrisActorVtable;
            ferrisConstructArray(field_0x02d4, 5, sizeof(dCcD_Stts),
                                 gabi::guest_fn((const void *)statusConstructor));
            ferrisConstructArray(field_0x0400, 5, sizeof(dCcD_Cyl),
                                 gabi::guest_fn((const void *)cylinderConstructor));
            ferrisConstructArray(field_0x09f0, 5, sizeof(dCcD_Stts),
                                 gabi::guest_fn((const void *)statusConstructor));
            ferrisConstructArray(field_0x0b1c, 5, sizeof(dCcD_Cyl),
                                 gabi::guest_fn((const void *)cylinderConstructor));
            ferrisConstructArray(field_0x110c, 5, sizeof(dCcD_Stts),
                                 gabi::guest_fn((const void *)statusConstructor));
            ferrisConstructArray(field_0x1238, 5, sizeof(dCcD_Sph), FerrisSphereConstructor);
            flags = actor_condition;
        }
        actor_condition = flags | FerrisInitialized;
    }
    s32 phase = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    if (!fopAcM_entrySolidHeap(this, solidHeapCB, 0x11A00))
        return cPhs_ERROR_e;
    J3DModel *wheelModel = mpModel[5];
    cullMtx = wheelModel ? gabi::ea(&wheelModel->mBaseTransformMtx) : 0;
    for (s32 i = 0; i < 6; i++) {
        dBgS *bgsp = dComIfG_Bgsp();
        dBgW *bg = mpBgW[i];
        bgsp->Regist(bg, this);
        reinterpret_cast<FerrisBgView *>((dBgW *)mpBgW[i])->mCorrectCallback = FerrisTypicalRotY;
        if (i < 5)
            reinterpret_cast<FerrisBgView *>((dBgW *)mpBgW[i])->mRideCallback.v =
                gabi::guest_fn((const void *)ride_call_back);
    }
    fopAcM_setCullSizeBox(this, -1400.0f, -1400.0f, -500.0f, 1400.0f, 1400.0f, 800.0f);
    cullSizeFar = 10.0f;
    s32 sw = (s32)param(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
    s8 room = home.roomNo;
    dSv_info_c *save = gabi::at<dSv_info_c>(dComIfGs_ea() + 0x20);
    if (ferrisIsSwitch(save, sw, room)) {
        mRotSpeed = 45;
        mRotState = 1;
    } else {
        mRotSpeed = 0;
        mRotState = 0;
    }
    mEventIdx_kanban_vive = ferrisEventIndex(dComIfGp_getPEvtManager(), "kanran_vive", 255);
    mEventIdx_kanban_start = ferrisEventIndex(dComIfGp_getPEvtManager(), "kanran_start", 255);
    for (s32 i = 0; i < 5; i++) {
        dCcD_Stts *status[3] = {&field_0x02d4[i], &field_0x09f0[i], &field_0x110c[i]};
        dCcD_GObjInf *coll[3] = {&field_0x0400[i], &field_0x0b1c[i], &field_0x1238[i]};
        for (u32 j = 0; j < 3; j++) {
            status[j]->Init(255, 255, this);
            if (j == 2)
                static_cast<dCcD_Sph *>(coll[j])->Set(&M_sph_src);
            else
                static_cast<dCcD_Cyl *>(coll[j])->Set(&M_cyl_src);
            coll[j]->mStts = status[j];
            // HD: these vector channels use integer loads and interleaved stores.
            FerrisVectorBits *vec = reinterpret_cast<FerrisVectorBits *>(&coll[j]->mGObjTg.mVec);
            u32 x = zeroVectorBits.x, flags = coll[j]->mGObjTg.mSPrm;
            vec->x = x;
            u32 y = zeroVectorBits.y;
            vec->y = y;
            u32 z = zeroVectorBits.z;
            coll[j]->mGObjTg.mSPrm = flags | 4;
            vec->z = z;
        }
    }
    return phase;
}

/* 0233E174 */
BOOL Act_c::_delete() {
    if (heap != nullptr)
        for (s32 i = 0; i < 6; i++) {
            dBgW *bg = mpBgW[i];
            if (bg && bg->ChkUsed()) {
                reinterpret_cast<FerrisBgView *>(bg)->mRideCallback.v = 0;
                dBgS *bgsp = dComIfG_Bgsp();
                bgsp->Release(mpBgW[i]);
            }
        }
    dComIfG_resDelete(&mPhs, M_arcname);
    return TRUE;
}

/* 0233D6B8 */
void Act_c::set_mtx(s32 idx) {

    if (!offsetsInitialized) {
        const f32 values[18] = {0.56f,     1078.13f, 162.68f, 1026.17f, 332.98f,  162.68f,
                                634.42f,   -872.69f, 162.68f, -633.3f,  -872.69f, 162.68f,
                                -1025.04f, 332.98f,  162.68f, 0,        0,        0};
        for (u32 j = 0; j < 18; j++)
            reinterpret_cast<be<f32> *>(gondolaOffsets)[j] = values[j];
        offsetsInitialized = 1;
    }
    if (idx > 5)
        return;
    if (idx < 5) {
        f32 z = current.pos.z, x = current.pos.x, y = current.pos.y;
        mDoMtx_stack_c::transS(x, y, z);
        mDoMtx_stack_c::YrotM((s16)shape_angle.y);
        mDoMtx_stack_c::ZrotM((s16)mRotAngle);
        // HD: preserve unsigned address wrapping for negative gondola indices.
        cXyz *off = gabi::at<cXyz>(gabi::ea(gondolaOffsets) + (u32)idx * sizeof(cXyz));
        x = off->x;
        y = off->y;
        z = off->z;
        mDoMtx_stack_c::transM(x, y, z);
        mDoMtx_stack_c::ZrotM((s16)-mRotAngle);
        mDoMtx_stack_c::ZrotM((s16)mGondolaWaveAngle);
    } else {
        gabi::Local<cXyz> pos;
        pos->set(current.pos.x, current.pos.y, current.pos.z);
        PSVECAdd(pos.get(), &gondolaOffsets[5], pos.get());
        f32 z = pos->z, y = pos->y, x = pos->x;
        mDoMtx_stack_c::transS(x, y, z);
        mDoMtx_stack_c::YrotM((s16)shape_angle.y);
        mDoMtx_stack_c::ZrotM((s16)mRotAngle);
    }
    f32 values[12];
    for (u32 j = 0; j < 12; j++)
        values[j] = reinterpret_cast<be<f32> *>(mDoMtx_stack_c::get())[j];
    J3DModel *model = mpModel[idx];
    for (u32 j = 0; j < 12; j++)
        reinterpret_cast<be<f32> *>(&model->mBaseTransformMtx)[j] = values[j];
    MTXCopy(mDoMtx_stack_c::get(), &mMtx[idx]);
}

/* 0233D97C */
void Act_c::init_mtx() {
    for (s32 i = 0; i < 6; i++) {
        f32 z = scale.z, x = scale.x;
        J3DModel *model = mpModel[i];
        f32 y = scale.y;
        model->mBaseScale.z = z;
        model->mBaseScale.x = x;
        model->mBaseScale.y = y;
        set_mtx(i);
        mpModel[i]->calc();
    }
}

/* 0233E22C */
bool Act_c::now_event(s16 id) { return mEventState != 0 && mEventIdx == id; }

/* 0233E204 */
BOOL Act_c::set_event(s16 id) {
    if (mEventState != 0)
        return FALSE;
    mEventIdx = id;
    mEventState = 1;
    return TRUE;
}

/* 0233E668 */
void Act_c::exe_event() {
    if (mEventState == 1) {
        if (eventInfo.mCommand == 2)
            mEventState = 2;
        else {
            s16 id = mEventIdx;
            ferrisOrderEvent(this, id, 255, 65535, 0, 1);
            eventInfo.mCondition |= dEvtCnd_UNK2_e;
        }
    } else if (mEventState == 2) {
        s16 id = mEventIdx;
        if (ferrisEventEnd(dComIfGp_getPEvtManager(), id)) {
            dComIfGp_event_reset();
            mEventIdx = -1;
            mEventState = 0;
        }
    }
}

/* 0233E254 */
void Act_c::angle_mng() {
    s16 angle = (s16)(mGondolaWaveTimer + 500);
    mGondolaWaveTimer = angle;
    f32 sine = cM_ssin(angle);
    mGondolaWaveAngle = (s16)gabi::ftoi(380.0f * sine);
}

/* 0233E2A0 */
void Act_c::rot_mng() {
    s16 state = mRotState;
    switch (state) {
    case 0: {
        mRotSpeed = 0;
        mRotAngle = 0x1800;
        s32 sw = (s32)param(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
        dSv_info_c *save = gabi::at<dSv_info_c>(dComIfGs_ea() + 0x20);
        s8 room = home.roomNo;
        if (!ferrisIsSwitch(save, sw, room))
            break;
        cXyz *wind = dKyw_get_wind_vec();
        f32 x = wind->x, z = wind->z;
        s32 angle = ferrisAtan2s(x, z);
        if (angle == -32768) {
            if (set_event(mEventIdx_kanban_start)) {
                mRotTimer = 0;
                mRotSpeed = 0;
                mRotState = 6;
                ferrisSound(0x806);
            }
        } else if (set_event(mEventIdx_kanban_vive)) {
            mRotSpeed = 0;
            mRotState = 2;
            mRotTimer = 0;
            sw = (s32)param(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
            save = gabi::at<dSv_info_c>(dComIfGs_ea() + 0x20);
            room = home.roomNo;
            ferrisOffSwitch(save, sw, room);
        }
        break;
    }
    case 2:
    case 6: {
        s16 timer = (s16)(mRotTimer + 1);
        if (timer > 20) {
            mRotTimer = 0;
            mRotState = (s16)(mRotState + 1);
        } else
            mRotTimer = timer;
        break;
    }
    case 3:
    case 4:
    case 5:
    case 7:
    case 8:
    case 9: {
        s16 timer = mRotTimer;
        if (timer < 11)
            mRotSpeed = 4;
        else if ((state != 5 && state != 9) || !(mFrameTimer & 3))
            mRotSpeed = (s16)(mRotSpeed - 1);
        s16 speed = mRotSpeed;
        mRotTimer = (s16)(timer + 1);
        if (speed < 0) {
            s16 currentState = mRotState;
            mRotSpeed = 0;
            mRotTimer = 0;
            switch (currentState) {
            case 3:
            case 4:
            case 7:
            case 8:
                mRotState = (s16)(currentState + 1);
                break;
            case 5:
                mRotState = 10;
                break;
            case 9:
                mRotState = 1;
                break;
            }
        }
        break;
    }
    case 10: {
        s16 angle = mRotAngle;
        mRotSpeed = -5;
        if (angle < 0x1800 || !now_event(mEventIdx_kanban_vive)) {
            mRotSpeed = 0;
            mRotState = 0;
        }
    } break;
    case 1: {
        dSv_info_c *save = gabi::at<dSv_info_c>(dComIfGs_ea() + 0x20);
        save->mSavedata.mEvent.onEventBit(0x2104);
        s16 frame = mFrameTimer, id = mEventIdx_kanban_start, speed = mRotSpeed;
        if (!(frame & 3)) {
            speed = (s16)(speed + 1);
            mRotSpeed = speed;
        }
        s32 limit = now_event(id) ? 67 : 45;
        if (speed > limit)
            mRotSpeed = limit;
        angle_mng();
        break;
    }
    }
}

/* 0233E9C4 */
void Act_c::set_collision() {
    gabi::Local<cXyz> point, offset;
    for (s32 i = 0; i < 5; i++) {
        if (!collisionOriginInitialized) {
            collisionOrigin.set(0, 0, 0);
            collisionOriginInitialized = 1;
        }
        PSMTXMultVec(&mMtx[i], &collisionOrigin, point.get());
        if (!(point->y > 1820.0f)) {
            dCcD_Cyl *cyl = &field_0x0400[i];
            cyl->SetR(50.0f);
            cyl->SetH(195.0f);
            offset->set(0, -100, -135);
            PSMTXMultVec(&mMtx[i], offset.get(), point.get());
            cyl->SetC(point.get());
            cCcS *ccsp = dComIfG_Ccsp();
            ferrisRegisterCollision(ccsp, cyl);
            cyl = &field_0x0b1c[i];
            cyl->SetR(50.0f);
            cyl->SetH(148.0f);
            offset->set(0, -56, 127);
            PSMTXMultVec(&mMtx[i], offset.get(), point.get());
            cyl->SetC(point.get());
            ccsp = dComIfG_Ccsp();
            ferrisRegisterCollision(ccsp, cyl);
        }
    }
    const Vec3f offsets[5] = {{-993.54f, -325.78f, 37.63f},
                              {-616.86f, 844.24f, 37.63f},
                              {612.3f, 847.55f, 37.63f},
                              {995.28f, -320.43f, 37.63f},
                              {2.82f, -1045.59f, 37.63f}};
    gabi::Local<Mtx34> matrix;
    for (s32 i = 0; i < 5; i++) {
        if (!sphereOriginInitialized) {
            sphereOrigin.set(0, 0, 0);
            sphereOriginInitialized = 1;
        }
        f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
        mDoMtx_stack_c::transS(x, y, z);
        mDoMtx_stack_c::YrotM((s16)shape_angle.y);
        mDoMtx_stack_c::ZrotM((s16)mRotAngle);
        mDoMtx_stack_c::transM(offsets[i].x, offsets[i].y, offsets[i].z);
        mDoMtx_stack_c::ZrotM((s16)-mRotAngle);
        s16 angle = mRideWaveAngle[i], wave = mGondolaWaveAngle;
        mDoMtx_stack_c::ZrotM((s16)(angle + wave));
        MTXCopy(mDoMtx_stack_c::get(), matrix.get());
        PSMTXMultVec(matrix.get(), &sphereOrigin, point.get());
        if (!(point->y > 1820.0f)) {
            dCcD_Sph *sph = &field_0x1238[i];
            sph->SetC(point.get());
            sph->SetR(30.0f);
            ferrisRegisterCollision(dComIfG_Ccsp(), sph);
        }
    }
}

/* 0233E770 */
void Act_c::make_lean() {
    gabi::Local<cXyz> origin, offset, pt0, pt1, delta;
    // HD: keep the outgoing linkage area below live locals: normalizeRS stores LR at SP+4.
    gabi::Local<cXyz> linkage;
    origin->set(0, 0, 0);
    offset->set(0, -100, -135);
    for (s32 i = 0; i < 5; i++) {
        s16 target = 0;
        if (mRideState[i] == 1) {
            PSMTXMultVec(&mMtx[i], origin.get(), pt0.get());
            PSMTXMultVec(&mMtx[i], offset.get(), pt1.get());
            f32 x0 = pt0->x, x1 = pt1->x, z1 = pt1->z, z0 = pt0->z;
            f32 rz = mRidePos.z, rx = mRidePos.x;
            f32 dx = rx - x0, dz = rz - z0;
            delta->set(x1 - x0, 0, z1 - z0);
            ferrisNormalizeRS(delta.get());
            f32 nz = delta->z, z = pt0->z, nx = delta->x;
            rz = mRidePos.z;
            x0 = pt0->x;
            rx = mRidePos.x;
            f32 a = gabi::fmadds(-nz, x0, nx * z), b = gabi::fmadds(-nz, rx, nx * rz);
            f32 cross = gabi::fmsubs(nx, dz, nz * dx);
            f32 height = std::fabs(b - a) / 162.0f;
            target = (s16)gabi::ftoi(height * (cross < 0.0f ? 550.0f : -550.0f));
        }
        mRideWaveTarget[i] = target;
        cLib_addCalcAngleS2(&mRideWaveAngle[i], target, 4, 0x1000);
        mRideState[i] = 0;
    }
}

/* 0233EDE0 */
BOOL Act_c::_execute() {
    s16 speed = mRotSpeed;
    f32 volume = ((f32)speed / 45.0f) * 100.0f;
    mRotAngle = (s16)(mRotAngle + speed);
    if (volume > 100.0f)
        volume = 100.0f;
    ferrisSoundVolume(0x107D, &current.pos, (u32)gabi::ftoi(volume));
    rot_mng();
    exe_event();
    make_lean();
    for (s32 i = 0; i < 6; i++) {
        set_mtx(i);
        dBgW *bg = mpBgW[i];
        bg->Move();
    }
    set_collision();
    mFrameTimer = (s16)(mFrameTimer + 1);
    return TRUE;
}

/* 0233EF4C */
BOOL Act_c::_draw() {
    settingTevStruct(dKy_getEnvlight(), 1, &current.pos, &tevStr);
    dComIfGd_setListBG();
    for (s32 i = 0; i < 6; i++) {
        dScnKy_env_light_c *env = dKy_getEnvlight();
        J3DModel *model = mpModel[i];
        setLightTevColorType(env, model, &tevStr);
        model = mpModel[i];
        mDoExt_modelUpdateDL(model, 0);
    }
    dComIfGd_setList();
    return TRUE;
}

/* 0233F004 */
BOOL methodCreate(Act_c *self) { return self->_create(); }

/* 0233F008 */
BOOL methodDelete(Act_c *self) { return self->_delete(); }

/* 0233F00C */
BOOL methodExecute(Act_c *self) { return self->_execute(); }

/* 0233F010 */
BOOL methodDraw(Act_c *self) { return self->_draw(); }

/* 0233F2CC */
BOOL isDelete(Act_c *self) { return TRUE; }

// HD: compiler-generated and template code of this translation unit.
/* 0233F014 */
void initStatics() {
    for (u32 i = 0; i < 4; i++)
        hioStorage[i] = 0;
    ferrisRegisterDestructor(&hioRegistration);
    minPi = -3.1415927410125732f;
    maxPi = 3.1415927410125732f;
    ferrisConstructSignedChar(&signedCharStorage);
    ferrisRegisterDestructor(&signedCharRegistration);
    ferrisConstructUnsignedChar(&unsignedCharStorage);
    ferrisRegisterDestructor(&unsignedCharRegistration);
}

/* 0233F0A8 */
void emptyDestructor(u8 *self, s32 flags) {
    if (self && (flags & 1))
        ferrisDelete(self);
}

/* 0233F0BC */
u8 *statusConstructor(u8 *self) {
    if (!self)
        self = static_cast<u8 *>(ferrisAllocate(sizeof(dCcD_Stts)));
    if (self)
        dCcD_Stts_ct(reinterpret_cast<dCcD_Stts *>(self));
    return self;
}

/* 0233F124 */
u8 *cylinderConstructor(u8 *self) {
    if (!self)
        self = static_cast<u8 *>(ferrisAllocate(sizeof(dCcD_Cyl)));
    if (self) {
        dCcD_Cyl *cyl = reinterpret_cast<dCcD_Cyl *>(self);
        FerrisShapeView *shape = reinterpret_cast<FerrisShapeView *>(cyl->mShapeAttr);
        ferrisConstructGObj(cyl);
        shape->mShapeVtable = FerrisShapeAttrVtable;
        shape->mAabVtable = FerrisAabVtable;
        ferrisConstructCyl(&cyl->mCyl);
        cyl->__vtbl_hitinf = FerrisCylinderHitVtable;
        // HD: geometry vtable precedes the final shape vtable store.
        cyl->mCyl.__vtbl = FerrisCylinderGeometryVtable;
        shape->mShapeVtable = FerrisCylinderShapeVtable;
    }
    return self;
}

/* 0233F1B0 */
void emptyVirtual(u8 *self) {}

/* 0233F1B4 */
void actorDestructor(Act_c *self, s32 flags) {
    if (!self)
        return;
    ferrisDestroyArray(self->field_0x1238, 5, sizeof(dCcD_Sph), FerrisSphereDestructor, 0, 0);
    ferrisDestroyArray(self->field_0x110c, 5, sizeof(dCcD_Stts), FerrisStatusDestructor, 0, 0);
    ferrisDestroyArray(self->field_0x0b1c, 5, sizeof(dCcD_Cyl), FerrisCylinderDestructor, 0, 0);
    ferrisDestroyArray(self->field_0x09f0, 5, sizeof(dCcD_Stts), FerrisStatusDestructor, 0, 0);
    ferrisDestroyArray(self->field_0x0400, 5, sizeof(dCcD_Cyl), FerrisCylinderDestructor, 0, 0);
    ferrisDestroyArray(self->field_0x02d4, 5, sizeof(dCcD_Stts), FerrisStatusDestructor, 0, 0);
    ferrisDestroyActor(self, 0);
    if (flags & 1)
        ferrisDelete(self);
}

/* 0233F2D4 */
u32 param(Act_c *self, u32 width, u32 shift) {
    u32 mask = (width & 32) ? 0 : (1u << (width & 31));
    u32 bits = (shift & 32) ? 0 : ((u32)self->mParameters >> (shift & 31));
    return bits & (mask - 1);
}
/* WWHD glue (tools/verify/autoglue.py)
data gondolaOffsets 104698A0 nocheck
data offsetsInitialized 104698E8 nocheck
data collisionOrigin 10469888 nocheck
data collisionOriginInitialized 104698EC nocheck
data sphereOrigin 10469894 nocheck
data sphereOriginInitialized 104698F0 nocheck
ext 025D7A58 void ferrisOrderEvent(fopAc_ac_c*, s16, s32, s32, s32, s32)
ext 025440C8 s32 ferrisEventEnd(dEvent_manager_c*, s16)
ext 025BA0C0 s32 ferrisIsSwitch(dSv_info_c*, s32, s32)
ext 025B9F7C void ferrisOffSwitch(dSv_info_c*, s32, s32)
ext 0200E240 void ferrisRegisterCollision(cCcS*, dCcD_GObjInf*)
str 10028250 "Skanran"
str 1002818C "d_a_obj_ferris.cpp"
str 1002815C "mdl_data_gondola != 0"
str 1002812C "mdl_data_wheelbase != 0"
str 10028174 "bgw_data_gondola != 0"
str 10028144 "bgw_data_wheelbase != 0"
str 100281B4 "kanran_vive"
str 100281C0 "kanran_start"
ext 024F23F4 dBgW* ferrisConstructBg(dBgW*)
ext 0200A030 s32 ferrisSetBg(dBgW*, cBgD_t*, u32, Mtx34*)
data M_sph_src 10028080 nocheck
data M_cyl_src 100280C0 nocheck
data zeroVectorBits 101FFBA8 nocheck
ext 025D4ED0 void ferrisConstructActor(fopAc_ac_c*)
ext 028EFFD0 void ferrisConstructArray(void*, u32, u32, u32)
ext 02543F10 s32 ferrisEventIndex(dEvent_manager_c*, const char*, s32)
data hioStorage 10469878 nocheck
data hioRegistration 101C91E0 nocheck
data signedCharRegistration 101C91EC nocheck
data unsignedCharRegistration 101C91F8 nocheck
data minPi 1046986C nocheck
data maxPi 10469870 nocheck
data signedCharStorage 10469874 nocheck
data unsignedCharStorage 10469875 nocheck
ext 028F026C void ferrisRegisterDestructor(FerrisRegistration*)
ext 028ED6F8 void ferrisConstructSignedChar(be<u8>*)
ext 028EAB2C void ferrisConstructUnsignedChar(be<u8>*)
ext 0273AF40 void ferrisDelete(void*)
ext 0273AD10 void* ferrisAllocate(u32)
ext 02515FB8 void ferrisConstructGObj(dCcD_GObjInf*)
ext 02018590 void ferrisConstructCyl(cM3dGCyl*)
ext 028F0164 void ferrisDestroyArray(void*, u32, u32, u32, s32, s32)
ext 025D50BC void ferrisDestroyActor(fopAc_ac_c*, s32)
ext 0201B47C void ferrisNormalizeRS(cXyz*)
ext 025E1A04 void ferrisSoundVolume(u32, cXyz*, u32)
ext 025E1988 void ferrisSound(u32)
ext 020195B0 s32 ferrisAtan2s(f32, f32)
*/
