/* Rito postbox, readable WWHD actor source. */
#include "d/actor/d_a_obj_toripost.h"
const gc_tu_data gc_tu = {
    .safeString = 0x10031248,
    .safeStringAssure = 0x023A5CC8,
    .objAcchDt = {0x10031270, 0x10031280},
    .headerStatics = {0x1046C524, 0x101CD468, 0x1046C548},
};
enum : u32 {
    POST_VTBL = 0x100315A0,
    HIO_VTBL = 0x10031290,
    ACCH_CHECK_VTBL = 0x10031260,
    ACCH_POLY_VTBL = 0x10031280,
    ACCH_GROUP_VTBL = 0x10031270
};
static daObjTpost_HIO_c l_HIO;
static daObjTpost_c__letter_data m_letter[13] = {
    {0, 6831, 7, 44035},  {0, 3321, 7, 46339},   {0, 3322, 195, 32003}, {0, 3324, 4, 31491},
    {0, 2053, 4, 40195},  {0, 3325, 1, 31235},   {1, 3510, 203, 45571}, {0, 4424, 4, 35587},
    {0, 6831, 7, 44035},  {1, 3958, 157, 45059}, {0, 6566, 63, 44547},  {1, 3323, 4, 31747},
    {1, 3959, 158, 44803}};
static u8 pay_type[49] = {2, 1, 1, 0, 0, 0, 0, 2, 1, 1, 0, 0, 0, 0, 2, 1, 1, 0, 0, 0, 0, 2, 1, 1, 0,
                          0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2};
static s32 a_anm_bcks_tbl[3] = {4, 5, 6};
static dLib_anm_prm_c a_anm_prm_tbl[5] = {{-1, -1, 0, 8.0, 1.0, 2},
                                          {0, -1, 0, 8.0, 0.0, 0},
                                          {0, -1, 0, 8.0, 1.0, 0},
                                          {1, -1, 0, 8.0, 1.0, 0},
                                          {2, -1, 0, 8.0, 1.0, 2}};
static u32 pay_msg_num[3] = {3308, 3309, 3310};
static s32 m_send_price[3] = {5, 10, 20};
static dCcD_SrcCyl m_cyl_src;
static const char* action_table[4] = {"PRESENT", "SET_ANM", "DISP_LETTER", "DEFAULT_POST"};
static u8 sea[4] = {115, 101, 97, 0};
static u8 hakidasu[9] = {72, 65, 75, 73, 68, 65, 83, 85, 0};
static daObjTpost_c::mode_entry_t mode_tbl[5] = {
    {&daObjTpost_c::modeWaitInit, &daObjTpost_c::modeWait, "WAIT"},
    {&daObjTpost_c::modeTalkInit, &daObjTpost_c::modeTalk, "TALK"},
    {&daObjTpost_c::modeTalkXYInit, &daObjTpost_c::modeTalkXY, "TALK_XY"},
    {&daObjTpost_c::modeReceiveInit, &daObjTpost_c::modeReceive, "RECEIVE"},
    {&daObjTpost_c::modeReceiveDemoInit, &daObjTpost_c::modeReceiveDemo, "RECEIVE_DEMO"}};
static u32 debugColorGuard;
static u32 debugColor;
static u32 debugColorSource = 0xFF000080;
inline daObjTpost_c__letter_data *letter(s32 index) { return &m_letter[index]; }
inline u16 letterEvent(s32 index) { return letter(index)->mEventReg; }
/* 023A3CC4 */
BOOL daObjTpost_c::_createHeap() {
    gabi::Local<PostString> name;
    gabi::Local<be<u32>> nameLinkage; // HD: preserve caller linkage below the live resource name.
    name->text.v = gabi::guest_addr("Toripost");
    name->vt.v = gc_tu.safeString;
    J3DModelData *modelData =
        postGetObjectRes(dComIfG_resControl(), name, dRes_INDEX_TORIPOST_BDL_VPOST_e);
    if (!modelData)
        JUT_ASSERT_fail("d_a_obj_toripost.cpp", 0x87, "modelData != NULL");
    mMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, nullptr, -1, 1.0f, 0, -1,
                                   1, nullptr, 0x80000, 0x11000022);
    if (!mMorf || !mMorf->getModel())
        return false;
    J3DModel *model = mMorf->getModel();
    model->mUserArea = gabi::ea(this);
    return true;
}

