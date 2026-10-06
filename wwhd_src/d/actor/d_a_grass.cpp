/**
 * d_a_grass.cpp (WWHD)
 * Grass / tree / flower placement: registers a group of units with the dGrass, dTree or
 * dFlower packet and deletes itself.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_grass.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define GRASS_VTBL 0x10010654 /* grass_class vtable (HD virtual destructor) */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
WWHD_OPAQUE(dGrass_packet_c);
WWHD_OPAQUE(dTree_packet_c);
WWHD_OPAQUE(dFlower_packet_c);
/* dComIfG_play_c::createGrass/createTree/createFlower (HD: `this` is play+0x12A0) */
static inline void* dComIfGp_createGrass() { return gabi::call<void*>(0x02524CC0, dComIfGp_ea() + 0x12A0); }
static inline void* dComIfGp_createTree() { return gabi::call<void*>(0x02524DC0, dComIfGp_ea() + 0x12A0); }
static inline void* dComIfGp_createFlower() { return gabi::call<void*>(0x02524FC0, dComIfGp_ea() + 0x12A0); }
/* packets: play+0x5AB8 (grass), +0x5ABC (tree), +0x5AC4 (flower) */
static inline dGrass_packet_c* dComIfGp_getGrass() { return gabi::at<dGrass_packet_c>(gabi::load<u32>(dComIfGp_ea() + 0x5AB8)); }
static inline dTree_packet_c* dComIfGp_getTree() { return gabi::at<dTree_packet_c>(gabi::load<u32>(dComIfGp_ea() + 0x5ABC)); }
static inline dFlower_packet_c* dComIfGp_getFlower() { return gabi::at<dFlower_packet_c>(gabi::load<u32>(dComIfGp_ea() + 0x5AC4)); }
/* 0254D2D4 dGrass_packet_c::newData(cXyz&, int roomNo, s8 itemIdx) */
static inline void dGrass_newData(dGrass_packet_c* p, cXyz* pos, s32 room, s8 item) { gabi::call(0x0254D2D4, p, pos, room, item); }
/* 025CAAF4 dTree_packet_c::newData(cXyz&, u8, int roomNo) */
static inline void dTree_newData(dTree_packet_c* p, cXyz* pos, u8 st, s32 room) { gabi::call(0x025CAAF4, p, pos, st, room); }
/* 02548FEC dFlower_packet_c::newData(s8 type, cXyz&, int roomNo, s8 itemIdx) */
static inline void dFlower_newData(dFlower_packet_c* p, s8 type, cXyz* pos, s32 room, s8 item) { gabi::call(0x02548FEC, p, type, pos, room, item); }

struct grass_class : fopAc_ac_c {
    /* 0x3AC */ be<u32> field_0x00;
    /* 0x3B0 */ be<u32> field_0x04;
};
WWHD_SIZE(grass_class, 0x3B4);

static inline s8 getItemNo(grass_class* ac) { return (fopAcM_GetParam(ac) >> 6) & 0x3F; }
static inline u8 getKind(u32 prm) { return (prm >> 4) & 0x03; }

/* 021618D0 */
static BOOL daGrass_IsDelete(grass_class*) {
    WWHD_FUNC(0x021618D0, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021618D0, daGrass_IsDelete);

/* 021618D8 */
static BOOL daGrass_Delete(grass_class*) {
    WWHD_FUNC(0x021618D8, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x021618D8, daGrass_Delete);

/* HD: the function-local `static const csXyz l_setTypeN[]` arrays are built on first use
 * (guard word, then one out-of-line csXyz constructor call per element) */
static void init_set(u32 guard, u32 arr, const s16 (*v)[2], int n) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        for (int i = 0; i < n; i++)
            csXyz_ct(gabi::at<csXyz>(arr + 6 * i), v[i][0], 0, v[i][1]);
    }
}
static const s16 l_setType0[][2] = {{0, 0}, {3, -50}, {-2, 50}, {50, 27}, {52, -25}, {-50, 22}, {-50, -29}};
static const s16 l_setType1[][2] = {{-18, 76}, {-15, 26}, {133, 0}, {80, 23}, {86, -83}, {33, -56}, {83, -27},
                                    {-120, -26}, {-18, -74}, {-20, -21}, {-73, 1}, {-67, -102}, {-21, 126},
                                    {-120, -78}, {-70, -49}, {32, 103}, {34, 51}, {-72, 98}, {-68, 47}, {33, -5},
                                    {135, -53}};
static const s16 l_setType2[][2] = {{-75, -50}, {75, -25}, {14, 106}};
static const s16 l_setType3[][2] = {{-24, -28}, {27, -28}, {-21, 33}, {-18, -34}, {44, -4}, {41, 10}, {24, 39}};
static const s16 l_setType4[][2] = {{-55, -22}, {-28, -50}, {-77, 11}, {55, -44}, {83, -71}, {11, -48},
                                    {97, -34}, {-74, -57}, {31, 58}, {59, 30}, {13, 23}, {-12, 54},
                                    {55, 97}, {10, 92}, {33, -10}, {-99, -27}, {40, -87}};
