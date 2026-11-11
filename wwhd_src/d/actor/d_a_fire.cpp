/* Fire hazard, derived from GameCube source and audited HD disassembly. */
#include "d/actor/d_a_fire.h"
/* 0213F5D0 */
bool daFire_c::_delete() {
    for (int i = 0; i < 3; ++i) {
        FireEmitterHD *e = emitter(i).get();
        if (e) {
            u32 flags = (u32)e->mFlags;
            e->mMaxFrame = -1;
            e->mFlags = flags | 1;
            emitter(i) = nullptr;
        }
    }
    return true;
}

/* 0213E930 */
bool daFire_c::CreateInit() {
    u32 param = (u32)mParameters;
    mSwitchNo = param & 255;
    mTrboxBit = (param >> 12) & 31;
    bool stopped = dComIfGs_isSwitch((s32)mSwitchNo, (s8)home.roomNo) != 0;
    if (!stopped)
        stopped = fireIsTbox(dComIfGs_ea() + 0x798, (u32)mTrboxBit) != 0;
    mWasStopped = stopped;
    if (fireIsTbox(dComIfGs_ea() + 0x798, (u32)mTrboxBit))
        return false;
    param = (u32)mParameters;
    u8 type = (param >> 17) & 7;
    mType = type;
    if (type == 0 && (s32)mSwitchNo == 255)
        mType = 2;
    mEventDelay = 30;
    u8 shape = (param >> 8) & 15;
    mShape = shape;
    if (shape == 1)
        scale.z = 0.0f;
    if (shape == 2) {
        FireEmitterHD *e = mpEmitter0.get();
        scale.x = 1.25f;
        scale.y = 1.25f;
        scale.z = 1.25f;
        if (e) {
            f32 z = (f32)scale.z, y = (f32)scale.y;
            e->mGlobalScale.z = z;
            e->mGlobalScale.y = y;
            e->mGlobalParticleScale.x = 1.25f;
            e->mGlobalParticleScale.y = y;
            e->mGlobalParticleScale.z = z;
            e->mGlobalScale.x = 1.25f;
        }
        for (int i = 1; i < 3; ++i) {
            e = emitter(i).get();
            if (e) {
                f32 z = (f32)scale.z, y = (f32)scale.y, x = (f32)scale.x;
                e->mGlobalScale.y = y;
                e->mGlobalScale.z = z;
                e->mGlobalParticleScale.x = x;
                e->mGlobalParticleScale.z = z;
                e->mGlobalParticleScale.y = y;
                e->mGlobalScale.x = x;
            }
        }
    }
    u32 st = gabi::ea(&mStts);
    fireSttsInit(st, 255, 255, gabi::ea(this));
    if ((u8)mShape == 1) {
        for (int i = 0; i < 3; ++i) {
            u32 c = gabi::ea(&mTgCyls[i]);
            fireCylinderSet(c, 0x101B4F64u);
            gabi::at<dCcD_Cyl>(c)->mStts.v = st;
            fireCylinderRadius(c + 0x118, 50.0f);
        }
    } else {
        fireCylinderSet(gabi::ea(&mTgCyls[0]), 0x101B4F64u);
        mTgCyls[0].mStts.v = st;
    }
    fireCylinderSet(gabi::ea(&mAtCyl), 0x101B4FECu);
    mAtCyl.mStts.v = st;
    fireCylinderSet(gabi::ea(&mCoCyl), 0x101B4FA8u);
    mCoCyl.mStts.v = st;
    f32 x = (f32)cXyz::Zero.x, y = (f32)cXyz::Zero.y, z = (f32)cXyz::Zero.z;
    mTgCyls[0].mGObjAt.mVec.x = x;
    mTgCyls[0].mGObjAt.mVec.y = y;
    mTgCyls[0].mGObjAt.mVec.z = z;
    fireOffDraw(gabi::ea(&draw_tag));
    x = (f32)fireBaseY().x;
    mParticleDir.x = x;
    y = (f32)fireBaseY().y;
    type = (u8)mType;
    mParticleDir.y = y;
    z = (f32)fireBaseY().z;
    mWindDir.x = x;
    mParticleDir.z = z;
    mWindDir.y = y;
    mWindDir.z = z;
    s32 sw;
    if (type == 0) {
        u32 sound = ((u32)mParameters >> 20) & 15;
        s16 off = dComIfGp_evmng_getEventIdx(sound ? "fire_off_cam_ws" : "fire_off_cam", 255);
        mOffEventIdx = off;
        s16 on = dComIfGp_evmng_getEventIdx("fire_on_cam", 255);
        sw = (s32)mSwitchNo;
        mOnEventIdx = on;
    } else if (type == 1) {
        u32 sound = ((u32)mParameters >> 20) & 15;
        s16 off = dComIfGp_evmng_getEventIdx(sound ? "zenfire_cam_ws" : "zenfire_cam", 255);
        sw = (s32)mSwitchNo;
        mOffEventIdx = off;
        mOnEventIdx = -1;
    } else {
        sw = (s32)mSwitchNo;
        mOffEventIdx = -1;
        mOnEventIdx = -1;
    }
    if (sw == 255) {
        mFireOn = 1;
        return true;
    }
    if (dComIfGs_isSwitch(sw, (s8)home.roomNo)) {
        mFireOn = 0;
        execStopFire();
    } else
        mFireOn = 1;
    return true;
}

