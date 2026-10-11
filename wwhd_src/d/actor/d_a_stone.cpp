/* Carryable stone: verified WWHD modes, water/lava correction, effects and HD resource paths. */
#include "d/actor/d_a_stone.h"
using daStone::Act_c;
namespace stone_tu {
inline constexpr u32 groundPolygonDestructorVtable = 0x1003D7F8;
inline constexpr u32 groundPassDestructorVtable = 0x1003D818;
inline constexpr u32 groundGroupDestructorVtable = 0x1003D7D8;
inline constexpr u32 acchGroupVtable = 0x1003D8B8;
inline constexpr u32 acchPassVtable = 0x1003D8C8;
inline constexpr u32 safeStringVtable = 0x1003D7B0;
inline constexpr u32 alwaysArchive = 0x1003DFE8;
inline constexpr u32 actorVtable = 0x1003D8D8;
inline constexpr u32 acchCheckVtable = 0x1003D8A8;
inline constexpr u32 groundCheckVtable = 0x1003D868;
inline constexpr u32 groundPolygonVtable = 0x1003D878;
inline constexpr u32 groundGroupVtable = 0x1003D888;
inline constexpr u32 groundPassVtable = 0x1003D898;
inline constexpr u32 statusVtable = 0x1004AE88;
inline constexpr u32 globalStatusVtable = 0x1004AEC0;
inline constexpr u32 baseShapeVtable = 0x100015A8;
inline constexpr u32 boundsVtable = 0x1003D7C8;
inline constexpr u32 cylinderHitVtable = 0x1004B108;
inline constexpr u32 cylinderVtable = 0x1004B150;
inline constexpr u32 cylinderShapeVtable = 0x1004B160;
} // namespace stone_tu

static daStone::Data_c M_data[5] = {{-6.0f,
                                     15.0f,
                                     22.0f,
                                     1.0f,
                                     200,
                                     {0, 0, 0},
                                     27.0f,
                                     36.0f,
                                     0.0f,
                                     0.0f,
                                     0.0f,
                                     2500,
                                     2500,
                                     1000,
                                     {0, 0},
                                     0.006000000052154064f,
                                     0.0010000000474974513f,
                                     200.0f,
                                     0.6000000238418579f,
                                     0,
                                     {0, 0},
                                     5.400000095367432f,
                                     0.03999999910593033f,
                                     0.014000000432133675f,
                                     0.03999999910593033f,
                                     0.014999999664723873f,
                                     30.0f,
                                     0.5f,
                                     1.0f,
                                     0.30000001192092896f,
                                     150,
                                     5,
                                     100,
                                     4,
                                     125,
                                     5,
                                     {0, 0},
                                     0,
                                     0x1003DFE8,
                                     1216,
                                     33,
                                     13,
                                     {0},
                                     35,
                                     30,
                                     35,
                                     35,
                                     0,
                                     {0, 0},
                                     26923,
                                     26921,
                                     26904,
                                     26907,
                                     13,
                                     0,
                                     8,
                                     0,
                                     300,
                                     0,
                                     8,
                                     0,
                                     35,
                                     6000.0f},
                                    {-10.0f,
                                     15.0f,
                                     22.0f,
                                     1.0f,
                                     200,
                                     {0, 0, 0},
                                     25.0f,
                                     23.0f,
                                     0.0f,
                                     0.0f,
                                     0.0f,
                                     2000,
                                     2000,
                                     0,
                                     {0, 0},
                                     0.006000000052154064f,
                                     0.0010000000474974513f,
                                     200.0f,
                                     0.6000000238418579f,
                                     2,
                                     {0, 0},
                                     6.5f,
                                     0.03999999910593033f,
                                     0.014000000432133675f,
                                     0.03999999910593033f,
                                     0.014999999664723873f,
                                     30.0f,
                                     0.5f,
                                     1.0f,
                                     0.30000001192092896f,
                                     150,
                                     5,
                                     100,
                                     4,
                                     125,
                                     5,
                                     {0, 0},
                                     2,
                                     0x1003DFE8,
                                     1216,
                                     34,
                                     13,
                                     {0},
                                     35,
                                     30,
                                     35,
                                     35,
                                     1,
                                     {0, 0},
                                     26923,
                                     26921,
                                     26905,
                                     26908,
                                     13,
                                     0,
                                     8,
                                     0,
                                     300,
                                     0,
                                     8,
                                     0,
                                     35,
                                     6000.0f},
                                    {-6.0f,
                                     200.0f,
                                     200.0f,
                                     1.0f,
                                     255,
                                     {0, 0, 0},
                                     27.0f,
                                     36.0f,
                                     0.0f,
                                     0.0f,
                                     0.0f,
                                     2500,
                                     2500,
                                     1000,
                                     {0, 0},
                                     0.006000000052154064f,
                                     0.0010000000474974513f,
                                     200.0f,
                                     3.0f,
                                     6,
                                     {0, 0},
                                     5.400000095367432f,
                                     0.03999999910593033f,
                                     0.014000000432133675f,
                                     0.03999999910593033f,
                                     0.014999999664723873f,
                                     30.0f,
                                     2.5f,
                                     1.0f,
                                     1.5f,
                                     255,
                                     10,
                                     100,
                                     4,
                                     255,
                                     10,
                                     {0, 0},
                                     15,
                                     0x1003DFF0,
                                     2944,
                                     4,
                                     25,
                                     {0},
                                     320,
                                     280,
                                     454,
                                     300,
                                     0,
                                     {0, 0},
                                     26986,
                                     26922,
                                     26906,
                                     26909,
                                     13,
                                     0,
                                     200,
                                     0,
                                     600,
                                     0,
                                     200,
                                     0,
                                     400,
                                     6000.0f},
                                    {-6.0f,
                                     120.0f,
                                     175.0f,
                                     1.0f,
                                     255,
                                     {0, 0, 0},
                                     27.0f,
                                     36.0f,
                                     0.0f,
                                     0.0f,
                                     0.0f,
                                     2500,
                                     2500,
                                     1000,
                                     {0, 0},
                                     0.006000000052154064f,
                                     0.0010000000474974513f,
                                     200.0f,
                                     3.0f,
                                     6,
                                     {0, 0},
                                     5.400000095367432f,
                                     0.03999999910593033f,
                                     0.014000000432133675f,
                                     0.03999999910593033f,
                                     0.014999999664723873f,
                                     30.0f,
                                     2.5f,
                                     1.0f,
                                     1.5f,
                                     255,
                                     10,
                                     100,
                                     4,
                                     255,
                                     10,
                                     {0, 0},
                                     31,
                                     0x1003DFF8,
                                     3392,
                                     4,
                                     24,
                                     {0},
                                     324,
                                     275,
                                     350,
                                     430,
                                     0,
                                     {0, 0},
                                     26985,
                                     26922,
                                     26906,
                                     26909,
                                     13,
                                     0,
                                     120,
                                     0,
                                     530,
                                     0,
                                     120,
                                     0,
                                     350,
                                     6000.0f},
                                    {-6.0f,
                                     90.0f,
                                     90.0f,
                                     1.0f,
                                     255,
                                     {0, 0, 0},
                                     27.0f,
                                     36.0f,
                                     0.0f,
                                     0.0f,
                                     0.0f,
                                     2500,
                                     2500,
                                     1000,
                                     {0, 0},
                                     0.006000000052154064f,
                                     0.0010000000474974513f,
                                     200.0f,
                                     2.0f,
                                     4,
                                     {0, 0},
                                     5.400000095367432f,
                                     0.03999999910593033f,
                                     0.014000000432133675f,
                                     0.03999999910593033f,
                                     0.014999999664723873f,
                                     30.0f,
                                     1.5f,
                                     1.0f,
                                     1.0f,
                                     255,
                                     10,
                                     100,
                                     4,
                                     255,
                                     10,
                                     {0, 0},
                                     15,
                                     0x1003DFF0,
                                     1568,
                                     5,
                                     14,
                                     {0},
                                     135,
                                     110,
                                     150,
                                     170,
                                     0,
                                     {0, 0},
                                     26987,
                                     26922,
                                     26906,
                                     26909,
                                     13,
                                     0,
                                     50,
                                     0,
                                     450,
                                     0,
                                     50,
                                     0,
                                     150,
                                     6000.0f}};
static const daStone::Data_c& stone_dataType(u32 type) {
    return gabi::object_at<daStone::Data_c>(gabi::ea(M_data) + type * sizeof(daStone::Data_c));
}

using StoneModeProc = gabi::ptmf<void (Act_c::*)()>;
static u32 stone_modeProc[8] = {0x0000FFFF, 0x0249D588, 0x0000FFFF, 0x0249DBC8,
                                0x0000FFFF, 0x0249DB04, 0x0000FFFF, 0x0249DF24};
static u32 address(const void* p) { return gabi::ea(p); }

static const daStone::Data_c* data(Act_c* actor, const char* file, const char* function) {
    u32 type = static_cast<u32>(actor->mType);
    if (type >= daStone::TYPE_COUNT) {
        stone_assertData(file, 0x306, function);
        type = static_cast<u32>(actor->mType);
    }
    return &stone_dataType(type);
}
// HD: Masked PPC shift counts are retained for out-of-range parameter widths.

// HD: Model submission is retained; the GC simple-shadow call is absent in this verified HD function.

static void update_rotation(Act_c* actor, be<f32> daStone::Data_c::* linearDrag,
                            be<f32> daStone::Data_c::* quadraticDrag, const char* file,
                            const char* function) {
    for (u32 axis = 0; axis < 2; ++axis) {
        auto* velocity = axis ? &actor->mTiltSpeed : &actor->mPitchSpeed;
        auto* angle = axis ? &actor->mTilt : &actor->mPitch;
        f32 value = stone_cSAngle__Radian(velocity);
        const daStone::Data_c *first = data(actor, file, function), *second = data(actor, file, function);
        f32 signedSquare = std::fabs(value) * value;
        f32 quadratic = signedSquare * static_cast<f32>(second->*quadraticDrag);
        f32 linear = static_cast<f32>(first->*linearDrag);
        s32 adjustment = stone_cM_rad2s(gabi::fmadds(value, linear, quadratic));
        stone_cSAngle_subtract(velocity, adjustment);
        stone_cSAngle_add(angle, velocity);
    }
    auto matrix = static_cast<void*>(&stone_matrix());
    stone_mDoMtx_YrotS(matrix, static_cast<s16>(actor->mYaw));
    stone_mDoMtx_XrotM(matrix, static_cast<s16>(actor->mPitch));
    stone_mDoMtx_ZrotM(matrix, static_cast<s16>(actor->mRoll));
    stone_mDoMtx_YrotM(matrix, 0x4000);
    stone_PSMTXMultVecSR(matrix, static_cast<void*>(const_cast<cXyz*>(&stone_baseZ())),
                         static_cast<void*>(&stone_axis()));
}

