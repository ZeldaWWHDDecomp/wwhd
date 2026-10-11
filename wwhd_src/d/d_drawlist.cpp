/* Draw-list submission, mirror rendering and CPU depth peeks.
 * GameCube names and private asserted HD views; every entry is verified against WWHD.
 * HD: GX2/BFRES replaces GX/J3D rendering; mirror geometry and varargs retain their guest frames.
 * Methods retain the verified argument widths; nullable GHS constructor/deleting adapters stay free. */
#include "d/d_drawlist.h"

using namespace gabi;
using namespace dDlstHD;
using BufferMember = be<u32> dDlstHD::dDlst_list_c::*;
// Native submission metadata; the SafeString used for each call remains guest-resident.
struct Allocation {
    BufferMember member;
    u32 capacity;
    const char* name;
};
static GlobalDestructor l_drawDestructorList;
static GlobalDestructor l_randomDestructorList;
static f32 mDrawInitialX;
static f32 mDrawInitialY;
static f32 mRandomInitialX;
static f32 mRandomInitialY;
static u8 l_drawHeaderByte;
static u8 l_drawArchiveByte;
static u8 l_randomHeaderByte;
static u8 l_randomArchiveByte;
static dDlst_base_c l_drawBase;
static cM_rnd_c l_randomState;
static DrawVector l_mirrorEye;
static DrawVector l_mirrorTarget;
static Matrix34 l_mirrorTextureMtx;
static GlobalDestructor l_drawGlobalDestructor;
static GlobalDestructor l_drawHeaderDestructor;
static GlobalDestructor l_drawArchiveDestructor;
static GlobalDestructor l_randomGlobalDestructor;
static GlobalDestructor l_randomHeaderDestructor;
static GlobalDestructor l_randomArchiveDestructor;
static f32 l_zero;
static f32 l_one;
static f32 mWipeRate;
static u32 mWipeColor;
static f32 mWipeSpeed;
static u8 mWipe;
static f32 l_drawInitialY;
static f32 l_drawInitialX;
static f32 l_randomInitialY;
static f32 l_randomInitialX;
static u32 mXluDrawBuffer;
static u32 mOpaDrawBuffer;
static f32 l_zSortStep;
static f32 l_zSortFar;
static f32 l_zSortNear;
static u32 mHeapManager;
static u32 mResourceControl;
static u32 mDrawVertexCount;
static u32 mDrawStart;
static u32 mDrawPrimitive;
static u32 mDrawCount;
static u32 mTextureManager;
static u32 mGpuContext;
static u32 l_textureGuard;
static u32 l_textureVtable;
static f32 l_uniformOne;
static f32 l_uniformZero;
static f32 l_mirrorFar;
static f32 l_mirrorOrthoTop;
static f32 l_mirrorOrthoBottom;
static f32 l_mirrorScaleY;
static u32 l_mirrorMatrixGuard;
static f32 l_mirrorEyeY;
static f32 l_mirrorEyeX;
static u32 l_mirrorTargetGuard;
static u32 l_mirrorEyeGuard;
static f32 l_degreesToRadians;
static f32 l_half;
static f32 l_mirrorDepth;
static f32 l_mirrorBottom;
static f32 l_mirrorRight;
static f32 l_mirrorTop;
static f32 l_mirrorLeft;
static u32 mDepthReadPixel;
static f32 l_depthYPack;
static f32 l_depthXPack;
static f32 l_unsignedSplit;
static f32 l_depthMultiplier;
static f32 l_depthHeight;
static f32 l_depthWidth;
static u32 mDepthManager;

/* 0252CA4C */
void dDlst_texSpecmapST(u32 position, u32 model, u32 data, f32 scale) {
    auto* positionView = gabi::at<u8>(position);
    auto* dataView = gabi::at<SpecularModelData>(data);
    const f32 one = l_one.get(), reciprocal = f32(one / scale);
    Local<DrawVector> eye, light, difference;
    Local<f32[12]> lookAt;
    Local<u32> textureIndex;
    const u32 camera = drawHD_getCamera();
    drawHD_positionToCamera(positionView, eye.get(), gabi::at<u8>(camera + 0xDC));
    drawHD_transformLightPosition(gabi::at<u8>(model + 0x84), positionView, light.get());
    drawHD_PSVECSubtract(eye.get(), light.get(), difference.get());
    drawHD_C_MTXLookAt(lookAt.get(), gabi::at<u8>(kSpecularEye), gabi::at<u8>(kSpecularUp),
                       difference.get());
    const u32 matrix = data + 0xF8;
    drawHD_PSMTXScale(gabi::at<u8>(matrix), reciprocal, reciprocal, one);
    drawHD_PSMTXConcat(gabi::at<u8>(matrix), gabi::at<u8>(kSpecularBaseMtx), gabi::at<u8>(matrix));
    drawHD_PSMTXConcat(gabi::at<u8>(matrix), lookAt.get(), gabi::at<u8>(matrix));
    const f32 zero = l_zero.get();
    u32 remaining = dataView->mMaterialCount.get();
    dataView->mTexMtx[3] = zero;
    dataView->mTexMtx[11] = zero;
    dataView->mTexMtx[7] = zero;
    for (u32 offset = 0; remaining; offset += 60, --remaining) {
        const u32 material = dataView->mpMaterials.get() + offset;
        for (u32 slot = 0; slot < 8; slot++) {
            *gabi::at<be<u32>>(gabi::ea(textureIndex.get())) = 0xFFFFFFFF;
            const u32 texture =
                drawHD_MaterialGetTexture(gabi::at<u8>(material), slot, textureIndex.get());
            if (!texture || gabi::at<Texture>(texture)->mFormat.get() != 10)
                continue;
            u32 definition = gabi::at<SpecularMaterial>(material)->mpDefinition.get(),
                relative = gabi::at<MaterialDefinition>(definition)->mTexMtxOffset.get();
            const u32 table = relative ? definition + 0x34 + relative : 0;
            const u32 index = (*gabi::at<be<u32>>(gabi::ea(textureIndex.get()))).get(),
                      entry = table + index * 20;
            if (s32(gabi::at<be<u32>>(gabi::ea(&gabi::at<TextureMatrixInfo>(entry)->mIndex))
                        ->get()) >= 0) {
                const u32 flags = gabi::at<SpecularMaterial>(material)->mFlags.get(),
                          bits = gabi::at<SpecularMaterial>(material)->mpDirtyBits.get(),
                          word = (u32(s32(index) >> 5)) * 4;
                gabi::at<SpecularMaterial>(material)->mFlags = u16(flags | 4);
                *gabi::at<be<u32>>((bits) + (word)) =
                    (*gabi::at<be<u32>>((bits) + (word))).get() | (u32(1) << (index & 31));
                definition = gabi::at<SpecularMaterial>(material)->mpDefinition.get();
            }
            relative = gabi::at<MaterialDefinition>(definition)->mTexMtxOffset.get();
            const u32 alternateTable = relative ? definition + 0x34 + relative : 0;
            const u32 alternate = gabi::at<TextureMatrixInfo>(entry)->mAlternate.get();
            if (s32((*gabi::at<be<u32>>((alternateTable + alternate * 20) + (4))).get()) >= 0) {
                const u32 flags = gabi::at<SpecularMaterial>(material)->mFlags.get(),
                          bits = gabi::at<SpecularMaterial>(material)->mpDirtyBits.get(),
                          word = (alternate >> 5) * 4;
                gabi::at<SpecularMaterial>(material)->mFlags = u16(flags | 4);
                *gabi::at<be<u32>>((bits) + (word)) =
                    (*gabi::at<be<u32>>((bits) + (word))).get() | (u32(1) << (alternate & 31));
            }
            const u32 output = drawHD_MaterialGetTextureMatrix(gabi::at<u8>(material), slot);
            if (output)
                *gabi::at<be<u32>>(output) = matrix;
        }
    }
}

// HD: inlined mirror texture synchronization preserves all comparison and store ordering.
void bindMirrorTexture(u32 mirror, u32 texture, u32 textureObject, u32 unit) {
    const TextureMember checks[] = {&Texture::mDimension, &Texture::mWidth,   &Texture::mHeight,
                                    &Texture::mDepth,     &Texture::mNumMips, &Texture::mSwizzle,
                                    &Texture::mTileMode,  &Texture::mPitch,   &Texture::mAlignment};
    bool same = true;
    for (TextureMember member : checks)
        if ((gabi::at<Texture>(textureObject)->*member).get() !=
            (gabi::at<Texture>(texture)->*member).get()) {
            same = false;
            break;
        }
    if (!same)
        drawHD_TextureCopy(gabi::at<u8>(textureObject), gabi::at<u8>(texture));
    else {
        const u32 image = gabi::at<Texture>(texture)->mpImage.get(),
                  buffer = gabi::at<Texture>(texture)->mpBuffer.get();
        gabi::at<Texture>(textureObject)->mpImage = image;
        gabi::at<Texture>(textureObject)->mpBuffer = buffer;
        gabi::at<Texture>(textureObject)->mpGpuImage = image;
        gabi::at<Texture>(textureObject)->mpGpuBuffer = buffer;
    }
    const u32 material = gabi::at<dDlst_mirrorPacket>(mirror)->mpMaterial.get();
    u32 record = 0;
    if (gabi::at<MirrorMaterial>(material)->mTextureCount.get() > unit)
        record = gabi::at<MirrorMaterial>(material)->mpTextures.get() + unit * 20;
    drawHD_TextureBind(gabi::at<u8>(textureObject), gabi::at<u8>(record + 4), unit, 0);
}
/* 0252CC8C */
void dDlstHD::dDlst_window_c::setViewPort(f32 x, f32 y, f32 width, f32 height, f32 nearZ,
                                          f32 farZ) {
    const u32 window = gabi::ea(this);
    auto* windowView = this;
    windowView->mNearZ = nearZ;
    windowView->mXOrig = x;
    windowView->mHeight = height;
    windowView->mFarZ = farZ;
    windowView->mYOrig = y;
    windowView->mWidth = width;
}

/* 0252CCA8 */
void dDlstHD::dDlst_window_c::setScissor(f32 x, f32 y, f32 width, f32 height) {
    const u32 window = gabi::ea(this);
    auto* windowView = this;
    windowView->mScissorWidth = width;
    windowView->mScissorYOrig = y;
    windowView->mScissorHeight = height;
    windowView->mScissorXOrig = x;
}

/* 0252CCBC */
u32 constructDrawBase(u32 object) {
    if (!object)
        object = drawHD_operatorNew(4);
    if (object)
        gabi::at<dDlst_base_c>(object)->mVtable = kDrawBaseVtable;
    return object;
}

/* 0252CCFC */
void destroyDrawBase(u32 object, u32 flags) {
    if (object && (flags & 1))
        drawHD_operatorDeleteWithFlags(gabi::at<u8>(object), flags);
}

/* 0252CD10 */
void dDlstHD::cM_rnd_c::init(u32 a, u32 b, u32 c) {
    const u32 object = gabi::ea(this);
    auto* objectView = this;
    objectView->mMultiplier = b;
    objectView->mIncrement = c;
    objectView->mSeed = a;
}

