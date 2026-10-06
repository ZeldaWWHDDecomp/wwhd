/**
 * d_a_npc_de1.cpp (WWHD)
 * NPC - Great Deku Tree
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_de1.cpp) has only "Nonmatching" stubs for this actor, so the functions are
 * written from the WWHD code, verified against cking.rpx. Names follow the GameCube symbols.
 */
#include "d/actor/d_a_npc_de1.h"

#define SAFESTRING_VTBL 0x10019664 /* this TU's sead::SafeString vtable */
#define DE1_VTBL 0x10019948        /* daNpc_De1_c vtable (HD: merged with fopNpc_npc_c's) */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* resources by id / by name (HD: sead::SafeString keys, per-TU vtable) */
static inline void* dComIfG_getObjectIDRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->__vtbl = SAFESTRING_VTBL;
    key->mStringTop = gabi::ea(arc);
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), id);
}
/* 02606900 dRes_control_c::getRes(arcName, resName) by file name */
static inline void* dComIfG_getObjectNameRes(const char* arc, const char* name) {
    gabi::Local<SafeString> a;
    gabi::Local<SafeString> n;
    a->__vtbl = SAFESTRING_VTBL;
    n->__vtbl = SAFESTRING_VTBL;
    n->mStringTop = gabi::ea(name);
    a->mStringTop = gabi::ea(arc);
    return gabi::call<void*>(0x02606900, dComIfG_resControl(), a.get(), n.get());
}
/* J3DModelData (HD): 027F68FC returns the joint name table header (self-relative offset at +0x10) */
static inline u32 J3DModelData_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s8 JUTNameTab_getIndex(u32 tab, const char* name) { return gabi::call<s8>(0x027DF9B0, tab, name); }
/* HD J3D: a model's joint matrices live in a block at +0x2C (+0x4 flags, +0x10 matrices) */
static Mtx34* getAnmMtx(J3DModel* model, s32 jntNo) {
    u32 blk = gabi::load<u32>(gabi::ea(model) + 0x2C);
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10)); /* HD: joint matrices dirty */
    return gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jntNo * 0x30);
}
/* dBgWDeform (0xC4): 024F5A18 constructor (dBgWSv base), vtable at +4; 024F49E8 Set(data, flags)
 * returns true on failure; 024F5B08 dBgWSv::CopyBackVtx */
static inline void dBgWDeform_ct(void* p) { gabi::call(0x024F5A18, p); }
static inline bool dBgWDeform_Set(dBgW* w, void* data, u32 flags) { return gabi::call<bool>(0x024F49E8, w, data, flags); }
static inline void dBgWSv_CopyBackVtx(dBgW* w) { gabi::call(0x024F5B08, w); }
/* skinned vertex object {res, vtx buffer}: 025EDD64 init(res) (allocates the buffer), 025EDE0C
 * calc(model) (deforms the vertices with the model's joint matrices) */
static inline BOOL deformVtx_init(void* p, void* res) { return gabi::call<BOOL>(0x025EDD64, p, res); }
static inline void deformVtx_calc(void* p, J3DModel* m) { gabi::call(0x025EDE0C, p, m); }
/* HD fopAcM_SearchByID(id, fopAc_ac_c** out) */
static inline void fopAcM_SearchByID_out(u32 id, be<u32>* out) { gabi::call(0x025D54C4, id, out); }
/* dBgS (play + 0x12A0) */
static inline s8 dBgS_GetRoomId_l(dBgS* bgs, void* poly) { return gabi::call<s8>(0x024EF130, bgs, poly); }
static inline u8 dBgS_GetPolyColor_l(dBgS* bgs, void* poly) { return gabi::call<u8>(0x024EEEB8, bgs, poly); }
/* save info: event flags (dSv_event_c) at *(0x101F84DC) + 0x644, the pointer re-read at each use */
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
/* a second dSv_event_c at save + 0x1178 (probably the temporary event bits, dComIfGs_onTmpBit) */
static inline void dComIfGs_onTmpBit(u16 f) { dSv_event_onEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x1178), f); }
/* 025B7D90 dSv_player_collect_c::isSymbol (collect at save + 0xD4) */
static inline BOOL dComIfGs_isSymbol(u8 i) { return gabi::call<BOOL>(0x025B7D90, gabi::load<u32>(0x101F84DC) + 0xD4, i); }
static inline BOOL dComIfGs_checkGetItem(u8 item) { return gabi::call<BOOL>(0x02520C0C, item); }
static inline BOOL dComIfGs_isStageBossEnemy(s32 stage) { return gabi::call<BOOL>(0x02520A84, stage); }
static inline u32 dLib_setFirstMsg(u16 eventBit, u32 firstMsg, u32 secondMsg) { return gabi::call<u32>(0x02588034, eventBit, firstMsg, secondMsg); }
/* play object */
static inline u8 dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }
static inline u8 dComIfGp_getMesgAnimeAttrInfo() { return gabi::load<u8>(dComIfGp_ea() + PLAY_BGS + 0x4925); }
/* event manager (play + 0x52C4) */
static inline BOOL dComIfGp_evmng_startCheckOld(const char* name) { return gabi::call<BOOL>(0x025445B8, dComIfGp_getPEvtManager(), name); }
static inline BOOL dComIfGp_evmng_endCheckOld(const char* name) { return gabi::call<BOOL>(0x0254457C, dComIfGp_getPEvtManager(), name); }
static inline s8 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force, s32 nameType) {
    return gabi::call<s8>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
/* 0253F124 dEvt_control_c::getPId(actor); the result goes to the event control's +0xD0 (probably
 * the item partner id) */
static inline u32 dEvt_control_getPId(dEvt_control_c* e, fopAc_ac_c* a) { return gabi::call<u32>(0x0253F124, e, a); }
/* HD message manager (*(0x101F4B5C)): 025F795C returns the current message's status, +0x948 the
 * selected answer */
static inline u32 fopMsgM_getStatus() { return gabi::call<u32>(0x025F795C, gabi::load<u32>(0x101F4B5C)); }
static inline u32 fopMsgM_getSelectNum() { return gabi::load<u32>(gabi::load<u32>(0x101F4B5C) + 0x948); }
/* 0252012C dComIfGp_setNextStage(stage, point, roomNo, layer, lastSpeed, lastMode, setPoint, wipe) */
static inline void dComIfGp_setNextStage(const char* stage, s16 point, s8 room, s8 layer, f32 speed, u32 mode, BOOL set, s8 wipe) {
    gabi::call(0x0252012C, stage, point, room, layer, speed, mode, set, wipe);
}
/* 025D77DC fopAcM_orderOtherEvent2(actor, name, flag, hind) */
static inline BOOL fopAcM_orderOtherEvent2(fopAc_ac_c* a, u32 name, u16 flag, u16 hind) { return gabi::call<BOOL>(0x025D77DC, a, name, flag, hind); }
/* 025E18EC: audio request with one id (0x80000045 in checkOrder; probably a BGM/fanfare start) */
static inline void mDoAud_req_l(u32 id) { gabi::call(0x025E18EC, id); }
/* daLlift_c (leaf lift) statics on a lift actor: 021B4090 MoveUpLift, 021B430C (unnamed; checked
 * by wait03 before the Deku Tree goes back to its first state, probably "lift is down") */
static inline BOOL daLlift_MoveUpLift(fopAc_ac_c* l) { return gabi::call<BOOL>(0x021B4090, l); }
static inline BOOL daLlift_021B430C(fopAc_ac_c* l) { return gabi::call<BOOL>(0x021B430C, l); }
/* 02055B64 cLib_calcTimer<s16> (out of line) */
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }

