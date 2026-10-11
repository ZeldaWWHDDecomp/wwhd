/* NPC projectile, WWHD layout.
 * HD: actor base is 0x3AC, shifting the tww projectile fields by 0x11C. */
#pragma once
#include "gc_api/gc_api.h"
struct daTama_c : fopAc_ac_c {
    BOOL createInit();
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();

    /* 0x3AC */ be<u32> mPartnerID;
    /* 0x3B0 */ dBgS_ObjAcch mAcch;
    /* 0x574 */ dBgS_AcchCir mAcchCir;
    /* 0x5B4 */ dCcD_Stts mStts;
    /* 0x5F0 */ dCcD_Sph mSph;
    /* 0x71C */ be<f32> mDis;
};
WWHD_OFFSET(daTama_c, mAcchCir, 0x574);
WWHD_OFFSET(daTama_c, mSph, 0x5F0);
WWHD_OFFSET(daTama_c, mDis, 0x71C);

WWHD_SIZE(daTama_c, 0x720);
WWHD_OFFSET(daTama_c, mPartnerID, 0x3AC);
WWHD_OFFSET(daTama_c, mAcch, 0x3B0);
WWHD_OFFSET(daTama_c, mStts, 0x5B4);

// HD: only the partner notification is accessed here; retain tww's unknown field name.
struct TamaPartner : fopAc_ac_c {
    u8 mPartnerState[0x7D6 - sizeof(fopAc_ac_c)];
    be<s16> field_0x6ba;
};
WWHD_OFFSET(TamaPartner, field_0x6ba, 0x7D6);
WWHD_SIZE(TamaPartner, 0x7D8);
dCcD_GObjInf* tamaGetCoHitObj(dCcD_GObjInf*);
void tamaSphere_ct(dCcD_Sph*);
void tamaSphere_dt(dCcD_Sph*, u32);
