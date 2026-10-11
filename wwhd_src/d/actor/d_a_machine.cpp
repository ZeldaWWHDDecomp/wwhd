// Wind Temple biting blade trap.
#include "d/actor/d_a_machine.h"
const gc_tu_data gc_tu = {.safeString = 0x10014234,
                          .objAcch = {0x10014284, 0x100142A4, 0x10014294},
                          .objAcchDt = {0x10014294, 0x100142A4},
                          .headerStatics = {0x10464E88, 0x101B9248, 0x10464E94}};
enum { dRes_INDEX_HKIKAI1_BDL_HKIKAI1_e = 7, dRes_INDEX_HKIKAI1_BCK_HKIKAI1_e = 4, fpcNm_WINDMILL_e = 0x72 };
enum { cCcD_AtSPrm_Set_e = 1 };
enum { JA_SE_OBJ_JAMA_MECHA_MOVE = 0x7045, JA_SE_OBJ_JAMA_MECHA_OUT = 0x69C5 };
struct MachineFloat {
    be<f32> mValue;
};
static MachineFloat l_rate;
static MachineFloat l_scale;
static MachineFloat l_cullMin;
static MachineFloat l_cullRadius;
static MachineFloat l_cullBack;
static MachineFloat l_cullFront;
static MachineFloat l_wallRadius;
static MachineFloat l_zero;
static MachineFloat l_speed;
static MachineFloat l_accel;
static MachineFloat l_minStep;
static MachineFloat l_nextDistance;
static MachineFloat l_searchNear;
static MachineFloat l_searchRadius;
static MachineFloat l_playerHeight;
static MachineFloat l_playerRadius;
static MachineFloat l_body0;
static MachineFloat l_body1;
static MachineFloat l_body2;
static MachineFloat l_attackEnd;
struct MachineWindSpeeds {
    be<s16> m_max_rot_speed[4];
};
static MachineWindSpeeds l_windSpeeds;
static dCcD_SrcSph l_sph_src_at;
static dCcD_SrcSph l_sph_src_col;
static cXyz l_origin;
struct MachineVtable {
    be<u32> mEntry;
};
static MachineVtable l_actorVtable;
static MachineVtable l_aabVtable;
static MachineVtable l_bckVtable;
static MachineVtable l_bckBaseVtable;
static MachineVtable l_materialVtable;
static MachineVtable l_shapeVtable;
static MachineVtable l_capsuleVtable;
static MachineVtable l_hitVtable;
static MachineVtable l_cpsShapeVtable;
// HD: private views expose the inline capsule and animation material/vtable construction.
struct MachineCapsule {
    cXyz mStart, mEnd;
    be<u32> __vtbl;
    be<f32> mRadius;
};
struct MachineCps : dCcD_GObjInf {
    u8 _F8[0x18];
    be<u32> mAabVtable, mShapeVtable;
    MachineCapsule mCapsule;
};
struct MachineMaterialTable {
    u8 _00[0x34];
    be<u32> __vtbl;
    u8 _38[0xC];
    be<u32> mAnim;
    u8 _48[0x20];
};
struct MachineBck {
    J3DFrameCtrl mFrameCtrl;
    be<u32> __vtbl;
    MachineMaterialTable mMaterial;
    be<u32> m7C, m80, m84, m88;
};
struct MachineVecWords {
    be<u32> mX, mY, mZ;
};
struct MachineJoint {
    u8 _00[4];
    be<u16> mJntNo;
};
struct MachineJointNames {
    u8 _00[0x10];
    be<u32> mNamesOffset;
};
struct MachineNameTab {
    u8 _opaque;
};
struct MachineJointNode {
    u8 _00[8];
    be<u32> mCallBack;
    u8 _0C[0x10];
};
struct MachineModelData {
    u8 _00[4];
    be<u32> mJointNum;
    gptr<MachineJointNode> mJoints;
};
using MachineNameJudge = void* (*)(void*, void*);
void* Machine_searchName(MachineNameJudge, void*);
void* Machine_nameJudge(void*, void*);
void Machine_chasePos(cXyz*, cXyz*, f32);
s32 Machine_atan2(f32, f32);
f32 Machine_addCalc(be<f32>*, f32, f32, f32, f32);
f32 Machine_sqrt(f32);
void Machine_capsuleSet(MachineCapsule*, cXyz*, cXyz*, f32);
BOOL Machine_capsuleCross(MachineCapsule*, cM3dGSph*, cXyz*);
void Machine_sphCtor(dCcD_Sph*);
void Machine_sphDtor(dCcD_Sph*, s32);
void Machine_gObjCtor(dCcD_GObjInf*);
void Machine_gObjDtor(dCcD_GObjInf*, s32);
void Machine_capsuleCtor(MachineCapsule*);
void Machine_sphereCtor(cM3dGSph*);
void Machine_materialCtor(MachineMaterialTable*);
void Machine_bckTailDtor(be<u32>*, s32);
void Machine_wallRadius(dBgS_AcchCir*, f32);
void Machine_acchSet(dBgS_ObjAcch*, cXyz*, cXyz*, fopAc_ac_c*, s32, dBgS_AcchCir*, cXyz*, void*, void*);
machine_layout::dPath* Machine_roomPath(u32, s32);
MachineJointNames* Machine_jointNames(J3DModelData*);
s32 Machine_findJoint(MachineNameTab*, const char*);
using MachineSphCtor = void (*)(dCcD_Sph*);
using MachineSphDtor = void (*)(dCcD_Sph*, s32);
void Machine_constructArray(dCcD_Sph*, u32, u32, MachineSphCtor);
void Machine_destroyArray(dCcD_Sph*, u32, u32, MachineSphDtor, u32, u32);
static gabi::HostFn l_sphCtorBinding(reinterpret_cast<const void*>(&Machine_sphCtor), 0x025166F0);
static gabi::HostFn l_sphDtorBinding(reinterpret_cast<const void*>(&Machine_sphDtor), 0x02515AE8);
static gabi::HostFn l_nameJudgeBinding(reinterpret_cast<const void*>(&Machine_nameJudge), 0x025E121C);
BOOL CheckCreateHeap(fopAc_ac_c*);
s32 nodeCallBack(J3DNode*, s32);
/* 021B7A3C */
bool daMachine_c::_delete() {
    dComIfG_resDelete(&mPhs, "Hkikai1");
    return true;
}
/* 021B7490 */
BOOL CheckCreateHeap(fopAc_ac_c* actor) {
    return static_cast<daMachine_c*>(actor)->CreateHeap();
}
/* 021B7364 */
BOOL daMachine_c::CreateHeap() {
    auto* modelData = static_cast<J3DModelData*>(dComIfG_getObjectRes("Hkikai1", dRes_INDEX_HKIKAI1_BDL_HKIKAI1_e));
    if (!modelData)
        JUT_ASSERT_fail("d_a_machine.cpp", 0x159, "modelData != (0)");
    J3DModel* model = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000222);
    mpModel = model;
    if (!model)
        return false;
    auto* pbck = static_cast<J3DAnmTransform*>(dComIfG_getObjectRes("Hkikai1", dRes_INDEX_HKIKAI1_BCK_HKIKAI1_e));
    if (!pbck)
        JUT_ASSERT_fail("d_a_machine.cpp", 0x169, "pbck != (0)");
    if (!field_0xc04.init(modelData, pbck, true, J3DFrameCtrl::EMode_NONE, l_rate.mValue, 0, -1, false))
        return false;
    model = mpModel;
    f32 frame = field_0xc04.getFrame();
    J3DModelData* data = model->getModelData();
    field_0xc04.entry(data, frame);
    return true;
}
/* 021B7494 */
s32 nodeCallBack(J3DNode* node, s32 timing) {
    if (timing == J3DNodeCBCalcTiming_In) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel* model = j3dSys.getModel();
        auto* actor = static_cast<daMachine_c*>(model->getUserArea());
        u16 index = reinterpret_cast<MachineJoint*>(joint)->mJntNo;
        if (actor) {
            J3DMtxBuffer_hd* buffer = model->mpMtxBuffer;
            u16 flags = buffer->mFlags;
            buffer->mFlags = flags | 16;
            // HD: the source matrix pointer is not bounds checked.
            gptr<Mtx34>& first = reinterpret_cast<gptr<Mtx34>&>(buffer->mpAnmMtx);
            PSMTXCopy(first.get() + index, mDoMtx_stack_c::get());
            PSMTXCopy(mDoMtx_stack_c::get(), &actor->field_0xc3c);
        }
    }
    return true;
}
/* 021B7600 */
void daMachine_c::CreateInit() {
    f32 modelScale = l_scale.mValue;
    scale.y = modelScale;
    J3DModel* model = mpModel;
    scale.z = modelScale;
    scale.x = modelScale;
    cullMtx = model ? gabi::ea(model->getBaseTRMtx()) : 0;
    fopAcM_setCullSizeBox(this, l_cullMin.mValue, l_cullMin.mValue, l_cullBack.mValue, l_cullRadius.mValue,
                          l_cullRadius.mValue, l_cullFront.mValue);
    mStts.Init(255, 255, this);
    mSph.Set(&l_sph_src_at);
    mSph.SetStts(&mStts);
    for (u32 i = 0; i < 3; ++i) {
        field_0x404[i].Set(&l_sph_src_col);
        field_0x404[i].SetStts(&mStts);
    }
    field_0x788.Set(&l_sph_src_col);
    field_0x788.SetStts(&mStts);
    Machine_wallRadius(&field_0xa78, l_wallRadius.mValue);
    Machine_acchSet(&field_0x84b, &current.pos, &old.pos, this, 1, &field_0xa78, &speed, nullptr, nullptr);
    set_mtx();
    model = mpModel;
    model->setUserArea(reinterpret_cast<uintptr_t>(this));
    model = mpModel;
    auto* table = Machine_jointNames(model->getModelData());
    u32 relative = table->mNamesOffset;
    // HD: the resource's name table offset is self-relative and null preserving.
    MachineNameTab* name = gabi::at<MachineNameTab>(relative ? gabi::ea(&table->mNamesOffset) + relative : 0);
    s32 index = Machine_findJoint(name, "joint3");
    if (index >= 0) {
        model = mpModel;
        auto* data = reinterpret_cast<MachineModelData*>(model->getModelData());
        u32 count = data->mJointNum;
        MachineJointNode* joints = data->mJoints;
        u32 i = u16(index);
        if (i < count)
            joints += i;
        joints->mCallBack = gabi::guest_fn(reinterpret_cast<const void*>(&nodeCallBack));
    }
    mpModel->calc();
    // HD: the path byte is the low byte of the actor parameter word.
    u8 id = u32(mParameters) & 255;
    field_0xc14 = id;
    if (id == 255)
        return;
    auto* path = Machine_roomPath(id, current.roomNo);
    field_0xc18 = path;
    if (!path) {
        field_0xc14 = 255;
        return;
    }
    path = field_0xc18;
    field_0xc16 = 1;
    field_0xc15 = 1;
    auto* points = path->m_points.get();
    auto* source = reinterpret_cast<MachineVecWords*>(&points[1].m_position);
    auto* next = reinterpret_cast<MachineVecWords*>(&field_0xc1c);
    u32 xb = source->mX;
    next->mX = xb;
    u32 yb = source->mY;
    next->mY = yb;
    u32 zb = source->mZ;
    field_0xc28.x = gabi::f32_from_bits(xb);
    next->mZ = zb;
    field_0xc28.y = gabi::f32_from_bits(yb);
    field_0xc28.z = gabi::f32_from_bits(zb);
    auto* position = reinterpret_cast<MachineVecWords*>(&current.pos);
    points = path->m_points;
    source = reinterpret_cast<MachineVecWords*>(&points[0].m_position);
    position->mX = u32(source->mX);
    points = path->m_points;
    source = reinterpret_cast<MachineVecWords*>(&points[0].m_position);
    position->mY = u32(source->mY);
    points = path->m_points;
    source = reinterpret_cast<MachineVecWords*>(&points[0].m_position);
    position->mZ = u32(source->mZ);
}
/* 021B7D14 */
void daMachine_c::path_move() {
    if (field_0xc14 == 255)
        return;
    Machine_chasePos(&current.pos, &field_0xc1c, speedF);
    gabi::Local<cXyz> difference;
    cXyz_mi(&field_0xc1c, difference.get(), &current.pos);
    f32 square = PSVECSquareMag(difference.get());
    f32 distance = Machine_sqrt(square);
    if (distance < l_nextDistance.mValue)
        set_next_pnt();
}
/* 021B7C24 */
void daMachine_c::set_next_pnt() {
    if (field_0xc14 == 255)
        return;
    s32 index = s8(u8(field_0xc15) + u8(field_0xc16));
    auto* path = field_0xc18.get();
    field_0xc15 = index;
    if (path->m_closed & 1) {
        u16 count = path->m_num;
        if (index > s32(s8(count)) - 1)
            field_0xc15 = 0;
        else if (index < 0)
            field_0xc15 = count - 1;
    } else {
        u16 count = path->m_num;
        if (index > s32(count) - 1) {
            path = field_0xc18;
            field_0xc16 = -1;
            count = path->m_num;
            field_0xc15 = count - 2;
        } else if (index < 0) {
            field_0xc16 = 1;
            field_0xc15 = 1;
        }
    }
    // HD: integer copies and pointer reloads preserve the original aliasing behavior.
    auto* next = reinterpret_cast<MachineVecWords*>(&field_0xc1c);
    auto* previous = reinterpret_cast<MachineVecWords*>(&field_0xc28);
    u32 y = next->mY, x = next->mX;
    previous->mY = y;
    previous->mX = x;
    u32 z = next->mZ;
    s8 idx = field_0xc15;
    path = field_0xc18;
    previous->mZ = z;
    auto* point = &path->m_points[idx];
    field_0xc1c.x = point->m_position.x;
    field_0xc1c.y = point->m_position.y;
    field_0xc1c.z = point->m_position.z;
}
/* 021B7AE4 */
machine_layout::WindMill* daMachine_c::search_wind_mill() {
    gabi::Local<be<s16>> id;
    *id = fpcNm_WINDMILL_e;
    return static_cast<machine_layout::WindMill*>(Machine_searchName(Machine_nameJudge, id.get()));
}
/* 021B7B18 */
void daMachine_c::set_speed() {
    auto* wind = search_wind_mill();
    f32 ratio;
    if (wind) {
        u8 id = wind->mType;
        s16 speed = wind->mAngle[1], base = l_windSpeeds.m_max_rot_speed[id];
        ratio = f32(speed) / f32(base);
    } else
        ratio = l_zero.mValue;
    gabi::Local<be<f32>> value;
    *value = speedF;
    Machine_addCalc(value.get(), f32(l_speed.mValue) * ratio, l_accel.mValue, l_rate.mValue, l_minStep.mValue);
    speedF = f32(*value);
}
/* 021B7844 */
cPhs_State daMachine_c::_create() {
    u32 flags = actor_condition;
    if (!(flags & fopAcCnd_INIT_e)) {
        if (this) {
            fopAc_ac_c_ct(this);
            __vtbl = gabi::ea(&l_actorVtable);
            dCcD_Stts_ct(&mStts);
            Machine_sphCtor(&mSph);
            Machine_constructArray(field_0x404, 3, 0x12C, Machine_sphCtor);
            Machine_sphCtor(&field_0x788);
            __ct(&field_0x84b);
            __ct(&field_0xa78);
            auto* cps = reinterpret_cast<MachineCps*>(&field_0xab8);
            Machine_gObjCtor(&field_0xab8);
            cps->mShapeVtable = gabi::ea(&l_shapeVtable);
            cps->mAabVtable = gabi::ea(&l_aabVtable);
            Machine_capsuleCtor(&cps->mCapsule);
            cps->__vtbl_hitinf = gabi::ea(&l_hitVtable);
            cps->mCapsule.__vtbl = gabi::ea(&l_capsuleVtable);
            cps->mShapeVtable = gabi::ea(&l_cpsShapeVtable);
            Machine_sphereCtor(&field_0xbf0);
            auto* animation = reinterpret_cast<MachineBck*>(&field_0xc04);
            animation->mFrameCtrl.init(0);
            animation->__vtbl = gabi::ea(&l_bckBaseVtable);
            Machine_materialCtor(&animation->mMaterial);
            animation->mMaterial.mAnim = 0;
            animation->m84 = 0;
            animation->m80 = 0;
            animation->__vtbl = gabi::ea(&l_bckVtable);
            animation->m7C = 0;
            animation->m88 = 0;
            flags = actor_condition;
            animation->mMaterial.__vtbl = gabi::ea(&l_materialVtable);
        }
        actor_condition = flags | fopAcCnd_INIT_e;
    }
    cPhs_State phase = dComIfG_resLoad(&mPhs, "Hkikai1");
    if (phase == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, 0xB00))
            return cPhs_ERROR_e;
        CreateInit();
    }
    return phase;
}
/* 021B751C */
void daMachine_c::set_mtx() {
    J3DModel* model = mpModel;
    f32 z = scale.z;
    s16 yrot = current.angle.y, xrot = current.angle.x;
    f32 x = scale.x, y = scale.y;
    s16 zrot = current.angle.z;
    model->mBaseScale.x = x;
    model->mBaseScale.z = z;
    model->mBaseScale.y = y;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), xrot, yrot, zrot);
    f32 matrix[12];
    for (u32 i = 0; i < 12; ++i)
        matrix[i] = mDoMtx_stack_c::get()->m[i / 4][i % 4];
    model = mpModel;
    // HD: all twelve values are cached before this store order; a null model retains the member offset.
    const u32 order[] = {2, 3, 5, 6, 7, 8, 9, 10, 0, 1, 11, 4};
    for (u32 i : order)
        model->mBaseTransformMtx.m[i / 4][i % 4] = matrix[i];
}
/* 021B8104 */
bool daMachine_c::_execute() {
    set_speed();
    gabi::Local<cXyz> difference;
    cXyz_mi(&l_origin, difference.get(), &current.pos);
    s32 angle = Machine_atan2(difference->x, difference->z);
    current.angle.y = angle;
    path_move();
    attack();
    set_body();
    set_at();
    set_mtx();
    if (speedF != l_zero.mValue) {
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        mDoAud_seStart(JA_SE_OBJ_JAMA_MECHA_MOVE, &eyePos, 0, reverb);
    }
    return true;
}
/* 021B7E48 */
void daMachine_c::attack() {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    if (!player)
        return;
    set_cube();
    gabi::Local<cXyz> pos;
    pos->x = player->current.pos.x;
    f32 y = player->current.pos.y;
    pos->y = y;
    pos->z = player->current.pos.z;
    pos->y = y + f32(l_playerHeight.mValue);
    field_0xbf0.SetC(pos.get());
    field_0xbf0.SetR(l_playerRadius.mValue);
    u8 state = field_0xc78;
    if (state == 0) {
        gabi::Local<cXyz> cross;
        if (Machine_capsuleCross(&reinterpret_cast<MachineCps*>(&field_0xab8)->mCapsule, &field_0xbf0, cross.get()))
            field_0xc78 = 1;
        return;
    }
    if (state == 1) {
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        mDoAud_seStart(JA_SE_OBJ_JAMA_MECHA_OUT, &eyePos, 0, reverb);
        u8 next = u8(field_0xc78) + 1;
        field_0xc04.setFrame(l_zero.mValue);
        field_0xc78 = next;
        field_0xc04.setPlaySpeed(l_rate.mValue);
    } else if (state != 2)
        return;
    if (field_0xc04.play())
        field_0xc78 = 0;
}
/* 021B7D8C */
void daMachine_c::set_cube() {
    gabi::Local<cXyz> near, far;
    far->z = l_cullFront.mValue;
    far->x = l_zero.mValue;
    near->x = l_zero.mValue;
    near->y = l_zero.mValue;
    far->y = l_zero.mValue;
    near->z = l_searchNear.mValue;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    mDoMtx_stack_c::multVec(far.get(), far.get());
    mDoMtx_stack_c::multVec(near.get(), near.get());
    Machine_capsuleSet(&reinterpret_cast<MachineCps*>(&field_0xab8)->mCapsule, near.get(), far.get(),
                       l_searchRadius.mValue);
}
/* 021B7F84 */
void daMachine_c::set_body() {
    gabi::Local<cXyz> positions[3];
    f32 zero = l_zero.mValue;
    positions[0]->x = zero;
    positions[1]->z = l_body1.mValue;
    positions[2]->y = zero;
    positions[2]->z = l_body2.mValue;
    positions[0]->y = zero;
    positions[1]->y = zero;
    positions[1]->x = zero;
    positions[0]->z = l_body0.mValue;
    positions[2]->x = zero;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, current.angle.z);
    for (u32 i = 0; i < 3; ++i) {
        auto* sphere = &field_0x404[i];
        mDoMtx_stack_c::multVec(positions[i].get(), positions[i].get());
        sphere->SetC(positions[i].get());
        cCcS_Set(dComIfG_Ccsp(), sphere);
    }
    field_0x404[0].OnAtSPrmBit(cCcD_AtSPrm_Set_e);
    field_0x404[0].SetR(l_cullRadius.mValue);
}
/* 021B8080 */
void daMachine_c::set_at() {
    f32 frame = field_0xc04.getFrame();
    if (l_speed.mValue < frame && l_attackEnd.mValue > frame) {
        f32 y = field_0xc3c.m[1][3], z = field_0xc3c.m[2][3], x = field_0xc3c.m[0][3];
        field_0xc6c.y = y;
        field_0xc6c.z = z;
        field_0xc6c.x = x;
        mSph.SetC(&field_0xc6c);
        cCcS_Set(dComIfG_Ccsp(), &mSph);
    }
}
/* 021B7A70 */
bool daMachine_c::_draw() {
    auto* env = dKy_getEnvlight();
    settingTevStruct(env, 0, &current.pos, &tevStr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, mpModel, &tevStr);
    J3DModel* model = mpModel;
    f32 frame = field_0xc04.getFrame();
    J3DModelData* data = model->getModelData();
    field_0xc04.entry(data, frame);
    mDoExt_modelUpdateDL(mpModel);
    return true;
}
/* 021B7A38 */
s32 daMachine_Create(daMachine_c* actor) {
    return actor->_create();
}
/* 021B7A6C */
s32 daMachine_Delete(daMachine_c* actor) {
    return actor->_delete();
}
/* 021B7AE0 */
s32 daMachine_Draw(daMachine_c* actor) {
    return actor->_draw();
}
/* 021B81B4 */
s32 daMachine_Execute(daMachine_c* actor) {
    return actor->_execute();
}

