/* d_a_bk: translation-unit statics, constants and local bindings shared by the d_a_bk*.cpp
 * parts. */
#pragma once
#include "d/actor/d_a_bk.h"

#define SAFESTRING_VTBL 0x10009474 /* this TU's sead::SafeString vtable */

enum {
 /* BAS */
 dRes_INDEX_BK_BAS_BK_AOMUKE_e=0x8,
 dRes_INDEX_BK_BAS_BK_ATTACK1_e=0x9,
 dRes_INDEX_BK_BAS_BK_ATTACK2_e=0xA,
 dRes_INDEX_BK_BAS_BK_ATTACK3_e=0xB,
 dRes_INDEX_BK_BAS_BK_BOKKURI_e=0xC,
 dRes_INDEX_BK_BAS_BK_CATCH_e=0xD,
 dRes_INDEX_BK_BAS_BK_HAKKEN_e=0xE,
 dRes_INDEX_BK_BAS_BK_IATTACK1_e=0xF,
 dRes_INDEX_BK_BAS_BK_JATTACK2_e=0x10,
 dRes_INDEX_BK_BAS_BK_JATTACK3_e=0x11,
 dRes_INDEX_BK_BAS_BK_JUMP1_e=0x12,
 dRes_INDEX_BK_BAS_BK_JUMP2_e=0x13,
 dRes_INDEX_BK_BAS_BK_KERI1_e=0x14,
 dRes_INDEX_BK_BAS_BK_KERI2_e=0x15,
 dRes_INDEX_BK_BAS_BK_KYORO1_e=0x16,
 dRes_INDEX_BK_BAS_BK_KYORO2_e=0x17,
 dRes_INDEX_BK_BAS_BK_NIGERU_e=0x18,
 dRes_INDEX_BK_BAS_BK_NOBI_e=0x19,
 dRes_INDEX_BK_BAS_BK_NOMWAIT_e=0x1A,
 dRes_INDEX_BK_BAS_BK_OKIRUA_e=0x1B,
 dRes_INDEX_BK_BAS_BK_OKIRUU_e=0x1C,
 dRes_INDEX_BK_BAS_BK_OTISOU1_e=0x1D,
 dRes_INDEX_BK_BAS_BK_OTISOU2_e=0x1E,
 dRes_INDEX_BK_BAS_BK_RUN_e=0x1F,
 dRes_INDEX_BK_BAS_BK_SLEEP_e=0x20,
 dRes_INDEX_BK_BAS_BK_SUWARI_e=0x21,
 dRes_INDEX_BK_BAS_BK_TUTUKU1_e=0x22,
 dRes_INDEX_BK_BAS_BK_UTUBUSE_e=0x23,
 dRes_INDEX_BK_BAS_BK_WAIT_e=0x24,
 dRes_INDEX_BK_BAS_BK_WALK_e=0x25,
 dRes_INDEX_BK_BAS_BK_WALK2_e=0x26,
 /* BCK */
 dRes_INDEX_BK_BCK_BK_AOMUKE_e=0x29,
 dRes_INDEX_BK_BCK_BK_ATTACK1_e=0x2A,
 dRes_INDEX_BK_BCK_BK_ATTACK2_e=0x2B,
 dRes_INDEX_BK_BCK_BK_ATTACK3_e=0x2C,
 dRes_INDEX_BK_BCK_BK_BIKKURI_e=0x2D,
 dRes_INDEX_BK_BCK_BK_BOUGYO1_e=0x2E,
 dRes_INDEX_BK_BCK_BK_BOUGYO2_e=0x2F,
 dRes_INDEX_BK_BCK_BK_CATCH_e=0x30,
 dRes_INDEX_BK_BCK_BK_HAKKEN_e=0x31,
 dRes_INDEX_BK_BCK_BK_HAKOBI_e=0x32,
 dRes_INDEX_BK_BCK_BK_HIDARIROT_e=0x33,
 dRes_INDEX_BK_BCK_BK_JATTACK1_e=0x34,
 dRes_INDEX_BK_BCK_BK_JATTACK2_e=0x35,
 dRes_INDEX_BK_BCK_BK_JATTACK3_e=0x36,
 dRes_INDEX_BK_BCK_BK_JUMP1_e=0x37,
 dRes_INDEX_BK_BCK_BK_JUMP2_e=0x38,
 dRes_INDEX_BK_BCK_BK_KERI1_e=0x39,
 dRes_INDEX_BK_BCK_BK_KERI2_e=0x3A,
 dRes_INDEX_BK_BCK_BK_KIME_e=0x3B,
 dRes_INDEX_BK_BCK_BK_KOUKA_e=0x3C,
 dRes_INDEX_BK_BCK_BK_KYORO1_e=0x3D,
 dRes_INDEX_BK_BCK_BK_KYORO2_e=0x3E,
 dRes_INDEX_BK_BCK_BK_MIGIROT_e=0x3F,
 dRes_INDEX_BK_BCK_BK_NIGERU_e=0x40,
 dRes_INDEX_BK_BCK_BK_NOBI_e=0x41,
 dRes_INDEX_BK_BCK_BK_NOMWAIT_e=0x42,
 dRes_INDEX_BK_BCK_BK_NOZOKU_e=0x43,
 dRes_INDEX_BK_BCK_BK_OKIRUA_e=0x44,
 dRes_INDEX_BK_BCK_BK_OKIRUU_e=0x45,
 dRes_INDEX_BK_BCK_BK_OTISOU1_e=0x46,
 dRes_INDEX_BK_BCK_BK_OTISOU2_e=0x47,
 dRes_INDEX_BK_BCK_BK_RUN_e=0x48,
 dRes_INDEX_BK_BCK_BK_SLEEP_e=0x49,
 dRes_INDEX_BK_BCK_BK_SUWARI_e=0x4A,
 dRes_INDEX_BK_BCK_BK_TATAKU_e=0x4B,
 dRes_INDEX_BK_BCK_BK_TUTUKU1_e=0x4C,
 dRes_INDEX_BK_BCK_BK_TUTUKU2_e=0x4D,
 dRes_INDEX_BK_BCK_BK_TUTUKU3_e=0x4E,
 dRes_INDEX_BK_BCK_BK_TYAKU_e=0x4F,
 dRes_INDEX_BK_BCK_BK_UTUBUSE_e=0x50,
 dRes_INDEX_BK_BCK_BK_WAIT_e=0x51,
 dRes_INDEX_BK_BCK_BK_WALK_e=0x52,
 dRes_INDEX_BK_BCK_BK_WALK2_e=0x53,
 /* BDLM */
 dRes_INDEX_BK_BDL_BK_e=0x56,
 dRes_INDEX_BK_BDL_BOUEN_e=0x57,
 /* BMD */
 dRes_INDEX_BK_BMD_BK_KB_e=0x5A,
 dRes_INDEX_BK_BMD_BK_TATE_e=0x5B,
 /* BMT */
 dRes_INDEX_BK_BMT_BK_BOKO_e=0x5E,
 dRes_INDEX_BK_BMT_BK_KEN_e=0x5F,
 dRes_INDEX_BK_BMT_GREEN_e=0x60,
 dRes_INDEX_BK_BMT_PINK_e=0x61,
 /* BTP */
 dRes_INDEX_BK_BTP_TMABATAKI_e=0x64,
};
enum {
 BK_JNT_KOSI_e=0x0,
 BK_JNT_HIP1_e=0x1,
 BK_JNT_KOKAL_e=0x2,
 BK_JNT_MOMOL_e=0x3,
 BK_JNT_SUNEL1_e=0x4,
 BK_JNT_SUNEL2_e=0x5,
 BK_JNT_ASIL_e=0x6,
 BK_JNT_KOKAR_e=0x7,
 BK_JNT_MOMORR_e=0x8,
 BK_JNT_SUNER1_e=0x9,
 BK_JNT_SUNER2_e=0xA,
 BK_JNT_ASIR_e=0xB,
 BK_JNT_SIPPO1_e=0xC,
 BK_JNT_SIPPO2_e=0xD,
 BK_JNT_SIPPO3_e=0xE,
 BK_JNT_SIPPO4_e=0xF,
 BK_JNT_MUNE_e=0x10,
 BK_JNT_KUBI_e=0x11,
 BK_JNT_HEAD_e=0x12,
 BK_JNT_AGO_e=0x13,
 BK_JNT_BERO1_e=0x14,
 BK_JNT_KUTI1_e=0x15,
 BK_JNT_BANDA_e=0x16,
 BK_JNT_HANA_e=0x17,
 BK_JNT_HOHOL_e=0x18,
 BK_JNT_HOHOR_e=0x19,
 BK_JNT_MAYUL_e=0x1A,
 BK_JNT_MAYUR_e=0x1B,
 BK_JNT_MIMIL_e=0x1C,
 BK_JNT_MIIML2_e=0x1D,
 BK_JNT_MIMIR_e=0x1E,
 BK_JNT_MIMIR2_e=0x1F,
 BK_JNT_UDEL1_e=0x20,
 BK_JNT_UDEL2_e=0x21,
 BK_JNT_UDEL3_e=0x22,
 BK_JNT_HANDL_e=0x23,
 BK_JNT_UDELSAKI_e=0x24,
 BK_JNT_TATE_e=0x25,
 BK_JNT_YUBIL1_e=0x26,
 BK_JNT_YUBIL2_e=0x27,
 BK_JNT_UDER1_e=0x28,
 BK_JNT_UDER2_e=0x29,
 BK_JNT_UDER3_e=0x2A,
 BK_JNT_HANDR_e=0x2B,
 BK_JNT_BUKI_e=0x2C,
 BK_JNT_UDERSAKI_e=0x2D,
 BK_JNT_YUBIR1_e=0x2E,
 BK_JNT_YUBIR2_e=0x2F,
};
enum { BK_KB_JNT_BLURS_e = 0, BK_KB_JNT_BLURA_e = 1, BK_KB_JNT_BLURB_e = 2 };
enum { fpcNm_BOMB_e = 0x126, fpcNm_BOKO_e = 0x1CF };
enum { fopAcStts_CARRY_e = 0x2000 };

