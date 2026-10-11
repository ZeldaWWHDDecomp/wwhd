/* Great Sea enemy cannon: GameCube methods and asserted HD layouts.
 * Original memory order, fused arithmetic and native register widths are verified unchanged. */
#include "bindings.h"
#include "d/actor/d_a_obj_canon.h"
using Actor = daObj_Canon_c;
static J3DModel* canonCurrentModel;
static Mtx34 canonCurrentMatrix;
static void* canonResources;
static canonHD::SaveBlock* canonSave;
static u32 canonDebugGuard;
static canonHD::DebugObject canonDebugObject;
static canonHD::DebugObject canonDebugType;
static canonHD::HeaderZeros canonHeaderZeros;
static f32 canonNegativePi;
static f32 canonPositivePi;
static u8 canonAngleTag;
static u8 canonByteTag;
static canonHD::DestructorRecord canonRecord0;
static canonHD::DestructorRecord canonRecord1;
static canonHD::DestructorRecord canonRecord2;
static dCcD_SrcSph canonSphereSource;
canonHD::JointData* canonGetJoint(void*);
void canonCopyMatrix(void*, void*);
void canonConstructAngles(csXyz*, s16, s16, s16);
void canonRotateZXY(void*, s16, s16, s16);
void canonRotateX(void*, s16);
void canonTranslate(f32, f32, f32);
J3DModelData* canonGetResource(void*, SafeString*, s32);
void canonAssert(const char*, s32, const char*);
J3DModel* canonCreateModel(void*, u32, u32);
void canonSetTranslation(void*, f32, f32, f32);
s32 canonIsSwitch(void*, s32, s32);
dPath* canonGetRoomPath(s32, s32);
void canonSetPath(void*, void*);
void canonCalcModel(void*);
void canonSetCullBox(void*, f32, f32, f32, f32, f32, f32);
void canonInitStatus(void*, u8, u8, void*);
void canonSetSphereSource(void*, void*);
void canonConstructActor(void*);
void canonConstructSphere(void*);
void canonConstructStatus(void*);
void canonConstructGameStatus(void*);
void canonConstructFollow(void*, u32, u32);
s32 canonLoadResource(void*, const char*);
s32 canonEnterHeap(void*, u32, u32);
void canonDeleteResource(void*, const char*);
void canonDeleteSound(void*);
void canonSetSphereRadius(void*, f32);
void canonSetSphereCenter(void*, void*);
dComIfG_inf_c* canonGetGameInfo();
void canonRegisterCollider(void*, void*);
void canonAddCalcAngle(void*, s16, s16, s16);
void canonTransformPosition(void*, void*, void*);
s32 canonCalcTimer(void*);
void canonEndFollow(void*);
void canonInitializeDebug(void*, void*, u32);
dScnKy_env_light_c* canonGetEnvironment();
void canonSetLighting(void*, s32, void*, void*);
void canonSetModelLighting(void*, void*, void*);
void canonEntryModel(void*);
void canonSetParticle(void*, u32, u16, void*, void*, void*, u8, void*, s8, void*, void*, u32);
void canonMoveGameStatus(void*);
dCcD_GObjInf* canonGetTargetHit(void*);
s32 canonGetReverb(s32);
void canonStartSound(u32, void*, u32, s32);
s32 canonCheckInside(void*, void*);
void canonSubtractPosition(void*, void*, void*);
f32 canonSquareMagnitude(void*);
f32 canonSqrt(f32);
s16 canonSearchPlayerAngle(void*, void*);
s16 canonTargetYaw(void*, void*);
s16 canonTargetPitch(void*, void*);
u32 canonMakeBombParameters(s32, s32, s32);
fopAc_ac_c* canonCreateBomb(s16, u32, void*, s8, void*, void*, s32, u32, u32);
void canonSetNoGravityTime(void*, s16);
void canonStartCannonSound(u32, void*);
void canonOnSwitch(void*, s32, s32);
void canonAddCalc(void*, f32, f32, f32);
void* canonAllocate(u32);
void canonRegisterDestructor(void*);
void canonConstructAngleTag(void*);
void canonConstructByteTag(void*);
void canonFree(void*);
void canonDestroyStatus(void*, s32);
void canonDestroySphere(void*, s32);
void canonDestroyActor(void*, s32);
static CanonModeEntry canonModeTable[5] = {
    {&daObj_Canon_c::modeWaitInit, &daObj_Canon_c::modeWait, "WAIT"},
    {&daObj_Canon_c::modeAttackInit, &daObj_Canon_c::modeAttack, "ATTACK"},
    {&daObj_Canon_c::modeDeleteInit, &daObj_Canon_c::modeDelete, "DELETE"},
    {&daObj_Canon_c::modeSwWaitInit, &daObj_Canon_c::modeSwWait, "SW_WAIT"},
    {&daObj_Canon_c::modeAppearInit, &daObj_Canon_c::modeAppear, "APPEAR"},
};

