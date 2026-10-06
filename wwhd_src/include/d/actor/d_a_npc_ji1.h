/* daNpc_Ji1_c (Orca), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the constructor (0224D280, inlined in _create) and the verified
 * functions:
 * - fopEn_enemy_c ends at 0x3C8 (GameCube 0x2AC + 0x11C).
 * - The pointers to member functions (Action_t, SubAction_t) are 8 bytes (GHS) instead of 12, so
 *   mSmokeCb is at 0x3EC (+0x10C after the four PTMFs).
 * - dPa_smokeEcallBack is 0x20 in both; mDoExt_btpAnm grew to 0x74, mDoExt_brkAnm to 0x78,
 *   mDoExt_btkAnm to 0x74; mShadowId is gone (HD: no blob shadow).
 * - From the colliders on everything is +0x228 (mAcch 0x65C, the dCcD_Cps at 0xC68), except
 *   pad_0xC20 (gone), so field_0xC24..field_0xC3C are +0x224, and a new HD byte at 0xE64.
 * Size 0xFB0 (GameCube 0xD88). */
#pragma once
#include "bindings.h"

/* GHS pointer to member function (8 bytes): this adjustment, virtual index (0: null, < 0: not
 * virtual), then the function address, or (virtual) the vtable pointer's offset at +6 */
struct ProcFunc_l {
    /* 0x0 */ be<s16> d;
    /* 0x2 */ be<s16> i;
    /* 0x4 */ be<u32> f;
};
WWHD_SIZE(ProcFunc_l, 8);

/* dPa_smokeEcallBack (HD 0x20): vtable, emitter, ... */
struct dPa_smokeEcallBack_l {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ gptr<JPABaseEmitter> mpEmitter;
    /* 0x08 */ u8 _08[0x20 - 0x08];
};
WWHD_SIZE(dPa_smokeEcallBack_l, 0x20);
/* mDoExt_btpAnm (HD 0x74), mDoExt_brkAnm (HD 0x78): J3DFrameCtrl first */
struct mDoExt_btpAnm_l {
    /* 0x00 */ J3DFrameCtrl mFrameCtrl;
    /* 0x10 */ u8 _10[0x74 - 0x10];
};
struct mDoExt_brkAnm_l {
    /* 0x00 */ J3DFrameCtrl mFrameCtrl;
    /* 0x10 */ u8 _10[0x78 - 0x10];
};
WWHD_SIZE(mDoExt_brkAnm_l, 0x78);
struct Quaternion_l {
    be<f32> x, y, z, w;
};

