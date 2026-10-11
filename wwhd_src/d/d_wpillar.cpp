/* Water pillars and bomb water effects. HD material colors, animation bindings,
 * and constructor/destructor work retain the verified call and memory order. */
#include "d/d_wpillar.h"
#include "gc_api/SSystem/c_phase.h"

using WordHD = be<u32>;
using SignedByteHD = be<s8>;
static f32 colorDivisorHD;
static f32 zeroValueHD;
static f32 playRateHD;
static f32 groundMissingHD;
using MinHeightsHD = be<f32>[4];
static MinHeightsHD min_y;
static Mtx34 currentMtxHD;
static Mtx34 stackMtxHD;
using ModelPointerHD = gptr<dWpillar_modelHD>;
static ModelPointerHD currentModelHD;
static WordHD pillarVtableHD;
static WordHD bckResourceVtableHD;
static WordHD bckVtableHD;
static WordHD bckColorVtableHD;
static dWpillar_lightParamHD lightDefaultsHD;
static dWpillar_runtimeStateHD runtimeState;
using RuntimeObjectsHD = dWpillar_runtimeObjectHD[3];
static RuntimeObjectsHD runtimeObjects;
static dWpillar_runtimeConstantsHD runtimeConstants;

static WordHD stringVtableHD;
using StringAddressHD = char[1];
static StringAddressHD heapNameHD;
static StringAddressHD jointAssertFileHD;
static StringAddressHD jointAssertMessageHD;
static StringAddressHD createAssertFileHD;
static StringAddressHD createAssertMessageHD;
using ResourcePointerHD = gptr<void>;
static ResourcePointerHD resourceManagerHD;
static WordHD singleRippleCallbackHD;
static SignedByteHD stayNoHD;
static WordHD objCheckVtableHD;
static WordHD objGroundVtableHD;
static WordHD objectPassVtableHD;
static WordHD groupPassVtableHD;
static WordHD baseGroundVtableHD;
static WordHD baseObjectPassVtableHD;
static WordHD baseGroupPassVtableHD;

/* 025D22F8 */
s32 dWpillar_c::jointCallBack(s32 i_jointNo) {
    auto* model = mpModel.get();
    auto* mtxBuffer = model->mpMtxBuffer.get();
    auto* matrices = mtxBuffer->mpAnmMtx.get();
    u16 flags = mtxBuffer->mFlags;
    // HD: joint indices use wrapping 32-bit guest arithmetic, including assertion inputs.
    auto* m = gabi::at<Mtx34>(gabi::ea(matrices) + u32(i_jointNo) * sizeof(Mtx34));
    mtxBuffer->mFlags = flags | 16;
    if (u32(i_jointNo) - 1 >= 3)
        dWpillar_jointAssertHD(jointAssertFileHD, 86, jointAssertMessageHD);
    f32 y = mPos.y;
    f32 scale = mScale.x;
    f32 low = gabi::load<f32>(gabi::ea(min_y) + u32(i_jointNo) * sizeof(f32));
    f32 var_f2 = y != y ? y : gabi::fmadds(low, scale, y);
    f32 old = m->m[1][3];
    f32 delta = f32(f64(old) - f64(var_f2));
    f32 height = mHeight;
    f32 result = gabi::fmadds(delta, height, var_f2);
    // HD: retain GHS's additive operand when both operands are NaNs.
    if (var_f2 != var_f2)
        result = var_f2;
    m->m[1][3] = result;
    MTXCopy(m, &currentMtxHD);
    return 1;
}

/* 025D23BC */
static s32 dWpillar_jointCallBack(void* i_node, s32 param_1) {
    if (param_1 == 0) {
        auto* joint = dWpillar_nodeToJointHD(i_node);
        auto* model = currentModelHD.get();
        u16 jntNo = joint->mJntNo;
        auto* i_this = model->mUserArea.get();
        i_this->jointCallBack(jntNo);
    }
    return 1;
}

// HD: convert byte color channels to the normalized material color representation.
/* 025D23FC */
static void dWpillar_colorToFloatHD(dWpillar_floatColorHD* out, GXColor* in) {
    u8 red = in->r;
    u8 green = in->g;
    f32 divisor = colorDivisorHD;
    u8 blue = in->b;
    f32 x = f32(red) / divisor;
    u8 alpha = in->a;
    f32 y = f32(green) / divisor;
    f32 z = f32(blue) / divisor;
    f32 w = f32(alpha) / divisor;
    out->value[0] = x;
    out->value[1] = y;
    out->value[2] = z;
    out->value[3] = w;
}

/* inlined into dWpillar_c::draw */
static void dWpillar_normalizeS10HD(dWpillar_floatColorHD* out, dWpillar_Color8HD* color,
                                    f32 divisor) {
    for (u32 i = 0; i < 4; ++i)
        out->value[i] = f32(color->value[i]) / divisor;
}

