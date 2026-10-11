/**
 * d_a_npc_bm1_b.cpp (WWHD)
 * NPC - Rito (generic Ritos on Dragon Roost Island): look-back, movement, execute/draw,
 * animation attributes, messages, status/action functions
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_bm1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * Part B: 021FFFFC .. 02204358 (the rest is in d_a_npc_bm1.cpp).
 */
#include "d/actor/d_a_npc_bm1_b.h"

// HD: part A owns global gc_tu; behavior-only vtables stay private to this source.
static const gc_tu_data l_behaviorTu = {
    .objAcchDt = {0x10016FE8, 0x10016FF8},
    .gndChk = {0x10016FA8, 0x10016FB8, 0x10016FD8, 0x10016FC8},
    .gndChkDt = {0x10016FB8, 0x10016FD8, 0x10016F98},
    .headerStatics = {0x104661D4, 0x101BCF34, 0},
};

static gptr<Bm1BehaviorMessage>& l_messageManager =
    gabi::object_at<gptr<Bm1BehaviorMessage>>(0x101F4B5C);

static Bm1BehaviorMessageCamera& messageCamera() {
    return gabi::object_at<Bm1BehaviorMessageCamera>(dComIfGp_ea() + 0x5C30);
}

static be<u32>& l_counter = gabi::object_at<be<u32>>(0x101FF558);
static inline bool dPath_ChkClose(dPath* p) {
    return (gabi::object_at<Bm1BehaviorPath>(gabi::ea(p)).mLoop & 1) != 0;
}

/* dBgS_GndChk on the stack (0x54), this TU's vtables */

/* (this->*pmf)(arg): GHS pointer to member (8 bytes; i < 0: not virtual) */
// HD: load the virtual index before the this adjustment, as GHS does.
static inline void pmf_call(void* self, ProcFunc_l* action, void* arg) {
    Bm1BehaviorAction* pmf = &gabi::object_at<Bm1BehaviorAction>(gabi::ea(action));
    s16 i = pmf->i;
    u32 thisp = gabi::ea(self) + (s32)(s16)pmf->d;
    if (i < 0) {
        gabi::call_ptr<BOOL>(pmf->f, thisp, arg);
    } else {
        s16 voff = gabi::object_at<Bm1BehaviorPtmf>(gabi::ea(pmf)).mVtableOffset;
        u32 vt = gabi::object_at<be<u32>>(thisp + voff);
        gabi::call_ptr<BOOL>(
            gabi::object_at<Bm1BehaviorVirtualSlot>(vt + i * sizeof(Bm1BehaviorVirtualSlot))
                .mFunction,
            thisp, arg);
    }
}
/* function-local static GXColor initialised on first use (guard word, then a memcpy from .data) */
static inline void static_color_init(u32 guard, u32 obj, u32 src) {
    be<u32>& initialized = gabi::object_at<be<u32>>(guard);
    if (initialized == 0) {
        initialized = 1;
        memcpy_g(&gabi::object_at<GXColor>(obj), &gabi::object_at<GXColor>(src),
                 4); /* 028FEAC0 memcpy */
    }
}
/* fsel: a >= 0 ? c : b (NaN: b) -- cLib_maxLimit / cLib_minLimit as GHS compiled them */
static inline f32 fsel(f32 a, f32 c, f32 b) { return a >= 0.0f ? c : b; }

static Bm1BehaviorHioPrm a_prm_tbl[10] = {
    {8192, 4000, -2500, -4000, 0,    7000, -1000,  -7000, 2048, 1200, 220.0f, 0,    {},    10,
     1024, {},   0.0f,  0.0f,  0.0f, 0.0f, 180.0f, 6,     1024, 0.5f, 3.0f,   0.1f, 40.0f, 870.0f},
    {8192, 8192, -1861, -8192, 0,    4150, 0,    -4150, 1145, 0,    209.0f, 0,    {},   0,
     0,    {},   0.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0,     0,    0.0f, 0.0f,   0.0f, 0.0f, 0.0f},
    {8192, 8192, -1861, -8192, 0,    4150, 0,    -4150, 1145, 0,    209.0f, 0,    {},   0,
     0,    {},   0.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0,     0,    0.0f, 0.0f,   0.0f, 0.0f, 0.0f},
    {8192, 8192, -4096, -8192, 0,    4096, 0,    -4096, 1400, 0,    220.0f, 0,    {},   0,
     0,    {},   0.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0,     0,    0.0f, 0.0f,   0.0f, 0.0f, 0.0f},
    {8192, 8192, -4096, -8192, 0,    4096, 0,    -4096, 1400, 0,    220.0f, 0,    {},   0,
     0,    {},   0.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0,     0,    0.0f, 0.0f,   0.0f, 0.0f, 0.0f},
    {8192, 8192, -6726, -8192, 0,    4096, 0,    -4096, 1288, 787,  220.0f, 0,    {},   0,
     0,    {},   0.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0,     0,    0.0f, 0.0f,   0.0f, 0.0f, 0.0f},
    {8192, 8192, -4096, -8192, 0,     4096, 0,      -4096, 1000, 1430, 220.0f, 0,    {},    10,
     1024, {},   20.0f, 1.0f,  20.0f, 0.6f, 180.0f, 6,     1024, 0.3f, 4.0f,   0.3f, 40.0f, 0.0f},
    {8192, 8192, -4096, -8192, 0,    8192, 0,    -8192, 1600, 1400, 220.0f, 0,    {},    0,
     0,    {},   0.0f,  0.0f,  0.0f, 0.0f, 0.0f, 6,     1024, 0.3f, 4.0f,   0.3f, 40.0f, 0.0f},
    {8192, 8192, -4096, -8192, 0,    8192, 0,    -8192, 1600, 1400, 220.0f, 0,    {},    0,
     0,    {},   0.0f,  0.0f,  0.0f, 0.0f, 0.0f, 6,     1024, 0.3f, 4.0f,   0.3f, 40.0f, 0.0f},
    {8192, 8192, -4096, -8192, 0,     4096, 0,      -4096, 1600, 1200, 220.0f, 0,    {},    10,
     1024, {},   20.0f, 1.0f,  20.0f, 0.3f, 180.0f, 6,     1024, 0.5f, 4.0f,   0.3f, 40.0f, 0.0f},
};
static Bm1BehaviorHio& l_HIO = gabi::object_at<Bm1BehaviorHio>(0x1046623C);
/* l_HIO.children[mType - 1].hio_prm */
static Bm1BehaviorHioPrm& hio_prm(s8 type) { return l_HIO.children[type - 1].hio_prm; }

static daNpc_Bm1_c::anm_prm_c a_status_anm_prm_tbl[19] = {
    {-1, -1, {}, 0.0f, 0.0f, -1, -1}, {0, 0, {}, 0.0f, 1.0f, 2, 0},
    {2, 0, {}, 8.0f, 1.0f, 0, 0},     {3, 0, {}, 6.0f, 1.7999999523162842f, 2, 0},
    {5, 0, {}, 8.0f, 1.0f, 2, 1},     {-1, -1, {}, 0.0f, 0.0f, -1, -1},
    {9, 0, {}, 8.0f, 1.0f, 2, 0},     {15, 0, {}, 8.0f, 1.0f, 2, 1},
    {15, 0, {}, 8.0f, 1.0f, 2, 1},    {-1, -1, {}, 0.0f, 0.0f, -1, -1},
    {10, 0, {}, 8.0f, 1.0f, 0, 1},    {-1, -1, {}, 0.0f, 0.0f, -1, -1},
    {4, 0, {}, 8.0f, 1.0f, 2, 1},     {5, 0, {}, 8.0f, 1.0f, 2, 1},
    {4, 0, {}, 8.0f, 1.0f, 2, 1},     {5, 0, {}, 8.0f, 1.0f, 2, 1},
    {17, 0, {}, 8.0f, 1.0f, 2, 1},    {20, 0, {}, 8.0f, 1.0f, 2, 1},
    {21, 0, {}, 8.0f, 1.0f, 2, 1},
};

