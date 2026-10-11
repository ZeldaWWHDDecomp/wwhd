/** Sliding puzzle actor, restored to zeldaret/tww names and HD layouts.
 * Verified against the original WWHD unit. */
#include "d/actor/d_a_obj_apzl.h"

const gc_tu_data gc_tu = {.safeString = 0x10024AFC};
static ApzlMessagePointer apzlMessage;
static ApzlResourcePointer apzlResourceControl;
static ApzlPadPointer apzlPad;
static s16 apzlPuzzleOverride;
static u32 apzlStaticWords[4] = {};
static f32 apzlAngleLimits[2] = {};
static u8 apzlStaticFlags[2] = {};
static ApzlDestructorRecord apzlDestructorRecords[3] = {
    {0, 0, 0, 0, 16, 70, 144, 200, 2, 128, 16, 36},
    {0, 0, 0, 0, 16, 70, 144, 196, 2, 142, 217, 148},
    {0, 0, 0, 0, 16, 70, 144, 197, 2, 142, 173, 200}};
static const char* daObjApzl_act_name[8] = {"WAIT", "TALK",  "GAME",   "GETITEM",
                                            "STOP", "SOUND", "PUSH_A", "EVENT_END"};
static const u8 daObjApzl_rupee_table[30] = {1, 0, 1, 0, 1, 0, 2, 0, 1, 0, 2, 0, 3, 0, 1,
                                             0, 1, 2, 1, 0, 1, 1, 1, 0, 1, 3, 1, 1, 1, 4};
// HD: each piece loads its own model resource; GC swaps a shared model's material table.
// The glue initializer checker requires literal resource IDs in this table.
static const u32 daObjApzl_bmt_table[16][16] = {
    {0xd, 0xe, 0xf, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c},
    {0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b,
     0x2c},
    {0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b,
     0x3c},
    {0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b,
     0x4c},
    {0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b,
     0x5c},
    {0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b,
     0x6c},
    {0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b,
     0x7c},
    {0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b,
     0x8c},
    {0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b,
     0x9c},
    {0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab,
     0xac},
    {0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb,
     0xbc},
    {0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb,
     0xcc},
    {0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb,
     0xdc},
    {0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb,
     0xec},
    {0xed, 0xee, 0xef, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb,
     0xfc},
    {0xfd, 0xfe, 0xff, 0x100, 0x101, 0x102, 0x103, 0x104, 0x105, 0x106, 0x107, 0x108, 0x109, 0x10a,
     0x10b, 0x10c},
};

/* 023183CC */
BOOL daObjApzl_c::move_piece() {
    apzlSTControlCheckTrigger(stick.get());
    u8 timer = mMoveTimer;
    if (timer) {
        mMoveTimer = (u8)(timer - 1);
        return FALSE;
    }
    ApzlPad *controller = apzlPad;
    // HD: pad trigger/repeat masks replace the GC stick direction queries.
    u32 repeated = (u32)controller->mButtonRepeat, restricted = (u32)controller->mDirectionRestrict,
        pressed = (u32)controller->mButtonTrigger;
    u32 keys = pressed | repeated;
    if (restricted & APZL_PAD_DIRECTION_MASK)
        keys &= APZL_PAD_DIRECTION_MASK;
    bool swapped = false;
    u8 blank = mBlankIdx, position = mPiecePos[blank];
    if ((position & 3) != 3 && (keys & APZL_PAD_LEFT)) {
        mMoveDirection = DIRECTION_LEFT;
        position = getblank();
        u8 piece = search_piece((u8)(position + 1));
        blank = mBlankIdx;
        mSwappedPieceIdx = piece;
        position = mPiecePos[blank];
        swapped = true;
    }
    if ((position & 3) != 0 && (keys & APZL_PAD_RIGHT)) {
        mMoveDirection = DIRECTION_RIGHT;
        position = getblank();
        u8 piece = search_piece((u8)(position - 1));
        blank = mBlankIdx;
        mSwappedPieceIdx = piece;
        position = mPiecePos[blank];
        swapped = true;
    }
    if ((position & 12) != 12 && (keys & APZL_PAD_UP)) {
        mMoveDirection = DIRECTION_UP;
        position = getblank();
        u8 piece = search_piece((u8)(position + 4));
        blank = mBlankIdx;
        mSwappedPieceIdx = piece;
        position = mPiecePos[blank];
        swapped = true;
    }
    if ((position & 12) != 0 && (keys & APZL_PAD_DOWN)) {
        mMoveDirection = DIRECTION_DOWN;
        position = getblank();
        u8 piece = search_piece((u8)(position - 4));
        blank = mBlankIdx;
        mSwappedPieceIdx = piece;
        swap_piece(blank, piece);
        mMoveTimer = check_clear() ? 40 : ITEM_PURPLE_RUPEE;
        return TRUE;
    }
    if (swapped) {
        u8 piece = mSwappedPieceIdx;
        swap_piece(blank, piece);
        mMoveTimer = check_clear() ? 40 : ITEM_PURPLE_RUPEE;
    }
    return swapped;
}

