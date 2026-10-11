// HD: Message insertions for names, counters, timers, item units, and ruby labels.
#include "d/hd_text_unit.h"

using namespace hd_text_unit;

static const u16 minuteSeparator[2] = {0x20, 0x0};
static const u16 spanishMinuteSeparator[4] = {0x20, 0x79, 0x20, 0x0};

static const u16 fallbackMessage[8] = {0x6e, 0x6f, 0x20, 0x6e, 0x61, 0x6d, 0x65, 0x0};

/* number of the event counter (event reg 0xBEFF) with its unit */
/* 025FD05C */
s32 hdTextUnit_c::putEventCount(hdUnitBufferedString_c* i_string, s32 pos) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    const s32 n = putAsciiNumberNative(self, w, pos, s32(eventReg(0xBEFF)), 0, 1);
    staticStr(gabi::ea(&textUnitStatics().eventCountUnitGuard), gabi::ea(&textUnitStatics().eventCountUnit), gabi::guest_addr("Unit_Hatu_00"), kSafeVt, staticRecordAddress(RECORD_eventCountUnit));
    return n + putLocalizedUnitNative(self, w, pos + n, gabi::ea(&textUnitStatics().eventCountUnit));
}

/* number + unit (singular for 1) */
/* 025FD124 */
s32 hdTextUnit_c::putRupeeCount(hdUnitBufferedString_c* i_string, s32 pos) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    const s32 v = s32(s16(gabi::at<hdUnitPlayInfo_c>(textUnitNative_getPlayInfo_0())->mCounter));
    const s32 n = putAsciiNumberNative(self, w, pos, v, 0, 1);
    staticPair(gabi::ea(&textUnitStatics().rupeeUnitsGuard), gabi::ea(&textUnitStatics().rupeeUnits), gabi::guest_addr("Unit_Rupee_00"), gabi::guest_addr("Unit_Rupee_01"), staticRecordAddress(RECORD_rupeeUnits));
    return n + putLocalizedUnitNative(self, w, pos + n, gabi::ea(&textUnitStatics().rupeeUnits) + (v == 1 ? 0u : 8u));
}

/* a tag inside an inserted message: handled by the tag processor, else copied as is;
 * returns the text after it (0 when the box is full) */
/* 025FD22C */
u32 hdTextUnit_c::putTag(hdUnitBufferedString_c* i_string, be<u32>* i_position, u32 tag, u32 ctx) {
    be<u32>& position = *i_position;
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    const u32 c = u16(tagAt(tag).mCode);
    u32 next, hdr;
    if (c == hdUnitTag_c::BEGIN_TAG) {
        next = tag + u16(tagAt(tag).mParamSize) + 8;
        hdr = tag;
    } else if (c == hdUnitTag_c::END_TAG) {
        hdr = tag;
        next = tag + 6;
    } else {
        hdr = 0;
        next = tag;
    }
    s32 r = s32(textUnitNative_tagDispatch_6(self, w, u32(position), hdr, tag, ctx));
    if (r < 0) {
        Local<hdUnitSafeString_c> T;
        stringAt(T.a).mBuffer = tag;
        stringAt(T.a).mVtable = kWSafeVt;
        r = s32(next - tag) >> 1;
        wcopyAt(w, s32(u32(position)), T.a, r);
    }
    const u32 p = u32(position) + u32(r);
    position = p;
    if (textUnitNative_boxFull_4(self, w, p - 1, next)) return 0;
    return next;
}

/* puts a message text (characters and tags); returns the number of characters */
/* 025FD46C */
s32 hdTextUnit_c::putText(hdUnitBufferedString_c* i_string, s32 pos, u32 text, u32 ctx) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    Local<be<u32>> P;
    *P.get() = u32(pos);
    if (!text) return 0;
    const s32 start = pos;
    u32 c = load<u16>(text);
    while (c) {
        if (c == hdUnitTag_c::BEGIN_TAG || c == hdUnitTag_c::END_TAG) {
            text = putTag(gabi::at<hdUnitBufferedString_c>(w), P.get(), text, ctx);
            if (!text) break;
            c = load<u16>(text);
        } else {
            const s32 r = s32(textUnitNative_putChar_4(self, w, u32(*P.get()), text));
            if (r < 0) break;
            *P.get() = u32(*P.get()) + u32(r);
            text += 2;
            c = load<u16>(text);
        }
    }
    return s32(u32(*P.get())) - start;
}

/* puts the message with that number (id 0: the fallback text) */
/* 025FD550 */
s32 hdTextUnit_c::putMessage(hdUnitBufferedString_c* i_string, s32 pos, u32 id, u32 ctx) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    s32 r = 0;
    if (!id) {
        staticStr(gabi::ea(&textUnitStatics().fallbackTextGuard), gabi::ea(&textUnitStatics().fallbackText), gabi::ea(fallbackMessage), kWSafeVt, staticRecordAddress(RECORD_fallbackText));
        vAssure(gabi::ea(&textUnitStatics().fallbackText));
        const s32 n = wcsLength(u32(textUnitStatics().fallbackText.mBuffer));
        vAssure(gabi::ea(&textUnitStatics().fallbackText));
        if (s32(textUnitNative_putCharacters_5(self, w, u32(pos), u32(textUnitStatics().fallbackText.mBuffer), u32(n))) == n) r = n;
        return r;
    }
    const u32 Q = gabi::ea(&textUnitStatics().setName);
    Local<HdMsgText> M;
    if (!u32(textUnitStatics().setNameGuard)) {
        /* the static FixedSafeString<1> set name "" */
        const u32 buf = gabi::ea(textUnitStatics().setName.mBuffer);
        textUnitStatics().setName.mBuffer[0] = 0;
        stringAt(Q).mVtable = kBufferedString1Vt;
        stringAt(Q).mBuffer = buf;
        bufferAt(Q).mCapacity = 1;
        Local<hdUnitSafeString_c> T;
        stringAt(T.a).mVtable = kSafeVt;
        textUnitStatics().setNameGuard = 1;
        stringAt(T.a).mBuffer = gabi::guest_addr("");
        textUnitNative_assureString_1(T.a);
        s32 n = cstrLength(u32(stringAt(T.a).mBuffer));
        if (n >= s32(u32(bufferAt(Q).mCapacity))) n = s32(u32(bufferAt(Q).mCapacity)) - 1;
        vAssure(T.a);
        textUnitNative_moveBytes_4(buf, u32(stringAt(T.a).mBuffer), u32(n), 0u);
        store<u8>(buf + n, 0);
        stringAt(Q).mVtable = kFixedString1Vt;
        textUnitNative_registerGlobalObject_1(staticRecordAddress(RECORD_setName));
    }
    textUnitNative_constructMessage_1(M.a);
    Local<hdUnitFormatFrame_c> F;
    const u32 S = gabi::ea(&F.get()->mHeader), Sbuf = gabi::ea(F.get()->mBuffer);
    F.get()->mBuffer[255] = 0;
    F.get()->mBuffer[0] = 0;
    stringAt(S).mBuffer = Sbuf;
    bufferAt(S).mCapacity = 0x100;
    stringAt(S).mVtable = kBufferedString256Vt;
    textUnitNative_formatString_3(S, gabi::guest_addr("%05d"), id & 0xFFFF);
    textUnitNative_loadMessage_4(resourceManagerAddress(), M.a, Q, S);
    const u32 set = gabi::ea(M.get()->mpMessageSet.get());
    if (!set) return 0;
    const u32 li = u32(M.get()->mLabelIndex);
    if (li >= u32(gabi::at<HdMsgSet>(set)->mEntryCount)) return 0;
    const u32 txt = textUnitNative_getLabelText_2(gabi::ea(gabi::at<HdMsgSet>(set)->mpData.get()), li);
    if (!txt) return 0;
    return putText(gabi::at<hdUnitBufferedString_c>(w), pos, txt, ctx);
}

