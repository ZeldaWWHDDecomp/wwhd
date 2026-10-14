/* HD Unicode line-break classification. Tables remain in the local RPX image. */
#include "d/hd_unicode_class.h"
#include "bindings.h"
static u32 sUnicodeClassHeaderStatics;
static u32 sUnicodeClassHeaderDescriptors;
static u32 sUnicodeClassHeaderZero;
static u32 sUnicodeClassPiConstants;
struct hdUnicodeClasses_c {
    hdUnicodeEntry_c mEntries[25734];
};
struct hdUnicodeBreakRules_c {
    be<u32> mRules[29 * 29];
};
static hdUnicodeClasses_c sClasses;
static hdUnicodeBreakRules_c sRules;

/* 027070D0 */
u32 hdUnicode_findClass(u32 code, hdUnicodeEntry_c *entries, s32 first, s32 last) {
    for (;;) {
        s32 distance = static_cast<s32>(static_cast<u32>(last) - static_cast<u32>(first)) / 2;
        if (distance <= 0) {
            u32 firstCode = entries[first].mCode;
            u32 lastClass = entries[last].mClass;
            u32 lastCode = entries[last].mCode;
            u32 firstClass = entries[first].mClass;
            if (firstCode == code)
                return firstClass;
            if (lastCode == code)
                return lastClass;
            return 14;
        }
        s32 middle = first + distance;
        u32 middleCode = entries[middle].mCode;
        u32 middleClass = entries[middle].mClass;
        if (middleCode < code)
            first = middle;
        else if (middleCode > code)
            last = middle;
        else
            return middleClass;
    }
}

/* 0270715C */
u32 hdUnicode_getClass(u32 code) {
    return hdUnicode_findClass(code, sClasses.mEntries, 0, 25733);
}

/* 02707170 */
u32 hdUnicode_getBreakRule(u32 previous, be<u32> *next) {
    return sRules.mRules[previous * 29 + *next];
}

/* 02707190 */
static void __sinit_hd_unicode_class_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sUnicodeClassHeaderStatics), gabi::ea(&sUnicodeClassHeaderDescriptors), gabi::ea(&sUnicodeClassHeaderZero), gabi::ea(&sUnicodeClassPiConstants));
}

/* WWHD glue
data sUnicodeClassHeaderStatics 1049E824 nocheck
data sUnicodeClassHeaderDescriptors 101F74B4 nocheck
data sUnicodeClassHeaderZero 1049E830 nocheck
data sUnicodeClassPiConstants 1013E558 nocheck
 data sClasses 1010C128
 data sRules 101F74D8
*/
