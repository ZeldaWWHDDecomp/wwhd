/**
 * d_a_npc_ji1.cpp (WWHD)
 * NPC - Orca
 *
 * Ported from the GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ji1.cpp) to the WWHD layout and verified against the WWHD code (cking.rpx).
 * The large action functions are in d_a_npc_ji1_<part>.cpp; functions not decompiled yet are
 * weak guest calls in d_a_npc_ji1_pending.cpp.
 */
#include "d/actor/d_a_npc_ji1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 0255F554 dKy_SordFlush_set(cXyz pos (by value: pointer to a copy), int) */
static inline void dKy_SordFlush_set(cXyz* pos, s32 p) { gabi::call(0x0255F554, pos, p); }
/* 025A5F88 dPa_smokeEcallBack::end */
static inline void smoke_end(dPa_smokeEcallBack_l* cb) { gabi::call(0x025A5F88, cb); }
/* dComIfGp_getSelectItem(i): the play object's select item bytes (play+0x5BBB) */
static inline u8 dComIfGp_getSelectItem(int i) { return gabi::load<u8>(dComIfGp_ea() + i + 0x5BBB); }
/* dComIfGp_getDoStatus(): play+0x5BB7 */
static inline u8 dComIfGp_getDoStatus() { return gabi::load<u8>(dComIfGp_ea() + 0x5BB7); }
/* fopAcM_Search (fpcM_Search) */
static inline void* fopAcM_Search(u32 fn, void* data) { return fpcM_Search(fn, data); }

enum { fpcNm_PLAYER_e = 0xA8 };
enum { dItemNo_KNIGHTS_CREST_e = 0x48 };
enum { dActStts_OPEN_e = 0xB };
enum {
    JA_SE_CV_JI_ATTACK = 0x4843,
    JA_SE_CV_JI_DEFENCE = 0x4833,
    JA_SE_OBJ_COL_SWS_NMTLP = 0x6817,
};
enum { dPa_ID_AK_JN_NG = 0xC };

/* 02248C64 */
s16 daNpc_Ji1_XyCheckCB(void* p, int i_itemBtn) {
    WWHD_FUNC(0x02248C64, s16, p, i_itemBtn);
    return dComIfGp_getSelectItem(i_itemBtn) == dItemNo_KNIGHTS_CREST_e ? TRUE : FALSE;
}
VERIFY(0x02248C64, daNpc_Ji1_XyCheckCB);

/* 02248CA4 */
void daJi1_CoHitCallback(fopAc_ac_c* i_this, dCcD_GObjInf* p2, fopAc_ac_c* actor, dCcD_GObjInf* p4) {
    WWHD_FUNC(0x02248CA4, void, i_this, p2, actor, p4);
    if (actor == dComIfGp_getPlayer(0)) {
        static_cast<daNpc_Ji1_c*>(i_this)->field_0xC28 = 1;
    }
}
VERIFY(0x02248CA4, daJi1_CoHitCallback);

/* 02248CF0 */
void daJi1_TgHitCallback(fopAc_ac_c* i_this, dCcD_GObjInf* p2, fopAc_ac_c* actor, dCcD_GObjInf* p4) {
    WWHD_FUNC(0x02248CF0, void, i_this, p2, actor, p4);
    /* daPy_getPlayerActorClass()->getCutType(): the player's u8 at +0x3AC */
    u32 type = gabi::load<u8>(gabi::ea(daPy_getPlayerActorClass()) + 0x3AC);
    if (actor) {
        static_cast<daNpc_Ji1_c*>(i_this)->field_0xC24 = type;
    }
}
VERIFY(0x02248CF0, daJi1_TgHitCallback);

/* 02248EA0 HD: a hit counts once per 10-frame window (mHD_E64) */
void daJi1_AtHitCallback(fopAc_ac_c* i_this, dCcD_GObjInf* param_2, fopAc_ac_c* actor, dCcD_GObjInf* p4) {
    WWHD_FUNC(0x02248EA0, void, i_this, param_2, actor, p4);
    if (fopAc_IsActor(actor) && actor != nullptr) {
        if (fpcM_GetName(actor) == fpcNm_PLAYER_e) {
            daNpc_Ji1_c* a = static_cast<daNpc_Ji1_c*>(i_this);
            if (a->mHD_E64 == 0) {
                a->mHD_E64 = 10;
                /* ChkAtShieldHit: mGObjAt.mRPrm bit 0 */
                if (!(param_2->mGObjAt.mRPrm & 1)) {
                    a->field_0xC3C += 1;
                } else {
                    a->battleSubActionNockBackInit(0);
                }
            }
        }
    }
}
VERIFY(0x02248EA0, daJi1_AtHitCallback);

