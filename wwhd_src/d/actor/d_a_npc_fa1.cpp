// Recovery fairy, ported from the GameCube source with verified HD behaviour.
#include "d/actor/d_a_npc_fa1.h"

const gc_tu_data gc_tu = {
    .safeString = 0x10019CF0,
    .safeStringAssure = 0x02235568,
    .objAcchDt = {0x10019DD8, 0x10019DE8},
    .gndChk = {0x10019D98, 0x10019DA8, 0x10019DC8, 0x10019DB8},
    .gndChkDt = {0x10019D28, 0x10019D48, 0x10019D08},
};
// HD: GHS stores this translation unit's own actor/callback and HIO vtables.
static constexpr u32 l_actorVtable = 0x10019DF8;
static constexpr u32 l_callbackVtable = 0x10019F40;
static constexpr u32 l_hioVtable = 0x10019E40;
static constexpr u32 l_areaHioVtable = 0x10019E30;
static constexpr u32 l_hoverHioVtable = 0x10019E20;

static Mtx34 l_matrixStack;
static fa1_hd::ResourceManagerHandle l_resourceManager;
static dCcD_SrcCyl l_cyl_src;
static fa1_hd::HeaderState l_headerState;
static fa1_hd::HeaderDescriptors l_headerDescriptors;

static fa1_hd::MainHioInitial l_hioInitial;
static fa1_hd::AreaHioInitial l_areaHioInitial;
static fa1_hd::HoverHioInitial l_hoverHioInitial;

static f32 l_one;
static f32 l_zero;
static f32 l_normalGravity;
static f32 l_normalMaxFallSpeed;
static f32 l_pointLightPower;
static f32 l_pointLightFluctuation;
static f32 l_areaRadius;
static f32 l_playerRadius;
static f32 l_invalidGround;
static f32 l_homeGroundOffset;
static f32 l_lookRadius;
static f32 l_lookUpper;
static f32 l_lookLower;
static f32 l_moveHeight;
static f32 l_animationAcceleration;
static f32 l_animationMaxRate;
static f32 l_orbitRadiusStep;
static f32 l_flowerOffsetX;
static f32 l_flowerDeleteHeight;
static f32 l_playerBabaHeight;
static f32 l_headerTimerInitial;
static f32 l_headerUnusedInitial;
static daNpc_Fa1_HIO_c l_HIO;
// HD: the instance count uses the original unsigned decrement/early-return logic.
static u32 l_hio_counter;
static fa1_hd::MoveTable moveProc;
static fa1_hd::MoveSubTable moveSubProc;
static fa1_hd::BottleMoveSubTable bottleMoveSubProc;
static fa1_hd::BottleBabaSubTable bottleBabaSubProc;

// HD: integer copies preserve signalling-NaN payloads and the original interleaving.
static be<u32> &fa1_word(const be<f32> &value) { return *gabi::at<be<u32>>(gabi::ea(&value)); }

/* inlined into daNpc_Fa1_c::areaMove */
static f32 Fa1_horizontalRadius(const cXyz *vector) {
    gabi::Local<cXyz> horizontal;
    horizontal->x = f32(vector->x);
    horizontal->y = l_zero;
    horizontal->z = f32(vector->z);
    f32 square = PSVECSquareMag(horizontal.get());
    return fa1_sqrtf(square);
}

/* inlined into daNpc_Fa1_c::areaMove */
static void Fa1_areaTurn(daNpc_Fa1_c *actor, s32 angle) {
    if (!fa1_calcAreaTimer(&actor->m798)) {
        actor->m79A = s16(u16(actor->m79A) ^ 1);
        u32 timer = fa1_randomTimer(15, 20);
        actor->m798 = s16(u8(timer));
    }
    s32 turn = actor->m79A == 0 ? 0x2000 : -0x2000;
    cLib_addCalcAngleS(&actor->current.angle.y, s16(u32(angle) + u32(turn)), 0x20, 0x800, 1);
}

/* inlined into daNpc_Fa1_c::normal_move */
static BOOL Fa1_checkCollision(daNpc_Fa1_c *actor) {
    fa1_cylinderSetCenter(&actor->mCyl.mCyl, &actor->current.pos);
    auto *play = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    fa1_collisionSet(&play->play.mCcS, &actor->mCyl);
    return fa1_cylinderCheckCoHit(&actor->mCyl);
}

/* inlined into daNpc_Fa1_c::get_player_move */
static void Fa1_speedAnimation(daNpc_Fa1_c *actor) {
    u32 morf = gabi::ea(actor->mpMorf.get());
    f32 increment = l_animationAcceleration;
    f32 rate = gabi::fadds_ppc(gabi::at<mDoExt_McaMorf>(morf)->getPlaySpeed(), increment);
    f32 maximum = l_animationMaxRate;
    if (rate > maximum) {
        rate = maximum;
    }
    gabi::at<mDoExt_McaMorf>(morf)->setPlaySpeed(rate);
}

/* inlined into daNpc_Fa1_c::get_player_move */
static void Fa1_copyPosition(daNpc_Fa1_c *actor, const cXyz *position) {
    u32 y = u32(fa1_word(position->y));
    u32 x = u32(fa1_word(position->x));
    u32 z = u32(fa1_word(position->z));
    fa1_word(actor->current.pos.x) = x;
    fa1_word(actor->current.pos.z) = z;
    fa1_word(actor->current.pos.y) = y;
}

/* inlined into daNpc_Fa1_c::bottle_baba_move */
static void Fa1_babaPosition(daNpc_Fa1_c *actor) {
    auto *flower = actor->m764.get();
    f32 forward = f32(l_HIO.m50.prm.m08);
    actor->speedF = forward;
    gabi::Local<cXyz> difference;
    fa1_vectorSubtract(&flower->current.pos, difference.get(), &actor->current.pos);
    s32 direction = fa1_atan2s(f32(difference->x), f32(difference->z));
    s16 turn = s16(l_HIO.m50.prm.m20);
    forward = actor->speedF;
    u16 angle = u16(u32(direction) + u32(s32(turn)));
    actor->current.angle.y = s16(angle);
    u16 rereadAngle = actor->current.angle.y;
    f32 fall = actor->maxFallSpeed;
    f32 x = gabi::fmuls_ppc(forward, cM_ssin(angle));
    f32 zero = l_zero;
    actor->speed.x = x;
    f32 cos = cM_scos(rereadAngle);
    f32 vertical = actor->speed.y;
    f32 z = gabi::fmuls_ppc(forward, cos);
    f32 gravity = actor->gravity;
    actor->speed.z = z;
    if (fall < zero) {
        vertical = gabi::fsubs_ppc(vertical, gravity);
        fall = actor->maxFallSpeed;
        actor->speed.y = vertical;
        if (vertical < fall) {
            actor->speed.y = fall;
        }
    } else {
        vertical = gabi::fadds_ppc(vertical, gravity);
        fall = actor->maxFallSpeed;
        actor->speed.y = vertical;
        if (vertical > fall) {
            actor->speed.y = fall;
        }
    }
    PSVECAdd(&actor->current.pos, &actor->speed, &actor->current.pos);
    actor->BGCheck();
}

/* 02235250 */
void *daNpc_Fa1_HIO3_c__ct(void *hio) {
    if (!hio) {
        hio = fa1_operatorNew(0x1C);
    }
    if (hio) {
        auto *object = static_cast<daNpc_Fa1_HIO3_c *>(hio);
        object->mNo = -1;
        object->__vtbl = l_hoverHioVtable;
        // HD: copy tuning data as words, including packed signed timer fields.
        auto *words = reinterpret_cast<be<u32> *>(&object->prm);
        for (u32 i = 0; i != 5; ++i) {
            words[i] = u32(l_hoverHioInitial.mWords[i]);
        }
    }
    return hio;
}