static daObj_Canon_HIO_c canonHIO;
static Mtx34 canonMatrix;

static Mtx34* animationMatrix(J3DModel* model, u32 joint) {
    canonHD::MatrixBuffer* buffer =
        reinterpret_cast<canonHD::MatrixBuffer*>(model->mpMtxBuffer.get());
    Mtx34* matrices = buffer->mpAnmMtx;
    u16 flags = buffer->mFlags;
    buffer->mFlags = flags | canonHD::MODEL_ANM_MTX_DIRTY;
    return matrices + joint;
}

static s32 nodeControl_CB(J3DNode*, s32);
static s32 createHeap_CB(Actor*);

static void particle(u16 id, cXyz* pos, csXyz* angle, cXyz* scale, dPa_followEcallBack* callback,
                     GXColor* color) {
    dComIfG_inf_c* play = canonGetGameInfo();
    dPa_control_c* control = play->play.mParticle;
    canonSetParticle(control, u32(0), id, pos, angle, scale, u8(255), callback, s8(-1), color,
                     color, u32(0));
}

/* 0232FFD4 */
static s32 nodeControl_CB(J3DNode* node, s32 timing) {
    if (timing == 0) {
        J3DModel* model = canonCurrentModel;
        Actor* actor = static_cast<Actor*>(model->getUserArea());
        if (actor)
            actor->_nodeControl(node, model);
    }
    return 1;
}

// HD: the accessor reaches joint data; matrix flags are dirtied after capturing the buffer pointer.
/* 0232FDF0 */
void daObj_Canon_c::_nodeControl(J3DNode* node, J3DModel* model) {
    canonHD::JointData* joint = canonGetJoint(node);
    u32 index = joint->mJntNo;
    Mtx34* matrix = animationMatrix(static_cast<J3DModel*>(model), index);
    canonCopyMatrix(matrix, &canonMatrix);
    gabi::Local<csXyz> rot;
    canonConstructAngles(rot.get(), s16(0), s16(0), s16(0));
    s16 x, y, z;
    if (REG_S(12, 1)) {
        x = REG_S(12, 2);
        y = REG_S(12, 3);
        z = REG_S(12, 4);
    } else {
        x = -s16(field_0x2CA);
        y = rot->y;
        z = -s16(field_0x2C6);
    }
    rot->x = x;
    rot->y = y;
    rot->z = z;
    canonRotateZXY(&canonMatrix, x, y, z);
    s16 phase = s16(s16(field_0x468) + REG_S(12, 2) + 0x1830);
    s16 recoil = field_0x46A;
    field_0x468 = phase;
    s32 factor = s32(REG_S(12, 5)) + 5;
    s16 shake = s16(gabi::ftoi(f32(s32(recoil) * factor) * cM_ssin(phase)));
    canonRotateX(&canonMatrix, shake);
    f32 rise = field_0x46C;
    f32 tx = REG_F(12, 3), tz = REG_F(12, 5);
    canonTranslate(tx, -rise, tz);
    canonCopyMatrix(&canonMatrix, &canonCurrentMatrix);
    matrix = animationMatrix(static_cast<J3DModel*>(model), index);
    mtx_copy(matrix, &canonMatrix);
}

/* 023300FC */
static s32 createHeap_CB(Actor* a) {
    return a->_createHeap();
}

/* 0233001C */
s32 daObj_Canon_c::_createHeap() {
    gabi::Local<SafeString> name;
    name->mStringTop = gabi::guest_addr("WallBom");
    name->__vtbl = 0x10026A78;
    void* resources = canonResources;
    J3DModelData* data =
        canonGetResource(resources, name.get(), s32(canonHD::dRes_INDEX_WALLBOM_BDL_WALLBOM_e));
    if (!data)
        canonAssert("d_a_obj_canon.cpp", s32(0x115), "modelData != (0)");
    J3DModel* model = canonCreateModel(data, u32(0x80000), u32(0x11000022));
    mpModel = model;
    if (!model)
        return 0;
    model->mUserArea = gabi::ea(this);
    u32 count = data->mJointNum;
    canonHD::Joint* joints = reinterpret_cast<canonHD::ModelData*>(data)->mJointNodes;
    if (count > canonHD::WALLBOM_JNT_SHOT_e)
        joints += canonHD::WALLBOM_JNT_SHOT_e;
    joints->mCallBack = gabi::guest_fn(reinterpret_cast<const void*>(nodeControl_CB));
    return 1;
}

