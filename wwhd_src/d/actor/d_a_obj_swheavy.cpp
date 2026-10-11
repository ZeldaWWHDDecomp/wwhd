/** Wind Temple Iron Boots switch. Verified against the HD image; private decompilation. */
#include "d/actor/d_a_obj_swheavy.h"
using daObjSwheavy::Act_c;
const gc_tu_data gc_tu = {.safeString = 0x1002FB34, .headerStatics = {0x1046C008, 0x101CCF28, 0x1046C014}};
static SwheavyAttrTable M_attr;
static SwheavyModeTable mode_proc;

static u32 PrmAbstract(Act_c*, s32, s32);
static void rideCB(void*, Act_c*, fopAc_ac_c*);

static void JUT_assert(const char* file, u32 line, const char* message) {
    Swheavy_assert(file, line, message);
}
static const char* arc() {
    return "Hhbot";
}
static const daObjSwheavy::Attr_c* attr(u32 type) {
    return &M_attr.mAttr[type];
}
// HD: assertions do not abort; reload mType after an out-of-range assertion.
static u32 checkedType(Act_c* a, u32 type, const char* file, const char* msg) {
    if (type >= 4) {
        JUT_assert(file, 0xFD, msg);
        type = a->mType;
    }
    return type;
}
static BOOL isSwitch(Act_c* a) {
    u32 sw = PrmAbstract(a, Act_c::PRM_SWSAVE_W, Act_c::PRM_SWSAVE_S);
    s8 room = a->home.roomNo;
    return dComIfGs_isSwitch(sw, room);
}

static void copyModelMtx(J3DModel* model) {
    SwheavyModel* view = reinterpret_cast<SwheavyModel*>(model);
    f32 v[3][4];
    Mtx34* matrix = mDoMtx_stack_c::get();
    for (u32 r = 0; r < 3; r++)
        for (u32 c = 0; c < 4; c++)
            v[r][c] = matrix->m[r][c];
    for (u32 r = 0; r < 3; r++)
        for (u32 c = 0; c < 4; c++)
            view->mBaseTRMtx.m[r][c] = v[r][c];
}
static void translateRotate(Act_c* a) {
    Mtx34* m = mDoMtx_stack_c::get();
    f32 x = a->current.pos.x, z = a->current.pos.z, y = a->current.pos.y;
    PSMTXTrans(m, x, y, z);
    s16 xrot = a->shape_angle.x, yrot = a->shape_angle.y, zrot = a->shape_angle.z;
    Swheavy_ZXYrotM(m, xrot, yrot, zrot);
}

/* 02393EC4 */
static u8 solidHeapCB(Act_c* a) {
    return a->create_heap();
}

/* 02393CD0 */
u8 daObjSwheavy::Act_c::create_heap() {
    auto data = dComIfG_getObjectRes(arc(), dRes_INDEX_HHBOT_BDL_HHBOT1_e);
    if (!data)
        JUT_assert("d_a_obj_swheavy.cpp", 0x118, "mdl_1_data != 0");
    mpModel1 = mDoExt_J3DModel__create((J3DModelData*)data, 0x80000, 0x11000022);
    data = dComIfG_getObjectRes(arc(), dRes_INDEX_HHBOT_BDL_HHBOT2_e);
    if (!data)
        JUT_assert("d_a_obj_swheavy.cpp", 0x121, "mdl_2_data != 0");
    mpModel2 = mDoExt_J3DModel__create((J3DModelData*)data, 0x80000, 0x11000022);
    BOOL b1 = FALSE, b2 = FALSE;
    SwheavyBgW* bg = Swheavy_newBgW(0);
    mpBgW1 = bg;
    if (bg) {
        data = dComIfG_getObjectRes(arc(), dRes_INDEX_HHBOT_DZB_HHBOT1_e);
        if (!data)
            JUT_assert("d_a_obj_swheavy.cpp", 0x139, "bgw_1_data != 0");
        if (!Swheavy_setBg(mpBgW1, static_cast<cBgD_t*>(data), dBgW::MOVE_BG_e, &mMtx1))
            b1 = TRUE;
    }
    bg = Swheavy_newBgW(0);
    mpBgW2 = bg;
    if (bg) {
        data = dComIfG_getObjectRes(arc(), dRes_INDEX_HHBOT_DZB_HHBOT2_e);
        if (!data)
            JUT_assert("d_a_obj_swheavy.cpp", 0x148, "bgw_2_data != 0");
        if (!Swheavy_setBg(mpBgW2, static_cast<cBgD_t*>(data), dBgW::MOVE_BG_e, &mMtx2))
            b2 = TRUE;
    }
    u8 success = mpModel1 != nullptr && mpModel2 != nullptr && b1 && b2;
    if (!success) {
        mpBgW2 = nullptr;
        mpBgW1 = nullptr;
    }
    return success;
}

