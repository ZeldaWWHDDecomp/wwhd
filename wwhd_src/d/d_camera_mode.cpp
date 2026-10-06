/**
 * d_camera_mode.cpp (WWHD)
 * Follow camera dCamera_c: camera types, modes and styles (mode/type/style changes, map-tool
 * camera lookup), lock-on and attention helpers.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/d_camera.cpp) to the WWHD layout and code, verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/d_camera.h"

/* inline strcmp (byte loop) */
static s32 cam_strcmp(u32 a, u32 b) {
    for (;;) {
        u32 x = gabi::load<u8>(a);
        u32 y = gabi::load<u8>(b);
        if (x != y || x == 0) return (s32)(x - y);
        a++;
        b++;
    }
}

/* 024F8EF0 */
f32 dCamera_c::heightOf(fopAc_ac_c* actor) {
    WWHD_FUNC(0x024F8EF0, f32, this, actor);
    if (actor != nullptr && fpcM_GetName(actor) == 0xA8 /* PLAYER */) {
        return gabi::load<f32>(gabi::ea(actor) + 0x3C8); /* daPy_py_c::getHeight() */
    }
    return (actor->eyePos.y - actor->current.pos.y) * 1.1f;
}
VERIFY(0x024F8EF0, &dCamera_c::heightOf);

/* 024F9B74. HD: the lock-on target comes from 024EC8D0 (matcher: dAttention_c::ActionTarget;
 * GameCube LockonTarget(0)) */
void dCamera_c::Att() {
    WWHD_FUNC(0x024F9B74, void, this);
    if (mpPlayerActor.get() != nullptr && !chkFlag(0x2000000)) {
        dAttention_c* attn = dComIfGp_getAttention();
        fopAc_ac_c* target = nullptr;
        if (gabi::call<bool>(0x024EDFCC, attn)) { /* LockonTruth */
            target = gabi::call<fopAc_ac_c*>(0x024EC8D0, attn, 0); /* LockonTarget(0) */
        }
        mpLockonTarget = target;
    }
}
VERIFY(0x024F9B74, &dCamera_c::Att);

/* 024FA170 */
s32 dCamera_c::GetCameraTypeFromCameraName(const char* name) {
    WWHD_FUNC(0x024FA170, s32, this, name);
    s32 cur = mCurType;
    if (cam_strcmp(gabi::ea(name), CAM_TYPES + cur * 0x40) == 0) return cur;
    s32 num = gabi::load<s32>(CAM_TYPE_NUM);
    s32 i = 0;
    if (num > 0) {
        for (s32 n = num; n != 0; n--) {
            if (cam_strcmp(gabi::ea(name), CAM_TYPES + i * 0x40) == 0) break;
            i++;
        }
    }
    if ((u32)i == (u32)num) return 0xFF;
    return i;
}
VERIFY(0x024FA170, &dCamera_c::GetCameraTypeFromCameraName);

/* 024FA208. HD: the arrow table is checked for NULL */
s32 dCamera_c::GetCameraTypeFromMapToolID(s32 id, s32 roomNo) {
    WWHD_FUNC(0x024FA208, s32, this, id, roomNo);
    u32 stage = dComIfGp_ea() + 0x5150; /* dComIfGp_getStage() */
    u32 camera;
    u32 arrow;
    if (roomNo == -1) {
        camera = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(stage) + 0x24), stage); /* getCamera() */
        arrow = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(stage) + 0x34), stage);  /* getArrow() */
    } else {
        camera = gabi::call<u32>(0x02520708, roomNo); /* dComIfGp_getRoomCamera */
        arrow = gabi::call<u32>(0x02520770, roomNo);  /* dComIfGp_getRoomArrow */
        if (camera == 0) return 0xFF;
    }
    if (id < 0 || camera == 0 || id >= gabi::load<s32>(camera)) return 0xFF;
    s32 num = gabi::load<s32>(CAM_TYPE_NUM);
    s32 i = 0;
    if (num > 0) {
        u32 entries = gabi::load<u32>(camera + 4);
        for (s32 n = num; n != 0; n--) {
            if (cam_strcmp(entries + id * 0x14, CAM_TYPES + i * 0x40) == 0) break;
            i++;
        }
    }
    if ((u32)i == (u32)num) return 0xFF;
    u32 entry = gabi::load<u32>(camera + 4) + id * 0x14;
    for (u32 k = 0; k < 0x14; k += 4) gabi::store<u32>(ea() + 0x5C4 + k, gabi::load<u32>(entry + k)); /* mCurRoomCamEntry */
    u32 arrowIdx = gabi::load<u8>(ea() + 0x5D4); /* mCurRoomCamEntry.m_arrow_idx */
    if (arrow != 0 && arrowIdx != (u32)-1 && (s32)arrowIdx < gabi::load<s32>(arrow)) {
        mCurArrowIdx = arrowIdx;
        u32 a = gabi::load<u32>(arrow + 4) + arrowIdx * 0x14;
        for (u32 k = 0; k < 0x14; k += 4) gabi::store<u32>(ea() + 0x5D8 + k, gabi::load<u32>(a + k)); /* mCurRoomArrowEntry */
    } else {
        mCurArrowIdx = 0xFF;
    }
    return i;
}
VERIFY(0x024FA208, &dCamera_c::GetCameraTypeFromMapToolID);

