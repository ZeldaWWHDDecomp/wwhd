// HD: Message tag dispatch, ruby collection, text layout loop, and static initialization.
#include "d/hd_text_unit.h"

using namespace hd_text_unit;

static const f32 rubyUnlimitedWidth[1] = {0.0f};
static const f32 initialRangeLow[1] = {-3.1415927410125732f};
static const f32 initialRangeHigh[1] = {3.1415927410125732f};
static const u16 rubyWideNewline[2] = {0xa, 0x0};

static const char* const fullWidthDigitLiterals[10] = {"\202O", "\202P", "\202Q", "\202R", "\202S",
                                 "\202T", "\202U", "\202V", "\202W", "\202X"};
static const hdUnitTagHandler group2Handlers[0x28] = {
    textUnitNative_putPlayerName_6, textUnitNative_putEventCount_6, textUnitNative_putRupeeCount_6, textUnitNative_putPlayMessage1_6,
    textUnitNative_putPlayMessage2_6, textUnitNative_putRupeeCount_6, textUnitNative_putRupeeCount_6, textUnitNative_putSwordGameDigits_6,
    textUnitNative_putBlows_6, textUnitNative_putEventMessage_6, textUnitNative_putPlayCount2_6, textUnitNative_putRupeeCount_6,
    textUnitNative_putLetters_6, textUnitNative_putKorokCount_6, textUnitNative_putSaveTimer_6, textUnitNative_putMeters_6,
    textUnitNative_putMeters_6, textUnitNative_putEvent86FF_6, textUnitNative_putSavedPendants_6, textUnitNative_putPendantsEvent_6,
    textUnitNative_putPlayTimer_6, textUnitNative_putRupeeCount_6, textUnitNative_putSavedBombs_6, textUnitNative_putSavedArrows_6,
    textUnitNative_putEvent8AFF_6, textUnitNative_putPlayCount_6, textUnitNative_putPlayCountX10_6, textUnitNative_putSeeds_6,
    textUnitNative_putNecklaces_6, textUnitNative_putChuchus_6, textUnitNative_putPendants_6, textUnitNative_putFeathers_6,
    textUnitNative_putCrests_6, textUnitNative_putStoredRupees_6, textUnitNative_putSeedDigits_6, textUnitNative_putNecklaceDigits_6,
    textUnitNative_putChuchuDigits_6, textUnitNative_putPendantDigits_6, textUnitNative_putFeatherDigits_6, textUnitNative_putCrestDigits_6
};
static const u32 messagePalette[27] = {0x000000FF, 0xB40000FF, 0x008282FF, 0x0000AAFF, 0xF0F01EFF, 0x82FFFFFF, 0x6400FFFF, 0x505050FF, 0xFFB400FF, 0x00000000, 0xB4000000, 0x00828200, 0x0000AA00, 0xF0F01E00, 0x82FFFF00, 0x6400FF00, 0x50505000, 0xFFB40000, 0x000000FF, 0xB40000FF, 0x008282FF, 0x0000AAFF, 0xE67D0FFF, 0x82FFFFFF, 0x6400FFFF, 0x3C3C3CFF, 0xFFB400FF};

/* puts a number with full-width (Shift-JIS) digits */
/* 0260001C */
s32 hdTextUnit_c::putNumberFull(hdUnitBufferedString_c* i_string, s32 pos, s32 v, u32 maxDigits, u32 noPad) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);

    staticArr(gabi::ea(&textUnitStatics().fullWidthDigitsGuard), gabi::ea(&textUnitStatics().fullWidthDigits), fullWidthDigitLiterals, 10, kSafeVt, staticRecordAddress(RECORD_fullWidthDigits));
    return s32(textUnitNative_putNumber_7(self, w, u32(pos), u32(v), gabi::ea(&textUnitStatics().fullWidthDigits), maxDigits, noPad));
}

