/** Resource management.
 * HD: SafeString names, red-black resource maps and pool nodes replace the GC fixed arrays.
 * This header holds the template layouts; the runtime control/info layouts are separate. */
#pragma once
#include "bindings.h"
namespace resorce_hd {
struct ResourcePool;
struct ResourceNodeVirtuals {
    u8 mOtherVirtuals[0x14];
    be<u32> mRelease;
};
WWHD_OFFSET(ResourceNodeVirtuals, mRelease, 0x14);
WWHD_SIZE(ResourceNodeVirtuals, 0x18);
struct ResourceNode {
    gptr<ResourceNode> mLeft;
    gptr<ResourceNode> mRight;
    be<u8> mRed;
    u8 mPadding[3];
    be<u32> mKey;
    gptr<ResourceNodeVirtuals> __vtbl;
    be<u32> mHash;
    gptr<void> mValue;
    gptr<ResourcePool> mpPool;
};
WWHD_OFFSET(ResourceNode, mRed, 8);
WWHD_OFFSET(ResourceNode, mKey, 0xC);
WWHD_OFFSET(ResourceNode, __vtbl, 0x10);
WWHD_OFFSET(ResourceNode, mHash, 0x14);
WWHD_OFFSET(ResourceNode, mValue, 0x18);
WWHD_OFFSET(ResourceNode, mpPool, 0x1C);
WWHD_SIZE(ResourceNode, 0x20);
struct ResourcePool {
    gptr<ResourceNode> mRoot;
    gptr<ResourceNode> mFree;
    gptr<ResourceNode> mStorage;
    be<u32> mCount;
    be<u32> mCapacity;
};
WWHD_OFFSET(ResourcePool, mCount, 0xC);
WWHD_OFFSET(ResourcePool, mCapacity, 0x10);
WWHD_SIZE(ResourcePool, 0x14);
struct ResourceCallback {
    be<u32> __vtbl;
    gptr<void> mpReceiver;
    be<u32> mMethodWords[2];
};
WWHD_OFFSET(ResourceCallback, mMethodWords, 8);
WWHD_SIZE(ResourceCallback, 0x10);
struct ResourceCallbackReceiver {
    gptr<void> mpObject;
};
WWHD_SIZE(ResourceCallbackReceiver, 4);
struct BufferedString {
    be<u32> mStringTop;
    be<u32> __vtbl;
    be<u32> mCapacity;
};
WWHD_SIZE(BufferedString, 0xC);
} // namespace resorce_hd

void resourceDisposer_destroy(void* self, u32 flags);
void resourceArchiveTransfer_apply(void* first, const be<u32>* key, gptr<void>* second);
void resourceTextureTransfer_apply(void* first, const be<u32>* key, gptr<void>* second);

namespace resorce_hd {
enum { ARCHIVE_CALLBACK_VTABLE = 0x100E1AB4, TEXTURE_CALLBACK_VTABLE = 0x100E1AC4 };
}

resorce_hd::ResourceNode* resourceInsertNode(resorce_hd::ResourcePool*, resorce_hd::ResourceNode*,
                                             resorce_hd::ResourceNode*);
resorce_hd::ResourceNode* resourceDeleteMinimum(resorce_hd::ResourceNode*);
resorce_hd::ResourceNode* resourceDeleteNode(resorce_hd::ResourcePool*, resorce_hd::ResourceNode*,
                                             const be<u32>*);
resorce_hd::ResourceNode* resourceFindNode(resorce_hd::ResourcePool*, resorce_hd::ResourceNode*,
                                           const be<u32>*);

namespace resorce_hd {
// HD: eight-byte GHS member pointer, direct target or signed virtual-table displacement.
struct ResourceMethod {
    be<s16> mThisAdjustment;
    be<s16> mSelector;
    union {
        be<u32> mTarget;
        struct {
            be<s16> mReserved;
            be<s16> mVtableOffset;
        } mVirtual;
    };
};
WWHD_OFFSET(ResourceMethod, mSelector, 2);
WWHD_OFFSET(ResourceMethod, mTarget, 4);
WWHD_SIZE(ResourceMethod, 8);
struct ResourceMethodVirtualEntry {
    be<u32> mAdjustment;
    be<u32> mTarget;
};
WWHD_OFFSET(ResourceMethodVirtualEntry, mTarget, 4);
WWHD_SIZE(ResourceMethodVirtualEntry, 8);
} // namespace resorce_hd
void resourceVisitArchive(resorce_hd::ResourceNode*, resorce_hd::ResourceCallback*);
void resourceVisitTexture(resorce_hd::ResourceNode*, resorce_hd::ResourceCallback*);