/* 024FA3CC */
void dCamera_c::pushPos() {
    WWHD_FUNC(0x024FA3CC, void, this);
    m084.copy(mViewCache.mCenter);
    m090.copy(mViewCache.mEye);
    gabi::store<u32>(ea() + 0x9C, gabi::load<u32>(ea() + 0x60)); /* m09C = mViewCache.mFovy */
    m0A0 = (s16)mViewCache.mBank;
}
VERIFY(0x024FA3CC, &dCamera_c::pushPos);

/* 024FA79C. HD: the restored view is 1280x720 */
bool dCamera_c::onModeChange(s32 curMode, s32 nextMode) {
    WWHD_FUNC(0x024FA79C, bool, this, curMode, nextMode);
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
    if (curMode == 3) clrComStat(4);
    switch ((u32)nextMode) {
    case 7:
        mEventFlags = mEventFlags | 0x10;
        break;
    case 0:
        if (curMode == 1 && camTypeStyle(mCurType, 0) == camTypeStyle(mCurType, 1)) m110 = 0;
        break;
    case 1:
        if (curMode == 0 && camTypeStyle(mCurType, 0) == camTypeStyle(mCurType, 1)) m110 = 0;
        break;
    case 12:
        if (curMode != 12) m254 = m254 | 2;
        break;
    }
    return true;
}
VERIFY(0x024FA79C, &dCamera_c::onModeChange);

/* 024FA90C */
bool dCamera_c::onTypeChange(s32 curType, s32 nextType) {
    WWHD_FUNC(0x024FA90C, bool, this, curType, nextType);
    m114 = 0;
    s32 mode = mCurMode;
    m118 = 0;
    if (m144 == 0) {
        s32 style = camTypeStyle(nextType, 0);
        if (style >= 0) {
            u32 alg = camStyleAlg(style);
            if ((alg >= 5 && alg <= 6) || (alg >= 11 && alg <= 13)) {
                /* FIXED_POSITION, FIXED_FRAME, EVENT, CRAWL, HOOKSHOT */
                mode = 0;
                m144 = 1;
            }
        }
    }
    if (onModeChange(mCurMode, mode)) m11C = 0;
    return true;
}
VERIFY(0x024FA90C, &dCamera_c::onTypeChange);

/* 024FB2CC */
void dCamera_c::setDMCAngle() {
    WWHD_FUNC(0x024FB2CC, void, this);
    mDMCSystem.field_0x0 = 1;
    gabi::Local<cSAngle_l> tmp;
    s16 inv = cSAngle_Inv(&mDirection.mU);
    cSAngle_l* a = gabi::call<cSAngle_l*>(0x0200658C, tmp.get(), inv);
    mDMCSystem.field_0x2 = (s16)*a;
    s16 stick = CPad_GET_STICK_ANGLE(mPadId);
    a = gabi::call<cSAngle_l*>(0x0200658C, tmp.get(), stick);
    mDMCSystem.field_0x4 = (s16)*a;
}
VERIFY(0x024FB2CC, &dCamera_c::setDMCAngle);

/* 024FB334. HD: mHD610 is set when a subject camera style changes to an event camera style
 * whose parameter flags have 0x180 */
bool dCamera_c::onStyleChange(s32 style1, s32 style2) {
    WWHD_FUNC(0x024FB334, bool, this, style1, style2);
    m11C = 0;
    u32 alg1 = camStyleAlg(style1);
    bool fixed = false;
    if (alg1 == 4) {
        setComZoomScale(1.0f);
        clrComStat(0x48);
    } else if (alg1 >= 5 && alg1 <= 6) {
        if (mDMCSystem.field_0x0 == 0) setDMCAngle();
        fixed = true;
    }
    u32 alg2 = camStyleAlg(style2);
    switch (alg2) {
    case 1:
    case 8: /* FOLLOW, RIDE */
        if (alg1 == alg2) {
            mHD610 = 0;
            mEventFlags = mEventFlags | 0x8000;
            goto hd610;
        }
        break;
    case 5:
    case 6: /* FIXED_POSITION, FIXED_FRAME */
        if (mDMCSystem.field_0x0 == 0 || fixed) setDMCAngle();
        /* fall through */
    case 4:
    case 12:
    case 13: /* SUBJECT, CRAWL, HOOKSHOT */
        if (m144 == 0) m144 = 1;
        break;
    }
    mHD610 = 0;
hd610:
    if (alg1 == 4 && alg2 == 11 && (camStyleFlags() & 0x180)) mHD610 = 1;
    return true;
}
VERIFY(0x024FB334, &dCamera_c::onStyleChange);

