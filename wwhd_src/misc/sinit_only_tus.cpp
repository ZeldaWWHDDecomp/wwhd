/* Sinit-only translation units (WWHD): fifteen TUs whose only code is the 148-byte header
 * __sinit every game TU gets (zero a 16-byte object, register it, store {-pi, pi}, construct and
 * register two one-byte objects). Their other contents are data, or code GHS inlined into the
 * callers. Each sits between two TUs that already have their own __sinit (see the comments), so
 * it is a TU of its own.
 * Verified against cking.rpx. */
#include "bindings.h"

namespace sinit_only_tus_cpp {

/* 024F1BB0 __sinit: sinit-only TU between d_bg_s_lin_chk (own __sinit 024F1B1C) and d_bg_s_movebg_actor. The header statics block at
 * 1046ED60 (zeroed 16-byte object at +0xC, {-pi, pi} from 10043BA0, two one-byte objects at +8/+9),
 * each registered for destruction (records 101D52B8, +0xC, +0x18). */
static void sinit_tu_024F1BB0() {
    WWHD_FUNC(0x024F1BB0, void);
    const u32 bss = 0x1046ED60, rec = 0x101D52B8, ro = 0x10043BA0;
    gabi::store<u32>(bss + 0x14, 0); gabi::store<u32>(bss + 0xC, 0); gabi::store<u32>(bss + 0x18, 0); gabi::store<u32>(bss + 0x10, 0);
    gabi::call(0x028F026C, rec);
    f32 lo = gabi::load<f32>(ro), hi = gabi::load<f32>(ro + 4);
    gabi::store<f32>(bss, lo);
    gabi::store<f32>(bss + 4, hi);
    gabi::call(0x028ED6F8, bss + 8);
    gabi::call(0x028F026C, rec + 0xC);
    gabi::call(0x028EAB2C, bss + 9);
    gabi::call(0x028F026C, rec + 0x18);
}
VERIFY(0x024F1BB0, sinit_tu_024F1BB0);

/* 024F7C38 __sinit: first of three sinit-only TUs between d_cam_param (own __sinit 024F7BA4) and d_camera (probably the camera style/type data TUs). The header statics block at
 * 1046EE78 (zeroed 16-byte object at +0xC, {-pi, pi} from 10044870, two one-byte objects at +8/+9),
 * each registered for destruction (records 101D5430, +0xC, +0x18). */
static void sinit_tu_024F7C38() {
    WWHD_FUNC(0x024F7C38, void);
    const u32 bss = 0x1046EE78, rec = 0x101D5430, ro = 0x10044870;
    gabi::store<u32>(bss + 0x14, 0); gabi::store<u32>(bss + 0xC, 0); gabi::store<u32>(bss + 0x18, 0); gabi::store<u32>(bss + 0x10, 0);
    gabi::call(0x028F026C, rec);
    f32 lo = gabi::load<f32>(ro), hi = gabi::load<f32>(ro + 4);
    gabi::store<f32>(bss, lo);
    gabi::store<f32>(bss + 4, hi);
    gabi::call(0x028ED6F8, bss + 8);
    gabi::call(0x028F026C, rec + 0xC);
    gabi::call(0x028EAB2C, bss + 9);
    gabi::call(0x028F026C, rec + 0x18);
}
VERIFY(0x024F7C38, sinit_tu_024F7C38);

/* 024F7CCC __sinit: second sinit-only TU after d_cam_param. The header statics block at
 * 1046EE94 (zeroed 16-byte object at +0xC, {-pi, pi} from 10049340, two one-byte objects at +8/+9),
 * each registered for destruction (records 101D5454, +0xC, +0x18). */
static void sinit_tu_024F7CCC() {
    WWHD_FUNC(0x024F7CCC, void);
    const u32 bss = 0x1046EE94, rec = 0x101D5454, ro = 0x10049340;
    gabi::store<u32>(bss + 0x14, 0); gabi::store<u32>(bss + 0xC, 0); gabi::store<u32>(bss + 0x18, 0); gabi::store<u32>(bss + 0x10, 0);
    gabi::call(0x028F026C, rec);
    f32 lo = gabi::load<f32>(ro), hi = gabi::load<f32>(ro + 4);
    gabi::store<f32>(bss, lo);
    gabi::store<f32>(bss + 4, hi);
    gabi::call(0x028ED6F8, bss + 8);
    gabi::call(0x028F026C, rec + 0xC);
    gabi::call(0x028EAB2C, bss + 9);
    gabi::call(0x028F026C, rec + 0x18);
}
VERIFY(0x024F7CCC, sinit_tu_024F7CCC);

/* 024F7D60 __sinit: third sinit-only TU after d_cam_param. The header statics block at
 * 1046EEB0 (zeroed 16-byte object at +0xC, {-pi, pi} from 1004A480, two one-byte objects at +8/+9),
 * each registered for destruction (records 101D5478, +0xC, +0x18). */
static void sinit_tu_024F7D60() {
    WWHD_FUNC(0x024F7D60, void);
    const u32 bss = 0x1046EEB0, rec = 0x101D5478, ro = 0x1004A480;
    gabi::store<u32>(bss + 0x14, 0); gabi::store<u32>(bss + 0xC, 0); gabi::store<u32>(bss + 0x18, 0); gabi::store<u32>(bss + 0x10, 0);
    gabi::call(0x028F026C, rec);
    f32 lo = gabi::load<f32>(ro), hi = gabi::load<f32>(ro + 4);
    gabi::store<f32>(bss, lo);
    gabi::store<f32>(bss + 4, hi);
    gabi::call(0x028ED6F8, bss + 8);
    gabi::call(0x028F026C, rec + 0xC);
    gabi::call(0x028EAB2C, bss + 9);
    gabi::call(0x028F026C, rec + 0x18);
}
VERIFY(0x024F7D60, sinit_tu_024F7D60);

/* 02525F50 __sinit: sinit-only TU between d_com_inf_game (own __sinit 02525A48) and d_com_lib_game (own __sinit 02526044). The header statics block at
 * 104753EC (zeroed 16-byte object at +0xC, {-pi, pi} from 1004BCB0, two one-byte objects at +8/+9),
 * each registered for destruction (records 101D5EA0, +0xC, +0x18). */
static void sinit_tu_02525F50() {
    WWHD_FUNC(0x02525F50, void);
    const u32 bss = 0x104753EC, rec = 0x101D5EA0, ro = 0x1004BCB0;
    gabi::store<u32>(bss + 0x14, 0); gabi::store<u32>(bss + 0xC, 0); gabi::store<u32>(bss + 0x18, 0); gabi::store<u32>(bss + 0x10, 0);
    gabi::call(0x028F026C, rec);
    f32 lo = gabi::load<f32>(ro), hi = gabi::load<f32>(ro + 4);
    gabi::store<f32>(bss, lo);
    gabi::store<f32>(bss + 4, hi);
    gabi::call(0x028ED6F8, bss + 8);
    gabi::call(0x028F026C, rec + 0xC);
    gabi::call(0x028EAB2C, bss + 9);
    gabi::call(0x028F026C, rec + 0x18);
}
VERIFY(0x02525F50, sinit_tu_02525F50);

/* 025429A4 __sinit: sinit-only TU between d_event_data (own __sinit 025428F8) and d_event_manager (own __sinit 02544A10). The header statics block at
 * 104758FC (zeroed 16-byte object at +0xC, {-pi, pi} from 1004DCE8, two one-byte objects at +8/+9),
 * each registered for destruction (records 101D63A4, +0xC, +0x18). */
static void sinit_tu_025429A4() {
    WWHD_FUNC(0x025429A4, void);
    const u32 bss = 0x104758FC, rec = 0x101D63A4, ro = 0x1004DCE8;
    gabi::store<u32>(bss + 0x14, 0); gabi::store<u32>(bss + 0xC, 0); gabi::store<u32>(bss + 0x18, 0); gabi::store<u32>(bss + 0x10, 0);
    gabi::call(0x028F026C, rec);
    f32 lo = gabi::load<f32>(ro), hi = gabi::load<f32>(ro + 4);
    gabi::store<f32>(bss, lo);
    gabi::store<f32>(bss + 4, hi);
    gabi::call(0x028ED6F8, bss + 8);
    gabi::call(0x028F026C, rec + 0xC);
    gabi::call(0x028EAB2C, bss + 9);
    gabi::call(0x028F026C, rec + 0x18);
}
VERIFY(0x025429A4, sinit_tu_025429A4);

/* 0254D9A4 __sinit: sinit-only TU between d_grass (own __sinit 0254D3F4) and d_item. The header statics block at
 * 104759B0 (zeroed 16-byte object at +0xC, {-pi, pi} from 1004E3C8, two one-byte objects at +8/+9),
 * each registered for destruction (records 101E3A74, +0xC, +0x18). */
static void sinit_tu_0254D9A4() {
    WWHD_FUNC(0x0254D9A4, void);
    const u32 bss = 0x104759B0, rec = 0x101E3A74, ro = 0x1004E3C8;
    gabi::store<u32>(bss + 0x14, 0); gabi::store<u32>(bss + 0xC, 0); gabi::store<u32>(bss + 0x18, 0); gabi::store<u32>(bss + 0x10, 0);
    gabi::call(0x028F026C, rec);
    f32 lo = gabi::load<f32>(ro), hi = gabi::load<f32>(ro + 4);
    gabi::store<f32>(bss, lo);
    gabi::store<f32>(bss + 4, hi);
    gabi::call(0x028ED6F8, bss + 8);
    gabi::call(0x028F026C, rec + 0xC);
    gabi::call(0x028EAB2C, bss + 9);
    gabi::call(0x028F026C, rec + 0x18);
}
VERIFY(0x0254D9A4, sinit_tu_0254D9A4);

/* 0259D1B8 __sinit: sinit-only TU after d_meter's deferred tail (d_meter has __sinit_d_meter_cpp 02598444; probably the HD d_file_error TU) and before d_npc (own __sinit 025A1648). The header statics block at
 * 1047B0AC (zeroed 16-byte object at +0xC, {-pi, pi} from 10051548, two one-byte objects at +8/+9),
 * each registered for destruction (records 101EA12C, +0xC, +0x18). */
static void sinit_tu_0259D1B8() {
    WWHD_FUNC(0x0259D1B8, void);
    const u32 bss = 0x1047B0AC, rec = 0x101EA12C, ro = 0x10051548;
    gabi::store<u32>(bss + 0x14, 0); gabi::store<u32>(bss + 0xC, 0); gabi::store<u32>(bss + 0x18, 0); gabi::store<u32>(bss + 0x10, 0);
    gabi::call(0x028F026C, rec);
    f32 lo = gabi::load<f32>(ro), hi = gabi::load<f32>(ro + 4);
    gabi::store<f32>(bss, lo);
    gabi::store<f32>(bss + 4, hi);
    gabi::call(0x028ED6F8, bss + 8);
    gabi::call(0x028F026C, rec + 0xC);
    gabi::call(0x028EAB2C, bss + 9);
    gabi::call(0x028F026C, rec + 0x18);
}
VERIFY(0x0259D1B8, sinit_tu_0259D1B8);

/* 025AB364 __sinit: sinit-only TU between d_path (own __sinit 025AB2D0) and d_point_wind (own __sinit 025AB718). The header statics block at
 * 1047B328 (zeroed 16-byte object at +0xC, {-pi, pi} from 10052518, two one-byte objects at +8/+9),
 * each registered for destruction (records 101EA584, +0xC, +0x18). */
static void sinit_tu_025AB364() {
    WWHD_FUNC(0x025AB364, void);
    const u32 bss = 0x1047B328, rec = 0x101EA584, ro = 0x10052518;
    gabi::store<u32>(bss + 0x14, 0); gabi::store<u32>(bss + 0xC, 0); gabi::store<u32>(bss + 0x18, 0); gabi::store<u32>(bss + 0x10, 0);
    gabi::call(0x028F026C, rec);
    f32 lo = gabi::load<f32>(ro), hi = gabi::load<f32>(ro + 4);
    gabi::store<f32>(bss, lo);
    gabi::store<f32>(bss + 4, hi);
    gabi::call(0x028ED6F8, bss + 8);
    gabi::call(0x028F026C, rec + 0xC);
    gabi::call(0x028EAB2C, bss + 9);
    gabi::call(0x028F026C, rec + 0x18);
}
VERIFY(0x025AB364, sinit_tu_025AB364);

/* 025AF210 __sinit: sinit-only TU between d_s_open (own __sinit 025AF174) and d_s_play. The header statics block at
 * 1047B3FC (zeroed 16-byte object at +0xC, {-pi, pi} from 10052B40, two one-byte objects at +8/+9),
 * each registered for destruction (records 101EA888, +0xC, +0x18). */
static void sinit_tu_025AF210() {
    WWHD_FUNC(0x025AF210, void);
    const u32 bss = 0x1047B3FC, rec = 0x101EA888, ro = 0x10052B40;
    gabi::store<u32>(bss + 0x14, 0); gabi::store<u32>(bss + 0xC, 0); gabi::store<u32>(bss + 0x18, 0); gabi::store<u32>(bss + 0x10, 0);
    gabi::call(0x028F026C, rec);
    f32 lo = gabi::load<f32>(ro), hi = gabi::load<f32>(ro + 4);
    gabi::store<f32>(bss, lo);
    gabi::store<f32>(bss + 4, hi);
    gabi::call(0x028ED6F8, bss + 8);
    gabi::call(0x028F026C, rec + 0xC);
    gabi::call(0x028EAB2C, bss + 9);
    gabi::call(0x028F026C, rec + 0x18);
}
VERIFY(0x025AF210, sinit_tu_025AF210);

/* 025DD130 __sinit: first of two sinit-only TUs between f_op_view (own __sinit 025DD09C) and f_pc_base (own __sinit 025DD644). The header statics block at
 * 1048A668 (zeroed 16-byte object at +0xC, {-pi, pi} from 10057DDC, two one-byte objects at +8/+9),
 * each registered for destruction (records 101F38C0, +0xC, +0x18). */
static void sinit_tu_025DD130() {
    WWHD_FUNC(0x025DD130, void);
    const u32 bss = 0x1048A668, rec = 0x101F38C0, ro = 0x10057DDC;
    gabi::store<u32>(bss + 0x14, 0); gabi::store<u32>(bss + 0xC, 0); gabi::store<u32>(bss + 0x18, 0); gabi::store<u32>(bss + 0x10, 0);
    gabi::call(0x028F026C, rec);
    f32 lo = gabi::load<f32>(ro), hi = gabi::load<f32>(ro + 4);
    gabi::store<f32>(bss, lo);
    gabi::store<f32>(bss + 4, hi);
    gabi::call(0x028ED6F8, bss + 8);
    gabi::call(0x028F026C, rec + 0xC);
    gabi::call(0x028EAB2C, bss + 9);
    gabi::call(0x028F026C, rec + 0x18);
}
VERIFY(0x025DD130, sinit_tu_025DD130);

/* 025DD1C4 __sinit: second sinit-only TU after f_op_view. The header statics block at
 * 1048A684 (zeroed 16-byte object at +0xC, {-pi, pi} from 10057DE8, two one-byte objects at +8/+9),
 * each registered for destruction (records 101F38E4, +0xC, +0x18). */
static void sinit_tu_025DD1C4() {
    WWHD_FUNC(0x025DD1C4, void);
    const u32 bss = 0x1048A684, rec = 0x101F38E4, ro = 0x10057DE8;
    gabi::store<u32>(bss + 0x14, 0); gabi::store<u32>(bss + 0xC, 0); gabi::store<u32>(bss + 0x18, 0); gabi::store<u32>(bss + 0x10, 0);
    gabi::call(0x028F026C, rec);
    f32 lo = gabi::load<f32>(ro), hi = gabi::load<f32>(ro + 4);
    gabi::store<f32>(bss, lo);
    gabi::store<f32>(bss + 4, hi);
    gabi::call(0x028ED6F8, bss + 8);
    gabi::call(0x028F026C, rec + 0xC);
    gabi::call(0x028EAB2C, bss + 9);
    gabi::call(0x028F026C, rec + 0x18);
}
VERIFY(0x025DD1C4, sinit_tu_025DD1C4);

/* 025DDD84 __sinit: sinit-only TU between f_pc_creator (own __sinit 025DDCF0) and f_pc_delete_tag (own __sinit 025DDF10). The header statics block at
 * 1048A72C (zeroed 16-byte object at +0xC, {-pi, pi} from 10058260, two one-byte objects at +8/+9),
 * each registered for destruction (records 101F39D4, +0xC, +0x18). */
static void sinit_tu_025DDD84() {
    WWHD_FUNC(0x025DDD84, void);
    const u32 bss = 0x1048A72C, rec = 0x101F39D4, ro = 0x10058260;
    gabi::store<u32>(bss + 0x14, 0); gabi::store<u32>(bss + 0xC, 0); gabi::store<u32>(bss + 0x18, 0); gabi::store<u32>(bss + 0x10, 0);
    gabi::call(0x028F026C, rec);
    f32 lo = gabi::load<f32>(ro), hi = gabi::load<f32>(ro + 4);
    gabi::store<f32>(bss, lo);
    gabi::store<f32>(bss + 4, hi);
    gabi::call(0x028ED6F8, bss + 8);
    gabi::call(0x028F026C, rec + 0xC);
    gabi::call(0x028EAB2C, bss + 9);
    gabi::call(0x028F026C, rec + 0x18);
}
VERIFY(0x025DDD84, sinit_tu_025DDD84);

/* 025DFF14 __sinit: sinit-only TU between f_pc_method_tag (own __sinit 025DFE80) and f_pc_node (own __sinit 025E029C). The header statics block at
 * 1048AAB4 (zeroed 16-byte object at +0xC, {-pi, pi} from 100583E0, two one-byte objects at +8/+9),
 * each registered for destruction (records 101F3D18, +0xC, +0x18). */
static void sinit_tu_025DFF14() {
    WWHD_FUNC(0x025DFF14, void);
    const u32 bss = 0x1048AAB4, rec = 0x101F3D18, ro = 0x100583E0;
    gabi::store<u32>(bss + 0x14, 0); gabi::store<u32>(bss + 0xC, 0); gabi::store<u32>(bss + 0x18, 0); gabi::store<u32>(bss + 0x10, 0);
    gabi::call(0x028F026C, rec);
    f32 lo = gabi::load<f32>(ro), hi = gabi::load<f32>(ro + 4);
    gabi::store<f32>(bss, lo);
    gabi::store<f32>(bss + 4, hi);
    gabi::call(0x028ED6F8, bss + 8);
    gabi::call(0x028F026C, rec + 0xC);
    gabi::call(0x028EAB2C, bss + 9);
    gabi::call(0x028F026C, rec + 0x18);
}
VERIFY(0x025DFF14, sinit_tu_025DFF14);

/* 025F0974 __sinit: sinit-only TU between m_Do_graphic (own __sinit 025F08B0 + deferred tail) and m_Do_hostIO. The header statics block at
 * 1048CFB8 (zeroed 16-byte object at +0xC, {-pi, pi} from 10058F60, two one-byte objects at +8/+9),
 * each registered for destruction (records 101F482C, +0xC, +0x18). */
static void sinit_tu_025F0974() {
    WWHD_FUNC(0x025F0974, void);
    const u32 bss = 0x1048CFB8, rec = 0x101F482C, ro = 0x10058F60;
    gabi::store<u32>(bss + 0x14, 0); gabi::store<u32>(bss + 0xC, 0); gabi::store<u32>(bss + 0x18, 0); gabi::store<u32>(bss + 0x10, 0);
    gabi::call(0x028F026C, rec);
    f32 lo = gabi::load<f32>(ro), hi = gabi::load<f32>(ro + 4);
    gabi::store<f32>(bss, lo);
    gabi::store<f32>(bss + 4, hi);
    gabi::call(0x028ED6F8, bss + 8);
    gabi::call(0x028F026C, rec + 0xC);
    gabi::call(0x028EAB2C, bss + 9);
    gabi::call(0x028F026C, rec + 0x18);
}
VERIFY(0x025F0974, sinit_tu_025F0974);

} // namespace sinit_only_tus_cpp