/* ---- file statics (.bss/.data) ---- */
static inline bkHIO_c& l_bkHIO() { return *gabi::at<bkHIO_c>(0x10462368); }
static inline be<u8>& hio_set() { return *gabi::at<be<u8>>(0x10191468); }
static inline be<s8>& search_sp() { return *gabi::at<be<s8>>(0x1046232D); }
static inline be<s32>& target_info_count() { return *gabi::at<be<s32>>(0x10462320); }
static inline gptr<fopAc_ac_c>* target_info() { return gabi::at<gptr<fopAc_ac_c>>(0x10462330); } /* [10] */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline bool fopAcM_checkCarryNow(fopAc_ac_c* a) { return (a->actor_status & fopAcStts_CARRY_e) != 0; }
/* virtual JPACallBackBase::remove (vtable slot +0x44) on dPa_smokeEcallBack / followEcallBack */
static inline void dPa_EcallBack_remove(void* cb) {
    u32 vt = gabi::load<u32>(gabi::ea(cb));
    gabi::call_ptr(gabi::load<u32>(vt + 0x44), cb);
}
static inline void enemy_fire_remove(enemyfire_l* f) { gabi::call(0x02041C30, f); }
static inline void mDoExt_McaMorf_stopZelAnime(mDoExt_McaMorf* m) { m->stopZelAnime(); }

