// HD: the verified temporary linkage and expanded collision layout are retained.
#include "d/actor/d_a_windmill.h"

static u32 m_arcname[2] = {};
static s16 m_bmdidx[2] = {};
static s16 m_dzbidx[2] = {};
static s16 m_heapsize[2] = {};
static cXyz m_cull_size[2][2] = {};
static dCcD_SrcSph l_sph_src;
static dCcD_SrcCps l_cps_src;
static dCcD_SrcCyl l_cyl_src;
static f32 windData[32] = {};
static Mtx34 matrixStack;
static u32 drawOpaque;
static u32 drawTranslucent;
static WindText assertFile;
static WindText modelAssert;
static WindText typeAssertFile;
static WindText typeAssert;
static WindStaticState staticState;
static WindRegistration staticRegistration0;
static WindRegistration staticRegistration1;
static WindRegistration staticRegistration2;
static f32 minimumPi;
static f32 maximumPi;
static u8 staticChar;
static u8 staticByte;
static Mtx34 *matrix() { return &matrixStack; }
static u32 archive(u8 type) { return m_arcname[type]; }
static void translate(daWindMill_c *a) {
    windCall_translate<void>(matrix(), (f32)a->current.pos.x, (f32)a->current.pos.y,
                             (f32)a->current.pos.z);
}
static void rotate(daWindMill_c *a, bool blade) {
    s16 x = a->current.angle.x, y = a->current.angle.y, z = a->current.angle.z;
    if (blade) {
        s16 sum = (s16)((s16)a->mAngle[0] + (s16)a->mAngle[1]);
        if ((u8)a->mType == daWindMill_c::LARGE)
            y = sum;
        else
            z = sum;
    }
    windCall_rotateZXY<void>(matrix(), x, y, z);
}
static void multiply(cXyz *v) { windCall_multiply<void>(matrix(), v, v); }
static void register_collision(void *obj) {
    void *play = windCall_getPlay<void *>();
    windCall_registerCollision<void>(windCollisionSpace(play), obj);
}

/* 024E0BA0 */
BOOL daWindMill_c::CreateHeap() {
    auto *a = this;
    wind::Local<be<u32>[2]> name;
    (*name)[1] = windAbi::SafeStringVtable;
    u8 type = a->mType;
    void *controller = windmillResourceController();
    s16 index = m_bmdidx[type];
    (*name)[0] = archive(type);
    void *data = windCall_getResource<void *>(controller, name.get(), index);
    if (!data)
        windCall_assertFail<void>(assertFile.bytes, 405, modelAssert.bytes);
    J3DModel *model = windCall_createModel<J3DModel *>(data, 0x80000u, 0x11000222u);
    a->mpModel = model;
    if (!model)
        return 0;
    type = a->mType;
    index = m_dzbidx[type];
    if (index == -1)
        return 1;
    void *bg = windCall_constructBg<void *>((void *)nullptr);
    a->mpBgW = static_cast<dBgW *>(bg);
    if (!bg)
        return 0;
    wind::Local<be<u32>[2]> bgName;
    type = a->mType;
    (*bgName)[1] = windAbi::SafeStringVtable;
    controller = windmillResourceController();
    (*bgName)[0] = archive(type);
    index = m_dzbidx[type];
    void *res = windCall_getResource<void *>(controller, bgName.get(), index);
    bg = a->mpBgW;
    if (windCall_setBg<s32>(bg, res, 1, &a->mMtx) != 0)
        return 0;
    bg = a->mpBgW;
    reinterpret_cast<WindBgView *>(bg)->mCrrFunc = windAbi::TypicalRotY;
    return 1;
}
/* 024E0CF0 */
BOOL windmill_heap_thunk(daWindMill_c *a) { return a->CreateHeap(); }

