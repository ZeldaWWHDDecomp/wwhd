/**
 * d_a_pirate_flag.cpp (WWHD)
 * Pirate ship flag: HD cloth simulation and GX2 packet support.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_pirate_flag.cpp) to the WWHD layout, with the HD-only GX2
 * packet/material code written from the WWHD code, and verified against cking.rpx.
 * Every marked function retains its verified WWHD behavior; the final glue records TU data.
 */
#include "d/actor/d_a_pirate_flag.h"
#include <cmath>

static daPirate_Flag_HIO_c l_HIO;
using PirateShipPointer = gptr<PirateShipView>;
static PirateShipPointer l_p_ship;
static PirateFlagOffset flag_offset;
static PiratePositionData l_pos;
static PirateTexCoordData l_texCoord;
using PirateWord = be<u32>;
static PirateWord l_calcMtx;
static PirateWord l_storageManager;
static PirateWord l_resourceLock;
static PirateWord l_resourceControl;
static PirateWord l_view;
static PirateWord l_frameCounter;
static PirateWord l_opaqueBuffer;
static PirateWord l_translucentBuffer;
static PirateWord l_debugGuardX;
static PirateWord l_debugGuardY;
static PirateWord l_debugGuardZ;
static PirateZeroVector l_zero;
static PiratePiData l_pi;
static PirateZeros l_headerZeros;
static PirateByteData l_headerAngle;
static PirateByteData l_headerCircle;
namespace {
void *ptr(u32 a) { return gabi::at<void>(a); }
} // namespace

// Linkage area is reserved below temporaries for real guest callees' LR save.

namespace {
void free_owned_pointer(be<u32> *slot) {
    u32 storage = *slot;
    u32 manager = (u32)l_storageManager;
    u32 heap = pirateFindStorageHeap(ptr(manager), ptr(storage));
    u32 vtable = gabi::at<PirateHeapView>(heap)->__vtbl;
    u32 target = gabi::at<PirateVirtualFunctions>(vtable)->mpFree;
    gabi::call_ptr(target, ptr(heap), ptr(u32(*slot)));
}
void release_cloth_vertex_buffers(u32 buffers) {
    auto *cloth = gabi::at<PirateClothBuffers>(buffers);
    for (u32 frame = 0; frame < 2; ++frame) {
        for (u32 channel = 0; channel < 2; ++channel) {
            PirateVertexBuffer *buffer = &cloth->mBuffers[frame][channel];
            pirateTextureRelease(buffer->mTexture);
            u32 storage = buffer->mpStorage;
            buffer->mpVertices = 0;
            if (storage) {
                (void)(u32)buffer->mCount;
                free_owned_pointer(&buffer->mpStorage);
                buffer->mCount = 0;
                buffer->mpStorage = 0;
            }
        }
    }
}
void release_material_array(PirateTextureArray *array) {
    u32 storage = array->mpTextures;
    if (!storage)
        return;
    s32 count = array->mCount;
    for (s32 index = 0; index < count; ++index) {
        u32 element = storage + u32(index) * 0xF4;
        u32 vtable = gabi::at<PirateTextureSamplerView>(element)->__vtbl;
        u32 target = gabi::at<PirateVirtualFunctions>(vtable)->mpDestroyTexture;
        gabi::call_ptr(target, ptr(element), 2);
        count = array->mCount;
        storage = array->mpTextures;
    }
    free_owned_pointer(&array->mpTextures);
    array->mCount = 0;
    array->mpTextures = 0;
}
void release_cloth_packet(u32 packet) {
    gabi::at<daPirate_Flag_packet_c>(packet)->__vtbl = 0x10034794;
    u32 buffers = packet + 0x914;
    release_cloth_vertex_buffers(buffers);
    gabi::at<PirateClothBuffers>(buffers)->mCount = 0;
    s32 materials = (s32)gabi::at<daPirate_Flag_packet_c>(packet)->mDrawMaterialCount;
    for (s32 index = 0; index < materials; ++index) {
        u32 storage = (u32)gabi::at<daPirate_Flag_packet_c>(packet)->mpDrawMaterials;
        if (u32(index) < u32(materials))
            storage += u32(index) * 0x23C;
        for (u32 channel = 0; channel < 2; ++channel)
            pirateUniformBufferRelease(ptr(storage + 0x10 + channel * 0x1C));
        materials = (s32)gabi::at<daPirate_Flag_packet_c>(packet)->mDrawMaterialCount;
    }
    for (u32 offset : {0x1298u, 0x1340u})
        for (u32 channel = 0; channel < 2; ++channel)
            pirateUniformBufferRelease(ptr(packet + offset + channel * 0x1C));
    pirateTextureSamplerDestroy(ptr(packet + 0x1968), 2);
    pirateTextureSamplerDestroy(ptr(packet + 0x17D0), 2);
    pirateIndexBufferDestroy(ptr(packet + 0x1698), 2);
    pirateMaterialDestroy(ptr(packet + 0x1330), 0);
    pirateMaterialDestroy(ptr(packet + 0x1288), 0);
    pirateDrawMaterialDestroy(ptr(packet + 0x127C), 2);
    if (buffers) {
        release_cloth_vertex_buffers(buffers);
        gabi::at<PirateClothBuffers>(buffers)->mCount = 0;
        pirateDestroyArray(
            ptr(buffers), 4, 0x254,
            ptr(gabi::guest_fn(reinterpret_cast<const void *>(pirate_vertex_buffer_destruct))), 0,
            0);
    }
    u32 list = (u32)gabi::at<daPirate_Flag_packet_c>(packet)->mpMaterials;
    if (list) {
        s32 count = (s32)gabi::at<daPirate_Flag_packet_c>(packet)->mMaterialCount;
        for (s32 index = 0; index < count; ++index) {
            u32 material = list + u32(index) * 0x14;
            if (material) {
                gabi::at<PirateMaterialRecord>(material)->mpProgram = 0;
                release_material_array(&gabi::at<PirateMaterialRecord>(material)->mTextures[0]);
                release_material_array(&gabi::at<PirateMaterialRecord>(material)->mTextures[1]);
                count = (s32)gabi::at<daPirate_Flag_packet_c>(packet)->mMaterialCount;
                list = (u32)gabi::at<daPirate_Flag_packet_c>(packet)->mpMaterials;
            }
        }
        free_owned_pointer(&gabi::at<daPirate_Flag_packet_c>(packet)->mpMaterials);
        gabi::at<daPirate_Flag_packet_c>(packet)->mMaterialCount = 0;
        gabi::at<daPirate_Flag_packet_c>(packet)->mpMaterials = 0;
    }
    piratePacketBaseDestroy(ptr(packet), 0);
}
} // namespace

namespace {

void copy_vector(cXyz *destination, cXyz *source) {
    auto *src = reinterpret_cast<PirateVectorWords *>(source);
    auto *dst = reinterpret_cast<PirateVectorWords *>(destination);
    u32 x = src->x, y = src->y, z = src->z;
    dst->x = x;
    dst->y = y;
    dst->z = z;
}
} // namespace

namespace {

u32 allocate_cloth_storage(u32 bytes, u32 alignment) {
    u32 heap = pirateGetStorageHeap(ptr((u32)l_storageManager));
    u32 target = (u32)gabi::at<PirateVirtualFunctions>(u32(gabi::at<PirateHeapView>(heap)->__vtbl))
                     ->mpAllocate;
    return gabi::call_ptr<u32>(target, ptr(heap), bytes, alignment);
}
void initialize_cloth_texture(u32 destination, u32 source) {
    auto *dst = gabi::at<PirateTextureSamplerView>(destination);
    auto *src = gabi::at<PirateTextureSamplerView>(source);
    bool same = true;
    be<u32> *dstFields[] = {&dst->mWidth,     &dst->mHeight,    &dst->mDepth,
                            &dst->mDimension, &dst->mFormat,    &dst->mMipCount,
                            &dst->mPitch,     &dst->mAlignment, &dst->mUse};
    be<u32> *srcFields[] = {&src->mWidth,     &src->mHeight,    &src->mDepth,
                            &src->mDimension, &src->mFormat,    &src->mMipCount,
                            &src->mPitch,     &src->mAlignment, &src->mUse};
    for (u32 i = 0; i < 9; ++i)
        if (u32(*dstFields[i]) != u32(*srcFields[i])) {
            same = false;
            break;
        }
    if (!same)
        pirateTextureCopy(dst, src);
    else {
        u32 image = src->mpImage, mipmap = src->mpMipmaps;
        dst->mpImage = image;
        dst->mpImageBuffer = image;
        dst->mpMipmaps = mipmap;
        dst->mpMipmapBuffer = mipmap;
    }
    u8 dirty = dst->mDirty;
    dst->mWrapY = 2;
    dst->mWrapX = 2;
    dst->mWrapZ = 2;
    dst->mDirty = u8(dirty | 2);
}
} // namespace

