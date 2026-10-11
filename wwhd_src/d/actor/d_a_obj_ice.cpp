/* Generic ice block, GameCube names and layout ported to the verified WWHD code.
 * HD adds actor-owned effect matrices and shader/material uniforms. All marked functions
 * are verified against the recompiled original. */
#include "d/actor/d_a_obj_ice.h"
#include "bindings.h"
const gc_tu_data gc_tu = {.safeString = 0x1002B0F4};
static const char l_arcname[] = "Ikori";
static dCcD_SrcCyl l_cyl_src;
static f32 l_co_radius_table[3] = {};
static f32 l_co_height_table[3] = {};
static f32 cylinderRadius;
static f32 cylinderHeight;
static f32 widthFade;
static f32 alphaFade;
static Mtx34 mtx_adj;
static Mtx34 initialAdjustment;
static u32 adjustmentInitialized;
static cXyz zeroVector;
static cXyz upVector;
static f32 minimumPi;
static f32 maximumPi;
static u8 staticCharacter;
static u8 staticByte;
static IceHeaderData headerData;
static u32 staticRegistration0[3] = {};
static u32 staticRegistration1[3] = {};
static u32 staticRegistration2[3] = {};

void ice_normalizeColor(IceColor *, void *);
u32 objIceParameter(daObjIce_c *, u32, u32);
void tg_hitCallback(daObjIce_c *, void *, void *, void *);
namespace {
f32 cosine(s16 a) { return cM_scos(a); }
f32 sine(s16 a) { return cM_ssin(a); }
J3DModelData *resource(s32 index) {
    return static_cast<J3DModelData *>(dComIfG_getObjectRes(l_arcname, index));
}
void setAction(daObjIce_c *ice, u32 fn) {
    ice->m43C.delta = 0;
    ice->m43C.index = -1;
    ice->m43C.target = fn;
}
void particle(daObjIce_c *ice, u32 id, cXyz *position, csXyz *angle, cXyz *scale) {
    dComIfGp_getParticle()->set(0, id, position, angle, scale, 255, nullptr, -1,
                                &ice->tevStr.mColorK0, nullptr, nullptr);
}
IceMaterial *materialAt(J3DModel *model, u16 i) {
    return reinterpret_cast<IceMaterial *>(model->getModelData()->getMaterialNodePointer(i));
}
IceModel *hdModel(J3DModel *model) { return reinterpret_cast<IceModel *>(model); }
} // namespace
/* 0235D514 */
u8 daObjIce_c::chk_appear() {
    auto *ice = this;
    u32 sw = objIceParameter(ice, 8, 0);
    if (sw == 255)
        return 1;
    return dComIfGs_isSwitch(sw, ice->home.roomNo) ? 0 : 1;
}
/* 0235D664 */
void daObjIce_c::set_mtx() {
    auto *ice = this;
    mDoMtx_stack_c::transS(ice->current.pos.x, ice->current.pos.y, ice->current.pos.z);
    iceXYZrotM(mDoMtx_stack_c::get(), (s16)ice->shape_angle.x, (s16)ice->shape_angle.y,
               (s16)ice->shape_angle.z);
    mDoMtx_stack_c::scaleM(ice->m44C, ice->m450, ice->m44C);
    J3DModel_setBaseTRMtx(ice->mModel.get(), mDoMtx_stack_c::get());
    mDoMtx_stack_c::scaleM(ice->scale.x, ice->scale.y, ice->scale.z);
    PSMTXCopy(mDoMtx_stack_c::get(), &ice->m40C);
}
/* 0235D754 */
void daObjIce_c::init_mtx() {
    auto *ice = this;
    J3DModel_setBaseScale(ice->mModel.get(), &ice->scale);
    ice->set_mtx();
}
/* 0235D660 */
BOOL solidHeapCB(daObjIce_c *ice) { return ice->create_heap(); }
/* 0235D588 */
BOOL daObjIce_c::create_heap() {
    auto *ice = this;
    J3DModelData *data = resource(dRes_INDEX_IKORI_BDL_VICE_e);
    if (!data) {
        JUT_ASSERT_fail("d_a_obj_ice.cpp", 395, "0");
        return 0;
    }
    ice->mModel = mDoExt_J3DModel__create(data, 0x80000, 0x11000222);
    auto *collision = reinterpret_cast<cBgD_t *>(resource(dRes_INDEX_IKORI_DZB_VICE_e));
    ice->mBgw = iceNewBackground(collision, 1, &ice->m40C);
    return ice->mModel && ice->mBgw;
}
/* 0235D774 */
void tg_hitCallback(daObjIce_c *ice, void *collider, void *other, void *otherCollider) {
    auto *col = static_cast<dCcD_GObjInf *>(collider);
    auto *hit = static_cast<dCcD_GObjInf *>(col->GetTgHitObj());
    if (!hit)
        return;
    s16 angle = (s16) - (s16)ice->shape_angle.y;
    f32 sin = sine(angle), cos = cosine(angle);
    f32 dx = col->mGObjTg.mHitPos.x - (f32)ice->current.pos.x,
        dz = col->mGObjTg.mHitPos.z - (f32)ice->current.pos.z;
    f32 x = gabi::fmadds(dx, cos, dz * sin), z = gabi::fmsubs(dz, cos, dx * sin),
        y = col->mGObjTg.mHitPos.y - (f32)ice->current.pos.y;
    ice->m45C = 0;
    for (u32 i = 0; i < 3; i++) {
        f32 radius = l_co_radius_table[i], height = l_co_height_table[i];
        f32 rx = (radius * (f32)ice->scale.x) * (f32)ice->m44C,
            rz = (radius * (f32)ice->scale.z) * (f32)ice->m44C,
            hy = (height * (f32)ice->scale.y) * (f32)ice->m450;
        if (y < hy && ((x * x) / (rx * rx) + (z * z) / (rz * rz)) < 1.f) {
            u32 type = hit->mAtType;
            if (type == 0x200 || type == 0x20000 || type == 0x40000) {
                ice->m45C = 2;
                col->mGObjTg.mSPrm |= 2;
                return;
            }
            if (type != 0x20) {
                f32 width = (f32)ice->m44C - .0009f;
                if (width < 0.f)
                    width = 0.f;
                ice->m44C = width;
                ice->m450 = 1.f - cosine((s16)gabi::ftoi(width * 16384.f));
                if (width < .1f)
                    ice->m45C = 1;
                gabi::Local<csXyz> rot;
                csXyz_ct(rot.get(), 0, 0, 0);
                rot->x = cM_atan2s((f32)ice->current.pos.y - col->mGObjTg.mHitPos.y,
                                   col->mGObjTg.mHitPos.z - (f32)ice->current.pos.z);
                rot->y = cM_atan2s(col->mGObjTg.mHitPos.x - (f32)ice->current.pos.x,
                                   col->mGObjTg.mHitPos.z - (f32)ice->current.pos.z);
                particle(ice, 0x465, &col->mGObjTg.mHitPos, rot.get(), nullptr);
                if (ice->m45C) {
                    col->mGObjTg.mSPrm |= 2;
                    return;
                }
            }
        }
    }
}
namespace {
void updateUniform(IceMaterial *material, void *rgba) {
    gabi::Local<IceColor> normalized;
    ice_normalizeColor(normalized.get(), rgba);
    gabi::Local<IceColor> converted;
    iceConvertColor(converted.get(), normalized.get(), 1.f);
    material->mFlags |= 0x400;
    IceColor *uniform = iceGetUniform(&material->mFlags, 10);
    f32 r = converted->r, g = converted->g, b = converted->b;
    f32 alpha = (f32) static_cast<IceRgba *>(rgba)->a / 255.f;
    uniform->r = r;
    uniform->g = g;
    uniform->b = b;
    uniform->a = alpha;
}
void constructCollision(daObjIce_c *ice) {
    u32 a = gabi::ea(ice);
    fopAc_ac_c_ct(ice);
    ice->__vtbl = ICE_ACTOR_VTABLE;
    dCcD_Stts_ct(&ice->mStts);
    iceConstructGObj(&ice->mCyl);
    auto *shape = reinterpret_cast<IceShapeVtables *>(&ice->mCyl);
    shape->mShapeVtable = ICE_SHAPE_BASE_VTABLE;
    shape->mAabVtable = ICE_AAB_VTABLE;
    iceConstructCylinder(&ice->mCyl.mCyl);
    ice->mCyl.__vtbl_hitinf = ICE_HIT_VTABLE;
    ice->mCyl.mCyl.__vtbl = ICE_CYL_VTABLE;
    shape->mShapeVtable = ICE_SHAPE_CYL_VTABLE;
    // Default matrix constructors do nothing for a valid embedded object.
    if (a + 0x578 == 0)
        iceAllocateMatrix(0x30);
    if (a + 0x5A8 == 0)
        iceAllocateMatrix(0x30);
}
} // namespace
/* 0235DB5C */
s32 daObjIce_c::_create() {
    auto *ice = this;
    if (!(ice->actor_condition & 8)) {
        if (gabi::ea(ice) != 0)
            constructCollision(ice);
        ice->actor_condition = ice->actor_condition | 8;
    }
    s32 phase = 5;
    if ((u8)ice->mInitState == 0)
        ice->m448 = ice->chk_appear();
    if (!ice->m448)
        return phase;
    phase = dComIfG_resLoad(&ice->mPhase, l_arcname);
    if (phase != 4)
        return phase;
    if (!fopAcM_entrySolidHeap(ice, gabi::guest_fn((const void *)solidHeapCB), 0x1A20))
        return 5;
    if (dComIfG_Bgsp()->Regist(ice->mBgw, ice))
        return 5;
    J3DModel *model = ice->mModel;
    ice->cullMtx = gabi::ea(model->getBaseTRMtx());
    ice->m44C = 1.f;
    ice->m450 = 1.f;
    ice->init_mtx();
    f32 radiusScale = ice->scale.x;
    if (radiusScale < (f32)ice->scale.z)
        radiusScale = ice->scale.z;
    ice->mStts.Init(255, 255, ice);
    ice->mCyl.Set(&l_cyl_src);
    ice->mCyl.SetStts(&ice->mStts);
    ice->mCyl.SetC(&ice->current.pos);
    ice->mCyl.SetR(cylinderRadius * radiusScale);
    ice->mCyl.SetH(cylinderHeight * (f32)ice->scale.y);
    ice->mCyl.mObjTg.mSPrm |= 0x10;
    ice->mCyl.mGObjTg.mHitCallback = gabi::guest_fn((const void *)tg_hitCallback);
    model = ice->mModel;
    for (s32 i = 0; i < hdModel(model)->mMaterialNum; i++) {
        IceMaterial *material = materialAt(model, (u16)i);
        IceShaderEntry *materials = hdModel(model)->mShaderEntries;
        gabi::Local<IceRgba> color;
        iceReadColor(color.get(), &materials[i], 3);
        IceTevBlock *block = material->mTevBlock;
        IceTevVtable *vt = block->mVtable;
        gabi::call_ptr(vt->setKonstColor.get(), block, 3, color.get());
        updateUniform(material, color.get());
        model = ice->mModel;
    }
    ice->m454 = 255.f;
    fopAcM_setCullSizeBox(ice, -80.f * (f32)ice->scale.x, 0.f * (f32)ice->scale.y,
                          -80.f * (f32)ice->scale.z, 80.f * (f32)ice->scale.x,
                          150.f * (f32)ice->scale.y, 80.f * (f32)ice->scale.z);
    f32 x = ice->current.pos.x, y = gabi::fmadds(60.f, ice->scale.y, ice->current.pos.y),
        z = ice->current.pos.z;
    ice->eyePos.set(x, y, z);
    ice->attention_info.position.x = x;
    ice->attention_info.position.y = y;
    ice->attention_info.position.z = z;
    setAction(ice, ICE_WAIT_ACTION);
    return phase;
}
/* 0235DF70 */
BOOL daObjIce_c::_delete() {
    auto *ice = this;
    if (ice->m448) {
        dComIfG_resDelete(&ice->mPhase, l_arcname);
        if (ice->heap && ice->mBgw) {
            if (ice->mBgw->ChkUsed())
                cBgS_Release(dComIfG_Bgsp(), ice->mBgw);
            ice->mBgw = 0;
        }
    }
    return 1;
}
/* 0235E6C4 */
void daObjIce_c::wait_act_proc() {
    auto *ice = this;
    if (!ice->mCyl.ChkTgHit() || ice->mStts.mTgApid != ice->mStts.mTgOldApid)
        ice->mCyl.mGObjAt.mSPrm &= ~1u;
    ice->mStts.Move();
    ice->mCyl.ClrTgHit();
    s32 state = ice->m45C;
    if (state == 2) {
        s32 reverb = iceGetReverb((s32)(s8)ice->current.roomNo);
        mDoAud_seStart(0x6A16, &ice->current.pos, 0, reverb);
    }
    if (state == 1 || state == 2) {
        ice->m458 = 90;
        auto *emitter = reinterpret_cast<IceEmitter *>(
            dComIfGp_getParticle()->set(0, 0x464, &ice->current.pos, nullptr, &ice->scale, 255,
                                        nullptr, -1, &ice->tevStr.mColorK0, nullptr, nullptr));
        if (emitter) {
            f32 x = ice->scale.x, y = ice->scale.y, z = ice->scale.z;
            emitter->mGlobalDynamicsScale.x = x;
            emitter->mGlobalDynamicsScale.y = y;
            emitter->mGlobalDynamicsScale.z = z;
            x = ice->scale.x;
            emitter->mGlobalParticleScale.x = x;
            emitter->mGlobalParticleScale.y = x;
            emitter->mGlobalParticleScale.z = x;
        }
        setAction(ice, ICE_FADE_ACTION);
    } else
        dComIfG_Ccsp_Set(&ice->mCyl);
}
/* 0235E820 */
void daObjIce_c::fade_out_retire_act_proc() {
    auto *ice = this;
    f32 alpha = (f32)ice->m454 - alphaFade, width = (f32)ice->m44C - widthFade;
    ice->m454 = alpha;
    if (alpha < 0.f)
        ice->m454 = 0.f;
    width = width >= 0.f ? width : 0.f;
    ice->m44C = width;
    ice->m450 = 1.f - cosine((s16)gabi::ftoi(width * 16384.f));
    if (width < .4f && ice->mBgw && ice->mBgw->ChkUsed()) {
        cBgS_Release(dComIfG_Bgsp(), ice->mBgw);
        u32 sw = objIceParameter(ice, 8, 0);
        if (sw != 255) {
            dComIfGs_onSwitch(sw, ice->home.roomNo);
        }
    }
    ice->m458 = (s32)((u32)ice->m458 - 1);
    if (ice->m458 <= 0)
        fopAcM_delete(ice);
}
namespace {
void setAlpha(J3DModelData *data, u8 alpha) {
    u16 i = 0;
    IceJointTree *header = iceGetJointTree(data);
    while (i < header->mJointNum) {
        auto *joint = reinterpret_cast<IceJoint *>(data->getJointNodePointer(i));
        IceMaterial *material = joint->mMesh;
        while (material) {
            J3DShape *shape = material->mShape;
            shape->mVisible = alpha ? 1 : 0;
            if (alpha) {
                IceTevBlock *block = material->mTevBlock;
                IceTevVtable *vt = block->mVtable;
                IceRgba *color = gabi::call_ptr<IceRgba *>(vt->getKonstColor.get(), block, 3);
                color->a = alpha;
                block = material->mTevBlock;
                vt = block->mVtable;
                color = gabi::call_ptr<IceRgba *>(vt->getKonstColor.get(), block, 3);
                block = material->mTevBlock;
                vt = block->mVtable;
                gabi::call_ptr(vt->setKonstColor.get(), block, 3, color);
                updateUniform(material, color);
            }
            material = material->mNext;
        }
        i = (u16)(i + 1);
        header = iceGetJointTree(data);
    }
}
IceShaderVariable *shaderVariableTable(IceShaderHeader *header) {
    s32 relative = header->mRelativeVariables;
    // HD: relative resource pointers and variable indices use 32-bit wraparound.
    return relative ? gabi::at<IceShaderVariable>(gabi::ea(&header->mRelativeVariables) + relative)
                    : nullptr;
}
void dirtyVariable(IceShaderEntry *entry, s32 index, IceShaderVariable *variable) {
    if (variable->mLocation < 0)
        return;
    entry->mDirtyFlags |= 4;
    auto *word = gabi::at<be<u32>>(gabi::ea(entry->mDirtyWords.get()) + (u32)(index >> 5) * 4);
    *word |= 1u << ((u32)index & 31);
}
void effectMatrices(daObjIce_c *ice) {
    if (!adjustmentInitialized) {
        adjustmentInitialized = 1;
        memcpy_g(&mtx_adj, &initialAdjustment, sizeof(Mtx34));
    }
    f32 sx = 1.8f / (f32)ice->scale.x, sy = 1.8f / (f32)ice->scale.y;
    IceCamera *camera = iceGetCamera();
    gabi::Local<cXyz> view, light, half;
    gabi::Local<Mtx34> look;
    iceSubtractVector(&ice->eyePos, view.get(), &camera->mEye);
    iceLightVector(&ice->tevStr.mLightPosWorld, &ice->eyePos, light.get());
    iceHalfAngle(view.get(), light.get(), half.get());
    iceLookAt(look.get(), &zeroVector, &upVector, half.get());
    Mtx34 *matrix = mDoMtx_stack_c::get();
    iceScaleMatrix(matrix, sx, sy, 1.f);
    iceConcatMatrix(matrix, &mtx_adj, matrix);
    iceConcatMatrix(matrix, look.get(), matrix);
    matrix->m[0][3] = 0.f;
    matrix->m[1][3] = 0.f;
    matrix->m[2][3] = 0.f;
    PSMTXCopy(matrix, &ice->mEffectLarge);
    iceScaleMatrix(matrix, .5f, .5f, 1.f);
    iceConcatMatrix(matrix, &mtx_adj, matrix);
    iceConcatMatrix(matrix, look.get(), matrix);
    matrix->m[0][3] = 0.f;
    matrix->m[1][3] = 0.f;
    matrix->m[2][3] = 0.f;
    PSMTXCopy(matrix, &ice->mEffectSmall);
    J3DModel *model = ice->mModel;
    u16 count = hdModel(model)->mMaterialNum;
    for (u16 i = 0; i < count; i++) {
        IceShaderEntry *entry = &hdModel(model)->mShaderEntries[i];
        for (u32 j = 0; j < 8; j++) {
            gabi::Local<be<s32>> index;
            *index.get() = -1;
            IceTextureInfo *info = iceGetTextureInfo(entry, j, index.get());
            if (!info)
                continue;
            u32 type = info->mInfo;
            if (type < 10 || type > 11)
                continue;
            IceShaderHeader *header = entry->mHeader;
            IceShaderVariable *variables = shaderVariableTable(header);
            s32 n = *index.get();
            auto *variable = gabi::at<IceShaderVariable>(gabi::ea(variables) +
                                                         (u32)n * sizeof(IceShaderVariable));
            dirtyVariable(entry, n, variable);
            header = entry->mHeader;
            variables = shaderVariableTable(header);
            u16 linked = variable->mLinked;
            dirtyVariable(entry, linked, &variables[linked]);
            gptr<Mtx34> *destination = iceGetTextureMatrix(entry, j);
            if (destination && destination->get()) {
                if (i == 0)
                    *destination = &ice->mEffectSmall;
                else if (i == 1)
                    *destination = &ice->mEffectLarge;
            }
        }
    }
}
} // namespace
/* 0235DFF8 */
BOOL daObjIce_c::_execute() {
    auto *ice = this;
    ice->mCyl.SetR((75.f * (f32)ice->scale.x) * (f32)ice->m44C);
    ice->mCyl.SetH((120.f * (f32)ice->scale.y) * (f32)ice->m450);
    ice->set_mtx();
    dBgW *bg = ice->mBgw;
    if (bg && bg->ChkUsed())
        bg->Move();
    ptmf_call(gabi::ea(&ice->m43C), ice);
    return 1;
}
/* 0235E0D0 */
BOOL daObjIce_c::_draw() {
    auto *ice = this;
    auto light = dKy_getEnvlight();
    settingTevStruct(light, 0, &ice->current.pos, &ice->tevStr);
    light = dKy_getEnvlight();
    setLightTevColorType(light, ice->mModel.get(), &ice->tevStr);
    iceSetEvent(dComIfGp_getPEvtManager(), "FREEZE_ICE", 255);
    if (dComIfGp_event_runCheck() && iceCheckEvent(dComIfGp_getPEvtManager(), "FREEZE_ICE"))
        dComIfGd_setList();
    else
        dComIfGd_setListMaskOff();
    J3DModelData *data = ice->mModel->getModelData();
    u8 alpha = (u8)gabi::ftoi(ice->m454);
    setAlpha(data, alpha);
    effectMatrices(ice);
    mDoExt_modelUpdateDL(ice->mModel.get(), 0);
    dComIfGd_setList();
    return 1;
}
/* 0235DF6C */
s32 daObjIce_Create(daObjIce_c *ice) { return ice->_create(); }
/* 0235DFF4 */
BOOL daObjIce_Delete(daObjIce_c *ice) { return ice->_delete(); }
/* 0235E0CC */
BOOL daObjIce_Execute(daObjIce_c *ice) { return ice->_execute(); }
/* 0235E6B8 */
BOOL daObjIce_Draw(daObjIce_c *ice) { return ice->_draw(); }
/* 0235E6BC */
BOOL daObjIce_IsDelete(daObjIce_c *ice) { return 1; }
/* 0235D460 */
void ice_normalizeColor(IceColor *out, void *rgba) {
    auto *color = static_cast<IceRgba *>(rgba);
    f32 r = color->r, g = color->g, b = color->b, alpha = color->a;
    out->r = r / 255.f;
    out->g = g / 255.f;
    out->b = b / 255.f;
    out->a = alpha / 255.f;
}
/* 0235E94C */
void ice_staticInit() {
    for (u32 i = 0; i < 4; i++)
        headerData.words[i] = 0;
    __register_global_object(gabi::ea(staticRegistration0));
    minimumPi = -3.1415927410125732f;
    maximumPi = 3.1415927410125732f;
    iceInitCharacter(&staticCharacter);
    __register_global_object(gabi::ea(staticRegistration1));
    iceInitByte(&staticByte);
    __register_global_object(gabi::ea(staticRegistration2));
    l_co_radius_table[0] = 40.f;
    l_co_radius_table[1] = 65.f;
    l_co_radius_table[2] = 75.f;
    l_co_height_table[0] = 120.f;
    l_co_height_table[1] = 90.f;
    l_co_height_table[2] = 60.f;
    cylinderRadius = 75.f;
    cylinderHeight = 120.f;
    widthFade = .011111111f;
    alphaFade = 2.8333333f;
}
/* 0235EA60 */
void ice_staticDtor(void *object, u32 flags) {
    if (object && (flags & 1))
        operator_delete(object);
}
/* 0235EA74 */
void ice_destructor(daObjIce_c *ice, u32 flags) {
    if (ice) {
        dCcD_Cyl_dt(&ice->mCyl, 2);
        dCcD_Stts_dt(&ice->mStts, 2);
        iceDestroyActor(ice, 0);
        if (flags & 1)
            operator_delete(ice);
    }
}
/* 0235EAE0 */
void ice_stringNoop(void *object) {}

