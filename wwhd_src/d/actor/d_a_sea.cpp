/**
 * d_a_sea.cpp (WWHD)
 * The sea surface: a 65x65 height grid around the player, animated by four travelling waves.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_sea.cpp) to the WWHD layout and verified against cking.rpx.
 * "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_sea.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"

const gc_tu_data gc_tu = {.safeString = 0x10039E38};

static daSea_WaveInfo* daSea_WaveInfo_ct(daSea_WaveInfo*);
static daSea_packet_c* daSea_packet_c_ct(daSea_packet_c*);
static u32 sea_vtxEntry_ct0(u32);
static u32 sea_vtxEntry_ct1(u32);
static u32 sea_vtxEntry_ct2(u32);
static void* daSea_empty_ct(void*);
struct SeaPolygonOffset;
struct SeaGfxState;
void seaPacketConstruct(daSea_packet_c*);
void seaIndexBufferConstruct(void*);
void seaTextureConstruct(void*);
void seaBufferFlush(u32, u32, u32);
void seaPrepareEnvironment();
void seaMaterialSetMatrices(u32, u32, be<f32>*, be<u32>*, u32);
void seaMtxIdentity(u32);
void seaMtxScale(u32, f32, f32, f32);
void seaMtxTrans(u32, f32, f32, f32);
void seaSetIndirectMatrices(u32, u32, be<f32>*, be<f32>*);
void seaGetColors(GXColor*, GXColor*);
void seaMaterialUpdate(u32, u32);
void seaRestoreEnvironment();
void seaDrawBufferEntry(u32, daSea_packet_c*, u32);
void seaFillVtxBufNative(daSea_packet_c*, u32);
void seaSetUniformsNative(daSea_packet_c*);
u32 seaHeapGetCurrent(u32);
u32 seaHeapFindOwner(u32, u32);
void seaVertexBufferBind(u32, u32, u32, u32);
void seaProgramBindAttributes(u32, u32, u32, u32, u32);
void seaFetchShaderConstruct(u32);
void seaProgramListCreate(u32, u32, u32);
void seaIndexBufferInvalidate(u32);
void seaIndexBufferSet(u32, u32, u32, u32);
void seaTextureLock(u32);
void seaTextureLoadImage(u32, u32);
u32 seaSamplerConstruct(u32, u32);
void seaTextureUnlock(u32);
u32 seaShaderManagerGet();
s32 seaShaderFind(u32, SafeString*);
void seaShaderProgramCreate(u32, u32, u32);
void seaMaterialConstruct(u32);
void seaMaterialBlockConstruct(u32);
void seaMemoryClear(u32, u32);
void seaMaterialInitialize(u32, u32, u32);
bool seaCreateNative(daSea_packet_c*, cXyz*);
void seaActorDestruct(sea_class*, u32);
void seaIndexBufferDestruct(void*, s32);
void seaPacketDestruct(daSea_packet_c*, u32);
void seaFetchShaderBind(u32);
void seaGX2DrawIndexedEx(u32, u32, u32, u32, u32, u32);
u32 seaShaderStateGet(u32);
void seaShaderValidate(u32, u32);
void seaFetchShaderSet(u32);
void seaGX2CallDisplayList(u32, u32);
void seaShaderBind(u32);
void seaMaterialBind(u32, u32, u32);
void seaGfxStateConstruct(SeaGfxState*);
void seaGfxStateApply(void*);
void seaSamplerBind(u32, u32, u32, u32);
void seaGX2SetPixelUniformBlock(s32, u32, u32);
void seaGX2SetVertexUniformBlock(s32, u32, u32);
void seaGX2SetGeometryUniformBlock(s32, u32, u32);
void seaDrawPassBind(u32, u32);
void seaPolygonOffsetReset(SeaPolygonOffset*);
void seaPolygonOffsetSetFactor(SeaPolygonOffset*, f32);
void seaPolygonOffsetSetUnits(SeaPolygonOffset*, f32);
void seaGX2DrawEx(u32, u32, u32, u32);
void seaPolygonOffsetApply(SeaPolygonOffset*);
void seaAngleHelperConstruct(u32);
void seaRangeHelperConstruct(u32);
void seaVertexBufferConstruct(u32);

static f32 l_gridOne = 1.0f;
static f32 l_gridZero = 0.0f;
static f32 l_gridStep = 800.0f;
static f32 l_gridTexScale = 0.00050000008195638657f;
static f32 l_materialZero = 0.0f;
static f32 l_materialOne = 1.0f;
static f32 l_baseHeight = 1.0f;
static daSea_WaveInfoDat wi_prm_ocean[4] = {
    {2.5f, 0.00007352941f, 0, {}, 0.98f, 0.2f, 200},
    {2.5f, 0.00008928571f, 4000, {}, 0.2f, 0.98f, 190},
    {2.5f, 0.000113636364f, 8000, {}, -0.98f, 0.2f, 210},
    {2.5f, 0.00015625f, 12000, {}, 0.2f, -0.98f, 180},
};
static s8 pos_around[8][2] = {
    {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0},
};

#define SAFESTRING_VTBL 0x10039E38 /* this TU's sead::SafeString vtable */
#define L_CLOTH_PTR 0x1046D8B0     /* HD: daSea_packet_c* to the function-local l_cloth */
#define L_CLOTH_GUARD 0x1046DC78
#define L_CLOTH_OBJ 0x1046D8D0

static inline daSea_packet_c* l_cloth() {
    return gabi::at<daSea_packet_c>(gabi::load<u32>(L_CLOTH_PTR));
}
static inline f32 BASE_HEIGHT() {
    return (f32)l_baseHeight;
}

// HD: native bindings preserve the original argument widths and extra registers.
void seaConstructArray(u32, u32, u32, u32);
/* dStage_stageDt_c virtuals through the play object (play+0x5150): getMulti (+0x20C), getStagInfo
 * (+0x15C) */
static inline u32 seaStageVirtual(u32 slot) {
    u32 st = dComIfGp_ea() + PLAY_STAGEDATA;
    return gabi::call_ptr<u32>(gabi::load<u32>((u32)gabi::at<SeaStageData>(st)->mpVtable + slot),
                               st);
}
static inline u32 seaStageGetMulti() {
    return seaStageVirtual(0x20C);
}
enum { dStageType_SEA_e = 7 };
static inline s32 dStage_stagInfo_GetSTType(void* i_info) {
    return ((u32)gabi::at<SeaStageInfo>(gabi::ea(i_info))->mFlags >> 16) & 7;
}
// HD: dStage_roomControl_c::getStatusRoomDt(int)
/* fopAcM_SearchByName (inline): the name is passed by address to fpcSch_JudgeForPName */
void* fpcSch_JudgeForPName(void*, void*);
static inline fopAc_ac_c* fopAcM_SearchByName(s16 name) {
    gabi::Local<be<s16>> key;
    *key = name;
    return fopAcIt_Judge(gabi::guest_fn((const void*)&fpcSch_JudgeForPName), key.get());
}
s16 cM_rad2s(f32);
void dKy_usonami_set(f32);
void cM3d_CalcPla(cXyz*, cXyz*, cXyz*, cXyz*, be<f32>*);
void* operator_new_arr(u32);
/* cM2dGBox (0x14: min, max, vtable): constructor / deleting destructor / Set / GetLen */

void cM2dGBox_ct(cM2dGBox*);
void cM2dGBox_dt(cM2dGBox*, s32);
void cM2dGBox_Set(cM2dGBox*, be<f32>*, be<f32>*);
f32 cM2dGBox_GetLen(cM2dGBox*, be<f32>*);
/* sead::SafeString equality (HD inline; as d_a_kb): both sides' virtual assureTermination (+0x14),
 * then a byte compare bounded by 0x40001 */
static inline bool sea_SafeString_eq(SafeString* a, SafeString* b) {
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a);
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a);
    u32 s1 = a->mStringTop;
    gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b);
    if (s1 == b->mStringTop)
        return true;
    u32 p = a->mStringTop, q = b->mStringTop;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c = gabi::load<u8>(p + i);
        if (c != gabi::load<u8>(q + i))
            return false;
        if (c == 0)
            return true;
    }
    return false;
}
/* strcmp(dComIfGp_getStartStageName(), lit) == 0 (HD: SafeString comparison, start stage name at
 * play+0x5134) */
static inline bool sea_isStartStage(const char* lit) {
    gabi::Local<SafeString> a;
    a->mStringTop = gabi::guest_str(lit);
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
    if (b != b)
        return gabi::ppc_qnan(b);
    if (a != a)
        return gabi::ppc_qnan(a);
    if (c != c)
        return gabi::ppc_qnan(c);
    return gabi::fmadds(a, c, b);
}
/* a struct copy GHS does with integer loads/stores (bit-exact) */
static inline u32 f2u(f32 f) {
    u32 u;
    memcpy(&u, &f, 4);
    return u;
}
static inline void copy_word(u32 dst, u32 src) {
    gabi::store<u32>(dst, gabi::load<u32>(src));
}

/* ======================================================================================== */

/* 0246B264 */
int get_wave_max(int roomNo) {
    u32 multi = seaStageGetMulti();
    if (multi == 0)
        return 10;

    s32 num = (s32)gabi::at<SeaStageMulti>(multi)->mCount;
    u32 entry = (u32)gabi::at<SeaStageMulti>(multi)->mpEntries;
    for (int i = 0; i < num; i++, entry += 0xC)
        if ((u32)roomNo ==
            (u8)gabi::at<SeaStageMultiEntry>(entry)->mRoomNo) /* dStage_Mult_info::mRoomNo */
            return (u8)gabi::at<SeaStageMultiEntry>(entry)->mWaveMax; /* mWaveMax */

    return 10;
}

/* 0246B2EC */
void calcMinMax(int v, be<f32>* min, be<f32>* max) {
    f32 mn = gabi::fmsubs((f32)v, 100000.0f, 450000.0f);
    f32 mx = mn + 100000.0f;
    *min = mn;
    *max = mx;
}