/* 0252CD20 */
u32 dDlst_effectLine_construct(u32 object) {
    if (!object)
        object = drawHD_operatorNew(0x38);
    if (!object)
        return 0;
    auto* objectView = gabi::at<dDlst_effectLine_c>(object);
    drawHD_drawBaseConstructEntry(gabi::at<u8>(object));
    objectView->mVtable = kEffectLineVtable;
    drawHD_randomInitEntry(gabi::at<u8>(object + 4), 100, 100, 100);
    drawHD_bzero(gabi::at<u8>(object + 0x1C), 4);
    f32 zero = l_zero.get();
    objectView->mLineSpeed = 0;
    objectView->mWidth = zero;
    objectView->mLineLength = 0;
    objectView->mHeight = zero;
    objectView->mLineCount = 0;
    objectView->mDepth = zero;
    objectView->mLineMode = 0;
    objectView->mScale = zero;
    return object;
}

/* 0252CDF0 */
void dDlstHD::dDlst_effectLine_c::update(u32 vector, u32 colour, u32 a, u32 b, u32 c, u32 d, f32 x,
                                         f32 y, f32 z, f32 w) {
    const u32 object = gabi::ea(this);
    auto* objectView = this;
    auto* vectorView = gabi::at<VectorWords>(vector);
    objectView->mPositionX = vectorView->x.get();
    objectView->mPositionY = vectorView->y.get();
    objectView->mPositionZ = vectorView->z.get();
    objectView->mColor = (*gabi::at<be<u32>>(colour)).get();
    objectView->mLineCount = a;
    objectView->mDepth = z;
    objectView->mLineSpeed = c;
    objectView->mLineMode = d;
    objectView->mScale = w;
    objectView->mHeight = y;
    objectView->mLineLength = b;
    objectView->mWidth = x;
    u32 list = drawHD_getPlay() + 0x5D30;
    drawHD_listSetEntry(gabi::at<u8>(list), gabi::at<u8>(list + 0x1DC), gabi::at<u8>(list + 0x1E0),
                        gabi::at<u8>(object));
}

/* 0252CDC0 */
u32 dDlstHD::dDlst_list_c::set(u32 current, u32 end, u32 packet) {
    u32 atSlot = (*gabi::at<be<u32>>(current)).get(), limit = (*gabi::at<be<u32>>(end)).get();
    if (atSlot >= limit)
        return 0;
    *gabi::at<be<u32>>(atSlot) = packet;
    atSlot = (*gabi::at<be<u32>>(current)).get();
    *gabi::at<be<u32>>(current) = atSlot + 4;
    return 1;
}

/* 0252CE74 */
u32 dDlst_mirrorPacketonstruct(u32 mirror) {
    if (!mirror)
        mirror = drawHD_operatorNew(0x3428);
    if (!mirror)
        return 0;
    auto* mirrorView = gabi::at<dDlst_mirrorPacket>(mirror);
    drawHD_J3DPacketConstruct(gabi::at<u8>(mirror));
    mirrorView->mVtable = u32(kMirrorPacketVtable);
    drawHD_bzero(&mirrorView->mPosMtx, 48);
    drawHD_bzero(&mirrorView->mTexMtx, 64);
    drawHD_bzero(&mirrorView->mLightMtx, 48);
    drawHD_bzero(&mirrorView->mShadowPolyCount, 0x2408);
    mirrorView->mShadowPolyCount = u16(0);
    mirrorView->mShadowPolyVtable = u32(kShadowPolyVtable);
    drawHD_bzero(&mirrorView->mTex, 64);
    mirrorView->mAlpha = u8(0);
    drawHD_ResourceTextureConstruct(&mirrorView->mSourceTexture);
    drawHD_TextureConstruct(&mirrorView->mTexObj);
    drawHD_TextureConstruct(&mirrorView->mSceneTexture);
    drawHD_TextureConstruct(&mirrorView->mIndirectTexture);
    u32 packets = gabi::ea(&mirrorView->mPackets);
    mirrorView->mpMaterial = u32(0);
    if (!packets)
        packets = drawHD_operatorNew(0x4C0);
    if (packets) {
        drawHD_constructArray(gabi::at<u8>(packets), 2, 0x254, kTexturePacketConstructor);
        gabi::at<DoubleTexturePacket>(packets)->mCurrent = u32(0);
        gabi::at<DoubleTexturePacket>(packets)->mVertexLayout = u32(0);
        gabi::at<DoubleTexturePacket>(packets)->mpMaterial = u32(0);
        gabi::at<DoubleTexturePacket>(packets)->mVertexStride = u32(0x98);
        gabi::at<DoubleTexturePacket>(packets)->mReady = u8(0);
        gabi::at<DoubleTexturePacket>(packets)->mPackets[0].mpVertices = u32(0);
        gabi::at<DoubleTexturePacket>(packets)->mPackets[1].mpVertices = u32(0);
    }
    drawHD_ShaderParamConstruct(&mirrorView->mShaderParam);
    drawHD_UniformBlockConstruct(&mirrorView->mUniformBlock);
    *gabi::at<be<u32>>(gabi::ea(&mirrorView->mUniformBlock[12])) = u32(kMirrorUniformDefinition);
    drawHD_bzero(gabi::at<u8>(mirror + 0x3020), 0x34);
    if (!(mirror + 0x3020))
        drawHD_operatorNew(48);
    drawHD_UniformBlockConstruct(&mirrorView->mViewUniformBlock);
    *gabi::at<be<u32>>(gabi::ea(&mirrorView->mViewUniformBlock[12])) =
        u32(kMirrorViewUniformDefinition);
    drawHD_bzero(&mirrorView->mUniformValues, 0x2F0);
    const f32 zero = l_uniformZero.get(), one = l_uniformOne.get();
    for (u32 i = 0; i < 44; i++)
        *gabi::at<be<f32>>((mirror) + (0x30C8 + i * 4)) = (i & 3) == 3 ? one : zero;
    drawHD_constructArray(&mirrorView->mLightParameters, 2, 16, kSmallPacketConstructor);
    drawHD_constructArray(&mirrorView->mSceneParameters, 2, 16, kSmallPacketConstructor);
    drawHD_constructArray(&mirrorView->mShadowParameters, 2, 16, kSmallPacketConstructor);
    for (u32 offset = 0x11C; offset <= 0x26C; offset += 0x30)
        if (!(mirror + 0x30BC + offset))
            drawHD_operatorNew(48);
    for (u32 offset = 0x290; offset <= 0x2E0; offset += 16)
        if (!(gabi::ea(&mirrorView->mUniformValues) + offset))
            drawHD_operatorNew(16);
    mirrorView->mEnabled = u8(0);
    drawHD_dBgSLinChkConstruct(&mirrorView->mLinChk);
    const u32 byteOffsets[] = {0x3419, 0x341D, 0x341C, 0x341E, 0x3418, 0x341A};
    for (u32 offset : byteOffsets)
        *gabi::at<be<u8>>((mirror) + (offset)) = u8(0);
    mirrorView->mPolyPassFlags[3] = u8(1);
    mirrorView->mPolyPassVtable = u32(kPolyPassVtable);
    mirrorView->mGrpPassMask = u32(31);
    *gabi::at<be<u32>>(gabi::ea(&mirrorView->mLinChk[0])) = gabi::ea(&mirrorView->mPolyPassVtable);
    *gabi::at<be<u32>>(gabi::ea(&mirrorView->mLinChk[4])) = gabi::ea(&mirrorView->mGrpPassVtable);
    mirrorView->mGrpPassVtable = u32(kGrpPassVtable);
    *gabi::at<be<u32>>(gabi::ea(&mirrorView->mLinChk[16])) = u32(kPolyInfoVtable);
    *gabi::at<be<u32>>(gabi::ea(&mirrorView->mLinChk[32])) = u32(kLinChkVtable);
    return mirror;
}

/* 0252D2A0 */
void dDlstHD::dDlst_mirrorPacket::drawMaterial(u32 packet) {
    const u32 mirror = gabi::ea(this);
    auto* mirrorView = this;
    auto* packetView = gabi::at<Packet>(packet);
    if (packetView->mKind.get() != 2)
        return;
    const u32 context = drawHD_getRenderContext(gabi::at<u8>(kJ3DSys));
    u32 material = mirrorView->mpMaterial.get();
    const u32 shape = gabi::at<MirrorMaterial>(material)->mpShape.get();
    if (shape != gabi::at<RenderContext>(context)->mpShape.get()) {
        const u32 shapeFlags = gabi::at<Shape>(shape)->mFlags.get(),
                  oldMaterial = gabi::at<RenderContext>(context)->mpMaterial.get();
        if (shapeFlags & 2) {
            gabi::at<Shape>(shape)->mFlags = u8(shapeFlags & ~2);
            drawHD_ShapeClearDirty(gabi::at<u8>(shape), 0);
        }
        const u32 currentMaterial =
            gabi::at<ShapeModel>(gabi::at<Shape>(shape)->mpModel.get())->mpMaterial.get();
        if (oldMaterial != currentMaterial)
            drawHD_MaterialBind(gabi::at<u8>(currentMaterial));
        const u32 size = gabi::at<Shape>(shape)->mDisplayListSize.get();
        if (size)
            drawHD_GX2CallDisplayList(gabi::at<u8>(gabi::at<Shape>(shape)->mpDisplayList.get()),
                                      size);
        else
            drawHD_ShapeBind(gabi::at<u8>(shape));
        gabi::at<RenderContext>(context)->mpMaterial = currentMaterial;
        gabi::at<RenderContext>(context)->mpShape = shape;
        material = mirrorView->mpMaterial.get();
    }
    drawHD_ShaderParamBindMaterial(&mirrorView->mShaderParam, gabi::at<u8>(material), 0);
    Local<u8[0x11C]> renderState;
    const u32 state = gabi::ea(renderState.get());
    drawHD_RenderStateConstruct(renderState.get());
    const u32 config = gabi::at<RenderState>(state)->mDepthConfig.get();
    gabi::at<RenderState>(state)->mCullMode = u8(0);
    gabi::at<RenderState>(state)->mDepthWrite = u8(0);
    gabi::at<RenderState>(state)->mDepthTest = u8(1);
    gabi::at<RenderState>(state)->mBlendMode = u32(0);
    gabi::at<RenderState>(state)->mDepthConfig = u32(((config & ~15u) + 7u & ~240u) + 16u);
    drawHD_RenderStateApply(renderState.get());
    material = mirrorView->mpMaterial.get();
    const u32 record = gabi::at<MirrorMaterial>(material)->mTextureCount.get()
                           ? gabi::at<MirrorMaterial>(material)->mpTextures.get()
                           : 0;
    drawHD_TextureBind(&mirrorView->mTexObj, gabi::at<u8>(record + 4), 0, 0);
    u32 resource = gabi::at<PacketModel>(packetView->mpModel.get())->mpResource.get();
    if (!l_textureGuard.get()) {
        l_textureGuard = 1;
        l_textureVtable = kTextureResourceVtable;
    }
    if (resource) {
        const u32 target =
            gabi::at<ResourceVtable>(gabi::at<Resource>(resource)->mVtable.get())->mIsValid.get();
        if (!gabi::call_ptr<u32>(target, gabi::at<u8>(resource)))
            resource = 0;
    }
    resource = gabi::at<Resource>(resource)->mpTexture.get();
    if (resource) {
        if (gabi::at<Texture>(resource)->mHasGpuBuffer.get()) {
            if (gabi::at<Texture>(resource)->mNeedsFlush.get()) {
                drawHD_TextureFlush(gabi::at<u8>(resource));
                gabi::at<Texture>(resource)->mNeedsFlush = u8(0);
            }
            drawHD_GX2Invalidate(gabi::at<u8>(resource + 0xA8));
            drawHD_GpuContextReset(gabi::at<u8>(mGpuContext.get()));
        }
        drawHD_TexturePrepare(gabi::at<u8>(resource));
        bindMirrorTexture(mirror, resource, gabi::ea(&mirrorView->mSceneTexture), 1);
    }
    resource = drawHD_TextureManagerGet(gabi::at<u8>(mTextureManager.get()), 3);
    if (resource)
        bindMirrorTexture(mirror, resource, gabi::ea(&mirrorView->mIndirectTexture), 2);
    const u32 selected = mirrorView->mPackets.mCurrent.get() ? 0 : 0x254;
    drawHD_VertexArrayBind(&mirrorView->mPackets.mPackets[selected / 0x254].mVertexArray);
    const u32 count = mDrawCount.get();
    if (count)
        drawHD_GX2DrawIndexedEx(mDrawPrimitive.get(), count, mDrawStart.get(),
                                mDrawVertexCount.get(), 0, 1);
}

