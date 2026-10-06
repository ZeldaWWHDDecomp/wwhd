/* hd_text_unit: HD message text builder (expands MSBT text with its tags into the text box string:
 * fonts, colours, ruby, item/unit strings, numbers, timers), WWHD. Part 1: static-array destroy
 * helpers, sound/state tags and the tag-insertion helpers (025FA5C0..025FB9BB).
 * HD-only code (no GameCube source): written from the WWHD
 * code. TU range 025FA5C0..02601B5F (static initialiser 026018B0); library companions (not verified):
 * 02601A70, 02601A84, 02601A98, 02601AC8, 02601AFC, 02601B10, 02601B24, 02601B38, 02601B4C (sead
 * string deleting dtors), 02601AAC, 02601ADC (empty assureTermination), 02601AB0, 02601AE0
 * (Buffered / WBuffered assureTermination).
 *
 * Builder (4 bytes): +0 the hd_msg_text object being laid out. Message tags in the text are
 * 0x0E <group> <tag> <size> <params...> (u16 units) and 0x0F <group> <tag> (end tags).
 */
#include "hd_text_unit.h"

namespace hd_text_unit {

void destroy_025FA5C0() {
    WWHD_FUNC(0x025FA5C0, void);
    call<void>(0x028F0164, 0x1048D974u, 2u, 8u, 0x02601A70u, 0u, 0u);
}
VERIFY(0x025FA5C0, destroy_025FA5C0);
void destroy_025FA5E4() {
    WWHD_FUNC(0x025FA5E4, void);
    call<void>(0x028F0164, 0x1048D984u, 2u, 8u, 0x02601A70u, 0u, 0u);
}
VERIFY(0x025FA5E4, destroy_025FA5E4);
void destroy_025FA608() {
    WWHD_FUNC(0x025FA608, void);
    call<void>(0x028F0164, 0x1048D994u, 2u, 8u, 0x02601A70u, 0u, 0u);
}
VERIFY(0x025FA608, destroy_025FA608);
void destroy_025FA62C() {
    WWHD_FUNC(0x025FA62C, void);
    call<void>(0x028F0164, 0x1048D9A4u, 2u, 8u, 0x02601A70u, 0u, 0u);
}
VERIFY(0x025FA62C, destroy_025FA62C);
void destroy_025FA650() {
    WWHD_FUNC(0x025FA650, void);
    call<void>(0x028F0164, 0x1048D9B4u, 2u, 8u, 0x02601A70u, 0u, 0u);
}
VERIFY(0x025FA650, destroy_025FA650);
void destroy_025FA674() {
    WWHD_FUNC(0x025FA674, void);
    call<void>(0x028F0164, 0x1048D9C4u, 2u, 8u, 0x02601A70u, 0u, 0u);
}
VERIFY(0x025FA674, destroy_025FA674);
void destroy_025FA698() {
    WWHD_FUNC(0x025FA698, void);
    call<void>(0x028F0164, 0x1048D9D4u, 2u, 8u, 0x02601A70u, 0u, 0u);
}
VERIFY(0x025FA698, destroy_025FA698);
void destroy_025FA6BC() {
    WWHD_FUNC(0x025FA6BC, void);
    call<void>(0x028F0164, 0x1048D9E4u, 2u, 8u, 0x02601A70u, 0u, 0u);
}
VERIFY(0x025FA6BC, destroy_025FA6BC);
void destroy_025FA6E0() {
    WWHD_FUNC(0x025FA6E0, void);
    call<void>(0x028F0164, 0x1048D9F4u, 2u, 8u, 0x02601A70u, 0u, 0u);
}
VERIFY(0x025FA6E0, destroy_025FA6E0);
void destroy_025FA704() {
    WWHD_FUNC(0x025FA704, void);
    call<void>(0x028F0164, 0x1048DA04u, 2u, 8u, 0x02601A70u, 0u, 0u);
}
VERIFY(0x025FA704, destroy_025FA704);
void destroy_025FA728() {
    WWHD_FUNC(0x025FA728, void);
    call<void>(0x028F0164, 0x1048DA14u, 2u, 8u, 0x02601A70u, 0u, 0u);
}
VERIFY(0x025FA728, destroy_025FA728);
void destroy_025FA74C() {
    WWHD_FUNC(0x025FA74C, void);
    call<void>(0x028F0164, 0x1048DA24u, 2u, 8u, 0x02601A70u, 0u, 0u);
}
VERIFY(0x025FA74C, destroy_025FA74C);
void destroy_025FA770() {
    WWHD_FUNC(0x025FA770, void);
    call<void>(0x028F0164, 0x1048DA34u, 2u, 8u, 0x02601A70u, 0u, 0u);
}
VERIFY(0x025FA770, destroy_025FA770);
void destroy_025FA794() {
    WWHD_FUNC(0x025FA794, void);
    call<void>(0x028F0164, 0x1048DA44u, 2u, 8u, 0x02601A70u, 0u, 0u);
}
VERIFY(0x025FA794, destroy_025FA794);
void destroy_025FA7B8() {
    WWHD_FUNC(0x025FA7B8, void);
    call<void>(0x028F0164, 0x1048DA54u, 2u, 8u, 0x02601A70u, 0u, 0u);
}
VERIFY(0x025FA7B8, destroy_025FA7B8);
void destroy_025FA7DC() {
    WWHD_FUNC(0x025FA7DC, void);
    call<void>(0x028F0164, 0x1048DA64u, 2u, 8u, 0x02601A70u, 0u, 0u);
}
VERIFY(0x025FA7DC, destroy_025FA7DC);
void destroy_025FA800() {
    WWHD_FUNC(0x025FA800, void);
    call<void>(0x028F0164, 0x1048DA74u, 2u, 8u, 0x02601A70u, 0u, 0u);
}
VERIFY(0x025FA800, destroy_025FA800);
void destroy_025FA824() {
    WWHD_FUNC(0x025FA824, void);
    call<void>(0x028F0164, 0x1048DA84u, 2u, 8u, 0x02601A70u, 0u, 0u);
}
VERIFY(0x025FA824, destroy_025FA824);
void destroy_025FA848() {
    WWHD_FUNC(0x025FA848, void);
    call<void>(0x028F0164, 0x1048DAE4u, 10u, 8u, 0x02601A70u, 0u, 0u);
}
VERIFY(0x025FA848, destroy_025FA848);
void destroy_025FA86C() {
    WWHD_FUNC(0x025FA86C, void);
    call<void>(0x028F0164, 0x1048DB34u, 10u, 8u, 0x02601A70u, 0u, 0u);
}
VERIFY(0x025FA86C, destroy_025FA86C);
void destroy_025FA890() {
    WWHD_FUNC(0x025FA890, void);
    call<void>(0x028F0164, 0x1048DAA4u, 2u, 8u, 0x02601A70u, 0u, 0u);
}
VERIFY(0x025FA890, destroy_025FA890);
void destroy_025FA8B4() {
    WWHD_FUNC(0x025FA8B4, void);
    call<void>(0x028F0164, 0x1048DAB4u, 2u, 8u, 0x02601A84u, 0u, 0u);
}
VERIFY(0x025FA8B4, destroy_025FA8B4);
void destroy_025FA8D8() {
    WWHD_FUNC(0x025FA8D8, void);
    call<void>(0x028F0164, 0x1048DAC4u, 2u, 8u, 0x02601A70u, 0u, 0u);
}
VERIFY(0x025FA8D8, destroy_025FA8D8);

/* 025FA8FC: builder constructor */
u32 ctor(u32 p) {
    WWHD_FUNC(0x025FA8FC, u32, p);
    if (!p) {
        p = call<u32>(0x0273AD10, 4u);
        if (!p) return 0;
    }
    store<u32>(p, 0);
    return p;
}
VERIFY(0x025FA8FC, ctor);

/* 025FA938: the end-of-message sound (none for some entry kinds; with the room's reverb) */
void playEndSe(u32 self) {
    WWHD_FUNC(0x025FA938, void, self);
    const u32 e = load<u32>(msg(self) + 0x22C);
    if (!e) return;
    const u32 k = load<u8>(e + 1);
    if (k == 2 || k == 5 || k == 6 || k == 7 || k == 9 || k == 0xB || k == 0xD) return;
    const s32 room = s32(s8(load<u8>(0x1047E6C8)));
    if (room) {
        const u32 rev = call<u32>(0x02520540, u32(room));
        call<void>(0x025E1C3C, 0u, 0u, rev);
    } else {
        call<void>(0x025E1C3C, 0u, 0u, 0u);
    }
}
VERIFY(0x025FA938, playEndSe);

/* 025FA9E8: next message state (0xA when waiting for input, else 0xE) */
void setEndState(u32 self) {
    WWHD_FUNC(0x025FA9E8, void, self);
    const u32 m = msg(self);
    store<u32>(m, (load<u8>(m + 0x697) || load<u8>(m + 0x698)) ? 0xAu : 0xEu);
}
VERIFY(0x025FA9E8, setEndState);

/* 025FAA1C: at the text end: ends the message (sound, state) unless a wait/continue is pending */
u32 onTextEnd(u32 self, u32 ch) {
    WWHD_FUNC(0x025FAA1C, u32, self, ch);
    if (load<u16>(ch)) return 0;
    const u32 m = msg(self);
    if (load<u32>(m + 0x6B8)) return 1;
    if (load<u8>(m + 0x699)) {
        store<u8>(m + 0x699, 0);
        return 1;
    }
    if (load<u8>(m + 0x658)) return 1;
    if (!load<u8>(m + 0x696)) playEndSe(self);
    setEndState(self);
    return 1;
}
VERIFY(0x025FAA1C, onTextEnd);

/* a WFixedSafeString<32> tag on the stack, inserted at pos */
struct Tag32 {
    Local<u8[0x4C]> L;
    u32 hdr() const { return L.a; }
    u32 buf() const { return load<u32>(L.a); }
};

/* 025FAAB8: inserts a font tag (id, current scale in percent) */
s32 insertFontTag(u32 self, u32 w, s32 pos, u32 id) {
    WWHD_FUNC(0x025FAAB8, s32, self, w, pos, id);
    Tag32 T;
    wfixed(T.hdr(), kWFixed32Vt, 0x20);
    const u32 b = T.buf();
    store<u16>(b + 4, 1);
    store<u16>(b + 6, 6);
    store<u16>(b, 0xE);
    store<u16>(b + 8, u16(id));
    store<u16>(b + 2, 0);
    const f32 k = load<f32>(0x100E10B4);
    store<u16>(b + 0xA, u16(ftoi(fmuls_ppc(load<f32>(msg(self) + 0x8E0), k))));
    store<u16>(b + 0xC, u16(ftoi(fmuls_ppc(load<f32>(msg(self) + 0x8E4), k))));
    wcopyAt(w, pos, T.hdr(), 7);
    return 7;
}
VERIFY(0x025FAAB8, insertFontTag);

/* 025FAC54: font tag: switches the layout font (0xFFFF: back to the saved one) and inserts it */
s32 fontTag(u32 self, u32 w, s32 pos, u32 tag) {
    WWHD_FUNC(0x025FAC54, s32, self, w, pos, tag);
    const u32 m = msg(self);
    const u32 id = load<u16>(tag + 8);
    const u32 saved = load<u32>(m + 0x68C);
    const u32 cur = load<u32>(m + 0x690);
    if (id == 0xFFFF) {
        store<u32>(m + 0x68C, 0);
        store<u32>(msg(self) + 0x690, saved);
        return insertFontTag(self, w, pos, id);
    }
    const u32 f = call<u32>(0x025F36B8, load<u32>(0x101F4A50), id);
    if (!saved) store<u32>(msg(self) + 0x68C, cur);
    store<u32>(msg(self) + 0x690, f);
    return insertFontTag(self, w, pos, id);
}
VERIFY(0x025FAC54, fontTag);

/* 025FAD04: line scale tag (percent) */
s32 scaleTag(u32 self, u32 tag) {
    WWHD_FUNC(0x025FAD04, s32, self, tag);
    const f32 s = f32(f64(load<u16>(tag + 8))) / load<f32>(0x100E10B4);
    store<f32>(msg(self) + 0x8E8, s);
    return -1;
}
VERIFY(0x025FAD04, scaleTag);

/* 025FAD4C: inserts a colour tag (RGBA bytes as two u16) */
s32 insertColorTag(u32 self, u32 w, s32 pos, u32 rgba) {
    WWHD_FUNC(0x025FAD4C, s32, self, w, pos, rgba);
    Tag32 T;
    wfixed(T.hdr(), kWFixed32Vt, 0x20);
    const u32 b = T.buf();
    store<u16>(b + 6, 4);
    store<u16>(b, 0xE);
    store<u16>(b + 4, 3);
    store<u16>(b + 2, 0);
    store<u16>(b + 8, u16(load<u8>(rgba) | (load<u8>(rgba + 1) << 8)));
    store<u16>(b + 0xA, u16(load<u8>(rgba + 2) | (load<u8>(rgba + 3) << 8)));
    wcopyAt(w, pos, T.hdr(), 6);
    return 6;
}
VERIFY(0x025FAD4C, insertColorTag);

static u32 load4(u32 a) {
    return (u32(load<u8>(a)) << 24) | (u32(load<u8>(a + 1)) << 16) | (u32(load<u8>(a + 2)) << 8) | load<u8>(a + 3);
}
static void store4(u32 a, u32 v) {
    store<u8>(a, u8(v >> 24));
    store<u8>(a + 1, u8(v >> 16));
    store<u8>(a + 2, u8(v >> 8));
    store<u8>(a + 3, u8(v));
}

/* 025FAEC4: colour tag: palette colour 1..8 of the project (row by mode), else the default
 * colour; it becomes the current colour (+0x91D) */
s32 colorTag(u32 self, u32 w, s32 pos, u32 tag, u32 mode) {
    WWHD_FUNC(0x025FAEC4, s32, self, w, pos, tag, mode);
    const u32 prj = call<u32>(0x025F4BC0, load<u32>(0x101F4AE8));
    const u32 id = load<u16>(tag + 8);
    Local<u32> C;
    u32 c;
    if (id - 1 < 8) {
        u32 idx;
        if (mode < 1) idx = (id + 9) & 0xFFFF;
        else if (mode == 1) idx = (id + 0x12) & 0xFFFF;
        else if (mode == 2) idx = (id + 0x1B) & 0xFFFF;
        else idx = id;
        u32 p = load<u32>(prj + 8);
        if (idx < load<u32>(prj + 4)) p += idx << 2;
        c = load<u32>(p);
    } else {
        const u32 m = msg(self);
        c = load4(m + (load<u8>(m + 0x921) ? 0x919 : 0x915));
    }
    store4(msg(self) + 0x91D, c);
    store<u32>(C.a, c);
    return insertColorTag(self, w, pos, C.a);
}
VERIFY(0x025FAEC4, colorTag);

/* 025FB08C: colour tag with the palette row of the message kind */
s32 colorTagAuto(u32 self, u32 w, s32 pos, u32 tag) {
    WWHD_FUNC(0x025FB08C, s32, self, w, pos, tag);
    const u32 m = msg(self);
    if (load<u32>(m + 0x11C) - 0x42 <= 9) return colorTag(self, w, pos, tag, 0);
    const u32 e = load<u32>(m + 0x22C);
    if (e) {
        const u32 k = load<u8>(e + 1);
        if (k == 2 || k == 6 || k == 7 || k == 0xB) return colorTag(self, w, pos, tag, 1);
    }
    return colorTag(self, w, pos, tag, 3);
}
VERIFY(0x025FB08C, colorTagAuto);

/* 025FB10C: tag group 0 (1 font, 2 scale, 3 colour) */
s32 group0(u32 self, u32 w, s32 pos, u32 tag) {
    WWHD_FUNC(0x025FB10C, s32, self, w, pos, tag);
    const u32 t = load<u16>(tag + 4);
    if (t < 1) return -1;
    if (t == 1) return fontTag(self, w, pos, tag);
    if (t == 2) return s32(call<u32>(0x025FAD04, self, tag, u32(pos), tag));
    if (t == 3) return colorTagAuto(self, w, pos, tag);
    return -1;
}
VERIFY(0x025FB10C, group0);

s32 tagNoEndSe(u32 self) {
    WWHD_FUNC(0x025FB148, s32, self);
    store<u8>(msg(self) + 0x696, 1);
    return 0;
}
VERIFY(0x025FB148, tagNoEndSe);

s32 tagEndSe(u32 self) {
    WWHD_FUNC(0x025FB15C, s32, self);
    store<u8>(msg(self) + 0x696, 0);
    const u32 m = msg(self);
    if (s32(load<u32>(m + 0x654)) < s32(load<u32>(m + 0x650))) store<u32>(m + 0x654, load<u32>(m + 0x650));
    return 0;
}
VERIFY(0x025FB15C, tagEndSe);

/* 025FB18C: may a wait tag stop here (not yet stopped at this character)? */
u32 canStop(u32 self) {
    WWHD_FUNC(0x025FB18C, u32, self);
    const u32 m = msg(self);
    if (load<u8>(m + 0x658)) return 0;
    return s32(load<u32>(m + 0x65C)) < s32(load<u32>(m + 0x64C)) ? 1u : 0u;
}
VERIFY(0x025FB18C, canStop);

static void stopHere(u32 self, u32 tag, u32 flag) {
    u32 m = msg(self);
    store<u32>(m + 0x65C, load<u32>(m + 0x64C));
    store<u32>(msg(self) + 0x6B4, load<u16>(tag + 8));
    store<u8>(msg(self) + flag, 1);
}

/* 025FB1BC: wait tag (frames) */
s32 tagWait(u32 self, u32 w, s32 pos, u32 tag) {
    WWHD_FUNC(0x025FB1BC, s32, self, w, pos, tag);
    if (canStop(self)) stopHere(self, tag, 0x697);
    return 0;
}
VERIFY(0x025FB1BC, tagWait);

/* 025FB210: wait tag for kinds that also set the auto-advance flag (+0x69B) */
s32 tagWaitAuto(u32 self, u32 w, s32 pos, u32 tag) {
    WWHD_FUNC(0x025FB210, s32, self, w, pos, tag);
    const u32 m = msg(self);
    const u32 e = load<u32>(m + 0x22C);
    if (!e) return 0;
    const u32 k = load<u8>(e + 1);
    if (k < 8) {
        if (k <= 1 || k == 5) store<u8>(m + 0x69B, 1);
    } else if (k == 8 || k == 0xC || k == 0xE) {
        store<u8>(m + 0x69B, 1);
    }
    if (!canStop(self)) return 0;
    stopHere(self, tag, 0x697);
    return 0;
}
VERIFY(0x025FB210, tagWaitAuto);

/* 025FB2C8: wait-for-input tag */
s32 tagWaitInput(u32 self, u32 w, s32 pos, u32 tag) {
    WWHD_FUNC(0x025FB2C8, s32, self, w, pos, tag);
    if (canStop(self)) stopHere(self, tag, 0x698);
    return 0;
}
VERIFY(0x025FB2C8, tagWaitInput);

/* 025FB31C: speed tag (not in instant mode 0x14F) */
s32 tagSpeed(u32 self, u32 w, s32 pos, u32 tag) {
    WWHD_FUNC(0x025FB31C, s32, self, w, pos, tag);
    if (canStop(self)) {
        u32 m = msg(self);
        store<u32>(m + 0x65C, load<u32>(m + 0x64C));
        m = msg(self);
        if (load<u32>(m + 0x654) != 0x14F) store<u32>(m + 0x6AC, load<u16>(tag + 8));
    }
    return 0;
}
VERIFY(0x025FB31C, tagSpeed);

static s32 choiceTag(u32 self, u32 mode, u32 state) {
    store<u32>(msg(self) + 0x6B8, mode);
    store<u32>(msg(self), state);
    return 0;
}
s32 tagChoice2(u32 self) { WWHD_FUNC(0x025FB370, s32, self); return choiceTag(self, 1, 8); }
VERIFY(0x025FB370, tagChoice2);
s32 tagChoice3(u32 self) { WWHD_FUNC(0x025FB390, s32, self); return choiceTag(self, 1, 9); }
VERIFY(0x025FB390, tagChoice3);
s32 tagRuby(u32 self) { WWHD_FUNC(0x025FB3B0, s32, self); return choiceTag(self, 2, 0x14); }
VERIFY(0x025FB3B0, tagRuby);

/* 025FB3D0: ruby tag after another one (remembers that one was open) */
s32 tagRuby2(u32 self) {
    WWHD_FUNC(0x025FB3D0, s32, self);
    const u32 m = msg(self);
    store<u32>(m + 0x6BC, load<u32>(m + 0x6B8) != 0 ? 1u : 0u);
    store<u32>(msg(self) + 0x6B8, 2);
    store<u32>(msg(self), 0x14);
    return 0;
}
VERIFY(0x025FB3D0, tagRuby2);

s32 tagCapital(u32 self) {
    WWHD_FUNC(0x025FB404, s32, self);
    store<u8>(msg(self) + 0x922, 1);
    return 0;
}
VERIFY(0x025FB404, tagCapital);

static u16 rgbaHalf(u32 p) { return u16(load<u8>(p) | (load<u8>(p + 1) << 8)); }

/* 025FB418: inserts the pane-style tag (two colours, two sizes in percent) */
s32 insertPaneTag(u32 self, u32 w, s32 pos, u32 c1, u32 c2, u32 sx, u32 sy) {
    WWHD_FUNC(0x025FB418, s32, self, w, pos, c1, c2, sx, sy);
    Tag32 T;
    wfixed(T.hdr(), kWFixed32Vt, 0x20);
    const u32 b = T.buf();
    store<u16>(b, 0xE);
    store<u16>(b + 2, 1);
    store<u16>(b + 4, 0xC);
    store<u16>(b + 6, 0xC);
    store<u16>(b + 8, rgbaHalf(c1));
    store<u16>(b + 0xA, rgbaHalf(c1 + 2));
    store<u16>(b + 0xC, rgbaHalf(c2));
    store<u16>(b + 0xE, rgbaHalf(c2 + 2));
    store<u16>(b + 0x10, load<u16>(sx));
    store<u16>(b + 0x12, load<u16>(sy));
    wcopyAt(w, pos, T.hdr(), 10);
    return 10;
}
VERIFY(0x025FB418, insertPaneTag);

/* 025FB5D4: copies the style of the layout pane "T_TypeOfJob_00" (font, colours, size) */
s32 copyPaneStyle(u32 self, u32 w, s32 pos, u32 holder) {
    WWHD_FUNC(0x025FB5D4, s32, self, w, pos, holder);
    const u32 lay = load<u32>(load<u32>(holder + 0x124) + 0xC);
    s32 total = 0;
    if (lay) {
        staticStr(0x1048DBF0, 0x1048D880, 0x100E10C8, kSafeVt, 0x101F4C04);
        const u32 vt = load<u32>(lay + 8);
        vAssure(0x1048D880);
        const u32 pane = call_ptr<u32>(load<u32>(vt + 0x5C), lay, load<u32>(0x1048D880), 1u);
        if (!load<u32>(0x101FD89C)) {
            const u32 g = load<u32>(0x101FD888);
            store<u32>(0x101FD89C, 1);
            if (!g) {
                store<u32>(0x101FD888, 1);
                store<u32>(0x101FD8FC, 0);
            }
            store<u32>(0x101FD8F8, 0x101FD8FC);
        }
        bool ok = false;
        if (pane) {
            const u32 t0 = call_ptr<u32>(load<u32>(load<u32>(pane + 8) + 0x14), pane);
            for (u32 t = t0; t; t = load<u32>(t))
                if (t == 0x101FD8F8) {
                    ok = true;
                    break;
                }
        }
        if (ok) {
            const u32 font = call<u32>(0x02878098, pane);
            u32 id = 0;
            for (u32 i = 0; i < 7; i++)
                if (call<u32>(0x025F36B8, load<u32>(0x101F4A50), i) == font) {
                    id = i & 0xFFFF;
                    break;
                }
            const s32 n = insertFontTag(self, w, pos, id);
            Local<u8[0xC]> L; /* +0/+2 size x/y, +4 colour 1, +8 colour 2 */
            store4(L.a + 4, load4(pane + 0xAC));
            store4(L.a + 8, load4(pane + 0xB0));
            const u32 f2 = call<u32>(0x02878098, pane);
            const u32 fnH = load<u32>(load<u32>(f2 + 4) + 0x1C);
            const f32 k = load<f32>(0x100E10B4);
            const f32 a = fmuls_ppc(load<f32>(pane + 0xB8), k);
            const s32 h = s32(call_ptr<u32>(fnH, f2));
            store<u16>(L.a, u16(ftoi(a / f32(f64(h)))));
            const u32 fnW = load<u32>(load<u32>(f2 + 4) + 0x24);
            const f32 b = fmuls_ppc(load<f32>(pane + 0xBC), k);
            const s32 wd = s32(call_ptr<u32>(fnW, f2));
            store<u16>(L.a + 2, u16(ftoi(b / f32(f64(wd)))));
            total = n + insertPaneTag(self, w, pos + n, L.a + 4, L.a + 8, L.a, L.a + 2);
        }
    }
    store<u8>(msg(self) + 0x923, 1);
    return total;
}
VERIFY(0x025FB5D4, copyPaneStyle);

/* 025FB928: tag group 1 */
s32 group1(u32 self, u32 w, s32 pos, u32 tag, u32 a7, u32 a8) {
    WWHD_FUNC(0x025FB928, s32, self, w, pos, tag, a7, a8);
    static const u32 fns[13] = {0x025FB148, 0x025FB15C, 0x025FB1BC, 0x025FB210, 0x025FB2C8, 0, 0x025FB31C,
                                0x025FB370, 0x025FB390, 0x025FB3B0, 0x025FB3D0, 0x025FB404, 0x025FB5D4};
    const u32 t = load<u16>(tag + 4);
    if (t > 12 || t == 5) return 0;
    if (t == 12) return s32(call<u32>(fns[t], self, w, u32(pos), a8, a7, a8));
    return s32(call<u32>(fns[t], self, w, u32(pos), tag, a7, a8));
}
VERIFY(0x025FB928, group1);

} // namespace hd_text_unit
