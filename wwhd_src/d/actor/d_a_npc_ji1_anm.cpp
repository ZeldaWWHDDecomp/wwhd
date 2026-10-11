/**
 * d_a_npc_ji1_anm.cpp (WWHD)
 * NPC - Orca: setAnm, setAnimFromMsgNo
 *
 * Ported from the GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ji1.cpp) to the WWHD layout and verified against the WWHD code (cking.rpx).
 */
#include "d/actor/d_a_npc_ji1_anm.h"

static Ji1AnmMessagePointer messageManager;
static inline void* ji_res(s32 index) {
    return dComIfG_getObjectRes("Ji", index, SAFESTRING_VTBL);
}

enum { dPa_ID_AK_SN_JITEAR00 = 0x81A4, dPa_ID_AK_SN_JITEAR01 = 0x81A5 };

// HD: guard-defence sound is absent: its comparison is false after mAnimation is stored.
/* 0224DFF0 */
BOOL daNpc_Ji1_c::setAnm(int param_1, f32 param_2, int param_3) {
    if ((u32)param_1 == (u32)(s32)mAnimation && param_3 == 0) {
        return false;
    }
    mAnimation = param_1;
    f32 speed = 1.0f;
    s32 bckIdx, basIdx;
    s32 loopMode;
    switch (param_1) {
    case 0:
        bckIdx = dRes_INDEX_JI_BCK_JI_WAIT01_e;
        basIdx = dRes_INDEX_JI_BAS_JI_WAIT01_e;
        loopMode = J3DFrameCtrl::EMode_LOOP;
        break;
    case 1:
        bckIdx = dRes_INDEX_JI_BCK_JI_WAIT02_e;
        basIdx = dRes_INDEX_JI_BAS_JI_WAIT02_e;
        loopMode = J3DFrameCtrl::EMode_LOOP;
        break;
    case 2:
        bckIdx = dRes_INDEX_JI_BCK_JI_TALK01_e;
        basIdx = dRes_INDEX_JI_BAS_JI_TALK01_e;
        loopMode = J3DFrameCtrl::EMode_LOOP;
        break;
    case 3:
        bckIdx = dRes_INDEX_JI_BCK_JI_TALK02_e;
        basIdx = dRes_INDEX_JI_BAS_JI_TALK02_e;
        loopMode = J3DFrameCtrl::EMode_LOOP;
        break;
    case 4:
        bckIdx = dRes_INDEX_JI_BCK_JI_AKIRE_e;
        basIdx = dRes_INDEX_JI_BAS_JI_AKIRE_e;
        loopMode = J3DFrameCtrl::EMode_LOOP;
        break;
    case 5:
        bckIdx = dRes_INDEX_JI_BCK_JI_KAMAE_e;
        basIdx = dRes_INDEX_JI_BAS_JI_KAMAE_e;
        loopMode = J3DFrameCtrl::EMode_LOOP;
        break;
    case 6:
        bckIdx = dRes_INDEX_JI_BCK_JI_KROT_e;
        basIdx = dRes_INDEX_JI_BAS_JI_KROT_e;
        loopMode = J3DFrameCtrl::EMode_LOOP;
        break;
    case 7:
        bckIdx = dRes_INDEX_JI_BCK_JI_INASI_e;
        basIdx = dRes_INDEX_JI_BAS_JI_INASI_e;
        loopMode = J3DFrameCtrl::EMode_NONE;
        break;
    case 8:
        bckIdx = dRes_INDEX_JI_BCK_JI_TATEGUARD_e;
        basIdx = dRes_INDEX_JI_BAS_JI_TATEGUARD_e;
        loopMode = J3DFrameCtrl::EMode_NONE;
        break;
    case 9:
        bckIdx = dRes_INDEX_JI_BCK_JI_YKGUARD_e;
        basIdx = dRes_INDEX_JI_BAS_JI_YKGUARD_e;
        loopMode = J3DFrameCtrl::EMode_NONE;
        break;
    case 10:
        bckIdx = dRes_INDEX_JI_BCK_JI_JPGUARD_e;
        basIdx = dRes_INDEX_JI_BAS_JI_JPGUARD_e;
        loopMode = J3DFrameCtrl::EMode_NONE;
        break;
    case 11:
        bckIdx = dRes_INDEX_JI_BCK_JI_TOKAMAE_e;
        basIdx = dRes_INDEX_JI_BAS_JI_TOKAMAE_e;
        loopMode = J3DFrameCtrl::EMode_NONE;
        break;
    case 12:
        bckIdx = dRes_INDEX_JI_BCK_JI_REI_e;
        basIdx = dRes_INDEX_JI_BAS_JI_REI_e;
        loopMode = J3DFrameCtrl::EMode_NONE;
        break;
    case 13:
        bckIdx = dRes_INDEX_JI_BCK_JI_WARAI_e;
        basIdx = dRes_INDEX_JI_BAS_JI_WARAI_e;
        loopMode = J3DFrameCtrl::EMode_LOOP;
        break;
    case 14:
        bckIdx = dRes_INDEX_JI_BCK_JI_JPGUARD_e;
        basIdx = dRes_INDEX_JI_BAS_JI_JPGUARD_e;
        loopMode = J3DFrameCtrl::EMode_NONE;
        break;
    case 15:
        bckIdx = dRes_INDEX_JI_BCK_JI_GUARD_e;
        basIdx = dRes_INDEX_JI_BAS_JI_GUARD_e;
        loopMode = J3DFrameCtrl::EMode_LOOP;
        break;
    case 16:
        bckIdx = dRes_INDEX_JI_BCK_JI_BTKAMASI_e;
        basIdx = dRes_INDEX_JI_BAS_JI_BTKAMASI_e;
        loopMode = J3DFrameCtrl::EMode_LOOP;
        break;
    case 17:
        bckIdx = dRes_INDEX_JI_BCK_JI_BTKAMAE_e;
        basIdx = dRes_INDEX_JI_BAS_JI_BTKAMAE_e;
        loopMode = J3DFrameCtrl::EMode_LOOP;
        break;
    case 18:
        bckIdx = dRes_INDEX_JI_BCK_JI_UDEGUMI_e;
        basIdx = dRes_INDEX_JI_BAS_JI_UDEGUMI_e;
        loopMode = J3DFrameCtrl::EMode_LOOP;
        break;
    case 19:
        bckIdx = dRes_INDEX_JI_BCK_JI_TATEATTACK_e;
        basIdx = dRes_INDEX_JI_BAS_JI_TATEATTACK_e;
        loopMode = J3DFrameCtrl::EMode_NONE;
        break;
    case 20:
        bckIdx = dRes_INDEX_JI_BCK_JI_YOKOATTACK_e;
        basIdx = dRes_INDEX_JI_BAS_JI_YOKOATTACK_e;
        loopMode = J3DFrameCtrl::EMode_NONE;
        break;
    case 21:
        bckIdx = dRes_INDEX_JI_BCK_JI_ODOROKU_e;
        basIdx = dRes_INDEX_JI_BAS_JI_ODOROKU_e;
        loopMode = J3DFrameCtrl::EMode_LOOP;
        break;
    case 22:
        bckIdx = dRes_INDEX_JI_BCK_JI_BIBIRI_e;
        basIdx = dRes_INDEX_JI_BAS_JI_BIBIRI_e;
        loopMode = J3DFrameCtrl::EMode_LOOP;
        break;
    case 23:
        bckIdx = dRes_INDEX_JI_BCK_JI_NAKU_e;
        basIdx = dRes_INDEX_JI_BAS_JI_NAKU_e;
        loopMode = J3DFrameCtrl::EMode_LOOP;
        break;
    case 24:
        bckIdx = dRes_INDEX_JI_BCK_JI_NUGUI_e;
        basIdx = dRes_INDEX_JI_BAS_JI_NUGUI_e;
        loopMode = J3DFrameCtrl::EMode_NONE;
        break;
    case 25:
        bckIdx = dRes_INDEX_JI_BCK_JI_DAMAGE_e;
        basIdx = dRes_INDEX_JI_BAS_JI_DAMAGE_e;
        loopMode = J3DFrameCtrl::EMode_NONE;
        break;
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
        J3DAnmTransform* spear = (J3DAnmTransform*)ji_res(dRes_INDEX_JI_BCK_JIYARI_TATEATTACK_e);
        mpSpearMorf->setAnm(spear, J3DFrameCtrl::EMode_LOOP, 0.0f, 1.0f, 0.0f, -1.0f, nullptr);
    } else if (mAnimation == 0x14) {
        J3DAnmTransform* spear = (J3DAnmTransform*)ji_res(dRes_INDEX_JI_BCK_JIYARI_YOKOATTACK_e);
        mpSpearMorf->setAnm(spear, J3DFrameCtrl::EMode_LOOP, 0.0f, 1.0f, 0.0f, -1.0f, nullptr);
    }
    return true;
}

// HD: message manager supplies the message number and status; msgNo argument is unused.
/* 02251A8C */
void daNpc_Ji1_c::setAnimFromMsgNo(u32 msgNo) {
    Ji1AnmMessage* msgObj = messageManager.message;
    msgNo = msgObj->mMsgNo;
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
        if (Ji1Anm_getMesgStatus(msgObj) != 0xE) {
            break;
        }
        if (mAnimation != 0x17) {
            break;
        }
        if (field_0x430.get() != nullptr) {
            reinterpret_cast<Ji1AnmEmitter*>(field_0x430.get())->becomeInvalidEmitter();
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

/* WWHD glue
 data messageManager 101F4B5C
 ext 025F795C u8 Ji1Anm_getMesgStatus(Ji1AnmMessage*)
 str 1001B1D8 "Ji"
*/
