/* Searchlights: GameCube method names, verified HD behavior. */
#include "d/actor/d_a_obj_search.h"
#include <cmath>
const gc_tu_data gc_tu = {.safeString = 0x1002EE6C};

static const f32 l_value_5 = 5.0f;
static const f32 l_value_2_1017 = 2.1017000675201416f;
static const f32 l_value_1_4013 = 1.4012999534606934f;
static const f32 l_value_1_05 = 1.0499999523162842f;
static const f32 l_one = 1.0f;
static const f32 l_value_1_27 = 1.2699999809265137f;
static const f32 l_zero = 0.0f;
static const f32 l_value_16000 = 16000.0f;
static const f32 l_value_1_02 = 1.0199999809265137f;
static const f32 l_value_4000 = 4000.0f;
static const f32 l_value_0_0001043 = 0.00010430000111227855f;
static const f32 l_value_40 = 40.0f;
static const f32 l_value__270 = -270.0f;
static const f32 l_value_30 = 30.0f;
static const f32 l_value_45 = 45.0f;
static const f32 l_value_35 = 35.0f;
static const f32 l_value__1000 = -1000.0f;
static const f32 l_value_1000 = 1000.0f;
static const f32 l_value_1500 = 1500.0f;
static const f32 l_value_460 = 460.0f;
static const f32 l_value_20 = 20.0f;
static const f32 l_value_2000 = 2000.0f;
static const f32 l_value_240 = 240.0f;
static const f32 l_value_60 = 60.0f;
static const f32 l_value_1000000 = 1000000.0f;
static const f32 l_value__600 = -600.0f;
static const f32 l_value_600 = 600.0f;
static const f32 l_value__100 = -100.0f;
static const f32 l_value_9585_62 = 9585.6201171875f;
static const f32 l_opaqueAlpha = 255.0f;
static const f32 l_value_0_1 = 0.10000000149011612f;
static const f32 l_value_80 = 80.0f;
static const f32 l_hundred = 100.0f;
static const f32 l_value_2_5 = 2.5f;
static const f32 l_value_3_5 = 3.5f;
static const f32 l_value_3600 = 3600.0f;
static const f32 l_value_10000 = 10000.0f;
static const f32 l_value__3800 = -3800.0f;
static const f32 l_value_15000 = 15000.0f;
static const f32 l_value_3000 = 3000.0f;
static const f32 l_value_500 = 500.0f;
static const f32 l_value_300 = 300.0f;
static const f32 l_value_200 = 200.0f;
static const f32 l_value__3_141593 = -3.1415927410125732f;
static const f32 l_value_3_141593 = 3.1415927410125732f;
static const f32 l_value_0_3 = 0.30000001192092896f;
static const u32 l_beamCollision[2] = {dRes_SEARCH_DZB_BEAM_A_e, dRes_SEARCH_DZB_BEAM_B_e};
static const f32 l_spotHeight[5] = {-15421.91015625f, -8272.2314453125f, -11118.5498046875f,
                                    -11630.1396484375f, -9072.736328125f};
static const f32 l_spotWidth[5] = {0.01981339044868946f, 0.035924870520830154f, 0.025529399514198303f,
                                   0.0272896196693182f, 0.03900891914963722f};
static const f32 l_spotOpacity[5] = {0.30000001192092896f, 0.30000001192092896f, 0.30000001192092896f,
                                     0.30000001192092896f, 0.30000001192092896f};

static SearchModeEntry l_modeTable[8] = {
    {&daObj_Search::Act_c::modeSearchRndInit, &daObj_Search::Act_c::modeSearchRnd, "MODE_SEARCH_RND"},
    {&daObj_Search::Act_c::modeSearchPathInit, &daObj_Search::Act_c::modeSearchPath, "MODE_SEARCH_PATH"},
    {&daObj_Search::Act_c::modeStopInit, &daObj_Search::Act_c::modeStop, "MODE_STOP"},
    {&daObj_Search::Act_c::modeToSearchInit, &daObj_Search::Act_c::modeToSearch, "MODE_TO_SEARCH"},
    {&daObj_Search::Act_c::modeToStopInit, &daObj_Search::Act_c::modeToStop, "MODE_TO_STOP"},
    {&daObj_Search::Act_c::modeFindInit, &daObj_Search::Act_c::modeFind, "MODE_FIND"},
    {&daObj_Search::Act_c::modeFind2ndInit, &daObj_Search::Act_c::modeFind2nd, "MODE_FIND_2ND"},
    {&daObj_Search::Act_c::modeSearchBdkInit, &daObj_Search::Act_c::modeSearchBdk, "MODE_SEARCH_BDK"}};

// HD: guest globals keep their original guarded storage and load/store order.
static s32 l_eventMode = 0;
static u32 l_lightRenderer = 0;
static f32 l_headerMinusPi = 0.0f;
static f32 l_headerPi = 0.0f;
static u32 l_headerZeroA = 0;
static u32 l_headerZeroB = 0;
static u32 l_headerZeroC = 0;
static u32 l_headerZeroD = 0;
static s32 l_trackingOffsetGuard2 = 0;
static s32 l_trackingOffsetGuard3 = 0;
static s32 l_trackingOffsetGuard4 = 0;
static s32 l_trackingOffsetGuard5 = 0;
static f32 l_birdCircleRange = 0.0f;
static f32 l_birdFanRange = 0.0f;
static s16 l_birdAngle = 0;
static s16 l_birdDirection = 0;
static s16 l_birdRadiusModifier = 0;
static s16 l_birdYawAdjustment = 0;
static f32 l_extraRange = 0.0f;
static f32 l_birdTargetHeight = 0.0f;
static f32 l_birdHeight = 0.0f;
static f32 l_smokeScaleOffset = 0.0f;
static f32 l_smokeGlobalScale = 0.0f;
static s16 l_yawRate = 0;
static s16 l_beamPitchOffset = 0;
static s16 l_randomYawOff = 0;
static s8 l_stayRoom = 0;
static f32 l_farClip = 0.0f;
static f32 l_defaultFarClip = 0.0f;
static u32 l_currentModel = 0;
static u32 l_drawListFirst = 0;
static u32 l_drawListLast = 0;

static const u32 l_capsuleSource[19] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x119, 0x0, 0x0,       0x0,
                                        0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,   0x0, 0x42480000};

static const f32 l_beamTuning0 = 1004.0f;

static const f32 l_beamTuning1 = 376.0f;

static const f32 l_beamTuning2 = 1111.0f;

static const f32 l_beamTuning3 = 1800.0f;

static const f32 l_beamTuning4 = 4500.0f;

static const f32 l_beamTuning5 = 4000.0f;

static const f32 l_beamTuning6 = 10500.0f;

static const f32 l_beamTuning7 = 742.0f;

static const f32 l_beamTuning8 = -402.0f;

static const f32 l_beamTuning9 = -1010.0f;

static cXyz l_trackingOffset0 = {0.0f, 0.0f, 0.0f};

static cXyz l_trackingOffset1 = {0.0f, 0.0f, 0.0f};

static cXyz l_trackingOffset2 = {0.0f, 0.0f, 0.0f};

static cXyz l_trackingOffset3 = {0.0f, 0.0f, 0.0f};

static cXyz l_trackingOffset4 = {0.0f, 0.0f, 0.0f};

static cXyz l_trackingOffset5 = {0.0f, 0.0f, 0.0f};

static s16 l_birdTurnRateFirst;

static s16 l_birdTurnRateOther;

static u32 l_trackingOffsetGuard0;
static u32 l_trackingOffsetGuard1;

namespace {
// HD: Keep live payloads above the outgoing linkage area. Real guest callees
// save their return address at caller SP+4 before consuming these objects.
struct SearchCallerLinkage {
    u8 bytes[32];
};
template <class T> class SearchLocal {
    gabi::Local<T> payload;
    gabi::Local<SearchCallerLinkage> linkage;

  public:
    T* get() const { return payload.get(); }
    T* operator->() const { return get(); }
    T& operator*() const { return *get(); }
    operator T*() const { return get(); }
};
} // namespace
static void vec_copy(cXyz* d, const cXyz* s);
using namespace daObj_Search;
using gabi::load;
using gabi::store;
static u32 address(const void* p) { return gabi::ea(p); }

// Both beam variants share the collision extension and matrix scaling
// algorithm.
static void light_matrix(Act_c* a, bool second) {
    u32 start = address(&a->mBeamStart[second ? 1 : 0]), end = address(&a->mBeamEnd[second ? 1 : 0]),
        hit = address(&a->mWallHit[second ? 1 : 0]);
    SearchLocal<cXyz> origin, far;
    f32 zero = l_zero, reach = l_value_16000;
    far->z = reach;
    origin->z = zero;
    origin->x = zero;
    origin->y = zero;
    far->y = zero;
    far->x = zero;
    u32 block = static_cast<u32>(gabi::at<J3DModel>(address(a->mpSearchModel.get()))->mpMtxBuffer.v);
    u16 flags = static_cast<u16>(gabi::at<J3DMtxBuffer_hd>(block)->mFlags);
    u32 array = static_cast<u32>(gabi::at<J3DMtxBuffer_hd>(block)->mpAnmMtx);
    gabi::at<J3DMtxBuffer_hd>(block)->mFlags = flags | 0x10;
    SearchHD_PSMTXCopy(gabi::at<void>(array + (second ? 0x120 : 0xF0)), mDoMtx_stack_c::get());
    f32 x = (second ? l_beamTuning7 : l_beamTuning0), y = (second ? l_beamTuning8 : l_beamTuning1),
        z = (second ? l_beamTuning9 : l_beamTuning2);
    SearchHD_mDoMtx_stack_c_transM(x, y, z);
    SearchHD_mDoMtx_XYZrotM(mDoMtx_stack_c::get(), second ? -0x8000 : 0, second ? 0xC8 : -0xC8, 0);
    SearchHD_PSMTXMultVec(mDoMtx_stack_c::get(), origin.get(), gabi::at<void>(start));
    SearchLocal<be<f32>> dot;
    *dot = zero;
    SearchHD_PSMTXMultVec(mDoMtx_stack_c::get(), far.get(), gabi::at<void>(end));
    if (a->mMode != Act_c::MODE_FIND_e)
        (&a->mBgc)->wall_pos(a, gabi::at<cXyz>(start), gabi::at<cXyz>(end), gabi::at<bool>(hit),
                             gabi::at<f32>(address(dot.get())));
    f32 square = SearchHD_PSVECSquareDistance(gabi::at<void>(start), gabi::at<void>(end));
    f32 length = SearchHD_sqrtf(square);
    if (static_cast<u8>(a->mWallHit[second ? 1 : 0])) {
        f32 factor = f32(*dot) + l_value_1_02;
        bool high = second || static_cast<u16>(a->setID) != 0;
        u8 control = static_cast<u8>(a->mControl);
        f32 extension = factor * (high ? l_beamTuning5 : l_beamTuning3);
        if (control == 6) {
            f32 tweak = l_extraRange;
            f32 base = (high ? l_beamTuning6 : l_beamTuning4);
            extension = -(factor * (high ? base + tweak : base - tweak));
        }
        length += extension;
        SearchLocal<cXyz> difference, normal, offset, sum;
        SearchHD_cXyz___mi(gabi::at<void>(end), difference.get(), gabi::at<void>(start));
        SearchHD_cXyz_normZP(difference.get(), normal.get());
        SearchHD_cXyz___ml(normal.get(), offset.get(), extension);
        SearchHD_cXyz___pl(gabi::at<void>(end), sum.get(), offset.get());
        u32 sx = static_cast<u32>(search_bits(sum->x)), sz = static_cast<u32>(search_bits(sum->z));
        search_bits(gabi::at<cXyz>(end)->x) = sx;
        u32 sy = static_cast<u32>(search_bits(sum->y));
        search_bits(gabi::at<cXyz>(end)->z) = sz;
        search_bits(gabi::at<cXyz>(end)->y) = sy;
    }
    u8 kind = static_cast<u8>(a->mKind);
    if (a->isSecond(kind) || length > static_cast<f32>(a->mBeamMaxLength[second ? 1 : 0]))
        a->mBeamMaxLength[second ? 1 : 0] = length;
    a->mBeamLength[second ? 1 : 0] = length;
    SearchHD_mDoMtx_XYZrotM(mDoMtx_stack_c::get(), 0, 0, 0xC80);
    kind = static_cast<u8>(a->mKind);
    if (!a->isSecond(kind))
        SearchHD_mDoMtx_stack_c_push();
    f32 scaleFactor = l_value_0_0001043, size = static_cast<f32>(a->mBeamMaxLength[second ? 1 : 0]);
    x = static_cast<f32>(a->scale.x);
    y = static_cast<f32>(a->scale.y);
    SearchHD_mDoMtx_stack_c_scaleM(x, y, size * scaleFactor);
    mtx_copy(&a->mpBeamModel[second ? 1 : 0].get()->mBaseTransformMtx, mDoMtx_stack_c::get());
    kind = static_cast<u8>(a->mKind);
    if (!a->isSecond(kind)) {
        SearchHD_mDoMtx_stack_c_pop();
        size = static_cast<f32>(a->mBeamLength[second ? 1 : 0]);
        x = static_cast<f32>(a->scale.x);
        y = static_cast<f32>(a->scale.y);
        SearchHD_mDoMtx_stack_c_scaleM(x, y, size * scaleFactor);
    }
    SearchHD_PSMTXCopy(mDoMtx_stack_c::get(), &a->mBeamScaleMatrix[second ? 1 : 0]);
}

static bool equal_string(u32 a, const char* text) {
    u32 b = gabi::guest_addr(text);
    for (;;) {
        u8 x = load<u8>(a++), y = load<u8>(b++);
        if (x != y)
            return false;
        if (!x)
            return true;
    }
}
static s32 angle_abs(s32 x) { return x < 0 ? s32(0u - u32(x)) : x; }

static void vec_copy(cXyz* d, const cXyz* s) { d->copy(*s); }
static f32 project_hit(Act_c* a, u32 other, u32 start, u32 end, bool second) {
    SearchLocal<cXyz> temporary, relative, beam, unit, scaled, point;
    SearchHD_cXyz___mi(gabi::at<void>(other + 0x314), temporary.get(), gabi::at<void>(start));
    vec_copy(relative.get(), temporary.get());
    SearchHD_cXyz___mi(gabi::at<void>(end), temporary.get(), gabi::at<void>(start));
    vec_copy(beam.get(), temporary.get());
    SearchHD_cXyz_normZP(beam.get(), temporary.get());
    vec_copy(unit.get(), temporary.get());
    f32 dot = SearchHD_PSVECDotProduct(unit.get(), relative.get());
    SearchHD_cXyz___ml(unit.get(), scaled.get(), dot);
    SearchHD_cXyz___pl(gabi::at<void>(start), point.get(), scaled.get());
    f32 height;
    if (second) {
        a->mSpotPos.z = f32(point->z);
        a->mSpotPos.x = f32(point->x);
        height = point->y;
        a->mSpotPos.y = height;
    } else {
        a->mSpotPos.x = f32(point->x);
        height = point->y;
        a->mSpotPos.y = height;
        a->mSpotPos.z = f32(point->z);
    }
    u8 kind = static_cast<u8>(a->mKind);
    s32 mode = a->initialMode(kind);
    a->mSpotPos.y = height + l_spotHeight[u32(mode)];
    return height;
}

