/** Wind Crest, WWHD layout.
 * HD: the actor base shifts tww members by 0x11C; bool state fields occupy guest bytes. */
#pragma once
#include "gc_api/gc_api.h"
enum { dRes_INDEX_ITACT_BDL_ITACT_e = 3 };
enum { daPyStts0_SHIP_RIDE_e = 0x10000, daPyStts1_WIND_WAKER_CONDUCT_e = 1 };
class daSwTact_c : public fopAc_ac_c {
public:
    using ArcName = gptr<const char>;
    enum { m_heapsize = 0x3000 };
    f32 getR() { return mRadius * scale.x; }

    bool _delete();
    BOOL CreateHeap();
    void CreateInit();
    void set_mtx();
    s32 getAnswer();
    cPhs_State _create();
    bool _execute();
    bool _draw();

    /* 0x3AC */ request_of_phase_process_class mPhs; /* GameCube 0x290 */
    /* 0x3B4 */ gptr<J3DModel> model;
    /* 0x3B8 */ be<f32> mRadius;
    /* 0x3BC */ be<u32> mSwitchNo;
    /* 0x3C0 */ be<u8> mAnswer;
    /* 0x3C1 */ be<u8> mTrigger;
    /* 0x3C2 */ be<u8> mPlayerStatus;
    /* 0x3C3 */ u8 _3C3;
};
WWHD_OFFSET(daSwTact_c, model, 0x3B4);
WWHD_OFFSET(daSwTact_c, mPlayerStatus, 0x3C2);

WWHD_SIZE(daSwTact_c, 0x3C4);

namespace swtact_layout {
// HD: the two player virtual slots used by this actor.
struct PlayerVirtuals {
    u8 pad[0x2C]; gfn getTactMusic;
    u8 padTact[0x84-0x30]; gfn setTactZev;
};
WWHD_OFFSET(PlayerVirtuals, getTactMusic, 0x2C);
WWHD_OFFSET(PlayerVirtuals, setTactZev, 0x84);
WWHD_SIZE(PlayerVirtuals, 0x88);
}
