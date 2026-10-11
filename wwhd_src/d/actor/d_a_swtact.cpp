/**
 * d_a_swtact.cpp (WWHD)
 * Object - Wind Crest (Wind's Requiem blue floor decoration)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_swtact.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_swtact.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor_mng.h"

const gc_tu_data gc_tu = {.safeString = 0x1003ED54};
enum { SWTACT_VTBL = 0x1003ED6C };
static daSwTact_c::ArcName m_arcname;

static u32 checkPlayerStatus(u32 index, u32 flag) {
    dComIfG_inf_c* game = gabi::at<dComIfG_inf_c>(dComIfGp_ea());
    return game->play.mPlayerStatus[0][index] & flag;
}
static swtact_layout::PlayerVirtuals* getPlayerVirtuals(fopAc_ac_c* player) {
    return gabi::at<swtact_layout::PlayerVirtuals>(player->__vtbl);
}

namespace daSwTact_prm {
inline u32 getSwitchNo(daSwTact_c* i_this) { return (fopAcM_GetParam(i_this) >> 0) & 0xFF; }
inline u32 getAnswer(daSwTact_c* i_this) { return (fopAcM_GetParam(i_this) >> 8) & 0xFF; }
inline u32 getModel(daSwTact_c* i_this) { return (fopAcM_GetParam(i_this) >> 16) & 0x0F; }
}  // namespace daSwTact_prm

/* 024A38A8 */
bool daSwTact_c::_delete() {
    dComIfG_resDelete(&mPhs, m_arcname.get());
    return true;
}

/* 024A3710 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return ((daSwTact_c*)i_this)->CreateHeap();
}

/* 024A3674 */
BOOL daSwTact_c::CreateHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(m_arcname.get(), dRes_INDEX_ITACT_BDL_ITACT_e, gc_tu.safeString);
    if (modelData == nullptr) /* JUT_ASSERT(0xe1, modelData != NULL) */
        JUT_ASSERT_fail("d_a_swtact.cpp", 0xE1, "modelData != NULL");
    model = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    if (!model)
        return FALSE;
    return TRUE;
}

/* 024A3714 */
void daSwTact_c::CreateInit() {
    /* cull_size[] = {-120, -10, -120, 120, 10, 120} */
    mSwitchNo = daSwTact_prm::getSwitchNo(this);
    mAnswer = daSwTact_prm::getAnswer(this);
    if (daSwTact_prm::getModel(this) == 1) {
        cullMtx = gabi::ea(J3DModel_getBaseTRMtx(model)); /* fopAcM_SetMtx */
        f32 scaleZ = scale.z;
        f32 scaleX = scale.x;
        fopAcM_setCullSizeBox(this, -120.0f * scaleX, -10.0f, -120.0f * scaleZ, 120.0f * scaleX, 10.0f, 120.0f * scaleZ);
        mRadius = 100.0f;
    } else {
        mRadius = 50.0f;
    }
}

/* 024A396C */
void daSwTact_c::set_mtx() {
    if (daSwTact_prm::getModel(this) == 1) {
        J3DModel_setBaseScale(model, &scale);
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    }
}

/* 024A37CC */
cPhs_State daSwTact_c::_create() {
    /* fopAcM_ct(this, daSwTact_c): HD: fopAc_ac_c has a vtable */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = SWTACT_VTBL;
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    s32 result = cPhs_COMPLEATE_e;
    if (daSwTact_prm::getModel(this) == 1) {
        result = dComIfG_resLoad(&mPhs, m_arcname.get());

        if (result == cPhs_COMPLEATE_e) {
            if (!fopAcM_entrySolidHeap(this, CheckCreateHeap, m_heapsize)) {
                return cPhs_ERROR_e;
            }
        }
    }

    if (result == cPhs_COMPLEATE_e) {
        CreateInit();
    }

    return result;
}

// HD: the switch is lowered to a byte lookup table; keep tww's case structure.
/* 024A3948 */
s32 daSwTact_c::getAnswer() {
    switch (mAnswer) {
    case 0:
        return 0;
    case 1:
        return 1;
    case 2:
        return 5;
    case 3:
        return 2;
    case 4:
        return 3;
    case 5:
        return 4;
    case 0xFF:
    default:
        return -1;
    }
}