/* inlined into dWpillar_c::draw */
static void* dWpillar_tevCallHD(dWpillar_materialHD* material, dWpillar_tevMethodHD method,
                                u32 index, void* color = nullptr, bool pointer = false) {
    auto* tev = static_cast<dWpillar_tevBlockHD*>(material->mTevBlock.get());
    auto* vtable = tev->mVtable.get();
    u32 target = vtable->slots[method];
    if (pointer)
        return gabi::call_ptr<void*>(target, tev, index, color);
    return gabi::call_ptr<void*>(target, tev, index);
}

/* 025D24B0 */
s32 dWpillar_c::draw() {
    auto* env = dWpillar_getEnvLightHD();
    dWpillar_settingTevStructHD(env, TEV_TYPE_BG1, &mPos, &mTevstr);
    env = dWpillar_getEnvLightHD();
    auto* model = mpModel.get();
    dWpillar_setLightTevColorTypeHD(env, model, &mTevstr);
    model = mpModel.get();
    f32 frame = mBck.mFrameCtrl.mFrame;
    auto* modelData = model->mModelData.get();
    dWpillar_bckEntryHD(&mBck, modelData, frame);
    bool bomb = s32(mParam) == 1;
    f32 divisor = colorDivisorHD;
    dWpillar_Color8HD color;
    dWpillar_Color8HD kcolor;
    dWpillar_floatColorHD normalized;
    dWpillar_floatColorHD transformed;
    dWpillar_floatColorHD normalizedKey;
    dWpillar_floatColorHD transformedKey;
    // HD: normalize and transform TEV colors through the material's floating-point slots.
    if (!bomb) {
        auto* channels = reinterpret_cast<be<s16>*>(&mTevstr.mColorC0);
        for (u32 i = 0; i < 4; ++i)
            color.value[i] = channels[i];
        kcolor.mColorWord = *reinterpret_cast<be<u32>*>(&mTevstr.mColorK0);
        model = mpModel.get();
        frame = mBck.mFrameCtrl.mFrame;
        modelData = model->mModelData.get();
        dWpillar_btkEntryHD(&mBtk, modelData, frame);
    }
    model = mpModel.get();
    modelData = model->mModelData.get();
    auto* table = dWpillar_getMaterialTableHD(modelData);
    u32 count = table->mMaterialNum;
    for (u32 remaining = count; remaining != 0; --remaining) {
        u32 matNo = (remaining - 1) & 0xFFFF;
        model = mpModel.get();
        modelData = model->mModelData.get();
        u32 bound = modelData->mMaterialCount;
        auto* material = modelData->mpMaterial.get();
        if (matNo < bound)
            material += matNo;
        if (bomb) {
            auto* channels = reinterpret_cast<be<s16>*>(&mTevstr.mColorC0);
            for (u32 i = 0; i < 4; ++i)
                color.value[i] = channels[i];
            auto* tevColor = static_cast<dWpillar_Color8HD*>(
                dWpillar_tevCallHD(material, WPILLAR_GET_TEV_COLOR_HD, 2));
            color.value[3] = tevColor->value[3];
            kcolor.mColorWord = *reinterpret_cast<be<u32>*>(&mTevstr.mColorK0);
            tevColor = static_cast<dWpillar_Color8HD*>(
                dWpillar_tevCallHD(material, WPILLAR_GET_TEV_COLOR_HD, 1));
            reinterpret_cast<GXColor*>(&kcolor)->a = tevColor->value[3];
        }
        env = dWpillar_getEnvLightHD();
        f32 factor = env->mTevColorScaleHD;
        dWpillar_tevCallHD(material, WPILLAR_SET_TEV_COLOR_HD, bomb ? 2 : 0, &color, true);
        dWpillar_normalizeS10HD(&normalized, &color, divisor);
        // HD: GHS aliases the upper half of the bomb color with normalized red scratch.
        if (bomb)
            kcolor.mRedScratch = normalized.value[0];
        dWpillar_transformColorHD(&transformed, &normalized, factor);
        u32 flags = material->mHDColorFlags;
        u32 slot = bomb ? 6 : 4;
        material->mHDColorFlags = flags | (1u << slot);
        // HD: the slot request can return a different pointer; retain that result.
        auto* out = dWpillar_getFloatColorHD(&material->mHDColorFlags, slot);
        f32 alpha = f32(color.value[3]) / divisor;
        f32 x = transformed.value[0];
        f32 y = transformed.value[1];
        f32 z = transformed.value[2];
        out->value[1] = y;
        out->value[2] = z;
        out->value[0] = x;
        out->value[3] = alpha;
        env = dWpillar_getEnvLightHD();
        factor = env->mTevKColorScaleHD;
        if (bomb) {
            dWpillar_tevCallHD(material, WPILLAR_SET_TEV_COLOR_HD, 1, &kcolor, true);
            dWpillar_normalizeS10HD(&normalizedKey, &kcolor, divisor);
        } else {
            dWpillar_tevCallHD(material, WPILLAR_SET_TEV_KCOLOR_HD, 0, &kcolor, true);
            dWpillar_colorToFloatHD(&normalizedKey, reinterpret_cast<GXColor*>(&kcolor));
        }
        dWpillar_transformColorHD(&transformedKey, &normalizedKey, factor);
        flags = material->mHDColorFlags;
        slot = bomb ? 5 : 7;
        material->mHDColorFlags = flags | (1u << slot);
        out = dWpillar_getFloatColorHD(&material->mHDColorFlags, slot);
        alpha = bomb ? f32(kcolor.value[3]) / divisor
                     : f32(reinterpret_cast<GXColor*>(&kcolor)->a) / divisor;
        x = transformedKey.value[0];
        y = transformedKey.value[1];
        z = transformedKey.value[2];
        out->value[1] = y;
        out->value[2] = z;
        out->value[0] = x;
        out->value[3] = alpha;
    }
    if (bomb) {
        // HD: both Link animation bindings include a floating frame evaluator.
        for (u32 pass = 0; pass < 2; ++pass) {
            auto* game = dWpillar_getGameInfoHD();
            auto* player = game->mpPlayer.get();
            frame = mBck.mFrameCtrl.mFrame;
            auto* anim = pass == 0 ? player->mpGwp00BrkData.get() : player->mpGwp00BtkData.get();
            auto* savedFrame = pass == 0 ? &player->mBombWaterPillarBrkFrameHD
                                         : &player->mBombWaterPillarBtkFrameHD;
            *savedFrame = frame;
            anim->mFrame = frame;
            auto* binding = pass == 0 ? &player->mpBombWaterPillarBrkBlendHD
                                      : &player->mpBombWaterPillarBtkBlendHD;
            auto* ctrl = binding->get();
            u32 target = ctrl->mEvaluator;
            f32 start = ctrl->mStart;
            f32 end = ctrl->mEnd;
            auto* data = ctrl->mpEvaluatorData.get();
            f32 result = gabi::call_ptr<f32>(target, data, frame, start, end);
            ctrl->mFrame = result;
            dWpillar_updateFrameBindingHD(binding);
        }
    }
    model = mpModel.get();
    dWpillar_modelUpdateDLHD(model, 0);
    return 1;
}
/* 025D2BA0 */
static s32 dWpillar_Draw(dWpillar_c* i_this) { return i_this->draw(); }

