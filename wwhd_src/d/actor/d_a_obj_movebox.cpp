/* WWHD pushable box. Ported from zeldaret/tww with HD changes verified against cking.rpx.
 */
#include "d/actor/d_a_obj_movebox.h"
#include "d/actor/d_a_player.h"
#include "gc_api/d/d_kankyo.h"
void moveboxGroundBaseDestroy(dBgS_GndChk*, s32);
namespace daObjMovebox {
static MoveboxRegistrationsHD l_registrations;
static MoveboxCollisionWorkHD l_collisionWork;
static MoveboxStaticsHD l_statics;
static cXyz l_baseY;
static MoveboxCylinderSourceHD l_cylinderSource;
static u32 l_modeGuard;
static MoveboxModeTableHD l_modeTable;
static MoveboxModeTableHD l_modeInitialTable;
static MoveboxAttrStorageHD l_attributes;
static MoveboxArchiveNamesHD l_archiveNames;
static MoveboxLineSourcesHD l_lineSources;
static const s16 l_directionBase[4] = {0, 0x4000, -0x8000, -0x4000};
const be<s16> (&Act_c::M_dir_base)[4] = l_directionBase;
gptr<const char> (&Act_c::M_arcname)[13] = l_archiveNames.mNames;
Attr_c (&Act_c::M_attr)[13] = l_attributes.mItems;
BgcSrc_c (&Bgc_c::M_lin5)[5] = l_lineSources.mSmall;
BgcSrc_c (&Bgc_c::M_lin20)[23] = l_lineSources.mLarge;
dBgS_ObjGndChk (&Bgc_c::M_gnd_work)[23] = l_collisionWork.mGround;
MoveboxWaterWorkHD& Bgc_c::M_wrt_work = l_collisionWork.mWater;
dBgS_ObjLinChk (&Bgc_c::M_wall_work)[23] = l_collisionWork.mWall;

/* 02376EE8 */
void Act_c::prmX_init() {
    if (!mbPrmXInitialized) {
        s16 x = home.angle.x;
        mbPrmXInitialized = 1;
        mPrmX = x;
    }
    home.angle.x = 0;
    current.angle.x = 0;
    shape_angle.x = 0;
}
/* 02376F18 */
void Act_c::prmZ_init() {
    if (mbPrmZInitialized)
        return;
    mbPrmZInitialized = 1;
    u16 z = 0xFFFF;
    if (!prm(1, 30))
        z = home.angle.z;
    mPrmZ = z;
    home.angle.z = 0;
    shape_angle.z = 0;
    current.angle.z = 0;
}
/* 02376F90 */
s32 Act_c::prm_get_swSave1() { return prm(8, 8); }
/* 02376F9C */
bool Act_c::chk_appear() {
    u8 path = pathId();
    s32 sw = prm_get_swSave1();
    if (path != 255 || sw == 255)
        return true;
    bool set = dComIfGs_isSwitch(sw, home.roomNo) != 0;
    bool enabled = prm(1, 30) != 0;
    return set == enabled;
}
/* 02377078 */
void Act_c::path_init() {
    s32 type = mType;
    u8 path = type == 11 ? 255 : (mPrmZ & 255);
    s32 sw = prm_get_swSave1();
    if (path == 255 || sw == 255)
        return;
    u8 sw2 = type == 11 ? 255 : (mPrmZ >> 8);
    bool moved1 = dComIfGs_isSwitch(sw, home.roomNo) != 0;
    bool moved2 = false;
    if (sw2 != 255)
        moved2 = dComIfGs_isSwitch(swSave2(), home.roomNo) != 0;
    s32 idx = moved1 ? 1 : 0;
    if (moved2)
        idx += 2;
    MoveboxPathHD* p = moveboxGetRoomPath(path, (s8)home.roomNo);
    mpPath = p;
    MoveboxPathPointHD* point = moveboxGetPathPoint(p, idx);
    home.pos.x = point->m_position.x;
    home.pos.y = point->m_position.y;
    home.pos.z = point->m_position.z;
    current.pos.x = point->m_position.x;
    current.pos.y = point->m_position.y;
    current.pos.z = point->m_position.z;
}
/* 023771D8 */
BOOL Act_c::CreateHeap() {
    const Attr_c* a = attr(0x1002D278);
    if (a->mModelFileIndex < 0) {
        mpModel = nullptr;
        return TRUE;
    }
    const char* name = arc();
    s32 idx = attr(0x1002D278)->mModelFileIndex;
    auto* data = (J3DModelData*)dComIfG_getObjectRes(name, idx, 0x1002CFAC);
    if (!data)
        JUT_ASSERT_fail(STR(0x1002D278), 0x6C1, STR(0x1002D2B0));
    J3DModel* model = mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
    BOOL success = model != nullptr;
    mpModel = model;
    return success;
}
/* 023772EC */
void Act_c::clr_moment_cnt() {
    for (s32 i = 0; i < 4; i++)
        mMomentCnt[i] = 0;
}
/* 02377B84 */
void Act_c::mode_wait_init() {
    speedF = 0.0f;
    const Attr_c* a = attr(0x1002D394);
    dBgW* w = mpBgW;
    gravity = a->m14;
    backgroundHD(w)->mCrrFunc = MOVEBOX_CRR_TRANSLATE;
    clr_moment_cnt();
    m634 = -1;
    mMode = MODE_WAIT;
}
/* 0237845C */
void Act_c::mode_afl_init() {
    dBgW* w = mpBgW;
    speedF = 0.0f;
    backgroundHD(w)->mCrrFunc = MOVEBOX_CRR_TYPICAL;
    mMode = MODE_AFLOAT;
}
/* 02378484 */
void Act_c::sound_land() {
    u32 material = 0;
    s32 idx = mBgc.mMaxGroundIdx;
    if (idx >= 0) {
        dBgS_ObjGndChk* check = &Bgc_c::M_gnd_work[idx];
        if (check->mBgIndex < 0x100) {
            dBgS* bgs = dComIfG_Bgsp();
            material = dBgS_GetMtrlSndId(bgs, static_cast<cBgS_PolyInfo*>(check));
        }
    }
    const Attr_c* a = attr(0x1002D41C);
    s8 reverb = mReverb;
    u32 sound = a->mNormalFallSE;
    mDoAud_seStart(sound, &eyePos, material, reverb);
}
/* 02378544 */
void Act_c::vib_land() {
    dVibration_c* vibration = dComIfGp_getVibration();
    gabi::Local<cXyz> v;
    v->x = 0.0f;
    v->y = 1.0f;
    v->z = 0.0f;
    vibration->StartShock(4, -33, v.get());
}
/* 02378594 */
void Act_c::eff_land_smoke() {
    f32 size = attr(0x1002D458)->mLandSmokeScale;
    gabi::Local<cXyz> v;
    v->set(size, size, size);
    PSVECScale(v.get(), v.get(), 1.6666666269302368f);
    moveboxCreateActor(0x1D3, 3, &current.pos, -1, (csXyz*)nullptr, v.get(), -1, 0);
}
/* 02378A4C */
void Act_c::eff_smoke_slip_remove() {
    for (s32 i = 0; i < 2; i++)
        mSmokeCbs[i].remove();
}
/* 02378A9C */
BOOL Act_c::Delete() {
    eff_smoke_slip_remove();
    if (mpBgW != nullptr) {
        backgroundHD(mpBgW)->mRideCallback = 0;
        backgroundHD(mpBgW)->mPushPullCallback = 0;
    }
    return TRUE;
}
/* 023792E8 */
void Act_c::mode_walk_init() {
    speedF = 0.0f;
    m64F = 0;
    mMode = MODE_WALK;
}
/* 0237966C */
void Act_c::sound_slip() {
    u32 material = 0;
    s32 idx = mBgc.mMaxGroundIdx;
    if (idx >= 0) {
        dBgS_ObjGndChk* check = &Bgc_c::M_gnd_work[idx];
        if (check->mBgIndex < 0x100) {
            dBgS* bgs = dComIfG_Bgsp();
            material = dBgS_GetMtrlSndId(bgs, static_cast<cBgS_PolyInfo*>(check));
        }
    }
    const Attr_c* a = attr(0x1002D5F8);
    s8 reverb = mReverb;
    u32 sound = a->mMoveSE;
    mDoAud_seStart(sound, &eyePos, material, reverb);
}
/* 0237972C */
void Act_c::sound_limit() {
    u32 material = 0;
    s32 idx = mBgc.mWallIdx;
    if (idx >= 0) {
        dBgS_ObjLinChk* check = &Bgc_c::M_wall_work[idx];
        if (check->mBgIndex < 0x100) {
            dBgS* bgs = dComIfG_Bgsp();
            material = dBgS_GetMtrlSndId(bgs, static_cast<cBgS_PolyInfo*>(check));
        }
    }
    const Attr_c* a = attr(0x1002D630);
    s8 reverb = mReverb;
    u32 sound = a->mCantMoveSE;
    mDoAud_seStart(sound, &eyePos, material, reverb);
}
/* 023797EC */
void Act_c::eff_smoke_slip_end() {
    for (s32 i = 0; i < 2; i++)
        mSmokeCbs[i].end();
}
/* 02378150 */
void Act_c::make_item() {
    s32 item = prm(6, 0);
    s32 bit = prm(7, 16);
    fopAcM_createItemFromTable(&current.pos, item, bit, (s8)home.roomNo, 0, &current.angle, 7, 0);
}
/* 023781C8 */
void Act_c::eff_break() {
    if (!l_statics.mBreakScaleGuard) {
        l_statics.mBreakScale.set(2.0f, 2.0f, 1.0f);
        l_statics.mBreakScaleGuard = 1;
    }
    gabi::Local<cXyz> pos;
    pos->set(current.pos.x, current.pos.y + 75.0f, current.pos.z);
    dPa_control_c* particle = dComIfGp_getParticle();
    JPABaseEmitter* emitter = particle->set(0, 0x3E6, pos.get(), 0, 0, 255, 0, -1, &tevStr.mColorK0,
                                            &tevStr.mColorK0, &l_statics.mBreakScale);
    if (emitter) {
        reinterpret_cast<MoveboxEmitterHD*>(emitter)->mLifeTime = 30;
        reinterpret_cast<MoveboxEmitterHD*>(emitter)->mInitialVelAxis = 30.0f;
    }
    moveboxCreateActor(0x1D3, 5, pos.get(), -1, 0, 0, -1, 0);
}
/* 023782D8 */
void Act_c::sound_break() {
    f32 x = current.pos.x, y = current.pos.y + 100.0f, z = current.pos.z;
    gabi::Local<dBgS_GndChk> check;
    dBgS_GndChk_ct(check.get(), {0x1002D034, 0x1002D044, 0x1002D064, 0x1002D054}, true);
    check->m_pos.set(x, y, z);
    check->mActorPid = mBsPcId;
    dComIfG_Bgsp()->GroundCross(check.get());
    u32 material = 0;
    if (check->mBgIndex < 0x100)
        material = dBgS_GetMtrlSndId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(check.get()));
    s8 reverb = dComIfGp_getReverb(current.roomNo);
    mDoAud_seStart(0x6805, &eyePos, material, reverb);
    check->cBgS_PolyInfo::__vtbl = 0x1002D004;
    check->dBgS_PolyPassChk::__vtbl = 0x1002D024;
    check->dBgS_GrpPassChk::__vtbl = 0x1002CFE4;
    moveboxGroundBaseDestroy(check.get(), 0);
}
/* 02378970 */
BOOL Act_c::Draw() {
    if (mpModel != nullptr) {
        u8 tevType = attr(0x1002D4C8)->mbUseBGTevType;
        g_env_light.settingTevStruct(tevType, &current.pos, &tevStr);
        g_env_light.setLightTevColorType(mpModel.get(), &tevStr);
        dComIfGd_setListBG();
        moveboxUpdateModel(mpModel.get(), 0);
        dComIfGd_setList();
    }
    return TRUE;
}