/* 02394114 */
s32 daObjSwheavy::Act_c::_create() {
    u32 condition = actor_condition;
    if (!(condition & fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            Swheavy_actorCtor(this);
            __vtbl = Act_c::VTABLE;
            condition = actor_condition;
        }
        actor_condition = condition | fopAcCnd_INIT_e;
    }
    s32 phase = dComIfG_resLoad(&mPhs, arc());
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    u32 type = PrmAbstract(this, PRM_TYPE_W, PRM_TYPE_S);
    mType = type;
    type = checkedType(this, type, "d_a_obj_swheavy.cpp", "(M_type >=0) && (M_type <Type_Max)");
    BOOL up = TRUE;
    if (!(static_cast<u32>(attr(type)->mFlags) & Act_c::FLAG_IS_TOGGLE) && isSwitch(this))
        up = FALSE;
    f32 z = scale.z, x = scale.x;
    scale.z = z * 1.5f;
    scale.x = x * 1.5f;
    translateRotate(this);
    f32 sy = scale.y, sx = scale.x, sz = scale.z;
    mDoMtx_stack_c::scaleM(sx, sy, sz);
    auto m = mDoMtx_stack_c::get();
    PSMTXCopy(m, &mMtx1);
    if (!up) {
        f32 y = static_cast<f32>(mDoMtx_stack_c::get()->m[1][3]);
        mDoMtx_stack_c::get()->m[1][3] = y + -35.5f;
    }
    PSMTXCopy(m, &mMtx2);
    if (!fopAcM_entrySolidHeap(this, heapCallbackFunc(gabi::guest_fn(reinterpret_cast<const void*>(solidHeapCB))),
                               0x2000))
        return cPhs_ERROR_e;
    dComIfG_Bgsp()->Regist(mpBgW1, this);
    mpBgW1->mCrrFunc = 0;
    dComIfG_Bgsp()->Regist(mpBgW2, this);
    mpBgW2->mCrrFunc = 0;
    mpBgW2->mRideCallback = gabi::guest_fn(reinterpret_cast<const void*>(rideCB));
    auto model = (J3DModel*)mpModel1;
    cullMtx = model ? gabi::ea(&reinterpret_cast<SwheavyModel*>(model)->mBaseTRMtx) : 0;
    init_mtx();
    fopAcM_setCullSizeBox(this, -97.5f, -2.0f, -97.5f, 97.5f, 55.0f, 97.5f);
    mPushFlg = 0;
    type = mType;
    mPrevHeavyRiding = 0;
    mPushTimer = 0;
    mVSpeed = 0.0f;
    mPrevRiding = 0;
    mMiniPushTimer = 0;
    m330 = 0;
    mHeavyRiding = 0;
    m334 = 0.0f;
    mMiniPushFlg = 0;
    mRiding = 0;
    type = checkedType(this, type, "d_a_obj_swheavy.cpp", "(M_type >=0) && (M_type <Type_Max)");
    if ((static_cast<u32>(attr(type)->mFlags) & Act_c::FLAG_IS_TOGGLE) || !isSwitch(this)) {
        mTopPos = 0.0f;
        m328 = 0.0f;
        mCurHFrac = 1.0f;
        m32C = 1.0f;
        m338 = 1.0f;
        mTargetHFrac = 1.0f;
        mode_upper_init();
    } else {
        mTopPos = -35.5f;
        m328 = -35.5f;
        m32C = 0.0f;
        mTargetHFrac = 0.0f;
        m338 = 0.0f;
        mCurHFrac = 0.0f;
        mode_lower_init();
    }
    return phase;
}