/* 02318598 */
void daObjApzl_c::check_arrow_draw() {
    for (u32 i = 0; i < 4; ++i)
        mDrawArrow[i] = 0;
    if ((u8)mState != STATE_PLAYING || (u8)mMoveTimer != 0)
        return;
    if ((getblank() & 3) != 3)
        mDrawArrow[DIRECTION_LEFT] = 1;
    if ((getblank() & 3) != 0)
        mDrawArrow[DIRECTION_RIGHT] = 1;
    if ((getblank() & 12) != 12)
        mDrawArrow[DIRECTION_UP] = 1;
    if ((getblank() & 12) != 0)
        mDrawArrow[DIRECTION_DOWN] = 1;
}

/* 02316DB8 */
u8 daObjApzl_c::search_piece(u8 piece) {
    for (u32 i = 0;; ++i)
        if (mPiecePos[i] == piece)
            return (u8)i;
}

/* 02316DA0 */
void daObjApzl_c::swap_piece(u8 first, u8 second) {
    u8 value = mPiecePos[second], old = mPiecePos[first];
    mPiecePos[first] = value;
    mPiecePos[second] = old;
}

/* 02316DDC */
void daObjApzl_c::randamize_piece() {
    for (u32 i = 0; i < 16; ++i)
        mPiecePos[i] = (u8)i;
    for (;;) {
        for (u32 step = 0; step < 10000; ++step) {
            u32 direction = (u32)gabi::ftoi(cM_rndF(4.0f)) & 3;
            u8 blank = mBlankIdx, position = mPiecePos[blank];
            if (direction == DIRECTION_LEFT) {
                if ((position & 3) != 3) {
                    u8 piece = search_piece((u8)(position + 1));
                    swap_piece(blank, piece);
                }
            } else if (direction == DIRECTION_RIGHT) {
                if ((position & 3) != 0) {
                    u8 piece = search_piece((u8)(position - 1));
                    swap_piece(blank, piece);
                }
            } else if (direction == DIRECTION_UP) {
                if ((position & 12) != 12) {
                    u8 piece = search_piece((u8)(position + 4));
                    swap_piece(blank, piece);
                }
            } else if ((position & 12) != 0) {
                u8 piece = search_piece((u8)(position - 4));
                swap_piece(blank, piece);
            }
        }
        u8 matches = 0;
        for (u32 i = 0; i < 16; ++i)
            if ((u8)mPiecePos[i] == (u8)i)
                ++matches;
        if (matches < 3)
            break;
    }
    for (u32 step = 0; step < 3; ++step) {
        u8 blank = mBlankIdx, position = mPiecePos[blank];
        if ((position & 3) != 3) {
            u8 piece = search_piece((u8)(position + 1));
            swap_piece(blank, piece);
            blank = mBlankIdx;
            position = mPiecePos[blank];
        }
        if ((position & 12) != 0) {
            u8 piece = search_piece((u8)(position - 4));
            swap_piece(blank, piece);
        }
    }
}

/* 0231839C */
void daObjApzl_c::save_piece() {
    for (u32 i = 0; i < 16; ++i) {
        dSv_info_c *save = dComIfGs_info();
        u8 position = mPiecePos[i];
        save->mSavedata.mPlayer.mInfo.mPuzzleData[i] = position;
    }
}

/* 02317014 */
BOOL daObjApzl_c::check_clear() {
    for (u32 i = 0; i < 16; ++i)
        if ((u8)mPiecePos[i] != (u8)i)
            return FALSE;
    return TRUE;
}

/* 02317BA0 */
u16 daObjApzl_c::next_msgStatus(u32 *msg) {
    u32 id = *reinterpret_cast<be<u32> *>(msg);
    msg_class *message = apzlMessage;
    auto assign = [&](u32 n) { *reinterpret_cast<be<u32> *>(msg) = n; };
    switch (id) {
    case 0x1BC6:
        assign((s32)message->mSelectNum ? 0x1BC8 : 0x1BC7);
        return fopMsgStts_MSG_CONTINUES_e;
    case 0x1BC7:
        assign((s32)message->mSelectNum ? 0x1BC9 : 0x1BCA);
        return fopMsgStts_MSG_CONTINUES_e;
    case 0x1BCA:
        assign(0x1BCB);
        return fopMsgStts_MSG_CONTINUES_e;
    case 0x1BCB:
        assign((s32)message->mSelectNum ? 0x1BCC : 0x1BC9);
        return fopMsgStts_MSG_CONTINUES_e;
    case 0x1BCC:
        assign(0x1BCA);
        return fopMsgStts_MSG_CONTINUES_e;
    case 0x1BCD:
        if ((s32)message->mSelectNum) {
            assign(0x1BCE);
            return fopMsgStts_MSG_CONTINUES_e;
        }
        return fopMsgStts_MSG_ENDS_e;
    case 0x1BC9:
    case 0x1BCF:
        return fopMsgStts_MSG_ENDS_e;
    case 0x1BD0:
        mShownRewardMessage = 1;
        return fopMsgStts_MSG_ENDS_e;
    default:
        mQuitGame = 1;
        return fopMsgStts_MSG_ENDS_e;
    }
}

