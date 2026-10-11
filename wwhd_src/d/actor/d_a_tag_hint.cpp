/* Local WWHD hint trigger reconstruction. */
#include "d/actor/d_a_tag_hint.h"

// HD: each translation unit owns these common-header statics.
static u32 l_headerZero[4] = {0, 0, 0, 0};
static f32 l_headerMinAngle;
static f32 l_headerMaxAngle;
static u8 l_headerAngleObject;
static u8 l_headerFlagObject;

static s32 l_msgId;
static u32 l_messageControl = 0;

static LIGHT_INFLUENCE plight;
static LIGHT_INFLUENCE elight;

static s16 search_angle_table[] = {0, 0x2000, -0x2000, 0x4000, -0x4000, 0x6000, -0x6000, 0x7FFF};
static s16 search_angle_table_grab[] = {0x4000, -0x4000, 0x6000, -0x6000,
                                        0x7FFF, -0x2000, 0x2000, 0};
static const char* action_table[] = {"WAIT", "MESSAGE", "MESSAGE2", "SURPRIS", "MESSAGE3"};

static s32 isSwitch(daTag_Hint_c* a, s32 sw) {
    dSv_info_c* info = dComIfGs_info();
    s32 room = a->current.roomNo;
    return info->isSwitch(sw, room);
}

/* 024A9494 */
u8 daTag_Hint_c::getEventNo() { return (u32)mParameters >> 24; }

/* 024A8F70 */
u8 daTag_Hint_c::getSwbit() { return ((u32)mParameters >> 8) & 255; }

/* 024A8F7C */
u8 daTag_Hint_c::getSwbit2() { return ((u32)mParameters >> 16) & 255; }

/* 024A8F90 */
u8 daTag_Hint_c::getType() { return (u32)mParameters & 63; }

/* 024A95B8 */
u8 daTag_Hint_c::getType2() { return ((u32)mParameters >> 6) & 3; }

/* 024A9928 */
u16 daTag_Hint_c::getMessage() { return (u16)home.angle.x; }

/* 024A8F88 */
u16 daTag_Hint_c::getEventFlag() { return (u16)home.angle.z; }

/* 024A9284 */
s32 daTag_Hint_c::arrivalTerms() {
    s32 sw = getSwbit2();
    u16 ev = getEventFlag();
    u8 type = getType();
    if (type != 8 && type != 10 && type != 16 && sw != 255 && !isSwitch(this, sw))
        return 0;
    if ((type == 2 || type == 8 || type == 10) && ev != 65535 && !dComIfGs_isEventBit(ev))
        return 0;
    if (type == 17 && !dComIfGs_isEventBit(0x4020))
        return 0;
    switch (type) {
    case 1:
        m2A0 = 600;
        break;
    case 4:
    case 8:
        m2A0 = 3600;
        break;
    case 9:
        if (!dComIfGs_isTmpBit(0x308))
            return 0;
        m2A0 = 600;
        break;
    case 11:
        if (gabi::load<u8>(0x101D5F4C) != 1)
            return 0;
        {
            s8 room = current.roomNo;
            s8 stay = dComIfGp_roomControl_getStayNo();
            if (stay != room)
                return 0;
        }
        break;
    case 12:
    case 14:
        m2A0 = 1800;
        break;
    case 15:
        m2A0 = 900;
        break;
    default:
        m2A0 = 0;
        break;
    }
    return 1;
}

/* 024A8FD0 */
s32 daTag_Hint_c::waitTerms() {
    s32 sw = getSwbit(), sw2 = getSwbit2();
    u16 ev = getEventFlag();
    u8 type = getType();
    if (sw != 255 && isSwitch(this, sw))
        return 1;
    if ((type == 0 || (type >= 3 && type <= 5)) && ev != 65535 && dComIfGs_isEventBit(ev))
        return 1;
    switch (type) {
    case 1:
        if (moveBoxCheck()) {
            if (sw != 255) {
                dSv_info_c* info = dComIfGs_info();
                s32 room = current.roomNo;
                info->onSwitch(sw, room);
            }
            dComIfGs_onEventBit(0x404);
            return 1;
        }
        break;
    case 6:
        return dComIfGs_isEventBit(0x2A10) != 0;
    case 7:
        return sw2 != 255 && !isSwitch(this, sw2);
    case 9:
        if (dComIfGs_isTmpBit(0x304))
            return 1;
        return dComIfGs_isEventBit(0x1801) != 0;
    case 8:
    case 10:
    case 16:
        return sw2 != 255 && isSwitch(this, sw2);
    case 11:
        return dComIfGs_getItem(dInvSlot_BOW_e) != 255;
    case 13:
        return dComIfGs_isStageBossEnemy() != 0;
    case 14:
        return dComIfGs_isEventBit(0x3C01) != 0;
    case 15:
        return dComIfGs_isEventBit(0x3F20) != 0;
    case 17:
        return dComIfGs_checkGetItem(dItemNo_POWER_BRACELETS_e) != 0;
    }
    return 0;
}

