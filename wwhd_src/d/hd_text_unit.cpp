// HD: Message builder, guarded string cleanup, state/sound tags, and layout-pane style.
#include "d/hd_text_unit.h"

using namespace hd_text_unit;

static const f32 percentScale[1] = {100.0f};

static const hdUnitTagHandler group1Handlers[13] = {
    textUnitNative_tagNoEndSe_6, textUnitNative_tagEndSe_6, textUnitNative_tagWait_6, textUnitNative_tagWaitAuto_6,
    textUnitNative_tagWaitInput_6, nullptr, textUnitNative_tagSpeed_6, textUnitNative_tagChoice2_6,
    textUnitNative_tagChoice3_6, textUnitNative_tagRuby_6, textUnitNative_tagRuby2_6, textUnitNative_tagCapital_6,
    textUnitNative_copyPaneStyle_6
};

/* 025FA5C0 */
void destroyRupeeUnits() {
    textUnitNative_destroyArray_6(gabi::ea(&textUnitStatics().rupeeUnits), 2u, 8u, SAFE_STRING_DESTRUCTOR, 0u, 0u);
}
/* 025FA5E4 */
void destroyBlowUnits() {
    textUnitNative_destroyArray_6(gabi::ea(&textUnitStatics().blowUnits), 2u, 8u, SAFE_STRING_DESTRUCTOR, 0u, 0u);
}
/* 025FA608 */
void destroyCountX10Units() {
    textUnitNative_destroyArray_6(gabi::ea(&textUnitStatics().countX10Units), 2u, 8u, SAFE_STRING_DESTRUCTOR, 0u, 0u);
}
/* 025FA62C */
void destroyLetterUnits() {
    textUnitNative_destroyArray_6(gabi::ea(&textUnitStatics().letterUnits), 2u, 8u, SAFE_STRING_DESTRUCTOR, 0u, 0u);
}
/* 025FA650 */
void destroyKorokUnits() {
    textUnitNative_destroyArray_6(gabi::ea(&textUnitStatics().korokUnits), 2u, 8u, SAFE_STRING_DESTRUCTOR, 0u, 0u);
}
/* 025FA674 */
void destroyMeterUnits() {
    textUnitNative_destroyArray_6(gabi::ea(&textUnitStatics().meterUnits), 2u, 8u, SAFE_STRING_DESTRUCTOR, 0u, 0u);
}
/* 025FA698 */
void destroyEvent86FFUnits() {
    textUnitNative_destroyArray_6(gabi::ea(&textUnitStatics().event86FFUnits), 2u, 8u, SAFE_STRING_DESTRUCTOR, 0u, 0u);
}
/* 025FA6BC */
void destroySavedPendantsUnits() {
    textUnitNative_destroyArray_6(gabi::ea(&textUnitStatics().savedPendantsUnits), 2u, 8u, SAFE_STRING_DESTRUCTOR, 0u, 0u);
}
/* 025FA6E0 */
void destroyPendantsEventUnits() {
    textUnitNative_destroyArray_6(gabi::ea(&textUnitStatics().pendantsEventUnits), 2u, 8u, SAFE_STRING_DESTRUCTOR, 0u, 0u);
}
/* 025FA704 */
void destroySavedBombsUnits() {
    textUnitNative_destroyArray_6(gabi::ea(&textUnitStatics().savedBombsUnits), 2u, 8u, SAFE_STRING_DESTRUCTOR, 0u, 0u);
}
/* 025FA728 */
void destroySavedArrowUnits() {
    textUnitNative_destroyArray_6(gabi::ea(&textUnitStatics().savedArrowUnits), 2u, 8u, SAFE_STRING_DESTRUCTOR, 0u, 0u);
}
/* 025FA74C */
void destroySeedsUnits() {
    textUnitNative_destroyArray_6(gabi::ea(&textUnitStatics().seedsUnits), 2u, 8u, SAFE_STRING_DESTRUCTOR, 0u, 0u);
}
/* 025FA770 */
void destroyNecklacesUnits() {
    textUnitNative_destroyArray_6(gabi::ea(&textUnitStatics().necklacesUnits), 2u, 8u, SAFE_STRING_DESTRUCTOR, 0u, 0u);
}
/* 025FA794 */
void destroyChuchusUnits() {
    textUnitNative_destroyArray_6(gabi::ea(&textUnitStatics().chuchusUnits), 2u, 8u, SAFE_STRING_DESTRUCTOR, 0u, 0u);
}
/* 025FA7B8 */
void destroyPendantsUnits() {
    textUnitNative_destroyArray_6(gabi::ea(&textUnitStatics().pendantsUnits), 2u, 8u, SAFE_STRING_DESTRUCTOR, 0u, 0u);
}
/* 025FA7DC */
void destroyFeathersUnits() {
    textUnitNative_destroyArray_6(gabi::ea(&textUnitStatics().feathersUnits), 2u, 8u, SAFE_STRING_DESTRUCTOR, 0u, 0u);
}
/* 025FA800 */
void destroyCrestsUnits() {
    textUnitNative_destroyArray_6(gabi::ea(&textUnitStatics().crestsUnits), 2u, 8u, SAFE_STRING_DESTRUCTOR, 0u, 0u);
}
/* 025FA824 */
void destroyStoredRupeeUnits() {
    textUnitNative_destroyArray_6(gabi::ea(&textUnitStatics().storedRupeeUnits), 2u, 8u, SAFE_STRING_DESTRUCTOR, 0u, 0u);
}
/* 025FA848 */
void destroyFullWidthDigits() {
    textUnitNative_destroyArray_6(gabi::ea(&textUnitStatics().fullWidthDigits), 10u, 8u, SAFE_STRING_DESTRUCTOR, 0u, 0u);
}
/* 025FA86C */
void destroyAsciiDigits() {
    textUnitNative_destroyArray_6(gabi::ea(&textUnitStatics().asciiDigits), 10u, 8u, SAFE_STRING_DESTRUCTOR, 0u, 0u);
}
/* 025FA890 */
void destroyMinuteUnits() {
    textUnitNative_destroyArray_6(gabi::ea(&textUnitStatics().minuteUnits), 2u, 8u, SAFE_STRING_DESTRUCTOR, 0u, 0u);
}
/* 025FA8B4 */
void destroySeparators() {
    textUnitNative_destroyArray_6(gabi::ea(&textUnitStatics().separators), 2u, 8u, WIDE_STRING_DESTRUCTOR, 0u, 0u);
}
/* 025FA8D8 */
void destroySecondUnits() {
    textUnitNative_destroyArray_6(gabi::ea(&textUnitStatics().secondUnits), 2u, 8u, SAFE_STRING_DESTRUCTOR, 0u, 0u);
}

