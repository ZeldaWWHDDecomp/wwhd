// HD: Message line breaking, character conversion, numbers, and localized unit strings.
#include "d/hd_text_unit.h"

using namespace hd_text_unit;

static const u16 fontCharacterMap[192] = {0x8340, 0x30a1, 0x8341, 0x30a2, 0x8342, 0x30a3, 0x8343, 0x30a4, 0x8344, 0x30a5, 0x8345, 0x30a6, 0x8346, 0x30a7, 0x8347, 0x30a8, 0x8348, 0x30a9, 0x8349, 0x30aa, 0x834a, 0x30ab, 0x834b, 0x30ac, 0x834c, 0x30ad, 0x834d, 0x30ae, 0x834e, 0x30af, 0x834f, 0x30b0, 0x8350, 0x30b1, 0x8351, 0x30b2, 0x8352, 0x30b3, 0x8353, 0x30b4, 0x8354, 0x30b5, 0x8355, 0x30b6, 0x8356, 0x30b7, 0x8357, 0x30b8, 0x8358, 0x30b9, 0x8359, 0x30ba, 0x835a, 0x30bb, 0x835b, 0x30bc, 0x835c, 0x30bd, 0x835d, 0x30be, 0x835e, 0x30bf, 0x835f, 0x30c0, 0x8360, 0x30c1, 0x8361, 0x30c2, 0x8362, 0x30c3, 0x8363, 0x30c4, 0x8364, 0x30c5, 0x8365, 0x30c6, 0x8366, 0x30c7, 0x8367, 0x30c8, 0x8368, 0x30c9, 0x8369, 0x30ca, 0x836a, 0x30cb, 0x836b, 0x30cc, 0x836c, 0x30cd, 0x836d, 0x30ce, 0x836e, 0x30cf, 0x836f, 0x30d0, 0x8370, 0x30d1, 0x8371, 0x30d2, 0x8372, 0x30d3, 0x8373, 0x30d4, 0x8374, 0x30d5, 0x8375, 0x30d6, 0x8376, 0x30d7, 0x8377, 0x30d8, 0x8378, 0x30d9, 0x8379, 0x30da, 0x837a, 0x30db, 0x837b, 0x30dc, 0x837c, 0x30dd, 0x837d, 0x30de, 0x837e, 0x30df, 0x8380, 0x30e0, 0x8381, 0x30e1, 0x8382, 0x30e2, 0x8383, 0x30e3, 0x8384, 0x30e4, 0x8385, 0x30e5, 0x8386, 0x30e6, 0x8387, 0x30e7, 0x8388, 0x30e8, 0x8389, 0x30e9, 0x838a, 0x30ea, 0x838b, 0x30eb, 0x838c, 0x30ec, 0x838d, 0x30ed, 0x838e, 0x30ee, 0x838f, 0x30ef, 0x8392, 0x30f2, 0x8393, 0x30f3, 0x8140, 0x3000, 0x824f, 0xff10, 0x8250, 0xff11, 0x8251, 0xff12, 0x8252, 0xff13, 0x8253, 0xff14, 0x8254, 0xff15, 0x8255, 0xff16, 0x8256, 0xff17, 0x8257, 0xff18, 0x8258, 0xff19, 0x815b, 0x30fc, 0x8141, 0xff61, 0x8175, 0xff62, 0x8176, 0xff63};
static const u16 lowercaseAccents[25] = {0xe1, 0xe2, 0xe4, 0xe0, 0xe9, 0xea, 0xeb, 0xe8, 0xed, 0xef, 0xee, 0xec, 0xf3, 0xf4, 0xf2, 0xf6, 0xfa, 0xfb, 0xf9, 0xfc, 0xe7, 0xff, 0xf1, 0x153, 0xe6};
static const u16 uppercaseAccents[25] = {0xc1, 0xc2, 0xc4, 0xc0, 0xc9, 0xca, 0xcb, 0xc8, 0xcd, 0xcf, 0xce, 0xcc, 0xd3, 0xd4, 0xd2, 0xd6, 0xda, 0xdb, 0xd9, 0xdc, 0xc7, 0x178, 0xd1, 0x152, 0xc6};
static const f32 unlimitedWidth[1] = {0.0f};
static const u16 wideNewline[2] = {0xa, 0x0};
static const u16 defaultPlayerName[5] = {0x4c, 0x69, 0x6e, 0x6b, 0x0};