/* 0252D6E8 */
void dDlstHD::dDlst_mirrorPacket::init(u32 texture) {
    const u32 mirror = gabi::ea(this);
    Local<u32[2]> name;
    if (!texture) {
        *gabi::at<be<u32>>((gabi::ea(name.get())) + (4)) = u32(kSafeStringVtable);
        *gabi::at<be<u32>>(gabi::ea(name.get())) = gabi::guest_addr("Always");
        texture = drawHD_getObjectResource(gabi::at<u8>(mResourceControl.get()), name.get(), 127);
    }
    drawHD_TextureSetResource(&this->mSourceTexture,
                              gabi::at<u8>((*gabi::at<be<u32>>((texture) + (32))).get()));
    const TextureMember members[] = {
        &Texture::mDimension, &Texture::mWidth,   &Texture::mHeight,
        &Texture::mDepth,     &Texture::mNumMips, &Texture::mSwizzle,
        &Texture::mTileMode,  &Texture::mPitch,   &Texture::mAlignment};
    bool same = true;
    for (TextureMember member : members) {
        if ((gabi::at<Texture>(mirror + 0x2614)->*member).get() !=
            (gabi::at<Texture>(mirror + 0x2584)->*member).get()) {
            same = false;
            break;
        }
    }
    if (!same)
        drawHD_TextureCopy(&this->mTexObj, &this->mSourceTexture);
    else {
        const u32 a = gabi::at<be<u32>>(gabi::ea(&this->mSourceTexture[40]))->get(),
                  b = gabi::at<be<u32>>(gabi::ea(&this->mSourceTexture[48]))->get();
        *gabi::at<be<u32>>(gabi::ea(&this->mTexObj[212])) = a;
        *gabi::at<be<u32>>(gabi::ea(&this->mTexObj[220])) = b;
        *gabi::at<be<u32>>(gabi::ea(&this->mTexObj[40])) = a;
        *gabi::at<be<u32>>(gabi::ea(&this->mTexObj[48])) = b;
    }
    *gabi::at<be<u32>>((gabi::ea(name.get())) + (4)) = u32(kSafeStringVtable);
    *gabi::at<be<u32>>(gabi::ea(name.get())) = gabi::guest_addr("mirror_sheild_draw");
    const u32 model = drawHD_getMirrorModel();
    const u32 index =
        drawHD_findMaterial(gabi::at<u8>(gabi::at<MirrorModel>(model)->mpData.get()), name.get());
    u32 material = 0;
    if (s32(index) >= 0) {
        u32 count = gabi::at<MirrorModel>(model)->mMaterialCount.get(),
            base = gabi::at<MirrorModel>(model)->mpMaterials.get();
        const u32 selected = index < count ? base + index * 36 : base;
        if (!gabi::at<MirrorMaterial>(selected)->mInitialized.get()) {
            const u32 data = gabi::at<MirrorModel>(model)->mpData.get(),
                      dataCount = gabi::at<MirrorModelData>(data)->mMaterialCount.get();
            const u32 initialization =
                index < dataCount ? gabi::at<MirrorModelData>(data)->mpMaterials.get() + index * 132
                                  : 0;
            drawHD_MaterialInit(gabi::at<u8>(selected), gabi::at<u8>(initialization), 0);
            count = gabi::at<MirrorModel>(model)->mMaterialCount.get();
            base = gabi::at<MirrorModel>(model)->mpMaterials.get();
        }
        material = index < count ? base + index * 36 : base;
    }
    this->mpMaterial = material;
    const u32 packets = gabi::ea(&this->mPackets);
    for (u32 offset = 0; offset < 0x4A8; offset += 0x254) {
        const u32 packet = packets + offset;
        u32 allocation = gabi::at<TexturePacket>(packet)->mpVertices.get();
        if (!allocation) {
            const u32 heap = drawHD_getAllocationHeap(gabi::at<u8>(mHeapManager.get()));
            const u32 target =
                gabi::at<HeapVtable>(gabi::at<Heap>(heap)->mVtable.get())->mAlloc.get();
            const u32 fresh = gabi::call_ptr<u32>(target, gabi::at<u8>(heap), 0x260, 64);
            if (fresh) {
                gabi::at<TexturePacket>(packet)->mpAllocation = fresh;
                gabi::at<TexturePacket>(packet)->mAlignment = u32(4);
            }
            allocation = gabi::at<TexturePacket>(packet)->mpAllocation.get();
            gabi::at<TexturePacket>(packet)->mpVertices = allocation;
        }
        drawHD_VertexBufferSetMirrorVertices(
            gabi::at<u8>(packet + 4), gabi::at<u8>(allocation), 4,
            &gabi::at<DoubleTexturePacket>(packets)->mVertexLayout);
        if (material && material != gabi::at<DoubleTexturePacket>(packets)->mpMaterial.get())
            drawHD_MaterialSetVertexArray(
                gabi::at<u8>(material), gabi::at<u8>(packet + 0x158), gabi::at<u8>(packet + 4),
                &gabi::at<DoubleTexturePacket>(packets)->mVertexLayout, 0);
    }
    gabi::at<DoubleTexturePacket>(packets)->mpMaterial = material;
    gabi::at<DoubleTexturePacket>(packets)->mReady = 1;
    drawHD_ShaderParamSetMode(&this->mShaderParam, 1, 0);
}

/* 0252D9C4 */
void copyMirrorMatrix(u32 destination, u32 source) {
    auto* sourceView = gabi::at<Matrix34>(source);
    f32 matrix[12];
    // Capture the complete matrix before writing, including overlapping matrices.
    matrix[5] = sourceView->m[5].get();
    matrix[4] = sourceView->m[4].get();
    matrix[6] = sourceView->m[6].get();
    matrix[7] = sourceView->m[7].get();
    matrix[8] = sourceView->m[8].get();
    matrix[9] = sourceView->m[9].get();
    matrix[11] = sourceView->m[11].get();
    matrix[0] = sourceView->m[0].get();
    matrix[3] = sourceView->m[3].get();
    matrix[1] = sourceView->m[1].get();
    matrix[10] = sourceView->m[10].get();
    matrix[2] = sourceView->m[2].get();
    for (u32 i = 0; i < 12; i++)
        gabi::at<Matrix34>(destination)->m[i] = matrix[i];
}

