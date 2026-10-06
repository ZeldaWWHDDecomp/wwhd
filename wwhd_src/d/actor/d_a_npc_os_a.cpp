/**
 * d_a_npc_os_a.cpp (WWHD)
 * NPC - Os, part A: event processing, action dispatch, beams, event ordering, execute, draw.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_os.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_os_local.h"

#define OS_L_HIO 0x10467E44u

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* functions of the other parts (by address) */
static inline BOOL os_setAction(daNpc_Os_c* t, ProcFunc_l* cur, ProcFunc_l* f, void* p) { return gabi::call<BOOL>(0x022AA630, t, cur, f, p); }
static inline void os_setNpcAction(daNpc_Os_c* t, ProcFunc_l* f, void* p) { gabi::call(0x022AA778, t, f, p); }
static inline void os_pmf_npc(daNpc_Os_c* t, u32 pmf, void* arg) {
    gabi::Local<ProcFunc_l> fn;
    md_pmf_load(fn, pmf);
    os_setNpcAction(t, fn, arg);
}

/* 022ABAC8 */
void daNpc_Os_c::setPlayerAction(ProcFunc_l* actionFunc, void* arg) {
    WWHD_FUNC(0x022ABAC8, void, this, actionFunc, arg);
    mNpcAction.d = 0;
    mNpcAction.i = 0;
    mNpcAction.f = 0;
    gabi::Local<ProcFunc_l> fn; /* by-value copy */
    md_pmf_load(fn, gabi::ea(actionFunc));
    os_setAction(this, &mPlayerAction, fn, arg);
}
VERIFY(0x022ABAC8, &daNpc_Os_c::setPlayerAction);

/* 022ABB18 */
void daNpc_Os_c::playerAction(void* param_1) {
    WWHD_FUNC(0x022ABB18, void, this, param_1);
    if (mPlayerAction.i == 0) { /* !mPlayerAction */
        speedF = 0.0f;
        gabi::Local<ProcFunc_l> fn;
        md_pmf_load(fn, 0x1001F268); /* &daNpc_Os_c::waitPlayerAction */
        setPlayerAction(fn, nullptr);
    }
    gabi::store<u8>(dComIfGp_ea() + 0x5BB8, 7);    /* dComIfGp_setRStatusForce(dActStts_RETURN_e) */
    gabi::store<u8>(dComIfGp_ea() + 0x5BB7, 0x3E); /* dComIfGp_setDoStatus(dActStts_HIDDEN_e) */
    gabi::store<u8>(dComIfGp_ea() + 0x5BB6, 0x3E); /* dComIfGp_setAStatus(dActStts_HIDDEN_e) */
    md_pmf_call<BOOL>(this, &mPlayerAction, param_1);
}
VERIFY(0x022ABB18, &daNpc_Os_c::playerAction);

/* 022ABBF8 */
BOOL daNpc_Os_c::returnLinkCheck() {
    WWHD_FUNC(0x022ABBF8, BOOL, this);
    if (!(gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0) /* dComIfGp_event_runCheck() */) {
        /* HD: one pad check (02007840) instead of CPad_CHECK_TRIG_R || CPad_CHECK_TRIG_START */
        if (gabi::call<BOOL>(0x02007840, 0) && (mAcch.m_flags & 0x20) /* ChkGroundHit */) {
            return TRUE;
        }
    }
    return FALSE;
}
VERIFY(0x022ABBF8, &daNpc_Os_c::returnLinkCheck);

/* 022ABC68 */
void daNpc_Os_c::carryCheck() {
    WWHD_FUNC(0x022ABC68, void, this);
    if (actor_status & 0x2000 /* fopAcM_checkCarryNow */) {
        os_pmf_npc(this, 0x1001F280 /* &daNpc_Os_c::carryNpcAction */, nullptr);
    }
}
VERIFY(0x022ABC68, &daNpc_Os_c::carryCheck);

/* 022ABCB0 */
void daNpc_Os_c::checkOrder() {
    WWHD_FUNC(0x022ABCB0, void, this);
    if (gabi::load<u16>(gabi::ea(this) + 0xF8) == 1 /* eventInfo.checkCommandTalk() */ &&
        (field_0x7A5 == 0x12 || field_0x7A5 == 0x11)) {
        field_0x7A5 = -1;
        os_pmf_npc(this, 0x1001F2A8 /* &daNpc_Os_c::talkNpcAction */, nullptr);
        fopAcM_cancelCarryNow(this);
    }
}
VERIFY(0x022ABCB0, &daNpc_Os_c::checkOrder);

static inline void os_initBrkAnm(daNpc_Os_c* t, u32 no, u32 b) { gabi::call(0x022A9F4C, t, no, b); }
static inline BOOL os_wakeupCheck(daNpc_Os_c* t) { return gabi::call<BOOL>(0x022A9E0C, t); }
static inline BOOL os_finishCheck(daNpc_Os_c* t) { return gabi::call<BOOL>(0x022A9EAC, t); }
static inline void os_cb_end(daNpc_Os_infiniteEcallBack_l* cb) { gabi::call(0x022AB258, cb); }

/* 022ABD30 */
void daNpc_Os_c::npcAction(void* param_1) {
    WWHD_FUNC(0x022ABD30, void, this, param_1);
    if (mNpcAction.i == 0) { /* !mNpcAction */
        s8 brk = field_0x7A2;
        speedF = 0.0f;
        if (brk == 8) {
            os_initBrkAnm(this, 6, 1);
        }
        os_pmf_npc(this, 0x1001F260 /* &daNpc_Os_c::waitNpcAction */, nullptr);
    }
    md_pmf_call<BOOL>(this, &mNpcAction, param_1);
}
VERIFY(0x022ABD30, &daNpc_Os_c::npcAction);

