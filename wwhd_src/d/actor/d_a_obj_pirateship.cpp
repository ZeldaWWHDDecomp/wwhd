/**
 * Object - Pirate ship
 * GameCube names and function order; verified HD behavior retains the sea model and demo changes.
 */
#include "d/actor/d_a_obj_pirateship.h"
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"
using Ship = daObjPirateship::Act_c;
static daObjPirateShipHIO_c l_HIO;
static PirateshipMathStatics l_math;
static PirateshipDescriptors l_descriptors;
static PirateshipDoorOffset l_doorOffset;
static PirateshipOffsets p_offset;
static PirateshipDemoManager l_resourceManager;
static PirateshipDoorParams dr_prm;
static PirateshipStageName l_demoStage;
static PirateshipIndices2 create_idx_tbl_1stIsland_demo;
static PirateshipIndices6 create_idx_tbl_umikz;
static PirateshipIndices6 create_idx_tbl_arriveMj;
static PirateshipIndices4 create_idx_tbl_Yuukaigo;
static PirateshipIndices5 create_idx_tbl_1stIsland;
static PirateshipDemoManager l_demoManager;
namespace dSv_event_flag_c {
enum { UNK_0520 = 0x0520, UNK_0310 = 0x0310, UNK_0001 = 0x0001 };
}
namespace {
// HD: legacy two-word SafeString ABI; checked virtual assure calls remain observable.
struct PirateshipSafeString {
    be<u32> name, vt;
};
void safeCheck(PirateshipSafeString* s) {
    gabi::call_ptr(gabi::at<PirateshipSafeStringVtable>(s->vt)->assureTermination, s);
}
bool stringEqual(u32 a, u32 b) {
    if (a == b)
        return true;
    // HD: checked guest bytes and the original bounded SafeString comparison.
    const be<u8>* left = gabi::at<const be<u8>>(a);
    const be<u8>* right = gabi::at<const be<u8>>(b);
    for (u32 i = 0; i < 0x40001; i++) {
        u8 x = left[i], y = right[i];
        if (x != y)
            return false;
        if (!x)
            return true;
    }
    return false;
}
bool stageEqual(const char* literal) {
    gabi::Local<PirateshipSafeString> a, b;
    a->name = gabi::guest_str(literal);
    a->vt = VT_SAFE_STRING;
    b->name = gabi::ea(dComIfGp_getStartStageName());
    b->vt = VT_SAFE_STRING;
    safeCheck(a.get());
    safeCheck(a.get());
    u32 name = a->name;
    safeCheck(b.get());
    return stringEqual(name, b->name);
}
} // namespace

