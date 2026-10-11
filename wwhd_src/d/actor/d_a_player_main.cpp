/**
 * d_a_player_main.cpp (WWHD)
 * Player - Link (daPy_lk_c): phase 1 (layout check), small accessors and checks
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_player_main.cpp and its .inc files) to the WWHD layout and code,
 * verified against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * The address ranges #01..#08 add their functions in d_a_player_main_NN.cpp files.
 */
#include "d/actor/d_a_player_main_readable.h"

static PlayerAnmData mAnmDataTable[] = {
    {294, 284, 0, 0, 0},
    {302, 302, 0, 0, 0},
    {112, 112, 2, 8, 0},
    {112, 115, 2, 8, 0},
    {306, 306, 0, 0, 0},
    {306, 115, 0, 0, 0},
    {310, 310, 2, 8, 0},
    {34, 33, 0, 0, 0},
    {36, 35, 0, 0, 0},
    {39, 38, 0, 0, 0},
    {41, 40, 0, 0, 0},
    {19, 18, 0, 0, 0},
    {21, 20, 0, 0, 0},
    {175, 174, 0, 0, 0},
    {37, 37, 0, 0, 0},
    {17, 17, 0, 0, 0},
    {29, 29, 0, 0, 48},
    {31, 31, 0, 0, 49},
    {30, 30, 0, 0, 0},
    {32, 32, 0, 0, 0},
    {230, 230, 0, 0, 0},
    {85, 86, 3, 8, 87},
    {118, 118, 3, 8, 2},
    {119, 119, 3, 8, 65},
    {93, 93, 3, 8, 37},
    {95, 95, 3, 8, 38},
    {96, 96, 3, 8, 38},
    {270, 270, 0, 0, 0},
    {287, 287, 0, 0, 14},
    {285, 285, 0, 0, 89},
    {63, 63, 3, 8, 30},
    {73, 73, 3, 8, 30},
    {82, 82, 3, 8, 30},
    {79, 79, 3, 8, 30},
    {67, 67, 3, 8, 31},
    {70, 70, 3, 8, 32},
    {125, 125, 3, 8, 33},
    {128, 128, 3, 8, 34},
    {66, 66, 3, 9, 106},
    {86, 86, 3, 8, 87},
    {88, 88, 3, 8, 38},
    {88, 91, 3, 8, 38},
    {90, 90, 3, 8, 88},
    {169, 169, 3, 8, 35},
    {173, 173, 3, 8, 35},
    {171, 171, 3, 8, 35},
    {172, 172, 3, 8, 35},
    {170, 170, 3, 8, 36},
    {186, 186, 0, 0, 56},
    {187, 187, 0, 0, 57},
    {219, 219, 0, 0, 51},
    {220, 220, 0, 0, 54},
    {250, 250, 0, 0, 0},
    {248, 248, 0, 0, 66},
    {249, 249, 0, 0, 67},
    {246, 246, 0, 0, 68},
    {247, 247, 0, 0, 69},
    {25, 25, 1, 8, 65},
    {26, 26, 1, 8, 65},
    {23, 23, 3, 8, 65},
    {176, 176, 0, 0, 50},
    {217, 217, 0, 0, 52},
    {218, 218, 0, 0, 53},
    {62, 62, 0, 0, 10},
    {189, 189, 2, 8, 10},
    {190, 190, 2, 8, 10},
    {311, 311, 0, 0, 15},
    {312, 312, 0, 0, 15},
    {319, 319, 0, 0, 26},
    {321, 321, 0, 0, 27},
    {320, 320, 0, 0, 26},
    {322, 322, 0, 0, 27},
    {315, 315, 0, 0, 28},
    {317, 317, 0, 0, 29},
    {316, 316, 0, 0, 28},
    {318, 318, 0, 0, 29},
    {278, 278, 0, 0, 0},
    {279, 279, 0, 0, 76},
    {280, 280, 0, 0, 0},
    {281, 281, 0, 0, 77},
    {159, 159, 0, 0, 84},
    {163, 163, 0, 0, 85},
    {161, 161, 0, 0, 12},
    {162, 162, 0, 0, 13},
    {97, 97, 0, 0, 58},
    {110, 110, 1, 8, 58},
    {111, 111, 1, 8, 58},
    {101, 101, 1, 8, 58},
    {98, 98, 1, 8, 58},
    {106, 106, 0, 0, 59},
    {108, 108, 0, 0, 59},
    {104, 104, 0, 0, 59},
    {102, 102, 0, 0, 59},
    {107, 107, 0, 0, 62},
    {109, 109, 0, 0, 63},
    {105, 105, 0, 0, 60},
    {103, 103, 0, 0, 61},
    {188, 188, 0, 0, 86},
    {117, 117, 0, 0, 99},
    {245, 245, 0, 0, 99},
    {255, 255, 0, 0, 100},
    {145, 145, 0, 0, 21},
    {148, 148, 0, 0, 22},
    {144, 144, 0, 0, 23},
    {149, 149, 0, 0, 9},
    {150, 150, 0, 0, 18},
    {147, 147, 0, 0, 24},
    {146, 146, 0, 0, 25},
    {194, 194, 2, 8, 39},
    {195, 195, 2, 8, 40},
    {201, 201, 2, 8, 41},
    {203, 203, 2, 8, 42},
    {202, 202, 3, 8, 43},
    {204, 204, 3, 8, 44},
    {205, 205, 0, 0, 45},
    {206, 206, 3, 8, 46},
    {207, 207, 3, 8, 47},
    {221, 221, 3, 9, 76},
    {225, 225, 3, 9, 8},
    {224, 224, 3, 9, 8},
    {229, 229, 3, 9, 9},
    {222, 222, 1, 7, 78},
    {223, 223, 3, 9, 79},
    {227, 227, 3, 8, 73},
    {51, 51, 0, 0, 73},
    {283, 283, 2, 7, 2},
    {292, 292, 0, 0, 2},
    {308, 308, 0, 0, 74},
    {307, 307, 2, 8, 75},
    {257, 257, 0, 0, 70},
    {258, 258, 0, 0, 3},
    {256, 256, 0, 0, 71},
    {185, 185, 0, 0, 80},
    {184, 184, 0, 0, 82},
    {183, 183, 0, 0, 82},
    {180, 180, 0, 0, 81},
    {179, 179, 0, 0, 83},
    {178, 178, 0, 0, 83},
    {182, 182, 0, 0, 10},
    {181, 181, 0, 0, 10},
    {134, 134, 0, 0, 9},
    {135, 135, 0, 0, 9},
    {56, 56, 0, 0, 97},
    {57, 57, 0, 0, 98},
    {168, 168, 4, 8, 93},
    {164, 164, 0, 0, 91},
    {313, 313, 0, 0, 92},
    {314, 314, 0, 0, 92},
    {61, 61, 0, 0, 90},
    {303, 303, 0, 0, 0},
    {231, 231, 3, 8, 94},
    {326, 326, 0, 0, 95},
    {209, 209, 0, 0, 96},
    {291, 291, 0, 0, 19},
    {252, 252, 0, 0, 101},
    {272, 272, 0, 0, 102},
    {193, 193, 0, 0, 103},
    {293, 293, 0, 0, 146},
    {143, 143, 2, 8, 105},
    {243, 243, 2, 7, 107},
    {244, 244, 2, 7, 108},
    {273, 273, 3, 9, 109},
    {275, 275, 3, 9, 110},
    {276, 276, 3, 9, 111},
    {198, 198, 2, 8, 0},
    {136, 136, 2, 8, 0},
    {166, 166, 3, 0, 20},
    {282, 282, 0, 0, 113},
    {295, 295, 5, 10, 6},
    {251, 251, 0, 0, 117},
    {152, 152, 3, 9, 118},
    {157, 157, 3, 9, 119},
    {155, 155, 3, 9, 120},
    {154, 154, 3, 9, 121},
    {240, 240, 0, 0, 122},
    {122, 122, 0, 0, 123},
    {123, 123, 0, 0, 124},
    {242, 242, 0, 0, 125},
    {212, 212, 0, 7, 126},
    {327, 327, 5, 0, 127},
    {328, 328, 5, 0, 128},
    {214, 214, 0, 7, 129},
    {44, 44, 6, 7, 130},
    {43, 43, 6, 7, 131},
    {42, 42, 6, 7, 132},
    {48, 48, 6, 7, 136},
    {46, 46, 6, 8, 137},
    {47, 47, 6, 8, 138},
    {49, 49, 6, 0, 139},
    {50, 50, 6, 0, 139},
    {45, 45, 6, 7, 140},
    {253, 253, 0, 0, 142},
    {216, 216, 0, 0, 1},
    {277, 277, 3, 9, 5},
    {329, 329, 0, 0, 6},
    {208, 208, 6, 7, 144},
    {124, 124, 0, 0, 4},
    {241, 241, 0, 0, 147},
    {141, 141, 3, 9, 5},
    {290, 290, 3, 9, 5},
    {191, 191, 2, 8, 148},
    {192, 192, 2, 8, 149},
    {266, 266, 5, 0, 93},
    {99, 99, 0, 0, 150},
    {234, 234, 0, 0, 151},
    {238, 238, 0, 0, 0},
    {235, 235, 0, 0, 0},
    {236, 236, 0, 0, 153},
    {232, 232, 0, 0, 153},
    {237, 237, 0, 0, 154},
    {233, 233, 0, 0, 154},
    {205, 174, 0, 0, 55},
    {58, 58, 0, 0, 97},
    {239, 239, 0, 0, 155},
    {323, 323, 0, 0, 156},
    {324, 324, 0, 0, 157},
    {254, 254, 0, 0, 101},
    {213, 213, 0, 7, 4},
    {211, 211, 0, 0, 158},
    {210, 210, 2, 8, 159},
    {177, 177, 0, 0, 2},
    {60, 60, 3, 8, 3},
    {76, 76, 3, 8, 160},
    {325, 325, 0, 0, 161},
    {286, 286, 0, 0, 0},
    {138, 138, 0, 0, 164},
    {139, 139, 0, 0, 165},
    {140, 140, 0, 0, 166},
    {268, 268, 5, 10, 167},
    {269, 269, 5, 10, 168},
    {265, 265, 5, 10, 169},
    {267, 267, 5, 10, 170},
    {263, 263, 5, 10, 171},
    {264, 264, 5, 10, 172},
};
static u32 l_noSeAnimation = 0;
static u16 l_curseState = 3;
static u16 upperAnime(daPy_lk_c* player) {
    return reinterpret_cast<PlayerAnmHeap*>(player->m_anm_heap_upper)[UPPER_MOVE2_e].mIdx;
}
/* 023D78E4 */
BOOL daPy_lk_c::checkSingleItemEquipAnime() {
    /* checkUpperAnime(..): m_anm_heap_upper[UPPER_MOVE2_e].mIdx (u16 at +0) */
    u16 idx = upperAnime(this);
    return idx == dRes_INDEX_LKANM_BCK_TAKEL_e || idx == dRes_INDEX_LKANM_BCK_TAKER_e;
}