/* 023A3DCC */
static BOOL createHeapCB(daObjTpost_c *a) { return a->_createHeap(); }

/* 023A3DD0 */
void daObjTpost_c::setAnm(s32 index, BOOL force) {
    if (index != AnmPrm_NULL)
        mAnmPrmIdx = index;
    s32 bck = mBckIdx;
    mDoExt_McaMorf *morf = mMorf;
    bool reloadMorf = true;
    if (bck == BckIdx_POST_GET) {
        if (morf->getFrame() == 1.0f) {
            postSeStart(JA_SE_OBJ_POST_EAT_LUGGAGE);
            bck = mBckIdx;
        } else {
            reloadMorf = false;
        }
    }
    if (reloadMorf && bck == BckIdx_POST_PUTOUT) {
        gabi::Local<cXyz> size;
        gabi::Local<be<u32>> sizeLinkage; // HD: outgoing particle call linkage.
        size->x = 1.0f;
        size->y = 1.0f;
        size->z = 1.0f;
        morf = mMorf; // HD: reload after the sound call, before testing the frame.
        if (morf->getFrame() == 1.0f) {
            dComIfG_inf_c *info = postInfo();
            postParticleSet(info->play.mParticle, 0, 0x8190, &current.pos, &current.angle, size,
                            255, nullptr, -1, nullptr, nullptr, nullptr);
            postSeStart(JA_SE_OBJ_POST_LUGGAGE_OUT);
        } else {
            reloadMorf = false;
        }
    }
    if (reloadMorf)
        morf = mMorf;
    postSetAnm("Toripost", morf, &mBckIdx, &mAnmPrmIdx, &mOldAnmPrmIdx, a_anm_bcks_tbl,
               a_anm_prm_tbl, force, 0);
}

/* 023A3F08 */
void daObjTpost_c::setMtx() {

    J3DModel *model = mMorf->getModel();

    model->mBaseScale.x = scale.x;
    model->mBaseScale.y = scale.y;
    model->mBaseScale.z = scale.z;

    auto *matrix = mDoMtx_stack_c::get();

    PSMTXTrans(matrix, (f32)current.pos.x, (f32)current.pos.y, (f32)current.pos.z);

    mDoMtx_YrotM(matrix, (s16)shape_angle.y);

    for (u32 i = 0; i < 12; i++)
        model->mBaseTransformMtx.m[i / 4][i % 4] = matrix->m[i / 4][i % 4];
}

/* 023A3FE8 */
u32 daObjTpost_c::checkSendPrice() {
    gabi::Local<PostString> left, right;
    gabi::Local<be<u32>> stringLinkage; // HD: live SafeString comparison linkage.
    left->text = sea;
    left->vt.v = gc_tu.safeString;
    dComIfG_inf_c *info = postInfo();
    right->text = reinterpret_cast<const be<u8> *>(info->play.mCurStage.mName);
    right->vt.v = gc_tu.safeString;
    postTerminate(left);
    postTerminate(left);
    const be<u8> *first = left->text;
    postTerminate(right);
    bool equal = first == right->text.get();
    if (!equal) {
        const be<u8> *a = left->text;
        const be<u8> *b = right->text;
        // HD: SafeString comparison has a fixed scan limit and reloads both string pointers.
        for (u32 i = 0; i < 0x40001; i++) {
            u8 c = a[i];
            if (c != b[i])
                break;
            if (!c) {
                equal = true;
                break;
            }
        }
    }
    return equal ? pay_type[(s32)(s8)current.roomNo] : 0;
}

/* 023A40E0 */
void daObjTpost_c::modeProc(s32 proc, s32 mode) {
    if (proc == PROC_INIT_e) {
        mCurMode = mode;
        (this->*mode_tbl[mode].init)();
    } else if (proc == PROC_EXEC_e) {
        (this->*mode_tbl[mCurMode].run)();
    }
}

