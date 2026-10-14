/** HD locale suffix predicates and layout/resource lookup adapters. Private decomp source. */
#include "d/hd_layout_locale.h"
#include "bindings.h"
static u32 sLayoutLocaleHeaderStatics;
static u32 sLayoutLocaleHeaderDescriptors;
static u32 sLayoutLocaleHeaderZero;
static u32 sLayoutLocalePiConstants;
static u32 sLayoutLocaleStringVTable;

/* 02705638 */
u32 hdLayoutLocale_c::getSuffixLength() { return 5; }

/* 02705640 */
u32 hdLayoutLocale_c::getSuffixCount() { return 13; }

/* 02705894 */
u32 hdLayoutLocale_c::bufferHasAnySuffix(LayoutLocaleBuffer_l* buffer) {
    LayoutLocaleString_l name;
    name.mpBuffer = buffer->mBuffer;
    name.mpVTable = reinterpret_cast<gfn*>(&sLayoutLocaleStringVTable);
    return hasAnySuffix(&name);
}

/* 02705AFC */
u32 hdLayoutLocale_c::bufferHasCurrentSuffix(LayoutLocaleBuffer_l* buffer) {
    LayoutLocaleString_l name;
    name.mpBuffer = buffer->mBuffer;
    name.mpVTable = reinterpret_cast<gfn*>(&sLayoutLocaleStringVTable);
    return hasCurrentSuffix(&name);
}

/* 02705B34 */
u32 hdLayoutLocale_c::bufferHasOtherSuffix(LayoutLocaleBuffer_l* buffer) {
    if (!bufferHasAnySuffix(buffer)) return 0;
    return bufferHasCurrentSuffix(buffer) == 0;
}

/* 02705EEC */
LayoutLocalePane_l* hdLayoutLocale_c::findCurrentPane(LayoutLocaleString_l* name, LayoutLocaleRoot_l* layout) {
    const be<u8>* suffixBuffer = LayoutLocale_currentSuffix();
    LayoutLocalePane_l* root = layout->mpRoot;
    LayoutLocaleString_l suffix;
    suffix.mpBuffer = suffixBuffer;
    suffix.mpVTable = reinterpret_cast<gfn*>(&sLayoutLocaleStringVTable);
    return findPane(name, root, &suffix);
}

/* 02705F44 */
LayoutLocalePane_l* hdLayoutLocale_c::findCurrentPaneBuffer(LayoutLocaleBuffer_l* buffer, LayoutLocaleRoot_l* layout) {
    LayoutLocaleString_l name;
    name.mpBuffer = buffer->mBuffer;
    name.mpVTable = reinterpret_cast<gfn*>(&sLayoutLocaleStringVTable);
    return findCurrentPane(&name, layout);
}

/* 02705F7C */
LayoutLocaleEntry_l* hdLayoutLocale_c::findEntry(const be<u8>* name, LayoutLocaleEntry_l* entries, s32 count, u32 type) {
    if (!name || !entries || count <= 0) return NULL;
    for (s32 i = 0; i < count; ++i) {
        LayoutLocaleEntry_l* entry = &entries[i];
        u32 offset = entry->mNameOffset;
        const be<u8>* entryName = gabi::at<const be<u8>>(gabi::ea(entry) + offset);
        if (!offset || !entryName || u32(entry->mType) != type || entry->mSize == 0) continue;
        const be<u8>* left = name;
        const be<u8>* right = entryName;
        u32 a, b;
        do {
            a = *left++;
            b = *right++;
        } while (a == b && a != 0);
        if (a == b) return entry;
    }
    return NULL;
}

/* 02706014 */
static void __sinit_hd_layout_locale_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sLayoutLocaleHeaderStatics), gabi::ea(&sLayoutLocaleHeaderDescriptors), gabi::ea(&sLayoutLocaleHeaderZero), gabi::ea(&sLayoutLocalePiConstants));
}

/* WWHD glue
data sLayoutLocaleHeaderStatics 1049E7EC nocheck
data sLayoutLocaleHeaderDescriptors 101F746C nocheck
data sLayoutLocaleHeaderZero 1049E7F8 nocheck
data sLayoutLocalePiConstants 1010C0F0 nocheck
 data sLayoutLocaleStringVTable 1010C0D8
 ext 02705568 const be<u8>* LayoutLocale_currentSuffix()
*/
