#pragma once
#include "wwhd.h"

// HD: these sead resource-manager views follow the verified Wii U layouts.
namespace ResourceManagerHD {
struct SafeString {
    be<u32> mTop, mVtable;
};
struct SafeStringVtable {
    u8 mPrefix[0x14];
    be<u32> assureTermination;
};
template <u32 N> struct FixedString {
    SafeString mString;
    be<s32> mCapacity;
    u8 mBuffer[N];
};
struct TreeHeader {
    be<u32> mRoot, mFreeHead, mNodes;
    be<s32> mCount, mCapacity;
};
struct ArchiveNode {
    be<u32> mLeft, mRight;
    be<u8> mColor;
    u8 mPadding[3];
    SafeString mKey;
    be<u32> mValueVtable, mHolder, mTree;
    u8 mBuffer[0x30];
    be<u8> mBufferFlag;
    u8 mTail[3];
};
struct PackNode {
    be<u32> mLeft, mRight;
    be<u8> mColor;
    u8 mPadding[3];
    be<u32> mHash, mValueVtable, mHashCopy, mHolder, mTree;
};
struct ArchiveTree {
    TreeHeader mHeader;
    ArchiveNode mNodes[0x280];
};
struct PackTree {
    TreeHeader mHeader;
    PackNode mNodes[0x200];
};
struct Manager {
    be<u32> mVtable;
    u8 mDisposer[0x10];
    ArchiveTree mArchives;
    PackTree mPacks;
    be<u32> mArena, mLoaderThread, mPackThread, mRequestState;
    FixedString<0x30> mRequestName;
    FixedString<0x40> mRequestPath;
    be<u32> mRequestHeap;
    be<u8> mFromPack;
    u8 mPadding[3];
    u8 mCriticalSection[0x3C];
    be<u32> mLoadAlignment;
};
WWHD_SIZE(ArchiveNode, 0x54);
WWHD_SIZE(PackNode, 0x20);
WWHD_SIZE(ArchiveTree, 0xD214);
WWHD_SIZE(PackTree, 0x4014);
WWHD_SIZE(Manager, 0x1131C);
WWHD_OFFSET(Manager, mPacks, 0xD228);
WWHD_OFFSET(Manager, mRequestName, 0x1124C);
WWHD_OFFSET(Manager, mCriticalSection, 0x112DC);
struct Holder {
    u8 mListNode[0xC];
    be<u32> mVtable;
    FixedString<0x30> mName;
    be<u32> mResource, mHeap;
};
WWHD_SIZE(Holder, 0x54);
struct HolderVtable {
    u8 mPrefix[0xC];
    be<u32> destroy;
};
struct ArchiveResource {
    u8 mPrefix[0x10];
    be<u32> mVtable, mBuffer, mSize;
};
struct ResourceVtable {
    u8 mPrefix[0xC];
    be<u32> isDerived;
    u8 mGap[0xC];
    be<u32> destroy;
    u8 mTail[0x1C];
    be<u32> getFile;
};
WWHD_OFFSET(ResourceVtable, getFile, 0x3C);
struct Heap {
    u8 mPrefix[0xC];
    be<u32> mVtable;
};
struct HeapVtable {
    u8 mPrefix[0x24];
    be<u32> destroy, mReserved, adjust;
    be<u32> mGap, allocate;
};
struct Thread {
    u8 mPrefix[0xC];
    be<u32> mVtable;
};
struct ThreadVtable {
    u8 mPrefix[0x1C];
    be<u32> sendMessage;
    u8 mGap[0xC];
    be<u32> start;
};
struct ThreadDelegate {
    be<u32> mVtable, mManager;
    be<u16> mOffset, mVirtualIndex;
    be<u32> mFunction;
};
struct ThreadArguments {
    be<u32> mCoreMask[2];
    SafeString mName;
};
struct FileInfo {
    be<u32> mStart, mSize;
};
struct LoadArg {
    SafeString mPath;
    be<u32> mHeap, mInstanceHeap, mAlignment, mInstanceAlignment;
    be<u32> mFlags, mReserved, mResourceList, mFileDevice, mBufferAlignment;
};
WWHD_SIZE(LoadArg, 0x2C);
struct ResourceList {
    be<u32> mNext, mPrevious, mHead, mCount;
    u8 mDisposer[0xC];
    be<u32> mVtable;
    FixedString<0x20> mName;
    be<u32> mDeviceVtable;
};
WWHD_SIZE(ResourceList, 0x50);
struct ArchiveFileDevice {
    be<u32> mNext, mPrevious, mHead, mCount;
    u8 mDisposer[0xC];
    be<u32> mVtable;
    FixedString<0x20> mName;
    be<u8> mEnabled;
    u8 mPadding[3];
    be<u32> mDeviceVtable, mArchive;
};
WWHD_SIZE(ArchiveFileDevice, 0x58);
struct DirectoryHandle {
    u8 mDisposer[0xC];
    be<u32> mVtable, mIsOpen, mDevice;
    u8 mTail[0x20];
};
WWHD_SIZE(DirectoryHandle, 0x38);
struct DirectoryEntry {
    FixedString<0x100> mName;
    be<u8> mIsDirectory;
    u8 mTail[3];
};
WWHD_SIZE(DirectoryEntry, 0x110);
struct SleepSpan {
    be<u32> mHigh, mLow, mDuplicateHigh, mDuplicateLow;
};
struct FileFrame {
    u8 mPrefix[8];
    LoadArg mLoad;
    ResourceList mList;
    FixedString<0x104> mPath;
};
struct CompressedFrame {
    u8 mPrefix[8];
    SafeString mFactory;
    LoadArg mLoad;
    ResourceList mList;
    FixedString<0x104> mPath;
    u8 mTail[4];
};
struct StageFrame {
    u8 mPrefix[0xC];
    LoadArg mLoad;
    ResourceList mList;
    FixedString<0x104> mPath;
    u8 mTail[8];
};
WWHD_SIZE(FileFrame, 0x194);
WWHD_SIZE(CompressedFrame, 0x1A0);
WWHD_SIZE(StageFrame, 0x1A0);
struct PackFrame {
    u8 mPrefix[8];
    be<u32> mHandleVtable, mListVtable, mDeviceVtable;
    SafeString mDot, mSubstring, mExtension;
    SafeString mFactory;
    u8 mGap[4];
    SleepSpan mSleep;
    SafeString mHeapName;
    FixedString<0x40> mEntryKey;
    SafeString mDeviceName;
    LoadArg mLoad;
    DirectoryEntry mEntry;
    DirectoryHandle mHandle;
    ArchiveFileDevice mDevice;
    ResourceList mList;
};
WWHD_SIZE(PackFrame, 0x2C0);
WWHD_OFFSET(PackFrame, mEntry, 0xD0);
WWHD_OFFSET(PackFrame, mDevice, 0x218);
struct SzsPackFrame {
    u8 mPrefix[8];
    be<u32> mListVtable, mDeviceVtable, mHandleVtable;
    SafeString mDot, mSubstring, mExtension;
    SafeString mHeapName;
    be<u32> mHash;
    SleepSpan mRetrySleep;
    u8 mGap[8];
    SleepSpan mSleep;
    FixedString<0x40> mEntryKey;
    SafeString mPath;
    LoadArg mLoad;
    DirectoryEntry mEntry;
    DirectoryHandle mHandle;
    ArchiveFileDevice mDevice;
    ResourceList mList;
};
WWHD_SIZE(SzsPackFrame, 0x2D0);
WWHD_OFFSET(SzsPackFrame, mEntry, 0xE0);
WWHD_OFFSET(SzsPackFrame, mDevice, 0x228);
struct Config {
    u8 mPrefix[0x10];
    be<u32> mRegion, mLanguage;
};
struct GameHeaps {
    u8 mPrefix[0x2040];
    be<u32> mPackHeap;
    u8 mGap[0xE4];
    be<u32> mPermanentPackHeap;
};
WWHD_OFFSET(GameHeaps, mPermanentPackHeap, 0x2128);
struct InitLists {
    be<u32> mFirst, mSecond, mThird, mFourth;
};
struct InitAngles {
    be<f32> mMinimum, mMaximum;
    u8 mFirstObject, mSecondObject;
};
struct InitializerRecord {
    u8 mStorage[0xC];
};
} // namespace ResourceManagerHD
namespace ResourceManagerHD {
enum ImageAddress : u32 {
    kSafeStringVt = 0x100E20A4,
    kResMgr = 0x101F4F7C,
};
struct Request {
    FixedString<0x30> mName;
    FixedString<0x40> mPath;
    be<u32> mHeap;
    be<u8> mFromPack;
    u8 mPadding[3];
};
struct Disposer {
    u8 mPrefix[0xC];
    be<u32> mVtable;
};
struct HashKeys {
    be<u32> mFirst, mSecond;
};
struct StringScratch {
    u8 mPrefix[0x14];
    SafeString mDot, mSubstring, mExtension;
};
} // namespace ResourceManagerHD
namespace ResourceManagerHD {
template <class T, class Field> inline T readField(Field& field) {
    return reinterpret_cast<be<T>*>(&field)->get();
}
template <class T, class Field> inline void writeField(Field& field, T value) {
    *reinterpret_cast<be<T>*>(&field) = value;
}
inline be<u32>& singletonSlot() { return *gabi::at<be<u32>>(kResMgr); }
inline SafeStringVtable* stringVtable(u32 s) {
    return gabi::at<SafeStringVtable>(gabi::at<SafeString>(s)->mVtable.get());
}
inline HeapVtable* heapVtable(u32 h) {
    return gabi::at<HeapVtable>(gabi::at<Heap>(h)->mVtable.get());
}
inline ThreadVtable* threadVtable(u32 t) {
    return gabi::at<ThreadVtable>(gabi::at<Thread>(t)->mVtable.get());
}
inline HolderVtable* holderVtable(u32 h) {
    return gabi::at<HolderVtable>(gabi::at<Holder>(h)->mVtable.get());
}
inline ResourceVtable* resourceVtable(u32 r) {
    return gabi::at<ResourceVtable>(gabi::at<ArchiveResource>(r)->mVtable.get());
}
inline Manager* managerFromPackTree(u32 p) { return gabi::at<Manager>(p - 0xD228); }
} // namespace ResourceManagerHD
namespace ResourceManagerHD {
inline be<u8>& byteAt(u32 address) { return *gabi::at<be<u8>>(address); }
inline be<u16>& halfAt(u32 address) { return *gabi::at<be<u16>>(address); }
inline be<u32>& wordAt(u32 address) { return *gabi::at<be<u32>>(address); }
inline be<f32>& floatAt(u32 address) { return *gabi::at<be<f32>>(address); }
inline be<u32>& disposerSlot() { return wordAt(0x101F4F80); }
inline be<u32>& heapManagerSlot() { return wordAt(0x101F8B4C); }
inline be<u32>& configurationSlot() { return wordAt(0x101F4BAC); }
inline be<u32>& fileDeviceManagerSlot() { return wordAt(0x101F8B08); }
inline be<u32>& resourceManagerSlot() { return wordAt(0x101F8B68); }
inline be<u32>& threadPrioritySlot() { return wordAt(0x101F8B9C); }
inline GameHeaps* gameHeaps() { return gabi::at<GameHeaps>(wordAt(0x101F4F28).get()); }
} // namespace ResourceManagerHD