/* 023A418C */
void daObjTpost_c::createInit() {

    if (dComIfGs_isSymbol(2))
        dLetter_autoStock(0xB503);

    if (dComIfGs_checkGetItem(0x31))
        dLetter_autoStock(0x7D03);

    if (dLetter_isDelivery(0xAC03) && dComIfGs_isStageBossEnemy(6))
        dLetter_autoStock(0x7C03);

    if (dComIfGs_isEventBit(0x1E80))
        dLetter_autoStock(0x7B03);

    u8 wallet = dComIfGs_getWalletSize();
    if (wallet == 1 || wallet == 2)
        dLetter_autoStock(0x7A03);

    field_0x8F0 = 1;
    mCurrMsgBsPcId = 0xFFFFFFFF;

    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 6;
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 5;
    attention_info.flags =
        fopAc_Attn_LOCKON_TALK_e | fopAc_Attn_ACTION_SPEAK_e | fopAc_Attn_TALKFLAG_CHECK_e;

    setAnm(AnmPrm_POST_GET0, false);
    setMtx();
    mMorf->calc();

    J3DModel *model = mMorf->getModel();
    cullMtx = gabi::ea(model->getBaseTRMtx());

    fopAcM_setCullSizeBox(this, -50.0f, 0.0f, -50.0f, 70.0f, 200.0f, 70.0f);
    cullSizeFar = 10.0f;

    mStts.Init(255, 255, this);
    mCyl.Set(&m_cyl_src);
    mCyl.SetStts(&mStts);

    mPayType = checkSendPrice();
    modeProc(PROC_INIT_e, MODE_WAIT);
    postCreateInitEffect(&current.pos);
    mEventCut.setActorInfo2("Tpost", this);
}

/* 023A4370 */
s32 daObjTpost_c::_create() {

    u32 flags = actor_condition;

    if (!(flags & 8)) {
        if (gabi::ea(this)) {
            postNpcCtor(this);
            __vtbl = POST_VTBL;
            postAcchCtor(&mAcch);

            mAcch.cBgS_Chk::__vtbl = ACCH_CHECK_VTBL;
            mAcch.dBgS_PolyPassChk::__vtbl = ACCH_POLY_VTBL;
            mAcch.mbObjThrough = 1;
            mAcch.dBgS_GrpPassChk::__vtbl = ACCH_GROUP_VTBL;
            postAcchCirCtor(&mAcchCir);
            flags = actor_condition;
        }
        actor_condition = flags | 8;
    }
    s32 phase = dComIfG_resLoad(&mPhs, "Toripost");

    if (phase == 4) {
        if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void *)createHeapCB), 0x7E0))
            return 5;
        createInit();
    }
    return phase;
}

/* 023A4470 */
static s32 createActor(daObjTpost_c *a) { return a->_create(); }

/* 023A4474 */
BOOL daObjTpost_c::_delete() {
    dComIfG_resDelete(&mPhs, "Toripost");
    return true;
}

/* 023A44A4 */
static BOOL removeActor(daObjTpost_c *a) { return a->_delete(); }

/* 023A44A8 */
s32 daObjTpost_c::getReadableLetterNum() {

    s32 start = mNumReadable;
    if (!start)
        start = 1;

    for (s32 i = start; i < 13; i++)
        if (dLetter_isStock(letterEvent(i)))
            return i;
    return 0;
}

/* 023A4548 */
void daObjTpost_c::checkOrder() {

    u16 command = eventInfo.getCommand();

    if (command == 2) {
        mEventIdx = 0;
        return;
    }
    if (command == 1 && ((s8)mEventIdx == 1 || (s8)mEventIdx == 2)) {
        mEventIdx = 0;
        if (dComIfGp_event_chkTalkXY())
            field_0x8F5 = 1;
        else
            field_0x8F4 = 1;
    }
}

/* 023A45F4 */
void daObjTpost_c::setAttention() {

    f32 z = current.pos.z, y = current.pos.y;
    attention_info.position.z = z;
    attention_info.position.y = y;
    f32 x = current.pos.x;

    attention_info.position.x = x;
    eyePos.z = z;
    eyePos.y = y;
    eyePos.x = x;

    attention_info.position.y = y + l_HIO.attn_pos_offset;
    eyePos.y = y + l_HIO.eye_pos_offset;
}

/* 023A463C */
void daObjTpost_c::cutSetAnmStart(s32 staff) {

    const be<u8> *name = postGetSubstance(&postInfo()->play.mEvtManager, staff, "Name", 4);

    bool equal = false;

    if (name) {
        u32 i = 0;
        for (;; i++) {
            u8 a = name[i], b = hakidasu[i];
            if (a != b)
                break;
            if (!a) {
                equal = true;
                break;
            }
        }
    }
    setAnm(equal ? AnmPrm_POST_PUTOUT : AnmPrm_POST_GET0, false);
}