/* 02248F58 */
BOOL daNpc_Ji1_plRoomOutCheck() {
    /* WWHD_FUNC(0x02248F58, BOOL) without arguments (the macro needs at least one) */
    if (gabi::Activation::nested()) return gabi::call<BOOL>(0x02248F58);
    gabi::Activation wwhd_activation_;
    fopAc_ac_c* player = daPy_getPlayerActorClass();
    gabi::Local<cXyz> plyrPos;
    plyrPos->x = (f32)player->current.pos.x;
    plyrPos->y = (f32)player->current.pos.y;
    plyrPos->z = (f32)player->current.pos.z;
    /* static cXyz out_chk_pos(0.0f, 0.0f, 500.0f): guard 0x104673AC, object 0x10467258 */
    cXyz* out_chk_pos = gabi::at<cXyz>(0x10467258);
    if (gabi::load<u32>(0x104673AC) == 0) {
        out_chk_pos->y = 0.0f;
        gabi::store<u32>(0x104673AC, 1);
        out_chk_pos->x = 0.0f;
        out_chk_pos->z = 500.0f;
    }
    gabi::Local<cXyz> posDiff;
    cXyz_mi(plyrPos, posDiff, out_chk_pos);
    gabi::Local<cXyz> xz;
    xz->x = (f32)posDiff->x;
    xz->y = 0.0f;
    xz->z = (f32)posDiff->z;
    if (std_sqrtf(PSVECSquareMag(xz)) < 100.0f && dComIfGp_getDoStatus() == dActStts_OPEN_e) {
        return true;
    }
    return false;
}
VERIFY(0x02248F58, daNpc_Ji1_plRoomOutCheck);

/* 02249058 */
u32 playerCutAtCheck() {
    /* WWHD_FUNC(0x02249058, u32) without arguments (the macro needs at least one) */
    if (gabi::Activation::nested()) return gabi::call<u32>(0x02249058);
    gabi::Activation wwhd_activation_;
    /* daPy_py_c::checkCutAtFlg(): mModeFlg (+0x3B8) & 0x40 */
    return gabi::load<u32>(gabi::ea(daPy_getPlayerActorClass()) + 0x3B8) & 0x40;
}
VERIFY(0x02249058, playerCutAtCheck);

/* 02248D3C */
void daNpc_Ji1_c::setGuardParticle() {
    WWHD_FUNC(0x02248D3C, void, this);
    dComIfGp_particle_set(dPa_ID_AK_JN_NG, field_0x7E0.GetTgHitPosP());
    ji1_seStart(this, JA_SE_OBJ_COL_SWS_NMTLP, 0);
    ji1_seStart(this, JA_SE_CV_JI_DEFENCE, 0);
    gabi::Local<cXyz> pos;
    cXyz* hit = field_0x7E0.GetTgHitPosP();
    pos->x = (f32)hit->x;
    pos->y = (f32)hit->y;
    pos->z = (f32)hit->z;
    dKy_SordFlush_set(pos, 0);
}
VERIFY(0x02248D3C, &daNpc_Ji1_c::setGuardParticle);

/* 02248E0C */
void daNpc_Ji1_c::battleSubActionNockBackInit(int param_1) {
    WWHD_FUNC(0x02248E0C, void, this, param_1);
    if (param_1) {
        setGuardParticle();
    }
    field_0xC9C = 0.0f;
    field_0xD38.copy(current.pos);
    mpOrcaMorf->setPlaySpeed(-1.0f);
    ji1_setSubAction(this, SUB_battleSubActionNockBack);
    mBtNowFrame = 30.0f; /* setBtNowFrame */
}
VERIFY(0x02248E0C, &daNpc_Ji1_c::battleSubActionNockBackInit);

/* 0224A2B4 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0224A2B4, BOOL, i_this);
    return static_cast<daNpc_Ji1_c*>(i_this)->CreateHeap();
}
VERIFY(0x0224A2B4, CheckCreateHeap);

/* 0224A9F0 */
BOOL daNpc_Ji1_c::isItemWaitAnim() {
    WWHD_FUNC(0x0224A9F0, BOOL, this);
    if (mAnimation == 0 || mAnimation == 0xC || mAnimation == 0xD || mAnimation == 0x18) {
        return true;
    }
    return false;
}
VERIFY(0x0224A9F0, &daNpc_Ji1_c::isItemWaitAnim);

