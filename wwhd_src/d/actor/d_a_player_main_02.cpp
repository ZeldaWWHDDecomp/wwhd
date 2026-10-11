/**
 * d_a_player_main_02.cpp (WWHD)
 * Player - Link (daPy_lk_c), address range #02 (023E0E50..023EDF6F): move animations, the wait
 * and slide procs, ship-ride helpers, the battle/cut proc inits, item-ready checks (bow,
 * boomerang, hookshot, rope), the do-status and talk logic and the item-equip animations.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_player_main.cpp and its .inc files) to the WWHD layout and code,
 * verified against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * Private boundary declarations retain HD register widths and virtual dispatch.
 */
#include "d/actor/d_a_player_main_02.h"
#include <cmath>

const gc_tu_data gc_tu = { .safeString = 0x10034B24 };
namespace player02_cpp {
static player02::TexAnmTableEntry mTexAnmIndexTable[173] = {
    {player02::dRes_INDEX_LKANM_BTP_TMABAA_e, player02::dRes_INDEX_LKANM_BTK_TMABA_e},
    {player02::dRes_INDEX_LKANM_BTP_TMABAA_e, player02::dRes_INDEX_LKANM_BTK_TEUP_e},
    {player02::dRes_INDEX_LKANM_BTP_TMABAB_e, player02::dRes_INDEX_LKANM_BTK_TMABA_e},
    {player02::dRes_INDEX_LKANM_BTP_TMABAC_e, player02::dRes_INDEX_LKANM_BTK_TMABA_e},
    {player02::dRes_INDEX_LKANM_BTP_TMABACB_e, player02::dRes_INDEX_LKANM_BTK_TMABA_e},
    {player02::dRes_INDEX_LKANM_BTP_TMABACB_e, player02::dRes_INDEX_LKANM_BTK_TEDW_e},
    {player02::dRes_INDEX_LKANM_BTP_TMABACC_e, player02::dRes_INDEX_LKANM_BTK_TMABA_e},
    {player02::dRes_INDEX_LKANM_BTP_TMABAD_e, player02::dRes_INDEX_LKANM_BTK_TMABA_e},
    {player02::dRes_INDEX_LKANM_BTP_TMABAD_e, player02::dRes_INDEX_LKANM_BTK_TEDW_e},
    {player02::dRes_INDEX_LKANM_BTP_TMABAE_e, player02::dRes_INDEX_LKANM_BTK_TMABA_e},
    {player02::dRes_INDEX_LKANM_BTP_TMABAE_e, player02::dRes_INDEX_LKANM_BTK_TEUP_e},
    {player02::dRes_INDEX_LKANM_BTP_TMABAE_e, player02::dRes_INDEX_LKANM_BTK_TEDW_e},
    {player02::dRes_INDEX_LKANM_BTP_TMABAE_e, player02::dRes_INDEX_LKANM_BTK_TEL_e},
    {player02::dRes_INDEX_LKANM_BTP_TMABAE_e, player02::dRes_INDEX_LKANM_BTK_TER_e},
    {player02::dRes_INDEX_LKANM_BTP_TMABAF_e, player02::dRes_INDEX_LKANM_BTK_TMABA_e},
    {player02::dRes_INDEX_LKANM_BTP_TMABAG_e, player02::dRes_INDEX_LKANM_BTK_TMABA_e},
    {player02::dRes_INDEX_LKANM_BTP_TMABAH_e, player02::dRes_INDEX_LKANM_BTK_TMABA_e},
    {player02::dRes_INDEX_LKANM_BTP_TMABAI_e, player02::dRes_INDEX_LKANM_BTK_TMABA_e},
    {player02::dRes_INDEX_LKANM_BTP_TMABAJ_e, player02::dRes_INDEX_LKANM_BTK_TEYORIME_e},
    {player02::dRes_INDEX_LKANM_BTP_TMABAD_e, player02::dRes_INDEX_LKANM_BTK_TWAITPICKUP_e},
    {player02::dRes_INDEX_LKANM_BTP_TMABAC_e, player02::dRes_INDEX_LKANM_BTK_THOOKSHOTJMP_e},
    {player02::dRes_INDEX_LKANM_BTP_TGRABP_e, player02::dRes_INDEX_LKANM_BTK_TGRABP_e},
    {player02::dRes_INDEX_LKANM_BTP_TGRABUP_e, player02::dRes_INDEX_LKANM_BTK_TGRABUP_e},
    {player02::dRes_INDEX_LKANM_BTP_TGRABNG_e, player02::dRes_INDEX_LKANM_BTK_TGRABNG_e},
    {player02::dRes_INDEX_LKANM_BTP_TGRABTHROW_e, player02::dRes_INDEX_LKANM_BTK_TGRABTHROW_e},
    {player02::dRes_INDEX_LKANM_BTP_TGRABRE_e, player02::dRes_INDEX_LKANM_BTK_TECN_e},
    {player02::dRes_INDEX_LKANM_BTP_TWALLWL_e, player02::dRes_INDEX_LKANM_BTK_TER_e},
    {player02::dRes_INDEX_LKANM_BTP_TWALLWR_e, player02::dRes_INDEX_LKANM_BTK_TEL_e},
    {player02::dRes_INDEX_LKANM_BTP_TWALLPL_e, player02::dRes_INDEX_LKANM_BTK_TWALLPL_e},
    {player02::dRes_INDEX_LKANM_BTP_TWALLPR_e, player02::dRes_INDEX_LKANM_BTK_TWALLPR_e},
    {player02::dRes_INDEX_LKANM_BTP_TCUT_e, player02::dRes_INDEX_LKANM_BTK_TECN_e},
    {player02::dRes_INDEX_LKANM_BTP_TCUTEA_e, player02::dRes_INDEX_LKANM_BTK_TCUTEA_e},
    {player02::dRes_INDEX_LKANM_BTP_TCUTEA_e, player02::dRes_INDEX_LKANM_BTK_TECN_e},
    {player02::dRes_INDEX_LKANM_BTP_TEXCA1_e, player02::dRes_INDEX_LKANM_BTK_TEXCA1_e},
    {player02::dRes_INDEX_LKANM_BTP_TEXCB1_e, player02::dRes_INDEX_LKANM_BTK_TEXCB1_e},
    {player02::dRes_INDEX_LKANM_BTP_TJATTACK_e, player02::dRes_INDEX_LKANM_BTK_TJATTACK_e},
    {player02::dRes_INDEX_LKANM_BTP_TJATTACK_e, player02::dRes_INDEX_LKANM_BTK_TEUP_e},
    {player02::dRes_INDEX_LKANM_BTP_TCUTTURNP_e, player02::dRes_INDEX_LKANM_BTK_TECN_e},
    {player02::dRes_INDEX_LKANM_BTP_TCUTTURN_e, player02::dRes_INDEX_LKANM_BTK_TCUTTURN_e},
    {player02::dRes_INDEX_LKANM_BTP_TMJMP_e, player02::dRes_INDEX_LKANM_BTK_TMJMP_e},
    {player02::dRes_INDEX_LKANM_BTP_TMJMPC_e, player02::dRes_INDEX_LKANM_BTK_TMJMPC_e},
    {player02::dRes_INDEX_LKANM_BTP_TMROLLL_e, player02::dRes_INDEX_LKANM_BTK_TMROLLL_e},
    {player02::dRes_INDEX_LKANM_BTP_TMROLLR_e, player02::dRes_INDEX_LKANM_BTK_TMROLLR_e},
    {player02::dRes_INDEX_LKANM_BTP_TMROLLLRC_e, player02::dRes_INDEX_LKANM_BTK_TMROLLLC_e},
    {player02::dRes_INDEX_LKANM_BTP_TMROLLLRC_e, player02::dRes_INDEX_LKANM_BTK_TMROLLRC_e},
    {player02::dRes_INDEX_LKANM_BTP_TMSTEPOVER_e, player02::dRes_INDEX_LKANM_BTK_TMSTEPOVER_e},
    {player02::dRes_INDEX_LKANM_BTP_TMSTEPOVERA_e, player02::dRes_INDEX_LKANM_BTK_TMSTEPOVERA_e},
    {player02::dRes_INDEX_LKANM_BTP_TMSTEPOVERLAND_e, player02::dRes_INDEX_LKANM_BTK_TMSTEPOVERLAND_e},
    {player02::dRes_INDEX_LKANM_BTP_TATNJLR_e, player02::dRes_INDEX_LKANM_BTK_TATNJL_e},
    {player02::dRes_INDEX_LKANM_BTP_TATNJLR_e, player02::dRes_INDEX_LKANM_BTK_TATNJR_e},
    {player02::dRes_INDEX_LKANM_BTP_TJMP_e, player02::dRes_INDEX_LKANM_BTK_TJMP_e},
    {player02::dRes_INDEX_LKANM_BTP_TROLLF_e, player02::dRes_INDEX_LKANM_BTK_TROLLF_e},
    {player02::dRes_INDEX_LKANM_BTP_TROLLB_e, player02::dRes_INDEX_LKANM_BTK_TROLLB_e},
    {player02::dRes_INDEX_LKANM_BTP_TROLLBLAND_e, player02::dRes_INDEX_LKANM_BTK_TECN_e},
    {player02::dRes_INDEX_LKANM_BTP_TROLLFMIS_e, player02::dRes_INDEX_LKANM_BTK_TROLLFMIS_e},
    {player02::dRes_INDEX_LKANM_BTP_TFALL_e, player02::dRes_INDEX_LKANM_BTK_TEDW_e},
    {player02::dRes_INDEX_LKANM_BTP_TLANDDAMA_e, player02::dRes_INDEX_LKANM_BTK_TLANDDAMA_e},
    {player02::dRes_INDEX_LKANM_BTP_TLANDDAMAST_e, player02::dRes_INDEX_LKANM_BTK_TLANDDAMAST_e},
    {player02::dRes_INDEX_LKANM_BTP_TDAM_e, player02::dRes_INDEX_LKANM_BTK_TDAM_e},
    {player02::dRes_INDEX_LKANM_BTP_TDAMF_e, player02::dRes_INDEX_LKANM_BTK_TECN_e},
    {player02::dRes_INDEX_LKANM_BTP_TDAMFFUP_e, player02::dRes_INDEX_LKANM_BTK_TDAMFFUP_e},
    {player02::dRes_INDEX_LKANM_BTP_TDAMFBUP_e, player02::dRes_INDEX_LKANM_BTK_TDAMFBUP_e},
    {player02::dRes_INDEX_LKANM_BTP_TDAMFLRUP_e, player02::dRes_INDEX_LKANM_BTK_TDAMFLUP_e},
    {player02::dRes_INDEX_LKANM_BTP_TDAMFLRUP_e, player02::dRes_INDEX_LKANM_BTK_TDAMFRUP_e},
    {player02::dRes_INDEX_LKANM_BTP_TDAMDASH_e, player02::dRes_INDEX_LKANM_BTK_TECN_e},
    {player02::dRes_INDEX_LKANM_BTP_TGUARD_e, player02::dRes_INDEX_LKANM_BTK_TGUARD_e},
    {player02::dRes_INDEX_LKANM_BTP_TSLIDEF_e, player02::dRes_INDEX_LKANM_BTK_TSLIDEF_e},
    {player02::dRes_INDEX_LKANM_BTP_TSLIDEFLAND_e, player02::dRes_INDEX_LKANM_BTK_TSLIDEFLAND_e},
    {player02::dRes_INDEX_LKANM_BTP_TSLIDEB_e, player02::dRes_INDEX_LKANM_BTK_TEDW_e},
    {player02::dRes_INDEX_LKANM_BTP_TSLIDEBLAND_e, player02::dRes_INDEX_LKANM_BTK_TSLIDEFLAND_e},
    {player02::dRes_INDEX_LKANM_BTP_TSWIMP_e, player02::dRes_INDEX_LKANM_BTK_TSWIMP_e},
    {player02::dRes_INDEX_LKANM_BTP_TSWIMING_e, player02::dRes_INDEX_LKANM_BTK_TSWIMING_e},
    {player02::dRes_INDEX_LKANM_BTP_TBOOMTHROW_e, player02::dRes_INDEX_LKANM_BTK_TBOOMTHROW_e},
    {player02::dRes_INDEX_LKANM_BTP_TBOOMCATCH_e, player02::dRes_INDEX_LKANM_BTK_TBOOMCATCH_e},
    {player02::dRes_INDEX_LKANM_BTP_TWALKPUSH_e, player02::dRes_INDEX_LKANM_BTK_TWALKPUSH_e},
    {player02::dRes_INDEX_LKANM_BTP_TWALKPULL_e, player02::dRes_INDEX_LKANM_BTK_TWALKPULL_e},
    {player02::dRes_INDEX_LKANM_BTP_TVJMPCHA_e, player02::dRes_INDEX_LKANM_BTK_TVJMPCHA_e},
    {player02::dRes_INDEX_LKANM_BTP_TVJMPCL_e, player02::dRes_INDEX_LKANM_BTK_TVJMPCL_e},
    {player02::dRes_INDEX_LKANM_BTP_TROPECLIMB_e, player02::dRes_INDEX_LKANM_BTK_TEUP_e},
    {player02::dRes_INDEX_LKANM_BTP_TROPEDOWN_e, player02::dRes_INDEX_LKANM_BTK_TEDW_e},
    {player02::dRes_INDEX_LKANM_BTP_TLADDERUPST_e, player02::dRes_INDEX_LKANM_BTK_TLADDERUPST_e},
    {player02::dRes_INDEX_LKANM_BTP_TLADDERDWST_e, player02::dRes_INDEX_LKANM_BTK_TLADDERDWST_e},
    {player02::dRes_INDEX_LKANM_BTP_TLADDERUPED_e, player02::dRes_INDEX_LKANM_BTK_TLADDERUPED_e},
    {player02::dRes_INDEX_LKANM_BTP_TLADDERDWED_e, player02::dRes_INDEX_LKANM_BTK_TLADDERDWED_e},
    {player02::dRes_INDEX_LKANM_BTP_THANGING_e, player02::dRes_INDEX_LKANM_BTK_THANGING_e},
    {player02::dRes_INDEX_LKANM_BTP_THANGUP_e, player02::dRes_INDEX_LKANM_BTK_THANGUP_e},
    {player02::dRes_INDEX_LKANM_BTP_TLAVADAM_e, player02::dRes_INDEX_LKANM_BTK_TLAVADAM_e},
    {player02::dRes_INDEX_LKANM_BTP_TCUTRE_e, player02::dRes_INDEX_LKANM_BTK_TCUTRE_e},
    {player02::dRes_INDEX_LKANM_BTP_TCUTTURNB_e, player02::dRes_INDEX_LKANM_BTK_TCUTTURNB_e},
    {player02::dRes_INDEX_LKANM_BTP_TWAITATOB_e, player02::dRes_INDEX_LKANM_BTK_TWAITATOB_e},
    {player02::dRes_INDEX_LKANM_BTP_TCOMEOUT_e, player02::dRes_INDEX_LKANM_BTK_TCOMEOUT_e},
    {player02::dRes_INDEX_LKANM_BTP_THOLDUP_e, player02::dRes_INDEX_LKANM_BTK_TMABA_e},
    {player02::dRes_INDEX_LKANM_BTP_TWALLHOLDUP_e, player02::dRes_INDEX_LKANM_BTK_TWALLHOLDUP_e},
    {player02::dRes_INDEX_LKANM_BTP_TITEMGET_e, player02::dRes_INDEX_LKANM_BTK_TITEMGET_e},
    {player02::dRes_INDEX_LKANM_BTP_TSALTATION_e, player02::dRes_INDEX_LKANM_BTK_TSALTATION_e},
    {player02::dRes_INDEX_LKANM_BTP_TWHO_e, player02::dRes_INDEX_LKANM_BTK_TWHO_e},
    {player02::dRes_INDEX_LKANM_BTP_TPICKUP_e, player02::dRes_INDEX_LKANM_BTK_TPICKUP_e},
    {player02::dRes_INDEX_LKANM_BTP_TBOXOPENLINK_e, player02::dRes_INDEX_LKANM_BTK_TBOXOPENLINK_e},
    {player02::dRes_INDEX_LKANM_BTP_TBOXOPENSHORTLINK_e, player02::dRes_INDEX_LKANM_BTK_TBOXOPENSHORTLINK_e},
    {player02::dRes_INDEX_LKANM_BTP_TDIELONG_e, player02::dRes_INDEX_LKANM_BTK_TDIELONG_e},
    {player02::dRes_INDEX_LKANM_BTP_TSWIMDIE_e, player02::dRes_INDEX_LKANM_BTK_TECN_e},
    {player02::dRes_INDEX_LKANM_BTP_TSURPRISED_e, player02::dRes_INDEX_LKANM_BTK_TSURPRISED_e},
    {player02::dRes_INDEX_LKANM_BTP_TTURNBACK_e, player02::dRes_INDEX_LKANM_BTK_TTURNBACK_e},
    {player02::dRes_INDEX_LKANM_BTP_TLOOKUP_e, player02::dRes_INDEX_LKANM_BTK_TLOOKUP_e},
    {player02::dRes_INDEX_LKANM_BTP_TWAITQ_e, player02::dRes_INDEX_LKANM_BTK_TECN_e},
    {player02::dRes_INDEX_LKANM_BTP_TGLAD_e, player02::dRes_INDEX_LKANM_BTK_TECN_e},
    {player02::dRes_INDEX_LKANM_BTP_TCUTBOKO_e, player02::dRes_INDEX_LKANM_BTK_TCUTBOKO_e},
    {player02::dRes_INDEX_LKANM_BTP_SHIP_JUMP1_e, player02::dRes_INDEX_LKANM_BTK_TECN_e},
    {player02::dRes_INDEX_LKANM_BTP_SHIP_JUMP2_e, player02::dRes_INDEX_LKANM_BTK_TECN_e},
    {player02::dRes_INDEX_LKANM_BTP_TUSEFANA_e, player02::dRes_INDEX_LKANM_BTK_TUSEFANA_e},
    {player02::dRes_INDEX_LKANM_BTP_TUSEFANB_e, player02::dRes_INDEX_LKANM_BTK_TUSEFANB_e},
    {player02::dRes_INDEX_LKANM_BTP_TUSEFANB2_e, player02::dRes_INDEX_LKANM_BTK_TEDW_e},
    {player02::dRes_INDEX_LKANM_BTP_TBOWWAIT_e, player02::dRes_INDEX_LKANM_BTK_TBOWWAIT_e},
    {player02::dRes_INDEX_LKANM_BTP_TVOMITJMP_e, player02::dRes_INDEX_LKANM_BTK_TEDW_e},
    {player02::dRes_INDEX_LKANM_BTP_TREST_e, player02::dRes_INDEX_LKANM_BTK_TREST_e},
    {player02::dRes_INDEX_LKANM_BTP_TTAKE_e, player02::dRes_INDEX_LKANM_BTK_TTAKE_e},
    {player02::dRes_INDEX_LKANM_BTP_TTAKELR_e, player02::dRes_INDEX_LKANM_BTK_TECN_e},
    {player02::dRes_INDEX_LKANM_BTP_TSLIPICE_e, player02::dRes_INDEX_LKANM_BTK_TECN_e},
    {player02::dRes_INDEX_LKANM_BTP_THAMSWINGA_e, player02::dRes_INDEX_LKANM_BTK_THAMSWINGA_e},
    {player02::dRes_INDEX_LKANM_BTP_THAMSWINGBPRE_e, player02::dRes_INDEX_LKANM_BTK_THAMSWINGBPRE_e},
    {player02::dRes_INDEX_LKANM_BTP_THAMSWINGBHIT_e, player02::dRes_INDEX_LKANM_BTK_THAMSWINGBHIT_e},
    {player02::dRes_INDEX_LKANM_BTP_THAMSWINGBEND_e, player02::dRes_INDEX_LKANM_BTK_THAMSWINGBEND_e},
    {player02::dRes_INDEX_LKANM_BTP_TSETBOOTS_e, player02::dRes_INDEX_LKANM_BTK_TECN_e},
    {player02::dRes_INDEX_LKANM_BTP_TDOOROPENALINK_e, player02::dRes_INDEX_LKANM_BTK_TDOOROPENALINK_e},
    {player02::dRes_INDEX_LKANM_BTP_TDOOROPENBLINK_e, player02::dRes_INDEX_LKANM_BTK_TDOOROPENBLINK_e},
    {player02::dRes_INDEX_LKANM_BTP_TSEYYES_e, player02::dRes_INDEX_LKANM_BTK_TSEYYES_e},
    {player02::dRes_INDEX_LKANM_BTP_TPRESENTATION_e, player02::dRes_INDEX_LKANM_BTK_TPRESENTATIONA_e},
    {player02::dRes_INDEX_LKANM_BTP_TWIND_e, player02::dRes_INDEX_LKANM_BTK_TWINDL_e},
    {player02::dRes_INDEX_LKANM_BTP_TWIND_e, player02::dRes_INDEX_LKANM_BTK_TWINDR_e},
    {player02::dRes_INDEX_LKANM_BTP_TPRESENTATION_e, player02::dRes_INDEX_LKANM_BTK_TPRESENTATIONB_e},
    {player02::dRes_INDEX_LKANM_BTP_TBINDRINKPRE_e, player02::dRes_INDEX_LKANM_BTK_TBINDRINKPRE_e},
    {player02::dRes_INDEX_LKANM_BTP_TBINDRINKING_e, player02::dRes_INDEX_LKANM_BTK_TECN_e},
    {player02::dRes_INDEX_LKANM_BTP_TBINDRINKAFTER_e, player02::dRes_INDEX_LKANM_BTK_TBINDRINKAFTER_e},
    {player02::dRes_INDEX_LKANM_BTP_TBINDRINKPRE2_e, player02::dRes_INDEX_LKANM_BTK_TBINDRINKPRE_e},
    {player02::dRes_INDEX_LKANM_BTP_TBINDRINKING2_e, player02::dRes_INDEX_LKANM_BTK_TECN_e},
    {player02::dRes_INDEX_LKANM_BTP_TBINDRINKAFTER2_e, player02::dRes_INDEX_LKANM_BTK_TBINDRINKAFTER2_e},
    {player02::dRes_INDEX_LKANM_BTP_TBINOPENPRE_e, player02::dRes_INDEX_LKANM_BTK_TBINOPENPRE_e},
    {player02::dRes_INDEX_LKANM_BTP_TBINOPENA_e, player02::dRes_INDEX_LKANM_BTK_TBINOPENA_e},
    {player02::dRes_INDEX_LKANM_BTP_TBINOPENB_e, player02::dRes_INDEX_LKANM_BTK_TBINOPENB_e},
    {player02::dRes_INDEX_LKANM_BTP_TBINSWING_e, player02::dRes_INDEX_LKANM_BTK_TBINSWING_e},
    {player02::dRes_INDEX_LKANM_BTP_TBINGET_e, player02::dRes_INDEX_LKANM_BTK_TBINGET_e},
    {player02::dRes_INDEX_LKANM_BTP_TARROWRELORD_e, player02::dRes_INDEX_LKANM_BTK_TARROWRELORD_e},
    {player02::dRes_INDEX_LKANM_BTP_TSURPRISEDB_e, player02::dRes_INDEX_LKANM_BTK_TSURPRISEDB_e},
    {player02::dRes_INDEX_LKANM_BTP_TFAN_e, player02::dRes_INDEX_LKANM_BTK_TFAN_e},
    {player02::dRes_INDEX_LKANM_BTP_TNENRIKI_e, player02::dRes_INDEX_LKANM_BTK_TECN_e},
    {player02::dRes_INDEX_LKANM_BTP_TMABAB_e, player02::dRes_INDEX_LKANM_BTK_TEDL_e},
    {player02::dRes_INDEX_LKANM_BTP_TMABAH_e, player02::dRes_INDEX_LKANM_BTK_TABEKOBE_e},
    {player02::dRes_INDEX_LKANM_BTP_TSETHYOINOMI_e, player02::dRes_INDEX_LKANM_BTK_TSETHYOINOMI_e},
    {player02::dRes_INDEX_LKANM_BTP_LINK_FREEZ_e, player02::dRes_INDEX_LKANM_BTK_TECN_e},
    {player02::dRes_INDEX_LKANM_BTP_LINK_MOGAKI_e, player02::dRes_INDEX_LKANM_BTK_TECN_e},
    {player02::dRes_INDEX_LKANM_BTP_TDAMBIRI_e, player02::dRes_INDEX_LKANM_BTK_TDAMBIRI_e},
    {player02::dRes_INDEX_LKANM_BTP_SALVLR_e, player02::dRes_INDEX_LKANM_BTK_SALVWAIT_e},
    {player02::dRes_INDEX_LKANM_BTP_SALVWAIT_e, player02::dRes_INDEX_LKANM_BTK_SALVWAIT_e},
    {player02::dRes_INDEX_LKANM_BTP_SALVBAD_e, player02::dRes_INDEX_LKANM_BTK_SALVWAIT_e},
    {player02::dRes_INDEX_LKANM_BTP_SALVGOOD_e, player02::dRes_INDEX_LKANM_BTK_SALVWAIT_e},
    {player02::dRes_INDEX_LKANM_BTP_SEARESET_e, player02::dRes_INDEX_LKANM_BTK_SEARESET_e},
    {player02::dRes_INDEX_LKANM_BTP_TWARPIN_e, player02::dRes_INDEX_LKANM_BTK_TWARPIN_e},
    {player02::dRes_INDEX_LKANM_BTP_TWARPOUT_e, player02::dRes_INDEX_LKANM_BTK_TWARPOUT_e},
    {player02::dRes_INDEX_LKANM_BTP_TPOWUPWAIT_e, player02::dRes_INDEX_LKANM_BTK_TPOWUPWAIT_e},
    {player02::dRes_INDEX_LKANM_BTP_TPOWUP_e, player02::dRes_INDEX_LKANM_BTK_TPOWUP_e},
    {player02::dRes_INDEX_LKANM_BTP_TCUTKESA_e, player02::dRes_INDEX_LKANM_BTK_TCUTKESA_e},
    {player02::dRes_INDEX_LKANM_BTP_TWARPOUTFIRST_e, player02::dRes_INDEX_LKANM_BTK_TWARPOUTFIRST_e},
    {player02::dRes_INDEX_LKANM_BTP_TDASHKAZE_e, player02::dRes_INDEX_LKANM_BTK_TDASHKAZE_e},
    {player02::dRes_INDEX_LKANM_BTP_TIYAYA_e, player02::dRes_INDEX_LKANM_BTK_TECN_e},
    {player02::dRes_INDEX_LKANM_BTP_TFREEA_e, player02::dRes_INDEX_LKANM_BTK_TFREEA_e},
    {player02::dRes_INDEX_LKANM_BTP_TFREEB_e, player02::dRes_INDEX_LKANM_BTK_TFREEB_e},
    {player02::dRes_INDEX_LKANM_BTP_TFREED_e, player02::dRes_INDEX_LKANM_BTK_TFREED_e},
    {player02::dRes_INDEX_LKANM_BTP_TTAKTKAZE_e, player02::dRes_INDEX_LKANM_BTK_TECN_e},
    {player02::dRes_INDEX_LKANM_BTP_TTAKTSIPPU_e, player02::dRes_INDEX_LKANM_BTK_TECN_e},
    {player02::dRes_INDEX_LKANM_BTP_TTAKTCHUYA_e, player02::dRes_INDEX_LKANM_BTK_TECN_e},
    {player02::dRes_INDEX_LKANM_BTP_TTAKTFUJIN_e, player02::dRes_INDEX_LKANM_BTK_TECN_e},
    {player02::dRes_INDEX_LKANM_BTP_TTAKTAYATSURI_e, player02::dRes_INDEX_LKANM_BTK_TECN_e},
    {player02::dRes_INDEX_LKANM_BTP_TTAKTCHISIN_e, player02::dRes_INDEX_LKANM_BTK_TECN_e},
};

static cXyz l_ship_offset = {0.0f, 15.0f, -35.0f};
static cXyz l_ship_offset2 = {0.0f, 15.0f, -66.8f};
static cXyz l_ship_offset3 = {0.0f, 15.0f, -96.0f};
static cXyz l_ship_redge = {-57.0f, 35.0f, -35.0f};
static cXyz l_ship_ledge = {57.0f, 35.0f, -35.0f};
static u16 m_damageType;
static u8 m_maba_flg;
static u8 m_maba_timer;
static u32 eff_scale_guard;
static u32 eff_dscale_guard;
static u32 eff_pscale_guard;
static cXyz eff_scale;
static cXyz eff_dscale;
static cXyz eff_pscale;
}

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void lk_onPlayerStatus0(u32 flag) { dComIfGp_setPlayerStatus0(0, flag); }
static inline void lk_offPlayerStatus0(u32 flag) { dComIfGp_clearPlayerStatus0(0, flag); }
static inline u32 lk_checkPlayerStatus0(u32 flag) { return dComIfGp_checkPlayerStatus0(0, flag); }
static inline void lk_onPlayerStatus1(u32 flag) { dComIfGp_setPlayerStatus1(0, flag); }
static inline void lk_offPlayerStatus1(u32 flag) { dComIfGp_clearPlayerStatus1(0, flag); }
static inline u32 lk_checkPlayerStatus1(u32 flag) { return dComIfGp_checkPlayerStatus1(0, flag); }
static inline u32 lk_getShipActor() { return gabi::ea(dComIfGp_getShipActor()); }
static inline u32 lk_getPlayerActorClass() { return gabi::ea(dComIfGp_getPlayer(0)); }
static inline u8 lk_getDoStatus() { return dComIfGp_getDoStatus(); }
/* checkAttentionLock(): dAttention_c::LockonTruth(mpAttention) (024EDFCC) || the attention flag 0x20000000 (+0x20) */
static inline BOOL lk_checkAttentionLock_l(u32 attn) {
    return player02AttentionLock(attn) || ((u32)gabi::at<player02::attention_c>(attn)->mFlags & 0x20000000);
}
#define lk_checkAttentionLock() lk_checkAttentionLock_l((u32)mpAttention)
/* play + 0x5BB5 / 0x5BB7: dComIfGp_set/getRStatus, dComIfGp_setDoStatus */
static inline u8 lk_getRStatus() { return dComIfGp_getRStatus(); }
static inline void lk_setRStatus(u8 s) { dComIfGp_setRStatus(s); }
static inline void lk_setDoStatus(u8 s) { dComIfGp_setDoStatus(s); }
/* 025E1988 seStartSystem(id) */
static inline void lk_seStartSystem(u32 id) { player02SystemSound(id); }
/* dBgS (play + 0x12A0) */
static inline BOOL dBgS_ChkPolySafe_l(u32 poly) { return player02PolySafe(dComIfG_Bgsp(), poly); }
static inline u32 dBgS_GetTriPla_l(u32 poly) {
    cBgS_PolyInfo* info = gabi::at<cBgS_PolyInfo>(poly);
    return gabi::ea(dComIfG_Bgsp()->GetTriPla(info->mBgIndex, info->mPolyIndex));
}
static inline s32 dBgS_GetSpecialCode_l(u32 poly) { return player02SpecialCode(dComIfG_Bgsp(), poly); }
static inline s32 dBgS_GetWallCode_l(u32 poly) { return player02WallCode(dComIfG_Bgsp(), poly); }
static inline s32 dBgS_GetGroundCode_l(u32 poly) { return player02GroundCode(dComIfG_Bgsp(), poly); }
/* cBgS::LineCross (play + 0x12A0) */
static inline BOOL lk_LineCross(u32 chk) { return dComIfG_Bgsp()->LineCross(gabi::at<dBgS_LinChk>(chk)); }
/* 0200ECD4 cLib_addCalc(f32* value, f32 target, f32 scale, f32 maxStep, f32 minStep) */
static inline f32 cLib_addCalc_l(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return player02AddCalc(v, target, scale, maxStep, minStep);
}
/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty; block at +0x2C, flags +4, matrices +0x10) */
static inline u32 lk_getAnmMtx(J3DModel* m, s32 jnt) {
    return gabi::ea(m->getAnmMtx(jnt));
}
/* daPy_pbCalc (J3DMtxCalc blend, HD): setRatio(idx, r) clears the ratio table of every animation slot.
 * 027F3F94 (matcher: __nw) resolves a relative offset (+0xC header), 027E0174 counts the slots */