/* number tag: one of the five message values (+0x8F8), ASCII or full-width digits */
/* 0260026C */
s32 hdTextUnit_c::putValueTag(hdUnitBufferedString_c* i_string, s32 pos, u32 tag) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    const u32 k = u16(tagAt(tag).mTag) - 0x28;
    const u32 ascii = u8(gabi::at<hdUnitNumberTag_c>(tag)->mAscii);
    const u32 maxD = u8(gabi::at<hdUnitNumberTag_c>(tag)->mMaxDigits);
    const u32 m = msg(self);
    const u32 noPad = u8(gabi::at<hdUnitNumberTag_c>(tag)->mNoPadding);
    const s32 v = s32(u32(messageAt(m).mLineFlags[k < 5 ? k : 0]));
    if (ascii) return putAsciiNumberNative(self, w, pos, v, maxD, noPad);
    return putNumberFull(gabi::at<hdUnitBufferedString_c>(w), pos, v, maxD, noPad);
}

/* tag group 2: inserted values (names, counters, timers, messages, item units) */
/* 026002E4 */
s32 hdTextUnit_c::group2(hdUnitBufferedString_c* i_string, s32 pos, u32 tag, u32 a7, u32 a8) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);

    const u32 t = u16(tagAt(tag).mTag);
    if (t > 0x2C) return 0;
    if (t >= 0x28) return s32(textUnitNative_putValueTag_6(self, w, u32(pos), tag, a7, a8));
    const u32 r6 = (t == 3 || t == 4 || t == 9) ? a8 : a7;
    return s32(group2Handlers[t](self, w, u32(pos), r6, a7, a8));
}

/* sound tag (at the message's sound position, with the room's reverb) */
/* 0260046C */
s32 hdTextUnit_c::tagSound(u32 tag) {
    const u32 self = gabi::ea(this);
    if (!textUnitNative_canStop_1(self)) return 0;
    u32 m = msg(self);
    messageAt(m).mCurrentTag = u32(messageAt(m).mLayoutMode);
    m = msg(self);
    Local<hdUnitSoundPosition_c> P;
    const s32 room = dComIfGp_roomControl_getStayNo();
    P.get()->mX = gabi::at<hdUnitSoundPosition_c>(gabi::ea(&messageAt(m).mSoundPosition))->mX;
    P.get()->mY = gabi::at<hdUnitSoundPosition_c>(gabi::ea(&messageAt(m).mSoundPosition))->mY;
    const u32 se = u16(tagAt(tag).mTag);
    P.get()->mZ = gabi::at<hdUnitSoundPosition_c>(gabi::ea(&messageAt(m).mSoundPosition))->mZ;
    if (room) {
        const u32 rev = textUnitNative_getRoomReverb_1(u32(room));
        textUnitNative_playMessageSound_3(se, se == 8 ? 0u : P.a, rev);
    } else {
        textUnitNative_playMessageSound_3(se, se == 8 ? 0u : P.a, 0u);
    }
    return 0;
}

/* play-info tags (a value / a camera mode byte) */
/* 02600548 */
s32 hdTextUnit_c::tagPlayValue(u32 tag) {
    const u32 self = gabi::ea(this);
    if (textUnitNative_canStop_1(self)) {
        const u32 m = msg(self);
        messageAt(m).mCurrentTag = u32(messageAt(m).mLayoutMode);
        const u32 v = u16(tagAt(tag).mTag);
        gabi::at<hdUnitPlayInfo_c>(textUnitNative_getPlayInfo_0())->mTextValue = v;
    }
    return 0;
}
/* 02600598 */
s32 hdTextUnit_c::tagPlayByte(u32 tag) {
    const u32 self = gabi::ea(this);
    if (textUnitNative_canStop_1(self)) {
        const u32 m = msg(self);
        messageAt(m).mCurrentTag = u32(messageAt(m).mLayoutMode);
        u32 v = u8(gabi::at<hdUnitTagByte_c>(tag)->mTagLow);
        if (v >= 7) v = (v + 4) & 0xFF;
        gabi::at<hdUnitPlayInfo_c>(textUnitNative_getPlayInfo_0())->mCameraMode = u8(v);
    }
    return 0;
}

