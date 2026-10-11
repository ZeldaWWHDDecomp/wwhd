/** Hyrule Castle pushable triangle blocks.
  * Verified HD behavior, with GameCube method names and measured actor fields.
  * HD: the actor base grows by 0x11C; models use SafeString resources and HD animation/callback layouts.
  * HD: GHS floating-point order, overlapping input words and PTMF guest wrapping are retained.
  */
#include "d/actor/d_a_obj_tribox.h"
using daObjTribox::Act_c;
static u8 M_angle_dtor;
static u8 M_random_dtor;
static u8 M_random_fast_dtor;
static u8 M_line_dtor;
static u8 M_random;
static u8 M_random_fast;

static TriboxModeTable M_mode_table;
static TriboxCreateTable M_create_table;

static constexpr struct {
    u32 safeStringVtable;
    u32 actorVtable;
    u32 linePolyPassDtorVtable;
    u32 groupPassDtorVtable;
    u32 linePolyInfoDtorVtable;
    u32 groundCheckVtable;
    u32 groundPolyInfoVtable;
    u32 groundPolyPassVtable;
    u32 groundGroupPassVtable;
    u32 groundPolyInfoDtorVtable;
    u32 groundPolyPassDtorVtable;
    u32 lineGroupPassVtable;
    u32 lineCheckVtable;
    u32 linePolyInfoVtable;
    u32 linePolyPassVtable;
    u32 archiveName;
} tribox_tu = {
    0x100318A8,
    0x100319E0,
    0x10031990,
    0x100318D0,
    0x100318C0,
    0x10031920,
    0x10031930,
    0x10031950,
    0x10031940,
    0x100318F0,
    0x10031910,
    0x100319C0,
    0x100319A0,
    0x100319B0,
    0x100319D0,
    0x10031C00,
};

