// Shared actor statics and helpers; GameCube names with verified HD register widths.
#include "d/d_com_static.h"

using Byte = be<u8>;
using Halfword = be<s16>;
using Word = be<u32>;
using SalvagePointer = gptr<ComStatic::SalvageControl>;
using SavePointer = gptr<ComStatic::SaveInfo>;
static Byte mEmitterNum;
static SalvagePointer mTagData_p;
static Byte mNowDaizaNum;
static Byte mNowItemNum;
static Byte m_keep_type;
static ComStatic::IballActors l_iballIDs;
static Byte daTitle_Kirakira_Sound_flag;
static Word M_S_pre_set_frame_LOD;
static Word M_S_pre_set_frame_FRRS;
static Halfword M_S_light_angle;
static Byte M_S_lod_access;
static Word mCounter0;
static SavePointer l_saveInfo;
static ComStatic::SinCosTable l_sincosTable;

using StandModelPointer = gptr<ComStatic::StandModel>;
using FloatConstant = be<f32>;
static cXyz l_zeroScale;
static csXyz l_zeroAngle;
static FloatConstant l_itemAngleRange;
static FloatConstant l_itemSpeedRange;
static FloatConstant l_itemVerticalSpeed;
static FloatConstant l_itemGravity;
static StandModelPointer l_currentModel;
static Mtx34 l_currentMtx;

static ComStatic::HeaderStatics l_headerStatics;
static ComStatic::PiPair l_pi;
static ComStatic::LightParameters l_smokeTemplate;
static ComStatic::SmokeTevstr m_smoke_tevstr;
static ComStatic::SmokeCallback m_smoke;

static constexpr u32 ERROR_PROCESS_ID = 0xFFFFFFFF;

// HD: free adapters preserve the verified signatures of GameCube static methods.
/* 025260D8 */
static void daSteamTag_init() {
    mEmitterNum = 0;
}

/* 025260E8 */
static void daSalvage_init_room(u32 roomNo) {
    ComStatic::SalvageControl* control = mTagData_p;
    if (control) SalvageControl_init_room(control, roomNo);
}

/* 02526100 */
static u32 daDai_getMaxDaiza() {
    return mNowDaizaNum;
}

/* 0252610C */
static u32 daDai_getDaizaSetItemNum() {
    return mNowItemNum;
}

/* 02526118 */
static void daArrow_setKeepType(u32 type) {
    // HD: caller's full register is truncated only by the byte store.
    m_keep_type = type;
}

/* 02526124 */
static void daIball_init() {
    for (u32 i = 0; i < 5; ++i) l_iballIDs.m_ib_actor[i] = ERROR_PROCESS_ID;
}

/* 02526144 */
static void daIball_regist(fopAc_ac_c* i_actor) {
    for (u32 i = 0; i < 5; ++i) {
        if (l_iballIDs.m_ib_actor[i] == ERROR_PROCESS_ID) {
            l_iballIDs.m_ib_actor[i] = i_actor ? u32(i_actor->mBsPcId) : ERROR_PROCESS_ID;
            return;
        }
    }
}

/* 02526180 */
static void daIball_remove(fopAc_ac_c* i_actor) {
    for (u32 i = 0; i < 5; ++i) {
        // HD: reload the actor ID on every iteration, including the null-actor fallback.
        u32 id = i_actor ? u32(i_actor->mBsPcId) : ERROR_PROCESS_ID;
        if (l_iballIDs.m_ib_actor[i] == id) {
            l_iballIDs.m_ib_actor[i] = ERROR_PROCESS_ID;
            return;
        }
    }
}

/* 025261C0 */
static void daIball_remove_old() {
    u32 iball_id = ERROR_PROCESS_ID;
    for (u32 i = 0; i < 5; ++i) {
        u32 id = l_iballIDs.m_ib_actor[i];
        if (id == ERROR_PROCESS_ID) return;
        if (id < iball_id) iball_id = id;
    }
    fopAc_ac_c* iball = fopAcM_SearchByID(iball_id);
    if (iball) {
        daIball_dead(iball);
        daIball_remove(iball);
    }
}

/* 0252624C */
static s32 daObj_Roten_getCreateCount() {
    s32 count = 0;
    for (u32 i = 0; i < 12; ++i) {
        // HD: the accessor call occurs for item zero too, before its unconditional count.
        ComStatic::SaveInfo* save = l_saveInfo;
        if (PlayerGetBagItem_isReserve(save->mGetBagItem, i) || i == 0) ++count;
    }
    if (count > 9) return 4;
    if (count > 6) return 3;
    if (count > 3) return 2;
    return 1;
}

