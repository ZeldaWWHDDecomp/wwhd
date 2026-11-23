/* f_pc_t60_frame_latch.h (WWHD, true 60 build; 2026-11-19): frame-start copies of a few
 * player words for actors EARLIER in the process line than Link (the process-order read rule addendum).
 * Decomp build only. Separate file: f_pc_executor.cpp
 * keeps one thin call.
 *
 * the process-order read rule addendum: when a later actor affects an earlier actor within a frame, reads of the later actor's state
 * (position, flags, procedure, mode) that feed the earlier actor's decisions see its frame-start value. At 30 Hz the ship
 * (0xA5) executes before Link (0xA8), so its frame f reads Link's frame f - 1 end: the player status word
 * (play + 0x5CD8, dComIfGp_checkPlayerStatus0(0, ..): SHIP_RIDE, bow / hookshot / boomerang aim, telescope ..., which
 * Link sets and clears in his execute: initShipRideUseItem, procShipCannon_init, ...) and Link's own words (no-reset
 * flags, procedure, damage wait, room, base animation rate). At N steps the ship's later steps of a frame read them after
 * Link's earlier steps of the same frame.
 *
 * Form (ONE latch, taken at the READER's own turn): the rate gate (fpcEx_Execute) copies
 * the latched words on the frame's first step just before the reader's execute (the ship, 0xA5), and the reader uses the
 * copy on all of the frame's steps: as in A it includes the frame-f writes of actors earlier in the line and excludes
 * those of later ones (Link). A frame whose first step has no reader turn has no copy (live reads). A reader asks for a word by its guest address and size; when the copy is of the
 * running 30 Hz frame and holds that address, the reader gets the frame-start bytes, else (another address, another
 * player, N = 1) the live word. Storage and the functions are defined once, in f_pc_executor.cpp
 * (WWHD_T60_FRAME_LATCH_IMPL), and exported (the fast build makes a unit's hidden definitions local, true60.h). Never
 * consulted at N = 1 (the original). Writes are never redirected: a read-modify-write (the ship's tact flag into Link's
 * no-reset flags) keeps the live word. */
#pragma once
#include <cstring>
#include "true60.h"

#if defined(GABI_GAME_BUILD) && GABI_T60
extern "C" {
/* the rate gate: process `profName` executes; on the frame's first step the reader's execute copies the latched words */
__attribute__((visibility("default"))) void wwhd_t60_flatch_exec(int profName);
/* the reader: 1 and the `size` frame-start bytes at guest address `ea` when the latch holds them, else 0 */
__attribute__((visibility("default"))) int wwhd_t60_flatch_read(uint32_t ea, uint32_t size, void* out);
}
static inline void t60_flatch_exec(s16 profName) {
    if (t60::steps() > 1 && t60::first_step())
        wwhd_t60_flatch_exec(profName);
}
/* the host value of `sizeof(T)` big-endian bytes held on the HOST (the latch copy). Never a be<T> here: be<> is a view of
 * guest memory (its conversion loads from ea(this)), so a be<T> host local reads whatever guest word ea() of the host
 * stack address lands on - the 2026-11-19 fsts60g/i ship mode-0 drop (gabi.h: keep host temporaries as plain values) */
template <typename T> static inline T t60_flatch_host_be(const uint8_t* b) {
    static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4, "t60 frame latch: 1, 2 or 4 byte words");
    uint32_t u = 0;
    for (size_t i = 0; i < sizeof(T); i++)
        u = (u << 8) | b[i];
    T v;
    if constexpr (sizeof(T) == 1) { const uint8_t x = (uint8_t)u; std::memcpy(&v, &x, 1); }
    else if constexpr (sizeof(T) == 2) { const uint16_t x = (uint16_t)u; std::memcpy(&v, &x, 2); }
    else std::memcpy(&v, &u, 4);
    return v;
}
/* the frame-start value of the big-endian word at `ea` (latched), else the live word */
template <typename T> static inline T t60_flatch_load(uint32_t ea) {
    uint8_t b[sizeof(T)];
    if (t60::steps() > 1 && wwhd_t60_flatch_read(ea, sizeof(T), b) != 0)
        return t60_flatch_host_be<T>(b);
    return gabi::load<T>(ea);
}
/* true and the frame-start value of the word at `ea` when the latch holds it; false (out untouched) otherwise: the
 * caller keeps its original read (a virtual getter, a call) */
