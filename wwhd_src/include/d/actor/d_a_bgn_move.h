#pragma once
#include "d/actor/d_a_bgn.h"
#include "gc_api/f_op/f_op_actor.h"

// HD: word views keep vector payloads and the original load/store order. Ground-checker
// storage uses the shared0x54-byte class; this split source adds no TU-wide definitions.
namespace bgn_move_detail {
struct Words { /* 0x00 */
    be<u32> x; /* 0x04 */
    be<u32> y; /* 0x08 */
    be<u32> z;
};
WWHD_SIZE(Words, 0xC);
inline Words *words(cXyz *p) { return gabi::at<Words>(gabi::ea(p)); }
inline const Words *words(const cXyz *p) { return gabi::at<Words>(gabi::ea(p)); }
// HD: this TU's base vtable identities restored by the inlined destructor.
enum : u32 {
    gnd_cBgS_PolyInfo_vt = 0x10008A64,
    gnd_dBgS_PolyPassChk_vt = 0x10008A84,
    gnd_dBgS_GrpPassChk_vt = 0x10008A44
};
enum : u32 {
    JA_SE_LK_ARROW_HIT = 0x2879,
    ID_AK_SN_KGTBREAKWEAKPOINT00 = 0x8457,
    ID_AK_SN_KGTBREAKWEAKPOINT01 = 0x8458,
    JA_SE_CV_BGN_HIT_2 = 0x496F,
    JA_SE_LK_LAST_HIT = 0x2828,
    JA_SE_CM_BGN_M_BRK_ORB = 0x5983,
    ID_AK_SN_KGTHITWEAKPOINT00 = 0x8459,
    JA_SE_CV_BGN_HIT_1 = 0x496E,
    ID_AK_JN_CCTHUNDER00 = 0x3ED,
    ID_AK_JN_CCTHUNDER01 = 0x3EE,
    ID_AK_SN_O_KGTCOMMONHAMON03 = 0x8443,
    JA_SE_CM_BGN_BODY_RIPPLE = 0x6A43,
};
} // namespace bgn_move_detail
void def_se_set(fopAc_ac_c *, u32, u32);
void mDoAud_bgmStop(u32);
void mDoAud_monsSeStart(u32, cXyz *, u32, s32);
void bgn_move_chk_dt(dBgS_GndChk *, s32);