// HD: Actor and embedded collision objects use the HD constructor/vtable copies.
static void construct_actor(Act_c* actor) {

    stone_fopAc_ac_c__fopAc_ac_c(actor);
    actor->__vtbl = static_cast<u32>(stone_tu::actorVtable);
    stone_dBgS_Acch__dBgS_Acch(static_cast<void*>(&actor->mAcch));
    actor->mAcch.mbObjThrough = static_cast<u8>(1);
    actor->mAcch.cBgS_Chk::__vtbl = static_cast<u32>(stone_tu::acchCheckVtable);
    actor->mAcch.dBgS_GrpPassChk::__vtbl = static_cast<u32>(stone_tu::acchGroupVtable);
    actor->mAcch.dBgS_PolyPassChk::__vtbl = static_cast<u32>(stone_tu::acchPassVtable);
    stone_dBgS_AcchCir__dBgS_AcchCir(static_cast<void*>(&actor->mAcchCir));
    stone_cBgS_GndChk__cBgS_GndChk(static_cast<void*>(&actor->mGndChkYogan));
    actor->mGndChkYogan.mbCamThrough = 0;
    actor->mGndChkYogan.mbLinkThrough = 0;
    actor->mGndChkYogan.mbArrowThrough = 0;
    actor->mGndChkYogan.mbBombThrough = 0;
    actor->mGndChkYogan.mbBoomerangThrough = 0;
    actor->mGndChkYogan.mbRopeThrough = 0;
    actor->mGndChkYogan.mPolyPassChk =
        static_cast<u32>(gabi::ea(static_cast<dBgS_PolyPassChk*>(&actor->mGndChkYogan)));
    actor->mGndChkYogan.mGrpPassChk =
        static_cast<u32>(gabi::ea(static_cast<dBgS_GrpPassChk*>(&actor->mGndChkYogan)));
    actor->mGndChkYogan.cBgS_Chk::__vtbl = static_cast<u32>(stone_tu::groundCheckVtable);
    actor->mGndChkYogan.cBgS_PolyInfo::__vtbl = static_cast<u32>(stone_tu::groundPolygonVtable);
    actor->mGndChkYogan.dBgS_GrpPassChk::__vtbl = static_cast<u32>(stone_tu::groundGroupVtable);
    actor->mGndChkYogan.dBgS_PolyPassChk::__vtbl = static_cast<u32>(stone_tu::groundPassVtable);
    actor->mGndChkYogan.mGrp = static_cast<u32>(4);
    actor->mGndChkYogan.mbObjThrough = static_cast<u8>(1);
    stone_cCcD_Stts_construct(static_cast<void*>(&actor->mStts));
    stone_dCcD_GStts__dCcD_GStts(static_cast<void*>(&actor->mStts.__vtbl_gstts));
    actor->mStts.__vtbl = static_cast<u32>(stone_tu::statusVtable);
    actor->mStts.__vtbl_gstts = static_cast<u32>(stone_tu::globalStatusVtable);
    stone_dCcD_GObjInf__dCcD_GObjInf(static_cast<void*>(&actor->mCyl));
    stone_shape(actor->mCyl).shapeVtable = static_cast<u32>(stone_tu::baseShapeVtable);
    stone_shape(actor->mCyl).boundsVtable = static_cast<u32>(stone_tu::boundsVtable);
    stone_cM3dGCyl_construct(static_cast<void*>(&actor->mCyl.mCyl));
    actor->mCyl.__vtbl_hitinf = static_cast<u32>(stone_tu::cylinderHitVtable);
    actor->mCyl.mCyl.__vtbl = static_cast<u32>(stone_tu::cylinderVtable);
    stone_shape(actor->mCyl).shapeVtable = static_cast<u32>(stone_tu::cylinderShapeVtable);
    for (auto* angle : {&actor->mRoll, &actor->mPitch, &actor->mPitchSpeed, &actor->mTilt, &actor->mTiltSpeed,
                        &actor->mYaw})
        stone_cSAngle_construct(angle);
}
static bool same_stage(u32 left, u32 right) {
    if (left == right)
        return true;
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 a = static_cast<u8>(stone_stringByte(left, i)), b = static_cast<u8>(stone_stringByte(right, i));
        if (a != b)
            return false;
        if (!a)
            return true;
    }
    return false;
}

/* 0249ADFC */
s32 CreateHeapCB(Act_c* actor) { return actor->create_heap(); }

/* 0249ABE8 */
s32 daStone::Act_c::create_heap() {
    const daStone::Data_c* record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    u32 archive = static_cast<u32>(record->archive);
    record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    u16 modelIndex = static_cast<u16>(record->modelIndex);
    u32 resources = static_cast<u32>(stone_resourceAddress());
    StoneLocal<SafeString> name;
    name->mStringTop = archive;
    name->__vtbl = stone_tu::safeStringVtable;
    void* modelData = stone_getObjectResource(static_cast<void*>(stone_pointer<StoneResources>(resources)),
                                              name.get(), modelIndex);
    if (!modelData)
        stone_assertData("d_a_stone.cpp", 0x320, "modelData != 0");
    void* model = stone_mDoExt_J3DModel__create(modelData, 0x80000, 0x11000002);
    this->mpModel.v = static_cast<u32>(address(model));
    if (!model)
        return 0;
    record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    f32 radius = f32(static_cast<s16>(record->wallRadius));
    f32 wallHeight = static_cast<f32>(stone_wallHeight());
    stone_dBgS_AcchCir__SetWall(static_cast<void*>(&this->mAcchCir), wallHeight, radius);

    stone_dBgS_Acch__Set(static_cast<void*>(&this->mAcch), static_cast<void*>(&this->current.pos),
                         static_cast<void*>(&this->old.pos), this, 1, static_cast<void*>(&this->mAcchCir),
                         static_cast<void*>(&this->speed), static_cast<void*>(&this->current.angle),
                         static_cast<void*>(&this->shape_angle));
    u32 flags = static_cast<u32>(this->mAcch.m_flags) & ~u32(dBgS_Acch::ROOF_NONE | dBgS_Acch::WATER_NONE);
    u32 type = static_cast<u32>(this->mType);
    this->mAcch.m_flags = static_cast<u32>(flags);
    if (type >= daStone::TYPE_COUNT) {
        stone_assertData("d_a_stone.cpp", 0x306, "M_type >= 0 && M_type < Type_Max");
        type = static_cast<u32>(this->mType);
    }
    f32 groundOffset = f32(static_cast<s16>(stone_dataType(type).roofHeight));
    f32 invalid = static_cast<f32>(stone_invalidHeight());
    this->mSurfaceHeights.x = static_cast<f32>(invalid);
    this->mSurfaceHeights.y = static_cast<f32>(invalid);
    this->mSurfaceHeights.z = static_cast<f32>(invalid);
    this->mWaterHit = static_cast<u8>(0);
    this->mSeaHit = static_cast<u8>(0);
    this->mAcch.m_roof_crr_height = static_cast<f32>(groundOffset);
    return 1;
}

/* 0249AB20 */
s32 daStone::Act_c::chk_appear() {
    u32 sw = parameter(this, daStone::PRM_SWSAVE_W, daStone::PRM_SWSAVE_S);
    const daStone::Data_c* record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    if ((static_cast<u32>(record->flags) & daStone::DATAFLG_SAVE_SWITCH) && sw != 255) {
        u32 save = static_cast<u32>(stone_saveAddress());
        s8 room = static_cast<s8>(this->home.roomNo);
        return !stone_dSv_info_c__isSwitch(static_cast<void*>(stone_pointer<StoneSaveInfo>(save + 0x20)), sw,
                                           room);
    }
    return 1;
}

/* 0249B2C4 */
s32 daStone::Act_c::_create() {
    u32 condition = static_cast<u32>(this->actor_condition);
    if (!(condition & 8)) {
        if (gabi::ea(this) != 0) {
            construct_actor(this);
            condition = static_cast<u32>(this->actor_condition);
        }
        this->actor_condition = static_cast<u32>(condition | 8);
    }
    u32 type = parameter(this, daStone::PRM_TYPE_W, daStone::PRM_TYPE_S);
    this->mType = static_cast<u32>(type);
    s32 appear = this->chk_appear();
    this->mAppeared = static_cast<u8>(u8(appear));
    if (!appear)
        return cPhs_ERROR_e;
    const daStone::Data_c* record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    s32 phase = cPhs_COMPLEATE_e;
    if (static_cast<u32>(record->archive) != stone_tu::alwaysArchive) {
        record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
        u32 archive = static_cast<u32>(record->archive);
        phase = stone_dComIfG_resLoad(static_cast<void*>(&this->mPhase), stone_archiveName(archive));
        if (phase != cPhs_COMPLEATE_e)
            return phase;
    }
    record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    u32 heapSize = static_cast<u32>(record->heapSize);
    if (!stone_fopAcM_entrySolidHeap(
            this, stone_callbackPointer(reinterpret_cast<const void*>(&CreateHeapCB)), heapSize))
        return cPhs_ERROR_e;
    if (!static_cast<s16>(this->home.angle.y)) {
        record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
        if (!(static_cast<u32>(record->flags) & daStone::DATAFLG_KEEP_YAW)) {
            f32 angle = stone_cM_rndFX(static_cast<f32>(stone_randomAngleRange()));
            u16 yaw = u16(gabi::ftoi(angle));
            this->home.angle.y = static_cast<u16>(yaw);
            this->current.angle.y = static_cast<u16>(yaw);
            this->shape_angle.y = static_cast<u16>(yaw);
        }
    }
    u32 model = static_cast<u32>(this->mpModel.v);
    this->cullMtx = static_cast<u32>(model ? model + 0xC8 : 0);
    this->cull_set_draw();
    StoneLocal<SafeString> stageName;
    stageName->mStringTop = gabi::guest_str("sea");
    stageName->__vtbl = stone_tu::safeStringVtable;
    u32 play = address(stone_getPlay());
    StoneLocal<SafeString> currentStage;
    currentStage->__vtbl = stone_tu::safeStringVtable;
    currentStage->mStringTop = gabi::ea(&gabi::object_at<StonePlayView>(play).stageName[0]);
    u32 vtable = stageName->__vtbl;
    gabi::call<void>(static_cast<u32>(gabi::object_at<StoneSafeStringVtable>(vtable).assureTermination),
                     stageName.get());
    vtable = stageName->__vtbl;
    gabi::call<void>(static_cast<u32>(gabi::object_at<StoneSafeStringVtable>(vtable).assureTermination),
                     stageName.get());
    vtable = currentStage->__vtbl;
    u32 left = stageName->mStringTop;
    gabi::call<void>(static_cast<u32>(gabi::object_at<StoneSafeStringVtable>(vtable).assureTermination),
                     currentStage.get());
    u32 right = currentStage->mStringTop;
    if (same_stage(left, right)) {
        play = address(stone_getPlay());
        u32 stage = gabi::ea(&gabi::object_at<StonePlayView>(play).stageVtable),
            table = static_cast<u32>(gabi::object_at<StonePlayView>(play).stageVtable);
        void* environment =
            gabi::call<void*>(static_cast<u32>(gabi::object_at<StoneStageVtable>(table).getEnvironment),
                              static_cast<void*>(stone_pointer<u8>(stage)));
        f32 distance =
            f32(static_cast<u16>(gabi::object_at<StoneStageEnvironment>(address(environment)).cullPoint));
        if (distance > static_cast<f32>(stone_one())) {
            record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
            this->cullSizeFar = static_cast<f32>(static_cast<f32>(record->cullDistance) / distance);
        }
    }
    record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    u8 weight = static_cast<u8>(record->weight);
    stone_dCcD_Stts__Init(static_cast<void*>(&this->mStts), weight, 255, this);
    stone_dCcD_Cyl__Set(static_cast<void*>(&this->mCyl), stone_cylinderSource());
    type = static_cast<u32>(this->mType);
    this->mCyl.mStts.v = static_cast<u32>(gabi::ea(&this->mStts));
    if (type >= daStone::TYPE_COUNT) {
        stone_assertData("d_a_stone.cpp", 0x306, "M_type >= 0 && M_type < Type_Max");
        type = static_cast<u32>(this->mType);
    }
    f32 radius = f32(static_cast<s16>(stone_dataType(type).wallRadius));
    stone_cM3dGCyl__SetR(static_cast<void*>(&this->mCyl.mCyl), radius);
    record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    f32 height = f32(static_cast<s16>(record->cylinderHeight));
    stone_cM3dGCyl__SetH(static_cast<void*>(&this->mCyl.mCyl), height);
    for (u32 i = 0; i != 3; ++i)
        stone_words(this->mCyl.mGObjAt.mVec).xyz[i] =
            static_cast<u32>(stone_words(const_cast<cXyz&>(stone_zeroVector())).xyz[i]);
    stone_words(this->mCyl.mGObjTg.mVec).xyz[0] =
        static_cast<u32>(stone_words(const_cast<cXyz&>(stone_zeroVector())).xyz[0]);
    stone_words(this->mCyl.mGObjTg.mVec).xyz[1] =
        static_cast<u32>(stone_words(const_cast<cXyz&>(stone_zeroVector())).xyz[1]);
    type = static_cast<u32>(this->mType);
    stone_words(this->mCyl.mGObjTg.mVec).xyz[2] =
        static_cast<u32>(stone_words(const_cast<cXyz&>(stone_zeroVector())).xyz[2]);
    if (type >= daStone::TYPE_COUNT) {
        stone_assertData("d_a_stone.cpp", 0x306, "M_type >= 0 && M_type < Type_Max");
        type = static_cast<u32>(this->mType);
    }
    this->gravity = static_cast<f32>(static_cast<f32>(stone_dataType(type).gravity));
    stone_fopAcM_posMoveF(this, nullptr);
    play = address(stone_getPlay());
    stone_dBgS_Acch__CrrPos(static_cast<void*>(&this->mAcch),
                            static_cast<void*>(&gabi::object_at<StonePlayView>(play).background[0]));
    this->mAcch.m_flags = static_cast<u32>(static_cast<u32>(this->mAcch.m_flags) & ~0x80u);
    if (parameter(this, daStone::PRM_MOVEBG_W, daStone::PRM_MOVEBG_S) == 1) {
        f32 x = static_cast<f32>(this->home.pos.x), y = static_cast<f32>(this->home.pos.y);
        this->current.pos.x = static_cast<f32>(x);
        f32 z = static_cast<f32>(this->home.pos.z);
        this->current.pos.y = static_cast<f32>(y);
        this->current.pos.z = static_cast<f32>(z);
        play = address(stone_getPlay());
        if (stone_dBgS__ChkMoveBG_NoDABg(
                static_cast<void*>(&gabi::object_at<StonePlayView>(play).background[0]),
                static_cast<void*>(static_cast<cBgS_PolyInfo*>(&this->mAcch.m_gnd)))) {
            u32 parameters = static_cast<u32>(this->mParameters);
            f32 ground = static_cast<f32>(this->mAcch.m_ground_h);
            this->mParameters = static_cast<u32>(parameters | 0xC0);
            this->current.pos.y = static_cast<f32>(ground);
        }
    }
    f32 y = static_cast<f32>(this->current.pos.y);
    this->mGrounded = static_cast<u8>(1);
    this->mLandingDelay = static_cast<u8>(20);
    this->mPreviousHeight = static_cast<f32>(y);
    this->init_mtx();
    this->mode_wait_init();
    u32 attention = static_cast<u32>(this->attention_info.flags);
    type = static_cast<u32>(this->mType);
    this->attention_info.flags = static_cast<u32>(attention | 0x10);
    if (type >= daStone::TYPE_COUNT) {
        stone_assertData("d_a_stone.cpp", 0x306, "M_type >= 0 && M_type < Type_Max");
        type = static_cast<u32>(this->mType);
    }
    record = &stone_dataType(type);
    type = static_cast<u32>(this->mType);
    this->attention_info.distances[4] = static_cast<u8>(static_cast<u8>(record->carryDistance));
    if (type >= daStone::TYPE_COUNT) {
        stone_assertData("d_a_stone.cpp", 0x306, "M_type >= 0 && M_type < Type_Max");
        type = static_cast<u32>(this->mType);
    }
    u32 dataFlags = static_cast<u32>(stone_dataType(type).flags);
    type = static_cast<u32>(this->mType);
    f32 x = static_cast<f32>(this->current.pos.x);
    if (dataFlags & daStone::DATAFLG_ALWAYS_THROW)
        this->actor_status = static_cast<u32>(static_cast<u32>(this->actor_status) | 0x10000);
    this->attention_info.position.x = static_cast<f32>(x);
    if (type >= daStone::TYPE_COUNT) {
        stone_assertData("d_a_stone.cpp", 0x306, "M_type >= 0 && M_type < Type_Max");
        type = static_cast<u32>(this->mType);
    }
    y = static_cast<f32>(this->current.pos.y);
    f32 z = static_cast<f32>(this->current.pos.z),
        offset = static_cast<f32>(stone_dataType(type).attentionHeight);
    this->mMovingGround = static_cast<u8>(1);
    y += offset;
    this->mMassDelay = static_cast<u8>(0);
    this->mGrabStarted = static_cast<u8>(0);
    this->attention_info.position.y = static_cast<f32>(y);
    this->attention_info.position.z = static_cast<f32>(z);
    return phase;
}

