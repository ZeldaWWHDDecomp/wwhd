/**
 * d_a_sea.cpp (WWHD)
 * The sea surface: a 65x65 height grid around the player, animated by four travelling waves.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_sea.cpp) to the WWHD layout and verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_sea.h"

#define SAFESTRING_VTBL 0x10039E38 /* this TU's sead::SafeString vtable */
#define L_CLOTH_PTR 0x1046D8B0     /* HD: daSea_packet_c* to the function-local l_cloth */
#define L_CLOTH_GUARD 0x1046DC78
#define L_CLOTH_OBJ 0x1046D8D0
#define BASE_HEIGHT_EA 0x101D01A0  /* daSea_packet_c::BASE_HEIGHT */
#define WI_PRM_OCEAN 0x101D0110    /* wi_prm_ocean[4] */
#define POS_AROUND 0x101D00E0      /* s8 pos_around[8][2] */
#define STAY_NO_EA 0x1047E6C8      /* dStage_roomControl_c::mStayNo */

static inline daSea_packet_c* l_cloth() { return gabi::at<daSea_packet_c>(gabi::load<u32>(L_CLOTH_PTR)); }
static inline f32 BASE_HEIGHT() { return gabi::load<f32>(BASE_HEIGHT_EA); }
static inline s32 dComIfGp_roomControl_getStayNo() { return gabi::load<s8>(STAY_NO_EA); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void __construct_array_l(u32 p, u32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }
/* dStage_stageDt_c virtuals through the play object (play+0x5150): getMulti (+0x20C), getStagInfo (+0x15C) */
static inline u32 dComIfGp_getStage_vcall(u32 slot) {
    u32 st = dComIfGp_ea() + PLAY_STAGEDATA;
    return gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(st) + slot), st);
}
static inline u32 dComIfGp_getStage_getMulti() { return dComIfGp_getStage_vcall(0x20C); }
static inline u32 dComIfGp_getStageStagInfo() { return dComIfGp_getStage_vcall(0x15C); }
enum { dStageType_SEA_e = 7 };
static inline s32 dStage_stagInfo_GetSTType(u32 info) { return (gabi::load<u32>(info + 0xC) >> 16) & 7; }
/* 025C11DC dStage_roomControl_c::getStatusRoomDt(int) */
static inline u32 dComIfGp_roomControl_getStatusRoomDt(s32 roomNo) {
    u32 rc = dComIfGp_ea() + PLAY_ROOMCTRL;
    return gabi::call<u32>(0x025C11DC, rc, roomNo);
}
/* fopAcM_SearchByName (inline): the name is passed by address to fpcSch_JudgeForPName */
static inline fopAc_ac_c* fopAcM_SearchByName(s16 name) {
    gabi::Local<be<s16>> key;
    *key = name;
    return fopAcIt_Judge(0x025E121C /* fpcSch_JudgeForPName */, key.get());
}
static inline s16 cM_rad2s(f32 x) { return gabi::call<s16>(0x02019510, x); }
static inline void dKy_usonami_set(f32 v) { gabi::call(0x025601C0, v); }
static inline void cM3d_CalcPla(cXyz* a, cXyz* b, cXyz* c, cXyz* n, be<f32>* d) { gabi::call(0x02010D18, a, b, c, n, d); }
static inline void* operator_new_arr(u32 size) { return gabi::call<void*>(0x0273ADAC, size); } /* __nwa */
/* cM2dGBox (0x14: min, max, vtable): constructor / deleting destructor / Set / GetLen */
struct cM2dGBox_l {
    be<f32> mP0x, mP0y, mP1x, mP1y;
    be<u32> __vtbl;
};
static inline void cM2dGBox_ct(cM2dGBox_l* b) { gabi::call(0x02010594, b); }
static inline void cM2dGBox_dt(cM2dGBox_l* b, s32 flags) { gabi::call(0x020105F8, b, flags); }
static inline void cM2dGBox_Set(cM2dGBox_l* b, be<f32>* min, be<f32>* max) { gabi::call(0x0201060C, b, min, max); }
static inline f32 cM2dGBox_GetLen(cM2dGBox_l* b, be<f32>* xy) { return gabi::call<f32>(0x02010630, b, xy); }
/* sead::SafeString equality (HD inline; as d_a_kb): both sides' virtual assureTermination (+0x14),
 * then a byte compare bounded by 0x40001 */
static inline bool sea_SafeString_eq(SafeString* a, SafeString* b) {
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a);
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a);
    u32 s1 = a->mStringTop;
    gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b);
    if (s1 == b->mStringTop) return true;
    u32 p = a->mStringTop, q = b->mStringTop;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c = gabi::load<u8>(p + i);
        if (c != gabi::load<u8>(q + i)) return false;
        if (c == 0) return true;
    }
    return false;
}
/* strcmp(dComIfGp_getStartStageName(), lit) == 0 (HD: SafeString comparison, start stage name at play+0x5134) */
static inline bool sea_isStartStage(u32 lit) {
    gabi::Local<SafeString> a;
    a->mStringTop = lit;
    a->__vtbl = SAFESTRING_VTBL;
    gabi::Local<SafeString> b;
    b->mStringTop = dComIfGp_ea() + 0x5134;
    b->__vtbl = SAFESTRING_VTBL;
    return sea_SafeString_eq(a, b);
}
/* fmadds a*c+b with the recompiled model's NaN priority: the host (arm64) contracts the
 * recompiled double expression into one fused multiply-add, which returns the addend's NaN
 * first, then a's, then c's (all already quieted by the single->double loads) */
static inline f32 sea_fmadds_nan(f32 a, f32 c, f32 b) {
    if (b != b) return gabi::ppc_qnan(b);
    if (a != a) return gabi::ppc_qnan(a);
    if (c != c) return gabi::ppc_qnan(c);
    return gabi::fmadds(a, c, b);
}
/* a struct copy GHS does with integer loads/stores (bit-exact) */
static inline u32 f2u(f32 f) { u32 u; memcpy(&u, &f, 4); return u; }
static inline void copy_word(u32 dst, u32 src) { gabi::store<u32>(dst, gabi::load<u32>(src)); }

/* ======================================================================================== */

/* 0246B264 */
int get_wave_max(int roomNo) {
    WWHD_FUNC(0x0246B264, int, roomNo);
    u32 multi = dComIfGp_getStage_getMulti();
    if (multi == 0)
        return 10;

    s32 num = gabi::load<s32>(multi + 0);
    u32 entry = gabi::load<u32>(multi + 4);
    for (int i = 0; i < num; i++, entry += 0xC)
        if ((u32)roomNo == gabi::load<u8>(entry + 0xA)) /* dStage_Mult_info::mRoomNo */
            return gabi::load<u8>(entry + 0xB);      /* mWaveMax */

    return 10;
}
VERIFY(0x0246B264, get_wave_max);

/* 0246B2EC */
void calcMinMax(int v, be<f32>* min, be<f32>* max) {
    WWHD_FUNC(0x0246B2EC, void, v, min, max);
    f32 mn = gabi::fmsubs((f32)v, 100000.0f, 450000.0f);
    f32 mx = mn + 100000.0f;
    *min = mn;
    *max = mx;
}
VERIFY(0x0246B2EC, calcMinMax);

/* 0246B33C daSea_WaveInfo::daSea_WaveInfo() (HD: allocates when this == NULL; vtable at +0x28) */
static daSea_WaveInfo* daSea_WaveInfo_ct(daSea_WaveInfo* i_this) {
    WWHD_FUNC(0x0246B33C, daSea_WaveInfo*, i_this);
    if (i_this == nullptr) {
        i_this = (daSea_WaveInfo*)operator_new(0x2C);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x10039F28;
    for (s32 i = 0; i < 4; i++) {
        i_this->m04[i] = 0.0f;
        i_this->mCounters[i] = 0;
    }
    i_this->mCurScale = 1.0f;
    return i_this;
}
VERIFY(0x0246B33C, daSea_WaveInfo_ct);

/* 0246B3AC daSea_packet_c::daSea_packet_c() (HD: J3DPacket base, HD texture/GPU sub-objects) */
static daSea_packet_c* daSea_packet_c_ct(daSea_packet_c* i_this) {
    WWHD_FUNC(0x0246B3AC, daSea_packet_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daSea_packet_c*)operator_new(0x3A8);
        if (i_this == nullptr)
            return i_this;
    }
    gabi::call(0x027F1278, i_this); /* J3DPacket::J3DPacket */
    i_this->__vtbl = 0x10039F38;
    i_this->mWaterHeightMgr.__vtbl = 0x10039E50;
    daSea_WaveInfo_ct(&i_this->mWaveInfo);
    gabi::call(0x027B5430, gabi::at<void>(gabi::ea(i_this) + 0x234));
    gabi::call(0x027BE6B8, gabi::at<void>(gabi::ea(i_this) + 0x27C));
    gabi::call(0x027BE6B8, gabi::at<void>(gabi::ea(i_this) + 0x30C));
    return i_this;
}
VERIFY(0x0246B3AC, daSea_packet_c_ct);

/* 0246B42C daSea_Init() (HD: first use constructs the function-local l_cloth) */
void daSea_Init() {
    WWHD_FUNC(0x0246B42C, void);
    u32 p = gabi::load<u32>(L_CLOTH_PTR);
    if (p == 0) {
        p = L_CLOTH_OBJ;
        if (gabi::load<u32>(L_CLOTH_GUARD) == 0) {
            gabi::store<u32>(L_CLOTH_GUARD, 1);
            daSea_packet_c_ct(gabi::at<daSea_packet_c>(p));
            __register_global_object(0x101D0170);
        }
        gabi::store<u32>(L_CLOTH_PTR, p);
    }
    daSea_packet_c* c = gabi::at<daSea_packet_c>(p);
    c->mInitFlag = false;
    c->mCullStopFlag = true;
    c->m13A = true;
}
VERIFY(0x0246B42C, daSea_Init);

/* 0246B4B4 */
int daSea_WaterHeightInfo_Mng::Pos2Index(f32 v, be<f32>* dst) {
    WWHD_FUNC(0x0246B4B4, int, this, v, dst);
    f32 f = v + 450000.0f;
    int idx = gabi::ftoi(f / 100000.0f);
    if (dst != nullptr)
        *dst = gabi::fnmsubs((f32)idx, 100000.0f, f);
    return idx;
}
VERIFY(0x0246B4B4, &daSea_WaterHeightInfo_Mng::Pos2Index);

/* 0246B51C */
int daSea_WaterHeightInfo_Mng::GetHeight(int x, int z) {
    WWHD_FUNC(0x0246B51C, int, this, x, z);
    if ((u32)x >= 9 || (u32)z >= 9)
        return 10;

    if (dStage_stagInfo_GetSTType(dComIfGp_getStageStagInfo()) == dStageType_SEA_e) {
        return mHeight[z][x];
    } else {
        return get_wave_max(dComIfGp_roomControl_getStayNo());
    }
}
typedef int (daSea_WaterHeightInfo_Mng::*GetHeightII)(int, int);
typedef int (daSea_WaterHeightInfo_Mng::*GetHeightFF)(f32, f32);
VERIFY(0x0246B51C, static_cast<GetHeightII>(&daSea_WaterHeightInfo_Mng::GetHeight));

/* 0246B5E8 */
int daSea_WaterHeightInfo_Mng::GetHeight(f32 x, f32 z) {
    WWHD_FUNC(0x0246B5E8, int, this, x, z);
    int xi = Pos2Index(x, nullptr);
    int zi = Pos2Index(z, nullptr);
    return GetHeight(xi, zi);
}
VERIFY(0x0246B5E8, static_cast<GetHeightFF>(&daSea_WaterHeightInfo_Mng::GetHeight));