// Aim a beam at the persistent offset for a followed actor.
static void follow_actor(Act_c* a, u32 actor, cXyz* global, be<u32>* guard) {
    SearchLocal<cXyz> offset, target, diff;
    f32 zero = l_zero, height = l_hundred;
    if (static_cast<s32>(u32(*guard))) {
        offset->x = static_cast<f32>(global->x);
        offset->z = static_cast<f32>(global->z);
        offset->y = static_cast<f32>(global->y);
    } else {
        global->x = zero;
        global->y = height;
        global->z = zero;
        offset->y = height;
        *guard = 1;
        offset->x = zero;
        offset->z = zero;
    }
    SearchHD_cXyz___pl(offset.get(), target.get(), gabi::at<void>(actor + 0x314));
    SearchHD_cXyz___mi(target.get(), diff.get(), &a->mBeamStart[0].x);
    offset->y = diff->y;
    a->mPreviousYaw = static_cast<s16>(a->mBeamAngle[0].y);
    f32 dx = diff->x, dz = diff->z;
    offset->x = dx;
    offset->z = dz;
    s32 yaw = static_cast<s32>(SearchHD_cM_atan2s(dx, dz));
    dz = offset->z;
    s16 base = static_cast<s16>(a->current.angle.y);
    dx = offset->x;
    s16 targetYaw = s16(yaw - base);
    f32 len = SearchHD_sqrtf(gabi::fmadds(dx, dx, dz * dz));
    s32 pitch = static_cast<s32>(SearchHD_cM_atan2s(f32(offset->y), len));
    u32 beam = static_cast<u32>(a->mBeamIndex);
    SearchHD_cLib_addCalcAngleS2(&a->mBeamAngle[beam].y, targetYaw, 10, 0x400);
    beam = static_cast<u32>(a->mBeamIndex);
    SearchHD_cLib_addCalcAngleS2(&a->mBeamAngle[beam].x, pitch, 10, 0x400);
}

static void initialize_beam_targets(Act_c* a) {
    for (u32 i = 0; i < 3; i++) {
        search_component_bits(a->mBeamEnd[0], i) = search_component_bits(a->mInitialTarget[0], i);
        search_component_bits(a->mBeamEnd[1], i) = search_component_bits(a->mInitialTarget[1], i);
        search_component_bits(a->mSpotPos, i) = search_component_bits(a->current.pos, i);
    }
    a->mSpotColor[0] = 235;
    a->mSpotColor[1] = 125;
    a->mSpotColor[2] = 0;
}

/* 02389188 */
s32 createHeap_CB(Act_c* a) { return a->_createHeap(); }

/* 02389110 */
s32 daObj_Search::Act_c::_createHeap() {
    auto* a = this;
    if (!a->searchCreateHeap() || !a->beamCreateHeap(0))
        return 0;
    return a->beamCreateHeap(1) != 0;
}

/* 02388F20 */
s32 daObj_Search::Act_c::searchCreateHeap() {
    auto* a = this;
    SearchLocal<SafeString> name;
    name->mStringTop = gabi::guest_addr("Search");
    name->__vtbl = gc_tu.safeString;
    void* data = SearchHD_dRes_control_c_getRes(gabi::at<void>(address(dComIfG_resControl())), name.get(),
                                                dRes_SEARCH_BDL_SEARCH_e);
    if (!data)
        SearchHD_JUTAssertion_showAssert("d_a_obj_search.cpp", 0x4A, "modelData != (0)");
    void* model = SearchHD_mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
    a->mpSearchModel = static_cast<J3DModel*>(model);
    if (!model)
        return 0;
    void* bg = SearchHD_dBgW_dBgW((void*)nullptr);
    a->mpBaseBackground = static_cast<dBgW*>(bg);
    if (!bg)
        return 0;
    SearchLocal<SafeString> collision;
    collision->__vtbl = gc_tu.safeString;
    collision->mStringTop = gabi::guest_addr("Search");
    data = SearchHD_dRes_control_c_getRes(gabi::at<void>(address(dComIfG_resControl())), collision.get(),
                                          dRes_SEARCH_DZB_BASE_e);
    return static_cast<s32>(SearchHD_cBgW_Set(a->mpBaseBackground.get(), data, 1, &a->mBaseMatrix)) == 0;
}

/* 02389018 */
s32 daObj_Search::Act_c::beamCreateHeap(s32 index) {
    auto* a = this;
    SearchLocal<SafeString> name;
    name->__vtbl = gc_tu.safeString;
    name->mStringTop = gabi::guest_addr("Search");
    void* data = SearchHD_dRes_control_c_getRes(gabi::at<void>(address(dComIfG_resControl())), name.get(),
                                                dRes_SEARCH_BDL_BEAM_e);
    if (!data)
        return 0;
    void* model = SearchHD_mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
    a->mpBeamModel[index] = static_cast<J3DModel*>(model);
    if (!model)
        return 0;
    void* bg = SearchHD_dBgW_dBgW((void*)nullptr);
    a->mpBeamBackground[index] = static_cast<dBgW*>(bg);
    if (!bg)
        return 0;
    u32 resources = address(dComIfG_resControl()), resourceIndex = l_beamCollision[u32(index)];
    SearchLocal<SafeString> collision;
    collision->mStringTop = gabi::guest_addr("Search");
    collision->__vtbl = gc_tu.safeString;
    data = SearchHD_dRes_control_c_getRes(gabi::at<void>(resources), collision.get(), resourceIndex);
    return static_cast<s32>(
               SearchHD_cBgW_Set(a->mpBeamBackground[index].get(), data, 1, &a->mBeamMatrix[index])) == 0;
}

/* 023894DC */
s32 nodeControl_CB(void* node, s32 timing) {
    if (timing == 0) {
        u32 model = l_currentModel;
        u32 actor = static_cast<u32>(gabi::at<J3DModel>(model)->mUserArea);
        if (actor)
            SearchHD_daObj_Search_Act_c__nodeControl(gabi::at<Act_c>(actor), node, gabi::at<void>(model));
    }
    return 1;
}

/* 0238918C */
void daObj_Search::Act_c::_nodeControl(void* node, void* model) {
    auto* a = this;
    u32 desc = address(SearchHD_J3DJoint_getJointData(node));
    u16 index = static_cast<u16>(gabi::at<SearchJointDataView>(desc)->mJointNo);
    u32 block = static_cast<u32>(gabi::at<J3DModel>(address(model))->mpMtxBuffer.v),
        array = static_cast<u32>(gabi::at<J3DMtxBuffer_hd>(block)->mpAnmMtx);
    u16 flags = static_cast<u16>(gabi::at<J3DMtxBuffer_hd>(block)->mFlags);
    gabi::at<J3DMtxBuffer_hd>(block)->mFlags = flags | 0x10;
    SearchHD_PSMTXCopy(gabi::at<void>(array + u32(index) * 48), mDoMtx_stack_c::get());
    s16 delta = s16(static_cast<s16>(a->mBeamAngle[0].y) - static_cast<s16>(a->mPreviousYaw));
    s16 spin = s16(gabi::ftoi(f32(delta) * l_value_5));
    a->mTopAngle = 0x2700;
    if (index == 5) {
        s16 angle = static_cast<s16>(a->mBeamAngle[0].y);
        SearchHD_mDoMtx_XrotM(mDoMtx_stack_c::get(), angle);
        angle = static_cast<s16>(a->mBeamAngle[0].x);
        SearchHD_mDoMtx_YrotM(mDoMtx_stack_c::get(), angle);
    } else if (index == 6) {
        s16 angle = static_cast<s16>(a->mBeamAngle[1].y);
        SearchHD_mDoMtx_XrotM(mDoMtx_stack_c::get(), angle);
        angle = static_cast<s16>(a->mBeamAngle[1].x);
        SearchHD_mDoMtx_YrotM(mDoMtx_stack_c::get(), angle);
    } else if (index == 7)
        SearchHD_mDoMtx_YrotM(mDoMtx_stack_c::get(), 0x2700);
    else if (index == 4) {
        SearchHD_mDoMtx_YrotM(mDoMtx_stack_c::get(), 0x2700);
        s16 angle = static_cast<s16>(a->mGearAngle[2]);
        SearchHD_mDoMtx_XrotM(mDoMtx_stack_c::get(), angle);
    }
    if (a->mMode != Act_c::MODE_STOP_e) {
        if (index == 3) {
            f32 old = f32(static_cast<s16>(a->mGearAngle[2]));
            s16 angle = s16(gabi::ftoi(gabi::fmadds(f32(spin), l_value_2_1017, old)));
            a->mGearAngle[2] = angle;
            SearchHD_mDoMtx_XrotM(mDoMtx_stack_c::get(), angle);
        } else if (index == 2) {
            f32 old = f32(static_cast<s16>(a->mGearAngle[1]));
            s16 angle = s16(gabi::ftoi(gabi::fnmsubs(f32(spin), l_value_1_4013, old)));
            a->mGearAngle[1] = angle;
            SearchHD_mDoMtx_XrotM(mDoMtx_stack_c::get(), angle);
        } else if (index == 1) {
            f32 old = f32(static_cast<s16>(a->mGearAngle[0]));
            s16 angle = s16(gabi::ftoi(gabi::fmadds(f32(spin), l_value_1_05, old)));
            a->mGearAngle[0] = angle;
            SearchHD_mDoMtx_XrotM(mDoMtx_stack_c::get(), angle);
        }
    }
    SearchHD_PSMTXCopy(mDoMtx_stack_c::get(), gabi::at<void>(0x104B4868));
    block = static_cast<u32>(gabi::at<J3DModel>(address(model))->mpMtxBuffer.v);
    flags = static_cast<u16>(gabi::at<J3DMtxBuffer_hd>(block)->mFlags);
    array = static_cast<u32>(gabi::at<J3DMtxBuffer_hd>(block)->mpAnmMtx);
    gabi::at<J3DMtxBuffer_hd>(block)->mFlags = flags | 0x10;
    mtx_copy(gabi::at<Mtx34>(array + u32(index) * 48), mDoMtx_stack_c::get());
}

/* 0238DCF4 */
void daObj_Search::Act_c::modeSearchRndInit() {}

/* 0238BE60 */
void daObj_Search::Act_c::modeSearchRnd() {
    auto* a = this;
    if (static_cast<u8>(a->mActive) && static_cast<s32>(SearchHD_daObj_Search_Act_c_player_check(a)))
        a->modeProc(PROC_INIT_e, MODE_FIND_e);
    s16 y = static_cast<s16>(a->mBeamAngle[0].y), x = static_cast<s16>(a->mBeamAngle[0].x);
    a->mPreviousPitch = x;
    s16 z = static_cast<s16>(a->mBeamAngle[1].x);
    a->mPreviousRoll = z;
    f32 next = f32(y) + l_value_80;
    a->mBeamAngle[0].x = 0;
    a->mPreviousYaw = y;
    s16 angle = s16(gabi::ftoi(next));
    a->mBeamAngle[0].y = angle;
    a->mBeamAngle[1].x = 0;
    a->mBeamAngle[1].y = angle;
}

/* 0238B6A0 */
void daObj_Search::Act_c::modeSearchPathInit() {
    auto* a = this;
    if (!a->is_path_info())
        a->modeProc(PROC_INIT_e, MODE_SEARCH_RND_e);
}

/* 0238BF20 */
void daObj_Search::Act_c::modeSearchPath() {
    auto* a = this;
    u32 path = static_cast<u32>(a->mpPath.v);
    SearchHD_dLib_pathMove(&a->mPathTarget.x, &a->mPathPointIndex, gabi::at<void>(path), 0, 0, l_value_20);
    if (static_cast<u8>(a->mActive)) {
        SearchLocal<be<s16>> profile;
        *profile = 0xF1;
        u32 found = address(SearchHD_fopAcIt_Judge(search_callback(SearchJudge_actorName), profile.get()));
        if (static_cast<s32>(a->mSwitch) != 255 && found) {
            SearchLocal<be<u32>> id;
            u32 child = static_cast<u32>(a->mChildId);
            *id = child;
            void* controller = nullptr;
            if (child != 0xFFFFFFFF)
                controller = SearchHD_fopAcIt_Judge(search_callback(SearchJudge_actorId), id.get());
            if (controller)
                gabi::at<SearchControllerView>(address(controller))->mBeamYaw =
                    static_cast<s16>(a->mGearAngle[2]);
            u8 control = static_cast<u8>(a->mControl);
            if (control == 4) {
                a->modeProc(PROC_INIT_e, MODE_TO_STOP_e);
                return;
            }
            if (control == 2) {
                auto* play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
                u32 player = static_cast<u32>(play->play.mPlayerInfo[0].mpPlayer.v);
                follow_actor(a, player, &l_trackingOffset0, &l_trackingOffsetGuard0);
                return;
            }
            if (control == 3) {
                follow_actor(a, found, &l_trackingOffset1, &l_trackingOffsetGuard1);
                return;
            }
        } else {
            if (static_cast<s32>(SearchHD_daObj_Search_Act_c_player_check(a))) {
                if (l_eventMode != 1) {
                    s32 count = s32(u32(static_cast<s32>(a->mFindTimer)) + 1);
                    a->mFindTimer = count;
                    if (count > 14) {
                        a->modeProc(PROC_INIT_e, MODE_FIND_e);
                        a->mFindTimer = 0;
                    }
                }
            } else
                a->mFindTimer = 0;
        }
    }
    u32 z = static_cast<u32>(search_bits(a->mPathTarget.z)),
        y = static_cast<u32>(search_bits(a->mPathTarget.y));
    search_bits(a->mInitialTarget[0].z) = z;
    u32 x = static_cast<u32>(search_bits(a->mPathTarget.x));
    search_bits(a->mInitialTarget[0].y) = y;
    search_bits(a->mInitialTarget[0].x) = x;
    SearchLocal<cXyz> diff;
    SearchHD_cXyz___mi(&a->mInitialTarget[0].x, diff.get(), &a->mBeamStart[0].x);
    f32 dz = diff->z;
    s16 angle = static_cast<s16>(a->mBeamAngle[0].y), ax = static_cast<s16>(a->mBeamAngle[0].x);
    a->mPreviousYaw = angle;
    f32 dx = diff->x;
    a->mPreviousPitch = ax;
    angle = static_cast<s16>(a->mBeamAngle[1].x);
    f32 dy = diff->y;
    a->mPreviousRoll = angle;
    s32 yaw = static_cast<s32>(SearchHD_cM_atan2s(dx, dz));
    s16 base = static_cast<s16>(a->current.angle.y);
    s16 targetYaw = s16(yaw - base);
    f32 len = SearchHD_sqrtf(gabi::fmadds(dx, dx, dz * dz));
    s32 pitch = static_cast<s32>(SearchHD_cM_atan2s(dy, len));
    if (l_randomYawOff == 0) {
        SearchHD_cLib_addCalcAngleS2(&a->mBeamAngle[0].y, targetYaw, 30, 0x400);
        SearchHD_cLib_addCalcAngleS2(&a->mBeamAngle[0].x, pitch, 30, 0x400);
        angle = static_cast<s16>(a->mBeamAngle[0].y);
        a->mBeamAngle[1].x = 0xE38;
        a->mBeamAngle[1].y = angle;
    } else {
        a->mBeamAngle[0].y = targetYaw;
        a->mBeamAngle[0].x = pitch;
        a->mBeamAngle[1].x = 0xE38;
        a->mBeamAngle[1].y = targetYaw;
    }
}

/* 0238B6DC */
void daObj_Search::Act_c::modeStopInit() {
    auto* a = this;
    a->mBeamAngle[0].x = 0x2300;
    a->mBeamAngle[1].x = -0x2300;
}

