// Forsaken Fortress wooden barricade.
#include "d/actor/d_a_obj_majyuu_door.h"

const gc_tu_data gc_tu = {
    .safeString = 0x1002C364,
    .headerStatics = {0x1046A510, 0x101CAB24, 0x1046A51C},
};

// HD: private views for data absent from the shared emitter and host IO layouts.
struct MjDoorEmitter {
    u8 _00[0x247];
    be<u8> mGlobalAlpha;
};
struct MjDoorOffsets {
    be<f32> mValues[10];
};
struct MjDoorFloat {
    be<f32> mValue;
};
struct MjDoorPi {
    be<f32> mValues[2];
};
struct MjDoorVtable {
    be<u32> mEntry;
};
struct MjDoorSmokeScale {
    be<u32> mInitialized;
    cXyz mScale;
};
struct MjDoorWordVec {
    be<u32> x, y, z;
};
struct MjDoorModeTable {
    gabi::ptmf<void (daObj_MjDoor_c::*)()> mProc[2];
};
struct MjDoorModeData {
    be<u32> mInitialized;
    MjDoorModeTable mTable;
};
static daObj_MjDoorHIO_c l_HIO;
static MjDoorOffsets l_cyl_offsets;
static MjDoorFloat l_reg12;
static MjDoorPi l_pi;
static MjDoorVtable l_actorVtable;
static MjDoorVtable l_hioVtable;
static MjDoorVtable l_aabVtable;
static MjDoorSmokeScale l_smokeScale;
static MjDoorModeTable l_modeSource;
static MjDoorModeData l_modeData;
static dCcD_SrcCyl l_cyl_src;
static GXColor smoke_col;

enum { dRes_INDEX_S_MSPDO_BDL_S_MSPDO_e = 4, dRes_INDEX_S_MSPDO_DZB_S_MSPDO_e = 7 };
namespace MjDoor {
enum {
    cCcD_CoSPrm_Set_e = 1,
    cCcD_CoSPrm_IsOther_e = 8,
    cCcD_CoSPrm_VsGrpAll_e = 0x70,
    cCcD_TgSPrm_Set_e = 1,
    cCcD_TgSPrm_IsOther_e = 8
};
}
namespace dPa_name {
enum { ID_IT_SN_MJMON_HAHEN_L00 = 0x811A, ID_IT_SN_MJMON_HAMON00 = 0x811C, ID_IT_ST_MJMON_SMOKE00 = 0xA11E };
}
enum { JA_SE_OBJ_MJ_GATE_BREAK = 0x6978 };
using MjDoorCylConstructor = dCcD_Cyl* (*)(dCcD_Cyl*);
using MjDoorCylDestructor = void (*)(dCcD_Cyl*, s32);
// HD: retain the complete Set result and the four-argument smoke constructor.
s32 MjDoor_bgSet(dBgW*, cBgD_t*, u32, Mtx34*);
void MjDoor_smokeInit(dPa_smokeEcallBack*, GXColor*, dKy_tevstr_c*, s32);
s32 MjDoor_calcTimer(be<s32>*);
dBgW* MjDoor_bgCtor(dBgW*);
dCcD_Cyl* MjDoor_cylCtor(dCcD_Cyl*);
void MjDoor_constructCylinders(dCcD_Cyl*, u32, u32, MjDoorCylConstructor);
void MjDoor_destroyCylinders(dCcD_Cyl*, u32, u32, MjDoorCylDestructor, u32, u32);
void MjDoor_cylDestructor(dCcD_Cyl*, s32);
// HD: external callbacks require explicit guest function metadata.
static gabi::HostFn l_cylDestructorBinding(reinterpret_cast<const void*>(MjDoor_cylDestructor), 0x02515A70);

/* 0236F38C */
static BOOL createHeap_CB(fopAc_ac_c* actor) {
    return static_cast<daObj_MjDoor_c*>(actor)->_createHeap();
}

/* 0236F390 */
static void TgHitCallback(daObj_MjDoor_c* actor, dCcD_GObjInf*, fopAc_ac_c* attacker, dCcD_GObjInf*) {
    if (attacker && attacker->mProcName == fpcNm_BOMB_e)
        actor->health = static_cast<s8>(static_cast<u8>(actor->health) - 1);
}

/* 0236F3C8 */
void daObj_MjDoor_c::set_mtx() {
    f32 x = scale.x, y = scale.y, z = scale.z;
    mpModel->mBaseScale.set(x, y, z);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
    if (mMode != MODE_DELETE) {
        PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
        mpBgW->Move();
    }
}