/* 023A46E4 */
void daObjTpost_c::cutDispLetterStart(s32 staff) { mCurrMsgNo = letter(mNumReadable)->mMsgNo; }

/* 023A4700 */
void daObjTpost_c::cutPresentProc(s32 staff) {

    u8 item = letter(mNumReadable)->mItemNo;

    s32 id = postCreatePresent(&current.pos, item, 0, -1, -1, 0, 0);
    dComIfG_inf_c *info = postInfo();

    if (id != -1) {
        info->play.mEvtCtrl.mPtItem = id;
        info = postInfo();
    }
    postCutEnd(&info->play.mEvtManager, staff);
}

/* 023A4798 */
void daObjTpost_c::cutSetAnmProc(s32 staff) {
    if (mMorf->isStop())
        dComIfGp_evmng_cutEnd(staff);
}

/* 023A47F4 */
void daObjTpost_c::cutDispLetterProc(s32 staff) {
    u16 status = talk(0);

    if (status == 5) {
        daPy_py_c *player = reinterpret_cast<daPy_py_c *>(postInfo()->play.getPlayer(0));
        player->mNoResetFlg1 |= 0x4000;
    } else if (status == 18)
        dComIfGp_evmng_cutEnd(staff);
}

/* 023A486C */
void daObjTpost_c::cutProc() {

    s32 staff = postGetStaff(&postInfo()->play.mEvtManager, "Tpost", 0, 0);
    if (staff == -1)
        return;

    s32 act = postGetAct(&postInfo()->play.mEvtManager, staff, action_table, 3, 1, 0);

    dComIfG_inf_c *info = postInfo();
    if (act == -1) {
        postCutEnd(&info->play.mEvtManager, staff);
        return;
    }
    if (postGetAdvance(&info->play.mEvtManager, staff)) {
        if (act == 1)
            cutSetAnmStart(staff);
        else if (act == 2)
            cutDispLetterStart(staff);
    }
    if (act == 0)
        cutPresentProc(staff);
    else if (act == 1)
        cutSetAnmProc(staff);
    else if (act == 2)
        cutDispLetterProc(staff);
}

/* 023A4A04 */
void daObjTpost_c::eventOrder() {

    s32 event = mEventIdx;

    if (event == 1 || event == 2) {
        event = mEventIdx;
        eventInfo.onCondition(dEvtCnd_CANTALK_e | dEvtCnd_CANTALKITEM_e);
        if (event == 1)
            fopAcM_orderSpeakEvent(this);
    } else if (event >= 3)
        postOrderEvent(this, action_table[event], 1, 0x14F);
}

/* 023A4A60 */
BOOL daObjTpost_c::_execute() {

    s32 readable = dComIfGs_isSymbol(1);
    if (readable)
        readable = getReadableLetterNum();
    mNumReadable = readable;

    checkOrder();
    setAttention();
    setCollision(40.0f, 140.0f);
    modeProc(PROC_EXEC_e, MODE_NULL);

    if (postInfo()->play.mEvtCtrl.mMode && !mEventCut.cutProc())
        cutProc();
    eventOrder();

    mStts.Move();

    if (postChkTgHit(&mCyl))
        postHitSe(&eyePos, (s32)(s8)current.roomNo, &mCyl, 11);

    postHitEffect(this, &mCyl);
    postRollPlayerCrash(this, 7, 40.0f);

    mMorf->play(nullptr, 0, 0);
    mMorf->calc();
    setAnm(5, false);
    return false;
}

/* 023A4BB0 */
static BOOL executeActor(daObjTpost_c *a) { return a->_execute(); }

/* 023A4BB4 */
static void debugDraw(daObjTpost_c *a) {

    if (!debugColorGuard) {
        debugColorGuard = 1;
        postCopyDebugColor(&debugColor, &debugColorSource, 4);
    }
}