/* 0238DD00 */
void daObj_Search::Act_c::modeStop() {}

/* 0238DCF8 */
void daObj_Search::Act_c::modeToSearchInit() {}

/* 0238C640 */
void daObj_Search::Act_c::modeToSearch() {
    auto* a = this;
    s16 x = static_cast<s16>(a->mPreviousPitch);
    s32 dx = static_cast<s32>(SearchHD_cLib_addCalcAngleS(&a->mBeamAngle[0].x, x, 10, 0x200, 0x100));
    s16 z = static_cast<s16>(a->mPreviousRoll);
    s32 dz = static_cast<s32>(SearchHD_cLib_addCalcAngleS(&a->mBeamAngle[1].x, z, 10, 0x200, 0x100));
    if (dx < 0x100 && dz < 0x100)
        a->modeProc(PROC_INIT_e, MODE_SEARCH_PATH_e);
}

/* 0238B6F0 */
void daObj_Search::Act_c::modeToStopInit() {
    auto* a = this;
    a->mPreviousYaw = static_cast<s16>(a->mBeamAngle[0].y);
    auto* save = gabi::at<dSv_info_c>(dComIfGs_ea() + 0x20);
    if (static_cast<s32>(SearchHD_dSv_event_c_isEventBit(&save->mSavedata.mEvent, 0x201))) {
        s8 room = static_cast<s8>(a->current.roomNo);
        s32 reverb = static_cast<s32>(SearchHD_dComIfGp_getReverb(room));
        SearchHD_mDoAud_seStart(0x6939, &a->eyePos.x, 0, reverb);
    }
}

/* 0238C834 */
void daObj_Search::Act_c::modeToStop() {
    auto* a = this;
    auto* save = gabi::at<dSv_info_c>(dComIfGs_ea() + 0x20);
    s32 event = static_cast<s32>(SearchHD_dSv_event_c_isEventBit(&save->mSavedata.mEvent, 0x201));
    f32 speed = l_value_5;
    if (!event && static_cast<s32>(a->mSwitch) == 255) {
        if (static_cast<u16>(a->eventInfo.mCommand) != 2) {
            SearchHD_fopAcM_orderOtherEvent2(a, "Search_Light_Up", 1, 0xFFFF);
            return;
        }
        a->mAlpha = 255;
        auto* play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
        s32 staff =
            static_cast<s32>(SearchHD_dEvent_manager_c_getMyStaffId(&play->play.mEvtManager, "Search", 0, 0));
        play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
        if (static_cast<s32>(
                SearchHD_dEvent_manager_c_endCheckOld(&play->play.mEvtManager, "Search_Light_Up"))) {
            play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
            play->play.mEvtCtrl.mEventFlag = static_cast<u16>(play->play.mEvtCtrl.mEventFlag) | 8;
            save = gabi::at<dSv_info_c>(dComIfGs_ea() + 0x20);
            SearchHD_dSv_event_c_onEventBit(&save->mSavedata.mEvent, 0x201);
            a->modeProc(PROC_INIT_e, MODE_STOP_e);
            return;
        }
        SearchLocal<be<u32>> id;
        u32 child = static_cast<u32>(a->mChildId);
        *id = child;
        void* controller = nullptr;
        if (child != 0xFFFFFFFF)
            controller = SearchHD_fopAcIt_Judge(search_callback(SearchJudge_actorId), id.get());
        if (controller && static_cast<s16>(gabi::at<SearchControllerView>(address(controller))->mTimer) < 5)
            gabi::at<SearchControllerView>(address(controller))->mTimer = 5;
        play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
        u32 cut = address(SearchHD_dEvent_manager_c_getMyNowCutName(&play->play.mEvtManager, staff));
        if (!cut) {
            SearchHD_JUTAssertion_showAssert("d_a_obj_search.cpp", 0x279, "cut_name != (0)");
            return;
        }
        if (equal_string(cut, "BK_FIND")) {
            play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
            SearchHD_dEvent_manager_c_cutEnd(&play->play.mEvtManager, staff);
        }
        if (equal_string(cut, "LIGHT_UP_SOUND")) {
            s8 room = static_cast<s8>(a->current.roomNo);
            s32 reverb = static_cast<s32>(SearchHD_dComIfGp_getReverb(room));
            SearchHD_mDoAud_seStart(0x6939, &a->eyePos.x, 0, reverb);
            play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
            SearchHD_dEvent_manager_c_cutEnd(&play->play.mEvtManager, staff);
        }
        if (equal_string(cut, "LIGHT_UP")) {
            s32 dx =
                static_cast<s32>(SearchHD_cLib_addCalcAngleS(&a->mBeamAngle[0].x, 0x2300, 30, 0x300, 0x10));
            s32 dz =
                static_cast<s32>(SearchHD_cLib_addCalcAngleS(&a->mBeamAngle[1].x, -0x2300, 30, 0x300, 0x10));
            if (angle_abs(dx) < 0x100 && angle_abs(dz) < 0x100) {
                a->smoke_set(speed, 10);
                play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
                SearchHD_dEvent_manager_c_cutEnd(&play->play.mEvtManager, staff);
            }
        }
        return;
    }
    s32 dx = static_cast<s32>(SearchHD_cLib_addCalcAngleS(&a->mBeamAngle[0].x, 0x2300, 20, 0x200, 0x10));
    s32 dz = static_cast<s32>(SearchHD_cLib_addCalcAngleS(&a->mBeamAngle[1].x, -0x2300, 20, 0x200, 0x10));
    if (angle_abs(dx) < 0x100 && angle_abs(dz) < 0x100) {
        a->smoke_set(speed, 10);
        a->modeProc(PROC_INIT_e, MODE_STOP_e);
    }
}

/* 0238B75C */
void daObj_Search::Act_c::modeFindInit() {
    auto* a = this;
    if (static_cast<s32>(a->mSwitch) != 255) {
        a->modeProc(PROC_INIT_e, MODE_FIND_2ND_e);
        return;
    }
    if (static_cast<u8>(a->mControl)) {
        auto* save = gabi::at<dSv_info_c>(dComIfGs_ea() + 0x20);
        SearchHD_dSv_event_c_onEventBit(&save->mSavedata.mEvent, 0x340);
    }
    if (static_cast<u16>(a->eventInfo.mCommand) != 2)
        SearchHD_mDoAud_bgmStart(0x834);
    s16 y = static_cast<s16>(a->mBeamAngle[0].y), z = static_cast<s16>(a->mBeamAngle[1].x);
    a->mPreviousYaw = y;
    s16 x = static_cast<s16>(a->mBeamAngle[0].x);
    a->mPreviousRoll = z;
    a->mPreviousPitch = x;
}

/* 0238CC18 */
void daObj_Search::Act_c::modeFind() {
    auto* a = this;
    for (const char* name :
         {"Search_Light_Find", "Search_Light_Find_With_Barrel", "Search_Light_Find_Wall"}) {
        auto* play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
        static_cast<s32>(SearchHD_dEvent_manager_c_getEventIdx(&play->play.mEvtManager, name, 255));
    }
    auto* play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
    u32 player = static_cast<u32>(play->play.mPlayerInfo[0].mpPlayer.v);
    static_cast<s32>(SearchHD_daObj_Search_Act_c_player_check(a));
    SearchLocal<cXyz> offset, target, diff;
    if (l_trackingOffsetGuard2) {
        offset->x = l_trackingOffset2.x;
        offset->y = l_trackingOffset2.y;
        offset->z = l_trackingOffset2.z;
    } else {
        f32 zero = l_zero, height = l_hundred;
        l_trackingOffset2.x = zero;
        offset->y = height;
        l_trackingOffset2.z = zero;
        l_trackingOffsetGuard2 = 1;
        offset->z = zero;
        offset->x = zero;
        l_trackingOffset2.y = height;
    }
    SearchHD_cXyz___pl(offset.get(), target.get(), gabi::at<void>(player + 0x314));
    u32 beam = static_cast<u32>(a->mBeamIndex);
    SearchHD_cXyz___mi(target.get(), diff.get(), &a->mBeamStart[beam]);
    s16 current = static_cast<s16>(a->mBeamAngle[0].y);
    a->mPreviousYaw = current;
    offset->y = diff->y;
    f32 dz = diff->z, dx = diff->x;
    offset->z = dz;
    offset->x = dx;
    s32 yaw = static_cast<s32>(SearchHD_cM_atan2s(dx, dz));
    dz = offset->z;
    s16 base = static_cast<s16>(a->current.angle.y);
    dx = offset->x;
    s16 desiredYaw = s16(yaw - base);
    f32 len = SearchHD_sqrtf(gabi::fmadds(dx, dx, dz * dz));
    s32 pitch = static_cast<s32>(SearchHD_cM_atan2s(f32(offset->y), len));

    beam = static_cast<u32>(a->mBeamIndex);
    if (beam == 0) {
        a->mBeamAngle[1].y = desiredYaw;
        beam = static_cast<u32>(a->mBeamIndex);
        SearchHD_cLib_addCalcAngleS2(&a->mBeamAngle[beam].y, desiredYaw, 10, 0x400);
        beam = static_cast<u32>(a->mBeamIndex);
        SearchHD_cLib_addCalcAngleS2(&a->mBeamAngle[beam].x, pitch, 10, 0x400);
    } else {
        desiredYaw = s16(desiredYaw - 0x8000);
        pitch = s16(-pitch);
        a->mBeamAngle[0].y = desiredYaw;
        beam = static_cast<u32>(a->mBeamIndex);
        SearchHD_cLib_addCalcAngleS2(&a->mBeamAngle[beam].y, desiredYaw, 10, 0x400);
        beam = static_cast<u32>(a->mBeamIndex);
        SearchHD_cLib_addCalcAngleS2(&a->mBeamAngle[beam].x, pitch, 10, 0x400);
    }
    if (static_cast<u16>(a->eventInfo.mCommand) == 2) {
        play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
        s32 staff =
            static_cast<s32>(SearchHD_dEvent_manager_c_getMyStaffId(&play->play.mEvtManager, "Search", 0, 0));
        bool ended = false;
        for (const char* name :
             {"Search_Light_Find", "Search_Light_Find_With_Barrel", "Search_Light_Find_Wall"}) {
            play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
            if (static_cast<s32>(SearchHD_dEvent_manager_c_endCheckOld(&play->play.mEvtManager, name))) {
                ended = true;
                break;
            }
        }
        if (ended) {
            SearchHD_mDoAud_bgmStop(0x834, 20);
            SearchHD_dComIfGp_setNextStage("majroom", 0, 0, -1, 0, 1, 0, l_zero);
            return;
        }
        play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
        bool stop = static_cast<u32>(play->play.mPlayerStatus[0][0]) & 1;
        if (!stop) {
            play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
            stop = static_cast<u32>(play->play.mPlayerStatus[0][0]) & 0x100;
        }
        if (!stop) {
            gabi::at<daPy_py_c>(player)->mDemo.mDemoType = 3;
            gabi::at<daPy_py_c>(player)->mDemo.mParam0 = 0;
            beam = static_cast<u32>(a->mBeamIndex);
            s16 angle = static_cast<s16>(a->mBeamAngle[beam].y), base = static_cast<s16>(a->current.angle.y);
            s16 yaw = s16(angle + base);
            if (beam == 0)
                yaw = s16(yaw - 0x8000);
            SearchHD_cLib_addCalcAngleS2(&a->mPlayerYaw, yaw, 4, 0x400);
            u32 vt = static_cast<u32>(gabi::at<daPy_py_c>(player)->__vtbl),
                target = search_virtual(vt, 0x114);
            angle = static_cast<s16>(a->mPlayerYaw);
            gabi::call<void>(target, gabi::at<void>(player), gabi::at<void>(player + 0x314), angle);
            angle = static_cast<s16>(a->mPlayerYaw);
            if (static_cast<s32>(SearchHD_cLib_distanceAngleS(yaw, angle)) < 0x500) {
                gabi::at<daPy_py_c>(player)->mDemo.mDemoType = 2;
                gabi::at<daPy_py_c>(player)->mDemo.mDemoMode = 1;
            }
        }
        play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
        SearchHD_dEvent_manager_c_cutEnd(&play->play.mEvtManager, staff);
        return;
    }
    u32 vt = static_cast<u32>(gabi::at<daPy_py_c>(player)->__vtbl), targetFn = search_virtual(vt, 0x4C);
    if (gabi::call<s32>(targetFn, gabi::at<void>(player)))
        return;
    if (static_cast<f32>(gabi::at<daPy_py_c>(player)->field_0x2b0) < l_zero) {
        SearchHD_fopAcM_orderOtherEvent2(a, "Search_Light_Find_With_Barrel", 1, 0xFFFF);
        return;
    }
    play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
    u32 status = static_cast<u32>(play->play.mPlayerStatus[0][0]);
    SearchHD_fopAcM_orderOtherEvent2(a, (status & 1 ? "Search_Light_Find_Wall" : "Search_Light_Find"), 1,
                                     0xFFFF);
}

/* 0238B800 */
void daObj_Search::Act_c::modeFind2ndInit() {
    auto* a = this;
    s16 z = static_cast<s16>(a->mBeamAngle[1].x), y = static_cast<s16>(a->mBeamAngle[0].y);
    a->mPreviousRoll = z;
    s16 x = static_cast<s16>(a->mBeamAngle[0].x);
    a->mPreviousYaw = y;
    a->mPreviousPitch = x;
    auto* save = gabi::at<dSv_info_c>(dComIfGs_ea() + 0x20);
    s8 room = l_stayRoom;
    u32 sw = static_cast<u32>(a->mSwitch);
    SearchHD_dSv_info_c_onSwitch(save, sw, room);
}

