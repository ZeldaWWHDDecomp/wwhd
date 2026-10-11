/* Ghost Ship: moon-phase visibility, circular effects, path movement and translucent cloth.
 * GameCube names follow zeldaret/tww. HD rendering and ABI differences are marked below.
 */
#include "d/actor/d_a_ghostship.h"
#include <cmath>
static f32 l_pathChaseScale;
static f32 l_circleWobble;
static f32 l_pathThreshold;
static f32 l_circleOffset;
static s16 l_circleSpeed;
static f32 l_negativePi;
static f32 l_positivePi;
static u8 l_angleTag;
static u8 l_byteTag;
static ghostshipHD::HeaderZeros l_headerZeros;
static ghostshipHD::DestructorRecord l_headerRecord0;
static ghostshipHD::DestructorRecord l_headerRecord1;
static ghostshipHD::DestructorRecord l_headerRecord2;
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/tu.h"

// HD: GHS emits these inline class vtables separately for each translation unit.
const gc_tu_data gc_tu = {
    .safeString = 0x1000FAE0,
    .objAcch = {0x1000FAF8, 0x1000FB18, 0x1000FB08},
    .objAcchDt = {0x1000FB08, 0x1000FB18},
};
static ghostshipHD::SaveBlock* l_save;
static GXColor l_particleColor;
extern u8 ghostshipGetEventRegByte(dSv_event_c*, u32);
extern void ghostshipAssert(const char*, u32, const char*);
extern BOOL ghostshipInitBtk(mDoExt_btkAnm*, J3DModelData*, J3DAnmTextureSRTKey*, bool, s32, f32,
                             s32, s16, bool, s32);
extern ghostshipHD::Cloth* ghostshipCreateCloth(ResTIMG*, ResTIMG*, s32, s32, f32, f32,
                                                dKy_tevstr_c*, u32);
extern u8* ghostshipGetRoomPath(s32, s8);
extern void ghostshipSetWall(dBgS_AcchCir*, f32, f32);
extern void ghostshipSetAcch(dBgS_ObjAcch*, cXyz*, cXyz*, daGhostship_c*, s32, dBgS_AcchCir*, cXyz*,
                             u32, u32);
extern void ghostshipConstructActor(daGhostship_c*);
extern void ghostshipConstructBtk(mDoExt_btkAnm*);
extern void ghostshipConstructAcch(dBgS_ObjAcch*);
extern void ghostshipConstructCir(dBgS_AcchCir*);
extern s32 ghostshipLoadResource(request_of_phase_process_class*, const char*);
extern void ghostshipDeleteResource(request_of_phase_process_class*, const char*);
extern u32 ghostshipGetEventReg(dSv_event_c*, u32);
extern dComIfG_inf_c* ghostshipGetPlayAddress();
extern f32 ghostshipActorDistanceXZ(daGhostship_c*, fopAc_ac_c*);
extern u32 ghostshipGetMoonPhase();
extern void ghostshipGhostShipCutoff();
extern void ghostshipGhostShipExist();
extern BOOL ghostshipIsOpenCollectMap(dSv_player_map_c*, u32);
extern BOOL ghostshipIsGetCollectMap(dSv_player_map_c*, u32);
extern s32 ghostshipGetReverb(s8);
extern void ghostshipStartSound(u32, cXyz*, u32, s32);
extern void ghostshipSetSimpleParticle(dPa_control_c*, u32, cXyz*, u32, GXColor*, GXColor*, u32);
extern ghostshipHD::SceneList* ghostshipGetSceneList(cXyz*);
extern void ghostshipSetEventReg(dSv_event_c*, u32, u32);
extern void ghostshipSetNextStage(const char*, s32, s32, s32, s32, s32, s32, f32);
extern void ghostshipPlayBtk(mDoExt_btkAnm*);
extern cXyz* ghostshipGetWindAddress();
extern dScnKy_env_light_c* ghostshipGetEnvironmentAddress();
extern void ghostshipSetLighting(dScnKy_env_light_c*, s32, cXyz*, dKy_tevstr_c*);
extern void ghostshipModelLighting(dScnKy_env_light_c*, J3DModel*, dKy_tevstr_c*);
extern void ghostshipEnterBtk(mDoExt_btkAnm*, J3DModelData*, f32);
extern void ghostshipUpdateModel(J3DModel*, s32);
extern void ghostshipPathMove(cXyz*, be<s8>*, u8*, f32, u32, daGhostship_c*);
extern u8* ghostshipAllocate(u32);
extern void ghostshipRegisterDestructor(void*);
extern void ghostshipConstructAngle(void*);
extern void ghostshipConstructByte(void*);
extern void ghostshipDelete(void*);
extern void ghostshipDestroyCircleBase(cM3dGCir_l*, s32);
extern void ghostshipDestroyAcch(dBgS_ObjAcch*, s32);
extern void ghostshipDestroyActor(daGhostship_c*, s32);
extern BOOL ghostshipEntryHeap(daGhostship_c*, u32, u32);
extern ghostshipHD::MaterialTable* ghostshipGetMaterialTable(J3DModelData*);
extern ghostshipHD::FloatColor* ghostshipGetUniform(ghostshipHD::UniformBlock*, u32);
extern void ghostshipConvertColor(ghostshipHD::FloatColor*, ghostshipHD::FloatColor*, f32);
static ghostshipHD::Hio l_HIO;
static Mtx34 l_matrix;
static ghostshipHD::DebugWord l_axisGuard;
static ghostshipHD::DebugWord l_fanGuard;
static ghostshipHD::DebugWord l_vectorGuard;
static ghostshipHD::DebugWord l_axisColor;
static ghostshipHD::DebugWord l_fanColor;
static ghostshipHD::DebugWord l_vectorColor;
static ghostshipHD::DebugWord l_axisColorSource;
static ghostshipHD::DebugWord l_fanColorSource;
static ghostshipHD::DebugWord l_vectorColorSource;
extern void ghostshipCopyDebugWord(void*, const void*, u32);