static daNpc_Bm1_c::anm_prm_c a_attribute_anm_prm_tbl[15] = {
    {4, 0, {}, 8.0f, 1.0f, 2, 1},
    {1, 0, {}, 8.0f, 1.0f, 2, 0},
    {0, 0, {}, 6.0f, 1.0f, 2, 0},
    {2, 0, {}, 8.0f, 1.0f, 0, 0},
    {3, 0, {}, 8.0f, 1.7999999523162842f, 2, 0},
    {5, 0, {}, 12.0f, 1.0f, 2, 1},
    {6, 0, {}, 8.0f, 1.0f, 2, 1},
    {7, 0, {}, 8.0f, 1.0f, 2, 1},
    {7, 0, {}, 5.0f, 1.0f, 2, 1},
    {12, 0, {}, 8.0f, 1.0f, 2, 1},
    {15, 0, {}, 12.0f, 1.0f, 2, 1},
    {16, 0, {}, 12.0f, 1.0f, 2, 1},
    {17, 0, {}, 12.0f, 1.0f, 2, 1},
    {6, 0, {}, 8.0f, 1.0f, 2, 1},
    {18, 0, {}, 8.0f, 1.0f, 2, 1},
};

/* getMsg_BMB_0/BMB_1/BMC_3/BMD_0 share one shape: a message per spawn condition (1..4),
 * depending on whether this Rito's bit is already set in event register 0xB8FF */
static u32 getMsg_spawnCond(daNpc_Bm1_c* i_this, u16 known1, u16 known2, u16 known3, u16 known4,
                            u16 unknown1, u16 unknown2, u16 unknown3, u16 unknown4) {
    s8 reg = dComIfGs_getEventReg(0xB8FF);
    s32 mask = i_this->getBitMask();
    u32 msg = 0;
    bool known = (reg & mask) != 0;
    switch (i_this->mSpawnCondition) {
    case 1:
        msg = known ? known1 : unknown1;
        break;
    case 2:
        msg = known ? known2 : unknown2;
        break;
    case 3:
        msg = known ? known3 : unknown3;
        break;
    case 4:
        msg = known ? known4 : unknown4;
        break;
    }
    if (msg != 0) {
        dComIfGs_setEventReg(0xB8FF, (u8)(reg | mask));
    }
    return msg;
}

//(unnamed by the matcher)

/* 02201258 */
bool daNpc_Bm1_c::setAnm() {
    anm_prm_c* a_anm_prm_tbl = a_status_anm_prm_tbl;
    if (a_anm_prm_tbl[mStatus].btpNum >= 0) {
        setAnm_tex(a_anm_prm_tbl[mStatus].btpNum);
    }
    if (a_anm_prm_tbl[mStatus].anmNum >= 0) {
        setAnm_anm(&a_anm_prm_tbl[mStatus]);
    }
    return true;
}

/* 02200AB4 */
void daNpc_Bm1_c::setPlaySpd(f32 i_speed) {
    if (mbHasArms) {
        mpArmMorf->setPlaySpeed(i_speed);
        mpHeadMorf->setPlaySpeed(i_speed);
        mpMorf->setPlaySpeed(i_speed);
    } else {
        mpWingMorf->setPlaySpeed(i_speed);
        mpHeadMorf->setPlaySpeed(i_speed);
        mpMorf->setPlaySpeed(i_speed);
    }
}

/* 0220134C */
void daNpc_Bm1_c::chg_anmAtr(u8 i_param_1) {
    if (i_param_1 >= 0xF || i_param_1 == m8F7) {
        return;
    }
    m8F7 = i_param_1;
    if (mSpecificType == SPECIFIC_TYPE_Skett_e || mSpecificType == SPECIFIC_TYPE_Akoot_e) {
        switch (m8F7) {
        case 9:
            mLookBackState = 1;
            m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
            break;
        case 0xD:
            if (mPartnerProcID != 0xFFFFFFFF) {
                fopAc_ac_c* actor = searchByID(mPartnerProcID);
                m82C.copy(actor->eyePos);
                mLookBackState = 2;
            }
            break;
        }
    }
    setAnm_ATR(1);
}

/* 0220141C */
void daNpc_Bm1_c::control_anmAtr() {
    switch (m8F7) {
    case 8:
        if (mbMorfAnimStopped) {
            m87B = m87B + 1;
            if (m87B >= 2) {
                m8F7 = 6;
                setAnm_NUM(0x16, 1);
            }
        }
        break;
    }
}

/* 022012DC */
void daNpc_Bm1_c::setAnm_ATR(int i_param_1) {
    anm_prm_c* a_anm_prm_tbl = a_attribute_anm_prm_tbl;
    if (i_param_1 != 0) {
        setAnm_tex(a_anm_prm_tbl[m8F7].btpNum);
    }
    setAnm_anm(&a_anm_prm_tbl[m8F7]);
}

/* 02201460 */
void daNpc_Bm1_c::anmAtr(u16 i_param_1) {
    Bm1BehaviorMessageCamera& caminfo = messageCamera();
    if (mbManzai != 0 && this != caminfo.mActor[(s32)caminfo.mBasicID - 1].get()) {
        /* control_anmTag(): empty */
        control_anmAtr();
        return;
    }
    switch (i_param_1) {
    case 6: {
        if (m905 == 0) {
            m8F7 = 0xFF;
            chg_anmAtr(dComIfGp_getMesgAnimeAttrInfo());
            m905 = m905 + 1;
        }
        u8 tag = dComIfGp_getMesgAnimeTagInfo(); /* dComIfGp_getMesgAnimeTagInfo() */
        dComIfGp_clearMesgAnimeTagInfo();        /* dComIfGp_clearMesgAnimeTagInfo() */
        if (tag != 0xFF && m8F8 != tag) {
            m8F8 = tag;
            /* chg_anmTag(): empty */
        }
        break;
    }
    case 0xE:
        m905 = 0;
        break;
    }
    /* control_anmTag(): empty */
    control_anmAtr();
}

