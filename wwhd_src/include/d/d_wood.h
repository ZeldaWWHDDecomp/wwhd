/* Wood/bush engine. Animation matches GameCube with compact HD member-function
 * descriptors. HD expands Unit lighting, omits shadow matrices, and stores room
 * members in bounded pointer arrays. Renderer layout remains private to this TU. */
#pragma once
#include "wwhd.h"

namespace dWood {
struct Packet_c;
struct DrawInfo;
enum AnmID_e : s32 {
    AnmID_Norm_Invalid = -1,
    AnmID_Norm0,
    AnmID_Norm1,
    AnmID_Norm2,
    AnmID_Norm3,
    AnmID_Norm4,
    AnmID_Norm5,
    AnmID_Norm6,
    AnmID_Norm7,
    AnmID_Norm_Max,
};
struct Anm_c {
    enum Mode_e : u32 {
        Mode_Cut,
        Mode_PushInto,
        Mode_PushBack,
        Mode_Fan,
        Mode_Norm,
        Mode_ToNorm,
        Mode_Max,
    };
    /* 0x00 */ be<f32> mModelMtx[3][4];
    /* 0x30 */ be<f32> mTrunkModelMtx[3][4];
    /* 0x60 */ be<u32> mMode;
    /* 0x64 */ be<s16> mCountdown;
    /* 0x66 */ be<s16> mForceDir;
    /* 0x68 */ be<f32> mWindPow;
    /* 0x6C */ be<f32> mPosOffsetY;
    /* 0x70 */ be<f32> mPosOffsetZ;
    /* 0x74 */ be<f32> mVelY;
    /* 0x78 */ be<s16> mPhaseY[2];
    /* 0x7C */ be<s16> mPhaseX[2];
    /* 0x80 */ be<s16> mAmpY[2];
    /* 0x84 */ be<s16> mAmpX[2];
    /* 0x88 */ be<s16> mNormAnimIdx;
    /* 0x8A */ be<u8> mAlphaScale;
    u8 mPadding[1];

    Anm_c();
    void play(Packet_c *packet);
    void copy_angamp(const Anm_c *other);
    void mode_cut_init(const Anm_c *other, s16 targetAngle);
    void mode_cut(Packet_c *packet);
    void mode_push_into_init(const Anm_c *other, s16 targetAngle);
    void mode_push_into(Packet_c *packet);
    void mode_push_back_init();
    void mode_push_back(Packet_c *packet);
    void mode_fan(Packet_c *packet);
    void mode_norm_init();
    void mode_norm(Packet_c *packet);
    void mode_norm_set_wind(f32 windPow, s16 windDir);
    // HD: the incoming animation ID is compared as an unsigned full register.
    void mode_to_norm_init(u32 anm_id_norm);
    void mode_to_norm(Packet_c *packet);
};
} // namespace dWood
WWHD_OFFSET(dWood::Anm_c, mTrunkModelMtx, 0x30);
WWHD_OFFSET(dWood::Anm_c, mMode, 0x60);
WWHD_OFFSET(dWood::Anm_c, mCountdown, 0x64);
WWHD_OFFSET(dWood::Anm_c, mForceDir, 0x66);
WWHD_OFFSET(dWood::Anm_c, mWindPow, 0x68);
WWHD_OFFSET(dWood::Anm_c, mPosOffsetY, 0x6C);
WWHD_OFFSET(dWood::Anm_c, mPosOffsetZ, 0x70);
WWHD_OFFSET(dWood::Anm_c, mVelY, 0x74);
WWHD_OFFSET(dWood::Anm_c, mPhaseY, 0x78);
WWHD_OFFSET(dWood::Anm_c, mPhaseX, 0x7C);
WWHD_OFFSET(dWood::Anm_c, mAmpY, 0x80);
WWHD_OFFSET(dWood::Anm_c, mAmpX, 0x84);
WWHD_OFFSET(dWood::Anm_c, mNormAnimIdx, 0x88);
WWHD_OFFSET(dWood::Anm_c, mAlphaScale, 0x8A);
WWHD_SIZE(dWood::Anm_c, 0x8C);

