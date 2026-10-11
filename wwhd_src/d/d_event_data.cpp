/* Event cuts, flags and special staff handlers. Verified HD behavior. */
#include "d/d_event_data.h"
using gabi::call_ptr;
static const f32 zero = 0.0f;
static const f32 one = 1.0f;
static const f32 hourToAngle = 15.0f;
static const f32 angleToHour = 0.06666667014360428f;
// HD: day wrapping uses the original double constant.
#define hoursPerDay (*gabi::at<be<f64>>(0x1004DC88))
static const f32 eventCullRate = 10.0f;
static const f32 piMin = -3.1415927410125732f;
static const f32 piMax = 3.1415927410125732f;
#define headerStatics (*gabi::at<EventHeaderStatics>(0x104758D8))
#define messageState (*gabi::at<EventMessageState>(0x104758F4))
#define demoState (*gabi::at<EventDemoState>(0x101D600C))
#define fadeState (*gabi::at<EventFadeState>(0x101F4810))
#define wipeRate (*gabi::at<be<f32>>(0x101D6160))
#define demoArchive (gabi::at<be<u8>>(0x1047E6B8))
#define currentRoom (*gabi::at<be<s8>>(0x1047E6C8))
#define resourceManager (*gabi::at<gptr<void>>(0x101F4F28))
#define messageManager (*gabi::at<gptr<void>>(0x101F4B5C))
static char* packageActions[] = {"WAIT", "PLAY", "PLAY2"};
static char* timekeeperActions[] = {"COUNTDOWN", "WAIT"};
static char* lightActions[] = {"WAIT", "CHANGE", "ADD_TIME"};
static char* createActions[] = {"WAIT", "CREATE"};
static char* soundActions[] = {
    "WAIT",       "STRM_PLAY", "STRM_DEMO_MJ_SISTER", "STRM_DEMO_GO_MAJU",
    "NOMSG_WAIT", "RIDDLE",    "LANDING_DEMO",        "BGMSTOP"};
static char* directorActions[] = {"WAIT",     "NEXT", "FADE",        "BGM_START",    "VIBRATION",
                                  "SE_START", "WIPE", "PLAYER_DRAW", "PLAYER_NODRAW"};
static char* messageActions[] = {"WAIT", "CREATE_MSG", "PUSHBUTTON", "FINISH",   "CONTINUE",
                                 "END",  "DELETE",     "TELOP_ON",   "TELOP_OFF"};
