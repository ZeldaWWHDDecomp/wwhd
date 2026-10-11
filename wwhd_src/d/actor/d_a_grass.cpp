/**
 * d_a_grass.cpp (WWHD)
 * Grass / tree / flower placement: registers a group of units with the dGrass, dTree or
 * dFlower packet and deletes itself.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_grass.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_grass.h"
#include "gc_api/d/d_grass.h"
#include "gc_api/d/d_tree.h"
#include "gc_api/d/d_flower.h"

const gc_tu_data gc_tu = {.headerStatics = {0x104641B4, 0x101B5DDC, 0x104641C0}};

namespace daGrass_prm {
static inline s8 getItemNo(grass_class* ac) { return (fopAcM_GetParam(ac) >> 6) & 0x3F; }
// HD: retain the parameter value read before the placement callbacks.
inline u8 getKind(u32 prm) { return (prm >> 4) & 0x03; }
}

/* 021618D0 */
static BOOL daGrass_IsDelete(grass_class*) {
    return TRUE;
}

/* 021618D8 */
static BOOL daGrass_Delete(grass_class*) {
    return TRUE;
}

// HD: set the guard before the out-of-line element constructors, as GHS does.
template <int N>
static void init_set(be<u32>& guard, GrassOffsetSet<N>& offsets, const s16 (&coordinates)[N][2]) {
    if (guard == 0) {
        guard = 1;
        for (int i = 0; i < N; i++) {
            csXyz_ct(&offsets.pos[i], coordinates[i][0], 0, coordinates[i][1]);
        }
    }
}
static const s16 l_coordinates0[][2] = {{0, 0}, {3, -50}, {-2, 50}, {50, 27}, {52, -25}, {-50, 22}, {-50, -29}};
static const s16 l_coordinates1[][2] = {{-18, 76}, {-15, 26}, {133, 0}, {80, 23}, {86, -83}, {33, -56}, {83, -27},
                                    {-120, -26}, {-18, -74}, {-20, -21}, {-73, 1}, {-67, -102}, {-21, 126},
                                    {-120, -78}, {-70, -49}, {32, 103}, {34, 51}, {-72, 98}, {-68, 47}, {33, -5},
                                    {135, -53}};
static const s16 l_coordinates2[][2] = {{-75, -50}, {75, -25}, {14, 106}};
static const s16 l_coordinates3[][2] = {{-24, -28}, {27, -28}, {-21, 33}, {-18, -34}, {44, -4}, {41, 10}, {24, 39}};
static const s16 l_coordinates4[][2] = {{-55, -22}, {-28, -50}, {-77, 11}, {55, -44}, {83, -71}, {11, -48},
                                    {97, -34}, {-74, -57}, {31, 58}, {59, 30}, {13, 23}, {-12, 54},
                                    {55, 97}, {10, 92}, {33, -10}, {-99, -27}, {40, -87}};
static const s16 l_coordinates5[][2] = {{0, 3}, {-26, -29}, {7, -25}, {31, -5}, {-7, 40}, {-35, 15}, {23, 32}};
static const s16 l_coordinates6[][2] = {{-40, 0}, {0, 0}, {80, 0}, {-80, 0}, {40, 0}};