/* 024A8BC0 */
s32 daTag_Hint_c::rangeCheck_local(cXyz* pos) {
    gabi::Local<cXyz> diff, flat;
    cXyz_mi(pos, diff.get(), &current.pos);
    f32 y = diff->y, x = diff->x, z = diff->z;
    if (y < 0.0f)
        y = -y;
    flat->z = z;
    flat->x = x;
    flat->y = 0.0f;
    f32 mag = PSVECSquareMag(flat.get());
    if (!(mag < m2A8))
        return 0;
    if (y > m2AC)
        return 0;
    return 1;
}

/* 024A9930 */
s32 daTag_Hint_c::rangeCheck() {
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    return rangeCheck_local(&player->current.pos);
}

/* 024A996C */
s32 daTag_Hint_c::otherCheck() {
    u32 play = dComIfGp_ea();
    daPy_py_c* player =
        gabi::at<daPy_py_c>(gabi::ea(gabi::at<dComIfG_inf_c>(play)->play.mpPlayerPtr[0].get()));
    u8 type = getType();
    if (type == 4 || type == 8 || type == 9 || type == 12) {
        s16 timer = m2A0;
        if (timer > 0) {
            m2A0 = timer - 1;
            return 0;
        }
    }
    if (!rangeCheck())
        return 0;
    if (type == 0) {
        hint::PlayerVtable* vtable = gabi::at<hint::PlayerVtable>(player->__vtbl);
        if (gabi::call<s32>(vtable->checkGrabBarrel, gabi::ea(player)))
            return 0;
        f32 v = (f32)player->field_0x2b0;
        if (v < 0.0f)
            return 0;
    } else if (type == 1) {
        dComIfGs_onEventBit(0x404);
        if (!dComIfGs_isEventBit(0x401))
            return 0;
    } else if (type == 6) {
        if (dComIfGs_getEventRegHD(0xA507) < 3)
            return 0;
    } else if (type == 14 || type == 15) {
        play = dComIfGp_ea();
        if (gabi::at<dComIfG_inf_c>(play)->play.mEvtCtrl.mMode)
            return 0;
    }
    if (m2A0 <= 0)
        return 1;
    u32 control = l_messageControl;
    if (!hintMessageStatus(control))
        m2A0 = (s16)m2A0 - 1;
    return 0;
}

/* 024A9B44 */
s32 daTag_Hint_c::getPriority() { return getType() == 2 ? 490 : 511; }

/* 024A94A0 */
void daTag_Hint_c::makeEventId() {
    u32 play = dComIfGp_ea();
    u32 ship = gabi::ea(gabi::at<dComIfG_inf_c>(play)->play.mPlayerInfo[0].mpPlayer.get());
    hint::PlayerVtable* vtable = gabi::at<hint::PlayerVtable>(gabi::at<fopAc_ac_c>(ship)->__vtbl);
    s32 id = gabi::call<s32>(vtable->getGrabActorID, ship);
    u8 type = getType();
    const char* name;
    u8 special = 0;
    if (type == 10)
        name = "HIRL_HINT";
    else {
        gabi::Local<be<s32>> search;
        search.get()->operator=(id);
        if (id != -1 && fopAcIt_Judge(hint::searchByIDJudge, search.get())) {
            name = "DEFAULT_HINT2";
            special = 1;
        } else
            name = "DEFAULT_HINT";
    }
    u8 event = getEventNo();
    play = dComIfGp_ea();
    s32 idx = hintGetEventIdx(&gabi::at<dComIfG_inf_c>(play)->play.mEvtManager, name, event);
    m2A4 = idx;
    m2A3 = special;
}