/* ---- file statics ---- */
/* l_HIO (daNpc_De1_HIO_c, 0x28 bytes): +0 mNo, +4 mCount, +8 parameters (0x1C bytes, .data
 * 0x101BE090), HD vtable at +0x24 */
static const u32 L_HIO = 0x10466D18;
static inline f32 HIO_attY() { return gabi::load<f32>(L_HIO + 0x08); }     /* attention height offset */
static inline f32 HIO_attDist() { return gabi::load<f32>(L_HIO + 0x0C); }  /* chkAttention radius */
static inline f32 HIO_scale() { return gabi::load<f32>(L_HIO + 0x10); }
static inline f32 HIO_callDist() { return gabi::load<f32>(L_HIO + 0x14); } /* wait04 radius */
/* leaf lift search results (searchActor_leafLift) */
static const u32 L_LIFT_NUM = 0x10466D08;
static const u32 L_LIFT_ACTOR = 0x10466D50; /* fopAc_ac_c*[100] */

/* pointers to member functions in .data (copied to the stack before set_action) */
enum : u32 { PMF_wait_action2 = 0x10019650, PMF_wait_action1 = 0x10019658 };
/* (this->*pmf)(arg) */
static inline void pmf_call(void* self, ProcFunc_l* pmf, void* arg) {
    s16 i = pmf->i;
    u32 thisp = gabi::ea(self) + (s32)(s16)pmf->d;
    if (i < 0) {
        gabi::call_ptr<BOOL>(pmf->f, thisp, arg);
    } else {
        s16 voff = gabi::load<s16>(gabi::ea(pmf) + 6);
        u32 vt = gabi::load<u32>(thisp + voff);
        gabi::call_ptr<BOOL>(gabi::load<u32>(vt + i * 8 + 4), thisp, arg);
    }
}
static inline void copy_f32_bits(u32 dst, u32 src) { gabi::store<u32>(dst, gabi::load<u32>(src)); }
static inline u32 actorEa(fopAc_ac_c* a) { return gabi::ea(a); }

/* 0222A8FC */
static void* searchActor_leafLift(void* i_actor, void*) {
    WWHD_FUNC(0x0222A8FC, void*, i_actor, (void*)nullptr);
    if (gabi::load<s32>(L_LIFT_NUM) < 100 && fopAc_IsActor(i_actor) && i_actor != nullptr &&
        fpcM_GetName(i_actor) == 0x78 /* PROC_Obj_Llift */) {
        s32 n = gabi::load<s32>(L_LIFT_NUM);
        gabi::store<s32>(L_LIFT_NUM, n + 1);
        gabi::store<u32>(L_LIFT_ACTOR + n * 4, gabi::ea(i_actor));
    }
    return nullptr;
}
VERIFY(0x0222A8FC, searchActor_leafLift);

/* 0222A97C */
BOOL daNpc_De1_c::CreateHeap() {
    WWHD_FUNC(0x0222A97C, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectIDRes(STR(0x10019738) /* "De" */, 7);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectIDRes(STR(0x10019738), 5);
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, 2 /* LOOP */, 1.0f, 0, -1, 1, nullptr,
                                    0x80000, 0x11000002);
    if (mpMorf.get() == nullptr) {
        return FALSE;
    }
    if (mpMorf->getModel() == nullptr) {
        mpMorf = nullptr;
        return FALSE;
    }
    mBranchJnt = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x10019728) /* "branchL" */);
    if (mBranchJnt < 0)
        JUT_ASSERT_fail(STR(0x10019744), 0x704, STR(0x10019780));
    mHeadJnt = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(0x1001973C) /* "head" */);
    if (mHeadJnt < 0)
        JUT_ASSERT_fail(STR(0x10019744), 0x706, STR(0x10019754));
    /* the joints of the ten leaf lifts (name table 0x101BDF24) */
    for (int i = 0; i < 10; i++) {
        u32 name = gabi::load<u32>(0x101BDF24 + 4 * i);
        mLiftJnt[i] = JUTNameTab_getIndex(J3DModelData_getJointName(modelData), STR(name));
        if (mLiftJnt[i] < 0)
            JUT_ASSERT_fail(STR(0x10019744), 0x709, STR(0x10019768));
    }
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, 0); /* setUserArea(0) */
    /* mpBgW = new dBgWDeform() */
    u8* bgw = (u8*)operator_new(0xC4);
    if (bgw != nullptr) {
        dBgWDeform_ct(bgw);
        gabi::store<u32>(gabi::ea(bgw) + 4, 0x100196C8);
    }
    mpBgW = (dBgW*)(void*)bgw;
    if (bgw == nullptr) {
        mpMorf = nullptr;
        return FALSE;
    }
    void* bgData = dComIfG_getObjectIDRes(STR(0x10019738), 8);
    if (dBgWDeform_Set(mpBgW, bgData, 0)) {
        mpMorf = nullptr;
        return FALSE;
    }
    void* vtx = dComIfG_getObjectNameRes(STR(0x10019738), STR(0x10019730) /* "de.cvtx" */);
    if (!deformVtx_init(&mDeformRes, vtx)) {
        mpMorf = nullptr;
        return FALSE;
    }
    mAcchCir.SetWall(0.0f, 0.0f);
    mObjAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    mObjAcch.m_flags |= 0x40C;
    return TRUE;
}
VERIFY(0x0222A97C, &daNpc_De1_c::CreateHeap);

/* 0222AC80: tail call, CreateHeap's result register is passed through */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0222AC80, BOOL, i_this);
    return static_cast<daNpc_De1_c*>(i_this)->CreateHeap();
}
VERIFY(0x0222AC80, CheckCreateHeap);