/* 02526570 */
static void daTitle_Kirakira_Sound_flag_on() {
    daTitle_Kirakira_Sound_flag = 1;
}

/* 02526890 */
static s32 daObjLight_set_light_dif_angle_LOD(s32 angle) {
    u32 frame = mCounter0;
    bool changed = frame != M_S_pre_set_frame_FRRS;
    if (changed) M_S_light_angle = angle;
    M_S_pre_set_frame_LOD = frame;
    return changed;
}

/* 025268C8 */
static s32 daObjLight_renew_light_angle() {
    M_S_lod_access = 1;
    if (dKy_daynight_check() == 1) {
        ComStatic::SaveInfo* save = l_saveInfo;
        if (ComStatic_isEventBit(save->mEvent, 0x1C02) == 1) {
            daObjLight_set_light_dif_angle_LOD(s16(M_S_light_angle + 0x80));
            return 1;
        }
    }
    return 0;
}

/* 02526944 */
static s32 daObjLight_get_light_angle() {
    return s16(M_S_light_angle + 0xF6C2);
}

/* 0252695C */
static s32 daObjLight_set_light_dif_angle_FRRS(s32 angle) {
    u32 frame = mCounter0;
    bool changed = frame != M_S_pre_set_frame_LOD && frame - 1 == M_S_pre_set_frame_FRRS;
    if (changed) M_S_light_angle = angle;
    M_S_pre_set_frame_FRRS = frame;
    return changed;
}

/* 02526C80 */
static void dPa_smokeEcallBack_dt(void* callback, u32 flags) {
    // HD: retain the flags register forwarded to the deleting allocator entry.
    if (callback && (flags & 1)) ComStaticSmoke_delete(callback, flags);
}

/* 02526C94 */
static s32 daNpc_Md_getMaxFlyingTimer() {
    return 15 * 30;
}

/* 02526C9C */
static u32 daArrow_getKeepType() {
    return m_keep_type;
}

/* 02526CA8 */
static s32 daObjLight_get_light_dif_angle() {
    return M_S_light_angle;
}

/* 025269A8 */
static f32 daObjPirateship_getShipOffsetY(be<s16>* angle, be<s16>* speed, f64 scale) {
    // HD: the scale argument arrives as f64, and the original rounds before multiplying.
    s32 current = *angle;
    cLib_addCalcAngleS2(speed, u32(current + 0x4000) < 0x8001 ? 0x180 : 0x280, 0x10, 0x300);
    u32 sum = u16(*angle + *speed);
    *angle = sum;
    return gabi::fmuls_ppc(f32(scale), l_sincosTable.m[sum >> 3].sin);
}

/* 02526330 */
void daTagKbItem_c::dig_main() {
    enum { NONE = 0xFF, KB = 220 };
    ComStatic::SaveInfo* save = l_saveInfo;
    // HD: the switch test uses the tag's home room, while item creation uses its current room.
    if (ComStatic_isSwitch(save->mSavedata, mSwBitNo, home.roomNo)) return;
    if (!field_0x299 || field_0x298) return;
    gabi::Local<cXyz> scale;
    scale->x = l_zeroScale.x;
    scale->y = l_zeroScale.y;
    scale->z = l_zeroScale.z;
    gabi::Local<csXyz> angle;
    angle->x = s16(l_zeroAngle.x);
    angle->y = s16(l_zeroAngle.y);
    angle->z = s16(l_zeroAngle.z);
    if (!ComStatic_IsActor(mpActor)) return;
    fopAc_ac_c* pig = mpActor;
    if (!pig || pig->mProcName != KB) return;
    bool done = false;
    if (mItemNo != NONE) {
        // HD: these random helpers return f64, retaining the original unrounded intermediate.
        angle->y = s16(gabi::ftoi(ComStatic_cM_rndF(l_itemAngleRange)));
        f32 range = l_itemSpeedRange;
        f64 horizontal = ComStatic_cM_rndF(range);
        f64 vertical = ComStatic_cM_rndFX(range);
        fopAc_ac_c* item = ComStatic_fastCreateItem(&mpActor->current.pos, mItemNo, current.roomNo,
            angle.get(), scale.get(), mItemBitNo, 0, horizontal,
            f32(vertical + l_itemVerticalSpeed), l_itemGravity);
        if (item) item->actor_status = item->actor_status | fopAcStts_UNK4000_e;
        u32 sw = mSwBitNo;
        if (sw != NONE) {
            save = l_saveInfo;
            ComStatic_onSwitch(save->mSavedata, sw, home.roomNo);
        } else {
            fopAcM_delete(this);
        }
        done = true;
    }
    u32 enemy = mEnemyKind;
    if (enemy != NONE) {
        mpActor->home.angle.z = enemy;
        save = l_saveInfo;
        ComStatic_onSwitch(save->mSavedata, mSwBitNo, home.roomNo);
        fopAcM_delete(this);
        done = true;
    }
    if (done) field_0x298 = 1;
    field_0x299 = 0;
}