namespace resorce_hd {
enum {
    BUFFERED_STRING_VTABLE = 0x100E19AC,
    FIXED_STRING_VTABLE = 0x100E19E4,
    FORMAT_STRING_VTABLE = 0x100E1A44
};
struct FixedString64 {
    be<u32> mStringTop, __vtbl, mCapacity;
    be<u8> mBuffer[64];
};
WWHD_OFFSET(FixedString64, mBuffer, 0xC);
WWHD_SIZE(FixedString64, 0x4C);
struct FormatArguments {
    be<u8> mGpr, mFpr;
    u8 mPadding[2];
    be<u32> mOverflow, mSave;
};
WWHD_OFFSET(FormatArguments, mOverflow, 4);
WWHD_SIZE(FormatArguments, 0xC);
struct SavedFloat {
    be<u32> mHigh, mLow;
};
WWHD_SIZE(SavedFloat, 8);
// HD: original 0x90-byte variadic frame, including the eight-byte call linkage.
struct FormatFrame {
    u8 mLinkage[0x10];
    FormatArguments mArguments;
    u8 mPadding[4];
    be<u32> mGprSave[8];
    SavedFloat mFprSave[8];
    u8 mTail[0x10];
};
WWHD_OFFSET(FormatFrame, mArguments, 0x10);
WWHD_OFFSET(FormatFrame, mGprSave, 0x20);
WWHD_OFFSET(FormatFrame, mFprSave, 0x40);
WWHD_SIZE(FormatFrame, 0x90);
} // namespace resorce_hd
void resourceString_format(resorce_hd::FixedString64*, const char*, resorce_hd::FormatArguments*);

namespace resorce_hd {
// HD: translation-unit math/random initialization records, with original initialization order.
struct HeaderStatics {
    be<f32> mMinimum, mMaximum;
    be<u8> mMath, mRandom;
    u8 mPadding[2];
    be<u32> mZero[4];
};
WWHD_OFFSET(HeaderStatics, mMath, 8);
WWHD_OFFSET(HeaderStatics, mZero, 0xC);
WWHD_SIZE(HeaderStatics, 0x1C);
struct GlobalDestructor {
    be<u32> mNext, mDestructor, mObject;
};
WWHD_SIZE(GlobalDestructor, 0xC);
} // namespace resorce_hd
void resourceMath_init(be<u8>* state);
void resourceRandom_init(be<u8>* state);

namespace resorce_hd {
struct DestructorRecords {
    GlobalDestructor mEntries[3];
};
WWHD_SIZE(DestructorRecords, 0x24);
struct ResourceBounds {
    be<f32> mValues[2];
};
WWHD_SIZE(ResourceBounds, 8);
} // namespace resorce_hd