struct daNpc_Ji1_c : fopEn_enemy_c {
    /* methods (GameCube names) */
    BOOL isGuardAnim();
    BOOL isAttackAnim();
    int isAttackFrame();
    BOOL isItemWaitAnim();
    BOOL isClearRecord(s16);
    void setClearRecord(s16);
    void normalSubActionHarpoonGuard(s16);
    void normalSubActionGuard(s16);
    BOOL normalAction(void*);
    BOOL kaitenExpAction(void*);
    BOOL kaitenspeakAction(void*);
    BOOL kaitenwaitAction(void*);
    BOOL kaitenAction(void*);
    u32 getMsg1stType();
    u32 getMsg2ndType();
    u32 getMsg();
    u16 next_msgStatus(be<u32>*);
    BOOL talkAction(void*);
    BOOL speakAction(void*);
    BOOL speakBadAction(void*);
    static void* initPosObject(void*, void*);
    void initPos(int);
    void createItem();
    void set_mtx();
    s32 getEventActionNo(int);
    BOOL eventAction(void*);
    u32 evn_init_pos_init(int);
    u32 evn_setAnm_init(int);
    u32 evn_talk_init(int);
    u32 evn_talk();
    u32 evn_continue_talk_init(int);
    u32 evn_continue_talk();
    u32 evn_setAngle_init(int);
    u32 evn_sound_proc_init(int);
    u32 evn_head_swing_init(int);
    u32 evn_harpoon_proc_init(int);
    u32 evn_RollAtControl_init(int);
    u32 evn_RollAtControl();
    u32 evn_game_mode_init(int);
    u32 evn_turn_to_player();
    u32 evn_hide_init(int);
    void AnimeControlToWait();
    u32 privateCut();
    u32 setParticle(int, f32, f32);
    void dtParticle();
    u32 setParticleAT(int, f32, f32);
    void dtParticleAT();
    BOOL startspeakAction(void*);
    BOOL endspeakAction(void*);
    BOOL reiAction(void*);
    BOOL plmoveAction(void*);
    BOOL teachMove(f32);
    BOOL teachSpRollCutMove(f32);
    f32 calcCoCorrectValue();
    f32 calcBgCorrectValue();
    BOOL MoveToPlayer(f32, u8);
    void teachSubActionAttackInit();
    BOOL teachSubActionAttack();
    void teachSubActionJumpInit();
    BOOL teachSubActionJump();
    BOOL teachAction(void*);
    BOOL teachSPRollCutAction(void*);
    void battleGameSetTimer();
    BOOL battleMove(f32);
    void battleSubActionWaitInit();
    BOOL battleSubActionWait();
    void battleSubActionNockBackInit(int);
    BOOL battleSubActionNockBack();
    void battleSubActionAttackInit();
    BOOL battleSubActionAttack();
    void battleSubActionTateAttackInit();
    BOOL battleSubActionTateAttack();
    void battleSubActionYokoAttackInit();
    BOOL battleSubActionYokoAttack();
    void battleSubActionJumpInit();
    BOOL battleSubActionJump();
    void battleSubActionDamageInit();
    BOOL battleSubActionDamage();
    void battleSubActionJpGuardInit();
    BOOL battleSubActionJpGuard();
    void battleSubActionGuardInit();
    BOOL battleSubActionGuard();
    BOOL battleAtSet();
    BOOL battleGuardCheck();
    BOOL battleAction(void*);
    BOOL checkCutType(int, int);
    void setAnimFromMsgNo(u32);
    BOOL setAnm(int, f32, int);
    cPhs_State _create();
    BOOL CreateHeap();
    BOOL CreateInit();
    BOOL _delete();
    BOOL _execute();
    BOOL _draw();
    BOOL chkAttention(cXyz*, s16); /* cXyz by value: pointer to a copy */
    BOOL lookBack();
    void setHitParticle(cXyz*, u32);
    void setGuardParticle();
    void BackSlideInit();
    void BackSlide(f32, f32);
    void harpoonRelease(cXyz*);
    void harpoonMove();