static cXyz M_vertex[3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
static dBgS_ObjLinChk M_linChk;
static Mtx34 mtx;
static TriboxPointer l_save;
static TriboxPointer l_resControl;
static s16 M_direction_angle[3] = {0, 21845, -21845};
static u32 M_random_state[4] = {0, 0, 0, 0};

static s32 M_correct_cnt;
static s32 M_b_cont_cnt;
static s32 M_c_cont_cnt;
static u8 M_correct_flag;
static u8 M_sink_start;
static f32 M_sink_height;
static f32 M_half_width;
static f32 M_triangle_height;
static f32 M_double_height;
static f32 M_angle_min;
static f32 M_angle_max;
static u32 M_smoke_scale_initialized;
static cXyz M_sound_pos;
static cXyz M_smoke_scale;
static cXyz M_default_sound_pos;

static inline void copy_model_matrix(u32 model) {
    f32 matrix[12];
    auto* source = &mtx;
    auto* destination = &gabi::at<J3DModel>(model)->mBaseTransformMtx;
    for (u32 i = 0; i < 12; ++i)
        matrix[i] = source->m[i / 4][i % 4];
    for (u32 i = 0; i < 12; ++i)
        destination->m[i / 4][i % 4] = matrix[i];
}

static inline void shared_sound(Act_c *a, u32 id) {
    if ((u8)a->mLeader) {
        u32 map = tribox_sound_get_mapinfo(a, &M_sound_pos);
        s8 room = (s8)a->current.roomNo;
        s32 reverb = tribox_dComIfGp_getReverb(room);
        tribox_JAIZelBasic_seStartPos(id, &M_sound_pos, map, reverb);
    }
}

// HD: dispatch preserves the signed GHS this adjustment and virtual-index encoding.
static inline s32 dispatch_member(Act_c* i_this, u32 entry) {
    using Method = s32 (Act_c::*)();
    const auto& method = *gabi::at<gabi::ptmf<Method>>(entry);
    return (i_this->*method)();
}

static inline void copy_vector(cXyz* dst, const cXyz* src) {
    auto* wordsDst = gabi::at<TriboxVectorWords>(gabi::ea(dst));
    auto* wordsSrc = gabi::at<TriboxVectorWords>(gabi::ea(src));
    for (u32 i = 0; i < 3; ++i)
        wordsDst->mWords[i] = (u32)wordsSrc->mWords[i];
}

static inline void vertical_direction(cXyz *p) {
    p->x = 0.0f;
    p->y = 1.0f;
    p->z = 0.0f;
}

/* 023A87EC */
void daObjTribox::Act_c::set_state() {
    u32 type = tribox_parameter(this, 1, 16);
    u32 sw = tribox_parameter(this, 8, 8);
    u32 save = gabi::ea((u8*)l_save.value);
    s8 room = (s8)this->home.roomNo;
    bool enabled =
            tribox_dSv_info_c_isSwitch(gabi::at<u8>(save + 0x20), sw, room) != 0;
    mState = type ? (enabled ? STATE_CORRECT_AFTER : STATE_CORRECT_BEFORE)
                                : (enabled ? STATE_BLOCK_AFTER : STATE_BLOCK_BEFORE);
}

/* 023A92BC */
BOOL daObjTribox::Act_c::create_heap() {
    gabi::Local<SafeString> blockName;
    blockName->mStringTop = tribox_tu.archiveName;
    blockName->__vtbl = tribox_tu.safeStringVtable;
    void *blockData =
            tribox_dRes_control_c_getRes((u8*)l_resControl.value,
                                                  blockName.get(), BDL_BLOCK);
    if (!blockData)
        tribox_JUTAssertion_showAssert("d_a_obj_tribox.cpp", 0x140, "model_data != 0");
    this->mBlockModel =
            tribox_mDoExt_J3DModel__create(blockData, 0x80000, 0x11000022);
    gabi::Local<SafeString> collisionName;
    collisionName->mStringTop = tribox_tu.archiveName;
    collisionName->__vtbl = tribox_tu.safeStringVtable;
    void *collision =
            tribox_dRes_control_c_getRes((u8*)l_resControl.value,
                                                  collisionName.get(), DZB_BLOCK);
    if (!collision)
        tribox_JUTAssertion_showAssert("d_a_obj_tribox.cpp", 0x149, "bgw_data != 0");
    u32 model = gabi::ea((J3DModel *)this->mBlockModel);
    if (model)
        this->mBackground =
                tribox_dBgW_NewSet(collision, 1, gabi::at<u8>(model + 0xC8));
    else
        this->mBackground = 0;
    gabi::Local<SafeString> lightName;
    lightName->mStringTop = tribox_tu.archiveName;
    lightName->__vtbl = tribox_tu.safeStringVtable;
    void *lightData =
            tribox_dRes_control_c_getRes((u8*)l_resControl.value,
                                                  lightName.get(), BDL_LIGHT);
    if (!lightData)
        tribox_JUTAssertion_showAssert("d_a_obj_tribox.cpp", 0x159, "model_ytfbl_data != 0");
    this->mLightModel =
            tribox_mDoExt_J3DModel__create(lightData, 0x80000, 0x11000022);
    gabi::Local<SafeString> blockAnimationName;
    blockAnimationName->mStringTop = tribox_tu.archiveName;
    blockAnimationName->__vtbl = tribox_tu.safeStringVtable;
    void *blockAnimation =
            tribox_dRes_control_c_getRes((u8*)l_resControl.value,
                                                  blockAnimationName.get(), BRK_BLOCK);
    if (!blockAnimation)
        tribox_JUTAssertion_showAssert("d_a_obj_tribox.cpp", 0x163, "brk_app_data != 0");
    s32 blockOk = tribox_mDoExt_brkAnm_initialize(reinterpret_cast<u8*>(&this->mBlockBrk), lightData,
                                                                blockAnimation, 1, 0, 1.0f, 0, -1, 0, 0);
    gabi::Local<SafeString> lightAnimationName;
    lightAnimationName->mStringTop = tribox_tu.archiveName;
    lightAnimationName->__vtbl = tribox_tu.safeStringVtable;
    void *lightAnimation =
            tribox_dRes_control_c_getRes((u8*)l_resControl.value,
                                                  lightAnimationName.get(), BRK_LIGHT);
    if (!lightAnimation)
        tribox_JUTAssertion_showAssert("d_a_obj_tribox.cpp", 0x16D, "brk_cmn_data != 0");
    s32 lightOk = tribox_mDoExt_brkAnm_initialize(reinterpret_cast<u8*>(&this->mLightBrk), lightData,
                                                                lightAnimation, 1, 2, 1.0f, 0, -1, 0, 0);
    BOOL result =
            this->mBlockModel && this->mBackground && this->mLightModel && blockOk && lightOk;
    if (!result)
        this->mBackground = 0;
    return result;
}

/* 023A96C0 */
void daObjTribox::Act_c::block_init() {
    u32 model = gabi::ea((J3DModel*)mBlockModel);
    this->cullMtx = model ? model + 0xC8 : 0;
    tribox_init_mtx(this);
    tribox_fopAcM_setCullSizeSphere(this, 0.0f, 125.0f, 0.0f, 200.0f);
    u32 play = tribox_dComIfGp_get();
    tribox_dBgS_Regist(reinterpret_cast<u8*>(&gabi::at<dComIfG_inf_c>(play)->play.mBgS), (u8 *)this->mBackground, this);
    u32 bg = gabi::ea((u8 *)this->mBackground);
    gabi::at<TriboxBackground>(bg)->m_crr_func = 0x024EE708;
    tribox_dBgW_Move((u8 *)this->mBackground);
    bg = gabi::ea((u8 *)this->mBackground);
    gabi::at<TriboxBackground>(bg)->mpPushPullCb = 0x023A956C;
    M_correct_flag = 0;
    f32 z = this->current.pos.z;
    this->mRotationStep = 0;
    this->mWalkStart.z = z;
    f32 y = this->current.pos.y;
    this->mPushSide = 0;
    this->mWalkStart.y = y;
    this->mPushDirection = 0;
    this->mPivot = 0;
    this->mPreviousPushInput = 0;
    this->mWalkPull = 0;
    this->mPushInput = 0;
    this->mWalkDirection = 0;
    this->mPreviousDirection = 0;
    this->mWalkInput = 0;
    this->mPull = 0;
    this->mWalkSide = 0;
    this->mPreviousSide = 0;
    this->mPreviousPull = 0;
    this->mRotationSign = 1;
    f32 x = this->current.pos.x;
    this->mFlash = 0;
    this->mWalkStart.x = x;
    tribox_mode_block_wait_init(this);
}

/* 023A97D4 */
s32 daObjTribox::Act_c::chk_light() {
    u32 save = gabi::ea((u8*)l_save.value);
    return tribox_dSv_event_c_isEventBit(gabi::at<u8>(save + 0x644), 0x3820) ? 2
                                                                                                                                                  : 0;
}

/* 023A982C */
void daObjTribox::Act_c::correct_before_init() {
    tribox_fopAcM_setCullSizeSphere(this, 0.0f, 125.0f, 0.0f, 200.0f);
    if ((u8)this->mLeader) {
        u32 play = tribox_dComIfGp_get();
        s16 event = tribox_dEvent_manager_c_getEventIdx(reinterpret_cast<u8*>(&gabi::at<dComIfG_inf_c>(play)->play.mEvtManager),
                                                                "MtryB_sink", 255);
        this->mEvent = event;
    } else
        this->mEvent = -1;
    tribox_mode_correct_on_init(this);
}

/* 023A98D0 */
void daObjTribox::Act_c::correct_after_init() {
    f32 homeY = (f32)this->home.pos.y,
            sinkHeight = (f32)M_sink_height;
    this->current.pos.y = homeY + sinkHeight;
    u32 model = gabi::ea((J3DModel *)this->mBlockModel);
    this->cullMtx = model ? model + 0xC8 : 0;
    tribox_init_mtx(this);
    tribox_fopAcM_setCullSizeSphere(this, 0.0f, 125.0f, 0.0f, 200.0f);
    u32 play = tribox_dComIfGp_get();
    tribox_dBgS_Regist(reinterpret_cast<u8*>(&gabi::at<dComIfG_inf_c>(play)->play.mBgS), (u8 *)this->mBackground, this);
    u32 bg = gabi::ea((u8 *)this->mBackground);
    gabi::at<TriboxBackground>(bg)->m_crr_func = 0;
    tribox_dBgW_Move((u8 *)this->mBackground);
    bg = gabi::ea((u8 *)this->mBackground);
    gabi::at<TriboxBackground>(bg)->mpPushPullCb = 0;
    u8 light = tribox_chk_light(this);
    this->mFlash = light;
    tribox_mode_correct_dummy_init(this);
}

/* 023A890C */
void daObjTribox::Act_c::controll_set() {
    u32 base = gabi::ea(this);
    if ((u8)this->mInitState)
        return;
    u32 type = tribox_parameter(this, 1, 16);
    if (type == 1) {
        tribox_fpcPi_setPriority(gabi::at<u8>(base + 0x68), -3, 2, 0xFFFD);
        u32 count = (u32)M_c_cont_cnt;
        if (!count) {
            this->mLeader = 1;
            M_sink_start = 0;
            M_correct_cnt = 0;
            M_correct_flag = 0;
        } else
            this->mLeader = 0;
        count = (u32)M_c_cont_cnt;
        M_c_cont_cnt = count + 1;
        tribox_sound_pos_init(this);
    } else {
        u32 count = (u32)M_b_cont_cnt;
        this->mLeader = count == 0;
        count = (u32)M_b_cont_cnt;
        M_b_cont_cnt = count + 1;
    }
}

/* 023A8B48 */
void daObjTribox::Act_c::controll_clear() {
    u32 type = tribox_parameter(this, 1, 16);
    auto* counter = type == 1 ? &M_c_cont_cnt : &M_b_cont_cnt;
    s32 count = (s32)((u32)*counter - 1);
    *counter = count;
    if (count < 0)
        tribox_JUTAssertion_showAssert("d_a_obj_tribox.cpp", type == 1 ? 0x214 : 0x217,
                              type == 1 ? "M_c_cont_cnt >= 0" : "M_b_cont_cnt >= 0");
}

/* 023A9998 */
s32 daObjTribox::Act_c::create_block_before() {
    s32 phase = tribox_dComIfG_resLoad(&this->mPhase, "MtryB");
    if (phase == 4) {
        if (!tribox_fopAcM_entrySolidHeap(this, 0x023A952C, 0x11C0))
            return 5;
        tribox_block_init(this);
    }
    return phase;
}

/* 023AB39C */
s32 daObjTribox::Act_c::create_block_after() {
    return 5;
}

/* 023A9A18 */
s32 daObjTribox::Act_c::create_correct_before() {
    tribox_correct_before_init(this);
    return 4;
}

/* 023A9A3C */
s32 daObjTribox::Act_c::create_correct_after() {
    s32 phase = tribox_dComIfG_resLoad(&this->mPhase, "MtryB");
    if (phase == 4) {
        if (!tribox_fopAcM_entrySolidHeap(this, 0x023A952C, 0x4000))
            return 5;
        tribox_correct_after_init(this);
    }
    return phase;
}

/* 023A8A18 */
s32 daObjTribox::Act_c::_create() {
    u32 flags = (u32)this->actor_condition;
    if (!(flags & 8)) {
        if (gabi::ea(this) != 0) {
            tribox_fopAc_ac_c_fopAc_ac_c(this);
            this->__vtbl = tribox_tu.actorVtable;
            tribox_mDoExt_brkAnm_init(reinterpret_cast<u8*>(&this->mBlockBrk));
            tribox_mDoExt_brkAnm_init(reinterpret_cast<u8*>(&this->mLightBrk));
            tribox_dPa_smokeEcallBack_dPa_smokeEcallBack(reinterpret_cast<u8*>(&this->mSmokeCallback), 1);
            tribox_dPa_footEcallBack_construct(reinterpret_cast<u8*>(&this->mSinkCallbacks[0]), 1, 1, 0, 0);
            tribox_dPa_footEcallBack_construct(reinterpret_cast<u8*>(&this->mSinkCallbacks[1]), 1, 1, 0, 0);
            tribox_dPa_footEcallBack_construct(reinterpret_cast<u8*>(&this->mSinkCallbacks[2]), 1, 1, 0, 0);
            flags = (u32)this->actor_condition;
        }
        this->actor_condition = flags | 8;
    }
    tribox_set_state(this);
    tribox_controll_set(this);
    return dispatch_member(this, gabi::ea(&M_create_table) + (u32)this->mState * sizeof(gabi::ptmf<s32 (Act_c::*)()>));
}

/* 023A8C48 */
BOOL daObjTribox::Act_c::_delete() {
    tribox_sound_pos_delete(this);
    tribox_controll_clear(this);
    tribox_eff_smoke_remove(this);
    tribox_eff_sink_smoke_remove(this);
    s32 state = this->mState;
    if (state == STATE_BLOCK_BEFORE || state == STATE_CORRECT_AFTER) {
        u32 bg = gabi::ea((u8 *)this->mBackground);
        if (bg) {
            gabi::at<TriboxBackground>(bg)->mpPushPullCb = 0;
            bg = gabi::ea((u8 *)this->mBackground);
            if (bg && (u32)gabi::at<TriboxBackground>(bg)->m_id < 0x100) {
                u32 play = tribox_dComIfGp_get();
                tribox_cBgS_Release(reinterpret_cast<u8*>(&gabi::at<dComIfG_inf_c>(play)->play.mBgS),
                                      (u8 *)this->mBackground);
            }
        }
        tribox_dComIfG_resDelete(&this->mPhase, "MtryB");
    }
    return TRUE;
}

/* 023A8D3C */
void daObjTribox::Act_c::set_mtx() {
    f32 x = this->current.pos.x, y = this->current.pos.y, z = this->current.pos.z;
    tribox_PSMTXTrans(reinterpret_cast<u8*>(&mtx), x, y, z);
    s16 ax = this->shape_angle.x, ay = this->shape_angle.y, az = this->shape_angle.z;
    tribox_mDoMtx_ZXYrotM(reinterpret_cast<u8*>(&mtx), ax, ay, az);
    copy_model_matrix(gabi::ea((J3DModel *)this->mBlockModel));
    f32 height = (f32)(&mtx)->m[1][3] + 251.0f;
    (&mtx)->m[1][3] = height;
    copy_model_matrix(gabi::ea((J3DModel *)this->mLightModel));
}

/* 023A9530 */
void daObjTribox::Act_c::init_mtx() {
    u32 model = gabi::ea((J3DModel *)this->mBlockModel);
    f32 x = this->scale.x, y = this->scale.y, z = this->scale.z;
    gabi::at<J3DModel>(model)->mBaseScale.x = x;
    gabi::at<J3DModel>(model)->mBaseScale.y = y;
    gabi::at<J3DModel>(model)->mBaseScale.z = z;
    model = gabi::ea((J3DModel *)this->mLightModel);
    x = this->scale.x;
    y = this->scale.y;
    z = this->scale.z;
    gabi::at<J3DModel>(model)->mBaseScale.x = x;
    gabi::at<J3DModel>(model)->mBaseScale.y = y;
    gabi::at<J3DModel>(model)->mBaseScale.z = z;
    tribox_set_mtx(this);
}

/* 023A9ABC */
BOOL daObjTribox::Act_c::line_cross(const cXyz *start, const cXyz *end) {
    tribox_dBgS_LinChk_Set(reinterpret_cast<u8*>(&M_linChk), start, end, this);
    M_linChk.mActorPid = (u32)mBsPcId;
    u32 play = tribox_dComIfGp_get();
    return tribox_cBgS_LineCross(reinterpret_cast<u8*>(&gabi::at<dComIfG_inf_c>(play)->play.mBgS),
                                                    reinterpret_cast<u8*>(&M_linChk));
}

/* 023A9B18 */
BOOL daObjTribox::Act_c::chk_wall(s32 kind) {
    if (kind != 1 && kind != 2)
        tribox_JUTAssertion_showAssert("d_a_obj_tribox.cpp", 0x30C, "(num == 1) || (num == 2)");
    s16 sign = (s16)this->mRotationSign;
    u32 pivot = (u32)this->mPivot;
    s32 next = (s32)(pivot + (sign < 0 ? 2u : 1u));
    next %= 3;
    u32 corner = gabi::ea(&M_vertex[0]) + (u32)next * 12;
    s16 angle = this->shape_angle.y;
    tribox_mDoMtx_YrotS(reinterpret_cast<u8*>(&mtx), angle);
    gabi::Local<cXyz> origin, rotated, end, temporary, scaled, scratch;
    tribox_PSMTXMultVecSR(reinterpret_cast<u8*>(&mtx), gabi::at<cXyz>(corner),
                          rotated.get());
    f32 y = this->current.pos.y, x = this->current.pos.x, z = this->current.pos.z;
    origin->x = x;
    origin->y = y + 5.0f;
    origin->z = z;
    tribox_cXyz___ml(rotated.get(), scratch.get(), 0.9900000095367432f);
    tribox_cXyz___mi(origin.get(), scaled.get(), scratch.get());
    copy_vector(end.get(), scaled.get());
    if (tribox_line_cross(this, origin.get(), end.get()))
        return TRUE;
    if (kind != 2)
        return FALSE;
    tribox_cXyz___mi(origin.get(), temporary.get(), rotated.get());
    copy_vector(origin.get(), temporary.get());
    sign = (s16)this->mRotationSign;
    tribox_mDoMtx_YrotM(reinterpret_cast<u8*>(&mtx), sign < 0 ? -0x2AAA : 0x2AAA);
    tribox_PSMTXMultVecSR(reinterpret_cast<u8*>(&mtx), gabi::at<cXyz>(corner),
                          rotated.get());
    tribox_cXyz___ml(rotated.get(), scaled.get(), 0.49000000953674316f);
    tribox_cXyz___mi(origin.get(), temporary.get(), scaled.get());
    copy_vector(end.get(), temporary.get());
    if (tribox_line_cross(this, origin.get(), end.get()))
        return TRUE;
    f32 high = origin->y;
    origin->y = high + 30.0f;
    tribox_cXyz___ml(rotated.get(), temporary.get(), 0.9900000095367432f);
    tribox_cXyz___mi(origin.get(), scaled.get(), temporary.get());
    copy_vector(end.get(), scaled.get());
    return tribox_line_cross(this, origin.get(), end.get()) ? TRUE : FALSE;
}

/* 023A9DA0 */
BOOL daObjTribox::Act_c::chk_space() {
    u32 direction = (u32)this->mWalkPull == 1 ? 2 : 1;
    return tribox_chk_wall(this, direction) ^ 1;
}

/* 023A9DD8 */
void daObjTribox::Act_c::eff_flash() {
    gabi::Local<cXyz> position;
    f32 x = this->current.pos.x, y = this->current.pos.y, z = this->current.pos.z;
    position->x = x;
    position->y = y + 251.0f;
    position->z = z;
    u32 play = tribox_dComIfGp_get(),
            particles = gabi::ea((dPa_control_c*)gabi::at<dComIfG_inf_c>(play)->play.mParticle);
    tribox_dPa_control_c_set(gabi::at<u8>(particles), 1, 0x833A, position.get(),
                          &this->shape_angle, 0, 255, 0, -1, 0, 0, 0);
}

/* 023A9FC0 */
void daObjTribox::Act_c::eff_smoke_start() {
    if (!(u32)M_smoke_scale_initialized) {
        M_smoke_scale_initialized = 1;
        M_smoke_scale.x = 0.6000000238418579f;
        M_smoke_scale.z = 0.6000000238418579f;
        M_smoke_scale.y = 0.6000000238418579f;
    }
    tribox_eff_smoke_pos(this);
    s8 room = (s8)this->current.roomNo;
    u32 play = tribox_dComIfGp_get(),
            particles = gabi::ea((dPa_control_c*)gabi::at<dComIfG_inf_c>(play)->play.mParticle);
    u32 emitter =
            tribox_dPa_control_c_set_abi2(gabi::at<u8>(particles), 2, 0x2022,
                                            reinterpret_cast<cXyz*>(&this->mSmokePos),
                                            reinterpret_cast<csXyz*>(&this->mSmokeAngle), &M_smoke_scale,
                                            0xB9, reinterpret_cast<u8*>(&this->mSmokeCallback), room, 0, 0, 0);
    if (emitter) {
        gabi::at<TriboxSmokeEmitter>(emitter)->mInitialVelRndm = 15.0f;
        gabi::at<TriboxSmokeEmitter>(emitter)->mSpread = 0.15000000596046448f;
        gabi::at<TriboxSmokeEmitter>(emitter)->mRate = 1.0f;
        gabi::at<TriboxSmokeEmitter>(emitter)->mLifeTime = 30;
    }
}

/* 023A9E64 */
void daObjTribox::Act_c::eff_smoke_pos() {
    s16 sign = (s16)this->mRotationSign;
    u32 index = (u32)this->mPivot;
    s32 first = (s32)(index + (sign < 0 ? 2u : 1u)),
            second = (s32)(index + (sign < 0 ? 1u : 2u));
    first %= 3;
    second %= 3;
    s16 angle = this->shape_angle.y;
    tribox_mDoMtx_YrotS(reinterpret_cast<u8*>(&mtx), angle);
    gabi::Local<cXyz> edge1, edge2, difference, normalized, offset, position;
    tribox_PSMTXMultVecSR(reinterpret_cast<u8*>(&mtx),
                          gabi::at<cXyz>(gabi::ea(&M_vertex[0]) + (u32)first * 12), edge1.get());
    tribox_PSMTXMultVecSR(reinterpret_cast<u8*>(&mtx),
                          gabi::at<cXyz>(gabi::ea(&M_vertex[0]) + (u32)second * 12), edge2.get());
    tribox_cXyz___mi(edge2.get(), difference.get(), edge1.get());
    tribox_cXyz_normalizeZP(difference.get(), normalized.get());
    tribox_PSVECScale(difference.get(), difference.get(), 10.0f);
    tribox_cXyz___pl(edge1.get(), offset.get(), difference.get());
    tribox_cXyz___pl(offset.get(), position.get(), &this->current.pos);
    u32 x = (u32)gabi::at<TriboxVectorWords>(gabi::ea(position.get()))->mWords[0],
            y = (u32)gabi::at<TriboxVectorWords>(gabi::ea(position.get()))->mWords[1],
            z = (u32)gabi::at<TriboxVectorWords>(gabi::ea(position.get()))->mWords[2];
    this->mSmokeAngle.x = 0;
    gabi::at<TriboxVectorWords>(gabi::ea(&mSmokePos))->mWords[1] = y;
    gabi::at<TriboxVectorWords>(gabi::ea(&mSmokePos))->mWords[2] = z;
    gabi::at<TriboxVectorWords>(gabi::ea(&mSmokePos))->mWords[0] = x;
    s16 yaw = tribox_cLib_targetAngleY(edge1.get(), edge2.get());
    this->mSmokeAngle.z = 0;
    this->mSmokeAngle.y = yaw;
}

/* 023AA0AC */
void daObjTribox::Act_c::eff_smoke_end() {
    tribox_dPa_smokeEcallBack_end(reinterpret_cast<u8*>(&this->mSmokeCallback));
}

/* 023A8BD4 */
void daObjTribox::Act_c::eff_smoke_remove() {
    mSmokeCallback.remove();
}

/* 023AA0B4 */
void daObjTribox::Act_c::eff_sink_smoke_init() {
    this->mSinkSmokeActive = 0;
}

/* 023AA0C0 */
void daObjTribox::Act_c::eff_sink_smoke_start() {
    u32 base = gabi::ea(this);
    if ((u8)this->mSinkSmokeActive)
        return;
    this->mSinkSmokeActive = 1;
    gabi::Local<csXyz> angles;
    for (u32 i = 0; i < 3; ++i) {
        s16 z = this->shape_angle.z, y = this->shape_angle.y, x = this->shape_angle.x;
        angles->z = z;
        angles->y = (s16)(y + i * 0x5555);
        angles->x = x;
        u32 callback = base + 0x574 + i * 0x20;
        u32 play = tribox_dComIfGp_get(),
                particles = gabi::ea((dPa_control_c*)gabi::at<dComIfG_inf_c>(play)->play.mParticle);
        tribox_dPa_control_c_set_abi3(gabi::at<u8>(particles), 2, 0xA320, &this->current.pos,
                              angles.get(), 0, 0xA0, gabi::at<u8>(callback), -1, 0, 0, 0);
    }
}

/* 023AA194 */
void daObjTribox::Act_c::eff_sink_smoke_end() {
    if ((u8)this->mSinkSmokeActive) {
        this->mSinkSmokeActive = 0;
        tribox_dPa_smokeEcallBack_end(reinterpret_cast<u8*>(&this->mSinkCallbacks[0]));
        tribox_dPa_smokeEcallBack_end(reinterpret_cast<u8*>(&this->mSinkCallbacks[1]));
        tribox_dPa_smokeEcallBack_end(reinterpret_cast<u8*>(&this->mSinkCallbacks[2]));
    }
}

/* 023A8BE4 */
void daObjTribox::Act_c::eff_sink_smoke_remove() {
    for (u32 i = 0; i < 3; ++i)
        mSinkCallbacks[i].remove();
}

/* 023AA1E8 */
void daObjTribox::Act_c::vib_sink_init() {
    this->mSinkVibrationActive = 0;
}

/* 023AA1F4 */
void daObjTribox::Act_c::vib_sink_start() {
    if ((u8)this->mSinkVibrationActive)
        return;
    this->mSinkVibrationActive = 1;
    u32 play = tribox_dComIfGp_get(), vibration = gabi::ea(&gabi::at<dComIfG_inf_c>(play)->play.mVibration);
    gabi::Local<cXyz> direction;
    vertical_direction(direction.get());
    tribox_dVibration_c_StartShock(gabi::at<u8>(vibration), 2, 1, direction.get());
    vertical_direction(direction.get());
    tribox_dVibration_c_StartShock(gabi::at<u8>(vibration), 1, 6, direction.get());
    vertical_direction(direction.get());
    tribox_dVibration_c_StartQuake(gabi::at<u8>(vibration), 3, 15, direction.get());
}

/* 023AA2D8 */
void daObjTribox::Act_c::vib_sink_end() {
    if (!(u8)this->mSinkVibrationActive)
        return;
    this->mSinkVibrationActive = 0;
    u32 play = tribox_dComIfGp_get(), vibration = gabi::ea(&gabi::at<dComIfG_inf_c>(play)->play.mVibration);
    tribox_dVibration_c_StopQuake(gabi::at<u8>(vibration), -1);
    gabi::Local<cXyz> direction;
    vertical_direction(direction.get());
    tribox_dVibration_c_StartShock(gabi::at<u8>(vibration), 3, 15, direction.get());
}

/* 023A887C */
void daObjTribox::Act_c::sound_pos_init() {
    u32 base = gabi::ea(this);
    u8 first = (u8)this->mLeader;
    gabi::Local<cXyz> scaled;
    tribox_cXyz___ml(gabi::at<cXyz>(base + 0x2EC), scaled.get(),
                          0.3333333432674408f);
    if (first) {
        f32 z = scaled->z, y = scaled->y, x = scaled->x;
        M_sound_pos.z = z;
        M_sound_pos.y = y;
        M_sound_pos.x = x;
    } else
        tribox_PSVECAdd(&M_sound_pos, scaled.get(),
                              &M_sound_pos);
}

/* 023A8EF4 */
u32 daObjTribox::Act_c::sound_get_mapinfo(const cXyz *position) {
    gabi::Local<TriboxGroundCheck> check;
    tribox_cBgS_GndChk_cBgS_GndChk(check.get());
    check->mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk*>(check.get()));
    check->mbArrowThrough = 0;
    check->mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk*>(check.get()));
    f32 x = position->x, y = position->y, z = position->z;
    check->mbCamThrough = 0;
    check->mGrp = 1;
    check->mbRopeThrough = 0;
    check->mbLinkThrough = 0;
    check->cBgS_Chk::__vtbl = tribox_tu.groundCheckVtable;
    check->mbObjThrough = 1;
    check->mbBombThrough = 0;
    check->m_pos.z = z;
    check->cBgS_PolyInfo::__vtbl = tribox_tu.groundPolyInfoVtable;
    check->dBgS_PolyPassChk::__vtbl = tribox_tu.groundPolyPassVtable;
    check->m_pos.y = y + 50.0f;
    check->mbBoomerangThrough = 0;
    check->m_pos.x = x;
    check->dBgS_GrpPassChk::__vtbl = tribox_tu.groundGroupPassVtable;
    u32 play = tribox_dComIfGp_get();
    tribox_cBgS_GroundCross(reinterpret_cast<u8*>(&gabi::at<dComIfG_inf_c>(play)->play.mBgS), check.get());
    u32 material = 13;
    if ((u16)check->mBgIndex < 0x100) {
        play = tribox_dComIfGp_get();
        material = tribox_dBgS_GetMtrlSndId(reinterpret_cast<u8*>(&gabi::at<dComIfG_inf_c>(play)->play.mBgS),
                                                              reinterpret_cast<u8*>(static_cast<cBgS_PolyInfo*>(check.get())));
    }
    check->cBgS_PolyInfo::__vtbl = tribox_tu.groundPolyInfoDtorVtable;
    check->dBgS_PolyPassChk::__vtbl = tribox_tu.groundPolyPassDtorVtable;
    check->dBgS_GrpPassChk::__vtbl = tribox_tu.groupPassDtorVtable;
    tribox_cBgS_Chk_cBgS_Chk(check.get(), 0);
    return material;
}

