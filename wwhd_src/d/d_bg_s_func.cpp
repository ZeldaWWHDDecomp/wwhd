/** Background ground, water and special-group queries. */
#include "d/d_bg_s_func.h"

static const f32 noGround = -1000000000.0f;
static const u32 headerPiMinBits = 0xC0490FDB;
static const u32 headerPiMaxBits = 0x40490FDB;
// HD: distinct GHS vtable copies for the two inline ground-check variants.
enum : u32 {
    objectCheckVtable = 0x10043AD4,
    objectPolyInfoVtable = 0x10043AE4,
    objectPolyPassVtable = 0x10043B04,
    objectGroupPassVtable = 0x10043AF4,
    waterCheckVtable = 0x10043B14,
    waterPolyInfoVtable = 0x10043B24,
    waterPolyPassVtable = 0x10043B44,
    waterGroupPassVtable = 0x10043B34,
    basePolyInfoVtable = 0x10043AA4,
    basePolyPassVtable = 0x10043AC4,
    baseGroupPassVtable = 0x10043A84
};
#define roomStatus (gabi::at<BackgroundRoomStatus>(0x1047E6CC))

/* inlined into the background query helpers */
static void configure(GroundCheck *check, bool water) {
    check->mCheck.mPolyPassChk = &check->mPolyPass;
    check->mCheck.mGrpPassChk = &check->mGrpPass;
    check->mCheck.__vtbl = water ? waterCheckVtable : objectCheckVtable;
    check->mPolyInfo.__vtbl = water ? waterPolyInfoVtable : objectPolyInfoVtable;
    check->mPolyPass.__vtbl = water ? waterPolyPassVtable : objectPolyPassVtable;
    check->mGrpPass.__vtbl = water ? waterGroupPassVtable : objectGroupPassVtable;
    check->mGrpPass.mGrp = water ? FuncGroupPass::WATER_GRP : FuncGroupPass::NORMAL_GRP;
    check->mPolyPass.mbObjThrough = 0;
    check->mPolyPass.mbCamThrough = 0;
    check->mPolyPass.mbLinkThrough = 0;
    check->mPolyPass.mbArrowThrough = 0;
    check->mPolyPass.mbBombThrough = 0;
    check->mPolyPass.mbBoomerangThrough = 0;
    check->mPolyPass.mbRopeThrough = 0;
    check->mPolyPass.mbObjThrough = 1;
}
/* inlined into the background query helpers */
static void teardown(GroundCheck *check) {
    check->mPolyInfo.__vtbl = basePolyInfoVtable;
    check->mPolyPass.__vtbl = basePolyPassVtable;
    check->mGrpPass.__vtbl = baseGroupPassVtable;
    destroyGroundCheck(check, 0);
}
/* inlined into the background query helpers */
static BackgroundSpace *dComIfG_Bgsp() {
    return &dComIfGp_getGameInfo()->mBgS;
}

/* 024F1350 */
f32 dBgS_ObjGndChk_Func(const cXyz *pos) {
    gabi::Stack<GroundCheck> check;
    constructGroundCheck(check.get());
    configure(check.get(), false);
    // HD: integer copies load all three source words before storing the position.
    const VectorWords *source = (const VectorWords *)pos;
    VectorWords *destination = (VectorWords *)&check->m_pos;
    u32 x = source->x, z = source->z, y = source->y;
    destination->x = x;
    destination->y = y;
    destination->z = z;
    BackgroundSpace *bg = dComIfG_Bgsp();
    f32 height = GroundCross(bg, check.get());
    teardown(check.get());
    return height;
}

/* 024F1478 */
f32 dBgS_ObjGndChk_Wtr_Func(const cXyz *pos) {
    gabi::Stack<GroundCheck> check;
    constructGroundCheck(check.get());
    configure(check.get(), true);
    const VectorWords *source = (const VectorWords *)pos;
    VectorWords *destination = (VectorWords *)&check->m_pos;
    u32 x = source->x, z = source->z, y = source->y;
    destination->x = x;
    destination->y = y;
    destination->z = z;
    BackgroundSpace *bg = dComIfG_Bgsp();
    f32 height = GroundCross(bg, check.get());
    teardown(check.get());
    return height;
}

/* 024F15A4 */
BOOL dBgS_SplGrpChk_In_ObjGnd(const cXyz *pos, SpecialGroupCheck *group, f32 offset) {
    gabi::Stack<GroundCheck> check;
    gabi::Stack<Bounds> bounds;
    constructGroundCheck(check.get());
    configure(check.get(), false);
    f32 z = pos->z, y = pos->y, x = pos->x;
    check->m_pos.y = y + offset;
    check->m_pos.x = x;
    check->m_pos.z = z;
    BackgroundSpace *bg = dComIfG_Bgsp();
    f32 height = GroundCross(bg, check.get());
    group->m_height = height;
    BOOL success = 0;
    if (height != noGround) {
        bg = dComIfG_Bgsp();
        u32 room = GetRoomId(bg, &check->mPolyInfo);
        if (room >= 64)
            JUT_ASSERT_fail(gabi::at<char>(0x10043B58), 93, gabi::at<char>(0x10043B68));
        // HD: accessor side effect precedes the fixed room-status table lookup.
        dComIfGp_getGameInfo();
        cBgW *world = roomStatus[room].mBgW.get();
        if (world != NULL) {
            GetTopUnder(world, &bounds->top, &bounds->bottom);
            f32 px = pos->x, pz = pos->z, top = bounds->top;
            // HD: retain the otherwise unused integer read from the source vector.
            u32 originalY = ((const VectorWords *)pos)->y;
            (void)originalY;
            group->m_roof = top;
            group->m_ground.x = px;
            group->m_ground.y = height;
            group->m_ground.z = pz;
            bg = dComIfG_Bgsp();
            if (SplGrpChk(bg, group)) {
                f32 actual = group->m_height, originalHeight = pos->y;
                if (actual > originalHeight)
                    group->m_flags = (u32)group->m_flags | SpecialGroupCheck::IN;
                success = 1;
            }
        }
    }
    teardown(check.get());
    return success;
}

