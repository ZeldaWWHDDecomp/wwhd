// Cannon minigame actor.
#include "d/actor/d_a_canon.h"
#include "bindings.h"
static canon::ArchiveName M_arcname;
static canon::ArchiveName M_textureArcname;
static f32 l_bombX;
static f32 l_bombY;
static f32 l_bombSpacing;
static f32 l_shipX;
static f32 l_shipY;
static f32 l_shipSpacingAndRange;
static f32 l_targetRange;
static f32 l_collisionDistance;
static f32 l_collisionRadius;
static f32 l_bombScale;
static f32 l_shipScale;
static f32 l_cannonScale;
static daCanon_c* l_cannon;
static fopAc_ac_c* l_gameNpc;
static u8 l_gameResult;
static void* l_resourceControl;
static canon::Meter* l_meter;
static canon::SaveRoot* l_save;
static canon::Model* l_currentModel;
static s16 l_zeroAngleX;
static s16 l_zeroAngleY;
static s16 l_zeroAngleZ;
static u32 l_lightInit3;
static u32 l_lightInit2;
static u32 l_lightInit1;
static u32 l_lightInit0;
static f32 l_defaultLightX;
static f32 l_defaultLightY;
static f32 l_lightX;
static f32 l_lightY;
static f32 l_lightInfluence;
static f32 l_lightDistance1;
static f32 l_lightPower1;
static f32 l_lightPower0;
static f32 l_lightDistance0;
static canon::Light l_pointLight;
static canon::Wind l_pointWind;

static canon::ColumnAngles l_columnAngles;
static canon::CylinderSource l_sourceBody;
static canon::CylinderSource l_sourceBall;
static canon::CutTable l_startCuts;
static canon::CutTable l_endCuts;
static canon::ScreenPosition l_startScreen;
static canon::ScreenPosition l_endScreen;
static canon::DestructorRecord l_lightDestructor;
static canon::DestructorRecord l_lightFlagDestructor;
static canon::DestructorRecord l_windFlagDestructor;
static u8 l_lightFlag;
static u8 l_windFlag;
static u32 l_safeStringVtable;
static u32 l_actorVtable;

enum : u32 {
    PROC_CANON_TARGET = 0x1C9,
    PROC_BOMB = 0x126,
    CREATE_ACTOR_CALLBACK = 0x02542D74,
    PROC_WAIT = 0x0210AAD4,
    PROC_GAME_START = 0x0210AF4C,
    PROC_GAME = 0x0210AE44,
    PROC_FIRE = 0x0210B228,
    PROC_PAUSE = 0x0210B5A4,
    PROC_END = 0x0210B5D8
};

static Mtx34* matrix() { return mDoMtx_stack_c::get(); }
static f32 sine(s16 angle) { return cM_ssin(angle); }
static f32 cosine(s16 angle) { return cM_scos(angle); }
static dComIfG_inf_c* play() { return static_cast<dComIfG_inf_c*>(canon_getPlay()); }
static void set_process(daCanon_c* a, u32 target) {
    a->mProcess.i = -1;
    a->mProcess.f = target;
    a->mProcess.d = 0;
}
static bool firing(daCanon_c* a) {
    s16 slot = a->mProcess.i;
    return slot == -1 &&
           (slot == 0 || ((s16)a->mProcess.d == 0 && (u32)a->mProcess.f == PROC_FIRE));
}
static void sound(void* a, u32 id) {
    u32 addr = gabi::ea(a);
    if (addr && addr + 0x37Cu) {
        auto* actor = static_cast<fopAc_ac_c*>(a);
        s8 room = actor->current.roomNo;
        s32 reverb = canon_getReverb(room);
        canon_startSound(id, &actor->eyePos, 0, reverb);
    }
}
static void render_light(daCanon_c* a, cXyz* p, bool ball) {
    void* light = canon_getEnvironment();
    canon_setLightTev(light, 0, p, &a->tevStr);
    light = canon_getEnvironment();
    J3DModel* m = ball ? (J3DModel*)a->mpModel2 : (J3DModel*)a->mpModel1;
    canon_applyLightTev(light, m, &a->tevStr);
    m = ball ? (J3DModel*)a->mpModel2 : (J3DModel*)a->mpModel1;
    mDoExt_modelUpdateDL(m, 0);
}
static void display_list(void* object) {
    auto* p = reinterpret_cast<canon::DrawList*>(&play()->drawlist);
    canon_addDisplayList(p, &p->mp2DOpaListCur, &p->mp2DOpaListEnd, object);
}
static void finish_game(dComIfG_inf_c* p) {
    u16 flags = p->play.mMinigameFlags;
    p->play.mMinigameResult = 0;
    p->play.mMiniGameType = 0;
    p->play.mMinigameFlags = flags ^ 4;
}

/* 02109054 */
BOOL daCanon_nodeCallBack(void* node, s32 timing) {
    if (timing == 0) {
        auto* model = l_currentModel.get();
        auto* joint = static_cast<canon::Joint*>(canon_getJoint(node));
        u16 n = joint->mJntNo;
        canon::MatrixBuffer* buffer = model->mpMatrixBuffer;
        u16 flags = buffer->mFlags;
        Mtx34* matrices = buffer->mpMatrices;
        daCanon_c* actor = model->mpActor;
        buffer->mFlags = flags | 0x10;
        PSMTXCopy(gabi::at<Mtx34>(gabi::ea(matrices) + sizeof(Mtx34) * n), matrix());
        mDoMtx_YrotM(matrix(), (s16)actor->shape_angle.x);
        buffer = model->mpMatrixBuffer;
        flags = buffer->mFlags;
        buffer->mFlags = flags | 0x10;
        matrices = buffer->mpMatrices;
        mtx_copy(gabi::at<Mtx34>(gabi::ea(matrices) + sizeof(Mtx34) * n), matrix());
    }
    return 1;
}
/* 02109160 */
BOOL target_createCB(void* a) {
    auto* actor = static_cast<fopAc_ac_c*>(a);
    actor->actor_status = (u32)actor->actor_status & ~0x180u;
    return 1;
}
/* 02109174 */
void* daCanon_BreakTarget(void* target, void* parent) {
    if (fopAc_IsActor(target) && target &&
        (s16) static_cast<fopAc_ac_c*>(target)->mProcName == PROC_CANON_TARGET) {
        u32 link = (u32) static_cast<fopAc_ac_c*>(target)->parentActorID;
        u32 id = parent ? (u32) static_cast<fopAc_ac_c*>(parent)->mBsPcId : 0xFFFFFFFFu;
        if (id == link) {
            u32 params = canon_makeBombParams(0, 0, 0);
            canon_createActor(PROC_BOMB, params, &static_cast<fopAc_ac_c*>(target)->current.pos, -1,
                              (void*)nullptr, (void*)nullptr, -1, CREATE_ACTOR_CALLBACK);
        }
    }
    return nullptr;
}
/* 02109220 */