/* the shared bindings.h declares daSea_ChkArea / daSea_calcWave as guest-call bindings: this
 * file's definitions live in their own namespace */
namespace d_a_sea {

/* 0246B630 */
bool daSea_ChkAreaBeforePos(f32 x, f32 z) {
    WWHD_FUNC(0x0246B630, bool, x, z);
    if (l_cloth()->mInitFlag == 0) {
        return false;
    }

    if (l_cloth()->mWaterHeightMgr.GetHeight(x, z) == 0 && l_cloth()->mCullStopFlag != 0) {
        return false;
    }

    return true;
}
VERIFY(0x0246B630, daSea_ChkAreaBeforePos);

/* 0246B6A4 */
bool daSea_ChkArea(f32 x, f32 z) {
    WWHD_FUNC(0x0246B6A4, bool, x, z);
    if (!daSea_ChkAreaBeforePos(x, z)) {
        return false;
    }

    daSea_packet_c* c = l_cloth();
    if (c->mDrawMinX < x && x < c->mDrawMaxX && c->mDrawMinZ < z && z < c->mDrawMaxZ) {
        return true;
    }

    return false;
}
VERIFY(0x0246B6A4, daSea_ChkArea);

}  // namespace d_a_sea

/* 0246B764 */
f32 daSea_WaveInfo::GetScale(f32 v) {
    WWHD_FUNC(0x0246B764, f32, this, v);
    f32 cur = mCurScale;
    f32 r = gabi::fadds_ppc(cur, gabi::fsubs_ppc(v, cur) / 100.0f);
    mCurScale = r;
    return r;
}
VERIFY(0x0246B764, &daSea_WaveInfo::GetScale);

/* 0246B784 */
f32 daSea_WaveInfo::GetKm(int idx) {
    WWHD_FUNC(0x0246B784, f32, this, idx);
    return gabi::fmuls_ppc(6.28f, mWaveInfoTable.get()[idx].mKm);
}
VERIFY(0x0246B784, &daSea_WaveInfo::GetKm);

/* 0246B7A4 */
f32 daSea_WaveInfo::GetRatio(int idx) {
    WWHD_FUNC(0x0246B7A4, f32, this, idx);
    return (f32)mCounters[idx] / (f32)mWaveInfoTable.get()[idx].mCounterMax;
}
VERIFY(0x0246B7A4, &daSea_WaveInfo::GetRatio);

/* 0246B808 */
void daSea_WaterHeightInfo_Mng::GetArea(int x, int z, be<f32>* minX, be<f32>* minZ, be<f32>* maxX, be<f32>* maxZ) {
    WWHD_FUNC(0x0246B808, void, this, x, z, minX, minZ, maxX, maxZ);
    calcMinMax(x, minX, maxX);
    calcMinMax(z, minZ, maxZ);
}
VERIFY(0x0246B808, &daSea_WaterHeightInfo_Mng::GetArea);

/* 0246B848 */
f32 daSea_packet_c::CalcFlatInterTarget(cXyz* i_pos) {
    WWHD_FUNC(0x0246B848, f32, this, i_pos);
    cXyz& pos = *i_pos;
    gabi::Local<cM2dGBox_l> box;
    cM2dGBox_ct(box);
    gabi::Local<be<f32>[2]> xzPos;
    f32 px = pos.x, pz = pos.z; /* lfs/stfs (not a word copy) */
    (*xzPos)[1] = pz;
    (*xzPos)[0] = px;

    if (mWaterHeightMgr.GetHeight((int)mIdxX, (int)mIdxZ) == 0) {
        cM2dGBox_dt(box, 2);
        return 0.0f;
    }

    f32 result = 1.0f;
    gabi::Local<be<f32>[4]> mm; /* cXy min, max: one frame slot, reused by every iteration */

    for (int i = 0; i < 8; i++) {
        int iz = mIdxZ + gabi::load<s8>(POS_AROUND + 2 * i + 1);
        int ix = mIdxX + gabi::load<s8>(POS_AROUND + 2 * i + 0);

        if (mWaterHeightMgr.GetHeight(ix, iz) == 0) {
            be<f32>* a = *mm;
            mWaterHeightMgr.GetArea(ix, iz, &a[0], &a[1], &a[2], &a[3]);

            // 12800 = GRID_SIZE * 16?
            a[0] = a[0] - 12800.0f;
            a[1] = a[1] - 12800.0f;
            a[2] = a[2] + 12800.0f;
            a[3] = a[3] + 12800.0f;

            cM2dGBox_Set(box, &a[0], &a[2]);

            f32 len = cM2dGBox_GetLen(box, *xzPos);

            if (len > 12800.0f) {
                len = 12800.0f;
            }

            len /= 12800.0f;
            if (result > len) {
                result = len;
            }
        }
    }

    cM2dGBox_dt(box, 2);
    return result;
}
VERIFY(0x0246B848, &daSea_packet_c::CalcFlatInterTarget);

namespace d_a_sea {

/* 0246BA0C */
f32 daSea_calcWave(f32 x, f32 z) {
    WWHD_FUNC(0x0246BA0C, f32, x, z);
    if (!daSea_ChkArea(x, z)) {
        return BASE_HEIGHT();
    }

    daSea_packet_c* c = l_cloth();
    const f32 frac = 1.0f / 800;
    f32 minX = c->mDrawMinX;
    f32 minZ = c->mDrawMinZ;
    int x0 = gabi::ftoi((x - minX) * frac);
    int z0 = gabi::ftoi((z - minZ) * frac);

    u32 pY = c->mpHeightTable + x0 * 4 + z0 * 0x104;

    gabi::Local<cXyz> v00, v01, v10, v11, norm;
    gabi::Local<be<f32>> baseY;

    f32 v00x = (f32)(x0 * 800) + minX;
    v00->x = v00x;
    v00->y = gabi::load<f32>(pY);
    f32 v00z = (f32)(z0 * 800) + (f32)c->mDrawMinZ;
    v00->z = v00z;

    v01->x = v00x;
    v01->y = gabi::load<f32>(pY + 0x104);
    f32 v01z = v00z + 800.0f;
    v01->z = v01z;

    f32 v10x = v00x + 800.0f;
    v10->x = v10x;
    v10->y = gabi::load<f32>(pY + 4);
    v10->z = v00z;

    v11->x = v10x;
    v11->y = gabi::load<f32>(pY + 0x108);
    v11->z = v01z;

    f32 f0 = (z - v00z) * frac;
    f32 sum = gabi::fmadds(x - v00x, frac, f0);

    if (sum < 1.0f) {
        cM3d_CalcPla(v00, v01, v10, norm, baseY);
    } else {
        cM3d_CalcPla(v01, v10, v11, norm, baseY);
    }
    f32 s = gabi::fmadds(norm->x, x, norm->z * z) + (f32)*baseY;
    return -(s / norm->y);
}
VERIFY(0x0246BA0C, daSea_calcWave);

}  // namespace d_a_sea

/* 0246BC20 */
void daSea_packet_c::ClrFlat() {
    WWHD_FUNC(0x0246BC20, void, this);
    mFlags &= ~0x01;
    mFlatInterCounter = 150.0f;
}
VERIFY(0x0246BC20, &daSea_packet_c::ClrFlat);

/* 0246BC3C */
void daSea_packet_c::SetFlat() {
    WWHD_FUNC(0x0246BC3C, void, this);
    mFlags |= 0x01;
    mFlatTarget = 0.0f;
    mFlatInterCounter = 150.0f;
}
VERIFY(0x0246BC3C, &daSea_packet_c::SetFlat);

/* 0246BC64 */
void daSea_packet_c::CheckRoomChange() {
    WWHD_FUNC(0x0246BC64, void, this);
    u32 room = dComIfGp_roomControl_getStatusRoomDt(dComIfGp_roomControl_getStayNo());
    if (room != 0) {
        mRoomNo = dComIfGp_roomControl_getStayNo();
        fopAc_ac_c* octa = fopAcM_SearchByName(0xE1 /* fpcNm_DAIOCTA_e */);
        if (octa == nullptr) {
            if (mFlags & 0x01) {
                ClrFlat();
            }
        } else {
            /* daDaiocta_c::getSw() (0x690), fopAcM_GetHomeRoomNo */
            if (!dComIfGs_isSwitch(gabi::load<u8>(gabi::ea(octa) + 0x690), octa->home.roomNo)) {
                SetFlat();
            } else {
                ClrFlat();
            }
        }
    }
}
VERIFY(0x0246BC64, &daSea_packet_c::CheckRoomChange);

/* 0246BD4C */
void daSea_packet_c::CalcFlatInter() {
    WWHD_FUNC(0x0246BD4C, void, this);
    if (mFlags & 1) {
        if (mFlatInterCounter != 0.0f) {
            f32 cnt = mFlatInterCounter;
            f32 inter = mFlatInter;
            mFlatInterCounter = (f32)mFlatInterCounter - 1.0f;
            mFlatInter = inter + (mFlatTarget - inter) / cnt;
        } else {
            copy_word(gabi::ea(&mFlatInter), gabi::ea(&mFlatTarget));
        }
    } else {
        f32 target = CalcFlatInterTarget(&mPlayerPos);
        if (mFlatInterCounter != 0.0f) {
            f32 cnt = mFlatInterCounter;
            f32 inter = mFlatInter;
            mFlatInterCounter = (f32)mFlatInterCounter - 1.0f;
            mFlatInter = inter + (target - inter) / cnt;
        } else {
            mFlatInter = target;
        }
    }
}
VERIFY(0x0246BD4C, &daSea_packet_c::CalcFlatInter);

/* 0246BEA4 */
void daSea_packet_c::SetCullStopFlag() {
    WWHD_FUNC(0x0246BEA4, void, this);
    if (sea_isStartStage(0x10039E94 /* "A_umikz" */)) {
        mCullStopFlag = false;
        return;
    }

    if (mWaterHeightMgr.GetHeight((int)mIdxX, (int)mIdxZ) != 0) {
        mCullStopFlag = false;
        return;
    }

    gabi::Local<be<f32>[4]> area; /* minX, minZ, maxX, maxZ */
    be<f32>* a = *area;
    mWaterHeightMgr.GetArea(mIdxX, mIdxZ, &a[0], &a[1], &a[2], &a[3]);

    // 25600 = GRID_SIZE * 32?
    f32 minX = a[0] + 25600.0f;
    f32 minZ = a[1] + 25600.0f;
    f32 maxX = a[2] - 25600.0f;
    f32 maxZ = a[3] - 25600.0f;

    f32 px = mPlayerPos.x;
    if (minX < px && px < maxX) {
        f32 pz = mPlayerPos.z;
        if (minZ < pz && pz < maxZ) {
            mCullStopFlag = true;
            return;
        }
    }

    mCullStopFlag = false;
}
VERIFY(0x0246BEA4, &daSea_packet_c::SetCullStopFlag);

/* 0246C03C daSea_WaveInfo::AddCounter() (not named by the matcher) */
void daSea_WaveInfo::AddCounter() {
    WWHD_FUNC(0x0246C03C, void, this);
    for (s32 i = 0; i < 4; i++) {
        s32 c = mCounters[i] + 1;
        mCounters[i] = c;
        if (c >= mWaveInfoTable.get()[i].mCounterMax)
            mCounters[i] = 0;
    }
}
VERIFY(0x0246C03C, &daSea_WaveInfo::AddCounter);

