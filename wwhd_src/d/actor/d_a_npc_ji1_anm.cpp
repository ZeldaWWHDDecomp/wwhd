/**
 * d_a_npc_ji1_anm.cpp (WWHD)
 * NPC - Orca: setAnm, setAnimFromMsgNo
 *
 * Ported from the GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ji1.cpp) to the WWHD layout and verified against the WWHD code (cking.rpx).
 */
#include "d/actor/d_a_npc_ji1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* HD message manager: *(0x101F4B5C) is the current message object, its message number at +0x938
 * (replaces GameCube l_msg->mMsgNo) */
static inline u32 ji1_msgObj() { return gabi::load<u32>(0x101F4B5C); }
/* 025F795C (matcher: fopMsgM_SearchByID): takes no argument, returns the play object's message
 * status byte (play+0x5BB2); replaces GameCube l_msg->mStatus. The harness compares r3 (the
 * GameCube signature has an id argument): r3 still holds the message object here, so it is
 * passed along. */
static inline u8 dComIfGp_getMesgStatus(u32 r3_incidental) { return gabi::call<u8>(0x025F795C, r3_incidental); }
/* JPABaseEmitter::becomeInvalidEmitter (inline): stopCreateParticle (flags +0x254 |= 1),
 * mMaxFrame (+0x5C) = -1 */
static inline void JPABaseEmitter_becomeInvalidEmitter(JPABaseEmitter* e) {
    u32 b = gabi::ea(e);
    u32 f = gabi::load<u32>(b + 0x254);
    gabi::store<s32>(b + 0x5C, -1);
    gabi::store<u32>(b + 0x254, f | 1);
}

/* "Ji" (one literal for the whole function) */
#define JI_ARC STR(0x1001B1D8)
static inline void* ji_res(s32 idx) { return dComIfG_getObjectRes(JI_ARC, idx, SAFESTRING_VTBL); }

enum { dPa_ID_AK_SN_JITEAR00 = 0x81A4, dPa_ID_AK_SN_JITEAR01 = 0x81A5 };

/* 0224DFF0 HD: the guard-defence sound (dead code: param_1 != mAnimation after the store) is gone */
BOOL daNpc_Ji1_c::setAnm(int param_1, f32 param_2, int param_3) {
    WWHD_FUNC(0x0224DFF0, BOOL, this, param_1, param_2, param_3);
    if ((u32)param_1 == (u32)(s32)mAnimation && param_3 == 0) {
        return false;
    }
    mAnimation = param_1;
    f32 speed = 1.0f;
    s32 bckIdx, basIdx;
    s32 loopMode;
    switch (param_1) {
    case 0: bckIdx = 0x38; basIdx = 0x1C; loopMode = 2; break;
    case 1: bckIdx = 0x39; basIdx = 0x1D; loopMode = 2; break;
    case 2: bckIdx = 0x32; basIdx = 0x16; loopMode = 2; break;
    case 3: bckIdx = 0x33; basIdx = 0x17; loopMode = 2; break;
    case 4: bckIdx = 0x24; basIdx = 0x08; loopMode = 2; break;
    case 5: bckIdx = 0x2C; basIdx = 0x10; loopMode = 2; break;
    case 6: bckIdx = 0x2D; basIdx = 0x11; loopMode = 2; break;
    case 7: bckIdx = 0x2A; basIdx = 0x0E; loopMode = 0; break;
    case 8: bckIdx = 0x35; basIdx = 0x19; loopMode = 0; break;
    case 9: bckIdx = 0x3B; basIdx = 0x1F; loopMode = 0; break;
    case 10: bckIdx = 0x2B; basIdx = 0x0F; loopMode = 0; break;
    case 11: bckIdx = 0x36; basIdx = 0x1A; loopMode = 0; break;
    case 12: bckIdx = 0x31; basIdx = 0x15; loopMode = 0; break;
    case 13: bckIdx = 0x3A; basIdx = 0x1E; loopMode = 2; break;
    case 14: bckIdx = 0x2B; basIdx = 0x0F; loopMode = 0; break;
    case 15: bckIdx = 0x29; basIdx = 0x0D; loopMode = 2; break;
    case 16: bckIdx = 0x27; basIdx = 0x0B; loopMode = 2; break;
    case 17: bckIdx = 0x26; basIdx = 0x0A; loopMode = 2; break;
    case 18: bckIdx = 0x37; basIdx = 0x1B; loopMode = 2; break;
    case 19: bckIdx = 0x34; basIdx = 0x18; loopMode = 0; break;
    case 20: bckIdx = 0x3C; basIdx = 0x20; loopMode = 0; break;
    case 21: bckIdx = 0x30; basIdx = 0x14; loopMode = 2; break;
    case 22: bckIdx = 0x25; basIdx = 0x09; loopMode = 2; break;
    case 23: bckIdx = 0x2E; basIdx = 0x12; loopMode = 2; break;
    case 24: bckIdx = 0x2F; basIdx = 0x13; loopMode = 0; break;
    case 25: bckIdx = 0x28; basIdx = 0x0C; loopMode = 0; break;
    default: return 0;
    }
    J3DAnmTransform* bckAnm = (J3DAnmTransform*)ji_res(bckIdx);
    void* pSoundAnimRes = ji_res(basIdx);
    switch (param_1) {
    case 6: speed = l_HIO().field_0x48; break;
    case 7: speed = l_HIO().field_0x44; break;
    case 13: speed = l_HIO().field_0x4C; break;
    case 14:
    case 15: speed = 2.0f; break;
    case 22:
        BackSlideInit();
        if (field_0xD84 == 1) {
            harpoonRelease(nullptr);
        }
        break;
    case 23:
        if (field_0x430.get() == nullptr) {
            field_0x430 = dComIfGp_particle_set(dPa_ID_AK_SN_JITEAR00, &current.pos);
            harpoonRelease(nullptr);
        }
        break;
    }
    mpOrcaMorf->setAnm(bckAnm, loopMode, param_2, speed, 0.0f, -1.0f, pSoundAnimRes);
    if (mAnimation == 0x13) {
        J3DAnmTransform* spear = (J3DAnmTransform*)ji_res(0x3D /* BCK_JIYARI_TATEATTACK */);
        mpSpearMorf->setAnm(spear, J3DFrameCtrl::EMode_LOOP, 0.0f, 1.0f, 0.0f, -1.0f, nullptr);
    } else if (mAnimation == 0x14) {
        J3DAnmTransform* spear = (J3DAnmTransform*)ji_res(0x3E /* BCK_JIYARI_YOKOATTACK */);
        mpSpearMorf->setAnm(spear, J3DFrameCtrl::EMode_LOOP, 0.0f, 1.0f, 0.0f, -1.0f, nullptr);
    }
    return true;
}
VERIFY(0x0224DFF0, &daNpc_Ji1_c::setAnm);