/* 025D2BA4 */
s32 dWpillar_c::execute() {
    mBck.play();
    bool stopped = (mBck.mFrameCtrl.mState & J3DFrameCtrl::STATE_STOP_E) != 0;
    if (!stopped)
        stopped = mBck.mFrameCtrl.mRate == zeroValueHD;
    if (stopped) {
        fopKyM_Delete(this);
    } else {
        gabi::Local<be<f32>> water_y;
        if (fopAcM_getWaterY(&mPos, water_y.get()) != 0) {
            auto* model = mpModel.get();
            f32 y = *water_y;
            // HD: a null model keeps the base matrix address at zero.
            auto* matrix = model == nullptr ? gabi::at<Mtx34>(0) : &model->mBaseTransformMtx;
            matrix->m[1][3] = y;
            mPos.y = *water_y;
        }
    }
    return 1;
}
/* 025D2C3C */
static s32 dWpillar_Execute(dWpillar_c* i_this) { return i_this->execute(); }
/* 025D2C40 */
static s32 dWpillar_IsDelete(dWpillar_c* i_this) { return 1; }
/* 025D2C48 */
s32 dWpillar_c::wp_delete() {
    auto* heap = mpHeap.get();
    if (heap != nullptr)
        mDoExt_destroySolidHeap(heap);
    // HD: destroy the BCK graphics resource and the environment actor base as well.
    dWpillar_bckResourceDeleteHD(&mBck.mpAnm, 0);
    kankyo_class__dt(this, 0);
    return 1;
}
/* 025D2C9C */
static s32 dWpillar_Delete(dWpillar_c* i_this) { return i_this->wp_delete(); }
/* 025D2EB0 */
cPhs_State dWpillar_c::create() {
    if (this != nullptr)
        dWpillar_c__ct(this);
    auto* heap = mDoExt_createSolidHeapFromGameToCurrent(0, 32);
    mpHeap = heap;
    if (heap == nullptr)
        return cPhs_ERROR_e;
    f32 rate = playRateHD;
    heap->mName = heapNameHD;
    bool normal = s32(mParam) == 0;
    dWpillar_stringHD modelName;
    dWpillar_stringHD bckName;
    dWpillar_stringHD btkName;
    modelName.vtable = gabi::ea(&stringVtableHD);
    modelName.text.v = gabi::guest_addr(normal ? "Always" : "Link");
    auto* resources = resourceManagerHD.get();
    auto* modelData = static_cast<dWpillar_modelDataHD*>(dWpillar_getObjectResHD(
        resources, &modelName,
        normal ? dRes_INDEX_ALWAYS_BDL_W_PILLAR_A_e : dRes_INDEX_LINK_BDL_GWP00_e));
    if (modelData == nullptr)
        dWpillar_jointAssertHD(createAssertFileHD, normal ? 328 : 355, createAssertMessageHD);
    auto* model = dWpillar_modelCreateHD(modelData, 0x80000, 0x11000202);
    bckName.vtable = gabi::ea(&stringVtableHD);
    mpModel = model;
    resources = resourceManagerHD.get();
    bckName.text.v = gabi::guest_addr(normal ? "Always" : "Link");
    auto* bck = dWpillar_getObjectResHD(resources, &bckName,
                                        normal ? dRes_INDEX_ALWAYS_BCK_W_PILLAR_A_e
                                               : dRes_INDEX_LINK_BCK_GWP00_e);
    s32 is_anm_init =
        dWpillar_bckInitHD(&mBck, modelData, bck, 1, J3DFrameCtrl::EMode_NONE, rate, 0, -1, 0);
    dWpillar_Color8HD color1;
    dWpillar_Color8HD color2;
    dWpillar_floatColorHD normalized1;
    dWpillar_floatColorHD normalized2;
    dWpillar_floatColorHD transformed1;
    dWpillar_floatColorHD transformed2;
    if (normal) {
        btkName.vtable = gabi::ea(&stringVtableHD);
        resources = resourceManagerHD.get();
        btkName.text.v = gabi::guest_addr("Always");
        auto* btk =
            dWpillar_getObjectResHD(resources, &btkName, dRes_INDEX_ALWAYS_BTK_W_PILLAR_A_e);
        is_anm_init &= dWpillar_btkInitHD(&mBtk, modelData, btk, 0, J3DFrameCtrl::EMode_LOOP, rate,
                                          0, -1, 0, 0);
    } else {
        // HD: initialize the bomb material's floating color slots from its two TEV colors.
        model = mpModel.get();
        if (model != nullptr) {
            u32 count = model->mMaterialNum;
            if (count != 0) {
                f32 divisor = colorDivisorHD;
                // HD: color records use a 60-byte stride and wrapping guest addresses.
                u32 offset = 0;
                for (u32 i = 0; i < count; ++i, offset += 60) {
                    auto* data = model->mModelData.get();
                    u32 bound = data->mMaterialCount;
                    auto* colors = model->mpMatColor.get();
                    auto* material = data->mpMaterial.get();
                    if (u16(i) < bound)
                        material += u16(i);
                    dWpillar_getMatColorHD(&color1, gabi::at<void>(gabi::ea(colors) + offset), 1);
                    dWpillar_tevCallHD(material, WPILLAR_SET_TEV_COLOR_HD, 1, &color1, true);
                    dWpillar_normalizeS10HD(&normalized1, &color1, divisor);
                    dWpillar_transformColorHD(&transformed1, &normalized1, rate);
                    u32 flags = material->mHDColorFlags;
                    material->mHDColorFlags = flags | 32;
                    auto* out = dWpillar_getFloatColorHD(&material->mHDColorFlags, 5);
                    f32 alpha = f32(color1.value[3]) / divisor;
                    f32 x = transformed1.value[0];
                    f32 y = transformed1.value[1];
                    f32 z = transformed1.value[2];
                    out->value[1] = y;
                    out->value[2] = z;
                    out->value[0] = x;
                    out->value[3] = alpha;
                    model = mpModel.get();
                    colors = model->mpMatColor.get();
                    dWpillar_getMatColorHD(&color2, gabi::at<void>(gabi::ea(colors) + offset), 2);
                    dWpillar_tevCallHD(material, WPILLAR_SET_TEV_COLOR_HD, 2, &color2, true);
                    dWpillar_normalizeS10HD(&normalized2, &color2, divisor);
                    dWpillar_transformColorHD(&transformed2, &normalized2, rate);
                    flags = material->mHDColorFlags;
                    material->mHDColorFlags = flags | 64;
                    out = dWpillar_getFloatColorHD(&material->mHDColorFlags, 6);
                    alpha = f32(color2.value[3]) / divisor;
                    x = transformed2.value[0];
                    y = transformed2.value[1];
                    z = transformed2.value[2];
                    out->value[1] = y;
                    out->value[2] = z;
                    out->value[0] = x;
                    out->value[3] = alpha;
                    model = mpModel.get();
                    count = model->mMaterialNum;
                }
            }
        }
    }
    mDoExt_restoreCurrentHeap();
    auto* storedHeap = static_cast<dWpillar_heapHD*>(mpHeap.get());
    mDoExt_adjustSolidHeap(storedHeap);
    model = mpModel.get();
    if (model == nullptr || is_anm_init == 0)
        return cPhs_ERROR_e;
    f32 height = mScale.y;
    f32 scale = mScale.x;
    mHeight = height;
    mScale.y = scale;
    model = mpModel.get();
    f32 scaleZ = mScale.z;
    model->mBaseScale.x = scale;
    model->mBaseScale.y = scale;
    model->mBaseScale.z = scaleZ;
    f32 posY = mPos.y;
    f32 posZ = mPos.z;
    f32 posX = mPos.x;
    MTXTrans(&stackMtxHD, posX, posY, posZ);
    // HD: retain the twelve float loads before any base transform stores.
    f32 matrix[12];
    for (u32 i = 0; i < 12; ++i)
        matrix[i] = stackMtxHD.m[i / 4][i % 4];
    model = mpModel.get();
    for (u32 i = 0; i < 12; ++i)
        model->mBaseTransformMtx.m[i / 4][i % 4] = matrix[i];
    normal = s32(mParam) == 0;
    if (normal) {
        auto* joints = dWpillar_getJointTableHD(modelData);
        for (u32 i = 1; i < joints->mJointNum;) {
            u32 count = modelData->mJointCount;
            auto* joint = modelData->mpJoint.get();
            if (i < count)
                joint += i;
            i = u16(i + 1);
            joint->mCallBack = gabi::guest_fn((const void*)dWpillar_jointCallBack);
            joints = dWpillar_getJointTableHD(modelData);
        }
        model = mpModel.get();
        model->mUserArea = this;
        for (u32 id = WPILLAR_HAMON01_HD; id <= WPILLAR_SHIBUKI_HD; ++id) {
            auto* game = dWpillar_getGameInfoHD();
            auto* particles = game->mpParticle.get();
            dWpillar_particleSetHD(
                particles, id == WPILLAR_SHIBUKI_HD ? 0 : 5, id, &mPos, nullptr, &mScale, 255,
                id == WPILLAR_SHIBUKI_HD ? nullptr : &singleRippleCallbackHD, -1, 0, 0, 0);
        }
    } else {
        auto* game = dWpillar_getGameInfoHD();
        auto* particles = game->mpParticle.get();
        dWpillar_particleSetBombSmokeHD(particles, WPILLAR_BOMB_SMOKE_HD, &mPos, nullptr, nullptr,
                                        255);
        for (u32 pass = 0; pass < 2; ++pass) {
            game = dWpillar_getGameInfoHD();
            particles = game->mpParticle.get();
            dWpillar_particleSetHD(particles, pass == 0 ? 3 : 1,
                                   pass == 0 ? WPILLAR_WATER_SMOKE_HD : WPILLAR_MIZUSHIBUKI_HD,
                                   &mPos, nullptr, nullptr, 255, nullptr, -1, 0, 0, 0);
        }
    }
    dWpillar_groundHD gndchk;
    dWpillar_groundCheckCtorHD(&gndchk);
    auto* posWords = reinterpret_cast<be<u32>*>(&mPos);
    for (u32 i = 0; i < 3; ++i)
        gndchk.mPos[i] = posWords[i];
    for (u32 i = 0; i < 6; ++i)
        gndchk.mObjectPassFlags[i] = 0;
    gndchk.mpObjectPass = &gndchk.mObjectPassVtable;
    gndchk.mpGroupPass = &gndchk.mGroupPassVtable;
    gndchk.mCheckVtable = gabi::ea(&objCheckVtableHD);
    gndchk.mGroundVtable = gabi::ea(&objGroundVtableHD);
    gndchk.mObjectPassVtable = gabi::ea(&objectPassVtableHD);
    gndchk.mObjectPassEnabled = 1;
    gndchk.mGroupPassVtable = gabi::ea(&groupPassVtableHD);
    gndchk.mGroupPassFlags = 1;
    auto* game = dWpillar_getGameInfoHD();
    f32 y = dWpillar_groundCrossHD(&game->mBgS, &gndchk);
    s32 roomNo;
    u8 polyColor;
    if (y == groundMissingHD) {
        roomNo = stayNoHD;
        polyColor = 255;
    } else {
        game = dWpillar_getGameInfoHD();
        roomNo = dWpillar_getRoomIdHD(&game->mBgS, gndchk.mPolyInfo);
        game = dWpillar_getGameInfoHD();
        polyColor = dWpillar_getPolyColorHD(&game->mBgS, gndchk.mPolyInfo);
    }
    dWpillar_tevstrInitHD(&mTevstr, s32(s8(roomNo)), polyColor);
    gndchk.mGroundVtable = gabi::ea(&baseGroundVtableHD);
    gndchk.mObjectPassVtable = gabi::ea(&baseObjectPassVtableHD);
    gndchk.mGroupPassVtable = gabi::ea(&baseGroupPassVtableHD);
    dWpillar_groundCheckDtorHD(&gndchk, 0);
    return cPhs_COMPLEATE_e;
}
/* 025D38B8 */
static cPhs_State dWpillar_Create(kankyo_class* i_this) {
    return static_cast<dWpillar_c*>(i_this)->create();
}

