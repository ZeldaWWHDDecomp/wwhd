/** HD layout suffix selection from system region and language. Private decomp source. */
#include "d/hd_text_locale.h"
#include "bindings.h"
static u32 sTextLocaleHeaderStatics;
static u32 sTextLocaleHeaderDescriptors;
static u32 sTextLocaleHeaderZero;
static u32 sTextLocalePiConstants;
static TextLocaleConfig_l* sConfig;
struct TextLocaleSuffixes { gptr<const char> mSuffixes[14]; };
static TextLocaleSuffixes sSuffixes;
extern void* TextLocale_allocate(u32);

/* 0270517C */
u32 hdTextLocale_c::getRegion() {
    switch (sConfig->mRegion) {
    case 1: return JAPAN;
    case 4: return EUROPE;
    default: return AMERICA;
    }
}

/* 027051B0 */
u32 hdTextLocale_c::getLanguage() {
    u32 language = sConfig->mLanguage;
    if (language == 0) return JAPANESE;
    if (language >= 2 && language <= 5) return language;
    if (language == 8) return DUTCH;
    if (language == 9) return PORTUGUESE;
    if (language == 10) return RUSSIAN;
    return ENGLISH;
}

/* 0270521C */
static hdTextLocale_c* hdTextLocale_construct(hdTextLocale_c* locale, u32 region, u32 language) {
    using C = hdTextLocale_c;
    u32 value = C::FALLBACK;
    switch (region) {
    case C::JAPAN:
        if (language == C::JAPANESE) value = C::JP_JA;
        break;
    case C::AMERICA:
        switch (language) {
        case C::ENGLISH: value = C::US_EN; break;
        case C::FRENCH: value = C::US_FR; break;
        case C::PORTUGUESE: value = C::US_PO; break;
        case C::SPANISH: value = C::US_SP; break;
        }
        break;
    case C::EUROPE:
        switch (language) {
        case C::ENGLISH: value = C::EU_EN; break;
        case C::DUTCH: value = C::EU_DU; break;
        case C::GERMAN: value = C::EU_GE; break;
        case C::FRENCH: value = C::EU_FR; break;
        case C::PORTUGUESE: value = C::EU_PO; break;
        case C::RUSSIAN: value = C::EU_RU; break;
        case C::SPANISH: value = C::EU_SP; break;
        case C::ITALIAN: value = C::EU_IT; break;
        }
        break;
    }
    if (!locale) {
        locale = static_cast<hdTextLocale_c*>(TextLocale_allocate(sizeof(hdTextLocale_c)));
        if (!locale) return NULL;
    }
    locale->mLocale = value;
    return locale;
}

/* 02705554 */
const char* hdTextLocale_c::getSuffix(u32 locale) {
    return sSuffixes.mSuffixes[locale];
}

/* 02705568 */
const char* hdTextLocale_c::getCurrentSuffix() {
    u32 region = getRegion();
    u32 language = getLanguage();
    hdTextLocale_c locale;
    hdTextLocale_construct(&locale, region, language);
    return getSuffix(locale.mLocale);
}

/* 027055A4 */
static void __sinit_hd_text_locale_cpp() {
    sinit_header_statics_rodata(gabi::ea(&sTextLocaleHeaderStatics), gabi::ea(&sTextLocaleHeaderDescriptors), gabi::ea(&sTextLocaleHeaderZero), gabi::ea(&sTextLocalePiConstants));
}

/* WWHD glue
data sTextLocaleHeaderStatics 1049E7D0 nocheck
data sTextLocaleHeaderDescriptors 101F7414 nocheck
data sTextLocaleHeaderZero 1049E7DC nocheck
data sTextLocalePiConstants 1010C0CC nocheck
 data sConfig 101F4BAC
 data sSuffixes 101F7438
 ext 0273AD10 void* TextLocale_allocate(u32)
*/