/* 023A4BE4 */
BOOL daObjTpost_c::_draw() {

    if (l_HIO.debug_draw)
        debugDraw(this);

    J3DModel *model = mMorf->getModel();

    dScnKy_env_light_c *light = dKy_getEnvlight();
    settingTevStruct(light, 1, &current.pos, &tevStr);

    light = dKy_getEnvlight();
    setLightTevColorType(light, model, &tevStr);

    mMorf->entryDL();
    return true;
}

/* 023A4C64 */
static BOOL drawActor(daObjTpost_c *a) { return a->_draw(); }

/* 023A4C68 */
void daObjTpost_c::modeWaitInit() {
    field_0x8F0 = 1;
    setAnm((s32)mNumReadable ? 4 : 1, false);
}

/* 023A4C88 */
BOOL daObjTpost_c::checkTalk() {
    daPy_py_c *player = reinterpret_cast<daPy_py_c *>(postInfo()->play.getPlayer(0));

    f32 distance = postPlayerDistance(this, player);
    return distance < l_HIO.talk_distance;
}

/* 023A4CD8 */
void daObjTpost_c::modeWait() {
    setAnm((s32)mNumReadable ? 4 : 1, false);
    if (field_0x8F4)
        modeProc(PROC_INIT_e, MODE_TALK);
    else if (field_0x8F5)
        modeProc(PROC_INIT_e, MODE_TALK_XY);
    else if (checkTalk())
        mEventIdx = 2;
}

/* 023A4D90 */
void daObjTpost_c::modeTalkInit() { setAnm(AnmPrm_POST_GET0, false); }

/* 023A4D9C */
void daObjTpost_c::modeTalk() {
    if (talk(1) == 18) {
        if (field_0x8E9) {
            modeProc(PROC_INIT_e, MODE_RECEIVE);
            field_0x8E9 = 0;
        } else
            modeProc(PROC_INIT_e, MODE_WAIT);
        dComIfGp_event_reset();
        field_0x8F4 = 0;
    }
}

/* 023A4E34 */
void daObjTpost_c::modeTalkXYInit() {
    setAnm(AnmPrm_POST_GET0, false);
    mPreItemNo = dComIfGp_event_getPreItemNo();
    field_0x8DC = l_HIO.field_0x14;
    field_0x8E0 = l_HIO.field_0x16;
}

/* 023A4E8C */
void daObjTpost_c::modeTalkXY() {

    dComIfG_inf_c *info = postInfo();
    s32 index = mAnmPrmIdx;
    daPy_py_c *player = reinterpret_cast<daPy_py_c *>(info->play.getPlayer(0));

    auto demo = [&](u32 mode) {
        player->mDemo.mParam0 = 0;
        player->mDemo.mDemoMode = mode;
        player->mDemo.mDemoType = 3;
    };

    if (index == 3) {
        if (field_0x8EA) {
            field_0x8EA = 0;
            field_0x8E4 = 10;
        }
        if ((s32)field_0x8E4 != -1 && !postCalcTimer(&field_0x8E4)) {
            demo(24);
            field_0x8E4 = -1;
        }
        u32 msg = mCurrMsgNo;

        if (msg == 0xCE9 || msg == 0xCEA || msg == 0xCF0 || msg == 0xCF1) {
            if (postPlayerFrameRate(player) == 0.0f)
                demo(1);
        }
        index = mAnmPrmIdx;
    }
    if (index == 1) {
        if (postPresentEnd(&postInfo()->play.mEvtManager) && !postCalcTimer(&field_0x8E0)) {
            postCancelPresent(&postInfo()->play.mEvtManager);
            setAnm(2, false);
        }
        index = mAnmPrmIdx;
    }
    if ((index == 2 || index == 3) && mMorf->isStop() && !postCalcTimer(&field_0x8DC) &&
        talk(1) == 18) {
        modeProc(PROC_INIT_e, MODE_WAIT);
        dComIfGp_event_reset();
        field_0x8F5 = 0;
    }
}

/* 023A5070 */
void daObjTpost_c::modeReceiveInit() {
    dComIfGp_event_reset();
    mEventIdx = 3;
    setAnm(AnmPrm_POST_GET0, false);
}

/* 023A50C0 */
void daObjTpost_c::modeReceive() { modeProc(PROC_INIT_e, MODE_RECEIVE_DEMO); }