/* 024E0CF4 */
BOOL windmill_node_callback(void *node, s32 timing) {
    if (timing != 0)
        return 1;
    void *joint = windCall_jointData<void *>(node);
    J3DModel *model = j3dSys.getModel();
    daWindMill_c *a = gabi::at<daWindMill_c>(model->mUserArea);
    u16 jointNo = reinterpret_cast<WindJointView *>(joint)->mJointNo;
    if (!a)
        return 1;
    a->mAngle[0] = (s16)((s16)a->mAngle[0] + (s16)a->mAngle[1]);
    J3DMtxBuffer_hd *buffer = model->mpMtxBuffer;
    u32 matrices = (u32)buffer->mpAnmMtx;
    u16 flags = (u16)buffer->mFlags;
    buffer->mFlags = flags | 0x10;
    windCall_copyMatrix<void>(gabi::at<Mtx34>(matrices + jointNo * 48u), matrix());
    u8 type = a->mType;
    if (type == daWindMill_c::LARGE)
        windCall_rotateY<void>(matrix(), (s16)a->mAngle[0]);
    else if (type == daWindMill_c::SMALL)
        windCall_rotateZ<void>(matrix(), (s16)a->mAngle[0]);
    buffer = model->mpMtxBuffer;
    flags = (u16)buffer->mFlags;
    matrices = (u32)buffer->mpAnmMtx;
    buffer->mFlags = flags | 0x10;
    mtx_copy(gabi::at<Mtx34>(matrices + jointNo * 48u), matrix());
    windCall_copyMatrix<void>(matrix(), J3DSys::mCurrentMtx);
    windCall_copyMatrix<void>(matrix(), &a->mMtx);
    a->shape_angle.y = a->mAngle[0];
    return 1;
}

