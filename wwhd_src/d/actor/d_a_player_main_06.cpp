/**
 * d_a_player_main_06.cpp (WWHD)
 * Player - Link (daPy_lk_c), address range #06 (02423D10..0242DAC7): the tail of the demo procs
 * (d_a_player_dproc.inc), ladder, hang, climb, wall hide, crawl and the first grab functions.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_player_main.cpp and its .inc files) to the WWHD layout and code,
 * verified against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * Both subranges use canonical member names and private views of opaque HD blocks.
 * HD: both halves use autoglue registration so their shared source compiles as one unit.
 */
#include "d/actor/d_a_player_main_06.h"

const gc_tu_data gc_tu = {.safeString = 0x10034B24};

// HD: GHS construction and destruction use this translation unit's vtables.
const PlayerDemoLineCheckVtables l_demoLineCheckVtables = {
    {0x10034DD4, 0x10034DE4, 0x10034E04, 0x10034DF4}, 0x10034C24, 0x10034BBC};

static cXyz l_crawl_front_offset;
static cXyz l_crawl_back_offset;
static cXyz l_crawl_top_offset;
static cXyz l_crawl_front_up_offset;
static cXyz l_crawl_back_up_offset;
static cXyz l_crawl_stand_up_offset;
static cXyz l_crawl_top_up_offset;
static cXyz l_crawl_side_offset;
static cXyz l_crawl_lside_offset;
static cXyz l_crawl_rside_offset;
static cXyz l_crawl_lside_front_offset;
static cXyz l_crawl_rside_front_offset;
static cXyz l_crawl_min_side_offset;
static f32 l_crawl_frame_radians;
static f32 l_crawl_wall_radius;
static PlayerMoveMabaState l_maba_state;

// HD: per-translation-unit tuning and message input state.
static PlayerDemoBottleTuning l_bottleTuning;
static f32 l_ladderOffset;
static u32 l_textInputObject;
static u8 l_textInputReady;
static s16 l_textInputResult;

/* 02423D10 */
BOOL daPy_lk_c::dProcPresent() {
    setShapeAngleToTalkActor();
    if (mProcVar6 != 0) {
        cXyz d;
        cXyz_mi(&m370C, &d, &current.pos);
        if (cLib_distanceAngleS(cM_atan2s(d.x, d.z), current.angle.y) > 0x4000 ||
            (mAcch.m_flags & dBgS_Acch::WALL_HIT)) {
            current.pos.x = m370C.x;
            current.pos.z = m370C.z;
            mProcVar6 = 0;
            dProcPresent_init_sub();
            mNormalSpeed = 0.0f;
        }
    } else {
        current.angle.y = shape_angle.y;
        if (mProcVar7 != 0) {
            dComIfGp_evmng_cutEnd(mStaffIdx);
        } else if (mFrameCtrlUnder[0].getRate() < 0.01f) {
            dComIfGp_evmng_cutEnd(mStaffIdx);
            mProcVar7 = 1;
            setSingleMoveAnime(player_demo::ANM_PRESENTATIONAWAIT, 1.0f, 0.0f, -1, 3.0f);
            mModeFlg = (mModeFlg | player_demo::ModeFlg_00000080 | player_demo::ModeFlg_00000100) & ~player_demo::ModeFlg_00000400;
        } else if (mFrameCtrlUnder[0].checkPass(52.0f /* HD: m_HIO->mTurn.m.field_0xE - 10.0f folded */)) {
            if (checkBottleItem(dComIfGp_event_getPreItemNo())) {
                if (mEquipItem == player_demo::daPyItem_NONE_e) {
                    setBottleModel((u32)dComIfGp_event_getPreItemNo());
                }
            } else {
                fopAc_ac_c* item = fopAcM_getItemEventPartner(this);
                if (item != nullptr &&
                    (fpcM_GetName(item) == player_demo::fpcNm_ITEM_e || fpcM_GetName(item) == player_demo::fpcNm_Demo_Item_e)) {
                    playerDemoShowItem(item);
                }
            }
        }
    }
    return TRUE;
}
/* 02423F08 */
BOOL daPy_lk_c::dProcWindChange_init() {
    int use = checkShipRideUseItem(0);
    if (mCurProc == player_demo::daPyProc_DEMO_WIND_CHANGE_e) {
        return FALSE;
    }
    commonProcInit(player_demo::daPyProc_DEMO_WIND_CHANGE_e);
    mNormalSpeed = 0.0f;
    int anm = player_demo::ANM_WINDR;
    if (dKyw_get_tactwind_dir()) {
        anm = player_demo::ANM_WINDL;
    }
    setSingleMoveAnime(anm, 1.0f, 0.0f, -1, 5.0f);
    initShipRideUseItem(use, 0);
    return TRUE;
}
/* 02424000 */
BOOL daPy_lk_c::dProcWindChange() {
    if (dComIfGp_checkPlayerStatus0(0, player_demo::daPyStts0_SHIP_RIDE_e)) {
        setShipRidePos(0);
    }
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        dComIfGp_evmng_cutEnd(mStaffIdx);
    }
    return TRUE;
}
/* 02424070 */
BOOL daPy_lk_c::dProcStandItemPut_init() {
    if (mCurProc == player_demo::daPyProc_DEMO_STAND_ITEM_PUT_e) {
        return TRUE;
    }
    commonProcInit(player_demo::daPyProc_DEMO_STAND_ITEM_PUT_e);
    mNormalSpeed = 0.0f;
    keepItemData();
    mProcVar6 = 0;
    fopAc_ac_c* partner = fopAcM_getEventPartner(this);
    csXyz* angle;
    if (partner != nullptr) {
        cXyz d;
        cXyz_mi(&partner->current.pos, &d, &current.pos);
        shape_angle.y = fopAcM_searchActorAngleY(this, partner);
        cXyz xz; /* absXZ() */
        xz.x = d.x;
        xz.y = 0.0f;
        xz.z = d.z;
        f32 dist = std_sqrtf(PSVECSquareMag(&xz));
        mProcVar6 = 1;
        m35C8 = dist - 47.0f;
        angle = &partner->shape_angle;
    } else {
        m35C8 = 15.0f;
        mProcVar6 = 1;
        angle = &shape_angle;
    }
    setSingleMoveAnime(player_demo::ANM_GRABWAIT, 1.0f /* HD: HIO folded */, 0.0f, -1, -1.0f);
    fopAc_ac_c* item = fopAcM_fastCreate(player_demo::fpcNm_STANDITEM_e, (u32)dComIfGp_event_getPreItemNo(),
                                         &current.pos, -1, angle, nullptr, -1, 0, 0);
    if (item != nullptr) {
        playerDemoSetActorKeep(&mActorKeepGrab, item);
        fopAcM_setCarryNow(item, 0);
    }
    return TRUE;
}
/* 02424240 */
BOOL daPy_lk_c::dProcStandItemPut() {
    if (mProcVar6 == 1) {
        if (playerDemoOldFrame(m_old_fdata)->mOldFrameRate < 0.01f) { /* m_old_fdata->getOldFrameRate() */
            setSingleMoveAnime(player_demo::ANM_GRABUP, -1.1f, 0.0f, 7, 0.0f); /* HD: HIO folded */
            mProcVar6 = 2;
            playerDemoVoiceStart(this, 7);
        }
    } else if (mFrameCtrlUnder[0].getRate() > -0.01f) {
        fopAc_ac_c* grab = mActorKeepGrab.mActor;
        if (grab != nullptr) {
            playerDemoActorSound(grab, player_demo::JA_SE_LK_W_DAIZA_ATTACH, 0);
        }
        freeGrabItem();
        dComIfGp_evmng_cutEnd(mStaffIdx);
    }
    return TRUE;
}
/* 02424348 */
BOOL daPy_lk_c::dProcVorcanoFail_init() {
    if (mCurProc == player_demo::daPyProc_DEMO_VORCANO_FAIL_e) {
        return TRUE;
    }
    commonProcInit(player_demo::daPyProc_DEMO_VORCANO_FAIL_e);
    mNormalSpeed = 0.0f;
    speed.y = 18.0f;
    gravity = 0.0f;
    setSingleMoveAnime(player_demo::ANM_LAVADAM, 1.0f, 0.0f, -1, 5.0f);
    Mtx34* mtx = mpCLModel->getAnmMtx(player_demo::CL_JNT_LINK_ROOT_e);
    playerDemoMakeEmitter(&m32E4, 0x8078 /* ID_AK_SN_HIDARUMAFIRE */, mtx, &current.pos, nullptr);
    return TRUE;
}
/* 024243F8 */
BOOL daPy_lk_c::dProcVorcanoFail() {
    dComIfGp_evmng_cutEnd(mStaffIdx);
    return TRUE;
}
/* 02424434 */
BOOL daPy_lk_c::dProcSlightSurprised_init() {
    if (mCurProc == player_demo::daPyProc_DEMO_SLIGHT_SURPRISED_e) {
        return FALSE;
    }
    int use = checkShipRideUseItem(1);
    commonProcInit(player_demo::daPyProc_DEMO_SLIGHT_SURPRISED_e);
    setSingleMoveAnime(player_demo::ANM_SURPRISEDB, 1.0f, 0.0f, -1, 5.0f);
    s32 p0 = mDemo.getParam0();
    mNormalSpeed = 0.0f;
    mProcVar6 = 0;
    if (p0 == 1) {
        playerDemoVoiceStart(this, 28);
    } else if (p0 == 2) {
        playerDemoVoiceStart(this, 45);
    } else if (p0 == 3) {
        playerDemoVoiceStart(this, 49);
    }
    initShipRideUseItem(use, 0);
    return TRUE;
}
/* 024245AC */
BOOL daPy_lk_c::dProcSlightSurprised() {
    if (dComIfGp_checkPlayerStatus0(0, player_demo::daPyStts0_SHIP_RIDE_e)) {
        setShipRidePos(0);
    }
    if (mProcVar6 != 0 || mFrameCtrlUnder[0].getRate() < 0.01f) {
        dComIfGp_evmng_cutEnd(mStaffIdx);
        if (mProcVar6 == 0) {
            mProcVar6 = 1;
            setSingleMoveAnime(player_demo::ANM_SURPRISEDWAIT, 1.0f, 0.0f, -1, 3.0f);
            if (gabi::at<PlayerDemoAnimation>(mpAnmTexPatternData)->getFrameMax() == 0) {
                m3530 = 0;
            } else {
                m3530 = gabi::at<PlayerDemoAnimation>(mpAnmTexPatternData)->getFrameMax() - 1;
            }
            if (gabi::at<PlayerDemoAnimation>(mpTexScrollResData)->getFrameMax() == 0) {
                m3532 = 0;
            } else {
                m3532 = gabi::at<PlayerDemoAnimation>(mpTexScrollResData)->getFrameMax() - 1;
            }
            mModeFlg = mModeFlg & ~player_demo::ModeFlg_00000400;
        }
    }
    return TRUE;
}
/* 02424724 */
BOOL daPy_lk_c::dProcSmile_init() {
    if (mCurProc == player_demo::daPyProc_DEMO_SMILE_e) {
        return FALSE;
    }
    int use = checkShipRideUseItem(1);
    commonProcInit(player_demo::daPyProc_DEMO_SMILE_e);
    setSingleMoveAnime(player_demo::ANM_WAITS, 1.0f, 0.0f, -1, 5.0f);
    setTextureAnime(143, 0);
    mNormalSpeed = 0.0f;
    mProcVar6 = 0;
    initShipRideUseItem(use, 1);
    return TRUE;
}
/* 02424818 */
BOOL daPy_lk_c::dProcSmile() {
    if (dComIfGp_checkPlayerStatus0(0, player_demo::daPyStts0_SHIP_RIDE_e)) {
        setShipRidePos(1);
    }
    m3530 = m3530 + 1;
    if ((s32)(u16)m3530 >= gabi::at<PlayerDemoAnimation>(mpAnmTexPatternData)->getFrameMax()) {
        if (mProcVar6 == 0) {
            mProcVar6 = 1;
        } else {
            dComIfGp_evmng_cutEnd(mStaffIdx);
        }
    }
    return TRUE;
}
/* 024248C0 */
BOOL daPy_lk_c::dProcBossWarp_init() {
    if (mCurProc == player_demo::daPyProc_DEMO_BOSS_WARP_e) {
        return TRUE;
    }
    commonProcInit(player_demo::daPyProc_DEMO_BOSS_WARP_e);
    if (mDemo.getParam0() == 0) {
        setSingleMoveAnime(player_demo::ANM_WARPOUTFIRST, 1.0f, 0.0f, -1, 3.0f);
        mProcVar6 = 1;
        deleteEquipItem(0);
    } else {
        setSingleMoveAnime(player_demo::ANM_WARPIN, 1.0f, 0.0f, -1, -1.0f);
        mProcVar6 = 0;
    }
    mNormalSpeed = 0.0f;
    if (mDemo.getParam0() == 2) {
        initShipRideUseItem(2, 1);
        m353E = m353C = 0;
    }
    if (mEquipItem == player_demo::daPyItem_BOKO_e) {
        deleteEquipItem(0);
    }
    return TRUE;
}
/* 024249DC */
BOOL daPy_lk_c::dProcBossWarp() {
    if (dComIfGp_checkPlayerStatus0(0, player_demo::daPyStts0_SHIP_RIDE_e)) {
        setShipRidePos(1);
        m353E = m353C = 0;
    }
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        if (mProcVar6 == 0) {
            dComIfGp_evmng_cutEnd(mStaffIdx);
        } else {
            mProcVar6 = 0;
            setSingleMoveAnime(player_demo::ANM_WARPOUT, 1.0f, 0.0f, -1, 10.0f);
        }
    }
    return TRUE;
}
/* 02424AA0 */
BOOL daPy_lk_c::dProcAgbUse_init() {
    if (mCurProc == player_demo::daPyProc_DEMO_AGB_USE_e) {
        return TRUE;
    }
    int use = checkShipRideUseItem(1);
    mProcVar6 = 0;
    commonProcInit(player_demo::daPyProc_DEMO_AGB_USE_e);
    keepItemData();
    setAnimeEquipItem();
    mNormalSpeed = 0.0f;
    setSingleMoveAnime(player_demo::ANM_USETCEIVER, 1.0f, 0.0f, -1, 2.4f /* HD: HIO folded */);
    playerDemoSystemSound(player_demo::JA_SE_TALK_START);
    initShipRideUseItem(use, 2);
    return TRUE;
}
/* 02424B58 */
BOOL daPy_lk_c::dProcAgbUse() {
    setShipRidePosUseItem();
    dComIfGp_evmng_cutEnd(mStaffIdx);
    return TRUE;
}
/* 02424B9C */
BOOL daPy_lk_c::dProcLookTurn_init() {
    if (mCurProc == player_demo::daPyProc_DEMO_LOOK_TURN_e) {
        return FALSE;
    }
    commonProcInit(player_demo::daPyProc_DEMO_LOOK_TURN_e);
    mNormalSpeed = 0.0f;
    int anm = mDemo.getParam0() ? player_demo::ANM_WINDL : player_demo::ANM_WINDR;
    setSingleMoveAnime(anm, 1.0f, 0.0f, -1, 5.0f);
    mProcVar6 = 0;
    return TRUE;
}
/* 02424C3C */
BOOL daPy_lk_c::dProcLookTurn() {
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        setBlendMoveAnime(5.0f);
        shape_angle.y = (s16)(shape_angle.y - 0x8000);
        current.angle.y = shape_angle.y;
        setOldRootQuaternion(0, -0x8000, 0);
        mProcVar6 = 1;
    }
    if (mProcVar6 != 0 && playerDemoOldFrame(m_old_fdata)->mOldFrameRate < 0.01f) {
        dComIfGp_evmng_cutEnd(mStaffIdx);
    }
    return TRUE;
}
/* 02424D14 */
BOOL daPy_lk_c::dProcLetterOpen_init() {
    if (mCurProc == player_demo::daPyProc_DEMO_LETTER_OPEN_e) {
        return TRUE;
    }
    int use = checkShipRideUseItem(1);
    commonProcInit(player_demo::daPyProc_DEMO_LETTER_OPEN_e);
    mNormalSpeed = 0.0f;
    setSingleMoveAnime(player_demo::ANM_GETLETTER, 1.0f, 0.0f, -1, 3.0f);
    deleteEquipItem(0);
    initShipRideUseItem(use, 2);
    return TRUE;
}
/* 02424DB8 */
BOOL daPy_lk_c::dProcLetterOpen() {
    setShipRidePosUseItem();
    m35EC = mFrameCtrlUnder[0].getFrame();
    if (playerDemoOldFrame(m_old_fdata)->mOldFrameRate < 0.01f) {
        setLetterModel();
    }
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        setLetterModel();
        dComIfGp_evmng_cutEnd(mStaffIdx);
    }
    return TRUE;
}
/* 02424E50 */
BOOL daPy_lk_c::dProcLetterRead_init() {
    if (mCurProc == player_demo::daPyProc_DEMO_LETTER_READ_e) {
        return TRUE;
    }
    f32 keep = m35EC;
    int use = checkShipRideUseItem(0);
    commonProcInit(player_demo::daPyProc_DEMO_LETTER_READ_e);
    mNormalSpeed = 0.0f;
    m35EC = keep;
    setSingleMoveAnime(player_demo::ANM_WAITLETTER, 1.0f, 0.0f, -1, 5.0f);
    setNoResetFlg1(noResetFlg1() & ~0x4000u); /* offNoResetFlg1(daPyFlg1_LETTER_READ_EYE_MOVE) */
    mProcVar0 = 0;
    initShipRideUseItem(use, 2);
    return TRUE;
}
/* 02424F1C */
BOOL daPy_lk_c::dProcLetterRead() {
    setShipRidePosUseItem();
    be<u16>* texScrollIdx = &playerDemoView(this).mTexScrollHeap.mIdx; /* m_tex_scroll_heap.mIdx */
    if (noResetFlg1() & 0x4000) {
        setNoResetFlg1(noResetFlg1() & ~0x4000u);
        *texScrollIdx = player_demo::dRes_INDEX_LKANM_BTK_TEDL_e;
        u32 res = loadTextureScrollResource(0x181, 0);
        setTextureScrollResource(res, 0);
        mProcVar0 = 4;
    } else if (mProcVar0 > 0) {
        mProcVar0 = mProcVar0 - 1;
        if (mProcVar0 == 0) {
            if (*texScrollIdx == 0x181) {
                mProcVar0 = 4;
                *texScrollIdx = player_demo::dRes_INDEX_LKANM_BTK_TEDR_e;
                u32 res = loadTextureScrollResource(0x182, 0);
                setTextureScrollResource(res, 0);
            } else {
                *texScrollIdx = player_demo::dRes_INDEX_LKANM_BTK_TEDW_e;
                u32 res = loadTextureScrollResource(0x183, 0);
                setTextureScrollResource(res, 0);
            }
        }
    }
    dComIfGp_evmng_cutEnd(mStaffIdx);
    return TRUE;
}
/* 02425048 */
BOOL daPy_lk_c::dProcRedeadStop_init() {
    if (mCurProc == player_demo::daPyProc_DEMO_REDEAD_STOP_e) {
        return FALSE;
    }
    commonProcInit(player_demo::daPyProc_DEMO_REDEAD_STOP_e);
    mNormalSpeed = 0.0f;
    setSingleMoveAnime(player_demo::ANM_LINK_FREEZ, 1.0f, 0.0f, -1, 5.0f);
    return TRUE;
}
/* 024250D0 */
BOOL daPy_lk_c::dProcRedeadStop() {
    dComIfGp_evmng_cutEnd(mStaffIdx);
    return TRUE;
}
/* 0242510C */
BOOL daPy_lk_c::dProcRedeadCatch_init() {
    if (mCurProc == player_demo::daPyProc_DEMO_REDEAD_CATCH_e) {
        return FALSE;
    }
    commonProcInit(player_demo::daPyProc_DEMO_REDEAD_CATCH_e);
    mNormalSpeed = 0.0f;
    setSingleMoveAnime(player_demo::ANM_LINK_MOGAKI, 1.0f, 0.0f, -1, 5.0f);
    return TRUE;
}
/* 02425194 */
BOOL daPy_lk_c::dProcRedeadCatch() {
    dComIfGp_evmng_cutEnd(mStaffIdx);
    return TRUE;
}
/* 024251D0 */
BOOL daPy_lk_c::dProcGetDance_init() {
    if (mCurProc == player_demo::daPyProc_DEMO_GET_DANCE_e) {
        return TRUE;
    }
    int use = checkShipRideUseItem(0);
    commonProcInit(player_demo::daPyProc_DEMO_GET_DANCE_e);
    mNormalSpeed = 0.0f;
    setSingleMoveAnime(player_demo::ANM_TAKTDGE, 1.0f, 0.0f, -1, 3.0f);
    mProcVar2 = 0;
    s32 camIdx = mCameraInfoIdx;
    s16 camAngleY = playerDemoCamera(camIdx)->mAngleY;
    mProcVar0 = 0;
    current.angle.y = shape_angle.y;
    mProcVar3 = (s16)(shape_angle.y - camAngleY);
    mGameOverId = 0xFFFFFFFF; /* fpcM_ERROR_PROCESS_ID_e */
    mProcVar6 = mDemo.getParam0() + 0xD2;
    playerDemoOnTact(dComIfGs_ea() + 0xD4, (u32)playerDemoParamByte(this));
    initShipRideUseItem(use, 0);
    dKy_Itemgetcol_chg_on();
    setTactModel();
    mProcVar7 = 0;
    return TRUE;
}
/* 024252E4 */
BOOL daPy_lk_c::dProcGetDance() {
    if (dComIfGp_checkPlayerStatus0(0, player_demo::daPyStts0_SHIP_RIDE_e)) {
        setShipRidePos(0);
    }
    if (mProcVar7 != 0) {
        dComIfGp_evmng_cutEnd(mStaffIdx);
        return TRUE;
    }
    f32 frame = mFrameCtrlUnder[0].getFrame();
    if (frame > 17.0f) {
        shape_angle.y = (s16)(current.angle.y - mProcVar3);
    } else if (!(frame < 11.0f)) { /* HD: the frame range test as the code branches */
        if (mProcVar0 == 0) {
            mProcVar0 = 1;
            mDoAud_subBgmStart(player_demo::JA_BGM_GET_SONG);
            playerDemoMessageBgmOn(); /* dComIfGp_setMesgBgmOn() */
            frame = mFrameCtrlUnder[0].getFrame();
        }
        f32 t = frame - 10.0f;
        if (mProcVar3 > 0x4000) {
            shape_angle.y =
                (s16)gabi::ftoi((f32)(s32)current.angle.y + ((f32)(s32)(0x10000 - mProcVar3) * t) / 7.0f);
        } else {
            shape_angle.y = (s16)gabi::ftoi((f32)(s32)current.angle.y - ((f32)(s32)mProcVar3 * t) / 7.0f);
        }
    }
    fopAcM_getItemEventPartner(this);
    if (mFrameCtrlUnder[0].getRate() < 0.01f && checkEndMessage((s32)mProcVar6)) {
        shape_angle.y = (s16)(shape_angle.y + 0x8000);
        m34DE = shape_angle.y;
        current.angle.y = shape_angle.y;
        setOldRootQuaternion(0, -0x8000, 0);
        PlayerDemoTransform* info = reinterpret_cast<PlayerDemoTransform*>(
            playerDemoOldFrame(m_old_fdata)
                ->mpOldFrameTransInfo.get()); /* m_old_fdata->getOldFrameTransInfo(0) */
        f32 x = (f32)info->mTranslate.x;
        f32 z = (f32)info->mTranslate.z;
        info->mTranslate.x = -x;
        info->mTranslate.z = -z;
        dComIfGp_evmng_cutEnd(mStaffIdx);
        mProcVar7 = 1;
        setBlendMoveAnime(2.4f /* HD: HIO folded */);
    }
    return TRUE;
}
/* 02425548 */
BOOL daPy_lk_c::dProcBottleOpenFairy_init() {
    if (mCurProc == player_demo::daPyProc_DEMO_BOTTLE_OPEN_FAIRY_e) {
        return TRUE;
    }
    commonProcInit(player_demo::daPyProc_DEMO_BOTTLE_OPEN_FAIRY_e);
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    setSingleMoveAnime(player_demo::ANM_BINOPENPRE, 1.0f, 0.0f, 0x2D, 2.0f); /* HD: HIO folded */
    keepItemData();
    setBottleModel(player_demo::dItemNo_FAIRY_BOTTLE_e);
    dComIfGp_setPlayerStatus1(0, 0x4000);
    mProcVar6 = 0;
    return TRUE;
}
/* 024255EC */
BOOL daPy_lk_c::dProcBottleOpenFairy() {
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    if (playerDemoView(this).mAnmHeapUnder[0].mIdx == 0x30 /* m_anm_heap_under[0].mIdx == BINOPENPRE */) {
        if (frameCtrl.getRate() < 0.01f) {
            setSingleMoveAnime(player_demo::ANM_BINOPENB, 1.0f, 0.0f, 0x7C, 5.0f);
        } else if (frameCtrl.checkPass(32.0f)) {
            m355E = 0;
            mRightHandIdx = 8;
        }
    } else if (frameCtrl.getRate() < 0.01f) {
        dComIfGp_evmng_cutEnd(mStaffIdx);
    } else if (playerDemoView(this).mpBottleContentsModel != nullptr) {
        if (!(frameCtrl.getFrame() < 0.0f)) { /* HD: HIO field_0x58 folded */
            playerDemoView(this).mpBottleContentsModel = nullptr;
            u32 btn;
            if (dComIfGp_event_getTalkXYBtn() == 1 /* dTalkBtn_X_e */) {
                btn = 0;
            } else {
                btn = dComIfGp_event_getTalkXYBtn() == 2 ? 1 : 2;
            }
            playerDemoEmptyBottle(dComIfGs_ea() + 0x5C, btn);
            fopAcM_fastCreate(player_demo::fpcNm_NPC_FA1_e, 3 /* Type_BABA_e */, &mLeftHandPos, (s32)current.roomNo,
                              &shape_angle, nullptr, -1, 0, 0);
        }
    }
    return TRUE;
}
/* 02425788 */
BOOL daPy_lk_c::dProcWarpShort_init() {
    if (mCurProc == player_demo::daPyProc_DEMO_WARP_SHORT_e) {
        return TRUE;
    }
    commonProcInit(player_demo::daPyProc_DEMO_WARP_SHORT_e);
    setSingleMoveAnime(player_demo::ANM_RISE, 1.0f, 0.0f, -1, 40.0f);
    gravity = mNormalSpeed = 0.0f;
    speed.y = 0.5f; /* HD: HIO folded */
    if (mEquipItem == player_demo::daPyItem_BOKO_e) {
        deleteEquipItem(0);
    }
    return TRUE;
}
/* 0242583C */
BOOL daPy_lk_c::dProcWarpShort() {
    f32 v = gabi::fadds_ppc(speed.y, 0.1f);
    if (v > 1.35f) {
        v = 1.35f;
    }
    speed.y = v;
    dComIfGp_evmng_cutEnd(mStaffIdx);
    return TRUE;
}
/* 024258A0 */
BOOL daPy_lk_c::dProcOpenSalvageTreasure_init() {
    if (mCurProc == player_demo::daPyProc_DEMO_OPEN_SALVAGE_TREASURE_e) {
        return TRUE;
    }
    int use = checkShipRideUseItem(1);
    commonProcInit(player_demo::daPyProc_DEMO_OPEN_SALVAGE_TREASURE_e);
    setSingleMoveAnime(player_demo::ANM_BOXOPENSLINK, 1.0f, 0.0f, -1, -1.0f);
    deleteEquipItem(0);
    initShipRideUseItem(use, 1);
    fopAc_ac_c* ship = dComIfGp_getShipActor(); /* dComIfGp_getShipActor() */
    if (ship != nullptr) {
        shape_angle.y = ship->shape_angle.y;
        current.angle.y = shape_angle.y;
    }
    return TRUE;
}
/* 0242595C */
BOOL daPy_lk_c::dProcOpenSalvageTreasure() {
    if (dComIfGp_checkPlayerStatus0(0, player_demo::daPyStts0_SHIP_RIDE_e)) {
        setShipRidePos(1);
    }
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        dComIfGp_evmng_cutEnd(mStaffIdx);
    }
    return TRUE;
}
/* 024259D8 */
BOOL daPy_lk_c::dProcSurprisedWait_init() {
    if (mCurProc == player_demo::daPyProc_DEMO_SURPRISED_WAIT_e) {
        return FALSE;
    }
    commonProcInit(player_demo::daPyProc_DEMO_SURPRISED_WAIT_e);
    int use = checkShipRideUseItem(0);
    setSingleMoveAnime(player_demo::ANM_SURPRISEDWAIT, 1.0f, 0.0f, -1, 3.0f);
    if (gabi::at<PlayerDemoAnimation>(mpAnmTexPatternData)->getFrameMax() >= 1) {
        m3530 = gabi::at<PlayerDemoAnimation>(mpAnmTexPatternData)->getFrameMax() - 1;
    }
    if (gabi::at<PlayerDemoAnimation>(mpTexScrollResData)->getFrameMax() >= 1) {
        m3532 = gabi::at<PlayerDemoAnimation>(mpTexScrollResData)->getFrameMax() - 1;
    }
    mNormalSpeed = 0.0f;
    initShipRideUseItem(use, 0);
    return TRUE;
}
/* 02425B24 */
BOOL daPy_lk_c::dProcSurprisedWait() {
    if (dComIfGp_checkPlayerStatus0(0, player_demo::daPyStts0_SHIP_RIDE_e)) {
        setShipRidePos(0);
    }
    dComIfGp_evmng_cutEnd(mStaffIdx);
    return TRUE;
}
/* 02425B80 */
BOOL daPy_lk_c::dProcPowerUpWait_init() {
    if (mCurProc == player_demo::daPyProc_DEMO_POWER_UP_WAIT_e) {
        return TRUE;
    }
    int use = checkShipRideUseItem(1);
    commonProcInit(player_demo::daPyProc_DEMO_POWER_UP_WAIT_e);
    setSingleMoveAnime(player_demo::ANM_POWUPWAIT, 1.0f, 0.0f, -1, 3.0f);
    mNormalSpeed = 0.0f;
    initShipRideUseItem(use, 2);
    if (dComIfGp_checkPlayerStatus0(0, player_demo::daPyStts0_SHIP_RIDE_e)) {
        shape_angle.y = (s16)(shape_angle.y - 0x8000);
        current.angle.y = shape_angle.y;
    }
    return TRUE;
}
/* 02425C54 */
BOOL daPy_lk_c::dProcPowerUp_init() {
    if (mCurProc == player_demo::daPyProc_DEMO_POWER_UP_e) {
        return TRUE;
    }
    int use = checkShipRideUseItem(1);
    commonProcInit(player_demo::daPyProc_DEMO_POWER_UP_e);
    setSingleMoveAnime(player_demo::ANM_POWUP, 1.0f, 0.0f, -1, 3.0f);
    mNormalSpeed = 0.0f;
    m3624 = 0;
    mProcVar0 = 0;
    s32 p0 = mDemo.getParam0();
    if (p0 != 0) {
        if (p0 != 0x100) {
            mProcVar6 = p0;
        } else {
            mProcVar6 = playerDemoGetItem(); /* dComIfGp_event_getGtItm() */
        }
    }
    mProcVar7 = 0;
    mGameOverId = 0xFFFFFFFF;
    initShipRideUseItem(use, 2);
    if (dComIfGp_checkPlayerStatus0(0, player_demo::daPyStts0_SHIP_RIDE_e)) {
        shape_angle.y = (s16)(shape_angle.y - 0x8000);
        current.angle.y = shape_angle.y;
    }
    return TRUE;
}
/* 02425D68 */
BOOL daPy_lk_c::dProcPowerUp() {
    setShipRidePosUseItem();
    if (dComIfGp_checkPlayerStatus0(0, player_demo::daPyStts0_SHIP_RIDE_e)) {
        shape_angle.y = (s16)(shape_angle.y - 0x8000);
        current.angle.y = shape_angle.y;
    }
    if (mProcVar7 != 0) {
        dComIfGp_evmng_cutEnd(mStaffIdx);
        return TRUE;
    } else if (!(mFrameCtrlUnder[0].getFrame() < 11.0f) && mProcVar0 == 0) {
        mProcVar0 = 1;
        mDoAud_subBgmStart(player_demo::JA_BGM_ITEM_GET);
        playerDemoMessageBgmOn(); /* dComIfGp_setMesgBgmOn() */
    } else if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        if (m3624 == 0) {
            m3624 = mProcVar6 + 0x65; /* MSG_NO_FOR_ITEM */
        }
        if (checkEndMessage((u32)m3624)) {
            dComIfGp_evmng_cutEnd(mStaffIdx);
            setBlendMoveAnime(2.4f /* HD: HIO folded */);
            mProcVar7 = 1;
        }
    }
    return TRUE;
}
/* 02425E98 */
BOOL daPy_lk_c::dProcShipSit_init() {
    if (mCurProc == player_demo::daPyProc_DEMO_SHIP_SIT_e) {
        return TRUE;
    }
    int use = checkShipRideUseItem(1);
    if (use == 0) {
        if (dComIfGp_getShipActor() == nullptr) { /* !dComIfGp_getShipActor() */
            return checkNextMode(0);
        }
        use = 1;
    }
    commonProcInit(player_demo::daPyProc_DEMO_SHIP_SIT_e);
    setSingleMoveAnime(player_demo::ANM_KOSHIKAKE, 1.0f, 0.0f, -1, -1.0f);
    mNormalSpeed = 0.0f;
    initShipRideUseItem(use, 1);
    PlayerDemoShip* ship = reinterpret_cast<PlayerDemoShip*>(dComIfGp_getShipActor());
    ship->mLinkSit = 2;        /* ship->onLinkSit() */
    ship->mFlags |= 0x4000000; /* ship->setPaddleMove() */
    return TRUE;
}
/* 02425FD8 */
BOOL daPy_lk_c::dProcShipSit() {
    if (dComIfGp_checkPlayerStatus0(0, player_demo::daPyStts0_SHIP_RIDE_e)) {
        setShipRidePos(1);
    }
    dComIfGp_evmng_cutEnd(mStaffIdx);
    return TRUE;
}
/* 02426034 */
BOOL daPy_lk_c::dProcLastCombo_init() {
    if (mCurProc == player_demo::daPyProc_DEMO_LAST_COMBO_e) {
        return TRUE;
    }
    commonProcInit(player_demo::daPyProc_DEMO_LAST_COMBO_e);
    setSingleMoveAnime(player_demo::ANM_COMBO_LINK, 1.0f, 0.0f, -1, -1.0f);
    mNormalSpeed = 0.0f;
    if (mEquipItem != player_demo::daPyItem_SWORD_e) {
        deleteEquipItem(0);
        setSwordModel(0);
    }
    u32 bck = getItemAnimeResource(player_demo::dRes_INDEX_LKANM_BCK_COMBO_KEN_e);
    playerDemoChangeBckOnly(&playerDemoView(this).mSwordAnim, bck);
    J3DModel* shms = playerDemoView(this).mpShmsModel; /* mpShmsModel */
    m35E8 = m35EC = 0.0f;
    bck = gabi::ea(dComIfG_getObjectRes("Link", player_demo::dRes_INDEX_LINK_BCK_COMBO_TATE_e));
    /* mAtngshaBck.init(mpShmsModel->getModelData(), bck, false, EMode_NONE, 0.0f, 0, -1, true) */
    playerDemoInitBck(&playerDemoView(this).mAtngshaBck, gabi::ea(shms->getModelData()), bck, 0, 0, 0.0f, 0,
                      -1, 1);
    mFootEffectPosType = 4;
    setNoResetFlg1(noResetFlg1() & ~0x20000000u); /* offNoResetFlg1(daPyFlg1_LAST_COMBO_WAIT) */
    setBlurPosResource(player_demo::dRes_INDEX_LKANM__LASTCOMBO_POS_e);
    mProcVar6 = 1;
    mProcVar7 = 0;
    seStartOnlyReverb(player_demo::JA_SE_LK_V_TURN_INTO_GN);
    return TRUE;
}
/* 024261A0 */
BOOL daPy_lk_c::dProcLastCombo() {
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    f32 frame = frameCtrl.getFrame();
    m35EC = frame;
    m35E8 = frame;
    if (frameCtrl.getRate() < 0.01f) {
        dComIfGp_evmng_cutEnd(mStaffIdx);
    } else if (frameCtrl.checkPass(27.0f)) {
        resetFootEffect();
        seStartOnlyReverb(player_demo::JA_SE_LK_V_GN_LAST_ATTACK);
    }
    frame = frameCtrl.getFrame();
    if (!(frame < 26.0f) && frame < 35.0f) {
        u32 f = resetFlg0();
        if (!(mNoResetFlg0 & player_demo::daPyFlg0_CUT_AT_FLG)) {
            f |= 1; /* daPyRFlg0_UNK1 */
        }
        setResetFlg0(f | 2);
    }
    return TRUE;
}
/* 02426298 */
BOOL daPy_lk_c::dProcHandUp_init() {
    if (mCurProc == player_demo::daPyProc_DEMO_HAND_UP_e) {
        return TRUE;
    }
    commonProcInit(player_demo::daPyProc_DEMO_HAND_UP_e);
    setSingleMoveAnime(player_demo::ANM_WAITAUCTION, 1.0f, 0.0f, -1, 10.0f);
    mNormalSpeed = 0.0f;
    return TRUE;
}
/* 02426324 */
BOOL daPy_lk_c::dProcHandUp() {
    dComIfGp_evmng_cutEnd(mStaffIdx);
    return TRUE;
}
/* 02426360 */
BOOL daPy_lk_c::dProcIceSlip_init() {
    if (mCurProc == player_demo::daPyProc_DEMO_ICE_SLIP_e) {
        return TRUE;
    }
    commonProcInit(player_demo::daPyProc_DEMO_ICE_SLIP_e);
    setSingleMoveAnime(player_demo::ANM_SLIPICE, 1.0f, 0.0f, -1, 3.0f);
    mNormalSpeed = 0.0f;
    return TRUE;
}
/* 024263EC */
BOOL daPy_lk_c::dProcIceSlip() {
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        dComIfGp_evmng_cutEnd(mStaffIdx);
    }
    return TRUE;
}