/* 02317B4C */
u32 daObjApzl_c::getMsg() {
    if ((u8)mShownRewardMessage)
        return 0x1BC5;
    if ((u8)mGaveReward)
        return 0x1BD0;
    if ((u8)mGameCleared)
        return 0x1BCF;
    return (u8)mGameStarted ? 0x1BCD : 0x1BC6;
}

/* 02317CB0 */
u16 daObjApzl_c::talk(s32 mode) {
    // HD: the message singleton and a started flag replace GC mpCurrMsg.
    u32 current = mCurrMsgId;
    msg_class *message = apzlMessage;
    u16 result = 255;
    if (current == 0xFFFFFFFF) {
        u32 id;
        if (mode == 1) {
            id = getMsg();
            mMsgNo = id;
        } else
            id = mMsgNo;
        u32 next = apzlMessageSet(message, id, &eyePos);
        mCurrMsgId = next;
        if (next != 0xFFFFFFFF)
            mMessageStarted = 0;
    } else if ((u8)mMessageStarted) {
        result = (u16)apzlMessageStatus(message);
        if (result == APZL_MSG_DISPLAYED) {
            u16 status = next_msgStatus((u32 *)&mMsgNo);
            apzlMessageSetStatus(message, status);
            if (apzlMessageStatus(message) == fopMsgStts_MSG_CONTINUES_e) {
                u32 id = mMsgNo;
                apzlMessageSet(message, id, 0);
            }
        } else if (result == fopMsgStts_BOX_CLOSED_e) {
            apzlMessageSetStatus(message, APZL_MSG_DESTROYED);
            mCurrMsgId = 0xFFFFFFFF;
        }
    } else
        mMessageStarted = 1;
    return result;
}

/* 02317DE4 */
void daObjApzl_c::privateCut() {
    ApzlPlay *play = reinterpret_cast<ApzlPlay *>(dComIfGp_get());
    s32 staff = apzlGetMyStaffId(&play->mEventManager, "Apzl", 0, 0);
    if (staff == -1)
        return;
    play = reinterpret_cast<ApzlPlay *>(dComIfGp_get());
    s8 action = apzlGetMyActIdx(&play->mEventManager, staff, daObjApzl_act_name, 8, 1, 0);
    mActIdx = action;
    play = reinterpret_cast<ApzlPlay *>(dComIfGp_get());
    if (action == -1) {
        apzlCutEnd(&play->mEventManager, staff);
        return;
    }
    if (apzlGetIsAdvance(&play->mEventManager, staff)) {
        action = mActIdx;
        switch (action) {
        case ACTION_GAME:
            mGameStarted = 1;
            break;
        case ACTION_GETITEM:
            mRewardTimer = 150;
            mGaveReward = 1;
            break;
        case ACTION_SOUND:
            apzlStartSound(JA_SE_15PUZZLE_COMPLETE);
            break;
        case ACTION_EVENT_END: {
            for (s32 i = 0; i < (s32)mGivenRupeeCount; ++i) {
                ApzlLinkage id;
                id.value = mRupeeIds[i];
                fopAc_ac_c *item = nullptr;
                if ((u32)id.value != 0xFFFFFFFF)
                    item = apzlFindItem(APZL_FIND_ITEM_BY_ID, &id);
                if (item) {
                    item->current.angle.x = 0;
                    item->current.angle.y = 0;
                    item->current.angle.z = 0;
                }
            }
            break;
        }
        }
    }
    action = mActIdx;
    bool finish = false;
    switch (action) {
    case ACTION_TALK:
        finish = talk(1) == fopMsgStts_BOX_CLOSED_e;
        break;
    case ACTION_GETITEM: {
        play = reinterpret_cast<ApzlPlay *>(dComIfGp_get());
        play->mDoStatusForce = 0;
        play = reinterpret_cast<ApzlPlay *>(dComIfGp_get());
        play->mAStatusForce = 0;
        s32 timer = mRewardTimer;
        if (!timer) {
            finish = true;
            break;
        }
        timer = (s32)((u32)timer - 1);
        mRewardTimer = timer;
        if ((u32)timer & 2)
            break;
        s32 count = mGivenRupeeCount;
        if (count >= 30)
            break;
        csXyz angle;
        cXyz position;
        cXyz linkage;
        s16 ax = current.angle.x, ay = current.angle.y, az = current.angle.z;
        angle.z = az;
        angle.x = ax;
        position.x = 0;
        angle.y = ay;
        position.y = 100.0f;
        position.z = 735.0f;
        s16 rx = (s16)gabi::ftoi(cM_rndFX(4000.0f));
        angle.x = (s16)(ax + rx);
        s16 ry = (s16)gabi::ftoi(cM_rndFX(6000.0f));
        ay = angle.y;
        u8 puzzle = mPuzzleNo;
        count = mGivenRupeeCount;
        angle.z = 0;
        angle.y = (s16)(ay + ry - 0x4000);
        u32 itemNo;
        if (puzzle == 15) {
            u8 reward = daObjApzl_rupee_table[(u32)count];
            itemNo = reward == 0   ? ITEM_GREEN_RUPEE
                     : reward == 1 ? ITEM_BLUE_RUPEE
                     : reward == 2 ? ITEM_YELLOW_RUPEE
                     : reward == 3 ? ITEM_RED_RUPEE
                                   : ITEM_PURPLE_RUPEE;
        } else
            itemNo = count % 6 == 0 ? ITEM_BLUE_RUPEE : ITEM_GREEN_RUPEE;
        f32 velocity = cM_rndF(15.0f) + 5.0f;
        f32 height = cM_rndF(15.0f) + 5.0f;
        s8 room = current.roomNo;
        fopAc_ac_c *item = apzlFastCreateItem(&position, itemNo, room, &angle, 0, -1, 0, velocity,
                                              height, -2.0999999046325684f);
        if (item) {
            u32 flags = (u32)item->actor_status;
            item->actor_status = flags | fopAcStts_UNK4000_e;
            apzlSetItemTimer(item, 450);
        }
        count = mGivenRupeeCount;
        u32 id = item ? (u32)item->mBsPcId : 0xFFFFFFFF;
        mRupeeIds[(u32)count] = id;
        count = mGivenRupeeCount;
        mGaveReward = 1;
        mGivenRupeeCount = (s32)((u32)count + 1);
        mRewardTimer = 60;
        return;
    }
    case ACTION_STOP:
        return;
    case ACTION_SOUND:
        play = reinterpret_cast<ApzlPlay *>(dComIfGp_get());
        play->mDoStatusForce = 0;
        play = reinterpret_cast<ApzlPlay *>(dComIfGp_get());
        play->mAStatusForce = 0;
        finish = !apzlSoundIsPlaying(JA_SE_15PUZZLE_COMPLETE);
        break;
    case ACTION_PUSH_A:
        play = reinterpret_cast<ApzlPlay *>(dComIfGp_get());
        play->mDoStatusForce = 25;
        play = reinterpret_cast<ApzlPlay *>(dComIfGp_get());
        play->mAStatusForce = 0;
        finish = apzlCheckATrigger(0) != 0;
        break;
    default:
        finish = true;
        break;
    }
    if (finish) {
        play = reinterpret_cast<ApzlPlay *>(dComIfGp_get());
        apzlCutEnd(&play->mEventManager, staff);
    }
}

