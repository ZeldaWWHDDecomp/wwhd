// Soft soil and Makar tree.
#include "d/actor/d_a_obj_vmc.h"
using daObjVmc::Act_c;
const gc_tu_data gc_tu = {.safeString = 0x10032D48, .headerStatics = {0x1046C894, 0x101CDA48, 0x1046C8A0}};
enum { STATE_BASE_MAIN, STATE_TREE_DEMO_WAIT, STATE_TREE_DEMO_MAIN, STATE_TREE_MAIN };
enum {
    dRes_INDEX_VMC_BDL_VMCBS_e = 10,
    dRes_INDEX_VMC_BDL_VMCWD_e = 13,
    dRes_INDEX_VMC_BCK_VMCWG_e = 6,
    dRes_INDEX_VMC_BCK_VMCWH_e = 7,
    dRes_INDEX_VMC_DZB_VMCBS_e = 16,
    dRes_INDEX_VMC_DZB_VMCWD_e = 17
};
enum {
    JA_SE_OBJ_CB_SEED_GERMIN = 0x6A13,
    JA_SE_OBJ_CB_SEED_GROW = 0x6A14,
    fpcNm_HOOKSHOT_e = 0xA9,
    fpcNm_NPC_CB1_e = 0x14E
};
namespace dPa_name {
enum { ID_AK_ST_MACORETREESMOKE00 = 0xA1BC };
}
// HD: private constructor views expose the embedded material table and compiler vtables.
struct VmcMaterialTable {
    u8 _00[0x34];
    be<u32> __vtbl;
    u8 _38[0xC];
    be<u32> field_0x44;
    u8 _48[0x20];
};
struct VmcBckConstruction {
    J3DFrameCtrl mFrameCtrl;
    be<u32> __vtbl;
    VmcMaterialTable mMaterial;
    be<u32> field_0x7C, field_0x80, field_0x84, field_0x88;
};
WWHD_SIZE(VmcBckConstruction, 0x8C);
struct VmcVtable {
    be<u32> mEntry;
};
struct VmcJointAnimation {
    u8 _00[0x14];
    be<u32> mpMtxCalc;
};
struct VmcModelData {
    u8 _00[8];
    gptr<VmcJointAnimation> mJointNodes;
};
static VmcVtable l_actorVtable;
static VmcVtable l_aabVtable;
static VmcVtable l_bckVtable;
static VmcVtable l_bckBaseVtable;
static VmcVtable l_materialVtable;
static dCcD_SrcCyl cyl_src_base;
u32 Vmc_PrmAbstract(Act_c*, s32, s32);
s32 Vmc_bgSet(dBgW*, cBgD_t*, u32, Mtx34*);
void Vmc_materialCtor(VmcMaterialTable*);
void Vmc_bckTailDtor(be<u32>*, s32);
void Vmc_setEventName(dEvt_info_c*, const char*);
f64 Vmc_sqrt(f64);