/* 0224AB7C */
BOOL daNpc_Ji1_c::isGuardAnim() {
    WWHD_FUNC(0x0224AB7C, BOOL, this);
    if ((mAnimation >= 8 && mAnimation <= 10) || mAnimation == 0xF) {
        return true;
    }
    return false;
}
VERIFY(0x0224AB7C, &daNpc_Ji1_c::isGuardAnim);

/* 02249880 */
static BOOL daNpc_Ji1_Draw(daNpc_Ji1_c* i_this) {
    WWHD_FUNC(0x02249880, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x02249880, daNpc_Ji1_Draw);

/* 0224C524 */
static BOOL daNpc_Ji1_Execute(daNpc_Ji1_c* i_this) {
    WWHD_FUNC(0x0224C524, BOOL, i_this);
    i_this->_execute();
    return true;
}
VERIFY(0x0224C524, daNpc_Ji1_Execute);

/* 0224C548 */
static BOOL daNpc_Ji1_IsDelete(daNpc_Ji1_c* i_this) {
    WWHD_FUNC(0x0224C548, BOOL, i_this);
    return true;
}
VERIFY(0x0224C548, daNpc_Ji1_IsDelete);

/* 0224C64C */
static BOOL daNpc_Ji1_Delete(daNpc_Ji1_c* i_this) {
    WWHD_FUNC(0x0224C64C, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x0224C64C, daNpc_Ji1_Delete);

/* 0224D4F4 */
static cPhs_State daNpc_Ji1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0224D4F4, cPhs_State, i_this);
    return static_cast<daNpc_Ji1_c*>(i_this)->_create();
}
VERIFY(0x0224D4F4, daNpc_Ji1_Create);

/* 0224D4F8 */
BOOL daNpc_Ji1_c::isAttackAnim() {
    WWHD_FUNC(0x0224D4F8, BOOL, this);
    if (mAnimation == 0x13 || mAnimation == 0x14 || mAnimation == 7) {
        return true;
    }
    return false;
}
VERIFY(0x0224D4F8, &daNpc_Ji1_c::isAttackAnim);

/* 0224D524 */
int daNpc_Ji1_c::isAttackFrame() {
    WWHD_FUNC(0x0224D524, int, this);
    f32 frame = mpOrcaMorf->getFrame();
    f32 temp2 = 0.0f;
    f32 temp3 = temp2;
    if (mAnimation == 0x13) {
        temp2 = 28.0f;
        temp3 = 35.0f;
    } else if (mAnimation == 0x14) {
        temp2 = 25.0f;
        temp3 = 35.0f;
    } else {
        if (mAnimation == 7) {
            temp2 = 12.0f;
            temp3 = 28.0f;
        }
    }
    if (temp2 > frame) {
        return -1;
    }
    return temp3 < frame;
}
VERIFY(0x0224D524, &daNpc_Ji1_c::isAttackFrame);

/* 0224D5BC HD: the level indexes the 4-entry table modulo 4 */
BOOL daNpc_Ji1_c::isClearRecord(s16 param_1) {
    WWHD_FUNC(0x0224D5BC, BOOL, this, param_1);
    if (dComIfGs_isEventBit(0x0F20 /* UNK_0F20 */)) {
        return false;
    }
    u8 level = dComIfGs_getEventReg(0xD003 /* UNK_D003 */);
    if (param_1 >= l_HIO().field_0x60[level & 3]) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x0224D5BC, &daNpc_Ji1_c::isClearRecord);

/* 0224D654 */
void daNpc_Ji1_c::setClearRecord(s16 hits) {
    WWHD_FUNC(0x0224D654, void, this, hits);
    u8 oldLevel = dComIfGs_getEventReg(0xD003);
    u8 newLevel = 0;
    while (hits >= l_HIO().field_0x60[newLevel & 3] && newLevel <= 3) {
        newLevel++;
    }
    if (oldLevel < newLevel) {
        dComIfGs_setEventReg(0xD003, newLevel);
        if (hits >= l_HIO().field_0x60[3]) {
            dComIfGs_onEventBit(0x0F20);
        }
    }
}
VERIFY(0x0224D654, &daNpc_Ji1_c::setClearRecord);

/* 0224DA20 */
u32 daNpc_Ji1_c::getMsg() {
    WWHD_FUNC(0x0224DA20, u32, this);
    if (dComIfGs_isEventBit(0x0520 /* UNK_0520 */)) {
        return getMsg2ndType();
    }
    return getMsg1stType();
}
VERIFY(0x0224DA20, &daNpc_Ji1_c::getMsg);