namespace {

void bind_material_uniforms(u32 program, u32 material) {
    u32 table =
        u32(gabi::at<PirateMaterialView>(material)->mCurrentUniform) * 0x1C + material + 0x10;
    u32 record = (u32)gabi::at<PirateShaderProgramView>(program)->mUniformCount
                     ? (u32)gabi::at<PirateShaderProgramView>(program)->mpUniforms
                     : 0;
    s16 vertex = s16(gabi::at<PirateUniformLocations>(record)->mVertex),
        pixel = s16(gabi::at<PirateUniformLocations>(record)->mPixel),
        geometry = s16(gabi::at<PirateUniformLocations>(record)->mGeometry);
    u32 size = (u32)gabi::at<PirateUniformView>(table)->mSize,
        data = (u32)gabi::at<PirateUniformView>(table)->mpData;
    if (pixel != -1)
        pirateGX2SetPixelUniform(pixel, ptr(data), size);
    if (vertex != -1)
        pirateGX2SetVertexUniform(vertex, ptr(data), size);
    if (geometry != -1)
        pirateGX2SetGeometryUniform(geometry, ptr(data), size);
}
void call_material_apply(u32 material, u32 program) {
    u32 target =
        (u32)gabi::at<PirateVirtualFunctions>(u32(gabi::at<PirateMaterialView>(material)->__vtbl))
            ->mpApplyMaterial;
    gabi::call_ptr(target, ptr(material), ptr(program));
}
} // namespace

/* 023D1804 */
void daPirate_Flag_packet_c::setCorrectNrmAngle(s16 windAngle, f32 windStrength) {
    f32 random = cM_rndF(200.0f);
    s16 wave = s16(u32(s32(m87C)) + u32(gabi::ftoi(random)) + 900u);
    m87C = wave;
    f32 sine = cM_ssin(wave);
    m878 = s16(gabi::ftoi(300.0f * sine));
    f32 attenuation = gabi::fnmsubs(0.5f, windStrength, 1.0f);
    s16 escape = s16(gabi::ftoi((f32)l_HIO.m1C * attenuation));
    s16 opposite = s16(u16(windAngle) + 0x8000u);
    s16 threshold = s16(gabi::ftoi((f32(escape) * 1.25f) * 182.04444885253906f));
    s16 target = 0;
    if (std::abs(s32(opposite)) < threshold) {
        s32 degree = opposite > 0 ? -s32(escape) : s32(escape);
        target = s16(gabi::ftoi(f32(degree) * 182.04444885253906f));
    } else if (std::abs(s32(windAngle)) < threshold) {
        s32 degree = windAngle > 0 ? -s32(escape) : s32(escape);
        target = s16(gabi::ftoi(f32(degree) * 182.04444885253906f));
    }
    pirateAddCalcAngle(&m87A, target, 5, 0xC0);
    s16 normal = m878;
    m878 = s16(u16(normal) + u16(s16(m87A)));
}

/* 023D1E08 */
void daPirate_Flag_packet_c::setBackNrm() {
    u32 buffer = m87E;
    cXyz *nrmBack = mBackNrm[buffer];
    cXyz *nrm = mNrm[buffer];
    for (u32 i = 0; i < 25; ++i) {
        nrmBack->x = 0.0f;
        nrmBack->y = 0.0f;
        nrmBack->z = 0.0f;
        pirateVectorSubtract(nrmBack, nrm, nrmBack);
        ++nrmBack;
        ++nrm;
    }
}

/* 023D1A2C */
void daPirate_Flag_packet_c::setNrmVtx(cXyz *normal, s32 column, s32 row) {
    cXyz *vec_arr = mPos[u32(m87E)];
    s32 index = column + row * 5;
    gabi::Local<NormalTemporary> local;
    local->center.x = (f32)vec_arr[u32(index)].x;
    local->center.y = (f32)vec_arr[u32(index)].y;
    local->center.z = (f32)vec_arr[u32(index)].z;
    local->sum.x = 0.0f;
    local->sum.y = 0.0f;
    local->sum.z = 0.0f;
    auto subtract = [&](s32 neighbor, cXyz *destination) {
        pirateVectorDifference(&vec_arr[u32(neighbor)], &local->scratch, &local->center);
        copy_vector(destination, &local->scratch);
    };
    auto accumulate = [&](bool otherFirst) {
        pirateVectorCross(otherFirst ? &local->other : &local->side, &local->scratch,
                          otherFirst ? &local->side : &local->other);
        copy_vector(&local->cross, &local->scratch);
        pirateVectorNormalize(&local->cross, &local->scratch);
        copy_vector(&local->cross, &local->scratch);
        pirateVectorAdd(&local->sum, &local->cross, &local->sum);
    };
    if (column != 0) {
        subtract(index - 1, &local->side);
        if (row != 0) {
            subtract(index - 5, &local->other);
            accumulate(false);
        }
        if (row != 4) {
            subtract(index + 5, &local->other);
            accumulate(true);
        }
    }
    if (column != 4) {
        subtract(index + 1, &local->side);
        if (row != 0) {
            subtract(index - 5, &local->other);
            accumulate(true);
        }
        if (row != 4) {
            subtract(index + 5, &local->other);
            accumulate(false);
        }
    }
    pirateVectorNormalize(&local->sum, &local->normalized);
    copy_vector(&local->sum, &local->normalized);
    MtxPush();
    u16 phase = u16(u32(column + row) * u32(-800));
    f32 sine = cM_ssin(s16(phase));
    s16 angle = s16(gabi::ftoi(900.0f * sine));
    u32 matrix = (u32)l_calcMtx;
    pirateMtxYrotM(ptr(matrix), angle);
    pirateMtxPosition(&local->sum, &local->cross);
    pirateVectorNormalize(&local->cross, &local->normalized);
    copy_vector(normal, &local->normalized);
    MtxPull();
}

/* 023D0E68 */
void daPirate_Flag_packet_c::draw() {
    auto *packet = this;
    PirateClothBuffers *buffers = &packet->mClothBuffers;
    for (u32 channel = 0; channel < 2; ++channel) {
        u32 frame = buffers->mCurrent;
        u32 vertex = buffers->mBuffers[channel][frame].mpVertices;
        for (u32 i = 0; i < 25; ++i) {
            u32 index = packet->m87E;
            cXyz *source = &packet->mPos[index][i];
            f32 z = source->z, x = source->x, y = source->y;
            PirateVertex *dest = gabi::at<PirateVertex>(vertex + i * 0x20);
            dest->position.x = x;
            dest->position.y = y;
            dest->position.z = z;
            index = packet->m87E;
            source = channel ? &packet->mBackNrm[index][i] : &packet->mNrm[index][i];
            x = source->x;
            y = source->y;
            z = source->z;
            dest->normal.x = x;
            dest->normal.y = y;
            dest->normal.z = z;
        }
    }
    u32 frame = buffers->mCurrent;
    u32 buffer = gabi::ea(&buffers->mBuffers[0][frame].mBuffer);
    for (u32 channel = 0; channel < 2; ++channel) {
        u32 bytes = gabi::at<PirateBufferDescriptor>(buffer)->mByteCount;
        pirateBufferFlush(ptr(buffer), 0, bytes);
        buffer += 0x4A8;
    }
    frame = buffers->mCurrent;
    buffers->mCurrent = frame == 0 ? 1 : 0;
    pirateSetFog(packet->mTevStr.get());
    pirateSetLight(packet->mTevStr.get());
    gabi::Local<PacketTemporary> local;
    pirate_copy_matrix(local->matrix, ptr(0x104B45F8));
    u32 view = (u32)l_view;
    pirateDrawMaterialSetView(&packet->mDrawMaterialCount, 0, local->matrix, ptr(0x104B470C),
                              ptr(view + 0x240));
    dKy_tevstr_c *tev = packet->mTevStr;
    u32 material = packet->mpDrawMaterials;
    pirate_color_s16(local->color, &tev->mColorC0);
    f32 scale = reinterpret_cast<PirateTevScaleView *>(packet->mTevStr.get())->mColorScaleC;
    pirateSetScaledColor(ptr(material + 0x1C4), local->color, scale);
    tev = packet->mTevStr;
    pirate_color_s16(local->color, &reinterpret_cast<PirateTevScaleView *>(tev)->mHdColorC);
    scale = reinterpret_cast<PirateTevScaleView *>(packet->mTevStr.get())->mHdColorScaleC;
    pirateSetScaledColor(ptr(material + 0x1D4), local->color, scale);
    pirateDrawMaterialUpdate(&packet->mDrawMaterialCount, 0);
    tev = packet->mTevStr;
    // HD: GX2 stores float color uniforms where GameCube used GX color registers.
    pirate_color_s16(&packet->mMaterialParams[16], &tev->mColorC0);
    pirate_color_u8(&packet->mMaterialParams[20], &tev->mColorK0);
    scale = reinterpret_cast<PirateTevScaleView *>(packet->mTevStr.get())->mColorScaleK;
    pirateScaleColor(&packet->mMaterialParams[20], scale);
    if (tev->mColorK1.a)
        pirate_color_u8(&packet->mMaterialParams[24], &tev->mColorK1);
    else
        for (u32 i = 0; i < 4; ++i)
            packet->mMaterialParams[24 + i] = 0.0f;
    pirateMaterialInvalidate(&packet->mTevMaterial);
    pirate_copy_matrix(local->position, &packet->mMtx);
    pirateMatrixCopy(local->position, &packet->mShapeMaterial[0x74]);
    pirateMaterialInvalidate(&packet->mShapeMaterial);
}