/* 02200CD0 */
void daNpc_Bm1_c::eventOrder() {
    s8 condition = m8FD;
    if (condition == 1 || condition == 2) {
        eventInfo_onCondition(this, dEvtCnd_CANTALK_e); /* dEvtCnd_CANTALK_e */
        if (mSpecificType == SPECIFIC_TYPE_Hoskit_e) {
            eventInfo_onCondition(this, dEvtCnd_CANTALKITEM_e); /* dEvtCnd_CANTALKITEM_e */
        }
        if (m8FD == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (condition >= 3) {
        mEventIdx = condition - 3;
        fopAcM_orderOtherEventId(this, mEventIdTable[mEventIdx], 0xFF, 0xFFFF, 0, 1);
    }
}

/* 02201558 */
u8 daNpc_Bm1_c::chk_manzai() {
    u8 o_retval = 1;
    if (mPartnerProcID != 0xFFFFFFFF) {
        daNpc_Bm1_c* actor = static_cast<daNpc_Bm1_c*>(searchByID(mPartnerProcID));
        if (actor == nullptr) {
            return 0;
        }
        mbManzai = actor->mStatus == 9;
        if (mbManzai) {
            /* dComIfGp_setMesgCameraInfoActor(this, actor, NULL x8) */
            Bm1BehaviorMessageCamera& ci = messageCamera();
            ci.mActor[0] = this;
            ci.mActor[1] = actor;
            for (int i = 2; i < 10; i++)
                ci.mActor[i] = nullptr;
        } else {
            actor->actor_status |= fopAcStts_UNK4000_e; /* fopAcM_OnStatus(actor, fopAcStts_UNK4000_e) */
            actor->mbManzai = 1;           /* set_manzai() */
            o_retval = 0;
        }
    }
    return o_retval;
}

/* 0220163C */
bool daNpc_Bm1_c::chk_talk() {
    bool ret = true;
    m87D = 0xFF;
    if (dComIfGp_event_chkTalkXY()) {
        if (dComIfGp_evmng_ChkPresentEnd()) {
            m87D = dComIfGp_event_getPreItemNo();
        } else {
            ret = false;
        }
    }
    return ret;
}

/* 022016BC */
u8 daNpc_Bm1_c::chk_partsNotMove() {
    return m85C == m_jnt.mAngles[0][1] && m85E == m_jnt.mAngles[1][1] && m860 == current.angle.y;
}

/* 021FFFFC */
void daNpc_Bm1_c::lookBack() {
    cXyz dstPos;
    cXyz* dstPos_p;
    s16 desiredYrot = current.angle.y;
    m860 = desiredYrot;
    m85E = m_jnt.mAngles[1][1];
    dstPos.set(0.0f, 0.0f, 0.0f);
    f32 srcY = eyePos.y;
    f32 srcX = current.pos.x;
    u8 headOnlyFollow = m896;
    dstPos_p = nullptr;
    f32 srcZ = current.pos.z;
    m85C = m_jnt.mAngles[0][1];

    switch ((u32)(s32)mLookBackState) {
    case 1: {
        cXyz eye;
        Bm1Behavior_playerEyePos(&eye, -20.0f);
        dstPos.copy(eye);
        srcX = current.pos.x;
        srcZ = current.pos.z;
        srcY = eyePos.y;
        dstPos_p = &dstPos;
        break;
    }
    case 2:
        dstPos.copy(m82C);
        srcX = current.pos.x;
        srcY = eyePos.y;
        srcZ = current.pos.z;
        dstPos_p = &dstPos;
        break;
    case 3:
        desiredYrot = m876;
        break;
    }
    cLib_addCalcAngleS2(&mHeadLookAtMaxVel, hio_prm(mType).mCalcAngleTarget, 4, 0x800);
    if (!m_jnt.mbTrn) { /* !m_jnt.trnChk() */
        mHeadLookAtMaxVel = 0;
    }
    cXyz src_pos; /* passed by value: a copy on the stack */
    src_pos.x = srcX;
    src_pos.y = srcY;
    src_pos.z = srcZ;
    Bm1Behavior_lookAtTarget(&m_jnt, &current.angle.y, dstPos_p, &src_pos, desiredYrot,
                             mHeadLookAtMaxVel, headOnlyFollow);
}

/* 022016FC */
u16 daNpc_Bm1_c::next_msgStatus(be<u32>* i_msg_no) {
    u16 msg_status = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    // HD: choices come from the global message manager rather than mpCurrMsg.
    Bm1BehaviorMessage* msg = l_messageManager;
    switch (*i_msg_no) {
    case 0x1771:
        *i_msg_no = 0x1772;
        break;
    case 0x1772:
        *i_msg_no = 0x1773;
        break;
    case 0x1773:
        *i_msg_no = 0x1774;
        break;
    case 0x1775:
        *i_msg_no = 0x1776;
        break;
    case 0x18A1:
        *i_msg_no = 0x18A2;
        break;
    case 0x5FE:
        *i_msg_no = 0x5FF;
        break;
    case 0x601:
        *i_msg_no = 0x602;
        break;
    case 0x602:
        *i_msg_no = 0x603;
        break;
    case 0x603:
        *i_msg_no = 0x604;
        break;
    case 0x604:
        *i_msg_no = 0x605;
        break;
    case 0x186B:
        *i_msg_no = 0x186C;
        break;
    case 0x186C:
        *i_msg_no = 0x186D;
        break;
    case 0x186D:
        *i_msg_no = 0x186E;
        break;
    case 0x1873:
        *i_msg_no = 0x1874;
        break;
    case 0x1874:
        *i_msg_no = 0x1875;
        break;
    case 0x1875:
        *i_msg_no = 0x1876;
        break;
    case 0x187B:
        *i_msg_no = 0x187C;
        break;
    case 0x187C:
        *i_msg_no = 0x187D;
        break;
    case 0x187D:
        *i_msg_no = 0x187E;
        break;
    case 0x1883:
        *i_msg_no = 0x1884;
        break;
    case 0x1884:
        *i_msg_no = 0x1885;
        break;
    case 0x1885:
        *i_msg_no = 0x1886;
        break;
    case 0x186F:
        *i_msg_no = 0x1870;
        break;
    case 0x1870:
        *i_msg_no = 0x1871;
        break;
    case 0x1871:
        *i_msg_no = 0x1872;
        break;
    case 0x1877:
        *i_msg_no = 0x1878;
        break;
    case 0x1878:
        *i_msg_no = 0x1879;
        break;
    case 0x1879:
        *i_msg_no = 0x187A;
        break;
    case 0x187F:
        *i_msg_no = 0x1880;
        break;
    case 0x1880:
        *i_msg_no = 0x1881;
        break;
    case 0x1881:
        *i_msg_no = 0x1882;
        break;
    case 0x1887:
        *i_msg_no = 0x1888;
        break;
    case 0x1888:
        *i_msg_no = 0x1889;
        break;
    case 0x1889:
        *i_msg_no = 0x188A;
        break;
    case 0x18A6:
        *i_msg_no = 0x18A7;
        break;
    case 0x18BC:
        *i_msg_no = 0x18BD;
        break;
    case 0x1996:
        *i_msg_no = 0x1997;
        break;
    case 0x1998:
        *i_msg_no = 0x1999;
        break;
    case 0x199E:
        *i_msg_no = 0x199F;
        break;
    case 0x199F:
        if (dComIfGs_getBeastNum(Bm1GoldenFeather) >= 20) {
            *i_msg_no = 0x19A1;
        } else {
            *i_msg_no = 0x19A0;
        }
        break;
    case 0x19A1:
        switch (msg->mSelectNum /* mSelectNum */) {
        case 0:
            *i_msg_no = 0x19A3;
            dComIfGp_setItemBeastNumCount(Bm1GoldenFeather, -20);
            break;
        case 1:
            *i_msg_no = 0x19A2;
            break;
        }
        break;
    case 0x19A3:
        *i_msg_no = 0x19A4;
        break;
    case 0x1EF0:
        *i_msg_no = 0x1EF1;
        break;
    case 0x1EF1:
        *i_msg_no = 0x1EF2;
        break;
    case 0x1EF2:
        *i_msg_no = 0x1EF3;
        break;
    case 0x1EF3:
        *i_msg_no = 0x1EF4;
        break;
    case 0x1EF4:
        *i_msg_no = 0x1EF5;
        break;
    case 0x1EF5:
        switch (msg->mSelectNum /* mSelectNum */) {
        case 0:
            *i_msg_no = 0x1EF6;
            break;
        case 1:
            *i_msg_no = 0x1EF8;
            break;
        }
        break;
    default:
        msg_status = 0x10; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return msg_status;
}

/* 02201C70 */
s32 daNpc_Bm1_c::getBitMask() {
    s8 o_retval = 0;
    switch (mSpecificType) {
    case SPECIFIC_TYPE_Basht_e:
        o_retval = 1;
        break;
    case SPECIFIC_TYPE_Bisht_e:
        o_retval = 2;
        break;
    case SPECIFIC_TYPE_Pashli_e:
        o_retval = 4;
        break;
    case SPECIFIC_TYPE_Namali_e:
        o_retval = 8;
        break;
    case SPECIFIC_TYPE_Kogoli_e:
        o_retval = 0x10;
        break;
    }
    return o_retval;
}

/* 02201CC4 */
u32 daNpc_Bm1_c::getMsg_PST_1() { return dComIfGs_isEventBit(0x1401) ? 0xC93 : 0xC92; }

/* 02201D04 */
u32 daNpc_Bm1_c::getMsg_PST_3() { return dComIfGs_isEventBit(0x2202) ? 0x18A3 : 0x18A1; }

/* 02201D44 */
u32 daNpc_Bm1_c::getMsg_SKT_0() {
    if (dComIfGs_isSymbol(1 /* dSymbol_DIN_e */)) {
        if (!dComIfGs_isEventBit(0x1A80)) {
            return 0x186B;
        } else if (!dComIfGs_isEventBit(0x1820)) {
            return 0x1873;
        } else {
            return !dComIfGs_isEventBit(0x2E04) ? 0x187B : 0x1883;
        }
    }
    return 0;
}

/* 02201E1C */
u32 daNpc_Bm1_c::getMsg_KKT_0() {
    if (dComIfGs_isSymbol(1 /* dSymbol_DIN_e */)) {
        if (!dComIfGs_isEventBit(0x1A80)) {
            return 0x186F;
        } else if (!dComIfGs_isEventBit(0x1820)) {
            return 0x1877;
        } else {
            return !dComIfGs_isEventBit(0x2E04) ? 0x187F : 0x1887;
        }
    }
    return 0;
}

/* 02201EF4 */
u32 daNpc_Bm1_c::getMsg_BMB_0() {
    return getMsg_spawnCond(this, 0x18A0, 0x18B1, 0x18B3, 0x18B5, 0x189F, 0x18B0, 0x18B2, 0x18B4);
}

//(unnamed by the matcher)
/* 02201FF0 */
u32 daNpc_Bm1_c::getMsg_BMB_1() {
    return getMsg_spawnCond(this, 0x18A9, 0x18BF, 0x18C1, 0x18C3, 0x18A8, 0x18BE, 0x18C0, 0x18C2);
}

/* 022020EC */
u32 daNpc_Bm1_c::getMsg_BMB_2() {
    if (m87C != 0) {
        return 0x199C;
    }
    if (m87D == 0x47) {
        if (dComIfGs_isEventBit(0x2180)) {
            return 0x199D;
        }
        if (dComIfGs_isEventBit(0x2140)) {
            if (dComIfGs_getBeastNum(Bm1GoldenFeather) >= 20) {
                return 0x19A1;
            } else {
                return 0x19A0;
            }
        }
        dComIfGs_onEventBit(0x2140);
        return 0x199E;
    } else if (m87D != 0xFF) {
        return 0x19A5;
    }
    if (dComIfGs_isEventBit(0x2180)) {
        return 0x199B;
    }
    if (dComIfGs_isEventBit(0x2001)) {
        return 0x199A;
    }
    if (dComIfGs_isEventBit(0x2120)) {
        return 0x1998;
    }
    return 0x1996;
}

/* 02202280 */
u32 daNpc_Bm1_c::getMsg_BMC_0() {
    switch (mSpawnCondition) {
    case 1:
        return 0x18A6;
    case 2:
        return 0x18BA;
    case 3:
        return 0x18BC;
    case 4:
        break;
    }
    return 0;
}

/* 022022B4 */
u32 daNpc_Bm1_c::getMsg_BMC_2() {
    if (m87C != 0) {
        return 0x1EF7;
    }
    if (dComIfGs_isGetItemReserve(0xF)) {
        return 0x1EF9;
    }
    return dComIfGs_isEventBit(0x1908) ? 0x1EF2 : 0x1EF0;
}

//(unnamed by the matcher)
/* 02202354 */
u32 daNpc_Bm1_c::getMsg_BMC_3() {
    return getMsg_spawnCond(this, 0x18C5, 0x18C7, 0x18C9, 0x18CB, 0x18C4, 0x18C6, 0x18C8, 0x18CA);
}

//(unnamed by the matcher)
/* 02202450 */
u32 daNpc_Bm1_c::getMsg_BMD_0() {
    return getMsg_spawnCond(this, 0x189E, 0x18AB, 0x18AD, 0x18AF, 0x189D, 0x18AA, 0x18AC, 0x18AE);
}

/* 0220254C */
u32 daNpc_Bm1_c::getMsg_BMD_1() {
    s8 reg = dComIfGs_getEventReg(0xB8FF);
    s32 mask = getBitMask();
    u32 msg = 0;
    if (reg & mask) {
        if (!dComIfGs_isEventBit(0x1820)) {
            msg = 0x18A5;
        } else {
            switch (mSpawnCondition) {
            case 1:
            case 2:
                msg = 0x18A5;
                break;
            case 3:
                msg = 0x18B7;
                break;
            case 4:
                msg = 0x18B9;
                break;
            }
        }
    } else {
        if (!dComIfGs_isEventBit(0x1820)) {
            msg = 0x18A4;
        } else {
            switch (mSpawnCondition) {
            case 1:
            case 2:
                msg = 0x18A4;
                break;
            case 3:
                msg = 0x18B6;
                break;
            case 4:
                msg = 0x18B8;
                break;
            }
        }
    }
    if (msg != 0) {
        dComIfGs_setEventReg(0xB8FF, (u8)(reg | mask));
    }
    return msg;
}

/* 0220269C */
u32 daNpc_Bm1_c::getMsg() {
    u32 o_retval = 0;
    switch (mSpecificType) {
    case SPECIFIC_TYPE_Quill_1_e:
        o_retval = getMsg_PST_1();
        break;
    case SPECIFIC_TYPE_Quill_3_e:
        o_retval = getMsg_PST_3();
        break;
    case SPECIFIC_TYPE_Skett_e:
        o_retval = getMsg_SKT_0();
        break;
    case SPECIFIC_TYPE_Akoot_e:
        o_retval = getMsg_KKT_0();
        break;
    case SPECIFIC_TYPE_Basht_e:
        o_retval = getMsg_BMB_0();
        break;
    case SPECIFIC_TYPE_Bisht_e:
        o_retval = getMsg_BMB_1();
        break;
    case SPECIFIC_TYPE_Hoskit_e:
        o_retval = getMsg_BMB_2();
        break;
    case SPECIFIC_TYPE_Ilari_0xA_e:
        o_retval = getMsg_BMC_0();
        break;
    case SPECIFIC_TYPE_Ilari_0xC_e:
        o_retval = getMsg_BMC_2();
        break;
    case SPECIFIC_TYPE_Pashli_e:
        o_retval = getMsg_BMC_3();
        break;
    case SPECIFIC_TYPE_Namali_e:
        o_retval = getMsg_BMD_0();
        break;
    case SPECIFIC_TYPE_Kogoli_e:
        o_retval = getMsg_BMD_1();
        break;
    }
    return o_retval;
}

/* 022027B0 */
u8 daNpc_Bm1_c::chkAttention() {
    dAttention_c* attention = dComIfGp_getAttention();
    if (Bm1Behavior_lockonTruth(attention)) {
        return this == (void*)Bm1Behavior_lockonTarget(attention, 0);
    }
    return this == (void*)Bm1Behavior_actionTarget(attention, 0);
}

/* 0220057C */
u32 daNpc_Bm1_c::bm_movPass(u32 i_param_1) {
    u8 point_idx = 0;
    u32 o_retval = 0;
    if (mPathRun.mPath.get() != nullptr &&
        (point_idx = mPathRun.mIdx, dPath_ChkClose(mPathRun.mPath))) {
        cXyz pos; /* passed by value */
        pos.x = current.pos.x;
        pos.y = current.pos.y;
        pos.z = current.pos.z;
        if (!Bm1Behavior_pathPassed(&mPathRun, &pos, mPathRun.mbDir != 0)) {
            return o_retval;
        }
        Bm1Behavior_pathNext(&mPathRun);
        o_retval = 1;
        if (i_param_1 == 0) {
            return o_retval;
        }
    } else {
        cXyz diff;
        cXyz_mi(&mTargetPos, &diff, &current.pos);
        cXyz xz; /* absXZ(): cXyz(x, 0, z).abs() */
        xz.x = diff.x;
        xz.y = 0.0f;
        xz.z = diff.z;
        f32 dist_xz = std_sqrtf(PSVECSquareMag(&xz));
        if (!(dist_xz > m858)) { /* dist_xz <= m858 (GHS: NaN passes) */
            o_retval = 1;
            // HD: without a path the pass flag is not updated (GameCube: also with i_param_1)

            if (mPathRun.mPath.get() == nullptr) {
                return o_retval;
            }
            if ((s32)point_idx < Bm1Behavior_pathMax(&mPathRun) - 2) {
                Bm1Behavior_pathNext(&mPathRun);
            } else {
                o_retval = 2;
            }
            if (i_param_1 == 0) {
                return o_retval;
            }
        } else {
            return o_retval;
        }
    }
    cXyz pt;
    Bm1Behavior_pathPoint(&mPathRun, &pt, mPathRun.mIdx);
    f32 idxY = pt.y;
    Bm1Behavior_pathPoint(&mPathRun, &pt, point_idx);
    m889 = !(pt.y > idxY); /* cmpCoords.y <= idxCoords.y (GHS: NaN gives true) */
    return o_retval;
}

/* 02200420 */
void daNpc_Bm1_c::bm_clcFlySpd() {
    if (m8F4 == 2 || m8F4 == 5) {
        s16 target = cLib_targetAngleY(&current.pos, &mTargetPos);
        Bm1BehaviorHioPrm& prm = hio_prm(mType);
        cLib_addCalcAngleS(&current.angle.y, target, prm.mFlyScale, prm.mFlyMaxStep, 0);
    }
    switch ((u32)(s32)m8F4) {
    case 3:
        speed.y = speed.y - mFlyAccelY;
        break;
    case 2:
    case 5:
        if (m889 != 0) {
            if (mTargetPos.y > current.pos.y) {
                f32 v = speed.y + mFlyAccelY; /* cLib_maxLimit(v, mFlySpeedY) */
                f32 max = mFlySpeedY;
                speed.y = fsel(v - max, max, v);
            } else {
                f32 v = speed.y - mFlyAccelY; /* cLib_minLimit(v, 0.0f) */
                speed.y = fsel(v, v, 0.0f);
            }
        } else if (mTargetPos.y > current.pos.y) {
            f32 v = speed.y + mFlyAccelY; /* cLib_maxLimit(v, 0.0f) */
            speed.y = fsel(v, 0.0f, v);
        } else {
            speed.y = speed.y - mFlyAccelY;
        }
        break;
    }
    cLib_chaseF(&speedF, mTargetFlySpeed, mTargetFlyStep);
}

/* 02200A40 */
void daNpc_Bm1_c::bm_clcMovSpd() {
    s16 target = cLib_targetAngleY(&current.pos, &mTargetPos);
    Bm1BehaviorHioPrm& prm = hio_prm(mType);
    cLib_addCalcAngleS(&current.angle.y, target, prm.m34, prm.m36, 0);
    cLib_chaseF(&speedF, mTargetFlySpeed, mTargetFlyStep);
}

/* 02200714 */
bool daNpc_Bm1_c::bm_flyMove() {
    if (m8F4 == 0) {
        return false;
    }
    if (m88A != 0) {
        m887 = 1;
        return true;
    }
    if (mPathRun.mPath.get() != nullptr) {
        cXyz pt;
        Bm1Behavior_pathPoint(&mPathRun, &pt, mPathRun.mIdx);
        mTargetPos.copy(pt);
    }
    switch ((u32)(s32)m8F4) {
    case 1:
        if (mAnmNum != 0xA) {
            setAnm_NUM(0xA, 1);
        } else if (mbMorfAnimStopped) {
            m8F4 = 2;
        }
        break;
    case 2:
        if (mAnmNum != 0xB) {
            setAnm_NUM(0xB, 1);
            speed.y = mTargetFlySpeed * 0.7f;
        } else if (mbMorfAnimStopped) {
            m8F4 = 5;
            m88A = 1;
            bm_setFlyAnm();
        }
        [[fallthrough]];
    case 5: {
        bm_clcFlySpd();
        u32 passResult = bm_movPass(1);
        switch (passResult) {
        case 1:
            if (m8F4 != 2) {
                bm_setFlyAnm();
                m8F4 = 5;
            }
            m88A = 1;
            break;
        case 2:
            mTargetFlySpeed = 0.0f;
            m88A = 1;
            m8F4 = 3;
            break;
        }
        break;
    }
    case 3:
        setAnm_NUM(9, 1);
        if (!mObjAcch.ChkGroundHit()) {
            bm_clcFlySpd();
        } else {
            speedF = 0.0f;
            speed.y = 0.0f;
            m8F4 = 4;
            m88A = 1;
        }
        break;
    case 4:
        if (mAnmNum != 0xD) {
            setAnm_NUM(0xD, 1);
        } else if (mbMorfAnimStopped) {
            gravity = -4.5f;
            setAnm_NUM(4, 1);
            m8F4 = 0;
            m88A = 1;
        }
        break;
    }
    return true;
}

/* 02200AF8 */
void daNpc_Bm1_c::bm_nMove() {
    if (m88B) {
        m887 = 1;
        return;
    }
    switch ((u32)(s32)mAnmNum) {
    case 0x14:
    case 0xE: {
        bm_clcMovSpd();
        f32 spd = speedF * hio_prm(mType).m38;
        if (spd < 0.5f) {
            spd = 0.5f;
        }
        setPlaySpd(spd);
        u32 passResult = bm_movPass(0);
        if (passResult >= 1 && passResult <= 2) {
            m88B = 1;
        }
        // HD: check ground below the next position and flag a drop larger than 50 units.
        dBgS_GndChk gnd;
        dBgS_GndChk_ct(&gnd, l_behaviorTu.gndChk, false);
        cXyz pos;
        pos.x = old.pos.x + speed.x;
        pos.y = old.pos.y + 50.0f;
        pos.z = old.pos.z + speed.z;
        dBgS_GndChk_SetPos(&gnd, &pos);
        f32 gndY = cBgS_GroundCross(dComIfG_Bgsp(), &gnd);
        if (gndY + 50.0f < current.pos.y) {
            mHD_A08 = 1;
        }
        /* ~dBgS_GndChk */

        gnd.cBgS_PolyInfo::__vtbl = l_behaviorTu.gndChkDt[0];
        gnd.dBgS_PolyPassChk::__vtbl = l_behaviorTu.gndChkDt[1];
        gnd.dBgS_GrpPassChk::__vtbl = l_behaviorTu.gndChkDt[2];
        Bm1Behavior_groundCheckDestroy(&gnd, 0); /* cBgS_GndChk::~cBgS_GndChk */
        break;
    }
    }
}

/* 0220026C */
void daNpc_Bm1_c::event_proc(int i_staff_idx) {
    if (dComIfGp_evmng_endCheck(mEventIdTable[mEventIdx])) {
        switch ((u32)(s32)mEventIdx) {
        case 0:
            m8FD = 1;
            m87C = 1;
            break;
        case 1:
            dComIfGs_onEventBit(0x1F40);
            fopAcM_delete(this);
            break;
        case 2:
            Bm1Behavior_sendLetter(0xAE03);
            dComIfGs_onEventBit(0x2180);
            break;
        case 3:
            dComIfGs_onEventBit(0x1E80);
            fopAcM_delete(this);
            break;
        }
        endEvent();
    } else {
        if (!mEventCut.cutProc()) {
            privateCut(i_staff_idx);
        }
        lookBack();
    }
}

/* 02202838 */
void daNpc_Bm1_c::setStt(s8 i_status) {
    s8 previous_status = mStatus;
    mStatus = i_status;
    m86E = 0;
    switch ((u32)(s32)i_status) {
    case 1:
        m870 = 0x50;
        break;
    case 3:
        m880 = 1;
        m870 = 0x5A;
        break;
    case 5:
        m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
        m8F7 = 0xFF;
        mLookBackState = 1;
        m8FF = previous_status;
        return;
    case 8:
        m870 = (s16)gabi::ftoi(cM_rndF(180.0f) + 180.0f);
        break;
    case 9:
        m8F7 = 0xFF;
        m8FF = previous_status;
        return;
    case 10: {
        cXyz offset;
        cXyz rel;
        mLookBackState = 0;
        offset.set(0.0f, 9999.0f, 100.0f);
        eInit_calcRelativPos(&rel, &offset, nullptr);
        mTargetFlySpeed = 0.0f;
        m870 = 0x5A;
        mTargetPos.copy(rel);
        mTargetFlyStep = 0.0f;
        Bm1BehaviorHioPrm& prm = hio_prm(mType);
        mFlySpeedY = prm.m28;
        mFlyAccelY = prm.m2C;
        speedF = 0.0f;
        m858 = prm.m30;
        speed.y = 0.0f;
        gravity = 0.0f;
        m889 = 1;
        m8F4 = 1;
        break;
    }
    case 0xD: {
        cXyz offset;
        cXyz rel;
        offset.set(0.0f, 1000.0f, 100.0f);
        mLookBackState = 2;
        eInit_calcRelativPos(&rel, &offset, nullptr);
        m82C.copy(rel);
        break;
    }
    case 0x10:
        m872 = (s16)((l_counter & 3) + 1);
        break;
    case 0x11: {
        Bm1BehaviorHioPrm& prm = hio_prm(mType);
        mTargetFlySpeed = prm.m3C;
        mTargetFlyStep = prm.m40;
        m858 = prm.m44;
        break;
    }
    case 0x12:
        speedF = 0.0f;
        m870 = 0x3C;
        break;
    }
    setAnm();
}

/* 02202B48 */
BOOL daNpc_Bm1_c::d_wait() {
    if (m881 && m882) {
        setStt(2);
        // HD: this status-map entry uses 0x27 rather than 7.
        actor_status = (actor_status & ~0x3Fu) | 0x27;
    }
    return TRUE;
}

/* 02202BA8 */
BOOL daNpc_Bm1_c::lookup() {
    if (mbMorfAnimStopped) {
        setStt(3);
    }
    return TRUE;
}

/* 02202BDC */
BOOL daNpc_Bm1_c::orooro() {
    if (dComIfGs_isEventBit(0x0001)) {
        fopAcM_delete(this);
    }
    return TRUE;
}

/* 02202C2C */
BOOL daNpc_Bm1_c::wait_1() {
    if (m8FD == 1 || m8FD >= 3) {
        return TRUE;
    }
    if (m895) {
        if (chk_talk()) {
            setStt(5);
            m896 = false;
        }
        return TRUE;
    }
    m8FD = 2;
    mLookBackState = 0;
    m896 = true;
    if (mbAttention != 0) {
        m86E = 0x28;
    }
    if (Bm1Behavior_calcTimer(&m86E) != 0) {
        mLookBackState = 1;
    }
    return TRUE;
}

/* 02202CF0 */
BOOL daNpc_Bm1_c::talk_1() {
    u8 partsNotMoving = chk_partsNotMove();
    // HD: cache the message-manager pointer once for both status reads.
    u32 messageManager = gabi::ea(l_messageManager.get());
    if (mbHasMsg) {
        mOldMsgStat = (u16)Bm1Behavior_messageStatus(messageManager);
    } else {
        mOldMsgStat = 0;
    }
    talk(1);
    if (mbHasMsg) {
        if (Bm1Behavior_messageStatus(messageManager) == 0x13 /* fopMsgStts_MSG_DESTROYED_e */) {
            switch (mCurrMsgNo) {
            case 0xC92:
                dComIfGs_onEventBit(0x1401);
                break;
            case 0x18A2:
                dComIfGs_onEventBit(0x2202);
                break;
            case 0x1997:
                dComIfGs_onEventBit(0x2120);
                break;
            case 0x19A2:
                dComIfGs_onEventBit(0x2001);
                break;
            case 0x19A4:
                m8FD = 5;
                break;
            case 0x1EF6:
                m8FD = 3;
                break;
            case 0x1EF8:
                dComIfGs_onEventBit(0x1908);
                break;
            case 0x1EF7:
                m87C = 0;
                break;
            }
            m87D = 0xFF;
            m895 = false;
            switch (mSpecificType) {
            case SPECIFIC_TYPE_Akoot_e:
            case SPECIFIC_TYPE_Skett_e:
                setStt(0xB);
                break;
            case SPECIFIC_TYPE_Ilari_0xA_e:
                setStt(0xA);
                break;
            default:
                setStt(m8FF);
                m86E = 0x28;
                endEvent();
            }
        }
    }
    return partsNotMoving;
}

/* 02202FF4 */
BOOL daNpc_Bm1_c::talk_2() {
    if (mPartnerProcID == 0xFFFFFFFF) {
        return TRUE;
    }
    daNpc_Bm1_c* actor = static_cast<daNpc_Bm1_c*>(searchByID(mPartnerProcID));
    if (actor) {
        bool notManzai = actor->mStatus != 9;
        actor->mbManzai = 0; /* clr_manzai() */
        if (notManzai) {
            actor->actor_status &= ~fopAcStts_UNK4000_e; /* fopAcM_OffStatus(actor, fopAcStts_UNK4000_e) */
            setStt(m8FF);
            endEvent();
            mbManzai = 0; /* clr_manzai() */
        }
    }
    return TRUE;
}

/* 02203084 */
BOOL daNpc_Bm1_c::manzai() {
    if (mPartnerProcID == 0xFFFFFFFF) {
        return TRUE;
    }
    if (mbManzai != 0) {
        daNpc_Bm1_c* partner = (daNpc_Bm1_c*)searchByID(mPartnerProcID);
        if (partner == nullptr) // HD: JUT_ASSERT(0x1111, partner != NULL)

            JUT_ASSERT_fail("d_a_npc_bm1.cpp", 0x1111, "a_actor != (0)");
        // HD: anmAtr is virtual (vtable slot +0x24)

        u16 stat = partner->mOldMsgStat;
        gabi::call_ptr(gabi::object_at<Bm1BehaviorActorVtable>(__vtbl).mAnmAtr, this, stat);
    } else {
        setStt(m8FF);
    }
    return TRUE;
}

/* 02203124 */
BOOL daNpc_Bm1_c::wait_4() {
    cLib_addCalcAngleS(&current.angle.y, m818.y, 4, 0x800, 0x80);
    if (m8FD == 1 || m8FD >= 3) {
        return TRUE;
    }
    if (m895) {
        if (chk_talk()) {
            setStt(5);
            m896 = false;
        }
        return TRUE;
    } else {
        m8FD = 2;
        mLookBackState = 0;
        m896 = true;
        if (mbAttention) {
            m86E = 0x28;
        }
    }
    if (Bm1Behavior_calcTimer(&m86E)) {
        mLookBackState = 1;
    }
    return TRUE;
}

/* 02203200 */
BOOL daNpc_Bm1_c::flyawy() {
    if (Bm1Behavior_calcTimer(&m870) == 0) {
        endEvent();
        fopAcM_delete(this);
    } else {
        m88A = 0;
    }
    return TRUE;
}

/* 0220325C */
BOOL daNpc_Bm1_c::wait_5() {
    if (m8FD == 0) {
        cXyz diff;
        cXyz_mi(&dComIfGp_getLinkPlayer()->current.pos, &diff, &current.pos);
        cXyz xz; /* absXZ(): cXyz(x, 0, z).abs() */
        xz.x = diff.x;
        xz.y = 0.0f;
        xz.z = diff.z;
        f32 fVar2 = std_sqrtf(PSVECSquareMag(&xz));
        f32 fVar1 = dComIfGp_getLinkPlayer()->current.pos.y - current.pos.y;
        if (fVar2 < hio_prm(mType).m48 && -40.0f < fVar1 && fVar1 < 300.0f) {
            m8FD = 4;
        }
    }
    return TRUE;
}

/* 02203348 */
BOOL daNpc_Bm1_c::h_wait() {
    if (m8FD == 1 || m8FD >= 3) {
        return TRUE;
    }
    if (m895) {
        if (chk_talk()) {
            setStt(5);
        }
        return TRUE;
    }
    m8FD = 2;
    if (mbAttention != 0) {
        m86E = 0x28;
    }
    if (Bm1Behavior_calcTimer(&m86E)) {
        mLookBackState = 1;
        m896 = 0;
    } else {
        mLookBackState = 3;
        m876 = m818.y;
        m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
    }
    return TRUE;
}

/* 02203414 */
BOOL daNpc_Bm1_c::wait_7() {
    if (m8FD == 1 || m8FD >= 3) {
        return TRUE;
    }
    if (m895) {
        if (chk_talk() && chk_manzai()) {
            setStt(5);
        }
        return TRUE;
    } else if (mbManzai) {
        setStt(9);
        return TRUE;
    } else {
        m8FD = 2;
        if (mbAttention != 0) {
            m86E = 0x28;
        }
        if (Bm1Behavior_calcTimer(&m86E)) {
            mLookBackState = 1;
        } else {
            mLookBackState = 3;
            m876 = m818.y;
            m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
        }
    }
    return TRUE;
}

/* 02203508 */
BOOL daNpc_Bm1_c::wait_3() {
    cLib_addCalcAngleS(&current.angle.y, m818.y, 4, 0x800, 0x80);
    if (m8FD == 1 || m8FD >= 3) {
        return TRUE;
    }
    if (m895) {
        if (chk_talk()) {
            setStt(5);
            m896 = false;
        }
        return TRUE;
    } else {
        m8FD = 2;
        mLookBackState = 0;
        m896 = true;
        if (mbAttention) {
            m86E = 0x28;
        }
    }
    if (Bm1Behavior_calcTimer(&m86E)) {
        mLookBackState = 1;
        m870 = (s16)gabi::ftoi(cM_rndF(180.0f) + 180.0f);
        return TRUE;
    }
    if (!Bm1Behavior_calcTimer(&m870)) {
        setStt(0x10);
    }
    return TRUE;
}

/* 02203648 */
BOOL daNpc_Bm1_c::wait_8() {
    m8FD = 2;
    if (mbMorfAnimStopped) {
        if (!Bm1Behavior_calcTimer(&m872)) {
            setStt(8);
        }
        if (m895 && chk_talk()) {
            setStt(8);
        }
    }
    return TRUE;
}

/* 022036CC */
BOOL daNpc_Bm1_c::wait_2() {
    if (m8FD == 1 || m8FD >= 3) {
        return TRUE;
    }
    if (dComIfGs_isEventBit(0x0A02 /* ENDLESS_NIGHT */)) {
        m8FD = 6;
    }
    return TRUE;
}

/* 02203734 */
BOOL daNpc_Bm1_c::walk_1() {
    if (m895) {
        setAnm_NUM(4, 1);
        speedF = 0.0f;
        if (chk_talk()) {
            setStt(5);
            m896 = false;
        }
        return TRUE;
    }
    cXyz pt;
    Bm1Behavior_pathPoint(&mPathRun, &pt, mPathRun.mIdx);
    mTargetPos.copy(pt);
    mLookBackState = 0;
    m896 = true;
    m8FD = 2;
    if (m88B) {
        if (m87E) {
            setStt(0x12);
        }
        m88B = 0;
    }
    m87E = Bm1Behavior_pathArgument(&mPathRun, mPathRun.mIdx);
    return TRUE;
}

/* 02203830 */
BOOL daNpc_Bm1_c::CHKwai() {
    if (m895) {
        if (chk_talk()) {
            setStt(5);
            mLookBackState = 1;
            m896 = false;
            m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
        }
        return TRUE;
    }
    m8FD = 2;
    if (!Bm1Behavior_calcTimer(&m870)) {
        setStt(0x11);
        return TRUE;
    }
    mLookBackState = 2;
    cXyz pt;
    Bm1Behavior_pathPoint(&mPathRun, &pt, mPathRun.mIdx);
    m82C.copy(pt);
    m82C.y = eyePos.y;
    m896 = false;
    m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
    return TRUE;
}

/* 0220390C */
BOOL daNpc_Bm1_c::demo_action1(void* userData) {
    switch (m904) {
    case 0:
        setStt(1);
        m904 = m904 + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch (mStatus) {
        case 1:
            mbSetEyePos = d_wait();
            break;
        case 2:
            mbSetEyePos = lookup();
            break;
        case 3:
            mbSetEyePos = orooro();
            break;
        }
        lookBack();
        break;
    case 9:
        break;
    }
    return TRUE;
}

/* 022039E0 */
BOOL daNpc_Bm1_c::wait_action1(void* userData) {
    switch (m904) {
    case 0:
        setStt(4);
        m904 = m904 + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch (mStatus) {
        case 4:
            mbSetEyePos = wait_1();
            break;
        case 5:
            mbSetEyePos = talk_1();
            break;
        }
        lookBack();
        break;
    case 9:
        break;
    }
    return TRUE;
}

/* 02203A9C */
BOOL daNpc_Bm1_c::wait_action2(void* userData) {
    switch (m904) {
    case 0:
        setStt(0xC);
        if (mSpecificType == SPECIFIC_TYPE_Kogoli_e && dComIfGs_isEventBit(0x1820) &&
            !dKy_daynight_check()) {
            current.angle.y -= 0x6000;
            m818.y = current.angle.y;
        }
        m904 = m904 + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch (mStatus) {
        case 0xC:
            mbSetEyePos = wait_4();
            break;
        case 5:
            mbSetEyePos = talk_1();
            break;
        case 0xA:
            mbSetEyePos = flyawy();
            break;
        }
        lookBack();
        break;
    case 9:
        break;
    }
    return TRUE;
}

/* 02203BAC */
BOOL daNpc_Bm1_c::wait_action3(void* userData) {
    switch (m904) {
    case 0:
        setStt(0xD);
        m904 = m904 + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch (mStatus) {
        case 0xD:
            mbSetEyePos = wait_5();
            break;
        }
        lookBack();
        break;
    case 9:
        break;
    }
    return TRUE;
}

/* 02203C44 */
BOOL daNpc_Bm1_c::wait_action4(void* userData) {
    switch (m904) {
    case 0:
        setAnm_NUM(4, 1);
        m904 = m904 + 1;
        break;
    case 1:
    case 2:
    case 3:
    case 9:
        break;
    }
    return TRUE;
}

/* 02203C98 */
BOOL daNpc_Bm1_c::wait_action5(void* userData) {
    switch (m904) {
    case 0:
        setStt(0xE);
        m904 = m904 + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch (mStatus) {
        case 0xE:
            mbSetEyePos = wait_4();
            break;
        case 5:
            mbSetEyePos = talk_1();
            break;
        }
        lookBack();
        break;
    case 9:
        break;
    }
    return TRUE;
}

/* 02203D48 */
BOOL daNpc_Bm1_c::wait_action6(void* userData) {
    switch (m904) {
    case 0:
        setStt(7);
        m904 = m904 + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch (mStatus) {
        case 7:
            mbSetEyePos = h_wait();
            break;
        case 5:
            mbSetEyePos = talk_1();
            break;
        }
        lookBack();
        break;
    case 9:
        break;
    }
    return TRUE;
}

/* 02203DF8 */
BOOL daNpc_Bm1_c::wait_action7(void* userData) {
    switch (m904) {
    case 0:
        setStt(0xF);
        m904 = m904 + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch (mStatus) {
        case 0xF:
            mbSetEyePos = wait_7();
            break;
        case 5:
            mbSetEyePos = talk_1();
            break;
        case 0xB:
            mbSetEyePos = talk_2();
            break;
        case 9:
            mbSetEyePos = manzai();
            break;
        }
        lookBack();
        break;
    case 9:
        break;
    }
    return TRUE;
}

/* 02203ED8 */
BOOL daNpc_Bm1_c::wait_action8(void* userData) {
    switch (m904) {
    case 0:
        setStt(8);
        m904 = m904 + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch (mStatus) {
        case 8:
            mbSetEyePos = wait_3();
            break;
        case 5:
            mbSetEyePos = talk_1();
            break;
        case 0x10:
            mbSetEyePos = wait_8();
            break;
        }
        lookBack();
        break;
    case 9:
        break;
    }
    return TRUE;
}

/* 02203FAC */
BOOL daNpc_Bm1_c::wait_action9(void* userData) {
    switch (m904) {
    case 0:
        setStt(6);
        m904 = m904 + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch (mStatus) {
        case 6:
            mbSetEyePos = wait_2();
            break;
        }
        lookBack();
        break;
    case 9:
        break;
    }
    return TRUE;
}

/* 02204044 */
BOOL daNpc_Bm1_c::wait_actionA(void* userData) {
    switch (m904) {
    case 0:
        setStt(0x11);
        m904 = m904 + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch (mStatus) {
        case 0x11:
            mbSetEyePos = walk_1();
            break;
        case 0x12:
            mbSetEyePos = CHKwai();
            break;
        case 5:
            mbSetEyePos = talk_1();
            break;
        }
        lookBack();
        break;
    case 9:
        break;
    }
    return TRUE;
}

/* 02200F90 */
BOOL daNpc_Bm1_c::_draw() {
    J3DModel* model = mpHeadMorf->getModel();
    J3DModelData* model_data = J3DModel_getModelData(model);
    J3DModel* morf_model = mpMorf->getModel();
    if (mbInitPostman0 || m888) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), morf_model, &tevStr);
    dComIfGp_get(); // HD: an unused accessor call (GameCube: the bm02.bmt material table for some types)

    mpMorf->entryDL(); // HD: always without the material table

    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mHeadBtpAnm, model_data, mBlinkFrame);
    mpHeadMorf->entryDL();
    gabi::object_at<Bm1BehaviorModelData>(gabi::ea(model_data)).mpTexPattern =
        nullptr; /* mHeadBtpAnm.remove(model_data) */
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    if (mbHasArms) {
        J3DModel* arm = mpArmMorf->getModel();
        setLightTevColorType(dKy_getEnvlight(), arm, &tevStr);
        mpArmMorf->entryDL(); // HD: always without the bmarm02.bmt material table

    } else {
        J3DModel* wing = mpWingMorf->getModel();
        setLightTevColorType(dKy_getEnvlight(), wing, &tevStr);
        mpWingMorf->entryDL();
    }
    if (mpBinderModel.get() != nullptr) {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, mpBinderModel, &tevStr);
        mDoExt_modelEntryDL(mpBinderModel);
    }
    if (mpBagModel.get() != nullptr) {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, mpBagModel, &tevStr);
        mDoExt_modelEntryDL(mpBagModel);
    }
    if (mpKnifeModel.get() != nullptr) {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, mpKnifeModel, &tevStr);
        mDoExt_modelEntryDL(mpKnifeModel);
    }
    if (mpStickModel.get() != nullptr) {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, mpStickModel, &tevStr);
        mDoExt_modelEntryDL(mpStickModel);
    }
    // HD: no shadowDraw()

    switch ((u32)(s32)mType) {
    case TYPE_Quill_e:
        dSnap_RegistFig(DSNAP_TYPE_NPC_BM1_PST, this, 1.0f, 1.0f, 1.0f);
        break;
    case TYPE_Akoot_e:
        dSnap_RegistFig(DSNAP_TYPE_NPC_BM1_SKT_KKT, this, 1.0f, 1.0f, 1.0f);
        break;
    case TYPE_Skett_e:
        dSnap_RegistFig(DSNAP_TYPE_NPC_BM1_SKT_KKT, this, 1.0f, 1.0f, 1.0f);
        break;
    case TYPE_Basht_e:
        dSnap_RegistFig(DSNAP_TYPE_NPC_BM1_BMB_0_1, this, 1.0f, 1.0f, 1.0f);
        break;
    case TYPE_Bisht_e:
        dSnap_RegistFig(DSNAP_TYPE_NPC_BM1_BMB_0_1, this, 1.0f, 1.0f, 1.0f);
        break;
    case TYPE_Hoskit_e:
        dSnap_RegistFig(DSNAP_TYPE_NPC_BM1_BMB_2, this, 1.0f, 1.0f, 1.0f);
        break;
    case TYPE_Ilari_e:
        dSnap_RegistFig(DSNAP_TYPE_NPC_BM1_BMC_0_1_2, this, 1.0f, 1.0f, 1.0f);
        break;
    case TYPE_Pashli_e:
        dSnap_RegistFig(DSNAP_TYPE_NPC_BM1_BMC_3, this, 1.0f, 1.0f, 1.0f);
        break;
    case TYPE_Namali_e:
        dSnap_RegistFig(DSNAP_TYPE_NPC_BM1_BMD_0, this, 1.0f, 1.0f, 1.0f);
        break;
    case TYPE_Kogoli_e:
        dSnap_RegistFig(DSNAP_TYPE_NPC_BM1_BMD_1, this, 1.0f, 1.0f, 1.0f);
        break;
    }
    /* debug leftovers: function-local static colors initialised on first use */
    if (hio_prm(mType).m18 != 0) {
        static_color_init(0x101FDA50, 0x101FEBF4,
                          0x10016F70); /* (GXColor){0xFF, 0x00, 0x00, 0x80} */
        static_color_init(0x101FDAC0, 0x101FEBF8,
                          0x10016F74); /* (GXColor){0x00, 0x00, 0xFF, 0x80} */
        if (mSpecificType == SPECIFIC_TYPE_Quill_2_e) {
            static_color_init(0x101FDA48, 0x101FEBEC, 0x10016F78);
        }
    }
    return TRUE;
}

