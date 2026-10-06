/** Salvaged treasure box. Ported from zeldaret/tww, checked against WWHD cking.rpx.
 * HD animation/light layouts and emitter inline writes differ. */
#include "d/actor/d_a_sbox.h"
static u32 eventManager() { return gabi::ea(dComIfGp_get()) + 0x52C4; }
static void cutEnd(s32 staff) {
    u32 mgr = eventManager();
    gabi::call(0x02543280, mgr, staff);
}
static void invalidateEmitter(void *e) {
    u32 p = gabi::ea(e);
    u32 flags = gabi::load<u32>(p + 0x254);
    gabi::store<s32>(p + 0x5C, -1);
    gabi::store<u32>(p + 0x254, flags | 1);
}
BOOL daSbox_c::volmProc() {
    WWHD_FUNC(0x0246A744, BOOL, this);
    if (!mEmitter)
        return TRUE;
    s16 timer = (s16)(mVolumeTimer + 1);
    mVolumeTimer = timer;
    if (timer == 36)
        gabi::store<u8>(gabi::ea((void *)mEmitter) + 0x247, 255);
    else if (timer >= 181) {
        gabi::store<u8>(gabi::ea((void *)mEmitter) + 0x247, 0);
        invalidateEmitter(mEmitter);
        mEmitter = nullptr;
        return TRUE;
    } else if (timer > 156)
        gabi::store<u8>(gabi::ea((void *)mEmitter) + 0x247, (181 - timer) * 10);
    return FALSE;
}
VERIFY(0x0246A744, &daSbox_c::volmProc);
BOOL daSbox_c::darkProc() {
    WWHD_FUNC(0x0246A7E0, BOOL, this);
    if (!chkFlag(0x10))
        return TRUE;
    s16 timer = (s16)(mDarkTimer + 1);
    mDarkTimer = timer;
    if (timer > 150) {
        mDarkRatio = 1.0f;
        gabi::call(0x02560444, 1.0f);
        clrFlag(0x10);
        return TRUE;
    }
    if (timer > 120)
        mDarkRatio = gabi::fmadds(((f32)timer - 120.0f) / 30.0f, 0.6f, 0.4f);
    gabi::call(0x02560444, (f32)mDarkRatio);
    return FALSE;
}
VERIFY(0x0246A7E0, &daSbox_c::darkProc);
BOOL daSbox_c::lightProc() {
    WWHD_FUNC(0x0246A8EC, BOOL, this);
    if (!chkFlag(0x20))
        return TRUE;
    s16 timer = (s16)(mLightTimer + 1);
    f32 p = mLight.mPower;
    mLightTimer = timer;
    if (timer < 156) {
        if (p < 130.0f)
            mLight.mPower = p + 13.0f;
        f32 q = mEffectLight.mPower;
        if (q < 120.0f)
            mEffectLight.mPower = q + 12.0f;
    } else {
        f32 q = mEffectLight.mPower;
        mLight.mPower = p > 5.2f ? p - 5.2f : 0.0f;
        f32 next = q > 4.8f ? q - 4.8f : 0.0f;
        f32 first = mLight.mPower;
        mEffectLight.mPower = next;
        if (first == 0.0f && next == 0.0f) {
            clrFlag(0x20);
            return TRUE;
        }
    }
    return FALSE;
}
VERIFY(0x0246A8EC, &daSbox_c::lightProc);
void daSbox_c::lightInit() {
    WWHD_FUNC(0x0246A1EC, void, this);
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    mLight.mPos.set(x, y + 55.0f, z);
    mEffectLight.mPos.set(x, y + 50.0f, z);
    mLight.mColorR = 255;
    mLight.mColorG = 255;
    mLight.mColorB = 255;
    mEffectLight.mColorR = 255;
    mEffectLight.mColorG = 255;
    mEffectLight.mColorB = 100;
    mLight.mPower = 0;
    mLight.mFluctuation = 0;
    mEffectLight.mPower = 0;
    mEffectLight.mFluctuation = 0;
    mLightTimer = 0;
    setFlag(0x20);
    gabi::call(0x0255A2B8, &mLight);
    gabi::call(0x0255B9C8, &mEffectLight);
}
VERIFY(0x0246A1EC, &daSbox_c::lightInit);
void daSbox_c::demoInitCom() {
    WWHD_FUNC(0x0246A124, void, this);
    s32 staff = mStaff;
    u32 mgr = eventManager();
    if (gabi::call<void *>(0x0254487C, mgr, staff, gabi::at<char>(0x10039D8C), 3))
        setFlag(8);
}
VERIFY(0x0246A124, &daSbox_c::demoInitCom);
void daSbox_c::demoInitWait() {
    WWHD_FUNC(0x0246A188, void, this);
    s32 staff = mStaff;
    u32 mgr = eventManager();
    void *p = gabi::call<void *>(0x0254487C, mgr, staff, gabi::at<char>(0x10039D94), 3);
    mTimer = p ? gabi::load<s16>(gabi::ea(p) + 2) : 0;
}
VERIFY(0x0246A188, &daSbox_c::demoInitWait);
BOOL daSbox_c::demoProcWait() {
    WWHD_FUNC(0x0246A5FC, BOOL, this);
    s16 timer = mTimer;
    if (timer > 0)
        mTimer = timer - 1;
    else
        cutEnd(mStaff);
    return FALSE;
}
VERIFY(0x0246A5FC, &daSbox_c::demoProcWait);
void daSbox_c::demoProcDelete() {
    WWHD_FUNC(0x0246A70C, void, this);
    cutEnd(mStaff);
}
VERIFY(0x0246A70C, &daSbox_c::demoProcDelete);
void daSbox_c::demoInitDelete() {
    WWHD_FUNC(0x0246A56C, void, this);
    if (chkFlag(2)) {
        if (mEmitter) {
            invalidateEmitter(mEmitter);
            mEmitter = nullptr;
        }
        if (chkFlag(0x10)) {
            gabi::call(0x02560444, 1.0f);
            clrFlag(0x10);
        }
    }
    dKy_plight_cut(&mLight);
    gabi::call(0x0255BA9C, &mEffectLight);
}
VERIFY(0x0246A56C, &daSbox_c::demoInitDelete);
void daSbox_c::demoProcOpen() {
    WWHD_FUNC(0x0246A654, void, this);
    if (chkFlag(1)) {
        if (mBck1.play()) {
            s32 reverb = gabi::call<s32>(0x02520540, (s8)current.roomNo);
            mDoAud_seStart(0x690D, &eyePos, 0, reverb);
            cutEnd(mStaff);
            clrFlag(1);
        }
    } else
        cutEnd(mStaff);
}
VERIFY(0x0246A654, &daSbox_c::demoProcOpen);
void daSbox_c::demoProcCom() {
    WWHD_FUNC(0x0246AA0C, void, this);
    if (chkFlag(2)) {
        BOOL a = mBck2.play(), b = mBtk.play(), c = mBrk.play();
        if (gabi::ftoi(mBck2.getFrame()) == 36)
            setFlag(4);
        BOOL d = volmProc(), e = darkProc(), f = lightProc();
        if (a && b && c && d && e && f)
            clrFlag(6);
    }
}
VERIFY(0x0246AA0C, &daSbox_c::demoProcCom);
s32 daSbox_c::getNowEventAction() {
    WWHD_FUNC(0x0246A0D8, s32, this);
    s32 staff = mStaff;
    u32 mgr = eventManager();
    return gabi::call<s32>(0x02542EDC, mgr, staff, gabi::at<void>(0x101D0000), 3, 0, 1);
}
VERIFY(0x0246A0D8, &daSbox_c::getNowEventAction);
BOOL daSbox_c::demoProc() {
    WWHD_FUNC(0x0246AAE8, BOOL, this);
    u32 mgr = eventManager();
    s32 staff = gabi::call<s32>(0x02542D88, mgr, gabi::at<char>(0x10039DD0), 0, 0);
    mStaff = staff;
    if (staff == -1)
        return FALSE;
    s32 action = getNowEventAction();
    staff = mStaff;
    mgr = eventManager();
    if (gabi::call<BOOL>(0x025447C8, mgr, staff)) {
        setFlag(1);
        demoInitCom();
        switch (action) {
        case 0:
            demoInitWait();
            break;
        case 1:
            demoInitOpen();
            break;
        case 2:
            demoInitDelete();
            break;
        }
    }
    switch (action) {
    case 0:
        demoProcWait();
        break;
    case 1:
        demoProcOpen();
        break;
    case 2:
        demoProcDelete();
        break;
    default:
        cutEnd(mStaff);
    }
    demoProcCom();
    return TRUE;
}
VERIFY(0x0246AAE8, &daSbox_c::demoProc);
BOOL daSbox_c::actionWait() {
    WWHD_FUNC(0x0246AC6C, BOOL, this);
    shipMtx();
    if (!demoProc())
        fopAcM_delete(this);
    return TRUE;
}
VERIFY(0x0246AC6C, &daSbox_c::actionWait);
static BOOL daSbox_Execute(daSbox_c *self) {
    WWHD_FUNC(0x0246ACB4, BOOL, self);
    if (self->mAction == 0)
        self->actionWait();
    return TRUE;
}
VERIFY(0x0246ACB4, daSbox_Execute);
static BOOL daSbox_IsDelete(daSbox_c *self) {
    WWHD_FUNC(0x0246ACE4, BOOL, self);
    return TRUE;
}
VERIFY(0x0246ACE4, daSbox_IsDelete);
static BOOL daSbox_Delete(daSbox_c *self) {
    WWHD_FUNC(0x0246ACEC, BOOL, self);
    gabi::call(0x025204C8, &self->mPhase, gabi::at<char>(0x10039DE0));
    mDoAud_seDeleteObject(&self->eyePos);
    return TRUE;
}
VERIFY(0x0246ACEC, daSbox_Delete);
void daSbox_c::calcMtx() {
    WWHD_FUNC(0x024699BC, void, this);
    J3DModel_setBaseScale(mpModel1, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpModel1, mDoMtx_stack_c::get());
    J3DModel_setBaseScale(mpModel2, &scale);
    mDoMtx_stack_c::transS(current.pos.x, (f32)current.pos.y + 50.0f, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    J3DModel_setBaseTRMtx(mpModel2, mDoMtx_stack_c::get());
}
VERIFY(0x024699BC, &daSbox_c::calcMtx);
BOOL daSbox_c::CreateHeap() {
    WWHD_FUNC(0x02469B40, BOOL, this);
    auto res = [](s32 index) {
        return dComIfG_getObjectRes(gabi::at<char>(0x10039D0C), index, 0x10039CB4);
    };
    J3DModelData *data = (J3DModelData *)res(0x15);
    if (!data)
        JUT_ASSERT_fail(gabi::at<char>(0x10039D2C), 113, gabi::at<char>(0x10039D3C));
    J3DAnmTransform *anm = (J3DAnmTransform *)res(8);
    if (!mBck1.init(data, anm, true, 0, 1.0f, 0, -1, false))
        return FALSE;
    mpModel1 = mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
    if (!mpModel1)
        return FALSE;
    data = (J3DModelData *)res(0x16);
    if (!data)
        JUT_ASSERT_fail(gabi::at<char>(0x10039D2C), 140, gabi::at<char>(0x10039D14));
    mpModel2 = mDoExt_J3DModel__create(data, 0x80000, 0x01000200);
    if (!mpModel2)
        return FALSE;
    anm = (J3DAnmTransform *)res(0xB);
    if (!mBck2.init(data, anm, true, 0, 1.0f, 0, -1, false))
        return FALSE;
    auto btk = (J3DAnmTextureSRTKey *)res(0x25);
    if (!mBtk.init(data, btk, true, 0, 1.0f, 0, -1, false, 0))
        return FALSE;
    void *brk = res(0x1E);
    if (!gabi::call<BOOL>(0x025E8154, &mBrk, data, brk, true, 0, 1.0f, (s16)0, (s16)-1, false, 0))
        return FALSE;
    calcMtx();
    return TRUE;
}
VERIFY(0x02469B40, &daSbox_c::CreateHeap);
static BOOL CheckCreateHeap(daSbox_c *self) {
    WWHD_FUNC(0x02469DA0, BOOL, self);
    return self->CreateHeap();
}
VERIFY(0x02469DA0, CheckCreateHeap);
void daSbox_c::shipMtx() {
    WWHD_FUNC(0x02469EB0, void, this);
    u32 play = gabi::ea(dComIfGp_get());
    u32 ship = gabi::load<u32>(play + 0x5B3C);
    if (!ship)
        JUT_ASSERT_fail(gabi::at<char>(0x10039D64), 220, gabi::at<char>(0x10039D60));
    auto body = [ship]() {
        u32 morf = gabi::load<u32>(ship + 0x3B4);
        u32 model = gabi::load<u32>(morf + 0x90);
        return model ? model + 0xC8 : 0;
    };
    Mtx34 *matrix = mDoMtx_stack_c::get();
    gabi::call(0x028E90D4, gabi::at<Mtx34>(body()), matrix);
    mDoMtx_stack_c::transM(0.0f, 22.35f, 20.0f);
    mDoMtx_stack_c::YrotM(0x7FFF);
    J3DModel_setBaseTRMtx(mpModel1, matrix);
    /* HD recompiled lfs/stfs transport preserves SNaN bits for x/y here,
     * whereas z is quieted. Keep those bits before the integer eyePos copies;
     * normal gabi::load<f32> would quiet x/y prematurely. */
    auto rawFloat = [](u32 a) {
        u32 bits = gabi::load<u32>(a);
        f32 f;
        memcpy(&f, &bits, 4);
        return f;
    };
    f32 x = rawFloat(gabi::ea(matrix) + 0xC);
    current.pos.x = x;
    f32 y = rawFloat(gabi::ea(matrix) + 0x1C);
    current.pos.y = y;
    f32 z = matrix->m[2][3];
    gabi::store<f32>(gabi::ea(this) + 0x390, x);
    u32 ybits = gabi::load<u32>(gabi::ea(&current.pos.y));
    current.pos.z = z;
    u32 zbits = gabi::load<u32>(gabi::ea(&current.pos.z));
    gabi::store<u32>(gabi::ea(&eyePos.y), ybits);
    gabi::store<u32>(gabi::ea(&eyePos.z), zbits);
    u32 xbits = gabi::load<u32>(gabi::ea(&current.pos.x));
    gabi::store<f32>(gabi::ea(this) + 0x394, y);
    gabi::store<u32>(gabi::ea(&eyePos.x), xbits);
    gabi::store<f32>(gabi::ea(this) + 0x398, z);
    current.angle.y = gabi::load<s16>(ship + 0x32A) + 0x7FFF;
    shape_angle.y = gabi::load<s16>(ship + 0x32A);
    gabi::call(0x028E90D4, gabi::at<Mtx34>(body()), matrix);
    mDoMtx_stack_c::transM(0.0f, 72.35f, 20.0f);
    J3DModel_setBaseTRMtx(mpModel2, matrix);
}
VERIFY(0x02469EB0, &daSbox_c::shipMtx);
static BOOL daSbox_Draw(daSbox_c *self) {
    WWHD_FUNC(0x02469DA4, BOOL, self);
    J3DModelData *data = J3DModel_getModelData(self->mpModel1);
    if (!self->chkFlag(8))
        return TRUE;
    auto env = dKy_getEnvlight();
    settingTevStruct(env, 0, &self->current.pos, &self->tevStr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, self->mpModel1, &self->tevStr);
    self->mBck1.entry(data, self->mBck1.getFrame());
    mDoExt_modelUpdateDL(self->mpModel1);
    if (self->chkFlag(4)) {
        data = J3DModel_getModelData(self->mpModel2);
        self->mBck2.entry(data, self->mBck2.getFrame());
        self->mBtk.entry(data, self->mBtk.getFrame());
        gabi::call(0x025E83FC, &self->mBrk, data, self->mBrk.getFrame());
        u32 play = gabi::ea(dComIfGp_get());
        gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D84));
        play = gabi::ea(dComIfGp_get());
        gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D88));
        mDoExt_modelUpdateDL(self->mpModel2);
        play = gabi::ea(dComIfGp_get());
        gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D78));
        play = gabi::ea(dComIfGp_get());
        gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D7C));
    }
    return TRUE;
}
VERIFY(0x02469DA4, daSbox_Draw);
void daSbox_c::demoInitOpen() {
    WWHD_FUNC(0x0246A2AC, void, this);
    s8 room = current.roomNo;
    mBck1.mFrameCtrl.mRate = 1.0f;
    mBck2.mFrameCtrl.mRate = 1.0f;
    mBrk.mFrameCtrl.mRate = 1.0f;
    mBtk.mFrameCtrl.mRate = 1.0f;
    s32 reverb = gabi::call<s32>(0x02520540, room);
    mDoAud_seStart(0x690C, &eyePos, 0, reverb);
    reverb = gabi::call<s32>(0x02520540, (s8)current.roomNo);
    mDoAud_seStart(0x690B, &eyePos, 0, reverb);
    gabi::call(0x025E1918, 0x80000009u);
    setFlag(2);
    auto particle = [this](u16 id) {
        u32 play = gabi::ea(dComIfGp_get());
        u32 control = gabi::load<u32>(play + 0x5AB0);
        return gabi::call<void *>(0x025A847C, control, 0, id, &current.pos, &current.angle, 0,
                                  (u8)255, 0, (s8)-1, 0, 0, 0);
    };
    particle(0x1F1);
    particle(0x1F2);
    particle(0x1F6);
    void *e = particle(0x1F3);
    if (e) {
        u32 p = gabi::ea(e);
        gabi::store<f32>(p + 0x23C, 1.0f);
        gabi::store<f32>(p + 0x10, 1.0f);
        gabi::store<f32>(p + 8, 0.7f);
        gabi::store<f32>(p + 0x220, 0.7f);
        gabi::store<f32>(p + 0x238, 0.7f);
        gabi::store<f32>(p + 0x228, 1.0f);
        gabi::store<f32>(p + 0xC, 1.0f);
        gabi::store<f32>(p + 0x240, 1.0f);
        gabi::store<f32>(p + 0x224, 1.0f);
    }
    e = particle(0x1F4);
    if (e) {
        u32 p = gabi::ea(e);
        gabi::store<f32>(p + 0xC, 1.0f);
        gabi::store<f32>(p + 8, 0.7f);
        gabi::store<f32>(p + 0x10, 1.0f);
    }
    e = particle(0x1F5);
    mEmitter = e;
    if (e) {
        mVolumeTimer = 0;
        gabi::store<u8>(gabi::ea((void *)mEmitter) + 0x247, 0);
        gabi::store<f32>(gabi::ea((void *)mEmitter) + 0x23C, 0.7f);
    }
    setFlag(0x10);
    mDarkTimer = 0;
    mDarkRatio = 0.4f;
    lightInit();
}
VERIFY(0x0246A2AC, &daSbox_c::demoInitOpen);
static daSbox_c *daSbox_ctor(daSbox_c *self) {
    WWHD_FUNC(0x0246AD30, daSbox_c *, self);
    if (!self)
        self = gabi::call<daSbox_c *>(0x0273AD10, 0x62C);
    if (self) {
        fopAc_ac_c_ct(self);
        self->__vtbl = 0x10039CF4;
        auto word = [self](u32 off, u32 v) { gabi::store<u32>(gabi::ea(self) + off, v); };
        gabi::call(0x027F2BC0, &self->mBck1, 0);
        word(0x3C8, 0x1016E54C);
        gabi::call(0x027DA984, gabi::at<void>(gabi::ea(self) + 0x3CC));
        word(0x400, 0x1016D820);
        word(0x440, 0);
        word(0x434, 0);
        word(0x43C, 0);
        word(0x3C8, 0x10039CCC);
        word(0x438, 0);
        word(0x410, 0);
        gabi::call(0x027F2BC0, &self->mBck2, 0);
        word(0x458, 0x1016E54C);
        gabi::call(0x027DA984, gabi::at<void>(gabi::ea(self) + 0x45C));
        word(0x4C4, 0);
        word(0x4CC, 0);
        word(0x4C8, 0);
        word(0x4D0, 0);
        word(0x4A0, 0);
        word(0x490, 0x1016D820);
        word(0x458, 0x10039CCC);
        mDoExt_btkAnm::ct(&self->mBtk);
        gabi::call(0x025E80D0, &self->mBrk);
        self->mEffectLight.mHD20 = 1.0f;
        self->mLight.mHD20 = 1.0f;
    }
    return self;
}
VERIFY(0x0246AD30, daSbox_ctor);
BOOL daSbox_c::CreateInit() {
    WWHD_FUNC(0x0246AE28, BOOL, this);
    s8 room = current.roomNo;
    mAction = 0;
    tevStr.mRoomNo = room;
    calcMtx();
    return TRUE;
}
VERIFY(0x0246AE28, &daSbox_c::CreateInit);
cPhs_State daSbox_c::create() {
    WWHD_FUNC(0x0246AE5C, cPhs_State, this);
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this)
            daSbox_ctor(this);
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State phase = gabi::call<cPhs_State>(0x02520460, &mPhase, gabi::at<char>(0x10039DE0));
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    if (!gabi::call<BOOL>(0x025D63E8, this, 0x02469DA0, 0x15C0))
        return cPhs_ERROR_e;
    CreateInit();
    return cPhs_COMPLEATE_e;
}
VERIFY(0x0246AE5C, &daSbox_c::create);
static cPhs_State daSbox_Create(daSbox_c *self) {
    WWHD_FUNC(0x0246AF04, cPhs_State, self);
    return self->create();
}
VERIFY(0x0246AF04, daSbox_Create);
static void __sinit_d_a_sbox_cpp() {
    WWHD_FUNC(0x0246AF08, void, 0);
    sinit_header_statics(0x1046D86C, 0x101D000C);
}
VERIFY(0x0246AF08, __sinit_d_a_sbox_cpp);
static void light_dtor_sbox(void *self, s32 flags) {
    WWHD_FUNC(0x0246AF9C, void, self, flags);
    if (self && (flags & 1))
        gabi::call(0x0273AF40, self);
}
VERIFY(0x0246AF9C, light_dtor_sbox);
static void daSbox_dtor(daSbox_c *self, s32 flags) {
    WWHD_FUNC(0x0246AFB0, void, self, flags);
    if (self) {
        gabi::call(0x027F3628, gabi::at<void>(gabi::ea(self) + 0x458), 0);
        gabi::call(0x027F3628, gabi::at<void>(gabi::ea(self) + 0x3C8), 0);
        gabi::call(0x025D50BC, self, 0);
        if (flags & 1)
            gabi::call(0x0273AF40, self);
    }
}
VERIFY(0x0246AFB0, daSbox_dtor);
static void daSbox_emptyVirtual(daSbox_c *self) { WWHD_FUNC(0x0246B01C, void, self); }
VERIFY(0x0246B01C, daSbox_emptyVirtual);
