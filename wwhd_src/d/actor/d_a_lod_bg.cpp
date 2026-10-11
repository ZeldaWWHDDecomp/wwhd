// HD: LOD uses a resource-name handle and an HD member-function dispatch pair.
// Retail member names are retained; HD also draws Dragon Roost Island extra models.
#include "d/actor/d_a_lod_bg.h"
using Lod = daLodbg_c;
static LodText lodbgFile;
static LodText lodbgModelAssert;
static LodText lodbgModel2Assert;
static LodText lodbgFormat;
static LodText lodbgModelPath;
static LodText lodbgLightPath;
static LodText lodbgModel2Path;
static LodClipper clipper;
static f32 clipFar;
static LodStaticState staticState;
static LodRegistration staticRegistration0;
static LodRegistration staticRegistration1;
static LodRegistration staticRegistration2;
static f32 minimumPi;
static f32 maximumPi;
static u8 staticChar;
static u8 staticByte;
static Mtx34 matrixStack;
static LodRoomState roomStates[64] = {};

/* 021B43BC */
BOOL daLodbg_c::createHeap() {
    auto *l = this;
    J3DModelData *data = l->mModelData;
    if (!data) {
        lodbgAssert<void>(lodbgFile.bytes, 0xB5, lodbgModelAssert.bytes);
        data = l->mModelData;
    }
    l->mModel = lodbgCreateModel<J3DModel *>(data, 0x80000, 0x11000022);
    if (!l->mModel)
        return 0;
    u32 room = l->mParameters;
    if (room == lodbg_room::WindfallIsland) {
        data = l->mModelData2;
        if (!data) {
            lodbgAssert<void>(lodbgFile.bytes, 0xC0, lodbgModel2Assert.bytes);
            data = l->mModelData2;
        }
        for (u32 i = 0; i < 2; i++) {
            l->mModel2[i] = lodbgCreateModel<J3DModel *>(data, 0x80000, 0x11000022);
            if (!l->mModel2[i]) {
                l->mModel = 0;
                l->mModel2[0] = 0;
                return 0;
            }
            if (i == 0)
                data = l->mModelData2;
        }
    } else if (room == lodbg_room::ForsakenFortress || room == lodbg_room::DragonRoostIsland) {
        data = l->mModelData2;
        if (data) {
            l->mModel2[0] = lodbgCreateModel<J3DModel *>(data, 0x80000, 0x11000022);
            if (!l->mModel2[0]) {
                l->mModel = 0;
                return 0;
            }
        }
    }
    return 1;
}
/* 021B4524 */
BOOL lodbg_heapCallback(Lod *l) { return lodbgCreateHeapTail<BOOL>(l); }
/* 021B4528 */
void lodbg_colorConvert(ColorF *out, void *input) {
    auto *color = static_cast<GXColor *>(input);
    u8 r = color->r, g = color->g, b = color->b, alpha = color->a;
    out->r = (f32)r / 255.0f;
    out->g = (f32)g / 255.0f;
    out->b = (f32)b / 255.0f;
    out->a = (f32)alpha / 255.0f;
}
/* 021B4C6C */
BOOL daLodbg_c::execute() {
    auto *l = this;
    u32 self;
    u32 target = actionTarget(l, self);
    return gabi::call<BOOL>(target, ptr(self));
}
/* 021B4CB0 */
BOOL lodbg_deleteResources(Lod *l) {
    if (l->mResourceActive) {
        nameAccess(l);
        if (lodbgResourceState<s32>(ptr(l->mResourceName.text)) > 0)
            return 0;
        nameAccess(l);
        lodbgReleaseResource<void>(ptr(l->mResourceName.text));
        l->mResourceActive = 0;
        lodbgDeleteHeap<void>(l);
        l->mModelData = 0;
        l->mModel2[1] = 0;
        l->mModelData2 = 0;
        l->mDrawModel2 = 0;
        l->mModel2[0] = 0;
        l->mModel = 0;
        l->mAlpha = 0;
    }
    return 1;
}
/* 021B4D78 */
BOOL lodbg_delete(Lod *l) { return lodbgDeleteModelDataTail<BOOL>(l); }
/* 021B4D7C */
BOOL lodbg_deleteWrapper(Lod *l) { return lodbgDeleteTail<BOOL>(l); }
/* 021B4D80 */
Lod *lodbg_constructor(Lod *l) {
    if (!l) {
        l = gabi::at<Lod>(lodbgAllocate<u32>(0x3F8));
        if (!l)
            return l;
    }
    lodbgConstructActor<void>(l);
    l->__vtbl = LodActorVtable;
    auto *name = &l->mResourceName;
    name->text = gabi::ea(name->bytes);
    name->vtable = LodSafeStringVtable;
    name->capacity = 0x20;
    name->bytes[0x1F] = 0;
    name->vtable = LodBufferVtable;
    *gabi::at<be<u8>>(name->text) = 0;
    name->vtable = LodResourceNameVtable;
    lodbgFormatName(name, lodbgFormat.bytes, (u32)l->mParameters);
    f32 scaled = l->scale.x * 20000.0f;
    setAction(l, lodbg_state::CreateWait);
    s32 room = (s8)(u8)l->mParameters;
    l->scale.x = scaled;
    l->scale.z = scaled + 20000.0f;
    lodbgInitTev<void>(&l->tevStr, room, 255);
    return l;
}
/* 021B4EB8 */
BOOL lodbg_createStep(Lod *l) {
    s32 room = (s8)lodbgPlay()->mRoomNo;
    if ((u32)room == l->mParameters)
        return 4;
    u32 self;
    u32 target = actionTarget(l, self);
    gabi::call<void>(target, ptr(self));
    s16 slot = l->mExecuteFunc.slot;
    if (slot != -1)
        return 4;
    if (slot == 0 || ((s16)l->mExecuteFunc.adjustment == 0 &&
                      (u32)l->mExecuteFunc.target == lodbg_state::ReadWait))
        return 0;
    return 4;
}
/* 021B4F90 */
BOOL lodbg_create(Lod *l) {
    if (!gabi::ea(dComIfGp_getPlayer(0)))
        return 0;
    u32 flags = (u32)l->actor_condition;
    if (!(flags & fopAcCnd_INIT_e)) {
        if (l) {
            lodbgConstructTail<void>(l);
            flags = (u32)l->actor_condition;
        }
        l->actor_condition = flags | fopAcCnd_INIT_e;
    }
    return lodbgCreateStepTail<BOOL>(l);
}
/* 021B5010 */
BOOL daLodbg_c::execCreateWait() {
    auto *l = this;
    u32 player = gabi::ea(dComIfGp_getPlayer(0));
    f32 distance = lodbgPlayerDistance<f32>(l, ptr(player));
    if (distance > l->scale.x)
        return 1;
    nameAccess(l);
    if (!lodbgLoadResource<s32>(ptr(l->mResourceName.text), 0)) {
        nameAccess(l);
        return 1;
    }
    l->mResourceActive = 1;
    setAction(l, lodbg_state::ReadWait);
    return 1;
}
/* 021B50C4 */
BOOL daLodbg_c::createModelData() {
    auto *l = this;
    nameAccess(l);
    l->mModelData =
        lodbgResourceModelData<J3DModelData *>(ptr(l->mResourceName.text), lodbgModelPath.bytes);
    if (!l->mModelData)
        return 0;
    u32 room = l->mParameters;
    if (room == lodbg_room::WindfallIsland) {
        nameAccess(l);
        l->mModelData2 = lodbgResourceModelData<J3DModelData *>(ptr(l->mResourceName.text),
                                                                lodbgLightPath.bytes);
        return l->mModelData2 != 0;
    }
    if ((room == lodbg_room::ForsakenFortress && !eventOn(0x1820)) ||
        (room == lodbg_room::DragonRoostIsland && !eventOn(0x3908))) {
        nameAccess(l);
        l->mModelData2 = lodbgResourceModelData<J3DModelData *>(ptr(l->mResourceName.text),
                                                                lodbgModel2Path.bytes);
        return l->mModelData2 != 0;
    }
    return 1;
}
/* 021B5224 */
BOOL daLodbg_c::execReadWait() {
    auto *l = this;
    nameAccess(l);
    s32 state = lodbgResourceState<s32>(ptr(l->mResourceName.text));
    if (state > 0)
        return 1;
    setAction(l, lodbg_state::DeleteWait);
    if (state < 0)
        return 1;
    if (!lodbgCreateModelDataTail<BOOL>(l) || !lodbgEnterHeap<BOOL>(l, ptr(LodHeapCallback), 0))
        return 1;
    u32 self;
    u32 target = actionTarget(l, self);
    gabi::call<void>(target, ptr(self));
    u32 model = gabi::ea(l->mModel.get());
    l->cullMtx = model ? model + 0xC8 : 0;
    return 1;
}
/* 021B58D8 */
void lodbg_staticInit() {
    staticState.words[2] = 0;
    staticState.words[0] = 0;
    staticState.words[3] = 0;
    staticState.words[1] = 0;
    lodbgRegisterStatic<void>(&staticRegistration0);
    minimumPi = -3.1415927410125732f;
    maximumPi = 3.1415927410125732f;
    lodbgInitStaticChar<void>(&staticChar);
    lodbgRegisterStatic<void>(&staticRegistration1);
    lodbgInitStaticByte<void>(&staticByte);
    lodbgRegisterStatic<void>(&staticRegistration2);
}
/* 021B596C */
BOOL lodbg_isDelete(Lod *l) { return 1; }
/* 021B5974 */
void lodbg_terminateName(void *obj) {
    auto *name = static_cast<lodbg_local::ResourceName *>(obj);
    *gabi::at<be<u8>>((u32)name->text + (u32)name->capacity - 1) = 0;
}
/* 021B598C */
void lodbg_safeStringDestructor(void *obj, s32 flags) {
    if (obj && (flags & 1))
        lodbgFree<void>(obj);
}
/* 021B59A0 */
void lodbg_bufferDestructor(void *obj, s32 flags) {
    if (obj && (flags & 1))
        lodbgFree<void>(obj);
}
/* 021B59B4 */
void lodbg_actorDestructor(Lod *l, s32 flags) {
    if (l) {
        lodbgDestroyActor<void>(l, 0);
        if (flags & 1)
            lodbgFree<void>(l);
    }
}
/* 021B5A08 */
void lodbg_baseNoop(void *obj) {}
/* 021B5A0C */
void lodbg_fixedStringDestructor(void *obj, s32 flags) {
    if (obj && (flags & 1))
        lodbgFree<void>(obj);
}
/* 021B5A20 */
void lodbg_resourceNameDestructor(void *obj, s32 flags) {
    if (obj && (flags & 1))
        lodbgFree<void>(obj);
}
namespace {
void updateMaterials(Lod *l, J3DModelData *data) {
    gabi::Local<ColorF> normalized, tinted;
    u16 i = 0;
    LodMaterialInfo *info = lodbgMaterialTable<LodMaterialInfo *>(data);
    while (i < info->mMaterialNum) {
        LodMaterialView *material = lodbgMaterial(data, i);
        LodTevBlock *block = material->mTevBlock;
        GXColor *color = gabi::at<GXColor>(block->getKColor(3));
        color->a = l->mAlpha;
        block = material->mTevBlock;
        color = gabi::at<GXColor>(block->getKColor(3));
        block = material->mTevBlock;
        block->setKColor(3, color);
        lodbgNormalizeColor<void>(normalized.get(), color);
        lodbgTintColor<void>(tinted.get(), normalized.get(), 1.0f);
        material->mChannelFlags = (u32)material->mChannelFlags | 0x400;
        ColorF *out = lodbgMaterialChannel<ColorF *>(&material->mChannelFlags, 10);
        f32 alpha = (f32)color->a / 255.0f;
        f32 red = tinted->r, green = tinted->g, blue = tinted->b;
        out->g = green;
        out->b = blue;
        out->r = red;
        out->a = alpha;
        i = (u16)(i + 1);
        info = lodbgMaterialTable<LodMaterialInfo *>(data);
    }
}
} // namespace
/* 021B45DC */
BOOL daLodbg_c::draw() {
    auto *l = this;
    if (!l->heap.get() || !l->mAlpha)
        return 1;
    u32 room = l->mParameters;
    if (room == lodbg_room::TowerOfTheGods && !eventOn(0x1E40))
        return 1;
    u32 env = lodbgEnvLight<u32>();
    lodbgSetTev<void>(ptr(env), 1, (void *)nullptr, &l->tevStr);
    env = lodbgEnvLight<u32>();
    lodbgSetLight<void>(ptr(env), l->mModel.get(), &l->tevStr);
    updateMaterials(l, l->mModel->mModelData.get());
    clipper.mFar = 500000.0f;
    lodbgUpdateClipper<void>(&clipper);
    drawModel(l->mModel);
    if (l->mModel2[0] && l->mDrawModel2) {
        if (room == lodbg_room::WindfallIsland) {
            updateMaterials(l, l->mModel2[0]->mModelData.get());
            for (u32 i = 0; i < 2; i++)
                drawModel(l->mModel2[i]);
        } else if (room == lodbg_room::ForsakenFortress || room == lodbg_room::DragonRoostIsland) {
            env = lodbgEnvLight<u32>();
            lodbgSetLight<void>(ptr(env), l->mModel2[0].get(), &l->tevStr);
            updateMaterials(l, l->mModel2[0]->mModelData.get());
            drawModel(l->mModel2[0]);
        }
    }
    clipper.mFar = (f32)clipFar;
    lodbgUpdateClipper<void>(&clipper);
    return 1;
}
/* 021B4C68 */
BOOL lodbg_drawWrapper(Lod *l) { return lodbgDrawTail<BOOL>(l); }
/* 021B5340 */
BOOL daLodbg_c::execDeleteWait() {
    auto *l = this;
    u32 room = 0;
    if (l->heap.get()) {
        u32 view = lodbgPlay()->mCurrentView;
        gabi::Local<cXyz> delta, horizontal;
        lodbgSubtract<void>(&gabi::at<LodCamera>(view)->mEye, delta.get(), &l->current.pos);
        horizontal->y = 0;
        horizontal->x = delta->x;
        horizontal->z = delta->z;
        f64 square = lodbgSquare<f64>(horizontal.get());
        f32 distance = (f32)(lodbgSqrt<f64>(square) + 700.0);
        if (distance < l->scale.z + 100000.0f) {
            room = l->mParameters;
            if (distance > l->scale.x + 100000.0f) {
                l->mAlpha = 0;
                return 1;
            }
            play();
            LodRoomState *roomState = &roomStates[room];
            if (roomState->mStatus & 1) {
                play();
                if (!(roomState->mStatus & 4)) {
                    l->mAlpha = 0;
                    return 1;
                }
            }
            f32 y = 150000.0f - distance;
            l->mAlpha = 255;
            if (y < 0)
                y *= 0.1f;
            else
                y = 0;
            lodbgTranslate<void>(&matrixStack, (f32)l->current.pos.x, l->current.pos.y + y,
                                 (f32)(f32)l->current.pos.z);
            lodbgRotateY<void>(&matrixStack, (s32)l->shape_angle.y);
            copyMatrix(l->mModel);
            if (l->mModel2[0]) {
                if (room == lodbg_room::WindfallIsland) {
                    s32 light = lodbgRenewLight<s32>();
                    l->mDrawModel2 = (u8)light;
                    if (light) {
                        lodbgTranslate<void>(&matrixStack, 630.46337890625f, y + 4044.508056640625f,
                                             -202724.0f);
                        s32 angle = lodbgGetLightAngle<s32>();
                        lodbgRotateY<void>(&matrixStack, angle);
                        copyMatrix(l->mModel2[0]);
                        lodbgRotateY<void>(&matrixStack, -32768);
                        copyMatrix(l->mModel2[1]);
                    }
                } else if (room == lodbg_room::ForsakenFortress ||
                           room == lodbg_room::DragonRoostIsland) {
                    copyMatrix(l->mModel2[0]);
                    l->mDrawModel2 = 1;
                }
            }
            return 1;
        }
        if (l->mAlpha) {
            l->mAlpha = 0;
            return 1;
        }
    }
    if (lodbgDeleteModelDataTail<BOOL>(l))
        setAction(l, lodbg_state::CreateWait);
    return 1;
}

/* WWHD glue (tools/verify/autoglue.py)
data lodbgFile 10013F6C nocheck
data lodbgModelAssert 10013F58 nocheck
data lodbgModel2Assert 10013F7C nocheck
data lodbgFormat 10014020 nocheck
data lodbgModelPath 10014034 nocheck
data lodbgLightPath 10014028 nocheck
data lodbgModel2Path 10014040 nocheck
data clipper 1048CFF0
data clipFar 1048D04C
data staticState 10464E40
data staticRegistration0 101B906C
data staticRegistration1 101B9078
data staticRegistration2 101B9084
data minimumPi 10464E34
data maximumPi 10464E38
data staticChar 10464E3C
data staticByte 10464E3D
data matrixStack 1048D0CC
data roomStates 1047E6CC nocheck
*/
