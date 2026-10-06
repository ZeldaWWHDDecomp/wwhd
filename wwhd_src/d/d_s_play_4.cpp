/* d_s_play part 4: phase_0 and phase_1. WWHD. 
 * See d_s_play.cpp for the unit's range and layouts.
 *
 * HD phase_0 does not mount the LkD archive through the DVD thread: it formats the archive name
 * ("LkD%02d") into a sead::FixedSafeString and copies it into a play-object buffer
 * (play+0x5A50, capacity play+0x5A58). HD phase_1 sets up the "Stage" resources and then an HD
 * system on the Zelda heap instead of the ice material control. */
#include "bindings.h"

namespace d_s_play_4_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void JUT_ASSERT_l(u32 file, s32 line, u32 msg) { gabi::call(0x0273AA24, file, line, msg); }

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st16(u32 a, u16 v) { gabi::store<u16>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }

struct sstr_l { be<u32> str, vt; };
struct fixedstr8_l { be<u32> str, vt, size; u8 buf[8]; }; /* sead::FixedSafeString<8> */

/* 025B1418: HD: see the file comment */
static s32 phase_0(u32 i_this) {
    WWHD_FUNC(0x025B1418, s32, i_this);
    if (gabi::call<s32>(0x025E18D0) != 0) return 0; /* mDoAud_checkAllWaveLoadStatus */
    u32 darcIdx = gabi::call<BOOL>(0x025B8B94, ld(0x101F84DC) + 0x644, 0x2D01) != 0; /* dComIfGs_isEventBit(0x2D01) */
    if (darcIdx == (u32)(s32)(s8)ld8(dComIfGp_ea() + 0x5AC8)) return 2;  /* dComIfGp_getLkDemoAnmNo */
    st8(dComIfGp_ea() + 0x5AC8, (u8)darcIdx);                           /* dComIfGp_setLkDemoAnmNo */
    gabi::Local<fixedstr8_l> name;
    s32 no = (s8)ld8(dComIfGp_ea() + 0x12A0 + 0x4828);
    gabi::call(0x025B3614, gabi::ea(name.get()), 0x1005328C, no); /* FixedSafeString<8>("LkD%02d", no) */
    gabi::call_ptr(ld(name->vt + 0x14), gabi::ea(name.get()));     /* assureTermination */
    u32 str = name->str;
    u32 play = dComIfGp_ea();
    gabi::Local<sstr_l> s;
    s->vt = 0x10052EEC;
    u32 base = play + 0x12A0;
    s->str = str;
    u32 dst = ld(play + 0x5A50);
    gabi::call(0x025B376C, gabi::ea(s.get()));
    /* the SafeString length (0 if longer than 0x40000) */
    u32 p = s->str;
    s32 len = 0;
    if (ld8(p) != 0) {
        for (;;) {
            len++;
            p++;
            if (len > 0x40000) { len = 0; break; }
            if (ld8(p) == 0) break;
        }
    }
    s32 max = (s32)ld(base + 0x47B8);
    if (!(len < max)) len = max - 1;
    gabi::call_ptr(ld(s->vt + 0x14), gabi::ea(s.get()));
    gabi::call(0xC0009988, dst, (u32)s->str, len, 0); /* OSBlockMove (import) */
    st8(dst + len, 0);
    return 2;
}
VERIFY(0x025B1418, phase_0);

/* 025B1590: HD: see the file comment */
static s32 phase_1(u32 i_this) {
    WWHD_FUNC(0x025B1590, s32, i_this);
    st(0x1047E6C4, i_this != 0 ? ld(i_this + 4) : (u32)-1); /* dStage_roomControl_c::setProcID(fopScnM_GetID(this)) */
    u32 next = dComIfGp_ea() + 0x5140;
    u32 play = dComIfGp_ea();
    for (int i = 0; i < 6; i++) st16(play + 0x5134 + 2 * i, ld16(next + 2 * i)); /* setStartStage(getNextStartStage()) */
    st8(dComIfGp_ea() + 0x514C, 0);                                            /* offEnableNextStage */
    u32 stage = dComIfGp_ea() + 0x5134;
    s32 room = (s8)ld8(dComIfGp_ea() + 0x12A0 + 0x3E9E);
    gabi::call(0x027EC938, 0x100532A4, stage, room); /* JUTReportConsole_f("Start StageName:RoomNo [%s:%d]\n") */
    st16(dComIfGp_ea() + 0x5ACA, 0);                 /* dComIfGp_setStatus(0) */
    s32 rt = gabi::call<s32>(0x02523A04, 0x1005329C, 0); /* dComIfG_setStageRes("Stage", NULL) */
    if (rt != 1) JUT_ASSERT_l(0x100532C4, 0x1042, 0x10053294);
    gabi::call(0x0259149C, gabi::call<u32>(0x025EE048));
    return 2;
}
VERIFY(0x025B1590, phase_1);

} // namespace d_s_play_4_cpp
