/* Resource-file and UTF-16 cursor helpers; canonical NW-LYT#1 layouts. */
#pragma once
#include "nw/lyt/FontResource.h"
/* The object OFFSET bytes past BASE: resource blocks, tables and names are addressed by byte offsets. */
template <class T> inline T* layoutAtOffset(const void* base, u32 offset) { return gabi::at<T>(gabi::ea(base) + offset); }
u32 layoutByteOrderReversed(const nw::lyt::FontFile* file);
nw::lyt::FontBlock* layoutNextResourceBlock(nw::lyt::FontFile* file, nw::lyt::FontBlock* previous);
u32 layoutShortNameEqual(const be<u8>* left, const be<u8>* right);
u32 layoutLongNameEqual(const be<u8>* left, const be<u8>* right);
u32 layoutValidateResourceHeader(nw::lyt::FontFile* file, u32 magic, u32 version, u32 minimumBlocks);
u32 layoutReadUtf16Character(nw::lyt::CharacterCursor* cursor);
u32 layoutZeroResult();
void layoutEmptyCallback();
void setLayoutStorageAllocator(void* allocator);
void layoutStorageAllocatorSink(void* allocator);
