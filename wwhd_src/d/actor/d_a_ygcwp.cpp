/**
 * Ganon's Castle warp portal, in the style of zeldaret/tww.
 * Verified against WWHD by d_a_ygcwp and d_a_ygcwp_oct04. Private game-derived source.
 */
#include "d/actor/d_a_ygcwp.h"

const gc_tu_data gc_tu = {.safeString = 0x10042F54};
const char daYgcwp_c::M_arcname[] = "Ygcwp";
static f32 l_playSpeed;
static f32 l_stoppedSpeed;
static f32 l_cullMinXZ;
static f32 l_cullMaxXZ;
static f32 l_cullMinY;
static f32 l_cullMaxY;
static f32 l_headerMin;
static f32 l_headerMax;
static YgcwpBrkTable M_brk_table;
static YgcwpBrkTable M_brk_mode_table;
static YgcwpActionTable M_act_table;
static YgcwpHeaderStatics l_headerStatics;
static YgcwpDestructorRecords l_destructorRecords;
static YgcwpActorVtable l_actorVtable;

/* 024E9E68 */
static BOOL solidHeapCB(daYgcwp_c* i_this) {
    return i_this->create_heap();
}

/* 024E9D04 */
BOOL daYgcwp_c::create_heap() {
    // HD: retain the three SafeString stack slots until this function returns.
    gabi::AutoFrame frame{gabi::current_function()};
    J3DModelData* mdl_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_YGCWP_BDL_YGCWP00_e);
    if (mdl_data == NULL) {
        JUT_ASSERT_fail("d_a_ygcwp.cpp", 0xBE, "mdl_data != 0");
        return FALSE;
    }
    mpModel = mDoExt_J3DModel__create(mdl_data, 0x80000, 0x11000222);
    if (mpModel == NULL)
        return FALSE;

    f32 speed = l_playSpeed;
    for (u32 i = 0; i < 2; i++) {
        J3DAnmTevRegKey* brk_p = (J3DAnmTevRegKey*)dComIfG_getObjectRes(M_arcname, M_brk_table.indices[i]);
        if (brk_p == NULL) {
            JUT_ASSERT_fail("d_a_ygcwp.cpp", 0xC9, "brk_p != 0");
            return FALSE;
        }
        u32 mode = M_brk_mode_table.indices[i];
        // HD: expanded animation init arguments, with the original full-width mode.
        if (!mBrkAnm[i].init(mdl_data, brk_p, TRUE, mode, speed, 0, -1, 0, 0))
            return FALSE;
    }
    return TRUE;
}

/* 024E9F4C */
s32 daYgcwp_c::_create() {
    u32 flags = actor_condition;
    if (!(flags & fopAcCnd_INIT_e)) {
        // HD: preserve the nullable placement constructor and the array constructor ABI.
        if (gabi::ea(this) != 0) {
            fopAc_ac_c_ct(this);
            __vtbl = gabi::ea(&l_actorVtable);
            YgcwpBrkArray_ct(&mBrkAnm[0], 2, sizeof(YgcwpBrkAnm), 0x025E80D0);
            flags = actor_condition;
        }
        actor_condition = flags | fopAcCnd_INIT_e;
    }
    s32 rt = dComIfG_resLoad(&mPhs, M_arcname);
    if (rt != cPhs_COMPLEATE_e)
        return rt;
    if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((const void*)solidHeapCB), 0))
        return cPhs_ERROR_e;

    J3DModel* model = mpModel;
    fopAcM_SetMtx(this, model ? &model->mBaseTransformMtx : NULL);
    init_mtx();
    u32 parameter = fopAcM_GetParam(this) & 0x0F;
    m2D8 = parameter < 3 ? (s32)parameter : 0;
    f32 speed = l_stoppedSpeed;
    mBrkAnm[1].setPlaySpeed(speed);
    if (!dComIfGs_isTmpBit(YGCWP_WARP_EVENT)) {
        speed = l_playSpeed;
        mCurBrk = 0;
    }
    mBrkAnm[0].setPlaySpeed(speed);
    Ygcwp_offTmpBit(dComIfGs_getTmpEvent(), YGCWP_WARP_EVENT);
    f32 low = l_cullMinXZ, high = l_cullMaxXZ;
    f32 bottom = l_cullMinY, top = l_cullMaxY;
    fopAcM_setCullSizeBox(this, low, bottom, low, high, top, high);
    return cPhs_COMPLEATE_e;
}

