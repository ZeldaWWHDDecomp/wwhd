/** Salvage registration and distance tracking, WWHD. */
#include "d/d_salvage.h"
#include "gc_api/d/d_com_inf_game.h"

s32 isCompleteCollectMap(dSv_player_map_c*, u32);
s32 isOceanSvBit(dSv_ocean_c*, u32, u32);
s32 dKy_moon_type_chk();
void squareMagnitudeGuest(const cXyz*);
f32 sqrtMagnitudeGuest();
static salvage_layout::SaveLabels m_savelabel;

/* 025B44A0 */
void dSalvage_control_c::entry(fopAc_ac_c* pact, JPABaseEmitter* emtr) {
    if (pact == nullptr)
        JUT_ASSERT_fail("d_salvage.cpp", 139, "pact != NULL");
    s32 roomNo = pact->current.roomNo;
    u32 params = pact->mParameters;
    u32 switchNo = (u16)pact->home.angle.z & 255;
    u32 kind = params >> 28;
    u32 saveNo = (params >> 20) & 255;
    s32 cmapNo = (s8)(params >> 12);
    u32 itemNo = (params >> 4) & 255;
    u32 type = params & 15;
    u32 no;
    bool invalid = false;
    if (roomNo != 0) {
        u32 count = mRoomNum;
        cmapNo = roomNo;
        no = count + MAX_SEA;
        invalid = count > MAX_ROOM;
    } else {
        no = mSeaNum;
        invalid = no > MAX_SEA;
    }
    if (u32(cmapNo - 1) >= 64)
        invalid = true;
    if (kind == 2 || kind == 3) {
        if (saveNo > 15)
            invalid = true;
    } else if (kind == 0 && u32(saveNo - 1) >= 128) {
        invalid = true;
    }
    if (kind == 2 && switchNo == 255)
        return;
    if (invalid)
        return;
    dSalvage_info_c& info = mInfo[no];
    info.mFlag = 0;
    switch (kind) {
    case 0:
        if (isCompleteCollectMap(&dComIfGs_info()->mSavedata.mPlayer.mMap, saveNo - 1))
            return;
        if (((u32)(s16)pact->home.angle.z & 3) != dComIfGs_info()->mSavedata.mPlayer.mInfo.mRandomSalvagePoint)
            return;
        info.mFlag = info.mFlag | 1;
        break;
    case 2:
        if (!dComIfGs_isSwitch(switchNo, (s8)pact->home.roomNo))
            info.mFlag = info.mFlag | 1;
        [[fallthrough]];
    case 3:
        if (saveNo != 31 && isOceanSvBit(&dComIfGs_info()->mSavedata.mOcean, u32(cmapNo) & 255, saveNo))
            return;
        break;
    case 4:
        if (saveNo != 31 && isOceanSvBit(&dComIfGs_info()->mSavedata.mOcean, u32(cmapNo) & 255, saveNo))
            return;
        if (!dKy_daynight_check())
            info.mFlag = info.mFlag | 1;
        break;
    case 6:
        if (dKy_moon_type_chk() || dComIfGs_getEvent()->isEventBit(m_savelabel.values[saveNo]))
            info.mFlag = info.mFlag | 1;
        break;
    }
    info.mRoomNo = roomNo;
    info.mPos = pact->current.pos;
    info.mScale = pact->scale;
    info.mItemNo = itemNo;
    info.mType = type;
    info.mEmtr = emtr;
    info.mKind = kind;
    info.mSaveNo = saveNo;
    info.mSwitchNo = switchNo;
    f32 height = 1000.0f;
    f32 radius = 500.0f;
    if (kind == 0) {
        radius = 700.0f;
    } else if (kind != 5) {
        height = radius;
        radius = 400.0f;
    }
    // HD: retain the scale operand order of the PowerPC multiplication.
    info.mH = gabi::fmuls_ppc(height, pact->scale.y);
    info.mR = gabi::fmuls_ppc(radius, pact->scale.x);
    if (roomNo != 0)
        mRoomNum = mRoomNum + 1;
    else
        mSeaNum = mSeaNum + 1;
}

/* 025B48BC */
s32 dSalvage_control_c::checkRegist(s32 no) { return mInfo[no].mRoomNo != -1; }

/* 025B48DC */
void dSalvage_control_c::calcDistanceXZ() {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    cXyz difference;
    cXyz horizontal;
    for (s32 i = 0; i < MAX_TOTAL; ++i) {
        if (!checkRegist(i))
            continue;
        difference = player->current.pos - mInfo[i].mPos;
        horizontal.x = difference.x;
        horizontal.y = 0.0f;
        horizontal.z = difference.z;
        // HD: the square magnitude remains in guest FPR1 for the following sqrt call.
        squareMagnitudeGuest(&horizontal);
        mInfo[i].mDistance = sqrtMagnitudeGuest();
    }
}