    /* 0x3C8 */ be<u32> mMsgNo;
    /* 0x3CC */ ProcFunc_l mAction;
    /* 0x3D4 */ ProcFunc_l field_0x2BC;
    /* 0x3DC */ ProcFunc_l field_0x2C8;
    /* 0x3E4 */ ProcFunc_l mSubAction;
    /* 0x3EC */ dPa_smokeEcallBack_l mSmokeCb;
    /* 0x40C */ dPa_smokeEcallBack_l mSmokeCbAT;
    /* 0x42C */ cXyz field_0x320;
    /* 0x438 */ be<s16> field_0x32C;
    /* 0x43A */ u8 _43A[2];
    /* 0x43C */ gptr<mDoExt_McaMorf> mpOrcaMorf;
    /* 0x440 */ dNpc_EventCut_c mEventCut;
    /* 0x4AC */ dNpc_JntCtrl_c m_jnt;
    /* 0x4E0 */ gptr<J3DAnmTexPattern> headTexPattern;
    /* 0x4E4 */ mDoExt_btpAnm_l mBlinkAnim;
    /* 0x558 */ be<u8> mBlinkFrame;
    /* 0x559 */ u8 _559;
    /* 0x55A */ be<s16> mBlinkTimer;
    /* 0x55C */ gptr<mDoExt_McaMorf> mpSpearMorf;     /* HD: no mShadowId before it */
    /* 0x560 */ gptr<J3DModel> mpTearsModel;
    /* 0x564 */ mDoExt_brkAnm_l mCryBrk;
    /* 0x5DC */ be<f32> mCryBrkFrame;
    /* 0x5E0 */ mDoExt_btkAnm mCryBtk;
    /* 0x654 */ be<f32> mCryBtkFrame;
    /* 0x658 */ gptr<JPABaseEmitter> field_0x430;
    /* 0x65C */ dBgS_ObjAcch mAcch;
    /* 0x820 */ dBgS_AcchCir mAcchCir;
    /* 0x860 */ dCcD_Stts field_0x638;
    /* 0x89C */ dCcD_Stts field_0x674;
    /* 0x8D8 */ dCcD_Cyl field_0x6B0;
    /* 0xA08 */ dCcD_Cyl field_0x7E0;
    /* 0xB38 */ dCcD_Cyl field_0x910;
    /* 0xC68 */ dCcD_Cps field_0xA40;
    /* 0xDA0 */ cXyz field_0xB78;
    /* 0xDAC */ cXyz field_0xB84;
    /* 0xDB8 */ cXyz field_0xB90;
    /* 0xDC4 */ be<s8> handRJointNo;
    /* 0xDC5 */ be<s8> hair1JointNo;
    /* 0xDC6 */ be<s8> hair2JointNo;
    /* 0xDC7 */ be<s8> hair3JointNo;
    /* 0xDC8 */ be<s8> armLJointNo;
    /* 0xDC9 */ be<s8> armRJointNo;
    /* 0xDCA */ be<s16> field_0xBA2;
    /* 0xDCC */ be<s16> field_0xBA4;
    /* 0xDCE */ be<s16> field_0xBA6;
    /* 0xDD0 */ be<s16> field_0xBA8;
    /* 0xDD2 */ be<s16> field_0xBAA;
    /* 0xDD4 */ be<s16> field_0xBAC;
    /* 0xDD6 */ be<s16> field_0xBAE;
    /* 0xDD8 */ be<s16> field_0xBB0;
    /* 0xDDA */ be<s16> field_0xBB2;
    /* 0xDDC */ be<s16> field_0xBB4;
    /* 0xDDE */ be<s16> field_0xBB6;
    /* 0xDE0 */ be<s16> field_0xBB8;
    /* 0xDE2 */ be<s16> field_0xBBA;
    /* 0xDE4 */ be<s16> field_0xBBC;
    /* 0xDE6 */ be<s16> field_0xBBE;
    /* 0xDE8 */ be<s16> field_0xBC0;
    /* 0xDEA */ be<s16> field_0xBC2;
    /* 0xDEC */ cXyz field_0xBC4;
    /* 0xDF8 */ be<s16> field_0xBD0;
    /* 0xDFA */ be<s16> field_0xBD2;
    /* 0xDFC */ be<s16> field_0xBD4;
    /* 0xDFE */ be<s16> field_0xBD6;
    /* 0xE00 */ cXyz field_0xBD8[3];
    /* 0xE24 */ u8 mHeadAnm[0x24];             /* dNpc_HeadAnm_c */
    /* 0xE48 */ be<s32> field_0xC24;           /* HD: pad_0xC20 gone */
    /* 0xE4C */ be<u32> field_0xC28;
    /* 0xE50 */ be<s32> field_0xC2C;
    /* 0xE54 */ be<s32> field_0xC30;
    /* 0xE58 */ be<s32> field_0xC34;
    /* 0xE5C */ be<s32> field_0xC38;
    /* 0xE60 */ be<s32> field_0xC3C;
    /* 0xE64 */ be<u8> mHD_E64;                /* HD only */
    /* 0xE65 */ u8 _E65[3];
    /* 0xE68 */ cXyz field_0xC40;
    /* 0xE74 */ be<f32> field_0xC4C;
    /* 0xE78 */ be<f32> field_0xC50;
    /* 0xE7C */ be<s16> mEventIdx[0x12];
    /* 0xEA0 */ be<s8> field_0xC78;
    /* 0xEA1 */ u8 _EA1[3];
    /* 0xEA4 */ request_of_phase_process_class mPhs;
    /* 0xEAC */ be<u32> field_0xC84;
    /* 0xEB0 */ be<s16> field_0xC88;
    /* 0xEB2 */ u8 _EB2[2];
    /* 0xEB4 */ be<s32> field_0xC8C;
    /* 0xEB8 */ be<s32> field_0xC90;
    /* 0xEBC */ be<s32> field_0xC94;
    /* 0xEC0 */ be<s32> field_0xC98;
    /* 0xEC4 */ be<f32> field_0xC9C;
    /* 0xEC8 */ Mtx34 field_0xCA0;
    /* 0xEF8 */ cXyz field_0xCD0;
    /* 0xF04 */ cXyz field_0xCDC;
    /* 0xF10 */ u8 pad_0xCE8[0xC];
    /* 0xF1C */ Quaternion_l field_0xCF4;
    /* 0xF2C */ Quaternion_l field_0xD04;
    /* 0xF3C */ be<s16> field_0xD14;
    /* 0xF3E */ be<s16> field_0xD16;
    /* 0xF40 */ be<f32> field_0xD18;
    /* 0xF44 */ cXyz field_0xD1C;
    /* 0xF50 */ cXyz field_0xD28;
    /* 0xF5C */ be<s32> field_0xD34;
    /* 0xF60 */ cXyz field_0xD38;
    /* 0xF6C */ u8 pad_0xD44[0xC];
    /* 0xF78 */ cXyz field_0xD50;
    /* 0xF84 */ csXyz field_0xD5C;
    /* 0xF8A */ u8 _F8A[2];
    /* 0xF8C */ be<s32> mAnimation;
    /* 0xF90 */ be<s32> field_0xD68;
    /* 0xF94 */ be<s32> field_0xD6C;
    /* 0xF98 */ be<s32> field_0xD70;
    /* 0xF9C */ be<s32> field_0xD74;
    /* 0xFA0 */ be<u8> field_0xD78;
    /* 0xFA1 */ be<u8> field_0xD79;
    /* 0xFA2 */ be<u8> field_0xD7A;
    /* 0xFA3 */ be<u8> field_0xD7B;
    /* 0xFA4 */ be<u8> field_0xD7C;
    /* 0xFA5 */ be<u8> mCreateItemNo;
    /* 0xFA6 */ be<u8> field_0xD7E;
    /* 0xFA7 */ u8 _FA7;
    /* 0xFA8 */ be<u32> mEndMsgNo;
    /* 0xFAC */ be<u8> field_0xD84;
    /* 0xFAD */ be<u8> mHide;
    /* 0xFAE */ u8 _FAE[2];
};
WWHD_OFFSET(daNpc_Ji1_c, mAction, 0x3CC);
WWHD_OFFSET(daNpc_Ji1_c, mSubAction, 0x3E4);
WWHD_OFFSET(daNpc_Ji1_c, mpOrcaMorf, 0x43C);
WWHD_OFFSET(daNpc_Ji1_c, m_jnt, 0x4AC);
WWHD_OFFSET(daNpc_Ji1_c, mCryBrk, 0x564);
WWHD_OFFSET(daNpc_Ji1_c, mCryBtk, 0x5E0);
WWHD_OFFSET(daNpc_Ji1_c, mAcch, 0x65C);
WWHD_OFFSET(daNpc_Ji1_c, mAcchCir, 0x820);
WWHD_OFFSET(daNpc_Ji1_c, field_0x638, 0x860);
WWHD_OFFSET(daNpc_Ji1_c, field_0xA40, 0xC68);
WWHD_OFFSET(daNpc_Ji1_c, field_0xB78, 0xDA0);
WWHD_OFFSET(daNpc_Ji1_c, mHeadAnm, 0xE24);
WWHD_OFFSET(daNpc_Ji1_c, field_0xC24, 0xE48);
WWHD_OFFSET(daNpc_Ji1_c, field_0xC40, 0xE68);
WWHD_OFFSET(daNpc_Ji1_c, mEventIdx, 0xE7C);
WWHD_OFFSET(daNpc_Ji1_c, field_0xC78, 0xEA0);
WWHD_OFFSET(daNpc_Ji1_c, mPhs, 0xEA4);
WWHD_OFFSET(daNpc_Ji1_c, field_0xCA0, 0xEC8);
WWHD_OFFSET(daNpc_Ji1_c, field_0xD5C, 0xF84);
WWHD_OFFSET(daNpc_Ji1_c, mAnimation, 0xF8C);
WWHD_OFFSET(daNpc_Ji1_c, mEndMsgNo, 0xFA8);
WWHD_SIZE(daNpc_Ji1_c, 0xFB0);

