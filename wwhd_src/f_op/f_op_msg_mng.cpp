/* Message process manager, tww names and verified HD behavior. Local only. */
#include "f_op/f_op_msg_mng.h"
using Flag = be<u8>;
using Word = be<u32>;
using MessageTable = fopMsgTableEntryHD[1];
using StickVtable = u8[4];
static Flag demoFlag;
static Word mStageProcID;
static Word g_fpcNd_type;
static MessageTable lMessageTable;
static STControl l_stick;
static StickVtable l_stickVtable;
// HD: GHS passes floats 9..12 as stack doubles; retain this outgoing-call ABI adapter.
static inline void J2DPicture_drawTexCoordHD(J2DPicture* pic, Mtx* mtx, f32 a1, f32 a2, f32 a3,
                                             f32 a4, f32 a5, f32 a6, f32 a7, f32 a8, f32 s9,
                                             f32 s10, f32 s11, f32 s12) {
    Cpu* c = gabi::cpu;
    c->r[3] = gabi::ea(pic);
    c->r[4] = gabi::ea(mtx);
    const f32 fr[8] = {a1, a2, a3, a4, a5, a6, a7, a8};
    for (int i = 0; i < 8; i++)
        c->f[i + 1].ps0 = c->f[i + 1].ps1 = (double)fr[i];
    u32 sp = c->r[1];
    c->r[1] = sp - 48;
    const f64 st[4] = {s9, s10, s11, s12};
    for (int i = 0; i < 4; i++) {
        u64 b;
        memcpy(&b, &st[i], 8);
        gabi::store<u64>(c->r[1] + 8 + 8 * i, b);
    }
    gmem_call(c, 0x027EFBCC, 0, 2, 8);
    c->r[1] = sp;
}
// HD: drawing returns without a matrix and preserves PPC operand order.
/* 025DB90C */
void MyPicture::drawSelf(f32 i_posx, f32 i_posy, Mtx* i_mtx) {
    if (i_mtx == 0)
        return;
    if (mpTexture[0] == 0)
        return;
    f32 y = gabi::fadds_ppc(mGlobalBounds.i.y, i_posy);
    f32 x = gabi::fadds_ppc(mGlobalBounds.i.x, i_posx);
    f32 w = gabi::fsubs_ppc(mBounds.f.x, mBounds.i.x);
    f32 h = gabi::fsubs_ppc(mBounds.f.y, mBounds.i.y);
    u8 flags = mMirror;
    u32 binding = mBinding;
    gabi::call_ptr(__vtbl->drawFullSet2, this, x, y, w, h, binding, (u32)(flags & 3),
                   (u32)((flags >> 2) & 1), i_mtx);
}

// HD: mirror and tumble arguments remain full-width; a null matrix skips drawing.
/* 025DB96C */
void MyPicture::drawFullSet2(f32 i_posx, f32 i_posy, f32 i_width, f32 i_height, u32 i_binding,
                             u32 i_mirror, u32 i_tumble, Mtx* i_mtx) {
    if (i_mtx == 0)
        return;
    f32 t1 = m130;
    f32 s1 = m12C;
    f32 t0 = m128;
    f32 s0 = m124;
    if (i_mirror & J2DMirror_X) {
        f32 tmp = s0;
        s0 = s1;
        s1 = tmp;
    }
    if (i_mirror & J2DMirror_Y) {
        f32 tmp = t0;
        t0 = t1;
        t1 = tmp;
    }
    if (m134 != 0) {
        if (i_tumble == 0)
            J2DPicture_drawTexCoordHD(this, i_mtx, 0.0f, 0.0f, i_width, i_height, s0, t0, s1, t0,
                                      s0, t1, s1, t1);
        else
            J2DPicture_drawTexCoordHD(this, i_mtx, 0.0f, 0.0f, i_width, i_height, s0, t1, s0, t0,
                                      s1, t1, s1, t0);
    } else {
        J2DPicture_drawFullSetHD(this, i_posx, i_posy, i_width, i_height, i_binding, i_mirror,
                                 i_tumble, i_mtx);
    }
}

