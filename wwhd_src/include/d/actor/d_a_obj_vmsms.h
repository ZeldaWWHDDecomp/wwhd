/* Master Sword pedestal sword, WWHD members +0x11C from GameCube. */
#pragma once
#include "gc_api/gc_api.h"
class daObjVmsms_c : public fopAc_ac_c {
public:
    daObjVmsms_c();
    enum : u32 { __vt = 0x10032EEC };
    BOOL create_heap();
    cPhs_State _create();
    bool _delete();
    void init_mtx();
    bool check_demo() const;
    bool _execute();
    bool _draw();
    static const char M_arcname[];
    /* 0x3AC */ gptr<J3DModel> mModel;
    /* 0x3B0 */ request_of_phase_process_class mPhs;
};
WWHD_OFFSET(daObjVmsms_c, mModel, 0x3AC);
WWHD_OFFSET(daObjVmsms_c, mPhs, 0x3B0);
WWHD_SIZE(daObjVmsms_c, 0x3B8);
namespace dSv_event_flag_c { enum { MASTER_SWORD_CUTSCENE = 0x2D04 }; }
BOOL vmsms_isEventBit(void*, u16);
BOOL vmsms_startCheckOld(void*, const char*);

// HD: private view of the save event-flag member.
struct vmsms_save_c {
    u8 _0[0x644];
    u8 mEvent;
};
WWHD_OFFSET(vmsms_save_c, mEvent, 0x644);
