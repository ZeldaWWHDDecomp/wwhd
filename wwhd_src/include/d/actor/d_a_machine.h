// Wind Temple biting blade trap.
#pragma once
#include "gc_api/gc_api.h"
namespace machine_layout {
struct dPnt {
    u8 mArgs[4];
    cXyz m_position;
};
struct dPath {
    be<u16> m_num, m_nextID;
    be<u8> mArg0, m_closed;
    u8 _06[2];
    gptr<dPnt> m_points;
};
struct WindMill : fopAc_ac_c {
    u8 _3AC[0x1534 - 0x3AC];
    be<u8> mType;
    u8 _1535;
    be<s16> mAngle[3];
};
} // namespace machine_layout
struct daMachine_c : fopAc_ac_c {
    BOOL CreateHeap();
    void CreateInit();
    void set_mtx();
    bool _delete();
    bool _draw();
    bool _execute();
    cPhs_State _create();
    machine_layout::WindMill* search_wind_mill();
    void set_speed();
    void set_next_pnt();
    void path_move();
    void set_cube();
    void attack();
    void set_body();
    void set_at();
    /* HD: grown actor and animation objects retain tww's member names/order. */
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ dCcD_Sph mSph;
    /* 0x520 */ dCcD_Sph field_0x404[3];
    /* 0x8A4 */ dCcD_Sph field_0x788;
    /* 0x9D0 */ dBgS_ObjAcch field_0x84b;
    /* 0xB94 */ dBgS_AcchCir field_0xa78;
    /* 0xBD4 */ dCcD_Cps field_0xab8;
    /* 0xD0C */ cM3dGSph field_0xbf0;
    /* 0xD20 */ mDoExt_bckAnm field_0xc04;
    /* 0xDAC */ be<u8> field_0xc14;
    /* 0xDAD */ be<s8> field_0xc15;
    /* 0xDAE */ be<s8> field_0xc16;
    /* 0xDAF */ be<s8> field_0xc17;
    /* 0xDB0 */ gptr<machine_layout::dPath> field_0xc18;
    /* 0xDB4 */ cXyz field_0xc1c;
    /* 0xDC0 */ cXyz field_0xc28;
    /* 0xDCC */ u8 field_0xc34[8];
    /* 0xDD4 */ Mtx34 field_0xc3c;
    /* 0xE04 */ cXyz field_0xc6c;
    /* 0xE10 */ be<u8> field_0xc78;
    /* 0xE11 */ u8 field_0xC79[0x1B];
};
WWHD_OFFSET(daMachine_c, field_0xc04, 0xD20);
WWHD_OFFSET(daMachine_c, field_0xc3c, 0xDD4);
WWHD_SIZE(daMachine_c, 0xE2C);