/* tag dispatch by group */
/* 026005F8 */
s32 hdTextUnit_c::tagDispatch(hdUnitBufferedString_c* i_string, s32 pos, u32 tag, u32 a7, u32 a8) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    const u32 g = u16(tagAt(tag).mGroup);
    if (g < 4) {
        if (g < 1) return s32(textUnitNative_group0_6(self, w, u32(pos), tag, a7, a8));
        if (g == 1) return s32(textUnitNative_group1_6(self, w, u32(pos), tag, a7, a8));
        if (g == 2) return group2(gabi::at<hdUnitBufferedString_c>(w), pos, tag, a7, a8);
        return -1;
    }
    if (g < 5) return tagSound(tag);
    if (g == 5) return tagPlayValue(tag);
    if (g == 6) return tagPlayByte(tag);
    return -1;
}

/* pictograph tag: advances the line by the glyph (line break when it does not fit) */
/* 02600660 */
s32 hdTextUnit_c::pictTag(hdUnitBufferedString_c* i_string, be<u32>* i_position, u32 hdr, u32 tag) {
    be<u32>& position = *i_position;
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    if (u16(tagAt(hdr).mGroup) != 3) return 0;
    s32 r = 0;
    Local<be<u16>> C;
    *C.get() = u16(textUnitNative_convertPictograph_2(gabi::ea(&gabi::at<hdFontMgr_c>(fontManagerAddress())->mTextConv), u32(u8(gabi::at<hdUnitTagByte_c>(hdr)->mTagLow))));
    textUnitNative_advancePictograph_2(msg(self), C.a);
    const u32 m = msg(self);
    const f32 lim = f32(messageAt(m).mLayoutWidth);
    if (!(lim == f32(rubyUnlimitedWidth[0])) && f32(messageAt(m).mCurrentLineWidth) > lim && !u8(messageAt(m).mFlag69A)) {
        r = s32(textUnitNative_newLine_4(self, w, u32(position), tag + 2));
        if (r < 0) return -1;
        position = u32(position) + u32(r);
        textUnitNative_advancePictograph_2(msg(self), C.a);
    }
    return r;
}

/* collects the ruby base text (tags, characters, at most 1 or 2 line breaks) into +0x6C0 */
/* 02600760 */
void hdTextUnit_c::collectRubyBase(u32 text) {
    const u32 self = gabi::ea(this);
    const u32 m = msg(self);
    const u32 dst = gabi::ea(&messageAt(m).mRubyText);
    s32 breaks = u32(messageAt(m).mState) == STATE_TWO_CHOICES ? 1 : 2;
    store<u16>(gabi::ea(messageAt(m).mRubyText.mString.mpBuffer.get()), 0);
    messageAt(msg(self)).mFontState = 0;
    s32 n = 0;
    Local<hdUnitSafeString_c> T8, T18, T20, T10;
    u32 c = load<u16>(text);
    while (c) {
        if (c == hdUnitTag_c::BEGIN_TAG || c == hdUnitTag_c::END_TAG) {
            u32 next;
            if (c == hdUnitTag_c::BEGIN_TAG) next = text + u16(tagAt(text).mParamSize) + 8;
            else next = text + 6;
            const s32 k = s32(next - text) >> 1;
            stringAt(T8.a).mBuffer = text;
            stringAt(T8.a).mVtable = kWSafeVt;
            wcopyAt(dst, n, T8.a, k);
            text = next;
            c = load<u16>(text);
            n += k;
        } else if (c == hdUnitTag_c::NEW_LINE) {
            if (breaks) {
                stringAt(T18.a).mBuffer = text;
                stringAt(T18.a).mVtable = kWSafeVt;
                wcopyAt(dst, n, T18.a, 1);
                breaks--;
                n++;
            }
            text += 2;
            c = load<u16>(text);
        } else {
            stringAt(T20.a).mBuffer = text;
            stringAt(T20.a).mVtable = kWSafeVt;
            wcopyAt(dst, n, T20.a, 1);
            text += 2;
            c = load<u16>(text);
            n++;
        }
    }
    wtmp(T10.a, gabi::ea(&rubyWideNewline[1]));
    wcopyAt(dst, n, T10.a, 1);
    messageAt(msg(self)).mFontState = u32(n + 1);
}

