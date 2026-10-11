/* Grass packets: GameCube data/animation names with WWHD layouts.
 * HD retains the 0x44-byte grass data and 0x38-byte animation, shifts packet pools by 0x88,
 * and adds material collections, double-buffered vertices, shaders and textures.
 * Exact caller workspaces preserve constructor/store ordering and recorded outputs.
 * Unit-local verified views and SDK bindings. */
#pragma once
#include "bindings.h"
namespace GrassHD {
using gabi::at;

struct Vector {
    /* 0x0 */ be<f32> x;
    /* 0x4 */ be<f32> y;
    /* 0x8 */ be<f32> z;
};
WWHD_SIZE(Vector, 12);
WWHD_OFFSET(Vector, x, 0x0);
WWHD_OFFSET(Vector, y, 0x4);
WWHD_OFFSET(Vector, z, 0x8);
struct Matrix {
    /* 0x0 */ be<f32> m[12];
};
WWHD_SIZE(Matrix, 48);
WWHD_OFFSET(Matrix, m, 0x0);
struct ColorF {
    /* 0x0 */ be<f32> mChannel[4];
};
WWHD_SIZE(ColorF, 16);
WWHD_OFFSET(ColorF, mChannel, 0x0);
struct ColorS {
    /* 0x0 */ be<s16> mChannel[4];
};
WWHD_SIZE(ColorS, 8);
WWHD_OFFSET(ColorS, mChannel, 0x0);
struct String {
    /* 0x0 */ be<u32> mpString;
    /* 0x4 */ be<u32> mVtable;
};
WWHD_SIZE(String, 8);
WWHD_OFFSET(String, mpString, 0x0);
WWHD_OFFSET(String, mVtable, 0x4);
struct dGrass_data_c {
    void WorkCo(u32 actor, u32 mode, s32 room);
    void WorkAt_NoCutAnim(u32 actor, u32 mode, s32 room, u32 hit, u32 object);
    void WorkAt(u32 actor, u32 mode, s32 room, u32 hit);
    void hitCheck(s32 room);
    /* 0x0 */ be<u8> mState;
    /* 0x1 */ be<u8> mInitFlags;
    /* 0x2 */ be<s8> mAnimIdx;
    /* 0x3 */ be<s8> mItemIdx;
    /* 0x4 */ union {
        Vector mPos;
        be<u32> mPosWords[3];
    };
    /* 0x10 */ Matrix mModelMtx;
    /* 0x40 */ be<u32> mpNextData;
};
WWHD_SIZE(dGrass_data_c, 68);
WWHD_OFFSET(dGrass_data_c, mState, 0x0);
WWHD_OFFSET(dGrass_data_c, mInitFlags, 0x1);
WWHD_OFFSET(dGrass_data_c, mAnimIdx, 0x2);
WWHD_OFFSET(dGrass_data_c, mItemIdx, 0x3);
WWHD_OFFSET(dGrass_data_c, mPos, 0x4);
WWHD_OFFSET(dGrass_data_c, mModelMtx, 0x10);
WWHD_OFFSET(dGrass_data_c, mpNextData, 0x40);
struct dGrass_anm_c {
    /* 0x0 */ be<u8> mState;
    u8 padding1[1];
    /* 0x2 */ be<s16> mRotY;
    /* 0x4 */ be<s16> mRotX;
    u8 padding6[2];
    /* 0x8 */ Matrix mAnimMtx;
};
WWHD_SIZE(dGrass_anm_c, 56);
WWHD_OFFSET(dGrass_anm_c, mState, 0x0);
WWHD_OFFSET(dGrass_anm_c, mRotY, 0x2);
WWHD_OFFSET(dGrass_anm_c, mRotX, 0x4);
WWHD_OFFSET(dGrass_anm_c, mAnimMtx, 0x8);
struct dGrass_room_c {
    void newData(u32 data);
    void deleteData();
    /* 0x0 */ be<u32> mpData;
};
WWHD_SIZE(dGrass_room_c, 4);
WWHD_OFFSET(dGrass_room_c, mpData, 0x0);
struct ParticleParams {
    /* 0x0 */ be<u32> mpEmptyString;
    /* 0x4 */ be<u32> mVtable;
    /* 0x8 */ be<u32> mColorWords[4];
    /* 0x18 */ be<u8> mEnable;
    /* 0x19 */ be<u8> mFlags[3];
};
WWHD_SIZE(ParticleParams, 28);
WWHD_OFFSET(ParticleParams, mpEmptyString, 0x0);
WWHD_OFFSET(ParticleParams, mVtable, 0x4);
WWHD_OFFSET(ParticleParams, mColorWords, 0x8);
WWHD_OFFSET(ParticleParams, mEnable, 0x18);
WWHD_OFFSET(ParticleParams, mFlags, 0x19);
struct ParticleControl {
    u8 padding0[8];
    /* 0x8 */ be<u32> mColorWords[4];
};
WWHD_SIZE(ParticleControl, 24);
WWHD_OFFSET(ParticleControl, mColorWords, 0x8);
struct HitInfo {
    u8 padding0[4];
    /* 0x4 */ be<u32> mpAtHitObj;
    u8 padding8[12];
};
WWHD_SIZE(HitInfo, 20);
WWHD_OFFSET(HitInfo, mpAtHitObj, 0x4);
struct Actor {
    u8 padding0[8];
    /* 0x8 */ be<s16> mProcName;
    u8 paddingA[778];
    /* 0x314 */ Vector mCurrentPos;
    u8 padding320[80];
    /* 0x370 */ be<f32> mSpeedF;
};
WWHD_SIZE(Actor, 884);
WWHD_OFFSET(Actor, mProcName, 0x8);
WWHD_OFFSET(Actor, mCurrentPos, 0x314);
WWHD_OFFSET(Actor, mSpeedF, 0x370);
struct CollisionObject {
    u8 padding0[16];
    /* 0x10 */ be<u32> mAtType;
    u8 padding14[40];
    /* 0x3C */ be<u32> mVtable;
};
WWHD_SIZE(CollisionObject, 64);
WWHD_OFFSET(CollisionObject, mAtType, 0x10);
WWHD_OFFSET(CollisionObject, mVtable, 0x3C);
struct HitObjectInfo {
    u8 padding0[124];
    /* 0x7C */ Vector mAtVec;
};
WWHD_SIZE(HitObjectInfo, 136);
WWHD_OFFSET(HitObjectInfo, mAtVec, 0x7C);
struct CollisionShape {
    u8 padding0[28];
    /* 0x1C */ be<u32> mVtable;
};
WWHD_SIZE(CollisionShape, 32);
WWHD_OFFSET(CollisionShape, mVtable, 0x1C);
struct Emitter {
    u8 padding0[4];
    /* 0x4 */ be<u32> mpEmitter;
};
WWHD_SIZE(Emitter, 8);
WWHD_OFFSET(Emitter, mpEmitter, 0x4);
struct Play {
    u8 padding0[21138];
    /* 0x5292 */ be<u8> mEventRunning;
    u8 padding5293[2077];
    /* 0x5AB0 */ be<u32> mpParticle;
    u8 padding5AB4[4];
    /* 0x5AB8 */ be<u32> mpGrassPacket;
};
WWHD_SIZE(Play, 23228);
WWHD_OFFSET(Play, mEventRunning, 0x5292);
WWHD_OFFSET(Play, mpParticle, 0x5AB0);
WWHD_OFFSET(Play, mpGrassPacket, 0x5AB8);
struct Background {
    u8 padding0[18448];
    /* 0x4810 */ be<u32> mpParticle;
};
WWHD_SIZE(Background, 18452);
WWHD_OFFSET(Background, mpParticle, 0x4810);
struct Environment {
    u8 padding0[4259];
    /* 0x10A3 */ be<u8> mRoomNo;
};
WWHD_SIZE(Environment, 4260);
WWHD_OFFSET(Environment, mRoomNo, 0x10A3);
struct Heap {
    u8 padding0[12];
    /* 0xC */ be<u32> mVtable;
};
WWHD_SIZE(Heap, 16);
WWHD_OFFSET(Heap, mVtable, 0xC);
struct Shape {
    u8 padding0[240];
    /* 0xF0 */ be<u32> mVtable;
};
WWHD_SIZE(Shape, 244);
WWHD_OFFSET(Shape, mVtable, 0xF0);
struct ShapePair {
    /* 0x0 */ be<s32> mCount;
    /* 0x4 */ be<u32> mpShapes;
};
WWHD_SIZE(ShapePair, 8);
WWHD_OFFSET(ShapePair, mCount, 0x0);
WWHD_OFFSET(ShapePair, mpShapes, 0x4);
struct ShapeCollection {
    /* 0x0 */ be<u32> mpMaterial;
    /* 0x4 */ ShapePair mShape[2];
};
WWHD_SIZE(ShapeCollection, 20);
WWHD_OFFSET(ShapeCollection, mpMaterial, 0x0);
WWHD_OFFSET(ShapeCollection, mShape, 0x4);
struct MaterialCollection {
    /* 0x0 */ be<u32> mCount;
    /* 0x4 */ be<s32> mCapacity;
    /* 0x8 */ be<u32> mpItems;
};
WWHD_SIZE(MaterialCollection, 12);
WWHD_OFFSET(MaterialCollection, mCount, 0x0);
WWHD_OFFSET(MaterialCollection, mCapacity, 0x4);
WWHD_OFFSET(MaterialCollection, mpItems, 0x8);
struct VertexBuffer {
    u8 padding0[332];
    /* 0x14C */ be<u32> mBufferSize;
    u8 padding150[4];
};
WWHD_SIZE(VertexBuffer, 340);
WWHD_OFFSET(VertexBuffer, mBufferSize, 0x14C);
struct Resource {
    /* 0x0 */ be<u32> mpBuffer;
    u8 padding4[584];
    /* 0x24C */ be<u32> mAlignment;
    /* 0x250 */ be<u32> mpAllocation;
};
WWHD_SIZE(Resource, 596);
WWHD_OFFSET(Resource, mpBuffer, 0x0);
WWHD_OFFSET(Resource, mAlignment, 0x24C);
WWHD_OFFSET(Resource, mpAllocation, 0x250);
struct Resources {
    /* 0x0 */ Resource mBuffer[4];
    /* 0x950 */ be<u32> mSelected;
    /* 0x954 */ be<u32> mVertexFormat;
    /* 0x958 */ be<u32> mVertexStride;
    /* 0x95C */ be<u32> mpVertexLayout;
    /* 0x960 */ be<u32> mCount;
    /* 0x964 */ be<u8> mReady;
    u8 padding965[3];
};
WWHD_SIZE(Resources, 2408);
WWHD_OFFSET(Resources, mBuffer, 0x0);
WWHD_OFFSET(Resources, mSelected, 0x950);
WWHD_OFFSET(Resources, mVertexFormat, 0x954);
WWHD_OFFSET(Resources, mVertexStride, 0x958);
WWHD_OFFSET(Resources, mpVertexLayout, 0x95C);
WWHD_OFFSET(Resources, mCount, 0x960);
WWHD_OFFSET(Resources, mReady, 0x964);
struct ModelBinding {
    u8 padding0[4];
    /* 0x4 */ be<u32> mpRenderer;
    u8 padding8[4];
};
WWHD_SIZE(ModelBinding, 12);
WWHD_OFFSET(ModelBinding, mpRenderer, 0x4);
struct ModelPacket {
    u8 padding0[12];
    /* 0xC */ be<u32> mVtable;
    u8 padding10[100];
    /* 0x74 */ Matrix mModelMtx;
    u8 paddingA4[4];
};
WWHD_SIZE(ModelPacket, 168);
WWHD_OFFSET(ModelPacket, mVtable, 0xC);
WWHD_OFFSET(ModelPacket, mModelMtx, 0x74);
struct ShaderPacket {
    u8 padding0[12];
    /* 0xC */ be<u32> mVtable;
    u8 padding10[100];
    /* 0x74 */ be<f32> mInitialValues[44];
    /* 0x124 */ ColorF mColorPairs[6];
    u8 padding184[480];
};
WWHD_SIZE(ShaderPacket, 868);
WWHD_OFFSET(ShaderPacket, mVtable, 0xC);
WWHD_OFFSET(ShaderPacket, mInitialValues, 0x74);
WWHD_OFFSET(ShaderPacket, mColorPairs, 0x124);
struct IndexBuffer {
    /* 0x0 */ be<u32> mPrimitive;
    /* 0x4 */ be<u32> mpIndices;
    /* 0x8 */ be<u32> mIndexType;
    /* 0xC */ be<u32> mCount;
    u8 padding10[8];
};
WWHD_SIZE(IndexBuffer, 24);
WWHD_OFFSET(IndexBuffer, mPrimitive, 0x0);
WWHD_OFFSET(IndexBuffer, mpIndices, 0x4);
WWHD_OFFSET(IndexBuffer, mIndexType, 0x8);
WWHD_OFFSET(IndexBuffer, mCount, 0xC);
struct Texture {
    /* 0x0 */ be<u32> mWords[100];
    /* 0x190 */ be<u8> mFlags;
    u8 padding191[7];
};
WWHD_SIZE(Texture, 408);
WWHD_OFFSET(Texture, mWords, 0x0);
WWHD_OFFSET(Texture, mFlags, 0x190);
struct TextureInfo {
    /* 0x0 */ be<u32> mWords[36];
};
WWHD_SIZE(TextureInfo, 144);
WWHD_OFFSET(TextureInfo, mWords, 0x0);
struct LightState {
    /* 0x0 */ be<u32> mWords[114];
};
WWHD_SIZE(LightState, 456);
WWHD_OFFSET(LightState, mWords, 0x0);
struct RenderState {
    u8 padding0[12];
    /* 0xC */ be<u32> mBlendMode;
    u8 padding10[208];
    /* 0xE0 */ be<u8> mDepthEnable;
    u8 paddingE1[3];
    /* 0xE4 */ be<u32> mDepthFunc;
    /* 0xE8 */ be<f32> mDepthBias;
    /* 0xEC */ be<u32> mRasterMode;
    u8 paddingF0[44];
};
WWHD_SIZE(RenderState, 284);
WWHD_OFFSET(RenderState, mBlendMode, 0xC);
WWHD_OFFSET(RenderState, mDepthEnable, 0xE0);
WWHD_OFFSET(RenderState, mDepthFunc, 0xE4);
WWHD_OFFSET(RenderState, mDepthBias, 0xE8);
WWHD_OFFSET(RenderState, mRasterMode, 0xEC);
struct RenderContext {
    u8 padding0[12];
    /* 0xC */ be<u32> mPass;
    u8 padding10[4];
    /* 0x14 */ be<u32> mpModelBinding;
    u8 padding18[24];
    /* 0x30 */ be<u32> mpCallback;
};
WWHD_SIZE(RenderContext, 52);
WWHD_OFFSET(RenderContext, mPass, 0xC);
WWHD_OFFSET(RenderContext, mpModelBinding, 0x14);
WWHD_OFFSET(RenderContext, mpCallback, 0x30);
struct Material {
    /* 0x0 */ be<u32> mpModel;
    u8 padding4[8];
    /* 0xC */ be<u32> mShaderCount;
    /* 0x10 */ be<u32> mpShaderInfo;
    /* 0x14 */ be<u32> mAttributeCount;
    /* 0x18 */ be<u32> mpAttributeInfo;
};
WWHD_SIZE(Material, 28);
WWHD_OFFSET(Material, mpModel, 0x0);
WWHD_OFFSET(Material, mShaderCount, 0xC);
WWHD_OFFSET(Material, mpShaderInfo, 0x10);
WWHD_OFFSET(Material, mAttributeCount, 0x14);
WWHD_OFFSET(Material, mpAttributeInfo, 0x18);
struct Model {
    /* 0x0 */ be<u8> mFlags;
    u8 padding1[3];
    /* 0x4 */ be<u32> mpDisplayList;
    u8 padding8[4];
    /* 0xC */ be<u32> mDisplayListSize;
    u8 padding10[108];
    /* 0x7C */ be<u32> mpShaderResource;
};
WWHD_SIZE(Model, 128);
WWHD_OFFSET(Model, mFlags, 0x0);
WWHD_OFFSET(Model, mpDisplayList, 0x4);
WWHD_OFFSET(Model, mDisplayListSize, 0xC);
WWHD_OFFSET(Model, mpShaderResource, 0x7C);
struct ShaderResource {
    u8 padding0[40];
    /* 0x28 */ be<u32> mpShader;
};
WWHD_SIZE(ShaderResource, 44);
WWHD_OFFSET(ShaderResource, mpShader, 0x28);
struct ShaderCache {
    /* 0x0 */ be<u32> mpShader;
    /* 0x4 */ be<u32> mpModel;
};
WWHD_SIZE(ShaderCache, 8);
WWHD_OFFSET(ShaderCache, mpShader, 0x0);
WWHD_OFFSET(ShaderCache, mpModel, 0x4);
struct ShaderInfo {
    u8 padding0[12];
    /* 0xC */ be<s16> mVertexBlock;
    /* 0xE */ be<s16> mPixelBlock;
    /* 0x10 */ be<s16> mGeometryBlock;
};
WWHD_SIZE(ShaderInfo, 18);
WWHD_OFFSET(ShaderInfo, mVertexBlock, 0xC);
WWHD_OFFSET(ShaderInfo, mPixelBlock, 0xE);
WWHD_OFFSET(ShaderInfo, mGeometryBlock, 0x10);
struct UniformData {
    u8 padding0[76];
    /* 0x4C */ be<u32> mBlockIndex;
};
WWHD_SIZE(UniformData, 80);
WWHD_OFFSET(UniformData, mBlockIndex, 0x4C);
struct UniformBlock {
    u8 padding0[4];
    /* 0x4 */ be<u32> mSize;
    u8 padding8[4];
    /* 0xC */ be<u32> mpBuffer;
    u8 padding10[12];
};
WWHD_SIZE(UniformBlock, 28);
WWHD_OFFSET(UniformBlock, mSize, 0x4);
WWHD_OFFSET(UniformBlock, mpBuffer, 0xC);
struct Renderer {
    u8 padding0[452];
    /* 0x1C4 */ be<u32> mColor0[4];
    /* 0x1D4 */ be<u32> mColor1[4];
};
WWHD_SIZE(Renderer, 484);
WWHD_OFFSET(Renderer, mColor0, 0x1C4);
WWHD_OFFSET(Renderer, mColor1, 0x1D4);
struct Clipper {
    u8 padding0[84];
    /* 0x54 */ be<f32> mFar;
    u8 padding58[8];
};
WWHD_SIZE(Clipper, 96);
WWHD_OFFSET(Clipper, mFar, 0x54);
struct Archive {
    u8 padding0[4];
    /* 0x4 */ be<u32> mpDirectory;
    /* 0x8 */ be<u32> mCount;
    /* 0xC */ be<u32> mpEntries;
};
WWHD_SIZE(Archive, 16);
WWHD_OFFSET(Archive, mpDirectory, 0x4);
WWHD_OFFSET(Archive, mCount, 0x8);
WWHD_OFFSET(Archive, mpEntries, 0xC);
struct ArchiveEntry {
    u8 padding0[32];
    /* 0x20 */ be<u8> mLoaded;
    u8 padding21[3];
};
WWHD_SIZE(ArchiveEntry, 36);
WWHD_OFFSET(ArchiveEntry, mLoaded, 0x20);
struct Directory {
    u8 padding0[28];
    /* 0x1C */ be<u32> mCount;
    /* 0x20 */ be<u32> mpEntries;
};
WWHD_SIZE(Directory, 36);
WWHD_OFFSET(Directory, mCount, 0x1C);
WWHD_OFFSET(Directory, mpEntries, 0x20);
struct MassAttribute {
    u8 padding0[10620];
    /* 0x297C */ be<u8> mType;
    /* 0x297D */ be<u8> mRoom;
};
WWHD_SIZE(MassAttribute, 10622);
WWHD_OFFSET(MassAttribute, mType, 0x297C);
WWHD_OFFSET(MassAttribute, mRoom, 0x297D);
struct dGrass_packet_c {
    s32 newAnm();
    void setAnm(u32 index, s32 angle);
    void setData(u32 data, u32 index, u32 pos, u32 room, u32 item);
    u32 newData(u32 pos, u32 room, u32 item);
    void calc();
    void update();
    void draw(u32 state);
    void buildShaders();
    u8 padding0[12];
    /* 0xC */ be<u32> mVtable;
    u8 padding10[136];
    /* 0x98 */ be<u16> mNextIdx;
    u8 padding9A[2];
    /* 0x9C */ dGrass_data_c mGrassData[1500];
    /* 0x18F0C */ dGrass_anm_c mGrassAnm[104];
    /* 0x1A5CC */ dGrass_room_c mGrassRoom[64];
    /* 0x1A6CC */ be<u16> mCoParticle;
    /* 0x1A6CE */ be<u16> mAtParticle;
    /* 0x1A6D0 */ MaterialCollection mMaterials;
    /* 0x1A6DC */ Resources mResources;
    /* 0x1B044 */ ModelBinding mModelBinding;
    /* 0x1B050 */ ModelPacket mModelPackets[1500];
    /* 0x588B0 */ ShaderPacket mShaderPackets[64];
    /* 0x661B0 */ IndexBuffer mIndices[2];
    /* 0x661E0 */ Texture mTexture;
    /* 0x66378 */ TextureInfo mTextureInfo;
};
WWHD_SIZE(dGrass_packet_c, 418824);
WWHD_OFFSET(dGrass_packet_c, mVtable, 0xC);
WWHD_OFFSET(dGrass_packet_c, mNextIdx, 0x98);
WWHD_OFFSET(dGrass_packet_c, mGrassData, 0x9C);
WWHD_OFFSET(dGrass_packet_c, mGrassAnm, 0x18F0C);
WWHD_OFFSET(dGrass_packet_c, mGrassRoom, 0x1A5CC);
WWHD_OFFSET(dGrass_packet_c, mCoParticle, 0x1A6CC);
WWHD_OFFSET(dGrass_packet_c, mAtParticle, 0x1A6CE);
WWHD_OFFSET(dGrass_packet_c, mMaterials, 0x1A6D0);
WWHD_OFFSET(dGrass_packet_c, mResources, 0x1A6DC);
WWHD_OFFSET(dGrass_packet_c, mModelBinding, 0x1B044);
WWHD_OFFSET(dGrass_packet_c, mModelPackets, 0x1B050);
WWHD_OFFSET(dGrass_packet_c, mShaderPackets, 0x588B0);
WWHD_OFFSET(dGrass_packet_c, mIndices, 0x661B0);
WWHD_OFFSET(dGrass_packet_c, mTexture, 0x661E0);
WWHD_OFFSET(dGrass_packet_c, mTextureInfo, 0x66378);
struct ShaderFrame {
    u8 conversion[8];
    Matrix matrix;
    LightState tev;
    ColorF color, scaled0, scaled1;
    Matrix view;
    u8 savedRegisters[64];
};
WWHD_SIZE(ShaderFrame, 0x2A0);
WWHD_OFFSET(ShaderFrame, tev, 0x38);
WWHD_OFFSET(ShaderFrame, view, 0x230);
struct Ground {
    be<u32> mVtable, mpInterface;
    u8 padding08[8];
    be<u32> mPolyVtable;
    u8 padding14[12];
    be<u32> mCheckVtable;
    Vector mPos;
    u8 padding30[16];
    be<u32> mObjectVtable;
    be<u8> mFlags[7];
    u8 padding4B;
    be<u32> mInterfaceVtable, mMask;
};
WWHD_SIZE(Ground, 84);

namespace SDK {
/* 0273AD10 [v d_grass]: verified caller ABI. */
template <class... A> inline u32 operator_new(A... args) { return gabi::call<u32>(0x0273AD10, args...); }
/* 028F521C [v d_grass]: verified caller ABI. */
template <class... A> inline void clearMemory(A... args) { return gabi::call<void>(0x028F521C, args...); }
/* 025E1B34 [v d_grass]: verified caller ABI. */
template <class... A> inline void JAIZelBasic__seDeleteObject(A... args) {
    return gabi::call<void>(0x025E1B34, args...);
}
/* 028F026C [v d_grass]: verified caller ABI. */
template <class... A> inline void Vector_staticInit(A... args) {
    return gabi::call<void>(0x028F026C, args...);
}
/* 028ED6F8 [v d_grass]: verified caller ABI. */
template <class... A> inline void Angle_staticInit(A... args) {
    return gabi::call<void>(0x028ED6F8, args...);
}
/* 028EAB2C [v d_grass]: verified caller ABI. */
template <class... A> inline void ShortVector_staticInit(A... args) {
    return gabi::call<void>(0x028EAB2C, args...);
}
/* 0273AF40 [v d_grass]: verified caller ABI. */
template <class... A> inline void operator_delete(A... args) { return gabi::call<void>(0x0273AF40, args...); }
/* 027FB40C [v d_grass]: verified caller ABI. */
template <class... A> inline void ModelPacket_construct(A... args) {
    return gabi::call<void>(0x027FB40C, args...);
}
/* 027B5BD8 [v d_grass]: verified caller ABI. */
template <class... A> inline void VertexBuffer_construct(A... args) {
    return gabi::call<void>(0x027B5BD8, args...);
}
/* 027BF734 [v d_grass]: verified caller ABI. */
template <class... A> inline void Shape_construct(A... args) { return gabi::call<void>(0x027BF734, args...); }
/* 027BF880 [v d_grass]: verified caller ABI. */
template <class... A> inline void Shape_destruct(A... args) { return gabi::call<void>(0x027BF880, args...); }
/* 027B5CBC [v d_grass]: verified caller ABI. */
template <class... A> inline void VertexBuffer_destruct(A... args) {
    return gabi::call<void>(0x027B5CBC, args...);
}
/* 027FB528 [v d_grass]: verified caller ABI. */
template <class... A> inline void ModelPacket_destruct(A... args) {
    return gabi::call<void>(0x027FB528, args...);
}
/* 028EFFD0 [v d_grass]: verified caller ABI. */
template <class... A> inline void constructArray(A... args) { return gabi::call<void>(0x028EFFD0, args...); }
/* 0256019C [v d_grass]: verified caller ABI. */
template <class... A> inline u32 dKy_rain_check(A... args) { return gabi::call<u32>(0x0256019C, args...); }
/* 025200D4 [v d_grass]: verified caller ABI. */
template <class... A> inline u32 dComIfGp_get(A... args) { return gabi::call<u32>(0x025200D4, args...); }
/* 0254D9A0 [v d_grass]: verified caller ABI. */
template <class... A> inline void SafeString_assureTermination(A... args) {
    return gabi::call<void>(0x0254D9A0, args...);
}
/* 02019788 [v d_grass]: verified caller ABI. */
template <class... A> inline f32 cM_rnd(A... args) { return gabi::call<f32>(0x02019788, args...); }
/* 025A847C [v d_grass]: verified caller ABI. */
template <class... A> inline void ParticleControl_set(A... args) {
    return gabi::call<void>(0x025A847C, args...);
}
/* 02008E0C [v d_grass]: verified caller ABI. */
template <class... A> inline void cBgS_GndChk__cBgS_GndChk(A... args) {
    return gabi::call<void>(0x02008E0C, args...);
}
/* 02008974 [v d_grass]: verified caller ABI. */
template <class... A> inline f32 cBgS__GroundCross(A... args) { return gabi::call<f32>(0x02008974, args...); }
/* 02008DAC [v d_grass]: verified caller ABI. */
template <class... A> inline void cBgS_Chk___cBgS_Chk(A... args) {
    return gabi::call<void>(0x02008DAC, args...);
}
/* 025DBE00 [v d_grass]: verified caller ABI. */
template <class... A> inline u32 fopOvlpM_IsPeek(A... args) { return gabi::call<u32>(0x025DBE00, args...); }
/* 02549F4C [v d_grass]: verified caller ABI. */
template <class... A> inline f32 checkGroundY(A... args) { return gabi::call<f32>(0x02549F4C, args...); }
/* 020198D8 [v d_grass]: verified caller ABI. */
template <class... A> inline f32 cM_rndF(A... args) { return gabi::call<f32>(0x020198D8, args...); }
/* 0254AC40 [v d_grass]: verified caller ABI. */
template <class... A> inline void dGrass_room_c__newData(A... args) {
    return gabi::call<void>(0x0254AC40, args...);
}
/* 0273AA24 [v d_grass]: verified caller ABI. */
template <class... A> inline void JUTAssertion__getSDevice(A... args) {
    return gabi::call<void>(0x0273AA24, args...);
}
/* 0254D19C [v d_grass]: verified caller ABI. */
template <class... A> inline void dGrass_packet_c__setData(A... args) {
    return gabi::call<void>(0x0254D19C, args...);
}
/* 028E8DD0 [v d_grass]: verified caller ABI. */
template <class... A> inline f32 PSVECSquareMag(A... args) { return gabi::call<f32>(0x028E8DD0, args...); }
/* 020195B0 [v d_grass]: verified caller ABI. */
template <class... A> inline u32 cM_atan2s(A... args) { return gabi::call<u32>(0x020195B0, args...); }
/* 028F4384 [v d_grass]: verified caller ABI. */
template <class... A> inline f32 sqrtf(A... args) { return gabi::call<f32>(0x028F4384, args...); }
/* 025A8D40 [v d_grass]: verified caller ABI. */
template <class... A> inline void ParticleControl_setSimple(A... args) {
    return gabi::call<void>(0x025A8D40, args...);
}
/* 025A8D10 [v d_grass]: verified caller ABI. */
template <class... A> inline u32 ParticleControl_getEmitter(A... args) {
    return gabi::call<u32>(0x025A8D10, args...);
}
/* 0281E5A8 [v d_grass]: verified caller ABI. */
template <class... A> inline void Emitter_setColor(A... args) {
    return gabi::call<void>(0x0281E5A8, args...);
}
/* 02549D38 [v d_grass]: verified caller ABI. */
template <class... A> inline void setBatta(A... args) { return gabi::call<void>(0x02549D38, args...); }
/* 0254A14C [v d_grass]: verified caller ABI. */
template <class... A> inline s32 dGrass_packet_c__newAnm(A... args) {
    return gabi::call<s32>(0x0254A14C, args...);
}
/* 025157AC [v d_grass]: verified caller ABI. */
template <class... A> inline u32 dCcD_GetGObjInf(A... args) { return gabi::call<u32>(0x025157AC, args...); }
/* 028E8E64 [v d_grass]: verified caller ABI. */
template <class... A> inline void PSVECScale(A... args) { return gabi::call<void>(0x028E8E64, args...); }
/* 0254A458 [v d_grass]: verified caller ABI. */
template <class... A> inline void dGrass_data_c__WorkAt_NoCutAnim(A... args) {
    return gabi::call<void>(0x0254A458, args...);
}
/* 0201A478 [v d_grass]: verified caller ABI. */
template <class... A> inline void csXyz__csXyz(A... args) { return gabi::call<void>(0x0201A478, args...); }
/* 025D8120 [v d_grass]: verified caller ABI. */
template <class... A> inline void fopAcM_createItemFromTable(A... args) {
    return gabi::call<void>(0x025D8120, args...);
}
/* 02520540 [v d_grass]: verified caller ABI. */
template <class... A> inline u32 dComIfGp_getReverb(A... args) {
    return gabi::call<u32>(0x02520540, args...);
}
/* 025E1A40 [v d_grass]: verified caller ABI. */
template <class... A> inline void mDoAud_seStart(A... args) { return gabi::call<void>(0x025E1A40, args...); }
/* 0251694C [v d_grass]: verified caller ABI. */
template <class... A> inline void HitInfo_construct(A... args) {
    return gabi::call<void>(0x0251694C, args...);
}
/* 025170D8 [v d_grass]: verified caller ABI. */
template <class... A> inline u32 dCcMassS_Mng__Chk(A... args) { return gabi::call<u32>(0x025170D8, args...); }
/* 0200F378 [v d_grass]: verified caller ABI. */
template <class... A> inline u32 cLib_addCalcAngleS(A... args) {
    return gabi::call<u32>(0x0200F378, args...);
}
/* 0200F8D0 [v d_grass]: verified caller ABI. */
template <class... A> inline u32 cLib_chaseAngleS(A... args) { return gabi::call<u32>(0x0200F8D0, args...); }
/* 0254A198 [v d_grass]: verified caller ABI. */
template <class... A> inline void dGrass_data_c__WorkCo(A... args) {
    return gabi::call<void>(0x0254A198, args...);
}
/* 0254A7C8 [v d_grass]: verified caller ABI. */
template <class... A> inline void dGrass_data_c__WorkAt(A... args) {
    return gabi::call<void>(0x0254A7C8, args...);
}
/* 02578348 [v d_grass]: verified caller ABI. */
template <class... A> inline float dKyw_get_wind_pow(A... args) {
    return gabi::call<float>(0x02578348, args...);
}
/* 020184DC [v d_grass]: verified caller ABI. */
template <class... A> inline void cM3dGCyl__SetR(A... args) { return gabi::call<void>(0x020184DC, args...); }
/* 02018428 [v d_grass]: verified caller ABI. */
template <class... A> inline void cM3dGCyl__SetH(A... args) { return gabi::call<void>(0x02018428, args...); }
/* 0254AA3C [v d_grass]: verified caller ABI. */
template <class... A> inline void dGrass_data_c__hitCheck(A... args) {
    return gabi::call<void>(0x0254AA3C, args...);
}
/* 025F1884 [v d_grass]: verified caller ABI. */
template <class... A> inline void mDoMtx_YrotS(A... args) { return gabi::call<void>(0x025F1884, args...); }
/* 025F1BF4 [v d_grass]: verified caller ABI. */
template <class... A> inline void mDoMtx_XrotM(A... args) { return gabi::call<void>(0x025F1BF4, args...); }
/* 025F1C28 [v d_grass]: verified caller ABI. */
template <class... A> inline void mDoMtx_YrotM(A... args) { return gabi::call<void>(0x025F1C28, args...); }
/* 028E90D4 [v d_grass]: verified caller ABI. */
template <class... A> inline void PSMTXCopy(A... args) { return gabi::call<void>(0x028E90D4, args...); }
/* 0283801C [v d_grass]: verified caller ABI. */
template <class... A> inline void J3DUClipper__calcViewFrustum(A... args) {
    return gabi::call<void>(0x0283801C, args...);
}
/* 02838148 [v d_grass]: verified caller ABI. */
template <class... A> inline u32 J3DUClipper__clip(A... args) { return gabi::call<u32>(0x02838148, args...); }
/* 028E93CC [v d_grass]: verified caller ABI. */
template <class... A> inline void PSMTXTrans(A... args) { return gabi::call<void>(0x028E93CC, args...); }
/* 027F0E04 [v d_grass]: verified caller ABI. */
template <class... A> inline void J3DDrawBuffer__entryImm(A... args) {
    return gabi::call<void>(0x027F0E04, args...);
}
/* 0254CA98 [v d_grass]: verified caller ABI. */
template <class... A> inline void buildShaders(A... args) { return gabi::call<void>(0x0254CA98, args...); }
/* 02755FEC [v d_grass]: verified caller ABI. */
template <class... A> inline u32 Heap_findContaining(A... args) {
    return gabi::call<u32>(0x02755FEC, args...);
}
/* 027BF7E8 [v d_grass]: verified caller ABI. */
template <class... A> inline void Shape_clear(A... args) { return gabi::call<void>(0x027BF7E8, args...); }
/* 027BE2B0 [v d_grass]: verified caller ABI. */
template <class... A> inline void Texture_destruct(A... args) {
    return gabi::call<void>(0x027BE2B0, args...);
}
/* 027B54A0 [v d_grass]: verified caller ABI. */
template <class... A> inline void IndexBuffer_destruct(A... args) {
    return gabi::call<void>(0x027B54A0, args...);
}
/* 028F0164 [v d_grass]: verified caller ABI. */
template <class... A> inline void destroyArray(A... args) { return gabi::call<void>(0x028F0164, args...); }
/* 027FD764 [v d_grass]: verified caller ABI. */
template <class... A> inline void ModelBinding_destruct(A... args) {
    return gabi::call<void>(0x027FD764, args...);
}
/* 027F13DC [v d_grass]: verified caller ABI. */
template <class... A> inline void Packet_destruct(A... args) { return gabi::call<void>(0x027F13DC, args...); }
/* 0254C934 [v d_grass]: verified caller ABI. */
template <class... A> inline void copyMatrix(A... args) { return gabi::call<void>(0x0254C934, args...); }
/* 02555D0C [v d_grass]: verified caller ABI. */
template <class... A> inline u32 dKy_getEnvlight(A... args) { return gabi::call<u32>(0x02555D0C, args...); }
/* 0255FFF4 [v d_grass]: verified caller ABI. */
template <class... A> inline void dKy_tevstr_init(A... args) { return gabi::call<void>(0x0255FFF4, args...); }
/* 025626A4 [v d_grass]: verified caller ABI. */
template <class... A> inline void dScnKy_env_light_c__settingTevStruct(A... args) {
    return gabi::call<void>(0x025626A4, args...);
}
/* 0255FAF0 [v d_grass]: verified caller ABI. */
template <class... A> inline void dKy_setLight(A... args) { return gabi::call<void>(0x0255FAF0, args...); }
/* 027FDA54 [v d_grass]: verified caller ABI. */
template <class... A> inline void ModelBinding_build(A... args) {
    return gabi::call<void>(0x027FDA54, args...);
}
/* 0254C9D4 [v d_grass]: verified caller ABI. */
template <class... A> inline void colorToFloat(A... args) { return gabi::call<void>(0x0254C9D4, args...); }
/* 0274D458 [v d_grass]: verified caller ABI. */
template <class... A> inline void ColorF_scale(A... args) { return gabi::call<void>(0x0274D458, args...); }
/* 027FE010 [v d_grass]: verified caller ABI. */
template <class... A> inline void ModelBinding_apply(A... args) {
    return gabi::call<void>(0x027FE010, args...);
}
/* 0274D2AC [v d_grass]: verified caller ABI. */
template <class... A> inline void ColorF_scaleInPlace(A... args) {
    return gabi::call<void>(0x0274D2AC, args...);
}
/* 027FB678 [v d_grass]: verified caller ABI. */
template <class... A> inline void ModelPacket_update(A... args) {
    return gabi::call<void>(0x027FB678, args...);
}
/* 027F29D4 [v d_grass]: verified caller ABI. */
template <class... A> inline u32 ShaderCache_get(A... args) { return gabi::call<u32>(0x027F29D4, args...); }
/* 027BB9E0 [v d_grass]: verified caller ABI. */
template <class... A> inline void Model_update(A... args) { return gabi::call<void>(0x027BB9E0, args...); }
/* 027B9F68 [v d_grass]: verified caller ABI. */
template <class... A> inline void Shader_bind(A... args) { return gabi::call<void>(0x027B9F68, args...); }
/* C00060E0 [v d_grass]: verified caller ABI. */
template <class... A> inline void GX2CallDisplayList(A... args) {
    return gabi::call<void>(0xC00060E0, args...);
}
/* 027BB7CC [v d_grass]: verified caller ABI. */
template <class... A> inline void Model_draw(A... args) { return gabi::call<void>(0x027BB7CC, args...); }
/* C0006900 [v d_grass]: verified caller ABI. */
template <class... A> inline void GX2SetPixelUniformBlock(A... args) {
    return gabi::call<void>(0xC0006900, args...);
}
/* C0006A38 [v d_grass]: verified caller ABI. */
template <class... A> inline void GX2SetVertexUniformBlock(A... args) {
    return gabi::call<void>(0xC0006A38, args...);
}
/* C00068A8 [v d_grass]: verified caller ABI. */
template <class... A> inline void GX2SetGeometryUniformBlock(A... args) {
    return gabi::call<void>(0xC00068A8, args...);
}
/* 027FFE54 [v d_grass]: verified caller ABI. */
template <class... A> inline void RenderContext_apply(A... args) {
    return gabi::call<void>(0x027FFE54, args...);
}
/* 027BE53C [v d_grass]: verified caller ABI. */
template <class... A> inline void Texture_bind(A... args) { return gabi::call<void>(0x027BE53C, args...); }
/* 02750250 [v d_grass]: verified caller ABI. */
template <class... A> inline void RenderState_construct(A... args) {
    return gabi::call<void>(0x02750250, args...);
}
/* 0280037C [v d_grass]: verified caller ABI. */
template <class... A> inline void RenderState_bind(A... args) {
    return gabi::call<void>(0x0280037C, args...);
}
/* 02750370 [v d_grass]: verified caller ABI. */
template <class... A> inline void RenderState_apply(A... args) {
    return gabi::call<void>(0x02750370, args...);
}
/* 027BFE5C [v d_grass]: verified caller ABI. */
template <class... A> inline void Shape_bind(A... args) { return gabi::call<void>(0x027BFE5C, args...); }
/* C0006178 [v d_grass]: verified caller ABI. */
template <class... A> inline void GX2DrawIndexedEx(A... args) {
    return gabi::call<void>(0xC0006178, args...);
}
/* 02756140 [v d_grass]: verified caller ABI. */
template <class... A> inline u32 Heap_getCurrent(A... args) { return gabi::call<u32>(0x02756140, args...); }
/* 027F1278 [v d_grass]: verified caller ABI. */
template <class... A> inline void Packet_construct(A... args) {
    return gabi::call<void>(0x027F1278, args...);
}
/* 027FD6F4 [v d_grass]: verified caller ABI. */
template <class... A> inline void ModelBinding_construct(A... args) {
    return gabi::call<void>(0x027FD6F4, args...);
}
/* 027B5430 [v d_grass]: verified caller ABI. */
template <class... A> inline void IndexBuffer_construct(A... args) {
    return gabi::call<void>(0x027B5430, args...);
}
/* 027BDF7C [v d_grass]: verified caller ABI. */
template <class... A> inline void Texture_construct(A... args) {
    return gabi::call<void>(0x027BDF7C, args...);
}
/* 027BE6B8 [v d_grass]: verified caller ABI. */
template <class... A> inline void TextureInfo_construct(A... args) {
    return gabi::call<void>(0x027BE6B8, args...);
}
/* 0254ACB4 [v d_grass]: verified caller ABI. */
template <class... A> inline void GrassPacket_setAnm(A... args) {
    return gabi::call<void>(0x0254ACB4, args...);
}
/* 027FFCBC [v d_grass]: verified caller ABI. */
template <class... A> inline u32 Archive_get(A... args) { return gabi::call<u32>(0x027FFCBC, args...); }
/* 027B90AC [v d_grass]: verified caller ABI. */
template <class... A> inline s32 Directory_find(A... args) { return gabi::call<s32>(0x027B90AC, args...); }
/* 02800B0C [v d_grass]: verified caller ABI. */
template <class... A> inline void ArchiveEntry_load(A... args) {
    return gabi::call<void>(0x02800B0C, args...);
}
/* 0280068C [v d_grass]: verified caller ABI. */
template <class... A> inline void MaterialCollection_build(A... args) {
    return gabi::call<void>(0x0280068C, args...);
}
/* 027FF478 [v d_grass]: verified caller ABI. */
template <class... A> inline void VertexBuffer_bind(A... args) {
    return gabi::call<void>(0x027FF478, args...);
}
/* 027FF530 [v d_grass]: verified caller ABI. */
template <class... A> inline void Shape_build(A... args) { return gabi::call<void>(0x027FF530, args...); }
/* 027FD838 [v d_grass]: verified caller ABI. */
template <class... A> inline void ModelBinding_setFlags(A... args) {
    return gabi::call<void>(0x027FD838, args...);
}
/* 027FB5D4 [v d_grass]: verified caller ABI. */
template <class... A> inline void ModelPacket_setKind(A... args) {
    return gabi::call<void>(0x027FB5D4, args...);
}
/* 026124B0 [v d_grass]: verified caller ABI. */
template <class... A> inline u32 ResourceManager_getTexture(A... args) {
    return gabi::call<u32>(0x026124B0, args...);
}
/* 027B54E0 [v d_grass]: verified caller ABI. */
template <class... A> inline void IndexBuffer_set(A... args) { return gabi::call<void>(0x027B54E0, args...); }
/* 027B5E94 [v d_grass]: verified caller ABI. */
template <class... A> inline void VertexBuffer_invalidate(A... args) {
    return gabi::call<void>(0x027B5E94, args...);
}
/* 0274FBF8 [v d_grass]: verified caller ABI. */
template <class... A> inline void DrawLock_lock(A... args) { return gabi::call<void>(0x0274FBF8, args...); }
/* 02773870 [v d_grass]: verified caller ABI. */
template <class... A> inline void TextureInfo_load(A... args) {
    return gabi::call<void>(0x02773870, args...);
}
/* 0274FCCC [v d_grass]: verified caller ABI. */
template <class... A> inline void DrawLock_unlock(A... args) { return gabi::call<void>(0x0274FCCC, args...); }
/* 027BDEB4 [v d_grass]: verified caller ABI. */
template <class... A> inline void Texture_assign(A... args) { return gabi::call<void>(0x027BDEB4, args...); }
} // namespace SDK
namespace Data {
enum : u32 {
    emptyString = 0x10000160,
    epsilon = 0x100030B8,
    safeStringVtable = 0x1004E1F4,
    groundBaseVtable = 0x1004E20C,
    groundPolyVtable = 0x1004E21C,
    groundCheckVtable = 0x1004E22C,
    groundInterfaceVtable = 0x1004E23C,
    groundVtable = 0x1004E24C,
    battaProbability = 0x1004E25C,
    stageKin = 0x1004E260,
    stageXboss = 0x1004E264,
    groundLift = 0x1004E26C,
    groundLimit = 0x1004E270,
    zero = 0x1004E274,
    contactDistanceSquared = 0x1004E278,
    runSpeed = 0x1004E27C,
    runParticleLift = 0x1004E280,
    one = 0x1004E284,
    bendRadius = 0x1004E288,
    normalScale = 0x1004E28C,
    randomBendRange = 0x1004E290,
    randomBendBias = 0x1004E294,
    cutParticleLift = 0x1004E2A0,
    hitAssertFile = 0x1004E2A4,
    hitAssertCondition = 0x1004E2B0,
    resourceStageKin = 0x1004E2C4,
    resourceStageXboss = 0x1004E2C8,
    forestTextureName = 0x1004E2D0,
    grassTextureName = 0x1004E2DC,
    modelResourceName = 0x1004E2EC,
    textureArchiveName = 0x1004E2F8,
    textureResourceName = 0x1004E308,
    depthBias = 0x1004E320,
    windScale = 0x1004E324,
    maxWind = 0x1004E328,
    massHeight = 0x1004E338,
    stageHyrule = 0x1004E33C,
    colorRange = 0x1004E344,
    farScale = 0x1004E348,
    clipRadius = 0x1004E34C,
    baseAnimationCount = 0x1004E350,
    allocationAssertFile = 0x1004E354,
    allocationAssertCondition = 0x1004E360,
    piLower = 0x1004E380,
    piUpper = 0x1004E384,
    vertexLayout = 0x1004E388,
    grassPacketVtable = 0x1004E398,
    shaderOne = 0x1014517C,
    shaderZero = 0x10145180,
    lightTemplate = 0x1016E414,
    modelPacketVtable = 0x1016EF84,
    shaderPacketVtable = 0x1016EFB4,
    grassVertexCount = 0x101E1D44,
    grassIndexCount = 0x101E1D48,
    cutVertexCount = 0x101E1D4C,
    cutIndexCount = 0x101E1D50,
    forestVertexCount = 0x101E1D54,
    grassPositions = 0x101E1D58,
    forestCutVertexCount = 0x101E1D5C,
    cutSoundFlag = 0x101E1D60,
    grassNormals = 0x101E1E84,
    grassColors = 0x101E1FAC,
    grassTexCoords = 0x101E2144,
    grassIndices = 0x101E2214,
    cutPositions = 0x101E22E0,
    cutNormals = 0x101E23A0,
    cutColors = 0x101E245C,
    cutTexCoords = 0x101E2564,
    cutIndices = 0x101E25EC,
    forestPositions = 0x101E264C,
    forestNormals = 0x101E2934,
    forestColors = 0x101E2C18,
    forestTexCoords = 0x101E3000,
    forestIndices = 0x101E31F8,
    forestCutPositions = 0x101E32C4,
    forestCutNormals = 0x101E34A4,
    forestCutColors = 0x101E3680,
    forestCutTexCoords = 0x101E3908,
    vectorStatic = 0x101E3A50,
    angleStatic = 0x101E3A5C,
    shortVectorStatic = 0x101E3A68,
    monotone = 0x101F4829,
    resourceManager = 0x101F4F7C,
    drawLock = 0x101F8B18,
    heapManager = 0x101F8B4C,
    counterTimer = 0x101FF560,
    piPair = 0x10475988,
    piPairUpper = 0x1047598C,
    cutAngle = 0x10475990,
    byteStatic = 0x10475998,
    shortStatic = 0x10475999,
    zeroAngleX = 0x1047599C,
    zeroAngleY = 0x104759A0,
    zeroVectorX = 0x104759A4,
    zeroVectorY = 0x104759A8,
    angleGuard = 0x104759AC,
    stayRoomNo = 0x1047E6C8,
    roomControl = 0x1047E6CC,
    clipper = 0x1048CFF0,
    originalFar = 0x1048D04C,
    matrixStack = 0x1048D0CC,
    sincosTable = 0x104A44F8,
    shaderContext = 0x104B45C0,
    viewMatrix = 0x104B45F8,
    opaqueDrawBuffer = 0x104B4634,
    renderModel = 0x104B4708,
    renderView = 0x104B470C,
    defaultRenderState = 0x104B474C,
};
}
namespace Target {
// HD: Array construction/destruction uses fixed guest callbacks.
enum : u32 {
    dGrass_data_c__ct = 0x0254A0DC,
    dGrass_anm_c__ct = 0x0254D49C,
    dGrass_room_c__ct = 0x0254D4D8,
    Resource_create = 0x0254D514,
    ModelPacket_create = 0x0254D570,
    ColorF_construct = 0x0254D5E0,
    ShaderPacket_create = 0x0254D60C,
    ModelPacket_delete = 0x0254D894,
    ShaderPacket_delete = 0x0254D8E8,
    Resource_delete = 0x0254D93C,
    operator_new = 0x0273AD10,
    clearMemory = 0x028F521C,
    JAIZelBasic__seDeleteObject = 0x025E1B34,
    Vector_staticInit = 0x028F026C,
    Angle_staticInit = 0x028ED6F8,
    ShortVector_staticInit = 0x028EAB2C,
    operator_delete = 0x0273AF40,
    ModelPacket_construct = 0x027FB40C,
    VertexBuffer_construct = 0x027B5BD8,
    Shape_construct = 0x027BF734,
    Shape_destruct = 0x027BF880,
    VertexBuffer_destruct = 0x027B5CBC,
    ModelPacket_destruct = 0x027FB528,
    constructArray = 0x028EFFD0,
    dKy_rain_check = 0x0256019C,
    dComIfGp_get = 0x025200D4,
    SafeString_assureTermination = 0x0254D9A0,
    cM_rnd = 0x02019788,
    ParticleControl_set = 0x025A847C,
    cBgS_GndChk__cBgS_GndChk = 0x02008E0C,
    cBgS__GroundCross = 0x02008974,
    cBgS_Chk___cBgS_Chk = 0x02008DAC,
    fopOvlpM_IsPeek = 0x025DBE00,
    checkGroundY = 0x02549F4C,
    cM_rndF = 0x020198D8,
    dGrass_room_c__newData = 0x0254AC40,
    JUTAssertion__getSDevice = 0x0273AA24,
    dGrass_packet_c__setData = 0x0254D19C,
    PSVECSquareMag = 0x028E8DD0,
    cM_atan2s = 0x020195B0,
    sqrtf = 0x028F4384,
    ParticleControl_setSimple = 0x025A8D40,
    ParticleControl_getEmitter = 0x025A8D10,
    Emitter_setColor = 0x0281E5A8,
    setBatta = 0x02549D38,
    dGrass_packet_c__newAnm = 0x0254A14C,
    dCcD_GetGObjInf = 0x025157AC,
    PSVECScale = 0x028E8E64,
    dGrass_data_c__WorkAt_NoCutAnim = 0x0254A458,
    csXyz__csXyz = 0x0201A478,
    fopAcM_createItemFromTable = 0x025D8120,
    dComIfGp_getReverb = 0x02520540,
    mDoAud_seStart = 0x025E1A40,
    HitInfo_construct = 0x0251694C,
    dCcMassS_Mng__Chk = 0x025170D8,
    cLib_addCalcAngleS = 0x0200F378,
    cLib_chaseAngleS = 0x0200F8D0,
    dGrass_data_c__WorkCo = 0x0254A198,
    dGrass_data_c__WorkAt = 0x0254A7C8,
    dKyw_get_wind_pow = 0x02578348,
    cM3dGCyl__SetR = 0x020184DC,
    cM3dGCyl__SetH = 0x02018428,
    dGrass_data_c__hitCheck = 0x0254AA3C,
    mDoMtx_YrotS = 0x025F1884,
    mDoMtx_XrotM = 0x025F1BF4,
    mDoMtx_YrotM = 0x025F1C28,
    PSMTXCopy = 0x028E90D4,
    J3DUClipper__calcViewFrustum = 0x0283801C,
    J3DUClipper__clip = 0x02838148,
    PSMTXTrans = 0x028E93CC,
    J3DDrawBuffer__entryImm = 0x027F0E04,
    buildShaders = 0x0254CA98,
    Heap_findContaining = 0x02755FEC,
    Shape_clear = 0x027BF7E8,
    Texture_destruct = 0x027BE2B0,
    IndexBuffer_destruct = 0x027B54A0,
    destroyArray = 0x028F0164,
    ModelBinding_destruct = 0x027FD764,
    Packet_destruct = 0x027F13DC,
    copyMatrix = 0x0254C934,
    dKy_getEnvlight = 0x02555D0C,
    dKy_tevstr_init = 0x0255FFF4,
    dScnKy_env_light_c__settingTevStruct = 0x025626A4,
    dKy_setLight = 0x0255FAF0,
    ModelBinding_build = 0x027FDA54,
    colorToFloat = 0x0254C9D4,
    ColorF_scale = 0x0274D458,
    ModelBinding_apply = 0x027FE010,
    ColorF_scaleInPlace = 0x0274D2AC,
    ModelPacket_update = 0x027FB678,
    ShaderCache_get = 0x027F29D4,
    Model_update = 0x027BB9E0,
    Shader_bind = 0x027B9F68,
    GX2CallDisplayList = 0xC00060E0,
    Model_draw = 0x027BB7CC,
    GX2SetPixelUniformBlock = 0xC0006900,
    GX2SetVertexUniformBlock = 0xC0006A38,
    GX2SetGeometryUniformBlock = 0xC00068A8,
    RenderContext_apply = 0x027FFE54,
    Texture_bind = 0x027BE53C,
    RenderState_construct = 0x02750250,
    RenderState_bind = 0x0280037C,
    RenderState_apply = 0x02750370,
    Shape_bind = 0x027BFE5C,
    GX2DrawIndexedEx = 0xC0006178,
    Heap_getCurrent = 0x02756140,
    Packet_construct = 0x027F1278,
    ModelBinding_construct = 0x027FD6F4,
    IndexBuffer_construct = 0x027B5430,
    Texture_construct = 0x027BDF7C,
    TextureInfo_construct = 0x027BE6B8,
    GrassPacket_setAnm = 0x0254ACB4,
    Archive_get = 0x027FFCBC,
    Directory_find = 0x027B90AC,
    ArchiveEntry_load = 0x02800B0C,
    MaterialCollection_build = 0x0280068C,
    VertexBuffer_bind = 0x027FF478,
    Shape_build = 0x027FF530,
    ModelBinding_setFlags = 0x027FD838,
    ModelPacket_setKind = 0x027FB5D4,
    ResourceManager_getTexture = 0x026124B0,
    IndexBuffer_set = 0x027B54E0,
    VertexBuffer_invalidate = 0x027B5E94,
    DrawLock_lock = 0x0274FBF8,
    TextureInfo_load = 0x02773870,
    DrawLock_unlock = 0x0274FCCC,
    Texture_assign = 0x027BDEB4,
};
} // namespace Target

struct DispatchTable {
    be<u32> reserved0[3];
    be<u32> mpDelete;
    be<u32> reserved10;
    be<u32> mpAssureTermination;
    be<u32> reserved18[5];
    be<u32> mpGetShape;
    be<u32> reserved30;
    be<u32> mpAlloc;
    be<u32> reserved38;
    be<u32> mpFree;
    be<u32> reserved40[23];
    be<u32> mpGetNormal;
};
WWHD_OFFSET(DispatchTable, mpAssureTermination, 0x14);
WWHD_OFFSET(DispatchTable, mpGetShape, 0x2C);
WWHD_OFFSET(DispatchTable, mpGetNormal, 0x9C);
struct SinCos {
    be<f32> mSin, mCos;
};
struct Vertex {
    be<u32> mPosition[3], mNormal[3], mTexCoord[2], mColor[4];
};
WWHD_SIZE(Vertex, 48);
struct CacheLine {
    be<u32> mWords[8];
};
WWHD_SIZE(CacheLine, 32);
struct FloatVectorWords {
    be<u32> mWords[3];
};
struct ColorWords {
    be<u32> mWords[4];
};
struct TexCoordWords {
    be<u32> mWords[2];
};
struct LightScales {
    u8 padding0[36];
    be<f32> mAmbientScale, mDiffuseScale;
    u8 padding2C[320];
    be<f32> mColorScale;
};
WWHD_OFFSET(LightScales, mColorScale, 0x16C);
} // namespace GrassHD