/* 0252DA64 */
void dDlstHD::dDlst_mirrorPacket::update(u32 matrix) {
    const u32 mirror = gabi::ea(this);
    auto* mirrorView = this;
    auto* matrixView = gabi::at<u8>(matrix);
    Local<MirrorFrame> scratch;

    const f32 a = l_mirrorLeft.get(), b = l_mirrorTop.get(), c = l_mirrorRight.get(),
              d = l_mirrorBottom.get();
    const f32 zero = l_zero.get(), extent = l_mirrorDepth.get();
    for (u32 i = 0; i < 8; i++) {
        const u32 v = gabi::ea(&scratch->mBounds[(i * 12) / 4]);
        *gabi::at<be<f32>>(v) = (i % 4 == 0 || i % 4 == 3) ? a : c;
        *gabi::at<be<f32>>((v) + (4)) = i % 4 < 2 ? b : d;
        *gabi::at<be<f32>>((v) + (8)) = i < 4 ? zero : extent;
    }
    for (u32 i = 0; i < 8; i++)
        drawHD_PSMTXMultVec(matrixView, &scratch->mBounds[(i * 12) / 4],
                            gabi::at<u8>((i < 4 ? gabi::ea(&scratch->mNearPoints[i * 3])
                                                : gabi::ea(&scratch->mFarPoints[(i - 4) * 3]))));
    f32 points[12];
    for (u32 i = 0; i < 4; i++) {
        drawHD_dBgSLinChkSet(&mirrorView->mLinChk, &scratch->mNearPoints[(i * 12) / 4],
                             &scratch->mFarPoints[(i * 12) / 4], 0);
        const u32 play = drawHD_getPlay();
        const bool hit =
            drawHD_dBgSLineCross(gabi::at<u8>(play + 0x12A0), &mirrorView->mLinChk) != 0;
        for (u32 j = 0; j < 3; j++) {
            points[i * 3 + j] =
                (*gabi::at<be<f32>>(
                     (hit ? mirror + 0x33EC : gabi::ea(&scratch->mFarPoints[(i * 12) / 4])) +
                     (j * 4)))
                    .get();
            *gabi::at<be<f32>>((gabi::ea(&scratch->mPoints[(i * 12) / 4])) + (j * 4)) =
                points[i * 3 + j];
        }
    }
    u32 selector = mirrorView->mPackets.mCurrent.get(),
        packetBase = gabi::ea(&mirrorView->mPackets);
    const u32 allocation = (*gabi::at<be<u32>>(packetBase + selector * 0x254)).get(),
              end = allocation + 0x260;
    if (allocation < end) {
        for (u32 cursor = allocation; cursor < end; cursor += 32)
            for (u32 j = 0; j < 8; j++)
                *gabi::at<be<u32>>((cursor & ~31u) + j * 4) = 0;
        for (u32 i = 0; i < 12; i++)
            points[i] = (scratch->mPoints[(i * 4) / 4]).get();
        selector = mirrorView->mPackets.mCurrent.get();
    }
    const u32 output = (*gabi::at<be<u32>>(packetBase + selector * 0x254)).get();
    const u32 offsets[] = {0x1C8, 0xA0, 0x134, 0, 4, 0x1CC, 0x138, 0x130, 0x1D0, 8, 0x9C, 0x98};
    const u32 indices[] = {9, 5, 7, 0, 1, 10, 8, 6, 11, 2, 4, 3};
    for (u32 i = 0; i < 12; i++)
        *gabi::at<be<f32>>((output) + (offsets[i])) = points[indices[i]];
    const u32 selected =
        packetBase + gabi::at<DoubleTexturePacket>(packetBase)->mCurrent.get() * 0x254;
    drawHD_VertexBufferFlush(
        gabi::at<u8>(selected + 4), 0,
        gabi::at<be<u32>>(gabi::ea(&gabi::at<TexturePacket>(selected)->mVertexBuffer[332]))->get());
    gabi::at<DoubleTexturePacket>(packetBase)->mCurrent =
        u32(gabi::at<DoubleTexturePacket>(packetBase)->mCurrent.get() == 0);
    const u32 renderer = kJ3DSys;
    for (u32 i = 0; i < 12; i++)
        *gabi::at<be<f32>>((gabi::ea(&scratch->mViewMtx[0])) + (i * 4)) =
            (*gabi::at<be<f32>>((renderer) + (0x38 + i * 4))).get();
    for (u32 i = 0; i < 16; i++)
        *gabi::at<be<u32>>((gabi::ea(&scratch->mProjectionWords[0])) + (i * 4)) =
            (*gabi::at<be<u32>>((renderer) + (0x14C + i * 4))).get();
    drawHD_ShaderParamSetCamera(&mirrorView->mShaderParam, 0, &scratch->mViewMtx[0],
                                &scratch->mProjectionWords[0],
                                gabi::at<u8>(gabi::at<Renderer>(renderer)->mpCamera.get() + 0x240));
    const f32 nearZ =
        (*gabi::at<be<f32>>(((*gabi::at<be<u32>>((drawHD_getPlay()) + (0x5FA4))).get()) + (0xCC)))
            .get();
    const f32 farZ =
        (*gabi::at<be<f32>>(((*gabi::at<be<u32>>((drawHD_getPlay()) + (0x5FA4))).get()) + (0xD0)))
            .get();
    const f32 one = l_one.get(), half = l_half.get();
    mirrorView->mUniformValues[42] = nearZ;
    mirrorView->mUniformValues[41] = f32(one - f32(nearZ / farZ));
    mirrorView->mUniformValues[43] = farZ;
    mirrorView->mUniformValues[40] = f32(nearZ / f32(farZ - nearZ));
    const f32 fov =
        (*gabi::at<be<f32>>(((*gabi::at<be<u32>>((drawHD_getPlay()) + (0x5FA4))).get()) + (0xD4)))
            .get();
    const f64 tangent = drawHD_tan(f32(f32(fov * half) * l_degreesToRadians.get()));
    const f32 aspect =
        (*gabi::at<be<f32>>(((*gabi::at<be<u32>>((drawHD_getPlay()) + (0x5FA4))).get()) + (0xD8)))
            .get();
    mirrorView->mUniformValues[38] = zero;
    mirrorView->mUniformValues[37] = f32(tangent);
    mirrorView->mUniformValues[39] = zero;
    mirrorView->mUniformValues[36] = f32(tangent * f64(aspect));
    if (!l_mirrorEyeGuard.get()) {
        l_mirrorEyeGuard = 1;
        l_mirrorEye.x = l_mirrorEyeX.get();
        l_mirrorEye.z = zero;
        l_mirrorEye.y = l_mirrorEyeY.get();
    }
    if (!l_mirrorTargetGuard.get()) {
        l_mirrorTargetGuard = 1;
        l_mirrorTarget.z = l_mirrorFar.get();
        l_mirrorTarget.x = l_mirrorEyeX.get();
        l_mirrorTarget.y = l_mirrorEyeY.get();
    }
    if (!l_mirrorMatrixGuard.get()) {
        const f32 values[] = {half, zero, half, zero, zero, l_mirrorScaleY.get(),
                              half, zero, zero, zero, one,  zero};
        for (u32 i = 0; i < 12; i++)
            l_mirrorTextureMtx.m[i] = values[i];
        l_mirrorMatrixGuard = 1;
    }
    drawHD_PSMTXMultVec(matrixView, &l_mirrorEye, &scratch->mWorldEye[0]);
    drawHD_PSMTXMultVec(matrixView, &l_mirrorTarget, &scratch->mWorldTarget[0]);
    drawHD_PSMTXInverse(gabi::at<u8>(renderer + 0x38), &scratch->mInverseViewMtx[0]);
    drawHD_mDoMTXLookAt(&scratch->mLightViewMtx[0], &scratch->mWorldEye[0],
                        &scratch->mWorldTarget[0], 0);
    drawHD_C_MTXLightOrtho(&scratch->mLightProjection[0], l_mirrorOrthoTop.get(),
                           l_mirrorOrthoBottom.get(), l_mirrorOrthoBottom.get(),
                           l_mirrorOrthoTop.get(), one, l_mirrorFar.get());
    drawHD_PSMTXConcat(&scratch->mLightViewMtx[0], &scratch->mInverseViewMtx[0],
                       &mirrorView->mLightMtx);
    drawHD_mDoMTXConcatProjView(&scratch->mLightProjection[0], &mirrorView->mLightMtx,
                                &mirrorView->mTexMtx);
    for (u32 i = 0; i < 4; i++)
        *gabi::at<be<f32>>((mirror) + (0xE8 + i * 4)) =
            (*gabi::at<be<f32>>((mirror) + (0xF8 + i * 4))).get();
    for (u32 i = 0; i < 12; i++)
        *gabi::at<be<f32>>((gabi::ea(&scratch->mViewProjection[0])) + (i * 4)) =
            (*gabi::at<be<f32>>((renderer) + (0x14C + i * 4))).get();
    drawHD_PSMTXCopy(&scratch->mViewProjection[0], &mirrorView->mProjectionMtx);
    drawHD_copyMirrorMatrixEntry(&scratch->mTextureMtx[0], &mirrorView->mTexMtx);
    drawHD_PSMTXConcat(&l_mirrorTextureMtx, &scratch->mTextureMtx[0], &mirrorView->mShadowMtx);
    drawHD_PSMTXMultVec(gabi::at<u8>(renderer + 0x38), &scratch->mWorldEye[0],
                        &scratch->mViewEye[0]);
    drawHD_PSMTXMultVec(gabi::at<u8>(renderer + 0x38), &scratch->mWorldTarget[0],
                        &scratch->mViewTarget[0]);
    for (u32 i = 0; i < 3; i++)
        *gabi::at<be<f32>>((mirror) + (0x3358 + i * 4)) =
            (*gabi::at<be<f32>>((gabi::ea(&scratch->mViewEye[0])) + (i * 4))).get();
    mirrorView->mEyePosition[3] = one;
    for (u32 i = 0; i < 3; i++)
        *gabi::at<be<f32>>((gabi::ea(&scratch->mDirection[0])) + (i * 4)) =
            f32((*gabi::at<be<f32>>((gabi::ea(&scratch->mViewTarget[0])) + (i * 4))).get() -
                (*gabi::at<be<f32>>((gabi::ea(&scratch->mViewEye[0])) + (i * 4))).get());
    drawHD_normalizeVector(&scratch->mDirection[0]);
    for (u32 i = 0; i < 3; i++)
        *gabi::at<be<f32>>((mirror) + (0x3368 + i * 4)) =
            (*gabi::at<be<f32>>((gabi::ea(&scratch->mDirection[0])) + (i * 4))).get();
    mirrorView->mEyeDirection[3] = zero;
    drawHD_ShaderParamCommit(&mirrorView->mShaderParam, 0);
}

/* 0252E34C */
u32 dDlstHD::dDlst_peekZ_c::newData(u32 x, u32 y, u32 output) {
    const u32 queue = gabi::ea(this);
    auto* queueView = this;
    u8 count = queueView->mCount.get();
    if (count >= 64)
        return 0;
    u32 entry = queue + (u32)count * 8 + 4;
    *gabi::at<be<u16>>(gabi::ea(&gabi::at<dDlst_peekZ_entry>(entry)->mX)) = x;
    *gabi::at<be<u16>>(gabi::ea(&gabi::at<dDlst_peekZ_entry>(entry)->mY)) = y;
    gabi::at<dDlst_peekZ_entry>(entry)->mpDst = output;
    count = queueView->mCount.get();
    queueView->mCount = count + 1;
    return 1;
}

/* 0252E388 */
void dDlstHD::dDlst_peekZ_c::peekData() {
    const u32 queue = gabi::ea(this);
    auto* queueView = this;
    Local<DepthFrame> scratch;

    const u32 manager = mDepthManager.get(),
              slot = gabi::at<DepthManager>(manager)->mpBuffers.get() +
                     (gabi::at<DepthManager>(manager)->mBufferCount.get() > 1 ? 4 : 0);
    // HD: CPU depth reads use the selected GX2 surface and its packed channel format.
    const u32 texture = (*gabi::at<be<u32>>(slot)).get() + 0x6B3C;
    if (gabi::at<Texture>(texture)->mFormat.get() != 58)
        return;
    const u32 width = s32(gabi::at<Texture>(texture)->mWidth.get()) >
                              s32(gabi::at<Texture>(texture)->mMinWidth.get())
                          ? gabi::at<Texture>(texture)->mWidth.get()
                          : gabi::at<Texture>(texture)->mMinWidth.get();
    const u32 height = s32(gabi::at<Texture>(texture)->mHeight.get()) >
                               s32(gabi::at<Texture>(texture)->mMinHeight.get())
                           ? gabi::at<Texture>(texture)->mHeight.get()
                           : gabi::at<Texture>(texture)->mMinHeight.get();
    const f32 xscale = f32(f32(width) / l_depthWidth.get()),
              yscale = f32(f32(height) / l_depthHeight.get());
    const u32 data = gabi::at<Texture>(texture)->mpBuffer.get();
    if (!queueView->mCount.get()) {
        queueView->mCount = u8(0);
        return;
    }
    const f32 one = l_one.get(), zero = l_zero.get(), multiplier = l_depthMultiplier.get();
    const f32 split = l_unsignedSplit.get(), xpacking = l_depthXPack.get(),
              ypacking = l_depthYPack.get();
    for (u32 i = 0; i < queueView->mCount.get(); i++) {
        const u32 entry = queue + 4 + i * 8;
        const u16 x = u16(
            ftoi(f32(f32(f32(gabi::at<dDlst_peekZ_entry>(entry)->mX.get()) * xscale) * xpacking)));
        const u16 y = u16(ftoi(f32(f32(gabi::at<dDlst_peekZ_entry>(entry)->mY.get()) * yscale)));
        const u32 bytes = drawHD_TextureFormatBytes(58), row = u16(ftoi(f32(f32(y) * ypacking)));
        u32 pixel = data + (((width << 4) * row + (y & 15) + x) * bytes), accumulated = 0;
        for (u32 j = 0; j < 4; j++) {
            if (drawHD_TextureFormatChannels(58) <= s32(j)) {
                scratch->mChannelWords[(j * 4) / 4] = 0;
                continue;
            }
            const u32 channel = drawHD_TextureFormatChannel(58, j),
                      bits = drawHD_TextureFormatChannelBits(58, channel);
            const u32 target = mDepthReadPixel.get(),
                      value = gabi::call_ptr<u32>(target, (*gabi::at<be<u32>>(pixel)).get());
            *gabi::at<be<u32>>(
                gabi::ea(&scratch->mChannelWords[((channel < 3 ? channel : 3) * 4) / 4])) = value;
            accumulated += bits;
            if (accumulated == 32) {
                pixel += 4;
                accumulated = 0;
            }
        }
        for (u32 j = 0; j < 4; j++) {
            if (drawHD_TextureFormatChannels(58) <= s32(j))
                scratch->mChannelValues[(j * 4) / 4] =
                    drawHD_TextureFormatChannelType(58, j) == 5 ? one : zero;
            else if (drawHD_TextureFormatChannelBits(58, j) == 32) {
                const u32 word = (scratch->mChannelWords[(j * 4) / 4]).get();
                scratch->mFloatBits = word;
                scratch->mChannelValues[(j * 4) / 4] =
                    (*gabi::at<be<f32>>(gabi::ea(&scratch->mFloatBits))).get();
            }
        }
        const f32 value = f32(multiplier * (scratch->mChannelValues[0]).get());
        const u32 depth =
            value < split ? u32(ftoi(value)) : u32(ftoi(f32(value - split))) + 0x80000000u;
        *gabi::at<be<u32>>(gabi::at<dDlst_peekZ_entry>(entry)->mpDst.get()) = depth;
        const s32 currentX = gabi::at<dDlst_peekZ_entry>(entry)->mX.get(),
                  currentY = gabi::at<dDlst_peekZ_entry>(entry)->mY.get();
        const u16 px = u16(ftoi(f32(f32(currentX) * xscale))),
                  py = u16(ftoi(f32(f32(currentY) * yscale)));
        cpu->cr[6] = 1;
        drawHD_formattedTextEntry(
            &scratch->mText, gabi::at<u8>(kDepthDiagnosticFormat), i,
            (*gabi::at<be<u32>>(gabi::at<dDlst_peekZ_entry>(entry)->mpDst.get())).get(), u32(px),
            u32(py), 0x43300000u, u32(currentX) ^ 0x80000000u, (scratch->mChannelValues[0]).get(),
            (scratch->mChannelValues[1]).get(), (scratch->mChannelValues[2]).get());
    }
    queueView->mCount = u8(0);
}

