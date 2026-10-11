/* Spiked Skull Hammer switch. Derived game source;
 */
#include "d/actor/d_a_obj_shmrgrd.h"
#include "gc_api/m_Do/m_Do_mtx.h"
#include "d/d_cc_uty.h"
#include "gc_api/JSystem/JUTAssert.h"
#include "gc_api/d/actor/d_a_player.h"
const char daObjShmrgrd_c::M_arcname[8] = "Shmrgrd";
using ShmrgrdResourceControl = gptr<u8>;
static ShmrgrdResourceControl resourceControl = {};
const gc_tu_data gc_tu = {.safeString = 0x1002F430};
using ShmrgrdPointer = gptr<daObjShmrgrd_c>;
using ShmrgrdMode = gabi::ptmf<void (daObjShmrgrd_c::*)()>;
static ShmrgrdMode mode_proc[3] = {};
static u32 hioStorage[4] = {};
static ShmrgrdRegistration hioRegistration = {};
static ShmrgrdRegistration signedCharRegistration = {};
static ShmrgrdRegistration unsignedCharRegistration = {};
static f32 minPi = {};
static f32 maxPi = {};
static u8 signedCharStorage = {};
static u8 unsignedCharStorage = {};
static cXyz particle_scale = {};
static u32 particleScaleInitialized = {};
static cXyz particleTranslation = {};
static u32 particleTranslationInitialized = {};
static ShmrgrdPointer M_list_p = {};
static s32 M_damage = {};
static s16 M_damage_dir = {};
static cXyz zeroVector = {};
static cXyz baseZ = {};
static ShmrgrdSrcCyl M_cyl_src_co = {};
static ShmrgrdSrcCyl M_cyl_src_tg = {};
static ShmrgrdSrcCyl M_cyl_src_at = {};
static ShmrgrdSrcCyl M_cyl_src_gap_co = {};
s32 prm(daObjShmrgrd_c *, s32, s32);
void daObjShmrgrd_get_at_v(cXyz *, cXyz *);
daObjShmrgrd_c *actor_constructor(daObjShmrgrd_c *);
BOOL solidHeapCB(fopAc_ac_c *);
s32 jnodeCB(void *, s32);

/* inlined into mode_upper and mode_u_l_init */
static void sound(daObjShmrgrd_c *a, u32 id) {
    s32 reverb = dComIfGp_getReverb(a->current.roomNo);
    shmrgrdSound(id, &a->eyePos, 0, reverb);
}
/* 0238E8DC */
daObjShmrgrd_c *actor_constructor(daObjShmrgrd_c *a) {

    if (!a) {
        a = shmrgrdAllocate(0x9F0);
        if (!a)
            return nullptr;
    }
    fopAc_ac_c_ct(a);
    a->__vtbl = ShmrgrdActorVtable;
    dCcD_Cyl *cylinders[4] = {&a->mCylCo, &a->mCylTg, &a->mCylAt, &a->mCylGapCo};
    dCcD_Stts *statuses[4] = {&a->mSttsCo, &a->mSttsTg, &a->mSttsAt, &a->mSttsGapCo};
    for (u32 i = 0; i < 4; i++) {
        dCcD_Cyl *cyl = cylinders[i];
        ShmrgrdShapeView *shape = reinterpret_cast<ShmrgrdShapeView *>(cyl->mShapeAttr);
        shmrgrdConstructGObj(cyl);
        shape->mShapeVtable = ShmrgrdShapeVtable;
        shape->mAabVtable = ShmrgrdAabVtable;
        shmrgrdConstructCyl(&cyl->mCyl);
        // HD: GHS emitted different vtable store orders for the middle colliders.
        if (i == 1 || i == 2) {
            cyl->mCyl.__vtbl = ShmrgrdGeometryVtable;
            shape->mShapeVtable = ShmrgrdCylinderShapeVtable;
            cyl->__vtbl_hitinf = ShmrgrdHitVtable;
        } else {
            shape->mShapeVtable = ShmrgrdCylinderShapeVtable;
            cyl->__vtbl_hitinf = ShmrgrdHitVtable;
            cyl->mCyl.__vtbl = ShmrgrdGeometryVtable;
        }
        dCcD_Stts_ct(statuses[i]);
    }
    shmrgrdConstructSmoke(&a->mSmokeCb, 1);
    a->mSmokeCb.mColor.r = 0xA0;
    a->mSmokeCb.mColor.b = 0x80;
    a->mSmokeCb.mColor.g = 0xA0;
    a->mpNext = nullptr;
    a->mSmokeCb.mColor.a = 0xFF;
    return a;
}