/* 0249BA60 */
s32 daStone::Act_c::_delete() {
    if (static_cast<u8>(this->mAppeared)) {
        const daStone::Data_c* record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
        if (static_cast<u32>(record->archive) != stone_tu::alwaysArchive) {
            record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
            u32 archive = static_cast<u32>(record->archive);
            stone_dComIfG_resDelete(static_cast<void*>(&this->mPhase), stone_archiveName(archive));
        }
    }
    return 1;
}

/* 0249B1C0 */
void daStone::Act_c::mode_wait_init() {

    u32 co = static_cast<u32>(this->mCyl.mObjCo.mSPrm), flags = static_cast<u32>(this->mAcch.m_flags);
    u32 at = static_cast<u32>(this->mCyl.mObjTg.mSPrm);
    f32 zero = static_cast<f32>(stone_zero());
    u32 target = static_cast<u32>(this->mCyl.mObjAt.mSPrm);
    this->mCyl.mObjTg.mSPrm = static_cast<u32>(at | 1);
    this->mAcch.m_flags = static_cast<u32>(
        (flags | dBgS_Acch::ROOF_NONE) &
        ~u32(dBgS_Acch::GRND_NONE | dBgS_Acch::WALL_NONE | dBgS_Acch::WATER_NONE | dBgS_Acch::LINE_CHECK));
    u32 type = static_cast<u32>(this->mType);
    this->mCyl.mObjAt.mSPrm = static_cast<u32>(target & ~1u);
    this->speedF = static_cast<f32>(zero);
    this->mCyl.mObjCo.mSPrm = static_cast<u32>(co | 1);
    if (type >= daStone::TYPE_COUNT) {
        stone_assertData("d_a_stone.cpp", 0x306, "M_type >= 0 && M_type < Type_Max");
        type = static_cast<u32>(this->mType);
    }
    const daStone::Data_c* record = &stone_dataType(type);
    type = static_cast<u32>(this->mType);
    f32 gravity = static_cast<f32>(record->gravity);
    this->gravity = static_cast<f32>(gravity);
    if (type >= daStone::TYPE_COUNT) {
        stone_assertData("d_a_stone.cpp", 0x306, "M_type >= 0 && M_type < Type_Max");
        type = static_cast<u32>(this->mType);
    }
    u8 weight = static_cast<u8>(stone_dataType(type).weight);
    stone_dCcD_Stts__Init(static_cast<void*>(&this->mStts), weight, 255, this);
    this->mMode = daStone::MODE_WAIT;
}

/* 0249D588 */
void daStone::Act_c::mode_wait() {

    if (static_cast<u32>(this->mAcch.m_flags) & 0x20) {
        stone_fopAcM_posMoveF(this, static_cast<void*>(&this->mStts));
        this->attention_info.flags = static_cast<u32>(static_cast<u32>(this->attention_info.flags) | 0x10);
    } else {
        const daStone::Data_c* first = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
        const daStone::Data_c* second = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
        f32 friction = static_cast<f32>(first->airFriction), speed = static_cast<f32>(second->airStreamSpeed);
        stone_daObj__posMoveF_stream(this, static_cast<void*>(&this->mStts),
                                     static_cast<void*>(const_cast<cXyz*>(&stone_zeroVector())), friction,
                                     speed);
        this->attention_info.flags = static_cast<u32>(static_cast<u32>(this->attention_info.flags) & ~0x10u);
    }
}

/* 0249CB44 */
void daStone::Act_c::mode_carry_init() {

    u32 at = static_cast<u32>(this->mCyl.mObjTg.mSPrm);
    u32 flags = static_cast<u32>(this->mAcch.m_flags) &
                ~u32(dBgS_Acch::GRND_NONE | dBgS_Acch::WALL_NONE | dBgS_Acch::ROOF_NONE |
                     dBgS_Acch::WATER_NONE | dBgS_Acch::LINE_CHECK);
    u32 co = static_cast<u32>(this->mCyl.mObjCo.mSPrm);
    this->mGrabStarted = static_cast<u8>(0);
    at |= 1;
    u32 attention = static_cast<u32>(this->attention_info.flags);
    u32 target = static_cast<u32>(this->mCyl.mObjAt.mSPrm);
    this->mAcch.m_flags = static_cast<u32>(flags);
    this->mCyl.mObjTg.mSPrm = static_cast<u32>(at);
    this->mCyl.mObjAt.mSPrm = static_cast<u32>(target & ~1u);
    this->mMode = daStone::MODE_CARRY;
    this->attention_info.flags = static_cast<u32>(attention & ~0x10u);
    this->mCyl.mObjCo.mSPrm = static_cast<u32>(co & ~1u);
}

/* 0249DBC8 */
void daStone::Act_c::mode_carry() {

    if (static_cast<u32>(this->actor_status) & 0x2000) {
        u32 play = address(stone_getPlay());
        u8 started = static_cast<u8>(this->mGrabStarted);
        u32 player = static_cast<u32>(gabi::object_at<StonePlayView>(play).player);
        if (!started) {
            u32 vtable = static_cast<u32>(gabi::object_at<StonePlayer>(player).vtable),
                target = static_cast<u32>(gabi::object_at<StonePlayerVtable>(vtable).getGrabActorID);
            u32 carried = gabi::call<u32>(target, static_cast<void*>(stone_pointer<StonePlayer>(player)));
            if (carried == static_cast<u32>(this->mBsPcId)) {
                if (static_cast<u32>(gabi::object_at<StonePlayer>(player).grabStatus) & 0x8000)
                    this->mGrabStarted = static_cast<u8>(1);
            } else
                this->mGrabStarted = static_cast<u8>(0);
        }
    } else {
        f32 speed = static_cast<f32>(this->speedF), zero = static_cast<f32>(stone_zero()),
            y = static_cast<f32>(this->current.pos.y);
        this->mPreviousHeight = static_cast<f32>(y);
        if (speed > zero || (static_cast<u32>(this->actor_status) & 0x10000)) {
            this->init_rot_throw();
            this->mode_drop_init();
            this->mode_drop();
        } else {
            stone_daObj__SetCurrentRoomNo(this, static_cast<void*>(&this->mAcch.m_gnd));
            this->mMassDelay = static_cast<u8>(2);
            this->mode_wait_init();
        }
    }
}

