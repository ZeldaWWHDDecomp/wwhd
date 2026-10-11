// Link range 02 private animation helpers.
#pragma once
#include "d/actor/d_a_player_main.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/tu.h"
#include "SSystem/SComponent/c_m3d_g_pla.h"

// HD: preserves the complete caller registers before the callee narrows its frame parameters.
void player02SetFrameCtrl(daPy_lk_c* player, J3DFrameCtrl* ctrl, u32 attr,
                          int start, int end, f32 rate, f32 frame);

void player02SetLine(u32 check, u32 start, u32 end, fopAc_ac_c* actor);
void player02MakeEmitter(u32 callback, u32 particleID, u32 matrix, u32 position, u32 color);
void player02MakeEmitterColor(u32 callback, u32 particleID, u32 matrix, u32 position,
                              u32 primaryColor, u32 environmentColor);

namespace player02 {
struct mtxPosFollowCallback_c {
    u8 mFollowBase[0xC];
    be<u32> mpAngle;
};
WWHD_OFFSET(mtxPosFollowCallback_c, mpAngle, 0xC);
WWHD_SIZE(mtxPosFollowCallback_c, 0x10);
enum : u32 {
    daPyStts0_TELESCOPE_LOOK_e = 0x200000,
    CUT_TYPE_CUT_TURN = 8,
    daPyStts1_PICTO_BOX_AIM_e = 8,
    dRes_INDEX_LKANM_BTP_TMABAA_e = 0x22D,
    dRes_INDEX_LKANM_BTK_TMABA_e = 0x1A7,
    dRes_INDEX_LKANM_BTK_TEUP_e = 0x187,
    dRes_INDEX_LKANM_BTP_TMABAB_e = 0x22E,
    dRes_INDEX_LKANM_BTP_TMABAC_e = 0x22F,
    dRes_INDEX_LKANM_BTP_TMABACB_e = 0x230,
    dRes_INDEX_LKANM_BTK_TEDW_e = 0x183,
    dRes_INDEX_LKANM_BTP_TMABACC_e = 0x231,
    dRes_INDEX_LKANM_BTP_TMABAD_e = 0x232,
    dRes_INDEX_LKANM_BTP_TMABAE_e = 0x233,
    dRes_INDEX_LKANM_BTK_TEL_e = 0x184,
    dRes_INDEX_LKANM_BTK_TER_e = 0x185,
    dRes_INDEX_LKANM_BTP_TMABAF_e = 0x234,
    dRes_INDEX_LKANM_BTP_TMABAG_e = 0x235,
    dRes_INDEX_LKANM_BTP_TMABAH_e = 0x236,
    dRes_INDEX_LKANM_BTP_TMABAI_e = 0x237,
    dRes_INDEX_LKANM_BTP_TMABAJ_e = 0x238,
    dRes_INDEX_LKANM_BTK_TEYORIME_e = 0x18B,
    dRes_INDEX_LKANM_BTK_TWAITPICKUP_e = 0x1CB,
    dRes_INDEX_LKANM_BTK_THOOKSHOTJMP_e = 0x19B,
    dRes_INDEX_LKANM_BTP_TGRABP_e = 0x215,
    dRes_INDEX_LKANM_BTK_TGRABP_e = 0x191,
    dRes_INDEX_LKANM_BTP_TGRABUP_e = 0x218,
    dRes_INDEX_LKANM_BTK_TGRABUP_e = 0x193,
    dRes_INDEX_LKANM_BTP_TGRABNG_e = 0x214,
    dRes_INDEX_LKANM_BTK_TGRABNG_e = 0x190,
    dRes_INDEX_LKANM_BTP_TGRABTHROW_e = 0x217,
    dRes_INDEX_LKANM_BTK_TGRABTHROW_e = 0x192,
    dRes_INDEX_LKANM_BTP_TGRABRE_e = 0x216,
    dRes_INDEX_LKANM_BTK_TECN_e = 0x180,
    dRes_INDEX_LKANM_BTP_TWALLWL_e = 0x271,
    dRes_INDEX_LKANM_BTP_TWALLWR_e = 0x272,
    dRes_INDEX_LKANM_BTP_TWALLPL_e = 0x26F,
    dRes_INDEX_LKANM_BTK_TWALLPL_e = 0x1CF,
    dRes_INDEX_LKANM_BTP_TWALLPR_e = 0x270,
    dRes_INDEX_LKANM_BTK_TWALLPR_e = 0x1D0,
    dRes_INDEX_LKANM_BTP_TCUT_e = 0x1F9,
    dRes_INDEX_LKANM_BTP_TCUTEA_e = 0x1FB,
    dRes_INDEX_LKANM_BTK_TCUTEA_e = 0x171,
    dRes_INDEX_LKANM_BTP_TEXCA1_e = 0x20C,
    dRes_INDEX_LKANM_BTK_TEXCA1_e = 0x189,
    dRes_INDEX_LKANM_BTP_TEXCB1_e = 0x20D,
    dRes_INDEX_LKANM_BTK_TEXCB1_e = 0x18A,
    dRes_INDEX_LKANM_BTP_TJATTACK_e = 0x223,
    dRes_INDEX_LKANM_BTK_TJATTACK_e = 0x19D,
    dRes_INDEX_LKANM_BTP_TCUTTURNP_e = 0x200,
    dRes_INDEX_LKANM_BTP_TCUTTURN_e = 0x1FE,
    dRes_INDEX_LKANM_BTK_TCUTTURN_e = 0x174,
    dRes_INDEX_LKANM_BTP_TMJMP_e = 0x239,
    dRes_INDEX_LKANM_BTK_TMJMP_e = 0x1A8,
    dRes_INDEX_LKANM_BTP_TMJMPC_e = 0x23A,
    dRes_INDEX_LKANM_BTK_TMJMPC_e = 0x1A9,
    dRes_INDEX_LKANM_BTP_TMROLLL_e = 0x23B,
    dRes_INDEX_LKANM_BTK_TMROLLL_e = 0x1AA,
    dRes_INDEX_LKANM_BTP_TMROLLR_e = 0x23D,
    dRes_INDEX_LKANM_BTK_TMROLLR_e = 0x1AC,
    dRes_INDEX_LKANM_BTP_TMROLLLRC_e = 0x23C,
    dRes_INDEX_LKANM_BTK_TMROLLLC_e = 0x1AB,
    dRes_INDEX_LKANM_BTK_TMROLLRC_e = 0x1AD,
    dRes_INDEX_LKANM_BTP_TMSTEPOVER_e = 0x23E,
    dRes_INDEX_LKANM_BTK_TMSTEPOVER_e = 0x1AE,
    dRes_INDEX_LKANM_BTP_TMSTEPOVERA_e = 0x23F,
    dRes_INDEX_LKANM_BTK_TMSTEPOVERA_e = 0x1AF,
    dRes_INDEX_LKANM_BTP_TMSTEPOVERLAND_e = 0x240,
    dRes_INDEX_LKANM_BTK_TMSTEPOVERLAND_e = 0x1B0,
    dRes_INDEX_LKANM_BTP_TATNJLR_e = 0x1E7,
    dRes_INDEX_LKANM_BTK_TATNJL_e = 0x15F,
    dRes_INDEX_LKANM_BTK_TATNJR_e = 0x160,
    dRes_INDEX_LKANM_BTP_TJMP_e = 0x224,
    dRes_INDEX_LKANM_BTK_TJMP_e = 0x19E,
    dRes_INDEX_LKANM_BTP_TROLLF_e = 0x249,
    dRes_INDEX_LKANM_BTK_TROLLF_e = 0x1B8,
    dRes_INDEX_LKANM_BTP_TROLLB_e = 0x247,
    dRes_INDEX_LKANM_BTK_TROLLB_e = 0x1B7,
    dRes_INDEX_LKANM_BTP_TROLLBLAND_e = 0x248,
    dRes_INDEX_LKANM_BTP_TROLLFMIS_e = 0x24A,
    dRes_INDEX_LKANM_BTK_TROLLFMIS_e = 0x1B9,
    dRes_INDEX_LKANM_BTP_TFALL_e = 0x20E,
    dRes_INDEX_LKANM_BTP_TLANDDAMA_e = 0x229,
    dRes_INDEX_LKANM_BTK_TLANDDAMA_e = 0x1A3,
    dRes_INDEX_LKANM_BTP_TLANDDAMAST_e = 0x22A,
    dRes_INDEX_LKANM_BTK_TLANDDAMAST_e = 0x1A4,
    dRes_INDEX_LKANM_BTP_TDAM_e = 0x201,
    dRes_INDEX_LKANM_BTK_TDAM_e = 0x176,
    dRes_INDEX_LKANM_BTP_TDAMF_e = 0x204,
    dRes_INDEX_LKANM_BTP_TDAMFFUP_e = 0x206,
    dRes_INDEX_LKANM_BTK_TDAMFFUP_e = 0x179,
    dRes_INDEX_LKANM_BTP_TDAMFBUP_e = 0x205,
    dRes_INDEX_LKANM_BTK_TDAMFBUP_e = 0x178,
    dRes_INDEX_LKANM_BTP_TDAMFLRUP_e = 0x207,
    dRes_INDEX_LKANM_BTK_TDAMFLUP_e = 0x17A,
    dRes_INDEX_LKANM_BTK_TDAMFRUP_e = 0x17B,
    dRes_INDEX_LKANM_BTP_TDAMDASH_e = 0x203,
    dRes_INDEX_LKANM_BTP_TGUARD_e = 0x219,
    dRes_INDEX_LKANM_BTK_TGUARD_e = 0x194,
    dRes_INDEX_LKANM_BTP_TSLIDEF_e = 0x253,
    dRes_INDEX_LKANM_BTK_TSLIDEF_e = 0x1BD,
    dRes_INDEX_LKANM_BTP_TSLIDEFLAND_e = 0x254,
    dRes_INDEX_LKANM_BTK_TSLIDEFLAND_e = 0x1BE,
    dRes_INDEX_LKANM_BTP_TSLIDEB_e = 0x251,
    dRes_INDEX_LKANM_BTP_TSLIDEBLAND_e = 0x252,
    dRes_INDEX_LKANM_BTP_TSWIMP_e = 0x25A,
    dRes_INDEX_LKANM_BTK_TSWIMP_e = 0x1C2,
    dRes_INDEX_LKANM_BTP_TSWIMING_e = 0x259,
    dRes_INDEX_LKANM_BTK_TSWIMING_e = 0x1C1,
    dRes_INDEX_LKANM_BTP_TBOOMTHROW_e = 0x1F4,
    dRes_INDEX_LKANM_BTK_TBOOMTHROW_e = 0x16A,
    dRes_INDEX_LKANM_BTP_TBOOMCATCH_e = 0x1F3,
    dRes_INDEX_LKANM_BTK_TBOOMCATCH_e = 0x169,
    dRes_INDEX_LKANM_BTP_TWALKPUSH_e = 0x26D,
    dRes_INDEX_LKANM_BTK_TWALKPUSH_e = 0x1CD,
    dRes_INDEX_LKANM_BTP_TWALKPULL_e = 0x26C,
    dRes_INDEX_LKANM_BTK_TWALKPULL_e = 0x1CC,
    dRes_INDEX_LKANM_BTP_TVJMPCHA_e = 0x267,
    dRes_INDEX_LKANM_BTK_TVJMPCHA_e = 0x1C8,
    dRes_INDEX_LKANM_BTP_TVJMPCL_e = 0x268,
    dRes_INDEX_LKANM_BTK_TVJMPCL_e = 0x1C9,
    dRes_INDEX_LKANM_BTP_TROPECLIMB_e = 0x24B,
    dRes_INDEX_LKANM_BTP_TROPEDOWN_e = 0x24C,
    dRes_INDEX_LKANM_BTP_TLADDERUPST_e = 0x228,
    dRes_INDEX_LKANM_BTK_TLADDERUPST_e = 0x1A2,
    dRes_INDEX_LKANM_BTP_TLADDERDWST_e = 0x226,
    dRes_INDEX_LKANM_BTK_TLADDERDWST_e = 0x1A0,
    dRes_INDEX_LKANM_BTP_TLADDERUPED_e = 0x227,
    dRes_INDEX_LKANM_BTK_TLADDERUPED_e = 0x1A1,
    dRes_INDEX_LKANM_BTP_TLADDERDWED_e = 0x225,
    dRes_INDEX_LKANM_BTK_TLADDERDWED_e = 0x19F,
    dRes_INDEX_LKANM_BTP_THANGING_e = 0x21E,
    dRes_INDEX_LKANM_BTK_THANGING_e = 0x199,
    dRes_INDEX_LKANM_BTP_THANGUP_e = 0x21F,
    dRes_INDEX_LKANM_BTK_THANGUP_e = 0x19A,
    dRes_INDEX_LKANM_BTP_TLAVADAM_e = 0x22B,
    dRes_INDEX_LKANM_BTK_TLAVADAM_e = 0x1A5,
    dRes_INDEX_LKANM_BTP_TCUTRE_e = 0x1FD,
    dRes_INDEX_LKANM_BTK_TCUTRE_e = 0x173,
    dRes_INDEX_LKANM_BTP_TCUTTURNB_e = 0x1FF,
    dRes_INDEX_LKANM_BTK_TCUTTURNB_e = 0x175,
    dRes_INDEX_LKANM_BTP_TWAITATOB_e = 0x26A,
    dRes_INDEX_LKANM_BTK_TWAITATOB_e = 0x1CA,
    dRes_INDEX_LKANM_BTP_TCOMEOUT_e = 0x1F8,
    dRes_INDEX_LKANM_BTK_TCOMEOUT_e = 0x16F,
    dRes_INDEX_LKANM_BTP_THOLDUP_e = 0x220,
    dRes_INDEX_LKANM_BTP_TWALLHOLDUP_e = 0x26E,
    dRes_INDEX_LKANM_BTK_TWALLHOLDUP_e = 0x1CE,
    dRes_INDEX_LKANM_BTP_TITEMGET_e = 0x221,
    dRes_INDEX_LKANM_BTK_TITEMGET_e = 0x19C,
    dRes_INDEX_LKANM_BTP_TSALTATION_e = 0x24D,
    dRes_INDEX_LKANM_BTK_TSALTATION_e = 0x1BA,
    dRes_INDEX_LKANM_BTP_TWHO_e = 0x276,
    dRes_INDEX_LKANM_BTK_TWHO_e = 0x1D6,
    dRes_INDEX_LKANM_BTP_TPICKUP_e = 0x242,
    dRes_INDEX_LKANM_BTK_TPICKUP_e = 0x1B1,
    dRes_INDEX_LKANM_BTP_TBOXOPENLINK_e = 0x1F6,
    dRes_INDEX_LKANM_BTK_TBOXOPENLINK_e = 0x16D,
    dRes_INDEX_LKANM_BTP_TBOXOPENSHORTLINK_e = 0x1F7,
    dRes_INDEX_LKANM_BTK_TBOXOPENSHORTLINK_e = 0x16E,
    dRes_INDEX_LKANM_BTP_TDIELONG_e = 0x209,
    dRes_INDEX_LKANM_BTK_TDIELONG_e = 0x17D,
    dRes_INDEX_LKANM_BTP_TSWIMDIE_e = 0x258,
    dRes_INDEX_LKANM_BTP_TSURPRISED_e = 0x256,
    dRes_INDEX_LKANM_BTK_TSURPRISED_e = 0x1BF,
    dRes_INDEX_LKANM_BTP_TTURNBACK_e = 0x263,
    dRes_INDEX_LKANM_BTK_TTURNBACK_e = 0x1C5,
    dRes_INDEX_LKANM_BTP_TLOOKUP_e = 0x22C,
    dRes_INDEX_LKANM_BTK_TLOOKUP_e = 0x1A6,
    dRes_INDEX_LKANM_BTP_TWAITQ_e = 0x26B,
    dRes_INDEX_LKANM_BTP_TGLAD_e = 0x213,
    dRes_INDEX_LKANM_BTP_TCUTBOKO_e = 0x1FA,
    dRes_INDEX_LKANM_BTK_TCUTBOKO_e = 0x170,
    dRes_INDEX_LKANM_BTP_SHIP_JUMP1_e = 0x1E4,
    dRes_INDEX_LKANM_BTP_SHIP_JUMP2_e = 0x1E5,
    dRes_INDEX_LKANM_BTP_TUSEFANA_e = 0x264,
    dRes_INDEX_LKANM_BTK_TUSEFANA_e = 0x1C6,
    dRes_INDEX_LKANM_BTP_TUSEFANB_e = 0x265,
    dRes_INDEX_LKANM_BTK_TUSEFANB_e = 0x1C7,
    dRes_INDEX_LKANM_BTP_TUSEFANB2_e = 0x266,
    dRes_INDEX_LKANM_BTP_TBOWWAIT_e = 0x1F5,
    dRes_INDEX_LKANM_BTK_TBOWWAIT_e = 0x16C,
    dRes_INDEX_LKANM_BTP_TVOMITJMP_e = 0x269,
    dRes_INDEX_LKANM_BTP_TREST_e = 0x246,
    dRes_INDEX_LKANM_BTK_TREST_e = 0x1B6,
    dRes_INDEX_LKANM_BTP_TTAKE_e = 0x25B,
    dRes_INDEX_LKANM_BTK_TTAKE_e = 0x1C4,
    dRes_INDEX_LKANM_BTP_TTAKELR_e = 0x25C,
    dRes_INDEX_LKANM_BTP_TSLIPICE_e = 0x255,
    dRes_INDEX_LKANM_BTP_THAMSWINGA_e = 0x21A,
    dRes_INDEX_LKANM_BTK_THAMSWINGA_e = 0x195,
    dRes_INDEX_LKANM_BTP_THAMSWINGBPRE_e = 0x21D,
    dRes_INDEX_LKANM_BTK_THAMSWINGBPRE_e = 0x198,
    dRes_INDEX_LKANM_BTP_THAMSWINGBHIT_e = 0x21C,
    dRes_INDEX_LKANM_BTK_THAMSWINGBHIT_e = 0x197,
    dRes_INDEX_LKANM_BTP_THAMSWINGBEND_e = 0x21B,
    dRes_INDEX_LKANM_BTK_THAMSWINGBEND_e = 0x196,
    dRes_INDEX_LKANM_BTP_TSETBOOTS_e = 0x24E,
    dRes_INDEX_LKANM_BTP_TDOOROPENALINK_e = 0x20A,
    dRes_INDEX_LKANM_BTK_TDOOROPENALINK_e = 0x17E,
    dRes_INDEX_LKANM_BTP_TDOOROPENBLINK_e = 0x20B,
    dRes_INDEX_LKANM_BTK_TDOOROPENBLINK_e = 0x17F,
    dRes_INDEX_LKANM_BTP_TSEYYES_e = 0x250,
    dRes_INDEX_LKANM_BTK_TSEYYES_e = 0x1BC,
    dRes_INDEX_LKANM_BTP_TPRESENTATION_e = 0x245,
    dRes_INDEX_LKANM_BTK_TPRESENTATIONA_e = 0x1B4,
    dRes_INDEX_LKANM_BTP_TWIND_e = 0x277,
    dRes_INDEX_LKANM_BTK_TWINDL_e = 0x1D7,
    dRes_INDEX_LKANM_BTK_TWINDR_e = 0x1D8,
    dRes_INDEX_LKANM_BTK_TPRESENTATIONB_e = 0x1B5,
    dRes_INDEX_LKANM_BTP_TBINDRINKPRE_e = 0x1EC,
    dRes_INDEX_LKANM_BTK_TBINDRINKPRE_e = 0x163,
    dRes_INDEX_LKANM_BTP_TBINDRINKING_e = 0x1EA,
    dRes_INDEX_LKANM_BTP_TBINDRINKAFTER_e = 0x1E8,
    dRes_INDEX_LKANM_BTK_TBINDRINKAFTER_e = 0x161,
    dRes_INDEX_LKANM_BTP_TBINDRINKPRE2_e = 0x1ED,
    dRes_INDEX_LKANM_BTP_TBINDRINKING2_e = 0x1EB,
    dRes_INDEX_LKANM_BTP_TBINDRINKAFTER2_e = 0x1E9,
    dRes_INDEX_LKANM_BTK_TBINDRINKAFTER2_e = 0x162,
    dRes_INDEX_LKANM_BTP_TBINOPENPRE_e = 0x1F1,
    dRes_INDEX_LKANM_BTK_TBINOPENPRE_e = 0x167,
    dRes_INDEX_LKANM_BTP_TBINOPENA_e = 0x1EF,
    dRes_INDEX_LKANM_BTK_TBINOPENA_e = 0x165,
    dRes_INDEX_LKANM_BTP_TBINOPENB_e = 0x1F0,
    dRes_INDEX_LKANM_BTK_TBINOPENB_e = 0x166,
    dRes_INDEX_LKANM_BTP_TBINSWING_e = 0x1F2,
    dRes_INDEX_LKANM_BTK_TBINSWING_e = 0x168,
    dRes_INDEX_LKANM_BTP_TBINGET_e = 0x1EE,
    dRes_INDEX_LKANM_BTK_TBINGET_e = 0x164,
    dRes_INDEX_LKANM_BTP_TARROWRELORD_e = 0x1E6,
    dRes_INDEX_LKANM_BTK_TARROWRELORD_e = 0x15E,
    dRes_INDEX_LKANM_BTP_TSURPRISEDB_e = 0x257,
    dRes_INDEX_LKANM_BTK_TSURPRISEDB_e = 0x1C0,
    dRes_INDEX_LKANM_BTP_TFAN_e = 0x20F,
    dRes_INDEX_LKANM_BTK_TFAN_e = 0x18C,
    dRes_INDEX_LKANM_BTP_TNENRIKI_e = 0x241,
    dRes_INDEX_LKANM_BTK_TEDL_e = 0x181,
    dRes_INDEX_LKANM_BTK_TABEKOBE_e = 0x15D,
    dRes_INDEX_LKANM_BTP_TSETHYOINOMI_e = 0x24F,
    dRes_INDEX_LKANM_BTK_TSETHYOINOMI_e = 0x1BB,
    dRes_INDEX_LKANM_BTP_LINK_FREEZ_e = 0x1DD,
    dRes_INDEX_LKANM_BTP_LINK_MOGAKI_e = 0x1DE,
    dRes_INDEX_LKANM_BTP_TDAMBIRI_e = 0x202,
    dRes_INDEX_LKANM_BTK_TDAMBIRI_e = 0x177,
    dRes_INDEX_LKANM_BTP_SALVLR_e = 0x1E1,
    dRes_INDEX_LKANM_BTK_SALVWAIT_e = 0x15B,
    dRes_INDEX_LKANM_BTP_SALVWAIT_e = 0x1E2,
    dRes_INDEX_LKANM_BTP_SALVBAD_e = 0x1DF,
    dRes_INDEX_LKANM_BTP_SALVGOOD_e = 0x1E0,
    dRes_INDEX_LKANM_BTP_SEARESET_e = 0x1E3,
    dRes_INDEX_LKANM_BTK_SEARESET_e = 0x15C,
    dRes_INDEX_LKANM_BTP_TWARPIN_e = 0x273,
    dRes_INDEX_LKANM_BTK_TWARPIN_e = 0x1D3,
    dRes_INDEX_LKANM_BTP_TWARPOUT_e = 0x274,
    dRes_INDEX_LKANM_BTK_TWARPOUT_e = 0x1D4,
    dRes_INDEX_LKANM_BTP_TPOWUPWAIT_e = 0x244,
    dRes_INDEX_LKANM_BTK_TPOWUPWAIT_e = 0x1B3,
    dRes_INDEX_LKANM_BTP_TPOWUP_e = 0x243,
    dRes_INDEX_LKANM_BTK_TPOWUP_e = 0x1B2,
    dRes_INDEX_LKANM_BTP_TCUTKESA_e = 0x1FC,
    dRes_INDEX_LKANM_BTK_TCUTKESA_e = 0x172,
    dRes_INDEX_LKANM_BTP_TWARPOUTFIRST_e = 0x275,
    dRes_INDEX_LKANM_BTK_TWARPOUTFIRST_e = 0x1D5,
    dRes_INDEX_LKANM_BTP_TDASHKAZE_e = 0x208,
    dRes_INDEX_LKANM_BTK_TDASHKAZE_e = 0x17C,
    dRes_INDEX_LKANM_BTP_TIYAYA_e = 0x222,
    dRes_INDEX_LKANM_BTP_TFREEA_e = 0x210,
    dRes_INDEX_LKANM_BTK_TFREEA_e = 0x18D,
    dRes_INDEX_LKANM_BTP_TFREEB_e = 0x211,
    dRes_INDEX_LKANM_BTK_TFREEB_e = 0x18E,
    dRes_INDEX_LKANM_BTP_TFREED_e = 0x212,
    dRes_INDEX_LKANM_BTK_TFREED_e = 0x18F,
    dRes_INDEX_LKANM_BTP_TTAKTKAZE_e = 0x261,
    dRes_INDEX_LKANM_BTP_TTAKTSIPPU_e = 0x262,
    dRes_INDEX_LKANM_BTP_TTAKTCHUYA_e = 0x25F,
    dRes_INDEX_LKANM_BTP_TTAKTFUJIN_e = 0x260,
    dRes_INDEX_LKANM_BTP_TTAKTAYATSURI_e = 0x25D,
    dRes_INDEX_LKANM_BTP_TTAKTCHISIN_e = 0x25E,
};
struct TexAnmTableEntry { be<u16> mBtpIdx, mBtkIdx; };
WWHD_SIZE(TexAnmTableEntry, 4);
struct anmHeap_c {
    be<u16> mIdx;
    be<u16> field_0x2;
    be<u16> field_0x4;
    be<u16> field_0x6;
    be<u32> m_buffer;
    be<u32> mpAnimeHeap;
};
WWHD_OFFSET(anmHeap_c, mIdx, 0);
WWHD_OFFSET(anmHeap_c, field_0x2, 2);
WWHD_OFFSET(anmHeap_c, field_0x4, 4);
WWHD_OFFSET(anmHeap_c, field_0x6, 6);
WWHD_OFFSET(anmHeap_c, m_buffer, 8);
WWHD_OFFSET(anmHeap_c, mpAnimeHeap, 0xC);
WWHD_SIZE(anmHeap_c, 0x10);
inline anmHeap_c* heaps(u8* bytes) { return gabi::at<anmHeap_c>(gabi::ea(bytes)); }

// HD: animation ratios use a counted array whose count and pointer are reloaded each iteration.
struct anmRatioPack_c {
    be<u32> __vtbl;
    be<u32> mpAnmTransform;
    be<s32> mCount;
    gptr<be<f32>> mpRatio;
};
WWHD_OFFSET(anmRatioPack_c, mpAnmTransform, 4);
WWHD_OFFSET(anmRatioPack_c, mCount, 8);
WWHD_OFFSET(anmRatioPack_c, mpRatio, 0xC);
WWHD_SIZE(anmRatioPack_c, 0x10);

struct hdItemState_c {
    be<u32> mItemTriggerLock;
    u8 reserved[0x18];
    be<u8> mBoomerangAim;
    u8 padding;
    be<s16> mAimAngleY;
    u8 trailing[4];
};
WWHD_OFFSET(hdItemState_c, mItemTriggerLock, 0);
WWHD_OFFSET(hdItemState_c, mBoomerangAim, 0x1C);
WWHD_OFFSET(hdItemState_c, mAimAngleY, 0x1E);
WWHD_SIZE(hdItemState_c, 0x24);
inline hdItemState_c* itemState(daPy_lk_c* player) {
    return gabi::at<hdItemState_c>(gabi::ea(player->m8260));
}
}