/* 0252E9CC */
u32 dDlst_list_construct(u32 lists) {
    if (!lists)
        lists = drawHD_operatorNew(0x5C0);
    if (!lists)
        return 0;
    auto* listsView = gabi::at<dDlstHD::dDlst_list_c>(lists);
    const f32 zero = l_zero.get();
    const BufferMember before[] = {
        &dDlstHD::dDlst_list_c::mpListSnapshot,       &dDlstHD::dDlst_list_c::mpOpaListP1,
        &dDlstHD::dDlst_list_c::mpListDisableBloom,   &dDlstHD::dDlst_list_c::mpOpaListMaskOff,
        &dDlstHD::dDlst_list_c::mpXluListP1,          &dDlstHD::dDlst_list_c::mpOpaList,
        &dDlstHD::dDlst_list_c::mpOpaListPlayerExtra, &dDlstHD::dDlst_list_c::mpOpaListFilter,
        &dDlstHD::dDlst_list_c::mpOpaList2D,          &dDlstHD::dDlst_list_c::mpOpaListP0,
        &dDlstHD::dDlst_list_c::mpOpaListInvisible,   &dDlstHD::dDlst_list_c::mpXluList,
        &dDlstHD::dDlst_list_c::mpXluListInvisible,   &dDlstHD::dDlst_list_c::mpXluListMaskOff,
        &dDlstHD::dDlst_list_c::mpListShadowOn,       &dDlstHD::dDlst_list_c::mpListSea,
        &dDlstHD::dDlst_list_c::mpXluListBG,          &dDlstHD::dDlst_list_c::mpOpaListZelda1,
        &dDlstHD::dDlst_list_c::mpOpaListSky,         &dDlstHD::dDlst_list_c::mpListCloud,
        &dDlstHD::dDlst_list_c::mpXluListZelda1,      &dDlstHD::dDlst_list_c::mpOpaListZelda0,
        &dDlstHD::dDlst_list_c::mpXluListSky,         &dDlstHD::dDlst_list_c::mpOpaListBG};
    for (BufferMember member : before)
        (listsView->*member) = u32(0);
    listsView->mXOrig = zero;
    listsView->mYOrig = zero;
    listsView->mWidth = zero;
    listsView->mHeight = zero;
    listsView->mNearZ = zero;
    listsView->mFarZ = zero;
    listsView->mHasWindow = 0;
    drawHD_bzero(&listsView->mp2DCopyList, 16);
    listsView->mp2DCopyListCur = u32(0);
    listsView->mp2DCopyListEnd = u32(0);
    drawHD_bzero(&listsView->mp2DOpaTopList, 64);
    listsView->mp2DOpaTopListCur = u32(0);
    listsView->mp2DOpaTopListEnd = u32(0);
    drawHD_bzero(&listsView->mp2DOpaList, 256);
    listsView->mp2DOpaListCur = u32(0);
    listsView->mp2DOpaListEnd = u32(0);
    drawHD_bzero(&listsView->mp2DXluList, 128);
    listsView->mp2DXluListCur = 0;
    listsView->mpProjection = 0;
    listsView->mpViewPort = 0;
    listsView->mpWindow = 0;
    listsView->mp2DXluListEnd = 0;
    listsView->mpRenderTarget = 0;
    listsView->mpView = 0;
    listsView->mpCamera = 0;
    drawHD_constructArray(&listsView->m3DlineMatSortPacket, 2, 0x9C, kLineMatSortConstructor);
    const BufferMember after[] = {&dDlstHD::dDlst_list_c::mpOpaListInvisible,
                                  &dDlstHD::dDlst_list_c::mpXluListZelda1,
                                  &dDlstHD::dDlst_list_c::mpXluListP1,
                                  &dDlstHD::dDlst_list_c::mpListDisableBloom,
                                  &dDlstHD::dDlst_list_c::mpOpaListP0,
                                  &dDlstHD::dDlst_list_c::mpOpaListZelda1,
                                  &dDlstHD::dDlst_list_c::mpListShadowOn,
                                  &dDlstHD::dDlst_list_c::mpXluListBG,
                                  &dDlstHD::dDlst_list_c::mpOpaListMaskOff,
                                  &dDlstHD::dDlst_list_c::mpListCloud,
                                  &dDlstHD::dDlst_list_c::mpXluList,
                                  &dDlstHD::dDlst_list_c::mpOpaListSky,
                                  &dDlstHD::dDlst_list_c::mpXluListMaskOff,
                                  &dDlstHD::dDlst_list_c::mpListSnapshot,
                                  &dDlstHD::dDlst_list_c::mpOpaListPlayerExtra,
                                  &dDlstHD::dDlst_list_c::mpXluListSky,
                                  &dDlstHD::dDlst_list_c::mpListSea,
                                  &dDlstHD::dDlst_list_c::mpOpaListFilter,
                                  &dDlstHD::dDlst_list_c::mpOpaListZelda0,
                                  &dDlstHD::dDlst_list_c::mpOpaList2D,
                                  &dDlstHD::dDlst_list_c::mpOpaListP1,
                                  &dDlstHD::dDlst_list_c::mpOpaListBG,
                                  &dDlstHD::dDlst_list_c::mpXluListInvisible,
                                  &dDlstHD::dDlst_list_c::mpOpaList};
    for (BufferMember member : after)
        (listsView->*member) = u32(0);
    listsView->mp2DXluListEnd = gabi::ea(&listsView->mp2DXluListCur);
    listsView->mp2DOpaListEnd = gabi::ea(&listsView->mp2DOpaListCur);
    listsView->mp2DCopyListEnd = gabi::ea(&listsView->mp2DCopyListCur);
    listsView->mPeekZ.mCount = 0;
    listsView->mp2DOpaTopListEnd = gabi::ea(&listsView->mp2DOpaTopListCur);
    return lists;
}

/* 0252EBA0 */
void dDlst_list_destruct(u32 lists, u32 flags) {
    auto* listsView = gabi::at<dDlstHD::dDlst_list_c>(lists);
    if (!lists)
        return;
    const BufferMember offsets[] = {&dDlstHD::dDlst_list_c::mpOpaListSky,
                                    &dDlstHD::dDlst_list_c::mpXluListSky,
                                    &dDlstHD::dDlst_list_c::mpOpaListP0,
                                    &dDlstHD::dDlst_list_c::mpOpaListP1,
                                    &dDlstHD::dDlst_list_c::mpXluListP1,
                                    &dDlstHD::dDlst_list_c::mpOpaListZelda0,
                                    &dDlstHD::dDlst_list_c::mpOpaListZelda1,
                                    &dDlstHD::dDlst_list_c::mpXluListZelda1,
                                    &dDlstHD::dDlst_list_c::mpOpaListPlayerExtra,
                                    &dDlstHD::dDlst_list_c::mpOpaListBG,
                                    &dDlstHD::dDlst_list_c::mpXluListBG,
                                    &dDlstHD::dDlst_list_c::mpOpaList,
                                    &dDlstHD::dDlst_list_c::mpXluList,
                                    &dDlstHD::dDlst_list_c::mpOpaListFilter,
                                    &dDlstHD::dDlst_list_c::mpOpaListMaskOff,
                                    &dDlstHD::dDlst_list_c::mpXluListMaskOff,
                                    &dDlstHD::dDlst_list_c::mpOpaListInvisible,
                                    &dDlstHD::dDlst_list_c::mpXluListInvisible,
                                    &dDlstHD::dDlst_list_c::mpListShadowOn,
                                    &dDlstHD::dDlst_list_c::mpListSnapshot,
                                    &dDlstHD::dDlst_list_c::mpOpaList2D,
                                    &dDlstHD::dDlst_list_c::mpListSea,
                                    &dDlstHD::dDlst_list_c::mpListCloud,
                                    &dDlstHD::dDlst_list_c::mpListDisableBloom};
    for (BufferMember member : offsets)
        drawHD_J3DDrawBufferDestruct(gabi::at<u8>((listsView->*member).get()), 3);
    drawHD_destroyArray(&listsView->m3DlineMatSortPacket, 2, 0x9C, kPacketDestructor, 0, 0);
    if (flags & 1)
        drawHD_operatorDelete(gabi::at<u8>(lists));
}