// HD: compiler-generated constructor, including the expanded light defaults.
/* 025D2CA0 */
dWpillar_c* dWpillar_c__ct(dWpillar_c* i_this) {
    if (i_this == nullptr)
        i_this = dWpillar_allocateHD(sizeof(dWpillar_c));
    if (i_this == nullptr)
        return i_this;
    kankyo_class__ct(i_this);
    i_this->base.__vtbl = gabi::ea(&pillarVtableHD);
    auto* bckStorage = reinterpret_cast<dWpillar_bckStorageHD*>(&i_this->mBck);
    J3DFrameCtrl__ct(&i_this->mBck.mFrameCtrl, 0);
    bckStorage->mResourceVtable = gabi::ea(&bckResourceVtableHD);
    dWpillar_shaderPairInitHD(bckStorage->mShaderPair);
    bckStorage->mResourceD = 0;
    bckStorage->mResourceFlags = 0;
    bckStorage->mResourceB = 0;
    bckStorage->mResourceVtable = gabi::ea(&bckVtableHD);
    bckStorage->mResourceA = 0;
    bckStorage->mResourceC = 0;
    bckStorage->mColorVtable = gabi::ea(&bckColorVtableHD);
    mDoExt_btkAnm__ct(&i_this->mBtk);
    f32 positionX = ((f32)lightDefaultsHD.mPosition.x);
    i_this->mTevstr.mLight.mPosition.x = positionX;
    f32 positionY = ((f32)lightDefaultsHD.mPosition.y);
    i_this->mTevstr.mLight.mPosition.y = positionY;
    f32 positionZ = ((f32)lightDefaultsHD.mPosition.z);
    i_this->mTevstr.mLight.mPosition.z = positionZ;
    f32 directionX = ((f32)lightDefaultsHD.mDirection.x);
    i_this->mTevstr.mLight.mDirection.x = directionX;
    f32 directionY = ((f32)lightDefaultsHD.mDirection.y);
    i_this->mTevstr.mLight.mDirection.y = directionY;
    f32 directionZ = ((f32)lightDefaultsHD.mDirection.z);
    i_this->mTevstr.mLight.mDirection.z = directionZ;
    u8 red = ((u8)lightDefaultsHD.mColor.r);
    i_this->mTevstr.mLight.mColor.r = red;
    u8 green = ((u8)lightDefaultsHD.mColor.g);
    i_this->mTevstr.mLight.mColor.g = green;
    u8 blue = ((u8)lightDefaultsHD.mColor.b);
    i_this->mTevstr.mLight.mColor.b = blue;
    u8 alpha = ((u8)lightDefaultsHD.mColor.a);
    i_this->mTevstr.mLight.mColor.a = alpha;
    u16 flags0 = ((u16)lightDefaultsHD.mFlags[0]);
    i_this->mTevstr.mLight.mFlags[0] = flags0;
    u16 flags1 = ((u16)lightDefaultsHD.mFlags[1]);
    i_this->mTevstr.mLight.mFlags[1] = flags1;
    u16 flags2 = ((u16)lightDefaultsHD.mFlags[2]);
    i_this->mTevstr.mLight.mFlags[2] = flags2;
    u16 flags3 = ((u16)lightDefaultsHD.mFlags[3]);
    i_this->mTevstr.mLight.mFlags[3] = flags3;
    f32 parameter0 = ((f32)lightDefaultsHD.mParameters[0]);
    i_this->mTevstr.mLight.mParameters[0] = parameter0;
    f32 parameter1 = ((f32)lightDefaultsHD.mParameters[1]);
    i_this->mTevstr.mLight.mParameters[1] = parameter1;
    f32 parameter2 = ((f32)lightDefaultsHD.mParameters[2]);
    i_this->mTevstr.mLight.mParameters[2] = parameter2;
    f32 parameter3 = ((f32)lightDefaultsHD.mParameters[3]);
    i_this->mTevstr.mLight.mParameters[3] = parameter3;
    f32 parameter4 = ((f32)lightDefaultsHD.mParameters[4]);
    i_this->mTevstr.mLight.mParameters[4] = parameter4;
    f32 parameter5 = ((f32)lightDefaultsHD.mParameters[5]);
    i_this->mTevstr.mLight.mParameters[5] = parameter5;
    f32 parameter6 = ((f32)lightDefaultsHD.mParameters[6]);
    i_this->mTevstr.mLight.mParameters[6] = parameter6;
    f32 parameter7 = ((f32)lightDefaultsHD.mParameters[7]);
    i_this->mTevstr.mLightCopy.mDirection.z = directionZ;
    i_this->mTevstr.mLightCopy.mParameters[7] = parameter7;
    i_this->mTevstr.mLightCopy.mParameters[3] = parameter3;
    i_this->mTevstr.mLight.mParameters[7] = parameter7;
    i_this->mTevstr.mLightCopy2.mColor.r = red;
    i_this->mTevstr.mLightCopy2.mPosition.x = positionX;
    i_this->mTevstr.mLightCopy2.mFlags[3] = flags3;
    i_this->mTevstr.mLightCopy.mDirection.x = directionX;
    i_this->mTevstr.mLightCopy.mFlags[1] = flags1;
    i_this->mTevstr.mLightCopy.mParameters[6] = parameter6;
    i_this->mTevstr.mLightCopy.mDirection.y = directionY;
    i_this->mTevstr.mLightCopy2.mParameters[2] = parameter2;
    i_this->mTevstr.mLightCopy.mColor.a = alpha;
    i_this->mTevstr.mLightCopy.mPosition.x = positionX;
    i_this->mTevstr.mLightCopy2.mFlags[0] = flags0;
    i_this->mTevstr.mLightCopy2.mDirection.x = directionX;
    i_this->mTevstr.mLightCopy.mPosition.y = positionY;
    i_this->mTevstr.mLightCopy.mColor.g = green;
    i_this->mTevstr.mLightCopy2.mColor.b = blue;
    i_this->mTevstr.mLightCopy.mParameters[2] = parameter2;
    i_this->mTevstr.mLightCopy2.mDirection.y = directionY;
    i_this->mTevstr.mLightCopy.mPosition.z = positionZ;
    i_this->mTevstr.mLightCopy2.mColor.a = alpha;
    i_this->mTevstr.mLightCopy2.mPosition.y = positionY;
    i_this->mTevstr.mLightCopy2.mParameters[1] = parameter1;
    i_this->mTevstr.mLightCopy2.mFlags[2] = flags2;
    i_this->mTevstr.mLightCopy2.mFlags[1] = flags1;
    i_this->mTevstr.mLightCopy2.mParameters[3] = parameter3;
    i_this->mTevstr.mLightCopy.mFlags[0] = flags0;
    i_this->mTevstr.mLightCopy.mParameters[5] = parameter5;
    i_this->mTevstr.mLightCopy.mFlags[3] = flags3;
    i_this->mTevstr.mLightCopy2.mPosition.z = positionZ;
    i_this->mTevstr.mLightCopy.mParameters[1] = parameter1;
    i_this->mTevstr.mLightCopy2.mColor.g = green;
    i_this->mTevstr.mLightCopy.mFlags[2] = flags2;
    i_this->mTevstr.mLightCopy.mColor.r = red;
    i_this->mTevstr.mLightCopy2.mDirection.z = directionZ;
    i_this->mTevstr.mLightCopy.mParameters[4] = parameter4;
    i_this->mTevstr.mLightCopy.mColor.b = blue;
    i_this->mTevstr.mLightCopy2.mParameters[4] = parameter4;
    i_this->mTevstr.mLightCopy2.mParameters[0] = parameter0;
    i_this->mTevstr.mLightCopy.mParameters[0] = parameter0;
    i_this->mTevstr.mLightCopy2.mParameters[5] = parameter5;
    i_this->mTevstr.mLightCopy2.mParameters[6] = parameter6;
    i_this->mTevstr.mLightCopy2.mParameters[7] = parameter7;
    return i_this;
}