static const char* const asciiDigitLiterals[10] = {"0", "1", "2", "3", "4",
                                 "5", "6", "7", "8", "9"};

/* are the characters between two line ends only tags (no pictograph tag)? */
/* 025FB9BC */
u32 hdTextUnit_c::onlyTagsBetween(u32 buf, s32 start, s32 end) {

    s32 i = start + 1;
    if (i >= end) return 1;
    do {
        const u32 p = buf + u32(i) * 2;
        const u32 c = load<u16>(p);
        if (c != hdUnitTag_c::BEGIN_TAG && c != hdUnitTag_c::END_TAG) return 0;
        const u32 c0 = load<u16>(p);
        u32 next;
        if (c0 == hdUnitTag_c::BEGIN_TAG) {
            next = p + load<u16>(p + 6) + 8;
            if (load<u16>(p + 2) == 3) return 0;
        } else if (c0 == hdUnitTag_c::END_TAG) {
            if (load<u16>(p + 2) == 3) return 0;
            next = p + 6;
        } else {
            // HD: Retain the emitted fallback read after the repeated tag-code test.
            if (load<u16>(2) == 3) return 0;
            next = p;
        }
        i = i + ((s32(next - p) >> 1) - 1) + 1;
    } while (i < end);
    return 1;
}

/* cuts the text after the last recorded line end and removes the breaks between lines */
/* 025FBA64 */
void hdTextUnit_c::cutLines(hdUnitBufferedString_c* i_string) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    u32 m = msg(self);
    const s32 n = s32(u32(messageAt(m).mLineCount));
    if (!n) return;
    const u32 i0 = u32(n - 1);
    s32 cur = s32(u32(messageAt(m).mLineValues[i0 < 5 ? i0 : 0]));
    Local<hdUnitSafeString_c> T18;
    wtmp(T18.a, gabi::ea(&wideNewline[1]));
    wcopyAt(w, cur, T18.a, 1);
    if (n < 2) return;
    s32 k = n - 2;
    Local<hdUnitSafeString_c> T10;
    Local<hdUnitSafeString_c> T8;
    for (s32 left = n - 1; left; left--, k--) {
        m = msg(self);
        const s32 prev = cur;
        cur = s32(u32(messageAt(m).mLineValues[u32(k) < 5 ? u32(k) : 0]));
        s32 gap = prev - cur;
        if (gap <= 1) {
            wtmp(T10.a, gabi::ea(&wideNewline[1]));
            wcopyAt(w, cur, T10.a, 1);
            continue;
        }
        vAssure(w);
        const u32 buf = u32(stringAt(w).mBuffer);
        if (!textUnitNative_onlyTagsBetween_4(self, buf, u32(cur), u32(prev))) return;
        if (cur < prev) {
            u32 src = buf + u32(cur) * 2;
            for (; gap; gap--, cur++, src += 2) {
                stringAt(T8.a).mVtable = kWSafeVt;
                stringAt(T8.a).mBuffer = src + 2;
                wcopyAt(w, cur, T8.a, 1);
            }
        }
        wtmp(T10.a, gabi::ea(&wideNewline[1]));
        cur = prev - 1;
        wcopyAt(w, cur, T10.a, 1);
    }
}