/* 025DB2BC */
s32 fopMsgM_setStageLayer(base_process_class* i_proc) {
    process_node_class* stageProc = fopScnM_SearchByIDHD(mStageProcID);
    if (stageProc == 0) {
        return (s32)JUTAssertion_showAssertResultHD("f_op_msg_mng.cpp", 0xC0, "stageProc != (0)");
    }
    u32 layer = 0xFFFFFFFF;
    if (fpcBs_Is_JustOfTypeHD(g_fpcNd_type, stageProc->mSubType))
        layer = stageProc->mLayer.mLayerID;
    return fpcPi_ChangeHD(i_proc->mPi, layer, fpcPi_CURRENT_e, fpcPi_CURRENT_e);
}

// HD: append creation is inlined; process name and optional words keep full register widths.
/* 025DB374 */
s32 fopMsgM_create(u32 i_procName, fopAc_ac_c* i_actor, cXyz* i_pos, be<u32>* i_msgNo,
                   be<u32>* param_5, u32 i_createFunc) {
    auto* params = static_cast<fopMsg_prm_class*>(cMl::memalignB(-4, sizeof(fopMsg_prm_class)));
    if (params == 0)
        return -1;
    params->mpActor = i_actor;
    if (i_msgNo != 0)
        params->mMsgNo = u32(*i_msgNo);
    if (param_5 != 0)
        params->field_0x14 = u32(*param_5);
    if (i_pos != 0) {
        auto* posWords = reinterpret_cast<fopMsg_positionWordsHD*>(i_pos);
        params->mPosWords.x = u32(posWords->x);
        params->mPosWords.y = u32(posWords->y);
        params->mPosWords.z = u32(posWords->z);
    } else {
        params->mPosWords.x = 0;
        params->mPosWords.y = 0;
        params->mPosWords.z = 0;
    }
    params->field_0x18 = 0xFFFFFFFF;
    return fpcSCtRq_RequestHD(fpcLy_CurrentLayerHD(), i_procName, i_createFunc, 0, params);
}

// HD: timer append creation is inlined; the timer mode is signed 32-bit.
/* 025DB468 */
s32 fopMsgM_Timer_create(u32 i_procName, s32 i_mode, u16 i_limitTimeMs, u8 i_showType,
                         u8 i_iconType, f32 i_posX, f32 i_posY, f32 i_rupeePosX, f32 i_rupeePosY,
                         u32 i_createFunc) {
    auto* req = static_cast<fopMsg_prm_timer*>(cMl::memalignB(-4, sizeof(fopMsg_prm_timer)));
    if (req != 0) {
        req->mpActor = 0;
        req->mMsgNo = 0;
        req->field_0x14 = 0;
        req->mPosWords.x = 0;
        req->mPosWords.y = 0;
        req->mPosWords.z = 0;
        req->field_0x18 = 0xFFFFFFFF;
        req->mTimerMode = (u32)i_mode;
        req->mLimitTimeMs = i_limitTimeMs;
        req->mShowType = i_showType;
        req->mIconType = i_iconType;
        req->mTimerPos.x = i_posX;
        req->mTimerPos.y = i_posY;
        req->mRupeePos.x = i_rupeePosX;
        req->mRupeePos.y = i_rupeePosY;
    }
    if (req == 0)
        return -1;
    return fpcSCtRq_RequestHD(fpcLy_CurrentLayerHD(), i_procName, i_createFunc, 0, req);
}

/* 025DB58C */
void fopMsgM_demoMsgFlagOn() { demoFlag = 1; }

/* 025DB59C */
void fopMsgM_demoMsgFlagOff() { demoFlag = 0; }

