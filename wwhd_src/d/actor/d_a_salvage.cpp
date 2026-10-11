/** Salvage lights and crane rewards, WWHD. */
#include "d/actor/d_a_salvage.h"

const gc_tu_data gc_tu = {.safeString = 0x100398DC, .headerStatics = {0x1046D7E4, 0x101CFEB0, 0}};
static const char* m_arcname;
static dSalvage_control_c* mTagData_p;
static s32 mSalvageId;
static s8 mNowRoomNo;
static salvage_actor::Hio l_HIO;
static salvage_actor::SaveLabels m_savelabel;
static salvage_actor::Depths l_salvage_depth;
static BOOL CheckCreateHeap(daSalvage_c*);
static cPhs_State salvage_createCB(fopAc_ac_c*);
static void colorFromGXColor(be<f32>*, be<u8>*);

namespace {
inline dSalvage_control_c* registry() {
    return mTagData_p;
}
} // namespace
namespace {
inline BOOL eventStart(s16 event) {
    auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    return salvageEventStart(&play->play.mEvtManager, event);
}
inline BOOL eventEnd(s16 event) {
    auto* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    return salvageEventEnd(&play->play.mEvtManager, event);
}
inline void eventReset() {
    dComIfG_inf_c* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    play->play.mEvtCtrl.mEventFlag = play->play.mEvtCtrl.mEventFlag | 8;
}
} // namespace

/* 024658FC */
static BOOL CheckCreateHeap(daSalvage_c* self) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return self->CreateHeap();
}

/* 024656EC */
BOOL daSalvage_c::CreateHeap() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dSalvage_control_c* reg =
        static_cast<dSalvage_control_c*>(operator_new(sizeof(dSalvage_control_c)));
    mTagData_p = reg;
    if (!reg)
        return FALSE;
    reg->init();
    auto resource = [](s32 index) {
        dRes_control_c* control = dComIfG_resControl();
        const char* name = m_arcname;
        gabi::Local<SafeString> key;
        key->mStringTop = gabi::guest_addr(name);
        key->__vtbl = gc_tu.safeString;
        return salvageObjectResource(control, key.get(), index);
    };
    J3DModelData* data =
        static_cast<J3DModelData*>(resource(salvage_actor::dRes_INDEX_YSLVG00_BDL_YSLVG00_e));
    this->mModelData = data;
    if (!data) {
        JUT_ASSERT_fail("d_a_salvage.cpp", 0x156, "mModelData != NULL");
        data = this->mModelData;
    }
    for (int i = 0; i < 16; i++) {
        J3DModel* model = mDoExt_J3DModel__create(data, 0x80000, 0x11000222);
        this->mModels[i] = model;
        if (!model)
            return FALSE;
        if (i != 15)
            data = this->mModelData;
    }
    J3DAnmTevRegKey* pbrk =
        static_cast<J3DAnmTevRegKey*>(resource(salvage_actor::dRes_INDEX_YSLVG00_BRK_YSLVG00_e));
    if (!pbrk)
        JUT_ASSERT_fail("d_a_salvage.cpp", 0x16B, "pbrk != NULL");
    mDoExt_brkAnm* brk = salvageCreateBrk(0);
    this->mpBrk = brk;
    if (!brk)
        return FALSE;
    data = this->mModelData;
    if (!salvageInitBrk(brk, data, pbrk, 1, 2, 1.0f, 0, -1, 0, 0))
        return FALSE;
    J3DAnmTextureSRTKey* pbtk = static_cast<J3DAnmTextureSRTKey*>(
        resource(salvage_actor::dRes_INDEX_YSLVG00_BTK_YSLVG00_e));
    if (!pbtk)
        JUT_ASSERT_fail("d_a_salvage.cpp", 0x179, "pbtk != NULL");
    mDoExt_btkAnm* btk = salvageCreateBtk(0);
    this->mpBtk = btk;
    if (!btk)
        return FALSE;
    data = this->mModelData;
    return salvageInitBtk(btk, data, pbtk, 1, 2, 1.0f, 0, -1, 0, 0) != 0;
}

/* 02465914 */
BOOL daSalvage_c::CreateInit() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s8 room = this->current.roomNo;
    mNowRoomNo = room;
    setProc(&daSalvage_c::proc_wait_init);
    this->m2E8 = room;
    fopAcM_setStageLayer(this);
    this->m2EA = 1;
    const char* names[] = {"SALVAGE_GETITEM", "SALVAGE_HAZURE", "SLVG_GETITEM_LEFT",
                           "SLVG_HAZURE_LEFT"};
    be<s16>* events[] = {&this->mSalvageGetItemEventIdx, &this->mSalvageHazureEventIdx,
                         &this->mSlvgGetItemLeftEventIdx, &this->mEventIdx};
    for (int i = 0; i < 4; i++) {
        dComIfG_inf_c* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        *events[i] = salvageEventIdx(&play->play.mEvtManager, names[i], 255);
    }
    dComIfG_inf_c* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    dPa_control_c* particles = play->play.mParticle;
    salvage_actor::Emitter* emitter =
        salvageSetParticle(particles, 0, 0x437, &this->current.pos, 0, 0, 255, 0, -1, 0, 0, 0);
    this->mEmitter = emitter;
    if (emitter)
        emitter->mStatus = emitter->mStatus | 1;
    return TRUE;
}

