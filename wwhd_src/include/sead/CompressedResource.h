/* WWHD sead SZS-compressed resources: the decompressed size of a resource's image and decompressing it
 * into a caller's buffer, plus the translation unit's file-handle copy and its interface object.
 * From this game's instructions. */
#pragma once
#include "sead/SZSDecompressor.h"

namespace sead {
namespace compressed_resource {
u32 getDecompSize(DirectResource* resource);
s32 decompressTo(DirectResource* resource, u8* destination, u32 destinationSize);
}

/* a four-byte object of this translation unit that holds only its vtable */
struct InterfaceObject { gptr<void> mVtable; };

namespace compressed_resource_text {
struct FileHandle : sead::FileHandle { void destroy(u32 flags); };
}
}

WWHD_SIZE(sead::InterfaceObject, 4);