enum AttrSway_e { SWAY_LIGHT, SWAY_MEDIUM, SWAY_STRONG, SWAY_PUSH };
namespace dWood {
struct AttrSway_c {
    be<s16> phaseVelY, ampY, phaseVelX, ampX;
    be<f32> phaseBiasX;
};
} // namespace dWood
WWHD_OFFSET(dWood::AttrSway_c, phaseBiasX, 8);
WWHD_SIZE(dWood::AttrSway_c, 12);
void PSMTXIdentity(u32 matrix);
void cMtx_YrotS(u32 matrix, s32 angle);
void wood_stack_transM(f32 x, f32 y, f32 z);
void cMtx_XrotM(u32 matrix, s32 angle);
void cMtx_YrotM(u32 matrix, s32 angle);
void PSMTXCopy(u32 source, u32 target);
void cLib_chaseAngleS(u32 value, s32 target, s32 step);
void cLib_chaseS(u32 value, s32 target, s32 step);
void cLib_addCalcAngleS(u32 value, s32 target, s32 scale, s32 step, s32 minimum);
void wood_assert(u32 file, u32 line, u32 text);

// HD: only the animation prefix is viewed here; the renderer extends Packet_c to 0x7D3D0.
namespace dWood {
struct PacketAnimationView {
    u8 mPacketAndUnits[0x1C8D8];
    Anm_c mAnm[72];
};
} // namespace dWood
WWHD_OFFSET(dWood::PacketAnimationView, mAnm, 0x1C8D8);
WWHD_SIZE(dWood::PacketAnimationView, 0x1F038);

using WoodModeProc = void (dWood::Anm_c::*)(dWood::Packet_c *);
struct WoodModeProcEntry {
    gabi::ptmf<WoodModeProc> fn;
};
WWHD_SIZE(WoodModeProcEntry, 8);