namespace {
using PirateshipGroundCheck = dBgS_GndChk;
void groundInit(PirateshipGroundCheck* chk, Ship* ship) {
    cBgS_GndChk__ct(chk);
    chk->cBgS_Chk::__vtbl = VT_GROUND_CHECK;
    chk->cBgS_PolyInfo::__vtbl = VT_GROUND_POLY_INFO;
    chk->mbObjThrough = 0;
    chk->mbCamThrough = 0;
    chk->mbLinkThrough = 0;
    chk->mbArrowThrough = 0;
    chk->mbBombThrough = 0;
    chk->mbBoomerangThrough = 0;
    chk->mbRopeThrough = 0;
    chk->dBgS_PolyPassChk::__vtbl = VT_GROUND_POLY_PASS;
    chk->mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk*>(chk));
    chk->mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk*>(chk));
    chk->dBgS_GrpPassChk::__vtbl = VT_GROUND_GROUP_PASS;
    chk->mGrp = 1;
    chk->m_pos.set(ship->current.pos.x, (f32)ship->current.pos.y + 50.f, ship->current.pos.z);
    chk->mFlag &= ~2u;
}
void groundDestroy(PirateshipGroundCheck* chk) {
    chk->dBgS_GrpPassChk::__vtbl = VT_GROUP_PASS_BASE;
    chk->dBgS_PolyPassChk::__vtbl = VT_GROUND_POLY_PASS;
    chk->cBgS_PolyInfo::__vtbl = VT_GROUND_POLY_INFO;
    cBgS_GndChk__dt(chk, 0);
}
bool eventBit(u32 flag) {
    return dComIfGs_isEventBit(flag) != 0;
}
void searchPart(be<u32>& idField, gptr<fopAc_ac_c>& actorField) {
    u32 id = idField;
    gabi::Local<be<u32>> key;
    *key = id;
    actorField = 0;
    fopAc_ac_c* actor = nullptr;
    if (id != 0xFFFFFFFF)
        actor = fopAcIt_Judge(FN_SEARCH_BY_ID, key.get());
    actorField = actor;
}
/* inlined into daObjPirateship::Act_c::_execute and daObjPirateship::Act_c::_create */
BOOL executeShip(Ship* ship) {
    gabi::Local<PirateshipGroundCheck> chk;
    // EABI callees save LR at SP+4; keep that slot below the live check.
    gabi::Local<be<u32>[4]> linkage;
    groundInit(chk.get(), ship);
    dBgS* bg = dComIfG_Bgsp();
    f32 ground = bg->GroundCross(chk.get());
    if (!(ground <= -1000000000.f)) {
        bg = dComIfG_Bgsp();
        s32 room = pirateshipGroundRoom(bg, static_cast<cBgS_PolyInfo*>(chk.get()));
        if ((u32)room >= 64)
            pirateshipAssert("d_a_obj_pirateship.cpp", 0x32C, "0 <= roomNo && roomNo < 64");
        ship->current.roomNo = (s8)room;
    }
    bool demo = ship->demo_move() != 0;
    if (!demo) {
        if (!(u8)l_HIO.m05 && ship->m4A8) {
            dLib_pathMove(&ship->current.pos, &ship->m4A4, ship->m4A8.get(), 3.f,
                          gabi::guest_fn(reinterpret_cast<const void*>(path_move_call_back)), ship);
            s16 bob = ship->m32E;
            s16 target = ((u32)((s32)bob + 0x4000) >= 0x8001) ? (s16)l_HIO.m12 : (s16)l_HIO.m10;
            cLib_addCalcAngleS2(&ship->m330, target, 0x10, 0x300);
            u16 angle = (s16)ship->m32E + (s16)ship->m330;
            ship->m32E = (s16)angle;
            f32 amplitude = (f32)l_HIO.m0C + 10.f;
            ship->current.pos.y =
                gabi::fmadds(amplitude, cM_ssin((s16)angle), (f32)ship->home.pos.y);
        }
        ship->m2CA = (s16)l_HIO.m06;
    }
    // HD: re-register collision only when the demo and arrival event both allow appearance.
    if (!ship->m2CC) {
        if (!demo || !eventBit(dSv_event_flag_c::UNK_0310)) {
            groundDestroy(chk.get());
            return 0;
        }
        if (!eventBit(dSv_event_flag_c::UNK_0001))
            ship->CreateWave();
        ship->m2CC = 1;
        bg = dComIfG_Bgsp();
        bg->Regist(reinterpret_cast<dBgW*>(ship->mpBgW.get()), ship);
    } else if (!ship->m2CD && eventBit(dSv_event_flag_c::UNK_0001)) {
        ship->m2CD = 1;
        ship->pirateCreate(create_idx_tbl_1stIsland_demo.values);
        ship->DeleteWave();
    }
    searchPart(ship->m_sail_id, ship->m2E4);
    searchPart(ship->m2F0, ship->m2EC);
    searchPart(ship->m300, ship->m2FC);
    ship->set_mtx();
    ship->sound_proc();
    ship->SetWave();
    BOOL result = pirateshipMoveBGExecute(ship) != 0;
    groundDestroy(chk.get());
    return result;
}
} // namespace

/* 02384068 */
void ride_call_back(void* bg, void* ship, fopAc_ac_c* rider) {
    rider->actor_status = rider->actor_status & ~fopAcStts_NOCULLEXEC_e;
}

/* 02384078 */
BOOL path_move_call_back(cXyz* position, cXyz* from, cXyz* to, Ship* ship) {
    gabi::Local<cXyz> start, end, direction;
    f32 sx = from->x, tx = to->x, y = ship->current.pos.y, tz = to->z, sz = from->z;
    start->set(sx, y, sz);
    end->set(tx, y, tz);
    *direction = *end - *start;
    if (!cXyz_normalizeRS(direction.get()))
        return 1;
    s16 heading = cM_atan2s((f32)direction->x, (f32)direction->z);
    s16 turn = cLib_addCalcAngleS(&ship->current.angle.y, heading, 0x40, 0x40, 8);
    ship->shape_angle.y = ship->current.angle.y;
    f32 cosine = cM_scos(turn);
    f32 speed = ((f32)l_HIO.m08 + 5.0f) * std::fabs(cosine);
    return cLib_chasePos(position, end.get(), speed) ? 1 : 0;
}

/* 02384690 */
void daObjPirateship::Act_c::set_mtx() {
    f32 x = scale.x, z = scale.z, y = scale.y;
    J3DModel* model = mModel;
    model->mBaseScale.x = x;
    model->mBaseScale.z = z;
    model->mBaseScale.y = y;
    PSMTXTrans(mDoMtx_stack_c::get(), (f32)current.pos.x, (f32)current.pos.y, (f32)current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), (s16)shape_angle.x, (s16)shape_angle.y,
                   (s16)shape_angle.z);
    mModel->setBaseTRMtx(mDoMtx_stack_c::get());
    fopAc_ac_c* door = m304;
    if (door) {
        if (!(u32)l_doorOffset.initialized) {
            l_doorOffset.pos.set(0.f, 400.f, 475.f);
            l_doorOffset.initialized = 1;
            door = m304;
        }
        PSMTXCopy(mModel->getBaseTRMtx(), mDoMtx_stack_c::get());
        gabi::Local<cXyz> pos;
        PSMTXMultVec(mDoMtx_stack_c::get(), &l_doorOffset.pos, pos.get());
        pirateshipDoorSetPos(door, pos.get(), (s16)((s16)shape_angle.y + 0x8000));
    } else {
        u32 id = m308;
        fopAc_ac_c* found = nullptr;
        gabi::Local<be<u32>> searchID;
        *searchID = id;
        if (id != 0xFFFFFFFF)
            found = fopAcIt_Judge(FN_SEARCH_BY_ID, searchID.get());
        m304 = found;
    }
    PSMTXTrans(mDoMtx_stack_c::get(), (f32)current.pos.x, (f32)current.pos.y + 3.f,
               (f32)current.pos.z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), (s16)shape_angle.y);
    mSeaModel->setBaseTRMtx(mDoMtx_stack_c::get());
    mSeaBtk.play();
}

