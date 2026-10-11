/* d_a_bb: the translation unit's functions as plain guest calls, weak, so each part (d_a_bb.cpp,
 * d_a_bb_move.cpp, a future d_a_bb_exec.cpp) can call the others naturally; a decompiled
 * definition linked into the same unit replaces the stub. */
#include "d/actor/d_a_bb.h"

// The separate execution/movement specs bind the owner TU's single HIO object.
// The owner source supplies the strong reference when the full TU is linked.
__attribute__((weak)) bbHIO_c& l_bbHIO = gabi::object_at<bbHIO_c>(0x104615AC);

BOOL bbGuest_daBb_Execute(bb_class* a);
void bbGuest_tail_control(bb_class* a);
void bbGuest_tex_anm_set(bb_class* a, u16 idx);
void bbGuest_anm_init(bb_class* a, int b, f32 m, u8 l, f32 s, int c);
esa_class* bbGuest_search_esa(bb_class* a);
void bbGuest_kuti_open(bb_class* a, s16 b, u32 c);
BOOL bbGuest_bb_player_bg_check(bb_class* a);
s32 bbGuest_bb_player_view_check(bb_class* a);
void bbGuest_path_check(bb_class* a);
void bbGuest_bb_pos_move(bb_class* a);
void bbGuest_bb_ground_pos_move(bb_class* a);
void bbGuest_bb_auto_move(bb_class* a);
void bbGuest_bb_water_check(bb_class* a);
void bbGuest_bb_wait_move(bb_class* a);
void bbGuest_bb_su_wait_move(bb_class* a);
void bbGuest_damage_check(bb_class* a);

__attribute__((weak)) BOOL daBb_Execute(bb_class* a) { return bbGuest_daBb_Execute(a); }
__attribute__((weak)) void tail_control(bb_class* a) { bbGuest_tail_control(a); }
__attribute__((weak)) void tex_anm_set(bb_class* a, u16 idx) { bbGuest_tex_anm_set(a, idx); }
__attribute__((weak)) void anm_init(bb_class* a, int b, f32 m, u8 l, f32 s, int c) { bbGuest_anm_init(a, b, m, l, s, c); }
__attribute__((weak)) esa_class* search_esa(bb_class* a) { return bbGuest_search_esa(a); }
__attribute__((weak)) void kuti_open(bb_class* a, s16 b, u32 c) { bbGuest_kuti_open(a, b, c); }
__attribute__((weak)) BOOL bb_player_bg_check(bb_class* a) { return bbGuest_bb_player_bg_check(a); }
__attribute__((weak)) s32 bb_player_view_check(bb_class* a) { return bbGuest_bb_player_view_check(a); }
__attribute__((weak)) void path_check(bb_class* a) { bbGuest_path_check(a); }
__attribute__((weak)) void bb_pos_move(bb_class* a) { bbGuest_bb_pos_move(a); }
__attribute__((weak)) void bb_ground_pos_move(bb_class* a) { bbGuest_bb_ground_pos_move(a); }
__attribute__((weak)) void bb_auto_move(bb_class* a) { bbGuest_bb_auto_move(a); }
__attribute__((weak)) void bb_water_check(bb_class* a) { bbGuest_bb_water_check(a); }
__attribute__((weak)) void bb_wait_move(bb_class* a) { bbGuest_bb_wait_move(a); }
__attribute__((weak)) void bb_su_wait_move(bb_class* a) { bbGuest_bb_su_wait_move(a); }
__attribute__((weak)) void damage_check(bb_class* a) { bbGuest_damage_check(a); }

/* WWHD glue
ext 0205EA58 BOOL bbGuest_daBb_Execute(bb_class* a)
ext 0205D4BC void bbGuest_tail_control(bb_class* a)
ext 0205D964 void bbGuest_tex_anm_set(bb_class* a, u16 idx)
ext 0205DA3C void bbGuest_anm_init(bb_class* a, int b, f32 m, u8 l, f32 s, int c)
ext 0205DCFC esa_class* bbGuest_search_esa(bb_class* a)
ext 0205DE54 void bbGuest_kuti_open(bb_class* a, s16 b, u32 c)
ext 0205DE74 BOOL bbGuest_bb_player_bg_check(bb_class* a)
ext 0205DFC0 s32 bbGuest_bb_player_view_check(bb_class* a)
ext 0205E14C void bbGuest_path_check(bb_class* a)
ext 0205E638 void bbGuest_bb_pos_move(bb_class* a)
ext 0205E8AC void bbGuest_bb_ground_pos_move(bb_class* a)
ext 020625E8 void bbGuest_bb_auto_move(bb_class* a)
ext 02063404 void bbGuest_bb_water_check(bb_class* a)
ext 0206348C void bbGuest_bb_wait_move(bb_class* a)
ext 02063BDC void bbGuest_bb_su_wait_move(bb_class* a)
ext 0206432C void bbGuest_damage_check(bb_class* a)
*/