/* ---- HD-only demo procs (no GameCube source; written from the WWHD code). They use the
 * letter animations (ANM_GETLETTER) with a text-input step and an extra camera: probably the HD
 * Tingle Bottle (message bottle) feature. Names are descriptive, not original. ---- */
/* 0242643C */
/* HD: init of proc 0xDD (the letter animation played backwards: closing) */
BOOL daPy_lk_c::dProcHDLetterClose_init() {
    if (mCurProc == 0xDD) {
        return TRUE;
    }
    mCurProc = 0xDD; /* HD: stored before checkShipRideUseItem as well */
    int use = checkShipRideUseItem(1);
    commonProcInit(0xDD);
    mNormalSpeed = 0.0f;
    setSingleMoveAnime(player_demo::ANM_GETLETTER, -1.0f, 0.0f, -1, 5.0f);
    mFrameCtrlUnder[0].mAttribute = 1;
    s32 max = playerDemoView(this)
                  .mpLetterAnimation->getFrameMax(); /* [?] the under-move animation (mAnmRatioUnder) */
    mFrameCtrlUnder[0].mFrame = (f32)max;
    deleteEquipItem(0);
    setLetterModel();
    J3DFrameCtrl* sw =
        reinterpret_cast<J3DFrameCtrl*>(&playerDemoView(this).mSwordAnim); /* mSwordAnim's frame control */
    s16 end = sw->mEnd;
    playerDemoCopyFloatBits(&m35EC,
                            &mFrameCtrlUnder[0].mFrame); /* m35EC = frame (bit copy, as the original) */
    sw->mAttribute = 1;
    sw->mFrame = (f32)end;
    sw->mRate = -1.0f;
    initShipRideUseItem(use, 2);
    mProcVar6 = 0;
    return TRUE;
}
/* 024265A0 */
/* HD: the letter (message) is opened, a text input runs, then it is closed */
BOOL daPy_lk_c::dProcHDLetterWrite() {
    setShipRidePosUseItem();
    J3DFrameCtrl& fc = mFrameCtrlUnder[0];
    s32 st = mProcVar6;
    if (st == 0) {
        f32 fr = fc.getFrame();
        if (fr > 40.0f) {
            fc.mRate = 0.75f;
            fr = fc.getFrame();
        }
        if (fr > 43.0f) {
            setSingleMoveAnime(player_demo::ANM_GETLETTER, 1.0f, 0.0f, -1, 5.0f);
            m35A0 = 0.0f;
            mProcVar6 = 1;
        }
        return TRUE;
    }
    if (st == 1) {
        f32 fr = fc.getFrame();
        m35EC = fr;
        fc.mRate = playerDemoFloatSelect(fr - 6.0f, 1.0f, 0.5f);
        if (playerDemoOldFrame(m_old_fdata)->mOldFrameRate < 0.01f) {
            setLetterModel();
        }
        f32 end = (f32)(s32)(fc.mEnd - 1);
        f32 t = m35A0;
        if (!(fc.getFrame() < end)) {
            t = t + 1.0f;
            m35A0 = t;
        }
        if (t > 20.0f) {
            m35A0 = 0.0f;
            mProcVar6 = 2;
        }
        return TRUE;
    }
    if (st == 2) {
        f32 t = m35A0;
        if (t < 0.1f) {
            playerDemoStartTextInput(l_textInputObject, -1); /* HD: starts the text input (unnamed) */
            t = m35A0;
        }
        t = t + 1.0f;
        m35A0 = t;
        if (t < 2.1f) {
            return TRUE;
        }
        if (l_textInputReady != 0 && l_textInputResult == 0) {
            setSingleMoveAnime(player_demo::ANM_GETLETTER, -1.0f, 0.0f, -1, 5.0f);
            fc.mAttribute = 1;
            s32 max = playerDemoView(this).mpLetterAnimation->getFrameMax();
            fc.mFrame = (f32)max;
            deleteEquipItem(0);
            setLetterModel();
            J3DFrameCtrl* sw =
                reinterpret_cast<J3DFrameCtrl*>(&playerDemoView(this).mSwordAnim); /* mSwordAnim */
            s16 end = sw->mEnd;
            mProcVar6 = 3;
            m35A0 = 0.0f;
            sw->mAttribute = 1;
            playerDemoCopyFloatBits(&m35EC, &fc.mFrame); /* bit copy, as the original */
            sw->mFrame = (f32)end;
            sw->mRate = -1.0f;
            return TRUE;
        }
        return dProcHDLetterClose_init();
    }
    if (st == 3) {
        f32 fr = fc.getFrame();
        f32 rate = playerDemoFloatSelect(fr - 6.0f, -1.0f, -0.5f);
        m35EC = fr;
        fc.mRate = rate;
        if (fr > 0.001f) {
            return TRUE;
        }
        mProcVar6 = 4;
        return playerDemoFinishLetter(this);
    }
    return TRUE;
}
/* 0242705C */
/* HD: closes the letter, puts the item away and ends the event camera */
BOOL daPy_lk_c::dProcHDLetterEnd() {
    setShipRidePosUseItem();
    J3DFrameCtrl& fc = mFrameCtrlUnder[0];
    s32 st = mProcVar6;
    f32 rate = 0.0f;
    bool closingAnimation = false;
    if (st == 0) {
        f32 fr = fc.getFrame();
        fc.mRate = playerDemoFloatSelect(fr - 6.0f, -1.0f, -0.5f);
        m35EC = fr;
        if (fr > 0.001f) {
            st = mProcVar6;
            if (st == 1) {
                rate = playerDemoFloatSelect(40.0f - fc.getFrame(), -1.0f, -0.75f);
                closingAnimation = true;
            }
        } else {
            deleteEquipItem(0);
            setSingleMoveAnime(player_demo::ANM_PRESENTATIONA, -1.0f, 0.0f, -1, 5.0f);
            fc.mAttribute = 1;
            mProcVar6 = 1;
            fc.mFrame = 43.0f;
            rate = playerDemoFloatSelect(40.0f - 43.0f, -1.0f, -0.75f);
            closingAnimation = true;
        }
    } else if (st == 1) {
        rate = playerDemoFloatSelect(40.0f - fc.getFrame(), -1.0f, -0.75f);
        closingAnimation = true;
    }
    if (closingAnimation) {
        f32 fr = fc.getFrame();
        fc.mRate = rate;
        if (!(fr > 25.0f)) {
            setSingleMoveAnime(player_demo::ANM_WAITS, 1.0f, 0.0f, -1, 5.0f);
            fc.mAttribute = 2;
            mProcVar6 = 2;
        }
        return TRUE;
    }
    if (st == 2) {
        u32 cam = playerDemoGetCameraBody();
        playerDemoEndEventCamera(cam, (u32)mBsPcId /* fopAcM_GetID */);
        dComIfGp_event_onEventFlag(8);
        endDemoMode();
        mProcVar6 = 3;
    }
    return TRUE;
}
/* 02427230 */
void daPy_lk_c::setBodyAngleXReadyAnime() {
    Mtx34* mtx = mpCLModel->getAnmMtx(player_demo::CL_JNT_BODY_CHN_e);
    cXyz pos; /* mDoMtx_multVecZero */
    pos.x = (f32)mtx->m[0][3];
    pos.y = (f32)mtx->m[1][3];
    pos.z = (f32)mtx->m[2][3];
    fopAc_ac_c* lockOn = mpAttnActorLockOn;
    s16 target;
    if (lockOn != nullptr) {
        cXyz d;
        cXyz_mi(&lockOn->eyePos, &d, &pos);
        d.y = d.y - 20.0f;
        if (std_sqrtf(PSVECSquareMag(&d)) < 1.0f) {
            target = 0;
        } else {
            cXyz xz;
            xz.x = d.x;
            xz.y = 0.0f;
            xz.z = d.z;
            f32 h = std_sqrtf(PSVECSquareMag(&xz));
            target = cM_atan2s(-d.y, h);
            if (target > 0x2000) {
                target = 0x2000;
            } else if (target < -0x2000) {
                target = -0x2000;
            }
        }
    } else {
        target = 0;
    }
    cLib_addCalcAngleS(&mBodyAngle.x, target, 4, 0xC00, 0x180);
}
/* 02427370 */
BOOL daPy_lk_c::checkItemModeActorPointer() {
    if (mActorKeepEquip.mActor != nullptr) {
        return TRUE;
    }
    playerDemoClearActorKeep(&mActorKeepEquip);
    resetActAnimeUpper(player_demo::UPPER_MOVE2_e, -1.0f);
    mEquipItem = player_demo::daPyItem_NONE_e;
    procWait_init();
    return FALSE;
}

