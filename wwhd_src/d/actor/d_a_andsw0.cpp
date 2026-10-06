/**
 * d_a_andsw0.cpp (WWHD)
 * Switch logic: AND of several switches, timers, and the Forest Haven opening (hajimari) check.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_andsw0.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define ANDSW0_VTBL 0x100071AC /* HD: andsw0_class vtable */

enum {
    ACT_ON_ALL = 0,
    ACT_OFF_ALL,
    ACT_WAIT = 10,
    ACT_TIMER = 20,
    ACT_TIMER2,
    ACT_TIMER_SET = 30,
};

enum {
    fpcNm_BB_e = 0xB5,
    fpcNm_BK_e = 0xBD,
};

struct andsw0_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhase; /* unused */
    /* 0x3B4 */ be<s8> mAction;
    /* 0x3B5 */ be<u8> mNumSwitchesToCheck;
    /* 0x3B6 */ be<u8> mBehaviorType;
    /* 0x3B7 */ be<u8> mSwitchToSet;
    /* 0x3B8 */ be<u8> mFirstSwitchToCheck;
    /* 0x3B9 */ u8 _3B9;
    /* 0x3BA */ be<u16> mTimer;
    /* 0x3BC */ be<s16> mEventIdx;
    /* 0x3BE */ be<u8> mEventNo;
    /* 0x3BF */ be<s8> mEventState;
};
WWHD_OFFSET(andsw0_class, mAction, 0x3B4);
WWHD_OFFSET(andsw0_class, mEventState, 0x3BF);

/* bk_class::m121C (HD +0x137C), only the field written here */
static inline void bk_set_m121C(void* bk, u8 v) { gabi::store<u8>(gabi::ea(bk) + 0x137C, v); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* dComIfGs_isEventBit(flag): dSv_event_c at *(0x101F84DC) + 0x644 (HD) */
static inline BOOL dComIfGs_isEventBit(u16 flag) {
    return dSv_event_isEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), flag);
}
/* fopAcM_onSwitch / offSwitch(a, sw): home room */
static inline void fopAcM_onSwitch(fopAc_ac_c* a, s32 sw) { dComIfGs_onSwitch(sw, a->home.roomNo); }
static inline void fopAcM_offSwitch(fopAc_ac_c* a, s32 sw) { dComIfGs_offSwitch(sw, a->home.roomNo); }

/* static void* ac[7]; static s32 check_count; */
static gptr<void>* ac() { return gabi::at<gptr<void>>(0x10461434); }
static be<s32>& check_count() { return *gabi::at<be<s32>>(0x10461414); }