static const s16 l_setType5[][2] = {{0, 3}, {-26, -29}, {7, -25}, {31, -5}, {-7, 40}, {-35, 15}, {23, 32}};
static const s16 l_setType6[][2] = {{-40, 0}, {0, 0}, {80, 0}, {-80, 0}, {40, 0}};

/* static struct OffsetData { u8 num; const csXyz* pos; } l_offsetData[8] (.data 0x101B5D7C) */
struct OffsetData {
    /* 0x0 */ be<u8> num;
    /* 0x1 */ u8 _1[3];
    /* 0x4 */ gptr<csXyz> pos;
};
WWHD_SIZE(OffsetData, 8);
static OffsetData* l_offsetData() { return gabi::at<OffsetData>(0x101B5D7C); }

/* 021618E0 */
static cPhs_State daGrass_Create(fopAc_ac_c* i_ac) {
    WWHD_FUNC(0x021618E0, cPhs_State, i_ac);
    init_set(0x10464370, 0x104641D0, l_setType0, 7);
    init_set(0x10464374, 0x10464288, l_setType1, 21);
    init_set(0x10464378, 0x104641FC, l_setType2, 3);
    init_set(0x1046437C, 0x10464210, l_setType3, 7);
    init_set(0x10464380, 0x10464308, l_setType4, 17);
    init_set(0x10464384, 0x1046423C, l_setType5, 7);
    init_set(0x10464388, 0x10464268, l_setType6, 5);

    grass_class* i_this = (grass_class*)i_ac;

    /* fopAcM_ct(i_this, grass_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            fopAc_ac_c_ct(i_this);
            i_this->__vtbl = GRASS_VTBL;
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }

    u32 prm = fopAcM_GetParam(i_this);
    u32 grp = prm & 0x0F;
    if (grp >= 8) { /* HD: JUT_ASSERT(0xF5, a_grass_type < ARRAY_SIZE(l_offsetData)) */
        JUT_ASSERT_fail(STR(0x100106B0), 0xF5, STR(0x10010670));
        prm = fopAcM_GetParam(i_this);
        grp = 0;
    }
    OffsetData* offset = &l_offsetData()[grp];
    const csXyz* ofpos = offset->pos;
    u32 kind = getKind(prm);
    cXyz* acpos = &i_this->current.pos;

    if (kind == 0) {
        /* grass */
        if (dComIfGp_createGrass() != nullptr) {
            s8 item = getItemNo(i_this);
            if (item < 0x20 || item >= 0x40)
                item = -1;

            for (s32 i = 0; i < offset->num; ofpos++, i++) {
                gabi::Local<cXyz> pos;
                pos->x = acpos->x + (f32)ofpos->x;
                pos->y = acpos->y;
                pos->z = acpos->z + (f32)ofpos->z;
                dGrass_packet_c* grass = dComIfGp_getGrass();
                dGrass_newData(grass, pos, fopAcM_GetRoomNo(i_this), item);
            }
        }
    } else if (kind == 1) {
        /* tree */
        if (dComIfGp_createTree() != nullptr) {
            f32 cosR = cM_scos(i_this->current.angle.y), sinR = cM_ssin(i_this->current.angle.y);
            gabi::Local<cXyz> pos;
            pos->y = acpos->y;

            for (s32 i = 0; i < offset->num; ofpos++, i++) {
                pos->x = acpos->x + gabi::fmadds((f32)ofpos->x, cosR, (f32)ofpos->z * sinR);
                pos->y = acpos->y;
                pos->z = acpos->z + gabi::fmsubs((f32)ofpos->z, cosR, (f32)ofpos->x * sinR);
                dTree_packet_c* tree = dComIfGp_getTree();
                dTree_newData(tree, pos, 0, fopAcM_GetRoomNo(i_this));
            }
        }
    } else if (kind == 2 || kind == 3) {
        /* white flower, pink flower */
        if (dComIfGp_createFlower() != nullptr) {
            s8 item = getItemNo(i_this);
            if (item < 0x20 || item >= 0x40)
                item = (s8)0xFF;

            s8 flowerType = 0;
            if (kind == 2)
                flowerType = 1;
            else if (kind == 3)
                flowerType = 2;

            for (s32 i = 0; i < offset->num; ofpos++, i++) {
                gabi::Local<cXyz> pos;
                pos->x = acpos->x + (f32)ofpos->x;
                pos->y = acpos->y;
                pos->z = acpos->z + (f32)ofpos->z;
                dFlower_packet_c* flower = dComIfGp_getFlower();
                dFlower_newData(flower, flowerType, pos, fopAcM_GetRoomNo(i_this), item);
            }
        }
    }

    return cPhs_ERROR_e;
}
VERIFY(0x021618E0, daGrass_Create);

/* 021622DC: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_grass_cpp() {
    WWHD_FUNC(0x021622DC, void, (u32)0);
    sinit_header_statics(0x104641B4, 0x101B5DDC);
}
VERIFY(0x021622DC, __sinit_d_a_grass_cpp);

/* 02162370: grass_class deleting destructor (compiler-generated, HD virtual destructor) */
static void grass_class_dt(grass_class* i_this, s32 flags) {
    WWHD_FUNC(0x02162370, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02162370, grass_class_dt);