/* 02200D58 */
BOOL daNpc_Bm1_c::_execute() {
    mHD_A08 = 0; /* HD */
    if (!mbRanExecute) {
        mInitialPos.copy(current.pos);
        mbRanExecute = true;
        m818.x = current.angle.x;
        m818.y = current.angle.y;
        m818.z = current.angle.z;
    }
    Bm1BehaviorHioPrm& prm = hio_prm(mType);
    m_jnt.setParam(prm.mMaxBackboneX, prm.mMaxBackboneY, prm.mMinBackboneX, prm.mMinBackboneY,
                   prm.mMaxHeadX, prm.mMaxHeadY, prm.mMinHeadX, prm.mMinHeadY, prm.mMaxTurnStep);
    if (!m881 && mbInitPostman0 && demoActorID == 0) {
        return TRUE;
    }
    m887 = 0;
    mbInitPostman0 = false;
    partner_srch();
    checkOrder();
    if (!demo()) {
        s32 eventIndex = -1;
        if (dComIfGp_event_runCheck() &&
            eventInfo.mCommand != dEvtCmd_INTALK_e /* !eventInfo.checkCommandTalk() */) {
            eventIndex = isEventEntry();
        }
        if (eventIndex >= 0) {
            event_proc(eventIndex);
        } else {
            pmf_call(this, &mCurrActionFunc, nullptr); /* (this->*mCurrActionFunc)(NULL) */
        }
        if (!bm_flyMove()) {
            bm_nMove();
        }
        if (!m887) {
            fopAcM_posMoveF(this, &mStts.m_cc_move); /* mStts.GetCCMoveP() */
        }
        if (!mbSetShapeAngle) {
            shape_angle.x = current.angle.x;
            shape_angle.y = current.angle.y;
            shape_angle.z = current.angle.z;
        }
    }
    eventOrder();
    setMtx(0);
    if (!mbInDemo) {
        setCollision(50.0f, 160.0f);
    }
    return TRUE;
}