/* 02331418 */
void daObj_Canon_c::setEffect(u16 id) {
    GXColor* color = id == canonHD::ID_IT_SN_MJTAIHOU_HAHEN00 ? &tevStr.mColorK0 : nullptr;
    dPa_followEcallBack* callback =
        id == canonHD::ID_IT_JN_MJTAIHOU_SMOKE01 ? &field_0x470 : nullptr;
    particle(id, &current.pos, &shape_angle, &scale, callback, color);
    if (id == canonHD::ID_IT_JN_MJTAIHOU_SMOKE01)
        field_0x484 = 0xE6;
}

// HD: collision type compares and byte health wrapping retain the original widths.
/* 02330A98 */
s32 daObj_Canon_c::checkTgHit() {
    canonMoveGameStatus(&mStts.__vtbl_gstts);
    if (canonHIO.field_0x05) {
        health = 0;
        modeProc(PROC_INIT_e, MODE_DELETE);
        return 1;
    }
    if (canonCalcTimer(&field_0x440))
        return 0;
    dCcD_GObjInf* hit = canonGetTargetHit(&mSph);
    gabi::Local<cXyz> pos;
    pos->copy(mSph.mGObjTg.mHitPos);
    canonGetTargetHit(&mSph);
    if (!hit)
        return 0;
    u32 type = hit->mAtType;
    if (type == canonHD::AT_TYPE_BOMB) {
        field_0x440 = 5;
        health = u8(u8(health) - 2);
    } else if (type == canonHD::AT_TYPE_BOOMERANG) {
        s8 room = current.roomNo;
        field_0x46A = 0x190;
        s32 reverb = canonGetReverb(s32(room));
        canonStartSound(u32(canonHD::JA_SE_LK_W_WEP_HIT), &eyePos, u32(0x20), reverb);
        field_0x440 = 5;
        health = u8(u8(health) - 1);
    } else
        return 0;
    particle(canonHD::ID_AK_JN_CRITICALHITFLASH, pos.get(), nullptr, nullptr, nullptr, nullptr);
    if (s8(u8(health)) > 0) {
        modeProc(PROC_INIT_e, MODE_WAIT);
        return 1;
    }
    dComIfG_inf_c* play = canonGetGameInfo();
    fopAc_ac_c* player = play->play.mPlayerInfo[0].mpPlayer;
    gabi::Local<cXyz> scale;
    scale->x = 2.f;
    scale->y = 2.f;
    scale->z = 2.f;
    particle(canonHD::ID_AK_JN_CRITICALHIT, pos.get(), &player->shape_angle, scale.get(), nullptr,
             nullptr);
    s32 reverb = canonGetReverb(s32(current.roomNo));
    canonStartSound(u32(canonHD::JA_SE_LK_LAST_HIT), &eyePos, u32(0), reverb);
    modeProc(PROC_INIT_e, MODE_DELETE);
    return 1;
}

// HD: allocation failure leaves the cannon idle.
/* 02331298 */
void daObj_Canon_c::attackCannon() {
    gabi::Local<csXyz> angle;
    angle->x = s16(s16(shape_angle.x) - s16(field_0x2CA));
    angle->y = s16(s16(shape_angle.y) + s16(field_0x2C6));
    angle->z = shape_angle.z;
    u32 params = canonMakeBombParameters(s32(4), s32(1), s32(1));
    s8 room = tevStr.mRoomNo;
    fopAc_ac_c* bomb = canonCreateBomb(s16(fpcNm_BOMB_e), params, &field_0x450, room, angle.get(),
                                       nullptr, s32(-1), u32(0), u32(0));
    if (!bomb)
        return;
    s16 noGravity = canonHIO.field_0x28;
    canonSetNoGravityTime(bomb, noGravity);
    s16 pitch = angle->x;
    bomb->speedF = cM_scos(pitch) * canonHIO.field_0x20;
    pitch = angle->x;
    bomb->speed.y = -(cM_ssin(pitch) * canonHIO.field_0x20);
    bomb->gravity = canonHIO.field_0x24;
    field_0x46A = 0xC8;
    canonStartCannonSound(u32(canonHD::JA_SE_LK_SHIP_CANNON_FIRE), &field_0x450);
}

/* 0233113C */
void daObj_Canon_c::lockonCannon() {
    gabi::Local<cXyz> difference, horizontal;
    canonSubtractPosition(&current.pos, difference.get(), &field_0x2CC);
    horizontal->x = difference->x;
    horizontal->y = 0.f;
    horizontal->z = difference->z;
    f32 sq = canonSquareMagnitude(horizontal.get());
    f32 distance = canonSqrt(sq);
    s16 yaw = canonTargetYaw(&current.pos, &field_0x2CC);
    field_0x2C4 = s16(yaw - s16(shape_angle.y));
    f32 threshold = canonHIO.field_0x2C;
    s16 bias = 0;
    if (distance > threshold) {
        bias = s16(gabi::ftoi(distance - threshold));
        s16 maximum = canonHIO.field_0x30;
        if (bias > maximum)
            bias = maximum;
    }
    s16 pitch = canonTargetPitch(&current.pos, &field_0x2CC);
    pitch = s16(pitch + bias);
    field_0x2C8 = pitch;
    s16 low = canonHIO.field_0x34, high = canonHIO.field_0x32;
    if (pitch < low)
        pitch = low;
    else if (pitch > high)
        pitch = high;
    field_0x2C8 = pitch;
    yaw = field_0x2C4;
    low = canonHIO.field_0x38;
    high = canonHIO.field_0x36;
    if (yaw < low)
        yaw = low;
    else if (yaw > high)
        yaw = high;
    field_0x2C4 = yaw;
}