BOOL daCanon_c::CreateHeap() {
    canon::Local<be<u32>[2]> name1;
    (*name1)[0] = gabi::ea(&M_arcname);
    (*name1)[1] = gabi::ea(&l_safeStringVtable);
    void* controller = l_resourceControl.get();
    void* data = canon_getObjectResource(controller, name1.get(), BOMBER_BDL_CANON);
    if (!data)
        canon_assert("d_a_canon.cpp", 849, "modelData != (0)");
    J3DModel* m = mDoExt_J3DModel__create(static_cast<J3DModelData*>(data), 0, 0x11020203u);
    this->mpModel1 = m;
    if (!m)
        return 0;
    canon::Local<be<u32>[2]> name2;
    (*name2)[1] = gabi::ea(&l_safeStringVtable);
    controller = l_resourceControl.get();
    (*name2)[0] = gabi::ea(&M_arcname);
    void* data2 = canon_getObjectResource(controller, name2.get(), BOMBER_BDL_CANNONBALL);
    if (!data2)
        canon_assert("d_a_canon.cpp", 857, "modelData2 != (0)");
    void* names = canon_getJointNames(data);
    u32 offset = static_cast<canon::JointNames*>(names)->mNameTableOffset;
    u32 table = offset ? gabi::ea(names) + 0x10 + offset : 0;
    s8 index = canon_findJointName(gabi::at<void>(table), "canon");
    u32 number = (u16)(s16)index;
    auto* modelData = static_cast<canon::ModelData*>(data);
    u32 size = modelData->mJointCount, joint = gabi::ea(modelData->mpJoints.get());
    if (number < size)
        joint += number * 0x1C;
    gabi::at<canon::JointEntry>(joint)->mCallback.v =
        gabi::guest_fn((const void*)&daCanon_nodeCallBack);
    m = this->mpModel1;
    reinterpret_cast<canon::Model*>(m)->mpActor = this;
    m = mDoExt_J3DModel__create(static_cast<J3DModelData*>(data2), 0, 0x11020203u);
    this->mpModel2 = m;
    if (!m)
        return 0;
    canon::Local<be<u32>[2]> textures[8];
    const s16 indices[8] = {BOMBER_BTI_BOMB1,    BOMBER_BTI_BOMB2,    BOMBER_BTI_SHIP1,
                            BOMBER_BTI_SHIP2,    BOMBER_BTI_BATTERY1, BOMBER_BTI_BATTERY2,
                            BOMBER_BTI_BATTERY3, BOMBER_BTI_BATTERY4};
    void* resources[8];
    for (int i = 0; i < 2; i++) {
        (*textures[i])[1] = gabi::ea(&l_safeStringVtable);
        controller = l_resourceControl.get();
        (*textures[i])[0] = gabi::ea(&M_textureArcname);
        resources[i] = canon_getObjectResource(controller, textures[i].get(), indices[i]);
    }
    for (int i = 0; i < 10; i++) {
        void* object = operator_new(0x2C);
        if (object)
            object = canon_constructIcon(object);
        this->mBombIcons[i] = static_cast<dDlst_2DObject_c*>(object);
        if (!object)
            return 0;
        if (!canon_initIcon(object, resources[0], resources[1]))
            return 0;
    }
    for (int i = 2; i < 4; i++) {
        (*textures[i])[1] = gabi::ea(&l_safeStringVtable);
        controller = l_resourceControl.get();
        (*textures[i])[0] = gabi::ea(&M_textureArcname);
        resources[i] = canon_getObjectResource(controller, textures[i].get(), indices[i]);
    }
    // HD: fixes the GameCube copy-paste loop: the ship array has five entries.
    for (int i = 0; i < 5; i++) {
        void* object = operator_new(0x2C);
        if (object)
            object = canon_constructIcon(object);
        this->mShipIcons[i] = static_cast<dDlst_2DObject_c*>(object);
        if (!object)
            return 0;
        if (!canon_initIcon(object, resources[2], resources[3]))
            return 0;
    }
    for (int i = 4; i < 8; i++) {
        (*textures[i])[1] = gabi::ea(&l_safeStringVtable);
        controller = l_resourceControl.get();
        (*textures[i])[0] = gabi::ea(&M_textureArcname);
        resources[i] = canon_getObjectResource(controller, textures[i].get(), indices[i]);
    }
    void* object = operator_new(0x5C);
    if (object)
        object = canon_constructBattery(object);
    this->mpBattery = static_cast<dDlst_2DBattery_c*>(object);
    if (!object)
        return 0;
    return canon_initBattery(object, resources[4], resources[5], resources[6], resources[7]) != 0;
}
/* 02109558 */
BOOL CheckCreateHeap(daCanon_c* a) { return a->CreateHeap(); }
/* 0210955C */

