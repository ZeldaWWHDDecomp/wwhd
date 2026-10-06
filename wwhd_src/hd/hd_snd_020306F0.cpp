/* hd_snd_020306F0: the HD sound interface TU (no GameCube source).
 *
 * TU 020306F0..02031263 (__sinit 020311C4 + fader companion 02031258). The free functions the HD game
 * code uses for sound (159 callers in the GamePad UI and system code): the sound manager's sub-systems
 * (020307E0 SE player, 020307EC BGM/fade controller, 020307F8 HD sound player, 0203103C sound archive),
 * game-mode switch SE and output fades, TV/DRC output modes, menu in/out through JAIZelBasic (101FFC78),
 * player flags +0x1F4..+0x1F8 / mode +0x1FC, HD stream start/is-playing/pause by index, group loading,
 * pause/resume of all sound; plus an RTTI checkDerived and a destructor of the sound heap class.
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_snd_020306F0 {

static u32 Heap_isDerived(u32 self, u32 ti) {
    WWHD_FUNC(0x020306F0, u32, self, ti);
    if (ld(0x101FD648) == 0) {
        st(0x101FD648, 1);
        st(0x101FD958, 0x10004768);
    }
    if (ti == 0x101FD958) return 1;
    if (ld(0x101FD588) == 0) {
        st(0x101FD588, 1);
        st(0x101FD95C, 0x10004758);
    }
    return ti == 0x101FD95C ? 1 : 0;
}
VERIFY(0x020306F0, Heap_isDerived);

static void Heap_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x02030768, void, self, flags);
    if (self == 0) return;
    st(self + 4, 0x1015EBB8);
    gabi::call(0x02766334, self + 0xFC, 2);
    gabi::call(0x02760168, self + 0xA4, 2);
    gabi::call(0x02760168, self + 0x44, 2);
    if (flags & 1) op_delete(self);
}
VERIFY(0x02030768, Heap_dt);

static u32 Snd_se() {
    WWHD_FUNC(0x020307E0, u32);
    return gabi::call<u32>(0x0202BF88, ld(0x1018EC64));
}
VERIFY(0x020307E0, Snd_se);

static u32 Snd_bgm() {
    WWHD_FUNC(0x020307EC, u32);
    return gabi::call<u32>(0x0202BF98, ld(0x1018EC64));
}
VERIFY(0x020307EC, Snd_bgm);

static u32 Snd_player() {
    WWHD_FUNC(0x020307F8, u32);
    return gabi::call<u32>(0x0202BF78, ld(0x1018EC64));
}
VERIFY(0x020307F8, Snd_player);

static u32 Snd_archive() {
    WWHD_FUNC(0x0203103C, u32);
    return gabi::call<u32>(0x0202BF80, ld(0x1018EC64));
}
VERIFY(0x0203103C, Snd_archive);

static inline u32 se() { return gabi::call<u32>(0x020307E0); }
static inline u32 bgm() { return gabi::call<u32>(0x020307EC); }
static inline u32 player() { return gabi::call<u32>(0x020307F8); }

/* fade the BGM controller and the player's master faders (FRAMES, TV, DRC) */
static inline void fade_both(u32 frames, f32 tv, f32 drc) {
    gabi::call(0x02031264, bgm(), frames, tv, drc);
    gabi::call(0x0202F14C, player(), frames, tv, drc);
}

/* 02030804: game-mode switch: optional SE, then fade everything out in 3 frames */
static void Snd_gameModeSwitch(u32 withSe) {
    WWHD_FUNC(0x02030804, void, withSe);
    if (withSe != 0) gabi::call(0x02031920, se(), 0x10004834); /* "SE_GAME_MODE_SWITCH" */
    f32 zero = ldf(0x10004830);
    fade_both(3, zero, zero);
}
VERIFY(0x02030804, Snd_gameModeSwitch);

/* 02030880: output mode 0 TV / 1 GamePad / 2 both (fade 5 frames) */
static void Snd_outputMode_02030880(u32 mode) {
    WWHD_FUNC(0x02030880, void, mode);
    f32 one = ldf(0x10004820), zero = ldf(0x10004830);
    if (mode == 0) fade_both(5, one, zero);
    else if (mode == 1) fade_both(5, zero, one);
    else if (mode == 2) fade_both(5, one, one);
}
VERIFY(0x02030880, Snd_outputMode_02030880);