#include "bindings.h"
namespace dWood {
// HD: J3DLightObj is larger, and the light record carries two extra parameter copies.
struct LightObj {
    /* 0x00 */ be<f32> mPositionAndDirection[6];
    /* 0x18 */ be<u8> mColor[4];
    /* 0x1C */ be<u16> mColorS10[4];
    /* 0x24 */ be<f32> mFactors[8];
    u8 mRendererData[0x40];
};
struct TevStr {
    /* 0x000 */ LightObj mLightObj;
    u8 mWorldPosition[0xC];
    be<s16> mColorC0[4];
    /* 0x098 */ GXColor mColorK0;
    u8 mFogAndEnvironment[0x24];
    /* 0x0C0 */ LightObj mHdLightParam[2];
};
struct Unit_c {
    enum State_e : u32 { STATE_ACTIVE = 1, STATE_FRUSTUM_CULLED = 2, STATE_CUT = 4 };
    /* 0x000 */ cXyz mPos;
    /* 0x00C */ be<s16> mAnimCooldown;
    u8 mPaddingE[2];
    /* 0x010 */ be<u32> mFlags;
    /* 0x014 */ be<s32> mAnmIdx;
    /* 0x018 */ Mtx34 mModelViewMtx;
    /* 0x048 */ Mtx34 mTrunkModelViewMtx;
    u8 mHDShadowState[4];
    /* 0x07C */ TevStr mTevStr;
    /* 0x244 */ be<f32> mDepth;
    Unit_c();
    void clear();
    s32 set_ground();
    void set_mtx(Anm_c *anim);
    void cc_hit_before_cut(Packet_c *packet);
    void proc(Packet_c *packet);
};
// HD: rooms own a bounded pointer array instead of the GameCube linked list.
struct Room_c {
    /* 0x000 */ be<s32> mCount;
    /* 0x004 */ be<s32> mCapacity;
    /* 0x008 */ gptr<gptr<Unit_c>> mpUnits;
    /* 0x00C */ gptr<Unit_c> mUnitStorage[200];
    Room_c();
    void entry_unit(Unit_c *unit);
    void delete_all_unit();
};
// HD: GX2 replaces GameCube display lists with shader groups, vertex buffers and uniforms.
struct ShaderMaterial;
struct UniformVtable {
    u8 mPrefix[0x2C];
    gfn apply;
};
struct Transform {
    u8 mUniformBase[0xC];
    gptr<UniformVtable> mpVtable;
    u8 mUniformState[0x64];
    Mtx34 mMatrix;
    u8 mPadding[4];
};
struct Vector4 {
    be<f32> mValues[4];
};
struct TextureUniformData {
    Vector4 mColors[11];
    Vector4 mVectors[3][2];
    Mtx34 mMatrices[8];
    Vector4 mTailVectors[6];
};
struct TextureTransform {
    u8 mUniformBase[0xC];
    gptr<UniformVtable> mpVtable;
    u8 mUniformState[0x64];
    TextureUniformData mData;
};
struct IndexBuffer {
    be<u32> mPrimitive;
    gptr<void> mpIndices;
    be<u32> mIndexType;
    be<u32> mCount;
    be<u32> mOffset;
    u8 mPadding[4];
};
struct ShapeVtable {
    u8 mPrefix[12];
    gfn mDestroy;
};
struct Shape {
    u8 mShapeState[0xF0];
    gptr<ShapeVtable> mpVtable;
};
struct ShapeArray {
    be<s32> mCount;
    gptr<Shape> mpShapes;
};
struct ShapeGroup {
    gptr<ShaderMaterial> mpMaterial;
    ShapeArray mShapes[2];
};
struct ShapeGroups {
    be<u32> mCount;
    be<u32> mCapacity;
    gptr<ShapeGroup> mpGroups;
};
struct Vertex {
    be<f32> mPosition[3];
    be<f32> mNormal[3];
    be<f32> mTexCoord[2];
    be<f32> mColor[4];
};
struct VertexBuffer {
    u8 mPrefix[0x14C];
    be<u32> mFlushSize;
    u8 mTail[4];
};
struct MaterialPart {
    gptr<Vertex> mpVertices;
    VertexBuffer mVertexBuffer;
    Shape mShape;
    be<u32> mVertexCapacity;
    gptr<Vertex> mpVertexStorage;
};
struct MaterialBuffers {
    MaterialPart mPart[2][2];
    be<u32> mFrameIndex;
    be<u32> mVertexFormat;
    be<u32> mVertexSize;
    gptr<void> mpVertexDeclaration;
    be<u32> mState;
    be<u8> mReady;
    u8 mPadding[3];
};
struct TransformSet {
    be<u32> mState;
    gptr<void> mpDescriptor;
    gptr<Transform> mpTransform;
};
struct FadePacket {
    u8 mPacket[12];
    be<u32> mVtable;
    u8 mPacketData[0x88];
    gptr<Packet_c> mpOwner;
};
struct PacketBase {
    u8 mPrefix[12];
    be<u32> mVtable;
    u8 mPacketData[0x88];
};
struct SamplerRecord {
    be<u32> mWords[0x190 / 4];
    be<u8> mFlags;
    u8 mTail[7];
};
struct SamplerPair {
    SamplerRecord mState;
    u8 mResource[0x90];
};
struct Packet_c {
    PacketBase mPacketBase;
    /* 0x00098 */ Unit_c mUnit[200];
    /* 0x1C8D8 */ Anm_c mAnm[72];
    /* 0x1F038 */ Room_c mRoom[64];
    /* 0x2BB38 */ ShapeGroups mRoomShapes;
    /* 0x2BB44 */ MaterialBuffers mMaterials;
    /* 0x2C4AC */ TransformSet mRoomTransformSet;
    /* 0x2C4B8 */ Transform mRoomTransforms[400];
    /* 0x3CB38 */ TextureTransform mRoomTextures[64];
    /* 0x4A438 */ IndexBuffer mLeafIndices;
    /* 0x4A450 */ IndexBuffer mTrunkIndices;
    /* 0x4A468 */ SamplerPair mSamplerState;
    /* 0x4A690 */ FadePacket mFadePacket;
    /* 0x4A72C */ Room_c mFadeUnits;
    /* 0x4AA58 */ ShapeGroups mFadeShapes;
    /* 0x4AA64 */ TransformSet mFadeTransformSet;
    /* 0x4AA70 */ Transform mFadeTransforms[200];
    /* 0x52DB0 */ TextureTransform mFadeTextures[200];
    void delete_room(s32 room_no);
    s32 search_anm(u32 mode);
    void calc_cc();
    void draw_fade(DrawInfo *draw);
    void draw_rooms(DrawInfo *draw);
    void calc();
    void update();
    s32 search_empty_UnitID();
    s32 put_unit(const cXyz *pos, u32 room);
};
struct HitInf {
    gptr<dCcD_GObjInf> mpArea, mpAtObj, mpCoObj;
    be<f32> mCoHitLen;
    be<u32> __vtbl;
};
struct EmitterColor {
    gptr<char> mName;
    be<u32> __vtbl;
    be<u32> mColor[4];
    be<u8> mEnabled;
    be<u8> mPadding[3];
};
struct ParticleColorView {
    u8 mControlPrefix[0x38];
    be<u32> mColor[4];
};
} // namespace dWood
WWHD_OFFSET(dWood::LightObj, mPositionAndDirection, 0);
WWHD_OFFSET(dWood::LightObj, mColor, 0x18);
WWHD_OFFSET(dWood::LightObj, mColorS10, 0x1C);
WWHD_SIZE(dWood::LightObj, 0x84);
WWHD_OFFSET(dWood::LightObj, mFactors, 0x24);
WWHD_OFFSET(dWood::TevStr, mColorK0, 0x98);
WWHD_OFFSET(dWood::TevStr, mHdLightParam, 0xC0);
WWHD_SIZE(dWood::TevStr, 0x1C8);
WWHD_OFFSET(dWood::Unit_c, mAnimCooldown, 0xC);
WWHD_OFFSET(dWood::Unit_c, mFlags, 0x10);
WWHD_OFFSET(dWood::Unit_c, mAnmIdx, 0x14);
WWHD_OFFSET(dWood::Unit_c, mModelViewMtx, 0x18);
WWHD_OFFSET(dWood::Unit_c, mTrunkModelViewMtx, 0x48);
WWHD_OFFSET(dWood::Unit_c, mTevStr, 0x7C);
WWHD_OFFSET(dWood::Unit_c, mDepth, 0x244);
WWHD_SIZE(dWood::Unit_c, 0x248);
WWHD_OFFSET(dWood::Room_c, mpUnits, 8);
WWHD_OFFSET(dWood::Room_c, mUnitStorage, 0xC);
WWHD_SIZE(dWood::Room_c, 0x32C);
WWHD_OFFSET(dWood::Packet_c, mUnit, 0x98);
WWHD_OFFSET(dWood::Packet_c, mAnm, 0x1C8D8);
WWHD_OFFSET(dWood::Packet_c, mRoom, 0x1F038);
WWHD_SIZE(dWood::Packet_c, 0x7D3D0);
WWHD_SIZE(dWood::HitInf, 20);
WWHD_OFFSET(dWood::HitInf, mCoHitLen, 0xC);
WWHD_SIZE(dWood::EmitterColor, 28);
WWHD_OFFSET(dWood::ParticleColorView, mColor, 0x38);
WWHD_SIZE(dWood::ParticleColorView, 0x48);
void woodMemorySet(void *object, u32 value, u32 size);
void woodGroundConstruct(cBgS_GndChk *check);
void woodGroundDestruct(cBgS_GndChk *check, s32 flags);
void woodHitConstruct(dWood::HitInf *hit);
u32 woodCheckMass(void *cc, cXyz *pos, gptr<fopAc_ac_c> *actor, dWood::HitInf *hit);
void woodMatrixConcat(Mtx34 *first, Mtx34 *second, Mtx34 *output);
void woodRoomArrayCreate(dWood::Room_c *room, s32 capacity, gptr<dWood::Unit_c> *storage);
void woodEmitterSetColor(JPABaseEmitter *emitter, dWood::EmitterColor *color);

