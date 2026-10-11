/**
 * d_camera_mode.cpp (WWHD)
 * Follow camera dCamera_c: camera types, modes and styles (mode/type/style changes, map-tool
 * camera lookup), lock-on and attention helpers.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/d_camera.cpp) to the WWHD layout and code, verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/d_camera_mode.h"

namespace camera_mode {
static CameraTypes types;
static GuestSignedWord type_num;
static GuestByte m_hyoi_kamome;
static GuestByte mirrorGuard;
static MoveBackgroundTypes mvBGTypes;
static GuestSignedWord mvBGType_num;
static PadOptionPointer padOption;
static CameraName seagalName;
static CameraName boatBattleName;
static s32 compareNames(const char* first, const char* second) {
    auto* a = reinterpret_cast<const be<u8>*>(first);
    auto* b = reinterpret_cast<const be<u8>*>(second);
    for (;;) {
        u32 x = *a;
        u32 y = *b;
        if (x != y || x == 0)
            return (s32)(x - y);
        ++a;
        ++b;
    }
}
static bool lockon(dAttention_c* attention) {
    return cameraModeLockonTruth(attention) ||
           (reinterpret_cast<AttentionView*>(attention)->mFlags & 0x20000000);
}
static bool cameraModeOptionC() {
    return (padOption->mFlags & 0x40) != 0;
}
static void copyEntry(void* dst, void* src) {
    auto* d = reinterpret_cast<EntryWords*>(dst);
    auto* s = reinterpret_cast<EntryWords*>(src);
    for (u32 i = 0; i < 5; i++)
        d->mWords[i] = (u32)s->mWords[i];
}
} // namespace camera_mode
using namespace camera_mode;
/* 024F8EF0 */
f32 dCamera_c::heightOf(fopAc_ac_c* actor) {
    if (actor != nullptr && fpcM_GetName(actor) == fpcNm_PLAYER_e /* PLAYER */) {
        return static_cast<daPy_py_c*>(actor)->getHeight(); /* daPy_py_c::getHeight() */
    }
    return (actor->eyePos.y - actor->current.pos.y) * 1.1f;
}

/* 024F9B74 */
// HD: the lock-on target comes from 024EC8D0 (matcher: dAttention_c::ActionTarget;
// GameCube LockonTarget(0))
void dCamera_c::Att() {
    if (mpPlayerActor.get() != nullptr && !chkFlag(0x2000000)) {
        dAttention_c* attn = dComIfGp_getAttention();
        fopAc_ac_c* target = nullptr;
        if (cameraModeLockonTruth(attn)) {            /* LockonTruth */
            target = cameraModeActionTarget(attn, 0); /* LockonTarget(0) */
        }
        mpLockonTarget = target;
    }
}

/* 024FA170 */
s32 dCamera_c::GetCameraTypeFromCameraName(const char* name) {
    s32 current = mCurType;
    if (compareNames(name, reinterpret_cast<const char*>(types.mEntries[current].name)) == 0)
        return current;
    s32 count = type_num;
    s32 i = 0;
    if (count > 0) {
        for (s32 left = count; left != 0; left--) {
            if (compareNames(name, reinterpret_cast<const char*>(types.mEntries[i].name)) == 0)
                break;
            ++i;
        }
    }
    if ((u32)i == (u32)count)
        return 0xFF;
    return i;
}
/* 024FA208 */
// HD: arrow may be null; the original unsigned-byte arrow index check is retained.
s32 dCamera_c::GetCameraTypeFromMapToolID(s32 id, s32 roomNo) {
    auto* stage = reinterpret_cast<StageData*>(cameraModeGameInfo()->play.mStageData);
    StageCamera* camera;
    StageArrow* arrow;
    if (roomNo == -1) {
        camera = gabi::call_ptr<StageCamera*>(stage->mpVtable->getCamera, stage);
        arrow = gabi::call_ptr<StageArrow*>(stage->mpVtable->getArrow, stage);
    } else {
        camera = cameraModeRoomCamera(roomNo);
        arrow = cameraModeRoomArrow(roomNo);
        if (!camera)
            return 0xFF;
    }
    if (id < 0 || !camera || id >= (s32)camera->num)
        return 0xFF;
    s32 count = type_num;
    s32 i = 0;
    if (count > 0) {
        CameraEntry* entries = camera->m_entries;
        for (s32 left = count; left != 0; left--) {
            if (compareNames(reinterpret_cast<const char*>(entries[id].m_cam_type),
                             reinterpret_cast<const char*>(types.mEntries[i].name)) == 0)
                break;
            ++i;
        }
    }
    if ((u32)i == (u32)count)
        return 0xFF;
    CameraEntry* entry = &camera->m_entries[id];
    copyEntry(mCurRoomCamEntry, entry);
    u32 arrowIndex = reinterpret_cast<CameraEntry*>(mCurRoomCamEntry)->m_arrow_idx;
    if (arrow && arrowIndex != (u32)-1 && (s32)arrowIndex < (s32)arrow->num) {
        mCurArrowIdx = arrowIndex;
        ArrowEntry* a = &arrow->m_entries[arrowIndex];
        copyEntry(mCurRoomArrowEntry, a);
    } else
        mCurArrowIdx = 0xFF;
    return i;
}
/* 024FA3CC */
void dCamera_c::pushPos() {
    m084.copy(mViewCache.mCenter);
    m090.copy(mViewCache.mEye);
    *reinterpret_cast<GuestWord*>(&m09C) =
        (u32) * reinterpret_cast<GuestWord*>(&mViewCache.mFovy); /* m09C = mViewCache.mFovy */
    m0A0 = (s16)mViewCache.mBank;
}