/* ---- d_a_player_ladder.inc ---- */
/* 024273EC */
void daPy_lk_c::setLadderFootSe() {
    cXyz end;
    end.x = gabi::fmadds(50.0f, cM_ssin(shape_angle.y), current.pos.x);
    end.y = current.pos.y;
    end.z = gabi::fmadds(50.0f, cM_scos(shape_angle.y), current.pos.z);
    dBgS_LinChk_Set(mLinkLinChk, &current.pos, &end, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        u32 mtrl = dBgS_GetMtrlSndId(dComIfG_Bgsp(), playerDemoLinePoly(this));
        mDoAud_seStart(player_demo::JA_SE_FT_LADDER_CLIMB_D, &current.pos, mtrl, mReverb);
    } else {
        seStartOnlyReverb(player_demo::JA_SE_FT_LADDER_CLIMB_D);
    }
}
/* 024274D0 */
BOOL daPy_lk_c::procLadderDownEnd_init(int param_0) {
    commonProcInit(player_demo::daPyProc_LADDER_DOWN_END_e);
    int anm = param_0 != 0 ? player_demo::ANM_LADDERDWEDL : player_demo::ANM_LADDERDWEDR;
    setSingleMoveAnime(anm, 1.0f, 0.0f, -1, 2.0f); /* HD: HIO folded */
    gravity = 0.0f;
    m34C2 = 7;
    dComIfGp_setPlayerStatus0(0, player_demo::daPyStts0_UNK2000000_e);
    return TRUE;
}
/* 0242757C */
int daPy_lk_c::changeLadderMoveProc(int param_0) {
    cLib_addCalc(&current.pos.x, m370C.x, 0.5f, 0.5f, 0.05f);
    cLib_addCalc(&current.pos.y, m370C.y, 0.5f, 0.5f, 0.05f);
    cLib_addCalc(&current.pos.z, m370C.z, 0.5f, 0.5f, 0.05f);
    if (mStickDistance > 0.05f) {
        s32 d = (s16)(m34E8 - shape_angle.y);
        int iVar2 = d < 0 ? -d : d;
        if (iVar2 > 0x3C72 && iVar2 < 0x438E) {
            return FALSE;
        }
        cXyz start;
        cXyz end;
        f32 x = current.pos.x;
        f32 y = current.pos.y;
        f32 z = current.pos.z;
        start.x = x;
        f32 hdOff = l_ladderOffset; /* HD: a global tuning value (GameCube 50.0f) */
        start.z = z;
        int direction;
        if (iVar2 < 0x4000) {
            start.y = (y + 125.0f) + hdOff;
            direction = 0; /* DIR_FORWARD */
        } else {
            start.y = y - hdOff;
            direction = 1; /* DIR_BACKWARD */
        }
        end.y = start.y;
        end.x = gabi::fmadds(50.0f, cM_ssin(shape_angle.y), x);
        end.z = gabi::fmadds(50.0f, cM_scos(shape_angle.y), z);
        dBgS_LinChk_Set(mLinkLinChk, &start, &end, this);
        if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk) ||
            playerDemoGetWallCode(dComIfG_Bgsp(), playerDemoLinePoly(this)) != 4) {
            if (iVar2 < 0x4000) {
                procLadderUpEnd_init(param_0);
            } else {
                procLadderDownEnd_init(param_0);
            }
        } else {
            procLadderMove_init(param_0, direction, &m370C);
        }
    }
    return FALSE;
}
/* 02427804 */
int daPy_lk_c::setMoveBGLadderCorrect() {
    if (playerDemoChkPolySafe(dComIfG_Bgsp(), playerDemoPolyInfo(this)) &&
        playerDemoChkMoveBG(dComIfG_Bgsp(), playerDemoPolyInfo(this))) {
        playerDemoMoveBgTransPos(dComIfG_Bgsp(), playerDemoPolyInfo(this), true, &current.pos, &current.angle,
                                 &shape_angle);
        playerDemoMoveBgTransPos(dComIfG_Bgsp(), playerDemoPolyInfo(this), true, &m370C, nullptr, nullptr);
    }
    cXyz end;
    end.x = gabi::fmadds(50.0f, cM_ssin(current.angle.y), m370C.x);
    end.y = m370C.y;
    end.z = gabi::fmadds(50.0f, cM_scos(current.angle.y), m370C.z);
    dBgS_LinChk_Set(mLinkLinChk, &m370C, &end, this);
    if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk) ||
        playerDemoGetWallCode(dComIfG_Bgsp(), playerDemoLinePoly(this)) != 4) {
        return procFall_init(1, 6.0f /* HD: HIO folded */);
    }
    playerDemoCopyPoly(playerDemoPolyInfo(this), playerDemoLinePoly(this)); /* mPolyInfo = mLinkLinChk */
    return FALSE;
}
/* 02427974 */
BOOL daPy_lk_c::procLadderUpStart() {
    if (setMoveBGLadderCorrect()) {
        return TRUE;
    }
    if (mProcVar6 == 0) {
        if ((playerDemoView(this).mAnmHeapUpper[2].mIdx == 0xFFFF)) {
            procLadderUpStart_init_sub();
        }
    } else {
        J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
        m34C2 = 5;
        if (frameCtrl.getRate() < 0.01f) {
            dComIfGp_setDoStatus(6 /* dActStts_LET_GO_e */);
            if (mProcVar2 == 0) {
                setLadderFootSe();
                mProcVar2 = 1;
            }
            if ((mItemTrigger & 1)) {
                m34C2 = 0;
                procFall_init(1, 6.0f);
            } else {
                changeLadderMoveProc(1);
            }
        } else if (frameCtrl.checkPass(9.0f)) {
            setLadderFootSe();
        }
    }
    return TRUE;
}
/* 02427A74 */
BOOL daPy_lk_c::procLadderUpEnd() {
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    if (setMoveBGLadderCorrect()) {
        return TRUE;
    }
    if (frameCtrl.getRate() < 0.01f) {
        checkNextMode(0);
    } else {
        if (frameCtrl.checkPass(9.0f) || frameCtrl.checkPass(19.0f)) {
            setLadderFootSe();
        }
        m34C2 = 5;
    }
    return TRUE;
}
/* 02427B20 */
BOOL daPy_lk_c::procLadderDownStart() {
    if (setMoveBGLadderCorrect()) {
        return TRUE;
    }
    if (mProcVar6 == 0) {
        if ((playerDemoView(this).mAnmHeapUpper[2].mIdx == 0xFFFF)) {
            procLadderDownStart_init_sub();
        }
    } else {
        J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
        m34C2 = 5;
        if (frameCtrl.getRate() < 0.01f) {
            dComIfGp_setDoStatus(6 /* dActStts_LET_GO_e */);
            if ((mItemTrigger & 1)) {
                m34C2 = 0;
                procFall_init(1, 6.0f);
            } else {
                changeLadderMoveProc(1);
            }
        } else if (frameCtrl.checkPass(26.0f) || frameCtrl.checkPass(36.0f) || frameCtrl.checkPass(43.0f)) {
            setLadderFootSe();
        }
    }
    return TRUE;
}
/* 02427C40 */
BOOL daPy_lk_c::procLadderDownEnd() {
    if (setMoveBGLadderCorrect()) {
        return TRUE;
    }
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        checkNextMode(0);
    } else {
        m34C2 = 5;
    }
    return TRUE;
}

/* 02427CA8 */
BOOL daPy_lk_c::procLadderMove() {
    if (setMoveBGLadderCorrect()) {
        return TRUE;
    }
    m34C2 = 5;
    dComIfGp_setDoStatus(player_move::dActStts_LET_GO_e);
    if ((mItemTrigger & 1)) {
        m34C2 = 0;
        procFall_init(1, 6.0f);
    } else {
        f32 rate = mFrameCtrlUnder[0].getRate();
        if (std::fabs(rate) < 0.01f) {
            if (mProcVar0 > 0) {
                mProcVar0 = mProcVar0 - 1;
                setLadderFootSe();
            } else {
                changeLadderMoveProc(mProcVar6);
            }
        } else {
            f32 spd = getLadderMoveAnmSpeed();
            if (rate < 0.0f) { /* HD: the rate read before the call */
                spd = -spd;
            }
            mFrameCtrlUnder[0].setRate(spd);
        }
    }
    return TRUE;
}

/* ---- d_a_player_hang.inc ---- */

/* 02427D8C */
f32 daPy_lk_c::getHangMoveAnmSpeed() {
    /* getAnmSpeedStickRate(0.7f, 1.4f) inlined, HIO folded: 0.7 + stick * 0.7 */
    return gabi::fmadds(0.7f, mStickDistance, 0.7f);
}

/* 02427DA0 */
int daPy_lk_c::getHangDirectionFromAngle() {
    s32 d = (s16)(m34E8 - shape_angle.y);
    if ((d < 0 ? -d : d) > 0x78E4) {
        return player_move::DIR_BACKWARD;
    } else if (d >= 0x71C) {
        return player_move::DIR_LEFT;
    } else if (d > -0x71C) {
        return player_move::DIR_FORWARD;
    }
    return player_move::DIR_RIGHT;
}

/* 02427DF0 */
BOOL daPy_lk_c::changeHangMoveProc(int i_direction) {
    if (!(mAcch.m_flags & dBgS_Acch::GROUND_HIT) || !playerMoveChkPolySafe(static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd))) {
        return FALSE;
    }
    cM3dGPla* pla = playerMoveTriPla(static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd));
    if (pla == 0 || pla->mNormal.y < 0.9986f) { /* HD: NULL check */
        return FALSE;
    }
    s16 angle;
    if (i_direction == 2) {
        angle = shape_angle.y + 0x4000;
    } else if (i_direction == 3) {
        angle = shape_angle.y - 0x4000;
    } else {
        angle = current.angle.y;
    }
    gabi::Local<cXyz> local_34; /* sp+0x08 */
    gabi::Local<cXyz> local_28; /* sp+0x14 */
    f32 y = current.pos.y + 5.0f;
    local_28->x = current.pos.x;
    local_28->y = y;
    local_28->z = current.pos.z;
    local_34->x = gabi::fmadds(30.0f, cM_ssin(angle), local_28->x);
    local_34->y = y;
    local_34->z = gabi::fmadds(30.0f, cM_scos(angle), local_28->z);
    dBgS_LinChk_Set(mLinkLinChk, local_28, local_34, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        return FALSE;
    }
    playerMoveGroundCheck(this)->m_pos.copy(*local_34.get()); /* mGndChk.SetPos(&local_34) */
    f64 gc = playerMoveGroundCross(dComIfG_Bgsp(), playerMoveGroundCheck(this));
    f32 posY = current.pos.y;
    if (std::fabs((f32)(gc - (f64)posY)) > 5.0f) {
        gabi::Local<cXyz> local_40; /* sp+0x2C */
        gabi::Local<cXyz> local_4c; /* sp+0x38 */
        f32 y2 = posY - 5.0f;
        local_40->x = local_34->x;
        local_40->y = y2;
        local_40->z = local_34->z;
        local_4c->x = gabi::fmadds(90.0f, cM_ssin(shape_angle.y), local_34->x);
        local_4c->y = y2;
        local_4c->z = gabi::fmadds(90.0f, cM_scos(shape_angle.y), local_34->z);
        dBgS_LinChk_Set(mLinkLinChk, local_40, local_4c, this);
        if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
            return FALSE;
        }
        cM3dGPla* tri = playerMoveTriPla(playerDemoLinePoly(this));
        if (tri == 0) { /* HD: NULL check */
            return FALSE;
        }
        if (cLib_distanceAngleS(cM_atan2s(tri->mNormal.x, tri->mNormal.z), shape_angle.y) < 0x549F) {
            return FALSE;
        }
    }
    gabi::Local<cXyz> local_58; /* sp+0x20 */
    local_58->x = 4.5f * cM_ssin(shape_angle.y);
    local_58->y = 62.5f;
    local_58->z = 4.5f * cM_scos(shape_angle.y);
    playerMoveSubtractVector(local_28, local_58, local_28);
    playerMoveSubtractVector(local_34, local_58, local_34);
    dBgS_LinChk_Set(mLinkLinChk, local_28, local_34, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        cM3dGPla* tri = playerMoveTriPla(playerDemoLinePoly(this));
        if (tri == 0) { /* HD: NULL check */
            return FALSE;
        }
        if (cLib_distanceAngleS(cM_atan2s(tri->mNormal.x, tri->mNormal.z), shape_angle.y) < 0x549F) {
            return FALSE;
        }
    }
    return TRUE;
}

/* 02428230 */
int daPy_lk_c::changeHangEndProc(int param_0) {
    Mtx34* mtx = mpCLModel->getAnmMtx(player_demo::CL_JNT_LINK_ROOT_e);
    s16 a = shape_angle.y;
    /* mGndChk.SetPos(&pos): HD stores the position into the check directly */
    f32 x = gabi::fnmsubs(4.0f, cM_ssin(a), mtx->m[0][3]);
    f32 z = gabi::fnmsubs(4.0f, cM_scos(a), mtx->m[2][3]);
    playerMoveGroundCheck(this)->m_pos.y = mtx->m[1][3];
    playerMoveGroundCheck(this)->m_pos.x = x;
    playerMoveGroundCheck(this)->m_pos.z = z;
    f64 h = playerMoveGroundCross(dComIfG_Bgsp(), playerMoveGroundCheck(this));
    mHangGroundH = (f32)h;
    if (h != -1000000000.0 /* -G_CM3D_F_INF */) {
        m3588 = playerMoveGetAttributeCode(playerMoveGroundPoly(this));
    } else {
        m3588 = player_move::dBgS_Attr_UNK1B_e;
    }
    if (mHangGroundH > playerMoveFoot(this, 1)->field_0x018.y + current.pos.y) {
        return procLand_init(1.3f /* HD: HIO folded */, 0);
    }
    if (param_0 != 0) {
        dComIfGp_setDoStatus(player_move::dActStts_LET_GO_e);
        if ((mItemTrigger & 1)) {
            speed.y = 0.0f;
            return procFall_init(1, 6.0f);
        }
    }
    return FALSE;
}