namespace resorce_hd {
// HD: both control maps keep 128 intrusive pool nodes inline instead of GC info arrays.
struct ControlResourcePoolHD {
    ResourcePool mPool;
    ResourceNode mNodes[128];
};
WWHD_OFFSET(ControlResourcePoolHD, mNodes, 0x14);
WWHD_SIZE(ControlResourcePoolHD, 0x1014);
struct ResourceControlHD {
    void destroy(u32 flags);
    u32 getRes(u32 name, u32 index);
    u32 getIDRes(u32 name, u32 index);
    u32 getPathArchive(u32 name);
    u32 getArchive(u32 name);
    u32 getResByName(u32 index, u32 name);
    u32 getObjectResName2Index(u32 archiveName, u32 name);
    void deleteRes(u32 name);
    void createStageHeaps();
    void createObjectHeaps();
    void destroyObjectHeaps();
    void destroyStageHeaps();
    u32 selectStageHeap(u32 name, u32 path, u32 heap);
    u32 needsSeparateHeap(u32 name, u32 path);
    u32 selectStageArchiveHeap(u32 name);
    u32 setRes(u32 archiveName, u32 path, u32 parent);
    u32 setArchive(u32 name, u32 parent, u32 arg6);
    u32 getPathRes(u32 archiveName, u32 path, u32 resourceName);
    u32 selectObjectHeap(u32 name, u32 path, u32 heap);
    u32 selectObjectArchiveHeap(u32 name);
    void deletePathRes(u32 name, u32 path);
    be<u32> __vtbl;
    u8 mDisposer[0x10];
    ControlResourcePoolHD mArchiveInfo;
    ControlResourcePoolHD mPathInfo;
    gptr<void> mpResourceHeap;
    gptr<void> mpObjectParentHeap;
    gptr<void> mpStageParentHeap;
    gptr<void> mpObjectHeap;
    gptr<void> mpStageHeap;
    gptr<void> mpObjectSpecialHeap;
    gptr<void> mpStageSpecialHeap;
    gptr<void> mObjectGroupHeaps[4];
    gptr<void> mStageGroupHeaps[4];
    gptr<void> mObjectArchiveHeaps[22];
    gptr<void> mStageArchiveHeaps[22];
    be<u32> mPendingCount;
    be<u8> mLoading;
    u8 mPadding212D[3];
};
WWHD_OFFSET(ResourceControlHD, mArchiveInfo, 0x14);
WWHD_OFFSET(ResourceControlHD, mPathInfo, 0x1028);
WWHD_OFFSET(ResourceControlHD, mpResourceHeap, 0x203C);
WWHD_OFFSET(ResourceControlHD, mpObjectParentHeap, 0x2040);
WWHD_OFFSET(ResourceControlHD, mpStageParentHeap, 0x2044);
WWHD_OFFSET(ResourceControlHD, mpObjectHeap, 0x2048);
WWHD_OFFSET(ResourceControlHD, mpStageHeap, 0x204C);
WWHD_OFFSET(ResourceControlHD, mObjectGroupHeaps, 0x2058);
WWHD_OFFSET(ResourceControlHD, mStageGroupHeaps, 0x2068);
WWHD_OFFSET(ResourceControlHD, mObjectArchiveHeaps, 0x2078);
WWHD_OFFSET(ResourceControlHD, mStageArchiveHeaps, 0x20D0);
WWHD_OFFSET(ResourceControlHD, mLoading, 0x212C);
WWHD_SIZE(ResourceControlHD, 0x2130);
// HD: archive info retains tww's mArchiveName, mpArchive, mpParentHeap and mRes names.
struct ResourceArchiveInfoHD {
    void loadRawResource(u32 index, u32 id, u32 name);
    void loadCollision(u32 index, u32 id, u32 name);
    void loadDemoScript(u32 index, u32 id, u32 name);
    void loadData(u32 index, u32 id, u32 name);
    void loadClothVertices(u32 index, u32 id, u32 name);
    void loadTextureTransformAnimation(u32 index, u32 id, u32 name, u32 heap);
    void loadTevRegAnimation(u32 index, u32 id, u32 name, u32 heap);
    void loadTexture(u32 index, u32 id, u32 name, u32 heap);
    void loadModel(u32 index, u32 id, u32 name, u32 heap);
    void loadSkeletalAnimation(u32 index, u32 id, u32 name, u32 heap);
    void loadTexturePatternAnimation(u32 index, u32 id, u32 name, u32 heap);
    void loadMaterialModel(u32 index, u32 id, u32 name, u32 heap);
    void loadExtraResources(u32 heap);
    void loadResourceTable(u32 name, u32 heap);
    u32 getResByFilename(u32 name);
    u32 getRes(u32 index);
    u32 getIDRes(u32 id);
    void setRes(s32 index, u32 value);
    void setIDRes(s32 index, u32 value);
    void allocateRes(s32 count, u32 heap);
    void allocateIDRes(s32 count, u32 heap);
    u8 mDisposer[0x10];
    FixedString64 mArchiveName;
    gptr<void> mpArchive;
    gptr<gptr<void>> mRes;
    gptr<gptr<void>> mIdRes;
    ResourcePool mExtraResources;
    gptr<void> mpParentHeap;
    gptr<void> mDataHeap;
    be<s32> mResCount;
    be<s32> mIdResCount;
};
WWHD_OFFSET(ResourceArchiveInfoHD, mArchiveName, 0x10);
WWHD_OFFSET(ResourceArchiveInfoHD, mpArchive, 0x5C);
WWHD_OFFSET(ResourceArchiveInfoHD, mRes, 0x60);
WWHD_OFFSET(ResourceArchiveInfoHD, mIdRes, 0x64);
WWHD_OFFSET(ResourceArchiveInfoHD, mExtraResources, 0x68);
WWHD_OFFSET(ResourceArchiveInfoHD, mpParentHeap, 0x7C);
WWHD_OFFSET(ResourceArchiveInfoHD, mDataHeap, 0x80);
WWHD_OFFSET(ResourceArchiveInfoHD, mResCount, 0x84);
WWHD_SIZE(ResourceArchiveInfoHD, 0x8C);
struct ResourcePathInfoHD {
    void loadResource();
    void allocatePool(s32 count, u32 heap);
    void setRes(u32 name, u32 resource);
    u32 getRes(u32 name);
    u8 mDisposer[0x10];
    FixedString64 mArchiveName;
    FixedString64 mArchivePath;
    gptr<void> mpArchive;
    ResourcePool mResources;
    gptr<void> mpParentHeap;
    gptr<void> mDataHeap;
};
WWHD_OFFSET(ResourcePathInfoHD, mArchivePath, 0x5C);
WWHD_OFFSET(ResourcePathInfoHD, mpArchive, 0xA8);
WWHD_OFFSET(ResourcePathInfoHD, mResources, 0xAC);
WWHD_OFFSET(ResourcePathInfoHD, mpParentHeap, 0xC0);
WWHD_OFFSET(ResourcePathInfoHD, mDataHeap, 0xC4);
WWHD_SIZE(ResourcePathInfoHD, 0xC8);
} // namespace resorce_hd