/* 023844DC */
void daObjPirateship::Act_c::pirateCreate(be<s32>* indices) {
    m2CD = 1;
    while ((s32)*indices != -1) {
        PirateshipOffset* entry = &p_offset.entries[(s32)*indices];
        s8 room = tevStr.mRoomNo;
        u32 parent = fopAcM_GetID(this), params = entry->params;
        const char* name = entry->name;
        fopAcM_createChildFromOffset(name, parent, params, &entry->pos, room, &entry->angle, 0, 0);
        indices++;
    }
}

/* 0238418C */
void daObjPirateship::Act_c::partsCreate() {
    u8 variant = mParameters >> 24;
    u32 id = fopAcM_createChild("Psail", fopAcM_GetID(this), 0, &current.pos, (s8)tevStr.mRoomNo,
                                &current.angle, 0, 0);
    m_sail_id = id;
    if (id == 0xFFFFFFFF)
        pirateshipAssert("d_a_obj_pirateship.cpp", 0x123, "(m_sail_id != fpcM_ERROR_PROCESS_ID_e)");
    m2F0 = fopAcM_createChild(fpcNm_PIRATE_FLAG_e, fopAcM_GetID(this), 0, &current.pos,
                              (s8)tevStr.mRoomNo, &current.angle, 0, -1, 0);
    if (variant != PIRATESHIP_ONLY) {
        gabi::Local<cXyz> pos;
        u32 trig = ((u16)current.angle.y >> 3) * 8;
        f32 y = (f32)current.pos.y + 700.f;
        f32 sine = cM_ssin((s16)trig), cosine = cM_scos((s16)trig);
        f32 x = gabi::fmadds(850.f, sine, (f32)current.pos.x),
            z = gabi::fmadds(850.f, cosine, (f32)current.pos.z);
        pos->set(x, y, z);
        u32 catapult = fopAcM_createChild(fpcNm_Obj_Tousekiki_e, fopAcM_GetID(this), 0, pos.get(),
                                          (s8)tevStr.mRoomNo, &current.angle, 0, -1, 0);
        trig = ((u16)current.angle.y >> 3) * 8;
        x = current.pos.x;
        y = (f32)current.pos.y + 838.f;
        cosine = cM_scos((s16)trig);
        z = gabi::fmadds(-788.f, cosine, (f32)current.pos.z);
        sine = cM_ssin((s16)trig);
        x = gabi::fmadds(-788.f, sine, x);
        m2F8 = catapult;
        pos->set(x, y, z);
        m300 = fopAcM_createChild(fpcNm_Kaji_e, fopAcM_GetID(this), 0, pos.get(),
                                  (s8)tevStr.mRoomNo, &current.angle, 0, -1, 0);
    }
    gabi::Local<cXyz> doorPos;
    gabi::Local<csXyz> angle;
    doorPos->set(current.pos.x, current.pos.y, current.pos.z);
    csXyz__ct(angle.get(), 0xFFF, (s16)((s16)current.angle.y + 0x8000), 0);
    u32 doorType = (mParameters >> 8) & 255;
    if (doorType > 1)
        doorType = 0;
    bool unlocked = dComIfGs_isEventBit(dSv_event_flag_c::UNK_0520) != 0;
    angle->z = unlocked ? 0x1B18 : 0;
    u32 trig = ((u16)current.angle.y >> 3) * 8;
    f32 x = gabi::fmadds(475.f, cM_ssin((s16)trig), (f32)doorPos->x);
    f32 z = gabi::fmadds(475.f, cM_scos((s16)trig), (f32)doorPos->z);
    f32 y = (f32)doorPos->y + 400.f;
    doorPos->set(x, y, z);
    id = fopAcM_createChild(fpcNm_KNOB00_e, fopAcM_GetID(this), (u32)dr_prm.params[doorType],
                            doorPos.get(), (s8)tevStr.mRoomNo, angle.get(), 0, -1, 0);
    m304 = 0;
    m308 = id;
}

