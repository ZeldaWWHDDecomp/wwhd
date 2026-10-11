/**
 * d_a_mant.cpp (WWHD)
 * Cape (cloth) of the Darknuts and Phantom Ganon.
 *
 * The GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_mant.cpp) has only "Nonmatching" stubs for this unit; WWHD
 * draws the cape through its own GX2 packet (shader programs, uniform blocks, double-buffered
 * vertex buffers), so everything here is written from the WWHD code and verified against
 * cking.rpx. The HD packet retains its cape simulation while using GX2 resources.
 */
#include "d/actor/d_a_mant.h"
#include "gc_api/tu.h"
// HD: Native adapters for the GX2 packet and exact full-register math results.
f32 mantGroundCross(dBgS*, dBgS_GndChk*);
void mantDestroyGround(dBgS_GndChk*, u32);
void mantConstructGround(dBgS_GndChk*);
void mantRegisterCollision(cCcS*, u32);
void mantTransformVector(cXyz*, cXyz*);
s32 mantFloatToAngle(f32);
s32 mantAtan2s(f32, f32);
void mantSetAngles(u32, u32, u32, u32);
void mantSubtractVector(cXyz*, cXyz*, cXyz*);
void mantScaleVector(cXyz*, cXyz*, f32);
void mantDrawPacket(daMant_packet_c*);
void mantExecuteActor(mant_class*);
void mantConstructPacket(daMant_packet_c*);
void mantSetupPacketUniforms(daMant_packet_c*, mantHD::RenderPass*, mantHD::Program*, u32);
void mantDestroySphere(dCcD_Sph*, u32);
s32 mantCheckTargetHit(dCcD_Sph*);
u32 mantGetTargetHitObject(dCcD_Sph*);
void mantConstructSphere(dCcD_Sph*);
void mantGetHitType(u32);
u32 mantGetGameInfo();
s32 mantGetReverb(u32);
void mantSetupTevColor(dKy_tevstr_c*);
void mantSetupTevLight(dKy_tevstr_c*);
void mantSetParticle(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
void mantSetSimpleParticle(u32, u32, u32, u32, u32, u32, u32);
void mantDestroyActor(fopAc_ac_c*, u32);
void mantDeleteActor(fopAc_ac_c*);
void mantSetCullMinimum(fopAc_ac_c*, f32, f32, f32);
void mantSetCullMaximum(fopAc_ac_c*, f32, f32, f32);
void mantStartSound(u32, u32, u32, u32);
void mantRotateY(Mtx34*, u32);
void mantRotateX(Mtx34*, u32);
void mantMultiplyRotateZ(Mtx34*, u32);
void mantMultiplyRotateY(Mtx34*, u32);
u32 mantGetTextureArchive(u32, u32, u32, u32);
void mantScaleColor(mantHD::Color*, f32);
void mantMultiplyColor(mantHD::Color*, mantHD::Color*, f32);
void mantLockRenderer(u32);
void mantUnlockRenderer(u32);
void mantConstructRenderState(mantHD::RenderState*);
void mantApplyRenderState(mantHD::RenderState*);
u32 mantFindContainingHeap(u32, u32);
u32 mantGetCurrentHeap(u32);
void mantLoadTexture(mantHD::Texture*, u32, u32);
void mantConstructIndexBuffer(mantHD::IndexBuffer*);
void mantDestroyIndexBuffer(mantHD::IndexBuffer*, u32);
void mantSetIndexBuffer(mantHD::IndexBuffer*, u32, u32, u32);
void mantConstructVertexBuffer(mantHD::GpuVertexBuffer*);
void mantDestroyVertexBuffer(mantHD::GpuVertexBuffer*, u32);
void mantFlushVertexBuffer(mantHD::GpuVertexBuffer*, u32, u32);
s32 mantFindShaderProgram(u32, u32);
void mantSetShaderMode(u32);
void mantActivateShader(u32);
void mantUpdateShader(u32, u32);
void mantSetSamplerTexture(mantHD::Sampler*, mantHD::Texture*);
void mantActivateSampler(mantHD::Sampler*, u32, u32, u32);
void mantReleaseUniformBuffer(mantHD::UniformEntry*);
void mantConstructAttributeBuffer(mantHD::AttributeBuffer*);
void mantReleaseAttributeBuffer(mantHD::AttributeBuffer*);
void mantDestroyAttributeBuffer(mantHD::AttributeBuffer*, u32);
void mantActivateAttributeBuffer(mantHD::AttributeBuffer*);
void mantEntryImmediate(u32, void*, u32);
void mantConstructJ3DPacket(u32);
void mantDestroyJ3DPacket(u32, u32);
u32 mantGetShaderCache(u32);
void mantConstructUniformBlock(void*);
void mantDestroyUniformBlock(void*, u32);
void mantSetUniformBlockSlots(void*, u32);
void mantFlushUniformBlock(void*);
void mantConstructLightBuffer(mantHD::LightBuffers*);
void mantDestroyLightBuffer(mantHD::LightBuffers*, u32);
void mantSetLightBufferSlots(mantHD::LightBuffers*, u32, u32);
void mantSetLightBuffer(mantHD::LightBuffers*, u32, u32, u32, u32);
void mantFlushLightBuffer(mantHD::LightBuffers*);
void mantSetVertexBuffer(mantHD::GpuVertexBuffer*, u32, u32, u32);
void mantBuildAttributeBuffer(u32, u32, u32, u32, u32);
u32 mantGetShaderManager();
void mantSetPassUniforms(u32, u32);
void mantConfigureRenderState(u32, mantHD::RenderState*);
void mantInitializePrograms(mantHD::ProgramList*, u32, u32);
void mantInitializeShaderResource(u32, u32, u32);
void mantAddVector(cXyz*, cXyz*, cXyz*);
void mantConstructArray(u32, u32, u32, u32);
void mantDestroyArray(u32, u32, u32, u32, u32, u32);
f32 mantSqrt(f32);
f32 mantPow(f32, f32);
void mantZeroMemory(u32, u32);
void mantGX2CallDisplayList(u32, u32);
void mantGX2DrawIndexed(u32, u32, u32, u32, u32, u32);
void mantGX2SetGeometryUniformBlock(u32, u32, u32);
void mantGX2SetPixelUniformBlock(u32, u32, u32);
void mantGX2SetVertexUniformBlock(u32, u32, u32);
void mantDCStoreRange(u32, u32);

static Mtx34* mantCalcMatrix;
static cXyz* mantOutput;
static u32 mantOutputCount;
static u32 mantCollisionIndex;
static cXyz mantHiddenPosition;
static mantHD::SinCosTable mantSinCos;
static mantHD::FloatTable mantWindScale;
static mantHD::FloatTable mantGustScale;
static f32 mantRowPhase;
static f32 mantWindTarget;
static f32 mantWindStep;
static f32 mantWindDecay;
static s16 mantWindDuration;
static u32 mantVertexCount;
static u32 mantIndexCount;
static mantHD::VertexIndices mantVertexIndices;
static mantHD::TextureCoordinates mantTextureCoordinates;
static f32 mantMaterialBrightness;
static mantHD::TextureNames mantTextureNames;
static void* mantHeapManager;
static void* mantRenderer;
static void* mantResources;
static void* mantDrawBuffer;
const gc_tu_data gc_tu = {
    .safeString = 0x10014574,
    .gndChk = {0x1001459C, 0x100145AC, 0x100145CC, 0x100145BC},
    .gndChkDt = {0x100145AC, 0x100145CC, 0x1001458C},
};

static constexpr u32 kMantVtable = 0x100145DC;   // HD: Cape actor virtual destructor.
static constexpr u32 kPacketVtable = 0x10014714; // HD: daMant_packet_c vtable.

static dCcD_SrcSph wind_cc_sph_src;
static dCcD_SrcSph mesh_cc_sph_src;

// HD: Native packet and heap bindings.
/* 027F0E04 J3DDrawBuffer::entryImm(J3DPacket*, u16) */
static inline void J3DDrawBuffer_entryImm(u32 buf, void* packet, u32 idx) {
    mantEntryImmediate(buf, packet, idx);
}
/* j3dSys opa draw buffer (HD) */
static inline u32 j3dSys_getDrawBuffer0() {
    return gabi::ea(mantDrawBuffer);
}
/* 028EFFD0 __construct_array(p, n, size, ctor), 028F0164 __destroy_arr(p, n, size, dtor) */
static inline void __construct_array(u32 p, u32 n, u32 size, u32 ctor) {
    mantConstructArray(p, n, size, ctor);
}
static inline void __destroy_arr(u32 p, u32 n, u32 size, u32 dtor) {
    mantDestroyArray(p, n, size, dtor, 0, 0);
}
static inline void fopAcM_setCullSizeBoxMin(fopAc_ac_c* a, f32 x, f32 y, f32 z) {
    mantSetCullMinimum(a, x, y, z);
}
static inline void fopAcM_setCullSizeBoxMax(fopAc_ac_c* a, f32 x, f32 y, f32 z) {
    mantSetCullMaximum(a, x, y, z);
}
/* sead heap free: findContainHeap(mgr, ptr) (02755FEC on the heap manager at *0x101F8B4C), then
 * heap->free(ptr) (vtable at +0xC, slot +0x3C) */
static inline void sead_free(u32 field) {
    u32 heap = mantFindContainingHeap(gabi::ea(mantHeapManager),
                                      gabi::ea(gabi::at<gptr<void>>(field)->get()));
    u32 vt = (u32)gabi::at<mantHD::VirtualObject>(heap)->mVtable;
    u32 fn = (u32)gabi::at<mantHD::HeapVtable>(vt)->mFree;
    gabi::call_ptr(
        fn, heap,
        gabi::ea(gabi::at<gptr<void>>(field)->get())); /* the pointer is re-read after the call */
}

using CapeType = mant_class::CapeType;
static constexpr CapeType Type_PHANTOM_GANON_e = mant_class::Type_PHANTOM_GANON_e;

static inline u32 packet(mant_class* i_this) {
    return gabi::ea(i_this) + 0x3B4;
}

/* 021BBAD0 */
static void mant_mtx_copy(void* dst, const void* src) {
    const Mtx34* source = static_cast<const Mtx34*>(src);
    Mtx34* destination = static_cast<Mtx34*>(dst);
    f32 values[3][4];
    for (u32 row = 0; row < 3; ++row)
        for (u32 column = 0; column < 4; ++column)
            values[row][column] = source->m[row][column];
    for (u32 row = 0; row < 3; ++row)
        for (u32 column = 0; column < 4; ++column)
            destination->m[row][column] = values[row][column];
}

/* 021BBB70 */
static void mant_color_f(void* dst, const void* src) {
    const GXColorS10* source = static_cast<const GXColorS10*>(src);
    f32 red = (f32)source->r / 255.0f;
    f32 green = (f32)source->g / 255.0f;
    f32 blue = (f32)source->b / 255.0f;
    f32 alpha = (f32)source->a / 255.0f;
    mantHD::Color* destination = static_cast<mantHD::Color*>(dst);
    destination->component[0] = red;
    destination->component[1] = green;
    destination->component[2] = blue;
    destination->component[3] = alpha;
}

static inline void mant_copy_packet_matrix(u32 src, u32 dst) {
    gabi::Local<Mtx34> matrix;
    const Mtx34* source = gabi::at<Mtx34>(src);
    f32 values[3][4];
    for (u32 row = 0; row < 3; ++row)
        for (u32 column = 0; column < 4; ++column)
            values[row][column] = source->m[row][column];
    for (u32 row = 0; row < 3; ++row)
        for (u32 column = 0; column < 4; ++column)
            matrix->m[row][column] = values[row][column];
    gabi::Local<be<u32>[4]> linkage;
    PSMTXCopy(matrix.get(), gabi::at<Mtx34>(dst));
}

/* 021BBC34 */
void daMant_packet_c::draw() {
    daMant_packet_c* packet = this;
    u32 active = (u32)packet->mVertices.mActive;
    u32 vertices = gabi::ea(packet->mVertices.mBuffer[active].mpVertices.get());
    u32 end = vertices + 0x1200;
    if (vertices < end) {
        for (u32 line = vertices; line < end; line += 0x20)
            for (u32 k = 0; k < 8; ++k)
                gabi::store<u32>((line & ~31u) + k * 4, 0);
        active = (u32)packet->mVertices.mActive;
    }
    vertices = gabi::ea(packet->mVertices.mBuffer[active].mpVertices.get());
    for (u32 k = 0; k < mantVertexCount; ++k) {
        const mantHD::VertexIndices::Entry* indices = &mantVertexIndices.value[k];
        const cXyz* position = &packet->mPos[(u8)indices->position];
        f32 x = position->x, y = position->y, z = position->z;
        mantHD::Vertex* out = gabi::at<mantHD::Vertex>(vertices + k * sizeof(mantHD::Vertex));
        out->mPos.x = x;
        out->mPos.y = y;
        out->mPos.z = z;
        const cXyz* normal = &packet->mNrm[(u8)indices->normal];
        x = normal->x;
        y = normal->y;
        z = normal->z;
        out->mNrm.x = x;
        out->mNrm.y = y;
        out->mNrm.z = z;
        const mantHD::TextureCoordinates::Entry* uv =
            &mantTextureCoordinates.value[(u8)indices->texture];
        x = uv->u;
        y = uv->v;
        out->mU = x;
        out->mV = y;
    }
    active = packet->mVertices.mActive;
    mantHD::VertexBuffer* buffer = &packet->mVertices.mBuffer[active];
    u32 dirty = buffer->mBuffer.mDirty;
    mantFlushVertexBuffer(&buffer->mBuffer, 0, dirty);
    active = packet->mVertices.mActive;
    packet->mVertices.mActive = active == 0;
    mantSetupTevColor(reinterpret_cast<dKy_tevstr_c*>(packet->mpTevStr.get()));
    mantSetupTevLight(reinterpret_cast<dKy_tevstr_c*>(packet->mpTevStr.get()));
    gabi::Local<Mtx34> camera;
    gabi::Local<be<f32>[4]> color, scaledA, scaledB;
    gabi::Local<be<u32>[4]> linkage;
    mant_mtx_copy(camera.get(), gabi::at<void>(0x104B45F8));
    u32 view = gabi::load<u32>(0x104B4708);
    mantSetLightBuffer(&packet->mLightBuffers, 0, gabi::ea(camera.get()), 0x104B470C, view + 0x240);
    for (u32 k = 0; k < 2; ++k) {
        u32 tev = gabi::ea(packet->mpTevStr.get());
        u32 uniform = gabi::ea(packet->mLightBuffers.mpBuffers.get());
        mant_color_f(color.get(), gabi::at<void>(tev + (k ? 0x160 : 0x90)));
        tev = gabi::ea(packet->mpTevStr.get());
        f32 factor = (k ? (f32)gabi::at<mantHD::TevLightParameters>(tev)->mSecondLightScale
                        : (f32)gabi::at<mantHD::TevLightParameters>(tev)->mLightScale);
        u32 scaled = gabi::ea(k ? scaledB.get() : scaledA.get());
        mantMultiplyColor(gabi::at<mantHD::Color>(scaled),
                          reinterpret_cast<mantHD::Color*>(color.get()), factor);
        // HD: Uniform colors use word copies to preserve signaling NaN payloads.
        be<u32>* destination =
            reinterpret_cast<be<u32>*>(&gabi::at<mantHD::UniformBuffer>(uniform)->mColor[k]);
        const be<u32>* source = gabi::at<be<u32>>(scaled);
        for (u32 j = 0; j < 4; ++j)
            destination[j] = source[j].get();
    }
    mantFlushLightBuffer(&packet->mLightBuffers);
    if ((u8)packet->mType == 1) {
        for (u32 colorIndex = 0; colorIndex < 2; ++colorIndex)
            for (u32 component = 0; component < 4; ++component)
                packet->mMaterialBlock[2].mColor[colorIndex].component[component] = 1.0f;
        u32 tev = gabi::ea(packet->mpTevStr.get());
        mantScaleColor(&packet->mMaterialBlock[2].mColor[1],
                       (f32)gabi::at<mantHD::TevLightParameters>(tev)->mMaterialScale);
        mantFlushUniformBlock(&packet->mMaterialBlock[2]);
        packet->mMatrixBlock[2].mEnabled = 0;
        mant_copy_packet_matrix(gabi::ea(&packet->mMtx), gabi::ea(&packet->mMatrixBlock[2].mMtx));
        mantFlushUniformBlock(&packet->mMatrixBlock[2]);
    } else {
        u32 tev = gabi::ea(packet->mpTevStr.get());
        f32 rgba[4];
        for (u32 j = 0; j < 4; ++j)
            rgba[j] =
                (f32) reinterpret_cast<const be<s16>*>(&gabi::at<dKy_tevstr_c>(tev)->mColorC0)[j]
                    .get() /
                255.0f;
        for (u32 j = 0; j < 4; ++j)
            packet->mMaterialBlock[0].mColor[0].component[j] = rgba[j];
        for (u32 j = 0; j < 4; ++j)
            rgba[j] =
                (f32) reinterpret_cast<const be<u8>*>(&gabi::at<dKy_tevstr_c>(tev)->mColorK0)[j]
                    .get() /
                255.0f;
        for (u32 j = 0; j < 4; ++j)
            packet->mMaterialBlock[0].mColor[1].component[j] = rgba[j];
        mantScaleColor(&packet->mMaterialBlock[0].mColor[1],
                       (f32)gabi::at<mantHD::TevLightParameters>(tev)->mMaterialScale);
        u8 alpha = (u8)gabi::at<dKy_tevstr_c>(tev)->mColorK1.a;
        if (alpha) {
            for (u32 j = 0; j < 3; ++j)
                rgba[j] =
                    (f32) reinterpret_cast<const be<u8>*>(&gabi::at<dKy_tevstr_c>(tev)->mColorK1)[j]
                        .get() /
                    255.0f;
            rgba[3] = (f32)alpha / 255.0f;
        } else
            for (u32 j = 0; j < 4; ++j)
                rgba[j] = 0.0f;
        for (u32 j = 0; j < 4; ++j)
            packet->mMaterialBlock[0].mColor[2].component[j] = rgba[j];
        tev = gabi::ea(packet->mpTevStr.get());
        mantScaleColor(&packet->mMaterialBlock[0].mColor[1],
                       (f32)gabi::at<mantHD::TevLightParameters>(tev)->mMaterialScale);
        mantFlushUniformBlock(&packet->mMaterialBlock[0]);
        packet->mMatrixBlock[0].mEnabled = 0;
        mant_copy_packet_matrix(gabi::ea(&packet->mMtx), gabi::ea(&packet->mMatrixBlock[0].mMtx));
        mantFlushUniformBlock(&packet->mMatrixBlock[0]);
        f32 gamma = mantPow(mantMaterialBrightness, 2.2f);
        for (u32 j = 0; j < 3; ++j)
            packet->mMaterialBlock[1].mColor[0].component[j] = gamma;
        packet->mMaterialBlock[1].mColor[0].component[3] = 1.0f;
        for (u32 j = 0; j < 3; ++j)
            packet->mMaterialBlock[1].mColor[1].component[j] = 0.0f;
        packet->mMaterialBlock[1].mColor[1].component[3] = 1.0f;
        tev = gabi::ea(packet->mpTevStr.get());
        mantScaleColor(&packet->mMaterialBlock[1].mColor[1],
                       (f32)gabi::at<mantHD::TevLightParameters>(tev)->mMaterialScale);
        mantFlushUniformBlock(&packet->mMaterialBlock[1]);
        packet->mMatrixBlock[1].mEnabled = 0;
        mant_copy_packet_matrix(gabi::ea(&packet->mMtx2), gabi::ea(&packet->mMatrixBlock[1].mMtx));
        mantFlushUniformBlock(&packet->mMatrixBlock[1]);
    }
}

/* 021BC2C8 */
static BOOL daMant_Draw(mant_class* i_this) {
    if (i_this->scale.y > 0.01f) {
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
        MtxTrans(0.0f, 0.0f, 0.0f, 0);
        PSMTXCopy(calc_mtx(), &i_this->mPacket.mMtx); /* mpacket.getMtx() */
        if (i_this->mType != Type_PHANTOM_GANON_e) {
            MtxTrans(0.0f, -3.0f, 0.0f, 0);
            PSMTXCopy(calc_mtx(), &i_this->mPacket.mMtx2); /* getMtx2() */
        }
        i_this->mPacket.mpTevStr = &i_this->tevStr; /* setTevStr */
        J3DDrawBuffer_entryImm(j3dSys_getDrawBuffer0(), gabi::at<u8>(packet(i_this)), 0);
        mantDrawPacket(gabi::at<daMant_packet_c>(
            packet(i_this))); /* HD: daMant_packet_c::draw() called directly */
    }
    return TRUE;
}

static inline f32 mant_sin(s32 angle) {
    return mantSinCos.value[(u16)angle >> 3].mSin.get();
}
static inline void mant_copy_vec(u32 dst, u32 src) {
    // HD: Copy each word before reading the next, including signaling NaNs.
    be<u32>* destination = gabi::at<be<u32>>(dst);
    const be<u32>* source = gabi::at<be<u32>>(src);
    for (u32 k = 0; k < 3; ++k)
        destination[k] = source[k].get();
}
static inline void mant_scale_vec(u32 src, u32 dst, f32 scale) {
    mantScaleVector(gabi::at<cXyz>(src), gabi::at<cXyz>(dst), scale);
}
static inline void mant_ground_finish(dBgS_GndChk* ground) {
    ground->dBgS_PolyPassChk::__vtbl = gc_tu.gndChkDt[1];
    ground->cBgS_PolyInfo::__vtbl = gc_tu.gndChkDt[0];
    ground->dBgS_GrpPassChk::__vtbl = gc_tu.gndChkDt[2];
    mantDestroyGround(ground, 0);
}

/* 021BC3C4 */
static BOOL daMant_Execute(mant_class* self) {
    u32 p = gabi::ea(self);
    dComIfG_inf_c* play = nullptr;
    if (!((f32)self->scale.y > 0.01f))
        return TRUE;
    mantGetGameInfo();
    self->mTick = (u32)self->mTick + 1;
    s16 hitDelay = (s16)self->m2836;
    if (hitDelay)
        self->m2836 = hitDelay - 1;
    mantOutputCount = 0;
    mantOutput = &self->mPacket.mPos[0];
    mantCollisionIndex = 0;
    gabi::Local<csXyz> angle, particleAngle;
    gabi::Local<cXyz> delta, bend, offset, wind, turbulence, waveInput, waveOutput;
    gabi::Local<cXyz> scaled, forceA, forceB, forceC, tangent, normal;
    gabi::Local<dBgS_GndChk> ground;
    gabi::Local<mantHD::AttackInfo> attack;
    gabi::Local<be<u32>[4]> linkage;

    mantSetAngles(gabi::ea(angle.get()), 0, 0, 0);
    mantSubtractVector(&self->m1BE0, delta.get(), &self->m1BEC);
    s32 heading = mantAtan2s(delta->x.get(), delta->z.get());
    angle->y = heading + 0x4000;
    for (u32 row = 0; row < 9; ++row) {
        mant_j_s* joint = &self->mJoint[row];
        u32 strip = gabi::ea(joint);
        f32 factor = (f32)row;
        f32 x = gabi::fmadds(delta->x.get() * 0.125f, factor, (f32)self->m1BEC.x);
        joint->mPos[0].x = x;
        f32 y = gabi::fmadds(delta->y.get() * 0.125f, factor, (f32)self->m1BEC.y);
        joint->mPos[0].y = y;
        f32 z = gabi::fmadds(delta->z.get() * 0.125f, factor, (f32)self->m1BEC.z);
        joint->mPos[0].z = z;
        mantRotateY(mantCalcMatrix, (s32)(s16)angle->y);
        s32 phase = mantFloatToAngle(factor * mantRowPhase);
        f32 curve = mant_sin(phase);
        bend->x = 0.0f;
        bend->y = curve * -10.0f;
        bend->z = curve * -20.0f;
        mantTransformVector(bend.get(), scaled.get());
        mantAddVector(gabi::at<cXyz>(strip), scaled.get(), gabi::at<cXyz>(strip));
        joint->mAngle.x = (s16)angle->x;
        joint->mAngle.y = (s16)angle->y - 12000 + row * 3000;
        joint->mAngle.z = (s16)angle->z;
        mantConstructGround(ground.get());
        ground->dBgS_GrpPassChk::mGrp = 1;
        ground->dBgS_PolyPassChk::__vtbl = gc_tu.gndChk.v40;
        ground->mbObjThrough = 0;
        ground->mbCamThrough = 0;
        ground->mbLinkThrough = 0;
        ground->mbArrowThrough = 0;
        ground->mbBombThrough = 0;
        ground->mbBoomerangThrough = 0;
        ground->mbRopeThrough = 0;
        ground->cBgS_PolyInfo::__vtbl = gc_tu.gndChk.v20;
        ground->cBgS_Chk::__vtbl = gc_tu.gndChk.v10;
        ground->dBgS_GrpPassChk::__vtbl = gc_tu.gndChk.v4C;
        ground->mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk*>(ground.get()));
        ground->mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk*>(ground.get()));
        ground->m_pos.x = (f32)joint->mPos[0].x;
        ground->m_pos.y = (f32)joint->mPos[0].y + 50.0f;
        ground->m_pos.z = (f32)joint->mPos[0].z;
        play = gabi::at<dComIfG_inf_c>(mantGetGameInfo());
        f32 floor = mantGroundCross(&play->play.mBgS, ground.get()) + 1.5f;
        f32 anchorY = (f32)joint->mPos[0].y;
        if (floor - anchorY > 50.0f)
            floor = anchorY;
        waveInput->x = 0.0f;
        waveInput->y = 0.0f;
        waveInput->z = 0.0f;
        waveOutput->x = 0.0f;
        waveOutput->y = 0.0f;
        waveOutput->z = 0.0f;
        f32 turbulenceX = 0.0f, turbulenceZ = 0.0f, windX = 0.0f, windZ = 0.0f, lift = 0.0f;

        mantRotateY(mantCalcMatrix, (s32)(s16)joint->mAngle.y);
        offset->x = 0.0f;
        offset->y = 0.0f;
        offset->z = -5.0f;
        mantTransformVector(offset.get(), wind.get());
        s16 windHeading = (s16)self->mWindHeading - 12000 + row * 3000;
        mantRotateY(mantCalcMatrix, (s32)windHeading);
        offset->x = 0.0f;
        offset->y = 0.0f;
        offset->z = (f32)self->mWindAmplitude;
        s16 difference = windHeading - (s16)joint->mAngle.y;
        if (difference < 0)
            difference = (s16)-difference;
        f32 windStrength;
        if ((u16)difference < 0x4000) {
            offset->z = offset->z.get() * 0.05f;
            windStrength = 0.0f;
        } else
            windStrength = 1.0f;
        mantTransformVector(offset.get(), turbulence.get());
        offset->x = 0.0f;
        offset->y = 0.0f;
        offset->z = ((f32)self->m1BF8 + mant_sin(row * 23000)) * (f32)self->scale.y;
        bool severed = false;
        for (u32 node = 0; node < 9; ++node) {
            cXyz* position = &joint->mPos[node];
            cXyz* speed = &joint->mVelocity[node];
            u32 point = gabi::ea(position), velocity = gabi::ea(speed);
            if (node) {
                mant_scale_vec(gabi::ea(wind.get()), gabi::ea(scaled.get()),
                               mantWindScale.value[node].get());
                f32 forceX = scaled->x.get(), forceZ = scaled->z.get();
                f32 amplitude = (f32)self->mWindAmplitude;
                f32 gust = (f32)self->mWindGust;
                if (__builtin_fabsf(amplitude) > 0.1f) {
                    u32 tick = (u32)self->mTick;
                    lift =
                        (mant_sin((tick << 12) - node * 10000 + row * 10000) * (amplitude * 0.5f)) *
                        gabi::fmadds((f32)node, 0.1f, 1.0f);
                    windZ = turbulence->z.get();
                    windX = turbulence->x.get();
                }
                if (gust > 0.01f) {
                    mant_scale_vec(gabi::ea(wind.get()), gabi::ea(forceA.get()),
                                   mantGustScale.value[node].get());
                    mant_scale_vec(gabi::ea(forceA.get()), gabi::ea(forceB.get()),
                                   (f32)self->mWindGust);
                    mant_scale_vec(gabi::ea(forceB.get()), gabi::ea(forceC.get()), windStrength);
                    turbulenceZ = forceC->z.get();
                    turbulenceX = forceC->x.get();
                }
                if ((u8)self->mType == 1) {
                    u32 tick = (u32)self->mTick;
                    waveInput->z = 2.0f * mant_sin((tick << 11) - node * 10000 + row * 10000);
                    mantRotateY(mantCalcMatrix, (s32)(s16)joint->mAngle.y);
                    mantTransformVector(waveInput.get(), waveOutput.get());
                }
                f32 dx = (f32)position->x - (f32)position[-1].x;
                f32 dz = (f32)position->z - (f32)position[-1].z;
                f32 dy = (f32)position->y + (f32)speed->y;
                dz = dz + (f32)speed->z;
                dx = dx + (f32)speed->x;
                dz = dz + forceZ;
                dx = dx + forceX;
                dy = dy + (f32)self->mGravity;
                dz = dz + windZ;
                dx = dx + windX;
                dy = dy + lift;
                dz = dz + turbulenceZ;
                dx = dx + turbulenceX;
                dz = dz + waveOutput->z.get();
                dx = dx + waveOutput->x.get();
                if (dy < floor)
                    dy = floor;
                f32 height = dy - (f32)position[-1].y;
                s16 pitch = (s16)-mantAtan2s(height, dz);
                f32 distance = mantSqrt(gabi::fmadds(height, height, dz * dz));
                s32 yaw = mantAtan2s(dx, distance);
                mantRotateX(mantCalcMatrix, (s32)pitch);
                mantMultiplyRotateY(mantCalcMatrix, yaw);
                mantTransformVector(offset.get(), normal.get());
                mant_copy_vec(velocity, point);
                x = (f32)position[-1].x + normal->x.get();
                y = (f32)position[-1].y + normal->y.get();
                z = (f32)position[-1].z + normal->z.get();
                position->x = x;
                position->y = y;
                position->z = z;
                for (u32 j = 0; j < 3; ++j) {
                    f32 previous = reinterpret_cast<be<f32>*>(speed)[j].get();
                    f32 next = reinterpret_cast<be<f32>*>(position)[j].get();
                    reinterpret_cast<be<f32>*>(speed)[j] = (next - previous) * (f32)self->m1BFC;
                }
            }
            u32 output = gabi::ea(mantOutput);
            mant_copy_vec(output, point);
            output = gabi::ea(mantOutput);
            u32 count = mantOutputCount;
            mantOutput = gabi::at<cXyz>(output + 12);
            mantOutputCount = count + 1;
            if ((u8)self->mType == 0 && ((node | row) & 1) == 0 && node != 0 && node != 8 &&
                row != 0 && row != 8) {
                u32 collision = mantCollisionIndex;
                u32 spheres = gabi::ea(&self->mMeshSph[0]);
                bool visible = (s8)(u8)self->m2834 != 0;
                self->mMeshSph[collision].SetR(visible ? 30.0f : -200.0f);
                collision = mantCollisionIndex;
                self->mMeshSph[collision].SetC(visible ? position : &mantHiddenPosition);
                play = gabi::at<dComIfG_inf_c>(mantGetGameInfo());
                collision = mantCollisionIndex;
                mantRegisterCollision(&play->play.mCcS, spheres + collision * 0x12C);
                if (!(s16)self->m2836 && !(s16)self->mBurnTimer) {
                    collision = mantCollisionIndex;
                    if (mantCheckTargetHit(&self->mMeshSph[collision])) {
                        self->m2836 = 10;
                        collision = mantCollisionIndex;
                        u32 object = mantGetTargetHitObject(&self->mMeshSph[collision]);
                        u32 hit = gabi::ea(attack.get());
                        attack->mpObject = object;
                        mantGetHitType(hit);
                        play = gabi::at<dComIfG_inf_c>(mantGetGameInfo());
                        u8 hitType = (u8)attack->mHitType;
                        u32 player = gabi::ea(play->play.mPlayerInfo[0].mpPlayer.get());
                        if ((hitType == 1 || hitType == 2) && !self->mPacket.mTexNo.get()) {
                            particleAngle->x = (s16)self->current.angle.x;
                            particleAngle->y = (s16)self->current.angle.y - 0x8000;
                            s32 room = (s8)(u8)self->current.roomNo;
                            self->m2834 = 0;
                            self->mPacket.mTexNo = hitType == 2 ? 2 : 4;
                            particleAngle->z = (s16)self->current.angle.z;
                            play = gabi::at<dComIfG_inf_c>(mantGetGameInfo());
                            u32 particles = gabi::ea(play->play.mParticle.get());
                            mantSetParticle(particles, 0, 0x81B6, point,
                                            gabi::ea(particleAngle.get()), 0, 0xFF, 0, room,
                                            p + 0x1A8, p + 0x1A8, 0);
                            if (p && p + 0x314) {
                                s32 reverb = mantGetReverb((s32)(s8)(u8)self->current.roomNo);
                                mantStartSound(0x58ED, p + 0x314, 0, reverb);
                            }
                            hitType = (u8)attack->mHitType;
                        }
                        self->m1C0E = 2;
                        s16 direction = gabi::at<fopAc_ac_c>(player)->shape_angle.y.get();
                        self->mWindAmplitude = 10.0f;
                        self->mWindHeading = direction;
                        if (hitType == 5) {
                            self->m1C0E = 6;
                            self->mBurnTimer = 26;
                            severed = true;
                            break;
                        }
                    }
                }
                u32 collisionNext = mantCollisionIndex;
                mantCollisionIndex = collisionNext + 1;
                if (node == 4 && row == 4) {
                    self->mWindSph.SetR(200.0f);
                    self->mWindSph.SetC(position);
                    play = gabi::at<dComIfG_inf_c>(mantGetGameInfo());
                    mantRegisterCollision(&play->play.mCcS, gabi::ea(&self->mWindSph));
                }
            }
        }
        mant_ground_finish(ground.get());
        (void)severed;
    }
    offset->x = 0.0f;
    offset->y = 0.0f;
    offset->z = 1.0f;
    mantRotateY(mantCalcMatrix, 0x4FA0);
    mantTransformVector(offset.get(), waveInput.get());
    for (u32 k = 0; k < 81; ++k) {
        u32 out = gabi::ea(&self->mPacket.mNrm[0]) + k * 12;
        if (k < 72 && k % 9 == 8)
            mant_copy_vec(out, out - 12);
        else if (k >= 72)
            mant_copy_vec(out, out - 0x6C);
        else {
            u32 point = gabi::ea(&self->mPacket.mPos[0]) + k * 12;
            mantSubtractVector(gabi::at<cXyz>(point + 0x78), tangent.get(), gabi::at<cXyz>(point));
            f32 dx = tangent->x.get(), dz = tangent->z.get();
            f32 dy = tangent->y.get();
            u32 matrix = gabi::ea(mantCalcMatrix);
            s32 yaw = mantAtan2s(dx, dz);
            mantRotateY(gabi::at<Mtx34>(matrix), yaw);
            matrix = gabi::ea(mantCalcMatrix);
            f32 length = mantSqrt(gabi::fmadds(dx, dx, dz * dz));
            s16 pitch = (s16)-mantAtan2s(dy, length);
            mantMultiplyRotateZ(gabi::at<Mtx34>(matrix), (s32)pitch);
            mantTransformVector(waveInput.get(), gabi::at<cXyz>(out));
        }
    }
    mantDCStoreRange(gabi::ea(&self->mPacket.mPos[0]), 0x3CC);
    bool deleting = false;
    s16 burn = (s16)self->mBurnTimer;
    if (burn) {
        self->mBurnTimer = burn - 1;
        for (u32 k = 0; k < 81; k += 4) {
            play = gabi::at<dComIfG_inf_c>(mantGetGameInfo());
            u32 particles = gabi::ea(play->play.mParticle.get());
            mantSetSimpleParticle(particles, 0x8069, gabi::ea(&self->mPacket.mPos[0]) + k * 12,
                                  0xFF, 0x101D5E98, 0x101D5E98, 0);
        }
        if ((s16)self->mBurnTimer <= 2) {
            mantDeleteActor(reinterpret_cast<fopAc_ac_c*>(self));
            deleting = true;
        }
    }
    if (!deleting) {
        cLib_addCalc2(&self->m1BF8, 30.0f, 0.1f, 1.0f);
        cLib_addCalc2(&self->m1BFC, 0.7f, 0.1f, 0.05f);
        if (mantCheckTargetHit(&self->mWindSph)) {
            self->m1C0E = mantWindDuration + 5;
            dComIfG_inf_c* play = gabi::at<dComIfG_inf_c>(mantGetGameInfo());
            self->mWindHeading = play->play.mPlayerInfo[0].mpPlayer->shape_angle.y.get();
        }
        {
            s16 windTimer = (s16)self->m1C0E;
            if (windTimer) {
                self->m1C0E = windTimer - 1;
                f32 target = mantWindTarget + 50.0f;
                f32 step = mantWindStep + 10.0f;
                cLib_addCalc2(&self->mWindAmplitude, target, 0.2f, step);
            } else {
                f32 step = mantWindDecay + 2.0f;
                cLib_addCalc0(&self->mWindAmplitude, 0.1f, step);
            }
            if ((s16)self->m1C0E <= 4)
                cLib_addCalc0(&self->mWindGust, 0.1f, 0.1f);
            else
                cLib_addCalc2(&self->mWindGust, 1.0f, 1.0f, 1.0f);
        }
    }
    f32 posX = (f32)self->current.pos.x, posZ = (f32)self->current.pos.z,
        posY = (f32)self->current.pos.y;
    MtxTrans(posX, posY, posZ, 0);
    PSMTXCopy(mantCalcMatrix, &self->mMtx);
    return TRUE;
}