/* 0233069C */
void daObj_Canon_c::setAttention() {
    f32 z = field_0x45C.z, y = field_0x45C.y, x = field_0x45C.x;
    attention_info.position.z = z;
    attention_info.position.y = y;
    attention_info.position.x = x;
    f32 offset = canonHIO.field_0x0C;
    eyePos.z = z;
    eyePos.y = y;
    eyePos.x = x;
    attention_info.position.y = y + offset;
    eyePos.y = y + canonHIO.field_0x10;
}

/* 023306E4 */
void daObj_Canon_c::setCollision() {
    f32 radius = (REG_F(12, 0) + 80.f) * f32(scale.x);
    canonSetSphereRadius(&mSph.mSph, radius);
    canonSetSphereCenter(&mSph.mSph, &field_0x45C);
    s32 mode = mCurMode;
    if (mode != MODE_DELETE && mode != MODE_SW_WAIT) {
        dComIfG_inf_c* play = canonGetGameInfo();
        canonRegisterCollider(&play->play.mCcS, &mSph);
    }
}

/* 02330224 */
void daObj_Canon_c::setMtx() {
    f32 x = scale.x, y = scale.y;
    J3DModel* model = mpModel;
    f32 z = scale.z;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    x = current.pos.x;
    y = current.pos.y;
    z = current.pos.z;
    canonSetTranslation(&canonMatrix, x, y, z);
    s16 rx = shape_angle.x, ry = shape_angle.y, rz = shape_angle.z;
    canonRotateZXY(&canonMatrix, rx, ry, rz);
    model = mpModel;
    mtx_copy(&model->mBaseTransformMtx, &canonMatrix);
}

/* 02330A88 */
void daObj_Canon_c::modeWaitInit() {
    field_0x444 = canonHIO.field_0x3E;
}

/* 02330D3C */
void daObj_Canon_c::modeWait() {
    if (checkTgHit())
        return;
    dComIfG_inf_c* play = canonGetGameInfo();
    fopAc_ac_c* player = play->play.mPlayerInfo[0].mpPlayer;
    play = canonGetGameInfo();
    if (play->play.mPlayerStatus[0][0] & canonHD::daPyStts0_UNK1000000_e) {
        field_0x488 = 0;
        field_0x444 = s32(canonHIO.field_0x3E) * 10;
    }
    play = canonGetGameInfo();
    canonHD::Ship* ship = reinterpret_cast<canonHD::Ship*>(play->play.mpPlayerPtr[2].get());
    if (ship && (ship->mState & canonHD::SHIP_SHOOT_CANNON))
        field_0x488 = s32(u32(s32(field_0x488)) + 1);
    if (canonCalcTimer(&field_0x444))
        return;
    if (!canonCheckInside(&field_0x298, &player->current.pos)) {
        field_0x2C8 = 0;
        field_0x2C4 = 0;
        field_0x444 = canonHIO.field_0x3E;
        return;
    }
    s16 prediction = s16(gabi::ftoi(canonHIO.field_0x08));
    gabi::Local<cXyz> difference, horizontal;
    canonSubtractPosition(&current.pos, difference.get(), &field_0x2CC);
    horizontal->x = difference->x;
    horizontal->y = 0.f;
    horizontal->z = difference->z;
    f32 sq = canonSquareMagnitude(horizontal.get());
    f32 distance = canonSqrt(sq);
    f32 threshold = canonHIO.field_0x40;
    if (distance > threshold)
        prediction = s16(
            gabi::ftoi(gabi::fmadds(distance - threshold, canonHIO.field_0x44, f32(prediction))));
    play = canonGetGameInfo();
    player = play->play.mPlayerInfo[0].mpPlayer;
    field_0x2CC.copy(*&player->current.pos);
    if (!canonCheckInside(&field_0x298, &field_0x2CC))
        return;
    play = canonGetGameInfo();
    if (play->play.mPlayerStatus[0][0] &
        (canonHD::daPyStts0_UNK1000000_e | canonHD::daPyStts0_SWIM_e)) {
        field_0x488 = 0;
        prediction = s16(gabi::ftoi(f32(prediction) + 3000.f));
    } else if (distance > canonHIO.field_0x40 && s32(field_0x488) < 6)
        prediction = s16(gabi::ftoi(
            gabi::fmadds(f32(s32(u32(6) - u32(s32(field_0x488)))), 200.f, f32(prediction))));
    play = canonGetGameInfo();
    player = play->play.mPlayerInfo[0].mpPlayer;
    s16 angle = canonSearchPlayerAngle(this, player);
    f32 x = field_0x2CC.x;
    field_0x2CC.x = gabi::fnmsubs(f32(prediction), cM_ssin(angle), x);
    f32 z = field_0x2CC.z;
    field_0x2CC.z = gabi::fnmsubs(f32(prediction), cM_scos(angle), z);
    field_0x444 = canonHIO.field_0x3E;
    modeProc(PROC_INIT_e, MODE_ATTACK);
}