/* 02316D9C */
BOOL solidHeapCB(daObjApzl_c *p) { return p->CreateHeap(); }

/* 02316AB0 */
BOOL daObjApzl_c::CreateHeap() {
    for (u32 i = 0; i < 16; ++i) {
        s16 override = (s16)apzlPuzzleOverride;
        if (override > 0) {
            override = (s16)apzlPuzzleOverride;
            s32 puzzle = override - 1;
            dSv_info_c *save = dComIfGs_info();
            if (puzzle >= 16)
                apzlPuzzleOverride = 0;
            save->mSavedata.mPlayer.mInfo.mPuzzleData[16] = (u8)puzzle;
        }
        dSv_info_c *save = dComIfGs_info();
        u8 puzzle = (u8)save->mSavedata.mPlayer.mInfo.mPuzzleData[16];
        if (puzzle >= 16)
            puzzle = 0;
        u32 resource = daObjApzl_bmt_table[puzzle][i];
        SafeString key;
        // Reserve caller linkage space below the SafeString passed to actual callees.
        ApzlLinkage keyLinkage;
        key.mStringTop = gabi::guest_str("Apzl");
        key.__vtbl = gc_tu.safeString;
        void *data = apzlGetResource(apzlResourceControl.get(), &key, resource);
        if (!data)
            JUT_ASSERT_fail("d_a_obj_apzl.cpp", 0x324, "modelData != (0)");
        mpPieceModel[i] = mDoExt_J3DModel__create((J3DModelData *)data, 0x80000, 0x37441422);
        if (!mpPieceModel[i])
            return FALSE;
    }
    SafeString arrows;
    ApzlLinkage arrowsLinkage;
    arrows.mStringTop = gabi::guest_str("Apzl");
    arrows.__vtbl = gc_tu.safeString;
    void *data = apzlGetResource(apzlResourceControl.get(), &arrows, dRes_INDEX_APZL_BDL_APZLY_e);
    if (!data)
        JUT_ASSERT_fail("d_a_obj_apzl.cpp", 0x340, "modelData != (0)");
    for (u32 i = 0; i < 4; ++i) {
        mpArrowModel[i] = mDoExt_J3DModel__create((J3DModelData *)data, 0, 0x11020203);
        if (!mpArrowModel[i])
            return FALSE;
    }
    SafeString scores;
    ApzlLinkage scoresLinkage;
    scores.mStringTop = gabi::guest_str("Apzl");
    scores.__vtbl = gc_tu.safeString;
    data = apzlGetResource(apzlResourceControl.get(), &scores, dRes_INDEX_APZL_BDL_VBSRP_e);
    if (!data)
        JUT_ASSERT_fail("d_a_obj_apzl.cpp", 0x34B, "modelData != (0)");
    SafeString animationName;
    ApzlLinkage animationNameLinkage;
    animationName.mStringTop = gabi::guest_str("Apzl");
    animationName.__vtbl = gc_tu.safeString;
    void *animation =
        apzlGetResource(apzlResourceControl.get(), &animationName, dRes_INDEX_APZL_BTP_VBSRP_e);
    if (!animation)
        JUT_ASSERT_fail("d_a_obj_apzl.cpp", 0x350, "btp_data != 0");
    for (u32 i = 0; i < 16; ++i) {
        mpScoreboardModel[i] = mDoExt_J3DModel__create((J3DModelData *)data, 0, 0x11020203);
        if (!mpScoreboardModel[i])
            return FALSE;
        if (!mScoreboardBtpAnm[i].init((J3DModelData *)data, (J3DAnmTexPattern *)animation, true, 0,
                                       1.0f, 0, -1, false, 0))
            return FALSE;
    }
    stick = apzlSTControlCreate(0, 60, 30, 0, 0, 0.9f, 0.5f, 0, 0);
    if (!stick)
        JUT_ASSERT_fail("d_a_obj_apzl.cpp", 0x361, "stick != (0)");
    return TRUE;
}

