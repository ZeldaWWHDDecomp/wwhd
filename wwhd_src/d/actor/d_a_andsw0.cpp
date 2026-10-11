/**
 * d_a_andsw0.cpp (WWHD)
 * Switch logic: AND of several switches, timers, and the Forest Haven opening (hajimari) check.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_andsw0.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_andsw0.h"

const gc_tu_data gc_tu = {.headerStatics = {0x10461418, 0x1018FCC4, 0x10461424}};

// HD: mutable BSS search state; the registration pass explicitly clears all seven slots.
static AndswSearchActors ac;
static s32 check_count;

/* 02050FF0 */
static BOOL daAndsw0_Draw(andsw0_class*) {
    return TRUE;
}

/* inlined into daAndsw0_Execute */
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
    /* HD: the inline fopAcM_GetName checks for NULL */
    if (fopAcM_IsActor(i_this) && i_this != nullptr && fopAcM_GetName(i_this) == fpcNm_BK_e &&
        (fopAcM_GetParam((fopAc_ac_c*)i_this) & 0xF) == 7) {
        s32 count = check_count;
        if (count < 2) {
            ac[count] = i_this;
            check_count = count + 1;
        }
        return 0;
    }
    return 0;
}

/* 02051078 */
static void* bk_s_sub2(void* i_this, void*) {
    if (fopAcM_IsActor(i_this) && i_this != nullptr && fopAcM_GetName(i_this) == fpcNm_BK_e &&
        (fopAcM_GetParam((fopAc_ac_c*)i_this) & 0xF) == 4) {
        s32 count = check_count;
        if (count == 2) {
            ac[count] = i_this;
            check_count = count + 1;
        }
        return 0;
    }
    return 0;
}

/* 020510F4 */
static void* bk_s_sub3(void* i_this, void*) {
    if (fopAcM_IsActor(i_this) && i_this != nullptr && fopAcM_GetName(i_this) == fpcNm_BK_e &&
        (fopAcM_GetParam((fopAc_ac_c*)i_this) & 0xF) == 5) {
        s32 count = check_count;
        if (count < 5) {
            ac[count] = i_this;
            check_count = count + 1;
        }
        return 0;
    }
    return 0;
}

/* 02051174 */
static void* bb_s_sub(void* search, void*) {
    if (fopAcM_IsActor(search) && search != nullptr && fopAcM_GetName(search) == fpcNm_BB_e) {
        s32 count = check_count;
        if (count < 7) {
            ac[count] = search;
            check_count = count + 1;
        }
        return 0;
    }
    return 0;
}

/* inlined into daAndsw0_Execute */
static s32 hajimari_actor_entry(andsw0_class* i_this) {
    for (int i = 0; i < 7; i++) {
        ac[i] = nullptr;
    }
    check_count = 0;
    fpcM_Search(&bk_s_sub1, i_this);
    fpcM_Search(&bk_s_sub2, i_this);
    fpcM_Search(&bk_s_sub3, i_this);
    check_count = 5;
    fpcM_Search(&bb_s_sub, i_this);
    for (int i = 0; i < 7; i++) {
        if (ac[i] == nullptr) {
            return 0;
        }
    }
    return 1;
}

/* inlined into daAndsw0_Execute */
static void hajimarinomori_check(andsw0_class* i_this) {
    fopAc_ac_c* actor = i_this;
    if (i_this->mBehaviorType == 0) {
        if (hajimari_actor_entry(i_this)) {
            i_this->mBehaviorType = 1;
        }
    } else {
        if (dComIfGs_isEventBit(dSv_event_flag_c::UNK_0004)) {
            fopAcM_delete((fopAc_ac_c*)ac[5].get());
            fopAcM_delete((fopAc_ac_c*)ac[6].get());

            ((bk_class*)ac[3].get())->m121C = 1;
            ((bk_class*)ac[4].get())->m121C = 1;

            if (dComIfGs_isEventBit(dSv_event_flag_c::UNK_0301)) {
                ((bk_class*)ac[0].get())->m121C = 1;
            }
            if (dComIfGs_isEventBit(dSv_event_flag_c::UNK_0480)) {
                ((bk_class*)ac[1].get())->m121C = 1;
            }
            if (dComIfGs_isEventBit(dSv_event_flag_c::UNK_0301) && dComIfGs_isEventBit(dSv_event_flag_c::UNK_0480)) {
                ((bk_class*)ac[2].get())->m121C = 1;
            }
        } else {
            ((bk_class*)ac[0].get())->m121C = 1;
            ((bk_class*)ac[1].get())->m121C = 1;
        }

        fopAcM_delete(actor);
    }
}

/* inlined into daAndsw0_Execute */
static void event_start_check(andsw0_class* i_this) {
    fopAc_ac_c* actor = i_this;
    switch (i_this->mEventState) {
    case 0:
        if (i_this->mEventIdx != -1 && fopAcM_isSwitch(actor, i_this->mSwitchToSet)) {
            if (actor->eventInfo.checkCommandDemoAccrpt()) {
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
    event_start_check(i_this);
    u8 numToCheck = i_this->mNumSwitchesToCheck;
    if (numToCheck == 0xFF)
        hajimarinomori_check(i_this);
    else
        daAndsw0_check(i_this, numToCheck);
    return TRUE;
}

/* 020517F0 */
static BOOL daAndsw0_IsDelete(andsw0_class*) {
    return TRUE;
}

/* 020517F8 */
// HD: fopAcM_RegisterDeleteID is empty in this build.
static BOOL daAndsw0_Delete(andsw0_class* i_this) {
    return TRUE;
}

/* 02051800 */
static cPhs_State daAndsw0_Create(fopAc_ac_c* a) {
    /* fopAcM_RegisterCreateID: empty; the unused dComIfGp_getPlayer(0) leaves its accessor call */
    dComIfGp_get();
    fopAcM_ct(a, andsw0_class);
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

// HD: compiler-generated common-header initializer and deleting destructor.
/* 02051904 */
static void __sinit_d_a_andsw0_cpp() {
    __sinit_header_statics();
}

/* 02051998 */
static void andsw0_class_dt(andsw0_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue (tools/verify/autoglue.py):
data ac 10461434
data check_count 10461414
ext 025DE508 void* fpcM_Search(AndswSearchFunc, void*)
*/
