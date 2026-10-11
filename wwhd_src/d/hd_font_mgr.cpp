/* HD font manager: seven CKing/Cafe fonts, archive paths and glyph dimensions.
 * GameCube-style source verified as hd_font_mgr; no GameCube counterpart. */
#include "d/hd_font_mgr.h"

static const u32 disposerVtable = 0x100E07F0;
static const u32 safeStringVtable = 0x100E0654;
static const u32 bufferedStringVtable = 0x100E069C;
static const u32 nameDestructor = 0x025F386C;
static hdFont_instances_c sInstances = {};
static hdFont_string_c sArchiveNames[7] = {};
static hdFont_string_c sFileNames[7] = {};
static u32 sArchiveNamesReady;
static u32 sFileNamesReady;
static hdFont_size_c sZeroSize;
static u32 sZeroSizeReady;
static f32 sZero;
static f32 sOne;
static u16 sGlyphParameters[7] = {};
static hdFont_pointer_c sResourceLoader;
static hdFont_pointer_c sFontLock;
static hdFont_systemPointer_c sSystem;
static hdFont_size_c* hdFont_constructSize(hdFont_size_c* size);

/* 025F2EA0 */
static void hdFont_destroyFileNames() {
    hdFont_destroyArray(sFileNames, FONT_COUNT, sizeof(hdFont_string_c), nameDestructor, 0, 0);
}

/* 025F2EC4 */
static void hdFont_destroyArchiveNames() {
    hdFont_destroyArray(sArchiveNames, FONT_COUNT, sizeof(hdFont_string_c), nameDestructor, 0, 0);
}

/* 025F2EE8 */
static hdFontMgr_c* hdFont_constructManager(hdFontMgr_c* i_this) {
    if (!i_this) {
        i_this = (hdFontMgr_c*)operator_new(sizeof(hdFontMgr_c));
        if (!i_this) return nullptr;
    }
    // HD: GHS checks embedded array addresses with 32-bit guest wraparound.
    if (!gabi::ea(i_this->mpFont)) operator_new(sizeof(i_this->mpFont));
    hdFont_size_c* sizes = gabi::at<hdFont_size_c>(gabi::ea(i_this->mGlyphSize));
    if (!sizes) sizes = (hdFont_size_c*)operator_new(sizeof(i_this->mGlyphSize));
    if (sizes) hdFont_constructArray(sizes, FONT_COUNT, sizeof(hdFont_size_c), hdFont_constructSize);
    if (!gabi::ea(i_this->mScale)) operator_new(sizeof(i_this->mScale));
    if (!sZeroSizeReady) {
        f32 zero = sZero;
        sZeroSizeReady = 1;
        sZeroSize.y = zero;
        sZeroSize.x = zero;
    }
    for (u32 i = 0; i < FONT_COUNT; ++i) {
        // HD: integer copies retain any NaN payload in the guarded zero vector.
        i_this->mGlyphSize[i].xBits = sZeroSize.xBits;
        i_this->mGlyphSize[i].yBits = sZeroSize.yBits;
    }
    f32 one = sOne;
    for (u32 i = 0; i < FONT_COUNT; ++i) i_this->mScale[i] = one;
    return i_this;
}

/* 025F2FE0 */
static void hdFont_createInstance(void* heap) {
    if (sInstances.mpInstance) return;
    hdFontMgr_c* manager = hdFont_allocInstance(sizeof(hdFontMgr_c), heap, 4);
    if (manager) {
        hdFont_constructDisposer(manager, heap, 3);
        manager->mDisposerVtable = disposerVtable;
    }
    sInstances.mpDisposerHolder = manager;
    sInstances.mpInstance = manager ? hdFont_constructManager(manager) : nullptr;
}

/* inlined into getArchiveName and getFileName */
static void hdFont_setName(hdFont_string_c* name, const char* text) {
    name->setVtable(safeStringVtable);
    name->setText(text);
}

/* 025F3074 */
hdFont_string_c* hdFontMgr_c::getArchiveName(u32 index) {
    if (!sArchiveNamesReady) {
        sArchiveNamesReady = 1;
        hdFont_setName(&sArchiveNames[FONT_MAIN], "CKingMain_bffnt");
        hdFont_setName(&sArchiveNames[FONT_MESSAGE], "CKingMsg_bffnt");
        hdFont_setName(&sArchiveNames[FONT_RUBY], "CKingRuby_bffnt");
        hdFont_setName(&sArchiveNames[FONT_ZELDA], "CKingZelda_bffnt");
        hdFont_setName(&sArchiveNames[FONT_PICTURE], "CKingPic_bffnt");
        hdFont_setName(&sArchiveNames[FONT_SYSTEM], "CafeStd_bffnt");
        hdFont_setName(&sArchiveNames[FONT_MAIN_L], "CKingMainL_bffnt");
        hdFont_registerDestructor(0x101F4A14);
    }
    // HD: unrestricted indices use unsigned guest address arithmetic, including wrap to null.
    return gabi::at<hdFont_string_c>(gabi::ea(sArchiveNames) + (index << 3));
}

