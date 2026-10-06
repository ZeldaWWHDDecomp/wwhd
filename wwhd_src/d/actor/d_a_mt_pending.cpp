/* d_a_mt: functions of the translation unit not decompiled yet, as plain guest calls (so the
 * decompiled parts can call them naturally). Weak: a decompiled definition in a part replaces it.
 */
#include "d/actor/d_a_mt.h"

__attribute__((weak)) void body_control2(mt_class* a) { gabi::call(0x021DD634, a); }
__attribute__((weak)) void body_control3(mt_class* a) { gabi::call(0x021DE1E0, a); }
__attribute__((weak)) void body_control4(mt_class* a) { gabi::call(0x021DE888, a); }
__attribute__((weak)) void body_control5(mt_class* a) { gabi::call(0x021DEBC8, a); }
__attribute__((weak)) void mt_move_maru(mt_class* a) { gabi::call(0x021DF238, a); }
__attribute__((weak)) BOOL daMt_Execute(mt_class* a) { return gabi::call<BOOL>(0x021D87A0, a); }
/* decompiled in d_a_mt.cpp; stubs for the part units */
__attribute__((weak)) void anm_init(mt_class* a, int bck, f32 morf, u8 loopMode, f32 speed, int snd) { gabi::call(0x021D73C0, a, bck, morf, loopMode, speed, snd); }
__attribute__((weak)) void mt_bg_check(mt_class* a) { gabi::call(0x021D74F8, a); }
__attribute__((weak)) void tex_anm_set(mt_class* a, u16 idx) { gabi::call(0x021D75D8, a, idx); }
__attribute__((weak)) void body_wall_check(mt_class* a) { gabi::call(0x021D7E80, a); }
__attribute__((weak)) void bakuha(mt_class* a) { gabi::call(0x021D8530, a); }
__attribute__((weak)) void water_damage_se_set(mt_class* a) { gabi::call(0x021D86F8, a); }
