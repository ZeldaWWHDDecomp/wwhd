/* f_op_actor_iter: actor process iterator (fopAcIt), WWHD. 
 *
 * Translation unit 025D51DC..025D52E4: fopAcIt_Executor, fopAcIt_Judge and the unit's __sinit
 * (025D5254, header statics only). f_op_actor ends with its __sinit at 025D5148 before it.
 * Ported from the GameCube f_op_actor_iter.cpp. g_fopAcTg_Queue (f_op_actor_tag) is at 101F3328. */
#include "bindings.h"

namespace f_op_actor_iter_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline s32 cLsIt_Method_l(u32 list, u32 fn, u32 data) { return gabi::call<s32>(0x020100A0, list, fn, data); }
static inline u32 cLsIt_Judge_l(u32 list, u32 fn, u32 data) { return gabi::call<u32>(0x020100BC, list, fn, data); }

static constexpr u32 g_fopAcTg_Queue = 0x101F3328;
static constexpr u32 cTgIt_MethodCall = 0x0201A9F4;
static constexpr u32 cTgIt_JudgeFilter = 0x0201AA08;

/* method_filter / judge_filter: {func, user data} */
struct filter_l {
    /* 0x0 */ be<u32> mpFunc;
    /* 0x4 */ be<u32> mpUserData;
};

/* 025D51DC */
static s32 fopAcIt_Executor(u32 i_execFunc, u32 i_data) {
    WWHD_FUNC(0x025D51DC, s32, i_execFunc, i_data);
    gabi::Local<filter_l> filter;
    filter->mpFunc = i_execFunc;
    filter->mpUserData = i_data;
    return cLsIt_Method_l(g_fopAcTg_Queue, cTgIt_MethodCall, gabi::ea(filter.get()));
}
VERIFY(0x025D51DC, fopAcIt_Executor);

/* 025D5218 */
static u32 fopAcIt_Judge(u32 i_judgeFunc, u32 i_data) {
    WWHD_FUNC(0x025D5218, u32, i_judgeFunc, i_data);
    gabi::Local<filter_l> filter;
    filter->mpFunc = i_judgeFunc;
    filter->mpUserData = i_data;
    return cLsIt_Judge_l(g_fopAcTg_Queue, cTgIt_JudgeFilter, gabi::ea(filter.get()));
}
VERIFY(0x025D5218, fopAcIt_Judge);

/* 025D5254 */
static void __sinit_f_op_actor_iter_cpp() {
    WWHD_FUNC(0x025D5254, void, (u32)0);
    sinit_header_statics(0x10487440, 0x101F30A8);
}
VERIFY(0x025D5254, __sinit_f_op_actor_iter_cpp);

} // namespace f_op_actor_iter_cpp
