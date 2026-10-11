/** Leaf pile (Vochi). GameCube names and HD layout, verified against the original. */
#include "d/actor/d_a_obj_leaves.h"
using namespace leaves_attack;
const gc_tu_data gc_tu = {.safeString = 0x1002BF54,
                          .headerStatics = {0x1046A470, 0x101CA90C, 0x1046A47C}};
static const char l_arcname[] = "Vochi";
// HD: the table checker currently requires numeric initializers .
// tww particle IDs, in order: ID_AK_SN_FALLENLEAFFAN00, ID_AK_ST_FALLENLEAFSMOKE00, ID_AK_SN_FALLENLEAFRUN00;
// ID_AK_SN_FALLENLEAFFIRE00, ID_AK_SN_FALLENLEAFFIRE01, ID_AK_SN_FALLENLEAFBOMB00.
static LeavesEffect l_effect[6] = {
    {0x822F, {}, 0, 0, 1, -12.0f},
    {0xA230, {}, 1, 0, 1, -12.0f},
    {0x822E, {}, 0, 0, 1, -12.0f},
    {0x8268, {}, 0, 60, 120, -3.0f},
    {0x8269, {}, 0, 60, 120, -3.0f},
    {0x826A, {}, 0, 0, 1, -12.0f},
};
static dCcD_SrcSph l_sph_src;
static const char* arc() { return l_arcname; }
static GXColor* tint(daObjLeaves_c* p) { return &p->tevStr.mColorK0; }
static void changeAction(daObjLeaves_c* p, LeavesAction target) {
    p->mMemberFunc.i = -1;
    p->mMemberFunc.f = target;
    p->mMemberFunc.d = 0;
}
static void soundCurrent(daObjLeaves_c* p, cXyz* pos, u32 sound) {
    if (gabi::ea(p) != 0 && gabi::ea(pos) != 0) {
        s8 room = reinterpret_cast<actor_place*>(pos)->roomNo;
        s32 reverb = dComIfGp_getReverb(room);
        mDoAud_seStart(sound, pos, 0, reverb);
    }
}
/* 02369D08 */
void daObjLeaves_c::birthEffect(s32 index, cXyz* pos, csXyz* angle, GXColor* color) {
    if ((u32)index >= 6) {
        JUT_ASSERT_fail("d_a_obj_leaves.cpp", 0x1AE, "0");
        return;
    }
    LeavesEffect* entry = &l_effect[index];
    m438 = entry->mKind;
    s32 kind = entry->mKind;
    if (kind == 0) {
        u16 id = entry->mId;
        dComIfGp_particle_set(id, pos, angle, nullptr, 255, nullptr, -1, color);
    } else if (kind == 1) {
        mSmokeCallback.end();
        u16 id = entry->mId;
        dPa_control_c* particles = dComIfGp_getParticle();
        dPa_control_set(particles, 2, id, pos, angle, nullptr, 160,
                        reinterpret_cast<dPa_levelEcallBack*>(&mSmokeCallback), -1, nullptr,
                        nullptr, nullptr);
    } else {
        JUT_ASSERT_fail("d_a_obj_leaves.cpp", 0x1D6, "0");
    }
    m430 = entry->mFadeDelay;
    m434 = entry->mRetireDelay;
    m40C = entry->mAlphaStep;
}
// HD: callbacks retain the free entry ABI; static class callbacks await autoglue support.
/* 02369E64 */
static void rideCallBack(dBgW* mpBgW, daObjLeaves_c* p, fopAc_ac_c* rider) {
    if (rider == nullptr || rider->mProcName != fpcNm_PLAYER_e)
        return;
    if (rider->speedF < 8.0f)
        return;
    if (!(static_cast<daPy_py_c*>(rider)->mResetFlg0 &
          (daPy_py_c::daPyRFlg0_RIGHT_FOOT_ON_GROUND | daPy_py_c::daPyRFlg0_LEFT_FOOT_ON_GROUND)))
        return;
    p->birthEffect(2, &rider->current.pos, nullptr, tint(p));
}
/* 02369EB0 */
static void colorToFloat(LeavesLinearColor* out, GXColor* color) {
    f32 values[4] = {(f32)color->r / 255.0f, (f32)color->g / 255.0f, (f32)color->b / 255.0f,
                     (f32)color->a / 255.0f};
    for (u32 i = 0; i < 4; ++i)
        out->channel[i] = values[i];
}
/* 0236AF4C */
static u32 PrmAbstract(daObjLeaves_c* p, u32 width, u32 shift) {
    u32 parameters = p->mParameters;
    u32 mask = (width & 32) ? 0 : (1U << (width & 31));
    u32 value = (shift & 32) ? 0 : (parameters >> (shift & 31));
    return value & (mask - 1);
}
/* 02369F64 */
u32 daObjLeaves_c::chk_appear() {
    s32 sw = PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
    if (sw == 255)
        return true;
    return !dComIfGs_isSwitch(sw, home.roomNo);
}
/* 02369FC8 */
bool daObjLeaves_c::create_heap() {
    auto data = static_cast<J3DModelData*>(dComIfG_getObjectRes(arc(), VOCHI_BDL_VOCHI));
    if (data == nullptr) {
        JUT_ASSERT_fail("d_a_obj_leaves.cpp", 0x226, "0");
        return false;
    }
    mModel = mDoExt_J3DModel__create(data, 0x80000, 0x31000202);
    auto collision = static_cast<cBgD_t*>(dComIfG_getObjectRes(arc(), VOCHI_DZB_VOCHI));
    Mtx34* matrix = J3DModel_getBaseTRMtx(mModel);
    dBgW* created = dBgW_NewSet(collision, 1, matrix);
    bool hasModel = mModel != nullptr;
    mpBgW = created;
    if (!hasModel || created == nullptr)
        return false;
    reinterpret_cast<LeavesBgW*>(created)->mRideCallback = gabi::guest_fn((void*)rideCallBack);
    return true;
}
/* 0236A0BC */
static BOOL solidHeapCB(daObjLeaves_c* p) { return p->create_heap(); }
/* 0236A0C0 */
void daObjLeaves_c::init_mtx() {
    f32 x = scale.x, y = scale.y, z = scale.z;
    J3DModel* m = mModel;
    m->mBaseScale.x = x;
    m->mBaseScale.y = y;
    m->mBaseScale.z = z;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    J3DModel_setBaseTRMtx(mModel, mDoMtx_stack_c::get());
}
/* 0236A18C */
static void tg_hitCallback(daObjLeaves_c* p, dCcD_GObjInf* mSph, fopAc_ac_c* attacker,
                           dCcD_GObjInf* other) {
    auto* object = static_cast<dCcD_GObjInf*>(mSph->GetTgHitObj());
    if (object == nullptr)
        return;
    u32 type = object->mAtType;
    cXyz* pos = &p->current.pos;
    if (type == AT_TYPE_WIND) {
        fopAc_ac_c* player = ((dComIfG_inf_c*)dComIfGp_get())->play.mPlayerInfo[0].mpPlayer;
        f32 x = p->current.pos.x - player->current.pos.x;
        f32 z = p->current.pos.z - player->current.pos.z;
        s16 yaw = cM_atan2s(x, z);
        csXyz angle;
        angle.x = 0;
        angle.y = yaw;
        angle.z = 0;
        p->birthEffect(0, pos, &angle, tint(p));
        p->birthEffect(1, pos, &angle, nullptr);
        soundCurrent(p, pos, JA_SE_OBJ_OCHIBA_BLOW);
        p->m44C = 1;
        return;
    }
    if (type == AT_TYPE_BOMB) {
        p->birthEffect(5, pos, nullptr, tint(p));
        soundCurrent(p, pos, JA_SE_OBJ_OCHIBA_BLOW);
        p->m44C = 1;
        return;
    }
    cXyz difference;
    leavesSubtract(&p->mSph.mGObjTg.mHitPos, &difference, pos);
    cXyz horizontal;
    horizontal.x = difference.x;
    horizontal.y = 0.0f;
    horizontal.z = difference.z;
    f32 squared = leavesSquareMag(&horizontal);
    f32 distance = leavesSqrt(squared);
    if (!(distance < 110.0f))
        return;
    switch (type) {
    case AT_TYPE_SWORD:
    case AT_TYPE_UNK8:
    case AT_TYPE_BOOMERANG:
    case AT_TYPE_BOKO_STICK:
    case AT_TYPE_MACHETE:
    case AT_TYPE_UNK800:
    case AT_TYPE_SPIKE:
    case AT_TYPE_UNK2000:
    case AT_TYPE_NORMAL_ARROW:
    case AT_TYPE_HOOKSHOT:
    case AT_TYPE_ICE_ARROW:
    case AT_TYPE_LIGHT_ARROW:
    case AT_TYPE_STALFOS_MACE:
    case AT_TYPE_FAN_SWING:
    case AT_TYPE_DARKNUT_SWORD:
    case AT_TYPE_MOBLIN_SPEAR:
        p->birthEffect(2, &p->mSph.mGObjTg.mHitPos, nullptr, tint(p));
        break;
    case AT_TYPE_SKULL_HAMMER:
        p->birthEffect(5, pos, nullptr, tint(p));
        soundCurrent(p, pos, JA_SE_OBJ_OCHIBA_BLOW);
        break;
    case AT_TYPE_FIRE:
    case AT_TYPE_UNK20000:
    case AT_TYPE_FIRE_ARROW:
        p->birthEffect(3, pos, nullptr, nullptr);
        p->birthEffect(4, pos, nullptr, nullptr);
        soundCurrent(p, pos, JA_SE_OBJ_OCHIBA_BURN);
        p->m44C = 1;
        break;
    }
}
/* 0236A554 */
s32 daObjLeaves_c::_create() {
    if (!(actor_condition & fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            fopAc_ac_c_ct(this);
            __vtbl = 0x1002BF6C; // HD: this actor's compiler-generated vtable.
            dCcD_Stts_ct(&mStts);
            leavesSphere_ct(&mSph);
            __ct(&mSmokeCallback, 1);
        }
        actor_condition = actor_condition | fopAcCnd_INIT_e;
    }
    s32 result = cPhs_ERROR_e;
    u32 isVisible;
    if (mInitState == 0) {
        isVisible = chk_appear();
        m43C = (u8)isVisible;
    } else
        isVisible = m43C;
    if (isVisible)
        result = dComIfG_resLoad(&mPhase, arc());
    if (result == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((void*)solidHeapCB), 0xC20))
            return cPhs_ERROR_e;
        dBgS* bgs = dComIfG_Bgsp();
        if (dBgS_Regist(bgs, mpBgW, this))
            return cPhs_ERROR_e;
        cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mModel));
        init_mtx();
        mStts.Init(255, 255, this);
        mSph.Set(&l_sph_src);
        f32 x = current.pos.x, y = current.pos.y - 115.0f, z = current.pos.z;
        mSph.SetStts(&mStts);
        leavesSetSphereCenter(&mSph.mSph, x, y, z);
        mSph.SetR(150.0f);
        mSmokeCallback.m11 = 0;
        m408 = 255.0f;
        mMemberFunc.f = LeavesWait;
        mSmokeCallback.m12 = 1;
        mMemberFunc.i = -1;
        mSph.mGObjTg.mHitCallback = gabi::guest_fn((void*)tg_hitCallback);
        mSmokeCallback.mTevstr = gabi::ea(&tevStr);
        mMemberFunc.d = 0;
    }
    return result;
}
/* 0236A74C */
bool daObjLeaves_c::_delete() {
    if (m43C) {
        dComIfG_resDelete(&mPhase, arc());
        mSmokeCallback.remove();
        if (heap != nullptr && mpBgW != nullptr) {
            dBgW* w = mpBgW;
            if (dBgW_ChkUsed(w)) {
                dBgS* bgs = dComIfG_Bgsp();
                cBgS_Release(bgs, mpBgW);
            }
            mpBgW = nullptr;
        }
    }
    return true;
}
/* 0236A7E8 */
bool daObjLeaves_c::_execute() {
    s32 fade = (s32)((u32)m430 - 1);
    s32 retire = (s32)((u32)m434 - 1);
    m430 = fade > 0 ? fade : 0;
    m434 = retire > 0 ? retire : 0;
    (this->*mMemberFunc)();
    return true;
}
/* 0236ABD0 */
void daObjLeaves_c::wait_proc() {
    dBgW* w = mpBgW;
    if (w != nullptr)
        dBgW_Move(w);
    mStts.Move();
    mSph.ClrTgHit();
    if (!m44C) {
        leavesSetCollision(dComIfG_Ccsp(), &mSph);
        return;
    }
    s32 sw = PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
    if (sw != 255)
        dComIfGs_onSwitch(sw, home.roomNo);
    actor_status = actor_status & ~u32(fopAcStts_NOCULLEXEC_e);
    w = mpBgW;
    if (w != nullptr && dBgW_ChkUsed(w)) {
        dBgS* bgs = dComIfG_Bgsp();
        cBgS_Release(bgs, mpBgW);
    }
    changeAction(this, LeavesAlphaWait);
}
/* 0236ACCC */
void daObjLeaves_c::alpha_calc_start_wait_proc() {
    if (m430 <= 0)
        changeAction(this, LeavesItemWait);
}
/* 0236ACF8 */
void daObjLeaves_c::item_set_wait_proc() {
    f32 value = m408 + m40C;
    m408 = value;
    if (!(value > 200.0f)) {
        u32 item = PrmAbstract(this, PRM_ITEM_NO_W, PRM_ITEM_NO_S);
        u32 saveBit = PrmAbstract(this, PRM_ITEM_SAVE_BIT_NO_W, PRM_ITEM_SAVE_BIT_NO_S);
        cXyz position;
        position.x = current.pos.x;
        position.y = current.pos.y;
        position.z = current.pos.z;
        csXyz angle;
        csXyz_ct(&angle, 0, 0, 0);
        position.y = position.y - 30.0f;
        fopAcM_createItemFromTable(&position, item, saveBit, home.roomNo, 0, &angle, 1, nullptr);
        changeAction(this, LeavesRetireWait);
    }
}
/* 0236ADFC */
u32 daObjLeaves_c::retire_wait_proc() {
    f32 value = m408 + m40C;
    if (!(value > 0.0f)) {
        s32 delay = m434;
        m408 = 0.0f;
        if (delay <= 0)
            return fopAcM_delete(this);
    } else
        m408 = value;
    return (u32)gabi::ea(this); // HD: preserve the original r3 result.
}
/* 0236A748 */
static s32 daObjLeaves_Create(daObjLeaves_c* p) { return p->_create(); }
/* 0236A7E4 */
static BOOL daObjLeaves_Delete(daObjLeaves_c* p) { return p->_delete(); }
/* 0236A880 */
static BOOL daObjLeaves_Execute(daObjLeaves_c* p) { return p->_execute(); }
/* 0236ABC8 */
static BOOL daObjLeaves_IsDelete(daObjLeaves_c* p) { return TRUE; }
// HD: material colors also feed linear float overrides in the HD renderer.
static GXColor* materialKColor(LeavesMaterial* material) {
    LeavesTevBlock* block = material->mTevBlock;
    LeavesTevVtable* vtable = block->mVtable;
    return gabi::call_ptr<GXColor*>(vtable->mGetKColor, block, 3);
}
static void materialSetKColor(LeavesMaterial* material, GXColor* color) {
    LeavesTevBlock* block = material->mTevBlock;
    LeavesTevVtable* vtable = block->mVtable;
    gabi::call_ptr(vtable->mSetKColor, block, 3, color);
}
/* 0236A884 */
bool daObjLeaves_c::_draw() {
    settingTevStruct(dKy_getEnvlight(), 1, &current.pos, &tevStr);
    auto* light = dKy_getEnvlight();
    setLightTevColorType(light, mModel, &tevStr);
    u8 opacity = (u8)gabi::ftoi(m408);
    J3DModel* model = mModel;
    J3DModelData* data = model->mModelData;
    leavesSetTranslucent(model, opacity != 255);
    LeavesJointData* jointData = leavesGetJointData(data);
    for (u16 i = 0; i < jointData->mJointNum; ++i) {
        u32 count = data->mJointNum;
        LeavesJoint* joints = gabi::at<LeavesJoint>(data->mJointNodes);
        if (i < count)
            joints += i;
        LeavesMaterial* material = joints->mMesh;
        while (material != nullptr) {
            LeavesShape* shape = material->mShape;
            if (opacity == 0)
                shape->mVisible = 0;
            else {
                shape->mVisible = 1;
                if (opacity != 255)
                    leavesSetMaterialAlpha(material, opacity, 0);
                else {
                    LeavesMaterialResource* entry = material->mResource;
                    u32 offset = entry->mBlendOffset;
                    void* blend = offset == 0
                                      ? nullptr
                                      : gabi::at<void>(gabi::ea(&entry->mBlendOffset) + offset);
                    leavesSetBlend(blend, 1);
                }
                GXColor* color = materialKColor(material);
                color->a = opacity;
                GXColor* copied = materialKColor(material);
                materialSetKColor(material, copied);
                LeavesLinearColor normalized;
                colorToFloat(&normalized, copied);
                cXyz linear;
                leavesToLinear(&linear, &normalized, 1.0f);
                u32 dirty = material->mDirty;
                material->mDirty = dirty | 0x400;
                LeavesLinearColor* overrideColor = leavesGetColorOverride(&material->mDirty, 10);
                f32 a = (f32)copied->a / 255.0f;
                f32 x = linear.x, y = linear.y, z = linear.z;
                overrideColor->channel[0] = x;
                overrideColor->channel[1] = y;
                overrideColor->channel[2] = z;
                overrideColor->channel[3] = a;
            }
            material = material->mNext;
        }
        jointData = leavesGetJointData(data);
    }
    mDoExt_modelUpdateDL(mModel);
    return true;
}
/* 0236ABC4 */
static BOOL daObjLeaves_Draw(daObjLeaves_c* p) { return p->_draw(); }