/* 024F17D4 */
f32 dBgS_GetWaterHeight(const cXyz *pos) {
    gabi::Stack<GroundCheck> check;
    constructGroundCheck(check.get());
    configure(check.get(), true);
    const VectorWords *source = (const VectorWords *)pos;
    VectorWords *destination = (VectorWords *)&check->m_pos;
    destination->x = source->x;
    destination->z = source->z;
    destination->y = source->y;
    BackgroundSpace *bg = dComIfG_Bgsp();
    f32 height = GroundCross(bg, check.get());
    f32 z = pos->z, x = pos->x;
    if (daSea_ChkArea(x, z)) {
        x = pos->x;
        z = pos->z;
        f32 wave = daSea_calcWave(x, z);
        if (height < wave)
            height = wave;
    }
    teardown(check.get());
    return height;
}

/* 024F1914 */
// HD: modifies the guest position in place; GameCube passes its vector by value.
u32 dBgS_GetGndMtrlSndId_Func(cXyz *pos, f32 offset) {
    pos->y = (f32)pos->y + offset;
    gabi::Stack<GroundCheck> check;
    constructGroundCheck(check.get());
    configure(check.get(), false);
    const VectorWords *source = (const VectorWords *)pos;
    VectorWords *destination = (VectorWords *)&check->m_pos;
    u32 z = source->z, x = source->x, y = source->y;
    destination->x = x;
    destination->z = z;
    destination->y = y;
    BackgroundSpace *bg = dComIfG_Bgsp();
    f32 height = GroundCross(bg, check.get());
    u32 result = 0;
    if (height != noGround) {
        bg = dComIfG_Bgsp();
        result = GetMtrlSndId(bg, &check->mPolyInfo);
    }
    teardown(check.get());
    return result;
}

// HD: compiler-generated initialization of this translation unit's header statics.
struct HeaderStatics {
    be<f32> piMin, piMax;
    u8 byteObject, flagObject;
    u8 padding[2];
};
struct HeaderZeroObject {
    be<u32> words[4];
};
#define headerStatics (*gabi::at<HeaderStatics>(0x1046ED28))
#define headerZeroObject (*gabi::at<HeaderZeroObject>(0x1046ED34))
/* 024F1A68 */
static void __sinit_d_bg_s_func_cpp() {
    headerZeroObject.words[2] = 0;
    headerZeroObject.words[0] = 0;
    headerZeroObject.words[3] = 0;
    headerZeroObject.words[1] = 0;
    registerGlobalObject(gabi::at<void>(0x101D5270));
    u32 lower = headerPiMinBits, upper = headerPiMaxBits;
    // HD: lfs/stfs stores preserve these exact source words.
    ((VectorWords *)&headerStatics)->x = lower;
    ((VectorWords *)&headerStatics)->y = upper;
    initializeHeaderByte(&headerStatics.byteObject);
    registerGlobalObject(gabi::at<void>(0x101D527C));
    initializeHeaderFlag(&headerStatics.flagObject);
    registerGlobalObject(gabi::at<void>(0x101D5288));
}

/* WWHD glue (tools/verify/autoglue.py)
data noGround 10043B54
data headerPiMinBits 10043B8C
data headerPiMaxBits 10043B90
ext 02008E0C void constructGroundCheck(GroundCheck*)
ext 02008DAC void destroyGroundCheck(GroundCheck*,u32)
ext 025200D4 BackgroundGameInfo* dComIfGp_getGameInfo()
ext 02008974 f32 GroundCross(BackgroundSpace*,GroundCheck*)
ext 024EF130 u32 GetRoomId(BackgroundSpace*,FuncPolyInfo*)
ext 024EECAC u32 GetMtrlSndId(BackgroundSpace*,FuncPolyInfo*)
ext 024EF7C0 BOOL SplGrpChk(BackgroundSpace*,SpecialGroupCheck*)
ext 0200A3FC void GetTopUnder(cBgW*,be<f32>*,be<f32>*)
ext 0273AA24 void JUT_ASSERT_fail(const char*,s32,const char*)
ext 0246B6A4 BOOL daSea_ChkArea(f32,f32)
ext 0246BA0C f32 daSea_calcWave(f32,f32)
ext 028F026C void registerGlobalObject(void*)
ext 028ED6F8 void initializeHeaderByte(void*)
ext 028EAB2C void initializeHeaderFlag(void*)
*/