/* 021BD490 */
static BOOL daMant_IsDelete(mant_class*) {
    return TRUE;
}

/* 021BD498 */
static BOOL daMant_Delete(mant_class*) {
    return TRUE;
}

/* 021BE134 */
static cPhs_State daMant_Create(fopAc_ac_c* a_this) {
    mant_class* i_this = (mant_class*)a_this;

    /* fopAcM_ct(a_this, mant_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = kMantVtable;
            mantConstructPacket(
                gabi::at<daMant_packet_c>(packet(i_this))); /* daMant_packet_c::daMant_packet_c */
            dCcD_Stts_ct(&i_this->mStts);
            mantConstructSphere(&i_this->mWindSph); /* dCcD_Sph::dCcD_Sph */
            __construct_array(gabi::ea(&i_this->mMeshSph[0]), 9, 0x12C, 0x025166F0);
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    i_this->mType = (u8)fopAcM_GetParam(a_this);
    fopAcM_setCullSizeBoxMin(a_this, -2000.0f, -2000.0f, -2000.0f);
    fopAcM_setCullSizeBoxMax(a_this, 2000.0f, 2000.0f, 2000.0f);
    u8 type = i_this->mType;
    a_this->cullMtx = gabi::ea(&i_this->mMtx);
    i_this->mPacket.mType = type; /* mpacket.setType */

    bool warmPhantom;
    if (type == Type_PHANTOM_GANON_e) {
        u8 t = i_this->mType;
        i_this->mPacket.mTexNo = 6; /* mpacket.setarg0 */
        i_this->m1BF8 = 40.0f;
        i_this->m1BFC = 0.8f;
        i_this->mGravity = -5.0f;
        warmPhantom = t == Type_PHANTOM_GANON_e;
    } else {
        i_this->m1BFC = 0.7f;
        i_this->mGravity = -10.0f;
        i_this->m1BF8 = 30.0f;
        i_this->mStts.Init(200, 0xFF, a_this);
        i_this->mWindSph.Set(&wind_cc_sph_src);
        i_this->mWindSph.SetStts(&i_this->mStts);
        for (int i = 0; i < 9; i++) {
            i_this->mMeshSph[i].Set(&mesh_cc_sph_src);
            i_this->mMeshSph[i].SetStts(&i_this->mStts);
        }
        u8 t = i_this->mType;
        i_this->m2834 = 10;
        warmPhantom = t == Type_PHANTOM_GANON_e;
    }
    if (warmPhantom) {
        for (int i = 0; i < 10; i++) {
            mantExecuteActor(reinterpret_cast<mant_class*>(i_this)); /* daMant_Execute */
        }
        i_this->scale.z = 1.0f;
        i_this->scale.y = 0.0f;
        i_this->scale.x = 1.0f;
    }
    for (int i = 0; i < 10; i++) {
        mantExecuteActor(reinterpret_cast<mant_class*>(i_this)); /* daMant_Execute */
    }
    return cPhs_COMPLEATE_e;
}

