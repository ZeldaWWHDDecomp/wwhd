/* d_com_inf_game, part 2: 02522F6C..02525F4F (stage resource helpers, dComIfG_play_c destructor,
 * itemInit, the grass/tree/wood/flower and magma packets, getLayerNo, the unit's __sinit 02525A48
 * and its per-TU inline copies). See
 * d_com_inf_game.cpp for the unit's layout notes.
 *
 * HD-only: the stage resources are set/synchronised/deleted through sead::SafeString names
 * (resource control *(101F4F54)); a stage's archive name is "ma3room" instead of "ma2room" once
 * event 0x1820 is set, and some stages load extra archives (tables 101D5DC8..101D5DDC). */
#include "d/d_com_inf_game_unit.h"
namespace GameInfoMemory2 {
static inline u32 ld(u32 a) {
    return gabi::load<u32>(a);
}
static inline u16 ld16(u32 a) {
    return gabi::load<u16>(a);
}
static inline u8 ld8(u32 a) {
    return gabi::load<u8>(a);
}
static inline s8 lds8(u32 a) {
    return gabi::load<s8>(a);
}
static inline f32 ldf(u32 a) {
    return gabi::load<f32>(a);
}
static inline void st(u32 a, u32 v) {
    gabi::store<u32>(a, v);
}
static inline void st16(u32 a, u16 v) {
    gabi::store<u16>(a, v);
}
static inline void st8(u32 a, u8 v) {
    gabi::store<u8>(a, v);
}
static inline void stf(u32 a, f32 v) {
    gabi::store<f32>(a, v);
}
} // namespace GameInfoMemory2

void gameInfo2_drawListDestructor(u32, u32);
void gameInfo2_stringDestructor(u32, u32);
void gameInfo2_memoryInit(u32);
u32 gameInfo2_checkEventNightStop();
s32 gameInfo2_getDaytimeHour();
void gameInfo2_scopedHeapDestructor(u32, u32);
void gameInfo2_scopedHeapFailed(u32);
void gameInfo2_scopedHeapConstructor(u32, u32, u32, u32);
void gameInfo2_deletePacketHeap(u32);
u32 gameInfo2_magmaConstructor(u32);
u32 gameInfo2_particleConstructor(u32);
void gameInfo2_eventManagerRun(u32);
void gameInfo2_setArrowKeepType(u32);
void gameInfo2_OSBlockMove(u32, u32, u32, u32);
void gameInfo2_wideAssureTermination(u32);
u32 gameInfo2_checkGetItem(u32);
void gameInfo2_fixedStringDestructor(u32, u32);
void gameInfo2_destroyArray(u32, u32, u32, u32, u32, u32);
void gameInfo2_archiveNameDestructor(u32, u32);
void gameInfo2_archiveStringDestructor(u32, u32);
void gameInfo2_admDestructor(u32, u32);
void gameInfo2_stageDataDestructor(u32, u32);
void gameInfo2_attentionDestructor(u32, u32);
void gameInfo2_vibrationDestructor(u32, u32);
void gameInfo2_detectDestructor(u32, u32);
void gameInfo2_matrix34Multiply44(u32, u32, u32);
u32 gameInfo2_cameraProjectionMatrix(u32);
u32 gameInfo2_getStageArchiveResource(u32, u32, u32, u32);
void gameInfo2_deleteStageArchive(u32, u32, u32);
void gameInfo2_deleteExtraArchive(u32, u32, u32);
void gameInfo2_deleteObjectArchive(u32, u32);
u32 gameInfo2_syncStageArchive(u32, u32, u32);
u32 gameInfo2_setStageArchive(u32, u32, u32, u32);
void gameInfo2_setStageArchive_discard(u32, u32, u32, u32);
void gameInfo2_assureTermination(u32);
u32 gameInfo2_setExtraArchive(u32, u32, u32);
u32 gameInfo2_setObjectArchive(u32, u32, u32);
void gameInfo2_SafeString_format(u32, u32, u32);
u32 gameInfo2_getTriforceNum(u32);
u32 gameInfo2_isEventBit(u32, u32);
u32 gameInfo2_getGameInfo();
void gameInfo2___dl(u32);
u32 gameInfo2_operator_new(u32);
u32 gameInfo2_JUT_ASSERT(u32, u32, u32);

// HD: private native adapters retain the original full register widths and call order.
static inline u32 JUT_ASSERT_l(u32 file, s32 line, u32 msg) {
    return gameInfo2_JUT_ASSERT(file, line, msg);
}
static inline u32 operator_new_l(u32 size) {
    return gameInfo2_operator_new(size);
}
static inline void __dl_l(u32 p) {
    gameInfo2___dl(p);
}
static inline u32 gameInfoAddress() {
    return gameInfo2_getGameInfo();
}
static inline u32 savep() {
    return gabi::load<u32>(0x101F84DC);
}
static inline u32 resctl() {
    return gabi::load<u32>(0x101F4F54);
}
static inline u32 isEventBit_l(u32 flag) {
    return gameInfo2_isEventBit(savep() + 0x644, flag);
}
static inline u32 getTriforceNum_l() {
    return gameInfo2_getTriforceNum(savep() + 0xD4);
}
static inline void SafeString_format_l(u32 ss, u32 fmt, u32 arg) {
    gameInfo2_SafeString_format(ss, fmt, arg);
}

static constexpr u32 SS_VT = 0x1004B8C0;
static constexpr u32 FSS_VT = 0x1004B930;

/* sead::FixedSafeString<32> {top, vt, size, buffer[32]} */

static inline void vss(u32 ss) {
    gabi::call_ptr(GameInfoMemory2::ld((u32)GameInfoHD::view<SafeString>(ss)->__vtbl + 0x14), ss);
} /* assureTermination */

/* inlined strcmp: equal for the same pointer or equal characters up to a NUL (at most 0x40001) */
static inline bool str_equal(u32 a, u32 b) {
    if (a == b)
        return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = (u8)*GameInfoHD::view<be<u8>>(a + i);
        u8 cb = (u8)*GameInfoHD::view<be<u8>>(b + i);
        if (ca != cb)
            return false;
        if (ca == 0)
            return true;
    }
    return false;
}
/* a == b for a SafeString object a and the literal SafeString b on the stack: a's
 * assureTermination twice, then b's */
static inline bool ss_eq(u32 a, u32 b) {
    vss(a);
    vss(a);
    u32 ta = (u32)GameInfoHD::view<SafeString>(a)->mStringTop;
    vss(b);
    return str_equal(ta, (u32)GameInfoHD::view<SafeString>(b)->mStringTop);
}
/* the start stage name (gi + 0x5134) compared with a literal: literal SafeString first */
static inline bool startStageIs(u32 lit) {
    gabi::Local<SafeString> a;
    gabi::Local<SafeString> b;
    u32 aa = gabi::ea(a.get()), bb = gabi::ea(b.get());
    GameInfoHD::view<SafeString>(aa)->mStringTop = lit;
    GameInfoHD::view<SafeString>(aa)->__vtbl = SS_VT;
    u32 g = gameInfoAddress();
    GameInfoHD::view<SafeString>(bb)->mStringTop = g + 0x5134;
    GameInfoHD::view<SafeString>(bb)->__vtbl = SS_VT;
    return ss_eq(aa, bb);
}
/* SafeString ss compared with a literal */
static inline bool ssIs(u32 ss, u32 lit) {
    gabi::Local<SafeString> l;
    u32 ll = gabi::ea(l.get());
    GameInfoHD::view<SafeString>(ll)->mStringTop = lit;
    GameInfoHD::view<SafeString>(ll)->__vtbl = SS_VT;
    return ss_eq(ss, ll);
}