/* is the box full (line count reached)? then the text is cut there */
/* 025FBE54 */
u32 hdTextUnit_c::boxFull(hdUnitBufferedString_c* i_string) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    const u32 m = msg(self);
    const u32 num = u32(messageAt(m).mMessageNumber);
    s32 maxLines = s32(u32(messageAt(m).mTextStyle));
    if (num == 0xFFFFFFF0 || (num <= 0xEE3 && (num >= 0xEE2 || num == 0xEDD || num == 0x266))) maxLines++;
    if (s32(u32(messageAt(m).mLineCount)) < maxLines) return 0;
    if (!u32(messageAt(m).mMessageNumber) && maxLines == 1) {
        /* debug report "[WARNING!] Over Style Setting!" (the print itself is compiled out) */
        Local<hdUnitFormatFrame_c> F;
        const u32 s = gabi::ea(&F.get()->mHeader), buf = gabi::ea(F.get()->mBuffer);
        F.get()->mBuffer[255] = 0;
        stringAt(s).mBuffer = buf;
        F.get()->mBuffer[0] = 0;
        bufferAt(s).mCapacity = 0x100;
        stringAt(s).mVtable = kBufferedString256Vt;
        textUnitNative_formatString_2(s, gabi::guest_addr("[WARNING!]\011Over Style Setting!\012"));
        vAssure(s);
        const u32 mm = msg(self);
        vAssure(gabi::ea(&messageAt(mm).mLabel));
        textUnitNative_formatString_3(s, gabi::guest_addr("[WARNING!]\011TextLabel:%s\012"), gabi::ea(messageAt(mm).mLabel.mString.mpBuffer.get()));
        vAssure(s);
        return 0;
    }
    vAssure(w);
    textUnitNative_cutLines_2(self, w);
    messageAt(msg(self)).mFlag69C = 0;
    messageAt(msg(self)).mFlag696 = 0;
    return 1;
}

/* line break: records the line, inserts '\n' when the box takes it; -1 when full */
/* 025FBFE8 */
s32 hdTextUnit_c::newLine(hdUnitBufferedString_c* i_string, s32 pos, u32 next) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    s32 r = 0;
    textUnitNative_recordLineEnd_2(msg(self), u32(pos));
    if (u8(messageAt(msg(self)).mLineReady)) {
        Local<hdUnitSafeString_c> T;
        wtmp(T.a, gabi::ea(wideNewline));
        wcopyAt(w, pos, T.a, 1);
        r = 1;
        pos++;
    }
    if (textUnitNative_boxFull_4(self, w, u32(pos), next)) r = -1;
    return r;
}

/* character replacement table (font-specific forms) */
/* 025FC154 */
u32 hdTextUnit_c::mapChar(u32 c) {

    for (u32 i = 0; i < 0x60; i++)
        if (u16(fontCharacterMap[2 * i]) == c) return u16(fontCharacterMap[2 * i + 1]);
    return c;
}

/* capitalises the character after a capital tag (ASCII and the accented table) */
/* 025FC184 */
void hdTextUnit_c::capitalize(u32 chp) {
    const u32 self = gabi::ea(this);
    const u32 m = msg(self);
    const u32 f = u8(messageAt(m).mFlag922);
    const u32 c = load<u16>(chp);
    if (!f) return;
    messageAt(m).mFlag922 = 0;
    if (c - 0x61 < 0x1A) {
        store<u16>(chp, u16(c - 0x20));
        return;
    }
    for (u32 i = 0; i < 0x19; i++)
        if (c == u16(lowercaseAccents[i])) {
            store<u16>(chp, u16(uppercaseAccents[i]));
            return;
        }
}

/* puts one character (line break when it does not fit) */
/* 025FC1F4 */
s32 hdTextUnit_c::putChar(hdUnitBufferedString_c* i_string, s32 pos, u32 chp) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    Local<hdUnitCharacterFrame_c> L; /* +0 the character, +4 WSafeString over it */
    const u32 ch = gabi::ea(&L.get()->mCharacter), T = gabi::ea(&L.get()->mString);
    const u32 m0 = msg(self);
    L.get()->mCharacter = load<u16>(chp);
    s32 r = 0;
    if (u8(messageAt(m0).mPictographFont)) L.get()->mCharacter = u16(textUnitNative_mapChar_2(self, u32(load<u16>(chp))));
    textUnitNative_advanceCharacter_2(m0, ch);
    const u32 m = msg(self);
    const f32 lim = f32(messageAt(m).mLayoutWidth);
    if (!(lim == f32(unlimitedWidth[0])) && f32(messageAt(m).mCurrentLineWidth) > lim && !u8(messageAt(m).mFlag69A)) {
        r = s32(textUnitNative_newLine_4(self, w, u32(pos), chp + 2));
        if (r < 0) return -1;
        pos += r;
        textUnitNative_advanceCharacter_2(msg(self), ch);
    }
    textUnitNative_capitalize_2(self, ch);
    wtmp(T, ch);
    wcopyAt(w, pos, T, 1);
    const u32 mm = msg(self);
    messageAt(mm).mLayoutProgress = u32(messageAt(mm).mLayoutProgress) + 1;
    return r + 1;
}