// HD: compiler-generated and template code of this translation unit.
/* 025D38BC */
static void __sinit_d_wpillar_cpp() {
    runtimeState.zero[2] = 0;
    runtimeState.zero[0] = 0;
    runtimeState.zero[3] = 0;
    runtimeState.zero[1] = 0;
    dWpillar_runtimeObjectInitHD(&runtimeObjects[0]);
    f32 lo = runtimeConstants.lower;
    f32 hi = runtimeConstants.upper;
    runtimeState.lower = lo;
    runtimeState.upper = hi;
    dWpillar_runtimeFlagAInitHD(&runtimeState.flagA);
    dWpillar_runtimeObjectInitHD(&runtimeObjects[1]);
    dWpillar_runtimeFlagBInitHD(&runtimeState.flagB);
    dWpillar_runtimeObjectInitHD(&runtimeObjects[2]);
}

// clang-format off
/* WWHD glue
data colorDivisorHD 10056C38
data zeroValueHD 10056C50
data playRateHD 10056C54
data groundMissingHD 10056C58
data min_y 10056BF8
data currentMtxHD 104B4868
data stackMtxHD 1048D0CC
data currentModelHD 104B462C
data pillarVtableHD 10056B34
data bckResourceVtableHD 1016E54C
data bckVtableHD 10056B44
data bckColorVtableHD 1016D820
data lightDefaultsHD 1016E414
data runtimeState 10487374
data runtimeObjects 101F2F80
data runtimeConstants 10056C98
data stringVtableHD 10056B1C
data heapNameHD 10056C5C
data jointAssertFileHD 10056C08
data jointAssertMessageHD 10056C18
data createAssertFileHD 10056C74
data createAssertMessageHD 10056C84
data resourceManagerHD 101F4F28
data singleRippleCallbackHD 1047B2E4
data stayNoHD 1047E6C8
data objCheckVtableHD 10056BBC
data objGroundVtableHD 10056BCC
data objectPassVtableHD 10056BEC
data groupPassVtableHD 10056BDC
data baseGroundVtableHD 10056B8C
data baseObjectPassVtableHD 10056BAC
data baseGroupPassVtableHD 10056B6C
str 10056C64
str 10056C6C
ext 0273AA24 void dWpillar_jointAssertHD(const char*,u32,const char*)
ext 028E90D4 void MTXCopy(const Mtx34*,Mtx34*)
ext 028E93CC void MTXTrans(Mtx34*,f32,f32,f32)
ext 027F7878 dWpillar_jointHD* dWpillar_nodeToJointHD(void*)
ext 02555D0C dWpillar_envLightHD* dWpillar_getEnvLightHD()
ext 025626A4 void dWpillar_settingTevStructHD(dWpillar_envLightHD*,u32,cXyz*,dWpillar_tevstrHD*)
ext 02562F5C void dWpillar_setLightTevColorTypeHD(dWpillar_envLightHD*,dWpillar_modelHD*,dWpillar_tevstrHD*)
ext 025E86B8 void dWpillar_bckEntryHD(mDoExt_bckAnm*,dWpillar_modelDataHD*,f32)
ext 025E7FC4 void dWpillar_btkEntryHD(mDoExt_btkAnm*,dWpillar_modelDataHD*,f32)
ext 027F3F8C dWpillar_materialTableHD* dWpillar_getMaterialTableHD(dWpillar_modelDataHD*)
ext 0274D458 void dWpillar_transformColorHD(dWpillar_floatColorHD*,dWpillar_floatColorHD*,f32)
ext 027F9F0C dWpillar_floatColorHD* dWpillar_getFloatColorHD(be<u32>*,u32)
ext 025200D4 dWpillar_gameInfoHD* dWpillar_getGameInfoHD()
ext 027DF40C void dWpillar_updateFrameBindingHD(gptr<dWpillar_frameBlendHD>*)
ext 025E2DE0 void dWpillar_modelUpdateDLHD(dWpillar_modelHD*,u32)
ext 025DAD48 void fopKyM_Delete(dWpillar_c*)
ext 025D9F70 s32 fopAcM_getWaterY(cXyz*,be<f32>*)
ext 025E3868 void mDoExt_destroySolidHeap(void*)
ext 027F3628 void dWpillar_bckResourceDeleteHD(void*,u32)
ext 025DD630 void kankyo_class__dt(dWpillar_c*,u32)
ext 0273AD10 dWpillar_c* dWpillar_allocateHD(u32)
ext 025DD5F0 void kankyo_class__ct(dWpillar_c*)
ext 027F2BC0 void J3DFrameCtrl__ct(J3DFrameCtrl*,u32)
ext 027DA984 void dWpillar_shaderPairInitHD(void*)
ext 025E7C6C void mDoExt_btkAnm__ct(mDoExt_btkAnm*)
ext 025E3630 dWpillar_heapHD* mDoExt_createSolidHeapFromGameToCurrent(u32,u32)
ext 026066C4 void* dWpillar_getObjectResHD(void*,dWpillar_stringHD*,u32)
ext 025E38E0 dWpillar_modelHD* dWpillar_modelCreateHD(dWpillar_modelDataHD*,u32,u32)
ext 025E8508 s32 dWpillar_bckInitHD(mDoExt_bckAnm*,dWpillar_modelDataHD*,void*,u32,u32,f32,u32,s32,u32)
ext 025E7CE0 s32 dWpillar_btkInitHD(mDoExt_btkAnm*,dWpillar_modelDataHD*,void*,u32,u32,f32,u32,s32,u32,u32)
ext 027FA160 void dWpillar_getMatColorHD(dWpillar_Color8HD*,void*,u32)
ext 025E37D8 void mDoExt_restoreCurrentHeap()
ext 025E3678 void mDoExt_adjustSolidHeap(dWpillar_heapHD*)
ext 027F3F94 dWpillar_jointTableHD* dWpillar_getJointTableHD(dWpillar_modelDataHD*)
ext 025A847C void dWpillar_particleSetHD(void*,u32,u32,cXyz*,void*,cXyz*,u32,void*,s32,u32,u32,u32)
ext 025A866C void dWpillar_particleSetBombSmokeHD(void*,u32,cXyz*,void*,void*,u32)
ext 02008E0C void dWpillar_groundCheckCtorHD(dWpillar_groundHD*)
ext 02008974 f32 dWpillar_groundCrossHD(void*,dWpillar_groundHD*)
ext 024EF130 s32 dWpillar_getRoomIdHD(void*,void*)
ext 024EEEB8 u32 dWpillar_getPolyColorHD(void*,void*)
ext 0255FFF4 void dWpillar_tevstrInitHD(dWpillar_tevstrHD*,s32,u32)
ext 02008DAC void dWpillar_groundCheckDtorHD(dWpillar_groundHD*,u32)
ext 028F026C void dWpillar_runtimeObjectInitHD(dWpillar_runtimeObjectHD*)
ext 028ED6F8 void dWpillar_runtimeFlagAInitHD(be<u8>*)
ext 028EAB2C void dWpillar_runtimeFlagBInitHD(be<u8>*)
*/
// clang-format on
