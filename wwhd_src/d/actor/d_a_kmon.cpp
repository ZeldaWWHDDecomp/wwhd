/**
 * d_a_kmon.cpp (WWHD)
 * Kmon - the bell (Orca's "Ji1_KmonTalk" talk target on Outset).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_kmon.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_kmon.h"
// HD: GHS per-unit constructor/destructor vtables and common header objects.
const gc_tu_data gc_tu = {
    .safeString = 0x100130F4,
    .objAcch = {0x10013134, 0x10013154, 0x10013144},
    .objAcchDt = {0x10013144, 0x10013154},
    .headerStatics = {0x10464BE8, 0x101B8540, 0x10464BF4},
};
enum { KMON_VTBL = 0x10013164 };
enum {
    dRes_INDEX_ALWAYS_BCK_VBELL_e = 0x0F,
    dRes_INDEX_ALWAYS_BDL_VBELL_e = 0x25,
    dRes_INDEX_ALWAYS_BTK_VBELL_e = 0x59
};

/* 021A174C */
void daKmon_c::set_mtx() {
    mpModel->setBaseScale(scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    mDoMtx_stack_c::transM(0.0f, -24.0f, 0.0f);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
}

/* 021A1748 */
static BOOL CheckCreateHeap(fopAc_ac_c *i_this) {
    return ((daKmon_c *)i_this)->CreateHeap();
}

/* 021A1590 */
BOOL daKmon_c::CreateHeap() {
    J3DModelData *modelData = (J3DModelData *)dComIfG_getObjectRes("Always", dRes_INDEX_ALWAYS_BDL_VBELL_e);
    if (modelData == nullptr) /* JUT_ASSERT(166, modelData != NULL) */
        JUT_ASSERT_fail("d_a_kmon.cpp", 0xA6, "modelData != (0)");
    mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (mpModel == nullptr) {
        return FALSE;
    }
    J3DAnmTextureSRTKey *pbtk =
        (J3DAnmTextureSRTKey *)dComIfG_getObjectRes("Always", dRes_INDEX_ALWAYS_BTK_VBELL_e);
    if (pbtk == nullptr) /* JUT_ASSERT(176, pbtk != NULL) */
        JUT_ASSERT_fail("d_a_kmon.cpp", 0xB0, "pbtk != (0)");
    if (!mBtkAnm.init(modelData, pbtk, TRUE, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }
    mBtkAnm.mFrameCtrl.setRate(1.0f);
    J3DAnmTransform *pbck = (J3DAnmTransform *)dComIfG_getObjectRes("Always", dRes_INDEX_ALWAYS_BCK_VBELL_e);
    if (pbck == nullptr) /* JUT_ASSERT(188, pbck != NULL) */
        JUT_ASSERT_fail("d_a_kmon.cpp", 0xBC, "pbck != (0)");
    if (!mBckAnm.init(modelData, pbck, TRUE, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false)) {
        return FALSE;
    }
    mBckAnm.mFrameCtrl.setRate(0.0f);
    return TRUE;
}

/* 021A1844 */
cPhs_State daKmon_c::CreateInit() {
    set_mtx();
    fopAcM_SetMtx(this, mpModel->getBaseTRMtx()); /* fopAcM_SetMtx */
    mAcchCir.SetWall(30.0f, 30.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed);
    gravity = -4.0f;
    attention_info.distances[fopAc_Attn_TYPE_TALK_e] =
        0x6E; /* attention_info.distances[fopAc_Attn_TYPE_TALK_e] */
    attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] =
        0x6C; /* attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] */
    /* attention_info.flags |= TALKFLAG_LOOK | ACTION_SPEAK | LOCKON_TALK */
    attention_info.flags |= fopAc_Attn_TALKFLAG_LOOK_e | fopAc_Attn_ACTION_SPEAK_e | fopAc_Attn_LOCKON_TALK_e;
    Kmon_setEventName(&eventInfo, "Ji1_KmonTalk" /* "Ji1_KmonTalk" */);
    mpJi1 = (KmonOrcaHD *)fopAcM_SearchByID(parentActorID);
    return cPhs_COMPLEATE_e;
}

/* 021A1A9C */
void daKmon_c::checkTalk() {
    fopAc_ac_c *player = dComIfGp_getPlayer(0);
    gabi::Local<cXyz> player_pos;
    player_pos->x = player->current.pos.x;
    player_pos->y = player->current.pos.y;
    player_pos->z = player->current.pos.z;
    gabi::Local<cXyz> vec_from_player;
    cXyz_mi(&current.pos, vec_from_player, player_pos);
    /* absXZ: |(x, 0, z)| */
    gabi::Local<cXyz> xz;
    xz->x = vec_from_player->x;
    xz->y = 0.0f;
    xz->z = vec_from_player->z;
    f32 distance_to_player = std_sqrtf(PSVECSquareMag(xz));

    eyePos = attention_info.position = current.pos;
    attention_info.position.y = attention_info.position.y + 25.0f;

    if (eventInfo.checkCommandTalk()) {
        KmonOrcaHD *ji1 = mpJi1;
        if (!ptmf_eq(ji1->mAction, ACT_eventAction) /* !mpJi1->checkAction(&daNpc_Ji1_c::eventAction) */) {
            dComIfGp_event_setTalkPartner(ji1);
            mpJi1->field_0xC84 = 0x10;
            ji1 = mpJi1;
            ptmf_copy(ji1->field_0x2C8, ji1->mAction); /* field_0x2C8 = mAction */
            ji1_setAction(mpJi1, ACT_eventAction, nullptr);
            return;
        }
    }

    if (distance_to_player < 300.0f && current.pos.y > 200.0f && !dComIfGs_isEventBit(0xD80) &&
        dComIfGp_getMiniGameType() == 0) {
        /* eventInfo.onCondition(dEvtCnd_CANTALK_e) */
        eventInfo.onCondition(dEvtCnd_CANTALK_e);
    }
}

cPhs_State daKmon_c::_create() {
    /* fopAcM_ct(this, daKmon_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = KMON_VTBL;
            mDoExt_btkAnm::ct(&mBtkAnm);
            KmonBck_construct(&mBckAnm);
            __ct(&mAcch);
            __ct(&mAcchCir);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&mPhase, "Always");
    if (state == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(this, CheckCreateHeap, 0x10000)) {
            state = CreateInit();
        } else {
            state = cPhs_ERROR_e;
        }
    }
    return state;
}

/* 021A1938 */
static cPhs_State daKmonCreate(void *i_this) {
    return ((daKmon_c *)i_this)->_create();
}

BOOL daKmon_c::_delete() {
    dComIfG_resDelete(&mPhase, "Always");
    return TRUE;
}

/* 021A1A6C */
static BOOL daKmonDelete(void *i_this) {
    return ((daKmon_c *)i_this)->_delete();
}

BOOL daKmon_c::_execute() {
    checkTalk();
    fopAcM_posMoveF(this, nullptr);
    mAcch.CrrPos(dComIfG_Bgsp());
    mBckAnm.play();
    mBtkAnm.play();
    set_mtx();
    return FALSE;
}

/* 021A1D74 */
static BOOL daKmonExecute(void *i_this) {
    return ((daKmon_c *)i_this)->_execute();
}

BOOL daKmon_c::_draw() {
    g_env_light.settingTevStruct(0x5C /* TEV_TYPE_BG1_PLIGHT */, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    mBckAnm.entry(mpModel->getModelData(), mBckAnm.getFrame());
    mBtkAnm.entry(mpModel->getModelData(), mBtkAnm.getFrame());
    mDoExt_modelUpdateDL(mpModel);
    return TRUE;
}

/* 021A1DD8 */
static BOOL daKmonDraw(void *i_this) {
    return ((daKmon_c *)i_this)->_draw();
}

/* 021A1F04 */
static BOOL daKmonIsDelete(void *i_this) {
    return TRUE;
}

/* 021A1E5C */
static void __sinit_d_a_kmon_cpp() {
    __sinit_header_statics();
}

/* 021A1EF0 */
static void trivial_dt(void *p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 021A1F0C */
static void daKmon_c_dt(daKmon_c *i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(&i_this->mAcchCir);
        __dt(&i_this->mAcch);
        KmonBck_destroy(&i_this->mBckAnm);
        __dt(i_this);
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 021A1F9C */
static void empty_virtual(void *p) {}

/* WWHD glue
str 100131E8 "Always"
str 10013194 "d_a_kmon.cpp"
str 100131A4 "modelData != (0)"
str 1001317C "pbtk != (0)"
str 10013188 "pbck != (0)"
str 100131C4 "Ji1_KmonTalk"
ext 0253E9B0 void Kmon_setEventName(dEvt_info_c*, const char*)
ext 027F2BC0 void KmonFrameCtrl_init(J3DFrameCtrl*, s16)
ext 027DA984 void KmonTransform_construct(void*)
ext 027F3628 void KmonMtxCalc_destroy(void*, s32)
*/