/* 02394484 */
BOOL daObjSwheavy::Act_c::_delete() {
    SwheavyBgW* bg = mpBgW1;
    if (bg && bg->ChkUsed()) {
        dComIfG_Bgsp()->Release(mpBgW1);
    }
    bg = mpBgW2;
    if (bg && bg->ChkUsed()) {
        dComIfG_Bgsp()->Release(mpBgW2);
    }
    dComIfG_resDelete(&mPhs, arc());
    return TRUE;
}

/* 02393F08 */
void daObjSwheavy::Act_c::set_mtx() {
    translateRotate(this);
    copyModelMtx(mpModel1);
    f32 height = m328, old = static_cast<f32>(mDoMtx_stack_c::get()->m[1][3]);
    mDoMtx_stack_c::get()->m[1][3] = old + height;
    copyModelMtx(mpModel2);
    translateRotate(this);
    f32 y = scale.y, x = scale.x, z = scale.z;
    mDoMtx_stack_c::scaleM(x, y, z);
    auto m = mDoMtx_stack_c::get();
    PSMTXCopy(m, &mMtx1);
    old = static_cast<f32>(mDoMtx_stack_c::get()->m[1][3]);
    height = mTopPos;
    mDoMtx_stack_c::get()->m[1][3] = old + height;
    PSMTXCopy(m, &mMtx2);
}

/* 0239409C */
void daObjSwheavy::Act_c::init_mtx() { // Matcher incorrectly names this PrmAbstract.
    SwheavyModel* model = reinterpret_cast<SwheavyModel*>(static_cast<J3DModel*>(mpModel1));
    f32 y = scale.y, x = scale.x, z = scale.z;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    model = reinterpret_cast<SwheavyModel*>(static_cast<J3DModel*>(mpModel2));
    x = scale.x;
    z = scale.z;
    y = scale.y;
    model->mBaseScale.z = z;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    set_mtx();
}

/* 02393EC8 */
static void rideCB(void* bg, Act_c* a, fopAc_ac_c* rider) {
    if (rider->actor_status & fopAcStts_FREEZE_e) {
        a->mRiding = 1;
        if (rider && rider->mProfName == SWHEAVY_PLAYER_PROC &&
            (reinterpret_cast<SwheavyPlayer*>(rider)->mEquipFlags & 0x02000000))
            a->mHeavyRiding = 1;
    }
}

/* 02394730 */
void daObjSwheavy::Act_c::calc_top_pos() {
    u32 type = mType;
    f32 target = mTargetHFrac, current = mCurHFrac, difference = current - target;
    type = checkedType(this, type, "d_a_obj_swheavy.cpp", "(M_type >=0) && (M_type <Type_Max)");
    const daObjSwheavy::Attr_c* params = attr(type);
    f32 decay = static_cast<f32>(params->mVSpeedDecay);
    if (type >= 4) {
        JUT_assert("d_a_obj_swheavy.cpp", 0xFD, "(M_type >=0) && (M_type <Type_Max)");
        type = mType;
        params = attr(type);
    }
    f32 speed = mVSpeed, spring = static_cast<f32>(params->mVSpring);
    speed = gabi::fnmsubs(difference, spring, speed);
    current = mCurHFrac;
    speed = gabi::fnmsubs(speed, decay, speed);
    current = current + speed;
    mCurHFrac = current;
    f32 height = (1.0f - current) * -35.5f;
    s16 timer = m330;
    mVSpeed = speed;
    if (height < -36.5f)
        height = -36.5f;
    else {
        f32 delta = height - 1.0f;
        height = delta >= 0.0f ? 1.0f : height;
    }
    m328 = height;
    if (timer > 0) {
        timer = (s16)(timer - 1);
        m330 = timer;
        if (timer == 0)
            m32C = m334;
    }
    f32 fraction = mMode == 0 ? (f32)mCurHFrac : (f32)m32C;
    f32 top = (1.0f - fraction) * -35.5f;
    f32 bottom = m328;
    mTopPos = top;
    m338 = fraction;
    if (top < bottom)
        mTopPos = bottom;
}

/* 02394ADC */
void daObjSwheavy::Act_c::top_bg_aim_req(f32 target, s16 timer) {
    m334 = target;
    m330 = timer;
}

