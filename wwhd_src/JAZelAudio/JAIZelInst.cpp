/* JAIZelInst (WWHD): the Wind Waker conducting sounds (metronome, beat judging, melodies).
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/JAZelAudio/JAIZelInst.cpp) and verified against cking.rpx.
 *
 * Translation unit 02029E5C..0202ABA3 (17 functions), from the image: three sinit-only data TUs end
 * at 02029E5B; JAIZelInst runs the constructor .. getMelodyPattern, its header __sinit (0202AB08,
 * rodata 10003B00 after the unit's constants 10003A68..10003AE8), then two empty trailing copies.
 *
 * HD layout (0x34 bytes; GameCube offset in parentheses): +0 the six judged positions (0x18),
 * +6 their count (0x1E), +7 mBeat (0x1F), +8 volume (0x20), +0xC/+0x10/+0x14 beat parameters
 * (0x24..0x2C), +0x18 mBeatFrames (0x30), +0x1C arm note (0x34), +0x20..+0x2C sound handles
 * (0x38..0x44), +0x30 the 6/4 second-bar flag (0x48). HD drops the stick/arm-swing state
 * (GameCube 0x0..0x14): playArmSwing/stopArmSwing/setStickPos/play/melodyStop are gone or stubs
 * (0202A034 empty, 0202A038 returns -1).
 * Tables (.data): mCPosToNote3/4/61/62 at 1018DB98/DBA0/DBA8/DBB0, mMelodyPattern[8][7] at 1018DBB8. */
#include "bindings.h"