/* 023D116C */
BOOL daPirate_Flag_Draw(void *actor) {
    auto *i_this = static_cast<pirate_flag_class *>(actor);
    u32 ship = gabi::ea(l_p_ship.get());
    if (!gabi::at<PirateShipView>(ship)->mVisible ||
        (gabi::at<PirateShipView>(ship)->actor_condition & fopAcCnd_NODRAW_e))
        return 0;
    // HD: float and integer subrecords have different NaN copy semantics.
    auto *src = reinterpret_cast<PirateTevView *>(&gabi::at<PirateShipView>(ship)->tevStr);
    auto *dst = reinterpret_cast<PirateTevView *>(&i_this->tevStr);
    for (u32 i = 0; i < 6; ++i)
        dst->light.positionDirection[i] = (f32)src->light.positionDirection[i];
    for (u32 i = 0; i < 4; ++i)
        dst->light.color[i] = (u8)src->light.color[i];
    for (u32 i = 0; i < 4; ++i)
        dst->light.attenuationShorts[i] = (u16)src->light.attenuationShorts[i];
    for (u32 i = 0; i < 8; ++i)
        dst->light.attenuation[i] = (f32)src->light.attenuation[i];
    for (u32 i = 0; i < 3; ++i)
        dst->worldPosition[i] = (u32)src->worldPosition[i];
    for (u32 i = 0; i < 4; ++i)
        dst->colorC0[i] = (u16)src->colorC0[i];
    for (u32 i = 0; i < 8; ++i)
        dst->colorsK[i] = (u8)src->colorsK[i];
    for (u32 i = 0; i < 4; ++i)
        dst->fogColor[i] = (u16)src->fogColor[i];
    for (u32 i = 0; i < 3; ++i)
        dst->fogBlend[i] = (f32)src->fogBlend[i];
    for (u32 i = 0; i < 9; ++i)
        dst->environment[i] = (u8)src->environment[i];
    for (u32 i = 0; i < 17; ++i)
        dst->hdLightParameters[i] = (f32)src->hdLightParameters[i];
    for (u32 i = 0; i < 6; ++i)
        dst->hdLight.positionDirection[i] = (f32)src->hdLight.positionDirection[i];
    for (u32 i = 0; i < 4; ++i)
        dst->hdLight.color[i] = (u8)src->hdLight.color[i];
    for (u32 i = 0; i < 4; ++i)
        dst->hdLight.attenuationShorts[i] = (u16)src->hdLight.attenuationShorts[i];
    for (u32 i = 0; i < 8; ++i)
        dst->hdLight.attenuation[i] = (f32)src->hdLight.attenuation[i];
    f32 y = (f32)i_this->current.pos.y, x = (f32)i_this->current.pos.x,
        z = (f32)i_this->current.pos.z;
    pirateMtxTrans(0, x, y, z);
    s16 angle = s16(u16((s16)i_this->current.angle.y) + u16((s16)i_this->shape_angle.y));
    pirateMtxYrotM(ptr((u32)l_calcMtx), angle);
    pirateMtxXrotM(ptr((u32)l_calcMtx), (s16)i_this->current.angle.x);
    pirateMtxZrotM(ptr((u32)l_calcMtx), (s16)i_this->current.angle.z);
    pirateMtxTrans(1, 0.0f, 0.0f, 30.0f);
    gabi::Local<DrawTemporary> local;
    pirateMatrixIdentity(local->identity);
    pirateMatrixConcat(local->identity, ptr((u32)l_calcMtx), &i_this->mPacket.mMtx);
    i_this->mPacket.mTevStr = &i_this->tevStr;
    u32 play = pirateGetPlay();
    l_opaqueBuffer = (u32)gabi::at<PiratePlayView>(play)->mpOpaqueDrawBuffer;
    play = pirateGetPlay();
    u32 translucent = (u32)gabi::at<PiratePlayView>(play)->mpTranslucentDrawBuffer,
        opaque = (u32)l_opaqueBuffer;
    l_translucentBuffer = translucent;
    pirateDrawBufferEntry(ptr(opaque), &i_this->mPacket, 0);
    play = pirateGetPlay();
    l_opaqueBuffer = (u32)gabi::at<PiratePlayView>(play)->mpOpaqueEntryBuffer;
    play = pirateGetPlay();
    l_translucentBuffer = (u32)gabi::at<PiratePlayView>(play)->mpTranslucentEntryBuffer;
    i_this->mPacket.draw();
    if ((u8)l_HIO.m05) {
        u16 facing = (u16)i_this->current.angle.y;
        pirateFirstLightVector(&local->result, &local->light);
        x = local->light.x;
        z = local->light.z;
        u16 light = u16(cM_atan2s(x, z));
        u16 corrected = u16(facing + u16((s16)i_this->mPacket.m87A));
        u32 angles[4] = {light, light, facing, corrected};
        for (u32 i = 0; i < 4; ++i) {
            f32 length = i == 1 ? -400.0f : 400.0f;
            local->arrows[i].x = length * cM_ssin(s16(angles[i]));
            local->arrows[i].y = 0.0f;
            local->arrows[i].z = length * cM_scos(s16(angles[i]));
        }
        for (u32 i = 0; i < 4; ++i)
            pirateVectorAdd(&local->arrows[i], &i_this->current.pos, &local->arrows[i]);
        PirateWord *guards[3] = {&l_debugGuardX, &l_debugGuardY, &l_debugGuardZ};
        constexpr u32 objects[3] = {0x101FEBEC, 0x101FEBF8, 0x101FEBF4};
        constexpr u32 names[3] = {0x10034620, 0x10034624, 0x10034628};
        for (u32 i = 0; i < 3; ++i)
            if (!u32(*guards[i])) {
                *guards[i] = 1;
                pirateDebugPropertyInit(ptr(objects[i]), (const char *)ptr(names[i]), 4);
            }
    }
    return 1;
}

/* 023D176C */
void get_cloth_anim_sub_factor(cXyz *origin, cXyz *neighbor, cXyz *accumulated, f32 restLength) {
    gabi::Local<ClothTemporary> local;
    auto *displacement = &local->displacement;
    auto *unit = &local->unit;
    pirateVectorDifference(neighbor, displacement, origin);
    pirateVectorNormalize(displacement, unit);
    f32 lengthSquared = pirateVectorSquareMag(displacement);
    f32 length = pirateSqrt(lengthSquared);
    f32 spring = (length - restLength) * (f32)l_HIO.m18;
    pirateVectorScale(unit, unit, spring);
    pirateVectorAdd(accumulated, unit, accumulated);
}

