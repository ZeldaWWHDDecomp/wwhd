/* Sequence opcode framing and argument decoding, reconstructed from WWHD. */
#include "nw/snd/SequenceCommandReader.h"
using namespace nw::snd;
/* inlined byte fetch preserves the cursor store before the byte load. */
static u32 commandByte(SequenceCursor& cursor) {
    const be<u8>* bytes=cursor.mPosition;
    cursor.mPosition=gabi::at<const be<u8>>(gabi::ea(bytes)+1);
    return *bytes;
}
/* 028AAB94 */
u32 SequenceCommandReader::readCommand(SequenceCommandTrack_l* track,u32 emitNotes) {
    gabi::AutoFrame frame{gabi::current_function()};
    SequenceCommandState_l* state=sequenceCommandState(track);
    SequenceArgumentReader* arguments=reinterpret_cast<SequenceArgumentReader*>(this);
    SequenceCursor& cursor=state->mCursor;
    u32 opcode=commandByte(cursor);
    u32 enabled=1;
    u32 secondaryEncoding=0;
    u32 overrideEncoding=0;
    void* owner=track->player.get();
    if (opcode==0xA2) {
        enabled=state->mCondition;
        opcode=commandByte(cursor);
    }
    if (opcode==0xA3 || opcode==0xA4 || opcode==0xA5) {
        secondaryEncoding=opcode==0xA3 ? 2 : opcode==0xA4 ? 4 : 5;
        opcode=commandByte(cursor);
    }
    if (opcode==0xA0 || opcode==0xA1) {
        overrideEncoding=opcode==0xA0 ? 4 : 5;
        opcode=commandByte(cursor);
    }
    if ((opcode&0x80)==0) {
        u32 velocity=commandByte(cursor);
        s32 duration=arguments->readArgument(&cursor,owner,track,overrideEncoding ? overrideEncoding : 3);
        s32 key=s32(opcode)+s32(state->mTranspose);
        if (!enabled) return 0;
        if (key>127) key=127;
        if (key<0) key=0;
        if (state->mMuted==0 && emitNotes!=0) {
            const be<u32>* table=mVtable;
            s32 noteLength=duration>0 ? duration : -1;
            gabi::call_ptr<void>(table[7],this,track,key,velocity,noteLength,static_cast<u32>(state->mNoteMode));
        }
        if (state->mWaitForNotes!=0) {
            state->mWait=duration;
            if (duration==0) state->mWaitForNoteEnd=1;
        }
        return 0;
    }
    u32 family=opcode&0xF0;
    auto dispatch=[&](u32 command,s32 first,s32 second) {
        const be<u32>* table=mVtable;
        gabi::call_ptr<void>(table[5],this,track,command,first,second);
    };
    if (family==0x80) {
        switch (opcode) {
        case 0x80: {
            s32 wait=arguments->readArgument(&cursor,owner,track,overrideEncoding ? overrideEncoding : 3);
            if (enabled) state->mWait=wait;
            return 0;
        }
        case 0x81: {
            s32 value=arguments->readArgument(&cursor,owner,track,overrideEncoding ? overrideEncoding : 3);
            if (enabled) dispatch(opcode,value,0);
            return 0;
        }
        case 0x88: {
            u32 index=commandByte(cursor);
            u32 offset=arguments->readOffset(&cursor);
            if (enabled) dispatch(opcode,index,offset);
            return 0;
        }
        case 0x89: case 0x8A: {
            u32 offset=arguments->readOffset(&cursor);
            if (enabled) dispatch(opcode,offset,0);
            return 0;
        }
        default: return 0;
        }
    }
    if (family==0xB0 || family==0xC0 || family==0xD0) {
        u32 first=u32(arguments->readArgument(&cursor,owner,track,overrideEncoding ? overrideEncoding : 1))&0xFF;
        gabi::FrameLocal<be<u8>> savedByte(8);
        *savedByte=first;
        s32 second=0;
        if (secondaryEncoding) second=arguments->readArgument(&cursor,owner,track,secondaryEncoding);
        if (enabled) {
            s32 value=first;
            if (opcode>=0xC3 && opcode<=0xC4) value=s8(static_cast<u8>(*savedByte));
            dispatch(opcode,value,second);
        }
        return 0;
    }
    if (family==0x90) {
        if (enabled) dispatch(opcode,0,0);
        return 0;
    }
    if (family==0xE0) {
        s32 value=s16(arguments->readArgument(&cursor,owner,track,overrideEncoding ? overrideEncoding : 2));
        if (enabled) dispatch(opcode,value,0);
        return 0;
    }
    if (family!=0xF0) return 0;
    if (opcode==0xFE) {
        arguments->readHalfword(&cursor);
        return 0;
    }
    if (opcode==0xFF) return enabled ? 1 : 0;
    if (opcode!=0xF0) {
        if (enabled) dispatch(opcode,0,0);
        return 0;
    }
    u32 extended=commandByte(cursor);
    u32 extendedFamily=extended&0xF0;
    if (extendedFamily==0xA0 || extendedFamily==0xB0) {
        u32 value=commandByte(cursor);
        if (enabled) dispatch((opcode<<8)+extended,value,0);
    } else if (extendedFamily==0xE0) {
        u32 value=u32(arguments->readArgument(&cursor,owner,track,overrideEncoding ? overrideEncoding : 2))&0xFFFF;
        if (enabled) dispatch((opcode<<8)+extended,value,0);
    } else if (extendedFamily==0x80 || extendedFamily==0x90) {
        u32 index=commandByte(cursor);
        s32 value=s16(arguments->readArgument(&cursor,owner,track,overrideEncoding ? overrideEncoding : 2));
        if (enabled) dispatch((opcode<<8)+extended,index,value);
    }
    return 0;
}
/* 028AB2A8 */
u32 SequenceCommandReader::readTrackMask(const be<u8>* sequence,u32 offset,be<u32>* mask) {
    const be<u8>* position=gabi::at<const be<u8>>(gabi::ea(sequence)+offset);
    if (*position!=0xFE) {
        *mask=1;
        return offset;
    }
    u32 high=*gabi::at<const be<u8>>(gabi::ea(position)+1);
    u32 low=*gabi::at<const be<u8>>(gabi::ea(position)+2);
    *mask=(high<<8)|low|1;
    return offset+3;
}
/* WWHD glue
*/