// HD, i_arc == "Stage" && i_stage is one of the sea stages
/* 02522F6C */
u32 hdGameInfo_isSeaStageRes(u32 i_arc, u32 i_stage) {
    if (!ssIs(i_arc, gabi::guest_addr("Stage")))
        return 0;
    const u32 names[5] = {gabi::guest_addr("ADMumi"), gabi::guest_addr("A_umikz"),
                          gabi::guest_addr("ENDumi"), gabi::guest_addr("sea_T"),
                          gabi::guest_addr("sea_E")};
    for (u32 k = 0; k < 5; k++)
        if (ssIs(i_stage, names[k]))
            return 1;
    return 0;
}

// HD, i_arc == "Stage" && i_stage == "ENDumi"
/* 02523318 */
u32 hdGameInfo_isEndStageRes(u32 i_arc, u32 i_stage) {
    if (!ssIs(i_arc, gabi::guest_addr("Stage")))
        return 0;
    return ssIs(i_stage, gabi::guest_addr("ENDumi")) ? 1 : 0;
}

// HD, i_arc == "Stage" && i_stage is "M_NewD2", "Siren" or "sea_T"
/* 0252349C */
u32 hdGameInfo_isExtraStageRes(u32 i_arc, u32 i_stage) {
    if (!ssIs(i_arc, gabi::guest_addr("Stage")))
        return 0;
    const u32 names[3] = {gabi::guest_addr("M_NewD2"), gabi::guest_addr("Siren"),
                          gabi::guest_addr("sea_T")};
    for (u32 k = 0; k < 3; k++)
        if (ssIs(i_stage, names[k]))
            return 1;
    return 0;
}

// HD, set the extra archives of a stage; true when all succeeded
/* 02523730 */
u32 hdGameInfo_setExtraStageRes(u32 i_stage) {
    u32 ok = 1;
    if (ssIs(i_stage, gabi::guest_addr("M_NewD2"))) {
        gabi::Local<SafeString> n;
        u32 nn = gabi::ea(n.get());
        for (u32 i = 0; i < 3; i++) {
            u32 rc = resctl();
            GameInfoHD::view<SafeString>(nn)->__vtbl = SS_VT;
            GameInfoHD::view<SafeString>(nn)->mStringTop = GameInfoMemory2::ld(0x101D5DDC + 4 * i);
            ok &= gameInfo2_setExtraArchive(rc, i_stage, nn);
        }
        return ok;
    }
    if (ssIs(i_stage, gabi::guest_addr("sea_T"))) {
        gabi::Local<SafeString> n;
        u32 nn = gabi::ea(n.get());
        for (u32 i = 0; i < 2; i++) {
            u32 name = GameInfoMemory2::ld(0x101D5DC8 + 4 * i);
            u32 rc = resctl();
            GameInfoHD::view<SafeString>(nn)->__vtbl = SS_VT;
            GameInfoHD::view<SafeString>(nn)->mStringTop = name;
            ok &= gameInfo2_setStageArchive(rc, i_stage, nn, 0);
        }
        gabi::Local<SafeString> m;
        u32 mm = gabi::ea(m.get());
        for (u32 i = 0; i < 2; i++) {
            u32 rc = resctl();
            GameInfoHD::view<SafeString>(mm)->__vtbl = SS_VT;
            GameInfoHD::view<SafeString>(mm)->mStringTop = GameInfoMemory2::ld(0x101D5DD0 + 4 * i);
            ok &= gameInfo2_setObjectArchive(rc, mm, 0);
        }
        return ok;
    }
    if (ssIs(i_stage, gabi::guest_addr("Siren"))) {
        gabi::Local<SafeString> n;
        u32 nn = gabi::ea(n.get());
        u32 rc = resctl();
        GameInfoHD::view<SafeString>(nn)->__vtbl = SS_VT;
        GameInfoHD::view<SafeString>(nn)->mStringTop = GameInfoMemory2::ld(0x101D5DD8);
        return gameInfo2_setExtraArchive(rc, i_stage, nn) & 1;
    }
    return ok;
}

/* FixedSafeString<32> initialised from a SafeString temporary {i_arc, vt}: strlen (0 beyond
 * 0x40000), clamped to size - 1, copied with 028FDC00 and terminated */
static inline void fss_from(u32 fs, u32 ss, u32 i_arc) {
    GameInfoHD::view<SafeString>(ss)->mStringTop = i_arc;
    GameInfoHD::view<SafeString>(ss)->__vtbl = SS_VT;
    GameInfoHD::view<GameInfoHD::FixedSafeString32>(fs)->mStringTop = fs + 0xC;
    GameInfoHD::view<GameInfoHD::FixedSafeString32>(fs)->mBufferSize = 0x20;
    GameInfoHD::view<GameInfoHD::FixedSafeString32>(fs)->mBuffer[31] = 0;
    GameInfoHD::view<GameInfoHD::FixedSafeString32>(fs)->__vtbl = 0x1004B918;
    gameInfo2_assureTermination(ss);
    u32 s = (u32)GameInfoHD::view<SafeString>(ss)->mStringTop;
    s32 len = 0;
    if ((u8)*GameInfoHD::view<be<u8>>(s) != 0) {
        for (;;) {
            len++;
            if (len > 0x40000) {
                len = 0;
                break;
            }
            if ((u8)*GameInfoHD::view<be<u8>>(s + (u32)len) == 0)
                break;
        }
    }
    s32 size = (s32)(u32)GameInfoHD::view<GameInfoHD::FixedSafeString32>(fs)->mBufferSize;
    if (len >= size)
        len = size - 1;
    vss(ss);
    gameInfo2_OSBlockMove(fs + 0xC, (u32)GameInfoHD::view<SafeString>(ss)->mStringTop, len, 0);
    *GameInfoHD::view<be<u8>>(fs + 0xC + (u32)len) = 0;
}
/* the stage name archive: "ma3room" for "ma2room" after event 0x1820, else the start stage */
static inline void stageArcName(u32 fs, u32 ma2, u32 ma3, u32 fmt) {
    bool ma = startStageIs(ma2);
    if (ma && isEventBit_l(0x1820) != 0) {
        SafeString_format_l(fs, fmt, ma3);
        return;
    }
    u32 g = gameInfoAddress();
    SafeString_format_l(fs, fmt, g + 0x5134);
}

// HD hdGameInfo_dComIfG_setStageRes(arcName) (matcher: dRes_control_c::setStageRes)
/* Returns the result of the final 0260D618 call (left in r3; callers read it: void_used / retvals
 * type-only). */