/* 024A9D04 */
void daTag_Hint_c::initLight() {
    // HD: the two lights use different colors, powers and fluctuation from GameCube.
    plight.mFluctuation = 93.80000305175781f;
    plight.mPower = 104.34782409667969f;
    plight.mColorR = 0;
    elight.mColorR = 0;
    elight.mPower = 60.0f;
    elight.mColorG = 47;
    elight.mColorB = 233;
    plight.mColorG = 351;
    elight.mFluctuation = 93.80000305175781f;
    plight.mColorB = 510;
}

/* 024A9D6C */
void daTag_Hint_c::setLightPos() {
    // HD: preserve fused arithmetic and the second heading load between position stores.
    u32 play = dComIfGp_ea();
    daPy_py_c* player =
        gabi::at<daPy_py_c>(gabi::ea(gabi::at<dComIfG_inf_c>(play)->play.mpPlayerPtr[0].get()));
    f32 y = (f32)player->mRightHandPos.y, x = (f32)player->mRightHandPos.x,
        z = (f32)player->mRightHandPos.z;
    plight.mPos.x = x;
    f32 height = y + 5.66f;
    plight.mPos.z = z;
    plight.mPos.y = height;
    u32 angle = (u16)player->shape_angle.y;
    f32 sin = cM_ssin(angle), cos = cM_scos(angle);
    f32 offset = sin * 11.13f;
    f32 lx = x + gabi::fmadds(cos, 10.25f, offset);
    plight.mPos.x = lx;
    angle = (u16)player->shape_angle.y;
    sin = cM_ssin(angle);
    offset = sin * 10.25f;
    cos = cM_scos(angle);
    f32 oz = gabi::fmsubs(cos, 11.13f, offset);
    elight.mPos.x = lx;
    f32 lz = z + oz;
    elight.mPos.y = height;
    plight.mPos.z = lz;
    elight.mPos.z = lz;
}

/* 024A9E28 */
void daTag_Hint_c::makeLight() {
    initLight();
    // HD: the call supplies no explicit this register.
    hintSetLightPosHD();
    hintPlightPrioritySet(&plight);
}

/* 024A9FA0 */
void daTag_Hint_c::deleteLight() { dKy_plight_cut(&plight); }

/* 024A8C84 */
static fopAc_ac_c* findObjectCallBack(fopAc_ac_c* object, daTag_Hint_c* hint) {
    if (!object || object->mProfName != hint::fpcNm_Obj_Movebox_e)
        return nullptr;
    if (!hint->rangeCheck_local(&object->home.pos))
        return nullptr;
    gabi::Local<cXyz> diff, flat;
    cXyz_mi(&object->current.pos, diff.get(), &object->home.pos);
    f32 x = diff->x, z = diff->z;
    flat->y = 0.0f;
    flat->x = x;
    flat->z = z;
    f32 mag = PSVECSquareMag(flat.get());
    return mag > 0.0f ? object : nullptr;
}

/* 024A8F9C */
s32 daTag_Hint_c::moveBoxCheck() {
    return fopAcM_Search(reinterpret_cast<void* (*)(void*, void*)>(findObjectCallBack), this) !=
           nullptr;
}