// HD: resource metadata uses both scalar and array heap-allocation entry points.
u32 resourceAllocate(u32 size);
u32 resourceAllocateArray(u32 size, u32 heap, u32 alignment);
u32 resourceAllocateOnHeap(u32 size, u32 heap, u32 alignment);
void resourceFree(u32 memory);

// HD: calls between verified runtime entries retain their original dispatch boundaries.
void resourceCallControlDestructor(u32 self, u32 flags);
u32 resourceCallGetIndex(u32 info, u32 index);
u32 resourceCallGetId(u32 info, u32 id);
void resourceCallSetResourceIndex(u32 info, s32 index, u32 value);
void resourceCallSetResourceId(u32 info, s32 index, u32 value);
u32 resourceCallControlCtor(u32 self);
void resourceCallAllocatePool(u32 self, s32 count, u32 heap);
u32 resourceCallGetPathResource(u32 self, u32 name);
u32 resourceCallResourceByFilename(u32 info, u32 name);
void resourceCallAllocateResourceIndices(u32 info, s32 count, u32 heap);
void resourceCallAllocateResourceIds(u32 info, s32 count, u32 heap);
void resourceCallCreateStageHeaps(u32 self);
void resourceCallCreateObjectHeaps(u32 self);
void resourceCallSetResource(u32 info, u32 name, u32 resource);
void resourceCallDestroyObjectHeaps(u32 self);
void resourceCallDestroyStageHeaps(u32 self);
u32 resourceCallInfoCtor(u32 self, u32 archiveName, u32 path, u32 unused, u32 parent, u32 heap);
u32 resourceCallArchiveInfoCtor(u32 self, u32 name, u32 archive, u32 parent, u32 heap);
u32 resourceCallSelectHeap(u32 control, u32 name, u32 path, u32 heap);
u32 resourceCallNeedsSeparateHeap(u32 control, u32 name, u32 path, u32 heap);
u32 resourceCallChooseSpecialHeap(u32 control, u32 name);
u32 resourceCallChooseObjectHeap(u32 control, u32 name);
void resourceCallLoadTableResources(u32 info, u32 name, u32 heap);
void resourceCallLoadExtraResources(u32 info, u32 heap);
void resourceCallLoadAllResources(u32 info);

