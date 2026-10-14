#pragma once
#include "wwhd.h"
struct TextLocaleConfig_l { u8 mReserved[0x10]; be<u32> mRegion, mLanguage; };
WWHD_OFFSET(TextLocaleConfig_l, mRegion, 0x10);
WWHD_OFFSET(TextLocaleConfig_l, mLanguage, 0x14);
struct hdTextLocale_c {
    enum Region { JAPAN, AMERICA, EUROPE };
    enum Language { JAPANESE, ENGLISH, FRENCH, GERMAN, ITALIAN, SPANISH, PORTUGUESE, RUSSIAN, DUTCH };
    enum Locale { JP_JA, US_EN, US_FR, US_PO, US_SP, EU_EN, EU_DU, EU_GE, EU_FR, EU_PO, EU_RU, EU_SP, EU_IT, FALLBACK };
    be<u32> mLocale;
    static u32 getRegion();
    static u32 getLanguage();
    static const char* getSuffix(u32 locale);
    static const char* getCurrentSuffix();
};
WWHD_SIZE(hdTextLocale_c, 4);