/* messages whose numbers are kept in the play info */
/* 025FD800 */
s32 hdTextUnit_c::putPlayMessage1(hdUnitBufferedString_c* i_string, s32 pos, u32 ctx) {
    const u32 w = gabi::ea(i_string);

    const u32 id = u32(gabi::at<hdUnitPlayInfo_c>(textUnitNative_getPlayInfo_0())->mMessage1);
    return putMessage(gabi::at<hdUnitBufferedString_c>(w), pos, id, ctx);
}
/* 025FD86C */
s32 hdTextUnit_c::putPlayMessage2(hdUnitBufferedString_c* i_string, s32 pos, u32 ctx) {
    const u32 w = gabi::ea(i_string);

    const u32 id = u32(gabi::at<hdUnitPlayInfo_c>(textUnitNative_getPlayInfo_0())->mMessage2);
    return putMessage(gabi::at<hdUnitBufferedString_c>(w), pos, id, ctx);
}

/* sword-game digits (full-width, the highlighted digit marked for the layout) */
/* 025FD8D8 */
s32 hdTextUnit_c::putSwordGameDigits(hdUnitBufferedString_c* i_string, s32 pos) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    staticStr(gabi::ea(&textUnitStatics().swordGameUnitGuard), gabi::ea(&textUnitStatics().swordGameUnit), gabi::guest_addr("Unit_Rupee_02"), kSafeVt, staticRecordAddress(RECORD_swordGameUnit));
    messageAt(msg(self)).mCurrentRuby = INSERTION_DIGITS;
    messageAt(msg(self)).mState = STATE_DIGITS;
    const u32 LABEL = gabi::ea(&textUnitStatics().swordGameUnit);
    Local<hdUnitDigitFrame_c> F; /* +0 WFixedSafeString<4> (buffer +0xC), +0x14 digit chars[4], +0x1C WSafeString */
    const u32 W = gabi::ea(&F.get()->mHeader), Wbuf = gabi::ea(F.get()->mBuffer);
    const u32 T = gabi::ea(&F.get()->mString);
    s32 total = 0;
    const u32 set = textUnitNative_getUnitSet_1(resourceManagerAddress());
    if (!set) return 0;
    vAssure(LABEL);
    const u32 li = textUnitNative_findLabel_2(gabi::ea(gabi::at<HdMsgSet>(set)->mpData.get()), u32(stringAt(LABEL).mBuffer));
    u32 txt = 0;
    if (li < u32(gabi::at<HdMsgSet>(set)->mEntryCount)) txt = textUnitNative_getLabelText_2(gabi::ea(gabi::at<HdMsgSet>(set)->mpData.get()), li);
    stringAt(T).mBuffer = txt;
    stringAt(T).mVtable = kWSafeVt;
    s32 digits = s32(textUnitNative_leadingZeros_2(self, T));
    F.get()->mBuffer[3] = 0;
    stringAt(W).mBuffer = Wbuf;
    bufferAt(W).mCapacity = 4;
    F.get()->mBuffer[0] = 0;
    stringAt(W).mVtable = kWideFixedString4Vt;
    s32 v = s32(s16(gabi::at<hdUnitPlayInfo_c>(textUnitNative_getPlayInfo_0())->mDigitCounter));
    for (s32 k = 0; k < digits; k++) {
        const s32 d = v % 10;
        F.get()->mDigitChars[u32(k) < 4 ? u32(k) : 0] = u16(0xFF10 + d);
        v = v / 10;
    }
    const s32 sel = digits - s32(selectedDigit()) - 1;
    s32 pending = 0;
    s32 idx = digits - 1;
    for (s32 i = 0; digits > 0; digits--, i++, idx--) {
        if (u32(i) == u32(sel)) {
            if (pending > 0) {
                vAssure(W);
                putCharactersNative(self, w, pos, u32(stringAt(W).mBuffer), pending);
                pos += pending;
                total += pending;
                pending = 0;
                store<u16>(u32(stringAt(W).mBuffer), 0);
            }
            u32 m = msg(self);
            messageAt(m).mPreviousLineWidth = f32(messageAt(m).mCurrentLineWidth);
            m = msg(self);
            messageAt(m).mCurrentLine = u32(messageAt(m).mLineCount);
        }
        const u16 ch = u16(F.get()->mDigitChars[u32(idx) < 4 ? u32(idx) : 0]);
        vAssure(W);
        const u32 top = u32(stringAt(W).mBuffer);
        const s32 len = wcsLength(top);
        if (len < s32(u32(bufferAt(W).mCapacity)) - 1) {
            store<u16>(top + u32(len) * 2, ch);
            store<u16>(top + u32(len) * 2 + 2, 0);
        }
        pending++;
        if (u32(i) == u32(sel)) {
            vAssure(W);
            putCharactersNative(self, w, pos, u32(stringAt(W).mBuffer), pending);
            pos += pending;
            total += pending;
            store<u16>(u32(stringAt(W).mBuffer), 0);
            pending = 0;
        }
    }
    if (pending > 0) {
        vAssure(W);
        putCharactersNative(self, w, pos, u32(stringAt(W).mBuffer), pending);
        total += pending;
        pos += pending;
    }
    return total + putLocalizedUnitNative(self, w, pos, LABEL);
}