/* 0213F3D0 */
s32 daFire_c::_create() {
    u32 flags = (u32)actor_condition;
    if (!(flags & 8)) {
        // HD: preserve the inline constructor null-this guard and store order.
        if (this) {
            fireActorConstruct(this);
            __vtbl = FIRE_ACTOR_VTABLE;
            fireSttsConstruct(gabi::ea(&mStts));
            fireGSttsConstruct(gabi::ea(&mStts.__vtbl_gstts));
            mStts.__vtbl = FIRE_STTS_VTABLE;
            mStts.__vtbl_gstts = FIRE_GSTTS_VTABLE;
            fireConstructArray(gabi::ea(&mTgCyls[0]), 3, 0x130, FIRE_CYLINDER_CTOR);
            fireCollisionConstruct(gabi::ea(&mCoCyl));
            fireCylinderVtables(&mCoCyl)->mShapeBaseVtable = FIRE_CYL_SHAPE_VTABLE;
            fireCylinderVtables(&mCoCyl)->mShapeVtable = FIRE_SHAPE_BASE_VTABLE;
            fireCylinderConstruct(gabi::ea(&mCoCyl.mCyl));
            mCoCyl.__vtbl_hitinf = FIRE_CYL_HIT_VTABLE;
            mCoCyl.mCyl.__vtbl = FIRE_CYL_VTABLE;
            fireCylinderVtables(&mCoCyl)->mShapeVtable = FIRE_CYL_SHAPE_FINAL_VTABLE;
            fireCollisionConstruct(gabi::ea(&mAtCyl));
            fireCylinderVtables(&mAtCyl)->mShapeVtable = FIRE_SHAPE_BASE_VTABLE;
            fireCylinderVtables(&mAtCyl)->mShapeBaseVtable = FIRE_CYL_SHAPE_VTABLE;
            fireCylinderConstruct(gabi::ea(&mAtCyl.mCyl));
            mAtCyl.mCyl.__vtbl = FIRE_CYL_VTABLE;
            fireCylinderVtables(&mAtCyl)->mShapeVtable = FIRE_CYL_SHAPE_FINAL_VTABLE;
            flags = (u32)actor_condition;
            mAtCyl.__vtbl_hitinf = FIRE_CYL_HIT_VTABLE;
        }
        actor_condition = flags | 8;
    }
    mpEmitter0.v = fireParticleSet(gabi::ea(dComIfGp_getParticle()), 0, 0x461,
                                    gabi::ea(&current.pos), 0, 0, 255, 0, -1, 0, 0, 0);
    mpEmitter1.v = fireParticleSet(gabi::ea(dComIfGp_getParticle()), 0, 0x462,
                                    gabi::ea(&current.pos), 0, 0, 255, 0, -1, 0, 0, 0);
    mpEmitter2.v = fireParticleSet(gabi::ea(dComIfGp_getParticle()), 4, 0x445B,
                                    gabi::ea(&current.pos), 0, 0, 255, 0, -1, 0, 0, 0);
    if (!CreateInit())
        return 5;
    ctrlEffect();
    return 4;
}