/* 0233112C */
void daObj_Canon_c::modeAttackInit() {
    field_0x444 = canonHIO.field_0x3C;
}

/* 023313B4 */
void daObj_Canon_c::modeAttack() {
    if (!checkTgHit()) {
        lockonCannon();
        if (!canonCalcTimer(&field_0x444)) {
            attackCannon();
            modeProc(PROC_INIT_e, MODE_WAIT);
        }
    }
}

/* 02331558 */
void daObj_Canon_c::modeDeleteInit() {
    u8 sw = field_0x295;
    field_0x46A = 0;
    if (sw != 255) {
        s8 room = current.roomNo;
        canonHD::SaveBlock* save = canonSave;
        canonOnSwitch(&save->mSaveInfo, s32(sw), s32(room));
    }
    setEffect(canonHD::ID_IT_SN_MJTAIHOU_HAHEN00);
    setEffect(canonHD::ID_IT_SN_MJTAIHOU_SMOKE00);
    setEffect(canonHD::ID_IT_SN_MJTAIHOU_SENKO00);
    setEffect(canonHD::ID_IT_JN_MJTAIHOU_SMOKE01);
    actor_status = u32(actor_status) & ~(u32(canonHD::MAP_ICON_MASK) | u32(fopAcStts_SHOWMAP_e));
}

/* 023318D8 */
void daObj_Canon_c::modeDelete() {}

/* 023315F8 */
void daObj_Canon_c::modeSwWaitInit() {
    actor_status = u32(actor_status) & ~(u32(canonHD::MAP_ICON_MASK) | u32(fopAcStts_SHOWMAP_e));
    field_0x46C = 120.f;
}

/* 02331614 */
void daObj_Canon_c::modeSwWait() {
    u8 sw = field_0x296;
    if (sw != 255) {
        s8 room = current.roomNo;
        canonHD::SaveBlock* save = canonSave;
        if (canonIsSwitch(&save->mSaveInfo, s32(sw), s32(room)))
            modeProc(PROC_INIT_e, MODE_APPEAR);
    }
}

// HD: SHOWMAP accompanies the cannon map icon.
/* 02331678 */
void daObj_Canon_c::modeAppearInit() {
    u32 status = actor_status;
    field_0x46A = 0x258;
    field_0x46C = 120.f;
    actor_status = (status & ~(u32(canonHD::MAP_ICON_MASK) | u32(fopAcStts_SHOWMAP_e))) |
                   (u32(canonHD::MAP_ICON_CANNON) | u32(fopAcStts_SHOWMAP_e));
}

/* 023316A0 */
void daObj_Canon_c::modeAppear() {
    canonAddCalc(&field_0x46C, 0.f, 0.1f, 1.f);
    if (f32(field_0x46C) < 2.f) {
        field_0x46C = 0.f;
        modeProc(PROC_INIT_e, MODE_WAIT);
    }
}

/* 02330178 */
void daObj_Canon_c::modeProc(s32 proc, s32 mode) {
    if (proc == PROC_INIT_e) {
        mCurMode = mode;
        (this->*canonModeTable[u32(mode)].init)();
    } else if (proc == PROC_EXEC_e) {
        s32 current = mCurMode;
        (this->*canonModeTable[u32(current)].run)();
    }
}

