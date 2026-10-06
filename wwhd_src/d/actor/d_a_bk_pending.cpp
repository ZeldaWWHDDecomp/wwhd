/* d_a_bk: functions of the translation unit not decompiled yet, as plain guest calls (so the
 * decompiled parts can call them naturally). Weak: a decompiled definition in a part replaces it. */
#include "d/actor/d_a_bk_local.h"

__attribute__((weak)) void smoke_set_s(bk_class* a, f32 rate) { gabi::call(0x02098EEC, a, rate); }
__attribute__((weak)) void ground_smoke_set(bk_class* a) { gabi::call(0x020A070C, a); }
__attribute__((weak)) void way_pos_check(bk_class* a, cXyz* p) { gabi::call(0x02099FF0, a, p); }
__attribute__((weak)) u8 ground_4_check(bk_class* a, int n, s16 ang, f32 r) { return gabi::call<u8>(0x0209A29C, a, n, ang, r); }
__attribute__((weak)) BOOL daBk_other_bg_check(bk_class* a, fopAc_ac_c* b) { return gabi::call<BOOL>(0x0209A4BC, a, b); }
__attribute__((weak)) BOOL daBk_wepon_view_check(bk_class* a) { return gabi::call<BOOL>(0x0209AB64, a); }
__attribute__((weak)) fopAc_ac_c* search_bomb(bk_class* a, BOOL f) { return gabi::call<fopAc_ac_c*>(0x0209A728, a, f); }
__attribute__((weak)) void path_check(bk_class* a, u8 f) { gabi::call(0x0209B354, a, f); }
__attribute__((weak)) void attack_set(bk_class* a, u8 f) { gabi::call(0x0209B760, a, f); }
__attribute__((weak)) void tate_mtx_set(bk_class* a) { gabi::call(0x0209BAD8, a); }
__attribute__((weak)) void bou_mtx_set(bk_class* a) { gabi::call(0x0209BBD4, a); }
__attribute__((weak)) void fight_run(bk_class* a) { gabi::call(0x020A0A98, a); }
__attribute__((weak)) void fight(bk_class* a) { gabi::call(0x020A1AAC, a); }
__attribute__((weak)) void p_lost(bk_class* a) { gabi::call(0x020A27F8, a); }
__attribute__((weak)) void b_nige(bk_class* a) { gabi::call(0x020A2A54, a); }
__attribute__((weak)) void defence(bk_class* a) { gabi::call(0x020A2E04, a); }
__attribute__((weak)) void oshi(bk_class* a) { gabi::call(0x020A2F68, a); }
__attribute__((weak)) void hukki(bk_class* a) { gabi::call(0x020A3048, a); }
__attribute__((weak)) void aite_miru(bk_class* a) { gabi::call(0x020A3624, a); }
__attribute__((weak)) void fail(bk_class* a) { gabi::call(0x020A3704, a); }
__attribute__((weak)) void yogan_fail(bk_class* a) { gabi::call(0x020A38D4, a); }
__attribute__((weak)) void water_fail(bk_class* a) { gabi::call(0x020A3B18, a); }
__attribute__((weak)) void wepon_search(bk_class* a) { gabi::call(0x020A3CC8, a); }
__attribute__((weak)) void d_dozou(bk_class* a) { gabi::call(0x020A4594, a); }
__attribute__((weak)) void carry_drop(bk_class* a) { gabi::call(0x020A47B4, a); }
__attribute__((weak)) void d_mahi(bk_class* a) { gabi::call(0x020A4B44, a); }
__attribute__((weak)) void tubo_wait(bk_class* a) { gabi::call(0x020A4CD4, a); }
__attribute__((weak)) void z_demo_1(bk_class* a) { gabi::call(0x020A5354, a); }
__attribute__((weak)) void b_hang(bk_class* a) { gabi::call(0x020A5674, a); }
__attribute__((weak)) void rope_on(bk_class* a) { gabi::call(0x020A5B74, a); }
__attribute__((weak)) void Bk_move(bk_class* a) { gabi::call(0x020A5EF4, a); }
__attribute__((weak)) BOOL daBk_Execute(bk_class* a) { return gabi::call<BOOL>(0x0209BD1C, a); }
__attribute__((weak)) BOOL useHeapInit(fopAc_ac_c* a) { return gabi::call<BOOL>(0x0209F220, a); }