static u32 landing_table[] = {0x2, 0x3, 0x4, 0x5, 0x6, 0x1, 0x7};
// HD: native IDs for JA_BGM_I_MAJU, JA_BGM_TETRA_MEET, JA_BGM_I_LINK_FOREST.
static u32 bgm_table[] = {0x80000007, 0x80000012, 0x80000017};
// HD: native ID for JA_SE_CM_ZL_FDEMO_JIMEN.
static u32 se_table[] = {0x583f};
static dComIfG_inf_c* gameInfo() { return (dComIfG_inf_c*)dComIfGp_get(); }
static EventManagerView* eventManager() { return (EventManagerView*)dComIfGp_getPEvtManager(); }
static EventValue* getSubstance(u32 staff, const char* name, u32 type) {
    return (EventValue*)dComIfGp_evmng_getMySubstanceP(staff, name, type);
}
static u32 staffId(const char* name) { return dComIfGp_evmng_getMyStaffId(name); }
static u32 actionId(u32 staff, gptr<char>* table, u32 count) {
    return dComIfGp_evmng_getMyActIdx(staff, table, count, 0, 0);
}
static u32 isAdvance(u32 staff) { return dComIfGp_evmng_getIsAddvance(staff); }
static void cutEnd(u32 staff) { dComIfGp_evmng_cutEnd(staff); }
// HD: the package fallback uses SafeString's bounded comparison; stage routing is unbounded.
static bool stringEq(const char* a, const char* b) {
    auto* first = gabi::at<be<u8>>(gabi::guest_addr(a));
    auto* second = gabi::at<be<u8>>(gabi::guest_addr(b));
    for (u32 i = 0;; ++i) {
        u8 x = first[i], y = second[i];
        if (x != y)
            return false;
        if (!x)
            return true;
    }
}
static bool boundedStringEq(const char* a, const char* b) {
    auto* first = gabi::at<be<u8>>(gabi::guest_addr(a));
    auto* second = gabi::at<be<u8>>(gabi::guest_addr(b));
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 x = first[i], y = second[i];
        if (x != y)
            return false;
        if (!x)
            return true;
    }
    return false;
}
static void assureTermination(EventSafeString* text) {
    call_ptr<void>(text->__vtbl->assureTermination, text);
}
/* 02540778 */
u32 dEvDtFlag_c::flagMaxCheck(s32 flag_id) {
    if (flag_id == -1)
        return 1;
    if (flag_id >= FlagMax) {
        assertion("d_event_data.cpp", 0xE7, "flag_id < FlagMax");
        return 1;
    }
    return 0;
}
/* 025407D4 */
u32 dEvDtFlag_c::flagCheck(u32 flag_id) {
    if (flagMaxCheck(flag_id))
        return 0;
    return ((u32)mFlag[flag_id >> 5] >> (flag_id & 31)) & 1;
}
/* 02540848 */
u32 dEvDtFlag_c::flagSet(u32 flag_id) {
    if (flagMaxCheck(flag_id))
        return 0;
    u32 byteNo = flag_id >> 5;
    if (byteNo >= 320)
        return 0;
    mFlag[byteNo] = (u32)mFlag[byteNo] | (1u << (flag_id & 31));
    return 1;
}
/* 025408CC */
void dEvDtFlag_c::init() {
    for (u32 i = 0; i < 320; ++i)
        mFlag[i] = 0;
}
/* 025408E8 */
u32 dEvDtCut_c::startCheck() {
    for (u32 i = 0; i < 3; ++i) {
        u32 flag_id = mStartFlag[i];
        if (flag_id == 0xFFFFFFFF)
            return i ? 1 : 0xFFFFFFFF;
        if (!eventManager()->mFlags.flagCheck(flag_id))
            return 0;
    }
    return 1;
}
/* 02542624 */
u32 dEvDtEvent_c::finish_check() {
    for (u32 i = 0; i < 3; ++i) {
        u32 flag_id = mFlagCheckFinish[i];
        if (flag_id == 0xFFFFFFFF)
            return 1;
        if (!eventManager()->mFlags.flagCheck(flag_id))
            return 0;
    }
    return 1;
}
/* 025426C4 */
void dEvDtEvent_c::specialStaffProc(dEvDtStaff_c* staff) {
    for (s32 i = 0; i < (s32)mNStaff; ++i)
        staff[(u32)mStaffIdx[i]].specialProc();
}
/* 025409B4 */
void dEvDtStaff_c::init() {
    s32 type = mStaffType;
    mbHasAction = 0;
    mCurActionIdx = 0xFFFFFFFF;
    mTimer = 0;
    m45 = 0;
    mWipeDirection = 0;
    u32 first = mFirstCutIdx;
    m44 = 0;
    mAdvance = 2;
    mCurCutIdx = first;
    if (type == CAMERA_e)
        eventManager()->mCameraPlay = 1;
}
/* 02540A18 */
void dEvDtStaff_c::advanceCut(u32 cutIdx) {
    mCurCutIdx = cutIdx;
    m45 = 0;
    mCurActionIdx = 0xFFFFFFFF;
    mbHasAction = 0;
    mAdvance = 1;
    mTimer = 0;
    mWipeDirection = 0;
    m44 = 0;
}
/* 02540A48 */
void dEvDtStaff_c::specialProc_WaitStart(u32 staffIdx) {
    EventValue* pTimer = getSubstance(staffIdx, "Timer", dEvDtData_c::TYPE_INT);
    mTimer = pTimer ? pTimer->halves.low : 0;
}
/* 02540AAC */
void dEvDtStaff_c::specialProc_WaitProc(u32 staffIdx) {
    s16 timer = mTimer;
    if (timer > 0)
        mTimer = (u16)(timer - 1);
    else
        cutEnd(staffIdx);
}
/* 0254273C */
void dEvDtBase_c::init() {
    mFDataP = NULL;
    mDataP = NULL;
    mSDataP = NULL;
    mIDataP = NULL;
    mEventP = NULL;
    mHeaderP = NULL;
    mCutP = NULL;
    mStaffP = NULL;
}
/* 02542764 */
u32 dEvDtBase_c::advanceCutLocal(dEvDtStaff_c* staff) {
    dEvDtCut_c* cuts = mCutP.get();
    dEvDtCut_c* cutP = &cuts[(u32)staff->mCurCutIdx];
    u32 next = cutP->mNextCutIdx;
    if (next != 0xFFFFFFFF) {
        u32 status = cuts[next].startCheck();
        if (status == 0xFFFFFFFF) {
            u32 flag_id = cutP->mFlagIdx;
            if (eventManager()->mFlags.flagCheck(flag_id)) {
                staff->advanceCut(cutP->mNextCutIdx);
                return 1;
            }
        } else if (status == 1) {
            u32 flag_id = cutP->mFlagIdx;
            eventManager()->mFlags.flagSet(flag_id);
            staff->advanceCut(cutP->mNextCutIdx);
            return 1;
        }
    }
    staff->mAdvance = staff->mAdvance > 1 ? 1 : 0;
    return 0;
}
/* 02542878 */
void dEvDtBase_c::advanceCut(dEvDtEvent_c* evt) {
    for (s32 i = 0; i < (s32)evt->mNStaff; ++i) {
        u32 idx = evt->mStaffIdx[i];
        dEvDtStaff_c* staff = mStaffP.get();
        advanceCutLocal(&staff[idx]);
    }
}
/* 025428F8 */
static void __sinit_d_event_data_cpp() {
    headerStatics.words[2] = 0;
    headerStatics.words[0] = 0;
    headerStatics.words[3] = 0;
    headerStatics.words[1] = 0;
    registerGlobalObject(gabi::at<void>(0x101D6380));
    headerStatics.piMin = piMin;
    headerStatics.piMax = piMax;
    initializeHeaderByte(&headerStatics.byteObject);
    registerGlobalObject(gabi::at<void>(0x101D638C));
    initializeHeaderFlag(&headerStatics.flagObject);
    registerGlobalObject(gabi::at<void>(0x101D6398));
}
/* 0254298C */
static void EventPackage__dt(void* object, u32 flags) {
    if (object && (flags & 1))
        operator_delete(object);
}
/* 025429A0 */
static void EventPackage_message() {}
/* 025424B8 */
void dEvDtStaff_c::specialProc() {
    switch ((s32)mStaffType) {
    case ALL_e:
        cutEnd(mStaffIdx);
        break;
    case TIMEKEEPER_e:
        specialProcTimekeeper();
        break;
    case DIRECTOR_e:
        specialProcDirector();
        break;
    case MESSAGE_e:
        specialProcMessage();
        break;
    case SOUND_e:
        specialProcSound();
        break;
    case LIGHT_e:
        specialProcLight();
        break;
    case PACKAGE_e:
        specialProcPackage();
        break;
    case CREATE_e:
        specialProcCreate();
        cutEnd(mStaffIdx);
        break;
    default:
        break;
    }
}
/* 02540508 */
static u32 dEvDt_Next_Stage(u32 staffIdx, u32 wipe) {
    u32 roomNo = 0, layerNo = 0xFFFFFFFF, mode = 0;
    EventValue* pStage = getSubstance(staffIdx, "Stage", dEvDtData_c::TYPE_STRING);
    if (!pStage)
        return 0;
    EventValue* pStartCode = getSubstance(staffIdx, "StartCode", dEvDtData_c::TYPE_INT);
    EventValue* p = getSubstance(staffIdx, "RoomNo", dEvDtData_c::TYPE_INT);
    if (p)
        roomNo = p->integer;
    p = getSubstance(staffIdx, "Layer", dEvDtData_c::TYPE_INT);
    if (p)
        layerNo = p->integer;
    EventValue* pHour = getSubstance(staffIdx, "Hour", dEvDtData_c::TYPE_FLOAT);
    p = getSubstance(staffIdx, "Mode", dEvDtData_c::TYPE_INT);
    if (p)
        mode = p->integer;
    p = getSubstance(staffIdx, "Wipe", dEvDtData_c::TYPE_INT);
    if (p)
        wipe = p->integer;
    if (!pStartCode) {
        assertion("d_event_data.cpp", 0xA3, "0");
        return 1;
    }
    if (pHour)
        dKy_set_nexttime(gabi::fmuls_ppc(hourToAngle, pHour->scalar));
    // HD: native stage aliases bypass serialized entrance/room/layer data.
    const char* stage = (const char*)pStage;
    if (stringEq("ENDING", stage))
        dComIfGp_setNextStage("ENDumi", 0, 0, 8, 0, 1, 0, zero);
    else if (stringEq("sea_E", stage))
        dComIfGp_setNextStage("ENDING", 0, 0, 8, 0, 1, 0, zero);
    else
        dComIfGp_setNextStage(stage, (s32)(s16)pStartCode->halves.low, (s32)(s8)roomNo,
                              (s32)(s8)layerNo, mode, 1, (s32)(s8)wipe, zero);
    return 1;
}