/* 0246C088 */
void daSea_packet_c::execute(cXyz* i_pos) {
    WWHD_FUNC(0x0246C088, void, this, i_pos);
    cXyz& pos = *i_pos;
    f32 px = pos.x;
    mPlayerPos.copy(pos);
    mIdxX = mWaterHeightMgr.Pos2Index(px, (be<f32>*)nullptr);
    mIdxZ = mWaterHeightMgr.Pos2Index(mPlayerPos.z, (be<f32>*)nullptr);

    /* HD: no "ADMumi" flat override */
    s32 stay = dComIfGp_roomControl_getStayNo();
    if ((u32)mRoomNo != (u32)stay && stay != 0) {
        CheckRoomChange();
    }

    CalcFlatInter();
    dKy_usonami_set(mFlatInter);
    // 25600 = GRID_SIZE * 32?
    mDrawMinX = pos.x - 25600.0f;
    mDrawMaxX = pos.x + 25600.0f;
    mDrawMinZ = pos.z - 25600.0f;
    mDrawMaxZ = pos.z + 25600.0f;
    SetCullStopFlag();

    /* HD: no early return when mCullStopFlag == 1; the grid is always computed */

    int h = mWaterHeightMgr.GetHeight((f32)pos.x, (f32)pos.z);

    f32 gs = mWaveInfo.GetScale((f32)h);
    f32 scale = gabi::fmuls_ppc(mFlatInter, gs);

    s16 aOffsAnimTable[4];
    f32 aThetaXTable[4];
    f32 aThetaZTable[4];
    f32 aHeightTable[4];

    u32 waveTab = mWaveInfo.mWaveInfoTable.v; /* loaded once, before the loop */
    for (int i = 0; i < 4; i++) {
        u32 dat = waveTab + 0x18 * i;
        f32 km = mWaveInfo.GetKm(i);
        aThetaXTable[i] = gabi::fmuls_ppc(km, gabi::load<f32>(dat + 0xC)); /* GetVx */
        km = mWaveInfo.GetKm(i);
        aThetaZTable[i] = gabi::fmuls_ppc(km, gabi::load<f32>(dat + 0x10)); /* GetVz */

        aOffsAnimTable[i] = (s16)gabi::ftoi(65536.0f * (mWaveInfo.GetRatio(i) - 0.5f));
        aHeightTable[i] = gabi::fmuls_ppc(scale, gabi::load<f32>(dat + 0)); /* GetBaseHeight */
    }

    f32 sz = mDrawMinZ + 800.0f;

    s16 theta[4];
    s16 t0 = cM_rad2s(gabi::fmuls_ppc(aThetaZTable[0], sz));
    s16 t1 = cM_rad2s(gabi::fmuls_ppc(aThetaZTable[1], sz));
    s16 t2 = cM_rad2s(gabi::fmuls_ppc(aThetaZTable[2], sz));
    s16 t3 = cM_rad2s(gabi::fmuls_ppc(aThetaZTable[3], sz));
    u32 tab = mWaveInfo.mWaveInfoTable.v;
    theta[0] = t0 - aOffsAnimTable[0] + gabi::load<s16>(tab + 0x00 + 8); /* GetPhai */
    theta[1] = t1 - aOffsAnimTable[1] + gabi::load<s16>(tab + 0x18 + 8);
    theta[2] = t2 - aOffsAnimTable[2] + gabi::load<s16>(tab + 0x30 + 8);
    theta[3] = t3 - aOffsAnimTable[3] + gabi::load<s16>(tab + 0x48 + 8);

    s16 waveTheta_DeltaZ[4];
    waveTheta_DeltaZ[0] = cM_rad2s(gabi::fmuls_ppc(aThetaZTable[0], 800.0f));
    waveTheta_DeltaZ[1] = cM_rad2s(gabi::fmuls_ppc(aThetaZTable[1], 800.0f));
    waveTheta_DeltaZ[2] = cM_rad2s(gabi::fmuls_ppc(aThetaZTable[2], 800.0f));
    waveTheta_DeltaZ[3] = cM_rad2s(gabi::fmuls_ppc(aThetaZTable[3], 800.0f));

    s16 waveTheta_DeltaX[4];
    waveTheta_DeltaX[0] = cM_rad2s(gabi::fmuls_ppc(aThetaXTable[0], 800.0f));
    waveTheta_DeltaX[1] = cM_rad2s(gabi::fmuls_ppc(aThetaXTable[1], 800.0f));
    waveTheta_DeltaX[2] = cM_rad2s(gabi::fmuls_ppc(aThetaXTable[2], 800.0f));
    waveTheta_DeltaX[3] = cM_rad2s(gabi::fmuls_ppc(aThetaXTable[3], 800.0f));

    f32 sx = mDrawMinX + 800.0f;

    s16 waveTheta_Phase[4];
    waveTheta_Phase[0] = cM_rad2s(gabi::fmuls_ppc(aThetaXTable[0], sx));
    waveTheta_Phase[1] = cM_rad2s(gabi::fmuls_ppc(aThetaXTable[1], sx));
    waveTheta_Phase[2] = cM_rad2s(gabi::fmuls_ppc(aThetaXTable[2], sx));
    waveTheta_Phase[3] = cM_rad2s(gabi::fmuls_ppc(aThetaXTable[3], sx));

    f32 aFadeTable[65];
    for (int fadeZ = 0; fadeZ < 65; fadeZ++) {
        aFadeTable[fadeZ] = 1.0f;
    }

    f32 frac = 1.0f / 6;
    aFadeTable[65 - 1] = frac * 0;
    aFadeTable[0] = frac * 0;
    aFadeTable[65 - 2] = frac * 1;
    aFadeTable[1] = frac * 1;
    aFadeTable[65 - 3] = frac * 2;
    aFadeTable[2] = frac * 2;
    aFadeTable[65 - 4] = frac * 3;
    aFadeTable[3] = frac * 3;
    aFadeTable[65 - 5] = frac * 4;
    aFadeTable[4] = frac * 4;
    aFadeTable[65 - 6] = frac * 5;
    aFadeTable[5] = frac * 5;

    for (int z = 1; z < 65 - 1; z++) {
        u32 pHeight = mpHeightTable + 4 * (65 * z + 1);
        s16 waveTheta0 = waveTheta_Phase[0] + theta[0];
        s16 waveTheta1 = waveTheta_Phase[1] + theta[1];
        s16 waveTheta2 = waveTheta_Phase[2] + theta[2];
        s16 waveTheta3 = waveTheta_Phase[3] + theta[3];

        for (int x = 1; x < 65 - 1; x++) {
            /* the height table may overlap itself unaligned, so NaN payloads reach memory bit for bit */
            f32 y = sea_fmadds_nan(aHeightTable[0], cM_scos(waveTheta0),
                                   gabi::fmuls_ppc(aHeightTable[1], cM_scos(waveTheta1)));
            y = sea_fmadds_nan(aHeightTable[2], cM_scos(waveTheta2), y);
            y = sea_fmadds_nan(aHeightTable[3], cM_scos(waveTheta3), y);
            y = gabi::fadds_ppc(y, BASE_HEIGHT());
            gabi::store<f32>(pHeight, y);
            gabi::store<f32>(pHeight, gabi::fmuls_ppc(y, gabi::fmuls_ppc(aFadeTable[z], aFadeTable[x])));
            waveTheta0 += waveTheta_DeltaX[0];
            waveTheta1 += waveTheta_DeltaX[1];
            waveTheta2 += waveTheta_DeltaX[2];
            waveTheta3 += waveTheta_DeltaX[3];

            pHeight += 4;
        }

        theta[0] += waveTheta_DeltaZ[0];
        theta[1] += waveTheta_DeltaZ[1];
        theta[2] += waveTheta_DeltaZ[2];
        theta[3] += waveTheta_DeltaZ[3];
    }

    mWaveInfo.AddCounter();
    mCurPos.copy(pos);
}
VERIFY(0x0246C088, &daSea_packet_c::execute);

/* 0246C5C8 */
void daSea_execute(cXyz* pos) {
    WWHD_FUNC(0x0246C5C8, void, pos);
    daSea_packet_c* c = l_cloth();
    if (c->mInitFlag)
        c->execute(pos);
}
VERIFY(0x0246C5C8, daSea_execute);

/* 0246C5E8 (HD, new): writes the 65x65 grid into the current vertex buffer (32-byte vertices:
 * position, normal (0,1,0), texture coordinates = position * 0.0005), flushes it and flips to
 * the other buffer. */
static void daSea_fillVtxBuf(daSea_packet_c* i_this) {
    WWHD_FUNC(0x0246C5E8, void, i_this);
    u32 vb = i_this->mpVtxBuf;
    u32 idx = gabi::load<u32>(vb + 0x4A8);
    u32 buf = gabi::load<u32>(vb + idx * 0x254);
    u32 end = buf + 0x21020;
    if (buf < end) {
        for (u32 q = buf; q < end; q += 0x20) /* dcbz */
            for (u32 i = 0; i < 0x20; i += 4) gabi::store<u32>((q & ~31u) + i, 0);
        idx = gabi::load<u32>(vb + 0x4A8);
    }
    f32 z = i_this->mDrawMinZ;
    u32 pH = i_this->mpHeightTable;
    u32 out = gabi::load<u32>(vb + idx * 0x254) - 0x20;
    /* the constants are read from .rodata after the dcbz (a buffer address there clears them) */
    const f32 one = gabi::load<f32>(0x10039E74), texScale = gabi::load<f32>(0x10039EB4) /* 0.0005 + 1 ulp */,
              step = gabi::load<f32>(0x10039E88), zero = gabi::load<f32>(0x10039E70);
    for (int iz = 0; iz < 65; iz++) {
        f32 x = i_this->mDrawMinX; /* re-read per row (the vertex stores may alias it) */
        f32 v = gabi::fmuls_ppc(z, texScale);
        for (int ix = 0; ix < 65; ix++) {
            f32 h = gabi::load<f32>(pH);
            pH += 4;
            f32 u = gabi::fmuls_ppc(x, texScale);
            out += 0x20;
            gabi::store<f32>(out + 0x00, x);
            gabi::store<f32>(out + 0x04, h);
            x = gabi::fadds_ppc(x, step);
            gabi::store<f32>(out + 0x14, zero);
            gabi::store<f32>(out + 0x18, u);
            gabi::store<f32>(out + 0x10, one);
            gabi::store<f32>(out + 0x1C, v);
            gabi::store<f32>(out + 0x0C, zero);
            gabi::store<f32>(out + 0x08, z);
        }
        z = gabi::fadds_ppc(z, step);
    }
    vb = i_this->mpVtxBuf; /* reloaded: the vertex stores may alias it */
    u32 e = vb + gabi::load<s32>(vb + 0x4A8) * 0x254;
    gabi::call(0x027B5E94, e + 4, 0, gabi::load<u32>(e + 0x150)); /* buffer flush */
    gabi::store<u32>(vb + 0x4A8, gabi::load<u32>(vb + 0x4A8) == 0);
}
VERIFY(0x0246C5E8, daSea_fillVtxBuf);