/* 02251A8C HD: the message number and status come from the HD message manager (the msgNo
 * argument is not read) */
void daNpc_Ji1_c::setAnimFromMsgNo(u32 msgNo) {
    WWHD_FUNC(0x02251A8C, void, this, msgNo);
    u32 msgObj = ji1_msgObj();
    msgNo = gabi::load<u32>(msgObj + 0x938);
    switch (msgNo) {
    case 0x9B1:
        if (field_0xD84 == 1) {
            setAnm(0, 4.0f, 0);
            break;
        }
        setAnm(1, 4.0f, 0);
        break;
    case 0x961:
    case 0x986:
        setAnm(3, 8.0f, 0);
        break;
    case 0x962:
        setAnm(3, 4.0f, 0);
        break;
    case 0x9AF:
    case 0x9B3:
    case 0x9B4:
        if (field_0xD84 == 1) {
            setAnm(0, 4.0f, 0);
            break;
        }
        setAnm(2, 4.0f, 0);
        break;
    case 0x963:
    case 0x964:
        setAnm(2, 4.0f, 0);
        break;
    case 0x975:
        setAnm(4, 4.0f, 0);
        break;
    case 0x9AE:
        if (mAnimation == 0x16 && mpOrcaMorf->checkFrame(mpOrcaMorf->getEndFrame() - 1.0f)) {
            setAnm(0x15, l_HIO().field_0xA8, 0);
            break;
        }
        if (mAnimation != 0x15) {
            setAnm(0x16, l_HIO().field_0xA4, 0);
        }
        break;
    case 0x9B9:
        if (dComIfGp_getMesgStatus(msgObj) != 0xE) {
            break;
        }
        if (mAnimation != 0x17) {
            break;
        }
        if (field_0x430.get() != nullptr) {
            JPABaseEmitter_becomeInvalidEmitter(field_0x430);
            field_0x430 = nullptr;
            field_0x430 = dComIfGp_particle_set(dPa_ID_AK_SN_JITEAR01, &current.pos);
        }
        setAnm(0x18, 8.0f, 0);
        field_0xD84 = 1;
        break;
    default:
        break;
    }
}
VERIFY(0x02251A8C, &daNpc_Ji1_c::setAnimFromMsgNo);
