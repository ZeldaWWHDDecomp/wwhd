/** Conducting sounds. */
#pragma once
#include "bindings.h"
struct JAISound;;
struct InstSoundHandle { gptr<JAISound> mpSound; };
struct JAIZelBasic;
struct JAIZelInst {
    // HD: stick/arm-swing state is absent; GameCube judged positions begin at 0x18.
    be<u8> mJudgedNotes[6]; be<u8> mJudgedNoteCount, mBeat;  /* was field_0x18: judge 0202A3B4: the last six conducted notes (cposNote), shifted when full and matched against mMelodyPattern; was field_0x1e: judge 0202A3B4: number of entries in mJudgedNotes (0 at beat 0, max 6) */
    be<f32> mVolume, field_0x24, field_0x28, mBeatRate, mBeatFrames;  /* was field_0x20: setVolume 0202A14C: 0.55 .. 1.0 from the stick; volume of every conducting SE (metronomePlay, judge, ambientPlay); was field_0x2c: setBeat 0202A040: 1.0 / 0.75 / 0.857 for 3 / 6 / 4 beats; mBeatFrames = 30 * it */
    be<u8> mArmNote; u8 _1D[3];  /* was field_0x34: armSoundPlay 0202A87C stores cposNote(mBeat, cpos); reset 02029F10 sets 0x37 */
    gptr<InstSoundHandle> field_0x38, mpMetronomeSound, mpDroneSound, mpPulseSound;  /* was field_0x3c: judge 0202A3B4: handle of JA_SE_WTAKT_METRONOME; ambientPlay 0202A77C sets its volume; was field_0x40: metronomePlay 0202A1B8: handle of JA_SE_WTAKT_DRONE(_4/_6) / JA_SE_WTAKT_AUFTAKT(_4/_6); ambientPlay 0202A77C sets its volume; was field_0x44: judge 0202A3B4: handle of JA_SE_WTAKT_PULSE_3/4/6; ambientPlay 0202A77C sets its volume */
    be<u8> mUseNote62; u8 _31[3];  /* was field_0x48: judge 0202A3B4 sets it for a 6 beat starting at cpos 1; cposNote then takes mCPosToNote62 instead of mCPosToNote61 */
    void reset(); s32 setStickPos(s32, s32); void setBeat(u32); void setVolume(f32);
    s32 metronomePlay(u32, u32); s32 judge(u32, u32); void ambientPlay();
    void armSoundPlay(u32); void melodyPlay(s32); u32 getMelodyBeat(s32);
    f32 getMelodyGFrames(s32); f32 getMelodyPattern(s32, s32, be<u32>*);
    u32 cposNote(u32 beat, u32 cpos);
};
WWHD_SIZE(JAIZelInst, 0x34);
WWHD_OFFSET(JAIZelInst, mBeat, 7);
WWHD_OFFSET(JAIZelInst, mVolume, 8);
WWHD_OFFSET(JAIZelInst, mBeatFrames, 0x18);
WWHD_OFFSET(JAIZelInst, field_0x38, 0x20);
WWHD_OFFSET(JAIZelInst, mUseNote62, 0x30);
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

WWHD_OFFSET(JAIZelInst, mJudgedNoteCount, 6);
WWHD_OFFSET(JAIZelInst, field_0x24, 0xC);
WWHD_OFFSET(JAIZelInst, field_0x28, 0x10);
WWHD_OFFSET(JAIZelInst, mBeatRate, 0x14);
WWHD_OFFSET(JAIZelInst, mArmNote, 0x1C);
WWHD_OFFSET(JAIZelInst, mpMetronomeSound, 0x24);
WWHD_OFFSET(JAIZelInst, mpDroneSound, 0x28);
WWHD_OFFSET(JAIZelInst, mpPulseSound, 0x2C);
WWHD_SIZE(InstSoundHandle, 4);
WWHD_OFFSET(InstHeaderStatics, zero, 0xC);
WWHD_SIZE(InstHeaderStatics, 0x1C);
WWHD_SIZE(InstPiConstants, 8);