/* 024FA79C */
// HD: the restored view is 1280x720
bool dCamera_c::onModeChange(s32 curMode, s32 nextMode) {
    if (curMode == 0xE && (camStyleFlags() & 0x10)) {
        setView(0.0f, 0.0f, 1280.0f, 720.0f);
    }
    m14C = 0.0f;
    m100 = 0;
    m108 = 0;
    m110 = 1;
    m101 = 0;
    mEventFlags = mEventFlags & ~0x211Eu;
    m10C = 0;
    m102 = 0;
    if (curMode == 3)
        clrComStat(dCamAttnStts_00000004_e);
    switch ((u32)nextMode) {
    case 7:
        mEventFlags = mEventFlags | 0x10;
        break;
    case 0:
        if (curMode == 1 && camTypeStyle(mCurType, 0) == camTypeStyle(mCurType, 1))
            m110 = 0;
        break;
    case 1:
        if (curMode == 0 && camTypeStyle(mCurType, 0) == camTypeStyle(mCurType, 1))
            m110 = 0;
        break;
    case 12:
        if (curMode != 12)
            m254 = m254 | 2;
        break;
    }
    return true;
}

/* 024FA90C */
bool dCamera_c::onTypeChange(s32 curType, s32 nextType) {
    m114 = 0;
    s32 mode = mCurMode;
    m118 = 0;
    if (m144 == 0) {
        s32 style = camTypeStyle(nextType, 0);
        if (style >= 0) {
            u32 alg = camStyleAlg(style);
            if ((alg >= dCamAlg_FIXED_POSITION_CAMERA_e && alg <= dCamAlg_FIXED_FRAME_CAMERA_e) ||
                (alg >= dCamAlg_EVENT_CAMERA_e && alg <= dCamAlg_HOOKSHOT_CAMERA_e)) {
                /* FIXED_POSITION, FIXED_FRAME, EVENT, CRAWL, HOOKSHOT */
                mode = 0;
                m144 = 1;
            }
        }
    }
    if (onModeChange(mCurMode, mode))
        m11C = 0;
    return true;
}

/* 024FB2CC */
void dCamera_c::setDMCAngle() {
    mDMCSystem.field_0x0 = 1;
    cSAngle_l tmp;
    s16 inv = cSAngle_Inv(&mDirection.mU);
    cSAngle_l* a = cameraModeAngleCtor(&tmp, inv);
    mDMCSystem.field_0x2 = (s16)*a;
    s16 stick = CPad_GET_STICK_ANGLE(mPadId);
    a = cameraModeAngleCtor(&tmp, stick);
    mDMCSystem.field_0x4 = (s16)*a;
}

