/**
 * d_a_spc_item01.cpp (WWHD)
 * Item - Special item (shield, joy pendant, knight's crest, ... placed by events)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_spc_item01.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "gc_api/gc_api.h"
#include "d/actor/d_a_spc_item01.h"

static dCcD_SrcCyl l_cyl_src;

/* 0248A0F0 */
void daSpcItem01_c::set_mtx() {
    s16 angleX = current.angle.x, angleY = current.angle.y, angleZ = current.angle.z; // HD: capture the three angles without a stack copy.
    f32 offsetY = 0.0f;
    switch (m_itemNo) {
    case dItemNo_KNIGHTS_CREST_e:
        offsetY = -24.0f;
        break;
    }
    ((SpcItemModel_view*)mpModel.get())->setBaseScale(scale);

    mDoMtx_stack_c::transS(current.pos.x, current.pos.y + offsetY, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), angleX, angleY, angleZ);

    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
}

/* 0248A288 */
BOOL daSpcItem01_c::_delete() {
    return DeleteBase(dItemDataView::getFieldArc(m_itemNo));
}

/* 0248A448 */
cPhs_State daSpcItem01_c::_create() {
    fopAcM_ct(this, daSpcItem01_c);

    m_itemNo = daSpcItem01_prm::getItemNo(this);
    if (m_itemNo == dItemNo_SHIELD_e && dComIfGs_isEventBit(0x0E20 /* dSv_event_flag_c::UNK_0E20 */)) {
        setLoadError();
        return cPhs_ERROR_e;
    }

    cPhs_State phase_state = dComIfG_resLoad(&mPhs, dItemDataView::getFieldArc(m_itemNo));
    if (phase_state == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(this, CheckFieldItemCreateHeap,
                                   dItemDataView::getHeapSize(m_itemNo)))
        {
            return cPhs_ERROR_e;
        }
        CreateInit();
    }

    return phase_state;
}

/* 0248A2A4 */
BOOL daSpcItem01_c::CreateInit() {
    set_mtx();
    fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
    mStts.Init(0, 0xFF, this);
    mCyl.Set(&l_cyl_src);
    mCyl.SetStts(&mStts);
    f32 tempVar1 = (f32)getHeight();
    f32 tempVar2 = (f32)getR();
    if (scale.x > 1.0f) {
        tempVar2 *= scale.x;
        tempVar1 *= scale.x;
    }
    mCyl.SetR(tempVar2);
    mCyl.SetH(tempVar1);
    mAcchCir.SetWall(30.0f, 30.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed);

    field_0x644 = daSpcItem01_prm::getFlag(this);
    gravity = -4.0f; /* fopAcM_SetGravity */
    switch (m_itemNo) {
    case dItemNo_SHIELD_e:
        scale.x = 1.5f;
        scale.y = 1.5f;
        scale.z = 1.5f;
        current.angle.x = 4000;
        current.angle.y = 4200;
        current.angle.z = 5200;
        gravity = 0.0f;
        break;
    }

    return TRUE;
}

/* 0248A1F4 */
BOOL daSpcItem01_c::_execute() {
    eyePos.copy(current.pos);
    attention_info.position.copy(current.pos);
    m_timer = m_timer + 1;
    set_effect();
    scale_anim();
    anim_play();
    move();
    rotate_item();
    /* setCol(): empty, not called in HD */
    set_mtx();
    return TRUE;
}

/* 02489E3C */
void daSpcItem01_c::set_effect() {
    if ((field_0x644 & 0x01) && dItemDataView::checkAppearEffect(m_itemNo) && !field_0x642 && m_itemNo != dItemNo_KNIGHTS_CREST_e) {
        dComIfGp_particle_setSimple(dItemDataView::getAppearEffect(m_itemNo), &current.pos);
    }
}

/* 02489ED0 */
void daSpcItem01_c::scale_anim() {
    if (isRupee(m_itemNo)) {
        cLib_chaseF(&scale.x, 1.0f, 0.05f);
        cLib_chaseF(&scale.y, 1.0f, 0.05f);
        cLib_chaseF(&scale.z, 1.0f, 0.05f);
    }
}