static void constructBck(mDoExt_bckAnm* animation, bool grow) {
    auto* view = reinterpret_cast<VmcBckConstruction*>(animation);
    view->mFrameCtrl.init(0);
    view->__vtbl = gabi::ea(&l_bckBaseVtable);
    Vmc_materialCtor(&view->mMaterial);
    if (grow) {
        view->field_0x88 = 0;
        view->mMaterial.field_0x44 = 0;
        view->field_0x7C = 0;
        view->field_0x84 = 0;
        view->mMaterial.__vtbl = gabi::ea(&l_materialVtable);
        view->field_0x80 = 0;
        view->__vtbl = gabi::ea(&l_bckVtable);
    } else {
        view->__vtbl = gabi::ea(&l_bckVtable);
        view->field_0x80 = 0;
        view->field_0x88 = 0;
        view->field_0x84 = 0;
        view->field_0x7C = 0;
        view->mMaterial.__vtbl = gabi::ea(&l_materialVtable);
        view->mMaterial.field_0x44 = 0;
    }
}
/* 023B2574 */
// HD: tww Act_c::solidHeapCB is a free binding until autoglue supports static methods.
BOOL Vmc_solidHeapCB(fopAc_ac_c* actor) {
    return static_cast<Act_c*>(actor)->create_heap();
}
/* 023B2294 */
bool Act_c::create_heap() {
    auto* mdl_bs_data =
        static_cast<J3DModelData*>(dComIfG_getObjectRes("Vmc", dRes_INDEX_VMC_BDL_VMCBS_e, gc_tu.safeString));
    if (!mdl_bs_data)
        JUT_ASSERT_fail("d_a_obj_vmc.cpp", 200, "mdl_bs_data != 0");
    J3DModel* model = mDoExt_J3DModel__create(mdl_bs_data, 0, 0x11020203);
    mModelBase = model;
    if (!model)
        return false;
    auto* mdl_wd_data =
        static_cast<J3DModelData*>(dComIfG_getObjectRes("Vmc", dRes_INDEX_VMC_BDL_VMCWD_e, gc_tu.safeString));
    if (!mdl_wd_data)
        JUT_ASSERT_fail("d_a_obj_vmc.cpp", 207, "mdl_wd_data != 0");
    model = mDoExt_J3DModel__create(mdl_wd_data, 0, 0x11020203);
    mModelTree = model;
    if (!model)
        return false;
    auto* bck_wg =
        static_cast<J3DAnmTransform*>(dComIfG_getObjectRes("Vmc", dRes_INDEX_VMC_BCK_VMCWG_e, gc_tu.safeString));
    if (!bck_wg)
        JUT_ASSERT_fail("d_a_obj_vmc.cpp", 215, "bck_wg != 0");
    BOOL ok = mBckAnmGrow.init(mdl_wd_data, bck_wg, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false);
    mBckAnmGrow.setPlaySpeed(0.75f);
    if (!ok)
        return false;
    auto* bck_wh =
        static_cast<J3DAnmTransform*>(dComIfG_getObjectRes("Vmc", dRes_INDEX_VMC_BCK_VMCWH_e, gc_tu.safeString));
    if (!bck_wh)
        JUT_ASSERT_fail("d_a_obj_vmc.cpp", 226, "bck_wh != 0");
    ok = mBckAnmHookshot.init(mdl_wd_data, bck_wh, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false);
    if (!ok)
        return false;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    PSMTXCopy(mDoMtx_stack_c::get(), &mMtxBase);
    PSMTXCopy(mDoMtx_stack_c::get(), &mMtxTree);
    bool success = true;
    mpBgBase = new_dBgW();
    if (!mpBgBase)
        success = false;
    else {
        auto* mesh = static_cast<cBgD_t*>(dComIfG_getObjectRes("Vmc", dRes_INDEX_VMC_DZB_VMCBS_e, gc_tu.safeString));
        if (Vmc_bgSet(mpBgBase, mesh, dBgW::MOVE_BG_e, &mMtxBase))
            success = false;
    }
    mpBgTree = new_dBgW();
    if (!mpBgTree)
        return false;
    auto* mesh = static_cast<cBgD_t*>(dComIfG_getObjectRes("Vmc", dRes_INDEX_VMC_DZB_VMCWD_e, gc_tu.safeString));
    s32 result = Vmc_bgSet(mpBgTree, mesh, dBgW::MOVE_BG_e, &mMtxTree);
    return result == 0 && success;
}
/* 023B2704 */
void Act_c::CreateInit() {
    J3DModel* model = mModelBase;
    cullMtx = model ? gabi::ea(model->getBaseTRMtx()) : 0;
    fopAcM_setCullSizeBox(this, -300.0f, -0.0f, -300.0f, 300.0f, 500.0f, 300.0f);
    mStts.Init(255, 255, this);
    mCyl.Set(&cyl_src_base);
    mCyl.SetStts(&mStts);
    auto* bg = dComIfG_Bgsp();
    bg->Regist(mpBgBase, this);
    u32 sw = Vmc_PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
    s8 room = home.roomNo;
    if (dComIfGs_isSwitch(sw, room)) {
        bg = dComIfG_Bgsp();
        s32 result = bg->Regist(mpBgTree, this);
        if (!result)
            mHasTreeBg = 1;
        f32 end = mBckAnmGrow.getEndFrame();
        mHasTree = 1;
        mState = STATE_TREE_MAIN;
        mBckAnmGrow.setFrame(end);
    } else {
        mState = STATE_BASE_MAIN;
        mHasTree = 0;
    }
    init_mtx();
    f32 x = current.pos.x, y = f32(current.pos.y) + 40.0f, z = current.pos.z;
    attention_info.position.x = x;
    attention_info.position.y = y;
    attention_info.position.z = z;
    eyePos.x = x;
    eyePos.y = y;
    eyePos.z = z;
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] = 0xA7;
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] = 0xA7;
    mHookshotAnim = 0;
    mSmoke.m11 = 0;
    mLinkRangeCheck = 1;
    Vmc_setEventName(&eventInfo, "cb_sow");
}
/* 023B2928 */
cPhs_State Act_c::_create() {
    u32 flags = actor_condition;
    if (!(flags & fopAcCnd_INIT_e)) {
        if (this) {
            fopAc_ac_c_ct(this);
            __vtbl = gabi::ea(&l_actorVtable);
            constructBck(&mBckAnmGrow, true);
            constructBck(&mBckAnmHookshot, false);
            dCcD_Stts_ct(&mStts);
            dCcD_Cyl_ct(&mCyl, gabi::ea(&l_aabVtable));
            __ct(&mSmoke, 1);
            flags = actor_condition;
        }
        actor_condition = flags | fopAcCnd_INIT_e;
    }
    mHasTreeBg = 0;
    cPhs_State phase = dComIfG_resLoad(&mPhs, "Vmc");
    if (phase == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(this, Vmc_solidHeapCB, 0x32A0))
            CreateInit();
        else
            phase = cPhs_ERROR_e;
    }
    return phase;
}
/* 023B2ADC */
bool Act_c::_delete() {
    auto* bg = dComIfG_Bgsp();
    bg->Release(mpBgBase);
    if (mHasTreeBg) {
        bg = dComIfG_Bgsp();
        bg->Release(mpBgTree);
    }
    mSmoke.remove();
    dComIfG_resDelete(&mPhs, "Vmc");
    return true;
}
/* 023B2578 */
void Act_c::set_mtx() {
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    mDoMtx_stack_c::transS(x, y, z);
    s16 rx = current.angle.x, ry = current.angle.y, rz = current.angle.z;
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), rx, ry, rz);
    mModelBase->setBaseTRMtx(mDoMtx_stack_c::get());
    x = current.pos.x;
    y = current.pos.y;
    z = current.pos.z;
    mDoMtx_stack_c::transS(x, y, z);
    rx = shape_angle.x;
    ry = shape_angle.y;
    rz = shape_angle.z;
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), rx, ry, rz);
    mModelTree->setBaseTRMtx(mDoMtx_stack_c::get());
}
/* 023B26C8 */
void Act_c::init_mtx() {
    J3DModel* model = mModelBase;
    f32 x = scale.x, y = scale.y, z = scale.z;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    model = mModelTree;
    x = scale.x;
    y = scale.y;
    z = scale.z;
    // HD: retain the second scale reload and z/x/y store order when fields overlap.
    model->mBaseScale.z = z;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    set_mtx();
}
/* 023B2B58 */
void Act_c::daObjVmc_base_main() {
    if (mHasTree) {
        dComIfGp_particle_setToon(dPa_name::ID_AK_ST_MACORETREESMOKE00, &current.pos, &current.angle, nullptr, 255,
                                  &mSmoke, current.roomNo);
        u32 flags = attention_info.flags;
        mState = STATE_TREE_DEMO_WAIT;
        attention_info.flags = flags & ~fopAc_Attn_UNK10000000_e;
        s16 angle = fopAcM_searchActorAngleY(this, dComIfGp_getPlayer(0));
        mTimer = 0;
        shape_angle.y = angle + 0x1800;
    }
}
/* 023B2C18 */
void Act_c::daObjVmc_tree_demo_wait() {
    mState = STATE_TREE_DEMO_MAIN;
}
/* 023B2C24 */
void Act_c::daObjVmc_tree_demo_main() {
    u32 timer = u32(mTimer) + 1;
    mTimer = static_cast<s32>(timer);
    if (timer == 10 || timer == 40) {
        s32 reverb = dComIfGp_getReverb(current.roomNo);
        mDoAud_seStart(timer == 10 ? JA_SE_OBJ_CB_SEED_GERMIN : JA_SE_OBJ_CB_SEED_GROW, &current.pos, 0, reverb);
    }
    mBckAnmGrow.play();
    if (mBckAnmGrow.isStop()) {
        mState = STATE_TREE_MAIN;
        s32 result = dComIfG_Bgsp()->Regist(mpBgTree, this);
        u32 status = actor_status;
        if (!result)
            mHasTreeBg = 1;
        actor_status = status | fopAcStts_UNK200000_e;
        u32 sw = Vmc_PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
        s32 room = home.roomNo;
        dComIfGs_onSwitch(sw, room);
    }
}
/* 023B2D50 */
void Act_c::daObjVmc_tree_main() {
    if (!mHookshotAnim) {
        if (mCyl.ChkCoHit()) {
            auto* bck_wh = static_cast<J3DAnmTransform*>(
                dComIfG_getObjectRes("Vmc", dRes_INDEX_VMC_BCK_VMCWH_e, gc_tu.safeString));
            if (!bck_wh)
                JUT_ASSERT_fail("d_a_obj_vmc.cpp", 429, "bck_wh != 0");
            mBckAnmHookshot.init(mModelTree->getModelData(), bck_wh, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, true);
            mHookshotAnim = 1;
        }
    } else {
        if (mBckAnmHookshot.isStop())
            mHookshotAnim = 0;
        else
            mBckAnmHookshot.play();
    }
    if (mCyl.ChkTgHit()) {
        auto* hit = static_cast<dCcD_GObjInf*>(mCyl.GetTgHitObj());
        if (hit) {
            dCcD_Stts* stts = hit->mStts;
            if (!stts)
                return;
            fopAc_ac_c* actor = stts->mp_actor;
            if (actor && actor->mProcName == fpcNm_HOOKSHOT_e)
                dComIfGs_onEventBit(0x3420);
        }
    }
}
/* 023B2EF4 */
bool Act_c::_execute() {
    mCyl.SetC(&current.pos);
    s32 state = mState;
    if (state == STATE_BASE_MAIN) {
        cCcS_Set(dComIfG_Ccsp(), &mCyl);
        state = mState;
    }
    switch (state) {
    case STATE_BASE_MAIN:
        daObjVmc_base_main();
        break;
    case STATE_TREE_DEMO_WAIT:
        daObjVmc_tree_demo_wait();
        break;
    case STATE_TREE_DEMO_MAIN:
        daObjVmc_tree_demo_main();
        break;
    case STATE_TREE_MAIN:
        daObjVmc_tree_main();
        break;
    }
    set_mtx();
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    gabi::Local<cXyz> dist;
    cXyz_mi(&player->current.pos, dist.get(), &current.pos);
    bool range = mLinkRangeCheck != 0;
    f32 x = dist->x, z = dist->z;
    gabi::Local<cXyz> xz;
    xz->x = x;
    xz->y = 0.0f;
    xz->z = z;
    f32 mag = PSVECSquareMag(xz.get());
    // HD: preserve the double-precision square-root call and range thresholds.
    f64 distance = Vmc_sqrt(static_cast<f64>(mag));
    if (!range) {
        bool away = distance > 110.0;
        if (!away) {
            f32 y = dist->y;
            away = y < -500.0f || y > 500.0f;
        }
        if (away) {
            mLinkRangeCheck = 1;
            range = true;
        } else
            range = mLinkRangeCheck != 0;
    } else {
        if (distance < 100.0) {
            f32 y = dist->y;
            if (y > -490.0f && y < 490.0f) {
                mLinkRangeCheck = 0;
                range = false;
            } else
                range = mLinkRangeCheck != 0;
        } else
            range = mLinkRangeCheck != 0;
    }
    constexpr u32 attentionMask = fopAc_Attn_UNK10000000_e | fopAc_Attn_ACTION_SPEAK_e | fopAc_Attn_LOCKON_TALK_e;
    if (!range) {
        attention_info.flags = u32(attention_info.flags) & ~attentionMask;
        return true;
    }
    fopAc_ac_c* cb = dComIfGp_getCb1Player();
    if (cb) {
        cb = dComIfGp_getCb1Player();
        if (cb && cb->mProcName == fpcNm_NPC_CB1_e)
            eventInfo.mCondition = u16(eventInfo.mCondition) | dEvtCnd_CANTALK_e;
    }
    if (mLinkRangeCheck && !mHasTree) {
        fopAc_ac_c* controlled = dComIfGp_getPlayer(0);
        if (controlled && controlled->mProcName == fpcNm_NPC_CB1_e) {
            attention_info.flags = u32(attention_info.flags) | attentionMask;
            return true;
        }
    }
    attention_info.flags = u32(attention_info.flags) & ~attentionMask;
    return true;
}
/* 023B320C */
bool Act_c::_draw() {
    auto* env = dKy_getEnvlight();
    settingTevStruct(env, 1, &current.pos, &tevStr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, mModelBase, &tevStr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, mModelTree, &tevStr);
    s32 state = mState;
    J3DModel* model = mModelTree;
    J3DModelData* data = model->getModelData();
    mDoExt_bckAnm* animation = state < STATE_TREE_MAIN ? &mBckAnmGrow : &mBckAnmHookshot;
    f32 frame = animation->getFrame();
    animation->entry(data, frame);
    mDoExt_modelUpdateDL(mModelTree);
    state = mState;
    model = mModelTree;
    data = model->getModelData();
    // HD: both animation remove paths clear the first joint's matrix calculator directly.
    reinterpret_cast<VmcModelData*>(data)->mJointNodes->mpMtxCalc = 0;
    mDoExt_modelUpdateDL(mModelBase);
    return true;
}
/* 023B3308 */
s32 Vmc_Mthd_Create(Act_c* actor) {
    return actor->_create();
}
/* 023B330C */
bool Vmc_Mthd_Delete(Act_c* actor) {
    return actor->_delete();
}
/* 023B3310 */
bool Vmc_Mthd_Execute(Act_c* actor) {
    return actor->_execute();
}
/* 023B3314 */
bool Vmc_Mthd_Draw(Act_c* actor) {
    return actor->_draw();
}
/* 023B3448 */
s32 isDelete(Act_c*) {
    return 1;
}