/* 02394510 */
void daObjSwheavy::Act_c::set_push_flag() {
    if (mMiniPushFlg) {
        if (mRiding) {
            u32 type = checkedType(this, mType, "d_a_obj_swheavy.cpp", "(M_type >=0) && (M_type <Type_Max)");
            mMiniPushTimer = static_cast<s16>(attr(type)->mMiniPushDelay);
        } else {
            s16 timer = (s16)((s16)mMiniPushTimer - 1);
            mMiniPushTimer = timer;
            if (timer <= 0)
                mMiniPushFlg = 0;
        }
    } else if (mRiding) {
        u32 type = checkedType(this, mType, "d_a_obj_swheavy.cpp", "(M_type >=0) && (M_type <Type_Max)");
        s16 timer = (s16)((s16)mMiniPushTimer + 1);
        mMiniPushTimer = timer;
        if (timer >= static_cast<s16>(attr(type)->mMiniPushDelay))
            mMiniPushFlg = 1;
    } else
        mMiniPushTimer = 0;
    if (mPushFlg) {
        if (mHeavyRiding) {
            u32 type = checkedType(this, mType, "d_a_obj_swheavy.cpp", "(M_type >=0) && (M_type <Type_Max)");
            mPushTimer = static_cast<s16>(attr(type)->mPushDelay);
        } else {
            s16 timer = (s16)((s16)mPushTimer - 1);
            mPushTimer = timer;
            if (timer <= 0)
                mPushFlg = 0;
        }
    } else if (mHeavyRiding) {
        u32 type = checkedType(this, mType, "d_a_obj_swheavy.cpp", "(M_type >=0) && (M_type <Type_Max)");
        s16 timer = (s16)((s16)mPushTimer + 1);
        mPushTimer = timer;
        if (timer >= static_cast<s16>(attr(type)->mPushDelay))
            mPushFlg = 1;
    } else
        mPushTimer = 0;
}

/* 023940D8 */
void daObjSwheavy::Act_c::mode_upper_init() {
    mMode = 0;
    mTargetHFrac = 1.0f;
    mChangingState = 0;
}

/* 02394BCC */
void daObjSwheavy::Act_c::mode_upper() {
    f32 height = 1.0f;
    u8 mini = mMiniPushFlg;
    u32 type = mType;
    if (mini) {
        type = checkedType(this, type, "d_a_obj_swheavy.cpp", "(M_type >=0) && (M_type <Type_Max)");
        if (!(static_cast<u32>(attr(type)->mFlags) & Act_c::FLAG_REQ_HEAVY) || mPushFlg) {
            mChangingState = 1;
            mode_u_l_init();
            return;
        }
        height = 0.9f;
    }
    type = checkedType(this, type, "d_a_obj_swheavy.cpp", "(M_type >=0) && (M_type <Type_Max)");
    if ((static_cast<u32>(attr(type)->mFlags) & Act_c::FLAG_OBEY_SAVE) && isSwitch(this)) {
        mode_u_l_init();
        return;
    }
    mTargetHFrac = height;
    top_bg_aim_req(height, 1);
}

/* 02394AE8 */
void daObjSwheavy::Act_c::mode_u_l_init() {
    u32 type = mType;
    mMode = 1;
    mTargetHFrac = 0.0f;
    type = checkedType(this, type, "d_a_obj_swheavy.cpp", "(M_type >=0) && (M_type <Type_Max)");
    const daObjSwheavy::Attr_c* params = attr(type);
    type = mType;
    mVSpeed = static_cast<f32>(params->mPushVSpeed0);
    type = checkedType(this, type, "d_a_obj_swheavy.cpp", "(M_type >=0) && (M_type <Type_Max)");
    top_bg_aim_req(0.0f, static_cast<s16>(attr(type)->m14));
}

/* 02394D50 */
void daObjSwheavy::Act_c::mode_u_l() {
    if (mCurHFrac > 0.0f)
        return;
    if (mChangingState) {
        u32 type = checkedType(this, mType, "d_a_obj_swheavy.cpp", "(M_type >=0) && (M_type <Type_Max)");
        u32 toggle = static_cast<u32>(attr(type)->mFlags) & Act_c::FLAG_IS_TOGGLE,
            sw = PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
        s8 room = home.roomNo;
        if (toggle)
            dComIfGs_revSwitch(sw, room);
        else
            dComIfGs_onSwitch(sw, room);
    }
    s8 room = current.roomNo;
    s32 reverb = dComIfGp_getReverb(room);
    mDoAud_seStart(JA_SE_OBJ_FOOT_SW_ON, &eyePos, 0, reverb);
    mode_lower_init();
}

