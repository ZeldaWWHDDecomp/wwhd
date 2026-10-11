/** Wind Waker conducting sounds. Local only. */
#include "JAZelAudio/JAIZelInst.h"
static JAIZelBasic* l_audio;
static InstNoteTable mCPosToNote3;
static InstNoteTable mCPosToNote4;
static InstNoteTable mCPosToNote61;
static InstNoteTable mCPosToNote62;
static InstMelodyTable mMelodyPattern;
static const f32 l_fourBeatRate;
static const f32 l_fourBeatTime;
static const f32 l_volumeScale;
static InstHeaderStatics l_header;
static const InstPiConstants l_pi;
static InstSoundHandle* se(u32 id, u32 info, f32 vol) {
    return JAIZelBasic_seStart(l_audio, id, nullptr, info, 0, 0, 1.0f, vol, -1.0f, -1.0f);
}
static void seStop(u32 id) { JAIZelBasic_seStop(l_audio, id, 0); }
// HD: constructor can allocate the reduced object and clears the six-position buffer.
/* 02029E5C */
JAIZelInst* JAIZelInst_ct(JAIZelInst* self) {
    if (self == 0) {
        self = static_cast<JAIZelInst*>(Inst_alloc(0x34));
        if (self == 0) return nullptr;
    }
    Inst_clear(self, 6); /* the judged positions (zeroed) */
    self->field_0x48 = 0;
    self->mBeat = 3;
    self->field_0x28 = 60.0f;
    self->field_0x20 = 0.6f;
    self->field_0x24 = 3.0f;
    self->field_0x40 = nullptr;
    self->field_0x2c = 1.0f;
    self->field_0x3c = nullptr;
    self->field_0x38 = nullptr;
    self->field_0x34 = 0x37;
    self->field_0x44 = nullptr;
    self->mBeatFrames = 30.0f;
    self->field_0x1e = 0;
    return self;
}

/* 02029F10 */
void JAIZelInst::reset() {
    field_0x2c = 1.0f;
    field_0x34 = 0x37;
    mBeat = 3;
    field_0x24 = 3.0f;
    field_0x1e = 0;
    mBeatFrames = 30.0f;
    field_0x20 = 0.6f;
    field_0x28 = 60.0f;
    field_0x48 = 0;
    seStop(JA_SE_WTAKT_ARM_SWING);
    seStop(JA_SE_WTAKT_METRONOME);
    seStop(JA_SE_WTAKT_DRONE);
    seStop(JA_SE_WTAKT_DRONE_4);
    seStop(JA_SE_WTAKT_DRONE_6);
    seStop(JA_SE_WTAKT_PULSE_3);
    seStop(JA_SE_WTAKT_PULSE_4);
    seStop(JA_SE_WTAKT_PULSE_6);
    field_0x38 = nullptr;
    field_0x44 = nullptr;
    field_0x3c = nullptr;
    field_0x40 = nullptr;
}

// HD: folded empty member; its exact GameCube arm/stick counterpart is unconfirmed.
/* 0202A034 */
void JAIZelInst_emptyMember(u32 self) {
}

// HD: stick-position entry is a stub.
/* 0202A038 */
s32 JAIZelInst::setStickPos(s32 a, s32 b) {
    return -1;
}

// HD: input beat selector retains its full unsigned ABI width.
/* 0202A040 */
void JAIZelInst::setBeat(u32 r) {
    switch (r) {
    case 0:
        mBeat = 3;
        field_0x24 = 3.0f;
        field_0x2c = 1.0f;
        field_0x28 = 60.0f;
        mBeatFrames = 30.0f * 1.0f;
        break;
    case 2:
        mBeat = 6;
        field_0x24 = 4.5f;
        field_0x28 = 80.0f;
        field_0x2c = 0.75f;
        mBeatFrames = 30.0f * 0.75f;
        break;
    case 4: {
        mBeat = 4;
        f32 f = l_fourBeatRate; /* 0.85714 */
        field_0x24 = l_fourBeatTime; /* 3.42857 */
        field_0x28 = 70.0f;
        field_0x2c = f;
        mBeatFrames = 30.0f * f;
        break;
    }
    }
}