static inline void lk_pbCalc_setRatio(u32 pb, s32, f32 r) {
    player02::pbCalc_c* calc = gabi::at<player02::pbCalc_c>(pb);
    u32 resource = player02RelativeData(calc->mpResource->mpRelativeHeader);
    s32 count = player02AnimationCount(resource, 0);
    for (s32 i = 0; i < count; i++) {
        player02::indexed(calc->mpRatioTable->mpRatio.get(), (u32)i) = r;
    }
}
/* fsel: a >= 0 ? b : c (NaN -> c) */
static inline f32 fsel_l(f32 a, f32 b, f32 c) { return a >= 0.0f ? b : c; }
/* a float copy through an FPR that the recompiled original keeps bit-exact (no SNaN quieting) */
static inline void fcpy_l(u32 dst, u32 src) { gmem_stf32(dst, gmem_ld32(src)); }

/* daPy_lk_c members in the opaque blocks of the shared header (offsets measured in this range) */
#define LK_FIELD(T, off) (*gabi::at<be<T>>(gabi::ea(this) + (off)))
#define mAcchGndPoly gabi::ea(static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd)) /* mAcch.m_gnd (cBgS_PolyInfo, dBgS_Acch + 0xE8) */
#define mBodyAngleY mBodyAngle.y
#define LK_underIdx() ((u16)player02::heaps(m_anm_heap_under)[0].mIdx) /* m_anm_heap_under[UNDER_MOVE0_e].mIdx */
#define LK_upperIdx() ((u16)player02::heaps(m_anm_heap_upper)[2].mIdx) /* m_anm_heap_upper[UPPER_MOVE2_e].mIdx */
#define LK_checkGrabAnime() (LK_upperIdx() == 0x95 || LK_upperIdx() == 0x96)
#define LK_checkNoUpperAnime() (LK_upperIdx() == 0xFFFF)
#define LK_demoMode() mDemo.getDemoMode() /* mDemo.getDemoMode() (GameCube 0x314) */
/* virtual daPy_lk_c::voiceStart(u32) (vtable slot 0xE4) */
#define LK_voiceStart(n) player02::voiceStart(this, (u32)(n))
#define LK_doTrigger() (mItemTrigger & 1)
/* checkNormalSwordEquip(): dComIfGs_getSelectEquip(0) == dItem_SWORD_e || dComIfGp_getMiniGameType() == 2 */
#define LK_checkNormalSwordEquip() \
    (dComIfGs_getSelectEquip(0) == 0x38 || dComIfGp_getMiniGameType() == 2)
/* mSwordAnim (mDoExt_bckAnm at 0x4444).changeBckOnly(bck) */
#define LK_swordAnim_changeBckOnly(bck) player02SwordBck(gabi::ea(&player02::equipment(this)->mSwordAnim), (u32)(bck))
/* v = cXyz::Zero (0x101FFBA8, copied as three words) */
#define LK_cXyzZero_copy(v) player02::copyZero(v)

/* mAtCyl (dCcD_Cyl at 0x78BC): SetAtAtp (u8 at +0x14) and its cM3dGCyl::SetR (0x020184DC, cylinder at +0x118) */
#define LK_mAtCyl_SetAtAtp(v) mAtCyl.SetAtAtp((u8)(v))
#define LK_mAtCyl_SetR(r) mAtCyl.SetR((f32)(r))
#define mHD827C player02::itemState(this)->mBoomerangAim /* HD-only (tail): cleared when a boomerang aim proc starts */
/* checkSwordEquip(): dComIfGs_getSelectEquip(0) != dItem_NONE_e || dComIfGp_getMiniGameType() == 2 */
#define LK_checkSwordEquip() \
    (dComIfGs_getSelectEquip(0) != 0xFF || dComIfGp_getMiniGameType() == 2)
#define mHD8260 player02::itemState(this)->mItemTriggerLock /* HD-only (tail): while non-zero the X/Y/Z item triggers are ignored */
#define mHDAngleY player02::itemState(this)->mAimAngleY /* HD-only (tail): an angle the HD procs copy to shape_angle.y */

/* ---- daPy_lk_c functions of other ranges (by address) ---- */
enum : u32 {
    LK_commonProcInit = 0x023DFDD8,
    LK_setSingleMoveAnime = 0x023E0A04,
    LK_deleteEquipItem = 0x023DC7AC,
    LK_checkShipNotNormalMode = 0x023D69CC,
    LK_endDamageEmitter = 0x023DCA08,
    LK_mtxFollow_makeEmitterColor = 0x023D460C,
    LK_setBlendMoveAnime = 0x023E14A0,
    LK_setShipRidePos = 0x023E287C,
    LK_procGrabWait_init = 0x023E2484,
    LK_procWait_init = 0x023E2FF4,
    LK_checkGrabSpecialHeavyState = 0x023DBBE8,
    LK_setActAnimeUpper = 0x023DE7E8,
    LK_checkRestHPAnime = 0x023DD12C,
    LK_setTextureAnime = 0x023DD768,
    LK_getItemAnimeResource = 0x023DDEEC, /* unnamed by the matcher */
    LK_getDirectionFromAngle = 0x023E3358,
    LK_mtxFollow_makeEmitter = 0x023D457C,
    LK_setItemHeap = 0x023DDF64,
    LK_initModel = 0x023D4BB8,
    LK_seStartOnlyReverb = 0x023DC0F4,
    LK_loadTextureAnimeResource = 0x023DC38C, /* unnamed by the matcher */
    LK_setTextureAnimeResource = 0x023DC110,  /* unnamed by the matcher */
    LK_loadTextureScrollResource = 0x023DC4DC, /* unnamed by the matcher */
    LK_setTextureScrollResource = 0x023DC43C,
    LK_checkBowAnime = 0x023D6A18,
    LK_setMoveAnime = 0x023E0E50,
    LK_resetFootEffect = 0x023DF9C0,
    LK_setHandModel = 0x023E063C,
    LK_checkHeavyStateOn = 0x023DBC24,
    LK_itemButton = 0x023E8C74,
    LK_checkBoomerangAnime = 0x023D69F8,
    LK_dBgS_LinChk_Set = 0x024F1AFC,
    LK_fopAcM_getTalkEventPartner = 0x025D7C6C,
    LK_setTactModel = 0x023DEF04,
    LK_checkBowItem = 0x023D8600,
    LK_checkPhotoBoxItem = 0x023D85E4,
    LK_checkBottleItem = 0x023D85CC,
    LK_checkEquipAnime = 0x023D794C,
    LK_setBottleModel = 0x023DF0FC,
    LK_getAnmData = 0x023E048C,
    LK_getUnderUpperAnime = 0x023E0530,
    LK_setFrameCtrl = 0x023DE788,
    LK_setSeAnime = 0x023E07E4,
    LK_seStartMapInfo = 0x023E0470,
    LK_checkGrabBarrelSearch = 0x023DCF08,
    LK_getDirectionFromShapeAngle = 0x023E4E18,
    LK_freeGrabItem = 0x023DCF8C,
    LK_resetActAnimeUpper = 0x023DC6A4,
    LK_actorKeep_setData = 0x023DE638,
    LK_daArrow_changeArrowMp = 0x02055774, /* unnamed by the matcher: dComIfGs_getMagic() != 0 */
    LK_SafeString_vtbl = 0x10034B24,      /* this TU's sead::SafeString vtable */
};

/* 023E2358 */
BOOL daPy_lk_c::procScope_init(int param_1) {
    if (noResetFlg1() & 0x200) { // HD: no scope while this flag is set
        return FALSE;
    }
    commonProcInit(player02::daPyProc_SCOPE_e);
    mNormalSpeed = 0.0f;
    setBlendMoveAnime(2.4f /* HD: m_HIO->mBasic.m.field_0xC folded */);
    current.angle.y = shape_angle.y;
    lk_seStartSystem(0x822 /* JA_SE_ITM_SUBMENU_IN_1 */);
    mNoResetFlg0 = mNoResetFlg0 & ~0x80000u; /* offNoResetFlg0(daPyFlg0_SCOPE_CANCEL) */
    if (param_1 == 0x20) {
        lk_onPlayerStatus0(player02::daPyStts0_TELESCOPE_LOOK_e); /* daPyStts0_TELESCOPE_LOOK_e */
        return TRUE;
    }
    lk_onPlayerStatus1(player02::daPyStts1_PICTO_BOX_AIM_e); /* daPyStts1_PICTO_BOX_AIM_e */
    lk_offPlayerStatus1(0x100000); /* HD */
    setSingleMoveAnime(player02::ANM_WAITS, 1.1f, 0.0f, -1, 3.0f); /* HD */
    return TRUE;
}

/* 023E2648 */
BOOL daPy_lk_c::procBoomerangCatch_init() {
    if (mCurProc == player02::daPyProc_BOOMERANG_CATCH_e) {
        return FALSE;
    }
    commonProcInit(0x82);
    setSingleMoveAnime(player02::ANM_BOOMCATCH, 1.0f, 1.0f, 0xB, 0.0f); // HD: HIO folded
    mNoResetFlg0 = mNoResetFlg0 & ~0x20u; /* offNoResetFlg0(daPyFlg0_UNK20) */
    shape_angle.y = mHDAngleY; /* HD */
    return TRUE;
}

/* 023E26EC */
int daPy_lk_c::checkShipRideUseItem(int param_1) {
    if (lk_checkPlayerStatus0(player02::daPyStts0_SHIP_RIDE_e) && lk_getShipActor() != 0) {
        if (param_1 != 0) {
            deleteEquipItem(0);
        }
        if (checkShipNotNormalMode()) {
            return 2;
        }
        return 1;
    }
    return 0;
}

/* 023E278C */
void daPy_lk_c::setOldRootQuaternion(s16 param_1, s16 param_2, s16 param_3) {
    gabi::Local<Quaternion> afStack_28;
    gabi::Local<Quaternion> local_38;
    player02::oldFrame_c* fdata = gabi::at<player02::oldFrame_c>(m_old_fdata);
    if (fdata->mOldFrameFlg == 0) { /* m_old_fdata->getOldFrameFlg() */
        return;
    }
    u32 q = gabi::ea(fdata->mpOldFrameQuaternion.get()); /* getOldFrameQuaternion(0) */
    if (param_1 != 0 || param_2 != 0) {
        player02EulerToQuat((s32)param_1, (s32)param_2, 0, afStack_28.a);
        for (int i = 0; i < 0x10; i += 4) fcpy_l(local_38.a + i, q + i);
        player02QuatConcat(afStack_28.a, local_38.a, gabi::ea(gabi::at<player02::oldFrame_c>(m_old_fdata)->mpOldFrameQuaternion.get()));
    }
    if (param_3 != 0) {
        player02EulerToQuat(0, 0, (s32)param_3, afStack_28.a);
        for (int i = 0; i < 0x10; i += 4) fcpy_l(local_38.a + i, q + i);
        player02QuatConcat(afStack_28.a, local_38.a, gabi::ea(gabi::at<player02::oldFrame_c>(m_old_fdata)->mpOldFrameQuaternion.get()));
    }
}

/* 023E2DC4 */
void daPy_lk_c::setShipRidePosUseItem() {
    if (lk_checkPlayerStatus0(player02::daPyStts0_SHIP_RIDE_e)) {
        setShipRidePos(1);
        shape_angle.y = (s16)(shape_angle.y - 0x4000);
        current.angle.y = shape_angle.y;
    }
}

/* 023E2E18 */
void daPy_lk_c::initShipRideUseItem(int param_1, int param_2) {
    if (param_1 == 0) {
        return;
    }
    gabi::at<player02::ship_c>(lk_getShipActor())->mPaddleMode = 2; /* daShip_c::setPaddleMove() */
    gravity = 0.0f;
    speed.y = 0.0f;
    if (param_1 == 1) {
        setOldRootQuaternion(0, 0x4000, 0);
    }
    lk_onPlayerStatus0(player02::daPyStts0_SHIP_RIDE_e);
    if (param_2 == 2) {
        setShipRidePosUseItem();
    } else if (param_2 == 1) {
        setShipRidePos(1);
    } else {
        setShipRidePos(0);
    }
    mModeFlg = mModeFlg | 0x2000; /* onModeFlg(ModeFlg_IN_SHIP) */
}

/* 023E2EFC */
BOOL daPy_lk_c::procControllWait_init() {
    if (mCurProc == player02::daPyProc_CONTROLL_WAIT_e) {
        return FALSE;
    }
    int iVar3 = checkShipRideUseItem(1);
    commonProcInit(3);
    mNormalSpeed = 0.0f;
    player02_cpp::m_maba_flg = 0; /* daPy_matAnm_c::offMabaFlg() */
    player02_cpp::m_maba_timer = 1;
    setSingleMoveAnime(player02::ANM_NENRIKI, 1.0f, 0.0f, -1, 2.4f);
    current.angle.y = shape_angle.y;
    deleteEquipItem(0);
    initShipRideUseItem(iVar3, 2);
    return TRUE;
}

/* 023E32AC */
BOOL daPy_lk_c::changeWaitProc() {
    u16 upper = player02::heaps(m_anm_heap_upper)[2].mIdx; /* m_anm_heap_upper[UPPER_MOVE2_e].mIdx */
    if (upper == 0x95 || upper == 0x96) { /* checkGrabAnime() */
        return procGrabWait_init();
    } else if (upper == player02::dRes_INDEX_LKANM_BCK_BOOMCATCH_e) {
        return procBoomerangCatch_init();
    } else if (lk_getPlayerActorClass() != gabi::ea(this)) {
        return procControllWait_init();
    } else {
        return procWait_init();
    }
}

/* 023E47C4 */
static void daPy_mtxPosFollowEcallBack_makeEmitterColor(u32 cb, u32 id, u32 mtx, u32 pos, u32 angle, u32 prm, u32 env) {
    gabi::at<player02::mtxPosFollowCallback_c>(cb)->mpAngle = angle;
    player02MakeEmitterColor( cb, id, mtx, pos, prm, env);
}

/* 023E4A58 */
u32 daPy_lk_c::getSlidePolygon() {
    if (!(mNoResetFlg0 & 0xA0000000u) && (mAcch.m_flags & dBgS_Acch::GROUND_HIT) && dBgS_ChkPolySafe_l(mAcchGndPoly)) {
        u32 pla = dBgS_GetTriPla_l(mAcchGndPoly);
        s32 special = dBgS_GetSpecialCode_l(mAcchGndPoly);
        if (pla != 0 && dBgS_GetGroundCode_l(mAcchGndPoly) != 8 && special == 1) {
            return pla;
        }
    }
    return 0;
}

/* 023E4B38 */
BOOL daPy_lk_c::procSlideFront_init(s16 param_1) {
    if (mCurProc == player02::daPyProc_SLIDE_FRONT_e || mCurProc == player02::daPyProc_SLIDE_BACK_e) {
        return FALSE;
    }
    commonProcInit(0x1A);
    m3526 = 8;
    current.angle.y = param_1;
    setSingleMoveAnime(player02::ANM_SLIDEF, 0.3f, 2.0f, 9, 5.0f); // HD: HIO folded
    mMaxNormalSpeed = 30.0f;
    mFootEffectPosType = 4;
    lk_onPlayerStatus1(player02::daPyStts1_UNK100_e);
    return TRUE;
}

/* 023E4C04 */
BOOL daPy_lk_c::procSlideBack_init(s16 param_1) {
    if (mCurProc == player02::daPyProc_SLIDE_BACK_e || mCurProc == player02::daPyProc_SLIDE_FRONT_e) {
        return FALSE;
    }
    commonProcInit(0x1B);
    m3526 = 8;
    current.angle.y = param_1;
    setSingleMoveAnime(player02::ANM_SLIDEB, 0.3f, 0.0f, 9, 5.0f); // HD: HIO folded
    mMaxNormalSpeed = 30.0f;
    mFootEffectPosType = 3;
    lk_onPlayerStatus1(player02::daPyStts1_UNK200_e);
    return TRUE;
}

/* 023E4CD0 */
int daPy_lk_c::changeSlideProc() {
    u32 pla = getSlidePolygon();
    if (pla == 0) {
        m3526 = 8;
        return FALSE;
    }
    s16 sVar4 = cM_atan2s((f32)player02::plane(pla)->mNormal.x, (f32)player02::plane(pla)->mNormal.z);
    if (m34C3 != 0 && !lk_checkPlayerStatus0(0x2000)) { // HD: not while daPyStts0_SUBJECT_e
        cLib_addCalc_l(&mNormalSpeed, 0.0f, 0.4f, 5.0f, 1.0f);
        m3526 = (s16)(m3526 - 1);
        if (mNormalSpeed > 5.0f || m3526 > 0) {
            return FALSE;
        }
    }
    if (cLib_distanceAngleS(sVar4, shape_angle.y) < 0x3800 /* HD: HIO folded */) {
        return procSlideFront_init(sVar4);
    }
    return procSlideBack_init(sVar4);
}

/* 023E4E2C */
void daPy_lk_c::endFlameDamageEmitter() {
    if ((u16)player02_cpp::m_damageType != 0) { /* !daPy_dmEcallBack_c::checkFlame() (HD: a global) */
        return;
    }
    if (current.pos.y < mWaterY - 10.0f) {
        gabi::Local<cXyz> pos;
        for (int i = 0; i < 4; i++) {
            u32 mtx = gabi::ea(player02::indexed(gabi::at<daPy_mtxFollowEcallBack_c>(gabi::ea(mDmEcallBack)), i).mpMatrix.get()); /* mDmEcallBack[i].getMtx() */
            fcpy_l(pos.a + 0, mtx + 0xC); /* mDoMtx_multVecZero */
            fcpy_l(pos.a + 8, mtx + 0x2C);
            fcpy_l(pos.a + 4, gabi::ea(&mWaterY)); /* pos.y = mWaterY */
            dComIfGp_particle_set(0x35A /* ID_AK_JN_EVAPORATION00 */, pos);
        }
    }
    endDamageEmitter();
}

/* 023E2484 */
BOOL daPy_lk_c::procGrabWait_init() {
    if (mCurProc == player02::daPyProc_GRAB_WAIT_e) {
        return FALSE;
    }
    commonProcInit(0x73);
    if (checkGrabSpecialHeavyState()) {
        setSingleMoveAnime(player02::ANM_GRABWAITB, 1.2f, 0.0f, -1, 3.0f); // HD: HIO folded
        if (!LK_checkGrabAnime()) {
            setActAnimeUpper(player02::dRes_INDEX_LKANM_BCK_GRABWAITB_e, player02::UPPER_MOVE2_e, 0.0f, 0.0f, -1, -1.0f);
        }
    } else {
        setSingleMoveAnime(player02::ANM_GRABWAIT, 1.0f, 0.0f, -1, 3.0f);
        if (!LK_checkGrabAnime()) {
            setActAnimeUpper(player02::dRes_INDEX_LKANM_BCK_GRABWAIT_e, 2, 0.0f, 0.0f, -1, -1.0f);
        }
    }
    lk_pbCalc_setRatio(m_pbCalc[1] /* PART_UPPER_e */, 2, 0.0f);
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    m35A0 = 0.0f;
    mProcVar6 = !(m35D8 > -29.0f); /* m35D8 <= -29.0f (NaN: 1) */
    return TRUE;
}

/* 023E287C */
void daPy_lk_c::setShipRidePos(int param_0) {
    u32 ship = lk_getShipActor();
    if (ship == 0) {
        return;
    }
    if (gabi::at<player02::ship_c>(ship)->mFlags & 0x4000) { /* ship->checkJumpOkFlg() */
        dComIfGp_setRStatus(0x12); /* dComIfGp_setRStatus(dActStts_JUMP_e) */
    }
    fopAc_ac_c* sh = gabi::at<fopAc_ac_c>(ship);
    if (param_0 != 0) {
        shape_angle.y = sh->shape_angle.y;
        current.angle.y = shape_angle.y;
    }
    if (mCurProc == 0xD3) {
        m353C = sh->shape_angle.x;
        m353E = sh->shape_angle.z;
        m3540 = 0;
        m3542 = 0;
    } else {
        m353C = (s16)(m353C + m3540);
        s16 v = (s16)gabi::ftoi(gabi::fmadds((f32)(s16)(sh->shape_angle.x - m353C), 0.75f, (f32)m3540));
        if (v > 0x400) { // HD: clamped to +-0x400
            v = 0x400;
        } else if (v < -0x400) {
            v = -0x400;
        }
        m3540 = v;
        cLib_addCalcAngleS(&m3540, 0, 5, 0x200, 0x100);
        m353E = (s16)(m353E + m3542);
        s16 w = (s16)(m3542 + ((s16)(sh->shape_angle.z - m353E) >> 1));
        if (w > 0x400) {
            w = 0x400;
        } else if (w < -0x400) {
            w = -0x400;
        }
        m3542 = w;
        cLib_addCalcAngleS(&m3542, 0, 5, 0x100, 0x30);
    }
    u32 off;
    if (mCurProc == 0xD7) {
        off = gabi::ea(&player02_cpp::l_ship_offset3); /* l_ship_offset3 */
    } else if (mCurProc == 0xAE && mProcVar4 != 0) {
        off = gabi::ea(&player02_cpp::l_ship_offset2); /* l_ship_offset2 */
    } else {
        off = gabi::ea(&player02_cpp::l_ship_offset); /* l_ship_offset */
    }
    u32 model = gabi::ea(gabi::at<player02::ship_c>(ship)->mpBodyMorf->getModel()); /* ship->getBodyMtx() */
    PSMTXMultVec(gabi::at<J3DModel>(model)->getBaseTRMtx(), gabi::at<cXyz>(off), &current.pos);
    if (checkShipNotNormalMode()) {
        mBodyAngleY = 0;
        return;
    }
    gabi::Local<be<f32>> sp08;
    gabi::Local<cXyz> local_38;
    player02AllWind(&current.pos, local_38.a, sp08.a);
    f32 fVar2 = gabi::fmadds(*sp08, 30.0f, sh->speedF) / 30.0f;
    if (fVar2 > 1.0f) {
        fVar2 = 1.0f;
    }
    mFrameCtrlUpper[2].setRate(gabi::fmadds(0.8f, fVar2, 0.2f)); // HD: HIO folded
    if (gabi::at<player02::ship_c>(ship)->mFlags & 0x40) { /* ship->getJumpFlg() */
        if (gabi::at<player02::ship_c>(ship)->mJumpRate > 0.6f) { /* ship->getJumpRate() */
            LK_voiceStart(6);
        } else if (gabi::at<player02::ship_c>(ship)->mJumpRate > 0.2f) {
            LK_voiceStart(7);
        }
        setSingleMoveAnime(player02::ANM_SHIP_JUMP1, 1.0f, 0.0f, -1, 20.0f);
    }
    if (gabi::at<player02::ship_c>(ship)->mFlags & 0x80) { /* ship->getLandFlg() */
        setSingleMoveAnime(player02::ANM_SHIP_JUMP2, 1.0f, 3.0f, -1, 3.0f);
    }
    if (LK_underIdx() == player02::dRes_INDEX_LKANM_BCK_SHIP_JUMP2_e) {
        if (mFrameCtrlUnder[0].getRate() < 0.01f) {
            if (gabi::at<player02::ship_c>(ship)->mFlags & 1) { /* ship->getFlyFlg() */
                setSingleMoveAnime(player02::ANM_SHIP_JUMP1, 1.0f, 0.0f, -1, 20.0f);
            } else {
                setSingleMoveAnime(player02::ANM_VOYAGE1, 1.0f, 0.0f, -1, 5.0f);
            }
        } else {
            mFrameCtrlUpper[2].setRate(0.0f);
        }
    }
    s16 sVar4;
    if (LK_underIdx() == player02::dRes_INDEX_LKANM_BCK_VOYAGE1_e) {
        sVar4 = (s16)gabi::ftoi(gabi::at<player02::ship_c>(ship)->mTillerAngleRate * -5325.0f); /* -HIO * ship->getTillerAngleRate() */
    } else {
        sVar4 = 0;
    }
    cLib_addCalcAngleS(&mBodyAngleY, sVar4, 4, 0x400, 0x80);
}

/* 023E2FF4 */
BOOL daPy_lk_c::procWait_init() {
    if (LK_demoMode() == 0x2A /* daPy_demo_c::DEMO_KM_WAIT_e */ && player02::heaps(m_tex_scroll_heap)->field_0x6 == 0xFFFF) {
        /* virtual changeTextureAnime(dRes_INDEX_LKANM_BTP_TMABACC_e, dRes_INDEX_LKANM_BTK_TEUR_e, -1) */
        player02::changeTextureAnime(this, 0x231, 0x188, -1);
        LK_voiceStart(38);
        mProcVar0 = 20;
    }
    if (mCurProc == player02::daPyProc_WAIT_e) {
        return FALSE;
    }
    lk_offPlayerStatus1(0x100000); /* HD */
    if (!dComIfGp_event_runCheck() /* !dComIfGp_event_runCheck() */ && mCurProc == player02::daPyProc_FREE_WAIT_e &&
        mFrameCtrlUnder[0].getRate() > 0.01f && LK_checkNoUpperAnime()) {
        return FALSE;
    }
    int iVar3 = player02::checkPlayerGuard(this); /* virtual checkPlayerGuard() */
    commonProcInit(4);
    mNormalSpeed = 0.0f;
    int iVar4 = checkRestHPAnime();
    if (iVar4 != 0 && iVar3 == 0) {
        s32 uVar2 = player02::heaps(m_tex_anm_heap)->mIdx == 0x234; /* m_tex_anm_heap.mIdx == mTexAnmIndexTable[daPyFace_TMABAF].mBtpIdx */
        u16 uVar1 = m3530;
        setSingleMoveAnime(player02::ANM_WAITATOB, 0.6f, 0.0f, 0xC, 6.0f);
        if (uVar2 == 0) {
            mModeFlg = (mModeFlg | 0x400) & ~0x100u;
        } else {
            setTextureAnime(0xE, (u32)uVar1);
        }
    } else {
        setBlendMoveAnime(2.4f);
    }
    current.angle.y = shape_angle.y;
    mDirection = 4; /* DIR_NONE */
    m35A0 = 0.0f;
    f32 dVar5 = cM_rndF(150.0f);
    mProcVar1 = (s16)gabi::ftoi(dVar5 + 300.0f);
    return TRUE;
}

/* 023E4F04 */
BOOL daPy_lk_c::procBtJump_init(fopEn_enemy_c* enemy) {
    gabi::Local<cXyz> local_34;
    cXyz_mi(&enemy->current.pos, local_34, &current.pos);
    commonProcInit(player02::daPyProc_BT_JUMP_e);
    gravity = -5.0f; // HD: HIO folded
    setSingleMoveAnime(player02::ANM_MJMP, 1.0f, 4.0f, -1, 1.0f);
    f32 fVar1 = gabi::fadds_ppc(enemy->mBtHeight, local_34->y);
    if (fVar1 < 30.0f) {
        fVar1 = 30.0f;
    }
    f32 dVar5 = std_sqrtf((-2.0f * fVar1) / gravity);
    m35A0 = -(dVar5 * gravity);
    gabi::Local<cXyz> xz; /* local_34.absXZ() */
    fcpy_l(xz.a + 0, local_34.a + 0);
    xz->y = 0.0f;
    fcpy_l(xz.a + 8, local_34.a + 8);
    m35A4 = std_sqrtf(PSVECSquareMag(xz)) / dVar5;
    s16 ang = fopAcM_searchActorAngleY(this, enemy);
    shape_angle.y = ang;
    current.angle.y = ang;
    lk_onPlayerStatus0(player02::daPyStts0_UNK2_e);
    mProcVar0 = 0;
    m370C.x = gabi::fnmsubs(enemy->mBtBodyR, cM_ssin(enemy->shape_angle.y), enemy->current.pos.x);
    fcpy_l(gabi::ea(&m370C.y), gabi::ea(&enemy->current.pos.y));
    m370C.z = gabi::fnmsubs(enemy->mBtBodyR, cM_scos(enemy->shape_angle.y), enemy->current.pos.z);
    mProcVar6 = 1; // HD: m_HIO->mBattle.mBJump.m.field_0x18 <= 0.0f folded
    fcpy_l(gabi::ea(&speed.y), gabi::ea(&m35A0));
    fcpy_l(gabi::ea(&mNormalSpeed), gabi::ea(&m35A4));
    LK_voiceStart(6);
    mNoResetFlg0 = mNoResetFlg0 & ~0x40000u; /* offNoResetFlg0(daPyFlg0_NO_FALL_VOICE) */
    endFlameDamageEmitter();
    return TRUE;
}

/* 023E50E0 */
BOOL daPy_lk_c::procBtRoll_init(fopEn_enemy_c* enemy) {
    commonProcInit(player02::daPyProc_BT_ROLL_e);
    mNormalSpeed = 0.0f;
    gabi::Local<cXyz> local_38;
    s16 ang = enemy->shape_angle.y;
    f32 r = enemy->mBtBodyR;
    local_38->x = gabi::fmuls_ppc(gabi::fadds_ppc(gabi::fnmsubs(r, cM_ssin(ang), enemy->current.pos.x), current.pos.x), 0.5f);
    local_38->y = gabi::fmuls_ppc(gabi::fadds_ppc(enemy->current.pos.y, current.pos.y), 0.5f);
    local_38->z = gabi::fmuls_ppc(gabi::fadds_ppc(gabi::fnmsubs(r, cM_scos(ang), enemy->current.pos.z), current.pos.z), 0.5f);
    int anm;
    if (mDirection == player02::DIR_LEFT) {
        mProcVar2 = -0x7FF0;
        anm = 0x6E; /* ANM_MROLLL */
    } else {
        mProcVar2 = 0x7FF0;
        anm = 0x6F; /* ANM_MROLLR */
    }
    setSingleMoveAnime(anm, 0.8f, 4.0f, 0x16, 5.0f); // HD: HIO folded
    gabi::Local<cXyz> local_2c;
    cXyz_mi(&current.pos, local_2c, local_38);
    gabi::Local<cXyz> xz; /* local_2c.absXZ() */
    fcpy_l(xz.a + 0, local_2c.a + 0);
    xz->y = 0.0f;
    fcpy_l(xz.a + 8, local_2c.a + 8);
    m35A0 = std_sqrtf(PSVECSquareMag(xz));
    mProcVar3 = cM_atan2s(local_2c->x, local_2c->z);
    shape_angle.y = fopAcM_searchActorAngleY(this, enemy);
    lk_onPlayerStatus0(player02::daPyStts0_UNK2_e);
    mFootEffectPosType = 4;
    LK_voiceStart(7);
    endFlameDamageEmitter();
    return TRUE;
}

