/* d_stage: stage/room data (dStage_*), part 2 of 5 (025C1384..025C1E44), WWHD. See d_stage.cpp for the unit.
 *
 * dStage_playerInit (with dStage_decodeSearchIkada / dStage_playerInitIkada inlined) and the
 * first chunk handlers. The handlers call the dStage_dt_c virtuals: the HD vtable has one
 * 8-byte entry per virtual ({adjust, function}, the function at vtbl + 12 + 8*k, k = GameCube
 * vt index - 2); the memory block virtuals (set/getMemoryConfig, set/getMemoryMap) are gone. */
#include "bindings.h"

namespace d_stage_2_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void JUT_ASSERT_l(u32 file, s32 line, u32 msg) { gabi::call(0x0273AA24, file, line, msg); }
static inline u32 fopAcM_CreateAppend_l() { return gabi::call<u32>(0x025D5600); }
static inline u32 fopScnM_SearchByID_l(u32 id) { return gabi::call<u32>(0x025DC80C, id); }
static inline u32 fopAcM_create_l(s32 proc, u32 prm, u32 pos, s32 room, u32 angle, u32 scale, s32 subtype, u32 cb) {
    return gabi::call<u32>(0x025D5834, proc, prm, pos, room, angle, scale, subtype, cb);
}
static inline u32 fpcLy_CurrentLayer_l() { return gabi::call<u32>(0x025DED64); }
static inline u32 fpcSCtRq_Request_l(u32 layer, s32 proc, u32 a2, u32 a3, u32 prm) { return gabi::call<u32>(0x025E14A8, layer, proc, a2, a3, prm); }
static inline void PSMTXTrans_l(u32 m, f32 x, f32 y, f32 z) { gabi::call(0x028E93CC, m, x, y, z); }
static inline void mDoMtx_YrotM_l(u32 m, s16 y) { gabi::call(0x025F1C28, m, y); }
static inline void PSMTXMultVec_l(u32 m, u32 in, u32 out) { gabi::call(0x028E8F64, m, in, out); }
static inline void daSea_execute_l(u32 pos) { gabi::call(0x0246C5C8, pos); }
static inline BOOL daSea_ChkArea_l(f32 x, f32 z) { return gabi::call<BOOL>(0x0246B6A4, x, z); }
static inline f32 daSea_calcWave_l(f32 x, f32 z) { return gabi::call<f32>(0x0246BA0C, x, z); }
static inline u32 cMl_memalignB_l(s32 align, u32 size) { return gabi::call<u32>(0x02019430, align, size); }
static inline u32 fopCamM_Create_l(s32 idx, s32 proc, u32 prm) { return gabi::call<u32>(0x025DA654, idx, proc, prm); }
/* this unit's functions */
static inline void dStage_startStage_c_set_l(u32 self, u32 name, u32 roomNo, s32 point, s32 layer) { gabi::call(0x025C1328, self, name, roomNo, point, layer); }
static inline void dStage_actorCreate_l(u32 data, u32 prm) { gabi::call(0x025C1154, data, prm); }

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline f32 ldf(u32 a) { return gabi::load<f32>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st16(u32 a, u16 v) { gabi::store<u16>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }
static inline u32 vfn(u32 obj, u32 off) { return ld(ld(obj) + off); }
static inline u32 save_l() { return ld(0x101F84DC); }

struct cXyz_l { be<f32> x, y, z; };

static constexpr u32 mDoMtx_stack_now = 0x1048D0CC;

/* inline strcmp(a, b) == 0 */
static bool streq(u32 a, u32 b) {
    for (;;) {
        u8 ca = ld8(a), cb = ld8(b);
        if (ca != cb) return false;
        if (ca == 0) return true;
        a++;
        b++;
    }
}

/* the common end of dStage_playerInit's four start modes */
static s32 playerInit_tail(u32 appen, u32 player_data) {
    st(save_l() + 0x116C, 0); /* dComIfGs_setRestartRoomParam(0) */
    st8(appen + 0x21, 0xFF);
    st16(appen + 0x16, 0xFFFF);
    u32 stage = dComIfGp_ea() + 0x5134;
    u32 name = dComIfGp_ea() + 0x5134;
    s32 point = (s16)ld16(dComIfGp_ea() + 0x513C);
    u32 p = dComIfGp_ea();
    s32 layer = (s8)ld8(p + 0x513F);
    dStage_startStage_c_set_l(stage, name, ld(appen) & 0x3F, point, layer);
    u32 shipId = (ld16(appen + 0x14) >> 8) & 0xFF;
    st8(dComIfGp_ea() + 0x5D14, (u8)shipId);
    u32 shipRoom = ld(appen) & 0x3F;
    st8(dComIfGp_ea() + 0x5D15, (u8)shipRoom);
    dStage_actorCreate_l(player_data, appen);
    u32 proc = fopScnM_SearchByID_l(ld(0x1047E6C4)); /* dStage_roomControl_c::mProcID */
    if (proc == 0) {
        JUT_ASSERT_l(0x1005593C, 0x7BA, 0x10055958);
        fopAcM_create_l(0x1BC, 0, 0, -1, 0, 0, -1, 0); /* TITLE */
    } else if ((s16)ld16(proc + 8) != 7) {
        fopAcM_create_l(0x1BC, 0, 0, -1, 0, 0, -1, 0);
    }
    /* fopMsgM_Create(METER): HD: always, and no AGB */
    u32 layer2 = fpcLy_CurrentLayer_l();
    fpcSCtRq_Request_l(layer2, 0x1E1, 0, 0, 0);
    return 1;
}

/* 025C1384: HD-changed: the player list count comes from the chunk (not the argument); an
 * unknown start point falls back to the first entry; the meter is always created and the
 * GBA (AGB) actor is gone */
static s32 dStage_playerInit(u32 i_stage, u32 i_data, s32 num, u32 i_file) {
    WWHD_FUNC(0x025C1384, s32, i_stage, i_data, num, i_file);
    (void)num;
    if (ld(dComIfGp_ea() + 0x5B2C) != 0) return 1;
    u32 player = i_data + 4;
    gabi::call_ptr(vfn(i_stage, 0x3C), i_stage, player); /* setPlayer */
    u32 fn = vfn(i_stage, 0x4C);
    u32 n = ld(i_data + 4);
    gabi::call_ptr(fn, i_stage, n & 0xFFFF); /* setPlayerNum */
    u32 appen = fopAcM_CreateAppend_l();
    if (appen == 0) JUT_ASSERT_l(0x1005593C, 0x746, 0x10055948);
    u32 p = dComIfGp_ea();
    s32 point = (s16)ld16(p + 0x513C);
    u32 player_data = ld(i_data + 8);
    u32 save = save_l();
    u32 roomParam = ld(save + 0x116C);
    if (point == -2) {
        /* dStage_playerInitIkada / dStage_decodeSearchIkada */
        u32 shipId = ld8(dComIfGp_ea() + 0x5D17);
        u32 ikada = 0;
        bool found = false;
        if (i_file != 0) {
            s32 cnt = (s32)ld(i_file);
            for (u32 i = 0; i < 13 && !found; i++) {
                u32 tag = ld(0x10055880 + i * 8); /* "ACTR", "ACT0".."ACTb" */
                u32 node = i_file + 4;
                for (s32 j = 0; j < cnt; j++, node += 0xC) {
                    if (ld(node) != tag) continue;
                    s32 en = (s32)ld(node + 4);
                    u32 a = ld(node + 8);
                    for (s32 k = 0; k < en; k++, a += 0x20) {
                        if (streq(a, 0x10055318) || streq(a, 0x10055320) || streq(a, 0x1005538C)) {
                            if (shipId == ((ld(a + 8) >> 18) & 0xFF)) {
                                ikada = a;
                                found = true;
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        }
        if (!found) JUT_ASSERT_l(0x100553F4, 0x6A3, 0x1005544C);
        u32 roomNo = ld8(dComIfGp_ea() + 0x5D16);
        st(appen, (roomNo | 0xFF000000) | 0x80);
        PSMTXTrans_l(mDoMtx_stack_now, ldf(ikada + 0xC), ldf(ikada + 0x10), ldf(ikada + 0x14));
        mDoMtx_YrotM_l(mDoMtx_stack_now, (s16)ld16(ikada + 0x1A));
        gabi::Local<cXyz_l> offset;
        gabi::Local<cXyz_l> pos;
        offset->x = 0.0f;
        offset->y = 87.0f;
        offset->z = 550.0f;
        PSMTXMultVec_l(mDoMtx_stack_now, gabi::ea(offset.get()), gabi::ea(pos.get()));
        daSea_execute_l(gabi::ea(pos.get()));
        f32 h = 0.0f;
        if (daSea_ChkArea_l(ldf(ikada + 0xC), ldf(ikada + 0x14))) h = daSea_calcWave_l(ldf(ikada + 0xC), ldf(ikada + 0x14));
        f32 y = pos->y;
        y = y + h;
        y = y + 500.0f;
        f32 x = pos->x;
        pos->y = y;
        stf(appen + 4, x);
        stf(appen + 8, y);
        stf(appen + 0xC, (f32)pos->z);
        u16 az = (u16)(ld8(ikada + 0x19) << 8);
        u16 ay = ld16(ikada + 0x1A);
        st16(appen + 0x10, 0);
        st16(appen + 0x14, az);
        st16(appen + 0x12, ay);
        return playerInit_tail(appen, player_data);
    }
    if (point == -3) {
        st(appen, ld(save + 0x1284)); /* turn restart param */
        u32 s = save_l();
        st(appen + 4, ld(s + 0x1278));
        st(appen + 8, ld(s + 0x127C));
        st(appen + 0xC, ld(s + 0x1280));
        s = save_l();
        u16 ay = ld16(s + 0x1288);
        st16(appen + 0x10, 0);
        st16(appen + 0x12, ay);
        st16(appen + 0x14, 0xFF00);
        return playerInit_tail(appen, player_data);
    }
    if (point == -1) {
        st(appen, roomParam);
        u32 s = save_l();
        st(appen + 4, ld(s + 0x1160));
        st(appen + 8, ld(s + 0x1164));
        st(appen + 0xC, ld(s + 0x1168));
        s = save_l();
        u16 ay = ld16(s + 0x115E);
        st16(appen + 0x14, 0xFF00);
        st16(appen + 0x12, ay);
        st16(appen + 0x10, 0);
        return playerInit_tail(appen, player_data);
    }
    u32 e = player_data;
    u32 i = 0;
    if ((s32)n > 0) {
        for (u32 c = n; c != 0; c--) {
            if ((u32)ld8(e + 0x1D) == (u32)point) break;
            e += 0x20;
            i++;
        }
    }
    if (i == n) e = player_data; /* HD: not found -> the first entry (GameCube: assert) */
    st(appen + 0, ld(e + 8));
    st(appen + 4, ld(e + 0xC));
    st(appen + 8, ld(e + 0x10));
    st(appen + 0xC, ld(e + 0x14));
    st(appen + 0x10, ld(e + 0x18));
    st(appen + 0x14, ld(e + 0x1C));
    if (roomParam != 0) {
        u32 prm = ld(appen);
        if (((prm >> 12) & 0xF) != 2) st(appen, (roomParam & ~0x3Fu) | (prm & 0x3F));
    }
    return playerInit_tail(appen, e);
}
VERIFY(0x025C1384, dStage_playerInit);

/* 025C1AD0: dStage_cameraInit, dStage_cameraCreate inlined (matcher: dStage_cameraCreate) */
static s32 dStage_cameraInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C1AD0, s32, i_stage, i_data, i_num, i_file);
    gabi::call_ptr(vfn(i_stage, 0x1C), i_stage, i_data + 4); /* setCamera */
    u32 prm = cMl_memalignB_l(-4, 0x18);
    if (prm != 0) {
        st(prm, 0);
        stf(prm + 4, 0.0f);
        stf(prm + 8, 0.0f);
        fopCamM_Create_l(0, 0x1DC, prm);
    }
    return 1;
}
VERIFY(0x025C1AD0, dStage_cameraInit);

/* 025C1B38 */
static s32 dStage_RoomCameraInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C1B38, s32, i_stage, i_data, i_num, i_file);
    gabi::call_ptr(vfn(i_stage, 0x1C), i_stage, i_data + 4);
    return 1;
}
VERIFY(0x025C1B38, dStage_RoomCameraInit);

/* 025C1B6C */
static s32 dStage_arrowInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C1B6C, s32, i_stage, i_data, i_num, i_file);
    gabi::call_ptr(vfn(i_stage, 0x2C), i_stage, i_data + 4); /* setArrow */
    return 1;
}
VERIFY(0x025C1B6C, dStage_arrowInit);