/* 024E0F8C */
void daWindMill_c::set_mtx() {
    auto *a = this;
    f32 x = a->scale.x, y = a->scale.y, z = a->scale.z;
    J3DModel *model = a->mpModel;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    translate(a);
    rotate(a, false);
    mtx_copy(&a->mpModel->mBaseTransformMtx, matrix());
}
/* 024E106C */
void daWindMill_c::search_wind() {
    auto *a = this;
    wind::Local<be<s16>> profile;
    *profile = windAbi::WindTagProfile;
    void *wind = windCall_search<void *>(windGuestFunction(windAbi::SearchByName), profile.get());
    a->mWindTagId = wind ? reinterpret_cast<fopAc_ac_c *>(wind)->mBsPcId.get() : 0xFFFFFFFFu;
}
/* 024E10C8 */
void daWindMill_c::CreateInit() {
    auto *a = this;
    J3DModel *model = a->mpModel;
    u8 type = a->mType;
    a->cullMtx = model ? gabi::ea(&model->mBaseTransformMtx) : 0;
    cXyz *cull = m_cull_size[type];
    f32 loX = cull[0].x, loY = cull[0].y, loZ = cull[0].z;
    f32 hiX = cull[1].x, hiY = cull[1].y, hiZ = cull[1].z;
    windCall_setCullBox<void>(a, loX, loY, loZ, hiX, hiY, hiZ);
    a->cullSizeFar = 3.f;
    windCall_initStts<void>(&a->mStts, 255, 255, a);
    type = a->mType;
    if (type <= daWindMill_c::SMALL) {
        for (int i = 0; i < 4; i++) {
            windCall_setCapsule<void>(&a->mCps[i], &l_cps_src);
            a->mCps[i].mStts = &a->mStts;
        }
        if (type == daWindMill_c::SMALL) {
            for (int i = 0; i < 9; i++) {
                windCall_setSphere<void>(&a->mSph[i], &l_sph_src);
                a->mSph[i].mStts = &a->mStts;
                if (i < 4)
                    windCall_setSphereRadius<void>(&a->mSph[i].mSph, 60.f);
            }
        } else {
            windCall_setCylinder<void>(&a->mCyl, &l_cyl_src);
            a->mCyl.mStts = &a->mStts;
        }
    }
    model = a->mpModel;
    model->mUserArea = gabi::ea(a);
    a->set_mtx();
    model = a->mpModel;
    void *joints = windCall_jointTree<void *>(model->mModelData.get());
    u16 count = reinterpret_cast<WindJointTree *>(joints)->mJointNum;
    for (u16 i = 0; i < count;) {
        if (i == 2) {
            model = a->mpModel;
            J3DModelData *data = model->mModelData;
            u32 size = data->mJointNum, joint = data->mJointNodes;
            if (size > 2)
                joint += 0x38;
            gabi::at<J3DJoint>(joint)->mCallBack = windAbi::NodeCallback;
            break;
        }
        model = a->mpModel;
        i++;
        joints = windCall_jointTree<void *>(model->mModelData.get());
        count = reinterpret_cast<WindJointTree *>(joints)->mJointNum;
    }
    windCall_calcModel<void>((J3DModel *)a->mpModel);
    a->search_wind();
    if (a->mpBgW) {
        translate(a);
        windCall_rotateY<void>(matrix(), (s16)a->current.angle.y);
        windCall_copyMatrix<void>(matrix(), &a->mMtx);
        void *play = windCall_getPlay<void *>();
        windCall_registerBg<void>(windBackgroundSpace(play), (void *)a->mpBgW, a);
        windCall_moveBg<void>((void *)a->mpBgW);
    }
}
/* 024E1370 */
s32 daWindMill_c::_create() {
    auto *a = this;
    if (!((u32)a->actor_condition & 8)) {
        if (gabi::ea(a) != 0) {
            windCall_constructActor<void>(a);
            a->__vtbl = windAbi::ActorVtable;
            windCall_constructStts<void>(&a->mStts);
            windCall_constructGStts<void>(&a->mStts.__vtbl_gstts);
            a->mStts.__vtbl = windAbi::SttsVtable;
            a->mStts.__vtbl_gstts = windAbi::GSttsVtable;
            windCall_constructArray<void>(a->mSph, 9, 0x12C,
                                          windGuestFunction(windAbi::SphereConstructor));
            windCall_constructArray<void>(a->mCps, 4, 0x138,
                                          windGuestFunction(windAbi::CapsuleConstructor));
            windCall_constructCollision<void>(&a->mCyl);
            reinterpret_cast<WindCylView *>(&a->mCyl)->mShapeVtable = windAbi::V100015A8;
            reinterpret_cast<WindCylView *>(&a->mCyl)->mAabVtable = windAbi::V10042C0C;
            windCall_constructCylinder<void>(&a->mCyl.mCyl);
            u32 condition = (u32)a->actor_condition;
            reinterpret_cast<WindCylView *>(&a->mCyl)->mShapeVtable = windAbi::V1004B160;
            reinterpret_cast<WindCylView *>(&a->mCyl)->__vtbl_hitinf = windAbi::V1004B108;
            reinterpret_cast<WindCylView *>(&a->mCyl)->mCyl.__vtbl = windAbi::V1004B150;
            a->actor_condition = condition | 8;
        } else
            a->actor_condition = (u32)a->actor_condition | 8;
    }
    u8 type = (u32)a->mParameters & 15;
    a->mType = type;
    if (type >= daWindMill_c::NUM_TYPES) {
        windCall_assertFail<void>(typeAssertFile.bytes, 615, typeAssert.bytes);
        type = a->mType;
    }
    s32 phase = windCall_loadResource<s32>(&a->mPhs, STR(archive(type)));
    if (phase == cPhs_COMPLEATE_e) {
        type = a->mType;
        s16 size = m_heapsize[type];
        if (!windCall_enterHeap<BOOL>(a, windGuestFunction(windAbi::HeapCallback), size))
            return cPhs_ERROR_e;
        a->CreateInit();
    }
    return phase;
}
/* 024E1518 */
s32 windmill_create_thunk(daWindMill_c *a) { return a->_create(); }
/* 024E151C */
BOOL daWindMill_c::_delete() {
    auto *a = this;
    if (gabi::ea(a->heap.get()) && a->mpBgW) {
        void *play = windCall_getPlay<void *>();
        windCall_releaseBg<void>(windBackgroundSpace(play), (void *)a->mpBgW);
    }
    windCall_deleteResource<void>(&a->mPhs, STR(archive(a->mType)));
    return 1;
}
/* 024E158C */
BOOL windmill_delete_thunk(daWindMill_c *a) { return a->_delete(); }
/* 024E1590 */
BOOL daWindMill_c::_draw() {
    auto *a = this;
    void *light = windCall_envLight<void *>();
    windCall_setTev<void>(light, 0, &a->current.pos, &a->tevStr);
    light = windCall_envLight<void *>();
    windCall_setLight<void>(light, (J3DModel *)a->mpModel, &a->tevStr);
    u8 type = a->mType;
    if (type == daWindMill_c::LARGE) {
        void *play = windCall_getPlay<void *>();
        drawOpaque = reinterpret_cast<WindPlayView *>(play)->mBgOpaque;
        play = windCall_getPlay<void *>();
        drawTranslucent = reinterpret_cast<WindPlayView *>(play)->mBgTranslucent;
        windCall_entryModel<void>((J3DModel *)a->mpModel);
        play = windCall_getPlay<void *>();
        drawOpaque = reinterpret_cast<WindPlayView *>(play)->mOpaque;
        play = windCall_getPlay<void *>();
        drawTranslucent = reinterpret_cast<WindPlayView *>(play)->mTranslucent;
    } else if (type == daWindMill_c::SMALL)
        windCall_updateModel<void>((J3DModel *)a->mpModel, 0);
    return 1;
}
/* 024E1650 */
BOOL windmill_draw_thunk(daWindMill_c *a) { return a->_draw(); }
/* 024E1654 */
void daWindMill_c::hane_move() {
    auto *a = this;
    f32 wind = 0;
    a->search_wind();
    u32 id = a->mWindTagId;
    if (id != 0xFFFFFFFFu) {
        wind::Local<be<u32>> searchId;
        *searchId = id;
        void *tag = windCall_search<void *>(windGuestFunction(windAbi::SearchById), searchId.get());
        if (tag) {
            auto *windTag = static_cast<WindTagView *>(tag);
            u8 type = windTag->mType;
            f32 scale = windTag->field_0x49c;
            f32 length = windData[2u * type] * scale;
            wind = windTag->mOffsY / length;
        }
    }
    s16 target = (s16)gabi::ftoi(wind * 2500.f);
    windCall_addCalcAngle<void>(&a->mAngle[1], target, 15, 100, 10);
    s16 speed = a->mAngle[1], previous = a->mAngle[2];
    if (previous <= speed && speed != 0) {
        u8 type = a->mType;
        if (type <= daWindMill_c::SMALL) {
            s8 room = (s8)a->current.roomNo;
            s32 reverb = windCall_getReverb<s32>(room);
            windCall_startSound<void>(type == daWindMill_c::LARGE ? windAbi::LargeWindSound
                                                                  : windAbi::SmallWindSound,
                                      &a->eyePos, 0, reverb);
        }
    }
}
/* 024E179C */
void daWindMill_c::set_at() {
    auto *a = this;
    wind::Local<cXyz[5]> small;
    (*small)[0].set(0, 0, 70);
    (*small)[1].set(450, 0, 70);
    (*small)[2].set(-450, 0, 70);
    (*small)[3].set(0, 450, 70);
    (*small)[4].set(0, -450, 70);
    wind::Local<cXyz[4]> start, end;
    (*start)[0].set(-1400, 290, 130);
    (*start)[1].set(-1400, 290, -130);
    (*start)[2].set(130, 290, -1400);
    (*start)[3].set(-130, 290, -1400);
    (*end)[0].set(1400, 290, 130);
    (*end)[1].set(1400, 290, -130);
    (*end)[2].set(130, 290, 1400);
    (*end)[3].set(-130, 290, 1400);
    u8 type = a->mType;
    if (type > daWindMill_c::SMALL || (s16)a->mAngle[1] <= 1000)
        return;
    translate(a);
    s16 sum = (s16)((s16)a->mAngle[0] + (s16)a->mAngle[1]);
    s16 x = a->current.angle.x, y, z;
    if (type == daWindMill_c::SMALL) {
        y = a->current.angle.y;
        z = sum;
    } else {
        y = sum;
        z = a->current.angle.z;
    }
    windCall_rotateZXY<void>(matrix(), x, y, z);
    if (type == daWindMill_c::SMALL)
        multiply(&(*small)[0]);
    for (int i = 0; i < 4; i++) {
        WindmillCapsuleData *data = &a->m1244[i];
        if (type == daWindMill_c::SMALL) {
            multiply(&(*small)[i + 1]);
            data->mStart.copy((*small)[0]);
            data->mEnd.copy((*small)[i + 1]);
            data->mRadius = 70.f;
        } else {
            multiply(&(*start)[i]);
            multiply(&(*end)[i]);
            data->mStart.copy((*start)[i]);
            data->mEnd.copy((*end)[i]);
            data->mRadius = 170.f;
        }
        windCall_setCapsuleShape<void>(a->mCps[i].mCps, data);
        if (type == daWindMill_c::LARGE)
            a->mCps[i].mGObjAt.mSpl = 10;
    }
    for (int i = 0; i < 4; i++)
        register_collision(&a->mCps[i]);
}
/* 024E1ADC */
void daWindMill_c::set_co() {
    auto *a = this;
    wind::Local<cXyz[9]> points;
    (*points)[0].set(150, 0, 70);
    (*points)[1].set(-150, 0, 70);
    (*points)[2].set(0, 150, 70);
    (*points)[3].set(0, -150, 70);
    (*points)[4].set(0, 0, 70);
    (*points)[5].set(350, 0, 70);
    (*points)[6].set(-350, 0, 70);
    (*points)[7].set(0, 350, 70);
    (*points)[8].set(0, -350, 70);
    u8 type = a->mType;
    if (type == daWindMill_c::LARGE) {
        windCall_setCylinderCenter<void>(&a->mCyl.mCyl, &a->current.pos);
        register_collision(&a->mCyl);
    } else if (type == daWindMill_c::SMALL && (s16)a->mAngle[1] <= 1000) {
        translate(a);
        s16 sum = (s16)((s16)a->mAngle[0] + (s16)a->mAngle[1]);
        windCall_rotateZXY<void>(matrix(), (s16)a->current.angle.x, (s16)a->current.angle.y, sum);
        for (int i = 0; i < 9; i++)
            multiply(&(*points)[i]);
        for (int i = 0; i < 9; i++) {
            windCall_setSphereCenter<void>(&a->mSph[i].mSph, &(*points)[i]);
            register_collision(&a->mSph[i]);
        }
    }
}
/* 024E1C84 */
BOOL daWindMill_c::_execute() {
    auto *a = this;
    a->hane_move();
    a->set_at();
    a->set_co();
    a->set_mtx();
    if ((u8)a->mType == daWindMill_c::LARGE)
        windCall_calcModel<void>((J3DModel *)a->mpModel);
    if (a->mpBgW)
        windCall_moveBg<void>((void *)a->mpBgW);
    a->mAngle[2] = a->mAngle[1];
    return 1;
}
/* 024E1CF8 */
BOOL windmill_execute_thunk(daWindMill_c *a) { return a->_execute(); }
/* 024E1CFC */
void windmill_static_init() {
    staticState.words[2] = 0;
    staticState.words[0] = 0;
    staticState.words[3] = 0;
    staticState.words[1] = 0;
    windCall_registerStatic<void>(&staticRegistration0);
    minimumPi = -3.1415927410125732f;
    maximumPi = 3.1415927410125732f;
    windCall_initStaticChar<void>(&staticChar);
    windCall_registerStatic<void>(&staticRegistration1);
    windCall_initStaticByte<void>(&staticByte);
    windCall_registerStatic<void>(&staticRegistration2);
}
/* 024E1D90 */
void windmill_local_delete(void *obj, u32 flags) {
    if (obj && (flags & 1))
        windCall_free<void>(obj);
}
/* 024E1DA4 */
dCcD_Cps *windmill_capsule_ctor(dCcD_Cps *obj) {
    if (!obj)
        obj = windCall_allocate<dCcD_Cps *>(0x138);
    if (!obj)
        return obj;
    windCall_constructCollision<void>(obj);
    reinterpret_cast<WindCapsuleView *>(obj)->mShapeVtable = windAbi::V100015A8;
    reinterpret_cast<WindCapsuleView *>(obj)->mAabVtable = windAbi::V10042C0C;
    windCall_constructCapsule<void>(&reinterpret_cast<WindCapsuleView *>(obj)->mCps);
    reinterpret_cast<WindCapsuleView *>(obj)->__vtbl_hitinf = windAbi::V1004AF18;
    reinterpret_cast<WindCapsuleView *>(obj)->mCps.__vtbl = windAbi::V1004AF60;
    reinterpret_cast<WindCapsuleView *>(obj)->mShapeVtable = windAbi::V1004AF70;
    return obj;
}
/* 024E1E30 */
BOOL windmill_is_delete(void *obj) { return 1; }
/* 024E1E38 */
void windmill_destructor(daWindMill_c *a, u32 flags) {
    if (!a)
        return;
    windCall_destroyCylinder<void>(&a->mCyl, 2);
    windCall_destroyArray<void>(a->mCps, 4, 0x138, windGuestFunction(windAbi::CapsuleDestructor), 0,
                                0);
    windCall_destroyArray<void>(a->mSph, 9, 0x12C, windGuestFunction(windAbi::SphereDestructor), 0,
                                0);
    windCall_destroyStts<void>(&a->mStts, 2);
    windCall_destroyActor<void>(a, 0);
    if (flags & 1)
        windCall_free<void>(a);
}
/* 024E1EE4 */
void windmill_empty(void *obj) {}

/* WWHD glue (tools/verify/autoglue.py)
data m_arcname 101D355C nocheck
data m_bmdidx 10042CD8 nocheck
data m_dzbidx 10042CDC nocheck
data m_heapsize 10042CE0 nocheck
data m_cull_size 10042CE4 nocheck
data l_sph_src 101D3448 nocheck
data l_cps_src 101D34A8 nocheck
data l_cyl_src 101D34F4 nocheck
data windData 1004BD40 nocheck
data matrixStack 1048D0CC nocheck
data drawOpaque 104B4634 nocheck
data drawTranslucent 104B4638 nocheck
data assertFile 10042C2C nocheck
data modelAssert 10042C40 nocheck
data typeAssertFile 10042C74 nocheck
data typeAssert 10042C5C nocheck
data staticState 1046EC0C nocheck
data staticRegistration0 101D3538 nocheck
data staticRegistration1 101D3544 nocheck
data staticRegistration2 101D3550 nocheck
data minimumPi 1046EC00 nocheck
data maximumPi 1046EC04 nocheck
data staticChar 1046EC08 nocheck
data staticByte 1046EC09 nocheck
*/
