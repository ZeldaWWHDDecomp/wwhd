/* Sequence byte-stream arguments. Names are descriptive, inferred from WWHD. */
#include "nw/snd/SequenceArgumentReader.h"
#include "nw/snd/AudioCurves.h"
using namespace nw::snd;
const be<s16>* trackSequenceVariable(void* variables,s32 index);
const be<s16>* playerSequenceVariable(void* variables,s32 index);

/* inlined into SequenceArgumentReader byte readers */
static const be<u8>* advanceSequence(const be<u8>* bytes,u32 count) {
    return gabi::at<const be<u8>>(gabi::ea(bytes)+count);
}

/* 028AA9C4 */
const be<s16>* SequenceArgumentReader::findVariable(void* trackVariables,void* playerVariables,s32 index) const {
    gabi::AutoFrame frame{gabi::current_function()};
    if (index < 32) return trackSequenceVariable(trackVariables,index);
    if (index < 48) return playerSequenceVariable(playerVariables,index-32);
    return nullptr;
}
/* 028AA9F0 */
u32 SequenceArgumentReader::readHalfword(SequenceCursor* cursor) const {
    const be<u8>* bytes=cursor->mPosition;
    cursor->mPosition=advanceSequence(bytes,1);
    u32 high=bytes[0];
    cursor->mPosition=advanceSequence(bytes,2);
    return (high<<8)|static_cast<u32>(*advanceSequence(bytes,1));
}
/* 028AAA18 */
u32 SequenceArgumentReader::readVariableLength(SequenceCursor* cursor) const {
    const be<u8>* bytes=cursor->mPosition;
    u32 value=0;
    u32 byte;
    do {
        byte=*bytes;
        bytes=advanceSequence(bytes,1);
        value=(value<<7)|(byte&0x7F);
        cursor->mPosition=bytes;
    } while (byte&0x80);
    return value;
}
/* 028AAA40 */
s32 SequenceArgumentReader::readArgument(SequenceCursor* cursor,void* trackVariables,void* playerVariables,u32 encoding) const {
    gabi::AutoFrame frame{gabi::current_function()};
    switch (encoding) {
    case Byte: {
        const be<u8>* bytes=cursor->mPosition;
        cursor->mPosition=advanceSequence(bytes,1);
        return *bytes;
    }
    case Halfword: return readHalfword(cursor);
    case VariableLength: return readVariableLength(cursor);
    case Variable: {
        const be<u8>* bytes=cursor->mPosition;
        cursor->mPosition=advanceSequence(bytes,1);
        u32 index=*bytes;
        const be<s16>* variable=findVariable(trackVariables,playerVariables,index);
        return variable == nullptr ? 0 : static_cast<s32>(*variable);
    }
    case RandomRange: {
        s32 lower=static_cast<s16>(readHalfword(cursor));
        s32 upper=static_cast<s16>(readHalfword(cursor));
        u32 random=nextRandomHalfword();
        u32 product=random*static_cast<u32>(upper-lower+1);
        return static_cast<s32>(static_cast<u32>(static_cast<s32>(product)>>16)+static_cast<u32>(lower));
    }
    default: return 0;
    }
}
/* 028AAB58 */
u32 SequenceArgumentReader::readOffset(SequenceCursor* cursor) const {
    const be<u8>* bytes=cursor->mPosition;
    cursor->mPosition=advanceSequence(bytes,1);
    u32 high=bytes[0];
    cursor->mPosition=advanceSequence(bytes,2);
    u32 middle=*advanceSequence(bytes,1);
    cursor->mPosition=advanceSequence(bytes,3);
    return ((high<<8)|middle)<<8|static_cast<u32>(*advanceSequence(bytes,2));
}
/* WWHD glue
ext 0288BF64 const be<s16>* trackSequenceVariable(void*,s32)
ext 0288E754 const be<s16>* playerSequenceVariable(void*,s32)
ext 0289CE14 u32 nw::snd::nextRandomHalfword()
*/