/* number + unit (singular for 1); the ruby label is not used */
/* 025FDD08 */
s32 hdTextUnit_c::putNumberWithUnit(hdUnitBufferedString_c* i_string, s32 pos, s32 v, u32 ruby, u32 units) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    const s32 n = putAsciiNumberNative(self, w, pos, v, 0, 1);
    return n + putLocalizedUnitNative(self, w, pos + n, units + (v == 1 ? 0u : 8u));
}

/* number + unit with ruby */
/* 025FDD80 */
s32 hdTextUnit_c::putBlows(hdUnitBufferedString_c* i_string, s32 pos) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    staticStr(gabi::ea(&textUnitStatics().blowsRubyGuard), gabi::ea(&textUnitStatics().blowsRuby), gabi::guest_addr("Ruby_SwordGameBlows_00"), kSafeVt, staticRecordAddress(RECORD_blowsRuby));
    staticPair(gabi::ea(&textUnitStatics().blowUnitsGuard), gabi::ea(&textUnitStatics().blowUnits), gabi::guest_addr("Unit_Blow_00"), gabi::guest_addr("Unit_Blow_01"), staticRecordAddress(RECORD_blowUnits));
    return putNumberWithUnitNative(self, w, pos, s32(s16(gabi::at<hdUnitPlayInfo_c>(textUnitNative_getPlayInfo_0())->mCounter)), gabi::ea(&textUnitStatics().blowsRuby), gabi::ea(&textUnitStatics().blowUnits));
}

/* message selected by event reg 0xBA0F (+0x1B37) */
/* 025FDEAC */
s32 hdTextUnit_c::putEventMessage(hdUnitBufferedString_c* i_string, s32 pos, u32 ctx) {
    const u32 w = gabi::ea(i_string);

    return putMessage(gabi::at<hdUnitBufferedString_c>(w), pos, eventReg(0xBA0F) + 0x1B37, ctx);
}

/* 025FDF28 / 025FDF8C / 025FE000: plain numbers */
/* 025FDF28 */
s32 hdTextUnit_c::putPlayCount2(hdUnitBufferedString_c* i_string, s32 pos) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    return putAsciiNumberNative(self, w, pos, s32(s16(gabi::at<hdUnitPlayInfo_c>(textUnitNative_getPlayInfo_0())->mCounter2)), 0, 1);
}
/* 025FDF8C */
s32 hdTextUnit_c::putEvent8AFF(hdUnitBufferedString_c* i_string, s32 pos) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    return putAsciiNumberNative(self, w, pos, s32(eventReg(0x8AFF)), 0, 1);
}
/* 025FE000 */
s32 hdTextUnit_c::putPlayCount(hdUnitBufferedString_c* i_string, s32 pos) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    return putAsciiNumberNative(self, w, pos, s32(s16(gabi::at<hdUnitPlayInfo_c>(textUnitNative_getPlayInfo_0())->mCounter)), 0, 1);
}

/* ten times the play counter with its unit */
/* 025FE064 */
s32 hdTextUnit_c::putPlayCountX10(hdUnitBufferedString_c* i_string, s32 pos) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    staticPair(gabi::ea(&textUnitStatics().countX10UnitsGuard), gabi::ea(&textUnitStatics().countX10Units), gabi::guest_addr("Unit_Rupee_00"), gabi::guest_addr("Unit_Rupee_01"), staticRecordAddress(RECORD_countX10Units));
    return putNumberWithUnitNative(self, w, pos, s32(s16(gabi::at<hdUnitPlayInfo_c>(textUnitNative_getPlayInfo_0())->mCounter)) * 10, 0, gabi::ea(&textUnitStatics().countX10Units));
}

/* number + unit with ruby */
/* 025FE158 */
s32 hdTextUnit_c::putLetters(hdUnitBufferedString_c* i_string, s32 pos) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    staticStr(gabi::ea(&textUnitStatics().lettersRubyGuard), gabi::ea(&textUnitStatics().lettersRuby), gabi::guest_addr("Ruby_Letter_00"), kSafeVt, staticRecordAddress(RECORD_lettersRuby));
    staticPair(gabi::ea(&textUnitStatics().letterUnitsGuard), gabi::ea(&textUnitStatics().letterUnits), gabi::guest_addr("Unit_Letter_00"), gabi::guest_addr("Unit_Letter_01"), staticRecordAddress(RECORD_letterUnits));
    return putNumberWithUnitNative(self, w, pos, s32(s16(gabi::at<hdUnitPlayInfo_c>(textUnitNative_getPlayInfo_0())->mCounter)), gabi::ea(&textUnitStatics().lettersRuby), gabi::ea(&textUnitStatics().letterUnits));
}

/* number + unit (singular for 1) */
/* 025FE284 */
s32 hdTextUnit_c::putKorokCount(hdUnitBufferedString_c* i_string, s32 pos) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    const s32 v = s32(s16(gabi::at<hdUnitPlayInfo_c>(textUnitNative_getPlayInfo_0())->mCounter));
    const s32 n = putAsciiNumberNative(self, w, pos, v, 0, 1);
    staticPair(gabi::ea(&textUnitStatics().korokUnitsGuard), gabi::ea(&textUnitStatics().korokUnits), gabi::guest_addr("Unit_Korg_00"), gabi::guest_addr("Unit_Korg_01"), staticRecordAddress(RECORD_korokUnits));
    return n + putLocalizedUnitNative(self, w, pos + n, gabi::ea(&textUnitStatics().korokUnits) + (v == 1 ? 0u : 8u));
}