/* 02384A78 */
void daObjPirateship::Act_c::sound_proc() {
    if (m2CC != 1)
        return;
    PSMTXCopy(mModel->getBaseTRMtx(), mDoMtx_stack_c::get());
    if (stageEqual("A_umikz")) {
        gabi::Local<cXyz> p;
        p->set(0, 800, 2000);
        PSMTXMultVec(mDoMtx_stack_c::get(), p.get(), &m30C);
        mDoAud_seStart(JA_SE_OBJ_PIRATE_CRUISE, &m30C);
    }
    if (m2CE) {
        gabi::Local<cXyz> p;
        p->set(0, 1500, 300);
        PSMTXMultVec(mDoMtx_stack_c::get(), p.get(), &m318);
        mDoAud_seStart(JA_SE_OBJ_PIRATE_SAIL, &m318);
    }
}

/* 02384DA0 */
BOOL daObjPirateship::Act_c::CreateHeap() {
    struct Safe {
        be<u32> name, vt;
    };
    struct Res {
        be<u32> name, vt, pad;
    };
    gabi::Local<Safe> archive;
    archive->name = gabi::guest_str("Kaizokusen");
    archive->vt = VT_SAFE_STRING;
    void* data = pirateshipObjectResource(l_resourceManager.pointer.get(), archive.get(),
                                          KAIZOKUSEN_BDL_SHIP);
    if (!data)
        pirateshipAssert("d_a_obj_pirateship.cpp", 0x189, "modelData != (0)");
    mModel = mDoExt_J3DModel__create(static_cast<J3DModelData*>(data), 0x80000, 0x11000002);
    if ((s8)l_HIO.mNo < 0)
        // HD: ASCII HIO label maps to the original Shift-JIS guest label, 海賊船.
        l_HIO.mNo = mDoHIO_createChild("Pirate Ship", &l_HIO);
    if (!mModel)
        return 0;
    // HD: animated sea surface is a separate model absent from the GameCube actor.
    gabi::Local<Safe> arc2;
    gabi::Local<Res> sea;
    arc2->name = gabi::guest_str("Kaizokusen");
    arc2->vt = VT_SAFE_STRING;
    sea->name = gabi::guest_str("kaizoku_minamo.bdl");
    sea->vt = VT_SAFE_STRING;
    sea->pad = VT_SAFE_STRING;
    J3DModelData* seaData = static_cast<J3DModelData*>(
        pirateshipNamedResource(l_resourceManager.pointer.get(), arc2.get(), sea.get()));
    mSeaModel = mDoExt_J3DModel__create(seaData, 0, 0x11020203);
    if (!mSeaModel)
        return 0;
    gabi::Local<Safe> arc3;
    gabi::Local<Res> btk;
    arc3->name = gabi::guest_str("Kaizokusen");
    arc3->vt = VT_SAFE_STRING;
    btk->name = gabi::guest_str("kaizoku_minamo.btk");
    btk->vt = VT_SAFE_STRING;
    btk->pad = VT_SAFE_STRING;
    void* animation =
        pirateshipNamedResource(l_resourceManager.pointer.get(), arc3.get(), btk.get());
    return mSeaBtk.init(seaData, static_cast<J3DAnmTextureSRTKey*>(animation), true, 2, 1.f, 0, -1,
                        false, 0)
               ? 1
               : 0;
}

/* 02384570 */
void daObjPirateship::Act_c::CreateWave() {
    m6C8.copy(current.pos);
    if (!m4AC.mpEmitter) {
        dComIfGp_getParticle()->set(5, PIRATESHIP_TAIL, &m6C8, &current.angle, nullptr, 0,
                                    reinterpret_cast<dPa_levelEcallBack*>(&m4AC), -1, nullptr,
                                    nullptr, nullptr);
        PirateshipEmitter* emitter = m4AC.mpEmitter;
        if (emitter) {
            emitter->mGlobalDynamicsScale.y = 20.0f;
            emitter->mGlobalParticleScale.z = 20.0f;
            emitter->mGlobalParticleScale.x = 20.0f;
            emitter->mGlobalParticleScale.y = 20.0f;
            emitter->mGlobalDynamicsScale.x = 20.0f;
            emitter->mGlobalDynamicsScale.z = 20.0f;
        }
    }
    if (!m4FC) {
        gabi::Local<GXColor> secondary;
        dKy_get_seacolor(&m500, secondary.get());
        m4FC = reinterpret_cast<PirateshipEmitter*>(
            dComIfGp_getParticle()->set(0, PIRATESHIP_SPLASH, &current.pos, &current.angle, nullptr,
                                        255, nullptr, -1, &m500, nullptr, nullptr));
    }
}