/* 023E533C */
BOOL daPy_lk_c::procBtVerticalJump_init(fopEn_enemy_c* enemy) {
    commonProcInit(player02::daPyProc_BT_VERTICAL_JUMP_e);
    setSingleMoveAnime(player02::ANM_MSTEPOVER, 0.2f, 2.0f, 5, 6.0f); // HD: HIO folded
    mNormalSpeed = 0.0f;
    gravity = -9.0f;
    speed.y = 70.0f;
    mNoResetFlg0 = mNoResetFlg0 & ~0x40000u; /* offNoResetFlg0(daPyFlg0_NO_FALL_VOICE) */
    current.angle.y = shape_angle.y;
    LK_voiceStart(6);
    lk_onPlayerStatus0(player02::daPyStts0_UNK2_e);
    endFlameDamageEmitter();
    return TRUE;
}

/* 023E5400 */
BOOL daPy_lk_c::changeSpecialBattle() {
    if (mpAttnActorLockOn.get() != nullptr && (lk_getDoStatus() == 0x1A /* dActStts_PARRY_e */ || m34C5 == 5)) {
        fopEn_enemy_c* enemy = gabi::at<fopEn_enemy_c>(gabi::ea(mpAttnActorLockOn.get()));
        // HD: the frame test is written as !(start > now)
        if (m34C5 == 5 || (LK_doTrigger() /* spBattleTrigger() */ && !(enemy->mBtStartFrame > enemy->mBtNowFrame))) {
            u8 type = enemy->mBtAttackType;
            if (type == 1) {
                return procBtJump_init(enemy);
            }
            if (type == 2) {
                return procBtRoll_init(enemy);
            }
            if (type == 3) {
                return procBtVerticalJump_init(enemy);
            }
            if (type == 4) {
                setNoResetFlg1(noResetFlg1() | 0x20000000); /* onNoResetFlg1(daPyFlg1_LAST_COMBO_WAIT) */
                return TRUE;
            }
        }
    }
    return FALSE;
}

/* 023E5518 */
int daPy_lk_c::getCutDirection() {
    if (!(mStickDistance > 0.05f)) { // HD: NaN gives DIR_NONE
        return 5; /* DIR_NONE + 1 */
    }
    s16 angle;
    if (mpAttnActorLockOn.get() != nullptr) {
        s16 toActor = fopAcM_searchActorAngleY(this, mpAttnActorLockOn);
        angle = (s16)(m34E8 - toActor);
    } else {
        angle = (s16)(m34E8 - m34DE);
    }
    return getDirectionFromAngle((s32)angle) + 1;
}

/* 023E55A4 */
void daPy_lk_c::setBlurPosResource(u16 index) {
    // HD: the blur positions are taken from the loaded "LkAnm" archive (GameCube: JKRReadIdxResource into a buffer)
    u32 res = gabi::ea(dComIfG_getObjectRes("LkAnm", index));
    gabi::at<player02::swordBlur_c>(mpSwBlur)->mpPosBuffer = res; /* mpSwBlur->mpPosBuffer */
}

/* 023E5600 */
void daPy_lk_c::setAtParam(u32 type, int atp, int spl, u8 se, u8 hitMark, u8 cutType, f32 radius) {
    if (type == player02::AT_TYPE_SWORD) {
        if (noResetFlg1() & 0x8000) { /* checkNoResetFlg1(daPyFlg1_SOUP_POWER_UP) */
            atp = (s32)((u32)atp * 2);
        }
    } else {
        player02::cutState(this)->mCutCount = 0; /* mCutCount */
    }
    for (int i = 0; i < 3; i++) {
        dCcD_Cps* cps = &mAtCps[i];
        cps->SetAtHitMark(hitMark);
        cps->SetAtType(type);
        cps->SetAtSe(se);
        player02::setCapsuleRadius(cps, radius);
        cps->SetAtAtp((u8)atp);
        cps->SetAtSpl((u8)spl);
    }
    player02::cutState(this)->mCutType = cutType;
    setResetFlg0(resetFlg0() & ~0x08000000u); /* offResetFlg0(daPyRFlg0_NOT_ATTACKING) */
    mNoResetFlg0 = mNoResetFlg0 & ~0x10000000u; /* offNoResetFlg0(daPyFlg0_UNK10000000) */
}

/* 023E5678 */
void daPy_lk_c::setFinishCutAtParam(u8 cutType) {
    if (LK_checkNormalSwordEquip()) {
        m35FC = 1.5f; // HD: HIO folded
        setAtParam(player02::AT_TYPE_SWORD, 2, player02::dCcG_At_Spl_UNK1, player02::dCcG_SE_UNK1, player02::dCcG_AtHitMark_Big_e, cutType, 20.0f);
    } else {
        m35FC = 2.5f;
        setAtParam(2, 4, 1, 1, 0xF, cutType, 30.0f);
    }
}

/* 023E5B0C */
void daPy_lk_c::setNormalCutAtParam(u8 cutType) {
    if (LK_checkNormalSwordEquip()) {
        m35FC = 1.5f; // HD: HIO folded
        setAtParam(player02::AT_TYPE_SWORD, 1, player02::dCcG_At_Spl_UNK0, player02::dCcG_SE_UNK1, player02::dCcG_AtHitMark_Nrm_e, cutType, 20.0f);
    } else {
        m35FC = 2.5f;
        setAtParam(2, 2, 0, 1, 0xD, cutType, 30.0f);
    }
}

/* the finishing and extra cuts (EA/EB/ExB/ExA/Kesa): common head up to the SWORD_SWING status (HD: HIO folded;
 * m35EC is the start frame) */

/* 023E573C */
BOOL daPy_lk_c::procCutEA_init() {
    commonProcInit(player02::daPyProc_CUT_EA_e);
    setSingleMoveAnime(player02::ANM_CUTEA, 1.0f, 4.0f, 0x13, 2.0f);
    if (LK_checkNormalSwordEquip()) {
        LK_swordAnim_changeBckOnly(getItemAnimeResource(player02::dRes_INDEX_LKANM_BCK_CUTEAA_e));
    } else {
        LK_swordAnim_changeBckOnly(getItemAnimeResource(player02::dRes_INDEX_LKANM_BCK_CUTEAMS_e));
    }
    m35EC = 4.0f;
    LK_cXyzZero_copy(&m3700);
    m34C2 = 1;
    mProcVar0 = 0;
    mNoResetFlg0 = mNoResetFlg0 | 4; /* onNoResetFlg0(daPyFlg0_UNK4) */
    LK_voiceStart(1);
    lk_onPlayerStatus0(player02::daPyStts0_SWORD_SWING_e);
    setBlurPosResource(player02::dRes_INDEX_LKANM__CUTEA_POS_e);
    setFinishCutAtParam(player02::CUT_TYPE_CUT_EA);
    m34C4 = 0;
    m3522 = 0;
    return TRUE;
}

/* 023E5924 */
BOOL daPy_lk_c::procCutEB_init() {
    commonProcInit(player02::daPyProc_CUT_EB_e);
    setSingleMoveAnime(player02::ANM_CUTEB, 0.9f, 4.0f, 0x13, 2.0f);
    if (LK_checkNormalSwordEquip()) {
        LK_swordAnim_changeBckOnly(getItemAnimeResource(player02::dRes_INDEX_LKANM_BCK_CUTEBA_e));
    } else {
        LK_swordAnim_changeBckOnly(getItemAnimeResource(player02::dRes_INDEX_LKANM_BCK_CUTEBMS_e));
    }
    m35EC = 4.0f;
    LK_cXyzZero_copy(&m3700);
    m34C2 = 1;
    mProcVar0 = 0;
    mNoResetFlg0 = mNoResetFlg0 | 4; /* onNoResetFlg0(daPyFlg0_UNK4) */
    LK_voiceStart(1);
    lk_onPlayerStatus0(player02::daPyStts0_SWORD_SWING_e);
    setBlurPosResource(player02::dRes_INDEX_LKANM__CUTEB_POS_e);
    setFinishCutAtParam(player02::CUT_TYPE_CUT_EB);
    m34C4 = 0;
    m3522 = 0;
    return TRUE;
}

/* HD inline anm setFrame: frame (+4), the animation's frame (through the pointer at +ptrOff) and a
 * functor at +fnOff ({f32 result, f32 a, f32 b, ?, fn, arg}) re-evaluated, then 027DF40C(&functor)
 * (same helper as d_a_dr2.cpp; SHARED-CANDIDATE) */
static inline void lk_anm_setFrame_hd(be<f32>& currentFrame, gptr<be<f32>>& animationFrame,
                                     gptr<player02::animationFrameFunction_c>& frameFunction, f32 frame) {
    currentFrame = frame;
    *animationFrame = frame;
    player02::animationFrameFunction_c* function = frameFunction;
    u32 target = function->mEvaluate;
    f32 param3 = function->mParam3;
    f32 param2 = function->mParam2;
    u32 argument = function->mArgument;
    f32 result = gabi::call_ptr<f32>(target, argument, frame, param2, param3);
    function->mResult = result;
    player02FrameApply(gabi::ea(&frameFunction));
}

/* the normal cuts F/R/A/L: common head up to voiceStart(0) (HD: HIO folded) */

/* 023E5BD0 */
BOOL daPy_lk_c::procCutF_init(s16 param_0) {
    commonProcInit(player02::daPyProc_CUT_F_e);
    setSingleMoveAnime(player02::ANM_CUTF, 1.2f, 4.0f, 0x13, 2.5f);
    if (LK_checkNormalSwordEquip()) {
        LK_swordAnim_changeBckOnly(getItemAnimeResource(player02::dRes_INDEX_LKANM_BCK_CUTFA_e));
    } else {
        LK_swordAnim_changeBckOnly(getItemAnimeResource(player02::dRes_INDEX_LKANM_BCK_CUTFMS_e));
    }
    m35EC = 4.0f;
    player02::bpkFrame_c* bpk = gabi::at<player02::bpkFrame_c>(gabi::ea(this) + 0x4980);
    lk_anm_setFrame_hd(bpk->mFrame, bpk->mpFrame, bpk->mpFunction, 4.0f); /* mpCutfBpk->setFrame */
                     player02::btkFrame_c* btk = gabi::at<player02::btkFrame_c>(gabi::ea(this) + 0x49F4);
    lk_anm_setFrame_hd(btk->mFrame, btk->mpFrame, btk->mpFunction, 4.0f);
    LK_cXyzZero_copy(&m3700);
    m34C2 = 1;
    LK_voiceStart(0); /* mpCutfBtk->setFrame */
    mProcVar2 = param_0;
    m351E = m34DC;
    m3522 = 2;
    mNoResetFlg0 = mNoResetFlg0 | 4; /* onNoResetFlg0(daPyFlg0_UNK4) */
    lk_onPlayerStatus0(player02::daPyStts0_SWORD_SWING_e);
    setNormalCutAtParam(player02::CUT_TYPE_CUT_F); // HD: no setBlurPosResource
    return TRUE;
}

/* 023E5D94 */
BOOL daPy_lk_c::procCutR_init(s16 param_0) {
    commonProcInit(player02::daPyProc_CUT_R_e);
    setSingleMoveAnime(player02::ANM_CUTR, 1.2f, 4.0f, 0x13, 2.5f);
    if (LK_checkNormalSwordEquip()) {
        LK_swordAnim_changeBckOnly(getItemAnimeResource(player02::dRes_INDEX_LKANM_BCK_CUTRA_e));
    } else {
        LK_swordAnim_changeBckOnly(getItemAnimeResource(player02::dRes_INDEX_LKANM_BCK_CUTRMS_e));
    }
    m35EC = 4.0f;
    (void)0;
    LK_cXyzZero_copy(&m3700);
    m34C2 = 1;
    LK_voiceStart(0);
    mProcVar2 = param_0;
    m351E = m34DC;
    m3522 = 2;
    mNoResetFlg0 = mNoResetFlg0 | 4; /* onNoResetFlg0(daPyFlg0_UNK4) */
    setBlurPosResource(player02::dRes_INDEX_LKANM__CUTR_POS_e);
    lk_onPlayerStatus0(player02::daPyStts0_SWORD_SWING_e);
    setNormalCutAtParam(player02::CUT_TYPE_CUT_R);
    return TRUE;
}

/* 023E5F88 */
BOOL daPy_lk_c::procCutA_init(s16 param_0) {
    commonProcInit(player02::daPyProc_CUT_A_e);
    setSingleMoveAnime(player02::ANM_CUTA, 1.2f, 4.0f, 0x13, 2.5f);
    if (LK_checkNormalSwordEquip()) {
        LK_swordAnim_changeBckOnly(getItemAnimeResource(player02::dRes_INDEX_LKANM_BCK_CUTAA_e));
    } else {
        LK_swordAnim_changeBckOnly(getItemAnimeResource(player02::dRes_INDEX_LKANM_BCK_CUTAMS_e));
    }
    m35EC = 4.0f;
    current.angle.y = shape_angle.y /* HD */;
    LK_cXyzZero_copy(&m3700);
    m34C2 = 1;
    LK_voiceStart(0);
    mProcVar2 = param_0;
    lk_onPlayerStatus0(player02::daPyStts0_SWORD_SWING_e);
    m3522 = 2;
    mNoResetFlg0 = mNoResetFlg0 | 4; /* onNoResetFlg0(daPyFlg0_UNK4) */
    m351E = m34DC;
    setBlurPosResource(player02::dRes_INDEX_LKANM__CUTA_POS_e);
    setNormalCutAtParam(player02::CUT_TYPE_CUT_A);
    return TRUE;
}

/* 023E618C */
BOOL daPy_lk_c::procCutL_init(s16 param_0) {
    commonProcInit(player02::daPyProc_CUT_L_e);
    setSingleMoveAnime(player02::ANM_CUTL, 1.2f, 4.0f, 0x12, 2.5f);
    if (LK_checkNormalSwordEquip()) {
        LK_swordAnim_changeBckOnly(getItemAnimeResource(player02::dRes_INDEX_LKANM_BCK_CUTLA_e));
    } else {
        LK_swordAnim_changeBckOnly(getItemAnimeResource(player02::dRes_INDEX_LKANM_BCK_CUTLMS_e));
    }
    m35EC = 4.0f;
    (void)0;
    LK_cXyzZero_copy(&m3700);
    m34C2 = 1;
    LK_voiceStart(0);
    mProcVar2 = param_0;
    m351E = m34DC;
    m3522 = 2;
    mNoResetFlg0 = mNoResetFlg0 | 4; /* onNoResetFlg0(daPyFlg0_UNK4) */
    lk_onPlayerStatus0(player02::daPyStts0_SWORD_SWING_e);
    setBlurPosResource(player02::dRes_INDEX_LKANM__CUTL_POS_e);
    setNormalCutAtParam(player02::CUT_TYPE_CUT_L);
    return TRUE;
}

/* 023E6380 */
BOOL daPy_lk_c::procCutTurn_init(BOOL param_0) {
    commonProcInit(player02::daPyProc_CUT_TURN_e);
    f32 fVar1 = param_0 ? 5.0f : 2.0f; // HD: HIO folded
    if (mEquipItem == 0x103 /* daPyItem_SWORD_e */) {
        setSingleMoveAnime(player02::ANM_CUTTURN, 1.2f, fVar1, 0x15, 3.0f);
    } else {
        setSingleMoveAnime(player02::ANM_CUTTURNC, 1.2f, fVar1, 0x15, 3.0f);
    }
    if (mEquipItem == 0x103 /* daPyItem_SWORD_e */) {
        if (LK_checkNormalSwordEquip()) {
            LK_swordAnim_changeBckOnly(getItemAnimeResource(player02::dRes_INDEX_LKANM_BCK_CUTTURNA_e));
        } else {
            LK_swordAnim_changeBckOnly(getItemAnimeResource(player02::dRes_INDEX_LKANM_BCK_CUTTURNMS_e));
        }
        if (LK_checkNormalSwordEquip()) {
            m35A4 = 180.0f;
            LK_mAtCyl_SetAtAtp((noResetFlg1() & 0x8000) ? 4 : 2); /* daPyFlg1_SOUP_POWER_UP */
        } else {
            m35A4 = 230.0f;
            LK_mAtCyl_SetAtAtp((noResetFlg1() & 0x8000) ? 8 : 4);
        }
        setBlurPosResource(player02::dRes_INDEX_LKANM__CUTTURN_POS_e);
    } else {
        u32 boko = gabi::ea(mActorKeepEquip.mActor.get());
        u32 boko_type;
        if (boko == 0) {
            LK_mAtCyl_SetAtAtp(1);
            m35A4 = 180.0f; /* daBoko_c::Type_BOKO_STICK_e */
        } else {
            boko_type = (u32)gabi::at<fopAc_ac_c>(boko)->mParameters; /* fopAcM_GetParam(boko) */
            LK_mAtCyl_SetAtAtp(player02::bokoAttackPoint(boko_type)); /* boko->getAtPoint() */
            if (boko_type == 0) {        /* Type_BOKO_STICK_e */
                m35A4 = 180.0f;
            } else if (boko_type == 1) { /* Type_MACHETE_e */
                m35A4 = 200.0f;
            } else if (boko_type == 2) { /* Type_STALFOS_MACE_e */
                m35A4 = 250.0f;
            } else if (boko_type == 3) { /* Type_DARKNUT_SWORD_e */
                m35A4 = 270.0f;
            } else if (boko_type == 4) { /* Type_MOBLIN_SPEAR_e */
                m35A4 = 360.0f;
            } else {
                m35A4 = 250.0f;
            }
        }
        setBlurPosResource(player02::dRes_INDEX_LKANM__WEAPONTURN_POS_e);
    }
    LK_mAtCyl_SetR(gabi::fmuls_ppc(m35A4, 0.5f));
    m35EC = fVar1;
    LK_cXyzZero_copy(&m3700);
    m34C2 = 1;
    mProcVar0 = 1; // HD: HIO folded
    current.angle.y = shape_angle.y;
    m3578 = 0;
    LK_voiceStart(1);
    lk_onPlayerStatus0(player02::daPyStts0_SWORD_SWING_e);
    player02::cutState(this)->mCutType = player02::CUT_TYPE_CUT_TURN; /* mCutType = CUT_TYPE_CUT_TURN */
    setResetFlg0(resetFlg0() & ~0x08000000u); /* offResetFlg0(daPyRFlg0_NOT_ATTACKING) */
    LK_mAtCyl_SetR(gabi::fmuls_ppc(m35A4, 0.5f));
    if (!(mNoResetFlg0 & 0x80000000u)) { /* !checkNoResetFlg0(daPyFlg0_UNK80000000) */
        player02MakeEmitter(gabi::ea(&m32E4), 0x1E /* ID_AK_JN_ROUNDATTACKTOE */,
                   lk_getAnmMtx(mpCLModel, 0x28 /* CL_JNT_RTOE_JNT_e */), gabi::ea(&current.pos), 0);
        if (mCurrAttributeCode == 4 /* dBgS_Attr_GRASS_e */) {
            gabi::Local<GXColor> color; /* tevStr.mColorC0 (HD: s16 components at 0x1A0) */
            color->g = (u8)tevStr.mColorC0.g;
            color->r = (u8)tevStr.mColorC0.r;
            color->a = (u8)tevStr.mColorC0.a;
            color->b = (u8)tevStr.mColorC0.b;
            player02MakeEmitterColor(gabi::ea(&m32F0), 0x21 /* ID_AK_JN_ROUNDATTACKKUSA */,
                       lk_getAnmMtx(mpCLModel, 0x23 /* CL_JNT_LTOE_JNT_e */), gabi::ea(&current.pos), color.a, gabi::ea(&tevStr.mColorK0));
            m35A0 = 18.0f;
        } else if (mCurrAttributeCode == 0x13 /* dBgS_Attr_WATER_e */) {
            gabi::Local<be<u32>> amb;
            gabi::Local<be<u32>> dif;
            player02SeaColor(amb.a, dif.a);
            player02MakeEmitterColor(gabi::ea(&m32F0), 0x20 /* ID_AK_JN_ROUNDATTACKSHIBUKI */,
                       lk_getAnmMtx(mpCLModel, 0x23 /* CL_JNT_LTOE_JNT_e */), gabi::ea(&current.pos), amb.a, 0);
            m35A0 = 17.0f;
        } else {
            /* dComIfGp_particle_setToonP1 */
            dPa_control_set(dComIfGp_getParticle(), 3, 0x201F /* ID_AK_JT_ROUNDATTACKSMOKE */, &current.pos, nullptr, nullptr, 0xA0,
                            gabi::at<dPa_levelEcallBack>(gabi::ea(this) + 0x6710) /* &mSmokeEcallBack */, -1, nullptr, nullptr, nullptr);
            m35A0 = 21.0f;
        }
    }
    mProcVar6 = 0;
    if (m34C4 == 6) {
        m34C4 = 0;
        mNoResetFlg0 = mNoResetFlg0 | 4; /* onNoResetFlg0(daPyFlg0_UNK4) */
        mProcVar6 = 1;
    } else {
        player02::cutState(this)->mCutCount = 0; /* mCutCount */
        m34C4 = 0;
    }
    mNormalSpeed = 0.0f;
    endFlameDamageEmitter();
    return TRUE;
}

/* 023E6A58 */
void daPy_lk_c::setExtraFinishCutAtParam(u8 cutType) {
    if (LK_checkNormalSwordEquip()) {
        m35FC = 1.7f; // HD: HIO folded
        setAtParam(player02::AT_TYPE_SWORD, 2, player02::dCcG_At_Spl_UNK1, player02::dCcG_SE_UNK1, player02::dCcG_AtHitMark_Big_e, cutType, 50.0f);
    } else {
        m35FC = 2.0f;
        setAtParam(2, 4, 1, 1, 0xF, cutType, 50.0f);
    }
}

/* 023E6B1C */
BOOL daPy_lk_c::procCutExB_init() {
    commonProcInit(player02::daPyProc_CUT_EX_B_e);
    setSingleMoveAnime(player02::ANM_EXCB1, 0.9f, 5.0f, 0x24, 5.0f);
    if (LK_checkNormalSwordEquip()) {
        LK_swordAnim_changeBckOnly(getItemAnimeResource(player02::dRes_INDEX_LKANM_BCK_EXCB1A_e));
    } else {
        LK_swordAnim_changeBckOnly(getItemAnimeResource(player02::dRes_INDEX_LKANM_BCK_EXCB1MS_e));
    }
    m35EC = 5.0f;
    LK_cXyzZero_copy(&m3700);
    m34C2 = 1;
    mProcVar0 = 0;
    mNoResetFlg0 = mNoResetFlg0 | 4; /* onNoResetFlg0(daPyFlg0_UNK4) */
    LK_voiceStart(1);
    lk_onPlayerStatus0(player02::daPyStts0_SWORD_SWING_e);
    setBlurPosResource(player02::dRes_INDEX_LKANM__CUTEXB_POS_e);
    setExtraFinishCutAtParam(player02::CUT_TYPE_CUT_EXB);
    m34C4 = 0;
    m3522 = 0;
    return TRUE;
}

/* 023E6D00 */
void daPy_lk_c::setExtraCutAtParam(u8 cutType) {
    if (LK_checkNormalSwordEquip()) {
        m35FC = 1.7f; // HD: HIO folded
        setAtParam(player02::AT_TYPE_SWORD, 1, player02::dCcG_At_Spl_UNK0, player02::dCcG_SE_UNK1, player02::dCcG_AtHitMark_Nrm_e, cutType, 50.0f);
    } else {
        m35FC = 2.0f;
        setAtParam(2, 2, 0, 1, 0xD, cutType, 50.0f);
    }
}

/* 023E6DC4 */
BOOL daPy_lk_c::procCutExMJ_init(int param_0) {
    commonProcInit(player02::daPyProc_CUT_EX_MJ_e);
    int dVar3;
    if (param_0 != 0) {
        dVar3 = 0x71; /* ANM_MROLLRC */
        mProcVar3 = 0x71;
        setBlurPosResource(player02::dRes_INDEX_LKANM__BTROTATECUTR_POS_e);
        m35A0 = 3.0f; // HD: HIO folded
    } else {
        dVar3 = 0x70; /* ANM_MROLLLC */
        mProcVar3 = 0x70;
        setBlurPosResource(player02::dRes_INDEX_LKANM__BTROTATECUTL_POS_e);
        m35A0 = 1.0f;
    }
    setSingleMoveAnime(dVar3, 0.9f, 0.0f, 6, 1.0f);
    if (LK_checkNormalSwordEquip()) {
        LK_swordAnim_changeBckOnly(getItemAnimeResource(player02::dRes_INDEX_LKANM_BCK_MROLLCA_e));
    } else {
        LK_swordAnim_changeBckOnly(getItemAnimeResource(player02::dRes_INDEX_LKANM_BCK_MROLLCMS_e));
    }
    m35EC = 0.0f;
    mNoResetFlg0 = mNoResetFlg0 | 4; /* onNoResetFlg0(daPyFlg0_UNK4) */
    LK_voiceStart(1);
    lk_onPlayerStatus0(player02::daPyStts0_SWORD_SWING_e);
    if (m34C4 != 6) {
        m3522 = 2;
        setExtraCutAtParam(player02::CUT_TYPE_CUT_EXMJ);
    } else {
        m34C4 = 0;
        m3522 = 0;
        setExtraFinishCutAtParam(0x1E);
    }
    speed.y = 29.0f;
    gravity = -5.0f;
    mProcVar2 = 0;
    mNoResetFlg0 = mNoResetFlg0 & ~0x40040u; /* offNoResetFlg0(daPyFlg0_NO_FALL_VOICE | daPyFlg0_CUT_AT_FLG) */
    mNormalSpeed = 7.5f;
    return TRUE;
}

/* 023E7054 */
BOOL daPy_lk_c::procCutKesa_init() {
    commonProcInit(player02::daPyProc_CUT_KESA_e);
    setSingleMoveAnime(player02::ANM_CUTKESA, 1.3f, 4.0f, 0x31, 1.0f);
    if (LK_checkNormalSwordEquip()) {
        LK_swordAnim_changeBckOnly(getItemAnimeResource(player02::dRes_INDEX_LKANM_BCK_EXCA1A_e));
    } else {
        LK_swordAnim_changeBckOnly(getItemAnimeResource(player02::dRes_INDEX_LKANM_BCK_EXCA1MS_e));
    }
    m35EC = 4.0f;
    LK_cXyzZero_copy(&m3700);
    m34C2 = 1;
    (void)0;
    mNoResetFlg0 = mNoResetFlg0 | 4; /* onNoResetFlg0(daPyFlg0_UNK4) */
    LK_voiceStart(1);
    lk_onPlayerStatus0(player02::daPyStts0_SWORD_SWING_e);
    setBlurPosResource(player02::dRes_INDEX_LKANM__CUTKESA_POS_e);
    setExtraCutAtParam(player02::CUT_TYPE_CUT_KESA);
    m3522 = 2;
    return TRUE;
}

/* 023E7224 */
BOOL daPy_lk_c::procCutExA_init() {
    commonProcInit(player02::daPyProc_CUT_EX_A_e);
    setSingleMoveAnime(player02::ANM_EXCA1, 1.1f, 4.0f, 0x15, 1.0f);
    if (LK_checkNormalSwordEquip()) {
        LK_swordAnim_changeBckOnly(getItemAnimeResource(player02::dRes_INDEX_LKANM_BCK_EXCA1A_e));
    } else {
        LK_swordAnim_changeBckOnly(getItemAnimeResource(player02::dRes_INDEX_LKANM_BCK_EXCA1MS_e));
    }
    m35EC = 4.0f;
    LK_cXyzZero_copy(&m3700);
    m34C2 = 1;
    mProcVar0 = 0;
    mNoResetFlg0 = mNoResetFlg0 | 4; /* onNoResetFlg0(daPyFlg0_UNK4) */
    LK_voiceStart(1);
    lk_onPlayerStatus0(player02::daPyStts0_SWORD_SWING_e);
    setBlurPosResource(player02::dRes_INDEX_LKANM__CUTEXA_POS_e);
    setExtraCutAtParam(player02::CUT_TYPE_CUT_EXA);
    m3522 = 1;
    return TRUE;
}

/* 023E73FC */
int daPy_lk_c::changeCutProc() {
    if (m34C5 == 5 && changeSpecialBattle()) {
        m34C4 = 0;
        return TRUE;
    }
    m34C4 = m34C4 + 1;
    int direction = getCutDirection() - 1;
    s16 sVar2;
    if (lk_checkAttentionLock() || !(mStickDistance > 0.05f)) {
        sVar2 = shape_angle.y;
    } else {
        sVar2 = m34E8;
    }
    u8 bVar1 = m34C4;
    if (bVar1 > 4) {
        player02::cutState(this)->mCutCount = (u8)(bVar1 - 2); /* mCutCount */
    } else {
        player02::cutState(this)->mCutCount = bVar1;
    }
    bVar1 = m34C4;
    if (bVar1 == 4) {
        if (direction == player02::DIR_RIGHT || direction == player02::DIR_FORWARD) {
            procCutEA_init();
        } else {
            procCutEB_init();
        }
    } else if (bVar1 < 4) {
        if (direction == player02::DIR_FORWARD) {
            procCutF_init(sVar2);
        } else if (direction == player02::DIR_RIGHT) {
            procCutR_init(sVar2);
        } else if (direction == player02::DIR_NONE && lk_checkAttentionLock()) {
            procCutA_init(sVar2);
        } else {
            procCutL_init(sVar2);
        }
    } else if (bVar1 == 6) {
        if (direction == player02::DIR_FORWARD) {
            procCutTurn_init(TRUE);
        } else if (direction == player02::DIR_NONE) {
            procCutExB_init();
        } else if (direction == player02::DIR_RIGHT) {
            procCutExMJ_init(1);
        } else {
            procCutExMJ_init(0);
        }
    } else if (bVar1 < 6) {
        if (direction == player02::DIR_FORWARD) {
            procCutExMJ_init(1);
        } else if (direction == player02::DIR_RIGHT) {
            procCutKesa_init();
        } else {
            procCutExA_init();
        }
    }
    m3578 = 0;
    return TRUE;
}

/* 023E7660 */
void daPy_lk_c::setFanModel() {
    u32 bck = getItemAnimeResource(player02::dRes_INDEX_LKANM_BCK_USEFANAA_e);
    u32 oldHeap = gabi::ea(setItemHeap());
    u32 tmp_modelData = initModel(gabi::ea(&player02::equipment(this)->mpEquipItemModel), player02::dRes_INDEX_LINK_BDL_FAN_e, 0x37221222);
    /* mSwordAnim.init(tmp_modelData, bck, false, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false) */
    int ret = player02BckInit(gabi::ea(&player02::equipment(this)->mSwordAnim), tmp_modelData, bck, 0, 2, 1.0f, 0, -1, 0);
    if (!ret) {
        JUT_ASSERT_fail("d_a_player_fan.inc", 0x105, "0");
    }
    player02RestoreHeap(oldHeap);
}

// HD: sead::SafeString equality (operator==), after the same-pointer test: the same characters (at most 0x40001)
static inline BOOL lk_safeStringEq(u32 s1, u32 s2) {
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c = (u8)player02::indexed(gabi::at<be<u8>>(s1), i);
        if (c != (u8)player02::indexed(gabi::at<be<u8>>(s2), i)) {
            return FALSE;
        }
        if (c == 0) {
            return TRUE;
        }
    }
    return FALSE;
}
/* compare the material's name (a relative offset at +4 of the name block at +0) with a C string through two
 * SafeString temporaries (the TU's vtable; slot 0x14 assures the termination) */