/* 023AA3FC */
void daObjTribox::Act_c::sound_sink_down_block() {
    shared_sound(this, 0x6222);
}

/* 023AA474 */
void daObjTribox::Act_c::sound_sink_stop_block() {
    shared_sound(this, 0x6A2C);
}

/* 023AA4EC */
void daObjTribox::Act_c::sound_flash_shine() {
    shared_sound(this, 0x6A24);
}

/* 023A9018 */
void daObjTribox::Act_c::sound_flash_light() {
    shared_sound(this, 0x6223);
}

/* 023A96A4 */
void daObjTribox::Act_c::mode_block_wait_init() {
    u32 flags = (u32)this->actor_status;
    mMode = MODE_BLOCK_WAIT;
    this->mHoldTimer = 0;
    this->actor_status = flags | 0x80;
}

/* 023AA618 */
void daObjTribox::Act_c::mode_block_wait() {
    if ((u8)M_correct_flag) {
        this->mHoldTimer = 0;
        tribox_mode_block_sink_init(this);
        return;
    }
    if (!(u8)this->mPushInput ||
            (u32)this->mPushDirection != (u32)this->mPreviousDirection ||
            (u32)this->mPushSide != (u32)this->mPreviousSide ||
            (u32)this->mPull != (u32)this->mPreviousPull) {
        this->mHoldTimer = 0;
        return;
    }
    s16 timer = (s16)((s16)this->mHoldTimer + 1);
    this->mHoldTimer = timer;
    if (timer < 4)
        return;
    u32 sign = (u32)this->mPushSide, pull;
    bool positive = false;
    if (sign == 0 && (u32)this->mPull == 1)
        positive = true;
    else if (sign == 1 && (u32)this->mPull == 0)
        positive = true;
    this->mRotationSign = positive ? 1 : -1;
    sign = (u32)this->mPushSide;
    u32 direction = (u32)this->mPushDirection, addition = sign ? 1 : 2;
    pull = (u32)this->mPull;
    s32 combined = (s32)(direction + addition);
    s32 pivot = combined % 3;
    this->mWalkPull = pull;
    sign = (u32)this->mPushSide;
    u32 input = (u32)this->mPushInputWord;
    this->mWalkInputWord = input;
    this->mWalkSide = sign;
    this->mWalkDirection = direction;
    this->mPivot = pivot;
    if (tribox_chk_space(this))
        tribox_mode_block_walk_init(this);
}