namespace player02 {
// GameCube names; resource and procedure values follow the verified HD build.
enum : u32 {
    ANM_ATNDB = 0xF,
    ANM_ATNDLS = 0xB,
    ANM_ATNDRS = 0xC,
    ANM_ATNLS = 0x7,
    ANM_ATNRS = 0x8,
    ANM_ATNWB = 0xE,
    ANM_ATNWLS = 0x9,
    ANM_ATNWRS = 0xA,
    ANM_BOOMCATCH = 0x7C,
    ANM_CUTA = 0x1E,
    ANM_CUTEA = 0x22,
    ANM_CUTEB = 0x23,
    ANM_CUTF = 0x1F,
    ANM_CUTKESA = 0xDE,
    ANM_CUTL = 0x21,
    ANM_CUTR = 0x20,
    ANM_CUTTURN = 0x28,
    ANM_CUTTURNC = 0x29,
    ANM_EXCA1 = 0x24,
    ANM_EXCB1 = 0x25,
    ANM_GRABWAIT = 0x68,
    ANM_GRABWAITB = 0x69,
    ANM_MJMP = 0x6C,
    ANM_MSTEPOVER = 0x72,
    ANM_NENRIKI = 0xC3,
    ANM_SHIP_JUMP1 = 0x9F,
    ANM_SHIP_JUMP2 = 0xA0,
    ANM_SLIDEB = 0x37,
    ANM_SLIDEF = 0x35,
    ANM_SLIPICE = 0xA9,
    ANM_USEFANA = 0xA1,
    ANM_VOMITJMP = 0xA7,
    ANM_VOYAGE1 = 0x7D,
    ANM_WAITATOB = 0x1D,
    ANM_WAITQ = 0x9D,
    ANM_WAITS = 0x0,
    AT_TYPE_FAN_SWING = 0x2000000,
    AT_TYPE_SWORD = 0x2,
    CUT_TYPE_CUT_A = 0x1,
    CUT_TYPE_CUT_EA = 0x6,
    CUT_TYPE_CUT_EB = 0x7,
    CUT_TYPE_CUT_EXA = 0x1A,
    CUT_TYPE_CUT_EXB = 0x1B,
    CUT_TYPE_CUT_EXMJ = 0x1E,
    CUT_TYPE_CUT_F = 0x2,
    CUT_TYPE_CUT_KESA = 0x1F,
    CUT_TYPE_CUT_L = 0x4,
    CUT_TYPE_CUT_R = 0x3,
    CUT_TYPE_NONE = 0x0,
    DIR_BACKWARD = 0x1,
    DIR_FORWARD = 0x0,
    DIR_LEFT = 0x2,
    DIR_NONE = 0x4,
    DIR_RIGHT = 0x3,
    UPPER_MOVE2_e = 0x2,
    dCcG_AtHitMark_Big_e = 0xF,
    dCcG_AtHitMark_Nrm_e = 0xD,
    dCcG_At_Spl_UNK0 = 0x0,
    dCcG_At_Spl_UNK1 = 0x1,
    dCcG_SE_UNK1 = 0x1,
    dCcG_SE_WOOD = 0x4,
    dRes_INDEX_LINK_BDL_FAN_e = 0x19,
    dRes_INDEX_LKANM_BCK_ARROWSHOOTA_e = 0xF,
    dRes_INDEX_LKANM_BCK_ARROWSHOOT_e = 0xE,
    dRes_INDEX_LKANM_BCK_BOOMCATCH_e = 0x33,
    dRes_INDEX_LKANM_BCK_CUTAA_e = 0x40,
    dRes_INDEX_LKANM_BCK_CUTAMS_e = 0x41,
    dRes_INDEX_LKANM_BCK_CUTEAA_e = 0x44,
    dRes_INDEX_LKANM_BCK_CUTEAMS_e = 0x45,
    dRes_INDEX_LKANM_BCK_CUTEBA_e = 0x47,
    dRes_INDEX_LKANM_BCK_CUTEBMS_e = 0x48,
    dRes_INDEX_LKANM_BCK_CUTFA_e = 0x4A,
    dRes_INDEX_LKANM_BCK_CUTFMS_e = 0x4B,
    dRes_INDEX_LKANM_BCK_CUTLA_e = 0x50,
    dRes_INDEX_LKANM_BCK_CUTLMS_e = 0x51,
    dRes_INDEX_LKANM_BCK_CUTRA_e = 0x53,
    dRes_INDEX_LKANM_BCK_CUTRMS_e = 0x57,
    dRes_INDEX_LKANM_BCK_CUTTURNA_e = 0x59,
    dRes_INDEX_LKANM_BCK_CUTTURNMS_e = 0x5C,
    dRes_INDEX_LKANM_BCK_EXCA1A_e = 0x7E,
    dRes_INDEX_LKANM_BCK_EXCA1MS_e = 0x7F,
    dRes_INDEX_LKANM_BCK_EXCB1A_e = 0x81,
    dRes_INDEX_LKANM_BCK_EXCB1MS_e = 0x82,
    dRes_INDEX_LKANM_BCK_GRABWAITB_e = 0x96,
    dRes_INDEX_LKANM_BCK_GRABWAIT_e = 0x95,
    dRes_INDEX_LKANM_BCK_HOOKSHOTWAIT_e = 0xA7,
    dRes_INDEX_LKANM_BCK_MROLLCA_e = 0xC7,
    dRes_INDEX_LKANM_BCK_MROLLCMS_e = 0xC8,
    dRes_INDEX_LKANM_BCK_SHIP_JUMP2_e = 0xF4,
    dRes_INDEX_LKANM_BCK_USEFANAA_e = 0x112,
    dRes_INDEX_LKANM_BCK_VOYAGE1_e = 0x11B,
    dRes_INDEX_LKANM_BCK_WAITQ_e = 0x125,
    dRes_INDEX_LKANM__BTROTATECUTL_POS_e = 0x27B,
    dRes_INDEX_LKANM__BTROTATECUTR_POS_e = 0x27C,
    dRes_INDEX_LKANM__CUTA_POS_e = 0x27D,
    dRes_INDEX_LKANM__CUTEA_POS_e = 0x27F,
    dRes_INDEX_LKANM__CUTEB_POS_e = 0x280,
    dRes_INDEX_LKANM__CUTEXA_POS_e = 0x281,
    dRes_INDEX_LKANM__CUTEXB_POS_e = 0x282,
    dRes_INDEX_LKANM__CUTKESA_POS_e = 0x284,
    dRes_INDEX_LKANM__CUTL_POS_e = 0x285,
    dRes_INDEX_LKANM__CUTR_POS_e = 0x286,
    dRes_INDEX_LKANM__CUTTURN_POS_e = 0x287,
    dRes_INDEX_LKANM__WEAPONTURN_POS_e = 0x28C,
    daPyProc_BOOMERANG_CATCH_e = 0x82,
    daPyProc_BOW_MOVE_e = 0x95,
    daPyProc_BOW_SUBJECT_e = 0x94,
    daPyProc_BT_JUMP_e = 0x5D,
    daPyProc_BT_ROLL_e = 0x60,
    daPyProc_BT_VERTICAL_JUMP_e = 0x62,
    daPyProc_CONTROLL_WAIT_e = 0x3,
    daPyProc_CUT_A_e = 0x41,
    daPyProc_CUT_EA_e = 0x45,
    daPyProc_CUT_EB_e = 0x46,
    daPyProc_CUT_EX_A_e = 0x47,
    daPyProc_CUT_EX_B_e = 0x48,
    daPyProc_CUT_EX_MJ_e = 0x49,
    daPyProc_CUT_F_e = 0x42,
    daPyProc_CUT_KESA_e = 0x4A,
    daPyProc_CUT_L_e = 0x44,
    daPyProc_CUT_R_e = 0x43,
    daPyProc_CUT_TURN_e = 0x55,
    daPyProc_FAN_SWING_e = 0x92,
    daPyProc_FREE_WAIT_e = 0x5,
    daPyProc_GRAB_WAIT_e = 0x73,
    daPyProc_HOOKSHOT_FLY_e = 0x85,
    daPyProc_HOOKSHOT_MOVE_e = 0x84,
    daPyProc_HOOKSHOT_SUBJECT_e = 0x83,
    daPyProc_ROPE_SWING_e = 0x78,
    daPyProc_SCOPE_e = 0x0,
    daPyProc_SLIDE_BACK_e = 0x1B,
    daPyProc_SLIDE_FRONT_e = 0x1A,
    daPyProc_VOMIT_JUMP_e = 0x98,
    daPyProc_WAIT_e = 0x4,
    daPyStts0_BOW_AIM_e = 0x1000,
    daPyStts0_HOOKSHOT_AIM_e = 0x4000,
    daPyStts0_SHIP_RIDE_e = 0x10000,
    daPyStts0_SWORD_SWING_e = 0x8000,
    daPyStts0_UNK2_e = 0x2,
    daPyStts0_UNK80000000_e = 0x80000000,
    daPyStts1_DEKU_LEAF_FAN_e = 0x40,
    daPyStts1_UNK100_e = 0x100,
    daPyStts1_UNK200_e = 0x200,
};
}

