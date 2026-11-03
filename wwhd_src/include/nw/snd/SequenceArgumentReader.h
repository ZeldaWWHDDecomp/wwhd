/* Sequence argument encodings and cursor layout, reconstructed from WWHD. */
#pragma once
#include "wwhd.h"
namespace nw { namespace snd {
struct SequenceCursor { gptr<const be<u8>> mPosition; };
struct SequenceArgumentReader {
    enum Encoding { Byte=1, Halfword=2, VariableLength=3, RandomRange=4, Variable=5 };
    const be<s16>* findVariable(void* trackVariables,void* playerVariables,s32 index) const;
    u32 readHalfword(SequenceCursor* cursor) const;
    u32 readVariableLength(SequenceCursor* cursor) const;
    s32 readArgument(SequenceCursor* cursor,void* trackVariables,void* playerVariables,u32 encoding) const;
    u32 readOffset(SequenceCursor* cursor) const;
};
WWHD_SIZE(SequenceCursor,4);
} }