/* 0222AC84 */
BOOL daNpc_De1_c::decideType(int i_prm) {
    WWHD_FUNC(0x0222AC84, BOOL, this, i_prm);
    m934 = -1;
    if (fpcM_GetName(this) == 0x74 /* PROC_NPC_DE1 */) {
        m934 = 0;
        mType = dComIfGs_isSymbol(2) != 0;
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x0222AC84, &daNpc_De1_c::decideType);

/* 0222AD00 */
BOOL daNpc_De1_c::set_action(ProcFunc_l* i_newProcFunc, void* i_argsP) {
    WWHD_FUNC(0x0222AD00, BOOL, this, i_newProcFunc, i_argsP);
    ProcFunc_l* cur = &mAction;
    s16 newI = i_newProcFunc->i;
    s16 newD;
    u32 newF;
    if ((u16)cur->i == (u16)newI) {
        if (cur->i == 0)
            return TRUE;
        newD = i_newProcFunc->d;
        newF = i_newProcFunc->f;
        if ((u16)cur->d == (u16)newD && cur->f == newF)
            return TRUE;
    } else {
        newF = i_newProcFunc->f;
        newD = i_newProcFunc->d;
        if (cur->i == 0)
            goto set;
    }
    mActPhase = -1;
    pmf_call(this, cur, i_argsP);
set:
    cur->f = newF;
    cur->d = newD;
    cur->i = newI;
    mActPhase = 0;
    pmf_call(this, cur, i_argsP);
    return TRUE;
}
VERIFY(0x0222AD00, &daNpc_De1_c::set_action);

/* 0222AE24 */
void daNpc_De1_c::followPa_happa() {
    WWHD_FUNC(0x0222AE24, void, this);
    if (mPaHappa.getEmitter() != nullptr) {
        PSMTXCopy(getAnmMtx(mpMorf->getModel(), mBranchJnt), mDoMtx_stack_c::get());
        u32 m = gabi::ea(mDoMtx_stack_c::get());
        copy_f32_bits(gabi::ea(&mPaHappaPos.x), m + 0x0C);
        copy_f32_bits(gabi::ea(&mPaHappaPos.y), m + 0x1C);
        copy_f32_bits(gabi::ea(&mPaHappaPos.z), m + 0x2C);
    }
}
VERIFY(0x0222AE24, &daNpc_De1_c::followPa_happa);

/* 0222AEB4 */
void daNpc_De1_c::setDemoStartCenter() {
    WWHD_FUNC(0x0222AEB4, void, this);
    gabi::Local<cXyz> offset; /* l_HIO demo start offset */
    copy_f32_bits(gabi::ea(&offset->x), L_HIO + 0x18);
    copy_f32_bits(gabi::ea(&offset->y), L_HIO + 0x1C);
    copy_f32_bits(gabi::ea(&offset->z), L_HIO + 0x20);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    PSMTXMultVec(mDoMtx_stack_c::get(), offset.get(), &mDemoStartCenter);
}
VERIFY(0x0222AEB4, &daNpc_De1_c::setDemoStartCenter);

/* 0222AF38 (the matcher calls it cLib_calcTimer<s>) */
fopAc_ac_c* daNpc_De1_c::searchByID(u32 i_id) {
    WWHD_FUNC(0x0222AF38, fopAc_ac_c*, this, i_id);
    gabi::Local<be<u32>> actor;
    *actor = 0;
    fopAcM_SearchByID_out(i_id, actor.get());
    return gabi::at<fopAc_ac_c>(*actor);
}
VERIFY(0x0222AF38, &daNpc_De1_c::searchByID);

/* 0222AF6C: the leaf lifts follow their joints */
void daNpc_De1_c::cc_set() {
    WWHD_FUNC(0x0222AF6C, void, this);
    for (int i = 0; i < 10; i++) {
        fopAc_ac_c* lift = searchByID(mLiftId[i]);
        if (lift != nullptr) {
            PSMTXCopy(getAnmMtx(mpMorf->getModel(), mLiftJnt[i]), mDoMtx_stack_c::get());
            PSMTXCopy(mDoMtx_stack_c::get(), gabi::at<Mtx34>(actorEa(lift) + 0x8F4));
        }
    }
}
VERIFY(0x0222AF6C, &daNpc_De1_c::cc_set);

/* 0222B008 */
void daNpc_De1_c::setAttention() {
    WWHD_FUNC(0x0222B008, void, this);
    gabi::Local<cXyz> offset;
    offset->x = 0.0f;
    offset->z = 700.0f;
    offset->y = 1100.0f;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    PSMTXMultVec(mDoMtx_stack_c::get(), offset.get(), &mAttPos);
    copy_f32_bits(gabi::ea(&m878.x), gabi::ea(&mAttPos.x));
    copy_f32_bits(gabi::ea(&m878.y), gabi::ea(&mAttPos.y));
    copy_f32_bits(gabi::ea(&m878.z), gabi::ea(&mAttPos.z));
    copy_f32_bits(gabi::ea(&eyePos.x), gabi::ea(&mAttPos.x));
    copy_f32_bits(gabi::ea(&eyePos.y), gabi::ea(&mAttPos.y));
    copy_f32_bits(gabi::ea(&eyePos.z), gabi::ea(&mAttPos.z));
    u32 att = gabi::ea(this) + 0x390; /* attention_info.position */
    copy_f32_bits(att + 0, gabi::ea(&mAttPos.x));
    gabi::store<f32>(att + 4, gabi::fadds_ppc(mAttPos.y, HIO_attY()));
    copy_f32_bits(att + 8, gabi::ea(&mAttPos.z));
}
VERIFY(0x0222B008, &daNpc_De1_c::setAttention);

/* 0222B0CC */
void daNpc_De1_c::setMtx() {
    WWHD_FUNC(0x0222B0CC, void, this);
    if (mInDemo == 0) {
        mAnmEnd = (s8)mpMorf->play(&eyePos, 0, 0);
        if (mpMorf->getFrame() < mAnmFrame) {
            mAnmEnd = 1;
        }
        copy_f32_bits(gabi::ea(&mAnmFrame), gabi::ea(mpMorf.get()) + 0x9C); /* frame */
        mObjAcch.CrrPos(dComIfG_Bgsp());
    }
    if (cLib_calcTimer(&mSeTimer) == 0 && mAnmEnd != 0 && mAnmNum == 3) {
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
        mDoAud_seStart(0x48B9, &current.pos, 0, reverb);
    }
    void* gnd = gabi::at<u8>(gabi::ea(&mObjAcch) + 0xE8); /* ground polygon info */
    gabi::store<s8>(gabi::ea(&tevStr) + 0xB9, dBgS_GetRoomId_l(dComIfG_Bgsp(), gnd));
    gabi::store<u8>(gabi::ea(&tevStr) + 0xBA, dBgS_GetPolyColor_l(dComIfG_Bgsp(), gnd));
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    f32 s = HIO_scale();
    mDoMtx_stack_c::scaleM(s, s, s);
    J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
    dBgWSv_CopyBackVtx(mpBgW);
    mpMorf->calc();
    deformVtx_calc(&mDeformRes, mpMorf->getModel());
    dBgW* bgw = mpBgW;
    u32 old = gabi::load<u32>(gabi::ea(bgw) + 0x90);
    gabi::store<u32>(gabi::ea(bgw) + 0x90, mDeformVtx);
    if (old == 0) {
        dBgWSv_CopyBackVtx(bgw);
    }
    dBgW_Move(bgw);
    followPa_happa();
    setDemoStartCenter();
    if (mType == 0 && !dComIfGs_isEventBit(0x1801)) {
        cc_set();
    }
    setAttention();
}
VERIFY(0x0222B0CC, &daNpc_De1_c::setMtx);

/* 0222B300 */
BOOL daNpc_De1_c::createInit() {
    WWHD_FUNC(0x0222B300, BOOL, this);
    mEventCut2.setActorInfo2(STR(0x100197C0) /* "De1" */, this);
    s8 type = mType;
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0xA);  /* attention_info.flags */
    gabi::store<u8>(gabi::ea(this) + 0x38B, 0x16); /* attention_info.distances[3] */
    gabi::store<u8>(gabi::ea(this) + 0x389, 0x15); /* attention_info.distances[1] */
    copy_f32_bits(gabi::ea(&mHomePos.x), gabi::ea(&current.pos.x));
    copy_f32_bits(gabi::ea(&mHomePos.y), gabi::ea(&current.pos.y));
    copy_f32_bits(gabi::ea(&mHomePos.z), gabi::ea(&current.pos.z));
    gravity = 0.0f;
    if ((u32)(s32)type < 1) {
        if (!dComIfGs_isEventBit(0x1801)) {
            gabi::Local<ProcFunc_l> pmf;
            gabi::store<u32>(gabi::ea(pmf.get()), gabi::load<u32>(PMF_wait_action1));
            gabi::store<u32>(gabi::ea(pmf.get()) + 4, gabi::load<u32>(PMF_wait_action1 + 4));
            set_action(pmf.get(), nullptr);
        } else {
            gabi::Local<ProcFunc_l> pmf;
            gabi::store<u32>(gabi::ea(pmf.get()), gabi::load<u32>(PMF_wait_action2));
            gabi::store<u32>(gabi::ea(pmf.get()) + 4, gabi::load<u32>(PMF_wait_action2 + 4));
            set_action(pmf.get(), nullptr);
        }
    } else if ((u32)(s32)type == 1) {
        gabi::Local<ProcFunc_l> pmf;
        gabi::store<u32>(gabi::ea(pmf.get()), gabi::load<u32>(PMF_wait_action2));
        gabi::store<u32>(gabi::ea(pmf.get()) + 4, gabi::load<u32>(PMF_wait_action2 + 4));
        set_action(pmf.get(), nullptr);
    }
    shape_angle.y = current.angle.y;
    shape_angle.z = current.angle.z;
    shape_angle.x = current.angle.x;
    mpMorf->setMorf(0.0f);
    m8C4 = 1;
    setMtx();
    return TRUE;
}
VERIFY(0x0222B300, &daNpc_De1_c::createInit);

