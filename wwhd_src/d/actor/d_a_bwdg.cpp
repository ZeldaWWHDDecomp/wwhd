/* Molgera's sand floor and wave height field.
 * GameCube names and layout, with the verified WWHD GPU packet and compiler helpers.
 */
#include "gc_api/gc_api.h"
#include "d/actor/d_a_bwdg.h"
#include "d/actor/d_a_bwd.h"

const gc_tu_data gc_tu = {.safeString = 0x1000C114, .headerStatics = {0x10462C08, 0x101B3820, 0}};

static constexpr u32 actorVtable = 0x1000C12C;
static constexpr u32 packetVtable = 0x1000C1F4;
static constexpr u32 modelUniformVtable = 0x1016EF84;
static constexpr u32 lightUniformVtable = 0x1016EFB4;
static bwd_class* boss;
static cXyz waveNormal;
static u32 waveNormalInitialized;
static BwdgModelInfo modelInfo;
static BwdgTextureCoordinates textureCoordinates;
static BwdgModelIndices modelIndices;
static BwdgSystem bwdgSystem;
static BwdgHeapManager* heapManager;
static BwdgGraphicsDevice* graphicsDevice;
static BwdgResourceManager* resourceManager;
static BwdgAttributeDescriptor attributeDescriptor;
static BwdgIndexData indexData;
static f32 zeroValue = 0.0f;
static f32 oneValue = 1.0f;

// HD: height-field and immediate packet helpers.
/*  dBgWHf::dBgWHf (allocates when this == NULL) */
static inline dBgW* new_dBgWHf() { return bwdgNewHeightField((u32)0); }
/*  dBgWHf::Set(cBgD_t*, u16* grid, f32 size, int, int, int) */
static inline BOOL dBgWHf_Set(dBgW* w, cBgD_t* d, void* grid, f32 s, s32 a, s32 b, s32 c) {
    return bwdgSetHeightField(w, d, grid, s, a, b, c);
}
/*  dBgWSv::CopyBackVtx,  dBgWHf::MoveHf */
static inline void dBgWSv_CopyBackVtx(dBgW* w) { bwdgCopyBackVertices(w); }
static inline void dBgWHf_MoveHf(dBgW* w) { bwdgMoveHeightField(w); }
/* dBgWSv::SetVtx (HD inline): the first vertex table also takes a back copy */
static inline void dBgWSv_SetVtx(dBgW* w, cXyz* vtx) {
    BwdgSavedVertices* world = reinterpret_cast<BwdgSavedVertices*>(w);
    cXyz* old = world->pm_vtx_tbl;
    world->pm_vtx_tbl = vtx;
    if (old == nullptr)
        dBgWSv_CopyBackVtx(w);
}
/*  mDoMtx_XrotS (also local in mo2) */
static inline void mDoMtx_XrotS(Mtx34* m, s16 x) { bwdgRotateX(m, x); }
/*  J3DDrawBuffer::entryImm(packet, u16); j3dSys opa buffer at  */
static inline void opa_entryImm(void* packet, u16 idx) {
    bwdgEntryPacket(bwdgSystem.mpOpaqueDrawBuffer.get(), packet, idx);
}

static void* bwdg_packet_ct(void*);

static inline void bwdg_bind_shader(BwdgUniformContainer* uniforms, BwdgShaderShape* shape) {
    BwdgMaterial* material = uniforms->mpMaterials;
    BwdgUniformBinding* binding = &material->mBindings[material->mCurrentBinding];
    BwdgUniformLocations* locations =
        shape->mUniformCount != 0 ? shape->mpUniformLocations.get() : nullptr;
    s32 vertex = locations->mVertex;
    void* data = binding->mpData;
    u32 size = binding->mSize;
    s32 pixel = locations->mPixel;
    s32 geometry = locations->mGeometry;
    if (vertex == -1 && pixel == -1 && geometry == -1)
        return;
    if (pixel != -1)
        bwdgSetPixelUniformBlock(pixel, size, data);
    if (vertex != -1)
        bwdgSetVertexUniformBlock(vertex, size, data);
    if (geometry != -1)
        bwdgSetGeometryUniformBlock(geometry, size, data);
}