/* 024283D4 */
void daPy_lk_c::setHangShapeOffset() {
    s16 a = (s16)(m34EC + shape_angle.y);
    f32 sn = cM_ssin(a);
    f32 cs = cM_scos(a);
    f32 y = current.pos.y - 5.0f;
    f32 s50 = 50.0f * sn;
    f32 c50 = 50.0f * cs;
    f32 dVar10 = 20.0f * cs;
    f32 dVar9 = -20.0f * sn;
    gabi::Local<cXyz> local_40; /* sp+0x08 */
    gabi::Local<cXyz> local_4c; /* sp+0x14 */
    gabi::Local<cXyz> local_58; /* sp+0x20 */
    local_40->x = (current.pos.x - s50) + dVar10;
    local_40->y = y;
    local_40->z = (current.pos.z - c50) + dVar9;
    local_4c->x = (current.pos.x + s50) + dVar10;
    local_4c->y = y;
    local_4c->z = (current.pos.z + c50) + dVar9;
    dBgS_LinChk_Set(mLinkLinChk, local_40, local_4c, this);
    if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        m34EC = 0;
        return;
    }
    local_58->copy(playerMoveLineCross(this));
    f32 dx = dVar10 + dVar10;
    f32 dz = dVar9 + dVar9;
    local_4c->x = local_4c->x - dx;
    local_4c->z = local_4c->z - dz;
    local_40->z = local_40->z - dz;
    local_40->x = local_40->x - dx;
    dBgS_LinChk_Set(mLinkLinChk, local_40, local_4c, this);
    if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        m34EC = 0;
    } else {
        playerMoveSubtractVector(local_58, &playerMoveLineCross(this), local_58);
        m34EC = (s16)(cM_atan2s(local_58->x, local_58->z) - 0x4000 - shape_angle.y);
    }
}

/* 024285AC */
BOOL daPy_lk_c::procHangClimb_init(f32 param_0) {
    if (mAcch.m_flags & dBgS_Acch::ROOF_HIT) {
        return FALSE;
    }
    commonProcInit(player_move::daPyProc_HANG_CLIMB_e);
    setSingleMoveAnime(player_move::ANM_VJMPCL, 0.75f, param_0, 0x18, 5.0f); /* HD: HIO folded */
    dComIfGp_setPlayerStatus0(0, player_move::daPyStts0_UNK200_e);
    mNormalSpeed = 0.0f;
    playerDemoVoiceStart(this, 32);
    return TRUE;
}

/* 02428688 */
BOOL daPy_lk_c::procHangWait_init() {
    commonProcInit(player_move::daPyProc_HANG_WAIT_e);
    setSingleMoveAnime(player_move::ANM_VJMPCHA, 0.0f, 7.0f /* HD: HIO folded */, -1, 5.0f);
    setTextureAnime(9, 0);
    speed.y = 0.0f;
    mNormalSpeed = 0.0f;
    mpSeAnmFrameCtrl = 0;
    dComIfGp_setPlayerStatus0(0, player_move::daPyStts0_HANG_e);
    return TRUE;
}

/* 02428730 */
BOOL daPy_lk_c::procHangStart() {
    if (changeHangEndProc(1)) {
        return TRUE;
    }
    f32 rate = mFrameCtrlUnder[0].getRate();
    if (!(rate < 0.01f) && !(mFrameCtrlUnder[0].getFrame() > 6.0f /* HD: HIO folded */)) {
        return TRUE;
    }
    if (mStickDistance > 0.05f || mProcVar6 != 0) {
        if (getDirectionFromShapeAngle() == player_move::DIR_FORWARD || mProcVar6 != 0) {
            procHangClimb_init(0.0f);
        }
    } else if (rate < 0.01f) {
        procHangWait_init();
    }
    return TRUE;
}

/* 02428804 */
BOOL daPy_lk_c::procHangUp_init(int param_0) {
    commonProcInit(player_move::daPyProc_HANG_UP_e);
    setSingleMoveAnime(player_move::ANM_HANGUP, 0.7f, 1.0f, 0xD, 8.0f); /* HD: HIO folded */
    dComIfGp_setPlayerStatus0(0, player_move::daPyStts0_HANG_e);
    mProcVar6 = param_0;
    mNormalSpeed = 0.0f;
    return TRUE;
}

/* 0242888C */
BOOL daPy_lk_c::procHangFallStart() {
    int iVar2;
    if (mFrameCtrlUnder[0].getRate() < 0.01f || mFrameCtrlUnder[0].getFrame() > 10.0f /* HD: HIO folded */) {
        iVar2 = 1;
    } else {
        iVar2 = 0;
    }
    if (changeHangEndProc(iVar2)) {
        return TRUE;
    }
    if (iVar2 != 0) {
        u32 flg = mModeFlg;
        if (flg & player_move::ModeFlg_00000400) {
            flg = (flg & ~player_move::ModeFlg_00000400) | player_move::ModeFlg_00000100;
            mModeFlg = flg;
        }
        if (flg & player_move::ModeFlg_00000100) {
            setTextureAnime(7, 0);
        }
        int dir = getHangDirectionFromAngle();
        if (mStickDistance > 0.05f && dir != 1) {
            procHangUp_init(dir);
        }
    }
    return TRUE;
}

/* 02428988 */
BOOL daPy_lk_c::procHangMove_init(int param_0) {
    commonProcInit(player_move::daPyProc_HANG_MOVE_e);
    int anm;
    if (param_0 == 2) {
        anm = player_move::ANM_HANGMOVEL;
        current.angle.y = shape_angle.y + 0x4000;
    } else {
        anm = player_move::ANM_HANGMOVER;
        current.angle.y = shape_angle.y - 0x4000;
    }
    f64 spd = playerMoveHangSpeedRegister(this); /* f1 passed on as returned */
    playerMoveSingleAnimeRegister(this, anm, spd, 0.0f, -1, 1.0f);
    dComIfGp_setPlayerStatus0(0, player_move::daPyStts0_HANG_e);
    mNormalSpeed = 0.0f;
    gabi::Local<cXyz> xz;
    gabi::Local<cXyz> d;
    cXyz_mi(&mLeftHandPos, d, &mRightHandPos);
    xz->z = d->z;
    xz->y = 0.0f;
    xz->x = d->x;
    m35A0 = std_sqrtf(PSVECSquareMag(xz)); /* absXZ() */
    return TRUE;
}

/* 02428AEC */
BOOL daPy_lk_c::procHangUp() {
    if (changeHangEndProc(1)) {
        if (mCurProc == player_move::daPyProc_FALL_e) {
            current.pos.x = gabi::fnmsubs(8.0f, cM_ssin(shape_angle.y), current.pos.x);
            current.pos.z = gabi::fnmsubs(8.0f, cM_scos(shape_angle.y), current.pos.z);
        }
        return TRUE;
    }
    if (mFrameCtrlUnder[0].getRate() < 0.01f || mFrameCtrlUnder[0].getFrame() > 13.0f /* HD: HIO folded */) {
        if (mProcVar6 == 0) {
            procHangClimb_init(0.0f);
        } else if (changeHangMoveProc(mProcVar6)) {
            procHangMove_init(mProcVar6);
        } else {
            procHangWait_init();
        }
    }
    return TRUE;
}

/* 02428BF4 */
BOOL daPy_lk_c::procHangWait() {
    if (changeHangEndProc(1)) {
        return TRUE;
    }
    setHangShapeOffset();
    if (mStickDistance > 0.05f) {
        int dir = getHangDirectionFromAngle();
        if (dir == 0) {
            procHangClimb_init(0.0f);
        } else if (dir != 1 && changeHangMoveProc(dir)) {
            s16 keep = m34EC;
            procHangMove_init(dir);
            m34EC = keep;
        }
    }
    return TRUE;
}

/* 02428CB4 */
BOOL daPy_lk_c::procHangMove() {
    int dir = getHangDirectionFromAngle();
    if (changeHangEndProc(1)) {
        return TRUE;
    }
    if (dir == 0 && mStickDistance > 0.05f) {
        procHangClimb_init(0.0f);
        return TRUE;
    }
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    frameCtrl.setRate(getHangMoveAnmSpeed());
    if (frameCtrl.checkPass(0.0f)) {
        f32 stick = mStickDistance;
        s16 keep = m34EC;
        if (stick > 0.05f && dir != 1 && changeHangMoveProc(dir)) {
            procHangMove_init(dir);
        } else {
            procHangWait_init();
        }
        m34EC = keep;
    } else {
        gabi::Local<cXyz> xz;    /* sp+0x08 */
        gabi::Local<cXyz> local_64; /* sp+0x2C */
        cXyz_mi(&mRightHandPos, local_64, &mLeftHandPos);
        xz->x = local_64->x;
        xz->y = 0.0f;
        xz->z = local_64->z;
        f32 dVar12 = std_sqrtf(PSVECSquareMag(xz)); /* absXZ() */
        if (changeHangMoveProc(4)) {
            f32 spd = std::fabs(dVar12 - m35A0); /* HD: HIO field_0x30 (1.0) folded */
            mNormalSpeed = spd;
            f32 dVar13 = cM_ssin(current.angle.y);
            f32 fVar1 = cM_ssin(shape_angle.y);
            f32 dVar11 = cM_scos(current.angle.y);
            f32 fVar2 = cM_scos(shape_angle.y);
            gabi::Local<cXyz> local_70; /* sp+0x14 */
            gabi::Local<cXyz> local_7c; /* sp+0x20 */
            f32 x70 = gabi::fnmsubs(30.0f, fVar1, gabi::fmadds(spd, dVar13, current.pos.x));
            f32 z70 = gabi::fnmsubs(30.0f, fVar2, gabi::fmadds(spd, dVar11, current.pos.z));
            f32 y70 = current.pos.y - 5.0f;
            local_70->x = x70;
            local_70->y = y70;
            local_70->z = z70;
            local_7c->x = gabi::fmadds(60.0f, fVar1, x70);
            local_7c->y = y70;
            local_7c->z = gabi::fmadds(60.0f, fVar2, z70);
            dBgS_LinChk_Set(mLinkLinChk, local_70, local_7c, this);
            cM3dGPla* tri;
            if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk) || (tri = playerMoveTriPla(playerDemoLinePoly(this))) == 0) { /* HD: NULL check */
                mNormalSpeed = 0.0f;
                m35A0 = dVar12;
            } else {
                s16 sVar6 = cM_atan2s(tri->mNormal.x, tri->mNormal.z) + 0x8000;
                s16 shapeY = shape_angle.y;
                if (sVar6 != shapeY) {
                    s16 newAngle = (s16)(current.angle.y - shapeY) > 0 ? (s16)(sVar6 + 0x4000) : (s16)(sVar6 - 0x4000);
                    shape_angle.y = sVar6;
                    current.angle.y = newAngle;
                    m34EC = m34EC + (s16)(shapeY - sVar6);
                    /* local_88 = cross - np * 1.5 (+25 in y): HD stores it into mGndChk directly */
                    f32 x = gabi::fnmsubs(1.5f, tri->mNormal.x, playerMoveLineCross(this).x);
                    f32 y = playerMoveLineCross(this).y + 25.0f;
                    f32 z = gabi::fnmsubs(1.5f, tri->mNormal.z, playerMoveLineCross(this).z);
                    playerMoveGroundCheck(this)->m_pos.x = x;
                    playerMoveGroundCheck(this)->m_pos.y = y;
                    playerMoveGroundCheck(this)->m_pos.z = z;
                    f64 gc = playerMoveGroundCross(dComIfG_Bgsp(), playerMoveGroundCheck(this));
                    if (!(std::fabs((f32)(gc - (f64)(f32)current.pos.y)) > 5.0f)) {
                        m35A0 = dVar12;
                        current.pos.x = x;
                        current.pos.z = z;
                        current.pos.y = (f32)gc;
                    } else {
                        m35A0 = dVar12;
                    }
                } else {
                    f32 x = gabi::fnmsubs(1.5f, tri->mNormal.x, playerMoveLineCross(this).x);
                    f32 spd2 = mNormalSpeed;
                    current.pos.x = gabi::fnmsubs(spd2, dVar13, x);
                    f32 z = gabi::fnmsubs(1.5f, tri->mNormal.z, playerMoveLineCross(this).z);
                    m35A0 = dVar12;
                    current.pos.z = gabi::fnmsubs(spd2, dVar11, z);
                }
            }
        } else {
            mNormalSpeed = 0.0f;
            m35A0 = dVar12;
        }
    }
    if (mModeFlg & player_move::ModeFlg_HANG) {
        setHangShapeOffset();
    }
    return TRUE;
}

/* 02429100 */
BOOL daPy_lk_c::procHangClimb() {
    if (mFrameCtrlUnder[0].getRate() < 0.01f) {
        checkNextMode(0);
    } else if (mFrameCtrlUnder[0].getFrame() > 23.0f /* HD: HIO folded */) {
        checkNextMode(1);
    }
    return TRUE;
}

/* 02429160 */
BOOL daPy_lk_c::procHangWallCatch() {
    if (mFrameCtrlUnder[0].getRate() < 0.01f && !procHangClimb_init(2.0f /* HD: HIO folded */)) {
        return procFall_init(1, 6.0f);
    }
    return TRUE;
}

/* ---- d_a_player_climb.inc ---- */

/* 024291E0 */
f32 daPy_lk_c::getClimbMoveAnmSpeed() {
    /* getAnmSpeedStickRate(0.8f, 1.25f) inlined, HIO folded */
    return gabi::fmadds(0.45f, mStickDistance, 0.8f);
}

/* 024291FC */
int daPy_lk_c::getClimbDirectionFromAngle() {
    s32 d = (s16)(m34E8 - shape_angle.y);
    if ((d < 0 ? -d : d) > 0x7000) {
        return player_move::DIR_BACKWARD;
    } else if (d >= 0x1000) {
        return player_move::DIR_LEFT;
    } else if (d > -0x1000) {
        return player_move::DIR_FORWARD;
    }
    return player_move::DIR_RIGHT;
}

/* 0242924C */
BOOL daPy_lk_c::procClimbMoveSide_init(int param_0) {
    f64 spd = playerMoveClimbSpeedRegister(this); /* f1 passed on as returned */
    commonProcInit(player_move::daPyProc_CLIMB_MOVE_SIDE_e);
    m34C2 = 7;
    mProcVar6 = param_0;
    mProcVar2 = (s16)(m34E8 - shape_angle.y);
    int anm;
    if (param_0 != 0) {
        anm = player_move::ANM_FCLIMBSLIDELUP;
        if (mDirection == player_move::DIR_RIGHT) {
            spd = -spd;
        }
    } else {
        anm = player_move::ANM_FCLIMBSLIDERUP;
        if (mDirection == player_move::DIR_LEFT) {
            spd = -spd;
        }
    }
    playerMoveSingleAnimeRegister(this, anm, spd, 0.0f, 0xD, 0.0f); /* HD: HIO folded */
    gravity = 0.0f;
    speedF = 0.0f;
    mProcVar0 = 1;
    mNormalSpeed = 0.0f;
    speed.y = 0.0f;
    dComIfGp_setPlayerStatus1(0, player_move::daPyStts1_UNK10000_e);
    gabi::Local<cXyz> xz;
    gabi::Local<cXyz> d;
    cXyz_mi(&mRightHandPos, d, &mLeftHandPos);
    xz->x = d->x;
    xz->z = d->z;
    xz->y = 0.0f;
    m35A0 = std_sqrtf(PSVECSquareMag(xz)); /* absXZ() */
    return TRUE;
}

/* 02429388 */
void daPy_lk_c::changeClimbMoveProc(int param_0) {
    f32 stick = mStickDistance;
    s16 keep = m34EC;
    if (stick > 0.05f) {
        u8 dir = (u8)getClimbDirectionFromAngle();
        mDirection = dir;
        if (dir == player_move::DIR_FORWARD || dir == player_move::DIR_BACKWARD) {
            procClimbMoveUpDown_init(param_0);
        } else {
            procClimbMoveSide_init(param_0);
        }
    }
    m34EC = keep;
}

/* 02429408 */
BOOL daPy_lk_c::setMoveBGCorrectClimb() {
    if (playerMoveChkPolySafe(playerDemoPolyInfo(this)) && playerMoveChkMoveBG(playerDemoPolyInfo(this))) {
        playerMoveMoveBgTransPos(playerDemoPolyInfo(this), true, &current.pos, &current.angle, &shape_angle);
    }
    if (noResetFlg1() & player_move::daPyFlg1_VINE_CATCH) {
        return procFall_init(1, 6.0f);
    }
    if (mCurProc == player_move::daPyProc_CLIMB_DOWN_START_e && mFrameCtrlUnder[0].getRate() > 0.01f) {
        return FALSE;
    }
    gabi::Local<cXyz> local_1c; /* sp+0x08 */
    gabi::Local<cXyz> local_28; /* sp+0x14 */
    f32 y = current.pos.y + 30.0f;
    local_1c->x = current.pos.x;
    local_1c->y = y;
    local_1c->z = current.pos.z;
    local_28->x = gabi::fmadds(80.0f, cM_ssin(shape_angle.y), current.pos.x);
    local_28->y = y;
    local_28->z = gabi::fmadds(80.0f, cM_scos(shape_angle.y), current.pos.z);
    dBgS_LinChk_Set(mLinkLinChk, local_1c, local_28, this);
    if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk) || playerMoveGetWallCode(playerDemoLinePoly(this)) != 1) {
        return procFall_init(1, 6.0f);
    }
    cM3dGPla* tri = playerMoveTriPla(playerDemoLinePoly(this));
    if (tri != 0 && std::fabs(tri->mNormal.y) > 0.05f) { /* HD: NULL check */
        return procFall_init(1, 6.0f);
    }
    if ((mCurProc == player_move::daPyProc_CLIMB_MOVE_UP_DOWN_e || mCurProc == player_move::daPyProc_CLIMB_MOVE_SIDE_e) &&
        current.pos.y - 15.0f < mAcch.m_ground_h) {
        return procFall_init(1, 6.0f);
    }
    tri = playerMoveTriPla(playerDemoLinePoly(this));
    if (tri == 0) { /* HD: NULL check */
        return FALSE;
    }
    s16 uVar6 = cM_atan2s(tri->mNormal.x, tri->mNormal.z);
    if (cLib_distanceAngleS(uVar6, shape_angle.y) <= 0x549F) {
        return procFall_init(1, 6.0f);
    }
    current.pos.x = gabi::fmadds(20.5f, cM_ssin(uVar6), playerMoveLineCross(this).x);
    s16 sVar1 = shape_angle.y;
    s16 newShape = uVar6 + 0x8000;
    shape_angle.y = newShape;
    current.angle.y = current.angle.y + (s16)(newShape - sVar1);
    current.pos.z = gabi::fmadds(20.5f, cM_scos(uVar6), playerMoveLineCross(this).z);
    m34EC = m34EC + (s16)(sVar1 - newShape);
    return FALSE;
}

/* 024296A8 */
void daPy_lk_c::checkBgCorrectClimbMove(cXyz* i_startPos, cXyz* i_endPos) {
    dBgS_LinChk_Set(mLinkLinChk, i_startPos, i_endPos, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        cM3dGPla* tri = playerMoveTriPla(playerDemoLinePoly(this));
        if (tri != 0) { /* HD: NULL check */
            if (cLib_distanceAngleS(cM_atan2s(tri->mNormal.x, tri->mNormal.z), shape_angle.y) < 0x549F) {
                gabi::Local<cXyz> d;
                cXyz_mi(&playerMoveLineCross(this), d, i_endPos);
                playerMoveAddVector(&current.pos, d, &current.pos); /* current.pos += cross - *i_endPos */
            }
        }
    }
}

/* 02429758 */
void daPy_lk_c::setClimbShapeOffset() {
    s16 shapeY = shape_angle.y;
    s16 a = (s16)(m34EC + shapeY);
    f32 fVar4 = cM_ssin(a);
    f32 fVar3 = cM_scos(a);
    f32 x58 = gabi::fmadds(20.5f, cM_ssin(shapeY), current.pos.x);
    f32 y58 = current.pos.y - 62.5f;
    f32 z58 = gabi::fmadds(20.5f, cM_scos(shapeY), current.pos.z);
    f32 s50 = 50.0f * fVar4;
    f32 c50 = 50.0f * fVar3;
    f32 dVar10 = 20.0f * fVar3;
    f32 dVar9 = -20.0f * fVar4;
    gabi::Local<cXyz> local_40; /* sp+0x08 */
    gabi::Local<cXyz> local_4c; /* sp+0x14 */
    gabi::Local<cXyz> local_64; /* sp+0x20 */
    local_40->x = (x58 - s50) + dVar10;
    local_40->y = y58;
    local_40->z = (z58 - c50) + dVar9;
    local_4c->x = (x58 + s50) + dVar10;
    local_4c->y = y58;
    local_4c->z = (z58 + c50) + dVar9;
    dBgS_LinChk_Set(mLinkLinChk, local_40, local_4c, this);
    if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        m34EC = 0;
        return;
    }
    local_64->copy(playerMoveLineCross(this));
    f32 dx = dVar10 + dVar10;
    f32 dz = dVar9 + dVar9;
    local_4c->x = local_4c->x - dx;
    local_4c->z = local_4c->z - dz;
    local_40->z = local_40->z - dz;
    local_40->x = local_40->x - dx;
    dBgS_LinChk_Set(mLinkLinChk, local_40, local_4c, this);
    if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        m34EC = 0;
    } else {
        playerMoveSubtractVector(local_64, &playerMoveLineCross(this), local_64);
        m34EC = (s16)(cM_atan2s(local_64->x, local_64->z) - 0x4000 - shape_angle.y);
    }
}