/* 0238ED74 */
BOOL solidHeapCB(fopAc_ac_c *i_this) {

    return static_cast<daObjShmrgrd_c *>(i_this)->create_heap();
}

/* 0238EC78 */
s32 daObjShmrgrd_c::create_heap() {

    // HD: both SafeString slots exist before the first resource request.
    gabi::Local<be<u32>[2]> request1, request2;
    (*request1)[0] = gabi::guest_addr(M_arcname);
    (*request1)[1] = gc_tu.safeString;
    J3DModelData *data = static_cast<J3DModelData *>(
        shmrgrdGetResource(resourceControl, &(*request1)[0], dRes_INDEX_SHMRGRD_BDL_SHMRGRD_e));
    if (!data) {
        JUTAssertion::showAssert("d_a_obj_shmrgrd.cpp", 0x21A, "mdl_data != 0");
        return 0;
    }
    J3DModel *model = mDoExt_J3DModel__create(data, 0, 0x11020203);
    mpModel = model;
    if (!model)
        return 0;
    data->getJointNodePointer(SHMRGRD_JNT_HIT_e)->mCallBack = gabi::guest_fn((const void *)jnodeCB);
    model = mpModel;
    model->setUserArea(gabi::ea(this));
    (*request2)[0] = gabi::guest_addr(M_arcname);
    (*request2)[1] = gc_tu.safeString;
    cBgD_t *bg = static_cast<cBgD_t *>(
        shmrgrdGetResource(resourceControl, &(*request2)[0], dRes_INDEX_SHMRGRD_DZB_HGBASE_e));
    dBgW *world = shmrgrdBgNewSet(bg, dBgW::MOVE_BG_e, &mMtx);
    mpBgW = world;
    return world != nullptr;
}

/* 0238EF8C */
s32 daObjShmrgrd_c::_create() {

    u32 condition = actor_condition;
    if (!(condition & ShmrgrdInitialized)) {
        if (gabi::ea(this)) {
            actor_constructor(this);
            condition = actor_condition;
        }
        actor_condition = condition | ShmrgrdInitialized;
    }
    s32 phase = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void *)solidHeapCB), 0x12A0))
        return cPhs_ERROR_e;
    J3DModel *model = mpModel;
    cullMtx = model ? gabi::ea(&model->mBaseTransformMtx) : 0;
    init_mtx();
    fopAcM_setCullSizeBox(this, -90.0f, 0.0f, -90.0f, 90.0f, 120.0f, 90.0f);
    mSttsCo.Init(255, 255, this);
    mCylCo.Set(reinterpret_cast<dCcD_SrcCyl *>(&M_cyl_src_co));
    mCylCo.mStts = &mSttsCo;
    mSttsTg.Init(255, 255, this);
    mCylTg.Set(reinterpret_cast<dCcD_SrcCyl *>(&M_cyl_src_tg));
    u32 flags = mCylTg.mGObjTg.mSPrm;
    mCylTg.mStts = &mSttsTg;
    mCylTg.mGObjTg.mSPrm = flags | 4;
    mSttsAt.Init(255, 255, this);
    mCylAt.Set(reinterpret_cast<dCcD_SrcCyl *>(&M_cyl_src_at));
    mCylAt.mStts = &mSttsAt;
    mSttsGapCo.Init(255, 255, this);
    mCylGapCo.Set(reinterpret_cast<dCcD_SrcCyl *>(&M_cyl_src_gap_co));
    flags = (u32)attention_info.flags;
    mVSpeed = 0;
    mAngleZ = 0;
    mAngleX = 0;
    mScaleY = 1;
    m85C = 0;
    mCrushState = 0;
    mAngleSpeedX = 0;
    mAngleSpeedZ = 0;
    attention_info.distances[fopAc_Attn_TYPE_MISC_e] = 0x2D;
    mCylGapCo.mStts = &mSttsGapCo;
    attention_info.flags = flags | fopAc_Attn_LOCKON_MISC_e;
    mCrushTimer = 0;
    mTargetHFrac = 0;
    s32 sw = prm(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
    bool set = false;
    if (sw != 255) {
        s8 room = home.roomNo;
        dSv_info_c *save = gabi::at<dSv_info_c>(dComIfGs_ea() + 0x20);
        set = shmrgrdIsSwitch(save, sw, room) != 0;
    }
    if (set) {
        mCurHFrac = 0;
        mTopPos = -85;
        mode_lower_init();
    } else {
        mCurHFrac = 1;
        mTopPos = 0;
        mode_upper_init();
    }
    register_list();
    model = mpModel;
    MTXCopy(model->getBaseTRMtx(), &mMtx);
    dBgS *bgsp = dComIfG_Bgsp();
    bgsp->Regist(mpBgW, this);
    return cPhs_COMPLEATE_e;
}