/* daNpc_Ji1_HIO_c, HD: the vtable pointer follows the members (0xFC), so every GameCube offset
 * is -4. l_HIO is at 0x104672AC. Names are the GameCube offsets. */
struct daNpc_Ji1_HIO_c {
    /* 0x000 */ be<s8> mNo;
    /* 0x001 */ u8 _001[3];
    /* 0x004 */ be<f32> field_0x08;
    /* 0x008 */ be<s16> field_0x0C;
    /* 0x00A */ be<s16> field_0x0E;
    /* 0x00C */ be<s16> field_0x10;
    /* 0x00E */ be<s16> field_0x12;
    /* 0x010 */ be<s16> field_0x14;
    /* 0x012 */ be<s16> field_0x16;
    /* 0x014 */ be<s16> field_0x18;
    /* 0x016 */ be<u8> field_0x1A;
    /* 0x017 */ u8 _017;
    /* 0x018 */ be<f32> field_0x1C;
    /* 0x01C */ be<f32> field_0x20;
    /* 0x020 */ be<f32> field_0x24;
    /* 0x024 */ be<u8> field_0x28;
    /* 0x025 */ u8 _025[3];
    /* 0x028 */ be<f32> field_0x2C;
    /* 0x02C */ be<u8> field_0x30;
    /* 0x02D */ u8 _02D[3];
    /* 0x030 */ be<f32> field_0x34;
    /* 0x034 */ be<f32> field_0x38;
    /* 0x038 */ be<f32> field_0x3C;
    /* 0x03C */ be<f32> field_0x40;
    /* 0x040 */ be<f32> field_0x44;
    /* 0x044 */ be<f32> field_0x48;
    /* 0x048 */ be<f32> field_0x4C;
    /* 0x04C */ be<f32> field_0x50;
    /* 0x050 */ be<s16> field_0x54[6];
    /* 0x05C */ be<s16> field_0x60[4];
    /* 0x064 */ be<u8> field_0x68;
    /* 0x065 */ u8 _065[3];
    /* 0x068 */ be<f32> field_0x6C;
    /* 0x06C */ be<f32> field_0x70;
    /* 0x070 */ be<f32> field_0x74;
    /* 0x074 */ be<f32> field_0x78;
    /* 0x078 */ be<f32> field_0x7C;
    /* 0x07C */ be<f32> field_0x80;
    /* 0x080 */ be<f32> field_0x84;
    /* 0x084 */ be<s16> field_0x88;
    /* 0x086 */ be<s16> field_0x8A;
    /* 0x088 */ be<s16> field_0x8C;
    /* 0x08A */ be<s16> field_0x8E;
    /* 0x08C */ be<s16> field_0x90;
    /* 0x08E */ be<s16> field_0x92;
    /* 0x090 */ be<s16> field_0x94;
    /* 0x092 */ be<s16> field_0x96;
    /* 0x094 */ be<s16> field_0x98;
    /* 0x096 */ be<s16> field_0x9A;
    /* 0x098 */ be<s16> field_0x9C;
    /* 0x09A */ be<s16> field_0x9E;
    /* 0x09C */ be<s16> field_0xA0;
    /* 0x09E */ be<u8> field_0xA2;
    /* 0x09F */ u8 _09F;
    /* 0x0A0 */ be<f32> field_0xA4;
    /* 0x0A4 */ be<f32> field_0xA8;
    /* 0x0A8 */ be<f32> field_0xAC;
    /* 0x0AC */ be<f32> field_0xB0;
    /* 0x0B0 */ be<f32> field_0xB4;
    /* 0x0B4 */ be<u8> field_0xB8;
    /* 0x0B5 */ u8 _0B5[3];
    /* 0x0B8 */ be<f32> field_0xBC;
    /* 0x0BC */ be<f32> field_0xC0;
    /* 0x0C0 */ cXyz field_0xC4[3];
    /* 0x0E4 */ be<s16> field_0xE8;
    /* 0x0E6 */ be<s16> field_0xEA;
    /* 0x0E8 */ be<s16> field_0xEC;
    /* 0x0EA */ be<s16> field_0xEE;
    /* 0x0EC */ be<s16> field_0xF0;
    /* 0x0EE */ be<s16> field_0xF2;
    /* 0x0F0 */ be<s16> field_0xF4;
    /* 0x0F2 */ be<s16> field_0xF6;
    /* 0x0F4 */ be<s16> field_0xF8;
    /* 0x0F6 */ be<s16> field_0xFA;
    /* 0x0F8 */ be<s16> field_0xFC;
    /* 0x0FA */ be<s16> field_0xFE;
    /* 0x0FC */ be<u32> __vtbl;
};
WWHD_OFFSET(daNpc_Ji1_HIO_c, field_0x60, 0x5C);
WWHD_OFFSET(daNpc_Ji1_HIO_c, field_0xC4, 0xC0);
WWHD_SIZE(daNpc_Ji1_HIO_c, 0x100);