/* builder constructor */
/* 025FA8FC */
u32 hdTextUnit_create(u32 p) {
    if (!p) {
        p = textUnitNative_allocate_1(4u);
        if (!p) return 0;
    }
    gabi::at<hdTextUnit_c>(p)->mpMessage = nullptr;
    return p;
}

/* the end-of-message sound (none for some entry kinds; with the room's reverb) */
/* 025FA938 */
void hdTextUnit_c::playEndSe() {
    const u32 self = gabi::ea(this);
    const u32 e = gabi::ea(messageAt(msg(self)).mpEntry.get());
    if (!e) return;
    const u32 k = u8(gabi::at<HdMsgEntry>(e)->mTag);
    if (k == 2 || k == 5 || k == 6 || k == 7 || k == 9 || k == 0xB || k == 0xD) return;
    const s32 room = dComIfGp_roomControl_getStayNo();
    if (room) {
        const u32 rev = textUnitNative_getRoomReverb_1(u32(room));
        textUnitNative_playMessageSound_3(0u, 0u, rev);
    } else {
        textUnitNative_playMessageSound_3(0u, 0u, 0u);
    }
}

/* next message state (0xA when waiting for input, else 0xE) */
/* 025FA9E8 */
void hdTextUnit_c::setEndState() {
    const u32 self = gabi::ea(this);
    const u32 m = msg(self);
    messageAt(m).mState = (u8(messageAt(m).mFlag697) || u8(messageAt(m).mFlag698)) ? STATE_END_WAIT : STATE_END;
}

/* at the text end: ends the message (sound, state) unless a wait/continue is pending */
/* 025FAA1C */
u32 hdTextUnit_c::onTextEnd(u32 ch) {
    const u32 self = gabi::ea(this);
    if (load<u16>(ch)) return 0;
    const u32 m = msg(self);
    if (u32(messageAt(m).mCurrentRuby)) return 1;
    if (u8(messageAt(m).mFlag699)) {
        messageAt(m).mFlag699 = 0;
        return 1;
    }
    if (u8(messageAt(m).mLayoutComplete)) return 1;
    if (!u8(messageAt(m).mFlag696)) playEndSe();
    setEndState();
    return 1;
}

