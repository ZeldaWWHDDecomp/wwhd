/** Gohma damage processing. Private WWHD decomp; verified against cking.rpx.
 * tww d_a_btd.cpp names and control flow, with HD call/store order retained. */
#include "d/actor/d_a_btd_damage.h"

static BtdHioDamageHD l_HIO;
static s16 l_pauseRegister;
static u8 l_pauseTimer;

enum BtdDamageAnimation {
    dRes_INDEX_BTD_BCK_HIT_L_e = 0x12,
    dRes_INDEX_BTD_BCK_HIT_M_e = 0x13,
    dRes_INDEX_BTD_BCK_HIT_R_e = 0x14
};
enum BtdDamageSound {
    JA_SE_CV_BTD_DMG_ROCK = 0x483B,
    JA_SE_CV_BTD_DAMAGE = 0x4836,
    JA_SE_LK_LAST_HIT = 0x2828
};
enum BtdDamageParticle { ID_AK_JN_CRITICALHITFLASH = 0x10, ID_AK_JN_OK = 0xD };

// HD: reverb forwarding retains the full register result.
static void monsSeStart(fopAc_ac_c* actor, u32 id) {
    if (gabi::ea(&actor->eyePos) == 0)
        return;
    s32 actorId = actor != nullptr ? (s32)(u32)actor->mBsPcId : -1;
    s32 room = actor->current.roomNo;
    s32 reverb = btdGetReverb(room);
    btdMonsSeStart(id, &actor->eyePos, actorId, 0, reverb);
}