/* 0224DE14 */
void daNpc_Ji1_c::BackSlideInit() {
    WWHD_FUNC(0x0224DE14, void, this);
    field_0xC9C = 0.0f;
    field_0xD38.copy(current.pos);
}
VERIFY(0x0224DE14, &daNpc_Ji1_c::BackSlideInit);

/* 0224F528 */
f32 daNpc_Ji1_c::calcCoCorrectValue() {
    WWHD_FUNC(0x0224F528, f32, this);
    gabi::Local<cXyz> temp;
    temp->x = cM_ssin(current.angle.y);
    temp->y = 0.0f;
    temp->z = cM_scos(current.angle.y);
    f32 d = PSVECDotProduct(&field_0x638.m_cc_move, temp);
    f32 v = field_0xC4C + d;
    field_0xC4C = v;
    return v;
}
VERIFY(0x0224F528, &daNpc_Ji1_c::calcCoCorrectValue);

/* 0224F59C */
f32 daNpc_Ji1_c::calcBgCorrectValue() {
    WWHD_FUNC(0x0224F59C, f32, this);
    gabi::Local<cXyz> temp;
    temp->x = cM_ssin(current.angle.y);
    temp->y = 0.0f;
    temp->z = cM_scos(current.angle.y);
    if (mAcch.ChkWallHit()) {
        f32 d = PSVECDotProduct(&field_0xC40, temp);
        f32 v = field_0xC50 + d;
        field_0xC50 = v;
        return v;
    }
    return field_0xC50;
}
VERIFY(0x0224F59C, &daNpc_Ji1_c::calcBgCorrectValue);

/* 0224FB98 */
void daNpc_Ji1_c::dtParticle() {
    WWHD_FUNC(0x0224FB98, void, this);
    if (mSmokeCb.mpEmitter.get()) {
        smoke_end(&mSmokeCb);
    }
}
VERIFY(0x0224FB98, &daNpc_Ji1_c::dtParticle);

/* 0225AEB4 */
void daNpc_Ji1_c::dtParticleAT() {
    WWHD_FUNC(0x0225AEB4, void, this);
    if (mSmokeCbAT.mpEmitter.get()) {
        smoke_end(&mSmokeCbAT);
    }
}
VERIFY(0x0225AEB4, &daNpc_Ji1_c::dtParticleAT);

/* 022525B4 */
void daNpc_Ji1_c::BackSlide(f32 param_1, f32 param_2) {
    WWHD_FUNC(0x022525B4, void, this, param_1, param_2);
    cLib_addCalc2(&field_0xC9C, param_1, 0.25f, 50.0f);
    gabi::Local<cXyz> temp;
    f32 x = field_0xD38.x;
    f32 c9c = field_0xC9C;
    f32 y = field_0xD38.y;
    f32 s = cM_ssin(current.angle.y);
    f32 c = cM_scos(current.angle.y);
    f32 z = field_0xD38.z;
    temp->x = gabi::fnmsubs(c9c, s, x);
    temp->y = y;
    temp->z = gabi::fnmsubs(c9c, c, z);
    if (mAcch.ChkWallHit()) {
        param_2 *= 0.35f;
    }
    /* 0200EF78 cLib_addCalcPosXZ(cXyz*, const cXyz&, f32, f32, f32) */
    gabi::call(0x0200EF78, &current.pos, temp.get(), 0.8f, param_2, 1.0f);
}
VERIFY(0x022525B4, &daNpc_Ji1_c::BackSlide);

/* 0225A2C8 */
void daNpc_Ji1_c::battleSubActionWaitInit() {
    WWHD_FUNC(0x0225A2C8, void, this);
    field_0xC9C = 0.0f;
    ji1_setSubAction(this, SUB_battleSubActionWait);
}
VERIFY(0x0225A2C8, &daNpc_Ji1_c::battleSubActionWaitInit);

/* 022552E4 (the matcher names it battleSubActionJumpInit: it is teachSubActionJumpInit, no
 * sub-action) */
void daNpc_Ji1_c::teachSubActionJumpInit() {
    WWHD_FUNC(0x022552E4, void, this);
    setAnm(0xB, 0.0f, 1);
    field_0xC9C = 0.0f;
    field_0xD38.copy(current.pos);
}
VERIFY(0x022552E4, &daNpc_Ji1_c::teachSubActionJumpInit);