/* ---- free functions of the translation unit ---- */
s16 daNpc_Ji1_XyCheckCB(void*, int);                                                 /* 02248C64 */
void daJi1_CoHitCallback(fopAc_ac_c*, dCcD_GObjInf*, fopAc_ac_c*, dCcD_GObjInf*);   /* 02248CA4 */
void daJi1_TgHitCallback(fopAc_ac_c*, dCcD_GObjInf*, fopAc_ac_c*, dCcD_GObjInf*);   /* 02248CF0 */
void daJi1_AtHitCallback(fopAc_ac_c*, dCcD_GObjInf*, fopAc_ac_c*, dCcD_GObjInf*);   /* 02248EA0 */
BOOL daNpc_Ji1_plRoomOutCheck();                                                     /* 02248F58 */
u32 playerCutAtCheck();                                                              /* 02249058 */
BOOL nodeCallBack1(J3DNode*, int);                                                   /* 02249084 */
BOOL nodeCallBack2(J3DNode*, int);                                                   /* 02249224 */
BOOL nodeCallBack3(J3DNode*, int);                                                   /* 022494C8 */

/* ---- this TU's constants and statics ---- */
#define SAFESTRING_VTBL 0x1001AE20 /* this TU's sead::SafeString vtable */
static inline daNpc_Ji1_HIO_c& l_HIO() { return *gabi::at<daNpc_Ji1_HIO_c>(0x104672AC); }
static inline be<u8>& game_life_point() { return *gabi::at<be<u8>>(0x101D5F44); } /* daNpc_Ji1_c::game_life_point */