static inline BOOL lk_matNameIs(gabi::Local<SafeString>& a, gabi::Local<SafeString>& b, u32 mtl, u32 str) {
    a->__vtbl = gc_tu.safeString;
    a->mStringTop = str;
    u32 blk = gabi::ea(gabi::at<player02::fanMaterial_c>(mtl)->mpName.get());
    s32 off = gabi::at<player02::materialName_c>(blk)->mNameOffset;
    b->__vtbl = gc_tu.safeString;
    b->mStringTop = off != 0 ? blk + 4 + off : 0;
    player02SafeStringCtor(a.a); /* (an empty function) */
    player02::assureTermination(a);
    u32 s1 = a->mStringTop;
    player02::assureTermination(b);
    if (s1 == b->mStringTop) {
        return TRUE;
    }
    return lk_safeStringEq(a->mStringTop, b->mStringTop);
}

/* 023E771C */
int daPy_lk_c::setShapeFanLeaf() {
    u32 mtl = 0;
    u32 model = gabi::ea(player02::equipment(this)->mpEquipItemModel.get()); /* mpEquipItemModel */
    if (model != 0) {
        /* getModelData()->getJointNodePointer(FAN_JNT_CL_FAN_e)->getMesh() (HD layout) */
        J3DModelData* data = gabi::at<J3DModel>(model)->getModelData();
        mtl = gabi::ea(gabi::at<player02::fanJoint_c>(data->mJointNodes)->mpMesh.get());
    }
    m355C = (s16)(m355C + 0x82F);
    m3558 = (s16)gabi::ftoi(1536.0f * cM_ssin(m355C));
    m355A = (s16)gabi::ftoi(2048.0f * cM_ssin(m355C - 0xE00));
    if (dComIfGs_getMagic() != 0) { /* dComIfGs_getMagic() >= 1 */
        for (; mtl != 0; mtl = gabi::ea(gabi::at<player02::fanMaterial_c>(mtl)->mpNext.get())) {
            gabi::at<player02::fanMaterial_c>(mtl)->mpShape->show(); /* mtl->getShape() flag */
        }
        return TRUE;
    }
    for (u32 m = mtl; m != 0; m = gabi::ea(gabi::at<player02::fanMaterial_c>(m)->mpNext.get())) {
        gabi::at<player02::fanMaterial_c>(m)->mpShape->hide();
    }
    // HD: only the "veinsAMat" / "veinsBMat" material is flagged (GameCube: "leafAMat" hidden)
    gabi::Local<SafeString> a, b, c, d;
    for (; mtl != 0; mtl = gabi::ea(gabi::at<player02::fanMaterial_c>(mtl)->mpNext.get())) {
        if (lk_matNameIs(a, b, mtl, gabi::guest_addr("veinsAMat")) || lk_matNameIs(c, d, mtl, gabi::guest_addr("veinsBMat"))) {
            gabi::at<player02::fanMaterial_c>(mtl)->mpShape->show();
            return FALSE;
        }
    }
    return FALSE;
}

/* 023E79DC */
BOOL daPy_lk_c::procFanSwing_init() {
    if (mCurProc == player02::daPyProc_FAN_SWING_e) { /* HD */
        return FALSE;
    }
    commonProcInit(0x92);
    setSingleMoveAnime(player02::ANM_USEFANA, 1.0f, 0.0f, 0x22, 5.0f); // HD: HIO folded
    setFanModel();
    mProcVar6 = 0;
    LK_voiceStart(7);
    setAtParam(player02::AT_TYPE_FAN_SWING, 1, player02::dCcG_At_Spl_UNK0, player02::dCcG_SE_WOOD, player02::dCcG_AtHitMark_Nrm_e, player02::CUT_TYPE_NONE, 50.0f);
    m35EC = 0.0f;
    if (setShapeFanLeaf()) {
        mProcVar0 = 1;
        seStartOnlyReverb(0x2843 /* JA_SE_LK_FAN_PRE_SWING */);
        lk_onPlayerStatus1(player02::daPyStts1_DEKU_LEAF_FAN_e);
    } else {
        mProcVar0 = 0;
    }
    return TRUE;
}

/* 023E7B24 */
void daPy_lk_c::setPriTextureAnime(u16 r4, int r30) {
    player02::TexAnmTableEntry& entry = player02::indexed(player02_cpp::mTexAnmIndexTable, r4);
    u16 btpIdx = entry.mBtpIdx;
    u16 btkIdx = entry.mBtkIdx;
    player02::anmHeap_c* tex = player02::heaps(m_tex_anm_heap);
    player02::anmHeap_c* scroll = player02::heaps(m_tex_scroll_heap);
    if (tex->field_0x2 != btpIdx) {
        tex->field_0x2 = btpIdx;
        if (tex->field_0x4 == 0xFFFF) {
            u32 res = loadTextureAnimeResource((u32)btpIdx, 0);
            setTextureAnimeResource(res, r30);
        }
    }
    if (scroll->field_0x2 != btkIdx) {
        scroll->field_0x2 = btkIdx;
        if (scroll->field_0x4 == 0xFFFF) {
            u32 res = loadTextureScrollResource((u32)btkIdx, 0);
            setTextureScrollResource(res, r30);
        }
    }
}

/* 023E7BEC */
void daPy_lk_c::setBowReadyAnime() {
    // HD: HIO folded (m_HIO->mItem.mBow.m.field_0x0 = 4)
    setActAnimeUpper(player02::dRes_INDEX_LKANM_BCK_ARROWSHOOT_e, player02::UPPER_MOVE2_e, 1.0f, 3.999f, 4, 10.0f);
    LK_swordAnim_changeBckOnly(getItemAnimeResource(player02::dRes_INDEX_LKANM_BCK_ARROWSHOOTA_e));
    m35EC = 3.999f;
    setPriTextureAnime(0x70, 0);
    m355E = 10;
    seStartOnlyReverb(0x2810 /* JA_SE_LK_ITEM_TAKEOUT */);
}

/* 023E7C98 */
BOOL daPy_lk_c::procBowSubject_init() {
    if (mCurProc == player02::daPyProc_BOW_SUBJECT_e) {
        return FALSE;
    }
    commonProcInit(0x94);
    if (!checkBowAnime()) {
        setBowReadyAnime();
    }
    setSingleMoveAnime(player02::ANM_ATNRS, 0.0f, 0.0f, -1, 2.4f);
    mNormalSpeed = 0.0f;
    lk_onPlayerStatus0(player02::daPyStts0_BOW_AIM_e);
    return TRUE;
}

/* 023E7D6C */
s16 daPy_lk_c::getGroundAngle(u32 param_1, s16 param_2) {
    u32 plane = dBgS_GetTriPla_l(param_1);
    if (plane == 0 || player02::plane(plane)->mNormal.y < 0.5f) { /* !cBgW_CheckBGround(plane->GetNP()->y) */
        return 0;
    }
    s16 a = cM_atan2s(player02::plane(plane)->mNormal.x, player02::plane(plane)->mNormal.z);
    f32 z = player02::plane(plane)->mNormal.z;
    f32 x = player02::plane(plane)->mNormal.x;
    f32 cos = cM_scos((s16)(a - param_2));
    f32 xz = std_sqrtf(gabi::fmadds(x, x, z * z));
    return cM_atan2s(gabi::fmuls_ppc(xz, cos), player02::plane(plane)->mNormal.y);
}

/* 023E7E5C */
void daPy_lk_c::setBlendAtnBackMoveAnime(f32 param_1) {
    f32 fVar1;
    if (m3580 == 8) {
        fVar1 = 1.0f;
    } else {
        fVar1 = cM_scos(m34E2);
    }
    f32 dVar7 = std::fabs(gabi::fmuls_ppc(fVar1, mNormalSpeed)) / mMaxNormalSpeed;
    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[1]; /* UNDER_MOVE1_e */
    // HD: HIO folded (mAtnMoveB 0x1C = 0.75, 0x20 = 1.0, 0x24 = 0.8, 0x28 = 0.95; mMove 0x38 = 1.1)
    if (dVar7 < 0.75f) {
        f32 f = dVar7 / 0.75f;
        if (mModeFlg & 1) { /* checkModeFlg(ModeFlg_00000001) */
            m3598 = 0.0f;
            setMoveAnime(f, 1.1f, 0.8f, player02::ANM_WAITS, player02::ANM_ATNWB, 2, param_1);
        } else {
            m3598 = 1.0f;
            setMoveAnime(f, 1.1f, 0.8f, 0, 0xE, 4, param_1);
        }
        if (!(mModeFlg & 1)) {
            if (frameCtrl->checkPass(2.0f)) {
                setResetFlg0(resetFlg0() | 0x400); /* onResetFlg0(daPyRFlg0_RIGHT_FOOT_ON_GROUND) */
            } else if (frameCtrl->checkPass(12.0f)) {
                setResetFlg0(resetFlg0() | 0x800); /* onResetFlg0(daPyRFlg0_LEFT_FOOT_ON_GROUND) */
            }
        }
    } else {
        f32 f;
        if (dVar7 < 1.0f) {
            f = (dVar7 - 0.75f) * 4.0f;
            setMoveAnime(f, 0.8f, 0.95f, player02::ANM_ATNWB, player02::ANM_ATNDB, 4, param_1);
            m3598 = 1.0f - f;
        } else {
            gabi::Local<cXyz> xz; /* m36A0.abs2XZ() */
            fcpy_l(xz.a + 0, gabi::ea(&m36A0.x));
            xz->y = 0.0f;
            fcpy_l(xz.a + 8, gabi::ea(&m36A0.z));
            if (!(PSVECSquareMag(xz) < 49.0f)) { // HD: NaN takes the fast branch
                setResetFlg0(resetFlg0() | 0x40000); /* onResetFlg0(daPyRFlg0_UNK40000) */
                setMoveAnime(1.0f, 1.805f, 1.805f, 0xF, 0xF, 4, param_1); /* 1.9f * 0.95f */
            } else {
                setMoveAnime(1.0f, 0.95f, 0.95f, 0xF, 0xF, 4, param_1);
            }
            m3598 = 0.0f;
        }
        if (frameCtrl->checkPass(5.0f) || frameCtrl->checkPass(15.0f)) {
            setResetFlg0(resetFlg0() | 0x400);
        } else if (frameCtrl->checkPass(3.0f) || frameCtrl->checkPass(13.0f)) {
            setResetFlg0(resetFlg0() | 0x800);
        }
    }
    if (!(dVar7 < 0.9f)) { // HD: NaN sets the flag
        setResetFlg0(resetFlg0() | 0x10); /* onResetFlg0(daPyRFlg0_UNK10) */
    }
    if (!(resetFlg0() & 0xC00)) { /* !getFootOnGround() */
        resetFootEffect();
    }
    setHandModel(player02::ANM_ATNDB);
}

/* 023E81E4 */
void daPy_lk_c::setBlendAtnMoveAnime(f32 f30) {
    s16 r3;
    if ((mAcch.m_flags & dBgS_Acch::GROUND_HIT) || !dBgS_ChkPolySafe_l(mAcchGndPoly)) {
        r3 = 0;
    } else {
        r3 = getGroundAngle(mAcchGndPoly, current.angle.y);
    }
    f32 f31 = std::fabs(gabi::fmuls_ppc(mNormalSpeed, cM_scos(r3)) / mMaxNormalSpeed);
    s16 iVar6 = (s16)(current.angle.y - shape_angle.y);
    f32 f2 = cM_ssin(iVar6);
    f32 fVar4 = cM_scos(iVar6);
    u8 uVar1 = mDirection;
    if (LK_demoMode() == 0x17 /* daPy_demo_c::DEMO_A_WAIT_e */) {
        if (demoParam0() == 1) {
            mDirection = 2; /* DIR_LEFT */
        } else {
            mDirection = 3; /* DIR_RIGHT */
        }
    } else if (mStickDistance > 0.05f) {
        // HD: HIO folded (mAtnMoveB 0x30 = -0.99, 0x2C = 0.99); the NaN-safe forms of the original's branches
        if (mpAttnActorLockOn.get() == nullptr && (!(fVar4 > -0.99f) || !(fVar4 < 0.99f))) {
            mDirection = !(fVar4 > -0.99f) ? player02::DIR_BACKWARD : player02::DIR_FORWARD;
        } else {
            if (uVar1 == 1 || uVar1 == 0) {
                mDirection = 3; /* DIR_RIGHT */
                mMaxNormalSpeed = 12.0f;
            }
            if (f2 > 0.0f) {
                mDirection = 2; /* DIR_LEFT */
            } else if (f2 < 0.0f) {
                mDirection = 3; /* DIR_RIGHT */
            }
        }
    }
    u8 uVar2 = mDirection;
    if (uVar1 != uVar2) {
        f30 = 2.4f; /* m_HIO->mBasic.m.field_0xC */
    }
    if (uVar2 == player02::DIR_BACKWARD) {
        mMaxNormalSpeed = 15.0f;
        setBlendAtnBackMoveAnime(f30);
        return;
    }
    if (uVar2 == player02::DIR_FORWARD) {
        mMaxNormalSpeed = 17.0f;
        setBlendMoveAnime(f30);
        return;
    }
    if (uVar2 != 3 && uVar2 != 2) {
        mDirection = 3; /* DIR_RIGHT */
        f30 = 2.4f;
    }
    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[1]; /* UNDER_MOVE1_e */
    f32 f29 = checkHeavyStateOn() ? 0.6f : 1.0f;
    int dVar8, dVar9;
    // HD: HIO folded (mAtnMove 0x1C = 0.01, 0x20 = 0.9, 0x24 = 1.25, 0x28 = 1.0, 0x2C = 1.8)
    if (f31 < 0.01f) {
        f32 f1 = f31 / 0.01f;
        if (mDirection == player02::DIR_LEFT) {
            dVar8 = 7;  /* ANM_ATNLS */
            dVar9 = 9;  /* ANM_ATNWLS */
        } else {
            dVar8 = 8;  /* ANM_ATNRS */
            dVar9 = 10; /* ANM_ATNWRS */
        }
        if (mModeFlg & 1) { /* checkModeFlg(ModeFlg_00000001) */
            m3598 = 0.0f;
            setMoveAnime(f1, 1.25f, f29, dVar8, dVar9, 2, f30);
        } else {
            m3598 = 1.0f;
            setMoveAnime(f1, 1.25f, f29, dVar8, dVar9, 4, f30);
        }
    } else if (f31 < 0.9f) {
        f32 f28 = (f31 - 0.01f) / 0.89f;
        if (mDirection == player02::DIR_LEFT) {
            dVar8 = 9;   /* ANM_ATNWLS */
            dVar9 = 0xB; /* ANM_ATNDLS */
        } else {
            dVar8 = 10;  /* ANM_ATNWRS */
            dVar9 = 0xC; /* ANM_ATNDRS */
        }
        setMoveAnime(f28, f29, 1.8f * f29, dVar8, dVar9, 4, f30);
        m3598 = gabi::fnmsubs(f28, m3598, 1.0f); /* 1.0f - f28 * m3598 */
    } else {
        dVar9 = mDirection == player02::DIR_LEFT ? player02::ANM_ATNDLS : player02::ANM_ATNDRS;
        gabi::Local<cXyz> xz; /* m36A0.abs2XZ() */
        fcpy_l(xz.a + 0, gabi::ea(&m36A0.x));
        xz->y = 0.0f;
        fcpy_l(xz.a + 8, gabi::ea(&m36A0.z));
        if (!(PSVECSquareMag(xz) < 49.0f)) { // HD: NaN takes the fast branch
            setResetFlg0(resetFlg0() | 0x40000); /* onResetFlg0(daPyRFlg0_UNK40000) */
            setMoveAnime(1.0f, 3.4199998f, 3.4199998f, dVar9, dVar9, 4, f30); /* 1.9f * 1.8f */
        } else {
            f32 f = 1.8f * f29;
            setMoveAnime(1.0f, f, f, dVar9, dVar9, 4, f30);
        }
        m3598 = 0.0f;
    }
    if (!(f31 < 0.9f)) { // HD: NaN sets the flag
        setResetFlg0(resetFlg0() | 0x10); /* onResetFlg0(daPyRFlg0_UNK10) */
    }
    if (!(mModeFlg & 1)) {
        if (mDirection == player02::DIR_LEFT) {
            if (frameCtrl->checkPass(2.0f)) {
                setResetFlg0(resetFlg0() | 0x800); /* onResetFlg0(daPyRFlg0_LEFT_FOOT_ON_GROUND) */
            } else if (frameCtrl->checkPass(10.0f)) {
                setResetFlg0(resetFlg0() | 0x400); /* onResetFlg0(daPyRFlg0_RIGHT_FOOT_ON_GROUND) */
            } else {
                resetFootEffect();
            }
        } else {
            if (frameCtrl->checkPass(10.0f)) {
                setResetFlg0(resetFlg0() | 0x800);
            } else if (frameCtrl->checkPass(2.0f)) {
                setResetFlg0(resetFlg0() | 0x400);
            } else {
                resetFootEffect();
            }
        }
    }
    setHandModel(dVar9);
}

/* 023E88B0 */
BOOL daPy_lk_c::procBowMove_init() {
    if (mCurProc == player02::daPyProc_BOW_MOVE_e) {
        return FALSE;
    }
    commonProcInit(0x95);
    if (mDirection == player02::DIR_LEFT) {
        current.angle.y = (s16)(shape_angle.y + 0x4000);
    } else {
        mDirection = 3; /* DIR_RIGHT */
        current.angle.y = (s16)(shape_angle.y - 0x4000);
    }
    if (!checkBowAnime()) {
        setBowReadyAnime();
    }
    setBlendAtnMoveAnime(2.4f /* m_HIO->mBasic.m.field_0xC */);
    lk_onPlayerStatus0(player02::daPyStts0_BOW_AIM_e);
    return TRUE;
}

/* 023E8984 */
BOOL daPy_lk_c::checkNextBowMode() {
    if (lk_checkAttentionLock()) {
        return procBowMove_init();
    }
    return procBowSubject_init();
}

/* 023E89FC */
BOOL daPy_lk_c::procHookshotSubject_init() {
    if (mCurProc == player02::daPyProc_HOOKSHOT_SUBJECT_e) {
        return FALSE;
    }
    commonProcInit(0x83);
    mNormalSpeed = 0.0f;
    setActAnimeUpper(player02::dRes_INDEX_LKANM_BCK_HOOKSHOTWAIT_e, player02::UPPER_MOVE2_e, 0.0f, 0.0f, -1, -1.0f);
    setSingleMoveAnime(player02::ANM_ATNLS, 0.0f, 0.0f, -1, 2.4f);
    lk_onPlayerStatus0(player02::daPyStts0_HOOKSHOT_AIM_e);
    return TRUE;
}

/* 023E8ADC */
BOOL daPy_lk_c::procHookshotMove_init() {
    if (mCurProc == player02::daPyProc_HOOKSHOT_MOVE_e) {
        return FALSE;
    }
    commonProcInit(0x84);
    if (mDirection == player02::DIR_RIGHT) {
        current.angle.y = (s16)(shape_angle.y - 0x4000);
    } else {
        mDirection = 2; /* DIR_LEFT */
        current.angle.y = (s16)(shape_angle.y + 0x4000);
    }
    u32 hookshot = gabi::ea(mActorKeepEquip.mActor.get());
    if ((u32)gabi::at<fopAc_ac_c>(hookshot)->mParameters == 0) { /* hookshot->checkWait() (HD: the actor's +0xB0) */
        setActAnimeUpper(player02::dRes_INDEX_LKANM_BCK_HOOKSHOTWAIT_e, 2, 1.0f, 0.0f, -1, -1.0f);
        setBlendAtnMoveAnime(2.4f);
    }
    lk_onPlayerStatus0(player02::daPyStts0_HOOKSHOT_AIM_e);
    return TRUE;
}

/* 023E8BD4 */
BOOL daPy_lk_c::checkNextHookshotMode() {
    if ((u16)player02::heaps(m_anm_heap_upper)[2].mIdx != 0xA7) { /* !checkHookshotReadyAnime() */
        m355C = 10;
        m355E = 0;
        seStartOnlyReverb(player02first::JA_SE_LK_ITEM_TAKEOUT);
    }
    if (lk_checkAttentionLock()) {
        return procHookshotMove_init();
    }
    return procHookshotSubject_init();
}

/* HD: entering a boomerang aim proc from another proc clears the HD byte at 0x827C and m355C and sets
 * m355E from the item button */
#define LK_BOOMERANG_ENTER(other) \
    if (mCurProc != (other)) { \
        mHD827C = 0; \
        m355C = 0; \
        m355E = itemButton() == 0; \
    }

/* 023E8CA4 */
BOOL daPy_lk_c::procBoomerangSubject_init() {
    if (mCurProc == player02first::daPyProc_BOOMERANG_SUBJECT_e) {
        return FALSE;
    }
    if (mCurProc != player02first::daPyProc_BOOMERANG_MOVE_e) {
        mHD827C = 0;
        m355C = 0;
        m355E = itemButton() == 0;
    }
    commonProcInit(0x80);
    mNormalSpeed = 0.0f;
    setActAnimeUpper(player02first::dRes_INDEX_LKANM_BCK_BOOMWAIT_e, 2, 0.8f, 0.0f, -1, -1.0f); // HD: rate 0.8
    setSingleMoveAnime(player02::ANM_ATNRS, 0.0f, 0.0f, -1, 2.4f);
    dComIfGp_setPlayerStatus0(0, player02first::daPyStts0_BOOMERANG_AIM_e);
    current.angle.y = shape_angle.y;
    return TRUE;
}

/* 023E8DC8 */
BOOL daPy_lk_c::procBoomerangMove_init() {
    if (mCurProc == player02first::daPyProc_BOOMERANG_MOVE_e) {
        return FALSE;
    }
    if (mCurProc != player02first::daPyProc_BOOMERANG_SUBJECT_e) {
        mHD827C = 0;
        m355C = 0;
        m355E = itemButton() == 0;
    }
    commonProcInit(0x81);
    if (mDirection == player02::DIR_LEFT) {
        current.angle.y = (s16)(shape_angle.y + 0x4000);
    } else {
        mDirection = 3; /* DIR_RIGHT */
        current.angle.y = (s16)(shape_angle.y - 0x4000);
    }
    setActAnimeUpper(player02first::dRes_INDEX_LKANM_BCK_BOOMWAIT_e, 2, 0.8f, 0.0f, -1, -1.0f);
    setBlendAtnMoveAnime(2.4f);
    dComIfGp_setPlayerStatus0(0, player02first::daPyStts0_BOOMERANG_AIM_e);
    return TRUE;
}

/* 023E8F2C */
BOOL daPy_lk_c::checkNextBoomerangMode() {
    if (!checkBoomerangAnime()) {
        seStartOnlyReverb(player02first::JA_SE_LK_ITEM_TAKEOUT);
    }
    if (lk_checkAttentionLock()) {
        return procBoomerangMove_init();
    }
    return procBoomerangSubject_init();
}

/* 023E8FBC */
BOOL daPy_lk_c::procRopeSubject_init() {
    if (mCurProc == player02first::daPyProc_ROPE_SUBJECT_e) {
        return FALSE;
    }
    commonProcInit(0x76);
    mNormalSpeed = 0.0f;
    if (player02::actorParameters(mActorKeepEquip.mActor.get()) == 0) { /* fopAcM_GetParam(mActorKeepEquip.getActor()) */
        setActAnimeUpper(player02first::dRes_INDEX_LKANM_BCK_ROPETHROWWAIT_e, 2, 0.0f, 0.0f, -1, -1.0f);
        setSingleMoveAnime(player02::ANM_ATNRS, 0.0f, 0.0f, -1, 3.0f); // HD: HIO folded
    }
    dComIfGp_setPlayerStatus0(0, player02first::daPyStts0_ROPE_AIM_e);
    current.angle.y = shape_angle.y;
    player02SystemSound(player02first::JA_SE_CAMERA_L_MOVE);
    mProcVar6 = 0;
    m3600 = -1.0f;
    m3604 = -1.0f;
    return TRUE;
}

/* 023E90EC */
BOOL daPy_lk_c::procRopeMove_init() {
    if (mCurProc == player02first::daPyProc_ROPE_MOVE_e) {
        return FALSE;
    }
    commonProcInit(0x7D);
    if (mDirection == player02::DIR_LEFT) {
        current.angle.y = (s16)(shape_angle.y + 0x4000);
    } else {
        mDirection = 3; /* DIR_RIGHT */
        current.angle.y = (s16)(shape_angle.y - 0x4000);
    }
    if (player02::actorParameters(mActorKeepEquip.mActor.get()) == 0) {
        setActAnimeUpper(player02first::dRes_INDEX_LKANM_BCK_ROPETHROWWAIT_e, 2, 1.0f, 0.0f, -1, -1.0f);
        setBlendAtnMoveAnime(3.0f);
    }
    dComIfGp_setPlayerStatus0(0, player02first::daPyStts0_ROPE_AIM_e);
    return TRUE;
}

/* 023E91E4 */
BOOL daPy_lk_c::checkNextRopeMode() {
    /* virtual checkRopeReadyAnime() (vtable slot 0xDC) */
    if (!gabi::call_ptr<BOOL>((u32)gabi::at<player02::readyVtable_c>(__vtbl)->mCheckRopeReadyAnime, this) && player02::actorParameters(mActorKeepEquip.mActor.get()) == 0) {
        m355C = 10;
        m355E = 0;
        seStartOnlyReverb(player02first::JA_SE_LK_ITEM_TAKEOUT);
    }
    if (lk_checkAttentionLock()) {
        return procRopeMove_init();
    }
    return procRopeSubject_init();
}

/* mDoExt_AnmRatioPack::setRatio(0.0f) (HD: a counted array, count +8 and pointer +0xC, both reloaded per entry) */
static inline void lk_anmRatio_clear(u32 pack, f32 r) {
    player02::anmRatioPack_c* ratios = gabi::at<player02::anmRatioPack_c>(pack);
    for (s32 i = 0; i < ratios->mCount; i++) {
        player02::indexed(ratios->mpRatio.get(), (u32)i) = r;
    }
}

/* 023E92A0 */
BOOL daPy_lk_c::procTactWait_init(int i_mode) {
    if (mCurProc == player02first::daPyProc_TACT_WAIT_e) {
        if ((u32)mProcVar6 != (u32)i_mode) {
            mProcVar4 = 0;
            mProcVar5 = 0;
            mProcVar1 = -1;
        }
        mProcVar6 = i_mode;
        gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mMetronome = i_mode != -4; /* dComIfGp_setMetronomeOn() / Off() */
        return TRUE;
    }
    if (i_mode == -1) {
        /* dComIfGp_event_compulsory(this, NULL, -1) */
        if (!player02EventCompulsory(dComIfGp_ea() + 0x51D0, this, 0, 0xFFFF)) {
            return FALSE;
        }
        mDemo.mDemoType = 5; /* mDemo.setSpecialDemoType() */
    }
    int shipItemMode = checkShipRideUseItem(1);
    if (shipItemMode == 0) {
        if (i_mode == -1) {
            u32 attn = gabi::ea(&attention_info.position); /* attention_info.position */
            gabi::Local<cXyz> startPos;
            gabi::Local<cXyz> endPos;
            f32 y = player02first::positionY(attn) - 50.0f;
            f32 forwardX = 200.0f * cM_ssin(shape_angle.y);
            f32 forwardZ = 200.0f * cM_scos(shape_angle.y);
            fcpy_l(startPos.a + 0, attn + 0);
            startPos->y = y;
            fcpy_l(startPos.a + 8, attn + 8);
            endPos->y = y;
            endPos->x = player02first::positionX(attn) + forwardX;
            endPos->z = player02first::positionZ(attn) + forwardZ;
            u32 chk = gabi::ea(mLinkLinChk); /* mLinkLinChk */
            player02SetLine( chk, startPos.a, endPos.a, this);
            if (lk_LineCross(chk)) {
                endPos->x = startPos->x - forwardX;
                endPos->z = startPos->z - forwardZ;
                player02SetLine( chk, startPos.a, endPos.a, this);
                if (!lk_LineCross(chk)) {
                    shape_angle.y = (s16)(shape_angle.y - 0x8000);
                    current.angle.y = shape_angle.y;
                } else {
                    endPos->x = startPos->x + forwardZ;
                    endPos->z = startPos->z - forwardX;
                    player02SetLine( chk, startPos.a, endPos.a, this);
                    if (!lk_LineCross(chk)) {
                        shape_angle.y = (s16)(shape_angle.y + 0x4000);
                        current.angle.y = shape_angle.y;
                    } else {
                        endPos->x = startPos->x - forwardZ;
                        endPos->z = startPos->z + forwardX;
                        player02SetLine( chk, startPos.a, endPos.a, this);
                        BOOL cross = lk_LineCross(chk);
                        s16 a = shape_angle.y;
                        if (!cross) {
                            a = (s16)(a - 0x4000);
                            shape_angle.y = a;
                        }
                        current.angle.y = a;
                    }
                }
            }
        } else if (i_mode == -4 || i_mode == -5) {
            if (player02TalkPartner(this) != 0) {
                fopAc_ac_c* partner = gabi::at<fopAc_ac_c>(player02TalkPartner(this));
                shape_angle.y = fopAcM_searchActorAngleY(this, partner);
            }
        }
    }
    commonProcInit(0x9A);
    player02_cpp::m_maba_flg = 0; /* daPy_matAnm_c::offMabaFlg() */
    player02_cpp::m_maba_timer = 1;
    setSingleMoveAnime(player02first::ANM_WAITTAKT, 0.8f, 0.0f, -1, 6.0f); // HD: HIO folded
    setActAnimeUpper(player02first::dRes_INDEX_LKANM_BCK_WAITTAKT_e, 1 /* UPPER_MOVE1_e */, 0.8f, 0.0f, -1, -1.0f);
    lk_anmRatio_clear(gabi::ea(mAnmRatioUpper) + sizeof(player02::anmRatioPack_c), 0.0f); /* mAnmRatioUpper[UPPER_MOVE1_e].setRatio(0.0f) */
    setActAnimeUpper(0x127, 2 /* UPPER_MOVE2_e */, 0.8f, 0.0f, -1, -1.0f);
    lk_anmRatio_clear(gabi::ea(mAnmRatioUpper) + 2 * sizeof(player02::anmRatioPack_c), 0.0f); /* mAnmRatioUpper[UPPER_MOVE2_e].setRatio(0.0f) */
    mProcVar2 = 0;
    mProcVar3 = 0;
    mNormalSpeed = 0.0f;
    dComIfGp_setPlayerStatus1(0, player02first::daPyStts1_WIND_WAKER_CONDUCT_e);
    if (i_mode == -1) {
        u32 cam = player02CameraBody();
        player02StartEventCamera(cam, 0xC, player02first::actorID(gabi::ea(this)) /* fopAcM_GetID(this) */, 0);
        player02SystemSound(player02first::JA_SE_TAKT_USE_BEGIN);
    }
    player02TactReset();
    player02TactSetBeat(0);
    player02TactSetVolume(0.0f);
    player02TactAmbientPlay();
    setTactModel();
    initShipRideUseItem(shipItemMode, 2);
    mProcVar1 = shipItemMode != 0 ? -1 : -30; // HD: -30 when not on the ship
    u32 model = gabi::ea(player02::equipment(this)->mpEquipItemModel.get()); /* mpEquipItemModel->getBaseTRMtx() */
    player02MakeEmitter(gabi::ea(&m32E4), 0x50 /* ID_AK_JN_TAKT00 */, model ? model + 0xC8 : 0, gabi::ea(&current.pos), 0);
    mProcVar6 = i_mode;
    m35A0 = 0.0f;
    mProcVar5 = 0;
    mProcVar7 = -1;
    mProcVar4 = 0;
    m3624 = 0;
    m35A4 = 0.0f;
    m35A8 = 0.0f;
    mProcVar0 = -1;
    if (i_mode == 6 || i_mode == 7) {
        m35AC = 600.0f;
    } else {
        m35AC = 900.0f;
    }
    gabi::call_ptr((u32)gabi::at<player02::readyVtable_c>(__vtbl)->mSetTactZev, this, -1, -1, 0); /* virtual setTactZev(-1, -1, NULL) */
    if (mProcVar6 != -4) {
        gabi::at<dComIfG_inf_c>(dComIfGp_ea())->play.mMetronome = 1; /* dComIfGp_setMetronomeOn() */
    }
    player02TaktModeMute();
    return TRUE;
}