// Boundaries whose callers preserve full guest register widths.
BOOL player02AttentionLock(u32 attention);
void player02SystemSound(u32 sound);
BOOL player02PolySafe(dBgS* bg, u32 poly);
s32 player02SpecialCode(dBgS* bg, u32 poly);
s32 player02WallCode(dBgS* bg, u32 poly);
s32 player02GroundCode(dBgS* bg, u32 poly);
f32 player02AddCalc(be<f32>* value, f32 target, f32 scale, f32 maxStep, f32 minStep);
u32 player02RelativeData(u32 header);
s32 player02AnimationCount(u32 resource, u32 index);

namespace player02 {
template<class T> inline T& indexed(T* values, u32 index) {
    return *gabi::at<T>(gabi::ea(values) + index * sizeof(T));
}
struct animationResource_c {
    u8 reserved[0xAC];
    be<u32> mpRelativeHeader;
};
WWHD_OFFSET(animationResource_c, mpRelativeHeader, 0xAC);
WWHD_SIZE(animationResource_c, 0xB0);
struct animationRatioTable_c {
    u8 reserved[0x2C];
    gptr<be<f32>> mpRatio;
};
WWHD_OFFSET(animationRatioTable_c, mpRatio, 0x2C);
WWHD_SIZE(animationRatioTable_c, 0x30);
struct pbCalc_c {
    u8 reserved[0x7C];
    gptr<animationRatioTable_c> mpRatioTable;
    gptr<animationResource_c> mpResource;
};
WWHD_OFFSET(pbCalc_c, mpRatioTable, 0x7C);
WWHD_OFFSET(pbCalc_c, mpResource, 0x80);
WWHD_SIZE(pbCalc_c, 0x84);
}