/* 02523A04 */
u32 hdGameInfo_dComIfG_setStageRes(u32 i_arc) {
    gabi::Local<SafeString> ss;
    gabi::Local<GameInfoHD::FixedSafeString32> arc;
    gabi::Local<GameInfoHD::FixedSafeString32> stg;
    u32 a = gabi::ea(arc.get()), s = gabi::ea(stg.get());
    fss_from(a, gabi::ea(ss.get()), i_arc);
    GameInfoHD::view<GameInfoHD::FixedSafeString32>(s)->mBuffer[0] = 0;
    GameInfoHD::view<GameInfoHD::FixedSafeString32>(s)->mStringTop = s + 0xC;
    GameInfoHD::view<GameInfoHD::FixedSafeString32>(s)->mBuffer[31] = 0;
    GameInfoHD::view<GameInfoHD::FixedSafeString32>(a)->__vtbl = FSS_VT;
    GameInfoHD::view<GameInfoHD::FixedSafeString32>(s)->mBufferSize = 0x20;
    GameInfoHD::view<GameInfoHD::FixedSafeString32>(s)->__vtbl = FSS_VT;
    stageArcName(s, gabi::guest_addr("ma2room"), gabi::guest_addr("ma3room"),
                 gabi::guest_addr("%s"));
    if (hdGameInfo_isSeaStageRes(a, s) != 0) {
        gabi::Local<SafeString> n1;
        gabi::Local<SafeString> n2;
        u32 x = gabi::ea(n1.get()), y = gabi::ea(n2.get());
        GameInfoHD::view<SafeString>(x)->__vtbl = SS_VT;
        GameInfoHD::view<SafeString>(y)->__vtbl = SS_VT;
        GameInfoHD::view<SafeString>(x)->mStringTop = gabi::guest_addr("sea");
        GameInfoHD::view<SafeString>(y)->mStringTop = gabi::guest_addr("Stage");
        gameInfo2_setStageArchive_discard(resctl(), x, y, 0);
    }
    if (hdGameInfo_isEndStageRes(a, s) != 0) {
        gabi::Local<SafeString> n1;
        gabi::Local<SafeString> n2;
        u32 x = gabi::ea(n1.get()), y = gabi::ea(n2.get());
        GameInfoHD::view<SafeString>(x)->__vtbl = SS_VT;
        GameInfoHD::view<SafeString>(y)->__vtbl = SS_VT;
        GameInfoHD::view<SafeString>(x)->mStringTop = gabi::guest_addr("Hyrule");
        GameInfoHD::view<SafeString>(y)->mStringTop = gabi::guest_addr("Room0");
        gameInfo2_setStageArchive_discard(resctl(), x, y, 0);
    }
    if (hdGameInfo_isExtraStageRes(a, s) != 0)
        hdGameInfo_setExtraStageRes(s);
    return gameInfo2_setStageArchive(resctl(), s, a, 0);
}

// HD hdGameInfo_dComIfG_syncStageRes(arcName)
/* 02523D08 */
u32 hdGameInfo_dComIfG_syncStageRes(u32 i_arc) {
    gabi::Local<SafeString> ss;
    gabi::Local<GameInfoHD::FixedSafeString32> stg;
    u32 s = gabi::ea(stg.get()), q = gabi::ea(ss.get());
    GameInfoHD::view<GameInfoHD::FixedSafeString32>(s)->mStringTop = s + 0xC;
    GameInfoHD::view<GameInfoHD::FixedSafeString32>(s)->mBufferSize = 0x20;
    GameInfoHD::view<GameInfoHD::FixedSafeString32>(s)->mBuffer[31] = 0;
    GameInfoHD::view<GameInfoHD::FixedSafeString32>(s)->__vtbl = FSS_VT;
    GameInfoHD::view<GameInfoHD::FixedSafeString32>(s)->mBuffer[0] = 0;
    stageArcName(s, gabi::guest_addr("ma2room"), gabi::guest_addr("ma3room"),
                 gabi::guest_addr("%s"));
    GameInfoHD::view<SafeString>(q)->mStringTop = i_arc;
    GameInfoHD::view<SafeString>(q)->__vtbl = SS_VT;
    return gameInfo2_syncStageArchive(resctl(), s, q);
}

// HD, delete the extra archives of a stage
/* 02523E9C */
void hdGameInfo_deleteExtraStageRes(u32 i_stage) {
    if (ssIs(i_stage, gabi::guest_addr("M_NewD2"))) {
        gabi::Local<SafeString> n;
        u32 nn = gabi::ea(n.get());
        for (u32 i = 0; i < 3; i++) {
            u32 name = GameInfoMemory2::ld(0x101D5DDC + 4 * i);
            u32 rc = resctl();
            GameInfoHD::view<SafeString>(nn)->__vtbl = SS_VT;
            GameInfoHD::view<SafeString>(nn)->mStringTop = name;
            gameInfo2_deleteExtraArchive(rc, i_stage, nn);
        }
        return;
    }
    if (ssIs(i_stage, gabi::guest_addr("sea_T"))) {
        gabi::Local<SafeString> n;
        u32 nn = gabi::ea(n.get());
        for (u32 i = 0; i < 2; i++) {
            u32 name = GameInfoMemory2::ld(0x101D5DC8 + 4 * i);
            u32 rc = resctl();
            GameInfoHD::view<SafeString>(nn)->__vtbl = SS_VT;
            GameInfoHD::view<SafeString>(nn)->mStringTop = name;
            gameInfo2_deleteStageArchive(rc, i_stage, nn);
        }
        gabi::Local<SafeString> m;
        u32 mm = gabi::ea(m.get());
        for (u32 i = 0; i < 2; i++) {
            u32 name = GameInfoMemory2::ld(0x101D5DD0 + 4 * i);
            u32 rc = resctl();
            GameInfoHD::view<SafeString>(mm)->__vtbl = SS_VT;
            GameInfoHD::view<SafeString>(mm)->mStringTop = name;
            gameInfo2_deleteObjectArchive(rc, mm);
        }
        return;
    }
    if (ssIs(i_stage, gabi::guest_addr("Siren"))) {
        gabi::Local<SafeString> n;
        u32 nn = gabi::ea(n.get());
        u32 rc = resctl();
        GameInfoHD::view<SafeString>(nn)->__vtbl = SS_VT;
        GameInfoHD::view<SafeString>(nn)->mStringTop = GameInfoMemory2::ld(0x101D5DD8);
        gameInfo2_deleteExtraArchive(rc, i_stage, nn);
    }
}