void daCanon_c::set_mtx() {
    f32 scale = (f32)l_cannonScale + 0.65f;
    f32 y = gabi::fmadds(100.f, scale, (f32)this->current.pos.y);
    PSMTXTrans(matrix(), (f32)this->current.pos.x, y, (f32)this->current.pos.z);
    mDoMtx_YrotM(matrix(), (s16)((s16)this->shape_angle.y + 0x8000));
    if (firing(this)) {
        f32 angle = (f32)(s16)this->mRecoilAmplitude * sine(this->mRecoilPhase);
        mDoMtx_YrotM(matrix(), (s16)gabi::ftoi(angle));
        angle = (f32)(s16)this->mRecoilAmplitude * cosine(this->mRecoilPhase);
        mDoMtx_XrotM(matrix(), (s16)gabi::ftoi(angle));
        canon_translate(0.f, (f32)this->mRecoil, 0.f);
    }
    auto* model = reinterpret_cast<canon::Model*>(this->mpModel1.get());
    model->mScale.x = scale;
    model->mScale.y = scale;
    model->mScale.z = scale;
    model = reinterpret_cast<canon::Model*>(this->mpModel1.get());
    mtx_copy(&model->mBaseTransform, matrix());
    PSMTXCopy(matrix(), &this->mBaseMtx);
}
/* 021097B0 */
void daCanon_c::wait_proc_init() {
    this->attention_info.flags = 10;
    set_process(this, PROC_WAIT);
}
/* 02109FF0 */
void daCanon_c::bul_set_mtx(cXyz* p) {
    auto* model = reinterpret_cast<canon::Model*>(this->mpModel2.get());
    model->mScale.x = 1.f;
    model->mScale.z = 1.f;
    model->mScale.y = 1.f;
    PSMTXTrans(matrix(), (f32)p->x, (f32)p->y, (f32)p->z);
    model = reinterpret_cast<canon::Model*>(this->mpModel2.get());
    mtx_copy(&model->mBaseTransform, matrix());
}
/* 0210A0B8 */
void daCanon_c::getGridPos(cXyz* out, s32 column, s32 row) {
    s16 angle = (s16)((s16)this->home.angle.y +
                      (s16)*gabi::at<be<s16>>(gabi::ea(&l_columnAngles) + 2u * (u32)column));
    f32 distance = gabi::fmadds((f32)row, 1500.f, 10500.f) + (f32)l_targetRange;
    f32 x = gabi::fmadds(distance, sine(angle), (f32)this->current.pos.x),
        z = gabi::fmadds(distance, cosine(angle), (f32)this->current.pos.z);
    f32 y = daSea_calcWave(x, z);
    if (!out)
        out = (cXyz*)operator_new(12);
    if (out) {
        out->y = y;
        out->z = z;
        out->x = x;
    }
}
/* 0210A1EC */
void daCanon_c::getBulEndPos(cXyz* out, s16 yaw, s16 pitch) {
    f32 ratio = (f32)((s32)(s16)this->home.angle.x + 0x1000 - pitch) * 0.000244140625f;
    f32 distance = gabi::fmadds(ratio * 1500.f, 7.f, 10500.f) + (f32)l_shipSpacingAndRange;
    f32 x = gabi::fmadds(distance, sine(yaw), (f32)this->current.pos.x),
        z = gabi::fmadds(distance, cosine(yaw), (f32)this->current.pos.z);
    f32 y = daSea_calcWave(x, z);
    if (!out)
        out = (cXyz*)operator_new(12);
    if (out) {
        out->y = y;
        out->z = z;
        out->x = x;
    }
}
/* 0210A31C */
BOOL daCanon_c::createCheck(s32 column, s32 row, s32 count) {
    // HD: unsigned effective-address arithmetic preserves out-of-range guest index wrapping.
    for (s32 n = count; n != 0;) {
        n = (s32)((u32)n - 1);
        if ((u32)column ==
            (u8)gabi::at<canon::TargetCell>(gabi::ea(&mTargetGrid) + 2u * (u32)n)->mColumn)
            return 0;
    }
    auto* cell = gabi::at<canon::TargetCell>(gabi::ea(&mTargetGrid) + 2u * (u32)count);
    cell->mColumn = (u8)column;
    cell->mRow = (u8)row;
    return 1;
}
/* 0210A380 */
void daCanon_c::createTargetObj() {
    for (int i = 0; i < 5;) {
        s32 column = gabi::ftoi(cM_rndF(8.f));
        s32 row = gabi::ftoi(cM_rndF(8.f));
        if (!this->createCheck(column, row, i))
            continue;
        canon::Local<cXyz> grid, position;
        this->getGridPos(grid.get(), column, row);
        position->copy(*grid);
        u32 id = (u32)this->mBsPcId;
        u32 child = canon_createChild(PROC_CANON_TARGET, id, 0x037F003Fu, position.get(), -1,
                                      (void*)nullptr, (void*)nullptr, -1, target_createCB);
        this->mTargetIds[i] = child;
        i++;
    }
}
/* 0210A494 */
Pair32 daCanon_c::breakAllObj() {
    return canon_searchTargets(daCanon_BreakTarget, this); /* fpcM_Search tail call: r3:r4 */
}
/* 0210AA30 */
void daCanon_c::game_start_proc_init() {
    this->mTimer = 60;
    this->shape_angle.y = this->home.angle.y;
    this->shape_angle.x = 0x2AAA;
    void* npc = l_gameNpc.get();
    if (npc)
        sound(npc, 0x8A8);
    set_process(this, PROC_GAME_START);
}
/* 0210AD20 */
void daCanon_c::game_proc_init() { set_process(this, PROC_GAME); }
/* 0210AF4C */
void daCanon_c::game_start_proc() {
    s16 timer = this->mTimer;
    this->mTimer = (s16)(timer - 1);
    if (timer < 0)
        this->game_proc_init();
}
/* 0210AD40 */
void daCanon_c::fire_proc_init() {
    f32 ratio =
        (f32)((s32)(s16)this->home.angle.x + 0x1000 - (s16)this->shape_angle.x) * 0.000244140625f;
    f32 frames = gabi::fmadds(ratio * 8.f, 7.f, 64.f);
    this->mProcess.f = PROC_FIRE;
    this->mRecoil = -20.f;
    this->mProcess.d = 0;
    this->mProcess.i = -1;
    s16 duration = (s16)gabi::ftoi(frames);
    s16 remaining = (s16)((s16)this->mRemainingShots - 1);
    this->mFlightFrames = duration;
    this->mFlightDuration = duration;
    this->mRecoilPhase = 0;
    this->mRecoilAmplitude = 0x400;
    this->mRemainingShots = remaining;
    dDlst_2DObject_c* icon =
        gabi::at<gptr<dDlst_2DObject_c>>(gabi::ea(&mBombIcons) +
                                         sizeof(gptr<dDlst_2DObject_c>) * (u32)(s32)remaining)
            ->get();
    icon->mCurrentNo = 1;
    auto* root = l_meter.get();
    canon::Hud* ui = root->mpGameInfo;
    canon_setShotsUsed(ui, 10 - (s16)this->mRemainingShots);
}
/* 0210B7D0 */
void canon_local_delete(void* object, u32 flags) {
    if (object && (flags & 1))
        operator_delete(object);
}
/* 0210B7E4 */
BOOL canon_is_delete(void* a) { return 1; }