/* 0238F2CC */
bool daObjShmrgrd_c::_delete() {

    if (heap) {
        auto bg = (dBgW *)mpBgW;
        if (bg && bg->ChkUsed()) {
            dBgS *bgsp = dComIfG_Bgsp();
            bgsp->Release(mpBgW);
            mpBgW = nullptr;
        }
    }
    leave_list();
    mSmokeCb.remove();
    dComIfG_resDelete(&mPhs, M_arcname);
    return true;
}

/* 0238EEF4 */
daObjShmrgrd_c *daObjShmrgrd_c::search_target_next(daObjShmrgrd_c *target) {

    daObjShmrgrd_c *cur = M_list_p;
    while (cur) {
        auto next = (daObjShmrgrd_c *)cur->mpNext;
        if (next == target)
            return cur;
        cur = next;
    }
    return nullptr;
}

/* 0238EF28 */
void daObjShmrgrd_c::register_list() {

    mpNext = nullptr;
    if (M_list_p == nullptr)
        M_list_p = this;
    else {
        auto last = search_target_next(nullptr);
        if (last)
            last->mpNext = this;
    }
}

/* 0238F254 */
void daObjShmrgrd_c::leave_list() {

    auto prev = search_target_next(this);
    auto next = (daObjShmrgrd_c *)mpNext;
    if (prev)
        prev->mpNext = next;
    else if (!next)
        M_list_p = nullptr;
    else if (this == M_list_p)
        M_list_p = next;
}

/* 0238F7F0 */
daObjShmrgrd_c *daObjShmrgrd_c::search_gap() {

    daObjShmrgrd_c *node = M_list_p;
    gabi::Local<cXyz> delta, flat, vec;
    while (node) {
        shmrgrdSubtract(&node->current.pos, delta.get(), &current.pos);
        s32 mode = node->mMode;
        f32 x = delta->x, z = delta->z;
        if (mode == 0) {
            flat->x = x;
            flat->y = 0;
            flat->z = z;
            f32 square = PSVECSquareMag(flat.get());
            f32 mag = shmrgrdSqrt(square);
            // HD: unordered magnitudes take the near-node path.
            if (!(mag > 165)) {
                mDoMtx_stack_c::YrotS(shape_angle.y);
                PSMTXMultVec(mDoMtx_stack_c::get(), &baseZ, vec.get());
                f32 dot = gabi::fmadds((f32)vec->x, x, (f32)vec->z * z);
                if (dot > 0)
                    return node;
            }
        }
        node = (daObjShmrgrd_c *)node->mpNext;
    }
    return nullptr;
}

/* 0238E84C */
void daObjShmrgrd_get_at_v(cXyz *out, cXyz *other) {

    fopAc_ac_c *player = dComIfGp_getPlayer(0);
    gabi::Local<cXyz> dir;
    shmrgrdSubtract(&player->current.pos, dir.get(), other);
    out->x = (f32)zeroVector.x;
    out->y = (f32)zeroVector.y;
    out->z = (f32)zeroVector.z;
    if (shmrgrdNormalizeRS(dir.get()) != 0) {
        f32 y = dir->y, x = dir->x, z = dir->z;
        out->x = x;
        out->z = z;
        out->y = y;
    }
}