/* 02376518 */
void Bgc_c::wrt_pos(const cXyz* pos) {
    MoveboxWaterWorkHD* w = &Bgc_c::M_wrt_work;
    f32 y = pos->y, z = pos->z, x = pos->x;
    w->mGroundPos.set(x, y - 100.0f, z);
    w->mRoofY = y + 400.0f;
    if (moveboxWaterCheck(dComIfG_Bgsp(), w)) {
        u32 flags = mStateFlags;
        f32 height = w->mHeight;
        flags |= 8;
        mWaterY = height;
        mStateFlags = flags;
        if (height > pos->y)
            mStateFlags = flags | 0x10;
    }
}
/* 02377B5C */
void Act_c::init_mtx() {
    if (mpModel != nullptr) {
        f32 y = scale.y, z = scale.z, x = scale.x;
        mpModel->mBaseScale.z = z;
        mpModel->mBaseScale.y = y;
        mpModel->mBaseScale.x = x;
    }
    set_mtx();
}
/* 02378AE8 */
void Act_c::path_save() {
    s32 type = mType;
    u8 path = type == 11 ? 255 : (mPrmZ & 255);
    s32 sw = prm_get_swSave1();
    if (path == 255 || sw == 255)
        return;
    u8 second = type == 11 ? 255 : (mPrmZ >> 8);
    MoveboxPathHD* p = mpPath;
    s32 count = second == 255 ? 2 : 4;
    s32 num = p->m_num;
    if (count > num)
        count = num;
    for (s32 index = 0; index < count; index++) {
        MoveboxPathPointHD* point = moveboxGetPathPoint(p, index);
        gabi::Local<cXyz> pos;
        pos->set(point->m_position.x, point->m_position.y, point->m_position.z);
        f32 dist = moveboxSquareDistance(&current.pos, pos.get());
        if (dist < 9.0f) {
            s32 first = prm_get_swSave1();
            if (index & 1)
                dComIfGs_onSwitch(first, home.roomNo);
            else
                dComIfGs_offSwitch(first, home.roomNo);
            if (second != 255) {
                if (index & 2)
                    dComIfGs_onSwitch(swSave2(), home.roomNo);
                else
                    dComIfGs_offSwitch(swSave2(), home.roomNo);
            }
            return;
        }
        p = mpPath;
    }
}