/* 0238D128 */
void daObj_Search::Act_c::modeFind2nd() {
    auto* a = this;
    auto* play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
    u32 player = static_cast<u32>(play->play.mPlayerInfo[0].mpPlayer.v);
    static_cast<s32>(SearchHD_daObj_Search_Act_c_player_check(a));
    SearchLocal<cXyz> offset, target, diff;
    if (l_trackingOffsetGuard3) {
        offset->x = l_trackingOffset3.x;
        offset->y = l_trackingOffset3.y;
        offset->z = l_trackingOffset3.z;
    } else {
        f32 zero = l_zero, height = l_hundred;
        l_trackingOffset3.x = zero;
        offset->y = height;
        l_trackingOffset3.z = zero;
        l_trackingOffsetGuard3 = 1;
        offset->z = zero;
        offset->x = zero;
        l_trackingOffset3.y = height;
    }
    SearchHD_cXyz___pl(offset.get(), target.get(), gabi::at<void>(player + 0x314));
    u32 beam = static_cast<u32>(a->mBeamIndex);
    SearchHD_cXyz___mi(target.get(), diff.get(), &a->mBeamStart[beam]);
    s16 current = static_cast<s16>(a->mBeamAngle[0].y);
    a->mPreviousYaw = current;
    offset->y = diff->y;
    f32 dz = diff->z, dx = diff->x;
    offset->z = dz;
    offset->x = dx;
    s32 yaw = static_cast<s32>(SearchHD_cM_atan2s(dx, dz));
    dz = offset->z;
    s16 base = static_cast<s16>(a->current.angle.y);
    dx = offset->x;
    s16 desiredYaw = s16(yaw - base);
    f32 len = SearchHD_sqrtf(gabi::fmadds(dx, dx, dz * dz));
    s32 pitch = static_cast<s32>(SearchHD_cM_atan2s(f32(offset->y), len));
    s16 lower = s16(l_beamPitchOffset - 10000), upper = s16(l_yawRate + 26000);
    bool above = (u32(upper) - u32(pitch) - 1) >> 31, below = (u32(pitch) - u32(lower) - 1) >> 31;
    if (above) {
        if (upper > pitch)
            pitch = upper;
        pitch = s16(pitch);
    }
    if (below) {
        if (lower < pitch)
            pitch = lower;
        pitch = s16(pitch);
    }
    SearchLocal<cXyz> eye;
    eye->x = static_cast<f32>(gabi::at<daPy_py_c>(player)->mHeadTopPos.x);
    beam = static_cast<u32>(a->mBeamIndex);
    eye->y = static_cast<f32>(gabi::at<daPy_py_c>(player)->mHeadTopPos.y);
    eye->z = static_cast<f32>(gabi::at<daPy_py_c>(player)->mHeadTopPos.z);
    SearchHD_dBgS_LinChk_Set(&a->mPlayerLine.cBgS_Chk::mPolyPassChk, &a->mBeamStart[beam], eye.get(), a);
    play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
    if (static_cast<s32>(SearchHD_cBgS_LineCross(&play->play.mBgS, &a->mPlayerLine.cBgS_Chk::mPolyPassChk)) ||
        above || below)
        a->modeProc(PROC_INIT_e, MODE_SEARCH_PATH_e);
    beam = static_cast<u32>(a->mBeamIndex);
    if (beam == 0) {
        a->mBeamAngle[1].y = desiredYaw;
        beam = static_cast<u32>(a->mBeamIndex);
        SearchHD_cLib_addCalcAngleS2(&a->mBeamAngle[beam].y, desiredYaw, 10, 0x400);
        beam = static_cast<u32>(a->mBeamIndex);
        SearchHD_cLib_addCalcAngleS2(&a->mBeamAngle[beam].x, pitch, 10, 0x400);
    } else {
        desiredYaw = s16(desiredYaw - 0x8000);
        a->mBeamAngle[0].y = desiredYaw;
        beam = static_cast<u32>(a->mBeamIndex);
        pitch = s16(-pitch);
        SearchHD_cLib_addCalcAngleS2(&a->mBeamAngle[beam].y, desiredYaw, 10, 0x400);
        beam = static_cast<u32>(a->mBeamIndex);
        SearchHD_cLib_addCalcAngleS2(&a->mBeamAngle[beam].x, pitch, 10, 0x400);
    }
}

/* 0238DCFC */
void daObj_Search::Act_c::modeSearchBdkInit() {}

/* 0238D444 */
void daObj_Search::Act_c::modeSearchBdk() {
    auto* a = this;
    SearchLocal<be<u32>> found;
    if (!static_cast<s32>(SearchHD_fopAcM_SearchByName(0xEE, found.get())))
        return;
    u32 bird = *found;
    if (!bird)
        return;
    f32 z = static_cast<f32>(gabi::at<fopAc_ac_c>(bird)->current.pos.z),
        y = static_cast<f32>(gabi::at<fopAc_ac_c>(bird)->current.pos.y),
        x = static_cast<f32>(gabi::at<fopAc_ac_c>(bird)->current.pos.x), tenThousand = l_value_10000,
        radius = l_extraRange, height = l_birdHeight;
    a->mBirdTarget.x = l_value_3600;
    a->mBirdTarget.y = tenThousand;
    a->mBirdTarget.z = l_value__3800;
    a->mBirdCircleRange = l_birdCircleRange + l_value_4000;
    a->mBirdFanRange = l_birdFanRange + l_value_15000;
    a->mBirdAngle = l_birdAngle + 0x3500;
    a->mBirdDirection = l_birdDirection;
    height = y + height;
    SearchLocal<cXyz> difference, horizontal, origin;
    SearchHD_cXyz___mi(&a->mBirdTarget.x, difference.get(), gabi::at<void>(bird + 0x314));
    f32 zero = l_zero;
    horizontal->z = difference->z;
    horizontal->x = difference->x;
    horizontal->y = zero;
    f32 sq = SearchHD_PSVECSquareMag(horizontal.get());
    SearchHD_sqrtf(sq);
    origin->x = static_cast<f32>(a->mBirdTarget.x);
    origin->y = static_cast<f32>(a->mBirdTarget.y);
    origin->z = static_cast<f32>(a->mBirdTarget.z);
    f32 fanRange = static_cast<f32>(a->mBirdFanRange), threeThousand = l_value_3000;
    s16 direction = static_cast<s16>(a->mBirdDirection), angle = static_cast<s16>(a->mBirdAngle);
    bird = *found;
    u8 inside = static_cast<u8>(SearchHD_dLib_checkActorInFan(origin.get(), gabi::at<void>(bird), direction,
                                                              angle, fanRange, threeThousand));
    origin->y = static_cast<f32>(a->mBirdTarget.y);
    a->mBirdInside = inside;
    origin->z = static_cast<f32>(a->mBirdTarget.z);
    origin->x = static_cast<f32>(a->mBirdTarget.x);
    bird = *found;
    f32 circle = static_cast<f32>(a->mBirdCircleRange);
    if (static_cast<s32>(
            SearchHD_dLib_checkActorInCircle(origin.get(), gabi::at<void>(bird), circle, tenThousand)))
        a->mBirdInside = 0;
    s16 state = static_cast<s16>(gabi::at<SearchBirdView>(bird)->mState);
    f32 hundred = l_hundred;
    bool track = true;
    if (u32(s32(state)) <= 1 || state == 7) {
        if (static_cast<u8>(a->mBirdInside)) {
            height += hundred;
            radius += l_value_500;
        } else
            track = false;
    } else if (u32(s32(state)) < 7 || (u32(s32(state)) > 7 && u32(s32(state)) < 10)) {
        if (static_cast<u8>(a->mBirdInside)) {
            radius += threeThousand;
            height += l_value_1000;
        } else
            track = false;
    } else if (state == 10) {
        s16 phase = static_cast<s16>(gabi::at<SearchBirdView>(bird)->mPhase);
        if (phase == 8) {
            if (static_cast<s32>(SearchHD_cLib_calcTimer_i(&a->mBirdLastTimer))) {
                x = static_cast<f32>(a->mBirdLastPos.x);
                height = static_cast<f32>(a->mBirdLastPos.y);
                z = static_cast<f32>(a->mBirdLastPos.z);
            } else
                track = false;
        } else if (phase == 7) {
            f32 random = SearchHD_cM_rndF(hundred);
            s16 adjustment = l_birdYawAdjustment;
            f32 base = l_value_300;
            height = static_cast<f32>(a->mBirdLastPos.y);
            z = static_cast<f32>(a->mBirdLastPos.z);
            x = static_cast<f32>(a->mBirdLastPos.x);
            a->mBirdLastTimer = gabi::ftoi((random + base) + f32(adjustment));
        } else if (phase >= 6) {
            a->mBirdLastPos.y = height;
            a->mBirdLastPos.z = z;
            a->mBirdLastPos.x = x;
        } else if (phase >= 5) {
            radius += hundred;
            height += hundred;
        } else
            track = false;
    } else
        track = false;
    s32 sign = (static_cast<u32>(a->mBsPcId) & 1) ? -1 : 1;
    if (track) {
        SearchHD_cLib_addCalc2(&a->mTrackingRadius, radius, l_value_0_1, hundred);
        a->mTrackingSpeed = l_birdTargetHeight + l_value_200;
        s16 rate = l_yawRate;
        a->mTrackingPos.x = x;
        a->mTrackingPos.y = height;
        a->mTrackingPos.z = z;
        a->mTrackingRate = (s32(rate) + 0x150) * sign;
        SearchHD_dLib_setCirclePath(&a->mTrackingPos.x);
        SearchLocal<cXyz> offset, target, diff;
        if (l_trackingOffsetGuard4) {
            offset->x = l_trackingOffset4.x;
            offset->y = l_trackingOffset4.y;
            offset->z = l_trackingOffset4.z;
        } else {
            offset->x = zero;
            l_trackingOffset4.x = zero;
            offset->y = hundred;
            l_trackingOffset4.y = hundred;
            l_trackingOffsetGuard4 = 1;
            offset->z = zero;
            l_trackingOffset4.z = zero;
        }
        SearchHD_cXyz___pl(offset.get(), target.get(), &a->mTrackingTarget.x);
        SearchHD_cXyz___mi(target.get(), diff.get(), &a->mBeamStart[0].x);
        a->mPreviousYaw = static_cast<s16>(a->mBeamAngle[0].y);
        f32 dx = diff->x;
        offset->x = dx;
        offset->y = diff->y;
        f32 dz = diff->z;
        offset->z = dz;
        s32 yaw = static_cast<s32>(SearchHD_cM_atan2s(dx, dz));
        dz = offset->z;
        s16 base = static_cast<s16>(a->current.angle.y);
        dx = offset->x;
        s16 desired = s16(yaw - base);
        f32 len = SearchHD_sqrtf(gabi::fmadds(dx, dx, dz * dz));
        s32 pitch = static_cast<s32>(SearchHD_cM_atan2s(f32(offset->y), len));
        SearchHD_cLib_addCalcAngleS2(&a->mBeamAngle[0].y, desired, 10, 0x100);
        SearchHD_cLib_addCalcAngleS2(&a->mBeamAngle[0].x, pitch, 10, 0x100);
        a->mBeamAngle[1].y = static_cast<s16>(a->mBeamAngle[0].y);
        a->mBeamAngle[1].x = -static_cast<s16>(a->mBeamAngle[0].x);
        return;
    }
    if (!l_trackingOffsetGuard5) {
        l_trackingOffsetGuard5 = 1;
        l_trackingOffset5.x = zero;
        l_trackingOffset5.y = hundred;
        l_trackingOffset5.z = zero;
    }
    s16 target = s16((static_cast<s32>(a->mBirdPhase) == 1 ? l_birdTurnRateFirst : l_birdTurnRateOther) +
                     (static_cast<s32>(a->mBirdPhase) == 1 ? 0x3400 : 0x2600));
    if (!static_cast<s32>(SearchHD_cLib_calcTimer_i(&a->mBirdTimer))) {
        f32 base = f32(s32(l_birdRadiusModifier) + 60);
        f32 random = SearchHD_cM_rndF(l_birdFanRange + l_value_60);
        s32 phase = static_cast<s32>(a->mBirdPhase);
        a->mBirdTimer = gabi::ftoi(base + random);
        a->mBirdPhase = phase == 1 ? -1 : 1;
    }
    a->mPreviousYaw = static_cast<s16>(a->mBeamAngle[0].y);
    SearchHD_cLib_addCalcAngleS2(&a->mBeamAngle[0].x, target, 20, 0x20);
    a->mBeamAngle[1].x = -static_cast<s16>(a->mBeamAngle[0].x);
}

/* 0238A540 */
void daObj_Search::Act_c::modeProc(s32 operation, s32 mode) {
    if (operation == PROC_INIT_e) {
        mMode = mode;
        (this->*l_modeTable[u32(mode)].init)();
    } else if (operation == PROC_EXEC_e) {
        (this->*l_modeTable[u32(s32(mMode))].run)();
    }
}

/* 023895D4 */
void daObj_Search::Bgc_c::wall_pos(const Act_c* a, cXyz* start, cXyz* end, bool* hit, f32* dot) {
    auto* bg = this;
    SearchHD_dBgS_LinChk_Set(bg, start, end, a);
    auto* play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
    if (static_cast<s32>(SearchHD_cBgS_LineCross(&play->play.mBgS, bg))) {
        play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
        u16 index = static_cast<u16>(bg->mPolyIndex), owner = static_cast<u16>(bg->mBgIndex);
        void* plane = SearchHD_cBgS_GetTriPla(&play->play.mBgS, owner, index);
        if (plane) {
            SearchLocal<cXyz> difference, normal;
            SearchHD_cXyz___mi(end, difference.get(), start);
            SearchHD_cXyz_normZP(difference.get(), normal.get());
            f32 d = SearchHD_PSVECDotProduct(normal.get(), plane);
            search_output_float(dot, d);
            u32 x = static_cast<u32>(search_bits(bg->mLin.mEnd.x)),
                y = static_cast<u32>(search_bits(bg->mLin.mEnd.y));
            search_bits(bg->mWallPos.x) = x;
            u32 z = static_cast<u32>(search_bits(bg->mLin.mEnd.z));
            search_bits(bg->mWallPos.y) = y;
            search_bits(bg->mWallPos.z) = z;
            search_bits(end->x) = x;
            search_bits(end->y) = static_cast<u32>(search_bits(bg->mWallPos.y));
            search_bits(end->z) = static_cast<u32>(search_bits(bg->mWallPos.z));
            search_output_hit(hit, true);
            return;
        }
    }
    search_output_hit(hit, false);
}

/* 023897D0 */
void daObj_Search::Act_c::SetArgData() {
    auto* a = this;
    u32 p = static_cast<u32>(a->mParameters);
    a->mKind = p >> 24;
    a->mPathId = p >> 16;
    u32 control = (p >> 8) & 255;
    a->mType = p;
    a->mControl = control == 255 ? 0 : control;
    u8 type = static_cast<u8>(a->mType);
    if (type == 255) {
        a->scale.x = 1;
        a->scale.z = 1;
        a->scale.y = 1;
    } else if (type == 1) {
        a->scale.x = l_value_1_27;
        a->scale.y = l_value_1_27;
        a->scale.z = l_value_1_27;
    }
    a->mSwitch = (u32(static_cast<s16>(a->home.angle.x)) >> 8) & 255;
}