/* 023E99D0 */
int daPy_lk_c::setHintActor() {
    /* dComIfGp_att_getZHint(): dAttCatch_c::convPId(player02first::hintCatchAddress(play), the Z-hint process id at play + 0x5930) */
    u32 play = dComIfGp_ea();
    if (player02HintActor(player02first::hintCatchAddress(play), player02first::hintProcessID(play)) != 0 &&
        !dComIfGp_event_runCheck() /* !dComIfGp_event_runCheck() */ &&
        mDemo.getDemoType() == 0 /* !checkPlayerDemoMode() */) {
        player02DoButtonQuake(this);
        setResetFlg0(resetFlg0() | 0x100); /* onResetFlg0(daPyRFlg0_UNK100) */
        dComIfGp_setDoStatus(0x31); /* dComIfGp_setDoStatus(dActStts_ba_sake__dupe_31) */
        return TRUE;
    }
    return FALSE;
}

/* 023E9A6C */
BOOL daPy_lk_c::checkDrinkBottleItem(int itemNo) {
    return (u32)(itemNo - 0x51) <= 4;
}

/* 023E9A84 */
BOOL daPy_lk_c::checkOpenBottleItem(int itemNo) {
    return (u32)(itemNo - 0x56) <= 3;
}

/* 023E9A9C */
BOOL daPy_lk_c::checkGroupItem(int itemGroup, int itemNo) {
    if (itemGroup == player02first::daPyItem_DRINK_BOTTLE_e) {
        return checkDrinkBottleItem(itemNo);
    } else if (itemGroup == player02first::daPyItem_OPEN_BOTTLE_e) {
        return checkOpenBottleItem(itemNo);
    } else if (itemGroup == player02first::daPyItem_ESA_e) {
        return player02IsEsa((u32)(u8)itemNo);
    } else if (itemGroup == player02first::daPyItem_BOW_e) {
        return checkBowItem(itemNo);
    } else if (itemGroup == player02first::daPyItem_PHOTOBOX_e) {
        return checkPhotoBoxItem(itemNo);
    }
    return itemGroup == itemNo;
}

/* 023E9AF4 */
BOOL daPy_lk_c::checkSetItemTrigger(int itemGroup, int checkCurse) {
    if (checkCurse != 0 && (u16)player02_cpp::m_damageType == 1) { /* daPy_dmEcallBack_c::checkCurse() (HD: a global) */
        return FALSE;
    }
    // HD: the X/Y/Z(/fourth) item triggers only while the HD word at 0x8260 is 0; it is re-read after each check
    if (mHD8260 == 0) {
        do {
            if (mItemTrigger & 4) { /* itemTriggerX() */
                if (checkGroupItem(itemGroup, dComIfGp_getSelectItem(0) /* dComIfGp_getSelectItem(dItemBtn_X_e) */)) {
                    mReadyItemBtn = 0;
                    return TRUE;
                }
                if (mHD8260 != 0) break;
            }
            if (mItemTrigger & 8) { /* itemTriggerY() */
                if (checkGroupItem(itemGroup, dComIfGp_getSelectItem(1))) {
                    mReadyItemBtn = 1;
                    return TRUE;
                }
                if (mHD8260 != 0) break;
            }
            if (mItemTrigger & 0x10) { /* itemTriggerZ() */
                if (checkGroupItem(itemGroup, dComIfGp_getSelectItem(2))) {
                    mReadyItemBtn = 2;
                    return TRUE;
                }
                if (mHD8260 != 0) break;
            }
            if (mItemTrigger & 0x80) { // HD: a fourth item button
                if (checkGroupItem(itemGroup, dComIfGp_getSelectItem(3))) {
                    mReadyItemBtn = 3;
                    return TRUE;
                }
            }
        } while (0);
    }
    // HD: on the ship (normal mode) the do button selects the item of a fifth slot
    if (checkShipRideUseItem(0) == 1 && !dComIfGp_checkPlayerStatus1(0, 2) && !dComIfGp_checkPlayerStatus1(0, 4) && !dComIfGp_checkPlayerStatus1(0, 0x400) &&
        (mItemTrigger & 1) && checkGroupItem(itemGroup, dComIfGp_getSelectItem(4))) {
        mReadyItemBtn = 4;
        return TRUE;
    }
    return FALSE;
}

/* 023E9D50 */
BOOL daPy_lk_c::changeDragonShield(int playErrorSound) {
    u32 flg = noResetFlg1();
    if (flg & 1) { /* checkNoResetFlg1(daPyFlg1_EQUIP_DRAGON_SHIELD) */
        setNoResetFlg1(flg & ~1u);
        return TRUE;
    }
    if (dComIfGs_getRupee() != 0) { // HD: Magic Armor uses rupees instead of the GameCube magic meter.
        setNoResetFlg1(flg | 1);
        m3548 = 60; // HD: HIO folded
        mTinkleShieldTimer = 0;
        return TRUE; // HD: no dComIfGp_setItemMagicCount(-1)
    }
    if (playErrorSound != 0) {
        player02SystemSound(player02first::JA_SE_ITEM_TARGET_OUT);
    }
    return TRUE;
}

/* 023E9DD0 */
BOOL daPy_lk_c::procBootsEquip_init(u16 itemNo) {
    commonProcInit(player02first::daPyProc_BOOTS_EQUIP_e);
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    setSingleMoveAnime(player02first::ANM_SETBOOTS, 1.0f, 0.0f, 0x13, 5.0f); // HD: HIO folded
    mProcVar6 = itemNo;
    return TRUE;
}

/* 023E9E48 */
BOOL daPy_lk_c::procGrabThrow_init(int throwType) {
    if (mCurProc == player02first::daPyProc_GRAB_THROW_e) {
        return FALSE;
    }
    commonProcInit(0x71);
    // HD: HIO folded
    if (mActorKeepGrab.mActor.get()->actor_status & 0x10000) { /* fopAcM_CheckStatus(.., fopAcStts_UNK10000_e) */
        setSingleMoveAnime(player02first::ANM_GRABRE, 1.1f, 3.0f, 0xC, 2.0f);
        mProcVar7 = 0;
    } else {
        setSingleMoveAnime(player02first::ANM_GRABTHROW, 0.8f, 1.0f, 0xE, 0.0f);
        mProcVar7 = 1;
    }
    fcpy_l(gabi::ea(&m35A0), gabi::ea(&mNormalSpeed)); /* m35A0 = mNormalSpeed */
    dComIfGp_clearPlayerStatus1(0, player02first::daPyStts1_UNK40000_e);
    mProcVar6 = throwType;
    return TRUE;
}

/* 023E9F60 */
void daPy_lk_c::setAnimeEquipSword(BOOL keepUnequipFlag) {
    if (!LK_checkSwordEquip()) {
        return;
    }
    m3562 = 0x103; /* daPyItem_SWORD_e */
    setActAnimeUpper(player02first::dRes_INDEX_LKANM_BCK_REST_e, 2 /* UPPER_MOVE2_e */, -1.8f, 1.0f, 0xE, 1.0f); // HD: HIO folded
    setPriTextureAnime(0x72, 0);
    dComIfGp_clearPlayerStatus0(0, player02first::daPyStts0_BOOMERANG_WAIT_e);
    u32 boomerang = gabi::ea(mActorKeepThrow.mActor.get());
    if (boomerang != 0) {
        gabi::at<player02::boomerangCancel_c>(boomerang)->mCancelFlg = 1; /* daBoomerang_c::onCancelFlg() */
    }
    if (!keepUnequipFlag) {
        setNoResetFlg1(noResetFlg1() | 0x4000000); /* onNoResetFlg1(daPyFlg1_UNK4000000) */
    } else {
        setNoResetFlg1(noResetFlg1() & ~0x4000000u);
    }
}

/* 023EA054 */
BOOL daPy_lk_c::procGrabPut_init() {
    if (mCurProc == player02first::daPyProc_GRAB_PUT_e) {
        return FALSE;
    }
    commonProcInit(0x72);
    if (field_0x2b0 < 0.0f) { /* checkGrabWear() (daPy_py_c, GameCube 0x2B0) */
        mProcVar6 = 1;
        setSingleMoveAnime(player02::ANM_GRABWAIT, 1.0f, 0.0f, -1, 3.0f); // HD: HIO folded
    } else {
        mProcVar6 = 0;
        setSingleMoveAnime(player02first::ANM_GRABUP, -1.1f, 0.0f, 7, 0.0f);
    }
    mNormalSpeed = 0.0f;
    u32 grab_actor = gabi::ea(mActorKeepGrab.mActor.get());
    u32 src = grab_actor != 0 ? grab_actor + 0x314 : gabi::ea(&current.pos);
    player02::copyWordX(&m370C, gabi::at<cXyz>(src)); /* m370C = ..->current.pos (word copy) */
    player02::copyWordY(&m370C, gabi::at<cXyz>(src));
    player02::copyWordZ(&m370C, gabi::at<cXyz>(src));
    setResetFlg0(resetFlg0() | 0x400000); /* onResetFlg0(daPyRFlg0_GRAB_PUT_START) */
    return TRUE;
}

/* 023EA1AC */
BOOL daPy_lk_c::checkNextActionGrab() {
    u32 grab_actor = gabi::ea(mActorKeepGrab.mActor.get());
    if (grab_actor == 0) {
        return FALSE;
    }
    int barrelSearch = checkGrabBarrelSearch(0);
    if ((player02first::actorStatus(grab_actor) & 0x10000) && !checkGrabSpecialHeavyState()) {
        dComIfGp_setRStatus(0xE);  /* dActStts_THROW_e */
        dComIfGp_setDoStatus(0xE);
    } else {
        dComIfGp_setRStatus(9);    /* dActStts_DROP_e */
    }
    if (barrelSearch != 0) {
        dComIfGp_setDoStatus(9);   /* dActStts_DROP_e */
    } else if (mpAttnActorLockOn.get() == nullptr && mpAttnEntryA != 0 && player02first::attentionType(mpAttnEntryA) == fopAc_Attn_TYPE_DOOR_e &&
               (player02first::actorStatus(grab_actor) & fopAcStts_UNK2000000_e)) {
        dComIfGp_setDoStatus(0xB); /* dActStts_OPEN_e */
    } else if (lk_checkAttentionLock()) {
        int direction = getDirectionFromShapeAngle();
        if (!(mStickDistance > 0.05f) || direction == player02::DIR_FORWARD || direction == player02::DIR_BACKWARD) {
            dComIfGp_setDoStatus(0xE);
        }
    } else {
        dComIfGp_setDoStatus(0xE);
    }
    if (setHintActor() && LK_doTrigger()) {
        u32 play = dComIfGp_ea();
        u32 hint = player02HintActor(player02first::hintCatchAddress(play), player02first::hintProcessID(play)); /* dComIfGp_att_getZHint() */
        player02OrderZHint(this, hint);
        return TRUE;
    }
    if (checkSetItemTrigger(player02first::dItemNo_MAGIC_ARMOR_e, 1) || ((noResetFlg1() & 1) && checkSetItemTrigger(0x2A, 0))) {
        changeDragonShield(1);
        return TRUE;
    }
    if (checkSetItemTrigger(player02first::dItemNo_IRON_BOOTS_e, 1) || ((mNoResetFlg0 & 0x2000000) /* checkEquipHeavyBoots() */ && checkSetItemTrigger(0x29, 0))) {
        return procBootsEquip_init(0x29);
    }
    if (fpcM_GetName(gabi::at<fopAc_ac_c>(grab_actor)) == player02first::fpcNm_BOMB_e && checkSetItemTrigger(player02first::dItemNo_BOMB_BAG_e, 0)) {
        return procGrabThrow_init(0);
    }
    if ((mItemTrigger & 2) /* cancelTrigger() */ && LK_checkSwordEquip() && barrelSearch == 0) {
        if (dComIfGp_getRStatus() == 0xE) {
            return procGrabThrow_init(1);
        }
        freeGrabItem();
        setAnimeEquipSword(0);
        return TRUE;
    }
    if (LK_doTrigger()) {
        bool checkThrow = true;
        if (!(mNoResetFlg0 & 0xA0000000u)) {
            if (dComIfGp_getDoStatus() == player02first::dActStts_OPEN_e) {
                player02OrderDoor(this, mpAttnActorA.get());
                return TRUE;
            }
            // HD: status queries may change the trigger; sample it again before checking THROW.
            checkThrow = LK_doTrigger() != 0;
        }
        if (checkThrow && dComIfGp_getDoStatus() == 0xE) {
            return procGrabThrow_init((mItemTrigger & 2) ? 1 : 0);
        }
    }
    {
        u8 rstatus = dComIfGp_getRStatus();
        u8 t = mItemTrigger;
        if (rstatus == 0xE && ((t & 0x1F) || (t & 0x40))) { /* allTrigger() || spActionTrigger() */
            return procGrabThrow_init((t & 2) ? 1 : 0);
        }
        if (!(t & 0x1F) && !(t & 0x40)) {
            return FALSE;
        }
        if ((t & 1) && dComIfGp_getDoStatus() == player02first::dActStts_BLANK_e) {
            return FALSE;
        }
    }
    return procGrabPut_init();
}

/* 023EAA80 */
int daPy_lk_c::getReadyItem() {
    u8 btn = mReadyItemBtn;
    if (btn == 0 /* dItemBtn_X_e */) {
        return dComIfGp_getSelectItem(0); /* dComIfGp_getSelectItem(dItemBtn_X_e) */
    } else if (btn == 1 /* dItemBtn_Y_e */) {
        return dComIfGp_getSelectItem(1);
    } else if (btn == 2 /* dItemBtn_Z_e */) {
        return dComIfGp_getSelectItem(2);
    }
    // HD: the fourth item button and the ship's do-button item slot.
    return dComIfGp_getSelectItem(btn == 3 ? 3 : 4);
}

/* 023EAB40 */
BOOL daPy_lk_c::itemTrigger() {
    if (mHD8260 != 0) { /* HD */
        return FALSE;
    }
    u8 btn = mReadyItemBtn;
    u8 trig = mItemTrigger;
    if (btn == 3) { // HD: the fourth button
        return trig & 0x80;
    } else if (btn == 0 /* dItemBtn_X_e */) {
        return trig & 4; /* itemTriggerX() */
    } else if (btn == 1 /* dItemBtn_Y_e */) {
        return trig & 8; /* itemTriggerY() */
    }
    return trig & 0x10; /* itemTriggerZ() */
}

/* 023EAB98 */
BOOL daPy_lk_c::procHookshotFly_init() {
    fopAc_ac_c* hookshot = mActorKeepEquip.mActor.get();
    commonProcInit(player02::daPyProc_HOOKSHOT_FLY_e);
    for (int i = 0; i < 3; i++) {
        player02SetWallRadius(&mAcchCir[i], 70.0f);
    }
    mAcchCir[0].m_wall_h = -25.0f; /* SetWallH */
    mAcchCir[1].m_wall_h = 0.0f;
    mAcchCir[2].m_wall_h = 50.0f;
    setSingleMoveAnime(player02first::ANM_HOOKSHOTJMP, 1.0f, 0.0f, -1, 0.0f);
    mNormalSpeed = 0.0f;
    mBodyAngle.x = 0; /* mBodyAngle.x */
    gravity = 0.0f;
    m34C2 = 11;
    dComIfGp_setPlayerStatus1(0, player02first::daPyStts1_UNK10_e);
    player02::voiceStart(this, 6);
    player02::copyWordY(&m370C, &old.pos); /* m370C = old.pos (word copy) */
    player02::copyWordZ(&m370C, &old.pos);
    player02::copyWordX(&m370C, &old.pos);
    shape_angle.x = hookshot->shape_angle.x;
    shape_angle.y = hookshot->shape_angle.y;
    return TRUE;
}

/* 023EACD4 */
BOOL daPy_lk_c::cancelItemUpperReadyAnime() {
    if (dComIfGp_getDoStatus() == player02first::dActStts_RETURN_e && LK_doTrigger()) {
        seStartOnlyReverb(player02first::JA_SE_CANCEL_1);
        resetActAnimeUpper(2 /* UPPER_MOVE2_e */, -1.0f);
        procWait_init();
        return TRUE;
    }
    return FALSE;
}

/* 023EB064 */
BOOL daPy_lk_c::bowButton() {
    if (mDemo.getDemoMode() == 0x44) { /* checkBowMiniGame() */
        return mItemButton & 0x10; // HD: doButton() reads 0x10 here
    }
    return itemButton();
}

/* 023EB080 */
void daPy_lk_c::setBowReloadAnime() {
    setActAnimeUpper(player02first::dRes_INDEX_LKANM_BCK_ARROWRELORD_e, 2, 1.0f, 0.0f, 6, 1.0f); // HD: HIO folded
    setPriTextureAnime(0x8D, 0);
    LK_swordAnim_changeBckOnly(getItemAnimeResource(player02first::dRes_INDEX_LKANM_BCK_ARROWRELORDA_e));
    m35EC = 0.0f;
    seStartOnlyReverb(player02first::JA_SE_LK_DRAW_BOW);
}

/* dComIfGp_event_getPt1(): dEvt_control_c::convPId(play + 0x51D0, play + 0x5294) */
static inline u32 player02FirstEventActor() {
    u32 play = dComIfGp_ea();
    return player02FirstEventPID(play + 0x51D0, gabi::load<u32>(play + 0x5294));
}

/* 023EB120 */
void daPy_lk_c::makeArrow() {
    BOOL make;
    if (mDemo.getDemoMode() != 0x44) { /* !checkBowMiniGame() */
        make = dComIfGs_getArrowNum() != 0; /* dComIfGs_getArrowNum() != 0 */
    } else {
        make = player02FirstEventActor() != 0 && fpcM_GetName(gabi::at<fopAc_ac_c>(player02FirstEventActor())) == player02first::fpcNm_NPC_SO_e &&
               (s32)(10u - player02first::miniGameArrowCount(player02FirstEventActor())) > 0; /* daNpc_So_c::getMiniGameRestArrow() > 0 (wrapping) */
    }
    if (make) {
        fopAc_ac_c* arrow_p = player02CreateArrow(player02first::fpcNm_ARROW_e, 0, &current.pos,
                                                      (s32)current.roomNo, nullptr, nullptr, -1, 0, 0);
        player02KeepSetData(&mActorKeepEquip, arrow_p);
    }
    m355C = 0;
}

/* mEquipItem == getReadyItem(), the field read after the call */
#define lk_equipIsReady() lk_equipIsReady_l(this)
static inline BOOL lk_equipIsReady_l(daPy_lk_c* t) {
    int ready = t->getReadyItem();
    return (u32)t->mEquipItem == (u32)ready;
}

/* 023EAD5C */
BOOL daPy_lk_c::checkNextActionHookshotReady() {
    player02::initializeScale(player02_cpp::eff_scale_guard, player02_cpp::eff_scale, 1.0f, 1.0f, 0.0f); /* eff_scale */
    player02::initializeScale(player02_cpp::eff_dscale_guard, player02_cpp::eff_dscale, 0.2f, 0.2f, 0.2f); /* eff_dscale */
    player02::initializeScale(player02_cpp::eff_pscale_guard, player02_cpp::eff_pscale, 0.4f, 0.4f, 0.4f); /* eff_pscale */
    u32 hookshot = gabi::ea(mActorKeepEquip.mActor.get());
    u32 state = (u32)gabi::at<fopAc_ac_c>(hookshot)->mParameters; /* daHookshot_c: 0 wait, 1 shot, 2 return, 3 pull (HD: +0xB0) */
    if (state == 0) { /* hookshot->checkWait() */
        BOOL shoot = FALSE;
        if (itemButton()) {
            int ready = getReadyItem();
            if ((u32)mEquipItem == (u32)ready && m355C == 0) {
                m355E = 1;
            } else if (!itemButton() && m355E != 0) {
                shoot = TRUE;
            }
        } else if (m355E != 0) {
            shoot = TRUE;
        }
        if (shoot) {
            m355E = 0;
            player02::setActorParameters(gabi::at<fopAc_ac_c>(hookshot), 1); /* hookshot->setShot() */
            mNormalSpeed = 0.0f;
            player02_cpp::m_maba_flg = 0; /* daPy_matAnm_c::offMabaFlg() */
            player02_cpp::m_maba_timer = 1;
            seStartOnlyReverb(player02first::JA_SE_LK_HS_SHOOT);
            player02::voiceStart(this, 41);
            resetFootEffect();
            if (mDirection != player02::DIR_LEFT) {
                mDirection = 2;
            }
            current.angle.y = (s16)(shape_angle.y + 0x4000);
            mModeFlg = mModeFlg | 1; /* onModeFlg(ModeFlg_00000001) */
            m3598 = 0.0f;
            setBlendAtnMoveAnime(2.4f);
            mFrameCtrlUnder[0].setRate(0.0f);
            mFrameCtrlUnder[1].setRate(0.0f);
            mFrameCtrlUpper[0].setRate(1.25f); // HD: HIO folded
            mFrameCtrlUpper[1].setRate(0.0f);
            mFrameCtrlUpper[2].setRate(0.0f);
            return TRUE;
        }
    } else if (state == 1) { /* hookshot->checkShot() */
        if (itemTrigger() && lk_equipIsReady()) {
            player02::setActorParameters(gabi::at<fopAc_ac_c>(hookshot), 2); /* hookshot->setReturn() */
        }
        return TRUE;
    } else if (state == 2) { /* hookshot->checkReturn() */
        return TRUE;
    } else if (state == 3) { /* hookshot->checkPull() */
        procHookshotFly_init();
        return TRUE;
    }
    if (m355C > 0) {
        m355C = (s16)(m355C - 1);
    }
    return cancelItemUpperReadyAnime();
}

/* 023EB208 */
BOOL daPy_lk_c::checkNextActionBowReady() {
    u16 upper = (u16)player02::heaps(m_anm_heap_upper)[2].mIdx;
    if (upper == 0x36) { /* checkBowWaitAnime() */
        s16 shot = m355C;
        u32 arrow = gabi::ea(mActorKeepEquip.mActor.get());
        if (shot != 0) {
            // HD: HIO folded
            setActAnimeUpper(player02::dRes_INDEX_LKANM_BCK_ARROWSHOOT_e, 2, 0.9f, 0.0f, 4, 0.0f);
            setPriTextureAnime(0x70, 0);
            LK_swordAnim_changeBckOnly(getItemAnimeResource(player02::dRes_INDEX_LKANM_BCK_ARROWSHOOTA_e));
            m35EC = 0.0f;
        } else if (!bowButton()) {
            if (arrow != 0) {
                player02::setActorParameters(gabi::at<fopAc_ac_c>(arrow), 1); /* fopAcM_SetParam(arrow, 1) */
                if (mDemo.getDemoMode() != 0x44) { /* !checkBowMiniGame() */
                    dComIfGp_setItemArrowNumCount(-1);
                }
                player02KeepClearData(&mActorKeepEquip);
                seStartOnlyReverb(player02first::JA_SE_LK_SHOOT_ARROW);
                player02::voiceStart(this, 41);
                setResetFlg0(resetFlg0() | 0x20000000); /* onResetFlg0(daPyRFlg0_ARROW_SHOOT) */
            }
            m355C = 1;
        } else if (player02ChangeArrowMp()) {
            if (mEquipItem != player02first::dItemNo_BOW_e) {
                dComIfGp_setRStatus(0x22); /* dActStts_SWAP_MODES_e */
                if ((mItemTrigger & 0x40) /* spActionTrigger() */ && arrow != 0) {
                    u32 na = player02ChangeArrowType(arrow);
                    player02KeepSetData(&mActorKeepEquip, gabi::at<fopAc_ac_c>(na));
                    setBowReloadAnime();
                }
            }
        }
    } else if (upper == 0xC) { /* checkBowReloadAnime() */
        if (mFrameCtrlUpper[2].getRate() < 0.01f) {
            setActAnimeUpper(player02first::dRes_INDEX_LKANM_BCK_BOWWAIT_e, 2, 1.0f, 0.0f, -1, -1.0f);
            LK_swordAnim_changeBckOnly(getItemAnimeResource(player02first::dRes_INDEX_LKANM_BCK_BOWWAITA_e));
            m35EC = 0.0f;
            setPriTextureAnime(0x70, 0);
        }
    } else if (upper == 0xE) { /* checkBowShootAnime() */
        if (mFrameCtrlUpper[2].getRate() < 0.01f) {
            if (m355E == 0 && bowButton() && (mDemo.getDemoMode() == 0x44 || lk_equipIsReady()) &&
                !(noResetFlg1() & 0x2000) /* !checkUseArrowEffect() */) {
                setBowReloadAnime();
                makeArrow();
                if (mActorKeepEquip.mActor.get() == nullptr) {
                    player02SystemSound(player02first::JA_SE_ITEM_TARGET_OUT);
                }
            } else if (player02ChangeArrowMp()) {
                if (mEquipItem != player02first::dItemNo_BOW_e) {
                    dComIfGp_setRStatus(0x22);
                    if (mItemTrigger & 0x40) {
                        player02ChangeArrowTypeNotReady();
                    }
                }
            }
        }
        if (m355E != 0) {
            m355E = (s16)(m355E - 1);
        }
    }
    return cancelItemUpperReadyAnime();
}

/* 023EB510 */
BOOL daPy_lk_c::checkNextActionBoomerangReady() {
    /* HD: the boomerang is thrown when its button is released (pad release bits *(0x101F5088) + 0x1C), or after
     * holding it for 12 frames; the GameCube !itemButton() test is gone */
    u8 btn = mReadyItemBtn;
    u32 rel = player02::padReleaseWord();
    BOOL released;
    if (btn == 0) {
        released = (rel & 8) != 0;
    } else if (btn == 1) {
        released = (rel & 0x10) != 0;
    } else {
        released = (rel & 0x4000) != 0;
    }
    BOOL throwIt = FALSE;
    if (released) {
        if (m355E != 0 || m355C >= 0xC) {
            mHD827C = 1;
            m355C = 0;
            throwIt = TRUE;
        } else {
            m355E = 1;
            m355C = 0;
            throwIt = mHD827C != 0;
        }
    } else {
        if (itemButton()) {
            m355C = (s16)(m355C + 1);
        }
        throwIt = mHD827C != 0;
    }
    if (throwIt) {
        if (mpAttnActorLockOn.get() == nullptr) {
            s32 idx = mCameraInfoIdx;
            /* dComIfGp_checkCameraAttentionStatus(mCameraInfoIdx, dCamAttnStts_00000010_e) */
            if (!dComIfGp_checkCameraAttentionStatus(idx, 0x10)) {
                return cancelItemUpperReadyAnime();
            }
        }
        // HD: HIO folded
        setActAnimeUpper(player02first::dRes_INDEX_LKANM_BCK_BOOMTHROW_e, 2, 1.5f, 2.0f, 10, 3.0f);
        setPriTextureAnime(0x48, 0);
        mHD827C = 0;
        mHDAngleY = shape_angle.y;
        return TRUE;
    }
    return cancelItemUpperReadyAnime();
}

// HD: SafeString == of a C string and a buffer (the C string's termination is assured twice, as compiled)
static inline BOOL lk_strEq2(gabi::Local<SafeString>& a, gabi::Local<SafeString>& b, u32 str, u32 buf) {
    a->__vtbl = gc_tu.safeString;
    a->mStringTop = str;
    b->__vtbl = gc_tu.safeString;
    b->mStringTop = buf;
    player02::assureTermination(a);
    player02::assureTermination(a);
    u32 s1 = a->mStringTop;
    player02::assureTermination(b);
    if (s1 == b->mStringTop) {
        return TRUE;
    }
    return lk_safeStringEq(a->mStringTop, b->mStringTop);
}

/* 023EB6C8 */
BOOL daPy_lk_c::checkBossGomaStage() {
    gabi::Local<SafeString> a, b, c, d;
    /* dComIfGp_getStartStageName(): play + 0x5134 */
    if (lk_strEq2(a, b, gabi::guest_addr("M_DragB"), dComIfGp_ea() + 0x5134)) {
        return TRUE;
    }
    return lk_strEq2(c, d, gabi::guest_addr("Xboss0"), dComIfGp_ea() + 0x5134);
}

/* 023EB864 */
BOOL daPy_lk_c::procRopeReady_init() {
    fopAc_ac_c* rope = mActorKeepEquip.mActor.get();
    commonProcInit(player02first::daPyProc_ROPE_READY_e);
    setMoveAnime(0.5f, 1.0f, 1.0f, player02first::ANM_ROPECATCH, player02first::ANM_ROPESWINGB, 7, 3.0f); // HD: HIO folded
    u32 anm = (u32)gabi::at<player02::anmRatioPack_c>(gabi::ea(mAnmRatioUnder))[0].mpAnmTransform; /* mAnmRatioUnder[UNDER_MOVE0_e].getAnmTransform() */
    mFrameCtrlUnder[0].mStart = 0;
    mFrameCtrlUnder[0].mEnd = 7;
    mFrameCtrlUnder[0].mFrame = 0.0f; // HD: the frame is set to the start (0) directly
    gabi::at<player02::animation_c>(anm)->mFrame = 0.0f;       /* ->setFrame() */
    fcpy_l(gabi::ea(&m3688.x), gabi::ea(&current.pos.x)); /* m3688 = current.pos */
    fcpy_l(gabi::ea(&m35F0), gabi::ea(&current.pos.y));
    fcpy_l(gabi::ea(&m35F4), gabi::ea(&current.pos.y));
    fcpy_l(gabi::ea(&m3688.y), gabi::ea(&current.pos.y));
    fcpy_l(gabi::ea(&m3688.z), gabi::ea(&current.pos.z));
    f32 y = current.pos.y;
    current.pos.y = gabi::fadds_ppc(y, player02::ropeCatchHeight()); // HD: += a global (GameCube 95.0f)
    f32 ropeHeight = m3600;
    fcpy_l(gabi::ea(&m370C.x), gabi::ea(&rope->current.pos.x));
    if (!(ropeHeight < 0.0f)) { /* m3600 >= 0.0f (HD: NaN too) */
        f32 v = rope->current.pos.y - ropeHeight;
        m370C.y = v;
        m3600 = v;
    } else if (checkBossGomaStage() && (f32)rope->current.pos.y - current.pos.y > 700.0f) {
        f32 v = rope->current.pos.y - 700.0f;
        m370C.y = v;
        m3600 = v;
    } else {
        u32 gnd = gabi::ea(mGndChk); /* mGndChk.SetPos(&rope->current.pos) */
        fcpy_l(player02first::groundPositionYAddress(gnd), gabi::ea(&rope->current.pos.y));
        fcpy_l(player02first::groundPositionXAddress(gnd), gabi::ea(&rope->current.pos.x));
        fcpy_l(player02first::groundPositionZAddress(gnd), gabi::ea(&rope->current.pos.z));
        f64 groundY = player02GroundCross(dComIfG_Bgsp(), gnd);
        f32 py = current.pos.y;
        if (groundY > (f64)(f32)(py + -175.0f)) { // HD: (y - 125) - 50 folded
            f32 v = (f32)(groundY + 175.0);
            m370C.y = v;
            m3600 = v;
        } else {
            fcpy_l(gabi::ea(&m3600), gabi::ea(&current.pos.y));
            fcpy_l(gabi::ea(&m370C.y), gabi::ea(&current.pos.y));
        }
    }
    gravity = 0.0f;
    fcpy_l(gabi::ea(&m370C.z), gabi::ea(&rope->current.pos.z));
    mNormalSpeed = 0.0f;
    m35A0 = 0.0f;
    player02::setActorParameters(rope, 3); /* fopAcM_SetParam(rope, 3) */
    mProcVar2 = 0x1800;
    s16 a = cM_atan2s(m370C.x - current.pos.x, m370C.z - current.pos.z);
    current.angle.y = a;
    shape_angle.y = a;
    player02::copyWordX(&mRopePos, player02::actorPosition(rope));
    player02::copyWordY(&mRopePos, player02::actorPosition(rope));
    player02::copyWordZ(&mRopePos, player02::actorPosition(rope));
    dComIfGp_setPlayerStatus0(0, player02first::daPyStts0_UNK800000_e);
    return TRUE;
}

