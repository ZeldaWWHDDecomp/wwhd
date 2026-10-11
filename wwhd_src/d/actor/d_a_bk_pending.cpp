// Bokoblin guest bindings shared by the split translation unit.
// Strong implementations in the verified parts override these weak fallbacks.
#include "d/actor/d_a_bk_local.h"

// HD: declaration attributes carry weak linkage into the generated bindings.
extern __attribute__((weak)) void smoke_set_s(bk_class* i_this, f32 rate);
extern __attribute__((weak)) void ground_smoke_set(bk_class* i_this);
extern __attribute__((weak)) void way_pos_check(bk_class* i_this, cXyz* r31);
extern __attribute__((weak)) u8 ground_4_check(bk_class* i_this, int r18, s16 r20, f32 f29);
extern __attribute__((weak)) BOOL daBk_other_bg_check(bk_class* i_this, fopAc_ac_c* r23);
extern __attribute__((weak)) BOOL daBk_wepon_view_check(bk_class* i_this);
extern __attribute__((weak)) fopAc_ac_c* search_bomb(bk_class* i_this, BOOL r26);
extern __attribute__((weak)) void path_check(bk_class* i_this, u8 r19);
extern __attribute__((weak)) void attack_set(bk_class* i_this, u8 r28);
extern __attribute__((weak)) void tate_mtx_set(bk_class* i_this);
extern __attribute__((weak)) void bou_mtx_set(bk_class* i_this);
extern __attribute__((weak)) void fight_run(bk_class* i_this);
extern __attribute__((weak)) void fight(bk_class* i_this);
extern __attribute__((weak)) void p_lost(bk_class* i_this);
extern __attribute__((weak)) void b_nige(bk_class* i_this);
extern __attribute__((weak)) void defence(bk_class* i_this);
extern __attribute__((weak)) void oshi(bk_class* i_this);
extern __attribute__((weak)) void hukki(bk_class* i_this);
extern __attribute__((weak)) void aite_miru(bk_class* i_this);
extern __attribute__((weak)) void fail(bk_class* i_this);
extern __attribute__((weak)) void yogan_fail(bk_class* i_this);
extern __attribute__((weak)) void water_fail(bk_class* i_this);
extern __attribute__((weak)) void wepon_search(bk_class* i_this);
extern __attribute__((weak)) void d_dozou(bk_class* i_this);
extern __attribute__((weak)) void carry_drop(bk_class* i_this);
extern __attribute__((weak)) void d_mahi(bk_class* i_this);
extern __attribute__((weak)) void tubo_wait(bk_class* i_this);
extern __attribute__((weak)) void z_demo_1(bk_class* i_this);
extern __attribute__((weak)) void b_hang(bk_class* i_this);
extern __attribute__((weak)) void rope_on(bk_class* i_this);
extern __attribute__((weak)) void Bk_move(bk_class* i_this);
extern __attribute__((weak)) BOOL daBk_Execute(bk_class* i_this);
extern __attribute__((weak)) BOOL useHeapInit(fopAc_ac_c* i_actor);

/* WWHD glue
ext 02098EEC void smoke_set_s(bk_class* i_this, f32 rate)
ext 020A070C void ground_smoke_set(bk_class* i_this)
ext 02099FF0 void way_pos_check(bk_class* i_this, cXyz* r31)
ext 0209A29C u8 ground_4_check(bk_class* i_this, int r18, s16 r20, f32 f29)
ext 0209A4BC BOOL daBk_other_bg_check(bk_class* i_this, fopAc_ac_c* r23)
ext 0209AB64 BOOL daBk_wepon_view_check(bk_class* i_this)
ext 0209A728 fopAc_ac_c* search_bomb(bk_class* i_this, BOOL r26)
ext 0209B354 void path_check(bk_class* i_this, u8 r19)
ext 0209B760 void attack_set(bk_class* i_this, u8 r28)
ext 0209BAD8 void tate_mtx_set(bk_class* i_this)
ext 0209BBD4 void bou_mtx_set(bk_class* i_this)
ext 020A0A98 void fight_run(bk_class* i_this)
ext 020A1AAC void fight(bk_class* i_this)
ext 020A27F8 void p_lost(bk_class* i_this)
ext 020A2A54 void b_nige(bk_class* i_this)
ext 020A2E04 void defence(bk_class* i_this)
ext 020A2F68 void oshi(bk_class* i_this)
ext 020A3048 void hukki(bk_class* i_this)
ext 020A3624 void aite_miru(bk_class* i_this)
ext 020A3704 void fail(bk_class* i_this)
ext 020A38D4 void yogan_fail(bk_class* i_this)
ext 020A3B18 void water_fail(bk_class* i_this)
ext 020A3CC8 void wepon_search(bk_class* i_this)
ext 020A4594 void d_dozou(bk_class* i_this)
ext 020A47B4 void carry_drop(bk_class* i_this)
ext 020A4B44 void d_mahi(bk_class* i_this)
ext 020A4CD4 void tubo_wait(bk_class* i_this)
ext 020A5354 void z_demo_1(bk_class* i_this)
ext 020A5674 void b_hang(bk_class* i_this)
ext 020A5B74 void rope_on(bk_class* i_this)
ext 020A5EF4 void Bk_move(bk_class* i_this)
ext 0209BD1C BOOL daBk_Execute(bk_class* i_this)
ext 0209F220 BOOL useHeapInit(fopAc_ac_c* i_actor)
*/