/* 0252ED28 */
u32 dDlstHD::dDlst_list_c::init() {
    const u32 lists = gabi::ea(this);
    auto* listsView = this;
    const Allocation allocations[] = {
        {&dDlstHD::dDlst_list_c::mpOpaListSky, 32, "VRBOX"},
        {&dDlstHD::dDlst_list_c::mpXluListSky, 32, "VRBOX"},
        {&dDlstHD::dDlst_list_c::mpOpaListP0, 1, "PLAYER_1"},
        {&dDlstHD::dDlst_list_c::mpOpaListP1, 32, "PLAYER_2"},
        {&dDlstHD::dDlst_list_c::mpXluListP1, 32, "PLAYER_3"},
        {&dDlstHD::dDlst_list_c::mpOpaListZelda0, 1, "Zelda_1"},
        {&dDlstHD::dDlst_list_c::mpOpaListZelda1, 32, "Zelda_2"},
        {&dDlstHD::dDlst_list_c::mpXluListZelda1, 32, "Zelda_3"},
        {&dDlstHD::dDlst_list_c::mpOpaListPlayerExtra, 32, "PLAYER_2"},
        {&dDlstHD::dDlst_list_c::mpOpaListBG, 128, "BG_OPA"},
        {&dDlstHD::dDlst_list_c::mpXluListBG, 128, "BG_XLU"},
        {&dDlstHD::dDlst_list_c::mpOpaList, 256, "OBJECT_OPA"},
        {&dDlstHD::dDlst_list_c::mpXluList, 256, "OBJECT_XLU"},
        {&dDlstHD::dDlst_list_c::mpOpaListFilter, 32, "FILTER"},
        {&dDlstHD::dDlst_list_c::mpOpaListMaskOff, 32, "MASKOFF"},
        {&dDlstHD::dDlst_list_c::mpXluListMaskOff, 256, "MASKOFF"},
        {&dDlstHD::dDlst_list_c::mpOpaListInvisible, 32, "INVISIBLE"},
        {&dDlstHD::dDlst_list_c::mpXluListInvisible, 32, "INVISIBLE"},
        {&dDlstHD::dDlst_list_c::mpListShadowOn, 32, "SHADOW_ON"},
        {&dDlstHD::dDlst_list_c::mpListSnapshot, 1, "SNAP"},
        {&dDlstHD::dDlst_list_c::mpOpaList2D, 1, "2D"},
        {&dDlstHD::dDlst_list_c::mpListSea, 1, "SEA"},
        {&dDlstHD::dDlst_list_c::mpListCloud, 1, "KUMO"},
        {&dDlstHD::dDlst_list_c::mpListDisableBloom, 32, "DISABLE_BLOOM"}};
    Local<u32[2]> name;
    u32 last = 0;
    for (const Allocation& allocation : allocations) {
        const u32 local = gabi::ea(name.get());
        gabi::at<SafeString>(local)->mVtable = u32(kSafeStringVtable);
        gabi::at<SafeString>(local)->mpText = gabi::guest_addr(allocation.name);
        last = drawHD_newDrawBuffer(allocation.capacity, name.get());
        (listsView->*allocation.member) = last;
    }
    const u32 first = listsView->mpOpaListSky.get();
    const BufferMember checks[] = {
        &dDlstHD::dDlst_list_c::mpXluListSky,       &dDlstHD::dDlst_list_c::mpOpaListP0,
        &dDlstHD::dDlst_list_c::mpOpaListP1,        &dDlstHD::dDlst_list_c::mpXluListP1,
        &dDlstHD::dDlst_list_c::mpOpaListZelda0,    &dDlstHD::dDlst_list_c::mpOpaListZelda1,
        &dDlstHD::dDlst_list_c::mpXluListZelda1,    &dDlstHD::dDlst_list_c::mpOpaListPlayerExtra,
        &dDlstHD::dDlst_list_c::mpOpaListBG,        &dDlstHD::dDlst_list_c::mpXluListBG,
        &dDlstHD::dDlst_list_c::mpOpaList,          &dDlstHD::dDlst_list_c::mpXluList,
        &dDlstHD::dDlst_list_c::mpOpaListFilter,    &dDlstHD::dDlst_list_c::mpOpaListMaskOff,
        &dDlstHD::dDlst_list_c::mpXluListMaskOff,   &dDlstHD::dDlst_list_c::mpOpaListInvisible,
        &dDlstHD::dDlst_list_c::mpXluListInvisible, &dDlstHD::dDlst_list_c::mpListShadowOn,
        &dDlstHD::dDlst_list_c::mpListSnapshot,     &dDlstHD::dDlst_list_c::mpOpaList2D,
        &dDlstHD::dDlst_list_c::mpListSea,          &dDlstHD::dDlst_list_c::mpListCloud};
    if (!first)
        return 0;
    for (BufferMember member : checks)
        if (!(listsView->*member).get())
            return 0;
    if (!last)
        return 0;
    gabi::at<DrawBuffer>(first)->mDrawType = NonSort;
    const BufferMember five[] = {
        &dDlstHD::dDlst_list_c::mpXluListSky,    &dDlstHD::dDlst_list_c::mpOpaListP0,
        &dDlstHD::dDlst_list_c::mpOpaListP1,     &dDlstHD::dDlst_list_c::mpXluListP1,
        &dDlstHD::dDlst_list_c::mpOpaListZelda0, &dDlstHD::dDlst_list_c::mpOpaListZelda1,
        &dDlstHD::dDlst_list_c::mpXluListZelda1, &dDlstHD::dDlst_list_c::mpOpaListPlayerExtra,
        &dDlstHD::dDlst_list_c::mpXluListBG};
    for (BufferMember member : five)
        gabi::at<DrawBuffer>((listsView->*member).get())->mDrawType = NonSort;
    const BufferMember two[] = {
        &dDlstHD::dDlst_list_c::mpXluList,        &dDlstHD::dDlst_list_c::mpOpaListFilter,
        &dDlstHD::dDlst_list_c::mpXluListMaskOff, &dDlstHD::dDlst_list_c::mpXluListInvisible,
        &dDlstHD::dDlst_list_c::mpListShadowOn,   &dDlstHD::dDlst_list_c::mpListSnapshot};
    for (BufferMember member : two)
        gabi::at<DrawBuffer>((listsView->*member).get())->mDrawType = ZSort;
    const BufferMember finalFive[] = {
        &dDlstHD::dDlst_list_c::mpOpaList2D, &dDlstHD::dDlst_list_c::mpListSea,
        &dDlstHD::dDlst_list_c::mpListCloud, &dDlstHD::dDlst_list_c::mpListDisableBloom};
    for (BufferMember member : finalFive)
        gabi::at<DrawBuffer>((listsView->*member).get())->mDrawType = NonSort;
    mOpaDrawBuffer = listsView->mpOpaList.get();
    mXluDrawBuffer = listsView->mpXluList.get();
    listsView->mp2DCopyListCur = gabi::ea(&listsView->mp2DCopyList);
    listsView->mp2DXluListCur = gabi::ea(&listsView->mp2DXluList);
    listsView->mp2DOpaListCur = gabi::ea(&listsView->mp2DOpaList);
    listsView->mp2DOpaTopListCur = gabi::ea(&listsView->mp2DOpaTopList);
    return 1;
}

/* 0252F264 */
void dDlstHD::dDlst_list_c::reset() {
    const u32 lists = gabi::ea(this);
    auto* listsView = this;
    const BufferMember offsets[] = {&dDlstHD::dDlst_list_c::mpOpaListSky,
                                    &dDlstHD::dDlst_list_c::mpXluListSky,
                                    &dDlstHD::dDlst_list_c::mpOpaListP0,
                                    &dDlstHD::dDlst_list_c::mpOpaListP1,
                                    &dDlstHD::dDlst_list_c::mpXluListP1,
                                    &dDlstHD::dDlst_list_c::mpOpaListZelda0,
                                    &dDlstHD::dDlst_list_c::mpOpaListZelda1,
                                    &dDlstHD::dDlst_list_c::mpXluListZelda1,
                                    &dDlstHD::dDlst_list_c::mpOpaListPlayerExtra,
                                    &dDlstHD::dDlst_list_c::mpOpaListBG,
                                    &dDlstHD::dDlst_list_c::mpXluListBG,
                                    &dDlstHD::dDlst_list_c::mpOpaList,
                                    &dDlstHD::dDlst_list_c::mpXluList,
                                    &dDlstHD::dDlst_list_c::mpOpaListFilter,
                                    &dDlstHD::dDlst_list_c::mpOpaListMaskOff,
                                    &dDlstHD::dDlst_list_c::mpXluListMaskOff,
                                    &dDlstHD::dDlst_list_c::mpOpaListInvisible,
                                    &dDlstHD::dDlst_list_c::mpXluListInvisible,
                                    &dDlstHD::dDlst_list_c::mpListShadowOn,
                                    &dDlstHD::dDlst_list_c::mpListSnapshot,
                                    &dDlstHD::dDlst_list_c::mpOpaList2D,
                                    &dDlstHD::dDlst_list_c::mpListSea,
                                    &dDlstHD::dDlst_list_c::mpListCloud,
                                    &dDlstHD::dDlst_list_c::mpListDisableBloom};
    for (BufferMember member : offsets) {
        u32 buffer = (listsView->*member).get();
        drawHD_J3DDrawBufferFrameInit(gabi::at<u8>(buffer));
    }
    mOpaDrawBuffer = listsView->mpOpaList.get();
    mXluDrawBuffer = listsView->mpXluList.get();
    listsView->mp2DCopyListCur = gabi::ea(&listsView->mp2DCopyList);
    listsView->mp2DOpaTopListCur = gabi::ea(&listsView->mp2DOpaTopList);
    listsView->mp2DOpaListCur = gabi::ea(&listsView->mp2DOpaList);
    listsView->mp2DXluListCur = gabi::ea(&listsView->mp2DXluList);
    drawHD_lineMatSortReset(&listsView->m3DlineMatSortPacket);
    drawHD_lineMatSortReset(gabi::at<u8>(lists + 0x320));
}

/* 0252F3B0 */
void dDlstHD::dDlst_list_c::entryZSortXluDrawList(u32 buffer, u32 packet, u32 position) {
    auto* positionView = gabi::at<VectorWords>(position);
    Local<DrawVector> snapshot;
    const u32 x = positionView->x.get(), z = positionView->z.get(), y = positionView->y.get();
    const u32 local = gabi::ea(snapshot.get());
    gabi::at<VectorWords>(local)->x = x;
    gabi::at<VectorWords>(local)->y = y;
    gabi::at<VectorWords>(local)->z = z;
    // HD: retain the unrounded double depth through the bucket comparisons.
    const double depth = -drawHD_PSMTXMultVecZ(gabi::at<u8>(kViewMtx), snapshot.get());
    u32 bucket = 0;
    if (l_zSortNear.get() < depth) {
        bucket = 255;
        if (l_zSortFar.get() > depth) {
            const f32 quotient = f32(depth / l_zSortStep.get());
            bucket = u32(ftoi(quotient)) & 0xFFFF;
        }
    }
    drawHD_J3DDrawBufferEntryImm(gabi::at<u8>(buffer), gabi::at<u8>(packet),
                                 (255 - bucket) & 0xFFFF);
}

/* 0252F480 */
void dDlstHD::dDlst_list_c::draw(u32 begin, u32 end) {
    for (u32 cursor = begin; cursor < end; cursor += 4) {
        u32 packet = (*gabi::at<be<u32>>(cursor)).get(),
            table = gabi::at<TexturePacket>(packet)->mpVertices.get(),
            draw = gabi::at<DrawVtable>(table)->mDraw.get();
        gabi::call_ptr(draw, gabi::at<u8>(packet));
    }
}

/* 0252F4E0 */
void compareProjectedDepth(u32 pair) {
    Local<DrawVector> first, second;
    drawHD_PSMTXMultVec(gabi::at<u8>(kViewMtx), gabi::at<u8>(pair), first.get());
    drawHD_PSMTXMultVec(gabi::at<u8>(kViewMtx), gabi::at<u8>(pair + 12), second.get());
    f32 b = second->z, a = first->z;
    gabi::at<DepthPair>(pair)->mSecondIsNearer = b < a;
}

/* 0252F54C */
// HD: wipe state is updated without the GameCube wipe texture/draw path.
void dDlst_list_wipeIn(u32 colour, f32 depth) {
    f32 positive = l_one.get();
    mWipe = 1;
    f32 negative = l_zero.get();
    mWipeSpeed = depth;
    u32 packed = (*gabi::at<be<u32>>(colour)).get();
    f32 direction = depth >= 0 ? negative : positive;
    mWipeColor = packed;
    mWipeRate = direction;
}

/* 0252F590 */
void dDlst_list_wipeOut(f32 depth) { drawHD_wipeInEntry(gabi::at<u8>(kDefaultWipeColor), -depth); }

/* 0252F5A0 */
// HD: only the fade state advances; no wipe packet is enqueued.
void dDlst_list_calcWipe() {
    if (!mWipe.get())
        return;
    f32 value = mWipeRate.get() + mWipeSpeed.get(), minimum = l_zero.get();
    if (value < minimum) {
        mWipeRate = minimum;
        return;
    }
    f32 maximum = l_one.get();
    if (!(value > maximum)) {
        mWipeRate = value;
        return;
    }
    mWipeRate = maximum;
    mWipe = 0;
}

// HD: compiler-generated and template code of this translation unit.