// HD hdGameInfo_dComIfG_deleteStageRes(arcName)
/* 02524180 */
u32 hdGameInfo_dComIfG_deleteStageRes(u32 i_arc) {
    gabi::Local<SafeString> ss;
    gabi::Local<GameInfoHD::FixedSafeString32> arc;
    gabi::Local<GameInfoHD::FixedSafeString32> stg;
    u32 a = gabi::ea(arc.get()), s = gabi::ea(stg.get());
    fss_from(a, gabi::ea(ss.get()), i_arc);
    GameInfoHD::view<GameInfoHD::FixedSafeString32>(s)->mBuffer[0] = 0;
    GameInfoHD::view<GameInfoHD::FixedSafeString32>(s)->mStringTop = s + 0xC;
    GameInfoHD::view<GameInfoHD::FixedSafeString32>(s)->mBuffer[31] = 0;
    GameInfoHD::view<GameInfoHD::FixedSafeString32>(s)->__vtbl = FSS_VT;
    GameInfoHD::view<GameInfoHD::FixedSafeString32>(s)->mBufferSize = 0x20;
    GameInfoHD::view<GameInfoHD::FixedSafeString32>(a)->__vtbl = FSS_VT;
    stageArcName(s, gabi::guest_addr("ma2room"), gabi::guest_addr("ma3room"),
                 gabi::guest_addr("%s"));
    if (hdGameInfo_isSeaStageRes(a, s) != 0) {
        gabi::Local<SafeString> n1;
        gabi::Local<SafeString> n2;
        u32 x = gabi::ea(n1.get()), y = gabi::ea(n2.get());
        u32 rc = resctl();
        GameInfoHD::view<SafeString>(x)->__vtbl = SS_VT;
        GameInfoHD::view<SafeString>(y)->__vtbl = SS_VT;
        GameInfoHD::view<SafeString>(x)->mStringTop = gabi::guest_addr("sea");
        GameInfoHD::view<SafeString>(y)->mStringTop = gabi::guest_addr("Stage");
        gameInfo2_deleteStageArchive(rc, x, y);
    }
    if (hdGameInfo_isEndStageRes(a, s) != 0) {
        gabi::Local<SafeString> n1;
        gabi::Local<SafeString> n2;
        u32 x = gabi::ea(n1.get()), y = gabi::ea(n2.get());
        u32 rc = resctl();
        GameInfoHD::view<SafeString>(x)->__vtbl = SS_VT;
        GameInfoHD::view<SafeString>(y)->__vtbl = SS_VT;
        GameInfoHD::view<SafeString>(x)->mStringTop = gabi::guest_addr("Hyrule");
        GameInfoHD::view<SafeString>(y)->mStringTop = gabi::guest_addr("Room0");
        gameInfo2_deleteStageArchive(rc, x, y);
    }
    if (hdGameInfo_isExtraStageRes(a, s) != 0)
        hdGameInfo_deleteExtraStageRes(s);
    gameInfo2_deleteStageArchive(resctl(), s, a);
    return 1;
}

// HD hdGameInfo_dComIfG_getStageRes(arcName, resName) through dRes_control_c (*(101F4F28))
/* 0252447C */
u32 hdGameInfo_dComIfG_getStageRes(u32 i_arc, u32 i_res) {
    gabi::Local<SafeString> ss;
    gabi::Local<GameInfoHD::FixedSafeString32> stg;
    gabi::Local<SafeString> rs;
    u32 s = gabi::ea(stg.get()), q = gabi::ea(ss.get()), r = gabi::ea(rs.get());
    GameInfoHD::view<GameInfoHD::FixedSafeString32>(s)->mBufferSize = 0x20;
    GameInfoHD::view<GameInfoHD::FixedSafeString32>(s)->mBuffer[31] = 0;
    GameInfoHD::view<GameInfoHD::FixedSafeString32>(s)->mStringTop = s + 0xC;
    GameInfoHD::view<GameInfoHD::FixedSafeString32>(s)->mBuffer[0] = 0;
    GameInfoHD::view<GameInfoHD::FixedSafeString32>(s)->__vtbl = FSS_VT;
    stageArcName(s, gabi::guest_addr("ma2room"), gabi::guest_addr("ma3room"),
                 gabi::guest_addr("%s"));
    GameInfoHD::view<SafeString>(q)->mStringTop = i_arc;
    GameInfoHD::view<SafeString>(q)->__vtbl = SS_VT;
    GameInfoHD::view<SafeString>(r)->__vtbl = SS_VT;
    GameInfoHD::view<SafeString>(r)->mStringTop = i_res;
    return gameInfo2_getStageArchiveResource(GameInfoMemory2::ld(0x101F4F28), q, s, r);
}

// HD, the current view's projection matrix (copied to the static 104753AC)
/* 02524628 */
u32 hdGameInfo_dComIfGd_getProjViewMtx() {
    u32 p = (u32)GameInfoHD::view<GameInfoHD::DrawInfo>(gameInfoAddress())->mpProjView;
    if (p != 0)
        return gabi::ea(&GameInfoHD::view<GameInfoHD::ProjectionView>(p)->mProjViewMtx);
    u32 disp = GameInfoMemory2::ld(0x101F95D0);
    u32 cnt = (u32)GameInfoHD::view<GameInfoHD::Display>(disp)->mViewNum;
    u32 lst = (u32)GameInfoHD::view<GameInfoHD::Display>(disp)->mpViews;
    if (cnt > 1)
        lst += 4;
    u32 guard = GameInfoMemory2::ld(0x101FD9AC);
    u32 view = GameInfoMemory2::ld(lst);
    if (guard == 0) {
        GameInfoMemory2::st(0x101FD9AC, 1);
        GameInfoMemory2::st(0x101FDCE4, 0x1004B9B8);
    }
    if (view != 0) {
        if (gabi::call_ptr<u32>(
                GameInfoMemory2::ld((u32)GameInfoHD::view<GameInfoHD::View>(view)->__vtbl + 0x14),
                view, 0x101FDCE4u) == 0)
            view = 0; /* DynamicCast: checkDerivedRuntimeTypeInfo(&type info 101FDCE4), r4 is live
                         at the bctrl (livein check 2026-10-04) */
    }
    u32 flags = (u32)GameInfoHD::view<GameInfoHD::View>(view)->mFlags;
    u32 cam;
    bool viaCam;
    if ((flags >> 12) & 1) {
        cam = (u32)GameInfoHD::view<GameInfoHD::View>(view)->mpConfiguredCamera;
        viaCam = (flags >> 6) & 1;
    } else {
        u32 c = (u32)GameInfoHD::view<GameInfoHD::View>(view)->mpCamera;
        cam = 0x104A20FC;
        if (c == 0)
            viaCam = (flags >> 6) & 1;
        else {
            cam = c;
            viaCam = (flags >> 6) & 1;
        }
    }
    gabi::Local<f32[16]> m;
    u32 mm = gabi::ea(m.get());
    u32 src;
    if (viaCam && ((flags >> 7) & 1)) {
        src = gabi::ea(&GameInfoHD::view<GameInfoHD::View>(view)->mViewMtx);
    } else {
        u32 v = (u32)GameInfoHD::view<GameInfoHD::View>(view)->mpViewMtx;
        src = v != 0 ? v : 0x104A2098;
    }
    u32 proj = gameInfo2_cameraProjectionMatrix(cam);
    gameInfo2_matrix34Multiply44(mm, src, proj);
    for (u32 i = 0; i < 16; i++)
        GameInfoMemory2::stf(0x104753AC + 4 * i, GameInfoMemory2::ldf(mm + 4 * i));
    return 0x104753AC;
}

// dComIfG_play_c::~dComIfG_play_c
/* 025247B8 */
void hdGameInfo_dComIfG_play_c_dt(u32 i_this, s32 i_flag) {
    if (i_this == 0)
        return;
    gameInfo2_detectDestructor(i_this + 0x4780, 2);    /* ~dDetect_c */
    gameInfo2_vibrationDestructor(i_this + 0x46FC, 2); /* ~dVibration_c */
    gameInfo2_attentionDestructor(i_this + 0x4564, 2); /* ~dAttention_c */
    gameInfo2_stageDataDestructor(i_this + 0x3EB0, 2);
    gameInfo2_admDestructor(i_this + 0x3DF8, 2);
    gameInfo2_stringDestructor(i_this + 0x3DEC, 2);
    gameInfo2_archiveStringDestructor(i_this + 0x3DAC, 2);
    gameInfo2_archiveNameDestructor(i_this + 0x3D48, 2);
    gameInfo2_destroyArray(i_this + 0x3D18, 2, 0x18, 0x02525B6C, 0, 0); /* __destroy_arr */
    gameInfo2_destroyArray(i_this + 0x3C9C, 5, 0x18, 0x02525B6C, 0, 0);
    gameInfo2_fixedStringDestructor(i_this + 0x3C58, 2);
    gameInfo2_fixedStringDestructor(i_this + 0x3C14, 2);
    if (i_flag & 1)
        __dl_l(i_this);
}