/* 02489F74 */
void daSpcItem01_c::anim_play() {
    f32 animPlayParam = 1.0f;
    if (m_itemNo == dItemNo_KNIGHTS_CREST_e) {
        animPlayParam = 0.0f;
    }
    animPlay(1.0f, 1.0f, 1.0f, 1.0f, animPlayParam);
}

/* 02489FA4 */
void daSpcItem01_c::move() {
    fopAcM_posMoveF(this, &mStts.m_cc_move);
    mAcch.CrrPos(dComIfG_Bgsp());
    switch (m_itemNo) {
    case dItemNo_SHIELD_e:
        break;
    case dItemNo_JOY_PENDANT_e:
        if (mAcch.ChkGroundLanding()) {
            speed.x = 0.0f;
            speed.y = 0.0f;
            speed.z = 0.0f;
            speedF = 0.0f;
        }
        break;
    default:
        if (mAcch.ChkGroundLanding()) {
            field_0x642 = field_0x642 + 1;
            f32 newGravity = field_0x63C * 0.62f;
            if (newGravity > gravity - 0.5f) {
                speedF = 0.0f;
            } else {
                speed.x = 0.0f;
                speed.y = -newGravity;
                speed.z = 0.0f;
            }
        }
        break;
    }

    if (speed.y != 0.0f) {
        field_0x63C = speed.y;
    }
}

/* 0248A08C */
void daSpcItem01_c::rotate_item() {
    if (isRupee(m_itemNo)) {
        if (field_0x642 == 0) {
            field_0x640 = current.angle.x + 0x2000;
        } else {
            field_0x640 = 0;
        }
        cLib_chaseAngleS(&current.angle.x, field_0x640, 0x2000);
    }
}

/* 02489E28 */
// HD: (not named by the matcher): DrawBase() is virtual in HD, called as a tail call
BOOL daSpcItem01_c::_draw() {
    return drawBaseVirtual();
}

/* 0248A654 */
void daSpcItem01_c::setTevStr() {
    s32 type = m_itemNo == dItemNo_KNIGHTS_CREST_e ? s32(TEV_TYPE_BG1) : s32(TEV_TYPE_ACTOR);
    SpcItemEnv_view* env = (SpcItemEnv_view*)dKy_getEnvlight();
    env->settingTevStruct(type, &current.pos, &tevStr);
    env = (SpcItemEnv_view*)dKy_getEnvlight();
    env->setLightTevColorType(mpModel, &tevStr);

    for (s32 modelIndex = 0; modelIndex < 2; modelIndex++) {
        if (mpModelArrow[modelIndex] != nullptr) {
            env = (SpcItemEnv_view*)dKy_getEnvlight();
            env->setLightTevColorType(mpModelArrow[modelIndex], &tevStr);
        }
    }
}

/* 02489E38 */
static BOOL daSpcItem01_Draw(daSpcItem01_c* i_this) {
    return i_this->_draw();
}

/* 0248A27C */
static BOOL daSpcItem01_Execute(daSpcItem01_c* i_this) {
    return i_this->_execute();
}

/* 0248A280 */
static BOOL daSpcItem01_IsDelete(daSpcItem01_c*) {
    return TRUE;
}

/* 0248A2A0 */
static BOOL daSpcItem01_Delete(daSpcItem01_c* i_this) {
    return i_this->_delete();
}

/* 0248A650 */
static cPhs_State daSpcItem01_Create(fopAc_ac_c* i_this) {
    return ((daSpcItem01_c*)i_this)->_create();
}

/* 0248A708 */
// HD: __sinit_d_a_spc_item01_cpp (new: compiler-generated header statics)
static void spc_item01_sinit() {
    sinit_header_statics(0x1046DF1C, 0x101D0AC4);
}

/* 0248A79C */
// HD: daSpcItem01_c deleting destructor (vtable 0x1003D0A8 slot 0xC): ~daItemBase_c
static void spc_item01_dtor(daSpcItem01_c* p, s32 flags) {
    if (p != nullptr) {
        __dt((daItemBase_c*)p);
        if (flags & 1)
            operator_delete(p);
    }
}

/* WWHD glue (tools/verify/autoglue.py)
 ext 02184288 u8 daItemBase_c::getHeight()
 ext 021842A0 u8 daItemBase_c::getR()
 data l_cyl_src 101D0A80
*/