/* Bind the three shader-stage locations captured before any GX2 call. */
static inline void mant_bind_uniform_buffer(u32 entry, u32 shader) {
    const mantHD::Program* program = gabi::at<mantHD::Program>(shader);
    mantHD::ShaderBinding* binding =
        program->mUniformCount ? program->mpUniforms.get() : gabi::at<mantHD::ShaderBinding>(0);
    const mantHD::UniformEntry* uniform = gabi::at<mantHD::UniformEntry>(entry);
    s32 vertex = (s16)binding->mVertex;
    u32 size = (u32)uniform->mSize;
    u32 data = gabi::ea(uniform->mpData.get());
    s32 pixel = (s16)binding->mPixel;
    s32 geometry = (s16)binding->mGeometry;
    if (pixel != -1)
        mantGX2SetPixelUniformBlock(pixel, data, size);
    if (vertex != -1)
        mantGX2SetVertexUniformBlock(vertex, data, size);
    if (geometry != -1)
        mantGX2SetGeometryUniformBlock(geometry, data, size);
}

/* 021BE340 */
void daMant_packet_c::setupUniforms(void* pass, void* program, u32 type) {
    daMant_packet_c* packet = this;
    u32 q = gabi::ea(pass), shader = gabi::ea(program);
    mantHD::RenderPass* renderPass = static_cast<mantHD::RenderPass*>(pass);
    u32 cache = mantGetShaderCache(0x104B45C0);
    mantHD::ShaderCache* shaderCache = gabi::at<mantHD::ShaderCache>(cache);
    mantHD::Program* shaderProgram = static_cast<mantHD::Program*>(program);
    u32 current = gabi::ea(shaderCache->mpShader.get());
    u32 selected = gabi::ea(shaderProgram->mpShader.get());
    mantHD::Shader* selectedShader = gabi::at<mantHD::Shader>(selected);
    if (selected != current) {
        u8 flags = (u8)selectedShader->mFlags;
        u32 mode = (u32)shaderCache->mMode;
        if (flags & 2) {
            selectedShader->mFlags = flags & ~2;
            mantUpdateShader(selected, 0);
        }
        u32 nextMode = (u32)selectedShader->mpMode->mMode;
        if (mode != nextMode)
            mantSetShaderMode(nextMode);
        u32 displaySize = (u32)selectedShader->mDisplayListSize;
        if (displaySize) {
            u32 display = gabi::ea(selectedShader->mpDisplayList.get());
            mantGX2CallDisplayList(display, displaySize);
            shaderCache->mMode = nextMode;
            shaderCache->mpShader = selectedShader;
        } else {
            mantActivateShader(selected);
            shaderCache->mpShader = selectedShader;
            shaderCache->mMode = nextMode;
        }
    }
    s32 stage = (s32)renderPass->mStage;
    if (stage == 0) {
        u32 block = gabi::ea(&packet->mMatrixBlock[0]) + type * 0xA8;
        u32 fn = (u32)gabi::at<mantHD::UniformVtable>(
                     (u32)gabi::at<mantHD::VirtualObject>(block)->mVtable)
                     ->mBind;
        gabi::call_ptr(fn, block, shader);
        u32 light = gabi::ea(renderPass->mpLights.get());
        if (light) {
            u32 buffer = gabi::ea(gabi::at<mantHD::LightBuffers>(light)->mpBuffers.get());
            u32 slot = (u32)gabi::at<mantHD::UniformBuffer>(buffer)->mActive;
            mant_bind_uniform_buffer(buffer + 0x10 + slot * 0x1C, shader);
        }
    } else if (stage == 1 || stage == 2) {
        u32 buffer = gabi::ea(packet->mLightBuffers.mpBuffers.get());
        u32 slot = (u32)gabi::at<mantHD::UniformBuffer>(buffer)->mActive;
        mant_bind_uniform_buffer(buffer + 0x10 + slot * 0x1C, shader);
        u32 block = gabi::ea(&packet->mMaterialBlock[0]) + type * 0x364;
        u32 fn = (u32)gabi::at<mantHD::UniformVtable>(
                     (u32)gabi::at<mantHD::VirtualObject>(block)->mVtable)
                     ->mBind;
        gabi::call_ptr(fn, block, shader);
        block = gabi::ea(&packet->mMatrixBlock[0]) + type * 0xA8;
        fn = (u32)gabi::at<mantHD::UniformVtable>(
                 (u32)gabi::at<mantHD::VirtualObject>(block)->mVtable)
                 ->mBind;
        gabi::call_ptr(fn, block, shader);
        if (stage == 2) {
            block = gabi::ea(renderPass->mpMatrixBlock.get());
            if (block) {
                fn = (u32)gabi::at<mantHD::UniformVtable>(
                         (u32)gabi::at<mantHD::VirtualObject>(block)->mVtable)
                         ->mBind;
                gabi::call_ptr(fn, block, shader);
            }
            mantSetPassUniforms(q, shader);
        }
    }
    u32 sampler = (u32)shaderProgram->mSamplerCount;
    u8 texture = (u8)packet->mTexNo;
    sampler = sampler ? gabi::ea(shaderProgram->mpSamplers.get()) : 0;
    mantActivateSampler(gabi::at<mantHD::Sampler>(gabi::ea(&packet->mSampler[0]) + texture * 0x198),
                        sampler + 4, 0, 0);
    gabi::Local<mantHD::RenderState> state;
    gabi::Local<be<u32>[4]> linkage;
    u32 st = gabi::ea(state.get());
    mantConstructRenderState(gabi::at<mantHD::RenderState>(st));
    u32 mask = (u32)state->mMask;
    state->mCullMode = 2;
    state->mAlphaFunction = 4;
    state->mAlphaTest = 1;
    state->mAlphaReference = 0.5f;
    state->mMask = (((mask & 0xFFFFFFF0) + 7) & 0xFFFFFF0F) + 0x10;
    u32 passType = (u32)renderPass->mStage;
    state->mDepthMode = type == 2 ? 2 : (type != 0);
    mantConfigureRenderState(passType, gabi::at<mantHD::RenderState>(st));
    mantApplyRenderState(gabi::at<mantHD::RenderState>(st));
    passType = (u32)renderPass->mStage;
    u32 count = packet->mPrograms[type].mCount;
    u32 index = (u32)packet->mVertices.mActive;
    u32 entries = gabi::ea(packet->mPrograms[type].mpEntries.get());
    if (passType < count)
        entries += passType * 0x14;
    mantActivateAttributeBuffer(reinterpret_cast<mantHD::AttributeBuffer*>(
        gabi::at<mantHD::ProgramEntry>(entries)->mAttributes[index == 0 ? 1 : 0].mpObjects.get()));
    u32 indexCount = (u32)packet->mIndices.mCount;
    if (indexCount) {
        u32 primitive = (u32)packet->mIndices.mPrimitive;
        u32 indices = gabi::ea(packet->mIndices.mpIndices.get());
        u32 format = (u32)packet->mIndices.mFormat;
        mantGX2DrawIndexed(format, indexCount, primitive, indices, 0, 1);
    }
}