/* 0213FAF4 */
bool daFire_c::_execute() {
    s8 t = (s8)mEventDelay;
    if (t >= 0)
        mEventDelay = t - 1;
    checkOrder();
    ctrlEffect();
    // HD: the empty normal_proc call is omitted.
    eventOrder();
    checkCol();
    return true;
}

/* 0213EE34 */
void daFire_c::search_wind() {
    gabi::Local<FireSearchNameHD> type;
    type->name = 0x187;
    u32 w = fireActorSearch(FIRE_SEARCH_BY_NAME, type.a);
    mWindId = w ? (u32)gabi::at<fopAc_ac_c>(w)->mBsPcId : 0xFFFFFFFFu;
}

/* 0213F078 */
void daFire_c::ctrlEffect() {
    if (!(u8)mFireOn)
        return;
    bool stopped = false;
    search_wind();
    gabi::Local<be<u32>> id;
    u32 n = (u32)mWindId;
    *id = n;
    u32 wind = 0;
    if (n != 0xFFFFFFFFu)
        wind = fireActorSearch(FIRE_SEARCH_BY_ID, id.a);
    s32 hit = fireCheckTargetHit(gabi::ea(&mTgCyls[0]));
    f32 zero = 0.0f, one = 1.0f;
    if (hit) {
        u32 o = fireGetTargetHitObj(gabi::ea(&mTgCyls[0]));
        bool cleared = false;
        if (o) {
            u32 t = (u32)gabi::at<dCcD_GObjInf>(o)->mAtType;
            if ((t & 0x200000) && wind) {
                u32 owner = fireGetTargetHitActor(gabi::ea(&mTgCyls[0].mGObjTg));
                if (owner == wind) {
                    stopped = true;
                    fireClearTargetHit(gabi::ea(&mTgCyls[0]));
                    cleared = true;
                } else
                    t = (u32)gabi::at<dCcD_GObjInf>(o)->mAtType;
            }
            if (!cleared) {
                if (t & 0x80100) {
                    mStopTimer = 80;
                    field_0x8D1 = 0;
                    stopped = true;
                    execStopNowFire();
                    fireParticleSet(gabi::ea(dComIfGp_getParticle()), 0, 0x463,
                                    gabi::ea(&current.pos), 0, 0, 255, 0, -1, 0, 0, 0);
                } else if (t & 0x200000) {
                    u32 owner = fireGetTargetHitActor(gabi::ea(&mTgCyls[0].mGObjTg));
                    if (owner != wind) {
                        f32 z = (f32)mTgCyls[0].mGObjTg.mRVec.z,
                            x = (f32)mTgCyls[0].mGObjTg.mRVec.x, z2 = z * z,
                            y = (f32)mTgCyls[0].mGObjTg.mRVec.y;
                        f32 sq = gabi::fmadds(x, x, z2);
                        mWindDir.y = y;
                        mWindDir.x = x;
                        mWindDir.z = z;
                        mWindDir.y = fireSquareRoot(sq / 1000.0f);
                        if (!fireNormalizeRS(&mWindDir)) {
                            mWindDir.y = one;
                            mWindDir.x = zero;
                            mWindDir.z = zero;
                        }
                    }
                }
            }
        }
        if (!cleared)
            fireClearTargetHit(gabi::ea(&mTgCyls[0]));
    } else {
        fireChaseF(&mWindDir.x, zero, 0.025f);
        fireChaseF(&mWindDir.y, one, 0.025f);
        fireChaseF(&mWindDir.z, zero, 0.025f);
    }
    if ((u8)field_0x8D1 == 0 && (u8)mStopTimer != 0) {
        u8 c = (u8)mStopTimer;
        mStopTimer = c - 1;
        stopped = true;
        execStopFire();
    } else if (stopped)
        execStopFire();
    else {
        execPlayFire();
        s32 rev = fireReverb((s8)current.roomNo);
        fireSoundStart(0x614F, &eyePos, 0, rev);
    }
    if ((u8)mWasStopped != (u8)stopped) {
        u32 snd = stopped ? 0x6950 : 0x694E;
        s32 rev = fireReverb((s8)current.roomNo);
        fireSoundStart(snd, &eyePos, 0, rev);
    }
    setDirParticle(&mWindDir);
    mWasStopped = stopped;
}

