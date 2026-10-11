/* f_op_actor (GameCube enums of tww f_op_actor.h / d_event.h). Layouts: f_op/f_op_actor.h (with
 * fopAcCnd_*, dEvtCmd_*).
 *
 * Values are unchanged in HD (status bits SHOWMAP, FREEZE, CARRY, UNK4000, HOOK_CARRY, BOSS, the
 * attention flags and event conditions are used with these values by verified units). Some raw
 * units define a few of these names locally: they do not include gc_api; the local copies go
 * when those units are rewritten (a rewritten unit uses these). */
#pragma once
#include "bindings.h"

/* fopAc_ac_c::actor_status; the low five bits (& 0x1F) select the map icon */
enum fopAc_Status_e {
    fopAcStts_SHOWMAP_e = 0x00000020,
    fopAcStts_UNK40_e = 0x00000040,
    fopAcStts_NOCULLEXEC_e = 0x00000080,
    fopAcStts_CULL_e = 0x00000100,
    fopAcStts_FREEZE_e = 0x00000400,
    fopAcStts_UNK800_e = 0x00000800,
    fopAcStts_UNK1000_e = 0x00001000, /* door related */
    fopAcStts_CARRY_e = 0x00002000,
    fopAcStts_UNK4000_e = 0x00004000,
    fopAcStts_FORCEMOVE_e = 0x00008000,
    fopAcStts_UNK10000_e = 0x00010000,
    fopAcStts_NOPAUSE_e = 0x00020000,
    fopAcStts_UNK40000_e = 0x00040000,
    fopAcStts_UNK80000_e = 0x00080000, /* hookshot related */
    fopAcStts_HOOK_CARRY_e = 0x00100000,
    fopAcStts_UNK200000_e = 0x00200000, /* hookshot related */
    fopAcStts_NODRAW_e = 0x01000000,
    fopAcStts_UNK2000000_e = 0x02000000,
    fopAcStts_BOSS_e = 0x04000000,
    fopAcStts_UNK8000000_e = 0x08000000,
    fopAcStts_UNK10000000_e = 0x10000000, /* hookshot related */
};

/* fopAc_ac_c::group */
enum fopAc_Group_e {
    fopAc_ACTOR_e = 0x0,
    fopAc_PLAYER_e = 0x1,
    fopAc_ENEMY_e = 0x2,
    fopAc_ENV_e = 0x3,
    fopAc_NPC_e = 0x4,
};

/* attention types (index into attention_info.distances) */
enum fopAc_AttentionType_e {
    fopAc_Attn_TYPE_MISC_e = 0x0,
    fopAc_Attn_TYPE_LOCK_e = fopAc_Attn_TYPE_MISC_e, /* older gc_api name ("LOCK") */
    fopAc_Attn_TYPE_TALK_e = 0x1,
    fopAc_Attn_TYPE_BATTLE_e = 0x2,
    fopAc_Attn_TYPE_SPEAK_e = 0x3,
    fopAc_Attn_TYPE_CARRY_e = 0x4,
    fopAc_Attn_TYPE_DOOR_e = 0x5,
    fopAc_Attn_TYPE_TREASURE_e = 0x6,
    fopAc_Attn_TYPE_SHIP_e = 0x7,
};
/* attention_info.flags */
enum fopAc_AttentionFlag_e {
    fopAc_Attn_LOCKON_MISC_e = 0x00000001,
    fopAc_Attn_LOCKON_TALK_e = 0x00000002,
    fopAc_Attn_LOCKON_BATTLE_e = 0x00000004,
    fopAc_Attn_ACTION_SPEAK_e = 0x00000008,
    fopAc_Attn_ACTION_CARRY_e = 0x00000010,
    fopAc_Attn_ACTION_DOOR_e = 0x00000020,
    fopAc_Attn_ACTION_TREASURE_e = 0x00000040,
    fopAc_Attn_ACTION_SHIP_e = 0x00000080,
    fopAc_Attn_UNK1000000_e = 0x01000000,
    fopAc_Attn_TALKFLAG_NOTALK_e = 0x02000000,
    fopAc_Attn_ENEMYFLAG_NOLOCKON_e = 0x04000000,
    fopAc_Attn_TALKFLAG_LOOK_e = 0x08000000,
    fopAc_Attn_UNK10000000_e = 0x10000000,
    fopAc_Attn_TALKFLAG_CHECK_e = 0x20000000,
    fopAc_Attn_TALKFLAG_READ_e = 0x40000000,
};

/* fopAc_ac_c::cullType */
enum fopAc_Cull_e {
    fopAc_CULLBOX_0_e,
    fopAc_CULLBOX_1_e,
    fopAc_CULLBOX_2_e,
    fopAc_CULLBOX_3_e,
    fopAc_CULLBOX_4_e,
    fopAc_CULLBOX_5_e,
    fopAc_CULLBOX_6_e,
    fopAc_CULLBOX_7_e,
    fopAc_CULLBOX_8_e,
    fopAc_CULLBOX_9_e,
    fopAc_CULLBOX_10_e,
    fopAc_CULLBOX_11_e,
    fopAc_CULLBOX_12_e,
    fopAc_CULLBOX_13_e,
    fopAc_CULLBOX_CUSTOM_e,
    fopAc_CULLSPHERE_0_e,
    fopAc_CULLSPHERE_1_e,
    fopAc_CULLSPHERE_2_e,
    fopAc_CULLSPHERE_3_e,
    fopAc_CULLSPHERE_4_e,
    fopAc_CULLSPHERE_5_e,
    fopAc_CULLSPHERE_6_e,
    fopAc_CULLSPHERE_7_e,
    fopAc_CULLSPHERE_CUSTOM_e,
};

/* dEvt_info_c::mCondition */
enum dEvt_Condition_e {
    dEvtCnd_NONE_e = 0x0000,
    dEvtCnd_CANTALK_e = 0x0001,
    dEvtCnd_UNK2_e = 0x0002,
    dEvtCnd_CANDOOR_e = 0x0004,
    dEvtCnd_CANGETITEM_e = 0x0008,
    dEvtCnd_UNK10_e = 0x0010,
    dEvtCnd_CANTALKITEM_e = 0x0020,
    dEvtCnd_CANCATCH_e = 0x0040,
};
enum dEvt_Flag_e { dEvtFlag_NOPARTNER_e = 0x01 };
