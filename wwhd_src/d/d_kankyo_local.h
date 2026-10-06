/* d_kankyo: shared local helpers of the d_kankyo parts, WWHD. 
 *
 * The environment light (dScnKy_env_light_c, HD 0x120C bytes, vtable pointer at +0x1208) is a
 * function-local static at 10475A68 behind the lazy accessor 02555D0C (guard 104773B4). Most
 * GameCube fields moved: HD offset = GameCube offset + 0x3E8 for the colour/ratio block
 * (addcol colours 0xF58.., all-colour ratio 0xFE4, ...). Offsets are used raw here; the
 * meaning of each is noted where a function establishes it. */
#pragma once
#include "bindings.h"

namespace d_kankyo_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline u32 envlight_l() { return gabi::call<u32>(0x02555D0C); }      /* dKy_getEnvlight (HD lazy static) */
static inline u32 dComIfGp_get_l() { return gabi::call<u32>(0x025200D4); }  /* play object */
static inline u32 operator_new_l(u32 size) { return gabi::call<u32>(0x0273AD10, size); }
static inline void memclr_l(u32 p, u32 size) { gabi::call(0x028F521C, p, size); }  /* HD zero fill (ptr, size) */
static inline void register_global_object_l(u32 p) { gabi::call(0x028F026C, p); }
static inline void JUT_ASSERT_l(u32 file, s32 line, u32 msg) { gabi::call(0x0273AA24, file, line, msg); }
/* save info event flags (dSv_event_c at save+0x644, tmp flags at save+0x1178) */
static inline BOOL isEventBit_l(u32 ev, u32 flag) { return gabi::call<BOOL>(0x025B8B94, ev, flag); }
static inline void onEventBit_l(u32 ev, u32 flag) { gabi::call(0x025B8B68, ev, flag); }
static inline void offEventBit_l(u32 ev, u32 flag) { gabi::call(0x025B8B7C, ev, flag); }
static inline u8 getEventReg_l(u32 ev, u32 reg) { return gabi::call<u8>(0x025B8BB0, ev, reg); }
static inline void setEventReg_l(u32 ev, u32 reg, u32 v) { gabi::call(0x025B8AF4, ev, reg, v); }

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline s16 lds16(u32 a) { return gabi::load<s16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline f32 ldf(u32 a) { return gabi::load<f32>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st16(u32 a, u16 v) { gabi::store<u16>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }
static inline u32 save_l() { return ld(0x101F84DC); }      /* dComIfGs save info */
static inline f32 i2f(s32 v) { return (f32)v; }            /* xoris/0x4330 double, frsp */

static constexpr u32 g_env_light = 0x10475A68;

}  // namespace d_kankyo_cpp
