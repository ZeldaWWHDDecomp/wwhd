// JPA textures, adapted from tww JPATexture.cpp for the Cafe texture backend.
#include "JSystem/JParticle/JPATexture.h"
void* JPATextureNew(u32);
void* JPATextureHeapAllocate(void*, u32, s32);
void JPATextureAssert(const char*,s32,const char*);
void JPATextureFlush(void*,u32);
void JPATextureRegister(void*);
namespace {
static const u32 sTextureArcVtable;
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegistration;
}

/* 02827DF4 */
JPATextureArc* JPATextureArcConstruct(JPATextureArc* self, const JPATextureData* data) {
    // HD: Cafe initializes JUTTexture ownership flags without storing a GX texture.
    if (!self) {
        self = static_cast<JPATextureArc*>(JPATextureNew(sizeof(JPATextureArc)));
        if (!self) return nullptr;
    }
    u32 flags = self->mTexture.mFlags;
    self->mpData = data;
    self->mTexture.mTexInfo = nullptr;
    self->mTexture.mEmbPalette = nullptr;
    self->mTexture.mFlags = flags & 2;
    self->__vtbl = &sTextureArcVtable;
    return self;
}

/* 02827E5C */
void JPADefaultTexture::initialize(void* heap) {
    void* buffer = JPATextureHeapAllocate(heap, 0x80, 0x20);
    imgBuf = static_cast<be<u8>*>(buffer);
    if (!buffer) JPATextureAssert("JPATexture.cpp", 51, "imgBuf");
    // HD: reload the destination for every byte, preserving pointer-slot overlap.
    imgBuf.get()[0x00] = 0x00;
    imgBuf.get()[0x01] = 0xff;
    imgBuf.get()[0x02] = 0x00;
    imgBuf.get()[0x03] = 0xff;
    imgBuf.get()[0x04] = 0x00;
    imgBuf.get()[0x05] = 0xff;
    imgBuf.get()[0x06] = 0x00;
    imgBuf.get()[0x07] = 0xff;
    imgBuf.get()[0x08] = 0x00;
    imgBuf.get()[0x09] = 0xff;
    imgBuf.get()[0x0a] = 0x1a;
    imgBuf.get()[0x0b] = 0xff;
    imgBuf.get()[0x0c] = 0x80;
    imgBuf.get()[0x0d] = 0xff;
    imgBuf.get()[0x0e] = 0xd6;
    imgBuf.get()[0x0f] = 0xff;
    imgBuf.get()[0x10] = 0x00;
    imgBuf.get()[0x11] = 0xff;
    imgBuf.get()[0x12] = 0x80;
    imgBuf.get()[0x13] = 0xff;
    imgBuf.get()[0x14] = 0xff;
    imgBuf.get()[0x15] = 0xff;
    imgBuf.get()[0x16] = 0xff;
    imgBuf.get()[0x17] = 0xff;
    imgBuf.get()[0x18] = 0x00;
    imgBuf.get()[0x19] = 0xff;
    imgBuf.get()[0x1a] = 0xd6;
    imgBuf.get()[0x1b] = 0xff;
    imgBuf.get()[0x1c] = 0xff;
    imgBuf.get()[0x1d] = 0xff;
    imgBuf.get()[0x1e] = 0xff;
    imgBuf.get()[0x1f] = 0xff;
    imgBuf.get()[0x20] = 0x00;
    imgBuf.get()[0x21] = 0xff;
    imgBuf.get()[0x22] = 0x00;
    imgBuf.get()[0x23] = 0xff;
    imgBuf.get()[0x24] = 0x00;
    imgBuf.get()[0x25] = 0xff;
    imgBuf.get()[0x26] = 0x00;
    imgBuf.get()[0x27] = 0xff;
    imgBuf.get()[0x28] = 0xd6;
    imgBuf.get()[0x29] = 0xff;
    imgBuf.get()[0x2a] = 0x80;
    imgBuf.get()[0x2b] = 0xff;
    imgBuf.get()[0x2c] = 0x1a;
    imgBuf.get()[0x2d] = 0xff;
    imgBuf.get()[0x2e] = 0x00;
    imgBuf.get()[0x2f] = 0xff;
    imgBuf.get()[0x30] = 0xff;
    imgBuf.get()[0x31] = 0xff;
    imgBuf.get()[0x32] = 0xff;
    imgBuf.get()[0x33] = 0xff;
    imgBuf.get()[0x34] = 0x80;
    imgBuf.get()[0x35] = 0xff;
    imgBuf.get()[0x36] = 0x00;
    imgBuf.get()[0x37] = 0xff;
    imgBuf.get()[0x38] = 0xff;
    imgBuf.get()[0x39] = 0xff;
    imgBuf.get()[0x3a] = 0xff;
    imgBuf.get()[0x3b] = 0xff;
    imgBuf.get()[0x3c] = 0xd6;
    imgBuf.get()[0x3d] = 0xff;
    imgBuf.get()[0x3e] = 0x00;
    imgBuf.get()[0x3f] = 0xff;
    imgBuf.get()[0x40] = 0x00;
    imgBuf.get()[0x41] = 0xff;
    imgBuf.get()[0x42] = 0xd6;
    imgBuf.get()[0x43] = 0xff;
    imgBuf.get()[0x44] = 0xff;
    imgBuf.get()[0x45] = 0xff;
    imgBuf.get()[0x46] = 0xff;
    imgBuf.get()[0x47] = 0xff;
    imgBuf.get()[0x48] = 0x00;
    imgBuf.get()[0x49] = 0xff;
    imgBuf.get()[0x4a] = 0x80;
    imgBuf.get()[0x4b] = 0xff;
    imgBuf.get()[0x4c] = 0xff;
    imgBuf.get()[0x4d] = 0xff;
    imgBuf.get()[0x4e] = 0xff;
    imgBuf.get()[0x4f] = 0xff;
    imgBuf.get()[0x50] = 0x00;
    imgBuf.get()[0x51] = 0xff;
    imgBuf.get()[0x52] = 0x1a;
    imgBuf.get()[0x53] = 0xff;
    imgBuf.get()[0x54] = 0x80;
    imgBuf.get()[0x55] = 0xff;
    imgBuf.get()[0x56] = 0xd6;
    imgBuf.get()[0x57] = 0xff;
    imgBuf.get()[0x58] = 0x00;
    imgBuf.get()[0x59] = 0xff;
    imgBuf.get()[0x5a] = 0x00;
    imgBuf.get()[0x5b] = 0xff;
    imgBuf.get()[0x5c] = 0x00;
    imgBuf.get()[0x5d] = 0xff;
    imgBuf.get()[0x5e] = 0x00;
    imgBuf.get()[0x5f] = 0xff;
    imgBuf.get()[0x60] = 0xff;
    imgBuf.get()[0x61] = 0xff;
    imgBuf.get()[0x62] = 0xff;
    imgBuf.get()[0x63] = 0xff;
    imgBuf.get()[0x64] = 0xd6;
    imgBuf.get()[0x65] = 0xff;
    imgBuf.get()[0x66] = 0x00;
    imgBuf.get()[0x67] = 0xff;
    imgBuf.get()[0x68] = 0xff;
    imgBuf.get()[0x69] = 0xff;
    imgBuf.get()[0x6a] = 0xff;
    imgBuf.get()[0x6b] = 0xff;
    imgBuf.get()[0x6c] = 0x80;
    imgBuf.get()[0x6d] = 0xff;
    imgBuf.get()[0x6e] = 0x00;
    imgBuf.get()[0x6f] = 0xff;
    imgBuf.get()[0x70] = 0xd6;
    imgBuf.get()[0x71] = 0xff;
    imgBuf.get()[0x72] = 0x80;
    imgBuf.get()[0x73] = 0xff;
    imgBuf.get()[0x74] = 0x1a;
    imgBuf.get()[0x75] = 0xff;
    imgBuf.get()[0x76] = 0x00;
    imgBuf.get()[0x77] = 0xff;
    imgBuf.get()[0x78] = 0x00;
    imgBuf.get()[0x79] = 0xff;
    imgBuf.get()[0x7a] = 0x00;
    imgBuf.get()[0x7b] = 0xff;
    imgBuf.get()[0x7c] = 0x00;
    imgBuf.get()[0x7d] = 0xff;
    imgBuf.get()[0x7e] = 0x00;
    imgBuf.get()[0x7f] = 0xff;
    // HD: Cafe flushes the image; GX texture-object setup belongs to the HD caller.
    JPATextureFlush(imgBuf, 0x80);
}

/* 028282D4 */
void __sinit_JPATexture_cpp() {
    sStaticBlock.words[2] = 0;
    sStaticBlock.words[0] = 0;
    sStaticBlock.words[3] = 0;
    sStaticBlock.words[1] = 0;
    JPATextureRegister(&sStaticRegistration);
}

/* WWHD glue
str 10171F08 "JPATexture.cpp" in 02827E5C
str 10171F00 "imgBuf" in 02827E5C
data sTextureArcVtable 10171ED8
data sStaticBlock 104B59A0
data sStaticRegistration 101FACCC
ext 0273AD10 void* JPATextureNew(u32)
ext 027EC0D8 void* JPATextureHeapAllocate(void*,u32,s32)
ext 0273AA24 void JPATextureAssert(const char*,s32,const char*)
ext C00088B0 void JPATextureFlush(void*,u32)
ext 028F026C void JPATextureRegister(void*)
*/
