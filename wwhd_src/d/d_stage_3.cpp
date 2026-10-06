/* d_stage: stage/room data (dStage_*), part 3 of 5 (025C1E44..025C2BC0), WWHD. See d_stage.cpp for the unit.
 *
 * Chunk handlers from STAG to Door. HD: the MEMA/MECO handlers (memory blocks) are empty stubs
 * that return 1 (025C474C/025C4754, part 5);
 * dStage_chkTaura and the sead::SafeString compares of the stage name are inlined;
 * dStage_KeepTresureInfoProc / dStage_KeepDoorInfoProc are inlined into the stage handlers
 * (TresureInfo 1047D9B0: num + 0x20 entries of 0x20; DoorInfo 1047DDB4: num + 0x40 entries
 * of 0x24). */
#include "bindings.h"

namespace d_stage_3_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline u32 fopAcM_CreateAppend_l() { return gabi::call<u32>(0x025D5600); }
static inline void dSv_info_c_getSave_l(u32 info, s32 stageNo) { gabi::call(0x025B9C9C, info, stageNo); }
static inline void dSv_danBit_c_init_l(u32 dan, s32 stageNo) { gabi::call(0x025B9174, dan, stageNo); }
static inline BOOL dSv_info_c_isActor_l(u32 info, u32 setId, u32 roomNo) { return gabi::call<BOOL>(0x025BA6A4, info, setId, roomNo); }
static inline BOOL dSv_event_c_isEventBit_l(u32 ev, u32 flag) { return gabi::call<BOOL>(0x025B8B94, ev, flag); }
static inline u32 fopAcIt_Judge_l(u32 judge, u32 data) { return gabi::call<u32>(0x025D5218, judge, data); }
static inline void daShip_c_initStartPos_l(u32 ship, u32 pos, s32 angle) { gabi::call(0x024832E0, ship, pos, angle); }
static inline u32 dComIfGp_getShip_l(s32 roomNo, s32 id) { return gabi::call<u32>(0x02520584, roomNo, id); } /* HD: out of line */
/* this unit's functions */
static inline void dStage_actorCreate_l(u32 data, u32 prm) { gabi::call(0x025C1154, data, prm); }

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u16 ld16(u32 a) { return gabi::load<u16>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st16(u32 a, u16 v) { gabi::store<u16>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline u32 vfn(u32 obj, u32 off) { return ld(ld(obj) + off); }
static inline u32 save_l() { return ld(0x101F84DC); }

struct sstr_l { be<u32> str, vt; };
struct shipframe_l { be<s16> pn1; be<s16> pn2; sstr_l s1; sstr_l s2; };
static constexpr u32 SafeString_vt = 0x10055394; /* this unit's sead::SafeString vtable copy */

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

/* 025C1E44 */
static s32 dStage_stagInfoInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C1E44, s32, i_stage, i_data, i_num, i_file);
    u32 fn = vfn(i_stage, 0x154);
    gabi::call_ptr(fn, i_stage, ld(i_data + 8)); /* setStagInfo */
    u32 stag = gabi::call_ptr<u32>(vfn(i_stage, 0x15C), i_stage); /* getStagInfo */
    u32 stageNo = (ld8(stag + 9) >> 1) & 0x7F; /* dStage_stagInfo_GetSaveTbl */
    dSv_info_c_getSave_l(save_l() + 0x20, stageNo);
    dSv_danBit_c_init_l(save_l() + 0x7BC, (s8)stageNo);
    return 1;
}
VERIFY(0x025C1E44, dStage_stagInfoInit);

/* 025C1ECC */
static s32 dStage_sclsInfoInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C1ECC, s32, i_stage, i_data, i_num, i_file);
    gabi::call_ptr(vfn(i_stage, 0x164), i_stage, i_data + 4);
    return 1;
}
VERIFY(0x025C1ECC, dStage_sclsInfoInit);