/* 0210508C */
void daBwdg_packet_c::draw(void* ctx) {
    daBwdg_packet_c* packet = this;
    BwdgDrawContext* context = static_cast<BwdgDrawContext*>(ctx);
    BwdgRenderState* render = &packet->mRenderState;
    BwdgShaderShape* shape = nullptr;
    s32 mode = context->mMode;
    if (mode < 4) {
        u32 count = render->mDrawModeCount;
        BwdgDrawMode* drawMode = render->mpDrawModes;
        if ((u32)mode < count)
            drawMode += mode;
        shape = reinterpret_cast<BwdgShaderShape*>(drawMode->mpShader.get());
    }
    BwdgProgramState* state = bwdgGetProgramState(&bwdgSystem);
    BwdgShaderProgram* program = shape->mpProgram;
    if (program != state->mpProgram.get()) {
        u8 flags = program->mFlags;
        void* currentGroup = state->mpGroup;
        if (flags & 2) {
            program->mFlags = flags & ~2;
            bwdgUpdateShaderProgram(program, 0);
        }
        void* group = program->mpGroup->mpGroup;
        if (currentGroup != group)
            bwdgBindProgramGroup(group);
        if (program->mDisplayListSize != 0) {
            bwdgCallDisplayList(program->mpDisplayList.get(), (u32)program->mDisplayListSize);
            state->mpGroup = group;
            state->mpProgram = program;
        } else {
            bwdgBindProgram(program);
            state->mpProgram = program;
            state->mpGroup = group;
        }
    }
    mode = context->mMode;
    if (mode == 0)
        return;
    if (mode == 1 || mode == 2) {
        BwdgUniformContainer* uniforms = &render->mUniforms;
        bwdg_bind_shader(uniforms, shape);
        gabi::call_ptr(uniforms->mLight.mpVtable->mBind.get(), &uniforms->mLight, shape);
        gabi::call_ptr(uniforms->mModel.mpVtable->mBind.get(), &uniforms->mModel, shape);
        if (mode == 2) {
            BwdgDrawObject* object = context->mpObject;
            if (object != nullptr)
                gabi::call_ptr(object->mpVtable->mBind.get(), object, shape);
            bwdgApplyDrawContext(context, shape);
        }
    }
    u32 vertices = shape->mVertexCount != 0 ? (u32)shape->mpVertices.v : 0;
    // HD: the empty vertex array still passes its attribute-header address (4).
    bwdgBindVertices(&render->mVertexBinding, vertices + offsetof(BwdgShaderVertices, mAttributes),
                     0, 0);
    {
        gabi::Local<bwdg_draw_state> drawState;
        bwdgDrawStateCtor(drawState.get());
        u32 flags = drawState->mFlags;
        drawState->mCullMode = 2;
        drawState->mPolygonOffsetEnable = 0;
        drawState->mDepthMode = 0;
        drawState->mFlags = ((((flags & ~0xFu) + 7) & ~0xF0u) + 0x10);
        bwdgSetDrawMode((u32)context->mMode, drawState.get());
        bwdgApplyDrawState(drawState.get());
    }
    {
        u32 drawModeIndex = context->mMode;
        u32 count = render->mDrawModeCount;
        BwdgDrawMode* drawMode = render->mpDrawModes;
        if (drawModeIndex < count)
            drawMode += drawModeIndex;
        // HD: prepare has advanced the buffer index; draw consumes the preceding buffer.
        u32 buffer = render->mVertices.mCurrentBuffer == 0 ? 1 : 0;
        bwdgBindFetchShader(drawMode->mBuffers[buffer].mpShaders.get());
    }
    if (render->mIndices.mCount != 0) {
        BwdgIndexBuffer* indices = &render->mIndices;
        bwdgDrawIndexed((u32)indices->mPrimitive, (u32)indices->mCount, indices->mpIndices.get(),
                        (u32)indices->mIndexType, 0, 1);
    }
    bwdgApplyDrawState(&bwdgSystem.mDrawState);
}

/* 02103F14 */
static BOOL daBwdg_Draw(bwdg_class* i_this) {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0_FULL, &i_this->current.pos, &i_this->tevStr);
    MtxTrans(0.0f, 10.0f, 0.0f, 0);
    PSMTXCopy(calc_mtx(), &i_this->mBwdgPacket.mMtx);
    i_this->mBwdgPacket.mpTevStr = &i_this->tevStr;
    opa_entryImm(i_this->packet(), 0);
    i_this->mBwdgPacket.prepare(); /* HD: packet prepare */
    return TRUE;
}

/* 02103FA4 */
static void wave_cont(bwdg_class* i_this, u8 r4) {
    cXyz* posVtx = i_this->getPos();
    f32 cx, cz;
    f32 f31, f30, f29, f28, f27;
    if (r4 != 0) {
        f30 = 1.0f;
        f31 = 2000.0f;
        cx = 0.0f;
        cz = 0.0f;
        f28 = -0.75f;
        f29 = 1.0f;
        f27 = 600.0f;
    } else {
        f30 = 0.5f;
        f31 = 0.0f;
        bwd_class* waveBoss = boss;
        i_this->m002B8 = (s16)(i_this->m002B8 + waveBoss->m17D8);
        waveBoss = boss;
        f27 = waveBoss->m17E0;
        cx = waveBoss->m17C8.x;
        f29 = waveBoss->m17DC;
        cz = waveBoss->m17C8.z;
        f28 = waveBoss->m17D4;
    }

    for (int i = 0; i < 0x1081; posVtx++, i++) {
        f32 px = posVtx->x;
        f32 pz = posVtx->z;
        f32 dz = pz - cz;
        f32 dx = px - cx;
        f32 f0 = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
        f32 f1 = 3000.0f - f0;
        f32 f4 = f1 >= 0.0f ? f1 : 0.0f; /* fsel */
        f1 = f1 >= 0.0f ? f1 : 0.0f;
        f32 f3 = gabi::fmadds(f1, 0.00667f, 10.0f);
        f4 *= 0.01f;
        if (f3 > 30.0f) {
            f3 = 30.0f;
        }
        s16 a = (s16)gabi::ftoi(f0 * 200.0f);
        f32 step = gabi::fmadds(f3, 0.3f, f31) * f29;
        f32 f2 = cM_ssin(i_this->m002B8 + a);
        cLib_addCalc2(&i_this->m002C0[i], gabi::fmadds(f4 * f4, f28, f2 * f3), f30, step);
        posVtx->y = i_this->m002C0[i] + f27;
    }

    /* HD: the rotated normal is a function-local static, (0, 1, 0) rotated by -0x4A38 about X */
    cXyz* sp0C = &waveNormal;
    if (waveNormalInitialized == 0) {
        waveNormalInitialized = 1;
        sp0C->x = 0.0f;
        sp0C->y = 0.9687129855155945f;
        sp0C->z = -0.24818499386310577f;
    }
    cXyz* nrmVtx = i_this->getNrm();
    posVtx = i_this->getPos();
    /* HD: the slope is taken from the next row (z + 130), not from the height above the base */
    for (int i = 0; i < 0x1081; i++, posVtx++, nrmVtx++) {
        if (i >= 0x1040) {
            continue;
        }
        gabi::Local<cXyz> d;
        cXyz_mi(posVtx + 0x41, d.get(), posVtx);
        f32 dy = d->y;
        Mtx34* m = calc_mtx();
        s16 ang = cM_atan2s(dy, 130.0f);
        mDoMtx_XrotS(m, (s16)-ang);
        MtxPosition(sp0C, nrmVtx);
    }
}