/* 0238F928 */
void daObjShmrgrd_c::set_gap_co() {

    auto gap = search_gap();
    if (!gap)
        return;
    gabi::Local<cXyz> delta, pos, at;
    shmrgrdSubtract(&gap->current.pos, delta.get(), &current.pos);
    pos->y = delta->y;
    pos->x = delta->x;
    pos->z = delta->z;
    PSVECScale(pos.get(), pos.get(), 0.5f);
    PSVECAdd(pos.get(), &current.pos, pos.get());
    mCylGapCo.mCyl.SetC(pos.get());
    daObjShmrgrd_get_at_v(at.get(), pos.get());
    f32 y = at->y, x = at->x;
    mCylGapCo.mGObjAt.mVec.y = y;
    f32 z = at->z;
    mCylGapCo.mGObjAt.mVec.x = x;
    mCylGapCo.mGObjAt.mVec.z = z;
    shmrgrdRegisterCollision(dComIfG_Ccsp(), &mCylGapCo);
}

/* 0238EE3C */
void daObjShmrgrd_c::init_mtx() {

    J3DModel *model = mpModel;
    f32 x = scale.x, y = scale.y, z = scale.z;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    set_mtx();
}

/* 0238ED78 */
void daObjShmrgrd_c::set_mtx() {

    auto matrix = mDoMtx_stack_c::get();
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    mDoMtx_stack_c::transS(x, y, z);
    shmrgrdZXYrotM(matrix, (s16)shape_angle.x, (s16)shape_angle.y, (s16)shape_angle.z);
    f32 values[12];
    for (u32 i = 0; i < 12; i++)
        values[i] = reinterpret_cast<be<f32> *>(mDoMtx_stack_c::get())[i];
    J3DModel *model = mpModel;
    for (u32 i = 0; i < 12; i++)
        reinterpret_cast<be<f32> *>(&model->mBaseTransformMtx)[i] = values[i];
}

/* 0238F360 */
s32 daObjShmrgrd_c::check_player_angle(fopAc_ac_c *actor) {

    s32 angle = shmrgrdTargetAngle(&actor->current.pos, &current.pos);
    s16 delta = (s16)((s16)actor->shape_angle.y - angle);
    return (u32)((s32)delta + 0x1FFF) < 0x3FFF;
}

/* 0238F3B4 */
void daObjShmrgrd_c::set_damage() {

    daPy_py_c *player = static_cast<daPy_py_c *>(dComIfGp_getPlayer(0));
    // HD: use endian storage for the raw byte read, including sparse guest pointers.
    u8 cut = reinterpret_cast<ShmrgrdPlayerView *>(player)->mCutType;
    M_damage = 0;
    M_damage_dir = 0;
    mSttsTg.Move();
    if (!mCylTg.ChkTgHit())
        return;
    void *hit = mCylTg.GetTgHitObj();
    auto actor = mCylTg.GetTgHitAc();
    if (cCcD_Obj_ChkAtType(hit, cc_uty::AT_TYPE_SKULL_HAMMER) && actor &&
        (s16)actor->mProfName == cc_uty::fpcNm_PLAYER_e) {
        s32 front = check_player_angle(actor);
        if (front && (cut == ShmrgrdCut::CUT_TYPE_HAMMER_FRONTSWING ||
                      cut == ShmrgrdCut::CUT_TYPE_JUMPCUT_HAMMER))
            M_damage = 1;
        else if (cut == ShmrgrdCut::CUT_TYPE_HAMMER_SIDESWING) {
            f32 x = (f32)mCylTg.mGObjTg.mRVec.x, z = (f32)mCylTg.mGObjTg.mRVec.z;
            s16 direction = cM_atan2s(x, z);
            M_damage_dir = direction;
            M_damage = 2;
        }
    }
    shmrgrdHitSeStart(&eyePos, (s8)current.roomNo, &mCylTg, 0x11);
    shmrgrdHitEffHibana(this, &mCylTg);
    mCylTg.ClrTgHit();
}