/* 023B3318 */
void staticInit() {
    __sinit_header_statics();
}
/* 023B33AC */
void staticDtor(void* actor, s32 flags) {
    if (actor && (flags & 1))
        operator_delete(actor);
}
/* 023B33C0 */
void empty(void*) {}
/* 023B33C4 */
void actorDtor(Act_c* actor, s32 flags) {
    if (actor) {
        __dt(&actor->mCyl);
        __dt(&actor->mStts);
        auto* hook = reinterpret_cast<VmcBckConstruction*>(&actor->mBckAnmHookshot);
        Vmc_bckTailDtor(&hook->__vtbl, 0);
        auto* grow = reinterpret_cast<VmcBckConstruction*>(&actor->mBckAnmGrow);
        Vmc_bckTailDtor(&grow->__vtbl, 0);
        __dt(static_cast<fopAc_ac_c*>(actor));
        if (flags & 1)
            operator_delete(actor);
    }
}
/* 023B3450 */
u32 Vmc_PrmAbstract(Act_c* actor, s32 width, s32 shift) {

    u32 w = (u32)width & 63, s = (u32)shift & 63;
    return (s < 32 ? u32(actor->mParameters) >> s : 0) & ((w < 32 ? 1u << w : 0) - 1);
}
/* WWHD glue
data l_actorVtable 10032D98
data l_aabVtable 10032D60
data l_bckVtable 10032D70
data l_bckBaseVtable 1016E54C
data l_materialVtable 1016D820
data cyl_src_base 101CDA04
str 10032E60
str 10032DAC in 023B2294
str 10032E28 in 023B2D50
str 10032DD4
str 10032DE8
str 10032DBC
str 10032DC8 in 023B2294
str 10032E38 in 023B2D50
str 10032E1C
ext 0200A030 s32 Vmc_bgSet(dBgW*,cBgD_t*,u32,Mtx34*)
ext 027DA984 void Vmc_materialCtor(VmcMaterialTable*)
ext 027F3628 void Vmc_bckTailDtor(be<u32>*,s32)
ext 0253E9B0 void Vmc_setEventName(dEvt_info_c*,const char*)
ext 028F4384 f64 Vmc_sqrt(f64)
*/