/* 02376B14 */
bool Bgc_c::chk_wall_pre(const Act_c* actor, const BgcSrc_c* src, s32 count, s16 angle) {
    wall_pos(actor, src, count, angle, 74.0f);
    return mWallIdx >= 0;
}
/* 02376E74 */
bool Bgc_c::chk_wall_touch2(const Act_c* actor, const BgcSrc_c* src, s32 count, s16 angle) {
    for (s32 i = 0; i < count; i++)
        if (chk_wall_touch(actor, src + i, angle))
            return true;
    return false;
}

/* 02376914 */
void Bgc_c::proc_vertical(Act_c* actor) {
    u32 previous = mStateFlags;
    mStateFlags = 0;
    const BgcSrc_c* src = actor->attr(0x1002D154, 0x1002D188)->m9A ? Bgc_c::M_lin20 : Bgc_c::M_lin5;
    s32 count = actor->attr(0x1002D154, 0x1002D188)->m9A ? 21 : 5;
    gnd_pos(actor, src, count, 100.0f);
    s32 idx = mMaxGroundIdx;
    if (idx >= 0 && mGroundY[idx] > actor->current.pos.y) {
        actor->current.pos.y = mGroundY[idx];
        mStateFlags = mStateFlags | 1;
        actor->speed.y = 0.0f;
    }
    wrt_pos(&actor->current.pos);
    u32 flags = mStateFlags;
    if ((flags & 8) && mWaterY > actor->current.pos.y) {
        flags |= 16;
        mStateFlags = flags;
    }
    if (!(flags & 1) && (previous & 1)) {
        mStateFlags = flags | 2;
        actor->speed.y = actor->attr(0x1002D154, 0x1002D188)->m20;
        flags = mStateFlags;
    }
    if ((flags & 1) && !(previous & 1)) {
        flags |= 4;
        mStateFlags = flags;
    }
    if ((flags & 16) && !(previous & 16)) {
        flags |= 32;
        mStateFlags = flags;
    }
    if ((flags & 1) && mMaxGroundIdx >= 0) {
        dBgS* bg = dComIfG_Bgsp();
        dBgS_ObjGndChk* check = &Bgc_c::M_gnd_work[(s32)mMaxGroundIdx];
        moveboxRideCallback(bg, check ? static_cast<cBgS_PolyInfo*>(check) : nullptr, actor);
    }
}

// HD: autoglue requires the original instrumentation for this static callback.
fopAc_ac_c* Act_c::PPCallBack(fopAc_ac_c* target, fopAc_ac_c* unused, s16 angle, u32 label) {
    WWHD_FUNC(0x023775DC, fopAc_ac_c*, target, unused, angle, label);
    Act_c* box = (Act_c*)target;
    u32 push = label & 3;
    bool eligible = box->attr(0x1002D300)->m9A ? ((label >> 3) & 1) != 0 : true;
    if (push && eligible) {
        s32 facing = angle;
        if (push & 2)
            facing -= 0x8000;
        facing = (s16)(facing - (s16)box->home.angle.y);
        if (push == 3)
            JUT_ASSERT_fail(STR(0x1002D300), 0x71C, STR(0x1002D338));
        box->mPPLabel = label;
        u32 direction;
        if ((u32)(facing + 0x2000) < 0x4000)
            direction = 0;
        else if ((u32)(facing - 0x2000) < 0x4000)
            direction = 1;
        else
            direction = (u32)(facing + 0x6000) < 0xC000 ? 3 : 2;
        for (u32 j = 0; j < 4; j++)
            box->mMomentCnt[j] = j == direction ? (s16)(box->mMomentCnt[j] + 1) : 0;
        box->m64A = 1;
    }
    return target;
}

VERIFY(0x023775DC, Act_c::PPCallBack);

/* 02376314 */
void Bgc_c::gnd_pos(const Act_c* actor, const BgcSrc_c* src, s32 count, f32 height) {
    f32 max = -1000000000.0f;
    PSMTXTrans(mDoMtx_stack_c::get(), (f32)actor->current.pos.x, (f32)actor->current.pos.y,
               (f32)actor->current.pos.z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)actor->home.angle.y);
    mMaxGroundIdx = -1;
    for (s32 i = 0; i < count; i++, src++) {
        f32 width = actor->attr(0x1002D154, 0x1002D188)->mScaleXZ;
        f32 x = gabi::fmadds(src->m04, width, src->m0C);
        width = actor->attr(0x1002D154, 0x1002D188)->mScaleXZ;
        f32 z = gabi::fmadds(src->m00, width, src->m08);
        gabi::Local<cXyz> offset, pos;
        offset->set(x, height, z);
        PSMTXMultVec(mDoMtx_stack_c::get(), offset.get(), pos.get());
        dBgS_ObjGndChk* check = &Bgc_c::M_gnd_work[i];
        check->m_pos.copy(*pos.get());
        check->mActorPid = actor->mBsPcId;
        f32 ground = dComIfG_Bgsp()->GroundCross(check);
        mGroundY[i] = ground;
        if (ground > max) {
            dBgS* bg = dComIfG_Bgsp();
            fopAc_ac_c* other = moveboxGroundActor(bg, (u16)check->mBgIndex);
            if (!other || other->mProcName != 0x2B || reinterpret_cast<Act_c*>(other)->mMode != 2) {
                max = mGroundY[i];
                mMaxGroundIdx = i;
            }
        }
    }
}