/* 02429950 */
void daPy_lk_c::checkBgClimbMove() {
    f32 dVar7 = cM_ssin(shape_angle.y);
    f32 dVar6 = cM_scos(shape_angle.y);
    f32 dVar4 = -30.0f * dVar7;
    f32 dVar5 = 30.0f * dVar6;
    gabi::Local<cXyz> local_5c; /* sp+0x08 */
    gabi::Local<cXyz> local_68; /* sp+0x14 */
    local_5c->z = current.pos.z + dVar4;
    local_5c->y = current.pos.y;
    local_5c->x = current.pos.x + dVar5;
    checkBgCorrectClimbMove(&current.pos, local_5c);
    local_5c->y = current.pos.y;
    local_5c->x = current.pos.x - dVar5;
    local_5c->z = current.pos.z - dVar4;
    checkBgCorrectClimbMove(&current.pos, local_5c);
    f32 z = current.pos.z;
    f32 y = current.pos.y + 125.0f;
    f32 x = current.pos.x;
    local_68->z = z;
    local_68->y = y;
    local_5c->y = y;
    local_5c->x = x + dVar5;
    local_5c->z = z + dVar4;
    local_68->x = x;
    checkBgCorrectClimbMove(local_68, local_5c);
    local_5c->y = local_68->y;
    local_5c->x = local_68->x - dVar5;
    local_5c->z = local_68->z - dVar4;
    checkBgCorrectClimbMove(local_68, local_5c);

    /* mRoofChk.SetPos(local_5c): HD stores the position into the check as well */
    f32 py = current.pos.y;
    playerMoveRoof(this)->m_pos.y = py;
    f32 px = current.pos.x + dVar5;
    local_5c->y = py;
    f32 pz = current.pos.z + dVar4;
    local_5c->x = px;
    playerMoveRoof(this)->m_pos.x = px;
    playerMoveRoof(this)->m_pos.z = pz;
    local_5c->z = pz;
    f32 roof = (f32)(playerMoveRoofCheck(dComIfG_Bgsp(), playerMoveRoof(this)) - 125.0);
    py = current.pos.y;
    f32 px0 = current.pos.x;
    if (roof < py) {
        py = roof;
        current.pos.y = roof;
    }
    px = px0 - dVar5;
    pz = current.pos.z - dVar4;
    playerMoveRoof(this)->m_pos.y = py;
    playerMoveRoof(this)->m_pos.x = px;
    local_5c->y = py;
    local_5c->x = px;
    playerMoveRoof(this)->m_pos.z = pz;
    local_5c->z = pz;
    roof = (f32)(playerMoveRoofCheck(dComIfG_Bgsp(), playerMoveRoof(this)) - 125.0);
    if (roof < current.pos.y) {
        current.pos.y = roof;
    }

    local_5c->y = local_68->y;
    local_5c->x = gabi::fmadds(100.0f, dVar7, local_68->x);
    local_5c->z = gabi::fmadds(100.0f, dVar6, local_68->z);
    dBgS_LinChk_Set(mLinkLinChk, local_68, local_5c, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        setClimbShapeOffset();
        return;
    }
    f32 gx = gabi::fmadds(25.0f, dVar7, current.pos.x);
    f32 gy = local_68->y + 30.0f;
    f32 gz = gabi::fmadds(25.0f, dVar6, current.pos.z);
    local_5c->x = gx;
    local_5c->y = gy;
    playerMoveGroundCheck(this)->m_pos.y = gy;
    playerMoveGroundCheck(this)->m_pos.z = gz;
    playerMoveGroundCheck(this)->m_pos.x = gx;
    local_5c->z = gz;
    f64 gc = playerMoveGroundCross(dComIfG_Bgsp(), playerMoveGroundCheck(this));
    if (gc != -1000000000.0 && !(gc < (f64)(f32)(local_68->y - 30.0f))) {
        cM3dGPla* tri = playerMoveTriPla(playerMoveGroundPoly(this));
        if (tri != 0 && !(tri->mNormal.y < 0.5f)) { /* cBgW_CheckBGround (HD: NULL check) */
            current.pos.y = (f32)gc;
            current.pos.x = gabi::fnmsubs(3.0f, dVar7, local_5c->x);
            current.pos.z = gabi::fnmsubs(3.0f, dVar6, local_5c->z);
            procHangClimb_init(0.0f);
            return;
        }
    }
    procFall_init(1, 6.0f);
}

/* 02429D8C */
BOOL daPy_lk_c::procClimbUpStart() {
    if (setMoveBGCorrectClimb()) {
        return TRUE;
    }
    if (mProcVar6 == 0) {
        if ((playerDemoView(this).mAnmHeapUpper[2].mIdx == 0xFFFF)) {
            procLadderUpStart_init_sub();
        }
    } else {
        J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
        m34C2 = 5;
        if (frameCtrl.getRate() < 0.01f) {
            dComIfGp_setDoStatus(player_move::dActStts_LET_GO_e);
            if (mProcVar2 == 0) {
                setLadderFootSe();
                mProcVar2 = 1;
            }
            if ((mItemTrigger & 1)) {
                m34C2 = 0;
                procFall_init(1, 6.0f);
            } else {
                changeClimbMoveProc(1);
            }
        } else if (frameCtrl.checkPass(9.0f)) {
            setLadderFootSe();
        }
    }
    return TRUE;
}

/* 02429E8C */
BOOL daPy_lk_c::procClimbDownStart() {
    if (setMoveBGCorrectClimb()) {
        return TRUE;
    }
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    m34C2 = 5;
    if (frameCtrl.getRate() < 0.01f) {
        dComIfGp_setDoStatus(player_move::dActStts_LET_GO_e);
        if ((mItemTrigger & 1)) {
            m34C2 = 0;
            procFall_init(1, 6.0f);
        } else {
            changeClimbMoveProc(1);
        }
    } else if (frameCtrl.checkPass(26.0f) || frameCtrl.checkPass(36.0f) || frameCtrl.checkPass(43.0f)) {
        setLadderFootSe();
    }
    return TRUE;
}

/* 02429F84 */
BOOL daPy_lk_c::procClimbMoveUpDown() {
    if (setMoveBGCorrectClimb()) {
        return TRUE;
    }
    m34C2 = 5;
    dComIfGp_setDoStatus(player_move::dActStts_LET_GO_e);
    if ((mItemTrigger & 1)) {
        m34C2 = 0;
        procFall_init(1, 6.0f);
    } else {
        f32 rate = mFrameCtrlUnder[0].getRate();
        if (std::fabs(rate) < 0.01f) {
            if (mProcVar0 != 0) {
                mProcVar0 = 0;
                setLadderFootSe();
            } else {
                changeClimbMoveProc(mProcVar6);
            }
        } else {
            f32 spd = getLadderMoveAnmSpeed();
            if (rate < 0.0f) { /* HD: the rate read before the call */
                spd = -spd;
            }
            mFrameCtrlUnder[0].setRate(spd);
            checkBgClimbMove();
        }
    }
    return TRUE;
}

/* 0242A06C */
BOOL daPy_lk_c::procClimbMoveSide() {
    if (setMoveBGCorrectClimb()) {
        return TRUE;
    }
    m34C2 = 5;
    dComIfGp_setDoStatus(player_move::dActStts_LET_GO_e);
    if ((mItemTrigger & 1)) {
        m34C2 = 0;
        procFall_init(1, 6.0f);
    } else {
        f32 rate = mFrameCtrlUnder[0].getRate();
        if (std::fabs(rate) < 0.01f) {
            if (mProcVar0 != 0) {
                mProcVar0 = 0;
                setLadderFootSe();
            } else {
                changeClimbMoveProc(mProcVar6);
            }
        } else {
            f32 spd = getClimbMoveAnmSpeed();
            if (rate < 0.0f) { /* HD: the rate read before the call */
                spd = -spd;
            }
            mFrameCtrlUnder[0].setRate(spd);
            gabi::Local<cXyz> xz;
            gabi::Local<cXyz> d;
            cXyz_mi(&mRightHandPos, d, &mLeftHandPos);
            xz->x = d->x;
            xz->y = 0.0f;
            xz->z = d->z;
            f32 dVar5 = std_sqrtf(PSVECSquareMag(xz)); /* absXZ() */
            f32 fVar1 = std::fabs(dVar5 - m35A0) * 0.8f; /* HD: HIO folded */
            f32 fVar2 = fVar1 * cM_ssin(mProcVar2);
            current.pos.y = gabi::fmadds(fVar1, cM_scos(mProcVar2), current.pos.y);
            current.pos.x = gabi::fmadds(fVar2, cM_scos(shape_angle.y), current.pos.x);
            f32 z = gabi::fnmsubs(fVar2, cM_ssin(shape_angle.y), current.pos.z);
            m35A0 = dVar5;
            current.pos.z = z;
            checkBgClimbMove();
        }
    }
    return TRUE;
}

/* ---- d_a_player_whide.inc ---- */

/* 0242A218 */
void daPy_lk_c::setBlendWHideMoveAnime(f32 param_0) {
    /* HD: HIO folded (the two postures differ in the animation speed, the end ratio and the animations) */
    f32 fVar1, fVar6;
    int dVar5, dVar6;
    if (!(mNoResetFlg0 & player_move::daPyFlg0_UNK10000)) {
        fVar1 = 1.6f;
        dVar6 = player_move::ANM_WALL;
        fVar6 = 1.0f;
        dVar5 = mDirection == player_move::DIR_LEFT ? player_move::ANM_WALLWR : player_move::ANM_WALLWL;
    } else {
        fVar1 = 1.8f;
        dVar6 = player_move::ANM_WALLDW;
        fVar6 = 0.98f;
        dVar5 = mDirection == 2 ? player_move::ANM_WALLWRDW : player_move::ANM_WALLWLDW;
    }
    f32 fVar7 = mNormalSpeed / mMaxNormalSpeed;
    f32 fVar5 = 5.0f * param_0;
    const f32 fVar3 = 0.9f;
    f32 fVar4;
    if (fVar7 < 0.0f) {
        fVar4 = 1.0f - 0.0f / fVar3;
        m3598 = 1.0f;
    } else if (fVar7 < fVar3) {
        fVar4 = 1.0f - fVar7 / fVar3;
        m3598 = 1.0f;
    } else {
        fVar4 = 0.0f;
        m3598 = 1.0f - ((1.0f - fVar6) * (fVar7 - fVar3)) / (1.0f - fVar3); /* GHS folds (1.0f - fVar3) */
    }
    setMoveAnime(fVar4, fVar1, 0.0f, dVar5, dVar6, 5, fVar5);
    s16 end1 = mFrameCtrlUnder[1].mEnd;
    mFrameCtrlUnder[0].mEnd = 0x1C; /* HD: HIO folded */
    mFrameCtrlUnder[1].mRate = 0.0f;
    f32 frame = (f32)(s32)end1 - 0.001f;
    mFrameCtrlUnder[1].mFrame = frame;
    playerMoveRatio(this, 1)->mpAnmTransform.get()->mFrame = frame; /* mAnmRatioUnder[UNDER_MOVE1_e].getAnmTransform()->setFrame() */
}

/* 0242A478 */
void daPy_lk_c::getWHideBasePos(cXyz* param_0) {
    f32 h = (mNoResetFlg0 & player_move::daPyFlg0_UNK10000) ? 89.9f : 125.0f;
    f32 y = current.pos.y + h;
    param_0->x = current.pos.x;
    param_0->z = current.pos.z;
    param_0->y = y;
}

/* 0242A4CC */
void daPy_lk_c::getWHideNextPos(cXyz* param_0, cXyz* param_1) {
    s16 a = current.angle.y;
    f32 x = gabi::fmadds(25.0f, cM_ssin(a), param_0->x); /* HD: HIO field_0x54 folded */
    f32 z = gabi::fmadds(25.0f, cM_scos(a), param_0->z);
    f32 y = param_0->y;
    param_1->x = x;
    param_1->y = y;
    param_1->z = z;
}

/* 0242A514 */
BOOL daPy_lk_c::checkWHideBackWall(cXyz* param_0) {
    if (param_0 == nullptr) {
        return TRUE;
    }
    gabi::Local<cXyz> end;
    end->x = gabi::fnmsubs(25.0f, cM_ssin(shape_angle.y), param_0->x);
    end->y = param_0->y;
    end->z = gabi::fnmsubs(25.0f, cM_scos(shape_angle.y), param_0->z);
    dBgS_LinChk_Set(mLinkLinChk, param_0, end, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        return TRUE;
    }
    return FALSE;
}

/* 0242A5D0 */
BOOL daPy_lk_c::checkWHideFrontFloor(cXyz* param_0) {
    /* mGndChk.SetPos(&local_10): HD stores the position into the check directly */
    f32 x = gabi::fmadds(40.0f, cM_ssin(shape_angle.y), param_0->x);
    f32 z = gabi::fmadds(40.0f, cM_scos(shape_angle.y), param_0->z);
    playerMoveGroundCheck(this)->m_pos.y = param_0->y;
    playerMoveGroundCheck(this)->m_pos.x = x;
    playerMoveGroundCheck(this)->m_pos.z = z;
    f64 g = playerMoveGroundCross(dComIfG_Bgsp(), playerMoveGroundCheck(this));
    f32 y = current.pos.y;
    if ((mNoResetFlg0 & player_move::daPyFlg0_UNK80000000) && y > g) {
        g = y;
    }
    return std::fabs((f32)(g - (f64)y)) < 5.0f;
}

/* 0242A6B4 */
int daPy_lk_c::checkWHideModeChange(cXyz* param_0) {
    if (!(mNoResetFlg0 & player_move::daPyFlg0_UNK10000)) {
        if (!checkWHideFrontFloor(param_0)) {
            dComIfGp_setRStatus(player_move::dActStts_BLANK_e);
        } else if ((mItemButton & 0x40)) {
            return 1;
        }
    } else {
        gabi::Local<cXyz> local_18;
        local_18->x = param_0->x;
        local_18->y = current.pos.y + 125.0f;
        local_18->z = param_0->z;
        if (!(mItemButton & 0x40) && !(mAcch.m_flags & dBgS_Acch::ROOF_HIT) && checkWHideBackWall(local_18)) {
            return -1;
        }
    }
    if ((mModeFlg & player_move::ModeFlg_00000001) && mCurProc != player_move::daPyProc_WHIDE_PEEP_e) {
        setTextureAnime(0xF /* mAnmDataTable[ANM_WALL].mTexAnmIdx */, 0);
    }
    return 0;
}

/* 0242A7CC */
int daPy_lk_c::changeWHideEndProc(cXyz* param_0) {
    dComIfGp_setDoStatus(player_move::dActStts_SIDLE_e);
    dComIfGp_setRStatus(player_move::dActStts_CROUCH_e);
    if ((!(mItemButton & 1) && (!(mNoResetFlg0 & player_move::daPyFlg0_UNK10000) || !(mAcch.m_flags & dBgS_Acch::ROOF_HIT))) ||
        (playerDemoView(this).mAnmHeapUpper[2].mIdx != player_move::dRes_INDEX_LKANM_BCK_REST_e && (mItemTrigger & 2)) ||
        !checkWHideBackWall(param_0)) {
        mNormalSpeed = mMaxNormalSpeed * 0.7f;
        current.angle.y = shape_angle.y;
        return checkNextMode(0);
    }
    if ((mNoResetFlg0 & player_move::daPyFlg0_UNK10000) && (mAcch.m_flags & dBgS_Acch::ROOF_HIT)) {
        mModeFlg = mModeFlg | player_move::ModeFlg_04000000;
    } else {
        mModeFlg = mModeFlg & ~player_move::ModeFlg_04000000;
    }
    return FALSE;
}

/* 0242A8E4 */
BOOL daPy_lk_c::procWHideWait_init() {
    int anm;
    s16 end;
    if (!(mNoResetFlg0 & player_move::daPyFlg0_UNK10000)) {
        anm = player_move::ANM_WALL;
        end = 0xA;  /* HD: HIO folded */
    } else {
        anm = player_move::ANM_WALLDW;
        end = 7;
    }
    f32 start;
    if (mCurProc == player_move::daPyProc_WHIDE_READY_e) {
        start = 3.0f;
    } else {
        start = (f32)(s32)end - 0.001f;
    }
    commonProcInit(player_move::daPyProc_WHIDE_WAIT_e);
    mNormalSpeed = 0.0f;
    setSingleMoveAnime(anm, 1.3f, start, (s32)end, 5.0f);
    current.angle.y = shape_angle.y;
    dComIfGp_setPlayerStatus0(0, player_move::daPyStts0_UNK1_e);
    return TRUE;
}

/* 0242AA58 */
BOOL daPy_lk_c::procWHideReady() {
    if (changeWHideEndProc(nullptr)) {
        return TRUE;
    }
    if (mProcVar6 == 0) {
        if ((playerDemoView(this).mAnmHeapUpper[2].mIdx == 0xFFFF)) {
            setSingleMoveAnime(player_move::ANM_WALL, 0.0f, 3.0f, -1, 5.0f); /* HD: HIO folded */
            mProcVar6 = 1;
        }
        return TRUE;
    }
    cLib_addCalc(&current.pos.x, m370C.x, 0.25f, 10.0f, 4.0f);
    cLib_addCalc(&current.pos.z, m370C.z, 0.25f, 10.0f, 4.0f);
    if (!cLib_addCalcAngleS(&shape_angle.y, mProcVar2, 2, 0x2000, 0x800)) {
        l_maba_state.m_maba_flg = 0; /* daPy_matAnm_c::offMabaFlg(): m_maba_flg = 0 */
        l_maba_state.m_maba_timer = 1; /* m_maba_timer = 1 */
        procWHideWait_init();
    }
    return TRUE;
}

/* 0242ABA8 */
BOOL daPy_lk_c::procWHideMove_init() {
    commonProcInit(player_move::daPyProc_WHIDE_MOVE_e);
    if (mDirection == player_move::DIR_LEFT) {
        current.angle.y = shape_angle.y + 0x4000;
    } else {
        current.angle.y = shape_angle.y - 0x4000;
    }
    setBlendWHideMoveAnime(1.0f);
    dComIfGp_setPlayerStatus0(0, player_move::daPyStts0_UNK1_e);
    return TRUE;
}

/* 0242AC48 */
BOOL daPy_lk_c::procWHidePeep_init() {
    commonProcInit(player_move::daPyProc_WHIDE_PEEP_e);
    /* HD: HIO folded (rate 0.8/0.9, start 1.0, end 14, morf 5.0) */
    f32 rate;
    int anm;
    if (!(mNoResetFlg0 & player_move::daPyFlg0_UNK10000)) {
        rate = 0.8f;
        anm = mDirection == 2 ? player_move::ANM_WALLPR : player_move::ANM_WALLPL;
    } else {
        rate = 0.9f;
        anm = mDirection == 2 ? player_move::ANM_WALLPRDW : player_move::ANM_WALLPLDW;
    }
    setSingleMoveAnime(anm, rate, 1.0f, 0xE, 5.0f);
    dComIfGp_setPlayerStatus0(0, player_move::daPyStts0_UNK1_e);
    return TRUE;
}