/* dBgS_LinChk (stack object): HD layout (as in d_a_kamome), this TU's vtables */
struct dBgS_LinChk_l {
    /* 0x00 */ be<u32> mpPolyPassChk; /* -> +0x58 */
    /* 0x04 */ be<u32> mpGrpPassChk;  /* -> +0x64 */
    /* 0x08 */ u8 _08[8];
    /* 0x10 */ be<u32> __vtbl_10;
    /* 0x14 */ u8 _14[0x20 - 0x14];
    /* 0x20 */ be<u32> __vtbl_20;
    /* 0x24 */ u8 _24[0x58 - 0x24];
    /* 0x58 */ be<u32> __vtbl_58;     /* dBgS_PolyPassChk */
    /* 0x5C */ be<u8> mPass[7];
    /* 0x63 */ u8 _63;
    /* 0x64 */ be<u32> __vtbl_64;     /* dBgS_GrpPassChk */
    /* 0x68 */ be<u32> mGrp;
};
WWHD_SIZE(dBgS_LinChk_l, 0x6C);
static inline void dBgS_LinChk_ct(dBgS_LinChk_l* c) {
    cBgS_LinChk_ct(c);
    for (int i = 0; i < 7; i++) c->mPass[i] = 0;
    c->mGrp = 1;
    c->mpGrpPassChk = gabi::ea(c) + 0x64;
    c->mpPolyPassChk = gabi::ea(c) + 0x58;
    c->__vtbl_10 = 0x1000952C;
    c->__vtbl_64 = 0x1000954C;
    c->__vtbl_58 = 0x1000955C;
    c->__vtbl_20 = 0x1000953C;
}
static inline void dBgS_LinChk_dt(dBgS_LinChk_l* c) {
    c->__vtbl_58 = 0x1000955C;
    c->__vtbl_64 = 0x100094AC;
    c->__vtbl_20 = 0x1000949C;
    cBgS_LinChk_dt(c, 0);
}
static inline void dBgS_LinChk_Set_l(dBgS_LinChk_l* c, cXyz* s, cXyz* e, fopAc_ac_c* a) { dBgS_LinChk_Set(c, s, e, a); }
static inline BOOL LineCross(dBgS_LinChk_l* c) { return cBgS_LineCross(dComIfG_Bgsp(), c); }