/* 023D1EA4 */
void pirate_flag_move(void *actor) {
    auto *i_this = static_cast<pirate_flag_class *>(actor);
    pirateGetPlay();
    u32 wind = pirateGetWindVector();
    f32 x = (f32)gabi::at<cXyz>(wind)->x, z = (f32)gabi::at<cXyz>(wind)->z;
    s16 windAngle = cM_atan2s(x, z);
    s16 rotation =
        s16(u16(windAngle) - u16((s16)i_this->current.angle.y) - u16((s16)i_this->shape_angle.y));
    pirateMtxYrotS(ptr((u32)l_calcMtx), rotation);
    gabi::Local<ClothMoveTemporary> local;
    local->probe.x = 0.0f;
    local->probe.y = 0.0f;
    local->probe.z = 0.06400000303983688f;
    pirateMtxPosition(&local->probe, &local->result);
    local->probe.z = 0.0f;
    local->probe.x = 1.0f;
    pirateMtxPosition(&local->probe, &local->result);
    s16 phase = s16(u32((s16)i_this->m2A8) + (u32)l_HIO.m08);
    i_this->m2A8 = phase;
    f32 wave = cM_ssin(phase);
    f32 factor = gabi::fmadds(0.5f, wave, 0.5f);
    f32 strength = gabi::fmadds((f32)l_HIO.m0C, factor, (f32)l_HIO.m10 * (1.0f - factor));
    f32 normalStrength = std::fabs(f32(local->result.z));
    local->wind.x = 0.0f;
    local->wind.y = 0.0f;
    local->wind.z = strength;
    s32 iterations = 1;
    u32 play = pirateGetPlay();
    if ((u8)gabi::at<PiratePlayView>(play)->mDemoFlag) {
        local->stageName = 0x100346F4;
        local->stageVtable = 0x10034630;
        local->expectedName = 0x1047E6B8;
        local->expectedVtable = 0x10034630;
        pirateSafeString_assure(&local->stageName);
        u32 target = (u32)gabi::at<PirateVirtualFunctions>(u32(local->stageVtable))->mpAssureString;
        gabi::call_ptr(target, &local->stageName);
        u32 first = local->stageName;
        target = (u32)gabi::at<PirateVirtualFunctions>(u32(local->expectedVtable))->mpAssureString;
        gabi::call_ptr(target, &local->expectedName);
        u32 second = local->expectedName;
        bool equal = first == second;
        if (!equal) {
            equal = true;
            for (u32 i = 0; i < 0x40001; ++i) {
                u8 left = (u8)gabi::at<PirateStringBytes>(first)->characters[i],
                   right = (u8)gabi::at<PirateStringBytes>(second)->characters[i];
                if (left != right) {
                    equal = false;
                    break;
                }
                if (!left)
                    break;
            }
        }
        if (equal) {
            f32 power = pirateGetWindPower();
            pirateVectorScale(&local->wind, &local->wind, power);
            if (!(u32)l_frameCounter)
                iterations = 60;
        }
    }
    for (s32 step = 0; step < iterations; ++step) {
        u32 previous = (u8)i_this->mPacket.m87E, current = previous ^ 1;
        u32 normals = gabi::ea(i_this->mPacket.mNrm[previous]);
        u32 positions = gabi::ea(i_this->mPacket.mPos[previous]);
        u32 next = gabi::ea(i_this->mPacket.mPos[current]);
        i_this->mPacket.m87E = u8(current);
        for (s32 column = 0; column < 5; ++column)
            for (s32 row = 0; row < 5; ++row) {
                u32 index = u32(column + row * 5), source = positions + index * 12,
                    dest = next + index * 12, velocity = gabi::ea(&i_this->mPacket.m74C[index]);
                auto *srcWords = gabi::at<PirateVectorWords>(source);
                auto *dstWords = gabi::at<PirateVectorWords>(dest);
                dstWords->x = (u32)srcWords->x;
                dstWords->y = (u32)srcWords->y;
                dstWords->z = (u32)srcWords->z;
                local->center.x = (f32)gabi::at<cXyz>(source)->x;
                local->center.y = (f32)gabi::at<cXyz>(source)->y;
                local->center.z = (f32)gabi::at<cXyz>(source)->z;
                f32 dot = pirateVectorDot(&local->wind, ptr(normals + index * 12));
                pirateVectorProduct(ptr(normals + index * 12), &local->scaled, dot);
                f32 gravity = (f32)l_HIO.m14;
                local->spring.x = local->scaled.x;
                local->spring.y = f32(local->scaled.y) + gravity;
                local->spring.z = local->scaled.z;
                auto spring = [&](s32 neighbor, f32 length) {
                    pirateClothSpringCall(&local->center, ptr(positions + u32(neighbor) * 12),
                                          &local->spring, ptr(velocity), length);
                };
                bool anchored = column == 0 && (row == 0 || row == 4);
                if (column != 0) {
                    spring(s32(index) - 1, 200.0f);
                    if (row != 0) {
                        spring(s32(index) - 5, 100.0f);
                        spring(s32(index) - 6, 223.60679626464844f);
                    }
                    if (row != 4) {
                        spring(s32(index) + 5, 100.0f);
                        spring(s32(index) + 4, 223.60679626464844f);
                    }
                    if (column != 4) {
                        spring(s32(index) + 1, 200.0f);
                        if (row != 0)
                            spring(s32(index) - 4, 223.60679626464844f);
                        if (row != 4)
                            spring(s32(index) + 6, 223.60679626464844f);
                    }
                } else if (!anchored) {
                    spring(s32(index) + 1, 200.0f);
                    if (row != 0) {
                        spring(s32(index) - 5, 100.0f);
                        spring(s32(index) - 4, 223.60679626464844f);
                    }
                    if (row != 4) {
                        spring(s32(index) + 5, 100.0f);
                        spring(s32(index) + 6, 223.60679626464844f);
                    }
                }
                if (anchored) {
                    local->delta.x = (f32)l_zero.value.x;
                    local->delta.y = (f32)l_zero.value.y;
                    local->delta.z = (f32)l_zero.value.z;
                } else
                    copy_vector(&local->delta, &local->spring);
                pirateVectorAdd(ptr(velocity), &local->delta, ptr(velocity));
                pirateVectorScale(ptr(velocity), ptr(velocity), 0.875f);
                pirateVectorAdd(ptr(dest), ptr(velocity), ptr(dest));
            }
        u32 buffer = (u8)i_this->mPacket.m87E, normal = gabi::ea(i_this->mPacket.mNrm[buffer]);
        s16 facing = (s16)i_this->current.angle.y;
        pirateFirstLightVector(&local->result, &local->light);
        x = local->light.x;
        z = local->light.z;
        s16 light = cM_atan2s(x, z);
        i_this->mPacket.setCorrectNrmAngle(s16(u16(light) - u16(facing)), normalStrength);
        pirateMtxYrotS(ptr((u32)l_calcMtx), (s16)i_this->mPacket.m878);
        for (s32 row = 0; row < 5; ++row)
            for (s32 column = 0; column < 5; ++column) {
                i_this->mPacket.setNrmVtx(gabi::at<cXyz>(normal), column, row);
                normal += 12;
            }
    }
    i_this->mPacket.setBackNrm();
    u32 buffer = (u8)i_this->mPacket.m87E;
    pirateDCStoreRangeNoSync(i_this->mPacket.mPos[buffer], 0x12C);
    buffer = (u8)i_this->mPacket.m87E;
    pirateDCStoreRangeNoSync(i_this->mPacket.mNrm[buffer], 0x12C);
    buffer = (u8)i_this->mPacket.m87E;
    pirateDCStoreRangeNoSync(i_this->mPacket.mBackNrm[buffer], 0x12C);
}

/* 023D2610 */
BOOL daPirate_Flag_Execute(void *actor) {
    auto *i_this = static_cast<pirate_flag_class *>(actor);
    if (l_HIO.m07)
        return TRUE;
    PirateShipView *ship = l_p_ship;
    if (ship->actor_condition & fopAcCnd_NODRAW_e)
        return TRUE;
    if (!flag_offset.initialized) {
        flag_offset.initialized = 1;
        flag_offset.position.z = 100.0f;
        flag_offset.position.x = 0.0f;
        flag_offset.position.y = 1000.0f;
    }
    u32 model = ship->mpModel;
    pirateMatrixMultVector(ptr(model ? model + 0xC8 : 0), &flag_offset.position,
                           &i_this->current.pos);
    ship = l_p_ship;
    i_this->current.angle.x = (u16)ship->shape_angle.x;
    i_this->current.angle.y = (u16)ship->shape_angle.y;
    i_this->current.angle.z = (u16)ship->shape_angle.z;
    pirateGetPlay(); // HD: the play accessor remains a separate call.
    auto *wind = gabi::at<cXyz>(pirateGetWindVector());
    f32 z = wind->z, x = wind->x;
    s16 angle = cM_atan2s(x, z);
    s16 target = s16(u16(angle) - u16(s16(i_this->current.angle.y)));
    pirateAddCalcAngle(&i_this->shape_angle.y, target, 8, 0x400);
    pirateClothMoveCall(actor);
    return TRUE;
}

/* 023D2718 */
BOOL daPirate_Flag_IsDelete(void *actor) { return 1; }

/* 023D2720 */
BOOL daPirate_Flag_Delete(void *actor) {
    auto *i_this = static_cast<pirate_flag_class *>(actor);
    pirateResDelete(&i_this->mPhs1, "Cloth");
    pirateResDelete(&i_this->mPhs2, "Kaizokusen");
    return 1;
}

