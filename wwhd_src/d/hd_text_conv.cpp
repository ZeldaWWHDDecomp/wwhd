/** HD message control-character conversion. No GameCube counterpart; private decomp source. */
#include "bindings.h"
#include "d/hd_text_conv.h"
static hdTextGlyphSelectors* sMessageManager;
static s32 sGlyphSelectorStep;
static hdTextDefaultGlyphTable sDefaultGlyphs;
static hdTextGlyphTable sFirstPrimaryGlyphs;
static hdTextGlyphTable sFirstSecondaryGlyphs;
static hdTextGlyphTable sFirstTertiaryGlyphs;
static hdTextGlyphTable sFirstQuaternaryGlyphs;
static hdTextGlyphTable sSecondPrimaryGlyphs;
static hdTextGlyphTable sSecondSecondaryGlyphs;
static hdTextGlyphTable sAlternateFirstPrimaryGlyphs;
static hdTextGlyphTable sAlternateFirstSecondaryGlyphs;
static hdTextGlyphTable sAlternateFirstTertiaryGlyphs;
static hdTextGlyphTable sAlternateFirstQuaternaryGlyphs;
static hdTextGlyphTable sAlternateSecondPrimaryGlyphs;
static hdTextGlyphTable sAlternateSecondSecondaryGlyphs;

/* 025F90C4 */
static void textConversionEmpty() {}

static u16 selectGlyph(s32 selector, const hdTextGlyphTable& table) {
    s32 index = selector / sGlyphSelectorStep;
    return table.get(index);
}

/* 025F90C8 */
static u16 convertControlCharacter(void*, u32 code) {
    u16 glyph = sDefaultGlyphs.get(code);
    hdTextGlyphSelectors* manager = sMessageManager;
    if (manager == NULL) {
        return glyph;
    }
    enum {
        FIRST_PRIMARY = 0xD, FIRST_SECONDARY, FIRST_TERTIARY, FIRST_QUATERNARY,
        SECOND_PRIMARY, SECOND_SECONDARY,
        ALT_FIRST_PRIMARY = 0x1C, ALT_FIRST_SECONDARY, ALT_FIRST_TERTIARY,
        ALT_FIRST_QUATERNARY, ALT_SECOND_PRIMARY, ALT_SECOND_SECONDARY,
    };
    switch (code) {
    case FIRST_PRIMARY: return selectGlyph(manager->mFirstSelector, sFirstPrimaryGlyphs);
    case FIRST_SECONDARY: return selectGlyph(manager->mFirstSelector, sFirstSecondaryGlyphs);
    case FIRST_TERTIARY: return selectGlyph(manager->mFirstSelector, sFirstTertiaryGlyphs);
    case FIRST_QUATERNARY: return selectGlyph(manager->mFirstSelector, sFirstQuaternaryGlyphs);
    case SECOND_PRIMARY: return selectGlyph(manager->mSecondSelector, sSecondPrimaryGlyphs);
    case SECOND_SECONDARY: return selectGlyph(manager->mSecondSelector, sSecondSecondaryGlyphs);
    case ALT_FIRST_PRIMARY: return selectGlyph(manager->mFirstSelector, sAlternateFirstPrimaryGlyphs);
    case ALT_FIRST_SECONDARY: return selectGlyph(manager->mFirstSelector, sAlternateFirstSecondaryGlyphs);
    case ALT_FIRST_TERTIARY: return selectGlyph(manager->mFirstSelector, sAlternateFirstTertiaryGlyphs);
    case ALT_FIRST_QUATERNARY: return selectGlyph(manager->mFirstSelector, sAlternateFirstQuaternaryGlyphs);
    case ALT_SECOND_PRIMARY: return selectGlyph(manager->mSecondSelector, sAlternateSecondPrimaryGlyphs);
    case ALT_SECOND_SECONDARY: return selectGlyph(manager->mSecondSelector, sAlternateSecondSecondaryGlyphs);
    default: return glyph;
    }
}

/* 025F92DC */
static void __sinit_hd_text_conv_cpp() {
    sinit_header_statics_rodata(0x1048D780, 0x101F4B64, 0x1048D78C, 0x100E0D74);
}

/* WWHD glue (tools/verify/autoglue.py): HD message selectors and glyph tables
 data sMessageManager 101F4B5C
 data sGlyphSelectorStep 100E0CB0
 data sDefaultGlyphs 100E0D20
 data sFirstPrimaryGlyphs 100E0D00
 data sFirstSecondaryGlyphs 100E0D04
 data sFirstTertiaryGlyphs 100E0D08
 data sFirstQuaternaryGlyphs 100E0D0C
 data sSecondPrimaryGlyphs 100E0CE0
 data sSecondSecondaryGlyphs 100E0CE8
 data sAlternateFirstPrimaryGlyphs 100E0D10
 data sAlternateFirstSecondaryGlyphs 100E0D14
 data sAlternateFirstTertiaryGlyphs 100E0D18
 data sAlternateFirstQuaternaryGlyphs 100E0D1C
 data sAlternateSecondPrimaryGlyphs 100E0CF0
 data sAlternateSecondSecondaryGlyphs 100E0CF8
*/