namespace player02 {
struct oldFrame_c {
    be<u8> mOldFrameFlg;
    u8 reserved[0x1F];
    gptr<Quaternion> mpOldFrameQuaternion;
};
WWHD_OFFSET(oldFrame_c, mOldFrameFlg, 0);
WWHD_OFFSET(oldFrame_c, mpOldFrameQuaternion, 0x20);
WWHD_SIZE(oldFrame_c, 0x24);

// The ship members used while Link rides or aims an item.
struct ship_c : fopAc_ac_c {
    u8 reserved[0x3B4 - sizeof(fopAc_ac_c)];
    gptr<mDoExt_McaMorf> mpBodyMorf;
    u8 bodyPadding[0x636 - 0x3B8];
    be<u8> mPaddleMode;
    u8 flagPadding[0x644 - 0x637];
    be<u32> mFlags;
    u8 tillerPadding[0x6C0 - 0x648];
    be<f32> mTillerAngleRate;
    u8 jumpPadding[4];
    be<f32> mJumpRate;
};
WWHD_OFFSET(ship_c, mpBodyMorf, 0x3B4);
WWHD_OFFSET(ship_c, mPaddleMode, 0x636);
WWHD_OFFSET(ship_c, mFlags, 0x644);
WWHD_OFFSET(ship_c, mTillerAngleRate, 0x6C0);
WWHD_OFFSET(ship_c, mJumpRate, 0x6C8);
WWHD_SIZE(ship_c, 0x6CC);
}

