/* Magtail bindings used when another decompiled part is absent. */
#include "d/actor/d_a_mt.h"

// HD: each declaration keeps its weak linkage so a decompiled part replaces the fallback.
extern __attribute__((weak)) void body_control2(mt_class* a);
extern __attribute__((weak)) void body_control3(mt_class* a);
extern __attribute__((weak)) void body_control4(mt_class* a);
extern __attribute__((weak)) void body_control5(mt_class* a);
extern __attribute__((weak)) void mt_move_maru(mt_class* a);
extern __attribute__((weak)) BOOL daMt_Execute(mt_class* a);
extern __attribute__((weak)) void anm_init(mt_class* a, int bck, f32 morf, u8 loopMode, f32 speed, int snd);
extern __attribute__((weak)) void mt_bg_check(mt_class* a);
extern __attribute__((weak)) void tex_anm_set(mt_class* a, u16 idx);
extern __attribute__((weak)) void body_wall_check(mt_class* a);
extern __attribute__((weak)) void bakuha(mt_class* a);
extern __attribute__((weak)) void water_damage_se_set(mt_class* a);

/* WWHD glue (tools/verify/autoglue.py)
ext 021DD634 void body_control2(mt_class* a)
ext 021DE1E0 void body_control3(mt_class* a)
ext 021DE888 void body_control4(mt_class* a)
ext 021DEBC8 void body_control5(mt_class* a)
ext 021DF238 void mt_move_maru(mt_class* a)
ext 021D87A0 BOOL daMt_Execute(mt_class* a)
ext 021D73C0 void anm_init(mt_class* a, int bck, f32 morf, u8 loopMode, f32 speed, int snd)
ext 021D74F8 void mt_bg_check(mt_class* a)
ext 021D75D8 void tex_anm_set(mt_class* a, u16 idx)
ext 021D7E80 void body_wall_check(mt_class* a)
ext 021D8530 void bakuha(mt_class* a)
ext 021D86F8 void water_damage_se_set(mt_class* a)
*/
