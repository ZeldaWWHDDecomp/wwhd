// JAudio 1 sequence command parser. Command words use guest byte order.
#pragma once
#include "JSystem/JAudio/JASTrack.h"
namespace JASystem {
class TSeqParser {
public:
    s32 cmdLoopE(TTrack* track, be<u32>* args);
    s32 cmdReadPort(TTrack* track, be<u32>* args);
    s32 cmdWritePort(TTrack* track, be<u32>* args);
    s32 cmdWait(TTrack* track, be<u32>* args);
    s32 cmdParentWritePort(TTrack* track, be<u32>* args);
    s32 cmdChildWritePort(TTrack* track, be<u32>* args);
    s32 cmdSetLastNote(TTrack* track, be<u32>* args);
    s32 cmdTimeRelate(TTrack* track, be<u32>* args);
    s32 cmdSimpleOsc(TTrack* track, be<u32>* args);
    s32 cmdSimpleEnv(TTrack* track, be<u32>* args);
    s32 cmdTranspose(TTrack* track, be<u32>* args);
    s32 cmdCloseTrack(TTrack* track, be<u32>* args);
    s32 cmdOutSwitch(TTrack* track, be<u32>* args);
    s32 cmdUpdateSync(TTrack* track, be<u32>* args);
    s32 cmdBusConnect(TTrack* track, be<u32>* args);
    s32 cmdPauseStatus(TTrack* track, be<u32>* args);
    s32 cmdSetInterrupt(TTrack* track, be<u32>* args);
    s32 cmdDisInterrupt(TTrack* track, be<u32>* args);
    s32 cmdClrI(TTrack* track, be<u32>* args);
    s32 cmdSetI(TTrack* track, be<u32>* args);
    s32 cmdRetI(TTrack* track, be<u32>* args);
    s32 cmdIntTimer(TTrack* track, be<u32>* args);
    s32 cmdFlushAll(TTrack* track, be<u32>* args);
    s32 cmdFlushRelease(TTrack* track, be<u32>* args);
    s32 cmdIIRSet(TTrack* track, be<u32>* args);
    s32 cmdFIRSet(TTrack* track, be<u32>* args);
    s32 cmdEXTSet(TTrack* track, be<u32>* args);
    s32 cmdVolumeMode(TTrack* track, be<u32>* args);
    s32 cmdVibDepth(TTrack* track, be<u32>* args);
    s32 cmdVibDepthMidi(TTrack* track, be<u32>* args);
    s32 cmdVibPitch(TTrack* track, be<u32>* args);
    s32 cmdOscRoute(TTrack* track, be<u32>* args);
    s32 cmdIIRCutOff(TTrack* track, be<u32>* args);
    s32 cmdOscFull(TTrack* track, be<u32>* args);
    s32 cmdTempo(TTrack* track, be<u32>* args);
    s32 cmdTimeBase(TTrack* track, be<u32>* args);
    s32 cmdOpenTrack(TTrack* track, be<u32>* args);
    s32 cmdOpenTrackBros(TTrack* track, be<u32>* args);
    s32 cmdLoopS(TTrack* track, be<u32>* args);
    s32 cmdCheckPortImport(TTrack* track, be<u32>* args);
    s32 cmdCheckPortExport(TTrack* track, be<u32>* args);
    s32 cmdSimpleADSR(TTrack* track, be<u32>* args);
    s32 cmdSyncCPU(TTrack* track, be<u32>* args);
    s32 cmdPanPowSet(TTrack* track, be<u32>* args);
    s32 cmdPanSwSet(TTrack* track, be<u32>* args);
    s32 cmdCheckWave(TTrack* track, be<u32>* args);
    s32 cmdWait(TTrack* track, u32 command);
    s32 conditionCheck(TTrack* track, u32 condition);
    s32 cmdNoteOn(TTrack* track, u32 note);
    s32 cmdNoteOff(TTrack* track, u32 command);
    s32 cmdSetParam(TTrack* track, u32 command);
    s32 Cmd_Process(TTrack* track, u32 command, u32 registerTypes);
    s32 RegCmd_Process(TTrack* track, u32 indirect, u32 count);
    s32 parseSeq(TTrack* track);
    s32 cmdCall(TTrack* track, be<u32>* args);
    s32 cmdRet(TTrack* track, be<u32>* args);
    s32 cmdJmp(TTrack* track, be<u32>* args);
    s32 cmdPrintf(TTrack* track, be<u32>* args);
};
}
void __sinit_JASSeqParser_cpp();