/* ---- pointers to member functions (GHS, 8 bytes) ---- */
/* `if (p)`: the virtual index is 0 only for a null PTMF */
static inline bool ptmf_nonnull(ProcFunc_l& p) { return p.i != 0; }
/* `p == &daNpc_Ji1_c::fn` for a non-virtual fn */
static inline bool ptmf_eq(ProcFunc_l& p, u32 fn) { return p.i == -1 && p.d == 0 && p.f == fn; }
static inline void ptmf_set(ProcFunc_l& p, u32 fn) {
    p.d = 0;
    p.i = -1;
    p.f = fn;
}
static inline void ptmf_copy(ProcFunc_l& dst, ProcFunc_l& src) {
    dst.d = (s16)src.d;
    dst.i = (s16)src.i;
    dst.f = (u32)src.f;
}
/* (self->*p)(args...) */
template <class... A> static inline BOOL ptmf_invoke(ProcFunc_l& p, void* self, A... a) {
    s16 d = p.d;
    s16 i = p.i;
    void* obj = gabi::at<void>(gabi::ea(self) + d);
    if (i < 0) return gabi::call_ptr<BOOL>(p.f, obj, a...);
    u32 vt = gabi::load<u32>(gabi::ea(obj) + gabi::load<s16>(gabi::ea(&p) + 6));
    return gabi::call_ptr<BOOL>(gabi::load<u32>(vt + i * 8 + 4), obj, a...);
}

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* daPy_getPlayerActorClass(): the play object's player 0 */
static inline fopAc_ac_c* daPy_getPlayerActorClass() { return dComIfGp_getPlayer(0); }
/* save events: dSv_event_c at *(0x101F84DC) + 0x644 (re-read at each use) */
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
static inline u8 dComIfGs_getEventReg(u16 r) { return dSv_event_getEventReg(dComIfGs_event(), r); }
static inline void dComIfGs_setEventReg(u16 r, u8 v) { dSv_event_setEventReg(dComIfGs_event(), r, v); }
/* fopAcM_seStart on this actor: GHS drops the inline's NULL checks (this and &eyePos non-NULL) */
static inline void ji1_seStart(fopAc_ac_c* a, u32 id, u32 param = 0) {
    mDoAud_seStart(id, &a->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
/* attention_info.flags (+0x39C), distances[] (+0x388) */
static inline be<u32>& attn_flags(fopAc_ac_c* a) { return *gabi::at<be<u32>>(gabi::ea(a) + 0x39C); }
static inline be<u8>& attn_distance(fopAc_ac_c* a, int i) { return *gabi::at<be<u8>>(gabi::ea(a) + 0x388 + i); }
/* eventInfo (+0xF8): mCommand u16 +0xF8, mCondition u16 +0xFA, XyCheckCB +0x104 */
static inline u16 eventInfo_command(fopAc_ac_c* a) { return gabi::load<u16>(gabi::ea(a) + 0xF8); }
static inline bool eventInfo_checkCommandTalk(fopAc_ac_c* a) { return eventInfo_command(a) == 1; }
static inline void eventInfo_setXyCheckCB(fopAc_ac_c* a, u32 cb) { gabi::store<u32>(gabi::ea(a) + 0x104, cb); }

/* setAction (inline in the GameCube header) */
static inline void ji1_setAction(daNpc_Ji1_c* i_this, u32 action, void* arg) {
    if (!ptmf_eq(i_this->mAction, action)) {
        if (ptmf_nonnull(i_this->mAction)) {
            i_this->field_0xC78 = -1;
            ptmf_invoke(i_this->mAction, i_this, arg);
        }
        ptmf_copy(i_this->field_0x2BC, i_this->mAction);
        ptmf_set(i_this->mAction, action);
        i_this->field_0xC78 = 0;
        gabi::call_ptr<BOOL>(i_this->mAction.f, i_this, arg);
    }
}
static inline void ji1_setSubAction(daNpc_Ji1_c* i_this, u32 sub) { ptmf_set(i_this->mSubAction, sub); }

/* member function addresses (for PTMFs) */
enum : u32 {
    ACT_normalAction = 0x02250A1C,
    ACT_kaitenExpAction = 0x02251690,
    ACT_kaitenspeakAction = 0x02251424,
    ACT_kaitenwaitAction = 0x02250D54,
    ACT_kaitenAction = 0x02251ED0,
    ACT_talkAction = 0x0225268C,
    ACT_speakAction = 0x02252FD0,
    ACT_speakBadAction = 0x02254A64,
    ACT_eventAction = 0x022596CC,
    ACT_startspeakAction = 0x02253880,
    ACT_endspeakAction = 0x02253EE8,
    ACT_reiAction = 0x02254434,
    ACT_plmoveAction = 0x02257C9C,
    ACT_teachAction = 0x02255908,
    ACT_teachSPRollCutAction = 0x02256F1C,
    ACT_battleAction = 0x0225AEC8,
    SUB_teachSubActionAttack = 0x02255654,
    SUB_teachSubActionJump = 0x022554D0,
    SUB_battleSubActionWait = 0x0225B344,
    SUB_battleSubActionNockBack = 0x0225C208,
    SUB_battleSubActionAttack = 0x0225BF44,
    SUB_battleSubActionTateAttack = 0x0225B85C,
    SUB_battleSubActionYokoAttack = 0x0225BBA4,
    SUB_battleSubActionJump = 0x0225C358,
    SUB_battleSubActionDamage = 0x0225CA04,
    SUB_battleSubActionJpGuard = 0x0225C660,
    SUB_battleSubActionGuard = 0x0225C7EC,
};