/* 0210439C */
static void* boss_a_d_sub(void* param_1, void* param_2) {
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == fpcNm_BWD_e) {
        return param_1;
    }
    return nullptr;
}

/* 021043EC */
static BOOL daBwdg_Execute(bwdg_class* i_this) {
    cXyz* posVtx = i_this->getPos();
    if (boss == nullptr) {
        void* b = bwdgSearchActors(boss_a_d_sub, i_this);
        boss = static_cast<bwd_class*>(b);
        if (b == nullptr) {
            return TRUE;
        }
    }
    i_this->m002B4 = (s16)(i_this->m002B4 + 1);
    dBgWSv_CopyBackVtx(i_this->mpBgW);
    wave_cont(i_this, 0);
    dBgWSv_SetVtx(i_this->mpBgW, posVtx);
    dBgWHf_MoveHf(i_this->mpBgW);
    return TRUE;
}

/* 02104498 */
static BOOL daBwdg_IsDelete(bwdg_class* i_this) { return TRUE; }

/* 021044A0 */
static BOOL daBwdg_Delete(bwdg_class* i_this) {
    dComIfG_resDelete(&i_this->mPhase, "Bwdg");
    if (i_this->heap) {
        cBgS_Release(dComIfG_Bgsp(), i_this->mpBgW);
    }
    return TRUE;
}

