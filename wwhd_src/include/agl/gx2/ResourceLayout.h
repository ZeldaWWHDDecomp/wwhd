/* Prefix-sum storage layouts used by graphics resource builders. */
#pragma once
#include "wwhd.h"
namespace agl { namespace gx2util {
struct ResourceArchive;
struct KeyBits;
struct MeshBufferRecord;
template <class T> struct ResourceLink {
    be<s32> mDelta;
    T* get() const {
        s32 delta = mDelta;
        return delta == 0 ? NULL : reinterpret_cast<T*>(reinterpret_cast<u8*>(const_cast<ResourceLink*>(this)) + delta);
    }
};
struct UniformFixup {
    be<u8> mKind; be<u8> mExtraOffset; be<u16> mDataOffset;
    be<s32> mFlags; be<u32> mConverter; be<u16> mParent; be<u16> mTarget; u8 mTail[4];
};
/* A name as a self-relative link to its text; the 32-bit byte count precedes the text (KeyBits). */
struct NameLink {
    be<s32> mDelta;
    const KeyBits* key() const;
};
/* ---- resource dictionaries (Patricia trees keyed by name bits) ---- */
struct DictionaryNode {
    be<u32> mBit; be<u16> mLeft; be<u16> mRight; NameLink mName; ResourceLink<void> mValue;
};
struct ResourceDictionary {
    be<u32> mSize; be<u32> mEntryCount; DictionaryNode mNodes[1];
    DictionaryNode* findKey(const KeyBits* key);
    static s32 findIndex(ResourceDictionary* dictionary, const KeyBits* key);
    DictionaryNode* findText(const be<u8>* text, u32 length);
    static s32 findTextIndex(ResourceDictionary* dictionary, const be<u8>* text);
    static void* findTextValue(ResourceDictionary* dictionary, const be<u8>* text);
    static void* findKeyValue(ResourceDictionary* dictionary, const KeyBits* key);
};
/* Texture binding record of a material (uniform) or texture-animation resource: the texture's name,
   then the texture link, zero until resolved. */
struct UniformBinding {
    /* +0x00 */ NameLink mName;
    /* +0x04 */ ResourceLink<void> mTexture;
    u32 resolve(ResourceArchive* archive);
};
struct LayoutBlock { be<u32> mSize; be<u32> mOffset; };
struct PoseBuilder;
struct UniformBuilder;
struct ParameterBuilder;
struct PoseLayout {
    gptr<LayoutBlock> mBlocks; be<u32> mTotal; LayoutBlock mSlots[3];
    void calculate(const PoseBuilder* builder);
};
struct UniformLayout {
    gptr<LayoutBlock> mBlocks; be<u32> mTotal; LayoutBlock mSlots[4];
    void calculate(const UniformBuilder* builder);
};
struct ParameterLayout {
    gptr<LayoutBlock> mBlocks; be<u32> mTotal; LayoutBlock mSlots[5];
    void calculate(const ParameterBuilder* builder);
};
struct PoseResource {
    u8 mPrefix[4]; be<u32> mFlags; be<u16> mBoneCount; u8 mTail[6];
    ResourceLink<ResourceDictionary> mNames; ResourceLink<void> mBones;
    s32 findDescendantEnd(s32 bone) const;
};
struct UniformDescriptor {
    u8 mPrefix[0x10]; be<u8> mTextureCount; be<u8> mWordCount; be<u16> mBitCount;
    be<u16> mDynamicCount; be<u16> mDataBytes;
    be<u16> mByteCount; u8 mPadding[0xE];
    ResourceLink<UniformBinding> mBindings;
    u8 mReserved[4]; ResourceLink<ResourceDictionary> mVisibilityNames; ResourceLink<UniformFixup> mFixups;
    ResourceLink<ResourceDictionary> mChannelNames; ResourceLink<void> mInitialData;
    void initializeConverters();
    u32 resolveTextures(ResourceArchive* archive);
};
struct ParameterDescriptor {
    u8 mPrefix[0x10]; be<u16> mBoneIndex; u8 mBoneTail[5];
    /* +0x17 */ be<u8> mMeshCount;
    /* +0x18 */ be<u8> mFloatCount; u8 mReserved;
    be<u16> mCurveCount; be<f32> mDefaultValue;
    u8 mMeshHeader[4];
    /* +0x24 */ ResourceLink<MeshBufferRecord> mMeshes;
    u8 mTail[0xC];
    /* +0x34 */ ResourceLink<be<u32>> mDefaults;
    void initializeBuffers();
};
struct PoseBuilder {
    gptr<PoseResource> mResource; be<u32> mFlags; PoseLayout mLayout;
    u32 workSize();
};
struct UniformBuilder {
    gptr<UniformDescriptor> mResource; be<s32> mViewCount; UniformLayout mLayout;
    u32 workSize();
};
struct ParameterBuilder {
    gptr<ParameterDescriptor> mResource;
    be<u32> mViewCount; be<u32> mInstanceCount;
    be<u8> mMultiBuffer; be<u8> mCached; u8 mReserved[2];
    be<u32> mCustomBytes; ParameterLayout mLayout;
    u32 workSize();
};
struct ParameterScratch { be<u32> mWords[5]; };
struct ParameterRuntime {
    gptr<ParameterDescriptor> mResource; be<u32> mFlags;
    be<u8> mInstanceCount; be<u8> mMultiBuffer; be<u8> mBufferCount; be<u8> mViewCount;
    gptr<ParameterScratch> mScratch; gptr<be<f32>> mFloats;
    gptr<void> mDefaults; gptr<void> mCurves; gptr<void> mCustom;
    be<u32> mCustomBytes; be<u32> mCacheState; gptr<void> mStorage; be<u32> mReserved;
    void clearValues();
    u32 initialize(ParameterBuilder* builder, void* storage, u32 size);
};
WWHD_SIZE(LayoutBlock, 8);
WWHD_OFFSET(LayoutBlock, mOffset, 4);
WWHD_SIZE(PoseLayout, 0x20);
WWHD_SIZE(UniformLayout, 0x28);
WWHD_SIZE(ParameterLayout, 0x30);
WWHD_OFFSET(PoseResource, mFlags, 4);
WWHD_OFFSET(PoseResource, mBoneCount, 8);
WWHD_OFFSET(PoseResource, mBones, 0x14);
WWHD_SIZE(PoseResource, 0x18);
WWHD_OFFSET(UniformDescriptor, mBitCount, 0x12);
WWHD_OFFSET(UniformDescriptor, mDataBytes, 0x16);
WWHD_OFFSET(UniformDescriptor, mBindings, 0x28);
WWHD_OFFSET(UniformDescriptor, mFixups, 0x34);
WWHD_OFFSET(UniformDescriptor, mInitialData, 0x3C);
WWHD_SIZE(UniformDescriptor, 0x40);
WWHD_SIZE(UniformFixup, 0x14);
WWHD_OFFSET(UniformFixup, mFlags, 4);
WWHD_OFFSET(UniformFixup, mTarget, 0xE);
WWHD_OFFSET(UniformFixup, mConverter, 8);
WWHD_OFFSET(UniformFixup, mParent, 0xC);
WWHD_OFFSET(UniformDescriptor, mTextureCount, 0x10);
WWHD_OFFSET(UniformDescriptor, mVisibilityNames, 0x30);
WWHD_OFFSET(UniformDescriptor, mChannelNames, 0x38);
WWHD_OFFSET(PoseResource, mNames, 0x10);
WWHD_SIZE(NameLink, 4);
WWHD_OFFSET(DictionaryNode, mName, 8); WWHD_OFFSET(DictionaryNode, mValue, 12); WWHD_SIZE(DictionaryNode, 16);
WWHD_OFFSET(ResourceDictionary, mNodes, 8);
WWHD_OFFSET(UniformBinding, mTexture, 4);
WWHD_SIZE(UniformBinding, 8);
WWHD_OFFSET(ParameterDescriptor, mMeshCount, 0x17);
WWHD_OFFSET(ParameterDescriptor, mFloatCount, 0x18);
WWHD_OFFSET(ParameterDescriptor, mCurveCount, 0x1A);
WWHD_OFFSET(ParameterDescriptor, mMeshes, 0x24);
WWHD_OFFSET(ParameterDescriptor, mDefaults, 0x34);
WWHD_SIZE(ParameterDescriptor, 0x38);
WWHD_OFFSET(PoseBuilder, mLayout, 8);
WWHD_SIZE(PoseBuilder, 0x28);
WWHD_OFFSET(UniformBuilder, mLayout, 8);
WWHD_SIZE(UniformBuilder, 0x30);
WWHD_OFFSET(ParameterBuilder, mLayout, 0x14);
WWHD_SIZE(ParameterBuilder, 0x44);
WWHD_SIZE(ParameterScratch, 0x14);
WWHD_OFFSET(ParameterRuntime, mFloats, 0x10);
WWHD_OFFSET(ParameterRuntime, mStorage, 0x28);
WWHD_SIZE(ParameterRuntime, 0x30);
} }