s32 woodTargetAngleY(const cXyz *from, const cXyz *to);

WWHD_SIZE(dWood::Transform, 0xA8);
WWHD_SIZE(dWood::TextureTransform, 0x364);
WWHD_SIZE(dWood::IndexBuffer, 0x18);
WWHD_SIZE(dWood::Shape, 0xF4);
WWHD_SIZE(dWood::ShapeGroup, 20);
WWHD_SIZE(dWood::ShapeGroups, 12);
WWHD_SIZE(dWood::Vertex, 48);
WWHD_SIZE(dWood::MaterialPart, 0x254);
WWHD_SIZE(dWood::MaterialBuffers, 0x968);
WWHD_SIZE(dWood::TransformSet, 12);
WWHD_SIZE(dWood::FadePacket, 0x9C);
WWHD_OFFSET(dWood::Packet_c, mMaterials, 0x2BB44);
WWHD_OFFSET(dWood::Packet_c, mRoomTransforms, 0x2C4B8);
WWHD_OFFSET(dWood::Packet_c, mRoomTextures, 0x3CB38);
WWHD_OFFSET(dWood::Packet_c, mLeafIndices, 0x4A438);
WWHD_OFFSET(dWood::Packet_c, mFadePacket, 0x4A690);
WWHD_OFFSET(dWood::Packet_c, mFadeUnits, 0x4A72C);
WWHD_OFFSET(dWood::Packet_c, mFadeShapes, 0x4AA58);
WWHD_OFFSET(dWood::Packet_c, mFadeTransforms, 0x4AA70);
WWHD_OFFSET(dWood::Packet_c, mFadeTextures, 0x52DB0);