static inline void mant_freeObjBuffer(u32 b);

static inline u32 mant_heap_alloc(u32 size, u32 align) {
    u32 heap = mantGetCurrentHeap(gabi::ea(mantHeapManager));
    u32 fn = (u32)gabi::at<mantHD::HeapVtable>((u32)gabi::at<mantHD::VirtualObject>(heap)->mVtable)
                 ->mAllocate;
    return gabi::call_ptr<u32>(fn, heap, size, align);
}

static inline void mant_init_programs(u32 dst, u32 name) {
    gabi::Local<mantHD::SafeString> key;
    key->mpString = gabi::at<char>(name);
    key->mVtable = gc_tu.safeString;
    gabi::Local<be<u32>[4]> linkage;
    u32 manager = mantGetShaderManager();
    mantHD::ShaderManager* shaderManager = gabi::at<mantHD::ShaderManager>(manager);
    u32 archive = gabi::ea(shaderManager->mpArchive.get());
    s32 index = mantFindShaderProgram(archive, gabi::ea(key.get()));
    u32 entry = 0;
    if (index >= 0) {
        u32 count = (u32)shaderManager->mCount;
        u32 base = gabi::ea(shaderManager->mpRecords.get());
        entry = base + ((u32)index < count ? (u32)index * 0x24 : 0);
        if (!(u8)gabi::at<mantHD::ShaderRecord>(entry)->mReady) {
            archive = gabi::ea(shaderManager->mpArchive.get());
            u32 records = (u32)gabi::at<mantHD::ShaderArchive>(archive)->mCount;
            u32 resource =
                (u32)index < records
                    ? gabi::ea(gabi::at<mantHD::ShaderArchive>(archive)->mpResources.get()) +
                          (u32)index * 0x84
                    : 0;
            mantInitializeShaderResource(entry, resource, 0);
            count = (u32)shaderManager->mCount;
            base = gabi::ea(shaderManager->mpRecords.get());
        }
        entry = base + ((u32)index < count ? (u32)index * 0x24 : 0);
    }
    mantInitializePrograms(gabi::at<mantHD::ProgramList>(dst), entry, 0);
}