/* 02465A8C */
cPhs_State daSalvage_c::_create() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            fopAc_ac_c_ct(this);
            this->__vtbl = salvage_actor::ActorVtable;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State result = salvageResLoad(&this->mPhase, (const char*)m_arcname);
    if (result != cPhs_COMPLEATE_e)
        return result;
    dSalvage_control_c* reg = registry();
    if (!reg) {
        if (!salvageSolidHeap(this, CheckCreateHeap, 0xB000))
            return cPhs_ERROR_e;
        CreateInit();
        reg = registry();
        if (!reg) {
            JUT_ASSERT_fail("d_a_salvage.cpp", 0x1DE, "mTagData_p != NULL");
            reg = registry();
        }
        reg->entry(this, 0);
        return cPhs_COMPLEATE_e;
    }
    s8 current = this->current.roomNo, old = mNowRoomNo;
    if (old != current && current) {
        for (s32 i = 128; i < 160; i++)
            if (registry()->checkRegist(i) && isEffectKind(i) && registry()->checkUsed(i)) {
                cXyz* pos = registry()->getPosP(i);
                mDoAud_seDeleteObject(pos);
            }
        registry()->init_room(old);
        this->m2E8 = mNowRoomNo;
        current = this->current.roomNo;
        reg = registry();
        mNowRoomNo = current;
    }
    reg->entry(this, 0);
    return cPhs_ERROR_e;
}

/* 02465C68 */
BOOL daSalvage_c::_delete() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (this->m2EA) {
        salvage_actor::Emitter* emitter = this->mEmitter;
        if (emitter) {
            u32 flags = emitter->mStatus;
            emitter->mMaxFrame = -1;
            emitter->mStatus = flags | 1;
            this->mEmitter = 0;
        }
        for (s32 i = 0; i < 160; i++)
            if (isEffectKind(i) && registry()->checkUsed(i)) {
                cXyz* pos = registry()->getPosP(i);
                mDoAud_seDeleteObject(pos);
            }
        dSalvage_control_c* reg = registry();
        if (reg) {
            reg->init_end();
            mTagData_p = nullptr;
        }
    }
    return TRUE;
}

/* 02466408 */
BOOL daSalvage_c::_execute() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    // HD: runtime HIO can enable every registry entry.
    if (l_HIO.mForceEnabled)
        for (s32 i = 0; i < 160; i++)
            registry()->clrFlag(i, 1);
    registry()->calcDistanceXZ();
    checkOrder();
    if (mProcIndex)
        runProc();
    eventOrder();
    this->mStayNo = (s8)dComIfGp_roomControl_getStayNo();
    return TRUE;
}

/* 02465900 */
static cPhs_State salvage_createCB(fopAc_ac_c* self) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    self->actor_status = self->actor_status | fopAcStts_FORCEMOVE_e;
    return cPhs_COMPLEATE_e;
}

/* 02465EC4 */
void daSalvage_c::checkOrder() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfG_inf_c* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    salvage_actor::Ship* ship =
        static_cast<salvage_actor::Ship*>((fopAc_ac_c*)play->play.mpPlayerPtr[2]);
    u8 type = registry()->getType(mSalvageId);
    u8 kind = registry()->getKind(mSalvageId);
    if (eventInfo.mCommand != 2)
        return;
    BOOL skipCountdown = FALSE;
    BOOL starting = (eventStart(this->mSalvageGetItemEventIdx) && this->m2E9 == 2) ||
                    (eventStart(this->mSlvgGetItemLeftEventIdx) && this->m2E9 == 1) ||
                    (eventStart(this->mSalvageHazureEventIdx) && this->m2E9 == 4) ||
                    (eventStart(this->mEventIdx) && this->m2E9 == 3);
    if (starting) {
        ship->mStateFlag = ship->mStateFlag | 0x800;
        u8 order = this->m2E9;
        type = 1;
        cXyz* top = ship->mCraneTop;
        this->m304 = 0xFFFFFFFF;
        if (order == 3 || order == 4) {
            this->m301 = 0;
            type = 0;
        } else {
            s8 room = mNowRoomNo;
            this->m304 =
                salvageCreateActor(salvage_actor::fpcNm_SBOX_e, 0, top, room, &this->current.angle,
                                   0, -1, gabi::guest_fn((const void*)&salvage_createCB));
            this->m301 = 1;
            if (kind == 0)
                type = 2;
        }
        u8 item = registry()->getItemNo(mSalvageId);
        s8 room = mNowRoomNo;
        u32 chest = salvageCreateActor(salvage_actor::fpcNm_SALVAGE_TBOX_e, ((u32)type << 8) | item,
                                       top, room, &this->current.angle, &this->scale, -1,
                                       gabi::guest_fn((const void*)&salvage_createCB));
        this->m2EC = chest;
        this->m2E9 = 0;
        if (chest == 0xFFFFFFFF || (this->m304 == 0xFFFFFFFF && this->m301))
            eventReset();
        else {
            setProc(&daSalvage_c::proc_salvage_init); // HD: consume the first countdown frame when
                                                      // ordering succeeds.
            this->m302 = 99;
            skipCountdown = TRUE;
        }
    }
    if (!skipCountdown) {
        if (this->m302 == 0) {
            gabi::Local<be<u32>> actor1, actor2;
            BOOL found = salvageSearchActor((u32)this->m2EC, actor1.get());
            if (this->m301)
                found &= salvageSearchActor((u32)this->m304, actor2.get());
            if (!found) {
                salvageDeleteActor((u32)this->m2EC);
                salvageDeleteActor((u32)this->m304);
                this->m301 = 0;
                eventReset();
            }
        }
        this->m302--;
    }
    if (eventEnd(this->mSalvageGetItemEventIdx) || eventEnd(this->mSalvageHazureEventIdx) ||
        eventEnd(this->mSlvgGetItemLeftEventIdx) || eventEnd(this->mEventIdx)) {
        ship->mStateFlag = ship->mStateFlag & ~0x800U;
        salvageDeleteActor((u32)this->m2EC);
        this->m301 = 0;
        end_salvage();
        eventReset();
        if ((eventEnd(this->mSalvageHazureEventIdx) || eventEnd(this->mEventIdx)) &&
            (type == 2 || kind == 5))
            createEnemy();
        proc_wait_init();
    }
}