/* 024FB334 */
// HD: mHD610 is set when a subject camera style changes to an event camera style
// whose parameter flags have 0x180
bool dCamera_c::onStyleChange(s32 style1, s32 style2) {
    m11C = 0;
    u32 alg1 = camStyleAlg(style1);
    bool fixed = false;
    if (alg1 == dCamAlg_SUBJECT_CAMERA_e) {
        setComZoomScale(1.0f);
        clrComStat(dCamAttnStts_TELESCOPE_LOOK_e | dCamAttnStts_PICTO_BOX_AIM_e);
    } else if (alg1 >= dCamAlg_FIXED_POSITION_CAMERA_e && alg1 <= dCamAlg_FIXED_FRAME_CAMERA_e) {
        if (mDMCSystem.field_0x0 == 0)
            setDMCAngle();
        fixed = true;
    }
    u32 alg2 = camStyleAlg(style2);
    bool resetDone = false;
    switch (alg2) {
    case dCamAlg_FOLLOW_CAMERA_e:
    case dCamAlg_RIDE_CAMERA_e: /* FOLLOW, RIDE */
        if (alg1 == alg2) {
            mHD610 = 0;
            mEventFlags = mEventFlags | 0x8000;
            resetDone = true;
            break;
        }
        break;
    case dCamAlg_FIXED_POSITION_CAMERA_e:
    case dCamAlg_FIXED_FRAME_CAMERA_e: /* FIXED_POSITION, FIXED_FRAME */
        if (mDMCSystem.field_0x0 == 0 || fixed)
            setDMCAngle();
        /* fall through */
    case dCamAlg_SUBJECT_CAMERA_e:
    case dCamAlg_CRAWL_CAMERA_e:
    case dCamAlg_HOOKSHOT_CAMERA_e: /* SUBJECT, CRAWL, HOOKSHOT */
        if (m144 == 0)
            m144 = 1;
        break;
    }
    if (!resetDone)
        mHD610 = 0;
    if (alg1 == dCamAlg_SUBJECT_CAMERA_e && alg2 == dCamAlg_EVENT_CAMERA_e &&
        (camStyleFlags() & 0x180))
        mHD610 = 1;
    return true;
}

/* 02514E24 */
// HD: the event manager's camera-play flag is read directly
bool dCamera_c::ChangeModeOK(s32 mode) {
    if (cameraModeIsPlaying() /* dComIfGp_evmng_cameraPlay() */ || chkFlag(0x20000000)) {
        return false;
    }
    return camTypeStyle(mCurType, mode) >= 0;
}