/* 023D33D4 */
s32 daPirate_Flag_Create(void *actor) {
    auto *i_this = static_cast<pirate_flag_class *>(actor);
    pirateGetPlay();
    u32 flags = (u32)i_this->actor_condition;
    if (!(flags & fopAcCnd_INIT_e)) {
        if (gabi::ea(actor) != 0) {
            pirateActorConstruct(actor);
            i_this->__vtbl = 0x10034658;
            piratePacketConstructCall(&i_this->mPacket);
            flags = (u32)i_this->actor_condition;
        }
        i_this->actor_condition = flags | fopAcCnd_INIT_e;
    }
    s32 phase = pirateResLoad(&i_this->mPhs1, "Cloth");
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    phase = pirateResLoad(&i_this->mPhs2, "Kaizokusen");
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    cXyz *dest = i_this->mPacket.mPos[u8(i_this->mPacket.m87E)];
    for (u32 row = 0; row < 5; ++row) {
        for (u32 col = 0; col < 5; ++col) {
            cXyz *source = &l_pos.points[row * 5 + col];
            // HD: float loads/stores quiet NaNs, unlike cXyz's word-copy assignment.
            f32 x = source->x, y = source->y, z = source->z;
            dest->x = x;
            dest->y = y;
            dest->z = z;
            ++dest;
        }
    }
    u32 parent = (u32)i_this->parentActorID, ship = 0;
    gabi::Local<ParentIDTemporary> key;
    key->id = parent;
    if (parent != 0xFFFFFFFFu)
        ship = pirateJudgeActor(ptr(0x025E1234), &key->id);
    l_p_ship = gabi::at<PirateShipView>(ship);
    pirateClothMoveCall(actor);
    return cPhs_COMPLEATE_e;
}

// HD: compiler-generated and GX2 packet/material code of this translation unit.

/* 023D0C50 */
void pirate_copy_matrix(void *dest, void *source) {
    auto *dst = static_cast<be<f32> *>(dest);
    auto *src = static_cast<be<f32> *>(source);
    f32 matrix[12];
    for (u32 i = 0; i < 12; ++i)
        matrix[i] = src[i];
    for (u32 i = 0; i < 12; ++i)
        dst[i] = matrix[i];
}

/* 023D0CF0 */
void pirate_color_s16(void *dest, void *source) {
    auto *dst = static_cast<be<f32> *>(dest);
    auto *src = static_cast<be<s16> *>(source);
    f32 color[4];
    for (u32 i = 0; i < 4; ++i)
        color[i] = f32((s16)src[i]) / 255.0f;
    for (u32 i = 0; i < 4; ++i)
        dst[i] = color[i];
}

/* 023D0DB4 */
void pirate_color_u8(void *dest, void *source) {
    auto *dst = static_cast<be<f32> *>(dest);
    auto *src = static_cast<be<u8> *>(source);
    f32 color[4];
    for (u32 i = 0; i < 4; ++i)
        color[i] = f32((u8)src[i]) / 255.0f;
    for (u32 i = 0; i < 4; ++i)
        dst[i] = color[i];
}

/* 023D3BA4 */
void pirateSafeString_delete(void *object, u32 flags) {
    if (object && (flags & 1))
        operator_delete(object);
}

/* 023D3BB8 */
void *pirate_vertex_buffer_construct(void *object) {
    if (!object)
        object = operator_new(0x254);
    if (object) {
        u32 a = gabi::ea(object);
        pirateBufferConstruct(ptr(a + 4));
        pirateTextureConstruct(ptr(a + 0x158));
        auto *buffer = static_cast<PirateVertexBuffer *>(object);
        buffer->mpStorage = 0;
        buffer->mCount = 0;
    }
    return object;
}

/* 023D3C14 */
void *pirate_material_block_allocate(void *object) { return object ? object : operator_new(0x10); }

/* 023D3C40 */
void pirate_vertex_buffer_destruct(void *object, u32 flags) {
    if (object) {
        u32 a = gabi::ea(object);
        pirateTextureDestroy(ptr(a + 0x158), 2);
        pirateBufferDestroy(ptr(a + 4), 2);
        if (flags & 1)
            operator_delete(object);
    }
}

/* 023D40C8 */
void pirate_empty_material_destructor(void *object) {}

/* 023D44FC */
void daPirate_Flag_HIO_delete(void *object, u32 flags) {
    if (object) {
        auto *hio = static_cast<daPirate_Flag_HIO_c *>(object);
        hio->mNo = -1;
        hio->__vtbl = 0x10034648;
        if (flags & 1)
            operator_delete(object);
    }
}

/* 023D4524 */
void pirateSafeString_assure(void *object) {}

/* 023D3A90 */
void __sinit_d_a_pirate_flag_cpp() {
    l_headerZeros.value[1] = 0;
    l_headerZeros.value[3] = 0;
    l_headerZeros.value[0] = 0;
    l_headerZeros.value[2] = 0;
    pirateRegisterGlobal(ptr(0x101CEA78));
    l_pi.minimum = -3.1415927410125732f;
    l_pi.maximum = 3.1415927410125732f;
    pirateHeaderAngleConstruct(&l_headerAngle);
    pirateRegisterGlobal(ptr(0x101CEA84));
    pirateHeaderCircleConstruct(&l_headerCircle);
    pirateRegisterGlobal(ptr(0x101CEA90));
    l_HIO.m1C = 0.0f;
    l_HIO.m0C = 13.0f;
    l_HIO.m10 = 7.0f;
    l_HIO.m14 = -3.5f;
    l_HIO.mNo = -1;
    l_HIO.m18 = 0.45f;
    l_HIO.__vtbl = 0x10034648;
    l_HIO.m06 = 0;
    l_HIO.m05 = 0;
    l_HIO.m08 = 0x40;
    l_HIO.m07 = 0;
    pirateRegisterGlobal(ptr(0x101CEA9C));
}

/* 023D3008 */
void *pirate_packet_construct(void *object) {
    if (!object)
        object = operator_new(0x1B00);
    if (!object)
        return nullptr;
    auto *packet = static_cast<daPirate_Flag_packet_c *>(object);
    u32 a = gabi::ea(object);
    piratePacketBaseConstruct(object);
    packet->m87C = 0;
    packet->mShaderCount = 0;
    packet->__vtbl = 0x10034794;
    packet->m87A = 0;
    packet->m878 = 0;
    packet->m87F = 1;
    packet->m87E = 0;
    u32 link = a + 0x90C;
    if (!link)
        link = gabi::ea(operator_new(8));
    if (link) {
        auto *record = gabi::at<PirateTextureArray>(link);
        record->mpTextures = 0;
        record->mCount = 0;
    }
    u32 buffers = a + 0x914;
    if (!buffers)
        buffers = gabi::ea(operator_new(0x968));
    if (buffers) {
        pirateConstructArray(
            ptr(buffers), 4, 0x254,
            ptr(gabi::guest_fn(reinterpret_cast<const void *>(pirate_vertex_buffer_construct))));
        gabi::at<PirateClothBuffers>(buffers)->mCurrent = 0;
        gabi::at<PirateClothBuffers>(buffers)->mCount = 0;
        gabi::at<PirateClothBuffers>(buffers)->mStride = 0x20;
        gabi::at<PirateClothBuffers>(buffers)->mInitialized = 0;
        gabi::at<PirateClothBuffers>(buffers)->mAttribute = 0;
        for (u32 channel = 0; channel < 2; ++channel)
            for (u32 frame = 0; frame < 2; ++frame)
                packet->mClothBuffers.mBuffers[frame][channel].mpVertices = 0;
    }
    pirateDrawMaterialConstruct(&packet->mDrawMaterialCount);
    pirateMaterialConstruct(&packet->mShapeMaterial);
    reinterpret_cast<PirateMaterialView *>(&packet->mShapeMaterial)->__vtbl = 0x1016EF84;
    pirateClearStorage(&packet->mShapeMaterial[0x74], 0x34);
    if (!(a + 0x12FC))
        operator_new(0x30);
    pirateMaterialConstruct(&packet->mTevMaterial);
    reinterpret_cast<PirateMaterialView *>(&packet->mTevMaterial)->__vtbl = 0x1016EFB4;
    pirateClearStorage(&packet->mMaterialParams, 0x2F0);
    // GX2 material identity matrices and initial RGBA blocks.
    for (u32 i = 0; i < 44; ++i)
        packet->mMaterialParams[i] = 0.0f;
    for (u32 i = 3; i < 44; i += 4)
        packet->mMaterialParams[i] = 1.0f;
    for (u32 offset : {0x1454u, 0x1474u, 0x1494u})
        pirateConstructArray(
            ptr(a + offset), 2, 0x10,
            ptr(gabi::guest_fn(reinterpret_cast<const void *>(pirate_material_block_allocate))));
    for (u32 i = 0; i < 8; ++i)
        if (!(a + 0x14B4 + i * 0x30))
            operator_new(0x30);
    for (u32 i = 0; i < 6; ++i)
        if (!(a + 0x1634 + i * 0x10))
            operator_new(0x10);
    packet->mMaterialInitialized = 0;
    pirateIndexBufferConstruct(&packet->mPrimitiveType);
    pirateTextureResourceConstruct(&packet->mClothTexture);
    pirateTextureResourceConstruct(&packet->mShipTexture);
    pirateTextureSamplerConstruct(&packet->mClothTextureSampler);
    pirateTextureSamplerConstruct(&packet->mShipTextureSampler);
    pirateGraphicsInitCall(object);
    return object;
}