/* inserts a font tag (id, current scale in percent) */
/* 025FAAB8 */
s32 hdTextUnit_c::insertFontTag(hdUnitBufferedString_c* i_string, s32 pos, u32 id) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    Local<hdUnitWideFixed32_c> T;
    wfixed(T.a, kWFixed32Vt, 0x20);
    const u32 b = u32(stringAt(T.a).mBuffer);
    tagAt(b).mTag = 1;
    tagAt(b).mParamSize = 6;
    tagAt(b).mCode = hdUnitTag_c::BEGIN_TAG;
    tagAt(b).mParameters[0] = u16(id);
    tagAt(b).mGroup = 0;
    const f32 k = f32(percentScale[0]);
    tagAt(b).mParameters[1] = u16(ftoi(fmuls_ppc(f32(messageAt(msg(self)).mScaleX), k)));
    tagAt(b).mParameters[2] = u16(ftoi(fmuls_ppc(f32(messageAt(msg(self)).mScaleY), k)));
    wcopyAt(w, pos, T.a, 7);
    return 7;
}

/* font tag: switches the layout font (0xFFFF: back to the saved one) and inserts it */
/* 025FAC54 */
s32 hdTextUnit_c::fontTag(hdUnitBufferedString_c* i_string, s32 pos, u32 tag) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    const u32 m = msg(self);
    const u32 id = u16(tagAt(tag).mParameters[0]);
    const u32 saved = u32(messageAt(m).mDrawIndex);
    const u32 cur = gabi::ea(messageAt(m).mpFont.get());
    if (id == 0xFFFF) {
        messageAt(m).mDrawIndex = 0;
        messageAt(msg(self)).mpFont = gabi::at<HdMsgFont>(saved);
        return insertFontTag(gabi::at<hdUnitBufferedString_c>(w), pos, id);
    }
    const u32 f = textUnitNative_getFont_2(fontManagerAddress(), id);
    if (!saved) messageAt(msg(self)).mDrawIndex = cur;
    messageAt(msg(self)).mpFont = gabi::at<HdMsgFont>(f);
    return insertFontTag(gabi::at<hdUnitBufferedString_c>(w), pos, id);
}

/* line scale tag (percent) */
/* 025FAD04 */
s32 hdTextUnit_c::scaleTag(u32 tag) {
    const u32 self = gabi::ea(this);
    const f32 s = f32(f64(u16(tagAt(tag).mParameters[0]))) / f32(percentScale[0]);
    messageAt(msg(self)).mFontScale = s;
    return -1;
}

/* inserts a colour tag (RGBA bytes as two u16) */
/* 025FAD4C */
s32 hdTextUnit_c::insertColorTag(hdUnitBufferedString_c* i_string, s32 pos, u32 rgba) {
    const u32 w = gabi::ea(i_string);

    Local<hdUnitWideFixed32_c> T;
    wfixed(T.a, kWFixed32Vt, 0x20);
    const u32 b = u32(stringAt(T.a).mBuffer);
    tagAt(b).mParamSize = 4;
    tagAt(b).mCode = hdUnitTag_c::BEGIN_TAG;
    tagAt(b).mTag = 3;
    tagAt(b).mGroup = 0;
    tagAt(b).mParameters[0] = u16(u8(gabi::at<hdUnitColor_c>(rgba)->mRed) | (u8(gabi::at<hdUnitColor_c>(rgba)->mGreen) << 8));
    tagAt(b).mParameters[1] = u16(u8(gabi::at<hdUnitColor_c>(rgba)->mBlue) | (u8(gabi::at<hdUnitColor_c>(rgba)->mAlpha) << 8));
    wcopyAt(w, pos, T.a, 6);
    return 6;
}

static u32 load4(u32 a) {
    return (u32(u8(gabi::at<hdUnitColor_c>(a)->mRed)) << 24) | (u32(u8(gabi::at<hdUnitColor_c>(a)->mGreen)) << 16) | (u32(u8(gabi::at<hdUnitColor_c>(a)->mBlue)) << 8) | u8(gabi::at<hdUnitColor_c>(a)->mAlpha);
}
static void store4(u32 a, u32 v) {
    gabi::at<hdUnitColor_c>(a)->mRed = u8(v >> 24);
    gabi::at<hdUnitColor_c>(a)->mGreen = u8(v >> 16);
    gabi::at<hdUnitColor_c>(a)->mBlue = u8(v >> 8);
    gabi::at<hdUnitColor_c>(a)->mAlpha = u8(v);
}