/* 025C1F00 */
static s32 dStage_actorInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C1F00, s32, i_stage, i_data, i_num, i_file);
    s32 n = (s32)ld(i_data + 4);
    u32 e = ld(i_data + 8);
    if (n <= 0) return 1;
    for (u32 c = (u32)n; c != 0; c--, e += 0x20) {
        u32 fn = vfn(i_stage, 0x14);
        u32 setId = ld16(e + 0x1E);
        u32 roomNo = gabi::call_ptr<u32>(fn, i_stage); /* getRoomNo */
        if (dSv_info_c_isActor_l(save_l() + 0x20, setId, roomNo)) continue;
        u32 appen = fopAcM_CreateAppend_l();
        if (appen == 0) continue;
        st(appen + 0, ld(e + 8));
        st(appen + 4, ld(e + 0xC));
        st(appen + 8, ld(e + 0x10));
        st(appen + 0xC, ld(e + 0x14));
        st(appen + 0x10, ld(e + 0x18));
        st(appen + 0x14, ld(e + 0x1C));
        u32 r = gabi::call_ptr<u32>(vfn(i_stage, 0x14), i_stage);
        st8(appen + 0x21, (u8)r);
        dStage_actorCreate_l(e, appen);
    }
    return 1;
}
VERIFY(0x025C1F00, dStage_actorInit);

/* 025C1FE0 */
static s32 dStage_tgscInfoInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C1FE0, s32, i_stage, i_data, i_num, i_file);
    s32 n = (s32)ld(i_data + 4);
    u32 e = ld(i_data + 8);
    if (n <= 0) return 1;
    for (u32 c = (u32)n; c != 0; c--, e += 0x24) {
        u32 appen = fopAcM_CreateAppend_l();
        if (appen == 0) continue;
        st(appen + 0, ld(e + 8));
        st(appen + 4, ld(e + 0xC));
        st(appen + 8, ld(e + 0x10));
        st(appen + 0xC, ld(e + 0x14));
        st(appen + 0x10, ld(e + 0x18));
        st(appen + 0x14, ld(e + 0x1C));
        u32 r = gabi::call_ptr<u32>(vfn(i_stage, 0x14), i_stage); /* getRoomNo */
        st8(appen + 0x21, (u8)r);
        u8 s0 = ld8(e + 0x20), s1 = ld8(e + 0x21), s2 = ld8(e + 0x22); /* scale u8[3] (lswi/stswi 3 bytes) */
        st8(appen + 0x18, s0);
        st8(appen + 0x19, s1);
        st8(appen + 0x1A, s2);
        dStage_actorCreate_l(e, appen);
    }
    return 1;
}
VERIFY(0x025C1FE0, dStage_tgscInfoInit);

/* 025C20B0: dStage_roomReadInit (unnamed by the matcher) */
static s32 dStage_roomReadInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C20B0, s32, i_stage, i_data, i_num, i_file);
    u32 rtbl = i_data + 4;
    u32 fn = vfn(i_stage, 0x5C);
    u32 entries = ld(i_data + 8);
    gabi::call_ptr(fn, i_stage, rtbl); /* setRoom */
    s32 n = (s32)ld(i_data + 4);
    if (0 >= n) return 1;
    u32 p = entries;
    for (s32 i = 0;;) {
        u32 r = ld(p) + i_file;
        st(p, r);
        st(r + 4, ld(r + 4) + i_file);
        p += 4;
        i++;
        if (!(i < (s32)ld(rtbl))) break;
    }
    return 1;
}
VERIFY(0x025C20B0, dStage_roomReadInit);

/* 025C2150 */
static u32 dStage_roomRead_dt_c_GetReverbStage(u32 room, s32 index) {
    WWHD_FUNC(0x025C2150, u32, room, index);
    u32 entries = ld(room + 4);
    if (index < 0 || index >= (s32)ld(room)) index = 0;
    return ld8(ld(entries + index * 4) + 1) & 0x7F;
}
VERIFY(0x025C2150, dStage_roomRead_dt_c_GetReverbStage);

