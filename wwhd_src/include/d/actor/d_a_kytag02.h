/* Wind rail tag. GameCube actor fields move by +0x11C in HD.
 */
#pragma once
#include "bindings.h"
namespace kytag02_path {
struct dPnt {
    be<u8> mArg0, mArg1, mArg2, mArg3;
    cXyz m_position;
};
struct dPath {
    be<u16> m_num, m_nextID;
    be<u8> mArg0, m_closed, field4_0x6, field5_0x7;
    gptr<dPnt> m_points;
};
WWHD_SIZE(dPnt, 0x10);
WWHD_SIZE(dPath, 0xC);
}
struct kytag02_class : fopAc_ac_c {
    enum : u32 { __vt = 0x10013AA8 }; // HD: virtual destructor table.
    /* 0x3AC */ gptr<kytag02_path::dPath> mpPath;
    /* 0x3B0 */ cXyz mWindVec;
};
WWHD_OFFSET(kytag02_class, mpPath, 0x3AC);
WWHD_OFFSET(kytag02_class, mWindVec, 0x3B0);
WWHD_SIZE(kytag02_class, 0x3BC);
struct kytag02_play_view {
    u8 _0000[0x5B2C];
    gptr<fopAc_ac_c> mPlayer;
};
struct kytag02_env_view {
    u8 _000[0xA08];
    struct Wind {
        gptr<cXyz> mpWindVecOverride;
        u8 _004[0x14 - 4];
        be<f32> mWindStrengthOverride;
    } mWind;
};
WWHD_OFFSET(kytag02_env_view, mWind.mpWindVecOverride, 0xA08);
WWHD_OFFSET(kytag02_env_view, mWind.mWindStrengthOverride, 0xA1C);
inline kytag02_env_view* kytag02_getEnvlight() {
    return reinterpret_cast<kytag02_env_view*>(dKy_getEnvlight());
}
kytag02_path::dPath* kytag02_nextPath(kytag02_path::dPath* path, s32 room);
void dKyr_get_vectle_calc(cXyz* from, cXyz* to, cXyz* direction);