/* 024A95C4 */
void daTag_Hint_c::setPlayerAngle() {
    u32 play = dComIfGp_ea();
    daPy_py_c* player =
        gabi::at<daPy_py_c>(gabi::ea(gabi::at<dComIfG_inf_c>(play)->play.mpPlayerPtr[0].get()));
    gabi::Local<hint::dBgS_CamLinChk> check;
    u32 c = gabi::ea(check.get());
    hintConstructLineCheck(check.get());
    u32 a = c + 0x58, b = c + 0x64;
    check->mPolyPassChk = a;
    check->dBgS_GrpPassChk::__vtbl = 0x1003F2A8;
    check->mGrp = 1;
    check->mbLinkThrough = 0;
    check->mbArrowThrough = 0;
    check->mbBombThrough = 0;
    check->mbRopeThrough = 0;
    check->mGrpPassChk = b;
    check->mbCamThrough = 1;
    check->cBgS_PolyInfo::__vtbl = 0x1003F298;
    check->cBgS_Chk::__vtbl = 0x1003F288;
    check->dBgS_PolyPassChk::__vtbl = 0x1003F2B8;
    check->mbBoomerangThrough = 0;
    check->mbObjThrough = 0;
    gabi::Local<cXyz> start, end;
    start->x = (f32)player->attention_info.position.x;
    start->y = (f32)player->attention_info.position.y;
    start->z = (f32)player->attention_info.position.z;
    s16 angle = 0;
    bool clear = false;
    for (u32 i = 0; i < 8; ++i) {
        u8 special = m2A3;
        s16 heading = (s16)player->shape_angle.y;
        s16 offset = special ? search_angle_table_grab[i] : search_angle_table[i];
        angle = (s16)(heading + offset);
        f32 x = start->x, z = start->z;
        f32 sn = cM_ssin(angle), co = cM_scos(angle);
        end->y = (f32)start->y;
        end->x = gabi::fmadds(sn, 120.0f, x);
        end->z = gabi::fmadds(co, 120.0f, z);
        hintSetLineCheck(check.get(), start.get(), end.get(), 0);
        play = dComIfGp_ea();
        if (!hintLineCross(&gabi::at<dComIfG_inf_c>(play)->play.mBgS, check.get())) {
            clear = true;
            break;
        }
    }
    if (!clear)
        angle = (s16)player->shape_angle.y;
    player->mDemo.setMoveAngle(angle);
    check->dBgS_PolyPassChk::__vtbl = 0x1003F278;
    check->dBgS_GrpPassChk::__vtbl = 0x1003F238;
    check->cBgS_PolyInfo::__vtbl = 0x1003F228;
    hintDestroyLineCheck(check.get(), 0);
}

/* 024A8D2C */
void daTag_Hint_c::darkProc() {
    gabi::Local<SafeString> name, stage;
    // HD: skip the light/fog adjustment in Hyrule, after the original SafeString calls.
    name->mStringTop = gabi::guest_addr("Hyrule");
    name->__vtbl = 0x1003F210;
    u32 play = dComIfGp_ea();
    stage->mStringTop = gabi::ea(gabi::at<dComIfG_inf_c>(play)->play.mCurStage.mName);
    stage->__vtbl = 0x1003F210;
    auto prepare = [](SafeString* s) {
        hint::SafeStringVtable* vtable = gabi::at<hint::SafeStringVtable>(s->__vtbl);
        gabi::call(vtable->prepare, s);
    };
    prepare(name.get());
    prepare(name.get());
    u32 a = name->mStringTop;
    prepare(stage.get());
    u32 b = stage->mStringTop;
    if (a == b)
        return;
    a = name->mStringTop;
    b = stage->mStringTop;
    for (u32 i = 0; i < 0x40001; ++i) {
        u8 av = gabi::load<u8>(a + i), bv = gabi::load<u8>(b + i);
        if (av != bv)
            break;
        if (av == 0)
            return;
    }
    dKy_set_vrboxcol_ratio((f32)m2B0);
    dKy_set_fogcol_ratio((f32)m2B0);
    f32 t = ((f32)m2B0 - 0.15f) * 1.1764705181121826f;
    if (t > 0.5f) {
        dKy_fog_startendz_set(300.0f, 2000.0f, 2.0f - (t + t));
    } else {
        f32 v = t * 0.85f;
        dKy_set_bgcol_ratio((v + v) + 0.15f);
        dKy_fog_startendz_set(300.0f, gabi::fmadds(3000.0f, t, 500.0f), 1.0f);
    }
}


/* 024A97E4 */
void daTag_Hint_c::startProc() {
    s32 sw = getSwbit();
    u8 type = getType();
    if (type == 0)
        dComIfGs_onEventBit(0x401);
    else if (type != 6 && type != 12 && sw != 255) {
        dSv_info_c* info = dComIfGs_info();
        s32 room = current.roomNo;
        info->onSwitch(sw, room);
    }
    u32 play = dComIfGp_ea();
    s32 staff = hintGetMyStaffId(&gabi::at<dComIfG_inf_c>(play)->play.mEvtManager, "TAGHINT", 0, 0);
    m29C = staff;
    m2B0 = 1.0f;
    if (type != 10 && !(getType2() & 1)) {
        hintSeStart(hint::JA_SE_ITM_OMAMORI_ST_TALK);
        setPlayerAngle();
    }
    u16 flag = getEventFlag();
    if (type == 5)
        dComIfGs_onEventBit(flag);
}