// HD: GX2 renderer and library bindings used by this translation unit.
void woodMassSetHeight(u32 a0, f32 a1);
void woodMassSetRadius(u32 a0, f32 a1);
s32 woodAtan2(f32 a0, f32 a1);
u32 woodGetGameInfo();
u32 woodGetEnvironment();
void woodSetLightColor(u32 a0);
void woodInitLight(u32 a0, u32 a1, u32 a2);
void woodSetupEnvironment(u32 a0, u32 a1, u32 a2, u32 a3);
f32 woodGetWindPower();
u32 woodGetWindVector();
u32 woodGetResource(u32 a0, u32 a1, u32 a2, u32 a3);
u32 woodAllocate(u32 a0);
void woodFree(u32 a0);
void woodCreatePointerArray(u32 a0, u32 a1, u32 a2);
void woodSortPointerArray(u32 a0, u32 a1);
void woodScaleColor(u32 a0, f32 a1);
void woodConvertFogColor(u32 a0, u32 a1, f32 a2);
void woodLockGraphics(u32 a0);
void woodUnlockGraphics(u32 a0);
void woodGraphicsStateConstruct(u32 a0);
void woodGraphicsStateApply(u32 a0);
u32 woodFindHeap(u32 a0, u32 a1);
u32 woodGetHeap(u32 a0);
void woodLoadSamplerResource(u32 a0, u32 a1, u32 a2);
void woodIndexBufferConstruct(u32 a0);
void woodIndexBufferDestroy(u32 a0, u32 a1);
void woodIndexBufferSet(u32 a0, u32 a1, u32 a2, u32 a3);
void woodVertexBufferConstruct(u32 a0);
void woodVertexBufferDestroy(u32 a0, u32 a1);
void woodVertexBufferFlush(u32 a0, u32 a1, u32 a2);
s32 woodFindMaterialIndex(u32 a0, u32 a1);
void woodBindShaderState(u32 a0);
void woodBindShaderProgram(u32 a0);
void woodUpdateShaderProgram(u32 a0, u32 a1);
void woodCopySamplerState(u32 a0, u32 a1);
void woodSamplerConstruct(u32 a0);
void woodSamplerDestroy(u32 a0, u32 a1);
void woodBindSamplers(u32 a0, u32 a1, u32 a2, u32 a3);
void woodSamplerResourceConstruct(u32 a0);
void woodShapeConstruct(u32 a0);
void woodShapeClear(u32 a0);
void woodShapeDestroy(u32 a0, u32 a1);
void woodShapeBind(u32 a0);
void woodDrawListEntry(u32 a0, u32 a1, u32 a2);
void woodPacketBaseConstruct(u32 a0);
void woodPacketBaseDestroy(u32 a0, u32 a1);
u32 woodGetShaderCache(u32 a0);
void woodUniformConstruct(u32 a0);
void woodUniformDestroy(u32 a0, u32 a1);
void woodUniformSetState(u32 a0, u32 a1);
void woodUniformUpdate(u32 a0);
void woodTransformSetConstruct(u32 a0);
void woodTransformSetDestroy(u32 a0, u32 a1);
void woodTransformSetConfigure(u32 a0, u32 a1, u32 a2);
void woodTransformSetView(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4);
void woodTransformSetUpdate(u32 a0);
void woodConfigureVertexBuffer(u32 a0, u32 a1, u32 a2, u32 a3);
void woodConfigureShape(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4);
u32 woodGetMaterialManager();
void woodApplyDrawObject(u32 a0, u32 a1);
void woodApplyDrawMode(u32 a0, u32 a1);
void woodBindMaterialGroups(u32 a0, u32 a1, u32 a2);
void woodCreateMaterialInstance(u32 a0, u32 a1, u32 a2);
s32 woodFrustumCheck(u32 a0, u32 a1, u32 a2, f32 a3);
void woodSecondStaticFlagConstruct(u32 a0);
void woodFirstStaticFlagConstruct(u32 a0);
void woodArrayConstruct(u32 a0, u32 a1, u32 a2, u32 a3);
void woodArrayDestroy(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5);
void woodRegisterStatic(u32 a0);
void woodClearUniformData(u32 a0, u32 a1);
void woodGX2BindProgram(u32 a0, u32 a1);
void woodGX2DrawIndexed(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5);
void woodGX2GeometryUniform(u32 a0, u32 a1, u32 a2);
void woodGX2PixelUniform(u32 a0, u32 a1, u32 a2);
void woodGX2VertexUniform(u32 a0, u32 a1, u32 a2);