/* 02379010 */
void Act_c::eff_set_slip_smoke_pos() {
    if (!l_statics.mSmokeBaseGuard) {
        l_statics.mSmokeBaseGuard = 1;
        l_statics.mSmokeBasePos[0].set(-0.5f, 0.0f, -0.5f);
        l_statics.mSmokeBasePos[1].set(0.5f, 0.0f, -0.5f);
    }
    s16 angle = (s16)((s16)home.angle.y + M_dir_base[(s32)m634]);
    f32 scale = attr(0x1002D540)->mScaleXZ;
    PSMTXTrans(mDoMtx_stack_c::get(), (f32)current.pos.x, (f32)current.pos.y, (f32)current.pos.z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), angle);
    mDoMtx_stack_c::transM(0.0f, 0.0f, 10.0f);
    mDoMtx_stack_c::scaleM(scale, scale, scale);
    for (s32 i = 0; i < 2; i++) {
        PSMTXMultVec(mDoMtx_stack_c::get(), &l_statics.mSmokeBasePos[i], &mSmokeCbs[i].field_0x20);
        mSmokeCbs[i].field_0x2C.x = 0;
        mSmokeCbs[i].field_0x2C.y = angle;
        mSmokeCbs[i].field_0x2C.z = 0;
    }
}
/* 0237918C */
void Act_c::eff_smoke_slip_start() {
    if (!l_statics.mSmokeScaleGuard) {
        l_statics.mSmokeScaleGuard = 1;
        l_statics.mSmokeScale.set(0.6000000238418579f, 0.6000000238418579f, 0.6000000238418579f);
    }
    attr(0x1002D584);
    eff_set_slip_smoke_pos();
    for (s32 i = 0; i < 2; i++) {
        EffSmokeCB* cb = &mSmokeCbs[i];
        s8 room = current.roomNo;
        dPa_control_c* particle = dComIfGp_getParticle();
        JPABaseEmitter* emitter =
            particle->set(2, 0x2022, &cb->field_0x20, &cb->field_0x2C, &l_statics.mSmokeScale, 0xB9,
                          reinterpret_cast<dPa_levelEcallBack*>(cb), room, 0, 0, 0);
        if (emitter) {
            reinterpret_cast<MoveboxEmitterHD*>(emitter)->mInitialVelDir = 15.0f;
            reinterpret_cast<MoveboxEmitterHD*>(emitter)->mSpread = 0.15000000596046448f;
            reinterpret_cast<MoveboxEmitterHD*>(emitter)->mRate = 2.0f;
            reinterpret_cast<MoveboxEmitterHD*>(emitter)->mLifeTime = 30;
        }
    }
}

// HD: autoglue requires the original instrumentation for this static callback.
void Act_c::RideCallBack(dBgW* bg, fopAc_ac_c* target, fopAc_ac_c* rider) {
    WWHD_FUNC(0x02377308, void, bg, target, rider);
    Act_c* box = (Act_c*)target;
    if (box->mMode != 2)
        return;
    const Attr_c* a = box->attr(0x1002D2C8);
    const Attr_c* b = box->attr(0x1002D2C8);
    f32 limit = (f32)a->m2C + (f32)b->m30;
    f32 dx = (f32)rider->current.pos.x - (f32)box->current.pos.x;
    f32 dz = (f32)rider->current.pos.z - (f32)box->current.pos.z;
    f32 weight, moment;
    const Attr_c* reciprocal;
    if (rider != nullptr && rider->mProfName == 0xA8) {
        weight = box->attr(0x1002D2C8)->m2C;
        const Attr_c* size = box->attr(0x1002D2C8);
        const Attr_c* force = box->attr(0x1002D2C8);
        moment = (f32)size->m74 * (f32)force->m44;
        reciprocal = box->attr(0x1002D2C8);
    } else {
        weight = box->attr(0x1002D2C8)->m30;
        const Attr_c* size = box->attr(0x1002D2C8);
        const Attr_c* force = box->attr(0x1002D2C8);
        moment = (f32)size->m74 * (f32)force->m48;
        reciprocal = box->attr(0x1002D2C8);
    }
    f32 distance = std_sqrtf(gabi::fmadds(dx, dx, dz * dz));
    f32 ratio = gabi::fnmsubs(distance, reciprocal->m74, 1.0f);
    ratio = gabi::fmadds(ratio, 0.8999999761581421f, 0.10000000149011612f);
    ratio = ratio >= 0.0f ? ratio : 0.0f;
    f32 down = gabi::fmadds(weight, ratio, box->m608);
    f32 x = box->m60C, z = box->m610;
    if (down < limit)
        down = limit;
    box->m608 = down;
    box->m60C = gabi::fmadds(moment, dx, x);
    box->m610 = gabi::fmadds(moment, dz, z);
}

VERIFY(0x02377308, Act_c::RideCallBack);

/* 023765D0 */
void Bgc_c::wall_pos(const Act_c* actor, const BgcSrc_c* src, s32 count, s16 angle, f32 distance) {
    angle = (s16)(angle + (s16)actor->home.angle.y);
    mWallIdx = -1;
    mNearestWallDist = 3.4028234663852886e38f;
    auto* matrix = mDoMtx_stack_c::get();
    gabi::Local<cXyz> dir, offset, rotated, start, finish, tmp;
    mDoMtx_YrotS(matrix, angle);
    mDoMtx_XrotM(matrix, 0x4000);
    PSMTXMultVec(matrix, &l_baseY, dir.get());
    f32 size = actor->attr(0x1002D154, 0x1002D188)->mScaleXZ;
    PSVECScale(dir.get(), dir.get(), gabi::fmadds(size, 0.5f, distance));
    for (s32 i = 0; i < count; i++, src++) {
        moveboxXrotS(matrix, 0x4000);
        offset->set(src->m0C, 0.0f, src->m08);
        PSMTXMultVec(matrix, offset.get(), rotated.get());
        mDoMtx_YrotS(matrix, angle);
        mDoMtx_stack_c::transM((f32)rotated->x, (f32)rotated->y, (f32)rotated->z);
        const Attr_c* a = actor->attr(0x1002D154, 0x1002D188);
        const Attr_c* b = actor->attr(0x1002D154, 0x1002D188);
        const Attr_c* c = actor->attr(0x1002D154, 0x1002D188);
        mDoMtx_stack_c::scaleM((f32)a->mScaleXZ, (f32)b->mScaleY, (f32)c->mScaleXZ);
        mDoMtx_stack_c::transM(0.0f, 0.5f, 0.0f);
        mDoMtx_XrotM(matrix, 0x4000);
        offset->set(src->m04, 0.0f, src->m00);
        PSMTXMultVec(matrix, offset.get(), start.get());
        PSVECAdd(start.get(), &actor->current.pos, start.get());
        cXyz_pl(start.get(), tmp.get(), dir.get());
        finish->copy(*tmp.get());
        dBgS_ObjLinChk* check = &Bgc_c::M_wall_work[i];
        check->Set(start.get(), finish.get(), const_cast<Act_c*>(actor));
        check->mActorPid = actor->mBsPcId;
        if (dComIfG_Bgsp()->LineCross(check)) {
            mWallPos[i].copy(*&check->mLin.mEnd);
            f32 dist = moveboxSquareDistance(start.get(), &mWallPos[i]);
            if (dist < mNearestWallDist) {
                mNearestWallDist = dist;
                mWallIdx = i;
            }
        } else
            mWallPos[i].copy(*&cXyz::Zero);
    }
}

