/**
 * d_a_kmon.cpp (WWHD)
 * Kmon - the bell (Orca's "Ji1_KmonTalk" talk target on Outset).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_kmon.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define m_arcname STR(0x100131E8) /* "Always" (static const char[]: one address) */
#define SAFESTRING_VTBL 0x100130F4
#define KMON_VTBL 0x10013164
#define KMON_BCK_VTBL 0x1001310C
static const dBgS_ObjAcch_vt OBJACCH_VT = {0x10013134, 0x10013154, 0x10013144};

enum {
    dRes_INDEX_ALWAYS_BCK_VBELL_e = 0x0F,
    dRes_INDEX_ALWAYS_BDL_VBELL_e = 0x25,
    dRes_INDEX_ALWAYS_BTK_VBELL_e = 0x59,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* dComIfGp_event_setTalkPartner: dEvt_control_c (play + 0x51D0) mPtTalk (+0xCC) = getPId(a) (0253F124) */
static inline void dComIfGp_event_setTalkPartner(fopAc_ac_c* a) {
    u32 evt = dComIfGp_ea() + 0x51D0;
    gabi::store<u32>(evt + 0xCC, gabi::call<u32>(0x0253F124, evt, a));
}
/* dEvt_info_c::setEventName (0253E9B0) on eventInfo (+0xF8) */
static inline void eventInfo_setEventName(fopAc_ac_c* a, const char* name) { gabi::call(0x0253E9B0, gabi::ea(a) + 0xF8, name); }
static inline BOOL dComIfGs_isEventBit(u16 f) {
    return dSv_event_isEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), f);
}
/* play+0x5CEA: the mini game type (u8) */
static inline u8 dComIfGp_getMiniGameType() { return gabi::load<u8>(dComIfGp_ea() + 0x5CEA); }

/* daNpc_Ji1_c (HD): only the fields this unit touches (see d_a_npc_ji1.h) */
struct ProcFunc_l {
    /* 0x0 */ be<s16> d;
    /* 0x2 */ be<s16> i;
    /* 0x4 */ be<u32> f;
};
struct daNpc_Ji1_l : fopAc_ac_c {
    /* 0x3AC */ u8 _3AC[0x3CC - 0x3AC];
    /* 0x3CC */ ProcFunc_l mAction;
    /* 0x3D4 */ ProcFunc_l field_0x2BC;
    /* 0x3DC */ ProcFunc_l field_0x2C8;
    /* 0x3E4 */ u8 _3E4[0xEA0 - 0x3E4];
    /* 0xEA0 */ be<s8> field_0xC78;
    /* 0xEA1 */ u8 _EA1[0xEAC - 0xEA1];
    /* 0xEAC */ be<u32> field_0xC84;
};
WWHD_OFFSET(daNpc_Ji1_l, field_0xC84, 0xEAC);
#define ACT_eventAction 0x022596CC /* daNpc_Ji1_c::eventAction */

/* pointers to member functions (GHS, 8 bytes): as d_a_npc_ji1.h */
static inline bool ptmf_nonnull(ProcFunc_l& p) { return p.i != 0; }
static inline bool ptmf_eq(ProcFunc_l& p, u32 fn) { return p.i == -1 && p.d == 0 && p.f == fn; }
static inline void ptmf_set(ProcFunc_l& p, u32 fn) {
    p.d = 0;
    p.i = -1;
    p.f = fn;
}
static inline void ptmf_copy(ProcFunc_l& dst, ProcFunc_l& src) {
    dst.d = (s16)src.d;
    dst.i = (s16)src.i;
    dst.f = (u32)src.f;
}
template <class... A> static inline BOOL ptmf_invoke(ProcFunc_l& p, void* self, A... a) {
    s16 d = p.d;
    s16 i = p.i;
    void* obj = gabi::at<void>(gabi::ea(self) + d);
    if (i < 0) return gabi::call_ptr<BOOL>(p.f, obj, a...);
    u32 vt = gabi::load<u32>(gabi::ea(obj) + gabi::load<s16>(gabi::ea(&p) + 6));
    return gabi::call_ptr<BOOL>(gabi::load<u32>(vt + i * 8 + 4), obj, a...);
}
/* daNpc_Ji1_c::setAction (inline in the GameCube header) */
static inline void ji1_setAction(daNpc_Ji1_l* i_this, u32 action, void* arg) {
    if (!ptmf_eq(i_this->mAction, action)) {
        if (ptmf_nonnull(i_this->mAction)) {
            i_this->field_0xC78 = -1;
            ptmf_invoke(i_this->mAction, i_this, arg);
        }
        ptmf_copy(i_this->field_0x2BC, i_this->mAction);
        ptmf_set(i_this->mAction, action);
        i_this->field_0xC78 = 0;
        gabi::call_ptr<BOOL>(i_this->mAction.f, i_this, arg);
    }
}