extern void ghostshipWaveRot(cXyz*, f32, dLib_wave_c*);
extern void ghostshipSetCirclePath(dLib_circle_path_c*);
extern f32 ghostshipGetWaterY(cXyz*, dBgS_ObjAcch*);
extern BOOL ghostshipNormalizeVector(cXyz*);
extern void ghostshipChasePos(cXyz*, cXyz*, f32);
extern void ghostshipAddCalcPosXZ(cXyz*, cXyz*, f32, f32);
extern void ghostshipSetGlobalWind(ghostshipHD::Cloth*, cXyz*);
extern void ghostshipSetClothMtx(ghostshipHD::Cloth*, Mtx34*);
extern void ghostshipMtxTrans(Mtx34*, f32, f32, f32);
extern void ghostshipMtxXYZrotM(Mtx34*, s16, s16, s16);
extern void ghostshipMtxYrotM(Mtx34*, s16);
extern void ghostshipMtxCopy(Mtx34*, Mtx34*);
extern void ghostshipMtxPush();
extern void ghostshipMtxPop();
extern void ghostshipMtxTransM(f32, f32, f32);

static ghostshipHD::ModeProc mode_proc[] = {
    {&daGhostship_c::modeWait},
    {&daGhostship_c::modeRealize},
    {&daGhostship_c::modePathMove},
};

static void ghostCopyWord(be<f32>* destination, const be<f32>* source) {
    reinterpret_cast<ghostshipHD::FloatWord*>(destination)->bits =
        reinterpret_cast<const ghostshipHD::FloatWord*>(source)->bits;
}
/* 02149ED4 */
static BOOL daGhostship_SailVtxFactorCB(u8* cloth, s32 row, s32 column) {
    return column == 0;
}
/* 02149EE0 */
BOOL daGhostship_c::_createHeap() {
    J3DModelData* data = static_cast<J3DModelData*>(
        dComIfG_getObjectRes("Ayush", dRes_INDEX_AYUSH_BDL_AYUSH_e, gc_tu.safeString));
    if (!data)
        ghostshipAssert("d_a_ghostship.cpp", 0x58, "modelData != (0)");
    this->mpModel = mDoExt_J3DModel__create(data, 0, 0x11020203);
    if (!this->mpModel)
        return 0;
    J3DAnmTextureSRTKey* animation = static_cast<J3DAnmTextureSRTKey*>(
        dComIfG_getObjectRes("Ayush", dRes_INDEX_AYUSH_BTK_AYUSH_e, gc_tu.safeString));
    if (!animation)
        ghostshipAssert("d_a_ghostship.cpp", 0x5F, "btk != 0");
    if (!ghostshipInitBtk(&this->mBtk, data, animation, true, 2, 1.0f, 0, (s16)-1, false, 0))
        return 0;
    ResTIMG* texture1 = static_cast<ResTIMG*>(
        dComIfG_getObjectRes("Ayush", dRes_INDEX_AYUSH_BTI_B_GSHIP_HATA_e, gc_tu.safeString));
    ResTIMG* texture2 = static_cast<ResTIMG*>(
        dComIfG_getObjectRes("Ayush", dRes_INDEX_AYUSH_BTI_B_GSHIP_HO_e, gc_tu.safeString));
    ResTIMG* toon = static_cast<ResTIMG*>(
        dComIfG_getObjectRes("Cloth", dRes_INDEX_CLOTH_BTI_CLOTHTOON_e, gc_tu.safeString));
    this->mpCloth = ghostshipCreateCloth(texture1, toon, 5, 5, 700.0f, 350.0f, &this->tevStr, 0);
    ghostshipHD::Cloth* second =
        ghostshipCreateCloth(texture2, toon, 6, 6, 1800.0f, 1000.0f, &this->tevStr, 0);
    bool firstPresent = this->mpCloth != nullptr;
    this->mpCloth2 = second;
    if (!firstPresent || !second)
        return 0;
    second->mpFactorCheckCB =
        gabi::guest_fn(reinterpret_cast<const void*>(daGhostship_SailVtxFactorCB));
    return 1;
}
/* 0214A0D8 */
static BOOL createHeap_CB(daGhostship_c* self) {
    return self->_createHeap();
}
/* 0214A0DC */
BOOL daGhostship_c::_pathMove(cXyz* position, cXyz* current, cXyz* next) {
    ghostCopyWord(&this->mCurPntPos.x, &current->x);
    ghostCopyWord(&this->mCurPntPos.y, &current->y);
    f32 water = this->current.pos.y;
    // HD: reads source Z before replacing Y, preserving overlapping point inputs.
    u32 currentZ = reinterpret_cast<ghostshipHD::FloatWord*>(&current->z)->bits;
    this->mCurPntPos.y = water;
    reinterpret_cast<ghostshipHD::FloatWord*>(&this->mCurPntPos.z)->bits = currentZ;
    ghostCopyWord(&this->mNextPntPos.x, &next->x);
    ghostCopyWord(&this->mNextPntPos.y, &next->y);
    u32 nextZ = reinterpret_cast<ghostshipHD::FloatWord*>(&next->z)->bits;
    this->mNextPntPos.y = water;
    reinterpret_cast<ghostshipHD::FloatWord*>(&this->mNextPntPos.z)->bits = nextZ;
    gabi::Local<cXyz> delta, difference, secondDifference, xz, secondXZ;
    // Reserve the caller linkage area below live guest-stack temporaries.
    gabi::Local<u8[16]> linkage;
    cXyz_mi(&this->mNextPntPos, delta.get(), &this->mCurPntPos);
    if (!ghostshipNormalizeVector(delta.get()))
        return 1;
    f32 x = delta->x, z = delta->z;
    s16 target = cM_atan2s(x, z);
    s16 remaining = cLib_addCalcAngleS(&this->current.angle.y, target, 8, 0x200, 8);
    f32 cosine = cM_scos(remaining);
    f32 step = (f32)this->speedF * std::fabs(cosine);
    ghostshipChasePos(position, &this->mNextPntPos, step);
    cXyz_mi(position, difference.get(), &this->mNextPntPos);
    xz->x = difference->x;
    xz->y = 0;
    xz->z = difference->z;
    f32 distance = std_sqrtf(PSVECSquareMag(xz.get()));
    f32 factor = static_cast<f32>(l_pathThreshold) + 1.0f;
    if (distance < step * factor)
        return 1;
    cXyz_mi(position, secondDifference.get(), &this->mNextPntPos);
    secondXZ->x = secondDifference->x;
    secondXZ->y = 0;
    secondXZ->z = secondDifference->z;
    return std_sqrtf(PSVECSquareMag(secondXZ.get())) == 0.0f;
}
/* 0214A2B4 */
static BOOL pathMove_CB(cXyz* p, cXyz* current, cXyz* next, daGhostship_c* self) {
    return self->_pathMove(p, current, next);
}
/* 0214A2CC */
void daGhostship_c::getArg() {
    u32 parameter = mParameters;
    moonPhase = (u8)parameter;
    mPathNo = (parameter >> 16) & 255;
}
/* 0214A2E0 */
void daGhostship_c::modePathMoveInit() {
    mMode = daGhostship_c::MODE_PATH_MOVE;
}
/* 0214A2EC */
void daGhostship_c::modeWaitInit() {
    mMode = daGhostship_c::MODE_WAIT;
}