/* 0222B4B8 */
cPhs_State daNpc_De1_c::_create() {
    WWHD_FUNC(0x0222B4B8, cPhs_State, this);
    /* fopAcM_ct(this, daNpc_De1_c): inline constructor */
    if (!fopAcM_CheckCondition(this, 8 /* fopAcCnd_INIT_e */)) {
        if (this != nullptr) {
            gabi::call(0x025A1458, this); /* fopNpc_npc_c::fopNpc_npc_c (the matcher calls it cDyl_LinkASync) */
            mDeformRes = 0;
            mDeformVtx = 0;
            __vtbl = DE1_VTBL;
            gabi::call(0x0259F740, &mEventCut2); /* dNpc_EventCut_c::dNpc_EventCut_c */
            dPa_followEcallBack_ct(&mPaHappa, 0, 0);
        }
        fopAcM_OnCondition(this, 8);
    }
    cPhs_State state = dComIfG_resLoad(&mPhs, STR(0x100197C4) /* "De" */);
    if (state != cPhs_COMPLEATE_e) {
        return state;
    }
    if (!decideType(gabi::load<u8>(gabi::ea(this) + 0xB3) /* fopAcM_GetParam(this) & 0xFF */)) {
        return cPhs_ERROR_e;
    }
    /* l_HIO.entryHIO("デクの木") */
    if (gabi::load<s32>(L_HIO + 4) < 0) {
        s8 no = mDoHIO_createChild(STR(0x100197C8), gabi::at<void>(L_HIO));
        gabi::store<s8>(L_HIO, no);
    }
    gabi::store<s32>(L_HIO + 4, gabi::load<s32>(L_HIO + 4) + 1);
    if (!fopAcM_entrySolidHeap(this, 0x0222AC80 /* CheckCreateHeap */, gabi::load<u32>(0x101BDF4C))) {
        return cPhs_ERROR_e;
    }
    J3DModel* model = mpMorf->getModel();
    cullMtx = model != nullptr ? gabi::ea(model) + 0xC8 : 0; /* fopAcM_SetMtx */
    dBgS_Regist(dComIfG_Bgsp(), mpBgW, this);
    if (!createInit()) {
        return cPhs_ERROR_e;
    }
    return state;
}
VERIFY(0x0222B4B8, &daNpc_De1_c::_create);

/* 0222B634 */
static cPhs_State daNpc_De1_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0222B634, cPhs_State, i_this);
    return static_cast<daNpc_De1_c*>(i_this)->_create();
}
VERIFY(0x0222B634, daNpc_De1_Create);

/* 0222B638 */
void daNpc_De1_c::del_pa_happa() {
    WWHD_FUNC(0x0222B638, void, this);
    mPaHappa.remove();
}
VERIFY(0x0222B638, &daNpc_De1_c::del_pa_happa);

/* 0222B648 */
BOOL daNpc_De1_c::_delete() {
    WWHD_FUNC(0x0222B648, BOOL, this);
    dComIfG_resDelete(&mPhs, STR(0x100197D4) /* "De" */);
    dBgS* bgs = dComIfG_Bgsp();
    cBgS_Release(bgs, mpBgW);
    if (heap.get() != nullptr && mpMorf.get() != nullptr) {
        mpMorf->stopZelAnime();
    }
    del_pa_happa();
    /* l_HIO.removeHIO() */
    s32 n = gabi::load<s32>(L_HIO + 4);
    if (n >= 0) {
        gabi::store<s32>(L_HIO + 4, n - 1);
        if (n - 1 < 0) {
            mDoHIO_deleteChild(gabi::load<s8>(L_HIO));
        }
    }
    return TRUE;
}
VERIFY(0x0222B648, &daNpc_De1_c::_delete);

/* 0222B6E4 */
static BOOL daNpc_De1_Delete(daNpc_De1_c* i_this) {
    WWHD_FUNC(0x0222B6E4, BOOL, i_this);
    return i_this->_delete();
}
VERIFY(0x0222B6E4, daNpc_De1_Delete);

/* 0222B6E8 */
void daNpc_De1_c::checkOrder() {
    WWHD_FUNC(0x0222B6E8, void, this);
    u16 cmd = gabi::load<u16>(gabi::ea(this) + 0xF8); /* eventInfo.mCommand */
    if (cmd == 2) {
        if (dComIfGp_evmng_startCheckOld(STR(0x100197D8) /* "LIFT_UP" */) && mOrder == 3) {
            mOrder = 0;
            return;
        }
        if (dComIfGp_evmng_startCheckOld(STR(0x100197F0) /* "DE_CHUCHU" */) && mOrder == 4) {
            mDoAud_req_l(0x80000045);
            mOrder = 0;
            return;
        }
        if (dComIfGp_evmng_startCheckOld(STR(0x100197E0) /* "contact" */) && mOrder == 5) {
            dComIfGs_onEventBit(0x1801);
            dComIfGp_setNextStage(STR(0x100197E8) /* "Omori" */, 0xD5, 0, 8, 0.0f, 0, 1, 0);
            mOrder = 0;
        }
    } else if (cmd == 1) {
        if (mOrder == 1 || mOrder == 2) {
            mOrder = 0;
            mTalkReq = 1;
        }
    }
}
VERIFY(0x0222B6E8, &daNpc_De1_c::checkOrder);

/* 0222B878 */
u8 daNpc_De1_c::demo() {
    WWHD_FUNC(0x0222B878, u8, this);
    if (demoActorID == 0) {
        if (mInDemo != 0) {
            mInDemo = 0;
            return 0;
        }
        return mInDemo;
    }
    u8 id = demoActorID;
    mInDemo = 1;
    /* dComIfGp_demo_getActor(demoActorID): HD inline with a range check and the demo object
     * (0x101D5FFC) asserted; the result is not used */
    if (id != 0 && id <= 0x20) {
        u32 obj = gabi::load<u32>(0x101D5FFC);
        if (obj == 0) { /* JUT_ASSERT(570, m_object != NULL) (d_demo.h) */
            JUT_ASSERT_fail(STR(0x100196BC), 0x23A, STR(0x100196AC));
            obj = gabi::load<u32>(0x101D5FFC);
        }
        gabi::call<void*>(0x02526E70, obj, id); /* dDemo_object_c::getActor */
    }
    gabi::call<BOOL>(0x02527028, this, 0x6A, mpMorf.get(), STR(0x100197FC) /* "De" */, 0, 0, 0, 0); /* dDemo_setDemoData */
    return mInDemo;
}
VERIFY(0x0222B878, &daNpc_De1_c::demo);

/* 0222B950 */
void daNpc_De1_c::endEvent() {
    WWHD_FUNC(0x0222B950, void, this);
    dComIfGp_event_reset();
    mAnmAtr = 0xFF;
}
VERIFY(0x0222B950, &daNpc_De1_c::endEvent);