/* 0235EAE4 */
u32 objIceParameter(daObjIce_c *ice, u32 width, u32 shift) {
    u32 mask = (width & 32) ? 0 : (1u << (width & 31));
    u32 value = (shift & 32) ? 0 : ((u32)ice->mParameters >> (shift & 31));
    return value & (mask - 1);
}
/* WWHD glue (tools/verify/autoglue.py)
data l_cyl_src 1002B12C nocheck
data l_co_radius_table 1046A068 nocheck
data l_co_height_table 1046A074 nocheck
data cylinderRadius 1046A044 nocheck
data cylinderHeight 1046A048 nocheck
data widthFade 1046A04C nocheck
data alphaFade 1046A050 nocheck
data mtx_adj 101FDBD8 nocheck
data initialAdjustment 101CA364 nocheck
data adjustmentInitialized 101FDBD4 nocheck
data zeroVector 101FFBA8 nocheck
data upVector 101FFBC0 nocheck
data minimumPi 1046A03C nocheck
data maximumPi 1046A040 nocheck
data staticCharacter 1046A054 nocheck
data staticByte 1046A055 nocheck
data headerData 1046A058 nocheck
data staticRegistration0 101CA3B4 nocheck
data staticRegistration1 101CA3C0 nocheck
data staticRegistration2 101CA3CC nocheck
str 1002B0EC
str 1002B174
str 1002B170
str 1002B1B8
ext 025F19F8 void iceXYZrotM(Mtx34*, s16, s16, s16)
ext 024F2478 dBgW* iceNewBackground(cBgD_t*, u32, Mtx34*)
ext 0274D458 void iceConvertColor(IceColor*, IceColor*, f32)
ext 027F9F0C IceColor* iceGetUniform(be<u32>*, s32)
ext 02515FB8 void iceConstructGObj(dCcD_Cyl*)
ext 02018590 void iceConstructCylinder(cM3dGCyl*)
ext 0273AD10 void* iceAllocateMatrix(u32)
ext 027FA3F4 void iceReadColor(IceRgba*, IceShaderEntry*, s32)
ext 027F3F94 IceJointTree* iceGetJointTree(J3DModelData*)
ext 024F8020 IceCamera* iceGetCamera()
ext 0201ADE0 void iceSubtractVector(cXyz*, cXyz*, cXyz*)
ext 02563F64 void iceLightVector(cXyz*, cXyz*, cXyz*)
ext 028E9E88 void iceHalfAngle(cXyz*, cXyz*, cXyz*)
ext 028E9684 void iceLookAt(Mtx34*, cXyz*, cXyz*, cXyz*)
ext 028E945C void iceScaleMatrix(Mtx34*, f32, f32, f32)
ext 028E9108 void iceConcatMatrix(Mtx34*, Mtx34*, Mtx34*)
ext 027FA974 IceTextureInfo* iceGetTextureInfo(IceShaderEntry*, u32, be<s32>*)
ext 027FA678 gptr<Mtx34>* iceGetTextureMatrix(IceShaderEntry*, u32)
ext 02543F10 void iceSetEvent(dEvent_manager_c*, const char*, s32)
ext 025445B8 s32 iceCheckEvent(dEvent_manager_c*, const char*)
ext 02520540 s32 iceGetReverb(s32)
ext 025D50BC void iceDestroyActor(fopAc_ac_c*, s32)
ext 028ED6F8 void iceInitCharacter(be<u8>*)
ext 028EAB2C void iceInitByte(be<u8>*)
*/