/* 022ABE08 */
s8 daNpc_Os_c::getFinishOrderEventNum() {
    WWHD_FUNC(0x022ABE08, s8, this);
    if (actor_status & 0x2000 /* fopAcM_checkCarryNow */) {
        if (argument == 0) return 0xB;
        if (argument == 1) return 0xD;
        if (argument == 2) return 0xF;
    } else {
        if (argument == 0) return 0xA;
        if (argument == 1) return 0xC;
        if (argument == 2) return 0xE;
    }
    return -1;
}
VERIFY(0x022ABE08, &daNpc_Os_c::getFinishOrderEventNum);

/* 022ABE84 */
BOOL daNpc_Os_c::checkGoalRoom() {
    WWHD_FUNC(0x022ABE84, BOOL, this);
    if (current.roomNo == 7 && os_wakeupCheck(this) && !os_finishCheck(this)) {
        field_0x7A5 = getFinishOrderEventNum();
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x022ABE84, &daNpc_Os_c::checkGoalRoom);

/* 022ABF04 */
static void daNpc_Os_infiniteEcallBack_makeEmitter(daNpc_Os_infiniteEcallBack_l* cb, u16 id, const cXyz* pos, const csXyz* angle,
                                                   const cXyz* scale) {
    WWHD_FUNC(0x022ABF04, void, cb, id, pos, angle, scale);
    os_cb_end(cb);
    /* dComIfGp_particle_set(id, pos, angle, scale, 0xFF, this) */
    dPa_control_set(dComIfGp_getParticle(), 0, id, pos, angle, scale, 0xFF, (dPa_levelEcallBack*)cb, -1, nullptr, nullptr, nullptr);
}
VERIFY(0x022ABF04, daNpc_Os_infiniteEcallBack_makeEmitter);

/* 022ABF7C */
void daNpc_Os_c::makeBeam(int param_1) {
    WWHD_FUNC(0x022ABF7C, void, this, param_1);
    if (field_0x738.mpBaseEmitter == nullptr) {
        daNpc_Os_infiniteEcallBack_makeEmitter(&field_0x738, 0x826E /* ID_AK_SN_OTOMOBEAM00 */, &current.pos, &shape_angle, nullptr);
        if (param_1) {
            s32 reverb = dComIfGp_getReverb(current.roomNo); /* fopAcM_seStartCurrent */
            mDoAud_seStart(0x5900 /* JA_SE_OBJ_OSTATUE_LIGHT_ST */, &current.pos, 0, reverb);
        }
    }
    if (field_0x740.mpBaseEmitter == nullptr) {
        daNpc_Os_infiniteEcallBack_makeEmitter(&field_0x740, 0x826F /* ID_AK_SN_OTOMOBEAM01 */, &current.pos, &shape_angle, nullptr);
    }
}
VERIFY(0x022ABF7C, &daNpc_Os_c::makeBeam);

static inline BOOL os_isSwitch(u32 sw, s32 room) { return gabi::call<BOOL>(0x025BA0C0, gabi::load<u32>(0x101F84DC) + 0x20, sw, room); }
static inline BOOL os_isEventBit(u16 f) { return gabi::call<BOOL>(0x025B8B94, gabi::load<u32>(0x101F84DC) + 0x644, f); }

/* 022AC030 */
void daNpc_Os_c::eventOrderCheck() {
    WWHD_FUNC(0x022AC030, void, this);
    if (field_0x7A5 == -1) {
        s8 roomNo = current.roomNo;
        if (argument == 0) {
            if (roomNo == 7 && os_isSwitch(field_0x794, roomNo) && !os_isEventBit(0x2510)) {
                field_0x7A5 = 7;
            }
        } else if (argument == 1) {
            if (roomNo == 7 && os_isSwitch(field_0x794, roomNo) && !os_isEventBit(0x2608)) {
                field_0x7A5 = 8;
            }
        } else if (argument == 2) {
            if (roomNo == 7 && os_isSwitch(field_0x794, roomNo) && !os_isEventBit(0x2604)) {
                field_0x7A5 = 9;
            }
        }
    }
}
VERIFY(0x022AC030, &daNpc_Os_c::eventOrderCheck);

/* 022AC160 */
void daNpc_Os_c::eventOrder() {
    WWHD_FUNC(0x022AC160, void, this);
    if (m4E4 & 2 /* isEventAccept() */) {
        return;
    }
    if (field_0x7A5 == 0x12 || field_0x7A5 == 0x11) {
        gabi::store<u16>(gabi::ea(this) + 0xFA, gabi::load<u16>(gabi::ea(this) + 0xFA) | 1); /* eventInfo.onCondition(CANTALK) */
        if (field_0x7A5 == 0x12) {
            gabi::call(0x025D76A8, this); /* fopAcM_orderSpeakEvent */
        }
    } else if (field_0x7A5 != -1 && field_0x7A5 < 0x10) {
        field_0x7AA = field_0x7A5;
        gabi::call(0x025D7A58, this, (s32)field_0x7C4[field_0x7AA], 0xFF, 0xFFFF, 0, 1); /* fopAcM_orderOtherEventId */
    }
}
VERIFY(0x022AC160, &daNpc_Os_c::eventOrder);

static inline u32 os_evmng() { return dComIfGp_ea() + 0x52C4; }

/* 022AB6DC */
BOOL daNpc_Os_c::eventProc() {
    WWHD_FUNC(0x022AB6DC, BOOL, this);
    if (gabi::load<u16>(gabi::ea(this) + 0xF8) == 2 /* eventInfo.checkCommandDemoAccrpt() */ && field_0x7A5 != -1) {
        if (field_0x7A5 == 0) {
            if (gabi::call<BOOL>(0x025445B8, os_evmng(), 0x1001F620 /* "OPTION_CHAR_END" */) /* startCheckOld */ ||
                gabi::call<BOOL>(0x0254457C, os_evmng(), 0x1001F620) /* endCheckOld */) {
                fopAc_ac_c* link = dComIfGp_getLinkPlayer();
                u32 evt = dComIfGp_ea() + 0x51D0; /* dComIfGp_event_setTalkPartner(link) */
                gabi::store<u32>(evt + 0xCC, gabi::call<u32>(0x0253F124, evt, link));
                gabi::call(0x025E1988, 0x886); /* mDoAud_seStart(JA_SE_CTRL_NPC_TO_LINK) */
            } else {
                m4E4 = m4E4 & ~1u; /* offReturnLink() */
                field_0x7A5 = -1;
                goto staff;
            }
        } else if (field_0x7A5 != 2 && field_0x7A5 != 4 && field_0x7A5 != 6) {
            if ((u32)(field_0x7A5 - 0xA) <= 5) {
                os_pmf_npc(this, 0x1001F260 /* &daNpc_Os_c::waitNpcAction */, nullptr);
                mNoResetFlg1 = (mNoResetFlg1 & ~2u) | 0x40; /* offNpcCallCommand(); onNpcNotChange() */
                gabi::store<u32>(dComIfGp_ea() + 0x5B38, 0); /* dComIfGp_setCb1Player(NULL) */
                gabi::call(0x022AB618, this);                /* setFinish() */
            } else if (field_0x7A5 == 8) {
                gabi::call(0x025B8B68, gabi::load<u32>(0x101F84DC) + 0x644, 0x2608); /* dComIfGs_onEventBit */
            } else if (field_0x7A5 == 9) {
                gabi::call(0x025B8B68, gabi::load<u32>(0x101F84DC) + 0x644, 0x2604);
            }
        }
        if (field_0x7A5 != -1) {
            u32 f = m4E4;
            field_0x7A5 = -1;
            m4E4 = f | 2; /* onEventAccept() */
        }
    }
staff:
    int staffIdx = gabi::call<s32>(0x022AB5A4, this); /* getMyStaffId() */
    if (gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0 /* dComIfGp_event_runCheck() */ && !gabi::call<BOOL>(0x022AB670, this) /* checkCommandTalk() */) {
        if (staffIdx != -1) {
            s32 actIdx = gabi::call<s32>(0x02542EDC, os_evmng(), staffIdx, 0x101C2854 /* cut_name_tbl */, 0xC, 1, 0); /* getMyActIdx */
            u32 ev = os_evmng();
            if (actIdx == -1) {
                gabi::call(0x02543280, ev, staffIdx); /* cutEnd */
            } else {
                if (gabi::call<BOOL>(0x025447C8, ev, staffIdx) /* getIsAddvance */) {
                    md_pmf_call<BOOL>(this, gabi::at<ProcFunc_l>(0x101C2964 + actIdx * 8) /* event_init_tbl */, staffIdx);
                }
                if (md_pmf_call<BOOL>(this, gabi::at<ProcFunc_l>(0x101C29C4 + actIdx * 8) /* event_action_tbl */, staffIdx)) {
                    gabi::call(0x02543280, os_evmng(), staffIdx); /* cutEnd */
                }
            }
        }
        if (m4E4 & 2 /* isEventAccept() */) {
            s16 id = field_0x7C4[field_0x7AA];
            if (gabi::call<BOOL>(0x025440C8, os_evmng(), (s32)id) /* endCheck */) {
                u32 play = dComIfGp_ea(); /* dComIfGp_event_reset() */
                gabi::store<u16>(play + 0x52B8, gabi::load<u16>(play + 0x52B8) | 8);
                m4E4 = m4E4 & ~2u; /* offEventAccept() */
                if (field_0x7AA == 0) {
                    gabi::call(0x022AB684, this); /* returnLinkPlayer() */
                    m4E4 = m4E4 & ~1u;            /* offReturnLink() */
                }
                field_0x7AA = -1;
            }
            return TRUE;
        }
        if (gabi::load<u16>(gabi::ea(dComIfGp_getLinkPlayer()) + 0xF8) != 3 /* !eventInfo.checkCommandDoor() */) {
            return TRUE;
        }
    }
    return FALSE;
}
VERIFY(0x022AB6DC, &daNpc_Os_c::eventProc);

/* 022AD0F0 */
static BOOL daNpc_Os_Execute(daNpc_Os_c* i_this) {
    WWHD_FUNC(0x022AD0F0, BOOL, i_this);
    BOOL ret = gabi::call<BOOL>(0x022AC1DC, i_this); /* execute() */
    if (i_this->argument == 2) {
        gabi::store<s8>(0x101D5F46, i_this->current.roomNo); /* setCattleRoomNo(fopAcM_GetRoomNo(this)) */
    }
    return ret;
}
VERIFY(0x022AD0F0, daNpc_Os_Execute);

/* 022AD138 HD: no blob shadow (dComIfGd_setShadow removed) */
BOOL daNpc_Os_c::draw() {
    WWHD_FUNC(0x022AD138, BOOL, this);
    if (home.roomNo < 0) {
        return TRUE;
    }
    if (gabi::call<BOOL>(0x022A9EAC, this) /* finishCheck() */ && mpPedestal == nullptr) {
        return TRUE;
    }
    s8 roomNo = current.roomNo;
    dComIfGp_get(); /* HD: dComIfGp_roomControl_checkStatusFlag (room status table 0x1047E8E8, 0x22C each) */
    if (!(gabi::load<u8>(0x1047E8E8 + roomNo * 0x22C) & 0x10)) {
        return TRUE;
    }
    J3DModel* pModel = mpMorf->getModel();
    J3DModelData* pModelData = gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(pModel) + 0xAC));
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), pModel, &tevStr);
    gabi::call(0x025E83FC, &mBrkAnm, pModelData, (f32)mBrkAnm.mFrameCtrl.mFrame); /* mBrkAnm.entry(modelData) */
    gabi::call(0x025E5590, mpMorf.get()); /* mpMorf->entryDL() */
    gabi::store<u32>(gabi::ea(pModelData) + 0x48, 0); /* mBrkAnm.remove(modelData) */
    return TRUE;
}
VERIFY(0x022AD138, &daNpc_Os_c::draw);