// HD: an eight-byte table entry supplies one byte; the entry count is not verified.
/* 025DB5AC */
u8 fopMsgM_getTableByte(s32 i) { return u8(lMessageTable[u32(i)].mValue); }

// HD: pane lookup omits the GameCube pane-parts initialization.
/* 025DB5C0 */
void fopMsgM_searchPane(gptr<J2DPane>* o_pane, J2DScreen* i_scrn, u32 i_tag) {
    J2DPane* pane = i_scrn->search(i_tag);
    if (pane != 0 && o_pane != 0)
        *o_pane = pane;
}

// HD: offsets are ignored.
/* 025DB648 */
void fopMsgM_paneTrans(fopMsgM_pane_class* i_pane, f32 i_transX, f32 i_transY) {
    if (i_pane == 0)
        return;
    fopMsgM_cposMove(i_pane);
}

/* 025DB654 */
void fopMsgM_paneScaleXY(fopMsgM_pane_class* i_pane, f32 i_scale) {
    if (i_pane == 0)
        return;
    f32 x = f32(i_pane->mSizeOrig.x) * i_scale;
    f32 y = f32(i_pane->mSizeOrig.y) * i_scale;
    i_pane->mSize.x = x;
    i_pane->mSize.y = y;
    fopMsgM_cposMove(i_pane);
}

// HD: pane helpers accept null; positioning updates only the top-left corner.
/* 025DB614 */
void fopMsgM_cposMove(fopMsgM_pane_class* i_pane) {
    if (i_pane == 0)
        return;
    f32 cx = f32(i_pane->mPosCenter.x);
    f32 sx = f32(i_pane->mSize.x);
    f32 cy = f32(i_pane->mPosCenter.y);
    f32 x = gabi::fnmsubs(sx, 0.5f, cx);
    f32 sy = f32(i_pane->mSize.y);
    f32 y = gabi::fnmsubs(sy, 0.5f, cy);
    i_pane->mPosTopLeft.x = x;
    i_pane->mPosTopLeft.y = y;
}

// HD: alpha application is empty.
/* 025DB678 */
void fopMsgM_setAlpha(fopMsgM_pane_class* i_pane) {}

/* 025DB67C */
void fopMsgM_setInitAlpha(fopMsgM_pane_class* i_pane) {
    if (i_pane == 0)
        return;
    i_pane->mNowAlpha = u8(i_pane->mInitAlpha);
}

/* 025DB690 */
void fopMsgM_setNowAlpha(fopMsgM_pane_class* i_pane, f32 i_alpha) {
    if (i_pane == 0)
        return;
    f32 a = (f32)u8(i_pane->mInitAlpha) * i_alpha;
    i_pane->mNowAlpha = (u8)gabi::ftoi(a);
}

/* 025DB6E0 */
void fopMsgM_setNowAlphaZero(fopMsgM_pane_class* i_pane) {
    if (i_pane == 0)
        return;
    i_pane->mNowAlpha = 0;
}

/* 025DB6F4 */
f32 fopMsgM_valueIncrease(s32 i_max, s32 i_value, u8 i_mode) {
    if (i_max <= 0)
        return 1.0f;
    if (i_value < 0)
        i_value = 0;
    else if (i_value > i_max)
        i_value = i_max;
    f32 v = (f32)i_value / (f32)i_max;
    f32 ret = v;
    switch (i_mode) {
    case 0:
        ret = v * v;
        break;
    case 1:
        ret = std_sqrtf(v);
        break;
    case 3: {
        f32 t = (v + v) - 1.0f;
        ret = (t + t) - 1.0f;
        break;
    }
    case 4: {
        u16 ang = (u16)gabi::ftoi((32768.0f * v) * 0.5f);
        f32 s = cM_ssin(ang);
        ret = s * s;
        break;
    }
    case 5: {
        u16 ang = (u16)gabi::ftoi((65535.0f * v) * 0.5f);
        f32 s = cM_ssin(ang);
        ret = s * s;
        break;
    }
    default:
        break;
    }
    return ret;
}

