// JAudio sequence decoding and control-flow commands, reconstructed from Cafe.
#include "JSystem/JAudio/JASSeqParser.h"
using namespace JASystem;
using namespace gabi;
extern u32 parserExchangeRegister(TTrack*, u32);
extern u32 parserReadRegister16(TTrack*, u32);
extern u32 parserReadRegister32(TTrack*, u32);
extern u32 parserRead16(TSeqCtrl*);
extern u32 parserRead24(TSeqCtrl*);
extern u32 parserGet24(TSeqCtrl*, u32);
extern s32 parserCondition(TSeqParser*, TTrack*, u32);
extern void parserSequenceStart(TSeqCtrl*, void*, u32);
extern void parserNoteOff(TTrack*, u32, u32);
extern void parserSetParameter(TTrack*, u32, f32, s32);
extern s32 parserProcessCommand(TSeqParser*, TTrack*, u32, u32);
extern s32 parserProcessRegisterCommand(TSeqParser*, TTrack*, u32, u32);
extern s32 parserProcessNoteOn(TSeqParser*, TTrack*, u32);
extern s32 parserProcessNoteOff(TSeqParser*, TTrack*, u32);
extern s32 parserProcessWait(TSeqParser*, TTrack*, u32);
extern s32 parserProcessParameter(TSeqParser*, TTrack*, u32);
extern void parserWriteRegisterParameter(TTrack*, u32);
struct ParserArgumentFormat { be<u16> count; be<u16> types; };
struct ParserArgumentFormats { ParserArgumentFormat entries[64]; };
struct ParserCommandTable { ptmf<s32 (TSeqParser::*)(TTrack*, be<u32>*)> entries[64]; };
struct ParserArguments { be<u32> values[8]; };
static ParserArgumentFormats sArgumentFormats;
static ParserCommandTable sCommandTable;
/* 02845E88 */
s32 TSeqParser::cmdNoteOff(TTrack* track, u32 command) {
    if (command == 0xF9) {
        u32 flag = track->mSeqCtrl.readByte();
        command = (u8)(parserExchangeRegister(track, flag & 7) + 0x80);
        if (flag & 0x80)
            command |= 8;
    }
    u32 release = 0;
    if (command & 8) {
        release = track->mSeqCtrl.readByte();
        if (release > 100)
            release = release * 20 - 1960;
    }
    parserNoteOff(track, command & 7, (u16)release);
    return 0;
}
/* 02845F30 */
s32 TSeqParser::cmdSetParam(TTrack* track, u32 command) {
    u32 parameter = track->mSeqCtrl.readByte();
    s32 data;
    switch (command & 12) {
    case 0: data = (s16)parserReadRegister16(track, track->mSeqCtrl.readByte()); break;
    case 4: data = track->mSeqCtrl.readByte(); break;
    case 8: {
        u32 value = track->mSeqCtrl.readByte();
        data = (s16)((value & 0x80) ? value << 8 : (value << 8) | (value << 1));
        break;
    }
    default: data = (s16)parserRead16(&track->mSeqCtrl); break;
    }
    s32 duration;
    switch (command & 3) {
    case 0: duration = -1; break;
    case 1: duration = parserReadRegister16(track, track->mSeqCtrl.readByte()); break;
    case 2: duration = track->mSeqCtrl.readByte(); break;
    default: duration = parserRead16(&track->mSeqCtrl); break;
    }
    parserSetParameter(track, parameter, (f32)data / 32767.0f, duration);
    return 0;
}
/* 02846190 */
s32 TSeqParser::Cmd_Process(TTrack* track, u32 command, u32 registerTypes) {
    AutoFrame frame{current_function()};
    u32 index = command - 0xC0;
    u32 count = sArgumentFormats.entries[index].count;
    u32 types = (u32)sArgumentFormats.entries[index].types | registerTypes;
    FrameLocal<ParserArguments> arguments(0xC);
    for (u32 i = 0; i < count; ++i) {
        u32 value;
        switch (types & 3) {
        case 0: value = track->mSeqCtrl.readByte(); break;
        case 1: value = parserRead16(&track->mSeqCtrl); break;
        case 2: value = parserRead24(&track->mSeqCtrl); break;
        default: value = parserExchangeRegister(track, track->mSeqCtrl.readByte()); break;
        }
        arguments->values[i] = value;
        types >>= 2;
    }
    const auto& handler = sCommandTable.entries[index];
    if ((s16)handler.i == 0)
        return 0;
    return (this->*handler)(track, arguments->values);
}
/* 028462EC */
s32 TSeqParser::RegCmd_Process(TTrack* track, u32 indirect, u32 count) {
    u32 command = track->mSeqCtrl.readByte();
    if (indirect)
        command = (u8)parserExchangeRegister(track, command);
    u32 types = 0;
    if (!indirect || count != 0) {
        u32 flags = track->mSeqCtrl.readByte();
        u32 mask = 3;
        for (u32 i = 0; i < count + 1; ++i) {
            if (flags & 0x80)
                types |= mask;
            flags = (flags << 1) & 0xFE;
            mask = (mask << 2) & 0xFFFC;
        }
    }
    return parserProcessCommand(this, track, command, types);
}
/* 028463B4 */
s32 TSeqParser::parseSeq(TTrack* track) {
    for (;;) {
        u32 command = track->mSeqCtrl.readByte();
        s32 result;
        if (!(command & 0x80)) {
            result = parserProcessNoteOn(this, track, command);
        } else if ((command & 0xF0) == 0x80 && !(command & 7)) {
            result = parserProcessWait(this, track, command);
        } else if ((command & 0xF0) == 0x80 || command == 0xF9) {
            result = parserProcessNoteOff(this, track, command);
        } else {
            switch (command & 0xF0) {
            case 0x90: result = parserProcessParameter(this, track, command & 15); break;
            case 0xA0:
                parserWriteRegisterParameter(track, command & 15);
                continue;
            case 0xB0:
                result = parserProcessRegisterCommand(this, track, (command >> 3) & 1, command & 7);
                break;
            default: result = parserProcessCommand(this, track, command, 0); break;
            }
        }
        if (result == 1)
            return 0;
        if (result == 3)
            return -1;
    }
}
/* 0284666C */
s32 TSeqParser::cmdCall(TTrack* track, be<u32>*) {
    u32 flag = track->mSeqCtrl.readByte();
    u32 offset;
    if (flag & 0x80) {
        offset = parserReadRegister16(track, track->mSeqCtrl.readByte());
        if (flag & 0x40) {
            u32 table = (flag & 0x20)
                ? parserReadRegister16(track, track->mSeqCtrl.readByte())
                : parserRead24(&track->mSeqCtrl);
            offset = parserGet24(&track->mSeqCtrl, table + offset * 3);
        }
    } else {
        offset = parserRead24(&track->mSeqCtrl);
    }
    if (parserCondition(this, track, flag)) {
        u32 index = track->mSeqCtrl.mLoopIndex;
        void* current = track->mSeqCtrl.mCurrentFilePtr;
        track->mSeqCtrl.mLoopIndex = index + 1;
        track->mSeqCtrl.mLoopStartPositions[index] = current;
        u8* base = static_cast<u8*>(track->mSeqCtrl.mRawFilePtr.get());
        track->mSeqCtrl.mCurrentFilePtr = base + offset;
    }
    return 0;
}
/* 028467B0 */
s32 TSeqParser::cmdRet(TTrack* track, be<u32>* args) {
    if (parserCondition(this, track, (u8)(u32)args[0])) {
        u32 index = track->mSeqCtrl.mLoopIndex;
        if (!index)
            return 3;
        --index;
        track->mSeqCtrl.mLoopIndex = index;
        track->mSeqCtrl.mCurrentFilePtr = track->mSeqCtrl.mLoopStartPositions[index];
    }
    return 0;
}
/* 02846820 */
s32 TSeqParser::cmdJmp(TTrack* track, be<u32>*) {
    u32 flag = track->mSeqCtrl.readByte();
    u32 offset;
    bool replaceBase = false;
    if (flag & 0x80) {
        u32 reg = track->mSeqCtrl.readByte();
        if (flag & 0x40) {
            offset = parserReadRegister16(track, reg);
            u32 table = (flag & 0x20)
                ? parserReadRegister16(track, track->mSeqCtrl.readByte())
                : parserRead24(&track->mSeqCtrl);
            offset = parserGet24(&track->mSeqCtrl, table + offset * 3);
        } else {
            offset = parserReadRegister32(track, reg);
            replaceBase = reg >= 0x28 && reg <= 0x2B;
        }
    } else {
        offset = parserRead24(&track->mSeqCtrl);
    }
    if (parserCondition(this, track, flag)) {
        if (replaceBase)
            parserSequenceStart(&track->mSeqCtrl, at<void>(offset), 0);
        else {
            u8* base = static_cast<u8*>(track->mSeqCtrl.mRawFilePtr.get());
            track->mSeqCtrl.mCurrentFilePtr = base + offset;
        }
    }
    return 0;
}