namespace resorce_hd {
// HD: resource archives point to a relative-offset header and a SafeString name.
struct ResourceArchiveHD {
    u8 mArchiveState[0x14];
    gptr<void> mpHeader;
    u8 mArchiveState18[8];
    gptr<void> mpArchiveName;
};
WWHD_OFFSET(ResourceArchiveHD, mpHeader, 0x14);
WWHD_OFFSET(ResourceArchiveHD, mpArchiveName, 0x20);
WWHD_SIZE(ResourceArchiveHD, 0x24);
struct ResourceRelativeOffsetHD {
    be<u32> mOffset;
    u32 getAddress() const {
        u32 offset = mOffset;
        return offset ? gabi::ea(this) + offset : 0;
    }
};
WWHD_SIZE(ResourceRelativeOffsetHD, 4);
struct ResourceHeaderHD {
    u8 mHeader[0x20];
    ResourceRelativeOffsetHD mModels;
    ResourceRelativeOffsetHD mTextures;
    ResourceRelativeOffsetHD mSkeletalAnimations;
    be<u32> mUnknown2C;
    ResourceRelativeOffsetHD mTevColors;
    ResourceRelativeOffsetHD mTextureTransforms;
    ResourceRelativeOffsetHD mTexturePatterns;
    u8 mUnknown3C[0x10];
    ResourceRelativeOffsetHD mRawResources;
    be<u16> mModelCount;
    be<u16> mTextureCount;
    be<u16> mSkeletalAnimationCount;
    be<u16> mUnknown56;
    be<u16> mTevColorCount;
    be<u16> mTextureTransformCount;
    be<u16> mTexturePatternCount;
    u8 mUnknown5E[8];
    be<u16> mRawResourceCount;
};
WWHD_OFFSET(ResourceHeaderHD, mModels, 0x20);
WWHD_OFFSET(ResourceHeaderHD, mRawResources, 0x4C);
WWHD_OFFSET(ResourceHeaderHD, mModelCount, 0x50);
WWHD_OFFSET(ResourceHeaderHD, mRawResourceCount, 0x66);
WWHD_SIZE(ResourceHeaderHD, 0x68);
} // namespace resorce_hd

namespace resorce_hd {
struct ResourceHeapVirtualsHD {
    u8 mOtherMethods[0x24];
    gfn mDestroy;
    u8 mOtherMethods28[4];
    gfn mAdjust;
    u8 mOtherMethods30[0x44];
    gfn mGetFreeSize;
};
WWHD_OFFSET(ResourceHeapVirtualsHD, mDestroy, 0x24);
WWHD_OFFSET(ResourceHeapVirtualsHD, mAdjust, 0x2C);
WWHD_OFFSET(ResourceHeapVirtualsHD, mGetFreeSize, 0x74);
WWHD_SIZE(ResourceHeapVirtualsHD, 0x78);
struct ResourceHeapHD {
    u8 mDisposer[0xC];
    gptr<ResourceHeapVirtualsHD> __vtbl;
    gptr<void> mpName;
};
WWHD_OFFSET(ResourceHeapHD, __vtbl, 0xC);
WWHD_OFFSET(ResourceHeapHD, mpName, 0x10);
WWHD_SIZE(ResourceHeapHD, 0x14);
} // namespace resorce_hd

namespace resorce_hd {
// HD: hashed archive rows select name/type tables and a separate resource-ID table.
struct ResourceTableEntryHD {
    gptr<const char> mpName;
    be<u32> mType;
};
WWHD_OFFSET(ResourceTableEntryHD, mType, 4);
WWHD_SIZE(ResourceTableEntryHD, 8);
struct ResourceTableRowHD {
    be<u32> mHash;
    gptr<ResourceTableEntryHD> mValues;
    be<u32> mCount;
};
WWHD_OFFSET(ResourceTableRowHD, mValues, 4);
WWHD_OFFSET(ResourceTableRowHD, mCount, 8);
WWHD_SIZE(ResourceTableRowHD, 12);
} // namespace resorce_hd

