/* Dragon Roost clouds. */
#include "d/actor/d_a_obj_rcloud.h"

const gc_tu_data gc_tu = {.safeString = 0x1002EC5C};
static const char* l_arcname = "BVkumo";
static const char* l_demo_name[] = { "demo41", "demo41" };
static daObjRcloud_c::RcloudProcEntry act_proc[] = {
    {&daObjRcloud_c::wait_act_proc},
    {&daObjRcloud_c::clouds_lift_start_wait_act_proc},
    {&daObjRcloud_c::clouds_lift_act_proc}
};
static u32 l_demoFrame = 0;
// HD: the HIO constant is copied into zero-initialized static storage by __sinit.
static RcloudStatics l_statics;
enum { dRes_INDEX_BVKUMO_BDL_BVKUMO_e = 4, dRes_INDEX_BVKUMO_BTK_BVKUMO_e = 7, dSymbol_NAYRU_e = 0 };
namespace dSv_event_flag_c { enum { UNK_3908 = 0x3908 }; }

// HD: inline base construction installs this TU's virtual destructor table.
daObjRcloud_c::daObjRcloud_c() {
    fopAc_ac_c_ct(this);
    __vtbl = 0x1002EC74;
    mDoExt_btkAnm::ct(&mBtkAnm);
}

/* 0238735C */
void daObjRcloud_c::init_mtx() {
    mpModel->setBaseScale(scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
}

/* 02387358 */
static BOOL solidHeapCB(daObjRcloud_c* i_this) {
    return i_this->create_heap();
}

/* 0238724C */
BOOL daObjRcloud_c::create_heap() {
    J3DModelData* modelData = static_cast<J3DModelData*>(dComIfG_getObjectRes(l_arcname, dRes_INDEX_BVKUMO_BDL_BVKUMO_e));
    J3DAnmTextureSRTKey* pAnm = static_cast<J3DAnmTextureSRTKey*>(dComIfG_getObjectRes(l_arcname, dRes_INDEX_BVKUMO_BTK_BVKUMO_e));
    if (modelData == nullptr || pAnm == nullptr) {
        JUT_ASSERT(0xDE, FALSE);
        return FALSE;
    }
    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000222);
    s32 btkRet = mBtkAnm.init(modelData, pAnm, TRUE, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, FALSE, 0);
    return mpModel != nullptr && btkRet != 0;
}

/* 02387448 */
s32 daObjRcloud_c::_create() {
    fopAcM_ct(this, daObjRcloud_c);
    s32 phase = cPhs_ERROR_e;
    if (mInitState == 0)
        mDemoNameIndex = param_get_arg();
    u32 name = mDemoNameIndex;
    if (name > 1)
        return phase;
    BOOL done = name ? Rcloud_isSymbol(&dComIfGs_info()->mSavedata.mPlayer.mCollect, dSymbol_NAYRU_e)
                     : dComIfGs_isEventBit(dSv_event_flag_c::UNK_3908);
    if (!done) {
        phase = dComIfG_resLoad(&mPhase, l_arcname);
        mResourceLoadedFlag = 1;
        if (phase == cPhs_COMPLEATE_e) {
            if (fopAcM_entrySolidHeap(this, gabi::guest_fn((const void*)solidHeapCB), 0x5A0)) {
                fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
                init_mtx();
                setup_action(0);
            } else {
                phase = cPhs_ERROR_e;
            }
        }
    }
    return phase;
}

/* 023875C0 */
BOOL daObjRcloud_c::_delete() {
    // HD: any nonzero loaded flag is accepted (GameCube compares with one).
    if (mResourceLoadedFlag)
        dComIfG_resDelete(&mPhase, l_arcname);
    return TRUE;
}

/* 023877E4 */
void daObjRcloud_c::wait_act_proc() {
    if (dComIfGp_event_runCheck()) {
        // HD: capture the name before the second play-object accessor call.
        const char* name = l_demo_name[mDemoNameIndex];
        if (Rcloud_startCheck(dComIfGp_getPEvtManager(), name))
            setup_action(1);
    }
}

/* 0238785C */
void daObjRcloud_c::clouds_lift_start_wait_act_proc() {
    // HD: reads the frame directly and compares its float conversion with 428.
    f32 frame = (f32)l_demoFrame;
    if (frame > 428.0f)
        setup_action(2);
}

/* 023878B8 */
void daObjRcloud_c::clouds_lift_act_proc() {
    f32 progress = (f32)mCloudAnimProgress + (f32)l_statics.mRate;
    if (progress < -1.0f) {
        mCloudAnimProgress = -1.0f;
        dComIfGs_onEventBit(dSv_event_flag_c::UNK_3908);
        fopAcM_delete(this);
    } else {
        mCloudAnimProgress = progress;
    }
}

/* 02387428 */
void daObjRcloud_c::setup_action(s32 action) {
    // HD: two-word member pointer; preserve the action store between the words.
    mCurrentActProc.d = act_proc[action].proc.d;
    mCurrentActProc.i = act_proc[action].proc.i;
    u32 target = act_proc[action].proc.f;
    m2BC = action;
    mCurrentActProc.f = target;
}