/* 023940F4 */
void daObjSwheavy::Act_c::mode_lower_init() {
    mChangingState = 0;
    mMode = 2;
    mTargetHFrac = 0.0f;
}

/* 02394E84 */
void daObjSwheavy::Act_c::mode_lower() {
    BOOL pressing = FALSE;
    u8 mini = mMiniPushFlg;
    u32 type = mType;
    if (mini) {
        type = checkedType(this, type, "d_a_obj_swheavy.cpp", "(M_type >=0) && (M_type <Type_Max)");
        if (!(static_cast<u32>(attr(type)->mFlags) & Act_c::FLAG_REQ_HEAVY) || mPushFlg)
            pressing = TRUE;
    }
    type = checkedType(this, type, "d_a_obj_swheavy.cpp", "(M_type >=0) && (M_type <Type_Max)");
    BOOL pop = !(static_cast<u32>(attr(type)->mFlags) & Act_c::FLAG_STAY_PRESSED) && !pressing;
    const daObjSwheavy::Attr_c* params = attr(type);
    type = checkedType(this, type, "d_a_obj_swheavy.cpp", "(M_type >=0) && (M_type <Type_Max)");
    params = attr(type);
    BOOL match = FALSE;
    if (static_cast<u32>(params->mFlags) & Act_c::FLAG_OBEY_SAVE) {
        BOOL active = isSwitch(this);
        if (!(active | pressing))
            match = TRUE;
    }
    if (pop || match) {
        type = checkedType(this, mType, "d_a_obj_swheavy.cpp", "(M_type >=0) && (M_type <Type_Max)");
        u32 toggle = (static_cast<u32>(attr(type)->mFlags) >> 2) & 1;
        if (pop && !(toggle | pressing))
            mChangingState = 1;
        mode_l_u_init();
    }
}

/* 02394E68 */
void daObjSwheavy::Act_c::mode_l_u_init() {
    mMode = 3;
    mTargetHFrac = 1.0f;
    top_bg_aim_req(1.0f, 1);
}

/* 0239502C */
void daObjSwheavy::Act_c::mode_l_u() {
    if (mCurHFrac < 1.0f)
        return;
    if (mChangingState) {
        u32 sw = PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
        s8 room = home.roomNo;
        dComIfGs_offSwitch(sw, room);
    }
    mode_upper_init();
}

/* 023948D4 */
BOOL daObjSwheavy::Act_c::_execute() {
    set_push_flag();
    (this->*mode_proc.mProc[static_cast<u32>(mMode)])();
    calc_top_pos();
    u8 riding = mRiding, heavy = mHeavyRiding;
    mPrevRiding = riding;
    mPrevHeavyRiding = heavy;
    mHeavyRiding = 0;
    mRiding = 0;
    set_mtx();
    mpBgW1->Move();
    mpBgW2->Move();
    f32 height = m328, y = current.pos.y;
    y = (y + height) - -35.5f;
    f32 x = current.pos.x, z = current.pos.z;
    eyePos.x = x;
    eyePos.z = z;
    eyePos.y = y;
    return TRUE;
}

/* 02394A28 */
BOOL daObjSwheavy::Act_c::_draw() {
    auto env = dKy_getEnvlight();
    settingTevStruct(env, 1, &current.pos, &tevStr);
    env = dKy_getEnvlight();
    auto model = (J3DModel*)mpModel1;
    setLightTevColorType(env, model, &tevStr);
    env = dKy_getEnvlight();
    model = (J3DModel*)mpModel2;
    setLightTevColorType(env, model, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel1, 0);
    mDoExt_modelUpdateDL(mpModel2, 0);
    dComIfGd_setList();
    return TRUE;
}

/* 023950A8 */
static s32 Mthd_Create(Act_c* a) {
    return a->_create();
}