/* 022352C4 */
void *daNpc_Fa1_HIO2_c__ct(void *hio) {
    if (!hio) {
        hio = fa1_operatorNew(0x28);
    }
    if (hio) {
        auto *object = static_cast<daNpc_Fa1_HIO2_c *>(hio);
        object->mNo = -1;
        object->__vtbl = l_areaHioVtable;
        // HD: copy tuning data as words, including packed signed timer fields.
        auto *words = reinterpret_cast<be<u32> *>(&object->prm);
        for (u32 i = 0; i != 8; ++i) {
            words[i] = u32(l_areaHioInitial.mWords[i]);
        }
    }
    return hio;
}

/* 02235350 */
void *daNpc_Fa1_HIO_c__ct(void *hio) {
    if (!hio) {
        hio = fa1_operatorNew(0x94);
    }
    if (hio) {
        auto *object = static_cast<daNpc_Fa1_HIO_c *>(hio);
        object->__vtbl = l_hioVtable;
        daNpc_Fa1_HIO2_c__ct(&object->m50);
        daNpc_Fa1_HIO3_c__ct(&object->m78);
        object->mNo = -1;
        // HD: copy tuning data as words, including packed signed timer fields.
        auto *words = reinterpret_cast<be<u32> *>(&object->prm);
        for (u32 i = 0; i != 18; ++i) {
            words[i] = u32(l_hioInitial.mWords[i]);
        }
    }
    return hio;
}

/* 02232FE4 */
daNpc_Fa1_McaMorfCallBack1_c *
daNpc_Fa1_McaMorfCallBack1_c__ct(daNpc_Fa1_McaMorfCallBack1_c *callback) {

    if (!callback) {
        callback = static_cast<daNpc_Fa1_McaMorfCallBack1_c *>(fa1_operatorNew(8));
    }
    if (callback) {
        callback->mNeckJoint = 0;
        callback->__vtbl = l_callbackVtable;
        callback->mNeckAngle = 0;
    }
    return callback;
}

/* 02233BE8 */
BOOL daNpc_Fa1_McaMorfCallBack1_c::execute(u16 joint, void *transform) {

    if (joint == mNeckJoint) {
        static_cast<fa1_hd::TransformInfo *>(transform)->mRotation.x = mNeckAngle;
    }
    return TRUE;
}

/* 02233494 */
void daNpc_Fa1_c::setPointLightParam() {

    u32 z = u32(fa1_word(attention_info.position.z));
    u32 x = u32(fa1_word(attention_info.position.x));
    mPointLight.mColorR = 200;
    f32 power = l_pointLightPower;
    fa1_word(this->mPointLight.mPos.x) = x;
    u32 y = u32(fa1_word(attention_info.position.y));
    mPointLight.mPower = power;
    mPointLight.mColorB = 200;
    f32 zero = l_zero;
    mPointLight.mColorG = 200;
    mPointLight.mFluctuation = zero;
    fa1_word(this->mPointLight.mPos.z) = z;
    f32 range = l_pointLightFluctuation;
    fa1_word(this->mPointLight.mPos.y) = y;
    mPointLight.mFluctuation = fa1_yuragiRatio(range);
}

/* 0223350C */
BOOL daNpc_Fa1_c::createInit() {

    f32 zero = l_zero;
    u32 parameters = u32(mParameters);
    m768.y = zero;
    m768.x = zero;
    s8 type = s8(parameters);
    m768.z = zero;
    mStatus = 0;
    m764 = nullptr;
    mType = type;
    bool bottle = type == Type_BABA_e || type == Type_LINK_e || type == Type_LINK_DOWN_e;
    bool hover = type == Type_HOVER_e;
    if (type == Type_BABA_e) {
        init_bottle_baba_wait();
    } else if (type == Type_LINK_e || type == Type_LINK_DOWN_e) {
        init_bottle_appear_move();
    } else if (type == Type_HOVER_e) {
        init_hover_move();
    } else {
        if (type == Type_AREA_e) {
            u32 flags = u32(actor_status);
            s8 reloadedType = mType;
            f32 radius = l_areaRadius;
            actor_status = flags & ~u32(fopAcStts_NOCULLEXEC_e);
            this->scale.x = radius;
            type = reloadedType;
        } else {
            f32 radius = f32(l_HIO.prm.m10);
            type = mType;
            this->scale.x = radius;
        }
        if (type == Type_TIMER_e) {
            u32 flags = u32(actor_status);
            mStatus = 1;
            actor_status = flags & ~u32(fopAcStts_NOCULLEXEC_e);
            u16 random = fa1_getRndValueU16(0, 255);
            s16 angle = this->current.angle.y;
            this->current.angle.y = s16(u32(s32(angle)) + (u32(random) << 8));
        } else {
            mType = Type_NORMAL_e;
        }
        init_normal_move();
    }
    if (bottle || hover) {
        u32 flags = u32(actor_status);
        actor_status = (flags & ~u32(fopAcStts_NOCULLEXEC_e)) | (hover ? 0 : fopAcStts_UNK4000_e);
    }
    f32 z = this->current.pos.z;
    f32 y = this->current.pos.y;
    f32 height = l_playerRadius;
    f32 x = this->current.pos.x;
    y = gabi::fadds_ppc(y, height);
    this->mGndChk.m_pos.z = z;
    this->mGndChk.m_pos.x = x;
    this->mGndChk.m_pos.y = y;
    auto *play = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    f32 ground = fa1_groundCross(&play->play.mBgS, &this->mGndChk);
    type = mType;
    mGroundY = ground;
    if (type != Type_HOVER_e && !(ground == l_invalidGround)) {
        this->home.pos.y = gabi::fadds_ppc(ground, l_homeGroundOffset);
    }
    fa1_collisionStatusInit(&this->mStts, 255, 255, this);
    mCyl.mStts = &mStts;
    fa1_cylinderSet(&this->mCyl, &l_cyl_src);
    setMtx();
    if (mType == Type_BABA_e) {
        setPointLightParam();
        fa1_effectPointLightSet(&mPointLight.mPos);
    }
    play = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    u32 particles = gabi::ea(play->play.mParticle.get());
    mpEmitter =
        fa1_particleSet(gabi::at<void>(particles), 0, dPa_name::ID_AK_JN_CUREFAIRY00,
                        &this->current.pos, 0, 0, 255, &this->mSparklePtclCallback, -1, 0, 0, 0);
    return TRUE;
}

/* 02233B58 */
BOOL daNpc_Fa1_c::_draw() {
    // HD: the GameCube simple-shadow submission is absent.

    u32 morf = gabi::ea(mpMorf.get());
    u32 model = gabi::ea(gabi::at<mDoExt_McaMorf>(morf)->getModel());
    void *environment = fa1_getEnvironmentLight();
    fa1_settingTevStruct(environment, 0, &this->current.pos, &this->tevStr);
    environment = fa1_getEnvironmentLight();
    fa1_setLightTevColorType(environment, gabi::at<void>(model), &this->tevStr);
    mpMorf->entryDL();
    f32 scale = l_one;
    dSnap_RegistFig(0x84, this, scale, scale, scale);
    return TRUE;
}