/* 02526560 */
void daTagKbItem_c::kb_dig(fopAc_ac_c* i_pigActor) {
    // HD: the pointer store precedes the request flag.
    mpActor = i_pigActor;
    field_0x299 = 1;
    dig_main();
}

/* 02526580 */
static s32 daStandItem_daiItemNodeCallBack(J3DNode* node, s32 calcTiming) {
    if (calcTiming) return 1;
    ComStatic::Joint* joint = ComStatic_getJoint(node);
    ComStatic::StandModel* model = l_currentModel;
    ComStatic::StandItem* i_this = model->mUserArea;
    u32 jntNo = joint->mJointNo;
    enum { STANDITEM = 462, PINWHEEL = 0x92, FOUNTAIN_IDOL = 0x95 };
    if (!i_this || !ComStatic_IsActor(i_this) || i_this->mProcName != STANDITEM) return 1;
    // HD: mMtxBuffer is at +0x2C here, and its flag is set before each matrix access.
    ComStatic::MtxBuffer* data = model->mMtxBuffer;
    data->mFlags = data->mFlags | 0x10;
    Mtx34* stack = mDoMtx_stack_c::get();
    PSMTXCopy(&data->mAnmMtx[jntNo], stack);
    u32 item = i_this->mItemNo;
    if (item == PINWHEEL) mDoMtx_XrotM(stack, i_this->m6B4);
    if (item == FOUNTAIN_IDOL) {
        if (jntNo == 0) PSMTXCopy(stack, &i_this->m630);
        else if (jntNo == 1) PSMTXCopy(stack, &i_this->m660);
    }
    data = model->mMtxBuffer;
    data->mFlags = data->mFlags | 0x10;
    // HD: the final animation matrix is copied through twelve float loads before any store.
    Mtx34* destination = &data->mAnmMtx[jntNo];
    f32 matrix[12];
    for (u32 i = 0; i < 12; ++i) matrix[i] = stack->m[i / 4][i % 4];
    for (u32 i = 0; i < 12; ++i) destination->m[i / 4][i % 4] = matrix[i];
    PSMTXCopy(stack, &l_currentMtx);
    return 1;
}

