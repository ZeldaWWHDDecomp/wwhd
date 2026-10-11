/* HD message state and tagged-string replacement. The GameCube scope helpers keep
 * their names; the other entry points operate on HD manager/string classes.
 * Private partial views preserve the manager's 0x9A4 layout without changing the
 * shared core/controller declarations. */
#pragma once
#include "wwhd.h"
struct dMesg_stringVtableHD {
    /* 0x00 */ u8 _00[0x14];
    /* 0x14 */ be<u32> mAssure;
};
WWHD_SIZE(dMesg_stringVtableHD, 0x18);
struct dMesg_stringBaseHD {
    /* 0x00 */ gptr<void> mData;
    /* 0x04 */ gptr<dMesg_stringVtableHD> mVtable;
    void assure() { gabi::call_ptr<void>(mVtable->mAssure, this); }
};
WWHD_SIZE(dMesg_stringBaseHD, 8);
struct dMesg_stringHD : dMesg_stringBaseHD {
    /* 0x08 */ be<u32> mCapacity;
};
WWHD_OFFSET(dMesg_stringHD, mCapacity, 8);
WWHD_SIZE(dMesg_stringHD, 12);
struct dMesg_nameBytesHD : dMesg_stringHD {
    be<u8> mBuffer[20];
};
WWHD_SIZE(dMesg_nameBytesHD, 32);
// HD: the name descriptor advertises 20 words, but its GHS stack slot reserves40 bytes.
struct dMesg_nameWordsHD : dMesg_stringHD {
    be<u16> mBuffer[20];
};
WWHD_SIZE(dMesg_nameWordsHD, 52);
struct dMesg_textKeyHD : dMesg_stringHD {
    be<u8> mBuffer[256];
};
WWHD_SIZE(dMesg_textKeyHD, 268);
struct dMesg_expandedTextHD : dMesg_stringHD {
    be<u16> mBuffer[17];
};
WWHD_SIZE(dMesg_expandedTextHD, 46);
struct dMesg_nameEntryHD {
    u8 _00[0x20];
    dMesg_stringBaseHD mName;
};
WWHD_OFFSET(dMesg_nameEntryHD, mName, 0x20);
WWHD_SIZE(dMesg_nameEntryHD, 0x28);
struct dMesg_nameSaveHD {
    u8 _00[0x12C0];
    u8 mPlayerNameSlot;
};
WWHD_OFFSET(dMesg_nameSaveHD, mPlayerNameSlot, 0x12C0);
WWHD_SIZE(dMesg_nameSaveHD, 0x12C1);
struct dMesg_positionHD {
    u8 _00[0x3C];
    be<f32> mPositionX;
    be<u32> mPositionYBits;
    be<u8> mFlags;
    u8 _45[3];
};
WWHD_OFFSET(dMesg_positionHD, mPositionX, 0x3C);
WWHD_OFFSET(dMesg_positionHD, mFlags, 0x44);
WWHD_SIZE(dMesg_positionHD, 0x48);
struct dMesg_resourceStateHD {
    u8 _00[0x54];
    gptr<dMesg_positionHD> mpPane;
};
WWHD_OFFSET(dMesg_resourceStateHD, mpPane, 0x54);
WWHD_SIZE(dMesg_resourceStateHD, 0x58);
struct dMesg_textTableHD {
    gptr<void> mTable;
    be<u32> mCount;
};
WWHD_SIZE(dMesg_textTableHD, 8);
class dMesg_state_c {
  public:
    void paneAction();
    void textAction();
    void textAdvance();
    u32 textReset();
    void positionActor(dMesg_positionHD* actor);
    void setContext(u32 context);
    void playerName(dMesg_stringHD* destination);
    void expandTagged(dMesg_stringHD* destination, u32 input, u32 context);
    void lookupText(be<u16>* destination, u32 id);
    u32 taggedLength(u32 input);
    u32 copyTagged(dMesg_stringHD* destination, u32 input);
    u32 ready();
    void paneReset();
    /* 0x000 */ u8 mDisposer[12];
    /* 0x00C */ be<u32> mVtable;
    /* 0x010 */ u8 _010[0x94C - 0x10];
    /* 0x94C */ be<u32> mMode;
    /* 0x950 */ u8 _950[0x10];
    /* 0x960 */ gptr<void> mSequenceWork;
    /* 0x964 */ gptr<dMesg_resourceStateHD> mResourceWork;
    /* 0x968 */ u8 _968[8];
    /* 0x970 */ be<u32> mTextContext;
    /* 0x974 */ u8 _974[0x30];
};
WWHD_OFFSET(dMesg_state_c, mVtable, 0xC);
WWHD_OFFSET(dMesg_state_c, mMode, 0x94C);
WWHD_OFFSET(dMesg_state_c, mSequenceWork, 0x960);
WWHD_OFFSET(dMesg_state_c, mResourceWork, 0x964);
WWHD_OFFSET(dMesg_state_c, mTextContext, 0x970);
WWHD_SIZE(dMesg_state_c, 0x9A4);
enum dMesg_tagHD { MESG_TAG_BEGIN_HD = 14, MESG_TAG_END_HD = 15, MESG_TAG_PLAYER_NAME_HD = 2 };

enum {
    fopMsgStts_SCOPE_ACTIVE_e = 11,
    fopMsgStts_SCOPE_WAIT_e = 13,
    daPyStts0_TELESCOPE_LOOK_e = 0x200000
};
void dMesg_sequenceActionHD(void*);
void dMesg_resourceActionHD(dMesg_resourceStateHD*);
void dMesg_resourceAdvanceHD(dMesg_resourceStateHD*);
u32 dMesg_resourceResetHD(dMesg_resourceStateHD*);
dMesg_nameEntryHD* dMesg_getSavedNameHD(void*);
void dMesg_byteStringNoopHD(dMesg_stringBaseHD*);
void dMesg_memcpyHD(void*, void*, u32, u32);
u32 dMesg_bytesToWordsHD(void*, u32, void*, u32);
void dMesg_playerNameWithContextHD(dMesg_state_c*, dMesg_stringHD*, u32);
void dMesg_formatKeyHD(dMesg_stringHD*, const char*, u32);
dMesg_textTableHD* dMesg_findTextTableHD(void*, dMesg_stringHD*);
u32 dMesg_findTextIndexHD(void*, void*);
u32 dMesg_getTextHD(void*, u32);
void dMesg_wideStringNoopHD(dMesg_stringBaseHD*);
u32 dMesg_getStatusHD(dMesg_state_c*);
void dMesg_sequenceResetHD(void*);
void dMesg_ownerResetHD(void*, u32);
void dMesg_disposerDestroyHD(dMesg_state_c*, u32);
void dMesg_deleteHD(void*);