template <typename T> static inline bool t60_flatch_try(uint32_t ea, T* out) {
    uint8_t b[sizeof(T)];
    if (t60::steps() > 1 && wwhd_t60_flatch_read(ea, sizeof(T), b) != 0) {
        *out = t60_flatch_host_be<T>(b);
        return true;
    }
    return false;
}
#else
static inline void t60_flatch_exec(s16) {}
template <typename T> static inline T t60_flatch_load(uint32_t ea) { return gabi::load<T>(ea); }
template <typename T> static inline bool t60_flatch_try(uint32_t, T*) { return false; }
#endif

#if defined(WWHD_T60_FRAME_LATCH_IMPL) && defined(GABI_GAME_BUILD) && GABI_T60
#include <cstring>
namespace t60flatch {
/* the latched words: play + offset (kPlay) or Link + offset (kLink) */
enum Base { kPlay, kLink };
struct Word {
    Base base;
    uint32_t off;
    uint32_t size;
};
static const Word kWords[] = {
    {kPlay, 0x5CD8, 4}, /* mPlayerStatus[0][0] (dComIfGp_checkPlayerStatus0(0, ..)) */
    {kLink, 0x326, 1},  /* current.roomNo (fopAc_ac_c::current 0x314 + 0x12) */
    {kLink, 0x3BC, 4},  /* daPy_lk_c::mNoResetFlg0 (the ship's units call it mNoResetFlg1 / mNoResetFlags) */
    {kLink, 0x430, 4},  /* the current procedure (checkBowMiniGame: 0x44) */
    {kLink, 0x5898, 4}, /* mFrameCtrlUnder[UNDER_MOVE0_e].mRate (getBaseAnimeFrameRate) */
    {kLink, 0x699E, 2}, /* mDamageWaitTimer */
};
enum { kCount = sizeof(kWords) / sizeof(kWords[0]) };
enum : int { kReader = 0xA5 }; /* the ship (fpcNm_SHIP_e), before Link (0xA8) in the line */
static uint32_t s_ea[kCount];     /* the guest address each word was copied from (0: not copied) */
static uint8_t s_bytes[kCount][4];
static uint32_t s_snap = ~0u;     /* step counter of the frame's first step the copy was taken on */
static uint32_t s_steps = 1;      /* N of that frame */
static uint32_t s_frameFirst = ~0u; /* step counter of the running frame's first step (the frame id; set by the first
                                     * step's first execute at N > 1, so a copy of an earlier frame - before a load,
                                     * a scene change, N = 1 frames - is never taken for this one) */
}  // namespace t60flatch

extern "C" __attribute__((visibility("default"))) void wwhd_t60_flatch_exec(int profName) {
    using namespace t60flatch;
    const uint32_t now = t60_step_counter();
    if (!t60::first_step())
        return;
    s_frameFirst = now; /* the frame id: every first-step execute marks it */
    if (profName != kReader || s_snap == now)
        return;
    s_snap = now;
    s_steps = (uint32_t)t60::steps();
    const uint32_t play = dComIfGp_ea();
    const uint32_t link = gabi::load<uint32_t>(play + PLAY_PLAYERPTR);
    for (int i = 0; i < kCount; i++) {
        const uint32_t base = kWords[i].base == kPlay ? play : link;
        s_ea[i] = base == 0 ? 0 : base + kWords[i].off;
        if (s_ea[i] != 0)
            std::memcpy(s_bytes[i], gabi::at<uint8_t>(s_ea[i]), kWords[i].size);
    }
}

extern "C" __attribute__((visibility("default"))) int wwhd_t60_flatch_read(uint32_t ea, uint32_t size, void* out) {
    using namespace t60flatch;
    /* a copy of the running frame only: taken on this frame's first step (the frame id) and the step within its N steps;
     * else (no copy this frame: the reader had no first-step turn, a load or scene change, the reader's first execute
     * after its creation on a later step) the live word */
    if (s_snap == ~0u || s_snap != s_frameFirst || t60_step_counter() - s_snap >= s_steps || ea == 0)
        return 0;
    for (int i = 0; i < kCount; i++) {
        if (s_ea[i] == ea && kWords[i].size == size) {
            std::memcpy(out, s_bytes[i], size);
            return 1;
        }
    }
    return 0;
}
#endif
