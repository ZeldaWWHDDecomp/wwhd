#pragma once
#include "d/actor/d_a_npc_ji1.h"

// HD: the current message is managed separately from the GameCube l_msg.
struct Ji1AnmMessage { u8 mPrefix[0x938]; be<u32> mMsgNo; };
struct Ji1AnmMessagePointer { gptr<Ji1AnmMessage> message; };
WWHD_OFFSET(Ji1AnmMessage, mMsgNo, 0x938);
struct Ji1AnmEmitter {
    u8 mPrefix[0x5C];
    be<s32> mMaxFrame;
    u8 mBody[0x254 - 0x60];
    be<u32> mFlags;
    void becomeInvalidEmitter() {
        u32 flags = mFlags;
        mMaxFrame = -1;
        mFlags = flags | 1;
    }
};
WWHD_OFFSET(Ji1AnmEmitter, mFlags, 0x254);
// HD: preserve the incidental r3 message pointer; this binding differs from the shared accessor.
u8 Ji1Anm_getMesgStatus(Ji1AnmMessage*);
enum Ji1AnmResource {
    dRes_INDEX_JI_BCK_JI_WAIT01_e = 0x38,
    dRes_INDEX_JI_BAS_JI_WAIT01_e = 0x1C,
    dRes_INDEX_JI_BCK_JI_WAIT02_e = 0x39,
    dRes_INDEX_JI_BAS_JI_WAIT02_e = 0x1D,
    dRes_INDEX_JI_BCK_JI_TALK01_e = 0x32,
    dRes_INDEX_JI_BAS_JI_TALK01_e = 0x16,
    dRes_INDEX_JI_BCK_JI_TALK02_e = 0x33,
    dRes_INDEX_JI_BAS_JI_TALK02_e = 0x17,
    dRes_INDEX_JI_BCK_JI_AKIRE_e = 0x24,
    dRes_INDEX_JI_BAS_JI_AKIRE_e = 0x08,
    dRes_INDEX_JI_BCK_JI_KAMAE_e = 0x2C,
    dRes_INDEX_JI_BAS_JI_KAMAE_e = 0x10,
    dRes_INDEX_JI_BCK_JI_KROT_e = 0x2D,
    dRes_INDEX_JI_BAS_JI_KROT_e = 0x11,
    dRes_INDEX_JI_BCK_JI_INASI_e = 0x2A,
    dRes_INDEX_JI_BAS_JI_INASI_e = 0x0E,
    dRes_INDEX_JI_BCK_JI_TATEGUARD_e = 0x35,
    dRes_INDEX_JI_BAS_JI_TATEGUARD_e = 0x19,
    dRes_INDEX_JI_BCK_JI_YKGUARD_e = 0x3B,
    dRes_INDEX_JI_BAS_JI_YKGUARD_e = 0x1F,
    dRes_INDEX_JI_BCK_JI_JPGUARD_e = 0x2B,
    dRes_INDEX_JI_BAS_JI_JPGUARD_e = 0x0F,
    dRes_INDEX_JI_BCK_JI_TOKAMAE_e = 0x36,
    dRes_INDEX_JI_BAS_JI_TOKAMAE_e = 0x1A,
    dRes_INDEX_JI_BCK_JI_REI_e = 0x31,
    dRes_INDEX_JI_BAS_JI_REI_e = 0x15,
    dRes_INDEX_JI_BCK_JI_WARAI_e = 0x3A,
    dRes_INDEX_JI_BAS_JI_WARAI_e = 0x1E,
    dRes_INDEX_JI_BCK_JI_GUARD_e = 0x29,
    dRes_INDEX_JI_BAS_JI_GUARD_e = 0x0D,
    dRes_INDEX_JI_BCK_JI_BTKAMASI_e = 0x27,
    dRes_INDEX_JI_BAS_JI_BTKAMASI_e = 0x0B,
    dRes_INDEX_JI_BCK_JI_BTKAMAE_e = 0x26,
    dRes_INDEX_JI_BAS_JI_BTKAMAE_e = 0x0A,
    dRes_INDEX_JI_BCK_JI_UDEGUMI_e = 0x37,
    dRes_INDEX_JI_BAS_JI_UDEGUMI_e = 0x1B,
    dRes_INDEX_JI_BCK_JI_TATEATTACK_e = 0x34,
    dRes_INDEX_JI_BAS_JI_TATEATTACK_e = 0x18,
    dRes_INDEX_JI_BCK_JI_YOKOATTACK_e = 0x3C,
    dRes_INDEX_JI_BAS_JI_YOKOATTACK_e = 0x20,
    dRes_INDEX_JI_BCK_JI_ODOROKU_e = 0x30,
    dRes_INDEX_JI_BAS_JI_ODOROKU_e = 0x14,
    dRes_INDEX_JI_BCK_JI_BIBIRI_e = 0x25,
    dRes_INDEX_JI_BAS_JI_BIBIRI_e = 0x09,
    dRes_INDEX_JI_BCK_JI_NAKU_e = 0x2E,
    dRes_INDEX_JI_BAS_JI_NAKU_e = 0x12,
    dRes_INDEX_JI_BCK_JI_NUGUI_e = 0x2F,
    dRes_INDEX_JI_BAS_JI_NUGUI_e = 0x13,
    dRes_INDEX_JI_BCK_JI_DAMAGE_e = 0x28,
    dRes_INDEX_JI_BAS_JI_DAMAGE_e = 0x0C,
    dRes_INDEX_JI_BCK_JIYARI_TATEATTACK_e = 0x3D,
    dRes_INDEX_JI_BCK_JIYARI_YOKOATTACK_e = 0x3E,
};
