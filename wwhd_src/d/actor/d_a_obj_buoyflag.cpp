/**
 * d_a_obj_buoyflag.cpp (WWHD)
 * Object - Buoy flag (a cloth flag on a buoy or barrel; jumps into the sea when released).
 *
 * The GameCube TU is "Nonmatching": written from the
 * WWHD code (cking.rpx) and verified against it, with the GameCube names. Part files:
 * d_a_obj_buoyflag_calc.cpp (cloth simulation), d_a_obj_buoyflag_draw.cpp (HD drawing),
 * d_a_obj_buoyflag_pending.cpp (weak guest-call stubs for the functions of the other parts).
 */
#include "d/actor/d_a_obj_buoyflag.h"

#define M_arcname "Cloth" /* "Cloth" */
#define ACT_VTBL 0x10026570
#define PACKET_VTBL 0x10026860

namespace daObjBuoyflag {
static void* elem10_ct(void*);
static void* vtx_ct(void*);
static void* vtx_ct2(void*);
using ModeProc = gabi::ptmf<BOOL (Act_c::*)()>;
struct ModeTable {
    ModeProc entries[2];
};
static ModeProc modeInitial[2] = {&Act_c::mode_afl, &Act_c::mode_jumpToSea};
static ModeTable modeTable;
static u32 modeInitialized;

// HD: daObj::PrmAbstract<Act_c::Prm_e>(actor, width, shift) (per TU)
/* 0232EDC4 */
u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {

    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}

/* 0232CD40 */
void Act_c::mtx_init() {

    PSMTXTrans(mDoMtx_stack_c::get(), current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    PSMTXCopy(mDoMtx_stack_c::get(), &m1090);
    PSMTXCopy(mDoMtx_stack_c::get(), &mOldMtx);
}

// HD: afloat (follows the setup matrix given by the parent)
// HD: the mode returns a continuation flag in WWHD.
/* 0232CDB4 */
BOOL Act_c::mode_afl() {

    f32 z = m1090.m[2][3];
    f32 y = m1090.m[1][3];
    f32 x = m1090.m[0][3];
    current.pos.y = y;
    current.pos.x = x;
    current.pos.z = z;
    mPacket.calc(this);
    fopAcM_setCullSizeSphere(this, 0.0f, 50.0f * scale.y, 0.0f, 90.0f * scale.x);
    if (attr_type(this, "d_a_obj_buoyflag.cpp", "(M_type >= 0) && (M_type < Type_Max)")->mCc) {
        f32 sx = scale.x;
        f32 r = sx * 5.0f;
        f32 sy = scale.y;
        gabi::Local<cXyz> center;
        center->x = 0.0f * sx;
        center->y = 12.0f * sy;
        center->z = 0.0f * (f32)scale.z;
        f32 h = sy * 88.0f;
        PSVECAdd(center, &current.pos, center);
        cM3dGCyl_Set(&mCyl.mCyl, center, r, h);
        dComIfG_Ccsp_Set(&mCyl);
    }
    return TRUE;
}

// HD: thrown into the sea (spins about a random axis, sinks)
/* 0232CF30 */
BOOL Act_c::mode_jumpToSea() {

    if (m1128) {
        mRotSpeed = 1000.0f;
        gravity = -8.0f;
        mRotAngle = 0;
        s16 rx = (s16)gabi::ftoi(cM_rndFX(4000.0f));
        s16 ry = (s16)gabi::ftoi(cM_rndFX(32768.0f));
        mDoMtx_ZXYrotS(mDoMtx_stack_c::get(), rx, ry, 0);
        PSMTXMultVecSR(mDoMtx_stack_c::get(), cXyz_BaseZ, &mRotAxis);
    }
    f32 wave = daSea_calcWave(current.pos.x, current.pos.z);
    f32 y = current.pos.y;
    if (y < gabi::fnmsubs(180.0f, scale.y, wave))
        return FALSE;
    f32 spd = gabi::fnmsubs((f32)(s16)mRotAngle, 0.005f, mRotSpeed);
    f32 k1, k2;
    if (y > wave) {
        spd = spd * 0.97f;
        k2 = 0.002f;
        k1 = 0.006f;
    } else {
        spd = spd * 0.92f;
        k2 = 0.015f;
        k1 = 0.04f;
    }
    s32 add = gabi::ftoi(spd);
    mRotSpeed = spd;
    mRotAngle = (s16)(mRotAngle + add);
    daObj_posMoveF_stream(this, nullptr, cXyz_Zero, k1, k2);
    PSMTXTrans(mDoMtx_stack_c::get(), current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_transM(0.0f, 60.0f, 0.0f);
    C_QUATRotAxisRad(mDoMtx_quatStack_get(), &mRotAxis, (f32)(s16)mRotAngle * 9.58738e-05f);
    mDoMtx_stack_quatM(mDoMtx_quatStack_get());
    buoyflag_concatMatrix(mDoMtx_stack_c::get(), &m10F0, mDoMtx_stack_c::get());
    mDoMtx_stack_transM(0.0f, -60.0f, 0.0f);
    PSMTXCopy(mDoMtx_stack_c::get(), &m1090);
    mPacket.calc(this);
    return TRUE;
}

void hasi_nrm_init_call() { buoyflag_call_hasi_nrm_init(); }

/* GHS inline constructor of a sub-object at `p`: allocates when the address is NULL */
static inline u32 inl_ct(u32 p, u32 size) {
    if (p == 0)
        p = gabi::ea(operator_new(size));
    return p;
}
static inline void buoyflag_clearMemory(u32 p, u32 n) { buoyflag_clearStorage(p, n); }
static inline void construct_array(u32 p, u32 n, u32 size, u32 ct) {
    buoyflag_construct_array(p, n, size, ct);
}

/* the HD GPU state's initial values (after the bzero): a zero cXyz, then 1.0 at every 0x10 bytes */
static inline void gpu_state_init(u32 M) {
    for (u32 i = 0; i < 44; i++)
        buoyflag_view<BuoyUniformState>(M)->initialColors[i] = (i >= 3 && ((i - 3) & 3) == 0) ? 1.0f : 0.0f;
}

// HD: Mthd_Create (Act_c::_create inlined)
/* 0232D1F8 */
static cPhs_State Mthd_Create(void* i_this) {

    Act_c* a = (Act_c*)i_this;
    u32 cond = a->actor_condition;
    if (!(cond & fopAcCnd_INIT_e)) {
        if (a != nullptr) {
            u32 A = gabi::ea(a);
            fopAc_ac_c_ct(a);
            a->__vtbl = ACT_VTBL;
            dCcD_Stts_ct(&a->mStts);
            dCcD_Cyl_ct(&a->mCyl, 0x10026560);
            buoyflag_constructPacket(&a->mPacket); /* J3DPacket */
            u32 P = gabi::ea(&a->mPacket);
            /* the flag's vertex buffers: {s32, sead::Buffer {count, ptr}, elements[4], ...} */
            buoyflag_view<Packet_c>(P)->flagGpu.programCount = 0;
            buoyflag_view<Packet_c>(P)->__vtbl = PACKET_VTBL;
            {
                u32 b = gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.programs);
                if (b == 0)
                    b = gabi::ea(operator_new(8));
                if (b != 0) {
                    buoyflag_view<BuoyList>(b)->array = 0;
                    buoyflag_view<BuoyList>(b)->count = 0;
                }
            }
            {
                u32 v = gabi::ea(&buoyflag_view<Packet_c>(P)->flagGpu.materialPool);
                if (v == 0)
                    v = gabi::ea(operator_new(0x968));
                if (v != 0) {
                    construct_array(v, 4, 0x254, gabi::guest_fn((const void*)vtx_ct));
                    buoyflag_view<BuoyMaterialPool>(v)->current = 0;
                    buoyflag_view<BuoyMaterialPool>(v)->unused = 0;
                    buoyflag_view<BuoyMaterialPool>(v)->stride = 0x20;
                    buoyflag_view<BuoyMaterialPool>(v)->initialized = 0;
                    buoyflag_view<BuoyMaterialPool>(v)->attributeCount = 0;
                    for (int k = 0; k < 2; k++)
                        for (int m = 0; m < 2; m++)
                            buoyflag_view<BuoyGpuMaterial>(v + (k + 2 * m) * sizeof(BuoyGpuMaterial))
                                ->vertices = nullptr;
                }
            }
            /* the flag's GPU state */
            u32 B = gabi::ea(&buoyflag_view<Act_c>(A)->mPacket.flagGpu.indexBuffer);
            buoyflag_constructIndexBuffer(B);
            buoyflag_constructMaterialList(gabi::ea(&buoyflag_view<Act_c>(A)->mPacket.flagGpu.uniforms));
            buoyflag_constructUniformObject(gabi::ea(&buoyflag_view<Act_c>(A)->mPacket.flagGpu.drawUniform));
            buoyflag_view<Act_c>(A)->mPacket.flagGpu.drawUniform.vtable = 0x1016EF84;
            buoyflag_clearMemory(gabi::ea(&buoyflag_view<Act_c>(A)->mPacket.flagGpu.modelMatrix), 0x34);
            inl_ct(gabi::ea(&a->mPacket.flagGpu.modelMatrix), 0x30);
            buoyflag_constructUniformObject(
                gabi::ea(&buoyflag_view<Act_c>(A)->mPacket.flagGpu.materialUniform));
            buoyflag_view<Act_c>(A)->mPacket.flagGpu.materialUniform.vtable = 0x1016EFB4;
            u32 M = gabi::ea(&buoyflag_view<Act_c>(A)->mPacket.flagGpu.uniformState);
            buoyflag_clearMemory(M, 0x2F0);
            gpu_state_init(M);
            construct_array(gabi::ea(&a->mPacket.flagGpu.uniformState.descriptors[0]), 2, 0x10,
                            gabi::guest_fn((const void*)elem10_ct));
            construct_array(gabi::ea(&a->mPacket.flagGpu.uniformState.descriptors[1]), 2, 0x10,
                            gabi::guest_fn((const void*)elem10_ct));
            construct_array(gabi::ea(&a->mPacket.flagGpu.uniformState.descriptors[2]), 2, 0x10,
                            gabi::guest_fn((const void*)elem10_ct));
            for (u32 i = 0; i < 8; i++)
                inl_ct(gabi::ea(&a->mPacket.flagGpu.uniformState.matrices[i]), sizeof(Mtx34));
            for (u32 i = 0; i < 6; i++)
                inl_ct(gabi::ea(&a->mPacket.flagGpu.uniformState.finalDescriptors[i]),
                       sizeof(BuoyDescriptor));
            buoyflag_view<Act_c>(A)->mPacket.flagGpu.uniformFlag = 0;
            buoyflag_constructTextureResource(
                gabi::ea(&buoyflag_view<Act_c>(A)->mPacket.flagGpu.primaryResource));
            buoyflag_constructTextureResource(
                gabi::ea(&buoyflag_view<Act_c>(A)->mPacket.flagGpu.alternateResource));
            buoyflag_constructTexture(gabi::ea(&buoyflag_view<Act_c>(A)->mPacket.flagGpu.primaryTexture));
            buoyflag_constructTexture(gabi::ea(&buoyflag_view<Act_c>(A)->mPacket.flagGpu.alternateTexture));
            /* the pole's vertex buffers */
            buoyflag_view<Act_c>(A)->mPacket.poleGpu.programCount = 0;
            {
                u32 b = gabi::ea(&buoyflag_view<Act_c>(A)->mPacket.poleGpu.programs);
                if (b == 0)
                    b = gabi::ea(operator_new(8));
                if (b != 0) {
                    buoyflag_view<BuoyList>(b)->array = 0;
                    buoyflag_view<BuoyList>(b)->count = 0;
                }
            }
            {
                u32 v = gabi::ea(&a->mPacket.poleGpu.materialPool);
                if (v == 0)
                    v = gabi::ea(operator_new(0x4C0));
                if (v != 0) {
                    construct_array(v, 2, 0x254, gabi::guest_fn((const void*)vtx_ct2));
                    buoyflag_view<BuoyPoleMaterialPool>(v)->current = 0;
                    buoyflag_view<BuoyPoleMaterialPool>(v)->attributeCount = 0;
                    buoyflag_view<BuoyPoleMaterialPool>(v)->unused = 0;
                    buoyflag_view<BuoyPoleMaterialPool>(v)->stride = 0x20;
                    buoyflag_view<BuoyPoleMaterialPool>(v)->initialized = 0;
                    buoyflag_view<BuoyGpuMaterial>(v)->vertices = nullptr;
                    buoyflag_view<BuoyGpuMaterial>(v + sizeof(BuoyGpuMaterial))->vertices = nullptr;
                }
            }
            buoyflag_constructIndexBuffer(gabi::ea(&buoyflag_view<Act_c>(A)->mPacket.poleGpu.indexBuffer));
            buoyflag_constructMaterialList(gabi::ea(&buoyflag_view<Act_c>(A)->mPacket.poleGpu.uniforms));
            buoyflag_constructUniformObject(gabi::ea(&buoyflag_view<Act_c>(A)->mPacket.poleGpu.drawUniform));
            buoyflag_view<Act_c>(A)->mPacket.poleGpu.drawUniform.vtable = 0x1016EF84;
            buoyflag_clearMemory(gabi::ea(&buoyflag_view<Act_c>(A)->mPacket.poleGpu.modelMatrix), 0x34);
            inl_ct(gabi::ea(&a->mPacket.poleGpu.modelMatrix), 0x30);
            buoyflag_constructUniformObject(
                gabi::ea(&buoyflag_view<Act_c>(A)->mPacket.poleGpu.materialUniform));
            u32 M2 = gabi::ea(&buoyflag_view<Act_c>(A)->mPacket.poleGpu.uniformState);
            buoyflag_view<Act_c>(A)->mPacket.poleGpu.materialUniform.vtable = 0x1016EFB4;
            buoyflag_clearMemory(M2, 0x2F0);
            gpu_state_init(M2);
            construct_array(gabi::ea(&a->mPacket.poleGpu.uniformState.descriptors[0]), 2, 0x10,
                            gabi::guest_fn((const void*)elem10_ct));
            construct_array(gabi::ea(&a->mPacket.poleGpu.uniformState.descriptors[1]), 2, 0x10,
                            gabi::guest_fn((const void*)elem10_ct));
            construct_array(gabi::ea(&a->mPacket.poleGpu.uniformState.descriptors[2]), 2, 0x10,
                            gabi::guest_fn((const void*)elem10_ct));
            for (u32 i = 0; i < 8; i++)
                inl_ct(gabi::ea(&a->mPacket.poleGpu.uniformState.matrices[i]), sizeof(Mtx34));
            for (u32 i = 0; i < 6; i++)
                inl_ct(gabi::ea(&a->mPacket.poleGpu.uniformState.finalDescriptors[i]),
                       sizeof(BuoyDescriptor));
            buoyflag_view<Act_c>(A)->mPacket.poleGpu.uniformFlag = 0;
            buoyflag_constructTextureResource(
                gabi::ea(&buoyflag_view<Act_c>(A)->mPacket.poleGpu.textureResource));
            buoyflag_constructTexture(gabi::ea(&buoyflag_view<Act_c>(A)->mPacket.poleGpu.texture));
            buoyflag_constructTextureResource(gabi::ea(&buoyflag_view<Act_c>(A)->mPacket.toonResource));
            buoyflag_constructTexture(gabi::ea(&buoyflag_view<Act_c>(A)->mPacket.toonTexture));
            hasi_nrm_init_call();
            a->mPacket.gpu_init_hata();
            a->mPacket.gpu_init_hasi();
            cond = a->actor_condition;
        }
        a->actor_condition = cond | fopAcCnd_INIT_e;
    }
    a->mType = PrmAbstract(a, 2, 0);
    cPhs_State phase = dComIfG_resLoad(&a->mPhs, M_arcname);
    if (phase == cPhs_COMPLEATE_e) {
        f32 s = attr_type(a, "d_a_obj_buoyflag.cpp", "(M_type >= 0) && (M_type < Type_Max)")->mScale;
        a->scale.x = s;
        a->cullMtx = gabi::ea(&a->m1090); /* fopAcM_SetMtx */
        a->scale.y = s;
        a->scale.z = s;
        fopAcM_setCullSizeSphere(a, 0.0f, 50.0f, 0.0f, 2000.0f);
        a->mtx_init();
        if (PrmAbstract(a, 1, 0x1F)) /* prm_get_noCull */
            a->actor_status &= ~0x180u;
        if (attr_type(a, "d_a_obj_buoyflag.cpp", "(M_type >= 0) && (M_type < Type_Max)")->mCc) {
            a->mStts.Init(0xFF, 0xFF, a);
            a->mCyl.Set(gabi::at<dCcD_SrcCyl>(0x1002681C)); /* M_cyl_src */
            a->mCyl.mStts = &a->mStts;
        }
        a->mPacket.mpActor = a;
        a->mPacket.init(a);
        a->m1124 = 0;
        a->m1128 = 1;
    }
    return phase;
}

/* 0232DABC */
static BOOL Mthd_Delete(void* i_this) {

    dComIfG_resDelete(&((Act_c*)i_this)->mPhs, M_arcname);
    return TRUE;
}

/* 0232DAEC */
static BOOL Mthd_Execute(void* i_this) {

    Act_c* a = (Act_c*)i_this;
    // HD: preserve the original lazy initialization and store order.
    if (static_cast<u32>(modeInitialized) == 0) {
        modeInitialized = 1;
        memcpy_g(&modeTable, &modeInitial, sizeof(modeInitial));
    }
    if ((a->*(*buoyflag_view<ModeProc>(gabi::ea(&modeTable) + sizeof(ModeProc) * (u32)a->m1124)))()) {
        a->m1128 = 0;
        PSMTXCopy(&a->m1090, &a->mOldMtx);
        return TRUE;
    }
    fopAcM_delete(a);
    return TRUE;
}

/* 0232DBD0 */
static BOOL Mthd_Draw(void* i_this) {

    Act_c* a = (Act_c*)i_this;
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &a->current.pos, &a->tevStr);
    a->mPacket.update(a);
    return TRUE;
}

/* 0232EDBC */
static BOOL Mthd_IsDelete(void* i_this) { return TRUE; }

// HD: compiler-generated and template code of this translation unit.
/* 0232DC1C */
static void __sinit_d_a_obj_buoyflag_cpp() { sinit_header_statics(0x1046934C, 0x101C81B4); }

// HD: deleting destructor of a class with a trivial destructor
/* 0232DCB0 */
static void trivial_dt(void* p, s32 flags) {

    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

// HD: constructor of a trivial 0x10-byte element (allocates when NULL)
/* 0232DCC4 */
static void* elem10_ct(void* p) {

    if (p == nullptr)
        p = operator_new(0x10);
    return p;
}

// HD: , 0232E54C: empty virtuals
/* 0232DCF0 */
static void empty_virtual(void* p) {}
/* 0232E54C */
static void empty_virtual2(void* p) {}

// HD: , 0232DD54: deleting destructors of the 0x254-byte vertex buffer element (two copies)
/* 0232DCF4 */
static void vtx_dt(void* p, s32 flags) {

    if (p != nullptr) {
        buoyflag_destroyShader(gabi::ea(&buoyflag_view<BuoyGpuMaterial>(gabi::ea(p))->shaderObject), 2);
        buoyflag_destroyVertexBuffer(gabi::ea(&buoyflag_view<BuoyGpuMaterial>(gabi::ea(p))->vertexBuffer), 2);
        if (flags & 1)
            operator_delete(p);
    }
}
/* 0232DD54 */
static void vtx_dt2(void* p, s32 flags) {

    if (p != nullptr) {
        buoyflag_destroyShader(gabi::ea(&buoyflag_view<BuoyGpuMaterial>(gabi::ea(p))->shaderObject), 2);
        buoyflag_destroyVertexBuffer(gabi::ea(&buoyflag_view<BuoyGpuMaterial>(gabi::ea(p))->vertexBuffer), 2);
        if (flags & 1)
            operator_delete(p);
    }
}

// HD: , 0232E5AC: constructors of the 0x254-byte vertex buffer element (two copies)
/* 0232E550 */
static void* vtx_ct(void* p) {

    if (p == nullptr) {
        p = operator_new(0x254);
        if (p == nullptr)
            return p;
    }
    buoyflag_constructVertexBuffer(gabi::ea(&buoyflag_view<BuoyGpuMaterial>(gabi::ea(p))->vertexBuffer));
    buoyflag_constructShader(gabi::ea(&buoyflag_view<BuoyGpuMaterial>(gabi::ea(p))->shaderObject));
    buoyflag_view<BuoyGpuMaterial>(gabi::ea(p))->allocation = nullptr;
    buoyflag_view<BuoyGpuMaterial>(gabi::ea(p))->allocationCount = 0;
    return p;
}
/* 0232E5AC */
static void* vtx_ct2(void* p) {

    if (p == nullptr) {
        p = operator_new(0x254);
        if (p == nullptr)
            return p;
    }
    buoyflag_constructVertexBuffer(gabi::ea(&buoyflag_view<BuoyGpuMaterial>(gabi::ea(p))->vertexBuffer));
    buoyflag_constructShader(gabi::ea(&buoyflag_view<BuoyGpuMaterial>(gabi::ea(p))->shaderObject));
    buoyflag_view<BuoyGpuMaterial>(gabi::ea(p))->allocation = nullptr;
    buoyflag_view<BuoyGpuMaterial>(gabi::ea(p))->allocationCount = 0;
    return p;
}

} // namespace daObjBuoyflag

using namespace daObjBuoyflag;
// clang-format off
/* WWHD glue
str 10026530 "Cloth"
str 10026730 "d_a_obj_buoyflag.cpp" in 0232CDB4
str 10026748 "(M_type >= 0) && (M_type < Type_Max)" in 0232CDB4
str 100267A8 "d_a_obj_buoyflag.cpp" in 0232D1F8
str 100267C0 "(M_type >= 0) && (M_type < Type_Max)" in 0232D1F8
data modeInitial 10026538
data modeTable 101FDB88
data modeInitialized 101FDB98
*/
// clang-format on