// dComIfG_play_c::itemInit
/* 025248BC */
void GameInfoHD::dComIfG_play_c::itemInit() {
    this->mItemLifeCount = 0.0f;
    this->mItemScopeWipeScale = 0.0f;
    this->mItemArrowNumCount = 0;
    this->mItemMaxLifeCount = 0;
    this->mItemKeyNumCount = 0;
    this->mItemPictureNumCount = 0;
    this->mItemBombNumCount = 0;
    this->mItemMagicCount = 0;
    this->mItemNowMagicCount = 0;
    this->field_0x48e6 = 0;
    this->field_0x48e2 = 0;
    this->mItemMaxMagicCount = 0;
    this->mItemNameMessageID = 0;
    this->mItemRupeeCount = 0;
    this->mNpcNameMessageID = 0;
    this->mItemTimeMax = 0;
    this->mAirMeter = 0;
    for (u32 i = 0; i < 8; i++) {
        this->mItemBeastNumCounts[i] = 0;
        GameInfoHD::view<GameInfoHD::ItemSupplement>(gabi::ea(this))->mAdditionalItemCounts[0][i] =
            0;
        GameInfoHD::view<GameInfoHD::ItemSupplement>(gabi::ea(this))->mAdditionalItemCounts[1][i] =
            0;
    }
    this->mRStatusForce = 0;
    this->mItemScopeWipeTimer = 0;
    this->mRStatus = 0;
    this->field_0x492c = 0;
    this->mAStatusForce = 0;
    this->mItemSwimTimerStatus = 0;
    this->mDoStatusForce = 0;
    this->mScopeMesgStatus = 0;
    this->mAStatus = 0;
    this->mMesgStatus = 0;
    this->mDoStatus = 0;
    this->mMsgSetNumber = 0;
    this->mItemNowLife = 0;
    this->mMessageRupee = 0;
    this->mItemTimer = 0;
    this->mAuctionRupee = 0;
    this->mAuctionGauge = 0;
    this->mItemNowRupee = 0;
    this->mMsgCountNumber = 0;
    for (u32 i = 0; i < 5; i++) {
        this->mSelectItem[i] = 0;
        GameInfoHD::view<GameInfoHD::ItemSupplement>(gabi::ea(this))->mSelectEquip[i] = 0;
    }
    this->mMesgAnime = 0;
    this->mMesgAnimeTagInfo = 0;
    this->mItemSlot = 0;
    this->mItemNo = 0;
    this->field_0x493f = 0;
    this->field_0x4940 = 0;
    this->mDirection = 0;
    this->mButtonMode = 0;
    u8 btn = 0x15;
    if (gameInfo2_checkGetItem(GameInfoHD::dItemNo_TELESCOPE_e) != 0)
        btn = 0; /* dComIfGs_checkGetItem(TELESCOPE) */
    this->mButtonInfo[0] = btn;
    this->mScopeType = 0;
    this->mOperateWind = 0;
    this->mMetronome = 0;
    this->mMesgSendButton = 0;
    this->mMesgCancelButton = 0;
    this->mButtonInfo[1] = 7;
    for (u32 i = 0; i < 6; i++)
        this->field_0x494a[i] = 0;
    this->mMelodyNum = 0;
    this->mFmapOpen = 0;
    this->mNameOpen = 0;
    this->field_0x4953 = 0;
    this->field_0x4954 = 0;
    this->mStartItemTimer = 0;
    this->mFwaterTimer = 0;
    this->mPlacenameIndex = 0;
    this->mPlacenameState = 0;
    this->mGameoverStatus = 0;
    this->mMenuCollect = 0;
    this->mPictureFlag = 0;
    this->mPictureResult = 0;
    this->mPictureResultDetail = 0;
    this->mPictureStatus = 0;
    this->mGetPictureNum = 0;
    GameInfoHD::view<GameInfoHD::PlaySupplement>(gabi::ea(this))->mPictureFormat = 0;
    GameInfoHD::view<GameInfoHD::PlaySupplement>(gabi::ea(this))->mSelectPicture = 0;
    GameInfoHD::view<GameInfoHD::PlaySupplement>(gabi::ea(this))->mHeapLockFlag = 0;
    GameInfoHD::view<GameInfoHD::PlaySupplement>(gabi::ea(this))->field_0x4965 = 0;
    /* strcpy(mInputPassword, "") on the HD WFixedSafeString at +0x4950 */
    u32 dst = (u32)GameInfoHD::view<GameInfoHD::PlaySupplement>(gabi::ea(this))
                  ->mInputPassword.mStringTop;
    gabi::Local<SafeString> w;
    u32 ww = gabi::ea(w.get());
    GameInfoHD::view<SafeString>(ww)->__vtbl = 0x1004B900;
    GameInfoHD::view<SafeString>(ww)->mStringTop = 0x1004BBD0;
    gameInfo2_wideAssureTermination(ww);
    u32 s = (u32)GameInfoHD::view<SafeString>(ww)->mStringTop;
    s32 len = 0;
    if (GameInfoMemory2::ld16(s) != 0) {
        u32 p = s;
        for (;;) {
            len++;
            p += 2;
            if (len > 0x40000) {
                len = 0;
                break;
            }
            if (GameInfoMemory2::ld16(p) == 0)
                break;
        }
    }
    s32 size = (s32)(u32)GameInfoHD::view<GameInfoHD::PlaySupplement>(gabi::ea(this))
                   ->mInputPassword.mBufferSize;
    if (len >= size)
        len = size - 1;
    vss(ww);
    gameInfo2_OSBlockMove(dst, (u32)GameInfoHD::view<SafeString>(ww)->mStringTop, (u32)len << 1, 0);
    GameInfoMemory2::st16(dst + ((u32)len << 1), 0);
    GameInfoHD::view<GameInfoHD::PlaySupplement>(gabi::ea(this))->field_0x497a = 0;
    GameInfoHD::view<GameInfoHD::PlaySupplement>(gabi::ea(this))->mMesgBgm = 0;
    GameInfoHD::view<GameInfoHD::PlaySupplement>(gabi::ea(this))->m2dShow = 0;
    GameInfoHD::view<GameInfoHD::PlaySupplement>(gabi::ea(this))->mScopeWipeFlag = 0;
    GameInfoHD::view<GameInfoHD::PlaySupplement>(gabi::ea(this))->mNowVibration =
        (u8)GameInfoHD::view<dSv_player_c>(savep() + 0x20)
            ->mConfig.mVibration;  /* option vibration */
    gameInfo2_setArrowKeepType(0); /* daArrow_c::setKeepType(TYPE_NORMAL) */
    GameInfoHD::view<GameInfoHD::PlaySupplement>(gabi::ea(this))->mMesgCamID = 0;
    GameInfoHD::view<GameInfoHD::PlaySupplement>(gabi::ea(this))->mMesgCamBasicID = 0;
    for (u32 i = 0; i < 10; i++)
        GameInfoHD::view<GameInfoHD::PlaySupplement>(gabi::ea(this))->mMesgCamActors[i] = 0;
    for (u32 i = 0; i < 6; i++)
        GameInfoHD::view<GameInfoHD::PlaySupplement>(gabi::ea(this))->hdItemFlags[i] = 0;
}