/* copies ruby text into dst up to the ruby end tag (group 1, tag 9/10), spaces skipped;
 * returns the text there */
/* 02600C48 */
u32 hdTextUnit_c::copyRubyText(u32 dst, u32 text) {
    const u32 self = gabi::ea(this);
    store<u16>(u32(stringAt(dst).mBuffer), 0);
    messageAt(msg(self)).mFontState = 0;
    s32 n = 0;
    Local<hdUnitSafeString_c> T8, T18, T10;
    u32 c = load<u16>(text);
    while (c) {
        if (c == hdUnitTag_c::BEGIN_TAG || c == hdUnitTag_c::END_TAG) {
            u32 hdr, next;
            if (c == hdUnitTag_c::BEGIN_TAG) {
                hdr = text;
                next = text + u16(tagAt(text).mParamSize) + 8;
            } else {
                hdr = text;
                next = text + 6;
            }
            if (u16(tagAt(hdr).mGroup) == 1) {
                const u32 t = u16(tagAt(hdr).mTag);
                if (t == 9 || t == 0xA) break;
            }
            const s32 k = s32(next - text) >> 1;
            stringAt(T8.a).mBuffer = text;
            stringAt(T8.a).mVtable = kWSafeVt;
            wcopyAt(dst, n, T8.a, k);
            text = next;
            c = load<u16>(text);
            n += k;
        } else if (c == 0x20) {
            text += 2;
            c = load<u16>(text);
        } else {
            stringAt(T18.a).mVtable = kWSafeVt;
            stringAt(T18.a).mBuffer = text;
            wcopyAt(dst, n, T18.a, 1);
            n++;
            text += 2;
            c = load<u16>(text);
        }
    }
    wtmp(T10.a, gabi::ea(&rubyWideNewline[1]));
    wcopyAt(dst, n, T10.a, 1);
    messageAt(msg(self)).mFontState = u32(n + 1);
    return text;
}

/* processes one tag (pictograph advance, dispatch, raw copy, ruby collection, box check);
 * returns the text after it (0 to stop) */
/* 02601060 */
u32 hdTextUnit_c::processTag(hdUnitBufferedString_c* i_string, be<u32>* i_position, u32 tag, u32 ctx) {
    be<u32>& position = *i_position;
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    const u32 c = u16(tagAt(tag).mCode);
    u32 hdr, next;
    if (c == hdUnitTag_c::BEGIN_TAG) {
        hdr = tag;
        next = tag + u16(tagAt(tag).mParamSize) + 8;
    } else if (c == hdUnitTag_c::END_TAG) {
        hdr = tag;
        next = tag + 6;
    } else {
        hdr = 0;
        next = tag;
    }
    const s32 r = pictTag(gabi::at<hdUnitBufferedString_c>(w), i_position, hdr, tag);
    if (r < 0) return 0;
    const u32 pos = u32(position) + u32(r);
    position = pos;
    s32 put = s32(textUnitNative_tagDispatch_6(self, w, pos, hdr, tag, ctx));
    const s32 k = s32(next - tag) >> 1;
    if (put < 0) {
        Local<hdUnitSafeString_c> T;
        stringAt(T.a).mBuffer = tag;
        stringAt(T.a).mVtable = kWSafeVt;
        put = k;
        wcopyAt(w, s32(u32(position)), T.a, k);
    }
    position = u32(position) + u32(put);
    u32 m = msg(self);
    messageAt(m).mLayoutMode = u32(messageAt(m).mLayoutMode) + u32(k);
    m = msg(self);
    if (u32(messageAt(m).mCurrentRuby) == INSERTION_CHOICE) {
        collectRubyBase(next);
        return 0;
    }
    if (u32(messageAt(m).mCurrentRuby) == INSERTION_RUBY) {
        if (!u32(messageAt(m).mFontState)) return copyRubyText(gabi::ea(&messageAt(m).mRubyText), next);
        copyRubyText(gabi::ea(&messageAt(m).mRubyReading), next);
        return 0;
    }
    if (textUnitNative_boxFull_4(self, w, u32(position) - 1, next)) return 0;
    return next;
}