/* 02384C2C */
void daObjPirateship::Act_c::SetWave() {
    PirateshipEmitter* emitter = m4AC.mpEmitter;
    if (emitter) {
        f32 scale = (f32)l_HIO.m24;
        emitter->mGlobalDynamicsScale.y = scale;
        emitter->mGlobalParticleScale.x = scale;
        emitter->mGlobalParticleScale.z = scale;
        emitter->mGlobalParticleScale.y = scale;
        emitter->mGlobalDynamicsScale.z = scale;
        emitter->mGlobalDynamicsScale.x = scale;
        PSMTXCopy(mModel->getBaseTRMtx(), mDoMtx_stack_c::get());
        PSMTXMultVec(mDoMtx_stack_c::get(), &l_HIO.m18, &m6C8);
        f32 waterY = m6C8.y;
        u32 flags = (u32)m504.m_flags;
        m4AC.mWaterY = waterY;
        f32 indirect = (f32)l_HIO.m28, size = (f32)l_HIO.m2C;
        m4AC.mIndirectOffset = indirect;
        m4AC.mIndirectScale = size;
        m4AC.mSpeed = (f32)l_HIO.m30;
        m4AC.mWaterFlatY = (flags & 0x800) ? (f32)m504.m_wtr.m_height : (f32)m6C8.y;
        m4AC.mLimitSpeed = (f32)l_HIO.m34;
        emitter = m4AC.mpEmitter;
        emitter->mInitialVelDir = (f32)l_HIO.m38;
    }
    if (m4FC) {
        gabi::Local<GXColor> secondary;
        dKy_get_seacolor(&m500, secondary.get());
        emitter = m4FC.get();
        u8 b = (u8)m500.b, r = (u8)m500.r, g = (u8)m500.g;
        emitter->mGlobalPrmColor.r = r;
        emitter->mGlobalPrmColor.b = b;
        emitter->mGlobalPrmColor.g = g;
        emitter = m4FC.get();
        f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
        // HD: reflected draw modes invert the splash translation vertically.
        if ((u8)emitter->mDrawMode >= 7)
            y = -y;
        emitter->mGlobalTranslation.y = y;
        emitter->mGlobalTranslation.x = x;
        emitter->mGlobalTranslation.z = z;
        emitter = m4FC.get();
        s16 az = current.angle.z, ay = current.angle.y, ax = current.angle.x;
        JPAGetXYZRotateMtx(ax, ay, az, &emitter->mGlobalRotation);
    }
}

/* 02384A48 */
void daObjPirateship::Act_c::DeleteWave() {
    PirateshipEmitter* emitter = m4FC.get();
    if (emitter) {
        u32 flags = (u32)emitter->mFlags;
        emitter->mMaxFrame = -1;
        emitter->mFlags = flags | 1;
        m4FC = 0;
    }
    dPa_trackEcallBack_remove(&m4AC);
}

/* 02384F20 */
BOOL daObjPirateship::Act_c::Create() {
    cullMtx = gabi::ea(mModel->getBaseTRMtx());
    fopAcM_setCullSizeBox(this, -1400.f, -200.f, -1800.f, 1400.f, 3300.f, 2200.f);
    cullSizeFar = 10.f;
    return 1;
}

/* 02384FA4 */
BOOL daObjPirateship::Act_c::Execute(be<u32>* matrix) {
    *matrix = 0;
    return 1;
}

/* 02384FB4 */
BOOL daObjPirateship::Act_c::Draw() {
    if (!m2CC)
        return 0;
    auto env = dKy_getEnvlight();
    settingTevStruct(env, 0, &current.pos, &tevStr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, mModel.get(), &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mModel.get(), 0);
    dComIfGd_setList();
    // HD: Demo37 hides the sea plane; other demos receive the animated sea color.
    gabi::Local<PirateshipSafeString> name, stage;
    name->name = gabi::guest_str("Demo37");
    name->vt = VT_SAFE_STRING;
    stage->name = gabi::ea(l_demoStage.bytes);
    stage->vt = VT_SAFE_STRING;
    pirateship_stringNoop(name.get());
    safeCheck(name.get());
    u32 str = name->name;
    safeCheck(stage.get());
    if (!stringEqual(str, stage->name)) {
        J3DModelData* modelData = mSeaModel->mModelData;
        PirateshipMaterial* material = gabi::at<PirateshipMaterial>(modelData->mMaterials);
        struct RGBA16 {
            be<s16> r, g, b, a;
        };
        struct RGBAf {
            be<f32> r, g, b, a;
        };
        gabi::Local<GXColor> primary, secondary;
        gabi::Local<RGBA16> color;
        gabi::Local<RGBAf> normalized, converted;
        dKy_get_seacolor(primary.get(), secondary.get());
        color->g = primary->g;
        color->r = primary->r;
        color->b = primary->b;
        color->a = 255;
        PirateshipColorBlock* block = material->mColorBlock;
        gabi::call_ptr(block->__vtbl->setMatColor, block, 1, color.get());
        normalized->r = (f32)(s16)color->r / 255.f;
        normalized->g = (f32)(s16)color->g / 255.f;
        normalized->b = (f32)(s16)color->b / 255.f;
        normalized->a = (f32)(s16)color->a / 255.f;
        pirateshipConvertSeaColor(converted.get(), normalized.get(), 1.f);
        material->mColors.flags |= 0x20;
        PirateshipColorF* destination = pirateshipMaterialColor(&material->mColors, 5);
        f32 b = converted->b, r = converted->r, g = converted->g;
        destination->g = g;
        destination->b = b;
        destination->r = r;
        destination->a = (f32)(s16)color->a / 255.f;
        mSeaBtk.entry(modelData, mSeaBtk.getFrame());
        mDoExt_modelUpdateDL(mSeaModel.get(), 0);
    }
    return 1;
}