/* 02233A0C */
BOOL daNpc_Fa1_c::_execute() {

    fa1_calcTimerU16(&mTimer);
    u8 mode = mMode;
    auto *move =
        gabi::at<fa1_hd::MovePtmf>(gabi::ea(&moveProc) + u32(mode) * sizeof(fa1_hd::MovePtmf));
    (this->*(*move))();
    s16 angle = this->current.angle.y;
    void *morf = mpMorf.get();
    this->shape_angle.y = angle;
    fa1_morfPlay(morf, 0, 0, 0);
    setMtx();
    if (mType == Type_BABA_e) {
        setPointLightParam();
    }
    if (mMode == Mode_PLAYER_MOVE_e) {
        u32 emitter = gabi::ea(mpEmitter.get());
        if (emitter) {
            gabi::at<fa1_hd::EmitterRatePrefix>(emitter)->mRate = l_one;
        }
    }
    auto *play = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    f32 radius = l_lookRadius;
    f32 upper = l_lookUpper;
    f32 lower = l_lookLower;
    fa1_attentionLookRequest(reinterpret_cast<fa1_hd::Attention *>(&play->play.mAttention)->mLook,
                             this, 0x6000, 1, radius, upper, lower);
    return TRUE;
}

/* 02233F40 */
BOOL daNpc_Fa1_c::checkBinCatch() {

    if (u16(eventInfo.mCommand) == dEvtCmd_INCATCH_e) {
        fopAcM_delete(this);
        return TRUE;
    }
    s16 timer = s16(l_HIO.prm.m48);
    f32 radius = f32(l_HIO.prm.m1C);
    f32 lower = f32(l_HIO.prm.m24);
    f32 upper = f32(l_HIO.prm.m20);
    auto *play = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    fa1_attentionCatchRequest(reinterpret_cast<fa1_hd::Attention *>(&play->play.mAttention)->mCatch,
                              this, dItemNo_FAIRY_BOTTLE_e, timer, 1, radius, upper, lower);
    u16 condition = u16(eventInfo.mCondition);
    eventInfo.mCondition = condition | dEvtCnd_CANCATCH_e;
    return FALSE;
}

/* 02233C04 */
void daNpc_Fa1_c::position_move(f32 height, f32 verticalSpeed) {

    f32 difference = gabi::fsubs_ppc(f32(this->current.pos.y), f32(this->home.pos.y));
    if (difference > height) {
        verticalSpeed = -verticalSpeed;
        maxFallSpeed = verticalSpeed;
    } else if (difference < -height) {
        maxFallSpeed = verticalSpeed;
    } else {
        verticalSpeed = maxFallSpeed;
    }
    f32 zero = l_zero;
    f32 gravity = this->gravity;
    u16 angle = u16(this->current.angle.y);
    f32 speed = this->speed.y;
    if (verticalSpeed < zero) {
        speed = gabi::fsubs_ppc(speed, gravity);
        f32 limit = maxFallSpeed;
        this->speed.y = speed;
        if (speed < limit) {
            this->speed.y = limit;
        }
    } else {
        speed = gabi::fadds_ppc(speed, gravity);
        f32 limit = maxFallSpeed;
        this->speed.y = speed;
        if (speed > limit) {
            this->speed.y = limit;
        }
    }
    u16 tableAngle = angle;
    f32 forward = speedF;
    this->speed.x = gabi::fmuls_ppc(forward, cM_ssin(tableAngle));
    this->speed.z = gabi::fmuls_ppc(forward, cM_scos(tableAngle));
    fopAcM_posMove(this, nullptr);
}

/* 02233CD8 */
void daNpc_Fa1_c::BGCheck() {

    u32 z = u32(fa1_word(this->current.pos.z));
    u32 y = u32(fa1_word(this->current.pos.y));
    fa1_word(this->mGndChk.m_pos.z) = z;
    u32 x = u32(fa1_word(this->current.pos.x));
    fa1_word(this->mGndChk.m_pos.y) = y;
    fa1_word(this->mGndChk.m_pos.x) = x;
    auto *play = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    f32 ground = fa1_groundCross(&play->play.mBgS, &this->mGndChk);
    f32 invalid = l_invalidGround;
    mGroundY = ground;
    if (ground == invalid) {
        f32 zero = l_zero;
        f32 one = l_one;
        m774.x = zero;
        m774.y = one;
        m774.z = zero;
        return;
    }
    play = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    u16 index = u16(this->mGndChk.mPolyIndex);
    u16 background = u16(this->mGndChk.mBgIndex);
    u32 normal = gabi::ea(fa1_getTriPlane(&play->play.mBgS, background, index));
    fa1_word(this->m774.x) = u32(fa1_word(gabi::at<cXyz>(normal)->x));
    fa1_word(this->m774.y) = u32(fa1_word(gabi::at<cXyz>(normal)->y));
    fa1_word(this->m774.z) = u32(fa1_word(gabi::at<cXyz>(normal)->z));
    play = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    u8 room = fa1_getRoomId(&play->play.mBgS, &this->mGndChk.cBgS_PolyInfo::mPolyIndex);
    tevStr.mRoomNo = s8(room);
    this->current.roomNo = s8(room);
    play = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    u8 color = fa1_getPolyColor(&play->play.mBgS, &this->mGndChk.cBgS_PolyInfo::mPolyIndex);
    tevStr.mEnvrIdxOverride = color;
}

/* 022332E0 */
void daNpc_Fa1_c::init_normal_move() {

    f32 fall = l_normalMaxFallSpeed;
    f32 gravity = l_normalGravity;
    mMode = Mode_NORMAL_MOVE_e;
    f32 speed = f32(l_HIO.prm.m08);
    maxFallSpeed = fall;
    speedF = speed;
    this->gravity = gravity;
    u16 timer = fa1_getRndValueU16(u16(l_HIO.prm.m44), 60);
    s8 type = mType;
    mTimer = timer;
    if (type == Type_AREA_e) {
        init_areaMove();
    } else {
        init_straight2();
    }
}

/* 02234078 */
void daNpc_Fa1_c::normal_move() {

    u8 submode = mSubMode;
    speedF = f32(l_HIO.prm.m08);
    auto *move = gabi::at<fa1_hd::MovePtmf>(gabi::ea(&moveSubProc) +
                                            u32(submode) * sizeof(fa1_hd::MovePtmf));
    (this->*(*move))();
    f32 height = l_moveHeight;
    f32 speed = l_one;
    position_move(height, speed);
    BGCheck();
    findPlayer();
    if (Fa1_checkCollision(this)) {
        init_get_player_move();
        return;
    }
    if (!checkBinCatch() && (u8(mStatus) & 1) && !mTimer) {
        init_escape_move();
    }
}

/* 02234DC8 */
void daNpc_Fa1_c::init_straight() {

    mSubMode = NormalSubMode_STRAIGHT_e;
    u8 timer = fa1_getRndValueU8(60, 60);
    f32 gravity = l_normalGravity;
    mMoveTimer = timer;
    this->gravity = gravity;
}

/* 022332C4 */
void daNpc_Fa1_c::init_straight2() {

    mMoveTimer = 0;
    gravity = l_one;
    mSubMode = NormalSubMode_STRAIGHT_e;
}

/* 02234E60 */
void daNpc_Fa1_c::straight() {

    this->scale.x = f32(l_HIO.prm.m10);
    if (!fa1_calcMoveTimer(&mMoveTimer)) {
        gabi::Local<cXyz> difference;
        fa1_vectorSubtract(&this->current.pos, difference.get(), &this->home.pos);
        gabi::Local<cXyz> horizontal;
        horizontal->x = f32(difference->x);
        horizontal->y = l_zero;
        horizontal->z = f32(difference->z);
        f32 square = PSVECSquareMag(horizontal.get());
        f32 radius = fa1_sqrtf(square);
        if (radius > f32(this->scale.x)) {
            init_turn();
        }
    }
}