/* 02514E24. HD: the event manager's camera-play flag is read directly */
bool dCamera_c::ChangeModeOK(s32 mode) {
    WWHD_FUNC(0x02514E24, bool, this, mode);
    if (gabi::load<s32>(dComIfGp_ea() + 0x52E4) != 0 /* dComIfGp_evmng_cameraPlay() */ || chkFlag(0x20000000)) {
        return false;
    }
    return camTypeStyle(mCurType, mode) >= 0;
}
VERIFY(0x02514E24, &dCamera_c::ChangeModeOK);

static inline u32 playerStatus0(s32 pad) { return gabi::load<u32>(dComIfGp_ea() + pad * 0x10 + 0x5CD8); }
static inline u32 playerStatus1(s32 pad) { return gabi::load<u32>(dComIfGp_ea() + pad * 0x10 + 0x5CDC); }

/* 024FA410. HD: m514 is a byte (only 0 selects a type); move-BG type 3 keeps the current type;
 * a map-tool type does not replace the type while a subject style is active */
s32 dCamera_c::nextType(s32 cur) {
    WWHD_FUNC(0x024FA410, s32, this, cur);
    s32 next = cur;
    if (gabi::load<s32>(dComIfGp_ea() + 0x52E4) != 0 /* dComIfGp_evmng_cameraPlay() */ || chkFlag(0x20000000)) {
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
        goto end;
    }
    if (mpPlayerActor.get() == nullptr || gabi::load<u8>(ea() + 0x518) != 0 /* m514 */) goto end;
    if ((u32)cur == (u32)(s32)mCamTypeEvent) {
        next = mEventData.field_0x0c;
        mEventData.field_0x0c = -1;
    }
    if (gabi::load<u8>(0x101D5F45) != 0) { /* daNpc_kam_c::m_hyoi_kamome */
        next = GetCameraTypeFromCameraName(STR(0x1004A7D0) /* "Seagal" */);
        goto end;
    }
    {
        bool boat = (playerStatus0(mPadId) & 0x1010000) != 0 || (playerStatus1(mPadId) & 0x80) != 0;
        s32 idx = mStageMapToolCameraIdx;
        s32 forced = m524;
        if (boat && forced == 0xFF) {
            next = mCamTypeBoat;
            goto end;
        }
        s32 roomNo = -1;
        if (idx == 0xFF) {
            s32 room = mRoomNo;
            idx = mRoomMapToolCameraIdx;
            if (room != -1) roomNo = room;
        }
        if (forced != 0xFF) {
            fopAc_ac_c* target = m528.get();
            next = forced;
            if (target != nullptr) mpLockonTarget = target;
            goto end;
        }
        if (idx == 0xFF) {
            s32 bg = m350;
            if (bg > 0) {
                if (bg >= gabi::load<s32>(0x1004A488) /* mvBGType_num */) goto end;
                s32 t = GetCameraTypeFromCameraName(gabi::at<const char>(gabi::load<u32>(0x101D549C + bg * 4)) /* mvBGTypes[bg] */);
                if ((u32)t != (u32)(s32)mCamTypeKeep) {
                    if (bg == 3) goto end;
                    next = t;
                }
                if (bg == 0x11) {
                    s32 playerRoom = gabi::load<s8>(gabi::ea(mpPlayerActor.get()) + 0x326); /* fopAcM_GetRoomNo */
                    GetCameraTypeFromMapToolID(0, playerRoom);
                }
                goto end;
            }
            if (playerStatus0(mPadId) & 0x100000 /* SWIM */) goto water;
            next = mMapToolType;
            goto end;
        }
        if (idx == 0x1FF) {
            if (playerStatus1(mPadId) & 0x20 /* DEKU_LEAF_FLY */) {
                s32 boatType = mCamTypeBoat;
                next = mMapToolType;
                if ((u32)cur == (u32)boatType) goto water;
            } else {
                if (playerStatus0(mPadId) & 0x100000) next = mCamTypeWater;
                if ((u32)cur == (u32)(s32)mCamTypeBoat) goto water;
            }
            if ((u32)cur == (u32)GetCameraTypeFromCameraName(STR(0x1004A7D8) /* "BoatBattle" */)) goto water;
            goto end;
        }
        s32 t = GetCameraTypeFromMapToolID(idx, roomNo);
        if ((u32)t == (u32)(s32)mCamTypeKeep) {
            next = cur;
        } else if (t == 0xFF) {
            next = mMapToolType;
        } else if (camStyleAlg(mCurStyle) != 4 /* SUBJECT */) {
            next = t;
        }
        goto end;
    }
water:
    next = mCamTypeWater;
end:
    m524 = 0xFF;
    m528 = nullptr;
    return next;
}
VERIFY(0x024FA410, &dCamera_c::nextType);