// HD: volume uses a fused multiply-add.
/* 0202A14C */
void JAIZelInst::setVolume(f32 v) {
    f32 f = v / l_volumeScale; /* 0.7071 */
    if (f > 1.0f) f = 1.0f;
    else if (f < -1.0f) f = -1.0f;
    field_0x20 = gabi::fmadds(f, 0.45f, 0.55f);
}

/* 0202A1B8 */
s32 JAIZelInst::metronomePlay(u32 beat, u32 cpos) {
    if (cpos >= 5) return -2;
    u32 b = mBeat;
    if (beat == 0) {
        if (b == 3) field_0x40 = se(JA_SE_WTAKT_DRONE, 0, field_0x20);
        else if (b == 4) field_0x40 = se(JA_SE_WTAKT_DRONE_4, 0, field_0x20);
        else if (b == 6) field_0x40 = se(JA_SE_WTAKT_DRONE_6, 0, field_0x20);
        return -1;
    }
    if (b == 3) {
        if (beat == 1) return -1;
        if (beat != 2) return -2;
        field_0x40 = se(JA_SE_WTAKT_AUFTAKT, 0, field_0x20);
    } else if (b == 4) {
        if (beat < 1) return -2;
        if (beat < 3) return -1;
        if (beat != 3) return -2;
        field_0x40 = se(JA_SE_WTAKT_AUFTAKT_4, 0, field_0x20);
    } else if (b == 6) {
        if (beat < 1) return -2;
        if (beat < 5) return -1;
        if (beat != 5) return -2;
        field_0x40 = se(JA_SE_WTAKT_AUFTAKT_6, 0, field_0x20);
    } else {
        return -2;
    }
    return -1;
}

u32 JAIZelInst::cposNote(u32 b, u32 cpos) {
    if (b == 3) return mCPosToNote3.note[cpos];
    if (b == 4) return mCPosToNote4.note[cpos];
    if (b == 6) return field_0x48 != 0 ? mCPosToNote62.note[cpos] : mCPosToNote61.note[cpos];
    return 0;
}

/* 0202A3B4 */
s32 JAIZelInst::judge(u32 beat, u32 cpos) {
    if (cpos >= 5) return -2;
    u32 b = mBeat;
    u32 note = cposNote(b, cpos);
    if (b == 3) field_0x44 = se(JA_SE_WTAKT_PULSE_3, note, field_0x20);
    else if (b == 4) field_0x44 = se(JA_SE_WTAKT_PULSE_4, note, field_0x20);
    else if (b == 6) field_0x44 = se(JA_SE_WTAKT_PULSE_6, note, field_0x20);
    if (beat == 0) {
        u32 b7 = mBeat;
        field_0x1e = 0;
        field_0x48 = 0;
        if (b7 == 6 && cpos == 1) field_0x48 = 1;
    }
    field_0x3c = se(JA_SE_WTAKT_METRONOME, (note + 0x6400) & 0xFFFF, field_0x20);
    u32 c = field_0x1e;
    if ((s32)beat == 5) field_0x48 = 0;
    if (c < 6) {
        c = (c + 1) & 0xFF;
        field_0x1e = c;
        field_0x18[c - 1] = cpos;
    } else {
        field_0x1e = 6;
        for (u32 i = 0; i < 5; i++) field_0x18[i] = (u8)field_0x18[i + 1];
        field_0x18[(u8)field_0x1e - 1] = cpos;
    }
    c = field_0x1e;
    u32 bt = mBeat;
    if (c < bt) return -1;
    for (u32 m = 0; m < 8; m++) {
        const InstMelody& melody = mMelodyPattern.rows[m];
        if (bt != melody.note[0]) continue;
        s32 k = c - bt;
        u32 j = 1;
        for (; k < (s32)c; k++) {
            if (field_0x18[k] != melody.note[j]) break;
            j++;
        }
        if (j == 7) return m;
        if (melody.note[j] == 0xFF) return m;
    }
    return -1;
}