/* 02386140 */
BOOL daObjPirateship::Act_c::Delete() {
    return 1;
}

/* 02384974 */
BOOL daObjPirateship::Act_c::demo_move() {
    u8 id = demoActorID;
    if (!id || id > 32)
        return 0;
    void* manager = l_demoManager.pointer;
    if (!manager) {
        pirateshipAssert("d_demo.h", 0x23A, "m_object != (0)");
        manager = l_demoManager.pointer;
    }
    if (!pirateshipDemoActor(manager, id))
        return 0;
    dDemo_setDemoData(this, 10, 0, "Pirates", 0, 0, 0, 0);
    return 1;
}

/* 02385B70 */
BOOL daObjPirateship::Act_c::_execute() {
    return executeShip(this);
}

/* 023852E0 */
BOOL daObjPirateship::Act_c::_create() {
    // HD: inlined GHS construction preserves each per-TU vtable store and callback argument.
    if (!(actor_condition & 8)) {
        if (gabi::ea(this)) {
            dBgS_MoveBgActor__ct(this);
            __vtbl = VT_ACTOR;
            mDoExt_btkAnm::ct(&mSeaBtk);
            cCcD_Stts__ct(&m338);
            dCcD_GStts__ct(&m338.__vtbl_gstts);
            m338.__vtbl = VT_STATUS;
            m338.__vtbl_gstts = VT_GLOBAL_STATUS;
            dCcD_GObjInf__ct(&mCyl);
            mCyl.mShape.__vtbl = VT_SHAPE_BASE;
            mCyl.mShape.boundsVtable = VT_CYLINDER_BOUNDS;
            cM3dGCyl__ct(&mCyl.mCyl);
            mCyl.mCyl.__vtbl = VT_CYLINDER_GEOMETRY;
            mCyl.__vtbl_hitinf = VT_CYLINDER_HIT;
            mCyl.mShape.__vtbl = VT_CYLINDER_SHAPE;
            m4AC.__vtbl = VT_TRACK;
            __construct_array(m4AC.mPos, 3, 12,
                              gabi::guest_fn(reinterpret_cast<const void*>(pirateship_vectorCtor)));
            pirateshipAcchConstruct(&m504);
            m504.dBgS_GrpPassChk::__vtbl = VT_ACCH_GROUP_PASS;
            m504.cBgS_Chk::__vtbl = VT_ACCH_CHECK;
            m504.dBgS_PolyPassChk::__vtbl = VT_ACCH_POLY_PASS;
            m504.mbObjThrough = 1;
        }
        actor_condition = actor_condition | 8;
    }
    s32 phase = dComIfG_resLoad(&mPhase, "Kaizokusen");
    if (phase != cPhs_COMPLEATE_e)
        return phase;
    s32 collision = stageEqual("sea") ? KAIZOKUSEN_DZB_ROOM1B : KAIZOKUSEN_DZB_ROOM1A;
    phase = pirateshipMoveBGCreate(this, "Kaizokusen", collision, FN_MOVE_BG_TYPICAL_ROT_Y, 0x7500);
    mpBgW->rideCallback = gabi::guest_fn(reinterpret_cast<const void*>(ride_call_back));
    m2CE = (mParameters & 15) != 0;
    partsCreate();
    u8 variant = mParameters >> 24;
    m4FC = 0;
    switch (variant) {
    case PIRATESHIP_FIRST_ISLAND:
        if (!eventBit(dSv_event_flag_c::UNK_0310)) {
            m2CC = 0;
            m2CD = 0;
            dBgS* bg = dComIfG_Bgsp();
            bg->Release(reinterpret_cast<dBgW*>(mpBgW.get()));
        } else {
            // HD: an arrived ship appears immediately, unlike the GameCube branch.
            m2CC = 1;
            if (eventBit(dSv_event_flag_c::UNK_0001)) {
                pirateCreate(create_idx_tbl_1stIsland.values);
                m2CD = 1;
            } else {
                m2CD = 0;
                CreateWave();
            }
            if (!m2CC) {
                dBgS* bg = dComIfG_Bgsp();
                bg->Release(reinterpret_cast<dBgW*>(mpBgW.get()));
            }
        }
        break;
    case PIRATESHIP_UMIKZ:
        pirateCreate(create_idx_tbl_umikz.values);
        m2CD = 1;
        m2CC = 1;
        CreateWave();
        if (!m2CC) {
            dBgS* bg = dComIfG_Bgsp();
            bg->Release(reinterpret_cast<dBgW*>(mpBgW.get()));
        }
        break;
    case PIRATESHIP_ARRIVE_MJ: {
        if (dComIfGp_getStartStagePoint() == 0x12)
            pirateCreate(create_idx_tbl_arriveMj.values);
        m2CD = 1;
        m2CC = 1;
        break;
    }
    case PIRATESHIP_YUUKAIGO:
        pirateCreate(create_idx_tbl_Yuukaigo.values);
        m2CD = 1;
        m2CC = 1;
        break;
    default:
        m2CD = 1;
        m2CC = 1;
        break;
    }
    u8 pathIndex = (mParameters >> 16) & 255;
    m4A8 = 0;
    m4A4 = 0;
    if (pathIndex != 255)
        m4A8 = dPath_GetRoomPath(pathIndex, (s8)current.roomNo);
    m32E = 0;
    m330 = 0x180;
    set_mtx();
    executeShip(this);
    return phase;
}