namespace player02 {
inline cM3dGPla* plane(u32 address) { return gabi::at<cM3dGPla>(address); }
inline cBgS_GndChk* ground(u32 address) { return gabi::at<cBgS_GndChk>(address); }
inline cBgS_LinChk* line(u32 address) { return gabi::at<cBgS_LinChk>(address); }
}

void player02EulerToQuat(s32 x, s32 y, s32 z, u32 out);
void player02QuatConcat(u32 a, u32 b, u32 out);
void player02AllWind(const cXyz* position, u32 direction, u32 power);
int player02BckInit(u32 animation, u32 data, u32 bck, s32 play, s32 mode, f32 rate, s32 start, s32 end, s32 modify);
void player02RestoreHeap(u32 heap);
void player02SafeStringCtor(u32 string);
void player02FrameApply(u32 function);
f64 player02GroundCross(dBgS* bg, u32 check);
f64 player02RoofCheck(dBgS* bg, u32 check);
void player02Normalize(u32 in, u32 out);
void player02PlaneCtor(u32 plane, u32 normal, f32 distance);
BOOL player02ThreePlaneCross(u32 wall, u32 ground, u32 plane, u32 out);
f64 player02SignedPlaneDistance(u32 plane, const cXyz* position);
void player02TrianglePoints(dBgS* bg, u32 poly, u32 a, u32 b, u32 c);
void player02SeaColor(u32 ambient, u32 diffuse);
void player02OldFrameMorf(u32 oldFrame, f32 morf, s32 start, s32 end);
void player02SwordBck(u32 animation, u32 bck);

