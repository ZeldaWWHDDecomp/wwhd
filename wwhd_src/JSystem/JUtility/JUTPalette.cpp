/** JUTPalette platform initialization; private decomp source. */
#include "bindings.h"
#include "JSystem/JUtility/JUTPalette.h"
static JUTPaletteStaticBlock sStaticBlock;
static u32 sStaticRegister;

/* 02843070 */
static void __sinit_JUTPalette_cpp() {
    sStaticBlock.mWords[2] = 0;
    sStaticBlock.mWords[0] = 0;
    sStaticBlock.mWords[3] = 0;
    sStaticBlock.mWords[1] = 0;
    JUTPaletteConstructRegister(&sStaticRegister);
}


/* 02842FBC */
void JUTPalette::storeTLUT(u32 name,ResTLUT* tlut) {
    if (tlut == NULL) {
        // HD: diagnostics retain the build path and literal zero assertion.
        JUTPaletteReport("D:/home/Cafe/cking/DevEnv/jsys/System/../System/JUtility/src/JUTPalette.cpp",36,"JUTTexture: TLUT is NULL\n");
        JUTPaletteAssert("JUTPalette.cpp",37,"0");
        return;
    }
    mTlutName = name;
    mFormat = tlut->format;
    mTransparency = tlut->transparency;
    u32 count = tlut->numColors;
    mColorTable = tlut + 1;
    mNumColors = count;
}
/* 02843048 */
void JUTPalette::storeTLUTRaw(u32 name,u32 format,u32 transparency,u32 colors,void* table) {
    // HD: GX palette initialization was removed; preserve the descriptor fields only.
    mColorTable = table;
    mTransparency = transparency;
    mTlutName = name;
    mFormat = format;
    mNumColors = colors;
}
/* 02843060 */
u32 JUTPalette::load() {
    // HD: palette upload is deferred to the graphics adapter.
    return (u16)mNumColors != 0;
}

/* WWHD glue
 ext C0009EB0 void JUTPaletteReport(const char*,s32,const char*)
 ext 0273AA24 void JUTPaletteAssert(const char*,s32,const char*)
 str 10173BC8 "D:/home/Cafe/cking/DevEnv/jsys/System/../System/JUtility/src/JUTPalette.cpp" renamed
 str 10173B9C "JUTTexture: TLUT is NULL\n" renamed
 str 10173BB8 "JUTPalette.cpp"
 str 10173B98 "0" renamed
 data sStaticBlock 104B6524
 data sStaticRegister 101FAFCC
 ext 028F026C void JUTPaletteConstructRegister(void*)
*/