/* 023D794C */
BOOL daPy_lk_c::checkEquipAnime() {
    return upperAnime(this) == dRes_INDEX_LKANM_BCK_REST_e || Player_checkItemEquipAnime(this);
}

/* 023D85CC */
BOOL daPy_lk_c::checkBottleItem(int item) {
    return (u32)(item - dItemNo_EMPTY_BOTTLE_e) < 0x11;
}

/* 023D85E4 */
BOOL daPy_lk_c::checkPhotoBoxItem(int item) {
    return item == dItemNo_PICTO_BOX_e || item == dItemNo_DELUXE_PICTO_BOX_e;
}

/* 023D8600 */
BOOL daPy_lk_c::checkBowItem(int item) {
    return item == dItemNo_BOW_e || item == dItemNo_MAGIC_ARROW_e || item == dItemNo_LIGHT_ARROW_e;
}

/* 023DC0F4 */
u32 daPy_lk_c::seStartOnlyReverb(u32 id) {
    return Player_seStart( id, &current.pos, 0u, (s32)(s8)mReverb); /* mDoAud_seStart */
}

/* 023DC63C */
static void daPy_actorKeep_clearData(daPy_actorKeep_c* k) {
    k->mID = 0xFFFFFFFF;
    k->mActor = nullptr;
}