// dComIfG_play_c::executeEvtManager
/* 02524BA8 */
void GameInfoHD::dComIfG_play_c::executeEvtManager() {
    gameInfo2_eventManagerRun(gabi::ea(this) + 0x4024);
}

/* 02524BB0 */
void GameInfoHD::dComIfG_play_c::createParticle() {
    u32 p = operator_new_l(0x41A4);
    if (p != 0)
        p = gameInfo2_particleConstructor(p); /* dPa_control_c */
    this->mParticle.v = p;
    if (p == 0)
        JUT_ASSERT_l(0x1004BBD4, 0x206, 0x1004BBE8);
}

/* Returns the magma packet: the existing one (lwz r3,0x4814) or the new one (dMagma_packet_c ctor
 * result); daMagma and daBtd test it (void_used triage 2026-10-05: a void declaration returned
 * i_this / the store's leftover). */
/* 02524C10 */
u32 GameInfoHD::dComIfG_play_c::createMagma() {
    u32 packet = (u32)this->mpMagmaPacket;
    if (packet != 0)
        return packet;
    packet = gameInfo2_magmaConstructor(0);
    this->mpMagmaPacket = packet;
    return packet;
}

/* packet slot: remove = delete with the HD virtual destructor (vtable +0xC, slot 0xC) */
static inline void removeVirt(be<u32>& packetSlot, s32 kind) {
    u32 p = (u32)packetSlot;
    if (p == 0)
        return;
    gabi::call_ptr(GameInfoMemory2::ld(GameInfoMemory2::ld(p + 0xC) + 0xC), p, kind);
    if (kind == 3) {
        packetSlot = 0;
        return;
    }
    packetSlot = 0;
    gameInfo2_deletePacketHeap(p);
}
static inline void callIf(be<u32>& packetSlot, u32 fn) {
    u32 p = (u32)packetSlot;
    if (p == 0)
        return;
    gabi::call(fn, p);
}
/* create a packet in its own heap (025F01D8 scoped heap guard) */
static inline u32 createPacket(be<u32>& packetSlot, u32 name, u32 size, u32 ct) {
    u32 p = (u32)packetSlot;
    if (p != 0)
        return p;
    gabi::Local<u8[0x10]> guard;
    u32 g = gabi::ea(guard.get());
    gameInfo2_scopedHeapConstructor(g, name, size, 0);
    u32 n = gabi::call<u32>(ct, 0);
    packetSlot = n;
    if (n == 0)
        gameInfo2_scopedHeapFailed(g);
    gameInfo2_scopedHeapDestructor(g, 2);
    return (u32)packetSlot;
}

/* 02524C50 */
void GameInfoHD::dComIfG_play_c::removeMagma() {
    removeVirt(this->mpMagmaPacket, 3);
}

/* 02524CA0 */
void GameInfoHD::dComIfG_play_c::executeMagma() {
    callIf(this->mpMagmaPacket, GameInfoHD::MagmaCalc);
}

/* 02524CB0 */
void GameInfoHD::dComIfG_play_c::drawMagma() {
    callIf(this->mpMagmaPacket, GameInfoHD::MagmaUpdate);
}

/* 02524CC0 */
u32 GameInfoHD::dComIfG_play_c::createGrass() {
    return createPacket(this->mpGrassPacket, 0x1004BBFC, 0x147740, GameInfoHD::GrassConstructor);
}

/* 02524D3C */
void GameInfoHD::dComIfG_play_c::removeGrass() {
    removeVirt(this->mpGrassPacket, 2);
}

/* 02524DA0 */
void GameInfoHD::dComIfG_play_c::executeGrass() {
    callIf(this->mpGrassPacket, GameInfoHD::GrassCalc);
}

/* 02524DB0 */
void GameInfoHD::dComIfG_play_c::drawGrass() {
    callIf(this->mpGrassPacket, GameInfoHD::GrassUpdate);
}

/* 02524DC0 */
u32 GameInfoHD::dComIfG_play_c::createTree() {
    return createPacket(this->mpTreePacket, 0x1004BC10, 0xA0E40, GameInfoHD::TreeConstructor);
}

/* 02524E3C */
void GameInfoHD::dComIfG_play_c::removeTree() {
    removeVirt(this->mpTreePacket, 2);
}

/* 02524EA0 */
void GameInfoHD::dComIfG_play_c::executeTree() {
    callIf(this->mpTreePacket, GameInfoHD::TreeCalc);
}

/* 02524EB0 */
void GameInfoHD::dComIfG_play_c::drawTree() {
    callIf(this->mpTreePacket, GameInfoHD::TreeUpdate);
}

/* 02524EC0 */
u32 GameInfoHD::dComIfG_play_c::createWood() {
    return createPacket(this->mpWoodPacket, 0x1004BC28, 0x156240, GameInfoHD::WoodConstructor);
}

/* 02524F3C */
void GameInfoHD::dComIfG_play_c::removeWood() {
    removeVirt(this->mpWoodPacket, 2);
}

/* 02524FA0 */
void GameInfoHD::dComIfG_play_c::executeWood() {
    callIf(this->mpWoodPacket, GameInfoHD::WoodCalc);
}

/* 02524FB0 */
void GameInfoHD::dComIfG_play_c::drawWood() {
    callIf(this->mpWoodPacket, GameInfoHD::WoodUpdate);
}

/* 02524FC0 */
u32 GameInfoHD::dComIfG_play_c::createFlower() {
    return createPacket(this->mpFlowerPacket, 0x1004BC40, 0x99340, GameInfoHD::FlowerConstructor);
}

/* 0252503C */
void GameInfoHD::dComIfG_play_c::removeFlower() {
    removeVirt(this->mpFlowerPacket, 2);
}

/* 025250A0 */
void GameInfoHD::dComIfG_play_c::executeFlower() {
    callIf(this->mpFlowerPacket, GameInfoHD::FlowerCalc);
}

/* 025250B0 */
void GameInfoHD::dComIfG_play_c::drawFlower() {
    callIf(this->mpFlowerPacket, GameInfoHD::FlowerUpdate);
}