/* 02540B0C */
void dEvDtStaff_c::specialProcPackage() {
    // HD: package loading uses SafeString resources and native stage fallback routing.
    enum { ACT_WAIT, ACT_PLAY, ACT_PLAY2 };
    u32 staffIdx = staffId("PACKAGE");
    if (staffIdx == 0xFFFFFFFF) {
        assertion("d_event_data.cpp", 0x412u, "0");
        return;
    }
    u32 actIdx = actionId(staffIdx, packageActions, 3);
    u32 advanceFlag = isAdvance(staffIdx);

    if (advanceFlag) {
        if (actIdx == ACT_WAIT)
            specialProc_WaitStart(staffIdx);
        else if (actIdx == ACT_PLAY || actIdx == ACT_PLAY2) {
            EventValue* filename = getSubstance(staffIdx, "FileName", dEvDtData_c::TYPE_STRING);
            if (!filename)
                assertion("d_event_data.cpp", 0x422u, "sdata");
            EventValue* offset = getSubstance(staffIdx, "OffsetPos", dEvDtData_c::TYPE_VEC);
            EventValue* angle = getSubstance(staffIdx, "OffsetAngY", dEvDtData_c::TYPE_FLOAT);
            f32 rotation = angle ? angle->scalar : zero;
            void* resource = NULL;
            if (demoArchive[0]) {
                EventSafeString archive;
                EventSafeString name;
                archive.mStringPtr = (const char*)demoArchive;
                archive.__vtbl = gabi::at<EventSafeStringVtable>(0x1004D928);
                name.mStringPtr = (const char*)filename;
                name.__vtbl = gabi::at<EventSafeStringVtable>(0x1004D928);
                resource = getObjectRes(resourceManager.get(), &archive, &name);
            }
            if (!resource) {
                resource = dComIfG_getStageRes("Stage", filename);
                if (!resource)
                    assertion("d_event_data.cpp", 0x44Fu, "demo_data");
            }
            dComIfGp_demo_create(resource, offset, rotation);
            gameInfo()->play.mEvtCtrl.mCullFarClipRatio = eventCullRate;
            EventValue* eventFlag = getSubstance(staffIdx, "EventFlag", dEvDtData_c::TYPE_INT);
            if (eventFlag)
                dComIfGs_onEventBit(eventFlag->halves.low);
        }
    }
    if (actIdx == ACT_WAIT) {
        specialProc_WaitProc(staffIdx);
        return;
    }
    if (actIdx == 0xFFFFFFFF) {
        assertion("d_event_data.cpp", 0x488u, "0");
        cutEnd(staffIdx);
        return;
    }
    if (actIdx != ACT_PLAY || (s16)mWipeDirection)
        return;
    u32 state = (u32)demoState.mode;
    if (state == 2) {
        if (dEvDt_Next_Stage(staffIdx, 5u)) {
            mWipeDirection = 1;
            state = (u32)demoState.mode;
        } else {
            EventSafeString a;
            EventSafeString b;
            a.mStringPtr = gabi::at<const char>(gabi::guest_addr("ENDumi"));
            a.__vtbl = gabi::at<EventSafeStringVtable>(0x1004D928);
            b.mStringPtr = gameInfo()->play.mCurStage.mName;
            b.__vtbl = gabi::at<EventSafeStringVtable>(0x1004D928);
            assureTermination(&a);
            assureTermination(&a);
            const char* text = a.mStringPtr.get();
            assureTermination(&b);
            if (text == b.mStringPtr.get() ||
                boundedStringEq(a.mStringPtr.get(), b.mStringPtr.get())) {
                dComIfGp_setNextStage("sea_E", 0u, 44u, 0u, 0u, 1u, 0u, zero);
                mWipeDirection = 1;
            } else
                dComIfGp_demo_remove();
            state = (u32)demoState.mode;
        }
    }
    if (!state)
        cutEnd(staffIdx);
}