/* 024A3A48 */
bool daSwTact_c::_execute() {
    fopAc_ac_c* player = dComIfGp_getPlayer(0); /* daPy_getPlayerActorClass() */
    u32 stts1 = checkPlayerStatus(1, daPyStts1_WIND_WAKER_CONDUCT_e);

    if (player == nullptr || checkPlayerStatus(0, daPyStts0_SHIP_RIDE_e))
        return true;

    /* (player->current.pos - current.pos).absXZ() */
    cXyz d = player->current.pos - current.pos;
    cXyz xz;
    xz.x = d.x;
    xz.y = 0.0f;
    xz.z = d.z;
    f32 dist = std_sqrtf(PSVECSquareMag(&xz));
    // HD: retain the unordered-distance case in this branch.
    if (!(dist > getR())) {
        if (mPlayerStatus != stts1 && stts1 != 0) {
            // HD: resolve the virtual target before getAnswer; the actor ID is read afterward.
            u32 target = getPlayerVirtuals(player)->setTactZev.get();
            s32 answer = getAnswer();
            gabi::call_ptr(target, player, (u32)mBsPcId, answer, (u32)0);
        }

        s32 tactMusic = gabi::call_ptr<s32>(getPlayerVirtuals(player)->getTactMusic.get(), player);
        switch (mAnswer) {
        case 0:
            if (tactMusic == 0)
                mTrigger = true;
            break;
        case 1:
            if (tactMusic == 1)
                mTrigger = true;
            break;
        case 2:
            if (tactMusic == 5)
                mTrigger = true;
            break;
        case 3:
            if (tactMusic == 2)
                mTrigger = true;
            break;
        case 4:
            if (tactMusic == 3)
                mTrigger = true;
            break;
        case 5:
            if (tactMusic == 4)
                mTrigger = true;
            break;
        case 0xFF:
            if (tactMusic == 0 || tactMusic == 1 || tactMusic == 2 || tactMusic == 3 || tactMusic == 4 || tactMusic == 5)
                mTrigger = true;
            break;
        }
    }

    if (mTrigger != 0 && !dComIfGp_event_runCheck()) {
        dComIfGs_onSwitch(mSwitchNo, home.roomNo); /* fopAcM_onSwitch */
        mTrigger = 0;
    }

    mPlayerStatus = stts1;
    set_mtx();

    return true;
}

/* 024A38DC */
bool daSwTact_c::_draw() {
    if (daSwTact_prm::getModel(this) == 0)
        return TRUE;

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    mDoExt_modelUpdateDL(model);
    return TRUE;
}

/* 024A38A4 */
static cPhs_State daSwTact_Create(void* i_this) {
    return ((daSwTact_c*)i_this)->_create();
}

/* 024A38D8 */
static BOOL daSwTact_Delete(void* i_this) {
    return ((daSwTact_c*)i_this)->_delete();
}

/* 024A3944 */
static BOOL daSwTact_Draw(void* i_this) {
    return ((daSwTact_c*)i_this)->_draw();
}

/* 024A3C50 */
static BOOL daSwTact_Execute(void* i_this) {
    return ((daSwTact_c*)i_this)->_execute();
}

/* 024A3CFC */
static BOOL daSwTact_IsDelete(void* i_this) {
    return TRUE;
}

// HD: compiler-generated and template code of this translation unit.
/* 024A3C54 */
static void __sinit_d_a_swtact_cpp() {
    sinit_header_statics(0x1046E1B8, 0x101D158C);
}

/* 024A3CE8 */
static void SafeString_dt(SafeString* s, s32 flags) {
    if (s != nullptr && (flags & 1))
        operator_delete(s);
}

/* 024A3D04 */
static void daSwTact_c_dt(daSwTact_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 024A3D58 */
static void SafeString_assureTermination(SafeString*) {
}

/* WWHD glue
str 1003ED7C
str 1003ED8C "modelData != NULL"
data m_arcname 101D15B0
*/
