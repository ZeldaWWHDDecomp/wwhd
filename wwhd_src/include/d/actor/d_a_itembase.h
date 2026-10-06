/* daItemBase_c (base class of the field/get items), WWHD layout.
 *
 * GameCube -> WWHD (measured from the verified functions and daItem_c::_daItem_create):
 *  - the GameCube vtable pointer at 0x290 is the HD C++ vtable at +0xB4, so the members start at
 *    0x3AC (GameCube 0x294): +0x118;
 *  - mShadowId (GameCube 0x62C) is gone (HD: no setShadow; the vtable has no slot for it), so
 *    +0x114 from mItemBitNo; size 0x750 (GameCube 0x63C).
 * HD vtable (daItemBase_c 0x10012158, daItem_c 0x10011FF8; 8 bytes per slot, function at +4):
 *   0x0C ~dtor, 0x14 DrawBase, 0x1C setListStart, 0x24 settingBeforeDraw, 0x2C setTevStr,
 *   0x34 animEntry, 0x3C clothCreate. */
#pragma once
#include "bindings.h"

struct daItemBase_c : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ gptr<J3DModel> mpModelArrow[2];
    /* 0x3C0 */ gptr<mDoExt_btkAnm> mpBtkAnm1;
    /* 0x3C4 */ gptr<mDoExt_btkAnm> mpBtkAnm2;
    /* 0x3C8 */ gptr<mDoExt_brkAnm> mpBrkAnm1;
    /* 0x3CC */ gptr<mDoExt_brkAnm> mpBrkAnm2;
    /* 0x3D0 */ gptr<mDoExt_bckAnm> mpBckAnm;
    /* 0x3D4 */ dBgS_ObjAcch mAcch;
    /* 0x598 */ dBgS_AcchCir mAcchCir;
    /* 0x5D8 */ dCcD_Stts mStts;
    /* 0x614 */ dCcD_Cyl mCyl;
    /* 0x744 */ be<s32> mItemBitNo;   /* GameCube 0x630 */
    /* 0x748 */ be<s32> m_timer;
    /* 0x74C */ be<s16> m_get_timer;
    /* 0x74E */ be<u8> m_itemNo;
    /* 0x74F */ be<u8> mDrawFlags;

    BOOL DeleteBase(const char*);
    BOOL CreateItemHeap(const char*, s16, s16, s16, s16, s16, s16, s16);
    BOOL DrawBase();
    void setListStart();
    void setListEnd();
    void settingBeforeDraw();
    void setTevStr();
    void animEntry();
    void animPlay(f32, f32, f32, f32, f32);
    BOOL clothCreate();

    u8 getItemNo();
    u8 getHeight();
    u8 getR();
    void hide();
    void show();
    void changeDraw();
    bool chkDraw();
    void dead();
    bool chkDead();
    void setLoadError();

    /* virtual calls through the HD vtable (+0xB4) */
    u32 vfn(u32 slot) { return gabi::load<u32>(__vtbl + slot); }
};
WWHD_OFFSET(daItemBase_c, mPhs, 0x3AC);
WWHD_OFFSET(daItemBase_c, mAcch, 0x3D4);
WWHD_OFFSET(daItemBase_c, mAcchCir, 0x598);
WWHD_OFFSET(daItemBase_c, mStts, 0x5D8);
WWHD_OFFSET(daItemBase_c, mCyl, 0x614);
WWHD_OFFSET(daItemBase_c, mItemBitNo, 0x744);
WWHD_OFFSET(daItemBase_c, m_itemNo, 0x74E);
WWHD_SIZE(daItemBase_c, 0x750);

enum {
    daItemBase_VT_DTOR = 0x0C,
    daItemBase_VT_DRAWBASE = 0x14,
    daItemBase_VT_SETLISTSTART = 0x1C,
    daItemBase_VT_SETTINGBEFOREDRAW = 0x24,
    daItemBase_VT_SETTEVSTR = 0x2C,
    daItemBase_VT_ANIMENTRY = 0x34,
    daItemBase_VT_CLOTHCREATE = 0x3C,
};

/* ---- dItem_data (d_item_data.cpp): static tables in .data ----
 * item_resource[0x100] at 0x101E4674 (0x24 bytes): +0 arcname, +8 bmd, +0xA srt, +0xC srt2,
 *   +0xE tev, +0x10 tev2, +0x12 bck, +0x14 s8 tevFrm;
 * field_item_res[0x100] at 0x101E6A74 (0x1C bytes): +0 arcname, +4 bmd, +6 srt, +8 srt2,
 *   +0xA tev, +0xC tev2, +0xE bck, +0x18 u16 heapSize;
 * item_info[0x100] at 0x101E8674 (4 bytes): +0 shadow size, +1 height, +2 radius, +3 flags. */
namespace dItem_data {
inline u32 item_resource(u32 no) { return 0x101E4674 + no * 0x24; }
inline u32 field_item_res(u32 no) { return 0x101E6A74 + no * 0x1C; }
inline u32 item_info(u32 no) { return 0x101E8674 + no * 4; }
inline s8 getTevFrm(u32 no) { return gabi::load<s8>(item_resource(no) + 0x14); }
inline u8 getH(u32 no) { return gabi::load<u8>(item_info(no) + 1); }
inline u8 getR(u32 no) { return gabi::load<u8>(item_info(no) + 2); }
inline u8 getFlag(u32 no) { return gabi::load<u8>(item_info(no) + 3); }
}  // namespace dItem_data

/* HARNESS WORKAROUND (SHARED-CANDIDATE): the original's own stack frame, for functions whose
 * result depends on stack addresses or on stack slots they never initialise (CreateItemHeap
 * stores self-relative offsets to a stack temporary; itemActionForRupee reads an uninitialised
 * slot). Locals allocated after it go below it. */
struct GuestFrame {
    u32 sp;
    u32 size;
    explicit GuestFrame(u32 n) : size(n) { gabi::cpu->r[1] -= n; sp = gabi::cpu->r[1]; }
    ~GuestFrame() { gabi::cpu->r[1] += size; }
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02550FC4 isBomb(u8), 025510E8 isHeart(u8) (d_item) */
inline BOOL isBomb(u8 itemNo) { return gabi::call<BOOL>(0x02550FC4, itemNo); }
inline BOOL isHeart(u8 itemNo) { return gabi::call<BOOL>(0x025510E8, itemNo); }
/* the frame control (rate at +0, frame at +4) of any mDoExt_*Anm */
inline J3DFrameCtrl* anm_frameCtrl(void* a) { return gabi::at<J3DFrameCtrl>(gabi::ea(a)); }