/* 021044F8 */
static BOOL useHeapInit(fopAc_ac_c* i_actor) {
    bwdg_class* i_this = (bwdg_class*)i_actor;
    i_this->mpBgW = new_dBgWHf();
    if (i_this->mpBgW == nullptr) {
        return FALSE;
    }
    cBgD_t* dzb =
        (cBgD_t*)dComIfG_getObjectRes("Bwdg", dRes_INDEX_BWDG_DZB_HSAND1_e, gc_tu.safeString);
    void* grid = dComIfG_getObjectRes("Bwdg", dRes_INDEX_BWDG_DAT_GRIDIDX_e, gc_tu.safeString);
    if (!dBgWHf_Set(i_this->mpBgW, dzb, grid, 130.0f, 0x40, 0x40, 0)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/* 02104EA0 */
static cPhs_State daBwdg_Create(fopAc_ac_c* i_actor) {
    bwdg_class* i_this = (bwdg_class*)i_actor;
    /* fopAcM_ct(i_this, bwdg_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->mSelf = i_this;
            i_this->__vtbl = actorVtable;
            bwdg_packet_ct(i_this->packet()); /* daBwdg_packet_c::daBwdg_packet_c (HD) */
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }

    cPhs_State phase_state = dComIfG_resLoad(&i_this->mPhase, "Bwdg");
    if (phase_state == cPhs_COMPLEATE_e) {
        boss = nullptr;
        i_this->current.pos.set(0.0f, 0.0f, 0.0f);
        if (!fopAcM_entrySolidHeap(i_this, useHeapInit, 0x96000)) {
            return cPhs_ERROR_e;
        }
        dBgS_Regist(dComIfG_Bgsp(), i_this->mpBgW, i_this);
        /* base_xz_set */
        cXyz* posVtx = i_this->getPos();
        for (int i = 0; i < 0x41; i++) {
            f32 z = gabi::fmsubs((f32)i, 130.0f, 4160.0f);
            for (int j = 0; j < 0x41; posVtx++, j++) {
                posVtx->x = gabi::fmsubs((f32)j, 130.0f, 4160.0f);
                posVtx->z = z;
            }
        }
        wave_cont(i_this, 1);
        dBgWSv_SetVtx(i_this->mpBgW, i_this->getPos());
        dBgWSv_CopyBackVtx(i_this->mpBgW);
        dBgW_Move(i_this->mpBgW);
    }
    return phase_state;
}

// HD: compiler-generated and template code of this translation unit.

/* 021039CC */
static void bwdg_mtx_assign(Mtx34* dst, const Mtx34* src) {
    f32 t[12];
    t[5] = src->m[1][1];
    t[4] = src->m[1][0];
    t[6] = src->m[1][2];
    t[7] = src->m[1][3];
    t[8] = src->m[2][0];
    t[9] = src->m[2][1];
    t[11] = src->m[2][3];
    t[0] = src->m[0][0];
    t[3] = src->m[0][3];
    t[1] = src->m[0][1];
    t[10] = src->m[2][2];
    t[2] = src->m[0][2];
    for (int i = 0; i < 12; i++)
        dst->m[i / 4][i % 4] = t[i];
}

/* 02103A6C */
static void bwdg_color_s10_to_f(be<f32>* out, const be<s16>* in) {
    f32 r = (f32)(s32)in[0] / 255.0f;
    f32 g = (f32)(s32)in[1] / 255.0f;
    f32 b = (f32)(s32)in[2] / 255.0f;
    f32 a = (f32)(s32)in[3] / 255.0f;
    out[0] = r;
    out[1] = g;
    out[2] = b;
    out[3] = a;
}

/* 02105478 */
static void __sinit_d_a_bwdg_cpp() {
    sinit_header_statics(gc_tu.headerStatics[0],
                         gc_tu.headerStatics[1]); // HD: no separate zero block.
}

/* 0210550C */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02105DD4 */
static void SafeString_assureTerminationImpl(void*) {}

/* 021059E4 */
static void packet_empty(void*) {}

/* 02105520 */
static void* packet_buf_ct(void* p) {
    if (p == nullptr) {
        p = operator_new(0x254);
        if (p == nullptr)
            return nullptr;
    }
    BwdgVertexBuffer* buffer = static_cast<BwdgVertexBuffer*>(p);
    bwdgAttributeBufferCtor(&buffer->mAttributeBuffer);
    bwdgFetchShaderCtor(&buffer->mFetchShader);
    buffer->mpAllocation = nullptr;
    buffer->mAllocationCount = 0;
    return p;
}

/* 0210557C */
static void* packet_elem16_ct(void* p) {
    if (p == nullptr)
        p = operator_new(0x10);
    return p;
}

/* 021055A8 */
static void packet_buf_dt(void* p, s32 flags) {
    if (p != nullptr) {
        BwdgVertexBuffer* buffer = static_cast<BwdgVertexBuffer*>(p);
        bwdgFetchShaderDt(&buffer->mFetchShader, 2);
        bwdgAttributeBufferDt(&buffer->mAttributeBuffer, 2);
        if (flags & 1)
            operator_delete(p);
    }
}

static inline void bwdg_heap_free(gptr<void>* field) {
    void* previous = *field;
    BwdgHeap* heap = bwdgFindAllocationHeap(static_cast<BwdgHeapManager*>(heapManager), previous);
    u32 free = heap->mpVtable->mFree.get();
    // HD: heap lookup can change the allocation; read the field again for free.
    gabi::call_ptr(free, heap, field->get());
}

static inline void bwdg_gpubuf_release(BwdgVertexBuffer* buffer) {
    bwdgReleaseFetchShader(&buffer->mFetchShader);
    void* allocation = buffer->mpAllocation;
    buffer->mpData = nullptr;
    if (allocation != nullptr) {
        bwdg_heap_free(&buffer->mpAllocation);
        buffer->mAllocationCount = 0;
        buffer->mpAllocation = nullptr;
    }
}

static inline void bwdg_objarr_free(BwdgFetchShaderArray* shaders) {
    s32 count = shaders->mCount;
    BwdgFetchShader* array = shaders->mpShaders;
    for (s32 i = 0; i < count;) {
        BwdgFetchShader* shader = &array[i];
        BwdgVirtualTable* vtable = shader->mpVtable;
        gabi::call_ptr(vtable->mDestroy.get(), shader, 2);
        i++;
        count = shaders->mCount;
        array = shaders->mpShaders;
    }
    bwdg_heap_free(reinterpret_cast<gptr<void>*>(&shaders->mpShaders));
    shaders->mCount = 0;
    shaders->mpShaders = nullptr;
}

static inline void bwdg_packet_dt_body(daBwdg_packet_c* packet) {
    BwdgRenderState* render = &packet->mRenderState;
    BwdgVertexBuffers* buffers = &render->mVertices;
    packet->mVtable = packetVtable;
    bwdg_gpubuf_release(&buffers->mBuffers[0]);
    bwdg_gpubuf_release(&buffers->mBuffers[1]);
    buffers->mFlags = 0;
    BwdgUniformContainer* uniforms = &render->mUniforms;
    s32 count = uniforms->mCount;
    for (s32 i = 0; i < count;) {
        BwdgMaterial* base = uniforms->mpMaterials;
        BwdgMaterial* material = (u32)i < (u32)count ? &base[i] : base;
        for (int k = 0; k < 2; k++)
            bwdgReleaseUniformBinding(&material->mBindings[k]);
        count = uniforms->mCount;
        i++;
    }
    for (int k = 0; k < 2; k++)
        bwdgReleaseUniformBinding(&uniforms->mModel.mBindings[k]);
    for (int k = 0; k < 2; k++)
        bwdgReleaseUniformBinding(&uniforms->mLight.mBindings[k]);
    bwdgVertexBindingDt(&render->mVertexBinding, 2);
    bwdgIndexBufferDt(&render->mIndices, 2);
    bwdgUniformDt(&uniforms->mLight, 0);
    bwdgUniformDt(&uniforms->mModel, 0);
    bwdgUniformContainerDt(uniforms, 2);
    if (gabi::ea(buffers) != 0) {
        bwdg_gpubuf_release(&buffers->mBuffers[0]);
        bwdg_gpubuf_release(&buffers->mBuffers[1]);
        buffers->mFlags = 0;
        bwdgDestroyArray(buffers, 2, 0x254, packet_buf_dt, 0, 0);
    }
    BwdgDrawMode* modes = render->mpDrawModes;
    if (modes != nullptr) {
        s32 count = render->mDrawModeCount;
        for (s32 i = 0; i < count; i++) {
            BwdgDrawMode* mode = &modes[i];
            if (gabi::ea(mode) == 0)
                continue;
            BwdgFetchShader* shaders = mode->mBuffers[0].mpShaders;
            mode->mpShader = nullptr;
            if (shaders != nullptr)
                bwdg_objarr_free(&mode->mBuffers[0]);
            if (mode->mBuffers[1].mpShaders != nullptr)
                bwdg_objarr_free(&mode->mBuffers[1]);
            count = render->mDrawModeCount;
            modes = render->mpDrawModes;
        }
        bwdg_heap_free(reinterpret_cast<gptr<void>*>(&render->mpDrawModes));
        render->mDrawModeCount = 0;
        render->mpDrawModes = nullptr;
    }
    bwdgBasePacketDt(packet, 0);
}

/* 02105608 */
static void bwdg_packet_dt(void* p, s32 flags) {
    if (p != nullptr) {
        bwdg_packet_dt_body(static_cast<daBwdg_packet_c*>(p));
        if (flags & 1)
            operator_delete(p);
    }
}

/* 021059E8 */
static void bwdg_class_dt(bwdg_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        bwdg_packet_dt_body(&i_this->mBwdgPacket);
        bwdgActorDt(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 02103B30 */
void daBwdg_packet_c::prepare() {
    daBwdg_packet_c* packet = this;
    BwdgVertexBuffers* buffers = &packet->mRenderState.mVertices;
    u32 first = buffers->mBuffers[buffers->mCurrentBuffer].mpData.v;
    // HD: dcbz clears complete cache lines, including the prefix of an unaligned buffer.
    for (u32 address = first; address < first + 0x41000; address += 0x20) {
        BwdgCacheLine& line = gabi::object_at<BwdgCacheLine>(address & ~31u);
        for (int i = 0; i < 8; i++)
            line.mWords[i] = 0;
    }
    BwdgVertex* output =
        reinterpret_cast<BwdgVertex*>(buffers->mBuffers[buffers->mCurrentBuffer].mpData.get());
    u32 count = modelInfo.mVertexCount;
    for (u32 i = 0; i < count; i++) {
        BwdgVertexIndex* index = &modelIndices.mIndices[i];
        BwdgVertex* vertex = &output[i];
        cXyz* position = &packet->mPos[index->mPosition];
        f32 z = position->z, x = position->x, y = position->y;
        vertex->mPosition.x = x;
        vertex->mPosition.y = y;
        vertex->mPosition.z = z;
        cXyz* normal = &packet->mNrm[index->mNormal];
        z = normal->z;
        x = normal->x;
        y = normal->y;
        vertex->mNormal.x = x;
        vertex->mNormal.y = y;
        vertex->mNormal.z = z;
        u32 texCoord = index->mTexCoord;
        f32 s = textureCoordinates.mCoords[texCoord][0];
        f32 t = textureCoordinates.mCoords[texCoord][1];
        vertex->mTexCoord[0] = s;
        vertex->mTexCoord[1] = t;
        count = modelInfo.mVertexCount;
    }
    BwdgVertexBuffer* buffer = &buffers->mBuffers[buffers->mCurrentBuffer];
    bwdgFlushAttributeBuffer(&buffer->mAttributeBuffer, 0, (u32)buffer->mAttributeBuffer.mSize);
    buffers->mCurrentBuffer = buffers->mCurrentBuffer == 0;
    bwdgSetFog(packet->mpTevStr.get());
    bwdgSetLight(packet->mpTevStr.get());
    gabi::Local<Mtx34> view;
    bwdg_mtx_assign(view.get(), &bwdgSystem.mViewMatrix);
    BwdgUniformContainer* uniforms = &packet->mRenderState.mUniforms;
    bwdgSetReflectionTexture(uniforms, 0, view.get(), &bwdgSystem.mTextureMatrix,
                             bwdgSystem.mpTexture->mReflectionTexture);
    {
        BwdgLightParameters* light = reinterpret_cast<BwdgLightParameters*>(packet->mpTevStr.get());
        BwdgMaterial* material = uniforms->mpMaterials;
        gabi::Local<BwdgColor> color;
        gabi::Local<BwdgColor> result;
        bwdg_color_s10_to_f(color->mValues, reinterpret_cast<be<s16>*>(&light->mColor));
        light = reinterpret_cast<BwdgLightParameters*>(packet->mpTevStr.get());
        bwdgConvertColor(result.get(), color.get(), (f32)light->mColorIntensity);
        for (int i = 0; i < 4; i++)
            material->mColor.mWords[i] = result->mWords[i];
        light = reinterpret_cast<BwdgLightParameters*>(packet->mpTevStr.get());
        material = uniforms->mpMaterials;
        bwdg_color_s10_to_f(color->mValues, reinterpret_cast<be<s16>*>(&light->mAmbientColor));
        light = reinterpret_cast<BwdgLightParameters*>(packet->mpTevStr.get());
        gabi::Local<BwdgColor> ambient;
        bwdgConvertColor(ambient.get(), color.get(), (f32)light->mAmbientIntensity);
        for (int i = 0; i < 4; i++)
            material->mAmbientColor.mWords[i] = ambient->mWords[i];
    }
    uniforms->mModel.mFlags = 0;
    {
        gabi::Local<Mtx34> matrix;
        for (int i = 0; i < 12; i++)
            matrix->m[i / 4][i % 4] = packet->mMtx.m[i / 4][i % 4];
        PSMTXCopy(matrix.get(), &uniforms->mModel.mMatrix);
    }
    BwdgLightParameters* light = reinterpret_cast<BwdgLightParameters*>(packet->mpTevStr.get());
    be<s16>* color = reinterpret_cast<be<s16>*>(&light->mColor);
    for (int i = 0; i < 4; i++)
        uniforms->mConstants[16 + i] = (f32)color[i] / 255.0f;
    be<u8>* konstColor = reinterpret_cast<be<u8>*>(&light->mKonstColor);
    for (int i = 0; i < 4; i++)
        uniforms->mConstants[20 + i] = (f32)konstColor[i] / 255.0f;
    light = reinterpret_cast<BwdgLightParameters*>(packet->mpTevStr.get());
    bwdgScaleColor(&uniforms->mConstants[20], (f32)light->mKonstIntensity);
    BwdgMaterial* material = uniforms->mpMaterials;
    be<u32>* uniformWords = reinterpret_cast<be<u32>*>(&uniforms->mConstants[8]);
    for (int i = 0; i < 4; i++)
        uniformWords[i] = material->mColor.mWords[i];
    bwdgUpdateUniforms(uniforms, 0);
}

static inline void* bwdg_heap_alloc(u32 size, s32 alignment) {
    BwdgHeap* heap = bwdgGetCurrentHeap(static_cast<BwdgHeapManager*>(heapManager));
    return gabi::call_ptr<void*>(heap->mpVtable->mAllocate.get(), heap, size, alignment);
}

/* 021045E8 */
static void* bwdg_packet_ct(void* pkt) {
    if (pkt == nullptr) {
        pkt = operator_new(sizeof(daBwdg_packet_c));
        if (pkt == nullptr)
            return nullptr;
    }
    daBwdg_packet_c* packet = static_cast<daBwdg_packet_c*>(pkt);
    bwdgBasePacketCtor(packet);
    BwdgRenderState* render = &packet->mRenderState;
    packet->mVtable = packetVtable;
    render->mModeCount = 0;
    {
        BwdgDrawModes* modes = reinterpret_cast<BwdgDrawModes*>(&render->mDrawModeCount);
        if (gabi::ea(modes) == 0) {
            modes = static_cast<BwdgDrawModes*>(operator_new(sizeof(BwdgDrawModes)));
            if (modes != nullptr) {
                modes->mpModes = nullptr;
                modes->mCount = 0;
            }
        } else {
            modes->mpModes = nullptr;
            modes->mCount = 0;
        }
    }
    BwdgVertexBuffers* buffers = &render->mVertices;
    {
        BwdgVertexBuffers* allocated = buffers;
        if (gabi::ea(allocated) == 0)
            allocated = static_cast<BwdgVertexBuffers*>(operator_new(sizeof(BwdgVertexBuffers)));
        if (allocated != nullptr) {
            bwdgConstructArray(allocated, 2, sizeof(BwdgVertexBuffer), packet_buf_ct);
            allocated->mCurrentBuffer = 0;
            allocated->mAttributeCount = 0;
            allocated->mFlags = 0;
            allocated->mStride = sizeof(BwdgVertex);
            allocated->mValid = 0;
            allocated->mBuffers[0].mpData = nullptr;
            allocated->mBuffers[1].mpData = nullptr;
        }
    }
    BwdgUniformContainer* uniforms = &render->mUniforms;
    bwdgUniformContainerCtor(uniforms);
    bwdgUniformCtor(&uniforms->mModel);
    uniforms->mModel.mpVtable.v = modelUniformVtable;
    bwdgClearMemory(&uniforms->mModel.mMatrix, 0x34);
    if (gabi::ea(&uniforms->mModel.mMatrix) == 0)
        operator_new(sizeof(Mtx34));
    bwdgUniformCtor(&uniforms->mLight);
    uniforms->mLight.mpVtable.v = lightUniformVtable;
    bwdgClearMemory(uniforms->mConstants, sizeof(uniforms->mConstants));
    {
        f32 zero = zeroValue, one = oneValue;
        const u16 diagonal[] = {11, 7, 31, 15, 35, 23, 27, 3, 19, 39, 43};
        for (u32 i = 0; i <= 43; i++) {
            bool isDiagonal = false;
            for (u16 index : diagonal)
                isDiagonal |= index == i;
            uniforms->mConstants[i] = isDiagonal ? one : zero;
        }
    }
    bwdgConstructArray(&uniforms->mConstants[44], 2, 0x10, packet_elem16_ct);
    bwdgConstructArray(&uniforms->mConstants[52], 2, 0x10, packet_elem16_ct);
    bwdgConstructArray(&uniforms->mConstants[60], 2, 0x10, packet_elem16_ct);
    {
        const u16 matrices[] = {68, 80, 92, 104, 116, 128, 140, 152, 164, 168, 172, 176, 180, 184};
        for (int i = 0; i < 14; i++) {
            if (gabi::ea(&uniforms->mConstants[matrices[i]]) == 0)
                operator_new(i < 8 ? sizeof(Mtx34) : 0x10);
        }
    }
    uniforms->mFlags = 0;
    bwdgIndexBufferCtor(&render->mIndices);
    bwdgVertexBindingCtor(&render->mVertexBinding);
    bwdgAttributeStateCtor(&render->mAttributes);
    {
        gabi::Local<SafeString> name;
        name->mStringTop = gabi::guest_addr("sand");
        name->__vtbl = gc_tu.safeString;
        BwdgShaderRegistry* registry = bwdgGetShaderRegistry();
        s32 index = bwdgFindShader(registry->mpArchive.get(), name.get());
        BwdgShaderEntry* shader;
        if (index < 0) {
            shader = nullptr;
        } else {
            u32 count = registry->mEntryCount;
            shader = registry->mpEntries;
            BwdgShaderEntry* entry = (u32)index < count ? &shader[index] : shader;
            if (entry->mReady == 0) {
                BwdgShaderArchive* archive = registry->mpArchive;
                BwdgShaderParameter* parameter = (u32)index < (u32)archive->mParameterCount
                                                     ? &archive->mpParameters.get()[index]
                                                     : nullptr;
                bwdgPrepareShader(entry, parameter, 0);
                count = registry->mEntryCount;
                shader = registry->mpEntries;
            }
            if ((u32)index < count)
                shader += index;
        }
        bwdgSetShaderEntry(render, shader, 0);
        buffers->mAttributeCount = 0x13;
        buffers->mpAttributeDescriptor = &attributeDescriptor;
    }
    for (int i = 0; i < 2; i++) {
        BwdgVertexBuffer* buffer = &buffers->mBuffers[i];
        if (buffer->mpData == nullptr) {
            void* allocation = bwdg_heap_alloc(0x41000, 0x40);
            if (allocation != nullptr) {
                buffer->mpAllocation = allocation;
                buffer->mAllocationCount = 0x2080;
            }
            buffer->mpData = buffer->mpAllocation;
        }
        bwdgSetVertexBuffer(&buffer->mAttributeBuffer, buffer->mpData.get(), 0x2080,
                            &buffers->mAttributeCount);
    }
    buffers->mFlags = 0;
    buffers->mValid = 1;
    for (u32 i = 0; i < (u32)render->mModeCount; i++) {
        BwdgDrawMode* mode = render->mpDrawModes;
        if (i < (u32)render->mDrawModeCount)
            mode += i;
        BwdgFetchShader* shaders = mode->mBuffers[0].mpShaders;
        void* shader = mode->mpShader;
        mode->mpShader = nullptr;
        if (shaders != nullptr)
            bwdg_objarr_free(&mode->mBuffers[0]);
        if (mode->mBuffers[1].mpShaders != nullptr)
            bwdg_objarr_free(&mode->mBuffers[1]);
        mode->mpShader = shader;
        for (int j = 0; j < 2; j++) {
            void* allocation = bwdg_heap_alloc(sizeof(BwdgFetchShader), 4);
            if (allocation != nullptr)
                bwdgFetchShaderCtor(static_cast<BwdgFetchShader*>(allocation));
            if (allocation != nullptr) {
                mode->mBuffers[j].mpShaders = static_cast<BwdgFetchShader*>(allocation);
                mode->mBuffers[j].mCount = 1;
            }
        }
        for (int j = 0; j < 2; j++) {
            bwdgSetFetchShader(shader, mode->mBuffers[j].mpShaders.get(),
                               &buffers->mBuffers[j].mAttributeBuffer, &buffers->mAttributeCount,
                               0);
        }
    }
    bwdgInitializeUniforms(uniforms, 1, 0);
    bwdgSetIndexBuffer(&render->mIndices, &indexData, 4, (u32)modelInfo.mIndexCount);
    render->mIndices.mPrimitive = 4;
    void* shader;
    {
        gabi::Local<SafeString> archive;
        gabi::Local<SafeString> resource;
        BwdgResourceManager* manager = resourceManager;
        resource->mStringTop = gabi::guest_addr("ProgramTexture.bfres");
        resource->__vtbl = gc_tu.safeString;
        archive->mStringTop = gabi::guest_addr("ProgramTexture");
        archive->__vtbl = gc_tu.safeString;
        shader = bwdgFindResource(manager, archive.get(), resource.get(), 0);
    }
    bwdgLockGraphics(static_cast<BwdgGraphicsDevice*>(graphicsDevice));
    bwdgConfigureAttributes(&render->mAttributes, shader, "B_sand2");
    bwdgUnlockGraphics(static_cast<BwdgGraphicsDevice*>(graphicsDevice));
    const u8 signature[] = {1, 2, 3, 4, 5, 6, 14, 13, 7};
    bool same = true;
    for (u8 i : signature) {
        if (same &&
            render->mVertexBinding.mAttributes.mSignature[i] != render->mAttributes.mSignature[i])
            same = false;
    }
    if (!same) {
        bwdgCopyAttributeState(&render->mVertexBinding, &render->mAttributes);
        u8 flags = render->mVertexBinding.mFlags;
        render->mVertexBinding.mBufferDirty = 1;
        render->mVertexBinding.mLayoutDirty = 1;
        render->mVertexBinding.mShaderDirty = 1;
        render->mVertexBinding.mFlags = flags | 2;
    } else {
        u8 flags = render->mVertexBinding.mFlags;
        u32 buffer = render->mAttributes.mSignature[10], size = render->mAttributes.mSignature[12];
        render->mVertexBinding.mAttributes.mSignature[10] = buffer;
        render->mVertexBinding.mFlags = flags | 2;
        render->mVertexBinding.mBufferSize = size;
        render->mVertexBinding.mAttributes.mSignature[12] = size;
        render->mVertexBinding.mShaderDirty = 1;
        render->mVertexBinding.mBuffer = buffer;
        render->mVertexBinding.mBufferDirty = 1;
        render->mVertexBinding.mLayoutDirty = 1;
    }
    return pkt;
}

/* WWHD glue (tools/verify/autoglue.py)
str 1000C188 in 021044A0
str 1000C190 in 021044F8
str 1000C1D4 in 02104EA0
str 1000C198
str 1000C1A0
str 1000C1A8
str 1000C1B8
ext 024F5800 dBgW* bwdgNewHeightField(u32)
ext 024F5870 BOOL bwdgSetHeightField(dBgW*,cBgD_t*,void*,f32,s32,s32,s32)
ext 024F5B08 void bwdgCopyBackVertices(dBgW*)
ext 024F5910 void bwdgMoveHeightField(dBgW*)
ext 025F18EC void bwdgRotateX(Mtx34*,s16)
ext 027F0E04 void bwdgEntryPacket(void*,void*,u16)
ext 025DE508 void* bwdgSearchActors(BwdgSearch,void*)
ext 027B5BD8 void bwdgAttributeBufferCtor(BwdgAttributeBuffer*)
ext 027BF734 void bwdgFetchShaderCtor(BwdgFetchShader*)
ext 027BF880 void bwdgFetchShaderDt(BwdgFetchShader*,s32)
ext 027B5CBC void bwdgAttributeBufferDt(BwdgAttributeBuffer*,s32)
ext 02755FEC BwdgHeap* bwdgFindAllocationHeap(BwdgHeapManager*,void*)
ext 027BF7E8 void bwdgReleaseFetchShader(BwdgFetchShader*)
ext 027BEBEC void bwdgReleaseUniformBinding(BwdgUniformBinding*)
ext 027BE2B0 void bwdgVertexBindingDt(BwdgVertexBindingState*,s32)
ext 027B54A0 void bwdgIndexBufferDt(BwdgIndexBuffer*,s32)
ext 027FB528 void bwdgUniformDt(void*,s32)
ext 027FD764 void bwdgUniformContainerDt(BwdgUniformContainer*,s32)
ext 028F0164 void bwdgDestroyArray(void*,u32,u32,BwdgDestructor,u32,u32)
ext 027F13DC void bwdgBasePacketDt(void*,s32)
ext 025D50BC void bwdgActorDt(fopAc_ac_c*,s32)
ext 027B5E94 void bwdgFlushAttributeBuffer(BwdgAttributeBuffer*,u32,u32)
ext 0255F8F4 void bwdgSetFog(dKy_tevstr_c*)
ext 0255FE90 void bwdgSetLight(dKy_tevstr_c*)
ext 027FDA54 void bwdgSetReflectionTexture(BwdgUniformContainer*,u32,Mtx34*,Mtx34*,void*)
ext 0274D458 void bwdgConvertColor(BwdgColor*,BwdgColor*,f32)
ext 0274D2AC void bwdgScaleColor(be<f32>*,f32)
ext 027FE0DC void bwdgUpdateUniforms(BwdgUniformContainer*,u32)
ext C0006900 void bwdgSetPixelUniformBlock(s32,u32,void*)
ext C0006A38 void bwdgSetVertexUniformBlock(s32,u32,void*)
ext C00068A8 void bwdgSetGeometryUniformBlock(s32,u32,void*)
ext 027F29D4 BwdgProgramState* bwdgGetProgramState(BwdgSystem*)
ext 027BB9E0 void bwdgUpdateShaderProgram(BwdgShaderProgram*,u32)
ext 027B9F68 void bwdgBindProgramGroup(void*)
ext C00060E0 void bwdgCallDisplayList(void*,u32)
ext 027BB7CC void bwdgBindProgram(BwdgShaderProgram*)
ext 027FFE54 void bwdgApplyDrawContext(BwdgDrawContext*,BwdgShaderShape*)
ext 027BE53C void bwdgBindVertices(BwdgVertexBindingState*,u32,u32,u32)
ext 02750250 void bwdgDrawStateCtor(bwdg_draw_state*)
ext 0280037C void bwdgSetDrawMode(u32,bwdg_draw_state*)
ext 02750370 void bwdgApplyDrawState(bwdg_draw_state*)
ext 027BFE5C void bwdgBindFetchShader(BwdgFetchShader*)
ext C0006178 void bwdgDrawIndexed(u32,u32,void*,u32,u32,u32)
ext 02756140 BwdgHeap* bwdgGetCurrentHeap(BwdgHeapManager*)
ext 027F1278 void bwdgBasePacketCtor(void*)
ext 028EFFD0 void bwdgConstructArray(void*,u32,u32,BwdgConstructor)
ext 027FD6F4 void bwdgUniformContainerCtor(BwdgUniformContainer*)
ext 027FB40C void bwdgUniformCtor(void*)
ext 028F521C void bwdgClearMemory(void*,u32)
ext 027B5430 void bwdgIndexBufferCtor(BwdgIndexBuffer*)
ext 027BDF7C void bwdgVertexBindingCtor(BwdgVertexBindingState*)
ext 027BE6B8 void bwdgAttributeStateCtor(BwdgAttributeState*)
ext 027FFCBC BwdgShaderRegistry* bwdgGetShaderRegistry()
ext 027B90AC s32 bwdgFindShader(BwdgShaderArchive*,SafeString*)
ext 02800B0C void bwdgPrepareShader(BwdgShaderEntry*,BwdgShaderParameter*,u32)
ext 0280068C void bwdgSetShaderEntry(BwdgRenderState*,BwdgShaderEntry*,u32)
ext 027FF478 void bwdgSetVertexBuffer(BwdgAttributeBuffer*,void*,u32,be<u32>*)
ext 027FF530 void bwdgSetFetchShader(void*,BwdgFetchShader*,BwdgAttributeBuffer*,be<u32>*,u32)
ext 027FE084 void bwdgInitializeUniforms(BwdgUniformContainer*,u32,u32)
ext 027B54E0 void bwdgSetIndexBuffer(BwdgIndexBuffer*,BwdgIndexData*,u32,u32)
ext 026124B0 void* bwdgFindResource(BwdgResourceManager*,SafeString*,SafeString*,u32)
ext 0274FBF8 void bwdgLockGraphics(BwdgGraphicsDevice*)
ext 02773870 void bwdgConfigureAttributes(BwdgAttributeState*,void*,const char*)
ext 0274FCCC void bwdgUnlockGraphics(BwdgGraphicsDevice*)
ext 027BDEB4 void bwdgCopyAttributeState(BwdgVertexBindingState*,BwdgAttributeState*)
data boss 10462C04
data waveNormal 10462C24
data waveNormalInitialized 10462C30
data modelInfo 101930F0
data textureCoordinates 10193118
data modelIndices 1019B520
data bwdgSystem 104B45C0
data heapManager 101F8B4C
data graphicsDevice 101F8B18
data resourceManager 101F4F7C
data attributeDescriptor 1000C1E8
data indexData 101A7820
data zeroValue 10145180
data oneValue 1014517C
*/