/* 023AA5B0 */
void daObjTribox::Act_c::mode_block_walk_init() {
    u32 flags = (u32)this->actor_status;
    f32 x = this->current.pos.x, y = this->current.pos.y, z = this->current.pos.z;
    this->mWalkStart.x = x;
    this->mWalkStart.y = y;
    this->actor_status = flags & ~0x80u;
    this->mWalkStart.z = z;
    mMode = MODE_BLOCK_WALK;
    this->mModeTimer = 20;
    tribox_eff_smoke_start(this);
    u32 play = tribox_dComIfGp_get(),
            player = gabi::ea((fopAc_ac_c*)gabi::at<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer),
            status = (u32)gabi::at<daPy_py_c>(player)->mNoResetFlg0;
    gabi::at<daPy_py_c>(player)->mNoResetFlg0 = status | daPy_py_c::daPyFlg0_PUSH_PULL_KEEP;
}

/* 023AA7A4 */
void daObjTribox::Act_c::mode_block_walk() {
    u32 base = gabi::ea(this);
    s16 timer = (s16)((s16)this->mModeTimer - 1);
    f32 phase = (f32)timer * 0.15707963705062866f;
    this->mModeTimer = timer;
    bool finished = timer <= 0;
    f64 cosine = tribox_cos(phase);
    s16 sign = (s16)this->mRotationSign;
    f32 progress = (f32)(cosine + 1.0);
    progress *= 0.5f;
    progress *= (f32)sign;
    f32 rotation = progress * 1.0471975803375244f;
    s32 step = (s32)this->mRotationStep;
    s16 homeAngle = (s16)this->home.angle.y;
    f32 baseAngle = gabi::fmadds((f32)step, 1.0471975803375244f,
                                                              (f32)homeAngle * 9.58738019107841e-05f);
    tribox_PSMTXRotRad(reinterpret_cast<u8*>(&mtx), 0x59, baseAngle);
    u32 pivot = (u32)this->mPivot;
    gabi::Local<cXyz> before, after, sum, position;
    tribox_PSMTXMultVecSR(reinterpret_cast<u8*>(&mtx),
                          gabi::at<cXyz>(gabi::ea(&M_vertex[0]) + pivot * 12), before.get());
    tribox_mDoMtx_stack_c_rYrotM(rotation);
    pivot = (u32)this->mPivot;
    tribox_PSMTXMultVecSR(reinterpret_cast<u8*>(&mtx),
                          gabi::at<cXyz>(gabi::ea(&M_vertex[0]) + pivot * 12), after.get());
    tribox_cXyz___pl(gabi::at<cXyz>(base + 0x520), sum.get(), before.get());
    tribox_cXyz___mi(sum.get(), position.get(), after.get());
    f32 fullAngle = baseAngle + rotation;
    f32 angleUnits = gabi::fmadds(fullAngle, 10430.3779296875f, 0.5f);
    u32 x = (u32)gabi::at<TriboxVectorWords>(gabi::ea(position.get()))->mWords[0],
            y = (u32)gabi::at<TriboxVectorWords>(gabi::ea(position.get()))->mWords[1],
            z = (u32)gabi::at<TriboxVectorWords>(gabi::ea(position.get()))->mWords[2];
    gabi::at<TriboxVectorWords>(gabi::ea(&current.pos))->mWords[0] = x;
    gabi::at<TriboxVectorWords>(gabi::ea(&current.pos))->mWords[1] = y;
    gabi::at<TriboxVectorWords>(gabi::ea(&current.pos))->mWords[2] = z;
    this->shape_angle.y = (s16)gabi::ftoi(angleUnits);
    if (finished) {
        tribox_eff_smoke_end(this);
        u32 play = tribox_dComIfGp_get(),
                player = gabi::ea((fopAc_ac_c*)gabi::at<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer),
                flags = (u32)gabi::at<daPy_py_c>(player)->mNoResetFlg0;
        gabi::at<daPy_py_c>(player)->mNoResetFlg0 = flags & ~daPy_py_c::daPyFlg0_PUSH_PULL_KEEP;
        u32 old = (u32)this->mRotationStep;
        sign = (s16)this->mRotationSign;
        s32 combined = (s32)(old + (u32)(s32)sign);
        this->mRotationStep = combined % 6;
        if (tribox_chk_wall_abi2(this, 1)) {
            play = tribox_dComIfGp_get();
            u32 material = tribox_dBgS_GetMtrlSndId(reinterpret_cast<u8*>(&gabi::at<dComIfG_inf_c>(play)->play.mBgS),
                                                                          reinterpret_cast<u8*>(static_cast<cBgS_PolyInfo*>(&M_linChk)));
            s8 room = (s8)this->current.roomNo;
            s32 reverb = tribox_dComIfGp_getReverb(room);
            tribox_JAIZelBasic_seStartPos_abi2(0x2823, gabi::at<cXyz>(base + 0x37C), material,
                                  reverb);
        }
        tribox_mode_block_wait_init(this);
        return;
    }
    tribox_eff_smoke_pos(this);
    gabi::Local<TriboxGroundCheck> check;
    tribox_cBgS_GndChk_cBgS_GndChk(check.get());
    f32 currentY = this->current.pos.y, currentZ = this->current.pos.z,
            currentX = this->current.pos.x;
    check->mbCamThrough = 0;
    check->mbLinkThrough = 0;
    check->mbArrowThrough = 0;
    check->mbBombThrough = 0;
    check->mbBoomerangThrough = 0;
    check->cBgS_Chk::__vtbl = tribox_tu.groundCheckVtable;
    check->mGrp = 1;
    check->m_pos.y = currentY + 50.0f;
    check->mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk*>(check.get()));
    check->m_pos.z = currentZ;
    check->mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk*>(check.get()));
    check->dBgS_GrpPassChk::__vtbl = tribox_tu.groundGroupPassVtable;
    check->mbRopeThrough = 0;
    check->dBgS_PolyPassChk::__vtbl = tribox_tu.groundPolyPassVtable;
    check->cBgS_PolyInfo::__vtbl = tribox_tu.groundPolyInfoVtable;
    check->mbObjThrough = 1;
    check->m_pos.x = currentX;
    u32 play = tribox_dComIfGp_get();
    tribox_cBgS_GroundCross(reinterpret_cast<u8*>(&gabi::at<dComIfG_inf_c>(play)->play.mBgS), check.get());
    u32 material = 0;
    if ((u16)check->mBgIndex < 0x100) {
        play = tribox_dComIfGp_get();
        material = tribox_dBgS_GetMtrlSndId(reinterpret_cast<u8*>(&gabi::at<dComIfG_inf_c>(play)->play.mBgS),
                                                              reinterpret_cast<u8*>(static_cast<cBgS_PolyInfo*>(check.get())));
    }
    s8 room = (s8)this->current.roomNo;
    s32 reverb = tribox_dComIfGp_getReverb(room);
    tribox_JAIZelBasic_seStartPos_abi2(0x2022, gabi::at<cXyz>(base + 0x37C), material,
                          reverb);
    check->cBgS_PolyInfo::__vtbl = tribox_tu.groundPolyInfoDtorVtable;
    check->dBgS_PolyPassChk::__vtbl = tribox_tu.groundPolyPassDtorVtable;
    check->dBgS_GrpPassChk::__vtbl = tribox_tu.groupPassDtorVtable;
    tribox_cBgS_Chk_cBgS_Chk(check.get(), 0);
}