/* 021097D8 */
s32 daCanon_c::_create() {
    u32 condition = (u32)this->actor_condition;
    if (!(condition & 8)) {
        if (this) {
            canon_constructActor(this);
            this->__vtbl = gabi::ea(&l_actorVtable);
            dCcD_Stts_ct(&mStts1);
            dCcD_Cyl_ct(&mCyl1, 0x1000C3DC);
            dCcD_Stts_ct(&mStts2);
            dCcD_Cyl_ct(&mCyl2, 0x1000C3DC);
            condition = (u32)this->actor_condition;
        }
        this->actor_condition = condition | 8;
    }
    s32 phase = canon_loadResource(&this->mPhase, "Bomber");
    if (phase == 4) {
        if (!canon_createHeap(this, CheckCreateHeap, 0x62E0))
            return 5;
        this->set_mtx();
        PSMTXCopy(matrix(), &this->mBaseMtx);
        auto* model = reinterpret_cast<canon::Model*>(this->mpModel1.get());
        this->cullMtx = model ? gabi::ea(&model->mBaseTransform) : 0;
        model = reinterpret_cast<canon::Model*>(this->mpModel1.get());
        PSMTXCopy(model ? &model->mBaseTransform : nullptr, &this->mBaseMtx);
        canon_initStatus(&this->mStts1, 255, 255, this);
        canon_setCylinder(&this->mCyl1, &l_sourceBody);
        this->mCyl1.mStts = &this->mStts1;
        canon_setCylinder(&this->mCyl2, &l_sourceBall);
        this->mCyl2.mStts = &this->mStts1;
        this->shape_angle.x = 0x2AAA;
        this->home.angle.x = 0x2000;
        this->wait_proc_init();
        s16 pitch = this->shape_angle.x, yaw = this->shape_angle.y;
        f32 x = this->current.pos.x, y = this->current.pos.y, z = this->current.pos.z;
        this->mUnused1 = 0;
        this->mUnused2 = 3;
        this->mMuzzlePos.z = z;
        this->mMuzzlePos.y = y;
        this->mMuzzlePos.x = x;
        this->mMuzzlePos.x = gabi::fmadds(200.f * sine(pitch), sine(yaw), x);
        this->mMuzzlePos.z = gabi::fmadds(200.f * sine(pitch), cosine(yaw), z);
        this->mMuzzleAngle.x = (s16)l_zeroAngleX;
        this->mMuzzleAngle.y = (s16)l_zeroAngleY;
        this->mMuzzleAngle.z = (s16)l_zeroAngleZ;
        this->mMuzzleAngle.y = yaw;
        l_cannon = this;
    }
    return phase;
}
/* 02109A78 */
BOOL daCanon_c::_delete() {
    canon_deleteResource(&this->mPhase, "Bomber");
    dComIfG_inf_c* p = play();
    if (p->play.mMiniGameType == 3) {
        p = play();
        finish_game(p);
    }
    canon_deleteSound(&this->mBallPos);
    l_cannon = nullptr;
    return 1;
}
/* 02109AF8 */
BOOL daCanon_c::_execute() {
    if ((u8)this->mEffectActive) {
        canon_clearDiceWeather(0, 0, 0, 0.f);
        canon_removePointLight(&l_pointLight);
        canon_removePointWind(&l_pointWind);
        this->mEffectActive = 0;
    }
    this->set_mtx();
    // HD: compact GHS member descriptor preserves virtual and adjusted-this dispatch.
    (this->*mProcess)();
    canon::Local<cXyz> center;
    f32 x = this->current.pos.x, z = this->current.pos.z;
    s16 yaw = this->shape_angle.y;
    f32 distance = (f32)l_collisionDistance;
    f32 xx = gabi::fmadds(distance, sine(yaw), x), zz = gabi::fmadds(distance, cosine(yaw), z);
    center->y = this->current.pos.y;
    center->x = xx;
    center->z = zz;
    canon_setCylinderCenter(&this->mCyl1.mCyl, center.get());
    f32 radius = (f32)l_collisionRadius + 75.f;
    canon_setCylinderRadius(&this->mCyl1.mCyl, radius);
    dComIfG_inf_c* p = play();
    canon_registerCollision(&p->play.mCcS, &this->mCyl1);
    return 0;
}
/* 02109CCC */
void daCanon_c::GameInfo2DDraw() {
    for (int i = 0; i < 10; i++) {
        f32 y0 = (f32)l_bombY + 368.f, step = (f32)l_bombSpacing - 27.f;
        f32 x = (f32)l_bombX + 30.f, y = gabi::fmadds(step, (f32)i, y0);
        auto* icon = reinterpret_cast<dDlst_2DObject_c*>((void*)this->mBombIcons[i]);
        icon->mPos.x = x;
        icon->mPos.y = y;
        f32 scale = (f32)l_bombScale + 0.8f;
        icon = reinterpret_cast<dDlst_2DObject_c*>((void*)this->mBombIcons[i]);
        icon->mScale = scale;
        void* object = this->mBombIcons[i];
        display_list(object);
    }
    for (int i = 0; i < 5; i++) {
        f32 y0 = (f32)l_shipY + 115.f, step = (f32)l_shipSpacingAndRange + 47.f;
        f32 x = (f32)l_shipX + 540.f, y = gabi::fmadds(step, (f32)i, y0);
        auto* icon = reinterpret_cast<dDlst_2DObject_c*>((void*)this->mShipIcons[i]);
        icon->mPos.x = x;
        icon->mPos.y = y;
        f32 scale = (f32)l_shipScale + 1.f;
        icon = reinterpret_cast<dDlst_2DObject_c*>((void*)this->mShipIcons[i]);
        icon->mScale = scale;
        void* object = this->mShipIcons[i];
        display_list(object);
    }
    f32 angle = (f32)(s16)(0x4000 - (s16)this->shape_angle.x) * 0.0054931640625f;
    static_cast<dDlst_2DBattery_c*>((void*)this->mpBattery)->setRotate(angle);
    void* object = this->mpBattery;
    display_list(object);
}
/* 02109F18 */
BOOL daCanon_c::_draw() {
    render_light(this, &this->current.pos, false);
    if (firing(this))
        render_light(this, &this->mBallPos, true);
    dComIfG_inf_c* p = play();
    if (p->play.mMiniGameType == 3)
        this->GameInfo2DDraw();
    return 1;
}
/* 0210A4A4 */
void daCanon_c::PadMove() {
    // HD: unordered comparisons retain the original NaN joystick branches.
    f32 x = canon_getStickX(0), y = canon_getStickY(0);
    canon::SaveRoot* save = l_save;
    s16 yaw = this->shape_angle.y, pitch = this->shape_angle.x;
    void* options = canon_getOptions(&save->mOptions);
    if (static_cast<canon::Options*>(options)->mCameraControl == 0)
        y = -y;
    if (!(x <= 0.5f)) {
        s16 step = (s16)gabi::ftoi(1024.f * (0.5f - x));
        s16 home = this->home.angle.y, limit = (s16)(home - 0x1200);
        s16 current = this->shape_angle.y;
        yaw = (s16)(yaw + step);
        if (current >= limit && yaw < limit)
            yaw = limit;
    } else if (!(x >= -0.5f)) {
        s16 step = (s16)gabi::ftoi(1024.f * (-0.5f - x));
        s16 home = this->home.angle.y, limit = (s16)(home + 0x1200);
        s16 current = this->shape_angle.y;
        yaw = (s16)(yaw + step);
        if (current <= limit && yaw > limit)
            yaw = limit;
    }
    if (!(y <= 0.5f)) {
        s16 home = this->home.angle.x;
        f32 raw = 512.f * (y - 0.5f);
        s16 current = this->shape_angle.x;
        s16 limit = (s16)(home + 0x1000);
        pitch = (s16)(pitch + (s16)gabi::ftoi(raw));
        if (current <= limit && pitch > limit)
            pitch = limit;
    } else if (!(y >= -0.5f)) {
        f32 raw = 512.f * (y + 0.5f);
        s16 home = this->home.angle.x, current = this->shape_angle.x;
        pitch = (s16)(pitch + (s16)gabi::ftoi(raw));
        if (current >= home && pitch < home)
            pitch = home;
    }
    s16 yd = canon_chaseAngle(&this->shape_angle.y, yaw, 4, 0x100, 0x10);
    s16 pd = canon_chaseAngle(&this->shape_angle.x, pitch, 4, 0x100, 0x10);
    if (yd || pd) {
        sound(this, 0x2051);
        f32 angle = (f32)(s16)(0x4000 - (s16)this->shape_angle.x) * 0.0054931640625f;
        auto* root = l_meter.get();
        canon::Hud* ui = root->mpGameInfo;
        canon_setHudAngle(ui, angle);
    }
}
/* 0210A7A4 */
void daCanon_c::makeFireEffect(cXyz* position, csXyz* rotation, s32 kind) {
    dComIfG_inf_c* p = play();
    s8 cameraId = p->play.mPlayerInfo[0].mCameraID;
    p = play();
    u32 cameraAddress = (u32)gabi::at<dComIfG_camera_info_class>(
                            gabi::ea(&p->play.mCameraInfo[0]) +
                            (u32)((s32)cameraId * sizeof(dComIfG_camera_info_class)))
                            ->mpCamera;
    auto* camera = gabi::at<canon::Camera>(cameraAddress);
    canon::Local<cXyz> scale;
    scale->set(0.75f, 0.75f, 0.75f);
    canon::Local<csXyz> view;
    view->x = (s16)-camera->mAngle.x;
    view->z = 0;
    view->y = (s16)(camera->mAngle.y + 0x8000);
    p = play();
    void* particles = p->play.mParticle.get();
    if (kind == 0)
        canon_emitSimpleParticle(particles, 0x200A, position, rotation, scale.get(), 255);
    else {
        canon_emitParticle(particles, 0, 0xB, position, view.get(), scale.get(), 255, 0, -1,
                           (void*)nullptr, (void*)nullptr, (void*)nullptr);
        p = play();
        particles = p->play.mParticle.get();
        canon_emitSimpleParticle(particles, 0x2009, position, rotation, scale.get(), 255);
        p = play();
        particles = p->play.mParticle.get();
        canon_emitSimpleParticle(particles, 0x200A, position, rotation, scale.get(), 255);
        p = play();
        particles = p->play.mParticle.get();
        canon_emitParticle(particles, 2, 0x2008, position, rotation, scale.get(), 255, 0, -1,
                           (void*)nullptr, (void*)nullptr, (void*)nullptr);
    }
    auto* light = &l_pointLight;
    light->mPosition.x = (f32)position->x;
    f32 y = position->y;
    light->mPosition.y = y;
    f32 z = position->z;
    light->mGreen = 200;
    light->mPosition.z = z;
    light->mPosition.y = y + 100.f;
    light->mRed = 200;
    light->mBlue = 160;
    light->mPower = 600.f;
    light->mFluctuation = 100.f;
    canon_addPointLight(light);
    auto* wind = &l_pointWind;
    wind->mPosition.copy(*position);
    wind->mDirection.x = 0.f;
    wind->mDirection.z = 0.f;
    wind->mRadius = 1000.f;
    wind->mDirection.y = 1.f;
    wind->mStrength = 1.f;
    wind->mSpeed = 0.f;
    canon_addPointWind(wind);
    this->mEffectActive = 1;
    sound(this, 0x6901);
    p = play();
    canon::Local<cXyz> shock;
    shock->set(0.f, 1.f, 0.f);
    canon_startShock(&p->play.mVibration, 7, -33, shock.get());
}
/* 0210AAD4 */
void daCanon_c::wait_proc() {
    dComIfG_inf_c* p = play();
    void* player = p->play.mPlayerInfo[0].mpPlayer.get();
    canon::Local<cXyz> difference, flat;
    canon_subtractVector(&static_cast<fopAc_ac_c*>(player)->current.pos, difference.get(),
                         &this->current.pos);
    flat->y = 0.f;
    flat->x = difference->x;
    flat->z = difference->z;
    f32 magnitude = canon_vectorLengthSquared(flat.get());
    canon_squareRoot(magnitude);
    p = play();
    s32 staff = canon_getStaffId(&p->play.mEvtManager, "Canon", 0, 0);
    if (staff == -1)
        return;
    p = play();
    s32 act = canon_getActIndex(&p->play.mEvtManager, staff, &l_startCuts, 1, 1, 0);
    p = play();
    if (act != 0) {
        canon_cutEnd(&p->play.mEvtManager, staff);
        return;
    }
    play();
    canon::Local<cXyz> position;
    position->copy(this->current.pos);
    p = play();
    player = p->play.mPlayerInfo[0].mpPlayer.get();
    this->createTargetObj();
    s16 yaw = this->home.angle.y;
    this->mHitCount = 0;
    this->mRemainingShots = 10;
    position->x = gabi::fnmsubs(200.f, sine(yaw), (f32)position->x);
    position->z = gabi::fnmsubs(200.f, cosine(yaw), (f32)position->z);
    u32 vtable = (u32) static_cast<fopAc_ac_c*>(player)->__vtbl;
    u32 target = gabi::at<canon::PlayerVtable>(vtable)->mSetPosition.get();
    gabi::call_ptr<void>(target, player, position.get(), yaw);
    p = play();
    u16 flags = p->play.mMinigameFlags;
    p->play.mMiniGameType = 3;
    p->play.mMinigameFlags = flags | 4;
    f32 angle = (f32)(s16)(0x4000 - (s16)this->shape_angle.x) * 0.0054931640625f;
    auto* root = l_meter.get();
    canon::Hud* ui = root->mpGameInfo;
    ui->mAngle = angle;
    canon_setScreenPosition(&ui->mScreenPosition, &l_startScreen);
    for (int i = 0; i < 10; i++)
        reinterpret_cast<dDlst_2DObject_c*>((void*)this->mBombIcons[i])->mCurrentNo = 0;
    for (int i = 0; i < 5; i++)
        reinterpret_cast<dDlst_2DObject_c*>((void*)this->mShipIcons[i])->mCurrentNo = 0;
    this->game_start_proc_init();
}
/* 0210AE44 */
void daCanon_c::game_proc() {
    this->PadMove();
    if (!canon_getATrigger(0))
        return;
    canon::Local<cXyz> launch;
    s16 pitch = this->shape_angle.x, yaw = this->shape_angle.y;
    f32 reach = 200.f * sine(pitch);
    launch->x = gabi::fmadds(reach, sine(yaw), (f32)this->current.pos.x);
    launch->y = gabi::fmadds(200.f, cosine(pitch), (f32)this->current.pos.y);
    launch->z = gabi::fmadds(reach, cosine(yaw), (f32)this->current.pos.z);
    this->makeFireEffect(launch.get(), &this->shape_angle, 0);
    this->mLaunchPos.copy(*launch);
    sound(this, 0x2852);
    this->fire_proc_init();
}
/* 0210AF64 */
void daCanon_c::pause_proc_init() {
    this->mTimer = 30;
    if (canon_checkAttackHit(&this->mCyl2)) {
        void* target = canon_getHitObject(&this->mCyl2.mGObjAt);
        if (target && (s16) static_cast<fopAc_ac_c*>(target)->mProfName == PROC_CANON_TARGET) {
            s16 hits = this->mHitCount;
            dDlst_2DObject_c* icon =
                gabi::at<gptr<dDlst_2DObject_c>>(gabi::ea(&mShipIcons) +
                                                 sizeof(gptr<dDlst_2DObject_c>) * (u32)(s32)hits)
                    ->get();
            icon->mCurrentNo = 1;
            canon::Local<csXyz> rotation;
            rotation->x = (s16)l_zeroAngleX;
            rotation->y = (s16)l_zeroAngleY;
            rotation->z = (s16)l_zeroAngleZ;
            this->makeFireEffect(&this->mBallPos, rotation.get(), 1);
            hits = (s16)((s16)this->mHitCount + 1);
            this->mHitCount = hits;
            auto* root = l_meter.get();
            canon::Hud* ui = root->mpGameInfo;
            canon_setHits(ui, hits);
            s16 result = this->mHitCount;
            dComIfG_inf_c* p = play();
            p->play.mMiniGameRupee = (u16)result;
            p = play();
            canon::Local<cXyz> direction;
            direction->y = 0.f;
            s16 yaw = this->shape_angle.y;
            direction->x = sine(yaw);
            direction->z = cosine(yaw);
            canon_startShock(&p->play.mVibration, 7, 0x3E, direction.get());
            void* npc = l_gameNpc.get();
            if (npc) {
                if ((s16)this->mHitCount == 5)
                    sound(npc, 0x8AB);
                else if ((s16)this->mRemainingShots == 0)
                    sound(npc, 0x8AC);
                else
                    sound(npc, 0x8AA);
            }
        }
        canon_clearAttackHit(&this->mCyl2);
    } else if ((s16)this->mRemainingShots == 0) {
        void* npc = l_gameNpc.get();
        if (npc)
            sound(npc, 0x8AC);
    }
    set_process(this, PROC_PAUSE);
}
/* 0210B228 */
void daCanon_c::fire_proc() {
    canon_chaseShort(&this->mRecoilAmplitude, 0, 4, 0x100);
    this->mRecoilPhase = (s16)((s16)this->mRecoilPhase + 0x3800);
    cLib_addCalc2(&this->mRecoil, 0.f, 0.25f, 5.f);
    canon::Local<cXyz> endpoint;
    this->getBulEndPos(endpoint.get(), (s16)this->shape_angle.y, (s16)this->shape_angle.x);
    s16 old = this->mFlightFrames;
    this->mEndPos.copy(*endpoint);
    this->mFlightFrames = (s16)(old - 1);
    if (old != 0 && !canon_checkAttackHit(&this->mCyl2)) {
        f32 ratio = (f32)(s16)this->mFlightFrames / (f32)(s16)this->mFlightDuration;
        f32 centered = ratio - 0.5f;
        f32 arc = gabi::fnmsubs(centered * centered, 4.f, 1.f);
        canon::Local<cXyz> difference, flat, start, end, result;
        canon_subtractVector(&this->mEndPos, difference.get(), &this->mLaunchPos);
        s16 angle = (s16)(0x4000 - (s16)this->shape_angle.x);
        flat->y = 0.f;
        flat->z = difference->z;
        flat->x = difference->x;
        f32 length = canon_vectorLengthSquared(flat.get());
        length = canon_squareRoot(length);
        f32 height = (0.25f * length) * (sine(angle) / cosine(angle));
        canon_scaleVector(&this->mLaunchPos, start.get(), ratio);
        canon_scaleVector(&this->mEndPos, end.get(), 1.f - ratio);
        canon_addVector(start.get(), result.get(), end.get());
        f32 y = gabi::fmadds(height, arc, (f32)result->y);
        this->mBallPos.x = result->x;
        this->mBallPos.y = y;
        this->mBallPos.z = result->z;
        this->bul_set_mtx(&this->mBallPos);
        canon_setCylinderCenter(&this->mCyl2.mCyl, &this->mBallPos);
        dComIfG_inf_c* p = play();
        canon_registerCollision(&p->play.mCcS, &this->mCyl2);
        s32 reverb = canon_getReverb((s8)this->current.roomNo);
        canon_startSound(0x381F, &this->mBallPos, 0, reverb);
    } else {
        canon_stopSound(&this->mBallPos);
        canon_createWaterPillar(&this->mBallPos, 2.f, 1.f, 1);
        this->pause_proc_init();
    }
}
/* 0210B524 */
void daCanon_c::end_proc_init() {
    s16 hits = this->mHitCount;
    this->mProcess.i = -1;
    this->mProcess.f = PROC_END;
    this->mTimer = 60;
    this->mProcess.d = 0;
    dComIfG_inf_c* p = play();
    u8 result = hits == 5 ? 1 : 2;
    p->play.mMinigameResult = result;
    l_gameResult = result;
}
/* 0210B5A4 */
void daCanon_c::pause_proc() {
    s16 timer = this->mTimer;
    this->mTimer = (s16)(timer - 1);
    if (timer < 0) {
        if ((s16)this->mRemainingShots > 0 && (s16)this->mHitCount < 5)
            this->game_proc_init();
        else
            this->end_proc_init();
    }
}
/* 0210B5D8 */
void daCanon_c::end_proc() {
    s16 timer = this->mTimer;
    if (timer > 0) {
        if (timer == 30) {
            this->breakAllObj();
            timer = this->mTimer;
        }
        this->mTimer = (s16)(timer - 1);
        return;
    }
    dComIfG_inf_c* p = play();
    s32 staff = canon_getStaffId(&p->play.mEvtManager, "Canon", 0, 0);
    if (staff != -1) {
        p = play();
        s32 act = canon_getActIndex(&p->play.mEvtManager, staff, &l_endCuts, 1, 1, 0);
        if (act != 0)
            return;
        p = play();
        canon_cutEnd(&p->play.mEvtManager, staff);
        p = play();
        if (p->play.mMiniGameType == 3) {
            p = play();
            finish_game(p);
            auto* root = l_meter.get();
            canon::Hud* ui = root->mpGameInfo;
            canon_setScreenPosition(&ui->mScreenPosition, &l_endScreen);
        }
    }
    this->wait_proc_init();
}
// HD: compiler-generated lifetime and SafeString helpers.
/* 0210B6FC */
void __sinit_d_a_canon_cpp() {
    l_lightInit3 = 0;
    l_lightInit2 = 0;
    l_lightInit1 = 0;
    l_lightInit0 = 0;
    canon_registerDestructor(&l_lightDestructor);
    l_defaultLightX = (f32)l_lightX;
    l_defaultLightY = (f32)l_lightY;
    canon_setInitFlag(&l_lightFlag);
    canon_registerDestructor(&l_lightFlagDestructor);
    canon_constructInitFlag(&l_windFlag);
    canon_registerDestructor(&l_windFlagDestructor);
    l_lightInfluence = 1.f;
    l_lightDistance1 = 10000.f;
    l_lightPower1 = 50000.f;
    l_lightPower0 = 50000.f;
    l_lightDistance0 = 10000.f;
}
/* 0210B7EC */