extern u32 parserGetTranspose(TTrack*);
extern u32 parserDspTime(TTrack*, u32, u32);
extern void parserGateOn(TTrack*, u32, u32, u32, u32);
extern void parserNoteOn(TTrack*, u32, u32, u32, u32, u32);
extern void parserKeySweep(TChannel*, u32, u32);
/* 02845B2C */
s32 TSeqParser::cmdNoteOn(TTrack* track, u32 note) {
    u32 flag = track->mSeqCtrl.readByte();
    if (flag & 0x80)
        note = (u8)parserExchangeRegister(track, note);
    note = (u8)(note + parserGetTranspose(track));
    u32 connect = (flag >> 5) & 3;
    u32 sweepNote = 0;
    if (connect & 2) {
        sweepNote = note;
        note = track->mLastNote;
    }
    u32 velocity = track->mSeqCtrl.readByte();
    if (velocity & 0x80)
        velocity = (u8)parserExchangeRegister(track, velocity & 0x7F);
    u32 index = flag & 7;
    u32 skipSamples = 0;
    u32 scale;
    u32 duration;
    if (!index) {
        scale = track->mSeqCtrl.readByte();
        if (scale & 0x80)
            scale = (u8)parserExchangeRegister(track, scale & 0x7F);
        duration = 0;
        u32 count = (flag >> 3) & 3;
        for (u32 i = 0; i < count; ++i)
            duration = (duration << 8) | track->mSeqCtrl.readByte();
        if (count == 1 && (duration & 0x80))
            duration = parserExchangeRegister(track, duration & 0x7F);
    } else {
        if ((flag >> 3) & 3)
            index = (u8)parserExchangeRegister(track, index - 1);
        if (connect & 1) {
            skipSamples = parserExchangeRegister(track, track->mSeqCtrl.readByte());
            connect ^= 1;
        }
        scale = 100;
        duration = 0xFFFFFFFF;
    }
    u32 time = duration;
    u32 beforeTie = track->mBeforeTieMode;
    track->mConnectCase = connect;
    if (beforeTie) {
        if (connect & 1)
            time = 0xFFFFFFFF;
        else if (time != 0xFFFFFFFF)
            time = parserDspTime(track, time, scale);
        if (!(u32)track->mIsPaused || !((u32)track->mPauseStatus & 0x10))
            parserGateOn(track, index, note, velocity, time);
    } else {
        if (time != 0xFFFFFFFF) {
            time = parserDspTime(track, time, scale);
            connect = track->mConnectCase;
        }
        if (connect & 1)
            time = 0xFFFFFFFF;
        if (!(u32)track->mIsPaused || !((u32)track->mPauseStatus & 0x10))
            parserNoteOn(track, index, note, velocity, time, skipSamples);
    }
    connect = track->mConnectCase;
    track->mBaseTime = duration;
    track->mBeforeTieMode = connect & 1;
    if (connect & 2) {
        if (time == 0xFFFFFFFF)
            time = parserDspTime(track, duration, scale);
        TChannel* channel = track->mNoteChannels[0];
        if (channel)
            parserKeySweep(channel, sweepNote, time);
        note = sweepNote;
    }
    track->mLastNote = note;
    if (duration == 0xFFFFFFFF)
        return 0;
    track->mSeqCtrl.mWaitTimer = duration ? duration : 0xFFFFFFFF;
    return 1;
}
struct ParserPrintStorage { be<u8> types[4]; be<u32> values[4]; };
/* 02847360 */
s32 TSeqParser::cmdPrintf(TTrack* track, be<u32>*) {
    // HD: reporting is removed, but sequence reads and register lookups remain.
    AutoFrame frame{current_function()};
    FrameLocal<ParserPrintStorage> storage(8);
    u32 count = 0;
    u32 position = 0;
    do {
        u32 value = track->mSeqCtrl.readByte();
        if (!value)
            break;
        if (value == '\\') {
            if (!track->mSeqCtrl.readByte())
                break;
        } else if (value == '%') {
            ++position;
            value = track->mSeqCtrl.readByte();
            if (!value)
                break;
            switch (value) {
            case 'd': storage->types[count] = 0; break;
            case 'x': storage->types[count] = 1; break;
            case 's': storage->types[count] = 2; break;
            case 'r': storage->types[count] = 3; break;
            case 'R': storage->types[count] = 4; break;
            case 't': storage->types[count] = 5; break;
            default: break;
            }
            ++count;
        }
        ++position;
    } while (position < 128);
    for (u32 i = 0; i < count; ++i) {
        u32 value = track->mSeqCtrl.readByte();
        u32 type = storage->types[i];
        if (type == 2) {
            storage->values[i] = value;
            storage->values[i] = gabi::ea(track->mSeqCtrl.mRawFilePtr.get()) + value;
        } else if (type == 5) {
            storage->values[i] = track->mRoute;
        } else {
            storage->values[i] = value;
            if (type >= 3)
                storage->values[i] = parserExchangeRegister(track, value);
        }
    }
    return 0;
}
/* WWHD glue
ext 028187E4 u32 parserGetTranspose(JASystem::TTrack*)
ext 02819E6C u32 parserDspTime(JASystem::TTrack*,u32,u32)
ext 02819BEC void parserGateOn(JASystem::TTrack*,u32,u32,u32,u32)
ext 02819A44 void parserNoteOn(JASystem::TTrack*,u32,u32,u32,u32,u32)
ext 028153F4 void parserKeySweep(JASystem::TChannel*,u32,u32)
data sArgumentFormats 1017A7E4
data sCommandTable 1017A90C
ext 02819758 u32 parserExchangeRegister(JASystem::TTrack*,u32)
ext 02818CC0 u32 parserReadRegister16(JASystem::TTrack*,u32)
ext 02818E28 u32 parserReadRegister32(JASystem::TTrack*,u32)
ext 02845920 u32 parserRead16(JASystem::TSeqCtrl*)
ext 02845948 u32 parserRead24(JASystem::TSeqCtrl*)
ext 028459A0 u32 parserGet24(JASystem::TSeqCtrl*,u32)
ext 028465E4 s32 parserCondition(JASystem::TSeqParser*,JASystem::TTrack*,u32)
ext 02845910 void parserSequenceStart(JASystem::TSeqCtrl*,void*,u32)
ext 02818254 void parserNoteOff(JASystem::TTrack*,u32,u32)
ext 02819778 void parserSetParameter(JASystem::TTrack*,u32,f32,s32)
ext 02846190 s32 parserProcessCommand(JASystem::TSeqParser*,JASystem::TTrack*,u32,u32)
ext 028462EC s32 parserProcessRegisterCommand(JASystem::TSeqParser*,JASystem::TTrack*,u32,u32)
ext 02845B2C s32 parserProcessNoteOn(JASystem::TSeqParser*,JASystem::TTrack*,u32)
ext 02845E88 s32 parserProcessNoteOff(JASystem::TSeqParser*,JASystem::TTrack*,u32)
ext 02845E38 s32 parserProcessWait(JASystem::TSeqParser*,JASystem::TTrack*,u32)
ext 02845F30 s32 parserProcessParameter(JASystem::TSeqParser*,JASystem::TTrack*,u32)
ext 02819030 void parserWriteRegisterParameter(JASystem::TTrack*,u32)
*/