/* a time (frames at 30 per second) as minutes and seconds, per language */
/* 025FE38C */
s32 hdTextUnit_c::putTimer(hdUnitBufferedString_c* i_string, s32 pos, s32 frames) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    const s32 minutes = frames / 1800;
    const s32 rem = frames - minutes * 1800;
    s32 secs = s32(u32(rem) * 0x4445) / 0x80000;
    if (!(minutes | secs)) secs = 1;
    s32 total = 0;
    u32 lang = messageLanguage();
    if (lang == 1 || lang == 3) {
        total = putAsciiNumberNative(self, w, pos, minutes, 0, 1);
        pos += total;
    } else if (minutes > 0) {
        const s32 n = putAsciiNumberNative(self, w, pos, minutes, 0, 1);
        pos += n;
        total = n;
    }
    staticPair(gabi::ea(&textUnitStatics().minuteUnitsGuard), gabi::ea(&textUnitStatics().minuteUnits), gabi::guest_addr("Unit_Minute_00"), gabi::guest_addr("Unit_Minute_01"), staticRecordAddress(RECORD_minuteUnits));
    lang = messageLanguage();
    if (lang == 1 || lang == 3) {
        const s32 r = putLocalizedUnitNative(self, w, pos, gabi::ea(&textUnitStatics().minuteUnits));
        pos += r;
        total += r;
        lang = messageLanguage();
    } else if (minutes > 0) {
        const s32 r = putLocalizedUnitNative(self, w, pos, gabi::ea(&textUnitStatics().minuteUnits) + (minutes > 1 ? 8u : 0u));
        pos += r;
        lang = messageLanguage();
        total += r;
    }
    if (lang == 1 || lang == 3) {
        const s32 n = putAsciiNumberNative(self, w, pos, secs, 2, 0);
        total += n;
        pos += n;
    } else if (!minutes) {
        const s32 n = putAsciiNumberNative(self, w, pos, secs, 2, 0);
        total += n;
        pos += n;
    } else if (secs) {
        if (!u32(textUnitStatics().separatorsGuard)) {
            const u32 lits[2] = {gabi::ea(minuteSeparator), gabi::ea(spanishMinuteSeparator)};
            staticArr(gabi::ea(&textUnitStatics().separatorsGuard), gabi::ea(&textUnitStatics().separators), lits, 2, kWSafeVt, staticRecordAddress(RECORD_separators));
            lang = messageLanguage();
        }
        const u32 sep = gabi::ea(&textUnitStatics().separators) + (lang == 5 ? 8u : 0u);
        vAssure(sep);
        const s32 len = wcsLength(u32(stringAt(sep).mBuffer));
        vAssure(sep);
        if (putCharactersNative(self, w, pos, u32(stringAt(sep).mBuffer), len) != len) return total;
        total += len;
        pos += len;
        const s32 n = putAsciiNumberNative(self, w, pos, secs, 2, 0);
        total += n;
        pos += n;
    }
    staticPair(gabi::ea(&textUnitStatics().secondUnitsGuard), gabi::ea(&textUnitStatics().secondUnits), gabi::guest_addr("Unit_Second_00"), gabi::guest_addr("Unit_Second_01"), staticRecordAddress(RECORD_secondUnits));
    lang = messageLanguage();
    if (lang == 1) return total;
    if (lang == 3) return total + putLocalizedUnitNative(self, w, pos, gabi::ea(&textUnitStatics().secondUnits));
    if (secs > 0) total += putLocalizedUnitNative(self, w, pos, gabi::ea(&textUnitStatics().secondUnits) + (secs > 1 ? 8u : 0u));
    return total;
}

/* timers */
/* 025FE7F4 */
s32 hdTextUnit_c::putSaveTimer(hdUnitBufferedString_c* i_string, s32 pos) {
    const u32 w = gabi::ea(i_string);

    return putTimer(gabi::at<hdUnitBufferedString_c>(w), pos, s32(textUnitNative_getTimer_1(gabi::ea(&gabi::at<hdUnitSaveInfo_c>(saveInfo())->mTimer))));
}
/* 025FECF4 */
s32 hdTextUnit_c::putPlayTimer(hdUnitBufferedString_c* i_string, s32 pos) {
    const u32 w = gabi::ea(i_string);

    return putTimer(gabi::at<hdUnitBufferedString_c>(w), pos, s32(s16(gabi::at<hdUnitPlayInfo_c>(textUnitNative_getPlayInfo_0())->mTimer)));
}

/* number + unit (singular for 1) */
/* 025FE858 */
s32 hdTextUnit_c::putMeters(hdUnitBufferedString_c* i_string, s32 pos) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    const s32 v = s32(s16(gabi::at<hdUnitPlayInfo_c>(textUnitNative_getPlayInfo_0())->mCounter));
    const s32 n = putAsciiNumberNative(self, w, pos, v, 0, 1);
    staticPair(gabi::ea(&textUnitStatics().meterUnitsGuard), gabi::ea(&textUnitStatics().meterUnits), gabi::guest_addr("Unit_Meter_00"), gabi::guest_addr("Unit_Meter_01"), staticRecordAddress(RECORD_meterUnits));
    return n + putLocalizedUnitNative(self, w, pos + n, gabi::ea(&textUnitStatics().meterUnits) + (v == 1 ? 0u : 8u));
}

/* number + unit (singular for 1) */
/* 025FE960 */
s32 hdTextUnit_c::putEvent86FF(hdUnitBufferedString_c* i_string, s32 pos) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    const s32 v = s32(eventReg(0x86FF));
    const s32 n = putAsciiNumberNative(self, w, pos, v, 0, 1);
    staticPair(gabi::ea(&textUnitStatics().event86FFUnitsGuard), gabi::ea(&textUnitStatics().event86FFUnits), gabi::guest_addr("Unit_Point_00"), gabi::guest_addr("Unit_Point_01"), staticRecordAddress(RECORD_event86FFUnits));
    return n + putLocalizedUnitNative(self, w, pos + n, gabi::ea(&textUnitStatics().event86FFUnits) + (v == 1 ? 0u : 8u));
}