/* 023950AC */
static BOOL Mthd_Delete(Act_c* a) {
    return a->_delete();
}

/* 023950B0 */
static BOOL Mthd_Execute(Act_c* a) {
    return a->_execute();
}

/* 023950B4 */
static BOOL Mthd_Draw(Act_c* a) {
    return a->_draw();
}

/* 023951B8 */
static BOOL Mthd_IsDelete(void* a) {
    return TRUE;
}

// HD: compiler-generated and template code of this translation unit.

/* 023951C0 */
static u32 PrmAbstract(Act_c* a, s32 width, s32 shift) {
    u32 value = a->mParameters, w = (u32)width & 63, sh = (u32)shift & 63;
    return (sh < 32 ? value >> sh : 0) & ((w < 32 ? 1u << w : 0) - 1);
}

/* 023950B8 */
static void sinit() {
    __sinit_header_statics();
}

/* 0239514C */
static void trivialDestructor(void* a, s32 flags) {
    if (a && (flags & 1))
        operator_delete(a);
}

/* 02395160 */
static void emptyVirtual(void* a) {}

/* 02395164 */
static void destructor(Act_c* a, s32 flags) {
    if (a) {
        Swheavy_actorDtor(a, 0);
        if (flags & 1)
            operator_delete(a);
    }
}

/* 023951DC */
static void colorToFloat(void* dest, void* src) {
    GXColor* color = static_cast<GXColor*>(src);
    SwheavyColorFloat* out = static_cast<SwheavyColorFloat*>(dest);
    f32 r = static_cast<f32>(static_cast<u8>(color->r)) / 255.0f,
        g = static_cast<f32>(static_cast<u8>(color->g)) / 255.0f;
    f32 b = static_cast<f32>(static_cast<u8>(color->b)) / 255.0f,
        alpha = static_cast<f32>(static_cast<u8>(color->a)) / 255.0f;
    out->r = r;
    out->g = g;
    out->b = b;
    out->a = alpha;
}

/* WWHD glue
 data M_attr 1002FD90
 data mode_proc 1002FC78
 ext 0273AA24 void Swheavy_assert(const char*,u32,const char*)
 ext 024F23F4 SwheavyBgW* Swheavy_newBgW(s32)
 ext 0200A030 BOOL Swheavy_setBg(SwheavyBgW*,cBgD_t*,u32,Mtx34*)
 ext 025F1B48 void Swheavy_ZXYrotM(Mtx34*,s16,s16,s16)
 ext 025D4ED0 void Swheavy_actorCtor(Act_c*)
 ext 025D50BC void Swheavy_actorDtor(Act_c*,s32)
 str 1002FD88 "Hhbot"
 str 1002FB9C "d_a_obj_swheavy.cpp" in 02393CD0
 str 1002FB5C "mdl_1_data != 0"
 str 1002FB6C "mdl_2_data != 0"
 str 1002FB7C "bgw_1_data != 0"
 str 1002FB8C "bgw_2_data != 0"
 str 1002FC04 "d_a_obj_swheavy.cpp" in 02394510
 str 1002FC18 "(M_type >=0) && (M_type <Type_Max)" in 02394510
 str 1002FC40 "d_a_obj_swheavy.cpp" in 02394730
 str 1002FC54 "(M_type >=0) && (M_type <Type_Max)" in 02394730
 str 1002FC98 "d_a_obj_swheavy.cpp" in 02394AE8
 str 1002FCAC "(M_type >=0) && (M_type <Type_Max)" in 02394AE8
 str 1002FCD4 "d_a_obj_swheavy.cpp" in 02394BCC
 str 1002FCE8 "(M_type >=0) && (M_type <Type_Max)" in 02394BCC
 str 1002FD0C "d_a_obj_swheavy.cpp" in 02394D50
 str 1002FD20 "(M_type >=0) && (M_type <Type_Max)" in 02394D50
 str 1002FD44 "d_a_obj_swheavy.cpp" in 02394E84
 str 1002FD58 "(M_type >=0) && (M_type <Type_Max)" in 02394E84
 str 1002FBCC "d_a_obj_swheavy.cpp" in 02394114
 str 1002FBE0 "(M_type >=0) && (M_type <Type_Max)" in 02394114
*/