/* 0213F654 */
void daFire_c::demo_proc() {
    s32 staff = (s32)mStaffId;
    u32 act = dComIfGp_evmng_getMyActIdx(staff, gabi::at<gptr<char>>(0x101B5030), 3, 0, 0);
    if (act == 1) {
        if (!(u8)mFireOn) {
            s32 rev = fireReverb((s8)current.roomNo);
            fireSoundStart(0x694E, &eyePos, 0, rev);
        }
        execPlayFire();
        staff = (s32)mStaffId;
        mFireOn = 1;
    } else if (act == 2) {
        FireEmitterHD *e = mpEmitter0.get();
        if (e && ((u32)e->mAliveParticles + (u32)e->mAliveChildren) && (u8)mFireOn) {
            s32 rev = fireReverb((s8)current.roomNo);
            fireSoundStart(0x6950, &eyePos, 0, rev);
        }
        execStopFire();
        staff = (s32)mStaffId;
        mFireOn = 0;
    } else
        staff = (s32)mStaffId;
    dComIfGp_evmng_cutEnd(staff);
}

/* 0213E8BC */
void daFire_c::normal_proc() {}

/* 0213F8C4 */
void daFire_c::eventOrder() {
    u8 off = fireIsTbox(dComIfGs_ea() + 0x798, (u32)mTrboxBit);
    u8 type = (u8)mType;
    if (type == 1) {
        if (fireSearchRoomEnemy((s8)current.roomNo))
            mNoEnemyTimer = 5;
        else if (!fireCalcTimer(&mNoEnemyTimer))
            off = 1;
    } else if (type == 0 && dComIfGs_isSwitch((s32)mSwitchNo, (s8)home.roomNo))
        off = 1;
    s32 state = (s32)mEventState;
    if (state == 1 || state == 2) {
        s16 ev = (s16)(state == 1 ? mOnEventIdx : mOffEventIdx);
        fireOrderOtherEvent(this, ev, 255, 65535, 0, 1);
        eventInfo.mCondition = (u16)eventInfo.mCondition | 2;
        mOffState = off != 0;
        return;
    }
    if (state == 0 && (s8)mEventDelay < 0 && (u8)mOffState != off)
        mEventState = off ? 2 : 1;
    mOffState = off != 0;
}

/* 0213F7A4 */
void daFire_c::checkOrder() {
    if ((u16)eventInfo.mCommand != 2)
        return;
    s16 ev = (s16)mOnEventIdx;
    bool reset = dComIfGp_evmng_startCheck(ev) && (u32)mEventState != 0;
    if (!reset) {
        ev = (s16)mOffEventIdx;
        reset = dComIfGp_evmng_startCheck(ev) && (u32)mEventState != 0;
    }
    if (reset)
        mEventState = 0;
    ev = (s16)mOnEventIdx;
    bool ended = ev != -1 && dComIfGp_evmng_endCheck(ev);
    if (!ended) {
        ev = (s16)mOffEventIdx;
        ended = ev != -1 && dComIfGp_evmng_endCheck(ev);
    }
    if (ended) {
        u32 p = gabi::ea(dComIfGp_get());
        gabi::at<FirePlayHD>(p)->mEventFlags = (u16)gabi::at<FirePlayHD>(p)->mEventFlags | 8;
    }
    const char *name = (*gabi::at<gptr<char>[2]>(0x101B503C))[(u8)mType].get();
    mStaffId = dComIfGp_evmng_getMyStaffId(name, nullptr, 0);
    demo_proc();
}

/* 0213E8C0 */
void daFire_c::execStopFire() {
    for (int i = 0; i < 3; ++i) {
        FireEmitterHD *e = emitter(i).get();
        if (e)
            e->mFlags = (u32)e->mFlags | 1;
    }
    u32 x = (u32)mTgCyls[0].mObjAt.mSPrm, y = (u32)mCoCyl.mObjCo.mSPrm,
        z = (u32)mAtCyl.mObjAt.mSPrm;
    mTgCyls[0].mObjAt.mSPrm = x & ~1u;
    mCoCyl.mObjCo.mSPrm = y & ~1u;
    mAtCyl.mObjAt.mSPrm = z & ~1u;
}