/* 0249D7A8 */
void daStone::Act_c::mode_drop_init() {
    u32 play = address(stone_getPlay());
    u32 type = static_cast<u32>(this->mType);
    u32 player = static_cast<u32>(gabi::object_at<StonePlayView>(play).player);
    if (type >= daStone::TYPE_COUNT) {
        stone_assertData("d_a_stone.cpp", 0x306, "M_type >= 0 && M_type < Type_Max");
        type = static_cast<u32>(this->mType);
    }
    const daStone::Data_c* first = &stone_dataType(type);
    const daStone::Data_c* second = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");

    u32 target = static_cast<u32>(this->mCyl.mObjAt.mSPrm);
    f32 playerSpeed = static_cast<f32>(gabi::object_at<StonePlayer>(player).speedF);
    u32 co = static_cast<u32>(this->mCyl.mObjCo.mSPrm), at = static_cast<u32>(this->mCyl.mObjTg.mSPrm);
    f32 base = static_cast<f32>(first->throwBaseSpeed),
        multiplier = static_cast<f32>(second->throwPlayerSpeedScale);
    type = static_cast<u32>(this->mType);
    f32 speed = gabi::fmadds(playerSpeed, multiplier, base);
    this->mCyl.mObjTg.mSPrm = static_cast<u32>(at | 1);
    this->mCyl.mObjAt.mSPrm = static_cast<u32>(target | 1);
    this->mCyl.mObjCo.mSPrm = static_cast<u32>(co | 1);
    if (type >= daStone::TYPE_COUNT) {
        stone_assertData("d_a_stone.cpp", 0x306, "M_type >= 0 && M_type < Type_Max");
        type = static_cast<u32>(this->mType);
    }
    u8 weight = static_cast<u8>(stone_dataType(type).weight);
    stone_dCcD_Stts__Init(static_cast<void*>(&this->mStts), weight, 255, this);
    u32 flags = static_cast<u32>(this->mAcch.m_flags);
    type = static_cast<u32>(this->mType);
    u32 attention = static_cast<u32>(this->attention_info.flags);
    this->mAcch.m_flags = static_cast<u32>((flags & ~u32(dBgS_Acch::GRND_NONE | dBgS_Acch::WALL_NONE |
                                                         dBgS_Acch::ROOF_NONE | dBgS_Acch::WATER_NONE)) |
                                           dBgS_Acch::LINE_CHECK);
    this->attention_info.flags = static_cast<u32>(attention & ~0x10u);
    if (type >= daStone::TYPE_COUNT) {
        stone_assertData("d_a_stone.cpp", 0x306, "M_type >= 0 && M_type < Type_Max");
        type = static_cast<u32>(this->mType);
    }
    const daStone::Data_c* record = &stone_dataType(type);
    type = static_cast<u32>(this->mType);
    f32 verticalSpeed = static_cast<f32>(record->throwVerticalSpeed);
    this->speedF = static_cast<f32>(speed);
    this->speed.y = static_cast<f32>(verticalSpeed);
    if (type >= daStone::TYPE_COUNT) {
        stone_assertData("d_a_stone.cpp", 0x306, "M_type >= 0 && M_type < Type_Max");
        type = static_cast<u32>(this->mType);
    }
    f32 gravity = static_cast<f32>(stone_dataType(type).gravity);
    this->mMode = daStone::MODE_DROP;
    this->gravity = static_cast<f32>(gravity);
}

/* 0249DB04 */
void daStone::Act_c::mode_drop() {
    stone_daObj__SetCurrentRoomNo(this, static_cast<void*>(&this->mAcch.m_gnd));
    void* movement = stone_daObj__get_wind_spd(this, static_cast<f32>(stone_windScale()));
    const daStone::Data_c* first = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    const daStone::Data_c* second = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    f32 friction = static_cast<f32>(first->airFriction), speed = static_cast<f32>(second->airStreamSpeed);
    stone_daObj__posMoveF_stream(this, static_cast<void*>(&this->mStts), movement, friction, speed);
    stone_set_drop_rot(this);
}

/* 0249C5F4 */
void daStone::Act_c::mode_sink_init() {

    u32 at = static_cast<u32>(this->mCyl.mObjTg.mSPrm), target = static_cast<u32>(this->mCyl.mObjAt.mSPrm),
        type = static_cast<u32>(this->mType);
    u32 co = static_cast<u32>(this->mCyl.mObjCo.mSPrm);
    this->mCyl.mObjAt.mSPrm = static_cast<u32>(target & ~1u);
    this->mCyl.mObjTg.mSPrm = static_cast<u32>(at | 1);
    this->mCyl.mObjCo.mSPrm = static_cast<u32>(co | 1);
    if (type >= daStone::TYPE_COUNT) {
        stone_assertData("d_a_stone.cpp", 0x306, "M_type >= 0 && M_type < Type_Max");
        type = static_cast<u32>(this->mType);
    }
    u8 weight = static_cast<u8>(stone_dataType(type).weight);
    stone_dCcD_Stts__Init(static_cast<void*>(&this->mStts), weight, 255, this);
    u32 flags = static_cast<u32>(this->mAcch.m_flags);
    type = static_cast<u32>(this->mType);
    this->mAcch.m_flags = static_cast<u32>((flags & ~u32(dBgS_Acch::GRND_NONE | dBgS_Acch::WALL_NONE |
                                                         dBgS_Acch::ROOF_NONE | dBgS_Acch::WATER_NONE)) |
                                           dBgS_Acch::LINE_CHECK);
    if (type >= daStone::TYPE_COUNT) {
        stone_assertData("d_a_stone.cpp", 0x306, "M_type >= 0 && M_type < Type_Max");
        type = static_cast<u32>(this->mType);
    }
    const daStone::Data_c* first = &stone_dataType(type);
    const daStone::Data_c* second = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    f32 horizontal = static_cast<f32>(this->speedF), gravity = static_cast<f32>(first->gravity);
    f32 horizontalSquare = horizontal * horizontal;
    f32 buoyancy = static_cast<f32>(second->buoyancy), vertical = static_cast<f32>(this->speed.y);
    f32 acceleration = gravity + buoyancy;
    f32 energy = gabi::fmadds(vertical, vertical, horizontalSquare);
    this->gravity = static_cast<f32>(acceleration);
    f32 speed = stone_sqrt(energy);
    const daStone::Data_c* record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    f32 limit = static_cast<f32>(record->sinkSpeedLimit);
    if (speed > limit) {
        record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
        f32 ratio = static_cast<f32>(record->sinkSpeedLimit) / speed;
        stone_PSVECScale(static_cast<void*>(&this->speed), static_cast<void*>(&this->speed), ratio);
        this->speedF = static_cast<f32>(static_cast<f32>(this->speedF) * ratio);
    }
    u32 attention = static_cast<u32>(this->attention_info.flags);
    this->mMode = daStone::MODE_SINK;
    this->attention_info.flags = static_cast<u32>(attention & ~0x10u);
}

/* 0249DF24 */
void daStone::Act_c::mode_sink() {
    stone_daObj__SetCurrentRoomNo(this, static_cast<void*>(&this->mAcch.m_gnd));
    this->water_tention();
    const daStone::Data_c *first = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max"),
                          *second = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    f32 friction = static_cast<f32>(first->waterFriction), speed = static_cast<f32>(second->waterStreamSpeed);
    stone_daObj__posMoveF_stream(this, static_cast<void*>(&this->mStts),
                                 static_cast<void*>(const_cast<cXyz*>(&stone_zeroVector())), friction, speed);
    this->set_sink_rot();
}

/* 0249D078 */
s32 daStone::Act_c::mode_proc_call() {

    s32 mode;
    if (static_cast<u32>(this->actor_status) & 0x2000) {
        mode = static_cast<s32>(this->mMode);
        if (mode != daStone::MODE_CARRY) {
            this->init_rot_clean();
            this->mode_carry_init();
            mode = static_cast<s32>(this->mMode);
        }
    } else
        mode = static_cast<s32>(this->mMode);
    bool restorePosition = parameter(this, daStone::PRM_MOVEBG_W, daStone::PRM_MOVEBG_S) == 1;

    f32 x = static_cast<f32>(this->current.pos.x);
    f32 z = static_cast<f32>(this->current.pos.z), y = static_cast<f32>(this->current.pos.y);
    (this->*gabi::object_at<StoneModeProc>(gabi::ea(stone_modeProc) +
                                           static_cast<u32>(mode) * sizeof(StoneModeProc)))();
    if (static_cast<s32>(this->mMode) == daStone::MODE_CARRY) {
        x = static_cast<f32>(this->current.pos.x);
        y = static_cast<f32>(this->current.pos.y);
        z = static_cast<f32>(this->current.pos.z);
        restorePosition = true;
    }
    u32 play = address(stone_getPlay());
    stone_dBgS_Acch__CrrPos(static_cast<void*>(&this->mAcch),
                            static_cast<void*>(&gabi::object_at<StonePlayView>(play).background[0]));
    this->bg_crr_water();
    this->bg_crr_lava();
    play = address(stone_getPlay());
    if (stone_dBgS__ChkMoveBG_NoDABg(static_cast<void*>(&gabi::object_at<StonePlayView>(play).background[0]),
                                     static_cast<void*>(static_cast<cBgS_PolyInfo*>(&this->mAcch.m_gnd)))) {
        play = address(stone_getPlay());
        if (stone_dBgS__ChkMoveBG(static_cast<void*>(&gabi::object_at<StonePlayView>(play).background[0]),
                                  static_cast<void*>(static_cast<cBgS_PolyInfo*>(&this->mAcch.m_gnd))))
            this->mMovingGround = static_cast<u8>(1);
        if (parameter(this, daStone::PRM_MOVEBG_W, daStone::PRM_MOVEBG_S) == 1) {
            u32 parameters = static_cast<u32>(this->mParameters);
            y = static_cast<f32>(this->mAcch.m_ground_h);
            this->mParameters = static_cast<u32>(parameters | 0xC0);
            this->mPreviousHeight = static_cast<f32>(y);
        }
    }
    if (restorePosition) {
        this->current.pos.y = static_cast<f32>(y);
        this->current.pos.x = static_cast<f32>(x);
        this->current.pos.z = static_cast<f32>(z);
    }
    if (this->damage_bg_proc_directly())
        return 0;
    if (static_cast<s32>(this->mMode) != daStone::MODE_CARRY) {
        this->tevStr.mRoomNo = static_cast<u8>(static_cast<u8>(this->current.roomNo));
        play = address(stone_getPlay());
        u8 room =
            stone_dBgS__GetPolyColor(static_cast<void*>(&gabi::object_at<StonePlayView>(play).background[0]),
                                     static_cast<void*>(static_cast<cBgS_PolyInfo*>(&this->mAcch.m_gnd)));
        this->tevStr.mEnvrIdxOverride = static_cast<u8>(room);
    }
    return 1;
}

/* 0249AE00 */
void daStone::Act_c::cull_set_draw() {
    const daStone::Data_c* x = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    const daStone::Data_c* y = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    const daStone::Data_c* z = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    const daStone::Data_c* radius = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    f32 fx = f32(static_cast<s16>(x->drawCullX)), fy = f32(static_cast<s16>(y->drawCullY));
    f32 fz = f32(static_cast<s16>(z->drawCullZ)), fr = f32(static_cast<s16>(radius->drawCullRadius));
    stone_fopAcM_setCullSizeSphere(this, fx, fy, fz, fr);
}

/* 0249BB2C */
void daStone::Act_c::cull_set_move() {
    const daStone::Data_c* x = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    const daStone::Data_c* y = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    const daStone::Data_c* z = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    const daStone::Data_c* radius = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    f32 fx = f32(static_cast<s16>(x->moveCullX)), fy = f32(static_cast<s16>(y->moveCullY));
    f32 fz = f32(static_cast<s16>(z->moveCullZ)), fr = f32(static_cast<s16>(radius->moveCullRadius));
    stone_fopAcM_setCullSizeSphere(this, fx, fy, fz, fr);
}