/* 025C2184: dStage_ppntInfoInit */
static s32 dStage_ppntInfoInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C2184, s32, i_stage, i_data, i_num, i_file);
    gabi::call_ptr(vfn(i_stage, 0x174), i_stage, i_data + 4);
    return 1;
}
VERIFY(0x025C2184, dStage_ppntInfoInit);

/* the path handlers: set the path chunk, then relocate each path's point pointer by the point
 * chunk's offset (getPntInf / getPnt2Inf) */
static s32 pathInit(u32 i_stage, u32 i_data, u32 setOff, u32 getOff) {
    u32 hdr = i_data + 4;
    u32 fn = vfn(i_stage, setOff);
    u32 path = ld(i_data + 8);
    gabi::call_ptr(fn, i_stage, hdr);
    s32 n = (s32)ld(i_data + 4);
    if (0 >= n) return 1;
    for (s32 i = 0;;) {
        u32 pnt = gabi::call_ptr<u32>(vfn(i_stage, getOff), i_stage);
        u32 off = ld(pnt + 4);
        st(path + 8, ld(path + 8) + off);
        i++;
        path += 0xC;
        if (!(i < (s32)ld(hdr))) break;
    }
    return 1;
}

/* 025C21B8: dStage_pathInfoInit */
static s32 dStage_pathInfoInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C21B8, s32, i_stage, i_data, i_num, i_file);
    return pathInit(i_stage, i_data, 0x184, 0x17C);
}
VERIFY(0x025C21B8, dStage_pathInfoInit);

/* 025C2264: dStage_rppnInfoInit */
static s32 dStage_rppnInfoInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C2264, s32, i_stage, i_data, i_num, i_file);
    gabi::call_ptr(vfn(i_stage, 0x194), i_stage, i_data + 4);
    return 1;
}
VERIFY(0x025C2264, dStage_rppnInfoInit);

/* 025C2298: dStage_rpatInfoInit */
static s32 dStage_rpatInfoInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C2298, s32, i_stage, i_data, i_num, i_file);
    return pathInit(i_stage, i_data, 0x1A4, 0x19C);
}
VERIFY(0x025C2298, dStage_rpatInfoInit);

/* 025C2344: dStage_soundInfoInit */
static s32 dStage_soundInfoInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C2344, s32, i_stage, i_data, i_num, i_file);
    gabi::call_ptr(vfn(i_stage, 0x1B4), i_stage, i_data + 4);
    return 1;
}
VERIFY(0x025C2344, dStage_soundInfoInit);

/* 025C2378: dStage_eventInfoInit */
static s32 dStage_eventInfoInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C2378, s32, i_stage, i_data, i_num, i_file);
    gabi::call_ptr(vfn(i_stage, 0x1C4), i_stage, i_data + 4);
    return 1;
}
VERIFY(0x025C2378, dStage_eventInfoInit);

/* 025C23AC: dStage_floorInfoInit */
static s32 dStage_floorInfoInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C23AC, s32, i_stage, i_data, i_num, i_file);
    gabi::call_ptr(vfn(i_stage, 0x1E4), i_stage, i_data + 4);
    return 1;
}
VERIFY(0x025C23AC, dStage_floorInfoInit);