/* 023D3CA0 */
void pirate_packet_destruct(void *object, u32 flags) {
    if (object) {
        release_cloth_packet(gabi::ea(object));
        if (flags & 1)
            operator_delete(object);
    }
}

/* 023D40CC */
void pirate_actor_destruct(void *object, u32 flags) {
    if (object) {
        release_cloth_packet(gabi::ea(object) + 0x3D4);
        pirateActorDestroy(object, 0);
        if (flags & 1)
            operator_delete(object);
    }
}

/* 023D276C */
void pirate_initialize_graphics(void *object) {
    auto *packet = static_cast<daPirate_Flag_packet_c *>(object);
    u32 a = gabi::ea(object), buffers = a + 0x914;
    gabi::Local<ArchiveTemporary> local;
    local->shaderName = 0x10034730;
    local->shaderVtable = 0x10034630;
    u32 manager = pirateShaderManagerGet(object);
    s32 shader = pirateShaderArchiveFind(
        ptr((u32)gabi::at<PirateShaderManagerView>(manager)->mpArchive), &local->shaderName);
    u32 options = 0;
    if (shader >= 0) {
        u32 count = (u32)gabi::at<PirateShaderManagerView>(manager)->mCount,
            records = (u32)gabi::at<PirateShaderManagerView>(manager)->mpRecords;
        u32 record = records + (u32(shader) < count ? u32(shader) * 0x24 : 0);
        if (!(u8)gabi::at<PirateShaderRecordView>(record)->mInitialized) {
            u32 archive = (u32)gabi::at<PirateShaderManagerView>(manager)->mpArchive,
                archiveCount = (u32)gabi::at<PirateShaderArchiveView>(archive)->mCount, program = 0;
            if (u32(shader) < archiveCount)
                program = (u32)gabi::at<PirateShaderArchiveView>(archive)->mpPrograms +
                          u32(shader) * 0x84;
            pirateShaderRecordConstruct(ptr(record), ptr(program), 0);
            count = (u32)gabi::at<PirateShaderManagerView>(manager)->mCount;
            records = (u32)gabi::at<PirateShaderManagerView>(manager)->mpRecords;
        }
        options = records + (u32(shader) < count ? u32(shader) * 0x24 : 0);
    }
    pirateSetShaderOptions(&packet->mShaderCount, ptr(options), 0);
    packet->mClothBuffers.mAttribute = 0x13;
    packet->mClothBuffers.mpDrawCallback = 0x10034788;
    for (u32 channel = 0; channel < 2; ++channel)
        for (u32 frame = 0; frame < 2; ++frame) {
            u32 buffer = buffers + channel * 0x254 + frame * 0x4A8;
            u32 vertices = (u32)gabi::at<PirateVertexBuffer>(buffer)->mpVertices;
            if (!vertices) {
                u32 storage = allocate_cloth_storage(0x320, 0x40);
                if (storage) {
                    gabi::at<PirateVertexBuffer>(buffer)->mpStorage = storage;
                    gabi::at<PirateVertexBuffer>(buffer)->mCount = 25;
                }
                vertices = (u32)gabi::at<PirateVertexBuffer>(buffer)->mpStorage;
                gabi::at<PirateVertexBuffer>(buffer)->mpVertices = vertices;
            }
            pirateVertexBufferSet(ptr(buffer + 4), ptr(vertices), 25, ptr(buffers + 0x954));
        }
    gabi::at<PirateClothBuffers>(buffers)->mCount = 0;
    gabi::at<PirateClothBuffers>(buffers)->mInitialized = 1;
    for (u32 index = 0; index < (u32)packet->mShaderCount; ++index) {
        u32 count = u32(packet->mMaterialCount), material = (u32)packet->mpMaterials;
        if (index < count)
            material += index * 0x14;
        u32 program = (u32)gabi::at<PirateMaterialRecord>(material)->mpProgram;
        gabi::at<PirateMaterialRecord>(material)->mpProgram = 0;
        release_material_array(&gabi::at<PirateMaterialRecord>(material)->mTextures[0]);
        release_material_array(&gabi::at<PirateMaterialRecord>(material)->mTextures[1]);
        gabi::at<PirateMaterialRecord>(material)->mpProgram = program;
        for (u32 channel = 0; channel < 2; ++channel) {
            u32 storage = allocate_cloth_storage(0x1E8, 4);
            for (u32 frame = 0; frame < 2; ++frame) {
                u32 texture = storage + frame * 0xF4;
                if (texture)
                    pirateTextureConstruct(ptr(texture));
            }
            if (storage) {
                auto *record = gabi::at<PirateMaterialRecord>(material);
                record->mTextures[channel].mpTextures = storage;
                record->mTextures[channel].mCount = 2;
            }
        }
        for (u32 channel = 0; channel < 2; ++channel)
            for (u32 frame = 0; frame < 2; ++frame) {
                u32 textureCount =
                        u32(gabi::at<PirateMaterialRecord>(material)->mTextures[channel].mCount),
                    texture = u32(
                        gabi::at<PirateMaterialRecord>(material)->mTextures[channel].mpTextures);
                if (frame < textureCount)
                    texture += frame * 0xF4;
                pirateTextureSetVertexBuffer(ptr(program), ptr(texture),
                                             ptr(buffers + 4 + channel * 0x254 + frame * 0x4A8),
                                             ptr(buffers + 0x954), 0);
            }
    }
    pirateDrawMaterialAllocate(&packet->mDrawMaterialCount, 1, 0);
    pirateIndexBufferSet(&packet->mPrimitiveType, ptr(0x10034668), 4, 0x28);
    packet->mIndexType = 6;
    for (u32 channel = 0; channel < 2; ++channel) {
        u32 frame = (u32)gabi::at<PirateClothBuffers>(buffers)->mCurrent,
            vertex = u32(packet->mClothBuffers.mBuffers[channel][frame].mpVertices),
            end = vertex + 0x320;
        for (u32 block = vertex; block < end; block += 0x20)
            for (u32 byte = 0; byte < 0x20; ++byte)
                gabi::at<be<u8>>(block & ~31u)[byte] = 0;
        frame = (u32)gabi::at<PirateClothBuffers>(buffers)->mCurrent;
        vertex = u32(packet->mClothBuffers.mBuffers[channel][frame].mpVertices);
        for (u32 i = 0; i < 25; ++i) {
            u32 position = 0x101CE884 + i * 12, uv = 0x101CE9B0 + i * 8, dest = vertex + i * 0x20;
            f32 x = (f32)gabi::at<cXyz>(position)->x, y = (f32)gabi::at<cXyz>(position)->y,
                z = (f32)gabi::at<cXyz>(position)->z;
            gabi::at<PirateVertex>(dest)->position.x = x;
            gabi::at<PirateVertex>(dest)->normal.x = 0.0f;
            gabi::at<PirateVertex>(dest)->position.z = z;
            gabi::at<PirateVertex>(dest)->normal.y = 0.0f;
            gabi::at<PirateVertex>(dest)->position.y = y;
            gabi::at<PirateVertex>(dest)->normal.z = 0.0f;
            gabi::at<PirateVertex>(dest)->uv[0] = (f32)gabi::at<be<f32>>(uv)[0];
            gabi::at<PirateVertex>(dest)->uv[1] = (f32)gabi::at<be<f32>>(uv)[1];
        }
    }
    u32 frame = (u32)gabi::at<PirateClothBuffers>(buffers)->mCurrent, other = frame == 0 ? 1 : 0;
    for (u32 channel = 0; channel < 2; ++channel)
        for (u32 vertex = 0; vertex < 25; ++vertex) {
            u32 source = u32(packet->mClothBuffers.mBuffers[channel][frame].mpVertices),
                dest = u32(packet->mClothBuffers.mBuffers[channel][other].mpVertices);
            for (u32 component = 0; component < 8; ++component)
                gabi::at<PirateFloatVertex>(dest)[vertex].component[component] =
                    (f32)gabi::at<PirateFloatVertex>(source)[vertex].component[component];
        }
    frame = (u32)gabi::at<PirateClothBuffers>(buffers)->mCurrent;
    u32 buffer = buffers + frame * 0x254 + 4;
    for (u32 channel = 0; channel < 2; ++channel) {
        pirateBufferFlush(ptr(buffer), 0,
                          (u32)gabi::at<PirateBufferDescriptor>(buffer)->mByteCount);
        buffer += 0x4A8;
    }
    frame = (u32)gabi::at<PirateClothBuffers>(buffers)->mCurrent;
    gabi::at<PirateClothBuffers>(buffers)->mCurrent = frame == 0 ? 1 : 0;
    pirateResourceLock(ptr((u32)l_resourceLock));
    local->clothVtable = 0x10034630;
    local->clothName = 0x10034740;
    u32 resource = pirateGetResource(ptr((u32)l_resourceControl), &local->clothName,
                                     dRes_INDEX_CLOTH_BTI_CLOTHTOON_e);
    if (!resource)
        pirateAssertion((const char *)ptr(0x1003471C), 0x2A7, (const char *)ptr(0x1003474C));
    pirateTextureSetImage(&packet->mClothTexture,
                          ptr((u32)gabi::at<PirateTextureResource>(resource)->mpImage));
    local->shipVtable = 0x10034630;
    local->shipName = 0x10034714;
    resource = pirateGetResource(ptr((u32)l_resourceControl), &local->shipName,
                                 dRes_INDEX_KAIZOKUSEN_BTI_TXA_KAIZOKU_HATA_e);
    if (!resource)
        pirateAssertion((const char *)ptr(0x1003471C), 0x2AE, (const char *)ptr(0x1003474C));
    pirateTextureSetImage(&packet->mShipTexture,
                          ptr((u32)gabi::at<PirateTextureResource>(resource)->mpImage));
    pirateResourceUnlock(ptr((u32)l_resourceLock));
    initialize_cloth_texture(a + 0x17D0, a + 0x16B0);
    initialize_cloth_texture(a + 0x1968, a + 0x1740);
}