/* 02541664 */
void dEvDtStaff_c::specialProcTimekeeper() {
    enum { ACT_COUNTDOWN, ACT_WAIT };
    u32 staffIdx = staffId("TIMEKEEPER");
    if (staffIdx == 0xFFFFFFFF) {
        assertion("d_event_data.cpp", 0x49Du, "0");
        return;
    }
    u32 actIdx = actionId(staffIdx, timekeeperActions, 2);
    if (isAdvance(staffIdx) && actIdx == ACT_COUNTDOWN) {
        EventValue* p = getSubstance(staffIdx, "Timer", dEvDtData_c::TYPE_INT);
        if (!p)
            assertion("d_event_data.cpp", 0x4AEu, "idata");
        mTimer = (u16)p->integer;
    }
    if (actIdx == ACT_COUNTDOWN) {
        s16 timer = (s16)mTimer;
        if (timer > 0)
            mTimer = (u16)(timer - 1);
        else
            cutEnd(staffIdx);
    } else if (actIdx == ACT_WAIT)
        cutEnd(staffIdx);
    else if (actIdx == 0xFFFFFFFF) {
        assertion("d_event_data.cpp", 0x4CBu, "0");
        cutEnd(staffIdx);
    }
}

/* 025422AC */
void dEvDtStaff_c::specialProcLight() {
    // HD: retain fused time accumulation and double-precision day wrapping.
    enum { ACT_WAIT, ACT_CHANGE, ACT_ADD_TIME };
    u32 staffIdx = staffId("LIGHT");
    if (staffIdx == 0xFFFFFFFF) {
        assertion("d_event_data.cpp", 0x191u, "0");
        return;
    }
    u32 actIdx = actionId(staffIdx, lightActions, 3);
    if (isAdvance(staffIdx)) {
        f32 multiplier = hourToAngle;
        if (actIdx == ACT_CHANGE) {
            EventValue* p = getSubstance(staffIdx, "Hour", dEvDtData_c::TYPE_FLOAT);
            if (p)
                dKy_instant_timechg(gabi::fmuls_ppc(multiplier, p->scalar));
            p = getSubstance(staffIdx, "Weather", dEvDtData_c::TYPE_INT);
            if (p && p->integer == 0) {
                dKy_instant_rainchg();
                cutEnd(staffIdx);
                return;
            }
        } else if (actIdx == ACT_ADD_TIME) {
            EventValue* p = getSubstance(staffIdx, "Hour", dEvDtData_c::TYPE_FLOAT);
            if (p) {
                f32 hour = p->scalar, speed = angleToHour, current = dComIfGs_getTime();
                f32 value = gabi::fmadds(current, speed, hour);
                f64 day = hoursPerDay;
                while (!(value < day))
                    value = (f32)((f64)value - day);
                dKy_instant_timechg(gabi::fmuls_ppc(multiplier, value));
            }
        }
    }
    cutEnd(staffIdx);
}