/* 02376B54 */
bool Bgc_c::chk_wall_touch(const Act_c* actor, const BgcSrc_c* src, s16 angle) {
    dBgS_ObjLinChk* check = &l_statics.mTouchWork;
    if (!l_statics.mTouchGuard) {
        l_statics.mTouchGuard = 1;
        dBgS_LinChk_ct(check, {0x1002D114, 0x1002D124, 0x1002D144, 0x1002D134}, true);
        __register_global_object(gabi::ea(&l_registrations.mTouch));
    }
    angle = (s16)(angle + (s16)actor->home.angle.y);
    auto* matrix = mDoMtx_stack_c::get();
    gabi::Local<cXyz> dir, offset, rotated, start, finish, tmp;
    mDoMtx_YrotS(matrix, angle);
    mDoMtx_XrotM(matrix, 0x4000);
    offset->set(src->m0C, 0.0f, src->m08);
    PSMTXMultVec(matrix, &l_baseY, dir.get());
    f32 size = actor->attr(0x1002D154, 0x1002D188)->mScaleXZ;
    PSVECScale(dir.get(), dir.get(), gabi::fmadds(size, 0.5f, 10.0f));
    mDoMtx_XrotM(matrix, 0x4000);
    offset->set(src->m0C, 0.0f, src->m08);
    PSMTXMultVec(matrix, offset.get(), rotated.get());
    mDoMtx_YrotS(matrix, angle);
    mDoMtx_stack_c::transM((f32)rotated->x, (f32)rotated->y, (f32)rotated->z);
    const Attr_c* a = actor->attr(0x1002D154, 0x1002D188);
    const Attr_c* b = actor->attr(0x1002D154, 0x1002D188);
    const Attr_c* c = actor->attr(0x1002D154, 0x1002D188);
    mDoMtx_stack_c::scaleM((f32)a->mScaleXZ, (f32)b->mScaleY, (f32)c->mScaleXZ);
    mDoMtx_stack_c::transM(0.0f, 0.5f, 0.0f);
    mDoMtx_XrotM(matrix, 0x4000);
    offset->set(src->m04, 0.0f, src->m00);
    PSMTXMultVec(matrix, offset.get(), start.get());
    PSVECAdd(start.get(), &actor->current.pos, start.get());
    cXyz_pl(start.get(), tmp.get(), dir.get());
    check->mActorPid = actor->mBsPcId;
    finish->copy(*tmp.get());
    check->Set(start.get(), finish.get(), const_cast<Act_c*>(actor));
    return moveboxLineCrossBool(dComIfG_Bgsp(), check);
}

/* 02378D18 */
s32 Act_c::check_to_walk() {
    s32 result = -1;
    bool clear = true;
    if (m64A && (mBgc.mStateFlags & 1) && !(mType == 11 && mChildPID != 0xFFFFFFFF)) {
        u32 label = mPPLabel;
        const Attr_c* a = attr(0x1002D504);
        s16 threshold = (label & 2) ? ((label & 4) ? (s16)a->m06 : (s16)a->m08)
                                    : ((label & 4) ? (s16)a->m00 : (s16)a->m02);
        for (s32 i = 0; i < 4; i++) {
            if (mMomentCnt[i] < threshold)
                clear = false;
            else {
                const BgcSrc_c* src = attr(0x1002D504)->m9A ? Bgc_c::M_lin20 : Bgc_c::M_lin5;
                bool extended = attr(0x1002D504)->m9A != 0;
                s32 count = extended ? ((s32)mType == 10 ? 23 : 21) : 5;
                s16 angle = M_dir_base[i];
                if (!mBgc.chk_wall_pre(this, src, count, angle))
                    result = i;
            }
        }
    }
    if (clear)
        clr_moment_cnt();
    m64A = 0;
    return result;
}

/* 02379308 */
void Act_c::mode_wait() {
    if (m646 > 0)
        m646 = m646 - 1;
    s32 direction = check_to_walk();
    if (!m64F && (mBgc.mStateFlags & 1)) {
        m64F = 1;
        path_save();
    }
    const Attr_c* a = attr(0x1002D5C0);
    const Attr_c* b = attr(0x1002D5C0);
    daObj_posMoveF_stream(this, 0, &cXyz::Zero, (f32)a->m18, (f32)b->m1C);
    auto* matrix = mDoMtx_stack_c::get();
    PSMTXTrans(matrix, (f32)home.pos.x, (f32)home.pos.y, (f32)home.pos.z);
    mDoMtx_YrotM(matrix, (s16)home.angle.y);
    a = attr(0x1002D5C0);
    b = attr(0x1002D5C0);
    f32 x = (f32)(s32)m628 * (f32)a->m0C, z = (f32)(s32)m62C * (f32)b->m0C;
    mDoMtx_stack_c::transM(x, 0.0f, z);
    gabi::Local<cXyz> pos;
    PSMTXMultVec(matrix, &cXyz::Zero, pos.get());
    current.pos.x = pos->x;
    current.pos.z = pos->z;
    if (direction != -1) {
        m634 = direction;
        eff_smoke_slip_start();
        daPy_py_c* player = static_cast<daPy_py_c*>(dComIfGp_getPlayer(0));
        player->onNoResetFlg0(daPy_py_c::daPyFlg0_PUSH_PULL_KEEP);
        mode_walk_init();
        if (mPPLabel & 2) {
            m644 = attr(0x1002D5C0)->m0A;
            m630 = 32768.0f / (f32)(s16)attr(0x1002D5C0)->m0A;
        } else {
            m644 = attr(0x1002D5C0)->m04;
            m630 = 32768.0f / (f32)(s16)attr(0x1002D5C0)->m04;
        }
    }
}