/* 025C23E0: fopAcM_SearchByName(SHIP) inlined (fopAcIt_Judge with fpcSch_JudgeForPName) */
static BOOL dStage_setShipPos(s32 param_0, s32 i_roomNo) {
    WWHD_FUNC(0x025C23E0, BOOL, param_0, i_roomNo);
    gabi::Local<shipframe_l> fr; /* the frame from SP+8: two process names, two SafeStrings */
    sstr_l* s1 = &fr->s1;
    sstr_l* s2 = &fr->s2;
    s1->str = 0x1005596C; /* "GanonM" */
    s1->vt = SafeString_vt;
    bool clear = false;
    u32 p = dComIfGp_ea();
    s2->vt = SafeString_vt;
    s2->str = p + 0x5134; /* the start stage name */
    if (sstr_equal(s1, s2)) {
        if (!dSv_event_c_isEventBit_l(save_l() + 0x644, 0x3D02)) clear = true;
    }
    if (clear) {
        param_0 = 0xFF;
        i_roomNo = 0xFF;
        st8(dComIfGp_ea() + 0x5D14, 0xFF);
        st8(dComIfGp_ea() + 0x5D15, 0xFF);
    }
    if ((s16)ld16(dComIfGp_ea() + 0x513C) == -3 && ld(save_l() + 0x12AC) != 0) {
        fr->pn1 = 0xA5;
        u32 ship = fopAcIt_Judge_l(0x025E121C, gabi::ea(&fr->pn1));
        if (ship != 0) {
            u32 s = save_l();
            daShip_c_initStartPos_l(ship, s + 0x129C, (s16)ld16(s + 0x12A8));
        }
        st(save_l() + 0x12AC, 0);
    }
    s32 roomNo = i_roomNo;
    if (i_roomNo == 0xFF) {
        roomNo = (s8)ld8(0x1047E6C8); /* mStayNo */
        if (param_0 == 0xFF) return FALSE;
    } else if (param_0 == 0xFF) {
        return FALSE;
    }
    u32 data = dComIfGp_getShip_l(roomNo, param_0);
    if (data == 0) return FALSE;
    fr->pn2 = 0xA5;
    u32 ship = fopAcIt_Judge_l(0x025E121C, gabi::ea(&fr->pn2));
    if (ship == 0) return FALSE;
    daShip_c_initStartPos_l(ship, data, (s16)ld16(data + 0xC));
    return TRUE;
}
VERIFY(0x025C23E0, dStage_setShipPos);

/* 025C2608: dStage_chkTaura inlined */
static s32 dStage_shipInfoInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C2608, s32, i_stage, i_data, i_num, i_file);
    gabi::call_ptr(vfn(i_stage, 0x1F4), i_stage, i_data + 4); /* setShip */
    s32 shipId = ld8(dComIfGp_ea() + 0x5D14);
    s32 roomId = ld8(dComIfGp_ea() + 0x5D15);
    gabi::Local<sstr_l> s1;
    gabi::Local<sstr_l> s2;
    s1->vt = SafeString_vt;
    s1->str = 0x1005532C; /* "sea" */
    bool taura = false;
    u32 p = dComIfGp_ea();
    s2->vt = SafeString_vt;
    s2->str = p + 0x5134;
    if (sstr_equal(s1.get(), s2.get())) {
        if (roomId == 0xB) taura = true;
    }
    if (taura && !dSv_event_c_isEventBit_l(save_l() + 0x644, 0x2A08)) {
        if (dStage_setShipPos(0x80, roomId)) {
            shipId = 0xFF;
            roomId = 0xFF;
            st8(dComIfGp_ea() + 0x5D14, 0xFF);
            st8(dComIfGp_ea() + 0x5D15, 0xFF);
        }
    }
    if (dStage_setShipPos(shipId, roomId)) {
        st8(dComIfGp_ea() + 0x5D14, 0xFF);
        st8(dComIfGp_ea() + 0x5D15, 0xFF);
    }
    return 1;
}
VERIFY(0x025C2608, dStage_shipInfoInit);

/* 025C27AC: dStage_multInfoInit */
static s32 dStage_multInfoInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C27AC, s32, i_stage, i_data, i_num, i_file);
    gabi::call_ptr(vfn(i_stage, 0x204), i_stage, i_data + 4);
    return 1;
}
VERIFY(0x025C27AC, dStage_multInfoInit);

/* 025C27E0: dStage_lbnkInfoInit */
static s32 dStage_lbnkInfoInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C27E0, s32, i_stage, i_data, i_num, i_file);
    gabi::call_ptr(vfn(i_stage, 0x214), i_stage, i_data + 4);
    return 1;
}
VERIFY(0x025C27E0, dStage_lbnkInfoInit);