/* colour tag: palette colour 1..8 of the project (row by mode), else the default
 * colour; it becomes the current colour (+0x91D) */
/* 025FAEC4 */
s32 hdTextUnit_c::colorTag(hdUnitBufferedString_c* i_string, s32 pos, u32 tag, u32 mode) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    const u32 prj = textUnitNative_getProject_1(resourceManagerAddress());
    const u32 id = u16(tagAt(tag).mParameters[0]);
    Local<be<u32>> C;
    u32 c;
    if (id - 1 < 8) {
        u32 idx;
        if (mode < 1) idx = (id + 9) & 0xFFFF;
        else if (mode == 1) idx = (id + 0x12) & 0xFFFF;
        else if (mode == 2) idx = (id + 0x1B) & 0xFFFF;
        else idx = id;
        u32 p = u32(gabi::at<hdUnitPaletteProject_c>(prj)->mColors);
        if (idx < u32(gabi::at<hdUnitPaletteProject_c>(prj)->mColorCount)) p += idx << 2;
        c = load<u32>(p);
    } else {
        const u32 m = msg(self);
        c = load4(u8(messageAt(m).mFlag921) ? gabi::ea(&messageAt(m).mPackedValue2) : gabi::ea(&messageAt(m).mPackedValue1));
    }
    store4(gabi::ea(&messageAt(msg(self)).mPackedValue3), c);
    *C.get() = c;
    return insertColorTag(gabi::at<hdUnitBufferedString_c>(w), pos, C.a);
}

/* colour tag with the palette row of the message kind */
/* 025FB08C */
s32 hdTextUnit_c::colorTagAuto(hdUnitBufferedString_c* i_string, s32 pos, u32 tag) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    const u32 m = msg(self);
    if (u32(messageAt(m).mMessageNumber) - 0x42 <= 9) return colorTag(gabi::at<hdUnitBufferedString_c>(w), pos, tag, 0);
    const u32 e = gabi::ea(messageAt(m).mpEntry.get());
    if (e) {
        const u32 k = u8(gabi::at<HdMsgEntry>(e)->mTag);
        if (k == 2 || k == 6 || k == 7 || k == 0xB) return colorTag(gabi::at<hdUnitBufferedString_c>(w), pos, tag, 1);
    }
    return colorTag(gabi::at<hdUnitBufferedString_c>(w), pos, tag, 3);
}

/* tag group 0 (1 font, 2 scale, 3 colour) */
/* 025FB10C */
s32 hdTextUnit_c::group0(hdUnitBufferedString_c* i_string, s32 pos, u32 tag) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    const u32 t = u16(tagAt(tag).mTag);
    if (t < 1) return -1;
    if (t == 1) return fontTag(gabi::at<hdUnitBufferedString_c>(w), pos, tag);
    if (t == 2) return s32(textUnitNative_scaleTag_4(self, tag, u32(pos), tag));
    if (t == 3) return colorTagAuto(gabi::at<hdUnitBufferedString_c>(w), pos, tag);
    return -1;
}

/* 025FB148 */
s32 hdTextUnit_c::tagNoEndSe() {
    const u32 self = gabi::ea(this);
    messageAt(msg(self)).mFlag696 = 1;
    return 0;
}

/* 025FB15C */
s32 hdTextUnit_c::tagEndSe() {
    const u32 self = gabi::ea(this);
    messageAt(msg(self)).mFlag696 = 0;
    const u32 m = msg(self);
    if (s32(u32(messageAt(m).mLayoutLimit)) < s32(u32(messageAt(m).mLayoutProgress))) messageAt(m).mLayoutLimit = u32(messageAt(m).mLayoutProgress);
    return 0;
}

/* may a wait tag stop here (not yet stopped at this character)? */
/* 025FB18C */
u32 hdTextUnit_c::canStop() {
    const u32 self = gabi::ea(this);
    const u32 m = msg(self);
    if (u8(messageAt(m).mLayoutComplete)) return 0;
    return s32(u32(messageAt(m).mCurrentTag)) < s32(u32(messageAt(m).mLayoutMode)) ? 1u : 0u;
}

void hdTextUnit_c::stopHere(u32 tag, u32 flag) {
    const u32 self = gabi::ea(this);
    u32 m = msg(self);
    messageAt(m).mCurrentTag = u32(messageAt(m).mLayoutMode);
    messageAt(msg(self)).mSoundIndex = u16(tagAt(tag).mParameters[0]);
    if (flag == STOP_WAIT_INPUT) messageAt(msg(self)).mFlag698 = 1;
    else messageAt(msg(self)).mFlag697 = 1;
}