/* 0252F6BC */
void dDlstHD::BufferedText::assureTermination() {
    const u32 text = gabi::ea(this);
    auto* textView = this;
    u32 buffer = textView->mpBuffer.get(), size = textView->mBufferSize.get();
    *gabi::at<be<u8>>(buffer + size - 1) = 0;
}

/* 0252F6D4 */
void destroyText(u32 object, u32 flags) {
    if (object && (flags & 1))
        drawHD_operatorDeleteWithFlags(gabi::at<u8>(object), flags);
}

/* 0252F6E8 */
void destroyPacket(u32 object, u32 flags) {
    auto* objectView = gabi::at<u8>(object);
    if (!object)
        return;
    drawHD_J3DPacketDestruct(objectView, 0);
    if (flags & 1)
        drawHD_operatorDelete(objectView);
}

/* 0252F74C */
u32 constructTexturePacket(u32 object) {
    if (!object)
        object = drawHD_operatorNew(0x254);
    if (object) {
        auto* objectView = gabi::at<TexturePacket>(object);
        drawHD_VertexBufferConstruct(gabi::at<u8>(object + 4));
        drawHD_VertexArrayConstruct(gabi::at<u8>(object + 0x158));
        objectView->mpAllocation = 0;
        objectView->mAlignment = 0;
    }
    return object;
}

/* 0252F7A8 */
u32 constructSmallPacket(u32 object) {
    if (!object)
        object = drawHD_operatorNew(16);
    return object;
}

/* 0252F7D8 */
void destroyTexturePacket(u32 object, u32 flags) {
    if (!object)
        return;
    drawHD_VertexArrayDestruct(gabi::at<u8>(object + 0x158), 2);
    drawHD_VertexBufferDestruct(gabi::at<u8>(object + 4), 2);
    if (flags & 1)
        drawHD_operatorDelete(gabi::at<u8>(object));
}

/* 0252F9D8 */
void destroyPacketCopy(u32 object, u32 flags) {
    auto* objectView = gabi::at<u8>(object);
    if (!object)
        return;
    drawHD_J3DPacketDestruct(objectView, 0);
    if (flags & 1)
        drawHD_operatorDelete(objectView);
}

/* 0252F73C */
void hdEmptyVirtualSlot1() {}
/* 0252F740 */
void hdEmptyVirtualSlot2() {}

/* 0252F744 */
void hdEmptyVirtualSlot3() {}
/* 0252F748 */
void hdEmptyVirtualSlot4() {}

/* 0252F7D4 */
void hdEmptyVirtualSlot5() {}
/* 0252F9D4 */
void hdEmptyVirtualSlot6() {}
/* 0252FB60 */
void hdEmptyVirtualSlot7() {}

/* 0252FB4C */
void hdDeletingDestructor1(u32 object, u32 flags) {
    if (object && (flags & 1))
        drawHD_operatorDeleteWithFlags(gabi::at<u8>(object), flags);
}
/* 0252FB64 */
void hdDeletingDestructor2(u32 object, u32 flags) {
    if (object && (flags & 1))
        drawHD_operatorDeleteWithFlags(gabi::at<u8>(object), flags);
}

/* 0252FB78 */
void hdDeletingDestructor3(u32 object, u32 flags) {
    if (object && (flags & 1))
        drawHD_operatorDeleteWithFlags(gabi::at<u8>(object), flags);
}
/* 0252FB8C */
void hdDeletingDestructor4(u32 object, u32 flags) {
    if (object && (flags & 1))
        drawHD_operatorDeleteWithFlags(gabi::at<u8>(object), flags);
}

/* 0252FBA0 */
void hdDeletingDestructor5(u32 object, u32 flags) {
    if (object && (flags & 1))
        drawHD_operatorDeleteWithFlags(gabi::at<u8>(object), flags);
}
/* 0252FBB4 */
void hdDeletingDestructor6(u32 object, u32 flags) {
    if (object && (flags & 1))
        drawHD_operatorDeleteWithFlags(gabi::at<u8>(object), flags);
}

/* 0252FBC8 */
u32 constructRandomState(u32 object) {
    if (!object)
        object = drawHD_operatorNew(12);
    if (object) {
        auto* objectView = gabi::at<cM_rnd_c>(object);
        objectView->mSeed = 5;
        objectView->mMultiplier = 20;
        objectView->mIncrement = 10;
    }
    return object;
}

/* 0252FCB4 */
u32 trailingDrawFlag() { return 1; }

/* 0252F608 */
void initializeDrawGlobals() {
    l_drawDestructorList.mDestructor = 0;
    l_drawDestructorList.mNext = 0;
    l_drawDestructorList.mFlags = 0;
    l_drawDestructorList.mObject = 0;
    drawHD_registerGlobal(&l_drawGlobalDestructor);
    f32 x = l_drawInitialX.get(), y = l_drawInitialY.get();
    mDrawInitialX = x;
    mDrawInitialY = y;
    drawHD_constructHeaderByte(&l_drawHeaderByte);
    drawHD_registerGlobal(&l_drawHeaderDestructor);
    drawHD_constructArchiveByte(&l_drawArchiveByte);
    drawHD_registerGlobal(&l_drawArchiveDestructor);
    drawHD_drawBaseConstructEntry(&l_drawBase);
    l_drawBase.mVtable = kGlobalDrawVtable;
}

/* 0252FC14 */
void initializeRandomGlobals() {
    l_randomDestructorList.mDestructor = 0;
    l_randomDestructorList.mNext = 0;
    l_randomDestructorList.mFlags = 0;
    l_randomDestructorList.mObject = 0;
    drawHD_registerGlobal(&l_randomGlobalDestructor);
    f32 x = l_randomInitialX.get(), y = l_randomInitialY.get();
    mRandomInitialX = x;
    mRandomInitialY = y;
    drawHD_constructHeaderByte(&l_randomHeaderByte);
    drawHD_registerGlobal(&l_randomHeaderDestructor);
    drawHD_constructArchiveByte(&l_randomArchiveByte);
    drawHD_registerGlobal(&l_randomArchiveDestructor);
    drawHD_randomConstructEntry(&l_randomState);
}

/* 0252F838 */
void dDlst_mirrorPacket_destruct(u32 object, u32 flags) {
    auto* objectView = gabi::at<dDlst_mirrorPacket>(object);
    auto* objectView1 = gabi::at<u8>(object);
    if (!object)
        return;
    objectView->mPolyPassVtable = u32(kPolyPassBaseVtable);
    objectView->mGrpPassVtable = u32(kGrpPassBaseVtable);
    *gabi::at<be<u32>>(gabi::ea(&objectView->mLinChk[32])) = u32(kLinChkBaseVtable);
    drawHD_dBgSLinChkDestruct(gabi::at<u8>(object + 0x33BC), 0);
    drawHD_UniformBlockDestruct(gabi::at<u8>(object + 0x3054), 0);
    drawHD_UniformBlockDestruct(gabi::at<u8>(object + 0x2FAC), 0);
    drawHD_ShaderParamDestruct(gabi::at<u8>(object + 0x2FA0), 2);
    if (object + 0x2AE0) {
        drawHD_VertexBufferRelease(gabi::at<u8>(object + 0x2C38));
        u32 allocation = objectView->mPackets.mPackets[0].mpAllocation.get();
        objectView->mPackets.mPackets[0].mpVertices = u32(0);
        if (allocation) {
            const u32 heap = drawHD_findAllocationHeap(gabi::at<u8>(mHeapManager.get()),
                                                       gabi::at<u8>(allocation));
            const u32 target =
                gabi::at<HeapVtable>(gabi::at<Heap>(heap)->mVtable.get())->mFree.get();
            gabi::call_ptr(target, gabi::at<u8>(heap),
                           gabi::at<u8>(objectView->mPackets.mPackets[0].mpAllocation.get()));
            objectView->mPackets.mPackets[0].mAlignment = u32(0);
            objectView->mPackets.mPackets[0].mpAllocation = u32(0);
        }
        drawHD_VertexBufferRelease(gabi::at<u8>(object + 0x2E8C));
        allocation = objectView->mPackets.mPackets[1].mpAllocation.get();
        objectView->mPackets.mPackets[1].mpVertices = u32(0);
        if (allocation) {
            const u32 heap = drawHD_findAllocationHeap(gabi::at<u8>(mHeapManager.get()),
                                                       gabi::at<u8>(allocation));
            const u32 target =
                gabi::at<HeapVtable>(gabi::at<Heap>(heap)->mVtable.get())->mFree.get();
            gabi::call_ptr(target, gabi::at<u8>(heap),
                           gabi::at<u8>(objectView->mPackets.mPackets[1].mpAllocation.get()));
            objectView->mPackets.mPackets[1].mAlignment = u32(0);
            objectView->mPackets.mPackets[1].mpAllocation = u32(0);
        }
        objectView->mPackets.mpMaterial = u32(0);
        drawHD_destroyArray(gabi::at<u8>(object + 0x2AE0), 2, 0x254, kTexturePacketDestructor, 0,
                            0);
    }
    drawHD_TextureDestruct(gabi::at<u8>(object + 0x2944), 2);
    drawHD_TextureDestruct(gabi::at<u8>(object + 0x27AC), 2);
    drawHD_TextureDestruct(gabi::at<u8>(object + 0x2614), 2);
    drawHD_J3DPacketDestruct(objectView1, 0);
    if (flags & 1)
        drawHD_operatorDelete(objectView1);
}

/* 0252FA2C */
u32 constructFormattedText(u32 object, u32 format, u32 a5, u32 a6, u32 a7, u32 a8, u32 a9,
                           u32 a10) {
    // The formatter accepts the PPC va_list, including the caller's register-save area.
    // HD: CR6 controls the eight saved FPRs in the PPC va_list frame.
    const u32 entrySP = gabi::cpu->r[1];
    const bool floatingArguments = gabi::cpu->cr[6] != 0;
    Local<PPCFormatFrame> frame;
    const u32 base = gabi::ea(frame.get());
    const u32 words[] = {object, format, a5, a6, a7, a8, a9, a10};
    for (u32 i = 0; i < 8; i++)
        frame->mGPR[i] = words[i];
    if (floatingArguments) {
        for (u32 i = 0; i < 8; i++) {
            u64 bits;
            std::memcpy(&bits, &gabi::cpu->f[i + 1].ps0, 8);
            frame->mFPRWords[i * 2] = u32(bits >> 32);
            frame->mFPRWords[i * 2 + 1] = u32(bits);
        }
    }
    if (!object)
        object = drawHD_operatorNew(0x4C);
    if (object) {
        gabi::at<BufferedText>(object)->mpBuffer = object + 12;
        gabi::at<BufferedText>(object)->mVtable = u32(kSafeStringBaseVtable);
        gabi::at<BufferedText>(object)->mBufferSize = u32(64);
        gabi::at<BufferedText>(object)->mBuffer[63] = 0;
        const u32 buffer = gabi::at<BufferedText>(object)->mpBuffer.get();
        gabi::at<BufferedText>(object)->mVtable = u32(kBufferedStringVtable);
        *gabi::at<be<u8>>(buffer) = 0;
        gabi::at<BufferedText>(object)->mVtable = u32(kFixedStringVtable);
        frame->mGprCount = 2;
        frame->mpOverflowArgs = entrySP + 8;
        frame->mpRegisterSaveArea = base + 0x18;
        frame->mFprCount = 0;
        drawHD_BufferedTextFormat(gabi::at<u8>(object), gabi::at<u8>(format),
                                  gabi::at<u8>(base + 8));
    }
    return object;
}