/* m_smoke_tevstr = tevStr (dKy_tevstr_c::operator=, HD: member-wise; m_smoke_tevstr at 0x10475460) */
static inline void os_copy_tevstr(daNpc_Os_c* self) {
    u32 a = gabi::ea(self);
    u32 T = 0x10475460;
    gabi::store<f32>(T + 0x0, gabi::load<f32>(a + 0x110));
    gabi::store<f32>(T + 0x4, gabi::load<f32>(a + 0x114));
    gabi::store<f32>(T + 0x8, gabi::load<f32>(a + 0x118));
    gabi::store<f32>(T + 0xC, gabi::load<f32>(a + 0x11C));
    gabi::store<f32>(T + 0x10, gabi::load<f32>(a + 0x120));
    gabi::store<f32>(T + 0x14, gabi::load<f32>(a + 0x124));
    gabi::store<u8>(T + 0x18, gabi::load<u8>(a + 0x128));
    gabi::store<u8>(T + 0x19, gabi::load<u8>(a + 0x129));
    gabi::store<u8>(T + 0x1A, gabi::load<u8>(a + 0x12A));
    gabi::store<u8>(T + 0x1B, gabi::load<u8>(a + 0x12B));
    gabi::store<u16>(T + 0x1C, gabi::load<u16>(a + 0x12C));
    gabi::store<u16>(T + 0x1E, gabi::load<u16>(a + 0x12E));
    gabi::store<u16>(T + 0x20, gabi::load<u16>(a + 0x130));
    gabi::store<u16>(T + 0x22, gabi::load<u16>(a + 0x132));
    gabi::store<f32>(T + 0x24, gabi::load<f32>(a + 0x134));
    gabi::store<f32>(T + 0x28, gabi::load<f32>(a + 0x138));
    gabi::store<f32>(T + 0x2C, gabi::load<f32>(a + 0x13C));
    gabi::store<f32>(T + 0x30, gabi::load<f32>(a + 0x140));
    gabi::store<f32>(T + 0x34, gabi::load<f32>(a + 0x144));
    gabi::store<f32>(T + 0x38, gabi::load<f32>(a + 0x148));
    gabi::store<f32>(T + 0x3C, gabi::load<f32>(a + 0x14C));
    gabi::store<f32>(T + 0x40, gabi::load<f32>(a + 0x150));
    gabi::store<u32>(T + 0x84, gabi::load<u32>(a + 0x194));
    gabi::store<u32>(T + 0x88, gabi::load<u32>(a + 0x198));
    gabi::store<u32>(T + 0x8C, gabi::load<u32>(a + 0x19C));
    gabi::store<u16>(T + 0x90, gabi::load<u16>(a + 0x1A0));
    gabi::store<u16>(T + 0x92, gabi::load<u16>(a + 0x1A2));
    gabi::store<u16>(T + 0x94, gabi::load<u16>(a + 0x1A4));
    gabi::store<u16>(T + 0x96, gabi::load<u16>(a + 0x1A6));
    gabi::store<u32>(T + 0x98, gabi::load<u32>(a + 0x1A8)); /* lswi/stswi 4 bytes */
    gabi::store<u32>(T + 0x9C, gabi::load<u32>(a + 0x1AC));
    gabi::store<u16>(T + 0xA0, gabi::load<u16>(a + 0x1B0));
    gabi::store<u16>(T + 0xA2, gabi::load<u16>(a + 0x1B2));
    gabi::store<u16>(T + 0xA4, gabi::load<u16>(a + 0x1B4));
    gabi::store<u16>(T + 0xA6, gabi::load<u16>(a + 0x1B6));
    gabi::store<f32>(T + 0xA8, gabi::load<f32>(a + 0x1B8));
    gabi::store<f32>(T + 0xAC, gabi::load<f32>(a + 0x1BC));
    gabi::store<f32>(T + 0xB0, gabi::load<f32>(a + 0x1C0));
    gabi::store<u8>(T + 0xB4, gabi::load<u8>(a + 0x1C4));
    gabi::store<u8>(T + 0xB5, gabi::load<u8>(a + 0x1C5));
    gabi::store<u8>(T + 0xB6, gabi::load<u8>(a + 0x1C6));
    gabi::store<u8>(T + 0xB7, gabi::load<u8>(a + 0x1C7));
    gabi::store<u8>(T + 0xB8, gabi::load<u8>(a + 0x1C8));
    gabi::store<u8>(T + 0xB9, gabi::load<u8>(a + 0x1C9));
    gabi::store<u8>(T + 0xBA, gabi::load<u8>(a + 0x1CA));
    gabi::store<u8>(T + 0xBB, gabi::load<u8>(a + 0x1CB));
    gabi::store<u8>(T + 0xBC, gabi::load<u8>(a + 0x1CC));
    gabi::store<f32>(T + 0xC0, gabi::load<f32>(a + 0x1D0));
    gabi::store<f32>(T + 0xC4, gabi::load<f32>(a + 0x1D4));
    gabi::store<f32>(T + 0xC8, gabi::load<f32>(a + 0x1D8));
    gabi::store<f32>(T + 0xCC, gabi::load<f32>(a + 0x1DC));
    gabi::store<f32>(T + 0xD0, gabi::load<f32>(a + 0x1E0));
    gabi::store<f32>(T + 0xD4, gabi::load<f32>(a + 0x1E4));
    gabi::store<u8>(T + 0xD8, gabi::load<u8>(a + 0x1E8));
    gabi::store<u8>(T + 0xD9, gabi::load<u8>(a + 0x1E9));
    gabi::store<u8>(T + 0xDA, gabi::load<u8>(a + 0x1EA));
    gabi::store<u8>(T + 0xDB, gabi::load<u8>(a + 0x1EB));
    gabi::store<u16>(T + 0xDC, gabi::load<u16>(a + 0x1EC));
    gabi::store<u16>(T + 0xDE, gabi::load<u16>(a + 0x1EE));
    gabi::store<u16>(T + 0xE0, gabi::load<u16>(a + 0x1F0));
    gabi::store<u16>(T + 0xE2, gabi::load<u16>(a + 0x1F2));
    gabi::store<f32>(T + 0xE4, gabi::load<f32>(a + 0x1F4));
    gabi::store<f32>(T + 0xE8, gabi::load<f32>(a + 0x1F8));
    gabi::store<f32>(T + 0xEC, gabi::load<f32>(a + 0x1FC));
    gabi::store<f32>(T + 0xF0, gabi::load<f32>(a + 0x200));
    gabi::store<f32>(T + 0xF4, gabi::load<f32>(a + 0x204));
    gabi::store<f32>(T + 0xF8, gabi::load<f32>(a + 0x208));
    gabi::store<f32>(T + 0xFC, gabi::load<f32>(a + 0x20C));
    gabi::store<f32>(T + 0x100, gabi::load<f32>(a + 0x210));
    gabi::store<f32>(T + 0x144, gabi::load<f32>(a + 0x254));
    gabi::store<f32>(T + 0x148, gabi::load<f32>(a + 0x258));
    gabi::store<f32>(T + 0x14C, gabi::load<f32>(a + 0x25C));
    gabi::store<f32>(T + 0x150, gabi::load<f32>(a + 0x260));
    gabi::store<f32>(T + 0x154, gabi::load<f32>(a + 0x264));
    gabi::store<f32>(T + 0x158, gabi::load<f32>(a + 0x268));
    gabi::store<u8>(T + 0x15C, gabi::load<u8>(a + 0x26C));
    gabi::store<u8>(T + 0x15D, gabi::load<u8>(a + 0x26D));
    gabi::store<u8>(T + 0x15E, gabi::load<u8>(a + 0x26E));
    gabi::store<u8>(T + 0x15F, gabi::load<u8>(a + 0x26F));
    gabi::store<u16>(T + 0x160, gabi::load<u16>(a + 0x270));
    gabi::store<u16>(T + 0x162, gabi::load<u16>(a + 0x272));
    gabi::store<u16>(T + 0x164, gabi::load<u16>(a + 0x274));
    gabi::store<u16>(T + 0x166, gabi::load<u16>(a + 0x276));
    gabi::store<f32>(T + 0x168, gabi::load<f32>(a + 0x278));
    gabi::store<f32>(T + 0x16C, gabi::load<f32>(a + 0x27C));
    gabi::store<f32>(T + 0x170, gabi::load<f32>(a + 0x280));
    gabi::store<f32>(T + 0x174, gabi::load<f32>(a + 0x284));
    gabi::store<f32>(T + 0x178, gabi::load<f32>(a + 0x288));
    gabi::store<f32>(T + 0x17C, gabi::load<f32>(a + 0x28C));
    gabi::store<f32>(T + 0x180, gabi::load<f32>(a + 0x290));
    gabi::store<f32>(T + 0x184, gabi::load<f32>(a + 0x294));
}