/* wait tag (frames) */
/* 025FB1BC */
s32 hdTextUnit_c::tagWait(hdUnitBufferedString_c* i_string, s32 pos, u32 tag) {

    if (canStop()) stopHere(tag, STOP_WAIT_FRAMES);
    return 0;
}

/* wait tag for kinds that also set the auto-advance flag (+0x69B) */
/* 025FB210 */
s32 hdTextUnit_c::tagWaitAuto(hdUnitBufferedString_c* i_string, s32 pos, u32 tag) {

    const u32 self = gabi::ea(this);
    const u32 m = msg(self);
    const u32 e = gabi::ea(messageAt(m).mpEntry.get());
    if (!e) return 0;
    const u32 k = u8(gabi::at<HdMsgEntry>(e)->mTag);
    if (k < 8) {
        if (k <= 1 || k == 5) messageAt(m).mFlag69B = 1;
    } else if (k == 8 || k == 0xC || k == 0xE) {
        messageAt(m).mFlag69B = 1;
    }
    if (!canStop()) return 0;
    stopHere(tag, STOP_WAIT_FRAMES);
    return 0;
}

/* wait-for-input tag */
/* 025FB2C8 */
s32 hdTextUnit_c::tagWaitInput(hdUnitBufferedString_c* i_string, s32 pos, u32 tag) {

    if (canStop()) stopHere(tag, STOP_WAIT_INPUT);
    return 0;
}

/* speed tag (not in instant mode 0x14F) */
/* 025FB31C */
s32 hdTextUnit_c::tagSpeed(hdUnitBufferedString_c* i_string, s32 pos, u32 tag) {

    const u32 self = gabi::ea(this);
    if (canStop()) {
        u32 m = msg(self);
        messageAt(m).mCurrentTag = u32(messageAt(m).mLayoutMode);
        m = msg(self);
        if (u32(messageAt(m).mLayoutLimit) != 0x14F) messageAt(m).mSoundState = u16(tagAt(tag).mParameters[0]);
    }
    return 0;
}

s32 hdTextUnit_c::choiceTag(u32 mode, u32 state) {
    const u32 self = gabi::ea(this);
    messageAt(msg(self)).mCurrentRuby = mode;
    messageAt(msg(self)).mState = state;
    return 0;
}
/* 025FB370 */
s32 hdTextUnit_c::tagChoice2() {

    return choiceTag(INSERTION_CHOICE, STATE_TWO_CHOICES);
}
/* 025FB390 */
s32 hdTextUnit_c::tagChoice3() {

    return choiceTag(INSERTION_CHOICE, STATE_THREE_CHOICES);
}
/* 025FB3B0 */
s32 hdTextUnit_c::tagRuby() {

    return choiceTag(INSERTION_RUBY, STATE_RUBY);
}

/* ruby tag after another one (remembers that one was open) */
/* 025FB3D0 */
s32 hdTextUnit_c::tagRuby2() {
    const u32 self = gabi::ea(this);
    const u32 m = msg(self);
    messageAt(m).mRubyState = u32(messageAt(m).mCurrentRuby) != 0 ? 1u : 0u;
    messageAt(msg(self)).mCurrentRuby = INSERTION_RUBY;
    messageAt(msg(self)).mState = STATE_RUBY;
    return 0;
}

/* 025FB404 */
s32 hdTextUnit_c::tagCapital() {
    const u32 self = gabi::ea(this);
    messageAt(msg(self)).mFlag922 = 1;
    return 0;
}

static u16 rgbaHalf(u32 p) { return u16(u8(gabi::at<hdUnitColorPair_c>(p)->mFirst) | (u8(gabi::at<hdUnitColorPair_c>(p)->mSecond) << 8)); }