/* 02379D00 */
void Act_c::afl_sway() {
    f32 z = m610, x = m60C, mag = gabi::fmadds(x, x, z * z);
    const Attr_c* a = attr(0x1002D6A4);
    const Attr_c* b = attr(0x1002D6A4);
    f32 max = (f32)a->m4C * (f32)b->m4C;
    const BgcSrc_c* src = attr(0x1002D6A4)->m9A ? Bgc_c::M_lin20 : Bgc_c::M_lin5;
    s32 count = attr(0x1002D6A4)->m9A ? 21 : 5;
    bool wallZ = mBgc.chk_wall_touch2(this, src, count, M_dir_base[0]);
    if (!wallZ)
        wallZ = mBgc.chk_wall_touch2(this, src, count, M_dir_base[2]);
    bool wallX = mBgc.chk_wall_touch2(this, src, count, M_dir_base[1]);
    if (!wallX)
        wallX = mBgc.chk_wall_touch2(this, src, count, M_dir_base[3]);
    f32 dx, dz;
    if (mag > max) {
        a = attr(0x1002D6A4);
        f32 distance = std_sqrtf(mag);
        f32 ratio = (f32)a->m4C / distance;
        x = (f32)m60C * ratio;
        z = (f32)m610 * ratio;
        f32 oldX = m614;
        m60C = x;
        f32 oldZ = m618;
        m610 = z;
        dx = oldX - x;
        dz = oldZ - z;
        a = attr(0x1002D6A4);
    } else {
        dx = (f32)m614 - (f32)m60C;
        dz = (f32)m618 - (f32)m610;
        a = attr(0x1002D6A4);
    }
    f32 forceX = -(dx * (f32)a->m50);
    f32 forceZ = -(dz * (f32)attr(0x1002D6A4)->m50);
    a = attr(0x1002D6A4);
    f32 velocityX = m61C;
    f32 dampX = -(velocityX * (f32)a->m54);
    bool reload = (u32)mType >= 13;
    a = attr(0x1002D6A4);
    if (reload)
        velocityX = m61C;
    f32 velocityZ = m620;
    f32 sumX = forceX + dampX;
    f32 sumZ = gabi::fnmsubs(velocityZ, a->m54, forceZ);
    velocityX = velocityX + sumX;
    velocityZ = velocityZ + sumZ;
    f32 oldX = m614, oldZ = m618;
    m61C = velocityX;
    m620 = velocityZ;
    m614 = oldX + velocityX;
    m618 = oldZ + velocityZ;
    if (wallX)
        m614 = 0.0f;
    if (wallZ)
        m618 = 0.0f;
    m60C = 0.0f;
    m610 = 0.0f;
}

/* 0237A0A8 */
void Act_c::mode_afl() {
    f32 immersion = (f32)current.pos.y - (f32)mBgc.mWaterY;
    const Attr_c* a;
    if (immersion < 0.0f) {
        a = attr(0x1002D6DC);
        if (!(immersion > -(f32)a->m68))
            immersion = 1.0f;
        else
            immersion = -(immersion * (f32)attr(0x1002D6DC)->m6C);
    } else
        immersion = 0.0f;
    a = attr(0x1002D6DC);
    f32 period = (f32)(s16)a->m38;
    f32 random = cM_rnd();
    m604 = (s16)(m604 + (s16)gabi::ftoi(period * (random + 1.0f)));
    const Attr_c* buoy = attr(0x1002D6DC);
    const Attr_c* grav = attr(0x1002D6DC);
    const Attr_c* wave = attr(0x1002D6DC);
    u16 phase = m604;
    f32 g = grav->m14, b = buoy->m28, w = wave->m34;
    f32 sin = cM_ssin(phase);
    f32 base = gabi::fmadds(immersion, b, g);
    f32 force = gabi::fmadds(w, sin, base);
    f32 down = m608;
    m608 = 0.0f;
    gravity = force + down;
    afl_sway();
    f32 air = 1.0f - immersion;
    a = attr(0x1002D6DC);
    const Attr_c* friction = attr(0x1002D6DC);
    f32 drag = gabi::fmadds(a->m3C, immersion, (f32)friction->m18 * air);
    a = attr(0x1002D6DC);
    friction = attr(0x1002D6DC);
    f32 resistance = gabi::fmadds(a->m40, immersion, (f32)friction->m1C * air);
    f32 depth = (f32)mBgc.mWaterY - (f32)current.pos.y;
    if (depth < 0.0f)
        m624 = 0.0f;
    else {
        m624 = depth;
        bool reload = (u32)mType >= 13;
        a = attr(0x1002D6DC);
        if (reload)
            depth = m624;
        if (depth > a->m68)
            m624 = attr(0x1002D6DC)->m68;
    }
    daObj_posMoveF_stream(this, 0, &cXyz::Zero, drag, resistance);
}

/* 02379834 */
void Act_c::mode_walk() {
    if (!l_statics.mWalkDirectionGuard) {
        const f32 values[12] = {0, 0, 1, 1, 0, 0, 0, 0, -1, -1, 0, 0};
        for (s32 i = 0; i < 4; i++)
            l_statics.mWalkDirection[i].set(values[i * 3], values[i * 3 + 1], values[i * 3 + 2]);
        l_statics.mWalkDirectionGuard = 1;
    }
    s16 counter = (s16)(m644 - 1);
    u16 phase = (u16)gabi::ftoi((f32)counter * (f32)m630);
    m644 = counter;
    f32 blend = (cM_scos(phase) + 1.0f) * 0.5f;
    bool finished = counter <= 0;
    auto* matrix = mDoMtx_stack_c::get();
    PSMTXTrans(matrix, (f32)home.pos.x, (f32)home.pos.y, (f32)home.pos.z);
    mDoMtx_YrotM(matrix, (s16)home.angle.y);
    const Attr_c* a = attr(0x1002D66C);
    const Attr_c* b = attr(0x1002D66C);
    cXyz* vec = &l_statics.mWalkDirection[(s32)m634];
    f32 x = gabi::fmadds(vec->x, blend, (f32)(s32)m628);
    f32 z = gabi::fmadds(vec->z, blend, (f32)(s32)m62C);
    mDoMtx_stack_c::transM(x * (f32)a->m0C, 0.0f, z * (f32)b->m0C);
    gabi::Local<cXyz> pos;
    PSMTXMultVec(matrix, &cXyz::Zero, pos.get());
    x = pos->x;
    z = pos->z;
    f32 y = current.pos.y;
    current.pos.x = x;
    eyePos.y = y;
    eyePos.z = z;
    eyePos.x = x;
    current.pos.z = z;
    sound_slip();
    if (finished) {
        const BgcSrc_c* src = attr(0x1002D66C)->m9A ? Bgc_c::M_lin20 : Bgc_c::M_lin5;
        bool extended = attr(0x1002D66C)->m9A != 0;
        s32 count = extended ? ((s32)mType == 10 ? 23 : 21) : 5;
        s16 angle = M_dir_base[(s32)m634];
        if (mBgc.chk_wall_pre(this, src, count, angle))
            sound_limit();
        eff_smoke_slip_end();
    } else
        eff_set_slip_smoke_pos();
    a = attr(0x1002D66C);
    b = attr(0x1002D66C);
    daObj_posMoveF_stream(this, 0, &cXyz::Zero, (f32)a->m18, (f32)b->m1C);
    current.pos.x = pos->x;
    current.pos.z = pos->z;
    if (finished) {
        s32 direction = m634;
        if (direction == 0)
            m62C = m62C + 1;
        else if (direction == 1)
            m628 = m628 + 1;
        else if (direction == 2)
            m62C = m62C - 1;
        else if (direction == 3)
            m628 = m628 - 1;
        daPy_py_c* player = static_cast<daPy_py_c*>(dComIfGp_getPlayer(0));
        player->offNoResetFlg0(daPy_py_c::daPyFlg0_PUSH_PULL_KEEP);
        mode_wait_init();
    }
}