/* 0249BE90 */
void daStone::Act_c::damaged(s32 kind) {
    u32 item = parameter(this, daStone::PRM_ITEMNO_W, daStone::PRM_ITEMNO_S),
        save = parameter(this, daStone::PRM_ITEMSAVE_W, daStone::PRM_ITEMSAVE_S);
    StoneLocal<csXyz> rotation;
    s16 yaw = static_cast<s16>(this->home.angle.y);
    stone_csXyz__csXyz(rotation.get(), 0, yaw, 0);
    s8 room = static_cast<s8>(this->home.roomNo);
    stone_fopAcM_createItemFromTable(static_cast<void*>(&this->current.pos), item, save, room, nullptr,
                                     rotation.get(), kind, nullptr);
    stone_fopAcM_cancelCarryNow(this);
    this->eff_break();
    const daStone::Data_c* record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    if (static_cast<s16>(record->shockPower) > 0) {
        u32 play = address(stone_getPlay());
        record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
        f32 zero = static_cast<f32>(stone_zero()), up = static_cast<f32>(stone_one());
        StoneLocal<cXyz> direction;
        direction->z = zero;
        s16 power = static_cast<s16>(record->shockPower);
        direction->x = zero;
        direction->y = up;
        stone_dVibration_c__StartShock(static_cast<void*>(&gabi::object_at<StonePlayView>(play).vibration[0]),
                                       power, -33, direction.get());
    }
    const daStone::Data_c *first = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max"),
                          *second = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    u8 secondSoundParameter = static_cast<u8>(second->breakSoundEnvironment),
       firstSoundParameter = static_cast<u8>(first->breakSoundParameter);
    this->set_senv(firstSoundParameter, secondSoundParameter);
    record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    room = static_cast<s8>(this->current.roomNo);
    u32 sound = static_cast<u32>(record->breakSound);
    s32 reverb = stone_dComIfGp_getReverb(room);
    stone_startActorSound(sound, static_cast<void*>(&this->eyePos), 0, reverb);
    record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    if (static_cast<u32>(record->flags) & daStone::DATAFLG_SAVE_SWITCH) {
        u32 sw = parameter(this, daStone::PRM_SWSAVE_W, daStone::PRM_SWSAVE_S);
        if (sw != 255) {
            u32 saveInfo = static_cast<u32>(stone_saveAddress());
            room = static_cast<s8>(this->home.roomNo);
            stone_dSv_info_c__onSwitch(static_cast<void*>(stone_pointer<StoneSaveInfo>(saveInfo + 0x20)), sw,
                                       room);
        }
    }
}

/* 0249C0E4 */
s32 daStone::Act_c::damage_cc_proc() {
    auto cylinder = static_cast<void*>(&this->mCyl);
    if (stone_dCcD_GObjInf__ChkAtHit(cylinder)) {
        stone_dCcD_GObjInf__ClrAtHit(cylinder);
        stone_daStone__Act_c__damaged(this, 3);
        return 1;
    }
    if (!stone_dCcD_GObjInf__ChkTgHit(cylinder))
        return 0;
    u32 hit = address(stone_dCcD_GObjInf__GetTgHitObj(cylinder));
    u32 type = static_cast<u32>(this->mType);
    if (hit) {
        u32 attack = static_cast<u32>(gabi::object_at<StoneAttack>(hit).type);
        if ((type != 3 && (attack & 0x20)) || (attack & 0x10000)) {
            stone_daStone__Act_c__damaged(this, 7);
            stone_dCcD_GObjInf__ClrTgHit(cylinder);
            return 1;
        }
    }
    if (type >= daStone::TYPE_COUNT) {
        stone_assertData("d_a_stone.cpp", 0x306, "M_type >= 0 && M_type < Type_Max");
        type = static_cast<u32>(this->mType);
    }
    s8 room = static_cast<s8>(this->current.roomNo);
    u32 sound = static_cast<u32>(stone_dataType(type).hitSound);
    stone_daObj__HitSeStart(static_cast<void*>(&this->eyePos), room, cylinder, sound);
    const daStone::Data_c *first = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max"),
                          *second = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    u8 secondSoundParameter = static_cast<u8>(second->hitSoundEnvironment),
       firstSoundParameter = static_cast<u8>(first->hitSoundParameter);
    this->set_senv(firstSoundParameter, secondSoundParameter);
    stone_daObj__HitEff_hibana(this, cylinder);
    stone_dCcD_GObjInf__ClrTgHit(cylinder);
    return 0;
}

/* 0249C994 */
s32 daStone::Act_c::damage_bg_proc() {
    bool grounded = (static_cast<u32>(this->mAcch.m_flags) & 0x20) != 0;
    u32 water = this->chk_sink_water();
    s32 lava = this->chk_sink_lava();
    s32 mode = static_cast<s32>(this->mMode);
    if (mode == daStone::MODE_WAIT || mode == daStone::MODE_DROP) {
        if (mode == daStone::MODE_WAIT && grounded)
            return 0;
        if (water) {
            this->se_fall_water();
            this->eff_hit_water_splash();
            if (mode == daStone::MODE_WAIT)
                this->init_rot_clean();
            this->mode_sink_init();
        } else if (lava) {
            this->se_fall_lava();
            this->eff_hit_lava_splash();
            const daStone::Data_c* record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
            if (static_cast<u32>(record->flags) & daStone::DATAFLG_SINK_LAVA) {
                if (mode == daStone::MODE_WAIT)
                    this->init_rot_clean();
                this->mode_sink_init();
            } else {
                stone_daStone__Act_c__damaged(this, 2);
                return 1;
            }
        }
    } else if (mode == daStone::MODE_SINK && (grounded || !(water | u32(lava)))) {
        this->mode_wait_init();
    }
    return 0;
}

/* 0249CE6C */
s32 daStone::Act_c::damage_bg_proc_directly() {
    u32 flags = static_cast<u32>(this->mAcch.m_flags);
    s32 mode = static_cast<s32>(this->mMode);
    bool grounded = flags & dBgS_Acch::GROUND_HIT;
    s32 damaged = 0;
    if (mode == daStone::MODE_WAIT && grounded) {
        f32 y;
        if (flags & dBgS_Acch::GROUND_LANDING) {
            const daStone::Data_c* record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
            f32 previous = static_cast<f32>(this->mPreviousHeight);
            y = static_cast<f32>(this->current.pos.y);
            f32 fall = previous - y, limit = static_cast<f32>(record->breakFallHeight);
            if (fall > limit) {
                stone_daStone__Act_c__damaged(this, 2);
                damaged = 1;
                y = static_cast<f32>(this->current.pos.y);
            }
        } else
            y = static_cast<f32>(this->current.pos.y);
        s8 delay = static_cast<s8>(this->mLandingDelay);
        this->mPreviousHeight = static_cast<f32>(y);
        if (delay > 0) {
            this->mLandingDelay = static_cast<u8>(u8(delay - 1));
            return damaged;
        }
    } else if (mode == daStone::MODE_DROP) {
        u32 water = this->chk_sink_water();
        bool roof = flags & dBgS_Acch::ROOF_HIT, wall = flags & dBgS_Acch::WALL_HIT;
        s32 lava = this->chk_sink_lava();
        if (grounded) {
            stone_daStone__Act_c__damaged(this, 2);
            damaged = 1;
        } else if (wall || roof) {
            stone_daStone__Act_c__damaged(this, 3);
            damaged = 1;
        }
        if (grounded || wall || roof || (water | u32(lava)))
            this->cam_lockoff();
    }
    s8 delay = static_cast<s8>(this->mLandingDelay);
    if (delay > 0)
        this->mLandingDelay = static_cast<u8>(u8(delay - 1));
    else if (grounded) {
        if (!static_cast<u8>(this->mGrounded) && static_cast<s32>(this->mMode) == daStone::MODE_WAIT) {
            if (!damaged) {
                const daStone::Data_c* record =
                    data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
                u32 soundId = static_cast<u32>(record->landSound);
                u32 play = address(stone_getPlay());
                s32 sound = stone_dBgS__GetMtrlSndId(
                    static_cast<void*>(&gabi::object_at<StonePlayView>(play).background[0]),
                    static_cast<void*>(static_cast<cBgS_PolyInfo*>(&this->mAcch.m_gnd)));
                s8 room = static_cast<s8>(this->current.roomNo);
                s32 reverb = stone_dComIfGp_getReverb(room);
                stone_startActorSound(soundId, static_cast<void*>(&this->eyePos), sound, reverb);
                this->eff_land_smoke();
            }
            this->mGrounded = static_cast<u8>(1);
        }
    } else
        this->mGrounded = static_cast<u8>(0);
    return damaged;
}

/* 0249AF50 */
void daStone::Act_c::set_mtx() {

    s16 zero = static_cast<s16>(stone_zeroAngle()), tilt = static_cast<s16>(this->mTilt);
    bool tilted = tilt != zero;
    f32 x = static_cast<f32>(this->current.pos.x), z = static_cast<f32>(this->current.pos.z),
        y = static_cast<f32>(this->current.pos.y);
    auto matrix = static_cast<void*>(&stone_matrix());
    stone_PSMTXTrans(matrix, x, y, z);
    f32 noTranslation = static_cast<f32>(stone_zero());
    if (tilted) {
        const daStone::Data_c* record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
        stone_mDoMtx_stack_c__transM(noTranslation, static_cast<f32>(record->bottomOffset), noTranslation);
        tilt = static_cast<s16>(this->mTilt);
        f32 radians = f32(tilt) * static_cast<f32>(stone_angleToRadians());
        u32 rotation = static_cast<u32>(stone_quaternionAddress());
        stone_C_QUATRotAxisRad(static_cast<void*>(stone_pointer<Quaternion>(rotation)),
                               static_cast<void*>(&stone_axis()), radians);
        rotation = static_cast<u32>(stone_quaternionAddress());
        stone_mDoMtx_stack_c__quatM(static_cast<void*>(stone_pointer<Quaternion>(rotation)));
    }
    s16 rz = static_cast<s16>(this->shape_angle.z), rx = static_cast<s16>(this->shape_angle.x),
        ry = static_cast<s16>(this->shape_angle.y);
    stone_mDoMtx_ZXYrotM(matrix, rx, ry, rz);
    if (tilted) {
        const daStone::Data_c* record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
        f32 height = static_cast<f32>(record->bottomOffset);
        stone_mDoMtx_stack_c__transM(noTranslation, -height, noTranslation);
    }
    f32 values[12];
    for (u32 i = 0; i != 12; ++i)
        values[i] = static_cast<f32>(stone_matrixValues()[i]);
    u32 model = static_cast<u32>(this->mpModel.v);
    // GHS snapshots the full matrix before writing the model transform.
    for (u32 i : {5u, 6u, 7u, 9u, 10u, 0u, 1u, 2u, 3u, 4u, 8u, 11u})
        reinterpret_cast<be<f32>*>(&gabi::object_at<J3DModel>(model).mBaseTransformMtx)[i] = values[i];
}

/* 0249B128 */
void daStone::Act_c::init_mtx() {
    u32 type = static_cast<u32>(this->mType);
    u32 model = static_cast<u32>(this->mpModel.v);
    if (type >= daStone::TYPE_COUNT) {
        stone_assertData("d_a_stone.cpp", 0x306, "M_type >= 0 && M_type < Type_Max");
        type = static_cast<u32>(this->mType);
    }
    f32 scale = static_cast<f32>(stone_dataType(type).modelScale);
    StoneLocal<cXyz> result;
    stone_cXyz____ml(static_cast<void*>(&this->scale), result.get(), scale);
    f32 x = result->x, z = result->z, y = result->y;
    gabi::object_at<J3DModel>(model).mBaseScale.z = z;
    gabi::object_at<J3DModel>(model).mBaseScale.x = x;
    gabi::object_at<J3DModel>(model).mBaseScale.y = y;
    stone_daStone__Act_c__set_mtx(this);
}

/* 0249D670 */
void daStone::Act_c::init_rot_throw() {
    const daStone::Data_c* record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    stone_cSAngle_setValue(static_cast<void*>(&this->mRoll), static_cast<s16>(record->throwRoll));
    f32 random = stone_cM_rnd();
    stone_cSAngle____amu(static_cast<void*>(&this->mRoll), random);
    f32 angle = stone_cM_rndFX(static_cast<f32>(stone_randomAngleRange()));
    s16 yaw = s16(u16(gabi::ftoi(angle)));
    stone_cSAngle_setValue(static_cast<void*>(&this->mPitch), yaw);
    record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    stone_cSAngle_setValue(static_cast<void*>(&this->mPitchSpeed), static_cast<s16>(record->throwPitchSpeed));
    stone_cSAngle_assign(static_cast<void*>(&this->mTilt),
                         static_cast<void*>(const_cast<be<s16>*>(&stone_zeroAngle())));
    record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    stone_cSAngle_setValue(static_cast<void*>(&this->mTiltSpeed), static_cast<s16>(record->throwTiltSpeed));
    yaw = static_cast<s16>(this->current.angle.y);
    stone_cSAngle_setValue(static_cast<void*>(&this->mYaw), yaw);
}