namespace JAIZelInst_cpp {
#include "jaizel_local.h"

static inline u32 se(u32 id, u32 info, f32 vol) {
    return JAIZelBasic_seStart_l(zel_basic(), id, 0, info, 0, 1.0f, vol, -1.0f, -1.0f, 0);
}
static inline void seStop_l(u32 id) { gabi::call(0x0201E538, zel_basic(), id, 0); }
static inline void jut_assert_l(u32 file, u32 line, u32 msg) { gabi::call(0x0273AA24, file, line, msg); }

/* 02029E5C JAIZelInst::JAIZelInst() */
u32 JAIZelInst_ct(u32 self) {
    WWHD_FUNC(0x02029E5C, u32, self);
    if (self == 0) {
        self = gabi::call<u32>(0x0273AD10, 0x34);
        if (self == 0) return 0;
    }
    gabi::call(0x028F521C, self, 6); /* the judged positions (zeroed) */
    st8(self + 0x30, 0);
    st8(self + 7, 3);
    stf(self + 0x10, 60.0f);
    stf(self + 8, 0.6f);
    stf(self + 0xC, 3.0f);
    st(self + 0x28, 0);
    stf(self + 0x14, 1.0f);
    st(self + 0x24, 0);
    st(self + 0x20, 0);
    st8(self + 0x1C, 0x37);
    st(self + 0x2C, 0);
    stf(self + 0x18, 30.0f);
    st8(self + 6, 0);
    return self;
}
VERIFY(0x02029E5C, JAIZelInst_ct);

/* 02029F10 JAIZelInst::reset() */
void reset(u32 self) {
    WWHD_FUNC(0x02029F10, void, self);
    stf(self + 0x14, 1.0f);
    st8(self + 0x1C, 0x37);
    st8(self + 7, 3);
    stf(self + 0xC, 3.0f);
    st8(self + 6, 0);
    stf(self + 0x18, 30.0f);
    stf(self + 8, 0.6f);
    stf(self + 0x10, 60.0f);
    st8(self + 0x30, 0);
    seStop_l(0x2867);
    seStop_l(0x887);
    seStop_l(0x888);
    seStop_l(0x8ED);
    seStop_l(0x8EE);
    seStop_l(0x893);
    seStop_l(0x894);
    seStop_l(0x895);
    st(self + 0x20, 0);
    st(self + 0x2C, 0);
    st(self + 0x24, 0);
    st(self + 0x28, 0);
}
VERIFY(0x02029F10, reset);

/* 0202A034: an empty member (probably one of the GameCube stick/arm-swing functions, folded) */
void inst_empty_0202A034(u32 self) {
    WWHD_FUNC(0x0202A034, void, self);
}
VERIFY(0x0202A034, inst_empty_0202A034);

/* 0202A038 JAIZelInst::setStickPos(s32, s32) (probably): HD stub, no melody from the stick */
s32 setStickPos(u32 self, s32 a, s32 b) {
    WWHD_FUNC(0x0202A038, s32, self, a, b);
    return -1;
}
VERIFY(0x0202A038, setStickPos);

/* 0202A040 JAIZelInst::setBeat(s32) */
void setBeat(u32 self, u32 r) {
    WWHD_FUNC(0x0202A040, void, self, r);
    switch (r) {
    case 0:
        st8(self + 7, 3);
        stf(self + 0xC, 3.0f);
        stf(self + 0x14, 1.0f);
        stf(self + 0x10, 60.0f);
        stf(self + 0x18, 30.0f * 1.0f);
        break;
    case 2:
        st8(self + 7, 6);
        stf(self + 0xC, 4.5f);
        stf(self + 0x10, 80.0f);
        stf(self + 0x14, 0.75f);
        stf(self + 0x18, 30.0f * 0.75f);
        break;
    case 4: {
        st8(self + 7, 4);
        f32 f = ldf(0x10003A94); /* 0.85714 */
        stf(self + 0xC, ldf(0x10003A8C)); /* 3.42857 */
        stf(self + 0x10, 70.0f);
        stf(self + 0x14, f);
        stf(self + 0x18, 30.0f * f);
        break;
    }
    }
}
VERIFY(0x0202A040, setBeat);

/* 0202A14C JAIZelInst::setVolume(f32) */
void setVolume(u32 self, f32 v) {
    WWHD_FUNC(0x0202A14C, void, self, v);
    f32 f = v / ldf(0x10003AA4); /* 0.7071 */
    if (f > 1.0f) f = 1.0f;
    else if (f < -1.0f) f = -1.0f;
    stf(self + 8, gabi::fmadds(f, 0.45f, 0.55f));
}
VERIFY(0x0202A14C, setVolume);

/* 0202A1B8 JAIZelInst::metronomePlay(s32 beat, s32 cpos) */
s32 metronomePlay(u32 self, u32 beat, u32 cpos) {
    WWHD_FUNC(0x0202A1B8, s32, self, beat, cpos);
    if (cpos >= 5) return -2;
    u32 b = ld8(self + 7);
    if (beat == 0) {
        if (b == 3) st(self + 0x28, se(0x888, 0, ldf(self + 8)));
        else if (b == 4) st(self + 0x28, se(0x8ED, 0, ldf(self + 8)));
        else if (b == 6) st(self + 0x28, se(0x8EE, 0, ldf(self + 8)));
        return -1;
    }
    if (b == 3) {
        if (beat == 1) return -1;
        if (beat != 2) return -2;
        st(self + 0x28, se(0x2868, 0, ldf(self + 8)));
    } else if (b == 4) {
        if (beat < 1) return -2;
        if (beat < 3) return -1;
        if (beat != 3) return -2;
        st(self + 0x28, se(0x2880, 0, ldf(self + 8)));
    } else if (b == 6) {
        if (beat < 1) return -2;
        if (beat < 5) return -1;
        if (beat != 5) return -2;
        st(self + 0x28, se(0x2881, 0, ldf(self + 8)));
    } else {
        return -2;
    }
    return -1;
}
VERIFY(0x0202A1B8, metronomePlay);

static inline u32 cposNote(u32 self, u32 b, u32 cpos) {
    if (b == 3) return ld8(0x1018DB98 + cpos);
    if (b == 4) return ld8(0x1018DBA0 + cpos);
    if (b == 6) return ld8(self + 0x30) != 0 ? ld8(0x1018DBB0 + cpos) : ld8(0x1018DBA8 + cpos);
    return 0;
}

/* 0202A3B4 JAIZelInst::judge(s32 beat, s32 cpos): the melody completed by this beat, or -1 / -2 */
s32 judge(u32 self, u32 beat, u32 cpos) {
    WWHD_FUNC(0x0202A3B4, s32, self, beat, cpos);
    if (cpos >= 5) return -2;
    u32 b = ld8(self + 7);
    u32 note = cposNote(self, b, cpos);
    if (b == 3) st(self + 0x2C, se(0x893, note, ldf(self + 8)));
    else if (b == 4) st(self + 0x2C, se(0x894, note, ldf(self + 8)));
    else if (b == 6) st(self + 0x2C, se(0x895, note, ldf(self + 8)));
    if (beat == 0) {
        u32 b7 = ld8(self + 7);
        st8(self + 6, 0);
        st8(self + 0x30, 0);
        if (b7 == 6 && cpos == 1) st8(self + 0x30, 1);
    }
    st(self + 0x24, se(0x887, (note + 0x6400) & 0xFFFF, ldf(self + 8)));
    u32 c = ld8(self + 6);
    if ((s32)beat == 5) st8(self + 0x30, 0);
    if (c < 6) {
        c = (c + 1) & 0xFF;
        st8(self + 6, c);
        st8(self + c - 1, cpos);
    } else {
        st8(self + 6, 6);
        for (u32 i = 0; i < 5; i++) st8(self + i, ld8(self + i + 1));
        st8(self + ld8(self + 6) - 1, cpos);
    }
    c = ld8(self + 6);
    u32 bt = ld8(self + 7);
    if (c < bt) return -1;
    for (u32 m = 0; m < 8; m++) {
        u32 e = 0x1018DBB8 + m * 7;
        if (bt != ld8(e)) continue;
        s32 k = c - bt;
        u32 j = 1;
        for (; k < (s32)c; k++) {
            if (ld8(self + k) != ld8(e + j)) break;
            j++;
        }
        if (j == 7) return m;
        if (ld8(e + j) == 0xFF) return m;
    }
    return -1;
}
VERIFY(0x0202A3B4, judge);

/* 0202A77C JAIZelInst::ambientPlay() */
void ambientPlay(u32 self) {
    WWHD_FUNC(0x0202A77C, void, self);
    se(0x2066, 0, ldf(self + 8));
    for (u32 o = 0x20; o <= 0x2C; o += 4) {
        u32 h = ld(self + o);
        if (h != 0 && ld(h) != 0) JAISound_setVolume_l(ld(h), ldf(self + 8), 3, 0);
    }
}
VERIFY(0x0202A77C, ambientPlay);

/* 0202A87C JAIZelInst::armSoundPlay(s32 cpos) */
void armSoundPlay(u32 self, u32 cpos) {
    WWHD_FUNC(0x0202A87C, void, self, cpos);
    st8(self + 0x1C, cposNote(self, ld8(self + 7), cpos));
}
VERIFY(0x0202A87C, armSoundPlay);

/* 0202A8F0 JAIZelInst::melodyPlay(s32) */
void melodyPlay(u32 self, s32 n) {
    WWHD_FUNC(0x0202A8F0, void, self, n);
    if (!(n < 8)) jut_assert_l(0x10003AB4, 0x3CC, 0x10003AC4); /* JUT_ASSERT(melody_num < MELODY_COUNT) */
    se(0x889 + n, 0, 1.0f);
}
VERIFY(0x0202A8F0, melodyPlay);

/* 0202A970 JAIZelInst::getMelodyBeat(s32) */
u32 getMelodyBeat(u32 self, s32 n) {
    WWHD_FUNC(0x0202A970, u32, self, n);
    if (!(n < 8)) jut_assert_l(0x10003AD8, 0x3A4, 0x10003AE8);
    return ld8(0x1018DBB8 + n * 7);
}
VERIFY(0x0202A970, getMelodyBeat);

/* 0202A9C8 JAIZelInst::getMelodyGFrames(s32) */
f32 getMelodyGFrames(u32 self, s32 n) {
    WWHD_FUNC(0x0202A9C8, f32, self, n);
    u32 b = getMelodyBeat(self, n);
    f32 f = 1.0f;
    if (b == 4) f = ldf(0x10003A94);
    else if (b == 6) f = 0.75f;
    return 30.0f * f;
}
VERIFY(0x0202A9C8, getMelodyGFrames);

/* 0202AA5C JAIZelInst::getMelodyPattern(s32 n, s32 step, s32* out) */
f32 getMelodyPattern(u32 self, s32 n, s32 step, u32 out) {
    WWHD_FUNC(0x0202AA5C, f32, self, n, step, out);
    if (step >= (s32)getMelodyBeat(self, n)) return 0.0f;
    u32 c = ld8(0x1018DBB9 + n * 7 + step);
    if (c == 0xFF) return 0.0f;
    st(out, c);
    return getMelodyGFrames(self, n);
}
VERIFY(0x0202AA5C, getMelodyPattern);

/* 0202AB08 __sinit_JAIZelInst_cpp (header statics) */
void __sinit_JAIZelInst_cpp() {
    WWHD_FUNC(0x0202AB08, void);
    header_sinit(0x101FFCD0, 0x1018DB74, 0x10003B00);
}
VERIFY(0x0202AB08, __sinit_JAIZelInst_cpp);

/* 0202AB9C / 0202ABA0: empty per-TU copies */
void inst_empty_0202AB9C(u32 a) {
    WWHD_FUNC(0x0202AB9C, void, a);
}
VERIFY(0x0202AB9C, inst_empty_0202AB9C);
void inst_empty_0202ABA0(u32 a) {
    WWHD_FUNC(0x0202ABA0, void, a);
}
VERIFY(0x0202ABA0, inst_empty_0202ABA0);

} // namespace JAIZelInst_cpp