/* puts n characters (stops at a full box) */
/* 025FC3D0 */
s32 hdTextUnit_c::putCharacters(hdUnitBufferedString_c* i_string, s32 pos, u32 chars, s32 n) {
    const u32 w = gabi::ea(i_string);

    s32 total = 0;
    for (; n > 0; n--, chars += 2) {
        const s32 r = putChar(gabi::at<hdUnitBufferedString_c>(w), pos, chars);
        if (r < 0) break;
        total += r;
        pos += r;
    }
    return total;
}

/* puts the player's name (the default name when it is empty) */
/* 025FC444 */
s32 hdTextUnit_c::putPlayerName(hdUnitBufferedString_c* i_string, s32 pos) {
    const u32 w = gabi::ea(i_string);

    const u32 name = textUnitNative_getPlayerName_1(gabi::ea(&gabi::at<hdUnitSaveInfo_c>(saveInfo())->mOptions));
    vAssure(name);
    Local<hdUnitPlayerNameFrame_c> F; /* +0 WSafeString over the name, +8 WFixedSafeString<32> */
    const u32 T = gabi::ea(&F.get()->mName);
    const u32 W = gabi::ea(&F.get()->mWideName), Wbuf = gabi::ea(F.get()->mWideName.mBuffer);
    stringAt(T).mBuffer = u32(stringAt(name).mBuffer);
    F.get()->mWideName.mBuffer[31] = 0;
    bufferAt(W).mCapacity = 0x20;
    stringAt(W).mVtable = kWideBufferedString32Vt;
    stringAt(W).mBuffer = Wbuf;
    stringAt(T).mVtable = kWSafeVt;
    textUnitNative_assureWideString_1(T);
    s32 n = wcsLength(u32(stringAt(T).mBuffer));
    if (n >= s32(u32(bufferAt(W).mCapacity))) n = s32(u32(bufferAt(W).mCapacity)) - 1;
    vAssure(T);
    textUnitNative_moveBytes_4(Wbuf, u32(stringAt(T).mBuffer), u32(n) * 2, 0u);
    store<u16>(Wbuf + u32(n) * 2, 0);
    stringAt(W).mVtable = kWFixed32Vt;
    textUnitNative_assureWideBuffer_1(W);
    u32 top = u32(stringAt(W).mBuffer);
    if (!wcsLength(top)) {
        staticStr(gabi::ea(&textUnitStatics().defaultNameGuard), gabi::ea(&textUnitStatics().defaultName), gabi::ea(defaultPlayerName), kWSafeVt, staticRecordAddress(RECORD_defaultName));
        top = u32(stringAt(W).mBuffer);
        vAssure(gabi::ea(&textUnitStatics().defaultName));
        s32 k = wcsLength(u32(textUnitStatics().defaultName.mBuffer));
        if (k >= s32(u32(bufferAt(W).mCapacity))) k = s32(u32(bufferAt(W).mCapacity)) - 1;
        vAssure(gabi::ea(&textUnitStatics().defaultName));
        textUnitNative_moveBytes_4(top, u32(textUnitStatics().defaultName.mBuffer), u32(k) * 2, 0u);
        store<u16>(top + u32(k) * 2, 0);
    }
    vAssure(W);
    const s32 len = wcsLength(u32(stringAt(W).mBuffer));
    vAssure(W);
    return putCharacters(gabi::at<hdUnitBufferedString_c>(w), pos, u32(stringAt(W).mBuffer), len) == len ? len : 0;
}