// HD: layer selection receives the room in r3, with no this argument; it adds the
// A_mori event check before Asoko and changes the kenroom check order.
/* 025250C0 */
s32 hdGameInfo_dComIfG_play_c_getLayerNo(s32 i_roomNo) {
    if ((s8)GameInfoHD::view<dComIfG_inf_c>(gameInfoAddress())->play.mCurStage.mLayer >= 0)
        return (s8)GameInfoHD::view<dComIfG_inf_c>(gameInfoAddress())->play.mCurStage.mLayer;
    s32 hour = gameInfo2_getDaytimeHour();
    s32 layer;
    if (gameInfo2_checkEventNightStop() != 0)
        layer = 1;
    else
        layer = ((u32)(hour - 6) < 12) ? 0 : 1;
    if (startStageIs(gabi::guest_addr("sea"))) {
        if (i_roomNo == 0x2C) {
            if (isEventBit_l(0x520))
                return layer | 4;
            if (isEventBit_l(0xE20))
                return layer | 2;
            if (isEventBit_l(0x101))
                return 9;
            return layer;
        }
        if (i_roomNo == 0xB) {
            if (isEventBit_l(0x2D01))
                return layer | 4;
            if (gameInfo2_checkEventNightStop() != 0)
                return layer | 2;
            return layer;
        }
        if (i_roomNo == 1) {
            if (isEventBit_l(0x1820))
                return 3;
            return 1;
        }
        return layer;
    }
    if (startStageIs(gabi::guest_addr("A_mori")))
        return isEventBit_l(0xF80) ? (layer | 2) : layer;
    if (startStageIs(gabi::guest_addr("Asoko")))
        return isEventBit_l(0x520) ? (layer | 2) : layer;
    if (startStageIs(gabi::guest_addr("Hyrule"))) {
        if (getTriforceNum_l() == 8)
            return layer | 4;
        return isEventBit_l(0x3280) ? (layer | 2) : layer;
    }
    if (startStageIs(gabi::guest_addr("Hyroom"))) {
        if (getTriforceNum_l() == 8 && isEventBit_l(0x2C01) == 0)
            return layer | 4;
        if (isEventBit_l(0x3280))
            return layer | 2;
        return isEventBit_l(0x3B40) ? (layer | 6) : layer;
    }
    if (startStageIs(gabi::guest_addr("kenroom"))) {
        if (isEventBit_l(0x2C01))
            return layer | 6;
        if (isEventBit_l(0x3802) && isEventBit_l(0x3280) == 0)
            return layer | 6;
        if (getTriforceNum_l() == 8)
            return layer | 4;
        return isEventBit_l(0x3802) ? (layer | 2) : layer;
    }
    if (startStageIs(gabi::guest_addr("M2tower")))
        return isEventBit_l(0x2D01) ? (layer | 2) : layer;
    if (startStageIs(gabi::guest_addr("GanonK")))
        return isEventBit_l(0x3B02) == 0 ? 8 : layer;
    if (startStageIs(gabi::guest_addr("GTower")))
        return isEventBit_l(0x4002) == 0 ? 8 : layer;
    return layer;
}

/* ---------------- HD: compiler-generated and template code of this translation unit
 * ---------------- */
/* 02525A48 */
void hdGameInfo___sinit_d_com_inf_game_cpp() {
    sinit_header_statics(0x1046F090, 0x101D5E6C);
}

// dSv_memory_c::dSv_memory_c (per-TU copy)
/* 02525ADC */
u32 hdGameInfo_dSv_memory_c_ct(u32 i_this) {
    if (i_this == 0) {
        i_this = operator_new_l(0x24);
        if (i_this == 0)
            return 0;
    }
    gameInfo2_memoryInit(i_this); /* dSv_memory_c::init */
    return i_this;
}

// sead::BufferedSafeString::assureTerminationImpl (per-TU copy)
/* 02525B24 */
void hdGameInfo_BufferedSafeString_assureTermination(u32 i_this) {
    u32 top = GameInfoMemory2::ld(i_this + 0);
    u32 size = GameInfoMemory2::ld(i_this + 8);
    GameInfoMemory2::st8(top + size - 1, 0);
}

static inline void dl_trivial(u32 i_this, s32 i_flag) {
    if (i_this == 0)
        return;
    if ((i_flag & 1) == 0)
        return;
    __dl_l(i_this);
}

// deleting destructor (no members)
/* 02525B3C */
void hdGameInfo_BufferedSafeStringDestructor(u32 i_this, s32 i_flag) {
    dl_trivial(i_this, i_flag);
}

// gameInfo_phase_3
/* 02525B50 */
s32 hdGameInfo_phase_3(u32 i_arcName) {
    return 4;
}

// deleting destructor (no members)
/* 02525B58 */
void hdGameInfo_SafeStringDestructor(u32 i_this, s32 i_flag) {
    dl_trivial(i_this, i_flag);
}

// deleting destructor of the 0x18-byte array element (member at +0xC)
/* 02525B6C */
void hdGameInfo_ArchiveNameEntryDestructor(u32 i_this, s32 i_flag) {
    if (i_this == 0)
        return;
    gameInfo2_stringDestructor(i_this + 0xC, 2);
    if (i_flag & 1)
        __dl_l(i_this);
}

// deleting destructor (no members)
/* 02525BC0 */
void hdGameInfo_FixedSafeStringDestructor(u32 i_this, s32 i_flag) {
    dl_trivial(i_this, i_flag);
}

// dComIfG_inf_c::~dComIfG_inf_c
/* 02525BD4 */
void hdGameInfo_dComIfG_inf_c_dt(u32 i_this, s32 i_flag) {
    if (i_this == 0)
        return;
    gameInfo2_drawListDestructor(i_this + 0x5D30, 2); /* ~dDlst_list_c */
    hdGameInfo_dComIfG_play_c_dt(i_this + 0x12A0, 2);
    if (i_flag & 1)
        __dl_l(i_this);
}

// sead::SafeString::assureTerminationImpl (empty, per-TU copy)
/* 02525C34 */
void hdGameInfo_SafeString_assureTermination(u32 i_this) {}

// deleting destructor (no members)
/* 02525C38 */
void hdGameInfo_SafeStringDestructorCopy(u32 i_this, s32 i_flag) {
    dl_trivial(i_this, i_flag);
}

// sead::WSafeString::assureTerminationImpl (empty, per-TU copy)
/* 02525C4C */
void hdGameInfo_WSafeString_assureTermination(u32 i_this) {}

// sead::WBufferedSafeString::assureTerminationImpl
/* 02525C50 */
void hdGameInfo_WBufferedSafeString_assureTermination(u32 i_this) {
    u32 size = GameInfoMemory2::ld(i_this + 8);
    u32 top = GameInfoMemory2::ld(i_this + 0);
    GameInfoMemory2::st16(top + (size << 1) - 2, 0);
}

// deleting destructor (no members)
/* 02525C6C */
void hdGameInfo_WBufferedSafeStringDestructor(u32 i_this, s32 i_flag) {
    dl_trivial(i_this, i_flag);
}

// deleting destructor (no members)
/* 02525C80 */
void hdGameInfo_WFixedSafeStringDestructor(u32 i_this, s32 i_flag) {
    dl_trivial(i_this, i_flag);
}

/* 02525C94: the last of the
 * unit's out-of-line inline copies (after its __sinit 02525A48; the next TU is the sinit-only TU
 * 02525F50). out (4x4) = a (3x4, extended by the row 0 0 0 1) * b (4x4): rows 0..2 are the
 * products, row 3 is b's row 3. Probably sead::Matrix44CalcCommon<f32>::multiply(Matrix44&,
 * const Matrix34&, const Matrix44&). Called by gameInfo_dComIfGd_getProjViewMtx (02524628) and, as
 * the kept COMDAT copy, from d_s_play and two other units. All loads come before the first store;
 * each element is a1*b1j, then + a0*b0j, + a2*b2j, + a3*b3j (fmadds). */
