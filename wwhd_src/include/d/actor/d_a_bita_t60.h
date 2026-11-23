/* d_a_bita_t60.h (WWHD, true 60 build, 2026-11-19): Gohma's wooden platforms
 * (d_a_bita.cpp, bita_class) on a general step length (dt = 1/N). Decomp build only.
 * Separate file: the actor's sources keep thin calls.
 *
 * the whole-frame form rule, whole form (the Magtail's form, as Gohma's btd): the frame reads Gohma (btd's mDemoCamMode: the
 * board's removal near the demo camera), the tick30 collision hit (mCyl's Tg hit by a fire At: the burn start), its
 * burn timer mTimer (20: burn SE, 88: break SE + particle, 90: delete; base_mtx_set's brk play below 60) and one
 * cM_rndF on the burn start; it registers its cylinder (dComIfG_Ccsp_Set) and moves its background (mpBgW Move,
 * dBgS_MoveBGProc_Typical: the platform is a ride party, rule 14). So daBita_Execute runs whole once per 30 Hz frame on
 * the tick30 step at dt 1 from the frame-start state (d/t60_whole.h, t60objw form, n 0); the earlier steps change no
 * logic state; the background moves once per frame.
 * The picture: frame interpolation (the board / burning-board model drawn by this process: blended between the frame ends; a
 * picture step's draw is kept and put back, so the draw's brk entry and mpTevRegAnimation reset leave nothing behind).
 * daBita_Draw: settingTevStruct on the tick30 step's draw only (rule 14).
 * daBita_Create calls base_mtx_set and the BgW Move itself, not the execute. Never held.
 * dt 1: the original. Negative control: T60C_NEGCTRL runs the execute on every step at dt. */
#pragma once
#include "d/t60_objw.h"

struct bita_class;
namespace bita_t60 {
static inline const t60w::Ops<bita_class>& ops() { return t60objw::Logic<bita_class>::kOps; }
/* the draw's light state: the tick30 step's draw only (rule 14) */
static inline bool drawStep() { return t60objw::drawStep(); }
}  // namespace bita_t60
