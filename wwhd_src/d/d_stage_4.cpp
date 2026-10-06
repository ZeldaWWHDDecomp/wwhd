/* d_stage: stage/room data (dStage_*), part 4 of 5 (025C2BC0..025C4734), WWHD. See d_stage.cpp for the unit.
 *
 * Loaders, create/delete, scene changes, room control, the room status constructor and the
 * unit's __sinit. HD: dStage_roomLoader registers a crash-dump context object (vtable 100553AC,
 * its virtual +0x14 is the HD dump 025C51C0) around the decode; the memory blocks of the
 * GameCube room control are gone; dStage_Create skips the sky boxes in GTower and Hyrule. */
#include "bindings.h"

namespace d_stage_4_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline void JUT_ASSERT_l(u32 file, s32 line, u32 msg) { gabi::call(0x0273AA24, file, line, msg); }
static inline s32 dComIfG_play_c_getLayerNo_l(s32 roomNo) { return gabi::call<s32>(0x025250C0, roomNo); } /* HD: static */
static inline u32 dComIfG_getStageRes_l(u32 arc, u32 res) { return gabi::call<u32>(0x0252447C, arc, res); }
static inline void dComIfG_deleteStageRes_l(u32 arc) { gabi::call(0x02524180, arc); }
static inline void dComIfG_deleteObjectRes_l(u32 arc) { gabi::call(0x02520488, arc); }
static inline void dumpCtx_ct_l(u32 obj) { gabi::call(0x02034434, obj); } /* HD crash-dump context: base constructor (8 bytes) */
static inline void dumpCtx_dt_l(u32 obj, s32 del) { gabi::call(0x02034538, obj, del); }
static inline void fopKyM_Create_l(s32 proc, u32 a1, u32 a2) { gabi::call(0x025DAD4C, proc, a1, a2); }
static inline void dSv_info_c_initZone_l(u32 info) { gabi::call(0x025B99DC, info); }
static inline void dSv_zoneBit_c_clearRoomSwitch_l(u32 zone) { gabi::call(0x025B93B0, zone); }
static inline void dSv_info_c_putSave_l(u32 info, s32 stageNo) { gabi::call(0x025B9D24, info, stageNo); }
static inline void dMap_c_create_l() { gabi::call(0x0258EFC8); }
static inline void dMap_c_remove_l() { gabi::call(0x02590FB0); }
static inline u32 fpcLy_CurrentLayer_l() { return gabi::call<u32>(0x025DED64); }
static inline u32 fpcSCtRq_Request_l(u32 layer, s32 proc, u32 a2, u32 a3, u32 prm) { return gabi::call<u32>(0x025E14A8, layer, proc, a2, a3, prm); }
static inline void dEvent_manager_c_create_l(u32 mng) { gabi::call(0x02544488, mng); }
static inline void dEvent_manager_c_remove_l(u32 mng) { gabi::call(0x025444F0, mng); }
/* this unit's functions in the other parts */
static inline s32 createRoomScene_l(s32 roomNo) { return gabi::call<s32>(0x025C1004, roomNo); }
static inline void setStayNo_l(s32 roomNo) { gabi::call(0x025C1070, roomNo); }
static inline u32 getStatusRoomDt_l(u32 self, s32 no) { return gabi::call<u32>(0x025C11DC, self, no); }
static inline void dStage_startStage_c_set_l(u32 self, u32 name, u32 roomNo, u32 point, u32 layer) { gabi::call(0x025C1328, self, name, roomNo, point, layer); }
static inline s32 dBgS_GetRoomId_l(u32 bgs, u32 poly) { return gabi::call<s32>(0x024EF130, bgs, poly); }
static inline u32 dBgS_GetGrpRoomInfId_l(u32 bgs, u32 poly) { return gabi::call<u32>(0x024EECE8, bgs, poly); }
static inline s32 dBgS_GetExitId_l(u32 bgs, u32 poly) { return gabi::call<s32>(0x024EECC8, bgs, poly); }
static inline u32 cBgS_GetActorPointer_l(u32 bgs, u32 bgIndex) { return gabi::call<u32>(0x02008438, bgs, bgIndex); } /* HD: takes the polygon's bg index */
/* dComIfGp_setNextStage(name, point, roomNo, layer, speed, mode, enable, wipe); the registers as the callers set them */
static inline void dComIfGp_setNextStage_l(u32 name, u32 point, u32 roomNo, u32 layer, f32 speed, u32 mode, u32 enable, u32 wipe) {
    gabi::call(0x0252012C, name, point, roomNo, layer, speed, mode, enable, wipe);
}
static inline BOOL dSv_event_c_isEventBit_l(u32 ev, u32 flag) { return gabi::call<BOOL>(0x025B8B94, ev, flag); }
static inline s32 dKy_getdaytime_hour_l() { return gabi::call<s32>(0x02556C34); }
static inline void dKy_DayProc_l() { gabi::call(0x02560768); }
static inline void dKy_set_nexttime_l(f32 t) { gabi::call(0x02560728, t); }
static inline u32 daPy_lk_c_getDayNightParamData_l(u32 pl) { return gabi::call<u32>(0x0243B188, pl); }
static inline void dSv_turnRestart_c_set_l(u32 self, u32 pos, u32 angle, u32 roomNo, u32 param, u32 shipPos, u32 shipAngle, u32 a7) {
    gabi::call(0x025B998C, self, pos, angle, roomNo, param, shipPos, shipAngle, a7);
}
static inline void dTimer_c_deleteRequest_l(u32 timer) { gabi::call(0x025C58D8, timer); }
static inline void cLib_chaseUC_l(u32 p, u32 target, u32 step) { gabi::call(0x0200F4FC, p, target, step); }
static inline void dKy_change_colset_l(u32 a, u32 b, f32 ratio) { gabi::call(0x0255FCEC, a, b, ratio); }
static inline u32 operator_new_l(u32 size) { return gabi::call<u32>(0x0273AD10, size); }
static inline void operator_delete_l(u32 p) { gabi::call(0x0273AF40, p); }
static inline void memclr_l(u32 p, u32 size) { gabi::call(0x028F521C, p, size); }
static inline void __register_global_object_l(u32 node) { gabi::call(0x028F026C, node); } /* HD: a prebuilt chain node */
static inline void ctor_028ED6F8_l(u32 obj) { gabi::call(0x028ED6F8, obj); }
static inline void ctor_028EAB2C_l(u32 obj) { gabi::call(0x028EAB2C, obj); }
static inline void __construct_array_l(u32 p, u32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st16(u32 a, u16 v) { gabi::store<u16>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline u32 vfn(u32 obj, u32 off) { return ld(ld(obj) + off); }
static inline u32 save_l() { return ld(0x101F84DC); }

static constexpr u32 mStayNo = 0x1047E6C8;
static constexpr u32 mOldStayNo = 0x1047E6C9;
static constexpr u32 mDarkRatio = 0x1047E6CA;
static constexpr u32 mStatus = 0x1047E6CC; /* [64], stride 0x22C: mRoomDt +0, flags +0x21C, mDraw +0x21D, zone count +0x21E, zone +0x21F, block id +0x220, mpBgW +0x228 */
static constexpr u32 mDemoArcName = 0x1047E6B8;
static constexpr u32 mRoomChangeCount = 0x1047E6C0; /* HD-only counter: reset by dStage_Create, counted by dStage_RoomCheck */
static constexpr u32 SafeString_vt = 0x10055394; /* this unit's sead::SafeString vtable copy */

struct sstr_l { be<u32> str, vt; };

/* inline sead::SafeString::isEqual(a, b) with both strings' assureTermination calls */
static bool sstr_equal(sstr_l* a, sstr_l* b) {
    gabi::call_ptr(ld((u32)a->vt + 0x14), gabi::ea(a));
    gabi::call_ptr(ld((u32)a->vt + 0x14), gabi::ea(a));
    u32 fn = ld((u32)b->vt + 0x14);
    u32 sa = a->str;
    gabi::call_ptr(fn, gabi::ea(b));
    u32 sb = b->str;
    if (sa == sb) return true;
    sb -= 1;
    for (u32 i = 0x40001; i != 0; i--) {
        u8 ca = ld8(sa);
        sb += 1;
        u8 cb = ld8(sb);
        if (ca != cb) return false;
        if (ca == 0) return true;
        sa++;
    }
    return false;
}

/* 025C2BC0: FuncTable entries are 0xC bytes (tag, -, function +8); file nodes 0xC (tag, num, offset) */
static void dStage_dt_c_decode(u32 i_data, u32 i_stage, u32 i_funcTbl, s32 i_tblSize) {
    WWHD_FUNC(0x025C2BC0, void, i_data, i_stage, i_funcTbl, i_tblSize);
    if (i_data == 0) return;
    u32 chunks = ld(i_data);
    if (i_tblSize <= 0) return;
    for (u32 n = (u32)i_tblSize; n != 0; n--, i_funcTbl += 0xC) {
        u32 node = i_data + 4;
        if (chunks == 0) continue;
        u32 tag = ld(i_funcTbl);
        for (u32 j = chunks; j != 0; j--, node += 0xC) {
            if (ld(node) == tag) {
                u32 fn = ld(i_funcTbl + 8);
                if (fn != 0) gabi::call_ptr(fn, i_stage, node, ld(node + 4), i_data);
                break;
            }
        }
    }
}
VERIFY(0x025C2BC0, dStage_dt_c_decode);

/* 025C2C70 */
static void dStage_dt_c_offsetToPtr(u32 i_data) {
    WWHD_FUNC(0x025C2C70, void, i_data);
    u32 n = ld(i_data);
    if (n == 0) return;
    u32 node = i_data + 4;
    for (; n != 0; n--, node += 0xC) {
        u32 off = ld(node + 8);
        if (off != 0) st(node + 8, i_data + off);
    }
}
VERIFY(0x025C2C70, dStage_dt_c_offsetToPtr);

/* 025C2CA4 */
static void dStage_dt_c_stageInitLoader(u32 i_data, u32 i_stage) {
    WWHD_FUNC(0x025C2CA4, void, i_data, i_stage);
    if (i_data == 0) JUT_ASSERT_l(0x10055974, 0x11CD, 0x10055980);
    if (i_stage == 0) JUT_ASSERT_l(0x10055974, 0x11CE, 0x10055990);
    dStage_dt_c_offsetToPtr(i_data);
    gabi::call_ptr(vfn(i_stage, 0xC), i_stage); /* init */
    dStage_dt_c_decode(i_data, i_stage, 0x101EBAD4, 1); /* STAG */
}
VERIFY(0x025C2CA4, dStage_dt_c_stageInitLoader);

/* 025C2D4C */
static void layerLoader(u32 i_data, u32 i_stage, s32 i_roomNo) {
    WWHD_FUNC(0x025C2D4C, void, i_data, i_stage, i_roomNo);
    s32 layer = dComIfG_play_c_getLayerNo_l(i_roomNo);
    dStage_dt_c_decode(i_data, i_stage, ld(0x101EBAE0 + layer * 4), 3); /* l_layerFuncTable_p */
}
VERIFY(0x025C2D4C, layerLoader);

/* 025C2DA8 */
static void dStage_dt_c_stageLoader(u32 i_data, u32 i_stage) {
    WWHD_FUNC(0x025C2DA8, void, i_data, i_stage);
    dStage_dt_c_decode(i_data, i_stage, 0x101EBCC0, 0x22);
    layerLoader(i_data, i_stage, -1);
}
VERIFY(0x025C2DA8, dStage_dt_c_stageLoader);

/* 025C2DFC: HD: a crash-dump context {base 8 bytes, vtable +4, file +8, stage +0xC} is
 * registered around the loading */
struct dumpCtx_l { be<u32> link, vt, data, stage; };
static void dStage_dt_c_roomLoader(u32 i_data, u32 i_stage) {
    WWHD_FUNC(0x025C2DFC, void, i_data, i_stage);
    gabi::Local<dumpCtx_l> ctx;
    dumpCtx_ct_l(gabi::ea(ctx.get()));
    ctx->data = i_data;
    ctx->vt = 0x100553AC;
    ctx->stage = i_stage;
    dStage_dt_c_offsetToPtr(i_data);
    gabi::call_ptr(vfn(i_stage, 0xC), i_stage); /* init */
    dStage_dt_c_decode(i_data, i_stage, 0x101EBE58, 0x16);
    dumpCtx_dt_l(gabi::ea(ctx.get()), 0);
}
VERIFY(0x025C2DFC, dStage_dt_c_roomLoader);

/* 025C2E8C */
static void dStage_dt_c_roomReLoader(u32 i_data, u32 i_stage, s32 param_2) {
    WWHD_FUNC(0x025C2E8C, void, i_data, i_stage, param_2);
    dStage_dt_c_decode(i_data, i_stage, 0x101EBF60, 7);
    layerLoader(i_data, i_stage, param_2);
}
VERIFY(0x025C2E8C, dStage_dt_c_roomReLoader);

/* 025C2EEC */
static void dStage_infoCreate() {
    WWHD_FUNC(0x025C2EEC, void);
    u32 res = dComIfG_getStageRes_l(0x100559A0, 0x100559B4); /* "Stage", "stage.dzs" */
    if (res == 0) JUT_ASSERT_l(0x100559A8, 0x1307, 0x100559C0);
    u32 p = dComIfGp_ea();
    dStage_dt_c_stageInitLoader(res, p + 0x5150);
}
VERIFY(0x025C2EEC, dStage_infoCreate);

/* 025C2F54 */
static void dStage_roomDt_c_init(u32 self) {
    WWHD_FUNC(0x025C2F54, void, self);
    st(self + 0x20, 0);
    st(self + 0x3C, 0);
    st(self + 0x34, 0);
    st(self + 0x44, 0);
    st(self + 0x4, 0);
    st(self + 0x18, 0);
    st(self + 0x10, 0);
    st(self + 0x2C, 0);
    st(self + 0x28, 0);
    st(self + 0xC, 0);
    st(self + 0x14, 0);
    st(self + 0x8, 0);
    st(self + 0x48, 0);
    st(self + 0x30, 0);
    st(self + 0x38, 0);
    st(self + 0x1C, 0);
    st(self + 0x24, 0);
    st(self + 0x40, 0);
}
VERIFY(0x025C2F54, dStage_roomDt_c_init);

/* 025C2FA4: HD: static members, `this` unused; no memory blocks */
static void dStage_roomControl_c_init(u32 self) {
    WWHD_FUNC(0x025C2FA4, void, self);
    st8(mOldStayNo, 0xFF);
    st8(mStayNo, 0xFF);
    if ((s16)ld16(dComIfGp_ea() + 0x513C) >= 0) dSv_info_c_initZone_l(save_l() + 0x20);
    u32 status = mStatus;
    for (u32 i = 0x40; i != 0; i--, status += 0x22C) {
        dStage_roomDt_c_init(status);
        st8(status + 0x21C, 0);
        st8(status + 0x21D, 0);
        if ((s16)ld16(dComIfGp_ea() + 0x513C) >= 0) {
            st(status + 0x228, 0);
            st8(status + 0x21F, 0xFF);
            st8(status + 0x220, 0xFF);
        } else {
            s8 zone = (s8)ld8(status + 0x21F);
            if (zone >= 0) dSv_zoneBit_c_clearRoomSwitch_l(save_l() + 0x7C8 + zone * 0x4C + 2);
            st8(status + 0x220, 0xFF);
            st(status + 0x228, 0);
        }
    }
    st8(mDarkRatio, 0xFF);
}
VERIFY(0x025C2FA4, dStage_roomControl_c_init);

/* 025C3094: dKankyo_create and dStage_roomInit inlined. HD: the sky boxes are created when
 * vr_sky.bmd exists, except in GTower and Hyrule; the HD room-change counter is reset */
static void dStage_Create() {
    WWHD_FUNC(0x025C3094, void);
    fopKyM_Create_l(0x13, 0, 0);
    fopKyM_Create_l(0x1DE, 0, 0);
    fopKyM_Create_l(0x1DF, 0, 0);
    fopKyM_Create_l(0x15, 0, 0);
    u32 res = dComIfG_getStageRes_l(0x100559D4, 0x100559F8); /* "Stage", "stage.dzs" */
    if (res == 0) JUT_ASSERT_l(0x100559EC, 0x1323, 0x10055A04);
    dStage_roomControl_c_init(dComIfGp_ea() + 0x51CC);
    dStage_dt_c_stageLoader(res, dComIfGp_ea() + 0x5150);
    if ((s8)ld8(dComIfGp_ea() + 0x513E) >= 0) {
        s32 roomNo = (s8)ld8(dComIfGp_ea() + 0x513E);
        dComIfGp_ea();
        st8(mStatus + roomNo * 0x22C + 0x21C, 2); /* setStatusFlag(roomNo, 2) */
        createRoomScene_l(roomNo);
        setStayNo_l(roomNo);
        st(mRoomChangeCount, 0);
    }
    dMap_c_create_l();
    st8(mDemoArcName, 0);
    bool create = false;
    if (dComIfG_getStageRes_l(0x100559D4, 0x10055A18) != 0) { /* "vr_sky.bmd" */
        gabi::Local<sstr_l> a;
        gabi::Local<sstr_l> b;
        a->vt = SafeString_vt;
        a->str = 0x100559DC; /* "GTower" */
        u32 p = dComIfGp_ea();
        b->vt = SafeString_vt;
        b->str = p + 0x5134;
        if (!sstr_equal(a, b)) {
            gabi::Local<sstr_l> c;
            gabi::Local<sstr_l> d;
            c->vt = SafeString_vt;
            c->str = 0x100559E4; /* "Hyrule" */
            u32 p2 = dComIfGp_ea();
            d->vt = SafeString_vt;
            d->str = p2 + 0x5134;
            if (!sstr_equal(c, d)) create = true;
        }
    }
    if (create) {
        fpcSCtRq_Request_l(fpcLy_CurrentLayer_l(), 0x1B5, 0, 0, 0); /* VRBOX */
        fpcSCtRq_Request_l(fpcLy_CurrentLayer_l(), 0x1B6, 0, 0, 0); /* VRBOX2 */
    }
    dEvent_manager_c_create_l(dComIfGp_ea() + 0x52C4);
}
VERIFY(0x025C3094, dStage_Create);

/* 025C3370: HD: no memory blocks to destroy */
static void dStage_Delete() {
    WWHD_FUNC(0x025C3370, void);
    if (ld8(mDemoArcName) != 0) dComIfG_deleteObjectRes_l(mDemoArcName);
    u32 stage = dComIfGp_ea() + 0x5150;
    u32 stag = gabi::call_ptr<u32>(vfn(stage, 0x15C), stage); /* getStagInfo */
    u32 stageNo = (ld8(stag + 9) >> 1) & 0x7F;
    dSv_info_c_putSave_l(save_l() + 0x20, stageNo);
    dComIfG_deleteStageRes_l(0x10055A24); /* "Stage" */
    dMap_c_remove_l();
    dEvent_manager_c_remove_l(dComIfGp_ea() + 0x52C4);
}
VERIFY(0x025C3370, dStage_Delete);


/* 025C33E8: dComIfGs_removeZone inlined */
static void dStage_roomControl_c_zoneCountCheck(u32 self, s32 i_roomNo) {
    WWHD_FUNC(0x025C33E8, void, self, i_roomNo);
    u32 status = mStatus;
    for (u32 i = 0x40; i != 0; i--, status += 0x22C) {
        s32 zone = (s8)ld8(status + 0x21F);
        if (zone < 0) continue;
        if ((s8)ld8(status + 0x21E) <= 0) continue;
        dSv_zoneBit_c_clearRoomSwitch_l(save_l() + 0x7C8 + zone * 0x4C + 2);
        if ((u32)i_roomNo == (u32)(s32)(s8)ld8(mOldStayNo)) continue;
        u8 cnt = (u8)(ld8(status + 0x21E) - 1);
        st8(status + 0x21E, cnt);
        if ((s8)cnt != 0) continue;
        s32 z = (s8)ld8(status + 0x21F);
        st8(save_l() + 0x7C8 + z * 0x4C, 0xFF);
        st8(status + 0x21F, 0xFF);
    }
    setStayNo_l(i_roomNo);
}
VERIFY(0x025C33E8, dStage_roomControl_c_zoneCountCheck);

/* 025C34AC: stayRoomCheck inlined. HD-changed: every room of the list that is not loaded yet
 * gets a room scene (the GameCube code returned after the first one) */
static BOOL dStage_roomControl_c_loadRoom(u32 self, s32 roomCount, u32 rooms) {
    WWHD_FUNC(0x025C34AC, BOOL, self, roomCount, rooms);
    for (u32 i = 0, f = mStatus + 0x21C; i < 0x40; i++, f += 0x22C)
        if (ld8(f) & 6) return FALSE;
    BOOL ok = TRUE;
    u32 f = mStatus + 0x21C;
    for (u32 roomNo = 0; roomNo < 0x40; roomNo++, f += 0x22C) {
        u8 flag = ld8(f);
        if (!(flag & 1)) continue;
        bool found = false;
        if (roomCount > 0) {
            for (u32 k = 0; k < (u32)roomCount; k++) {
                if (roomNo == (u32)(ld8(rooms + k) & 0x3F)) {
                    found = true;
                    break;
                }
            }
        }
        if (found) continue;
        st8(f, flag | 4);
        ok = FALSE;
    }
    if (!ok) return FALSE;
    if (roomCount <= 0) return TRUE;
    for (u32 k = 0; k < (u32)roomCount; k++) {
        u32 roomNo = ld8(rooms + k) & 0x3F;
        u32 status = mStatus + roomNo * 0x22C;
        u8 flag = ld8(status + 0x21C);
        st8(status + 0x21E, 2); /* setZoneCount */
        if (flag & 1) continue;
        if (createRoomScene_l((s32)roomNo) == 0) continue;
        u8 b = ld8(rooms + k);
        u8 cur = ld8(status + 0x21C);
        st8(status + 0x21C, cur | ((b & 0x80) ? 2 : 0xA));
    }
    return TRUE;
}
VERIFY(0x025C34AC, dStage_roomControl_c_loadRoom);

/* 025C35E8: HD: the time pass is the static 101EBFD8; a room change bumps the HD counter */
static s32 dStage_RoomCheck(u32 i_gndChk) {
    WWHD_FUNC(0x025C35E8, s32, i_gndChk);
    u32 p = dComIfGp_ea();
    s32 roomId = dBgS_GetRoomId_l(p + 0x12A0, i_gndChk ? i_gndChk + 0x14 : 0);
    if (roomId < 0) return 0;
    if ((u32)roomId != (u32)(s32)(s8)ld8(mStayNo)) {
        dStage_roomControl_c_zoneCountCheck(dComIfGp_ea() + 0x51CC, roomId);
        st(mRoomChangeCount, ld(mRoomChangeCount) + 1);
    }
    p = dComIfGp_ea();
    u32 grp = dBgS_GetGrpRoomInfId_l(p + 0x12A0, i_gndChk ? i_gndChk + 0x14 : 0);
    if (grp == 0xFF) return 0;
    u32 stage = dComIfGp_ea() + 0x5150;
    u32 room = gabi::call_ptr<u32>(vfn(stage, 0x64), stage); /* getRoom */
    if (room == 0 || (s32)ld(room) <= (s32)grp) return 1;
    u32 tp = ld8(ld(ld(room + 4) + grp * 4) + 2) & 3; /* dStage_roomRead_dt_c_GetTimePass */
    dComIfGp_ea();
    st8(0x101EBFD8, (u8)tp); /* setTimePass */
    u32 e = ld(ld(room + 4) + grp * 4);
    u32 list = ld(e + 4);
    u32 num = ld8(e);
    return dStage_roomControl_c_loadRoom(dComIfGp_ea() + 0x51CC, (s32)num, list);
}
VERIFY(0x025C35E8, dStage_RoomCheck);

/* 025C3748 */
static s32 dStage_changeScene(s32 i_exitId, f32 speed, u32 mode, s32 room_no) {
    WWHD_FUNC(0x025C3748, s32, i_exitId, speed, mode, room_no);
    u32 scls;
    if (room_no == -1) {
        u32 stage = dComIfGp_ea() + 0x5150;
        scls = gabi::call_ptr<u32>(vfn(stage, 0x16C), stage); /* getSclsInfo */
    } else {
        if ((u32)room_no >= 0x40) JUT_ASSERT_l(0x10055A2C, 0x1476, 0x10055A60);
        u32 rd = getStatusRoomDt_l(dComIfGp_ea() + 0x51CC, room_no);
        scls = gabi::call_ptr<u32>(vfn(rd, 0x16C), rd);
    }
    if (scls == 0) return 0;
    if (i_exitId < 0 || i_exitId >= (s32)ld(scls)) JUT_ASSERT_l(0x10055A2C, 0x1480, 0x10055A38);
    u32 e = ld(scls + 4) + i_exitId * 0xC;
    u32 wipe = ld8(e + 0xA) & 0xF; /* dStage_sclsInfo_getWipe */
    if (wipe == 0xF) wipe = 0;
    u32 room = (u32)(s32)(s8)ld8(e + 9);
    u32 start = ld8(e + 8);
    dComIfGp_setNextStage_l(e, start, room, (u32)-1, speed, mode, 1, wipe);
    return 1;
}
VERIFY(0x025C3748, dStage_changeScene);

/* 025C38B8: IkadaGetRoomNoArg0/IkadaGetLinkIdArg1/IkadaGetIkadaIdArg2 and the stage name
 * compare inlined */
static s32 dStage_changeSceneExitId(u32 i_poly, f32 i_speed, u32 i_mode, s32 i_roomNo) {
    WWHD_FUNC(0x025C38B8, s32, i_poly, i_speed, i_mode, i_roomNo);
    u32 p = dComIfGp_ea();
    s32 exit_id = dBgS_GetExitId_l(p + 0x12A0, i_poly);
    if (exit_id == 0x3E || exit_id == 0x3B) {
        p = dComIfGp_ea();
        u32 actor = cBgS_GetActorPointer_l(p + 0x12A0, ld16(i_poly + 2));
        u32 prm = ld(actor + 0xB0);
        u32 point = (prm >> 10) & 0xFF;
        u32 roomNo = (u32)(s32)(s8)((prm >> 4) & 0x3F);
        if (exit_id == 0x3E) {
            dComIfGp_setNextStage_l(0x10055A94, point, roomNo, (u32)-1, i_speed, i_mode, 1, 0); /* "Obshop" */
        } else if (exit_id == 0x3B) {
            dComIfGp_setNextStage_l(0x10055A9C, point, roomNo, (u32)-1, i_speed, i_mode, 1, 0); /* "Abship" */
        }
        st8(dComIfGp_ea() + 0x5D16, (u8)i_roomNo); /* IkadaShipBeforeRoomId */
        u32 id = (ld(actor + 0xB0) >> 18) & 0xFF;
        st8(dComIfGp_ea() + 0x5D17, (u8)id); /* IkadaShipId */
        p = dComIfGp_ea();
        st(p + 0x5D18, ld(actor + 0x314)); /* IkadaShipBeforePos = current.pos */
        st(p + 0x5D1C, ld(actor + 0x318));
        st(p + 0x5D20, ld(actor + 0x31C));
        return 1;
    }
    if (exit_id == 0x3D) {
        dComIfGp_ea();
        p = dComIfGp_ea();
        if (ld8(p + 0x5D16) >= 0x40) JUT_ASSERT_l(0x10055AAC, 0x143B, 0x10055AB8);
        p = dComIfGp_ea();
        u32 room = (u32)(s32)(s8)ld8(p + 0x5D16);
        dComIfGp_setNextStage_l(0x10055A88, (u32)-2, room, (u32)-1, i_speed, i_mode, 1, 0); /* "sea" */
        return 1;
    }
    if (exit_id == 0x3C) {
        gabi::Local<sstr_l> a;
        gabi::Local<sstr_l> b;
        a->str = 0x10055A8C; /* "Asoko" */
        a->vt = SafeString_vt;
        p = dComIfGp_ea();
        b->vt = SafeString_vt;
        b->str = p + 0x5134;
        if (sstr_equal(a, b)) {
            if (!dSv_event_c_isEventBit_l(save_l() + 0x644, 0x808)) {
                dComIfGp_setNextStage_l(0x10055A80, 0, 0, (u32)-1, i_speed, i_mode, 1, 0); /* "A_umikz" */
            } else if (dSv_event_c_isEventBit_l(save_l() + 0x644, 0x520)) {
                dComIfGp_setNextStage_l(0x10055A88, 5, 0xB, (u32)-1, i_speed, i_mode, 1, 0); /* "sea", Windfall */
            } else {
                dComIfGp_setNextStage_l(0x10055AA4, 0x12, 0, (u32)-1, i_speed, i_mode, 1, 0); /* "MajyuE" */
            }
        } else {
            dComIfGp_setNextStage_l(0x10055A8C, 0, 0, (u32)-1, i_speed, i_mode, 1, 0); /* "Asoko" */
        }
        return 1;
    }
    return dStage_changeScene(exit_id, i_speed, i_mode, i_roomNo);
}
VERIFY(0x025C38B8, dStage_changeSceneExitId);

/* 025C3D68 */
static void dStage_restartRoom(u32 roomParam, u32 mode) {
    WWHD_FUNC(0x025C3D68, void, roomParam, mode);
    u32 p = dComIfGp_ea();
    u32 room = (u32)(s32)(s8)ld8(save_l() + 0x1148); /* dComIfGs_getRestartRoomNo */
    dComIfGp_setNextStage_l(p + 0x5134, (u32)-1, room, (u32)-1, gabi::load<f32>(0x1005592C), mode, 0, 0);
    st(save_l() + 0x116C, roomParam); /* dComIfGs_setRestartRoomParam */
}
VERIFY(0x025C3D68, dStage_restartRoom);

/* 025C3DE4: HD: the day counts up for hours 6..23 (an unsigned range check) */
static void dStage_turnRestart() {
    WWHD_FUNC(0x025C3DE4, void);
    s32 layerNo = (s8)ld8(dComIfGp_ea() + 0x513F);
    if (layerNo >= 0) layerNo = (s8)(layerNo ^ 1);
    u32 p = dComIfGp_ea();
    f32 zero = gabi::load<f32>(0x1005592C);
    u32 room = (u32)(s32)(s8)ld8(save_l() + 0x128A); /* dComIfGs_getTurnRestartRoomNo */
    dComIfGp_setNextStage_l(p + 0x5134, (u32)-3, room, (u32)layerNo, zero, 0, 0, 6);
    s32 hour = dKy_getdaytime_hour_l();
    u32 h6 = (u32)(hour - 6);
    f32 nextTime = gabi::load<f32>(0x10055B10); /* 180.0 */
    if (h6 < 12) nextTime = zero;
    if (h6 < 18) {
        u32 s = save_l();
        st16(s + 0x48, (u16)(ld16(s + 0x48) + 1)); /* dComIfGs_setDate(getDate() + 1) */
        dKy_DayProc_l();
    }
    dKy_set_nexttime_l(nextTime);
}
VERIFY(0x025C3DE4, dStage_turnRestart);

/* 025C3ED0: dComIfG_TimerDeleteRequest inlined */
static void dStage_escapeRestart() {
    WWHD_FUNC(0x025C3ED0, void);
    u32 player = ld(dComIfGp_ea() + 0x5B34);
    u32 angle = (u32)(s32)(s16)ld16(player + 0x32A);
    u32 pos = player + 0x314;
    u32 room = (u32)(s32)(s8)ld8(player + 0x326);
    u32 dn = daPy_lk_c_getDayNightParamData_l(player);
    dSv_turnRestart_c_set_l(save_l() + 0x1278, pos, angle, room, dn, pos, angle, 0);
    if (ld(dComIfGp_ea() + 0x5CFC) == 3) {
        u32 timer = ld(dComIfGp_ea() + 0x5CF0);
        if (timer != 0) dTimer_c_deleteRequest_l(timer);
    }
    u32 p = dComIfGp_ea();
    u32 r = (u32)(s32)(s8)ld8(save_l() + 0x128A);
    dComIfGp_setNextStage_l(p + 0x5134, (u32)-3, r, (u32)-1, gabi::load<f32>(0x1005592C), 0, 0, 9);
}
VERIFY(0x025C3ED0, dStage_escapeRestart);

/* 025C3FAC: dStage_nextStage_c::dStage_nextStage_c() (0xE bytes) */
static u32 dStage_nextStage_c_ct(u32 self) {
    WWHD_FUNC(0x025C3FAC, u32, self);
    if (self == 0) {
        self = operator_new_l(0xE);
        if (self == 0) return 0;
    }
    st8(self + 0xD, 0); /* wipe */
    st8(self + 0xC, 0); /* enable */
    return self;
}
VERIFY(0x025C3FAC, dStage_nextStage_c_ct);

/* 025C3FEC: dStage_nextStage_c::set(name, roomNo, point, layer, wipe) */
static s32 dStage_nextStage_c_set(u32 self, u32 i_name, u32 i_roomNo, u32 i_point, u32 i_layer, u32 i_wipe) {
    WWHD_FUNC(0x025C3FEC, s32, self, i_name, i_roomNo, i_point, i_layer, i_wipe);
    if ((s8)ld8(self + 0xC) != 0) return 0;
    st8(self + 0xD, (u8)i_wipe);
    st8(self + 0xC, 1);
    dStage_startStage_c_set_l(self, i_name, i_roomNo, i_point, i_layer);
    return 1;
}
VERIFY(0x025C3FEC, dStage_nextStage_c_set);

/* 025C4048: dStage_roomDt_c::dStage_roomDt_c() (HD 0x54 bytes, vtable 10055B90) */
static u32 dStage_roomDt_c_ct(u32 self) {
    WWHD_FUNC(0x025C4048, u32, self);
    if (self == 0) {
        self = operator_new_l(0x54);
        if (self == 0) return 0;
    }
    st(self + 0, 0x10055B90);
    for (u32 o = 4; o <= 0x50; o += 4)
        if (o != 0x4C) st(self + o, 0);
    st8(self + 0x4C, 0);
    st16(self + 0x4E, 0);
    return self;
}
VERIFY(0x025C4048, dStage_roomDt_c_ct);

/* 025C40E0: dStage_roomDt_c::getMapInfo2(int) (unnamed by the matcher; vtable slot +0x7C) */
static u32 dStage_roomDt_c_getMapInfo2(u32 self, u32 i_no) {
    WWHD_FUNC(0x025C40E0, u32, self, i_no);
    u32 base = gabi::call_ptr<u32>(vfn(self, 0x8C), self); /* getMapInfoBase */
    if (base == 0) return 0;
    u32 n = ld(base);
    if (n == 0) return 0;
    u32 e = ld(base + 4);
    if (e == 0) return 0;
    if ((s32)n <= 0) return 0;
    for (; n != 0; n--, e += 0x38)
        if (i_no == ld8(e + 0x35)) return e;
    return 0;
}
VERIFY(0x025C40E0, dStage_roomDt_c_getMapInfo2);

/* 025C415C: dStage_roomStatus_c::dStage_roomStatus_c() (0x22C bytes): the room data, then three
 * copies of a 0x44-byte light template (1016E414, probably a light influence) */
static u32 dStage_roomStatus_c_ct(u32 self) {
    WWHD_FUNC(0x025C415C, u32, self);
    if (self == 0) {
        self = operator_new_l(0x22C);
        if (self == 0) return 0;
    }
    dStage_roomDt_c_ct(self);
    memclr_l(self + 0x54, 0x1C8);
    const u32 t = 0x1016E414;
    static const u32 dst[3] = {0x54, 0x114, 0x198};
    for (u32 d : dst) {
        for (u32 o = 0; o < 0x18; o += 4) gabi::store<f32>(self + d + o, gabi::load<f32>(t + o));
        for (u32 o = 0x18; o < 0x1C; o++) st8(self + d + o, ld8(t + o));
        for (u32 o = 0x1C; o < 0x24; o += 2) st16(self + d + o, ld16(t + o));
        for (u32 o = 0x24; o < 0x44; o += 4) gabi::store<f32>(self + d + o, gabi::load<f32>(t + o));
    }
    st8(self + 0x21C, 0);
    st8(self + 0x21D, 0);
    st8(self + 0x21E, 0);
    st8(self + 0x21F, 0);
    st8(self + 0x220, 0);
    st(self + 0x224, 0);
    st(self + 0x228, 0);
    return self;
}
VERIFY(0x025C415C, dStage_roomStatus_c_ct);

/* 025C4334 */
static BOOL dStage_roomControl_c_checkRoomDisp(u32 self, s32 i_roomNo) {
    WWHD_FUNC(0x025C4334, BOOL, self, i_roomNo);
    u8 f = ld8(mStatus + 0x21C + i_roomNo * 0x22C);
    if (f & 8) return FALSE;
    return (f & 0x10) ? TRUE : FALSE;
}
VERIFY(0x025C4334, dStage_roomControl_c_checkRoomDisp);

/* 025C4364: HD-changed: no spot models around the player in dark rooms (only the colour-set
 * blend remains); the dark statuses are the static table 101EE5C8 (8 entries of 0x20) */
static void dStage_roomControl_c_checkDrawArea(u32 self) {
    WWHD_FUNC(0x025C4364, void, self);
    if ((s8)ld8(mStayNo) < 0) return;
    if (ld(dComIfGp_ea() + 0x5B2C) == 0) return; /* dComIfGp_getPlayer(0) */
    s32 stay = (s8)ld8(mStayNo);
    u32 plist = ld(mStatus + stay * 0x22C + 0x18); /* mpFileList */
    u32 target = 0xFF;
    if (plist == 0) return;
    u32 w = ld(plist);
    u32 dark = (w >> 3) & 0xF; /* dStage_FileList_dt_DarkNo */
    if (dark >= 8) {
        JUT_ASSERT_l(0x10055B28, 0x971, 0x10055B34);
        w = ld(plist);
    }
    u32 darkOn = w & 1;
    if (darkOn) target = ld8(0x101EE5C8 + dark * 0x20); /* getEnvAlpha */
    cLib_chaseUC_l(mDarkRatio, target, 8);
    u32 r = ld8(mDarkRatio);
    if (r == 0xFF) return;
    f32 ratio = (f32)r * gabi::load<f32>(0x10055B20); /* 1/128 */
    f32 one = gabi::load<f32>(0x10055B24);
    if (!(ratio <= one)) ratio = one;
    if (darkOn) {
        if (r != target) return;
        dKy_change_colset_l(0, 4, one - ratio);
    } else {
        dKy_change_colset_l(4, 0, ratio);
    }
}
VERIFY(0x025C4364, dStage_roomControl_c_checkDrawArea);

/* 025C44AC: dStage_roomControl_c::getDarkStatus() (unnamed by the matcher) */
static u32 dStage_roomControl_c_getDarkStatus() {
    WWHD_FUNC(0x025C44AC, u32);
    s32 stay = (s8)ld8(mStayNo);
    u32 plist = ld(mStatus + stay * 0x22C + 0x18);
    if (plist == 0) return 0;
    u32 idx = (ld(plist) >> 3) & 0xF;
    if (idx >= 8) JUT_ASSERT_l(0x10055B58, 0x971, 0x10055B64);
    return 0x101EE5C8 + idx * 0x20;
}
VERIFY(0x025C44AC, dStage_roomControl_c_getDarkStatus);

/* 025C4548: dStage_stageDt_c::dStage_stageDt_c() (HD 0x7C bytes, vtable 10055DE0) */
static u32 dStage_stageDt_c_ct(u32 self) {
    WWHD_FUNC(0x025C4548, u32, self);
    if (self == 0) {
        self = operator_new_l(0x7C);
        if (self == 0) return 0;
    }
    st(self + 0, 0x10055DE0);
    for (u32 o = 4; o <= 0x78; o += 4)
        if (o != 0x44) st(self + o, 0);
    st16(self + 0x44, 0);
    return self;
}
VERIFY(0x025C4548, dStage_stageDt_c_ct);

/* 025C4604: an empty deleting destructor emitted in this unit (called from 02524808 for the play
 * object's member at +0x3EB0) */
static void dtor_025C4604(u32 self, u32 del) {
    WWHD_FUNC(0x025C4604, void, self, del);
    if (self == 0) return;
    if (!(del & 1)) return;
    operator_delete_l(self);
}
VERIFY(0x025C4604, dtor_025C4604);

/* 025C4618: dStage_stageDt_c::init() (vtable slot +0xC) */
static void dStage_stageDt_c_init(u32 self) {
    WWHD_FUNC(0x025C4618, void, self);
    for (u32 o = 4; o <= 0x78; o += 4)
        if (o != 0x30 && o != 0x34 && o != 0x38 && o != 0x3C && o != 0x44) st(self + o, 0);
}
VERIFY(0x025C4618, dStage_stageDt_c_init);

/* 025C4684: __sinit_d_stage_cpp */
static void __sinit_d_stage_cpp() {
    WWHD_FUNC(0x025C4684, void);
    st(0x1047D9A0 + 8, 0);
    st(0x1047D9A0 + 0, 0);
    st(0x1047D9A0 + 0xC, 0);
    st(0x1047D9A0 + 4, 0);
    __register_global_object_l(0x101EBFB4);
    gabi::store<f32>(0x1047D994, gabi::load<f32>(0x10055B88)); /* -pi */
    gabi::store<f32>(0x1047D998, gabi::load<f32>(0x10055B8C)); /* pi */
    ctor_028ED6F8_l(0x1047D99C);
    __register_global_object_l(0x101EBFC0);
    ctor_028EAB2C_l(0x1047D99D);
    __register_global_object_l(0x101EBFCC);
    __construct_array_l(mStatus, 0x40, 0x22C, 0x025C415C); /* dStage_roomControl_c::mStatus */
}
VERIFY(0x025C4684, __sinit_d_stage_cpp);

} // namespace d_stage_4_cpp