/* 0238A1F0 */
void daObj_Search::Act_c::CreateInit() {
    auto* a = this;
    a->set_path_info();
    u32 beam = address(a->mpBeamModel[0].get());
    a->mBeamCull[0] = 0;
    a->mBeamCull[1] = 0;
    a->mSearchCull = 0;
    if (beam)
        beam += 0xC8;
    f32 high = l_value_1000, low = l_value__1000, height = l_value_1500, zero = l_zero;
    a->cullMtx = beam;
    SearchHD_fopAcM_setCullSizeBox(a, low, zero, low, high, height, high);
    for (gptr<dBgW>* slot : {&a->mpBeamBackground[0], &a->mpBeamBackground[1], &a->mpBaseBackground})
        gabi::at<SearchBackgroundView>(address(slot->get()))->mCorrection =
            gabi::guest_fn((const void*)SearchBackground_correctPosition);
    for (gptr<dBgW>* slot : {&a->mpBeamBackground[0], &a->mpBeamBackground[1], &a->mpBaseBackground}) {
        auto* play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
        void* bg = gabi::at<void>(static_cast<u32>(slot->v));
        SearchHD_dBgS_Regist(&play->play.mBgS, bg, a);
    }
    SearchHD_dCcD_Stts_Init(&a->mStts[0], 255, 255, a);
    SearchHD_dCcD_Cps_Set(&a->mCps[0], const_cast<void*>(static_cast<const void*>(l_capsuleSource)));
    a->mCps[0].mStts.v = address(&a->mStts[0]);
    SearchHD_cM3dGCps_set(search_capsule(a->mCps[0]), &a->current.pos.x, &a->current.pos.x);
    f32 radius = l_value_460;
    search_capsule(a->mCps[0])->mRadius = radius;
    SearchHD_dCcD_Stts_Init(&a->mStts[1], 255, 255, a);
    SearchHD_dCcD_Cps_Set(&a->mCps[1], const_cast<void*>(static_cast<const void*>(l_capsuleSource)));
    a->mCps[1].mStts.v = address(&a->mStts[1]);
    SearchHD_cM3dGCps_set(search_capsule(a->mCps[1]), &a->current.pos.x, &a->current.pos.x);
    search_capsule(a->mCps[1])->mRadius = radius;
    a->set_mtx_base();
    a->set_moveBG_mtx_base();
    a->mpSearchModel->mUserArea = address(a);
    u32 data = static_cast<u32>(gabi::at<J3DModel>(address(a->mpSearchModel.get()))->mModelData.v);
    u16 index = 0;
    u32 table = address(SearchHD_J3DModelData_getJointTree(gabi::at<void>(data)));
    while (index < static_cast<u16>(gabi::at<SearchJointTreeView>(table)->mJointNum)) {
        if (index >= 1 && index <= 7) {
            u32 count = static_cast<u32>(gabi::at<J3DModelData>(data)->mJointNum),
                nodes = static_cast<u32>(gabi::at<J3DModelData>(data)->mJointNodes);
            u32 joint = index < count ? nodes + u32(index) * 0x1C : nodes;
            gabi::at<J3DJoint>(joint)->mCallBack = gabi::guest_fn((const void*)nodeControl_CB);
        }
        index = u16(index + 1);
        table = address(SearchHD_J3DModelData_getJointTree(gabi::at<void>(data)));
    }
    SearchHD_J3DModel_calc(a->mpSearchModel.get());
    if (a->is_path_info()) {
        s8 room = static_cast<s8>(a->current.roomNo);
        u8 pathId = static_cast<u8>(a->mPathId);
        void* path = SearchHD_dPath_GetRoomPath(pathId, room);
        a->mpPath.v = address(path);
        SearchHD_dLib_pathMove(&a->mPathTarget.x, &a->mPathPointIndex, path, 0, 0, l_value_20);
        u32 y = static_cast<u32>(search_bits(a->mPathTarget.y)),
            x = static_cast<u32>(search_bits(a->mPathTarget.x));
        search_bits(a->mInitialTarget[0].y) = y;
        u32 z = static_cast<u32>(search_bits(a->mPathTarget.z));
        search_bits(a->mInitialTarget[0].x) = x;
        search_bits(a->mInitialTarget[0].z) = z;
        SearchLocal<cXyz> diff;
        SearchHD_cXyz___mi(&a->mInitialTarget[0].x, diff.get(), &a->mBeamStart[0].x);
        s16 angle = static_cast<s16>(a->mBeamAngle[0].y);
        f32 dx = diff->x;
        a->mPreviousYaw = angle;
        f32 dz = diff->z;
        angle = static_cast<s16>(a->mBeamAngle[1].x);
        s16 ax = static_cast<s16>(a->mBeamAngle[0].x);
        a->mPreviousRoll = angle;
        f32 dy = diff->y;
        a->mPreviousPitch = ax;
        s32 yaw = static_cast<s32>(SearchHD_cM_atan2s(dx, dz));
        s16 base = static_cast<s16>(a->current.angle.y);
        yaw -= base;
        f32 len = SearchHD_sqrtf(gabi::fmadds(dx, dx, dz * dz));
        s32 pitch = static_cast<s32>(SearchHD_cM_atan2s(dy, len));
        a->mBeamAngle[1].y = yaw;
        a->mBeamAngle[0].x = pitch;
        a->mBeamAngle[0].y = yaw;
    }
    if (static_cast<u8>(a->mControl) == 6) {
        a->mBeamAngle[0].x = 0x4000;
        a->mBeamAngle[1].x = -0x4000;
    }
    SearchHD_daObj_Search_Act_c_set_mtx_light_A(a);
    SearchHD_daObj_Search_Act_c_set_mtx_light_B(a);
    a->set_moveBG_mtx_light_A();
    a->set_moveBG_mtx_light_B();
    a->mFindTimer = 0;
}

/* 0238AAA0 */
s32 daObj_Search::Act_c::_create() {
    auto* a = this;
    u32 condition = static_cast<u32>(a->actor_condition);
    if (!(condition & 8)) {
        if (a) {
            Act_ctor(a);
            condition = static_cast<u32>(a->actor_condition);
        }
        a->actor_condition = condition | 8;
    }
    s32 phase = static_cast<s32>(SearchHD_dComIfG_resLoad(&a->mPhase, "Search"));
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    a->SetArgData();
    if (!static_cast<s32>(SearchHD_fopAcM_entrySolidHeap(a, search_callback(createHeap_CB), 0x4620)))
        return 5;
    a->CreateInit();
    u32 x = static_cast<u32>(search_bits(a->current.pos.x)),
        z = static_cast<u32>(search_bits(a->current.pos.z));
    search_bits(a->mBeamStart[1].x) = x;
    u32 y = static_cast<u32>(search_bits(a->current.pos.y));
    search_bits(a->mBeamStart[0].z) = z;
    search_bits(a->mBeamStart[1].y) = y;
    u8 control = static_cast<u8>(a->mControl);
    search_bits(a->mBeamStart[1].z) = z;
    search_bits(a->mBeamStart[0].y) = y;
    search_bits(a->mBeamStart[0].x) = x;
    if (control == 6) {
        a->modeProc(PROC_INIT_e, MODE_SEARCH_BDK_e);
        initialize_beam_targets(a);
    } else if (control == 5) {
        a->mDisabled = 1;
        a->modeProc(PROC_INIT_e, MODE_STOP_e);
        initialize_beam_targets(a);
    } else {
        bool path = a->is_path_info();
        f32 one = l_one;
        SearchLocal<cXyz> unit, result;
        unit->x = one;
        unit->y = one;
        unit->z = one;
        if (path) {
            search_bits(a->mInitialTarget[0].z) = static_cast<u32>(search_bits(a->mPathTarget.z));
            search_bits(a->mInitialTarget[0].x) = static_cast<u32>(search_bits(a->mPathTarget.x));
            search_bits(a->mInitialTarget[0].y) = static_cast<u32>(search_bits(a->mPathTarget.y));
            SearchHD_cXyz___pl(&a->mBeamStart[1].x, result.get(), unit.get());
            vec_copy(&a->mInitialTarget[1], result.get());
            s8 room = l_stayRoom;
            auto* save = gabi::at<dSv_info_c>(dComIfGs_ea() + 0x20);
            u8 sw = static_cast<u8>(a->mKind);
            if (static_cast<s32>(SearchHD_dSv_info_c_isSwitch(save, sw, room))) {
                a->mBkControl = 0;
                a->modeProc(PROC_INIT_e, MODE_STOP_e);
                initialize_beam_targets(a);
            } else {
                a->mBkControl = 1;
                a->modeProc(PROC_INIT_e, MODE_SEARCH_PATH_e);
                a->set_mtx_light_A();
                a->set_mtx_light_B();
                SearchLocal<cXyz> diff;
                SearchHD_cXyz___mi(&a->mInitialTarget[0].x, diff.get(), &a->mBeamStart[0].x);
                f32 dx = diff->x, dz = diff->z, dy = diff->y;
                s32 yaw = static_cast<s32>(SearchHD_cM_atan2s(dx, dz));
                s16 base = static_cast<s16>(a->current.angle.y);
                yaw -= base;
                f32 len = SearchHD_sqrtf(gabi::fmadds(dx, dx, dz * dz));
                s32 pitch = static_cast<s32>(SearchHD_cM_atan2s(dy, len));
                a->mBeamAngle[0].x = pitch;
                a->mBeamAngle[0].y = yaw;
                initialize_beam_targets(a);
                a->mBeamAngle[1].y = yaw;
            }
        } else {
            SearchHD_cXyz___pl(&a->mBeamStart[0].x, result.get(), unit.get());
            vec_copy(&a->mInitialTarget[0], result.get());
            SearchHD_cXyz___pl(&a->mBeamStart[1].x, result.get(), unit.get());
            vec_copy(&a->mInitialTarget[1], result.get());
            a->modeProc(PROC_INIT_e, MODE_STOP_e);
            initialize_beam_targets(a);
        }
    }
    u8 kind = static_cast<u8>(a->mKind);
    s32 mode = a->initialMode(kind);
    f32 width = l_spotWidth[u32(mode)], zero = l_zero;
    a->mSpotWidth = width;
    a->mSpotFalloff = zero;
    kind = static_cast<u8>(a->mKind);
    mode = a->initialMode(kind);
    control = static_cast<u8>(a->mControl);
    a->mSpotOpacity = l_spotOpacity[u32(mode)];
    bool visible = false;
    if (control != 5) {
        auto* save = gabi::at<dSv_info_c>(dComIfGs_ea() + 0x20);
        f32 time = static_cast<f32>(save->mSavedata.mPlayer.mPlayerStatusB.mTime);
        visible = !(time < l_value_240) || !(time > l_value_60);
    }
    a->mAlpha = visible ? 255 : 0;
    a->mLightColor.g = 125;
    a->mLightColor.r = 235;
    static_cast<s32>(SearchHD_daObj_Search_Act_c__execute(a));
    static_cast<s32>(SearchHD_daObj_Search_Act_c__execute(a));
    return phase;
}

/* 0238C6E8 */
void daObj_Search::Act_c::smoke_set(f32 speed, s32 duration) {
    auto* a = this;
    u32 y = static_cast<u32>(search_bits(a->mModelLightPos.y)),
        emitter = static_cast<u32>(a->mSmokeCallback.mpEmitter.v);
    search_bits(a->mSmokePos.y) = y;
    u32 z = static_cast<u32>(search_bits(a->mModelLightPos.z)),
        x = static_cast<u32>(search_bits(a->mModelLightPos.x));
    search_bits(a->mSmokePos.z) = z;
    search_bits(a->mSmokePos.x) = x;
    a->mSmokeAngle.y = 0;
    a->mSmokeAngle.x = 0;
    a->mSmokeAngle.z = 0;
    if (!emitter) {
        s8 room = static_cast<s8>(a->current.roomNo);
        auto* play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
        u32 particles = static_cast<u32>(play->play.mParticle.v);
        SearchHD_dPa_control_c_set(gabi::at<void>(particles), 2, 0x2022, &a->mSmokePos.x, &a->mSmokeAngle.x,
                                   0, 0xB9, &a->mSmokeCallback, room, 0, 0, 0);
        emitter = static_cast<u32>(a->mSmokeCallback.mpEmitter.v);
    }
    if (emitter) {
        gabi::at<JPABaseEmitter>(emitter)->mRate = speed;
        f32 one = l_one, offset = l_value_2_5;
        emitter = static_cast<u32>(a->mSmokeCallback.mpEmitter.v);
        gabi::at<JPABaseEmitter>(emitter)->mSpread = one;
        f32 scale = l_smokeScaleOffset + offset;
        emitter = static_cast<u32>(a->mSmokeCallback.mpEmitter.v);
        f32 extra = l_value_3_5;
        gabi::at<SearchEmitterView>(emitter)->mParticleScale.x = scale;
        gabi::at<SearchEmitterView>(emitter)->mParticleScale.y = scale;
        gabi::at<SearchEmitterView>(emitter)->mParticleScale.z = scale;
        gabi::at<SearchEmitterView>(emitter)->mGlobalScale.x = scale;
        gabi::at<SearchEmitterView>(emitter)->mGlobalScale.y = scale;
        gabi::at<SearchEmitterView>(emitter)->mGlobalScale.z = scale;
        scale = l_smokeGlobalScale + extra;
        emitter = static_cast<u32>(a->mSmokeCallback.mpEmitter.v);
        gabi::at<SearchEmitterView>(emitter)->mGlobalScale.x = scale;
        gabi::at<SearchEmitterView>(emitter)->mGlobalScale.z = scale;
        gabi::at<SearchEmitterView>(emitter)->mGlobalScale.y = scale;
    }
    a->mSmokeTimer = duration;
}

/* 0238A7A4 */
s32 daObj_Search::Act_c::_execute() {
    auto* a = this;
    if (!static_cast<s32>(SearchHD_cLib_calcTimer_i(&a->mSmokeTimer)))
        SearchHD_dPa_smokeEcallBack_end(&a->mSmokeCallback);
    bool copied = false;
    s32 angleDelta;
    u8 control = static_cast<u8>(a->mControl);
    if (control != 6 && control != 5) {
        SearchLocal<be<u32>> id;
        u32 child = static_cast<u32>(a->mChildId);
        *id = child;
        void* controller = nullptr;
        if (child != 0xFFFFFFFF)
            controller = SearchHD_fopAcIt_Judge(search_callback(SearchJudge_actorId), id.get());
        if (controller) {
            s16 current = static_cast<s16>(a->mBeamAngle[0].y), previous = static_cast<s16>(a->mPreviousYaw);
            u32 x = static_cast<u32>(search_bits(a->current.pos.x)),
                y = static_cast<u32>(search_bits(a->current.pos.y));
            u8 sw = static_cast<u8>(gabi::at<SearchControllerView>(address(controller))->mSwitch);
            angleDelta = current - previous;
            search_bits(a->eyePos.x) = x;
            a->mChildSwitch = sw;
            u32 z = static_cast<u32>(search_bits(a->current.pos.z));
            search_bits(a->eyePos.y) = y;
            search_bits(a->eyePos.z) = z;
            copied = true;
        } else {
            u32 sw = static_cast<u32>(a->mChildSwitch);
            if (sw != 255) {
                auto* save = gabi::at<dSv_info_c>(dComIfGs_ea() + 0x20);
                s8 room = l_stayRoom;
                if (!static_cast<s32>(SearchHD_dSv_info_c_isSwitch(save, sw, room))) {
                    room = l_stayRoom;
                    save = gabi::at<dSv_info_c>(dComIfGs_ea() + 0x20);
                    sw = static_cast<u32>(a->mChildSwitch);
                    SearchHD_dSv_info_c_onSwitch(save, sw, room);
                }
            }
        }
    }
    if (!copied) {
        s16 current = static_cast<s16>(a->mBeamAngle[0].y), previous = static_cast<s16>(a->mPreviousYaw);
        u32 x = static_cast<u32>(search_bits(a->current.pos.x)),
            y = static_cast<u32>(search_bits(a->current.pos.y));
        angleDelta = current - previous;
        search_bits(a->eyePos.x) = x;
        u32 z = static_cast<u32>(search_bits(a->current.pos.z));
        search_bits(a->eyePos.y) = y;
        search_bits(a->eyePos.z) = z;
    }
    s16 magnitude = s16(angleDelta < 0 ? -angleDelta : angleDelta);
    if (magnitude && a->mMode != Act_c::MODE_STOP_e) {
        s8 room = static_cast<s8>(a->current.roomNo);
        s32 reverb = static_cast<s32>(SearchHD_dComIfGp_getReverb(room));
        SearchHD_mDoAud_seStart(0x6131, &a->eyePos.x, 0, reverb);
    }
    SearchHD_J3DModel_calc(a->mpSearchModel.get());
    SearchLocal<cXyz> zero;
    f32 value = l_zero;
    zero->y = value;
    zero->z = value;
    zero->x = value;
    u32 block = static_cast<u32>(gabi::at<J3DModel>(address(a->mpSearchModel.get()))->mpMtxBuffer.v);
    u16 flags = static_cast<u16>(gabi::at<J3DMtxBuffer_hd>(block)->mFlags);
    u32 array = static_cast<u32>(gabi::at<J3DMtxBuffer_hd>(block)->mpAnmMtx);
    gabi::at<J3DMtxBuffer_hd>(block)->mFlags = flags | 0x10;
    SearchHD_PSMTXCopy(gabi::at<void>(array + 0xF0), mDoMtx_stack_c::get());
    SearchHD_PSMTXMultVec(mDoMtx_stack_c::get(), zero.get(), &a->mModelLightPos.x);
    SearchHD_daObj_Search_Act_c_set_mtx_light_A(a);
    SearchHD_daObj_Search_Act_c_set_mtx_light_B(a);
    s32 frame = s32(u32(static_cast<s32>(a->mFrame)) + 1);
    if (frame >= 4)
        frame = 0;
    a->mFrame = frame;
    auto* save = gabi::at<dSv_info_c>(dComIfGs_ea() + 0x20);
    f32 time = static_cast<f32>(save->mSavedata.mPlayer.mPlayerStatusB.mTime);
    bool active;
    if (time >= l_value_240)
        active = !static_cast<u8>(a->mDisabled);
    else if (time > l_value_60)
        active = false;
    else
        active = !static_cast<u8>(a->mDisabled);
    a->mActive = active ? 1 : 0;
    s16 speed = s16(l_yawRate + 20);
    SearchHD_cLib_chaseS(&a->mAlpha, active ? 255 : 0, speed);
    SearchLocal<be<s16>> profile;
    *profile = 0xF1;
    void* found = SearchHD_fopAcIt_Judge(search_callback(SearchJudge_actorName), profile.get());
    if (static_cast<s32>(a->mSwitch) == 255 || !found) {
        if (static_cast<u8>(a->mControl) != 5)
            a->check_bk_control();
    }
    a->modeProc(PROC_EXEC_e, 8);
    a->bg_check();
    return 1;
}