/* inserts the "line start" tag (group 1 tag 0xD) */
/* 0260137C */
s32 hdTextUnit_c::insertLineTag(hdUnitBufferedString_c* i_string, s32 pos) {
    const u32 w = gabi::ea(i_string);

    Local<hdUnitWideFixed32_c> L;
    wfixed(L.a, kWFixed32Vt, 0x20);
    const u32 b = u32(stringAt(L.a).mBuffer);
    tagAt(b).mCode = hdUnitTag_c::BEGIN_TAG;
    tagAt(b).mGroup = 1;
    tagAt(b).mTag = 0xD;
    tagAt(b).mParamSize = 0;
    wcopyAt(w, pos, L.a, 4);
    return 4;
}

/* inserts it once when the first visible character is reached */
/* 026014C8 */
void hdTextUnit_c::markLineStart(hdUnitBufferedString_c* i_string, be<u32>* i_position) {
    be<u32>& position = *i_position;
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    const u32 m = msg(self);
    if (u8(messageAt(m).mLayoutComplete) || u8(messageAt(m).mFlag696) || u8(messageAt(m).mFlag699)) return;
    if (s32(u32(messageAt(m).mLayoutProgress)) < s32(u32(messageAt(m).mLayoutLimit)) && !u32(messageAt(m).mSoundState)) return;
    const s32 n = insertLineTag(gabi::at<hdUnitBufferedString_c>(w), s32(u32(position)));
    position = u32(position) + u32(n);
    messageAt(msg(self)).mLayoutComplete = 1;
}

/* the text loop: tags, line breaks and characters; then the last line and the cut */
/* 02601564 */
u32 hdTextUnit_c::processText(hdUnitBufferedString_c* i_string, be<u32>* i_position, u32 text, u32 ctx) {
    be<u32>& position = *i_position;
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    u32 c = load<u16>(text);
    while (c) {
        if (c == hdUnitTag_c::BEGIN_TAG || c == hdUnitTag_c::END_TAG) {
            messageAt(msg(self)).mLineReady = 1;
            const u32 r = processTag(gabi::at<hdUnitBufferedString_c>(w), i_position, text, ctx);
            if (!r) break;
            text = r;
            markLineStart(gabi::at<hdUnitBufferedString_c>(w), i_position);
        } else if (c == hdUnitTag_c::NEW_LINE) {
            const s32 r = s32(textUnitNative_newLine_4(self, w, u32(position), text));
            const u32 m = msg(self);
            messageAt(m).mLayoutMode = u32(messageAt(m).mLayoutMode) + 1;
            if (r < 0) return text;
            text += 2;
            position = u32(position) + u32(r);
            markLineStart(gabi::at<hdUnitBufferedString_c>(w), i_position);
        } else {
            messageAt(msg(self)).mLineReady = 1;
            const s32 r = s32(textUnitNative_putChar_4(self, w, u32(position), text));
            if (r < 0) return text;
            position = u32(position) + u32(r);
            const u32 m = msg(self);
            text += 2;
            messageAt(m).mLayoutMode = u32(messageAt(m).mLayoutMode) + 1;
            markLineStart(gabi::at<hdUnitBufferedString_c>(w), i_position);
        }
        c = load<u16>(text);
    }
    messageAt(msg(self)).mWaitComplete = 1;
    const u32 m = msg(self);
    const u32 n = u32(messageAt(m).mLineCount);
    if (!n) {
        textUnitNative_recordLineEnd_2(m, u32(position));
    } else {
        const u32 last = u32(messageAt(m).mLineValues[n - 1 < 5 ? n - 1 : 0]);
        if (u32(position) != last + 1) textUnitNative_recordLineEnd_2(m, u32(position));
    }
    textUnitNative_cutLines_2(self, w);
    return text;
}