/* 024A9E60 */
u16 daTag_Hint_c::next_msgStatus(u32* msg) { return hint::fopMsgStts_MSG_ENDS_e; }

/* 024A9E58 */
u32 daTag_Hint_c::getMsg() { return mMsgNo; }

/* 024A9CF8 */
void daTag_Hint_c::talkInit() { m294 = 0; }

/* 024A9E68 */
u16 daTag_Hint_c::talk() {
    s8 state = m294;
    u32 control = l_messageControl;
    u16 status = 255;
    if (state == 0) {
        l_msgId = fpcM_ERROR_PROCESS_ID_e;
        m290 = getMsg();
        m294 = 1;
    } else if (state != -1) {
        s32 id = l_msgId;
        if (id == -1) {
            u32 msg = m290;
            u32 res = hintMessageSet(control, msg, &eyePos);
            l_msgId = res;
        } else if (state == 1)
            m294 = 2;
        else if (state == 2) {
            status = (u16)hintMessageStatus(control);
            if (status == hint::fopMsgStts_MSG_DISPLAYED_e) {
                u16 next = next_msgStatus(reinterpret_cast<u32*>(&m290));
                hintSetMessageStatus(control, next);
                if (hintMessageStatus(control) == hint::fopMsgStts_MSG_CONTINUES_e)
                    hintMessageSet(control, (u32)m290, 0);
            } else if (status == hint::fopMsgStts_BOX_CLOSED_e) {
                hintSetMessageStatus(control, hint::fopMsgStts_MSG_DESTROYED_e);
                m294 = -1;
            }
        }
    }
    return status;
}

/* 024A9FAC */
s32 daTag_Hint_c::actionEvent() {
    s32 staff = m29C;
    u32 play = dComIfGp_ea();
    s32 action = hintGetMyActIdx(&gabi::at<dComIfG_inf_c>(play)->play.mEvtManager, staff,
                                 action_table, 5, 0, 0);
    staff = m29C;
    play = dComIfGp_ea();
    if (hintGetIsAddvance(&gabi::at<dComIfG_inf_c>(play)->play.mEvtManager, staff)) {
        m2A6 = (u16)m2A6 | 1;
        switch (action) {
        case 1:
            talkInit();
            // HD: the call supplies no explicit this register.
            hintMakeLightHD();
            hintSeStart(hint::JA_SE_ITM_OMAMORI_TETLA);
            break;
        case 2:
            talkInit();
            hintSeStart(hint::JA_SE_ITM_OMAMORI_ST_TALK);
            hintSeStart(hint::JA_SE_ITM_OMAMORI_TETLA);
            break;
        case 3:
            hintSeStart(hint::JA_SE_ITM_OMAMORI_BLINK);
            play = dComIfGp_ea();
            {
                u32 p = gabi::ea(gabi::at<dComIfG_inf_c>(play)->play.mpPlayerPtr[0].get());
                hint::PlayerVtable* vtable =
                    gabi::at<hint::PlayerVtable>(gabi::at<daPy_py_c>(p)->__vtbl);
                gabi::call(vtable->voiceStart, p, 28);
            }
            break;
        case 4:
            talkInit();
            hintSeStart(hint::JA_SE_ITM_OMAMORI_TETLA);
            break;
        }
    }
    auto endCut = [&]() {
        s32 id = m29C;
        u32 p = dComIfGp_ea();
        hintCutEnd(&gabi::at<dComIfG_inf_c>(p)->play.mEvtManager, id);
    };
    auto resetEvent = []() { dComIfGp_getEvent()->reset(); };
    if (action == 1) {
        if (talk() == hint::fopMsgStts_BOX_CLOSED_e) {
            hintSeStart(hint::JA_SE_ITM_OMAMORI_ED_TALK);
            endCut();
            mCurProc = hint::ACTION_LIGHT;
            deleteLight();
            resetEvent();
        }
        f32 v = m2B0;
        if (v > 0.15f) {
            if (v > 0.2f) {
                m2B0 = v - 0.05f;
                darkProc();
                return 1;
            }
            m2B0 = 0.15f;
        }
        darkProc();
    } else if (action == 2) {
        u16 status = talk();
        u16 flags = m2A6;
        if (flags & 1) {
            if (status == hint::fopMsgStts_BOX_CLOSED_e) {
                hintSeStart(hint::JA_SE_ITM_OMAMORI_ED_TALK);
                s32 id = m29C;
                mMsgNo = 0x454;
                u32 p = dComIfGp_ea();
                hintCutEnd(&gabi::at<dComIfG_inf_c>(p)->play.mEvtManager, id);
            }
        } else
            endCut();
    } else if (action == 4) {
        if (talk() == hint::fopMsgStts_BOX_CLOSED_e) {
            mCurProc = hint::ACTION_WAIT;
            resetEvent();
        }
    } else
        endCut();
    return 1;
}