/* 021BE814 */
void daMant_packet_c::setUniforms(void* pass) {
    daMant_packet_c* packet = this;
    mantHD::RenderPass* renderPass = static_cast<mantHD::RenderPass*>(pass);
    s32 k = (s32)renderPass->mStage;
    if (k < 4) {
        if ((u8)packet->mType == 1) {
            u32 n = (u32)packet->mPrograms[2].mCount;
            u32 e = gabi::ea(packet->mPrograms[2].mpEntries.get());
            if ((u32)k < n)
                e += k * 0x14;
            mantSetupPacketUniforms(reinterpret_cast<daMant_packet_c*>(this),
                                    reinterpret_cast<mantHD::RenderPass*>(pass),
                                    reinterpret_cast<mantHD::Program*>(
                                        gabi::at<mantHD::ProgramEntry>(e)->mpProgram.get()),
                                    2);
        } else {
            u32 n = (u32)packet->mPrograms[0].mCount;
            u32 e = gabi::ea(packet->mPrograms[0].mpEntries.get());
            if ((u32)k < n)
                e += k * 0x14;
            mantSetupPacketUniforms(reinterpret_cast<daMant_packet_c*>(this),
                                    reinterpret_cast<mantHD::RenderPass*>(pass),
                                    reinterpret_cast<mantHD::Program*>(
                                        gabi::at<mantHD::ProgramEntry>(e)->mpProgram.get()),
                                    0);
            k = (s32)renderPass->mStage;
            n = (u32)packet->mPrograms[1].mCount;
            e = gabi::ea(packet->mPrograms[1].mpEntries.get());
            if ((u32)k < n)
                e += k * 0x14;
            mantSetupPacketUniforms(reinterpret_cast<daMant_packet_c*>(this),
                                    reinterpret_cast<mantHD::RenderPass*>(pass),
                                    reinterpret_cast<mantHD::Program*>(
                                        gabi::at<mantHD::ProgramEntry>(e)->mpProgram.get()),
                                    1);
        }
    }
    mantApplyRenderState(gabi::at<mantHD::RenderState>(0x104B474C));
}