/* 0222B990 */
BOOL daNpc_De1_c::anmResID(u32 i_anmNum, be<s32>* o_bckIdx, be<s32>* o_soundIdx) {
    WWHD_FUNC(0x0222B990, BOOL, this, i_anmNum, o_bckIdx, o_soundIdx);
    if (i_anmNum >= 7) /* JUT_ASSERT(317, 0 <= i_anmNum && i_anmNum < 7) */
        JUT_ASSERT_fail(STR(0x10019838), 0x13D, STR(0x10019848));
    if (o_bckIdx == nullptr || o_soundIdx == nullptr)
        JUT_ASSERT_fail(STR(0x10019838), 0x13E, STR(0x10019868));
    /* a_anm_tbl (.data 0x10019800): {bck, sound} pairs */
    *o_bckIdx = gabi::load<s32>(0x10019800 + i_anmNum * 8);
    *o_soundIdx = gabi::load<s32>(0x10019800 + i_anmNum * 8 + 4);
    return TRUE;
}
VERIFY(0x0222B990, &daNpc_De1_c::anmResID);

/* 0222BA40 */
void daNpc_De1_c::set_pa_happa() {
    WWHD_FUNC(0x0222BA40, void, this);
    mPaHappa.remove();
    s8 room = fopAcM_GetRoomNo(this);
    dPa_control_c* pa = dComIfGp_getParticle();
    dPa_control_set(pa, 0, 0x81BA, &mPaHappaPos, &current.angle, nullptr, 0xFF,
                    gabi::at<dPa_levelEcallBack>(gabi::ea(&mPaHappa)), room, nullptr, nullptr, nullptr);
}
VERIFY(0x0222BA40, &daNpc_De1_c::set_pa_happa);

/* 0222BAC8 */
BOOL daNpc_De1_c::setAnm_anm(anm_prm_c* i_anmPrm) {
    WWHD_FUNC(0x0222BAC8, BOOL, this, i_anmPrm);
    s8 num = i_anmPrm->mAnmNum;
    BOOL ret = FALSE;
    if (num < 0 || mAnmNum == num) {
        return FALSE;
    }
    mAnmNum = num;
    if (mpMorf.get() != nullptr) {
        gabi::Local<be<s32>> bckIdx;
        gabi::Local<be<s32>> soundIdx;
        anmResID((u32)(s32)num, bckIdx.get(), soundIdx.get());
        if (*bckIdx >= 0) {
            dNpc_setAnmIDRes(mpMorf, i_anmPrm->mLoopMode, i_anmPrm->mMorf, i_anmPrm->mSpeed, *bckIdx, *soundIdx,
                             STR(0x10019880) /* "De" */);
        }
        if (mAnmNum == 3) {
            set_pa_happa();
        } else {
            del_pa_happa();
        }
        ret = TRUE;
    }
    m8BD = 0;
    mAnmFrame = 0.0f;
    mAnmEnd = 0;
    return ret;
}
VERIFY(0x0222BAC8, &daNpc_De1_c::setAnm_anm);

/* 0222BBA8: animation of the current state (a_stt_anm_tbl, .data 0x101BDF50) */
BOOL daNpc_De1_c::setAnm() {
    WWHD_FUNC(0x0222BBA8, BOOL, this);
    return setAnm_anm(gabi::at<anm_prm_c>(0x101BDF50 + mStt * 0x10));
}
VERIFY(0x0222BBA8, &daNpc_De1_c::setAnm);

/* 0222BBC4 */
void daNpc_De1_c::setStt(u32 i_stt) {
    WWHD_FUNC(0x0222BBC4, void, this, i_stt);
    s8 old = mStt;
    mStt = (s8)i_stt;
    switch (i_stt) {
    case 2:
    case 4:
        mPrevStt = old;
        mAnmAtr = 0xFF;
        return;
    case 7:
        gabi::store<u8>(gabi::ea(this) + 0x38A, 0x22); /* attention_info.distances[2] */
        gabi::store<u32>(gabi::ea(this) + 0x39C, gabi::load<u32>(gabi::ea(this) + 0x39C) | 0x4000000);
        setAnm();
        return;
    default:
        setAnm();
        return;
    }
}
VERIFY(0x0222BBC4, &daNpc_De1_c::setStt);

/* 0222BC18 (a_anm_num_tbl, .data 0x101BDFD0) */
BOOL daNpc_De1_c::setAnm_NUM(int i_num) {
    WWHD_FUNC(0x0222BC18, BOOL, this, i_num);
    return setAnm_anm(gabi::at<anm_prm_c>(0x101BDFD0 + i_num * 0x10));
}
VERIFY(0x0222BC18, &daNpc_De1_c::setAnm_NUM);

/* 0222BC2C */
void daNpc_De1_c::event_actionInit(int i_staffIdx) {
    WWHD_FUNC(0x0222BC2C, void, this, i_staffIdx);
    be<s32>* actNo = (be<s32>*)dComIfGp_evmng_getMySubstanceP(i_staffIdx, STR(0x10019888) /* "ActNo" */, 3);
    be<s32>* timer = (be<s32>*)dComIfGp_evmng_getMySubstanceP(i_staffIdx, STR(0x10019890) /* "Timer" */, 3);
    be<s32>* num = (be<s32>*)dComIfGp_evmng_getMySubstanceP(i_staffIdx, STR(0x10019884) /* "Num" */, 3);
    if (actNo == nullptr) {
        return;
    }
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s8 act = (s8)gabi::load<u8>(gabi::ea(actNo) + 3);
    mEvtActNo = act;
    switch ((u32)(s32)act) {
    case 1: /* the selected leaf lift becomes the item partner */
        if (num != nullptr) {
            fopAc_ac_c* lift = searchByID(mLiftId[(s32)*num]);
            if (lift != nullptr) {
                dEvt_control_c* evt = dComIfGp_getEvent();
                gabi::store<u32>(gabi::ea(evt) + 0xD0, dEvt_control_getPId(evt, lift));
            }
        }
        break;
    case 2: {
        s16 t = 0;
        if (timer != nullptr)
            t = gabi::load<s16>(gabi::ea(timer) + 2);
        mEvtTimer = t;
        s16 n = 0;
        if (num != nullptr)
            n = gabi::load<s16>(gabi::ea(num) + 2);
        mEvtLiftNo = n;
        break;
    }
    case 3:
        if (num != nullptr)
            setAnm_NUM(*num);
        break;
    case 4: {
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
        mDoAud_seStart(0x48B8, &current.pos, 0, reverb);
        mSeTimer = 0x54;
        break;
    }
    case 5: { /* player demo mode (daPy_lk_c fields; virtual at vtable +0xE4) */
        u32 p = gabi::ea(player);
        gabi::store<s32>(p + 0x430, 0x18);
        gabi::store<u16>(p + 0x420, 3);
        gabi::store<s32>(p + 0x428, 0);
        u32 vt = gabi::load<u32>(p + 0xB4);
        gabi::call_ptr(gabi::load<u32>(vt + 0xE4), player, 0x1D);
        break;
    }
    }
}
VERIFY(0x0222BC2C, &daNpc_De1_c::event_actionInit);

/* 0222BEB0 */
BOOL daNpc_De1_c::event_action() {
    WWHD_FUNC(0x0222BEB0, BOOL, this);
    switch ((u32)(s32)mEvtActNo) {
    case 0: {
        fopAc_ac_c* lift = searchByID(mPartnerId);
        if (lift != nullptr && daLlift_MoveUpLift(lift)) {
            return TRUE;
        }
        return FALSE;
    }
    case 1:
    case 3:
    case 4:
    case 5:
        return TRUE;
    case 2:
        if (cLib_calcTimer(&mEvtTimer) != 0) {
            return FALSE;
        }
        if ((u32)(s32)mEvtLiftNo < 10) {
            fopAc_ac_c* lift = searchByID(mLiftId[mEvtLiftNo]);
            if (lift != nullptr) {
                gabi::store<u8>(actorEa(lift) + 0x403, 1);
            }
        }
        return TRUE;
    default:
        return FALSE;
    }
}
VERIFY(0x0222BEB0, &daNpc_De1_c::event_action);