/* 0238FC50 */
void daObjShmrgrd_c::vib_start(s16 dir, f32 mag) {

    // HD: retain separate table reads around the first output store (alias witnesses).
    mAngleSpeedZ = cM_scos(dir) * mag;
    mAngleSpeedX = cM_ssin(dir) * mag;
}

/* 0238F734 */
void daObjShmrgrd_c::vib_proc() {

    f32 z = mAngleZ, x = mAngleX, sz = mAngleSpeedZ, sx = mAngleSpeedX;
    f32 nz = sz + gabi::fnmsubs(sz, 0.18f, -(z * 0.99f)),
        nx = sx + gabi::fnmsubs(sx, 0.18f, -(x * 0.99f));
    mAngleSpeedZ = nz;
    mAngleSpeedX = nx;
    mAngleZ = z + nz;
    mAngleX = x + nx;
}

/* 0238F4F0 */
void daObjShmrgrd_c::crush_start() { mCrushState = 1; }

/* 0238F790 */
void daObjShmrgrd_c::crush_proc() {

    if (mCrushState == 1) {
        mCrushTimer = 18;
        mCrushState = 2;
        mScaleY = 0.2f;
    } else if (mCrushState == 2) {
        s16 timer = (s16)((s16)mCrushTimer - 1);
        mCrushTimer = timer;
        if (timer > 0)
            return;
        mCrushState = 0;
        mScaleY = 1;
    }
}

/* 0238F4FC */
void daObjShmrgrd_c::eff_crush() {

    if (mMode == MODE_UPPER) {
        dPa_control_c *manager = dComIfGp_getParticle();
        manager->set(0, 0x81B7, &current.pos, 0, 0, 255, 0, -1, 0, 0, 0);
        manager = dComIfGp_getParticle();
        manager->set(0, 0x81B8, &current.pos, 0, 0, 255, 0, -1, 0, 0, 0);
    }
    if (!particleScaleInitialized) {
        particle_scale.x = 1.5f;
        particle_scale.z = 1;
        particle_scale.y = 1.5f;
        particleScaleInitialized = 1;
    }
    dPa_control_c *manager = dComIfGp_getParticle();
    JPABaseEmitter *emitter =
        manager->set(2, 0x2027, &current.pos, 0, 0, 200,
                     reinterpret_cast<dPa_levelEcallBack *>(&mSmokeCb), -1, 0, 0, &particle_scale);
    if (!emitter)
        return;
    ShmrgrdEmitterView *view = reinterpret_cast<ShmrgrdEmitterView *>(emitter);
    view->mMaxFrame = 1;
    view->mLifeTime = 45;
    view->mRate = 30;
    view->mInitialVelOmni = 0;
    view->mInitialVelAxis = 38;
    if (!particleTranslationInitialized) {
        particleTranslationInitialized = 1;
        particleTranslation.z = 0;
        particleTranslation.y = 15;
        particleTranslation.x = 0;
    }
    view->mLocalTranslation.x = (f32)particleTranslation.x;
    view->mLocalTranslation.y = (f32)particleTranslation.y;
    view->mLocalTranslation.z = (f32)particleTranslation.z;
}

/* 0238F6D8 */
void daObjShmrgrd_c::calc_top_pos() {

    f32 target = mTargetHFrac, cur = mCurHFrac, speed = mVSpeed;
    f32 next = gabi::fnmsubs(cur - target, 0.8f, speed);
    next = gabi::fnmsubs(next, 0.55f, next);
    f32 height = cur + next;
    if (height - 1.0f >= 0)
        height = 1;
    mVSpeed = next;
    mCurHFrac = height;
    mTopPos = (1.0f - height) * -85.0f;
}

