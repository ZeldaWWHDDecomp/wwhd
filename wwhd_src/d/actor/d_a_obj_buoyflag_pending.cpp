/* d_a_obj_buoyflag: member functions of the translation unit defined in another part file, as plain
 * guest calls (so the parts can call each other naturally). Weak: the decompiled definition in a
 * part replaces it. */
#include "d/actor/d_a_obj_buoyflag.h"

// HD: one TU owns the shared header vtable and all asset tables across the split specs.
const gc_tu_data gc_tu = {.safeString = 0x10026548};

// Model data (vertex positions, texture coordinates, index lists) generated from your copy of the
// game at build time (tools/verify/assets.py, build/verify/include/assets/); never committed.
#include "assets/buoyflag_positions__d_a_obj_buoyflag.h"
#include "assets/buoyflag_texCoords__d_a_obj_buoyflag.h"
#include "assets/buoyflag_strip__d_a_obj_buoyflag.h"
#include "assets/buoyflag_indices__d_a_obj_buoyflag.h"
#include "assets/buoyflag_polePositions__d_a_obj_buoyflag.h"
#include "assets/buoyflag_poleTexCoords__d_a_obj_buoyflag.h"
#include "assets/buoyflag_poleStrip__d_a_obj_buoyflag.h"
#include "assets/buoyflag_poleIndices__d_a_obj_buoyflag.h"
static u32 buoyflag_poleVertexCount = 35;
static u32 buoyflag_poleIndexCount = 45;
static u32 buoyflag_vertexCount = 62;
static u32 buoyflag_indexCount = 144;

static u32 buoyflag_attributes[3] = {0, 12, 24};

namespace daObjBuoyflag {
static Attr_c L_attr_type[4] = {{1.0f, 1, 1}, {4.0f, 0, 1}, {5.760000228881836f, 0, 1}, {2.0f, 0, 0}};

#define W __attribute__((weak))
W void Packet_c::draw_hata(Act_c* a) { buoyflag_call_Packet_c_draw_hata(this, a); }
W void Packet_c::draw_hasi(Act_c* a) { buoyflag_call_Packet_c_draw_hasi(this, a); }
W void Packet_c::draw(Act_c* a) { buoyflag_call_Packet_c_draw(this, a); }
W void Packet_c::calc_wind_base(Act_c* a) { buoyflag_call_Packet_c_calc_wind_base(this, a); }
W void Packet_c::calc_pos_spring_near(const cXyz* p, const cXyz* q, f32 l, f32 k) {
    buoyflag_call_Packet_c_calc_pos_spring_near(this, p, q, l, k);
}
W void Packet_c::calc_pos(Act_c* a) { buoyflag_call_Packet_c_calc_pos(this, a); }
W void Packet_c::calc_nrm() { buoyflag_call_Packet_c_calc_nrm(this); }
W void Packet_c::calc(Act_c* a) { buoyflag_call_Packet_c_calc(this, a); }
W void Packet_c::init(Act_c* a) { buoyflag_call_Packet_c_init(this, a); }
W void Packet_c::load_texture() { buoyflag_call_Packet_c_load_texture(this); }
W void Packet_c::update_hata() { buoyflag_call_Packet_c_update_hata(this); }
W void Packet_c::update_hasi() { buoyflag_call_Packet_c_update_hasi(this); }
W void Packet_c::update(Act_c* a) { buoyflag_call_Packet_c_update(this, a); }
W void Packet_c::gpu_init_hata() { buoyflag_call_Packet_c_gpu_init_hata(this); }
W void Packet_c::gpu_init_hasi() { buoyflag_call_Packet_c_gpu_init_hasi(this); }
W void Act_c::mtx_init() { buoyflag_call_Act_c_mtx_init(this); }
W BOOL Act_c::mode_afl() { return buoyflag_call_Act_c_mode_afl(this); }
W BOOL Act_c::mode_jumpToSea() { return buoyflag_call_Act_c_mode_jumpToSea(this); }
W u32 PrmAbstract(fopAc_ac_c* a, s32 w, s32 s) { return buoyflag_call_PrmAbstract(a, w, s); }
} // namespace daObjBuoyflag