/* 02234E14 */
void daNpc_Fa1_c::init_turn() {

    mSubMode = NormalSubMode_TURN_e;
    u8 timer = fa1_getRndValueU8(60, 60);
    f32 gravity = l_normalGravity;
    mMoveTimer = timer;
    this->gravity = gravity;
}

/* 02234EF0 */
void daNpc_Fa1_c::turn() {

    if (fa1_calcMoveTimer(&mMoveTimer)) {
        gabi::Local<cXyz> difference;
        fa1_vectorSubtract(&this->current.pos, difference.get(), &this->home.pos);
        f32 x = -f32(difference->x);
        f32 z = -f32(difference->z);
        s32 angle = fa1_atan2s(x, z);
        cLib_addCalcAngleS(&this->current.angle.y, angle, 0x20, 0x800, 1);
    } else {
        init_straight();
    }
}

/* 022332A0 */
void daNpc_Fa1_c::init_areaMove() {

    mSubMode = NormalSubMode_AREAMOVE_e;
    gravity = l_one;
    m798 = 0;
    m79A = 0;
}

/* 02234FA4 */
void daNpc_Fa1_c::areaMove() {

    gabi::Local<cXyz> difference;
    fa1_vectorSubtract(&this->current.pos, difference.get(), &this->home.pos);
    f32 radius = Fa1_horizontalRadius(difference.get());
    if (radius > f32(this->scale.x)) {
        init_areaOutMove();
        return;
    }
    s16 direction = this->current.angle.y;
    Fa1_areaTurn(this, direction);
}

/* 02234F80 */
void daNpc_Fa1_c::init_areaOutMove() {

    mSubMode = NormalSubMode_AREAOUTMOVE_e;
    gravity = l_normalGravity;
    m798 = 0;
    m79A = 0;
}

/* 0223509C */
void daNpc_Fa1_c::areaOutMove() {

    gabi::Local<cXyz> difference;
    fa1_vectorSubtract(&this->current.pos, difference.get(), &this->home.pos);
    f32 radius = Fa1_horizontalRadius(difference.get());
    if (radius < f32(this->scale.x)) {
        init_areaMove();
        return;
    }
    s32 direction = fa1_targetAngleY(&this->current.pos, &this->home.pos);
    Fa1_areaTurn(this, direction);
}

/* 02233EA8 */
void daNpc_Fa1_c::init_get_player_move() {

    mMode = Mode_PLAYER_MOVE_e;
    u16 timer = u16(l_HIO.prm.m42);
    f32 radius = l_playerRadius;
    mTimer = timer;
    s16 turn = s16(l_HIO.prm.m3C);
    mMcaMorfCallback1.mNeckAngle = 0;
    m78A = turn;
    mPlayerR = radius;
    this->current.angle.y = 0;
    auto *play = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    auto *player = play->play.mPlayerInfo[0].mpPlayer.get();
    s8 room = this->current.roomNo;
    this->current.pos.y = f32(player->current.pos.y);
    s32 reverb = fa1_getReverb(room);
    fa1_seStart(JA_SE_OBJ_FAILY_GET, &this->current.pos, nullptr, reverb);
}

/* 02234204 */
void daNpc_Fa1_c::get_player_move() {

    Fa1_speedAnimation(this);
    s16 turn = m78A;
    s16 angle = s16(u16(this->current.angle.y) - u16(turn));
    this->current.angle.y = angle;
    s16 acceleration = s16(l_HIO.prm.m40);
    s16 limit = s16(l_HIO.prm.m3E);
    turn = s16(turn + acceleration);
    if (turn > limit) {
        turn = limit;
    }
    m78A = turn;
    u16 orbitAngle = u16(u16(angle) + 0x4000);
    auto *play = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    u16 tableAngle = orbitAngle;
    f32 radius = mPlayerR;
    f32 x = gabi::fmuls_ppc(radius, cM_ssin(tableAngle));
    auto *player = play->play.mPlayerInfo[0].mpPlayer.get();
    m768.x = x;
    f32 z = gabi::fmuls_ppc(radius, cM_scos(tableAngle));
    f32 y = m768.y;
    m768.z = z;
    m768.y = gabi::fadds_ppc(y, f32(l_HIO.prm.m0C));
    gabi::Local<cXyz> position;
    fa1_vectorAdd(&player->current.pos, position.get(), &m768);
    Fa1_copyPosition(this, position.get());
    BGCheck();
    if (!mTimer) {
        execItemGet(dItemNo_RECOVER_FAIRY_e);
        fopAcM_delete(this);
    }
}

/* 0223404C */
void daNpc_Fa1_c::init_escape_move() {

    mMode = Mode_ESCAPE_MOVE_e;
    gravity = f32(l_HIO.prm.m18);
    maxFallSpeed = f32(l_HIO.prm.m14);
    mTimer = u16(l_HIO.prm.m46);
}

/* 0223434C */
void daNpc_Fa1_c::escape_move() {

    f32 gravity = this->gravity;
    f32 speed = gabi::fadds_ppc(f32(this->speed.y), gravity);
    f32 limit = maxFallSpeed;
    this->speed.y = speed;
    u16 angle = this->current.angle.y;
    if (speed > limit) {
        this->speed.y = limit;
    }
    u16 tableAngle = angle;
    f32 forward = speedF;
    this->speed.x = gabi::fmuls_ppc(forward, cM_ssin(tableAngle));
    this->speed.z = gabi::fmuls_ppc(forward, cM_scos(tableAngle));
    fopAcM_posMove(this, nullptr);
    BGCheck();
    s16 neck = mMcaMorfCallback1.mNeckAngle;
    if (neck > 0) {
        neck = s16(neck - 0x800);
        if (neck < 0) {
            neck = 0;
        }
    } else if (neck < 0) {
        neck = s16(neck + 0x800);
        if (neck > 0) {
            neck = 0;
        }
    }
    mMcaMorfCallback1.mNeckAngle = neck;
    if (Fa1_checkCollision(this)) {
        init_get_player_move();
    } else if (!mTimer) {
        fopAcM_delete(this);
    }
}

/* 0223326C */
void daNpc_Fa1_c::init_hover_move() {

    mMode = Mode_HOVER_MOVE_e;
    f32 gravity = f32(l_HIO.m78.prm.m0C);
    this->gravity = gravity;
    f32 rise = f32(l_HIO.m78.prm.m08);
    f32 zero = l_zero;
    speedF = zero;
    maxFallSpeed = -rise;
}

/* 0223447C */
void daNpc_Fa1_c::hover_move() {

    f32 speed = f32(l_HIO.m78.prm.m08);
    f32 height = f32(l_HIO.m78.prm.m10);
    position_move(height, speed);
    BGCheck();
    auto *play = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    auto *player = play->play.mPlayerInfo[0].mpPlayer.get();
    s32 direction = fa1_searchActorAngleY(this, player);
    cLib_addCalcAngleS(&this->current.angle.y, direction, 8, 0x2000, 0x400);
}

/* 02233220 */
void daNpc_Fa1_c::init_bottle_appear_move() {

    mMode = Mode_BOTTLE_APPEAR_MOVE_e;
    init_up1();
    if (mType == Type_LINK_DOWN_e) {
        execItemGet(dItemNo_RECOVER_FAIRY_e);
    }
}