/* 02330760 */
s32 daObj_Canon_c::_execute() {
    canonAddCalcAngle(&field_0x46A, s16(0), s16(10), s16(10));
    s16 pitch = field_0x2C8;
    canonAddCalcAngle(&field_0x2CA, pitch, s16(6), s16(0x300));
    s16 yaw = field_0x2C4;
    canonAddCalcAngle(&field_0x2C6, yaw, s16(6), s16(0x300));
    setAttention();
    setCollision();
    modeProc(PROC_EXEC_e, MODE_COUNT);
    canonCalcModel(mpModel.get());
    gabi::Local<cXyz> muzzle, attention;
    f32 actorScale = scale.x, height = canonHIO.field_0x18, x = canonHIO.field_0x14;
    muzzle->x = x;
    muzzle->y = height * actorScale;
    muzzle->z = canonHIO.field_0x1C;
    Mtx34* matrix = animationMatrix(mpModel, canonHD::WALLBOM_JNT_SHOT_e);
    canonTransformPosition(matrix, muzzle.get(), &field_0x450);
    actorScale = scale.x;
    f32 y = REG_F(12, 1) + 60.f;
    x = REG_F(12, 0);
    J3DModel* model = mpModel;
    attention->x = x;
    attention->z = REG_F(12, 2);
    attention->y = y * actorScale;
    matrix = animationMatrix(model, canonHD::WALLBOM_JNT_SHOT_e);
    canonTransformPosition(matrix, attention.get(), &field_0x45C);
    if (field_0x470.mpEmitter.v && canonCalcTimer(&field_0x484) == 0)
        canonEndFollow(&field_0x470);
    setMtx();
    return 0;
}

// HD: debugDraw initializes the debug service instead of drawing the GC debug colors.
/* 023308EC */
void daObj_Canon_c::debugDraw() {
    if (!canonDebugGuard) {
        canonDebugGuard = 1;
        canonInitializeDebug(&canonDebugObject, &canonDebugType, u32(4));
        if (!canonDebugGuard) {
            canonDebugGuard = 1;
            canonInitializeDebug(&canonDebugObject, &canonDebugType, u32(4));
        }
    }
}

/* 02330974 */
s32 daObj_Canon_c::_draw() {
    if (canonHIO.debug_draw)
        debugDraw();
    s32 mode = mCurMode;
    J3DModel* model = mpModel;
    J3DModelData* data = model->mModelData;
    u32 count = data->mJointNum;
    canonHD::Joint* joints = reinterpret_cast<canonHD::ModelData*>(data)->mJointNodes;
    if (count > canonHD::WALLBOM_JNT_SHOT_e)
        joints += canonHD::WALLBOM_JNT_SHOT_e;
    J3DMaterial* mesh = joints->mMesh;
    J3DShape* shape = mesh->mShape;
    shape->mVisible = (mode == MODE_DELETE || mode == MODE_SW_WAIT) ? 0 : 1;
    dScnKy_env_light_c* env = canonGetEnvironment();
    canonSetLighting(env, s32(0), &current.pos, &tevStr);
    env = canonGetEnvironment();
    model = mpModel;
    canonSetModelLighting(env, model, &tevStr);
    canonEntryModel(mpModel.get());
    return 1;
}

/* 02330304 */
void daObj_Canon_c::createInit() {
    u8 sw = field_0x295;
    s32 mode = MODE_WAIT;
    if (sw != 255 && canonIsSwitch(&canonSave->mSaveInfo, s32(sw), s32(current.roomNo)))
        mode = MODE_DELETE;
    else {
        sw = field_0x296;
        if (sw != 255 && !canonIsSwitch(&canonSave->mSaveInfo, s32(sw), s32(current.roomNo)))
            mode = MODE_SW_WAIT;
    }
    modeProc(PROC_INIT_e, mode);
    s8 room = current.roomNo;
    u8 path = field_0x294;
    health = 2;
    max_health = 2;
    dPath* route = canonGetRoomPath(s32(path), s32(room));
    field_0x2B0 = route;
    canonSetPath(&field_0x298, route);
    setMtx();
    canonCalcModel(mpModel.get());
    J3DModel* model = mpModel;
    f32 actorScale = scale.x;
    cullMtx = model ? gabi::ea(&model->mBaseTransformMtx) : 0;
    canonSetCullBox(this, -100.f * actorScale, -100.f * actorScale, -10.f * actorScale,
                    100.f * actorScale, 100.f * actorScale, 150.f * actorScale);
    cullSizeFar = 10.f;
    canonInitStatus(&mStts, u8(255), u8(255), this);
    canonSetSphereSource(&mSph, &canonSphereSource);
    mSph.mStts = &mStts;
}

/* 02330100 */
void daObj_Canon_c::getArg() {
    u32 param = mParameters;
    field_0x297 = u8(param);
    field_0x296 = u8(param >> 8);
    field_0x294 = u8(param >> 16);
    field_0x295 = u8(param >> 24);
    if (u8(param) != 255) {
        f32 actorScale = f32(gabi::fmadd(f64(u8(param)), 0.1, 1.0));
        scale.x = actorScale;
        scale.y = actorScale;
        scale.z = actorScale;
    }
}