/* 023A50CC */
void daObjTpost_c::modeReceiveDemo() {

    if (postEventEnd(&postInfo()->play.mEvtManager, "DEFAULT_POST")) {
        dLetter_read(letterEvent(mNumReadable));
        mNumReadable = getReadableLetterNum();
        dComIfGp_event_reset();

        if ((s32)mNumReadable) {
            modeProc(PROC_INIT_e, MODE_TALK);
            mEventIdx = 1;
            field_0x8EB = 1;
            field_0x8F0 = (u32)field_0x8F0 + 1;
        } else {
            modeProc(PROC_INIT_e, MODE_WAIT);
            field_0x8F0 = 1;
        }
    }
}

/* 023A5198 */
void daObjTpost_c::deliverLetter() {
    u8 item = mPreItemNo;
    if (item == 0x99)
        dLetter_send(0xAC03);
    else if (item == 0x9A)
        dComIfGs_onEventBit(0x1220);
}

/* 023A51CC */
s16 daObjTpost_c::getReceiveLetterNum() {
    s16 n = 0;
    for (s32 i = 1; i < 13; i++)
        if (dLetter_isStock(letterEvent(i)))
            n++;
    return n;
}

/* 023A5238 */
static f32 colorGamma(u32 color) {

    f32 input = (f32)color / 255.0f;
    f32 out = postPow(input, 2.2f);

    if (out < 0.0f)
        out = 0.0f;
    else if (out > 1.0f)
        out = 1.0f;
    return out;
}

/* 023A52B8 */
u32 daObjTpost_c::getMsgXY() {

    dComIfGp_get();
    // HD: debug position and RGB registers, with component load order preserved.
    u8 item = mPreItemNo;

    gabi::Local<cXyz> pos;
    pos->y = REG_F(12, 1);
    pos->x = REG_F(12, 0);
    pos->z = REG_F(12, 2);

    gabi::Local<cXyz> size;
    size->x = 2.0f;
    size->y = 2.0f;
    size->z = 2.0f;
    gabi::Local<GXColor> color;
    gabi::Local<be<u32>> effectLinkage;
    color->a = 128;

    if (item == 0x99 || item == 0x9A) {
        u8 r = REG_S(12, 0) + 128, g = REG_S(12, 1) + 128, b = REG_S(12, 2) + 128;

        color->r = r;
        color->g = g;
        color->b = b;

        color->r = (u8)gabi::ftoi(colorGamma(r) * 255.0f);
        color->g = (u8)gabi::ftoi(colorGamma(color->g) * 255.0f);
        color->b = (u8)gabi::ftoi(colorGamma(color->b) * 255.0f);

        J3DModel *model = mMorf->getModel();
        auto *data = reinterpret_cast<PostMatrixBuffer *>(model->mpMtxBuffer.get());
        u16 flags = data->flags;
        Mtx34 *matrices = data->matrices;
        data->flags = flags | 0x10;

        auto *matrix = mDoMtx_stack_c::get();
        PSMTXCopy(&matrices[2], matrix);
        PSMTXMultVec(matrix, pos, pos);

        dPa_control_c *control = postInfo()->play.mParticle;
        postParticleSet(control, 0, 0x57, pos.get(), &shape_angle, size.get(), 255, 0, -1,
                        color.get(), 0, 0);
        return 0xCE8;
    }
    color->r = 0;
    color->g = 0;
    color->b = 0;
    setAnm(AnmPrm_POST_PUTOUT, false);
    field_0x8EA = 1;
    return (item == 0x98 || item == 0x9B) ? 0xCEA : 0xCE9;
}

/* 023A5500 */
u32 daObjTpost_c::getMsgNormal() {
    if (field_0x8EB) {
        s16 n = mLetterOrdinalLow;
        dComIfGp_setMessageCountNumber(n);
        field_0x8EB = 0;
        return 0xCF7;
    }
    return postDayNight() ? 0xCE6 : 0xCE5;
}

/* 023A5570 */
u32 daObjTpost_c::getMsg() { return dComIfGp_event_chkTalkXY() ? getMsgXY() : getMsgNormal(); }

