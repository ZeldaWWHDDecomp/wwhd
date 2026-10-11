#pragma once
#include "gc_api/gc_api.h"
struct PhotoEventCut {
    u8 mPadding[0x60];
    be<u8> mAttnFlag;
    u8 mTail[0x0B];
};
struct PhotoLenzo : fopAc_ac_c {
    u8 mPadding[0xB35-sizeof(fopAc_ac_c)];
    be<u8> field_0x9C1;
    bool isDate() const { return field_0x9C1 == 4; }
};
struct daTagPhoto_c : fopAc_ac_c {
    using ProcFunc_t = void (daTagPhoto_c::*)();
    struct PhotoProcEntry { gabi::ptmf<ProcFunc_t> proc; };
    u8 getPrmTagNo();
    s32 createInit();
    s32 createHeap();
    s32 _create();
    void setMessage(u32 msg);
    void eventMesSetInit(s32 staff);
    u32 getMsg();
    u16 next_msgStatus(be<u32>* out);
    u16 talk(s32 mode);
    bool eventMesSet();
    void privateCut();
    void eventMove();
    s32 _execute();
    void setMode(u32 mode);
    void executeWait();
    void executeTalk();
    request_of_phase_process_class mPhase;
    be<u32> mMsgID;
    // HD: message readiness is a flag rather than GC mpCurrMsg.
    be<u8> mMessageActive; u8 _3b9[3];
    be<u32> mMsgNo;
    PhotoEventCut mEventCut;
    gptr<be<u32>> field_0x310;
    be<s16> mPhotoTalkEventIdx, mPhotoTalk2EventIdx;
    be<u8> mTagNo, mMode, _436;
    be<s8> mActIdx;
    be<u8> field_0x31C; u8 _439[3];
};
WWHD_SIZE(daTagPhoto_c, 0x43c);
WWHD_OFFSET(daTagPhoto_c, mMsgID, 0x3b4);
WWHD_OFFSET(daTagPhoto_c, mEventCut, 0x3c0);
WWHD_OFFSET(daTagPhoto_c, field_0x310, 0x42c);
WWHD_OFFSET(daTagPhoto_c, mTagNo, 0x434);

void Photo_eventCutConstruct(PhotoEventCut*);
void Photo_setActorInfo(PhotoEventCut*, const char*, daTagPhoto_c*);
BOOL Photo_cutProc(PhotoEventCut*);
s32 Photo_entryHeap(daTagPhoto_c*, u32, u32);
s32 Photo_phaseHandler(request_of_phase_process_class*, const void*, daTagPhoto_c*);
be<u32>* Photo_getMyIntegerP(dEvent_manager_c*, s32, const char*, s32);
s32 Photo_messageSet(void*, u32, cXyz*);
u16 Photo_getStatus(void*);
s32 Photo_getStatusInt(void*);
void Photo_setStatus(void*, u16);
s32 Photo_getMyActIdx(dEvent_manager_c*, s32, const void*, s32, s32, s32);
BOOL Photo_isAdvance(dEvent_manager_c*, s32);
void Photo_cutEnd(dEvent_manager_c*, s32);
PhotoLenzo* Photo_searchFromName(const char*, u32, u32);

WWHD_OFFSET(PhotoLenzo, field_0x9C1, 0xB35);
WWHD_SIZE(PhotoEventCut, 0x6C);
