/* Ghost Ship clear-event tag, WWHD layout. */
#pragma once
#include "gc_api/gc_api.h"

/* Existing unit-local bindings, retaining the HD argument widths. */
BOOL dEvent_manager_endCheckOld(dEvent_manager_c*, const char*);
const char* dEvent_manager_getMyNowCutName(dEvent_manager_c*, s32);
void dKy_set_nexttime(f32);
void dComIfGp_setNextStage(const char*, s16, s8, s8, f32, u32, s32, s8);
void fopAcM_orderOtherEvent(fopAc_ac_c*, const char*, u16, u16);
inline BOOL dComIfGp_evmng_endCheck(const char* name) {
    return dEvent_manager_endCheckOld(dComIfGp_getPEvtManager(), name);
}
inline const char* dComIfGp_evmng_getMyNowCutName(s32 staffIdx) {
    return dEvent_manager_getMyNowCutName(dComIfGp_getPEvtManager(), staffIdx);
}
namespace ghostship {
/* HD: GHS inlines strcmp as an unsigned-byte loop; resolve host literals through the glue. */
inline int strcmp(const char* a, const char* b) {
    u32 ap = gabi::guest_addr(a), bp = gabi::guest_addr(b);
    u8 ca, cb;
    do {
        ca = gabi::load<u8>(ap++);
        cb = gabi::load<u8>(bp++);
    } while (ca == cb && ca != 0);
    return (int)ca - (int)cb;
}
}

enum {
    JA_SE_CV_G_SHIP_LAUGH = 0x4925,
    JA_SE_CV_G_SHIP_SCREAM = 0x4926,
    JA_SE_LK_WARP_TO_G_SHIP = 0x2889,
};
struct dSv_event_flag_c {
    enum { GHOST_SHIP = 0x8803, UNK_85FF = 0x85FF, UNK_C3FF = 0xC3FF };
};

/* HD: the HIO virtual table follows the data at +0xC. */
struct daTag_Gship_HIO_c {
    enum : u32 { __vt = 0x1003F0FC };
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ be<u8> field_0x05;
    /* 0x02 */ u8 _02[0xC - 0x2];
    /* 0x0C */ be<u32> __vtbl;
};
WWHD_SIZE(daTag_Gship_HIO_c, 0x10);

class daTag_Gship_c : public fopAc_ac_c {
public:
    enum Proc_e { PROC_INIT_e = 0, PROC_EXEC_e = 1 };
    typedef void (daTag_Gship_c::*ProcFunc)();
    struct ProcEntry {
        gabi::ptmf<ProcFunc> init;
        gabi::ptmf<ProcFunc> run;
        gptr<const char> name;
    };
    daTag_Gship_c() {
        fopAc_ac_c_ct(this);
        __vtbl = 0x1003F0EC; // HD: this translation unit's actor vtable.
    }
    void modeClearWaitInit();
    void modeClearWait();
    void modeClearEventInit();
    void modeClearEvent();
    void modeProc(Proc_e, int);
    bool _execute();
    void getArg();
    cPhs_State _create();

    /* 0x3AC */ be<s32> mMode;
    /* 0x3B0 */ be<s8> field_0x294;
    /* 0x3B1 */ u8 _3B1[3];
    /* 0x3B4 */ be<f32> field_0x298;
};
WWHD_OFFSET(daTag_Gship_c, mMode, 0x3AC);
WWHD_OFFSET(daTag_Gship_c, field_0x298, 0x3B4);
WWHD_SIZE(daTag_Gship_c, 0x3B8);