/* after the text: end sound / state unless waiting */
/* 02601710 */
void hdTextUnit_c::finishText(u32 text) {
    const u32 self = gabi::ea(this);
    const u32 m = msg(self);
    if (u32(messageAt(m).mCurrentRuby) || u8(messageAt(m).mLayoutComplete)) return;
    if (u8(messageAt(m).mFlag699)) {
        messageAt(m).mFlag699 = 0;
        return;
    }
    if (u8(messageAt(m).mWaitComplete)) {
        textUnitNative_playEndSe_1(self);
        textUnitNative_setEndState_1(self);
        return;
    }
    if (u8(messageAt(m).mFlag69C) || u8(messageAt(m).mFlag696)) return;
    u32 mm = m;
    if (!u8(messageAt(m).mFlag697) && !u8(messageAt(m).mFlag698)) {
        textUnitNative_playEndSe_1(self);
        mm = msg(self);
    }
    messageAt(mm).mState = STATE_BUILDING;
}

/* builds the text box string of a message from its current position; returns the length */
/* 026017F0 */
s32 hdTextUnit_c::build(hdUnitBufferedString_c* i_string, u32 text, u32 m, u32 ctx) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    Local<be<u32>> P;
    mpMessage = gabi::at<HdMsgText>(m);
    const u32 e = gabi::ea(messageAt(m).mpEntry.get());
    *P.get() = 0;
    const u32 start = u32(messageAt(m).mLayoutMode);
    if (e && u8(gabi::at<hdUnitEntry_c>(e)->mKind) == 1) messageAt(m).mFlag696 = 1;
    text += start << 1;
    if (!textUnitNative_onTextEnd_2(self, text)) {
        const u32 r = processText(gabi::at<hdUnitBufferedString_c>(w), P.get(), text, ctx);
        finishText(r);
    }
    return s32(u32(*P.get()));
}

/* static initialiser: header statics and the colour constants */
/* 026018B0 */
void staticInit() {
    textUnitStatics().zeroWords[3] = 0;
    textUnitStatics().zeroWords[2] = 0;
    textUnitStatics().zeroWords[1] = 0;
    textUnitStatics().zeroWords[0] = 0;
    textUnitNative_registerGlobalObject_1(staticRecordAddress(RECORD_headerFloatPair));
    const f32 lo = f32(initialRangeLow[0]), hi = f32(initialRangeHigh[0]);
    textUnitStatics().rangeLow = lo;
    textUnitStatics().rangeHigh = hi;
    textUnitNative_constructFlag_1(gabi::ea(&textUnitStatics().flag));
    textUnitNative_registerGlobalObject_1(staticRecordAddress(RECORD_headerFlag));
    textUnitNative_constructByte_1(gabi::ea(&textUnitStatics().byte));
    textUnitNative_registerGlobalObject_1(staticRecordAddress(RECORD_headerByte));

    for (u32 i = 0; i < 27; i++) textUnitStatics().palette[i] = messagePalette[i];
    textUnitStatics().whiteColor.mRed = 0xFF;
    textUnitStatics().whiteColor.mGreen = 0xFF;
    textUnitStatics().whiteColor.mBlue = 0xFF;
    textUnitStatics().whiteColor.mAlpha = 0xFF;
    textUnitStatics().redColor.mRed = 0xFF;
    textUnitStatics().redColor.mGreen = 0;
    textUnitStatics().redColor.mBlue = 0;
    textUnitStatics().redColor.mAlpha = 0xFF;
}

/* WWHD glue
data rubyUnlimitedWidth 100E12FC
data initialRangeLow 100E1794
data initialRangeHigh 100E1798
data rubyWideNewline 100E0F8C
str 100E1774 "\202O"
str 100E1777 "\202P"
str 100E177A "\202Q"
str 100E177D "\202R"
str 100E1780 "\202S"
str 100E1783 "\202T"
str 100E1786 "\202U"
str 100E1789 "\202V"
str 100E178C "\202W"
str 100E178F "\202X"
*/
