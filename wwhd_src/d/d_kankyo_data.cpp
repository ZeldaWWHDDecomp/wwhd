/** Environment data accessors. */
#include "d/d_kankyo_data.h"

static stage_palet_info_class l_field_data;
static stage_pselect_info_class l_pselect_default[] = {
    {{0, 0, 1, 2, 2, 3, 0, 0}, 0.0f},
    {{4, 4, 5, 6, 6, 7, 0, 0}, 0.0f},
};
static stage_envr_info_class l_envr_default[] = {
    {{0, 1, 2, 3, 0, 0, 0, 0}},
    {{4, 5, 6, 7, 0, 0, 0, 0}},
};
static stage_vrbox_info_class l_vr_box_data;
// HD: Field/menu timing differs from the GameCube schedules.
static dKyd_Schedule l_time_attribute[] = {
    {0.0f, 90.0f, 5, 5, {0, 0}},
    {90.0f, 105.0f, 5, 0, {0, 0}},
    {105.0f, 120.0f, 0, 1, {0, 0}},
    {120.0f, 150.0f, 1, 2, {0, 0}},
    {150.0f, 255.0f, 2, 2, {0, 0}},
    {255.0f, 270.0f, 2, 3, {0, 0}},
    {270.0f, 300.0f, 3, 4, {0, 0}},
    {300.0f, 315.0f, 4, 5, {0, 0}},
    {315.0f, 360.0f, 5, 5, {0, 0}},
    {360.0f, 360.0f, 5, 5, {0, 0}},
    {360.0f, 360.0f, 5, 5, {0, 0}},
};
static dKyd_Schedule l_time_attribute_boss[] = {
    {0.0f, 45.0f, 0, 1, {0, 0}},
    {45.0f, 90.0f, 1, 2, {0, 0}},
    {90.0f, 180.0f, 2, 3, {0, 0}},
    {180.0f, 225.0f, 3, 4, {0, 0}},
    {225.0f, 270.0f, 4, 5, {0, 0}},
    {270.0f, 360.0f, 5, 0, {0, 0}},
    {360.0f, 360.0f, 5, 5, {0, 0}},
    {360.0f, 360.0f, 5, 5, {0, 0}},
    {360.0f, 360.0f, 5, 5, {0, 0}},
    {360.0f, 360.0f, 5, 5, {0, 0}},
    {360.0f, 360.0f, 5, 5, {0, 0}},
};
static dKyd_Schedule l_time_attribute_menu[] = {
    {0.0f, 90.0f, 5, 5, {0, 0}},
    {90.0f, 105.0f, 5, 0, {0, 0}},
    {105.0f, 120.0f, 0, 1, {0, 0}},
    {120.0f, 150.0f, 1, 2, {0, 0}},
    {150.0f, 255.0f, 2, 2, {0, 0}},
    {255.0f, 270.0f, 2, 3, {0, 0}},
    {270.0f, 300.0f, 3, 4, {0, 0}},
    {300.0f, 315.0f, 4, 5, {0, 0}},
    {315.0f, 360.0f, 5, 5, {0, 0}},
    {360.0f, 360.0f, 5, 5, {0, 0}},
    {360.0f, 360.0f, 5, 5, {0, 0}},
};
static GXFogAdjTable S_xfog_table_data[] = {
    {{260, 260, 270, 280, 290, 300, 310, 320, 330, 340}},
    {{250, 260, 280, 320, 350, 400, 480, 550, 600, 800}},
};

/* 025638D8 */
void dKyd_xfog_table_set(u32 index) {
    // HD: The index remains 32-bit, and the lazy accessor is called for every entry.
    GXFogAdjTable* table = &S_xfog_table_data[index];
    for (u32 i = 0; i < 10; i++) {
        dKyd_environment_c* environment = (dKyd_environment_c*)dKy_getEnvlight();
        u16 value = table->fogVals[i];
        environment->mFogAdjTable.fogVals[i] = value;
    }
}

/* 025639D4 */
void* dKyd_dmpalet_getp() {
    return &l_field_data;
}

/* 025639E0 */
void* dKyd_dmpselect_getp() {
    return l_pselect_default;
}

/* 025639EC */
void* dKyd_dmenvr_getp() {
    return l_envr_default;
}

/* 025639F8 */
void* dKyd_dmvrbox_getp() {
    return &l_vr_box_data;
}

/* 02563A04 */
void* dKyd_schejule_getp() {
    return l_time_attribute;
}

/* 02563A10 */
void* dKyd_schejule_boss_getp() {
    return l_time_attribute_boss;
}

/* 02563A1C */
void* dKyd_schejule_menu_getp() {
    return l_time_attribute_menu;
}

/* 02563A28 */
void __sinit_d_kankyo_data_cpp() {
    sinit_header_statics_rodata(0x104773D8, 0x101E99A8, 0x104773E4, 0x1004F368);
}

/* 02563940 */
static void __sinit_d_kankyo_data_xfog_tu() {
    // HD: The fog setter belongs to a separate TU with its own header statics.
    sinit_header_statics_rodata(0x104773BC, 0x101E9984, 0x104773C8, 0x1004F35C);
}

/* WWHD glue
data S_xfog_table_data 101E8F50
data l_field_data 101E9104
data l_pselect_default 101E8F28
data l_envr_default 101E8F40
data l_vr_box_data 101E97C4
data l_time_attribute 101E8F78
data l_time_attribute_boss 101E8FFC
data l_time_attribute_menu 101E9080
*/