namespace player02 {
struct AnmDataTableEntry {
    u8 mAnmIdx[4];
    be<u8> mLeftHandIdx, mRightHandIdx;
    be<u16> mTexAnmIdx;
};
WWHD_OFFSET(AnmDataTableEntry, mTexAnmIdx, 6);
WWHD_SIZE(AnmDataTableEntry, 8);
inline AnmDataTableEntry& animationData(int index) {
    return indexed(gabi::at<AnmDataTableEntry>(0x100366A0), (u32)index);
}
struct animationVtable_c {
    u8 reserved0[0xC];
    be<u32> getAttribute;
    u8 reserved10[4];
    be<u32> getFrameMax;
};
WWHD_SIZE(animationVtable_c, 0x18);
WWHD_OFFSET(animationVtable_c, getAttribute, 0xC);
WWHD_OFFSET(animationVtable_c, getFrameMax, 0x14);
struct animation_c {
    be<f32> mFrame;
    gptr<animationVtable_c> mpVtable;
};
WWHD_OFFSET(animation_c, mpVtable, 4);
WWHD_SIZE(animation_c, 8);
inline f32 wallHeightBias() { return *gabi::at<be<f32>>(0x1047C780); }
struct attention_c {
    u8 reserved[0x20];
    be<u32> mFlags;
};
WWHD_SIZE(attention_c, 0x24);
WWHD_OFFSET(attention_c, mFlags, 0x20);
struct playerVtable_c {
    u8 reserved0[0x3C];
    be<u32> checkPlayerGuard;
    u8 reserved40[0xA4];
    be<u32> voiceStart;
    u8 reservedE8[0x4C];
    be<u32> changeTextureAnime;
};
WWHD_SIZE(playerVtable_c, 0x138);
WWHD_OFFSET(playerVtable_c, checkPlayerGuard, 0x3C);
WWHD_OFFSET(playerVtable_c, voiceStart, 0xE4);
WWHD_OFFSET(playerVtable_c, changeTextureAnime, 0x134);
inline void voiceStart(daPy_lk_c* player, u32 sound) {
    gabi::call_ptr(gabi::at<playerVtable_c>(player->__vtbl)->voiceStart, player, sound);
}
inline BOOL checkPlayerGuard(daPy_lk_c* player) {
    return gabi::call_ptr<BOOL>(gabi::at<playerVtable_c>(player->__vtbl)->checkPlayerGuard, player);
}
inline void changeTextureAnime(daPy_lk_c* player, u32 btp, u32 btk, int mode) {
    gabi::call_ptr(gabi::at<playerVtable_c>(player->__vtbl)->changeTextureAnime, player, btp, btk, mode);
}
inline u32 bokoAttackPoint(u32 type) {
    return (u32)indexed(gabi::at<be<s32>>(0x10192138), type);
}
struct vectorWords_c { be<u32> x, y, z; };
WWHD_SIZE(vectorWords_c, 0xC);
inline void copyZero(cXyz* destination) {
    vectorWords_c* to = gabi::at<vectorWords_c>(gabi::ea(destination));
    vectorWords_c* zero = gabi::at<vectorWords_c>(gabi::ea(cXyz_Zero));
    to->x = zero->x;
    to->y = zero->y;
    to->z = zero->z;
}
struct animationFrameFunction_c {
    be<f32> mResult;
    be<f32> mParam2;
    be<f32> mParam3;
    u8 reserved[4];
    be<u32> mEvaluate;
    be<u32> mArgument;
};
WWHD_OFFSET(animationFrameFunction_c, mEvaluate, 0x10);
WWHD_OFFSET(animationFrameFunction_c, mArgument, 0x14);
WWHD_SIZE(animationFrameFunction_c, 0x18);
struct bpkFrame_c {
    u8 reserved0[4];
    be<f32> mFrame;
    u8 reserved8[8];
    gptr<be<f32>> mpFrame;
    u8 reserved14[4];
    gptr<animationFrameFunction_c> mpFunction;
};
WWHD_SIZE(bpkFrame_c, 0x1C);
WWHD_OFFSET(bpkFrame_c, mpFrame, 0x10);
WWHD_OFFSET(bpkFrame_c, mpFunction, 0x18);
struct btkFrame_c {
    u8 reserved0[4];
    be<f32> mFrame;
    u8 reserved8[8];
    gptr<animationFrameFunction_c> mpFunction;
    u8 reserved14[0x54];
    gptr<be<f32>> mpFrame;
};
WWHD_SIZE(btkFrame_c, 0x6C);
WWHD_OFFSET(btkFrame_c, mpFunction, 0x10);
WWHD_OFFSET(btkFrame_c, mpFrame, 0x68);
struct swordBlur_c {
    u8 reserved[0xAC];
    be<u32> mpPosBuffer;
};
WWHD_SIZE(swordBlur_c, 0xB0);
WWHD_OFFSET(swordBlur_c, mpPosBuffer, 0xAC);
struct materialName_c {
    u8 reserved[4];
    be<s32> mNameOffset;
};
WWHD_SIZE(materialName_c, 0x8);
WWHD_OFFSET(materialName_c, mNameOffset, 4);
struct fanMaterial_c {
    gptr<materialName_c> mpName;
    gptr<fanMaterial_c> mpNext;
    gptr<J3DShape> mpShape;
};
WWHD_OFFSET(fanMaterial_c, mpNext, 4);
WWHD_OFFSET(fanMaterial_c, mpShape, 8);
WWHD_SIZE(fanMaterial_c, 0xC);
struct fanJoint_c {
    u8 reserved[0x10];
    gptr<fanMaterial_c> mpMesh;
};
WWHD_SIZE(fanJoint_c, 0x14);
WWHD_OFFSET(fanJoint_c, mpMesh, 0x10);
struct safeStringVtable_c {
    u8 reserved[0x14];
    be<u32> assureTermination;
};
WWHD_SIZE(safeStringVtable_c, 0x18);
WWHD_OFFSET(safeStringVtable_c, assureTermination, 0x14);
inline void assureTermination(SafeString* string) {
    gabi::call_ptr(gabi::at<safeStringVtable_c>(string->__vtbl)->assureTermination, string);
}
struct cutState_c {
    be<u8> mCutType;
    be<u8> mCutCount;
};
WWHD_SIZE(cutState_c, 2);
inline cutState_c* cutState(daPy_lk_c* player) {
    return gabi::at<cutState_c>(gabi::ea(&player->mCutType));
}
struct equipment_c {
    gptr<J3DModel> mpEquipItemModel;
    mDoExt_bckAnm mSwordAnim;
};
WWHD_OFFSET(equipment_c, mpEquipItemModel, 0);
WWHD_OFFSET(equipment_c, mSwordAnim, 4);
WWHD_SIZE(equipment_c, 0x90);
inline equipment_c* equipment(daPy_lk_c* player) {
    return gabi::at<equipment_c>(gabi::ea(player) + 0x4440);
}
struct capsule_c {
    u8 reserved[0x1C];
    be<f32> mRadius;
};
WWHD_OFFSET(capsule_c, mRadius, 0x1C);
WWHD_SIZE(capsule_c, 0x20);
inline void setCapsuleRadius(dCcD_Cps* capsule, f32 radius) {
    gabi::at<capsule_c>(gabi::ea(capsule->mCps))->mRadius = radius;
}
}

// HD: private bindings preserve caller register widths for range 02 item-ready code.
BOOL player02EventCompulsory(u32 event, daPy_lk_c* player, u32 partner, u32 eventID);
u32 player02TalkPartner(daPy_lk_c* player);
u32 player02CameraBody();
void player02StartEventCamera(u32 camera, u32 eventID, u32 actorID, u32 flags);
void player02TactReset();
void player02TactSetBeat(int beat);
void player02TactSetVolume(f32 volume);
void player02TactAmbientPlay();
void player02TaktModeMute();
u32 player02HintActor(u32 attention, u32 actorID);
void player02DoButtonQuake(daPy_lk_c* player);
BOOL player02IsEsa(u32 item);
void player02OrderZHint(daPy_lk_c* player, u32 actor);
void player02OrderDoor(daPy_lk_c* player, fopAc_ac_c* actor);
void player02SetWallRadius(dBgS_AcchCir* circle, f32 radius);
fopAc_ac_c* player02CreateArrow(u32 process, u32 parameters, const cXyz* position, s32 room, void* angle, void* scale, int subtype, int flags, int extra);
void player02KeepSetData(daPy_actorKeep_l* keep, fopAc_ac_c* actor);
void player02KeepClearData(daPy_actorKeep_l* keep);
BOOL player02ChangeArrowMp();
u32 player02ChangeArrowType(u32 arrow);
void player02ChangeArrowTypeNotReady();
void player02RopeTarget(u32 rope, u32 target, be<f32>* distance, be<f32>* angle);

namespace player02 {
// HD: pad release bits are read through a pointer, outside the older input-state prefix.
struct padRelease_c {
    u8 reserved[0x1C];
    be<u32> mRelease;
};
WWHD_OFFSET(padRelease_c, mRelease, 0x1C);
WWHD_SIZE(padRelease_c, 0x20);
struct readyVtable_c {
    u8 beforeTact[0x84];
    be<u32> mSetTactZev;
    u8 beforeRope[0xDC - 0x88];
    be<u32> mCheckRopeReadyAnime;
};
WWHD_OFFSET(readyVtable_c, mSetTactZev, 0x84);
WWHD_OFFSET(readyVtable_c, mCheckRopeReadyAnime, 0xDC);
WWHD_SIZE(readyVtable_c, 0xE0);
inline void initializeScale(be<u32>& guard, cXyz& scale, f32 x, f32 y, f32 z) {
    if (guard == 0) {
        guard = 1;
        scale.x = x;
        scale.z = z;
        scale.y = y;
    }
}
} // namespace player02

namespace player02 {
// HD: only the cancel byte is needed from the large boomerang actor.
struct boomerangCancel_c {
    u8 beforeCancel[0x26274];
    be<u8> mCancelFlg;
};
WWHD_OFFSET(boomerangCancel_c, mCancelFlg, 0x26274);
WWHD_SIZE(boomerangCancel_c, 0x26275);
struct sightPacket_c {
    be<u32> mVtable;
    be<u8> mDrawFlg;
};
WWHD_OFFSET(sightPacket_c, mDrawFlg, 4);
WWHD_SIZE(sightPacket_c, 5);
struct ropeMode_c {
    u8 beforeMode[0x3F8];
    be<s32> m02DC;
};
WWHD_OFFSET(ropeMode_c, m02DC, 0x3F8);
WWHD_SIZE(ropeMode_c, 0x3FC);
inline f32 ropeCatchHeight() { return *gabi::at<be<f32>>(0x1046CCEC); }
} // namespace player02

// HD: partial views used only by Link's talk, carry and combat routines.
#include "gc_api/f_op/f_op_actor.h"

namespace player02tail {
struct BokoByteTable {
    be<u8> mValues[6];
};
WWHD_SIZE(BokoByteTable, 6);
} // namespace player02tail

