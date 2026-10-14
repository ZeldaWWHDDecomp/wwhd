/** HD locale suffix matching and recursive pane search. Private game-derived source. */
#include "d/hd_layout_locale_search.h"

static u32 sLocaleSearchStringVTable;
static u32 sPaneTypeGuard;
static u32 sRootTypeGuard;
static LocalePaneType_l sPaneType;
static LocalePaneType_l sRootType;
static u32 sTextPaneTypeGuard;
static u32 sDerivedPaneTypeGuard;
static LocalePaneType_l sTextPaneType;
static LocalePaneType_l sDerivedPaneType;
const be<u8> *LayoutLocale_suffixForIndex(s32 index);
void LayoutLocale_assureString(LayoutLocaleString_l *string);

static void refreshString(LayoutLocaleString_l *string) {
    gabi::call_ptr<void>(string->mpVTable[0x14 / 4].get(), string);
}
static s32 bufferLength(const be<u8> *buffer) {
    s32 length = 0;
    while (*buffer != 0) {
        if (++length > 0x40000)
            return 0;
        ++buffer;
    }
    return length;
}
static s32 stringLength(LayoutLocaleString_l *string) {
    refreshString(string);
    return bufferLength(string->mpBuffer);
}
static bool equalPrefix(const be<u8> *left, const be<u8> *right, s32 length) {
    if (left == right || length <= 0)
        return true;
    for (s32 i = 0; i < length; ++i) {
        u32 a = left[i];
        u32 b = right[i];
        if (a == 0)
            return b == 0;
        if (b == 0 || a != b)
            return false;
    }
    return true;
}
/* inlined into locale suffix matching and pane search */
static s32 findLastSuffix(LayoutLocaleString_l *name, LayoutLocaleString_l *suffix, bool refreshName) {
    s32 nameLength = refreshName ? stringLength(name) : bufferLength(name->mpBuffer);
    s32 suffixLength = stringLength(suffix);
    s32 position = nameLength - suffixLength;
    if (position < 0)
        return -1;
    do {
        LayoutLocaleString_l tail;
        tail.mpBuffer = name->mpBuffer.get() + position;
        tail.mpVTable = reinterpret_cast<gfn *>(&sLocaleSearchStringVTable);
        LayoutLocale_assureString(&tail);
        refreshString(&tail);
        const be<u8> *tailBuffer = tail.mpBuffer;
        refreshString(suffix);
        if (tailBuffer == suffix->mpBuffer.get() ||
            equalPrefix(tail.mpBuffer, suffix->mpBuffer, suffixLength))
            return position;
    } while (--position >= 0);
    return -1;
}

/* 02705648 */
u32 hdLayoutLocale_c::hasAnySuffix(LayoutLocaleString_l *name) {
    s32 length = stringLength(name);
    s32 count = getSuffixCount();
    for (s32 i = 0; i < count; ++i) {
        LayoutLocaleString_l suffix;
        suffix.mpBuffer = LayoutLocale_suffixForIndex(i);
        suffix.mpVTable = reinterpret_cast<gfn *>(&sLocaleSearchStringVTable);
        s32 position = findLastSuffix(name, &suffix, true);
        if (position != -1 && u32(length - position) == getSuffixLength())
            return 1;
    }
    return 0;
}

/* 027058CC */
u32 hdLayoutLocale_c::hasCurrentSuffix(LayoutLocaleString_l *name) {
    LayoutLocaleString_l suffix;
    suffix.mpBuffer = LayoutLocale_currentSuffix();
    suffix.mpVTable = reinterpret_cast<gfn *>(&sLocaleSearchStringVTable);
    s32 position = findLastSuffix(name, &suffix, true);
    if (position == -1)
        return 0;
    s32 length = stringLength(name);
    return u32(length - position) == getSuffixLength();
}

/* 02705B94 */
LayoutLocalePane_l *hdLayoutLocale_c::findPane(LayoutLocaleString_l *name, LayoutLocalePane_l *root,
                                               LayoutLocaleString_l *suffix) {
    if (!sPaneTypeGuard) {
        sPaneTypeGuard = 1;
        if (!sRootTypeGuard) {
            sRootTypeGuard = 1;
            sRootType.mpBase = NULL;
        }
        sPaneType.mpBase = &sRootType;
    }
    LocaleSearchPane_l *pane = reinterpret_cast<LocaleSearchPane_l *>(root);
    if (pane) {
        LocalePaneType_l *type = gabi::call_ptr<LocalePaneType_l *>(pane->mpVTable[0x14 / 4].get(), pane);
        while (type && type != &sPaneType)
            type = type->mpBase;
        if (type) {
            LayoutLocaleString_l paneName;
            paneName.mpBuffer = pane->mName;
            paneName.mpVTable = reinterpret_cast<gfn *>(&sLocaleSearchStringVTable);
            LayoutLocale_assureString(&paneName);
            s32 position = findLastSuffix(&paneName, suffix, false);
            if (position != -1) {
                refreshString(name);
                refreshString(name);
                const be<u8> *nameBuffer = name->mpBuffer;
                refreshString(&paneName);
                if (nameBuffer == paneName.mpBuffer.get() ||
                    equalPrefix(name->mpBuffer, paneName.mpBuffer, position > 0x40000 ? 0x40000 : position))
                    return root;
            }
        }
    }
    // HD: children use an intrusive sentinel rooted in the parent's child-list field.
    LocaleSearchPane_l *child = pane->mpFirstChild;
    LocaleSearchPane_l *sentinel = reinterpret_cast<LocaleSearchPane_l *>(&pane->mpFirstChild);
    while (child != sentinel) {
        LayoutLocalePane_l *found = findPane(name, reinterpret_cast<LayoutLocalePane_l *>(child), suffix);
        if (found)
            return found;
        child = child->mpNext;
    }
    return NULL;
}

/* 02704C24 */
static LocalePaneType_l *LayoutLocale_getTextPaneType() {
    if (sTextPaneTypeGuard)
        return &sTextPaneType;
    sTextPaneTypeGuard = 1;
    if (!sDerivedPaneTypeGuard) {
        sDerivedPaneTypeGuard = 1;
        if (!sPaneTypeGuard) {
            sPaneTypeGuard = 1;
            if (!sRootTypeGuard) {
                sRootTypeGuard = 1;
                sRootType.mpBase = NULL;
            }
            sDerivedPaneType.mpBase = &sPaneType;
            sPaneType.mpBase = &sRootType;
        } else {
            sDerivedPaneType.mpBase = &sPaneType;
        }
    }
    sTextPaneType.mpBase = &sDerivedPaneType;
    return &sTextPaneType;
}

/* WWHD glue
 data sLocaleSearchStringVTable 1010C0D8
 data sPaneTypeGuard 101FD89C
 data sRootTypeGuard 101FD888
 data sPaneType 101FD8F8
 data sRootType 101FD8FC
 data sTextPaneTypeGuard 101FDD40
 data sDerivedPaneTypeGuard 101FD8F0
 data sTextPaneType 101FDD3C
 data sDerivedPaneType 101FD8F4
 ext 02705554 const be<u8>* LayoutLocale_suffixForIndex(s32)
 ext 027060BC void LayoutLocale_assureString(LayoutLocaleString_l*)
*/