/* 0236F264 */
BOOL daObj_MjDoor_c::_createHeap() {
    auto* modelData =
        static_cast<J3DModelData*>(dComIfG_getObjectRes("S_MSPDo", dRes_INDEX_S_MSPDO_BDL_S_MSPDO_e, gc_tu.safeString));
    if (!modelData)
        JUT_ASSERT_fail("d_a_obj_majyuu_door.cpp", 0xC7, "modelData != (0)");
    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    if (!mpModel)
        return FALSE;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(current.angle.y);
    PSMTXCopy(mDoMtx_stack_c::get(), &mMtx);
    mpBgW = MjDoor_bgCtor(nullptr);
    if (!mpBgW)
        return FALSE;
    auto* dzb =
        static_cast<cBgD_t*>(dComIfG_getObjectRes("S_MSPDo", dRes_INDEX_S_MSPDO_DZB_S_MSPDO_e, gc_tu.safeString));
    return MjDoor_bgSet(mpBgW, dzb, dBgW::MOVE_BG_e, &mMtx) == 0;
}

/* 0236F3BC */
void daObj_MjDoor_c::getArg() {
    field_0x2CC = static_cast<u8>(fopAcM_GetParam(this));
}

/* 0236F4CC */
void daObj_MjDoor_c::CreateInit() {
    mStts.Init(0xFF, 0, this);
    for (int i = 0; i < 10; ++i) {
        mCyl[i].Set(&l_cyl_src);
        mCyl[i].SetStts(&mStts);
        mCyl[i].mGObjTg.mHitCallback = gabi::guest_fn(reinterpret_cast<const void*>(TgHitCallback));
    }
    mCyl[1].SetR(95.0f);
    auto* bg = dComIfG_Bgsp();
    bg->Regist(mpBgW, this);
    set_mtx();
    modeWaitInit();
    health = 30;
    max_health = 30;
}

/* 0236F580 */
cPhs_State daObj_MjDoor_c::_create() {
    u32 flags = actor_condition;
    if (!(flags & fopAcCnd_INIT_e)) {
        if (this) {
            fopAc_ac_c_ct(this);
            __vtbl = gabi::ea(&l_actorVtable);
            MjDoor_smokeInit(&mSmoke, &smoke_col, &tevStr, 0);
            dCcD_Stts_ct(&mStts);
            MjDoor_constructCylinders(mCyl, 10, sizeof(dCcD_Cyl), MjDoor_cylCtor);
            flags = actor_condition;
        }
        actor_condition = flags | fopAcCnd_INIT_e;
    }
    cPhs_State phase = dComIfG_resLoad(&mPhs, "S_MSPDo");
    if (phase == cPhs_COMPLEATE_e) {
        getArg();
        u8 sw = field_0x2CC;
        if (sw != 0xFF && dComIfGs_isSwitch(sw, current.roomNo))
            return cPhs_ERROR_e;
        if (!fopAcM_entrySolidHeap(this, createHeap_CB, 0x820))
            return cPhs_ERROR_e;
        CreateInit();
    }
    return phase;
}

/* 0236F6D4 */
BOOL daObj_MjDoor_c::_delete() {
    dComIfG_resDelete(&mPhs, "S_MSPDo");
    dBgW* bg = mpBgW;
    if (bg && bg->ChkUsed()) {
        auto* manager = dComIfG_Bgsp();
        manager->Release(mpBgW);
    }
    // HD: destruction calls the virtual remove entry rather than the GameCube end call.
    mSmoke.remove();
    return TRUE;
}

/* 0236F8A0 */
void daObj_MjDoor_c::setCollision() {
    f32 offsets[10];
    for (int i = 0; i < 10; ++i)
        offsets[i] = l_cyl_offsets.mValues[i];
    u16 angle = current.angle.y;
    f32 sine = -cM_ssin(angle), cosine = cM_scos(angle);
    gabi::Local<cXyz> center;
    for (int i = 0; i < 10; ++i) {
        f32 x = current.pos.x, y = current.pos.y, offset = offsets[i], reg = l_reg12.mValue, z = current.pos.z;
        center->x = gabi::fmadds(offset, cosine, x);
        center->y = y - reg;
        center->z = gabi::fmadds(offset, sine, z);
        mCyl[i].SetC(center.get());
        mCyl[i].SetR(100.0f);
        mCyl[i].SetH(1350.0f);
        cCcS_Set(dComIfG_Ccsp(), &mCyl[i]);
    }
}

/* 0236F4C0 */
void daObj_MjDoor_c::modeWaitInit() {
    mMode = MODE_WAIT;
}

/* 0236FCD8 */
void daObj_MjDoor_c::modeWait() {
    setCollision();
    if (health < 0 || l_HIO.field_0x4)
        modeDeleteInit();
}

