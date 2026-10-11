/**
 * d_a_obj_ospbox.cpp — surprise box.
 * GameCube names and flow, with the verified WWHD behavior.
 */
#include "d/actor/d_a_obj_ospbox.h"
using daObjOspbox::Act_c;

const gc_tu_data gc_tu = {
    .gndChk = {0x1002E160, 0x1002E170, 0x1002E190, 0x1002E180},
    .gndChkDt = {0x1002E130, 0x1002E150, 0x1002E110},
};
static const char M_arcname[] = "Ospbox";
static Mtx34 M_tmp_mtx;
static cXyz particleScale;
enum OspboxProcess { fpcNm_Obj_Ospbox_e = 0x54, fpcNm_KB_e = 0xDC };
// Obj_Ospbox, KB, KB, KB, Obj_Ospbox, Obj_Ospbox, Obj_Ospbox, Obj_Ospbox.
static const s16 pf_name[8] = {0x54, 0xDC, 0xDC, 0xDC, 0x54, 0x54, 0x54, 0x54};
static const u32 prm[4] = {0, 0x01FF0000, 0x01FF0001, 0x01FF0002};
struct OspboxVtable {
    u8 _00[0x2C];
    be<u32> mDraw;
    u8 _30[0xC];
    be<u32> mIsDelete;
};
// HD: cBgW lock and emitter life are fields absent from the shared views.
struct OspboxBgW {
    u8 _00[0x6C];
    be<u8> mFlags;
};
struct OspboxEmitter {
    u8 _00[0x60];
    be<u16> mLifeTime;
};