/* 023175B8 */
void daObjApzl_c::CreateInit() {
    mType = (u8)(fopAcM_GetParam(this) >> 8);
    dSv_info_c *save = dComIfGs_info();
    u8 puzzle = (u8)save->mSavedata.mPlayer.mInfo.mPuzzleData[16];
    if (puzzle >= 16)
        puzzle = 0;
    mPuzzleNo = puzzle;
    for (u32 i = 0; i < 16; ++i) {
        mScoreboardBtpAnm[i].mFrameCtrl.mFrame = i < puzzle ? 1.0f : 0.0f;
        puzzle = mPuzzleNo;
    }
    J3DModel *model = mpPieceModel[0];
    cullMtx = model ? gabi::ea(&model->mBaseTransformMtx) : 0;
    fopAcM_setCullSizeBox(this, -600.0f, -600.0f, -600.0f, 600.0f, 600.0f, 600.0f);
    u8 type = mType;
    mShowBlankPiece = 0;
    mBlankIdx = 3;
    cullSizeFar = 1.0f;
    if (!type)
        randamize_piece();
    else
        for (u32 i = 0; i < 16; ++i)
            mPiecePos[i] = (u8)i;
    ApzlPlay *play = reinterpret_cast<ApzlPlay *>(dComIfGp_get());
    mEventIdx[EVENT_TALK] = apzlGetEventIdx(&play->mEventManager, "PUZZLE_TALK", 255);
    play = reinterpret_cast<ApzlPlay *>(dComIfGp_get());
    mEventIdx[EVENT_GAME] = apzlGetEventIdx(&play->mEventManager, "PUZZLE_GAME", 255);
    play = reinterpret_cast<ApzlPlay *>(dComIfGp_get());
    s32 rewardEvent = apzlGetEventIdx(&play->mEventManager, "PUZZLE_RUPEE", 255);
    type = mType;
    // GHS copies position components as words; preserve signaling-NaN bits and store order.
    u32 y = (u32) reinterpret_cast<ApzlWordPosition *>(&current.pos)->y;
    s16 talkEvent = mEventIdx[EVENT_TALK];
    mEventIdx[EVENT_RUPEE] = rewardEvent;
    eventInfo.mEventId = talkEvent;
    if (!type) {
        u32 flags = (u32)attention_info.flags;
        attention_info.flags = flags | fopAc_Attn_TALKFLAG_CHECK_e | fopAc_Attn_ACTION_SPEAK_e;
    }
    reinterpret_cast<ApzlWordPosition *>(&attention_info.position)->y = y;
    mMessageStarted = 0;
    mState = STATE_IDLE;
    mMoveTimer = 0;
    mQuitGame = 0;
    u32 x = (u32) reinterpret_cast<ApzlWordPosition *>(&current.pos)->x;
    mGameCleared = 0;
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 40;
    mShownRewardMessage = 0;
    u32 z = (u32) reinterpret_cast<ApzlWordPosition *>(&current.pos)->z;
    mCurrMsgId = 0xFFFFFFFF;
    mPlayedStartSound = 0;
    reinterpret_cast<ApzlWordPosition *>(&attention_info.position)->z = z;
    mGaveReward = 0;
    mGameStarted = 0;
    mSwappedPieceIdx = 0;
    reinterpret_cast<ApzlWordPosition *>(&attention_info.position)->x = x;
    mMoveDirection = DIRECTION_UP;
    for (u32 i = 0; i < 4; ++i)
        mDrawArrow[i] = 0;
    mGivenRupeeCount = 0;
    set_mtx();
}