/* 02541828 */
void dEvDtStaff_c::specialProcCreate() {
    enum { ACT_WAIT, ACT_CREATE };
    u32 staffIdx = staffId("CREATER");
    if (staffIdx == 0xFFFFFFFF) {
        assertion("d_event_data.cpp", 0x2F3u, "0");
        return;
    }
    u32 actIdx = actionId(staffIdx, createActions, 2);
    if (!isAdvance(staffIdx) || actIdx != ACT_CREATE)
        return;
    EventValue* name = getSubstance(staffIdx, "MAKECAST", dEvDtData_c::TYPE_STRING);
    if (!name)
        assertion("d_event_data.cpp", 0x303u, "name");
    EventObjectName* info = dStage_searchName(name);
    if (!info)
        assertion("d_event_data.cpp", 0x305u, "objectName");
    EventValue* p = getSubstance(staffIdx, "ARG", dEvDtData_c::TYPE_INT);
    u32 params = p ? p->integer : 0xFFFFFFFF;
    cXyz pos;
    cXyz scale;
    csXyz angle;
    p = getSubstance(staffIdx, "POS", dEvDtData_c::TYPE_VEC);
    if (p)
        pos.copy(*(cXyz*)p);
    else
        pos.copy(dComIfGp_getPlayer(0)->current.pos);
    p = getSubstance(staffIdx, "ANGLE", dEvDtData_c::TYPE_INT);
    angle.x = p ? p[0].halves.low : 0;
    angle.y = p ? p[1].halves.low : 0;
    angle.z = p ? p[2].halves.low : 0;
    p = getSubstance(staffIdx, "SCALE", dEvDtData_c::TYPE_VEC);
    if (p)
        scale.copy(*(cXyz*)p);
    else {
        scale.x = one;
        scale.y = one;
        scale.z = one;
    }
    s32 room = currentRoom, layer = info->argument, profile = info->procname;
    createActor(profile, params, &pos, room, &angle, &scale, layer, 0u);
}

/* 02541B40 */
void dEvDtStaff_c::specialProcSound() {
    enum {
        ACT_WAIT,
        ACT_STRM_PLAY,
        ACT_STRM_DEMO_MJ_SISTER,
        ACT_STRM_DEMO_GO_MAJU,
        ACT_NOMSG_WAIT,
        ACT_RIDDLE,
        ACT_LANDING_DEMO,
        ACT_BGMSTOP
    };
    u32 staffIdx = staffId("SOUND");
    if (staffIdx == 0xFFFFFFFF) {
        assertion("d_event_data.cpp", 0x274u, "0");
        return;
    }
    u32 actIdx = actionId(staffIdx, soundActions, 8);
    if (isAdvance(staffIdx)) {
        switch (actIdx) {
        case ACT_WAIT:
        case ACT_NOMSG_WAIT:
            specialProc_WaitStart(staffIdx);
            break;
        case ACT_STRM_PLAY:
            mDoAud_bgmStreamPlay();
            break;
        case ACT_STRM_DEMO_MJ_SISTER:
            mDoAud_bgmStreamPrepare(0xC0000004u);
            break;
        case ACT_STRM_DEMO_GO_MAJU:
            mDoAud_bgmStreamPrepare(0xC0000005u);
            break;
        case ACT_RIDDLE:
            mDoAud_seStart(0x806u);
            break;
        case ACT_LANDING_DEMO: {
            EventValue* p = getSubstance(staffIdx, "PREPARE", dEvDtData_c::TYPE_INT);
            if (p) {
                u32 choice = p->integer;
                if ((s32)choice > 6) {
                    assertion("d_event_data.cpp", 0x2B3u, "0");
                    choice = p->integer;
                }
                mDoAud_prepareLandingDemo(landing_table[choice]);
            }
            p = getSubstance(staffIdx, "START", dEvDtData_c::TYPE_INT);
            if (p)
                mDoAud_startLandingDemo(p);
            p = getSubstance(staffIdx, "END", dEvDtData_c::TYPE_INT);
            if (p)
                mDoAud_endLandingDemo(p);
            break;
        }
        case ACT_BGMSTOP: {
            EventValue* p = getSubstance(staffIdx, "Timer", dEvDtData_c::TYPE_INT);
            if (p)
                mDoAud_bgmStop(p->integer);
            break;
        }
        default:
            break;
        }
    }
    if (actIdx == ACT_WAIT) {
        specialProc_WaitProc(staffIdx);
        return;
    }
    if (actIdx != ACT_NOMSG_WAIT || (u32)(s32)(s16)mTimer <= (u32)demoState.frameNoMsg)
        cutEnd(staffIdx);
}