/* 023EBC1C */
BOOL daPy_lk_c::procRopeThrowCatch_init() {
    commonProcInit(player02first::daPyProc_ROPE_THROW_CATCH_e);
    setSingleMoveAnime(player02first::ANM_ROPETHROWCATCH, 0.8f, 3.0f, 0xB, 0.0f); // HD: HIO folded
    mNormalSpeed = 0.0f;
    dComIfGp_setPlayerStatus0(0, player02first::daPyStts0_ROPE_AIM_e);
    mProcVar0 = 0x1E;
    return TRUE;
}

/* 023EBCB0 */
BOOL daPy_lk_c::checkSightLine(f32 distance, cXyz* sightPosition) {
    s32 idx = mCameraInfoIdx;
    u32 cam = gabi::ea(dComIfGp_getCamera(idx)); /* dComIfGp_getCamera(mCameraInfoIdx) */
    cXyz* eye = player02first::cameraEye(cam);    /* fopCamM_GetEye_p */
    cXyz* center = player02first::cameraCenter(cam); /* fopCamM_GetCenter_p */
    gabi::Local<cXyz> sightDirection, normalizedDirection, sightOffset, sightEnd;
    cXyz_mi(center, sightDirection, eye);
    player02Normalize(sightDirection.a, normalizedDirection.a);
    cXyz_ml(normalizedDirection, sightOffset, distance);
    cXyz_pl(sightOffset, sightEnd, eye);
    player02::copyWordX(sightPosition, sightEnd);
    player02::copyWordY(sightPosition, sightEnd);
    player02::copyWordZ(sightPosition, sightEnd);
    u32 linChk = mEquipItem == player02first::dItemNo_BOOMERANG_e ? gabi::ea(mBoomerangLinChk) /* mBoomerangLinChk */ : gabi::ea(mRopeLinChk) /* mRopeLinChk */;
    player02SetLine(linChk, gabi::ea(eye), gabi::ea(sightPosition), this);
    BOOL temp_r3 = lk_LineCross(linChk);
    if (temp_r3) {
        player02::copyWordX(sightPosition, player02::lineCrossPosition(linChk)); /* *linChk->GetCrossP() */
        player02::copyWordY(sightPosition, player02::lineCrossPosition(linChk));
        player02::copyWordZ(sightPosition, player02::lineCrossPosition(linChk));
    }
    return temp_r3;
}

/* 023EBE04 */
int daPy_lk_c::throwRope() {
    u32 rope = gabi::ea(mActorKeepEquip.mActor.get()); /* himo2_class* */
    setActAnimeUpper(player02first::dRes_INDEX_LKANM_BCK_ROPETHROW_e, 2, 1.3f, 1.0f, 0xB, -1.0f); // HD: HIO folded
    setPriTextureAnime(0x48, 0);
    mNormalSpeed = 0.0f;
    if (mDirection != player02::DIR_RIGHT) {
        mDirection = 3;
    }
    current.angle.y = (s16)(shape_angle.y - 0x4000);
    mModeFlg = mModeFlg | 1; /* onModeFlg(ModeFlg_00000001) */
    setBlendAtnMoveAnime(0.0f);
    gabi::Local<cXyz> targetPos;
    if (mCurProc == player02first::daPyProc_ROPE_MOVE_e) {
        gabi::Local<cXyz> startPos;
        u32 lock = gabi::ea(mpAttnActorLockOn.get());
        f32 py = current.pos.y + 100.0f;
        if (lock != 0) {
            player02::copyWordX(targetPos, &gabi::at<fopAc_ac_c>(lock)->eyePos); /* targetPos = lockOn->eyePos */
            player02::copyWordY(targetPos, &gabi::at<fopAc_ac_c>(lock)->eyePos);
            player02::copyWordZ(targetPos, &gabi::at<fopAc_ac_c>(lock)->eyePos);
        } else {
            s16 bx = mBodyAngle.x; /* mBodyAngle.x */
            f32 horizontalRange = 500.0f * cM_scos(bx);
            targetPos->x = gabi::fmadds(horizontalRange, cM_ssin(shape_angle.y), current.pos.x);
            targetPos->y = gabi::fnmsubs(500.0f, cM_ssin(bx), py);
            targetPos->z = gabi::fmadds(horizontalRange, cM_scos(shape_angle.y), current.pos.z);
        }
        fcpy_l(startPos.a + 0, gabi::ea(&current.pos.x));
        startPos->y = py;
        fcpy_l(startPos.a + 8, gabi::ea(&current.pos.z));
        u32 chk = gabi::ea(mRopeLinChk); /* mRopeLinChk */
        player02SetLine( chk, startPos.a, targetPos.a, this);
        if (lk_LineCross(chk)) {
            player02::copyWordX(targetPos, player02::lineCrossPosition(chk)); /* targetPos = *mRopeLinChk.GetCrossP() */
            player02::copyWordZ(targetPos, player02::lineCrossPosition(chk));
            player02::copyWordY(targetPos, player02::lineCrossPosition(chk));
        }
    } else {
        checkSightLine(2200.0f, targetPos);
    }
    player02RopeTarget(rope, targetPos.a, &m3600, &m3604);
    player02::setActorParameters(gabi::at<fopAc_ac_c>(rope), 1); /* fopAcM_SetParam(rope, 1) */
    seStartOnlyReverb(player02first::JA_SE_LK_ROPE_LAUNCH);
    gabi::at<player02::sightPacket_c>(gabi::ea(mSightPacket))->mDrawFlg = 0; /* mSightPacket.offDrawFlg() */
    resetFootEffect();
    return TRUE;
}

/* 023EC084 */
BOOL daPy_lk_c::checkNextActionRopeReady() {
    u32 rope = gabi::ea(mActorKeepEquip.mActor.get());
    u32 ropeState = player02::actorParameters(gabi::at<fopAc_ac_c>(rope)); /* fopAcM_GetParam(rope) */
    if (ropeState == 2) {
        return procRopeReady_init();
    }
    if (ropeState == 0) {
        if ((u16)player02::heaps(m_anm_heap_upper)[2].mIdx == player02first::dRes_INDEX_LKANM_BCK_ROPETHROW_e) {
            if (mFrameCtrlUpper[2].getRate() < 0.01f) {
                return procRopeThrowCatch_init();
            }
            return TRUE;
        } else if (!lk_checkAttentionLock()) {
            dComIfGp_setDoStatus(7); /* dActStts_RETURN_e */
        }
        BOOL doThrow = FALSE;
        if (itemButton()) {
            if (lk_equipIsReady() && m355C == 0) {
                m355E = 1;
            } else if (!itemButton() && m355E != 0) {
                doThrow = TRUE;
            }
        } else if (m355E != 0) {
            doThrow = TRUE;
        }
        if (doThrow) {
            m355E = 0;
            if (throwRope()) {
                return TRUE;
            }
        }
        if (cancelItemUpperReadyAnime()) {
            return TRUE;
        }
    } else {
        s32 st = (s32)gabi::at<player02::ropeMode_c>(rope)->m02DC; /* rope->m02DC */
        if (st == 5 || st == 8 || st == 9) {
            return procRopeThrowCatch_init();
        }
        checkNextRopeMode();
        return TRUE;
    }
    if (m355C > 0) {
        m355C = (s16)(m355C - 1);
    }
    return FALSE;
}

namespace player02tail {
static BokoByteTable l_cut_type = {{
    CUT_TYPE_STICK, CUT_TYPE_MACHETE, CUT_TYPE_CLUB,
    CUT_TYPE_DN_SWORD, CUT_TYPE_SPEAR, CUT_TYPE_PG_SWORD,
}};
u8 cutType(u32 type) { return gabi::load<u8>(gabi::ea(&l_cut_type) + type * 1); }
u32 atType(u32 type) { return gabi::load<u32>(player02tail::borrowedIndexEA(gabi::at<be<u32>>(0x10192180), type)); }
u32 atPoint(u32 type) { return gabi::load<u32>(player02tail::borrowedIndexEA(gabi::at<be<u32>>(0x10192138), type)); }
u8 seType(u32 type) { return gabi::load<u8>(player02tail::borrowedIndexEA(gabi::at<be<u8>>(0x10192118), type)); }
f32 cpsRadius(u32 type) { return gabi::load<f32>(player02tail::borrowedIndexEA(gabi::at<be<f32>>(0x10192120), type)); }
}

/* 023EC260 */
BOOL daPy_lk_c::setTalkStatus() {
    if (mNoResetFlg0 & 0xA0000000u) {
        return FALSE;
    }
    if (mpAttnEntryA == 0) {
        return FALSE;
    }
    s32 type = player02tail::entry(mpAttnEntryA)->mType; /* mpAttnEntryA->mType */
    u32 flags;
    if (type == 3 /* fopAc_Attn_TYPE_SPEAK_e */) {
        flags = player02tail::attentionFlags(mpAttnActorA.get()); /* attention_info.flags */
    } else if (type == 1 /* fopAc_Attn_TYPE_TALK_e */ && mpAttnActorA.get() == mpAttnActorLockOn.get()) {
        flags = player02tail::attentionFlags(mpAttnActorA.get());
    } else {
        return FALSE;
    }
    if (flags & 0x2000000) { /* fopAc_Attn_TALKFLAG_NOTALK_e */
        return FALSE;
    }
    if (flags & 0x20000000) {        /* fopAc_Attn_TALKFLAG_CHECK_e */
        dComIfGp_setDoStatus(0xA);         /* dActStts_CHECK_e */
    } else if (flags & 0x40000000) { /* fopAc_Attn_TALKFLAG_READ_e */
        dComIfGp_setDoStatus(0x26);        /* dActStts_READ_e */
    } else {
        BOOL look = (flags & 0x8000000) != 0; /* fopAc_Attn_TALKFLAG_LOOK_e */
        dComIfGp_setDoStatus(look ? player02tail::dActStts_LOOK_e : player02tail::dActStts_SPEAK_e);
    }
    return TRUE;
}

/* 023EC370 */
void daPy_lk_c::setSpecialBattle(BOOL param_0) {
    if (param_0) {
        u32 stage = gabi::ea(dComIfGp_getStartStageName());
        if (!player02tail::sameStage(stage, gabi::guest_addr("GTower"))) {
            return;
        }
    }
    if (mpAttnEntryA == 0) {
        return;
    }
    if (player02tail::entry(mpAttnEntryA)->mType != 2 /* fopAc_Attn_TYPE_BATTLE_e */) {
        return;
    }
    fopEn_enemy_c* enemy = gabi::at<fopEn_enemy_c>(gabi::ea(mpAttnActorLockOn.get()));
    if (gabi::ea(enemy) == 0) {
        return;
    }
    if (enemy->group != 2 /* fopAc_ENEMY_e */) {
        return;
    }
    if (player02::checkPlayerGuard(this)) { /* virtual checkPlayerGuard() */
        return;
    }
    if ((u16)player02_cpp::m_damageType == 1) { /* daPy_dmEcallBack_c::checkCurse() (HD: a global) */
        return;
    }
    if (mEquipItem != player02tail::daPyItem_SWORD_e) {
        return;
    }
    if (enemy->mBtAttackType == 0) {
        return;
    }
    // HD: the NaN-safe forms of the original's branches; HIO folded (1.5)
    f32 now = enemy->mBtNowFrame;
    if (enemy->mBtStartFrame - 1.5f > now) {
        return;
    }
    if (!(now < enemy->mBtEndFrame)) {
        return;
    }
    f64 dist = player02TailDistanceXZ(this, enemy);
    if ((f64)enemy->mBtMaxDis > dist) {
        dComIfGp_setDoStatus(0x1A); /* dActStts_PARRY_e */
    }
}

// HD: a status byte is only set while it is still blank (read and written through two play-object lookups)
static inline void lk_setStatusIfBlank(u32 off, u8 v) {
    if (gabi::load<u8>(dComIfGp_ea() + off) == 0) {
        gabi::store<u8>(dComIfGp_ea() + off, v);
    }
}

/* 023EA680 */
void daPy_lk_c::setDoStatusBasic() {
    /* HD: no up-front dComIfGp_getDoStatus() == BLANK test; each status (do 0x5BB7, HD 0x5BB6 for the
     * enemy-weapon throw) is set only while blank */
    int direction = getDirectionFromShapeAngle();
    f32 fVar1 = checkHeavyStateOn() ? 0.375f : 0.75f; // HD: HIO folded
    if (lk_checkAttentionLock() || (mActorKeepThrow.mActor.get() != nullptr && mpAttnActorLockOn.get() == mActorKeepThrow.mActor.get())) {
        if (mStickDistance > 0.05f && direction != 0 /* DIR_FORWARD */) {
            if (mEquipItem == player02tail::daPyItem_BOKO_e && direction == 1 /* DIR_BACKWARD */) {
                player02tail::setAStatusIfBlank( 0xE); /* dActStts_THROW_e */
            } else {
                player02tail::setDoStatusIfBlank( 0x12); /* dActStts_JUMP_e */
            }
            return;
        }
        BOOL guard = player02::checkPlayerGuard(this); /* virtual checkPlayerGuard() */
        u16 e = mEquipItem;
        if (!guard) {
            if (e == player02tail::daPyItem_BOKO_e) {
                player02tail::setAStatusIfBlank( 0xE);
                return;
            }
            if (e == player02tail::daPyItem_SWORD_e || e == player02tail::dItemNo_SKULL_HAMMER_e) {
                player02tail::setDoStatusIfBlank( 0x51); /* dActStts_UNK43 (HD 0x51) */
                return;
            }
        }
        if (e == 0x101) {
            player02tail::setAStatusIfBlank( 0xE);
        } else {
            player02tail::setDoStatusIfBlank( 0xC); /* dActStts_ATTACK_e */
        }
        return;
    }
    if (mStickDistance > fVar1) {
        u16 e = mEquipItem;
        if (e == 0x101) {
            player02tail::setAStatusIfBlank( 0xE);
        } else {
            player02tail::setDoStatusIfBlank( 0xC); /* dActStts_ATTACK_e */
        }
        return;
    }
    if (!(player02::heaps(m_anm_heap_upper)[2].mIdx == 0xFFFF)) {
        return;
    }
    u16 e = mEquipItem;
    if (e == player02tail::daPyItem_NONE_e || !(mModeFlg & 4) /* checkModeFlg(ModeFlg_00000004) */) {
        return;
    }
    if (e == 0x101) {
        player02tail::setAStatusIfBlank( 0xE);
    } else {
        player02tail::setDoStatusIfBlank( 8); /* dActStts_PUT_AWAY_e */
    }
}

/* 023EC498 */
void daPy_lk_c::setDoStatus() {
    if (setHintActor()) {
        return;
    }
    u32 entry = mpAttnEntryA;
    if (entry == 0) {
        if (mpAttnActorLockOn.get() == nullptr) {
            if (resetFlg0() & 8) { /* checkResetFlg0(daPyRFlg0_UNK8) */
                dComIfGp_setDoStatus(0x11); // HD: no RStatus / climb status
                return;
            }
            if (mFrontWallType == 2) {
                // HD: no sidle status in "Obombh"
                gabi::Local<SafeString> a, b;
                if (lk_strEq2(a, b, 0x100356E8 /* "Obombh" */, dComIfGp_ea() + 0x5134 /* dComIfGp_getStartStageName() */)) {
                    return;
                }
                dComIfGp_setDoStatus(0x10); /* dActStts_SIDLE_e */
                return;
            }
        }
        setDoStatusBasic();
        return;
    }
    s32 type = player02tail::entry(entry)->mType; /* mpAttnEntryA->mType */
    if (!(mNoResetFlg0 & 0xA0000000u)) {
        if (type == 5 /* fopAc_Attn_TYPE_DOOR_e */ || type == 6 /* fopAc_Attn_TYPE_TREASURE_e */) {
            dComIfGp_setDoStatus(0xB); /* dActStts_OPEN_e */
            setDoStatusBasic();
            return;
        }
        if (type == 7 /* fopAc_Attn_TYPE_SHIP_e */) {
            if (!dComIfGp_checkPlayerStatus0(0, player02::daPyStts0_SHIP_RIDE_e)) {
                dComIfGp_setDoStatus(0x1C); /* dActStts_GET_IN_SHIP_e */
            }
            setDoStatusBasic();
            return;
        }
    }
    if (type == 4 /* fopAc_Attn_TYPE_CARRY_e */) {
        u32 actorA = gabi::ea(mpAttnActorA.get());
        if (!(player02tail::actorStatus(gabi::at<fopAc_ac_c>(actorA)) & 0x2000)) { /* !fopAcM_CheckStatus(mpAttnActorA, fopAcStts_CARRY_e) */
            if (actorA != 0 && fpcM_GetName(gabi::at<fopAc_ac_c>(actorA)) == player02tail::fpcNm_BOKO_e) {
                dComIfGp_setDoStatus(0x1B); /* dActStts_PICK_UP_e */
            } else {
                dComIfGp_setDoStatus(4); /* dActStts_LIFT_e */
            }
        }
    } else if (!setTalkStatus()) {
        setSpecialBattle(0);
    }
    setDoStatusBasic();
}

/* 023ECC68 */
void daPy_lk_c::setAnimeUnequipSword() {
    // HD: HIO folded
    if ((mModeFlg & 0x01FD2810) || dComIfGp_event_runCheck() /* dComIfGp_event_runCheck() */ ||
        mDemo.getDemoType() != 0 /* checkPlayerDemoMode() */) {
        setActAnimeUpper(player02tail::dRes_INDEX_LKANM_BCK_REST_e, 2, 1.1f, 0.0f, 0xC, 6.0f);
    } else {
        setActAnimeUpper(0xD7, 2, 0.8f, 0.0f, 0xF, 3.0f);
    }
    setPriTextureAnime(0x72, 0);
}

/* 023ECD3C */
void daPy_lk_c::setAnimeEquipSingleItem(u16 bckIdx) {
    setActAnimeUpper((u32)bckIdx, 2 /* UPPER_MOVE2_e */, 0.7f, 1.0f, 0xA, 3.0f); // HD: HIO folded
    setPriTextureAnime(0x74, 0);
}

/* 023ECD98 */
void daPy_lk_c::setAnimeUnequipItem(u16 i_itemNo) {
    if (i_itemNo == player02tail::dItemNo_BOOMERANG_e || i_itemNo == player02tail::dItemNo_GRAPPLING_HOOK_e ||
        i_itemNo == player02tail::dItemNo_TELESCOPE_e || player02TailPhotoBox(this, (u32)i_itemNo) ||
        (i_itemNo >= 0x21 && i_itemNo <= 0x22) /* dItemNo_TINGLE_TUNER_e, dItemNo_WIND_WAKER_e (HD order) */ ||
        i_itemNo == player02tail::dItemNo_HOOKSHOT_e || i_itemNo == player02tail::dItemNo_DEKU_LEAF_e ||
        player02TailBottle(this, (u32)i_itemNo)) {
        setAnimeEquipSingleItem(player02tail::dRes_INDEX_LKANM_BCK_TAKEL_e);
    } else if (player02TailBow(this, (u32)i_itemNo)) {
        setAnimeEquipSingleItem(player02tail::dRes_INDEX_LKANM_BCK_TAKER_e);
    } else if (i_itemNo == player02tail::dItemNo_SKULL_HAMMER_e) {
        setActAnimeUpper(player02tail::dRes_INDEX_LKANM_BCK_TAKEBOTH_e, 2, 0.8f, 3.0f, 7, 4.0f); // HD: HIO folded
    } else {
        setActAnimeUpper(player02tail::dRes_INDEX_LKANM_BCK_TAKE_e, 2, 1.0f, 2.0f, 0xF, 1.0f);
        setPriTextureAnime(0x73, 0);
    }
}

/* 023ECEF0 */
void daPy_lk_c::setAnimeUnequip() {
    u16 e = mEquipItem;
    if (e == player02tail::daPyItem_SWORD_e) {
        setAnimeUnequipSword();
    } else if (e == player02tail::daPyItem_BOKO_e) {
        deleteEquipItem(0);
        player02OldFrameMorf((u32)m_old_fdata, 5.0f, 0, 0x2A);
    } else {
        setAnimeUnequipItem(e);
    }
    m3562 = 0x100; /* daPyItem_NONE_e */
}

/* 023ECF74 */
BOOL daPy_lk_c::procPushPullWait_init(int param_0) {
    commonProcInit(player02tail::daPyProc_PUSH_PULL_WAIT_e);
    mProcVar6 = param_0;
    mNormalSpeed = 0.0f;
    if (param_0 == 0 || mEquipItem == player02tail::daPyItem_NONE_e) {
        if (checkEquipAnime()) {
            resetActAnimeUpper(2 /* UPPER_MOVE2_e */, -1.0f);
        }
        player02_cpp::m_maba_flg = 0; /* daPy_matAnm_c::offMabaFlg() */
        player02_cpp::m_maba_timer = 1;
        setSingleMoveAnime(player02tail::ANM_WAITPUSHPULL, 1.0f, 0.0f, -1, 5.0f); // HD: HIO folded
        mProcVar3 = 0;
    } else {
        setBlendMoveAnime(2.4f);
        setAnimeUnequip();
        m3598 = 0.0f;
        mProcVar3 = 1;
    }
    if (param_0 != 0) {
        s16 a = m352C;
        mProcVar2 = (s16)(a + 0x8000);
        m370C.x = gabi::fmadds(40.0f, cM_ssin(a), m3724.x);
        fcpy_l(gabi::ea(&m370C.y), gabi::ea(&current.pos.y));
        m370C.z = gabi::fmadds(40.0f, cM_scos(a), m3724.z);
    }
    dComIfGp_setPlayerStatus0(0, player02tail::daPyStts0_UNK4000000_e);
    return TRUE;
}

/* 023ED0F0 */
BOOL daPy_lk_c::procGrabReady_init() {
    commonProcInit(player02tail::daPyProc_GRAB_READY_e);
    mNormalSpeed = 0.0f;
    if (mEquipItem == player02tail::daPyItem_NONE_e) {
        if (checkEquipAnime()) {
            resetActAnimeUpper(2 /* UPPER_MOVE2_e */, -1.0f);
        }
        setSingleMoveAnime(player02tail::ANM_GRABP, 0.8f, 0.0f, 4, 1.0f); // HD: HIO folded
        mProcVar6 = 1;
    } else {
        setBlendMoveAnime(2.4f);
        setAnimeUnequip();
        mProcVar6 = 0;
    }
    BOOL glove = dComIfGs_getSelectEquip(2) == 0x28; /* checkPowerGloveEquip() */
    u32 actorA = gabi::ea(mpAttnActorA.get());
    u32 st = player02tail::actorStatus(gabi::at<fopAc_ac_c>(actorA));
    if ((glove || !(st & 0x10000) /* fopAcStts_UNK10000_e */) && !(st & 0x2000) /* fopAcStts_CARRY_e */) {
        player02TailSetCarryNow(gabi::at<fopAc_ac_c>(actorA), 1);
        player02KeepSetData(&mActorKeepGrab, mpAttnActorA.get());
        player02KeepClearData(&mActorKeepRope);
    } else {
        player02KeepSetData(&mActorKeepRope, gabi::at<fopAc_ac_c>(actorA));
        freeGrabItem();
    }
    fopAc_ac_c* a = mpAttnActorA.get();
    if (a != nullptr && fpcM_GetName(a) == player02tail::fpcNm_BOKO_e) {
        reinterpret_cast<player02tail::BokoCarryView*>(a)->mNowMode = 3; /* boko->setNowMode(daBoko_c::Mode_PLAYER_CARRY_e) */
        a = mpAttnActorA.get();
    }
    s16 angle = fopAcM_searchActorAngleY(this, a);
    shape_angle.y = angle;
    mProcVar2 = angle;
    current.angle.y = angle;
    gabi::Local<cXyz> d;
    cXyz_mi(&mpAttnActorA.get()->current.pos, d, &m3748);
    player02tail::copyZXY(&m370C, d.get());
    return TRUE;
}

/* 023ED2B0 */
void daPy_lk_c::setAtCpsSpl(u8 spl) {
    for (int i = 0; i < 3; i++) {
        mAtCps[i].SetAtSpl(spl);
    }
}

/* 023ED2CC */
void daPy_lk_c::setEnemyWeaponAtParam(BOOL r4) {
    u32 boko = gabi::ea(mActorKeepEquip.mActor.get());
    u32 type = (u32)gabi::at<fopAc_ac_c>(boko)->mParameters; /* fopAcM_GetParam(boko) */
    u8 cutType = player02tail::cutType(type); /* cut_type[] */
    /* the daBoko_c getters read static per-type tables (at type 0x10192180, atp 0x10192138, se 0x10192118, cps R 0x10192120) */
    u32 atType = player02tail::atType(type);
    u32 atp = player02tail::atPoint(type);
    u8 se = player02tail::seType(type);
    f32 r = player02tail::cpsRadius(type);
    if (r4) {
        setAtParam(atType, atp, player02tail::dCcG_At_Spl_UNK1, se, player02::dCcG_AtHitMark_Big_e, (u8)(cutType + 1), r);
    } else {
        setAtParam(atType, atp, player02tail::dCcG_At_Spl_UNK0, se, player02::dCcG_AtHitMark_Nrm_e, cutType, r);
    }
}

/* 023ED380 */
void daPy_lk_c::setJumpCutAtParam() {
    player02::cutState(this)->mCutCount = 0; /* mCutCount */
    u16 e = mEquipItem;
    if (e == player02tail::daPyItem_SWORD_e) {
        // HD: HIO folded
        if ((dComIfGs_getSelectEquip(0) == 0x38 || dComIfGp_getMiniGameType() == 2)) {
            m35FC = 1.7f;
            setAtParam(player02tail::AT_TYPE_SWORD, 2, player02tail::dCcG_At_Spl_UNK1, player02tail::dCcG_SE_UNK1, player02::dCcG_AtHitMark_Big_e, player02tail::CUT_TYPE_JUMPCUT_SWORD, 20.0f);
        } else {
            m35FC = 2.0f;
            setAtParam(2, 4, 1, 1, 0xF, 0xA, 30.0f);
        }
        // HD: a strong (spin) jump cut (|m3578| > 0xF800) also gets the HD flag at 0x69E8 and spl 0
        s32 v = m3578;
        s32 av = (s32)(v < 0 ? 0u - (u32)v : (u32)v); /* abs (INT_MIN stays negative) */
        if (av > 0xF800 && (u16)player02_cpp::m_damageType != 1 /* !checkCurse() */) {
            u16 e2 = mEquipItem;
            if (e2 == 0x103 || e2 == player02tail::daPyItem_BOKO_e) {
                player02tail::strongJumpCut(this) = 1;
                setAtCpsSpl(0);
            }
        }
    } else if (e == player02tail::dItemNo_SKULL_HAMMER_e) {
        setAtParam(player02tail::AT_TYPE_SKULL_HAMMER, 4, player02tail::dCcG_At_Spl_UNK1, player02tail::dCcG_SE_METAL, player02::dCcG_AtHitMark_Nrm_e, player02tail::CUT_TYPE_JUMPCUT_HAMMER, 50.0f);
    } else if (mActorKeepEquip.mActor.get() != nullptr) {
        setEnemyWeaponAtParam(TRUE);
    } else {
        setAtParam(player02tail::AT_TYPE_MACHETE, 2, 1, 1, 0xF, player02tail::CUT_TYPE_JUMPCUT_MACHETE, 30.0f);
    }
}

/* 023ED5A4 */
BOOL daPy_lk_c::procJumpCut_init(int param_0) {
    fopAc_ac_c* equip_actor = mActorKeepEquip.mActor.get();
    if ((u16)player02_cpp::m_damageType == 1 /* daPy_dmEcallBack_c::checkCurse() */ ||
        (mEquipItem == player02tail::daPyItem_BOKO_e && equip_actor == nullptr)) {
        return FALSE;
    }
    commonProcInit(player02tail::daPyProc_JUMP_CUT_e);
    // HD: HIO folded
    if (param_0 != 0) {
        setSingleMoveAnime(player02tail::ANM_JATTACK, 0.8f, 1.0f, 0xF, 5.0f);
        speed.y = 18.0f;
        gravity = -3.0f;
        mNormalSpeed = 9.0f;
    } else {
        setSingleMoveAnime(0x2B, 0.74f, 2.0f, 0xF, 1.0f);
        mNormalSpeed = 18.0f;
        speed.y = 27.0f;
        gravity = -3.0f;
    }
    mProcVar6 = 0;
    player02tail::strongJumpCut(this) = 0;
    current.angle.y = shape_angle.y;
    player02::voiceStart(this, 1);
    dComIfGp_setPlayerStatus0(0, player02::daPyStts0_SWORD_SWING_e);
    if (mEquipItem == player02tail::daPyItem_BOKO_e) {
        setBlurPosResource(player02tail::dRes_INDEX_LKANM__WEAPONJUMP_POS_e);
    } else {
        setBlurPosResource(player02tail::dRes_INDEX_LKANM__CUTJUMP_POS_e);
    }
    setJumpCutAtParam();
    return TRUE;
}

/* 023ED788 */
BOOL daPy_lk_c::procWHideReady_init(u32 param_0, cXyz* param_1) {
    BOOL uVar1 = (mModeFlg >> 4) & 1; /* checkModeFlg(ModeFlg_WHIDE) */
    mNoResetFlg0 = mNoResetFlg0 & ~0x10000u; /* offNoResetFlg0(daPyFlg0_UNK10000) */
    commonProcInit(player02tail::daPyProc_WHIDE_READY_e);
    mNormalSpeed = 0.0f;
    current.angle.y = shape_angle.y;
    player02tail::copyXYZ(&m370C, param_1);
    // HD: HIO folded (m_HIO->mWall.m.field_0x50 = 8.5)
    if (param_0 != 0) {
        mProcVar2 = cM_atan2s(gabi::at<cXyz>(param_0)->x, gabi::at<cXyz>(param_0)->z);
        m370C.x = gabi::fmadds(gabi::at<cXyz>(param_0)->x, 8.5f, m370C.x);
        m370C.z = gabi::fmadds(gabi::at<cXyz>(param_0)->z, 8.5f, m370C.z);
    } else {
        s16 a = m352C;
        mProcVar2 = a;
        m370C.x = gabi::fmadds(cM_ssin(a), 8.5f, m370C.x);
        m370C.z = gabi::fmadds(cM_scos(a), 8.5f, m370C.z);
    }
    if (mEquipItem == player02tail::daPyItem_NONE_e) {
        if (checkEquipAnime()) {
            resetActAnimeUpper(2 /* UPPER_MOVE2_e */, -1.0f);
        }
        setSingleMoveAnime(player02tail::ANM_WALL, 0.0f, 3.0f, -1, 5.0f);
        mProcVar6 = 1;
    } else {
        setBlendMoveAnime(2.4f);
        setAnimeUnequip();
        mProcVar6 = 0;
    }
    if (uVar1 != 0) {
        dComIfGp_setPlayerStatus0(0, player02tail::daPyStts0_UNK1_e);
    }
    return TRUE;
}