static inline void apzlCopyMatrix(J3DModel *model) {
    // HD: float assignment, with all twelve loads before any model stores.
    model->setBaseTRMtx(mDoMtx_stack_c::get());
}

/* 0231704C */
void daObjApzl_c::set_mtx() {
    for (u32 i = 0; i < 16; ++i) {
        J3DModel *model = mpPieceModel[i];
        f32 z = scale.z, x = scale.x, y = scale.y;
        model->mBaseScale.x = x;
        model->mBaseScale.y = y;
        model->mBaseScale.z = z;
        x = current.pos.x;
        y = current.pos.y;
        z = current.pos.z;
        PSMTXTrans(mDoMtx_stack_c::get(), x, y, z);
        s16 angle = current.angle.y;
        mDoMtx_YrotM(mDoMtx_stack_c::get(), angle);
        u8 position = mPiecePos[i];
        x = gabi::fmadds((f32)(position % 4), 40.0f, -60.0f);
        y = -gabi::fmadds((f32)(position / 4), 40.0f, -60.0f);
        mDoMtx_stack_c::transM(x, y, 0.0f);
        if (i == (u8)mSwappedPieceIdx) {
            u8 timer = mMoveTimer;
            if (timer) {
                BOOL clear = check_clear();
                u8 direction = mMoveDirection;
                f32 distance = (clear ? 1.0f : 8.0f) * (f32)timer;
                if (direction == DIRECTION_UP)
                    mDoMtx_stack_c::transM(0.0f, -distance, 0.0f);
                else if (direction == DIRECTION_LEFT)
                    mDoMtx_stack_c::transM(distance, 0.0f, 0.0f);
                else if (direction == DIRECTION_DOWN)
                    mDoMtx_stack_c::transM(0.0f, distance, 0.0f);
                else
                    mDoMtx_stack_c::transM(-distance, 0.0f, 0.0f);
            }
        }
        model = mpPieceModel[i];
        apzlCopyMatrix(model);
    }
    for (u32 i = 0; i < 4; ++i) {
        J3DModel *model = mpArrowModel[i];
        f32 z = scale.z, x = scale.x, y = scale.y;
        model->mBaseScale.x = x;
        model->mBaseScale.y = y;
        model->mBaseScale.z = z;
        x = current.pos.x;
        y = current.pos.y;
        z = current.pos.z;
        PSMTXTrans(mDoMtx_stack_c::get(), x, y, z);
        s16 angle = current.angle.y;
        mDoMtx_YrotM(mDoMtx_stack_c::get(), angle);
        u8 position = getblank();
        x = gabi::fmadds((f32)(position % 4), 40.0f, -60.0f);
        y = -gabi::fmadds((f32)(position / 4), 40.0f, -60.0f);
        mDoMtx_stack_c::transM(x, y, 0.0f);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), (s16)(i * 0x4000));
        mDoMtx_stack_c::transM(0.0f, -15.0f, 0.0f);
        model = mpArrowModel[i];
        apzlCopyMatrix(model);
    }
    for (u32 i = 0; i < 16; ++i) {
        f32 y = scale.y, x = scale.x;
        J3DModel *model = mpScoreboardModel[i];
        f32 z = scale.z;
        model->mBaseScale.y = y;
        model->mBaseScale.z = z;
        model->mBaseScale.x = x;
        y = gabi::fmadds(28.6200008392334f, -(f32)((i % 8) + 1), 10.0f);
        z = 28.6200008392334f * (f32)((s32)(i / 8) - 2);
        PSMTXTrans(mDoMtx_stack_c::get(), 0.0f, y, z);
        model = mpScoreboardModel[i];
        apzlCopyMatrix(model);
    }
}

/* 02317878 */
s32 daObjApzl_c::_create() {
    u32 flags = actor_condition;
    if (!(flags & fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            fopAc_ac_c_ct(this);
            __vtbl = APZL_ACTOR_VTABLE;
            apzlConstructArray(&mScoreboardBtpAnm[0], 16, 0x74, APZL_BTP_CONSTRUCTOR);
            flags = (u32)actor_condition;
        }
        actor_condition = flags | fopAcCnd_INIT_e;
    }
    s32 phase = dComIfG_resLoad(&mPhs, "Apzl");
    if (phase == cPhs_COMPLEATE_e) {
        if (!apzlEntrySolidHeap(this, gabi::guest_fn((const void *)solidHeapCB), 0x1460))
            return cPhs_ERROR_e;
        CreateInit();
    }
    return phase;
}

/* 02317954 */
BOOL daObjApzl_c::_delete() {
    ApzlSTControl *controller = stick;
    if (controller)
        controller->destroy(3);
    dComIfG_resDelete(&mPhs, "Apzl");
    return TRUE;
}

