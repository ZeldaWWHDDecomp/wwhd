#pragma once
#include "gc_api/gc_api.h"
struct PhotoEventCut {
    u8 mPadding[0x60];
    be<u8> mAttnFlag;
    u8 mTail[0x0B];
};
struct PhotoLenzo : fopAc_ac_c {
    u8 mPadding[0xB35-sizeof(fopAc_ac_c)];
    be<u8> mPlaceState;  /* was field_0x9C1: view of daNpcPhoto_c::mPlaceState (d_a_npc_photo.h); isDate() tests 4 as tww's daNpc_Photo isDate() */
    bool isDate() const { return mPlaceState == 4; }
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
    gptr<be<u32>> mpMsgSeq;  /* was field_0x310: eventMesSetInit 024B1F30 / getMsg 024B1FD4 clear it; next_msgStatus 024B1FF0 steps it through a 0-terminated message number list while it is set and clears it at the end (tww the same) */
    be<s16> mPhotoTalkEventIdx, mPhotoTalk2EventIdx;
    be<u8> mTagNo, mMode, _436;
    be<s8> mActIdx;
    be<u8> mTalkEventSet; u8 _439[3];  /* mTalkEventSet was field_0x31C: daTagPhoto_construct 024B1D20 clears it; executeWait 024B24A4: the first time the player comes within 160 it is set and the photo talk event is ordered unless Po is on a date (tww the same) */
};
WWHD_SIZE(daTagPhoto_c, 0x43c);
WWHD_OFFSET(daTagPhoto_c, mMsgID, 0x3b4);
WWHD_OFFSET(daTagPhoto_c, mEventCut, 0x3c0);
WWHD_OFFSET(daTagPhoto_c, mpMsgSeq, 0x42c);
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

WWHD_OFFSET(PhotoLenzo, mPlaceState, 0xB35);
WWHD_SIZE(PhotoEventCut, 0x6C);