struct WoodMassView {
    u8 mPrefix[0x5008];
    u8 mShape[0x18];
    be<u8> mAttribute;
    be<u8> mEnabled;
};
WWHD_OFFSET(WoodMassView, mAttribute, 0x5020);
WWHD_SIZE(WoodMassView, 0x5022);

namespace dWood {
struct ShaderProgramState {
    u8 mPrefix[0x28];
    be<u32> mState;
};
struct ShaderProgram {
    be<u8> mFlags;
    u8 mPadding[3];
    gptr<void> mpBinary;
    u8 mPadding8[4];
    be<u32> mBinarySize;
    u8 mPrefix10[0x6C];
    gptr<ShaderProgramState> mpState;
};
struct UniformSlots {
    u8 mPrefix[0xC];
    be<s16> mVertex;
    be<s16> mPixel;
    be<s16> mGeometry;
};
struct ShaderMaterial {
    gptr<ShaderProgram> mpProgram;
    u8 mPrefix[8];
    be<u32> mUniformCount;
    gptr<UniformSlots> mpUniforms;
    be<u32> mSamplerCount;
    gptr<void> mpSamplers;
};
struct ShaderCache {
    be<u32> mState;
    gptr<ShaderProgram> mpProgram;
};
struct UniformDescriptor {
    be<u32> mFlags;
    gptr<void> mpData;
    be<u32> mCapacity;
    be<u32> mSize;
    u8 mPrefix[12];
};
struct UniformShape {
    u8 mPrefix[0x10];
    UniformDescriptor mDescriptors[2];
    u8 mPadding[4];
    be<u32> mDescriptorIndex;
    u8 mShapeData[0x23C - 0x50];
};
struct DrawShapeTable {
    be<u32> mCapacity;
    gptr<UniformShape> mpShapes;
};
struct UniformObject {
    u8 mPrefix[0xC];
    gptr<UniformVtable> mpVtable;
};
struct DrawInfo {
    u8 mPrefix[4];
    be<u32> mShapeIndex;
    u8 mPadding[4];
    be<u32> mMode;
    u8 mPadding10[4];
    gptr<DrawShapeTable> mpShapeTable;
    u8 mPadding18[0x18];
    gptr<UniformObject> mpObject;
};
struct GraphicsState {
    be<u8> mEnabled;
    be<u8> mSecondaryEnabled;
    u8 mPrefix[10];
    be<u32> mBlendMode;
    u8 mPrefix10[0xD0];
    be<u8> mDepthEnabled;
    u8 mPadding[3];
    be<u32> mDepthFunction;
    be<f32> mDepthValue;
    be<u32> mDepthBits;
    u8 mTail[0x2C];
};
} // namespace dWood
WWHD_SIZE(dWood::ShaderProgram, 0x80);
WWHD_SIZE(dWood::UniformDescriptor, 28);
WWHD_SIZE(dWood::UniformShape, 0x23C);
WWHD_OFFSET(dWood::UniformShape, mDescriptorIndex, 0x4C);
WWHD_SIZE(dWood::DrawInfo, 0x34);
WWHD_SIZE(dWood::GraphicsState, 0x11C);
WWHD_OFFSET(dWood::GraphicsState, mDepthBits, 0xEC);

namespace dWood {
struct HeapVtable {
    u8 mPrefix[0x34];
    gfn mAllocate;
    u8 mPadding[4];
    gfn mFree;
};
struct Heap {
    u8 mPrefix[12];
    gptr<HeapVtable> mpVtable;
};
} // namespace dWood
WWHD_OFFSET(dWood::HeapVtable, mFree, 0x3C);
WWHD_SIZE(dWood::Heap, 16);