namespace player02tail {
struct dAttListView {
    u8 beforeType[8];
    be<s32> mType;
};
WWHD_OFFSET(dAttListView, mType, 8);
WWHD_SIZE(dAttListView, 0xC);
struct BokoCarryView {
    u8 beforeMode[0x430];
    be<u8> mNowMode;
};
WWHD_OFFSET(BokoCarryView, mNowMode, 0x430);
WWHD_SIZE(BokoCarryView, 0x431);
struct BottleAttentionView {
    u8 beforeCatch[0x5934];
    u8 mCatch[0x10];
    be<u32> mCatghTargetID;
};
WWHD_OFFSET(BottleAttentionView, mCatch, 0x5934);
WWHD_OFFSET(BottleAttentionView, mCatghTargetID, 0x5944);
WWHD_SIZE(BottleAttentionView, 0x5948);
struct ShipModelView {
    u8 beforeBody[0x90];
    gptr<J3DModel> mBody;
};
WWHD_OFFSET(ShipModelView, mBody, 0x90);
WWHD_SIZE(ShipModelView, 0x94);
struct ShipView {
    u8 beforeModel[0x3B4];
    gptr<ShipModelView> mpModel;
    u8 beforeNext[0x636 - 0x3B8];
    be<u8> mNextMode;
};
WWHD_OFFSET(ShipView, mpModel, 0x3B4);
WWHD_OFFSET(ShipView, mNextMode, 0x636);
WWHD_SIZE(ShipView, 0x637);
struct VecWords {
    be<u32> x, y, z;
};
WWHD_SIZE(VecWords, 0xC);
// HD: these guest word views preserve signalling NaNs and observed copy order.
inline VecWords* words(cXyz* v) { return reinterpret_cast<VecWords*>(v); }
inline const VecWords* words(const cXyz* v) { return reinterpret_cast<const VecWords*>(v); }
inline void copyXYZ(cXyz* to, const cXyz* from) {
    words(to)->x = gabi::load<u32>(gabi::ea(from));
    words(to)->y = gabi::load<u32>(gabi::ea(from) + 4);
    words(to)->z = gabi::load<u32>(gabi::ea(from) + 8);
}
inline void copyZXY(cXyz* to, const cXyz* from) {
    words(to)->z = gabi::load<u32>(gabi::ea(from) + 8);
    words(to)->x = gabi::load<u32>(gabi::ea(from));
    words(to)->y = gabi::load<u32>(gabi::ea(from) + 4);
}
inline dAttListView* entry(u32 ea) { return gabi::at<dAttListView>(ea); }
// HD: raw actor-status reads precede the source's null check; guest zero remains readable.
inline u32 actorStatus(fopAc_ac_c* actor) { return gabi::load<u32>(gabi::ea(actor) + 0x2E0); }
inline void setDoStatusIfBlank(u8 status) {
    if (dComIfGp_getDoStatus() == 0)
        dComIfGp_setDoStatus(status);
}
inline void setAStatusIfBlank(u8 status) {
    if (dComIfGp_getAStatus() == 0)
        dComIfGp_setAStatus(status);
}
// HD: native parameters supply full 32-bit indices. Preserve guest address wrapping for malformed indices.
u8 cutType(u32 type);
u32 atType(u32 type);
u32 atPoint(u32 type);
u8 seType(u32 type);
f32 cpsRadius(u32 type);
} // namespace player02tail
// HD: this comparison uses the complete FPR result, not the shared f32 convenience binding.
f64 player02TailDistanceXZ(fopAc_ac_c*, fopAc_ac_c*);
void player02TailSetCarryNow(fopAc_ac_c*, BOOL);
fopAc_ac_c* player02TailCatchTarget(void*, u32);
BOOL player02TailBottle(daPy_lk_c*, u32);
BOOL player02TailPhotoBox(daPy_lk_c*, u32);
BOOL player02TailBow(daPy_lk_c*, u32);
namespace player02tail {
enum : u32 {
    dActStts_LOOK_e = 1,
    dActStts_SPEAK_e = 2,
    daPyItem_SWORD_e = 0x103,
    daPyItem_BOKO_e = 0x101,
    dItemNo_SKULL_HAMMER_e = 0x33,
    daPyItem_NONE_e = 0x100,
    fpcNm_BOKO_e = 0x1CF,
    dRes_INDEX_LKANM_BCK_REST_e = 0xD7,
    dItemNo_BOOMERANG_e = 0x2D,
    dItemNo_GRAPPLING_HOOK_e = 0x25,
    dItemNo_TELESCOPE_e = 0x20,
    dItemNo_HOOKSHOT_e = 0x2F,
    dItemNo_DEKU_LEAF_e = 0x34,
    dRes_INDEX_LKANM_BCK_TAKEL_e = 0x105,
    dRes_INDEX_LKANM_BCK_TAKER_e = 0x106,
    dRes_INDEX_LKANM_BCK_TAKEBOTH_e = 0x104,
    dRes_INDEX_LKANM_BCK_TAKE_e = 0x103,
    daPyProc_PUSH_PULL_WAIT_e = 0x32,
    ANM_WAITPUSHPULL = 0x7E,
    daPyStts0_UNK4000000_e = 0x4000000,
    daPyProc_GRAB_READY_e = 0x6E,
    ANM_GRABP = 0x65,
    dCcG_At_Spl_UNK1 = 1,
    dCcG_At_Spl_UNK0 = 0,
    AT_TYPE_SWORD = 2,
    dCcG_SE_UNK1 = 1,
    CUT_TYPE_JUMPCUT_SWORD = 0xA,
    AT_TYPE_SKULL_HAMMER = 0x10000,
    dCcG_SE_METAL = 5,
    CUT_TYPE_JUMPCUT_HAMMER = 0x13,
    AT_TYPE_MACHETE = 0x400,
    CUT_TYPE_JUMPCUT_MACHETE = 0xE,
    daPyProc_JUMP_CUT_e = 0x5B,
    ANM_JATTACK = 0x2B,
    dRes_INDEX_LKANM__WEAPONJUMP_POS_e = 0x28B,
    dRes_INDEX_LKANM__CUTJUMP_POS_e = 0x283,
    daPyProc_WHIDE_READY_e = 0x13,
    ANM_WALL = 0x42,
    daPyStts0_UNK1_e = 1,
    daPyProc_CUT_TURN_CHARGE_e = 0x58,
    ANM_CUTTURNP = 0x18,
    daPyProc_WEAPON_NORMAL_SWING_e = 0x4B,
    ANM_CUTBOKO = 0x26,
    dRes_INDEX_LKANM__CUTBOKO_POS_e = 0x27E,
    daPyProc_WEAPON_SIDE_SWING_e = 0x4C,
    ANM_HAMSWINGA = 0xAA,
    dRes_INDEX_LKANM__HAMMERSIDE_POS_e = 0x289,
    daPyProc_WEAPON_FRONT_SWING_READY_e = 0x4D,
    ANM_HAMSWINGBPRE = 0xAB,
    daPyProc_WEAPON_THROW_e = 0x50,
    daPyProc_BOTTLE_SWING_e = 0xA5,
    dItemNo_EMPTY_BOTTLE_e = 0x50,
    ANM_BINSWINGU = 0xBD,
    ANM_BINSWINGS = 0xBC,
    daPyProc_SHIP_READY_e = 0x86,
    ANM_VJMPCL = 0x4F,
    daPyStts0_UNK1000000_e = 0x1000000,
    dActStts_READ_e = 0x26,
    dActStts_CHECK_e = 0xA,
};
}

using Player02TailOrderTalk = void (*)(daPy_lk_c*, fopAc_ac_c*);
BOOL player02TailIsEsa(u32);
void player02TailSystemSound(u32);
void player02TailOrderTalkEvent(daPy_lk_c*, fopAc_ac_c*);
void player02TailOrderTalkX(daPy_lk_c*, fopAc_ac_c*);
void player02TailOrderTalkY(daPy_lk_c*, fopAc_ac_c*);
void player02TailOrderTalkZ(daPy_lk_c*, fopAc_ac_c*);
void player02TailOrderTalkHD(daPy_lk_c*, fopAc_ac_c*);

// HD: attention flags can also be read through a null guest actor.
namespace player02tail { inline u32 attentionFlags(fopAc_ac_c* actor) { return gabi::load<u32>(gabi::ea(actor)+0x39C); } }

namespace player02tail {
// HD: nested model lookups read guest zero and use 32-bit address arithmetic;
// a host pointer member chain would dereference nullptr when the intermediate pointer is zero.
inline J3DModel* shipBodyModel(fopAc_ac_c* ship) {
    u32 shipModel = gabi::load<u32>(gabi::ea(ship) + 0x3B4);
    u32 bodyModel = gabi::load<u32>(shipModel + 0x90);
    return gabi::at<J3DModel>(bodyModel);
}
}

namespace player02tail {
// HD: borrowed daBoko tables retain their owning TU; this helper only computes an indexed guest address.
// Full-width unsigned indexing wraps before the guest load, which also permits guest address zero.
template<class T> inline u32 borrowedIndexEA(T* base, u32 index) {
    return (u32)(gabi::ea(base) + index * sizeof(T));
}
}