/* 024EA0DC */
BOOL daYgcwp_c::_delete() {
    dComIfG_resDeleteDemo(&mPhs, M_arcname);
    return TRUE;
}

/* 024E9E6C */
void daYgcwp_c::init_mtx() {
    // HD: all three scale loads precede the stores, as do all twelve matrix loads.
    J3DModel_setBaseScale(mpModel, &scale);
    f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
    mDoMtx_stack_c::transS(x, y, z);
    mDoMtx_stack_c::ZXYrotM(shape_angle);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}

/* 024EA10C */
void daYgcwp_c::make_shine() {
    dComIfGp_particle_set(ID_AK_SN_GANONCASTLEWARP00, &current.pos, NULL, &scale);
}

/* 024EA174 */
void daYgcwp_c::set_timer() {
    s32 staff = mStaffId;
    be<s32>* a_intP = dComIfGp_evmng_getMyIntegerP(staff, "Timer");
    mTimer = 0;
    if (a_intP != NULL)
        mTimer = (s32)*a_intP;
}

/* 024EA1DC */
BOOL daYgcwp_c::_execute() {
    YgcwpPlayer* player = (YgcwpPlayer*)dComIfGp_getPlayer(0);
    s32 staff = dComIfGp_evmng_getMyStaffId(M_arcname);
    mStaffId = staff;
    if (staff != -1) {
        s32 actIdx = dComIfGp_evmng_getMyActIdx(staff, M_act_table.names, 3, FALSE, 0);
        staff = mStaffId;
        if (dComIfGp_evmng_getIsAddvance(staff)) {
            // HD: both constants are read before dispatch, including unknown actions.
            f32 zero = l_stoppedSpeed, one = l_playSpeed;
            switch (actIdx) {
            case EVENT_WARP_START: {
                make_shine();
                mBrkAnm[1].setFrame(zero);
                mBrkAnm[1].setPlaySpeed(one);
                mCurBrk = 1;
                player->mNoResetFlg0 |= YgcwpPlayer::daPyFlg0_NO_DRAW;
                set_timer();
                s32 room = current.roomNo;
                // HD: retain the signed 32-bit reverb result used by this call site.
                s32 reverb = Ygcwp_getReverb(room);
                mDoAud_seStart(JA_SE_LK_FG_ROOM_WARP_OUT, &current.pos, 0, reverb);
                break;
            }
            case EVENT_WARP_APPEAR:
                set_timer();
                player->mNoResetFlg0 &= ~YgcwpPlayer::daPyFlg0_NO_DRAW;
                break;
            case EVENT_WARP_MAKE:
                mBrkAnm[0].setFrame(zero);
                mBrkAnm[0].setPlaySpeed(one);
                mCurBrk = 0;
                break;
            }
        }
        if ((u32)actIdx <= EVENT_WARP_APPEAR) {
            // HD: the countdown wraps as a 32-bit subtraction before its signed test.
            s32 timer = (s32)((u32)(s32)mTimer - 1u);
            mTimer = timer;
            if (timer <= 0) {
                staff = mStaffId;
                dComIfGp_evmng_cutEnd(staff);
            }
        }
    }
    for (u32 i = 0; i < 2; i++)
        mBrkAnm[i].play();
    return TRUE;
}