// Shared by two inlined HD paths; this is not a guest function call.
static void ghostMatrices(daGhostship_c* self) {
    ghostshipWaveRot(&self->current.pos, 0.0f, &self->mWave);
    self->shape_angle.x = self->mWave.mRotX;
    self->shape_angle.z = self->mWave.mRotZ;
    f32 sx = self->scale.x, sy = self->scale.y, sz = self->scale.z;
    J3DModel* model = self->mpModel;
    model->mBaseScale.x = sx;
    model->mBaseScale.y = sy;
    model->mBaseScale.z = sz;
    Mtx34* matrix = &l_matrix;
    f32 px = self->current.pos.x, py = self->current.pos.y, pz = self->current.pos.z;
    ghostshipMtxTrans(matrix, px, py, pz);
    s16 ax = self->shape_angle.x, az = self->shape_angle.z;
    ghostshipMtxXYZrotM(matrix, ax, 0, az);
    ghostshipMtxYrotM(matrix, (s16)self->shape_angle.y);
    self->mpModel->setBaseTRMtx(matrix);
    ghostshipMtxCopy(self->mpModel->getBaseTRMtx(), matrix);
    ghostshipMtxPush();
    ghostshipMtxTransM(0.0f, 3200.0f, 75.0f);
    ghostshipSetClothMtx((ghostshipHD::Cloth*)self->mpCloth, matrix);
    ghostshipMtxPop();
    ghostshipMtxTransM(-900.0f, 2080.0f, 85.0f);
    ghostshipMtxYrotM(matrix, 0x4000);
    ghostshipSetClothMtx((ghostshipHD::Cloth*)self->mpCloth2, matrix);
}
/* 0214A2F8 */
void daGhostship_c::createInit() {
    ghostCopyWord(&this->mPathPos.x, &this->current.pos.x);
    ghostCopyWord(&this->mPathPos.y, &this->current.pos.y);
    ghostCopyWord(&this->mPathPos.z, &this->current.pos.z);
    for (s32 i = 0; i < 12; i++) {
        auto& circle = this->mPaths[i];
        s16 angle = (s16)(i * 0x2000), oldSpeed = circle.mAngleSpeed;
        circle.mAngle = angle;
        f32 vertical = 600.0f * cM_scos((s16)(angle + oldSpeed));
        s32 direction = 1;
        if (i & 1) {
            vertical = 600.0f * cM_ssin((s16)(angle + oldSpeed));
            direction = -1;
        }
        circle.mRadius = (f32)i * 100.0f;
        circle.mWobbleAmplitude = static_cast<f32>(l_circleWobble) + 300.0f;
        s32 base = 0x100 + static_cast<s16>(l_circleSpeed) + 16 * i;
        f32 distance = static_cast<f32>(l_circleOffset) + 500.0f;
        f32 random = cM_rndF(100.0f);
        circle.mAngleSpeed = (s16)gabi::ftoi(((f32)base + random) * (f32)direction);
        circle.mTranslation.x =
            gabi::fmadds(distance, cM_ssin(this->shape_angle.y), (f32)this->current.pos.x);
        f32 y = ((f32)this->current.pos.y + 600.0f) + vertical;
        random = cM_rndF(100.0f);
        circle.mTranslation.y = y + random;
        circle.mTranslation.z =
            gabi::fmadds(distance, cM_scos(this->shape_angle.y), (f32)this->current.pos.z);
        ghostshipSetCirclePath(&circle);
    }
    if (this->mPathNo != 255) {
        this->mPath = ghostshipGetRoomPath((s32)this->mPathNo, (s8)this->current.roomNo);
        this->modePathMoveInit();
    } else
        this->modeWaitInit();
    ghostshipSetWall(&this->mCir, 30.0f, 30.0f);
    ghostshipSetAcch(&this->mAcch, &this->current.pos, &this->old.pos, this, 1, &this->mCir,
                     &this->speed, 0, 0);
    this->mAcch.m_flags = (u32)this->mAcch.m_flags | dBgS_Acch::WALL_NONE | dBgS_Acch::ROOF_NONE;
    ghostMatrices(this);
}
/* 0214A758 */
static BOOL daGhostship_Create(daGhostship_c* self) {
    if (!((u32)self->actor_condition & fopAcCnd_INIT_e)) {
        if (self) {
            ghostshipConstructActor(self);
            self->__vtbl = 0x1000FB28; // HD: this actor vtable is local to the unit.
            ghostshipConstructBtk(&self->mBtk);
            ghostshipConstructAcch(&self->mAcch);
            static_cast<cBgS_Chk*>(&self->mAcch)->__vtbl = gc_tu.objAcch.v10;
            static_cast<dBgS_GrpPassChk*>(&self->mAcch)->__vtbl = gc_tu.objAcch.v20;
            static_cast<dBgS_PolyPassChk*>(&self->mAcch)->__vtbl = gc_tu.objAcch.v14;
            self->mAcch.mbObjThrough = 1;
            ghostshipConstructCir(&self->mCir);
        }
        self->actor_condition = (u32)self->actor_condition | fopAcCnd_INIT_e;
    }
    s32 phase = ghostshipLoadResource(&self->mPhs, "Ayush");
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    phase = ghostshipLoadResource(&self->mClothPhs, "Cloth");
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    self->getArg();
    ghostshipHD::SaveBlock* save = l_save;
    if (ghostshipGetEventRegByte(&save->save.mSavedata.mEvent, 0x8803) == 3)
        return cPhs_ERROR_e;
    if (!ghostshipEntryHeap(self, gabi::guest_fn(reinterpret_cast<const void*>(createHeap_CB)),
                            0x1EA0))
        return cPhs_ERROR_e;
    self->createInit();
    return cPhs_COMPLEATE_e;
}
/* 0214A8B8 */
static BOOL daGhostship_Delete(daGhostship_c* self) {
    ghostshipDeleteResource(&self->mPhs, "Ayush");
    ghostshipDeleteResource(&self->mClothPhs, "Cloth");
    ghostshipHD::Cloth* cloth = self->mpCloth;
    if (cloth) {
        cloth->destroy(3);
    }
    cloth = self->mpCloth2;
    self->mpCloth = nullptr;
    if (cloth) {
        cloth->destroy(3);
    }
    self->mpCloth2 = nullptr;
    return 1;
}
/* 0214A958 */
void daGhostship_c::modeProcCall() {
    (this->*mode_proc[mMode].fn)();
}