/* 025C1BA0 */
static s32 dStage_mapInfo_GetOceanX(u32 i_mapInfo) {
    WWHD_FUNC(0x025C1BA0, s32, i_mapInfo);
    s32 rt = ld8(i_mapInfo + 0x36) & 0xF;
    if (rt & 8) return rt - 0x10;
    return rt;
}
VERIFY(0x025C1BA0, dStage_mapInfo_GetOceanX);

/* 025C1BB8 */
static s32 dStage_mapInfo_GetOceanZ(u32 i_mapInfo) {
    WWHD_FUNC(0x025C1BB8, s32, i_mapInfo);
    s32 rt = (ld8(i_mapInfo + 0x36) >> 4) & 0xF;
    if (rt & 8) return rt - 0x10;
    return rt;
}
VERIFY(0x025C1BB8, dStage_mapInfo_GetOceanZ);

/* 025C1BD4 */
static s32 dStage_mapInfoInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C1BD4, s32, i_stage, i_data, i_num, i_file);
    u32 fn = vfn(i_stage, 0x6C);
    gabi::call_ptr(fn, i_stage, ld(i_data + 8)); /* setMapInfo(m_entries) */
    gabi::call_ptr(vfn(i_stage, 0x84), i_stage, i_data + 4); /* setMapInfoBase */
    return 1;
}
VERIFY(0x025C1BD4, dStage_mapInfoInit);