// HD: Compiler-generated and template code of this translation unit.

/* 021BD4A0 */
static void* daMant_packet_c_ct(void* self) {
    u32 p = gabi::ea(self);
    if (!p) {
        p = gabi::ea(operator_new(0x28DC));
        if (!p)
            return nullptr;
    }
    daMant_packet_c* packet = gabi::at<daMant_packet_c>(p);
    mantConstructJ3DPacket(p);
    packet->mVtable = kPacketVtable;
    __construct_array(gabi::ea(&packet->mPrograms[0]), 3, 0xC, 0x021BEA40);
    u32 db = gabi::ea(&packet->mVertices), storage = db;
    if (!storage)
        storage = gabi::ea(operator_new(0x4C0));
    if (storage) {
        __construct_array(storage, 2, 0x254, 0x021BEAB0);
        mantHD::DoubleBuffer* doubleBuffer = gabi::at<mantHD::DoubleBuffer>(storage);
        doubleBuffer->mActive = 0;
        doubleBuffer->mFormat = 0;
        doubleBuffer->mCount = 0;
        doubleBuffer->mAlignment = 0x20;
        doubleBuffer->mReady = 0;
        doubleBuffer->mBuffer[0].mpVertices = nullptr;
        doubleBuffer->mBuffer[1].mpVertices = nullptr;
    }
    mantConstructLightBuffer(&packet->mLightBuffers);
    __construct_array(gabi::ea(&packet->mMatrixBlock[0]), 3, 0xA8, 0x021BEB0C);
    __construct_array(gabi::ea(&packet->mMaterialBlock[0]), 3, 0x364, 0x021BEBA8);
    mantConstructIndexBuffer(&packet->mIndices);
    __construct_array(gabi::ea(&packet->mSampler[0]), 7, 0x198, 0x027BDF7C);
    __construct_array(gabi::ea(&packet->mTexture[0]), 7, 0x90, 0x027BE6B8);
    mant_init_programs(gabi::ea(&packet->mPrograms[0]), 0x10014668);
    mant_init_programs(gabi::ea(&packet->mPrograms[1]), 0x1001465C);
    mant_init_programs(gabi::ea(&packet->mPrograms[2]), 0x10014650);
    packet->mVertices.mFormat = 0x13;
    packet->mVertices.mpAttributes = gabi::at<void>(0x10014708);
    for (u32 k = 0; k < 2; ++k) {
        mantHD::VertexBuffer* buffer = &packet->mVertices.mBuffer[k];
        if (!buffer->mpVertices) {
            u32 mem = mant_heap_alloc(0x1200, 0x40);
            if (mem) {
                buffer->mpAllocation = gabi::at<void>(mem);
                buffer->mCapacity = 0x90;
            }
            buffer->mpVertices = buffer->mpAllocation.get();
        }
        mantSetVertexBuffer(&buffer->mBuffer, gabi::ea(buffer->mpVertices.get()), 0x90,
                            gabi::ea(&packet->mVertices.mFormat));
    }
    packet->mVertices.mCount = 0;
    packet->mVertices.mReady = 1;
    for (u32 set = 0; set < 3; ++set) {
        mantHD::ProgramList* programs = &packet->mPrograms[set];
        for (u32 n = 0; n < (u32)programs->mSize; ++n) {
            u32 count = (u32)programs->mCount;
            u32 entry = gabi::ea(programs->mpEntries.get());
            if (n < count)
                entry += n * 0x14;
            mantHD::ProgramEntry* programEntry = gabi::at<mantHD::ProgramEntry>(entry);
            u32 shader = gabi::ea(programEntry->mpProgram.get());
            programEntry->mpProgram = nullptr;
            mant_freeObjBuffer(entry + 4);
            mant_freeObjBuffer(entry + 0xC);
            programEntry->mpProgram = gabi::at<mantHD::Program>(shader);
            for (u32 k = 0; k < 2; ++k) {
                u32 object = mant_heap_alloc(0xF4, 4);
                if (object)
                    mantConstructAttributeBuffer(gabi::at<mantHD::AttributeBuffer>(object));
                if (object) {
                    programEntry->mAttributes[k].mpObjects =
                        gabi::at<mantHD::AttributeBuffer>(object);
                    programEntry->mAttributes[k].mCount = 1;
                }
            }
            for (u32 k = 0; k < 2; ++k) {
                u32 object = gabi::ea(programEntry->mAttributes[k].mpObjects.get());
                mantBuildAttributeBuffer(shader, object, db + 4 + k * 0x254, db + 0x4AC, 0);
            }
        }
    }
    mantSetLightBufferSlots(&packet->mLightBuffers, 1, 0);
    for (u32 k = 0; k < 3; ++k)
        mantSetUniformBlockSlots(gabi::at<void>(gabi::ea(&packet->mMaterialBlock[0]) + k * 0x364),
                                 0);
    for (u32 k = 0; k < 3; ++k)
        mantSetUniformBlockSlots(gabi::at<void>(gabi::ea(&packet->mMatrixBlock[0]) + k * 0xA8), 0);
    u32 count = mantIndexCount;
    mantSetIndexBuffer(&packet->mIndices, 0x101B9F18, 4, count);
    packet->mIndices.mFormat = 4;
    gabi::Local<mantHD::SafeString> archiveName, fileName;
    fileName->mVtable = gc_tu.safeString;
    fileName->mpString = gabi::at<char>(0x10014688);
    u32 resources = gabi::ea(mantResources);
    archiveName->mVtable = gc_tu.safeString;
    archiveName->mpString = gabi::at<char>(0x10014678);
    gabi::Local<be<u32>[4]> linkage;
    u32 textureArchive =
        mantGetTextureArchive(resources, gabi::ea(archiveName.get()), gabi::ea(fileName.get()), 0);
    mantLockRenderer(gabi::ea(mantRenderer));
    for (u32 k = 0; k < 7; ++k) {
        mantHD::SafeString* textureName = &mantTextureNames.value[k];
        u32 name = gabi::ea(textureName);
        const be<u8>* string = reinterpret_cast<const be<u8>*>(textureName->mpString.get());
        if (!string[0].get())
            continue;
        u32 fn = (u32)gabi::at<mantHD::StringVtable>((u32)textureName->mVtable)->mAssureTermination;
        gabi::call_ptr(fn, name);
        u32 texture = gabi::ea(&packet->mTexture[0]) + k * 0x90;
        mantLoadTexture(gabi::at<mantHD::Texture>(texture), textureArchive,
                        gabi::ea(textureName->mpString.get()));
        u32 sampler = gabi::ea(&packet->mSampler[0]) + k * 0x198;
        mantHD::Texture* capeTexture = gabi::at<mantHD::Texture>(texture);
        mantHD::Sampler* capeSampler = gabi::at<mantHD::Sampler>(sampler);
        const mantHD::TextureShape& source = capeTexture->mShape;
        const mantHD::TextureShape& destination = capeSampler->mShape;
        bool same =
            destination.mDimension == source.mDimension && destination.mWidth == source.mWidth &&
            destination.mHeight == source.mHeight && destination.mDepth == source.mDepth &&
            destination.mMipLevels == source.mMipLevels && destination.mFormat == source.mFormat &&
            destination.mSwizzle == source.mSwizzle && destination.mTileMode == source.mTileMode &&
            destination.mAA == source.mAA;
        if (!same)
            mantSetSamplerTexture(capeSampler, capeTexture);
        else {
            void* image = capeTexture->mShape.mpImage.get();
            void* mipmaps = capeTexture->mShape.mpMipmaps.get();
            capeSampler->mShape.mpImage = image;
            capeSampler->mpMipmaps = mipmaps;
            capeSampler->mpImage = image;
            capeSampler->mShape.mpMipmaps = mipmaps;
        }
    }
    mantUnlockRenderer(gabi::ea(mantRenderer));
    return gabi::at<void>(p);
}