/* 020309AC: output mode 0 TV / 1 GamePad / 2 both (fade 0 frames) */
static void Snd_outputMode_020309AC(u32 mode) {
    WWHD_FUNC(0x020309AC, void, mode);
    f32 one = ldf(0x10004820), zero = ldf(0x10004830);
    if (mode == 0) fade_both(0, one, zero);
    else if (mode == 1) fade_both(0, zero, one);
    else if (mode == 2) fade_both(0, one, one);
}
VERIFY(0x020309AC, Snd_outputMode_020309AC);

/* 02030AD8: player mode 0 / 1 */
static void Snd_setPlayerMode(u32 mode) {
    WWHD_FUNC(0x02030AD8, void, mode);
    if (mode > 1) return;
    st(player() + 0x1FC, mode == 0 ? 0 : 1);
}
VERIFY(0x02030AD8, Snd_setPlayerMode);

static void Snd_playSe(u32 name) {
    WWHD_FUNC(0x02030B38, void, name);
    gabi::call(0x02031920, se(), name);
}
VERIFY(0x02030B38, Snd_playSe);

/* 02030B6C: set the volume (0, A) and pitch (B) of the playing SE NAME */
static void Snd_setSeParams(u32 name, f32 a, f32 b) {
    WWHD_FUNC(0x02030B6C, void, name, a, b);
    u32 h = gabi::call<u32>(0x020319C8, se(), name);
    if (h == 0 || ld(h) == 0) return;
    gabi::call(0x02885914, ld(h), 0, a);
    if (ld(h) == 0) return;
    gabi::call(0x028859A0, ld(h), b);
}
VERIFY(0x02030B6C, Snd_setSeParams);

static void Snd_bgmUpdate() {
    WWHD_FUNC(0x02030C10, void);
    gabi::call(0x020316E0, bgm());
}
VERIFY(0x02030C10, Snd_bgmUpdate);

static void Snd_menuIn() {
    WWHD_FUNC(0x02030C34, void);
    u32 b = ld(0x101FFC78);
    if (b == 0) return;
    gabi::call(0x0201E5E0, b);
    stb(player() + 0x1F4, 1);
}
VERIFY(0x02030C34, Snd_menuIn);

static void Snd_menuOut() {
    WWHD_FUNC(0x02030C70, void);
    u32 b = ld(0x101FFC78);
    if (b == 0) return;
    gabi::call(0x0201DCD4, b);
    stb(player() + 0x1F4, 0);
}
VERIFY(0x02030C70, Snd_menuOut);

static u32 Snd_get_02030CAC() {
    WWHD_FUNC(0x02030CAC, u32);
    return lbz(player() + 0x1F4);
}
VERIFY(0x02030CAC, Snd_get_02030CAC);

static void Snd_set_02030CD0() {
    WWHD_FUNC(0x02030CD0, void);
    stb(player() + 0x1F5, 1);
}
VERIFY(0x02030CD0, Snd_set_02030CD0);

static void Snd_set_02030CF8() {
    WWHD_FUNC(0x02030CF8, void);
    stb(player() + 0x1F5, 0);
}
VERIFY(0x02030CF8, Snd_set_02030CF8);

static u32 Snd_get_02030D20() {
    WWHD_FUNC(0x02030D20, u32);
    return lbz(player() + 0x1F5);
}
VERIFY(0x02030D20, Snd_get_02030D20);

static void Snd_set_02030D44() {
    WWHD_FUNC(0x02030D44, void);
    stb(player() + 0x1F6, 1);
}
VERIFY(0x02030D44, Snd_set_02030D44);

static void Snd_set_02030D6C() {
    WWHD_FUNC(0x02030D6C, void);
    stb(player() + 0x1F6, 0);
}
VERIFY(0x02030D6C, Snd_set_02030D6C);

static void Snd_set_02030D94() {
    WWHD_FUNC(0x02030D94, void);
    stb(player() + 0x1F7, 1);
    gabi::call(0x02030C10);
}
VERIFY(0x02030D94, Snd_set_02030D94);

static void Snd_set_02030DC0() {
    WWHD_FUNC(0x02030DC0, void);
    stb(player() + 0x1F7, 0);
}
VERIFY(0x02030DC0, Snd_set_02030DC0);

static void Snd_set_02030DE8() {
    WWHD_FUNC(0x02030DE8, void);
    stb(player() + 0x1F8, 1);
}
VERIFY(0x02030DE8, Snd_set_02030DE8);

static void Snd_set_02030E10() {
    WWHD_FUNC(0x02030E10, void);
    stb(player() + 0x1F8, 0);
}
VERIFY(0x02030E10, Snd_set_02030E10);

static void Snd_bgmFlag_02030E38() {
    WWHD_FUNC(0x02030E38, void);
    stb(ld(bgm() + 0x5C) + 0x38, 1);
}
VERIFY(0x02030E38, Snd_bgmFlag_02030E38);