// HD: compiler-generated statics, smoke callback and repeated light-parameter blocks.
/* 02526A4C */
static void __sinit_d_com_static_cpp() {
    for (u32 i = 0; i < 4; ++i) l_headerStatics.mZero[i] = 0;
    __register_global_object(0x101D5EE8);
    l_headerStatics.mNegativePi = l_pi.negative;
    l_headerStatics.mPositivePi = l_pi.positive;
    ComStaticFirstHeaderObject_ct(&l_headerStatics.mFirstObject);
    __register_global_object(0x101D5EF4);
    ComStaticSecondHeaderObject_ct(&l_headerStatics.mSecondObject);
    __register_global_object(0x101D5F00);
    ComStaticSmoke_ct(&m_smoke, 0, 1, 1, 1);
    __register_global_object(0x101D5F0C);
    // HD: materialize the TEV template in the verified load/store order.
    f32 directionZ = f32(l_smokeTemplate.mDirection.z);
    s16 materialB = s16(l_smokeTemplate.mMaterialColor.b);
    s16 materialG = s16(l_smokeTemplate.mMaterialColor.g);
    m_smoke_tevstr.mLight.mMaterialColor.b = materialB;

    f32 positionZ = f32(l_smokeTemplate.mPosition.z);
    u8 colorA = u8(l_smokeTemplate.mColor.a);
    m_smoke_tevstr.mLight.mMaterialColor.g = materialG;

    f32 directionY = f32(l_smokeTemplate.mDirection.y);
    m_smoke_tevstr.mLight.mDirection.z = directionZ;
    m_smoke_tevstr.mLight.mDirection.y = directionY;
    m_smoke_tevstr.mLight.mPosition.z = positionZ;

    f32 positionX = f32(l_smokeTemplate.mPosition.x);
    f32 directionX = f32(l_smokeTemplate.mDirection.x);
    m_smoke_tevstr.mLight.mPosition.x = positionX;
    m_smoke_tevstr.mLight.mDirection.x = directionX;

    f32 parameter0 = f32(l_smokeTemplate.mParameters[0]);
    f32 parameter2 = f32(l_smokeTemplate.mParameters[2]);
    u8 colorR = u8(l_smokeTemplate.mColor.r);
    u8 colorG = u8(l_smokeTemplate.mColor.g);
    m_smoke_tevstr.mLight.mColor.r = colorR;

    s16 materialR = s16(l_smokeTemplate.mMaterialColor.r);
    f32 parameter7 = f32(l_smokeTemplate.mParameters[7]);
    m_smoke_tevstr.mLight.mColor.g = colorG;
    s16 materialA = s16(l_smokeTemplate.mMaterialColor.a);
    u8 colorB = u8(l_smokeTemplate.mColor.b);
    m_smoke_tevstr.mLight.mMaterialColor.a = materialA;
    m_smoke_tevstr.mLight.mParameters[0] = parameter0;

    f32 parameter1 = f32(l_smokeTemplate.mParameters[1]);
    f32 parameter3 = f32(l_smokeTemplate.mParameters[3]);
    m_smoke_tevstr.mLight.mParameters[2] = parameter2;
    m_smoke_tevstr.mLight.mParameters[3] = parameter3;
    m_smoke_tevstr.mLight.mParameters[1] = parameter1;
    m_smoke_tevstr.mLight.mMaterialColor.r = materialR;
    m_smoke_tevstr.mLight.mColor.a = colorA;
    m_smoke_tevstr.mHdCopy[0].mColor.g = colorG;
    m_smoke_tevstr.mHdCopy[0].mColor.r = colorR;
    m_smoke_tevstr.mHdCopy[0].mColor.b = colorB;

    f32 parameter4 = f32(l_smokeTemplate.mParameters[4]);
    f32 positionY = f32(l_smokeTemplate.mPosition.y);
    m_smoke_tevstr.mLight.mParameters[4] = parameter4;
    m_smoke_tevstr.mLight.mPosition.y = positionY;
    f32 parameter5 = f32(l_smokeTemplate.mParameters[5]);

    m_smoke_tevstr.mLight.mColor.b = colorB;
    m_smoke_tevstr.mHdCopy[0].mMaterialColor.r = materialR;
    m_smoke_tevstr.mHdCopy[0].mMaterialColor.g = materialG;
    m_smoke_tevstr.mHdCopy[0].mMaterialColor.b = materialB;
    m_smoke_tevstr.mHdCopy[0].mMaterialColor.a = materialA;

    m_smoke_tevstr.mHdCopy2.mPosition.y = positionY;
    m_smoke_tevstr.mHdCopy2.mPosition.x = positionX;
    m_smoke_tevstr.mHdCopy2.mPosition.z = positionZ;
    m_smoke_tevstr.mHdCopy2.mDirection.x = directionX;
    m_smoke_tevstr.mHdCopy2.mDirection.y = directionY;
    m_smoke_tevstr.mHdCopy2.mDirection.z = directionZ;

    f32 parameter6 = f32(l_smokeTemplate.mParameters[6]);
    m_smoke_tevstr.mLight.mParameters[5] = parameter5;
    m_smoke_tevstr.mLight.mParameters[6] = parameter6;
    m_smoke_tevstr.mHdCopy[0].mColor.a = colorA;
    m_smoke_tevstr.mHdCopy2.mColor.r = colorR;
    m_smoke_tevstr.mHdCopy2.mColor.g = colorG;
    m_smoke_tevstr.mHdCopy2.mColor.b = colorB;

    m_smoke_tevstr.mHdCopy[0].mParameters[7] = parameter7;
    m_smoke_tevstr.mHdCopy[0].mDirection.z = directionZ;
    m_smoke_tevstr.mHdCopy[0].mParameters[6] = parameter6;
    m_smoke_tevstr.mHdCopy[0].mDirection.y = directionY;
    m_smoke_tevstr.mHdCopy[0].mParameters[5] = parameter5;
    m_smoke_tevstr.mHdCopy[0].mDirection.x = directionX;
    m_smoke_tevstr.mHdCopy[0].mParameters[4] = parameter4;
    m_smoke_tevstr.mHdCopy[0].mPosition.z = positionZ;
    m_smoke_tevstr.mHdCopy[0].mParameters[3] = parameter3;
    m_smoke_tevstr.mHdCopy[0].mPosition.y = positionY;
    m_smoke_tevstr.mHdCopy[0].mParameters[2] = parameter2;
    m_smoke_tevstr.mHdCopy[0].mPosition.x = positionX;
    m_smoke_tevstr.mHdCopy[0].mParameters[0] = parameter0;
    m_smoke_tevstr.mHdCopy[0].mParameters[1] = parameter1;

    m_smoke_tevstr.mHdCopy2.mColor.a = colorA;
    m_smoke_tevstr.mHdCopy2.mMaterialColor.r = materialR;
    m_smoke_tevstr.mHdCopy2.mMaterialColor.g = materialG;
    m_smoke_tevstr.mHdCopy2.mMaterialColor.b = materialB;
    m_smoke_tevstr.mHdCopy2.mMaterialColor.a = materialA;
    m_smoke_tevstr.mHdCopy2.mParameters[0] = parameter0;
    m_smoke_tevstr.mHdCopy2.mParameters[1] = parameter1;
    m_smoke_tevstr.mHdCopy2.mParameters[2] = parameter2;
    m_smoke_tevstr.mHdCopy2.mParameters[3] = parameter3;
    m_smoke_tevstr.mHdCopy2.mParameters[4] = parameter4;
    m_smoke_tevstr.mHdCopy2.mParameters[5] = parameter5;
    m_smoke_tevstr.mHdCopy2.mParameters[6] = parameter6;
    m_smoke_tevstr.mHdCopy2.mParameters[7] = parameter7;
    m_smoke_tevstr.mLight.mParameters[7] = parameter7;

}