struct WoodDrawListsView {
    u8 mPrefix[0x5D74];
    be<u32> mRoomList;
    u8 mPadding[4];
    be<u32> mAfterDrawList;
};
WWHD_OFFSET(WoodDrawListsView, mAfterDrawList, 0x5D7C);
WWHD_SIZE(WoodDrawListsView, 0x5D80);
struct WoodColorF {
    be<f32> mComponents[4];
};
WWHD_SIZE(WoodColorF, 16);

WWHD_SIZE(dWood::TextureUniformData, 0x2F0);
WWHD_OFFSET(dWood::TextureUniformData, mVectors, 0xB0);
WWHD_OFFSET(dWood::TextureUniformData, mMatrices, 0x110);
WWHD_OFFSET(dWood::TextureUniformData, mTailVectors, 0x290);
namespace dWood {
struct InitState {
    be<f32> mFirstValue, mSecondValue;
    be<u8> mFirstFlag, mSecondFlag;
    u8 mPadding[2];
    be<u32> mZero[4];
};
struct DestructorRecords {
    u8 mRecord[3][12];
};
} // namespace dWood
WWHD_SIZE(dWood::InitState, 0x1C);
WWHD_SIZE(dWood::DestructorRecords, 0x24);

WWHD_SIZE(dWood::SamplerRecord, 0x198);
WWHD_SIZE(dWood::SamplerPair, 0x228);
WWHD_SIZE(dWood::PacketBase, 0x98);

namespace dWood {
struct FogDescriptor {
    u8 mPrefix[0x1C4];
    be<u32> mColors[2][4];
};
struct DrawFrame {
    u8 mPrefix[0x20];
    Mtx34 mTrunkMatrix;
    Mtx34 mNormalMatrix;
    Mtx34 mFadeMatrix;
    TevStr mLight;
    WoodColorF mConverted;
    WoodColorF mFog[4];
    Mtx34 mView;
    u8 mTail[0x68];
};
} // namespace dWood
WWHD_SIZE(dWood::DrawFrame, 0x360);
WWHD_OFFSET(dWood::DrawFrame, mLight, 0xB0);
WWHD_OFFSET(dWood::DrawFrame, mConverted, 0x278);
WWHD_OFFSET(dWood::DrawFrame, mView, 0x2C8);

namespace dWood {
struct MaterialInstance {
    u8 mPrefix[0x20];
    be<u8> mReady;
    u8 mTail[3];
};
struct MaterialBinding {
    u8 mResource[132];
};
struct MaterialResource {
    u8 mPrefix[28];
    be<u32> mCount;
    gptr<MaterialBinding> mpBindings;
};
struct MaterialManager {
    u8 mPrefix[4];
    gptr<MaterialResource> mpResource;
    be<u32> mCount;
    gptr<MaterialInstance> mpInstances;
};
struct StringView {
    be<u32> mString;
    be<u32> mVtable;
};
struct Vector2 {
    be<f32> mValues[2];
};
struct Vector3 {
    be<f32> mValues[3];
};
struct CacheLine {
    be<u32> mWords[8];
};
} // namespace dWood
WWHD_SIZE(dWood::MaterialInstance, 36);
WWHD_SIZE(dWood::MaterialBinding, 132);
WWHD_SIZE(dWood::MaterialManager, 16);
WWHD_SIZE(dWood::StringView, 8);
WWHD_SIZE(dWood::Vector2, 8);
WWHD_SIZE(dWood::Vector3, 12);
WWHD_SIZE(dWood::CacheLine, 32);

namespace dWood {
struct GeometrySource {
    be<u32> mLeafVertexCount, mLeafIndexCount, mTrunkVertexCount, mTrunkIndexCount;
    Vector3 mLeafPosition[98], mLeafNormal[98];
    Vector4 mLeafColor[98];
    Vector2 mLeafUV[98];
    be<u16> mLeafIndices[252];
    Vector3 mTrunkPosition[35], mTrunkNormal[35];
    Vector4 mTrunkColor[35];
    Vector2 mTrunkUV[35];
    be<u16> mTrunkIndices[64];
};
} // namespace dWood
WWHD_SIZE(dWood::GeometrySource, 0x1B78);
WWHD_OFFSET(dWood::GeometrySource, mLeafPosition, 0x10);
WWHD_OFFSET(dWood::GeometrySource, mTrunkPosition, 0x1468);

WWHD_SIZE(dWood::VertexBuffer, 0x154);
struct WoodWordPair {
    be<u32> mFirst, mSecond;
};
WWHD_SIZE(WoodWordPair, 8);