// HD: created heaps receive the diagnostic name.
/* 025DB8A4 */
JKRExpHeap* fopMsgM_createExpHeap(u32 i_size) {
    void* parent = mDoExt_getGameHeapHD();
    JKRExpHeap* heap = JKRExpHeap_createHD(i_size, parent, FALSE);
    if (heap != 0)
        heap->setName("fopMsgM_createExpHeap");
    return heap;
}

/* 025DB8F4 */
void fopMsgM_destroyExpHeap(JKRExpHeap* i_heap) {
    if (i_heap == 0)
        return;
    i_heap->destroy();
}

// HD: compiler-generated and per-TU inline copies follow.
/* 025DBA44 */
void __sinit_f_op_msg_mng_cpp() {
    sinit_header_statics_z(0x1048A4E0, 0x101F3664, 0x1048A514);
    l_stick.__vtbl = gabi::ea(&l_stickVtable);
    l_stick.setWaitParm(0xF, 0xF, 0, 0, 0.9f, 0.5f, 0, 0x2000);
    l_stick.init();
    __register_global_object(0x101F3688);
}

/* 025DBB40 */
u8 fopMsgM_demoMsgFlagCheck() { return u8(demoFlag); }

/* 025DBB4C */
void fopMsgM_emptyObjectHD__dt(void* i_this, s32 i_flag) {
    if (i_this == 0)
        return;
    if ((i_flag & 1) == 0)
        return;
    fopMsgM_operatorDeleteHD(i_this);
}

/* 025DBB60 */
void J2DPane::calcMtx() {
    if (!mpParent)
        return;
    u32 fn = __vtbl->makeMatrix;
    f32 y = mBounds.i.y;
    f32 x = mBounds.i.x;
    gabi::call_ptr(fn, this, x, y);
}

/* 025DBB84 */
void MyPicture__dt(MyPicture* i_this, s32 i_flag) {
    if (i_this == 0)
        return;
    J2DPicture_dtorHD(i_this, 0);
    if ((i_flag & 1) == 0)
        return;
    fopMsgM_operatorDeleteHD(i_this);
}

/* 025DBBD8 */
void fopMsgM_emptyVirtualHD(void* i_this) {}

// clang-format off
/* WWHD glue
data demoFlag 101F3480
data mStageProcID 1047E6C4
data g_fpcNd_type 101F3D60
data lMessageTable 101F3484
data l_stick 1048A4EC
data l_stickVtable 10050788
str 10057BF4 "f_op_msg_mng.cpp"
str 10057C08 "stageProc != (0)"
str 10057C5C "fopMsgM_createExpHeap"
ext 0273AA24 u32 JUTAssertion_showAssertResultHD(const char*,s32,const char*)
ext 025DC80C process_node_class* fopScnM_SearchByIDHD(u32)
ext 025DD258 BOOL fpcBs_Is_JustOfTypeHD(s32,s32)
ext 025E0D38 s32 fpcPi_ChangeHD(void*,u32,u32,u32)
ext 025DED64 u32 fpcLy_CurrentLayerHD()
ext 025E14A8 s32 fpcSCtRq_RequestHD(u32,u32,u32,u32,fopMsg_prm_class*)
ext 025E2FBC void* mDoExt_getGameHeapHD()
ext 027EBCB0 JKRExpHeap* JKRExpHeap_createHD(u32,void*,BOOL)
ext 0273AF40 void fopMsgM_operatorDeleteHD(void*)
ext 027EF9AC void J2DPicture_dtorHD(J2DPicture*,s32)
ext 027EFC6C void J2DPicture_drawFullSetHD(J2DPicture*,f32,f32,f32,f32,u32,u32,u32,Mtx*)
ext 025885C4 void STControl::setWaitParm(s16,s16,s16,s16,f32,f32,s16,u16)
ext 025885E8 void STControl::init()
*/
// clang-format on