/* 024662F0 */
void daSalvage_c::eventOrder() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfG_inf_c* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    if (!(play->play.mPlayerStatus[0][0] & 0x10000))
        return;
    u8 order = this->m2E9;
    s16 event;
    if (order == 2)
        event = this->mSalvageGetItemEventIdx;
    else if (order == 1)
        event = this->mSlvgGetItemLeftEventIdx;
    else if (order == 4)
        event = this->mSalvageHazureEventIdx;
    else if (order == 3)
        event = this->mEventIdx;
    else
        return;
    salvageOrderEvent(this, event, 0xFF, 0xFFFF, 0, 1);
    eventInfo.mCondition = eventInfo.mCondition | 2;
}

/* 02465E9C */
BOOL daSalvage_c::proc_wait_init() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    setProc(&daSalvage_c::proc_wait);
    this->m2EC = 0xFFFFFFFF;
    return TRUE;
}

/* 02467364 */
BOOL daSalvage_c::proc_wait() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfG_inf_c* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    salvage_actor::Ship* ship =
        static_cast<salvage_actor::Ship*>((fopAc_ac_c*)play->play.mpPlayerPtr[2]);
    dComIfGp_get();
    if (!ship)
        return TRUE;
    gabi::Local<be<f32>> distance;
    for (s32 i = 0; i < 160; i++) {
        s32 sw = registry()->getSwitchNo(i);
        u8 slot = registry()->getSaveNo(i);
        u8 room = (u8)registry()->getRoomNo(i);
        gabi::Local<cXyz> ignored;
        salvageGetPos(registry(), ignored.get(), i);
        u8 kind = registry()->getKind(i);
        if (!registry()->checkRegist(i))
            continue;
        getDistance(i, distance.get());
        if (kind == 2) {
            if (room != 255 && sw != 255) {
                if (!registry()->checkUsed(i) && salvageIsSwitch(dComIfGs_info(), sw, room) &&
                    !salvageIsOceanBit(&dComIfGs_info()->mSavedata.mOcean, room, slot))
                    registry()->clrFlag(i, 1);
                if (!salvageIsSwitch(dComIfGs_info(), sw, room) &&
                    !salvageIsOceanBit(&dComIfGs_info()->mSavedata.mOcean, room, slot)) {
                    gabi::Local<be<u16>> name;
                    *name.get() = salvage_actor::fpcNm_DAIOCTA_e;
                    salvage_actor::Octopus* octo =
                        salvageFindActor(salvage_actor::fpcSch_JudgeForPName, name.get());
                    if (octo && octo->mSwitchNo == (u32)sw) {
                        gabi::Local<cXyz> pos;
                        salvageGetPos(registry(), pos.get(), i);
                        f32 x = octo->current.pos.x, y = pos->y, z = octo->current.pos.z;
                        gabi::Local<cXyz> moved;
                        moved->x = x;
                        moved->y = y;
                        moved->z = z;
                        registry()->setPos(i, moved.get());
                    }
                }
            }
        } else if (kind == 0) {
            if (salvageIsOpenMap(&dComIfGs_info()->mSavedata.mPlayer.mMap, (s32)slot - 1) &&
                !salvageIsCompleteMap(&dComIfGs_info()->mSavedata.mPlayer.mMap, (s32)slot - 1)) {
                u8 item = registry()->getItemNo(i);
                BOOL triforce = salvageIsTriforce(item);
                if (!triforce || salvageIsTriforceMap(&dComIfGs_info()->mSavedata.mPlayer.mMap,
                                                      (s32)(u8)(item - 0x60) - 1))
                    registry()->clrFlag(i, 1);
            }
        } else if (kind == 4) {
            if (salvageDayNight() != 0) {
                if (!salvageIsOceanBit(&dComIfGs_info()->mSavedata.mOcean, room, slot))
                    registry()->clrFlag(i, 1);
            } else if (!salvageDayNight())
                registry()->setFlag(i, 1);
        } else if (kind == 6) {
            if (!salvageMoonType() && salvageMoonArrival() &&
                !salvageIsEventBit(&dComIfGs_info()->mSavedata.mEvent, m_savelabel.values[slot]))
                registry()->clrFlag(i, 1);
            else
                registry()->setFlag(i, 1);
        }
        if (!isEffectKind(i) || !registry()->checkUsed(i))
            continue;
        if (!((f32)*distance.get() < 5000))
            continue;
        cXyz* pos = registry()->getPosP(i);
        s8 roomNo = this->mStayNo;
        s32 reverb = salvageReverb(roomNo);
        mDoAud_seStart(salvage_actor::JA_SE_OBJ_SALVAGE_TAG, pos, 0, reverb);
        // HD: the nearby salvage sound has a distance-dependent volume.
        f32 dist = *distance.get();
        if (dist > 2000)
            continue;
        f32 volume = 100.0f - dist / 20.0f;
        if (volume < 0)
            volume = 0;
        else if (volume > 100)
            volume = 100;
        pos = registry()->getPosP(i);
        u32 encoded = volume >= 2147483648.0f
                          ? (u32)gabi::ftoi(volume - 2147483648.0f) + 0x80000000U
                          : (u32)gabi::ftoi(volume);
        roomNo = this->mStayNo;
        reverb = salvageReverb(roomNo);
        mDoAud_seStart(salvage_actor::JA_SE_OBJ_SALVAGE_NEAR, pos, encoded, reverb);
    }
    if (checkXZDistance() == -1)
        this->mRndDepthIdx = gabi::ftoi(cM_rndF(10000)) % 3;
    s32 id = checkDistance();
    mSalvageId = id;
    if (id == -1)
        return TRUE;
    play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    if (play->play.mEvtCtrl.mMode)
        return TRUE;
    u8 type = registry()->getType(mSalvageId), kind = registry()->getKind(mSalvageId);
    u8 debug = l_HIO.mTypeOverride;
    if (debug != 255)
        type = debug;
    gabi::Local<cXyz> pos;
    salvageGetPos(registry(), pos.get(), mSalvageId);
    auto* positionWords = reinterpret_cast<salvage_actor::VectorWords*>(pos.get());
    u32 x = positionWords->x, y = positionWords->y, z = positionWords->z;
    auto* attentionWords = reinterpret_cast<salvage_actor::VectorWords*>(&attention_info.position);
    attentionWords->x = x;
    attentionWords->y = y;
    attentionWords->z = z;
    if (type == 1)
        this->m2E9 = ship->m0398 < 0 ? 1 : 2;
    else if (kind == 5 || type == 0 || type == 2 || type == 3)
        this->m2E9 = ship->m0398 < 0 ? 3 : 4;
    return TRUE;
}