/* 02200F8C */
static BOOL daNpc_Bm1_Execute(daNpc_Bm1_c* i_this) { return i_this->_execute(); }

/* 0220124C */
static BOOL daNpc_Bm1_Draw(daNpc_Bm1_c* i_this) { return i_this->_draw(); }

/* 02201250 */
static BOOL daNpc_Bm1_IsDelete(daNpc_Bm1_c* i_this) { return TRUE; }

// HD: compiler-generated and template code of this translation unit.
/* 028EFFD0 Bm1Behavior_constructArray(array, n, size, ctor) (GHS runtime) */

// HD: the HIO constructor allocates when called with a null this pointer.
/* 02204118 */
static Bm1BehaviorChildHio* Bm1BehaviorChildHio_ct(Bm1BehaviorChildHio* i_this) {
    if (i_this == nullptr) {
        i_this = (Bm1BehaviorChildHio*)operator_new(0x54);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x10017008;
    return i_this;
}

// HD: the HIO constructor allocates when called with a null this pointer.
/* 02204158 */
static Bm1BehaviorHio* Bm1BehaviorHio_ct(Bm1BehaviorHio* i_this) {
    if (i_this == nullptr) {
        i_this = (Bm1BehaviorHio*)operator_new(0x354);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x10017018;
    Bm1Behavior_constructArray(i_this->children, 10, 0x54,
                               gabi::guest_fn((const void*)Bm1BehaviorChildHio_ct));
    for (int i = 0; i < 10; i++) {
        i_this->children[i].m50 = i;

        memcpy_g(&i_this->children[i].hio_prm, &a_prm_tbl[i],
                 sizeof(Bm1BehaviorHioPrm)); /* 028FEAC0 memcpy */
    }
    i_this->m8 = -1;
    i_this->m4 = -1;
    return i_this;
}

// HD: static initialisation of the translation unit
/* 02204208 */
static void __sinit_d_a_npc_bm1_cpp() {
    sinit_header_statics(l_behaviorTu.headerStatics[0], l_behaviorTu.headerStatics[1]);
    Bm1BehaviorHio_ct(&l_HIO); /* static Bm1BehaviorHio l_HIO */
}

// HD: sead::SafeString deleting destructor (this TU's copy; vtable 0x10016F80 slot +0xC)
/* 022042A8 */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

// HD: deleting virtual destructor destroys the NPC base members.
/* 022042BC */
static void daNpc_Bm1_c_dt(daNpc_Bm1_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        Bm1Behavior_circleDestroy(&i_this->mAcchCir.m_cir,
                                  2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        /* ~dBgS_ObjAcch: this TU's vtables of its sub-objects, then dBgS_Acch::~dBgS_Acch */
        i_this->mObjAcch.dBgS_GrpPassChk::__vtbl = l_behaviorTu.objAcchDt[0];
        i_this->mObjAcch.dBgS_PolyPassChk::__vtbl = l_behaviorTu.objAcchDt[1];
        Bm1Behavior_acchDestroy(&i_this->mObjAcch, 0);
        Bm1Behavior_actorDestroy(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

// HD: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty)
/* 02204358 */
static void SafeString_assureTerminationImpl(SafeString* i_this) {}

/* WWHD glue
str 100179F8 "a_actor != (0)" in 02203084
str 100179E8 "d_a_npc_bm1.cpp" in 02203084
data a_prm_tbl 101BCC3C
data a_attribute_anm_prm_tbl 101BCB10
data a_status_anm_prm_tbl 101BC994
ext 0259DED0 void Bm1Behavior_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly)
ext 0259D54C void Bm1Behavior_playerEyePos(cXyz* out, f32 offs)
ext 024EDFCC bool Bm1Behavior_lockonTruth(dAttention_c* attention)
ext 024EC8D0 fopAc_ac_c* Bm1Behavior_lockonTarget(dAttention_c* attention, s32 index)
ext 024EE464 fopAc_ac_c* Bm1Behavior_actionTarget(dAttention_c* attention, s32 index)
ext 025F795C u32 Bm1Behavior_messageStatus(u32 manager)
ext 02055B64 s16 Bm1Behavior_calcTimer(be<s16>* timer)
ext 0259E778 void Bm1Behavior_pathPoint(dNpc_PathRun_bm1* path, cXyz* out, u8 index)
ext 0259EE4C u8 Bm1Behavior_pathArgument(dNpc_PathRun_bm1* path, u8 index)
ext 0259E838 BOOL Bm1Behavior_pathPassed(dNpc_PathRun_bm1* path, cXyz* pos, u32 direction)
ext 0259ED58 void Bm1Behavior_pathNext(dNpc_PathRun_bm1* path)
ext 0259EDB8 s32 Bm1Behavior_pathMax(dNpc_PathRun_bm1* path)
ext 02586D5C void Bm1Behavior_sendLetter(u16 number)
ext 028EFFD0 void Bm1Behavior_constructArray(void* array, s32 count, u32 size, u32 constructor)
ext 02008DAC void Bm1Behavior_groundCheckDestroy(dBgS_GndChk* check, s32 flags)
ext 02018034 void Bm1Behavior_circleDestroy(cM3dGCir_l* circle, s32 flags)
ext 024EFD9C void Bm1Behavior_acchDestroy(dBgS_ObjAcch* acch, s32 flags)
ext 025D50BC void Bm1Behavior_actorDestroy(daNpc_Bm1_c* actor, s32 flags)
*/