/* 022344EC */
void daNpc_Fa1_c::bottle_appear_move() {

    Fa1_speedAnimation(this);
    s16 angle = this->current.angle.y;
    s16 turn = m78A;
    s16 acceleration = m78E;
    s16 limit = m78C;
    angle = s16(angle - turn);
    turn = s16(turn + acceleration);
    this->current.angle.y = angle;
    m78A = turn;
    if (turn > limit) {
        m78A = limit;
    }
    u16 orbitAngle = u16(u16(angle) + 0x4000);
    auto *play = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    f32 radius = mPlayerR;
    f32 vertical = this->speed.y;
    u16 tableAngle = orbitAngle;
    f32 x = gabi::fmuls_ppc(radius, cM_ssin(tableAngle));
    auto *player = play->play.mPlayerInfo[0].mpPlayer.get();
    f32 y = gabi::fadds_ppc(f32(m768.y), vertical);
    m768.x = x;
    f32 z = gabi::fmuls_ppc(radius, cM_scos(tableAngle));
    m768.y = y;
    m768.z = z;
    gabi::Local<cXyz> position;
    fa1_vectorAdd(&player->current.pos, position.get(), &m768);
    Fa1_copyPosition(this, position.get());
    f32 zero = l_zero;
    f32 fall = maxFallSpeed;
    f32 speed = this->speed.y;
    f32 gravity = this->gravity;
    if (fall < zero) {
        speed = gabi::fsubs_ppc(speed, gravity);
        fall = maxFallSpeed;
        this->speed.y = speed;
        if (speed < fall) {
            this->speed.y = fall;
        }
    } else {
        speed = gabi::fadds_ppc(speed, gravity);
        fall = maxFallSpeed;
        this->speed.y = speed;
        if (speed > fall) {
            this->speed.y = fall;
        }
    }
    radius = gabi::fadds_ppc(f32(mPlayerR), l_orbitRadiusStep);
    f32 maximum = l_playerRadius;
    if (radius > maximum) {
        radius = maximum;
    }
    mPlayerR = radius;
    BGCheck();
    auto *move = gabi::at<fa1_hd::MovePtmf>(gabi::ea(&bottleMoveSubProc) +
                                            u32(u8(mSubMode)) * sizeof(fa1_hd::MovePtmf));
    (this->*(*move))();
    if (!mTimer) {
        if (mType != Type_LINK_DOWN_e) {
            execItemGet(dItemNo_RECOVER_FAIRY_e);
        }
        fopAcM_delete(this);
    }
}

/* 02233154 */
void daNpc_Fa1_c::init_up1() {

    mSubMode = BottleSubMode_UP1_e;
    mTimer = u16(l_HIO.prm.m4A);
    maxFallSpeed = f32(l_HIO.prm.m30);
    this->speed.y = f32(l_HIO.prm.m28);
    f32 gravity = f32(l_HIO.prm.m34);
    m78A = 0;
    this->gravity = gravity;
    m78C = 0;
    m78E = 0;
    auto *play = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    auto *player = play->play.mPlayerInfo[0].mpPlayer.get();
    gabi::Local<cXyz> difference;
    fa1_vectorSubtract(&this->current.pos, difference.get(), &player->current.pos);
    f32 x = difference->x;
    f32 zero = l_zero;
    f32 z = difference->z;
    f32 y = difference->y;
    gabi::Local<cXyz> horizontal;
    horizontal->y = zero;
    m768.x = x;
    m768.y = y;
    horizontal->x = x;
    horizontal->z = z;
    m768.z = z;
    f32 square = PSVECSquareMag(horizontal.get());
    f32 radius = fa1_sqrtf(square);
    z = m768.z;
    mPlayerR = radius;
    x = m768.x;
    s32 angle = fa1_atan2s(x, z);
    this->current.angle.y = s16(u32(angle) - 0x4000);
}

/* 022351C4 */
void daNpc_Fa1_c::up1() {

    if (f32(this->speed.y) < l_zero) {
        init_down();
    }
}

/* 022351A0 */
void daNpc_Fa1_c::init_down() {

    mSubMode = BottleSubMode_DOWN_e;
    m78C = s16(l_HIO.prm.m4C);
    m78E = s16(l_HIO.prm.m4E);
}

/* 02235238 */
void daNpc_Fa1_c::down() {

    if (m768.y < f32(l_HIO.prm.m38)) {
        init_up2();
    }
}

/* 022351DC */
void daNpc_Fa1_c::init_up2() {

    mSubMode = BottleSubMode_UP2_e;
    s8 room = s8(this->current.roomNo);
    maxFallSpeed = f32(l_HIO.prm.m2C);
    s32 reverb = fa1_getReverb(room);
    fa1_seStart(JA_SE_OBJ_BOTTLE_FAILY, &this->current.pos, nullptr, reverb);
}

/* 02235498 */
void daNpc_Fa1_c::up2() {}

/* 0223311C */
void daNpc_Fa1_c::init_bottle_baba_wait() {

    mMode = Mode_BOTTLE_BABA_WAIT_e;
    f32 zero = l_zero;
    maxFallSpeed = zero;
    this->speed.z = zero;
    this->speed.x = zero;
    this->speed.y = zero;
    speedF = zero;
    gravity = zero;
    mTimer = u16(l_HIO.m50.prm.m24);
}

/* 022347E4 */
void daNpc_Fa1_c::bottle_baba_wait() {

    if (!mTimer) {
        init_bottle_baba_move2();
    }
}

/* 022347F4 */
void daNpc_Fa1_c::init_bottle_baba_move() {

    mMode = Mode_BOTTLE_BABA_MOVE_e;
    fa1_searchByName(fpcNm_NPC_BA1_e, &m764);
    auto *flower = m764.get();
    mTimer = u16(l_HIO.m50.prm.m22);
    f32 speed = f32(l_HIO.m50.prm.m08);
    m798 = 1;
    speedF = speed;
    m79C = f32(l_HIO.m50.prm.m10);
    m7A0 = f32(l_HIO.m50.prm.m0C);
    if (flower) {
        f32 x = f32(flower->current.pos.x);
        f32 offset = l_flowerOffsetX;
        x = gabi::fadds_ppc(x, offset);
        flower = m764.get();
        this->current.pos.x = x;
        this->current.pos.z = f32(flower->current.pos.z);
    }
    init_baba_down();
}

/* 02234894 */
void daNpc_Fa1_c::bottle_baba_move() {

    if (!m764.get()) {
        init_bottle_baba_move();
        BGCheck();
        return;
    }
    Fa1_babaPosition(this);
    u8 submode = mSubMode;
    auto *move = gabi::at<fa1_hd::MovePtmf>(gabi::ea(&bottleBabaSubProc) +
                                            u32(submode) * sizeof(fa1_hd::MovePtmf));
    (this->*(*move))();
    if (submode == BottleBabaSubMode_UP2_e) {
        auto *flower = m764.get();
        f32 height = l_flowerDeleteHeight;
        f32 top = gabi::fadds_ppc(f32(flower->current.pos.y), height);
        if (this->current.pos.y > top) {
            fopAcM_delete(this);
        }
    }
}

/* 02234778 */
void daNpc_Fa1_c::init_bottle_baba_move2() {

    mMode = Mode_BOTTLE_BABA_MOVE2_e;
    auto *play = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    auto *player = play->play.mPlayerInfo[0].mpPlayer.get();
    m764 = player;
    f32 speed = f32(l_HIO.m50.prm.m08);
    m798 = 0;
    speedF = speed;
    m79C = f32(l_HIO.m50.prm.m18);
    m7A0 = f32(l_HIO.m50.prm.m14);
    init_baba_down();
}