/* number + unit with ruby */
/* 025FEA7C */
s32 hdTextUnit_c::putSavedPendants(hdUnitBufferedString_c* i_string, s32 pos) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    staticStr(gabi::ea(&textUnitStatics().savedPendantsRubyGuard), gabi::ea(&textUnitStatics().savedPendantsRuby), gabi::guest_addr("Ruby_Pendant_00"), kSafeVt, staticRecordAddress(RECORD_savedPendantsRuby));
    staticPair(gabi::ea(&textUnitStatics().savedPendantsUnitsGuard), gabi::ea(&textUnitStatics().savedPendantsUnits), gabi::guest_addr("Unit_Pendant_00"), gabi::guest_addr("Unit_Pendant_01"), staticRecordAddress(RECORD_savedPendantsUnits));
    return putNumberWithUnitNative(self, w, pos, s32(u8(gabi::at<hdUnitSaveInfo_c>(saveInfo())->mPendantCount)), gabi::ea(&textUnitStatics().savedPendantsRuby), gabi::ea(&textUnitStatics().savedPendantsUnits));
}

/* number + unit with ruby */
/* 025FEBA8 */
s32 hdTextUnit_c::putPendantsEvent(hdUnitBufferedString_c* i_string, s32 pos) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    staticStr(gabi::ea(&textUnitStatics().pendantsEventRubyGuard), gabi::ea(&textUnitStatics().pendantsEventRuby), gabi::guest_addr("Ruby_Pendant_00"), kSafeVt, staticRecordAddress(RECORD_pendantsEventRuby));
    staticPair(gabi::ea(&textUnitStatics().pendantsEventUnitsGuard), gabi::ea(&textUnitStatics().pendantsEventUnits), gabi::guest_addr("Unit_Pendant_00"), gabi::guest_addr("Unit_Pendant_01"), staticRecordAddress(RECORD_pendantsEventUnits));
    return putNumberWithUnitNative(self, w, pos, s32(eventReg(0xC0FF)), gabi::ea(&textUnitStatics().pendantsEventRuby), gabi::ea(&textUnitStatics().pendantsEventUnits));
}

/* number + unit with ruby */
/* 025FED50 */
s32 hdTextUnit_c::putSavedBombs(hdUnitBufferedString_c* i_string, s32 pos) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    staticStr(gabi::ea(&textUnitStatics().savedBombsRubyGuard), gabi::ea(&textUnitStatics().savedBombsRuby), gabi::guest_addr("Ruby_Bomb_00"), kSafeVt, staticRecordAddress(RECORD_savedBombsRuby));
    staticPair(gabi::ea(&textUnitStatics().savedBombsUnitsGuard), gabi::ea(&textUnitStatics().savedBombsUnits), gabi::guest_addr("Unit_Bomb_00"), gabi::guest_addr("Unit_Bomb_01"), staticRecordAddress(RECORD_savedBombsUnits));
    return putNumberWithUnitNative(self, w, pos, s32(u8(gabi::at<hdUnitSaveInfo_c>(saveInfo())->mBombCount)), gabi::ea(&textUnitStatics().savedBombsRuby), gabi::ea(&textUnitStatics().savedBombsUnits));
}

/* number + unit (singular for 1) */
/* 025FEE7C */
s32 hdTextUnit_c::putSavedArrows(hdUnitBufferedString_c* i_string, s32 pos) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    const s32 v = s32(u8(gabi::at<hdUnitSaveInfo_c>(saveInfo())->mArrowCount));
    const s32 n = putAsciiNumberNative(self, w, pos, v, 0, 1);
    staticPair(gabi::ea(&textUnitStatics().savedArrowUnitsGuard), gabi::ea(&textUnitStatics().savedArrowUnits), gabi::guest_addr("Unit_Arrow_00"), gabi::guest_addr("Unit_Arrow_01"), staticRecordAddress(RECORD_savedArrowUnits));
    return n + putLocalizedUnitNative(self, w, pos + n, gabi::ea(&textUnitStatics().savedArrowUnits) + (v == 1 ? 0u : 8u));
}

/* number + unit with ruby */
/* 025FEF7C */
s32 hdTextUnit_c::putSeeds(hdUnitBufferedString_c* i_string, s32 pos) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    staticStr(gabi::ea(&textUnitStatics().seedsRubyGuard), gabi::ea(&textUnitStatics().seedsRuby), gabi::guest_addr("Ruby_Seed_00"), kSafeVt, staticRecordAddress(RECORD_seedsRuby));
    staticPair(gabi::ea(&textUnitStatics().seedsUnitsGuard), gabi::ea(&textUnitStatics().seedsUnits), gabi::guest_addr("Unit_Seed_00"), gabi::guest_addr("Unit_Seed_01"), staticRecordAddress(RECORD_seedsUnits));
    return putNumberWithUnitNative(self, w, pos, s32(u8(itemCounters().mItemCount)), gabi::ea(&textUnitStatics().seedsRuby), gabi::ea(&textUnitStatics().seedsUnits));
}

/* number + unit with ruby */
/* 025FF0A0 */
s32 hdTextUnit_c::putNecklaces(hdUnitBufferedString_c* i_string, s32 pos) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    staticStr(gabi::ea(&textUnitStatics().necklacesRubyGuard), gabi::ea(&textUnitStatics().necklacesRuby), gabi::guest_addr("Ruby_Necklace_00"), kSafeVt, staticRecordAddress(RECORD_necklacesRuby));
    staticPair(gabi::ea(&textUnitStatics().necklacesUnitsGuard), gabi::ea(&textUnitStatics().necklacesUnits), gabi::guest_addr("Unit_Necklace_00"), gabi::guest_addr("Unit_Necklace_01"), staticRecordAddress(RECORD_necklacesUnits));
    return putNumberWithUnitNative(self, w, pos, s32(u8(itemCounters().mItemCount)), gabi::ea(&textUnitStatics().necklacesRuby), gabi::ea(&textUnitStatics().necklacesUnits));
}