namespace resorce_hd {
// HD: BFRES animation wrappers retain guest resource pointers and frame state.
struct ResourceFrameAnimationHD {
    be<f32> mFrame;
    be<u32> __vtbl;
    be<u32> mFlags;
    gptr<void> mpResource;
};
WWHD_OFFSET(ResourceFrameAnimationHD, mpResource, 0xC);
WWHD_SIZE(ResourceFrameAnimationHD, 0x10);
struct ResourceTextureHD {
    u8 mState[2];
    be<u16> mWidth;
    be<u16> mHeight;
    u8 mState6[2];
    be<u8> mFlags;
    u8 mState9[0x17];
    gptr<void> mpResource;
};
WWHD_OFFSET(ResourceTextureHD, mWidth, 2);
WWHD_OFFSET(ResourceTextureHD, mHeight, 4);
WWHD_OFFSET(ResourceTextureHD, mFlags, 8);
WWHD_OFFSET(ResourceTextureHD, mpResource, 0x20);
WWHD_SIZE(ResourceTextureHD, 0x24);
struct ResourceTextureHeaderHD {
    u8 mHeader[8];
    be<u32> mWidth;
    be<u32> mHeight;
};
WWHD_OFFSET(ResourceTextureHeaderHD, mWidth, 8);
WWHD_OFFSET(ResourceTextureHeaderHD, mHeight, 0xC);
WWHD_SIZE(ResourceTextureHeaderHD, 0x10);
} // namespace resorce_hd

namespace resorce_hd {
struct ResourceSafeStringHD {
    be<u32> mStringTop;
    be<u32> __vtbl;
};
WWHD_SIZE(ResourceSafeStringHD, 8);
struct ResourceStringBuffer128HD {
    be<u32> mStringTop;
    be<u32> __vtbl;
    be<u32> mCapacity;
    be<u8> mBuffer[128];
};
WWHD_OFFSET(ResourceStringBuffer128HD, mBuffer, 0xC);
WWHD_SIZE(ResourceStringBuffer128HD, 0x8C);
} // namespace resorce_hd

u32 resourceHashString(u32 text, u32 length);
s32 resourceFindName(u32 entries, u32 name);
u32 resourceArchiveGetId(u32 archive, u32 index);
u32 resourceCreateModel(u32 resource, u32 id, resorce_hd::ResourceSafeStringHD *name, u32 heap);
u32 resourceCreateSkeletalAnimation(u32 resource, resorce_hd::ResourceSafeStringHD *name, u32 heap);
u32 resourceCreateTexturePatternAnimation(u32 resource, u32 heap);
u32 resourceCreateNamedModel(u32 resource, u32 id, u32 label, u32 heap);
void resourceDisposerConstruct(u32 object);
void resourceDisposerConstructOnHeap(u32 object, u32 heap, u32 flags);
void resourceTransferArchiveData(u32 destination, u32 source, u32 mode);
void resourceCopyBytes(u32 destination, u32 source, u32 size, u32 flags);

u32 resourceCreateExpHeap(u32 size, resorce_hd::ResourceSafeStringHD *name, u32 parent,
                          u32 alignment, u32 flags);
u32 resourceCreateSolidHeap(u32 size, resorce_hd::ResourceSafeStringHD *name, u32 parent,
                            u32 alignment, u32 flags);
u32 resourceSwitchCurrentHeap(u32 manager, u32 heap);
u32 resourceConstructArchive(u32 archive);
u32 resourceLoadArchiveFile(u32 manager, u32 name);
void resourceMountPathArchive(u32 archive, u32 data, u32 name, u32 heap);
void resourceMountArchive(u32 archive, u32 data, u32 name, u32 heap);
void resourceReleaseCachedArchive(u32 manager, u32 hash);
void resourceAssureSafeString(u32 string);
void resourceTerminateString(u32 string);
void resourceConvertCollision(u32 resource);
void resourceConstructArchiveCallback(be<u32> *callback, u32 receiver, be<u32> *method);
void resourceConstructTextureCallback(be<u32> *callback, u32 receiver, be<u32> *method);

void resourceFormatPath(u32 string, const char *format, u32 path, u32 archiveName);
void resourceFormatName(u32 string, const char *format, u32 name);
u32 resourceFormatLabel(u8 *string, const char *format, u32 archiveName, u32 path);