/* 024679A0 */
BOOL daSalvage_c::proc_salvage_init() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    setProc(&daSalvage_c::proc_salvage);
    s32 id = mSalvageId;
    dSalvage_control_c* reg = registry();
    reg->setFlag(id, 1);
    return TRUE;
}

/* 02467CD4 */
BOOL daSalvage_c::proc_salvage() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return TRUE;
}

/* 02466578 */
void daSalvage_c::calcAlpha() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfG_inf_c* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    fopAc_ac_c* player = play->play.mPlayerInfo[0].mpPlayer;
    f32 x = player->current.pos.x, z = player->current.pos.z;
    BOOL sea = daSea_ChkArea(x, z);
    u8 frames = l_HIO.mDisappearFrame;
    u8 step = frames ? 255 / frames : 0;
    f32 cmap = l_HIO.mCmapNear, near = (sea ? l_HIO.mOuterseaNear : l_HIO.mInsideNear);
    gabi::Local<be<f32>> distance;
    for (s32 i = 0; i < 160; i++) {
        if (!registry()->checkRegist(i))
            continue;
        BOOL valid = getDistance(i, distance.get());
        u8 kind = registry()->getKind(i);
        f32 minimum = kind == 0 ? cmap : near;
        BOOL used = registry()->checkUsed(i);
        f32 dist = *distance.get();
        if (used && valid && !(dist > 25000) && !(dist < minimum * 100)) {
            play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
            if (!play->play.mEvtCtrl.mMode) {
                s32 delay = this->m2FC;
                if (delay > 0)
                    this->m2FC = delay - 1;
            }
            kind = registry()->getKind(i);
            if (kind == 2 && this->m2FC != 0)
                continue;
            be<u8>* alpha = static_cast<be<u8>*>(registry()->getAlphaPtr(i));
            cLib_chaseUC(alpha, 255, step);
            registry()->setDrawMode(i, 1);
            if (!registry()->getKind(i)) {
                salvage_actor::Emitter* emitter = this->mEmitter;
                if (emitter) {
                    emitter->mStatus = emitter->mStatus & ~1U;
                    gabi::Local<cXyz> pos;
                    emitter = this->mEmitter;
                    salvageGetPos(registry(), pos.get(), i);
                    f32 px = pos->x, py = pos->y, pz = pos->z;
                    u8 plane = emitter->mPlane;
                    emitter->mGlobalTranslation.x = px;
                    emitter->mGlobalTranslation.y = py;
                    emitter->mGlobalTranslation.z = pz;
                    if (plane >= 7)
                        emitter->mGlobalTranslation.y = -emitter->mGlobalTranslation.y;
                }
            }
        } else {
            be<u8>* alpha = static_cast<be<u8>*>(registry()->getAlphaPtr(i));
            BOOL done = cLib_chaseUC(alpha, 0, step);
            registry()->setDrawMode(i, done ? 0 : 1);
            if (!registry()->getKind(i)) {
                salvage_actor::Emitter* emitter = this->mEmitter;
                if (emitter)
                    emitter->mStatus = emitter->mStatus | 1;
            }
        }
    }
}

/* 02466F88 */
BOOL daSalvage_c::checkArea(cXyz* crane, s32 id) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    registry()->getH(id);
    f32 radius = registry()->getR(id);
    gabi::Local<cXyz> pos;
    salvageGetPos(registry(), pos.get(), id);
    gabi::Local<cXyz> delta;
    salvageSubtract(crane, delta.get(), pos.get());
    gabi::Local<cXyz> flat;
    flat->x = delta->x;
    flat->y = 0;
    flat->z = delta->z;
    f32 squared = salvageSquareMag(flat.get());
    f32 distance = salvageSqrt(squared);
    if (!(distance < radius))
        return FALSE;
    dComIfG_inf_c* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    salvage_actor::Ship* ship =
        static_cast<salvage_actor::Ship*>((fopAc_ac_c*)play->play.mpPlayerPtr[2]);
    f32 z = crane->z, x = crane->x;
    BOOL sea = daSea_ChkArea(x, z);
    x = crane->x;
    f32 water;
    if (sea) {
        z = crane->z;
        water = daSea_calcWave(x, z);
    } else {
        f32 y = ship->current.pos.y + 100.0f;
        z = crane->z;
        gabi::Local<cXyz> probe;
        probe->x = x;
        probe->y = y;
        probe->z = z;
        water = salvageWaterHeight(probe.get());
    }
    u8 depth = this->mRndDepthIdx;
    f32 bottom = water - l_salvage_depth.values[depth];
    return (f32)crane->y < bottom;
}