/* 02378638 */
BOOL Act_c::Execute(gptr<Mtx34>* output) {
    if (mCyl.ChkTgHit() || mbRollCrash) {
        make_item();
        eff_break();
        sound_break();
        fopAcM_delete(this);
        *output = &mMtx;
        return TRUE;
    }
    if (mBgc.mStateFlags & 32) {
        const Attr_c* a = attr(0x1002D490);
        s8 reverb = mReverb;
        mDoAud_seStart(a->mWaterFallSE, &eyePos, 0, reverb);
    }
    s32 mode = mMode;
    if (mode == 0) {
        u32 flags = mBgc.mStateFlags;
        if ((flags & 16) && m646 == 0) {
            bool floatUp = !(flags & 1);
            if (!floatUp) {
                const Attr_c* a = attr(0x1002D490);
                const Attr_c* b = attr(0x1002D490);
                floatUp = ((f32)a->m28 + (f32)b->m14) > 0.0f;
            }
            if (floatUp)
                mode_afl_init();
        }
    } else if (mode == 2 && (mBgc.mStateFlags & 1)) {
        mode_wait_init();
        m646 = 20;
    }
    if (!l_modeGuard) {
        l_modeGuard = 1;
        memcpy_g(&l_modeTable, &l_modeInitialTable, sizeof(l_modeTable));
    }
    ptmf_call(gabi::ea(&l_modeTable.mWords[2 * (s32)mMode]), this);
    mBgc.proc_vertical(this);
    if (mBgc.mMaxGroundIdx >= 0) {
        tevStr.mRoomNo = current.roomNo;
        dBgS* bg = dComIfG_Bgsp();
        dBgS_ObjGndChk* check = &Bgc_c::M_gnd_work[(s32)mBgc.mMaxGroundIdx];
        u32 color = moveboxPolyColor(bg, check ? static_cast<cBgS_PolyInfo*>(check) : nullptr);
        tevStr.mEnvrIdxOverride = color;
    }
    if (m648 > 0) {
        m648 = m648 - 1;
        set_mtx();
    } else {
        if (mBgc.mStateFlags & 4) {
            sound_land();
            vib_land();
            if (!(mBgc.mStateFlags & 16))
                eff_land_smoke();
        }
        set_mtx();
    }
    if (mType == 0 || mType == 5) {
        moveboxMoveCollisionCenter(&mCyl, &current.pos);
        cCcS_Set(dComIfG_Ccsp(), &mCyl);
    }
    *output = &mMtx;
    return TRUE;
}

/* 02377764 */
void Act_c::set_mtx() {
    bool floating = mMode == 2;
    auto* matrix = mDoMtx_stack_c::get();
    PSMTXTrans(matrix, (f32)current.pos.x, (f32)current.pos.y, (f32)current.pos.z);
    if (floating) {
        gabi::Local<cXyz> vec;
        vec->set(m614, 1.0f, m618);
        mDoMtx_stack_c::transM(0.0f, (f32)m624, 0.0f);
        gabi::Local<Quaternion> quat;
        moveboxQuaternion(quat.get(), vec.get());
        mDoMtx_stack_c::quatM(quat.get());
    }
    mDoMtx_ZXYrotM(matrix, (s16)shape_angle.x, (s16)shape_angle.y, (s16)shape_angle.z);
    if (floating)
        mDoMtx_stack_c::transM(0.0f, -(f32)m624, 0.0f);
    if (mpModel != nullptr) {
        f32 values[12];
        for (s32 i = 0; i < 12; i++)
            values[i] = matrix->m[i / 4][i % 4];
        for (s32 i = 0; i < 12; i++)
            mpModel->mBaseTransformMtx.m[i / 4][i % 4] = values[i];
    }
    PSMTXCopy(matrix, &mMtx);
    if (mChildPID == 0xFFFFFFFF)
        return;
    gabi::Local<be<u32>> found;
    if (!moveboxFindActor((u32)mChildPID, found.get())) {
        mChildPID = 0xFFFFFFFF;
        return;
    }
    MoveboxChildHD* child = gabi::at<MoveboxChildHD>(*found.get());
    if (!child)
        return;
    s32 type = mType;
    if (type == 7 || type == 10) {
        f32 distance = moveboxSquareDistance(&child->mPosition, &current.pos);
        if (distance > 0.00009999999747378752f) {
            child->mPosition.x = current.pos.x;
            child->mPosition.y = current.pos.y;
            f32 z = current.pos.z;
            if (type == 7)
                child->mSpringSetup = 1;
            else
                child->mMirrorSetup = 1;
            child->mPosition.z = z;
        }
    } else if (type == 11) {
        gabi::Local<cXyz> pos;
        pos->set(current.pos.x, current.pos.y + 150.0f, current.pos.z);
        f32 distance = moveboxSquareDistance(&child->mPosition, pos.get());
        if (distance > 0.00009999999747378752f) {
            child->mPosition.x = pos->x;
            child->mPosition.y = pos->y;
            f32 z = pos->z;
            child->mMkieSetup = 1;
            child->mPosition.z = z;
        }
    } else {
        f32 height = attr(0x1002D35C)->m68 - 5.0f;
        gabi::Local<cXyz> offset, pos;
        offset->set(0.0f, height, 0.0f);
        PSMTXMultVecSR(matrix, offset.get(), pos.get());
        f32 x = pos->x, y = pos->y, z = pos->z;
        f32 mx = matrix->m[0][3], my = matrix->m[1][3], mz = matrix->m[2][3];
        matrix->m[0][3] = mx + x;
        matrix->m[1][3] = my + y;
        matrix->m[2][3] = mz + z;
        PSMTXCopy(matrix, &gabi::at<MoveboxChildHD>(*found.get())->mBuoyMtx);
    }
}