/* 023179B0 */
BOOL daObjApzl_c::_draw() {
    u8 type = mType;
    dComIfGd_setListBG();
    for (u32 i = 0; i < 16; ++i)
        if (type || i != (u8)mBlankIdx || (u8)mShowBlankPiece)
            mDoExt_modelUpdateDL((J3DModel *)mpPieceModel[i], 0);
    dComIfGd_setList();
    if (!type) {
        for (u32 i = 0; i < 4; ++i)
            if ((u8)mDrawArrow[i])
                mDoExt_modelUpdateDL((J3DModel *)mpArrowModel[i], 0);
        for (u32 i = 0; i < 16; ++i) {
            f32 frame = mScoreboardBtpAnm[i].mFrameCtrl.mFrame;
            J3DModel *model = mpScoreboardModel[i];
            J3DModelData *data = model->mModelData;
            mScoreboardBtpAnm[i].entry(data, (s16)gabi::ftoi(frame));
            mDoExt_modelUpdateDL((J3DModel *)mpScoreboardModel[i], 0);
            model = mpScoreboardModel[i];
            ApzlModelData *md = reinterpret_cast<ApzlModelData *>(model->mModelData.get());
            md->mpTexPatternAnm.v = 0;
        }
    }
    return TRUE;
}

/* 02318640 */
BOOL daObjApzl_c::_execute() {
    if (!(u8)mType) {
        switch ((u8)mState) {
        case STATE_IDLE: {
            u16 condition = (u16)eventInfo.mCondition, command = (u16)eventInfo.mCommand;
            eventInfo.mCondition = condition | dEvtCnd_CANTALK_e;
            if (command == dEvtCmd_INTALK_e)
                mState = STATE_TALK;
            break;
        }
        case STATE_TALK: {
            privateCut();
            s16 event = mEventIdx[EVENT_TALK];
            ApzlPlay *play = reinterpret_cast<ApzlPlay *>(dComIfGp_get());
            if (apzlEventEndCheck(&play->mEventManager, event)) {
                if ((u8)mQuitGame) {
                    play = reinterpret_cast<ApzlPlay *>(dComIfGp_get());
                    u16 flags = (u16)play->mEventFlag;
                    play->mEventFlag = flags | 8;
                    mGameStarted = 0;
                    mState = STATE_IDLE;
                    mQuitGame = 0;
                    save_piece();
                } else
                    mState = STATE_START;
            }
            break;
        }
        case STATE_START: {
            if (!(u8)mPlayedStartSound) {
                apzlStartSound(JA_SE_15PUZZLE_START);
                mPlayedStartSound = 1;
            }
            s16 event = mEventIdx[EVENT_GAME];
            apzlOrderChangeEvent(this, event, 0, 65535);
            mState = STATE_PLAYING;
            break;
        }
        case STATE_PLAYING: {
            privateCut();
            if (move_piece())
                apzlStartSound(JA_SE_TALK_CURSOR);
            ApzlPlay *play = reinterpret_cast<ApzlPlay *>(dComIfGp_get());
            play->mDoStatusForce = 0;
            play = reinterpret_cast<ApzlPlay *>(dComIfGp_get());
            play->mAStatusForce = 39;
            if (check_clear() && !(u8)mMoveTimer) {
                mShowBlankPiece = 1;
                mGameCleared = 1;
                play = reinterpret_cast<ApzlPlay *>(dComIfGp_get());
                cXyz direction;
                direction.y = 1.0f;
                direction.x = 0.0f;
                direction.z = 0.0f;
                apzlStartShock(&play->mVibration, 4, 1, &direction);
                mState = STATE_END;
            } else if (apzlCheckBTrigger(0)) {
                apzlStartSound(JA_SE_15PUZZLE_EXIT);
                mState = STATE_END;
            }
            break;
        }
        case STATE_END: {
            u8 cleared = mGameCleared;
            mQuitGame = 0;
            if (cleared) {
                dSv_info_c *save = dComIfGs_info();
                u8 count = (u8)((u8)save->mSavedata.mPlayer.mInfo.mPuzzleData[16] + 1),
                   puzzle = mPuzzleNo;
                if (count >= 16)
                    count = 0;
                mScoreboardBtpAnm[puzzle].mFrameCtrl.mFrame = 1.0f;
                save = dComIfGs_info();
                save->mSavedata.mPlayer.mInfo.mPuzzleData[16] = count;
                s16 event = mEventIdx[EVENT_RUPEE];
                apzlOrderChangeEvent(this, event, 0, 65535);
                mState = STATE_RUPEE;
            } else {
                s16 event = mEventIdx[EVENT_TALK];
                apzlOrderChangeEvent(this, event, 0, 65535);
                mState = STATE_TALK;
            }
            [[fallthrough]];
        }
        case STATE_RUPEE: {
            privateCut();
            s16 event = mEventIdx[EVENT_RUPEE];
            ApzlPlay *play = reinterpret_cast<ApzlPlay *>(dComIfGp_get());
            if (apzlEventEndCheck(&play->mEventManager, event)) {
                play = reinterpret_cast<ApzlPlay *>(dComIfGp_get());
                u16 flags = (u16)play->mEventFlag;
                play->mEventFlag = flags | 8;
                mState = STATE_IDLE;
                mGameStarted = 0;
                mQuitGame = 0;
            }
            break;
        }
        }
        check_arrow_draw();
    }
    set_mtx();
    return TRUE;
}