/* the inlined dStage_KeepTresureInfoProc / dStage_KeepDoorInfoProc */
static void keepInfo(u32 i_stage, u32 src, u32 info, u32 max, u32 size) {
    u32 stag = gabi::call_ptr<u32>(vfn(i_stage, 0x15C), i_stage); /* getStagInfo */
    if (stag == 0) return;
    u32 type = (ld(stag + 0xC) >> 16) & 7; /* dStage_stagInfo_GetSTType */
    if (type == 3 || type == 6) return;
    if (src == 0 || ld(src) >= max) {
        st(info, 0);
        return;
    }
    u32 num = ld(src);
    st(info, num);
    if (num == 0) return;
    u32 s = ld(src + 4), d = info + 4;
    for (s32 i = 0; i < (s32)ld(info); i++, s += size, d += size)
        for (u32 o = 0; o < size; o += 4) st(d + o, ld(s + o));
}

/* 025C2814: dStage_stageTresureInit, dStage_KeepTresureInfoProc inlined (matcher:
 * dStage_KeepTresureInfoProc) */
static s32 dStage_stageTresureInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C2814, s32, i_stage, i_data, i_num, i_file);
    u32 tres = i_data + 4;
    gabi::call_ptr(vfn(i_stage, 0x224), i_stage, tres); /* setTresure */
    dStage_actorInit(i_stage, i_data, i_num, i_file);
    keepInfo(i_stage, tres, 0x1047D9B0, 0x20, 0x20);
    return 1;
}
VERIFY(0x025C2814, dStage_stageTresureInit);

/* 025C2950 */
static s32 dStage_roomTresureInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C2950, s32, i_stage, i_data, i_num, i_file);
    gabi::call_ptr(vfn(i_stage, 0x224), i_stage, i_data + 4);
    dStage_actorInit(i_stage, i_data, i_num, i_file);
    return 1;
}
VERIFY(0x025C2950, dStage_roomTresureInit);

/* 025C29C8 */
static s32 dStage_layerTresureInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C29C8, s32, i_stage, i_data, i_num, i_file);
    dStage_actorInit(i_stage, i_data, i_num, i_file);
    return 1;
}
VERIFY(0x025C29C8, dStage_layerTresureInit);

/* 025C29EC: dStage_dmapInfoInit */
static s32 dStage_dmapInfoInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C29EC, s32, i_stage, i_data, i_num, i_file);
    gabi::call_ptr(vfn(i_stage, 0x234), i_stage, i_data + 4);
    return 1;
}
VERIFY(0x025C29EC, dStage_dmapInfoInit);

/* 025C2A20: dStage_stageDrtgInfoInit, dStage_KeepDoorInfoProc inlined (matcher:
 * dStage_KeepDoorInfoProc) */
static s32 dStage_stageDrtgInfoInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C2A20, s32, i_stage, i_data, i_num, i_file);
    u32 drtg = i_data + 4;
    gabi::call_ptr(vfn(i_stage, 0x244), i_stage, drtg); /* setDrTg */
    dStage_tgscInfoInit(i_stage, i_data, i_num, i_file);
    keepInfo(i_stage, drtg, 0x1047DDB4, 0x40, 0x24);
    return 1;
}
VERIFY(0x025C2A20, dStage_stageDrtgInfoInit);

/* 025C2B48 */
static s32 dStage_roomDrtgInfoInit(u32 i_stage, u32 i_data, s32 i_num, u32 i_file) {
    WWHD_FUNC(0x025C2B48, s32, i_stage, i_data, i_num, i_file);
    gabi::call_ptr(vfn(i_stage, 0x244), i_stage, i_data + 4);
    dStage_tgscInfoInit(i_stage, i_data, i_num, i_file);
    return 1;
}
VERIFY(0x025C2B48, dStage_roomDrtgInfoInit);

} // namespace d_stage_3_cpp