/* number + unit with ruby */
/* 025FF1C4 */
s32 hdTextUnit_c::putChuchus(hdUnitBufferedString_c* i_string, s32 pos) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    staticStr(gabi::ea(&textUnitStatics().chuchusRubyGuard), gabi::ea(&textUnitStatics().chuchusRuby), gabi::guest_addr("Ruby_Chuchu_00"), kSafeVt, staticRecordAddress(RECORD_chuchusRuby));
    staticPair(gabi::ea(&textUnitStatics().chuchusUnitsGuard), gabi::ea(&textUnitStatics().chuchusUnits), gabi::guest_addr("Unit_Chuchu_00"), gabi::guest_addr("Unit_Chuchu_01"), staticRecordAddress(RECORD_chuchusUnits));
    return putNumberWithUnitNative(self, w, pos, s32(u8(itemCounters().mItemCount)), gabi::ea(&textUnitStatics().chuchusRuby), gabi::ea(&textUnitStatics().chuchusUnits));
}

/* number + unit with ruby */
/* 025FF2E8 */
s32 hdTextUnit_c::putPendants(hdUnitBufferedString_c* i_string, s32 pos) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    staticStr(gabi::ea(&textUnitStatics().pendantsRubyGuard), gabi::ea(&textUnitStatics().pendantsRuby), gabi::guest_addr("Ruby_Pendant_00"), kSafeVt, staticRecordAddress(RECORD_pendantsRuby));
    staticPair(gabi::ea(&textUnitStatics().pendantsUnitsGuard), gabi::ea(&textUnitStatics().pendantsUnits), gabi::guest_addr("Unit_Pendant_00"), gabi::guest_addr("Unit_Pendant_01"), staticRecordAddress(RECORD_pendantsUnits));
    return putNumberWithUnitNative(self, w, pos, s32(u8(itemCounters().mItemCount)), gabi::ea(&textUnitStatics().pendantsRuby), gabi::ea(&textUnitStatics().pendantsUnits));
}

/* number + unit with ruby */
/* 025FF40C */
s32 hdTextUnit_c::putFeathers(hdUnitBufferedString_c* i_string, s32 pos) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    staticStr(gabi::ea(&textUnitStatics().feathersRubyGuard), gabi::ea(&textUnitStatics().feathersRuby), gabi::guest_addr("Ruby_Feather_00"), kSafeVt, staticRecordAddress(RECORD_feathersRuby));
    staticPair(gabi::ea(&textUnitStatics().feathersUnitsGuard), gabi::ea(&textUnitStatics().feathersUnits), gabi::guest_addr("Unit_Feather_00"), gabi::guest_addr("Unit_Feather_01"), staticRecordAddress(RECORD_feathersUnits));
    return putNumberWithUnitNative(self, w, pos, s32(u8(itemCounters().mItemCount)), gabi::ea(&textUnitStatics().feathersRuby), gabi::ea(&textUnitStatics().feathersUnits));
}

/* number + unit with ruby */
/* 025FF530 */
s32 hdTextUnit_c::putCrests(hdUnitBufferedString_c* i_string, s32 pos) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    staticStr(gabi::ea(&textUnitStatics().crestsRubyGuard), gabi::ea(&textUnitStatics().crestsRuby), gabi::guest_addr("Ruby_Crest_00"), kSafeVt, staticRecordAddress(RECORD_crestsRuby));
    staticPair(gabi::ea(&textUnitStatics().crestsUnitsGuard), gabi::ea(&textUnitStatics().crestsUnits), gabi::guest_addr("Unit_Crest_00"), gabi::guest_addr("Unit_Crest_01"), staticRecordAddress(RECORD_crestsUnits));
    return putNumberWithUnitNative(self, w, pos, s32(u8(itemCounters().mItemCount)), gabi::ea(&textUnitStatics().crestsRuby), gabi::ea(&textUnitStatics().crestsUnits));
}

/* Stored rupees with the localized unit, without ruby. */
/* 025FF654 */
s32 hdTextUnit_c::putStoredRupees(hdUnitBufferedString_c* i_string, s32 pos) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    staticPair(gabi::ea(&textUnitStatics().storedRupeeUnitsGuard), gabi::ea(&textUnitStatics().storedRupeeUnits), gabi::guest_addr("Unit_Rupee_00"), gabi::guest_addr("Unit_Rupee_01"), staticRecordAddress(RECORD_storedRupeeUnits));
    return putNumberWithUnitNative(self, w, pos, s32(s16(itemCounters().mStoredRupees)), 0, gabi::ea(&textUnitStatics().storedRupeeUnits));
}

