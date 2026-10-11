/* gc_api: the GameCube decompilation's API (names and signatures of zeldaret/tww) mapped to the
 * WWHD functions and data, for sources written in GameCube style (tools/verify/autoglue.py).
 *
 * Organisation: one header per GameCube header (gc_api/<GC path>), each holding only GameCube
 * names; HD differences are marked "HD:". Shared layouts stay in the existing headers
 * (f_op/f_op_actor.h, d/d_npc.h, d/d_bg_s.h, m_Do/m_Do_ext.h, JSystem/J3DGraph.h), which gained
 * the GameCube-named members these sources use. Guest objects are passed by pointer; a host
 * string literal or a decompiled function's name is turned into its guest address when it is
 * passed to a guest call (gabi::guest_str / gabi::guest_fn).
 *
 * Units written with WWHD_FUNC/bindings directly are not affected: these headers are opt-in
 * (they define GameCube enum names that some older unit headers define locally).
 *
 * Per translation unit (GHS emits a copy per TU): vtables of inline-constructed classes and the
 * sead::SafeString vtable. A unit defines them once in `gc_tu` (gc_api/tu.h). */
#pragma once
#include "bindings.h"
#include "gc_api/tu.h"
#include "gc_api/SSystem/c_lib.h"
#include "gc_api/SSystem/c_math.h"
#include "gc_api/SSystem/c_phase.h"
#include "gc_api/m_Do/m_Do_mtx.h"
#include "gc_api/JSystem/J3DModelData.h"
#include "gc_api/m_Do/m_Do_ext.h"
#include "gc_api/m_Do/m_Do_audio.h"
#include "gc_api/JSystem/JUTAssert.h"
#include "gc_api/JSystem/JPAMath.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/f_op/f_op_msg.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_bg_s.h"
#include "gc_api/d/d_item.h"
#include "gc_api/d/d_kankyo.h"
#include "gc_api/d/d_kankyo_wether.h"
#include "gc_api/d/d_lib.h"
#include "gc_api/d/d_particle.h"
#include "gc_api/d/d_npc.h"
#include "gc_api/d/actor/d_a_player.h"
#include "gc_api/d/actor/d_a_shop_item.h"
#include "gc_api/d/actor/d_a_esa.h"
#include "gc_api/d/actor/d_a_tsubo.h"
#include "gc_api/d/actor/d_a_obj_barrel.h"

/* float arithmetic as GHS emitted it: where GHS fused a*b+c, the source says so (the recompiler
 * rounds the fused result once more to single; see tools/verify/README.md) */
using gabi::fmadds;
using gabi::fmsubs;
using gabi::fnmadds;
using gabi::fnmsubs;
/* float -> integer conversion (fctiwz: truncating, saturating, NaN -> INT_MIN); a C++ cast of an
 * out-of-range float is undefined, so GameCube `(int)f` is written ftoi(f) */
using gabi::ftoi;

#define FLOAT_MAX 3.4028235e+38f
struct ResTIMG;
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#ifndef NULL
#define NULL nullptr
#endif
