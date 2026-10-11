#pragma once
// HD: MSBP/MSBT resource and layout-tag resolution. No GameCube counterpart.
#include "wwhd.h"

struct MsgSafeString {
    be<u32> mpText;
    be<u32> __vtbl;
};
struct MsgBufferedString : MsgSafeString {
    be<s32> mCapacity;
};
template <u32 N> struct MsgFixedString : MsgBufferedString {
    be<u8> mBuffer[N];
};
struct MsgStringVtable {
    u8 pad0[0x14];
    be<u32> mpAssureTermination;
};
struct MsgResourceVtable {
    u8 pad0[0x3C];
    be<u32> mpGetFile;
};
struct MsgResource {
    u8 pad0[0x10];
    be<u32> __vtbl;
};
struct MsgPair {
    be<u32> mValue, mSize;
};
struct MsgProject {
    be<u32> mpData;
    MsgPair mSections[3];
    be<u32> mNameCount, __vtbl;
};
struct MsgResMgr {
    u8 mDisposer[0xC];
    be<u32> __vtbl;
    MsgProject mProject;
    be<u32> mSetCount;
    be<u32> mpSets[0x100];
};
struct MsgSet {
    be<u32> mpData, mEntryCount, __vtbl;
    MsgFixedString<256> mName;
};
struct MsgResult {
    u8 pad0[8];
    be<u32> mpSet;
    MsgFixedString<256> mSetName;
    be<u32> mLabelIndex;
    u8 pad11C[4];
    MsgFixedString<256> mLabel;
    be<u32> mpEntry;
};
struct MsgLayout {
    u8 pad0[8];
    be<u32> __vtbl, mpLayout;
    u8 pad10[0xC];
    be<u32> mpSetName;
    u8 pad20[0x14];
    be<u32> mpParent;
};
struct MsgLayoutVtable {
    u8 pad0[0x14];
    be<u32> mpGetRtti;
    u8 pad18[0x44];
    be<u32> mpFindChild;
};
struct MsgTagTarget {
    u8 pad0[0x128];
    MsgSafeString mLabel;
};
struct MsgTag {
    be<u32> mType, mValueOffset;
};
struct MsgAttribute {
    u8 pad0[0xC];
    be<s16> mId;
};
struct MsgConfig {
    u8 pad0[0x14];
    be<u32> mLanguage;
};
struct MsgWords4 {
    be<u32> mWords[4];
};
struct MsgLanguageNames {
    be<u32> mNames[6];
};
struct MsgLanguageMap {
    be<u8> mIndices[6];
};
WWHD_SIZE(MsgSafeString, 8);
WWHD_SIZE(MsgBufferedString, 0xC);
WWHD_SIZE(MsgFixedString<256>, 0x10C);
WWHD_SIZE(MsgProject, 0x24);
WWHD_SIZE(MsgResMgr, 0x438);
WWHD_SIZE(MsgSet, 0x118);
WWHD_OFFSET(MsgResult, mLabelIndex, 0x118);
WWHD_OFFSET(MsgResult, mLabel, 0x120);
WWHD_OFFSET(MsgResult, mpEntry, 0x22C);

u32 MsgFormatPath(u32, u32, u32, u32, u32, u32);
u32 MsgFindResource(u32, u32);
u32 MsgLoadResource(u32, u32, u32, u32, u32, u32, u32);
u32 MsgFindTextTag(u32, u32, u32, u32);
u32 MsgFindLabel(u32, u32);
u32 MsgGetAttribute(u32, u32);
u32 MsgGetProjectName(u32, u32);
u32 MsgAllocate(u32);
void MsgDeallocate(u32);
u32 MsgAllocateSet(u32, u32, u32);
u32 MsgAllocateManager(u32, u32, u32);
void MsgDisposerConstruct(u32, u32, u32);
void MsgDisposerDestroy(u32, u32);
void MsgProjectDestroy(u32, u32);
void MsgProjectBind(u32, u32, u32);
void MsgSetBind(u32, u32, u32);
u32 MsgSetGetEntry(u32, u32);
u32 MsgSetGetLabel(u32, u32, u32);
u32 MsgTextTagEnd(u32);
u32 MsgTextTagBegin(u32);
void MsgCircleConstruct(u32);
void MsgAngleConstruct(u32);
void MsgRegisterGlobal(u32);
void MsgCopyBytes(u32, u32, u32, u32);
void MsgSafeStringAssureEmpty(u32);
void MsgBufferedStringAssure(u32);