/* 023D350C */
void pirate_render_material(void *object, void *context) {
    auto *packet = static_cast<daPirate_Flag_packet_c *>(object);
    u32 a = gabi::ea(object), c = gabi::ea(context),
        index = (u32)gabi::at<PirateRenderContextView>(c)->mMaterialIndex, program = 0;
    if (s32(index) < 4) {
        u32 count = u32(packet->mMaterialCount), record = (u32)packet->mpMaterials;
        if (index < count)
            record += index * 0x14;
        program = u32(gabi::at<PirateMaterialRecord>(record)->mpProgram);
    }
    u32 cache = pirateGetShaderCache(ptr(0x104B45C0));
    u32 shader = (u32)gabi::at<PirateShaderProgramView>(program)->mpShader,
        cachedShader = (u32)gabi::at<PirateShaderCacheView>(cache)->mpShader;
    if (shader != cachedShader) {
        u8 flags = (u8)gabi::at<PirateShaderView>(shader)->mFlags;
        u32 cachedArchive = (u32)gabi::at<PirateShaderCacheView>(cache)->mpArchive;
        if (flags & 2) {
            gabi::at<PirateShaderView>(shader)->mFlags = (u8)(u8(flags & ~2u));
            pirateShaderPrepare(ptr(shader), 0);
        }
        u32 archive = (u32)gabi::at<PirateShaderArchiveView>(
                          u32(gabi::at<PirateShaderView>(shader)->mpArchive))
                          ->mpParentArchive;
        if (cachedArchive != archive)
            pirateShaderArchiveBind(ptr(archive));
        if ((u32)gabi::at<PirateShaderView>(shader)->mDisplayListSize)
            pirateGX2CallDisplayList(ptr((u32)gabi::at<PirateShaderView>(shader)->mpDisplayList),
                                     (u32)gabi::at<PirateShaderView>(shader)
                                         ->mDisplayListSize); // HD: GX2 display list replaces the
                                                              // GameCube material list.
        else
            pirateShaderBind(ptr(shader));
        gabi::at<PirateShaderCacheView>(cache)->mpArchive = (u32)(archive);
        gabi::at<PirateShaderCacheView>(cache)->mpShader = (u32)(shader);
    }
    index = (u32)gabi::at<PirateRenderContextView>(c)->mMaterialIndex;
    if (index == 0) {
        call_material_apply(a + 0x1288, program);
        u32 material = (u32)gabi::at<PirateRenderContextView>(c)->mpMaterial;
        if (material)
            bind_material_uniforms(program,
                                   (u32)gabi::at<PirateMaterialView>(material)->mpDrawMaterial);
    } else if (index == 1) {
        bind_material_uniforms(program, (u32)packet->mpDrawMaterials);
        call_material_apply(a + 0x1330, program);
        call_material_apply(a + 0x1288, program);
    } else if (index == 2) {
        bind_material_uniforms(program, (u32)packet->mpDrawMaterials);
        call_material_apply(a + 0x1330, program);
        call_material_apply(a + 0x1288, program);
        u32 material = (u32)gabi::at<PirateRenderContextView>(c)->mpExtraMaterial;
        if (material)
            call_material_apply(material, program);
        pirateRenderContextApply(context, ptr(program));
    }
    u32 textures = (u32)gabi::at<PirateShaderProgramView>(program)->mTextureCount
                       ? (u32)gabi::at<PirateShaderProgramView>(program)->mpTextures
                       : 0;
    pirateTextureSamplerBind(&packet->mClothTextureSampler, ptr(textures + 4), -1, 0);
    u32 textureCount = (u32)gabi::at<PirateShaderProgramView>(program)->mTextureCount;
    textures =
        textureCount > 1 ? (u32)gabi::at<PirateShaderProgramView>(program)->mpTextures + 0x14 : 0;
    pirateTextureSamplerBind(&packet->mShipTextureSampler, ptr(textures + 4), -1, 0);
    gabi::Local<RasterTemporary> local;
    u32 descriptor = gabi::ea(local.get()) + 16;
    pirateRasterConstruct(ptr(descriptor));
    gabi::at<PirateRasterView>(descriptor)->mDepthMode = (u32)(2);
    gabi::at<PirateRasterView>(descriptor)->mCullMode = (u32)(0);
    u32 bits = (u32)gabi::at<PirateRasterView>(descriptor)->mControl & ~15u;
    gabi::at<PirateRasterView>(descriptor)->mAlphaCompare = (u32)(4);
    gabi::at<PirateRasterView>(descriptor)->mAlphaRef = (f32)(0.5f);
    gabi::at<PirateRasterView>(descriptor)->mAlphaEnabled = (u8)(1);
    gabi::at<PirateRasterView>(descriptor)->mControl = (u32)((((bits + 7) & 0xFFFFFF0Fu) + 0x10));
    pirateRasterSetMaterial((u32)gabi::at<PirateRenderContextView>(c)->mMaterialIndex,
                            ptr(descriptor));
    pirateRasterApply(ptr(descriptor));
    u32 count = u32(packet->mMaterialCount), record = (u32)packet->mpMaterials;
    index = (u32)gabi::at<PirateRenderContextView>(c)->mMaterialIndex;
    if (index < count)
        record += index * 0x14;
    u32 frame = u32(packet->mClothBuffers.mCurrent) == 0 ? 2 : 0;
    pirateTextureBind(
        ptr(u32(gabi::at<PirateMaterialRecord>(record)->mTextures[frame / 2].mpTextures)));
    for (u32 face = 0; face < 4; ++face) {
        u32 topology = (u32)packet->mPrimitiveType, stride = (u32)packet->mIndexStride,
            indices = (u32)packet->mpIndices, type = (u32)packet->mIndexType;
        pirateGX2DrawIndexed(type, 0xA, topology, ptr(indices + stride * (face * 10)), 0, 1);
    }
    gabi::at<PirateRasterView>(descriptor)->mCullMode = (u32)(1);
    pirateRasterSetCull(ptr(descriptor));
    count = u32(packet->mMaterialCount);
    index = (u32)gabi::at<PirateRenderContextView>(c)->mMaterialIndex;
    record = (u32)packet->mpMaterials;
    if (index < count)
        record += index * 0x14;
    frame = u32(packet->mClothBuffers.mCurrent) == 0 ? 2 : 0;
    u32 textureCount2 = u32(gabi::at<PirateMaterialRecord>(record)->mTextures[frame / 2].mCount),
        vertices = u32(gabi::at<PirateMaterialRecord>(record)->mTextures[frame / 2].mpTextures);
    if (textureCount2 > 1)
        vertices += 0xF4;
    pirateTextureBind(ptr(vertices));
    for (u32 face = 0; face < 4; ++face) {
        u32 topology = (u32)packet->mPrimitiveType, stride = (u32)packet->mIndexStride,
            indices = (u32)packet->mpIndices, type = (u32)packet->mIndexType;
        pirateGX2DrawIndexed(type, 0xA, topology, ptr(indices + stride * (face * 10)), 0, 1);
    }
    pirateRasterApply(ptr(0x104B474C));
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
ext 02578348 f32 pirateGetWindPower()
data l_calcMtx 1018C7B0
data l_storageManager 101F8B4C
data l_resourceLock 101F8B18
data l_resourceControl 101F4F28
data l_view 104B4708
data l_frameCounter 101D6008
data l_opaqueBuffer 104B4634
data l_translucentBuffer 104B4638
data l_debugGuardX 101FDA48
data l_debugGuardY 101FDAC0
data l_debugGuardZ 101FDA50
data l_zero 101FFBA8
data l_pi 1046CC3C
data l_headerZeros 1046CC68
data l_headerAngle 1046CC44
data l_headerCircle 1046CC45
data l_HIO 1046CC48
data l_p_ship 1046CC38
data flag_offset 1046CC78
data l_pos 101CE884
data l_texCoord 101CE9B0
ext 0200F428 void pirateAddCalcAngle(void*,s32,u32,u32)
ext 0200FAD8 void pirateMtxTrans(u32,f32,f32,f32)
ext 0200FCD8 void pirateMtxPosition(void*,void*)
ext 0201ADE0 void pirateVectorDifference(void*,void*,void*)
ext 0201AE48 void pirateVectorProduct(void*,void*,f32)
ext 0201B080 void pirateVectorCross(void*,void*,void*)
ext 0201B12C void pirateVectorNormalize(void*,void*)
ext 023D176C void pirateClothSpringCall(void*,void*,void*,void*,f32)
ext 023D1EA4 void pirateClothMoveCall(void*)
ext 023D276C void pirateGraphicsInitCall(void*)
ext 023D3008 void piratePacketConstructCall(void*)
ext 025200D4 u32 pirateGetPlay()
ext 02520460 s32 pirateResLoad(void*, const char*)
ext 025204C8 void pirateResDelete(void*, const char*)
ext 0255F5FC void pirateFirstLightVector(void*,void*)
ext 0255F8F4 void pirateSetFog(void*)
ext 0255FE90 void pirateSetLight(void*)
ext 0257DAA8 u32 pirateGetWindVector()
ext 025D4ED0 void pirateActorConstruct(void*)
ext 025D50BC void pirateActorDestroy(void*,u32)
ext 025D5218 u32 pirateJudgeActor(void*,void*)
ext 025F1884 void pirateMtxYrotS(void*,s32)
ext 025F1BF4 void pirateMtxXrotM(void*,s32)
ext 025F1C28 void pirateMtxYrotM(void*,s32)
ext 025F1C5C void pirateMtxZrotM(void*,s32)
ext 026066C4 u32 pirateGetResource(void*,void*,u32)
ext 0273AA24 void pirateAssertion(const char*, u32, const char*)
ext 0274D2AC void pirateScaleColor(void*,f32)
ext 0274D458 void pirateSetScaledColor(void*,void*,f32)
ext 0274FBF8 void pirateResourceLock(void*)
ext 0274FCCC void pirateResourceUnlock(void*)
ext 02750250 void pirateRasterConstruct(void*)
ext 02750370 void pirateRasterApply(void*)
ext 02750684 void pirateRasterSetCull(void*)
ext 02755FEC u32 pirateFindStorageHeap(void*,void*)
ext 02756140 u32 pirateGetStorageHeap(void*)
ext 02773798 void pirateTextureSetImage(void*,void*)
ext 027B5430 void pirateIndexBufferConstruct(void*)
ext 027B54A0 void pirateIndexBufferDestroy(void*,u32)
ext 027B54E0 void pirateIndexBufferSet(void*,void*,u32,u32)
ext 027B5BD8 void pirateBufferConstruct(void*)
ext 027B5CBC void pirateBufferDestroy(void*,u32)
ext 027B5E94 void pirateBufferFlush(void*,u32,u32)
ext 027B90AC s32 pirateShaderArchiveFind(void*,void*)
ext 027B9F68 void pirateShaderArchiveBind(void*)
ext 027BB7CC void pirateShaderBind(void*)
ext 027BB9E0 void pirateShaderPrepare(void*,u32)
ext 027BDEB4 void pirateTextureCopy(void*,void*)
ext 027BDF7C void pirateTextureSamplerConstruct(void*)
ext 027BE2B0 void pirateTextureSamplerDestroy(void*,u32)
ext 027BE53C void pirateTextureSamplerBind(void*,void*,s32,u32)
ext 027BE6B8 void pirateTextureResourceConstruct(void*)
ext 027BEBEC void pirateUniformBufferRelease(void*)
ext 027BF734 void pirateTextureConstruct(void*)
ext 027BF7E8 void pirateTextureRelease(void*)
ext 027BF880 void pirateTextureDestroy(void*,u32)
ext 027BFE5C void pirateTextureBind(void*)
ext 027F0E04 void pirateDrawBufferEntry(void*,void*,u32)
ext 027F1278 void piratePacketBaseConstruct(void*)
ext 027F13DC void piratePacketBaseDestroy(void*,u32)
ext 027F29D4 u32 pirateGetShaderCache(void*)
ext 027FB40C void pirateMaterialConstruct(void*)
ext 027FB528 void pirateMaterialDestroy(void*,u32)
ext 027FB678 void pirateMaterialInvalidate(void*)
ext 027FD6F4 void pirateDrawMaterialConstruct(void*)
ext 027FD764 void pirateDrawMaterialDestroy(void*,u32)
ext 027FDA54 void pirateDrawMaterialSetView(void*,u32,void*,void*,void*)
ext 027FDFF4 void pirateDrawMaterialUpdate(void*,u32)
ext 027FE084 void pirateDrawMaterialAllocate(void*,u32,u32)
ext 027FF478 void pirateVertexBufferSet(void*,void*,u32,void*)
ext 027FF530 void pirateTextureSetVertexBuffer(void*,void*,void*,void*,u32)
ext 027FFCBC u32 pirateShaderManagerGet(void*)
ext 027FFE54 void pirateRenderContextApply(void*,void*)
ext 0280037C void pirateRasterSetMaterial(u32,void*)
ext 0280068C void pirateSetShaderOptions(void*,void*,u32)
ext 02800B0C void pirateShaderRecordConstruct(void*,void*,u32)
ext 028E8D88 void pirateVectorAdd(void*,void*,void*)
ext 028E8DAC void pirateVectorSubtract(void*,void*,void*)
ext 028E8DD0 f32 pirateVectorSquareMag(void*)
ext 028E8E64 void pirateVectorScale(void*,void*,f32)
ext 028E8F44 f32 pirateVectorDot(void*,void*)
ext 028E8F64 void pirateMatrixMultVector(void*,void*,void*)
ext 028E9098 void pirateMatrixIdentity(void*)
ext 028E90D4 void pirateMatrixCopy(void*,void*)
ext 028E9108 void pirateMatrixConcat(void*,void*,void*)
ext 028EAB2C void pirateHeaderCircleConstruct(void*)
ext 028ED6F8 void pirateHeaderAngleConstruct(void*)
ext 028EFFD0 void pirateConstructArray(void*,u32,u32,void*)
ext 028F0164 void pirateDestroyArray(void*,u32,u32,void*,u32,u32)
ext 028F026C void pirateRegisterGlobal(void*)
ext 028F4384 f32 pirateSqrt(f32)
ext 028F521C void pirateClearStorage(void*,u32)
ext C00060E0 void pirateGX2CallDisplayList(void*,u32)
ext C0006178 void pirateGX2DrawIndexed(u32,u32,u32,void*,u32,u32)
ext C00068A8 void pirateGX2SetGeometryUniform(s32,void*,u32)
ext C0006900 void pirateGX2SetPixelUniform(s32,void*,u32)
ext C0006A38 void pirateGX2SetVertexUniform(s32,void*,u32)
ext C00088B8 void pirateDCStoreRangeNoSync(void*,u32)
ext C000A848 void pirateDebugPropertyInit(void*, const char*, u32)
str 10034700 "Cloth" in 023D2720
str 10034708 "Kaizokusen" in 023D2720
str 1003475C "Cloth" in 023D33D4
str 10034764 "Kaizokusen" in 023D33D4
*/