static inline bool os_isPmf(ProcFunc_l* p, u32 f) { return p->i == -1 && (p->i == 0 || (p->d == 0 && p->f == f)); }
static inline void os_endBeam(daNpc_Os_c* t) { gabi::call(0x022AB28C, t); }
static inline void os_setEmitterScale(u32 e, u32 scale) {
    f32 x = gabi::load<f32>(scale);
    gabi::store<f32>(e + 0x220, x);
    f32 y = gabi::load<f32>(scale + 4);
    gabi::store<f32>(e + 0x224, y);
    f32 z = gabi::load<f32>(scale + 8);
    gabi::store<f32>(e + 0x238, x);
    gabi::store<f32>(e + 0x228, z);
    gabi::store<f32>(e + 0x23C, y);
    gabi::store<f32>(e + 0x240, z);
}
static inline u32 os_gnd(daNpc_Os_c* t) { return gabi::ea(&t->mAcch) + 0xE8; } /* mAcch.m_gnd */
static inline f32 os_groundH(daNpc_Os_c* t) { return gabi::load<f32>(gabi::ea(&t->mAcch) + 0x94); }

/* 022AC1DC */
BOOL daNpc_Os_c::execute() {
    WWHD_FUNC(0x022AC1DC, BOOL, this);
    /* static JGeometry::TVec3<f32> splash_scale(0.6f, ...), ripple_scale(1.0f, ...) */
    if (gabi::load<u32>(0x10467F00) == 0) {
        gabi::store<u32>(0x10467F00, 1);
        gabi::store<f32>(0x10467F08, 0.6f);
        gabi::store<f32>(0x10467F10, 0.6f);
        gabi::store<f32>(0x10467F0C, 0.6f);
    }
    if (gabi::load<u32>(0x10467F04) == 0) {
        gabi::store<u32>(0x10467F04, 1);
        gabi::store<f32>(0x10467F14, 1.0f);
        gabi::store<f32>(0x10467F1C, 1.0f);
        gabi::store<f32>(0x10467F18, 1.0f);
    }
    field_0x784 = field_0x784 & ~0x10u;
    actor_status = actor_status & ~0x20u; /* fopAcM_OffStatus(SHOWMAP) */
    gabi::call(0x022AB1C4, this);          /* checkPlayerRoom() */
    if (!gabi::call<BOOL>(0x022A9EAC, this) /* finishCheck() */) {
        if (!gabi::call<BOOL>(0x024451B4, this) /* check_initialRoom() */) {
            current.roomNo = (s8)gabi::call<s32>(0x024EF130, dComIfG_Bgsp(), os_gnd(this)); /* HD: GetRoomId(m_gnd) */
            if (gabi::load<u32>(dComIfGp_ea() + 0x5B38) == gabi::ea(this)) { /* dComIfGp_getCb1Player() == this */
                gabi::store<u32>(dComIfGp_ea() + 0x5B38, 0);
            }
            os_endBeam(this);
            return TRUE;
        }
        gabi::call(0x0244586C, this, (s32)gabi::call<s8>(0x022AA42C, this) /* getRestartNumber() */, 1); /* initialRestartOption */
        if (gabi::call<BOOL>(0x022A9E0C, this) /* wakeupCheck() */) {
            mNoResetFlg1 = mNoResetFlg1 & ~0x40u; /* offNpcNotChange() */
        } else {
            mNoResetFlg1 = mNoResetFlg1 | 0x40u;  /* onNpcNotChange() */
        }
    }
    fopAcM_setStageLayer(this);

    f32 r15 = 15.0f;
    if (os_isPmf(&mNpcAction, 0x022AECD4 /* carryNpcAction */)) {
        gabi::call(0x024EFF3C, &mAcchCir[0], r15); /* SetWallR */
        gabi::call(0x024EFF3C, &mAcchCir[1], r15); /* SetWallR */
    } else {
        gabi::call(0x024EFF3C, &mAcchCir[0], 40.0f); /* SetWallR */
        gabi::call(0x024EFF3C, &mAcchCir[1], 40.0f); /* SetWallR */
    }
    /* searchFromName(l_daiza_name[argument], 0xFF, 1) */
    fopAc_ac_c* ped = gabi::call<fopAc_ac_c*>(0x025D9F38, gabi::load<u32>(0x101C28F0 + 4 * argument), 0xFF, 1);
    if (ped != nullptr) {
        u32 id = ped != nullptr ? gabi::load<u32>(gabi::ea(ped) + 4) : 0xFFFFFFFFu;
        if (gabi::call<BOOL>(0x025DD868, id) /* fpcM_IsCreating */) {
            ped = nullptr;
        }
    }
    mpPedestal = ped;

    if (ped != nullptr) {
        if (!(field_0x784 & 1) /* !isFinish() */) {
            if (gabi::call<BOOL>(0x022A9EAC, this)) {
                fopAc_ac_c* p = mpPedestal;
                home.pos.x = p->current.pos.x;
                home.pos.y = p->current.pos.y + 240.0f;
                home.pos.z = p->current.pos.z;
            }
        } else if (!(field_0x784 & 2) /* !isSetHomePos() */) {
            fopAc_ac_c* p = mpPedestal;
            home.pos.x = ped->current.pos.x;
            home.pos.y = p->current.pos.y + 240.0f;
            home.pos.z = p->current.pos.z;
            s8 pedRoom = p->home.roomNo;
            if (home.roomNo != pedRoom) {
                home.roomNo = pedRoom;
            }
            if (current.roomNo != pedRoom) {
                current.roomNo = pedRoom;
            }
            mAcch.CrrPos(dComIfG_Bgsp());
            field_0x784 = field_0x784 | 0x10;
            if (os_groundH(this) != -1000000000.0f) {
                gabi::store<u8>(gabi::ea(this) + 0x1C9, (u8)gabi::call<s32>(0x024EF130, dComIfG_Bgsp(), os_gnd(this))); /* tevStr.mRoomNo */
                u8 color = (u8)gabi::call<u32>(0x024EEEB8, dComIfG_Bgsp(), os_gnd(this)); /* GetPolyColor */
                u32 f = field_0x784;
                gabi::store<u8>(gabi::ea(this) + 0x1CA, color); /* tevStr.mEnvrIdxOverride */
                field_0x784 = f | 2; /* onSetHomePos() */
            }
        }
    }

    if (gabi::call<BOOL>(0x022A9EAC, this) /* finishCheck() */) {
        u32 f = field_0x784;
        s8 roomNo = current.roomNo;
        if (f & 1) { /* isFinish() */
            current.pos.copy(home.pos);
        }
        dComIfGp_get(); /* dComIfGp_roomControl_checkStatusFlag(roomNo, 0x10) */
        u8 st = gabi::load<u8>(0x1047E8E8 + roomNo * 0x22C);
        if (roomNo < 0 || !(st & 0x10) || mpPedestal == nullptr) {
            os_endBeam(this);
            return TRUE;
        }
    } else {
        if (gabi::call<BOOL>(0x024452A0, this) /* check_moveStop() */) {
            gabi::Local<ProcFunc_l> fn;
            md_pmf_load(fn, 0x1001F260); /* &daNpc_Os_c::waitNpcAction */
            os_setNpcAction(this, fn, nullptr);
            field_0x7A3 = 0;
            os_endBeam(this);
            return TRUE;
        }
        if (field_0x7A8 && gabi::call<BOOL>(0x02008254, dComIfG_Bgsp(), &field_0x7FC) /* ChkPolySafe */ &&
            gabi::call<BOOL>(0x024EEABC, dComIfG_Bgsp(), &field_0x7FC) /* ChkMoveBG */) {
            gabi::call(0x024EF968, dComIfG_Bgsp(), &field_0x7FC, 1, &old.pos, 0, 0); /* MoveBgCrrPos */
        }
    }

    actor_status = (actor_status & ~0x3Fu) | 0x2E; /* fopAcM_SetStatusMap(this, 0xE) (HD 0x2E) */
    {
        u32 h = OS_L_HIO; /* mJntCtrl.setParam(l_HIO.mNpc...) */
        gabi::call(0x0259E08C, &mJntCtrl, (s32)gabi::load<s16>(h + 0x36), (s32)gabi::load<s16>(h + 0x3A), (s32)gabi::load<s16>(h + 0x3E),
                   (s32)gabi::load<s16>(h + 0x42), (s32)gabi::load<s16>(h + 0x34), (s32)gabi::load<s16>(h + 0x38),
                   (s32)gabi::load<s16>(h + 0x3C), (s32)gabi::load<s16>(h + 0x40), (s32)gabi::load<s16>(h + 0x44));
    }

    if (!(field_0x784 & 1) /* !isFinish() */) {
        if (!(actor_status & 0x2000) && (field_0x784 & 8) /* isGravity() */ &&
            gabi::call<BOOL>(0x02445950, this, gabi::load<u32>(0x101C28E4 + 4 * argument)) /* checkNowPosMove(l_staff_name[argument]) */) {
            u16 ang = current.angle.y;
            f32 sy = speed.y;
            if (maxFallSpeed < sy) {
                sy = sy - gravity;
                speed.y = sy;
                if (sy < maxFallSpeed) {
                    speed.y = maxFallSpeed;
                }
            } else if (maxFallSpeed > sy) {
                sy = sy + gravity;
                speed.y = sy;
                if (sy > maxFallSpeed) {
                    speed.y = maxFallSpeed;
                }
            }
            u32 e = 0x104A44F8 + ((ang >> 3) << 3); /* cM_ssin / cM_scos */
            f32 sf = speedF;
            speed.x = sf * gabi::load<f32>(e);
            speed.z = sf * gabi::load<f32>(e + 4);
            gabi::call(0x025D6800, this, &mStts); /* fopAcM_posMove(this, mStts.GetCCMoveP()) */
        }
        mAcch.CrrPos(dComIfG_Bgsp());
        field_0x784 = field_0x784 | 0x10;

        f32 gndY = os_groundH(this);
        if ((os_isPmf(&mPlayerAction, 0x022AFF94 /* walkPlayerAction */) || os_isPmf(&mNpcAction, 0x022AF8D4 /* searchNpcAction */)) &&
            !(mAcch.m_flags & 0x20) /* !ChkGroundHit() */) {
            f32 delta = gndY - current.pos.y;
            if (delta < 0.0f && !(delta < -30.1f)) {
                speed.y = 0.0f;
                current.pos.y = gndY;
                mAcch.m_flags = mAcch.m_flags | 0x20; /* SetGroundHit() */
            }
            gndY = os_groundH(this);
        }

        if (gndY != -1000000000.0f) {
            u32 gnd = os_gnd(this);
            u32 pla = gabi::call<u32>(0x020084C8, dComIfG_Bgsp(), (u32)gabi::load<u16>(gnd + 2), (u32)gabi::load<u16>(gnd)); /* GetTriPla */
            if (pla) {
                field_0x7F0.copy(*gabi::at<cXyz>(pla)); /* plane->mNormal */
            }
            s8 roomNo = (s8)gabi::call<s32>(0x024EF130, dComIfG_Bgsp(), gnd); /* GetRoomId */
            current.roomNo = roomNo;
            gabi::store<s8>(gabi::ea(this) + 0x1C9, roomNo); /* tevStr.mRoomNo */
            u8 color = (u8)gabi::call<u32>(0x024EEEB8, dComIfG_Bgsp(), gnd); /* GetPolyColor */
            field_0x7FC.mpBgW = gabi::load<u32>(gnd + 4); /* field_0x7FC.SetPolyInfo(m_gnd) */
            field_0x7FC.mPolyIndex = gabi::load<u16>(gnd);
            field_0x7FC.mBgIndex = gabi::load<u16>(gnd + 2);
            gabi::store<u8>(gabi::ea(this) + 0x1CA, color);
            gabi::store<s8>(gabi::ea(&mStts) + 0x22, roomNo); /* mStts.SetRoomId(roomNo) */
            gabi::store<u32>(gabi::ea(&field_0x7FC) + 8, gabi::load<u32>(gnd + 8));
            if (roomNo != 7) {
                mpPedestal = nullptr;
            }
        }

        field_0x7A8 = (mAcch.m_flags >> 5) & 1; /* ChkGroundHit() */
        if (!(actor_status & 0x2000)) {
            if (os_groundH(this) == -1000000000.0f ||
                gabi::call<s32>(0x024EF0BC, dComIfG_Bgsp(), os_gnd(this)) /* GetGroundCode */ == 4) {
                u8 c = m4E8;
                if (c < 30) {
                    m4E8 = c + 1;
                } else {
                    s8 room = home.roomNo;
                    dComIfGp_get(); /* dComIfGp_roomControl_checkStatusFlag(fopAcM_GetHomeRoomNo(this), 0x10) */
                    if (!(gabi::load<u8>(0x1047E8E8 + room * 0x22C) & 0x10)) {
                        u32 a = gabi::ea(this); /* current = home; shape_angle = home.angle */
                        u16 ax = gabi::load<u16>(a + 0x2F8);
                        u16 ay = gabi::load<u16>(a + 0x2FA);
                        gabi::store<u16>(a + 0x328, ax);
                        gabi::store<u32>(a + 0x31C, gabi::load<u32>(a + 0x2F4));
                        gabi::store<u32>(a + 0x320, gabi::load<u32>(a + 0x2F8));
                        gabi::store<u32>(a + 0x324, gabi::load<u32>(a + 0x2FC));
                        speedF = 0.0f;
                        m4E8 = 0;
                        gabi::store<u16>(a + 0x32A, ay);
                        gabi::store<u32>(a + 0x318, gabi::load<u32>(a + 0x2F0));
                        gabi::store<u32>(a + 0x314, gabi::load<u32>(a + 0x2EC));
                        gabi::store<u16>(a + 0x32C, gabi::load<u16>(a + 0x2FC));
                    } else {
                        /* daPy_getPlayerLinkActorClass()->npcStartRestartRoom() */
                        gabi::call(0x023FD4E4, gabi::load<u32>(dComIfGp_ea() + 0x5B34), 5, 0xC9, -1.0f, 0);
                    }
                }
            } else {
                m4E8 = 0;
            }
            if (mAcch.m_flags & 0x1000 /* ChkWaterIn() */) {
                gabi::call(0x023FD4E4, gabi::load<u32>(dComIfGp_ea() + 0x5B34), 5, 0xC9, -1.0f, 0); /* npcStartRestartRoom */
                if (!(field_0x784 & 4) /* !isWaterHit() */) {
                    field_0x784 = field_0x784 | 4; /* onWaterHit() */
                    u32 splash = gabi::ea(dPa_control_set(dComIfGp_getParticle(), 0, 0x40 /* ID_IT_JN_WP_SHIBUKI */, &current.pos,
                                                          nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr));
                    if (splash) {
                        gabi::store<f32>(splash + 0x34, r15); /* setRate(15.0f) */
                        os_setEmitterScale(splash, 0x10467F08 /* splash_scale */);
                    }
                    /* dComIfGp_particle_setSingleRipple(ID_IT_JN_WP_HAMON01, &current.pos) */
                    u32 ripple = gabi::ea(dPa_control_set(dComIfGp_getParticle(), 5, 0x3D, &current.pos, nullptr, nullptr, 0xFF,
                                                          gabi::at<dPa_levelEcallBack>(0x1047B2E4), -1, nullptr, nullptr, nullptr));
                    if (ripple) {
                        os_setEmitterScale(ripple, 0x10467F14 /* ripple_scale */);
                    }
                }
            }
        }
    } else {
        mAcch.CrrPos(dComIfG_Bgsp());
        f32 gh = os_groundH(this);
        field_0x784 = field_0x784 | 0x10;
        if (gh != -1000000000.0f) {
            gabi::store<u8>(gabi::ea(this) + 0x1C9, (u8)gabi::call<s32>(0x024EF130, dComIfG_Bgsp(), os_gnd(this)));
            gabi::store<u8>(gabi::ea(this) + 0x1CA, (u8)gabi::call<u32>(0x024EEEB8, dComIfG_Bgsp(), os_gnd(this)));
        }
    }

    if (!(actor_status & 0x2000)) {
        gabi::call(0x022AB2BC, this); /* setCollision() */
    }
    if (gabi::call<BOOL>(0x02445950, this, gabi::load<u32>(0x101C28E4 + 4 * argument)) /* checkNowPosMove */) {
        gabi::call(0x022AB488, this); /* animationPlay() */
    }

    if (!eventProc()) {
        if (!gabi::call<BOOL>(0x022A9EAC, this) /* finishCheck() */) {
            if (dComIfGp_getPlayer(0) == this) {
                u32 st = actor_status;
                u32 flg = m4E4;
                actor_status = (st & ~0x3Fu) | 0x33; /* fopAcM_SetStatusMap(this, 0x13) (HD 0x33) */
                if (flg & 1) { /* isReturnLink() */
                    field_0x7A5 = 0;
                } else {
                    playerAction(nullptr);
                    if (returnLinkCheck()) {
                        m4E4 = m4E4 | 1; /* returnLink() */
                    }
                }
            } else {
                if (gabi::call<BOOL>(0x022A9E0C, this) /* wakeupCheck() */) {
                    actor_status = (actor_status & ~0x3Fu) | 0x2E;
                }
                carryCheck();
                checkOrder();
                npcAction(nullptr);
                if (!os_isPmf(&mNpcAction, 0x022AEF70 /* throwNpcAction */)) {
                    current.angle.y = shape_angle.y;
                }
            }
            checkGoalRoom();
        }
        if (field_0x78C == 2 || field_0x78C == 4) {
            makeBeam(0);
        } else {
            os_endBeam(this);
        }
        eventOrderCheck();
    }

    eventOrder();
    gabi::call(0x022AA464, this); /* setBaseMtx() */
    if (field_0x738.mpBaseEmitter != nullptr || field_0x740.mpBaseEmitter != nullptr) {
        s32 reverb = dComIfGp_getReverb(current.roomNo); /* fopAcM_seStartCurrent(JA_SE_OBJ_OSTATUE_LIGHT_SUS) */
        mDoAud_seStart(0x7046, &current.pos, 0, reverb);
    }
    os_copy_tevstr(this);
    return TRUE;
}
VERIFY(0x022AC1DC, &daNpc_Os_c::execute);

/* 022AD210 */
static BOOL daNpc_Os_IsDelete(daNpc_Os_c*) {
    WWHD_FUNC(0x022AD210, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x022AD210, daNpc_Os_IsDelete);
