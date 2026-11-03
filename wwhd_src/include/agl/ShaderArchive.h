/* AGL#14 shader archive prefix, from its own constructor and update calls. */
#pragma once
#include "sead/HeapRegistry.h"
#include "agl/ShaderInputLayout.h"
namespace agl {
struct ShaderArchive_l;
struct ShaderArchiveOwnedObject_l { u8 mPrefix[4]; gptr<sead::DisposerVtable> mMethods; };
struct ShaderProgramSetMethods_l { u8 mPrefix[0x14]; be<u32> mDestroy; };
struct ShaderArchiveProgramView_l;
struct ShaderResourceWord_l { be<u32> mValue; };
struct ShaderVariantList_l { u8 mPrefix[4]; be<u32> mCount; };
struct ShaderVariantListHandle_l { gptr<ShaderVariantList_l> mList; };
struct ShaderVariantSource_l { be<u32> mSize; u8 mReserved[4]; be<u32> mValueCount; u8 mTail[4]; };
struct ShaderVariantSourceHandle_l { gptr<ShaderVariantSource_l> mSource; };
struct ShaderArchiveBinary_l {
    sead::SafeText mName;
    gptr<ShaderArchiveProgramView_l> mPrimary;
    be<u32> mVariantCount;
    gptr<ShaderArchiveProgramView_l> mVariants;
    be<u32> mVariantDimensionCount;
    gptr<void> mVariantRadices;
    u8 mReserved[4];
    be<u32> mContext;
    u8 mResourcePrefix[8];
    // Opaque word region: this caller writes slots0,2..5; slot1 retains its prior contents.
    be<u32> mResourceWords[6];
};
struct ShaderArchiveProgramView_l {
    be<u8> mFlags;
    u8 mOpaque[0x7B];
    gptr<ShaderArchiveBinary_l> mBinary;
    gptr<sead::DisposerVtable> mMethods;
    void initializeSources(const sead::SafeText* name,const ShaderVariantListHandle_l* variants,
        const ShaderResourceWord_l* first,u32 unused,const ShaderResourceWord_l* second,
        const ShaderResourceWord_l* third,const ShaderResourceWord_l* fourth,
        const ShaderResourceWord_l* fifth,sead::RegistryHeap* heap);
};
struct ShaderArchiveCallbackMethods_l { u8 mPrefix[0xC]; be<u32> mUpdate; };
struct ShaderArchiveCallback_l { gptr<ShaderArchiveCallbackMethods_l> mMethods; };
struct ShaderVariationSource_l { be<u32> mSize, mTextOffset; u8 mReserved[8]; };
struct ShaderVariationSourceHandle_l { gptr<ShaderVariationSource_l> mSource; };
struct ShaderVariation_l {
    sead::RegisteredObject mDisposer;
    be<u32> mNameMethods, mIndex, mFlags;
    gptr<ShaderArchive_l> mArchive;
    gptr<ShaderVariationSource_l> mSource;
    gptr<ShaderArchiveOwnedObject_l> mSourceObject, mCompiledObject;
    be<u32> mSelectionCount;
    gptr<be<u8>> mSelections;
    ShaderVariation_l* construct();
    void initialize(ShaderArchive_l* archive,u32 index,const ShaderVariationSourceHandle_l* handle,u32 compiled,sead::RegistryHeap* heap);
    void compile();
    void destroy(u32 flags);
};
struct ShaderStageReference_l { u8 mReserved[2]; be<s16> mVariationIndex; };
struct ShaderProgramSource_l { u8 mPrefix[8]; ShaderStageReference_l mStages[3]; };
struct ShaderProgramSourceHandle_l { gptr<ShaderProgramSource_l> mSource; };
struct ShaderStageBindings_l { be<u32> mSize, mCount; };
struct ShaderStageBinding_l { be<u32> mSize, mValueOffset; u8 mReserved[4]; };
struct ShaderArchiveStageView_l { u8 mPrefix[4]; gptr<ShaderInputLayout_l> mSource; };
struct ShaderArchiveProgramSet_l {
    gptr<ShaderProgramSetMethods_l> mMethods;
    be<u16> mProgramIndex, mVariantIndex;
    gptr<ShaderArchive_l> mArchive;
    ShaderInputLayout_l mLayouts[3];
    be<u32> mValueCount;
    gptr<be<u32>> mValues;
    be<u8> mFlags;
    u8 mPadding[3];
    ShaderArchiveProgramSet_l* construct();
    void initialize(ShaderArchive_l* archive,u32 index,const ShaderProgramSourceHandle_l* handle,sead::RegistryHeap* heap);
    void destroy(u32 flags);
    void updateLayouts();
    void markProgramDirty();
};
using ShaderLayoutConstructor_l = ShaderInputLayout_l* (*)(ShaderInputLayout_l*);
using ShaderLayoutDestructor_l = void (*)(ShaderInputLayout_l*, u32);
WWHD_OFFSET(ShaderVariation_l,mNameMethods,0x10);
WWHD_OFFSET(ShaderVariation_l,mFlags,0x18);
WWHD_OFFSET(ShaderVariation_l,mArchive,0x1C);
WWHD_OFFSET(ShaderVariation_l,mCompiledObject,0x28);
WWHD_OFFSET(ShaderVariation_l,mSelections,0x30);
WWHD_SIZE(ShaderVariation_l,0x34);
WWHD_OFFSET(ShaderArchiveProgramSet_l,mLayouts,0xC);
WWHD_OFFSET(ShaderArchiveProgramSet_l,mValueCount,0xE4);
WWHD_OFFSET(ShaderArchiveProgramSet_l,mFlags,0xEC);
WWHD_SIZE(ShaderArchiveProgramSet_l,0xF0);
struct ShaderArchive_l {
    sead::RegisteredObject mDisposer;
    be<u32> mNameMethods, mBinaryImageAddress;
    gptr<void> mResource;
    be<u32> mProgramCount;
    gptr<ShaderArchiveProgramView_l> mPrograms;
    be<u32> mContext;
    gptr<ShaderArchiveCallback_l> mCallback;
    gptr<void> mFilename;
    be<u16> mUpdateCount, mFlags;
    be<u32> mProgramSetCount;
    gptr<ShaderArchiveProgramSet_l> mProgramSets;
    be<u32> mVariationCount;
    gptr<ShaderVariation_l> mVariations;
    be<u32> mStringCount;
    gptr<gptr<be<u8>>> mStrings;
    be<u32> mBinaryCount;
    gptr<gptr<be<u8>>> mBinaries;
    ShaderArchive_l* construct();
    void releaseTransient();
    void release();
    void destroy(u32 flags);
    void updateVariations();
    u32 updatePrograms(u32 once);
    u32 update();
    s32 findProgram(const sead::SafeText* name);
};
WWHD_OFFSET(ShaderProgramSetMethods_l,mDestroy,0x14);
WWHD_OFFSET(ShaderArchiveProgramView_l,mBinary,0x7C);
WWHD_OFFSET(ShaderArchiveBinary_l,mContext,0x20);
WWHD_SIZE(ShaderArchiveProgramView_l,0x84);
WWHD_OFFSET(ShaderArchive_l,mProgramCount,0x1C);
WWHD_OFFSET(ShaderArchive_l,mContext,0x24);
WWHD_OFFSET(ShaderArchive_l,mUpdateCount,0x30);
WWHD_OFFSET(ShaderArchive_l,mFlags,0x32);
WWHD_OFFSET(ShaderArchive_l,mProgramSetCount,0x34);
WWHD_OFFSET(ShaderArchive_l,mVariationCount,0x3C);
WWHD_OFFSET(ShaderArchive_l,mStrings,0x48);
WWHD_SIZE(ShaderArchive_l,0x54);
}