/* FixedSafeString<32> dst += string str (sead append as inlined) */
static void appendStr(u32 dst, u32 base, u32 str) {
    vAssure(dst);
    const s32 cur = cstrLength(u32(stringAt(dst).mBuffer));
    const s32 c0 = cur < 0 ? 0 : cur;
    vAssure(str);
    s32 n = cstrLength(u32(stringAt(str).mBuffer));
    const s32 avail = s32(u32(bufferAt(dst).mCapacity)) - c0;
    if (n >= avail) n = avail - 1;
    if (n <= 0) return;
    vAssure(str);
    textUnitNative_moveBytes_4(base + u32(c0), u32(stringAt(str).mBuffer), u32(n), 0u);
    if (c0 + n > cur) store<u8>(base + u32(c0 + n), 0);
}

/* puts a number with the given digit strings (padded to maxDigits unless noPad) */
/* 025FC708 */
s32 hdTextUnit_c::putNumber(hdUnitBufferedString_c* i_string, s32 pos, s32 v, u32 digits, u32 maxDigits, u32 noPad) {
    const u32 w = gabi::ea(i_string);

    Local<hdUnitNumberFrame_c> F; /* +0 FixedSafeString<32>, +0x2C WFixedSafeString<32>, +0x78 digits[10] */
    const u32 S = gabi::ea(&F.get()->mNarrow), Sbuf = gabi::ea(F.get()->mNarrow.mBuffer);
    const u32 W = gabi::ea(&F.get()->mWide), Wbuf = gabi::ea(F.get()->mWide.mBuffer);
    F.get()->mWide.mBuffer[31] = 0;
    stringAt(W).mBuffer = Wbuf;
    bufferAt(S).mCapacity = 0x20;
    bufferAt(W).mCapacity = 0x20;
    F.get()->mWide.mBuffer[0] = 0;
    stringAt(S).mBuffer = Sbuf;
    F.get()->mNarrow.mBuffer[31] = 0;
    stringAt(W).mVtable = kWFixed32Vt;
    stringAt(S).mVtable = kFixedString32Vt;
    F.get()->mNarrow.mBuffer[0] = 0;
    for (u32 i = 0; i < 10; i++) F.get()->mDigits[i] = 0;
    u32 cnt = 0;
    if (maxDigits) {
        u32 left = maxDigits;
        for (;;) {
            const u32 slot = cnt < 10 ? cnt : 0;
            const s32 d = v % 10;
            v = v / 10;
            cnt++;
            left = (left - 1) & 0xFF;
            F.get()->mDigits[slot] = u32(d);
            if (!v || !left) break;
        }
    } else {
        for (;;) {
            const s32 d = v % 10;
            v = v / 10;
            F.get()->mDigits[cnt < 10 ? cnt : 0] = u32(d);
            cnt++;
            if (!v) break;
        }
        maxDigits = 5;
    }
    if (!noPad) {
        for (s32 pad = s32(maxDigits) - s32(cnt); pad > 0; pad--) appendStr(S, u32(stringAt(S).mBuffer), digits);
    }
    while (cnt) {
        cnt--;
        const u32 d = u32(F.get()->mDigits[cnt < 10 ? cnt : 0]);
        appendStr(S, u32(stringAt(S).mBuffer), digits + d * 8);
    }
    vAssure(W);
    const u32 wtop = u32(stringAt(W).mBuffer);
    vAssure(S);
    const s32 n = s32(textUnitNative_convertToWide_4(wtop, u32(bufferAt(W).mCapacity), u32(stringAt(S).mBuffer), 0xFFFFFFFFu));
    vAssure(W);
    return putCharacters(gabi::at<hdUnitBufferedString_c>(w), pos, u32(stringAt(W).mBuffer), n) == n ? n : 0;
}