/* 0249C578 */
void daStone::Act_c::init_rot_clean() {
    for (auto* angle : {&this->mRoll, &this->mPitch, &this->mPitchSpeed, &this->mTilt, &this->mTiltSpeed})
        stone_cSAngle_assign(angle, static_cast<void*>(const_cast<be<s16>*>(&stone_zeroAngle())));
    s16 yaw = static_cast<s16>(this->current.angle.y);
    stone_cSAngle_setValue(static_cast<void*>(&this->mYaw), yaw);
}

/* 0249D948 */
void daStone::Act_c::set_drop_rot() {
    update_rotation(this, &daStone::Data_c::dropLinearDrag, &daStone::Data_c::dropQuadraticDrag,
                    "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
}

/* 0249DD68 */
void daStone::Act_c::set_sink_rot() {
    update_rotation(this, &daStone::Data_c::sinkLinearDrag, &daStone::Data_c::sinkQuadraticDrag,
                    "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
}

/* 0249BC7C */
void daStone::Act_c::eff_break() {
    u32 type = static_cast<u32>(this->mType);
    f32 x = static_cast<f32>(this->current.pos.x);
    if (type >= daStone::TYPE_COUNT) {
        stone_assertData("d_a_stone.cpp", 0x306, "M_type >= 0 && M_type < Type_Max");
        type = static_cast<u32>(this->mType);
    }
    f32 y = static_cast<f32>(this->current.pos.y),
        offset = static_cast<f32>(stone_dataType(type).bottomOffset);
    StoneLocal<cXyz> position;
    position->x = x;
    f32 z = static_cast<f32>(this->current.pos.z);
    position->z = z;
    position->y = y + offset;
    u32 play = address(stone_getPlay());
    u32 particles = static_cast<u32>(gabi::object_at<StonePlayView>(play).particles);
    auto color = static_cast<void*>(&this->tevStr.mColorK0);
    stone_dPa_control_c__set(static_cast<void*>(stone_pointer<StoneParticleManager>(particles)), 0, 0x3E3,
                             position.get(), nullptr, nullptr, 255, nullptr, -1, color, color, nullptr);
    u32 resources = static_cast<u32>(stone_resourceAddress());
    StoneLocal<SafeString> firstName;
    firstName->mStringTop = gabi::guest_str("Always");
    firstName->__vtbl = stone_tu::safeStringVtable;
    void* model = stone_getObjectResource_2(static_cast<void*>(stone_pointer<StoneResources>(resources)),
                                            firstName.get(), 0x30);
    resources = static_cast<u32>(stone_resourceAddress());
    StoneLocal<SafeString> secondName;
    secondName->mStringTop = gabi::guest_str("Always");
    secondName->__vtbl = stone_tu::safeStringVtable;
    void* animation = stone_getObjectResource_2(static_cast<void*>(stone_pointer<StoneResources>(resources)),
                                                secondName.get(), 0x66);
    play = address(stone_getPlay());
    particles = static_cast<u32>(gabi::object_at<StonePlayView>(play).particles);
    void* emitter = stone_dPa_control_c__set(
        static_cast<void*>(stone_pointer<StoneParticleManager>(particles)), 0, 0x3E2, position.get(), nullptr,
        nullptr, 255, nullptr, -1, nullptr, nullptr, nullptr);
    if (emitter) {
        const daStone::Data_c* record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
        u16 colorIndex = static_cast<u16>(record->textureIndex);
        void* instance = stone_dPa_J3DmodelEmitter_c__dPa_J3DmodelEmitter_c(
            nullptr, emitter, model, 1, static_cast<void*>(&this->tevStr), animation, colorIndex, nullptr);
        if (instance) {
            play = address(stone_getPlay());
            particles = static_cast<u32>(gabi::object_at<StonePlayView>(play).particles);
            u32 list = static_cast<u32>(gabi::object_at<StoneParticleManager>(particles).modelEmitters);
            stone_cLs_Addition(static_cast<void*>(stone_pointer<StoneParticleList>(list)), instance);
        }
    }
}

/* 0249C4C4 */
void daStone::Act_c::eff_hit_water_splash() {
    StoneLocal<cXyz> position;
    f32 z = static_cast<f32>(this->current.pos.z), y = static_cast<f32>(this->mSurfaceHeights.y);
    u32 type = static_cast<u32>(this->mType);
    f32 x = static_cast<f32>(this->current.pos.x);
    position->z = z;
    position->x = x;
    position->y = y;
    if (type >= daStone::TYPE_COUNT) {
        stone_assertData("d_a_stone.cpp", 0x306, "M_type >= 0 && M_type < Type_Max");
        type = static_cast<u32>(this->mType);
    }
    const daStone::Data_c* first = &stone_dataType(type);
    const daStone::Data_c* second = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    f32 scale = static_cast<f32>(first->waterSplashScale),
        height = static_cast<f32>(second->waterSplashHeight);
    stone_fopKyM_createWpillar(position.get(), 0, scale, height);
}

/* 0249C914 */
void daStone::Act_c::eff_hit_lava_splash() {
    StoneLocal<cXyz> position;
    f32 x = static_cast<f32>(this->current.pos.x), y = static_cast<f32>(this->mSurfaceHeights.x);
    position->x = x;
    u32 type = static_cast<u32>(this->mType);
    f32 z = static_cast<f32>(this->current.pos.z);
    position->y = y;
    position->z = z;
    if (type >= daStone::TYPE_COUNT) {
        stone_assertData("d_a_stone.cpp", 0x306, "M_type >= 0 && M_type < Type_Max");
        type = static_cast<u32>(this->mType);
    }
    f32 scale = static_cast<f32>(stone_dataType(type).lavaSplashScale);
    stone_fopKyM_createMpillar(position.get(), scale);
}

/* 0249CE00 */
void daStone::Act_c::eff_land_smoke() {
    const daStone::Data_c* record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    f32 scale = static_cast<f32>(record->landSmokeScale);
    stone_daObj__make_land_effect(this, static_cast<void*>(&this->mAcch.m_gnd), scale);
}

/* 0249CB98 */
void daStone::Act_c::bg_crr_water() {

    f32 x = static_cast<f32>(this->current.pos.x), z = static_cast<f32>(this->current.pos.z),
        water = static_cast<f32>(this->mAcch.m_wtr.m_height);
    s32 seaHit = stone_daSea_ChkArea(x, z);
    z = static_cast<f32>(this->current.pos.z);
    x = static_cast<f32>(this->current.pos.x);
    f32 sea = stone_daSea_calcWave(x, z);
    const daStone::Data_c* record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    u32 flags = static_cast<u32>(this->mAcch.m_flags);
    f32 y = static_cast<f32>(this->current.pos.y), offset = static_cast<f32>(record->bottomOffset);
    f32 bottom = y + offset;
    bool waterHit = (flags & dBgS_Acch::WATER_IN) && bottom < water;
    f32 previous = static_cast<f32>(this->mSurfaceHeights.y);
    bool seaInside = seaHit && bottom < sea;
    this->mSurfaceHeights.z = static_cast<f32>(previous);
    if (waterHit && (!seaInside || water > sea)) {
        this->mSurfaceHeights.y = static_cast<f32>(water);
        this->mSeaHit = static_cast<u8>(0);
        this->mWaterHit = static_cast<u8>(1);
    } else if (seaInside) {
        this->mSurfaceHeights.y = static_cast<f32>(sea);
        this->mWaterHit = static_cast<u8>(1);
        this->mSeaHit = static_cast<u8>(1);
    } else {
        this->mWaterHit = static_cast<u8>(0);
        f32 invalid = static_cast<f32>(stone_invalidHeight());
        this->mSeaHit = static_cast<u8>(0);
        this->mSurfaceHeights.y = static_cast<f32>(invalid);
    }
}

/* 0249CD04 */
void daStone::Act_c::bg_crr_lava() {
    u32 type = static_cast<u32>(this->mType);
    f32 x = static_cast<f32>(this->current.pos.x);
    if (type >= daStone::TYPE_COUNT) {
        stone_assertData("d_a_stone.cpp", 0x306, "M_type >= 0 && M_type < Type_Max");
        type = static_cast<u32>(this->mType);
    }
    f32 z = static_cast<f32>(this->current.pos.z), oldY = static_cast<f32>(this->old.pos.y);
    f32 offset = static_cast<f32>(stone_dataType(type).bottomOffset);
    f32 height = (oldY + offset) + static_cast<f32>(stone_one());
    this->mGndChkYogan.m_pos.x = static_cast<f32>(x);
    this->mGndChkYogan.m_pos.z = static_cast<f32>(z);
    this->mGndChkYogan.m_pos.y = static_cast<f32>(height);
    void* play = stone_getPlay();
    f32 ground = stone_cBgS__GroundCross(
        static_cast<void*>(&gabi::object_at<StonePlayView>(address(play)).background[0]),
        static_cast<void*>(&this->mGndChkYogan));
    this->mSurfaceHeights.x = static_cast<f32>(ground);
}

/* 0249C28C */
s32 daStone::Act_c::chk_sink_lava() {
    const daStone::Data_c* record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    f32 minimum = static_cast<f32>(stone_lavaOffsetLimit());
    f32 offset = static_cast<f32>(record->bottomOffset);
    if (offset < minimum) {
        // The second access asserts again and can reload an externally changed
        // type.
        record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
        minimum = static_cast<f32>(record->bottomOffset);
    }
    f32 threshold = static_cast<f32>(this->current.pos.y) + minimum;
    return static_cast<f32>(this->mSurfaceHeights.x) > threshold;
}

/* 0249C284 */
u32 daStone::Act_c::chk_sink_water() { return static_cast<u8>(this->mWaterHit); }

/* 0249DCF8 */
void daStone::Act_c::water_tention() {
    if (this->chk_sink_water()) {
        f32 current = static_cast<f32>(this->mSurfaceHeights.y);
        f32 invalid = static_cast<f32>(stone_invalidHeight());
        if (current != invalid) {
            f32 previous = static_cast<f32>(this->mSurfaceHeights.z);
            if (previous != invalid) {
                f32 negativeRate = static_cast<f32>(stone_fallingWaterRate());
                f32 difference = current - previous;
                f32 positiveRate = static_cast<f32>(stone_risingWaterRate());
                f32 rate = difference >= 0 ? positiveRate : negativeRate;
                f32 y = static_cast<f32>(this->current.pos.y);
                this->current.pos.y = static_cast<f32>(gabi::fmadds(difference, rate, y));
            }
        }
    }
}

/* 0249C358 */
void daStone::Act_c::se_fall_water() {

    u32 water = static_cast<u8>(this->mSeaHit) ? 0 : gabi::ea(&this->mAcch.m_wtr);
    s32 sound = 0x13;
    for (u32 polygon : {water, gabi::ea(static_cast<cBgS_PolyInfo*>(&this->mAcch.m_gnd))}) {
        if (polygon && static_cast<u16>(gabi::object_at<cBgS_PolyInfo>(polygon).mBgIndex) < 0x100) {
            u32 play = address(stone_getPlay());
            sound = stone_dBgS__GetMtrlSndId(
                static_cast<void*>(&gabi::object_at<StonePlayView>(play).background[0]),
                static_cast<void*>(stone_pointer<cBgS_PolyInfo>(polygon)));
            break;
        }
    }
    const daStone::Data_c* record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    s8 room = static_cast<s8>(this->current.roomNo);
    u32 soundId = static_cast<u32>(record->waterSound);
    s32 reverb = stone_dComIfGp_getReverb(room);
    stone_startActorSound(soundId, static_cast<void*>(&this->eyePos), sound, reverb);
    const daStone::Data_c *first = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max"),
                          *second = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    u8 secondSoundParameter = static_cast<u8>(second->fallSoundEnvironment),
       firstSoundParameter = static_cast<u8>(first->fallSoundParameter);
    this->set_senv(firstSoundParameter, secondSoundParameter);
}

/* 0249C7C0 */
void daStone::Act_c::se_fall_lava() {
    s32 sound = 0x17;
    for (auto* polygon : {static_cast<cBgS_PolyInfo*>(&this->mGndChkYogan),
                          static_cast<cBgS_PolyInfo*>(&this->mAcch.m_gnd)}) {
        if (static_cast<u16>(polygon->mBgIndex) < 0x100) {
            u32 play = address(stone_getPlay());
            sound = stone_dBgS__GetMtrlSndId(
                static_cast<void*>(&gabi::object_at<StonePlayView>(play).background[0]),
                static_cast<void*>(polygon));
            break;
        }
    }
    const daStone::Data_c* record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    s8 room = static_cast<s8>(this->current.roomNo);
    u32 soundId = static_cast<u32>(record->lavaSound);
    s32 reverb = stone_dComIfGp_getReverb(room);
    stone_startActorSound(soundId, static_cast<void*>(&this->eyePos), sound, reverb);
    const daStone::Data_c *first = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max"),
                          *second = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
    u8 secondSoundParameter = static_cast<u8>(second->fallSoundEnvironment),
       firstSoundParameter = static_cast<u8>(first->fallSoundParameter);
    this->set_senv(firstSoundParameter, secondSoundParameter);
}

/* 0249BE4C */
void daStone::Act_c::set_senv(u32 firstSoundParameter, u32 secondSoundParameter) {
    StoneLocal<cXyz> position;
    f32 x = static_cast<f32>(this->current.pos.x);
    f32 y = static_cast<f32>(this->current.pos.y);
    position->x = x;
    f32 z = static_cast<f32>(this->current.pos.z);
    u32 owner = static_cast<u32>(this->mBsPcId);
    position->y = y;
    position->z = z;
    stone_dKy_Sound_set(position.get(), firstSoundParameter, owner, secondSoundParameter);
}

/* 0249CDC4 */
void daStone::Act_c::cam_lockoff() {
    u32 play = address(stone_getPlay());
    u32 camera = static_cast<u32>(gabi::object_at<StonePlayView>(play).camera);
    u32 owner = static_cast<u32>(this->mBsPcId);
    stone_dCamera_c__ForceLockOff(static_cast<void*>(&gabi::object_at<StoneCamera>(camera).camera), owner);
}

/* 0249D2BC */
s32 daStone::Act_c::_execute() {

    this->cull_set_move();
    bool skip = false;
    if (!static_cast<u8>(this->mMovingGround) && static_cast<s32>(this->mMode) == daStone::MODE_WAIT) {
        u32 flags = static_cast<u32>(this->mAcch.m_flags);
        if ((flags & dBgS_Acch::GROUND_HIT) && !(flags & dBgS_Acch::GROUND_LANDING) &&
            parameter(this, daStone::PRM_CULL_W, daStone::PRM_CULL_S) &&
            (static_cast<u32>(this->actor_condition) & 4) && stone_fopAcM_cullingCheck(this) &&
            parameter(this, daStone::PRM_MOVEBG_W, daStone::PRM_MOVEBG_S) != 1)
            skip = true;
    }
    if (!skip) {
        this->mMovingGround = static_cast<u8>(0);
        bool remove = true;
        if (!this->damage_cc_proc() && !this->damage_bg_proc()) {
            u8 delay = static_cast<u8>(this->mMassDelay);
            if (delay)
                this->mMassDelay = static_cast<u8>(u8(delay - 1));
            if (this->mode_proc_call()) {
                remove = false;
                this->set_mtx();
                this->mStts.mRoomId = static_cast<u8>(static_cast<u8>(this->current.roomNo));
                stone_dCcD_Cyl__MoveCAtTg(static_cast<void*>(&this->mCyl),
                                          static_cast<void*>(&this->current.pos));
                u32 play = address(stone_getPlay());
                stone_cCcS__Set(static_cast<void*>(&gabi::object_at<StonePlayView>(play).collision[0]),
                                static_cast<void*>(&this->mCyl));
                s32 mode = static_cast<s32>(this->mMode);
                if (mode == daStone::MODE_DROP || mode == daStone::MODE_SINK ||
                    static_cast<u8>(this->mMassDelay)) {
                    play = address(stone_getPlay());
                    stone_dCcMassS_Mng__Set(static_cast<void*>(&gabi::object_at<StonePlayView>(play).mass[0]),
                                            static_cast<void*>(&this->mCyl), 3);
                }
                u32 type = static_cast<u32>(this->mType);
                this->attention_info.position.x = static_cast<f32>(static_cast<f32>(this->current.pos.x));
                if (type >= daStone::TYPE_COUNT) {
                    stone_assertData("d_a_stone.cpp", 0x306, "M_type >= 0 && M_type < Type_Max");
                    type = static_cast<u32>(this->mType);
                }
                f32 y = static_cast<f32>(this->current.pos.y),
                    x = static_cast<f32>(this->attention_info.position.x);
                f32 height = static_cast<f32>(stone_dataType(type).attentionHeight),
                    z = static_cast<f32>(this->current.pos.z);
                y += height;
                this->eyePos.x = static_cast<f32>(x);
                this->attention_info.position.z = static_cast<f32>(z);
                this->attention_info.position.y = static_cast<f32>(y);
                this->eyePos.z = static_cast<f32>(z);
                this->eyePos.y = static_cast<f32>(y);
            }
        }
        u32 model = 0;
        if (static_cast<s32>(this->mMode) == daStone::MODE_CARRY && static_cast<u8>(this->mGrabStarted))
            model = static_cast<u32>(this->mpModel.v);
        this->model = static_cast<u32>(model);
        if (remove)
            stone_fopAcM_delete(this);
    }
    this->cull_set_draw();
    return 1;
}

/* 0249D4D0 */
s32 daStone::Act_c::_draw() {
    // HD: The verified HD draw omits the GC simple-shadow submission.
    if (parameter(this, daStone::PRM_MOVEBG_W, daStone::PRM_MOVEBG_S) != 1) {
        const daStone::Data_c* record = data(this, "d_a_stone.cpp", "M_type >= 0 && M_type < Type_Max");
        u32 lightType = (static_cast<u32>(record->flags) >> 2) & 1;
        void* environment = stone_getEnvironmentLight();
        stone_dScnKy_env_light_c__settingTevStruct(environment, lightType,
                                                   static_cast<void*>(&this->current.pos),
                                                   static_cast<void*>(&this->tevStr));
        environment = stone_getEnvironmentLight();
        u32 model = static_cast<u32>(this->mpModel.v);
        stone_setLightTevColorType(environment, static_cast<void*>(stone_pointer<J3DModel>(model)),
                                   static_cast<void*>(&this->tevStr));
        model = static_cast<u32>(this->mpModel.v);
        stone_mDoExt_modelUpdateDL(static_cast<void*>(stone_pointer<J3DModel>(model)), 0);
    }
    return 1;
}

/* 0249DFE0 */
s32 daStone_Create(Act_c* actor) { return stone_daStone__Act_c___create(actor); }

/* 0249DFE4 */
s32 daStone_Delete(Act_c* actor) { return actor->_delete(); }

/* 0249DFE8 */
s32 daStone_Execute(Act_c* actor) { return stone_daStone__Act_c___execute(actor); }

/* 0249DFEC */
s32 daStone_Draw(Act_c* actor) { return actor->_draw(); }

/* 0249E188 */
s32 daStone_IsDelete(void* actor) { return 1; }

// HD: Compiler-generated helpers and the preserved parameter extractor.
/* 0249DFF0 */
void stone_staticInit() {
    stone_staticState().words[3] = 0;
    stone_staticState().words[2] = 0;
    stone_staticState().words[1] = 0;
    stone_staticState().words[0] = 0;
    stone_register_global_object(stone_staticRecordA());
    f32 a = static_cast<f32>(stone_staticFirstInitial()), b = static_cast<f32>(stone_staticSecondInitial());
    stone_staticState().first = a;
    stone_staticState().second = b;
    stone_initializeStaticFlag(static_cast<void*>(&stone_staticState().firstFlag));
    stone_register_global_object(stone_staticRecordB());
    stone_initializeSecondStaticFlag(static_cast<void*>(&stone_staticState().secondFlag));
    stone_register_global_object(stone_staticRecordC());
    f32 x = static_cast<f32>(stone_baseZ().x), y = static_cast<f32>(stone_baseZ().y);
    stone_axis().x = x;
    f32 z = static_cast<f32>(stone_baseZ().z);
    stone_axis().y = y;
    stone_axis().z = z;
}

/* 0249E0A4 */
void stone_deleteStorage(void* object, s32 flags) {
    if (object && (u32(flags) & 1))
        stone_dl(object);
}

/* 0249E0B8 */
void stone_emptyVirtual() {}

/* 0249E0BC */
void stone_destructActor(Act_c* actor, s32 flags) {
    if (gabi::ea(actor) != 0) {

        stone_dCcD_Cyl_destruct(static_cast<void*>(&actor->mCyl), 2);
        stone_dCcD_Stts_destruct(static_cast<void*>(&actor->mStts), 2);
        actor->mGndChkYogan.cBgS_PolyInfo::__vtbl = static_cast<u32>(stone_tu::groundPolygonDestructorVtable);
        actor->mGndChkYogan.dBgS_PolyPassChk::__vtbl = static_cast<u32>(stone_tu::groundPassDestructorVtable);
        actor->mGndChkYogan.dBgS_GrpPassChk::__vtbl = static_cast<u32>(stone_tu::groundGroupDestructorVtable);
        stone_cBgS_GndChk_destruct(static_cast<void*>(&actor->mGndChkYogan), 0);
        stone_cM3dGAab_destruct(
            static_cast<void*>(&gabi::object_at<StoneAcchCircleView>(gabi::ea(&actor->mAcchCir)).bounds), 2);
        actor->mAcch.dBgS_GrpPassChk::__vtbl = static_cast<u32>(stone_tu::acchGroupVtable);
        actor->mAcch.dBgS_PolyPassChk::__vtbl = static_cast<u32>(stone_tu::acchPassVtable);
        stone_dBgS_Acch_destruct(static_cast<void*>(&actor->mAcch), 0);
        stone_fopAc_ac_c___fopAc_ac_c(actor, 0);
        if (u32(flags) & 1)
            stone_dl_2(actor);
    }
}

/* 0249E190 */
u32 parameter(Act_c* actor, s32 width, s32 shift) {
    // HD: PPC parameter shifts preserve masked out-of-range counts.
    u32 value = static_cast<u32>(actor->mParameters);
    u32 leftCount = u32(width) & 63u, rightCount = u32(shift) & 63u;
    u32 mask = (leftCount < 32 ? 1u << leftCount : 0u) - 1u;
    return (rightCount < 32 ? value >> rightCount : 0u) & mask;
}
// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
str 1003DAF8 in 0249BC7C
str 1003DA58 in 0249B2C4
data M_data 1003E044
str 1003D8E8 in 0249AB20
str 1003D8F8 in 0249AB20
str 1003DBA0 in 0249C28C
str 1003DBB0 in 0249C28C
str 1003DD74 in 0249CE00
str 1003DD84 in 0249CE00
str 1003DE30 in 0249D4D0
str 1003DE40 in 0249D4D0
str 1003D930 in 0249ABE8
str 1003D940 in 0249ABE8
str 1003DA90 in 0249BA60
str 1003DAA0 in 0249BA60
str 1003D974 in 0249AE00
str 1003D984 in 0249AE00
str 1003DAC4 in 0249BB2C
str 1003DAD4 in 0249BB2C
str 1003D9B0 in 0249AF50
str 1003D9C0 in 0249AF50
str 1003DE64 in 0249D588
str 1003DE74 in 0249D588
str 1003DC08 in 0249C4C4
str 1003DC18 in 0249C4C4
str 1003DF38 in 0249DB04
str 1003DF48 in 0249DB04
str 1003DF00 in 0249D948
str 1003DF10 in 0249D948
str 1003DF74 in 0249DD68
str 1003DF84 in 0249DD68
str 1003DFA8 in 0249DF24
str 1003DFB8 in 0249DF24
str 1003DE98 in 0249D670
str 1003DEA8 in 0249D670
str 1003DECC in 0249D7A8
str 1003DEDC in 0249D7A8
str 1003DC3C in 0249C5F4
str 1003DC4C in 0249C5F4
str 1003DC70 in 0249C7C0
str 1003DC80 in 0249C7C0
str 1003DD0C in 0249CB98
str 1003DD1C in 0249CB98
str 1003DBD4 in 0249C358
str 1003DBE4 in 0249C358
str 1003DB68 in 0249C0E4
str 1003DB78 in 0249C0E4
str 1003DCD8 in 0249C994
str 1003DCE8 in 0249C994
str 1003DDA8 in 0249CE6C
str 1003DDB8 in 0249CE6C
str 1003DB00 in 0249BC7C
str 1003DB10 in 0249BC7C
str 1003DB34 in 0249BE90
str 1003DB44 in 0249BE90
str 1003DA5C in 0249B2C4
str 1003DA6C in 0249B2C4
data stone_modeProc 1003DDDC
ext 0273AA24 void stone_assertData(const char *, int, const char *)
ext 025BA0C0 int stone_dSv_info_c__isSwitch(void *, unsigned int, signed char)
ext 0255F458 void stone_dKy_Sound_set(cXyz *, unsigned int, unsigned int, unsigned int)
ext 02006638 void stone_cSAngle_assign(void *, void *)
ext 02006584 void stone_cSAngle_setValue(void *, short)
ext 025200D4 void * stone_getPlay()
ext 025052BC void stone_dCamera_c__ForceLockOff(void *, unsigned int)
ext 02311CD8 void stone_daObj__make_land_effect(daStone::Act_c *, void *, float)
ext 0273AF40 void stone_dl(void *)
ext 0201AE48 void stone_cXyz____ml(void *, cXyz *, float)
ext 0249AF50 void stone_daStone__Act_c__set_mtx(daStone::Act_c *)
ext 02555D0C void * stone_getEnvironmentLight()
ext 025626A4 void stone_dScnKy_env_light_c__settingTevStruct(void *, unsigned int, void *, void *)
ext 02562F5C void stone_setLightTevColorType(void *, void *, void *)
ext 025E2DE0 void stone_mDoExt_modelUpdateDL(void *, int)
ext 028F026C void stone_register_global_object(void *)
ext 028ED6F8 void stone_initializeStaticFlag(void *)
ext 028EAB2C void stone_initializeSecondStaticFlag(void *)
ext 02515A70 void stone_dCcD_Cyl_destruct(void *, int)
ext 02515860 void stone_dCcD_Stts_destruct(void *, int)
ext 02008DAC void stone_cBgS_GndChk_destruct(void *, int)
ext 02018034 void stone_cM3dGAab_destruct(void *, int)
ext 024EFD9C void stone_dBgS_Acch_destruct(void *, int)
ext 025D50BC void stone_fopAc_ac_c___fopAc_ac_c(daStone::Act_c *, int)
ext 0273AF40 void stone_dl_2(daStone::Act_c *)
ext 026066C4 void * stone_getObjectResource(void *, SafeString *, unsigned short)
ext 025E38E0 void * stone_mDoExt_J3DModel__create(void *, int, int)
ext 024EFF44 void stone_dBgS_AcchCir__SetWall(void *, float, float)
ext 024F06B4 void stone_dBgS_Acch__Set(void *, void *, void *, daStone::Act_c *, int, void *, void *, void *, void *)
ext 025204C8 void stone_dComIfG_resDelete(void *, const char *)
ext 025D6768 void stone_fopAcM_setCullSizeSphere(daStone::Act_c *, float, float, float, float)
ext 028E93CC void stone_PSMTXTrans(void *, float, float, float)
ext 025F24E0 void stone_mDoMtx_stack_c__transM(float, float, float)
ext 028E9B14 void stone_C_QUATRotAxisRad(void *, void *, float)
ext 025F25CC void stone_mDoMtx_stack_c__quatM(void *)
ext 025F1B48 void stone_mDoMtx_ZXYrotM(void *, short, short, short)
ext 025D6870 void stone_fopAcM_posMoveF(daStone::Act_c *, void *)
ext 023123C0 void stone_daObj__posMoveF_stream(daStone::Act_c *, void *, void *, float, float)
ext 02515F14 void stone_dCcD_Stts__Init(void *, unsigned char, int, daStone::Act_c *)
ext 025DAE64 void stone_fopKyM_createWpillar(cXyz *, int, float, float)
ext 025DAF3C void stone_fopKyM_createMpillar(cXyz *, float)
ext 02312968 void stone_daObj__SetCurrentRoomNo(daStone::Act_c *, void *)
ext 02311F80 void * stone_daObj__get_wind_spd(daStone::Act_c *, float)
ext 0249D948 void stone_set_drop_rot(daStone::Act_c *)
ext 02006760 float stone_cSAngle__Radian(void *)
ext 02019510 int stone_cM_rad2s(float)
ext 0200692C void stone_cSAngle_subtract(void *, int)
ext 020068CC void stone_cSAngle_add(void *, void *)
ext 025F1884 void stone_mDoMtx_YrotS(void *, short)
ext 025F1BF4 void stone_mDoMtx_XrotM(void *, short)
ext 025F1C5C void stone_mDoMtx_ZrotM(void *, short)
ext 025F1C28 void stone_mDoMtx_YrotM(void *, int)
ext 028E9044 void stone_PSMTXMultVecSR(void *, void *, void *)
ext 02019788 float stone_cM_rnd()
ext 020069A0 void stone_cSAngle____amu(void *, float)
ext 02019918 float stone_cM_rndFX(float)
ext 02008974 float stone_cBgS__GroundCross(void *, void *)
ext 028F4384 float stone_sqrt(float)
ext 028E8E64 void stone_PSVECScale(void *, void *, float)
ext 024EECAC int stone_dBgS__GetMtrlSndId(void *, void *)
ext 02520540 int stone_dComIfGp_getReverb(signed char)
ext 025E1A40 void stone_startActorSound(unsigned int, void *, int, int)
ext 0249B2C4 int stone_daStone__Act_c___create(daStone::Act_c *)
ext 0249D2BC int stone_daStone__Act_c___execute(daStone::Act_c *)
ext 0246B6A4 int stone_daSea_ChkArea(float, float)
ext 0246BA0C float stone_daSea_calcWave(float, float)
ext 025160DC int stone_dCcD_GObjInf__ChkAtHit(void *)
ext 02516094 void stone_dCcD_GObjInf__ClrAtHit(void *)
ext 0249BE90 void stone_daStone__Act_c__damaged(daStone::Act_c *, int)
ext 025162A4 int stone_dCcD_GObjInf__ChkTgHit(void *)
ext 02516300 void * stone_dCcD_GObjInf__GetTgHitObj(void *)
ext 0251621C void stone_dCcD_GObjInf__ClrTgHit(void *)
ext 023129C4 void stone_daObj__HitSeStart(void *, signed char, void *, unsigned int)
ext 02312E54 void stone_daObj__HitEff_hibana(daStone::Act_c *, void *)
ext 024F08A8 void stone_dBgS_Acch__CrrPos(void *, void *)
ext 024EEB2C int stone_dBgS__ChkMoveBG_NoDABg(void *, void *)
ext 024EEABC int stone_dBgS__ChkMoveBG(void *, void *)
ext 024EEEB8 unsigned char stone_dBgS__GetPolyColor(void *, void *)
ext 025A847C void * stone_dPa_control_c__set(void *, int, int, cXyz *, void *, void *, int, void *, int, void *, void *, void *)
ext 026066C4 void * stone_getObjectResource_2(void *, SafeString *, int)
ext 025A3BDC void * stone_dPa_J3DmodelEmitter_c__dPa_J3DmodelEmitter_c(void *, void *, void *, int, void *, void *, unsigned short, void *)
ext 0200FE78 void stone_cLs_Addition(void *, void *)
ext 0201A478 void stone_csXyz__csXyz(csXyz *, int, short, int)
ext 025D8120 void stone_fopAcM_createItemFromTable(void *, unsigned int, unsigned int, signed char, void *, csXyz *, int, void *)
ext 025D9D24 void stone_fopAcM_cancelCarryNow(daStone::Act_c *)
ext 025CB374 void stone_dVibration_c__StartShock(void *, short, int, cXyz *)
ext 025B9E38 void stone_dSv_info_c__onSwitch(void *, unsigned int, signed char)
ext 025D6CE8 int stone_fopAcM_cullingCheck(daStone::Act_c *)
ext 025165A4 void stone_dCcD_Cyl__MoveCAtTg(void *, void *)
ext 0200E240 void stone_cCcS__Set(void *, void *)
ext 02516C14 void stone_dCcMassS_Mng__Set(void *, void *, int)
ext 025D57E0 void stone_fopAcM_delete(daStone::Act_c *)
ext 025D4ED0 void stone_fopAc_ac_c__fopAc_ac_c(daStone::Act_c *)
ext 024F0474 void stone_dBgS_Acch__dBgS_Acch(void *)
ext 024EFE94 void stone_dBgS_AcchCir__dBgS_AcchCir(void *)
ext 02008E0C void stone_cBgS_GndChk__cBgS_GndChk(void *)
ext 0200BD2C void stone_cCcD_Stts_construct(void *)
ext 02515DA0 void stone_dCcD_GStts__dCcD_GStts(void *)
ext 02515FB8 void stone_dCcD_GObjInf__dCcD_GObjInf(void *)
ext 02018590 void stone_cM3dGCyl_construct(void *)
ext 020065FC void stone_cSAngle_construct(void *)
ext 02520460 int stone_dComIfG_resLoad(void *, const char *)
ext 025D63E8 int stone_fopAcM_entrySolidHeap(daStone::Act_c *, void *, unsigned int)
ext 02516518 void stone_dCcD_Cyl__Set(void *, void *)
ext 020184DC void stone_cM3dGCyl__SetR(void *, float)
ext 02018428 void stone_cM3dGCyl__SetH(void *, float)
str 1003D9E4 in 0249B128
str 1003D9F4 in 0249B128
str 1003D930 in 0249ABE8
str 1003D964 in 0249ABE8
str 1003D940 in 0249ABE8
str 1003DA18 in 0249B1C0
str 1003DA28 in 0249B1C0
str 1003DC08 in 0249C4C4
str 1003DC18 in 0249C4C4
str 1003DCA4 in 0249C914
str 1003DCB4 in 0249C914
str 1003DD40 in 0249CD04
str 1003DD50 in 0249CD04
str 1003DECC in 0249D7A8
str 1003DEDC in 0249D7A8
str 1003DC3C in 0249C5F4
str 1003DC4C in 0249C5F4
str 1003DB68 in 0249C0E4
str 1003DB78 in 0249C0E4
str 1003DB00 in 0249BC7C
str 1003DB10 in 0249BC7C
str 1003DDFC in 0249D2BC
str 1003DE0C in 0249D2BC
str 1003DA5C in 0249B2C4
str 1003DA6C in 0249B2C4
*/
// clang-format on