/* 023A55BC */
u16 daObjTpost_c::next_msgStatus(be<u32> *message) {

    msg_class *msgControl = fopNpc_npc_c::mpCurrMsg.get();
    dComIfGp_get();
    u16 status = 15;

    switch ((u32)*message) {
    case 0xCE5:
    case 0xCE6:
        if ((s32)mNumReadable) {
            s16 n = getReceiveLetterNum();
            dComIfGp_setMessageCountNumber(n);
            *message = 0xCEB;
        } else
            *message = 0xCE7;
        break;

    case 0xCEB:
        dComIfGp_setMessageCountNumber(mLetterOrdinalLow);
        *message = 0xCF7;
        break;

    case 0xCF7:
        if (l_HIO.field_0x07 || letter(mNumReadable)->field_0x00)
            *message = 0xCF3;
        else {
            field_0x8E9 = 1;
            status = 16;
        }
        break;

    case 0xCE8:
        *message = pay_msg_num[mPayType];
        break;

    case 0xCEC:
    case 0xCED:
    case 0xCEE:
        *message = 0xCEF;
        break;

    case 0xCEF:
        if (!msgControl->mSelectNum) {
            u8 type = mPayType;
            dSv_info_c *savedata = dComIfGs_info();
            s32 price = m_send_price[type];
            u16 money = savedata->mSavedata.mPlayer.mPlayerStatusA.mRupee;

            if (money >= price) {
                dComIfG_inf_c *info = postInfo();
                info->play.mItemRupeeCount = (u32)info->play.mItemRupeeCount - price;
                dComIfGs_setReserveItemEmpty();
                deliverLetter();
                *message = 0xCF2;
            } else {
                setAnm(AnmPrm_POST_PUTOUT, false);
                field_0x8EA = 1;
                *message = 0xCF1;
            }
        } else {
            setAnm(AnmPrm_POST_PUTOUT, false);
            field_0x8EA = 1;
            *message = 0xCF0;
        }
        break;

    case 0xCF3:
        *message = letterEvent(mNumReadable) == 0xB203 ? 0xCF8 : 0xCF4;
        break;

    case 0xCF4:
    case 0xCF8:
        if (!msgControl->mSelectNum) {
            s32 i = mNumReadable;
            dSv_info_c *savedata = dComIfGs_info();
            u16 event = letterEvent(i), money = savedata->mSavedata.mPlayer.mPlayerStatusA.mRupee;
            s32 price = event == 0xB203 ? 201 : 10;

            if (money >= price) {
                dComIfG_inf_c *info = postInfo();
                info->play.mItemRupeeCount = (u32)info->play.mItemRupeeCount - price;
                field_0x8E9 = 1;
                status = 16;
            } else
                *message = 0xCF6;

        } else
            *message = 0xCF5;
        break;

    case 0xCF5:
    case 0xCF6:
        mNumReadable = (s32)((u32)(s32)mNumReadable + 1);
        field_0x8F0 = (u32)field_0x8F0 + 1;
        mNumReadable = getReadableLetterNum();

        if ((s32)mNumReadable) {
            dComIfGp_setMessageCountNumber(mLetterOrdinalLow);
            *message = 0xCF7;
        } else
            status = 16;
        break;

    default:
        status = 16;
        break;
    }
    s32 index = mNumReadable;
    u32 current = mCurrMsgNo;
    if (current == letter(index)->mMsgNo && status == 16)
        status = 14;
    return status;
}

// HD: compiler-generated and template code of this translation unit.
/* 023A5AB4 */
static daObjTpost_HIO_c *createHIO(daObjTpost_HIO_c *h) {

    if (!gabi::ea(h)) {
        h = static_cast<daObjTpost_HIO_c *>(operator_new(0x18));
        if (!h)
            return h;
    }
    h->field_0x06 = 0;
    h->mNo = -1;
    h->eye_pos_offset = 100.0f;
    h->talk_distance = 300.0f;
    h->field_0x16 = 30;
    h->attn_pos_offset = 140.0f;
    h->field_0x07 = 0;
    h->field_0x14 = 30;
    h->debug_draw = 0;
    h->vt = HIO_VTBL;
    return h;
}

/* 023A5B3C */
static void initializeStatics() {
    __sinit_header_statics();
    createHIO(&l_HIO);
}

/* 023A5BDC */
static void deleteHIO(daObjTpost_HIO_c *h, u32 flags) {
    if (gabi::ea(h) && (flags & 1))
        operator_delete(h);
}

/* 023A5BF0 */
static BOOL isDelete(daObjTpost_c *a) { return true; }