/* 023AA564 */
void daObjTribox::Act_c::mode_block_sink_init() {
    u32 flags = (u32)this->actor_status;
    this->mModeTimer = 0;
    this->actor_status = flags & ~0x80u;
    mMode = MODE_BLOCK_SINK;
    this->mSinkVelocity = 0.0f;
    tribox_eff_sink_smoke_init(this);
    tribox_vib_sink_init(this);
}

/* 023AABB0 */
void daObjTribox::Act_c::mode_block_sink() {
    if (!(u8)M_sink_start) {
        this->mModeTimer = 0;
        this->mSinkVelocity = 0.0f;
        return;
    }
    tribox_eff_sink_smoke_start(this);
    tribox_vib_sink_start(this);
    f32 velocity = (f32)this->mSinkVelocity - 0.800000011920929f;
    f32 sinkHeight = (f32)M_sink_height;
    s16 timer = (s16)this->mModeTimer;
    f32 homeY = (f32)this->home.pos.y;
    this->mSinkVelocity = velocity;
    f32 target = homeY + sinkHeight;
    bool first = (u32)((s32)timer - 5) < 7;
    f32 damping = first ? 0.5f
                                            : ((u32)((s32)timer - 65) < 9 ? 0.4000000059604645f
                                                                                                        : 0.8199999928474426f);
    velocity *= damping;
    f32 next = this->current.pos.y + velocity;
    this->mSinkVelocity = velocity;
    // PPC bge tests the absence of LT, including unordered comparisons.
    bool moving = !(next < target);
    if (moving) {
        this->current.pos.y = next;
        tribox_sound_sink_down_block(this);
        timer = (s16)this->mModeTimer;
        this->mModeTimer = (s16)(timer + 1);
        return;
    }
    this->current.pos.y = target;
    tribox_eff_sink_smoke_end(this);
    tribox_vib_sink_end(this);
    tribox_sound_sink_stop_block(this);
    tribox_mode_block_lower_init(this);
}

/* 023AAB88 */
void daObjTribox::Act_c::mode_block_lower_init() {
    u32 flags = (u32)this->actor_status;
    mMode = MODE_BLOCK_LOWER;
    this->mSoundTimer = 94;
    this->actor_status = flags & ~0x80u;
    this->mModeTimer = 50;
}

/* 023AAD18 */
void daObjTribox::Act_c::mode_block_lower() {
    s16 timer = (s16)this->mModeTimer;
    if (timer < 0) {
        this->mSoundTimer = 94;
        return;
    }
    if (timer > 0) {
        timer = (s16)(timer - 1);
        this->mModeTimer = timer;
        if (timer) {
            this->mSoundTimer = 94;
            return;
        }
        tribox_eff_flash(this);
        timer = (s16)this->mModeTimer;
        this->mFlash = 1;
        if (timer) {
            this->mSoundTimer = 94;
            return;
        }
    }
    s16 soundTimer = (s16)this->mSoundTimer;
    if (soundTimer > 0) {
        soundTimer = (s16)(soundTimer - 1);
        this->mSoundTimer = soundTimer;
        if (!soundTimer)
            tribox_sound_flash_shine(this);
    }
}

/* 023AADBC */
void daObjTribox::Act_c::mode_correct_off_init() {
    u32 flags = (u32)this->actor_status;
    mMode = MODE_CORRECT_OFF;
    this->actor_status = flags & ~0x80u;
}

/* 023AADD4 */
void daObjTribox::Act_c::mode_correct_off() {
    if (tribox_fopAcIt_Judge(0x023AA354, this)) {
        u32 count = (u32)M_correct_cnt;
        M_correct_cnt = count + 1;
        tribox_mode_correct_off_init(this);
    }
}