/* 0246C714 (HD, new): GXColor -> four floats in 0..1 (for the sea shader's colour uniforms) */
static void daSea_colorToVec4(be<f32>* out, GXColor* c) {
    WWHD_FUNC(0x0246C714, void, out, c);
    f32 r = (f32)(u8)c->r / 255.0f;
    f32 g = (f32)(u8)c->g / 255.0f;
    f32 b = (f32)(u8)c->b / 255.0f;
    f32 a = (f32)(u8)c->a / 255.0f;
    /* stored through a stack temporary with integer copies */
    gabi::store<u32>(gabi::ea(&out[0]), f2u(r));
    gabi::store<u32>(gabi::ea(&out[1]), f2u(g));
    gabi::store<u32>(gabi::ea(&out[2]), f2u(b));
    gabi::store<u32>(gabi::ea(&out[3]), f2u(a));
}
VERIFY(0x0246C714, daSea_colorToVec4);

/* 0246C7C8 (HD, new): the sea shader's uniforms. View and projection matrices from j3dSys, an
 * identity/scale texture matrix, a scrolling texture offset (counter 0..300 in mAnimCounter), a
 * fixed second offset, the indirect texture matrix, and the two sea colours: the colours from
 * dKy_get_seacolor scaled by the env light's two sea factors, the first one blended towards the
 * second by mFlatInter^2 and the second dimmed by 1 - mFlatInter^2 / 10. */
struct Color4f_l { be<f32> r, g, b, a; };
static inline void Color4f_mul(Color4f_l* out, Color4f_l* a, f32 s) { gabi::call(0x0274D458, out, a, s); }
static inline void Color4f_sub(Color4f_l* out, Color4f_l* a, Color4f_l* b) { gabi::call(0x0274D380, out, a, b); }
static inline void Color4f_add(Color4f_l* out, Color4f_l* a, Color4f_l* b) { gabi::call(0x0274D314, out, a, b); }
static void daSea_setUniforms(daSea_packet_c* i_this) {
    WWHD_FUNC(0x0246C7C8, void, i_this);
    const u32 J3DSYS = 0x104B45C0;
    gabi::call(0x0255F8A0);
    u32 mat = i_this->mpMaterial;
    u32 texMtx = mat + 0x80, ind = mat + 0xB4, blk = mat + 0x128;

    gabi::Local<be<f32>[12]> view;   /* j3dSys view matrix (lfs/stfs copy) */
    for (int i = 0; i < 12; i++) (*view)[i] = (f32)gabi::load<f32>(J3DSYS + 0x38 + 4 * i);
    gabi::Local<be<u32>[16]> proj;   /* j3dSys projection matrix (word copy) */
    for (int i = 0; i < 16; i++) (*proj)[i] = gabi::load<u32>(J3DSYS + 0x14C + 4 * i);
    gabi::call(0x027FDA54, mat, 0, view.get(), proj.get(), gabi::load<u32>(J3DSYS + 0x148) + 0x240);

    gabi::store<u32>(texMtx + 0x30, 0);
    gabi::call(0x028E9098, texMtx);                         /* PSMTXIdentity */
    gabi::call(0x028E945C, blk + 0x110, 1.5f, 1.5f, 1.0f);  /* PSMTXScale */
    s16 cnt = (s16)(i_this->mAnimCounter + 1);
    if (cnt > 300)
        cnt = 0;
    f32 scroll = (f32)cnt / 300.0f;
    i_this->mAnimCounter = cnt;
    gabi::call(0x028E93CC, blk + 0x140, 0.0f, scroll, 0.0f); /* PSMTXTrans */
    gabi::call(0x028E93CC, blk + 0x170, 0.2f, -0.2f, 0.0f);

    gabi::Local<be<f32>[4]> indA, indB;
    (*indB)[0] = 0.0f;
    (*indA)[1] = 0.0f;
    (*indA)[2] = 0.0f;
    (*indA)[3] = 0.3f;
    (*indA)[0] = 0.3f;
    (*indB)[3] = 0.0f;
    (*indB)[1] = 0.0f;
    (*indB)[2] = 0.0f;
    gabi::call(0x027FBB58, ind, 0, indA.get(), indB.get());

    gabi::Local<GXColor> col0, col1;
    gabi::call(0x025602F0, col0.get(), col1.get()); /* dKy_get_seacolor */
    gabi::Local<Color4f_l> c68, c78, c88, c98, cA8, cB8;
    daSea_colorToVec4(&c78->r, col0);
    f32 k0 = gabi::load<f32>(gabi::ea(dKy_getEnvlight()) + 0x10B8);
    Color4f_mul(cB8, c78, k0);
    daSea_colorToVec4(&c88->r, col1);
    f32 k1 = gabi::load<f32>(gabi::ea(dKy_getEnvlight()) + 0x10BC);
    Color4f_mul(c68, c88, k1);
    f32 fl = i_this->mFlatInter;
    f32 fl2 = gabi::fmuls_ppc(fl, fl);
    Color4f_sub(c78, cB8, c68);
    Color4f_mul(c88, c78, fl2);
    Color4f_add(c98, c88, c68);
    c98->a = 1.0f;
    Color4f_mul(cA8, c68, gabi::fnmsubs(fl2, 0.1f, 1.0f));
    cA8->a = 1.0f;
    for (int i = 0; i < 4; i++) copy_word(blk + 0x40 + 4 * i, gabi::ea(c68.get()) + 4 * i);
    for (int i = 0; i < 4; i++) copy_word(blk + 0x70 + 4 * i, gabi::ea(c98.get()) + 4 * i);
    for (int i = 0; i < 4; i++) copy_word(blk + 0x80 + 4 * i, gabi::ea(cA8.get()) + 4 * i);
    gabi::call(0x027FE0DC, (u32)i_this->mpMaterial, 0);
    gabi::call(0x0255F84C);
}
VERIFY(0x0246C7C8, daSea_setUniforms);

/* 0246CAEC daSea_Draw (HD: the GPU vertex upload and uniforms are done here, at entry time;
 * HD: nothing is drawn in stage "Siren" room 18) */
static BOOL daSea_Draw(sea_class* i_this) {
    WWHD_FUNC(0x0246CAEC, BOOL, i_this);
    bool skip = false;
    if (sea_isStartStage(0x10039EE0 /* "Siren" */)) {
        if (dComIfGp_roomControl_getStayNo() == 0x12)
            skip = true;
    }
    if (!skip) {
        /* dComIfGd_setListSky() */
        u32 list = gabi::load<u32>(dComIfGp_ea() + 0x5DA0);
        gabi::store<u32>(0x104B4634, list); /* j3dSys.mDrawBuffer[0] */
        gabi::call(0x027F0E04, list, l_cloth(), 0); /* J3DDrawBuffer::entryImm */
        gabi::call(0x0246C5E8, l_cloth(), 0);       /* HD: fill the vertex buffer */
        gabi::call(0x0246C7C8, l_cloth());          /* HD: shader uniforms */
        /* dComIfGd_setList() */
        gabi::store<u32>(0x104B4634, gabi::load<u32>(dComIfGp_ea() + 0x5D78));
        gabi::store<u32>(0x104B4638, gabi::load<u32>(dComIfGp_ea() + 0x5D7C));
    }
    return TRUE;
}
VERIFY(0x0246CAEC, daSea_Draw);