/* puts a number with ASCII digits */
/* 025FCB90 */
s32 hdTextUnit_c::putAsciiNumber(hdUnitBufferedString_c* i_string, s32 pos, s32 v, u32 maxDigits, u32 noPad) {
    const u32 w = gabi::ea(i_string);

    staticArr(gabi::ea(&textUnitStatics().asciiDigitsGuard), gabi::ea(&textUnitStatics().asciiDigits), asciiDigitLiterals, 10, kSafeVt, staticRecordAddress(RECORD_asciiDigits));
    return putNumber(gabi::at<hdUnitBufferedString_c>(w), pos, v, gabi::ea(&textUnitStatics().asciiDigits), maxDigits, noPad);
}

/* number of leading '0' characters */
/* 025FCDE0 */
s32 hdTextUnit_c::leadingZeros(u32 s) {

    vAssure(s);
    u32 p = u32(stringAt(s).mBuffer);
    s32 n = 0;
    if (!p || !load<u16>(p)) return 0;
    while (load<u16>(p) == 0x30) {
        p += 2;
        n++;
        if (!load<u16>(p)) break;
    }
    return n;
}

/* puts the "unitString" text of a label (leading '0' placeholders skipped; a lone space
 * puts nothing) */
/* 025FCE5C */
s32 hdTextUnit_c::putLocalizedUnit(hdUnitBufferedString_c* i_string, s32 pos, u32 label) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    const u32 set = textUnitNative_getUnitSet_1(resourceManagerAddress());
    if (!set) return 0;
    vAssure(label);
    const u32 li = textUnitNative_findLabel_2(gabi::ea(gabi::at<HdMsgSet>(set)->mpData.get()), u32(stringAt(label).mBuffer));
    u32 txt = 0;
    if (li < u32(gabi::at<HdMsgSet>(set)->mEntryCount)) txt = textUnitNative_getLabelText_2(gabi::ea(gabi::at<HdMsgSet>(set)->mpData.get()), li);
    Local<hdUnitSafeString_c> T;
    stringAt(T.a).mBuffer = txt;
    stringAt(T.a).mVtable = kWSafeVt;
    textUnitNative_assureWideString_1(T.a);
    {
        const u32 p = u32(stringAt(T.a).mBuffer);
        u32 q = p;
        s32 n = 0;
        bool exhausted = false;
        if (load<u16>(q)) {
            for (;;) {
                n++;
                if (n > 0x40000) {
                    exhausted = true;
                    break;
                }
                q += 2;
                if (!load<u16>(q)) break;
            }
        }
        (void)exhausted;
        if (load<u16>(p) == 0x20) {
            vAssure(T.a);
            u32 r = u32(stringAt(T.a).mBuffer);
            s32 k = 0;
            bool ex2 = false;
            if (load<u16>(r)) {
                for (;;) {
                    k++;
                    if (k > 0x40000) {
                        ex2 = true;
                        break;
                    }
                    r += 2;
                    if (!load<u16>(r)) break;
                }
            }
            if (!ex2 && k == 1) return 0;
        }
    }
    const s32 z = s32(textUnitNative_leadingZeros_2(self, T.a));
    vAssure(T.a);
    const s32 len = wcsLength(u32(stringAt(T.a).mBuffer));
    vAssure(T.a);
    const s32 n = len - z;
    return putCharacters(gabi::at<hdUnitBufferedString_c>(w), pos, u32(stringAt(T.a).mBuffer) + u32(z) * 2, n) == n ? n : 0;
}

/* WWHD glue
data fontCharacterMap 100E1114
data lowercaseAccents 100E1294
data uppercaseAccents 100E12C8
data unlimitedWidth 100E12FC
data wideNewline 100E0F8C
data defaultPlayerName 100E1300
str 100E10D8 "[WARNING!]\011Over Style Setting!\012"
str 100E10F8 "[WARNING!]\011TextLabel:%s\012"
str 100E130C "0"
str 100E130E "1"
str 100E1310 "2"
str 100E1312 "3"
str 100E1314 "4"
str 100E1316 "5"
str 100E1318 "6"
str 100E131A "7"
str 100E131C "8"
str 100E131E "9"
*/
