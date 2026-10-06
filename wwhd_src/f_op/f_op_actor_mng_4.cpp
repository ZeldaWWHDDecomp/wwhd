/* f_op_actor_mng part 4: 025D9000..025DA2F4 — steal items, item balls, warp flower, enemy search,
 * disappear, ground angle, carrying, sound check, name search, water height, GBA name, relative
 * positions, the unit's __sinit and its per-TU inline destructors. WWHD. See f_op_actor_mng.cpp for the unit's range.
 *
 * GameCube fopAcM_createItemFromEnemyTable and fopAcM_isItemForIb are inlined into their HD
 * callers; fopAcM_viewCutoffCheck and fpoAcM_relativePos have no HD copy in this unit. */
#include "bindings.h"

namespace f_op_actor_mng_4_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void JUT_ASSERT_l(u32 file, s32 line, u32 msg) { gabi::call(0x0273AA24, file, line, msg); }
static inline u32 fopAcM_create_l(u32 name, u32 prm, u32 pos, u32 room, u32 angle, u32 scale, u32 arg, u32 fn) {
    return gabi::call<u32>(0x025D5834, name, prm, pos, room, angle, scale, arg, fn);
}
static inline u32 fopAcM_fastCreate_l(u32 name, u32 prm, u32 pos, u32 room, u32 angle, u32 scale, u32 arg, u32 fn, u32 data) {
    return gabi::call<u32>(0x025D5928, name, prm, pos, room, angle, scale, arg, fn, data);
}
static inline u32 fopAcM_fastCreateItem_l(u32 pos, u32 itemNo, u32 room, u32 angle, u32 scale, f32 sF, f32 sY, f32 g, u32 bit, u32 fn) {
    return gabi::call<u32>(0x025D8AB0, pos, itemNo, room, angle, scale, sF, sY, g, bit, fn);
}
static inline u32 fopAcM_createItemFromTable_l(u32 pos, u32 tbl, u32 bit, u32 room, u32 type, u32 angle, u32 action, u32 scale) {
    return gabi::call<u32>(0x025D8120, pos, tbl, bit, room, type, angle, action, scale);
}
static inline u32 getEmonoItemFromLifeBallTable_l(u32 t) { return gabi::call<u32>(0x025512A4, t); }
static inline u32 getItemFromLifeBallTableWithoutEmono_l(u32 t) { return gabi::call<u32>(0x02551684, t); }
static inline BOOL isLimitedItem_l(u32 i) { return gabi::call<BOOL>(0x0255106C, i); }
static inline BOOL isNonSavedEmono_l(u32 i) { return gabi::call<BOOL>(0x02551080, i); }
static inline u32 getItemNoByLife_l(u32 i) { return gabi::call<u32>(0x02551108, i); }
static inline BOOL dSv_memBit_isSwitch_l(u32 p, u32 bit, s32 room) { return gabi::call<BOOL>(0x025B8DC8, p, bit, room); }
static inline BOOL dSv_info_isItem_l(u32 p, u32 bit, s32 room) { return gabi::call<BOOL>(0x025BA494, p, bit, room); }
static inline s32 cDT_GetInf_l(u32 tbl, u32 field, u32 idx) { return gabi::call<s32>(0x0200EA74, tbl, field, idx); }
static inline f32 cM_rndF_l(f32 x) { return gabi::call<f32>(0x020198D8, x); }
static inline f32 cM_rndFX_l(f32 x) { return gabi::call<f32>(0x02019918, x); }
static inline void daIball_remove_old_l() { gabi::call(0x025261C0); }
static inline BOOL fopAc_IsActor_l(u32 p) { return gabi::call<BOOL>(0x025D4604, p); }
static inline u32 fopScnM_SearchByID_l(u32 id) { return gabi::call<u32>(0x025DC80C, id); }
static inline BOOL fpcBs_Is_JustOfType_l(s32 a, s32 b) { return gabi::call<BOOL>(0x025DD258, a, b); }
static inline u32 fopAcIt_Judge_l(u32 fn, u32 data) { return gabi::call<u32>(0x025D5218, fn, data); }
static inline u32 fpcM_JudgeInLayer_l(u32 layer, u32 fn, u32 data) { return gabi::call<u32>(0x025DFB28, layer, fn, data); }
static inline void cBgS_GndChk_ct_l(u32 p) { gabi::call(0x02008E0C, p); }
static inline void cBgS_Chk_dt_l(u32 p, s32 f) { gabi::call(0x02008DAC, p, f); }
static inline f32 cBgS_GroundCross_l(u32 bgs, u32 chk) { return gabi::call<f32>(0x02008974, bgs, chk); }
static inline s16 cM_atan2s_l(f32 y, f32 x) { return gabi::call<s16>(0x020195B0, y, x); }
static inline void cLib_addCalcAngleS_l(u32 p, u32 target, s32 scale, s32 maxStep, s32 minStep) {
    gabi::call(0x0200F378, p, target, scale, maxStep, minStep);
}
static inline void fopAcM_setStageLayer_l(u32 a) { gabi::call(0x025D537C, a); }
static inline void fopAcM_setRoomLayer_l(u32 a, s32 room) { gabi::call(0x025D5418, a, room); }
static inline u32 dKy_Sound_get_l() { return gabi::call<u32>(0x0255F530); }
static inline void cXyz_mi_l(u32 self, u32 out, u32 other) { gabi::call(0x0201ADE0, self, out, other); }
static inline f32 PSVECSquareMag_l(u32 v) { return gabi::call<f32>(0x028E8DD0, v); }
static inline f32 sqrtf_l(f32 x) { return gabi::call<f32>(0x028F4384, x); }
static inline u32 dStage_searchName_l(u32 name) { return gabi::call<u32>(0x025C109C, name); }
static inline void dBgS_WtrChk_ct_l(u32 p) { gabi::call(0x024F22DC, p); }
static inline void __register_global_object_l(u32 d) { gabi::call(0x028F026C, d); }
static inline BOOL dBgS_SplGrpChk_l(u32 bgs, u32 chk) { return gabi::call<BOOL>(0x024EF7C0, bgs, chk); }
static inline BOOL daSea_ChkArea_l(f32 x, f32 z) { return gabi::call<BOOL>(0x0246B6A4, x, z); }
static inline f32 daSea_calcWave_l(f32 x, f32 z) { return gabi::call<f32>(0x0246BA0C, x, z); }
static inline BOOL dComIfGs_checkGetItem_l(u32 i) { return gabi::call<BOOL>(0x02520C0C, i); }
static inline void operator_delete_l(u32 p) { gabi::call(0x0273AF40, p); }

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline f32 ldf(u32 a) { return gabi::load<f32>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st16(u32 a, u16 v) { gabi::store<u16>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }

static constexpr u32 SAVE = 0x101F84DC;
static constexpr u32 sincosTable = 0x104A44F8;
static constexpr u32 stealItem_CB = 0x025D8FD0;

struct sstr_l { be<u32> str, vt; };
struct cXyz_l { be<f32> x, y, z; };
struct items_l { be<s32> v[16]; };
struct prm_l { be<u32> name, mask, param; };
struct u32_l { be<u32> v; };
/* dBgS_GndChk on the stack (0x54 bytes from its start) */
struct gndchk_l { be<u32> w[0x54 / 4]; };

/* fopAcM_isItemForIb (inline): whether the steal/ball item bit was already collected */
static inline BOOL isItemForIb(u32 bit, u32 itemNo, u32 room) {
    u32 save = ld(SAVE);
    if (itemNo == 0x4B) return dSv_memBit_isSwitch_l(save + 0x598, bit, (s32)(s8)room);
    return dSv_info_isItem_l(save + 0x20, bit, (s32)(s8)room);
}
static inline bool no_bit(u32 bit) { return (s32)bit == 0x1F || (s32)bit == -1 || (s32)bit == 0xFF; }

/* 025D9000 */
static u32 fopAcM_createStealItem(u32 p_pos, u32 i_tblNo, u32 i_roomNo, u32 p_angle, u32 i_itemBitNo) {
    WWHD_FUNC(0x025D9000, u32, p_pos, i_tblNo, i_roomNo, p_angle, i_itemBitNo);
    u32 itemNo = getEmonoItemFromLifeBallTable_l(i_tblNo & 0xFFFF);
    u32 bit = i_itemBitNo;
    if (isLimitedItem_l(itemNo)) {
        if (no_bit(bit) || isItemForIb(bit, itemNo, i_roomNo)) {
            u32 n = getItemFromLifeBallTableWithoutEmono_l(i_tblNo & 0xFFFF);
            return fopAcM_fastCreateItem_l(p_pos, n, i_roomNo, p_angle, 0, 0.0f, 0.0f, -6.0f, (u32)-1, stealItem_CB);
        }
    } else if (isNonSavedEmono_l(itemNo)) {
        if (bit != 0) itemNo = getItemFromLifeBallTableWithoutEmono_l(i_tblNo & 0xFFFF);
        bit = (u32)-1;
    } else {
        if (itemNo == 0xFF) itemNo = getItemFromLifeBallTableWithoutEmono_l(i_tblNo & 0xFFFF);
        bit = (u32)-1;
    }
    return fopAcM_fastCreateItem_l(p_pos, itemNo, i_roomNo, p_angle, 0, 0.0f, 0.0f, -6.0f, bit, stealItem_CB);
}
VERIFY(0x025D9000, fopAcM_createStealItem);

/* strcmp(dComIfGp_getStartStageName(), lit) == 0 through two sead::SafeString temporaries */
static bool stage_is(u32 lit) {
    gabi::Local<sstr_l> a, b;
    a->str = lit;
    a->vt = 0x1005716C;
    u32 play = dComIfGp_ea();
    b->vt = 0x1005716C;
    b->str = play + 0x5134;
    gabi::call_ptr(ld(a->vt + 0x14), gabi::ea(a.get()));
    gabi::call_ptr(ld(a->vt + 0x14), gabi::ea(a.get()));
    u32 s1 = a->str;
    gabi::call_ptr(ld(b->vt + 0x14), gabi::ea(b.get()));
    u32 s2 = b->str;
    if (s1 == s2) return true;
    for (u32 n = 0; n < 0x40001; n++, s1++, s2++) {
        u8 c = ld8(s1);
        if (c != ld8(s2)) return false;
        if (c == 0) return true;
    }
    return false;
}

/* 025D9154: HD: item tables 0x1D-0x1F, 0x24, 0x2B drop from item table 0x29 (action 0xD);
 * GameCube fopAcM_createItemFromEnemyTable is inlined */
static u32 fopAcM_createIball(u32 p_pos, u32 itemTableIdx, u32 i_roomNo, u32 p_angle, u32 i_itemBitNo) {
    WWHD_FUNC(0x025D9154, u32, p_pos, itemTableIdx, i_roomNo, p_angle, i_itemBitNo);
    u32 charTbl = dComIfGp_ea() + 0x50A0;
    s32 dropChance = cDT_GetInf_l(charTbl, ld(dComIfGp_ea() + 0x510C), itemTableIdx & 0xFFFF); /* GetPercent */
    s32 randPercent = gabi::ftoi(cM_rndF_l(99.999f));
    if (stage_is(0x100578BC) || stage_is(0x100578C4) || stage_is(0x100578CC)) /* Savage Labyrinth */
        return (u32)-1;
    if (itemTableIdx >= 0x1D && (itemTableIdx <= 0x1F || itemTableIdx == 0x24 || itemTableIdx == 0x2B))
        return fopAcM_createItemFromTable_l(p_pos, 0x29, i_itemBitNo, i_roomNo, 0, p_angle, 0xD, 0);
    if (dropChance > randPercent) {
        u32 params = (itemTableIdx & 0xFFFF) | ((i_itemBitNo << 16) & 0xFF0000);
        daIball_remove_old_l();
        u32 item = fopAcM_fastCreate_l(0x18F, params, p_pos, i_roomNo, 0, 0, (u32)-1, 0, 0);
        return item != 0 ? ld(item + 4) : (u32)-1;
    }
    /* fopAcM_createItemFromEnemyTable */
    s32 itemIdx = gabi::ftoi(cM_rndF_l(15.999f));
    gabi::Local<cXyz_l> scale;
    scale->x = ldf(0x101FFBA8);
    scale->y = ldf(0x101FFBAC);
    scale->z = ldf(0x101FFBB0);
    gabi::Local<items_l> items;
    for (int i = 0; i < 16; i++) {
        u32 tbl = dComIfGp_ea() + 0x50A0;
        u32 field = ld(dComIfGp_ea() + 0x50A0 + 0x2C + 4 * i); /* GetNITEM0..15 */
        items->v[i] = cDT_GetInf_l(tbl, field, itemTableIdx & 0xFFFF);
    }
    u32 itemNo = ld(gabi::ea(items.get()) + (u32)(itemIdx << 2)) & 0xFF;
    u32 bit = i_itemBitNo;
    if (isLimitedItem_l(itemNo)) {
        if (no_bit(bit) || isItemForIb(bit, itemNo, i_roomNo)) {
            u32 n = getItemNoByLife_l(3); /* YELLOW_RUPEE */
            f32 sF = cM_rndFX_l(5.0f);
            f32 sY = gabi::fadds_ppc(cM_rndFX_l(10.0f), 50.0f);
            u32 item = fopAcM_fastCreateItem_l(p_pos, n, i_roomNo, 0, gabi::ea(scale.get()), sF, sY, -6.0f, (u32)-1, 0);
            return item != 0 ? ld(item + 4) : (u32)-1;
        }
    } else {
        if (isNonSavedEmono_l(itemNo) && bit != 0) itemNo = getItemFromLifeBallTableWithoutEmono_l(itemTableIdx & 0xFFFF) & 0xFF;
        bit = (u32)-1;
    }
    u32 n = getItemNoByLife_l(itemNo);
    f32 sF = cM_rndFX_l(5.0f);
    f32 sY = gabi::fadds_ppc(cM_rndFX_l(10.0f), 50.0f);
    u32 item = fopAcM_fastCreateItem_l(p_pos, n, i_roomNo, 0, gabi::ea(scale.get()), sF, sY, -6.0f, bit, 0);
    return item != 0 ? ld(item + 4) : (u32)-1;
}
VERIFY(0x025D9154, fopAcM_createIball);

/* 025D9874 */
static u32 fopAcM_createWarpFlower(u32 p_pos, u32 p_angle, u32 i_roomNo, u32 param_4) {
    WWHD_FUNC(0x025D9874, u32, p_pos, p_angle, i_roomNo, param_4);
    return fopAcM_create_l(0x68, param_4 & 0x0FFFFFFF, p_pos, i_roomNo, p_angle, 0, (u32)-1, 0);
}
VERIFY(0x025D9874, fopAcM_createWarpFlower);

/* 025D9898 */
static u32 enemySearchJugge(u32 ptr, u32 unused) {
    WWHD_FUNC(0x025D9898, u32, ptr, unused);
    if (ptr != 0 && fopAc_IsActor_l(ptr) && ld8(ptr + 0x2DA) == 2) return ptr;
    return 0;
}
VERIFY(0x025D9898, enemySearchJugge);

/* 025D98E8: HD: getGrabActorID is a virtual of the player (vtable +0xBC) */
static u32 fopAcM_myRoomSearchEnemy(u32 roomNo) {
    WWHD_FUNC(0x025D98E8, u32, roomNo);
    if ((s32)roomNo < 0) JUT_ASSERT_l(0x100578F0, 0x1065, 0x100578E4);
    u32 roomProc = fopScnM_SearchByID_l(ld(0x1047E8F0 + roomNo * 0x22C));
    if (roomProc == 0) JUT_ASSERT_l(0x100578F0, 0x1068, 0x100578D4);
    u32 player = ld(dComIfGp_ea() + 0x5B2C);
    u32 grab = gabi::call_ptr<u32>(ld(ld(player + 0xB4) + 0xBC), player);
    gabi::Local<u32_l> id;
    id->v = grab;
    if ((s32)grab != -1) {
        u32 enemy = fopAcIt_Judge_l(0x025E1234, gabi::ea(id.get()));
        if (enemy != 0 && ld8(enemy + 0x2DA) == 2) return enemy;
    }
    u32 layer = (u32)-1;
    if (fpcBs_Is_JustOfType_l(ld(0x101F3D60), ld(roomProc + 0xB8))) layer = ld(roomProc + 0xCC);
    return fpcM_JudgeInLayer_l(layer, 0x025D9898, 0);
}
VERIFY(0x025D98E8, fopAcM_myRoomSearchEnemy);

/* 025D99E8: HD: the parameter bytes are not masked */
static u32 fopAcM_createDisappear(u32 i_actor, u32 p_pos, u32 i_scale, u32 i_dropType, u32 i_itemBitNo) {
    WWHD_FUNC(0x025D99E8, u32, i_actor, p_pos, i_scale, i_dropType, i_itemBitNo);
    u32 params = (i_itemBitNo << 16) | (i_scale << 8) | i_dropType;
    u32 d = fopAcM_fastCreate_l(0x190, params, p_pos, (u32)(s32)(s8)ld8(i_actor + 0x326), i_actor + 0x320, 0, (u32)-1, 0, 0);
    if (d != 0) st(d + 0x3A4, ld(i_actor + 0x3A4)); /* itemTableIdx */
    return d != 0 ? ld(d + 4) : (u32)-1;
}
VERIFY(0x025D99E8, fopAcM_createDisappear);

/* 025D9A70 */
static BOOL fopAcM_getGroundAngle(u32 actor, u32 p_angle) {
    WWHD_FUNC(0x025D9A70, BOOL, actor, p_angle);
    gabi::Local<gndchk_l> chk; /* dBgS_GndChk */
    u32 g = gabi::ea(chk.get());
    cBgS_GndChk_ct_l(g);
    f32 y = gabi::fadds_ppc(ldf(actor + 0x318), 50.0f);
    f32 z = ldf(actor + 0x31C);
    st(g + 0x00, g + 0x40);       /* poly pass check */
    st8(g + 0x46, 0);
    st(g + 0x10, 0x100571A4);
    st8(g + 0x48, 0);
    stf(g + 0x2C, z);
    stf(g + 0x28, y);
    st8(g + 0x47, 0);
    st(g + 0x04, g + 0x4C);       /* group pass check */
    st8(g + 0x4A, 0);
    st8(g + 0x49, 0);
    st(g + 0x40, 0x100571D4);
    st8(g + 0x45, 0);
    st(g + 0x20, 0x100571B4);
    f32 x = ldf(actor + 0x314);
    st(g + 0x4C, 0x100571C4);
    stf(g + 0x24, x);
    st8(g + 0x44, 0);
    st(g + 0x50, 1);
    BOOL ret = TRUE;
    u32 bgs = dComIfGp_ea() + 0x12A0;
    f32 gy = cBgS_GroundCross_l(bgs, g);
    const f32 NONE = -1000000000.0f;
    s16 angleX = 0, angleZ = 0;
    bool done = false;
    if (gy == NONE) {
        ret = FALSE;
        done = true;
    } else {
        f32 y1 = gabi::fadds_ppc(gy, 50.0f);
        f32 z1 = gabi::fadds_ppc(z, 10.0f);
        stf(g + 0x24, x);
        stf(g + 0x28, y1);
        stf(g + 0x2C, z1);
        f32 g2 = cBgS_GroundCross_l(dComIfGp_ea() + 0x12A0, g);
        f32 x1;
        if (g2 != NONE) {
            angleX = (s16)-cM_atan2s_l(gabi::fsubs_ppc(g2, gy), gabi::fsubs_ppc(z1, z));
            x1 = gabi::fadds_ppc(x, 10.0f);
        } else {
            x1 = gabi::fadds_ppc(x, 10.0f);
            ret = FALSE;
        }
        stf(g + 0x28, y1);
        stf(g + 0x2C, z);
        stf(g + 0x24, x1);
        f32 g3 = cBgS_GroundCross_l(dComIfGp_ea() + 0x12A0, g);
        if (g3 == NONE) {
            ret = FALSE;
            done = true;
        } else {
            angleZ = cM_atan2s_l(gabi::fsubs_ppc(g3, gy), gabi::fsubs_ppc(x1, x));
        }
    }
    if (!done && ret) {
        cLib_addCalcAngleS_l(p_angle, (u32)(s32)angleX, 4, 0x200, 0x80);
        cLib_addCalcAngleS_l(p_angle + 4, (u32)(s32)angleZ, 4, 0x200, 0x80);
    }
    /* ~dBgS_GndChk */
    st(g + 0x20, 0x100571B4);
    st(g + 0x40, 0x100571D4);
    st(g + 0x4C, 0x10057194);
    cBgS_Chk_dt_l(g, 0);
    return done ? FALSE : ret;
}
VERIFY(0x025D9A70, fopAcM_getGroundAngle);

/* 025D9D0C */
static void fopAcM_setCarryNow(u32 i_this, u32 stageLayer) {
    WWHD_FUNC(0x025D9D0C, void, i_this, stageLayer);
    st(i_this + 0x2E0, ld(i_this + 0x2E0) | 0x2000); /* fopAcStts_CARRY */
    if (stageLayer != 0) fopAcM_setStageLayer_l(i_this);
}
VERIFY(0x025D9D0C, fopAcM_setCarryNow);

/* 025D9D24 */
static void fopAcM_cancelCarryNow(u32 i_this) {
    WWHD_FUNC(0x025D9D24, void, i_this);
    u32 s = ld(i_this + 0x2E0);
    if (s & 0x2000) {
        s8 room = (s8)ld8(i_this + 0x326);
        st(i_this + 0x2E0, s & ~0x2000u);
        fopAcM_setRoomLayer_l(i_this, room);
        st16(i_this + 0x32C, 0);
        if (ld8(dComIfGp_ea() + 0x5292) != 0 && ld8(i_this + 0x2DA) != 2)
            st(i_this + 0x2E0, ld(i_this + 0x2E0) | 0x800);
    }
}
VERIFY(0x025D9D24, fopAcM_cancelCarryNow);

/* 025D9DA0: HD: null-safe actor id */
static s32 fopAcM_otoCheck(u32 actor, f32 param_2) {
    WWHD_FUNC(0x025D9DA0, s32, actor, param_2);
    u32 sound = dKy_Sound_get_l();
    u32 id = ld(sound + 0x14);
    if ((s32)id == -1) return 0;
    if (actor != 0 && id == ld(actor + 4)) return 0;
    gabi::Local<cXyz_l> delta;
    cXyz_mi_l(sound, gabi::ea(delta.get()), actor + 0x314);
    f32 d = sqrtf_l(PSVECSquareMag_l(gabi::ea(delta.get())));
    if (!(d < param_2)) return 0;
    return (s32)ld(sound + 0xC);
}
VERIFY(0x025D9DA0, fopAcM_otoCheck);

/* 025D9E64 */
static u32 fopAcM_findObjectCB(u32 it, u32 i_prm) {
    WWHD_FUNC(0x025D9E64, u32, it, i_prm);
    if (i_prm == 0) JUT_ASSERT_l(0x10057910, 0x125D, 0x1005790C);
    u32 inf = dStage_searchName_l(ld(i_prm + 0));
    if (inf == 0) return 0;
    s32 procname = (s16)ld16(inf + 8);
    s32 name = it != 0 ? (s16)ld16(it + 0xE) : 0x7FFF;
    if (procname != name) return 0;
    if ((s8)ld8(inf + 0xA) != (s8)ld8(it + 0x2DD)) return 0;
    u32 mask = ld(i_prm + 4);
    if (mask != 0 && (ld(it + 0xB0) & mask) != ld(i_prm + 8)) return 0;
    return it;
}
VERIFY(0x025D9E64, fopAcM_findObjectCB);

/* 025D9F38 */
static u32 fopAcM_searchFromName(u32 pProcName, u32 paramMask, u32 parameter) {
    WWHD_FUNC(0x025D9F38, u32, pProcName, paramMask, parameter);
    gabi::Local<prm_l> prm;
    prm->name = pProcName;
    prm->mask = paramMask;
    prm->param = parameter;
    return fopAcIt_Judge_l(0x025D9E64, gabi::ea(prm.get()));
}
VERIFY(0x025D9F38, fopAcM_searchFromName);

/* 025D9F70: HD: dBgS::SplGrpChk is the water check */
static BOOL fopAcM_getWaterY(u32 pPos, u32 pDstWaterY) {
    WWHD_FUNC(0x025D9F70, BOOL, pPos, pDstWaterY);
    const u32 wc = 0x104874B8; /* static dBgS_WtrChk water_check */
    if (ld(0x1048750C) == 0) {
        st(0x1048750C, 1);
        dBgS_WtrChk_ct_l(wc);
        __register_global_object_l(0x101F32C0);
    }
    stf(pDstWaterY, -1000000000.0f);
    f32 y = gabi::fsubs_ppc(ldf(pPos + 4), 500.0f);
    f32 x = ldf(pPos + 0);
    f32 z = ldf(pPos + 8);
    stf(wc + 0x38, x);
    stf(wc + 0x3C, y);
    stf(wc + 0x40, z);
    stf(wc + 0x44, gabi::fadds_ppc(y, 1000.0f));
    BOOL ret = FALSE;
    if (dBgS_SplGrpChk_l(dComIfGp_ea() + 0x12A0, wc)) {
        stf(pDstWaterY, ldf(wc + 0x48));
        ret = TRUE;
    }
    if (daSea_ChkArea_l(ldf(pPos + 0), ldf(pPos + 8))) {
        f32 wave = daSea_calcWave_l(ldf(pPos + 0), ldf(pPos + 8));
        if (wave > ldf(pDstWaterY)) stf(pDstWaterY, wave);
        ret = TRUE;
    }
    return ret;
}
VERIFY(0x025D9F70, fopAcM_getWaterY);

/* 025DA088 */
static void fopAcM_setGbaName(u32 i_this, u32 itemNo, u32 gbaName0, u32 gbaName1) {
    WWHD_FUNC(0x025DA088, void, i_this, itemNo, gbaName0, gbaName1);
    bool got;
    if (dComIfGs_checkGetItem_l(itemNo)) got = true;
    else if (itemNo == 0x27) got = dComIfGs_checkGetItem_l(0x35) || dComIfGs_checkGetItem_l(0x36); /* BOW: magic/light arrows */
    else if (itemNo == 0x35) got = dComIfGs_checkGetItem_l(0x36) != 0;
    else got = false;
    st8(i_this + 0x2DE, (u8)(got ? gbaName1 : gbaName0));
}
VERIFY(0x025DA088, fopAcM_setGbaName);

/* 025DA124 */
static void fpoAcM_absolutePos(u32 i_this, u32 relPos, u32 absPos) {
    WWHD_FUNC(0x025DA124, void, i_this, relPos, absPos);
    f32 px = ldf(i_this + 0x314);
    stf(absPos + 0, px);
    f32 py = ldf(i_this + 0x318);
    stf(absPos + 4, py);
    f32 pz = ldf(i_this + 0x31C);
    stf(absPos + 8, pz);
    u32 t = sincosTable + ((u32)(ld16(i_this + 0x32A) >> 3) << 3);
    f32 rx = ldf(relPos + 0);
    f32 c = ldf(t + 4);
    f32 rz = ldf(relPos + 8);
    f32 s = ldf(t);
    stf(absPos + 0, gabi::fadds_ppc(px, gabi::fmadds(s, rz, gabi::fmuls_ppc(c, rx))));
    stf(absPos + 4, gabi::fadds_ppc(py, ldf(relPos + 4)));
    t = sincosTable + ((u32)(ld16(i_this + 0x32A) >> 3) << 3);
    s = ldf(t);
    rx = ldf(relPos + 0);
    rz = ldf(relPos + 8);
    c = ldf(t + 4);
    stf(absPos + 8, gabi::fadds_ppc(pz, gabi::fmsubs(c, rz, gabi::fmuls_ppc(s, rx))));
}
VERIFY(0x025DA124, fpoAcM_absolutePos);

/* 025DA1B4: the unit's __sinit (header statics; its own float pair and objects at 10487478..) */
static void __sinit_f_op_actor_mng_cpp() {
    WWHD_FUNC(0x025DA1B4, void, (u32)0);
    for (int i = 0; i < 4; i++) st(0x10487484 + 4 * i, 0);
    __register_global_object_l(0x101F32CC);
    stf(0x10487478, -3.1415927f);
    stf(0x1048747C, 3.1415927f);
    gabi::call(0x028ED6F8, 0x10487481);
    __register_global_object_l(0x101F32D8);
    gabi::call(0x028EAB2C, 0x10487482);
    __register_global_object_l(0x101F32E4);
    __register_global_object_l(0x101F32F0);
}
VERIFY(0x025DA1B4, __sinit_f_op_actor_mng_cpp);

/* 025DA254: a per-TU inline deleting destructor without members */
static void inline_dt_025DA254(u32 self, u32 flags) {
    WWHD_FUNC(0x025DA254, void, self, flags);
    if (self == 0) return;
    if (!(flags & 1)) return;
    operator_delete_l(self);
}
VERIFY(0x025DA254, inline_dt_025DA254);

/* 025DA268: dBgS_WtrChk::~dBgS_WtrChk (this unit's copy) */
static void dBgS_WtrChk_dt(u32 self, u32 flags) {
    WWHD_FUNC(0x025DA268, void, self, flags);
    if (self == 0) return;
    st(self + 0x20, 0x100571E4);
    st(self + 0x24, 0x10057204);
    st(self + 0x30, 0x10057194);
    gabi::call(0x02008B4C, self + 0x10, 0);
    if (flags & 1) operator_delete_l(self);
}
VERIFY(0x025DA268, dBgS_WtrChk_dt);

/* 025DA2E0: a per-TU inline deleting destructor without members */
static void inline_dt_025DA2E0(u32 self, u32 flags) {
    WWHD_FUNC(0x025DA2E0, void, self, flags);
    if (self == 0) return;
    if (!(flags & 1)) return;
    operator_delete_l(self);
}
VERIFY(0x025DA2E0, inline_dt_025DA2E0);

/* 025DA2F4: an empty per-TU inline (called on SafeString temporaries by fopAcM_entrySolidHeap) */
static void inline_empty_025DA2F4(u32 self) {
    WWHD_FUNC(0x025DA2F4, void, self);
    (void)self;
}
VERIFY(0x025DA2F4, inline_empty_025DA2F4);

} // namespace f_op_actor_mng_4_cpp