namespace resorce_hd {
struct ResourceSafeStringVirtualsHD {
    u8 mOtherMethods[0x14];
    gfn mAssureTermination;
};
WWHD_OFFSET(ResourceSafeStringVirtualsHD, mAssureTermination, 0x14);
WWHD_SIZE(ResourceSafeStringVirtualsHD, 0x18);
struct ResourceDisposerHD {
    u8 mState[0xC];
    be<u32> __vtbl;
};
WWHD_OFFSET(ResourceDisposerHD, __vtbl, 0xC);
WWHD_SIZE(ResourceDisposerHD, 0x10);
} // namespace resorce_hd

namespace resorce_hd {
struct ResourceControlGlobalsHD {
    gptr<ResourceControlHD> mpControl;
    gptr<void> mpDisposer;
};
WWHD_OFFSET(ResourceControlGlobalsHD, mpDisposer, 4);
WWHD_SIZE(ResourceControlGlobalsHD, 8);
struct ResourceObjectSlotHD {
    gptr<void> mpObject;
};
WWHD_SIZE(ResourceObjectSlotHD, 4);
} // namespace resorce_hd

namespace resorce_hd {
enum ResourceTransferMethodHD {
    DIRECT_METHOD = 0xFFFF,
    ARCHIVE_TRANSFER_METHOD = 0x0260AC60,
    TEXTURE_TRANSFER_METHOD = 0x0260AC94
};
struct ResourceMethodWordsHD {
    be<u32> mWords[2];
};
WWHD_SIZE(ResourceMethodWordsHD, 8);
struct ResourceTransferMethodsHD {
    be<u32> mArchive[2];
    be<u32> mTexture[2];
};
WWHD_SIZE(ResourceTransferMethodsHD, 0x10);
struct ResourceTransferReceiverHD {
    gptr<ResourceArchiveHD> mpArchive;
    u8 mState[8];
    gptr<ResourceTransferReceiverHD> mpSelf;
};
WWHD_OFFSET(ResourceTransferReceiverHD, mpSelf, 0xC);
WWHD_SIZE(ResourceTransferReceiverHD, 0x10);
} // namespace resorce_hd

namespace resorce_hd {
// HD: each dictionary entry view keeps the prefix while the cursor advances by one 16-byte node.
struct ResourceDictionaryEntryViewHD {
    u8 mDictionaryPrefix[0x20];
    ResourceRelativeOffsetHD mName;
    ResourceRelativeOffsetHD mResource;
};
WWHD_OFFSET(ResourceDictionaryEntryViewHD, mName, 0x20);
WWHD_OFFSET(ResourceDictionaryEntryViewHD, mResource, 0x24);
WWHD_SIZE(ResourceDictionaryEntryViewHD, 0x28);
struct ResourceModelHeaderHD {
    be<u32> mSignature;
    ResourceRelativeOffsetHD mName;
};
WWHD_OFFSET(ResourceModelHeaderHD, mName, 4);
WWHD_SIZE(ResourceModelHeaderHD, 8);
} // namespace resorce_hd

namespace resorce_hd {
enum ResourceRuntimeVtableHD {
    CONTROL_VTABLE = 0x100E1A5C,
    CONTROL_DISPOSER_VTABLE = 0x100E1DA0,
    ARCHIVE_DISPOSER_VTABLE = 0x100E1DB0,
    PATH_DISPOSER_VTABLE = 0x100E1DC0,
    SAFE_STRING_VTABLE = 0x100E1994,
    RESOURCE_NAME_VTABLE = 0x100E19FC,
    RAW_RESOURCE_STRING_VTABLE = 0x100E1A14,
    RESOURCE_STRING_VTABLE = 0x100E1A2C,
    ARCHIVE_NODE_VTABLE = 0x100E1A6C,
    PATH_NODE_VTABLE = 0x100E1A84,
    VALUE_NODE_VTABLE = 0x100E1A9C,
    TEXTURE_TRANSFORM_VTABLE = 0x1016E4AC,
    TEV_REG_ANIMATION_VTABLE = 0x1016E4CC
};
} // namespace resorce_hd