/* 024FA9C8. HD: the C stick starts the manual camera without the "stick down" condition; a
 * pad-option bit (101F5088 +0x18 & 0x40) turns the C-stick-up look into an immediate status;
 * shield/aim status order differs (mode 6 before the manual-camera check); 0x800000 always gives
 * mode 0x12; a mirror flag (101D5F3F) also selects mode 0x13 */
s32 dCamera_c::nextMode(s32 cur) {
    WWHD_FUNC(0x024FA9C8, s32, this, cur);
    dAttention_c* attn = dComIfGp_getAttention();
    u32 at = gabi::ea(attn);
    gabi::Local<cXyz> pp, tmp;
    positionOf(pp, mpPlayerActor.get());
    s32 next = cur;
    auto S0 = [&]() { return playerStatus0(mPadId); };
    auto S1 = [&]() { return playerStatus1(mPadId); };
    auto lockon = [&]() { return gabi::call<bool>(0x024EDFCC, attn) || (gabi::load<u32>(at + 0x20) & 0x20000000); };
    auto optC = []() { return (gabi::load<u32>(gabi::load<u32>(0x101F5088) + 0x18) & 0x40) != 0; };
    if (gabi::load<s32>(dComIfGp_ea() + 0x52E4) != 0) goto tail; /* dComIfGp_evmng_cameraPlay() */
    if (gabi::load<f32>(ea() + 0x2B8) > pp->y) m1AE = 0; /* mBG.m00.m58 */
    {
        u32 c = (u32)cur;
        int path; /* 0: setA, 1: stick check, 2: mode 12 */
        if (c >= 10) {
            if (c < 12 || (c > 12 && c <= 14)) path = 0;
            else if (c == 12) path = 2;
            else path = m19B != 0 ? 0 : 1;
        } else if (c == 4) {
            path = 0;
        } else {
            if (c == 5 || c == 6) {
                m184 = 0;
                m144 = 1;
            }
            if (c == 1 || c == 5 || c == 6) mpLockonTarget = nullptr;
            path = m19B != 0 ? 0 : 1;
        }
        if (path == 0) {
            m184 = 0;
            m144 = 1;
        } else if (path == 1) {
            if (mStickCValueLast > gabi::load<f32>(ea() + 0x7E0) /* mManualStartCThreshold */) {
                m144 = 0;
            } else if (cur == 0 || cur == 0x13) {
                positionOf(tmp, mpPlayerActor.get());
                if (mStickMainValueLast < 0.5f &&
                    !gabi::call<bool>(0x024EDFCC, dComIfGp_ea() + PLAY_ATTENTION) && !(S0() & 0x100000 /* SWIM */)) {
                    if (m184 == 1) {
                        if (mStickCPosYLast < gabi::load<f32>(ea() + 0x830) /* mCstick.m00 */) m184 = 0;
                    } else if (optC()) {
                        s32 id = mCameraID;
                        u32 p = dComIfGp_ea() + id * 0x34 + 0x5B00;
                        gabi::store<u32>(p, gabi::load<u32>(p) | 0x1000);
                        m184 = 1;
                        setComStat(0x400);
                    } else {
                        s32 id = mCameraID;
                        u32 p = dComIfGp_ea() + id * 0x34 + 0x5B00;
                        gabi::store<u32>(p, gabi::load<u32>(p) | 0x400);
                    }
                }
            }
        } else {
            bool stop;
            if (mStickCValueLast < 0.01f) {
                stop = mDirection.mRadius < gabi::load<f32>(ea() + 0x7DC) /* m098 */;
                if (!stop) stop = chkFlag(0x80000000) || m19B != 0;
            } else {
                stop = chkFlag(0x80000000) || m19B != 0;
            }
            if (stop) {
                m184 = 0;
                m144 = 1;
            }
            if (!gabi::call<bool>(0x024EDFCC, dComIfGp_ea() + PLAY_ATTENTION) && optC()) {
                setComStat(0x1000);
                m184 = 1;
                setComStat(0x400);
            }
        }
    }
    if (chkFlag(0x4000000)) {
        s32 m144v = m144;
        s32 pad = mPadId;
        if (m144v == 0) m254 = m254 | 1;
        if (gabi::load<u32>(dComIfGp_ea() + pad * 0x10 + 0x5CD8) & 0x80000000) mEventFlags = mEventFlags | 0x8000;
        u32 f = mEventFlags;
        m144 = 1;
        mEventFlags = f & ~0x4000000u;
    }
    {
        fopAc_ac_c* lock = mpLockonActor.get();
        if (mLockOnActorId != fpcM_ERROR_PROCESS_ID_e && lock != nullptr && fpcM_GetName(lock) == 0x16F /* NPC_MD */) {
            m144 = 1;
            cur = 0;
            dComIfGp_ea();
        } else {
            if (cur == 12 && m144 != 0) goto mode0;
            dComIfGp_ea();
        }
    }
    if ((S0() & 0x200000) || (S1() & 0x8)) { next = 0xE; goto clrId; }
    if (S0() & 0x80000080) { next = 0x11; goto clrId; }
    if (S0() & 0x800000) { next = 0x12; goto clrId; }
    if (S1() & 0x10) { next = 0xF; goto clrId; }
    if (S0() & 0x2000) { next = 4; goto clrId; }
    if ((S0() & 0x25000) && !lockon()) { next = 10; goto clrId; }
    if ((S0() & 0x80000) && !lockon()) { next = 11; goto clrId; }
    if (S1() & 0x4) { next = 6; goto clrId; }
    if (m144 == 0) { next = 12; goto clrId; }
    if (S1() & 0x2) { next = 5; goto clrId; }
    if (S0() & 0x60) { next = 6; goto clrId; }
    if (S0() & 0x61) { next = 5; goto clrId; }
    if ((S0() & 0x406) && cur != 12) {
        if (mpLockonTarget.get() == nullptr) goto tail;
        next = 8;
        goto clrId;
    }
    if (gabi::call<bool>(0x024EDFCC, attn) && !(S0() & 0xC000000)) {
        next = 2;
        goto style;
    }
    if (lockon()) { next = 1; goto clrId; }
    if ((S0() & 0x400000) && !(S0() & 0x36A02371) && !(S1() & 0x11)) {
        fopAc_ac_c* player = mpPlayerActor.get();
        fopAc_ac_c* target = nullptr;
        if (player != nullptr && fpcM_GetName(player) == 0xA8) {
            u32 vt = gabi::load<u32>(gabi::ea(player) + 0xB4);
            u32 id = gabi::call_ptr<u32>(gabi::load<u32>(vt + 0xB4), player); /* getThrowBoomerangID() */
            target = fopAcM_SearchByID(id);
        }
        mpLockonTarget = target;
        next = 2;
        mLockOnActorId = fpcM_ERROR_PROCESS_ID_e;
        goto style;
    }
    if ((S1() & 0x80000) || gabi::load<u8>(0x101D5F3F) != 0) { next = 0x13; goto clrId; }
    if (mLockOnActorId != fpcM_ERROR_PROCESS_ID_e) {
        fopAc_ac_c* lock = mpLockonActor.get();
        if (lock == nullptr) goto mode0;
        mpLockonTarget = lock;
        next = 2;
        goto style;
    }
    if (cur == 12 && m144 == 0) goto tail;
mode0:
    next = 0;
clrId:
    mLockOnActorId = fpcM_ERROR_PROCESS_ID_e;
    goto check12;
tail:
    if (next != 2) mLockOnActorId = fpcM_ERROR_PROCESS_ID_e;
check12:
    if (next == 12) {
        s32 type = mCurType;
        if (camTypeStyle(type, 12) < 0) {
            next = cur;
            if ((u32)type != (u32)(s32)mCamTypeEvent && (u32)type != (u32)(s32)mCamTypeBoat &&
                (u32)type != (u32)(s32)mCamTypeBoatBattle && (u32)type != (u32)(s32)mCamTypeRestrict) {
                m254 = m254 | 1;
            }
            m144 = 1;
        }
    }
style:
    if (camTypeStyle(mCurType, next) >= 0) {
        if (next == 1) mEventFlags = mEventFlags | 0x100000;
        return next;
    }
    return cur;
}
VERIFY(0x024FA9C8, &dCamera_c::nextMode);