/* 023A9814 */
void daObjTribox::Act_c::mode_correct_on_init() {
    u32 flags = (u32)this->actor_status;
    mMode = MODE_CORRECT_ON;
    this->actor_status = flags & ~0x80u;
}

/* 023AAE64 */
void daObjTribox::Act_c::mode_correct_on() {
    if (!tribox_fopAcIt_Judge(0x023AA354, this)) {
        u32 count = (u32)M_correct_cnt;
        M_correct_cnt = count - 1;
        tribox_mode_correct_on_init(this);
        return;
    }
    u8 leader = (u8)this->mLeader;
    if (leader && (s32)M_correct_cnt >= 3) {
        M_correct_flag = 1;
        leader = (u8)this->mLeader;
    } else if (!(u8)M_correct_flag)
        return;
    if (leader)
        tribox_mode_correct_demoreq_init(this);
    else
        tribox_mode_correct_end_init(this);
}

/* 023AAE2C */
void daObjTribox::Act_c::mode_correct_demoreq_init() {
    u32 flags = (u32)this->actor_status;
    mMode = MODE_CORRECT_DEMOREQ;
    this->mModeTimer = 10;
    this->actor_status = flags & ~0x80u;
}

/* 023AAF5C */
void daObjTribox::Act_c::mode_correct_demoreq() {
    s16 event = (s16)this->mEvent;
    u32 play = tribox_dComIfGp_get();
    if (!tribox_dEvent_manager_c_getEventData(reinterpret_cast<u8*>(&gabi::at<dComIfG_inf_c>(play)->play.mEvtManager), event)) {
        tribox_mode_correct_demorun_init(this);
        return;
    }
    if ((u16)this->eventInfo.mCommand == dEvtCmd_INDEMO_e) {
        this->mDemoActive = 1;
        tribox_mode_correct_demorun_init(this);
        return;
    }
    event = (s16)this->mEvent;
    tribox_fopAcM_orderOtherEventId(this, event, 255, 0xFFFF, 0, 1);
    u16 flags = (u16)this->eventInfo.mCondition;
    this->eventInfo.mCondition = flags | 2;
}

/* 023AAF38 */
u32 daObjTribox::Act_c::mode_correct_demorun_init() {
    u32 flags = (u32)this->actor_status;
    mMode = MODE_CORRECT_DEMORUN;
    this->mModeTimer = 50;
    this->actor_status = flags & ~0x80u;
    return tribox_JAIZelBasic_seStart(0x806);
}

/* 023AB018 */
void daObjTribox::Act_c::mode_correct_demorun() {
    s16 timer = (s16)this->mModeTimer;
    if (timer > 0) {
        timer = (s16)(timer - 1);
        this->mModeTimer = timer;
        if (!timer) {
            u32 sw = tribox_parameter(this, 8, 8),
                    save = gabi::ea((u8*)l_save.value);
            s8 room = (s8)this->home.roomNo;
            tribox_dSv_info_c_onSwitch(gabi::at<u8>(save + 0x20), sw, room);
            M_sink_start = 1;
        }
        return;
    }
    if ((u8)this->mDemoActive) {
        s16 event = (s16)this->mEvent;
        u32 play = tribox_dComIfGp_get();
        if (!tribox_dEvent_manager_c_endCheck(reinterpret_cast<u8*>(&gabi::at<dComIfG_inf_c>(play)->play.mEvtManager), event))
            return;
        play = tribox_dComIfGp_get();
        u16 flags = (u16)gabi::at<dComIfG_inf_c>(play)->play.mEvtCtrl.mEventFlag;
        gabi::at<dComIfG_inf_c>(play)->play.mEvtCtrl.mEventFlag = flags | 8;
        this->mDemoActive = 0;
    }
    tribox_mode_correct_end_init(this);
}

/* 023AAE4C */
void daObjTribox::Act_c::mode_correct_end_init() {
    u32 flags = (u32)this->actor_status;
    mMode = MODE_CORRECT_END;
    this->actor_status = flags | 0x80;
}

/* 023AB3A4 */
void daObjTribox::Act_c::mode_correct_end() {  }

/* 023A98B8 */
void daObjTribox::Act_c::mode_correct_dummy_init() {
    u32 flags = (u32)this->actor_status;
    mMode = MODE_CORRECT_DUMMY;
    this->actor_status = flags & ~0x80u;
}

/* 023AB3A8 */
void daObjTribox::Act_c::mode_correct_dummy() {  }

/* 023A8CF0 */
void daObjTribox::Act_c::mode_proc_call() {
    dispatch_member(this, gabi::ea(&M_mode_table) + (u32)this->mMode * sizeof(gabi::ptmf<s32 (Act_c::*)()>));
}

/* 023A8E74 */
void daObjTribox::Act_c::execute_block() {
    tribox_mode_proc_call(this);
    u32 direction = (u32)this->mPushDirection,
            pull = (u32)this->mPull;
    this->mPreviousDirection = direction;
    this->mPreviousPull = pull;
    f32 y = this->current.pos.y;
    u32 input = (u32)this->mPushInputWord;
    this->mPushInput = 0;
    this->mPreviousPushInputWord = input;
    f32 x = this->current.pos.x;
    this->eyePos.y = y;
    this->eyePos.x = x;
    u32 sign = (u32)this->mPushSide;
    f32 z = this->current.pos.z;
    this->mPreviousSide = sign;
    this->eyePos.z = z;
    tribox_set_mtx(this);
    tribox_dBgW_Move((u8 *)this->mBackground);
}

/* 023A8EF0 */
void daObjTribox::Act_c::execute_correct() {
    tribox_mode_proc_call(this);
}

/* 023A9090 */
BOOL daObjTribox::Act_c::_execute() {
    u32 type = tribox_parameter(this, 1, 16);
    if (type)
        tribox_execute_correct(this);
    else
        tribox_execute_block(this);
    if (!this->mLightModel)
        return TRUE;
    u8 flash = (u8)this->mFlash;
    if (flash == 1) {
        if (tribox_mDoExt_baseAnm_play(reinterpret_cast<u8*>(&this->mBlockBrk))) {
            this->mFlash = 2;
            tribox_sound_flash_light(this);
            return TRUE;
        }
    } else if (flash == 2)
        tribox_mDoExt_baseAnm_play_abi2(reinterpret_cast<u8*>(&this->mLightBrk));
    flash = (u8)this->mFlash;
    if (flash == 1 || flash == 2)
        tribox_sound_flash_light(this);
    return TRUE;
}

/* 023A9154 */
BOOL daObjTribox::Act_c::_draw() {
    u32 state = this->mState;
    if (state != STATE_BLOCK_BEFORE && state != STATE_CORRECT_AFTER)
        return TRUE;
    u32 env = tribox_dKy_getEnvlight(), base = gabi::ea(this);
    tribox_dScnKy_env_light_c_settingTevStruct(gabi::at<u8>(env), 0, &this->current.pos,
                          gabi::at<u8>(base + 0x110));
    env = tribox_dKy_getEnvlight();
    tribox_dKy_setLightTevColorType_MAJI(gabi::at<u8>(env), (J3DModel *)this->mBlockModel,
                          gabi::at<u8>(base + 0x110));
    if ((u8)this->mFlash) {
        env = tribox_dKy_getEnvlight();
        tribox_dKy_setLightTevColorType_MAJI(gabi::at<u8>(env), (J3DModel *)this->mLightModel,
                              gabi::at<u8>(base + 0x110));
        u8 flash = (u8)this->mFlash;
        u32 data = gabi::ea((J3DModelData*)this->mLightModel->mModelData);
        u32 animation = base + (flash == 1 ? 0x3BC : 0x434);
        f32 frame = (f32)gabi::at<mDoExt_baseAnm>(animation)->mFrameCtrl.mFrame;
        tribox_mDoExt_brkAnm_entry(gabi::at<u8>(animation), gabi::at<u8>(data), frame);
        if ((u8)this->mFlash) {
            dComIfGd_setListP1();
            tribox_mDoExt_modelUpdateDL((J3DModel *)this->mLightModel, 0);
            dComIfGd_setList();
        }
    }
    dComIfGd_setListBG();
    tribox_mDoExt_modelUpdateDL((J3DModel *)this->mBlockModel, 0);
    dComIfGd_setList();
    return TRUE;
}

/* 023AB0FC */
s32 Mthd_Create(Act_c *a) {
    return tribox_create(a);
}

/* 023AB100 */
BOOL Mthd_Delete(Act_c *a) {
    return tribox_remove(a);
}

/* 023AB104 */
BOOL Mthd_Execute(Act_c *a) {
    return tribox_execute(a);
}

/* 023AB108 */
BOOL Mthd_Draw(Act_c *a) {
    return tribox_draw(a);
}

// HD: compiler-generated and template code of this translation unit.
// sound_pos_delete retains the HD entry point without an actor argument.
/* 023AB3B4 */
u32 parameter(Act_c *actor, u32 width, u32 shift) {
    u32 bits = (u32)actor->mParameters;
    u32 mask = (width & 32) ? 0u : (1u << (width & 31));
    return ((shift & 32) ? 0u : (bits >> (shift & 31))) & (mask - 1);
}

/* 023A8B3C */
void sound_pos_delete() {
    tribox_JAIZelBasic_seDeleteObject(&M_sound_pos);
}

/* 023AB3AC */
BOOL Mthd_IsDelete(Act_c *actor) {
    return TRUE;
}

/* 023AB344 */
void SafeString_assureTerminationImpl() {  }

