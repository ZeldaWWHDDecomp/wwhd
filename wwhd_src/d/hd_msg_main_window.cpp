/**
 * hd_msg_main_window.cpp
 * HD - the main message window (hdMsgMainWindow_c): exit-time destruction of its name tables,
 * the deleting destructor and its opening
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), verified function by function against the
 * recompiled original (tools/verify, unit hd_msg_msg_window_main). Each function is marked with its WWHD
 * address; the glue block at the end lists this translation unit's data (tools/verify/autoglue.py). Layout,
 * pane, animation, sound label and state names are HD strings; all other names are ours.
 */

#include "d/hd_msg_only_window.h"
using namespace gabi;
struct hdMsgMainWindow_c : hdMsgOnlyWindow_c { void destroy(u32); void enterOpen(); };
struct hdMsgMainEightNames_l { hdUiSafeString mNames[8]; };
struct hdMsgMainThreeNames_l { hdUiSafeString mNames[3]; };
static hdMsgMainEightNames_l sMsgMainEightNames;
static hdMsgMainThreeNames_l sMsgMainThreeNames;
static u8 sMsgMainStateVtable;
static u8 sMsgMainSecondaryVtable;
using MsgMainStringDestructor = void (*)(void*);
extern void msgMainDestroyString(void*);
extern void msgMainDestroyArray(void*, u32, u32, MsgMainStringDestructor, u32, u32);
extern void msgMainDestroyBase(void*, u32);
extern void msgMainFree(void*);

/* 026B1660 */
static s32 msgMainStateNumber(hdPartsStateId_c* state) { return state->mNumber; }

/* 026B1760 */
static void destroyMsgMainEightNames() { msgMainDestroyArray(&sMsgMainEightNames, 8, 8, msgMainDestroyString, 0, 0); }

/* 026B1784 */
static void destroyMsgMainThreeNames() { msgMainDestroyArray(&sMsgMainThreeNames, 3, 8, msgMainDestroyString, 0, 0); }

/* 026B17A8 */
void hdMsgMainWindow_c::destroy(u32 flags) {
    if (!this) return;
    auto* parts = reinterpret_cast<hdParts_c*>(this);
    parts->mStateHolder.mHolderVtable = &sMsgMainStateVtable;
    parts->mSecondaryVtable = &sMsgMainSecondaryVtable;
    msgMainDestroyBase(this, 0);
    if (flags&1) msgMainFree(this);
}
struct hdMsgMainDummy_c {
    hdUiSafeString mString;
    be<s32> mCapacity;
    be<u8> mBuffer[6];
    u8 mPadding[2];
};
struct hdMsgMainDummyRegistration_c { be<u32> mWords[3]; };
struct hdMsgMainOpenSound_c { u8 mReserved[0xA]; be<u8> mSound; };
struct hdMsgMainOpenMessage_c {
    u8 mReserved[0x120];
    PaneString_l mLabel;
    u8 mReserved12C[0x100];
    gptr<hdMsgMainOpenSound_c> mpSound;
};
WWHD_OFFSET(hdMsgMainDummy_c, mBuffer, 0xC);
WWHD_OFFSET(hdMsgMainOpenSound_c, mSound, 0xA);
WWHD_OFFSET(hdMsgMainOpenMessage_c, mLabel, 0x120);
WWHD_OFFSET(hdMsgMainOpenMessage_c, mpSound, 0x22C);
struct hdMsgMainMessageService_c { gptr<void> mpService; };
static hdMsgMainDummy_c sMsgMainDummy;
static u32 sMsgMainDummyInitialized;
static hdMsgMainDummyRegistration_c sMsgMainDummyRegistration;
static hdMsgMainMessageService_c sMsgMainMessageService;
static s8 sMsgMainRoom;
extern void msgMainOpenBeginSound();
extern void msgMainOpenSound(u32);
extern void msgMainOpenResolveSound(void*, HdMessageData*, hdUiSafeString*, PaneString_l*);
extern void msgMainOpenAssureSource(hdUiSafeString*);
extern void msgMainOpenCopy(be<u8>*, be<u8>*, u32, u32);
extern void msgMainOpenRegister(hdMsgMainDummyRegistration_c*);
extern u32 msgMainOpenReverb(s32);
extern void msgMainOpenVoice(u32, u32, u32);
enum { MsgMainStringScanLimit = 0x40000 };