/* 0236AE34 */
// HD: compiler-generated header initialization and deleting destructors.
static void sinit() { __sinit_header_statics(); }
/* 0236AEC8 */
static void trivial_dt(void* p, s32 flags) {
    if (p && (flags & 1))
        operator_delete(p);
}
/* 0236AEDC */
static void destroy(daObjLeaves_c* p, s32 flags) {
    if (p) {
        leavesSphere_dt(&p->mSph, 2);
        dCcD_Stts_dt(&p->mStts, 2);
        leavesActor_dt(p, 0);
        if (flags & 1)
            operator_delete(p);
    }
}
/* 0236AF48 */
static void emptyVirtual(void* p) {}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py): unit-local data and HD ABI adapters

str 1002BF4C "Vochi"
str 1002BF0C "d_a_obj_leaves.cpp" in 02369D08
str 1002BF08 "0" in 02369D08
str 1002BFC0 "d_a_obj_leaves.cpp" in 02369FC8
str 1002BFBC "0" in 02369FC8
data l_effect 101CA874
data l_sph_src 1002BF7C
ext 025166F0 void leavesSphere_ct(dCcD_Sph* self)
ext 02515AE8 void leavesSphere_dt(dCcD_Sph* self,s32 flags)
ext 025D50BC void leavesActor_dt(fopAc_ac_c* self,s32 flags)
ext 02018F3C void leavesSetSphereCenter(cM3dGSph* self,f32 x,f32 y,f32 z)
ext 0200E240 void leavesSetCollision(cCcS* self,dCcD_Sph* sphere)
ext 0201ADE0 void leavesSubtract(cXyz* left,cXyz* out,cXyz* right)
ext 028E8DD0 f32 leavesSquareMag(cXyz* value)
ext 028F4384 f32 leavesSqrt(f32 value)
ext 027F58E0 void leavesSetTranslucent(J3DModel* self,bool enabled)
ext 027F3F94 LeavesJointData* leavesGetJointData(J3DModelData* self)
ext 025F0AD0 void leavesSetMaterialAlpha(LeavesMaterial* self,u32 opacity,u32 mode)
ext 027E212C void leavesSetBlend(void* blend,u32 enabled)
ext 0274D458 void leavesToLinear(cXyz* out,LeavesLinearColor* input,f32 scale)
ext 027F9F0C LeavesLinearColor* leavesGetColorOverride(be<u32>* block,u32 index)
*/
// clang-format on