static void ghostClothParameters(ghostshipHD::Cloth* cloth, f32 vertical) {
    cloth->mWaveSpeed = 0x100;
    cloth->field_0xF2 = 0;
    cloth->mRipple = 900;
    cloth->mSpring = 0.45f;
    cloth->mGravity = vertical;
    cloth->mDrag = 0.875f;
    cloth->mFlyFlex = 1.0f;
    cloth->mRotateY = static_cast<u16>(-800);
    cloth->mWindSpeed = 7.0f;
    cloth->mWindSpeedWave = 6.0f;
    cloth->mHoistFlex = 1.0f;
}

static void ghostClothMove(ghostshipHD::Cloth* cloth) {
    cloth->cloth_move();
}
/* 0214A9A4 */
BOOL daGhostship_c::_execute() {
    ghostshipHD::SaveBlock* save = l_save;
    f32 time = static_cast<f32>(save->save.mSavedata.mPlayer.mPlayerStatusB.mTime);
    dComIfG_inf_c* play = ghostshipGetPlayAddress();
    fopAc_ac_c* player = play->play.mPlayerInfo[0].mpPlayer;
    f32 distance = ghostshipActorDistanceXZ(this, player);
    this->mbCanEnterShip = 0;
    u32 moon = ghostshipGetMoonPhase();
    // HD: unordered time takes the HD ble branch and remains in the active path.
    if ((u32)this->moonPhase != moon || (time > 90.0f && time < 285.0f)) {
        ghostshipGhostShipCutoff();
        this->mAlpha = 0;
        this->actor_status = (u32)this->actor_status & ~(3u | fopAcStts_SHOWMAP_e);
    } else {
        ghostshipGhostShipExist();
        f32 hideDistance = static_cast<f32>(l_HIO.shipHideDist);
        save = l_save;
        f32 target = distance < hideDistance ? 0.0f : static_cast<f32>(l_HIO.shipAlpha);
        // HD: the collectible map index is 0x23 here (GameCube 0x24).
        if (ghostshipIsOpenCollectMap(&save->save.mSavedata.mPlayer.mMap, 0x23)) {
            save = l_save;
            if (ghostshipIsGetCollectMap(&save->save.mSavedata.mPlayer.mMap, 0x23)) {
                this->mbCanEnterShip = 1;
                target = static_cast<f32>(l_HIO.shipAlpha);
            }
        }
        cLib_chaseF(&this->mAlpha, target, 0.02f);
        if (this->mAlpha == 0.0f)
            this->actor_status = (u32)this->actor_status & ~(3u | fopAcStts_SHOWMAP_e);
        else {
            s32 reverb = ghostshipGetReverb((s8)this->current.roomNo);
            ghostshipStartSound(0x107A, &this->current.pos, 0, reverb);
            this->actor_status = (u32)this->actor_status | (3u | fopAcStts_SHOWMAP_e);
        }
    }
    for (s32 i = 0; i < 12; i++) {
        auto& circle = this->mPaths[i];
        f32 radius = circle.mRadius, target;
        // HD: bge takes unordered inputs; only an ordered radius below 900 selects this target.
        if (radius < 900.0f) {
            target = gabi::fmadds((f32)i, 100.0f, 1500.0f);
            this->field_0x6C4[i] = target;
        } else if (radius > gabi::fmadds((f32)i, 100.0f, 1400.0f)) {
            target = 800.0f;
            this->field_0x6C4[i] = target;
        } else
            target = this->field_0x6C4[i];
        s32 phase = (s32)(s16)circle.mAngle + (s32)(s16)circle.mAngleSpeed;
        s16 doubled = (s16)((phase * 2) & 0xFFFE);
        f32 vertical = 600.0f * cM_scos(doubled);
        s32 direction = 1;
        if (i & 1) {
            vertical = 600.0f * cM_ssin(doubled);
            direction = -1;
        }
        cLib_addCalc2(&circle.mRadius, target, 0.1f, 10.0f);
        circle.mWobbleAmplitude = static_cast<f32>(l_circleWobble) + 300.0f;
        s32 base = static_cast<s16>(l_circleSpeed) + 2 * i + 5;
        f32 offset = static_cast<f32>(l_circleOffset) + 500.0f;
        f32 random = cM_rndF(20.0f);
        circle.mAngleSpeed = (s16)gabi::ftoi(((f32)base + random) * (f32)direction);
        circle.mTranslation.x =
            gabi::fmadds(offset, cM_ssin(this->shape_angle.y), (f32)this->current.pos.x);
        circle.mTranslation.y = ((f32)this->current.pos.y + 600.0f) + vertical;
        circle.mTranslation.z =
            gabi::fmadds(offset, cM_scos(this->shape_angle.y), (f32)this->current.pos.z);
        ghostshipSetCirclePath(&circle);
        if (this->mAlpha != 0.0f) {
            play = ghostshipGetPlayAddress();
            dPa_control_c* particles = play->play.mParticle;
            ghostshipSetSimpleParticle(particles, 0x8306, &circle.mPos, 255, &l_particleColor,
                                       &l_particleColor, 0);
        }
    }
    if (this->mAlpha == static_cast<f32>(l_HIO.shipAlpha) &&
        distance < static_cast<f32>(l_HIO.shipEnterDist)) {
        save = l_save;
        u32 event = ghostshipGetEventReg(&save->save.mSavedata.mEvent, 0x8803);
        if (event < 3 && !this->mbEnteredShip) {
            ghostshipStartSound(0x2889, 0, 0, 0);
            ghostshipHD::SceneList* scene = ghostshipGetSceneList(&this->current.pos);
            if (!scene)
                ghostshipAssert("d_a_ghostship.cpp", 0x1D3, "scls_data != (0)");
            save = l_save;
            u8 room = scene->room, start = scene->start;
            ghostshipSetEventReg(&save->save.mSavedata.mEvent, 0xC3FF, room);
            save = l_save;
            ghostshipSetEventReg(&save->save.mSavedata.mEvent, 0x85FF, start);
            ghostshipSetNextStage("PShip", 0, 2, -1, 0, 1, 0, 0.0f);
            this->mbEnteredShip = 1;
        }
    }
    this->modeProcCall();
    ghostshipPlayBtk(&this->mBtk);
    ghostMatrices(this);
    ghostshipHD::Cloth* cloth = this->mpCloth;
    ghostClothParameters(cloth, -1.5f);
    cXyz* wind = ghostshipGetWindAddress();
    ghostshipSetGlobalWind((ghostshipHD::Cloth*)this->mpCloth, wind);
    ghostClothMove(this->mpCloth);
    cloth = this->mpCloth2;
    ghostClothParameters(cloth, -0.25f);
    wind = ghostshipGetWindAddress();
    gabi::Local<cXyz> secondWind;
    gabi::Local<u8[16]> linkage;
    secondWind->x = static_cast<f32>(wind->x);
    secondWind->y = static_cast<f32>(wind->y);
    secondWind->z = static_cast<f32>(wind->z);
    PSVECScale(secondWind.get(), secondWind.get(), 0.548f);
    secondWind->y = -0.83f;
    ghostshipSetGlobalWind((ghostshipHD::Cloth*)this->mpCloth2, secondWind.get());
    ghostClothMove(this->mpCloth2);
    return 0;
}
/* 0214B3F4 */
static void ghostNormalize(ghostshipHD::FloatColor* out, GXColor* color) {
    f32 r = (f32)color->r / 255.0f, g = (f32)color->g / 255.0f, b = (f32)color->b / 255.0f,
        alpha = (f32)color->a / 255.0f;
    out->r = r;
    out->g = g;
    out->b = b;
    out->a = alpha;
}
// HD: these compiler guard words cover the debug draw colors.
static void ghostCopyGuard(ghostshipHD::DebugWord* guard, ghostshipHD::DebugWord* destination,
                           const ghostshipHD::DebugWord* source) {
    if (!guard->value) {
        guard->value = 1;
        ghostshipCopyDebugWord(destination, source, 4);
    }
}
/* 0214B4A8 */
BOOL daGhostship_c::_draw() {
    if (l_HIO.field_0x04) {
        ghostCopyGuard(&l_axisGuard, &l_axisColor, &l_axisColorSource);
        ghostCopyGuard(&l_fanGuard, &l_fanColor, &l_fanColorSource);
        ghostCopyGuard(&l_fanGuard, &l_fanColor, &l_fanColorSource);
        ghostCopyGuard(&l_fanGuard, &l_fanColor, &l_fanColorSource);
        ghostCopyGuard(&l_vectorGuard, &l_vectorColor, &l_vectorColorSource);
        ghostCopyGuard(&l_axisGuard, &l_axisColor, &l_axisColorSource);
        ghostCopyGuard(&l_axisGuard, &l_axisColor, &l_axisColorSource);
        ghostCopyGuard(&l_axisGuard, &l_axisColor, &l_axisColorSource);
    }
    if (this->mAlpha == 0.0f || l_HIO.hideShip)
        return 1;
    dScnKy_env_light_c* env = ghostshipGetEnvironmentAddress();
    ghostshipSetLighting(env, 0, &this->current.pos, &this->tevStr);
    env = ghostshipGetEnvironmentAddress();
    ghostshipModelLighting(env, (J3DModel*)this->mpModel, &this->tevStr);
    u8 alpha = (u8)gabi::ftoi(255.5f * (f32)this->mAlpha);
    // HD: convert each material color and update its renderer uniform before drawing.
    J3DModelData* data = this->mpModel->getModelData();
    ghostshipHD::MaterialTable* table = ghostshipGetMaterialTable(data);
    for (u8 i = 0; i < table->mMaterialNum; i = (u8)(i + 1)) {
        auto* material = reinterpret_cast<ghostshipHD::Material*>(data->getMaterialNodePointer(i));
        GXColor* color = material->mTevBlock->getTevKColor(3);
        color->a = alpha;
        color = material->mTevBlock->getTevKColor(3);
        ghostshipHD::TevBlock* block = material->mTevBlock;
        block->setTevKColor(3, color);
        gabi::Local<ghostshipHD::FloatColor> input, converted;
        gabi::Local<u8[16]> linkage;
        ghostNormalize(input.get(), color);
        ghostshipConvertColor(converted.get(), input.get(), 1.0f);
        material->uniforms.flags = static_cast<u32>(material->uniforms.flags) | 0x400;
        ghostshipHD::FloatColor* output = ghostshipGetUniform(&material->uniforms, 10);
        f32 savedAlpha = (f32)color->a / 255.0f;
        f32 r = converted->r, g = converted->g, b = converted->b;
        output->g = g;
        output->b = b;
        output->r = r;
        output->a = savedAlpha;
        table = ghostshipGetMaterialTable(data);
    }
    ghostshipEnterBtk(&this->mBtk, data, (f32)this->mBtk.mFrameCtrl.mFrame);
    ghostshipUpdateModel((J3DModel*)this->mpModel, 0);
    reinterpret_cast<ghostshipHD::ModelDataTail*>(data)->mTextureAnimation = 0;
    ghostshipHD::Cloth* cloth = this->mpCloth;
    this->tevStr.mColorC0.a = alpha;
    cloth->cloth_draw();
    cloth = this->mpCloth2;
    this->tevStr.mColorC0.a = alpha;
    cloth->cloth_draw();
    return 1;
}
/* 0214B834 */
void daGhostship_c::pathMove() {
    cLib_addCalc2(&this->speedF, (f32)this->mPathSpeed, 0.1f, 2.0f);
    ghostshipPathMove(&this->mPathPos, &this->mPathPntNo, (u8*)this->mPath, (f32)this->speedF,
                      gabi::guest_fn(reinterpret_cast<const void*>(pathMove_CB)), this);
    f32 factor = static_cast<f32>(l_pathChaseScale) + 0.01f;
    ghostshipAddCalcPosXZ(&this->current.pos, &this->mPathPos, factor, (f32)this->speedF);
    if (this->speedF != 0.0f && this->mPathSpeed != 0.0f) {
        s16 angle = cLib_targetAngleY(&this->current.pos, &this->mPathPos);
        cLib_addCalcAngleS2(&this->shape_angle.y, angle, 8, 0x100);
    }
}
/* 0214B8FC */
void daGhostship_c::modePathMove() {
    if (this->mPathNo != 255) {
        this->mPathSpeed = 10.0f;
        this->pathMove();
    }
    this->current.pos.y = ghostshipGetWaterY(&this->current.pos, &this->mAcch);
}
/* 0214B954 */
static u8* ghostHioCtor(u8* self) {
    if (!self) {
        self = ghostshipAllocate(0x18);
        if (!self)
            return nullptr;
    }
    auto* hio = reinterpret_cast<ghostshipHD::Hio*>(self);
    hio->shipAlpha = 0.51f;
    hio->shipHideDist = 3000.0f;
    hio->field_0x04 = 0;
    hio->vtable = 0x1000FB38;
    hio->field_0x0C = 5.0f;
    hio->hideShip = 0;
    hio->shipEnterDist = 1500.0f;
    return self;
}
/* 0214B9D0 */
static void ghostStaticInit() {
    l_headerZeros.zero2 = 0;
    l_headerZeros.zero0 = 0;
    l_headerZeros.zero3 = 0;
    l_headerZeros.zero1 = 0;
    ghostshipRegisterDestructor(&l_headerRecord0);
    l_negativePi = -3.1415927410125732f;
    l_positivePi = 3.1415927410125732f;
    ghostshipConstructAngle(&l_angleTag);
    ghostshipRegisterDestructor(&l_headerRecord1);
    ghostshipConstructByte(&l_byteTag);
    ghostshipRegisterDestructor(&l_headerRecord2);
    ghostHioCtor(reinterpret_cast<u8*>(&l_HIO));
}
/* 0214BA70 */
static void ghostStringDtor(u8* self, s32 flags) {
    if (self && (flags & 1))
        ghostshipDelete(self);
}
/* 0214BA84 */
static BOOL daGhostship_IsDelete(daGhostship_c* self) {
    return 1;
}
/* 0214BA8C */
void daGhostship_c::modeWait() {}
/* 0214BA90 */
void daGhostship_c::modeRealize() {}
/* 0214BA94 */
static void ghostActorDtor(daGhostship_c* self, s32 flags) {
    if (self) {
        ghostshipDestroyCircleBase(&self->mCir.m_cir, 2);
        static_cast<dBgS_GrpPassChk*>(&self->mAcch)->__vtbl = gc_tu.objAcch.v20;
        static_cast<dBgS_PolyPassChk*>(&self->mAcch)->__vtbl = gc_tu.objAcch.v14;
        ghostshipDestroyAcch(&self->mAcch, 0);
        ghostshipDestroyActor(self, 0);
        if (flags & 1)
            ghostshipDelete(self);
    }
}
/* 0214BB18 */
static void ghostStringVirtual() {}
/* WWHD glue
str 1000FC84
str 1000FC8C
str 1000FB84 in 02149EE0
str 1000FB98
str 1000FBAC
str 1000FB48 in 0214A9A4
str 1000FB5C
str 1000FAD8
data l_pathChaseScale 1047BCD0
data l_circleWobble 1047BCD4
data l_pathThreshold 1047BCE4
data l_circleOffset 1047BCF8
data l_circleSpeed 1047BD48
data l_negativePi 10463F6C
data l_positivePi 10463F70
data l_angleTag 10463F74
data l_byteTag 10463F75
data l_headerZeros 10463F90
data l_headerRecord0 101B529C
data l_headerRecord1 101B52A8
data l_headerRecord2 101B52B4
data l_save 101F84DC
data l_particleColor 101D5E98
ext 025B8BB0 u8 ghostshipGetEventRegByte(dSv_event_c*, u32)
ext 0273AA24 void ghostshipAssert(const char*, u32, const char*)
ext 025E7CE0 BOOL ghostshipInitBtk(mDoExt_btkAnm*, J3DModelData*, J3DAnmTextureSRTKey*, bool, s32, f32, s32, s16, bool, s32)
ext 0251BE14 ghostshipHD::Cloth* ghostshipCreateCloth(ResTIMG*, ResTIMG*, s32, s32, f32, f32, dKy_tevstr_c*, u32)
ext 025AAF88 u8* ghostshipGetRoomPath(s32, s8)
ext 024EFF44 void ghostshipSetWall(dBgS_AcchCir*, f32, f32)
ext 024F06B4 void ghostshipSetAcch(dBgS_ObjAcch*, cXyz*, cXyz*, daGhostship_c*, s32, dBgS_AcchCir*, cXyz*, u32, u32)
ext 025D4ED0 void ghostshipConstructActor(daGhostship_c*)
ext 025E7C6C void ghostshipConstructBtk(mDoExt_btkAnm*)
ext 024F0474 void ghostshipConstructAcch(dBgS_ObjAcch*)
ext 024EFE94 void ghostshipConstructCir(dBgS_AcchCir*)
ext 02520460 s32 ghostshipLoadResource(request_of_phase_process_class*, const char*)
ext 025204C8 void ghostshipDeleteResource(request_of_phase_process_class*, const char*)
ext 025B8BB0 u32 ghostshipGetEventReg(dSv_event_c*, u32)
ext 025200D4 dComIfG_inf_c* ghostshipGetPlayAddress()
ext 025D6958 f32 ghostshipActorDistanceXZ(daGhostship_c*, fopAc_ac_c*)
ext 02560828 u32 ghostshipGetMoonPhase()
ext 02560C5C void ghostshipGhostShipCutoff()
ext 02560C34 void ghostshipGhostShipExist()
ext 025B80C8 BOOL ghostshipIsOpenCollectMap(dSv_player_map_c*, u32)
ext 025B7F68 BOOL ghostshipIsGetCollectMap(dSv_player_map_c*, u32)
ext 02520540 s32 ghostshipGetReverb(s8)
ext 025E1A40 void ghostshipStartSound(u32, cXyz*, u32, s32)
ext 025A8D40 void ghostshipSetSimpleParticle(dPa_control_c*, u32, cXyz*, u32, GXColor*, GXColor*, u32)
ext 02521678 ghostshipHD::SceneList* ghostshipGetSceneList(cXyz*)
ext 025B8AF4 void ghostshipSetEventReg(dSv_event_c*, u32, u32)
ext 0252012C void ghostshipSetNextStage(const char*, s32, s32, s32, s32, s32, s32, f32)
ext 025E742C void ghostshipPlayBtk(mDoExt_btkAnm*)
ext 0257DAA8 cXyz* ghostshipGetWindAddress()
ext 02555D0C dScnKy_env_light_c* ghostshipGetEnvironmentAddress()
ext 025626A4 void ghostshipSetLighting(dScnKy_env_light_c*, s32, cXyz*, dKy_tevstr_c*)
ext 02562F5C void ghostshipModelLighting(dScnKy_env_light_c*, J3DModel*, dKy_tevstr_c*)
ext 025E7FC4 void ghostshipEnterBtk(mDoExt_btkAnm*, J3DModelData*, f32)
ext 025E2DE0 void ghostshipUpdateModel(J3DModel*, s32)
ext 02587D24 void ghostshipPathMove(cXyz*, be<s8>*, u8*, f32, u32, daGhostship_c*)
ext 0273AD10 u8* ghostshipAllocate(u32)
ext 028F026C void ghostshipRegisterDestructor(void*)
ext 028ED6F8 void ghostshipConstructAngle(void*)
ext 028EAB2C void ghostshipConstructByte(void*)
ext 0273AF40 void ghostshipDelete(void*)
ext 02018034 void ghostshipDestroyCircleBase(cM3dGCir_l*, s32)
ext 024EFD9C void ghostshipDestroyAcch(dBgS_ObjAcch*, s32)
ext 025D50BC void ghostshipDestroyActor(daGhostship_c*, s32)
ext 025D63E8 BOOL ghostshipEntryHeap(daGhostship_c*, u32, u32)
ext 027F3F8C ghostshipHD::MaterialTable* ghostshipGetMaterialTable(J3DModelData*)
ext 027F9F0C ghostshipHD::FloatColor* ghostshipGetUniform(ghostshipHD::UniformBlock*, u32)
ext 0274D458 void ghostshipConvertColor(ghostshipHD::FloatColor*, ghostshipHD::FloatColor*, f32)
data l_HIO 10463F78
data l_matrix 1048D0CC
ext C000A848 void ghostshipCopyDebugWord(void*, const void*, u32)
data l_axisGuard 101FDAC0
data l_fanGuard 101FDA50
data l_vectorGuard 101FDA48
data l_axisColor 101FEBF8
data l_fanColor 101FEBF4
data l_vectorColor 101FEBEC
data l_axisColorSource 1000FAC8
data l_fanColorSource 1000FACC
data l_vectorColorSource 1000FAD0
ext 025872F4 void ghostshipWaveRot(cXyz*, f32, dLib_wave_c*)
ext 02587128 void ghostshipSetCirclePath(dLib_circle_path_c*)
ext 025871F8 f32 ghostshipGetWaterY(cXyz*, dBgS_ObjAcch*)
ext 0201B47C BOOL ghostshipNormalizeVector(cXyz*)
ext 0200F764 void ghostshipChasePos(cXyz*, cXyz*, f32)
ext 0200F268 void ghostshipAddCalcPosXZ(cXyz*, cXyz*, f32, f32)
ext 0251E7A8 void ghostshipSetGlobalWind(ghostshipHD::Cloth*, cXyz*)
ext 0251B638 void ghostshipSetClothMtx(ghostshipHD::Cloth*, Mtx34*)
ext 028E93CC void ghostshipMtxTrans(Mtx34*, f32, f32, f32)
ext 025F19F8 void ghostshipMtxXYZrotM(Mtx34*, s16, s16, s16)
ext 025F1C28 void ghostshipMtxYrotM(Mtx34*, s16)
ext 028E90D4 void ghostshipMtxCopy(Mtx34*, Mtx34*)
ext 025F23EC void ghostshipMtxPush()
ext 025F2468 void ghostshipMtxPop()
ext 025F24E0 void ghostshipMtxTransM(f32, f32, f32)
data mode_proc 1000FBF0
*/