namespace ResourceManagerHD {
enum ResourceImageAddress : u32 {
    kEmptyText = 0x10000160,
    kFixedStringBaseVtable = 0x100E20BC,
    kFixedString64BaseVtable = 0x100E2104,
    kFixedString64Vtable = 0x100E211C,
    kDirectoryEntryStringVtable = 0x100E214C,
    kFixedString32Vtable = 0x100E217C,
    kDirectoryHandleVtable = 0x100E2194,
    kArchiveResourceTypeInfo = 0x100E21B4,
    kPathBufferVtable = 0x100E2214,
    kManagerVtable = 0x100E222C,
    kFixedString48BaseVtable = 0x100E223C,
    kFixedString48Vtable = 0x100E2254,
    kResourceListDeviceVtable = 0x100E226C,
    kResourceListVtable = 0x100E22AC,
    kThreadDelegateVtable = 0x100E22BC,
    kPackResourceListDeviceVtable = 0x100E22CC,
    kPackResourceListVtable = 0x100E230C,
    kArchiveMapValueVtable = 0x100E231C,
    kPackMapValueVtable = 0x100E2334,
    kArchiveLoaderThreadName = 0x100E2354,
    kPackLoaderThreadName = 0x100E2364,
    kObjectPathFormat = 0x100E237C,
    kArchiveDeviceName = 0x100E238C,
    kArchiveExtension = 0x100E2390,
    kArchiveDot = 0x100E2394,
    kArchiveRootPath = 0x100E2396,
    kPackArchiveFactory = 0x100E2398,
    kArchiveTemporaryHeapName = 0x100E23A0,
    kArchiveResourceHeapName = 0x100E23B0,
    kSzsDeviceName = 0x100E23BC,
    kSzsExtension = 0x100E23C0,
    kSzsDot = 0x100E23C4,
    kSzsRootPath = 0x100E23C6,
    kSzsPackHeapName = 0x100E23C8,
    kSzsResourceHeapName = 0x100E23DC,
    kStagePathFormat = 0x100E23EC,
    kStageRequestFormat = 0x100E23FC,
    kStagePackRequestFormat = 0x100E2404,
    kArchiveFactory = 0x100E2420,
    kFirstPermanentPackPath = 0x100E2428,
    kSecondPermanentPackPath = 0x100E2444,
    kThirdPermanentPackPath = 0x100E2460,
    kSingletonDisposerVtable = 0x100E25C0,
    kArchiveHolderVtable = 0x100E25D0,
    kPackHolderVtable = 0x100E25E0,
    kArchiveFileDeviceVtable = 0x101439A4,
    kArchiveFileDeviceDisposerVtable = 0x10143AA4,
    kFileDeviceVtable = 0x10143B78,
    kFileDeviceDisposerVtable = 0x10143C58,
    kResourceListBaseVtable = 0x1015E028,
    kArchiveTypeInfoGuard = 0x101FD6F4,
    kPackTypeInfoGuard = 0x101FD778,
    kArchiveTypeInfo = 0x101FD9D0,
    kPackTypeInfo = 0x101FDD64,
    kEmptyStringTopSlot = 0x104A0CD8,
};
inline SafeString* languagePackObjects() { return gabi::at<SafeString>(0x1048DD4C); }
inline be<u32>* languagePackTable() { return gabi::at<be<u32>>(0x100E2408); }
enum RequestState : u32 { RequestIdle = 0, RequestArchive = 1, RequestPermanentPacks = 2 };
enum LoaderMessage : u32 {
    LoadArchiveMessage = 1,
    LoadPermanentPacksMessage = 2,
    ScanStagePackMessage = 3
};
} // namespace ResourceManagerHD
