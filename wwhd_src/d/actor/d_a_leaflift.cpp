/* Forest Haven lilypad lift. */
#include "d/actor/d_a_leaflift.h"
// HD: keep the linkage area clear for the nonleaf guest callees used by this actor.
template <class T> struct ProtectedLocal {
    struct Frame {
        u8 linkage[16];
        T value;
    };
    gabi::Local<Frame> storage;
    T* get() const {
        return gabi::at<T>(storage.a + offsetof(Frame, value));
    }
    T* operator->() const {
        return get();
    }
    T& operator*() const {
        return *get();
    }
};
struct LeafColor {
    be<u8> r, g, b, a;
};
// HD: fields beyond the shared emitter view, with HD global transform/color offsets.
struct LeafEmitter : JPABaseEmitter {
    u8 _70[0x1F0 - 0x70];
    Mtx34 mGlobalRotation;
    u8 _220[0x22C - 0x220];
    cXyz mGlobalTranslation;
    u8 _238[0x244 - 0x238];
    LeafColor mGlobalPrmColor, mGlobalEnvColor;
    u8 _24C[8];
    be<u32> mStatus;
    void becomeInvalidEmitter() {
        u32 flags = mStatus;
        mMaxFrame = -1;
        mStatus = flags | 1;
    }
    void stopCreateParticle() {
        mStatus = (u32)mStatus | 1;
    }
    void playCreateParticle() {
        mStatus = (u32)mStatus & ~1u;
    }
};
WWHD_OFFSET(LeafEmitter, mStatus, 0x254);
struct LeafBgWorld : dBgW {
    u8 _04[0xA8 - 4];
    be<u32> mCrrFunc;
    u8 _AC[4];
    be<u32> mRideCallback;
};
struct QuatWords {
    be<u32> x, y, z, w;
};
static QuatWords ZeroQuat;
static cXyz up_vec;
static dCcD_SrcCyl l_cyl_src;
static void resetRotation(daLlift_c* a) {
    u32 x = ZeroQuat.x, y = ZeroQuat.y, z = ZeroQuat.z, w = ZeroQuat.w;
    auto* target = reinterpret_cast<QuatWords*>(&a->mTargetRotation);
    target->x = x;
    target->y = y;
    target->z = z;
    target->w = w;
}
static void copyRotation(Quaternion* dst, const Quaternion* src) {
    auto* d = reinterpret_cast<QuatWords*>(dst);
    const auto* s = reinterpret_cast<const QuatWords*>(src);
    d->x = (u32)s->x;
    d->y = (u32)s->y;
    d->z = (u32)s->z;
    d->w = (u32)s->w;
}
struct LeafJointInfo {
    u8 _00[4];
    be<u16> mJntNo;
};
extern void* jointData(J3DNode*);
extern void seStartLevel(u32, cXyz*, u32, s32);
extern void operatorDelete(void*);
extern s32 normalizeRS(cXyz*);
extern f32 sqrtf_guest(f32);
extern f32 addCalc(be<f32>*, f32, f32, f32, f32);
extern f32 waterHeight(cXyz*);
extern void getParticleColors(LeafColor*, LeafColor*);
extern void JPASetRMtxTVecfromMtx(const Mtx34*, Mtx34*, cXyz*);
extern void dBgS_MoveBGProc_TypicalRotY(dBgW*, fopAc_ac_c*, fopAc_ac_c*);
extern void constructHeader0(void*);
extern void constructHeader1(void*);
extern void registerHeader(void*);
static void rideCallBack(dBgW*, daLlift_c*, fopAc_ac_c*);
static s32 nodeCallBack(J3DNode*, s32);
static s32 CheckCreateHeap(fopAc_ac_c*);
static LeafEmitter* particle(u16 id, cXyz* pos, csXyz* angle) {
    return reinterpret_cast<LeafEmitter*>(
        dComIfGp_getParticle()->set(0, id, pos, angle, nullptr, 255, nullptr, -1, nullptr, nullptr, nullptr));
}
static void colorEmitter(gptr<LeafEmitter>* slot, LeafColor* color) {
    LeafEmitter* emitter = *slot;
    if (!emitter)
        return;
    u8 r = color->r, g = color->g, b = color->b;
    emitter->mGlobalPrmColor.r = r;
    emitter->mGlobalPrmColor.g = g;
    emitter->mGlobalPrmColor.b = b;
    emitter = *slot;
    emitter->mGlobalEnvColor.r = r;
    emitter->mGlobalEnvColor.g = g;
    emitter->mGlobalEnvColor.b = b;
}
extern void* getObjectRes(dRes_control_c*, SafeString*, s32);
extern s32 SetBackground(dBgW*, cBgD_t*, u32, Mtx34*);
static gabi::HostFn typicalRotYBinding(reinterpret_cast<const void*>(&dBgS_MoveBGProc_TypicalRotY), 0x024EE708);
/* 021B3260 */
s32 daLlift_c::CreateHeap() {
    ProtectedLocal<SafeString> name;
    name->mStringTop = gabi::guest_addr("Olift");
    name->__vtbl = 0x10013E78;
    auto* data = static_cast<J3DModelData*>(getObjectRes(dComIfG_resControl(), name.get(), 4));
    if (!data)
        JUT_ASSERT_fail("d_a_leaflift.cpp", 0x14E, "modelData != NULL");
    J3DModel* model = mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
    mpModel = model;
    if (!model)
        return FALSE;
    model->mUserArea = gabi::ea(this);
    mpBgW = static_cast<LeafBgWorld*>(new_dBgW());
    if (!mpBgW)
        return FALSE;
    ProtectedLocal<SafeString> bgname;
    bgname->__vtbl = 0x10013E78;
    bgname->mStringTop = gabi::guest_addr("Olift");
    auto* collision = static_cast<cBgD_t*>(getObjectRes(dComIfG_resControl(), bgname.get(), 7));
    LeafBgWorld* bg = mpBgW;
    if (SetBackground(bg, collision, dBgW::MOVE_BG_e, &mMtx))
        return FALSE;
    bg = mpBgW;
    bg->mCrrFunc = gabi::guest_fn(reinterpret_cast<const void*>(&dBgS_MoveBGProc_TypicalRotY));
    bg = mpBgW;
    bg->mRideCallback = gabi::guest_fn(reinterpret_cast<const void*>(&rideCallBack));
    return TRUE;
}
/* 021B34AC */
void daLlift_c::setMoveBGMtx() {
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    mDoMtx_stack_c::transS(x, y, z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    mDoMtx_stack_c::quatM(&mCurrentRotation);
    MTXCopy(mDoMtx_stack_c::get(), &mMtx);
}
/* 021B3514 */
void daLlift_c::set_mtx() {
    f32 x = scale.x, y = scale.y, z = scale.z;
    J3DModel* model = mpModel;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    x = current.pos.x;
    y = current.pos.y;
    z = current.pos.z;
    mDoMtx_stack_c::transS(x, y, z);
    mDoMtx_stack_c::ZXYrotM(current.angle);
    model = mpModel;
    model->setBaseTRMtx(mDoMtx_stack_c::get());
}
/* 021B35F4 */
void daLlift_c::CreateInit() {
    J3DModel* model = mpModel;
    fopAcM_SetMtx(this, model->getBaseTRMtx());
    fopAcM_setCullSizeBox(this, -300.f, -600.f, -300.f, 300.f, 100.f, 300.f);
    fopAcM_setCullSizeFar(this, 1.f);
    mStts.Init(255, 255, this);
    mCyl.Set(&l_cyl_src);
    ProtectedLocal<cXyz> water, particlePosition;
    water->x = current.pos.x;
    water->z = current.pos.z;
    water->y = f32(current.pos.y) + 200.f;
    mCyl.SetStts(&mStts);
    f32 height = waterHeight(water.get());
    mWaterY = height;
    if (height != -1000000000.f) {
        particlePosition->x = current.pos.x;
        particlePosition->z = current.pos.z;
        particlePosition->y = height + 1.f;
        LeafEmitter* emitter = particle(0x82AA, particlePosition.get(), &current.angle);
        mEmitter3 = emitter;
        if (emitter)
            emitter->stopCreateParticle();
    }
    f32 y = f32(home.pos.y) + 15.f;
    model = mpModel;
    home.pos.y = y;
    current.pos.y = y;
    J3DModelData* data = model->getModelData();
    JUTNameTab* stringTable = data->getJointName();
    s32 index = stringTable->getIndex("j_happa");
    if (index >= 0) {
        model = mpModel;
        data = model->getModelData();
        data->getJointNodePointer(u16(index))->setCallBack(nodeCallBack);
    }
    resetRotation(this);
    copyRotation(&mCurrentRotation, &mTargetRotation);
    if ((u32(mParameters) & 15) == 1)
        current.pos.y = f32(home.pos.y) + 560.f;
    else {
        LeafEmitter* emitter = mEmitter3;
        if (emitter)
            emitter->playCreateParticle();
    }
    model = mpModel;
    m43F = 30;
    model->calc();
    setMoveBGMtx();
    dComIfG_Bgsp()->Regist(mpBgW, this);
    set_mtx();
    mpBgW->Move();
}
/* 021B38D0 */
s32 daLlift_c::_create() {
    if (!(u32(actor_condition) & fopAcCnd_INIT_e)) {
        fopAc_ac_c_ct(this);
        __vtbl = 0x10013EA0;
        dCcD_Stts_ct(&mStts);
        dCcD_Cyl_ct(&mCyl, 0x10013E90);
        actor_condition = u32(actor_condition) | fopAcCnd_INIT_e;
    }
    s32 phase = dComIfG_resLoad(&mPhs, "Olift");
    if (phase == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, 0xF40))
            return cPhs_ERROR_e;
        CreateInit();
    }
    return phase;
}
/* 021B3A0C */
s32 daLlift_c::_delete() {
    LeafEmitter* emitter = mEmitter1;
    if (emitter) {
        emitter->becomeInvalidEmitter();
        mEmitter1 = nullptr;
    }
    emitter = mEmitter2;
    if (emitter) {
        emitter->becomeInvalidEmitter();
        mEmitter2 = nullptr;
    }
    emitter = mEmitter3;
    if (emitter) {
        emitter->becomeInvalidEmitter();
        mEmitter3 = nullptr;
    }
    if (heap)
        dComIfG_Bgsp()->Release(mpBgW);
    dComIfG_resDelete(&mPhs, "Olift");
    return TRUE;
}
/* 021B3AD8 */
s32 daLlift_c::_draw() {
    dScnKy_env_light_c* env = dKy_getEnvlight();
    settingTevStruct(env, 1, &current.pos, &tevStr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelEntryDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}
/* 021B3B70 */
s32 daLlift_c::MoveDownLift() {
    if (!mbIsDescending)
        return 1;
    f32 delta = addCalc(&current.pos.y, f32(home.pos.y), 0.1f, 10.f, 5.f);
    if (delta == 0.f) {
        LeafEmitter* e = mEmitter3;
        mbIsAscending = 0;
        mbIsDescending = 0;
        if (e)
            e->playCreateParticle();
        return 1;
    }
    if (!mbIsAscending) {
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        seStartLevel(0x6980, &eyePos, 0, reverb);
        mbIsAscending = 1;
    }
    return 0;
}
/* 021B3C4C */
void daLlift_c::emitterCtrl() {
    if (s32(mEmitterTimer) == 200) {
        LeafEmitter* emitter = mEmitter1;
        if (emitter) {
            emitter->becomeInvalidEmitter();
            mEmitter1 = nullptr;
        }
        emitter = mEmitter2;
        if (emitter) {
            emitter->becomeInvalidEmitter();
            mEmitter2 = nullptr;
        }
    }
    LeafEmitter* emitter = mEmitter1;
    if (emitter)
        JPASetRMtxTVecfromMtx(&mBGMtx, &emitter->mGlobalRotation, &emitter->mGlobalTranslation);
    emitter = mEmitter2;
    if (emitter)
        JPASetRMtxTVecfromMtx(&mBGMtx, &emitter->mGlobalRotation, &emitter->mGlobalTranslation);
}
/* 021B3D04 */
s32 daLlift_c::_execute() {
    f32 distance = fopAcM_searchActorDistanceXZ(this, dComIfGp_getPlayer(0));
    s16 life = s16(s16(mActorLifetimeFrameCount) + 1);
    u8 timer = m43F, ridden = m43C;
    mActorLifetimeFrameCount = life;
    if (timer) {
        timer--;
        m43F = timer;
        if (!timer)
            m43E = 1;
    }
    if (!ridden && m43E) {
        resetRotation(this);
        if (distance > 270.f)
            mbIsDescending = 1;
    }
    ProtectedLocal<LeafColor> color, secondary;
    getParticleColors(color.get(), secondary.get());
    colorEmitter(&mEmitter3, color.get());
    colorEmitter(&mEmitter4, color.get());
    colorEmitter(&mEmitter1, color.get());
    MoveDownLift();
    set_mtx();
    setMoveBGMtx();
    J3DModel* model = mpModel;
    m43C = 0;
    model->calc();
    ProtectedLocal<Quaternion> interpolatedRotation;
    mDoMtx_quatSlerp(&mCurrentRotation, &mTargetRotation, interpolatedRotation.get(), 0.25f);
    copyRotation(&mCurrentRotation, interpolatedRotation.get());
    mpBgW->Move();
    ProtectedLocal<cXyz> pos;
    pos->x = current.pos.x;
    pos->y = f32(current.pos.y) - 560.f;
    pos->z = current.pos.z;
    mCyl.SetC(pos.get());
    cCcS_Set(dComIfG_Ccsp(), &mCyl);
    emitterCtrl();
    return 1;
}
/* 021B4090 */
s32 daLlift_c::MoveUpLift() {
    f32 target = f32(home.pos.y) + 560.f, y = current.pos.y;
    mEmitterTimer = s32(u32(mEmitterTimer) + 1);
    if (y != target) {
        target = f32(home.pos.y) + 560.f;
        m43D = 1;
    }
    f32 delta = addCalc(&current.pos.y, target, 0.1f, 10.f, 5.f);
    s32 result = 0;
    ProtectedLocal<LeafColor> color, secondary;
    if (delta == 0.f) {
        mbIsAscending = 0;
        result = 1;
    } else if (!mbIsAscending) {
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        seStartLevel(0x697F, &eyePos, 0, reverb);
        mbIsAscending = 1;
        mEmitter1 = particle(0x82AC, &current.pos, &current.angle);
        ProtectedLocal<cXyz> pos;
        pos->z = current.pos.z;
        pos->x = current.pos.x;
        pos->y = mWaterY;
        mEmitter2 = nullptr;
        mEmitter4 = particle(0x82AB, pos.get(), &current.angle);
        LeafEmitter* e = mEmitter3;
        mEmitterTimer = 0;
        if (e)
            e->stopCreateParticle();
    }
    getParticleColors(color.get(), secondary.get());
    colorEmitter(&mEmitter3, color.get());
    colorEmitter(&mEmitter4, color.get());
    colorEmitter(&mEmitter1, color.get());
    return result;
}
/* 021B430C */
s32 daLlift_c::checkEndDownLift() {
    return !(f32(current.pos.y) > f32(home.pos.y));
}
/* 021B3028 */
static void rideCallBack(dBgW* bg, daLlift_c* a, fopAc_ac_c* other) {

    ProtectedLocal<cXyz> offset, cross;
    *offset = other->current.pos - a->current.pos;
    f32 factor = 2.f;
    if (other && fopAcM_GetName(other) == 0xA8) {
        a->m43C = 1;
        a->m43E = 1;
        *cross = offset->outprod(up_vec);
        offset->copy(*cross);
        mDoMtx_stack_c::YrotS(s16(-s16(a->current.angle.y)));
        mDoMtx_stack_c::multVec(offset.get(), offset.get());
        if (a->m43D) {
            offset->x = f32(other->current.pos.x) + 200.f;
            offset->y = other->current.pos.y;
            offset->z = other->current.pos.z;
            *cross = offset->outprod(up_vec);
            offset->copy(*cross);
            mDoMtx_stack_c::YrotS(s16(-s16(a->current.angle.y)));
            mDoMtx_stack_c::multVec(offset.get(), offset.get());
            u16 angle = u16(u16(a->mActorLifetimeFrameCount) << 11);
            factor = (cM_ssin(s16(angle)) + cM_ssin(s16(angle))) * 0.25f;
            f32 random = cM_rndFX(0.2f);
            factor *= random + 1.f;
        }
        f32 sq = PSVECSquareMag(offset.get());
        // HD: round the square root to f32 at this callsite.
        f32 length = sqrtf_guest(sq);
        if (!normalizeRS(offset.get()))
            return;
        s16 target = s16(gabi::ftoi(-(length * factor)));
        cLib_addCalcAngleS2(&a->mTiltAngle, target, 8, 0x200);
        f32 tilt = cM_ssin(a->mTiltAngle);
        a->mTargetRotation.x = f32(offset->x) * tilt;
        a->mTargetRotation.y = f32(offset->y) * tilt;
        a->mTargetRotation.z = f32(offset->z) * tilt;
        a->mTargetRotation.w = cM_scos(a->mTiltAngle);
    }
    a->m43D = 0;
}
/* 021B3380 */
static s32 nodeCallBack(J3DNode* node, s32 timing) {
    if (timing == J3DNodeCBCalcTiming_In) {
        auto* joint = static_cast<LeafJointInfo*>(jointData(node));
        J3DModel* model = j3dSys.getModel();
        auto* actor = static_cast<daLlift_c*>(model->getUserArea());
        u32 index = joint->mJntNo;
        if (actor) {
            Mtx34* matrix = model->getAnmMtx(index);
            MTXCopy(matrix, mDoMtx_stack_c::get());
            mDoMtx_stack_c::quatM(&actor->mCurrentRotation);
            MTXCopy(mDoMtx_stack_c::get(), J3DSys::mCurrentMtx);
            matrix = model->getAnmMtx(index);
            mtx_copy(matrix, mDoMtx_stack_c::get());
            MTXCopy(mDoMtx_stack_c::get(), &actor->mBGMtx);
        }
    }
    return TRUE;
}
/* 021B337C */
static s32 CheckCreateHeap(fopAc_ac_c* a) {
    return static_cast<daLlift_c*>(a)->CreateHeap();
}
/* 021B3A08 */
static s32 daLlift_Create(daLlift_c* a) {
    return a->_create();
}
/* 021B3AD4 */
static s32 daLlift_Delete(daLlift_c* a) {
    return a->_delete();
}
/* 021B3B6C */
static s32 daLlift_Draw(daLlift_c* a) {
    return a->_draw();
}
/* 021B3F4C */
static s32 daLlift_Execute(daLlift_c* a) {
    return a->_execute();
}
/* 021B4018 */
static s32 daLlift_IsDelete(daLlift_c* a) {
    return TRUE;
}
/* 021B4004 */
static void deletingStatic(void* p, s32 flags) {
    if (p && (flags & 1))
        operatorDelete(p);
}
/* 021B4020 */
static void daLlift_destruct(daLlift_c* a, s32 flags) {
    if (a) {
        dCcD_Cyl_dt(&a->mCyl, 2);
        dCcD_Stts_dt(&a->mStts, 2);
        __dt(static_cast<fopAc_ac_c*>(a));
        if (flags & 1)
            operatorDelete(a);
    }
}
/* 021B408C */
static void SafeString_assureTerminationImpl() {}
struct HeaderZeros {
    be<u32> words[4];
};
struct HeaderFloat {
    be<f32> value;
};
struct HeaderByte {
    u8 value;
};
struct HeaderRecord {
    u8 bytes[12];
};
static HeaderZeros zeros;
static HeaderFloat minimum;
static HeaderFloat maximum;
static HeaderByte flag0;
static HeaderByte flag1;
static HeaderRecord record0;
static HeaderRecord record1;
static HeaderRecord record2;
/* 021B3F50 */
static void __sinit_d_a_leaflift_cpp() {
    zeros.words[2] = 0;
    zeros.words[0] = 0;
    zeros.words[3] = 0;
    zeros.words[1] = 0;
    registerHeader(&record0);
    minimum.value = -3.1415927410125732f;
    maximum.value = 3.1415927410125732f;
    constructHeader0(&flag0);
    registerHeader(&record1);
    constructHeader1(&flag1);
    registerHeader(&record2);
    up_vec.x = 0.f;
    up_vec.y = 1.f;
    up_vec.z = 0.f;
}
static HeaderZeros zerosStaticTU;
static HeaderFloat minimumStaticTU;
static HeaderFloat maximumStaticTU;
static HeaderByte flag0StaticTU;
static HeaderByte flag1StaticTU;
static HeaderRecord record0StaticTU;
static HeaderRecord record1StaticTU;
static HeaderRecord record2StaticTU;
/* 021B4328 */
static void __sinit_d_a_leafliftStaticTU_cpp() {
    zerosStaticTU.words[2] = 0;
    zerosStaticTU.words[0] = 0;
    zerosStaticTU.words[3] = 0;
    zerosStaticTU.words[1] = 0;
    registerHeader(&record0StaticTU);
    minimumStaticTU.value = -3.1415927410125732f;
    maximumStaticTU.value = 3.1415927410125732f;
    constructHeader0(&flag0StaticTU);
    registerHeader(&record1StaticTU);
    constructHeader1(&flag1StaticTU);
    registerHeader(&record2StaticTU);
}

/* WWHD glue
data ZeroQuat 101E9C38
data up_vec 10464E0C
data l_cyl_src 101B8F90
data zeros 10464DFC
data minimum 10464DF0
data maximum 10464DF4
data flag0 10464DF8
data flag1 10464DF9
data record0 101B8FD4
data record1 101B8FE0
data record2 101B8FEC
data zerosStaticTU 10464E24
data minimumStaticTU 10464E18
data maximumStaticTU 10464E1C
data flag0StaticTU 10464E20
data flag1StaticTU 10464E21
data record0StaticTU 101B9028
data record1StaticTU 101B9034
data record2StaticTU 101B9040
str 10013F20 "Olift"
str 10013EC0 "d_a_leaflift.cpp"
str 10013ED4 "modelData != NULL"
str 10013F00 "j_happa"
ext 0273AF40 void operatorDelete(void*)
ext 0201B47C s32 normalizeRS(cXyz*)
ext 028F4384 f32 sqrtf_guest(f32)
ext 0200ECD4 f32 addCalc(be<f32>*, f32, f32, f32, f32)
ext 024F1478 f32 waterHeight(cXyz*)
ext 025602F0 void getParticleColors(LeafColor*, LeafColor*)
ext 028249B0 void JPASetRMtxTVecfromMtx(const Mtx34*, Mtx34*, cXyz*)
ext 024EE708 void dBgS_MoveBGProc_TypicalRotY(dBgW*, fopAc_ac_c*, fopAc_ac_c*)
ext 028ED6F8 void constructHeader0(void*)
ext 028EAB2C void constructHeader1(void*)
ext 028F026C void registerHeader(void*)
ext 026066C4 void* getObjectRes(dRes_control_c*, SafeString*, s32)
ext 0200A030 s32 SetBackground(dBgW*, cBgD_t*, u32, Mtx34*)
ext 025E1A40 void seStartLevel(u32, cXyz*, u32, s32)
ext 027F7878 void* jointData(J3DNode*)
*/