/* Digits of the play counter with the selected localized unit label. */
/* 025FF738 */
s32 hdTextUnit_c::putDigitsLabel(hdUnitBufferedString_c* i_string, s32 pos, u32 ruby, u32 unit, u32 ctx) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    messageAt(msg(self)).mCurrentRuby = INSERTION_DIGITS;
    messageAt(msg(self)).mState = STATE_DIGITS;
    const u32 LABEL = unit;
    Local<hdUnitDigitFrame_c> F; /* +0 WFixedSafeString<4> (buffer +0xC), +0x14 digit chars[4], +0x1C WSafeString */
    const u32 W = gabi::ea(&F.get()->mHeader), Wbuf = gabi::ea(F.get()->mBuffer);
    const u32 T = gabi::ea(&F.get()->mString);
    s32 total = 0;
    const u32 set = textUnitNative_getUnitSet_1(resourceManagerAddress());
    if (!set) return 0;
    vAssure(LABEL);
    const u32 li = textUnitNative_findLabel_2(gabi::ea(gabi::at<HdMsgSet>(set)->mpData.get()), u32(stringAt(LABEL).mBuffer));
    u32 txt = 0;
    if (li < u32(gabi::at<HdMsgSet>(set)->mEntryCount)) txt = textUnitNative_getLabelText_2(gabi::ea(gabi::at<HdMsgSet>(set)->mpData.get()), li);
    stringAt(T).mBuffer = txt;
    stringAt(T).mVtable = kWSafeVt;
    s32 digits = s32(textUnitNative_leadingZeros_2(self, T));
    F.get()->mBuffer[3] = 0;
    stringAt(W).mBuffer = Wbuf;
    bufferAt(W).mCapacity = 4;
    F.get()->mBuffer[0] = 0;
    stringAt(W).mVtable = kWideFixedString4Vt;
    s32 v = s32(s16(gabi::at<hdUnitPlayInfo_c>(textUnitNative_getPlayInfo_0())->mDigitCounter));
    for (s32 k = 0; k < digits; k++) {
        const s32 d = v % 10;
        F.get()->mDigitChars[u32(k) < 4 ? u32(k) : 0] = u16(0xFF10 + d);
        v = v / 10;
    }
    const s32 sel = digits - s32(selectedDigit()) - 1;
    s32 pending = 0;
    s32 idx = digits - 1;
    for (s32 i = 0; digits > 0; digits--, i++, idx--) {
        if (u32(i) == u32(sel)) {
            if (pending > 0) {
                vAssure(W);
                putCharactersNative(self, w, pos, u32(stringAt(W).mBuffer), pending);
                pos += pending;
                total += pending;
                pending = 0;
                store<u16>(u32(stringAt(W).mBuffer), 0);
            }
            u32 m = msg(self);
            messageAt(m).mPreviousLineWidth = f32(messageAt(m).mCurrentLineWidth);
            m = msg(self);
            messageAt(m).mCurrentLine = u32(messageAt(m).mLineCount);
        }
        const u16 ch = u16(F.get()->mDigitChars[u32(idx) < 4 ? u32(idx) : 0]);
        vAssure(W);
        const u32 top = u32(stringAt(W).mBuffer);
        const s32 len = wcsLength(top);
        if (len < s32(u32(bufferAt(W).mCapacity)) - 1) {
            store<u16>(top + u32(len) * 2, ch);
            store<u16>(top + u32(len) * 2 + 2, 0);
        }
        pending++;
        if (u32(i) == u32(sel)) {
            vAssure(W);
            putCharactersNative(self, w, pos, u32(stringAt(W).mBuffer), pending);
            pos += pending;
            total += pending;
            store<u16>(u32(stringAt(W).mBuffer), 0);
            pending = 0;
        }
    }
    if (pending > 0) {
        vAssure(W);
        putCharactersNative(self, w, pos, u32(stringAt(W).mBuffer), pending);
        total += pending;
        pos += pending;
    }
    return total + putLocalizedUnitNative(self, w, pos, LABEL);
}

/* digit display with ruby and unit labels */
/* 025FFB24 */
s32 hdTextUnit_c::putSeedDigits(hdUnitBufferedString_c* i_string, s32 pos, u32 ctx) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    staticStr(gabi::ea(&textUnitStatics().seedDigitsRubyGuard), gabi::ea(&textUnitStatics().seedDigitsRuby), gabi::guest_addr("Ruby_Seed_00"), kSafeVt, staticRecordAddress(RECORD_seedDigitsRuby));
    staticStr(gabi::ea(&textUnitStatics().seedDigitsUnitGuard), gabi::ea(&textUnitStatics().seedDigitsUnit), gabi::guest_addr("Unit_Seed_02"), kSafeVt, staticRecordAddress(RECORD_seedDigitsUnit));
    return s32(textUnitNative_putDigitsLabel_6(self, w, u32(pos), gabi::ea(&textUnitStatics().seedDigitsRuby), gabi::ea(&textUnitStatics().seedDigitsUnit), ctx));
}

/* digit display with ruby and unit labels */
/* 025FFBF8 */
s32 hdTextUnit_c::putNecklaceDigits(hdUnitBufferedString_c* i_string, s32 pos, u32 ctx) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    staticStr(gabi::ea(&textUnitStatics().necklaceDigitsRubyGuard), gabi::ea(&textUnitStatics().necklaceDigitsRuby), gabi::guest_addr("Ruby_Necklace_00"), kSafeVt, staticRecordAddress(RECORD_necklaceDigitsRuby));
    staticStr(gabi::ea(&textUnitStatics().necklaceDigitsUnitGuard), gabi::ea(&textUnitStatics().necklaceDigitsUnit), gabi::guest_addr("Unit_Necklace_02"), kSafeVt, staticRecordAddress(RECORD_necklaceDigitsUnit));
    return s32(textUnitNative_putDigitsLabel_6(self, w, u32(pos), gabi::ea(&textUnitStatics().necklaceDigitsRuby), gabi::ea(&textUnitStatics().necklaceDigitsUnit), ctx));
}

/* digit display with ruby and unit labels */
/* 025FFCCC */
s32 hdTextUnit_c::putChuchuDigits(hdUnitBufferedString_c* i_string, s32 pos, u32 ctx) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    staticStr(gabi::ea(&textUnitStatics().chuchuDigitsRubyGuard), gabi::ea(&textUnitStatics().chuchuDigitsRuby), gabi::guest_addr("Ruby_Chuchu_00"), kSafeVt, staticRecordAddress(RECORD_chuchuDigitsRuby));
    staticStr(gabi::ea(&textUnitStatics().chuchuDigitsUnitGuard), gabi::ea(&textUnitStatics().chuchuDigitsUnit), gabi::guest_addr("Unit_Chuchu_02"), kSafeVt, staticRecordAddress(RECORD_chuchuDigitsUnit));
    return s32(textUnitNative_putDigitsLabel_6(self, w, u32(pos), gabi::ea(&textUnitStatics().chuchuDigitsRuby), gabi::ea(&textUnitStatics().chuchuDigitsUnit), ctx));
}

/* digit display with ruby and unit labels */
/* 025FFDA0 */
s32 hdTextUnit_c::putPendantDigits(hdUnitBufferedString_c* i_string, s32 pos, u32 ctx) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    staticStr(gabi::ea(&textUnitStatics().pendantDigitsRubyGuard), gabi::ea(&textUnitStatics().pendantDigitsRuby), gabi::guest_addr("Ruby_Pendant_00"), kSafeVt, staticRecordAddress(RECORD_pendantDigitsRuby));
    staticStr(gabi::ea(&textUnitStatics().pendantDigitsUnitGuard), gabi::ea(&textUnitStatics().pendantDigitsUnit), gabi::guest_addr("Unit_Pendant_02"), kSafeVt, staticRecordAddress(RECORD_pendantDigitsUnit));
    return s32(textUnitNative_putDigitsLabel_6(self, w, u32(pos), gabi::ea(&textUnitStatics().pendantDigitsRuby), gabi::ea(&textUnitStatics().pendantDigitsUnit), ctx));
}

