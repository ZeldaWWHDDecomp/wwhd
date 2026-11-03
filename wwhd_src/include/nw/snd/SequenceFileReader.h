/* Sequence resource layout reconstructed from WWHD accesses. */
#pragma once
#include "nw/snd/GroupFileReader.h"
namespace nw { namespace snd {
struct SequenceFileReader {
    gptr<const GroupFileHeader> mHeader;
    gptr<const be<u8>> mData;
    gptr<const be<u8>> mLabels;
    static u32 isSupported(const GroupFileHeader* header);
    static SequenceFileReader* construct(SequenceFileReader* reader,const GroupFileHeader* header);
    const be<u8>* data() const;
    u32 findLabel(const char* label, be<u32>* offset) const;
};
static_assert(sizeof(SequenceFileReader)==0xC);
static_assert(offsetof(SequenceFileReader,mData)==4);
static_assert(offsetof(SequenceFileReader,mLabels)==8);
}}