/* 0238EA88 */
s32 jnodeCB(void *joint, s32 phase) {

    if (phase != 0)
        return 1;
    J3DModel *model = j3dSys.getModel();
    daObjShmrgrd_c *actor = static_cast<daObjShmrgrd_c *>(model->getUserArea());
    J3DJoint *node = J3DNode_toJoint(static_cast<J3DNode *>(joint));
    u16 index = reinterpret_cast<ShmrgrdJointView *>(node)->mJntNo;
    J3DMtxBuffer_hd *matrices = model->mpMtxBuffer;
    Mtx34 *base = gabi::at<Mtx34>(matrices->mpAnmMtx);
    u16 flags = matrices->mFlags;
    matrices->mFlags = flags | 0x10;
    MTXCopy(&base[index], mDoMtx_stack_c::get());
    f32 scale = actor->mScaleY;
    mDoMtx_stack_c::transM(0, 20.0f * (1.0f - scale), 0);
    mDoMtx_stack_c::scaleM(1, actor->mScaleY, 1);
    mDoMtx_stack_c::transM(0, actor->mTopPos, 0);
    gabi::Local<cXyz> axis;
    gabi::Local<Quaternion> quat;
    axis->y = 1;
    axis->x = actor->mAngleX;
    axis->z = actor->mAngleZ;
    shmrgrdQuatRotBaseY2(quat.get(), axis.get());
    mDoMtx_stack_c::transM(0, -20, 0);
    mDoMtx_stack_c::quatM(quat.get());
    mDoMtx_stack_c::transM(0, 20, 0);
    matrices = model->mpMtxBuffer;
    flags = matrices->mFlags;
    base = gabi::at<Mtx34>(matrices->mpAnmMtx);
    matrices->mFlags = flags | 0x10;
    f32 values[12];
    for (u32 i = 0; i < 12; i++)
        values[i] = reinterpret_cast<be<f32> *>(mDoMtx_stack_c::get())[i];
    for (u32 i = 0; i < 12; i++)
        reinterpret_cast<be<f32> *>(&base[index])[i] = values[i];
    MTXCopy(mDoMtx_stack_c::get(), J3DSys::mCurrentMtx);
    return 1;
}

/* 0238EE80 */
void daObjShmrgrd_c::mode_upper_init() {

    mCylTg.mCyl.SetH(112.5f);
    mCylCo.mCyl.SetH(112.5f);
    mMode = MODE_UPPER;
    mTargetHFrac = 1;
}