/* 0238A684 */
void daObj_Search::Act_c::check_bk_control() {
    auto* a = this;
    SearchLocal<be<u32>> id;
    u32 child = static_cast<u32>(a->mChildId);
    *id = child;
    void* controller = nullptr;
    if (child != 0xFFFFFFFF)
        controller = SearchHD_fopAcIt_Judge(search_callback(SearchJudge_actorId), id.get());
    if (!controller)
        return;
    s16 angle = static_cast<s16>(a->mGearAngle[2]);
    a->mDisabled = 0;
    gabi::at<SearchControllerView>(address(controller))->mBeamYaw = angle;
    u8 control = a->mBkControl;
    s32 mode = a->mMode;
    if (control) {
        if (mode == 4 || mode == 2)
            a->modeProc(PROC_INIT_e, MODE_TO_SEARCH_e);
    } else if (mode != 4 && mode != 2)
        a->modeProc(PROC_INIT_e, MODE_TO_STOP_e);
}

/* 0238987C */
void daObj_Search::Act_c::set_mtx_base() {
    auto* a = this;
    f32 x = static_cast<f32>(a->scale.x), y = static_cast<f32>(a->scale.y);
    u32 model = address(a->mpSearchModel.get());
    f32 z = static_cast<f32>(a->scale.z);
    gabi::at<J3DModel>(model)->mBaseScale.x = x;
    gabi::at<J3DModel>(model)->mBaseScale.y = y;
    gabi::at<J3DModel>(model)->mBaseScale.z = z;
    x = static_cast<f32>(a->current.pos.x);
    y = static_cast<f32>(a->current.pos.y);
    z = static_cast<f32>(a->current.pos.z);
    SearchHD_PSMTXTrans(mDoMtx_stack_c::get(), x, y, z);
    s16 angle = static_cast<s16>(a->current.angle.y);
    SearchHD_mDoMtx_YrotM(mDoMtx_stack_c::get(), angle);
    mtx_copy(&a->mpSearchModel.get()->mBaseTransformMtx, mDoMtx_stack_c::get());
}

/* 02389A24 */
void daObj_Search::Act_c::set_mtx_light_A() {
    auto* a = this;
    light_matrix(a, false);
}

/* 02389DC4 */
void daObj_Search::Act_c::set_mtx_light_B() {
    auto* a = this;
    light_matrix(a, true);
}

/* 02389954 */
void daObj_Search::Act_c::set_moveBG_mtx_base() {
    auto* a = this;
    u32 model = address(a->mpSearchModel.get());
    f32 y = static_cast<f32>(a->scale.y), x = static_cast<f32>(a->scale.x), z = static_cast<f32>(a->scale.z);
    gabi::at<J3DModel>(model)->mBaseScale.x = x;
    gabi::at<J3DModel>(model)->mBaseScale.y = y;
    gabi::at<J3DModel>(model)->mBaseScale.z = z;
    x = static_cast<f32>(a->current.pos.x);
    y = static_cast<f32>(a->current.pos.y);
    z = static_cast<f32>(a->current.pos.z);
    SearchHD_PSMTXTrans(mDoMtx_stack_c::get(), x, y, z);
    s16 angle = static_cast<s16>(a->current.angle.y);
    SearchHD_mDoMtx_YrotM(mDoMtx_stack_c::get(), angle);
    SearchHD_PSMTXCopy(mDoMtx_stack_c::get(), &a->mBaseMatrix);
    SearchHD_dBgW_Move(a->mpBaseBackground.get());
}

/* 0238A0B8 */
void daObj_Search::Act_c::set_moveBG_mtx_light_A() {
    auto* a = this;
    u32 model = address(a->mpSearchModel.get()),
        block = static_cast<u32>(gabi::at<J3DModel>(model)->mpMtxBuffer.v);
    u16 flags = static_cast<u16>(gabi::at<J3DMtxBuffer_hd>(block)->mFlags);
    u32 matrices = static_cast<u32>(gabi::at<J3DMtxBuffer_hd>(block)->mpAnmMtx);
    gabi::at<J3DMtxBuffer_hd>(block)->mFlags = flags | 0x10;
    SearchHD_PSMTXCopy(gabi::at<void>(matrices + 240), mDoMtx_stack_c::get());
    SearchHD_mDoMtx_XYZrotM(mDoMtx_stack_c::get(), 0, 0, -0x4000);
    f32 z = l_value_30, x = l_value_40, y = l_value__270;
    SearchHD_mDoMtx_stack_c_transM(x, y, z);
    SearchHD_PSMTXCopy(mDoMtx_stack_c::get(), &a->mBeamMatrix[0]);
    SearchHD_dBgW_Move(a->mpBeamBackground[0].get());
}

/* 0238A154 */
void daObj_Search::Act_c::set_moveBG_mtx_light_B() {
    auto* a = this;
    u32 model = address(a->mpSearchModel.get()),
        block = static_cast<u32>(gabi::at<J3DModel>(model)->mpMtxBuffer.v);
    u16 flags = static_cast<u16>(gabi::at<J3DMtxBuffer_hd>(block)->mFlags);
    u32 matrices = static_cast<u32>(gabi::at<J3DMtxBuffer_hd>(block)->mpAnmMtx);
    gabi::at<J3DMtxBuffer_hd>(block)->mFlags = flags | 0x10;
    SearchHD_PSMTXCopy(gabi::at<void>(matrices + 288), mDoMtx_stack_c::get());
    SearchHD_mDoMtx_XYZrotM(mDoMtx_stack_c::get(), 0, 0, -0x4000);
    f32 z = l_value_35, x = l_value_45, y = l_value__270;
    SearchHD_mDoMtx_stack_c_transM(x, y, z);
    SearchHD_PSMTXCopy(mDoMtx_stack_c::get(), &a->mBeamMatrix[1]);
    SearchHD_dBgW_Move(a->mpBeamBackground[1].get());
}

/* 0238A73C */
void daObj_Search::Act_c::bg_check() {
    auto* a = this;
    auto* play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
    void* player = gabi::at<void>(static_cast<u32>(play->play.mPlayerInfo[0].mpPlayer.v));
    f32 distance = SearchHD_fopAcM_searchActorDistance(a, player);
    if (!(distance > l_value_2000) && static_cast<u8>(a->mSearchCull) == 0) {
        SearchHD_daObj_Search_Act_c_set_moveBG_mtx_light_A(a);
        SearchHD_daObj_Search_Act_c_set_moveBG_mtx_light_B(a);
    }
}

/* 0238B838 */
s32 daObj_Search::Act_c::player_check() {
    auto* a = this;
    auto* play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
    u32 player = static_cast<u32>(play->play.mPlayerInfo[0].mpPlayer.v);
    SearchLocal<cXyz> temporary, copy;
    SearchHD_cXyz___mi(gabi::at<void>(player + 0x314), temporary.get(), &a->mBeamStart[0].x);
    vec_copy(copy.get(), temporary.get());
    f32 sq = SearchHD_PSVECSquareMag(copy.get());
    f32 distanceA = SearchHD_sqrtf(sq);
    SearchHD_cXyz___mi(gabi::at<void>(player + 0x314), temporary.get(), &a->mBeamStart[1].x);
    vec_copy(copy.get(), temporary.get());
    sq = SearchHD_PSVECSquareMag(copy.get());
    f32 distanceB = SearchHD_sqrtf(sq);
    SearchHD_cM3dGCps_set(search_capsule(a->mCps[0]), &a->mBeamStart[0].x, &a->mBeamEnd[0].x);
    f32 radius = l_value_460;
    search_capsule(a->mCps[0])->mRadius = radius;
    play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
    SearchHD_cCcS_Set(&play->play.mCcS, &a->mCps[0]);
    s32 hit = static_cast<s32>(SearchHD_dCcD_GObjInf_ChkCoHit(&a->mCps[0]));
    f32 speed = l_value_0_1, far = l_value_2000, zero = l_zero;
    if (hit) {
        u32 other = address(SearchHD_dCcD_GAtTgCoCommonBase_GetAc(&a->mCps[0].mGObjCo));
        project_hit(a, other, address(&a->mBeamStart[0]), address(&a->mBeamEnd[0]), false);
        if (other && static_cast<s16>(gabi::at<fopAc_ac_c>(other)->mProcName) == 168) {
            if (distanceA < far || distanceB < far)
                return 0;
            if (!(std::fabs(static_cast<f32>(gabi::at<daPy_py_c>(player)->speedF)) < speed) ||
                !(static_cast<f32>(gabi::at<daPy_py_c>(player)->field_0x2b0) < zero))
                return 1;
        }
    }
    SearchHD_cM3dGCps_set(search_capsule(a->mCps[1]), &a->mBeamStart[1].x, &a->mBeamEnd[1].x);
    search_capsule(a->mCps[1])->mRadius = radius;
    play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
    SearchHD_cCcS_Set(&play->play.mCcS, &a->mCps[1]);
    if (static_cast<s32>(SearchHD_dCcD_GObjInf_ChkCoHit(&a->mCps[1]))) {
        u32 other = address(SearchHD_dCcD_GAtTgCoCommonBase_GetAc(&a->mCps[1].mGObjCo));
        project_hit(a, other, address(&a->mBeamStart[1]), address(&a->mBeamEnd[1]), true);
        if (distanceA < far)
            return 0;
        if (distanceB < far)
            return 0;
        if (!(std::fabs(static_cast<f32>(gabi::at<daPy_py_c>(player)->speedF)) < speed) ||
            !(static_cast<f32>(gabi::at<daPy_py_c>(player)->field_0x2b0) < zero))
            return 1;
    }
    if (!static_cast<s32>(SearchHD_dCcD_GObjInf_ChkCoHit(&a->mCps[0])) &&
        !static_cast<s32>(SearchHD_dCcD_GObjInf_ChkCoHit(&a->mCps[1]))) {
        using Line = dBgS_LinChk;
        SearchLocal<Line> line;
        u32 p = address(line.get());
        SearchHD_cBgS_LinChk_ct(line.get());
        line->cBgS_Chk::mPolyPassChk = p + 0x58;
        line->mGrpPassChk = address(static_cast<dBgS_GrpPassChk*>(line.get()));
        line->mbBombThrough = 0;
        line->mbLinkThrough = 0;
        line->mbObjThrough = 0;
        line->cBgS_Chk::__vtbl = 0x1002EF34;
        line->mbRopeThrough = 0;
        line->mbArrowThrough = 0;
        line->dBgS_PolyPassChk::__vtbl = 0x1002EF64;
        line->dBgS_GrpPassChk::__vtbl = 0x1002EF54;
        line->cBgS_PolyInfo::__vtbl = 0x1002EF44;
        line->mGrp = 3;
        line->mbBoomerangThrough = 0;
        line->mbCamThrough = 1;
        SearchHD_dBgS_LinChk_Set(line.get(), &a->mBeamStart[0].x, &a->mBeamEnd[0].x, 0);
        play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
        if (static_cast<s32>(SearchHD_cBgS_LineCross(&play->play.mBgS, line.get()))) {
            a->mSpotPos.z = static_cast<f32>(line->mLin.mEnd.z);
            f32 height = static_cast<f32>(line->mLin.mEnd.y);
            a->mSpotPos.y = height;
            a->mSpotPos.x = static_cast<f32>(line->mLin.mEnd.x);
            u8 kind = static_cast<u8>(a->mKind);
            s32 mode = a->initialMode(kind);
            line->dBgS_PolyPassChk::__vtbl = 0x1002EEE4;
            line->dBgS_GrpPassChk::__vtbl = 0x1002EEA4;
            line->cBgS_PolyInfo::__vtbl = 0x1002EE94;
            a->mSpotPos.y = height + l_spotHeight[u32(mode)];
        } else {
            u32 y = static_cast<u32>(search_bits(a->mBeamStart[0].y)),
                x = static_cast<u32>(search_bits(a->mBeamStart[0].x)),
                z = static_cast<u32>(search_bits(a->mBeamStart[0].z));
            line->dBgS_PolyPassChk::__vtbl = 0x1002EEE4;
            search_bits(a->mSpotPos.x) = x;
            search_bits(a->mSpotPos.y) = y;
            line->dBgS_GrpPassChk::__vtbl = 0x1002EEA4;
            line->cBgS_PolyInfo::__vtbl = 0x1002EE94;
            search_bits(a->mSpotPos.z) = z;
        }
        SearchHD_cBgS_LinChk_destruct(line.get(), 0);
    }
    u8 kind = static_cast<u8>(a->mKind);
    s32 mode = a->initialMode(kind);
    a->mSpotOpacity = l_spotOpacity[u32(mode)];
    kind = static_cast<u8>(a->mKind);
    mode = a->initialMode(kind);
    a->mSpotWidth = l_spotWidth[u32(mode)];
    return 0;
}

/* 02389860 */
s32 daObj_Search::Act_c::set_path_info() {
    auto* a = this;
    u8 path = static_cast<u8>(a->mPathId);
    return path == 255 ? 0 : static_cast<s32>(SearchHD_dLib_pathInfo(&a->mpPath.v, path));
}

/* 023899D8 */
bool daObj_Search::Act_c::is_path_info() {
    auto* a = this;
    return static_cast<u8>(a->mPathId) != 255;
}