// HD: private first-half Link02 views; guest address helpers preserve wrapping and zero-page reads.
#include "gc_api/f_op/f_op_actor.h"
u32 player02FirstEventPID(u32 control, u32 processID);
namespace player02first {
struct GroundCheckView {
    u8 beforePosition[0x24];
    cXyz mPos;
};
WWHD_OFFSET(GroundCheckView, mPos, 0x24);
WWHD_SIZE(GroundCheckView, 0x30);
inline u32 groundPositionXAddress(u32 address) { return address + offsetof(GroundCheckView, mPos) + offsetof(cXyz, x); }
inline u32 groundPositionYAddress(u32 address) { return address + offsetof(GroundCheckView, mPos) + offsetof(cXyz, y); }
inline u32 groundPositionZAddress(u32 address) { return address + offsetof(GroundCheckView, mPos) + offsetof(cXyz, z); }
inline s32 attentionType(u32 address) { return gabi::load<s32>(address + 8); }
inline u32 actorID(u32 address) { return gabi::load<u32>(address + 4); }
inline u32 actorStatus(u32 address) { return gabi::load<u32>(address + 0x2E0); }
inline u32 miniGameArrowCount(u32 actor) { return gabi::load<u32>(actor + 0xCF0); }
inline u32 hintCatchAddress(u32 play) { return play + 0x5928; }
inline u32 hintProcessID(u32 play) { return gabi::load<u32>(play + 0x5930); }
inline cXyz* cameraEye(u32 camera) { return gabi::at<cXyz>(camera + 0xDC); }
inline cXyz* cameraCenter(u32 camera) { return gabi::at<cXyz>(camera + 0xE8); }
enum : u32 {
    JA_SE_LK_ITEM_TAKEOUT = 0x2810,
    daPyProc_BOOMERANG_SUBJECT_e = 0x80,
    dRes_INDEX_LKANM_BCK_BOOMWAIT_e = 0x35,
    daPyStts0_BOOMERANG_AIM_e = 0x80000,
    daPyProc_BOOMERANG_MOVE_e = 0x81,
    daPyProc_ROPE_SUBJECT_e = 0x76,
    dRes_INDEX_LKANM_BCK_ROPETHROWWAIT_e = 0xE4,
    daPyStts0_ROPE_AIM_e = 0x20000,
    JA_SE_CAMERA_L_MOVE = 0x81C,
    daPyProc_ROPE_MOVE_e = 0x7D,
    daPyProc_TACT_WAIT_e = 0x9A,
    ANM_WAITTAKT = 0xA8,
    dRes_INDEX_LKANM_BCK_WAITTAKT_e = 0x127,
    daPyStts1_WIND_WAKER_CONDUCT_e = 1,
    JA_SE_TAKT_USE_BEGIN = 0x86F,
    daPyItem_DRINK_BOTTLE_e = 0x105,
    daPyItem_OPEN_BOTTLE_e = 0x106,
    daPyItem_ESA_e = 0x107,
    daPyItem_BOW_e = 0x108,
    daPyItem_PHOTOBOX_e = 0x109,
    JA_SE_ITEM_TARGET_OUT = 0x883,
    daPyProc_BOOTS_EQUIP_e = 0xA1,
    ANM_SETBOOTS = 0xAE,
    daPyProc_GRAB_THROW_e = 0x71,
    ANM_GRABRE = 0x6B,
    ANM_GRABTHROW = 0x6A,
    daPyStts1_UNK40000_e = 0x40000,
    dRes_INDEX_LKANM_BCK_REST_e = 0xD7,
    daPyStts0_BOOMERANG_WAIT_e = 0x400000,
    daPyProc_GRAB_PUT_e = 0x72,
    ANM_GRABUP = 0x66,
    dItemNo_MAGIC_ARMOR_e = 0x2A,
    dItemNo_IRON_BOOTS_e = 0x29,
    fpcNm_BOMB_e = 0x126,
    dItemNo_BOMB_BAG_e = 0x31,
    dActStts_OPEN_e = 0xB,
    dActStts_BLANK_e = 0,
    ANM_HOOKSHOTJMP = 0xA6,
    daPyStts1_UNK10_e = 0x10,
    dActStts_RETURN_e = 7,
    JA_SE_CANCEL_1 = 0x802,
    dRes_INDEX_LKANM_BCK_ARROWRELORD_e = 0xC,
    dRes_INDEX_LKANM_BCK_ARROWRELORDA_e = 0xD,
    JA_SE_LK_DRAW_BOW = 0x2869,
    fpcNm_NPC_SO_e = 0x76,
    fpcNm_ARROW_e = 0x1D8,
    JA_SE_LK_HS_SHOOT = 0x286B,
    JA_SE_LK_SHOOT_ARROW = 0x286A,
    dItemNo_BOW_e = 0x27,
    dRes_INDEX_LKANM_BCK_BOWWAIT_e = 0x36,
    dRes_INDEX_LKANM_BCK_BOWWAITA_e = 0x37,
    dRes_INDEX_LKANM_BCK_BOOMTHROW_e = 0x34,
    daPyProc_ROPE_READY_e = 0x77,
    ANM_ROPECATCH = 0x75,
    ANM_ROPESWINGB = 0x77,
    daPyStts0_UNK800000_e = 0x800000,
    daPyProc_ROPE_THROW_CATCH_e = 0x7E,
    ANM_ROPETHROWCATCH = 0x7B,
    dItemNo_BOOMERANG_e = 0x2D,
    dRes_INDEX_LKANM_BCK_ROPETHROW_e = 0xE2,
    JA_SE_LK_ROPE_LAUNCH = 0x2817,
};
} // namespace player02first

namespace player02 {
// HD: compute each guest field address before conversion, preserving zero-page and wrapping accesses.
inline u32 padReleaseWord() {
    return gabi::load<u32>(gabi::load<u32>(0x101F5088) + offsetof(padRelease_c, mRelease));
}
inline u32 actorParameters(fopAc_ac_c* actor) {
    return gabi::load<u32>(gabi::ea(actor) + offsetof(fopAc_ac_c, mParameters));
}
inline void setActorParameters(fopAc_ac_c* actor, u32 parameters) {
    gabi::store<u32>(gabi::ea(actor) + offsetof(fopAc_ac_c, mParameters), parameters);
}
inline cXyz* actorPosition(fopAc_ac_c* actor) {
    return gabi::at<cXyz>(gabi::ea(actor) + offsetof(fopAc_ac_c, current.pos));
}
inline cXyz* lineCrossPosition(u32 check) {
    return gabi::at<cXyz>(check + offsetof(cBgS_LinChk, mLin) + offsetof(cM3dGLin, mEnd));
}
inline void copyWordX(cXyz* to, const cXyz* from) {
    gabi::store<u32>(gabi::ea(to) + offsetof(cXyz, x), gabi::load<u32>(gabi::ea(from) + offsetof(cXyz, x)));
}
inline void copyWordY(cXyz* to, const cXyz* from) {
    gabi::store<u32>(gabi::ea(to) + offsetof(cXyz, y), gabi::load<u32>(gabi::ea(from) + offsetof(cXyz, y)));
}
inline void copyWordZ(cXyz* to, const cXyz* from) {
    gabi::store<u32>(gabi::ea(to) + offsetof(cXyz, z), gabi::load<u32>(gabi::ea(from) + offsetof(cXyz, z)));
}
} // namespace player02

namespace player02first {
inline f32 positionX(u32 position) { return gabi::load<f32>(position + offsetof(cXyz, x)); }
inline f32 positionY(u32 position) { return gabi::load<f32>(position + offsetof(cXyz, y)); }
inline f32 positionZ(u32 position) { return gabi::load<f32>(position + offsetof(cXyz, z)); }
}
namespace player02tail {
enum : u32 {
    CUT_TYPE_STICK = 0x0B,
    CUT_TYPE_MACHETE = 0x0D,
    CUT_TYPE_CLUB = 0x14,
    CUT_TYPE_DN_SWORD = 0x16,
    CUT_TYPE_SPEAR = 0x18,
    CUT_TYPE_PG_SWORD = 0x1C,
};
// HD: retain the inline byte comparison, including guest-zero and wrapping accesses.
inline BOOL sameStage(u32 stage, u32 name) {
    u8 a, b;
    do {
        a = gabi::load<u8>(stage++);
        b = gabi::load<u8>(name++);
    } while (a == b && a != 0);
    return a == b;
}
struct jumpCutState_c {
    u8 beforeStrongCut[0x69E8];
    be<u8> mStrongJumpCut;
};
WWHD_OFFSET(jumpCutState_c, mStrongJumpCut, 0x69E8);
WWHD_SIZE(jumpCutState_c, 0x69E9);
inline be<u8>& strongJumpCut(daPy_lk_c* player) {
    return *gabi::at<be<u8>>(gabi::ea(player) + offsetof(jumpCutState_c, mStrongJumpCut));
}
}
