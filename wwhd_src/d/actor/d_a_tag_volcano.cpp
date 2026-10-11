/* HD Volcano tag reconstruction derived from zeldaret/tww. */
#include "d/actor/d_a_tag_volcano.h"
const gc_tu_data gc_tu = {.safeString = 0x1003FCA0};
enum { JA_SE_ISLE_TIMER_0 = 0x8B2, dIsleRoom_FireMountain_e = 20, dIsleRoom_IceRingIsle_e = 40 };
using daTagvolcano::Act_c;
/* 024B3970 */
u32 prmAbstract(Act_c *actor, u32 width, u32 shift) {
    u32 parameters = fopAcM_GetParam(actor);
    width &= 63;
    shift &= 63;
    u32 mask = (width < 32 ? 1u << width : 0u) - 1u;
    return (shift < 32 ? parameters >> shift : 0u) & mask;
}

// HD: each accessor call is retained; some original paths intentionally reuse a result.
static dComIfG_inf_c* play() { return reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get()); }
// HD: save storage has a 0x20-byte prefix before the shared save-info layout.
static dSv_info_c* saveInfo() { return gabi::at<dSv_info_c>(dComIfGs_ea() + 0x20); }
Act_c::Act_c() {
    fopAc_ac_c_ct(this);
    __vtbl = 0x1003FCB8;
}
static s32 room(Act_c *a) { return a->home.roomNo; }
static void offSwitch(Act_c *a) {
    u32 sw = prmAbstract(a, 8, 8);
    dSv_info_c* save = saveInfo();
    save->offSwitch(sw, room(a));
}
/* 024B2DC0 */
s32 Act_c::_create() {
    fopAcM_ct(this, Act_c);
    field_0x298 = 0;
    field_0x299 = 0;
    u32 type = prmAbstract(this, 2, 6);
    field_0x2a0 = 0;
    mType = (s32)type;
    if (type == 0) {
        s32 currentRoom = current.roomNo;
        dSv_info_c* save = saveInfo();
        BOOL complete = save->mSavedata.mEvent.isEventBit(
                                                                        currentRoom == 20 ? 0x1902 : 0x1901);
        if (complete) {
            u32 sw = prmAbstract(this, 8, 8);
            save = saveInfo();
            save->onSwitch(sw, room(this));
        } else {
            save = saveInfo();
            if (save->mRestart.mStartCode == 2 &&
                    current.roomNo == save->mRestart.mRestartRoom) {
                field_0x2a0 = 10;
                u32 sw = prmAbstract(this, 8, 8);
                save = saveInfo();
                save->onSwitch(sw, room(this));
            } else {
                u32 sw = prmAbstract(this, 8, 8);
                save->offSwitch(sw, room(this));
            }
        }
    }
    field_0x2a4 = 0;
    return 4;
}
/* 024B3028 */
BOOL Act_c::check_timer_clear() {
    if (!play()->play.mNextStage.enable)
        return TRUE;
    // HD: preserve all virtual assure calls and the bounded, wrapping byte comparison.
    for (const char* name : {"MiniKaz", "MiniHyo"}) {
        gabi::Local<SafeString> a, b;
        a->mStringTop = gabi::guest_addr(name);
        a->__vtbl = 0x1003FCA0;
        b->mStringTop = gabi::ea(play()->play.mNextStage.mName);
        b->__vtbl = 0x1003FCA0;
        gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
        gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
        u32 left = a->mStringTop;
        gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b.get());
        u32 right = b->mStringTop;
        if (left == right)
            return FALSE;
        for (u32 i = 0; i < 0x40001; ++i) {
            u8 c = gabi::load<u8>(left + i);
            if (c != gabi::load<u8>(right + i))
                break;
            if (!c)
                return FALSE;
        }
    }
    return TRUE;
}
static void deleteTimer() {
    if (play()->play.mTimerInfo.mTimerMode == 3) {
        void* timer = dComIfG_getTimerPtr();
        if (timer)
            TagVolcano_timerDelete(timer);
    }
}
/* 024B31F8 */
BOOL Act_c::_delete() {
    if (check_timer_clear())
        deleteTimer();
    offSwitch(this);
    return TRUE;
}
static s32 restTime() {
    u32 end = play()->play.mTimerInfo.mTimerLimitTimeMs;
    u32 start = play()->play.mTimerInfo.mTimerNowTimeMs;
    return (s32)((end - start) * 1000u) / 30;
}
static void expire() {
    TagVolcano_seStart(JA_SE_ISLE_TIMER_0);
    dComIfG_inf_c* p = play();
    gabi::Local<cXyz> direction;
    direction->x = 0.0f;
    direction->y = 1.0f;
    direction->z = 0.0f;
    p->play.mVibration.StartShock(6, -33, direction.get());
}
/* 024B3280 */
BOOL Act_c::_execute() {
    if (mType == 0) {
        u32 sw = prmAbstract(this, 8, 8);
        dSv_info_c* save = saveInfo();
        if (!save->isSwitch(sw, room(this)))
            return TRUE;
        s32 delay = field_0x2a0;
        if (delay > 0) {
            delay = (s32)((u32)delay - 1u);
            field_0x2a0 = delay;
            if (delay == 0)
                offSwitch(this);
            return TRUE;
        }
        if (field_0x298) {
            if (!play()->play.mTimerInfo.mTimerPtr)
                return TRUE;
            u8 started = field_0x299;
            dComIfG_inf_c* p = play();
            if (!started) {
                if (p->play.mTimerInfo.mTimerMode == 3) {
                    void* timer = dComIfG_getTimerPtr();
                    if (timer)
                        TagVolcano_timerStart(timer);
                }
                field_0x299 = 1;
                return TRUE;
            }
            u8 running = p->play.mEvtCtrl.mMode;
            if (play()->play.mTimerInfo.mTimerMode == 3) {
                void* timer = dComIfG_getTimerPtr();
                if (timer) {
                    if (running) {
                        TagVolcano_timerStop(timer, 2);
                    } else {
                        TagVolcano_timerRestart(timer, 2);
                    }
                }
            }
            if (restTime() <= 0) {
                expire();
                offSwitch(this);
                deleteTimer();
                field_0x298 = 0;
                field_0x299 = 0;
            }
        } else {
            u16 duration = (u16)(prmAbstract(this, 8, 16) * 10u);
            s32 currentRoom = current.roomNo;
            dComIfG_inf_c* p = play();
            if (TagVolcano_endCheck(&p->play.mEvtManager, currentRoom == dIsleRoom_FireMountain_e ? "FREEZE_VOLCANO" : "MELT_ICE")) {
                dTimer_createTimer( 3, duration, 1, 0, 221.0f, 439.0f, 32.0f,
                                      419.0f);
                field_0x298 = 1;
            }
        }
    } else {
        u32 bit = prmAbstract(this, 6, 0);
        dSv_info_c* save = saveInfo();
        BOOL chest = TagVolcano_isTbox(&save->mMemory, bit);
        void* timer = dComIfG_getTimerPtr();
        if (chest) {
            if (timer)
                deleteTimer();
            s32 type = mType;
            save = saveInfo();
            save->mSavedata.mEvent.onEventBit(
                                  type == 1 ? 0x1902 : 0x1901);
            field_0x2a4 = 0;
            return TRUE;
        }
        if (timer) {
            u8 running = play()->play.mEvtCtrl.mMode;
            if (play()->play.mTimerInfo.mTimerMode == 3) {
                timer = dComIfG_getTimerPtr();
                if (timer) {
                    if (running) {
                        TagVolcano_timerStop(timer, 2);
                    } else {
                        TagVolcano_timerRestart(timer, 2);
                    }
                }
            }
            if (restTime() <= 0) {
                expire();
                deleteTimer();
                field_0x2a4 = 1;
                TagVolcano_orderOtherEvent(this, "TAG_VOLCANO", 1, 0xFFFF);
            }
        } else if (!field_0x2a4) {
            expire();
            field_0x2a4 = 1;
            TagVolcano_orderOtherEvent(this, "TAG_VOLCANO", 1, 0xFFFF);
        }
        if (field_0x2a4) {
            if (eventInfo.checkCommandDemoAccrpt()) {
                s32 type = mType;
                TagVolcano_setNextStage("sea", 2, type == 1 ? 20 : 40, -1, 0,
                                      1, 0, 0.0f);
            } else
                TagVolcano_orderOtherEvent(this, "TAG_VOLCANO", 1, 0xFFFF);
        }
    }
    return TRUE;
}
/* 024B3854 */
s32 Mthd_Create(Act_c *actor) {
    return actor->_create();
}
/* 024B3858 */
BOOL Mthd_Delete(Act_c *actor) {
    return actor->_delete();
}
/* 024B385C */
BOOL Mthd_Execute(Act_c *actor) {
    return actor->_execute();
}
/* 024B3868 */
void staticInitialize() {
    sinit_header_statics(0x1046E67C, 0x101D2184);
}
/* 024B3914 */
void destruct(Act_c *actor, s32 flags) {
    if (actor) {
        __dt(static_cast<fopAc_ac_c*>(actor));
        if (flags & 1)
            operator_delete(actor);
    }
}
/* 024B3860 */
BOOL Mthd_Draw(Act_c *actor) {
    return TRUE;
}
/* 024B3968 */
BOOL Mthd_IsDelete(Act_c *actor) {
    return TRUE;
}
/* 024B38FC */
void deleteStatic(void *p, s32 flags) {
    if (p && (flags & 1))
        operator_delete(p);
}
/* 024B3910 */
void emptyVirtual() {}


/* WWHD glue (tools/verify/autoglue.py)
ext 025C58D8 void TagVolcano_timerDelete(void*)
ext 025C5864 void TagVolcano_timerStart(void*)
ext 025C5794 void TagVolcano_timerStop(void*, s32)
ext 025C57D4 void TagVolcano_timerRestart(void*, s32)
ext 025C60F4 void dTimer_createTimer(s32, u16, s32, s32, f32, f32, f32, f32)
ext 0254457C BOOL TagVolcano_endCheck(dEvent_manager_c*, const char*)
ext 025E1988 void TagVolcano_seStart(u32)
ext 025D77DC void TagVolcano_orderOtherEvent(fopAc_ac_c*, const char*, u8, u16)
ext 0252012C void TagVolcano_setNextStage(const char*, s32, s32, s32, s32, s32, s32, f32)
ext 025B8C74 BOOL TagVolcano_isTbox(dSv_memory_c*, u32)
str 1003FCC8 "MiniKaz"
str 1003FCD0 "MiniHyo"
str 1003FCFC "FREEZE_VOLCANO"
str 1003FD0C "MELT_ICE"
str 1003FCF0 "TAG_VOLCANO"
str 1003FCEC "sea"
*/