/* 0246CC44 */
static BOOL daSea_Execute(sea_class* i_this) {
    WWHD_FUNC(0x0246CC44, BOOL, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<cXyz> pos;
    pos->copy(player->current.pos);
    l_cloth()->execute(pos);
    /* HD: clears actor_status bit 0x4000 */
    i_this->actor_status &= ~0x4000u;
    return TRUE;
}
VERIFY(0x0246CC44, daSea_Execute);

/* 0246CCAC */
static BOOL daSea_IsDelete(sea_class*) {
    WWHD_FUNC(0x0246CCAC, BOOL, (u32)0);
    l_cloth()->mInitFlag = false;
    return TRUE;
}
VERIFY(0x0246CCAC, daSea_IsDelete);

/* 0246CCC4 */
static BOOL daSea_Delete(sea_class*) {
    WWHD_FUNC(0x0246CCC4, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0246CCC4, daSea_Delete);

/* 0246CDC4 (HD, new): the sea's GPU buffers. An 0x8200-byte index buffer (this+0x24C); nine
 * vertex-buffer sets (this+0x250..0x270, alternately 2 entries (0x4C0 bytes) and 2x10 entries
 * (0x2EA8 bytes) of 0x254 bytes each) whose entries get GPU memory (65*65 vertices for the sea
 * grid, 10 or 500 vertices for the others) and are bound to the "wave_draw" program; the
 * program's attribute list (this+0x278) is rebuilt with two fresh fetch-shader objects per
 * program; finally the triangle-strip indices of the 65x65 grid. */
#define SEA_GPU_HEAP 0x101F8B4C
static inline u32 sea_gpuAlloc(u32 size, u32 align) {
    u32 h = gabi::call<u32>(0x02756140, gabi::load<u32>(SEA_GPU_HEAP));
    return gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(h + 0xC) + 0x34), h, size, align);
}
static inline void sea_gpuFree(u32 pa) {
    u32 h = gabi::call<u32>(0x02755FEC, gabi::load<u32>(SEA_GPU_HEAP), gabi::load<u32>(pa));
    u32 fn = gabi::load<u32>(gabi::load<u32>(h + 0xC) + 0x3C);
    gabi::call_ptr(fn, h, gabi::load<u32>(pa));
}
/* a set of 2 entries (0x4C0 bytes) */
static inline u32 sea_newBufSet2(u32 ctor) {
    u32 o = gabi::ea(operator_new(0x4C0));
    if (o != 0) {
        __construct_array_l(o, 2, 0x254, ctor);
        gabi::store<u32>(o + 0x4A8, 0);
        gabi::store<u32>(o + 0x4AC, 0);
        gabi::store<u32>(o + 0x4B8, 0);
        gabi::store<u32>(o + 0x4B0, 0x20);
        gabi::store<u8>(o + 0x4BC, 0);
        for (int j = 0; j < 2; j++) gabi::store<u32>(o + j * 0x254, 0);
    }
    return o;
}
/* a set of 2x10 entries (0x2EA8 bytes) */
static inline u32 sea_newBufSet20(u32 ctor) {
    u32 o = gabi::ea(operator_new(0x2EA8));
    if (o != 0) {
        __construct_array_l(o, 0x14, 0x254, ctor);
        gabi::store<u32>(o + 0x2E90, 0);
        gabi::store<u32>(o + 0x2EA0, 0);
        gabi::store<u32>(o + 0x2E98, 0x20);
        gabi::store<u8>(o + 0x2EA4, 0);
        gabi::store<u32>(o + 0x2E94, 0);
        for (int j = 0; j < 2; j++)
            for (int k = 0; k < 10; k++) gabi::store<u32>(o + j * 0x254 + k * 0x4A8, 0);
    }
    return o;
}
/* one entry: GPU memory on first use, then the vertex layout, then the program binding */
static inline void sea_bindEntry(u32 e, u32 vc, u32 size, u32 desc, u32 sh, u32 bound) {
    u32 data = gabi::load<u32>(e);
    if (data == 0) {
        u32 buf = e + 0x24C;
        u32 p = sea_gpuAlloc(size, 0x40);
        if (p != 0) {
            gabi::store<u32>(buf + 4, p);
            gabi::store<u32>(buf, vc);
        }
        data = gabi::load<u32>(buf + 4);
        gabi::store<u32>(e, data);
    }
    gabi::call(0x027FF478, e + 4, data, vc, desc);
    if (sh != 0 && sh != gabi::load<u32>(bound))
        gabi::call(0x027FF530, sh, e + 0x158, e + 4, desc, 0);
}
static inline void sea_setupSet2(u32 P, u32 off, u32 vc, u32 size) {
    u32 s = gabi::load<u32>(P + off);
    gabi::store<u32>(s + 0x4AC, 0x13);
    gabi::store<u32>(s + 0x4B4, 0x10039F1C);
    s = gabi::load<u32>(P + off);
    u32 sh = gabi::load<u32>(P + 0x230);
    for (int j = 0; j < 2; j++) sea_bindEntry(s + j * 0x254, vc, size, s + 0x4AC, sh, s + 0x4B8);
    gabi::store<u32>(s + 0x4B8, sh);
    gabi::store<u8>(s + 0x4BC, 1);
}
static inline void sea_setupSet20(u32 P, u32 off, u32 vc, u32 size) {
    u32 s = gabi::load<u32>(P + off);
    gabi::store<u32>(s + 0x2E94, 0x13);
    gabi::store<u32>(s + 0x2E9C, 0x10039F1C);
    s = gabi::load<u32>(P + off);
    u32 sh = gabi::load<u32>(P + 0x230);
    for (int j = 0; j < 2; j++)
        for (int k = 0; k < 10; k++)
            sea_bindEntry(s + j * 0x254 + k * 0x4A8, vc, size, s + 0x2E94, sh, s + 0x2EA0);
    gabi::store<u32>(s + 0x2EA0, sh);
    gabi::store<u8>(s + 0x2EA4, 1);
}
/* releases the fetch-shader objects of one attribute pair (count at +c, array at +c+4) */
static inline void sea_freeAttr(u32 a, u32 c) {
    u32 arr = gabi::load<u32>(a + c + 4);
    if (arr == 0)
        return;
    for (s32 k = 0; k < gabi::load<s32>(a + c); k++) {
        u32 o = arr + k * 0xF4;
        gabi::call_ptr(gabi::load<u32>(gabi::load<u32>(o + 0xF0) + 0xC), o, 2); /* virtual destructor */
        arr = gabi::load<u32>(a + c + 4);
    }
    sea_gpuFree(a + c + 4);
    gabi::store<u32>(a + c, 0);
    gabi::store<u32>(a + c + 4, 0);
}
static inline void sea_newAttr(u32 a, u32 c) {
    u32 p = sea_gpuAlloc(0xF4, 4);
    if (p != 0)
        gabi::call(0x027BF734, p);
    if (p != 0) {
        gabi::store<u32>(a + c + 4, p);
        gabi::store<u32>(a + c, 1);
    }
}
static void daSea_createBuffers(daSea_packet_c* i_this) {
    WWHD_FUNC(0x0246CDC4, void, i_this);
    u32 P = gabi::ea(i_this);
    gabi::store<u32>(P + 0x24C, gabi::ea(operator_new_arr(0x8200)));
    gabi::store<u32>(P + 0x250, sea_newBufSet2(0x0246F77C));
    gabi::store<u32>(P + 0x254, sea_newBufSet20(0x0246F7D8));
    gabi::store<u32>(P + 0x258, sea_newBufSet2(0x0246F7D8));
    gabi::store<u32>(P + 0x25C, sea_newBufSet20(0x0246F7D8));
    gabi::store<u32>(P + 0x260, sea_newBufSet2(0x0246F7D8));
    gabi::store<u32>(P + 0x264, sea_newBufSet20(0x0246F834));
    gabi::store<u32>(P + 0x268, sea_newBufSet2(0x0246F834));
    gabi::store<u32>(P + 0x26C, sea_newBufSet20(0x0246F834));
    gabi::store<u32>(P + 0x270, sea_newBufSet2(0x0246F834));

    sea_setupSet2(P, 0x250, 0x1081, 0x21020); /* the sea grid: 65 * 65 vertices */
    sea_setupSet20(P, 0x254, 0xA, 0x140);
    sea_setupSet2(P, 0x258, 0xA, 0x140);
    sea_setupSet20(P, 0x25C, 0xA, 0x140);
    sea_setupSet2(P, 0x260, 0xA, 0x140);
    sea_setupSet20(P, 0x264, 0x1F4, 0x3E80);
    sea_setupSet2(P, 0x268, 0x1F4, 0x3E80);
    sea_setupSet20(P, 0x26C, 0x1F4, 0x3E80);
    sea_setupSet2(P, 0x270, 0x1F4, 0x3E80);

    /* the program's attribute list {count, n, entries (0x14 bytes)} */
    u32 lst = gabi::ea(operator_new(0xC));
    if (lst != 0) {
        gabi::store<u32>(lst, 0);
        u32 q = lst + 4;
        if (q == 0)
            q = gabi::ea(operator_new(8));
        if (q != 0) {
            gabi::store<u32>(q + 4, 0);
            gabi::store<u32>(q, 0);
        }
    }
    u32 sh = gabi::load<u32>(P + 0x230);
    gabi::store<u32>(P + 0x278, lst);
    gabi::call(0x0280068C, lst, sh, 0);
    lst = gabi::load<u32>(P + 0x278);
    u32 vb = gabi::load<u32>(P + 0x250);
    for (u32 i = 0; i < gabi::load<u32>(lst); i++) {
        u32 a = gabi::load<u32>(lst + 8);
        if (i < gabi::load<u32>(lst + 4))
            a += i * 0x14;
        u32 prog = gabi::load<u32>(a);
        gabi::store<u32>(a, 0);
        sea_freeAttr(a, 4);
        sea_freeAttr(a, 0xC);
        gabi::store<u32>(a, prog);
        sea_newAttr(a, 4);
        sea_newAttr(a, 0xC);
        for (int j = 0; j < 2; j++)
            gabi::call(0x027FF530, prog, gabi::load<u32>(a + 8 + 8 * j), vb + 4 + j * 0x254, vb + 0x4AC, 0);
    }

    /* triangle-strip indices: (row + 1) * 65 + x, row * 65 + x */
    gabi::store<u32>(P + 0x238, 6);
    u32 lo = 0, hi = 0x41, n = 0;
    for (int row = 0; row < 0x40; row++) {
        for (int x = 0; x < 0x41; x++) {
            gabi::store<u32>(gabi::load<u32>(P + 0x24C) + n * 4, hi);
            n++;
            gabi::store<u32>(gabi::load<u32>(P + 0x24C) + n * 4, lo);
            n++;
            hi++;
            lo++;
        }
    }
    gabi::call(0x027B548C, P + 0x234);
    gabi::call(0x027B54E0, P + 0x234, gabi::load<u32>(P + 0x24C), 9, 0x2080);
    gabi::store<u32>(P + 0x238, 6);
}
VERIFY(0x0246CDC4, daSea_createBuffers);

/* 0246DB68 (HD, new): the sea textures. Under the texture lock: texture 0x6F of the "Stage"-like
 * archive at 0x10039EE8 into the first GX2 texture object (0x27C) with two samplers (0x39C,
 * 0x3A4), texture 0x70 into the second (0x30C) with one sampler (0x3A0). Each sampler gets the
 * same filter/wrap setup. */
static inline void sea_samplerSetA(u32 s, int order) {
    u8 f = gabi::load<u8>(s + 0x190);
    if (order == 2) {
        gabi::store<u32>(s + 0x164, 2);
        gabi::store<u32>(s + 0x160, 0);
        gabi::store<u8>(s + 0x190, f | 2);
        gabi::store<u32>(s + 0x15C, 0);
    } else {
        gabi::store<u32>(s + 0x15C, 0);
        gabi::store<u32>(s + 0x164, 2);
        gabi::store<u8>(s + 0x190, f | 2);
        gabi::store<u32>(s + 0x160, 0);
    }
}
static inline void sea_samplerSetB(u32 s) {
    u8 f = gabi::load<u8>(s + 0x190);
    gabi::store<u32>(s + 0x184, 0);
    gabi::store<u8>(s + 0x190, f | 4);
}
static inline void sea_samplerSetC(u32 s) {
    u8 f = gabi::load<u8>(s + 0x190);
    gabi::store<u32>(s + 0x150, 1);
    gabi::store<u32>(s + 0x154, 1);
    gabi::store<u32>(s + 0x158, 2);
    gabi::store<u8>(s + 0x190, f | 4);
}
static void daSea_createTex(daSea_packet_c* i_this) {
    WWHD_FUNC(0x0246DB68, void, i_this);
    u32 P = gabi::ea(i_this);
    gabi::call(0x0274FBF8, gabi::load<u32>(0x101F8B18)); /* texture lock */
    void* img = dComIfG_getObjectRes(STR(0x10039EE8), 0x6F, SAFESTRING_VTBL);
    gabi::call(0x02773798, P + 0x27C, gabi::load<u32>(gabi::ea(img) + 0x20));
    u32 smp = gabi::call<u32>(0x027BE114, 0, P + 0x27C);
    gabi::store<u32>(P + 0x39C, smp);
    sea_samplerSetA(smp, 0);
    sea_samplerSetB(gabi::load<u32>(P + 0x39C));
    sea_samplerSetC(gabi::load<u32>(P + 0x39C));
    smp = gabi::call<u32>(0x027BE114, 0, P + 0x27C);
    gabi::store<u32>(P + 0x3A4, smp);
    sea_samplerSetA(smp, 0);
    sea_samplerSetB(gabi::load<u32>(P + 0x3A4));
    sea_samplerSetC(gabi::load<u32>(P + 0x3A4));
    img = dComIfG_getObjectRes(STR(0x10039EE8), 0x70, SAFESTRING_VTBL);
    gabi::call(0x02773798, P + 0x30C, gabi::load<u32>(gabi::ea(img) + 0x20));
    smp = gabi::call<u32>(0x027BE114, 0, P + 0x30C);
    gabi::store<u32>(P + 0x3A0, smp);
    sea_samplerSetA(smp, 2);
    sea_samplerSetB(gabi::load<u32>(P + 0x3A0));
    sea_samplerSetC(gabi::load<u32>(P + 0x3A0));
    gabi::call(0x0274FCCC, gabi::load<u32>(0x101F8B18)); /* texture unlock */
}
VERIFY(0x0246DB68, daSea_createTex);

/* 0246CCCC */
void daSea_WaterHeightInfo_Mng::SetInf() {
    WWHD_FUNC(0x0246CCCC, void, this);
    u32 multi = dComIfGp_getStage_getMulti();

    for (s32 i = 0; i < 9; i++)
        for (s32 j = 0; j < 9; j++)
            mHeight[i][j] = 10;

    if (multi != 0) {
        s32 roomNo = 1;
        for (s32 i = 1; i < 9 - 1; i++) {
            for (s32 j = 1; j < 9 - 1; j++) {
                mHeight[i][j] = get_wave_max(roomNo);
                roomNo++;
            }
        }
    }
}
VERIFY(0x0246CCCC, &daSea_WaterHeightInfo_Mng::SetInf);

/* 0246CD74 */
void daSea_packet_c::CleanUp() {
    WWHD_FUNC(0x0246CD74, void, this);
    s32 idx = 0;
    for (s32 z = 0; z < 65; z++)
        for (s32 x = 0; x < 65; x++)
            copy_word(mpHeightTable + 4 * idx++, BASE_HEIGHT_EA);
    mCurPos.z = 0.0f;
    mCurPos.y = 0.0f;
    mCurPos.x = 0.0f;
}
VERIFY(0x0246CD74, &daSea_packet_c::CleanUp);

/* 0246DD14 daSea_packet_c::create (HD: GX texture setup replaced by the "wave_draw" shader
 * program lookup, the GPU vertex buffers (0246CDC4), the GX2 textures (0246DB68) and an inlined
 * 0x41C-byte shader material; GameCube cXyz& taken as a pointer) */
