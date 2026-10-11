/* Tower of the Gods glowing staircase. */
#include "d/actor/d_a_lstair.h"
const gc_tu_data gc_tu = {.safeString = 0x1001407C};
static BOOL CheckCreateHeap(fopAc_ac_c*);
// HD: inline construction preserves the embedded animation SDK objects.
daLStair_c::daLStair_c() {
    fopAc_ac_c_ct(this);
    __vtbl = 0x100140BC;
    LStair_frameInit(&mBckAnm.mFrameCtrl, 0);
    mBckState.mVtable = 0x1016E54C;
    LStair_objectConstruct(mBckState.mObject);
    mBckState.field_0x88 = 0;
    mBckState.field_0x58 = 0;
    mBckState.field_0x80 = 0;
    mBckState.mVtable = 0x10014094;
    mBckState.field_0x7C = 0;
    mBckState.field_0x84 = 0;
    mBckState.mObjectVtable = 0x1016D820;
    mDoExt_btkAnm::ct(&mBtkAnm);
    LStair_colorConstruct(&mBpkAnm0);
    LStair_colorConstruct(&mBpkAnm1);
    LStair_tevConstruct(&mBrkAnm);
}
static const char* m_arcname = "Gkai00";
enum {
    dRes_INDEX_GKAI00_BDL_GKAI00_e = 11,
    dRes_INDEX_GKAI00_BCK_GKAI00_e = 8,
    dRes_INDEX_GKAI00_BTK_GKAI00_e = 21,
    dRes_INDEX_GKAI00_BPK_GKAI00_01_e = 14,
    dRes_INDEX_GKAI00_BPK_GKAI00_02_e = 15,
    dRes_INDEX_GKAI00_BRK_GKAI00_e = 18,
    dRes_INDEX_GKAI00_DZB_GKAI00_e = 24,
    JA_SE_OBJ_L_BRIDGE_OFF = 0x6957,
    JA_SE_OBJ_R_STAIR_APPEAR = 0x69B3,
};
namespace dPa_name { enum { ID_IT_SN_RSTAIRS_F00 = 0x8174, ID_IT_SN_RSTAIRS_S00 = 0x8175 }; }
static void* lstair_getObjectRes(const char* arc, s32 index, u32 vtable) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::guest_addr(arc);
    key->__vtbl = vtable;
    auto control = dComIfG_resControl();
