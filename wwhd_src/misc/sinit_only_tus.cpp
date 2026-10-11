/** Initializer-only WWHD translation units.
 * Each initializes the common header statics: one zeroed object, a rodata pi pair, and two
 * constructed byte objects, with destruction registrations. Each lies between translation
 * units with their own initializers.
 * The original filenames are unknown. Function names below describe linked positions. */
#include "bindings.h"

/* 024F1BB0 */
// HD: sinit-only TU between d_bg_s_lin_chk (own __sinit 024F1B1C) and d_bg_s_movebg_actor. Original filename unconfirmed.
static void __sinit_after_d_bg_s_lin_chk_cpp() {
    sinit_header_statics_rodata(0x1046ED60, 0x101D52B8, 0x1046ED6C, 0x10043BA0);
}

/* 024F7C38 */
// HD: first of three sinit-only TUs between d_cam_param (own __sinit 024F7BA4) and d_camera (probably the camera style/type data TUs). Original filename unconfirmed.
static void __sinit_after_d_cam_param_first_cpp() {
    sinit_header_statics_rodata(0x1046EE78, 0x101D5430, 0x1046EE84, 0x10044870);
}

/* 024F7CCC */
// HD: second sinit-only TU after d_cam_param. Original filename unconfirmed.
static void __sinit_after_d_cam_param_second_cpp() {
    sinit_header_statics_rodata(0x1046EE94, 0x101D5454, 0x1046EEA0, 0x10049340);
}

/* 024F7D60 */
// HD: third sinit-only TU after d_cam_param. Original filename unconfirmed.
static void __sinit_after_d_cam_param_third_cpp() {
    sinit_header_statics_rodata(0x1046EEB0, 0x101D5478, 0x1046EEBC, 0x1004A480);
}

/* 02525F50 */
// HD: sinit-only TU between d_com_inf_game (own __sinit 02525A48) and d_com_lib_game (own __sinit 02526044). Original filename unconfirmed.
static void __sinit_after_d_com_inf_game_cpp() {
    sinit_header_statics_rodata(0x104753EC, 0x101D5EA0, 0x104753F8, 0x1004BCB0);
}

/* 025429A4 */
// HD: sinit-only TU between d_event_data (own __sinit 025428F8) and d_event_manager (own __sinit 02544A10). Original filename unconfirmed.
static void __sinit_after_d_event_data_cpp() {
    sinit_header_statics_rodata(0x104758FC, 0x101D63A4, 0x10475908, 0x1004DCE8);
}

/* 0254D9A4 */
// HD: sinit-only TU between d_grass (own __sinit 0254D3F4) and d_item. Original filename unconfirmed.
static void __sinit_after_d_grass_cpp() {
    sinit_header_statics_rodata(0x104759B0, 0x101E3A74, 0x104759BC, 0x1004E3C8);
}

/* 0259D1B8 */
// HD: sinit-only TU after d_meter's deferred tail (d_meter has __sinit_d_meter_cpp 02598444; probably the HD d_file_error TU) and before d_npc (own __sinit 025A1648). Original filename unconfirmed.
static void __sinit_after_d_meter_tail_cpp() {
    sinit_header_statics_rodata(0x1047B0AC, 0x101EA12C, 0x1047B0B8, 0x10051548);
}

/* 025AB364 */
// HD: sinit-only TU between d_path (own __sinit 025AB2D0) and d_point_wind (own __sinit 025AB718). Original filename unconfirmed.
static void __sinit_after_d_path_cpp() {
    sinit_header_statics_rodata(0x1047B328, 0x101EA584, 0x1047B334, 0x10052518);
}

/* 025AF210 */
// HD: sinit-only TU between d_s_open (own __sinit 025AF174) and d_s_play. Original filename unconfirmed.
static void __sinit_after_d_s_open_cpp() {
    sinit_header_statics_rodata(0x1047B3FC, 0x101EA888, 0x1047B408, 0x10052B40);
}

/* 025DD130 */
// HD: first of two sinit-only TUs between f_op_view (own __sinit 025DD09C) and f_pc_base (own __sinit 025DD644). Original filename unconfirmed.
static void __sinit_after_f_op_view_first_cpp() {
    sinit_header_statics_rodata(0x1048A668, 0x101F38C0, 0x1048A674, 0x10057DDC);
}

/* 025DD1C4 */
// HD: second sinit-only TU after f_op_view. Original filename unconfirmed.
static void __sinit_after_f_op_view_second_cpp() {
    sinit_header_statics_rodata(0x1048A684, 0x101F38E4, 0x1048A690, 0x10057DE8);
}

/* 025DDD84 */
// HD: sinit-only TU between f_pc_creator (own __sinit 025DDCF0) and f_pc_delete_tag (own __sinit 025DDF10). Original filename unconfirmed.
static void __sinit_after_f_pc_creator_cpp() {
    sinit_header_statics_rodata(0x1048A72C, 0x101F39D4, 0x1048A738, 0x10058260);
}

/* 025DFF14 */
// HD: sinit-only TU between f_pc_method_tag (own __sinit 025DFE80) and f_pc_node (own __sinit 025E029C). Original filename unconfirmed.
static void __sinit_after_f_pc_method_tag_cpp() {
    sinit_header_statics_rodata(0x1048AAB4, 0x101F3D18, 0x1048AAC0, 0x100583E0);
}

/* 025F0974 */
// HD: sinit-only TU between m_Do_graphic (own __sinit 025F08B0 + deferred tail) and m_Do_hostIO. Original filename unconfirmed.
static void __sinit_after_m_Do_graphic_cpp() {
    sinit_header_statics_rodata(0x1048CFB8, 0x101F482C, 0x1048CFC4, 0x10058F60);
}

/* WWHD glue (tools/verify/autoglue.py): no data bindings. */