/* 02465E54 */
void daSalvage_c::createEnemy() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfG_inf_c* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    fopAc_ac_c* player = play->play.mPlayerInfo[0].mpPlayer;
    salvageCreateActor(salvage_actor::fpcNm_OQ_e, 2, salvage_actor::actorPosition(player), -1, 0, 0,
                       -1, 0);
}

/* 024679F4 */
BOOL daSalvage_c::onSalvageForOship(void* other) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    salvage_actor::OtherShip* ship = static_cast<salvage_actor::OtherShip*>(other);
    if (!ship || ship->mSwitchA == 255)
        return FALSE;
    for (s32 i = 128; i < 160; i++) {
        if (!registry()->checkRegist(i))
            continue;
        s32 sw = registry()->getSwitchNo(i);
        u8 slot = registry()->getSaveNo(i);
        u8 room = (u8)registry()->getRoomNo(i);
        gabi::Local<cXyz> ignored;
        salvageGetPos(registry(), ignored.get(), i);
        registry()->getKind(i);
        if (sw == 255 || (u32)sw != ship->mSwitchA)
            continue;
        if (salvageIsOceanBit(&dComIfGs_info()->mSavedata.mOcean, room, slot))
            continue;
        gabi::Local<cXyz> pos;
        salvageGetPos(registry(), pos.get(), i);
        f32 y = pos->y;
        f32 x = ship->current.pos.x;
        f32 z = ship->current.pos.z;
        gabi::Local<cXyz> moved;
        moved->x = x;
        moved->y = y;
        moved->z = z;
        registry()->setPos(i, moved.get());
        registry()->setNowAlpha(i, 255);
        this->m2FC = 90;
        return TRUE;
    }
    return FALSE;
}

/* 02465D30 */
BOOL daSalvage_c::end_salvage() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    // HD: debug tuning can retain a completed salvage.
    if (!l_HIO.mKeepSalvage) {
        u8 room = (u8)registry()->getRoomNo(mSalvageId);
        u8 kind = registry()->getKind(mSalvageId);
        u8 slot = registry()->getSaveNo(mSalvageId);
        if (kind == 0) {
            salvageCompleteMap(&dComIfGs_info()->mSavedata.mPlayer.mMap, (s32)slot - 1);
            salvageOnEventBit(&dComIfGs_info()->mSavedata.mEvent, 0x3E02);
        } else if (kind >= 2 && kind <= 4)
            salvageOnOceanBit(&dComIfGs_info()->mSavedata.mOcean, room, (u16)slot);
        else if (kind == 6) {
            u16 bit = m_savelabel.values[slot];
            salvageOnEventBit(&dComIfGs_info()->mSavedata.mEvent, bit);
        }
        if (kind != 6)
            registry()->init_one(mSalvageId);
        mSalvageId = -1;
    }
    return TRUE;
}

/* 02467124 */
s32 daSalvage_c::checkDistance() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfG_inf_c* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    salvage_actor::Ship* ship =
        static_cast<salvage_actor::Ship*>((fopAc_ac_c*)play->play.mpPlayerPtr[2]);
    if (!ship)
        return -1;
    cXyz* top = ship->mCraneTop;
    if (!top || ship->mCurMode != salvage_actor::MODE_CRANE_e ||
        ship->mNextMode != salvage_actor::MODE_CRANE_e)
        return -1;
    if (!(ship->speedF < 1.0f) || (ship->mStateFlag & 1) || ship->mTornadoActor ||
        ship->mWhirlActor)
        return -1;
    s32 found = -1;
    for (s32 i = 0; i < 160; i++) {
        if (!registry()->checkRegist(i) || !registry()->checkUsed(i))
            continue;
        gabi::Local<cXyz> crane;
        crane->x = top->x;
        crane->y = top->y;
        crane->z = top->z;
        if (checkArea(crane.get(), i)) {
            if (registry()->getKind(i) == 5)
                found = i;
            else
                return i;
        }
    }
    return found;
}

/* 02467290 */
s32 daSalvage_c::checkXZDistance() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dComIfG_inf_c* play = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
    salvage_actor::Ship* ship =
        static_cast<salvage_actor::Ship*>((fopAc_ac_c*)play->play.mpPlayerPtr[2]);
    if (!ship || !ship->mCraneTop)
        return -1;
    for (s32 i = 0; i < 160; i++) {
        if (!registry()->checkRegist(i))
            continue;
        if (!registry()->checkUsed(i))
            continue;
        gabi::Local<be<f32>> distance;
        if (!getDistance(i, distance.get()))
            continue;
        f32 radius = registry()->getR(i);
        if ((f32)*distance.get() < radius)
            return i;
    }
    return -1;
}

/* 024664F4 */
BOOL daSalvage_c::getDistance(s32 id, be<f32>* dist) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!registry()->checkRegist(id))
        return FALSE;
    *dist = registry()->getDistance(id);
    return TRUE;
}

