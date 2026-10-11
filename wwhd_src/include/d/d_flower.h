// Flower data and animation retain the GameCube sizes (0x44 and 0x38).
// HD: J3DPacket grows from 0x10 to 0x98; packet fields shift by 0x88 and GX2 geometry
// replaces display lists. Type/item/angle arguments retain the verified full registers.
#pragma once
#include "bindings.h"
#include "d/d_cc_d.h"
#include "gc_api/f_op/f_op_actor_mng.h"
namespace flower {
struct HitInf;
}
struct dFlower_packet_c;
struct dFlower_data_c {
    enum Flags_e : u32 {
        FLAG_ACTIVE = 2,
        FLAG_CULLED = 4,
        FLAG_CUT = 8,
        FLAG_GROUND_PENDING = 0x10,
        FLAG_PINK = 0x20,
        FLAG_OASIS = 0x40
    };

    /* 0x00 */ be<u8> mFlags;
    /* 0x01 */ be<s8> mAnmIdx;
    /* 0x02 */ be<s8> mItemTableIdx;
    /* 0x03 */ be<u8> mParticleTimer;
    /* 0x04 */ cXyz mPosition;
    /* 0x10 */ Mtx34 mMtx;
    /* 0x40 */ gptr<dFlower_data_c> mpNext;
    void WorkCo(fopAc_ac_c* actor, u32 mask, s32 room);
    void WorkAt_NoCutAnim(fopAc_ac_c* actor, u32 mask, s32 room, flower::HitInf* hit,
                          void* collision);
    void WorkAt(fopAc_ac_c* actor, u32 mask, s32 room, flower::HitInf* hit);
    void hitCheck(fopAc_ac_c* player, s32 room);
};
struct dFlower_anm_c {
    be<u8> mActive;
    u8 mPadding;
    be<s16> mAngleY, mBend;
    u8 mPadding6[2];
    Mtx34 mMtx;
};
struct dFlower_room_c {
    gptr<dFlower_data_c> mpHead;
    void newData(dFlower_data_c* data);
    void deleteData();
};
namespace flower {
enum Particle_e : u32 {
    PARTICLE_PINK = 0x3DD,
    PARTICLE_REGULAR = 0x3DE,
    PARTICLE_OASIS = 0x82C6,
    PARTICLE_OASIS_EXTRA = 0x82C7
};
struct RoomColor {
    u8 mPrefix[0xEC];
    be<u32> mColor[4];
    u8 mTail[0x130];
};
struct RoomColors {
    RoomColor mRoom[64];
};
struct DestructorRecord {
    u8 mRecord[12];
};
struct DestructorRecords {
    DestructorRecord mRecords[3];
};

struct SurfaceTransfer {
    u8 mPrefix[4];
    be<u32> mDimension, mWidth, mHeight, mDepth, mMipLevels, mFormat, mAntialias;
    u8 mAllocationSizes[8];
    be<u32> mImage, mMipmapSize, mMipmap, mTileMode, mSwizzle;
    u8 mTextureState[0x98];
    be<u32> mCachedImage;
    u8 mPadding[4];
    be<u32> mCachedMipmap;
};

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

enum Process_e { fpcNm_STONE_e = 454, fpcNm_PLAYER_e = 168 };
enum AttackType_e : u32 {
    AT_TYPE_NORMAL_ARROW = 0x4000,
    AT_TYPE_HOOKSHOT = 0x8000,
    AT_TYPE_FIRE_ARROW = 0x40000,
    AT_TYPE_ICE_ARROW = 0x80000,
    AT_TYPE_LIGHT_ARROW = 0x100000,
    AT_TYPE_WIND = 0x200000
};
struct HitInf {
    gptr<dCcD_GObjInf> mpArea, mpAtObject, mpCoObject;
    be<f32> mCoHitLength;
    be<u32> mVtable;
};

struct CosineTable {
    be<f32> mValues[8192][2];
};

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

struct StringVtable {
    u8 mPrefix[0x14];
    gfn mEnsureText;
};
struct String {
    be<u32> mText;
    gptr<StringVtable> mpVtable;
};

struct Vector2 {
    be<f32> mValues[2];
};
struct MeshCount {
    be<u32> mVertices, mIndices;
};
struct MeshCounts {
    MeshCount mMeshes[6];
};
template <class T, u32 N> struct Table {
    T mValues[N];
};

struct GroundCheck {
    be<u32> mBackgroundInterface, mWallInterface;
    u8 mPrefix[8];
    be<u32> mCheckVtable;
    u8 mPolygonState[12];
    be<u32> mPolygonVtable;
    cXyz mPosition;
    u8 mGroundState[16];
    be<u32> mBaseVtable;
    be<u8> mFlags[7];
    u8 mPadding;
    be<u32> mWallVtable, mWallEnabled;
};

struct EmitterColor {
    gptr<char> mpName;
    be<u32> mVtable;
    union {
        be<u32> mWords[4];
        be<f32> mValues[4];
    };
    be<u8> mEnabled;
    be<u8> mPadding[3];
};

struct ParticleControl {
    u8 mPrefix[0x50];
    be<u32> mColor[4];
};
struct Emitter {
    u8 mPrefix[0x34];
    be<f32> mScaleX;
};
struct MassManager {
    u8 mCollisionState[0x110];
    u8 mShape[0x18];
    be<u8> mAttribute, mEnabled;
};
// HD: the shared play layout leaves the collision manager opaque.
struct MassGameView {
    u8 mPrefix[0x4EF8];
    MassManager mMass;
};
struct SwordView {
    u8 mPrefix[0x3B8];
    be<u32> mCutFlags;
    u8 mPadding[0x28];
    cXyz mSwordTop;
};

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
struct UniformRecord {
    u8 mPrefix[4];
    be<u32> mSize;
    u8 mPadding[4];
    be<u32> mBuffer;
    u8 mTail[12];
};
struct UniformData {
    u8 mPrefix[16];
    UniformRecord mRecords[2];
    u8 mPadding[4];
    be<u32> mRecordIndex;
};
struct DrawObject {
    u8 mPrefix[4];
    gptr<UniformData> mpUniformData;
};
struct DrawInfo {
    u8 mPrefix[12];
    be<s32> mPass;
    u8 mPadding[4];
    gptr<DrawObject> mpDrawObject;
    u8 mTail[0x18];
    gptr<void> mpObject;
};
struct UniformVtable {
    u8 mPrefix[0x2C];
    gfn mApply;
};
struct UniformObject {
    u8 mPrefix[12];
    gptr<UniformVtable> mpVtable;
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
struct LightDefaults {
    union {
        be<f32> mFloat[17];
        be<u32> mWords[17];
    };
};
struct Vector4 {
    be<f32> mValues[4];
};
struct FogDescriptor {
    u8 mPrefix[0x1C4];
    be<u32> mColors[2][4];
};

struct PacketBase {
    u8 mPrefix[12];
    be<u32> mVtable;
    u8 mPacketState[0x88];
};
struct Transform {
    u8 mPrefix[12];
    gptr<UniformVtable> mpVtable;
    u8 mUniformState[0x64];
    Mtx34 mMatrix;
    u8 mTail[4];
};
struct TextureTransform {
    u8 mPrefix[12];
    gptr<UniformVtable> mpVtable;
    u8 mUniformState[0x64];
    Vector4 mColors[11];
    Vector4 mVectors[3][2];
    Mtx34 mMatrices[8];
    Vector4 mTailVectors[6];
};
struct TransformSet {
    be<u32> mState;
    gptr<void> mpDescriptor, mpUniform;
};
struct IndexBuffer {
    be<u32> mPrimitive;
    be<u32> mIndexFormat;
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
    gptr<void> mpMaterial;
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
struct SamplerRecord {
    be<u32> mWords[0x190 / 4];
    be<u8> mFlags;
    u8 mTail[7];
};
struct Geometry {
    ShapeGroups mShapes;
    MaterialBuffers mMaterials;
    IndexBuffer mUncut, mCut;
    SamplerRecord mSampler;
    u8 mSamplerResource[0x90];
};

} // namespace flower
struct dFlower_packet_c {
    /* 0x00000 */ flower::PacketBase mPacketBase;
    /* 0x00098 */ be<u16> mNextData;
    u8 mAlignment[2];
    /* 0x0009C */ dFlower_data_c mData[200];
    /* 0x035BC */ dFlower_anm_c mAnm[72];
    /* 0x0457C */ dFlower_room_c mRoom[64];
    be<u8> mPlayerCutFlag;
    u8 mPadding;
    be<s16> mPlayerSwordAngle, mPlayerSwordMoveAngle;
    u8 mPadding2[2];
    cXyz mPlayerSwordTop;
    /* 0x04690 */ flower::TransformSet mTransformSet;
    /* 0x0469C */ flower::Transform mTransforms[200];
    /* 0x0C9DC */ flower::TextureTransform mTextures[64];
    /* 0x1A2DC */ flower::Geometry mGeometry[3];
    gptr<void> mpShapeGroups, mpMaterialBuffers, mpUncutIndices, mpCutIndices, mpSampler;
    void draw();
    void calc();
    void update();
    void prepareGraphics();
    void drawGraphics(flower::DrawInfo* context);
    s32 newAnm();
    void setAnm(s32 index, s32 angle);
    void* newData(s32 type, cXyz* position, s32 room, s32 variant);
    void setData(dFlower_data_c* data, s32 index, s32 type, cXyz* position, s32 room, s32 item);
};
WWHD_OFFSET(dFlower_data_c, mPosition, 4);
WWHD_OFFSET(dFlower_data_c, mMtx, 0x10);
WWHD_OFFSET(dFlower_data_c, mpNext, 0x40);
WWHD_SIZE(dFlower_data_c, 0x44);
WWHD_OFFSET(dFlower_anm_c, mMtx, 8);
WWHD_SIZE(dFlower_anm_c, 0x38);
WWHD_SIZE(dFlower_room_c, 4);
WWHD_OFFSET(dFlower_packet_c, mData, 0x9C);
WWHD_OFFSET(dFlower_packet_c, mAnm, 0x35BC);
WWHD_OFFSET(dFlower_packet_c, mRoom, 0x457C);
WWHD_OFFSET(dFlower_packet_c, mPlayerCutFlag, 0x467C);
WWHD_OFFSET(dFlower_packet_c, mPlayerSwordTop, 0x4684);
WWHD_OFFSET(dFlower_packet_c, mTransformSet, 0x4690);
WWHD_OFFSET(dFlower_packet_c, mTransforms, 0x469C);
WWHD_OFFSET(dFlower_packet_c, mTextures, 0xC9DC);
WWHD_OFFSET(dFlower_packet_c, mGeometry, 0x1A2DC);
WWHD_OFFSET(dFlower_packet_c, mpShapeGroups, 0x1C640);
WWHD_SIZE(dFlower_packet_c, 0x1C654);
WWHD_SIZE(flower::PacketBase, 0x98);
WWHD_SIZE(flower::Transform, 0xA8);
WWHD_SIZE(flower::TextureTransform, 0x364);
WWHD_SIZE(flower::TransformSet, 12);
WWHD_SIZE(flower::Geometry, 0xBCC);

void dFlower_data_c__dt(void* self, u32 flags);
f32 checkGroundY(void* position);
void* dFlower_data_c__ct(void* self);
void flowerCopyMatrix(void* destination, void* source);
void flowerNormalizedColor(void* destination, void* source);
void flowerStaticInit();
void* dFlower_anm_c__ct(void* self);
void* dFlower_room_c__ct(void* self);
void* flowerStringConstruct(void* self, void* text);
void* flowerDrawNodeConstruct(void* self);
void* flowerMatrixConstruct(void* self);
void* flowerBuffer1Construct(void* self);
void* flowerBuffer2Construct(void* self);
void flowerAnimationDestroy(void* self, u32 flags);
void flowerSetVec3(void* self, f32 x, f32 y, f32 z);
void flowerSetVec4(void* self, f32 x, f32 y, f32 z, f32 w);
void flowerSetVec2(void* self, f32 x, f32 y);
void* flowerBufferSelect(void* self, u32 slot, u32 clear);
void flowerBufferRotate(void* self, u32 unused, u32 count);
void flowerSurfaceAssign(void* self, void* source);
void flowerFlagsSet(void* self, u32 mask);
void flowerBuffer1Destroy(void* self, u32 flags);
void flowerBuffer2Destroy(void* self, u32 flags);
void flowerDrawNodeDestroy(void* self, u32 flags);
void flowerRoomNodeDestroy(void* self, u32 flags);
void flowerEmptyResource1(void* self);
void flowerEmptyResource2(void* self);
u32 flowerResourceEnabled(void* self);
void* flowerRoomNodeConstruct(void* self);
void dFlower_packet_c__dt(void* self, u32 flags);
void* dFlower_packet_c__ct(void* self);
void* flowerGetHeap(void* a0);
void* flowerGetMaterialManager();
f32 flowerVectorSquareMagnitude(void* a0);
void flowerVertexBufferFlush(void* a0, u32 a1, u32 a2);
void flowerFirstStaticFlagConstruct(u32 a0);
void flowerFirstStaticFlagConstruct(void* a0);
void* flowerDCcD_GetGObjInf(void* a0);
f32 flowerCBgS_GroundCross(void* a0, void* a1);
void flowerScaleColor(void* a0, f32 a1);
void flowerTransformSetConfigure(void* a0, u32 a1, u32 a2);
void flowerMassSetHeight(void* a0, f32 a1);
void flowerBindSamplers(void* a0, void* a1, u32 a2, u32 a3);
s32 flowerFindMaterialIndex(void* a0, void* a1);
void flowerCMtx_YrotS(void* a0, s16 a1);
void flowerCMtx_YrotS(void* a0, s32 a1);
void flowerGX2BindProgram(u32 a0, u32 a1);
void flowerTransformSetConstruct(void* a0);
void flowerHitConstruct(void* a0);
void flowerPacketBaseDestroy(void* a0, u32 a1);
void flowerShapeDestroy(void* a0, u32 a1);
void flowerShapeConstruct(void* a0);
void flowerPacketBaseConstruct(void* a0);
s32 flowerFopOvlpM_IsPeek();
void flowerEmitterSetColor(void* a0, void* a1);
u32 flowerFopAcM_createItemFromTable(void* a0, s32 a1, s32 a2, s32 a3, s32 a4, void* a5, s32 a6,
                                     void* a7);
void flowerFopAcM_createItemFromTable(void* a0, s32 a1, s32 a2, s32 a3, u32 a4, u32 a5, u32 a6,
                                      u32 a7);
void flowerConvertFogColor(void* a0, void* a1, f32 a2);
f32 flowerCM_rndF(f32 a0);
void flowerCreateMaterialInstance(void* a0, void* a1, u32 a2);
void flowerIndexBufferConstruct(void* a0);
void flowerUniformDestroy(void* a0, u32 a1);
s32 flowerFrustumCheck(void* a0, void* a1, void* a2, f32 a3);
void flowerArrayConstruct(void* a0, u32 a1, u32 a2, u32 a3);
void flowerArrayConstruct(void* a0, u32 a1, u32 a2, void* a3);
void flowerCMtx_YrotM(void* a0, s16 a1);
void flowerCMtx_YrotM(void* a0, s32 a1);
void flowerMassSetRadius(void* a0, f32 a1);
void flowerGX2GeometryUniform(s32 a0, u32 a1, u32 a2);
void flowerRegisterStatic(u32 a0);
void flowerRegisterStatic(void* a0);
void flowerCsXyz_csXyz(void* a0, u32 a1, u32 a2, u32 a3);
void* flowerCsXyz_csXyz(void* a0, s16 a1, s16 a2, s16 a3);
void flowerGX2DrawIndexed(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5);
void flowerBindMaterialGroups(void* a0, void* a1, u32 a2);
s32 flowerDComIfGp_getReverb(s32 a0);
void flowerCLib_calcTimer(void* a0);
void flowerStartSound(u32 a0, void* a1, u32 a2, s32 a3);
void flowerJAIZelBasic_seDeleteObject(void* a0);
u32 flowerCheckMass(void* a0, void* a1, void* a2, void* a3);
void flowerCMtx_XrotM(void* a0, s16 a1);
void flowerCMtx_XrotM(void* a0, s32 a1);
void flowerCopySamplerState(void* a0, void* a1);
void flowerGraphicsStateApply(void* a0);
void flowerLockGraphics(void* a0);
void flowerIndexBufferDestroy(void* a0, u32 a1);
void* flowerGetShaderCache(void* a0);
void flowerCXyzSubtract(void* a0, void* a1, void* a2);
void flowerSecondStaticFlagConstruct(u32 a0);
void flowerSecondStaticFlagConstruct(void* a0);
void flowerVertexBufferConstruct(void* a0);
void flowerTransformSetDestroy(void* a0, u32 a1);
void flowerApplyDrawObject(void* a0, void* a1);
f32 flowerSquareRoot(f32 a0);
f64 flowerSquareRoot(f64 a0);
void flowerBindShaderProgram(void* a0);
void flowerSamplerConstruct(void* a0);
void flowerTransformSetView(void* a0, u32 a1, void* a2, void* a3, void* a4);
s32 flowerCLib_addCalcAngleS(void* a0, s32 a1, s32 a2, s32 a3, s32 a4);
short flowerCLib_addCalcAngleS(void* a0, s16 a1, s16 a2, s16 a3, s16 a4);
void flowerArrayDestroy(void* a0, u32 a1, u32 a2, void* a3, u32 a4, u32 a5);
void flowerUniformConstruct(void* a0);
void flowerUpdateShaderProgram(void* a0, u32 a1);
void flowerUniformSetState(void* a0, u32 a1);
void flowerLoadSamplerResource(void* a0, void* a1, void* a2);
void flowerSetupEnvironment(void* a0, s32 a1, u32 a2, void* a3);
void flowerSetupEnvironment(void* a0, s32 a1, void* a2, void* a3);
void flowerMatrixCopy(void* a0, void* a1);
void flowerSamplerDestroy(void* a0, u32 a1);
void flowerVertexBufferDestroy(void* a0, u32 a1);
void flowerUniformUpdate(void* a0);
s32 flowerCLib_chaseAngleS(void* a0, s16 a1, s16 a2);
s32 flowerCLib_chaseAngleS(void* a0, s32 a1, s32 a2);
void flowerGraphicsStateConstruct(void* a0);
void flowerApplyDrawMode(u32 a0, void* a1);
void flowerConfigureShape(void* a0, void* a1, void* a2, void* a3, u32 a4);
void flowerShapeBind(void* a0);
void* flowerDPa_control_c_set(void* a0, u32 a1, u32 a2, void* a3, u32 a4, u32 a5, u32 a6, u32 a7,
                              s32 a8, void* a9, void* a10, u32 a11);
void* flowerDPa_control_c_set(void* a0, u32 a1, u32 a2, void* a3, void* a4, u32 a5, u32 a6, u32 a7,
                              s32 a8, void* a9, void* a10, u32 a11);
void* flowerDPa_control_c_set(void* a0, u8 a1, u16 a2, void* a3, void* a4, void* a5, u8 a6,
                              void* a7, s8 a8, void* a9, void* a10, void* a11);
void flowerGX2PixelUniform(s32 a0, u32 a1, u32 a2);
void flowerGroundConstruct(void* a0);
void flowerApplyLightColor(void* a0);
void* flowerGetResource(void* a0, void* a1, void* a2, u32 a3);
s32 flowerAtan2(f32 a0, f32 a1);
void flowerUnlockGraphics(void* a0);
void* flowerGetEnvironment();
s32 flowerDrawListEntry(void* a0, void* a1, u16 a2);
void flowerDrawListEntry(void* a0, void* a1, u32 a2);
void flowerShapeClear(void* a0);
void* flowerFindHeap(void* a0, void* a1);
void flowerBindShaderState(void* a0);
void flowerClearUniformData(void* a0, u32 a1);
void flowerIndexBufferSet(void* a0, void* a1, u32 a2, u32 a3);
void flowerGroundDestruct(void* a0, u32 a1);
void flowerFree(void* a0);
void flowerGX2VertexUniform(s32 a0, u32 a1, u32 a2);
void flowerConfigureVertexBuffer(void* a0, void* a1, u32 a2, void* a3);
void* flowerAllocate(u32 a0);
void flowerTransformSetUpdate(void* a0);
void flowerInitLight(void* a0, s32 a1, u32 a2);
void flowerSamplerResourceConstruct(void* a0);
void flower_assert(void* a0, s32 a1, void* a2);
void flower_assert(void* a0, u32 a1, void* a2);

WWHD_SIZE(flower::IndexBuffer, 0x18);
WWHD_SIZE(flower::ShapeVtable, 0x10);
WWHD_SIZE(flower::Shape, 0xF4);
WWHD_SIZE(flower::ShapeArray, 8);
WWHD_SIZE(flower::ShapeGroup, 0x14);
WWHD_SIZE(flower::ShapeGroups, 12);
WWHD_SIZE(flower::Vertex, 48);
WWHD_SIZE(flower::VertexBuffer, 0x154);
WWHD_SIZE(flower::MaterialPart, 0x254);
WWHD_OFFSET(flower::MaterialPart, mShape, 0x158);
WWHD_OFFSET(flower::MaterialPart, mVertexCapacity, 0x24C);
WWHD_SIZE(flower::MaterialBuffers, 0x968);
WWHD_OFFSET(flower::MaterialBuffers, mFrameIndex, 0x950);
WWHD_SIZE(flower::SamplerRecord, 0x198);
WWHD_OFFSET(flower::Geometry, mMaterials, 12);
WWHD_OFFSET(flower::Geometry, mUncut, 0x974);
WWHD_OFFSET(flower::Geometry, mCut, 0x98C);
WWHD_OFFSET(flower::Geometry, mSampler, 0x9A4);
WWHD_OFFSET(flower::Geometry, mSamplerResource, 0xB3C);

WWHD_SIZE(flower::LightObj, 0x84);
WWHD_SIZE(flower::TevStr, 0x1C8);
WWHD_OFFSET(flower::TevStr, mColorC0, 0x90);
WWHD_OFFSET(flower::TevStr, mHdLightParam, 0xC0);
WWHD_SIZE(flower::LightDefaults, 0x44);
WWHD_SIZE(flower::Vector4, 16);
WWHD_SIZE(flower::FogDescriptor, 0x1E4);
WWHD_OFFSET(flower::FogDescriptor, mColors, 0x1C4);

WWHD_SIZE(flower::ShaderProgramState, 0x2C);
WWHD_SIZE(flower::ShaderProgram, 0x80);
WWHD_OFFSET(flower::ShaderProgram, mpState, 0x7C);
WWHD_SIZE(flower::UniformSlots, 0x12);
WWHD_SIZE(flower::ShaderMaterial, 0x1C);
WWHD_SIZE(flower::ShaderCache, 8);
WWHD_SIZE(flower::UniformRecord, 28);
WWHD_OFFSET(flower::UniformData, mRecordIndex, 0x4C);
WWHD_SIZE(flower::UniformData, 0x50);
WWHD_SIZE(flower::DrawObject, 8);
WWHD_SIZE(flower::DrawInfo, 0x34);
WWHD_SIZE(flower::UniformVtable, 0x30);
WWHD_SIZE(flower::UniformObject, 16);
WWHD_SIZE(flower::GraphicsState, 0x11C);
WWHD_OFFSET(flower::GraphicsState, mDepthBits, 0xEC);

WWHD_SIZE(flower::ParticleControl, 0x60);
WWHD_SIZE(flower::Emitter, 0x38);
WWHD_SIZE(flower::MassManager, 0x12A);
WWHD_OFFSET(flower::SwordView, mSwordTop, 0x3E4);
WWHD_SIZE(flower::SwordView, 0x3F0);

WWHD_SIZE(flower::EmitterColor, 28);
WWHD_OFFSET(flower::EmitterColor, mValues, 8);

WWHD_SIZE(flower::GroundCheck, 0x54);
WWHD_OFFSET(flower::GroundCheck, mPosition, 0x24);
WWHD_OFFSET(flower::GroundCheck, mFlags, 0x44);
WWHD_OFFSET(flower::GroundCheck, mWallEnabled, 0x50);

WWHD_SIZE(flower::Vector2, 8);
WWHD_SIZE(flower::MeshCount, 8);
WWHD_SIZE(flower::MeshCounts, 48);
using FlowerRegularPositions = flower::Table<cXyz, 90>;
WWHD_SIZE(FlowerRegularPositions, 1080);
using FlowerRegularNormals = flower::Table<cXyz, 90>;
WWHD_SIZE(FlowerRegularNormals, 1080);
using FlowerRegularTexCoords = flower::Table<flower::Vector2, 90>;
WWHD_SIZE(FlowerRegularTexCoords, 720);
using FlowerRegularColors = flower::Table<flower::Vector4, 90>;
WWHD_SIZE(FlowerRegularColors, 1440);
using FlowerRegularCutPositions = flower::Table<cXyz, 60>;
WWHD_SIZE(FlowerRegularCutPositions, 720);
using FlowerRegularCutNormals = flower::Table<cXyz, 60>;
WWHD_SIZE(FlowerRegularCutNormals, 720);
using FlowerRegularCutTexCoords = flower::Table<flower::Vector2, 60>;
WWHD_SIZE(FlowerRegularCutTexCoords, 480);
using FlowerRegularCutColors = flower::Table<flower::Vector4, 60>;
WWHD_SIZE(FlowerRegularCutColors, 960);
using FlowerPinkPositions = flower::Table<cXyz, 73>;
WWHD_SIZE(FlowerPinkPositions, 876);
using FlowerPinkNormals = flower::Table<cXyz, 73>;
WWHD_SIZE(FlowerPinkNormals, 876);
using FlowerPinkTexCoords = flower::Table<flower::Vector2, 73>;
WWHD_SIZE(FlowerPinkTexCoords, 584);
using FlowerPinkColors = flower::Table<flower::Vector4, 73>;
WWHD_SIZE(FlowerPinkColors, 1168);
using FlowerPinkCutPositions = flower::Table<cXyz, 31>;
WWHD_SIZE(FlowerPinkCutPositions, 372);
using FlowerPinkCutNormals = flower::Table<cXyz, 31>;
WWHD_SIZE(FlowerPinkCutNormals, 372);
using FlowerPinkCutTexCoords = flower::Table<flower::Vector2, 31>;
WWHD_SIZE(FlowerPinkCutTexCoords, 248);
using FlowerPinkCutColors = flower::Table<flower::Vector4, 31>;
WWHD_SIZE(FlowerPinkCutColors, 496);
using FlowerOasisPositions = flower::Table<cXyz, 605>;
WWHD_SIZE(FlowerOasisPositions, 7260);
using FlowerOasisNormals = flower::Table<cXyz, 605>;
WWHD_SIZE(FlowerOasisNormals, 7260);
using FlowerOasisTexCoords = flower::Table<flower::Vector2, 605>;
WWHD_SIZE(FlowerOasisTexCoords, 4840);
using FlowerOasisColors = flower::Table<flower::Vector4, 605>;
WWHD_SIZE(FlowerOasisColors, 9680);
using FlowerOasisCutPositions = flower::Table<cXyz, 42>;
WWHD_SIZE(FlowerOasisCutPositions, 504);
using FlowerOasisCutNormals = flower::Table<cXyz, 42>;
WWHD_SIZE(FlowerOasisCutNormals, 504);
using FlowerOasisCutTexCoords = flower::Table<flower::Vector2, 42>;
WWHD_SIZE(FlowerOasisCutTexCoords, 336);
using FlowerOasisCutColors = flower::Table<flower::Vector4, 42>;
WWHD_SIZE(FlowerOasisCutColors, 672);

WWHD_SIZE(flower::StringVtable, 0x18);
WWHD_SIZE(flower::String, 8);

WWHD_SIZE(flower::HeapVtable, 0x40);
WWHD_SIZE(flower::Heap, 16);
WWHD_OFFSET(flower::HeapVtable, mFree, 0x3C);

WWHD_SIZE(flower::CosineTable, 0x10000);

WWHD_SIZE(flower::HitInf, 20);
WWHD_OFFSET(flower::HitInf, mCoHitLength, 12);

WWHD_SIZE(flower::MaterialInstance, 36);
WWHD_SIZE(flower::MaterialBinding, 132);
WWHD_SIZE(flower::MaterialResource, 36);
WWHD_SIZE(flower::MaterialManager, 16);

WWHD_SIZE(flower::SurfaceTransfer, 0xE0);
WWHD_OFFSET(flower::SurfaceTransfer, mImage, 0x28);
WWHD_OFFSET(flower::SurfaceTransfer, mCachedImage, 0xD4);
WWHD_OFFSET(flower::TextureTransform, mVectors, 0x124);
WWHD_OFFSET(flower::TextureTransform, mMatrices, 0x184);
WWHD_OFFSET(flower::TextureTransform, mTailVectors, 0x304);
WWHD_OFFSET(flower::MassGameView, mMass, 0x4EF8);
WWHD_SIZE(flower::MassGameView, 0x5022);

WWHD_SIZE(flower::RoomColor, 0x22C);
WWHD_SIZE(flower::RoomColors, 0x8B00);
WWHD_SIZE(flower::DestructorRecord, 12);
WWHD_SIZE(flower::DestructorRecords, 36);
using FlowerRegularIndices = flower::Table<be<u8>, 288>;
WWHD_SIZE(FlowerRegularIndices, 288);
using FlowerRegularCutIndices = flower::Table<be<u8>, 144>;
WWHD_SIZE(FlowerRegularCutIndices, 144);
using FlowerPinkIndices = flower::Table<be<u8>, 312>;
WWHD_SIZE(FlowerPinkIndices, 312);
using FlowerPinkCutIndices = flower::Table<be<u8>, 144>;
WWHD_SIZE(FlowerPinkCutIndices, 144);
using FlowerOasisIndices = flower::Table<be<u8>, 2844>;
WWHD_SIZE(FlowerOasisIndices, 2844);
using FlowerOasisCutIndices = flower::Table<be<u8>, 216>;
WWHD_SIZE(FlowerOasisCutIndices, 216);
