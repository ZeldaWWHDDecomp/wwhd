/* Attention tag, WWHD layout. GameCube actor fields move by +0x11C.
 */
#pragma once
#include "gc_api/gc_api.h"

struct dAttLook_c {
    void requestF(fopAc_ac_c* actor, s32 angle, s32 type);
};
// HD: view of the play object's look-request work used by this actor.
struct tag_attention_play_view {
    /* 0x0000 */ u8 _0000[0x595C];
    /* 0x595C */ dAttLook_c mLook;
};
WWHD_OFFSET(tag_attention_play_view, mLook, 0x595C);
void dCcD_Sph_construct(dCcD_Sph* sphere);
void dCcD_Sph_dt(dCcD_Sph* sphere, s32 flags);

namespace daTagAttention {
class Act_c : public fopAc_ac_c {
public:
    enum : u32 { __vt = 0x1003EF28 }; // HD: virtual destructor vtable
    enum Prm_e {
        PRM_TYPE_W = 0x02,
        PRM_TYPE_S = 0x08,
        PRM_SWSAVE_W = 0x08,
        PRM_SWSAVE_S = 0x00,
    };
    int prm_get_Type();
    int prm_get_swSave();
    cPhs_State _create();
    bool _execute();

    /* 0x3AC */ be<u8> m_b0x290;
    /* 0x3AD */ u8 _3AD[0x3B8 - 0x3AD];
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ dCcD_Sph mSph;
};
WWHD_OFFSET(Act_c, m_b0x290, 0x3AC);
WWHD_OFFSET(Act_c, mStts, 0x3B8);
WWHD_OFFSET(Act_c, mSph, 0x3F4);
WWHD_SIZE(Act_c, 0x520);
} // namespace daTagAttention
