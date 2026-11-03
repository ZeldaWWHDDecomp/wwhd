/* Text adapters in the AGL utility translation unit. Layouts use canonical SEAD
 * declarations; distinct fixed-text identities follow this unit's vtable slots.
 * No external source or layout reference was used. */
#pragma once
#include "sead/FileDevice.h"
#include "sead/RuntimeFormatSupport.h"
namespace agl { namespace utility_text {
using Text = sead::SafeText;
using TextBuffer = sead::SafeTextBuffer;
struct FormattedText : sead::format::ByteTextBuffer { be<u8> mStorage[1024]; };
WWHD_OFFSET(FormattedText, mStorage, 12);
WWHD_SIZE(FormattedText, 0x40C);
FormattedText* formatUtilityText(FormattedText*, const char*, u32, u32, u32, u32, u32, u32);
FormattedText* formatCaptureText(FormattedText*, const char*, u32, u32, u32, u32, u32, u32);
/* Capacities of these destructor-only views are deliberately unspecified. */
struct FixedTextA : TextBuffer {};
struct FixedTextB : TextBuffer {};
struct FixedTextC : TextBuffer {};
struct FixedTextD : TextBuffer {};
struct FixedTextE : TextBuffer {};
struct FixedTextF : TextBuffer {};
struct FixedTextG : TextBuffer {};
struct FixedTextH : TextBuffer {};
} }
void aglUtilityTextStorageDelete(void*);
void* allocateFormattedText(u32);
void releaseShadowTextArray(void*);

namespace agl { namespace shadow_text {
void assureShadowTermination(void*);
sead::SafeTextVtable* definitionTextTable();
sead::SafeTextVtable* definitionBufferTable();
sead::SafeTextVtable* ownedIncludeBufferTable();
} }