/* 02234A60 */
void daNpc_Fa1_c::bottle_baba_move2() {

    if (!m764.get()) {
        init_bottle_baba_move2();
        BGCheck();
        return;
    }
    Fa1_babaPosition(this);
    auto *move = gabi::at<fa1_hd::MovePtmf>(gabi::ea(&bottleBabaSubProc) +
                                            u32(u8(mSubMode)) * sizeof(fa1_hd::MovePtmf));
    (this->*(*move))();
    auto *flower = m764.get();
    f32 height = l_playerBabaHeight;
    f32 top = gabi::fadds_ppc(f32(flower->current.pos.y), height);
    if (this->current.pos.y > top) {
        init_bottle_baba_move();
    }
}

/* 022346FC */
void daNpc_Fa1_c::init_baba_down() {

    f32 fall = m79C;
    mSubMode = BottleBabaSubMode_BABADOWN_e;
    f32 zero = l_zero;
    u8 mode = mMode;
    this->speed.y = zero;
    maxFallSpeed = fall;
    f32 gravity = f32(l_HIO.m50.prm.m1C);
    this->gravity = gravity;
    if (mode == Mode_BOTTLE_BABA_MOVE_e) {
        s32 reverb = fa1_getReverb(s8(this->current.roomNo));
        fa1_seStart(JA_SE_OBJ_BA_FAILY_2, &this->current.pos, nullptr, reverb);
    }
}

/* 02234CA0 */
void daNpc_Fa1_c::baba_down() {

    if (f32(this->current.pos.y) < f32(m764->current.pos.y)) {
        init_baba_up();
    }
}

/* 02234C3C */
void daNpc_Fa1_c::init_baba_up() {

    u8 mode = mMode;
    f32 rise = m7A0;
    mSubMode = BottleBabaSubMode_UP2_e;
    maxFallSpeed = rise;
    if (mode == Mode_BOTTLE_BABA_MOVE2_e) {
        s32 reverb = fa1_getReverb(s8(this->current.roomNo));
        fa1_seStart(JA_SE_OBJ_BA_FAILY_1, &this->current.pos, nullptr, reverb);
    }
}

/* 02234CB8 */
void daNpc_Fa1_c::init_bigelf_change() {

    u32 flags = u32(actor_status);
    u16 angle = this->current.angle.y;
    mMode = Mode_BIGELF_CHANGE_e;
    actor_status = flags | fopAcStts_UNK4000_e;
    f32 speed = f32(l_HIO.m78.prm.m14);
    speedF = speed;
    u16 tableAngle = angle;
    this->speed.x = gabi::fmuls_ppc(speed, cM_ssin(tableAngle));
    this->speed.z = gabi::fmuls_ppc(speed, cM_scos(tableAngle));
    PSVECAdd(&this->current.pos, &this->speed, &this->current.pos);
}

/* 02234D1C */
void daNpc_Fa1_c::bigelf_change() {

    gabi::Local<cXyz> difference;
    fa1_vectorSubtract(&this->home.pos, difference.get(), &this->current.pos);
    s32 direction = fa1_atan2s(f32(difference->x), f32(difference->z));
    s16 turn = s16(l_HIO.m78.prm.m18);
    u16 angle = u16(u32(direction) + u32(s32(turn)));
    f32 speed = speedF;
    this->current.angle.y = s16(angle);
    u16 tableAngle = angle;
    this->speed.x = gabi::fmuls_ppc(speed, cM_ssin(tableAngle));
    this->speed.z = gabi::fmuls_ppc(speed, cM_scos(tableAngle));
    PSVECAdd(&this->current.pos, &this->speed, &this->current.pos);
    BGCheck();
}

/* 02233DC4 */
void daNpc_Fa1_c::findPlayer() {

    auto *play = reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get());
    auto *player = play->play.mPlayerInfo[0].mpPlayer.get();
    gabi::Local<cXyz> difference;
    fa1_vectorSubtract(&player->current.pos, difference.get(), &this->current.pos);
    s32 direction = fa1_atan2s(f32(difference->x), f32(difference->z));
    s16 target = s16(u32(direction) - u32(s32(s16(this->current.angle.y))));
    s16 neck = mMcaMorfCallback1.mNeckAngle;
    if (target < 0) {
        if (target < -0x4000) {
            target = -0x4000;
        }
        s16 step = s16(s16(target - neck) / 4);
        if (step < -0x800) {
            step = -0x800;
        }
        neck = s16(neck + step);
        if (neck < target) {
            neck = target;
        }
    } else {
        if (target > 0x4000) {
            target = 0x4000;
        }
        s16 step = s16(s16(target - neck) / 4);
        if (step > 0x800) {
            step = 0x800;
        }
        neck = s16(neck + step);
        if (neck > target) {
            neck = target;
        }
    }
    mMcaMorfCallback1.mNeckAngle = neck;
}

/* 02233984 */
BOOL daNpc_Fa1_c::_delete() {

    fa1_followCallbackRemove(&this->mSparklePtclCallback);
    if (mType == Type_BABA_e) {
        fa1_effectPointLightCut(&mPointLight.mPos);
    }
    u32 count = u32(l_hio_counter);
    if (count) {
        count -= 1;
        l_hio_counter = count;
        if (s32(count) > 0) {
            return TRUE;
        }
    }
    s8 hio = s8(l_HIO.mNo);
    if (hio >= 0) {
        fa1_hioDeleteChild(hio);
        l_HIO.mNo = -1;
    }
    return TRUE;
}

// HD: this free heap adapter preserves the verified callback ABI.
/* 02232FE0 */
BOOL CheckCreateHeap(daNpc_Fa1_c *actor) { return actor->CreateHeap(); }

/* 02233854 */
s32 daNpc_Fa1_c::_create() {

    u32 condition = u32(actor_condition);
    if (!(condition & 8)) {
        if (gabi::ea(this) != 0) {
            daNpc_Fa1_c__ct(this);
            condition = u32(actor_condition);
        }
        actor_condition = condition | 8;
    }
    if (!fa1_entrySolidHeap(this, CheckCreateHeap, 0x1100)) {
        return cPhs_ERROR_e;
    }
    u32 morf = gabi::ea(mpMorf.get());
    u32 model = gabi::ea(gabi::at<mDoExt_McaMorf>(morf)->getModel());
    cullMtx = model ? gabi::ea(&gabi::at<J3DModel>(model)->mBaseTransformMtx) : 0;
    if (s8(l_HIO.mNo) < 0) {
        // HD: the guest label retains its original Shift-JIS recovery-fairy text.
        s32 id = fa1_hioCreateChild("Recovery fairy", &l_HIO);
        l_HIO.mNo = s8(id);
        l_hio_counter = 1;
    } else {
        l_hio_counter = u32(l_hio_counter) + 1;
    }
    return createInit() ? cPhs_COMPLEATE_e : cPhs_ERROR_e;
}