/* inserts the pane-style tag (two colours, two sizes in percent) */
/* 025FB418 */
s32 hdTextUnit_c::insertPaneTag(hdUnitBufferedString_c* i_string, s32 pos, u32 c1, u32 c2, u32 sx, u32 sy) {
    const u32 w = gabi::ea(i_string);

    Local<hdUnitWideFixed32_c> T;
    wfixed(T.a, kWFixed32Vt, 0x20);
    const u32 b = u32(stringAt(T.a).mBuffer);
    tagAt(b).mCode = hdUnitTag_c::BEGIN_TAG;
    tagAt(b).mGroup = 1;
    tagAt(b).mTag = 0xC;
    tagAt(b).mParamSize = 0xC;
    tagAt(b).mParameters[0] = rgbaHalf(c1);
    tagAt(b).mParameters[1] = rgbaHalf(c1 + 2);
    tagAt(b).mParameters[2] = rgbaHalf(c2);
    tagAt(b).mParameters[3] = rgbaHalf(c2 + 2);
    tagAt(b).mParameters[4] = load<u16>(sx);
    tagAt(b).mParameters[5] = load<u16>(sy);
    wcopyAt(w, pos, T.a, 10);
    return 10;
}

/* copies the style of the layout pane "T_TypeOfJob_00" (font, colours, size) */
/* 025FB5D4 */
s32 hdTextUnit_c::copyPaneStyle(hdUnitBufferedString_c* i_string, s32 pos, u32 holder) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);
    const u32 lay = u32(gabi::at<hdUnitLayoutDescription_c>(gabi::at<hdUnitLayoutHolder_c>(holder)->mDescription)->mLayout);
    s32 total = 0;
    if (lay) {
        staticStr(gabi::ea(&textUnitStatics().paneNameGuard), gabi::ea(&textUnitStatics().paneName), gabi::guest_addr("T_TypeOfJob_00"), kSafeVt, staticRecordAddress(RECORD_paneName));
        const u32 vt = u32(gabi::at<hdUnitPane_c>(lay)->mVtable);
        vAssure(gabi::ea(&textUnitStatics().paneName));
        const u32 pane = call_ptr<u32>(u32(gabi::at<hdUnitPaneVtable_c>(vt)->mFindPane), lay, u32(textUnitStatics().paneName.mBuffer), 1u);
        if (!u32(paneTypeStatics().mTypeGuard)) {
            const u32 g = u32(paneTypeStatics().mGuard);
            paneTypeStatics().mTypeGuard = 1;
            if (!g) {
                paneTypeStatics().mGuard = 1;
                paneTypeStatics().mTypeEnd = 0;
            }
            paneTypeStatics().mType = gabi::ea(&paneTypeStatics().mTypeEnd);
        }
        bool ok = false;
        if (pane) {
            const u32 t0 = call_ptr<u32>(u32(gabi::at<hdUnitPaneVtable_c>(gabi::at<hdUnitPane_c>(pane)->mVtable)->mGetType), pane);
            for (u32 t = t0; t; t = load<u32>(t))
                if (t == gabi::ea(&paneTypeStatics().mType)) {
                    ok = true;
                    break;
                }
        }
        if (ok) {
            const u32 font = textUnitNative_getPaneFont_1(pane);
            u32 id = 0;
            for (u32 i = 0; i < 7; i++)
                if (textUnitNative_getFont_2(fontManagerAddress(), i) == font) {
                    id = i & 0xFFFF;
                    break;
                }
            const s32 n = insertFontTag(gabi::at<hdUnitBufferedString_c>(w), pos, id);
            Local<hdUnitPaneStyleFrame_c> L; /* +0/+2 size x/y, +4 colour 1, +8 colour 2 */
            store4(gabi::ea(&L.get()->mColor1), load4(gabi::ea(&gabi::at<hdUnitPane_c>(pane)->mColor1)));
            store4(gabi::ea(&L.get()->mColor2), load4(gabi::ea(&gabi::at<hdUnitPane_c>(pane)->mColor2)));
            const u32 f2 = textUnitNative_getPaneFont_1(pane);
            const u32 fnH = u32(gabi::at<hdUnitFontVtable_c>(gabi::at<hdUnitFont_c>(f2)->mVtable)->mGetWidth);
            const f32 k = f32(percentScale[0]);
            const f32 a = fmuls_ppc(f32(gabi::at<hdUnitPane_c>(pane)->mFontSizeX), k);
            const s32 h = s32(call_ptr<u32>(fnH, f2));
            L.get()->mSizeX = u16(ftoi(a / f32(f64(h))));
            const u32 fnW = u32(gabi::at<hdUnitFontVtable_c>(gabi::at<hdUnitFont_c>(f2)->mVtable)->mGetHeight);
            const f32 b = fmuls_ppc(f32(gabi::at<hdUnitPane_c>(pane)->mFontSizeY), k);
            const s32 wd = s32(call_ptr<u32>(fnW, f2));
            L.get()->mSizeY = u16(ftoi(b / f32(f64(wd))));
            total = n + insertPaneTag(gabi::at<hdUnitBufferedString_c>(w), pos + n, L.a + 4, L.a + 8, L.a, L.a + 2);
        }
    }
    messageAt(msg(self)).mFlag923 = 1;
    return total;
}