/* 024A9B78 */
s32 daTag_Hint_c::actionHunt() {
    if (eventInfo.checkCommandTalk()) {
        mCurProc = hint::ACTION_EVENT;
        startProc();
        mMsgNo = getMessage();
        return 1;
    }
    if (waitTerms()) {
        mCurProc = hint::ACTION_WAIT;
        return 1;
    }
    if (otherCheck()) {
        makeEventId();
        eventInfo.setEventId(m2A4);
        u8 event = getEventNo();
        eventInfo.mMapToolId = event;
        if (getType2() & 1)
            fopAcM_orderSpeakEvent(this);
        else {
            s32 priority = getPriority();
            u32 play = dComIfGp_ea();
            hintRequest(gabi::at<void>(play + 0x5928), this, priority);
        }
        eventInfo.onCondition(dEvtCnd_CANTALK_e);
    }
    return 1;
}

/* 024A9C74 */
s32 daTag_Hint_c::actionArrival() {
    if (waitTerms()) {
        mCurProc = hint::ACTION_WAIT;
        return 1;
    }
    if (arrivalTerms()) {
        makeEventId();
        eventInfo.setEventId(m2A4);
        u8 event = getEventNo();
        eventInfo.mMapToolId = event;
        mCurProc = hint::ACTION_HUNT;
        actionHunt();
    }
    return 1;
}

/* 024A8F08 */
s32 daTag_Hint_c::actionLight() {
    f32 v = m2B0;
    if (!(v < 0.95f)) {
        mCurProc = hint::ACTION_WAIT;
        m2B0 = 1.0f;
    } else
        m2B0 = v + 0.05f;
    darkProc();
    return 1;
}

/* 024AA52C */
static s32 daTag_Hint_Draw(daTag_Hint_c* a) { return 1; }

/* 024AA254 */
static s32 daTag_Hint_Execute(daTag_Hint_c* a) {
    switch ((u8)a->mCurProc) {
    case hint::ACTION_LIGHT:
        a->actionLight();
        break;
    case hint::ACTION_ARRIVAL:
        a->actionArrival();
        break;
    case hint::ACTION_HUNT:
        a->actionHunt();
        break;
    case hint::ACTION_EVENT:
        a->actionEvent();
        break;
    }
    return 1;
}

/* 024AA2C8 */
static s32 daTag_Hint_IsDelete(daTag_Hint_c* a) { return 1; }

/* 024AA2D0 */
static s32 daTag_Hint_Delete(daTag_Hint_c* a) {
    u8 type = a->getType();
    if (type == 9)
        dComIfGs_offTmpBit(0x308);
    else if (type == 17)
        dComIfGs_offEventBit(0x4020);
    return 1;
}

/* 024AA334 */
static s32 daTag_Hint_Create(daTag_Hint_c* a) {
    u32 flags = a->actor_condition;
    if (!(flags & fopAcCnd_INIT_e)) {
        if (a) {
            hintConstructActor(a);
            flags = a->actor_condition;
            a->__vtbl = 0x1003F2C8;
        }
        a->actor_condition = flags | fopAcCnd_INIT_e;
    }
    f32 x = a->scale.x, y = a->scale.y;
    a->m2A8 = (x * x) * 10000.0f;
    a->m2AC = y * 100.0f;
    u8 type = a->getType();
    if (type == 14) {
        a->m2AC = 2410.0f;
        type = a->getType();
    }
    u8 sw = a->getSwbit();
    if (type == 1 && dComIfGs_isEventBit(0x404) && sw != 255) {
        dSv_info_c* info = dComIfGs_info();
        s32 room = a->current.roomNo;
        info->onSwitch(sw, room);
    }
    a->mCurProc = a->waitTerms() ? hint::ACTION_WAIT : hint::ACTION_ARRIVAL;
    a->shape_angle.x = 0;
    a->current.angle.z = 0;
    a->current.angle.x = 0;
    a->m2A0 = 0;
    a->shape_angle.z = 0;
    return cPhs_COMPLEATE_e;
}