/* 0238B258 */
s32 daObj_Search::Act_c::_draw() {
    auto* a = this;
    l_farClip = l_value_1000000;
    SearchHD_J3DUClipper_calcViewFrustum(gabi::at<void>(0x1048CFF0));
    u32 matrix = address(a->mpSearchModel.get());
    if (matrix)
        matrix += 0xC8;
    f32 low = l_value__1000, high = l_value_1000, zero = l_zero, height = l_value_1500;
    u8 culled = static_cast<u8>(
        SearchHD_fopAcM_checkCullingBox(gabi::at<void>(matrix), low, zero, low, high, height, high));
    matrix = address(a->mpBeamModel[0].get());
    a->mSearchCull = culled;
    if (matrix)
        matrix += 0xC8;
    f32 bHigh = l_value_600, bLow = l_value__600, near = l_value__100, far = l_value_9585_62;
    culled = static_cast<u8>(
        SearchHD_fopAcM_checkCullingBox(gabi::at<void>(matrix), bLow, bLow, near, bHigh, bHigh, far));
    matrix = address(a->mpBeamModel[1].get());
    a->mBeamCull[0] = culled;
    if (matrix)
        matrix += 0xC8;
    near = l_value__100;
    far = l_value_9585_62;
    culled = static_cast<u8>(
        SearchHD_fopAcM_checkCullingBox(gabi::at<void>(matrix), bLow, bLow, near, bHigh, bHigh, far));
    a->mBeamCull[1] = culled;
    l_farClip = l_defaultFarClip;
    SearchHD_J3DUClipper_calcViewFrustum(gabi::at<void>(0x1048CFF0));
    if (!static_cast<u8>(a->mSearchCull)) {
        if (static_cast<u8>(a->mType) == 1) {
            u32 env = address(SearchHD_dScnKy_env_light_c_setLightTevColorType());
            void* again = SearchHD_dScnKy_env_light_c_setLightTevColorType();
            SearchHD_dScnKy_env_light_c_settingTevStruct(again, 1, &a->current.pos.x, &a->tevStr);
            for (u32 i = 0; i < 3; i++)
                search_color_component(a->tevStr.mColorC0, i) = search_environment_color(env, i);
            for (u32 i = 0; i < 3; i++)
                search_color_component(a->tevStr.mColorK0, i) = search_environment_color(env, 4 + i);
        } else {
            void* env = SearchHD_dScnKy_env_light_c_setLightTevColorType();
            SearchHD_dScnKy_env_light_c_settingTevStruct(env, 0, &a->current.pos.x, &a->tevStr);
        }
        void* env = SearchHD_dScnKy_env_light_c_setLightTevColorType();
        SearchHD_dKy_setLightTevColorType_MAJI(env, a->mpSearchModel.get(), &a->tevStr);
        auto* play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
        l_drawListFirst =
            static_cast<u32>(gabi::at<SearchDrawListView>(address(&play->drawlist))->mpOpaListBG);
        play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
        l_drawListLast =
            static_cast<u32>(gabi::at<SearchDrawListView>(address(&play->drawlist))->mpXluListBG);
        SearchHD_mDoExt_modelEntryDL(a->mpSearchModel.get());
        play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
        l_drawListFirst = static_cast<u32>(gabi::at<SearchDrawListView>(address(&play->drawlist))->mpOpaList);
        play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
        l_drawListLast = static_cast<u32>(gabi::at<SearchDrawListView>(address(&play->drawlist))->mpXluList);
    }
    if (!static_cast<u8>(a->mBeamCull[0]) && static_cast<s16>(a->mAlpha) > 0) {
        SearchLocal<cXyz> difference, normal, secondDifference, direction, point, position;
        SearchHD_cXyz___mi(&a->mBeamEnd[0].x, difference.get(), &a->mBeamStart[0].x);
        SearchHD_cXyz_norm(difference.get(), normal.get());
        SearchHD_cXyz___mi(&a->mBeamEnd[0].x, secondDifference.get(), &a->mBeamStart[0].x);
        f32 sq = SearchHD_PSVECSquareMag(secondDifference.get());
        f32 length = SearchHD_sqrtf(sq);
        direction->z = normal->z;
        direction->y = normal->y;
        direction->x = normal->x;
        u8 kind = static_cast<u8>(a->mKind);
        s32 mode = a->initialMode(kind);
        f32 offset = l_spotHeight[u32(mode)];
        SearchHD_PSVECScale(direction.get(), direction.get(), offset);
        SearchHD_cXyz___pl(&a->mBeamStart[0].x, point.get(), direction.get());
        s16 red = static_cast<s16>(a->mSpotColor[0]), green = static_cast<s16>(a->mSpotColor[1]);
        position->z = point->z;
        direction->x = normal->x;
        position->x = point->x;
        direction->y = normal->y;
        s16 blue = static_cast<s16>(a->mSpotColor[2]);
        position->y = point->y;
        direction->z = normal->z;
        struct Color {
            be<f32> r, g, b, a;
        };
        SearchLocal<Color> color;
        f32 full = l_opaqueAlpha;
        color->r = f32(red) / full;
        color->a = l_one;
        color->g = f32(green) / full;
        color->b = f32(blue) / full;
        kind = static_cast<u8>(a->mKind);
        mode = a->initialMode(kind);
        f32 width = l_spotWidth[u32(mode)];
        kind = static_cast<u8>(a->mKind);
        mode = a->initialMode(kind);
        offset = l_spotHeight[u32(mode)];
        void* renderer = gabi::at<void>(l_lightRenderer);
        f32 radius = length - offset + high;
        zero = l_zero;
        f32 opacity = l_value_0_3;
        SearchHD_dDlst_2D_LightSpot_draw(renderer, position.get(), direction.get(), color.get(), 0,
                                         gabi::at<void>(0x104A01CC), width, radius, zero, opacity);
        SearchHD_mDoExt_modelUpdateDL(a->mpBeamModel[0].get(), 0);
    }
    if (!static_cast<u8>(a->mBeamCull[1]) && static_cast<s16>(a->mAlpha) > 0)
        SearchHD_mDoExt_modelUpdateDL(a->mpBeamModel[1].get(), 0);
    return 1;
}

/* 0238B16C */
s32 daObj_Search::Act_c::_delete() {
    auto* a = this;
    SearchHD_dKy_plight_cut(&a->mSpotPos.x);
    for (gptr<dBgW>* slot : {&a->mpBeamBackground[0], &a->mpBeamBackground[1], &a->mpBaseBackground}) {
        u32 bg = static_cast<u32>(slot->v);
        if (bg && static_cast<u32>(gabi::at<dBgW>(bg)->m_id) < 0x100) {
            auto* play = static_cast<dComIfG_inf_c*>(SearchHD_dComIfGp_getPlay());
            bg = static_cast<u32>(slot->v);
            SearchHD_cBgS_Release(&play->play.mBgS, gabi::at<void>(bg));
        }
    }
    SearchHD_dComIfG_resDelete(&a->mPhase, "Search");
    u32 vtable = static_cast<u32>(a->mSmokeCallback.__vtbl), target = search_virtual(vtable, 0x44);
    gabi::call<void>(target, &a->mSmokeCallback);
    if (static_cast<s32>(SearchHD_mDoAud_bgmCheck(0x834)))
        SearchHD_mDoAud_bgmStop(0x834, 0);
    return 1;
}

/* 0238DB3C */
s32 daObj_Search::Mthd::Create(Act_c* a) { return static_cast<s32>(SearchHD_daObj_Search_Act_c__create(a)); }

/* 0238DB40 */
s32 daObj_Search::Mthd::Delete(Act_c* a) { return static_cast<s32>(SearchHD_daObj_Search_Act_c__delete(a)); }

/* 0238DB44 */
s32 daObj_Search::Mthd::Execute(Act_c* a) {
    return static_cast<s32>(SearchHD_daObj_Search_Act_c__execute(a));
}

/* 0238DB48 */
s32 daObj_Search::Mthd::Draw(Act_c* a) { return static_cast<s32>(SearchHD_daObj_Search_Act_c__draw(a)); }

/* 0238DB4C */
s32 daObj_Search::Mthd::IsDelete(Act_c* a) { return 1; }

/* 023899EC */
bool daObj_Search::Act_c::isSecond(u32 mode) {
    auto* a = this;
    u32 expected = static_cast<s32>(a->mSwitch) == 255 ? 10 : 103;
    return mode == expected || a->mMode == Act_c::MODE_SEARCH_BDK_e;
}

/* 0238A5EC */
s32 daObj_Search::Act_c::initialMode(u32 kind) {
    auto* a = this;
    if (kind == 10 || kind == 102) {
        auto* save = gabi::at<dSv_info_c>(dComIfGs_ea() + 0x20);
        if (static_cast<s32>(SearchHD_dSv_info_c_isSwitch(save, 0x2A, 1)) &&
            static_cast<s32>(a->mSwitch) != 255)
            return 3;
        return 1;
    }
    if (kind == 11)
        return 2;
    if (kind == 103)
        return 4;
    return 0;
}

// HD: compiler-generated and template code of this translation unit.

/* 0238DCF0 */
void search_SafeString_assureTermination(void* p) {}

/* 0238DBE8 */
void search_static_dtor(void* p, u32 flags) {
    if (p && (flags & 1))
        SearchHD_dl(p);
}

/* 0238DBFC */
void* search_status_ctor(void* p) {
    if (!p) {
        p = SearchHD_operator_new(0x3C);
        if (!p)
            return p;
    }
    SearchHD_cCcD_Stts_construct(p);
    SearchHD_dCcD_GStts_dCcD_GStts(gabi::at<void>(address(p) + 0x1C));
    gabi::at<dCcD_Stts>(address(p))->__vtbl = 0x1004AE88;
    gabi::at<dCcD_Stts>(address(p))->__vtbl_gstts = 0x1004AEC0;
    return p;
}

/* 0238DC64 */
void* search_capsule_ctor(void* p) {
    if (!p) {
        p = SearchHD_operator_new(0x138);
        if (!p)
            return p;
    }
    SearchHD_dCcD_GObjInf_dCcD_GObjInf(p);
    search_shape(*gabi::at<dCcD_Cps>(address(p)))->mGeometryVtable = 0x100015A8;
    search_shape(*gabi::at<dCcD_Cps>(address(p)))->mShapeVtable = 0x1002EE84;
    SearchHD_cM3dGCps_construct(gabi::at<void>(address(p) + 0x118));
    gabi::at<dCcD_Cps>(address(p))->__vtbl_hitinf = 0x1004AF18;
    search_capsule(*gabi::at<dCcD_Cps>(address(p)))->__vtbl = 0x1004AF60;
    search_shape(*gabi::at<dCcD_Cps>(address(p)))->mGeometryVtable = 0x1004AF70;
    return p;
}

/* 02389524 */
Bgc_c* Bgc_ctor(Bgc_c* p) {
    if (!p) {
        p = static_cast<Bgc_c*>(SearchHD_operator_new(0x78));
        if (!p)
            return p;
    }
    SearchHD_cBgS_LinChk_ct(p);
    p->mbBoomerangThrough = 0;
    p->mbBombThrough = 0;
    p->cBgS_Chk::__vtbl = 0x1002EEB4;
    p->mbObjThrough = 0;
    p->cBgS_Chk::mGrpPassChk = address(p) + 0x64;
    p->dBgS_GrpPassChk::__vtbl = 0x1002EED4;
    p->mPolyPassChk = address(static_cast<dBgS_PolyPassChk*>(p));
    p->mbLinkThrough = 0;
    p->dBgS_PolyPassChk::__vtbl = 0x1002EEE4;
    p->mbArrowThrough = 0;
    p->cBgS_PolyInfo::__vtbl = 0x1002EEC4;
    p->mGrp = 1;
    p->mbRopeThrough = 0;
    p->mbCamThrough = 0;
    return p;
}

/* 023896BC */
Act_c* Act_ctor(Act_c* p) {
    if (!p) {
        p = static_cast<Act_c*>(SearchHD_operator_new(0xA28));
        if (!p)
            return p;
    }
    SearchHD_fopAc_ac_c_fopAc_ac_c(p);
    p->__vtbl = 0x1002EFB4;
    Bgc_ctor(gabi::at<Bgc_c>(address(&p->mBgc.cBgS_Chk::mPolyPassChk)));
    SearchHD_construct_array(&p->mCps[0], 2, 0x138, search_callback(search_capsule_ctor));
    SearchHD_construct_array(&p->mStts[0], 2, 0x3C, search_callback(search_status_ctor));
    p->mSpotOpacity = l_one;
    SearchHD_dPa_smokeEcallBack_dPa_smokeEcallBack(&p->mSmokeCallback, 1);
    SearchHD_cBgS_LinChk_ct(&p->mPlayerLine.cBgS_Chk::mPolyPassChk);
    p->mPlayerLine.cBgS_Chk::mPolyPassChk = address(p) + 0x9DC;
    p->mPlayerLine.mbLinkThrough = 0;
    p->mPlayerLine.mbBombThrough = 0;
    p->mPlayerLine.dBgS_PolyPassChk::__vtbl = 0x1002EFA4;
    p->mPlayerLine.cBgS_Chk::__vtbl = 0x1002EF74;
    p->mPlayerLine.mGrp = 1;
    p->mPlayerLine.mbObjThrough = 1;
    p->mPlayerLine.mbArrowThrough = 0;
    p->mPlayerLine.mbCamThrough = 0;
    p->mPlayerLine.cBgS_Chk::mGrpPassChk = address(p) + 0x9E8;
    p->mPlayerLine.cBgS_PolyInfo::__vtbl = 0x1002EF84;
    p->mPlayerLine.dBgS_GrpPassChk::__vtbl = 0x1002EF94;
    p->mPlayerLine.mbRopeThrough = 0;
    p->mPlayerLine.mbBoomerangThrough = 0;
    return p;
}

/* 0238DB54 */
void search_sinit() {
    l_headerZeroC = 0;
    l_headerZeroA = 0;
    l_headerZeroD = 0;
    l_headerZeroB = 0;
    SearchHD_register_global_object(gabi::at<void>(0x101CCA0C));
    l_headerMinusPi = l_value__3_141593;
    l_headerPi = l_value_3_141593;
    SearchHD_initializeHeaderByte(gabi::at<void>(0x1046BDAC));
    SearchHD_register_global_object(gabi::at<void>(0x101CCA18));
    SearchHD_initializeHeaderFlag(gabi::at<void>(0x1046BDAD));
    SearchHD_register_global_object(gabi::at<void>(0x101CCA24));
}