struct daKmon_c : fopAc_ac_c {
    void set_mtx();
    BOOL CreateHeap();
    cPhs_State CreateInit();
    void checkTalk();

    cPhs_State _create();
    BOOL _delete();
    BOOL _draw();
    BOOL _execute();

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ mDoExt_btkAnm mBtkAnm;
    /* 0x42C */ mDoExt_bckAnm mBckAnm;
    /* 0x4B8 */ dBgS_ObjAcch mAcch;
    /* 0x67C */ dBgS_AcchCir mAcchCir;
    /* 0x6BC */ gptr<daNpc_Ji1_l> mpJi1;
};
WWHD_OFFSET(daKmon_c, mBckAnm, 0x42C);
WWHD_OFFSET(daKmon_c, mAcch, 0x4B8);
WWHD_OFFSET(daKmon_c, mAcchCir, 0x67C);
WWHD_OFFSET(daKmon_c, mpJi1, 0x6BC);
WWHD_SIZE(daKmon_c, 0x6C0);

/* 021A174C */
void daKmon_c::set_mtx() {
    WWHD_FUNC(0x021A174C, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    mDoMtx_stack_c::transM(0.0f, -24.0f, 0.0f);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
VERIFY(0x021A174C, &daKmon_c::set_mtx);

/* 021A1748 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x021A1748, BOOL, i_this);
    return ((daKmon_c*)i_this)->CreateHeap();
}
VERIFY(0x021A1748, CheckCreateHeap);

/* 021A1590 */
BOOL daKmon_c::CreateHeap() {
    WWHD_FUNC(0x021A1590, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(m_arcname, dRes_INDEX_ALWAYS_BDL_VBELL_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(166, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x10013194), 0xA6, STR(0x100131A4));
    mpModel = mDoExt_J3DModel__create(modelData, 0, 0x11020203);
    if (mpModel == nullptr) {
        return FALSE;
    }
    J3DAnmTextureSRTKey* pbtk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(m_arcname, dRes_INDEX_ALWAYS_BTK_VBELL_e, SAFESTRING_VTBL);
    if (pbtk == nullptr) /* JUT_ASSERT(176, pbtk != NULL) */
        JUT_ASSERT_fail(STR(0x10013194), 0xB0, STR(0x1001317C));
    if (!mBtkAnm.init(modelData, pbtk, TRUE, 2 /* J3DFrameCtrl::EMode_LOOP */, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }
    mBtkAnm.mFrameCtrl.setRate(1.0f);
    J3DAnmTransform* pbck = (J3DAnmTransform*)dComIfG_getObjectRes(m_arcname, dRes_INDEX_ALWAYS_BCK_VBELL_e, SAFESTRING_VTBL);
    if (pbck == nullptr) /* JUT_ASSERT(188, pbck != NULL) */
        JUT_ASSERT_fail(STR(0x10013194), 0xBC, STR(0x10013188));
    if (!mBckAnm.init(modelData, pbck, TRUE, 2 /* J3DFrameCtrl::EMode_LOOP */, 1.0f, 0, -1, false)) {
        return FALSE;
    }
    mBckAnm.mFrameCtrl.setRate(0.0f);
    return TRUE;
}
VERIFY(0x021A1590, &daKmon_c::CreateHeap);

/* 021A1844 */
cPhs_State daKmon_c::CreateInit() {
    WWHD_FUNC(0x021A1844, cPhs_State, this);
    set_mtx();
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    mAcchCir.SetWall(30.0f, 30.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed);
    gravity = -4.0f;
    gabi::store<u8>(gabi::ea(this) + 0x389, 0x6E); /* attention_info.distances[fopAc_Attn_TYPE_TALK_e] */
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0x6C); /* attention_info.distances[fopAc_Attn_TYPE_SPEAK_e] */
    /* attention_info.flags |= TALKFLAG_LOOK | ACTION_SPEAK | LOCKON_TALK */
    gabi::store<u32>(gabi::ea(this) + 0x39C, gabi::load<u32>(gabi::ea(this) + 0x39C) | 0x0800000A);
    eventInfo_setEventName(this, STR(0x100131C4) /* "Ji1_KmonTalk" */);
    mpJi1 = (daNpc_Ji1_l*)fopAcM_SearchByID(parentActorID);
    return cPhs_COMPLEATE_e;
}
VERIFY(0x021A1844, &daKmon_c::CreateInit);

/* 021A1A9C */
void daKmon_c::checkTalk() {
    WWHD_FUNC(0x021A1A9C, void, this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
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

    /* eyePos = attention_info.position = current.pos (word copies) */
    u32 px = gabi::load<u32>(gabi::ea(this) + 0x314);
    u32 py = gabi::load<u32>(gabi::ea(this) + 0x318);
    u32 pz = gabi::load<u32>(gabi::ea(this) + 0x31C);
    gabi::store<u32>(gabi::ea(this) + 0x390, px);
    gabi::store<u32>(gabi::ea(this) + 0x394, py);
    gabi::store<u32>(gabi::ea(this) + 0x398, pz);
    gabi::store<u32>(gabi::ea(this) + 0x37C, px);
    gabi::store<u32>(gabi::ea(this) + 0x380, py);
    gabi::store<u32>(gabi::ea(this) + 0x384, pz);
    gabi::store<f32>(gabi::ea(this) + 0x394, gabi::load<f32>(gabi::ea(this) + 0x394) + 25.0f);

    if (gabi::load<u16>(gabi::ea(this) + 0xF8) == 1 /* eventInfo.checkCommandTalk() */) {
        daNpc_Ji1_l* ji1 = mpJi1;
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
        gabi::store<u16>(gabi::ea(this) + 0xFA, gabi::load<u16>(gabi::ea(this) + 0xFA) | 1);
    }
}
VERIFY(0x021A1A9C, &daKmon_c::checkTalk);

cPhs_State daKmon_c::_create() {
    /* fopAcM_ct(this, daKmon_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = KMON_VTBL;
            mDoExt_btkAnm::ct(&mBtkAnm);
            /* mDoExt_bckAnm constructor (inline) */
            u32 b = gabi::ea(&mBckAnm);
            gabi::call(0x027F2BC0, &mBckAnm.mFrameCtrl, 0); /* J3DFrameCtrl::init */
            gabi::store<u32>(b + 0x10, 0x1016E54C);
            gabi::call(0x027DA984, gabi::at<void>(b + 0x14));
            gabi::store<u32>(b + 0x7C, 0);
            gabi::store<u32>(b + 0x48, 0x1016D820);
            gabi::store<u32>(b + 0x58, 0);
            gabi::store<u32>(b + 0x84, 0);
            gabi::store<u32>(b + 0x10, KMON_BCK_VTBL);
            gabi::store<u32>(b + 0x80, 0);
            gabi::store<u32>(b + 0x88, 0);
            dBgS_ObjAcch_ct(&mAcch, OBJACCH_VT);
            dBgS_AcchCir_ct(&mAcchCir);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State state = dComIfG_resLoad(&mPhase, m_arcname);
    if (state == cPhs_COMPLEATE_e) {
        if (fopAcM_entrySolidHeap(this, 0x021A1748 /* CheckCreateHeap */, 0x10000)) {
            state = CreateInit();
        } else {
            state = cPhs_ERROR_e;
        }
    }
    return state;
}

/* 021A1938 */
static cPhs_State daKmonCreate(void* i_this) {
    WWHD_FUNC(0x021A1938, cPhs_State, i_this);
    return ((daKmon_c*)i_this)->_create();
}
VERIFY(0x021A1938, daKmonCreate);

BOOL daKmon_c::_delete() {
    dComIfG_resDelete(&mPhase, m_arcname);
    return TRUE;
}

/* 021A1A6C */
static BOOL daKmonDelete(void* i_this) {
    WWHD_FUNC(0x021A1A6C, BOOL, i_this);
    return ((daKmon_c*)i_this)->_delete();
}
VERIFY(0x021A1A6C, daKmonDelete);

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
static BOOL daKmonExecute(void* i_this) {
    WWHD_FUNC(0x021A1D74, BOOL, i_this);
    return ((daKmon_c*)i_this)->_execute();
}
VERIFY(0x021A1D74, daKmonExecute);

BOOL daKmon_c::_draw() {
    settingTevStruct(dKy_getEnvlight(), 0x5C /* TEV_TYPE_BG1_PLIGHT */, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    mBckAnm.entry(J3DModel_getModelData(mpModel), mBckAnm.getFrame());
    mBtkAnm.entry(J3DModel_getModelData(mpModel), mBtkAnm.getFrame());
    mDoExt_modelUpdateDL(mpModel);
    return TRUE;
}

/* 021A1DD8 */
static BOOL daKmonDraw(void* i_this) {
    WWHD_FUNC(0x021A1DD8, BOOL, i_this);
    return ((daKmon_c*)i_this)->_draw();
}
VERIFY(0x021A1DD8, daKmonDraw);

/* 021A1F04 */
static BOOL daKmonIsDelete(void* i_this) {
    WWHD_FUNC(0x021A1F04, BOOL, i_this);
    return TRUE;
}
VERIFY(0x021A1F04, daKmonIsDelete);

/* 021A1E5C */
static void __sinit_d_a_kmon_cpp() {
    WWHD_FUNC(0x021A1E5C, void, (u32)0);
    sinit_header_statics(0x10464BE8, 0x101B8540);
}
VERIFY(0x021A1E5C, __sinit_d_a_kmon_cpp);

/* 021A1EF0: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x021A1EF0, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x021A1EF0, trivial_dt);

/* 021A1F0C: daKmon_c deleting destructor */
static void daKmon_c_dt(daKmon_c* i_this, s32 flags) {
    WWHD_FUNC(0x021A1F0C, void, i_this, flags);
    if (i_this != nullptr) {
        u32 b = gabi::ea(i_this);
        gabi::call(0x02018034, gabi::at<void>(b + 0x690), 2); /* mAcchCir's cM3dGCir::~cM3dGCir */
        /* dBgS_ObjAcch::~dBgS_ObjAcch: the ObjAcch vtables, then dBgS_Acch::~dBgS_Acch */
        gabi::store<u32>(b + 0x4D8, OBJACCH_VT.v20);
        gabi::store<u32>(b + 0x4CC, OBJACCH_VT.v14);
        gabi::call(0x024EFD9C, &i_this->mAcch, 0);
        gabi::call(0x027F3628, gabi::at<void>(b + 0x43C), 0); /* mBckAnm's J3DMtxCalc part */
        gabi::call(0x025D50BC, i_this, 0);                   /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x021A1F0C, daKmon_c_dt);

/* 021A1F9C: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x021A1F9C, void, p);
}
VERIFY(0x021A1F9C, empty_virtual);