/* 021BE8F8 */
static void __sinit_d_a_mant_cpp() {
    sinit_header_statics(0x10464F24, 0x101BA298);
    const u32 names[7] = {0x100146C8, 0x100146D4, 0x100146E0, 0x100146C4,
                          0x100146EC, 0x100146C4, 0x100146F8};
    for (int i = 0; i < 7; i++) {
        mantTextureNames.value[i].mVtable = gc_tu.safeString;
        mantTextureNames.value[i].mpString = gabi::at<char>(names[i]);
    }
    mantHiddenPosition.x = -20000.0f;
    mantHiddenPosition.y = -200000.0f;
    mantHiddenPosition.z = -100000.0f;
}

/* 021BEA2C */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

/* 021BEA40 */
static void* mantProgramList__ct(void* self) {
    u32 p = gabi::ea(self);
    if (p == 0) {
        p = gabi::ea(operator_new(0xC));
        if (p == 0)
            return gabi::at<void>(p);
    }
    gabi::at<mantHD::ProgramList>(p)->mSize = 0;
    u32 q = p + 4;
    if (q == 0) {
        q = gabi::ea(operator_new(8));
        if (q == 0)
            return gabi::at<void>(p);
    }
    gabi::at<mantHD::EntryBuffer>(q)->mpEntries = nullptr;
    gabi::at<mantHD::EntryBuffer>(q)->mCount = 0;
    return gabi::at<void>(p);
}

/* 021BEAB0 */
static void* mantVertexBuffer__ct(void* self) {
    u32 p = gabi::ea(self);
    if (p == 0) {
        p = gabi::ea(operator_new(0x254));
        if (p == 0)
            return gabi::at<void>(p);
    }
    mantHD::VertexBuffer* buffer = gabi::at<mantHD::VertexBuffer>(p);
    mantConstructVertexBuffer(&buffer->mBuffer);
    mantConstructAttributeBuffer(&buffer->mAttributes);
    buffer->mpAllocation = nullptr;
    buffer->mCapacity = 0;
    return gabi::at<void>(p);
}

/* 021BEB0C */
static void* mantMatrixBlock__ct(void* self) {
    u32 p = gabi::ea(self);
    if (p == 0) {
        p = gabi::ea(operator_new(0xA8));
        if (p == 0)
            return gabi::at<void>(p);
    }
    mantConstructUniformBlock(gabi::at<void>(p));
    gabi::at<mantHD::MatrixBlock>(p)->mVtable = 0x1016EF84;
    mantZeroMemory(p + 0x74, 0x34);
    if (p + 0x74 == 0)
        operator_new(0x30);
    return gabi::at<void>(p);
}

/* 021BEB7C */
static void* mantColor__ct(void* self) {
    if (self == nullptr)
        return operator_new(0x10);
    return self;
}

/* 021BEBA8 */
static void* mantMaterialBlock__ct(void* self) {
    u32 p = gabi::ea(self);
    if (p == 0) {
        p = gabi::ea(operator_new(0x364));
        if (p == 0)
            return gabi::at<void>(p);
    }
    mantConstructUniformBlock(gabi::at<void>(p));
    gabi::at<mantHD::MaterialBlock>(p)->mVtable = 0x1016EFB4;
    mantZeroMemory(p + 0x74, 0x2F0);
    mantHD::InitialMaterial* initial =
        gabi::at<mantHD::InitialMaterial>(p + offsetof(mantHD::MaterialBlock, mMatrices));
    for (u32 component = 0; component < 44; ++component) {
        bool one = component >= 3 && ((component - 3) & 3) == 0;
        initial->mValues[component] = one ? 1.0f : 0.0f;
    }
    __construct_array(p + 0x124, 2, 0x10, 0x021BEB7C);
    __construct_array(p + 0x144, 2, 0x10, 0x021BEB7C);
    __construct_array(p + 0x164, 2, 0x10, 0x021BEB7C);
    /* GHS's null-checked member constructors */
    for (u32 off = 0x110; off <= 0x260; off += 0x30)
        if (p + 0x74 + off == 0)
            operator_new(0x30);
    for (u32 off = 0x290; off <= 0x2E0; off += 0x10)
        if (p + 0x74 + off == 0)
            operator_new(0x10);
    return gabi::at<void>(p);
}

/* 021BEE30 */
static void mantMaterialBlock__dt(void* self, s32 flags) {
    if (self != nullptr) {
        mantDestroyUniformBlock(reinterpret_cast<void*>(self), 0);
        if (flags & 1)
            operator_delete(self);
    }
}

/* 021BEE84 */
static void mantMatrixBlock__dt(void* self, s32 flags) {
    if (self != nullptr) {
        mantDestroyUniformBlock(reinterpret_cast<void*>(self), 0);
        if (flags & 1)
            operator_delete(self);
    }
}

/* 021BEED8 */
static void mantVertexBuffer__dt(void* self, s32 flags) {
    if (self != nullptr) {
        u32 p = gabi::ea(self);
        mantDestroyAttributeBuffer(gabi::at<mantHD::AttributeBuffer>(p + 0x158), 2);
        mantDestroyVertexBuffer(gabi::at<mantHD::GpuVertexBuffer>(p + 4), 2);
        if (flags & 1)
            operator_delete(self);
    }
}

/* free the 0xF4-byte objects of one sead::Buffer {s32 n; T* p} at b (virtual destructor at
 * vtable +0xC of each, vtable at +0xF0), then the buffer */
static inline void mant_freeObjBuffer(u32 b) {
    mantHD::ObjectBuffer* objects = gabi::at<mantHD::ObjectBuffer>(b);
    u32 arr = gabi::ea(objects->mpObjects.get());
    if (arr != 0) {
        for (s32 j = 0; j < (s32)objects->mCount; j++) {
            u32 obj = gabi::ea(objects->mpObjects.get()) + j * 0xF4;
            gabi::call_ptr((u32)gabi::at<mantHD::AttributeVtable>(
                               (u32)gabi::at<mantHD::AttributeBuffer>(obj)->mVtable)
                               ->mDestroy,
                           obj, 2);
        }
        sead_free(b + 4);
        objects->mCount = 0;
        objects->mpObjects = nullptr;
    }
}

/* 021BEF38 */
static void mantProgramList__dt(void* self, s32 flags) {
    u32 p = gabi::ea(self);
    if (p == 0)
        return;
    mantHD::ProgramList* programs = gabi::at<mantHD::ProgramList>(p);
    u32 arr = gabi::ea(programs->mpEntries.get());
    if (arr != 0) {
        s32 n = (s32)programs->mCount;
        for (s32 i = 0; i < n; i++) {
            u32 e = arr + i * 0x14;
            if (e != 0) {
                mantHD::ProgramEntry* entry = gabi::at<mantHD::ProgramEntry>(e);
                u32 a = gabi::ea(entry->mAttributes[0].mpObjects.get());
                entry->mpProgram = nullptr;
                if (a != 0) {
                    for (s32 j = 0; j < (s32)entry->mAttributes[0].mCount; j++) {
                        u32 obj = gabi::ea(entry->mAttributes[0].mpObjects.get()) + j * 0xF4;
                        gabi::call_ptr((u32)gabi::at<mantHD::AttributeVtable>(
                                           (u32)gabi::at<mantHD::AttributeBuffer>(obj)->mVtable)
                                           ->mDestroy,
                                       obj, 2);
                    }
                    sead_free(e + 8);
                    entry->mAttributes[0].mCount = 0;
                    entry->mAttributes[0].mpObjects = nullptr;
                }
                mant_freeObjBuffer(e + 0xC);
            }
            n = (s32)programs->mCount;
            arr = gabi::ea(programs->mpEntries.get());
        }
        sead_free(p + 8);
        programs->mCount = 0;
        programs->mpEntries = nullptr;
    }
    if (flags & 1)
        operator_delete(self);
}

/* the double-buffered vertex buffer D of the packet (+0x8BC): two 0x254-byte buffers, their
 * GPU memory at +0x250 / +0x4A4 (size +0x24C / +0x4A0) */
static inline void mant_dbuf_release(u32 address) {
    mantHD::DoubleBuffer* buffers = gabi::at<mantHD::DoubleBuffer>(address);
    for (u32 k = 0; k < 2; ++k) {
        mantHD::VertexBuffer* buffer = &buffers->mBuffer[k];
        mantReleaseAttributeBuffer(&buffer->mAttributes);
        u32 allocation = gabi::ea(buffer->mpAllocation.get());
        buffer->mpVertices = nullptr;
        if (allocation != 0) {
            sead_free(gabi::ea(&buffer->mpAllocation));
            buffer->mCapacity = 0;
            buffer->mpAllocation = nullptr;
        }
    }
    buffers->mCount = 0;
}

/* daMant_packet_c::~daMant_packet_c body (inlined into both destructors) */
static inline void mant_packet_dt_body(u32 P) {
    daMant_packet_c* packet = gabi::at<daMant_packet_c>(P);
    packet->mVtable = kPacketVtable;
    u32 D = gabi::ea(&packet->mVertices);
    mant_dbuf_release(D);
    for (s32 i = 0; i < (s32)packet->mLightBuffers.mCount; i++) {
        u32 e = gabi::ea(packet->mLightBuffers.mpBuffers.get());
        if ((u32)i < (u32)packet->mLightBuffers.mCount)
            e += i * 0x23C;
        for (int k = 0; k < 2; k++)
            mantReleaseUniformBuffer(gabi::at<mantHD::UniformEntry>(e + 0x10 + k * 0x1C));
    }
    for (int a = 0; a < 3; a++)
        for (int k = 0; k < 2; k++)
            mantReleaseUniformBuffer(&packet->mMaterialBlock[a].mBuffer[k]);
    for (int a = 0; a < 3; a++)
        for (int k = 0; k < 2; k++)
            mantReleaseUniformBuffer(&packet->mMatrixBlock[a].mBuffer[k]);
    __destroy_arr(gabi::ea(&packet->mSampler), 7, 0x198, 0x027BE2B0);
    mantDestroyIndexBuffer(&packet->mIndices, 2);
    __destroy_arr(gabi::ea(&packet->mMaterialBlock), 3, 0x364, 0x021BEE30);
    __destroy_arr(gabi::ea(&packet->mMatrixBlock), 3, 0xA8, 0x021BEE84);
    mantDestroyLightBuffer(&packet->mLightBuffers, 2);
    if (D != 0) {
        mant_dbuf_release(D);
        __destroy_arr(D, 2, 0x254, 0x021BEED8);
    }
    __destroy_arr(gabi::ea(&packet->mPrograms), 3, 0xC, 0x021BEF38);
    mantDestroyJ3DPacket(P, 0); /* J3DPacket::~J3DPacket */
}