/* 025B4998 */
void dSalvage_control_c::setPos(s32 no, cXyz* pos) { mInfo[no].mPos = *pos; }
/* 025B49BC */
void dSalvage_control_c::setNowAlpha(s32 no, u32 value) { mInfo[no].mNowAlpha = value; }
/* 025B49CC */
void dSalvage_control_c::setDrawMode(s32 no, s32 value) { mInfo[no].mDrawMode = value; }
/* 025B49DC */
void dSalvage_control_c::init() {
    for (s32 i = 0; i < MAX_TOTAL; ++i) {
        mInfo[i].mRoomNo = -1;
        mInfo[i].mEmtr = nullptr;
    }
    mSeaNum = 0;
    mRoomNum = 0;
}
/* 025B4A10 */
void dSalvage_control_c::init_one_sub(s32 no) {
    salvage_layout::Emitter* emtr = reinterpret_cast<salvage_layout::Emitter*>(mInfo[no].mEmtr.get());
    if (emtr != nullptr) {
        u32 status = emtr->mStatus;
        emtr->mMaxFrame = -1;
        emtr->mStatus = status | 1;
    }
    mInfo[no].mRoomNo = -1;
    mInfo[no].mPos = *cXyz_Zero;
    mInfo[no].mScale = *cXyz_Zero;
    mInfo[no].mSaveNo = 0;
    mInfo[no].mItemNo = 0;
    mInfo[no].mFlag = 0;
    mInfo[no].mSwitchNo = 0;
    mInfo[no].mType = 0;
    mInfo[no].mEmtr = nullptr;
}
/* 025B4A98 */
void dSalvage_control_c::init_end() {
    for (s32 i = 0; i < MAX_TOTAL; ++i)
        init_one_sub(i);
    mSeaNum = 0;
    mRoomNum = 0;
}
/* 025B4AD8 */
void dSalvage_control_c::init_room(s32 roomNo) {
    for (s32 i = MAX_SEA; i < MAX_TOTAL; ++i)
        init_one_sub(i);
    mRoomNum = 0;
}
/* 025B4B14 */
void dSalvage_control_c::init_one(s32 no) {
    init_one_sub(no);
    if (no >= MAX_SEA)
        mRoomNum = mRoomNum - 1;
    else
        mSeaNum = mSeaNum - 1;
}
/* 025B4B4C */
void dSalvage_control_c::setFlag(s32 no, u32 flag) { mInfo[no].mFlag = mInfo[no].mFlag | flag; }
/* 025B4B64 */
void dSalvage_control_c::clrFlag(s32 no, u32 flag) { mInfo[no].mFlag = mInfo[no].mFlag & ~flag; }

// HD: these two guest ABI adapters retain GHS hidden result storage, null allocation and float copies.
/* 025B4B7C */
void* getPosGuest(dSalvage_control_c* self, void* out, s32 no) {
    if (out == nullptr)
        out = operator_new(12);
    if (out != nullptr) {
        cXyz* result = static_cast<cXyz*>(out);
        result->x = self->mInfo[no].mPos.x;
        result->y = self->mInfo[no].mPos.y;
        result->z = self->mInfo[no].mPos.z;
    }
    return out;
}
/* 025B4BE0 */
cXyz* dSalvage_control_c::getPosP(s32 no) { return &mInfo[no].mPos; }
/* 025B4BF0 */
void getScaleGuest(dSalvage_control_c* self, void* out, s32 no) {
    if (out == nullptr)
        out = operator_new(12);
    if (out != nullptr) {
        cXyz* result = static_cast<cXyz*>(out);
        result->x = self->mInfo[no].mScale.x;
        result->y = self->mInfo[no].mScale.y;
        result->z = self->mInfo[no].mScale.z;
    }
}
/* 025B4C54 */
f32 dSalvage_control_c::getR(s32 no) { return mInfo[no].mR; }
/* 025B4C64 */
f32 dSalvage_control_c::getH(s32 no) { return mInfo[no].mH; }
/* 025B4C74 */
s8 dSalvage_control_c::getRoomNo(s32 no) { return mInfo[no].mRoomNo; }
/* 025B4C88 */
u8 dSalvage_control_c::getItemNo(s32 no) { return mInfo[no].mItemNo; }
/* 025B4C98 */
s32 dSalvage_control_c::getSwitchNo(s32 no) { return mInfo[no].mSwitchNo; }
/* 025B4CA8 */
u8 dSalvage_control_c::getSaveNo(s32 no) { return mInfo[no].mSaveNo; }
/* 025B4CB8 */
u8 dSalvage_control_c::getType(s32 no) { return mInfo[no].mType; }
/* 025B4CC8 */
u8 dSalvage_control_c::getKind(s32 no) { return mInfo[no].mKind; }
// HD: like GameCube, getDrawMode reads the alpha byte rather than mDrawMode.
/* 025B4CEC */
s32 dSalvage_control_c::getDrawMode(s32 no) { return mInfo[no].mNowAlpha; }
/* 025B4CFC */
f32 dSalvage_control_c::getDistance(s32 no) { return mInfo[no].mDistance; }
/* 025B4CD8 */
void* dSalvage_control_c::getAlphaPtr(s32 no) { return &mInfo[no].mNowAlpha; }
/* 025B4D0C */
s32 dSalvage_control_c::checkUsed(s32 no) { return !(mInfo[no].mFlag & 1); }

// HD: compiler-generated header statics of this translation unit.
/* 025B4D2C */
void __sinit_d_salvage_cpp() { sinit_header_statics(0x1047C8E4, 0x101EAD7C); }

/* WWHD glue
str 100535F0
str 100535E4 "pact != NULL"
ext 025B8228 s32 isCompleteCollectMap(dSv_player_map_c*, u32)
ext 025B8A50 s32 isOceanSvBit(dSv_ocean_c*, u32, u32)
ext 02560828 s32 dKy_moon_type_chk()
ext 028E8DD0 void squareMagnitudeGuest(const cXyz*)
ext 028F4384 f32 sqrtMagnitudeGuest()
data m_savelabel 1004BD1C
*/