/* 023DCB3C */
void daPy_lk_c::cancelNoDamageMode() {
    mTinkleShieldTimer = 0;
    offNoResetFlg1(daPyFlg1_EQUIP_DRAGON_SHIELD);
}

/* 023DD754 */
BOOL daPy_lk_c::checkMabaAnimeBtp(int idx) {
    return (u32)(idx - dRes_INDEX_LKANM_BTP_TMABAA_e) < 0xC;
}

/* 023DDF64 */
J3DModel* daPy_lk_c::setItemHeap() {
    if (!checkResetFlg0(daPyRFlg0_UNK4000)) { /* !checkResetFlg0(daPyRFlg0_UNK4000) */
        mCurrItemHeapIdx = mCurrItemHeapIdx ^ 1;
        onResetFlg0(daPyRFlg0_UNK4000);
    }
    /* setAnimeHeap(mpItemHeaps[mCurrItemHeapIdx]) (023DDEA8, unnamed by the matcher) */
    u32 heap = reinterpret_cast<PlayerItemBlock*>(mD90)->mpItemHeaps[mCurrItemHeapIdx];
    return Player_setAnimeHeap(this, heap);
}

/* 023DE638 */
static void daPy_actorKeep_setData(daPy_actorKeep_c* k, fopAc_ac_c* actor) {
    k->mActor = actor;
    k->mID = actor != nullptr ? fopAcM_GetID(actor) : 0xFFFFFFFF;
}