/* tag group 1 */
/* 025FB928 */
s32 hdTextUnit_c::group1(hdUnitBufferedString_c* i_string, s32 pos, u32 tag, u32 a7, u32 a8) {
    const u32 w = gabi::ea(i_string);
    const u32 self = gabi::ea(this);

    const u32 t = u16(tagAt(tag).mTag);
    if (t > 12 || t == 5) return 0;
    if (t == 12) return s32(group1Handlers[t](self, w, u32(pos), a8, a7, a8));
    return s32(group1Handlers[t](self, w, u32(pos), tag, a7, a8));
}

/* WWHD glue
data percentScale 100E10B4
str 100E10C8 "T_TypeOfJob_00"
ext 025FC444 u32 textUnitNative_putPlayerName_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FD05C u32 textUnitNative_putEventCount_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FD124 u32 textUnitNative_putRupeeCount_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FD800 u32 textUnitNative_putPlayMessage1_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FD86C u32 textUnitNative_putPlayMessage2_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FD8D8 u32 textUnitNative_putSwordGameDigits_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FDD80 u32 textUnitNative_putBlows_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FDEAC u32 textUnitNative_putEventMessage_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FDF28 u32 textUnitNative_putPlayCount2_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FE158 u32 textUnitNative_putLetters_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FE284 u32 textUnitNative_putKorokCount_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FE7F4 u32 textUnitNative_putSaveTimer_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FE858 u32 textUnitNative_putMeters_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FE960 u32 textUnitNative_putEvent86FF_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FEA7C u32 textUnitNative_putSavedPendants_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FEBA8 u32 textUnitNative_putPendantsEvent_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FECF4 u32 textUnitNative_putPlayTimer_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FED50 u32 textUnitNative_putSavedBombs_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FEE7C u32 textUnitNative_putSavedArrows_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FDF8C u32 textUnitNative_putEvent8AFF_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FE000 u32 textUnitNative_putPlayCount_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FE064 u32 textUnitNative_putPlayCountX10_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FEF7C u32 textUnitNative_putSeeds_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FF0A0 u32 textUnitNative_putNecklaces_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FF1C4 u32 textUnitNative_putChuchus_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FF2E8 u32 textUnitNative_putPendants_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FF40C u32 textUnitNative_putFeathers_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FF530 u32 textUnitNative_putCrests_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FF654 u32 textUnitNative_putStoredRupees_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FFB24 u32 textUnitNative_putSeedDigits_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FFBF8 u32 textUnitNative_putNecklaceDigits_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FFCCC u32 textUnitNative_putChuchuDigits_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FFDA0 u32 textUnitNative_putPendantDigits_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FFE74 u32 textUnitNative_putFeatherDigits_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FFF48 u32 textUnitNative_putCrestDigits_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FB148 u32 textUnitNative_tagNoEndSe_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FB15C u32 textUnitNative_tagEndSe_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FB1BC u32 textUnitNative_tagWait_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FB210 u32 textUnitNative_tagWaitAuto_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FB2C8 u32 textUnitNative_tagWaitInput_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FB31C u32 textUnitNative_tagSpeed_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FB370 u32 textUnitNative_tagChoice2_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FB390 u32 textUnitNative_tagChoice3_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FB3B0 u32 textUnitNative_tagRuby_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FB3D0 u32 textUnitNative_tagRuby2_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FB404 u32 textUnitNative_tagCapital_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FB5D4 u32 textUnitNative_copyPaneStyle_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 028F026C void textUnitNative_registerGlobalObject_1(u32 arg0)
ext 028EAB2C void textUnitNative_constructByte_1(u32 arg0)
ext 028ED6F8 void textUnitNative_constructFlag_1(u32 arg0)
ext 025FAA1C u32 textUnitNative_onTextEnd_2(u32 arg0, u32 arg1)
ext 025FA938 void textUnitNative_playEndSe_1(u32 arg0)
ext 025FA9E8 void textUnitNative_setEndState_1(u32 arg0)
ext 025FBA64 void textUnitNative_cutLines_2(u32 arg0, u32 arg1)
ext 025F72A0 void textUnitNative_recordLineEnd_2(u32 arg0, u32 arg1)
ext 025FC1F4 u32 textUnitNative_putChar_4(u32 arg0, u32 arg1, u32 arg2, u32 arg3)
ext 025FBFE8 u32 textUnitNative_newLine_4(u32 arg0, u32 arg1, u32 arg2, u32 arg3)
ext 025FBE54 u32 textUnitNative_boxFull_4(u32 arg0, u32 arg1, u32 arg2, u32 arg3)
ext 026005F8 u32 textUnitNative_tagDispatch_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025F724C void textUnitNative_advancePictograph_2(u32 arg0, u32 arg1)
ext 025F90C8 u32 textUnitNative_convertPictograph_2(u32 arg0, u32 arg1)
ext 025FB928 u32 textUnitNative_group1_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FB10C u32 textUnitNative_group0_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025200D4 u32 textUnitNative_getPlayInfo_0()
ext 025FB18C u32 textUnitNative_canStop_1(u32 arg0)
ext 025E1C3C void textUnitNative_playMessageSound_3(u32 arg0, u32 arg1, u32 arg2)
ext 02520540 u32 textUnitNative_getRoomReverb_1(u32 arg0)
ext 0260026C u32 textUnitNative_putValueTag_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FC708 u32 textUnitNative_putNumber_7(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5, u32 arg6)
ext 025FCDE0 u32 textUnitNative_leadingZeros_2(u32 arg0, u32 arg1)
ext 02601ADC void textUnitNative_assureWideString_1(u32 arg0)
ext 0273A2E8 u32 textUnitNative_getLabelText_2(u32 arg0, u32 arg1)
ext 0273A1CC u32 textUnitNative_findLabel_2(u32 arg0, u32 arg1)
ext 025F4CFC u32 textUnitNative_getUnitSet_1(u32 arg0)
ext 0275BAA0 u32 textUnitNative_convertToWide_4(u32 arg0, u32 arg1, u32 arg2, u32 arg3)
ext C0009988 void textUnitNative_moveBytes_4(u32 arg0, u32 arg1, u32 arg2, u32 arg3)
ext 02601AE0 void textUnitNative_assureWideBuffer_1(u32 arg0)
ext 02720170 u32 textUnitNative_getPlayerName_1(u32 arg0)
ext 025FC184 void textUnitNative_capitalize_2(u32 arg0, u32 arg1)
ext 025F7240 void textUnitNative_advanceCharacter_2(u32 arg0, u32 arg1)
ext 025FC154 u32 textUnitNative_mapChar_2(u32 arg0, u32 arg1)
ext 02759C28 void textUnitNative_formatString_3(u32 arg0, u32 arg1, u32 arg2)
ext 02759C28 void textUnitNative_formatString_2(u32 arg0, u32 arg1)
ext 025FB9BC u32 textUnitNative_onlyTagsBetween_4(u32 arg0, u32 arg1, u32 arg2, u32 arg3)
ext 025FF738 u32 textUnitNative_putDigitsLabel_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025B61C8 u32 textUnitNative_getTimer_1(u32 arg0)
ext 025F50E8 void textUnitNative_loadMessage_4(u32 arg0, u32 arg1, u32 arg2, u32 arg3)
ext 025F65F4 void textUnitNative_constructMessage_1(u32 arg0)
ext 02601AAC void textUnitNative_assureString_1(u32 arg0)
ext 025FC3D0 u32 textUnitNative_putCharacters_5(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4)
ext 02878098 u32 textUnitNative_getPaneFont_1(u32 arg0)
ext 025F36B8 u32 textUnitNative_getFont_2(u32 arg0, u32 arg1)
ext 025FAD04 u32 textUnitNative_scaleTag_4(u32 arg0, u32 arg1, u32 arg2, u32 arg3)
ext 025F4BC0 u32 textUnitNative_getProject_1(u32 arg0)
ext 0273AD10 u32 textUnitNative_allocate_1(u32 arg0)
ext 028F0164 void textUnitNative_destroyArray_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FD550 u32 textUnitNative_putMessage_5(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4)
ext 025FE38C u32 textUnitNative_putTimer_4(u32 arg0, u32 arg1, u32 arg2, u32 arg3)
ext 025FDD08 u32 textUnitNative_putNumberWithUnit_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025FCE5C u32 textUnitNative_putLocalizedUnit_4(u32 arg0, u32 arg1, u32 arg2, u32 arg3)
ext 025FCB90 u32 textUnitNative_putAsciiNumber_6(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
ext 025B8BB0 u32 textUnitNative_getEventRegister_2(u32 arg0, u32 arg1)
*/