/* 02466BD0 */
void daSalvage_c::send_agb() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    salvageSetGbaName(this, salvage_actor::dItemNo_GRAPPLING_HOOK_e, 0x40, 0x3F);
    u8 count = 0;
    for (s32 i = 0; i < 160; i++) {
        if (registry()->getKind(i) == 5 || !registry()->getKind(i))
            continue;
        if (!registry()->checkRegist(i) || !registry()->checkUsed(i))
            continue;
        s8 room = registry()->getRoomNo(i);
        if (room != (s8)dComIfGp_roomControl_getStayNo())
            continue;
        gabi::Local<cXyz> pos;
        salvageGetPos(registry(), pos.get(), i);
        f32 z = pos->z, x = pos->x, y = pos->y;
        room = registry()->getRoomNo(i);
        u8 name = gbaName;
        salvageDrawPoint(0x10, x, y, z, room, -32768, count, name, 0);
        count++;
    }
}

/* 02467D30 */
static void debug_print2() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}

/* 02466D30 */
BOOL daSalvage_c::_draw() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    calcAlpha();
    int count = 0;
    for (s32 i = 0; i < 160; i++) {
        if (!registry()->checkRegist(i) || !registry()->checkUsed(i) || !isEffectKind(i) ||
            !registry()->getDrawMode(i))
            continue;
        J3DModelData* data = this->mModelData;
        salvage_actor::Material* material = salvage_actor::firstMaterial(data);
        be<u8>* alpha = static_cast<be<u8>*>(registry()->getAlphaPtr(i));
        u8 value = (u8)*alpha;
        if (!material)
            break;
        salvage_actor::ColorBytes* color = material->getTevKColor(3);
        u8 old = (u8)color->rgba[3];
        color = material->getTevKColor(3);
        color->rgba[3] = value;
        color = material->getTevKColor(3);
        material->setTevKColor(3, color);
        // HD: update the linear material color as well as the GX konst color.
        gabi::Local<salvage_actor::Color> normalized, adjusted;
        colorFromGXColor(normalized->rgba, color->rgba);
        salvageScaleColor(adjusted.get(), normalized.get(), 1.0f);
        material->mColor.mFlags = material->mColor.mFlags | 0x400;
        salvage_actor::Color* output = salvageMaterialColor(&material->mColor, 10);
        f32 a = (f32)(u8)color->rgba[3] / 255.0f;
        f32 x = adjusted->rgba[0], z = adjusted->rgba[2], y = adjusted->rgba[1];
        output->rgba[1] = y;
        output->rgba[2] = z;
        output->rgba[0] = x;
        output->rgba[3] = a;
        J3DModel* model = (J3DModel*)(void*)this->mModels[count];
        set_mtx(model, i);
        mDoExt_brkAnm* brk = this->mpBrk;
        data = this->mModelData;
        f32 frame = brk->mFrameCtrl.mFrame;
        salvageEntryBrk(brk, data, frame);
        mDoExt_btkAnm* btk = this->mpBtk;
        data = this->mModelData;
        frame = btk->mFrameCtrl.mFrame;
        salvageEntryBtk(btk, data, frame);
        model = (J3DModel*)(void*)this->mModels[count];
        mDoExt_modelUpdateDL(model, 0);
        color = material->getTevKColor(3);
        color->rgba[3] = old;
        if (++count >= 16)
            break;
    }
    salvagePlayAnimation((mDoExt_brkAnm*)this->mpBrk);
    salvagePlayAnimation((mDoExt_btkAnm*)this->mpBtk);
    send_agb();
    return TRUE;
}

/* 02466928 */
void daSalvage_c::set_mtx(J3DModel* model, s32 id) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<cXyz> pos;
    salvageGetPos(registry(), pos.get(), id);
    gabi::Local<csXyz> rot;
    rot->x = 0;
    rot->y = 0;
    rot->z = 0;
    f32 x = pos->x, z = pos->z;
    if (daSea_ChkArea(x, z)) {
        f32 y1 = daSea_calcWave((f32)pos->x - 50.0f, (f32)pos->z - 50.0f);
        f32 y2 = daSea_calcWave((f32)pos->x - 50.0f, (f32)pos->z + 50.0f);
        f32 y3 = daSea_calcWave((f32)pos->x + 50.0f, (f32)pos->z - 50.0f);
        x = pos->x;
        z = pos->z;
        gabi::Local<cXyz> a, b, c;
        a->x = x - 50;
        a->y = y1;
        a->z = z - 50;
        b->x = x - 50;
        b->y = y2;
        b->z = z + 50;
        c->x = x + 50;
        c->y = y3;
        c->z = z - 50;
        gabi::Local<salvage_actor::Triangle> tri;
        salvageTriangle(tri.get(), a.get(), b.get(), c.get());
        salvageTriangleAngles(tri.get(), &rot->x, &rot->z);
        salvage_actor::Emitter* emitter = this->mEmitter;
        if (emitter) {
            s16 rx = rot->x, rz = rot->z, ry = rot->y;
            salvageEmitterRotation(rx, ry, rz, &emitter->mGlobalRotation);
        }
        x = pos->x;
        z = pos->z;
        pos->y = daSea_calcWave(x, z);
    } else {
        f32 water = salvageWaterHeight(pos.get());
        if (water != -1e9f)
            pos->y = water;
    }
    gabi::Local<cXyz> moved, scale;
    f32 y = pos->y;
    x = pos->x;
    z = pos->z;
    moved->x = x;
    moved->y = y;
    moved->z = z;
    scale->x = x;
    scale->y = y;
    scale->z = z;
    registry()->setPos(id, moved.get());
    salvageGetScale(registry(), scale.get(), id);
    f32 sx = scale->x, sy = scale->y, sz = scale->z;
    z = pos->z;
    y = pos->y;
    x = pos->x;
    model->mBaseScale.z = sz;
    model->mBaseScale.x = sx;
    model->mBaseScale.y = sy;
    PSMTXTrans(mDoMtx_stack_c::get(), x, y, z);
    s16 rz = rot->z, rx = rot->x, ry = rot->y;
    salvageRotateMatrix(mDoMtx_stack_c::get(), rx, ry, rz);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
}