/* 023E0470 */
u32 daPy_lk_c::seStartMapInfo(u32 id) {
    return Player_seStart( id, &current.pos, (u32)mMtrlSndId, (s32)(s8)mReverb); /* mDoAud_seStart */
}

/* 023E063C */
void daPy_lk_c::setHandModel(int anmIdx) {
    mLeftHandIdx = mAnmDataTable[anmIdx].mLeftHandIdx;
    mRightHandIdx = mAnmDataTable[anmIdx].mRightHandIdx;
}

/* 023E07C0 */
void daPy_lk_c::resetSeAnime() {
    // HD: a pointer to the no-sound animation sentinel replaces mSeAnmIdx = -1.
    mpSeAnm = gabi::ea(&l_noSeAnimation);
    m34F0 = 0xFFFF;
    mpSeAnmFrameCtrl = 0;
}

/* 023E3358 */
int daPy_lk_c::getDirectionFromAngle(s16 angle) {
    s32 a = angle;
    if ((a < 0 ? -a : a) > 0x6000) {
        return DIR_BACKWARD;
    } else if (angle >= 0x2000) {
        return DIR_LEFT;
    } else if (angle <= -0x2000) {
        return DIR_RIGHT;
    }
    return DIR_FORWARD;
}

/* 023E3398 */
int daPy_lk_c::getDirectionFromCurrentAngle() {
    return getDirectionFromAngle((s16)(m34E8 - current.angle.y));
}

/* 023E4E18 */
int daPy_lk_c::getDirectionFromShapeAngle() {
    return getDirectionFromAngle((s16)(m34E8 - shape_angle.y));
}