/* 02385AEC */
BOOL daObjPirateship::Act_c::_delete() {
    s8 child = (s8)l_HIO.mNo;
    if (child >= 0) {
        mDoHIO_deleteChild(child);
        l_HIO.mNo = -1;
    }
    DeleteWave();
    mDoAud_seDeleteObject(&m30C);
    mDoAud_seDeleteObject(&m318);
    s32 result = pirateshipMoveBGDelete(this);
    dComIfG_deleteObjectRes("Kaizokusen");
    return result != 0;
}

/* 02385F6C */
BOOL daObjPirateship::Act_c::_draw() {
    s32 result = gabi::call_ptr<s32>(gabi::at<PirateshipActorVtable>(__vtbl)->Draw, this);
    return result != 0;
}

/* 023861D8 */
static BOOL pirateship_Method_IsDelete(void* p) {
    return TRUE;
}

// HD: compiler-generated and template code of this translation unit.

/* 02385FA0 */
void pirateship_sinit() {
    for (u32 i = 0; i < 4; i++)
        l_math.zero[i] = 0;
    pirateshipInitDescriptor(&l_descriptors.words[0]);
    l_math.maximum = 3.1415927410125732f;
    l_math.minimum = -3.1415927410125732f;
    pirateshipInitSinTable(&l_math.sinInit);
    pirateshipInitDescriptor(&l_descriptors.words[3]);
    pirateshipInitAtanTable(&l_math.atanInit);
    pirateshipInitDescriptor(&l_descriptors.words[6]);
    l_HIO.m08 = 2;
    l_HIO.m0C = 100;
    l_HIO.m24 = 4;
    l_HIO.m28 = -0.02f;
    l_HIO.m2C = 5;
    l_HIO.m30 = 300;
    l_HIO.__vtbl = VT_HIO;
    // HD: static construction copies raw words, including NaN payloads, in GHS order.
    const be<u32>* zero = reinterpret_cast<const be<u32>*>(&cXyz::Zero);
    be<u32>* offset = reinterpret_cast<be<u32>*>(&l_HIO.m18);
    u32 yz = zero[1], x = zero[0];
    offset[0] = x;
    offset[1] = yz;
    l_HIO.m06 = 0;
    l_HIO.m10 = 250;
    l_HIO.m12 = 500;
    u32 z = zero[2];
    l_HIO.mNo = -1;
    l_HIO.m05 = 0;
    l_HIO.m14 = 0;
    l_HIO.m34 = 3;
    l_HIO.m38 = 17;
    offset[2] = z;
}

/* 023860F4 */
void pirateship_hioDelete(void* obj, u32 flags) {
    if (obj && (flags & 1))
        operator_delete(obj);
}

/* 02386110 */
void pirateship_stringNoop(void* str) {}

/* 02386114 */
void* pirateship_vectorCtor(void* str) {
    if (!str)
        str = operator_new(12);
    return str;
}

/* 02386148 */
static void pirateship_Act_c_dt(void* p, s32 flags) {
    if (p != nullptr) {
        Ship* actor = static_cast<Ship*>(p);
        actor->m504.dBgS_GrpPassChk::__vtbl = VT_ACCH_GROUP_PASS;
        actor->m504.dBgS_PolyPassChk::__vtbl = VT_ACCH_POLY_PASS;
        pirateshipLineDestruct(&actor->m504, 0);   /* line check */
        pirateshipAcchDestruct(&actor->mCyl, 2);   /* dBgS_Acch::~dBgS_Acch */
        pirateshipStatusDestruct(&actor->m338, 2); /* dCcD_Stts */
        pirateshipActorDestruct(p, 0);             /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(p);
    }
}

/* 02386108 */
BOOL pirateship_IsDelete(Ship* ship) {
    return 1;
}