/* 0222BF7C */
void daNpc_De1_c::privateCut() {
    WWHD_FUNC(0x0222BF7C, void, this);
    s32 staffId = dComIfGp_evmng_getMyStaffId(STR(0x10019898) /* "De1" */, nullptr, 0);
    if (staffId == -1) {
        return;
    }
    s8 actIdx = dComIfGp_evmng_getMyActIdx(staffId, 0x101BE040 /* cut names */, 1, TRUE, 0);
    mActIdx = actIdx;
    dEvent_manager_c* evm = dComIfGp_getPEvtManager();
    if (actIdx == -1) {
        gabi::call(0x02543280, evm, staffId); /* cutEnd */
        return;
    }
    if (gabi::call<BOOL>(0x025447C8, evm, staffId) /* getIsAddvance */) {
        if (mActIdx == 0) {
            event_actionInit(staffId);
        }
    }
    if (mActIdx == 0 && !event_action()) {
        return;
    }
    dComIfGp_evmng_cutEnd(staffId);
}
VERIFY(0x0222BF7C, &daNpc_De1_c::privateCut);

/* 0222C06C */
void daNpc_De1_c::event_proc() {
    WWHD_FUNC(0x0222C06C, void, this);
    if (dComIfGp_evmng_endCheckOld(STR(0x100198A4) /* "LIFT_UP" */)) {
        endEvent();
        mOrder = 1;
        setStt(3);
        return;
    }
    if (dComIfGp_evmng_endCheckOld(STR(0x100198B4) /* "DE_CHUCHU" */)) {
        dComIfGs_onTmpBit(0x308);
        endEvent();
        setStt(7);
        return;
    }
    if (dComIfGp_evmng_endCheckOld(STR(0x100198AC) /* "contact" */)) {
        endEvent();
        gabi::Local<ProcFunc_l> pmf;
        gabi::store<u32>(gabi::ea(pmf.get()), gabi::load<u32>(PMF_wait_action2));
        gabi::store<u32>(gabi::ea(pmf.get()) + 4, gabi::load<u32>(PMF_wait_action2 + 4));
        set_action(pmf.get(), nullptr);
        return;
    }
    if (!mEventCut2.cutProc()) {
        privateCut();
    }
}
VERIFY(0x0222C06C, &daNpc_De1_c::event_proc);

