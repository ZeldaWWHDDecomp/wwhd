// JAudio sequence commands, checked against the Cafe command handlers.
#include "JSystem/JAudio/JASSeqParser.h"
using namespace JASystem;
extern u32 parserLoopEnd(TSeqCtrl*);
extern u32 parserReadPort(TTrack*, u32);
extern void parserWriteRegister(TTrack*, u32, u32);
extern void parserWritePort(TTrack*, u32, u32);
extern void parserWriteImport(TTrack*, u32, u32);
extern u32 parserTranspose(TTrack*);
extern void parserSimpleOsc(TTrack*, u32);
extern void parserSimpleEnv(TTrack*, u32, u32);
extern void parserCloseTrack(TTrack*);
extern void parserOuterSwitch(TOuterParam*, u32);
extern void parserOuterUpdate(TOuterParam*, u32);
extern void parserUpdateTrack(TTrack*, u32);
extern void parserConnectBus(TTrack*, u32, u32);
extern void parserSetInterrupt(TIntrMgr*, u32, void*);
extern void parserDisableInterrupt(TIntrMgr*, u32);
extern u32 parserReturnInterrupt(TSeqCtrl*);
extern void parserTryInterrupt(TTrack*);
/* 02846A00 */
s32 TSeqParser::cmdLoopE(TTrack* track, be<u32>*) {
    parserLoopEnd(&track->mSeqCtrl);
    return 0;
}
/* 02846A28 */
s32 TSeqParser::cmdReadPort(TTrack* track, be<u32>* args) {
    u32 value = parserReadPort(track, args[0]);
    // The destination is read after the port callback.
    u32 destination = (u8)(u32)args[1];
    parserWriteRegister(track, destination, value);
    return 0;
}
/* 02846A7C */
s32 TSeqParser::cmdWritePort(TTrack* track, be<u32>* args) {
    u32 value = (u16)(u32)args[1];
    parserWritePort(track, args[0], value);
    return 0;
}
/* 02846AE0 */
s32 TSeqParser::cmdWait(TTrack* track, be<u32>* args) {
    u32 duration = args[0];
    track->mSeqCtrl.mWaitTimer = duration;
    return duration != 0;
}
/* 02846AF4 */
s32 TSeqParser::cmdParentWritePort(TTrack* track, be<u32>* args) {
    u32 port = (u32)args[0] & 15;
    u32 value = (u16)(u32)args[1];
    parserWriteImport(track->mParent, port, value);
    return 0;
}
/* 02846B28 */
s32 TSeqParser::cmdChildWritePort(TTrack* track, be<u32>* args) {
    u32 route = (u8)(u32)args[0];
    u32 value = (u16)(u32)args[1];
    parserWriteImport(track->mChildren[route >> 4], route & 15, value);
    return 0;
}
/* 02846B64 */
s32 TSeqParser::cmdSetLastNote(TTrack* track, be<u32>* args) {
    u32 note = args[0];
    track->mLastNote = note + parserTranspose(track);
    return 0;
}
/* 02846BAC */
s32 TSeqParser::cmdTimeRelate(TTrack* track, be<u32>* args) {
    track->mTimeRelate = (u32)args[0] != 0;
    return 0;
}
/* 02846BC4 */
s32 TSeqParser::cmdSimpleOsc(TTrack* track, be<u32>* args) {
    parserSimpleOsc(track, (u8)(u32)args[0]);
    return 0;
}
/* 02846BF4 */
s32 TSeqParser::cmdSimpleEnv(TTrack* track, be<u32>* args) {
    parserSimpleEnv(track, (u8)(u32)args[0], args[1]);
    return 0;
}
/* 02846C94 */
s32 TSeqParser::cmdTranspose(TTrack* track, be<u32>* args) {
    track->mTranspose = (u32)args[0];
    return 0;
}
/* 02846CA4 */
s32 TSeqParser::cmdCloseTrack(TTrack* track, be<u32>* args) {
    u32 index = (u8)(u32)args[0];
    TTrack* child = track->mChildren[index];
    if (child) {
        parserCloseTrack(child);
        track->mChildren[index] = nullptr;
    }
    return 0;
}
/* 02846CF8 */
s32 TSeqParser::cmdOutSwitch(TTrack* track, be<u32>* args) {
    TOuterParam* outer = track->mOuterParam;
    if (outer) {
        parserOuterSwitch(outer, (u16)(u32)args[0]);
        parserOuterUpdate(outer, 0xFFFF);
    }
    return 0;
}
/* 02846D48 */
s32 TSeqParser::cmdUpdateSync(TTrack* track, be<u32>* args) {
    parserUpdateTrack(track, args[0]);
    return 0;
}
/* 02846D74 */
s32 TSeqParser::cmdBusConnect(TTrack* track, be<u32>* args) {
    parserConnectBus(track, args[0], args[1]);
    return 0;
}
/* 02846DA4 */
s32 TSeqParser::cmdPauseStatus(TTrack* track, be<u32>* args) {
    track->mPauseStatus = (u32)args[0];
    return 0;
}
/* 02846DB4 */
s32 TSeqParser::cmdSetInterrupt(TTrack* track, be<u32>* args) {
    u8* base = static_cast<u8*>(track->mSeqCtrl.mRawFilePtr.get());
    u32 offset = args[1];
    parserSetInterrupt(&track->mIntrMgr, args[0], base + offset);
    return 0;
}
/* 02846DEC */
s32 TSeqParser::cmdDisInterrupt(TTrack* track, be<u32>* args) {
    parserDisableInterrupt(&track->mIntrMgr, args[0]);
    return 0;
}
/* 02846E1C */
s32 TSeqParser::cmdClrI(TTrack* track, be<u32>*) {
    track->mIntrMgr.mEnabled = 1;
    track->mSeqCtrl.mPreviousFilePtr = nullptr;
    return 0;
}
/* 02846E30 */
s32 TSeqParser::cmdSetI(TTrack* track, be<u32>*) {
    track->mIntrMgr.mEnabled = 0;
    return 0;
}
/* 02846E3C */
s32 TSeqParser::cmdRetI(TTrack* track, be<u32>*) {
    track->mIntrMgr.mEnabled = 1;
    parserReturnInterrupt(&track->mSeqCtrl);
    parserTryInterrupt(track);
    return (s32)track->mSeqCtrl.mWaitTimer > 0;
}
/* 02846E8C */
s32 TSeqParser::cmdIntTimer(TTrack* track, be<u32>* args) {
    track->mIntrMgr.mTimerRepeats = (u32)args[0];
    u32 interval = args[1];
    track->mIntrMgr.mTimerInterval = interval;
    track->mIntrMgr.mTimerRemaining = interval;
    return 0;
}
/* 02846F00 */
s32 TSeqParser::cmdFlushAll(TTrack*, be<u32>*) { return 0; }
/* 02846F08 */
s32 TSeqParser::cmdFlushRelease(TTrack*, be<u32>*) { return 0; }
struct ParserIIRTable { be<s16> coefficients[256][4]; };
static ParserIIRTable sCutoffToIIR;
struct ParserInitialization { be<u32> words[4]; };
struct ParserRegistration { be<u32> words[3]; };
static ParserInitialization sParserInitialization;
static ParserRegistration sParserRegistration;
extern void parserSetFir(TOuterParam*, be<s16>*);
extern void parserInitOuter(TOuterParam*);
extern void parserAssignOuter(TTrack*, TOuterParam*);
extern void parserOscFull(TTrack*, u32, u32, u32);
extern void parserSetTempo(TTrack*, u32);
extern void parserSetTimebase(TTrack*, u32);
extern void parserRegisterGlobals(void*);
/* 02846FA8 */
s32 TSeqParser::cmdIIRSet(TTrack* track, be<u32>* args) {
    for (u32 i = 0; i < 4; ++i) {
        TTrack::TimedParam& parameter = track->mTimedParam[12 + i];
        f32 value = (f32)(s16)(u32)args[i] * (1.0f / 32768.0f);
        parameter.mTarget = value;
        parameter.mCurrent = value;
        parameter.mDelta = 0.0f;
        parameter.mDuration = 1.0f;
    }
    return 0;
}
/* 02847030 */
s32 TSeqParser::cmdFIRSet(TTrack* track, be<u32>* args) {
    u8* base = static_cast<u8*>(track->mSeqCtrl.mRawFilePtr.get());
    parserSetFir(track->mOuterParam, reinterpret_cast<be<s16>*>(base + (u32)args[0]));
    return 0;
}
/* 02847064 */
s32 TSeqParser::cmdEXTSet(TTrack* track, be<u32>* args) {
    u32 offset = args[0];
    u8* base = static_cast<u8*>(track->mSeqCtrl.mRawFilePtr.get());
    TOuterParam* outer = reinterpret_cast<TOuterParam*>(base + offset);
    parserInitOuter(outer);
    parserAssignOuter(track, outer);
    return 0;
}
/* 02847174 */
s32 TSeqParser::cmdVolumeMode(TTrack* track, be<u32>* args) {
    track->mVolumeMode = (u32)args[0];
    return 0;
}
/* 02847184 */
s32 TSeqParser::cmdVibDepth(TTrack* track, be<u32>* args) {
    u32 depth = (u8)(u32)args[0];
    track->mVibrate.mDepth = (f32)((depth << 8) | (depth << 1)) / 393204.0f;
    return 0;
}
/* 028471D8 */
s32 TSeqParser::cmdVibDepthMidi(TTrack* track, be<u32>* args) {
    track->mVibrate.mDepth = (f32)(u32)args[0] / 393204.0f;
    return 0;
}
/* 0284721C */
s32 TSeqParser::cmdVibPitch(TTrack* track, be<u32>* args) {
    u32 pitch = (u8)(u32)args[0];
    track->mVibrate.mPitch = (f32)(u16)((pitch << 8) | (pitch << 1)) / 294903.0f;
    return 0;
}
/* 02847270 */
s32 TSeqParser::cmdOscRoute(TTrack* track, be<u32>* args) {
    u32 route = (u8)(u32)args[0];
    track->mOscRoute[route >> 4] = route & 15;
    return 0;
}
/* 0284728C */
s32 TSeqParser::cmdIIRCutOff(TTrack* track, be<u32>* args) {
    u32 index = (u8)(u32)args[0];
    for (u32 i = 0; i < 4; ++i) {
        TTrack::TimedParam& parameter = track->mTimedParam[12 + i];
        f32 value = (f32)(s16)sCutoffToIIR.coefficients[index][i] / 32767.0f;
        parameter.mTarget = value;
        parameter.mCurrent = value;
        parameter.mDelta = 0.0f;
        parameter.mDuration = 1.0f;
    }
    return 0;
}
/* 0284731C */
s32 TSeqParser::cmdOscFull(TTrack* track, be<u32>* args) {
    parserOscFull(track, (u8)(u32)args[0], args[1], args[2]);
    return 0;
}
/* 0284754C */
s32 TSeqParser::cmdTempo(TTrack* track, be<u32>* args) {
    parserSetTempo(track, (u16)(u32)args[0]);
    return 0;
}
/* 0284757C */
s32 TSeqParser::cmdTimeBase(TTrack* track, be<u32>* args) {
    parserSetTimebase(track, (u16)(u32)args[0]);
    return 0;
}
/* 028475AC */
void __sinit_JASSeqParser_cpp() {
    sParserInitialization.words[2] = 0;
    sParserInitialization.words[0] = 0;
    sParserInitialization.words[3] = 0;
    sParserInitialization.words[1] = 0;
    parserRegisterGlobals(&sParserRegistration);
}