/* 02465A48 */
BOOL daSalvage_c::isEffectKind(s32 id) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u8 kind = registry()->getKind(id);
    return kind <= 4 || kind == 6;
}

/* 02465C64 */
static cPhs_State daSalvageCreate(daSalvage_c* self) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return self->_create();
}

/* 02465D2C */
static BOOL daSalvageDelete(daSalvage_c* self) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return self->_delete();
}

/* 024664F0 */
static BOOL daSalvageExecute(daSalvage_c* self) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return self->_execute();
}

/* 02466F84 */
static BOOL daSalvageDraw(daSalvage_c* self) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return self->_draw();
}

/* 02467CCC */
static BOOL daSalvageIsDelete(void* self) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return TRUE;
}

// HD: compiler-generated and material conversion code of this translation unit.
/* 02466874 */
static void colorFromGXColor(be<f32>* out, be<u8>* in) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    f32 r = (f32)(u8)in[0] / 255.0f, g = (f32)(u8)in[1] / 255.0f;
    f32 b = (f32)(u8)in[2] / 255.0f, a = (f32)(u8)in[3] / 255.0f;
    out[0] = r;
    out[1] = g;
    out[2] = b;
    out[3] = a;
}

/* 02467CB8 */
static void daSalvage_HIO_c_dt(void* self, s32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (self && (flags & 1))
        operator_delete(self);
}

/* 02467CDC */
static void daSalvage_c_dt(daSalvage_c* self, s32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (self) {
        salvageActorDelete(self, 0);
        if (flags & 1)
            operator_delete(self);
    }
}

/* 02467B50 */
static void* daSalvage_HIO_c_ct(void* self) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (!self) {
        self = operator_new(sizeof(salvage_actor::Hio));
        if (!self)
            return nullptr;
    }
    auto* tuning = static_cast<salvage_actor::Hio*>(self);
    tuning->mCmapNear = 80;
    tuning->mVolumeNear = 15;
    tuning->mMaxDistance = 250;
    tuning->mDebugDraw = 0;
    tuning->mDebugPrint = 0;
    tuning->mForceEnabled = 0;
    tuning->mNo = -1;
    tuning->mDebugRegister = 0;
    tuning->mTypeOverride = 255;
    tuning->mUnused15 = 0;
    tuning->mUnused14 = 0;
    tuning->mCallback = 0;
    tuning->mKeepSalvage = 0;
    tuning->mDisappearFrame = 20;
    tuning->mOuterseaNear = 20;
    tuning->mEnemyDistance = 1000;
    tuning->mInsideNear = 10;
    tuning->__vtbl = salvage_actor::HioVtable;
    tuning->mDistance = 1000;
    return self;
}

/* 02467C18 */
static void __sinit_d_a_salvage_cpp() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    sinit_header_statics(gc_tu.headerStatics[0], gc_tu.headerStatics[1]);
    daSalvage_HIO_c_ct(&l_HIO);
}