// HD: daSea_Init() (HD: first use constructs the function-local l_cloth)
/* 0246B42C */
void daSea_Init() {
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

/* 0246B4B4 */
int daSea_WaterHeightInfo_Mng::Pos2Index(f32 v, be<f32>* dst) {
    f32 f = v + 450000.0f;
    int idx = gabi::ftoi(f / 100000.0f);
    if (dst != nullptr)
        *dst = gabi::fnmsubs((f32)idx, 100000.0f, f);
    return idx;
}

/* 0246B51C */
int daSea_WaterHeightInfo_Mng::GetHeight(int x, int z) {
    if ((u32)x >= 9 || (u32)z >= 9)
        return 10;

    if (dStage_stagInfo_GetSTType(dComIfGp_getStageStagInfo()) == dStageType_SEA_e) {
        return mHeight[z][x];
    } else {
        return get_wave_max(dComIfGp_roomControl_getStayNo());
    }
}

/* 0246B5E8 */
int daSea_WaterHeightInfo_Mng::GetHeight(f32 x, f32 z) {
    int xi = Pos2Index(x, nullptr);
    int zi = Pos2Index(z, nullptr);
    return GetHeight(xi, zi);
}

/* the shared bindings.h declares sea_ChkArea / sea_calcWave as guest-call bindings: this
 * file's definitions live in their own namespace */

/* 0246B630 */
bool sea_ChkAreaBeforePos(f32 x, f32 z) {
    if (l_cloth()->mInitFlag == 0) {
        return false;
    }

    if (l_cloth()->mWaterHeightMgr.GetHeight(x, z) == 0 && l_cloth()->mCullStopFlag != 0) {
        return false;
    }

    return true;
}

/* 0246B6A4 */
bool sea_ChkArea(f32 x, f32 z) {
    if (!sea_ChkAreaBeforePos(x, z)) {
        return false;
    }

    daSea_packet_c* c = l_cloth();
    if (c->mDrawMinX < x && x < c->mDrawMaxX && c->mDrawMinZ < z && z < c->mDrawMaxZ) {
        return true;
    }

    return false;
}

/* 0246B764 */
f32 daSea_WaveInfo::GetScale(f32 v) {
    f32 cur = mCurScale;
    f32 r = gabi::fadds_ppc(cur, gabi::fsubs_ppc(v, cur) / 100.0f);
    mCurScale = r;
    return r;
}

/* 0246B784 */
f32 daSea_WaveInfo::GetKm(int idx) {
    return gabi::fmuls_ppc(6.28f, mWaveInfoTable.get()[idx].mKm);
}

/* 0246B7A4 */
f32 daSea_WaveInfo::GetRatio(int idx) {
    return (f32)mCounters[idx] / (f32)mWaveInfoTable.get()[idx].mCounterMax;
}

/* 0246B808 */
void daSea_WaterHeightInfo_Mng::GetArea(int x, int z, be<f32>* minX, be<f32>* minZ, be<f32>* maxX,
                                        be<f32>* maxZ) {
    calcMinMax(x, minX, maxX);
    calcMinMax(z, minZ, maxZ);
}

/* 0246B848 */
f32 daSea_packet_c::CalcFlatInterTarget(cXyz* i_pos) {
    cXyz& pos = *i_pos;
    gabi::Local<cM2dGBox> box;
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
        int iz = mIdxZ + (s8)pos_around[i][1];
        int ix = mIdxX + (s8)pos_around[i][0];

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

/* 0246BA0C */
f32 sea_calcWave(f32 x, f32 z) {
    if (!sea_ChkArea(x, z)) {
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

/* 0246BC20 */
void daSea_packet_c::ClrFlat() {
    mFlags &= ~0x01;
    mFlatInterCounter = 150.0f;
}

/* 0246BC3C */
void daSea_packet_c::SetFlat() {
    mFlags |= 0x01;
    mFlatTarget = 0.0f;
    mFlatInterCounter = 150.0f;
}

/* 0246BC64 */
void daSea_packet_c::CheckRoomChange() {
    void* room = dComIfGp_roomControl_getStatusRoomDt(dComIfGp_roomControl_getStayNo());
    if (room != 0) {
        mRoomNo = dComIfGp_roomControl_getStayNo();
        fopAc_ac_c* octa = fopAcM_SearchByName(fpcNm_DAIOCTA_e);
        if (octa == nullptr) {
            if (mFlags & 0x01) {
                ClrFlat();
            }
        } else {
            /* daDaiocta_c::getSw() (0x690), fopAcM_GetHomeRoomNo */
            if (!dComIfGs_isSwitch((u8)gabi::at<SeaDaiocta>(gabi::ea(octa))->mSwitch,
                                   octa->home.roomNo)) {
                SetFlat();
            } else {
                ClrFlat();
            }
        }
    }
}

/* 0246BD4C */
void daSea_packet_c::CalcFlatInter() {
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

/* 0246BEA4 */
void daSea_packet_c::SetCullStopFlag() {
    if (sea_isStartStage("A_umikz")) {
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

// HD: daSea_WaveInfo::AddCounter() (not named by the matcher)
/* 0246C03C */
void daSea_WaveInfo::AddCounter() {
    for (s32 i = 0; i < 4; i++) {
        s32 c = mCounters[i] + 1;
        mCounters[i] = c;
        if (c >= mWaveInfoTable.get()[i].mCounterMax)
            mCounters[i] = 0;
    }
}

/* 0246C088 */
void daSea_packet_c::execute(cXyz* i_pos) {
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
        aThetaXTable[i] =
            gabi::fmuls_ppc(km, (f32)gabi::at<daSea_WaveInfoDat>(dat)->mScaleX); /* GetVx */
        km = mWaveInfo.GetKm(i);
        aThetaZTable[i] =
            gabi::fmuls_ppc(km, (f32)gabi::at<daSea_WaveInfoDat>(dat)->mScaleZ); /* GetVz */

        aOffsAnimTable[i] = (s16)gabi::ftoi(65536.0f * (mWaveInfo.GetRatio(i) - 0.5f));
        aHeightTable[i] = gabi::fmuls_ppc(
            scale, (f32)gabi::at<daSea_WaveInfoDat>(dat)->mHeight); /* GetBaseHeight */
    }

    f32 sz = mDrawMinZ + 800.0f;

    s16 theta[4];
    s16 t0 = cM_rad2s(gabi::fmuls_ppc(aThetaZTable[0], sz));
    s16 t1 = cM_rad2s(gabi::fmuls_ppc(aThetaZTable[1], sz));
    s16 t2 = cM_rad2s(gabi::fmuls_ppc(aThetaZTable[2], sz));
    s16 t3 = cM_rad2s(gabi::fmuls_ppc(aThetaZTable[3], sz));
    u32 tab = mWaveInfo.mWaveInfoTable.v;
    theta[0] =
        t0 - aOffsAnimTable[0] + (s16)gabi::at<daSea_WaveInfoDat>(tab)[0].mPhase; /* GetPhai */
    theta[1] = t1 - aOffsAnimTable[1] + (s16)gabi::at<daSea_WaveInfoDat>(tab)[1].mPhase;
    theta[2] = t2 - aOffsAnimTable[2] + (s16)gabi::at<daSea_WaveInfoDat>(tab)[2].mPhase;
    theta[3] = t3 - aOffsAnimTable[3] + (s16)gabi::at<daSea_WaveInfoDat>(tab)[3].mPhase;

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
            /* the height table may overlap itself unaligned, so NaN payloads reach memory bit for
             * bit */
            f32 y = sea_fmadds_nan(aHeightTable[0], cM_scos(waveTheta0),
                                   gabi::fmuls_ppc(aHeightTable[1], cM_scos(waveTheta1)));
            y = sea_fmadds_nan(aHeightTable[2], cM_scos(waveTheta2), y);
            y = sea_fmadds_nan(aHeightTable[3], cM_scos(waveTheta3), y);
            y = gabi::fadds_ppc(y, BASE_HEIGHT());
            gabi::store<f32>(pHeight, y);
            gabi::store<f32>(pHeight,
                             gabi::fmuls_ppc(y, gabi::fmuls_ppc(aFadeTable[z], aFadeTable[x])));
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

/* 0246C5C8 */
void daSea_execute(cXyz* pos) {
    daSea_packet_c* c = l_cloth();
    if (c->mInitFlag)
        c->execute(pos);
}

/*  (HD, new): writes the 65x65 grid into the current vertex buffer (32-byte vertices:
 * position, normal (0,1,0), texture coordinates = position * 0.0005), flushes it and flips to
 * the other buffer. */
/* 0246C5E8 */
void daSea_packet_c::fillVtxBuf() {
    u32 vertexBuffers = mpVtxBuf;
    u32 idx = (u32)gabi::at<SeaBufferSet>(vertexBuffers)->mCurrentBuffer;
    u32 buf = (u32)gabi::at<SeaVertexEntry>(vertexBuffers + idx * sizeof(SeaVertexEntry))->mpData;
    u32 end = buf + 0x21020;
    if (buf < end) {
        for (u32 q = buf; q < end; q += 0x20) /* dcbz */
            for (u32 i = 0; i < 0x20; i += 4)
                gabi::store<u32>((q & ~31u) + i, 0);
        idx = (u32)gabi::at<SeaBufferSet>(vertexBuffers)->mCurrentBuffer;
    }
    f32 z = mDrawMinZ;
    u32 pH = mpHeightTable;
    u32 out =
        (u32)gabi::at<SeaVertexEntry>(vertexBuffers + idx * sizeof(SeaVertexEntry))->mpData - 0x20;
    /* the constants are read from .rodata after the dcbz (a buffer address there clears them) */
    const f32 one = (f32)l_gridOne, texScale = (f32)l_gridTexScale /* 0.0005 + 1 ulp */,
              step = (f32)l_gridStep, zero = (f32)l_gridZero;
    for (int iz = 0; iz < 65; iz++) {
        f32 x = mDrawMinX; /* re-read per row (the vertex stores may alias it) */
        f32 v = gabi::fmuls_ppc(z, texScale);
        for (int ix = 0; ix < 65; ix++) {
            f32 h = gabi::load<f32>(pH);
            pH += 4;
            f32 u = gabi::fmuls_ppc(x, texScale);
            out += 0x20;
            gabi::at<SeaVertex>(out)->mPosX = x;
            gabi::at<SeaVertex>(out)->mPosY = h;
            x = gabi::fadds_ppc(x, step);
            gabi::at<SeaVertex>(out)->mNormalZ = zero;
            gabi::at<SeaVertex>(out)->mTexU = u;
            gabi::at<SeaVertex>(out)->mNormalY = one;
            gabi::at<SeaVertex>(out)->mTexV = v;
            gabi::at<SeaVertex>(out)->mNormalX = zero;
            gabi::at<SeaVertex>(out)->mPosZ = z;
        }
        z = gabi::fadds_ppc(z, step);
    }
    vertexBuffers = mpVtxBuf; /* reloaded: the vertex stores may alias it */
    u32 e = vertexBuffers + (s32)gabi::at<SeaBufferSet>(vertexBuffers)->mCurrentBuffer * 0x254;
    seaBufferFlush(e + 4, 0, (u32)gabi::at<SeaVertexEntry>(e)->mBufferSize); /* buffer flush */
    gabi::at<SeaBufferSet>(vertexBuffers)->mCurrentBuffer =
        (u32)gabi::at<SeaBufferSet>(vertexBuffers)->mCurrentBuffer == 0;
}

// HD: (HD, new): GXColor -> four floats in 0..1 (for the sea shader's colour uniforms)
/* 0246C714 */
static void daSea_colorToVec4(be<f32>* out, GXColor* c) {
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

/*  (HD, new): the sea shader's uniforms. View and projection matrices from j3dSys, an
 * identity/scale texture matrix, a scrolling texture offset (counter 0..300 in mAnimCounter), a
 * fixed second offset, the indirect texture matrix, and the two sea colours: the colours from
 * dKy_get_seacolor scaled by the env light's two sea factors, the first one blended towards the
 * second by mFlatInter^2 and the second dimmed by 1 - mFlatInter^2 / 10. */

void Color4f_mul(SeaColor4f*, SeaColor4f*, f32);
void Color4f_sub(SeaColor4f*, SeaColor4f*, SeaColor4f*);
void Color4f_add(SeaColor4f*, SeaColor4f*, SeaColor4f*);
/* 0246C7C8 */
void daSea_packet_c::setUniforms() {
    const u32 J3DSYS = 0x104B45C0;
    seaPrepareEnvironment();
    u32 mat = mpMaterial;
    u32 texMtx = mat + 0x80, ind = mat + 0xB4, blk = mat + 0x128;

    gabi::Local<be<f32>[12]> view; /* j3dSys view matrix (lfs/stfs copy) */
    for (int i = 0; i < 12; i++)
        (*view)[i] = (f32)gabi::at<SeaJ3DSys>(J3DSYS)->mViewMtx[i];
    gabi::Local<be<u32>[16]> proj; /* j3dSys projection matrix (word copy) */
    for (int i = 0; i < 16; i++)
        (*proj)[i] = (u32)gabi::at<SeaJ3DSys>(J3DSYS)->mProjection[i];
    seaMaterialSetMatrices(mat, 0, *view, *proj,
                           (u32)gabi::at<SeaJ3DSys>(J3DSYS)->mpCamera + 0x240);

    gabi::at<SeaTextureMatrix>(texMtx)->mMode = 0;
    seaMtxIdentity(texMtx);                     /* PSMTXIdentity */
    seaMtxScale(blk + 0x110, 1.5f, 1.5f, 1.0f); /* PSMTXScale */
    s16 cnt = (s16)(mAnimCounter + 1);
    if (cnt > 300)
        cnt = 0;
    f32 scroll = (f32)cnt / 300.0f;
    mAnimCounter = cnt;
    seaMtxTrans(blk + 0x140, 0.0f, scroll, 0.0f); /* PSMTXTrans */
    seaMtxTrans(blk + 0x170, 0.2f, -0.2f, 0.0f);

    gabi::Local<be<f32>[4]> indA, indB;
    (*indB)[0] = 0.0f;
    (*indA)[1] = 0.0f;
    (*indA)[2] = 0.0f;
    (*indA)[3] = 0.3f;
    (*indA)[0] = 0.3f;
    (*indB)[3] = 0.0f;
    (*indB)[1] = 0.0f;
    (*indB)[2] = 0.0f;
    seaSetIndirectMatrices(ind, 0, *indA, *indB);

    gabi::Local<GXColor> col0, col1;
    seaGetColors(col0.get(), col1.get()); /* dKy_get_seacolor */
    gabi::Local<SeaColor4f> c68, c78, c88, c98, cA8, cB8;
    daSea_colorToVec4(&c78->r, col0);
    f32 k0 = (f32)gabi::at<SeaEnvLight>(gabi::ea(dKy_getEnvlight()))->mSeaColorScale;
    Color4f_mul(cB8, c78, k0);
    daSea_colorToVec4(&c88->r, col1);
    f32 k1 = (f32)gabi::at<SeaEnvLight>(gabi::ea(dKy_getEnvlight()))->mSeaUnderwaterColorScale;
    Color4f_mul(c68, c88, k1);
    f32 fl = mFlatInter;
    f32 fl2 = gabi::fmuls_ppc(fl, fl);
    Color4f_sub(c78, cB8, c68);
    Color4f_mul(c88, c78, fl2);
    Color4f_add(c98, c88, c68);
    c98->a = 1.0f;
    Color4f_mul(cA8, c68, gabi::fnmsubs(fl2, 0.1f, 1.0f));
    cA8->a = 1.0f;
    for (int i = 0; i < 4; i++)
        copy_word(blk + 0x40 + 4 * i, gabi::ea(c68.get()) + 4 * i);
    for (int i = 0; i < 4; i++)
        copy_word(blk + 0x70 + 4 * i, gabi::ea(c98.get()) + 4 * i);
    for (int i = 0; i < 4; i++)
        copy_word(blk + 0x80 + 4 * i, gabi::ea(cA8.get()) + 4 * i);
    seaMaterialUpdate((u32)mpMaterial, 0);
    seaRestoreEnvironment();
}

/*  daSea_Draw (HD: the GPU vertex upload and uniforms are done here, at entry time;
 * HD: nothing is drawn in stage "Siren" room 18) */
/* 0246CAEC */
static BOOL daSea_Draw(sea_class* i_this) {
    bool skip = false;
    if (sea_isStartStage("Siren")) {
        if (dComIfGp_roomControl_getStayNo() == 0x12)
            skip = true;
    }
    if (!skip) {
        /* dComIfGd_setListSky() */
        u32 list = (u32)gabi::at<SeaPlayDrawLists>(dComIfGp_ea())->mpSky;
        gabi::at<SeaDrawBuffers>(0x104B45C0)->mBuffers[0] = list; /* j3dSys.mDrawBuffer[0] */
        seaDrawBufferEntry(list, l_cloth(), 0);                   /* J3DDrawBuffer::entryImm */
        seaFillVtxBufNative(l_cloth(), 0);                        /* HD: fill the vertex buffer */
        seaSetUniformsNative(l_cloth());                          /* HD: shader uniforms */
        /* dComIfGd_setList() */
        gabi::at<SeaDrawBuffers>(0x104B45C0)->mBuffers[0] =
            (u32)gabi::at<SeaPlayDrawLists>(dComIfGp_ea())->mpOpaque;
        gabi::at<SeaDrawBuffers>(0x104B45C0)->mBuffers[1] =
            (u32)gabi::at<SeaPlayDrawLists>(dComIfGp_ea())->mpTranslucent;
    }
    return TRUE;
}

/* 0246CC44 */
static BOOL daSea_Execute(sea_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<cXyz> pos;
    pos->copy(player->current.pos);
    l_cloth()->execute(pos);
    /* HD: clears actor_status bit 0x4000 */
    i_this->actor_status &= ~(u32)fopAcStts_UNK4000_e;
    return TRUE;
}

/* 0246CCAC */
static BOOL daSea_IsDelete(u32) {
    l_cloth()->mInitFlag = false;
    return TRUE;
}

/* 0246CCC4 */
static BOOL daSea_Delete(u32) {
    return TRUE;
}

/*  (HD, new): the sea's GPU buffers. An 0x8200-byte index buffer (this+0x24C); nine
 * vertex-buffer sets (this+0x250..0x270, alternately 2 entries (0x4C0 bytes) and 2x10 entries
 * (0x2EA8 bytes) of 0x254 bytes each) whose entries get GPU memory (65*65 vertices for the sea
 * grid, 10 or 500 vertices for the others) and are bound to the "wave_draw" program; the
 * program's attribute list (this+0x278) is rebuilt with two fresh fetch-shader objects per
 * program; finally the triangle-strip indices of the 65x65 grid. */
#define SEA_GPU_HEAP 0x101F8B4C
static inline u32 sea_gpuAlloc(u32 size, u32 align) {
    u32 h = seaHeapGetCurrent(gabi::load<u32>(SEA_GPU_HEAP));
    return gabi::call_ptr<u32>(
        (u32)gabi::at<SeaHeapVtable>((u32)gabi::at<SeaHeap>(h)->mpVtable)->mAlloc, h, size, align);
}
static inline void sea_gpuFree(u32 pa) {
    u32 h = seaHeapFindOwner(gabi::load<u32>(SEA_GPU_HEAP), gabi::load<u32>(pa));
    u32 fn = (u32)gabi::at<SeaHeapVtable>((u32)gabi::at<SeaHeap>(h)->mpVtable)->mFree;
    gabi::call_ptr(fn, h, gabi::load<u32>(pa));
}
/* a buffers of 2 entries (0x4C0 bytes) */
static inline u32 sea_newBufSet2(u32 ctor) {
    u32 o = gabi::ea(operator_new(0x4C0));
    if (o != 0) {
        seaConstructArray(o, 2, 0x254, ctor);
        gabi::at<SeaBufferSet>(o)->mCurrentBuffer = 0;
        gabi::at<SeaBufferSet>(o)->mFormat = 0;
        gabi::at<SeaBufferSet>(o)->mpShader = 0;
        gabi::at<SeaBufferSet>(o)->mStride = 0x20;
        gabi::at<SeaBufferSet>(o)->mReady = 0;
        for (int j = 0; j < 2; j++)
            gabi::at<SeaVertexEntry>(o + j * sizeof(SeaVertexEntry))->mpData = 0;
    }
    return o;
}
/* a buffers of 2x10 entries (0x2EA8 bytes) */
static inline u32 sea_newBufSet20(u32 ctor) {
    u32 o = gabi::ea(operator_new(0x2EA8));
    if (o != 0) {
        seaConstructArray(o, 0x14, 0x254, ctor);
        gabi::at<SeaRowBufferSet>(o)->mCurrentBuffer = 0;
        gabi::at<SeaRowBufferSet>(o)->mpShader = 0;
        gabi::at<SeaRowBufferSet>(o)->mStride = 0x20;
        gabi::at<SeaRowBufferSet>(o)->mReady = 0;
        gabi::at<SeaRowBufferSet>(o)->mFormat = 0;
        for (int j = 0; j < 2; j++)
            for (int k = 0; k < 10; k++)
                gabi::at<SeaVertexEntry>(o + (j + 2 * k) * sizeof(SeaVertexEntry))->mpData = 0;
    }
    return o;
}
/* one entry: GPU memory on first use, then the vertex layout, then the program binding */
static inline void sea_bindEntry(u32 e, u32 vc, u32 size, u32 desc, u32 sh, u32 bound) {
    u32 data = (u32)gabi::at<SeaVertexEntry>(e)->mpData;
    if (data == 0) {
        u32 buf = e + 0x24C;
        u32 p = sea_gpuAlloc(size, 0x40);
        if (p != 0) {
            gabi::at<SeaBufferAllocation>(buf)->mpData = p;
            gabi::at<SeaBufferAllocation>(buf)->mCount = vc;
        }
        data = (u32)gabi::at<SeaBufferAllocation>(buf)->mpData;
        gabi::at<SeaVertexEntry>(e)->mpData = data;
    }
    seaVertexBufferBind(e + 4, data, vc, desc);
    if (sh != 0 && sh != gabi::load<u32>(bound))
        seaProgramBindAttributes(sh, e + 0x158, e + 4, desc, 0);
}
static inline void sea_setupSet2(u32 packetAddress, u32 off, u32 vc, u32 size) {
    u32 s = (u32)*gabi::at<be<u32>>(packetAddress + off);
    gabi::at<SeaBufferSet>(s)->mFormat = 0x13;
    gabi::at<SeaBufferSet>(s)->mpAttributes = 0x10039F1C;
    s = (u32)*gabi::at<be<u32>>(packetAddress + off);
    u32 sh = (u32)gabi::at<daSea_packet_c>(packetAddress)->mpShader;
    for (int j = 0; j < 2; j++)
        sea_bindEntry(s + j * 0x254, vc, size, s + 0x4AC, sh, s + 0x4B8);
    gabi::at<SeaBufferSet>(s)->mpShader = sh;
    gabi::at<SeaBufferSet>(s)->mReady = 1;
}
static inline void sea_setupSet20(u32 packetAddress, u32 off, u32 vc, u32 size) {
    u32 s = (u32)*gabi::at<be<u32>>(packetAddress + off);
    gabi::at<SeaRowBufferSet>(s)->mFormat = 0x13;
    gabi::at<SeaRowBufferSet>(s)->mpAttributes = 0x10039F1C;
    s = (u32)*gabi::at<be<u32>>(packetAddress + off);
    u32 sh = (u32)gabi::at<daSea_packet_c>(packetAddress)->mpShader;
    for (int j = 0; j < 2; j++)
        for (int k = 0; k < 10; k++)
            sea_bindEntry(s + j * 0x254 + k * 0x4A8, vc, size, s + 0x2E94, sh, s + 0x2EA0);
    gabi::at<SeaRowBufferSet>(s)->mpShader = sh;
    gabi::at<SeaRowBufferSet>(s)->mReady = 1;
}
/* releases the fetch-shader objects of one attribute pair (count at +c, array at +c+4) */
static inline void sea_freeAttr(u32 a, u32 c) {
    u32 arr = (u32)gabi::at<SeaAttributeArray>(a + c)->mpEntries;
    if (arr == 0)
        return;
    for (s32 k = 0; k < (s32)gabi::at<SeaAttributeArray>(a + c)->mCount; k++) {
        u32 o = arr + k * 0xF4;
        gabi::call_ptr(
            (u32)gabi::at<SeaFetchShaderVtable>((u32)gabi::at<SeaFetchShader>(o)->mpVtable)
                ->mDestroy,
            o, 2); /* virtual destructor */
        arr = (u32)gabi::at<SeaAttributeArray>(a + c)->mpEntries;
    }
    sea_gpuFree(a + c + 4);
    gabi::at<SeaAttributeArray>(a + c)->mCount = 0;
    gabi::at<SeaAttributeArray>(a + c)->mpEntries = 0;
}
static inline void sea_newAttr(u32 a, u32 c) {
    u32 p = sea_gpuAlloc(0xF4, 4);
    if (p != 0)
        seaFetchShaderConstruct(p);
    if (p != 0) {
        gabi::at<SeaAttributeArray>(a + c)->mpEntries = p;
        gabi::at<SeaAttributeArray>(a + c)->mCount = 1;
    }
}
/* 0246CDC4 */
void daSea_packet_c::createBuffers() {
    u32 packetAddress = gabi::ea(this);
    gabi::at<daSea_packet_c>(packetAddress)->mpIndexData = gabi::ea(operator_new_arr(0x8200));
    gabi::at<daSea_packet_c>(packetAddress)->mpVtxBuf =
        sea_newBufSet2(gabi::guest_fn((const void*)&sea_vtxEntry_ct0));
    gabi::at<daSea_packet_c>(packetAddress)->mpLowerRows =
        sea_newBufSet20(gabi::guest_fn((const void*)&sea_vtxEntry_ct1));
    gabi::at<daSea_packet_c>(packetAddress)->mpLowerClosingRow =
        sea_newBufSet2(gabi::guest_fn((const void*)&sea_vtxEntry_ct1));
    gabi::at<daSea_packet_c>(packetAddress)->mpUpperRows =
        sea_newBufSet20(gabi::guest_fn((const void*)&sea_vtxEntry_ct1));
    gabi::at<daSea_packet_c>(packetAddress)->mpUpperClosingRow =
        sea_newBufSet2(gabi::guest_fn((const void*)&sea_vtxEntry_ct1));
    gabi::at<daSea_packet_c>(packetAddress)->mpLeftRows =
        sea_newBufSet20(gabi::guest_fn((const void*)&sea_vtxEntry_ct2));
    gabi::at<daSea_packet_c>(packetAddress)->mpLeftClosingRow =
        sea_newBufSet2(gabi::guest_fn((const void*)&sea_vtxEntry_ct2));
    gabi::at<daSea_packet_c>(packetAddress)->mpRightRows =
        sea_newBufSet20(gabi::guest_fn((const void*)&sea_vtxEntry_ct2));
    gabi::at<daSea_packet_c>(packetAddress)->mpRightClosingRow =
        sea_newBufSet2(gabi::guest_fn((const void*)&sea_vtxEntry_ct2));

    sea_setupSet2(packetAddress, 0x250, 0x1081, 0x21020); /* the sea grid: 65 * 65 vertices */
    sea_setupSet20(packetAddress, 0x254, 0xA, 0x140);
    sea_setupSet2(packetAddress, 0x258, 0xA, 0x140);
    sea_setupSet20(packetAddress, 0x25C, 0xA, 0x140);
    sea_setupSet2(packetAddress, 0x260, 0xA, 0x140);
    sea_setupSet20(packetAddress, 0x264, 0x1F4, 0x3E80);
    sea_setupSet2(packetAddress, 0x268, 0x1F4, 0x3E80);
    sea_setupSet20(packetAddress, 0x26C, 0x1F4, 0x3E80);
    sea_setupSet2(packetAddress, 0x270, 0x1F4, 0x3E80);

    /* the program's attribute list {count, n, entries (0x14 bytes)} */
    u32 programList = gabi::ea(operator_new(0xC));
    if (programList != 0) {
        gabi::at<SeaProgramList>(programList)->mCount = 0;
        u32 q = programList + 4;
        if (q == 0)
            q = gabi::ea(operator_new(8));
        if (q != 0) {
            gabi::at<SeaBufferAllocation>(q)->mpData = 0;
            gabi::at<SeaBufferAllocation>(q)->mCount = 0;
        }
    }
    u32 sh = (u32)gabi::at<daSea_packet_c>(packetAddress)->mpShader;
    gabi::at<daSea_packet_c>(packetAddress)->mpPrograms = programList;
    seaProgramListCreate(programList, sh, 0);
    programList = (u32)gabi::at<daSea_packet_c>(packetAddress)->mpPrograms;
    u32 vertexBuffers = (u32)gabi::at<daSea_packet_c>(packetAddress)->mpVtxBuf;
    for (u32 i = 0; i < (u32)gabi::at<SeaProgramList>(programList)->mCount; i++) {
        u32 a = (u32)gabi::at<SeaProgramList>(programList)->mpEntries;
        if (i < (u32)gabi::at<SeaProgramList>(programList)->mCapacity)
            a += i * 0x14;
        u32 program = (u32)gabi::at<SeaProgramEntry>(a)->mpProgram;
        gabi::at<SeaProgramEntry>(a)->mpProgram = 0;
        sea_freeAttr(a, 4);
        sea_freeAttr(a, 0xC);
        gabi::at<SeaProgramEntry>(a)->mpProgram = program;
        sea_newAttr(a, 4);
        sea_newAttr(a, 0xC);
        for (int j = 0; j < 2; j++)
            seaProgramBindAttributes(program, gabi::load<u32>(a + 8 + 8 * j),
                                     vertexBuffers + 4 + j * 0x254, vertexBuffers + 0x4AC, 0);
    }

    /* triangle-strip indices: (row + 1) * 65 + x, row * 65 + x */
    gabi::at<daSea_packet_c>(packetAddress)->mIndexPrimitive = 6;
    u32 lo = 0, hi = 0x41, n = 0;
    for (int row = 0; row < 0x40; row++) {
        for (int x = 0; x < 0x41; x++) {
            gabi::store<u32>((u32)gabi::at<daSea_packet_c>(packetAddress)->mpIndexData + n * 4, hi);
            n++;
            gabi::store<u32>((u32)gabi::at<daSea_packet_c>(packetAddress)->mpIndexData + n * 4, lo);
            n++;
            hi++;
            lo++;
        }
    }
    seaIndexBufferInvalidate(packetAddress + 0x234);
    seaIndexBufferSet(packetAddress + 0x234,
                      (u32)gabi::at<daSea_packet_c>(packetAddress)->mpIndexData, 9, 0x2080);
    gabi::at<daSea_packet_c>(packetAddress)->mIndexPrimitive = 6;
}

/*  (HD, new): the sea textures. Under the texture lock: texture 0x6F of the "Stage"-like
 * archive at  into the first GX2 texture object (0x27C) with two samplers (0x39C,
 * 0x3A4), texture 0x70 into the second (0x30C) with one sampler (0x3A0). Each sampler gets the
 * same filter/wrap setup. */
static inline void sea_samplerSetA(u32 s, int order) {
    u8 f = (u8)gabi::at<SeaSampler>(s)->mDirtyFlags;
    if (order == 2) {
        gabi::at<SeaSampler>(s)->mMipFilter = 2;
        gabi::at<SeaSampler>(s)->mMagFilter = 0;
        gabi::at<SeaSampler>(s)->mDirtyFlags = f | 2;
        gabi::at<SeaSampler>(s)->mMinFilter = 0;
    } else {
        gabi::at<SeaSampler>(s)->mMinFilter = 0;
        gabi::at<SeaSampler>(s)->mMipFilter = 2;
        gabi::at<SeaSampler>(s)->mDirtyFlags = f | 2;
        gabi::at<SeaSampler>(s)->mMagFilter = 0;
    }
}
static inline void sea_samplerSetB(u32 s) {
    u8 f = (u8)gabi::at<SeaSampler>(s)->mDirtyFlags;
    gabi::at<SeaSampler>(s)->mAnisotropy = 0;
    gabi::at<SeaSampler>(s)->mDirtyFlags = f | 4;
}
static inline void sea_samplerSetC(u32 s) {
    u8 f = (u8)gabi::at<SeaSampler>(s)->mDirtyFlags;
    gabi::at<SeaSampler>(s)->mWrapS = 1;
    gabi::at<SeaSampler>(s)->mWrapT = 1;
    gabi::at<SeaSampler>(s)->mWrapR = 2;
    gabi::at<SeaSampler>(s)->mDirtyFlags = f | 4;
}
/* 0246DB68 */
void daSea_packet_c::createTex() {
    u32 packetAddress = gabi::ea(this);
    seaTextureLock(gabi::load<u32>(0x101F8B18)); /* texture lock */
    void* img = dComIfG_getObjectRes("Always", SEA_TEXTURE_SEA, SAFESTRING_VTBL);
    seaTextureLoadImage(packetAddress + 0x27C,
                        (u32)gabi::at<SeaTextureResource>(gabi::ea(img))->mpImage);
    u32 smp = seaSamplerConstruct(0, packetAddress + 0x27C);
    gabi::at<daSea_packet_c>(packetAddress)->mpSeaSampler = smp;
    sea_samplerSetA(smp, 0);
    sea_samplerSetB((u32)gabi::at<daSea_packet_c>(packetAddress)->mpSeaSampler);
    sea_samplerSetC((u32)gabi::at<daSea_packet_c>(packetAddress)->mpSeaSampler);
    smp = seaSamplerConstruct(0, packetAddress + 0x27C);
    gabi::at<daSea_packet_c>(packetAddress)->mpSeaSamplerCopy = smp;
    sea_samplerSetA(smp, 0);
    sea_samplerSetB((u32)gabi::at<daSea_packet_c>(packetAddress)->mpSeaSamplerCopy);
    sea_samplerSetC((u32)gabi::at<daSea_packet_c>(packetAddress)->mpSeaSamplerCopy);
    img = dComIfG_getObjectRes("Always", SEA_TEXTURE_YURA, SAFESTRING_VTBL);
    seaTextureLoadImage(packetAddress + 0x30C,
                        (u32)gabi::at<SeaTextureResource>(gabi::ea(img))->mpImage);
    smp = seaSamplerConstruct(0, packetAddress + 0x30C);
    gabi::at<daSea_packet_c>(packetAddress)->mpYuraSampler = smp;
    sea_samplerSetA(smp, 2);
    sea_samplerSetB((u32)gabi::at<daSea_packet_c>(packetAddress)->mpYuraSampler);
    sea_samplerSetC((u32)gabi::at<daSea_packet_c>(packetAddress)->mpYuraSampler);
    seaTextureUnlock(gabi::load<u32>(0x101F8B18)); /* texture unlock */
}

/* 0246CCCC */
void daSea_WaterHeightInfo_Mng::SetInf() {
    u32 multi = seaStageGetMulti();

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

/* 0246CD74 */
void daSea_packet_c::CleanUp() {
    s32 idx = 0;
    for (s32 z = 0; z < 65; z++)
        for (s32 x = 0; x < 65; x++)
            copy_word(mpHeightTable + 4 * idx++, gabi::ea(&l_baseHeight));
    mCurPos.z = 0.0f;
    mCurPos.y = 0.0f;
    mCurPos.x = 0.0f;
}

/*  daSea_packet_c::create (HD: GX texture setup replaced by the "wave_draw" shader
 * program lookup, the GPU vertex buffers (), the GX2 textures () and an inlined
 * 0x41C-byte shader material; GameCube cXyz& taken as a pointer) */
static inline void sea_deadAlloc(u32 sub, u32 size) {
    /* inlined sub-object constructors: "this == NULL" allocates (never true here in practice) */
    if (sub == 0)
        operator_new(size);
}
/* 0246DD14 */
bool daSea_packet_c::create(cXyz* pos) {
    l_baseHeight = 1.0f;
    l_baseHeight = gabi::fadds_ppc(1.0f, pos->y);
    mFlatInter = 0.0f;
    u32 tbl = gabi::ea(operator_new_arr(65 * 65 * 4));
    mpHeightTable = tbl;
    if (tbl == 0)
        return false;

    mWaterHeightMgr.SetInf();
    mWaveInfo.mWaveInfoTable = wi_prm_ocean; /* SetDat(wi_prm_ocean) */
    CleanUp();
    mAnimCounter = 0;
    mRoomNo = -1;
    mFlags = 0;
    mInitFlag = true;

    /* the shader program "wave_draw" */
    u32 packetAddress = gabi::ea(this);
    gabi::Local<SafeString> key;
    key->__vtbl = SAFESTRING_VTBL;
    key->mStringTop = gabi::guest_str("wave_draw");
    u32 mgr = seaShaderManagerGet();
    s32 i = seaShaderFind((u32)gabi::at<SeaShaderManager>(mgr)->mpArchive, key.get());
    u32 e = 0;
    if (i >= 0) {
        u32 n = (u32)gabi::at<SeaShaderManager>(mgr)->mCount;
        u32 tab = (u32)gabi::at<SeaShaderManager>(mgr)->mpPrograms;
        u32 ent = (u32)i < n ? tab + i * 0x24 : tab;
        if ((u8)gabi::at<SeaProgram>(ent)->mReady == 0) {
            u32 src = (u32)gabi::at<SeaShaderManager>(mgr)->mpArchive;
            u32 m = (u32)gabi::at<SeaShaderArchive>(src)->mCount;
            u32 info = (u32)i < m ? (u32)gabi::at<SeaShaderArchive>(src)->mpEntries + i * 0x84 : 0;
            seaShaderProgramCreate(ent, info, 0);
            n = (u32)gabi::at<SeaShaderManager>(mgr)->mCount;
            tab = (u32)gabi::at<SeaShaderManager>(mgr)->mpPrograms;
        }
        e = (u32)i < n ? tab + i * 0x24 : tab;
    }
    mpShader = e;
    createBuffers(); /* HD: GPU buffers */
    createTex();

    /* the shader material (inlined constructor) */
    u32 mat = gabi::ea(operator_new(0x41C));
    if (mat != 0) {
        seaMaterialConstruct(mat);
        seaMaterialBlockConstruct(mat + 0xC);
        gabi::at<SeaMaterial>(mat)->mTextureMatrixVtable = 0x1016EF84;
        seaMemoryClear(mat + 0x80, 0x34);
        sea_deadAlloc(mat + 0xC + 0x74, 0x30);
        seaMaterialBlockConstruct(mat + 0xB4);
        gabi::at<SeaMaterial>(mat)->mIndirectMatrix.mVtable = 0x1016EFB4;
        seaMemoryClear(mat + 0x128, 0x2F0);
        /* three 4x4 identity-like matrices and a vector, as floats 0/1 */
        const u16 zeros[] = {0x128, 0x12C, 0x130, 0x138, 0x160, 0x140, 0x190, 0x1B0, 0x180,
                             0x158, 0x17C, 0x1AC, 0x19C, 0x16C, 0x148, 0x15C, 0x14C, 0x170,
                             0x13C, 0x198, 0x1A0, 0x18C, 0x178, 0x1A8, 0x150, 0x188, 0x168,
                             0x1B8, 0x1BC, 0x1C0, 0x1C8, 0x1CC, 0x1D0};
        const u16 ones[] = {0x154, 0x144, 0x1A4, 0x164, 0x1B4, 0x184,
                            0x194, 0x134, 0x174, 0x1C4, 0x1D4};
        f32 zero = (f32)l_materialZero, one = (f32)l_materialOne;
        for (u16 o : zeros)
            gabi::at<SeaMaterial>(mat)->mUniforms[(o - 0x128) / 4] = zero;
        for (u16 o : ones)
            gabi::at<SeaMaterial>(mat)->mUniforms[(o - 0x128) / 4] = one;
        seaConstructArray(mat + 0x1D8, 2, 0x10, gabi::guest_fn((const void*)&daSea_empty_ct));
        seaConstructArray(mat + 0x1F8, 2, 0x10, gabi::guest_fn((const void*)&daSea_empty_ct));
        seaConstructArray(mat + 0x218, 2, 0x10, gabi::guest_fn((const void*)&daSea_empty_ct));
        const u16 subs[] = {0x110, 0x140, 0x170, 0x1A0, 0x1D0, 0x200, 0x230, 0x260};
        for (u16 o : subs)
            sea_deadAlloc(mat + 0x128 + o, 0x30);
        const u16 subs2[] = {0x290, 0x2A0, 0x2B0, 0x2C0, 0x2D0, 0x2E0};
        for (u16 o : subs2)
            sea_deadAlloc(mat + 0x128 + o, 0x10);
        gabi::at<SeaMaterial>(mat)->mFlags = 0;
    }
    mpMaterial = mat;
    seaMaterialInitialize(mat, 1, 0);
    (void)packetAddress;
    return true;
}

/* 0246E1A4 */
static bool CheckCreateHeap(fopAc_ac_c* i_this) {
    return seaCreateNative(l_cloth(), &i_this->current.pos); /* daSea_packet_c::create */
}

/* 0246E1B4 */
static cPhs_State daSea_Create(fopAc_ac_c* i_this) {
    /* fopAcM_ct(i_this, sea_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = 0x10039E60;
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }
    if (!fopAcM_entrySolidHeap(i_this, gabi::guest_fn((const void*)&CheckCreateHeap), 0xA000))
        return cPhs_ERROR_e;
    return cPhs_COMPLEATE_e;
}

/*  (HD, new): the sea draw (draw passes 1 and 2). Binds the "wave_draw" program, its
 * three textures and (pass 2) the material's texture-matrix uniforms, sets a polygon offset, and
 * draws the 65x65 grid as 64 indexed strips. In pass 2 it then fills and draws the "skirt"
 * around the grid out to +-450000 in steps of the global  (225000): rows of 5 quads
 * below and above the grid in z (sets 0x254/0x25C, closing rows 0x258/0x260), and the strips
 * left and right of it in x (sets 0x264/0x26C, closing rows 0x268/0x270), with a partial last
 * column. Vertices are (x, BASE_HEIGHT, z), normal (0,1,0) kept from the buffer, uv = (x, z) *
 * 0.0005 (some columns use the constant 225.00003). HD quirk: the right-hand strips reuse the
 * previous row's v (f26), which is the caller's f26 when no other skirt part was drawn. */
static s32 l_skirtStep;
static inline f32 sea_step() {
    return (f32)(s32)l_skirtStep;
}
static inline f32 sea_base() {
    return (f32)l_baseHeight;
}
/* one skirt vertex: position, uv (the normal words +0xC..+0x14 are left as they are) */
static inline void sea_vtx(u32 v, f32 x, f32 y, f32 z, f32 u, f32 w) {
    gabi::at<SeaVertex>(v)->mPosX = x;
    gabi::at<SeaVertex>(v)->mPosY = y;
    gabi::at<SeaVertex>(v)->mPosZ = z;
    gabi::at<SeaVertex>(v)->mTexU = u;
    gabi::at<SeaVertex>(v)->mTexV = w;
}
static inline u32 sea_curData(u32 buffers) {
    return (u32)gabi::at<SeaVertexEntry>(buffers +
                                         (u32)gabi::at<SeaBufferSet>(buffers)->mCurrentBuffer *
                                             sizeof(SeaVertexEntry))
        ->mpData;
}
/* flush the n entries of a double-buffered row set's current half, flip it */
static inline void sea_flush20(u32 buffers, s32 n) {
    u32 cur = (u32)gabi::at<SeaRowBufferSet>(buffers)->mCurrentBuffer;
    if (n != 0) {
        u32 e = buffers + cur * 0x254 + 4;
        u32 k = (u32)n;
        do {
            seaBufferFlush(e, 0, (u32)gabi::at<SeaVertexEntry>(e - 4)->mBufferSize);
            e += 0x4A8;
        } while (--k != 0);
        cur = (u32)gabi::at<SeaRowBufferSet>(buffers)->mCurrentBuffer;
    }
    gabi::at<SeaRowBufferSet>(buffers)->mCurrentBuffer = cur == 0;
}
/* draw the n entries of a 2x10 buffers (the half just flushed) with count vertices each */
static inline void sea_draw20(u32 buffers, s32 n, u32 count, bool checkCount) {
    u32 e = buffers + ((u32)gabi::at<SeaRowBufferSet>(buffers)->mCurrentBuffer == 0) * 0x254;
    if (n == 0)
        return;
    u32 k = (u32)n;
    do {
        if ((u32)gabi::at<SeaRowBufferSet>(buffers)->mpShader != 0)
            seaFetchShaderBind(e + 0x158);
        if (!checkCount || count != 0)
            seaGX2DrawIndexedEx((u32)gabi::at<SeaDrawIndices>(0x104B4A84)->mPrimitive, count,
                                (u32)gabi::at<SeaDrawIndices>(0x104B4A84)->mFormat,
                                (u32)gabi::at<SeaDrawIndices>(0x104B4A84)->mpData, 0, 1);
        e += 0x4A8;
    } while (--k != 0);
}
/* flush a two-entry buffer set's current entry, flip it, bind its fetch shader */
static inline void sea_flush2(u32 packetAddress, u32 off) {
    u32 buffers = (u32)*gabi::at<be<u32>>(packetAddress + off);
    u32 e = buffers + (u32)gabi::at<SeaBufferSet>(buffers)->mCurrentBuffer * 0x254;
    seaBufferFlush(e + 4, 0, (u32)gabi::at<SeaVertexEntry>(e)->mBufferSize);
    gabi::at<SeaBufferSet>(buffers)->mCurrentBuffer =
        (u32)gabi::at<SeaBufferSet>(buffers)->mCurrentBuffer == 0;
    buffers = (u32)*gabi::at<be<u32>>(packetAddress + off);
    u32 half = (u32)gabi::at<SeaBufferSet>(buffers)->mCurrentBuffer == 0;
    if ((u32)gabi::at<SeaBufferSet>(buffers)->mpShader != 0)
        seaFetchShaderBind(buffers + half * 0x254 + 0x158);
}
static inline void sea_drawIdx(u32 count) {
    seaGX2DrawIndexedEx((u32)gabi::at<SeaDrawIndices>(0x104B4A84)->mPrimitive, count,
                        (u32)gabi::at<SeaDrawIndices>(0x104B4A84)->mFormat,
                        (u32)gabi::at<SeaDrawIndices>(0x104B4A84)->mpData, 0, 1);
}

/* 0246E25C */
void daSea_packet_c::draw(void* i_pass) {
    const f32 K = 0x1.0624ep-11f; /* 0.0005 (+1 ulp) */
    const f32 LO = -450000.0f, HI = 450000.0f;
    const f32 VC = 225.00003f;             /* 10039F0C */
                                           // HD:
    f32 vPrev = (f32)gabi::cpu->f[26].ps0; /* f26 on entry (see above) */
    u32 packetAddress = gabi::ea(this);
    u32 pass = gabi::ea(i_pass);
    const u32 J3D = 0x104B45C0;

    /* the program for this pass */
    u32 programList = (u32)gabi::at<daSea_packet_c>(packetAddress)->mpPrograms;
    u32 pi = (u32)gabi::at<SeaDrawPass>(pass)->mPass;
    u32 ent = (u32)gabi::at<SeaProgramList>(programList)->mpEntries;
    if (pi < (u32)gabi::at<SeaProgramList>(programList)->mCapacity)
        ent += pi * 0x14;
    u32 program = (u32)gabi::at<SeaProgramEntry>(ent)->mpProgram;
    u32 st = seaShaderStateGet(J3D);
    u32 sh = (u32)gabi::at<SeaProgram>(program)->mpShader;
    if (sh != (u32)gabi::at<SeaShaderState>(st)->mpShader) {
        u8 f = (u8)gabi::at<SeaShader>(sh)->mFlags;
        u32 old = (u32)gabi::at<SeaShaderState>(st)->mpFetchShader;
        if (f & 2) {
            gabi::at<SeaShader>(sh)->mFlags = f & ~2;
            seaShaderValidate(sh, 0);
        }
        u32 fs = (u32)gabi::at<SeaFetchShaderInfo>((u32)gabi::at<SeaShader>(sh)->mpFetchShaderInfo)
                     ->mpShader;
        if (old != fs)
            seaFetchShaderSet(fs);
        if ((u32)gabi::at<SeaShader>(sh)->mDisplayListSize != 0) {
            seaGX2CallDisplayList((u32)gabi::at<SeaShader>(sh)->mpDisplayList,
                                  (u32)gabi::at<SeaShader>(sh)->mDisplayListSize);
            gabi::at<SeaShaderState>(st)->mpFetchShader = fs;
            gabi::at<SeaShaderState>(st)->mpShader = sh;
        } else {
            seaShaderBind(sh);
            gabi::at<SeaShaderState>(st)->mpShader = sh;
            gabi::at<SeaShaderState>(st)->mpFetchShader = fs;
        }
    }
    seaMaterialBind((u32)mpMaterial, program, 0);

    gabi::Local<SeaGfxState> gs;
    u32 stateAddress = gabi::ea(gs.get());
    seaGfxStateConstruct(gs.get());
    u32 w = (u32)gabi::at<SeaGfxState>(stateAddress)->mDepthControl;
    gabi::at<SeaGfxState>(stateAddress)->mDepthTest = 0;
    gabi::at<SeaGfxState>(stateAddress)->mDepthDirty = 1;
    gabi::at<SeaGfxState>(stateAddress)->mDepthMode = 0;
    gabi::at<SeaGfxState>(stateAddress)->mPolygonDirty = 1;
    gabi::at<SeaGfxState>(stateAddress)->mDepthWrite = 1;
    gabi::at<SeaGfxState>(stateAddress)->mDepthCompare = 0;
    gabi::at<SeaGfxState>(stateAddress)->mDepthControl = (((w & ~0xFu) + 7) & 0xFFFFFF0Fu) + 0x10;
    gabi::at<SeaGfxState>(stateAddress)->mPolygonMode = 0;
    gabi::at<SeaGfxState>(stateAddress)->mBlendEnable = 0;
    seaGfxStateApply(gs.get());

    /* the three textures */
    u32 t = 0;
    if ((u32)gabi::at<SeaProgram>(program)->mSamplerCount != 0)
        t = (u32)gabi::at<SeaProgram>(program)->mpSamplers;
    seaSamplerBind((u32)gabi::at<daSea_packet_c>(packetAddress)->mpSeaSampler, t + 4, 0, 0);
    t = 0;
    if ((u32)gabi::at<SeaProgram>(program)->mSamplerCount > 1)
        t = (u32)gabi::at<SeaProgram>(program)->mpSamplers + 0x14;
    seaSamplerBind((u32)gabi::at<daSea_packet_c>(packetAddress)->mpYuraSampler, t + 4, 1, 0);
    t = 0;
    if ((u32)gabi::at<SeaProgram>(program)->mSamplerCount > 2)
        t = (u32)gabi::at<SeaProgram>(program)->mpSamplers + 0x28;
    seaSamplerBind((u32)gabi::at<daSea_packet_c>(packetAddress)->mpSeaSamplerCopy, t + 4, 2, 0);

    /* pass 2: the texture-matrix uniforms of the material */
    if ((s32)gabi::at<SeaDrawPass>(pass)->mPass == 2) {
        u32 m = (u32)gabi::at<SeaDrawPass>(pass)->mpMaterial;
        if (m != 0) {
            u32 blk = m + 0x10 + (u32)gabi::at<SeaPassMaterial>(m)->mCurrentUniformBlock * 0x1C;
            u32 u = 0;
            if ((u32)gabi::at<SeaProgram>(program)->mUniformCount > 4)
                u = (u32)gabi::at<SeaProgram>(program)->mpUniforms + 0x50;
            s16 l0 = (s16)gabi::at<SeaUniform>(u)->mVertexLocation;
            u32 a27 = (u32)gabi::at<SeaUniformBlock>(blk)->mpData;
            u32 a24 = (u32)gabi::at<SeaUniformBlock>(blk)->mSize;
            s16 l1 = (s16)gabi::at<SeaUniform>(u)->mPixelLocation;
            s16 l2;
            bool any = true;
            if (l0 == -1 && l1 == -1) {
                l2 = (s16)gabi::at<SeaUniform>(u)->mGeometryLocation;
                if (l2 == -1)
                    any = false;
            } else {
                l2 = (s16)gabi::at<SeaUniform>(u)->mGeometryLocation;
            }
            if (any) {
                if (l1 != -1)
                    seaGX2SetPixelUniformBlock((s32)l1, a24, a27);
                if (l0 != -1)
                    seaGX2SetVertexUniformBlock((s32)l0, a24, a27);
                if (l2 != -1)
                    seaGX2SetGeometryUniformBlock((s32)l2, a24, a27);
            }
        }
        seaDrawPassBind(pass, program);
    }

    gabi::Local<SeaPolygonOffset> po;
    seaPolygonOffsetReset(po.get());
    seaPolygonOffsetSetFactor(po.get(), 3.0f);
    seaPolygonOffsetSetUnits(po.get(), -20.0f);

    /* the grid: the fetch shader of the buffer just filled, 64 strips of 130 indices */
    {
        u32 l2 = (u32)gabi::at<daSea_packet_c>(packetAddress)->mpPrograms;
        u32 pi2 = (u32)gabi::at<SeaDrawPass>(pass)->mPass;
        u32 n2 = (u32)gabi::at<SeaProgramList>(l2)->mCapacity;
        u32 e2 = (u32)gabi::at<SeaProgramList>(l2)->mpEntries;
        u32 vertexBuffers = (u32)gabi::at<daSea_packet_c>(packetAddress)->mpVtxBuf;
        u32 cur = (u32)gabi::at<SeaBufferSet>(vertexBuffers)->mCurrentBuffer;
        if (pi2 < n2)
            e2 += pi2 * 0x14;
        seaFetchShaderBind(gabi::load<u32>(e2 + (cur == 0 ? 8 : 0) + 8));
        for (u32 i = 0; i < 0x40; i++)
            seaGX2DrawIndexedEx((u32)gabi::at<daSea_packet_c>(packetAddress)->mIndexPrimitive, 0x82,
                                (u32)gabi::at<daSea_packet_c>(packetAddress)->mIndexFormat,
                                (u32)gabi::at<daSea_packet_c>(packetAddress)->mpIndexBufferData +
                                    (u32)gabi::at<daSea_packet_c>(packetAddress)->mIndexStride *
                                        (i * 0x82),
                                0, 1);
    }

    if ((s32)gabi::at<SeaDrawPass>(pass)->mPass == 2) {
        gabi::at<SeaGfxState>(stateAddress)->mCullMode = 1;
        seaGfxStateApply(gs.get());

        f32 vNew = 0, z = 0;
        /* ---- below the grid in z: rows from -450000 up to mDrawMinZ ---- */
        f32 B = mDrawMinZ;
        if (B > LO) {
            s32 n = gabi::ftoi(gabi::fsubs_ppc(B, LO) / sea_step());
            vNew = -225.00003f;
            z = LO;
            if (n > 0) {
                for (s32 i = 0; i < n; i++) {
                    u32 buffers = (u32)gabi::at<daSea_packet_c>(packetAddress)->mpLowerRows;
                    u32 data =
                        (u32)gabi::at<SeaVertexEntry>(
                            buffers +
                            (2 * i + (u32)gabi::at<SeaRowBufferSet>(buffers)->mCurrentBuffer) *
                                sizeof(SeaVertexEntry))
                            ->mpData;
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
            u32 buffers = (u32)gabi::at<daSea_packet_c>(packetAddress)->mpLowerRows;
            sea_flush20(buffers, n);
            sea_draw20((u32)gabi::at<daSea_packet_c>(packetAddress)->mpLowerRows, n, 0xA, false);
            /* the closing row up to mDrawMinZ */
            f32 b = mDrawMinZ;
            if (z < b) {
                u32 data =
                    sea_curData((u32)gabi::at<daSea_packet_c>(packetAddress)->mpLowerClosingRow);
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
                        b = mDrawMinZ;
                }
                sea_flush2(packetAddress, 0x258);
                sea_drawIdx(0xA);
            }
        }
        /* ---- above the grid in z: rows from mDrawMaxZ up to 450000 ---- */
        f32 D = mDrawMaxZ;
        if (D < HI) {
            s32 n = gabi::ftoi(gabi::fsubs_ppc(HI, D) / sea_step());
            z = D;
            vNew = gabi::fmuls_ppc(z, K);
            if (n > 0) {
                for (s32 i = 0; i < n; i++) {
                    u32 buffers = (u32)gabi::at<daSea_packet_c>(packetAddress)->mpUpperRows;
                    u32 cur = (u32)gabi::at<SeaRowBufferSet>(buffers)->mCurrentBuffer;
                    vPrev = vNew;
                    vNew = gabi::fmuls_ppc(gabi::fadds_ppc(z, sea_step()), K);
                    u32 data = (u32)gabi::at<SeaVertexEntry>(buffers +
                                                             (2 * i + cur) * sizeof(SeaVertexEntry))
                                   ->mpData;
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
            u32 buffers = (u32)gabi::at<daSea_packet_c>(packetAddress)->mpUpperRows;
            sea_flush20(buffers, n);
            sea_draw20((u32)gabi::at<daSea_packet_c>(packetAddress)->mpUpperRows, n, 0xA, false);
            if (z < HI) {
                u32 data =
                    sea_curData((u32)gabi::at<daSea_packet_c>(packetAddress)->mpUpperClosingRow);
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
                sea_flush2(packetAddress, 0x260);
                seaGX2DrawEx((u32)gabi::at<SeaDrawIndices>(0x104B4A84)->mPrimitive, 0xA, 0, 1);
            }
        }
        /* ---- left and right of the grid, between mDrawMinZ and mDrawMaxZ ---- */
        D = mDrawMaxZ;
        B = mDrawMinZ;
        if (D > B) {
            f32 g = sea_step();
            f32 A = mDrawMinX;
            s32 n = gabi::ftoi(gabi::fsubs_ppc(D, B) / g);
            if (A > LO) {
                f32 rem = gabi::fsubs_ppc(A, LO);
                s32 m = gabi::ftoi(rem / g);
                bool part = gabi::fmuls_ppc((f32)m, g) < rem;
                z = B;
                vNew = gabi::fmuls_ppc(z, K);
                for (s32 i = 0; i < n; i++) {
                    u32 buffers = (u32)gabi::at<daSea_packet_c>(packetAddress)->mpLeftRows;
                    u32 cur = (u32)gabi::at<SeaRowBufferSet>(buffers)->mCurrentBuffer;
                    vPrev = vNew;
                    u32 data = (u32)gabi::at<SeaVertexEntry>(buffers +
                                                             (2 * i + cur) * sizeof(SeaVertexEntry))
                                   ->mpData;
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
                        f32 a = mDrawMinX;
                        f32 y0 = sea_base();
                        f32 u = gabi::fmuls_ppc(a, K);
                        sea_vtx(data + vi * 0x20, a, y0, gabi::fadds_ppc(z, gg), u, VC);
                        vi++;
                        f32 a2 = mDrawMinX;
                        sea_vtx(data + vi * 0x20, a2, sea_base(), z, u, vPrev);
                    }
                    z = gabi::fadds_ppc(z, sea_step());
                }
                u32 buffers = (u32)gabi::at<daSea_packet_c>(packetAddress)->mpLeftRows;
                sea_flush20(buffers, n);
                u32 count = (u32)(m + (part ? 1 : 0) + 1) * 2;
                sea_draw20((u32)gabi::at<daSea_packet_c>(packetAddress)->mpLeftRows, n, count,
                           true);
                f32 d = mDrawMaxZ;
                if (z < d) {
                    u32 data =
                        sea_curData((u32)gabi::at<daSea_packet_c>(packetAddress)->mpLeftClosingRow);
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
                            d = mDrawMaxZ;
                        }
                    }
                    if (part) {
                        f32 a = mDrawMinX;
                        f32 y0 = sea_base();
                        f32 dd = mDrawMaxZ;
                        f32 u = gabi::fmuls_ppc(a, K);
                        sea_vtx(data + vi * 0x20, a, y0, dd, u, vNew);
                        vi++;
                        f32 a2 = mDrawMinX;
                        sea_vtx(data + vi * 0x20, a2, sea_base(), z, u, vPrev);
                    }
                    sea_flush2(packetAddress, 0x268);
                    if (count != 0)
                        sea_drawIdx(count);
                }
            }
            /* right of the grid: from mDrawMaxX to 450000 */
            f32 Cx = mDrawMaxX;
            if (Cx < HI) {
                f32 g2 = sea_step();
                f32 rem = gabi::fsubs_ppc(HI, Cx);
                s32 m = gabi::ftoi(rem / g2);
                bool part = gabi::fmuls_ppc((f32)m, g2) < rem;
                z = mDrawMinZ;
                vNew = gabi::fmuls_ppc(z, K);
                u32 buffers = (u32)gabi::at<daSea_packet_c>(packetAddress)->mpRightRows;
                f32 x0 = Cx;
                for (s32 i = 0; i < n; i++) {
                    u32 data =
                        (u32)gabi::at<SeaVertexEntry>(
                            buffers +
                            (2 * i + (u32)gabi::at<SeaRowBufferSet>(buffers)->mCurrentBuffer) *
                                sizeof(SeaVertexEntry))
                            ->mpData;
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
                    buffers = (u32)gabi::at<daSea_packet_c>(packetAddress)->mpRightRows;
                    if (i + 1 < n)
                        x0 = mDrawMaxX;
                }
                sea_flush20(buffers, n);
                u32 count = (u32)(m + (part ? 1 : 0) + 1) * 2;
                sea_draw20((u32)gabi::at<daSea_packet_c>(packetAddress)->mpRightRows, n, count,
                           true);
                f32 d = mDrawMaxZ;
                if (z < d) {
                    u32 data = sea_curData(
                        (u32)gabi::at<daSea_packet_c>(packetAddress)->mpRightClosingRow);
                    vPrev = vNew;
                    f32 x = mDrawMaxX;
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
                            d = mDrawMaxZ;
                        }
                    }
                    if (part) {
                        f32 y0 = sea_base();
                        f32 dd = mDrawMaxZ;
                        sea_vtx(data + vi * 0x20, HI, y0, dd, VC, vNew);
                        vi++;
                        sea_vtx(data + vi * 0x20, HI, sea_base(), z, VC, vPrev);
                    }
                    sea_flush2(packetAddress, 0x270);
                    if (count != 0)
                        sea_drawIdx(count);
                }
            }
        }
    }
    seaPolygonOffsetReset(po.get());
    seaPolygonOffsetApply(po.get());
    seaGfxStateApply(gabi::at<void>(J3D + 0x18C));
}

/*  (HD, new): draw callback of the packet's material: passes draw passes 1 and 2 (r4->0xC)
 * on to the sea draw  */
/* 0246F6AC */
static void daSea_drawPass(void* a, void* pass) {
    u32 k = (u32)gabi::at<SeaDrawPass>(gabi::ea(pass))->mPass;
    if (k < 1 || k > 2)
        return;
    ((daSea_packet_c*)a)->draw(pass);
}

// HD: three native constructor copies initialize vertex-buffer and fetch-shader subobjects.
static inline u32 sea_vtxEntry_ct(u32 p) {
    if (p == 0) {
        p = gabi::ea(operator_new(0x254));
        if (p == 0)
            return p;
    }
    seaVertexBufferConstruct(p + 4);
    seaFetchShaderConstruct(p + 0x158);
    gabi::at<SeaVertexEntry>(p)->mpAllocatedData = 0;
    gabi::at<SeaVertexEntry>(p)->mVertexCount = 0;
    return p;
}

// HD: compiler-generated and template code of this translation unit.

/* 0246B33C */
static daSea_WaveInfo* daSea_WaveInfo_ct(daSea_WaveInfo* i_this) {
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

/* 0246B3AC */
static daSea_packet_c* daSea_packet_c_ct(daSea_packet_c* i_this) {
    if (i_this == nullptr) {
        i_this = (daSea_packet_c*)operator_new(0x3A8);
        if (i_this == nullptr)
            return i_this;
    }
    seaPacketConstruct(i_this); /* J3DPacket::J3DPacket */
    i_this->__vtbl = 0x10039F38;
    i_this->mWaterHeightMgr.__vtbl = 0x10039E50;
    daSea_WaveInfo_ct(&i_this->mWaveInfo);
    seaIndexBufferConstruct(gabi::at<void>(gabi::ea(i_this) + 0x234));
    seaTextureConstruct(gabi::at<void>(gabi::ea(i_this) + 0x27C));
    seaTextureConstruct(gabi::at<void>(gabi::ea(i_this) + 0x30C));
    return i_this;
}

/* 0246E248 */
static void daSea_WaveInfo_dt(daSea_WaveInfo* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

/* 0246F768 */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

/* 0246F8BC */
static void sea_class_dt(sea_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        seaActorDestruct(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 0246F910 */
static void daSea_packet_c_dt(daSea_packet_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        seaIndexBufferDestruct(gabi::at<void>(gabi::ea(i_this) + 0x234), 2);
        daSea_WaveInfo_dt(&i_this->mWaveInfo, 2);
        seaPacketDestruct(i_this, 0); /* J3DPacket::~J3DPacket */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 0246F890 */
static void* daSea_empty_ct(void* i_this) {
    if (i_this == nullptr)
        i_this = operator_new(0x10);
    return i_this;
}

/* 0246F6C4 */
static void sinit_d_a_sea() {
    gabi::at<SeaHeaderStatics>(L_CLOTH_PTR)->mWords[2] = 0;
    gabi::at<SeaHeaderStatics>(L_CLOTH_PTR)->mWords[0] = 0;
    gabi::at<SeaHeaderStatics>(L_CLOTH_PTR)->mWords[3] = 0;
    gabi::at<SeaHeaderStatics>(L_CLOTH_PTR)->mWords[1] = 0;
    __register_global_object(0x101D017C);
    gabi::at<SeaHeaderStatics>(L_CLOTH_PTR)->mAngleMin = -3.1415927f;
    gabi::at<SeaHeaderStatics>(L_CLOTH_PTR)->mAngleMax = 3.1415927f;
    seaAngleHelperConstruct(gabi::ea(&gabi::at<SeaHeaderStatics>(L_CLOTH_PTR)->mAngleHelper));
    __register_global_object(0x101D0188);
    seaRangeHelperConstruct(gabi::ea(&gabi::at<SeaHeaderStatics>(L_CLOTH_PTR)->mRangeHelper));
    __register_global_object(0x101D0194);
    l_skirtStep = 225000;
}

/* 0246F77C */
static u32 sea_vtxEntry_ct0(u32 p) {
    return sea_vtxEntry_ct(p);
}

/* 0246F7D8 */
static u32 sea_vtxEntry_ct1(u32 p) {
    return sea_vtxEntry_ct(p);
}

/* 0246F834 */
static u32 sea_vtxEntry_ct2(u32 p) {
    return sea_vtxEntry_ct(p);
}

/* 0246F97C */
static void daSea_packet_c_v14(u32) {}

/* 0246F980 */
static void SafeString_assureTerminationImpl(u32) {}

/* 0246F984 */
static BOOL daSea_true_0(u32) {
    return TRUE;
}

/* 0246F98C */
static BOOL daSea_true_1(u32) {
    return TRUE;
}

// clang-format off
/* WWHD glue
ext 027F1278 void seaPacketConstruct(daSea_packet_c*)
ext 027B5430 void seaIndexBufferConstruct(void*)
ext 027BE6B8 void seaTextureConstruct(void*)
ext 027B5E94 void seaBufferFlush(u32,u32,u32)
ext 0255F8A0 void seaPrepareEnvironment()
ext 027FDA54 void seaMaterialSetMatrices(u32,u32,be<f32>*,be<u32>*,u32)
ext 028E9098 void seaMtxIdentity(u32)
ext 028E945C void seaMtxScale(u32,f32,f32,f32)
ext 028E93CC void seaMtxTrans(u32,f32,f32,f32)
ext 027FBB58 void seaSetIndirectMatrices(u32,u32,be<f32>*,be<f32>*)
ext 025602F0 void seaGetColors(GXColor*,GXColor*)
ext 027FE0DC void seaMaterialUpdate(u32,u32)
ext 0255F84C void seaRestoreEnvironment()
ext 027F0E04 void seaDrawBufferEntry(u32,daSea_packet_c*,u32)
ext 0246C5E8 void seaFillVtxBufNative(daSea_packet_c*,u32)
ext 0246C7C8 void seaSetUniformsNative(daSea_packet_c*)
ext 02756140 u32 seaHeapGetCurrent(u32)
ext 02755FEC u32 seaHeapFindOwner(u32,u32)
ext 027FF478 void seaVertexBufferBind(u32,u32,u32,u32)
ext 027FF530 void seaProgramBindAttributes(u32,u32,u32,u32,u32)
ext 027BF734 void seaFetchShaderConstruct(u32)
ext 0280068C void seaProgramListCreate(u32,u32,u32)
ext 027B548C void seaIndexBufferInvalidate(u32)
ext 027B54E0 void seaIndexBufferSet(u32,u32,u32,u32)
ext 0274FBF8 void seaTextureLock(u32)
ext 02773798 void seaTextureLoadImage(u32,u32)
ext 027BE114 u32 seaSamplerConstruct(u32,u32)
ext 0274FCCC void seaTextureUnlock(u32)
ext 027FFCBC u32 seaShaderManagerGet()
ext 027B90AC s32 seaShaderFind(u32,SafeString*)
ext 02800B0C void seaShaderProgramCreate(u32,u32,u32)
ext 027FD6F4 void seaMaterialConstruct(u32)
ext 027FB40C void seaMaterialBlockConstruct(u32)
ext 028F521C void seaMemoryClear(u32,u32)
ext 027FE084 void seaMaterialInitialize(u32,u32,u32)
ext 0246DD14 bool seaCreateNative(daSea_packet_c*,cXyz*)
ext 025D50BC void seaActorDestruct(sea_class*,u32)
ext 027B54A0 void seaIndexBufferDestruct(void*,s32)
ext 027F13DC void seaPacketDestruct(daSea_packet_c*,u32)
ext 027BFE5C void seaFetchShaderBind(u32)
ext C0006178 void seaGX2DrawIndexedEx(u32,u32,u32,u32,u32,u32)
ext 027F29D4 u32 seaShaderStateGet(u32)
ext 027BB9E0 void seaShaderValidate(u32,u32)
ext 027B9F68 void seaFetchShaderSet(u32)
ext C00060E0 void seaGX2CallDisplayList(u32,u32)
ext 027BB7CC void seaShaderBind(u32)
ext 027FE118 void seaMaterialBind(u32,u32,u32)
ext 02750250 void seaGfxStateConstruct(SeaGfxState*)
ext 02750370 void seaGfxStateApply(void*)
ext 027BE53C void seaSamplerBind(u32,u32,u32,u32)
ext C0006900 void seaGX2SetPixelUniformBlock(s32,u32,u32)
ext C0006A38 void seaGX2SetVertexUniformBlock(s32,u32,u32)
ext C00068A8 void seaGX2SetGeometryUniformBlock(s32,u32,u32)
ext 027FFE54 void seaDrawPassBind(u32,u32)
ext 027E1A0C void seaPolygonOffsetReset(SeaPolygonOffset*)
ext 027E1A30 void seaPolygonOffsetSetFactor(SeaPolygonOffset*,f32)
ext 027E1A44 void seaPolygonOffsetSetUnits(SeaPolygonOffset*,f32)
ext C0006170 void seaGX2DrawEx(u32,u32,u32,u32)
ext 027E1A08 void seaPolygonOffsetApply(SeaPolygonOffset*)
ext 028ED6F8 void seaAngleHelperConstruct(u32)
ext 028EAB2C void seaRangeHelperConstruct(u32)
ext 027B5BD8 void seaVertexBufferConstruct(u32)
ext 028EFFD0 void seaConstructArray(u32,u32,u32,u32)
ext 02019510 s16 cM_rad2s(f32)
ext 025601C0 void dKy_usonami_set(f32)
ext 02010D18 void cM3d_CalcPla(cXyz*,cXyz*,cXyz*,cXyz*,be<f32>*)
ext 0273ADAC void* operator_new_arr(u32)
ext 02010594 void cM2dGBox_ct(cM2dGBox*)
ext 020105F8 void cM2dGBox_dt(cM2dGBox*,s32)
ext 0201060C void cM2dGBox_Set(cM2dGBox*,be<f32>*,be<f32>*)
ext 02010630 f32 cM2dGBox_GetLen(cM2dGBox*,be<f32>*)
ext 0274D458 void Color4f_mul(SeaColor4f*,SeaColor4f*,f32)
ext 0274D380 void Color4f_sub(SeaColor4f*,SeaColor4f*,SeaColor4f*)
ext 0274D314 void Color4f_add(SeaColor4f*,SeaColor4f*,SeaColor4f*)
data l_baseHeight 101D01A0
data wi_prm_ocean 101D0110
data pos_around 101D00E0
str 10039E94 "A_umikz"
str 10039EE0 "Siren"
str 10039EE8 "Always"
str 10039EF0 "wave_draw"
data l_gridOne 10039E74
data l_gridZero 10039E70
data l_gridStep 10039E88
data l_gridTexScale 10039EB4
data l_materialZero 10145180
data l_materialOne 1014517C
data l_skirtStep 1046DC7C
ext 025E121C void* fpcSch_JudgeForPName(void*, void*) callback
*/