/* 0236F9F4 */
void daObj_MjDoor_c::smoke_set() {
    if (!l_smokeScale.mInitialized) {
        l_smokeScale.mInitialized = 1;
        l_smokeScale.mScale.x = 1.25f;
        l_smokeScale.mScale.z = 1.25f;
        l_smokeScale.mScale.y = 1.25f;
    }
    dComIfGp_particle_setToon(dPa_name::ID_IT_ST_MJMON_SMOKE00, &mPos, &mAngle, nullptr, 0xB9, &mSmoke, current.roomNo);
    if (mSmoke.mpEmitter)
        reinterpret_cast<MjDoorEmitter*>(mSmoke.mpEmitter.get())->mGlobalAlpha = 200;
}

/* 0236FAAC */
void daObj_MjDoor_c::modeDeleteInit() {
    u8 sw = field_0x2CC;
    if (sw != 0xFF)
        dComIfGs_onSwitch(sw, current.roomNo);
    s8 room = current.roomNo;
    mMode = MODE_DELETE;
    s32 reverb = dComIfGp_getReverb(room);
    mDoAud_seStart(JA_SE_OBJ_MJ_GATE_BREAK, &eyePos, 0, reverb);
    gabi::Local<cXyz> scale;
    scale->set(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < 2; ++i)
        dComIfGp_particle_set(dPa_name::ID_IT_SN_MJMON_HAHEN_L00 + i, &current.pos, &current.angle, scale.get(), 0xFF,
                              nullptr, -1, &tevStr.mColorK0, &tevStr.mColorK0, nullptr);
    gabi::Local<cXyz> pos;
    // HD: these copies use integer words and retain NaN payloads.
    auto* currentWords = reinterpret_cast<MjDoorWordVec*>(&current.pos);
    auto* posWords = reinterpret_cast<MjDoorWordVec*>(pos.get());
    u32 y = currentWords->y, z = currentWords->z, x = currentWords->x;
    posWords->z = z;
    posWords->x = x;
    posWords->y = y;
    pos->y = dBgS_GetWaterHeight(pos.get());
    for (int i = 0; i < 2; ++i)
        dComIfGp_particle_set(dPa_name::ID_IT_SN_MJMON_HAMON00 + i, pos.get(), &current.angle, scale.get());
    auto* smokeWords = reinterpret_cast<MjDoorWordVec*>(&mPos);
    y = currentWords->y;
    u16 ry = current.angle.y;
    smokeWords->y = y;
    mAngle.y = ry;
    z = currentWords->z;
    x = currentWords->x;
    smokeWords->z = z;
    smokeWords->x = x;
    u16 rx = current.angle.x, rz = current.angle.z;
    mAngle.x = rx;
    mAngle.z = rz;
    smoke_set();
    mSmokeTimer = 50;
    mDeleteTimer = 600;
    auto* bg = dComIfG_Bgsp();
    bg->Release(mpBgW);
    for (int i = 0; i < 10; ++i) {
        u32 co = mCyl[i].mObjCo.mSPrm, tg = mCyl[i].mObjTg.mSPrm;
        mCyl[i].mObjCo.mSPrm =
            co & ~(MjDoor::cCcD_CoSPrm_Set_e | MjDoor::cCcD_CoSPrm_IsOther_e | MjDoor::cCcD_CoSPrm_VsGrpAll_e);
        mCyl[i].mObjTg.mSPrm = tg & ~(MjDoor::cCcD_TgSPrm_Set_e | MjDoor::cCcD_TgSPrm_IsOther_e);
    }
}

/* 0236FD28 */
void daObj_MjDoor_c::modeDelete() {
    if (mSmoke.mpEmitter) {
        if (MjDoor_calcTimer(&mSmokeTimer)) {
            s32 timer = mSmokeTimer;
            if (timer <= 40)
                reinterpret_cast<MjDoorEmitter*>(mSmoke.mpEmitter.get())->mGlobalAlpha =
                    static_cast<u8>(u32(timer) * 5);
        } else {
            mSmoke.end();
        }
    }
    if (!MjDoor_calcTimer(&mDeleteTimer))
        fopAcM_delete(this);
}

/* 0236F750 */
BOOL daObj_MjDoor_c::_execute() {
    if (!l_modeData.mInitialized) {
        l_modeData.mInitialized = 1;
        memcpy_g(&l_modeData.mTable, &l_modeSource, sizeof(l_modeSource));
    }
    (this->*l_modeData.mTable.mProc[static_cast<u32>(mMode)])();
    fopAcM_posMoveF(this, nullptr);
    set_mtx();
    return FALSE;
}