// Keep the key above the callee's incoming LR-save/linkage words.
    gabi::Local<be<u32>[4]> linkage;
    return LStair_getResource(control, key.get(), index);
}
static bool switchOn(daLStair_c* a) {
    s32 room = a->home.roomNo;
    s32 no = a->mSwitchNo;
    return dComIfGs_isSwitch(no, room)!=0;
}
static bool enemyGone(daLStair_c* a) {
    return fopAcM_myRoomSearchEnemy(a->current.roomNo)==nullptr;
}
/* 021B5D5C */
void daLStair_c::setMoveBGMtx() {
    f32 y = (current.pos.y-300.0f)+mStairYOffset;
    mDoMtx_stack_c::transS((f32)current.pos.x, y, (f32)current.pos.z);
    mDoMtx_stack_c::YrotM((s16)shape_angle.y);
    LStair_scaleM((f32)scale.x, (f32)scale.y, (f32)scale.z);
    PSMTXCopy(mDoMtx_stack_c::get(), &mBgMtx);
}
/* 021B5DE0 */
void daLStair_c::set_mtx() {
    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS((f32)current.pos.x, (f32)(current.pos.y+mStairYOffset), (f32)current.pos.z);
    mDoMtx_stack_c::YrotM((s16)current.angle.y);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
/* 021B614C */
bool daLStair_c::_delete() {
    if (heap!=nullptr) {
        auto bg = dComIfG_Bgsp();
        bg->Release(mpBgW);
    }
    dComIfG_resDelete(&mPhase, m_arcname);
    return true;
}
/* 021B65F8 */
void daLStair_c::checkAppear() {
    bool sw = switchOn(this), gone = enemyGone(this);
    if (mSwitchNo!=255) {
        if (sw!=mSwitchStatus) {
            if (sw) appear_stair();
            else disappear_stair();
        }
        return;
    }
    if (gone!=mEnemyGone) {
        if (gone) {
            // HD: wait 50 frames and return immediately; GC sets 51 before decrementing.
                mAppearTimer = 50;
            return;
        }
        disappear_stair();
    }
    s8 t = mAppearTimer;
    if (t>0) mAppearTimer = t-1;
    else if (t==0) {
        appear_stair();
        mAppearTimer = (s8)((u8)mAppearTimer-1);
    }
}
/* 021B6820 */
void daLStair_c::moveBG() {
    bool sw = switchOn(this), gone = enemyGone(this);
    f32 target = (mSwitchNo!=255?sw:gone)?0.0f:-300.0f;
    if (mAppearTimer<0 && LStair_calcTimer(&mTimer)==0)
        LStair_addCalc(&mStairYOffset, target, 1.0f/60.0f, 10.0f, 5.0f);
    setMoveBGMtx();
    mpBgW->Move();
}
/* 021B6930 */
bool daLStair_c::_execute() {
    bool sw = switchOn(this), gone = enemyGone(this);
    checkAppear();
    demoMove();
    moveBG();
    mBtkAnm.play();
    mBpkAnm0.play();
    mBpkAnm1.play();
    mBrkAnm.play();
    set_mtx();
    mSwitchStatus = sw;
    mEnemyGone = gone;
    return true;
}
/* 021B673C */
void daLStair_c::demoMove() {
    if (eventInfo.checkCommandDemoAccrpt()) {
        s16 idx = mEventIdx;
        auto ev = dComIfGp_getPEvtManager();
        if (LStair_endCheck(ev, idx)) {
            dComIfGp_event_reset();
        }
    } else if (mEventState==1) {
        fopAcM_orderOtherEventId(this, (s16)mEventIdx, 255, 65535, 0, 1);
        eventInfo.onCondition(dEvtCnd_UNK2_e);
    }
    s16 idx = mEventIdx;
    auto ev = dComIfGp_getPEvtManager();
    if (LStair_startCheck(ev, idx)) mEventState = 0;
}
/* 021B6500 */
void daLStair_c::set_off_se() {
    s32 rev = dComIfGp_getReverb(current.roomNo);
    mDoAud_seStart(JA_SE_OBJ_L_BRIDGE_OFF, &eyePos, 0, rev);
}
/* 021B6290 */
void daLStair_c::set_on_se() {
    auto* stage_info = reinterpret_cast<LStairStageInfo*>(dComIfGp_getStageStagInfo());
    if ((stage_info->mType >> 1) == 5) {
        s32 rev = dComIfGp_getReverb(current.roomNo);
        mDoAud_seStart(JA_SE_OBJ_R_STAIR_APPEAR, &eyePos, 0, rev);
    }
}
/* 021B6548 */
void daLStair_c::disappear_stair() {
        set_off_se();
    f32 a = mBpkAnm0.mFrameCtrl.mEnd, b = mBpkAnm1.mFrameCtrl.mEnd, c = mBtkAnm.mFrameCtrl.mEnd;
    mTimer = 40;
    mBpkAnm0.mFrameCtrl.mFrame = a;
    mBpkAnm0.mFrameCtrl.mRate = -1.0f;
    mBpkAnm1.mFrameCtrl.mFrame = b;
    mBpkAnm1.mFrameCtrl.mRate = -1.0f;
    mBtkAnm.mFrameCtrl.mFrame = c;
    mBtkAnm.mFrameCtrl.mRate = -1.0f;
    mBrkAnm.mFrameCtrl.mRate = -1.0f;
}
/* 021B62FC */
void daLStair_c::appear_stair() {
    gabi::Local<cXyz> center, left, right;
// Non-leaf matrix/play callees save LR at incoming SP+4.
    gabi::Local<be<u32>[4]> linkage;
    center->x = (f32)home.pos.x;
    center->y = (f32)home.pos.y;
    center->z = (f32)home.pos.z;
    left->x = -148.0f;
    left->y = 0.0f;
    left->z = 0.0f;
    right->x = 148.0f;
    right->y = 0.0f;
    right->z = 0.0f;
    mDoMtx_stack_c::transS((f32)current.pos.x, (f32)(home.pos.y-300.0f), (f32)current.pos.z);
    mDoMtx_stack_c::YrotM((s16)current.angle.y);
    PSMTXMultVec(mDoMtx_stack_c::get(), left.get(), left.get());
    PSMTXMultVec(mDoMtx_stack_c::get(), right.get(), right.get());
    center->y = (f32)center->y-300.0f;
    auto pa = dComIfGp_getParticle();
    LStair_particleSet(pa, 0, dPa_name::ID_IT_SN_RSTAIRS_F00, center.get(), &current.angle, 0, 255, 0, -1, 0, 0, 0);
    pa = dComIfGp_getParticle();
    LStair_particleSet(pa, 0, dPa_name::ID_IT_SN_RSTAIRS_S00, left.get(), &current.angle, 0, 255, 0, -1, 0, 0, 0);
    pa = dComIfGp_getParticle();
    LStair_particleSet(pa, 0, dPa_name::ID_IT_SN_RSTAIRS_S00, right.get(), &current.angle, 0, 255, 0, -1, 0, 0, 0);
    set_on_se();
    mBpkAnm1.mFrameCtrl.mFrame = 0.0f;
    mEventState = 1;
    mBtkAnm.mFrameCtrl.mFrame = 0.0f;
    mBpkAnm0.mFrameCtrl.mFrame = 0.0f;
    mBpkAnm0.mFrameCtrl.mRate = 1.0f;
    mBtkAnm.mFrameCtrl.mRate = 1.0f;
    mTimer = 40;
    mBpkAnm1.mFrameCtrl.mRate = 1.0f;
    mBrkAnm.mFrameCtrl.mRate = 1.0f;
}
/* 021B5A34 */
BOOL daLStair_c::CreateHeap() {
    auto modelData = (J3DModelData*)lstair_getObjectRes(m_arcname, dRes_INDEX_GKAI00_BDL_GKAI00_e, gc_tu.safeString);
    if (!modelData) JUT_ASSERT_fail(__FILE_NAME__, 216, "modelData != NULL");
    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000223);
    if (!mpModel) return 0;
    auto bck = (J3DAnmTransform*)lstair_getObjectRes(m_arcname, dRes_INDEX_GKAI00_BCK_GKAI00_e, gc_tu.safeString);
    if (!bck) JUT_ASSERT_fail(__FILE_NAME__, 233, "pbck != NULL");
    if (!mBckAnm.init(modelData, bck, true, 0, 1.0f, 0, -1, false)) return 0;
    auto btk = (J3DAnmTextureSRTKey*)lstair_getObjectRes(m_arcname, dRes_INDEX_GKAI00_BTK_GKAI00_e, gc_tu.safeString);
    if (!btk) JUT_ASSERT_fail(__FILE_NAME__, 244, "pbtk != NULL");
    if (!mBtkAnm.init(modelData, btk, true, 2, 1.0f, 0, -1, false, 0)) return 0;
    auto color0 = lstair_getObjectRes(m_arcname, dRes_INDEX_GKAI00_BPK_GKAI00_01_e, gc_tu.safeString);
    if (!color0) JUT_ASSERT_fail(__FILE_NAME__, 255, "pbpk != NULL");
    if (!LStair_colorInit(&mBpkAnm0, modelData, color0, 1, 2, 1.0f, 0, -1, 0, 0)) return 0;
    auto color1 = lstair_getObjectRes(m_arcname, dRes_INDEX_GKAI00_BPK_GKAI00_02_e, gc_tu.safeString);
    if (!color1) JUT_ASSERT_fail(__FILE_NAME__, 266, "pbpk != NULL");
    if (!LStair_colorInit(&mBpkAnm1, modelData, color1, 1, 2, 1.0f, 0, -1, 0, 0)) return 0;
    auto tev = lstair_getObjectRes(m_arcname, dRes_INDEX_GKAI00_BRK_GKAI00_e, gc_tu.safeString);
    if (!tev) JUT_ASSERT_fail(__FILE_NAME__, 278, "pbrk != NULL");
    if (!LStair_tevInit(&mBrkAnm, modelData, tev, 1, 2, 1.0f, 0, -1, 0, 0)) return 0;
    mpBgW = new_dBgW();
    if (!mpBgW) return 0;
    auto bgData = lstair_getObjectRes(m_arcname, dRes_INDEX_GKAI00_DZB_GKAI00_e, gc_tu.safeString);
    return LStair_bgSet(mpBgW.get(), bgData, 1, &mBgMtx)==0;
}
/* 021B5EC0 */
void daLStair_c::CreateInit() {
    fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
    fopAcM_setCullSizeBox(this, -200.0f, -300.0f, -100.0f, 200.0f, 100.0f, 700.0f);
    s32 no = fopAcM_GetParam(this)&0xFF;
    s32 room = home.roomNo;
    mSwitchNo = no;
    cullSizeFar = 1.5f;
    mSwitchStatus = dComIfGs_isSwitch(no, room)!=0;
    mEnemyGone = enemyGone(this);
    auto ev = dComIfGp_getPEvtManager();
    s32 idx = LStair_getEventIdx(ev, "STAIRAPPEAR", 255);
    mStairYOffset = -330.0f;
    mAppearTimer = -1;
    mEventIdx = idx;
    if ((mSwitchNo==255?mEnemyGone:mSwitchStatus)!=0) mStairYOffset = 0.0f;
    setMoveBGMtx();
    auto bg = dComIfG_Bgsp();
    bg->Regist(mpBgW, this);
    mpBgW->Move();
    set_mtx();
}
/* 021B6014 */
s32 daLStair_c::_create() {
    fopAcM_ct(this, daLStair_c);
    s32 phase = dComIfG_resLoad(&mPhase, m_arcname);
    if (phase==4) {
        if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, 0x2960)) return cPhs_ERROR_e;
        CreateInit();
    }
    return phase;
}
/* 021B61A8 */
BOOL daLStair_c::_draw() {
    auto light = dKy_getEnvlight();
    settingTevStruct(light, 1, &current.pos, &tevStr);
    light = dKy_getEnvlight();
    setLightTevColorType(light, mpModel.get(), &tevStr);
    auto data = mpModel->getModelData();
    LStair_btkEntry(&mBtkAnm, data, (f32)mBtkAnm.mFrameCtrl.mFrame);
    data = mpModel->getModelData();
    LStair_colorEntry(&mBpkAnm0, data, (f32)mBpkAnm0.mFrameCtrl.mFrame);
    data = mpModel->getModelData();
    LStair_colorEntry(&mBpkAnm1, data, (f32)mBpkAnm1.mFrameCtrl.mFrame);
    data = mpModel->getModelData();
    LStair_tevEntry(&mBrkAnm, data, (f32)mBrkAnm.mFrameCtrl.mFrame);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return 1;
}
/* 021B5D58 */
static BOOL CheckCreateHeap(fopAc_ac_c* a) {
    return static_cast<daLStair_c*>(a)->CreateHeap();
}
/* 021B6148 */
static s32 daLStair_Create(daLStair_c* a) {
    return a->_create();
}
/* 021B61A4 */
static bool daLStair_Delete(daLStair_c* a) {
    return a->_delete();
}
/* 021B69EC */
static bool daLStair_Execute(daLStair_c* a) {
    return a->_execute();
}
/* 021B6A98 */
static BOOL daLStair_IsDelete(daLStair_c* a) {
    return 1;
}
/* 021B6A84 */
static void LStairSafeString_destructor(void* a, s32 flags) {
    if (a && (flags&1)) operator_delete(a);
}
/* 021B6AA0 */
static void LStair_destructor(daLStair_c* a, s32 flags) {
    if (a) {
        LStair_objectDestruct(&a->mBckState.mVtable, 0);
        __dt(static_cast<fopAc_ac_c*>(a));
        if (flags&1) operator_delete(a);
    }
}
/* 021B6B00 */
static void LStairSafeString_terminate(void* a) {
}
/* 021B69F0 */
static void LStair_static_init() {
    sinit_header_statics(0x10464E50, 0x101B90E0);
}