/* 0222C1AC */
void daNpc_De1_c::eventOrder() {
    WWHD_FUNC(0x0222C1AC, void, this);
    s8 order = mOrder;
    if (order == 1 || order == 2) {
        eventInfo_onCondition(this, 1 /* dEvtCnd_CANTALK_e */);
        if (mOrder == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (order >= 3) {
        /* event names (.data 0x101BE038) */
        u32 name = gabi::load<u32>(0x101BE038 + order * 4);
        if (order == 3) {
            fopAcM_orderOtherEvent2(this, name, 1, 0xF);
        } else {
            fopAcM_orderOtherEvent2(this, name, 1, 0xFFFF);
        }
    }
}
VERIFY(0x0222C1AC, &daNpc_De1_c::eventOrder);

/* 0222C218 */
BOOL daNpc_De1_c::_execute() {
    WWHD_FUNC(0x0222C218, BOOL, this);
    if (mInitDone == 0) {
        mInitAngle.y = current.angle.y;
        mInitAngle.z = current.angle.z;
        copy_f32_bits(gabi::ea(&mInitPos.y), gabi::ea(&current.pos.y));
        copy_f32_bits(gabi::ea(&mInitPos.x), gabi::ea(&current.pos.x));
        copy_f32_bits(gabi::ea(&mInitPos.z), gabi::ea(&current.pos.z));
        mInitDone = 1;
        mInitAngle.x = current.angle.x;
    }
    checkOrder();
    if (!demo()) {
        if (dComIfGp_event_runCheck() != 0 && gabi::load<u16>(gabi::ea(this) + 0xF8) != 1) {
            event_proc();
        } else {
            pmf_call(this, &mAction, nullptr);
        }
        fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
        shape_angle.x = current.angle.x;
        shape_angle.y = current.angle.y;
        shape_angle.z = current.angle.z;
    }
    eventOrder();
    setMtx();
    return TRUE;
}
VERIFY(0x0222C218, &daNpc_De1_c::_execute);

/* 0222C368 */
static BOOL daNpc_De1_Execute(daNpc_De1_c* i_this) {
    WWHD_FUNC(0x0222C368, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x0222C368, daNpc_De1_Execute);

/* 0222C36C */
BOOL daNpc_De1_c::_draw() {
    WWHD_FUNC(0x0222C36C, BOOL, this);
    J3DModel* model = mpMorf->getModel();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    dComIfGd_setListBG();
    mpMorf->entryDL();
    dComIfGd_setList();
    /* HD: no shadow */
    dSnap_RegistFig(0xA6 /* DSNAP_TYPE_NPC_DE1 */, this, 1.0f, 1.0f, 1.0f);
    return TRUE;
}
VERIFY(0x0222C36C, &daNpc_De1_c::_draw);

/* 0222C424 */
static BOOL daNpc_De1_Draw(daNpc_De1_c* i_this) {
    WWHD_FUNC(0x0222C424, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x0222C424, daNpc_De1_Draw);

/* 0222C428 */
static BOOL daNpc_De1_IsDelete(daNpc_De1_c*) {
    WWHD_FUNC(0x0222C428, BOOL, (daNpc_De1_c*)nullptr);
    return TRUE;
}
VERIFY(0x0222C428, daNpc_De1_IsDelete);

/* 0222C430 (a_atr_anm_tbl, .data 0x101BE050) */
BOOL daNpc_De1_c::setAnm_ATR() {
    WWHD_FUNC(0x0222C430, BOOL, this);
    return setAnm_anm(gabi::at<anm_prm_c>(0x101BE050 + mAnmAtr * 0x10));
}
VERIFY(0x0222C430, &daNpc_De1_c::setAnm_ATR);

/* 0222C448 */
void daNpc_De1_c::chngAnmAtr(u32 i_atr) {
    WWHD_FUNC(0x0222C448, void, this, i_atr);
    if (i_atr >= 4 || i_atr == mAnmAtr) {
        return;
    }
    mAnmAtr = (u8)i_atr;
    setAnm_ATR();
}
VERIFY(0x0222C448, &daNpc_De1_c::chngAnmAtr);

/* 0222C464 */
void daNpc_De1_c::ctrlAnmAtr() {
    WWHD_FUNC(0x0222C464, void, this);
    if (mAnmAtr != 3 || mAnmEnd == 0) {
        return;
    }
    mAnmAtr = 0;
    setAnm_ATR();
}
VERIFY(0x0222C464, &daNpc_De1_c::ctrlAnmAtr);

/* 0222C488 */
void daNpc_De1_c::anmAtr(u32 i_msgStatus) {
    WWHD_FUNC(0x0222C488, void, this, i_msgStatus);
    if (i_msgStatus == 6) {
        if (mMsgAtrInit == 0) {
            mMsgAnmAtr = 0xFF;
            chngAnmAtr(dComIfGp_getMesgAnimeAttrInfo());
            mMsgAtrInit = (s8)(mMsgAtrInit + 1);
        }
        u8 atr = gabi::load<u8>(dComIfGp_ea() + 0x5BC6); /* dComIfGp_getMesgCameraAttrInfo (probably) */
        if (atr != 0xFF && atr != mMsgAnmAtr) {
            gabi::store<u8>(dComIfGp_ea() + 0x5BC6, 0xFF);
            mMsgAnmAtr = atr;
        }
    } else if (i_msgStatus == 0xE) {
        mMsgAtrInit = 0;
    }
    ctrlAnmAtr();
}
VERIFY(0x0222C488, &daNpc_De1_c::anmAtr);

/* 0222C544 */
u16 daNpc_De1_c::next_msgStatus(be<u32>* pMsgNo) {
    WWHD_FUNC(0x0222C544, u16, this, pMsgNo);
    u16 status = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    switch (*pMsgNo) {
    case 0x139D: *pMsgNo = 0x139E; break;
    case 0x139F: *pMsgNo = 0x13A0; break;
    case 0x13A1: *pMsgNo = 0x13A2; break;
    case 0x13A3: *pMsgNo = 0x13A4; break;
    case 0x13A4: *pMsgNo = 0x13A5; break;
    case 0x13A5: *pMsgNo = 0x13A6; break;
    case 0x13A8:
    case 0x13AC:
        *pMsgNo = 0x13A9;
        break;
    case 0x13A9:
    case 0x13C5:
        if (dComIfGs_isEventBit(0x1D40)) {
            *pMsgNo = 0x13AA;
        } else {
            *pMsgNo = 0x13AB;
        }
        break;
    case 0x13AA: {
        u32 sel = fopMsgM_getSelectNum();
        if (sel < 1) {
            *pMsgNo = 0x13AD;
        } else if (sel == 1) {
            if (dComIfGs_isEventBit(0x102)) {
                *pMsgNo = 0x13D2;
            } else {
                *pMsgNo = 0x13C7;
            }
        } else {
            *pMsgNo = 0x13C6;
        }
        break;
    }
    case 0x13AB: {
        u32 sel = fopMsgM_getSelectNum();
        if (sel < 1) {
            *pMsgNo = 0x13AD;
        } else if (sel == 1) {
            *pMsgNo = 0x13CC;
        } else {
            *pMsgNo = 0x13C6;
        }
        break;
    }
    case 0x13AD: {
        u32 sel = fopMsgM_getSelectNum();
        if (sel < 1) {
            if (dComIfGs_isEventBit(0x102)) {
                *pMsgNo = 0x13B0;
            } else {
                *pMsgNo = 0x13AE;
            }
        } else if (sel == 1) {
            *pMsgNo = 0x13B6;
        } else {
            *pMsgNo = 0x13C6;
        }
        break;
    }
    case 0x13AE:
    case 0x13AF:
    case 0x13B0:
    case 0x13C1:
    case 0x13C3:
    case 0x13C4:
        *pMsgNo = 0x13C5;
        break;
    case 0x13B6:
        if (dComIfGs_isEventBit(0x1820)) {
            if (dComIfGs_isStageBossEnemy(6)) {
                *pMsgNo = 0x13C2;
            } else {
                *pMsgNo = 0x13C4;
            }
        } else {
            *pMsgNo = 0x13C0;
        }
        break;
    case 0x13C0: *pMsgNo = 0x13C1; break;
    case 0x13C2: *pMsgNo = 0x13C3; break;
    case 0x13C7: *pMsgNo = 0x13C8; break;
    case 0x13C8: *pMsgNo = 0x13C9; break;
    case 0x13C9: *pMsgNo = 0x13CA; break;
    case 0x13CA:
        *pMsgNo = 0x13CB;
        dComIfGs_onEventBit(0x3940);
        break;
    case 0x13CC: *pMsgNo = 0x13CD; break;
    case 0x13CD: *pMsgNo = 0x13CE; break;
    case 0x13CE: *pMsgNo = 0x13CF; break;
    case 0x13CF: *pMsgNo = 0x13D0; break;
    case 0x13D0: *pMsgNo = 0x13D1; break;
    default:
        status = 0x10; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return status;
}
VERIFY(0x0222C544, &daNpc_De1_c::next_msgStatus);

/* 0222C998 */
u32 daNpc_De1_c::getMsg() {
    WWHD_FUNC(0x0222C998, u32, this);
    switch ((u32)(s32)mType) {
    case 0:
        if (dComIfGs_checkGetItem(0x20)) {
            return dLib_setFirstMsg(0xE40, 0x139F, 0x13A1);
        }
        return dLib_setFirstMsg(0xE80, 0x139C, 0x139D);
    case 1:
        if (dComIfGs_isEventBit(0x1C40)) {
            if (m8BF != 0) {
                return 0x13AC;
            }
            m8BF = 1;
            return 0x13A8;
        }
        return dLib_setFirstMsg(0x1C20, 0x13A3, 0x13A7);
    default:
        return 0;
    }
}
VERIFY(0x0222C998, &daNpc_De1_c::getMsg);

/* 0222CA64: the player is near the partner leaf lift */
u8 daNpc_De1_c::chkAttention() {
    WWHD_FUNC(0x0222CA64, u8, this);
    fopAc_ac_c* lift = searchByID(mPartnerId);
    if (lift == nullptr) {
        return FALSE;
    }
    f32 dist = HIO_attDist();
    if (mAttention != 0) {
        dist = dist + 50.0f;
    }
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<cXyz> diff;
    cXyz_mi(&player->current.pos, diff.get(), &lift->current.pos);
    gabi::Local<cXyz> xz;
    copy_f32_bits(gabi::ea(&xz->x), gabi::ea(&diff->x));
    xz->y = 0.0f;
    copy_f32_bits(gabi::ea(&xz->z), gabi::ea(&diff->z));
    f32 len = std_sqrtf(PSVECSquareMag(xz.get()));
    return !(dist < len);
}
VERIFY(0x0222CA64, &daNpc_De1_c::chkAttention);

/* 0222CB30 */
BOOL daNpc_De1_c::partner_srch() {
    WWHD_FUNC(0x0222CB30, BOOL, this);
    mPartnerId = 0xFFFFFFFF;
    gabi::store<s32>(L_LIFT_NUM, 0);
    for (int i = 0; i < 100; i++) {
        gabi::store<u32>(L_LIFT_ACTOR + 4 * i, 0);
    }
    fpcM_Search(0x0222A8FC /* searchActor_leafLift */, this);
    if (gabi::load<s32>(L_LIFT_NUM) != 0) {
        mPartnerId = fopAcM_GetID(gabi::at<void>(gabi::load<u32>(L_LIFT_ACTOR)));
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x0222CB30, &daNpc_De1_c::partner_srch);

/* 0222CBE0: creates the ten leaf lifts */
void daNpc_De1_c::ccCreate() {
    WWHD_FUNC(0x0222CBE0, void, this);
    /* static const u32 a_prm_tbl[10] (.data 0x100198E0), copied to the stack */
    u32 prm[10];
    for (int i = 0; i < 10; i++) {
        prm[i] = gabi::load<u32>(0x100198E0 + 4 * i);
    }
    for (int i = 0; i < 10; i++) {
        u32 id = fopAcM_create(0xCE /* PROC_Obj_Llift? */, prm[i], &current.pos, fopAcM_GetRoomNo(this), nullptr,
                               nullptr, -1, 0);
        mLiftId[i] = id;
        if (id == 0xFFFFFFFF)
            JUT_ASSERT_fail(STR(0x10019908), 0x39F, STR(0x10019918));
    }
}
VERIFY(0x0222CBE0, &daNpc_De1_c::ccCreate);

/* 0222CC98 */
BOOL daNpc_De1_c::wait01() {
    WWHD_FUNC(0x0222CC98, BOOL, this);
    if (mTalkReq != 0) {
        setStt(2);
        return TRUE;
    }
    mOrder = mAttention != 0 ? 2 : 0;
    return TRUE;
}
VERIFY(0x0222CC98, &daNpc_De1_c::wait01);

/* 0222CCEC */
BOOL daNpc_De1_c::wait02() {
    WWHD_FUNC(0x0222CCEC, BOOL, this);
    if (mTalkReq != 0) {
        setStt(4);
        return TRUE;
    }
    fopAc_ac_c* lift = searchByID(mPartnerId);
    if (lift != nullptr && lift->current.pos.y < lift->home.pos.y + 560.0f) {
        setStt(5);
        return TRUE;
    }
    mOrder = mAttention != 0 ? 2 : 0;
    return TRUE;
}
VERIFY(0x0222CCEC, &daNpc_De1_c::wait02);

/* 0222CD8C */
BOOL daNpc_De1_c::wait03() {
    WWHD_FUNC(0x0222CD8C, BOOL, this);
    fopAc_ac_c* lift = searchByID(mPartnerId);
    if (lift != nullptr && daLlift_021B430C(lift)) {
        setStt(1);
    }
    return TRUE;
}
VERIFY(0x0222CD8C, &daNpc_De1_c::wait03);

/* 0222CDE0 */
BOOL daNpc_De1_c::wait04() {
    WWHD_FUNC(0x0222CDE0, BOOL, this);
    mOrder = 0;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<cXyz> diff;
    cXyz_mi(&player->current.pos, diff.get(), &mDemoStartCenter);
    gabi::Local<cXyz> xz;
    copy_f32_bits(gabi::ea(&xz->x), gabi::ea(&diff->x));
    xz->y = 0.0f;
    copy_f32_bits(gabi::ea(&xz->z), gabi::ea(&diff->z));
    f32 len = std_sqrtf(PSVECSquareMag(xz.get()));
    if (len < HIO_callDist()) {
        fopAc_ac_c* pl = dComIfGp_getPlayer(0);
        mOrder = 4;
        gabi::store<s16>(actorEa(pl) + 0x422, cLib_targetAngleY(&pl->current.pos, &current.pos));
    }
    return TRUE;
}
VERIFY(0x0222CDE0, &daNpc_De1_c::wait04);

/* 0222CE8C */
BOOL daNpc_De1_c::wait05() {
    WWHD_FUNC(0x0222CE8C, BOOL, this);
    if (mOrder == 5) {
        return TRUE;
    }
    mOrder = 0;
    for (int i = 0; i < 10; i++) {
        if (searchByID(mLiftId[i]) != nullptr) {
            return TRUE;
        }
    }
    mOrder = 5;
    gabi::store<u32>(gabi::ea(this) + 0x39C, gabi::load<u32>(gabi::ea(this) + 0x39C) & ~0x4000000u);
    return TRUE;
}
VERIFY(0x0222CE8C, &daNpc_De1_c::wait05);

/* 0222CF18 */
BOOL daNpc_De1_c::talk01() {
    WWHD_FUNC(0x0222CF18, BOOL, this);
    if (mOrder != 3) {
        m8BE = 0xFF;
        m8D0 = 0;
        mTalkReq = 0;
        endEvent();
        mOrder = 3;
    }
    return TRUE;
}
VERIFY(0x0222CF18, &daNpc_De1_c::talk01);

/* 0222CF78 */
BOOL daNpc_De1_c::talk02() {
    WWHD_FUNC(0x0222CF78, BOOL, this);
    talk(1);
    if (mbHasMsg != 0 && fopMsgM_getStatus() == 0x13 /* fopMsgStts_BOX_CLOSED_e */) {
        m8BE = 0xFF;
        setStt((u32)(s32)mPrevStt);
        m8D0 = 0;
        mTalkReq = 0;
        endEvent();
    }
    return TRUE;
}
VERIFY(0x0222CF78, &daNpc_De1_c::talk02);

/* 0222CFF8: before the leaf lifts exist */
BOOL daNpc_De1_c::wait_action1(void*) {
    WWHD_FUNC(0x0222CFF8, BOOL, this, (void*)nullptr);
    if (mActPhase == 0) {
        setStt(6);
        ccCreate();
        mActPhase = (s8)(mActPhase + 1);
        return TRUE;
    }
    if (mActPhase != -1) {
        mAttention = chkAttention();
        switch ((u32)(s32)mStt) {
        case 6:
            wait04();
            break;
        case 7:
            wait05();
            break;
        }
    }
    return TRUE;
}
VERIFY(0x0222CFF8, &daNpc_De1_c::wait_action1);

/* 0222D0A4 */
BOOL daNpc_De1_c::wait_action2(void*) {
    WWHD_FUNC(0x0222D0A4, BOOL, this, (void*)nullptr);
    if (mActPhase == 0) {
        setStt(1);
        mActPhase = (s8)(mActPhase + 1);
        return TRUE;
    }
    if (mActPhase == -1) {
        return TRUE;
    }
    if (mActPhase == 1) {
        partner_srch();
        mActPhase = (s8)(mActPhase + 1);
    }
    mAttention = chkAttention();
    switch ((u32)(s32)mStt) {
    case 1:
        wait01();
        break;
    case 2:
        talk01();
        break;
    case 3:
        wait02();
        break;
    case 4:
        talk02();
        break;
    case 5:
        wait03();
        break;
    }
    return TRUE;
}
VERIFY(0x0222D0A4, &daNpc_De1_c::wait_action2);

/* ---- compiler-generated tail ---- */
/* 0222D1B8 daNpc_De1_HIO_c::daNpc_De1_HIO_c (HD: allocates when this == NULL) */
static u8* daNpc_De1_HIO_c_ct(u8* i_this) {
    WWHD_FUNC(0x0222D1B8, u8*, i_this);
    if (i_this == nullptr) {
        i_this = (u8*)operator_new(0x28);
        if (i_this == nullptr)
            return i_this;
    }
    gabi::store<u32>(gabi::ea(i_this) + 0x24, 0x1001969C); /* vtable */
    memcpy_g(gabi::at<u8>(gabi::ea(i_this) + 8), gabi::at<u8>(0x101BE090), 0x1C); /* parameters */
    gabi::store<s8>(gabi::ea(i_this), -1);     /* mNo */
    gabi::store<s32>(gabi::ea(i_this) + 4, -1); /* mCount */
    return i_this;
}
VERIFY(0x0222D1B8, daNpc_De1_HIO_c_ct);

/* 0222D224: static initialisation of the translation unit */
static void __sinit_d_a_npc_de1_cpp() {
    WWHD_FUNC(0x0222D224, void, (u32)0);
    sinit_header_statics_z(0x10466D0C, 0x101BE0AC, 0x10466D40);
    daNpc_De1_HIO_c_ct(gabi::at<u8>(L_HIO)); /* static daNpc_De1_HIO_c l_HIO */
}
VERIFY(0x0222D224, __sinit_d_a_npc_de1_cpp);

/* 0222D2C4: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x0222D2C4, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x0222D2C4, SafeString_dt);

/* 0222D2D8: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x0222D2D8, void, (SafeString*)nullptr);
}
VERIFY(0x0222D2D8, SafeString_assureTerminationImpl);

/* 0222D2DC: daNpc_De1_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpc_De1_c_dt(daNpc_De1_c* i_this, s32 flags) {
    WWHD_FUNC(0x0222D2DC, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        /* ~dBgS_ObjAcch: this TU's vtables of its sub-objects, then dBgS_Acch::~dBgS_Acch */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x1001967C);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x1001968C);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0222D2DC, daNpc_De1_c_dt);

/* 0222D378: empty virtual (this TU's copy) */
static void de1_empty_0222D378(void*) {
    WWHD_FUNC(0x0222D378, void, (void*)nullptr);
}
VERIFY(0x0222D378, de1_empty_0222D378);