/* 024EA3C4 */
BOOL daYgcwp_c::_draw() {
    g_env_light.settingTevStruct(TEV_TYPE_BG0, &current.pos, &tevStr);
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    for (u32 i = 0; i < 2; i++) {
        if ((u32)(s32)mCurBrk == i) {
            J3DModel* model = mpModel;
            f32 frame = mBrkAnm[i].getFrame();
            J3DModelData* data = model->getModelData();
            mBrkAnm[i].entry(data, frame);
            break;
        }
    }
    mDoExt_modelUpdateDL(mpModel);
    return TRUE;
}

/* 024EA474 */
static s32 Mthd_Create(daYgcwp_c* i_this) { return i_this->_create(); }
/* 024EA478 */
static BOOL Mthd_Delete(daYgcwp_c* i_this) { return i_this->_delete(); }
/* 024EA47C */
static BOOL Mthd_Execute(daYgcwp_c* i_this) { return i_this->_execute(); }
/* 024EA480 */
static BOOL Mthd_Draw(daYgcwp_c* i_this) { return i_this->_draw(); }
/* 024EA584 */
static BOOL Mthd_IsDelete(daYgcwp_c*) { return TRUE; }

// HD: compiler-generated and template code of this translation unit.
/* 024EA484 */
static void __sinit_d_a_ygcwp_cpp() {
    l_headerStatics.zero[2] = 0;
    l_headerStatics.zero[0] = 0;
    l_headerStatics.zero[3] = 0;
    l_headerStatics.zero[1] = 0;
    __register_global_object(gabi::ea(l_destructorRecords.entries[0]));
    l_headerStatics.minimum = l_headerMin;
    l_headerStatics.maximum = l_headerMax;
    YgcwpFirstHeaderObject_ct(&l_headerStatics.first);
    __register_global_object(gabi::ea(l_destructorRecords.entries[1]));
    YgcwpSecondHeaderObject_ct(&l_headerStatics.second);
    __register_global_object(gabi::ea(l_destructorRecords.entries[2]));
}
/* 024EA518 */
static void SafeString_dtor(SafeString* i_this, s32 flags) {
    if (i_this != NULL && (flags & 1))
        operator_delete(i_this);
}
/* 024EA52C */
static void daYgcwp_c_dtor(daYgcwp_c* i_this, s32 flags) {
    if (i_this != NULL) {
        __dt((fopAc_ac_c*)i_this);
        if (flags & 1)
            operator_delete(i_this);
    }
}
/* 024EA580 */
static void SafeString_assureTerminationImpl(SafeString*) {}

/* WWHD glue: translation-unit data and other-unit calls.
str 10043010 "Ygcwp"
str 10042FB0 "d_a_ygcwp.cpp"
str 10042FC0 "mdl_data != 0"
str 10042FD0 "brk_p != 0"
str 10042FF0 "Timer"
data l_playSpeed 10042FAC
data l_stoppedSpeed 10042FDC
data l_cullMinXZ 10042FE0
data l_cullMaxXZ 10042FE4
data l_cullMinY 10042FE8
data l_cullMaxY 10042FEC
data l_headerMin 10042FF8
data l_headerMax 10042FFC
data M_brk_table 10043000
data M_brk_mode_table 10043008
data M_act_table 101D3B9C
data l_headerStatics 1046EC38
data l_destructorRecords 101D3BA8
data l_actorVtable 10042F6C
ext 025E8154 BOOL YgcwpBrkAnm::init(J3DModelData*, J3DAnmTevRegKey*, u32, u32, f32, s32, s32, u32, u32)
ext 025E83FC void YgcwpBrkAnm::entry(J3DModelData*, f32)
ext 028EFFD0 void YgcwpBrkArray_ct(YgcwpBrkAnm*, u32, u32, u32)
ext 025B8B7C void Ygcwp_offTmpBit(dSv_event_c*, u32)
ext 02520540 s32 Ygcwp_getReverb(s32)
ext 028ED6F8 void YgcwpFirstHeaderObject_ct(u8*)
ext 028EAB2C void YgcwpSecondHeaderObject_ct(u8*)
*/