/* digit display with ruby and unit labels */
/* 025FFE74 */
s32 hdTextUnit_c::putFeatherDigits(hdUnitBufferedString_c* i_string, s32 pos, u32 ctx) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    staticStr(gabi::ea(&textUnitStatics().featherDigitsRubyGuard), gabi::ea(&textUnitStatics().featherDigitsRuby), gabi::guest_addr("Ruby_Feather_00"), kSafeVt, staticRecordAddress(RECORD_featherDigitsRuby));
    staticStr(gabi::ea(&textUnitStatics().featherDigitsUnitGuard), gabi::ea(&textUnitStatics().featherDigitsUnit), gabi::guest_addr("Unit_Feather_02"), kSafeVt, staticRecordAddress(RECORD_featherDigitsUnit));
    return s32(textUnitNative_putDigitsLabel_6(self, w, u32(pos), gabi::ea(&textUnitStatics().featherDigitsRuby), gabi::ea(&textUnitStatics().featherDigitsUnit), ctx));
}

/* digit display with ruby and unit labels */
/* 025FFF48 */
s32 hdTextUnit_c::putCrestDigits(hdUnitBufferedString_c* i_string, s32 pos, u32 ctx) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    staticStr(gabi::ea(&textUnitStatics().crestDigitsRubyGuard), gabi::ea(&textUnitStatics().crestDigitsRuby), gabi::guest_addr("Ruby_Crest_00"), kSafeVt, staticRecordAddress(RECORD_crestDigitsRuby));
    staticStr(gabi::ea(&textUnitStatics().crestDigitsUnitGuard), gabi::ea(&textUnitStatics().crestDigitsUnit), gabi::guest_addr("Unit_Crest_02"), kSafeVt, staticRecordAddress(RECORD_crestDigitsUnit));
    return s32(textUnitNative_putDigitsLabel_6(self, w, u32(pos), gabi::ea(&textUnitStatics().crestDigitsRuby), gabi::ea(&textUnitStatics().crestDigitsUnit), ctx));
}

/* WWHD glue
data minuteSeparator 100E142C
data spanishMinuteSeparator 100E1424
data fallbackMessage 100E135C
str 100E1320 "Unit_Hatu_00"
str 100E1330 "Unit_Rupee_00" in 025FD124
str 100E1340 "Unit_Rupee_01" in 025FD124
str 100E1350 ""
str 100E1354 "%05d"
str 100E136C "Unit_Rupee_02"
str 100E137C "Ruby_SwordGameBlows_00"
str 100E1394 "Unit_Blow_00"
str 100E13A4 "Unit_Blow_01"
str 100E13B4 "Unit_Rupee_00" in 025FE064
str 100E13C4 "Unit_Rupee_01" in 025FE064
str 100E13D4 "Ruby_Letter_00"
str 100E13E4 "Unit_Letter_00"
str 100E13F4 "Unit_Letter_01"
str 100E1404 "Unit_Korg_00"
str 100E1414 "Unit_Korg_01"
str 100E1430 "Unit_Minute_00"
str 100E1440 "Unit_Minute_01"
str 100E1450 "Unit_Second_00"
str 100E1460 "Unit_Second_01"
str 100E1470 "Unit_Meter_00"
str 100E1480 "Unit_Meter_01"
str 100E1490 "Unit_Point_00"
str 100E14A0 "Unit_Point_01"
str 100E14B0 "Ruby_Pendant_00" in 025FEA7C
str 100E14C0 "Unit_Pendant_00" in 025FEA7C
str 100E14D0 "Unit_Pendant_01" in 025FEA7C
str 100E14E0 "Ruby_Pendant_00" in 025FEBA8
str 100E14F0 "Unit_Pendant_00" in 025FEBA8
str 100E1500 "Unit_Pendant_01" in 025FEBA8
str 100E1510 "Ruby_Bomb_00"
str 100E1520 "Unit_Bomb_00"
str 100E1530 "Unit_Bomb_01"
str 100E1540 "Unit_Arrow_00"
str 100E1550 "Unit_Arrow_01"
str 100E1560 "Ruby_Seed_00" in 025FEF7C
str 100E1570 "Unit_Seed_00"
str 100E1580 "Unit_Seed_01"
str 100E1590 "Ruby_Necklace_00" in 025FF0A0
str 100E15A4 "Unit_Necklace_00"
str 100E15B8 "Unit_Necklace_01"
str 100E15CC "Ruby_Chuchu_00" in 025FF1C4
str 100E15DC "Unit_Chuchu_00"
str 100E15EC "Unit_Chuchu_01"
str 100E15FC "Ruby_Pendant_00" in 025FF2E8
str 100E160C "Unit_Pendant_00" in 025FF2E8
str 100E161C "Unit_Pendant_01" in 025FF2E8
str 100E162C "Ruby_Feather_00" in 025FF40C
str 100E163C "Unit_Feather_00"
str 100E164C "Unit_Feather_01"
str 100E165C "Ruby_Crest_00" in 025FF530
str 100E166C "Unit_Crest_00"
str 100E167C "Unit_Crest_01"
str 100E168C "Unit_Rupee_00" in 025FF654
str 100E169C "Unit_Rupee_01" in 025FF654
str 100E16AC "Ruby_Seed_00" in 025FFB24
str 100E16BC "Unit_Seed_02"
str 100E16CC "Ruby_Necklace_00" in 025FFBF8
str 100E16E0 "Unit_Necklace_02"
str 100E16F4 "Ruby_Chuchu_00" in 025FFCCC
str 100E1704 "Unit_Chuchu_02"
str 100E1714 "Ruby_Pendant_00" in 025FFDA0
str 100E1724 "Unit_Pendant_02"
str 100E1734 "Ruby_Feather_00" in 025FFE74
str 100E1744 "Unit_Feather_02"
str 100E1754 "Ruby_Crest_00" in 025FFF48
str 100E1764 "Unit_Crest_02"
*/
