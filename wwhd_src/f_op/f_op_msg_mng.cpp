/* f_op_msg_mng: message process manager helpers (fopMsgM), WWHD.
 *
 * Translation unit 025DB2BC..025DBBDC, from the image: f_op_msg (the msg_class process methods
 * 025DB050..025DB150 and its __sinit 025DB228) precedes it; the unit's own __sinit 025DBA44
 * follows MyPicture::drawFullSet2 and is followed by its per-TU inline copies (demoMsgFlagCheck
 * 025DBB40, a trivial deleting destructor 025DBB4C, J2DPane::calcMtx 025DBB60, the MyPicture
 * deleting destructor 025DBB84 and an empty virtual 025DBBD8); f_op_overlap starts at 025DBBDC.
 *
 * HD keeps only a small part of the GameCube f_op_msg_mng.cpp: the message windows were
 * rewritten, so most helpers are gone. The pane helpers no longer drive a J2DPane (cposMove only
 * updates the top-left corner, paneTrans ignores its offsets, setAlpha is empty), every pane
 * helper and the heap/picture helpers test their pointer first, and the drawing helpers return
 * early without a matrix. fopMsgM_create (matcher: createAppend) and fopMsgM_Timer_create
 * (matcher: createTimerAppend) have their append builders inlined.
 *
 * fopMsg_prm_class 0x1C (actor +0, pos +4, msgNo +0x10, +0x14, +0x18); fopMsg_prm_timer 0x34
 * (+0x1C mode, +0x20 limit time, +0x22 show type, +0x23 icon type, timer pos +0x24, rupee pos
 * +0x2C). fopMsgM_pane_class (HD): pos top-left +0xC, centre +0x1C, original size +0x24, size
 * +0x2C, init alpha +0x34, now alpha +0x35. */
#include "bindings.h"