/* 0213EE90 */
void daFire_c::execStopNowFire() {
    for (int i = 0; i < 3; ++i) {
        FireEmitterHD *e = emitter(i).get();
        if (e)
            fireDeleteAllParticles(gabi::ea(e), 1);
    }
    u32 x = (u32)mTgCyls[0].mObjAt.mSPrm, y = (u32)mCoCyl.mObjCo.mSPrm,
        z = (u32)mAtCyl.mObjAt.mSPrm;
    mTgCyls[0].mObjAt.mSPrm = x & ~1u;
    mCoCyl.mObjCo.mSPrm = y & ~1u;
    mAtCyl.mObjAt.mSPrm = z & ~1u;
}

/* 0213EF18 */
void daFire_c::execPlayFire() {
    for (int i = 0; i < 3; ++i) {
        FireEmitterHD *e = emitter(i).get();
        if (e)
            e->mFlags = (u32)e->mFlags & ~1u;
    }
    u32 x = (u32)mTgCyls[0].mObjAt.mSPrm, y = (u32)mCoCyl.mObjCo.mSPrm,
        z = (u32)mAtCyl.mObjAt.mSPrm;
    mTgCyls[0].mObjAt.mSPrm = x | 1;
    mCoCyl.mObjCo.mSPrm = y | 1;
    mAtCyl.mObjAt.mSPrm = z | 1;
}

/* 0213EF88 */
void daFire_c::setDirParticle(cXyz *d) {
    fireChaseF(&mParticleDir.x, (f32)d->x, 1.0f);
    fireChaseF(&mParticleDir.y, (f32)d->y, 1.0f);
    fireChaseF(&mParticleDir.z, (f32)d->z, 1.0f);
    for (int i = 0; i < 3; ++i) {
        FireEmitterHD *e = emitter(i).get();
        if (e) {
            f32 y = (f32)mParticleDir.y, z = (f32)mParticleDir.z, x = (f32)mParticleDir.x;
            e->mDirection.z = z;
            e->mDirection.y = y;
            e->mDirection.x = x;
        }
    }
}

/* 0213FA58 */
void daFire_c::checkCol() {
    if ((u8)mShape != 1) {
        fireCylinderCenter(&mTgCyls[0].mCyl, &current.pos);
        fireCollisionSet(gabi::ea(dComIfG_Ccsp()), gabi::ea(&mTgCyls[0]));
    }
    fireCylinderCenter(&mAtCyl.mCyl, &current.pos);
    fireCollisionSet(gabi::ea(dComIfG_Ccsp()), gabi::ea(&mAtCyl));
    fireCylinderCenter(&mCoCyl.mCyl, &current.pos);
    fireCollisionSet(gabi::ea(dComIfG_Ccsp()), gabi::ea(&mCoCyl));
}

/* 0213F64C */
bool daFire_c::_draw() { return true; }

/* 0213F5CC */
s32 daFire_Create(daFire_c *a) { return a->_create(); }

/* 0213F648 */
bool daFire_Delete(daFire_c *a) { return a->_delete(); }

/* 0213FB54 */
bool daFire_Execute(daFire_c *a) { return a->_execute(); }

/* 0213FC78 */
bool daFire_IsDelete(daFire_c *a) { return true; }

/* 0213FB58 */
void staticInit() {
    fireHeaderStatics()->mCounter14 = 0;
    fireHeaderStatics()->mCounterC = 0;
    fireHeaderStatics()->mCounter18 = 0;
    fireHeaderStatics()->mCounter10 = 0;
    fireRegisterStatic(0x101B5044u);
    f32 lo = (f32)*gabi::at<be<f32>>(0x1000F168), hi = (f32)*gabi::at<be<f32>>(0x1000F16C);
    fireHeaderStatics()->mMinimum = lo;
    fireHeaderStatics()->mMaximum = hi;
    fireInitializeOne(0x10463DC0u);
    fireRegisterStatic(0x101B5050u);
    fireInitializeZero(0x10463DC1u);
    fireRegisterStatic(0x101B505Cu);
}