/* WWHD glue (tools/verify/autoglue.py)
ext 026066C4 void* LStair_getResource(void*, SafeString*, s32)
ext 025D98E8 void* fopAcM_myRoomSearchEnemy(s32)
ext 025F2518 void LStair_scaleM(f32, f32, f32)
ext 0207A9A0 u8 LStair_calcTimer(be<u8>*)
ext 0200ECD4 f32 LStair_addCalc(be<f32>*, f32, f32, f32, f32)
ext 0254407C BOOL LStair_startCheck(dEvent_manager_c*, s16)
ext 025440C8 BOOL LStair_endCheck(dEvent_manager_c*, s16)
ext 02543F10 s32 LStair_getEventIdx(dEvent_manager_c*, const char*, u8)
ext 027F2BC0 void LStair_frameInit(J3DFrameCtrl*, s32)
ext 027DA984 void LStair_objectConstruct(void*)
ext 027F3628 void LStair_objectDestruct(void*, s32)
ext 025E7480 void LStair_colorConstruct(void*)
ext 025E80D0 void LStair_tevConstruct(void*)
ext 025E74FC BOOL LStair_colorInit(void*, J3DModelData*, void*, s32, s32, f32, s16, s16, s32, s32)
ext 025E8154 BOOL LStair_tevInit(void*, J3DModelData*, void*, s32, s32, f32, s16, s16, s32, s32)
ext 025E7FC4 void LStair_btkEntry(void*, J3DModelData*, f32)
ext 025E779C void LStair_colorEntry(void*, J3DModelData*, f32)
ext 025E83FC void LStair_tevEntry(void*, J3DModelData*, f32)
ext 025A847C JPABaseEmitter* LStair_particleSet(dPa_control_c*, s32, u16, cXyz*, csXyz*, s32, u8, s32, s8, s32, s32, s32)
ext 0200A030 BOOL LStair_bgSet(dBgW*, void*, u32, Mtx34*)
str 10014180 "Gkai00"
str 10014100 "d_a_lstair.cpp"
str 10014110 "modelData != NULL"
str 100140D0 "pbck != NULL"
str 100140DC "pbtk != NULL"
str 100140E8 "pbpk != NULL"
str 100140F4 "pbrk != NULL"
str 10014148 "STAIRAPPEAR"
*/
