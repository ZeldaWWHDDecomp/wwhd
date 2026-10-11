/** HD message routing: GameCube message-set names with explicit HD manager views.
 * HD moves renderer selection into a manager and addresses messages by SafeString keys.
 * These private views do not change the shared core/controller layouts.
 */
#pragma once
#include "wwhd.h"
namespace MesgRouting {
struct StringVtable {
    u8 prefix[0x14];
    be<u32> assure;
};
struct StringBase {
    gptr<be<u8>> mData;
    gptr<StringVtable> mVtable;
};
struct String : StringBase {
    be<u32> mCapacity;
};
struct String32 : String {
    be<u8> mBuffer[32];
};
struct String64 : String {
    be<u8> mBuffer[64];
};
struct ResourceKey : String {
    be<u8> mBuffer[6];
    u8 padding[2];
};
struct KeyFrame {
    u8 linkage[8];
    StringBase source;
    String32 name;
};
struct PairFrame {
    u8 linkage[8];
    String64 group;
    String64 name;
};
struct PositionFrame {
    u8 linkage[8];
    be<f32> x, y, z;
};
// HD: BMG entry fields are regrouped; retain tww names where a role is known.
struct MessageEntry {
    be<u8> mDrawType, mTextboxType;
    u8 reserved[2];
    be<s16> mMsgNo;
    u8 data[5];
    be<u8> mInitialCamera;
    u8 flags[2];
    be<u8> mTextboxPosition;
};
struct Manager {
    /* 0x000 */ u8 prefix[0x14];
    /* 0x014 */ u8 mProcessorWork[0x12C - 0x14];
    /* 0x12C */ be<u32> mMessageContext;
    /* 0x130 */ be<u32> mRouteCode;
    u8 processorTail[0x240 - 0x134];
    /* 0x240 */ gptr<MessageEntry> mWordWork;
    u8 managerData[0x920 - 0x244];
    /* 0x920 */ be<u8> mTactMsgFlag;
    u8 flagTail[0x938 - 0x921];
    /* 0x938 */ be<u32> mMsgNo;
    u8 selectionData[0x94C - 0x93C];
    /* 0x94C */ be<u32> mMode;
    /* 0x950 */ gptr<void> mResetWork, mScreenWork, mRenderWork, mAuxScreenWork, mSequenceWork;
};
struct Play {
    u8 prefix[0x5BA4];
    /* 0x5BA4 */ be<s16> mMsgNo;
    u8 messageData[0x5BB3 - 0x5BA6];
    /* 0x5BB3 */ be<u8> mScopeMesgStatus;
    u8 statusData[0x5BC5 - 0x5BB4];
    /* 0x5BC5 */ be<u8> mTextboxPosition, mMesgAnimeTag;
    u8 tagData[0x5C30 - 0x5BC7];
    /* 0x5C30 */ be<u32> mMesgCameraTag;
    /* 0x5C34 */ be<u32> mInitialCamera;
    u8 playerData[0x5CD8 - 0x5C38];
    /* 0x5CD8 */ be<u32> mPlayerStatus;
};
enum {
    STRING_MAX_LENGTH = 0x40000,
    MESG_ERROR = 0xFFFFFFFFu,
    MESG_IDLE = 0,
    MESG_CONTINUES = 15,
    MESG_BOX_OPENING = 2,
    MESG_SCOPE_ACTIVE = 11,
    PLAYER_TELESCOPE_LOOK = 0x200000,
    MODE_AUX = 0,
    MODE_TALK = 1,
    MODE_RESET = 2,
    MODE_BOX = 3,
    MODE_OVERLAY = 4,
    MODE_SEQUENCE = 5,
    TEXTBOX_AUX = 1,
    TEXTBOX_RESET = 2,
    TEXTBOX_OVERLAY = 6,
    TEXTBOX_BOX = 7,
    TEXTBOX_AUX_SMALL = 9,
    TEXTBOX_AUX_EVENT = 14,
    MSG_SPECIAL = 0x266,
    MSG_TACT = 0x5AC,
    MSG_SEQUENCE = 0xEDC,
    MSG_SEQUENCE_NEXT = 0xEDD,
    MSG_SEQUENCE_ALT = 0xEE2,
    MSG_SEQUENCE_ALT_NEXT = 0xEE3
};
WWHD_OFFSET(StringVtable, assure, 0x14);
WWHD_SIZE(StringVtable, 0x18);
WWHD_OFFSET(StringBase, mData, 0);
WWHD_OFFSET(StringBase, mVtable, 4);
WWHD_SIZE(StringBase, 8);
WWHD_OFFSET(String, mCapacity, 8);
WWHD_SIZE(String, 12);
WWHD_OFFSET(String32, mBuffer, 12);
WWHD_SIZE(String32, 44);
WWHD_OFFSET(String64, mBuffer, 12);
WWHD_SIZE(String64, 76);
WWHD_OFFSET(ResourceKey, mBuffer, 12);
WWHD_SIZE(ResourceKey, 20);
WWHD_OFFSET(KeyFrame, source, 8);
WWHD_OFFSET(KeyFrame, name, 0x10);
WWHD_SIZE(KeyFrame, 0x3C);
WWHD_OFFSET(PairFrame, group, 8);
WWHD_OFFSET(PairFrame, name, 0x54);
WWHD_SIZE(PairFrame, 0xA0);
WWHD_OFFSET(PositionFrame, x, 8);
WWHD_OFFSET(PositionFrame, y, 12);
WWHD_OFFSET(PositionFrame, z, 16);
WWHD_SIZE(PositionFrame, 20);
WWHD_OFFSET(MessageEntry, mTextboxType, 1);
WWHD_OFFSET(MessageEntry, mMsgNo, 4);
WWHD_OFFSET(MessageEntry, mInitialCamera, 0xB);
WWHD_OFFSET(MessageEntry, mTextboxPosition, 0xE);
WWHD_SIZE(MessageEntry, 15);
WWHD_OFFSET(Manager, mProcessorWork, 0x14);
WWHD_OFFSET(Manager, mMessageContext, 0x12C);
WWHD_OFFSET(Manager, mRouteCode, 0x130);
WWHD_OFFSET(Manager, mWordWork, 0x240);
WWHD_OFFSET(Manager, mTactMsgFlag, 0x920);
WWHD_OFFSET(Manager, mMsgNo, 0x938);
WWHD_OFFSET(Manager, mMode, 0x94C);
WWHD_OFFSET(Manager, mResetWork, 0x950);
WWHD_OFFSET(Manager, mScreenWork, 0x954);
WWHD_OFFSET(Manager, mRenderWork, 0x958);
WWHD_OFFSET(Manager, mAuxScreenWork, 0x95C);
WWHD_OFFSET(Manager, mSequenceWork, 0x960);
WWHD_SIZE(Manager, 0x964);
WWHD_OFFSET(Play, mMsgNo, 0x5BA4);
WWHD_OFFSET(Play, mScopeMesgStatus, 0x5BB3);
WWHD_OFFSET(Play, mTextboxPosition, 0x5BC5);
WWHD_OFFSET(Play, mMesgAnimeTag, 0x5BC6);
WWHD_OFFSET(Play, mMesgCameraTag, 0x5C30);
WWHD_OFFSET(Play, mInitialCamera, 0x5C34);
WWHD_OFFSET(Play, mPlayerStatus, 0x5CD8);
WWHD_SIZE(Play, 0x5CDC);
} // namespace MesgRouting

// Unit-private HD bindings retain full register widths.
u32 MesgRoute_format(MesgRouting::String*, const char*, u32);
u32 MesgRoute_sourceNoop(MesgRouting::StringBase*);
u32 MesgRoute_copy(void*, void*, u32, u32);
u32 MesgRoute_initColor(void*);
u32 MesgRoute_lookup(void*, void*, MesgRouting::StringBase*, MesgRouting::StringBase*);
MesgRouting::Play* MesgRoute_getPlay();
u32 MesgRoute_getStatus();
u32 MesgRoute_sequence(void*, u32, u32, u32, u32);
u32 MesgRoute_aux(void*, u32, u32, u32);
u32 MesgRoute_reset(void*, u32, u32, u32);
u32 MesgRoute_talk(void*, u32, u32, u32);
u32 MesgRoute_render(void*, u32, u32);
u32 MesgRoute_select(u32, u32, u32);
u32 MesgRoute_setStatus(MesgRouting::Manager*, u32);
u32 MesgRoute_setNames(void*, MesgRouting::StringBase*, MesgRouting::StringBase*);
u32 MesgRoute_sequenceNamed(void*, u32, u32);