/* 025C1C38 */
static s32 dStage_paletInfoInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C1C38, s32, i_stage, i_data, i_num, i_file);
    u32 fn = vfn(i_stage, 0x94);
    gabi::call_ptr(fn, i_stage, ld(i_data + 8));
    return 1;
}
VERIFY(0x025C1C38, dStage_paletInfoInit);

/* 025C1C6C */
static s32 dStage_pselectInfoInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C1C6C, s32, i_stage, i_data, i_num, i_file);
    u32 fn = vfn(i_stage, 0xA4);
    gabi::call_ptr(fn, i_stage, ld(i_data + 8));
    return 1;
}
VERIFY(0x025C1C6C, dStage_pselectInfoInit);

/* 025C1CA0 */
static s32 dStage_envrInfoInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C1CA0, s32, i_stage, i_data, i_num, i_file);
    u32 fn = vfn(i_stage, 0xB4);
    gabi::call_ptr(fn, i_stage, ld(i_data + 8));
    return 1;
}
VERIFY(0x025C1CA0, dStage_envrInfoInit);

/* 025C1CD4 */
static s32 dStage_filiInfoInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C1CD4, s32, i_stage, i_data, i_num, i_file);
    u32 fn = vfn(i_stage, 0x1D4); /* setFileListInfo */
    if (i_num == 0) gabi::call_ptr(fn, i_stage, 0u);
    else gabi::call_ptr(fn, i_stage, ld(i_data + 8));
    return 1;
}
VERIFY(0x025C1CD4, dStage_filiInfoInit);