/* 0242AD20 */
BOOL daPy_lk_c::procWHideWait() {
    gabi::Local<cXyz> cStack_48; /* sp+0x08 */
    gabi::Local<cXyz> cStack_3c; /* sp+0x14 */
    getWHideBasePos(cStack_3c);
    if (changeWHideEndProc(cStack_3c)) {
        return TRUE;
    }
    int mode = checkWHideModeChange(cStack_3c);
    if (mode == 1) {
        setSingleMoveAnime(player_move::ANM_WALLDW, 1.1f, 0.0f, 7, 5.0f); /* HD: HIO folded */
        mNoResetFlg0 = mNoResetFlg0 | player_move::daPyFlg0_UNK10000;
        mAcchCir[2].m_wall_h = 89.9f; /* mAcchCir[2].SetWallH() */
    } else if (mode == -1) {
        setSingleMoveAnime(player_move::ANM_WALLDW, -1.1f, 0.0f, 7, 5.0f);
        PlayerMoveAnimation* anm = playerMoveRatio(this, 0)->mpAnmTransform; /* mAnmRatioUnder[UNDER_MOVE0_e].getAnmTransform() */
        mFrameCtrlUnder[0].mFrame = 6.999f;
        anm->mFrame = 6.999f;
        mNoResetFlg0 = mNoResetFlg0 & ~player_move::daPyFlg0_UNK10000;
        mAcchCir[2].m_wall_h = 125.0f;
    }
    if (mStickDistance > 0.05f && mAcch.m_ground_h != -1000000000.0f) {
        cM3dGPla* tri = playerMoveTriPla(static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd));
        if (tri != 0 && !(tri->mNormal.y < 0.5f)) { /* cBgW_CheckBGround (HD: NULL check) */
            int direction = getDirectionFromCurrentAngle();
            if (direction == player_move::DIR_RIGHT || direction == player_move::DIR_LEFT) {
                if (direction == 3) {
                    mDirection = 3;
                    current.angle.y = shape_angle.y - 0x4000;
                } else {
                    mDirection = 2;
                    current.angle.y = shape_angle.y + 0x4000;
                }
                getWHideNextPos(cStack_3c, cStack_48);
                if (checkWHideBackWall(cStack_48)) {
                    if (!(mNoResetFlg0 & player_move::daPyFlg0_UNK10000) || checkWHideFrontFloor(cStack_48)) {
                        return procWHideMove_init();
                    }
                } else {
                    gabi::Local<cXyz> d;  /* sp+0x2C */
                    gabi::Local<cXyz> d4; /* sp+0x20 */
                    cXyz_mi(cStack_48, d, cStack_3c);
                    cXyz_ml(d, d4, 0.25f);
                    playerMoveAddVector(cStack_48, d4, cStack_48);
                    dBgS_LinChk_Set(mLinkLinChk, cStack_3c, cStack_48, this);
                    if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
                        return procWHidePeep_init();
                    }
                }
            }
        }
    }
    current.angle.y = shape_angle.y;
    return TRUE;
}

/* 0242AFB8 */
BOOL daPy_lk_c::procWHideMove() {
    if (changeSlideProc()) {
        return TRUE;
    }
    gabi::Local<cXyz> acStack_24; /* sp+0x08 */
    gabi::Local<cXyz> cStack_30;  /* sp+0x14 */
    gabi::Local<cXyz> cStack_3c;  /* sp+0x20 */
    gabi::Local<cXyz> cStack_48;  /* sp+0x2C */
    getWHideBasePos(acStack_24);
    s16 uVar1 = current.angle.y;
    if (changeWHideEndProc(acStack_24)) {
        if (!(mItemButton & 1) && mStickDistance > 0.05f) {
            current.pos.x = gabi::fmadds(50.0f, cM_ssin(uVar1), current.pos.x);
            current.pos.z = gabi::fmadds(50.0f, cM_scos(uVar1), current.pos.z);
        }
        return TRUE;
    }
    cM3dGPla* tri = playerMoveTriPla(playerDemoLinePoly(this));
    if (tri == 0) { /* HD: NULL check */
        return FALSE;
    }
    s16 sVar8 = cM_atan2s(tri->mNormal.x, tri->mNormal.z);
    f32 fVar2 = 0.0f;
    if (checkWHideModeChange(acStack_24) == 0 && mStickDistance > 0.05f && mAcch.m_ground_h != -1000000000.0f) {
        cM3dGPla* gnd = playerMoveTriPla(static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd));
        if (gnd != 0 && !(gnd->mNormal.y < 0.5f)) { /* cBgW_CheckBGround (HD: NULL check) */
            int direction = getDirectionFromCurrentAngle();
            if (direction == player_move::DIR_BACKWARD) {
                f32 spd = mNormalSpeed * 0.5f;
                current.angle.y = current.angle.y - 0x8000;
                mNormalSpeed = spd;
                mDirection = mDirection == player_move::DIR_LEFT ? 3 : 2;
                setBlendWHideMoveAnime(2.0f);
                fVar2 = 3.5f * mStickDistance; /* HD: HIO folded */
            } else if (direction == player_move::DIR_FORWARD) {
                fVar2 = 3.5f * mStickDistance;
            }
        }
    }
    setNormalSpeedF(fVar2, 0.5f, 2.0f, 0.25f);
    getWHideNextPos(acStack_24, cStack_30);
    if ((mNoResetFlg0 & player_move::daPyFlg0_UNK10000) && !checkWHideFrontFloor(cStack_30)) {
        mNormalSpeed = 0.0f;
        return procWHideWait_init();
    }
    u32 pla = getWHideModePolygon(acStack_24.get(), cStack_30.get(), cStack_3c.get(), (u32)mDirection);
    if (pla != 0) {
        return procWHideReady_init(pla, cStack_3c.get());
    }
    if (!(std::fabs(mNormalSpeed) > 0.001f)) {
        return procWHideWait_init();
    }
    bool stop = mAcch.m_ground_h == -1000000000.0f;
    if (!stop) {
        cM3dGPla* gnd = playerMoveTriPla(static_cast<cBgS_PolyInfo*>(&mAcch.m_gnd));
        stop = (gnd != 0 && gnd->mNormal.y < 0.5f) || !checkWHideBackWall(cStack_30); /* HD: NULL check */
    }
    if (stop) {
        mNormalSpeed = 0.0f;
        return procWHideWait_init();
    }
    if (sVar8 != shape_angle.y &&
        getWHideModePolygon(nullptr, nullptr, cStack_48.get(), (u32)mDirection) != 0) {
        s16 a = (s16)(current.angle.y - shape_angle.y) > 0 ? (s16)(sVar8 + 0x4000) : (s16)(sVar8 - 0x4000);
        current.angle.y = a;
        shape_angle.y = sVar8;
        current.pos.x = gabi::fmadds(8.0f, cM_ssin(a), current.pos.x);
        current.pos.z = gabi::fmadds(8.0f, cM_scos(a), current.pos.z);
    }
    /* 0.8f * m_HIO->mWall.m.field_0x50 folded to 6.8f */
    current.pos.x = gabi::fnmsubs(6.8f, cM_ssin(shape_angle.y), current.pos.x);
    current.pos.z = gabi::fnmsubs(6.8f, cM_scos(shape_angle.y), current.pos.z);
    setBlendWHideMoveAnime(-1.0f);
    return TRUE;
}

/* 0242B648 */
BOOL daPy_lk_c::procWHidePeep() {
    gabi::Local<cXyz> cStack_38;
    getWHideBasePos(cStack_38);
    if (changeWHideEndProc(cStack_38)) {
        return TRUE;
    }
    J3DFrameCtrl& fc = mFrameCtrlUnder[0];
    f32 dVar3 = fc.getRate();
    if (std::fabs(dVar3) < 0.01f) {
        if (fc.getFrame() < (f32)(s32)(fc.mStart + 1)) {
            return procWHideWait_init();
        }
    }
    if (std::fabs(dVar3) < 0.01f) {
        dComIfGp_setPlayerStatus0(0, mDirection == player_move::DIR_LEFT ? player_move::daPyStts0_UNK40_e : player_move::daPyStts0_UNK20_e);
        dVar3 = (mNoResetFlg0 & player_move::daPyFlg0_UNK10000) ? 0.9f : 0.8f; /* HD: HIO folded */
    }
    if (checkWHideModeChange(cStack_38) != 0) {
        if (dVar3 > 0.0f) {
            fc.setRate(-dVar3);
        }
    } else if (mStickDistance > 0.05f) {
        int direction = getDirectionFromCurrentAngle();
        if ((direction != 0 && dVar3 > 0.0f) || (direction == 0 && dVar3 < 0.0f)) {
            fc.setRate(-dVar3);
        }
    } else if (dVar3 > 0.0f) {
        fc.setRate(-dVar3);
    }
    return TRUE;
}

/* ---- d_a_player_crawl.inc ---- */

/* 0242B800 */
f32 daPy_lk_c::getCrawlMoveSpeed() {
    f32 fVar1 = mFrameCtrlUnder[0].getFrame();
    if (!(fVar1 < 17.0f)) {
        fVar1 = fVar1 - 17.0f;
    }
    /* cM_fsin((M_PI / 17) * fVar1): cM_rad2s and the sine table */
    s16 a = cM_rad2s(fVar1 * l_crawl_frame_radians);
    f32 r = 3.0f * mFrameCtrlUnder[0].getRate(); /* HD: HIO field_0x3C folded */
    return r * cM_ssin(a);
}

/* 0242B8BC */
void daPy_lk_c::setCrawlMoveDirectionArrow() {
    u8 direction = 0;
    s32 camIdx = mCameraInfoIdx;
    BOOL bVar = cLib_distanceAngleS(playerDemoCamera(camIdx)->mAngleY, shape_angle.y) > 0x4000;
    s32 v = mProcVar6;
    if (v & 4) {
        direction |= !bVar ? 1 : 4;
    }
    if (v & 8) {
        direction |= !bVar ? 4 : 1;
    }
    if (v & 1) {
        if (shape_angle.y == current.angle.y) {
            direction |= !bVar ? 8 : 2;
        } else {
            direction |= !bVar ? 2 : 8;
        }
    }
    if (shape_angle.y == current.angle.y) {
        direction |= !bVar ? 2 : 8;
    } else {
        direction |= !bVar ? 8 : 2;
    }
    playerMovePlay()->mAdvanceDirection = direction; /* dComIfGp_setAdvanceDirection */
}

/* 0242B9E0 */
BOOL daPy_lk_c::checkCrawlSideWall(cXyz* param_1, cXyz* param_2, cXyz* param_3, cXyz* param_4, s16* param_5, s16* param_6) {
    dBgS_LinChk_Set(mLinkLinChk, param_1, param_2, this);
    if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        return FALSE;
    }
    param_3->copy(playerMoveLineCross(this));
    cM3dGPla* pla = playerMoveTriPla(playerDemoLinePoly(this));
    if (pla == 0) { /* HD: NULL check */
        return FALSE;
    }
    playerMoveAngle(param_5) = cM_atan2s(pla->mNormal.x, pla->mNormal.z);
    gabi::Local<cXyz> t14;      /* sp+0x14 */
    gabi::Local<cXyz> t20;      /* sp+0x20 */
    gabi::Local<cXyz> local_90; /* sp+0x2C */
    gabi::Local<cXyz> local_78; /* sp+0x44 */
    playerMoveScaleVector(&pla->mNormal, t20, 75.0f);
    playerDemoAddVector(param_3, t14, t20);
    local_90->copy(*t14.get());
    playerDemoAddVector(local_90, t14, param_3);
    playerMoveScaleVector(t14, t20, 0.5f);
    local_78->copy(*t20.get());
    dBgS_LinChk_Set(mLinkLinChk, local_78, local_90, this);
    if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        return FALSE;
    }
    param_4->copy(playerMoveLineCross(this));
    pla = playerMoveTriPla(playerDemoLinePoly(this));
    if (pla == 0) {
        return FALSE;
    }
    playerMoveAngle(param_6) = cM_atan2s(pla->mNormal.x, pla->mNormal.z);
    gabi::Local<cXyz> xz;       /* sp+0x08 */
    gabi::Local<cXyz> local_9c; /* sp+0x38 */
    cXyz_mi(param_3, local_9c, param_4);
    xz->y = 0.0f;
    xz->x = local_9c->x;
    xz->z = local_9c->z;
    f32 d2 = PSVECSquareMag(xz); /* abs2XZ() */
    if (cLib_distanceAngleS(playerMoveAngle(param_5), playerMoveAngle(param_6)) > 0x7F00 &&
        d2 < 5625.0f && d2 > 3600.0f) {
        return TRUE;
    }
    return FALSE;
}

/* 0242BC1C */
BOOL daPy_lk_c::procCrawlAutoMove_init(int param_0, cXyz* param_1) {
    u32 subject = dComIfGp_checkPlayerStatus0(0, player_move::daPyStts0_SUBJECT_e);
    commonProcInit(player_move::daPyProc_CRAWL_AUTO_MOVE_e);
    m370C.copy(*param_1);
    mProcVar3 = 300;
    m35E4 = 1.0f;
    mProcVar6 = param_0;
    mProcVar0 = 20;
    dComIfGp_setPlayerStatus0(0, player_move::daPyStts0_CRAWL_e);
    setCrawlMoveDirectionArrow();
    setResetFlg0(resetFlg0() | player_move::daPyRFlg0_CRAWL_AUTO_MOVE);
    mNormalSpeed = 0.0f;
    for (int i = 0; i < 3; i++) {
        /* mAcchCir[i].SetWallR(): HD reads the radius from a global */
        playerMoveSetWallR(&mAcchCir[i], l_crawl_wall_radius);
    }
    if (subject) {
        dComIfGp_setPlayerStatus0(0, player_move::daPyStts0_SUBJECT_e);
    }
    return TRUE;
}

/* 0242BD24 */
BOOL daPy_lk_c::changeCrawlAutoMoveProc(cXyz* param_1) {
    f32 temp_f31 = cM_ssin(current.angle.y);
    f32 temp_f30 = cM_scos(current.angle.y);
    f32 temp_f29 = cM_ssin(shape_angle.y);
    f32 temp_f28 = cM_scos(shape_angle.y);
    f32 s75 = 75.0f * temp_f31;
    f32 c75 = 75.0f * temp_f30;
    int var_r29 = 0;
    gabi::Local<cXyz> sp4C;   /* sp+0x08 */
    gabi::Local<u32> angles;  /* sp+0x14: spA (+0), sp8 (+2) */
    gabi::Local<cXyz> sp58;   /* sp+0x18 */
    gabi::Local<cXyz> sp70;   /* sp+0x24 */
    gabi::Local<cXyz> sp40;   /* sp+0x30 */
    gabi::Local<cXyz> sp64;   /* sp+0x3C */
    gabi::Local<cXyz> sp34;   /* sp+0x48 */
    gabi::Local<cXyz> sp28;   /* sp+0x54 */
    gabi::Local<f32> spC;     /* sp+0x60 */
    gabi::Local<cXyz> tmp64;  /* sp+0x64 */
    gabi::Local<cXyz> tmp70;  /* sp+0x70 */
    s16* spA = gabi::at<s16>(angles.a);
    s16* sp8 = gabi::at<s16>(angles.a + 2);
    sp70->y = param_1->y;
    sp70->x = param_1->x + s75;
    sp70->z = param_1->z + c75;
    dBgS_LinChk_Set(mLinkLinChk, param_1, sp70, this);
    if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        sp4C->x = gabi::fmadds(50.0f, temp_f30, sp70->x);
        sp4C->y = sp70->y;
        sp4C->z = gabi::fnmsubs(50.0f, temp_f31, sp70->z);
        if (!checkCrawlSideWall(sp70, sp4C, sp34, sp28, spA, sp8)) {
            return FALSE;
        }
        var_r29 = 1;
    }
    sp4C->x = gabi::fmadds(50.0f, temp_f28, param_1->x);
    sp4C->y = param_1->y;
    sp4C->z = gabi::fnmsubs(50.0f, temp_f29, param_1->z);
    playerMoveScaleVector(param_1, tmp70, 2.0f);
    cXyz_mi(tmp70, tmp64, sp4C);
    sp40->copy(*tmp64.get()); /* sp40 = (*param_1 * 2.0f) - sp4C */
    dBgS_LinChk_Set(mLinkLinChk, param_1, sp4C, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
        var_r29 |= 8;
        sp58->x = gabi::fmadds(75.0f, temp_f29, sp40->x);
        sp58->y = sp40->y;
        sp58->z = gabi::fmadds(75.0f, temp_f28, sp40->z);
        if (!checkCrawlSideWall(sp40, sp58, sp34, sp28, spA, sp8)) {
            return FALSE;
        }
    } else {
        var_r29 |= 4;
        sp58->x = gabi::fnmsubs(75.0f, temp_f29, sp4C->x);
        sp58->y = sp4C->y;
        sp58->z = gabi::fnmsubs(75.0f, temp_f28, sp4C->z);
        if (!checkCrawlSideWall(sp4C, sp58, sp34, sp28, spA, sp8)) {
            return FALSE;
        }
        dBgS_LinChk_Set(mLinkLinChk, param_1, sp40, this);
        if (!cBgS_LineCross(dComIfG_Bgsp(), mLinkLinChk)) {
            var_r29 |= 8;
        }
    }
    f32 x1 = param_1->x - s75;
    f32 y1 = param_1->z - c75;
    f32 mx = (sp34->x + sp28->x) * 0.5f;
    f32 mz = (sp34->z + sp28->z) * 0.5f;
    if (cM3d_Len2dSqPntAndSegLine(mx, mz, (f32)sp70->x, (f32)sp70->z, x1, y1,
                         &sp64->x, &sp64->z, spC.get())) {
        sp64->y = current.pos.y;
        return procCrawlAutoMove_init(var_r29, sp64);
    }
    return FALSE;
}

/* 0242C10C */
void daPy_lk_c::crawlBgCheck(cXyz* param_0, cXyz* param_1) {
    gabi::Local<cXyz> cStack_50; /* sp+0x08 */
    gabi::Local<cXyz> cStack_44; /* sp+0x14 */
    gabi::Local<cXyz> cStack_38; /* sp+0x20 */
        PSMTXMultVec(playerMoveBaseMatrix(mpCLModel), &l_crawl_top_offset, cStack_50);
    int iVar1 = getCrawlMoveVec(cStack_50.get(), param_0, cStack_44.get());
    PSMTXMultVec(playerMoveBaseMatrix(mpCLModel), &l_crawl_top_up_offset, cStack_50);
    int iVar2 = getCrawlMoveVec(cStack_50.get(), param_1, cStack_38.get());
    if (iVar1 != 0) {
        if (iVar2 != 0) {
            f64 a2 = playerMoveSquareMagnitudeRegister(cStack_44.get());
            f64 b2 = playerMoveSquareMagnitudeRegister(cStack_38.get());
            f64 b = playerMoveSqrtRegister(b2);
            if (!(a2 > b)) { /* GameCube compares abs2() with abs() */
                playerMoveSubtractVector(&current.pos, cStack_38, &current.pos);
                return;
            }
        }
        playerMoveSubtractVector(&current.pos, cStack_44, &current.pos);
    } else if (iVar2 != 0) {
        playerMoveSubtractVector(&current.pos, cStack_38, &current.pos);
    }
}

/* 0242C234 */
void daPy_lk_c::setDoStatusCrawl() {
    dComIfGp_setRStatus(player_move::dActStts_CROUCH_e);
    if (dComIfGp_getCameraAttentionStatus(mCameraInfoIdx) & player_move::dCamAttnStts_00000080_e) {
        dComIfGp_setAStatus(7); /* dComIfGp_setAStatus(dActStts_RETURN_e) */
        if (checkSubjectEnd(1) || mWaterY > current.pos.y) {
            dComIfGp_clearPlayerStatus0(0, player_move::daPyStts0_SUBJECT_e);
        } else {
            dComIfGp_setPlayerStatus0(0, player_move::daPyStts0_SUBJECT_e);
        }
    } else if (playerMoveCameraChangeModeOK(playerDemoGetCameraBody(), 4)) {
        if (!(mWaterY > current.pos.y)) {
            setResetFlg0(resetFlg0() | player_move::daPyRFlg0_SUBJECT_ACCEPT);
            if ((dComIfGp_getCameraAttentionStatus(mCameraInfoIdx) & player_move::dCamAttnStts_00001000_e) &&
                playerMovePlay()->mEventMode == 0 /* !dComIfGp_event_runCheck() */) {
                setSubjectMode();
            }
        }
    }
    if ((dComIfGp_getCameraAttentionStatus(mCameraInfoIdx) & player_move::dCamAttnStts_00000080_e) && (mCurProc != player_move::daPyProc_CRAWL_MOVE_e || mProcVar6 == 0)) {
        setBodyAngleToCamera();
        if (mBodyAngle.x > 0) {
            mBodyAngle.x = 0;
        }
    } else {
        mBodyAngle.x = 0;
    }
}

/* 0242C3D0 */
BOOL daPy_lk_c::checkNotCrawlStand(cXyz* param_0) {
    playerMoveRoof(this)->m_pos.copy(*param_0); /* mRoofChk.SetPos(*param_0) */
    f64 r = playerMoveRoofCheck(dComIfG_Bgsp(), playerMoveRoof(this));
    return !((f32)(r - (f64)(f32)current.pos.y) > 125.0f);
}

/* 0242C440 */
BOOL daPy_lk_c::checkNotCrawlStand(cXyz* param_0, cXyz* param_1) {
    gabi::Local<cXyz> tmp;      /* sp+0x08 */
    gabi::Local<cXyz> local_2c; /* sp+0x14 */
    playerDemoAddVector(param_0, tmp, param_1);
    local_2c->copy(*tmp.get());
    if (checkNotCrawlStand(local_2c.get())) {
        return TRUE;
    }
    cXyz_mi(param_0, tmp, param_1);
    local_2c->copy(*tmp.get());
    if (checkNotCrawlStand(local_2c.get())) {
        return TRUE;
    }
    return FALSE;
}