/* 02540FB8 */
void dEvDtStaff_c::specialProcDirector() {
    enum {
        ACT_WAIT,
        ACT_NEXT,
        ACT_FADE,
        ACT_BGM_START,
        ACT_VIBRATION,
        ACT_SE_START,
        ACT_WIPE,
        ACT_PLAYER_DRAW,
        ACT_PLAYER_NODRAW
    };
    daPy_py_c* player = (daPy_py_c*)dComIfGp_getLinkPlayer();
    u32 staffIdx = staffId("DIRECTOR");
    if (staffIdx == 0xFFFFFFFF) {
        assertion("d_event_data.cpp", 0x350u, "0");
        return;
    }
    u32 actIdx = actionId(staffIdx, directorActions, 9);
    u32 advanceFlag = isAdvance(staffIdx);

    if (advanceFlag) {
        switch (actIdx) {
        case ACT_WAIT:
            specialProc_WaitStart(staffIdx);
            break;
        case ACT_NEXT:
            dEvDt_Next_Stage(staffIdx, 0u);
            break;
        case ACT_FADE: {
            EventValue* rate = getSubstance(staffIdx, "Rate", dEvDtData_c::TYPE_FLOAT);
            EventValue* col = getSubstance(staffIdx, "Color", dEvDtData_c::TYPE_INT);
            if (!rate)
                assertion("d_event_data.cpp", 0x363u, "rate");
            if (rate->scalar > zero)
                fadeState.rate = zero;
            if (col) {
                GXColor c;
                c.r = (u8)(u32)col[0].integer;
                c.g = (u8)(u32)col[1].integer;
                c.b = (u8)(u32)col[2].integer;
                c.a = (u8)(u32)col[3].integer;
                fadeOutColor(&c, rate->scalar);
            } else
                fadeOut(rate->scalar);
            break;
        }
        case ACT_BGM_START: {
            EventValue* p = getSubstance(staffIdx, "BGM_ID", dEvDtData_c::TYPE_INT);
            u32 i = p ? p->integer : 0;
            mDoAud_bgmStart(bgm_table[i]);
            break;
        }
        case ACT_VIBRATION: {
            specialProc_WaitStart(staffIdx);
            if (!(s16)mTimer)
                assertion("d_event_data.cpp", 0x38Bu, "0");
            EventValue* strength = getSubstance(staffIdx, "Pattern", dEvDtData_c::TYPE_INT);
            EventValue* type = getSubstance(staffIdx, "Type", dEvDtData_c::TYPE_INT);
            if (!strength || !type)
                assertion("d_event_data.cpp", 0x38Fu, "0");
            dVibration_c* vibration = dComIfGp_getVibration();
            cXyz dir;
            dir.z = zero;
            dir.y = one;
            dir.x = zero;
            startQuake(vibration, strength, 0u, type->integer, &dir);
            break;
        }
        case ACT_SE_START: {
            EventValue* p = getSubstance(staffIdx, "SENUM", dEvDtData_c::TYPE_INT);
            if (p && (s32)p->integer <= 0)
                mDoAud_seStart(se_table[0]);
            break;
        }
        case ACT_WIPE: {
            EventValue* rate = getSubstance(staffIdx, "Rate", dEvDtData_c::TYPE_FLOAT);
            if (!rate)
                assertion("d_event_data.cpp", 0x3A4u, "rate");
            wipeOut(-rate->scalar);
            mWipeDirection = rate->scalar > zero ? 0 : 1;
            EventValue* p = getSubstance(staffIdx, "SOUND", dEvDtData_c::TYPE_INT);
            if (p && !p->integer)
                mDoAud_seStart(0x8DAu);
            break;
        }
        case ACT_PLAYER_DRAW:
            player->mNoResetFlg0 = (u32)player->mNoResetFlg0 & ~daPyFlg0_NO_DRAW;
            break;
        case ACT_PLAYER_NODRAW:
            player->mNoResetFlg0 = (u32)player->mNoResetFlg0 | daPyFlg0_NO_DRAW;
            break;
        default:
            break;
        }
    }
    if (actIdx == ACT_WAIT) {
        specialProc_WaitProc(staffIdx);
        return;
    }
    if (actIdx == ACT_NEXT)
        return;
    if (actIdx == ACT_FADE) {
        if (!fadeState.enabled) {
            cutEnd(staffIdx);
            return;
        }
        if ((f32)fadeState.rate < one)
            return;
        EventValue* rate = getSubstance(staffIdx, "Rate", dEvDtData_c::TYPE_FLOAT);
        if (!rate)
            assertion("d_event_data.cpp", 0x3D8u, "rate");
        if (rate->scalar > zero)
            cutEnd(staffIdx);
        return;
    }
    if (actIdx == ACT_VIBRATION) {
        s16 timer = (s16)mTimer;
        if (timer > 0) {
            timer = (s16)(timer - 1);
            mTimer = (u16)timer;
            if (!timer)
                stopQuake(dComIfGp_getVibration(), 0xFFFFFFFFu);
        } else
            cutEnd(staffIdx);
        return;
    }
    if (actIdx == ACT_WIPE) {
        f32 fade = (f32)wipeRate;
        if ((s16)mWipeDirection) {
            if (fade == zero)
                cutEnd(staffIdx);
        } else if (!(fade < one))
            cutEnd(staffIdx);
        return;
    }
    cutEnd(staffIdx);
}