/* 021618E0 */
static cPhs_State daGrass_Create(fopAc_ac_c* i_ac) {

    static u32 l_guard0;
    static GrassOffsetSet7 l_setType0;
    init_set(l_guard0, l_setType0, l_coordinates0);
    static u32 l_guard1;
    static GrassOffsetSet21 l_setType1;
    init_set(l_guard1, l_setType1, l_coordinates1);
    static u32 l_guard2;
    static GrassOffsetSet3 l_setType2;
    init_set(l_guard2, l_setType2, l_coordinates2);
    static u32 l_guard3;
    static GrassOffsetSet7 l_setType3;
    init_set(l_guard3, l_setType3, l_coordinates3);
    static u32 l_guard4;
    static GrassOffsetSet17 l_setType4;
    init_set(l_guard4, l_setType4, l_coordinates4);
    static u32 l_guard5;
    static GrassOffsetSet7 l_setType5;
    init_set(l_guard5, l_setType5, l_coordinates5);
    static u32 l_guard6;
    static GrassOffsetSet5 l_setType6;
    init_set(l_guard6, l_setType6, l_coordinates6);

    grass_class* i_this = (grass_class*)i_ac;

    fopAcM_ct(i_this, grass_class);

    u32 prm = fopAcM_GetParam(i_this);
    u32 grp = prm & 0x0F;
    if (grp >= 8) { /* HD: JUT_ASSERT(0xF5, a_grass_type < (sizeof(l_offsetData) / sizeof(l_offsetData[0]))) */
        // HD: shorten the source-path text; glue keeps the original guest string.
        JUT_ASSERT_fail("d_a_grass.cpp", 0xF5, "a_grass_type < (sizeof(l_offsetData) / sizeof(l_offsetData[0]))");
        prm = fopAcM_GetParam(i_this);
        grp = 0;
    }
    // HD: guest pointers in the checked placement table name the static arrays above.
    static OffsetData l_offsetData[] = {
        {1, {}, 0x104641D0}, {7, {}, 0x104641D0}, {21, {}, 0x10464288},
        {3, {}, 0x104641FC}, {7, {}, 0x10464210}, {17, {}, 0x10464308},
        {7, {}, 0x1046423C}, {5, {}, 0x10464268},
    };
    OffsetData* offset = &l_offsetData[grp];
    const csXyz* ofpos = offset->pos;
    u32 kind = daGrass_prm::getKind(prm);
    cXyz* acpos = &i_this->current.pos;

    if (kind == 0) {
        /* grass */
        if (dComIfGp_createGrass() != nullptr) {
            s8 item = daGrass_prm::getItemNo(i_this);
            if (item < 0x20 || item >= 0x40)
                item = -1;

            for (s32 i = 0; i < offset->num; ofpos++, i++) {
                cXyz pos;
                pos.x = acpos->x + (f32)ofpos->x;
                pos.y = acpos->y;
                pos.z = acpos->z + (f32)ofpos->z;
                dGrass_packet_c* grass = dComIfGp_getGrass();
                grass->newData(&pos, fopAcM_GetRoomNo(i_this), item);
            }
        }
    } else if (kind == 1) {
        /* tree */
        if (dComIfGp_createTree() != nullptr) {
            f32 cosR = cM_scos(i_this->current.angle.y), sinR = cM_ssin(i_this->current.angle.y);
            cXyz pos;
            pos.y = acpos->y;

            for (s32 i = 0; i < offset->num; ofpos++, i++) {
                pos.x = acpos->x + gabi::fmadds((f32)ofpos->x, cosR, (f32)ofpos->z * sinR);
                pos.y = acpos->y;
                pos.z = acpos->z + gabi::fmsubs((f32)ofpos->z, cosR, (f32)ofpos->x * sinR);
                dTree_packet_c* tree = dComIfGp_getTree();
                tree->newData(&pos, 0, fopAcM_GetRoomNo(i_this));
            }
        }
    } else if (kind == 2 || kind == 3) {
        /* white flower, pink flower */
        if (dComIfGp_createFlower() != nullptr) {
            s8 item = daGrass_prm::getItemNo(i_this);
            if (item < 0x20 || item >= 0x40)
                item = (s8)0xFF;

            s8 flowerType = 0;
            if (kind == 2)
                flowerType = 1;
            else if (kind == 3)
                flowerType = 2;

            for (s32 i = 0; i < offset->num; ofpos++, i++) {
                cXyz pos;
                pos.x = acpos->x + (f32)ofpos->x;
                pos.y = acpos->y;
                pos.z = acpos->z + (f32)ofpos->z;
                dFlower_packet_c* flower = dComIfGp_getFlower();
                flower->newData(flowerType, &pos, fopAcM_GetRoomNo(i_this), item);
            }
        }
    }

    return cPhs_ERROR_e;
}

// HD: compiler-generated common-header initializer and deleting destructor.
/* 021622DC */
static void __sinit_d_a_grass_cpp() {
    __sinit_header_statics();
}

/* 02162370 */
static void grass_class_dt(grass_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* WWHD glue (tools/verify/autoglue.py):
data l_guard0 10464370
data l_setType0 104641D0
data l_guard1 10464374
data l_setType1 10464288
data l_guard2 10464378
data l_setType2 104641FC
data l_guard3 1046437C
data l_setType3 10464210
data l_guard4 10464380
data l_setType4 10464308
data l_guard5 10464384
data l_setType5 1046423C
data l_guard6 10464388
data l_setType6 10464268
data l_offsetData 101B5D7C
str 100106B0 "d_a_grass.cpp" renamed
str 10010670 "a_grass_type < (sizeof(l_offsetData) / sizeof(l_offsetData[0]))"
*/