/* 023ED970 */
BOOL daPy_lk_c::procCutTurnCharge_init() {
    commonProcInit(player02tail::daPyProc_CUT_TURN_CHARGE_e);
    setSingleMoveAnime(player02tail::ANM_CUTTURNP, 0.8f, 1.0f, 9, 3.0f); // HD: HIO folded
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    return TRUE;
}

/* 023ED9E0 */
BOOL daPy_lk_c::procWeaponNormalSwing_init() {
    if (mActorKeepEquip.mActor.get() == nullptr) {
        return FALSE;
    }
    commonProcInit(player02tail::daPyProc_WEAPON_NORMAL_SWING_e);
    setSingleMoveAnime(player02tail::ANM_CUTBOKO, 1.1f, 1.0f, 0x22, 3.0f); // HD: HIO folded
    current.angle.y = shape_angle.y;
    player02::copyZero(&m3700);
    m34C2 = 1;
    player02::voiceStart(this, 1);
    if (lk_checkAttentionLock() || !(mStickDistance > 0.05f)) {
        mProcVar2 = shape_angle.y;
    } else {
        mProcVar2 = m34E8;
    }
    dComIfGp_setPlayerStatus0(0, player02::daPyStts0_SWORD_SWING_e);
    m351E = m34DC;
    setBlurPosResource(player02tail::dRes_INDEX_LKANM__CUTBOKO_POS_e);
    setEnemyWeaponAtParam(FALSE);
    mProcVar0 = 0; // HD: HIO folded
    return TRUE;
}

/* 023EDB6C */
BOOL daPy_lk_c::procWeaponSideSwing_init() {
    commonProcInit(player02tail::daPyProc_WEAPON_SIDE_SWING_e);
    setSingleMoveAnime(player02tail::ANM_HAMSWINGA, 0.9f, 20.0f, 0x2A, 6.0f); // HD: HIO folded
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    player02::voiceStart(this, 1);
    mProcVar2 = shape_angle.y;
    setBlurPosResource(player02tail::dRes_INDEX_LKANM__HAMMERSIDE_POS_e);
    setEnemyWeaponAtParam(FALSE);
    return TRUE;
}

/* 023EDC14 */
BOOL daPy_lk_c::procWeaponFrontSwingReady_init() {
    commonProcInit(player02tail::daPyProc_WEAPON_FRONT_SWING_READY_e);
    setSingleMoveAnime(player02tail::ANM_HAMSWINGBPRE, 0.7f, 3.0f, 0xD, 3.0f); // HD: HIO folded
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    if (lk_checkAttentionLock() || !(mStickDistance > 0.05f)) {
        mProcVar2 = shape_angle.y;
    } else {
        mProcVar2 = m34E8;
    }
    setEnemyWeaponAtParam(FALSE);
    player02::voiceStart(this, 7);
    return TRUE;
}

/* 023EDD14 */
BOOL daPy_lk_c::procWeaponThrow_init() {
    commonProcInit(player02tail::daPyProc_WEAPON_THROW_e);
    setSingleMoveAnime(player02::ANM_CUTA, 1.0f, 0.0f, -1, 5.0f);
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    m34C2 = 1;
    return TRUE;
}

/* 023EDD9C */
BOOL daPy_lk_c::procBottleSwing_init(int param_0) {
    int iVar2 = checkShipRideUseItem(1);
    commonProcInit(player02tail::daPyProc_BOTTLE_SWING_e);
    if (param_0 != 0) {
        deleteEquipItem(0);
        setBottleModel(player02tail::dItemNo_EMPTY_BOTTLE_e);
    }
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    u32 play = dComIfGp_ea();
    /* dComIfGp_att_getCatghTarget(): dAttLook_c::convPId(play + 0x5934, play + 0x5944) */
    player02tail::BottleAttentionView* attention = gabi::at<player02tail::BottleAttentionView>(play);
    fopAc_ac_c* catch_target = player02TailCatchTarget(attention->mCatch, attention->mCatghTargetID);
    BOOL bVar1;
    if (catch_target != 0) {
        u32 mtx = lk_getAnmMtx(mpCLModel, 0 /* CL_JNT_LINK_ROOT_e */);
        bVar1 = !(catch_target->current.pos.y > gabi::at<Mtx34>(mtx)->m[1][3]);
    } else {
        bVar1 = mWaterY > current.pos.y + 10.0f;
    }
    // HD: both swings use the same (folded HIO) parameters
    setSingleMoveAnime(bVar1 ? player02tail::ANM_BINSWINGU : player02tail::ANM_BINSWINGS, 1.2f, 0.0f, 0x10, 0.0f);
    m35A0 = 15.0f;
    mProcVar6 = 0;
    mProcVar2 = 0;
    mProcVar7 = 0;
    player02::voiceStart(this, 7);
    initShipRideUseItem(iVar2, 2);
    return TRUE;
}

/* 023E47D4 */
BOOL daPy_lk_c::procVomitJump_init(int param_0) {
    commonProcInit(player02::daPyProc_VOMIT_JUMP_e);
    m35C4 = 40.0f;
    f32 fVar1 = (param_0 == 2 || param_0 == 3) ? 3.0f : -1.0f;
    mProcVar6 = param_0;
    setSingleMoveAnime(player02::ANM_VOMITJMP, 1.0f, 0.0f, -1, fVar1);
    lk_onPlayerStatus0(player02::daPyStts0_UNK80000000_e);
    mStts.m_cc_move.x = 0.0f;
    mStts.m_cc_move.y = 0.0f;
    mStts.m_cc_move.z = 0.0f;
    mProcVar0 = 2;
    mProcVar2 = 0;
    u32 mtx = lk_getAnmMtx(mpCLModel, 0x12 /* CL_JNT_CHIN_JNT_e */);
    daPy_mtxPosFollowEcallBack_makeEmitterColor(gabi::ea(m33A8), 0x8112 /* ID_IT_SN_LK_BLUR00 */, mtx,
                                                gabi::ea(&current.pos), gabi::ea(&shape_angle), gabi::ea(&tevStr.mColorK0),
                                                gabi::ea(&tevStr.mColorK0));
    if (param_0 == 0) {
        gabi::Local<cXyz> v;
        v->x = 0.0f;
        v->y = 1.0f;
        v->z = 0.0f;
        dComIfGp_getVibration_StartShock(4, -0x21, v);
    }
    if (param_0 == 1) {
        gravity = 0.0f;
        mProcVar1 = 20;
    } else {
        LK_voiceStart(0);
        if (param_0 == 3) {
            mProcVar6 = 1;
            speed.y = 46.0f;
        } else {
            speed.y = 64.5f; // HD: HIO folded
        }
        mProcVar1 = 0;
    }
    return TRUE;
}

/* 023ECA94 */
BOOL daPy_lk_c::procShipReady_init() {
    commonProcInit(player02tail::daPyProc_SHIP_READY_e);
    deleteEquipItem(1);
    fopAc_ac_c* ship = gabi::at<fopAc_ac_c>(lk_getShipActor());
    setSingleMoveAnime(player02tail::ANM_VJMPCL, 0.75f, 0.0f, 0x18, 5.0f); // HD: HIO folded
    gravity = 0.0f;
    mNormalSpeed = 0.0f;
    s16 sVar3 = fopAcM_searchActorAngleY(ship, this);
    J3DModel* model = player02tail::shipBodyModel(ship); /* ship->getBodyMtx() */
    s16 a;
    if ((s16)(sVar3 - ship->shape_angle.y) > 0) {
        PSMTXMultVec(model->getBaseTRMtx(), &player02_cpp::l_ship_ledge, &current.pos);
        a = (s16)(ship->shape_angle.y - 0x4000);
        shape_angle.y = a;
        mProcVar2 = a;
        mProcVar0 = 0;
    } else {
        PSMTXMultVec(model->getBaseTRMtx(), &player02_cpp::l_ship_redge, &current.pos);
        a = (s16)(ship->shape_angle.y + 0x4000);
        shape_angle.y = a;
        mProcVar2 = (s16)(a + 0x8000);
        mProcVar0 = 1;
    }
    current.angle.y = a;
    mProcVar6 = 0;
    reinterpret_cast<player02tail::ShipView*>(ship)->mNextMode = 3; /* ship->setReadyFirst() */
    dComIfGp_setPlayerStatus0(0, player02tail::daPyStts0_UNK1000000_e);
    swimOutAfter(1);
    mNoResetFlg0 = mNoResetFlg0 & ~0x200u; /* offNoResetFlg0(daPyFlg0_SHIP_DROP) */
    m3540 = 0;
    m3542 = 0;
    m353C = 0;
    m353E = 0;
    return TRUE;
}

/* orderTalk: the talk test of one item button (entry, actor, trigger bit, selected item, order function);
 * returns 1 (ordered), 0 (refused) or -1 (not this button) */
static inline int player02TailOrderTalkButton(daPy_lk_c* t, u32 entry, gptr<fopAc_ac_c>* actor, u8 trig, s32 button, Player02TailOrderTalk orderFn) {
    if (entry == 0) {
        return -1;
    }
    s32 type = player02tail::entry(entry)->mType;
    if (!(type == 3 /* fopAc_Attn_TYPE_SPEAK_e */ ||
          (type == 1 /* fopAc_Attn_TYPE_TALK_e */ && gabi::ea(t->mpAttnActorLockOn.get()) == gabi::ea(actor->get())))) {
        return -1;
    }
    if (player02::itemState(t)->mItemTriggerLock != 0 || !(t->mItemTrigger & trig)) { /* HD word 0x8260 blocks it */
        return -1;
    }
    if (player02TailIsEsa( (u32)dComIfGp_getSelectItem(button)) &&
        dComIfGp_getSelectItem(button) != 0x83 /* dItemNo_HYOI_PEAR_e */ && (s32)t->m3630 != -1 /* fpcM_ERROR_PROCESS_ID_e */) {
        player02TailSystemSound(0x883 /* JA_SE_ITEM_TARGET_OUT */);
        return 0;
    }
    orderFn(t, actor->get());
    return 1;
}

/* 023EC724 */
int daPy_lk_c::orderTalk() {
    if (dComIfGp_getDoStatus() == player02tail::dActStts_SPEAK_e || dComIfGp_getDoStatus() == player02tail::dActStts_LOOK_e ||
        dComIfGp_getDoStatus() == player02tail::dActStts_READ_e || dComIfGp_getDoStatus() == player02tail::dActStts_CHECK_e) {
        if (mHD8260 == 0 && (mItemTrigger & 1) /* talkTrigger() */) {
            player02TailOrderTalkEvent(this, mpAttnActorA.get());
            return TRUE;
        }
    }
    int r = player02TailOrderTalkButton(this, mpAttnEntryX, &mpAttnActorX, 4, 0, player02TailOrderTalkX);
    if (r >= 0) return r;
    r = player02TailOrderTalkButton(this, mpAttnEntryY, &mpAttnActorY, 8, 1, player02TailOrderTalkY);
    if (r >= 0) return r;
    r = player02TailOrderTalkButton(this, mpAttnEntryZ, &mpAttnActorZ, 0x10, 2, player02TailOrderTalkZ);
    if (r >= 0) return r;
    // HD: the fourth item button
    r = player02TailOrderTalkButton(this, mpAttnEntryHD, &mpAttnActorHD, 0x80, 3, player02TailOrderTalkHD);
    if (r >= 0) return r;
    return FALSE;
}

/* J3DAnmTransform (HD: vtable at +4): getFrameMax (slot 0x14, an int) and getAttribute (slot 0xC); the frame at +0 */
static inline s32 lk_anm_getFrameMax(u32 anm) { return gabi::call_ptr<s32>(gabi::at<player02::animation_c>(anm)->mpVtable->getFrameMax, anm); }
static inline u32 lk_anm_getAttribute(u32 anm) { return gabi::call_ptr<u32>(gabi::at<player02::animation_c>(anm)->mpVtable->getAttribute, anm); }

/* 023E0E50 */
int daPy_lk_c::setMoveAnime(f32 f27, f32 f28, f32 f25, int r27, int r28, int r29, f32 i_morf) {
    u32 r3 = (u32)gabi::at<player02::anmRatioPack_c>(gabi::ea(mAnmRatioUnder))[0].mpAnmTransform; /* mAnmRatioUnder[UNDER_MOVE0_e].getAnmTransform() */
    J3DFrameCtrl* frameCtrl0 = &mFrameCtrlUnder[0];
    J3DFrameCtrl* frameCtrl1 = &mFrameCtrlUnder[1];
    f32 f31;
    u8 prev = m34C3;
    if (prev == 0 || prev == 9 || prev == 10) {
        f31 = 0.0f;
    } else {
        s32 fm = lk_anm_getFrameMax(r3);
        f31 = (f32)gabi::at<player02::animation_c>(r3)->mFrame / (f32)fm; /* r3->getFrame() / r3->getFrameMax() */
    }
    u32 r25 = getAnmData(r27);
    u32 r24 = getAnmData(r28);
    gabi::Local<be<u32>> sp14, sp18, sp10, sp1C; /* under/upper of the two animations */
    getUnderUpperAnime(r25, sp14.a, sp18.a, 0, 0x2400);
    getUnderUpperAnime(r24, sp10.a, sp1C.a, 1, 0x2400);
    f32 inv = 1.0f - f27;
    lk_anmRatio_clear(gabi::ea(this) + 0x57F8, inv); /* mAnmRatioUnder[UNDER_MOVE0_e].setRatio(1.0f - f27) */
    lk_anmRatio_clear(gabi::ea(this) + 0x5808, f27);  /* mAnmRatioUnder[UNDER_MOVE1_e].setRatio(f27) */
    lk_anmRatio_clear(gabi::ea(this) + 0x5818, inv);  /* mAnmRatioUpper[UPPER_MOVE0_e] */
    lk_anmRatio_clear(gabi::ea(this) + 0x5828, f27);  /* mAnmRatioUpper[UPPER_MOVE1_e] */
    f32 f3 = (f32)lk_anm_getFrameMax(*sp14);
    s32 fmB = lk_anm_getFrameMax(*sp10);
    f32 f30 = 1.0f / f3;
    f32 f26 = (f32)fmB;
    f32 rate = gabi::fmadds((f25 * f3) / f26 - f28, f27, f28); /* f28 + f27 * ((f25 * f3) / f26 - f28) */
    u32 attr = lk_anm_getAttribute(*sp14);
    player02SetFrameCtrl(this, frameCtrl0, attr, 0, (s32)(s16)gabi::ftoi(f3), rate, f31 * f3);
    fcpy_l(*sp14, gabi::ea(frameCtrl0) + 4); /* sp14->setFrame(frameCtrl0.getFrame()) */
    attr = lk_anm_getAttribute(*sp10);
    player02SetFrameCtrl(this, frameCtrl1, attr, 0, (s32)(s16)gabi::ftoi(f26), (rate * f26) * f30, f31 * f26);
    fcpy_l(*sp10, gabi::ea(frameCtrl1) + 4);
    gabi::at<player02::anmRatioPack_c>(gabi::ea(mAnmRatioUnder))[0].mpAnmTransform = *sp14; /* mAnmRatioUnder[0].setAnmTransform(sp14) */
    gabi::at<player02::anmRatioPack_c>(gabi::ea(mAnmRatioUnder))[1].mpAnmTransform = *sp10; /* mAnmRatioUnder[1].setAnmTransform(sp0C) */
    if (*sp18 != 0) {
        f32 f2 = (f32)lk_anm_getFrameMax(*sp18);
        gabi::at<player02::anmRatioPack_c>(gabi::ea(mAnmRatioUpper))[0].mpAnmTransform = *sp18; /* mAnmRatioUpper[0].setAnmTransform(sp10) */
        attr = lk_anm_getAttribute(*sp18);
        player02SetFrameCtrl(this, &mFrameCtrlUpper[0], attr, 0, (s32)(s16)gabi::ftoi(f2), (rate * f2) * f30, f31 * f2);
        fcpy_l(*sp18, gabi::ea(&mFrameCtrlUpper[0]) + 4);
    } else {
        gabi::at<player02::anmRatioPack_c>(gabi::ea(mAnmRatioUpper))[0].mpAnmTransform = *sp14;
    }
    if (*sp1C != 0) {
        f32 f2 = (f32)lk_anm_getFrameMax(*sp1C);
        gabi::at<player02::anmRatioPack_c>(gabi::ea(mAnmRatioUpper))[1].mpAnmTransform = *sp1C; /* mAnmRatioUpper[1].setAnmTransform(sp08) */
        attr = lk_anm_getAttribute(*sp1C);
        player02SetFrameCtrl(this, &mFrameCtrlUpper[1], attr, 0, (s32)(s16)gabi::ftoi(f2), (rate * f2) * f30, f31 * f2);
        fcpy_l(*sp1C, gabi::ea(&mFrameCtrlUpper[1]) + 4);
    } else {
        gabi::at<player02::anmRatioPack_c>(gabi::ea(mAnmRatioUpper))[1].mpAnmTransform = *sp10;
    }
    if (!(i_morf < 0.0f)) { // HD: NaN morphs too
        player02OldFrameMorf((u32)m_old_fdata, i_morf, 0, 0x2A);
    }
    /* mAnmDataTable (0x100366A0, 8 bytes each): mTexAnmIdx at +6 */
    if (mDirection == player02::DIR_BACKWARD || (mCurProc == player02::daPyProc_ROPE_SWING_e && !(mModeFlg & 0x400))) {
        setTextureAnime((u32)player02::animationData(r28).mTexAnmIdx, 0);
    } else {
        setTextureAnime((u32)player02::animationData(r27).mTexAnmIdx, 0);
    }
    if (r29 == 5 || r29 == 2) {
        setSeAnime((u32)gabi::at<player02::anmRatioPack_c>(gabi::ea(mAnmRatioUnder))[0].mpAnmTransform, gabi::ea(&player02::heaps(m_anm_heap_under)[0]), gabi::ea(frameCtrl0));
    } else {
        setSeAnime((u32)gabi::at<player02::anmRatioPack_c>(gabi::ea(mAnmRatioUnder))[1].mpAnmTransform, gabi::ea(&player02::heaps(m_anm_heap_under)[1]), gabi::ea(frameCtrl1));
    }
    m34C3 = (u8)r29;
    return TRUE;
}

/* v.abs2XZ() through a stack cXyz {x, 0, z} and PSVECSquareMag */
static inline f32 lk_abs2XZ(cXyz* v) {
    gabi::Local<cXyz> xz;
    fcpy_l(xz.a + 0, gabi::ea(&v->x));
    xz->y = 0.0f;
    fcpy_l(xz.a + 8, gabi::ea(&v->z));
    return PSVECSquareMag(xz);
}

/* 023E14A0 */
void daPy_lk_c::setBlendMoveAnime(f32 param_1) {
    f32 f1_1 = m3580 == 8 ? 1.0f : cM_scos(m34E2);
    f32 f30 = std::fabs(gabi::fmuls_ppc(f1_1, mNormalSpeed)) / mMaxNormalSpeed;
    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[1]; /* UNDER_MOVE1_e */
    // HD: HIO folded
    f32 f29, f28;
    if (dComIfGp_event_runCheck() /* dComIfGp_event_runCheck() */ || mDemo.getDemoType() != 0 /* checkPlayerDemoMode() */) {
        f29 = 1.2f;
        f28 = 0.87f;
    } else if (checkHeavyStateOn()) {
        f29 = 0.85f;
        f28 = 0.7f;
    } else {
        f29 = 0.8f;
        f28 = 1.0f;
    }
    int r28 = 0; /* ANM_WAITS */
    int r27, r26;
    if (field_0x2b0 < 0.0f) { /* checkGrabWear() */
        r27 = 0x95; /* ANM_WALKBARREL */
        r26 = 0x95;
    } else if (checkHeavyStateOn()) {
        if ((lk_abs2XZ(&m373C) > 25.0f && cLib_distanceAngleS(cM_atan2s(m373C.x, m373C.z), shape_angle.y) >= 0x4000) ||
            ((noResetFlg1() & 0x10000000) && m3644 > 5.0f && cLib_distanceAngleS(m3640, shape_angle.y) >= 0x4000)) {
            r27 = 5; /* ANM_WALKHBOOTSKAZE */
            r26 = 5;
            if (!(noResetFlg1() & 0x1000000)) { /* daPyFlg1_UNK1000000 */
                param_1 = 2.4f;
                setNoResetFlg1(noResetFlg1() | 0x1000000);
            }
        } else {
            r27 = 4; /* ANM_WALKHBOOTS */
            r26 = 4;
            if (noResetFlg1() & 0x1000000) {
                param_1 = 2.4f;
                setNoResetFlg1(noResetFlg1() & ~0x1000000u);
            }
        }
    } else if (m3580 != 8 && m34E2 <= -0x11C7) {
        r27 = 6; /* ANM_WALKSLOPE */
        r26 = 6;
        if (!(noResetFlg1() & 0x80)) { /* daPyFlg1_UNK80 */
            param_1 = 2.4f;
            setNoResetFlg1(noResetFlg1() | 0x80);
        }
    } else {
        r27 = 1; /* ANM_WALK */
        if ((lk_abs2XZ(&m3730) > 25.0f && cLib_distanceAngleS(cM_atan2s(m3730.x, m3730.z), shape_angle.y) >= 0x4000) ||
            ((noResetFlg1() & 0x10000000) && m3644 > 5.0f && cLib_distanceAngleS(m3640, shape_angle.y) >= 0x4000)) {
            r26 = 3; /* ANM_DASHKAZE */
            if (!(noResetFlg1() & 0x1000000)) {
                param_1 = 2.4f;
                setNoResetFlg1(noResetFlg1() | 0x1000000);
            }
        } else {
            r26 = 2; /* ANM_DASH */
            if (noResetFlg1() & 0x1000000) {
                param_1 = 2.4f;
                setNoResetFlg1(noResetFlg1() & ~0x1000000u);
            }
        }
        if (noResetFlg1() & 0x80) {
            param_1 = 2.4f;
            setNoResetFlg1(noResetFlg1() & ~0x80u);
        }
    }
    BOOL r25 = FALSE;
    f32 f31 = lk_abs2XZ(&m36A0);
    f32 f25 = lk_abs2XZ(&m3730);
    f32 f1_2 = lk_abs2XZ(&m36B8);
    BOOL bVar3;
    if (f31 > f25) {
        f1_2 = fsel_l(f31 - f1_2, f31, f1_2);
        bVar3 = TRUE;
    } else {
        if (f25 > f1_2) {
            f1_2 = f25;
        }
        bVar3 = FALSE;
    }
    // HD: the NaN-safe forms of the original's branches (NaN slips)
    BOOL slip = FALSE;
    if (mStickDistance < 0.05f) {
        if (!(f1_2 < 25.0f)) {
            slip = TRUE;
        } else if (!(f1_2 < 0.09f) && (m34C3 == 9 || m34C3 == 10)) {
            slip = TRUE;
        }
    }
    if (slip) {
        seStartMapInfo(0x205A /* JA_SE_LK_SLIP_SUS */);
        u8 c = m34C3;
        if ((c != 9 && bVar3) || (c != 10 && !bVar3)) {
            if (bVar3) {
                setSingleMoveAnime(player02::ANM_SLIPICE, 0.9f, 0.0f, 0x1D, 2.0f);
                m34C3 = 9;
            } else {
                setSingleMoveAnime(player02::ANM_WAITQ, 1.3f, 0.0f, -1, 2.4f);
                setTextureAnime(0x68, 0);
                m34C3 = 10;
            }
            LK_voiceStart(35);
        }
        if (LK_checkGrabAnime()) {
            resetActAnimeUpper(player02::UPPER_MOVE2_e, -1.0f);
        }
        freeGrabItem();
        if (mEquipItem == 0x101 /* daPyItem_BOKO_e */) {
            deleteEquipItem(0);
        }
        m3598 = 0.0f;
    } else {
        if (m34C3 == 9) {
            param_1 = 2.4f;
        }
        if (f30 < 0.5f || checkHeavyStateOn()) {
            BOOL heavy = checkHeavyStateOn();
            f32 f25_2 = f30 + f30; /* f30 / 0.5f */
            if (heavy && f25_2 > 0.55f) {
                r25 = TRUE;
            }
            r28 = 0; /* ANM_WAITS */
            if (mModeFlg & 1) { /* checkModeFlg(ModeFlg_00000001) */
                if (lk_checkAttentionLock() && (LK_upperIdx() == 0x16 || LK_upperIdx() == 0x1B) /* checkUpperGuardAnime() */) {
                    setMoveAnime(0.0f, 1.25f, 1.0f, player02::ANM_ATNRS, player02::ANM_ATNWRS, 2, 2.4f);
                    return;
                }
                m3598 = 0.0f;
                if ((noResetFlg1() & 0x100) /* daPyFlg1_CONFUSE */ && mDemo.getDemoType() == 0 &&
                    !dComIfGp_event_runCheck()) {
                    if (LK_underIdx() != player02::dRes_INDEX_LKANM_BCK_WAITQ_e) {
                        setSingleMoveAnime(player02::ANM_WAITQ, 1.3f, 0.0f, -1, 2.4f);
                        m34C3 = 2;
                    }
                    return;
                }
                if (shape_angle.y != m34DE && !lk_checkAttentionLock()) {
                    s16 r3 = (s16)(shape_angle.y - m34DE);
                    r27 = r3 > 0 ? player02::ANM_ATNWLS : player02::ANM_ATNWRS;
                    s32 ar = r3 < 0 ? -r3 : r3;
                    f25_2 = gabi::fmadds((f32)ar, 0.001f, 0.5f);
                    if (f25_2 > 1.0f) {
                        f25_2 = 1.0f;
                    }
                    if (!(noResetFlg1() & 0x800000)) { /* daPyFlg1_UNK800000 */
                        param_1 = 2.4f;
                    }
                    setNoResetFlg1(noResetFlg1() | 0x800000);
                    setMoveAnime(f25_2, 1.1f, 1.0f, r28, r27, 2, param_1);
                } else {
                    f32 rate;
                    if (checkRestHPAnime()) {
                        r28 = 0x1C; /* ANM_WAITB */
                        rate = 0.4f;
                    } else {
                        rate = 1.1f;
                    }
                    if (noResetFlg1() & 0x800000) {
                        param_1 = 2.4f;
                    }
                    setNoResetFlg1(noResetFlg1() & ~0x800000u);
                    setMoveAnime(f25_2, rate, f29, r28, r27, 2, param_1);
                }
            } else {
                m3598 = gabi::fnmsubs(1.0f - f28, f25_2, 1.0f); /* 1.0f - (1.0f - f28) * f25_2 */
                setMoveAnime(f25_2, 1.1f, f29, r28, r27, 1, param_1);
            }
            if (r28 == 0x1C && (!dComIfGp_event_runCheck() || mDemo.getDemoType() != 3) &&
                mFrameCtrlUnder[0].checkPass(15.0f)) {
                if (dComIfGs_getLife() <= 2) { /* dComIfGs_getLife() */
                    LK_voiceStart(21);
                } else {
                    LK_voiceStart(20);
                }
            }
        } else if (f30 < 1.0f) {
            f32 d = f30 - 0.5f;
            f32 f1 = d + d; /* (f30 - 0.5f) / 0.5f */
            setMoveAnime(f1, f29, 2.3f, r27, r26, 1, param_1);
            m3598 = gabi::fmuls_ppc(f28, 1.0f - f1);
        } else {
            if (!(f31 < 169.0f)) { // HD: NaN takes the fast branch
                setResetFlg0(resetFlg0() | 0x40000); /* onResetFlg0(daPyRFlg0_UNK40000) */
                setMoveAnime(1.0f, 3.91f, 3.91f, r26, r26, 1, param_1); /* 1.7f * 2.3f */
            } else {
                setMoveAnime(1.0f, 2.3f, 2.3f, r26, r26, 1, param_1);
            }
            m3598 = 0.0f;
        }
    }
    if (!(f30 < 0.9f) || r25) {
        setResetFlg0(resetFlg0() | 0x10); /* onResetFlg0(daPyRFlg0_UNK10) */
    }
    if (!(mModeFlg & 1)) {
        if (frameCtrl->checkPass(4.0f)) {
            setResetFlg0(resetFlg0() | 0x400); /* daPyRFlg0_RIGHT_FOOT_ON_GROUND */
        } else if (frameCtrl->checkPass(20.0f)) {
            setResetFlg0(resetFlg0() | 0x800); /* daPyRFlg0_LEFT_FOOT_ON_GROUND */
        } else {
            resetFootEffect();
        }
    }
    if (m34C3 != 9 && m34C3 != 10) {
        if (f30 > 0.8f) {
            setHandModel(r26);
        } else if (f30 > 0.0f) {
            setHandModel(r27);
        } else {
            setHandModel(r28);
        }
    }
}

/* getWHideModePolygon: the side line check of one direction (s = +1 for the left side, -1 for the right) */
static inline BOOL lk_whideSideCheck(daPy_lk_c* t, u32 tri, u32 p2, f32 f31, BOOL left, s16 sVar7) {
    gabi::Local<cXyz> local_3c;
    gabi::Local<cXyz> local_48;
    f32 nz = player02::plane(tri)->mNormal.z;
    f32 nx = player02::plane(tri)->mNormal.x;
    f32 x0 = (f32)gabi::at<cXyz>(p2)->x;
    f32 z0 = (f32)gabi::at<cXyz>(p2)->z;
    f32 x, z;
    if (left) {
        x = gabi::fmadds(1.25f, nx, gabi::fmadds(f31, nz, x0));  /* p2.x + f31 * n.z + n.x * 1.25f */
        z = gabi::fmadds(1.25f, nz, gabi::fnmsubs(f31, nx, z0)); /* p2.z - f31 * n.x + n.z * 1.25f */
    } else {
        x = gabi::fmadds(1.25f, nx, gabi::fnmsubs(f31, nz, x0)); /* p2.x - f31 * n.z + n.x * 1.25f */
        z = gabi::fmadds(1.25f, nz, gabi::fmadds(f31, nx, z0));  /* p2.z + f31 * n.x + n.z * 1.25f */
    }
    fcpy_l(local_3c.a + 4, p2 + 4);
    local_3c->x = x;
    local_3c->z = z;
    fcpy_l(local_48.a + 4, p2 + 4);
    local_48->x = gabi::fnmsubs(2.5f, player02::plane(tri)->mNormal.x, x); /* local_3c.x - 2.5f * n.x */
    local_48->z = gabi::fnmsubs(2.5f, player02::plane(tri)->mNormal.z, z);
    u32 chk = gabi::ea(t->mLinkLinChk); /* mLinkLinChk */
    player02SetLine( chk, local_3c.a, local_48.a, t);
    if (!lk_LineCross(chk)) {
        return FALSE;
    }
    u32 tri2 = dBgS_GetTriPla_l(chk + 0x14);
    if (tri2 == 0) { /* HD */
        return FALSE;
    }
    return sVar7 == cM_atan2s(player02::plane(tri2)->mNormal.x, player02::plane(tri2)->mNormal.z);
}