/* 024FA410 */
// HD: m514 is a byte (only 0 selects a type); move-BG type 3 keeps the current type;
// a map-tool type does not replace the type while a subject style is active
s32 dCamera_c::nextType(s32 cur) {
    s32 next = cur;
    auto chooseType = [&]() -> s32 {
        if (cameraModeIsPlaying() /* dComIfGp_evmng_cameraPlay() */ || chkFlag(0x20000000)) {
            next = mCamTypeEvent;
            if ((u32)cur != (u32)next) {
                u32 f = mEventFlags;
                s32 ev = mCamTypeEvent;
                mEventFlags = f & ~0x200000u;
                if ((u32)cur != (u32)ev) {
                    pushPos();
                    mEventData.field_0x0c = cur;
                }
            }
            return next;
        }
        if (mpPlayerActor.get() == nullptr || *reinterpret_cast<be<u8>*>(&m514) != 0 /* m514 */)
            return next;
        if ((u32)cur == (u32)(s32)mCamTypeEvent) {
            next = mEventData.field_0x0c;
            mEventData.field_0x0c = -1;
        }
        if ((u8)m_hyoi_kamome != 0) { /* daNpc_kam_c::m_hyoi_kamome */
            next = GetCameraTypeFromCameraName(seagalName.text /* "Seagal" */);
            return next;
        }
        {
            bool boat = (dComIfGp_getPlayerStatus0(mPadId) &
                         (daPyStts0_UNK1000000_e | daPyStts0_SHIP_RIDE_e)) != 0 ||
                        (dComIfGp_getPlayerStatus1(mPadId) & daPyStts1_UNK80_e) != 0;
            s32 idx = mStageMapToolCameraIdx;
            s32 forced = m524;
            if (boat && forced == 0xFF) {
                next = mCamTypeBoat;
                return next;
            }
            s32 roomNo = -1;
            if (idx == 0xFF) {
                s32 room = mRoomNo;
                idx = mRoomMapToolCameraIdx;
                if (room != -1)
                    roomNo = room;
            }
            if (forced != 0xFF) {
                fopAc_ac_c* target = m528.get();
                next = forced;
                if (target != nullptr)
                    mpLockonTarget = target;
                return next;
            }
            if (idx == 0xFF) {
                s32 bg = m350;
                if (bg > 0) {
                    if (bg >= (s32)mvBGType_num /* mvBGType_num */)
                        return next;
                    s32 t =
                        GetCameraTypeFromCameraName(mvBGTypes.mNames[bg].get() /* mvBGTypes[bg] */);
                    if ((u32)t != (u32)(s32)mCamTypeKeep) {
                        if (bg == 3)
                            return next;
                        next = t;
                    }
                    if (bg == 0x11) {
                        s32 playerRoom = mpPlayerActor->current.roomNo; /* fopAcM_GetRoomNo */
                        GetCameraTypeFromMapToolID(0, playerRoom);
                    }
                    return next;
                }
                if (dComIfGp_getPlayerStatus0(mPadId) & daPyStts0_SWIM_e /* SWIM */)
                    return (s32)mCamTypeWater;
                next = mMapToolType;
                return next;
            }
            if (idx == 0x1FF) {
                if (dComIfGp_getPlayerStatus1(mPadId) &
                    daPyStts1_DEKU_LEAF_FLY_e /* DEKU_LEAF_FLY */) {
                    s32 boatType = mCamTypeBoat;
                    next = mMapToolType;
                    if ((u32)cur == (u32)boatType)
                        return (s32)mCamTypeWater;
                } else {
                    if (dComIfGp_getPlayerStatus0(mPadId) & daPyStts0_SWIM_e)
                        next = mCamTypeWater;
                    if ((u32)cur == (u32)(s32)mCamTypeBoat)
                        return (s32)mCamTypeWater;
                }
                if ((u32)cur ==
                    (u32)GetCameraTypeFromCameraName(boatBattleName.text /* "BoatBattle" */))
                    return (s32)mCamTypeWater;
                return next;
            }
            s32 t = GetCameraTypeFromMapToolID(idx, roomNo);
            if ((u32)t == (u32)(s32)mCamTypeKeep) {
                next = cur;
            } else if (t == 0xFF) {
                next = mMapToolType;
            } else if (camStyleAlg(mCurStyle) != dCamAlg_SUBJECT_CAMERA_e /* SUBJECT */) {
                next = t;
            }
            return next;
        }
    };
    next = chooseType();
    m524 = 0xFF;
    m528 = nullptr;
    return next;
}