/* 023AB330 */
void SafeString_dtor(void *p, s32 flags) {
    if (p && (flags & 1))
        tribox_operator_delete(p);
}

/* 023AB348 */
void Act_c_dtor(Act_c *p, s32 flags) {
    if (p) {
        tribox_fopAc_ac_c_fopAc_ac_c(p, 0);
        if (flags & 1)
            tribox_operator_delete_abi2(p);
    }
}

/* 023A952C */
BOOL solidHeapCB(Act_c *a) {
    return tribox_create_heap(a);
}

/* 023AB3D0 */
void reset() {
    M_c_cont_cnt = 0;
    M_sink_start = 0;
    f32 y = (f32)M_default_sound_pos.y, x = (f32)M_default_sound_pos.x;
    M_correct_cnt = 0;
    M_sound_pos.y = y;
    f32 z = (f32)M_default_sound_pos.z;
    M_b_cont_cnt = 0;
    M_sound_pos.z = z;
    M_correct_flag = 0;
    M_sound_pos.x = x;
}

/* 023AA354 */
Act_c *search_block(Act_c *other, Act_c *self) {
    if (!tribox_fopAc_IsActor(other) || !other)
        return nullptr;
    if ((s16)other->mProcName != 0x2C)
        return nullptr;
    if (tribox_parameter(other, 1, 16) != 0)
        return nullptr;
    f32 distance = tribox_fopAcM_searchActorDistance2(self, other);
    return distance < 225.0f ? other : nullptr;
}

/* 023AB10C */
void __sinit_d_a_obj_tribox_cpp() {
    M_random_state[2] = 0;
    M_random_state[0] = 0;
    M_random_state[3] = 0;
    M_random_state[1] = 0;
    tribox_registerGlobalObject(reinterpret_cast<u8*>(&M_angle_dtor));
    M_angle_min = -3.1415927410125732f;
    M_angle_max = 3.1415927410125732f;
    tribox_JMath_random_construct(reinterpret_cast<u8*>(&M_random));
    tribox_registerGlobalObject(reinterpret_cast<u8*>(&M_random_dtor));
    tribox_JMath_randomFast_construct(reinterpret_cast<u8*>(&M_random_fast));
    tribox_registerGlobalObject(reinterpret_cast<u8*>(&M_random_fast_dtor));
    M_half_width = 125.0f;
    f32 height = tribox_sqrtf(3.0f) * 41.66666793823242f;
    M_sink_height = -245.0f;
    f32 twice = height + height, width = (f32)M_half_width;
    M_triangle_height = height;
    M_vertex[0].x = 0.0f;
    M_vertex[0].y = 0.0f;
    M_vertex[0].z = -twice;
    M_vertex[1].x = -width;
    M_vertex[1].y = 0.0f;
    M_vertex[1].z = height;
    M_vertex[2].x = width;
    M_vertex[2].y = 0.0f;
    M_double_height = twice;
    M_vertex[2].z = height;
    tribox_cBgS_LinChk_ct(reinterpret_cast<u8*>(&M_linChk));
    M_linChk.mbCamThrough = 0;
    M_linChk.mbLinkThrough = 0;
    M_linChk.mbArrowThrough = 0;
    M_linChk.mbBombThrough = 0;
    M_linChk.mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk*>(&M_linChk));
    M_linChk.mGrp = 1;
    M_linChk.dBgS_GrpPassChk::__vtbl = tribox_tu.lineGroupPassVtable;
    M_linChk.mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk*>(&M_linChk));
    M_linChk.cBgS_Chk::__vtbl = tribox_tu.lineCheckVtable;
    M_linChk.mbBoomerangThrough = 0;
    M_linChk.mbRopeThrough = 0;
    M_linChk.cBgS_PolyInfo::__vtbl = tribox_tu.linePolyInfoVtable;
    M_linChk.mbObjThrough = 1;
    M_linChk.dBgS_PolyPassChk::__vtbl = tribox_tu.linePolyPassVtable;
    tribox_registerGlobalObject(reinterpret_cast<u8*>(&M_line_dtor));
}

/* 023AB2B8 */
void dBgS_ObjLinChk_dtor(void *p, s32 flags) {
    if (p) {
        u32 base = gabi::ea(p);
        gabi::at<dBgS_ObjLinChk>(base)->dBgS_PolyPassChk::__vtbl = tribox_tu.linePolyPassDtorVtable;
        gabi::at<dBgS_ObjLinChk>(base)->dBgS_GrpPassChk::__vtbl = tribox_tu.groupPassDtorVtable;
        gabi::at<dBgS_ObjLinChk>(base)->cBgS_PolyInfo::__vtbl = tribox_tu.linePolyInfoDtorVtable;
        tribox_cBgS_LinChk_destroy(p, 0);
        if (flags & 1)
            tribox_operator_delete(p);
    }
}

/* 023A956C */
Act_c *push_pullCB(Act_c *a, Act_c *other, s16 pushAngle, u32 label) {
    u32 mode = label & 3;
    if (!mode)
        return a;
    if (mode == 3)
        tribox_JUTAssertion_showAssert("d_a_obj_tribox.cpp", 0x2B0, "pp_label != pp_field");
    s16 angle = a->shape_angle.y;
    a->mPull = (mode & 1) ^ 1;
    s16 relative = (s16)((s32)pushAngle - 0x8000 - (s32)angle);
    s32 direction;
    if ((u32)((s32)relative + 0x2AAA) < 0x5554)
        direction = 0;
    else
        direction = relative < 0x2AAA ? 2 : 1;
    a->mPushDirection = direction;
    s16 actorAngle = tribox_fopAcM_searchActorAngleY(a, other);
    direction = (s32)a->mPushDirection;
    angle = a->shape_angle.y;
    s16 offset = (s16)*gabi::at<be<s16>>(gabi::ea(&M_direction_angle[0]) + (u32)direction * 2);
    s16 delta = (s16)((s32)angle + (s32)offset - (s32)actorAngle);
    a->mPushSide = (u32)(s32)delta >> 31;
    a->mPushInput = 1;
    return a;
}

/* 023AB428 */
static void __sinit_d_a_obj_tribox_static_cpp() {
        sinit_header_statics(0x1046C75C, 0x101CD6F8);
}