/* 025C1D24 */
static s32 dStage_vrboxInfoInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C1D24, s32, i_stage, i_data, i_num, i_file);
    u32 fn = vfn(i_stage, 0xC4);
    gabi::call_ptr(fn, i_stage, ld(i_data + 8));
    return 1;
}
VERIFY(0x025C1D24, dStage_vrboxInfoInit);

/* 025C1D58 */
static s32 dStage_plightInfoInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C1D58, s32, i_stage, i_data, i_num, i_file);
    u32 fn = vfn(i_stage, 0xD4);
    gabi::call_ptr(fn, i_stage, ld(i_data + 8)); /* setPlightInfo */
    gabi::call_ptr(vfn(i_stage, 0x124), i_stage, (u32)i_num); /* setPlightNumInfo */
    return 1;
}
VERIFY(0x025C1D58, dStage_plightInfoInit);

/* 025C1DBC */
static s32 dStage_lgtvInfoInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C1DBC, s32, i_stage, i_data, i_num, i_file);
    gabi::call_ptr(vfn(i_stage, 0x144), i_stage, (u32)i_num); /* setLightVecInfoNum */
    u32 fn = vfn(i_stage, 0x134); /* setLightVecInfo */
    if (i_num == 0) gabi::call_ptr(fn, i_stage, 0u);
    else gabi::call_ptr(fn, i_stage, ld(i_data + 8));
    return 1;
}
VERIFY(0x025C1DBC, dStage_lgtvInfoInit);

} // namespace d_stage_2_cpp