/* 0225A2F4 (the matcher names it teachSubActionJumpInit: it is battleSubActionJumpInit) */
void daNpc_Ji1_c::battleSubActionJumpInit() {
    WWHD_FUNC(0x0225A2F4, void, this);
    setAnm(0xB, 0.0f, 1);
    field_0xC9C = 0.0f;
    field_0xD38.copy(current.pos);
    ji1_setSubAction(this, SUB_battleSubActionJump);
}
VERIFY(0x0225A2F4, &daNpc_Ji1_c::battleSubActionJumpInit);

/* 0225A384 */
void daNpc_Ji1_c::battleSubActionDamageInit() {
    WWHD_FUNC(0x0225A384, void, this);
    setAnm(0x19, 0.0f, 1);
    mpOrcaMorf->setPlaySpeed(1.5f);
    field_0xC9C = 0.0f;
    field_0xD38.copy(current.pos);
    ji1_setSubAction(this, SUB_battleSubActionDamage);
}
VERIFY(0x0225A384, &daNpc_Ji1_c::battleSubActionDamageInit);

/* 0225A424 */
void daNpc_Ji1_c::battleSubActionGuardInit() {
    WWHD_FUNC(0x0225A424, void, this);
    field_0xC9C = 0.0f;
    field_0xD38.copy(current.pos);
    setParticle(0x10, 1.0f, 0.1f);
    ji1_setSubAction(this, SUB_battleSubActionGuard);
}
VERIFY(0x0225A424, &daNpc_Ji1_c::battleSubActionGuardInit);

/* 0225A4A4 */
void daNpc_Ji1_c::battleSubActionJpGuardInit() {
    WWHD_FUNC(0x0225A4A4, void, this);
    setAnm(0xA, 0.0f, 1);
    field_0xC9C = 0.0f;
    field_0xD38.copy(current.pos);
    ji1_setSubAction(this, SUB_battleSubActionJpGuard);
}
VERIFY(0x0225A4A4, &daNpc_Ji1_c::battleSubActionJpGuardInit);

/* 0225589C */
void daNpc_Ji1_c::teachSubActionAttackInit() {
    WWHD_FUNC(0x0225589C, void, this);
    setAnm(0x7, 0.0f, 0);
    mpOrcaMorf->setPlaySpeed(0.4f);
    ji1_seStart(this, JA_SE_CV_JI_ATTACK, 0);
}
VERIFY(0x0225589C, &daNpc_Ji1_c::teachSubActionAttackInit);

/* 0225B1A0 */
void daNpc_Ji1_c::battleSubActionYokoAttackInit() {
    WWHD_FUNC(0x0225B1A0, void, this);
    setAnm(0x14, 0.0f, 0);
    mpOrcaMorf->setPlaySpeed(l_HIO().field_0x80);
    ji1_seStart(this, JA_SE_CV_JI_ATTACK, 0);
    field_0xC34 = 0;
    ji1_setSubAction(this, SUB_battleSubActionYokoAttack);
}
VERIFY(0x0225B1A0, &daNpc_Ji1_c::battleSubActionYokoAttackInit);

/* 0225B22C */
void daNpc_Ji1_c::battleSubActionTateAttackInit() {
    WWHD_FUNC(0x0225B22C, void, this);
    setAnm(0x13, 0.0f, 0);
    mpOrcaMorf->setPlaySpeed(l_HIO().field_0x78);
    ji1_seStart(this, JA_SE_CV_JI_ATTACK, 0);
    field_0xC34 = 0;
    ji1_setSubAction(this, SUB_battleSubActionTateAttack);
}
VERIFY(0x0225B22C, &daNpc_Ji1_c::battleSubActionTateAttackInit);

/* 0225B2B8 */
void daNpc_Ji1_c::battleSubActionAttackInit() {
    WWHD_FUNC(0x0225B2B8, void, this);
    setAnm(0x7, 0.0f, 0);
    mpOrcaMorf->setPlaySpeed(l_HIO().field_0x6C);
    ji1_seStart(this, JA_SE_CV_JI_ATTACK, 0);
    field_0xC34 = 0;
    ji1_setSubAction(this, SUB_battleSubActionAttack);
}
VERIFY(0x0225B2B8, &daNpc_Ji1_c::battleSubActionAttackInit);

/* 0225D0D0 daNpc_Ji1_HIO_c::genMessage(JORMContext*) (empty virtual, HD; new) */
static void daNpc_Ji1_HIO_genMessage(void* self, void* ctx) {
    WWHD_FUNC(0x0225D0D0, void, self, ctx);
}
VERIFY(0x0225D0D0, daNpc_Ji1_HIO_genMessage);