// clang-format off
/* WWHD glue
data L_attr_type 100267F0
data buoyflag_attributes 10026810
data buoyflag_positions 101C83C0
data buoyflag_texCoords 101C8564
data buoyflag_strip 101C867C
data buoyflag_indices 101C8738
data buoyflag_polePositions 101C8274
data buoyflag_poleTexCoords 101C821C
data buoyflag_poleStrip 101C82F8
data buoyflag_poleIndices 101C8364
data buoyflag_poleVertexCount 101C81D8
data buoyflag_poleIndexCount 101C81DC
data buoyflag_vertexCount 101C81E0
data buoyflag_indexCount 101C81E4
ext 0201AEAC void buoyflag_vectorDivide(const cXyz *, cXyz *, float)
ext 0201B080 void buoyflag_vectorCross(const cXyz *, cXyz *, const cXyz *)
ext 0201B47C int buoyflag_vectorNormalizeRS(cXyz *)
ext 0201B31C void buoyflag_vectorNormalizeResult(cXyz *, cXyz *)
ext 028E91EC void buoyflag_matrixInverse(const Mtx34 *, Mtx34 *)
ext 028E9108 void buoyflag_matrixConcat(const Mtx34 *, const Mtx34 *, Mtx34 *)
ext 025F1AA4 void buoyflag_rotateZXY(Mtx34 *, short, short, short)
ext 028E9B14 void buoyflag_quaternionFromAxis(unsigned int, const cXyz *, float)
ext 025F25CC void buoyflag_stackQuaternion(unsigned int)
ext 025D6768 void buoyflag_setCullSphere(fopAc_ac_c *, float, float, float, float)
ext 0201864C void buoyflag_setCylinder(cM3dGCyl *, const cXyz *, float, float)
ext 0257E34C void buoyflag_getAllWind(cXyz *, cXyz *)
ext 0232BCE0 void buoyflag_call_hasi_nrm_init()
ext 028F521C void buoyflag_clearStorage(unsigned int, unsigned int)
ext 028EFFD0 void buoyflag_construct_array(unsigned int, unsigned int, unsigned int, unsigned int)
ext 027F1278 void buoyflag_constructPacket(daObjBuoyflag::Packet_c *)
ext 027B5430 void buoyflag_constructIndexBuffer(unsigned int)
ext 027FD6F4 void buoyflag_constructMaterialList(unsigned int)
ext 027FB40C void buoyflag_constructUniformObject(unsigned int)
ext 027BE6B8 void buoyflag_constructTextureResource(unsigned int)
ext 027BDF7C void buoyflag_constructTexture(unsigned int)
ext 027BF880 void buoyflag_destroyShader(unsigned int, int)
ext 027B5CBC void buoyflag_destroyVertexBuffer(unsigned int, int)
ext 027B5BD8 void buoyflag_constructVertexBuffer(unsigned int)
ext 027BF734 void buoyflag_constructShader(unsigned int)
ext 027BDEB4 void buoyflag_rebuildTexture(unsigned int, unsigned int)
ext 0274FBF8 void buoyflag_lockTextureManager(unsigned int)
ext 02773798 void buoyflag_loadTexture(unsigned int, unsigned int)
ext 0274FCCC void buoyflag_unlockTextureManager(unsigned int)
ext 0255F8F4 void buoyflag_updateLighting(unsigned int)
ext 0255FE90 void buoyflag_calculateLightingColors(unsigned int)
ext 027FDA54 void buoyflag_setMaterialView(unsigned int, int, Mtx34 *, unsigned int, unsigned int)
ext 0274D458 void buoyflag_scaleMaterialColor(BuoyColorScratch*, BuoyColorScratch*, float)
ext 027FDFF4 void buoyflag_flushDrawMaterial(unsigned int, int)
ext 0274D2AC void buoyflag_scaleUniformColor(unsigned int, float)
ext 027FB678 void buoyflag_flushUniformObject(unsigned int)
ext 027B5E94 void buoyflag_flushVertexBuffer(unsigned int, int, unsigned int)
ext 027F0E04 void buoyflag_entryImm(unsigned int, daObjBuoyflag::Packet_c *, int)
ext 027F29D4 unsigned int buoyflag_getActiveShader(unsigned int)
ext 027BB9E0 void buoyflag_updateShaderProgram(unsigned int, int)
ext 027B9F68 void buoyflag_bindShaderGroup(unsigned int)
ext C00060E0 void buoyflag_GX2CallDisplayList(unsigned int, unsigned int)
ext 027BB7CC void buoyflag_bindShaderProgram(unsigned int)
ext C0006900 void buoyflag_GX2SetPixelUniformBlock(short, unsigned int, unsigned int)
ext C0006A38 void buoyflag_GX2SetVertexUniformBlock(short, unsigned int, unsigned int)
ext C00068A8 void buoyflag_GX2SetGeometryUniformBlock(short, unsigned int, unsigned int)
ext 027BE53C void buoyflag_bindTexture(unsigned int, unsigned int, int, int)
ext 0280037C void buoyflag_configureRenderState(int, unsigned int)
ext 02750370 void buoyflag_applyRenderState(unsigned int)
ext 027BFE5C void buoyflag_bindShader(unsigned int)
ext C0006178 void buoyflag_GX2DrawIndexedEx(unsigned int, unsigned int, unsigned int, unsigned int, int, int)
ext 02750250 void buoyflag_initializeRenderState(unsigned int)
ext 027FFE54 void buoyflag_applyExtraMaterial(unsigned int, unsigned int)
ext 02756140 unsigned int buoyflag_getCurrentHeap(unsigned int)
ext 02755FEC unsigned int buoyflag_findOwningHeap(unsigned int, unsigned int)
ext 027FFCBC unsigned int buoyflag_getShaderArchives(unsigned int)
ext 027B90AC int buoyflag_findShaderArchive(unsigned int, SafeString *)
ext 02800B0C void buoyflag_initializeShaderArchive(unsigned int, unsigned int, int)
ext 0280068C void buoyflag_createShaderCollections(unsigned int, unsigned int, int)
ext 027FF478 void buoyflag_initializeVertexBuffer(unsigned int, unsigned int, int, unsigned int)
ext 027FF530 void buoyflag_initializeShader(unsigned int, unsigned int, unsigned int, unsigned int, int)
ext 027FE084 void buoyflag_createDrawMaterials(unsigned int, int, int)
ext 027B54E0 void buoyflag_initializeIndexBuffer(unsigned int, unsigned int, int, unsigned int)
ext 026124B0 unsigned int buoyflag_getArchiveResource(unsigned int, SafeString *, SafeString *, int)
ext 02773870 void buoyflag_loadNamedTexture(unsigned int, unsigned int, unsigned int)
ext 027BF7E8 void buoyflag_releaseShader(unsigned int)
ext 027BEBEC void buoyflag_destroyUniformBinding(unsigned int)
ext 027BE2B0 void buoyflag_destroyTexture(unsigned int, int)
ext 027FB528 void buoyflag_releaseUniformObject(unsigned int, int)
ext 027FD764 void buoyflag_destroyMaterialList(unsigned int, int)
ext 027B54A0 void buoyflag_destroyIndexBuffer(unsigned int, int)
ext 028F0164 void buoyflag_destroyArray(unsigned int, int, int, unsigned int, int, int)
ext 027F13DC void buoyflag_destroyMaterialPacket(unsigned int, int)
ext 025D50BC void buoyflag_destroyActor(daObjBuoyflag::Act_c *, int)
ext 02328FD4 void buoyflag_call_Packet_c_draw_hata(daObjBuoyflag::Packet_c *, daObjBuoyflag::Act_c *)
ext 023296B0 void buoyflag_call_Packet_c_draw_hasi(daObjBuoyflag::Packet_c *, daObjBuoyflag::Act_c *)
ext 02329C4C void buoyflag_call_Packet_c_draw(daObjBuoyflag::Packet_c *, daObjBuoyflag::Act_c *)
ext 02329CE4 void buoyflag_call_Packet_c_calc_wind_base(daObjBuoyflag::Packet_c *, daObjBuoyflag::Act_c *)
ext 0232A13C void buoyflag_call_Packet_c_calc_pos_spring_near(daObjBuoyflag::Packet_c *, const cXyz *, const cXyz *, float, float)
ext 0232A218 void buoyflag_call_Packet_c_calc_pos(daObjBuoyflag::Packet_c *, daObjBuoyflag::Act_c *)
ext 0232A7F4 void buoyflag_call_Packet_c_calc_nrm(daObjBuoyflag::Packet_c *)
ext 0232AEF8 void buoyflag_call_Packet_c_calc(daObjBuoyflag::Packet_c *, daObjBuoyflag::Act_c *)
ext 0232B03C void buoyflag_call_Packet_c_init(daObjBuoyflag::Packet_c *, daObjBuoyflag::Act_c *)
ext 0232B3B8 void buoyflag_call_Packet_c_load_texture(daObjBuoyflag::Packet_c *)
ext 0232B624 void buoyflag_call_Packet_c_update_hata(daObjBuoyflag::Packet_c *)
ext 0232B984 void buoyflag_call_Packet_c_update_hasi(daObjBuoyflag::Packet_c *)
ext 0232BBEC void buoyflag_call_Packet_c_update(daObjBuoyflag::Packet_c *, daObjBuoyflag::Act_c *)
ext 0232BDC0 void buoyflag_call_Packet_c_gpu_init_hata(daObjBuoyflag::Packet_c *)
ext 0232C670 void buoyflag_call_Packet_c_gpu_init_hasi(daObjBuoyflag::Packet_c *)
ext 0232CD40 void buoyflag_call_Act_c_mtx_init(daObjBuoyflag::Act_c *)
ext 0232CDB4 int buoyflag_call_Act_c_mode_afl(daObjBuoyflag::Act_c *)
ext 0232CF30 int buoyflag_call_Act_c_mode_jumpToSea(daObjBuoyflag::Act_c *)
ext 0232EDC4 unsigned int buoyflag_call_PrmAbstract(fopAc_ac_c *, int, int)
*/
// clang-format on