/* 02541DF8 */
void dEvDtStaff_c::specialProcMessage() {
    // HD: message creation/status are handled by the native manager rather than msg_class.
    enum {
        ACT_WAIT,
        ACT_CREATE_MSG,
        ACT_PUSHBUTTON,
        ACT_FINISH,
        ACT_CONTINUE,
        ACT_END,
        ACT_DELETE,
        ACT_TELOP_ON,
        ACT_TELOP_OFF
    };
    void* manager = messageManager.get();
    u32 staffIdx = staffId("MESSAGE");
    if (staffIdx == 0xFFFFFFFF) {
        assertion("d_event_data.cpp", 0x1EAu, "0");
        return;
    }
    u32 actIdx = actionId(staffIdx, messageActions, 9);
    if (isAdvance(staffIdx)) {
        switch (actIdx) {
        case ACT_WAIT:
            specialProc_WaitStart(staffIdx);
            break;
        case ACT_CREATE_MSG: {
            messageState.l_msgId = 0xFFFFFFFF;
            EventValue* p = getSubstance(staffIdx, "msgNo", dEvDtData_c::TYPE_INT);
            if (!p)
                assertion("d_event_data.cpp", 0x1F9u, "idata");
            messageState.l_msgNo = p->integer;
            mWipeDirection = 0;
            break;
        }
        case ACT_FINISH:
        case ACT_END:
            messageSetStatus(manager, 16u);
            break;
        case ACT_CONTINUE: {
            messageSetStatus(manager, 15u);
            EventValue* p = getSubstance(staffIdx, "msgNo", dEvDtData_c::TYPE_INT);
            if (!p)
                assertion("d_event_data.cpp", 0x204u, "idata");
            u32 id = p->integer;
            messageState.l_msgNo = id;
            messageSet(manager, id, 0u);
            break;
        }
        case ACT_TELOP_ON: {
            EventValue* p = getSubstance(staffIdx, "tlpNo", dEvDtData_c::TYPE_INT);
            if (!p)
                assertion("d_event_data.cpp", 0x20Au, "idata");
            u8 choice = p->bytes[3];
            dComIfG_inf_c* game = gameInfo();
            game->play.mPlacenameIndex = choice;
            game->play.mPlacenameState = 2;
            specialProc_WaitStart(staffIdx);
            break;
        }
        case ACT_TELOP_OFF:
            gameInfo()->play.mPlacenameState = 1;
            break;
        default:
            break;
        }
    }
    switch (actIdx) {
    case ACT_WAIT:
    case ACT_TELOP_ON:
        specialProc_WaitProc(staffIdx);
        return;
    case ACT_CREATE_MSG: {
        u32 phase = (u32)(s32)(s16)mWipeDirection;
        if (phase == 0) {
            u32 id = messageSet(manager, (u32)messageState.l_msgNo, 0u);
            messageState.l_msgId = id;
            if (id == 0xFFFFFFFF)
                return;
            phase = (u32)(s32)(s16)mWipeDirection;
        } else if (phase == 2) {
            cutEnd(staffIdx);
            return;
        } else if (phase != 1)
            return;
        mWipeDirection = (u16)(phase + 1);
        return;
    }
    case ACT_PUSHBUTTON:
        if (messageGetStatus(manager) == 14)
            cutEnd(staffIdx);
        return;
    case ACT_FINISH:
    case ACT_DELETE:
        if ((u32)messageState.l_msgId == 0xFFFFFFFF) {
            cutEnd(staffIdx);
            return;
        }
        if (messageGetStatus(manager) == 18) {
            messageSetStatus(manager, 19u);
            messageState.l_msgId = 0xFFFFFFFF;
            cutEnd(staffIdx);
        }
        return;
    case ACT_CONTINUE:
        cutEnd(staffIdx);
        return;
    case ACT_END: {
        u32 state = messageGetStatus(manager);
        if (state >= 17 && state <= 18)
            cutEnd(staffIdx);
        return;
    }
    case ACT_TELOP_OFF:
        if (!gameInfo()->play.mPlacenameState)
            cutEnd(staffIdx);
        return;
    default:
        cutEnd(staffIdx);
        return;
    }
}