static hdUiStringTable sMsgMainSourceStringVtable;
static hdUiStringTable sMsgMainDummyStringVtable;
static hdUiStringTable sMsgMainDummyOwnedVtable;

/* 026B1814 */
void hdMsgMainWindow_c::enterOpen() {
    msgMainOpenBeginSound();
    msgMainOpenSound(0x80C);
    if (!sMsgMainDummyInitialized) {
        sMsgMainDummy.mBuffer[5] = 0;
        hdUiSafeString source;
        source.mVtable = &sMsgMainSourceStringVtable;
        source.mText.v = guest_str("dummy");
        sMsgMainDummy.mString.mText = sMsgMainDummy.mBuffer;
        sMsgMainDummy.mCapacity = 6;
        sMsgMainDummy.mString.mVtable = &sMsgMainDummyStringVtable;
        sMsgMainDummyInitialized = 1;
        msgMainOpenAssureSource(&source);
        be<u8>* text = source.mText;
        s32 length = 0;
        if (*text != 0) {
            do {
                ++length;
                text = hdPartsView<be<u8>>(ea(text) + 1);
                if (length > MsgMainStringScanLimit) {
                    length = 0;
                    break;
                }
            } while (*text != 0);
        }
        s32 capacity = sMsgMainDummy.mCapacity;
        u32 assure = source.mVtable->mAssureTermination.get();
        if (length >= capacity) length = s32(u32(capacity) - 1);
        call_ptr<void>(assure, &source);
        msgMainOpenCopy(sMsgMainDummy.mBuffer, source.mText, u32(length), 0);
        *hdPartsView<be<u8>>(ea(sMsgMainDummy.mBuffer) + u32(length)) = 0;
        sMsgMainDummy.mString.mVtable = &sMsgMainDummyOwnedVtable;
        msgMainOpenRegister(&sMsgMainDummyRegistration);
    }
    HdMessageData* message = mMessage;
    auto* record = reinterpret_cast<hdMsgMainOpenMessage_c*>(message);
    msgMainOpenResolveSound(sMsgMainMessageService.mpService, message, &sMsgMainDummy.mString, &record->mLabel);
    record = reinterpret_cast<hdMsgMainOpenMessage_c*>(mMessage.get());
    hdMsgMainOpenSound_c* sound = record->mpSound;
    if (sound && sound->mSound != 0) {
        u32 reverb = msgMainOpenReverb(sMsgMainRoom);
        msgMainOpenVoice(sound->mSound, 0, reverb);
    }
}

/* WWHD glue
 data sMsgMainDummy 1049827C
 data sMsgMainDummyInitialized 10498390
 data sMsgMainDummyRegistration 101F65AC
 data sMsgMainMessageService 101F4AE8
 data sMsgMainRoom 1047E6C8
 data sMsgMainSourceStringVtable 100FD88C
 data sMsgMainDummyStringVtable 100FD8BC
 data sMsgMainDummyOwnedVtable 100FD8D4
 str 100FD994 "dummy" in 026B1814
 ext 025E1C58 void msgMainOpenBeginSound()
 ext 025E1988 void msgMainOpenSound(u32)
 ext 025F50E8 void msgMainOpenResolveSound(void*,HdMessageData*,hdUiSafeString*,PaneString_l*)
 ext 026B5454 void msgMainOpenAssureSource(hdUiSafeString*)
 ext C0009988 void msgMainOpenCopy(be<u8>*,be<u8>*,u32,u32)
 ext 028F026C void msgMainOpenRegister(hdMsgMainDummyRegistration_c*)
 ext 02520540 u32 msgMainOpenReverb(s32)
 ext 025E1C3C void msgMainOpenVoice(u32,u32,u32)
 data sMsgMainEightNames 104981B4
 data sMsgMainThreeNames 104981F4
 data sMsgMainStateVtable 100FE438
 data sMsgMainSecondaryVtable 100FE448
 ext 026B5440 void msgMainDestroyString(void*) callback
 ext 028F0164 void msgMainDestroyArray(void*,u32,u32,MsgMainStringDestructor,u32,u32)
 ext 026FC4D8 void msgMainDestroyBase(void*,u32)
 ext 0273AF40 void msgMainFree(void*)
*/