/* 02318A14 */
BOOL IsDelete(daObjApzl_c *p) { return TRUE; }

/* HD: compiler-generated and template code of this translation unit. */

/* 0231896C */
void staticInitialize() {
    apzlStaticWords[2] = 0;
    apzlStaticWords[0] = 0;
    apzlStaticWords[3] = 0;
    apzlStaticWords[1] = 0;
    apzlRegisterGlobal(&apzlDestructorRecords[0]);
    apzlAngleLimits[0] = -3.1415927410125732f;
    apzlAngleLimits[1] = 3.1415927410125732f;
    apzlLimitInit(&apzlStaticFlags[0]);
    apzlRegisterGlobal(&apzlDestructorRecords[1]);
    apzlAngleInit(&apzlStaticFlags[1]);
    apzlRegisterGlobal(&apzlDestructorRecords[2]);
}

/* 02318A00 */
void deleteStatic(void *p, s32 flags) {
    if (p && (flags & 1))
        operator_delete(p);
}

/* 02318A1C */
void destruct(daObjApzl_c *p, s32 flags) {
    if (p) {
        apzlActorDestructor(p, 0);
        if (flags & 1)
            operator_delete(p);
    }
}

/* 02318A70 */
void emptyVirtual() {}
/* WWHD glue (tools/verify/autoglue.py)
str 10024AF4 in 02317878
str 10024F38
str 10024F48
str 10024F5C
str 10024F70
str 10024FBC
str 10024FC8
str 10024FD4
str 10024FE4 in 02317954
str 10025008
str 10024F30 in 02316AB0
data apzlMessage 101F4B5C
data apzlResourceControl 101F4F28
data apzlPad 101F5088
data apzlPuzzleOverride 1047C80A
data apzlStaticWords 104690C8 nocheck # BSS storage, initialized by staticInitialize.
data apzlAngleLimits 104690BC nocheck # BSS storage, initialized by staticInitialize.
data apzlStaticFlags 104690C4 nocheck # BSS storage, initialized by staticInitialize.
data apzlDestructorRecords 101C7C00
data daObjApzl_act_name 101C7BE0
data daObjApzl_rupee_table 101C7C54
data daObjApzl_bmt_table 10024B24
ext 028EFFD0 void apzlConstructArray(void*, u32, u32, u32)
ext 025D63E8 s32 apzlEntrySolidHeap(fopAc_ac_c*, u32, u32)
ext 025D50BC void apzlActorDestructor(fopAc_ac_c*, s32)
ext 025F7DB0 u32 apzlMessageSet(msg_class*, u32, cXyz*)
ext 025F795C u32 apzlMessageStatus(msg_class*)
ext 025F74D0 void apzlMessageSetStatus(msg_class*, u32)
ext 026066C4 void* apzlGetResource(void*, SafeString*, s32)
ext 0258861C ApzlSTControl* apzlSTControlCreate(ApzlSTControl*, s32, s32, s32, s32, f32, f32, s32, s32)
ext 0258873C void apzlSTControlCheckTrigger(ApzlSTControl*)
ext 02543F10 s32 apzlGetEventIdx(dEvent_manager_c*, const char*, u32)
ext 025440C8 s32 apzlEventEndCheck(dEvent_manager_c*, s16)
ext 025E1988 void apzlStartSound(u32)
ext 025D7874 void apzlOrderChangeEvent(fopAc_ac_c*, s16, u8, u16)
ext 025CB374 void apzlStartShock(dVibration_c*, s32, s32, cXyz*)
ext 020076E0 s32 apzlCheckBTrigger(s32)
ext 020076BC s32 apzlCheckATrigger(s32)
ext 02542D88 s32 apzlGetMyStaffId(dEvent_manager_c*, const char*, fopAc_ac_c*, s32)
ext 02542EDC s32 apzlGetMyActIdx(dEvent_manager_c*, s32, const ApzlStringPointer*, s32, s32, s32)
ext 02543280 void apzlCutEnd(dEvent_manager_c*, s32)
ext 025447C8 s32 apzlGetIsAdvance(dEvent_manager_c*, s32)
ext 025D5218 fopAc_ac_c* apzlFindItem(u32, void*)
ext 025D8AB0 fopAc_ac_c* apzlFastCreateItem(cXyz*, u32, s8, csXyz*, u32, s32, s32, f32, f32, f32)
ext 0218084C void apzlSetItemTimer(fopAc_ac_c*, s32)
ext 025E1B24 s32 apzlSoundIsPlaying(u32)
ext 028F026C void apzlRegisterGlobal(ApzlDestructorRecord*)
ext 028ED6F8 void apzlLimitInit(be<u8>*)
ext 028EAB2C void apzlAngleInit(be<u8>*)
*/