// clang-format off
/* WWHD glue
str 100399C4 "SALVAGE_GETITEM"
str 100399E8 "SALVAGE_HAZURE"
str 100399D4 "SLVG_GETITEM_LEFT"
str 100399F8 "SLVG_HAZURE_LEFT"
str 10039A0C "d_a_salvage.cpp" in 02465A8C
str 10039988 "d_a_salvage.cpp" in 024656EC
str 10039A1C "mTagData_p != NULL" renamed
str 100399B0 "mModelData != NULL" renamed
str 10039998 "pbrk != NULL" renamed
str 100399A4 "pbtk != NULL" renamed
data m_arcname 101CFED4
data mTagData_p 10475634
data mSalvageId 10475638
data mNowRoomNo 10475654
data l_HIO 1046D800
data m_savelabel 1004BD1C
data l_salvage_depth 10039A5C
ext 025B44A0 void dSalvage_control_c::entry(fopAc_ac_c*, JPABaseEmitter*)
ext 025B48BC s32 dSalvage_control_c::checkRegist(s32)
ext 025B48DC void dSalvage_control_c::calcDistanceXZ()
ext 025B4998 void dSalvage_control_c::setPos(s32, cXyz*)
ext 025B49BC void dSalvage_control_c::setNowAlpha(s32, u32)
ext 025B49CC void dSalvage_control_c::setDrawMode(s32, s32)
ext 025B49DC void dSalvage_control_c::init()
ext 025B4A98 void dSalvage_control_c::init_end()
ext 025B4AD8 void dSalvage_control_c::init_room(s32)
ext 025B4B14 void dSalvage_control_c::init_one(s32)
ext 025B4B4C void dSalvage_control_c::setFlag(s32, u32)
ext 025B4B64 void dSalvage_control_c::clrFlag(s32, u32)
ext 025B4BE0 cXyz* dSalvage_control_c::getPosP(s32)
ext 025B4C54 f32 dSalvage_control_c::getR(s32)
ext 025B4C64 f32 dSalvage_control_c::getH(s32)
ext 025B4C74 s8 dSalvage_control_c::getRoomNo(s32)
ext 025B4C88 u8 dSalvage_control_c::getItemNo(s32)
ext 025B4C98 s32 dSalvage_control_c::getSwitchNo(s32)
ext 025B4CA8 u8 dSalvage_control_c::getSaveNo(s32)
ext 025B4CB8 u8 dSalvage_control_c::getType(s32)
ext 025B4CC8 u8 dSalvage_control_c::getKind(s32)
ext 025B4CD8 void* dSalvage_control_c::getAlphaPtr(s32)
ext 025B4CEC s32 dSalvage_control_c::getDrawMode(s32)
ext 025B4CFC f32 dSalvage_control_c::getDistance(s32)
ext 025B4D0C s32 dSalvage_control_c::checkUsed(s32)
ext 0200F4FC BOOL cLib_chaseUC(be<u8>*, u32, u32)
ext 025D5834 u32 salvageCreateActor(s32, u32, const cXyz*, s32, const csXyz*, const cXyz*, s32, SalvageCreateCallback)
ext 025B8140 void salvageCompleteMap(dSv_player_map_c*, s32)
ext 025B8B68 void salvageOnEventBit(dSv_event_c*, u32)
ext 025B89B4 void salvageOnOceanBit(dSv_ocean_c*, u32, u32)
ext 025B8B94 BOOL salvageIsEventBit(dSv_event_c*, u32)
ext 025B8A50 BOOL salvageIsOceanBit(dSv_ocean_c*, u32, u32)
ext 025BA0C0 BOOL salvageIsSwitch(dSv_info_c*, s32, u32)
ext 025B80C8 BOOL salvageIsOpenMap(dSv_player_map_c*, s32)
ext 025B8228 BOOL salvageIsCompleteMap(dSv_player_map_c*, s32)
ext 025B8308 BOOL salvageIsTriforceMap(dSv_player_map_c*, s32)
ext 025D7A58 void salvageOrderEvent(fopAc_ac_c*, s32, u32, u32, u32, u32)
ext 025D50BC void salvageActorDelete(fopAc_ac_c*, s32)
ext 02543F10 s16 salvageEventIdx(dEvent_manager_c*, const char*, u32)
ext 025A847C salvage_actor::Emitter* salvageSetParticle(dPa_control_c*, u32, u32, const cXyz*, const csXyz*, const cXyz*, u32, void*, s32, void*, void*, void*)
ext 02520460 cPhs_State salvageResLoad(request_of_phase_process_class*, const char*)
ext 025D63E8 BOOL salvageSolidHeap(fopAc_ac_c*, SalvageHeapCallback, u32)
ext 025DA088 void salvageSetGbaName(fopAc_ac_c*, u32, u32, u32)
ext 02590488 void salvageDrawPoint(u32, f32, f32, f32, s32, s32, u32, u32, u32)
ext 0201ADE0 void salvageSubtract(const cXyz*, cXyz*, const cXyz*)
ext 028E8DD0 f32 salvageSquareMag(const cXyz*)
ext 028F4384 f32 salvageSqrt(f32)
ext 024F1478 f32 salvageWaterHeight(const cXyz*)
ext 020190B8 void salvageTriangle(salvage_actor::Triangle*, const cXyz*, const cXyz*, const cXyz*)
ext 02017264 void salvageTriangleAngles(salvage_actor::Triangle*, be<s16>*, be<s16>*)
ext 028245AC void salvageEmitterRotation(s32, s32, s32, Mtx34*)
ext 025F19F8 void salvageRotateMatrix(Mtx34*, s32, s32, s32)
ext 026066C4 void* salvageObjectResource(dRes_control_c*, SafeString*, s32)
ext 025E80D0 mDoExt_brkAnm* salvageCreateBrk(mDoExt_brkAnm*)
ext 025E7C6C mDoExt_btkAnm* salvageCreateBtk(mDoExt_btkAnm*)
ext 025E8154 BOOL salvageInitBrk(mDoExt_brkAnm*, J3DModelData*, J3DAnmTevRegKey*, u32, s32, f32, s32, s32, u32, u32)
ext 025E7CE0 BOOL salvageInitBtk(mDoExt_btkAnm*, J3DModelData*, J3DAnmTextureSRTKey*, u32, s32, f32, s32, s32, u32, u32)
ext 025E83FC void salvageEntryBrk(mDoExt_brkAnm*, J3DModelData*, f32)
ext 025E7FC4 void salvageEntryBtk(mDoExt_btkAnm*, J3DModelData*, f32)
ext 025E742C void salvagePlayAnimation(mDoExt_baseAnm*)
ext 0274D458 void salvageScaleColor(salvage_actor::Color*, salvage_actor::Color*, f32)
ext 027F9F0C salvage_actor::Color* salvageMaterialColor(salvage_actor::MaterialColor*, u32)
ext 025D54C4 BOOL salvageSearchActor(u32, be<u32>*)
ext 025D57E4 void salvageDeleteActor(u32)
ext 025D5218 salvage_actor::Octopus* salvageFindActor(u32, be<u16>*)
ext 025510D0 BOOL salvageIsTriforce(u32)
ext 02556D14 s32 salvageDayNight()
ext 02560828 s32 salvageMoonType()
ext 02565DAC BOOL salvageMoonArrival()
ext 02520540 s32 salvageReverb(s32)
ext 0254407C BOOL salvageEventStart(dEvent_manager_c*, s32)
ext 025440C8 BOOL salvageEventEnd(dEvent_manager_c*, s32)
ext 025B4B7C void salvageGetPos(dSalvage_control_c*, cXyz*, s32)
ext 025B4BF0 void salvageGetScale(dSalvage_control_c*, cXyz*, s32)
*/