/* 02050FF0 */
static BOOL daAndsw0_Draw(andsw0_class*) {
    WWHD_FUNC(0x02050FF0, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02050FF0, daAndsw0_Draw);

/* daAndsw0_check, inlined in daAndsw0_Execute */
static void daAndsw0_check(andsw0_class* i_this, s32 numToCheck) {
    fopAc_ac_c* actor = i_this;
    u32 switchToCheck;

    if (i_this->mFirstSwitchToCheck) {
        switchToCheck = i_this->mFirstSwitchToCheck;
    } else {
        switchToCheck = i_this->mSwitchToSet + 1;
    }

    switch (i_this->mAction) {
    case ACT_ON_ALL:
        for (int i = 0; i < numToCheck; i++) {
            if (dComIfGs_isSwitch(switchToCheck, fopAcM_GetRoomNo(actor)) == false) {
                break;
            }
            if (i == numToCheck - 1) {
                if (i_this->mBehaviorType != 3) {
                    dComIfGs_onSwitch(i_this->mSwitchToSet, fopAcM_GetRoomNo(actor));
                }
                switch (i_this->mBehaviorType) {
                case 0:
                    i_this->mAction = ACT_WAIT;
                    break;
                case 3:
                    i_this->mAction = ACT_TIMER_SET;
                    i_this->mTimer = 65;
                    break;
                default:
                    i_this->mAction = ACT_OFF_ALL;
                    break;
                }
            }
            switchToCheck += 1;
        }
        break;
    case ACT_OFF_ALL: {
        u32 switchToCheck2 = i_this->mFirstSwitchToCheck ? (u32)i_this->mFirstSwitchToCheck : i_this->mSwitchToSet + 1;
        for (int i = 0; i < numToCheck; i++) {
            if (dComIfGs_isSwitch(switchToCheck2, fopAcM_GetRoomNo(actor)) == false) {
                dComIfGs_offSwitch(i_this->mSwitchToSet, fopAcM_GetRoomNo(actor));
                i_this->mAction = ACT_ON_ALL;
                break;
            }
            switchToCheck2 += 1;
        }
        break;
    }
    case ACT_TIMER:
        i_this->mTimer = (actor->home.angle.z & 0xFF) * 15;
        if (fopAcM_isSwitch(actor, i_this->mSwitchToSet)) {
            i_this->mAction = ACT_WAIT;
        } else {
            for (int i = 0; i < numToCheck; i++) {
                if (fopAcM_isSwitch(actor, switchToCheck)) {
                    i_this->mAction = i_this->mAction + 1;
                    break;
                }
                switchToCheck += 1;
            }
        }
        break;
    case ACT_TIMER2:
        i_this->mTimer = i_this->mTimer - 1;
        if (i_this->mTimer == 0) {
            for (int i = 0; i < numToCheck; i++) {
                fopAcM_offSwitch(actor, switchToCheck);
                switchToCheck += 1;
            }
            i_this->mAction = ACT_TIMER;
        } else {
            switchToCheck = i_this->mFirstSwitchToCheck ? (u32)i_this->mFirstSwitchToCheck : i_this->mSwitchToSet + 1;
            for (int i = 0; i < numToCheck; i++) {
                if (fopAcM_isSwitch(actor, switchToCheck) == false) {
                    break;
                }
                if (i == numToCheck - 1) {
                    fopAcM_onSwitch(actor, i_this->mSwitchToSet);
                    i_this->mAction = ACT_WAIT;
                }
                switchToCheck += 1;
            }
        }
        break;
    case ACT_TIMER_SET:
        i_this->mTimer = i_this->mTimer - 1;
        if (i_this->mTimer == 0) {
            fopAcM_onSwitch(actor, i_this->mSwitchToSet);
            i_this->mAction = ACT_WAIT;
        }
        break;
    case ACT_WAIT:
    default:
        break;
    }
}

/* 02050FF8 */
static void* bk_s_sub1(void* i_this, void*) {
    WWHD_FUNC(0x02050FF8, void*, i_this, (u32)0);
    /* HD: the inline fopAcM_GetName checks for NULL */
    if (fopAc_IsActor(i_this) && i_this != nullptr && fpcM_GetName(i_this) == fpcNm_BK_e &&
        (fopAcM_GetParam((fopAc_ac_c*)i_this) & 0xF) == 7) {
        s32 count = check_count();
        if (count < 2) {
            ac()[count] = i_this;
            check_count() = count + 1;
        }
        return 0;
    }
    return 0;
}
VERIFY(0x02050FF8, bk_s_sub1);

/* 02051078 */
static void* bk_s_sub2(void* i_this, void*) {
    WWHD_FUNC(0x02051078, void*, i_this, (u32)0);
    if (fopAc_IsActor(i_this) && i_this != nullptr && fpcM_GetName(i_this) == fpcNm_BK_e &&
        (fopAcM_GetParam((fopAc_ac_c*)i_this) & 0xF) == 4) {
        s32 count = check_count();
        if (count == 2) {
            ac()[count] = i_this;
            check_count() = count + 1;
        }
        return 0;
    }
    return 0;
}
VERIFY(0x02051078, bk_s_sub2);

/* 020510F4 */
static void* bk_s_sub3(void* i_this, void*) {
    WWHD_FUNC(0x020510F4, void*, i_this, (u32)0);
    if (fopAc_IsActor(i_this) && i_this != nullptr && fpcM_GetName(i_this) == fpcNm_BK_e &&
        (fopAcM_GetParam((fopAc_ac_c*)i_this) & 0xF) == 5) {
        s32 count = check_count();
        if (count < 5) {
            ac()[count] = i_this;
            check_count() = count + 1;
        }
        return 0;
    }
    return 0;
}
VERIFY(0x020510F4, bk_s_sub3);

/* 02051174 */
static void* bb_s_sub(void* search, void*) {
    WWHD_FUNC(0x02051174, void*, search, (u32)0);
    if (fopAc_IsActor(search) && search != nullptr && fpcM_GetName(search) == fpcNm_BB_e) {
        s32 count = check_count();
        if (count < 7) {
            ac()[count] = search;
            check_count() = count + 1;
        }
        return 0;
    }
    return 0;
}
VERIFY(0x02051174, bb_s_sub);

/* hajimari_actor_entry, inlined */
static s32 hajimari_actor_entry(andsw0_class* i_this) {
    for (int i = 0; i < 7; i++) {
        ac()[i] = nullptr;
    }
    check_count() = 0;
    fpcM_Search(0x02050FF8 /* bk_s_sub1 */, i_this);
    fpcM_Search(0x02051078 /* bk_s_sub2 */, i_this);
    fpcM_Search(0x020510F4 /* bk_s_sub3 */, i_this);
    check_count() = 5;
    fpcM_Search(0x02051174 /* bb_s_sub */, i_this);
    for (int i = 0; i < 7; i++) {
        if (ac()[i] == nullptr) {
            return 0;
        }
    }
    return 1;
}

/* hajimarinomori_check, inlined */
static void hajimarinomori_check(andsw0_class* i_this) {
    fopAc_ac_c* actor = i_this;
    if (i_this->mBehaviorType == 0) {
        if (hajimari_actor_entry(i_this)) {
            i_this->mBehaviorType = 1;
        }
    } else {
        if (dComIfGs_isEventBit(0x0004)) {
            fopAcM_delete((fopAc_ac_c*)ac()[5].get());
            fopAcM_delete((fopAc_ac_c*)ac()[6].get());

            bk_set_m121C(ac()[3], 1);
            bk_set_m121C(ac()[4], 1);

            if (dComIfGs_isEventBit(0x0301)) {
                bk_set_m121C(ac()[0], 1);
            }
            if (dComIfGs_isEventBit(0x0480)) {
                bk_set_m121C(ac()[1], 1);
            }
            if (dComIfGs_isEventBit(0x0301) && dComIfGs_isEventBit(0x0480)) {
                bk_set_m121C(ac()[2], 1);
            }
        } else {
            bk_set_m121C(ac()[0], 1);
            bk_set_m121C(ac()[1], 1);
        }

        fopAcM_delete(actor);
    }
}

/* event_start_check, inlined */
static void event_start_check(andsw0_class* i_this) {
    fopAc_ac_c* actor = i_this;
    switch (i_this->mEventState) {
    case 0:
        if (i_this->mEventIdx != -1 && fopAcM_isSwitch(actor, i_this->mSwitchToSet)) {
            if (eventInfo_checkCommandDemoAccrpt(actor)) {
                i_this->mEventState = i_this->mEventState + 1;
            } else {
                fopAcM_orderOtherEventId(actor, i_this->mEventIdx, i_this->mEventNo, 0xFFFF, 0, 1);
            }
        }
        break;
    case 1:
        if (dComIfGp_evmng_endCheck(i_this->mEventIdx)) {
            dComIfGp_event_reset();
            i_this->mEventState = i_this->mEventState + 1;
        }
        break;
    case 2:
        break;
    }
}

/* 020511E4 */
static BOOL daAndsw0_Execute(andsw0_class* i_this) {
    WWHD_FUNC(0x020511E4, BOOL, i_this);
    event_start_check(i_this);
    u8 numToCheck = i_this->mNumSwitchesToCheck;
    if (numToCheck == 0xFF)
        hajimarinomori_check(i_this);
    else
        daAndsw0_check(i_this, numToCheck);
    return TRUE;
}
VERIFY(0x020511E4, daAndsw0_Execute);

/* 020517F0 */
static BOOL daAndsw0_IsDelete(andsw0_class*) {
    WWHD_FUNC(0x020517F0, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x020517F0, daAndsw0_IsDelete);

/* 020517F8: fopAcM_RegisterDeleteID is empty in this build */
static BOOL daAndsw0_Delete(andsw0_class* i_this) {
    WWHD_FUNC(0x020517F8, BOOL, i_this);
    return TRUE;
}
VERIFY(0x020517F8, daAndsw0_Delete);

/* 02051800 */
static cPhs_State daAndsw0_Create(fopAc_ac_c* a) {
    WWHD_FUNC(0x02051800, cPhs_State, a);
    /* fopAcM_RegisterCreateID: empty; the unused dComIfGp_getPlayer(0) leaves its accessor call */
    dComIfGp_get();
    /* fopAcM_ct(a, andsw0_class) */
    if (!fopAcM_CheckCondition(a, fopAcCnd_INIT_e)) {
        if (a != nullptr) {
            fopAc_ac_c_ct(a);
            a->__vtbl = ANDSW0_VTBL;
        }
        fopAcM_OnCondition(a, fopAcCnd_INIT_e);
    }
    andsw0_class* i_this = (andsw0_class*)a;
    i_this->mNumSwitchesToCheck = (fopAcM_GetParam(a) >> 0) & 0xFF;
    i_this->mBehaviorType = (fopAcM_GetParam(a) >> 8) & 0xFF;
    i_this->mSwitchToSet = (fopAcM_GetParam(a) >> 24) & 0xFF;
    i_this->mFirstSwitchToCheck = (fopAcM_GetParam(a) >> 16) & 0xFF;
    i_this->mTimer = (a->home.angle.z & 0xFF) * 15;
    u8 evNo = (u8)a->home.angle.x;
    i_this->mEventNo = evNo;
    i_this->mEventIdx = dComIfGp_evmng_getEventIdx(nullptr, evNo);
    if (i_this->mBehaviorType == 2)
        i_this->mAction = ACT_TIMER;
    if (i_this->mFirstSwitchToCheck == 0xFF)
        i_this->mFirstSwitchToCheck = 0;
    if (i_this->mNumSwitchesToCheck == 0xFF) {
        i_this->mBehaviorType = 0;
        i_this->mSwitchToSet = 0;
    }
    return cPhs_COMPLEATE_e;
}
VERIFY(0x02051800, daAndsw0_Create);

/* 02051904 */
static void __sinit_d_a_andsw0_cpp() {
    WWHD_FUNC(0x02051904, void, (u32)0);
    sinit_header_statics(0x10461418, 0x1018FCC4);
}
VERIFY(0x02051904, __sinit_d_a_andsw0_cpp);

/* 02051998: andsw0_class deleting destructor (vtable +0xC) */
static void andsw0_class_dt(andsw0_class* i_this, s32 flags) {
    WWHD_FUNC(0x02051998, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02051998, andsw0_class_dt);