static inline void sea_deadAlloc(u32 sub, u32 size) {
    /* inlined sub-object constructors: "this == NULL" allocates (never true here in practice) */
    if (sub == 0)
        operator_new(size);
}
bool daSea_packet_c::create(cXyz* pos) {
    WWHD_FUNC(0x0246DD14, bool, this, pos);
    gabi::store<f32>(BASE_HEIGHT_EA, 1.0f);
    gabi::store<f32>(BASE_HEIGHT_EA, gabi::fadds_ppc(1.0f, pos->y));
    mFlatInter = 0.0f;
    u32 tbl = gabi::ea(operator_new_arr(65 * 65 * 4));
    mpHeightTable = tbl;
    if (tbl == 0)
        return false;

    mWaterHeightMgr.SetInf();
    gabi::store<u32>(gabi::ea(&mWaveInfo.mWaveInfoTable), WI_PRM_OCEAN); /* SetDat(wi_prm_ocean) */
    CleanUp();
    mAnimCounter = 0;
    mRoomNo = -1;
    mFlags = 0;
    mInitFlag = true;

    /* the shader program "wave_draw" */
    u32 P = gabi::ea(this);
    gabi::Local<SafeString> key;
    key->__vtbl = SAFESTRING_VTBL;
    key->mStringTop = 0x10039EF0u;
    u32 mgr = gabi::call<u32>(0x027FFCBC);
    s32 i = gabi::call<s32>(0x027B90AC, gabi::load<u32>(mgr + 4), key.get());
    u32 e = 0;
    if (i >= 0) {
        u32 n = gabi::load<u32>(mgr + 8);
        u32 tab = gabi::load<u32>(mgr + 0xC);
        u32 ent = (u32)i < n ? tab + i * 0x24 : tab;
        if (gabi::load<u8>(ent + 0x20) == 0) {
            u32 src = gabi::load<u32>(mgr + 4);
            u32 m = gabi::load<u32>(src + 0x1C);
            u32 info = (u32)i < m ? gabi::load<u32>(src + 0x20) + i * 0x84 : 0;
            gabi::call(0x02800B0C, ent, info, 0);
            n = gabi::load<u32>(mgr + 8);
            tab = gabi::load<u32>(mgr + 0xC);
        }
        e = (u32)i < n ? tab + i * 0x24 : tab;
    }
    mpShader = e;
    daSea_createBuffers(this); /* HD: GPU buffers */
    daSea_createTex(this);

    /* the shader material (inlined constructor) */
    u32 mat = gabi::ea(operator_new(0x41C));
    if (mat != 0) {
        gabi::call(0x027FD6F4, mat);
        gabi::call(0x027FB40C, mat + 0xC);
        gabi::store<u32>(mat + 0x18, 0x1016EF84);
        gabi::call(0x028F521C, mat + 0x80, 0x34);
        sea_deadAlloc(mat + 0xC + 0x74, 0x30);
        gabi::call(0x027FB40C, mat + 0xB4);
        gabi::store<u32>(mat + 0xC0, 0x1016EFB4);
        gabi::call(0x028F521C, mat + 0x128, 0x2F0);
        /* three 4x4 identity-like matrices and a vector, as floats 0/1 */
        static const u16 zeros[] = {0x128, 0x12C, 0x130, 0x138, 0x160, 0x140, 0x190, 0x1B0, 0x180,
                                    0x158, 0x17C, 0x1AC, 0x19C, 0x16C, 0x148, 0x15C, 0x14C, 0x170,
                                    0x13C, 0x198, 0x1A0, 0x18C, 0x178, 0x1A8, 0x150, 0x188, 0x168,
                                    0x1B8, 0x1BC, 0x1C0, 0x1C8, 0x1CC, 0x1D0};
        static const u16 ones[] = {0x154, 0x144, 0x1A4, 0x164, 0x1B4, 0x184, 0x194, 0x134, 0x174,
                                   0x1C4, 0x1D4};
        f32 zero = gabi::load<f32>(0x10145180), one = gabi::load<f32>(0x1014517C);
        for (u16 o : zeros) gabi::store<f32>(mat + o, zero);
        for (u16 o : ones) gabi::store<f32>(mat + o, one);
        __construct_array_l(mat + 0x1D8, 2, 0x10, 0x0246F890);
        __construct_array_l(mat + 0x1F8, 2, 0x10, 0x0246F890);
        __construct_array_l(mat + 0x218, 2, 0x10, 0x0246F890);
        static const u16 subs[] = {0x110, 0x140, 0x170, 0x1A0, 0x1D0, 0x200, 0x230, 0x260};
        for (u16 o : subs) sea_deadAlloc(mat + 0x128 + o, 0x30);
        static const u16 subs2[] = {0x290, 0x2A0, 0x2B0, 0x2C0, 0x2D0, 0x2E0};
        for (u16 o : subs2) sea_deadAlloc(mat + 0x128 + o, 0x10);
        gabi::store<u8>(mat + 0x418, 0);
    }
    mpMaterial = mat;
    gabi::call(0x027FE084, mat, 1, 0);
    (void)P;
    return true;
}
VERIFY(0x0246DD14, &daSea_packet_c::create);

/* 0246E1A4 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0246E1A4, bool, i_this);
    return gabi::call<bool>(0x0246DD14, l_cloth(), &i_this->current.pos); /* daSea_packet_c::create */
}
VERIFY(0x0246E1A4, CheckCreateHeap);

/* 0246E1B4 */
static cPhs_State daSea_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0246E1B4, cPhs_State, i_this);
    /* fopAcM_ct(i_this, sea_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = 0x10039E60;
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    if (!fopAcM_entrySolidHeap(i_this, 0x0246E1A4 /* CheckCreateHeap */, 0xA000))
        return cPhs_ERROR_e;
    return cPhs_COMPLEATE_e;
}
VERIFY(0x0246E1B4, daSea_Create);