/* 02387600 */
BOOL daObjRcloud_c::_execute() {
    mBtkAnm.play();
    (this->*mCurrentActProc)();
    return TRUE;
}

/* 0238767C */
void daObjRcloud_c::setTexMtx() {
    J3DModelData* modelData = mpModel->getModelData();
    for (u16 i = 0; i < Rcloud_getMaterialTable(modelData)->mMaterialNum; i++) {
        RcloudMaterial* material = reinterpret_cast<RcloudMaterial*>(modelData->getMaterialNodePointer(i));
        u16 materialIndex = material->mpData->mMaterialIndex;
        RcloudModelLayout* model = reinterpret_cast<RcloudModelLayout*>((J3DModel*)mpModel);
        RcloudMatPacket* packet = (RcloudMatPacket*)model->mMatPackets + materialIndex;
        for (u32 texMtxID = 0; texMtxID < 8; texMtxID++) {
            RcloudTexMtx* pTexMtx = packet->getTexMtx(texMtxID);
            if (pTexMtx != nullptr) {
                // HD: packet texture translation is doubled directly; no GameCube matrix access.
                f32 x = pTexMtx->mTranslationX;
                pTexMtx->mTranslationX = x + x;
                if (m2BC == 2) {
                    f32 y = pTexMtx->mTranslationY;
                    pTexMtx->mTranslationY = y + (f32)mCloudAnimProgress;
                }
            }
        }
    }
}

/* 02387764 */
BOOL daObjRcloud_c::_draw() {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    // HD: model entry variant followed by the combined model update.
    mBtkAnm.entry(mpModel, mBtkAnm.getFrame());
    setTexMtx();
    mDoExt_modelUpdateDL(mpModel);
    return TRUE;
}

/* 023875BC */
static s32 daObjRcloud_Create(daObjRcloud_c* i_this) { return i_this->_create(); }
/* 023875FC */
static BOOL daObjRcloud_Delete(daObjRcloud_c* i_this) { return i_this->_delete(); }
/* 02387678 */
static BOOL daObjRcloud_Execute(daObjRcloud_c* i_this) { return i_this->_execute(); }
/* 023877D8 */
static BOOL daObjRcloud_Draw(daObjRcloud_c* i_this) { return i_this->_draw(); }
/* 023877DC */
static BOOL daObjRcloud_IsDelete(daObjRcloud_c*) { return TRUE; }

// HD: compiler-generated translation-unit initialization.
/* 02387938 */
static void __sinit_d_a_obj_rcloud_cpp() {
    for (u32 i = 0; i < 4; i++)
        l_statics.mZero[i] = 0;
    __register_global_object(0x101CC7A4);
    l_statics.mMinAngle = -3.1415927410125732f;
    l_statics.mMaxAngle = 3.1415927410125732f;
    Rcloud_initSDKObjectA(&l_statics.mSDKObjectA);
    __register_global_object(0x101CC7B0);
    Rcloud_initSDKObjectB(&l_statics.mSDKObjectB);
    __register_global_object(0x101CC7BC);
    l_statics.mUnused = 112.0f;
    l_statics.mRate = -0.008928571827709675f;
}
/* 023879EC */
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
/* 02387A00 */
static void daObjRcloud_c_dt(daObjRcloud_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}
/* 02387A54 */
static void empty_virtual() {}

// HD: per-TU daObj parameter helper preserves PowerPC shift counts.
/* 02387A58 */
static u32 PrmAbstract(daObjRcloud_c* actor, u32 width, u32 shift) {
    u32 p = fopAcM_GetParam(actor);
    u32 bit = (width & 32) ? 0 : 1u << (width & 31);
    u32 shifted = shift & 32 ? 0 : p >> (shift & 31);
    return shifted & (bit - 1);
}
int daObjRcloud_c::param_get_arg() { return PrmAbstract(this, PRM_ARG_W, PRM_ARG_S); }

/* WWHD glue (tools/verify/autoglue.py)
str 1002EC44
str 1002EC4C
str 1002EC8C
str 1002EC88 "FALSE" renamed
data l_demo_name 101CC7C8
data act_proc 101CC78C
data l_demoFrame 101D6008
data l_statics 1046BD1C nocheck # HD BSS has no image bytes; __sinit writes its initial values.
ext 028ED6F8 void Rcloud_initSDKObjectA(u8*)
ext 028EAB2C void Rcloud_initSDKObjectB(u8*)
ext 027F3F8C RcloudMaterialTable* Rcloud_getMaterialTable(J3DModelData*)
ext 027FA7F8 RcloudTexMtx* RcloudMatPacket::getTexMtx(u32)
ext 025B7D90 BOOL Rcloud_isSymbol(dSv_player_collect_c*, u8)
ext 025445B8 BOOL Rcloud_startCheck(dEvent_manager_c*, const char*)
ext 025E8048 void RcloudBtkAnm::entry(J3DModel*, f32)
*/