/* 02380D0C */
static s32 parameter(Act_c *a, s32 width, s32 shift) {
    u32 value = fopAcM_GetParam(a);
    // HD: the PowerPC shift forms produce zero when bit 5 is set.
    u32 mask = (width & 32) ? 0 : (1u << (width & 31));
    return ((shift & 32) ? 0 : (value >> (shift & 31))) & (mask - 1);
}
/* 02380290 */
s32 Act_c::Mthd_Create() {
    fopAcM_ct(this, Act_c);
    s32 phase = dComIfG_resLoad(&mPhase, M_arcname);
    if (phase == cPhs_COMPLEATE_e) {
        phase = MoveBGCreate(M_arcname, 7, 0, 0x8A0);
        if (phase != cPhs_COMPLEATE_e && phase != cPhs_ERROR_e)
            JUT_ASSERT_fail("d_a_obj_ospbox.cpp", 0xC2,
                            "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)");
    }
    return phase;
}
/* 02380444 */
BOOL Act_c::Mthd_Delete() {
    BOOL result = MoveBGDelete();
    ospbox_deleteRes(M_arcname);
    return result;
}
/* 02380480 */
BOOL Act_c::CreateHeap() {
    auto *data = (J3DModelData *)dComIfG_getObjectRes(M_arcname, 4, 0x1002E0E8);
    if (!data)
        JUT_ASSERT_fail("d_a_obj_ospbox.cpp", 0x86, "model_data != 0");
    J3DModel *model = mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
    mpModel = model;
    return model != nullptr;
}
/* 0238051C */
void Act_c::set_mtx() {
    PSMTXTrans(mDoMtx_stack_c::get(), (f32)current.pos.x, (f32)current.pos.y, (f32)current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), (s16)shape_angle.x, (s16)shape_angle.y,
                   (s16)shape_angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &M_tmp_mtx);
}
/* 023805F0 */
void Act_c::init_mtx() {
    J3DModel *model = mpModel;
    f32 y = scale.y, x = scale.x, z = scale.z;
    model->mBaseScale.x = x;
    model->mBaseScale.y = y;
    model->mBaseScale.z = z;
    set_mtx();
}
/* 02380610 */
void Act_c::set_ground() {
    if (m498 <= 0)
        return;
    mObjGndChk.SetActorPid(fopAcM_GetID(this));
    f32 y = current.pos.y, x = current.pos.x, z = current.pos.z;
    y = y + 100.0f;
    mObjGndChk.m_pos.x = x;
    mObjGndChk.m_pos.z = z;
    mObjGndChk.m_pos.y = y;
    f32 ground = dComIfG_Bgsp()->GroundCross(&mObjGndChk);
    m494 = ground;
    if (ground > -1000000000.0f)
        m498 = 0;
    else
        m498 = (s16)m498 - 1;
}
/* 023806CC */
void Act_c::init_ground() {
    m498 = 5;
    set_ground();
}
/* 023806D8 */
BOOL Act_c::Create() {
    u32 model = gabi::ea((J3DModel *)mpModel);
    cullMtx = model ? model + offsetof(J3DModel, mBaseTransformMtx) : 0;
    init_mtx();
    fopAcM_setCullSizeBox(this, -76.0f, -1.0f, -76.0f, 76.0f, 151.0f, 76.0f);
    mStts.Init(255, 255, this);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x1002E280));
    mCyl.SetStts(&mStts);
    f32 x = (f32)attention_info.position.x, y = current.pos.y + 75.0f,
        z = (f32)attention_info.position.z;
    mCyl.mGObjTg.mVec.copy(cXyz::Zero);
    u32 flags = (u32)mCyl.mGObjTg.mSPrm;
    attention_info.position.y = y;
    eyePos.y = y;
    eyePos.z = z;
    eyePos.x = x;
    mCyl.mGObjTg.mSPrm = flags | 4;
    init_ground();
    m49A = 2;
    return TRUE;
}
/* 023807DC */
void Act_c::make_item() {
    s32 index = parameter(this, PRM_SPEC_W, PRM_SPEC_S);
    s16 process = (s16)pf_name[index];
    if (process == fpcNm_Obj_Ospbox_e) {
        s32 item = parameter(this, PRM_ITEMNO_W, PRM_ITEMNO_S);
        s8 room = home.roomNo;
        fopAcM_createItemFromTable(&current.pos, item, 127, room, 0, &current.angle, 7, nullptr);
    } else {
        u32 params = (u32)prm[index];
        s8 room = home.roomNo;
        ospbox_create(process, params, &current.pos, room, &current.angle, nullptr, -1, nullptr);
    }
}
/* 023808A0 */
void Act_c::eff_break() {
    // HD: retain the GHS guard word and component store order.
    static u32 scaleInitialized;
    if (!scaleInitialized) {
        particleScale.x = 2.0f;
        particleScale.z = 1.0f;
        particleScale.y = 2.0f;
        scaleInitialized = 1;
    }
    // HD: one 12-byte guest temporary, with no constructor call.
    gabi::Local<cXyz> pos;
    f32 y = current.pos.y + 75.0f, z = current.pos.z, x = current.pos.x;
    pos->z = z;
    pos->x = x;
    pos->y = y;
    JPABaseEmitter *emitter =
        dPa_control_set(dComIfGp_getParticle(), 0, 0x3E6, pos.get(), nullptr, nullptr, 255, nullptr,
                        -1, &tevStr.mColorK0, &tevStr.mColorK0, &particleScale);
    if (emitter) {
        gabi::at<OspboxEmitter>(gabi::ea(emitter))->mLifeTime = 30;
        emitter->setAwayFromAxisSpeed(30.0f);
    }
}
/* 0238098C */
void Act_c::sound_break() {
    u32 sound = 0;
    if (mObjGndChk.mBgIndex < 256)
        sound = dComIfG_Bgsp()->GetMtrlSndId(&mObjGndChk);
    s8 room = current.roomNo;
    s32 reverb = dComIfGp_getReverb(room);
    mDoAud_seStart(0x69B6, &eyePos, sound, reverb);
}
/* 02380A00 */
BOOL Act_c::Execute(gptr<Mtx34> *output) {
    if (mCyl.ChkTgHit()) {
        make_item();
        eff_break();
        sound_break();
        fopAcM_delete(this);
    } else {
        u8 timer = m49A;
        if (timer) {
            timer = timer - 1;
            m49A = timer;
            if (!timer) {
                auto *bg = gabi::at<OspboxBgW>(gabi::ea((dBgW *)mpBgW));
                bg->mFlags = (u8)bg->mFlags | 0x80;
            }
        }
        ospbox_MoveCAtTg(&mCyl, &current.pos);
        cCcS_Set(dComIfG_Ccsp(), &mCyl);
    }
    set_ground();
    set_mtx();
    *output = &M_tmp_mtx;
    return TRUE;
}
/* 02380AF0 */
BOOL Act_c::Draw() {
    settingTevStruct(dKy_getEnvlight(), 0, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    // HD: the GameCube simple shadow is absent.
    return TRUE;
}
/* 02380C68 */
BOOL Act_c::Delete() {
    return TRUE;
}
/* 0238023C */
static void movebg_dtor(Act_c *a, s32 flags) {
    if (a) {
        __dt(static_cast<fopAc_ac_c *>(a));
        if (flags & 1)
            operator_delete(a);
    }
}
/* 02380B88 */
static s32 create(Act_c *a) {
    return a->Mthd_Create();
}
/* 02380B8C */
static BOOL remove(Act_c *a) {
    return a->Mthd_Delete();
}
/* 02380B90 */
static BOOL execute(Act_c *a) {
    return a->MoveBGExecute();
}
/* 02380B94 */
static BOOL draw(Act_c *a) {
    // HD: preserve this slot's indirect guest-call convention.
    auto *vt = gabi::at<OspboxVtable>(a->__vtbl);
    return gabi::call<BOOL>(vt->mDraw, a);
}
/* 02380BA4 */
static BOOL isDelete(Act_c *a) {
    // HD: preserve this slot's indirect guest-call convention.
    auto *vt = gabi::at<OspboxVtable>(a->__vtbl);
    return gabi::call<BOOL>(vt->mIsDelete, a);
}
/* 02380BB4 */
static void static_init() {
    sinit_header_statics(0x1046BB28, 0x101CC164);
}
/* 02380C48 */
static void empty_dtor(u8 *a, s32 flags) {
    if (a && (flags & 1))
        operator_delete(a);
}

/* WWHD glue
str 1002E278 "Ospbox"
str 1002E1A0 "d_a_obj_ospbox.cpp" in 02380290
str 1002E208 "d_a_obj_ospbox.cpp" in 02380480
str 1002E1B4 "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)"
str 1002E1F8 "model_data != 0"
data M_tmp_mtx 1046BB54
data particleScale 1046BB44
data scaleInitialized 1046BB50
data pf_name 1002E238
data prm 1002E248
ext 02520488 void ospbox_deleteRes(const char*)
ext 025D5834 u32 ospbox_create(s16, u32, cXyz*, s8, csXyz*, cXyz*, s32, void*)
ext 025165A4 void ospbox_MoveCAtTg(dCcD_Cyl*, cXyz*)
*/