/* WWHD glue
data M_angle_dtor 101CD678
data M_random_dtor 101CD684
data M_random_fast_dtor 101CD690
data M_line_dtor 101CD69C
data M_random 1046C6A8
data M_random_fast 1046C6A9
str 10031A18
str 10031A2C
data M_mode_table 10031A54
data M_create_table 100319F8
data M_vertex 1046C6CC nocheck # BSS: constructor writes the vertices
data M_linChk 1046C6F0
data mtx 1048D0CC
data l_save 101F84DC
data l_resControl 101F4F28
data M_direction_angle 10031B20
data M_random_state 1046C6AC nocheck # BSS: explicitly cleared by the initializer
ext 025D4604 u32 tribox_fopAc_IsActor(Act_c*)
ext 025D6924 f32 tribox_fopAcM_searchActorDistance2(Act_c*, Act_c*)
ext 025D6894 s16 tribox_fopAcM_searchActorAngleY(Act_c*, Act_c*)
ext 023A8EF0 void tribox_execute_correct(Act_c*)
ext 023A8E74 void tribox_execute_block(Act_c*)
ext 023AAE2C void tribox_mode_correct_demoreq_init(Act_c*)
data M_correct_cnt 1046C778
data M_b_cont_cnt 1046C77C
data M_c_cont_cnt 1046C780
data M_correct_flag 1046C790
data M_sink_start 1046C791
data M_sink_height 1046C6A4
data M_half_width 1046C698
data M_triangle_height 1046C69C
data M_double_height 1046C6A0
data M_angle_min 1046C690
data M_angle_max 1046C694
data M_smoke_scale_initialized 1046C6C8
data M_sound_pos 1046C784
data M_smoke_scale 1046C6BC
data M_default_sound_pos 101FFBA8
str 10031A40 in 023A8B48
str 10031AAC
str 10031ABC
str 10031ACC
str 10031AE4
str 10031AF8
str 10031B0C in 023A92BC
str 10031B28 in 023A956C
str 10031B3C
str 10031B60
str 10031B7C in 023A9B18
str 10031B90
str 10031C00
ext 023AB3B4 u32 tribox_parameter(Act_c *, int, int)
ext 025BA0C0 u32 tribox_dSv_info_c_isSwitch(u8 *, u32, s8)
ext 025E1B34 void tribox_JAIZelBasic_seDeleteObject(cXyz *)
ext 0273AA24 void tribox_JUTAssertion_showAssert(const char *, int, const char *)
ext 0273AF40 void tribox_operator_delete(void *)
ext 025D50BC void tribox_fopAc_ac_c_fopAc_ac_c(Act_c *, int)
ext 0273AF40 void tribox_operator_delete_abi2(Act_c *)
ext 025A5F88 void tribox_dPa_smokeEcallBack_end(u8 *)
ext 025E1988 u32 tribox_JAIZelBasic_seStart(int)
ext 023A8CF0 void tribox_mode_proc_call(Act_c *)
ext 023A92BC int tribox_create_heap(Act_c *)
ext 023AA0B4 void tribox_eff_sink_smoke_init(Act_c *)
ext 023AA1E8 void tribox_vib_sink_init(Act_c *)
ext 023A8A18 int tribox_create(Act_c *)
ext 023A8C48 int tribox_remove(Act_c *)
ext 023A9090 int tribox_execute(Act_c *)
ext 023A9154 int tribox_draw(Act_c *)
ext 0201AE48 void tribox_cXyz___ml(cXyz *, cXyz *, float)
ext 028E8D88 void tribox_PSVECAdd(cXyz *, cXyz *, cXyz *)
ext 028E93CC void tribox_PSMTXTrans(u8 *, f32, f32, f32)
ext 025F1B48 void tribox_mDoMtx_ZXYrotM(u8 *, s16, s16, s16)
ext 023A8D3C void tribox_set_mtx(Act_c *)
ext 026066C4 void * tribox_dRes_control_c_getRes(u8 *, SafeString *, int)
ext 025E38E0 J3DModel * tribox_mDoExt_J3DModel__create(void *, int, int)
ext 024F2478 u8 * tribox_dBgW_NewSet(void *, int, u8 *)
ext 025E8154 int tribox_mDoExt_brkAnm_initialize(u8 *, void *, void *, int, int, float, int, int, int, int)
ext 023A8EF4 u32 tribox_sound_get_mapinfo(Act_c *, cXyz *)
ext 02520540 int tribox_dComIfGp_getReverb(s8)
ext 025E1A40 void tribox_JAIZelBasic_seStartPos(u32, cXyz *, u32, s32)
ext 025E742C int tribox_mDoExt_baseAnm_play(u8 *)
ext 023A9018 void tribox_sound_flash_light(Act_c *)
ext 025E742C void tribox_mDoExt_baseAnm_play_abi2(u8 *)
ext 02555D0C u32 tribox_dKy_getEnvlight()
ext 025626A4 void tribox_dScnKy_env_light_c_settingTevStruct(u8 *, int, cXyz *, u8 *)
ext 02562F5C void tribox_dKy_setLightTevColorType_MAJI(u8 *, J3DModel *, u8 *)
ext 025E83FC void tribox_mDoExt_brkAnm_entry(u8 *, u8 *, f32)
ext 025200D4 u32 tribox_dComIfGp_get()
ext 025E2DE0 void tribox_mDoExt_modelUpdateDL(J3DModel *, int)
ext 025B8B94 u32 tribox_dSv_event_c_isEventBit(u8 *, int)
ext 02520460 int tribox_dComIfG_resLoad(request_of_phase_process_class *, const char *)
ext 025D63E8 int tribox_fopAcM_entrySolidHeap(Act_c *, int, int)
ext 023A96C0 void tribox_block_init(Act_c *)
ext 023A982C void tribox_correct_before_init(Act_c *)
ext 023A98D0 void tribox_correct_after_init(Act_c *)
ext 023A8B3C void tribox_sound_pos_delete(Act_c *)
ext 023A8B48 void tribox_controll_clear(Act_c *)
ext 023A8BD4 void tribox_eff_smoke_remove(Act_c *)
ext 023A8BE4 void tribox_eff_sink_smoke_remove(Act_c *)
ext 020087EC void tribox_cBgS_Release(u8 *, u8 *)
ext 025204C8 void tribox_dComIfG_resDelete(request_of_phase_process_class *, const char *)
ext 025D4ED0 void tribox_fopAc_ac_c_fopAc_ac_c(Act_c *)
ext 025E80D0 void tribox_mDoExt_brkAnm_init(u8 *)
ext 025A5B18 void tribox_dPa_smokeEcallBack_dPa_smokeEcallBack(u8 *, int)
ext 025A5C04 void tribox_dPa_footEcallBack_construct(u8 *, int, int, int, int)
ext 023A87EC void tribox_set_state(Act_c *)
ext 023A890C void tribox_controll_set(Act_c *)
ext 025E0D38 void tribox_fpcPi_setPriority(u8 *, int, int, int)
ext 023A887C void tribox_sound_pos_init(Act_c *)
ext 025D6768 void tribox_fopAcM_setCullSizeSphere(Act_c *, float, float, float, float)
ext 02543F10 int tribox_dEvent_manager_c_getEventIdx(u8 *, const char *, int)
ext 023A9814 void tribox_mode_correct_on_init(Act_c *)
ext 024F1AFC void tribox_dBgS_LinChk_Set(u8 *, const cXyz *, const cXyz *, Act_c *)
ext 02008860 int tribox_cBgS_LineCross(u8 *, u8 *)
ext 024F43DC void tribox_dBgW_Move(u8 *)
ext 023A9B18 u32 tribox_chk_wall(Act_c *, u32)
ext 025A847C void tribox_dPa_control_c_set(u8 *, int, int, cXyz *, csXyz *, int, int, int, int, int, int, int)
ext 025F1884 void tribox_mDoMtx_YrotS(u8 *, s16)
ext 028E9044 void tribox_PSMTXMultVecSR(u8 *, cXyz *, cXyz *)
ext 0201ADE0 void tribox_cXyz___mi(cXyz *, cXyz *, cXyz *)
ext 0201B3C0 void tribox_cXyz_normalizeZP(cXyz *, cXyz *)
ext 028E8E64 void tribox_PSVECScale(cXyz *, cXyz *, float)
ext 0201AD78 void tribox_cXyz___pl(cXyz *, cXyz *, cXyz *)
ext 0200F93C short tribox_cLib_targetAngleY(cXyz *, cXyz *)
ext 023A9530 void tribox_init_mtx(Act_c *)
ext 024EEA6C void tribox_dBgS_Regist(u8 *, u8 *, Act_c *)
ext 023A97D4 int tribox_chk_light(Act_c *)
ext 023A98B8 void tribox_mode_correct_dummy_init(Act_c *)
ext 023A96A4 void tribox_mode_block_wait_init(Act_c *)
ext 023A9E64 void tribox_eff_smoke_pos(Act_c *)
ext 025A847C u32 tribox_dPa_control_c_set_abi2(u8 *, int, int, cXyz *, csXyz *, cXyz *, int, u8 *, s8, int, int, int)
ext 028F026C void tribox_registerGlobalObject(u8 *)
ext 028ED6F8 void tribox_JMath_random_construct(u8 *)
ext 028EAB2C void tribox_JMath_randomFast_construct(u8 *)
ext 028F4384 float tribox_sqrtf(float)
ext 02008FEC void tribox_cBgS_LinChk_ct(u8 *)
ext 02008B4C void tribox_cBgS_LinChk_destroy(void *, int)
ext 025A847C void tribox_dPa_control_c_set_abi3(u8 *, int, int, cXyz *, csXyz *, int, int, u8 *, int, int, int, int)
ext 025CB374 void tribox_dVibration_c_StartShock(u8 *, int, int, cXyz *)
ext 025CB408 void tribox_dVibration_c_StartQuake(u8 *, int, int, cXyz *)
ext 025CB610 void tribox_dVibration_c_StopQuake(u8 *, int)
ext 023A9FC0 void tribox_eff_smoke_start(Act_c *)
ext 02544044 u32 tribox_dEvent_manager_c_getEventData(u8 *, s16)
ext 023AAF38 void tribox_mode_correct_demorun_init(Act_c *)
ext 025D7A58 void tribox_fopAcM_orderOtherEventId(Act_c *, s16, int, int, int, int)
ext 025B9E38 void tribox_dSv_info_c_onSwitch(u8 *, u32, s8)
ext 025440C8 int tribox_dEvent_manager_c_endCheck(u8 *, s16)
ext 023AAE4C void tribox_mode_correct_end_init(Act_c *)
ext 02008E0C void tribox_cBgS_GndChk_cBgS_GndChk(TriboxGroundCheck *)
ext 02008974 void tribox_cBgS_GroundCross(u8 *, TriboxGroundCheck *)
ext 024EECAC u32 tribox_dBgS_GetMtrlSndId(u8 *, u8 *)
ext 02008DAC void tribox_cBgS_Chk_cBgS_Chk(TriboxGroundCheck *, int)
ext 025D5218 u32 tribox_fopAcIt_Judge(int, Act_c *)
ext 023AADBC void tribox_mode_correct_off_init(Act_c *)
ext 023AA564 void tribox_mode_block_sink_init(Act_c *)
ext 023A9DA0 int tribox_chk_space(Act_c *)
ext 023AA5B0 void tribox_mode_block_walk_init(Act_c *)
ext 023A9DD8 void tribox_eff_flash(Act_c *)
ext 023AA4EC void tribox_sound_flash_shine(Act_c *)
ext 023AA0C0 void tribox_eff_sink_smoke_start(Act_c *)
ext 023AA1F4 void tribox_vib_sink_start(Act_c *)
ext 023AA3FC void tribox_sound_sink_down_block(Act_c *)
ext 023AA194 void tribox_eff_sink_smoke_end(Act_c *)
ext 023AA2D8 void tribox_vib_sink_end(Act_c *)
ext 023AA474 void tribox_sound_sink_stop_block(Act_c *)
ext 023AAB88 void tribox_mode_block_lower_init(Act_c *)
ext 023A9ABC int tribox_line_cross(Act_c *, cXyz *, cXyz *)
ext 025F1C28 void tribox_mDoMtx_YrotM(u8 *, int)
ext 028F4BE0 double tribox_cos(f32)
ext 028E98C0 void tribox_PSMTXRotRad(u8 *, int, f32)
ext 025F2590 void tribox_mDoMtx_stack_c_rYrotM(f32)
ext 023AA0AC void tribox_eff_smoke_end(Act_c *)
ext 023A9B18 int tribox_chk_wall_abi2(Act_c *, int)
ext 025E1A40 void tribox_JAIZelBasic_seStartPos_abi2(int, cXyz *, u32, s32)
*/