/* 0242C518 */
BOOL daPy_lk_c::procCrawlEnd_init(int param_1, s16 param_2, s16 param_3) {
    f32 dVar4 = mFrameCtrlUnder[0].getFrame();
    commonProcInit(player_move::daPyProc_CRAWL_END_e);
    m34C2 = 1;
    current.angle.y = shape_angle.y;
    /* HD: HIO folded (rate -1.0, start 0.0, end 8, morf 2.0) */
    setSingleMoveAnime(player_move::ANM_LIE, -1.0f, 0.0f, 8, param_1 != 0 ? 2.0f : -1.0f);
    if (param_1 == 0) {
        if (0.0f > dVar4) {
            dVar4 = 0.0f;
        } else if (!(8.0f > dVar4)) {
            dVar4 = 7.999f;
        }
        mFrameCtrlUnder[0].setFrame(dVar4);
        playerMoveRatio(this, 0)->mpAnmTransform.get()->mFrame = dVar4; /* mAnmRatioUnder[UNDER_MOVE0_e].getAnmTransform()->setFrame() */
    }
    /* getTransform(0, &info) inlined: the translation of joint 0 (HD: the calc object of m_pbCalc[0], marked dirty) */
    PlayerMoveCalcObject* obj = gabi::at<PlayerMoveCalcPack>(m_pbCalc[0])->mpCalc;
    obj->mFlags = obj->mFlags | 8;
    PlayerMoveJointTransform* tr = obj->mpTransform;
    m3700.x = tr->mTranslate.x;
    m3700.y = tr->mTranslate.y;
    mNormalSpeed = 0.0f;
    m3700.z = tr->mTranslate.z;
    shape_angle.z = param_3;
    m35A0 = 0.2f; /* 1.0f / (8 - 3) folded */
    shape_angle.x = param_2;
    return TRUE;
}

/* 0242C688 */
BOOL daPy_lk_c::procCrawlStart() {
    setDoStatusCrawl();
    f32 r = gabi::fnmsubs(7.0f - mFrameCtrlUnder[0].getFrame(), m35A0, 1.0f); /* HD: HIO field_0x28 (7.0) folded */
    m35E4 = r;
    const cXyz& offset = l_crawl_front_offset;
    gabi::Local<cXyz> local_20; /* sp+0x08 */
    gabi::Local<cXyz> cStack_38; /* sp+0x14 */
    gabi::Local<cXyz> afStack_2c; /* sp+0x20 */
    local_20->x = offset.x;
    local_20->y = offset.y;
    local_20->z = offset.z * r;
        PSMTXMultVec(playerMoveBaseMatrix(mpCLModel), local_20, afStack_2c);
    local_20->y = 50.0f;
    PSMTXMultVec(playerMoveBaseMatrix(mpCLModel), local_20, cStack_38);
    crawlBgCheck(cStack_38, cStack_38);
    if (!(mItemButton & 0x40) || mWaterY > current.pos.y + 15.0f /* checkCrawlWaterIn() */) {
        procCrawlEnd_init(0, shape_angle.x, shape_angle.z);
    } else if (mStickDistance > 0.05f &&
               (mFrameCtrlUnder[0].getRate() < 0.01f || mFrameCtrlUnder[0].getFrame() > 7.0f)) {
        playerMoveCrawlInitRegister(this, (s32)shape_angle.x, (s32)shape_angle.z);
    } else {
        m34C2 = 1;
    }
    return TRUE;
}

/* 0242C80C */
BOOL daPy_lk_c::procCrawlMove() {
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    gabi::Local<u32> angles;  /* sp+0x08: sp0A (+0), sp08 (+2) */
    gabi::Local<cXyz> spF4;   /* sp+0x0C */
    gabi::Local<cXyz> spA0;   /* sp+0x18 */
    gabi::Local<cXyz> spAC;   /* sp+0x24 */
    gabi::Local<cXyz> sp124;  /* sp+0x30 */
    gabi::Local<cXyz> sp10C;  /* sp+0x3C */
    gabi::Local<cXyz> spE8;   /* sp+0x48 */
    gabi::Local<cXyz> spB8;   /* sp+0x54 */
    gabi::Local<cXyz> sp100;  /* sp+0x60 */
    gabi::Local<cXyz> sp88;   /* sp+0x6C */
    gabi::Local<cXyz> sp94;   /* sp+0x78 */
    gabi::Local<cXyz> sp118;  /* sp+0x84 */
    gabi::Local<cXyz> spD0;   /* sp+0x90 */
    gabi::Local<cXyz> spC4;   /* sp+0x9C */
    gabi::Local<cXyz> t1;     /* temporaries */
    gabi::Local<cXyz> t2;
    gabi::Local<cXyz> spDC;   /* sp+0xC0 */
    s16* sp0A = gabi::at<s16>(angles.a);
    s16* sp08 = gabi::at<s16>(angles.a + 2);
    setDoStatusCrawl();
    PSMTXMultVec(playerMoveBaseMatrix(mpCLModel), &l_crawl_stand_up_offset, sp118);
    PSMTXMultVec(playerMoveBaseMatrix(mpCLModel), &l_crawl_front_up_offset, sp124);
    PSMTXMultVec(playerMoveBaseMatrix(mpCLModel), &l_crawl_back_up_offset, sp10C);
    PSMTXMultVecSR(playerMoveBaseMatrix(mpCLModel), &l_crawl_min_side_offset, sp100);
    int iVar6 = checkNotCrawlStand(sp118.get());
    int iVar7 = checkNotCrawlStand(sp124.get());
    int iVar8 = checkNotCrawlStand(sp10C.get());
    BOOL bVar5;
    bool end;
    if ((iVar6 | iVar7 | iVar8) != 0 || checkNotCrawlStand(sp124, sp100) || checkNotCrawlStand(sp10C, sp100) ||
        checkNotCrawlStand(sp118, sp100)) {
        bVar5 = FALSE;
        mModeFlg = mModeFlg | player_move::ModeFlg_04000000;
        end = getSlidePolygon() != 0;
    } else {
        bVar5 = TRUE;
        mModeFlg = mModeFlg & ~player_move::ModeFlg_04000000;
        end = !(mItemButton & 0x40) || getSlidePolygon() != 0;
    }
    if (end || mWaterY > current.pos.y + 15.0f /* checkCrawlWaterIn() */) {
        procCrawlEnd_init(1, shape_angle.x, shape_angle.z);
        return TRUE;
    }
    BOOL bVar4 = FALSE;
    f32 dVar11 = getCrawlMoveAnmSpeed();
    f32 rate = frameCtrl.getRate();
    bool pass;
    if (rate > 0.0f) {
        frameCtrl.setRate(dVar11);
        pass = false;
    } else if (rate < 0.0f) {
        frameCtrl.setRate(-dVar11);
        pass = false;
    } else {
        bVar4 = TRUE;
        pass = true;
    }
    if (pass || frameCtrl.checkPass(0.0f) || frameCtrl.checkPass(17.0f)) {
        if (mStickDistance > 0.05f) {
            if (getDirectionFromShapeAngle() != 1) {
                frameCtrl.setRate(dVar11);
                frameCtrl.mLoop = 0;
            } else {
                frameCtrl.mLoop = frameCtrl.mEnd;
                frameCtrl.setRate(-dVar11);
            }
            initSeAnime();
        } else if (!bVar4) {
            f32 f = frameCtrl.checkPass(0.0f) ? 0.0f : 17.0f;
            frameCtrl.setRate(0.0f);
            frameCtrl.setFrame(f);
            playerMoveRatio(this, 0)->mpAnmTransform.get()->mFrame = f; /* mAnmRatioUnder[UNDER_MOVE0_e].getAnmTransform()->setFrame() */
        }
    }
    int iVar9 = mProcVar6;
    mProcVar6 = 0;
    PSMTXMultVec(playerMoveBaseMatrix(mpCLModel), &l_crawl_side_offset, spE8);
    PSMTXMultVec(playerMoveBaseMatrix(mpCLModel), &l_crawl_top_up_offset, spF4);
    if (!(frameCtrl.getRate() < 0.0f)) { /* HD: as the code branches */
        cXyz_mi(sp124, t1, spF4);
    } else {
        cXyz_mi(sp10C, t1, spF4);
    }
    playerMoveScaleVector(t1, t2, 0.5f);
    spB8->copy(*t2.get());
    playerMoveAddVector(spE8, spB8, spE8);
    playerMoveAddVector(spF4, spB8, spF4);
    playerMoveScaleVector(spF4, t1, 2.0f);
    cXyz_mi(t1, t2, spE8);
    spDC->copy(*t2.get());
    if (!bVar5 && (checkCrawlSideWall(spF4, spE8, spD0, spC4, sp0A, sp08) ||
                   checkCrawlSideWall(spF4, spDC, spC4, spD0, sp08, sp0A))) {
        mProcVar6 = 1;
        setResetFlg0(resetFlg0() | player_move::daPyRFlg0_CRAWL_AUTO_MOVE);
        playerDemoAddVector(spD0, t1, spC4);
        playerMoveScaleVector(t1, t2, 0.5f);
        gabi::Local<cXyz> t3;
        cXyz_mi(t2, t3, spB8);
        s16 a = playerMoveAngle(sp0A);
        m370C.copy(*t3.get());
        mProcVar2 = a + 0x4000;
    }
    if (mProcVar6 != 0) {
        if (frameCtrl.getRate() != 0.0f) {
            cLib_addCalcAngleS(&shape_angle.y, mProcVar2, 5, 0x1000, 0x800);
            current.angle.y = shape_angle.y;
        }
        if (std::fabs(current.pos.x - m370C.x) > 1.0f) {
            cLib_addCalc(&current.pos.x, m370C.x, 0.5f, 10.0f, 1.0f);
        }
        if (std::fabs(current.pos.z - m370C.z) > 1.0f) {
            cLib_addCalc(&current.pos.z, m370C.z, 0.5f, 10.0f, 1.0f);
        }
    } else {
        if (iVar9 != 0 && iVar6 != 0 && iVar7 != 0 && iVar8 != 0 && changeCrawlAutoMoveProc(spF4)) {
            return TRUE;
        }
        if (mStickDistance > 0.05f && frameCtrl.getRate() > 0.0f) {
            s16 r24 = shape_angle.y;
            if (dComIfGp_checkPlayerStatus0(0, player_move::daPyStts0_SUBJECT_e)) {
                /* HD: in first-person view the crawl turns only towards a side direction, 45 degrees short */
                int dir = getDirectionFromShapeAngle();
                if (dir == 2 || dir == 3) {
                    s16 target = m34E8;
                    s16 off = (s16)(target - shape_angle.y) > 0 ? 0x2000 : -0x2000;
                    cLib_addCalcAngleS(&shape_angle.y, (s16)(target - off), 0x20, 0x1B58, 0x1F4);
                }
            } else {
                cLib_addCalcAngleS(&shape_angle.y, m34E8, 0x20, 0x1B58, 0x1F4); /* HD: HIO folded */
            }
            s16 sy = shape_angle.y;
            if (sy != r24) {
                if ((s16)(sy - r24) > 0) {
                    PSMTXMultVec(playerMoveBaseMatrix(mpCLModel), &l_crawl_lside_front_offset, spAC);
                    PSMTXMultVec(playerMoveBaseMatrix(mpCLModel), &l_crawl_lside_offset, sp94);
                } else {
                    PSMTXMultVec(playerMoveBaseMatrix(mpCLModel), &l_crawl_rside_front_offset, spAC);
                    PSMTXMultVec(playerMoveBaseMatrix(mpCLModel), &l_crawl_rside_offset, sp94);
                }
                /* mGndChk.SetPos(&spAC): HD stores the position into the check directly */
                playerMoveGroundCheck(this)->m_pos.y = spAC->y;
                playerMoveGroundCheck(this)->m_pos.x = spAC->x;
                f32 dx = spAC->x - sp94->x;
                playerMoveGroundCheck(this)->m_pos.z = spAC->z;
                f64 gc = playerMoveGroundCross(dComIfG_Bgsp(), playerMoveGroundCheck(this));
                f32 dz = spAC->z - sp94->z;
                spA0->x = dx;
                f32 dy = (f32)(gc - (f64)(f32)sp94->y);
                spA0->y = 0.0f; /* HD: absXZ() built in place */
                spA0->z = dz;
                f32 h = std_sqrtf(PSVECSquareMag(spA0));
                if (cLib_distanceAngleS(cM_atan2s(-dy, h), shape_angle.x) > 0x800) {
                    shape_angle.y = r24;
                    current.angle.y = r24;
                } else {
                    current.angle.y = shape_angle.y;
                }
            } else {
                current.angle.y = sy;
            }
        }
    }
    f32 spd = getCrawlMoveSpeed();
    if (spd < 0.0f) {
        mNormalSpeed = -spd;
        current.angle.y = shape_angle.y + 0x8000;
        PSMTXMultVec(playerMoveBaseMatrix(mpCLModel), &l_crawl_back_offset, sp88);
        crawlBgCheck(sp88, sp10C);
    } else {
        mNormalSpeed = spd;
        PSMTXMultVec(playerMoveBaseMatrix(mpCLModel), &l_crawl_front_offset, sp88);
        crawlBgCheck(sp88, sp124);
    }
    return TRUE;
}

/* 0242D1A4 */
BOOL daPy_lk_c::procCrawlAutoMove() {
    dComIfGp_setRStatus(player_move::dActStts_CROUCH_e);
    J3DFrameCtrl& frameCtrl = mFrameCtrlUnder[0];
    s16 var0 = mProcVar0;
    s16 sVar4 = shape_angle.y;
    s16 sVar5 = current.angle.y;
    setResetFlg0(resetFlg0() | player_move::daPyRFlg0_CRAWL_AUTO_MOVE);
    if (var0 > 0) {
        if (frameCtrl.checkPass(0.0f) || frameCtrl.checkPass(17.0f)) {
            f32 f = frameCtrl.checkPass(0.0f) ? 0.0f : 17.0f;
            frameCtrl.setRate(0.0f);
            frameCtrl.setFrame(f);
            playerMoveRatio(this, 0)->mpAnmTransform.get()->mFrame = f; /* mAnmRatioUnder[UNDER_MOVE0_e].getAnmTransform()->setFrame() */
            mNormalSpeed = 0.0f;
        } else if (std::fabs(frameCtrl.getRate()) < 0.01f) {
            mNormalSpeed = 0.0f;
            mProcVar0 = mProcVar0 - 1;
        }
        setCrawlMoveDirectionArrow();
        setDoStatusCrawl();
        shape_angle.y = sVar4;
        current.angle.y = sVar5;
    } else if (var0 == 0) {
        setCrawlMoveDirectionArrow();
        if (mStickDistance > 0.05f) {
            int direction = getDirectionFromShapeAngle();
            f32 fVar2 = cM_ssin(shape_angle.y);
            f32 fVar3 = cM_scos(shape_angle.y);
            s32 v6 = mProcVar6;
            if (direction == player_move::DIR_LEFT && (v6 & 4)) {
                m370C.x = gabi::fmadds(75.0f, fVar3, m370C.x);
                mProcVar2 = current.angle.y + 0x4000;
                m370C.z = gabi::fnmsubs(75.0f, fVar2, m370C.z);
                m35A0 = current.angle.y == shape_angle.y ? 1.0f : -1.0f;
                mProcVar0 = -1;
            } else if (direction == player_move::DIR_RIGHT && (v6 & 8)) {
                m370C.x = gabi::fnmsubs(75.0f, fVar3, m370C.x);
                mProcVar2 = current.angle.y - 0x4000;
                m370C.z = gabi::fmadds(75.0f, fVar2, m370C.z);
                m35A0 = current.angle.y == shape_angle.y ? 1.0f : -1.0f;
                mProcVar0 = -1;
            } else if ((v6 & 1) && ((shape_angle.y == current.angle.y && direction == 0) ||
                                    (shape_angle.y != current.angle.y && direction == 1))) {
                s16 a = current.angle.y;
                m370C.x = gabi::fmadds(75.0f, cM_ssin(a), m370C.x);
                mProcVar0 = -1;
                m370C.z = gabi::fmadds(75.0f, cM_scos(a), m370C.z);
                mProcVar2 = shape_angle.y;
                m35A0 = 0.0f;
            } else if ((shape_angle.y == current.angle.y && direction == 1) ||
                       (shape_angle.y != current.angle.y && direction == 0)) {
                s16 a = current.angle.y + 0x8000;
                current.angle.y = a;
                m370C.x = gabi::fnmsubs(75.0f, cM_ssin(a), m370C.x);
                mProcVar0 = -1;
                m370C.z = gabi::fnmsubs(75.0f, cM_scos(a), m370C.z);
                mProcVar2 = shape_angle.y;
                m35A0 = 0.0f;
            }
            if (mProcVar0 == -1) {
                playerMovePlay()->mAdvanceDirection = 0; /* dComIfGp_setAdvanceDirection(0) */
                if (shape_angle.y == current.angle.y) {
                    frameCtrl.mLoop = 0;
                    frameCtrl.setRate(2.0f);
                } else {
                    frameCtrl.mLoop = frameCtrl.mEnd;
                    frameCtrl.setRate(-2.0f);
                }
                initSeAnime();
            }
        } else {
            setDoStatusCrawl();
            shape_angle.y = sVar4;
            current.angle.y = sVar5;
        }
    } else {
        BOOL bVar1 = TRUE;
        s16 v3 = mProcVar3;
        f32 r = m35A0;
        if (v3 > 0) {
            mProcVar3 = v3 - 1;
        }
        if (std::fabs(r) > 0.5f) {
            if (cLib_addCalcAngleS(&shape_angle.y, mProcVar2, 5, 0x480, 0x80)) {
                bVar1 = FALSE;
            }
            if (m35A0 < -0.5f) {
                current.angle.y = shape_angle.y + 0x8000;
            } else {
                current.angle.y = shape_angle.y;
            }
            cLib_addCalc(&current.pos.x, m370C.x, 0.5f, 3.0f, 1.0f);
            cLib_addCalc(&current.pos.z, m370C.z, 0.5f, 3.0f, 1.0f);
        }
        gabi::Local<cXyz> local_28;
        cXyz_mi(&m370C, local_28, &current.pos);
        if (cLib_distanceAngleS(cM_atan2s(local_28->x, local_28->z), current.angle.y) < 0x6000) {
            bVar1 = FALSE;
        }
        f32 spd = getCrawlMoveSpeed();
        if (spd < 0.0f) {
            mNormalSpeed = -spd;
            current.angle.y = shape_angle.y + 0x8000;
        } else {
            mNormalSpeed = spd;
        }
        if (bVar1 || mProcVar3 == 0) {
            playerMoveCrawlInitRegister(this, (s32)shape_angle.x, (s32)shape_angle.z);
        }
    }
    return TRUE;
}

/* 0242D7EC */
BOOL daPy_lk_c::procCrawlEnd() {
    m35E4 = (mFrameCtrlUnder[0].getFrame() - 3.0f) * m35A0; /* HD: HIO field_0x48 (3.0) folded */
    if (mFrameCtrlUnder[0].getRate() > -0.01f) {
        checkNextMode(0);
    } else if (mFrameCtrlUnder[0].getFrame() < 3.0f) {
        if (!checkNextMode(1)) {
            m34C2 = 1;
        }
    } else {
        m34C2 = 1;
    }
    return TRUE;
}

/* ---- d_a_player_grab.inc (start) ---- */

/* 0242D888 */
void daPy_lk_c::setWeaponBlur() {
    J3DModel* heap = setItemHeap(); /* JKRHeap* */
    PlayerMoveSwBlur* blur = gabi::at<PlayerMoveSwBlur>(mpSwBlur); /* HD: daPy_swBlur_c is allocated */
    u32 buf = playerMoveAllocateAligned(0x4800, 0x20); /* new (0x20) Vec[2 * 0x300] */
    blur->mpPosBuffer = gabi::at<cXyz>(buf); /* mSwBlur.mpPosBuffer */
    playerMoveRestoreHeap(heap);
}