/* 02525C94 */
void hdGameInfo_mtx34x44Multiply(GameInfoHD::Matrix44* result, const GameInfoHD::Matrix34* matrixA,
                                 const GameInfoHD::Matrix44* matrixB) {
    f32 r[12];
    for (u32 i = 0; i < 3; i++)
        for (u32 j = 0; j < 4; j++) {
            f32 v = gabi::fmuls_ppc((f32)matrixA->m[i][1], (f32)matrixB->m[1][j]);
            v = gabi::fmadds((f32)matrixA->m[i][0], (f32)matrixB->m[0][j], v);
            v = gabi::fmadds((f32)matrixA->m[i][2], (f32)matrixB->m[2][j], v);
            v = gabi::fmadds((f32)matrixA->m[i][3], (f32)matrixB->m[3][j], v);
            r[i * 4 + j] = v;
        }
    for (u32 k : {0u, 2u, 3u, 6u, 1u, 7u, 4u, 5u, 8u, 10u, 9u, 11u})
        result->m[k / 4][k % 4] = r[k];
    for (u32 k = 12; k < 16; k++)
        result->m[k / 4][k % 4] = (f32)matrixB->m[k / 4][k % 4];
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
ext 0252EBA0 void gameInfo2_drawListDestructor(u32,u32)
ext 0200B708 void gameInfo2_stringDestructor(u32,u32)
ext 025B9170 void gameInfo2_memoryInit(u32)
ext 02556BC0 u32 gameInfo2_checkEventNightStop()
ext 02556C34 s32 gameInfo2_getDaytimeHour()
ext 025F0270 void gameInfo2_scopedHeapDestructor(u32,u32)
ext 025F02D0 void gameInfo2_scopedHeapFailed(u32)
ext 025F01D8 void gameInfo2_scopedHeapConstructor(u32,u32,u32,u32)
ext 025F0148 void gameInfo2_deletePacketHeap(u32)
ext 0258C4C0 u32 gameInfo2_magmaConstructor(u32)
ext 025A7398 u32 gameInfo2_particleConstructor(u32)
ext 02544374 void gameInfo2_eventManagerRun(u32)
ext 02526118 void gameInfo2_setArrowKeepType(u32)
ext C0009988 void gameInfo2_OSBlockMove(u32,u32,u32,u32)
ext 02525C4C void gameInfo2_wideAssureTermination(u32)
ext 02520C0C u32 gameInfo2_checkGetItem(u32)
ext 0200B7C0 void gameInfo2_fixedStringDestructor(u32,u32)
ext 028F0164 void gameInfo2_destroyArray(u32,u32,u32,u32,u32,u32)
ext 0200B680 void gameInfo2_archiveNameDestructor(u32,u32)
ext 0200B600 void gameInfo2_archiveStringDestructor(u32,u32)
ext 025ABCA8 void gameInfo2_admDestructor(u32,u32)
ext 025C4604 void gameInfo2_stageDataDestructor(u32,u32)
ext 024EDB20 void gameInfo2_attentionDestructor(u32,u32)
ext 025CC110 void gameInfo2_vibrationDestructor(u32,u32)
ext 0252A140 void gameInfo2_detectDestructor(u32,u32)
ext 02525C94 void gameInfo2_matrix34Multiply44(u32,u32,u32)
ext 0274D80C u32 gameInfo2_cameraProjectionMatrix(u32)
ext 02606BEC u32 gameInfo2_getStageArchiveResource(u32,u32,u32,u32)
ext 0260E8A8 void gameInfo2_deleteStageArchive(u32,u32,u32)
ext 0260EEE8 void gameInfo2_deleteExtraArchive(u32,u32,u32)
ext 0260E500 void gameInfo2_deleteObjectArchive(u32,u32)
ext 0260F4CC u32 gameInfo2_syncStageArchive(u32,u32,u32)
ext 0260D618 u32 gameInfo2_setStageArchive(u32,u32,u32,u32)
ext 0260D618 void gameInfo2_setStageArchive_discard(u32,u32,u32,u32)
ext 02525C34 void gameInfo2_assureTermination(u32)
ext 0260E0DC u32 gameInfo2_setExtraArchive(u32,u32,u32)
ext 0260CBB4 u32 gameInfo2_setObjectArchive(u32,u32,u32)
ext 02759C28 void gameInfo2_SafeString_format(u32,u32,u32)
ext 025B7E00 u32 gameInfo2_getTriforceNum(u32)
ext 025B8B94 u32 gameInfo2_isEventBit(u32,u32)
ext 025200D4 u32 gameInfo2_getGameInfo()
ext 0273AF40 void gameInfo2___dl(u32)
ext 0273AD10 u32 gameInfo2_operator_new(u32)
ext 0273AA24 u32 gameInfo2_JUT_ASSERT(u32,u32,u32)
str 1004BAC0 "Stage" in 02522F6C
str 1004BAD8 "ADMumi" in 02522F6C
str 1004BAB8 "A_umikz" in 02522F6C
str 1004BAE0 "ENDumi" in 02522F6C
str 1004BAC8 "sea_T" in 02522F6C
str 1004BAD0 "sea_E" in 02522F6C
str 1004BAE8 "Stage" in 02523318
str 1004BAF0 "ENDumi" in 02523318
str 1004BB00 "Stage" in 0252349C
str 1004BAF8 "M_NewD2" in 0252349C
str 1004BB08 "Siren" in 0252349C
str 1004BB10 "sea_T" in 0252349C
str 1004BB18 "M_NewD2" in 02523730
str 1004BB20 "sea_T" in 02523730
str 1004BB28 "Siren" in 02523730
str 1004BB30 "ma2room" in 02523A04
str 1004BB38 "ma3room" in 02523A04
str 1004BB54 "%s" in 02523A04
str 1004BB40 "sea" in 02523A04
str 1004BB44 "Stage" in 02523A04
str 1004BB58 "Hyrule" in 02523A04
str 1004BB4C "Room0" in 02523A04
str 1004BB60 "ma2room" in 02523D08
str 1004BB68 "ma3room" in 02523D08
str 1004BB70 "%s" in 02523D08
str 1004BB74 "M_NewD2" in 02523E9C
str 1004BB7C "sea_T" in 02523E9C
str 1004BB84 "Siren" in 02523E9C
str 1004BB8C "ma2room" in 02524180
str 1004BB94 "ma3room" in 02524180
str 1004BBB0 "%s" in 02524180
str 1004BB9C "sea" in 02524180
str 1004BBA0 "Stage" in 02524180
str 1004BBB4 "Hyrule" in 02524180
str 1004BBA8 "Room0" in 02524180
str 1004BBBC "ma2room" in 0252447C
str 1004BBC4 "ma3room" in 0252447C
str 1004BBCC "%s" in 0252447C
str 1004BC68 "sea" in 025250C0
str 1004BC74 "A_mori" in 025250C0
str 1004BC6C "Asoko" in 025250C0
str 1004BC7C "Hyrule" in 025250C0
str 1004BC84 "Hyroom" in 025250C0
str 1004BC58 "kenroom" in 025250C0
str 1004BC60 "M2tower" in 025250C0
str 1004BC8C "GanonK" in 025250C0
str 1004BC94 "GTower" in 025250C0
*/
