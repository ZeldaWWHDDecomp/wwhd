/* HD: font archives, glyph sizes and the Cafe system font; no GameCube counterpart. */
#pragma once
#include "bindings.h"

enum hdFontIndex_e { FONT_MAIN, FONT_MESSAGE, FONT_RUBY, FONT_ZELDA, FONT_PICTURE, FONT_SYSTEM, FONT_MAIN_L, FONT_COUNT };
struct hdFont_stringVtable_c { u8 _00[0x14]; be<u32> mAssureTermination; };
struct hdFont_string_c {
    gptr<be<u8>> mStringTop;
    gptr<hdFont_stringVtable_c> mVtable;
    void setVtable(u32 address) { mVtable.v = address; }
    void setText(const char* text) { mStringTop.v = gabi::guest_addr(text); }
    void assureTermination() { gabi::call_ptr<void>(mVtable->mAssureTermination, this); }
};
WWHD_SIZE(hdFont_string_c, 8);
WWHD_OFFSET(hdFont_string_c, mVtable, 4);
struct hdFont_vtable_c {
    u8 _00[0x1C]; be<u32> mGetWidth;
    u8 _20[4]; be<u32> mGetHeight;
    u8 _28[0x74 - 0x28]; be<u32> mSetGlyphParameter;
    u8 _78[0xEC - 0x78]; be<u32> mSetPictureParameter;
};
WWHD_OFFSET(hdFont_vtable_c, mGetWidth, 0x1C);
WWHD_OFFSET(hdFont_vtable_c, mGetHeight, 0x24);
WWHD_OFFSET(hdFont_vtable_c, mSetGlyphParameter, 0x74);
WWHD_OFFSET(hdFont_vtable_c, mSetPictureParameter, 0xEC);
struct hdFont_c { u8 _00[4]; gptr<hdFont_vtable_c> mVtable; u8 _08[0xC4-8]; };
WWHD_OFFSET(hdFont_c, mVtable, 4);
WWHD_SIZE(hdFont_c, 0xC4);
struct hdFont_size_c {
    union { be<f32> x; be<u32> xBits; };
    union { be<f32> y; be<u32> yBits; };
};
WWHD_SIZE(hdFont_size_c, 8);
struct hdFontMgr_c {
    /* 0x00 */ u8 mDisposer[0xC];
    /* 0x0C */ be<u32> mDisposerVtable;
    /* 0x10 */ gptr<hdFont_c> mpFont[FONT_COUNT];
    /* 0x2C */ u8 _2C[4];
    /* 0x30 */ hdFont_size_c mGlyphSize[FONT_COUNT];
    /* 0x68 */ be<f32> mScale[FONT_COUNT];
    hdFont_string_c* getArchiveName(u32 index);
    hdFont_string_c* getFileName(u32 index);
    void loadFonts(void* heap);
    hdFont_c* getFont(s32 index);
    void registerFonts(void* manager);
};
WWHD_OFFSET(hdFontMgr_c, mDisposerVtable, 0xC);
WWHD_OFFSET(hdFontMgr_c, mpFont, 0x10);
WWHD_OFFSET(hdFontMgr_c, mGlyphSize, 0x30);
WWHD_OFFSET(hdFontMgr_c, mScale, 0x68);
WWHD_SIZE(hdFontMgr_c, 0x84);
struct hdFont_instances_c { gptr<hdFontMgr_c> mpInstance; gptr<hdFontMgr_c> mpDisposerHolder; };
struct hdFont_pointer_c { gptr<void> mpObject; };
struct hdFont_system_c { u8 _00[0x3C]; hdFont_c mFont; };
WWHD_OFFSET(hdFont_system_c, mFont, 0x3C);
struct hdFont_systemPointer_c { gptr<hdFont_system_c> mpSystem; };
struct hdFont_fileInfo_c { be<u32> mOffset; be<u32> mSize; };
struct hdFont_loadFrame_c {
    hdFont_string_c mPath;
    be<u32> mCapacity;
    union {
        be<u8> mBuffer[0x100];
        struct { u8 _00[0xFC]; hdFont_fileInfo_c mInfo; } mFile;
    } mStorage;
};
WWHD_OFFSET(hdFont_loadFrame_c, mStorage, 0xC);
WWHD_OFFSET(hdFont_loadFrame_c, mStorage.mFile.mInfo, 0x108);
WWHD_SIZE(hdFont_loadFrame_c, 0x110);
struct hdFont_archiveVtable_c { u8 _00[0x3C]; be<u32> mGetFile; };
struct hdFont_archive_c { u8 _00[0x10]; gptr<hdFont_archiveVtable_c> mVtable; };
WWHD_OFFSET(hdFont_archive_c, mVtable, 0x10);

void hdFont_destroyArray(void* array, u32 count, u32 stride, u32 destructor, u32 arg5, u32 arg6);
typedef hdFont_size_c* (*hdFont_sizeConstructor)(hdFont_size_c*);
void hdFont_constructArray(hdFont_size_c* array, u32 count, u32 stride, hdFont_sizeConstructor constructor);
void hdFont_registerDestructor(u32 record);
hdFontMgr_c* hdFont_allocInstance(u32 size, void* heap, u32 alignment);
void hdFont_constructDisposer(hdFontMgr_c* manager, void* heap, u32 flags);
void hdFont_destroyDisposer(hdFontMgr_c* manager, u32 flags);
void hdFont_formatPath(hdFont_string_c* path, const char* format, be<u8>* name);
hdFont_archive_c* hdFont_findArchive(void* loader, hdFont_string_c* name);
u32 hdFont_loadArchive(void* loader, hdFont_string_c* name, hdFont_string_c* path, void* heap, u32 flags, u32 size, u32 alignment);
hdFont_c* hdFont_allocFont(u32 size, void* heap, u32 alignment);
hdFont_c* hdFont_construct(hdFont_c* font);
void hdFont_load(hdFont_c* font, void* data);
void hdFont_lock(void* lock);
void hdFont_unlock(void* lock);
void hdFont_register(void* manager, be<u8>* name, hdFont_c* font);

WWHD_SIZE(hdFont_stringVtable_c, 0x18);
WWHD_OFFSET(hdFont_stringVtable_c, mAssureTermination, 0x14);
WWHD_SIZE(hdFont_vtable_c, 0xF0);
WWHD_OFFSET(hdFont_size_c, x, 0);
WWHD_OFFSET(hdFont_size_c, y, 4);
WWHD_SIZE(hdFont_instances_c, 8);
WWHD_OFFSET(hdFont_instances_c, mpDisposerHolder, 4);
WWHD_SIZE(hdFont_pointer_c, 4);
WWHD_SIZE(hdFont_system_c, 0x100);
WWHD_SIZE(hdFont_systemPointer_c, 4);
WWHD_SIZE(hdFont_fileInfo_c, 8);
WWHD_OFFSET(hdFont_fileInfo_c, mSize, 4);
WWHD_SIZE(hdFont_archiveVtable_c, 0x40);
WWHD_OFFSET(hdFont_archiveVtable_c, mGetFile, 0x3C);
WWHD_SIZE(hdFont_archive_c, 0x14);