/* WWHD glue (tools/verify/autoglue.py)
data packageActions 101D62C4
data timekeeperActions 101D6304
data lightActions 101D6374
data createActions 101D630C
data soundActions 101D6314
data directorActions 101D62D4
data messageActions 101D6350
data landing_table 101D6334
data bgm_table 101D62F8
data se_table 101D62D0
str 1004D908 in 02540778
str 1004D8F4 in 02540778
str 1004D91C in 02540A48
str 1004D888 in 02540508
str 1004D8D4 in 02540508
str 1004D8A4 in 02540508
str 1004D890 in 02540508
str 1004D8AC in 02540508
str 1004D8B4 in 02540508
str 1004D8BC in 02540508
str 1004D8E0 in 02540508
str 1004D8A0 in 02540508
str 1004D8C4 in 02540508
str 1004D8CC in 02540508
str 1004D898 in 02540508
str 1004D944 in 02540B0C
str 1004D994 in 02540B0C
str 1004D94C in 02540B0C
str 1004D9A8 in 02540B0C
str 1004D950 in 02540B0C
str 1004D970 in 02540B0C
str 1004D9B4 in 02540B0C
str 1004D958 in 02540B0C
str 1004D97C in 02540B0C
str 1004D988 in 02540B0C
str 1004D968 in 02540B0C
str 1004D960 in 02540B0C
str 1004DAB4 in 02541664
str 1004DAC0 in 02541664
str 1004DAA0 in 02541664
str 1004DAA4 in 02541664
str 1004DAAC in 02541664
str 1004DC98 in 025422AC
str 1004DCAC in 025422AC
str 1004DCA0 in 025422AC
str 1004DCA4 in 025422AC
str 1004DC90 in 025422AC
str 1004DAE8 in 02541828
str 1004DB14 in 02541828
str 1004DAF8 in 02541828
str 1004DB28 in 02541828
str 1004DB0C in 02541828
str 1004DB34 in 02541828
str 1004DAF0 in 02541828
str 1004DAF4 in 02541828
str 1004DAFC in 02541828
str 1004DB04 in 02541828
str 1004DB5C in 02541B40
str 1004DB78 in 02541B40
str 1004DB64 in 02541B40
str 1004DB50 in 02541B40
str 1004DB68 in 02541B40
str 1004DB58 in 02541B40
str 1004DB70 in 02541B40
str 1004DA20 in 02540FB8
str 1004DA2C in 02540FB8
str 1004D9E4 in 02540FB8
str 1004DA00 in 02540FB8
str 1004D9E8 in 02540FB8
str 1004DA08 in 02540FB8
str 1004DA10 in 02540FB8
str 1004D9DC in 02540FB8
str 1004DA18 in 02540FB8
str 1004D9F0 in 02540FB8
str 1004D9F8 in 02540FB8
str 1004DBF4 in 02541DF8
str 1004DC18 in 02541DF8
str 1004DBFC in 02541DF8
str 1004DC00 in 02541DF8
str 1004DC08 in 02541DF8
str 1004DC10 in 02541DF8
data zero 1004D884
data one 1004D9D8
data hourToAngle 1004D880
data angleToHour 1004DC84
data eventCullRate 1004D940
data piMin 1004DCE0
data piMax 1004DCE4
ext 0273AA24 void assertion(const char*,u32,const char*)
ext 028F026C void registerGlobalObject(void*)
ext 028ED6F8 void initializeHeaderByte(void*)
ext 028EAB2C void initializeHeaderFlag(void*)
ext 02560728 void dKy_set_nexttime(f32)
ext 0252012C void dComIfGp_setNextStage(const char*,s32,s32,s32,u32,u32,s32,f32)
ext 02606900 void* getObjectRes(void*,EventSafeString*,EventSafeString*)
ext 0252447C void* dComIfG_getStageRes(const char*,EventValue*)
ext 02528CEC void dComIfGp_demo_create(void*,EventValue*,f32)
ext 02528AC4 void dComIfGp_demo_remove()
ext 0256076C void dKy_instant_timechg(f32)
ext 025607E0 void dKy_instant_rainchg()
ext 025C109C EventObjectName* dStage_searchName(EventValue*)
ext 025D5834 void createActor(s32,u32,cXyz*,s32,csXyz*,cXyz*,s32,u32)
ext 025E1944 void mDoAud_bgmStreamPlay()
ext 025E1934 void mDoAud_bgmStreamPrepare(u32)
ext 025E1988 void mDoAud_seStart(u32)
ext 025E1FE4 void mDoAud_prepareLandingDemo(u32)
ext 025E1FF4 void mDoAud_startLandingDemo(EventValue*)
ext 025E2000 void mDoAud_endLandingDemo(EventValue*)
ext 025E1904 void mDoAud_bgmStop(u32)
ext 025F0658 void fadeOutColor(GXColor*,f32)
ext 025F069C void fadeOut(f32)
ext 025E18EC void mDoAud_bgmStart(u32)
ext 025CB4E0 void startQuake(dVibration_c*,EventValue*,u32,u32,cXyz*)
ext 025CB610 void stopQuake(dVibration_c*,u32)
ext 0252F590 void wipeOut(f32)
ext 025F74D0 void messageSetStatus(void*,u32)
ext 025F7DB0 u32 messageSet(void*,u32,u32)
ext 025F795C u32 messageGetStatus(void*)
*/