static void Snd_bgmFlag_02030E64() {
    WWHD_FUNC(0x02030E64, void);
    stb(ld(bgm() + 0x5C) + 0x38, 0);
}
VERIFY(0x02030E64, Snd_bgmFlag_02030E64);

static u32 Snd_streamStart(u32 i) {
    WWHD_FUNC(0x02030E90, u32, i);
    static const u32 fn[4] = {0x0202FFB8, 0x02030054, 0x020300F0, 0x0203018C};
    if (i > 3) return 0;
    return gabi::call<u32>(fn[i], player());
}
VERIFY(0x02030E90, Snd_streamStart);

static u32 Snd_streamIsPlaying(u32 i) {
    WWHD_FUNC(0x02030F1C, u32, i);
    static const u32 fn[4] = {0x0202FFF4, 0x02030090, 0x0203012C, 0x020301C8};
    if (i > 3) return 0;
    return gabi::call<u32>(fn[i], player());
}
VERIFY(0x02030F1C, Snd_streamIsPlaying);

static u32 Snd_streamPause(u32 i, u32 a, u32 b) {
    WWHD_FUNC(0x02030FA8, u32, i, a, b);
    static const u32 fn[4] = {0x02030044, 0x020300E0, 0x0203017C, 0x02030218};
    if (i > 3) return i;
    return gabi::call<u32>(fn[i], player(), a, b);
}
VERIFY(0x02030FA8, Snd_streamPause);

static void Snd_loadStatic() {
    WWHD_FUNC(0x02031048, void);
    gabi::call(0x02030468, gabi::call<u32>(0x0203103C));
}
VERIFY(0x02031048, Snd_loadStatic);

static void Snd_loadBgmWds() {
    WWHD_FUNC(0x0203106C, void);
    gabi::call(0x02030474, gabi::call<u32>(0x0203103C));
}
VERIFY(0x0203106C, Snd_loadBgmWds);

static u32 Snd_jaiQuery1() {
    WWHD_FUNC(0x02031090, u32);
    u32 b = ld(0x101FFC78);
    if (b == 0) return 0;
    return gabi::call<u32>(0x02027A7C, b);
}
VERIFY(0x02031090, Snd_jaiQuery1);

static u32 Snd_jaiQuery2() {
    WWHD_FUNC(0x020310A4, u32);
    u32 b = ld(0x101FFC78);
    if (b == 0) return 1;
    return gabi::call<u32>(0x020278E8, b);
}
VERIFY(0x020310A4, Snd_jaiQuery2);

static void Snd_unpause() {
    WWHD_FUNC(0x020310C0, void);
    gabi::call(0x0202FC38, player(), 1);
    gabi::call(0x020316C8, bgm(), 1);
}
VERIFY(0x020310C0, Snd_unpause);

/* 020310F4: reset the HD sound state */
static void Snd_reset() {
    WWHD_FUNC(0x020310F4, void);
    gabi::call(0x020309AC, 0);
    gabi::call(0x02030E38);
    gabi::call(0x02030C70);
    gabi::call(0x02030CF8);
    gabi::call(0x02030D6C);
    gabi::call(0x02030DC0);
    gabi::call(0x020310C0);
    gabi::call(0x02030E10);
    gabi::call(0x02030C10);
}
VERIFY(0x020310F4, Snd_reset);

static void Snd_pauseAll() {
    WWHD_FUNC(0x02031138, void);
    gabi::call(0x0202FB98, player(), 3);
    gabi::call(0x02031698, bgm(), 3);
}
VERIFY(0x02031138, Snd_pauseAll);

static void Snd_resumeAll() {
    WWHD_FUNC(0x0203116C, void);
    gabi::call(0x0202FC28, player(), 3);
    gabi::call(0x020316B8, bgm(), 3);
}
VERIFY(0x0203116C, Snd_resumeAll);

static void Snd_bgmStop() {
    WWHD_FUNC(0x020311A0, void);
    gabi::call(0x020316D4, bgm());
}
VERIFY(0x020311A0, Snd_bgmStop);

static void sinit_020311C4() {
    WWHD_FUNC(0x020311C4, void);
    header_sinit(0x10200454, 0x1018EE08, 0x10004848);
}
VERIFY(0x020311C4, sinit_020311C4);

static u32 Fader_ct1(u32 self) {
    WWHD_FUNC(0x02031258, u32, self);
    return gabi::call<u32>(0x02762170, self, ldf(0x10004850));
}
VERIFY(0x02031258, Fader_ct1);

}  // namespace hd_snd_020306F0