/* 02232EA0 */
BOOL daNpc_Fa1_c::CreateHeap() {

    gabi::Local<SafeString> modelKey;
    modelKey->mStringTop = gabi::guest_str("Always");
    u32 manager = gabi::ea(l_resourceManager.mResource.get());
    modelKey->__vtbl = gc_tu.safeString;
    void *data =
        fa1_getObjectResource(gabi::at<void>(manager), modelKey.get(), dRes_INDEX_ALWAYS_BDL_FA_e);
    manager = gabi::ea(l_resourceManager.mResource.get());
    gabi::Local<SafeString> animationKey;
    animationKey->mStringTop = gabi::guest_str("Always");
    animationKey->__vtbl = gc_tu.safeString;
    void *animation = fa1_getObjectResource(gabi::at<void>(manager), animationKey.get(),
                                            dRes_INDEX_ALWAYS_BCK_FA_e);
    f32 rate = l_one;
    void *morf = fa1_morfCreate(nullptr, data, &mMcaMorfCallback1, nullptr, animation, 2, 0, -1, 0,
                                0, 0, 0x11020203, rate);
    mpMorf = static_cast<mDoExt_McaMorf *>(morf);
    if (!morf) {
        return FALSE;
    }
    if (!gabi::ea(static_cast<mDoExt_McaMorf *>(morf)->getModel())) {
        mpMorf = nullptr;
        return FALSE;
    }
    u32 names = gabi::ea(fa1_getJointNameResource(data));
    u32 displacement = u32(gabi::at<fa1_hd::JointNameResourcePrefix>(names)->mNameTableOffset);
    void *table = gabi::at<void>(displacement ? names + 0x10 + displacement : 0);
    s32 joint = fa1_getJointIndex(table, "neck");
    mMcaMorfCallback1.mNeckJoint = u16(joint);
    mMcaMorfCallback1.mNeckAngle = 0;
    return TRUE;
}

/* 02233378 */
void daNpc_Fa1_c::setMtx() {

    u32 morf = gabi::ea(mpMorf.get());
    f32 x = this->current.pos.x, y = this->current.pos.y, z = this->current.pos.z;
    u32 model = gabi::ea(gabi::at<mDoExt_McaMorf>(morf)->getModel());
    fa1_matrixTranslate(&l_matrixStack, x, y, z);
    fa1_matrixRotateY(&l_matrixStack, s16(this->shape_angle.y));
    // Capture the complete source matrix before writing the model destination.
    f32 matrix[12];
    for (u32 i = 0; i != 12; ++i) {
        matrix[i] = f32(l_matrixStack.m[i / 4][i % 4]);
    }
    const u8 order[12] = {1, 9, 10, 8, 7, 5, 2, 11, 3, 0, 6, 4};
    for (u8 i : order) {
        gabi::at<J3DModel>(model)->mBaseTransformMtx.m[i / 4][i % 4] = matrix[i];
    }
    mpMorf->calc();
    morf = gabi::ea(mpMorf.get());
    u16 joint = mMcaMorfCallback1.mNeckJoint;
    model = gabi::ea(gabi::at<mDoExt_McaMorf>(morf)->getModel());
    auto *matrices = gabi::at<J3DModel>(model)->mpMtxBuffer.get();
    u16 flags = u16(matrices->mFlags);
    u32 table = u32(matrices->mpAnmMtx);
    auto *selected = gabi::at<Mtx34>(table + u32(joint) * sizeof(Mtx34));
    matrices->mFlags = flags | 0x10;
    x = f32(selected->m[0][3]);
    y = f32(selected->m[1][3]);
    z = f32(selected->m[2][3]);
    this->attention_info.position.y = y;
    this->eyePos.z = z;
    this->attention_info.position.x = x;
    this->attention_info.position.z = z;
    this->eyePos.x = x;
    this->eyePos.y = y;
}

/* 02233980 */
s32 daNpc_Fa1_Create(daNpc_Fa1_c *actor) { return actor->_create(); }

/* 02233A08 */
BOOL daNpc_Fa1_Delete(daNpc_Fa1_c *actor) { return actor->_delete(); }

/* 02233B54 */
BOOL daNpc_Fa1_Execute(daNpc_Fa1_c *actor) { return actor->_execute(); }

/* 02233BDC */
BOOL daNpc_Fa1_Draw(daNpc_Fa1_c *actor) { return actor->_draw(); }

/* 02233BE0 */
BOOL daNpc_Fa1_IsDelete(daNpc_Fa1_c *actor) { return TRUE; }

// HD: compiler-generated and template code of this translation unit.

/* 0223556C */
u16 fa1_calcTimerU16(be<u16> *timer) {

    u16 value = *timer;
    if (value) {
        value = u16(value - 1);
        *timer = value;
    }
    return value;
}

/* 0223558C */
u16 fa1_getRndValueU16(u16 base, u16 range) {

    f32 random = cM_rndF(f32(range));
    return u16(gabi::ftoi(gabi::fadds_ppc(f32(base), random)));
}

/* 02235608 */
u8 fa1_getRndValueU8(u8 base, u8 range) {

    f32 random = cM_rndF(f32(range));
    return u8(gabi::ftoi(gabi::fadds_ppc(f32(base), random)));
}

/* 02235568 */
void fa1_SafeString_assureTerminationImpl_(SafeString *string) {}

/* 02235484 */
void fa1_SafeString__dt(void *string, s32 flags) {

    if (string && (flags & 1)) {
        fa1_operatorDelete(string);
    }
}

/* 02233030 */
daNpc_Fa1_c *daNpc_Fa1_c__ct(daNpc_Fa1_c *actor) {

    if (!actor) {
        actor = static_cast<daNpc_Fa1_c *>(fa1_operatorNew(0x8C0));
    }
    if (!actor) {
        return nullptr;
    }
    u32 a = gabi::ea(actor);
    fa1_npcConstructor(actor);
    actor->__vtbl = l_actorVtable;
    daNpc_Fa1_McaMorfCallBack1_c__ct(&actor->mMcaMorfCallback1);
    fa1_followCallbackConstructor(&actor->mSparklePtclCallback, 0, 0);
    fa1_groundCheckConstructor(&actor->mGndChk);
    actor->mGndChk.dBgS_GrpPassChk::__vtbl = gc_tu.gndChk.v4C;
    actor->mGndChk.dBgS_PolyPassChk::__vtbl = gc_tu.gndChk.v40;
    actor->mGndChk.cBgS_PolyInfo::__vtbl = gc_tu.gndChk.v20;
    actor->mGndChk.cBgS_Chk::__vtbl = gc_tu.gndChk.v10;
    actor->mGndChk.mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk *>(&actor->mGndChk));
    actor->mGndChk.mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk *>(&actor->mGndChk));
    actor->mGndChk.mbObjThrough = 1;
    actor->mGndChk.mbCamThrough = 0;
    actor->mGndChk.mbLinkThrough = 0;
    actor->mGndChk.mbArrowThrough = 0;
    actor->mGndChk.mbBombThrough = 0;
    actor->mGndChk.mbBoomerangThrough = 0;
    actor->mGndChk.mbRopeThrough = 0;
    actor->mGndChk.mGrp = 0xF;
    actor->mPointLight.mHD20 = l_one;
    return actor;
}

/* 0223549C */
void daNpc_Fa1_c__dt(daNpc_Fa1_c *actor, s32 flags) {

    if (!actor) {
        return;
    }
    actor->mGndChk.cBgS_PolyInfo::__vtbl = gc_tu.gndChkDt[0];
    actor->mGndChk.dBgS_PolyPassChk::__vtbl = gc_tu.gndChkDt[1];
    actor->mGndChk.dBgS_GrpPassChk::__vtbl = gc_tu.gndChkDt[2];
    fa1_groundCheckDestructor(&actor->mGndChk, 0);
    fa1_cylinderDestructor(&actor->mCyl, 2);
    fa1_collisionStatusDestructor(&actor->mStts, 2);
    fa1_circleDestructor(&actor->mAcchCir.m_cir, 2);
    actor->mObjAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcchDt[0];
    actor->mObjAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcchDt[1];
    fa1_actorGroundDestructor(&actor->mObjAcch, 0);
    fa1_actorDestructor(actor, 0);
    if (flags & 1) {
        fa1_operatorDelete(actor);
    }
}