/* 023E33AC */
u32 daPy_lk_c::getWHideModePolygon(cXyz* i_start, cXyz* i_end, cXyz* param_2, int direction) {
    u32 chk = gabi::ea(mLinkLinChk); /* mLinkLinChk */
    if (i_start != nullptr) {
        player02SetLine( chk, gabi::ea(i_start), gabi::ea(i_end), this);
        if (!lk_LineCross(chk)) {
            return 0;
        }
    }
    u32 triPla = dBgS_GetTriPla_l(chk + 0x14);
    if (triPla == 0) { /* HD */
        return 0;
    }
    s16 sVar7 = cM_atan2s(player02::plane(triPla)->mNormal.x, player02::plane(triPla)->mNormal.z);
    if (std::fabs(player02::plane(triPla)->mNormal.y) > 0.05f) {
        return 0;
    }
    if (dBgS_GetSpecialCode_l(chk + 0x14) == 3) {
        return 0;
    }
    u32 p2 = gabi::ea(param_2);
    fcpy_l(p2 + 0, chk + 0x30); /* *param_2 = mLinkLinChk.GetCross() */
    fcpy_l(p2 + 4, chk + 0x34);
    fcpy_l(p2 + 8, chk + 0x38);
    /* mGndChk.SetPos(local_3c) (HD: HIO folded, 8.5; the GameCube sin for z kept) */
    u32 gnd = gabi::ea(mGndChk);
    f32 s = cM_ssin(sVar7);
    fcpy_l(gnd + 0x28, chk + 0x34);
    player02::ground(gnd)->m_pos.x = gabi::fmadds(s, 8.5f, (f32)player02::line(chk)->mLin.mEnd.x);
    player02::ground(gnd)->m_pos.z = gabi::fmadds(s, 8.5f, (f32)player02::line(chk)->mLin.mEnd.z);
    f64 f31 = player02GroundCross(dComIfG_Bgsp(), gnd);
    if (f31 != (f64)-1000000000.0f /* -G_CM3D_F_INF */) {
        u32 tri = dBgS_GetTriPla_l(gnd + 0x14);
        if (tri == 0 || player02::plane(tri)->mNormal.y < 0.5f) { /* !cBgW_CheckBGround() (HD: null check) */
            return 0;
        }
    }
    f32 dist = direction == player02::DIR_NONE ? 49.9f : 99.8f;
    if (direction != player02::DIR_RIGHT) {
        if (!lk_whideSideCheck(this, triPla, p2, dist, TRUE, sVar7)) {
            return 0;
        }
    }
    if (direction != player02::DIR_LEFT) {
        if (!lk_whideSideCheck(this, triPla, p2, dist, FALSE, sVar7)) {
            return 0;
        }
    }
    if (direction == player02::DIR_NONE) {
        s16 d = (s16)(sVar7 - current.angle.y - 0x8000);
        s32 ad = d < 0 ? -d : d;
        return ad > 0x1555 ? 0 : triPla;
    }
    // HD: HIO folded (m_HIO->mWall.m.field_0x54 = 25.0)
    s16 d;
    if (direction == player02::DIR_LEFT) {
        s16 sy = shape_angle.y;
        gabi::at<cXyz>(p2)->x = gabi::fmadds(25.0f, player02::plane(triPla)->mNormal.z, (f32)gabi::at<cXyz>(p2)->x);
        gabi::at<cXyz>(p2)->z = gabi::fnmsubs(25.0f, player02::plane(triPla)->mNormal.x, (f32)gabi::at<cXyz>(p2)->z);
        d = (s16)(sVar7 - sy);
    } else {
        s16 sy = shape_angle.y;
        gabi::at<cXyz>(p2)->x = gabi::fnmsubs(25.0f, player02::plane(triPla)->mNormal.z, (f32)gabi::at<cXyz>(p2)->x);
        gabi::at<cXyz>(p2)->z = gabi::fmadds(25.0f, player02::plane(triPla)->mNormal.x, (f32)gabi::at<cXyz>(p2)->z);
        d = (s16)(sy - sVar7);
    }
    if (d > 0x2AAA) {
        return 0;
    }
    return triPla;
}

/* fnmsubs with a double first operand (an unrounded f1 result): b - a * c, rounded once */
static inline f32 lk_fnmsubs_d(f64 a, f32 c, f32 b) { return (f32)(-(a * (f64)c - (f64)b)); }
/* cBgS_PolyInfo assignment (poly/bg index, two words; not the vtable at +0xC) */
static inline void lk_polyInfo_copy(u32 dst, u32 src) {
    cBgS_PolyInfo* out = gabi::at<cBgS_PolyInfo>(dst);
    cBgS_PolyInfo* in = gabi::at<cBgS_PolyInfo>(src);
    out->mPolyIndex = in->mPolyIndex;
    out->mBgIndex = in->mBgIndex;
    out->mpBgW = in->mpBgW;
    out->mActorId = in->mActorId;
}
/* the plane (normal.z, 0, s * normal.x) through m3724 (a cM3dGPla built by 020189EC, HD size 0x14) crossed with
 * the wall and the ground plane of mGndChk (cM3d_3PlaneCrossPos 020176D4) */
static inline BOOL lk_wallGroundCross(daPy_lk_c* t, u32 wall, f32 xsign, gabi::Local<cXyz>& out) {
    gabi::Local<cXyz> n;
    gabi::Local<cXyz> nn;
    n->x = player02::plane(wall)->mNormal.z;
    n->y = 0.0f;
    if (xsign < 0.0f) {
        n->z = -player02::plane(wall)->mNormal.x;
    } else {
        fcpy_l(n.a + 8, wall + 0);
    }
    player02Normalize(n.a, nn.a);
    f32 d = -gabi::fmadds(n->x, t->m3724.x, gabi::fmuls_ppc(n->z, t->m3724.z));
    gabi::Local<u8[0x14]> pla;
    player02PlaneCtor( pla.a, n.a, d);
    u32 gtri = dBgS_GetTriPla_l(gabi::ea(static_cast<cBgS_PolyInfo*>(player02::ground(gabi::ea(t->mGndChk)))));
    return player02ThreePlaneCross(wall, gtri, pla.a, out.a);
}

/* 023E3850 */
void daPy_lk_c::setFrontWallType() {
    f32 radius = mAcchCir[0].GetWallR(); /* mAcchCir[0].GetWallR() */
    if (mFrontWallType != 0) {
        return;
    }
    mFrontWallType = 1;
    if (!(mAcch.m_flags & 0x10) /* !ChkWallHit() */ && !(mModeFlg & 0x200200) /* ModeFlg_HOOKSHOT | ModeFlg_PUSHPULL */) {
        return;
    }
    if (field_0x2b0 < 0.0f) { /* checkGrabWear() */
        return;
    }
    s16 sy = shape_angle.y;
    f32 sn = cM_ssin(sy);
    f32 cs = cM_scos(sy);
    f32 r25 = radius + 25.0f;
    f32 dx = sn * r25;
    f32 dz = cs * r25;
    u32 chk = gabi::ea(mLinkLinChk); /* mLinkLinChk */
    u32 chkPoly = chk + 0x14;
    gabi::Local<cXyz> start;
    gabi::Local<cXyz> end;
    int i;
    for (i = 2; i >= 0; i--) {
        f32 y = current.pos.y + mAcchCir[i].GetWallH(); /* GetWallH() */
        fcpy_l(start.a + 0, gabi::ea(&current.pos.x));
        start->y = y;
        fcpy_l(start.a + 8, gabi::ea(&current.pos.z));
        end->x = current.pos.x + dx;
        end->y = y;
        end->z = current.pos.z + dz;
        player02SetLine( chk, start.a, end.a, this);
        if (lk_LineCross(chk)) {
            break;
        }
    }
    if (i == -1) {
        return;
    }
    u32 wall = dBgS_GetTriPla_l(chkPoly);
    if (wall == 0 || std::fabs(player02::plane(wall)->mNormal.y) > 0.05f) { // HD: null check
        return;
    }
    s16 a = cM_atan2s(player02::plane(wall)->mNormal.x, player02::plane(wall)->mNormal.z);
    s16 back = (s16)(shape_angle.y + 0x8000);
    m352C = a;
    if (cLib_distanceAngleS(a, back) > 0x2000) {
        return;
    }
    s32 wall_code;
    if (mModeFlg & 2) { /* ModeFlg_MIDAIR */
        gabi::at<player02::vectorWords_c>(gabi::ea(&m3724))->x = gabi::at<player02::vectorWords_c>(gabi::ea(&player02::line(chk)->mLin.mEnd))->x; /* m3724 = mLinkLinChk.GetCross() */
        gabi::at<player02::vectorWords_c>(gabi::ea(&m3724))->y = gabi::at<player02::vectorWords_c>(gabi::ea(&player02::line(chk)->mLin.mEnd))->y;
        gabi::at<player02::vectorWords_c>(gabi::ea(&m3724))->z = gabi::at<player02::vectorWords_c>(gabi::ea(&player02::line(chk)->mLin.mEnd))->z;
        wall_code = dBgS_GetWallCode_l(chkPoly);
    } else {
        f64 dVar12 = player02SignedPlaneDistance(wall, &current.pos);
        f32 x = lk_fnmsubs_d(dVar12, player02::plane(wall)->mNormal.x, current.pos.x);
        f32 z = lk_fnmsubs_d(dVar12, player02::plane(wall)->mNormal.z, current.pos.z);
        fcpy_l(gabi::ea(&m3724.y), gabi::ea(&current.pos.y));
        m3724.x = x;
        m3724.z = z;
        wall_code = dBgS_GetWallCode_l(chkPoly);
        if (wall_code != 4 && wall_code != 5) {
            f32 d2 = (f32)(dVar12 + dVar12);
            for (i = 2; i >= 0; i--) {
                f32 y = (current.pos.y + mAcchCir[i].GetWallH()) + player02::wallHeightBias(); // HD: + a global
                if (i == 2) {
                    y = (f32)((f64)y - 0.0085); // HD: the top circle 0.0085 lower
                }
                fcpy_l(start.a + 0, gabi::ea(&current.pos.x));
                start->y = y;
                fcpy_l(start.a + 8, gabi::ea(&current.pos.z));
                end->y = y;
                end->x = gabi::fnmsubs(d2, player02::plane(wall)->mNormal.x, current.pos.x);
                end->z = gabi::fnmsubs(d2, player02::plane(wall)->mNormal.z, current.pos.z);
                player02SetLine( chk, start.a, end.a, this);
                if (lk_LineCross(chk)) {
                    u32 tri2 = dBgS_GetTriPla_l(chkPoly); // HD: no null check
                    gabi::Local<cXyz> sp90;
                    cXyz_mi(gabi::at<cXyz>(wall), sp90, gabi::at<cXyz>(tri2));
                    if (std_sqrtf(PSVECSquareMag(sp90)) < 0.001f) {
                        break;
                    }
                }
            }
            if (i == -1) {
                return;
            }
        }
        wall_code = dBgS_GetWallCode_l(chkPoly);
    }
    if (wall_code == 2) {
        return;
    }
    if (current.pos.y - m35D4 < 125.0f) {
        return;
    }
    if (mNoResetFlg0 & 0x100) { /* checkNoResetFlg0(daPyFlg0_UNK100) */
        if (wall_code == 3) {
            if (!(mModeFlg & 0x40002) /* ModeFlg_SWIM | ModeFlg_MIDAIR */) {
                setResetFlg0(resetFlg0() | 8); /* onResetFlg0(daPyRFlg0_UNK8) */
                lk_polyInfo_copy(gabi::ea(mPolyInfo) /* mPolyInfo */, chkPoly);
                if (mModeFlg & 0x200000) { /* ModeFlg_PUSHPULL */
                    return;
                }
            }
        } else if (wall_code == 1) {
            if (mModeFlg & 2) { /* ModeFlg_MIDAIR */
                u32 gnd = gabi::ea(mGndChk); /* mGndChk.SetPos() */
                f32 py = current.pos.y;
                player02::ground(gnd)->m_pos.y = py + 150.0f;
                player02::ground(gnd)->m_pos.x = gabi::fnmsubs(15.0f, player02::plane(wall)->mNormal.x, m3724.x);
                player02::ground(gnd)->m_pos.z = gabi::fnmsubs(15.0f, player02::plane(wall)->mNormal.z, m3724.z);
                f64 g = player02GroundCross(dComIfG_Bgsp(), gnd);
                if (!(g < (f64)current.pos.y)) {
                    u32 tri = dBgS_GetTriPla_l(gnd + 0x14);
                    if (tri != 0 && !(player02::plane(tri)->mNormal.y < 0.5f) /* cBgW_CheckBGround() */) {
                        gabi::Local<cXyz> sp84;
                        if (lk_wallGroundCross(this, wall, 1.0f, sp84) && !(sp84->y - current.pos.y > 150.0f)) {
                            mFrontWallType = 7;
                            fcpy_l(gabi::ea(&m3724.z), sp84.a + 8);
                            fcpy_l(gabi::ea(&m3724.x), sp84.a + 0);
                            fcpy_l(gabi::ea(&m3724.y), sp84.a + 4);
                            return;
                        }
                    }
                }
            }
            lk_polyInfo_copy(gabi::ea(mPolyInfo) /* mPolyInfo */, chkPoly);
            mFrontWallType = 3;
            return;
        } else if (wall_code == 4 || (wall_code == 5 && !(mModeFlg & 2))) {
            mFrontWallType = wall_code == 4 ? 4 : 5;
            gabi::Local<cXyz> sp6C, sp60, sp54;
            player02TrianglePoints(dComIfG_Bgsp(), chkPoly, sp6C.a, sp60.a, sp54.a);
            lk_polyInfo_copy(gabi::ea(mPolyInfo) /* mPolyInfo */, chkPoly);
            f32 y6C = sp6C->y;
            f32 y60 = sp60->y;
            u32 my = gabi::ea(&m3724.y);
            u32 m8 = gabi::ea(&m35F8);
            if (std::fabs(y6C - y60) < 1.0f) {
                m3724.x = gabi::fmuls_ppc(gabi::fadds_ppc(sp6C->x, sp60->x), 0.5f);
                if (sp54->y > y6C) {
                    fcpy_l(my, sp6C.a + 4);
                    fcpy_l(m8, sp54.a + 4);
                } else {
                    fcpy_l(my, sp54.a + 4);
                    fcpy_l(m8, sp6C.a + 4);
                }
                m3724.z = gabi::fmuls_ppc(gabi::fadds_ppc(sp6C->z, sp60->z), 0.5f);
            } else if (std::fabs(y6C - sp54->y) < 1.0f) {
                m3724.x = gabi::fmuls_ppc(gabi::fadds_ppc(sp6C->x, sp54->x), 0.5f);
                if (y60 > y6C) {
                    fcpy_l(my, sp6C.a + 4);
                    fcpy_l(m8, sp60.a + 4);
                } else {
                    fcpy_l(my, sp60.a + 4);
                    fcpy_l(m8, sp6C.a + 4);
                }
                m3724.z = gabi::fmuls_ppc(gabi::fadds_ppc(sp6C->z, sp54->z), 0.5f);
            } else {
                m3724.x = gabi::fmuls_ppc(gabi::fadds_ppc(sp60->x, sp54->x), 0.5f);
                if (y6C > y60) {
                    fcpy_l(my, sp60.a + 4);
                    fcpy_l(m8, sp6C.a + 4);
                } else {
                    fcpy_l(my, sp6C.a + 4);
                    fcpy_l(m8, sp60.a + 4);
                }
                m3724.z = gabi::fmuls_ppc(gabi::fadds_ppc(sp60->z, sp54->z), 0.5f);
            }
            return;
        }
    }
    // HD: HIO folded (m_HIO->mWallCatch.m.field_0x18 = 170.0)
    f32 py = current.pos.y;
    f32 r50 = radius + 50.0f;
    f32 y = py + 170.1f;
    fcpy_l(start.a + 0, gabi::ea(&current.pos.x));
    start->y = y;
    fcpy_l(start.a + 8, gabi::ea(&current.pos.z));
    end->x = gabi::fmadds(r50, sn, current.pos.x);
    end->y = y;
    end->z = gabi::fmadds(r50, cs, current.pos.z);
    player02SetLine( chk, start.a, end.a, this);
    BOOL cVar8 = lk_LineCross(chk);
    if (!cVar8) {
        u32 roof = gabi::ea(mRoofChk); /* mRoofChk.SetPos(current.pos) */
        gabi::at<player02::vectorWords_c>(gabi::ea(&gabi::at<dBgS_RoofChk>(roof)->m_pos))->z = gabi::at<player02::vectorWords_c>(gabi::ea(&current.pos))->z;
        gabi::at<player02::vectorWords_c>(gabi::ea(&gabi::at<dBgS_RoofChk>(roof)->m_pos))->y = gabi::at<player02::vectorWords_c>(gabi::ea(&current.pos))->y;
        gabi::at<player02::vectorWords_c>(gabi::ea(&gabi::at<dBgS_RoofChk>(roof)->m_pos))->x = gabi::at<player02::vectorWords_c>(gabi::ea(&current.pos))->x;
        f64 r = player02RoofCheck(dComIfG_Bgsp(), roof);
        cVar8 = !((f32)(r - (f64)current.pos.y) > 180.0f);
    }
    if (cVar8) {
        if (LK_checkGrabAnime() || (mModeFlg & 0x40002) /* ModeFlg_MIDAIR | ModeFlg_SWIM */) {
            return;
        }
        f32 r2 = radius + radius;
        s16 ca = current.angle.y;
        f32 y2 = current.pos.y + 149.9f;
        fcpy_l(start.a + 0, gabi::ea(&current.pos.x));
        start->y = y2;
        fcpy_l(start.a + 8, gabi::ea(&current.pos.z));
        end->x = gabi::fmadds(r2, cM_ssin(ca), current.pos.x);
        end->y = y2;
        end->z = gabi::fmadds(r2, cM_scos(ca), current.pos.z);
        gabi::Local<cXyz> sp48;
        if (getWHideModePolygon(start, end, sp48, player02::DIR_NONE) != 0) {
            mFrontWallType = 2;
        }
        return;
    }
    u32 gnd = gabi::ea(mGndChk); /* mGndChk.SetPos() */
    f32 py2 = current.pos.y;
    player02::ground(gnd)->m_pos.y = py2 + 170.1f;
    player02::ground(gnd)->m_pos.x = gabi::fnmsubs(15.0f, player02::plane(wall)->mNormal.x, m3724.x);
    player02::ground(gnd)->m_pos.z = gabi::fnmsubs(15.0f, player02::plane(wall)->mNormal.z, m3724.z);
    f64 ground_y = player02GroundCross(dComIfG_Bgsp(), gnd);
    if (!(mModeFlg & 0x40000) /* !ModeFlg_SWIM */ && ground_y < (f64)current.pos.y) {
        return;
    }
    if (ground_y == (f64)-1000000000.0f /* -G_CM3D_F_INF */) {
        return;
    }
    u32 tri = dBgS_GetTriPla_l(gnd + 0x14);
    if (tri != 0 && player02::plane(tri)->mNormal.y < 0.5f) { /* !cBgW_CheckBGround() (HD: null check) */
        return;
    }
    gabi::Local<cXyz> sp3C;
    if (!lk_wallGroundCross(this, wall, -1.0f, sp3C)) {
        return;
    }
    f32 fVar3 = sp3C->y - current.pos.y;
    fcpy_l(gabi::ea(&m3724.z), sp3C.a + 8); /* m3724 = sp3C */
    fcpy_l(gabi::ea(&m3724.y), sp3C.a + 4);
    fcpy_l(gabi::ea(&m3724.x), sp3C.a + 0);
    u32 mf = mModeFlg;
    // HD: the NaN-safe forms of the original's branches; HIO folded
    if (mf & 0x40000) { /* ModeFlg_SWIM */
        if (fVar3 > 37.6f || fVar3 < -5.0f) {
            return;
        }
        mFrontWallType = 7;
    } else if (mf & 2) { /* ModeFlg_MIDAIR */
        if (!((f32)(ground_y - (f64)mAcch.m_ground_h) > 125.0f)) {
            return;
        }
        if (mCurProc == player02::daPyProc_HOOKSHOT_FLY_e && !(fVar3 < 100.0f)) {
            return;
        }
        mFrontWallType = 7;
    } else {
        if (!(fVar3 < 170.1f)) {
            return;
        }
        if (LK_checkGrabAnime() && !(fVar3 < 75.1f)) {
            return;
        }
        if (fVar3 < 27.09f) {
            return;
        }
        if (fVar3 < 75.1f) {
            mFrontWallType = 6;
        } else if (fVar3 < 110.1f) {
            mFrontWallType = 7;
        } else if (fVar3 < 130.1f) {
            mFrontWallType = 8;
        } else {
            mFrontWallType = 9;
        }
    }
}

/* WWHD glue
data l_ship_ledge 10034FB4
data l_ship_redge 10034FA8
str 100356E0 "GTower" in 023EC370
data eff_pscale_guard 1046D0A0
data eff_pscale 1046D0BC
data eff_dscale_guard 1046D09C
data eff_dscale 1046D0B0
data eff_scale_guard 1046D098
data eff_scale 1046D0A4
ext 02540310 BOOL player02EventCompulsory(u32 event, daPy_lk_c* player, u32 partner, u32 eventID)
ext 025D7C6C u32 player02TalkPartner(daPy_lk_c* player)
ext 024F8044 u32 player02CameraBody()
ext 0253E70C void player02StartEventCamera(u32 camera, u32 eventID, u32 actorID, u32 flags)
ext 025E1E94 void player02TactReset()
ext 025E1EE0 void player02TactSetBeat(int beat)
ext 025E1EF0 void player02TactSetVolume(f32 volume)
ext 025E1F14 void player02TactAmbientPlay()
ext 025E1E7C void player02TaktModeMute()
ext 024EBAE8 u32 player02HintActor(u32 attention, u32 actorID)
ext 023D46F4 void player02DoButtonQuake(daPy_lk_c* player)
ext 0255101C BOOL player02IsEsa(u32 item)
ext 025D7640 void player02OrderZHint(daPy_lk_c* player, u32 actor)
ext 025D7710 void player02OrderDoor(daPy_lk_c* player, fopAc_ac_c* actor)
ext 024EFF3C void player02SetWallRadius(dBgS_AcchCir* circle, f32 radius)
ext 025D5928 fopAc_ac_c* player02CreateArrow(u32 process, u32 parameters, const cXyz* position, s32 room, void* angle, void* scale, int subtype, int flags, int extra)
ext 023DE638 void player02KeepSetData(daPy_actorKeep_l* keep, fopAc_ac_c* actor)
ext 023DC63C void player02KeepClearData(daPy_actorKeep_l* keep)
ext 02055774 BOOL player02ChangeArrowMp()
ext 0205584C u32 player02ChangeArrowType(u32 arrow)
ext 0205578C void player02ChangeArrowTypeNotReady()
ext 0216EEB8 void player02RopeTarget(u32 rope, u32 target, be<f32>* distance, be<f32>* angle)
ext 023DEF04 void daPy_lk_c::setTactModel()
ext 023D8600 BOOL daPy_lk_c::checkBowItem(int item)
ext 023D85E4 BOOL daPy_lk_c::checkPhotoBoxItem(int item)
ext 023DCF08 BOOL daPy_lk_c::checkGrabBarrelSearch(int mode)
ext 023E4E18 int daPy_lk_c::getDirectionFromShapeAngle()
ext 023E8C74 BOOL daPy_lk_c::itemButton()
ext 023D69F8 BOOL daPy_lk_c::checkBoomerangAnime()
data mTexAnmIndexTable 100362A8
data l_ship_offset 10034F84
data l_ship_offset2 10034F90
data l_ship_offset3 10034F9C
data m_damageType 101CEF16
data m_maba_flg 101CEF18
data m_maba_timer 101CEF19
str 1003561C "LkAnm"
str 10035650 "d_a_player_fan.inc"
str 1003564C "0"
str 1003566C "veinsAMat"
str 10035678 "veinsBMat"
ext 024F1AFC void player02SetLine(u32 check, u32 start, u32 end, fopAc_ac_c* actor)
ext 023D457C void player02MakeEmitter(u32 callback, u32 particleID, u32 matrix, u32 position, u32 color)
ext 023D460C void player02MakeEmitterColor(u32 callback, u32 particleID, u32 matrix, u32 position, u32 primaryColor, u32 environmentColor)
ext 027ED2D0 void player02EulerToQuat(s32 x, s32 y, s32 z, u32 out)
ext 025F2258 void player02QuatConcat(u32 a, u32 b, u32 out)
ext 0257E1B8 void player02AllWind(const cXyz* position, u32 direction, u32 power)
ext 025E8508 int player02BckInit(u32 animation, u32 data, u32 bck, s32 play, s32 mode, f32 rate, s32 start, s32 end, s32 modify)
ext 025E3570 void player02RestoreHeap(u32 heap)
ext 02444F48 void player02SafeStringCtor(u32 string)
ext 027DF40C void player02FrameApply(u32 function)
ext 02008974 f64 player02GroundCross(dBgS* bg, u32 check)
ext 024EF6E8 f64 player02RoofCheck(dBgS* bg, u32 check)
ext 0201B31C void player02Normalize(u32 in, u32 out)
ext 020189EC void player02PlaneCtor(u32 plane, u32 normal, f32 distance)
ext 020176D4 BOOL player02ThreePlaneCross(u32 wall, u32 ground, u32 plane, u32 out)
ext 02010C50 f64 player02SignedPlaneDistance(u32 plane, const cXyz* position)
ext 02008570 void player02TrianglePoints(dBgS* bg, u32 poly, u32 a, u32 b, u32 c)
ext 025602F0 void player02SeaColor(u32 ambient, u32 diffuse)
ext 025E3F3C void player02OldFrameMorf(u32 oldFrame, f32 morf, s32 start, s32 end)
ext 025E871C void player02SwordBck(u32 animation, u32 bck)
ext 023DDF64 J3DModel* daPy_lk_c::setItemHeap()
ext 023DDEEC u32 daPy_lk_c::getItemAnimeResource(u32 index)
ext 023DC38C u32 daPy_lk_c::loadTextureAnimeResource(u32 index, BOOL force)
ext 023DC4DC u32 daPy_lk_c::loadTextureScrollResource(u32 index, BOOL force)
ext 023E048C u32 daPy_lk_c::getAnmData(int anm)
ext 023E0530 void daPy_lk_c::getUnderUpperAnime(u32 data, u32 under, u32 upper, int slot, u32 flags)
ext 023D4BB8 u32 daPy_lk_c::initModel(u32 out, int index, u32 flags)
ext 024EDFCC BOOL player02AttentionLock(u32 attention)
ext 025E1988 void player02SystemSound(u32 sound)
ext 02008254 BOOL player02PolySafe(dBgS* bg, u32 poly)
ext 024EF09C s32 player02SpecialCode(dBgS* bg, u32 poly)
ext 024EF080 s32 player02WallCode(dBgS* bg, u32 poly)
ext 024EF0BC s32 player02GroundCode(dBgS* bg, u32 poly)
ext 0200ECD4 f32 player02AddCalc(be<f32>* value, f32 target, f32 scale, f32 maxStep, f32 minStep)
ext 027F3F94 u32 player02RelativeData(u32 header)
ext 027E0174 s32 player02AnimationCount(u32 resource, u32 index)
ext 023D6A18 BOOL daPy_lk_c::checkBowAnime()
ext 023DBBE8 BOOL daPy_lk_c::checkGrabSpecialHeavyState()
ext 023DBC24 BOOL daPy_lk_c::checkHeavyStateOn()
ext 023DD12C BOOL daPy_lk_c::checkRestHPAnime()
ext 023D69CC BOOL daPy_lk_c::checkShipNotNormalMode()
ext 023DFDD8 BOOL daPy_lk_c::commonProcInit(int proc)
ext 023DC7AC void daPy_lk_c::deleteEquipItem(BOOL sound)
ext 023DCA08 void daPy_lk_c::endDamageEmitter()
ext 023DCF8C void daPy_lk_c::freeGrabItem()
ext 023E3358 int daPy_lk_c::getDirectionFromAngle(s16 angle)
ext 023DC6A4 BOOL daPy_lk_c::resetActAnimeUpper(int upper, f32 morf)
ext 023DF9C0 void daPy_lk_c::resetFootEffect()
ext 023E0470 u32 daPy_lk_c::seStartMapInfo(u32 sound)
ext 023DC0F4 u32 daPy_lk_c::seStartOnlyReverb(u32 sound)
ext 023DE7E8 BOOL daPy_lk_c::setActAnimeUpper(u32 index, int upper, f32 rate, f32 start, int end, f32 morf)
ext 023DE788 void player02SetFrameCtrl(daPy_lk_c* player, J3DFrameCtrl* ctrl, u32 attr, int start, int end, f32 rate, f32 frame)
ext 023E063C void daPy_lk_c::setHandModel(int anm)
ext 023E07E4 void daPy_lk_c::setSeAnime(u32 anm, u32 heap, u32 frame)
ext 023E0A04 BOOL daPy_lk_c::setSingleMoveAnime(int anm, f32 rate, f32 start, int end, f32 morf)
ext 023DD768 void daPy_lk_c::setTextureAnime(u32 index, int mode)
ext 023DC110 void daPy_lk_c::setTextureAnimeResource(u32 anm, int mode)
ext 023DC43C void daPy_lk_c::setTextureScrollResource(u32 anm, int mode)

ext 025D6958 f64 player02TailDistanceXZ(fopAc_ac_c*, fopAc_ac_c*)
ext 025D9D0C void player02TailSetCarryNow(fopAc_ac_c*, BOOL)
ext 024EBD10 fopAc_ac_c* player02TailCatchTarget(void*, u32)
ext 023D794C BOOL daPy_lk_c::checkEquipAnime()
ext 023D85CC BOOL player02TailBottle(daPy_lk_c*, u32)
ext 023D85E4 BOOL player02TailPhotoBox(daPy_lk_c*, u32)
ext 023D8600 BOOL player02TailBow(daPy_lk_c*, u32)
ext 023DF0FC void daPy_lk_c::setBottleModel(u32)
ext 023DFAAC void daPy_lk_c::swimOutAfter(BOOL)
data l_cut_type 100356F0
ext 0255101C BOOL player02TailIsEsa(u32)
ext 025E1988 void player02TailSystemSound(u32)
ext 025D744C void player02TailOrderTalkEvent(daPy_lk_c*, fopAc_ac_c*)
ext 025D74B0 void player02TailOrderTalkX(daPy_lk_c*, fopAc_ac_c*)
ext 025D7514 void player02TailOrderTalkY(daPy_lk_c*, fopAc_ac_c*)
ext 025D7578 void player02TailOrderTalkZ(daPy_lk_c*, fopAc_ac_c*)
ext 025D75DC void player02TailOrderTalkHD(daPy_lk_c*, fopAc_ac_c*)
ext 0253EE04 u32 player02FirstEventPID(u32 control, u32 processID)
str 100356C0 "M_DragB" in 023EB6C8
str 100356C8 "Xboss0" in 023EB6C8
*/