// These tail helpers are owned by the actor and SafeString vtables above.
void canon_destruct(daCanon_c* a, u32 flags) {
    if (a) {
        canon_destroyCylinder(&a->mCyl2, 2);
        canon_destroyStatus(&a->mStts2, 2);
        canon_destroyCylinder(&a->mCyl1, 2);
        canon_destroyStatus(&a->mStts1, 2);
        canon_destroyActor(a, 0);
        if (flags & 1)
            operator_delete(a);
    }
}
/* 0210B870 */

void canon_safe_string_empty(void* object) {}

/* WWHD glue
data l_columnAngles 1000C3FC
data l_sourceBody 101B3A08
data l_sourceBall 101B3A4C
data l_startCuts 101B3A90
data l_endCuts 101B3A94
data l_startScreen 10490FFC
data l_endScreen 1049102C
data l_lightDestructor 101B3A98
data l_lightFlagDestructor 101B3AA4
data l_windFlagDestructor 101B3AB0
data l_lightFlag 10462C8C
data l_windFlag 10462C8D
data l_safeStringVtable 1000C3C4
data l_actorVtable 1000C3EC
data M_arcname 1000C520
data M_textureArcname 1000C414
data l_bombX 1047BBC4
data l_bombY 1047BBC8
data l_bombSpacing 1047BBCC
data l_shipX 1047BBD0
data l_shipY 1047BBD4
data l_shipSpacingAndRange 1047BBD8
data l_targetRange 1047BBDC
data l_collisionDistance 1047BBE0
data l_collisionRadius 1047BBE4
data l_bombScale 1047BC00
data l_shipScale 1047BC04
data l_cannonScale 1047BC18
data l_cannon 101D5F20
data l_gameNpc 101D5F24
data l_gameResult 101D5F42
data l_resourceControl 101F4F28
data l_meter 101F8344
data l_save 101F84DC
data l_currentModel 104B462C
data l_zeroAngleX 101FFB14
data l_zeroAngleY 101FFB16
data l_zeroAngleZ 101FFB18
data l_lightInit3 10462C9C
data l_lightInit2 10462C98
data l_lightInit1 10462C94
data l_lightInit0 10462C90
data l_defaultLightX 10462C74
data l_defaultLightY 10462C78
data l_lightX 1000C510
data l_lightY 1000C514
data l_lightInfluence 10462CC0
data l_lightDistance1 10462C88
data l_lightPower1 10462C80
data l_lightPower0 10462C7C
data l_lightDistance0 10462C84
str 1000C40C
str 1000C41C
str 1000C42C
str 1000C440
str 1000C4D0 in 0210AAD4
str 1000C4FC in 0210B5D8
str 1000C520 in 021097D8
str 1000C520 in 02109A78
data l_pointLight 10462CA0
data l_pointWind 10462CC4
ext 020063C0 void canon_setScreenPosition(void *, void *)
ext 02007898 BOOL canon_getATrigger(s32)
ext 0200796C f32 canon_getStickX(s32)
ext 02007990 f32 canon_getStickY(s32)
ext 0200E240 void canon_registerCollision(void *, void *)
ext 0200F378 s16 canon_chaseAngle(be<s16> *, s16, s32, s32, s32)
ext 0200F428 void canon_chaseShort(be<s16> *, s32, s32, s32)
ext 020182E0 void canon_setCylinderCenter(void *, cXyz *)
ext 020184DC void canon_setCylinderRadius(void *, f32)
ext 0201AD78 void canon_addVector(cXyz *, cXyz *, cXyz *)
ext 0201ADE0 void canon_subtractVector(cXyz *, cXyz *, cXyz *)
ext 0201AE48 void canon_scaleVector(cXyz *, cXyz *, f32)
ext 0204698C void * canon_constructBattery(void *)
ext 02046A68 BOOL canon_initBattery(void *, void *, void *, void *, void *)
ext 020470F4 void dDlst_2DBattery_c::setRotate(f32)
ext 020472CC void * canon_constructIcon(void *)
ext 02047358 BOOL canon_initIcon(void *, void *, void *)
ext 020CB8D8 u32 canon_makeBombParams(s32, s32, s32)
ext 02515860 void canon_destroyStatus(void *, s32)
ext 02515A70 void canon_destroyCylinder(void *, s32)
ext 02515BBC void * canon_getHitObject(void *)
ext 02515F14 void canon_initStatus(void *, s32, s32, void *)
ext 02516094 void canon_clearAttackHit(void *)
ext 025160DC BOOL canon_checkAttackHit(void *)
ext 02516518 void canon_setCylinder(void *, void *)
ext 025200D4 void * canon_getPlay()
ext 02520460 s32 canon_loadResource(void *, const char *)
ext 025204C8 void canon_deleteResource(void *, const char *)
ext 02520540 s32 canon_getReverb(s8)
ext 0252CDC0 void canon_addDisplayList(void *, void *, void *, void *)
ext 02542D88 s32 canon_getStaffId(void *, const char *, s32, s32)
ext 02542EDC s32 canon_getActIndex(void *, s32, void *, s32, s32, s32)
ext 02543280 void canon_cutEnd(void *, s32)
ext 025553B8 void canon_clearDiceWeather(s32, s32, s32, f32)
ext 02555D0C void * canon_getEnvironment()
ext 0255B9C8 void canon_addPointLight(void *)
ext 0255BA9C void canon_removePointLight(void *)
ext 025626A4 void canon_setLightTev(void *, s32, cXyz *, void *)
ext 02562F5C void canon_applyLightTev(void *, J3DModel *, void *)
ext 0257D1B8 void canon_removePointWind(void *)
ext 0257DC90 void canon_addPointWind(void *)
ext 025A847C void canon_emitParticle(void *, s32, s32, cXyz *, csXyz *, cXyz *, s32, s32, s32, void *, void *, void *)
ext 025A866C void canon_emitSimpleParticle(void *, s32, cXyz *, csXyz *, cXyz *, s32)
ext 025CB374 void canon_startShock(void *, s32, s32, cXyz *)
ext 025D4ED0 void canon_constructActor(void *)
ext 025D50BC void canon_destroyActor(void *, s32)
ext 025D5218 Pair32 canon_searchTargets(CanonTargetSearch, void *)
ext 025D5834 void canon_createActor(s32, u32, cXyz *, s32, void *, void *, s32, u32)
ext 025D5A20 u32 canon_createChild(s32, u32, u32, cXyz *, s32, void *, void *, s32, CanonTargetCreateCallback)
ext 025D63E8 BOOL canon_createHeap(void *, CanonHeapCallback, u32)
ext 025DAE64 void canon_createWaterPillar(cXyz *, f32, f32, s32)
ext 025E1A40 void canon_startSound(u32, void *, s32, s32)
ext 025E1AF8 void canon_stopSound(cXyz *)
ext 025E1B34 void canon_deleteSound(cXyz *)
ext 025F24E0 void canon_translate(f32, f32, f32)
ext 026066C4 void * canon_getObjectResource(void *, void *, s32)
ext 02646764 void canon_setShotsUsed(void *, s32)
ext 0264679C void canon_setHits(void *, s16)
ext 026467E0 void canon_setHudAngle(void *, f32)
ext 027200D0 void * canon_getOptions(void *)
ext 0273AA24 void canon_assert(const char *, s32, const char *)
ext 027DF9B0 s8 canon_findJointName(void *, const char *)
ext 027F68FC void * canon_getJointNames(void *)
ext 027F7878 void * canon_getJoint(void *)
ext 028E8DD0 f32 canon_vectorLengthSquared(cXyz *)
ext 028EAB2C void canon_constructInitFlag(void *)
ext 028ED6F8 void canon_setInitFlag(void *)
ext 028F026C void canon_registerDestructor(void *)
ext 028F4384 f32 canon_squareRoot(f32)
*/