/* 025F320C */
hdFont_string_c* hdFontMgr_c::getFileName(u32 index) {
    if (!sFileNamesReady) {
        sFileNamesReady = 1;
        hdFont_setName(&sFileNames[FONT_MAIN], "CKingMain.bffnt");
        hdFont_setName(&sFileNames[FONT_MESSAGE], "CKingMsg.bffnt");
        hdFont_setName(&sFileNames[FONT_RUBY], "CKingRuby.bffnt");
        hdFont_setName(&sFileNames[FONT_ZELDA], "CKingZelda.bffnt");
        hdFont_setName(&sFileNames[FONT_PICTURE], "CKingPic.bffnt");
        hdFont_setName(&sFileNames[FONT_SYSTEM], "CafeStd.bffnt");
        hdFont_setName(&sFileNames[FONT_MAIN_L], "CKingMainL.bffnt");
        hdFont_registerDestructor(0x101F4A20);
    }
    return gabi::at<hdFont_string_c>(gabi::ea(sFileNames) + (index << 3));
}

/* 025F33A4 */
void hdFontMgr_c::loadFonts(void* heap) {
    // HD: the file information overlaps the last four bytes of the path buffer.
    gabi::Local<hdFont_loadFrame_c> frame;
    for (u32 i = 0; i < FONT_COUNT; ++i) {
        if (i == FONT_SYSTEM) {
            mpFont[i] = &sSystem.mpSystem->mFont;
            continue;
        }
        frame->mStorage.mBuffer[0] = 0;
        frame->mPath.mStringTop = frame->mStorage.mBuffer;
        frame->mCapacity = 0x100;
        frame->mStorage.mBuffer[0xFF] = 0;
        frame->mPath.setVtable(bufferedStringVtable);
        hdFont_string_c* name = getArchiveName(i);
        name->assureTermination();
        hdFont_formatPath(&frame->mPath, "Common/Font/EuJpUs/%s.szs", name->mStringTop);
        void* loader = sResourceLoader.mpObject;
        name = getArchiveName(i);
        hdFont_archive_c* archive = hdFont_findArchive(loader, name);
        if (!archive) {
            loader = sResourceLoader.mpObject;
            name = getArchiveName(i);
            if (hdFont_loadArchive(loader, name, &frame->mPath, heap, 1, 0x40000, 0x2000)) {
                loader = sResourceLoader.mpObject;
                name = getArchiveName(i);
                archive = hdFont_findArchive(loader, name);
            }
        }
        frame->mStorage.mFile.mInfo.mOffset = 0;
        frame->mStorage.mFile.mInfo.mSize = 0;
        name = getFileName(i);
        void* data = gabi::call_ptr<void*>(archive->mVtable->mGetFile, archive, name, &frame->mStorage.mFile.mInfo);
        hdFont_c* font = hdFont_allocFont(sizeof(hdFont_c), heap, 4);
        if (font) font = hdFont_construct(font);
        hdFont_lock(sFontLock.mpObject);
        hdFont_load(font, data);
        hdFont_unlock(sFontLock.mpObject);
        gabi::call_ptr<void>(font->mVtable->mSetGlyphParameter, font, u32(sGlyphParameters[i]));
        u32 getWidth = font->mVtable->mGetWidth;
        f32 width = f32(f64(gabi::call_ptr<s32>(getWidth, font)));
        f32 height = f32(f64(gabi::call_ptr<s32>(font->mVtable->mGetHeight, font)));
        mGlyphSize[i].x = width;
        mGlyphSize[i].y = height;
        mpFont[i] = font;
    }
    hdFont_c* picture = mpFont[FONT_PICTURE];
    gabi::call_ptr<void>(picture->mVtable->mSetPictureParameter, picture, 0u);
}

/* 025F36B8 */
hdFont_c* hdFontMgr_c::getFont(s32 index) {
    if (index >= FONT_COUNT) return nullptr;
    // HD: negative indices remain permitted and wrap in the original guest address calculation.
    return *gabi::at<gptr<hdFont_c>>(gabi::ea(mpFont) + 4u * u32(index));
}