/* 023E8C74 */
BOOL daPy_lk_c::itemButton() {
    u8 btn = mReadyItemBtn;
    u8 b = mItemButton;
    if (btn == dItemBtn_X_e) {
        return b & 4; /* itemButtonX() */
    }
    return b & (btn == dItemBtn_Y_e ? 8 : 0x10); /* itemButtonY() / itemButtonZ() */
}

/* 023EE9FC */
void daPy_lk_c::setSubjectMode() {
    dComIfGp_setPlayerStatus0(0, daPyStts0_SUBJECT_e);
    Player_systemSound(JA_SE_SUBJ_VIEW_IN);
}

/* 023F73A4 */
void daPy_lk_c::resetCurse() {
    if (l_curseState == 1) { /* HD: a global (GameCube: the curse state) */
        Player_endDamageEmitter(this);
    }
}

/* 023F931C */
BOOL daPy_lk_c::checkSuccessGuard(int atSpl) {
    /* mCyl.ChkTgShieldHit(): the TG hit flags of mCyl (+0x98) */
    if (!((u32)mCyl.mGObjTg.mRPrm & 2) || atSpl >= dCcG_At_Spl_UNK8) {
        return FALSE;
    }
    return TRUE;
}

/* 023FD4D0 */
u32 daPy_lk_c::setParamData(int roomNo, int spawnType, int eventInfoIdx, int extraParams) {
    return (roomNo & 0x3F) | (spawnType & 0xF) << 0xC | (u32)eventInfoIdx << 0x18 | extraParams;
}

/* 024072F4 */
u32 daPy_lk_c::seStartSwordCut(u32 id) {
    return Player_seStart( id, &mSwordTopPos, 0u, (s32)(s8)mReverb); /* mDoAud_seStart */
}

/* 0241E428 */
BOOL daPy_lk_c::procPolyDamage() {
    if (mFrameCtrlUnder[UNDER_MOVE0_e].getRate() < 0.01f) { /* UNDER_MOVE0_e */
        Player_checkNextMode(this, 0);
    }
    return TRUE;
}

/* 024430A4 */
s16 daPy_lk_c::checkAutoJumpFlying() {
    if (mCurProc != daPyProc_AUTO_JUMP_e) {
        return -1;
    }
    return mProcVar0;
}

/* 024435E0 */
void daPy_lk_c::onDekuSpReturnFlg(u8 i_point) {
    if (i_point != 0xFF) {
        mDekuSpRestartPoint = i_point;
    }
    onNoResetFlg0(daPyFlg0_DEKU_SP_RETURN_FLG);
}

/* 02443728 */
BOOL daPy_lk_c::checkPlayerGuard() {
    s32 proc = mCurProc;
    if (proc == daPyProc_CROUCH_DEFENSE_e) return TRUE;
    u16 upper = upperAnime(this); /* m_anm_heap_upper[UPPER_MOVE2_e].mIdx */
    return upper == dRes_INDEX_LKANM_BCK_ATNG_e || upper == dRes_INDEX_LKANM_BCK_ATNGHAM_e || proc == daPyProc_GUARD_SLIP_e || proc == daPyProc_CROUCH_DEFENSE_SLIP_e;
}

/* WWHD glue (tools/verify/autoglue.py)
data mAnmDataTable 100366A0
data l_noSeAnimation 10035574
data l_curseState 101CEF16
ext 025E1A40 u32 Player_seStart(u32, cXyz*, u32, s32)
ext 025E1988 void Player_systemSound(u32)
ext 023D7904 BOOL Player_checkItemEquipAnime(daPy_lk_c*)
ext 023DDEA8 J3DModel* Player_setAnimeHeap(daPy_lk_c*, u32)
ext 023DCA08 void Player_endDamageEmitter(daPy_lk_c*)
ext 023F14E0 BOOL Player_checkNextMode(daPy_lk_c*, s32)
*/