/* 0242D8DC */
BOOL daPy_lk_c::procGrabUp_init() {
    commonProcInit(player_move::daPyProc_GRAB_UP_e);
    s32 end = (mActorKeepGrab.mActor->actor_status & (fopAcStts_UNK10000_e | fopAcStts_UNK8000000_e)) ? 5 : 9; /* HD: HIO folded */
    setSingleMoveAnime(player_move::ANM_GRABUP, 0.9f, 0.0f, end, 1.0f);
    gabi::Local<cXyz> xz; /* sp+0x08 */
    gabi::Local<cXyz> d;  /* sp+0x14 */
    cXyz_mi(&mActorKeepGrab.mActor->current.pos, d, &m3748);
    m370C.x = d->x;
    m370C.z = d->z;
    xz->y = 0.0f;
    xz->z = d->z;
    xz->x = d->x;
    m370C.y = d->y;
    m35C8 = std_sqrtf(PSVECSquareMag(xz)) - 47.0f; /* m370C.absXZ() - 47.0f */
    playerDemoVoiceStart(this, (mActorKeepGrab.mActor->actor_status & fopAcStts_UNK10000_e) ? 16 : 15);
    fopAcM_setStageLayer(mActorKeepGrab.mActor.get());
    setResetFlg0(resetFlg0() | player_move::daPyRFlg0_GRAB_UP_START);
    fopAc_ac_c* grab = mActorKeepGrab.mActor;
    if (grab != nullptr && fpcM_GetName(grab) == player_move::fpcNm_Stone2_e) { /* HD: NULL check */
        dComIfGp_setPlayerStatus1(0, player_move::daPyStts1_UNK40000_e);
    }
    return TRUE;
}

/* 0242DA4C */
BOOL daPy_lk_c::procGrabMiss_init() {
    commonProcInit(player_move::daPyProc_GRAB_MISS_e);
    setSingleMoveAnime(player_move::ANM_GRABNG, 1.0f, 0.0f, 6, 1.0f); /* HD: HIO folded */
    mProcVar6 = 0;
    mProcVar0 = 0xC;
    playerMoveOnEventBit(dComIfGs_getEvent(), player_move::dSv_event_flag_UNK_4020);
    return TRUE;
}

/* 02426964 */
/* HD: the thrown message bottle (probably the HD Tingle Bottle): the throw animation, then an
 * event camera follows the bottle until it is far or hidden, then the camera is reset */
BOOL daPy_lk_c::dProcHDBottleThrow() {
    setShipRidePosUseItem();
    s32 st = mProcVar6;
    if (st == 3) {
        f32 t = m35A0 + 1.0f;
        if (t > 7.0f) {
            m35A0 = 0.0f;
            mProcVar6 = 4;
        } else {
            m35A0 = t;
            st = mProcVar6;
            if (st < 4) {
                return TRUE;
            }
        }
    } else if (st == 4) {
        f32 a = m35A0 + 1.0f;
        f32 b = m35A4 + 0.1f;
        m35A0 = a;
        m35A4 = b;
        f32 f0 = b > 1.0f ? 1.0f : playerDemoFloatSelect(b, b, 0.0f);
        for (s32 i = 0; i < playerDemoView(this).mBlendCount;
             i++) { /* [?] an HD float array (count +0x5840, data +0x5844) */
            playerDemoView(this).mpBlendWeights[i] = f0;
        }
        if (m35A0 > 17.0f && playerDemoOldFrame(m_old_fdata)->mOldFrameRate < 0.01f) {
            mProcVar6 = 5;
            setSingleMoveAnime(0x6A, 0.8f, 1.0f, 0xE, 0.0f);
            fopAcM_cancelCarryNow(playerDemoView(this).mHDThrowActor.get());
            playerDemoView(this).mHDThrowActor.get()->mThrowState = 1;
        }
        st = mProcVar6;
        if (st < 4) {
            return TRUE;
        }
    } else if (st == 5) {
        J3DFrameCtrl& fc = mFrameCtrlUnder[0];
        if (!(fc.getFrame() < (f32)(s32)(fc.mEnd - 1))) {
            setSingleMoveAnime(player_demo::ANM_WAITS, 1.0f, 0.0f, -1, 5.0f);
            mProcVar6 = 6;
        }
    } else if (st == 6) {
        BOOL r21 = FALSE;
        s32 idx = mCameraInfoIdx;
        u32 cam = gabi::ea(playerDemoCamera(idx));
        PlayerDemoBottle* actor = playerDemoView(this).mHDThrowActor;
        bool endCamera = true;
        if (actor != 0) {
            cXyz d;  /* sp+0x08 */
            cXyz e1; /* sp+0x50 */
            cXyz e2; /* sp+0x5C */
            cXyz e3; /* sp+0x68 */
            playerDemoCameraEye(cam, &e1);
            playerDemoCameraEye(cam, &e2);
            playerDemoCameraEye(cam, &e3);
            d.x = (f32)actor->current.pos.x - e1.x;
            d.y = (f32)actor->current.pos.y - e2.y;
            d.z = (f32)actor->current.pos.z - e3.z;
            f64 len = playerDemoVectorMagnitudeRegister(&d); /* f1 compared as returned */
            if (len > (f64)(f32)(l_bottleTuning.mCameraDistance + 1500.0f) ||
                d.y < l_bottleTuning.mCameraDrop + -500.0f) {
                r21 = TRUE;
            }
            gabi::Local<dBgS_LinChk> chk; /* sp+0x80 */
            cXyz eye;                     /* sp+0x74 */
            playerDemoConstructLineCheck(chk);
            playerDemoCameraEye(cam, &eye);
            dBgS_LinChk_Set(chk, &playerDemoView(this).mHDThrowActor.get()->current.pos, &eye,
                            this);
            if (cBgS_LineCross(dComIfG_Bgsp(), chk)) {
                r21 = TRUE;
            }
            playerDemoDestroyLineCheck(chk);
            actor = playerDemoView(this).mHDThrowActor;
            if (!r21 && actor->mThrowState != 5) {
                st = mProcVar6;
                if (st < 4) {
                    return TRUE;
                }
                endCamera = false;
            }
        }
        if (endCamera) {
            u32 id = actor != 0 ? (u32)actor->mBsPcId : 0xFFFFFFFF;
            playerDemoEndEventCamera(playerDemoGetCameraBody(), id);
            actor = playerDemoView(this).mHDThrowActor;
            if (actor != 0) {
                cXyz center; /* sp+0x14 */
                cXyz eyeP;   /* sp+0x20 */
                playerDemoCopyFloatBits(gabi::ea(&center.x),
                                        gabi::ea(&actor->current.pos.x));
                playerDemoCopyFloatBits(gabi::ea(&center.y),
                                        gabi::ea(&actor->current.pos.y));
                playerDemoCopyFloatBits(gabi::ea(&eyeP.y), gabi::ea(&m370C.y));
                playerDemoCopyFloatBits(gabi::ea(&center.z),
                                        gabi::ea(&actor->current.pos.z));
                playerDemoCopyFloatBits(gabi::ea(&eyeP.z), gabi::ea(&m370C.z));
                playerDemoCopyFloatBits(gabi::ea(&eyeP.x), gabi::ea(&m370C.x));
                playerDemoResetCamera(cam + 0x248, &center, &eyeP);
                actor = playerDemoView(this).mHDThrowActor;
            }
            u32 id2 = actor != 0 ? (u32)actor->mBsPcId : 0xFFFFFFFF;
            playerDemoForceLockOff(cam + 0x248, id2);
            playerDemoStartCamera(cam + 0x248);
            dComIfGp_event_onEventFlag(8);
            endDemoMode();
            playerDemoView(this).mHDThrowActor = nullptr;
            mProcVar6 = 7;
        }
    } else if (st < 4) {
        return TRUE;
    }
    if (playerDemoView(this).mHDThrowActor != nullptr) {
        s32 idx = mCameraInfoIdx;
        u32 cam = gabi::ea(playerDemoCamera(idx));
        s16 maxStep = (s16)(l_bottleTuning.mCameraAngleStep + 0x800);
        cLib_addCalcAngleS(&playerDemoView(this).mHDThrowAngle, shape_angle.y, 6, maxStep, 0x40);
        cLib_addCalc(&m35A8, 1.0f, 5.0f, 0.25f, 0.1f);
        s16 a = playerDemoView(this).mHDThrowAngle;
        f32 r = gabi::fmadds(120.0f, m35A8, 100.0f);
        f32 y = gabi::fmadds(l_bottleTuning.mCameraHeight + 130.0f, m35A8,
                             playerDemoView(this).mHDThrowHeight + 70.0f);
        cXyz target; /* sp+0x08 */
        target.y = y;
        target.x = gabi::fnmsubs(cM_ssin(a), r, current.pos.x);
        target.z = gabi::fnmsubs(cM_scos(a), r, current.pos.z);
        cLib_addCalcPos(&m370C, &target, 5.0f, l_bottleTuning.mCameraStep + 38.0f, 20.0f);
        gabi::Local<dBgS_LinChk> chk; /* sp+0x74 */
        playerDemoConstructLineCheck(chk);
        dBgS_LinChk_Set(chk, &playerDemoView(this).mHDThrowActor.get()->current.pos, &m370C,
                        this);
        cXyz center; /* sp+0x2C */
        cXyz eyeP;   /* sp+0x38 */
        if (!cBgS_LineCross(dComIfG_Bgsp(), chk)) {
            PlayerDemoBottle* actor = playerDemoView(this).mHDThrowActor;
            playerDemoCopyFloatBits(gabi::ea(&center.x), gabi::ea(&actor->current.pos.x));
            playerDemoCopyFloatBits(gabi::ea(&center.y), gabi::ea(&actor->current.pos.y));
            playerDemoCopyFloatBits(gabi::ea(&eyeP.y), gabi::ea(&m370C.y));
            playerDemoCopyFloatBits(gabi::ea(&center.z), gabi::ea(&actor->current.pos.z));
            playerDemoCopyFloatBits(gabi::ea(&eyeP.z), gabi::ea(&m370C.z));
            playerDemoCopyFloatBits(gabi::ea(&eyeP.x), gabi::ea(&m370C.x));
        } else {
            u32 c =
                gabi::ea(chk.get()) + 0x30; /* GetCross(): float copies kept bit-exact as in the original */
            playerDemoCopyFloatBits(gabi::ea(&m370C.y), c + 4);
            playerDemoCopyFloatBits(gabi::ea(&m370C.z), c + 8);
            playerDemoCopyFloatBits(gabi::ea(&m370C.x), c + 0);
            PlayerDemoBottle* actor = playerDemoView(this).mHDThrowActor;
            playerDemoCopyFloatBits(gabi::ea(&center.x), gabi::ea(&actor->current.pos.x));
            playerDemoCopyFloatBits(gabi::ea(&center.y), gabi::ea(&actor->current.pos.y));
            playerDemoCopyFloatBits(gabi::ea(&eyeP.z), c + 8);
            playerDemoCopyFloatBits(gabi::ea(&center.z), gabi::ea(&actor->current.pos.z));
            playerDemoCopyFloatBits(gabi::ea(&eyeP.x), c + 0);
            playerDemoCopyFloatBits(gabi::ea(&eyeP.y), c + 4);
        }
        playerDemoSetCamera(cam + 0x248, &center, &eyeP);
        playerDemoDestroyLineCheck(chk);
    }
    return TRUE;
}

/* WWHD glue (tools/verify/autoglue.py)
ext 0241FBCC BOOL daPy_lk_c::checkEndMessage(u32)
ext 023F14E0 BOOL daPy_lk_c::checkNextMode(int)
ext 023E26EC int daPy_lk_c::checkShipRideUseItem(int)
ext 023DFDD8 BOOL daPy_lk_c::commonProcInit(int)
ext 02423BCC void daPy_lk_c::dProcPresent_init_sub()
ext 023DC7AC void daPy_lk_c::deleteEquipItem(BOOL)
ext 023F2048 void daPy_lk_c::endDemoMode()
ext 023DCF8C void daPy_lk_c::freeGrabItem()
ext 023DDEEC u32 daPy_lk_c::getItemAnimeResource(u32)
ext 023E2E18 void daPy_lk_c::initShipRideUseItem(int, int)
ext 023EEB10 void daPy_lk_c::keepItemData()
ext 023DC4DC u32 daPy_lk_c::loadTextureScrollResource(u32, BOOL)
ext 023F6564 BOOL daPy_lk_c::procFall_init(int, f32)
ext 024188F0 void daPy_lk_c::procLadderDownStart_init_sub()
ext 02418230 BOOL daPy_lk_c::procLadderMove_init(int, int, cXyz*)
ext 02418178 BOOL daPy_lk_c::procLadderUpEnd_init(int)
ext 024186A4 void daPy_lk_c::procLadderUpStart_init_sub()
ext 023E2FF4 BOOL daPy_lk_c::procWait_init()
ext 023DC6A4 BOOL daPy_lk_c::resetActAnimeUpper(int, f32)
ext 023DF9C0 void daPy_lk_c::resetFootEffect()
ext 023EF750 void daPy_lk_c::setAnimeEquipItem()
ext 023E14A0 void daPy_lk_c::setBlendMoveAnime(f32)
ext 023E55A4 void daPy_lk_c::setBlurPosResource(u16)
ext 023DF0FC void daPy_lk_c::setBottleModel(u32)
ext 0241FA98 void daPy_lk_c::setLetterModel()
ext 023E278C void daPy_lk_c::setOldRootQuaternion(s16, s16, s16)
ext 0241FB78 void daPy_lk_c::setShapeAngleToTalkActor()
ext 023E287C void daPy_lk_c::setShipRidePos(int)
ext 023E2DC4 void daPy_lk_c::setShipRidePosUseItem()
ext 023E0A04 BOOL daPy_lk_c::setSingleMoveAnime(int, f32, f32, int, f32)
ext 023DE0B4 void daPy_lk_c::setSwordModel(BOOL)
ext 023DEF04 void daPy_lk_c::setTactModel()
ext 023DD768 void daPy_lk_c::setTextureAnime(u32, int)
ext 023DC43C void daPy_lk_c::setTextureScrollResource(u32, int)

ext 0200EE00 void cLib_addCalcPos(cXyz*, cXyz*, f32, f32, f32)
ext 0203C118 void playerDemoStartTextInput(u32, s32)
ext 021842C8 void playerDemoShowItem(fopAc_ac_c*)
ext 023DC63C void playerDemoClearActorKeep(daPy_actorKeep_l*)
ext 024F8044 u32 playerDemoGetCameraBody()
ext 025052BC void playerDemoForceLockOff(u32, u32)
ext 02514F38 void playerDemoStartCamera(u32)
ext 02514F50 void playerDemoSetCamera(u32, cXyz*, cXyz*)
ext 0251510C void playerDemoResetCamera(u32, cXyz*, cXyz*)
ext 0253E860 void playerDemoEndEventCamera(u32, u32)
ext 0255F378 void dKy_Itemgetcol_chg_on()
ext 0257E560 s32 dKyw_get_tactwind_dir()
ext 025B58B8 void playerDemoEmptyBottle(u32, u32)
ext 025B7AA8 void playerDemoOnTact(u32, u32)
ext 025D5928 fopAc_ac_c* fopAcM_fastCreate(s32, u32, cXyz*, s32, csXyz*, void*, s32, s32, s32)
ext 025D7C98 fopAc_ac_c* fopAcM_getItemEventPartner(fopAc_ac_c*)
ext 025D7CC4 fopAc_ac_c* fopAcM_getEventPartner(fopAc_ac_c*)
ext 025D9D0C void fopAcM_setCarryNow(fopAc_ac_c*, s32)
ext 025E1918 void mDoAud_subBgmStart(u32)
ext 025E1988 void playerDemoSystemSound(u32)
ext 025E8508 void playerDemoInitBck(mDoExt_bckAnm*, u32, u32, s32, s32, f32, s32, s32, s32)
ext 025E871C void playerDemoChangeBckOnly(mDoExt_bckAnm*, u32)
ext 028E8E10 f64 playerDemoVectorMagnitudeRegister(cXyz*)
ext 0200ECD4 f32 cLib_addCalc(be<f32>*, f32, f32, f32, f32)
data l_bottleTuning 1047B608
data l_ladderOffset 1046CCE0
data l_textInputObject 1018F504
data l_textInputReady 1018F510
data l_textInputResult 1047C7FA
ext 0201AD78 void playerDemoAddVector(const cXyz*, cXyz*, const cXyz*)
str 101CEB48 in 02426034
ext 02008254 BOOL playerDemoChkPolySafe(dBgS*, cBgS_PolyInfo*)
ext 024EEABC BOOL playerDemoChkMoveBG(dBgS*, cBgS_PolyInfo*)
ext 024EFA38 void playerDemoMoveBgTransPos(dBgS*, cBgS_PolyInfo*, bool, cXyz*, csXyz*, csXyz*)
ext 024EF080 s32 playerDemoGetWallCode(dBgS*, cBgS_PolyInfo*)
ext 023DE638 void playerDemoSetActorKeep(daPy_actorKeep_l*, fopAc_ac_c*)
ext 023D457C void playerDemoMakeEmitter(daPy_mtxFollowEcallBack_c*, u32, Mtx34*, cXyz*, void*)
ext 023F42EC BOOL playerDemoFinishLetter(daPy_lk_c*)
ext 02417FF4 f32 daPy_lk_c::getLadderMoveAnmSpeed()
ext 0241C714 BOOL daPy_lk_c::procLand_init(f32, int)
ext 02418008 BOOL daPy_lk_c::procClimbMoveUpDown_init(int)
ext 023E0E50 int daPy_lk_c::setMoveAnime(f32, f32, f32, int, int, int, f32)
ext 023E4CD0 int daPy_lk_c::changeSlideProc()
ext 02416230 void daPy_lk_c::setNormalSpeedF(f32, f32, f32, f32)
ext 023E33AC u32 daPy_lk_c::getWHideModePolygon(cXyz*, cXyz*, cXyz*, int)
ext 023ED788 BOOL daPy_lk_c::procWHideReady_init(u32, cXyz*)
ext 02415FB0 int daPy_lk_c::getCrawlMoveVec(cXyz*, cXyz*, cXyz*)
ext 02415EF0 BOOL daPy_lk_c::checkSubjectEnd(BOOL)
ext 02416E90 BOOL daPy_lk_c::setBodyAngleToCamera()
ext 023E4A58 u32 daPy_lk_c::getSlidePolygon()
ext 02412F2C f32 daPy_lk_c::getCrawlMoveAnmSpeed()
ext 023E0660 void daPy_lk_c::initSeAnime()
ext 02427D8C f64 playerMoveHangSpeedRegister(daPy_lk_c*)
ext 024291E0 f64 playerMoveClimbSpeedRegister(daPy_lk_c*)
ext 023E0A04 BOOL playerMoveSingleAnimeRegister(daPy_lk_c*, int, f64, f32, int, f32)
ext 02412F68 BOOL playerMoveCrawlInitRegister(daPy_lk_c*, s32, s32)
ext 020084C8 cM3dGPla* playerMoveGetTriPla(dBgS*, u32, u32)
ext 02008974 f64 playerMoveGroundCross(dBgS*, dBgS_GndChk*)
ext 024EF6E8 f64 playerMoveRoofCheck(dBgS*, dBgS_RoofChk*)
ext 024EF0F4 s32 playerMoveAttributeCode(dBgS*, cBgS_PolyInfo*)
ext 0201AE48 void playerMoveScaleVector(const cXyz*, cXyz*, f32)
ext 028E8D88 void playerMoveAddVector(const cXyz*, const cXyz*, cXyz*)
ext 028E8DAC void playerMoveSubtractVector(const cXyz*, const cXyz*, cXyz*)
ext 02019510 s16 cM_rad2s(f32)
ext 020109FC BOOL cM3d_Len2dSqPntAndSegLine(f32, f32, f32, f32, f32, f32, be<f32>*, be<f32>*, f32*)
ext 024EFF3C void playerMoveSetWallR(dBgS_AcchCir*, f32)
ext 028E8DD0 f64 playerMoveSquareMagnitudeRegister(cXyz*)
ext 028F4384 f64 playerMoveSqrtRegister(f64)
ext 02514E24 BOOL playerMoveCameraChangeModeOK(u32, s32)
ext 0273AEC4 u32 playerMoveAllocateAligned(u32, s32)
ext 025E3570 void playerMoveRestoreHeap(J3DModel*)
ext 025D537C void fopAcM_setStageLayer(fopAc_ac_c*)
ext 025B8B68 void playerMoveOnEventBit(void*, u32)
data l_crawl_front_offset 101CEBCC
data l_crawl_back_offset 101CEBD8
data l_crawl_top_offset 101CEBE4
data l_crawl_front_up_offset 101CEBF0
data l_crawl_back_up_offset 101CEBFC
data l_crawl_stand_up_offset 101CEC08
data l_crawl_top_up_offset 101CEC14
data l_crawl_side_offset 101CEC20
data l_crawl_lside_offset 101CEC2C
data l_crawl_rside_offset 101CEC38
data l_crawl_lside_front_offset 101CEC44
data l_crawl_rside_front_offset 101CEC50
data l_crawl_min_side_offset 101CEC5C
data l_crawl_frame_radians 10036084
data l_crawl_wall_radius 1046CCE4
data l_maba_state 101CEF18
*/