/* 0246E248 daSea_WaveInfo::~daSea_WaveInfo() (deleting destructor) */
static void daSea_WaveInfo_dt(daSea_WaveInfo* i_this, s32 flags) {
    WWHD_FUNC(0x0246E248, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x0246E248, daSea_WaveInfo_dt);

/* 0246F768 sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x0246F768, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x0246F768, SafeString_dt);

/* 0246F8BC sea_class deleting destructor */
static void sea_class_dt(sea_class* i_this, s32 flags) {
    WWHD_FUNC(0x0246F8BC, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0246F8BC, sea_class_dt);

/* 0246F910 daSea_packet_c deleting destructor */
static void daSea_packet_c_dt(daSea_packet_c* i_this, s32 flags) {
    WWHD_FUNC(0x0246F910, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x027B54A0, gabi::at<void>(gabi::ea(i_this) + 0x234), 2);
        daSea_WaveInfo_dt(&i_this->mWaveInfo, 2);
        gabi::call(0x027F13DC, i_this, 0); /* J3DPacket::~J3DPacket */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0246F910, daSea_packet_c_dt);

/* 0246F890: constructor of an empty 0x10-byte HD helper (allocates when this == NULL) */
static void* daSea_empty_ct(void* i_this) {
    WWHD_FUNC(0x0246F890, void*, i_this);
    if (i_this == nullptr)
        i_this = operator_new(0x10);
    return i_this;
}
VERIFY(0x0246F890, daSea_empty_ct);

/* 0246E25C (HD, new): the sea draw (draw passes 1 and 2). Binds the "wave_draw" program, its
 * three textures and (pass 2) the material's texture-matrix uniforms, sets a polygon offset, and
 * draws the 65x65 grid as 64 indexed strips. In pass 2 it then fills and draws the "skirt"
 * around the grid out to +-450000 in steps of the global 1046DC7C (225000): rows of 5 quads
 * below and above the grid in z (sets 0x254/0x25C, closing rows 0x258/0x260), and the strips
 * left and right of it in x (sets 0x264/0x26C, closing rows 0x268/0x270), with a partial last
 * column. Vertices are (x, BASE_HEIGHT, z), normal (0,1,0) kept from the buffer, uv = (x, z) *
 * 0.0005 (some columns use the constant 225.00003). HD quirk: the right-hand strips reuse the
 * previous row's v (f26), which is the caller's f26 when no other skirt part was drawn. */
#define SEA_STEP_EA 0x1046DC7C
static inline f32 sea_step() { return (f32)gabi::load<s32>(SEA_STEP_EA); }
static inline f32 sea_base() { return gabi::load<f32>(BASE_HEIGHT_EA); }
/* one skirt vertex: position, uv (the normal words +0xC..+0x14 are left as they are) */
static inline void sea_vtx(u32 v, f32 x, f32 y, f32 z, f32 u, f32 w) {
    gabi::store<f32>(v + 0x00, x);
    gabi::store<f32>(v + 0x04, y);
    gabi::store<f32>(v + 0x08, z);
    gabi::store<f32>(v + 0x18, u);
    gabi::store<f32>(v + 0x1C, w);
}
static inline u32 sea_curData(u32 set) {
    return gabi::load<u32>(set + gabi::load<u32>(set + 0x4A8) * 0x254);
}
/* flush the n entries of a 2x10 set's current half, flip it */
static inline void sea_flush20(u32 set, s32 n) {
    u32 cur = gabi::load<u32>(set + 0x2E90);
    if (n != 0) {
        u32 e = set + cur * 0x254 + 4;
        u32 k = (u32)n;
        do {
            gabi::call(0x027B5E94, e, 0, gabi::load<u32>(e + 0x14C));
            e += 0x4A8;
        } while (--k != 0);
        cur = gabi::load<u32>(set + 0x2E90);
    }
    gabi::store<u32>(set + 0x2E90, cur == 0);
}
/* draw the n entries of a 2x10 set (the half just flushed) with count vertices each */
static inline void sea_draw20(u32 set, s32 n, u32 count, bool checkCount) {
    u32 e = set + (gabi::load<u32>(set + 0x2E90) == 0) * 0x254;
    if (n == 0)
        return;
    u32 k = (u32)n;
    do {
        if (gabi::load<u32>(set + 0x2EA0) != 0)
            gabi::call(0x027BFE5C, e + 0x158);
        if (!checkCount || count != 0)
            gabi::call(0xC0006178 /* GX2DrawIndexedEx */, gabi::load<u32>(0x104B4A88), count, gabi::load<u32>(0x104B4A84),
                       gabi::load<u32>(0x104B4A8C), 0, 1);
        e += 0x4A8;
    } while (--k != 0);
}
/* flush a 2-entry set's current entry, flip it, bind its fetch shader */
static inline void sea_flush2(u32 P, u32 off) {
    u32 set = gabi::load<u32>(P + off);
    u32 e = set + gabi::load<u32>(set + 0x4A8) * 0x254;
    gabi::call(0x027B5E94, e + 4, 0, gabi::load<u32>(e + 0x150));
    gabi::store<u32>(set + 0x4A8, gabi::load<u32>(set + 0x4A8) == 0);
    set = gabi::load<u32>(P + off);
    u32 half = gabi::load<u32>(set + 0x4A8) == 0;
    if (gabi::load<u32>(set + 0x4B8) != 0)
        gabi::call(0x027BFE5C, set + half * 0x254 + 0x158);
}
static inline void sea_drawIdx(u32 count) {
    gabi::call(0xC0006178 /* GX2DrawIndexedEx */, gabi::load<u32>(0x104B4A88), count, gabi::load<u32>(0x104B4A84),
               gabi::load<u32>(0x104B4A8C), 0, 1);
}
struct sea_PolyOfs_l { u8 _00[0x14]; };
struct sea_GfxState_l { u8 _00[0x11C]; };
static void daSea_draw(daSea_packet_c* i_this, void* i_pass) {
    WWHD_FUNC(0x0246E25C, void, i_this, i_pass);
    const f32 K = 0x1.0624ep-11f;          /* 0.0005 (+1 ulp) */
    const f32 LO = -450000.0f, HI = 450000.0f;
    const f32 VC = 225.00003f;             /* 10039F0C */
    f32 vPrev = (f32)gabi::cpu->f[26].ps0; /* f26 on entry (see above) */
    u32 P = gabi::ea(i_this);
    u32 pass = gabi::ea(i_pass);
    const u32 J3D = 0x104B45C0;

    /* the program for this pass */
    u32 lst = gabi::load<u32>(P + 0x278);
    u32 pi = gabi::load<u32>(pass + 0xC);
    u32 ent = gabi::load<u32>(lst + 8);
    if (pi < gabi::load<u32>(lst + 4))
        ent += pi * 0x14;
    u32 prog = gabi::load<u32>(ent);
    u32 st = gabi::call<u32>(0x027F29D4, J3D);
    u32 sh = gabi::load<u32>(prog);
    if (sh != gabi::load<u32>(st + 4)) {
        u8 f = gabi::load<u8>(sh);
        u32 old = gabi::load<u32>(st);
        if (f & 2) {
            gabi::store<u8>(sh, f & ~2);
            gabi::call(0x027BB9E0, sh, 0);
        }
        u32 fs = gabi::load<u32>(gabi::load<u32>(sh + 0x7C) + 0x28);
        if (old != fs)
            gabi::call(0x027B9F68, fs);
        if (gabi::load<u32>(sh + 0xC) != 0) {
            gabi::call(0xC00060E0 /* GX2CallDisplayList */, gabi::load<u32>(sh + 4), gabi::load<u32>(sh + 0xC));
            gabi::store<u32>(st, fs);
            gabi::store<u32>(st + 4, sh);
        } else {
            gabi::call(0x027BB7CC, sh);
            gabi::store<u32>(st + 4, sh);
            gabi::store<u32>(st, fs);
        }
    }
    gabi::call(0x027FE118, (u32)i_this->mpMaterial, prog, 0);

    gabi::Local<sea_GfxState_l> gs;
    u32 G = gabi::ea(gs.get());
    gabi::call(0x02750250, gs.get());
    u32 w = gabi::load<u32>(G + 0xEC);
    gabi::store<u8>(G + 0x114, 0);
    gabi::store<u8>(G + 0x01, 1);
    gabi::store<u32>(G + 0x0C, 0);
    gabi::store<u8>(G + 0x00, 1);
    gabi::store<u8>(G + 0x115, 1);
    gabi::store<u8>(G + 0x116, 0);
    gabi::store<u32>(G + 0xEC, (((w & ~0xFu) + 7) & 0xFFFFFF0Fu) + 0x10);
    gabi::store<u32>(G + 0x08, 0);
    gabi::store<u8>(G + 0xE0, 0);
    gabi::call(0x02750370, gs.get());

    /* the three textures */
    u32 t = 0;
    if (gabi::load<u32>(prog + 0x14) != 0)
        t = gabi::load<u32>(prog + 0x18);
    gabi::call(0x027BE53C, gabi::load<u32>(P + 0x39C), t + 4, 0, 0);
    t = 0;
    if (gabi::load<u32>(prog + 0x14) > 1)
        t = gabi::load<u32>(prog + 0x18) + 0x14;
    gabi::call(0x027BE53C, gabi::load<u32>(P + 0x3A0), t + 4, 1, 0);
    t = 0;
    if (gabi::load<u32>(prog + 0x14) > 2)
        t = gabi::load<u32>(prog + 0x18) + 0x28;
    gabi::call(0x027BE53C, gabi::load<u32>(P + 0x3A4), t + 4, 2, 0);

    /* pass 2: the texture-matrix uniforms of the material */
    if (gabi::load<s32>(pass + 0xC) == 2) {
        u32 m = gabi::load<u32>(pass + 0x30);
        if (m != 0) {
            u32 blk = m + 0x10 + gabi::load<u32>(m + 0x4C) * 0x1C;
            u32 u = 0;
            if (gabi::load<u32>(prog + 0xC) > 4)
                u = gabi::load<u32>(prog + 0x10) + 0x50;
            s16 l0 = gabi::load<s16>(u + 0xC);
            u32 a27 = gabi::load<u32>(blk + 4);
            u32 a24 = gabi::load<u32>(blk + 0xC);
            s16 l1 = gabi::load<s16>(u + 0xE);
            s16 l2;
            bool any = true;
            if (l0 == -1 && l1 == -1) {
                l2 = gabi::load<s16>(u + 0x10);
                if (l2 == -1)
                    any = false;
            } else {
                l2 = gabi::load<s16>(u + 0x10);
            }
            if (any) {
                if (l1 != -1)
                    gabi::call(0xC0006900 /* GX2SetPixelUniformBlock */, (s32)l1, a24, a27);
                if (l0 != -1)
                    gabi::call(0xC0006A38 /* GX2SetVertexUniformBlock */, (s32)l0, a24, a27);
                if (l2 != -1)
                    gabi::call(0xC00068A8 /* GX2SetGeometryUniformBlock */, (s32)l2, a24, a27);
            }
        }
        gabi::call(0x027FFE54, pass, prog);
    }

    gabi::Local<sea_PolyOfs_l> po;
    gabi::call(0x027E1A0C, po.get());
    gabi::call(0x027E1A30, po.get(), 3.0f);
    gabi::call(0x027E1A44, po.get(), -20.0f);

    /* the grid: the fetch shader of the buffer just filled, 64 strips of 130 indices */
    {
        u32 l2 = gabi::load<u32>(P + 0x278);
        u32 pi2 = gabi::load<u32>(pass + 0xC);
        u32 n2 = gabi::load<u32>(l2 + 4);
        u32 e2 = gabi::load<u32>(l2 + 8);
        u32 vb = gabi::load<u32>(P + 0x250);
        u32 cur = gabi::load<u32>(vb + 0x4A8);
        if (pi2 < n2)
            e2 += pi2 * 0x14;
        gabi::call(0x027BFE5C, gabi::load<u32>(e2 + (cur == 0 ? 8 : 0) + 8));
        for (u32 i = 0; i < 0x40; i++)
            gabi::call(0xC0006178 /* GX2DrawIndexedEx */, gabi::load<u32>(P + 0x238), 0x82, gabi::load<u32>(P + 0x234),
                       gabi::load<u32>(P + 0x23C) + gabi::load<u32>(P + 0x244) * (i * 0x82), 0, 1);
    }

    if (gabi::load<s32>(pass + 0xC) == 2) {
        gabi::store<u32>(G + 4, 1);
        gabi::call(0x02750370, gs.get());

        f32 vNew = 0, z = 0;
        /* ---- below the grid in z: rows from -450000 up to mDrawMinZ ---- */
        f32 B = i_this->mDrawMinZ;
        if (B > LO) {
            s32 n = gabi::ftoi(gabi::fsubs_ppc(B, LO) / sea_step());
            vNew = -225.00003f;
            z = LO;
            if (n > 0) {
                for (s32 i = 0; i < n; i++) {
                    u32 set = gabi::load<u32>(P + 0x254);
                    u32 data = gabi::load<u32>(set + (2 * i + gabi::load<u32>(set + 0x2E90)) * 0x254);
                    vPrev = vNew;
                    vNew = gabi::fmuls_ppc(gabi::fadds_ppc(z, sea_step()), K);
                    f32 x = LO;
                    u32 vi = 0;
                    for (int c = 0; c < 5; c++) {
                        f32 y0 = sea_base();
                        f32 z1 = gabi::fadds_ppc(z, sea_step());
                        f32 u = gabi::fmuls_ppc(x, K);
                        sea_vtx(data + vi * 0x20, x, y0, z1, u, vNew);
                        vi++;
                        sea_vtx(data + vi * 0x20, x, sea_base(), z, u, vPrev);
                        vi++;
                        x = gabi::fadds_ppc(x, sea_step());
                    }
                    z = gabi::fadds_ppc(z, sea_step());
                }
            }
            u32 set = gabi::load<u32>(P + 0x254);
            sea_flush20(set, n);
            sea_draw20(gabi::load<u32>(P + 0x254), n, 0xA, false);
            /* the closing row up to mDrawMinZ */
            f32 b = i_this->mDrawMinZ;
            if (z < b) {
                u32 data = sea_curData(gabi::load<u32>(P + 0x258));
                vPrev = vNew;
                vNew = gabi::fmuls_ppc(b, K);
                f32 x = LO;
                u32 vi = 0;
                for (int c = 0; c < 5; c++) {
                    f32 u = gabi::fmuls_ppc(x, K);
                    sea_vtx(data + vi * 0x20, x, sea_base(), b, u, vNew);
                    vi++;
                    sea_vtx(data + vi * 0x20, x, sea_base(), z, u, vPrev);
                    vi++;
                    x = gabi::fadds_ppc(x, sea_step());
                    if (c < 4)
                        b = i_this->mDrawMinZ;
                }
                sea_flush2(P, 0x258);
                sea_drawIdx(0xA);
            }
        }
        /* ---- above the grid in z: rows from mDrawMaxZ up to 450000 ---- */
        f32 D = i_this->mDrawMaxZ;
        if (D < HI) {
            s32 n = gabi::ftoi(gabi::fsubs_ppc(HI, D) / sea_step());
            z = D;
            vNew = gabi::fmuls_ppc(z, K);
            if (n > 0) {
                for (s32 i = 0; i < n; i++) {
                    u32 set = gabi::load<u32>(P + 0x25C);
                    u32 cur = gabi::load<u32>(set + 0x2E90);
                    vPrev = vNew;
                    vNew = gabi::fmuls_ppc(gabi::fadds_ppc(z, sea_step()), K);
                    u32 data = gabi::load<u32>(set + (2 * i + cur) * 0x254);
                    f32 x = LO;
                    u32 vi = 0;
                    for (int c = 0; c < 5; c++) {
                        f32 y0 = sea_base();
                        f32 z1 = gabi::fadds_ppc(z, sea_step());
                        f32 u = gabi::fmuls_ppc(x, K);
                        sea_vtx(data + vi * 0x20, x, y0, z1, u, vNew);
                        vi++;
                        sea_vtx(data + vi * 0x20, x, sea_base(), z, u, vPrev);
                        vi++;
                        x = gabi::fadds_ppc(x, sea_step());
                    }
                    z = gabi::fadds_ppc(z, sea_step());
                }
            }
            u32 set = gabi::load<u32>(P + 0x25C);
            sea_flush20(set, n);
            sea_draw20(gabi::load<u32>(P + 0x25C), n, 0xA, false);
            if (z < HI) {
                u32 data = sea_curData(gabi::load<u32>(P + 0x260));
                vPrev = vNew;
                f32 x = LO;
                u32 vi = 0;
                for (int c = 0; c < 5; c++) {
                    f32 u = gabi::fmuls_ppc(x, K);
                    sea_vtx(data + vi * 0x20, x, sea_base(), HI, u, VC);
                    vi++;
                    sea_vtx(data + vi * 0x20, x, sea_base(), z, u, vPrev);
                    vi++;
                    x = gabi::fadds_ppc(x, sea_step());
                }
                sea_flush2(P, 0x260);
                gabi::call(0xC0006170 /* GX2DrawEx */, gabi::load<u32>(0x104B4A88), 0xA, 0, 1);
            }
        }
        /* ---- left and right of the grid, between mDrawMinZ and mDrawMaxZ ---- */
        D = i_this->mDrawMaxZ;
        B = i_this->mDrawMinZ;
        if (D > B) {
            f32 g = sea_step();
            f32 A = i_this->mDrawMinX;
            s32 n = gabi::ftoi(gabi::fsubs_ppc(D, B) / g);
            if (A > LO) {
                f32 rem = gabi::fsubs_ppc(A, LO);
                s32 m = gabi::ftoi(rem / g);
                bool part = gabi::fmuls_ppc((f32)m, g) < rem;
                z = B;
                vNew = gabi::fmuls_ppc(z, K);
                for (s32 i = 0; i < n; i++) {
                    u32 set = gabi::load<u32>(P + 0x264);
                    u32 cur = gabi::load<u32>(set + 0x2E90);
                    vPrev = vNew;
                    u32 data = gabi::load<u32>(set + (2 * i + cur) * 0x254);
                    vNew = VC;
                    u32 vi = 0;
                    f32 x = LO;
                    if (m >= 0) {
                        s32 c = 0;
                        do {
                            f32 y0 = sea_base();
                            f32 z1 = gabi::fadds_ppc(z, sea_step());
                            f32 u = gabi::fmuls_ppc(x, K);
                            sea_vtx(data + vi * 0x20, x, y0, z1, u, VC);
                            vi++;
                            sea_vtx(data + vi * 0x20, x, sea_base(), z, u, vPrev);
                            vi++;
                            x = gabi::fadds_ppc(x, sea_step());
                            c++;
                        } while (c <= m);
                    }
                    if (part) {
                        f32 gg = sea_step();
                        f32 a = i_this->mDrawMinX;
                        f32 y0 = sea_base();
                        f32 u = gabi::fmuls_ppc(a, K);
                        sea_vtx(data + vi * 0x20, a, y0, gabi::fadds_ppc(z, gg), u, VC);
                        vi++;
                        f32 a2 = i_this->mDrawMinX;
                        sea_vtx(data + vi * 0x20, a2, sea_base(), z, u, vPrev);
                    }
                    z = gabi::fadds_ppc(z, sea_step());
                }
                u32 set = gabi::load<u32>(P + 0x264);
                sea_flush20(set, n);
                u32 count = (u32)(m + (part ? 1 : 0) + 1) * 2;
                sea_draw20(gabi::load<u32>(P + 0x264), n, count, true);
                f32 d = i_this->mDrawMaxZ;
                if (z < d) {
                    u32 data = sea_curData(gabi::load<u32>(P + 0x268));
                    vPrev = vNew;
                    vNew = gabi::fmuls_ppc(d, K);
                    f32 x = LO;
                    u32 vi = 0;
                    if (m >= 0) {
                        s32 c = 0;
                        for (;;) {
                            f32 u = gabi::fmuls_ppc(x, K);
                            sea_vtx(data + vi * 0x20, x, sea_base(), d, u, vNew);
                            vi++;
                            sea_vtx(data + vi * 0x20, x, sea_base(), z, u, vPrev);
                            vi++;
                            x = gabi::fadds_ppc(x, sea_step());
                            c++;
                            if (c > m)
                                break;
                            d = i_this->mDrawMaxZ;
                        }
                    }
                    if (part) {
                        f32 a = i_this->mDrawMinX;
                        f32 y0 = sea_base();
                        f32 dd = i_this->mDrawMaxZ;
                        f32 u = gabi::fmuls_ppc(a, K);
                        sea_vtx(data + vi * 0x20, a, y0, dd, u, vNew);
                        vi++;
                        f32 a2 = i_this->mDrawMinX;
                        sea_vtx(data + vi * 0x20, a2, sea_base(), z, u, vPrev);
                    }
                    sea_flush2(P, 0x268);
                    if (count != 0)
                        sea_drawIdx(count);
                }
            }
            /* right of the grid: from mDrawMaxX to 450000 */
            f32 Cx = i_this->mDrawMaxX;
            if (Cx < HI) {
                f32 g2 = sea_step();
                f32 rem = gabi::fsubs_ppc(HI, Cx);
                s32 m = gabi::ftoi(rem / g2);
                bool part = gabi::fmuls_ppc((f32)m, g2) < rem;
                z = i_this->mDrawMinZ;
                vNew = gabi::fmuls_ppc(z, K);
                u32 set = gabi::load<u32>(P + 0x26C);
                f32 x0 = Cx;
                for (s32 i = 0; i < n; i++) {
                    u32 data = gabi::load<u32>(set + (2 * i + gabi::load<u32>(set + 0x2E90)) * 0x254);
                    u32 vi = 0;
                    f32 x = x0;
                    if (m >= 0) {
                        s32 c = 0;
                        do {
                            f32 y0 = sea_base();
                            f32 z1 = gabi::fadds_ppc(z, sea_step());
                            f32 u = gabi::fmuls_ppc(x, K);
                            sea_vtx(data + vi * 0x20, x, y0, z1, u, vNew);
                            vi++;
                            sea_vtx(data + vi * 0x20, x, sea_base(), z, u, vPrev);
                            vi++;
                            x = gabi::fadds_ppc(x, sea_step());
                            c++;
                        } while (c <= m);
                    }
                    if (part) {
                        f32 gg = sea_step();
                        f32 y0 = sea_base();
                        sea_vtx(data + vi * 0x20, HI, y0, gabi::fadds_ppc(z, gg), VC, vNew);
                        vi++;
                        sea_vtx(data + vi * 0x20, HI, sea_base(), z, VC, vPrev);
                    }
                    z = gabi::fadds_ppc(z, sea_step());
                    set = gabi::load<u32>(P + 0x26C);
                    if (i + 1 < n)
                        x0 = i_this->mDrawMaxX;
                }
                sea_flush20(set, n);
                u32 count = (u32)(m + (part ? 1 : 0) + 1) * 2;
                sea_draw20(gabi::load<u32>(P + 0x26C), n, count, true);
                f32 d = i_this->mDrawMaxZ;
                if (z < d) {
                    u32 data = sea_curData(gabi::load<u32>(P + 0x270));
                    vPrev = vNew;
                    f32 x = i_this->mDrawMaxX;
                    vNew = gabi::fmuls_ppc(d, K);
                    u32 vi = 0;
                    if (m >= 0) {
                        s32 c = 0;
                        for (;;) {
                            f32 u = gabi::fmuls_ppc(x, K);
                            sea_vtx(data + vi * 0x20, x, sea_base(), d, u, vNew);
                            vi++;
                            sea_vtx(data + vi * 0x20, x, sea_base(), z, u, vPrev);
                            vi++;
                            x = gabi::fadds_ppc(x, sea_step());
                            c++;
                            if (c > m)
                                break;
                            d = i_this->mDrawMaxZ;
                        }
                    }
                    if (part) {
                        f32 y0 = sea_base();
                        f32 dd = i_this->mDrawMaxZ;
                        sea_vtx(data + vi * 0x20, HI, y0, dd, VC, vNew);
                        vi++;
                        sea_vtx(data + vi * 0x20, HI, sea_base(), z, VC, vPrev);
                    }
                    sea_flush2(P, 0x270);
                    if (count != 0)
                        sea_drawIdx(count);
                }
            }
        }
    }
    gabi::call(0x027E1A0C, po.get());
    gabi::call(0x027E1A08, po.get());
    gabi::call(0x02750370, J3D + 0x18C);
}
VERIFY(0x0246E25C, daSea_draw);

/* 0246F6AC (HD, new): draw callback of the packet's material: passes draw passes 1 and 2 (r4->0xC)
 * on to the sea draw 0246E25C */
static void daSea_drawPass(void* a, void* pass) {
    WWHD_FUNC(0x0246F6AC, void, a, pass);
    u32 k = gabi::load<u32>(gabi::ea(pass) + 0xC);
    if (k < 1 || k > 2)
        return;
    daSea_draw((daSea_packet_c*)a, pass);
}
VERIFY(0x0246F6AC, daSea_drawPass);

/* 0246F6C4 __sinit_d_a_sea_cpp (new): this TU's statics (a 16-byte object cleared, the +-pi
 * range, two 1-byte helper objects) registered for destruction, and a 0x36EE8 constant */
static void sinit_d_a_sea() {
    WWHD_FUNC(0x0246F6C4, void);
    gabi::store<u32>(0x1046D8C8, 0);
    gabi::store<u32>(0x1046D8C0, 0);
    gabi::store<u32>(0x1046D8CC, 0);
    gabi::store<u32>(0x1046D8C4, 0);
    __register_global_object(0x101D017C);
    gabi::store<f32>(0x1046D8B4, -3.1415927f);
    gabi::store<f32>(0x1046D8B8, 3.1415927f);
    gabi::call(0x028ED6F8, 0x1046D8BC);
    __register_global_object(0x101D0188);
    gabi::call(0x028EAB2C, 0x1046D8BD);
    __register_global_object(0x101D0194);
    gabi::store<u32>(0x1046DC7C, 0x36EE8);
}
VERIFY(0x0246F6C4, sinit_d_a_sea);

/* 0246F77C / 0246F7D8 / 0246F834 (new): three copies of the constructor of one 0x254-byte
 * vertex-buffer entry (allocates when this == NULL): GX2 buffer (+4) and fetch-shader (+0x158)
 * sub-objects, +0x24C/+0x250 cleared */
static inline u32 sea_vtxEntry_ct(u32 p) {
    if (p == 0) {
        p = gabi::ea(operator_new(0x254));
        if (p == 0)
            return p;
    }
    gabi::call(0x027B5BD8, p + 4);
    gabi::call(0x027BF734, p + 0x158);
    gabi::store<u32>(p + 0x250, 0);
    gabi::store<u32>(p + 0x24C, 0);
    return p;
}
static u32 sea_vtxEntry_ct0(u32 p) { WWHD_FUNC(0x0246F77C, u32, p); return sea_vtxEntry_ct(p); }
VERIFY(0x0246F77C, sea_vtxEntry_ct0);
static u32 sea_vtxEntry_ct1(u32 p) { WWHD_FUNC(0x0246F7D8, u32, p); return sea_vtxEntry_ct(p); }
VERIFY(0x0246F7D8, sea_vtxEntry_ct1);
static u32 sea_vtxEntry_ct2(u32 p) { WWHD_FUNC(0x0246F834, u32, p); return sea_vtxEntry_ct(p); }
VERIFY(0x0246F834, sea_vtxEntry_ct2);

/* 0246F97C daSea_packet_c virtual (empty), 0246F980 SafeString::assureTerminationImpl_ (empty) */
static void daSea_packet_c_v14(daSea_packet_c*) {
    WWHD_FUNC(0x0246F97C, void, (u32)0);
}
VERIFY(0x0246F97C, daSea_packet_c_v14);
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x0246F980, void, (u32)0);
}
VERIFY(0x0246F980, SafeString_assureTerminationImpl);

/* 0246F984 / 0246F98C: return TRUE */
static BOOL daSea_true_0(void*) {
    WWHD_FUNC(0x0246F984, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0246F984, daSea_true_0);
static BOOL daSea_true_1(void*) {
    WWHD_FUNC(0x0246F98C, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0246F98C, daSea_true_1);