/* 0202A77C */
void JAIZelInst::ambientPlay() {
    se(JA_SE_WTAKT_AMBIENT, 0, field_0x20);
    // HD: reload each handle and its sound after the preceding volume callback.
    InstSoundHandle* h = field_0x38;
    if (h && h->mpSound) JAISound_setVolume(h->mpSound, 3, 0, field_0x20);
    h = field_0x3c;
    if (h && h->mpSound) JAISound_setVolume(h->mpSound, 3, 0, field_0x20);
    h = field_0x40;
    if (h && h->mpSound) JAISound_setVolume(h->mpSound, 3, 0, field_0x20);
    h = field_0x44;
    if (h && h->mpSound) JAISound_setVolume(h->mpSound, 3, 0, field_0x20);
}

/* 0202A87C */
void JAIZelInst::armSoundPlay(u32 cpos) {
    field_0x34 = cposNote(mBeat, cpos);
}

/* 0202A8F0 */
void JAIZelInst::melodyPlay(s32 n) {
    if (!(n < 8)) Inst_assert(STR(0x10003AB4), 0x3CC, STR(0x10003AC4)); /* JUT_ASSERT(melody_num < MELODY_COUNT) */
    se(JA_SE_WTAKT_DANCE01 + n, 0, 1.0f);
}

/* 0202A970 */
u32 JAIZelInst::getMelodyBeat(s32 n) {
    if (!(n < 8)) Inst_assert(STR(0x10003AD8), 0x3A4, STR(0x10003AE8));
    return mMelodyPattern.rows[n].note[0];
}

/* 0202A9C8 */
f32 JAIZelInst::getMelodyGFrames(s32 n) {
    u32 b = getMelodyBeat(n);
    f32 f = 1.0f;
    if (b == 4) f = l_fourBeatRate;
    else if (b == 6) f = 0.75f;
    return 30.0f * f;
}

/* 0202AA5C */
f32 JAIZelInst::getMelodyPattern(s32 n, s32 step, be<u32>* out) {
    if (step >= (s32)getMelodyBeat(n)) return 0.0f;
    u32 c = mMelodyPattern.rows[n].note[step + 1];
    if (c == 0xFF) return 0.0f;
    *out = c;
    return getMelodyGFrames(n);
}

/* 0202AB08 */
void __sinit_JAIZelInst_cpp() {
    l_header.zero[2] = 0; l_header.zero[0] = 0;
    l_header.zero[3] = 0; l_header.zero[1] = 0;
    __register_global_object(0x1018DB74);
    f32 lo = l_pi.negativePi, hi = l_pi.positivePi;
    l_header.negativePi = lo; l_header.positivePi = hi;
    Inst_firstHeader_ct(&l_header.firstObject);
    __register_global_object(0x1018DB80);
    Inst_secondHeader_ct(&l_header.secondObject);
    __register_global_object(0x1018DB8C);
}

// HD: empty header helper copies.
/* 0202AB9C */
void Inst_headerObject_dt(u32 a) {
}
/* 0202ABA0 */
void Inst_headerObject_ct(u32 a) {
}



/* WWHD glue (tools/verify/autoglue.py)
data l_audio 101FFC78
data mCPosToNote3 1018DB98
data mCPosToNote4 1018DBA0
data mCPosToNote61 1018DBA8
data mCPosToNote62 1018DBB0
data mMelodyPattern 1018DBB8
data l_fourBeatRate 10003A94
data l_fourBeatTime 10003A8C
data l_volumeScale 10003AA4
data l_header 101FFCD0
data l_pi 10003B00
ext 0273AD10 void* Inst_alloc(u32)
ext 028F521C void Inst_clear(void*, u32)
ext 0201EBA0 InstSoundHandle* JAIZelBasic_seStart(JAIZelBasic*, u32, const cXyz*, u32, u32, u32, f32, f32, f32, f32)
ext 0201E538 void JAIZelBasic_seStop(JAIZelBasic*, u32, u32)
ext 0280B68C void JAISound_setVolume(JAISound*, u32, u32, f32)
ext 0273AA24 void Inst_assert(const char*, u32, const char*)
ext 028ED6F8 void Inst_firstHeader_ct(u8*)
ext 028EAB2C void Inst_secondHeader_ct(u8*)
*/