/* 0236F834 */
BOOL daObj_MjDoor_c::_draw() {
    if (mMode == MODE_DELETE)
        return TRUE;
    auto* env = dKy_getEnvlight();
    settingTevStruct(env, 0, &current.pos, &tevStr);
    env = dKy_getEnvlight();
    setLightTevColorType(env, mpModel, &tevStr);
    mDoExt_modelUpdateDL(mpModel);
    return TRUE;
}

/* 0236F6D0 */
static cPhs_State daObj_MjDoorCreate(daObj_MjDoor_c* actor) {
    return actor->_create();
}
/* 0236F74C */
static BOOL daObj_MjDoorDelete(daObj_MjDoor_c* actor) {
    return actor->_delete();
}
/* 0236F830 */
static BOOL daObj_MjDoorExecute(daObj_MjDoor_c* actor) {
    return actor->_execute();
}
/* 0236F89C */
static BOOL daObj_MjDoorDraw(daObj_MjDoor_c* actor) {
    return actor->_draw();
}
/* 0236FF40 */
static BOOL daObj_MjDoorIsDelete(daObj_MjDoor_c*) {
    return TRUE;
}

// HD: compiler-generated construction, destruction and header statics.
/* 0236FDB8 */
daObj_MjDoorHIO_c* MjDoor_hioCtor(daObj_MjDoorHIO_c* obj) {
    if (!obj) {
        obj = static_cast<daObj_MjDoorHIO_c*>(operator_new(sizeof(daObj_MjDoorHIO_c)));
        if (!obj)
            return nullptr;
    }
    obj->field_0x4 = 0;
    obj->__vtbl = gabi::ea(&l_hioVtable);
    return obj;
}
/* 0236FE00 */
void __sinit_d_a_obj_majyuu_door_cpp() {
    sinit_header_statics_rodata(gc_tu.headerStatics[0], gc_tu.headerStatics[1], gc_tu.headerStatics[2],
                                gabi::ea(&l_pi));
    MjDoor_hioCtor(&l_HIO);
}
/* 0236FEA0 */
void MjDoor_SafeString_dtor(void* obj, u32 flags) {
    if (obj && (flags & 1))
        operator_delete(obj);
}
/* 0236FEB4 */
dCcD_Cyl* MjDoor_cylCtor(dCcD_Cyl* obj) {
    if (!obj) {
        obj = static_cast<dCcD_Cyl*>(operator_new(sizeof(dCcD_Cyl)));
        if (!obj)
            return nullptr;
    }
    dCcD_Cyl_ct(obj, gabi::ea(&l_aabVtable));
    return obj;
}
/* 0236FF48 */
void MjDoor_dtor(daObj_MjDoor_c* obj, u32 flags) {
    if (obj) {
        MjDoor_destroyCylinders(obj->mCyl, 10, sizeof(dCcD_Cyl), MjDoor_cylDestructor, 0, 0);
        dCcD_Stts_dt(&obj->mStts, 2);
        __dt(static_cast<fopAc_ac_c*>(obj));
        if (flags & 1)
            operator_delete(obj);
    }
}
/* 0236FFC8 */
void MjDoor_SafeString_assureTerminationImpl(void*) {}

/* WWHD glue
 str 1002C420
 str 1002C3AC
 str 1002C3C4
 data l_HIO 1046A508
 data l_cyl_offsets 1002C3E4
 data l_reg12 1047BCD0
 data l_pi 1002C418
 data l_actorVtable 1002C38C
 data l_hioVtable 1002C39C
 data l_aabVtable 1002C37C
 data l_smokeScale 1046A52C
 data l_modeSource 1002C354
 data l_modeData 101FDC10
 data l_cyl_src 101CAAE0
 data smoke_col 101CAB48
 ext 0200A030 s32 MjDoor_bgSet(dBgW*, cBgD_t*, u32, Mtx34*)
 ext 025A5CEC void MjDoor_smokeInit(dPa_smokeEcallBack*, GXColor*, dKy_tevstr_c*, s32)
 ext 0211D2F8 s32 MjDoor_calcTimer(be<s32>*)
 ext 024F23F4 dBgW* MjDoor_bgCtor(dBgW*)
 ext 028EFFD0 void MjDoor_constructCylinders(dCcD_Cyl*, u32, u32, MjDoorCylConstructor)
 ext 028F0164 void MjDoor_destroyCylinders(dCcD_Cyl*, u32, u32, MjDoorCylDestructor, u32, u32)
 ext 02515A70 void MjDoor_cylDestructor(dCcD_Cyl*, s32)
*/