namespace f_op_msg_mng_cpp {

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline u32 JUT_ASSERT_r(u32 file, s32 line, u32 msg) { return gabi::call<u32>(0x0273AA24, file, line, msg); }
static inline u32 fopScnM_SearchByID_l(u32 id) { return gabi::call<u32>(0x025DC80C, id); }
static inline BOOL fpcBs_Is_JustOfType_l(s32 a, s32 b) { return gabi::call<BOOL>(0x025DD258, a, b); }
static inline s32 fpcPi_Change_l(u32 pi, u32 layer, u32 listId, u32 listPrio) { return gabi::call<s32>(0x025E0D38, pi, layer, listId, listPrio); }
static inline u32 cMl_memalignB_l(s32 align, u32 size) { return gabi::call<u32>(0x02019430, align, size); }
static inline u32 fpcLy_CurrentLayer_l() { return gabi::call<u32>(0x025DED64); }
static inline s32 fpcSCtRq_Request_l(u32 layer, u32 name, u32 fn, u32 data, u32 append) { return gabi::call<s32>(0x025E14A8, layer, name, fn, data, append); }
static inline u32 mDoExt_getGameHeap_l() { return gabi::call<u32>(0x025E2FBC); }
static inline u32 JKRExpHeap_create_l(u32 size, u32 parent, BOOL err) { return gabi::call<u32>(0x027EBCB0, size, parent, err); }
static inline void __dl_l(u32 p) { gabi::call(0x0273AF40, p); }
static inline void J2DPicture_dt_l(u32 p, s32 flag) { gabi::call(0x027EF9AC, p, flag); }
static inline void J2DPicture_drawFullSet_l(u32 pic, f32 x, f32 y, f32 w, f32 h, u32 binding, u32 mirror, u32 tumble, u32 mtx) {
    gabi::call(0x027EFC6C, pic, x, y, w, h, binding, mirror, tumble, mtx);
}
/* J2DPicture::drawTexCoord(x, y, w, h, s0..t3, mtx): twelve floats, the last four on the stack
 * (GHS stores them as doubles at SP+8, +0x10, +0x18, +0x20) */
static inline void J2DPicture_drawTexCoord_l(u32 pic, u32 mtx, f32 a1, f32 a2, f32 a3, f32 a4, f32 a5, f32 a6, f32 a7, f32 a8,
                                             f32 s9, f32 s10, f32 s11, f32 s12) {
    Cpu* c = gabi::cpu;
    c->r[3] = pic;
    c->r[4] = mtx;
    const f32 fr[8] = {a1, a2, a3, a4, a5, a6, a7, a8};
    for (int i = 0; i < 8; i++) c->f[i + 1].ps0 = c->f[i + 1].ps1 = (double)fr[i];
    u32 sp = c->r[1];
    c->r[1] = sp - 48;
    const f64 st[4] = {s9, s10, s11, s12};
    for (int i = 0; i < 4; i++) {
        u64 b;
        memcpy(&b, &st[i], 8);
        gabi::store<u64>(c->r[1] + 8 + 8 * i, b);
    }
    gmem_call(c, 0x027EFBCC, 0, 2, 8);
    c->r[1] = sp;
}
/* STControl (HD 0x28: vtable +0x24) */
static inline void STControl_setWaitParm_l(u32 p, s16 a, s16 b, s16 c, s16 d, f32 e, f32 f, s16 g, u16 h) {
    gabi::call(0x025885C4, p, a, b, c, d, e, f, g, h);
}
static inline void STControl_init_l(u32 p) { gabi::call(0x025885E8, p); }

static inline u32 ld(u32 a) { return gabi::load<u32>(a); }
static inline u8 ld8(u32 a) { return gabi::load<u8>(a); }
static inline f32 ldf(u32 a) { return gabi::load<f32>(a); }
static inline void st(u32 a, u32 v) { gabi::store<u32>(a, v); }
static inline void st16(u32 a, u16 v) { gabi::store<u16>(a, v); }
static inline void st8(u32 a, u8 v) { gabi::store<u8>(a, v); }
static inline void stf(u32 a, f32 v) { gabi::store<f32>(a, v); }

static constexpr u32 FILE_ = 0x10057BF4; /* "f_op_msg_mng.cpp" */
static constexpr u32 DEMO_FLAG = 0x101F3480;

/* 025DB2BC */
static s32 fopMsgM_setStageLayer(u32 i_proc) {
    WWHD_FUNC(0x025DB2BC, s32, i_proc);
    u32 stageProc = fopScnM_SearchByID_l(ld(0x1047E6C4) /* dStage_roomControl_c::mProcID */);
    if (stageProc == 0) {
        return (s32)JUT_ASSERT_r(FILE_, 0xC0, 0x10057C08 /* "stageProc != (0)" */);
    }
    u32 layer = 0xFFFFFFFF;
    if (fpcBs_Is_JustOfType_l(ld(0x101F3D60) /* g_fpcNd_type */, ld(stageProc + 0xB8)))
        layer = ld(stageProc + 0xCC);
    return fpcPi_Change_l(i_proc + 0x68, layer, 0xFFFD, 0xFFFD);
}
VERIFY(0x025DB2BC, fopMsgM_setStageLayer);

/* 025DB374: fopMsgM_create(procName, actor, pos, msgNo, p5, createFunc), createAppend inlined */
static s32 fopMsgM_create(u32 i_procName, u32 i_actor, u32 i_pos, u32 i_msgNo, u32 p5, u32 i_createFunc) {
    WWHD_FUNC(0x025DB374, s32, i_procName, i_actor, i_pos, i_msgNo, p5, i_createFunc);
    u32 params = cMl_memalignB_l(-4, 0x1C);
    if (params == 0) return -1;
    st(params + 0, i_actor);
    if (i_msgNo != 0) st(params + 0x10, ld(i_msgNo));
    if (p5 != 0) st(params + 0x14, ld(p5));
    if (i_pos != 0) {
        st(params + 4, ld(i_pos + 0));
        st(params + 8, ld(i_pos + 4));
        st(params + 0xC, ld(i_pos + 8));
    } else {
        stf(params + 4, 0.0f);
        stf(params + 8, 0.0f);
        stf(params + 0xC, 0.0f);
    }
    st(params + 0x18, 0xFFFFFFFF);
    return fpcSCtRq_Request_l(fpcLy_CurrentLayer_l(), i_procName, i_createFunc, 0, params);
}
VERIFY(0x025DB374, fopMsgM_create);

/* 025DB468: fopMsgM_Timer_create, createTimerAppend inlined */
static s32 fopMsgM_Timer_create(u32 i_procName, s32 i_mode, u16 i_limitTimeMs, u8 i_showType, u8 i_iconType,
                                f32 i_posX, f32 i_posY, f32 i_rupeePosX, f32 i_rupeePosY, u32 i_createFunc) {
    WWHD_FUNC(0x025DB468, s32, i_procName, i_mode, i_limitTimeMs, i_showType, i_iconType, i_posX, i_posY,
              i_rupeePosX, i_rupeePosY, i_createFunc);
    u32 req = cMl_memalignB_l(-4, 0x34);
    if (req != 0) {
        st(req + 0, 0);
        st(req + 0x10, 0);
        st(req + 0x14, 0);
        stf(req + 4, 0.0f);
        stf(req + 8, 0.0f);
        stf(req + 0xC, 0.0f);
        st(req + 0x18, 0xFFFFFFFF);
        st(req + 0x1C, (u32)i_mode);
        st16(req + 0x20, i_limitTimeMs);
        st8(req + 0x22, i_showType);
        st8(req + 0x23, i_iconType);
        stf(req + 0x24, i_posX);
        stf(req + 0x28, i_posY);
        stf(req + 0x2C, i_rupeePosX);
        stf(req + 0x30, i_rupeePosY);
    }
    if (req == 0) return -1;
    return fpcSCtRq_Request_l(fpcLy_CurrentLayer_l(), i_procName, i_createFunc, 0, req);
}
VERIFY(0x025DB468, fopMsgM_Timer_create);

/* 025DB58C */
static void fopMsgM_demoMsgFlagOn() {
    WWHD_FUNC(0x025DB58C, void);
    st8(DEMO_FLAG, 1);
}
VERIFY(0x025DB58C, fopMsgM_demoMsgFlagOn);

/* 025DB59C */
static void fopMsgM_demoMsgFlagOff() {
    WWHD_FUNC(0x025DB59C, void);
    st8(DEMO_FLAG, 0);
}
VERIFY(0x025DB59C, fopMsgM_demoMsgFlagOff);

/* 025DB5AC: HD-only, the first byte of entry i of an 8-byte table at 101F3484 */
static u8 fopMsgM_getTableByte(s32 i) {
    WWHD_FUNC(0x025DB5AC, u8, i);
    return ld8(0x101F3484 + (u32)i * 8);
}
VERIFY(0x025DB5AC, fopMsgM_getTableByte);

/* 025DB5C0: HD pane lookup, *o_pane = screen->search(tag) when found (fopMsgM_setPaneData without
 * the pane parts) */
static void fopMsgM_searchPane(u32 o_pane, u32 i_scrn, u32 i_tag) {
    WWHD_FUNC(0x025DB5C0, void, o_pane, i_scrn, i_tag);
    u32 pane = gabi::call_ptr<u32>(ld(ld(i_scrn + 0xC8) + 0x5C), i_scrn, i_tag);
    if (pane != 0 && o_pane != 0) st(o_pane, pane);
}
VERIFY(0x025DB5C0, fopMsgM_searchPane);

/* 025DB614: HD only updates the top-left corner (no J2DPane move/resize) */
static void fopMsgM_cposMove(u32 i_pane) {
    WWHD_FUNC(0x025DB614, void, i_pane);
    if (i_pane == 0) return;
    f32 cx = ldf(i_pane + 0x1C);
    f32 sx = ldf(i_pane + 0x2C);
    f32 cy = ldf(i_pane + 0x20);
    f32 x = gabi::fnmsubs(sx, 0.5f, cx);
    f32 sy = ldf(i_pane + 0x30);
    f32 y = gabi::fnmsubs(sy, 0.5f, cy);
    stf(i_pane + 0xC, x);
    stf(i_pane + 0x10, y);
}
VERIFY(0x025DB614, fopMsgM_cposMove);

/* 025DB648: HD ignores the offsets */
static void fopMsgM_paneTrans(u32 i_pane, f32 i_transX, f32 i_transY) {
    WWHD_FUNC(0x025DB648, void, i_pane, i_transX, i_transY);
    if (i_pane == 0) return;
    fopMsgM_cposMove(i_pane);
}
VERIFY(0x025DB648, fopMsgM_paneTrans);

/* 025DB654 */
static void fopMsgM_paneScaleXY(u32 i_pane, f32 i_scale) {
    WWHD_FUNC(0x025DB654, void, i_pane, i_scale);
    if (i_pane == 0) return;
    f32 x = ldf(i_pane + 0x24) * i_scale;
    f32 y = ldf(i_pane + 0x28) * i_scale;
    stf(i_pane + 0x2C, x);
    stf(i_pane + 0x30, y);
    fopMsgM_cposMove(i_pane);
}
VERIFY(0x025DB654, fopMsgM_paneScaleXY);

/* 025DB678: HD empty */
static void fopMsgM_setAlpha(u32 i_pane) {
    WWHD_FUNC(0x025DB678, void, i_pane);
}
VERIFY(0x025DB678, fopMsgM_setAlpha);

/* 025DB67C */
static void fopMsgM_setInitAlpha(u32 i_pane) {
    WWHD_FUNC(0x025DB67C, void, i_pane);
    if (i_pane == 0) return;
    st8(i_pane + 0x35, ld8(i_pane + 0x34));
}
VERIFY(0x025DB67C, fopMsgM_setInitAlpha);

/* 025DB690 */
static void fopMsgM_setNowAlpha(u32 i_pane, f32 i_alpha) {
    WWHD_FUNC(0x025DB690, void, i_pane, i_alpha);
    if (i_pane == 0) return;
    f32 a = (f32)ld8(i_pane + 0x34) * i_alpha;
    st8(i_pane + 0x35, (u8)gabi::ftoi(a));
}
VERIFY(0x025DB690, fopMsgM_setNowAlpha);

/* 025DB6E0 */
static void fopMsgM_setNowAlphaZero(u32 i_pane) {
    WWHD_FUNC(0x025DB6E0, void, i_pane);
    if (i_pane == 0) return;
    st8(i_pane + 0x35, 0);
}
VERIFY(0x025DB6E0, fopMsgM_setNowAlphaZero);

/* 025DB6F4 */
static f32 fopMsgM_valueIncrease(s32 i_max, s32 i_value, u8 i_mode) {
    WWHD_FUNC(0x025DB6F4, f32, i_max, i_value, i_mode);
    if (i_max <= 0) return 1.0f;
    if (i_value < 0) i_value = 0;
    else if (i_value > i_max) i_value = i_max;
    f32 v = (f32)i_value / (f32)i_max;
    f32 ret = v;
    switch (i_mode) {
    case 0:
        ret = v * v;
        break;
    case 1:
        ret = std_sqrtf(v);
        break;
    case 3: {
        f32 t = (v + v) - 1.0f;
        ret = (t + t) - 1.0f;
        break;
    }
    case 4: {
        u16 ang = (u16)gabi::ftoi((32768.0f * v) * 0.5f);
        f32 s = ldf(0x104A44F8 + (u32)(ang >> 3) * 8); /* JMASSin table */
        ret = s * s;
        break;
    }
    case 5: {
        u16 ang = (u16)gabi::ftoi((65535.0f * v) * 0.5f);
        f32 s = ldf(0x104A44F8 + (u32)(ang >> 3) * 8);
        ret = s * s;
        break;
    }
    default:
        break;
    }
    return ret;
}
VERIFY(0x025DB6F4, fopMsgM_valueIncrease);

/* 025DB8A4: HD names the heap */
static u32 fopMsgM_createExpHeap(u32 i_size) {
    WWHD_FUNC(0x025DB8A4, u32, i_size);
    u32 parent = mDoExt_getGameHeap_l();
    u32 heap = JKRExpHeap_create_l(i_size, parent, FALSE);
    if (heap != 0) st(heap + 0x10, 0x10057C5C /* "fopMsgM_createExpHeap" */);
    return heap;
}
VERIFY(0x025DB8A4, fopMsgM_createExpHeap);

/* 025DB8F4: heap->destroy() (vtable +0xC, slot 0x24) */
static void fopMsgM_destroyExpHeap(u32 i_heap) {
    WWHD_FUNC(0x025DB8F4, void, i_heap);
    if (i_heap == 0) return;
    gabi::call_ptr(ld(ld(i_heap + 0xC) + 0x24), i_heap);
}
VERIFY(0x025DB8F4, fopMsgM_destroyExpHeap);

/* 025DB90C: MyPicture::drawSelf(x, y, mtx); J2DPicture (HD): vtable +0xC8, bounds +8..+0x14,
 * global bounds +0x18, texture +0xCC, binding +0xDE, mirror/tumble +0xDF */
static void MyPicture_drawSelf(u32 i_this, f32 i_posx, f32 i_posy, u32 i_mtx) {
    WWHD_FUNC(0x025DB90C, void, i_this, i_posx, i_posy, i_mtx);
    if (i_mtx == 0) return;
    if (ld(i_this + 0xCC) == 0) return;
    f32 y = gabi::fadds_ppc(ldf(i_this + 0x1C), i_posy);
    f32 x = gabi::fadds_ppc(ldf(i_this + 0x18), i_posx);
    f32 w = gabi::fsubs_ppc(ldf(i_this + 0x10), ldf(i_this + 8));
    f32 h = gabi::fsubs_ppc(ldf(i_this + 0x14), ldf(i_this + 0xC));
    u8 flags = ld8(i_this + 0xDF);
    u32 binding = ld8(i_this + 0xDE);
    gabi::call_ptr(ld(ld(i_this + 0xC8) + 0x7C), i_this, x, y, w, h, binding, (u32)(flags & 3), (u32)((flags >> 2) & 1),
                   i_mtx);
}
VERIFY(0x025DB90C, MyPicture_drawSelf);

/* 025DB96C: MyPicture::drawFullSet2; texture coordinates s0 +0x124, t0 +0x128, s1 +0x12C,
 * t1 +0x130, flag +0x134 */
static void MyPicture_drawFullSet2(u32 i_this, f32 i_posx, f32 i_posy, f32 i_width, f32 i_height, u32 i_binding,
                                   u32 i_mirror, u32 i_tumble, u32 i_mtx) {
    WWHD_FUNC(0x025DB96C, void, i_this, i_posx, i_posy, i_width, i_height, i_binding, i_mirror, i_tumble, i_mtx);
    if (i_mtx == 0) return;
    f32 t1 = ldf(i_this + 0x130);
    f32 s1 = ldf(i_this + 0x12C);
    f32 t0 = ldf(i_this + 0x128);
    f32 s0 = ldf(i_this + 0x124);
    if (i_mirror & 2) {
        f32 tmp = s0;
        s0 = s1;
        s1 = tmp;
    }
    if (i_mirror & 1) {
        f32 tmp = t0;
        t0 = t1;
        t1 = tmp;
    }
    if (ld8(i_this + 0x134) != 0) {
        if (i_tumble == 0)
            J2DPicture_drawTexCoord_l(i_this, i_mtx, 0.0f, 0.0f, i_width, i_height, s0, t0, s1, t0, s0, t1, s1, t1);
        else
            J2DPicture_drawTexCoord_l(i_this, i_mtx, 0.0f, 0.0f, i_width, i_height, s0, t1, s0, t0, s1, t1, s1, t0);
    } else {
        J2DPicture_drawFullSet_l(i_this, i_posx, i_posy, i_width, i_height, i_binding, i_mirror, i_tumble, i_mtx);
    }
}
VERIFY(0x025DB96C, MyPicture_drawFullSet2);

/* 025DBA44: static initialisers (header statics, then the STControl object at 1048A4EC) */
static void __sinit_f_op_msg_mng_cpp() {
    WWHD_FUNC(0x025DBA44, void, (u32)0);
    sinit_header_statics_z(0x1048A4E0, 0x101F3664, 0x1048A514);
    u32 stc = 0x1048A4EC;
    st(stc + 0x24, 0x10050788); /* STControl vtable */
    STControl_setWaitParm_l(stc, 0xF, 0xF, 0, 0, 0.9f, 0.5f, 0, 0x2000);
    STControl_init_l(stc);
    __register_global_object(0x101F3688);
}
VERIFY(0x025DBA44, __sinit_f_op_msg_mng_cpp);

/* 025DBB40: per-TU inline copy */
static u8 fopMsgM_demoMsgFlagCheck() {
    WWHD_FUNC(0x025DBB40, u8);
    return ld8(DEMO_FLAG);
}
VERIFY(0x025DBB40, fopMsgM_demoMsgFlagCheck);

/* 025DBB4C: deleting destructor of a class without members (per-TU copy) */
static void dtor_trivial(u32 i_this, s32 i_flag) {
    WWHD_FUNC(0x025DBB4C, void, i_this, i_flag);
    if (i_this == 0) return;
    if ((i_flag & 1) == 0) return;
    __dl_l(i_this);
}
VERIFY(0x025DBB4C, dtor_trivial);

/* 025DBB60: J2DPane::calcMtx (per-TU copy): if (mpParent) makeMatrix(mTranslateX, mTranslateY) */
static void J2DPane_calcMtx(u32 i_this) {
    WWHD_FUNC(0x025DBB60, void, i_this);
    if (ld(i_this + 0xBC) == 0) return;
    u32 fn = ld(ld(i_this + 0xC8) + 0x64);
    f32 y = ldf(i_this + 0xC);
    f32 x = ldf(i_this + 8);
    gabi::call_ptr(fn, i_this, x, y);
}
VERIFY(0x025DBB60, J2DPane_calcMtx);

/* 025DBB84: MyPicture deleting destructor */
static void MyPicture_dt(u32 i_this, s32 i_flag) {
    WWHD_FUNC(0x025DBB84, void, i_this, i_flag);
    if (i_this == 0) return;
    J2DPicture_dt_l(i_this, 0);
    if ((i_flag & 1) == 0) return;
    __dl_l(i_this);
}
VERIFY(0x025DBB84, MyPicture_dt);

/* 025DBBD8: empty virtual (per-TU copy) */
static void empty_virtual(u32 i_this) {
    WWHD_FUNC(0x025DBBD8, void, i_this);
}
VERIFY(0x025DBBD8, empty_virtual);

} // namespace f_op_msg_mng_cpp
