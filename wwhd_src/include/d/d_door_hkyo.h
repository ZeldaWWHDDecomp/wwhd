/* Door message and Triforce crest helpers, GameCube names with WWHD layouts.
 * HD: message work grows by 0x100 and its message pointer becomes a ready byte. */
#pragma once
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"

struct dDoor_messageManagerGlobal_c { gptr<void> mpManager; };
WWHD_SIZE(dDoor_messageManagerGlobal_c, 4);

class dDoor_msg_c {
public:
    enum Status { MSG_TYPING = 6, MSG_DISPLAYED = 14, MSG_CONTINUES = 15,
                  MSG_ENDS = 16, BOX_CLOSED = 18, MSG_DESTROYED = 19 };
    /* 0x000 */ be<s32> mMsgPId;
    /* 0x004 */ be<u8> mReady;
    /* 0x005 */ u8 _005[0x108 - 5];
    /* 0x108 */ be<u8> mState;
    /* 0x109 */ u8 _109;
    /* 0x10A */ be<s16> mMsgId;
    void init(s32 msg_id);
    BOOL proc(cXyz* pos);
};
WWHD_OFFSET(dDoor_msg_c, mReady, 4);
WWHD_OFFSET(dDoor_msg_c, mState, 0x108);
WWHD_OFFSET(dDoor_msg_c, mMsgId, 0x10A);
WWHD_SIZE(dDoor_msg_c, 0x10C);

// HD: only the frame controller prefix of the opaque brk wrapper is used here.
struct dDoor_brkFrame_c { J3DFrameCtrl mFrameCtrl; };
class dDoor_hkyo_c {
public:
    enum Resource { dRes_INDEX_HKYO_BDL_HKYO1_e = 4, dRes_INDEX_HKYO_BRK_HKYO1A_e = 7, dRes_INDEX_HKYO_BRK_HKYO1B_e = 8, dRes_INDEX_HKYO_BRK_HKYO1C_e = 9 };
    enum Animation { NONE, FIRST, SECOND, THIRD };
    /* 0x00 */ request_of_phase_process_class mPhs;
    /* 0x08 */ gptr<J3DModel> mpModel;
    /* 0x0C */ gptr<mDoExt_brkAnm> mpBrkAnm;
    /* 0x10 */ be<u8> mAnmIdx;
    /* 0x11 */ be<u8> m11;
    /* 0x12 */ u8 _12[2];
    s32 resLoad();
    void resDelete();
    BOOL create();
    void init();
    void calcMtx(fopAc_ac_c* door, f32 offsY);
    BOOL chkFirst();
    void draw(fopAc_ac_c* door);
    void setAnm(u32 idx); // HD: comparison uses the full register before storing a byte.
    void proc(fopAc_ac_c* door);
    void onFirst();
};
WWHD_OFFSET(dDoor_hkyo_c, mpModel, 8);
WWHD_OFFSET(dDoor_hkyo_c, mpBrkAnm, 0xC);
WWHD_OFFSET(dDoor_hkyo_c, mAnmIdx, 0x10);
WWHD_OFFSET(dDoor_hkyo_c, m11, 0x11);
WWHD_SIZE(dDoor_hkyo_c, 0x14);

// HD: archive stream and embedded fixed buffer; no GameCube equivalent.
struct dDoor_safeString_vt { u8 _00[0x14]; be<u32> assureTermination; };
struct dDoor_safeString_c {
    gptr<be<u8>> mStringTop;
    gptr<dDoor_safeString_vt> __vtbl;
    void assureTermination() { gabi::call_ptr<void>(__vtbl->assureTermination, this); }
};
struct dDoor_bufferString_c {
    gptr<be<u8>> mStringTop;
    be<u32> __vtbl;
    be<s32> mBufferSize;
    be<u8> mBuffer[16];
};
struct dDoor_archiveStream_c {
    /* 0x00 */ be<u32> mBase[2];
    /* 0x08 */ u8 _08[0x24 - 8];
    /* 0x24 */ dDoor_bufferString_c mName;
};
WWHD_OFFSET(dDoor_bufferString_c, mBuffer, 0xC);
WWHD_SIZE(dDoor_bufferString_c, 0x1C);
WWHD_OFFSET(dDoor_archiveStream_c, mName, 0x24);
WWHD_SIZE(dDoor_archiveStream_c, 0x40);

struct dDoor_playerFlags_c { u8 _000[0x3C0]; be<u32> mGrabUpEnd; };
WWHD_OFFSET(dDoor_playerFlags_c, mGrabUpEnd, 0x3C0);

// HD: these entry points have ABI shapes different from their GC counterparts.
s32 dDoor_messageSet(void* manager, s32 id, cXyz* pos);
u32 dDoor_messageStatus(void* manager);
void dDoor_messageSetStatus(void* manager, u32 status);
void dDoor_phaseConstruct(request_of_phase_process_class* phase, u32 size);
mDoExt_brkAnm* dDoor_brkConstruct(mDoExt_brkAnm* anm);
u32 dDoor_brkInit(mDoExt_brkAnm* anm, J3DModelData* data, void* key,
                  u32 play, s32 mode, s32 start, s32 end, u32 enable, s32 extra, f32 speed);
BOOL dDoor_isTact(dSv_player_collect_c* collect, u32 tact);
void dDoor_actorDestroy(fopAc_ac_c* actor, u32 flags);
void dDoor_streamConstruct(dDoor_archiveStream_c* stream);
u32 dDoor_streamOpen(dDoor_archiveStream_c* stream, void* archive);
void dDoor_streamDestroy(dDoor_archiveStream_c* stream, u32 flags);
void dDoor_copyBytes(be<u8>* dest, be<u8>* source, s32 count, u32 flags);
void dDoor_assert(const char* file, s32 line, const char* expression);