/* 023A5BF8 */
void daObjTpost_c::modeReceiveDemoInit() {}

/* 023A5BFC */
static void deleteInstance(daObjTpost_c *a, u32 flags) {
    if (!gabi::ea(a))
        return;
    __dt(&a->mAcchCir);
    __dt(&a->mAcch);
    __dt(static_cast<fopNpc_npc_c *>(a));
    if (flags & 1)
        operator_delete(a);
}

/* 023A5CC8 */
static void getArg(daObjTpost_c *a) {}
/* WWHD glue (tools/verify/autoglue.py)
ext 02586E94 void dLetter_autoStock(u32)
ext 02586ED0 u32 dLetter_isDelivery(u32)
ext 02586DC8 s32 dLetter_isStock(u32)
ext 02586E04 void dLetter_read(u32)
ext 02586D5C void dLetter_send(u32)
ext 02588BDC void postCreateInitEffect(cXyz*)
str 100314B4 in 023A3CC4
ext 026066C4 J3DModelData* postGetObjectRes(dRes_control_c*, PostString*, s32)
ext 025E1988 void postSeStart(u32)
ext 025A847C void postParticleSet(dPa_control_c*, u32, u32, cXyz*, csXyz*, cXyz*, u32, void*, s32, GXColor*, void*, void*)
ext 02587A0C void postSetAnm(const char*, mDoExt_McaMorf*, be<s8>*, be<s8>*, be<s8>*, const be<s32>*, const dLib_anm_prm_c*, BOOL, u32)
ext 0254487C const be<u8>* postGetSubstance(dEvent_manager_c*, s32, const char*, s32)
ext 025D7DEC s32 postCreatePresent(cXyz*, u32, u32, s32, s32, u32, u32)
ext 02543280 void postCutEnd(dEvent_manager_c*, s32)
ext 02542D88 s32 postGetStaff(dEvent_manager_c*, const char*, void*, s32)
ext 02542EDC s32 postGetAct(dEvent_manager_c*, s32, const gptr<char>*, s32, BOOL, s32)
ext 025447C8 BOOL postGetAdvance(dEvent_manager_c*, s32)
ext 025D77DC void postOrderEvent(fopAc_ac_c*, const char*, u32, u32)
ext 023129C4 void postHitSe(cXyz*, s32, dCcD_Cyl*, s32)
ext 02312C8C void postHitEffect(fopAc_ac_c*, dCcD_Cyl*)
ext 025D69FC void postRollPlayerCrash(fopAc_ac_c*, s32, f32)
ext C000A848 void postCopyDebugColor(void*, const void*, u32)
ext 025D6958 f32 postPlayerDistance(fopAc_ac_c*, fopAc_ac_c*)
ext 0211D2F8 s32 postCalcTimer(be<s32>*)
ext 02544980 void postCancelPresent(dEvent_manager_c*)
ext 0254457C BOOL postEventEnd(dEvent_manager_c*, const char*)
ext 028F4560 f32 postPow(f32, f32)
ext 025A1458 void postNpcCtor(fopNpc_npc_c*)
ext 024F0474 void postAcchCtor(dBgS_Acch*)
ext 024EFE94 void postAcchCirCtor(dBgS_AcchCir*)
ext 02515E50 void postMoveStatus(void*)
ext 02544950 BOOL postPresentEnd(dEvent_manager_c*)
ext 025162A4 BOOL postChkTgHit(dCcD_Cyl*)
ext 02556D14 BOOL postDayNight()
str 100312A4 in 023A3CC4
str 100312BC "modelData != NULL" in 023A3CC4
str 100314B4 in 023A3DD0
str 1003140C in 023A418C
str 100314B4 in 023A4370
str 100314B4 in 023A4474
str 10031414 in 023A463C
str 10031428 in 023A486C
str 10031464 in 023A50CC
data l_HIO 1046C530
data m_letter 10031504
data pay_type 10031330
data a_anm_bcks_tbl 100312D0
data a_anm_prm_tbl 100312DC
data pay_msg_num 1003148C
data m_send_price 100314A8
data m_cyl_src 100314C0
data action_table 101CD458
data sea 1003132C
data hakidasu 1003141C
data mode_tbl 10031364
data debugColorGuard 101FDA50
data debugColor 101FEBF4
data debugColorSource 10031240
*/