/* 02330524 */
s32 daObj_Canon_c::_create() {
    u32 cond = actor_condition;
    if (!(cond & fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            canonConstructActor(this);
            __vtbl = 0x10026A90;
            canonConstructSphere(&mSph);
            canonConstructStatus(&mStts);
            canonConstructGameStatus(&mStts.__vtbl_gstts);
            mStts.__vtbl = 0x1004AE88;
            mStts.__vtbl_gstts = 0x1004AEC0;
            canonConstructFollow(&field_0x470, u32(0), u32(0));
            cond = actor_condition;
        }
        actor_condition = cond | fopAcCnd_INIT_e;
    }
    s32 result = canonLoadResource(&mPhs, "WallBom");
    if (result == cPhs_COMPLEATE_e) {
        getArg();
        if (u8(field_0x294) == 255 ||
            !canonEnterHeap(this, gabi::guest_fn(reinterpret_cast<const void*>(createHeap_CB)),
                            u32(0x8C0)))
            return cPhs_ERROR_e;
        createInit();
    }
    return result;
}

/* 02330640 */
s32 daObj_Canon_c::_delete() {
    canonDeleteResource(&mPhs, "WallBom");
    field_0x470.remove();
    canonDeleteSound(&field_0x450);
    return 1;
}

/* 0233063C */
static s32 daObj_CanonCreate(Actor* a) {
    return a->_create();
}

/* 02330698 */
static s32 daObj_CanonDelete(Actor* a) {
    return a->_delete();
}

/* 023308E8 */
static s32 daObj_CanonExecute(Actor* a) {
    return a->_execute();
}

/* 02330A84 */
static s32 daObj_CanonDraw(Actor* a) {
    return a->_draw();
}

/* 023318D0 */
static s32 daObj_CanonIsDelete(void* a) {
    return 1;
}

// HD: compiler-generated and template code of this translation unit.
/* 0233172C */
static void* daObj_Canon_HIO_c_ct(void* self) {
    daObj_Canon_HIO_c* p = static_cast<daObj_Canon_HIO_c*>(self);
    if (!p) {
        p = static_cast<daObj_Canon_HIO_c*>(canonAllocate(u32(0x4C)));
        if (!p)
            return nullptr;
    }
    p->field_0x24 = -1.1f;
    p->field_0x3A = 0x2800;
    p->field_0x32 = 0x2328;
    p->field_0x20 = 90.f;
    p->field_0x36 = 0x2328;
    p->field_0x08 = 150.f;
    p->field_0x28 = 0x16;
    p->field_0x18 = 180.f;
    p->field_0x3E = 30;
    p->field_0x44 = 0.2f;
    p->field_0x3C = 30;
    p->__vtbl = 0x10026AA0;
    p->field_0x2C = 5000.f;
    p->field_0x40 = 2000.f;
    p->field_0x38 = -0x2328;
    p->field_0x48 = 0;
    p->debug_draw = 0;
    p->field_0x05 = 0;
    p->field_0x30 = 0x3000;
    p->field_0x1C = 0.f;
    p->field_0x14 = 0.f;
    p->field_0x34 = -0x2328;
    return p;
}

/* 0233181C */
static void __sinit_d_a_obj_canon_cpp() {
    canonHeaderZeros.words[2] = 0;
    canonHeaderZeros.words[0] = 0;
    canonHeaderZeros.words[3] = 0;
    canonHeaderZeros.words[1] = 0;
    canonRegisterDestructor(&canonRecord0);
    canonNegativePi = -3.1415927410125732f;
    canonPositivePi = 3.1415927410125732f;
    canonConstructAngleTag(&canonAngleTag);
    canonRegisterDestructor(&canonRecord1);
    canonConstructByteTag(&canonByteTag);
    canonRegisterDestructor(&canonRecord2);
    daObj_Canon_HIO_c_ct(&canonHIO);
}

/* 023318BC */
static void daObj_Canon_HIO_c_dt(void* self, s32 flags) {
    if (self && (flags & 1))
        canonFree(self);
}

/* 023318DC */
static void daObj_Canon_c_dt(void* p, s32 flags) {
    if (p != nullptr) {
        canonDestroyStatus(&static_cast<Actor*>(p)->mStts, 2); /* dCcD_Stts */
        canonDestroySphere(&static_cast<Actor*>(p)->mSph, 2);  /* dCcD_Sph */
        canonDestroyActor(p, 0);                               /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(p);
    }
}