/* WWHD glue (tools/verify/autoglue.py):
data mEmitterNum 10475650
data mTagData_p 10475634
data mNowDaizaNum 10475655
data mNowItemNum 10475656
data m_keep_type 101D5F43
data l_iballIDs 1047563C
data daTitle_Kirakira_Sound_flag 101D5F49
data M_S_pre_set_frame_LOD 101D5F30
data M_S_pre_set_frame_FRRS 101D5F34
data M_S_light_angle 101D5F3C
data M_S_lod_access 101D5F4A
data mCounter0 101FF558
data l_saveInfo 101F84DC
data l_sincosTable 104A44F8
data l_zeroScale 101FFBA8
data l_zeroAngle 101FFB14
data l_itemAngleRange 1004BCC4
data l_itemSpeedRange 1004BCC8
data l_itemVerticalSpeed 1004BCCC
data l_itemGravity 1004BCD0
data l_currentModel 104B462C
data l_currentMtx 104B4868
data l_headerStatics 10475424
data l_pi 1004BCDC
data l_smokeTemplate 1016E414
data m_smoke_tevstr 10475460
data m_smoke 10475440
ext 028ED6F8 void ComStaticFirstHeaderObject_ct(be<u8>*)
ext 028EAB2C void ComStaticSecondHeaderObject_ct(be<u8>*)
ext 025A5C04 void ComStaticSmoke_ct(ComStatic::SmokeCallback*, u32, u32, u32, u32)
ext 025BA0C0 s32 ComStatic_isSwitch(void*, u32, s32)
ext 025B9E38 void ComStatic_onSwitch(void*, u32, s32)
ext 025D4604 s32 ComStatic_IsActor(fopAc_ac_c*)
ext 020198D8 f64 ComStatic_cM_rndF(f32)
ext 02019918 f64 ComStatic_cM_rndFX(f32)
ext 025D8AB0 fopAc_ac_c* ComStatic_fastCreateItem(cXyz*, u32, s32, csXyz*, cXyz*, u32, u32, f64, f32, f32)
ext 027F7878 ComStatic::Joint* ComStatic_getJoint(J3DNode*)
ext 025B4AD8 void SalvageControl_init_room(ComStatic::SalvageControl*, u32)
ext 0217BE98 void daIball_dead(fopAc_ac_c*)
ext 025B7840 s32 PlayerGetBagItem_isReserve(void*, u32)
ext 025B8B94 s32 ComStatic_isEventBit(void*, u32)
ext 0273AF40 void ComStaticSmoke_delete(void*, u32)
*/