// HD: compiler-generated and template code of this translation unit.

/* 024AA480 */
static void __sinit_d_a_tag_hint_cpp() {
    l_headerZero[2] = 0;
    l_headerZero[0] = 0;
    l_headerZero[3] = 0;
    l_headerZero[1] = 0;
    __register_global_object(0x101D1A88);
    l_headerMinAngle = -3.1415927410125732f;
    l_headerMaxAngle = 3.1415927410125732f;
    hintConstructHeaderAngle(&l_headerAngleObject);
    __register_global_object(0x101D1A94);
    hintConstructHeaderFlag(&l_headerFlagObject);
    __register_global_object(0x101D1AA0);
    elight.mHD20 = 1.0f;
    plight.mHD20 = 1.0f;
}

/* 024AA548 */
static void daTag_Hint_destructor(daTag_Hint_c* actor, s32 flags) {
    if (actor) {
        __dt(static_cast<fopAc_ac_c*>(actor));
        if (flags & 1)
            operator_delete(actor);
    }
}

/* WWHD glue
data l_headerZero 1046E2F4 nocheck
data l_headerMinAngle 1046E2E8 nocheck
data l_headerMaxAngle 1046E2EC nocheck
data l_headerAngleObject 1046E2F0 nocheck
data l_headerFlagObject 1046E2F1 nocheck
ext 028ED6F8 void hintConstructHeaderAngle(void* object)
ext 028EAB2C void hintConstructHeaderFlag(void* object)
ext 025D4ED0 void hintConstructActor(daTag_Hint_c* actor)
ext 02008FEC void hintConstructLineCheck(hint::dBgS_CamLinChk* check)
ext 024F1AFC void hintSetLineCheck(hint::dBgS_CamLinChk* check, cXyz* start, cXyz* end, u32 actor)
ext 02008860 s32 hintLineCross(dBgS* background, hint::dBgS_CamLinChk* check)
ext 02008B4C void hintDestroyLineCheck(hint::dBgS_CamLinChk* check, s32 flags)
data l_msgId 1046E2E4 nocheck
data l_messageControl 101F4B5C
ext 025E1988 void hintSeStart(u32 sound)
ext 025F795C u32 hintMessageStatus(u32 control)
ext 025F74D0 void hintSetMessageStatus(u32 control, u32 status)
ext 025F7DB0 u32 hintMessageSet(u32 control, u32 message, const void* position)
ext 024EBA68 void hintRequest(void* attentionHint, daTag_Hint_c* actor, s32 priority)
ext 024A9D6C void hintSetLightPosHD()
ext 024A9E28 void hintMakeLightHD()
str 1003F300
data plight 1046E304 nocheck
data elight 1046E328 nocheck
ext 0255A51C void dKy_set_vrboxcol_ratio(f32 value)
ext 0255A458 void dKy_set_fogcol_ratio(f32 value)
ext 0255671C void dKy_fog_startendz_set(f32 start, f32 end, f32 ratio)
ext 0255A418 void dKy_set_bgcol_ratio(f32 value)
ext 0255A2B8 void hintPlightPrioritySet(LIGHT_INFLUENCE* light)
data search_angle_table 101D1A54
data search_angle_table_grab 101D1A64
data action_table 101D1A74
str 1003F310
str 1003F31C
str 1003F32C
str 1003F340
ext 02543F10 s32 hintGetEventIdx(dEvent_manager_c* manager, const char* name, u8 event)
ext 02542D88 s32 hintGetMyStaffId(dEvent_manager_c* manager, const char* name, u32 actor, u32 id)
ext 02542EDC s32 hintGetMyActIdx(dEvent_manager_c* manager, s32 staff, const void* names, s32 count, u32 force, u32 prefix)
ext 025447C8 s32 hintGetIsAddvance(dEvent_manager_c* manager, s32 staff)
ext 02543280 void hintCutEnd(dEvent_manager_c* manager, s32 staff)
*/