static TTrack::RegisterParam* parserRegisters(TTrack* track) {
    return &track->mRegisterParam;
}
struct ParserAdsrDefinition { be<u32> words[6]; };
static ParserAdsrDefinition sAdsrDefinition;
extern TTrack* parserOpenChild(TTrack*, u32, u32);
extern void parserStartTrack(TTrack*, void*, u32);
/* 02845E38 */
s32 TSeqParser::cmdWait(TTrack* track, u32 command) {
    u32 count = command == 0x80 ? 1 : 2;
    u32 duration = 0;
    for (u32 i = 0; i < count; ++i)
        duration = (duration << 8) | track->mSeqCtrl.readByte();
    track->mSeqCtrl.mWaitTimer = duration;
    return duration != 0;
}
/* 0284652C */
s32 TSeqParser::cmdOpenTrack(TTrack* track, be<u32>* args) {
    u32 route = args[0];
    u32 offset = args[1];
    TTrack* child = parserOpenChild(track, route & 15, (route >> 6) & 3);
    parserStartTrack(child, track->mSeqCtrl.mRawFilePtr, offset);
    return 0;
}
/* 02846584 */
s32 TSeqParser::cmdOpenTrackBros(TTrack* track, be<u32>* args) {
    TTrack* parent = track->mParent;
    if (parent) {
        u32 route = args[0];
        u32 offset = args[1];
        TTrack* child = parserOpenChild(parent, route & 15, (route >> 6) & 3);
        parserStartTrack(child, track->mSeqCtrl.mRawFilePtr, offset);
    }
    return 0;
}
/* 028465E4 */
s32 TSeqParser::conditionCheck(TTrack* track, u32 condition) {
    u32 flag = parserRegisters(track)->mFlag;
    switch (condition & 15) {
    case 0: return 1;
    case 1: return flag == 0;
    case 2: return flag != 0;
    case 3: return flag == 1;
    case 4: return flag >= 0x8000;
    case 5: return flag < 0x8000;
    default: return 0;
    }
}
/* 028469C8 */
s32 TSeqParser::cmdLoopS(TTrack* track, be<u32>* args) {
    u32 index = track->mSeqCtrl.mLoopIndex;
    void* current = track->mSeqCtrl.mCurrentFilePtr;
    u32 count = args[0];
    track->mSeqCtrl.mLoopStartPositions[index] = current;
    index = track->mSeqCtrl.mLoopIndex;
    track->mSeqCtrl.mLoopIndex = index + 1;
    track->mSeqCtrl.mLoopTimers[index] = count;
    return 0;
}
/* 02846AB0 */
s32 TSeqParser::cmdCheckPortImport(TTrack* track, be<u32>* args) {
    u32 port = args[0];
    parserRegisters(track)->mFlag = track->mTrackPort.mImportFlag[port];
    return 0;
}
/* 02846AC8 */
s32 TSeqParser::cmdCheckPortExport(TTrack* track, be<u32>* args) {
    u32 port = args[0];
    parserRegisters(track)->mFlag = track->mTrackPort.mExportFlag[port];
    return 0;
}
/* 02846C28 */
s32 TSeqParser::cmdSimpleADSR(TTrack* track, be<u32>* args) {
    for (u32 i = 0; i < 5; ++i)
        track->mOscConfigWords[i] = sAdsrDefinition.words[i];
    u32 finalWord = sAdsrDefinition.words[5];
    track->mOscConfigWords[2] = gabi::ea(track->mFilterCoefficients);
    track->mOscConfigWords[5] = finalWord;
    track->mFilterCoefficients[1] = (u32)args[0];
    track->mFilterCoefficients[4] = (u32)args[1];
    track->mFilterCoefficients[7] = (u32)args[2];
    track->mFilterCoefficients[8] = (u32)args[3];
    track->mTempoAccumulator = (u32)args[4];
    return 0;
}
/* 02846EA8 */
s32 TSeqParser::cmdSyncCPU(TTrack* track, be<u32>* args) {
    u32 result = 0xFFFF;
    u32 callback = sCallBackFunc;
    if (callback)
        result = gabi::call_ptr<u32>(callback, track, (u32)(u16)(u32)args[0]);
    parserRegisters(track)->mFlag = result;
    return 0;
}
/* 02846F10 */
s32 TSeqParser::cmdPanPowSet(TTrack* track, be<u32>* args) {
    TTrack::RegisterParam* registers = parserRegisters(track);
    for (u32 i = 0; i < 3; ++i)
        registers->mPanPower[i] = (u16)(u32)args[i];
    registers->mPanPower[3] = gabi::ftoi(gabi::fmuls_ppc((f32)(u32)args[3], 327.67f));
    registers->mPanPower[4] = gabi::ftoi(gabi::fmuls_ppc((f32)(u32)args[4], 327.67f));
    return 0;
}
/* 028470B8 */
s32 TSeqParser::cmdPanSwSet(TTrack* track, be<u32>* args) {
    const u8 calcTypes[7] = { 0, 0, 0, 1, 1, 2, 2 };
    const u8 parentCalcTypes[7] = { 0, 1, 2, 0, 2, 0, 2 };
    for (u32 i = 0; i < 3; ++i) {
        track->mCalcTypes[i] = calcTypes[(u32)args[i] >> 5];
        track->mParentCalcTypes[i] = parentCalcTypes[(u32)args[i] >> 5];
        track->mChannelState.mCalcTypes[i] = (u32)args[i] & 31;
        track->mUpdateFlags = (u32)track->mUpdateFlags | 0x1C;
    }
    return 0;
}
/* 02847354 */
s32 TSeqParser::cmdCheckWave(TTrack* track, be<u32>*) {
    parserRegisters(track)->mFlag = 0;
    return 0;
}
/* WWHD glue
data sAdsrDefinition 1017A764
ext 02818A60 JASystem::TTrack* parserOpenChild(JASystem::TTrack*,u32,u32)
ext 02818B0C void parserStartTrack(JASystem::TTrack*,void*,u32)
data sCutoffToIIR 101FC2FC
data sParserInitialization 104B6674
data sParserRegistration 101FC714
ext 02816D7C void parserSetFir(TOuterParam*,be<s16>*)
ext 02816C58 void parserInitOuter(TOuterParam*)
ext 028197FC void parserAssignOuter(JASystem::TTrack*,TOuterParam*)
ext 02819C64 void parserOscFull(JASystem::TTrack*,u32,u32,u32)
ext 0281896C void parserSetTempo(JASystem::TTrack*,u32)
ext 0281898C void parserSetTimebase(JASystem::TTrack*,u32)
ext 028F026C void parserRegisterGlobals(void*)
ext 02845A08 u32 parserLoopEnd(JASystem::TSeqCtrl*)
ext 02818CB0 u32 parserReadPort(JASystem::TTrack*,u32)
ext 02818EBC void parserWriteRegister(JASystem::TTrack*,u32,u32)
ext 02818CB8 void parserWritePort(JASystem::TTrack*,u32,u32)
ext 02818B50 void parserWriteImport(JASystem::TTrack*,u32,u32)
ext 028187E4 u32 parserTranspose(JASystem::TTrack*)
ext 02819DAC void parserSimpleOsc(JASystem::TTrack*,u32)
ext 02819D48 void parserSimpleEnv(JASystem::TTrack*,u32,u32)
ext 028182D0 void parserCloseTrack(JASystem::TTrack*)
ext 02816C68 void parserOuterSwitch(TOuterParam*,u32)
ext 02816C84 void parserOuterUpdate(TOuterParam*,u32)
ext 02817C60 void parserUpdateTrack(JASystem::TTrack*,u32)
ext 02818A50 void parserConnectBus(JASystem::TTrack*,u32,u32)
ext 0281A0D8 void parserSetInterrupt(JASystem::TIntrMgr*,u32,void*)
ext 0281A0FC void parserDisableInterrupt(JASystem::TIntrMgr*,u32)
ext 02845AD0 u32 parserReturnInterrupt(JASystem::TSeqCtrl*)
ext 02817A98 void parserTryInterrupt(JASystem::TTrack*)
*/
