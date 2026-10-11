/* Environment tag03, private HD layout. */
#pragma once
#include "gc_api/gc_api.h"
void dKy_change_colpat(u8);
BOOL dKy_contrast_flg_get();
void dKy_contrast_flg_set(bool);
void kytag03_actor_dt(void*, s32);
struct kytag03_play_c {
    u8 padding[0x52B8];
    be<u16> mEventFlag;
};
WWHD_OFFSET(kytag03_play_c, mEventFlag, 0x52B8);
WWHD_SIZE(kytag03_play_c, 0x52BA);
inline bool kytag03_chkEventFlag(u16 flag) {
    return (((kytag03_play_c*)dComIfGp_get())->mEventFlag & flag) != 0;
}
struct kytag03_class : fopAc_ac_c {
    kytag03_class();
    enum : u32 { __vt = 0x10013B1C };
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<mDoExt_McaMorf> mpModel;
    /* 0x3B8 */ u8 m29C[4];
    /* 0x3BC */ be<u8> field_0x2a0;
    /* 0x3BD */ be<u8> mSwitchNo;
    /* 0x3BE */ be<u8> mbRoomActive;
    /* 0x3BF */ be<u8> mbVisible;
    /* 0x3C0 */ be<u8> mbIsActive;
    /* 0x3C1 */ u8 _3C1[3];
    /* 0x3C4 */ be<f32> field_0x2a8;
};
WWHD_OFFSET(kytag03_class, mPhs, 0x3AC);
WWHD_OFFSET(kytag03_class, mpModel, 0x3B4);
WWHD_OFFSET(kytag03_class, mSwitchNo, 0x3BD);
WWHD_OFFSET(kytag03_class, field_0x2a8, 0x3C4);
WWHD_SIZE(kytag03_class, 0x3C8);