/* player (daPy_py_c): fields read here */
static inline f32 daPy_getGrabWearTimer(fopAc_ac_c* p) { return gabi::load<f32>(gabi::ea(p) + 0x3CC); } /* checkGrabWear: < 0 */

/* the TU's functions: declared here (natural calls between the parts go through WWHD_FUNC);
 * functions not yet decompiled are defined as plain guest calls in d_a_bk_pending.cpp */
void anm_init(bk_class* i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 speed, int soundFileIdx);
void smoke_set_s(bk_class* i_this, f32 rate);
void ground_smoke_set(bk_class* i_this);
void way_pos_check(bk_class* i_this, cXyz* r31);
u8 ground_4_check(bk_class* i_this, int r18, s16 r20, f32 f29);
BOOL daBk_other_bg_check(bk_class* i_this, fopAc_ac_c* r23);
BOOL daBk_wepon_view_check(bk_class* i_this); /* 0209AB64 (search_wepon inlined) */
fopAc_ac_c* search_bomb(bk_class* i_this, BOOL r26);
BOOL daBk_bomb_view_check(bk_class* i_this);
BOOL daBk_player_bg_check(bk_class* i_this, cXyz* r22);
BOOL daBk_player_view_check(bk_class* i_this, cXyz* r30, s16 r27, s16 r31);
BOOL daBk_player_way_check(bk_class* i_this);
void wait_set(bk_class* i_this);
void path_check(bk_class* i_this, u8 r19);
void attack_set(bk_class* i_this, u8 r28);
void tate_mtx_set(bk_class* i_this);
void bou_mtx_set(bk_class* i_this);
void fight_run(bk_class* i_this);
void fight(bk_class* i_this);
void p_lost(bk_class* i_this);
void b_nige(bk_class* i_this);
void defence(bk_class* i_this);
void oshi(bk_class* i_this);
void hukki(bk_class* i_this);
void aite_miru(bk_class* i_this);
void fail(bk_class* i_this);
void yogan_fail(bk_class* i_this);
void water_fail(bk_class* i_this);
void wepon_search(bk_class* i_this);
void d_dozou(bk_class* i_this);
void carry_drop(bk_class* i_this);
void d_mahi(bk_class* i_this);
void tubo_wait(bk_class* i_this);
void z_demo_1(bk_class* i_this);
void b_hang(bk_class* i_this);
void rope_on(bk_class* i_this);
void Bk_move(bk_class* i_this);
BOOL daBk_Execute(bk_class* i_this);
BOOL useHeapInit(fopAc_ac_c* i_actor);