/* 02377C1C */
BOOL Act_c::Create() {
    m608 = 0.0f;
    m60C = 0.0f;
    m610 = 0.0f;
    m614 = 0.0f;
    m618 = 0.0f;
    m61C = 0.0f;
    m604 = 0;
    m620 = 0.0f;
    f32 height = attr(0x1002D3D8)->m68;
    m628 = 0;
    m62C = 0;
    m634 = -1;
    mPPLabel = 0;
    m624 = height * 0.5f;
    m630 = 0.0f;
    clr_moment_cnt();
    m64A = 0;
    m644 = 0;
    m646 = 0;
    m648 = 20;
    backgroundHD(mpBgW)->mRideCallback = MOVEBOX_RIDE_CALLBACK;
    backgroundHD(mpBgW)->mPushPullCallback = MOVEBOX_PUSH_PULL_CALLBACK;
    mStts.Init(255, 255, this);
    mCyl.Set(reinterpret_cast<const dCcD_SrcCyl*>(&l_cylinderSource));
    u32 co = mCyl.mObjCo.mSPrm;
    mCyl.mStts = &mStts;
    mCyl.mGObjTg.mVec.copy(cXyz::Zero);
    u32 tg = mCyl.mGObjTg.mSPrm;
    cullMtx = gabi::ea(&mMtx);
    mCyl.mObjCo.mSPrm = co & ~1u;
    mCyl.mGObjTg.mSPrm = tg | 4;
    const Attr_c* a[6];
    for (s32 i = 0; i < 6; i++)
        a[i] = attr(0x1002D3D8);
    fopAcM_setCullSizeBox(this, (f32)(s16)a[0]->mCullMinX, (f32)(s16)a[1]->mCullMinY,
                          (f32)(s16)a[2]->mCullMinZ, (f32)(s16)a[3]->mCullMaxX, (f32)(s16)a[4]->mCullMaxY,
                          (f32)(s16)a[5]->mCullMaxZ);
    speedF = 0.0f;
    gravity = attr(0x1002D3D8)->m14;
    fopAcM_posMoveF(this, 0);
    mBgc.proc_vertical(this);
    s8 room = home.roomNo;
    u32 flags = mBgc.mStateFlags;
    m64F = 1;
    mBgc.mStateFlags = flags & ~0x26u;
    mReverb = dComIfGp_getReverb(room);
    mChildPID = 0xFFFFFFFF;
    s32 noBuoy = prm(1, 31);
    s32 type = mType;
    if (!noBuoy) {
        f32 x = current.pos.x;
        f32 h = attr(0x1002D3D8)->m68;
        f32 y = current.pos.y;
        s8 childRoom = current.roomNo;
        gabi::Local<cXyz> pos;
        pos->set(x, (y + h) - 5.0f, current.pos.z);
        mChildPID = moveboxCreateChild(0x1D1, (u32)mBsPcId, 0, pos.get(), childRoom, &shape_angle, 0, -1, 0);
    } else if (type == 7 || type == 10) {
        s8 childRoom = current.roomNo;
        u32 pid = (u32)mBsPcId;
        mChildPID = moveboxCreateChild(type == 7 ? 0xA6 : 0x110, pid, type == 7 ? 1 : 0, &current.pos,
                                       childRoom, &shape_angle, 0, -1, 0);
    } else if (type == 11) {
        u32 low = mPrmX & 255;
        gabi::Local<cXyz> pos;
        pos->set(current.pos.x, current.pos.y + 150.0f, current.pos.z);
        u32 second = mPrmZ >> 8;
        u32 params = low | 0x3000 | (second << 16);
        s8 childRoom = current.roomNo;
        u32 pid = (u32)mBsPcId;
        mChildPID = moveboxCreateChild(0x4B, pid, params, pos.get(), childRoom, &shape_angle, 0, -1, 0);
    }
    init_mtx();
    mbRollCrash = 0;
    mode_wait_init();
    return TRUE;
}

} // namespace daObjMovebox

using namespace daObjMovebox;
/* WWHD glue
ext 02333CD8 s32 moveboxGetParameterBits(const fopAc_ac_c* actor,s32 width,s32 shift)
data l_registrations 101CB1C0
data l_baseY 101FFBC0
data l_cylinderSource 1002D758
data l_modeGuard 101FDC24
data l_modeTable 101FDC28
data l_modeInitialTable 1002CF94
ext 025AAF88 daObjMovebox::MoveboxPathHD* moveboxGetRoomPath(s32 index,s32 room)
ext 025AAEB8 daObjMovebox::MoveboxPathPointHD* moveboxGetPathPoint(daObjMovebox::MoveboxPathHD* path,s32 index)
ext 02008438 fopAc_ac_c* moveboxGroundActor(dBgS* manager,u16 index)
ext 024EFBC0 void moveboxRideCallback(dBgS* manager,cBgS_PolyInfo* poly,daObjMovebox::Act_c* actor)
ext 024EEEB8 u32 moveboxPolyColor(dBgS* manager,cBgS_PolyInfo* poly)
ext 024EF7C0 BOOL moveboxWaterCheck(dBgS* manager,daObjMovebox::MoveboxWaterWorkHD* check)
ext 023123D8 void moveboxQuaternion(Quaternion* quaternion,cXyz* direction)
ext 025D54C4 BOOL moveboxFindActor(u32 id,be<u32>* output)
ext 025D5834 fpc_ProcID moveboxCreateActor(s32 name,u32 parameters,cXyz* position,s32 room,csXyz* angles,cXyz* scale,s32 setId,u32 extra)
ext 025D5A20 fpc_ProcID moveboxCreateChild(s32 name,u32 parent,u32 parameters,cXyz* position,s32 room,csXyz* angles,cXyz* scale,s32 setId,u32 extra)
data l_attributes 101CB424
data l_archiveNames 1002D724
data l_lineSources 101CB264
data l_directionBase 1004BCE8
data l_statics 1046A680
ext 02008860 bool moveboxLineCrossBool(dBgS* manager,dBgS_LinChk* check)
data l_collisionWork 1046A77C
ext 025F18EC void moveboxXrotS(Mtx34* matrix,s16 angle)
ext 025E2DE0 void moveboxUpdateModel(J3DModel* model,u32 flags)
ext 02516680 void moveboxMoveCollisionCenter(dCcD_Cyl* cylinder,cXyz* position)
ext 02008DAC void moveboxGroundBaseDestroy(dBgS_GndChk* check,s32 flags)
ext 028E8DE8 f32 moveboxSquareDistance(const cXyz* first,const cXyz* second)
*/