/* 024FA9C8 */
// HD: the C stick starts the manual camera without the "stick down" condition; a
// pad-option bit (101F5088 +0x18 & 0x40) turns the C-stick-up look into an immediate status;
// shield/aim status order differs (mode 6 before the manual-camera check); 0x800000 always gives
// mode 0x12; a mirror flag (101D5F3F) also selects mode 0x13
s32 dCamera_c::nextMode(s32 cur) {
    dAttention_c* attn = dComIfGp_getAttention();
    cXyz player_pos;
    cXyz temporary;
    positionOf(&player_pos, mpPlayerActor.get());
    s32 next = cur;
    auto S0 = [&]() { return dComIfGp_getPlayerStatus0(mPadId); };
    auto S1 = [&]() { return dComIfGp_getPlayerStatus1(mPadId); };
    auto chooseMode = [&]() -> ModeExit {
        if (cameraModeIsPlaying())
            return NORMAL_TAIL; /* dComIfGp_evmng_cameraPlay() */
        if ((f32)mBG.m00.m58 > player_pos.y)
            m1AE = 0; /* mBG.m00.m58 */
        {
            u32 c = (u32)cur;
            int path; /* 0: setA, 1: stick check, 2: mode 12 */
            if (c >= 10) {
                if (c < 12 || (c > 12 && c <= 14))
                    path = 0;
                else if (c == 12)
                    path = 2;
                else
                    path = m19B != 0 ? 0 : 1;
            } else if (c == 4) {
                path = 0;
            } else {
                if (c == 5 || c == 6) {
                    m184 = 0;
                    m144 = 1;
                }
                if (c == 1 || c == 5 || c == 6)
                    mpLockonTarget = nullptr;
                path = m19B != 0 ? 0 : 1;
            }
            if (path == 0) {
                m184 = 0;
                m144 = 1;
            } else if (path == 1) {
                if (mStickCValueLast > (f32)cameraModeSetup(this)
                                           ->mManualStartCThreshold /* mManualStartCThreshold */) {
                    m144 = 0;
                } else if (cur == 0 || cur == 0x13) {
                    positionOf(&temporary, mpPlayerActor.get());
                    if (mStickMainValueLast < 0.5f &&
                        !cameraModeLockonTruth(dComIfGp_getAttention()) &&
                        !(S0() & daPyStts0_SWIM_e /* SWIM */)) {
                        if (m184 == 1) {
                            if (mStickCPosYLast <
                                (f32)cameraModeSetup(this)->mCstick.m00 /* mCstick.m00 */)
                                m184 = 0;
                        } else if (cameraModeOptionC()) {
                            s32 id = mCameraID;
                            auto& status =
                                cameraModeGameInfo()->play.mCameraInfo[id].mCameraAttentionStatus;
                            status = (u32)status | dCamAttnStts_00001000_e;
                            m184 = 1;
                            setComStat(dCamAttnStts_00000400_e);
                        } else {
                            s32 id = mCameraID;
                            auto& status =
                                cameraModeGameInfo()->play.mCameraInfo[id].mCameraAttentionStatus;
                            status = (u32)status | dCamAttnStts_00000400_e;
                        }
                    }
                }
            } else {
                bool stop;
                if (mStickCValueLast < 0.01f) {
                    stop = mDirection.mRadius < (f32)cameraModeSetup(this)->m098 /* m098 */;
                    if (!stop)
                        stop = chkFlag(0x80000000) || m19B != 0;
                } else {
                    stop = chkFlag(0x80000000) || m19B != 0;
                }
                if (stop) {
                    m184 = 0;
                    m144 = 1;
                }
                if (!cameraModeLockonTruth(dComIfGp_getAttention()) && cameraModeOptionC()) {
                    setComStat(dCamAttnStts_00001000_e);
                    m184 = 1;
                    setComStat(dCamAttnStts_00000400_e);
                }
            }
        }
        if (chkFlag(0x4000000)) {
            s32 m144v = m144;
            s32 pad = mPadId;
            if (m144v == 0)
                m254 = m254 | 1;
            if (dComIfGp_getPlayerStatus0(pad) & daPyStts0_UNK80000000_e)
                mEventFlags = mEventFlags | 0x8000;
            u32 f = mEventFlags;
            m144 = 1;
            mEventFlags = f & ~0x4000000u;
        }
        {
            fopAc_ac_c* lock = mpLockonActor.get();
            if (mLockOnActorId != fpcM_ERROR_PROCESS_ID_e && lock != nullptr &&
                fpcM_GetName(lock) == fpcNm_NPC_MD_e /* NPC_MD */) {
                m144 = 1;
                cur = 0;
                dComIfGp_ea();
            } else {
                if (cur == 12 && m144 != 0) {
                    next = 0;
                    return CLEAR_LOCK_ID;
                }
                dComIfGp_ea();
            }
        }
        if ((S0() & daPyStts0_TELESCOPE_LOOK_e) || (S1() & daPyStts1_PICTO_BOX_AIM_e)) {
            next = 0xE;
            return CLEAR_LOCK_ID;
        }
        if (S0() & (daPyStts0_UNK80000000_e | daPyStts0_UNK80_e)) {
            next = 0x11;
            return CLEAR_LOCK_ID;
        }
        if (S0() & daPyStts0_UNK800000_e) {
            next = 0x12;
            return CLEAR_LOCK_ID;
        }
        if (S1() & daPyStts1_UNK10_e) {
            next = 0xF;
            return CLEAR_LOCK_ID;
        }
        if (S0() & daPyStts0_SUBJECT_e) {
            next = 4;
            return CLEAR_LOCK_ID;
        }
        if ((S0() & (daPyStts0_ROPE_AIM_e | daPyStts0_HOOKSHOT_AIM_e | daPyStts0_BOW_AIM_e)) &&
            !camera_mode::lockon(attn)) {
            next = 10;
            return CLEAR_LOCK_ID;
        }
        if ((S0() & daPyStts0_BOOMERANG_AIM_e) && !camera_mode::lockon(attn)) {
            next = 11;
            return CLEAR_LOCK_ID;
        }
        if (S1() & daPyStts1_UNK4_e) {
            next = 6;
            return CLEAR_LOCK_ID;
        }
        if (m144 == 0) {
            next = 12;
            return CLEAR_LOCK_ID;
        }
        if (S1() & daPyStts1_UNK2_e) {
            next = 5;
            return CLEAR_LOCK_ID;
        }
        if (S0() & (daPyStts0_UNK40_e | daPyStts0_UNK20_e)) {
            next = 6;
            return CLEAR_LOCK_ID;
        }
        if (S0() & (daPyStts0_UNK40_e | daPyStts0_UNK20_e | daPyStts0_UNK1_e)) {
            next = 5;
            return CLEAR_LOCK_ID;
        }
        if ((S0() & (daPyStts0_UNK400_e | daPyStts0_UNK4_e | daPyStts0_UNK2_e)) && cur != 12) {
            if (mpLockonTarget.get() == nullptr)
                return NORMAL_TAIL;
            next = 8;
            return CLEAR_LOCK_ID;
        }
        if (cameraModeLockonTruth(attn) && !(S0() & (daPyStts0_CRAWL_e | daPyStts0_UNK4000000_e))) {
            next = 2;
            return STYLE_ONLY;
        }
        if (camera_mode::lockon(attn)) {
            next = 1;
            return CLEAR_LOCK_ID;
        }
        if ((S0() & daPyStts0_BOOMERANG_WAIT_e) &&
            !(S0() & (daPyStts0_UNK37a02371_e & ~daPyStts0_UNK1000000_e)) &&
            !(S1() & (daPyStts1_UNK10_e | daPyStts1_WIND_WAKER_CONDUCT_e))) {
            fopAc_ac_c* player = mpPlayerActor.get();
            fopAc_ac_c* target = nullptr;
            if (player != nullptr && fpcM_GetName(player) == fpcNm_PLAYER_e) {
                auto* vtable = gabi::at<PlayerVtable>((u32)player->__vtbl);
                u32 id = gabi::call_ptr<u32>(vtable->getThrowBoomerangID,
                                             player); /* getThrowBoomerangID() */
                target = fopAcM_SearchByID(id);
            }
            mpLockonTarget = target;
            next = 2;
            mLockOnActorId = fpcM_ERROR_PROCESS_ID_e;
            return STYLE_ONLY;
        }
        if ((S1() & daPyStts1_UNK80000_e) || (u8)mirrorGuard != 0) {
            next = 0x13;
            return CLEAR_LOCK_ID;
        }
        if (mLockOnActorId != fpcM_ERROR_PROCESS_ID_e) {
            fopAc_ac_c* lock = mpLockonActor.get();
            if (lock == nullptr) {
                next = 0;
                return CLEAR_LOCK_ID;
            }
            mpLockonTarget = lock;
            next = 2;
            return STYLE_ONLY;
        }
        if (cur == 12 && m144 == 0)
            return NORMAL_TAIL;
        next = 0;
        return CLEAR_LOCK_ID;
    };
    ModeExit exit = chooseMode();
    if (exit == CLEAR_LOCK_ID || (exit == NORMAL_TAIL && next != 2))
        mLockOnActorId = fpcM_ERROR_PROCESS_ID_e;
    if (exit != STYLE_ONLY && next == 12) {
        s32 type = mCurType;
        if (camTypeStyle(type, 12) < 0) {
            next = cur;
            if ((u32)type != (u32)(s32)mCamTypeEvent && (u32)type != (u32)(s32)mCamTypeBoat &&
                (u32)type != (u32)(s32)mCamTypeBoatBattle &&
                (u32)type != (u32)(s32)mCamTypeRestrict) {
                m254 = m254 | 1;
            }
            m144 = 1;
        }
    }
    if (camTypeStyle(mCurType, next) >= 0) {
        if (next == 1)
            mEventFlags = mEventFlags | 0x100000;
        return next;
    }
    return cur;
}

/* WWHD glue (tools/verify/autoglue.py)
data types 10049350
data type_num 10049348
data m_hyoi_kamome 101D5F45
data mirrorGuard 101D5F3F
data mvBGTypes 101D549C
data mvBGType_num 1004A488
data padOption 101F5088
data seagalName 1004A7D0
data boatBattleName 1004A7D8
ext 024EDFCC bool cameraModeLockonTruth(dAttention_c*)
ext 024EC8D0 fopAc_ac_c* cameraModeActionTarget(dAttention_c*,s32)
ext 02520708 StageCamera* cameraModeRoomCamera(s32)
ext 02520770 StageArrow* cameraModeRoomArrow(s32)
ext 0200658C cSAngle_l* cameraModeAngleCtor(cSAngle_l*,s32)
*/
