/* d_a_bb: the translation unit's functions as plain guest calls, weak, so each part (d_a_bb.cpp,
 * d_a_bb_move.cpp, a future d_a_bb_exec.cpp) can call the others naturally; a decompiled
 * definition linked into the same unit replaces the stub. */
#include "d/actor/d_a_bb.h"

__attribute__((weak)) BOOL daBb_Execute(bb_class* a) { return gabi::call<BOOL>(0x0205EA58, a); }
__attribute__((weak)) void tail_control(bb_class* a) { gabi::call(0x0205D4BC, a); }
__attribute__((weak)) void tex_anm_set(bb_class* a, u16 idx) { gabi::call(0x0205D964, a, idx); }
__attribute__((weak)) void anm_init(bb_class* a, int b, f32 m, u8 l, f32 s, int c) { gabi::call(0x0205DA3C, a, b, m, l, s, c); }
__attribute__((weak)) esa_class* search_esa(bb_class* a) { return gabi::call<esa_class*>(0x0205DCFC, a); }
__attribute__((weak)) void kuti_open(bb_class* a, s16 b, u32 c) { gabi::call(0x0205DE54, a, b, c); }
__attribute__((weak)) BOOL bb_player_bg_check(bb_class* a) { return gabi::call<BOOL>(0x0205DE74, a); }
__attribute__((weak)) s32 bb_player_view_check(bb_class* a) { return gabi::call<s32>(0x0205DFC0, a); }
__attribute__((weak)) void path_check(bb_class* a) { gabi::call(0x0205E14C, a); }
__attribute__((weak)) void bb_pos_move(bb_class* a) { gabi::call(0x0205E638, a); }
__attribute__((weak)) void bb_ground_pos_move(bb_class* a) { gabi::call(0x0205E8AC, a); }
__attribute__((weak)) void bb_auto_move(bb_class* a) { gabi::call(0x020625E8, a); }
__attribute__((weak)) void bb_water_check(bb_class* a) { gabi::call(0x02063404, a); }
__attribute__((weak)) void bb_wait_move(bb_class* a) { gabi::call(0x0206348C, a); }
__attribute__((weak)) void bb_su_wait_move(bb_class* a) { gabi::call(0x02063BDC, a); }
__attribute__((weak)) void damage_check(bb_class* a) { gabi::call(0x0206432C, a); }