/* 02331948 */
static void canon_SafeString_assureTermination(void* p) {}
/* WWHD glue (tools/verify/autoglue.py)
str 10026BE0
str 10026AB8
str 10026ACC
data canonModeTable 10026AF8
data canonCurrentModel 104B462C
data canonCurrentMatrix 104B4868
data canonResources 101F4F28
data canonSave 101F84DC
data canonDebugGuard 101FDA50
data canonDebugObject 101FEBF4
data canonDebugType 10026A70
data canonHeaderZeros 10469450
data canonNegativePi 10469444
data canonPositivePi 10469448
data canonAngleTag 1046944C
data canonByteTag 1046944D
data canonRecord0 101C8960
data canonRecord1 101C896C
data canonRecord2 101C8978
data canonSphereSource 10026BEC
data canonHIO 10469460
data canonMatrix 1048D0CC
ext 027F7878 canonHD::JointData* canonGetJoint(void*)
ext 028E90D4 void canonCopyMatrix(void*, void*)
ext 0201A478 void canonConstructAngles(csXyz*, s16, s16, s16)
ext 025F1B48 void canonRotateZXY(void*, s16, s16, s16)
ext 025F1BF4 void canonRotateX(void*, s16)
ext 025F24E0 void canonTranslate(f32, f32, f32)
ext 026066C4 J3DModelData* canonGetResource(void*, SafeString*, s32)
ext 0273AA24 void canonAssert(const char*, s32, const char*)
ext 025E38E0 J3DModel* canonCreateModel(void*, u32, u32)
ext 028E93CC void canonSetTranslation(void*, f32, f32, f32)
ext 025BA0C0 s32 canonIsSwitch(void*, s32, s32)
ext 025AAF88 dPath* canonGetRoomPath(s32, s32)
ext 0259E730 void canonSetPath(void*, void*)
ext 027F4D5C void canonCalcModel(void*)
ext 025D674C void canonSetCullBox(void*, f32, f32, f32, f32, f32, f32)
ext 02515F14 void canonInitStatus(void*, u8, u8, void*)
ext 0251677C void canonSetSphereSource(void*, void*)
ext 025D4ED0 void canonConstructActor(void*)
ext 025166F0 void canonConstructSphere(void*)
ext 0200BD2C void canonConstructStatus(void*)
ext 02515DA0 void canonConstructGameStatus(void*)
ext 025A5894 void canonConstructFollow(void*, u32, u32)
ext 02520460 s32 canonLoadResource(void*, const char*)
ext 025D63E8 s32 canonEnterHeap(void*, u32, u32)
ext 025204C8 void canonDeleteResource(void*, const char*)
ext 025E1B34 void canonDeleteSound(void*)
ext 02018C8C void canonSetSphereRadius(void*, f32)
ext 02018D40 void canonSetSphereCenter(void*, void*)
ext 025200D4 dComIfG_inf_c* canonGetGameInfo()
ext 0200E240 void canonRegisterCollider(void*, void*)
ext 0200F428 void canonAddCalcAngle(void*, s16, s16, s16)
ext 028E8F64 void canonTransformPosition(void*, void*, void*)
ext 0211D2F8 s32 canonCalcTimer(void*)
ext 025A5AC8 void canonEndFollow(void*)
ext C000A848 void canonInitializeDebug(void*, void*, u32)
ext 02555D0C dScnKy_env_light_c* canonGetEnvironment()
ext 025626A4 void canonSetLighting(void*, s32, void*, void*)
ext 02562F5C void canonSetModelLighting(void*, void*, void*)
ext 025E2E5C void canonEntryModel(void*)
ext 025A847C void canonSetParticle(void*, u32, u16, void*, void*, void*, u8, void*, s8, void*, void*, u32)
ext 02515E50 void canonMoveGameStatus(void*)
ext 02516300 dCcD_GObjInf* canonGetTargetHit(void*)
ext 02520540 s32 canonGetReverb(s32)
ext 025E1A40 void canonStartSound(u32, void*, u32, s32)
ext 0259F220 s32 canonCheckInside(void*, void*)
ext 0201ADE0 void canonSubtractPosition(void*, void*, void*)
ext 028E8DD0 f32 canonSquareMagnitude(void*)
ext 028F4384 f32 canonSqrt(f32)
ext 025D6894 s16 canonSearchPlayerAngle(void*, void*)
ext 0200F93C s16 canonTargetYaw(void*, void*)
ext 0200F974 s16 canonTargetPitch(void*, void*)
ext 020CB8D8 u32 canonMakeBombParameters(s32, s32, s32)
ext 025D5928 fopAc_ac_c* canonCreateBomb(s16, u32, void*, s8, void*, void*, s32, u32, u32)
ext 020CB89C void canonSetNoGravityTime(void*, s16)
ext 025E19CC void canonStartCannonSound(u32, void*)
ext 025B9E38 void canonOnSwitch(void*, s32, s32)
ext 0200ED84 void canonAddCalc(void*, f32, f32, f32)
ext 0273AD10 void* canonAllocate(u32)
ext 028F026C void canonRegisterDestructor(void*)
ext 028ED6F8 void canonConstructAngleTag(void*)
ext 028EAB2C void canonConstructByteTag(void*)
ext 0273AF40 void canonFree(void*)
ext 02515860 void canonDestroyStatus(void*, s32)
ext 02515AE8 void canonDestroySphere(void*, s32)
ext 025D50BC void canonDestroyActor(void*, s32)
*/