struct MsgCommonSetNames {
    MsgSafeString mNames[4];
};

namespace msg_res_tu {
constexpr u32 safeString = 0x100E0800;
constexpr u32 bufferedString = 0x100E0860;
constexpr u32 fixed256 = 0x100E0878;
constexpr u32 project = 0x10147400;
constexpr u32 disposer = 0x100E09F0;
constexpr u32 messageSet = 0x100E08C0;
constexpr u32 buffered256Base = 0x100E0818;
constexpr u32 buffered64Base = 0x100E0830;
constexpr u32 fixed64 = 0x100E0848;
constexpr u32 buffered128Base = 0x100E0890;
constexpr u32 fixed128 = 0x100E08A8;
constexpr u32 msbtExtension = 0x100E092C;
constexpr u32 archiveSuffix = 0x100E0934;
constexpr u32 setPathFormat = 0x100E093C;
constexpr u32 pathSeparator = 0x100E0954;
constexpr u32 extensionSeparator = 0x100E0956;
constexpr u32 emptyString = 0x100E0958;
constexpr u32 projectPathFormat = 0x100E095C;
constexpr u32 unitSetName = 0x100E0974;
constexpr u32 layoutSeparator = 0x100E0984;
constexpr u32 copyTag = 0x100E0988;
constexpr u32 copyTagLower = 0x100E0990;
constexpr u32 projectFile = 0x100E09B4;
constexpr u32 projectArchive = 0x100E09C0;
constexpr u32 message = 0x100E09A0;
constexpr u32 message2 = 0x100E09CC;
constexpr u32 message3 = 0x100E09D8;
constexpr u32 message4 = 0x100E09E4;
constexpr u32 country = 0x100E09B0;
constexpr u32 region = 0x100E09A8;
constexpr u32 pathSeparatorDtor = 0x101F4A70;
constexpr u32 extensionSeparatorDtor = 0x101F4A7C;
constexpr u32 emptyStringDtor = 0x101F4A88;
constexpr u32 unitSetNameDtor = 0x101F4A94;
constexpr u32 copyTagDtor = 0x101F4AA0;
constexpr u32 copyTagLowerDtor = 0x101F4AAC;
constexpr u32 layoutSeparatorDtor = 0x101F4AB8;
constexpr u32 headerZeroDtor = 0x101F4AC4;
constexpr u32 headerAngleDtor = 0x101F4AD0;
constexpr u32 headerCircleDtor = 0x101F4ADC;
} // namespace msg_res_tu

enum MsgLimits : u32 {
    MSG_LABEL_NOT_FOUND = 0xFFFFFFFEu,
    MSG_MAX_SETS = 0x100,
    MSG_MAX_SCAN = 0x40000
};

WWHD_OFFSET(MsgResMgr, __vtbl, 0xC);
WWHD_OFFSET(MsgResMgr, mProject, 0x10);
WWHD_OFFSET(MsgResMgr, mSetCount, 0x34);
WWHD_OFFSET(MsgResMgr, mpSets, 0x38);
WWHD_OFFSET(MsgProject, mpData, 0x0);
WWHD_OFFSET(MsgProject, mSections, 0x4);
WWHD_OFFSET(MsgProject, mNameCount, 0x1C);
WWHD_OFFSET(MsgProject, __vtbl, 0x20);
WWHD_OFFSET(MsgSet, mpData, 0x0);
WWHD_OFFSET(MsgSet, mEntryCount, 0x4);
WWHD_OFFSET(MsgSet, __vtbl, 0x8);
WWHD_OFFSET(MsgSet, mName, 0xC);
WWHD_OFFSET(MsgLayout, __vtbl, 0x8);
WWHD_OFFSET(MsgLayout, mpLayout, 0xC);
WWHD_OFFSET(MsgLayout, mpSetName, 0x1C);
WWHD_OFFSET(MsgLayout, mpParent, 0x34);
WWHD_OFFSET(MsgLayoutVtable, mpGetRtti, 0x14);
WWHD_OFFSET(MsgLayoutVtable, mpFindChild, 0x5C);
WWHD_OFFSET(MsgTagTarget, mLabel, 0x128);
WWHD_OFFSET(MsgAttribute, mId, 0xC);
WWHD_OFFSET(MsgConfig, mLanguage, 0x14);
WWHD_OFFSET(MsgStringVtable, mpAssureTermination, 0x14);
WWHD_OFFSET(MsgResourceVtable, mpGetFile, 0x3C);
WWHD_OFFSET(MsgResource, __vtbl, 0x10);
