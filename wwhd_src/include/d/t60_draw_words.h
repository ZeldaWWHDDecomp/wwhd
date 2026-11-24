/* t60_draw_words.h (WWHD, true 60 build, 2026-11-23): draws that are no J3DModel,
 * drawn at the world's time like the models. Decomp build only. Separate file
 * actor sources keep thin calls.
 *
 * At N > 1 the port draws a converted process's J3DModels at the interpolated world's time W (runtime/src/t60/
 * t60_draw.h: one step back from its newest step at N 2). A draw that is no J3DModel reads guest words its owner wrote
 * on its newest step (the sail cloth's packet matrix and vertices, the boomerang's trail, Link's sword blur): drawn one
 * step ahead of the owner's models (a constant 1/60 s lead at N 2). Around such a draw the block of words goes to the
 * port: begin puts the words of the owner's step at W in their place (kept per step by the port), end puts the live
 * words back. Host call only; the words the logic reads are back before the draw returns. Game build only, N > 1 only.
 * Port switch WWHD_T60_DRAW_WORDS=0: off (drawn at the newest step, the form before).
 *
 * 3D lines (m_Do_ext mDoExt_3DlineMat0_c / Mat1_c update: ropes, chains, the grappling hook's rope, the ship's lines):
 * the update builds the line's vertices from its segment positions at the owner's draw; each line's positions go to
 * the port with owner 0 (the process whose draw runs: only a converted one is aligned). */
#pragma once
#include "true60.h"

#if defined(GABI_GAME_BUILD) && GABI_T60
extern "C" {
__attribute__((visibility("default"))) void wwhd_t60_draw_words_begin(unsigned owner, unsigned ea, unsigned n, int kind);
__attribute__((visibility("default"))) void wwhd_t60_draw_words_end(unsigned ea);
}
#endif

namespace t60dw {
enum Kind { kWhole = 0, kFloats = 1, kMatrix = 2, kFrameEnd = 3, kUnitVecs = 0x80 };
/* a point list that shifts / floats blended / 3x4 matrices / floats drawn between the two frame ends around W when both
 * are kept (the earlier steps' own pictures not drawn; a frame end not kept: as kFloats). kUnitVecs (with kFrameEnd): the
 * block's first third are positions, the last two thirds unit vectors (renormalized after the blend) */
/* (kind | logWord(w): the port's DRAWLOG shows the point at word w of the block) */
static inline int logWord(int w) { return w << 8; }
static inline void begin(u32 owner, const void* block, u32 bytes, int kind) {
#if defined(GABI_GAME_BUILD) && GABI_T60
    if (t60::steps() > 1)
        wwhd_t60_draw_words_begin(owner, gabi::ea(block), bytes / 4, kind);
#else
    (void)owner, (void)block, (void)bytes, (void)kind;
#endif
}
/* begin with the owner from a getter (e.g. dComIfGp_getLinkPlayer), called only when the port takes the words: at
 * N = 1 the draw makes no call the original does not (dt-1 check 024142F0 daPy_swBlur_c::draw, gate 2026-11-24) */
template <class G> static inline void beginOf(G owner, const void* block, u32 bytes, int kind) {
#if defined(GABI_GAME_BUILD) && GABI_T60
    if (t60::steps() > 1)
        wwhd_t60_draw_words_begin(gabi::ea(owner()), gabi::ea(block), bytes / 4, kind);
#else
    (void)owner, (void)block, (void)bytes, (void)kind;
#endif
}
static inline void end(const void* block) {
#if defined(GABI_GAME_BUILD) && GABI_T60
    if (t60::steps() > 1)
        wwhd_t60_draw_words_end(gabi::ea(block));
#else
    (void)block;
#endif
}
/* a line material's lines (mpLines[0..numLines), each numSegments positions): around its update */
template <class L> static inline void linesBegin(L* lines, u32 numLines, u32 numSegments) {
#if defined(GABI_GAME_BUILD) && GABI_T60
    if (t60::steps() <= 1 || !numSegments)
        return;
    for (u32 i = 0; i < numLines; i++)
        if (lines[i].mpSegments.v.get())
            wwhd_t60_draw_words_begin(0, lines[i].mpSegments.v.get(), numSegments * 3, kFloats);
#else
    (void)lines, (void)numLines, (void)numSegments;
#endif
}
template <class L> static inline void linesEnd(L* lines, u32 numLines) {
#if defined(GABI_GAME_BUILD) && GABI_T60
    if (t60::steps() <= 1)
        return;
    for (u32 i = numLines; i-- > 0;)
        if (lines[i].mpSegments.v.get())
            wwhd_t60_draw_words_end(lines[i].mpSegments.v.get());
#else
    (void)lines, (void)numLines;
#endif
}
}  // namespace t60dw