/* 021B8260 */
s32 daMachine_IsDelete(daMachine_c*) {
    return true;
}

/* 021B81B8 */
void machine_static() {
    __sinit_header_statics();
}
/* 021B824C */
void machine_nameDestructor(void* object, u32 flags) {
    if (object && (flags & 1))
        operator_delete(object);
}
/* 021B8268 */
void machine_destructor(daMachine_c* actor, u32 flags) {
    if (!actor)
        return;
    auto* animation = reinterpret_cast<MachineBck*>(&actor->field_0xc04);
    Machine_bckTailDtor(&animation->__vtbl, 0);
    Machine_gObjDtor(&actor->field_0xab8, 2);
    __dt(&actor->field_0xa78);
    __dt(&actor->field_0x84b);
    Machine_sphDtor(&actor->field_0x788, 2);
    Machine_destroyArray(actor->field_0x404, 3, 0x12C, Machine_sphDtor, 0, 0);
    Machine_sphDtor(&actor->mSph, 2);
    __dt(&actor->mStts);
    __dt(static_cast<fopAc_ac_c*>(actor));
    if (flags & 1)
        operator_delete(actor);
}
/* 021B8348 */
void machine_empty(void*) {}

/* WWHD glue
data l_rate 100142C4
data l_scale 100142F8
data l_cullMin 100142FC
data l_cullRadius 10014300
data l_cullBack 10014304
data l_cullFront 10014308
data l_wallRadius 1001430C
data l_zero 10014320
data l_speed 10014324
data l_accel 10014328
data l_minStep 1001432C
data l_nextDistance 10014330
data l_searchNear 10014334
data l_searchRadius 10014338
data l_playerHeight 1001433C
data l_playerRadius 10014340
data l_body0 10014344
data l_body1 10014348
data l_body2 1001434C
data l_attackEnd 10014350
data l_windSpeeds 1004BCF0
data l_sph_src_at 101B91A8
data l_sph_src_col 101B91E8
data l_origin 101FFBA8
data l_actorVtable 100142B4
data l_aabVtable 1001424C
data l_bckVtable 1001425C
data l_bckBaseVtable 1016E54C
data l_materialVtable 1016D820
data l_shapeVtable 100015A8
data l_capsuleVtable 1004AF60
data l_hitVtable 1004AF18
data l_cpsShapeVtable 1004AF70
str 10014360
str 100142C8
str 100142E4
str 100142D8
str 10014310
ext 025D5218 void* Machine_searchName(MachineNameJudge,void*)
ext 025E121C void* Machine_nameJudge(void*,void*)
ext 0200ECD4 f32 Machine_addCalc(be<f32>*,f32,f32,f32,f32)
ext 028F4384 f32 Machine_sqrt(f32)
ext 020181B0 void Machine_capsuleSet(MachineCapsule*,cXyz*,cXyz*,f32)
ext 020168D4 BOOL Machine_capsuleCross(MachineCapsule*,cM3dGSph*,cXyz*)
ext 025166F0 void Machine_sphCtor(dCcD_Sph*)
ext 02515AE8 void Machine_sphDtor(dCcD_Sph*,s32)
ext 02515FB8 void Machine_gObjCtor(dCcD_GObjInf*)
ext 02515980 void Machine_gObjDtor(dCcD_GObjInf*,s32)
ext 02018150 void Machine_capsuleCtor(MachineCapsule*)
ext 02018C40 void Machine_sphereCtor(cM3dGSph*)
ext 027DA984 void Machine_materialCtor(MachineMaterialTable*)
ext 027F3628 void Machine_bckTailDtor(be<u32>*,s32)
ext 024EFF3C void Machine_wallRadius(dBgS_AcchCir*,f32)
ext 024F06B4 void Machine_acchSet(dBgS_ObjAcch*,cXyz*,cXyz*,fopAc_ac_c*,s32,dBgS_AcchCir*,cXyz*,void*,void*)
ext 025AAF88 machine_layout::dPath* Machine_roomPath(u32,s32)
ext 027F68FC MachineJointNames* Machine_jointNames(J3DModelData*)
ext 027DF9B0 s32 Machine_findJoint(MachineNameTab*,const char*)
ext 028EFFD0 void Machine_constructArray(dCcD_Sph*,u32,u32,MachineSphCtor)
ext 028F0164 void Machine_destroyArray(dCcD_Sph*,u32,u32,MachineSphDtor,u32,u32)
ext 0200F62C void Machine_chasePos(cXyz*,cXyz*,f32)
ext 020195B0 s32 Machine_atan2(f32,f32)
*/