/* WWHD glue
str 1002E8E0 "d_demo.h"
str 1002E8D0 "m_object != (0)"
str 1002E9DC "Pirates"
str 1002EAB0 "Kaizokusen"
str 1002EA04 "d_a_obj_pirateship.cpp" in 02384DA0
str 1002EA1C "modelData != (0)"
str 1002E9FC "Pirate Ship" renamed
str 1002EA30 "kaizoku_minamo.bdl"
str 1002EA44 "kaizoku_minamo.btk"
str 1002E944 "Psail"
str 1002E94C "d_a_obj_pirateship.cpp" in 0238418C
str 1002E964 "(m_sail_id != fpcM_ERROR_PROCESS_ID_e)"
str 1002E9F4 "A_umikz"
str 1002EA80 "Demo37"
str 1002E8EC "d_a_obj_pirateship.cpp" in 02385B70
str 1002E8EC "d_a_obj_pirateship.cpp" in 023852E0
str 1002E904 "0 <= roomNo && roomNo < 64"
str 1002E814 "sea"
data l_demoStage 1047E6B8
data create_idx_tbl_1stIsland_demo 101CC434
data create_idx_tbl_umikz 101CC43C
data create_idx_tbl_arriveMj 101CC454
data create_idx_tbl_Yuukaigo 101CC46C
data create_idx_tbl_1stIsland 101CC47C
data l_resourceManager 101F4F28
data dr_prm 101CC490
ext 024EF130 s32 pirateshipGroundRoom(dBgS*, cBgS_PolyInfo*)
ext 0200F62C BOOL cLib_chasePos(cXyz*, const cXyz*, f32)
ext 0201B47C BOOL cXyz_normalizeRS(cXyz*)
ext 02008E0C void cBgS_GndChk__ct(void*)
ext 02008DAC void cBgS_GndChk__dt(void*, s32)
ext 0200BD2C void cCcD_Stts__ct(void*)
ext 02018590 void cM3dGCyl__ct(void*)
ext 0201A478 void csXyz__ct(csXyz*, s32, s32, s32)
ext 024EFD9C void pirateshipLineDestruct(void*, s32)
ext 024F0474 void pirateshipAcchConstruct(void*)
ext 024F1D40 void dBgS_MoveBgActor__ct(Ship*)
ext 024F1D9C s32 pirateshipMoveBGCreate(Ship*, const char*, s32, u32, u32)
ext 024F1E9C s32 pirateshipMoveBGExecute(Ship*)
ext 024F1F64 s32 pirateshipMoveBGDelete(Ship*)
ext 02515860 void pirateshipStatusDestruct(void*, s32)
ext 02515A70 void pirateshipAcchDestruct(void*, s32)
ext 02515DA0 void dCcD_GStts__ct(void*)
ext 02515FB8 void dCcD_GObjInf__ct(void*)
ext 02520488 void dComIfG_deleteObjectRes(const char*)
ext 02526E70 u32 pirateshipDemoActor(void*, u32)
ext 02527028 void dDemo_setDemoData(Ship*, s32, u32, const char*, u32, u32, u32, u32)
ext 0252A7E8 void pirateshipDoorSetPos(void*, cXyz*, s32)
ext 025602F0 void dKy_get_seacolor(GXColor*, GXColor*)
ext 02587D24 void dLib_pathMove(cXyz*, be<s8>*, void*, f32, u32, Ship*)
ext 025A9E38 void dPa_trackEcallBack_remove(dPa_trackEcallBack*)
ext 025D50BC void pirateshipActorDestruct(void*, s32)
ext 025D5A20 u32 fopAcM_createChild(s32, u32, u32, cXyz*, s32, csXyz*, void*, s32, void*)
ext 025D5AA4 u32 fopAcM_createChild(const char*, u32, u32, cXyz*, s32, csXyz*, void*, void*)
ext 025D5D88 void fopAcM_createChildFromOffset(const char*, u32, u32, cXyz*, s32, csXyz*, void*, void*)
ext 025E19CC void mDoAud_seStart(u32, cXyz*)
ext 025E1B34 void mDoAud_seDeleteObject(cXyz*)
ext 026066C4 void* pirateshipObjectResource(void*, void*, s32)
ext 02606900 void* pirateshipNamedResource(void*, void*, void*)
ext 0273AA24 void pirateshipAssert(const char*, s32, const char*)
ext 0273AD10 void* operator_new(u32)
ext 0274D458 void pirateshipConvertSeaColor(void*, const void*, f32)
ext 027F9F0C PirateshipColorF* pirateshipMaterialColor(void*, s32)
ext 028245AC void JPAGetXYZRotateMtx(s32, s32, s32, Mtx34*)
ext 028EAB2C void pirateshipInitAtanTable(be<u8>*)
ext 028ED6F8 void pirateshipInitSinTable(be<u8>*)
ext 028EFFD0 void __construct_array(void*, s32, u32, u32)
ext 028F026C void pirateshipInitDescriptor(void*)
data l_HIO 1046BC84
data l_math 1046BC68
data l_descriptors 101CC498
data l_doorOffset 1046BCC0
data p_offset 101CC50C
data l_demoManager 101D5FFC
*/