/* WWHD glue
data l_randomArchiveDestructor 101D6188
data l_randomHeaderDestructor 101D617C
data l_randomGlobalDestructor 101D6170
data l_drawArchiveDestructor 101D6154
data l_drawHeaderDestructor 101D6148
data l_drawGlobalDestructor 101D613C
data l_mirrorTextureMtx 1047572C
data l_mirrorTarget 10475778
data l_mirrorEye 1047576C
data l_randomState 10475828
data l_drawBase 10475790
data l_randomArchiveByte 10475815
data l_randomHeaderByte 10475814
data l_drawArchiveByte 10475729
data l_drawHeaderByte 10475728
data mRandomInitialY 10475810
data mRandomInitialX 1047580C
data mDrawInitialY 10475724
data mDrawInitialX 10475720
data l_randomDestructorList 10475818
data l_drawDestructorList 1047575C
str 1004CA4C "Always"
str 1004CA54 "mirror_sheild_draw"
str 1004CAD8 "Zelda_1"
str 1004CAE0 "Zelda_2"
str 1004CAE8 "Zelda_3"
str 1004CAF0 "MASKOFF"
str 1004CAF8 "SEA"
str 1004CAFC "VRBOX"
str 1004CB04 "BG_OPA"
str 1004CB0C "BG_XLU"
str 1004CB14 "FILTER"
str 1004CB1C "SNAP"
str 1004CB24 "2D"
str 1004CB28 "KUMO"
str 1004CB30 "INVISIBLE"
str 1004CB3C "SHADOW_ON"
str 1004CB48 "DISABLE_BLOOM"
str 1004CB58 "PLAYER_1"
str 1004CB64 "PLAYER_2"
str 1004CB70 "PLAYER_3"
str 1004CB7C "OBJECT_OPA"
str 1004CB88 "OBJECT_XLU"
data l_zero 1004CA34
data l_one 1004CA30
data mWipeRate 101D6160
data mWipeColor 101D6168
data mWipeSpeed 101D6164
data mWipe 101D616C
data l_drawInitialY 1004CBA4
data l_drawInitialX 1004CBA0
data l_randomInitialY 1004CC6C
data l_randomInitialX 1004CC68
data mXluDrawBuffer 104B4638
data mOpaDrawBuffer 104B4634
data l_zSortStep 1004CB9C
data l_zSortFar 1004CB98
data l_zSortNear 1004CB94
data mHeapManager 101F8B4C
data mResourceControl 101F4F28
data mDrawVertexCount 104B4D04
data mDrawStart 104B4CFC
data mDrawPrimitive 104B4D00
data mDrawCount 104B4D08
data mTextureManager 101F9968
data mGpuContext 101F8BD8
data l_textureGuard 101FD7BC
data l_textureVtable 101FDCF8
data l_uniformOne 1014517C
data l_uniformZero 10145180
data l_mirrorFar 1004CA88
data l_mirrorOrthoTop 1004CA90
data l_mirrorOrthoBottom 1004CA94
data l_mirrorScaleY 1004CA8C
data l_mirrorMatrixGuard 1047578C
data l_mirrorEyeY 1004CA84
data l_mirrorEyeX 1004CA80
data l_mirrorTargetGuard 10475788
data l_mirrorEyeGuard 10475784
data l_degreesToRadians 1004CA7C
data l_half 1004CA44
data l_mirrorDepth 1004CA78
data l_mirrorBottom 1004CA74
data l_mirrorRight 1004CA70
data l_mirrorTop 1004CA6C
data l_mirrorLeft 1004CA68
data mDepthReadPixel 10147624
data l_depthYPack 1004CAB0
data l_depthXPack 1004CAA8
data l_unsignedSplit 1004CAAC
data l_depthMultiplier 1004CAB4
data l_depthHeight 1004CAA4
data l_depthWidth 1004CAA0
data mDepthManager 101F95D0
ext 0252FA2C void drawHD_formattedTextEntry(void*, void*, u32, u32, u32, u32, u32, u32, f32, f32, f32)
ext 027BFF68 u32 drawHD_TextureFormatChannelBits(u32, u32)
ext 027BFF9C s32 drawHD_TextureFormatChannelType(u32, u32)
ext 027BFF54 s32 drawHD_TextureFormatChannels(u32)
ext 027BFF80 u32 drawHD_TextureFormatChannel(u32, u32)
ext 027BFF40 u32 drawHD_TextureFormatBytes(u32)
ext 027FE0DC void drawHD_ShaderParamCommit(void*, u32)
ext 025155D8 void drawHD_normalizeVector(void*)
ext 028E8F64 void drawHD_PSMTXMultVec(void*, void*, void*)
ext 028E9108 void drawHD_PSMTXConcat(void*, void*, void*)
ext 0252D9C4 void drawHD_copyMirrorMatrixEntry(void*, void*)
ext 028E90D4 void drawHD_PSMTXCopy(void*, void*)
ext 025F1FC4 void drawHD_mDoMTXConcatProjView(void*, void*, void*)
ext 028E9A70 void drawHD_C_MTXLightOrtho(void*, f32, f32, f32, f32, f32, f32)
ext 025F1C90 void drawHD_mDoMTXLookAt(void*, void*, void*, u32)
ext 028E91EC void drawHD_PSMTXInverse(void*, void*)
ext 025200D4 u32 drawHD_getPlay()
ext 028F423C f64 drawHD_tan(f32)
ext 027FDA54 void drawHD_ShaderParamSetCamera(void*, u32, void*, void*, void*)
ext 027B5E94 void drawHD_VertexBufferFlush(void*, u32, u32)
ext 02008860 u32 drawHD_dBgSLineCross(void*, void*)
ext 024F1AFC void drawHD_dBgSLinChkSet(void*, void*, void*, u32)
ext 02008FEC void drawHD_dBgSLinChkConstruct(void*)
ext 0273AD10 u32 drawHD_operatorNew(u32)
ext 028EFFD0 void drawHD_constructArray(void*, u32, u32, u32)
ext 028F521C void drawHD_bzero(void*, u32)
ext 027FB40C void drawHD_UniformBlockConstruct(void*)
ext 027FD6F4 void drawHD_ShaderParamConstruct(void*)
ext 027BDF7C void drawHD_TextureConstruct(void*)
ext 027BE6B8 void drawHD_ResourceTextureConstruct(void*)
ext 027F1278 void drawHD_J3DPacketConstruct(void*)
ext C0006178 void drawHD_GX2DrawIndexedEx(u32, u32, u32, u32, u32, u32)
ext 027BFE5C void drawHD_VertexArrayBind(void*)
ext 027F81A4 u32 drawHD_TextureManagerGet(void*, u32)
ext 027B6F18 void drawHD_TexturePrepare(void*)
ext 0276B114 void drawHD_GpuContextReset(void*)
ext C00061A0 void drawHD_GX2Invalidate(void*)
ext 027B6F90 void drawHD_TextureFlush(void*)
ext 027BE53C void drawHD_TextureBind(void*, void*, u32, u32)
ext 02750370 void drawHD_RenderStateApply(void*)
ext 02750250 void drawHD_RenderStateConstruct(void*)
ext 027FE118 void drawHD_ShaderParamBindMaterial(void*, void*, u32)
ext 027BB7CC void drawHD_ShapeBind(void*)
ext C00060E0 void drawHD_GX2CallDisplayList(void*, u32)
ext 027B9F68 void drawHD_MaterialBind(void*)
ext 027BB9E0 void drawHD_ShapeClearDirty(void*, u32)
ext 027F29D4 u32 drawHD_getRenderContext(void*)
ext 027BDEB4 void drawHD_TextureCopy(void*, void*)
ext 027FA678 u32 drawHD_MaterialGetTextureMatrix(void*, u32)
ext 027FA974 u32 drawHD_MaterialGetTexture(void*, u32, void*)
ext 028E945C void drawHD_PSMTXScale(void*, f32, f32, f32)
ext 028E9684 void drawHD_C_MTXLookAt(void*, void*, void*, void*)
ext 028E9E88 void drawHD_PSVECSubtract(void*, void*, void*)
ext 02563F64 void drawHD_transformLightPosition(void*, void*, void*)
ext 0201ADE0 void drawHD_positionToCamera(void*, void*, void*)
ext 024F8020 u32 drawHD_getCamera()
ext 027FE084 void drawHD_ShaderParamSetMode(void*, u32, u32)
ext 027FF530 void drawHD_MaterialSetVertexArray(void*, void*, void*, void*, u32)
ext 027FF478 void drawHD_VertexBufferSetMirrorVertices(void*, void*, u32, void*)
ext 02756140 u32 drawHD_getAllocationHeap(void*)
ext 02800B0C void drawHD_MaterialInit(void*, void*, u32)
ext 027B90AC u32 drawHD_findMaterial(void*, void*)
ext 027FFCBC u32 drawHD_getMirrorModel()
ext 02773798 void drawHD_TextureSetResource(void*, void*)
ext 026066C4 u32 drawHD_getObjectResource(void*, void*, u32)
ext 0252C8B4 u32 drawHD_newDrawBuffer(u32, void*)
ext 0273AF40 void drawHD_operatorDelete(void*)
ext 028F0164 void drawHD_destroyArray(void*, u32, u32, u32, u32, u32)
ext 027F0A0C void drawHD_J3DDrawBufferDestruct(void*, u32)
ext 02759C10 void drawHD_BufferedTextFormat(void*, void*, void*)
ext 027F13DC void drawHD_J3DPacketDestruct(void*, u32)
ext 027BE2B0 void drawHD_TextureDestruct(void*, u32)
ext 02755FEC u32 drawHD_findAllocationHeap(void*, void*)
ext 027BF7E8 void drawHD_VertexBufferRelease(void*)
ext 027FD764 void drawHD_ShaderParamDestruct(void*, u32)
ext 027FB528 void drawHD_UniformBlockDestruct(void*, u32)
ext 02008B4C void drawHD_dBgSLinChkDestruct(void*, u32)
ext 027F0E04 void drawHD_J3DDrawBufferEntryImm(void*, void*, u32)
ext 027F2B68 f64 drawHD_PSMTXMultVecZ(void*, void*)
ext 025EDBC0 void drawHD_lineMatSortReset(void*)
ext 027F093C void drawHD_J3DDrawBufferFrameInit(void*)
ext 0252CDC0 void drawHD_listSetEntry(void*, void*, void*, void*)
ext 0252CD10 void drawHD_randomInitEntry(void*, u32, u32, u32)
ext 0252CCBC void drawHD_drawBaseConstructEntry(void*)
ext 0252FBC8 void drawHD_randomConstructEntry(void*)
ext 028F026C void drawHD_registerGlobal(void*)
ext 028EAB2C void drawHD_constructArchiveByte(void*)
ext 028ED6F8 void drawHD_constructHeaderByte(void*)
ext 0273AF40 void drawHD_operatorDeleteWithFlags(void*, u32)
ext 027B5CBC void drawHD_VertexBufferDestruct(void*, u32)
ext 027BF880 void drawHD_VertexArrayDestruct(void*, u32)
ext 027BF734 void drawHD_VertexArrayConstruct(void*)
ext 027B5BD8 void drawHD_VertexBufferConstruct(void*)
ext 0252F54C void drawHD_wipeInEntry(void*, f32)
*/
