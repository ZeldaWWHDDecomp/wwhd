/** Conducting sounds. */
#pragma once
#include "bindings.h"
struct JAISound;;
struct InstSoundHandle { gptr<JAISound> mpSound; };
struct JAIZelBasic;
struct JAIZelInst {
    // HD: stick/arm-swing state is absent; GameCube judged positions begin at 0x18.
    be<u8> field_0x18[6]; be<u8> field_0x1e, mBeat;
    be<f32> field_0x20, field_0x24, field_0x28, field_0x2c, mBeatFrames;
    be<u8> field_0x34; u8 _1D[3];
    gptr<InstSoundHandle> field_0x38, field_0x3c, field_0x40, field_0x44;
    be<u8> field_0x48; u8 _31[3];
    void reset(); s32 setStickPos(s32, s32); void setBeat(u32); void setVolume(f32);
    s32 metronomePlay(u32, u32); s32 judge(u32, u32); void ambientPlay();
    void armSoundPlay(u32); void melodyPlay(s32); u32 getMelodyBeat(s32);
    f32 getMelodyGFrames(s32); f32 getMelodyPattern(s32, s32, be<u32>*);
    u32 cposNote(u32 beat, u32 cpos);
};
WWHD_SIZE(JAIZelInst, 0x34);
WWHD_OFFSET(JAIZelInst, mBeat, 7);
WWHD_OFFSET(JAIZelInst, field_0x20, 8);
WWHD_OFFSET(JAIZelInst, mBeatFrames, 0x18);
WWHD_OFFSET(JAIZelInst, field_0x38, 0x20);
WWHD_OFFSET(JAIZelInst, field_0x48, 0x30);
struct InstNoteTable { be<u8> note[5]; };
struct InstMelody { be<u8> note[7]; };
struct InstMelodyTable { InstMelody rows[8]; };
WWHD_SIZE(InstNoteTable, 5);
WWHD_SIZE(InstMelody, 7);
WWHD_SIZE(InstMelodyTable, 56);
struct InstHeaderStatics {
    be<f32> negativePi, positivePi; u8 firstObject, secondObject, padding[2]; be<u32> zero[4];
};
struct InstPiConstants { be<f32> negativePi, positivePi; };
void* Inst_alloc(u32);
void Inst_clear(void*, u32);
InstSoundHandle* JAIZelBasic_seStart(JAIZelBasic*, u32, const cXyz*, u32, u32, u32, f32, f32, f32, f32);
void JAIZelBasic_seStop(JAIZelBasic*, u32, u32);
void JAISound_setVolume(JAISound*, u32, u32, f32);
void Inst_assert(const char*, u32, const char*);
void Inst_firstHeader_ct(u8*);
void Inst_secondHeader_ct(u8*);
enum { MELODY_COUNT = 8 };
enum {
 JA_SE_WTAKT_METRONOME=0x887, JA_SE_WTAKT_DRONE=0x888, JA_SE_WTAKT_DANCE01=0x889,
 JA_SE_WTAKT_PULSE_3=0x893, JA_SE_WTAKT_PULSE_4=0x894, JA_SE_WTAKT_PULSE_6=0x895,
 JA_SE_WTAKT_DRONE_4=0x8ED, JA_SE_WTAKT_DRONE_6=0x8EE, JA_SE_WTAKT_AMBIENT=0x2066,
 JA_SE_WTAKT_ARM_SWING=0x2867, JA_SE_WTAKT_AUFTAKT=0x2868,
 JA_SE_WTAKT_AUFTAKT_4=0x2880, JA_SE_WTAKT_AUFTAKT_6=0x2881
};

WWHD_OFFSET(JAIZelInst, field_0x1e, 6);
WWHD_OFFSET(JAIZelInst, field_0x24, 0xC);
WWHD_OFFSET(JAIZelInst, field_0x28, 0x10);
WWHD_OFFSET(JAIZelInst, field_0x2c, 0x14);
WWHD_OFFSET(JAIZelInst, field_0x34, 0x1C);
WWHD_OFFSET(JAIZelInst, field_0x3c, 0x24);
WWHD_OFFSET(JAIZelInst, field_0x40, 0x28);
WWHD_OFFSET(JAIZelInst, field_0x44, 0x2C);
WWHD_SIZE(InstSoundHandle, 4);
WWHD_OFFSET(InstHeaderStatics, zero, 0xC);
WWHD_SIZE(InstHeaderStatics, 0x1C);
WWHD_SIZE(InstPiConstants, 8);