/* 0213FBEC */
u32 cylinderCtor(u32 a) {
    if (!a)
        a = fireNew(0x130);
    if (!a)
        return 0;
    fireCollisionConstruct(a);
    fireCylinderVtables(gabi::at<dCcD_Cyl>(a))->mShapeVtable = FIRE_SHAPE_BASE_VTABLE;
    fireCylinderVtables(gabi::at<dCcD_Cyl>(a))->mShapeBaseVtable = FIRE_CYL_SHAPE_VTABLE;
    fireCylinderConstruct(a + 0x118);
    gabi::at<dCcD_Cyl>(a)->__vtbl_hitinf = FIRE_CYL_HIT_VTABLE;
    gabi::at<dCcD_Cyl>(a)->mCyl.__vtbl = FIRE_CYL_VTABLE;
    fireCylinderVtables(gabi::at<dCcD_Cyl>(a))->mShapeVtable = FIRE_CYL_SHAPE_FINAL_VTABLE;
    return a;
}

/* 0213FC80 */
void actorDtor(daFire_c *a, u32 flags) {
    if (a) {
        fireCylinderDestroy(&a->mAtCyl, 2);
        fireCylinderDestroy(&a->mCoCyl, 2);
        fireDestroyArray(gabi::ea(&a->mTgCyls[0]), 3, 0x130, FIRE_CYLINDER_DTOR, 0, 0);
        fireSttsDestroy(&a->mStts, 2);
        fireActorDestroy(a, 0);
        if (flags & 1)
            fireDelete(gabi::ea(a));
    }
}

// clang-format off
/* WWHD glue
str 1000F0F0
str 1000F118
str 1000F100
str 1000F128
str 1000F10C
ext 0281DE68 void fireDeleteAllParticles(u32,s32)
ext 025D5218 u32 fireActorSearch(u32,u32)
ext 0200F5C8 void fireChaseF(be<f32>*,f32,f32)
ext 025B8C74 u32 fireIsTbox(u32,u32)
ext 02515F14 void fireSttsInit(u32,s32,s32,u32)
ext 02516518 void fireCylinderSet(u32,u32)
ext 020184DC void fireCylinderRadius(u32,f32)
ext 025DA884 void fireOffDraw(u32)
ext 025162A4 s32 fireCheckTargetHit(u32)
ext 02516300 u32 fireGetTargetHitObj(u32)
ext 02515BBC u32 fireGetTargetHitActor(u32)
ext 0251621C void fireClearTargetHit(u32)
ext 025A847C u32 fireParticleSet(u32,u32,u32,u32,u32,u32,u32,u32,s32,u32,u32,u32)
ext 028F4384 f32 fireSquareRoot(f32)
ext 0201B47C s32 fireNormalizeRS(cXyz*)
ext 02520540 s32 fireReverb(s32)
ext 025E1A40 void fireSoundStart(u32,const cXyz*,u32,s32)
ext 0273AD10 u32 fireNew(u32)
ext 02515FB8 void fireCollisionConstruct(u32)
ext 02018590 void fireCylinderConstruct(u32)
ext 025D4ED0 void fireActorConstruct(fopAc_ac_c*)
ext 0200BD2C void fireSttsConstruct(u32)
ext 02515DA0 void fireGSttsConstruct(u32)
ext 028EFFD0 void fireConstructArray(u32,u32,u32,u32)
ext 025D98E8 s32 fireSearchRoomEnemy(s32)
ext 0207A9A0 u8 fireCalcTimer(be<u8>*)
ext 025D7A58 s32 fireOrderOtherEvent(fopAc_ac_c*,s32,u32,u32,u32,u32)
ext 020182E0 void fireCylinderCenter(cM3dGCyl*,cXyz*)
ext 0200E240 void fireCollisionSet(u32,u32)
ext 028F026C void fireRegisterStatic(u32)
ext 028ED6F8 void fireInitializeOne(u32)
ext 028EAB2C void fireInitializeZero(u32)
ext 02515A70 void fireCylinderDestroy(dCcD_Cyl*,s32)
ext 028F0164 void fireDestroyArray(u32,u32,u32,u32,u32,u32)
ext 02515860 void fireSttsDestroy(dCcD_Stts*,s32)
ext 025D50BC void fireActorDestroy(fopAc_ac_c*,s32)
ext 0273AF40 void fireDelete(u32)
*/