/* 0238DD04 */
void Act_dtor(Act_c* a, u32 flags) {
    if (!a)
        return;
    a->mPlayerLine.dBgS_PolyPassChk::__vtbl = 0x1002EEE4;
    a->mPlayerLine.dBgS_GrpPassChk::__vtbl = 0x1002EEA4;
    a->mPlayerLine.cBgS_PolyInfo::__vtbl = 0x1002EE94;
    SearchHD_cBgS_LinChk_destruct(&a->mPlayerLine.cBgS_Chk::mPolyPassChk, 0);
    SearchHD_destroy_arr(&a->mStts[0], 2, 0x3C, search_callback(SearchDestroy_status), 0, 0);
    SearchHD_destroy_arr(&a->mCps[0], 2, 0x138, search_callback(SearchDestroy_capsule), 0, 0);
    a->mBgc.dBgS_PolyPassChk::__vtbl = 0x1002EEE4;
    a->mBgc.dBgS_GrpPassChk::__vtbl = 0x1002EEA4;
    a->mBgc.cBgS_PolyInfo::__vtbl = 0x1002EE94;
    SearchHD_cBgS_LinChk_destruct(&a->mBgc.cBgS_Chk::mPolyPassChk, 0);
    SearchHD_fopAc_ac_c_fopAc_ac_c(a, 0);
    if (flags & 1)
        SearchHD_dl(a);
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py): HD ABI bindings.
str 1002F218
str 1002F208
str 1002F1C4
data l_trackingOffsetGuard0 1046BE08 nocheck
data l_trackingOffsetGuard1 1046BE0C nocheck
data l_birdTurnRateOther 1047BB18 nocheck
data l_birdTurnRateFirst 1047BB16 nocheck
ext 024EE658 void SearchBackground_correctPosition(void*) callback
data l_trackingOffset5 1046BDFC nocheck
data l_trackingOffset4 1046BDF0 nocheck
data l_trackingOffset3 1046BDE4 nocheck
data l_trackingOffset2 1046BDD8 nocheck
data l_trackingOffset1 1046BDCC nocheck
data l_trackingOffset0 1046BDC0 nocheck
ext 02515980 void SearchDestroy_capsule(void*, u32) callback
ext 02515860 void SearchDestroy_status(void*, u32) callback
ext 025E121C void* SearchJudge_actorName(void*, void*) callback
ext 025E1234 void* SearchJudge_actorId(void*, void*) callback
data l_beamTuning9 1002F044
data l_beamTuning8 1002F040
data l_beamTuning7 1002F03C
data l_beamTuning6 1002F034
data l_beamTuning5 1002F030
data l_beamTuning4 1002F02C
data l_beamTuning3 1002F028
data l_beamTuning2 1002F020
data l_beamTuning1 1002F01C
data l_beamTuning0 1002F018
data l_capsuleSource 101CC9B8
data l_eventMode 101D6010
data l_lightRenderer 101F8A20
data l_headerMinusPi 1046BDA4 nocheck
data l_headerPi 1046BDA8 nocheck
data l_headerZeroA 1046BDB0 nocheck
data l_headerZeroB 1046BDB4 nocheck
data l_headerZeroC 1046BDB8 nocheck
data l_headerZeroD 1046BDBC nocheck
data l_trackingOffsetGuard2 1046BE10 nocheck
data l_trackingOffsetGuard3 1046BE14 nocheck
data l_trackingOffsetGuard4 1046BE18 nocheck
data l_trackingOffsetGuard5 1046BE1C nocheck
data l_birdCircleRange 1047BA90 nocheck
data l_birdFanRange 1047BAA4 nocheck
data l_birdAngle 1047BB08 nocheck
data l_birdDirection 1047BB0A nocheck
data l_birdRadiusModifier 1047BB0C nocheck
data l_birdYawAdjustment 1047BB10 nocheck
data l_extraRange 1047BCD0 nocheck
data l_birdTargetHeight 1047BCD4 nocheck
data l_birdHeight 1047BCD8 nocheck
data l_smokeScaleOffset 1047BCF8 nocheck
data l_smokeGlobalScale 1047BCFC nocheck
data l_yawRate 1047BD48 nocheck
data l_beamPitchOffset 1047BD4A nocheck
data l_randomYawOff 1047BD50 nocheck
data l_stayRoom 1047E6C8 nocheck
data l_farClip 1048D044 nocheck
data l_defaultFarClip 1048D04C nocheck
data l_currentModel 104B462C nocheck
data l_drawListFirst 104B4634 nocheck
data l_drawListLast 104B4638 nocheck
data l_modeTable 1002F070
str 1002F110 in 0238A540
str 1002F120 in 0238A540
str 1002F134 in 0238A540
str 1002F140 in 0238A540
str 1002F150 in 0238A540
str 1002F160 in 0238A540
str 1002F16C in 0238A540
str 1002F17C in 0238A540
data l_value_5 1002EFF8
data l_value_2_1017 1002EFFC
data l_value_1_4013 1002F000
data l_value_1_05 1002F004
data l_one 1002F008
data l_value_1_27 1002F00C
data l_zero 1002F010
data l_value_16000 1002F014
data l_value_1_02 1002F024
data l_value_4000 1002F030
data l_value_0_0001043 1002F038
data l_value_40 1002F048
data l_value__270 1002F04C
data l_value_30 1002F050
data l_value_45 1002F054
data l_value_35 1002F058
data l_value__1000 1002F05C
data l_value_1000 1002F060
data l_value_1500 1002F064
data l_value_460 1002F068
data l_value_20 1002F06C
data l_value_2000 1002F18C
data l_value_240 1002F190
data l_value_60 1002F194
data l_value_1000000 1002F198
data l_value__600 1002F19C
data l_value_600 1002F1A0
data l_value__100 1002F1A4
data l_value_9585_62 1002F1A8
data l_opaqueAlpha 1002F1AC
data l_value_0_1 1002F1B0
data l_value_80 1002F1B4
data l_hundred 1002F1B8
data l_value_2_5 1002F1BC
data l_value_3_5 1002F1C0
data l_value_3600 1002F280
data l_value_10000 1002F284
data l_value__3800 1002F288
data l_value_15000 1002F28C
data l_value_3000 1002F290
data l_value_500 1002F294
data l_value_300 1002F298
data l_value_200 1002F29C
data l_value__3_141593 1002F2A0
data l_value_3_141593 1002F2A4
data l_value_0_3 101CCAA8
data l_beamCollision 101CCA04
data l_spotHeight 101CCA80
data l_spotWidth 101CCA94
data l_spotOpacity 101CCAA8
str 1002EFC4 in 02388F20
str 1002EFD8
str 1002F1CC in 0238C834
str 1002F1D4
str 1002F1E4
str 1002F1F4 in 0238C834
str 1002F224
str 1002F22C in 0238CC18
str 1002F234
str 1002F248
str 1002F268
str 1002F2AC
ext 026066C4 void* SearchHD_dRes_control_c_getRes(void*, void*, u32)
ext 0273AA24 void SearchHD_JUTAssertion_showAssert(const char*, u32, const char*)
ext 025E38E0 void* SearchHD_mDoExt_J3DModel__create(void*, u32, u32)
ext 024F23F4 void* SearchHD_dBgW_dBgW(void*)
ext 0200A030 u32 SearchHD_cBgW_Set(void*, void*, u32, void*)
ext 02587CA4 u32 SearchHD_dLib_pathInfo(void*, u32)
ext 025BA0C0 u32 SearchHD_dSv_info_c_isSwitch(void*, u32, u32)
ext 025D5218 void* SearchHD_fopAcIt_Judge(void*, void*)
ext 025200D4 void* SearchHD_dComIfGp_getPlay()
ext 025D68EC f32 SearchHD_fopAcM_searchActorDistance(void*, void*)
ext 0238A0B8 void SearchHD_daObj_Search_Act_c_set_moveBG_mtx_light_A(void*)
ext 0238A154 void SearchHD_daObj_Search_Act_c_set_moveBG_mtx_light_B(void*)
ext 025B8B94 u32 SearchHD_dSv_event_c_isEventBit(void*, u32)
ext 02520540 u32 SearchHD_dComIfGp_getReverb(u32)
ext 025E1A40 void SearchHD_mDoAud_seStart(u32, void*, u32, u32)
ext 025B8B68 void SearchHD_dSv_event_c_onEventBit(void*, u32)
ext 025E1988 void SearchHD_mDoAud_bgmStart(u32)
ext 025B9E38 void SearchHD_dSv_info_c_onSwitch(void*, u32, u32)
ext 0238AAA0 u32 SearchHD_daObj_Search_Act_c__create(void*)
ext 0238B16C u32 SearchHD_daObj_Search_Act_c__delete(void*)
ext 0238A7A4 u32 SearchHD_daObj_Search_Act_c__execute(void*)
ext 0238B258 u32 SearchHD_daObj_Search_Act_c__draw(void*)
ext 0273AF40 void SearchHD_dl(void*)
ext 0273AD10 void* SearchHD_operator_new(u32)
ext 0200BD2C void SearchHD_cCcD_Stts_construct(void*)
ext 02515DA0 void SearchHD_dCcD_GStts_dCcD_GStts(void*)
ext 02515FB8 void SearchHD_dCcD_GObjInf_dCcD_GObjInf(void*)
ext 02018150 void SearchHD_cM3dGCps_construct(void*)
ext 02008FEC void SearchHD_cBgS_LinChk_ct(void*)
ext 025D4ED0 void SearchHD_fopAc_ac_c_fopAc_ac_c(void*)
ext 028EFFD0 void SearchHD_construct_array(void*, u32, u32, void*)
ext 025A5B18 void SearchHD_dPa_smokeEcallBack_dPa_smokeEcallBack(void*, u32)
ext 028F026C void SearchHD_register_global_object(void*)
ext 028ED6F8 void SearchHD_initializeHeaderByte(void*)
ext 028EAB2C void SearchHD_initializeHeaderFlag(void*)
ext 028E93CC void SearchHD_PSMTXTrans(void*, f32, f32, f32)
ext 025F1C28 void SearchHD_mDoMtx_YrotM(void*, u32)
ext 028E90D4 void SearchHD_PSMTXCopy(void*, void*)
ext 024F43DC void SearchHD_dBgW_Move(void*)
ext 025F19F8 void SearchHD_mDoMtx_XYZrotM(void*, u32, u32, u32)
ext 025F24E0 void SearchHD_mDoMtx_stack_c_transM(f32, f32, f32)
ext 024F1AFC void SearchHD_dBgS_LinChk_Set(void*, void*, void*, const void*)
ext 02008860 u32 SearchHD_cBgS_LineCross(void*, void*)
ext 020084C8 void* SearchHD_cBgS_GetTriPla(void*, u32, u32)
ext 0201ADE0 void SearchHD_cXyz___mi(void*, void*, void*)
ext 0201B12C void SearchHD_cXyz_normZP(void*, void*)
ext 028E8F44 f32 SearchHD_PSVECDotProduct(void*, void*)
ext 02008B4C void SearchHD_cBgS_LinChk_destruct(void*, u32)
ext 028F0164 void SearchHD_destroy_arr(void*, u32, u32, void*, u32, u32)
ext 025D50BC void SearchHD_fopAc_ac_c_fopAc_ac_c(void*, u32)
ext 0238918C void SearchHD_daObj_Search_Act_c__nodeControl(void*, void*, void*)
ext 0255A374 void SearchHD_dKy_plight_cut(void*)
ext 020087EC void SearchHD_cBgS_Release(void*, void*)
ext 025204C8 void SearchHD_dComIfG_resDelete(void*, const char*)
ext 025E1B24 u32 SearchHD_mDoAud_bgmCheck(u32)
ext 025E1AE0 void SearchHD_mDoAud_bgmStop(u32, u32)
ext 0238B838 u32 SearchHD_daObj_Search_Act_c_player_check(void*)
ext 0200F378 u32 SearchHD_cLib_addCalcAngleS(void*, u32, u32, u32, u32)
ext 027F7878 void* SearchHD_J3DJoint_getJointData(void*)
ext 025F1BF4 void SearchHD_mDoMtx_XrotM(void*, u32)
ext 0211D2F8 u32 SearchHD_cLib_calcTimer_i(void*)
ext 025A5F88 void SearchHD_dPa_smokeEcallBack_end(void*)
ext 027F4D5C void SearchHD_J3DModel_calc(void*)
ext 028E8F64 void SearchHD_PSMTXMultVec(void*, void*, void*)
ext 02389A24 void SearchHD_daObj_Search_Act_c_set_mtx_light_A(void*)
ext 02389DC4 void SearchHD_daObj_Search_Act_c_set_mtx_light_B(void*)
ext 0200F564 void SearchHD_cLib_chaseS(void*, u32, u32)
ext 025A847C void SearchHD_dPa_control_c_set(void*, u32, u32, void*, void*, u32, u32, void*, u32, u32, u32, u32)
ext 025D674C void SearchHD_fopAcM_setCullSizeBox(void*, f32, f32, f32, f32, f32, f32)
ext 024EEA6C void SearchHD_dBgS_Regist(void*, void*, void*)
ext 02515F14 void SearchHD_dCcD_Stts_Init(void*, u32, u32, void*)
ext 025164C0 void SearchHD_dCcD_Cps_Set(void*, void*)
ext 02018808 void SearchHD_cM3dGCps_set(void*, void*, void*)
ext 027F3F94 void* SearchHD_J3DModelData_getJointTree(void*)
ext 025AAF88 void* SearchHD_dPath_GetRoomPath(u32, u32)
ext 02587D24 void SearchHD_dLib_pathMove(void*, void*, void*, u32, u32, f32)
ext 020195B0 u32 SearchHD_cM_atan2s(f32, f32)
ext 028F4384 f32 SearchHD_sqrtf(f32)
ext 028E8DE8 f32 SearchHD_PSVECSquareDistance(void*, void*)
ext 0201AE48 void SearchHD_cXyz___ml(void*, void*, f32)
ext 0201AD78 void SearchHD_cXyz___pl(void*, void*, void*)
ext 025F23EC void SearchHD_mDoMtx_stack_c_push()
ext 025F2518 void SearchHD_mDoMtx_stack_c_scaleM(f32, f32, f32)
ext 025F2468 void SearchHD_mDoMtx_stack_c_pop()
ext 0200F428 void SearchHD_cLib_addCalcAngleS2(void*, u32, u32, u32)
ext 0283801C void SearchHD_J3DUClipper_calcViewFrustum(void*)
ext 025D6B70 u32 SearchHD_fopAcM_checkCullingBox(void*, f32, f32, f32, f32, f32, f32)
ext 02555D0C void* SearchHD_dScnKy_env_light_c_setLightTevColorType()
ext 025626A4 void SearchHD_dScnKy_env_light_c_settingTevStruct(void*, u32, void*, void*)
ext 02562F5C void SearchHD_dKy_setLightTevColorType_MAJI(void*, void*, void*)
ext 025E2E5C void SearchHD_mDoExt_modelEntryDL(void*)
ext 0201B084 void SearchHD_cXyz_norm(void*, void*)
ext 028E8DD0 f32 SearchHD_PSVECSquareMag(void*)
ext 028E8E64 void SearchHD_PSVECScale(void*, void*, f32)
ext 027386D4 void SearchHD_dDlst_2D_LightSpot_draw(void*, void*, void*, void*, u32, void*, f32, f32, f32, f32)
ext 025E2DE0 void SearchHD_mDoExt_modelUpdateDL(void*, u32)
ext 025D77DC void SearchHD_fopAcM_orderOtherEvent2(void*, const char*, u32, u32)
ext 02542D88 u32 SearchHD_dEvent_manager_c_getMyStaffId(void*, const char*, u32, u32)
ext 0254457C u32 SearchHD_dEvent_manager_c_endCheckOld(void*, const char*)
ext 02544830 void* SearchHD_dEvent_manager_c_getMyNowCutName(void*, u32)
ext 02543280 void SearchHD_dEvent_manager_c_cutEnd(void*, u32)
ext 02543F10 u32 SearchHD_dEvent_manager_c_getEventIdx(void*, const char*, u32)
ext 0252012C void SearchHD_dComIfGp_setNextStage(const char*, u32, u32, u32, u32, u32, u32, f32)
ext 0200FAAC u32 SearchHD_cLib_distanceAngleS(u32, u32)
ext 0200E240 void SearchHD_cCcS_Set(void*, void*)
ext 02516464 u32 SearchHD_dCcD_GObjInf_ChkCoHit(void*)
ext 02515BBC void* SearchHD_dCcD_GAtTgCoCommonBase_GetAc(void*)
ext 025D5578 u32 SearchHD_fopAcM_SearchByName(u32, void*)
ext 02588230 u32 SearchHD_dLib_checkActorInFan(void*, void*, u32, u32, f32, f32)
ext 025880B4 u32 SearchHD_dLib_checkActorInCircle(void*, void*, f32, f32)
ext 020198D8 f32 SearchHD_cM_rndF(f32)
ext 0200ED84 void SearchHD_cLib_addCalc2(void*, f32, f32, f32)
ext 02587128 void SearchHD_dLib_setCirclePath(void*)
ext 02520460 u32 SearchHD_dComIfG_resLoad(void*, const char*)
ext 025D63E8 u32 SearchHD_fopAcM_entrySolidHeap(void*, void*, u32)
*/
// clang-format on