/* 021BF0D4 */
static void daMant_packet_c_dt(void* self, s32 flags) {
    if (self != nullptr) {
        mant_packet_dt_body(gabi::ea(self));
        if (flags & 1)
            operator_delete(self);
    }
}

/* 021BF3C4 */
static void daMant_packet_c_empty(void*) {}

/* 021BF3C8 */
static void mant_class_dt(mant_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        __destroy_arr(gabi::ea(&i_this->mMeshSph[0]), 9, 0x12C, 0x02515AE8);
        mantDestroySphere(&i_this->mWindSph, 2); /* dCcD_Sph::~dCcD_Sph */
        dCcD_Stts_dt(&i_this->mStts, 2);
        if (packet(i_this) != 0)
            mant_packet_dt_body(packet(i_this));
        mantDestroyActor(reinterpret_cast<fopAc_ac_c*>(i_this), 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 021BF708 */
static void SafeString_assureTerminationImpl(void*) {}

/* WWHD glue
data mantCalcMatrix 1018C7B0
data mantOutput 10464F18
data mantOutputCount 10464F1C
data mantCollisionIndex 10464F20
data mantHiddenPosition 10464F40
data mantSinCos 104A44F8
data mantWindScale 101B9A9C
data mantGustScale 101B9ABC
data mantRowPhase 10014624
data mantWindTarget 1047BDF4
data mantWindStep 1047BDF8
data mantWindDecay 1047BDFC
data mantWindDuration 1047BE70
data mantVertexCount 101B9A78
data mantIndexCount 101B9A7C
data mantVertexIndices 101B9D68
data mantTextureCoordinates 101B9AE0
data mantMaterialBrightness 100145FC
data mantTextureNames 10464F4C
data mantHeapManager 101F8B4C
data mantRenderer 101F8B18
data mantResources 101F4F7C
data mantDrawBuffer 104B4634
data wind_cc_sph_src 101BA218
data mesh_cc_sph_src 101BA258
ext 02008974 f32 mantGroundCross(dBgS*,dBgS_GndChk*)
ext 02008DAC void mantDestroyGround(dBgS_GndChk*,u32)
ext 02008E0C void mantConstructGround(dBgS_GndChk*)
ext 0200E240 void mantRegisterCollision(cCcS*,u32)
ext 0200FCD8 void mantTransformVector(cXyz*,cXyz*)
ext 02019510 s32 mantFloatToAngle(f32)
ext 020195B0 s32 mantAtan2s(f32,f32)
ext 0201A478 void mantSetAngles(u32,u32,u32,u32)
ext 0201ADE0 void mantSubtractVector(cXyz*,cXyz*,cXyz*)
ext 0201AE48 void mantScaleVector(cXyz*,cXyz*,f32)
ext 021BBC34 void mantDrawPacket(daMant_packet_c*)
ext 021BC3C4 void mantExecuteActor(mant_class*)
ext 021BD4A0 void mantConstructPacket(daMant_packet_c*)
ext 021BE340 void mantSetupPacketUniforms(daMant_packet_c*,mantHD::RenderPass*,mantHD::Program*,u32)
ext 02515AE8 void mantDestroySphere(dCcD_Sph*,u32)
ext 025162A4 s32 mantCheckTargetHit(dCcD_Sph*)
ext 02516300 u32 mantGetTargetHitObject(dCcD_Sph*)
ext 025166F0 void mantConstructSphere(dCcD_Sph*)
ext 02518DB0 void mantGetHitType(u32)
ext 025200D4 u32 mantGetGameInfo()
ext 02520540 s32 mantGetReverb(u32)
ext 0255F8F4 void mantSetupTevColor(dKy_tevstr_c*)
ext 0255FE90 void mantSetupTevLight(dKy_tevstr_c*)
ext 025A847C void mantSetParticle(u32,u32,u32,u32,u32,u32,u32,u32,u32,u32,u32,u32)
ext 025A8D40 void mantSetSimpleParticle(u32,u32,u32,u32,u32,u32,u32)
ext 025D50BC void mantDestroyActor(fopAc_ac_c*,u32)
ext 025D57E0 void mantDeleteActor(fopAc_ac_c*)
ext 025D672C void mantSetCullMinimum(fopAc_ac_c*,f32,f32,f32)
ext 025D673C void mantSetCullMaximum(fopAc_ac_c*,f32,f32,f32)
ext 025E1A40 void mantStartSound(u32,u32,u32,u32)
ext 025F1884 void mantRotateY(Mtx34*,u32)
ext 025F18EC void mantRotateX(Mtx34*,u32)
ext 025F1BF4 void mantMultiplyRotateZ(Mtx34*,u32)
ext 025F1C28 void mantMultiplyRotateY(Mtx34*,u32)
ext 026124B0 u32 mantGetTextureArchive(u32,u32,u32,u32)
ext 0274D2AC void mantScaleColor(mantHD::Color*,f32)
ext 0274D458 void mantMultiplyColor(mantHD::Color*,mantHD::Color*,f32)
ext 0274FBF8 void mantLockRenderer(u32)
ext 0274FCCC void mantUnlockRenderer(u32)
ext 02750250 void mantConstructRenderState(mantHD::RenderState*)
ext 02750370 void mantApplyRenderState(mantHD::RenderState*)
ext 02755FEC u32 mantFindContainingHeap(u32,u32)
ext 02756140 u32 mantGetCurrentHeap(u32)
ext 02773870 void mantLoadTexture(mantHD::Texture*,u32,u32)
ext 027B5430 void mantConstructIndexBuffer(mantHD::IndexBuffer*)
ext 027B54A0 void mantDestroyIndexBuffer(mantHD::IndexBuffer*,u32)
ext 027B54E0 void mantSetIndexBuffer(mantHD::IndexBuffer*,u32,u32,u32)
ext 027B5BD8 void mantConstructVertexBuffer(mantHD::GpuVertexBuffer*)
ext 027B5CBC void mantDestroyVertexBuffer(mantHD::GpuVertexBuffer*,u32)
ext 027B5E94 void mantFlushVertexBuffer(mantHD::GpuVertexBuffer*,u32,u32)
ext 027B90AC s32 mantFindShaderProgram(u32,u32)
ext 027B9F68 void mantSetShaderMode(u32)
ext 027BB7CC void mantActivateShader(u32)
ext 027BB9E0 void mantUpdateShader(u32,u32)
ext 027BDEB4 void mantSetSamplerTexture(mantHD::Sampler*,mantHD::Texture*)
ext 027BE53C void mantActivateSampler(mantHD::Sampler*,u32,u32,u32)
ext 027BEBEC void mantReleaseUniformBuffer(mantHD::UniformEntry*)
ext 027BF734 void mantConstructAttributeBuffer(mantHD::AttributeBuffer*)
ext 027BF7E8 void mantReleaseAttributeBuffer(mantHD::AttributeBuffer*)
ext 027BF880 void mantDestroyAttributeBuffer(mantHD::AttributeBuffer*,u32)
ext 027BFE5C void mantActivateAttributeBuffer(mantHD::AttributeBuffer*)
ext 027F0E04 void mantEntryImmediate(u32,void*,u32)
ext 027F1278 void mantConstructJ3DPacket(u32)
ext 027F13DC void mantDestroyJ3DPacket(u32,u32)
ext 027F29D4 u32 mantGetShaderCache(u32)
ext 027FB40C void mantConstructUniformBlock(void*)
ext 027FB528 void mantDestroyUniformBlock(void*,u32)
ext 027FB5D4 void mantSetUniformBlockSlots(void*,u32)
ext 027FB678 void mantFlushUniformBlock(void*)
ext 027FD6F4 void mantConstructLightBuffer(mantHD::LightBuffers*)
ext 027FD764 void mantDestroyLightBuffer(mantHD::LightBuffers*,u32)
ext 027FD838 void mantSetLightBufferSlots(mantHD::LightBuffers*,u32,u32)
ext 027FDA54 void mantSetLightBuffer(mantHD::LightBuffers*,u32,u32,u32,u32)
ext 027FE010 void mantFlushLightBuffer(mantHD::LightBuffers*)
ext 027FF478 void mantSetVertexBuffer(mantHD::GpuVertexBuffer*,u32,u32,u32)
ext 027FF530 void mantBuildAttributeBuffer(u32,u32,u32,u32,u32)
ext 027FFCBC u32 mantGetShaderManager()
ext 027FFE54 void mantSetPassUniforms(u32,u32)
ext 0280037C void mantConfigureRenderState(u32,mantHD::RenderState*)
ext 0280068C void mantInitializePrograms(mantHD::ProgramList*,u32,u32)
ext 02800B0C void mantInitializeShaderResource(u32,u32,u32)
ext 028E8D88 void mantAddVector(cXyz*,cXyz*,cXyz*)
ext 028EFFD0 void mantConstructArray(u32,u32,u32,u32)
ext 028F0164 void mantDestroyArray(u32,u32,u32,u32,u32,u32)
ext 028F4384 f32 mantSqrt(f32)
ext 028F4560 f32 mantPow(f32,f32)
ext 028F521C void mantZeroMemory(u32,u32)
ext C00060E0 void mantGX2CallDisplayList(u32,u32)
ext C0006178 void mantGX2DrawIndexed(u32,u32,u32,u32,u32,u32)
ext C00068A8 void mantGX2SetGeometryUniformBlock(u32,u32,u32)
ext C0006900 void mantGX2SetPixelUniformBlock(u32,u32,u32)
ext C0006A38 void mantGX2SetVertexUniformBlock(u32,u32,u32)
ext C00088B8 void mantDCStoreRange(u32,u32)
*/