/* 020F4D80 */
void damage_check(btd_class* self) {
    BtdDamageViewHD* i_this = reinterpret_cast<BtdDamageViewHD*>(self);
    fopAc_ac_c* actor = &i_this->actor;
    // HD: retain the initial play accessor even though its result is unused.
    dComIfGp_get();
    if (i_this->m02F6 == 1) {
        i_this->m02F6 = 2;
        i_this->mGohmaState = 20;
        i_this->m02E4 = 2;
        if (actor != nullptr)
            monsSeStart(actor, JA_SE_CV_BTD_DMG_ROCK);
        eff_off(self);
        return;
    }
    s32 hit = i_this->mEyeSph.ChkTgHit();
    if (hit && i_this->m02E0 == 0 && i_this->mActionState == 0) {
        i_this->mActionState = 10;
        void* obj = i_this->mEyeSph.GetTgHitObj();
        def_se_set(actor, obj, 0x40);
    }
    hit = i_this->mEyeSph.ChkTgHit();
    if (hit && i_this->m02E0 == 1) {
        u32 flags = i_this->mEyeSph.mGObjTg.mSPrm;
        s16 cooldown = i_this->mActionState;
        i_this->mEyeSph.mGObjTg.mSPrm = flags | 2; // OnTgNoConHit
        if (cooldown == 0) {
            i_this->mActionState = 7;
            void* obj = i_this->mEyeSph.GetTgHitObj();
            CcAtInfoBtdHD atInfo;
            atInfo.mpObj = obj;
            atInfo.pParticlePos = i_this->mEyeSph.GetTgHitPosP();
            at_power_check(&atInfo);
            u8 health = actor->health;
            u8 damage = atInfo.mDamage;
            u8 total = i_this->mTotalDamage;
            s8 remaining = (s8)(health - damage);
            i_this->mTotalDamage = (u8)(total + damage);
            actor->health = remaining;
            if (remaining <= 0 || l_HIO.m06 != 0) {
                // HD: reset precedes the pause and two independently fetched particle controls.
                reinterpret_cast<BtdPlayDamageHD*>(dComIfGp_get())->mDeathReset = 0.0f;
                s16 pause = l_pauseRegister;
                l_pauseTimer = (u8)(pause + 8);
                dPa_control_c* control = dComIfGp_getParticle();
                btdParticleSet(control, 0, ID_AK_JN_CRITICALHITFLASH, &i_this->m03E0, nullptr,
                               nullptr, 255, nullptr, -1, nullptr, nullptr, nullptr);
                csXyz angle;
                cXyz pos;
                angle.x = 0;
                angle.z = 0;
                pos.z = 2.0f;
                pos.y = 2.0f;
                pos.x = 2.0f;
                fopAc_ac_c* player = dComIfGp_getPlayer(0);
                angle.y = fopAcM_searchActorAngleY(actor, player);
                control = dComIfGp_getParticle();
                btdParticleSet(control, 0, ID_AK_JN_OK, &i_this->m03E0, &angle, &pos, 255, nullptr,
                               -1, nullptr, nullptr, nullptr);
                i_this->m02E4 = 11;
                i_this->mGohmaState = 50;
                if (gabi::ea(&actor->eyePos) != 0) {
                    s32 room = actor->current.roomNo;
                    s32 reverb = btdGetReverb(room);
                    mDoAud_seStart(JA_SE_LK_LAST_HIT, &actor->eyePos, 0, reverb);
                }
                return;
            }
            if (i_this->m02E4 == 2) {
                s8 totalDamage = (s8)(u8)i_this->mTotalDamage;
                s16 threshold = l_HIO.m0E;
                if (totalDamage >= threshold) {
                    i_this->mTotalDamage = 0;
                    i_this->mGohmaState = 10;
                } else {
                    s32 anm = dRes_INDEX_BTD_BCK_HIT_M_e;
                    if (atInfo.mResultingAttackType == 1) {
                        fopAc_ac_c* player = dComIfGp_getPlayer(0);
                        s16 angle = fopAcM_searchActorAngleY(actor, player);
                        s16 facing = actor->current.angle.y;
                        s16 delta = (s16)(angle - facing);
                        if ((u32)((s32)delta + 255) >= 511)
                            anm =
                                delta > 0 ? dRes_INDEX_BTD_BCK_HIT_R_e : dRes_INDEX_BTD_BCK_HIT_L_e;
                    }
                    anm_init(self, anm, 1.0f, 0, 1.0f, -1);
                    i_this->mGohmaState = 3;
                }
            } else {
                i_this->mGohmaState = 0;
                i_this->m02E4 = 2;
            }
            monsSeStart(actor, JA_SE_CV_BTD_DAMAGE);
            eff_off(self);
        }
    }
    for (u32 i = 0; i < 19; ++i) {
        s16 cooldown = i_this->m5FC4[i];
        if (cooldown) {
            i_this->m5FC4[i] = (s16)(cooldown - 1);
            continue;
        }
        dCcD_Sph* sph = &i_this->mAtSphs[i];
        if (sph->ChkTgHit()) {
            u32 flags = sph->mGObjTg.mSPrm;
            sph->mGObjTg.mSPrm = flags | 2;
            i_this->m5FC4[i] = 10;
            // HD: sequential word copies retain signaling NaNs and possible guest aliases.
            i_this->m5E98[i] = i_this->m02FC[i];
            void* obj = sph->GetTgHitObj();
            def_se_set_p(actor, &i_this->m5E98[i], obj, 0x42);
            break;
        }
    }
    for (u32 i = 0; i < 6; ++i) {
        s16 cooldown = i_this->m5FEA[i];
        if (cooldown) {
            i_this->m5FEA[i] = (s16)(cooldown - 1);
            continue;
        }
        dCcD_Cyl* cyl = &i_this->mHandCyls[i];
        if (cyl->ChkTgHit()) {
            u32 flags = cyl->mGObjTg.mSPrm;
            cyl->mGObjTg.mSPrm = flags | 2;
            i_this->m5FEA[i] = 10;
            void* obj = cyl->GetTgHitObj();
            def_se_set_p(actor, &i_this->m5E98[i + 19], obj, 0x42);
            return;
        }
    }
}
// clang-format off
/* WWHD glue (tools/verify/autoglue.py):
data l_HIO 104629FC
data l_pauseRegister 1047B696
data l_pauseTimer 101EACB7
ext 02520540 s32 btdGetReverb(s32)
ext 025E1AA4 void btdMonsSeStart(u32,cXyz*,s32,u32,s32)
ext 020F0C38 void eff_off(btd_class*)
ext 02518CC8 void def_se_set(fopAc_ac_c*,void*,u32)
ext 02518D40 void def_se_set_p(fopAc_ac_c*,cXyz*,void*,u32)
ext 02518DB0 void at_power_check(CcAtInfoBtdHD*)
ext 020F03BC void anm_init(btd_class*,s32,f32,u8,f32,s32)
ext 025A847C void btdParticleSet(dPa_control_c*,u32,u32,cXyz*,csXyz*,cXyz*,u32,void*,s32,void*,void*,void*)
*/
// clang-format on