/* 0238FCE0 */
void daObjShmrgrd_c::mode_upper() {

    s32 damage = (s32)M_damage;
    if (damage == 1) {
        mode_u_l_init();
        return;
    }
    s32 index = prm(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
    if (index != 255) {
        s8 room = home.roomNo;
        dSv_info_c *save = gabi::at<dSv_info_c>(dComIfGs_ea() + 0x20);
        if (shmrgrdIsSwitch(save, index, room) != 0) {
            mode_u_l_init();
            return;
        }
        damage = (s32)M_damage;
    }
    if (damage == 2) {
        s16 dir = (s16)M_damage_dir;
        vib_start(dir, 1.0f);
        sound(this, JA_SE_OBJ_HAMMER_SW_SWING);
    }
}

/* 0238FC84 */
void daObjShmrgrd_c::mode_u_l_init() {

    sound(this, JA_SE_OBJ_HAMMER_SW_ON);
    mMode = MODE_U_L;
    mTargetHFrac = 0;
}

/* 0238FDDC */
void daObjShmrgrd_c::mode_u_l() {
    // HD: unordered heights also complete the descent.

    if ((f32)mCurHFrac > 0)
        return;
    s32 index = prm(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
    if (index != 255) {
        s8 room = home.roomNo;
        dSv_info_c *save = gabi::at<dSv_info_c>(dComIfGs_ea() + 0x20);
        shmrgrdOnSwitch(save, index, room);
    }
    mode_lower_init();
}

/* 0238EE5C */
void daObjShmrgrd_c::mode_lower_init() {

    u32 flags = (u32)attention_info.flags;
    mMode = MODE_LOWER;
    attention_info.flags = flags & ~fopAc_Attn_LOCKON_MISC_e;
    mTargetHFrac = 0;
}

/* 0238FFF8 */
void daObjShmrgrd_c::mode_lower() {}

/* 0238F9F4 */
s32 daObjShmrgrd_c::_execute() {

    set_damage();
    if ((s32)M_damage == 1) {
        crush_start();
        eff_crush();
    }
    (this->*mode_proc[(u32)mMode])();
    calc_top_pos();
    vib_proc();
    crush_proc();
    set_mtx();
    if (mMode == MODE_UPPER) {
        mCylTg.mCyl.SetC(&current.pos);
        shmrgrdRegisterCollision(dComIfG_Ccsp(), &mCylTg);
        mCylCo.mCyl.SetC(&current.pos);
        shmrgrdRegisterCollision(dComIfG_Ccsp(), &mCylCo);
        gabi::Local<cXyz> at;
        daObjShmrgrd_get_at_v(at.get(), &current.pos);
        f32 y = at->y, z = at->z, x = at->x;
        mCylAt.mGObjAt.mVec.y = y;
        mCylAt.mGObjAt.mVec.z = z;
        mCylAt.mGObjAt.mVec.x = x;
        mCylAt.mCyl.SetC(&current.pos);
        shmrgrdRegisterCollision(dComIfG_Ccsp(), &mCylAt);
        set_gap_co();
    }
    f32 height = (f32)current.pos.y + (f32)mTopPos;
    attention_info.position.y = height + 125;
    eyePos.y = height + 75;
    if (heap) {
        auto bg = (dBgW *)mpBgW;
        if (bg && bg->ChkUsed())
            bg->Move();
    }
    return 1;
}

/* 0238FBB8 */
s32 daObjShmrgrd_c::_draw() {

    settingTevStruct(dKy_getEnvlight(), 0, &current.pos, &tevStr);
    dScnKy_env_light_c *light = dKy_getEnvlight();
    setLightTevColorType(light, mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel, 0);
    dComIfGd_setList();
    return 1;
}

/* 0238FE54 */
s32 create_method(daObjShmrgrd_c *a) { return a->_create(); }

/* 0238FE58 */
bool delete_method(daObjShmrgrd_c *a) { return a->_delete(); }

/* 0238FE5C */
s32 execute_method(daObjShmrgrd_c *a) { return a->_execute(); }

/* 0238FE60 */
s32 draw_method(daObjShmrgrd_c *a) { return a->_draw(); }

/* 02390000 */
s32 is_delete(daObjShmrgrd_c *a) { return 1; }

// HD: compiler-generated and template code of this translation unit.
/* 0238FE64 */
void initialize_globals() {

    hioStorage[3] = 0;
    hioStorage[2] = 0;
    hioStorage[1] = 0;
    hioStorage[0] = 0;
    shmrgrdRegisterDestructor(&hioRegistration);
    minPi = -3.1415927410125732f;
    maxPi = 3.1415927410125732f;
    shmrgrdConstructSignedChar(&signedCharStorage);
    shmrgrdRegisterDestructor(&signedCharRegistration);
    shmrgrdConstructUnsignedChar(&unsignedCharStorage);
    shmrgrdRegisterDestructor(&unsignedCharRegistration);
    M_cyl_src_tg.mCyl.mRadius = 50;
    M_cyl_src_tg.mCyl.mHeight = 112.5f;
    M_cyl_src_co.mCyl.mRadius = 79;
    M_cyl_src_co.mCyl.mHeight = 112.5f;
}

/* 0238FF30 */
void global_destructor(void *a, s32 flags) {

    if (a && ((u32)flags & 1))
        shmrgrdDelete(a);
}

/* 0238FF44 */
void actor_destructor(daObjShmrgrd_c *a, s32 flags) {

    if (!a)
        return;
    dCcD_Stts_dt(&a->mSttsGapCo, 2);
    dCcD_Cyl_dt(&a->mCylGapCo, 2);
    dCcD_Stts_dt(&a->mSttsAt, 2);
    dCcD_Cyl_dt(&a->mCylAt, 2);
    dCcD_Stts_dt(&a->mSttsTg, 2);
    dCcD_Cyl_dt(&a->mCylTg, 2);
    dCcD_Stts_dt(&a->mSttsCo, 2);
    dCcD_Cyl_dt(&a->mCylCo, 2);
    shmrgrdDestroyActor(a, 0);
    if ((u32)flags & 1)
        shmrgrdDelete(a);
}

/* 0238FFFC */
void empty_callback(void *a) {}

/* 02390008 */
s32 prm(daObjShmrgrd_c *a, s32 width, s32 shift) {

    u32 w = (u32)width & 63, sh = (u32)shift & 63;
    u32 mask = (w < 32 ? 1u << w : 0) - 1;
    return (sh < 32 ? (u32)a->mParameters >> sh : 0) & mask;
}
/* WWHD glue (tools/verify/autoglue.py)
data M_list_p 101CCB54 nocheck
data M_damage 101CCB58 nocheck
data M_damage_dir 101CCB5C nocheck
data zeroVector 101FFBA8 nocheck
data baseZ 101FFBCC nocheck
data M_cyl_src_co 101CCBB0 nocheck
data M_cyl_src_tg 101CCBF4 nocheck
data M_cyl_src_at 1002F518 nocheck
data M_cyl_src_gap_co 1002F55C nocheck
ext 0200F93C s32 shmrgrdTargetAngle(cXyz*, cXyz*)
ext 025BA0C0 s32 shmrgrdIsSwitch(dSv_info_c*, s32, s32)
ext 025B9E38 void shmrgrdOnSwitch(dSv_info_c*, s32, s32)
ext 0273AF40 void shmrgrdDelete(void*)
ext 025D50BC void shmrgrdDestroyActor(fopAc_ac_c*, s32)
ext 0201ADE0 void shmrgrdSubtract(const cXyz*, cXyz*, const cXyz*)
ext 0201B47C s32 shmrgrdNormalizeRS(cXyz*)
ext 028F4384 f32 shmrgrdSqrt(f32)
ext 023129C4 void shmrgrdHitSeStart(cXyz*, s32, dCcD_Cyl*, s32)
ext 02312E54 void shmrgrdHitEffHibana(fopAc_ac_c*, dCcD_Cyl*)
ext 0273AD10 daObjShmrgrd_c* shmrgrdAllocate(u32)
ext 025A5B18 void shmrgrdConstructSmoke(dPa_smokeEcallBack*, s32)
ext 024F2478 dBgW* shmrgrdBgNewSet(cBgD_t*, u32, Mtx34*)
ext 025F1B48 void shmrgrdZXYrotM(Mtx34*, s16, s16, s16)
ext 02312548 void shmrgrdQuatRotBaseY2(Quaternion*, cXyz*)
ext 0200E240 void shmrgrdRegisterCollision(cCcS*, dCcD_GObjInf*)
data resourceControl 101F4F28 nocheck
str 1002F510 "Shmrgrd"
str 1002F478 "d_a_obj_shmrgrd.cpp"
str 1002F48C "mdl_data != 0"
ext 026066C4 void* shmrgrdGetResource(u8*, be<u32>*, s32)
ext 02515FB8 void shmrgrdConstructGObj(dCcD_GObjInf*)
ext 02018590 void shmrgrdConstructCyl(cM3dGCyl*)
data hioStorage 1046BE7C nocheck
data hioRegistration 101CCB30 nocheck
data signedCharRegistration 101CCB3C nocheck
data unsignedCharRegistration 101CCB48 nocheck
data minPi 1046BE70 nocheck
data maxPi 1046BE74 nocheck
data signedCharStorage 1046BE78 nocheck
data unsignedCharStorage 1046BE79 nocheck
data particle_scale 1046BE8C nocheck
data particleScaleInitialized 1046BEA4 nocheck
data particleTranslation 1046BE98 nocheck
data particleTranslationInitialized 1046BEA8 nocheck
data mode_proc 1002F4E4 nocheck
ext 028F026C void shmrgrdRegisterDestructor(ShmrgrdRegistration*)
ext 028ED6F8 void shmrgrdConstructSignedChar(be<u8>*)
ext 028EAB2C void shmrgrdConstructUnsignedChar(be<u8>*)
ext 025E1A40 void shmrgrdSound(u32, cXyz*, s32, s32)
*/