/* 022353E4 */
void __sinit_d_a_npc_fa1_cpp() {

    l_headerState.mZero[2] = 0;
    l_headerState.mZero[0] = 0;
    l_headerState.mZero[3] = 0;
    l_headerState.mZero[1] = 0;
    fa1_registerGlobalObject(&l_headerDescriptors.mWords[0]);
    f32 timer = l_headerTimerInitial;
    f32 unused = l_headerUnusedInitial;
    l_headerState.mTimer = timer;
    l_headerState.mUnused = unused;
    fa1_headerStaticConstructor(&l_headerState.mFirst);
    fa1_registerGlobalObject(&l_headerDescriptors.mWords[3]);
    fa1_headerStaticByteConstructor(&l_headerState.mSecond);
    fa1_registerGlobalObject(&l_headerDescriptors.mWords[6]);
    daNpc_Fa1_HIO_c__ct(&l_HIO);
}

// clang-format off
/* WWHD glue
str 10019E54 "Always"
data l_matrixStack 1048D0CC
data l_resourceManager 101F4F28
data l_cyl_src 101BE2DC
data l_headerState 10466F94
data l_headerDescriptors 101BE368
data l_hioInitial 10019EF0
data l_areaHioInitial 10019ED0
data l_hoverHioInitial 10019EBC
str 10019E5C "neck"
str 10019E88 "Recovery fairy" renamed
data l_one 10019E50
data l_zero 10019E64
data l_normalGravity 10019E68
data l_normalMaxFallSpeed 10019E6C
data l_pointLightPower 10019E70
data l_pointLightFluctuation 10019E74
data l_areaRadius 10019E78
data l_playerRadius 10019E7C
data l_invalidGround 10019E80
data l_homeGroundOffset 10019E84
data l_lookRadius 10019E94
data l_lookUpper 10019E98
data l_lookLower 10019E9C
data l_moveHeight 10019EA0
data l_animationAcceleration 10019EA4
data l_animationMaxRate 10019EA8
data l_orbitRadiusStep 10019EAC
data l_flowerOffsetX 10019EB0
data l_flowerDeleteHeight 10019EB4
data l_playerBabaHeight 10019EB8
data l_headerTimerInitial 10019F38
data l_headerUnusedInitial 10019F3C
ext 02516464 BOOL fa1_cylinderCheckCoHit(void* cylinder)
ext 0200E240 void fa1_collisionSet(void* collision, void* object)
ext 020182E0 void fa1_cylinderSetCenter(void* cylinder, const cXyz* pos)
ext 024EBD80 void fa1_attentionLookRequest(void* attention, fopAc_ac_c* actor, s32 angle, s32 flags, f32 radius, f32 upper, f32 lower)
ext 025E535C void fa1_morfPlay(void* morf, s32 pos, s32 sound, s32 reverb)
ext 025F0A10 s32 fa1_hioCreateChild(const char* name, void* hio)
ext 025D63E8 BOOL fa1_entrySolidHeap(fopAc_ac_c* actor, Fa1HeapCallback callback, u32 size)
ext 025A847C void* fa1_particleSet(void* controller, s32 group, s32 effect, const cXyz* pos, s32 angle, s32 scale, s32 alpha, void* callback, s32 setup, s32 primary, s32 environment, s32 scale2D)
ext 0255B9C8 void fa1_effectPointLightSet(void* light)
ext 02516518 void fa1_cylinderSet(void* cylinder, void* source)
ext 02515F14 void fa1_collisionStatusInit(void* status, s32 weight, s32 flags, fopAc_ac_c* actor)
ext 024EBB70 void fa1_attentionCatchRequest(void* attention, fopAc_ac_c* actor, s32 item, s16 timer, s32 flags, f32 radius, f32 upper, f32 lower)
ext 025D6894 s32 fa1_searchActorAngleY(fopAc_ac_c* actor, void* target)
ext 025F1C28 void fa1_matrixRotateY(void* matrix, s16 angle)
ext 028E93CC void fa1_matrixTranslate(void* matrix, f32 x, f32 y, f32 z)
ext 027DF9B0 s32 fa1_getJointIndex(void* table, const char* name)
ext 027F68FC void* fa1_getJointNameResource(void* modelData)
ext 025E4F64 void* fa1_morfCreate(void* object, void* data, void* callback1, void* callback2, void* animation, s32 loop, s32 start, s32 end, s32 param, s32 sound, s32 flags, u32 displayFlags, f32 rate)
ext 026066C4 void* fa1_getObjectResource(void* manager, SafeString* name, s32 index)
ext 024EEEB8 u8 fa1_getPolyColor(void* background, void* poly)
ext 024EF130 u8 fa1_getRoomId(void* background, void* poly)
ext 020084C8 void* fa1_getTriPlane(void* background, u16 backgroundId, u16 index)
ext 02008974 f32 fa1_groundCross(void* background, void* check)
ext 02562F5C void fa1_setLightTevColorType(void* environment, void* model, void* tev)
ext 025626A4 void fa1_settingTevStruct(void* environment, s32 type, const cXyz* pos, void* tev)
ext 02555D0C void* fa1_getEnvironmentLight()
ext 025F0A18 void fa1_hioDeleteChild(s8 child)
ext 0255BA9C void fa1_effectPointLightCut(void* light)
ext 025A5AC8 void fa1_followCallbackRemove(void* callback)
ext 025D5578 void fa1_searchByName(s32 name, gptr<fopAc_ac_c>* result)
ext 028EAB2C void fa1_headerStaticByteConstructor(void* object)
ext 028ED6F8 void fa1_headerStaticConstructor(void* object)
ext 028F026C void fa1_registerGlobalObject(void* descriptor)
ext 025D50BC void fa1_actorDestructor(fopAc_ac_c* actor, s32 flags)
ext 024EFD9C void fa1_actorGroundDestructor(void* acch, s32 flags)
ext 02018034 void fa1_circleDestructor(void* circle, s32 flags)
ext 02515860 void fa1_collisionStatusDestructor(void* status, s32 flags)
ext 02515A70 void fa1_cylinderDestructor(void* cylinder, s32 flags)
ext 02008DAC void fa1_groundCheckDestructor(void* check, s32 flags)
ext 02008E0C void fa1_groundCheckConstructor(void* check)
ext 025A5894 void fa1_followCallbackConstructor(void* callback, s32 param, s32 flags)
ext 025A1458 void fa1_npcConstructor(fopNpc_npc_c* actor)
ext 0273AF40 void fa1_operatorDelete(void* object)
ext 0273AD10 void* fa1_operatorNew(u32 size)
ext 0201AD78 void fa1_vectorAdd(const void* a, cXyz* result, const void* b)
ext 0201ADE0 void fa1_vectorSubtract(const void* a, cXyz* result, const void* b)
ext 025E1A40 void fa1_seStart(u32 id, void* pos, void* param, s32 reverb)
data l_HIO 10466FB0
data l_hio_counter 10466F90
data moveProc 101BE320
data moveSubProc 101BE274
data bottleMoveSubProc 101BE294
data bottleBabaSubProc 101BE2AC
ext 020195B0 s32 fa1_atan2s(f32 x, f32 z)
ext 0200F93C s32 fa1_targetAngleY(const cXyz* a, const cXyz* b)
ext 02520540 s32 fa1_getReverb(s8 room)
ext 028F4384 f32 fa1_sqrtf(f32 value)
ext 0207A9A0 u8 fa1_calcMoveTimer(be<u8>* timer)
ext 02055B64 s16 fa1_calcAreaTimer(be<s16>* timer)
ext 021E1E78 u32 fa1_randomTimer(u32 base, u32 range)
ext 0255F360 f32 fa1_yuragiRatio(f32 value)
*/
// clang-format on