/* 025F36D8 */
void hdFontMgr_c::registerFonts(void* manager) {
    for (u32 i = 0; i < FONT_COUNT; ++i) {
        hdFont_string_c* name = getFileName(i);
        name->assureTermination();
        be<u8>* text = name->mStringTop;
        hdFont_c* font = getFont(s32(i));
        hdFont_register(manager, text, font);
    }
}

/* 025F3758 */
static void hdFont_destroyManager(hdFontMgr_c* i_this, u32 flags) {
    if (!i_this) return;
    i_this->mDisposerVtable = disposerVtable;
    if (i_this == sInstances.mpDisposerHolder) {
        sInstances.mpDisposerHolder = nullptr;
        sInstances.mpInstance = nullptr;
    }
    hdFont_destroyDisposer(i_this, 0);
    if (flags & 1) operator_delete(i_this);
}

/* ---------------- HD: compiler-generated and template code ---------------- */

/* 025F37D8 */
static void __sinit_hd_font_mgr_cpp() {
    sinit_header_statics_rodata(0x1048D5B0, 0x101F4A2C, 0x1048D62C, 0x100E07E4);
}

/* 025F38D8 */
static hdFont_size_c* hdFont_constructSize(hdFont_size_c* size) {
    if (!size) {
        size = (hdFont_size_c*)operator_new(sizeof(hdFont_size_c));
        if (!size) return nullptr;
    }
    f32 zero = sZero;
    size->y = zero;
    size->x = zero;
    return size;
}

/* WWHD glue
data sInstances 101F4A50 nocheck
data sArchiveNames 1048D5F4 nocheck
data sFileNames 1048D5BC nocheck
data sArchiveNamesReady 1048D648 nocheck
data sFileNamesReady 1048D64C nocheck
data sZeroSize 1048D63C nocheck
data sZeroSizeReady 1048D644 nocheck
data sZero 100E06BC nocheck
data sOne 100E06B8 nocheck
data sGlyphParameters 100E07D4 nocheck
data sResourceLoader 101F4F7C nocheck
data sFontLock 101F8B18 nocheck
data sSystem 101F4BD8 nocheck
str 100E06C0 "CKingMain_bffnt" in 025F3074
str 100E06F0 "CKingMsg_bffnt" in 025F3074
str 100E06D0 "CKingRuby_bffnt" in 025F3074
str 100E0700 "CKingZelda_bffnt" in 025F3074
str 100E0714 "CKingPic_bffnt" in 025F3074
str 100E06E0 "CafeStd_bffnt" in 025F3074
str 100E0724 "CKingMainL_bffnt" in 025F3074
str 100E0738 "CKingMain.bffnt" in 025F320C
str 100E0768 "CKingMsg.bffnt" in 025F320C
str 100E0748 "CKingRuby.bffnt" in 025F320C
str 100E0778 "CKingZelda.bffnt" in 025F320C
str 100E078C "CKingPic.bffnt" in 025F320C
str 100E0758 "CafeStd.bffnt" in 025F320C
str 100E079C "CKingMainL.bffnt" in 025F320C
str 100E07B8 "Common/Font/EuJpUs/%s.szs" in 025F33A4
ext 028F0164 void hdFont_destroyArray(void* array, u32 count, u32 stride, u32 destructor, u32 arg5, u32 arg6)
ext 028EFFD0 void hdFont_constructArray(hdFont_size_c* array, u32 count, u32 stride, hdFont_sizeConstructor constructor)
ext 028F026C void hdFont_registerDestructor(u32 record)
ext 0273B0D4 hdFontMgr_c* hdFont_allocInstance(u32 size, void* heap, u32 alignment)
ext 02752B0C void hdFont_constructDisposer(hdFontMgr_c* manager, void* heap, u32 flags)
ext 02752BEC void hdFont_destroyDisposer(hdFontMgr_c* manager, u32 flags)
ext 02759C28 void hdFont_formatPath(hdFont_string_c* path, const char* format, be<u8>* name)
ext 026123FC hdFont_archive_c* hdFont_findArchive(void* loader, hdFont_string_c* name)
ext 02612E64 u32 hdFont_loadArchive(void* loader, hdFont_string_c* name, hdFont_string_c* path, void* heap, u32 flags, u32 size, u32 alignment)
ext 0273B050 hdFont_c* hdFont_allocFont(u32 size, void* heap, u32 alignment)
ext 0286ED48 hdFont_c* hdFont_construct(hdFont_c* font)
ext 0286EFEC void hdFont_load(hdFont_c* font, void* data)
ext 0274FBF8 void hdFont_lock(void* lock)
ext 0274FCCC void hdFont_unlock(void* lock)
ext 028736AC void hdFont_register(void* manager, be<u8>* name, hdFont_c* font)
*/
